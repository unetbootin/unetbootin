/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the tools applications of the Qt Toolkit.
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

#include "qlock_p.h"

#ifndef QT_NO_QWS_MULTIPROCESS

#include "qwssignalhandler_p.h"
#include <unistd.h>
#include <sys/types.h>
#if defined(Q_OS_DARWIN)
#   define Q_NO_SEMAPHORE
#   include <sys/stat.h>
#   include <sys/file.h>
#else // Q_OS_DARWIN
#   include <sys/sem.h>
#   if (defined(__GNU_LIBRARY__) && !defined(_SEM_SEMUN_UNDEFINED) && !defined(QT_LINUXBASE)) \
    || defined(Q_OS_FREEBSD) || defined(Q_OS_OPENBSD) || defined(Q_OS_NETBSD) \
    || defined(Q_OS_BSDI)
        /* union semun is defined by including <sys/sem.h> */
#   else
/* according to X/OPEN we have to define it ourselves */
union semun {
    int val;                    /* value for SETVAL */
    struct semid_ds *buf;       /* buffer for IPC_STAT, IPC_SET */
    unsigned short *array;      /* array for GETALL, SETALL */
};
#   endif
#endif // Q_OS_DARWIN
#include <sys/ipc.h>
#include <string.h>
#include <errno.h>
#include <qdebug.h>
#include <signal.h>

#endif // QT_NO_QWS_MULTIPROCESS

#define MAX_LOCKS   200            // maximum simultaneous read locks

QT_BEGIN_NAMESPACE


#ifndef QT_NO_QWS_MULTIPROCESS
class QLockData
{
public:
#ifdef Q_NO_SEMAPHORE
    QByteArray file;
#endif // Q_NO_SEMAPHORE
    int id;
    int count;
    bool owned;
};
#endif // QT_NO_QWS_MULTIPROCESS

/*!
    \class QLock qlock_p.h
    \brief The QLock class is a wrapper for a System V shared semaphore.

    \ingroup qws
    \ingroup io

    \internal

    It is used by \l{Qt for Embedded Linux} for synchronizing access to the graphics
    card and shared memory region between processes.
*/

/*!
    \enum QLock::Type

    \value Read
    \value Write
*/

/*!
    \fn QLock::QLock(const QString &filename, char id, bool create)

    Creates a lock. \a filename is the file path of the Unix-domain
    socket the \l{Qt for Embedded Linux} client is using. \a id is the name of the
    particular lock to be created on that socket. If \a create is true
    the lock is to be created (as the Qt for Embedded Linux server does); if \a
    create is false the lock should exist already (as the Qt for Embedded Linux
    client expects).
*/

QLock::QLock(const QString &filename, char id, bool create)
{
#ifdef QT_NO_QWS_MULTIPROCESS
    Q_UNUSED(filename);
    Q_UNUSED(id);
    Q_UNUSED(create);
#else
    data = new QLockData;
    data->count = 0;
#ifdef Q_NO_SEMAPHORE
    data->file = QString(filename+id).toLocal8Bit().constData();
    for(int x = 0; x < 2; x++) {
        data->id = open(data->file, O_RDWR | (x ? O_CREAT : 0), S_IRWXU);
        if(data->id != -1 || !create) {
            data->owned = x;
            break;
        }
    }
#else
    key_t semkey = ftok(filename.toLocal8Bit().constData(), id);
    data->id = semget(semkey,0,0);
    data->owned = create;
    if (create) {
        semun arg; arg.val = 0;
        if (data->id != -1)
            semctl(data->id,0,IPC_RMID,arg);
        data->id = semget(semkey,1,IPC_CREAT|0600);
        arg.val = MAX_LOCKS;
        semctl(data->id,0,SETVAL,arg);

        QWSSignalHandler::instance()->addSemaphore(data->id);
    }
#endif
    if (data->id == -1) {
        int eno = errno;
        qWarning("Cannot %s semaphore %s '%c'", (create ? "create" : "get"),
                 qPrintable(filename), id);
        qDebug() << "Error" << eno << strerror(eno);
    }
#endif
}

/*!
    \fn QLock::~QLock()

    Destroys a lock
*/

QLock::~QLock()
{
#ifndef QT_NO_QWS_MULTIPROCESS
    if (locked())
        unlock();
#ifdef Q_NO_SEMAPHORE
    if(isValid()) {
        close(data->id);
        if(data->owned)
            unlink(data->file);
    }
#else
    if(data->owned)
        QWSSignalHandler::instance()->removeSemaphore(data->id);
#endif
    delete data;
#endif
}

/*!
    \fn bool QLock::isValid() const

    Returns true if the lock constructor was successful; returns false if
    the lock could not be created or was not available to connect to.
*/

bool QLock::isValid() const
{
#ifndef QT_NO_QWS_MULTIPROCESS
    return (data->id != -1);
#else
    return true;
#endif
}

/*!
    Locks the semaphore with a lock of type \a t. Locks can either be
    \c Read or \c Write. If a lock is \c Read, attempts by other
    processes to obtain \c Read locks will succeed, and \c Write
    attempts will block until the lock is unlocked. If locked as \c
    Write, all attempts to lock by other processes will block until
    the lock is unlocked. Locks are stacked: i.e. a given QLock can be
    locked multiple times by the same process without blocking, and
    will only be unlocked after a corresponding number of unlock()
    calls.
*/

void QLock::lock(Type t)
{
#ifdef QT_NO_QWS_MULTIPROCESS
    Q_UNUSED(t);
#else
    if (!data->count) {
#ifdef Q_NO_SEMAPHORE
        int op = LOCK_SH;
        if(t == Write)
            op = LOCK_EX;
        for(int rv=1; rv;) {
            rv = flock(data->id, op);
            if (rv == -1 && errno != EINTR)
                qDebug("Semop lock failure %s",strerror(errno));
        }
#else
        sembuf sops;
        sops.sem_num = 0;
        sops.sem_flg = SEM_UNDO;

        if (t == Write) {
            sops.sem_op = -MAX_LOCKS;
            type = Write;
        } else {
            sops.sem_op = -1;
            type = Read;
        }

        int rv;
        do {
            rv = semop(data->id,&sops,1);
            if (rv == -1 && errno != EINTR)
                qDebug("Semop lock failure %s",strerror(errno));
        } while (rv == -1 && errno == EINTR);
#endif
    }
    data->count++;
#endif
}

/*!
    \fn void QLock::unlock()

    Unlocks the semaphore. If other processes were blocking waiting to
    lock() the semaphore, one of them will wake up and succeed in
    lock()ing.
*/

void QLock::unlock()
{
#ifndef QT_NO_QWS_MULTIPROCESS
    if(data->count) {
        data->count--;
        if(!data->count) {
#ifdef Q_NO_SEMAPHORE
            for(int rv=1; rv;) {
                rv = flock(data->id, LOCK_UN);
                if (rv == -1 && errno != EINTR)
                    qDebug("Semop lock failure %s",strerror(errno));
            }
#else
            sembuf sops;
            sops.sem_num = 0;
            sops.sem_op = 1;
            sops.sem_flg = SEM_UNDO;
            if (type == Write)
                sops.sem_op = MAX_LOCKS;

            int rv;
            do {
                rv = semop(data->id,&sops,1);
                if (rv == -1 && errno != EINTR)
                    qDebug("Semop unlock failure %s",strerror(errno));
            } while (rv == -1 && errno == EINTR);
#endif
        }
    } else {
        qDebug("Unlock without corresponding lock");
    }
#endif
}

/*!
    \fn bool QLock::locked() const

    Returns true if the lock is currently held by the current process;
    otherwise returns false.
*/

bool QLock::locked() const
{
#ifndef QT_NO_QWS_MULTIPROCESS
    return (data->count > 0);
#else
    return false;
#endif
}

QT_END_NAMESPACE
