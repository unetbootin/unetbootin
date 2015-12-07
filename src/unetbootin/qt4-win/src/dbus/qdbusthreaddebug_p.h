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
#ifndef QDBUSTHREADDEBUG_P_H
#define QDBUSTHREADDEBUG_P_H

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

#if !defined(QDBUS_THREAD_DEBUG) && defined(QT_BUILD_INTERNAL)
# define QDBUS_THREAD_DEBUG 1
#endif

#if QDBUS_THREAD_DEBUG
QT_BEGIN_NAMESPACE
typedef void (*qdbusThreadDebugFunc)(int, int, QDBusConnectionPrivate *);
QDBUS_EXPORT void qdbusDefaultThreadDebug(int, int, QDBusConnectionPrivate *);
extern QDBUS_EXPORT qdbusThreadDebugFunc qdbusThreadDebug;
QT_END_NAMESPACE
#endif

enum ThreadAction {
    ConnectAction = 0,
    DisconnectAction = 1,
    RegisterObjectAction = 2,
    UnregisterObjectAction = 3,
    ObjectRegisteredAtAction = 4,

    CloseConnectionAction = 10,
    ObjectDestroyedAction = 11,
    RelaySignalAction = 12,
    HandleObjectCallAction = 13,
    HandleSignalAction = 14,
    ConnectRelayAction = 15,
    DisconnectRelayAction = 16,
    FindMetaObject1Action = 17,
    FindMetaObject2Action = 18,
    RegisterServiceAction = 19,
    UnregisterServiceAction = 20,
    UpdateSignalHookOwnerAction = 21,
    HandleObjectCallPostEventAction = 22,
    HandleObjectCallSemaphoreAction = 23,
    DoDispatchAction = 24,
    SendWithReplyAsyncAction = 25,
    MessageResultReceivedAction = 26,
    ActivateSignalAction = 27,

    AddTimeoutAction = 50,
    RemoveTimeoutAction = 51,
    AddWatchAction = 52,
    RemoveWatchAction = 53,
    ToggleWatchAction = 54
};

struct QDBusLockerBase
{
    enum Condition
    {
        BeforeLock,
        AfterLock,
        BeforeUnlock,
        AfterUnlock,

        BeforePost,
        AfterPost,
        BeforeDeliver,
        AfterDeliver,

        BeforeAcquire,
        AfterAcquire,
        BeforeRelease,
        AfterRelease
    };

#if QDBUS_THREAD_DEBUG
    static inline void reportThreadAction(int action, int condition, QDBusConnectionPrivate *ptr)
    { if (qdbusThreadDebug) qdbusThreadDebug(action, condition, ptr); }
#else
    static inline void reportThreadAction(int, int, QDBusConnectionPrivate *) { }
#endif
};

struct QDBusReadLocker: QDBusLockerBase
{
    QDBusConnectionPrivate *self;
    ThreadAction action;
    inline QDBusReadLocker(ThreadAction a, QDBusConnectionPrivate *s)
        : self(s), action(a)
    {
        reportThreadAction(action, BeforeLock, self);
        self->lock.lockForRead();
        reportThreadAction(action, AfterLock, self);
    }

    inline ~QDBusReadLocker()
    {
        reportThreadAction(action, BeforeUnlock, self);
        self->lock.unlock();
        reportThreadAction(action, AfterUnlock, self);
    }
};

struct QDBusWriteLocker: QDBusLockerBase
{
    QDBusConnectionPrivate *self;
    ThreadAction action;
    inline QDBusWriteLocker(ThreadAction a, QDBusConnectionPrivate *s)
        : self(s), action(a)
    {
        reportThreadAction(action, BeforeLock, self);
        self->lock.lockForWrite();
        reportThreadAction(action, AfterLock, self);
    }

    inline ~QDBusWriteLocker()
    {
        reportThreadAction(action, BeforeUnlock, self);
        self->lock.unlock();
        reportThreadAction(action, AfterUnlock, self);
    }
};

struct QDBusDispatchLocker: QDBusLockerBase
{
    QDBusConnectionPrivate *self;
    ThreadAction action;
    inline QDBusDispatchLocker(ThreadAction a, QDBusConnectionPrivate *s)
        : self(s), action(a)
    {
        reportThreadAction(action, BeforeLock, self);
        self->dispatchLock.lock();
        reportThreadAction(action, AfterLock, self);
    }

    inline ~QDBusDispatchLocker()
    {
        reportThreadAction(action, BeforeUnlock, self);
        self->dispatchLock.unlock();
        reportThreadAction(action, AfterUnlock, self);
    }
};

#if QDBUS_THREAD_DEBUG
# define SEM_ACQUIRE(action, sem)                                       \
    do {                                                                \
    QDBusLockerBase::reportThreadAction(action, QDBusLockerBase::BeforeAcquire, this); \
    sem.acquire();                                                      \
    QDBusLockerBase::reportThreadAction(action, QDBusLockerBase::AfterAcquire, this); \
    } while (0)

# define SEM_RELEASE(action, sem)                                       \
    do {                                                                \
    QDBusLockerBase::reportThreadAction(action, QDBusLockerBase::BeforeRelease, that); \
    sem.release();                                                      \
    QDBusLockerBase::reportThreadAction(action, QDBusLockerBase::AfterRelease, that); \
    } while (0)

#else
# define SEM_ACQUIRE(action, sem)       sem.acquire()
# define SEM_RELEASE(action, sem)       sem.release()
#endif

#endif
