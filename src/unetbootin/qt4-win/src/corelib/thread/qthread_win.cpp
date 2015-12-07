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

#include "qthread.h"
#include "qthread_p.h"
#include "qthreadstorage.h"
#include "qmutex.h"

#include <qcoreapplication.h>
#include <qpointer.h>

#include <private/qcoreapplication_p.h>
#include <private/qeventdispatcher_win_p.h>

#include <windows.h>


#ifndef Q_OS_WINCE
#ifndef _MT
#define _MT
#endif
#include <process.h>
#endif

#ifndef QT_NO_THREAD
QT_BEGIN_NAMESPACE

void qt_watch_adopted_thread(const HANDLE adoptedThreadHandle, QThread *qthread);
void qt_adopted_thread_watcher_function(void *);

static DWORD qt_current_thread_data_tls_index = TLS_OUT_OF_INDEXES;
void qt_create_tls()
{
    if (qt_current_thread_data_tls_index != TLS_OUT_OF_INDEXES)
        return;
    static QMutex mutex;
    QMutexLocker locker(&mutex);
    qt_current_thread_data_tls_index = TlsAlloc();
}


/*
    QThreadData
*/
QThreadData *QThreadData::current()
{
    qt_create_tls();
    QThreadData *threadData = reinterpret_cast<QThreadData *>(TlsGetValue(qt_current_thread_data_tls_index));
    if (!threadData) {
        QThread *adopted = 0;
        if (QInternal::activateCallbacks(QInternal::AdoptCurrentThread, (void **) &adopted)) {
            Q_ASSERT(adopted);
            threadData = QThreadData::get2(adopted);
            TlsSetValue(qt_current_thread_data_tls_index, threadData);
            adopted->d_func()->running = true;
            adopted->d_func()->finished = false;
            static_cast<QAdoptedThread *>(adopted)->init();
        } else {
            threadData = new QThreadData;
            // This needs to be called prior to new AdoptedThread() to
            // avoid recursion.
            TlsSetValue(qt_current_thread_data_tls_index, threadData);
            threadData->thread = new QAdoptedThread(threadData);
            threadData->deref();
        }

        if (!QCoreApplicationPrivate::theMainThread) {
            QCoreApplicationPrivate::theMainThread = threadData->thread;
        } else {
            HANDLE realHandle = INVALID_HANDLE_VALUE;
#if !defined(Q_OS_WINCE) || (defined(_WIN32_WCE) && (_WIN32_WCE>=0x600))
            DuplicateHandle(GetCurrentProcess(),
                    GetCurrentThread(),
                    GetCurrentProcess(),
                    &realHandle,
                    0,
                    FALSE,
                    DUPLICATE_SAME_ACCESS);
#else
			realHandle = (HANDLE)GetCurrentThreadId();
#endif
            qt_watch_adopted_thread(realHandle, threadData->thread);
        }
    }
    return threadData;
}

void QAdoptedThread::init()
{
    d_func()->handle = GetCurrentThread();
    d_func()->id = GetCurrentThreadId();
}

static QVector<HANDLE> qt_adopted_thread_handles;
static QVector<QThread *> qt_adopted_qthreads;
static QMutex qt_adopted_thread_watcher_mutex;
static HANDLE qt_adopted_thread_watcher_handle = 0;
static HANDLE qt_adopted_thread_wakeup = 0;

/*! \internal
    Adds an adopted thread to the list of threads that Qt watches to make sure
    the thread data is properly cleaned up. This function starts the watcher
    thread if necessary.
*/
void qt_watch_adopted_thread(const HANDLE adoptedThreadHandle, QThread *qthread)
{
    QMutexLocker lock(&qt_adopted_thread_watcher_mutex);
    qt_adopted_thread_handles.append(adoptedThreadHandle);
    qt_adopted_qthreads.append(qthread);

    // Start watcher thread if it is not already running.
    if (qt_adopted_thread_watcher_handle == 0) {
        if (qt_adopted_thread_wakeup == 0) {
            qt_adopted_thread_wakeup = QT_WA_INLINE(CreateEventW(0, false, false, 0),
                                                    CreateEventA(0, false, false, 0));
            qt_adopted_thread_handles.prepend(qt_adopted_thread_wakeup);
        }

        qt_adopted_thread_watcher_handle =
            (HANDLE)_beginthread(qt_adopted_thread_watcher_function, 0, NULL);
    } else {
        SetEvent(qt_adopted_thread_wakeup);
    }
}

/*! \internal
    This function loops and waits for native adopted threads to finish.
    When this happens it derefs the QThreadData for the adopted thread
    to make sure it gets cleaned up properly.
*/
void qt_adopted_thread_watcher_function(void *)
{
    forever {
        qt_adopted_thread_watcher_mutex.lock();

        if (qt_adopted_thread_handles.count() == 1) {
            qt_adopted_thread_watcher_handle = 0;
            qt_adopted_thread_watcher_mutex.unlock();
            break;
        }

        QVector<HANDLE> handlesCopy = qt_adopted_thread_handles;
        qt_adopted_thread_watcher_mutex.unlock();

        DWORD ret = WAIT_TIMEOUT;
        int loops = (handlesCopy.count() / MAXIMUM_WAIT_OBJECTS) + 1, offset, count;
        if (loops == 1) {
            // no need to loop, no timeout
            offset = 0;
            count = handlesCopy.count();
            ret = WaitForMultipleObjects(handlesCopy.count(), handlesCopy.constData(), false, INFINITE);
        } else {
            int loop = 0;
            do {
                offset = loop * MAXIMUM_WAIT_OBJECTS;
                count = qMin(handlesCopy.count() - offset, MAXIMUM_WAIT_OBJECTS);
                ret = WaitForMultipleObjects(count, handlesCopy.constData() + offset, false, 100);
                loop = (loop + 1) % loops;
            } while (ret == WAIT_TIMEOUT);
        }

        if (ret == WAIT_FAILED || !(ret >= WAIT_OBJECT_0 && ret < WAIT_OBJECT_0 + uint(count))) {
            qWarning("QThread internal error while waiting for adopted threads: %d", int(GetLastError()));
            continue;
        }

        const int handleIndex = offset + ret - WAIT_OBJECT_0;
        if (handleIndex == 0){
            // New handle to watch was added.
            continue;
        } else {
//             printf("(qt) - qt_adopted_thread_watcher_function... called\n");
            const int qthreadIndex = handleIndex - 1;
            QThreadData::get2(qt_adopted_qthreads.at(qthreadIndex))->deref();
#if !defined(Q_OS_WINCE) || (defined(_WIN32_WCE) && (_WIN32_WCE>=0x600))
            CloseHandle(qt_adopted_thread_handles.at(handleIndex));
#endif
            QMutexLocker lock(&qt_adopted_thread_watcher_mutex);
            qt_adopted_thread_handles.remove(handleIndex);
            qt_adopted_qthreads.remove(qthreadIndex);
        }
    }
}

/**************************************************************************
 ** QThreadPrivate
 *************************************************************************/

#endif // QT_NO_THREAD

void QThreadPrivate::createEventDispatcher(QThreadData *data)
{
    data->eventDispatcher = new QEventDispatcherWin32;
    data->eventDispatcher->startingUp();
}

#ifndef QT_NO_THREAD

unsigned int __stdcall QThreadPrivate::start(void *arg)
{
    QThread *thr = reinterpret_cast<QThread *>(arg);
    QThreadData *data = QThreadData::get2(thr);

    qt_create_tls();
    TlsSetValue(qt_current_thread_data_tls_index, data);

    QThread::setTerminationEnabled(false);

    data->quitNow = false;
    // ### TODO: allow the user to create a custom event dispatcher
    createEventDispatcher(data);

    emit thr->started();
    QThread::setTerminationEnabled(true);
    thr->run();

    finish(arg);
    return 0;
}

void QThreadPrivate::finish(void *arg, bool lockAnyway)
{
    QThread *thr = reinterpret_cast<QThread *>(arg);
    QThreadPrivate *d = thr->d_func();

    if (lockAnyway)
        d->mutex.lock();
    d->priority = QThread::InheritPriority;
    d->running = false;
    d->finished = true;
    if (d->terminated)
        emit thr->terminated();
    d->terminated = false;
    emit thr->finished();

    if (d->data->eventDispatcher) {
        d->data->eventDispatcher->closingDown();
        QAbstractEventDispatcher *eventDispatcher = d->data->eventDispatcher;
        d->data->eventDispatcher = 0;
        delete eventDispatcher;
    }

    QThreadStorageData::finish(reinterpret_cast<void **>(&d->data->tls));

    if (!d->waiters) {
        CloseHandle(d->handle);
        d->handle = 0;
    }

    d->id = 0;

    if (lockAnyway)
        d->mutex.unlock();
}

/**************************************************************************
 ** QThread
 *************************************************************************/

Qt::HANDLE QThread::currentThreadId()
{
    return (Qt::HANDLE)GetCurrentThreadId();
}

int QThread::idealThreadCount()
{
    SYSTEM_INFO sysinfo;
    GetSystemInfo(&sysinfo);
    return sysinfo.dwNumberOfProcessors;
}

void QThread::sleep(unsigned long secs)
{
    ::Sleep(secs * 1000);
}

void QThread::msleep(unsigned long msecs)
{
    ::Sleep(msecs);
}

void QThread::usleep(unsigned long usecs)
{
    ::Sleep((usecs / 1000) + 1);
}


void QThread::start(Priority priority)
{
    Q_D(QThread);
    QMutexLocker locker(&d->mutex);

    if (d->running)
        return;

    d->running = true;
    d->finished = false;
    d->terminated = false;

    /*
      NOTE: we create the thread in the suspended state, set the
      priority and then resume the thread.

      since threads are created with normal priority by default, we
      could get into a case where a thread (with priority less than
      NormalPriority) tries to create a new thread (also with priority
      less than NormalPriority), but the newly created thread preempts
      its 'parent' and runs at normal priority.
    */
    d->handle = (Qt::HANDLE) _beginthreadex(NULL, d->stackSize, QThreadPrivate::start,
                                            this, CREATE_SUSPENDED, &(d->id));

    if (!d->handle) {
        qErrnoWarning(errno, "QThread::start: Failed to create thread");
        d->running = false;
        d->finished = true;
        return;
    }

    // Since Win 9x will have problems if the priority is idle or time critical
    // we have to use the closest one instead
    int prio;
    d->priority = priority;
    switch (d->priority) {
    case IdlePriority:
	if (QSysInfo::WindowsVersion & QSysInfo::WV_DOS_based) {
	    prio = THREAD_PRIORITY_LOWEST;
	} else {
	    prio = THREAD_PRIORITY_IDLE;
	}
        break;

    case LowestPriority:
        prio = THREAD_PRIORITY_LOWEST;
        break;

    case LowPriority:
        prio = THREAD_PRIORITY_BELOW_NORMAL;
        break;

    case NormalPriority:
        prio = THREAD_PRIORITY_NORMAL;
        break;

    case HighPriority:
        prio = THREAD_PRIORITY_ABOVE_NORMAL;
        break;

    case HighestPriority:
        prio = THREAD_PRIORITY_HIGHEST;
        break;

    case TimeCriticalPriority:
	if (QSysInfo::WindowsVersion & QSysInfo::WV_DOS_based) {
	    prio = THREAD_PRIORITY_HIGHEST;
	} else {
	    prio = THREAD_PRIORITY_TIME_CRITICAL;
	}
        break;

    case InheritPriority:
    default:
        prio = GetThreadPriority(GetCurrentThread());
        break;
    }

    if (!SetThreadPriority(d->handle, prio)) {
        qErrnoWarning("QThread::start: Failed to set thread priority");
    }

    if (ResumeThread(d->handle) == (DWORD) -1) {
        qErrnoWarning("QThread::start: Failed to resume new thread");
    }
}

void QThread::terminate()
{
    Q_D(QThread);
    QMutexLocker locker(&d->mutex);
    if (!d->running)
        return;
    if (!d->terminationEnabled) {
        d->terminatePending = true;
        return;
    }
    TerminateThread(d->handle, 0);
    d->terminated = true;
    QThreadPrivate::finish(this, false);
}

bool QThread::wait(unsigned long time)
{
    Q_D(QThread);
    QMutexLocker locker(&d->mutex);

    if (d->id == GetCurrentThreadId()) {
        qWarning("QThread::wait: Thread tried to wait on itself");
        return false;
    }
    if (d->finished || !d->running)
        return true;

    ++d->waiters;
    locker.mutex()->unlock();

    bool ret = false;
    switch (WaitForSingleObject(d->handle, time)) {
    case WAIT_OBJECT_0:
        ret = true;
        break;
    case WAIT_FAILED:
        qErrnoWarning("QThread::wait: Thread wait failure");
        break;
    case WAIT_ABANDONED:
    case WAIT_TIMEOUT:
    default:
        break;
    }

    locker.mutex()->lock();
    --d->waiters;

    if (ret && !d->finished) {
        // thread was terminated by someone else
        d->terminated = true;
        QThreadPrivate::finish(this, false);
    }

    if (d->finished && !d->waiters) {
        CloseHandle(d->handle);
        d->handle = 0;
    }

    return ret;
}

void QThread::setTerminationEnabled(bool enabled)
{
    QThread *thr = currentThread();
    Q_ASSERT_X(thr != 0, "QThread::setTerminationEnabled()",
               "Current thread was not started with QThread.");
    QThreadPrivate *d = thr->d_func();
    QMutexLocker locker(&d->mutex);
    d->terminationEnabled = enabled;
    if (enabled && d->terminatePending) {
        d->terminated = true;
        QThreadPrivate::finish(thr, false);
        locker.unlock(); // don't leave the mutex locked!
        _endthreadex(0);
    }
}

void QThread::setPriority(Priority priority)
{
    Q_D(QThread);
    QMutexLocker locker(&d->mutex);
    if (!d->running) {
        qWarning("QThread::setPriority: Cannot set priority, thread is not running");
        return;
    }

    // copied from start() with a few modifications:

    // Since Win 9x will have problems if the priority is idle or time critical
    // we have to use the closest one instead
    int prio;
    d->priority = priority;
    switch (d->priority) {
    case IdlePriority:
	if (QSysInfo::WindowsVersion & QSysInfo::WV_DOS_based) {
	    prio = THREAD_PRIORITY_LOWEST;
	} else {
	    prio = THREAD_PRIORITY_IDLE;
	}
        break;

    case LowestPriority:
        prio = THREAD_PRIORITY_LOWEST;
        break;

    case LowPriority:
        prio = THREAD_PRIORITY_BELOW_NORMAL;
        break;

    case NormalPriority:
        prio = THREAD_PRIORITY_NORMAL;
        break;

    case HighPriority:
        prio = THREAD_PRIORITY_ABOVE_NORMAL;
        break;

    case HighestPriority:
        prio = THREAD_PRIORITY_HIGHEST;
        break;

    case TimeCriticalPriority:
	if (QSysInfo::WindowsVersion & QSysInfo::WV_DOS_based) {
	    prio = THREAD_PRIORITY_HIGHEST;
	} else {
	    prio = THREAD_PRIORITY_TIME_CRITICAL;
	}
        break;

    case InheritPriority:
    default:
        qWarning("QThread::setPriority: Argument cannot be InheritPriority");
        return;
    }

    if (!SetThreadPriority(d->handle, prio)) {
        qErrnoWarning("QThread::setPriority: Failed to set thread priority");
    }
}

QT_END_NAMESPACE
#endif // QT_NO_THREAD
