/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the QtCore module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Commercial License Agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Nokia.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
** package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef Q_QDOC

#ifndef QSHAREDPOINTER_H
#error Do not include qsharedpointer_impl.h directly
#endif
#if 0
#pragma qt_sync_stop_processing
#endif

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Core)

// Macro QSHAREDPOINTER_VERIFY_AUTO_CAST
//  generates a compiler error if the following construct isn't valid:
//    T *ptr1;
//    X *ptr2 = ptr1;
//
#ifdef QT_NO_DEBUG
# define QSHAREDPOINTER_VERIFY_AUTO_CAST(T, X)          qt_noop()
#else

template<typename T> inline void qt_sharedpointer_cast_check(T *) { }
# define QSHAREDPOINTER_VERIFY_AUTO_CAST(T, X)          \
    qt_sharedpointer_cast_check<T>(static_cast<X *>(0))
#endif

//
// forward declarations
//
template <class T> class QWeakPointer;
template <class T> class QSharedPointer;

template <class X, class T>
QSharedPointer<X> qSharedPointerCast(const QSharedPointer<T> &ptr);
template <class X, class T>
QSharedPointer<X> qSharedPointerDynamicCast(const QSharedPointer<T> &ptr);
template <class X, class T>
QSharedPointer<X> qSharedPointerConstCast(const QSharedPointer<T> &ptr);

namespace QtSharedPointer {
    template <class T> class InternalRefCount;
    template <class T> class ExternalRefCount;

    template <class X, class T> QSharedPointer<X> qStrongRefFromWeakHelper(const QWeakPointer<T> &, X*);
    template <class X, class T> QSharedPointer<X> qSharedPointerCastHelper(const QSharedPointer<T> &src, X *);
    template <class X, class T> QSharedPointer<X> qSharedPointerConstCastHelper(const QSharedPointer<T> &src, X *);

    // used in debug mode to verify the reuse of pointers
    Q_CORE_EXPORT void internalSafetyCheckAdd(const volatile void *);
    Q_CORE_EXPORT void internalSafetyCheckRemove(const volatile void *);

    template <class T, typename Klass, typename RetVal>
    inline void executeDeleter(T *t, RetVal (Klass:: *memberDeleter)())
    { (t->*memberDeleter)(); }
    template <class T, typename Deleter>
    inline void executeDeleter(T *t, Deleter d)
    { d(t); }

    //
    // Depending on its template parameter, QSharedPointer derives from either
    // QtSharedPointer::InternalRefCount or from QtSharedPointer::ExternalRefCount.
    // Both of these classes derive from QtSharedPointer::Basic, which provides common
    // operations,
    //
    template <class T>
    class Basic
    {
        typedef T *Basic:: *RestrictedBool;
    public:
        typedef T Type;

        inline T *data() const { return value; }
        inline bool isNull() const { return !data(); }
        inline operator RestrictedBool() const { return isNull() ? 0 : &Basic::value; }
        inline bool operator !() const { return isNull(); }
        inline T &operator*() const { return *data(); }
        inline T *operator->() const { return data(); }

    protected:
        inline Basic() : value(0 * sizeof(T)) { }
        // ~Basic();

        inline void verifyReconstruction(const T *ptr)
        {
            Q_ASSERT_X(!ptr || value != ptr, "QSharedPointer",
                       "QSharedPointer violation: you cannot create two QSharedPointer objects "
                       "from the same pointer");

            // make use of the "ptr" variable in the no-op statement below
            // since this function is in a public header, we don't
            // want warnings on "unused variables" to show up everywhere
            ptr = 0;
        }

        inline void internalConstruct(T *ptr)
        {
#ifndef QT_NO_DEBUG
            if (ptr) internalSafetyCheckAdd(ptr);
#endif
            value = ptr;
        }
        inline void internalDestroy()
        {
#ifndef QT_NO_DEBUG
            if (value) internalSafetyCheckRemove(value);
#endif
        }

#if defined(Q_NO_TEMPLATE_FRIENDS)
    public:
#else
        template <class X> friend class QWeakPointer;
#endif

        Type *value;
    };

    struct ExternalRefCountData
    {
        QAtomicInt weakref;
        QAtomicInt strongref;

        inline ExternalRefCountData() : weakref(1), strongref(1) { }
        virtual inline ~ExternalRefCountData() { Q_ASSERT(!weakref); Q_ASSERT(!strongref); }

        virtual inline bool destroy() { return false; }
    };

    template <class T, typename Deleter>
    struct ExternalRefCountWithSpecializedDeleter: public ExternalRefCountData
    {
        T *ptr;
        Deleter deleter;

        inline ExternalRefCountWithSpecializedDeleter(T *p, Deleter d)
            : ptr(p), deleter(d)
        { }
        inline bool destroy() { executeDeleter(ptr, deleter); return true; }
    };

    template <class T>
    class ExternalRefCount: public Basic<T>
    {
        typedef ExternalRefCountData Data;
        typedef void (*DeleterFunction)(T *);
    protected:
        inline void ref() const { d->weakref.ref(); d->strongref.ref(); }
        inline bool deref()
        {
            if (!d->strongref.deref())
                this->internalDestroy();
            return d->weakref.deref();
        }

        inline void internalConstruct(T *ptr)
        {
            Basic<T>::internalConstruct(ptr);
            Q_ASSERT(!d);
            if (ptr)
                d = new Data;
        }

        template <typename Deleter>
        inline void internalConstruct(T *ptr, Deleter deleter)
        {
            Basic<T>::internalConstruct(ptr);
            Q_ASSERT(!d);
            if (ptr)
                d = new ExternalRefCountWithSpecializedDeleter<T, Deleter>(ptr, deleter);
        }

        inline ExternalRefCount() : d(0) { }
        inline ~ExternalRefCount() { if (d && !deref()) delete d; }
        inline ExternalRefCount(const ExternalRefCount<T> &other) : Basic<T>(other), d(other.d)
        { if (d) ref(); }

        template <class X>
        inline void internalCopy(const ExternalRefCount<X> &other)
        {
            internalSet(other.d, other.data());
        }

        inline void internalDestroy()
        {
            Basic<T>::internalDestroy();
            if (!d->destroy())
                delete this->value;
        }

    private:
#if defined(Q_NO_TEMPLATE_FRIENDS)
    public:
#else
        template <class X> friend class ExternalRefCount;
        template <class X> friend class QWeakPointer;
        template <class X, class Y> friend QSharedPointer<X> qSharedPointerCastHelper(const QSharedPointer<Y> &src, X *);
        template <class X, class Y> friend QSharedPointer<X> qSharedPointerConstCastHelper(const QSharedPointer<Y> &src, X *);
        template <class X, class Y> friend QSharedPointer<X> QtSharedPointer::qStrongRefFromWeakHelper(const QWeakPointer<Y> &src, X *);
#endif

        inline void internalSet(Data *o, T *actual)
        {
            if (d == o) return;
            if (o && !o->strongref)
                o = 0;
            if (o) {
                verifyReconstruction(actual);
                o->weakref.ref();
                o->strongref.ref();
            }
            if (d && !deref())
                delete d;
            d = o;
            this->value = d && d->strongref ? actual : 0;
        }

#if defined(QT_BUILD_INTERNAL)
    public:
#endif
        Data *d;

    private:
        template<class X> ExternalRefCount(const InternalRefCount<X> &);
    };
} // namespace QtSharedPointer

template <class T>
class QSharedPointer: public QtSharedPointer::ExternalRefCount<T>
{
    typedef typename QtSharedPointer::ExternalRefCount<T> BaseClass;
public:
    inline QSharedPointer() { }
    // inline ~QSharedPointer() { }

    inline explicit QSharedPointer(T *ptr) { internalConstruct(ptr); }

    template <typename Deleter>
    inline QSharedPointer(T *ptr, Deleter d) { internalConstruct(ptr, d); }

    inline QSharedPointer(const QSharedPointer<T> &other) : BaseClass(other) { }
    inline QSharedPointer<T> &operator=(const QSharedPointer<T> &other)
    {
        internalCopy(other);
        return *this;
    }

    inline QSharedPointer(const QWeakPointer<T> &other)
    { *this = QtSharedPointer::qStrongRefFromWeakHelper(other, static_cast<T*>(0)); }
    inline QSharedPointer<T> &operator=(const QWeakPointer<T> &other)
    { *this = QtSharedPointer::qStrongRefFromWeakHelper(other, static_cast<T*>(0)); return *this; }

    template <class X>
    inline QSharedPointer(const QSharedPointer<X> &other) { *this = other; }

    template <class X>
    inline QSharedPointer<T> &operator=(const QSharedPointer<X> &other)
    {
        QSHAREDPOINTER_VERIFY_AUTO_CAST(T, X); // if you get an error in this line, the cast is invalid
        internalCopy(other);
        return *this;
    }

    template <class X>
    inline QSharedPointer(const QWeakPointer<X> &other)
    { *this = QtSharedPointer::qStrongRefFromWeakHelper(other, static_cast<T *>(0)); }

    template <class X>
    inline QSharedPointer<T> &operator=(const QWeakPointer<X> &other)
    { *this = qStrongRefFromWeakHelper(other, static_cast<T *>(0)); return *this; }

    template <class X>
    QSharedPointer<X> staticCast() const
    {
        return qSharedPointerCast<X, T>(*this);
    }

    template <class X>
    QSharedPointer<X> dynamicCast() const
    {
        return qSharedPointerDynamicCast<X, T>(*this);
    }

    template <class X>
    QSharedPointer<X> constCast() const
    {
        return qSharedPointerConstCast<X, T>(*this);
    }

    inline void clear() { *this = QSharedPointer<T>(); }

    QWeakPointer<T> toWeakRef() const;
};

template <class T>
class QWeakPointer
{
    typedef T *QWeakPointer:: *RestrictedBool;
    typedef QtSharedPointer::ExternalRefCountData Data;

public:
    inline bool isNull() const { return d == 0 || d->strongref == 0 || value == 0; }
    inline operator RestrictedBool() const { return isNull() ? 0 : &QWeakPointer::value; }
    inline bool operator !() const { return isNull(); }

    inline QWeakPointer() : d(0), value(0) { }
    inline ~QWeakPointer() { if (d && !d->weakref.deref()) delete d; }
    inline QWeakPointer(const QWeakPointer<T> &o) : d(o.d), value(o.value)
    { if (d) d->weakref.ref(); }
    inline QWeakPointer<T> &operator=(const QWeakPointer<T> &o)
    {
        internalSet(o.d, o.value);
        return *this;
    }

    inline QWeakPointer(const QSharedPointer<T> &o) : d(o.d), value(o.data())
    { if (d) d->weakref.ref();}
    inline QWeakPointer<T> &operator=(const QSharedPointer<T> &o)
    {
        internalSet(o.d, o.value);
        return *this;
    }

    template <class X>
    inline QWeakPointer(const QWeakPointer<X> &o) : d(0), value(0)
    { *this = o; }

    template <class X>
    inline QWeakPointer<T> &operator=(const QWeakPointer<X> &o)
    {
        // conversion between X and T could require access to the virtual table
        // so force the operation to go through QSharedPointer
        *this = o.toStrongRef();
        return *this;
    }

    template <class X>
    inline bool operator==(const QWeakPointer<X> &o) const
    { return d == o.d && value == static_cast<const T *>(o.value); }

    template <class X>
    inline bool operator!=(const QWeakPointer<X> &o) const
    { return !(*this == o); }

    template <class X>
    inline QWeakPointer(const QSharedPointer<X> &o) : d(0), value(0)
    { *this = o; }

    template <class X>
    inline QWeakPointer<T> &operator=(const QSharedPointer<X> &o)
    {
        QSHAREDPOINTER_VERIFY_AUTO_CAST(T, X); // if you get an error in this line, the cast is invalid
        internalSet(o.d, o.data());
        return *this;
    }

    template <class X>
    inline bool operator==(const QSharedPointer<X> &o) const
    { return d == o.d && value == static_cast<const T *>(o.data()); }

    template <class X>
    inline bool operator!=(const QSharedPointer<X> &o) const
    { return !(*this == o); }

    inline void clear() { *this = QWeakPointer<T>(); }

    inline QSharedPointer<T> toStrongRef() const { return QSharedPointer<T>(*this); }

private:

#if defined(Q_NO_TEMPLATE_FRIENDS)
public:
#else
    template <class X, class Y> friend QSharedPointer<X> QtSharedPointer::qStrongRefFromWeakHelper(const QWeakPointer<Y> &src, X *);
#endif

    inline void internalSet(Data *o, T *actual)
    {
        if (d == o) return;
        if (o)
            o->weakref.ref();
        if (d && !d->weakref.deref())
            delete d;
        d = o;
        value = actual;
    }

    Data *d;
    T *value;
};

template <class T, class X>
bool operator==(const QSharedPointer<T> &ptr1, const QSharedPointer<X> &ptr2)
{
    return ptr1.data() == ptr2.data();
}
template <class T, class X>
bool operator!=(const QSharedPointer<T> &ptr1, const QSharedPointer<X> &ptr2)
{
    return ptr1.data() != ptr2.data();
}

template <class T, class X>
bool operator==(const QSharedPointer<T> &ptr1, const X *ptr2)
{
    return ptr1.data() == ptr2;
}
template <class T, class X>
bool operator==(const T *ptr1, const QSharedPointer<X> &ptr2)
{
    return ptr1 == ptr2.data();
}

template <class T, class X>
bool operator!=(const QSharedPointer<T> &ptr1, const X *ptr2)
{
    return !(ptr1 == ptr2);
}
template <class T, class X>
bool operator!=(const T *ptr1, const QSharedPointer<X> &ptr2)
{
    return !(ptr2 == ptr1);
}

template <class T, class X>
bool operator==(const QSharedPointer<T> &ptr1, const QWeakPointer<X> &ptr2)
{
    return ptr2 == ptr1;
}
template <class T, class X>
bool operator!=(const QSharedPointer<T> &ptr1, const QWeakPointer<X> &ptr2)
{
    return ptr2 != ptr1;
}

template <class T>
Q_INLINE_TEMPLATE QWeakPointer<T> QSharedPointer<T>::toWeakRef() const
{
    return QWeakPointer<T>(*this);
}

namespace QtSharedPointer {
// helper functions:
    template <class X, class T>
    Q_INLINE_TEMPLATE X *qVerifyStaticCast(T *src, X *)
    {
        return static_cast<X *>(src); // if you get an error in this line, the cast is invalid
    }
    template <class X, class T>
    Q_INLINE_TEMPLATE X *qVerifyDynamicCast(T *src, X *)
    {
        return dynamic_cast<X *>(src); // if you get an error in this line, the cast is invalid
    }
    template <class X, class T>
    Q_INLINE_TEMPLATE X *qVerifyConstCast(T *src, X *)
    {
        return const_cast<X *>(src); // if you get an error in this line, the cast is invalid
    }

    template <class X, class T>
    Q_INLINE_TEMPLATE QSharedPointer<X> qSharedPointerCastHelper(const QSharedPointer<T> &src, X *)
    {
        QSharedPointer<X> result;
        register T *ptr = src.data();
        result.internalSet(src.d, static_cast<X *>(ptr));
        return result;
    }
    template <class X, class T>
    Q_INLINE_TEMPLATE QSharedPointer<X> qSharedPointerConstCastHelper(const QSharedPointer<T> &src, X *)
    {
        QSharedPointer<X> result;
        register T *ptr = src.data();
        result.internalSet(src.d, const_cast<X *>(ptr));
        return result;
    }
    template <class X, class T>
    Q_INLINE_TEMPLATE QSharedPointer<X> qStrongRefFromWeakHelper
        (const QT_PREPEND_NAMESPACE(QWeakPointer<T>) &src, X *)
    {
        QSharedPointer<X> result;
        result.internalSet(src.d, src.value);
        return result;
    }
}

// cast operators
template <class X, class T>
Q_INLINE_TEMPLATE QSharedPointer<X> qSharedPointerCast(const QSharedPointer<T> &src)
{
    X *x = 0;
    QtSharedPointer::qVerifyStaticCast(src.data(), x);
    return QtSharedPointer::qSharedPointerCastHelper(src, x);
}
template <class X, class T>
Q_INLINE_TEMPLATE QSharedPointer<X> qSharedPointerCast(const QWeakPointer<T> &src)
{
    return qSharedPointerCast<X, T>(src.toStrongRef());
}

template <class X, class T>
Q_INLINE_TEMPLATE QSharedPointer<X> qSharedPointerDynamicCast(const QSharedPointer<T> &src)
{
    X *x = 0;
    if (QtSharedPointer::qVerifyDynamicCast(src.data(), x))
        return QtSharedPointer::qSharedPointerCastHelper(src, x);
    return QSharedPointer<X>();
}
template <class X, class T>
Q_INLINE_TEMPLATE QSharedPointer<X> qSharedPointerDynamicCast(const QWeakPointer<T> &src)
{
    return qSharedPointerDynamicCast<X, T>(src.toStrongRef());
}

template <class X, class T>
Q_INLINE_TEMPLATE QSharedPointer<X> qSharedPointerConstCast(const QSharedPointer<T> &src)
{
    X *x = 0;
    if (QtSharedPointer::qVerifyConstCast(src.data(), x))
        return QtSharedPointer::qSharedPointerConstCastHelper(src, x);
    return QSharedPointer<X>();
}
template <class X, class T>
Q_INLINE_TEMPLATE QSharedPointer<X> qSharedPointerConstCast(const QWeakPointer<T> &src)
{
    X *x = 0;
    if (QtSharedPointer::qVerifyConstCast(src.data(), x))
        return QtSharedPointer::qSharedPointerCastHelper(src, x);
    return QSharedPointer<X>();
}

template <class X, class T>
Q_INLINE_TEMPLATE
QWeakPointer<X> qWeakPointerCast(const QSharedPointer<T> &src)
{
    return qSharedPointerCast<X, T>(src).toWeakRef();
}

QT_END_NAMESPACE

QT_END_HEADER

#endif
