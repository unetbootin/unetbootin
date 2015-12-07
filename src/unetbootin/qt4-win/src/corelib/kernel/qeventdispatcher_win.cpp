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

#include "qeventdispatcher_win_p.h"

#include "qcoreapplication.h"
#include "qhash.h"
#include "qlibrary.h"
#include "qpair.h"
#include "qset.h"
#include "qsocketnotifier.h"
#include "qvarlengtharray.h"
#include "qwineventnotifier_p.h"

#include "qabstracteventdispatcher_p.h"
#include "qcoreapplication_p.h"
#include <private/qthread_p.h>
#include <private/qmutexpool_p.h>

QT_BEGIN_NAMESPACE

HINSTANCE qWinAppInst();
extern uint qGlobalPostedEventsCount();

#ifndef TIME_KILL_SYNCHRONOUS
#  define TIME_KILL_SYNCHRONOUS 0x0100
#endif

#if defined(Q_OS_WINCE)
QT_BEGIN_INCLUDE_NAMESPACE
#include <winsock.h>
// Asynchronous Winsocks ------------------------------------------
#ifndef QT_NO_THREAD
#include <qthread.h>
#include <qmap.h>
#include <qmutex.h>
QT_END_INCLUDE_NAMESPACE

//#define QCE_ASYNC_DEBUG

namespace {
    class SocketAsyncHandler;

    class SocketAsyncHandler : public QThread
    {
    public:
        SocketAsyncHandler();
        ~SocketAsyncHandler();
        void run();
        void select(SOCKET sock, HWND handle, unsigned int msg, long ev);
        void removeSelect(SOCKET sock);
        void safeRemove(SOCKET sock);
    private:
        struct SockInfo {
            HWND handle;
            unsigned int msg;
            long ev;
        };
        QMap<SOCKET, SockInfo> sockets;
        QMutex mutex;
        QWaitCondition cond;
        bool supposedToDie;
    };

    SocketAsyncHandler::SocketAsyncHandler()
        : supposedToDie(false)
    {
    }

    SocketAsyncHandler::~SocketAsyncHandler()
    {
        mutex.lock();
        supposedToDie = true;
        mutex.unlock();
        cond.wakeOne();
        wait();
        while (sockets.size() > 0)
            removeSelect(sockets.begin().key());
    }

    void SocketAsyncHandler::removeSelect(SOCKET sock)
    {
        if (!sockets.contains(sock))
            return;
        sockets.remove(sock);
        return;
    }

    void SocketAsyncHandler::safeRemove(SOCKET sock)
    {
        QMutexLocker locker(&mutex);
        removeSelect(sock);
    }

    void SocketAsyncHandler::select(SOCKET sock, HWND handle, unsigned int msg, long ev)
    {
        QMutexLocker locker(&mutex);

        if (sockets.contains(sock))
            sockets.remove(sock);

        SockInfo info;
        info.handle = handle;
        info.msg = msg;
        info.ev = ev;
        sockets.insert(sock, info);
        cond.wakeOne();
    }

    void SocketAsyncHandler::run()
    {
        do {
            mutex.lock();

            while (!supposedToDie && sockets.isEmpty()) {
                cond.wait(&mutex);
            }

            if (supposedToDie) {
                mutex.unlock();
                break;
            }

            // Copy current items to reduce lock time
            // and to be able to use SendMessage
            QMap<SOCKET, SockInfo> currentSockets = sockets;
            mutex.unlock();

            fd_set readS, writeS, exS;
            FD_ZERO(&readS);
            FD_ZERO(&writeS);
            FD_ZERO(&exS);

            int maxFd = 0;

            for (QMap<SOCKET, SockInfo>::iterator it = currentSockets.begin(); it != currentSockets.end(); ++it) {
                const SockInfo &info = it.value();
                int socket = it.key();
                maxFd = qMax(maxFd, socket);

                if ((info.ev & FD_READ) || (info.ev & FD_CLOSE) || (info.ev & FD_ACCEPT))
                    FD_SET(socket, &readS);
                if ((info.ev & FD_WRITE)|| (info.ev & FD_CONNECT))
                    FD_SET(socket, &writeS);
                if (info.ev & FD_OOB)
                    FD_SET(socket, &exS);
            }

            timeval timeout;
            timeout.tv_sec = 0;
            timeout.tv_usec = 50000;
            int result = ::select(maxFd + 1, &readS, &writeS, &exS, &timeout);
            if (result > 0) {
                HWND handle;
                unsigned int tmpMsg;
                SOCKET sock;
                HRESULT ret;
                for (QMap<SOCKET, SockInfo>::const_iterator it = currentSockets.constBegin();
                    it != currentSockets.constEnd(); ++it) {
                        handle = (*it).handle;
                        tmpMsg = (*it).msg;
                        sock = it.key();
                        if (FD_ISSET(sock, &readS))
                            ret = SendMessage(handle, tmpMsg, sock, FD_READ);

                        if (FD_ISSET(sock, &writeS))
                            ret = SendMessage(handle, tmpMsg, sock, FD_WRITE);

                        if (FD_ISSET(sock, &exS))
                            ret = SendMessage(handle, tmpMsg, sock, FD_OOB);
                }
            }

#ifdef QCE_ASYNC_DEBUG
            else if (result == 0) { //timeout
                qDebug("    WSAAsync select timeout");
            } else if (result < 0) { // SocketError
                // This might happen because of two reasons
                // 1. We already closed a socket in between the copy and the select
                //    and thus select() returns an error
                // 2. Something is really wrong, then
                //    ### Loop on all descriptors, try to select and remove the
                //    ### broken one.
                qWarning("WSAAsync select error %d", WSAGetLastError());
            }
#endif
        } while(true);
    }
} // namespace

Q_GLOBAL_STATIC(SocketAsyncHandler, qt_async_handler)

int WSAAsyncSelect(SOCKET sock, HWND handle, unsigned int msg, long ev)
{
    if (sock == 0 || handle == 0 || handle == INVALID_HANDLE_VALUE) {
        WSASetLastError(WSAEINVAL);
        return SOCKET_ERROR;
    }

    if (msg == 0 && ev == 0)
        qt_async_handler()->safeRemove(sock);
    else
        qt_async_handler()->select(sock, handle, msg, ev);

    qt_async_handler()->start(QThread::LowPriority);
    WSASetLastError(0);
    return 0;
}
#else // QT_NO_THREAD
int WSAAsyncSelect(SOCKET, HWND, unsigned int, long)
{
    return SOCKET_ERROR;
}
#endif
#endif // Q_OS_WINCE

class QEventDispatcherWin32Private;

struct QSockNot {
    QSocketNotifier *obj;
    int fd;
};
typedef QHash<int, QSockNot *> QSNDict;

struct WinTimerInfo {                           // internal timer info
    QObject *dispatcher;
    int timerId;
    int interval;
    QObject *obj;                               // - object to receive events
    bool inTimerEvent;
    int fastTimerId;
};

class QZeroTimerEvent : public QEvent
{
public:
    int timerid;
    QZeroTimerEvent(int id) : QEvent(QEvent::ZeroTimerEvent), timerid(id) {}
};

typedef QList<WinTimerInfo*>  WinTimerVec;      // vector of TimerInfo structs
typedef QHash<int, WinTimerInfo*> WinTimerDict; // fast dict of timers

#if !defined(DWORD_PTR) && !defined(Q_WS_WIN64)
#define DWORD_PTR DWORD
#endif

typedef MMRESULT(WINAPI *ptimeSetEvent)(UINT, UINT, LPTIMECALLBACK, DWORD_PTR, UINT);
typedef MMRESULT(WINAPI *ptimeKillEvent)(UINT);

static ptimeSetEvent qtimeSetEvent = 0;
static ptimeKillEvent qtimeKillEvent = 0;

static void resolveTimerAPI()
{
    static bool triedResolve = false;
    if (!triedResolve) {
#ifndef QT_NO_THREAD
        QMutexLocker locker(QMutexPool::globalInstanceGet(&triedResolve));
        if (triedResolve)
            return;
#endif
        triedResolve = true;
#if !defined(Q_OS_WINCE)
        qtimeSetEvent = (ptimeSetEvent)QLibrary::resolve(QLatin1String("winmm"), "timeSetEvent");
        qtimeKillEvent = (ptimeKillEvent)QLibrary::resolve(QLatin1String("winmm"), "timeKillEvent");
#else
        qtimeSetEvent = (ptimeSetEvent)QLibrary::resolve(QLatin1String("Mmtimer"), "timeSetEvent");
        qtimeKillEvent = (ptimeKillEvent)QLibrary::resolve(QLatin1String("Mmtimer"), "timeKillEvent");
#endif
    }
}


class QEventDispatcherWin32Private : public QAbstractEventDispatcherPrivate
{
    Q_DECLARE_PUBLIC(QEventDispatcherWin32)
public:
    QEventDispatcherWin32Private();
    ~QEventDispatcherWin32Private();

    DWORD threadId;

    bool interrupt;

    // internal window handle used for socketnotifiers/timers/etc
    HWND internalHwnd;

    // timers
    WinTimerVec timerVec;
    WinTimerDict timerDict;
    void registerTimer(WinTimerInfo *t);
    void unregisterTimer(WinTimerInfo *t);
    void sendTimerEvent(int timerId);

    // socket notifiers
    QSNDict sn_read;
    QSNDict sn_write;
    QSNDict sn_except;
    void doWsaAsyncSelect(int socket);

    // event notifier
    QWinEventNotifier wakeUpNotifier;

    QList<QWinEventNotifier *> winEventNotifierList;
    void activateEventNotifier(QWinEventNotifier * wen);

    QList<MSG> queuedUserInputEvents;
    QList<MSG> queuedSocketEvents;
};

QEventDispatcherWin32Private::QEventDispatcherWin32Private()
    : threadId(GetCurrentThreadId()), interrupt(false), internalHwnd(0)
{
    resolveTimerAPI();

    wakeUpNotifier.setHandle(QT_WA_INLINE(CreateEventW(0, FALSE, FALSE, 0),
                                          CreateEventA(0, FALSE, FALSE, 0)));
    if (!wakeUpNotifier.handle())
        qWarning("QEventDispatcher: Creating QEventDispatcherWin32Private wakeup event failed");
}

QEventDispatcherWin32Private::~QEventDispatcherWin32Private()
{
    wakeUpNotifier.setEnabled(false);
    CloseHandle(wakeUpNotifier.handle());
    if (internalHwnd)
        DestroyWindow(internalHwnd);
    QByteArray className = "QEventDispatcherWin32_Internal_Widget" + QByteArray::number(quintptr(qt_internal_proc));
#if !defined(Q_OS_WINCE)
    UnregisterClassA(className.constData(), qWinAppInst());
#else
	UnregisterClassW(reinterpret_cast<const wchar_t *> (QString::fromLatin1(className.constData()).utf16()), qWinAppInst());
#endif
}

void QEventDispatcherWin32Private::activateEventNotifier(QWinEventNotifier * wen)
{
    QEvent event(QEvent::WinEventAct);
    QCoreApplication::sendEvent(wen, &event);
}


Q_CORE_EXPORT bool winPeekMessage(MSG* msg, HWND hWnd, UINT wMsgFilterMin,
                     UINT wMsgFilterMax, UINT wRemoveMsg)
{
    QT_WA({ return PeekMessage(msg, hWnd, wMsgFilterMin, wMsgFilterMax, wRemoveMsg); } ,
          { return PeekMessageA(msg, hWnd, wMsgFilterMin, wMsgFilterMax, wRemoveMsg); });
}

Q_CORE_EXPORT bool winPostMessage(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    QT_WA({ return PostMessage(hWnd, msg, wParam, lParam); } ,
          { return PostMessageA(hWnd, msg, wParam, lParam); });
}

Q_CORE_EXPORT bool winGetMessage(MSG* msg, HWND hWnd, UINT wMsgFilterMin,
                     UINT wMsgFilterMax)
{
    QT_WA({ return GetMessage(msg, hWnd, wMsgFilterMin, wMsgFilterMax); } ,
          { return GetMessageA(msg, hWnd, wMsgFilterMin, wMsgFilterMax); });
}

// This function is called by a workerthread
void WINAPI CALLBACK qt_fast_timer_proc(uint timerId, uint /*reserved*/, DWORD_PTR user, DWORD_PTR /*reserved*/, DWORD_PTR /*reserved*/)
{
    if (!timerId) // sanity check
        return;
    WinTimerInfo *t = (WinTimerInfo*)user;
    Q_ASSERT(t);
    QCoreApplication::postEvent(t->dispatcher, new QTimerEvent(t->timerId));
}

LRESULT CALLBACK qt_internal_proc(HWND hwnd, UINT message, WPARAM wp, LPARAM lp)
{
    if (message == WM_NCCREATE) {
            return true;
    } else if (message == WM_USER) {

        // socket notifier message
        MSG msg;
        msg.hwnd = hwnd;
        msg.message = message;
        msg.wParam = wp;
        msg.lParam = lp;

        QCoreApplication *app = QCoreApplication::instance();
        long result;
        if (app && app->filterEvent(&msg, &result))
            return result;

        int type = -1;
        switch (WSAGETSELECTEVENT(lp)) {
        case FD_READ:
        case FD_CLOSE:
        case FD_ACCEPT:
            type = 0;
            break;
        case FD_WRITE:
        case FD_CONNECT:
            type = 1;
            break;
        case FD_OOB:
            type = 2;
            break;
        }
        if (type >= 0) {

    #ifdef GWLP_USERDATA
            QEventDispatcherWin32 *eventDispatcher =
                (QEventDispatcherWin32 *) GetWindowLongPtrA(hwnd, GWLP_USERDATA);
    #else
            QEventDispatcherWin32 *eventDispatcher =
                (QEventDispatcherWin32 *) GetWindowLongA(hwnd, GWL_USERDATA);
    #endif
            if (eventDispatcher) {
                QEventDispatcherWin32Private *d = eventDispatcher->d_func();
                QSNDict *sn_vec[3] = { &d->sn_read, &d->sn_write, &d->sn_except };
                QSNDict *dict = sn_vec[type];

                QSockNot *sn = dict ? dict->value(wp) : 0;
                if (sn) {
                    QEvent event(QEvent::SockAct);
                    QCoreApplication::sendEvent(sn->obj, &event);
                }
            }
        }
        return 0;

    } else if (message == WM_TIMER) {

        MSG msg;
        msg.hwnd = hwnd;
        msg.message = message;
        msg.wParam = wp;
        msg.lParam = lp;

        QCoreApplication *app = QCoreApplication::instance();
        Q_ASSERT_X(app, "qt_interal_proc", "Timer fired, but no QCoreApplication");
        if (!app) {
            KillTimer(hwnd, wp);
            return 0;
        }

        long result;
        if (app->filterEvent(&msg, &result))
            return result;

        QEventDispatcherWin32 *eventDispatcher =
            qobject_cast<QEventDispatcherWin32 *>(QAbstractEventDispatcher::instance());
        Q_ASSERT(eventDispatcher != 0);
        QEventDispatcherWin32Private *d = eventDispatcher->d_func();
        d->sendTimerEvent(wp);
        return 0;
    }

    return  DefWindowProc(hwnd, message, wp, lp);
}

static HWND qt_create_internal_window(const QEventDispatcherWin32 *eventDispatcher)
{
    HINSTANCE hi = qWinAppInst();
#if defined(Q_OS_WINCE)
    WNDCLASS wc;
#else
    WNDCLASSA wc;
#endif
    wc.style = 0;
    wc.lpfnWndProc = qt_internal_proc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = hi;
    wc.hIcon = 0;
    wc.hCursor = 0;
    wc.hbrBackground = 0;
    wc.lpszMenuName = NULL;

    // make sure that multiple Qt's can coexist in the same process
    QByteArray className = "QEventDispatcherWin32_Internal_Widget" + QByteArray::number(quintptr(qt_internal_proc));
#if defined(Q_OS_WINCE)
    QString tmp = QString::fromLatin1(className.data());
    wc.lpszClassName = reinterpret_cast<const wchar_t *> (tmp.utf16());
    RegisterClass(&wc);
    HWND wnd = CreateWindow(wc.lpszClassName,  // classname
                             wc.lpszClassName,  // window name
                             0,                 // style
                             0, 0, 0, 0,        // geometry
                             0,                 // parent
                             0,                 // menu handle
                             hi,                // application
                             0);                // windows creation data.
#else
    wc.lpszClassName = className.constData();
    RegisterClassA(&wc);
    HWND wnd = CreateWindowA(wc.lpszClassName,  // classname
                             wc.lpszClassName,  // window name
                             0,                 // style
                             0, 0, 0, 0,        // geometry
                             0,                 // parent
                             0,                 // menu handle
                             hi,                // application
                             0);                // windows creation data.
#endif


#ifdef GWLP_USERDATA
    SetWindowLongPtrA(wnd, GWLP_USERDATA, (LONG_PTR)eventDispatcher);
#else
    SetWindowLongA(wnd, GWL_USERDATA, (LONG)eventDispatcher);
#endif

    if (!wnd) {
        qWarning("QEventDispatcher: Failed to create QEventDispatcherWin32 internal window: %d\n", (int)GetLastError());
    }
    return wnd;
}

void QEventDispatcherWin32Private::registerTimer(WinTimerInfo *t)
{
    Q_ASSERT(internalHwnd);

    Q_Q(QEventDispatcherWin32);

    int ok = 0;

    if (t->interval > 10 || !t->interval || !qtimeSetEvent) {
        ok = 1;
        if (!t->interval)  // optimization for single-shot-zero-timer
            QCoreApplication::postEvent(q, new QZeroTimerEvent(t->timerId));
        else
            ok = SetTimer(internalHwnd, t->timerId, (uint) t->interval, 0);
    } else {
        ok = t->fastTimerId = qtimeSetEvent(t->interval, 1, qt_fast_timer_proc, (DWORD_PTR)t,
                                            TIME_CALLBACK_FUNCTION | TIME_PERIODIC | TIME_KILL_SYNCHRONOUS);
        if (ok == 0) { // fall back to normal timer if no more multimedia timers avaiable
            ok = SetTimer(internalHwnd, t->timerId, (uint) t->interval, 0);
        }
    }

    if (ok == 0)
        qErrnoWarning("QEventDispatcherWin32::registerTimer: Failed to create a timer");
}

void QEventDispatcherWin32Private::unregisterTimer(WinTimerInfo *t)
{
    if (t->fastTimerId != 0) {
        qtimeKillEvent(t->fastTimerId);
        QCoreApplicationPrivate::removePostedTimerEvent(t->obj, t->timerId);
    } else if (internalHwnd) {
        KillTimer(internalHwnd, t->timerId);
    }
    delete t;
}

void QEventDispatcherWin32Private::sendTimerEvent(int timerId)
{
    WinTimerInfo *t = timerDict.value(timerId);
    if (t && !t->inTimerEvent) {
        // send event, but don't allow it to recurse
        t->inTimerEvent = true;

        QTimerEvent e(t->timerId);
        QCoreApplication::sendEvent(t->obj, &e);

        // timer could have been removed
        t = timerDict.value(timerId);
        if (t) {
            t->inTimerEvent = false;
        }
    }
}

void QEventDispatcherWin32Private::doWsaAsyncSelect(int socket)
{
    Q_ASSERT(internalHwnd);
    int sn_event = 0;
    if (sn_read.contains(socket))
        sn_event |= FD_READ | FD_CLOSE | FD_ACCEPT;
    if (sn_write.contains(socket))
        sn_event |= FD_WRITE | FD_CONNECT;
    if (sn_except.contains(socket))
        sn_event |= FD_OOB;
    // BoundsChecker may emit a warning for WSAAsyncSelect when sn_event == 0
    // This is a BoundsChecker bug and not a Qt bug
    WSAAsyncSelect(socket, internalHwnd, sn_event ? WM_USER : 0, sn_event);
}

void QEventDispatcherWin32::createInternalHwnd()
{
    Q_D(QEventDispatcherWin32);

    Q_ASSERT(!d->internalHwnd);
    if (d->internalHwnd)
        return;
    d->internalHwnd = qt_create_internal_window(this);

    // register all socket notifiers
    QList<int> sockets = (d->sn_read.keys().toSet()
                          + d->sn_write.keys().toSet()
                          + d->sn_except.keys().toSet()).toList();
    for (int i = 0; i < sockets.count(); ++i)
        d->doWsaAsyncSelect(sockets.at(i));

    // start all normal timers
    for (int i = 0; i < d->timerVec.count(); ++i)
        d->registerTimer(d->timerVec.at(i));
}

QEventDispatcherWin32::QEventDispatcherWin32(QObject *parent)
    : QAbstractEventDispatcher(*new QEventDispatcherWin32Private, parent)
{
}

QEventDispatcherWin32::~QEventDispatcherWin32()
{
}

bool QEventDispatcherWin32::processEvents(QEventLoop::ProcessEventsFlags flags)
{
    Q_D(QEventDispatcherWin32);

    if (!d->internalHwnd)
        createInternalHwnd();

    d->interrupt = false;
    emit awake();

    bool canWait;
    bool retVal = false;
    do {
        QCoreApplicationPrivate::sendPostedEvents(0, 0, d->threadData);

        DWORD waitRet = 0;
        HANDLE pHandles[MAXIMUM_WAIT_OBJECTS - 1];
        QVarLengthArray<MSG> processedTimers;
        while (!d->interrupt) {
            DWORD nCount = d->winEventNotifierList.count();
            Q_ASSERT(nCount < MAXIMUM_WAIT_OBJECTS - 1);

            MSG msg;
            bool haveMessage;

            if (!(flags & QEventLoop::ExcludeUserInputEvents) && !d->queuedUserInputEvents.isEmpty()) {
                // process queued user input events
                haveMessage = true;
                msg = d->queuedUserInputEvents.takeFirst();
            } else if(!(flags & QEventLoop::ExcludeSocketNotifiers) && !d->queuedSocketEvents.isEmpty()) {
                // process queued socket events
                haveMessage = true;
                msg = d->queuedSocketEvents.takeFirst();
            } else {
                haveMessage = winPeekMessage(&msg, 0, 0, 0, PM_REMOVE);
                if (haveMessage && (flags & QEventLoop::ExcludeUserInputEvents)
                    && ((msg.message >= WM_KEYFIRST
                         && msg.message <= WM_KEYLAST)
                        || (msg.message >= WM_MOUSEFIRST
                            && msg.message <= WM_MOUSELAST)
                        || msg.message == WM_MOUSEWHEEL)) {
                    // queue user input events for later processing
                    haveMessage = false;
                    d->queuedUserInputEvents.append(msg);
                }
                if (haveMessage && (flags & QEventLoop::ExcludeSocketNotifiers)
                    && (msg.message == WM_USER && msg.hwnd == d->internalHwnd)) {
                    // queue socket events for later processing
                    haveMessage = false;
                    d->queuedSocketEvents.append(msg);
                }
            }
            if (!haveMessage) {
                // no message - check for signalled objects
                for (int i=0; i<(int)nCount; i++)
                    pHandles[i] = d->winEventNotifierList.at(i)->handle();
                waitRet = MsgWaitForMultipleObjectsEx(nCount, pHandles, 0, QS_ALLINPUT, MWMO_ALERTABLE);
                if ((haveMessage = (waitRet == WAIT_OBJECT_0 + nCount))) {
                    // a new message has arrived, process it
                    continue;
                }
            }
            if (haveMessage) {
                if (msg.message == WM_TIMER) {
                    // avoid live-lock by keeping track of the timers we've already sent
                    bool found = false;
                    for (int i = 0; !found && i < processedTimers.count(); ++i) {
                        const MSG processed = processedTimers.constData()[i];
                        found = (processed.wParam == msg.wParam && processed.hwnd == msg.hwnd && processed.lParam == msg.lParam);
                    }
                    if (found)
                        continue;
                    processedTimers.append(msg);
                } else if (msg.message == WM_QUIT) {
                    if (QCoreApplication::instance())
                        QCoreApplication::instance()->quit();
                    return false;
                }

                if (!filterEvent(&msg)) {
                    TranslateMessage(&msg);
                    QT_WA({
                        DispatchMessage(&msg);
                    } , {
                        DispatchMessageA(&msg);
                    });
                }
            } else if (waitRet >= WAIT_OBJECT_0 && waitRet < WAIT_OBJECT_0 + nCount) {
                d->activateEventNotifier(d->winEventNotifierList.at(waitRet - WAIT_OBJECT_0));
            } else {
                // nothing todo so break
                break;
            }
            retVal = true;
        }

        // still nothing - wait for message or signalled objects
        QThreadData *data = d->threadData;
        canWait = (!retVal
                   && data->canWait
                   && !d->interrupt
                   && (flags & QEventLoop::WaitForMoreEvents));
        if (canWait) {
            DWORD nCount = d->winEventNotifierList.count();
            Q_ASSERT(nCount < MAXIMUM_WAIT_OBJECTS - 1);
            for (int i=0; i<(int)nCount; i++)
                pHandles[i] = d->winEventNotifierList.at(i)->handle();

            emit aboutToBlock();
            waitRet = MsgWaitForMultipleObjectsEx(nCount, pHandles, INFINITE, QS_ALLINPUT, MWMO_ALERTABLE);
            emit awake();
            if (waitRet >= WAIT_OBJECT_0 && waitRet < WAIT_OBJECT_0 + nCount) {
                d->activateEventNotifier(d->winEventNotifierList.at(waitRet - WAIT_OBJECT_0));
                retVal = true;
            }
        }
    } while (canWait);

    return retVal;
}

bool QEventDispatcherWin32::hasPendingEvents()
{
    MSG msg;
    return qGlobalPostedEventsCount() || winPeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE);
}

void QEventDispatcherWin32::registerSocketNotifier(QSocketNotifier *notifier)
{
    Q_ASSERT(notifier);
    int sockfd = notifier->socket();
    int type = notifier->type();
#ifndef QT_NO_DEBUG
    if (sockfd < 0) {
        qWarning("QSocketNotifier: Internal error");
        return;
    } else if (notifier->thread() != thread() || thread() != QThread::currentThread()) {
        qWarning("QSocketNotifier: socket notifiers cannot be enabled from another thread");
        return;
    }
#endif

    Q_D(QEventDispatcherWin32);
    QSNDict *sn_vec[3] = { &d->sn_read, &d->sn_write, &d->sn_except };
    QSNDict *dict = sn_vec[type];

    if (QCoreApplication::closingDown()) // ### d->exitloop?
        return; // after sn_cleanup, don't reinitialize.

    if (dict->contains(sockfd)) {
        const char *t[] = { "Read", "Write", "Exception" };
	/* Variable "socket" below is a function pointer. */
        qWarning("QSocketNotifier: Multiple socket notifiers for "
                 "same socket %d and type %s", sockfd, t[type]);
    }

    QSockNot *sn = new QSockNot;
    sn->obj = notifier;
    sn->fd  = sockfd;
    dict->insert(sn->fd, sn);

    if (d->internalHwnd)
        d->doWsaAsyncSelect(sockfd);
}

void QEventDispatcherWin32::unregisterSocketNotifier(QSocketNotifier *notifier)
{
    Q_ASSERT(notifier);
    int sockfd = notifier->socket();
    int type = notifier->type();
#ifndef QT_NO_DEBUG
    if (sockfd < 0) {
        qWarning("QSocketNotifier: Internal error");
        return;
    } else if (notifier->thread() != thread() || thread() != QThread::currentThread()) {
        qWarning("QSocketNotifier: socket notifiers cannot be disabled from another thread");
        return;
    }
#endif

    Q_D(QEventDispatcherWin32);
    QSNDict *sn_vec[3] = { &d->sn_read, &d->sn_write, &d->sn_except };
    QSNDict *dict = sn_vec[type];
    QSockNot *sn = dict->value(sockfd);
    if (!sn)
        return;

    dict->remove(sockfd);
    delete sn;

    if (d->internalHwnd)
        d->doWsaAsyncSelect(sockfd);
}

void QEventDispatcherWin32::registerTimer(int timerId, int interval, QObject *object)
{
    if (timerId < 1 || interval < 0 || !object) {
        qWarning("QEventDispatcherWin32::registerTimer: invalid arguments");
        return;
    } else if (object->thread() != thread() || thread() != QThread::currentThread()) {
        qWarning("QObject::startTimer: timers cannot be started from another thread");
        return;
    }

    Q_D(QEventDispatcherWin32);

    register WinTimerInfo *t = new WinTimerInfo;
    t->dispatcher = this;
    t->timerId  = timerId;
    t->interval = interval;
    t->obj  = object;
    t->inTimerEvent = false;
    t->fastTimerId = 0;

    if (d->internalHwnd)
        d->registerTimer(t);

    d->timerVec.append(t);                      // store in timer vector
    d->timerDict.insert(t->timerId, t);          // store timers in dict
}

bool QEventDispatcherWin32::unregisterTimer(int timerId)
{
    if (timerId < 1) {
        qWarning("QEventDispatcherWin32::unregisterTimer: invalid argument");
        return false;
    }
    QThread *currentThread = QThread::currentThread();
    if (thread() != currentThread) {
        qWarning("QObject::killTimer: timers cannot be stopped from another thread");
        return false;
    }

    Q_D(QEventDispatcherWin32);
    if (d->timerVec.isEmpty() || timerId <= 0)
        return false;

    WinTimerInfo *t = d->timerDict.value(timerId);
    if (!t)
        return false;

    d->timerDict.remove(t->timerId);
    d->timerVec.removeAll(t);
    d->unregisterTimer(t);
    return true;
}

bool QEventDispatcherWin32::unregisterTimers(QObject *object)
{
    if (!object) {
        qWarning("QEventDispatcherWin32::unregisterTimers: invalid argument");
        return false;
    }
    QThread *currentThread = QThread::currentThread();
    if (object->thread() != thread() || thread() != currentThread) {
        qWarning("QObject::killTimers: timers cannot be stopped from another thread");
        return false;
    }

    Q_D(QEventDispatcherWin32);
    if (d->timerVec.isEmpty())
        return false;
    register WinTimerInfo *t;
    for (int i=0; i<d->timerVec.size(); i++) {
        t = d->timerVec.at(i);
        if (t && t->obj == object) {                // object found
            d->timerDict.remove(t->timerId);
            d->timerVec.removeAt(i);
            d->unregisterTimer(t);
            --i;
        }
    }
    return true;
}

QList<QEventDispatcherWin32::TimerInfo>
QEventDispatcherWin32::registeredTimers(QObject *object) const
{
    if (!object) {
        qWarning("QEventDispatcherWin32:registeredTimers: invalid argument");
        return QList<TimerInfo>();
    }

    Q_D(const QEventDispatcherWin32);
    QList<TimerInfo> list;
    for (int i = 0; i < d->timerVec.size(); ++i) {
        const WinTimerInfo *t = d->timerVec.at(i);
        if (t && t->obj == object)
            list << TimerInfo(t->timerId, t->interval);
    }
    return list;
}

bool QEventDispatcherWin32::registerEventNotifier(QWinEventNotifier *notifier)
{
    if (!notifier) {
        qWarning("QWinEventNotifier: Internal error");
        return false;
    } else if (notifier->thread() != thread() || thread() != QThread::currentThread()) {
        qWarning("QWinEventNotifier: event notifiers cannot be enabled from another thread");
        return false;
    }

    Q_D(QEventDispatcherWin32);

    if (d->winEventNotifierList.contains(notifier))
        return true;

    if (d->winEventNotifierList.count() >= MAXIMUM_WAIT_OBJECTS - 2) {
        qWarning("QWinEventNotifier: Cannot have more than %d enabled at one time", MAXIMUM_WAIT_OBJECTS - 2);
        return false;
    }
    d->winEventNotifierList.append(notifier);
    return true;
}

void QEventDispatcherWin32::unregisterEventNotifier(QWinEventNotifier *notifier)
{
    if (!notifier) {
        qWarning("QWinEventNotifier: Internal error");
        return;
    } else if (notifier->thread() != thread() || thread() != QThread::currentThread()) {
        qWarning("QWinEventNotifier: event notifiers cannot be disabled from another thread");
        return;
    }

    Q_D(QEventDispatcherWin32);

    int i = d->winEventNotifierList.indexOf(notifier);
    if (i != -1)
        d->winEventNotifierList.takeAt(i);
}

void QEventDispatcherWin32::activateEventNotifiers()
{
    Q_D(QEventDispatcherWin32);
    //### this could break if events are removed/added in the activation
    for (int i=0; i<d->winEventNotifierList.count(); i++) {
#if !defined(Q_OS_WINCE)
        if (WaitForSingleObjectEx(d->winEventNotifierList.at(i)->handle(), 0, TRUE) == WAIT_OBJECT_0)
            d->activateEventNotifier(d->winEventNotifierList.at(i));
#else
		if (WaitForSingleObject(d->winEventNotifierList.at(i)->handle(), 0) == WAIT_OBJECT_0)
            d->activateEventNotifier(d->winEventNotifierList.at(i));
#endif
    }
}

void QEventDispatcherWin32::wakeUp()
{
    Q_D(QEventDispatcherWin32);
    SetEvent(d->wakeUpNotifier.handle());
}

void QEventDispatcherWin32::interrupt()
{
    Q_D(QEventDispatcherWin32);
    d->interrupt = true;
    wakeUp();
}

void QEventDispatcherWin32::flush()
{ }


void QEventDispatcherWin32::startingUp()
{
    Q_D(QEventDispatcherWin32);

    if (d->wakeUpNotifier.handle()) d->wakeUpNotifier.setEnabled(true);
}

void QEventDispatcherWin32::closingDown()
{
    Q_D(QEventDispatcherWin32);

    // clean up any socketnotifiers
    while (!d->sn_read.isEmpty())
        unregisterSocketNotifier((*(d->sn_read.begin()))->obj);
    while (!d->sn_write.isEmpty())
        unregisterSocketNotifier((*(d->sn_write.begin()))->obj);
    while (!d->sn_except.isEmpty())
        unregisterSocketNotifier((*(d->sn_except.begin()))->obj);

    // clean up any timers
    while (!d->timerDict.isEmpty())
        unregisterTimer((*(d->timerDict.begin()))->timerId);
}

bool QEventDispatcherWin32::event(QEvent *e)
{
    Q_D(QEventDispatcherWin32);
    if (e->type() == QEvent::ZeroTimerEvent) {
        QZeroTimerEvent *zte = static_cast<QZeroTimerEvent*>(e);
        WinTimerInfo *t = d->timerDict.value(zte->timerid);
        if (t) {
            QTimerEvent te(zte->timerid);
            QCoreApplication::sendEvent(t->obj, &te);
            WinTimerInfo *tn = d->timerDict.value(zte->timerid);
            if (tn && t == tn)
                QCoreApplication::postEvent(this, new QZeroTimerEvent(zte->timerid));
        }
        return true;
    } else if (e->type() == QEvent::Timer) {
        QTimerEvent *te = static_cast<QTimerEvent*>(e);
        d->sendTimerEvent(te->timerId());
    }
    return QAbstractEventDispatcher::event(e);
}

QT_END_NAMESPACE
