/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the QtScript module of the Qt Toolkit.
**
** This file may be used under the terms of the GNU General Public
** License versions 2.0 or 3.0 as published by the Free Software
** Foundation and appearing in the files LICENSE.GPL2 and LICENSE.GPL3
** included in the packaging of this file.  Alternatively you may (at
** your option) use any later version of the GNU General Public
** License if such license has been publicly approved by Trolltech ASA
** (or its successors, if any) and the KDE Free Qt Foundation. In
** addition, as a special exception, Trolltech gives you certain
** additional rights. These rights are described in the Trolltech GPL
** Exception version 1.2, which can be found at
** http://www.trolltech.com/products/qt/gplexception/ and in the file
** GPL_EXCEPTION.txt in this package.
**
** Please review the following information to ensure GNU General
** Public Licensing requirements will be met:
** http://trolltech.com/products/qt/licenses/licensing/opensource/. If
** you are unsure which license is appropriate for your use, please
** review the following information:
** http://trolltech.com/products/qt/licenses/licensing/licensingoverview
** or contact the sales department at sales@trolltech.com.
**
** In addition, as a special exception, Trolltech, as the sole
** copyright holder for Qt Designer, grants users of the Qt/Eclipse
** Integration plug-in the right for the Qt/Eclipse Integration to
** link to functionality provided by Qt Designer and its related
** libraries.
**
** This file is provided "AS IS" with NO WARRANTY OF ANY KIND,
** INCLUDING THE WARRANTIES OF DESIGN, MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE. Trolltech reserves all rights not expressly
** granted herein.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

#ifndef QSCRIPTBUFFER_P_H
#define QSCRIPTBUFFER_P_H

#include <QtCore/qglobal.h>

QT_BEGIN_NAMESPACE

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

namespace QScript
{
    template <typename T> class Buffer
    {
    public:
        typedef T *iterator;
        typedef const T *const_iterator;

        Buffer() : m_data(0), m_capacity(0), m_size(0) { }
        ~Buffer() { delete [] m_data; }

        inline void reserve(int num);
        inline void reset();
        inline void clear();
        inline void append(const T &t);

        inline iterator begin();
        inline iterator end();

        inline const_iterator begin() const;
        inline const_iterator end() const;

        inline int size() const;
        inline void resize(int s);

        inline int capacity() const;
        inline T *data();
        inline const T *constData() const;

        inline T &last();
        inline T &takeLast();

        inline T &at(int i) { return (*this)[i]; }
        inline const T &at(int i) const { return (*this)[i]; }

        inline T &operator[](int i);
        inline const T &operator[](int i) const;

        inline bool isEmpty() const;

    private:
        T *m_data;
        int m_capacity;
        int m_size;

    private:
        Q_DISABLE_COPY(Buffer)
    };

} // namespace QScript

template <typename T> T *QScript::Buffer<T>::data() { return m_data; }
template <typename T> const T *QScript::Buffer<T>::constData() const { return m_data; }
template <typename T> void QScript::Buffer<T>::reset() { m_size = 0; }
template <typename T> int QScript::Buffer<T>::capacity() const { return m_capacity; }

template <typename T> int QScript::Buffer<T>::size() const { return m_size; }
template <typename T> void QScript::Buffer<T>::resize(int s)
{
    if (m_capacity < s)
        reserve (s << 1);

    m_size = s;
}

template <typename T> void QScript::Buffer<T>::clear()
{
    delete [] m_data;
    m_data = 0;
    m_size = 0;
    m_capacity = 0;
}

template <typename T> void QScript::Buffer<T>::reserve(int x)
{
    // its an ever expanding buffer so it never gets smaller..
    if (x < m_capacity)
        return;
    m_capacity = x;
    T *new_data = new T[m_capacity];
    for (int i=0; i<m_size; ++i)
        new_data[i] = m_data[i];
    delete [] m_data;
    m_data = new_data;
}

template <typename T> void QScript::Buffer<T>::append(const T &t)
{
    if (m_size == m_capacity)
        reserve(m_capacity + 32);
    m_data[m_size++] = t;
}

template <typename T> T &QScript::Buffer<T>::operator[](int i)
{
    Q_ASSERT(i >= 0);
    Q_ASSERT(i < m_size);
    return m_data[i];
}

template <typename T> const T &QScript::Buffer<T>::operator[](int i) const
{
    Q_ASSERT(i >= 0);
    Q_ASSERT(i < m_size);
    return m_data[i];
}

template <typename T> bool QScript::Buffer<T>::isEmpty() const
{
    return m_size == 0;
}

template <typename T> T &QScript::Buffer<T>::takeLast()
{
    Q_ASSERT(!isEmpty());
    --m_size;
    return m_data[m_size];
}

template <typename T> T &QScript::Buffer<T>::last()
{
    return m_data[m_size - 1];
}

template <typename T> typename QScript::Buffer<T>::iterator QScript::Buffer<T>::begin()
{
    return m_data;
}

template <typename T> typename QScript::Buffer<T>::iterator QScript::Buffer<T>::end()
{
    return m_data + m_size;
}

template <typename T> typename QScript::Buffer<T>::const_iterator QScript::Buffer<T>::begin() const
{
    return m_data;
}

template <typename T> typename QScript::Buffer<T>::const_iterator QScript::Buffer<T>::end() const
{
    return m_data + m_size;
}

QT_END_NAMESPACE

#endif
