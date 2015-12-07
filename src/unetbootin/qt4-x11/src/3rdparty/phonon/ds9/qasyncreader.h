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

#ifndef PHONON_QASYNCREADER_H
#define PHONON_QASYNCREADER_H

#include <QtCore/QWaitCondition>
#include <QtCore/QQueue>
#include <QtCore/QMutex>

#include "qpin.h"

QT_BEGIN_NAMESPACE

namespace Phonon
{
    namespace DS9
    {
        //his class reads asynchronously from a QIODevice
        class QAsyncReader : public QPin, public IAsyncReader
        {
        public:
            QAsyncReader(QBaseFilter *, const QVector<AM_MEDIA_TYPE> &mediaTypes);
            ~QAsyncReader();

            //reimplementation from IUnknown
            STDMETHODIMP QueryInterface(REFIID iid, void** out);
            STDMETHODIMP_(ULONG) AddRef();
            STDMETHODIMP_(ULONG) Release();

            //reimplementation from IAsyncReader
            STDMETHODIMP RequestAllocator(IMemAllocator *,ALLOCATOR_PROPERTIES *,IMemAllocator **);
            STDMETHODIMP Request(IMediaSample *,DWORD_PTR);
            STDMETHODIMP WaitForNext(DWORD,IMediaSample **,DWORD_PTR *);
            STDMETHODIMP SyncReadAligned(IMediaSample *);
            STDMETHODIMP SyncRead(LONGLONG,LONG,BYTE *);
            virtual STDMETHODIMP Length(LONGLONG *,LONGLONG *) = 0;
            STDMETHODIMP BeginFlush();
            STDMETHODIMP EndFlush();

        protected:
            virtual HRESULT read(LONGLONG pos, LONG length, BYTE *buffer, LONG *actual) = 0;

        private:
            struct AsyncRequest
            {
                AsyncRequest(IMediaSample *s = 0, DWORD_PTR u = 0) : sample(s), user(u) {}
                IMediaSample *sample;
                DWORD_PTR user;
            };
            AsyncRequest getNextRequest();

            QMutex m_mutexWait;

            QQueue<AsyncRequest> m_requestQueue;
            QWaitCondition m_requestWait;
        };
    }
}

QT_END_NAMESPACE

#endif
