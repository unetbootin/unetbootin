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

#ifndef QSHAREDDATA_H
#define QSHAREDDATA_H

#include <QtCore/qglobal.h>
#include <QtCore/qatomic.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Core)

template <class T> class QSharedDataPointer;

class Q_CORE_EXPORT QSharedData
{
public:
    mutable QAtomicInt ref;

    inline QSharedData() : ref(0) { }
    inline QSharedData(const QSharedData &) : ref(0) { }

private:
    // using the assignment operator would lead to corruption in the ref-counting
    QSharedData &operator=(const QSharedData &);
};

template <class T> class QSharedDataPointer
{
public:
    inline void detach() { if (d && d->ref != 1) detach_helper(); }
    inline T &operator*() { detach(); return *d; }
    inline const T &operator*() const { return *d; }
    inline T *operator->() { detach(); return d; }
    inline const T *operator->() const { return d; }
    inline operator T *() { detach(); return d; }
    inline operator const T *() const { return d; }
    inline T *data() { detach(); return d; }
    inline const T *data() const { return d; }
    inline const T *constData() const { return d; }

    inline bool operator==(const QSharedDataPointer<T> &other) const { return d == other.d; }
    inline bool operator!=(const QSharedDataPointer<T> &other) const { return d != other.d; }

    inline QSharedDataPointer() { d = 0; }
    inline ~QSharedDataPointer() { if (d && !d->ref.deref()) delete d; }

    explicit QSharedDataPointer(T *data);
    inline QSharedDataPointer(const QSharedDataPointer<T> &o) : d(o.d) { if (d) d->ref.ref(); }
    inline QSharedDataPointer<T> & operator=(const QSharedDataPointer<T> &o) {
        if (o.d != d) {
            if (o.d)
                o.d->ref.ref();
            if (d && !d->ref.deref())
                delete d;
            d = o.d;
        }
        return *this;
    }
    inline QSharedDataPointer &operator=(T *o) {
        if (o != d) {
            if (o)
                o->ref.ref();
            if (d && !d->ref.deref())
                delete d;
            d = o;
        }
        return *this;
    }

    inline bool operator!() const { return !d; }

private:
    void detach_helper();

    T *d;
};

template <class T> class QExplicitlySharedDataPointer
{
public:
    typedef T Type;

    inline T &operator*() { return *d; }
    inline const T &operator*() const { return *d; }
    inline T *operator->() { return d; }
    inline T *operator->() const { return d; }
    inline T *data() const { return d; }
    inline const T *constData() const { return d; }

    inline void detach() { if (d && d->ref != 1) detach_helper(); }

    inline void reset()
    {
        if(d && !d->ref.deref())
            delete d;

        d = 0;
    }

    inline operator bool () const { return d != 0; }

    inline bool operator==(const QExplicitlySharedDataPointer<T> &other) const { return d == other.d; }
    inline bool operator!=(const QExplicitlySharedDataPointer<T> &other) const { return d != other.d; }
    inline bool operator==(const T *ptr) const { return d == ptr; }
    inline bool operator!=(const T *ptr) const { return d != ptr; }

    inline QExplicitlySharedDataPointer() { d = 0; }
    inline ~QExplicitlySharedDataPointer() { if (d && !d->ref.deref()) delete d; }

    explicit QExplicitlySharedDataPointer(T *data);
    inline QExplicitlySharedDataPointer(const QExplicitlySharedDataPointer<T> &o) : d(o.d) { if (d) d->ref.ref(); }

#ifndef QT_NO_MEMBER_TEMPLATES
    template<class X>
    inline QExplicitlySharedDataPointer(const QExplicitlySharedDataPointer<X> &o) : d(static_cast<T *>(o.data()))
    {
        if(d)
            d->ref.ref();
    }
#endif

    inline QExplicitlySharedDataPointer<T> & operator=(const QExplicitlySharedDataPointer<T> &o) {
        if (o.d != d) {
            if (o.d)
                o.d->ref.ref();
            if (d && !d->ref.deref())
                delete d;
            d = o.d;
        }
        return *this;
    }
    inline QExplicitlySharedDataPointer &operator=(T *o) {
        if (o != d) {
            if (o)
                o->ref.ref();
            if (d && !d->ref.deref())
                delete d;
            d = o;
        }
        return *this;
    }

    inline bool operator!() const { return !d; }

private:
    void detach_helper();

    T *d;
};

template <class T>
Q_INLINE_TEMPLATE QSharedDataPointer<T>::QSharedDataPointer(T *adata) : d(adata)
{ if (d) d->ref.ref(); }

template <class T>
Q_OUTOFLINE_TEMPLATE void QSharedDataPointer<T>::detach_helper()
{
    T *x = new T(*d);
    x->ref.ref();
    if (!d->ref.deref())
        delete d;
    d = x;
}

template <class T>
Q_OUTOFLINE_TEMPLATE void QExplicitlySharedDataPointer<T>::detach_helper()
{
    T *x = new T(*d);
    x->ref.ref();
    if (!d->ref.deref())
        delete d;
    d = x;
}

template <class T>
Q_INLINE_TEMPLATE QExplicitlySharedDataPointer<T>::QExplicitlySharedDataPointer(T *adata) : d(adata)
{ if (d) d->ref.ref(); }

QT_END_NAMESPACE

QT_END_HEADER

#endif // QSHAREDDATA_H
