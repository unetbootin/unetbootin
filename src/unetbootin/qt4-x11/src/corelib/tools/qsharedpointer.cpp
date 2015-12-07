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

#include "qsharedpointer.h"

// to be sure we aren't causing a namespace clash:
#include "qshareddata.h"

/*!
    \class QSharedPointer
    \brief The QSharedPointer class holds a strong reference to a shared pointer
    \since 4.5

    \reentrant
    \ingroup misc

    The QSharedPointer is an automatic, shared pointer in C++. It
    behaves exactly like a normal pointer for normal purposes,
    including respect for constness.

    QSharedPointer will delete the pointer it is holding when it goes
    out of scope, provided no other QSharedPointer objects are
    referencing it.

    A QSharedPointer object can be created from a normal pointer,
    another QSharedPointer object or by promoting a
    QWeakPointer object to a strong reference.

    \section1 Thread-Safety

    QSharedPointer and QWeakPointer are thread-safe and operate
    atomically on the pointer value. Different threads can also access
    the same QSharedPointer or QWeakPointer object at the same time
    without need for locking mechanisms.

    It should be noted that, while the pointer value can be accessed
    in this manner, QSharedPointer and QWeakPointer provide no
    guarantee about the object being pointed to. Thread-safety and
    reentrancy rules for that object still apply.

    \section1 Other Pointer Classes

    Qt also provides two other pointer wrapper classes: QPointer and
    QSharedDataPointer. They are incompatible with one another, since
    each has its very different use case.

    QSharedPointer holds a shared pointer by means of an external
    reference count (i.e., a reference counter placed outside the
    object). Like its name indicates, the pointer value is shared
    among all instances of QSharedPointer and QWeakPointer. The
    contents of the object pointed to by the pointer should not
    considered shared, however: there is only one object. For that
    reason, QSharedPointer does not provide a way to detach or make
    copies of the pointed object.

    QSharedDataPointer, on the other hand, holds a pointer to shared
    data (i.e., a class derived from QSharedData). It does so by means
    of an internal reference count, placed in the QSharedData base
    class. This class can, therefore, detach based on the type of
    access made to the data being guarded: if it's a non-const access,
    it creates a copy atomically for the operation to complete.

    QExplicitlySharedDataPointer behaves like QSharedDataPointer,
    except that it only detaches if
    QExplicitlySharedDataPointer::detach() is explicitly called.

    Finally, QPointer holds a pointer to a QObject-derived object, but
    it does so weakly. QPointer is similar, in that behaviour, to
    QWeakPointer: it does not allow you to prevent the object from
    being destroyed. All you can do is query whether it has been
    destroyed or not.

    \sa QSharedDataPointer, QWeakPointer
*/

/*!
    \class QWeakPointer
    \brief The QWeakPointer class holds a weak reference to a shared pointer
    \since 4.5
    \reentrant
    \ingroup misc

    The QWeakPointer is an automatic weak reference to a
    pointer in C++. It cannot be used to dereference the pointer
    directly, but it can be used to verify if the pointer has been
    deleted or not in another context.

    QWeakPointer objects can only be created by assignment
    from a QSharedPointer.

    To access the pointer that QWeakPointer is tracking, you
    must first create a QSharedPointer object and verify if the pointer
    is null or not.

    \sa QSharedPointer
*/

/*!
    \fn QSharedPointer::QSharedPointer()

    Creates a QSharedPointer that points to null (0).
*/

/*!
    \fn QSharedPointer::~QSharedPointer()

    Destroys this QSharedPointer object. If it is the last reference to
    the pointer stored, this will delete the pointer as well.
*/

/*!
    \fn QSharedPointer::QSharedPointer(T *ptr)

    Creates a QSharedPointer that points to \a ptr. The pointer \a ptr
    becomes managed by this QSharedPointer and must not be passed to
    another QSharedPointer object or deleted outside this object.
*/

/*!
    \fn QSharedPointer::QSharedPointer(T *ptr, Deleter deleter)

    Creates a QSharedPointer that points to \a ptr. The pointer \a ptr
    becomes managed by this QSharedPointer and must not be passed to
    another QSharedPointer object or deleted outside this object.

    The \a deleter paramter specifies the custom deleter for this
    object. The custom deleter is called when the strong reference
    count drops to 0 instead of the operator delete(). This is useful,
    for instance, for calling deleteLater() in a QObject instead:

    \code
    static void doDeleteLater(MyObject *obj)
    {
        obj->deleteLater();
    }

    void otherFunction()
    {
        QSharedPointer<MyObject> obj =
            QSharedPointer<MyObject>(new MyObject, doDeleteLater);

        // continue using obj
        obj.clear();    // calls obj->deleteLater();
    }
    \endcode

    It is also possible to specify a member function directly, as in:
    \code
        QSharedPointer<MyObject> obj =
            QSharedPointer<MyObject>(new MyObject, &QObject::deleteLater);
    \endcode

    \sa clear()
*/

/*!
    \fn QSharedPointer::QSharedPointer(const QSharedPointer<T> &other)

    Creates a QSharedPointer object that shares \a other's pointer.

    If \tt T is a derived type of the template parameter of this class,
    QSharedPointer will perform an automatic cast. Otherwise, you will
    get a compiler error.
*/

/*!
    \fn QSharedPointer::QSharedPointer(const QWeakPointer<T> &other)

    Creates a QSharedPointer by promoting the weak reference \a other
    to strong reference and sharing its pointer.

    If \tt T is a derived type of the template parameter of this
    class, QSharedPointer will perform an automatic cast. Otherwise,
    you will get a compiler error.
*/

/*!
    \fn QSharedPointer &QSharedPointer::operator=(const QSharedPointer<T> &other)

    Makes this object share \a other's pointer. The current pointer
    reference is discarded and, if it was the last, the pointer will
    be deleted.

    If \tt T is a derived type of the template parameter of this
    class, QSharedPointer will perform an automatic cast. Otherwise,
    you will get a compiler error.
*/

/*!
    \fn QSharedPointer &QSharedPointer::operator=(const QWeakPointer<T> &other)

    Promotes \a other to a strong reference and makes this object
    share a reference to the pointer referenced by it. The current pointer
    reference is discarded and, if it was the last, the pointer will
    be deleted.

    If \tt T is a derived type of the template parameter of this
    class, QSharedPointer will perform an automatic cast. Otherwise,
    you will get a compiler error.
*/

/*!
    \fn T *QSharedPointer::data() const

    Returns the value of the pointer referenced by this object.

    Note: do not delete the pointer returned by this function or pass
    it to another function that could delete it, including creating
    QSharedPointer or QWeakPointer objects.
*/

/*!
    \fn T &QSharedPointer::operator *() const

    Provides access to the shared pointer's members.

    \sa isNull()
*/

/*!
    \fn T *QSharedPointer::operator ->() const

    Provides access to the shared pointer's members.

    \sa isNull()
*/

/*!
    \fn bool QSharedPointer::isNull() const

    Returns true if this object is holding a reference to a null
    pointer.
*/

/*!
    \fn QSharedPointer::operator bool() const

    Returns true if this object is not null. This function is suitable
    for use in \tt if-constructs, like:

    \code
        if (sharedptr) { ... }
    \endcode

    \sa isNull()
*/

/*!
    \fn bool QSharedPointer::operator !() const

    Returns true if this object is null. This function is suitable
    for use in \tt if-constructs, like:

    \code
        if (!sharedptr) { ... }
    \endcode

    \sa isNull()
*/

/*!
    \fn QSharedPointer<X> QSharedPointer::staticCast() const

    Performs a static cast from this pointer's type to \tt X and returns
    a QSharedPointer that shares the reference. This function can be
    used for up- and for down-casting, but is more useful for
    up-casting.

    Note: the template type \c X must have the same const and volatile
    qualifiers as the template of this object, or the cast will
    fail. Use constCast() if you need to drop those qualifiers.

    \sa dynamicCast(), constCast(), qSharedPointerCast()
*/

/*!
    \fn QSharedPointer<X> QSharedPointer::dynamicCast() const

    Performs a dynamic cast from this pointer's type to \tt X and
    returns a QSharedPointer that shares the reference. If this
    function is used to up-cast, then QSharedPointer will perform a \tt
    dynamic_cast, which means that if the object being pointed by this
    QSharedPointer is not of type \tt X, the returned object will be
    null.

    Note: the template type \c X must have the same const and volatile
    qualifiers as the template of this object, or the cast will
    fail. Use constCast() if you need to drop those qualifiers.

    \sa qSharedPointerDynamicCast()
*/

/*!
    \fn QSharedPointer<X> QSharedPointer::constCast() const

    Performs a \tt const_cast from this pointer's type to \tt X and returns
    a QSharedPointer that shares the reference. This function can be
    used for up- and for down-casting, but is more useful for
    up-casting.

    \sa isNull(), qSharedPointerConstCast()
*/

/*!
    \fn QWeakPointer<T> QSharedPointer::toWeakRef() const

    Returns a weak reference object that shares the pointer referenced
    by this object.
*/

/*!
    \fn void QSharedPointer::clear()

    Clears this QSharedPointer object, dropping the reference that it
    may have had to the pointer. If this was the last reference, then
    the pointer itself will be deleted.
*/

/*!
    \fn QWeakPointer::QWeakPointer()

    Creates a QWeakPointer that points to nothing.
*/

/*!
    \fn QWeakPointer::~QWeakPointer()

    Destroys this QWeakPointer object. The pointer referenced
    by this object will not be deleted.
*/

/*!
    \fn QWeakPointer::QWeakPointer(const QWeakPointer<T> &other)

    Creates a QWeakPointer that holds a weak reference to the
    pointer referenced by \a other.

    If \tt T is a derived type of the template parameter of this
    class, QWeakPointer will perform an automatic cast. Otherwise,
    you will get a compiler error.
*/

/*!
    \fn QWeakPointer::QWeakPointer(const QSharedPointer<T> &other)

    Creates a QWeakPointer that holds a weak reference to the
    pointer referenced by \a other.

    If \tt T is a derived type of the template parameter of this
    class, QWeakPointer will perform an automatic cast. Otherwise,
    you will get a compiler error.
*/

/*!
    \fn QWeakPointer &QWeakPointer::operator=(const QWeakPointer<T> &other)

    Makes this object share \a other's pointer. The current pointer
    reference is discarded but is not deleted.

    If \tt T is a derived type of the template parameter of this
    class, QWeakPointer will perform an automatic cast. Otherwise,
    you will get a compiler error.
*/

/*!
    \fn QWeakPointer &QWeakPointer::operator=(const QSharedPointer<T> &other)

    Makes this object share \a other's pointer. The current pointer
    reference is discarded but is not deleted.

    If \tt T is a derived type of the template parameter of this
    class, QWeakPointer will perform an automatic cast. Otherwise,
    you will get a compiler error.
*/

/*!
    \fn bool QWeakPointer::isNull() const

    Returns true if this object is holding a reference to a null
    pointer.

    Note that, due to the nature of weak references, the pointer that
    QWeakPointer references can become null at any moment, so
    the value returned from this function can change from false to
    true from one call to the next.
*/

/*!
    \fn QWeakPointer::operator bool() const

    Returns true if this object is not null. This function is suitable
    for use in \tt if-constructs, like:

    \code
        if (weakref) { ... }
    \endcode

    Note that, due to the nature of weak references, the pointer that
    QWeakPointer references can become null at any moment, so
    the value returned from this function can change from true to
    false from one call to the next.

    \sa isNull()
*/

/*!
    \fn bool QWeakPointer::operator !() const

    Returns true if this object is null. This function is suitable
    for use in \tt if-constructs, like:

    \code
        if (!weakref) { ... }
    \endcode

    Note that, due to the nature of weak references, the pointer that
    QWeakPointer references can become null at any moment, so
    the value returned from this function can change from false to
    true from one call to the next.

    \sa isNull()
*/

/*!
    \fn QSharedPointer<T> QWeakPointer::toStrongRef() const

    Promotes this weak reference to a strong one and returns a
    QSharedPointer object holding that reference.
*/

/*!
    \fn void QWeakPointer::clear()

    Clears this QWeakPointer object, dropping the reference that it
    may have had to the pointer.
*/

/*!
    \fn bool operator==(const QSharedPointer<T> &ptr1, const QSharedPointer<X> &ptr2)
    \relates QSharedPointer

    Returns true if the pointer referenced by \a ptr1 is the
    same pointer as that referenced by \a ptr2.

    If \a ptr2's template parameter is different from \a ptr1's,
    QSharedPointer will attempt to perform an automatic \tt static_cast
    to ensure that the pointers being compared are equal. If \a ptr2's
    template parameter is not a base or a derived type from
    \a ptr1's, you will get a compiler error.
*/

/*!
    \fn bool operator!=(const QSharedPointer<T> &ptr1, const QSharedPointer<X> &ptr2)
    \relates QSharedPointer

    Returns true if the pointer referenced by \a ptr1 is not the
    same pointer as that referenced by \a ptr2.

    If \a ptr2's template parameter is different from \a ptr1's,
    QSharedPointer will attempt to perform an automatic \tt static_cast
    to ensure that the pointers being compared are equal. If \a ptr2's
    template parameter is not a base or a derived type from
    \a ptr1's, you will get a compiler error.
*/

/*!
    \fn bool operator==(const QSharedPointer<T> &ptr1, const X *ptr2)
    \relates QSharedPointer

    Returns true if the pointer referenced by \a ptr1 is the
    same pointer as \a ptr2.

    If \a ptr2's type is different from \a ptr1's,
    QSharedPointer will attempt to perform an automatic \tt static_cast
    to ensure that the pointers being compared are equal. If \a ptr2's
    type is not a base or a derived type from this
    \a ptr1's, you will get a compiler error.
*/

/*!
    \fn bool operator!=(const QSharedPointer<T> &ptr1, const X *ptr2)
    \relates QSharedPointer

    Returns true if the pointer referenced by \a ptr1 is not the
    same pointer as \a ptr2.

    If \a ptr2's type is different from \a ptr1's,
    QSharedPointer will attempt to perform an automatic \tt static_cast
    to ensure that the pointers being compared are equal. If \a ptr2's
    type is not a base or a derived type from this
    \a ptr1's, you will get a compiler error.
*/

/*!
    \fn bool operator==(const T *ptr1, const QSharedPointer<X> &ptr2)
    \relates QSharedPointer

    Returns true if the pointer \a ptr1 is the
    same pointer as that referenced by \a ptr2.

    If \a ptr2's template parameter is different from \a ptr1's type,
    QSharedPointer will attempt to perform an automatic \tt static_cast
    to ensure that the pointers being compared are equal. If \a ptr2's
    template parameter is not a base or a derived type from
    \a ptr1's type, you will get a compiler error.
*/

/*!
    \fn bool operator!=(const T *ptr1, const QSharedPointer<X> &ptr2)
    \relates QSharedPointer

    Returns true if the pointer \a ptr1 is not the
    same pointer as that referenced by \a ptr2.

    If \a ptr2's template parameter is different from \a ptr1's type,
    QSharedPointer will attempt to perform an automatic \tt static_cast
    to ensure that the pointers being compared are equal. If \a ptr2's
    template parameter is not a base or a derived type from
    \a ptr1's type, you will get a compiler error.
*/

/*!
    \fn bool operator==(const QSharedPointer<T> &ptr1, const QWeakPointer<X> &ptr2)
    \relates QWeakPointer

    Returns true if the pointer referenced by \a ptr1 is the
    same pointer as that referenced by \a ptr2.

    If \a ptr2's template parameter is different from \a ptr1's,
    QSharedPointer will attempt to perform an automatic \tt static_cast
    to ensure that the pointers being compared are equal. If \a ptr2's
    template parameter is not a base or a derived type from
    \a ptr1's, you will get a compiler error.
*/

/*!
    \fn bool operator!=(const QSharedPointer<T> &ptr1, const QWeakPointer<X> &ptr2)
    \relates QWeakPointer

    Returns true if the pointer referenced by \a ptr1 is not the
    same pointer as that referenced by \a ptr2.

    If \a ptr2's template parameter is different from \a ptr1's,
    QSharedPointer will attempt to perform an automatic \tt static_cast
    to ensure that the pointers being compared are equal. If \a ptr2's
    template parameter is not a base or a derived type from
    \a ptr1's, you will get a compiler error.
*/

/*!
    \fn bool operator==(const QWeakPointer<T> &ptr1, const QSharedPointer<X> &ptr2)
    \relates QWeakPointer

    Returns true if the pointer referenced by \a ptr1 is the
    same pointer as that referenced by \a ptr2.

    If \a ptr2's template parameter is different from \a ptr1's,
    QSharedPointer will attempt to perform an automatic \tt static_cast
    to ensure that the pointers being compared are equal. If \a ptr2's
    template parameter is not a base or a derived type from
    \a ptr1's, you will get a compiler error.
*/

/*!
    \fn bool operator!=(const QWeakPointer<T> &ptr1, const QSharedPointer<X> &ptr2)
    \relates QWeakPointer

    Returns true if the pointer referenced by \a ptr1 is not the
    same pointer as that referenced by \a ptr2.

    If \a ptr2's template parameter is different from \a ptr1's,
    QSharedPointer will attempt to perform an automatic \tt static_cast
    to ensure that the pointers being compared are equal. If \a ptr2's
    template parameter is not a base or a derived type from
    \a ptr1's, you will get a compiler error.
*/

/*!
    \fn QSharedPointer<X> qSharedPointerCast(const QSharedPointer<T> &other)
    \relates QSharedPointer

    Returns a shared pointer to the pointer held by \a other, cast to
    type \tt X.  The types \tt T and \tt X must belong to one
    hierarchy for the \tt static_cast to succeed.

    Note that \tt X must have the same cv-qualifiers (\tt const and
    \tt volatile) that \tt T has, or the code will fail to
    compile. Use qSharedPointerConstCast to cast away the constness.

    \sa QSharedPointer::staticCast(), qSharedPointerDynamicCast(), qSharedPointerConstCast()
*/

/*!
    \fn QSharedPointer<X> qSharedPointerCast(const QWeakPointer<T> &other)
    \relates QSharedPointer
    \relates QWeakPointer

    Returns a shared pointer to the pointer held by \a other, cast to
    type \tt X.  The types \tt T and \tt X must belong to one
    hierarchy for the \tt static_cast to succeed.

    The \a other object is converted first to a strong reference. If
    that conversion fails (because the object it's pointing to has
    already been deleted), this function returns a null
    QSharedPointer.

    Note that \tt X must have the same cv-qualifiers (\tt const and
    \tt volatile) that \tt T has, or the code will fail to
    compile. Use qSharedPointerConstCast to cast away the constness.

    \sa QWeakPointer::toStrongRef(), qSharedPointerDynamicCast(), qSharedPointerConstCast()
*/

/*!
    \fn QSharedPointer<X> qSharedPointerDynamicCast(const QSharedPointer<T> &other)
    \relates QSharedPointer

    Returns a shared pointer to the pointer held by \a other, using a
    dynamic cast to type \tt X to obtain an internal pointer of the
    appropriate type. If the \tt dynamic_cast fails, the object
    returned will be null.

    Note that \tt X must have the same cv-qualifiers (\tt const and
    \tt volatile) that \tt T has, or the code will fail to
    compile. Use qSharedPointerConstCast to cast away the constness.

    \sa QSharedPointer::dynamicCast(), qSharedPointerCast(), qSharedPointerConstCast()
*/

/*!
    \fn QSharedPointer<X> qSharedPointerDynamicCast(const QWeakPointer<T> &other)
    \relates QSharedPointer
    \relates QWeakPointer

    Returns a shared pointer to the pointer held by \a other, using a
    dynamic cast to type \tt X to obtain an internal pointer of the
    appropriate type. If the \tt dynamic_cast fails, the object
    returned will be null.

    The \a other object is converted first to a strong reference. If
    that conversion fails (because the object it's pointing to has
    already been deleted), this function also returns a null
    QSharedPointer.

    Note that \tt X must have the same cv-qualifiers (\tt const and
    \tt volatile) that \tt T has, or the code will fail to
    compile. Use qSharedPointerConstCast to cast away the constness.

    \sa QWeakPointer::toStrongRef(), qSharedPointerCast(), qSharedPointerConstCast()
*/

/*!
    \fn QSharedPointer<X> qSharedPointerConstCast(const QSharedPointer<T> &other)
    \relates QSharedPointer

    Returns a shared pointer to the pointer held by \a other, cast to
    type \tt X.  The types \tt T and \tt X must belong to one
    hierarchy for the \tt const_cast to succeed. The \tt const and \tt
    volatile differences between \tt T and \tt X are ignored.

    \sa QSharedPointer::constCast(), qSharedPointerCast(), qSharedPointerDynamicCast()
*/

/*!
    \fn QSharedPointer<X> qSharedPointerConstCast(const QWeakPointer<T> &other)
    \relates QSharedPointer
    \relates QWeakPointer

    Returns a shared pointer to the pointer held by \a other, cast to
    type \tt X. The types \tt T and \tt X must belong to one
    hierarchy for the \tt const_cast to succeed. The \tt const and
    \tt volatile differences between \tt T and \tt X are ignored.

    The \a other object is converted first to a strong reference. If
    that conversion fails (because the object it's pointing to has
    already been deleted), this function returns a null
    QSharedPointer.

    \sa QWeakPointer::toStrongRef(), qSharedPointerCast(), qSharedPointerDynamicCast()
*/

/*!
    \fn QWeakPointer<X> qWeakPointerCast(const QWeakPointer<T> &other)
    \relates QWeakPointer

    Returns a weak pointer to the pointer held by \a other, cast to
    type \tt X. The types \tt T and \tt X must belong to one
    hierarchy for the \tt static_cast to succeed.

    Note that \tt X must have the same cv-qualifiers (\tt const and
    \tt volatile) that \tt T has, or the code will fail to
    compile. Use qSharedPointerConstCast to cast away the constness.
*/

#include <qset.h>
#include <qmutex.h>

#if !defined(QT_NO_MEMBER_TEMPLATES)

//#  define QT_SHARED_POINTER_BACKTRACE_SUPPORT
#  ifdef QT_SHARED_POINTER_BACKTRACE_SUPPORT
#    if defined(__GLIBC__) && (__GLIBC__ >= 2) && !defined(__UCLIBC__) && !defined(QT_LINUXBASE)
#      define BACKTRACE_SUPPORTED
#    elif defined(Q_OS_MACX)
#      define BACKTRACE_SUPPORTED
#    endif
#  endif

#  if !defined(BACKTRACE_SUPPORTED)
// Dummy implementation of the functions.
// Using QHashDummyValue also means that the QHash below is actually a QSet
typedef QT_PREPEND_NAMESPACE(QHashDummyValue) Backtrace;

static inline Backtrace saveBacktrace() { return Backtrace(); }
static inline void printBacktrace(Backtrace) { }

#  else
#    include <sys/types.h>
#    include <execinfo.h>
#    include <stdio.h>
#    include <unistd.h>
#    include <sys/wait.h>

typedef QT_PREPEND_NAMESPACE(QByteArray) Backtrace;

static inline Backtrace saveBacktrace() __attribute__((always_inline));
static inline Backtrace saveBacktrace()
{
    static const int maxFrames = 32;

    Backtrace stacktrace;
    stacktrace.resize(sizeof(void*) * maxFrames);
    int stack_size = backtrace((void**)stacktrace.data(), maxFrames);
    stacktrace.resize(sizeof(void*) * stack_size);

    return stacktrace;
}

static void printBacktrace(Backtrace stacktrace)
{
    void *const *stack = (void *const *)stacktrace.constData();
    int stack_size = stacktrace.size() / sizeof(void*);
    char **stack_symbols = backtrace_symbols(stack, stack_size);

    int filter[2];
    pid_t child = -1;
    if (pipe(filter) != -1)
        child = fork();
    if (child == 0) {
        // child process
        dup2(fileno(stderr), fileno(stdout));
        dup2(filter[0], fileno(stdin));
        close(filter[0]);
        close(filter[1]);
        execlp("c++filt", "c++filt", "-n", NULL);

        // execlp failed
        execl("/bin/cat", "/bin/cat", NULL);
        _exit(127);
    }

    // parent process
    close(filter[0]);
    FILE *output;
    if (child == -1) {
        // failed forking
        close(filter[1]);
        output = stderr;
    } else {
        output = fdopen(filter[1], "w");
    }

    fprintf(stderr, "Backtrace of the first creation (most recent frame first):\n");
    for (int i = 0; i < stack_size; ++i) {
        if (strlen(stack_symbols[i]))
            fprintf(output, "#%-2d %s\n", i, stack_symbols[i]);
        else
            fprintf(output, "#%-2d %p\n", i, stack[i]);
    }

    if (child != -1) {
        fclose(output);
        waitpid(child, 0, 0);
    }
}
#  endif  // BACKTRACE_SUPPORTED

namespace {
    QT_USE_NAMESPACE
    class KnownPointers
    {
    public:
        QMutex mutex;
        QHash<void *, Backtrace> values;
    };
}

Q_GLOBAL_STATIC(KnownPointers, knownPointers)

QT_BEGIN_NAMESPACE

/*!
    \internal
*/
void QtSharedPointer::internalSafetyCheckAdd(const volatile void *ptr)
{
    QMutexLocker lock(&knownPointers()->mutex);
    void *actual = const_cast<void*>(ptr);
    if (knownPointers()->values.contains(actual)) {
        printBacktrace(knownPointers()->values.value(actual));
        qFatal("QSharedPointerData: internal self-check failed: pointer %p was already tracked "
               "by another QSharedPointerData object", actual);
    }

    knownPointers()->values.insert(actual, saveBacktrace());
}

/*!
    \internal
*/
void QtSharedPointer::internalSafetyCheckRemove(const volatile void *ptr)
{
    QMutexLocker lock(&knownPointers()->mutex);
    void *actual = const_cast<void*>(ptr);
    knownPointers()->values.remove(actual);
}

QT_END_NAMESPACE

#endif
