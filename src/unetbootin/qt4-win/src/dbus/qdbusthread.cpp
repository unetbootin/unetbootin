/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the QtDBus module of the Qt Toolkit.
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

#include <QtCore/qmutex.h>
#include <QtCore/qwaitcondition.h>

#include <stdlib.h>
#include <qdbus_symbols_p.h>

QT_USE_NAMESPACE

static DBusMutex* mutex_new()
{
    return reinterpret_cast<DBusMutex *>(new QMutex(QMutex::NonRecursive));
}

#if 0
static DBusMutex* recursive_mutex_new()
{
    return reinterpret_cast<DBusMutex *>(new QMutex(QMutex::Recursive));
}
#endif

static void mutex_free(DBusMutex *mutex)
{
    delete reinterpret_cast<QMutex *>(mutex);
}

static dbus_bool_t mutex_lock(DBusMutex *mutex)
{
    reinterpret_cast<QMutex *>(mutex)->lock();
    return true;
}

static dbus_bool_t mutex_unlock(DBusMutex *mutex)
{
    reinterpret_cast<QMutex *>(mutex)->unlock();
    return true;
}

static DBusCondVar* condvar_new()
{
    return reinterpret_cast<DBusCondVar *>(new QWaitCondition);
}

static void condvar_free(DBusCondVar *cond)
{
    delete reinterpret_cast<QWaitCondition *>(cond);
}

static void condvar_wait(DBusCondVar *cond, DBusMutex *mutex)
{
    reinterpret_cast<QWaitCondition *>(cond)->wait(reinterpret_cast<QMutex *>(mutex));
}

static dbus_bool_t condvar_wait_timeout(DBusCondVar *cond, DBusMutex *mutex, int msec)
{
    return reinterpret_cast<QWaitCondition *>(cond)->wait(reinterpret_cast<QMutex *>(mutex), msec);
}

static void condvar_wake_one(DBusCondVar *cond)
{
    reinterpret_cast<QWaitCondition *>(cond)->wakeOne();
}

static void condvar_wake_all(DBusCondVar *cond)
{
    reinterpret_cast<QWaitCondition *>(cond)->wakeAll();
}

QT_BEGIN_NAMESPACE

bool qDBusInitThreads()
{
    // ### Disable the recursive mutex functions.
    static DBusThreadFunctions fcn = {
        DBUS_THREAD_FUNCTIONS_MUTEX_NEW_MASK |
        DBUS_THREAD_FUNCTIONS_MUTEX_FREE_MASK |
        DBUS_THREAD_FUNCTIONS_MUTEX_LOCK_MASK |
        DBUS_THREAD_FUNCTIONS_MUTEX_UNLOCK_MASK |
        DBUS_THREAD_FUNCTIONS_CONDVAR_NEW_MASK |
        DBUS_THREAD_FUNCTIONS_CONDVAR_FREE_MASK |
        DBUS_THREAD_FUNCTIONS_CONDVAR_WAIT_MASK |
        DBUS_THREAD_FUNCTIONS_CONDVAR_WAIT_TIMEOUT_MASK |
        DBUS_THREAD_FUNCTIONS_CONDVAR_WAKE_ONE_MASK |
        DBUS_THREAD_FUNCTIONS_CONDVAR_WAKE_ALL_MASK,
#if 0
        DBUS_THREAD_FUNCTIONS_RECURSIVE_MUTEX_NEW_MASK |
        DBUS_THREAD_FUNCTIONS_RECURSIVE_MUTEX_FREE_MASK |
        DBUS_THREAD_FUNCTIONS_RECURSIVE_MUTEX_LOCK_MASK |
        DBUS_THREAD_FUNCTIONS_RECURSIVE_MUTEX_UNLOCK_MASK,
#endif
        mutex_new,
        mutex_free,
        mutex_lock,
        mutex_unlock,
        condvar_new,
        condvar_free,
        condvar_wait,
        condvar_wait_timeout,
        condvar_wake_one,
        condvar_wake_all,
#if 0
        recursive_mutex_new,
        mutex_free,
        mutex_lock,
        mutex_unlock,
#else
        0, 0, 0, 0,
#endif
        0, 0, 0, 0
    };

#if !defined QT_LINKED_LIBDBUS
    void (*threads_init_default)() = (void (*)())qdbus_resolve_conditionally("dbus_threads_init_default");
    void (*threads_init)(DBusThreadFunctions *) = (void (*)(DBusThreadFunctions*))qdbus_resolve_conditionally("dbus_threads_init");

    if (threads_init_default)
        threads_init_default();
    else if (threads_init)
        threads_init(&fcn);
    else
        return false;

    return true;
#else
    dbus_threads_init(&fcn);

    return true;
#endif
}

QT_END_NAMESPACE
