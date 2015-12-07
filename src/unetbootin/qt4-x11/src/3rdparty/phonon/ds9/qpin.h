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

#ifndef PHONON_QPIN_H
#define PHONON_QPIN_H

#include "phononds9_namespace.h"

#include <QtCore/QString>
#include <QtCore/QVector>
#include <QtCore/QReadWriteLock>

#include <dshow.h>

QT_BEGIN_NAMESPACE

namespace Phonon
{
    namespace DS9
    {
        class QBaseFilter;

        //this is the base class for our self-implemented Pins
        class QPin : public IPin
        {
        public:
            QPin(QBaseFilter *parent, PIN_DIRECTION dir, const QVector<AM_MEDIA_TYPE> &mt);
            virtual ~QPin();

            //reimplementation from IUnknown
            STDMETHODIMP QueryInterface(REFIID iid, void** out);
            STDMETHODIMP_(ULONG) AddRef();
            STDMETHODIMP_(ULONG) Release();

            //reimplementation from IPin
            STDMETHODIMP Connect(IPin *,const AM_MEDIA_TYPE *);
            STDMETHODIMP ReceiveConnection(IPin *,const AM_MEDIA_TYPE *);
            STDMETHODIMP Disconnect();
            STDMETHODIMP ConnectedTo(IPin **);
            STDMETHODIMP ConnectionMediaType(AM_MEDIA_TYPE *);
            STDMETHODIMP QueryPinInfo(PIN_INFO *);
            STDMETHODIMP QueryDirection(PIN_DIRECTION *);
            STDMETHODIMP QueryId(LPWSTR*);
            STDMETHODIMP QueryAccept(const AM_MEDIA_TYPE*);
            STDMETHODIMP EnumMediaTypes(IEnumMediaTypes **);
            STDMETHODIMP QueryInternalConnections(IPin **, ULONG*);
            STDMETHODIMP EndOfStream();
            STDMETHODIMP BeginFlush();
            STDMETHODIMP EndFlush();
            STDMETHODIMP NewSegment(REFERENCE_TIME, REFERENCE_TIME, double);

            QVector<AM_MEDIA_TYPE> mediaTypes() const;

            HRESULT setAcceptedMediaType(const AM_MEDIA_TYPE &);

            bool isFlushing() const;
            void setConnectedType(const AM_MEDIA_TYPE &type);
            const AM_MEDIA_TYPE &connectedType() const;
            void setConnected(IPin *pin);
            IPin *connected(bool = false) const;
            void setMemoryAllocator(IMemAllocator *alloc);
            IMemAllocator *memoryAllocator(bool = false) const;
            void createDefaultMemoryAllocator(ALLOCATOR_PROPERTIES * = 0);
            PIN_DIRECTION direction() const;

            FILTER_STATE filterState() const;

            static AM_MEDIA_TYPE copyMediaType(const AM_MEDIA_TYPE &type);
            static void freeMediaType(AM_MEDIA_TYPE *type);
            static void freeMediaType(const AM_MEDIA_TYPE &type);

        protected:
            //this can be used by sub-classes
            mutable QReadWriteLock m_lock;
            QBaseFilter *m_parent;
            bool m_flushing;

        private:
            HRESULT checkOutputMediaTypesConnection(IPin *pin);
            HRESULT checkOwnMediaTypesConnection(IPin *pin);

            LONG m_refCount;
            IPin *m_connected;
            const PIN_DIRECTION m_direction;
            QVector<AM_MEDIA_TYPE> m_mediaTypes; //accepted media types
            AM_MEDIA_TYPE m_connectedType;
            QString m_name;
            IMemAllocator *m_memAlloc;
        };

        //utility function
        class QAMMediaType : public AM_MEDIA_TYPE
        {
        public:
            ~QAMMediaType() 
            {
                QPin::freeMediaType(*this);
            }

        };

    }
}

QT_END_NAMESPACE

#endif //PHONON_QPIN_H
