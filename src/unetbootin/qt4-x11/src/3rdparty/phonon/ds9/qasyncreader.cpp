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

#include <QtCore/QFile>

#include "qasyncreader.h"
#include "qbasefilter.h"

QT_BEGIN_NAMESPACE

namespace Phonon
{
    namespace DS9
    {
        QAsyncReader::QAsyncReader(QBaseFilter *parent, const QVector<AM_MEDIA_TYPE> &mediaTypes) : QPin(parent, PINDIR_OUTPUT, mediaTypes)
        {
        }

        QAsyncReader::~QAsyncReader()
        {
        }

        STDMETHODIMP QAsyncReader::QueryInterface(REFIID iid, void **out)
        {
            if (!out) {
                return E_POINTER;
            }

            if (iid == IID_IAsyncReader) {
                AddRef();
                *out = static_cast<IAsyncReader*>(this);
                return S_OK;
            }

            return QPin::QueryInterface(iid, out);
        }

        STDMETHODIMP_(ULONG) QAsyncReader::AddRef()
        {
            return QPin::AddRef();
        }

        STDMETHODIMP_(ULONG) QAsyncReader::Release()
        {
            return QPin::Release();
        }


        STDMETHODIMP QAsyncReader::RequestAllocator(IMemAllocator *preferred, ALLOCATOR_PROPERTIES *prop,IMemAllocator **actual)
        {
            ALLOCATOR_PROPERTIES prop2;

            if (prop->cbAlign == 0) {
                prop->cbAlign = 1; //align on 1 char
            }

            if (preferred && preferred->SetProperties(prop, &prop2) == S_OK) {
                preferred->AddRef();
                *actual = preferred;
                return S_OK;
            }

            //we should try to create one memory allocator ourselves here
            return E_FAIL;
        }

        STDMETHODIMP QAsyncReader::Request(IMediaSample *sample,DWORD_PTR user)
        {
            QMutexLocker mutexLocker(&m_mutexWait);
            QWriteLocker locker(&m_lock);
            if (m_flushing) {
                return VFW_E_WRONG_STATE;
            }

            m_requestQueue.enqueue(AsyncRequest(sample, user));
            m_requestWait.wakeOne();
            return S_OK;
        }

        STDMETHODIMP QAsyncReader::WaitForNext(DWORD timeout, IMediaSample **sample, DWORD_PTR *user)
        {
            QMutexLocker locker(&m_mutexWait);
            if (!sample ||!user) {
                return E_POINTER;
            }

            *sample = 0;
            *user = 0;

            AsyncRequest r = getNextRequest();

            if (r.sample == 0) {
                //there is no request in the queue
                if (isFlushing()) {
                    return VFW_E_WRONG_STATE;
                } else {
                    //First we need to lock the mutex
                    if (m_requestWait.wait(&m_mutexWait, timeout) == false) {
                        return VFW_E_TIMEOUT;
                    }
                    if (isFlushing()) {
                        return VFW_E_WRONG_STATE;
                    }

                    r = getNextRequest();
                }
            }

            //at this point we're sure to have a request to proceed
            if (r.sample == 0) {
                return E_FAIL;
            }

            *sample = r.sample;
            *user = r.user;

            return SyncReadAligned(r.sample);
        }

        STDMETHODIMP QAsyncReader::BeginFlush()
        {
            QMutexLocker mutexLocker(&m_mutexWait);
            QWriteLocker locker(&m_lock);
            m_flushing = true;
            m_requestWait.wakeOne();
            return S_OK;
        }

        STDMETHODIMP QAsyncReader::EndFlush()
        {
            QWriteLocker locker(&m_lock);
            m_flushing = false;
            return S_OK;
        }

        STDMETHODIMP QAsyncReader::SyncReadAligned(IMediaSample *sample)
        {
            if (!sample) {
                return E_POINTER;
            }

            REFERENCE_TIME start = 0,
                stop = 0;
            HRESULT hr = sample->GetTime(&start, &stop);
            if(FAILED(hr)) {
                return hr;
            }

            LONGLONG startPos = start / 10000000;
            LONG length = static_cast<LONG>((stop - start) / 10000000);

            BYTE *buffer;
            hr = sample->GetPointer(&buffer);
            if(FAILED(hr)) {
                return hr;
            }

            LONG actual = 0;
            read(startPos, length, buffer, &actual);

            return sample->SetActualDataLength(actual);
        }

        STDMETHODIMP QAsyncReader::SyncRead(LONGLONG pos, LONG length, BYTE *buffer)
        {
            return read(pos, length, buffer, 0);
        }


        //addition
        QAsyncReader::AsyncRequest QAsyncReader::getNextRequest()
        {
            QWriteLocker locker(&m_lock);
            AsyncRequest ret;
            if (!m_requestQueue.isEmpty()) {
                ret = m_requestQueue.dequeue();
            }

            return ret;
        }
    }
}

QT_END_NAMESPACE
