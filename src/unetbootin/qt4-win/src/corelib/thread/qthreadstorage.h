/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the QtCore module of the Qt Toolkit.
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

#ifndef QTHREADSTORAGE_H
#define QTHREADSTORAGE_H

#include <QtCore/qglobal.h>

#ifndef QT_NO_THREAD

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Core)

class Q_CORE_EXPORT QThreadStorageData
{
public:
    explicit QThreadStorageData(void (*func)(void *));
    ~QThreadStorageData();

    void** get() const;
    void** set(void* p);

    static void finish(void**);
    int id;
};

#if !defined(QT_MOC_CPP)
// MOC_SKIP_BEGIN

// pointer specialization
template <typename T>
inline
T *&qThreadStorage_localData(QThreadStorageData &d, T **)
{
    void **v = d.get();
    if (!v) v = d.set(0);
    return *(reinterpret_cast<T**>(v));
}

template <typename T>
inline
T *qThreadStorage_localData_const(const QThreadStorageData &d, T **)
{
    void **v = d.get();
    return v ? *(reinterpret_cast<T**>(v)) : 0;
}

template <typename T>
inline
void qThreadStorage_setLocalData(QThreadStorageData &d, T **t)
{ (void) d.set(*t); }

#ifndef QT_NO_PARTIAL_TEMPLATE_SPECIALIZATION

// value-based specialization
template <typename T>
inline
T &qThreadStorage_localData(QThreadStorageData &d, T *)
{
    void **v = d.get();
    if (!v) v = d.set(new T());
    return *(reinterpret_cast<T*>(*v));
}

template <typename T>
inline
T qThreadStorage_localData_const(const QThreadStorageData &d, T *)
{
    void **v = d.get();
    return v ? *(reinterpret_cast<T*>(*v)) : T();
}

template <typename T>
inline
void qThreadStorage_setLocalData(QThreadStorageData &d, T *t)
{ (void) d.set(new T(*t)); }

#endif // QT_NO_PARTIAL_TEMPLATE_SPECIALIZATION

// MOC_SKIP_END
#endif

template <class T>
class QThreadStorage
{
private:
    QThreadStorageData d;

    Q_DISABLE_COPY(QThreadStorage)

    static inline void deleteData(void *x)
    { delete static_cast<T>(x); }

public:
    inline QThreadStorage() : d(deleteData) { }
    inline ~QThreadStorage() { }

    inline bool hasLocalData() const
    { return d.get() != 0; }

    inline T& localData()
    { return qThreadStorage_localData(d, reinterpret_cast<T*>(0)); }
    inline T localData() const
    { return qThreadStorage_localData_const(d, reinterpret_cast<T*>(0)); }

    inline void setLocalData(T t)
    { qThreadStorage_setLocalData(d, &t); }
};

QT_END_NAMESPACE

QT_END_HEADER

#endif // QT_NO_THREAD

#endif // QTHREADSTORAGE_H
