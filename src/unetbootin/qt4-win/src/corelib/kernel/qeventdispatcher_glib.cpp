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

#include "qeventdispatcher_glib_p.h"
#include "qeventdispatcher_unix_p.h"

#include <private/qthread_p.h>

#include "qcoreapplication.h"
#include "qsocketnotifier.h"

#include <QtCore/qhash.h>
#include <QtCore/qlist.h>
#include <QtCore/qpair.h>

#include <glib.h>

QT_BEGIN_NAMESPACE

struct GPollFDWithQSocketNotifier
{
    GPollFD pollfd;
    QSocketNotifier *socketNotifier;
};

struct GSocketNotifierSource
{
    GSource source;
    QList<GPollFDWithQSocketNotifier *> pollfds;
};

static gboolean socketNotifierSourcePrepare(GSource *, gint *timeout)
{
    if (timeout)
        *timeout = -1;
    return false;
}

static gboolean socketNotifierSourceCheck(GSource *source)
{
    GSocketNotifierSource *src = reinterpret_cast<GSocketNotifierSource *>(source);

    bool pending = false;
    for (int i = 0; !pending && i < src->pollfds.count(); ++i) {
        GPollFDWithQSocketNotifier *p = src->pollfds.at(i);

        if (p->pollfd.revents & G_IO_NVAL) {
            // disable the invalid socket notifier
            static const char *t[] = { "Read", "Write", "Exception" };
            qWarning("QSocketNotifier: Invalid socket %d and type '%s', disabling...",
                     p->pollfd.fd, t[int(p->socketNotifier->type())]);
            // ### note, modifies src->pollfds!
            p->socketNotifier->setEnabled(false);
        }

        pending = ((p->pollfd.revents & p->pollfd.events) != 0);
    }

    return pending;
}

static gboolean socketNotifierSourceDispatch(GSource *source, GSourceFunc, gpointer)
{
    QEvent event(QEvent::SockAct);

    GSocketNotifierSource *src = reinterpret_cast<GSocketNotifierSource *>(source);
    for (int i = 0; i < src->pollfds.count(); ++i) {
        GPollFDWithQSocketNotifier *p = src->pollfds.at(i);

        if ((p->pollfd.revents & p->pollfd.events) != 0)
            QCoreApplication::sendEvent(p->socketNotifier, &event);
    }

    return true; // ??? don't remove, right?
}

static GSourceFuncs socketNotifierSourceFuncs = {
    socketNotifierSourcePrepare,
    socketNotifierSourceCheck,
    socketNotifierSourceDispatch,
    NULL,
    NULL,
    NULL
};

struct GTimerSource
{
    GSource source;
    QTimerInfoList timerList;
    QEventLoop::ProcessEventsFlags processEventsFlags;
};

static gboolean timerSourcePrepare(GSource *source, gint *timeout)
{
    gint dummy;
    if (!timeout)
        timeout = &dummy;

    GTimerSource *src = reinterpret_cast<GTimerSource *>(source);

    timeval tv = { 0l, 0l };
    if (!(src->processEventsFlags & QEventLoop::X11ExcludeTimers) && src->timerList.timerWait(tv))
        *timeout = (tv.tv_sec * 1000) + (tv.tv_usec / 1000);
    else
        *timeout = -1;

    return (*timeout == 0);
}

static gboolean timerSourceCheck(GSource *source)
{
    GTimerSource *src = reinterpret_cast<GTimerSource *>(source);

    if (src->timerList.isEmpty()
        || (src->processEventsFlags & QEventLoop::X11ExcludeTimers))
        return false;

    if (src->timerList.updateCurrentTime() < src->timerList.first()->timeout)
        return false;

    return true;
}

static gboolean timerSourceDispatch(GSource *source, GSourceFunc, gpointer)
{
    (void) reinterpret_cast<GTimerSource *>(source)->timerList.activateTimers();
    return true; // ??? don't remove, right again?
}

static GSourceFuncs timerSourceFuncs = {
    timerSourcePrepare,
    timerSourceCheck,
    timerSourceDispatch,
    NULL,
    NULL,
    NULL
};

struct GPostEventSource
{
    GSource source;
    QAtomicInt serialNumber;
    int lastSerialNumber;
};

static gboolean postEventSourcePrepare(GSource *s, gint *timeout)
{
    QThreadData *data = QThreadData::current();
    if (!data)
        return false;

    gint dummy;
    if (!timeout)
        timeout = &dummy;
    *timeout = data->canWait ? -1 : 0;

    GPostEventSource *source = reinterpret_cast<GPostEventSource *>(s);
    return (!data->canWait
            || (source->serialNumber != source->lastSerialNumber));
}

static gboolean postEventSourceCheck(GSource *source)
{
    return postEventSourcePrepare(source, 0);
}

static gboolean postEventSourceDispatch(GSource *s, GSourceFunc, gpointer)
{
    GPostEventSource *source = reinterpret_cast<GPostEventSource *>(s);
    source->lastSerialNumber = source->serialNumber;
    QCoreApplication::sendPostedEvents();
    return true; // i dunno, george...
}

static GSourceFuncs postEventSourceFuncs = {
    postEventSourcePrepare,
    postEventSourceCheck,
    postEventSourceDispatch,
    NULL,
    NULL,
    NULL
};


QEventDispatcherGlibPrivate::QEventDispatcherGlibPrivate(GMainContext *context)
    : mainContext(context)
{
    if (qgetenv("QT_NO_THREADED_GLIB").isEmpty()) {
        if (!g_thread_supported())
            g_thread_init(NULL);
    }

    if (mainContext) {
        g_main_context_ref(mainContext);
    } else {
        QCoreApplication *app = QCoreApplication::instance();
	if (app && QThread::currentThread() == app->thread()) {
	    mainContext = g_main_context_default();
	    g_main_context_ref(mainContext);
	} else {
	    mainContext = g_main_context_new();
	}
    }

    postEventSource = reinterpret_cast<GPostEventSource *>(g_source_new(&postEventSourceFuncs,
                                                                        sizeof(GPostEventSource)));
    g_source_set_can_recurse(&postEventSource->source, true);
    g_source_attach(&postEventSource->source, mainContext);

    // setup socketNotifierSource
    socketNotifierSource =
        reinterpret_cast<GSocketNotifierSource *>(g_source_new(&socketNotifierSourceFuncs,
                                                               sizeof(GSocketNotifierSource)));
    (void) new (&socketNotifierSource->pollfds) QList<GPollFDWithQSocketNotifier *>();
    g_source_set_can_recurse(&socketNotifierSource->source, true);
    g_source_attach(&socketNotifierSource->source, mainContext);

    // setup timerSource
    timerSource = reinterpret_cast<GTimerSource *>(g_source_new(&timerSourceFuncs,
                                                                sizeof(GTimerSource)));
    (void) new (&timerSource->timerList) QTimerInfoList();
    timerSource->processEventsFlags = QEventLoop::AllEvents;
    g_source_set_can_recurse(&timerSource->source, true);
    g_source_attach(&timerSource->source, mainContext);
}

QEventDispatcherGlib::QEventDispatcherGlib(QObject *parent)
    : QAbstractEventDispatcher(*(new QEventDispatcherGlibPrivate), parent)
{
}

QEventDispatcherGlib::QEventDispatcherGlib(GMainContext *mainContext,
					   QObject *parent)
    : QAbstractEventDispatcher(*(new QEventDispatcherGlibPrivate(mainContext)),
			       parent)
{
}

QEventDispatcherGlib::~QEventDispatcherGlib()
{
    Q_D(QEventDispatcherGlib);

    // destroy all timer sources
    qDeleteAll(d->timerSource->timerList);
    d->timerSource->timerList.~QTimerInfoList();
    g_source_destroy(&d->timerSource->source);
    g_source_unref(&d->timerSource->source);
    d->timerSource = 0;

    // destroy socket notifier source
    for (int i = 0; i < d->socketNotifierSource->pollfds.count(); ++i) {
        GPollFDWithQSocketNotifier *p = d->socketNotifierSource->pollfds[i];
        g_source_remove_poll(&d->socketNotifierSource->source, &p->pollfd);
        delete p;
    }
    d->socketNotifierSource->pollfds.~QList<GPollFDWithQSocketNotifier *>();
    g_source_destroy(&d->socketNotifierSource->source);
    g_source_unref(&d->socketNotifierSource->source);
    d->socketNotifierSource = 0;

    // destroy post event source
    g_source_destroy(&d->postEventSource->source);
    g_source_unref(&d->postEventSource->source);
    d->postEventSource = 0;

    Q_ASSERT(d->mainContext != 0);
    g_main_context_unref(d->mainContext);
    d->mainContext = 0;
}

bool QEventDispatcherGlib::processEvents(QEventLoop::ProcessEventsFlags flags)
{
    Q_D(QEventDispatcherGlib);

    const bool canWait = (flags & QEventLoop::WaitForMoreEvents);
    if (canWait)
        emit aboutToBlock();
    else
        emit awake();

    // tell postEventSourcePrepare() and timerSource about any new flags
    QEventLoop::ProcessEventsFlags savedFlags = d->timerSource->processEventsFlags;
    d->timerSource->processEventsFlags = flags;
    
    bool result = g_main_context_iteration(d->mainContext, canWait);
    while (!result && canWait)
        result = g_main_context_iteration(d->mainContext, canWait);
    
    d->timerSource->processEventsFlags = savedFlags;

    if (canWait)
        emit awake();

    return result;
}

bool QEventDispatcherGlib::hasPendingEvents()
{
    Q_D(QEventDispatcherGlib);
    return g_main_context_pending(d->mainContext);
}

void QEventDispatcherGlib::registerSocketNotifier(QSocketNotifier *notifier)
{
    Q_ASSERT(notifier);
    int sockfd = notifier->socket();
    int type = notifier->type();
#ifndef QT_NO_DEBUG
    if (sockfd < 0) {
        qWarning("QSocketNotifier: Internal error");
        return;
    } else if (notifier->thread() != thread()
               || thread() != QThread::currentThread()) {
        qWarning("QSocketNotifier: socket notifiers cannot be enabled from another thread");
        return;
    }
#endif

    Q_D(QEventDispatcherGlib);


    GPollFDWithQSocketNotifier *p = new GPollFDWithQSocketNotifier;
    p->pollfd.fd = sockfd;
    switch (type) {
    case QSocketNotifier::Read:
        p->pollfd.events = G_IO_IN | G_IO_HUP | G_IO_ERR;
        break;
    case QSocketNotifier::Write:
        p->pollfd.events = G_IO_OUT | G_IO_ERR;
        break;
    case QSocketNotifier::Exception:
        p->pollfd.events = G_IO_PRI | G_IO_ERR;
        break;
    }
    p->socketNotifier = notifier;

    d->socketNotifierSource->pollfds.append(p);

    g_source_add_poll(&d->socketNotifierSource->source, &p->pollfd);
}

void QEventDispatcherGlib::unregisterSocketNotifier(QSocketNotifier *notifier)
{
    Q_ASSERT(notifier);
#ifndef QT_NO_DEBUG
    int sockfd = notifier->socket();
    if (sockfd < 0) {
        qWarning("QSocketNotifier: Internal error");
        return;
    } else if (notifier->thread() != thread()
               || thread() != QThread::currentThread()) {
        qWarning("QSocketNotifier: socket notifiers cannot be disabled from another thread");
        return;
    }
#endif

    Q_D(QEventDispatcherGlib);

    for (int i = 0; i < d->socketNotifierSource->pollfds.count(); ++i) {
        GPollFDWithQSocketNotifier *p = d->socketNotifierSource->pollfds.at(i);
        if (p->socketNotifier == notifier) {
            // found it
            g_source_remove_poll(&d->socketNotifierSource->source, &p->pollfd);

            d->socketNotifierSource->pollfds.removeAt(i);
            delete p;

            return;
        }
    }
}

void QEventDispatcherGlib::registerTimer(int timerId, int interval, QObject *object)
{
#ifndef QT_NO_DEBUG
    if (timerId < 1 || interval < 0 || !object) {
        qWarning("QEventDispatcherUNIX::registerTimer: invalid arguments");
        return;
    } else if (object->thread() != thread() || thread() != QThread::currentThread()) {
        qWarning("QObject::startTimer: timers cannot be started from another thread");
        return;
    }
#endif

    Q_D(QEventDispatcherGlib);
    d->timerSource->timerList.registerTimer(timerId, interval, object);
}

bool QEventDispatcherGlib::unregisterTimer(int timerId)
{
#ifndef QT_NO_DEBUG
    if (timerId < 1) {
        qWarning("QEventDispatcherUNIX::unregisterTimer: invalid argument");
        return false;
    } else if (thread() != QThread::currentThread()) {
        qWarning("QObject::killTimer: timers cannot be stopped from another thread");
        return false;
    }
#endif

    Q_D(QEventDispatcherGlib);
    return d->timerSource->timerList.unregisterTimer(timerId);
}

bool QEventDispatcherGlib::unregisterTimers(QObject *object)
{
#ifndef QT_NO_DEBUG
    if (!object) {
        qWarning("QEventDispatcherUNIX::unregisterTimers: invalid argument");
        return false;
    } else if (object->thread() != thread() || thread() != QThread::currentThread()) {
        qWarning("QObject::killTimers: timers cannot be stopped from another thread");
        return false;
    }
#endif

    Q_D(QEventDispatcherGlib);
    return d->timerSource->timerList.unregisterTimers(object);
}

QList<QEventDispatcherGlib::TimerInfo> QEventDispatcherGlib::registeredTimers(QObject *object) const
{
    if (!object) {
        qWarning("QEventDispatcherUNIX:registeredTimers: invalid argument");
        return QList<TimerInfo>();
    }

    Q_D(const QEventDispatcherGlib);
    return d->timerSource->timerList.registeredTimers(object);
}

void QEventDispatcherGlib::interrupt()
{
    wakeUp();
}

void QEventDispatcherGlib::wakeUp()
{
    Q_D(QEventDispatcherGlib);
    d->postEventSource->serialNumber.ref();
    g_main_context_wakeup(d->mainContext);
}

void QEventDispatcherGlib::flush()
{
}

bool QEventDispatcherGlib::versionSupported()
{
#if !defined(GLIB_MAJOR_VERSION) || !defined(GLIB_MINOR_VERSION) || !defined(GLIB_MICRO_VERSION)
    return false;
#else
    return ((GLIB_MAJOR_VERSION << 16) + (GLIB_MINOR_VERSION << 8) + GLIB_MICRO_VERSION) >= 0x020301;
#endif
}

QEventDispatcherGlib::QEventDispatcherGlib(QEventDispatcherGlibPrivate &dd, QObject *parent)
    : QAbstractEventDispatcher(dd, parent)
{
}

QT_END_NAMESPACE
