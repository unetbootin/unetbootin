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

#ifndef PHONON_COMPOINTER_H
#define PHONON_COMPOINTER_H

#include <windows.h>
#include <dshow.h>

QT_BEGIN_NAMESPACE

namespace Phonon
{
    namespace DS9
    {
        template<class T> class ComPointer
        {
        public:
            explicit ComPointer(T *t = 0) : m_t(t)
            {
            }

            explicit ComPointer( const IID &clsid, const IID &iid) : m_t(0)
            {
                ::CoCreateInstance(clsid, 0, CLSCTX_INPROC_SERVER, iid,
                    reinterpret_cast<void**>(&m_t));
            }

            explicit ComPointer(IUnknown *_unk, const GUID &guid) : m_t(0)
            {
                if (_unk) {
                    _unk->QueryInterface(guid, reinterpret_cast<void**>(&m_t));
                }
            }

            ComPointer(const ComPointer<T> &other) : m_t(other.m_t)
            {
                if (m_t) {
                    m_t->AddRef();
                }
            }

            ComPointer<T> &operator=(const ComPointer<T> &other)
            {
                if (other.m_t) {
                    other.m_t->AddRef();
                }
                if (m_t) { 
                    m_t->Release();
                }
                m_t = other.m_t;
                return *this;
            }

            T *operator->() const 
            {
                return m_t;
            }

            operator T*() const 
            {
                return m_t; 
            }

            //the following method first reinitialize their value to avoid mem leaks
            T ** pparam()
            {
                if (m_t) { 
                    m_t->Release();
                    m_t = 0;
                }
                return &m_t;
            }

			bool operator==(const ComPointer<T> &other) const
            {
                return m_t == other.m_t;
            }

            bool operator!=(const ComPointer<T> &other) const
            {
                return m_t != other.m_t;
            }

            ~ComPointer()
            {
                if (m_t) { 
                    m_t->Release();
                }
            }

        private:
            T *m_t;
        };
    }
}

QT_END_NAMESPACE

#endif
