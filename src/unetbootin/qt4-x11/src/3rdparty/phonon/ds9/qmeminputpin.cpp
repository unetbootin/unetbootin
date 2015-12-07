/*  This file is part of the KDE project.

Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).

This library is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation, either version 2.1 or 3 of the License.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with this library.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "qmeminputpin.h"
#include "qbasefilter.h"
#include "compointer.h"

#include <QtCore/QDebug>

QT_BEGIN_NAMESPACE

namespace Phonon
{
    namespace DS9
    {

        QMemInputPin::QMemInputPin(QBaseFilter *parent, const QVector<AM_MEDIA_TYPE> &mt, bool transform) : 
            QPin(parent, PINDIR_INPUT, mt), m_shouldDuplicateSamples(true), m_transform(transform)
        {
        }

        QMemInputPin::~QMemInputPin()
        {
        }

        STDMETHODIMP QMemInputPin::QueryInterface(REFIID iid, void **out)
        {
            if (!out) {
                return E_POINTER;
            }

            if (iid == IID_IMemInputPin) {
                *out = static_cast<IMemInputPin*>(this);
                AddRef();
                return S_OK;
            } else {
                return QPin::QueryInterface(iid, out);
            }
        }

        STDMETHODIMP_(ULONG) QMemInputPin::AddRef()
        {
            return QPin::AddRef();
        }

        STDMETHODIMP_(ULONG) QMemInputPin::Release()
        {
            return QPin::Release();
        }

        STDMETHODIMP QMemInputPin::EndOfStream()
        {
            //this allows to serialize with Receive calls
            QMutexLocker locker(&m_mutexReceive);
            for(int i = 0; i < m_outputs.count(); ++i) {
                IPin *conn = m_outputs.at(i)->connected();
                if (conn) {
                    conn->EndOfStream();
                }
            }
            return S_OK;
        }

        STDMETHODIMP QMemInputPin::BeginFlush()
        {
            //pass downstream
            for(int i = 0; i < m_outputs.count(); ++i) {
                IPin *conn = m_outputs.at(i)->connected();
                if (conn) {
                    conn->BeginFlush();
                }
            }
            QWriteLocker locker(&m_lock);
            m_flushing = true;
            return S_OK;
        }

        STDMETHODIMP QMemInputPin::EndFlush()
        {
            //pass downstream
            for(int i = 0; i < m_outputs.count(); ++i) {
                IPin *conn = m_outputs.at(i)->connected();
                if (conn) {
                    conn->EndFlush();
                }
            }
            QWriteLocker locker(&m_lock);
            m_flushing = false;
            return S_OK;
        }

        STDMETHODIMP QMemInputPin::NewSegment(REFERENCE_TIME start, REFERENCE_TIME stop, double rate)
        {
            for(int i = 0; i < m_outputs.count(); ++i) {
                m_outputs.at(i)->NewSegment(start, stop, rate);
            }
            return S_OK;
        }

        //reimplementation to set the type for the output pin
        //no need to make a deep copy here
        STDMETHODIMP QMemInputPin::ReceiveConnection(IPin *pin ,const AM_MEDIA_TYPE *mt)
        {
            HRESULT hr = QPin::ReceiveConnection(pin, mt);
            if (hr == S_OK &&
                mt->majortype != MEDIATYPE_NULL &&
                mt->subtype != MEDIASUBTYPE_NULL &&
                mt->formattype != GUID_NULL) {
                    //we tell the output pins that they should connect with this type
                    for(int i = 0; i < m_outputs.count(); ++i) {
                        hr = m_outputs.at(i)->setAcceptedMediaType(connectedType());
                        if (FAILED(hr)) {
                            break;
                        }
                    }
            }
            return hr;
        }

        STDMETHODIMP QMemInputPin::GetAllocator(IMemAllocator **alloc)
        {
            if (!alloc) {
                return E_POINTER;
            }

            if (*alloc = memoryAllocator(true)) {
                return S_OK;
            }

            return VFW_E_NO_ALLOCATOR;
        }

        STDMETHODIMP QMemInputPin::NotifyAllocator(IMemAllocator *alloc, BOOL readonly)
        {
            if (!alloc) {
                return E_POINTER;
            }

            {
                QWriteLocker locker(&m_lock);
                m_shouldDuplicateSamples = m_transform && readonly;
            }

            setMemoryAllocator(alloc);

            for(int i = 0; i < m_outputs.count(); ++i) {
                IPin *pin = m_outputs.at(i)->connected();
                if (pin) {
                    ComPointer<IMemInputPin> input(pin, IID_IMemInputPin);
                    input->NotifyAllocator(alloc, m_shouldDuplicateSamples);
                }
            }

            return S_OK;
        }

        STDMETHODIMP QMemInputPin::GetAllocatorRequirements(ALLOCATOR_PROPERTIES *prop)
        {
            if (!prop) {
                return E_POINTER;
            }

            //we have no particular requirements
            return E_NOTIMPL;
        }

        STDMETHODIMP QMemInputPin::Receive(IMediaSample *sample)
        {
            QMutexLocker locker(&m_mutexReceive);
            if (!sample) {
                return E_POINTER;
            }

            if (filterState() == State_Stopped) {
                return VFW_E_WRONG_STATE;
            }

            if (isFlushing()) {
                return S_FALSE; //we are still flushing
            }

            if (!m_shouldDuplicateSamples) {
                //we do it just once
                HRESULT hr = m_parent->processSample(sample);
                if (!SUCCEEDED(hr)) {
                    return hr;
                }
            }

            for (int i = 0; i < m_outputs.count(); ++i) {
                QPin *current = m_outputs.at(i);
                IMediaSample *outSample = m_shouldDuplicateSamples ? 
                    duplicateSampleForOutput(sample, current->memoryAllocator())
                    : sample;

                if (m_shouldDuplicateSamples) {
                    m_parent->processSample(outSample);
                }

                IPin *pin = current->connected();
                if (pin) {
                    ComPointer<IMemInputPin> input(pin, IID_IMemInputPin);
                    if (input) {
                        input->Receive(outSample);
                    }
                }

                if (m_shouldDuplicateSamples) {
                    outSample->Release();
                }
            }
            return S_OK;
        }

        STDMETHODIMP QMemInputPin::ReceiveMultiple(IMediaSample **samples,long count,long *nbDone)
        {
            //no need to lock here: there is no access to member data
            if (!samples || !nbDone) {
                return E_POINTER;
            }

            *nbDone = 0; //initialization
            while( *nbDone != count) {
                HRESULT hr = Receive(samples[*nbDone]);
                if (FAILED(hr)) {
                    return hr;
                }
                (*nbDone)++;
            }

            return S_OK;
        }

        STDMETHODIMP QMemInputPin::ReceiveCanBlock()
        {
            //we test the output to see if they can block
            for(int i = 0; i < m_outputs.count(); ++i) {
                IPin *input = m_outputs.at(i)->connected();
                if (input) {
                    ComPointer<IMemInputPin> meminput(input, IID_IMemInputPin);
                    if (meminput && meminput->ReceiveCanBlock() != S_FALSE) {
                        return S_OK;
                    }
                }
            }
            return S_FALSE;
        }

        //addition
        //this should be used by the filter to tell it's input pins to which output they should route the samples

        void QMemInputPin::addOutput(QPin *output)
        {
            QWriteLocker locker(&m_lock);
            m_outputs += output;
        }

        void QMemInputPin::removeOutput(QPin *output)
        {
            QWriteLocker locker(&m_lock);
            m_outputs.removeOne(output);
        }

        QList<QPin*> QMemInputPin::outputs() const
        {
            QReadLocker locker(&m_lock);
            return m_outputs;
        }

        ALLOCATOR_PROPERTIES QMemInputPin::getDefaultAllocatorProperties() const
        {
            //those values reduce buffering a lot (good for the volume effect)
            ALLOCATOR_PROPERTIES prop = {4096, 1, 1, 0};
            return prop;
        }


        IMediaSample *QMemInputPin::duplicateSampleForOutput(IMediaSample *sample, IMemAllocator *alloc)
        {
            LONG length = sample->GetActualDataLength();

            HRESULT hr = alloc->Commit();
            if (hr == VFW_E_SIZENOTSET) {
                ALLOCATOR_PROPERTIES prop = getDefaultAllocatorProperties();
                prop.cbBuffer = qMax(prop.cbBuffer, length);
                ALLOCATOR_PROPERTIES actual;
                //we just try to set the properties...
                alloc->SetProperties(&prop, &actual);
                hr = alloc->Commit();
            }

            Q_ASSERT(SUCCEEDED(hr));

            IMediaSample *out;
            hr = alloc->GetBuffer(&out, 0, 0, AM_GBF_NOTASYNCPOINT);
            Q_ASSERT(SUCCEEDED(hr));

            //let's copy the sample
            {
                REFERENCE_TIME start, end;
                sample->GetTime(&start, &end);
                out->SetTime(&start, &end);
            }

            hr = out->SetActualDataLength(length);
            Q_ASSERT(SUCCEEDED(hr));
            hr = out->SetDiscontinuity(sample->IsDiscontinuity());
            Q_ASSERT(SUCCEEDED(hr));

            {
                LONGLONG start, end;
                hr = sample->GetMediaTime(&start, &end);
                if (hr != VFW_E_MEDIA_TIME_NOT_SET) {
                    hr = out->SetMediaTime(&start, &end);
                    Q_ASSERT(SUCCEEDED(hr));
                }
            }

            AM_MEDIA_TYPE *type = 0;
            hr = sample->GetMediaType(&type);
            Q_ASSERT(SUCCEEDED(hr));
            hr = out->SetMediaType(type);
            Q_ASSERT(SUCCEEDED(hr));

            hr = out->SetPreroll(sample->IsPreroll());
            Q_ASSERT(SUCCEEDED(hr));
            hr = out->SetSyncPoint(sample->IsSyncPoint());
            Q_ASSERT(SUCCEEDED(hr));

            BYTE *dest = 0, *src = 0;
            hr = out->GetPointer(&dest);
            Q_ASSERT(SUCCEEDED(hr));
            hr = sample->GetPointer(&src);
            Q_ASSERT(SUCCEEDED(hr));

            qMemCopy(dest, src, sample->GetActualDataLength());

            return out;
        }
    }
}

QT_END_NAMESPACE
