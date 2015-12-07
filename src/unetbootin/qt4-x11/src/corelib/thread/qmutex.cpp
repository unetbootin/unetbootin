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

#include "qplatformdefs.h"
#include "qmutex.h"

#ifndef QT_NO_THREAD
#include "qatomic.h"
#include "qthread.h"
#include "qmutex_p.h"

QT_BEGIN_NAMESPACE

/*!
    \class QMutex
    \brief The QMutex class provides access serialization between threads.

    \threadsafe

    \ingroup thread
    \ingroup environment
    \mainclass

    The purpose of a QMutex is to protect an object, data structure or
    section of code so that only one thread can access it at a time
    (this is similar to the Java \c synchronized keyword). It is
    usually best to use a mutex with a QMutexLocker since this makes
    it easy to ensure that locking and unlocking are performed
    consistently.

    For example, say there is a method that prints a message to the
    user on two lines:

    \snippet doc/src/snippets/code/src_corelib_thread_qmutex.cpp 0

    If these two methods are called in succession, the following happens:

    \snippet doc/src/snippets/code/src_corelib_thread_qmutex.cpp 1

    If these two methods are called simultaneously from two threads then the
    following sequence could result:

    \snippet doc/src/snippets/code/src_corelib_thread_qmutex.cpp 2

    If we add a mutex, we should get the result we want:

    \snippet doc/src/snippets/code/src_corelib_thread_qmutex.cpp 3

    Then only one thread can modify \c number at any given time and
    the result is correct. This is a trivial example, of course, but
    applies to any other case where things need to happen in a
    particular sequence.

    When you call lock() in a thread, other threads that try to call
    lock() in the same place will block until the thread that got the
    lock calls unlock(). A non-blocking alternative to lock() is
    tryLock().

    \sa QMutexLocker, QReadWriteLock, QSemaphore, QWaitCondition
*/

/*!
    \enum QMutex::RecursionMode

    \value Recursive  In this mode, a thread can lock the same mutex
                      multiple times and the mutex won't be unlocked
                      until a corresponding number of unlock() calls
                      have been made.

    \value NonRecursive  In this mode, a thread may only lock a mutex
                         once.

    \sa QMutex()
*/

/*!
    Constructs a new mutex. The mutex is created in an unlocked state.

    If \a mode is QMutex::Recursive, a thread can lock the same mutex
    multiple times and the mutex won't be unlocked until a
    corresponding number of unlock() calls have been made. The
    default is QMutex::NonRecursive.

    \sa lock(), unlock()
*/
QMutex::QMutex(RecursionMode mode)
    : d(new QMutexPrivate(mode))
{ }

/*!
    Destroys the mutex.

    \warning Destroying a locked mutex may result in undefined behavior.
*/
QMutex::~QMutex()
{ delete d; }

/*!
    Locks the mutex. If another thread has locked the mutex then this
    call will block until that thread has unlocked it.

    Calling this function multiple times on the same mutex from the
    same thread is allowed if this mutex is a
    \l{QMutex::Recursive}{recursive mutex}. If this mutex is a
    \l{QMutex::NonRecursive}{non-recursive mutex}, this function will
    \e dead-lock when the mutex is locked recursively.

    \sa unlock()
*/
void QMutex::lock()
{
    Qt::HANDLE self;

    if (d->recursive) {
        self = QThread::currentThreadId();
        if (d->owner == self) {
            ++d->count;
            Q_ASSERT_X(d->count != 0, "QMutex::lock", "Overflow in recursion counter");
            return;
        }

        bool isLocked = d->contenders.fetchAndAddAcquire(1) == 0;
        if (!isLocked) {
#ifndef QT_NO_DEBUG
            if (d->owner == self)
                qWarning("QMutex::lock: Deadlock detected in thread %ld",
                         long(d->owner));
#endif

            // didn't get the lock, wait for it
            isLocked = d->wait();
            Q_ASSERT_X(isLocked, "QMutex::lock",
                       "Internal error, infinite wait has timed out.");

            // don't need to wait for the lock anymore
            d->contenders.deref();
        }

        d->owner = self;
        ++d->count;
        Q_ASSERT_X(d->count != 0, "QMutex::lock", "Overflow in recursion counter");
        return;
    }

#ifndef QT_NO_DEBUG
    self = QThread::currentThreadId();
#endif

    bool isLocked = d->contenders == 0 && d->contenders.testAndSetAcquire(0, 1);
    if (!isLocked) {
        int spinCount = 0;
        int lastSpinCount = d->lastSpinCount;

        enum { AdditionalSpins = 100, SpinCountPenalizationDivisor = 4 };
        const int maximumSpinCount = lastSpinCount + AdditionalSpins;

        do {
            if (spinCount++ > maximumSpinCount) {
                // puts("spinning useless, sleeping");
                isLocked = d->contenders.fetchAndAddAcquire(1) == 0;
                if (!isLocked) {
#ifndef QT_NO_DEBUG
                    if (d->owner == self)
                        qWarning("QMutex::lock: Deadlock detected in thread %ld",
                                 long(d->owner));
#endif

                    // didn't get the lock, wait for it
                    isLocked = d->wait();
                    Q_ASSERT_X(isLocked, "QMutex::lock",
                               "Internal error, infinite wait has timed out.");

                    // don't need to wait for the lock anymore
                    d->contenders.deref();
                }
                // decrease the lastSpinCount since we didn't actually get the lock by spinning
                spinCount = -d->lastSpinCount / SpinCountPenalizationDivisor;
                break;
            }

            QThread::yieldCurrentThread();
            isLocked = d->contenders == 0 && d->contenders.testAndSetAcquire(0, 1);
        } while (!isLocked);

        // adjust the last spin lock count
        lastSpinCount = d->lastSpinCount;
        d->lastSpinCount = spinCount >= 0
                           ? qMax(lastSpinCount, spinCount)
                           : lastSpinCount + spinCount;
    }

#ifndef QT_NO_DEBUG
    d->owner = self;
#endif
}

/*!
    Attempts to lock the mutex. If the lock was obtained, this function
    returns true. If another thread has locked the mutex, this
    function returns false immediately.

    If the lock was obtained, the mutex must be unlocked with unlock()
    before another thread can successfully lock it.

    Calling this function multiple times on the same mutex from the
    same thread is allowed if this mutex is a
    \l{QMutex::Recursive}{recursive mutex}. If this mutex is a
    \l{QMutex::NonRecursive}{non-recursive mutex}, this function will
    \e always return false when attempting to lock the mutex
    recursively.

    \sa lock(), unlock()
*/
bool QMutex::tryLock()
{
    Qt::HANDLE self;

    if (d->recursive) {
        self = QThread::currentThreadId();
        if (d->owner == self) {
            ++d->count;
            Q_ASSERT_X(d->count != 0, "QMutex::tryLock", "Overflow in recursion counter");
            return true;
        }

        bool isLocked = d->contenders == 0 && d->contenders.testAndSetAcquire(0, 1);
        if (!isLocked) {
            // some other thread has the mutex locked, or we tried to
            // recursively lock an non-recursive mutex
            return isLocked;
        }

        d->owner = self;
        ++d->count;
        Q_ASSERT_X(d->count != 0, "QMutex::tryLock", "Overflow in recursion counter");
        return isLocked;
    }

#ifndef QT_NO_DEBUG
    self = QThread::currentThreadId();
#endif
    bool isLocked = d->contenders == 0 && d->contenders.testAndSetAcquire(0, 1);
    if (!isLocked) {
        // some other thread has the mutex locked, or we tried to
        // recursively lock an non-recursive mutex
        return isLocked;
    }
#ifndef QT_NO_DEBUG
    d->owner = self;
#endif
    return isLocked;
}

/*! \overload

    Attempts to lock the mutex. This function returns true if the lock
    was obtained; otherwise it returns false. If another thread has
    locked the mutex, this function will wait for at most \a timeout
    milliseconds for the mutex to become available.

    Note: Passing a negative number as the \a timeout is equivalent to
    calling lock(), i.e. this function will wait forever until mutex
    can be locked if \a timeout is negative.

    If the lock was obtained, the mutex must be unlocked with unlock()
    before another thread can successfully lock it.

    Calling this function multiple times on the same mutex from the
    same thread is allowed if this mutex is a
    \l{QMutex::Recursive}{recursive mutex}. If this mutex is a
    \l{QMutex::NonRecursive}{non-recursive mutex}, this function will
    \e always return false when attempting to lock the mutex
    recursively.

    \sa lock(), unlock()
*/
bool QMutex::tryLock(int timeout)
{
    Qt::HANDLE self;

    if (d->recursive) {
        self = QThread::currentThreadId();
        if (d->owner == self) {
            ++d->count;
            Q_ASSERT_X(d->count != 0, "QMutex::tryLock", "Overflow in recursion counter");
            return true;
        }

        bool isLocked = d->contenders.fetchAndAddAcquire(1) == 0;
        if (!isLocked) {
            // didn't get the lock, wait for it
            isLocked = d->wait(timeout);

            // don't need to wait for the lock anymore
            d->contenders.deref();
            if (!isLocked)
                return false;
        }

        d->owner = self;
        ++d->count;
        Q_ASSERT_X(d->count != 0, "QMutex::tryLock", "Overflow in recursion counter");
        return true;
    }

#ifndef QT_NO_DEBUG
    self = QThread::currentThreadId();
#endif
    bool isLocked = d->contenders.fetchAndAddAcquire(1) == 0;
    if (!isLocked) {
        // didn't get the lock, wait for it
        isLocked = d->wait(timeout);

        // don't need to wait for the lock anymore
        d->contenders.deref();
        if (!isLocked)
            return false;
    }
#ifndef QT_NO_DEBUG
    d->owner = self;
#endif
    return true;
}


/*!
    Unlocks the mutex. Attempting to unlock a mutex in a different
    thread to the one that locked it results in an error. Unlocking a
    mutex that is not locked results in undefined behavior.

    \sa lock()
*/
void QMutex::unlock()
{
    Q_ASSERT_X(d->owner == QThread::currentThreadId(), "QMutex::unlock()",
               "A mutex must be unlocked in the same thread that locked it.");

    if (d->recursive) {
        if (!--d->count) {
            d->owner = 0;
            if (!d->contenders.testAndSetRelease(1, 0))
                d->wakeUp();
        }
    } else {
#ifndef QT_NO_DEBUG
        d->owner = 0;
#endif
        if (!d->contenders.testAndSetRelease(1, 0))
            d->wakeUp();
    }
}

/*!
    \fn bool QMutex::locked()

    Returns true if the mutex is locked by another thread; otherwise
    returns false.

    It is generally a bad idea to use this function, because code
    that uses it has a race condition. Use tryLock() and unlock()
    instead.

    \oldcode
        bool isLocked = mutex.locked();
    \newcode
        bool isLocked = true;
        if (mutex.tryLock()) {
            mutex.unlock();
            isLocked = false;
        }
    \endcode
*/

/*!
    \class QMutexLocker
    \brief The QMutexLocker class is a convenience class that simplifies
    locking and unlocking mutexes.

    \threadsafe

    \ingroup thread
    \ingroup environment

    Locking and unlocking a QMutex in complex functions and
    statements or in exception handling code is error-prone and
    difficult to debug. QMutexLocker can be used in such situations
    to ensure that the state of the mutex is always well-defined.

    QMutexLocker should be created within a function where a
    QMutex needs to be locked. The mutex is locked when QMutexLocker
    is created. You can unlock and relock the mutex with \c unlock()
    and \c relock(). If locked, the mutex will be unlocked when the
    QMutexLocker is destroyed.

    For example, this complex function locks a QMutex upon entering
    the function and unlocks the mutex at all the exit points:

    \snippet doc/src/snippets/code/src_corelib_thread_qmutex.cpp 4

    This example function will get more complicated as it is
    developed, which increases the likelihood that errors will occur.

    Using QMutexLocker greatly simplifies the code, and makes it more
    readable:

    \snippet doc/src/snippets/code/src_corelib_thread_qmutex.cpp 5

    Now, the mutex will always be unlocked when the QMutexLocker
    object is destroyed (when the function returns since \c locker is
    an auto variable).

    The same principle applies to code that throws and catches
    exceptions. An exception that is not caught in the function that
    has locked the mutex has no way of unlocking the mutex before the
    exception is passed up the stack to the calling function.

    QMutexLocker also provides a \c mutex() member function that returns
    the mutex on which the QMutexLocker is operating. This is useful
    for code that needs access to the mutex, such as
    QWaitCondition::wait(). For example:

    \snippet doc/src/snippets/code/src_corelib_thread_qmutex.cpp 6

    \sa QReadLocker, QWriteLocker, QMutex
*/

/*!
    \fn QMutexLocker::QMutexLocker(QMutex *mutex)

    Constructs a QMutexLocker and locks \a mutex. The mutex will be
    unlocked when the QMutexLocker is destroyed. If \a mutex is zero,
    QMutexLocker does nothing.

    \sa QMutex::lock()
*/

/*!
    \fn QMutexLocker::~QMutexLocker()

    Destroys the QMutexLocker and unlocks the mutex that was locked
    in the constructor.

    \sa QMutex::unlock()
*/

/*!
    \fn QMutex *QMutexLocker::mutex() const

    Returns a pointer to the mutex that was locked in the
    constructor.
*/

/*!
    \fn void QMutexLocker::unlock()

    Unlocks this mutex locker. You can use \c relock() to lock
    it again. It does not need to be locked when destroyed.

    \sa relock()
*/

/*!
    \fn void QMutexLocker::relock()

    Relocks an unlocked mutex locker.

    \sa unlock()
*/

/*!
    \fn QMutex::QMutex(bool recursive)

    Use the constructor that takes a RecursionMode parameter instead.
*/

QT_END_NAMESPACE

#endif // QT_NO_THREAD
