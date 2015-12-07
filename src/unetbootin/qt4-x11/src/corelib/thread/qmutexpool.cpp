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

#include "qatomic.h"
#include "qmutexpool_p.h"

#ifndef QT_NO_THREAD

QT_BEGIN_NAMESPACE

// qt_global_mutexpool is here for backwards compatability only,
// use QMutexpool::instance() in new clode.
Q_CORE_EXPORT QMutexPool *qt_global_mutexpool = 0;
Q_GLOBAL_STATIC_WITH_ARGS(QMutexPool, globalMutexPool, (true))

/*!
    \class QMutexPool qmutexpool_p.h
    \brief The QMutexPool class provides a pool of QMutex objects.

    \internal

    \ingroup thread

    QMutexPool is a convenience class that provides access to a fixed
    number of QMutex objects.

    Typical use of a QMutexPool is in situations where it is not
    possible or feasible to use one QMutex for every protected object.
    The mutex pool will return a mutex based on the address of the
    object that needs protection.

    For example, consider this simple class:

    \snippet doc/src/snippets/code/src_corelib_thread_qmutexpool.cpp 0

    Adding a QMutex member to the Number class does not make sense,
    because it is so small. However, in order to ensure that access to
    each Number is protected, you need to use a mutex. In this case, a
    QMutexPool would be ideal.

    Code to calculate the square of a number would then look something
    like this:

    \snippet doc/src/snippets/code/src_corelib_thread_qmutexpool.cpp 1

    This function will safely calculate the square of a number, since
    it uses a mutex from a QMutexPool. The mutex is locked and
    unlocked automatically by the QMutexLocker class. See the
    QMutexLocker documentation for more details.
*/

/*!
    Constructs  a QMutexPool, reserving space for \a size QMutexes. If
    \a recursive is true, all QMutexes in the pool will be recursive
    mutexes; otherwise they will all be non-recursive (the default).

    The QMutexes are created when needed, and deleted when the
    QMutexPool is destructed.
*/
QMutexPool::QMutexPool(bool recursive, int size)
    : count(size), recurs(recursive)
{
    mutexes = new QMutex*[count];
    for (int index = 0; index < count; ++index) {
        mutexes[index] = 0;
    }
}

/*!
    Destructs a QMutexPool. All QMutexes that were created by the pool
    are deleted.
*/
QMutexPool::~QMutexPool()
{
    QMutexLocker locker(&mutex);
    for (int index = 0; index < count; ++index) {
        delete mutexes[index];
        mutexes[index] = 0;
    }
    delete [] mutexes;
    mutexes = 0;
}

/*!
    Returns the global QMutexPool instance.
*/
QMutexPool *QMutexPool::instance()
{
    return globalMutexPool();
}

/*!
    Returns a QMutex from the pool. QMutexPool uses the value \a address
    to determine which mutex is returned from the pool.
*/
QMutex *QMutexPool::get(const void *address)
{
    Q_ASSERT_X(address != 0, "QMutexPool::get()", "'address' argument cannot be zero");
    int index = int((quintptr(address) >> (sizeof(address) >> 1)) % count);

    if (!mutexes[index]) {
        // mutex not created, create one

        QMutexLocker locker(&mutex);
        // we need to check once again that the mutex hasn't been created, since
        // 2 threads could be trying to create a mutex at the same index...
        if (!mutexes[index])
            mutexes[index] = new QMutex(recurs ? QMutex::Recursive : QMutex::NonRecursive);
    }

    return mutexes[index];
}

/*!
    Returns a QMutex from the global mutex pool.
*/
QMutex *QMutexPool::globalInstanceGet(const void *address)
{
    QMutexPool * const globalInstance = globalMutexPool();
    if (globalInstance == 0)
        return 0;
    return globalInstance->get(address);
}

QT_END_NAMESPACE

#endif // QT_NO_THREAD
