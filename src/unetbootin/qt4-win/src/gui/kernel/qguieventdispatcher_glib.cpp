/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the QtGui module of the Qt Toolkit.
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

#include "qguieventdispatcher_glib_p.h"

#include "qapplication.h"
#include "qx11info_x11.h"

#include "qt_x11_p.h"

#include <glib.h>

QT_BEGIN_NAMESPACE

struct GX11EventSource
{
    GSource source;
    GPollFD pollfd;
    QEventLoop::ProcessEventsFlags flags;
    QGuiEventDispatcherGlib *q;
    QGuiEventDispatcherGlibPrivate *d;
};

class QGuiEventDispatcherGlibPrivate : public QEventDispatcherGlibPrivate
{
    Q_DECLARE_PUBLIC(QGuiEventDispatcherGlib)

public:
    QGuiEventDispatcherGlibPrivate();
    GX11EventSource *x11EventSource;
    QList<XEvent> queuedUserInputEvents;
};

static gboolean x11EventSourcePrepare(GSource *s, gint *timeout)
{
    if (timeout)
        *timeout = -1;
    GX11EventSource *source = reinterpret_cast<GX11EventSource *>(s);
    return (XEventsQueued(X11->display, QueuedAfterFlush)
            || (!(source->flags & QEventLoop::ExcludeUserInputEvents)
                && !source->d->queuedUserInputEvents.isEmpty()));
}

static gboolean x11EventSourceCheck(GSource *s)
{
    GX11EventSource *source = reinterpret_cast<GX11EventSource *>(s);
    return (XEventsQueued(X11->display, QueuedAfterFlush)
            || (!(source->flags & QEventLoop::ExcludeUserInputEvents)
                && !source->d->queuedUserInputEvents.isEmpty()));
}

static gboolean x11EventSourceDispatch(GSource *s, GSourceFunc callback, gpointer user_data)
{
    GX11EventSource *source = reinterpret_cast<GX11EventSource *>(s);

    ulong marker = XNextRequest(X11->display);
    do {
        XEvent event;
        if (!(source->flags & QEventLoop::ExcludeUserInputEvents)
            && !source->d->queuedUserInputEvents.isEmpty()) {
            // process a pending user input event
            event = source->d->queuedUserInputEvents.takeFirst();
        } else if (XEventsQueued(X11->display, QueuedAlready)) {
            // process events from the X server
            XNextEvent(X11->display, &event);

            if (source->flags & QEventLoop::ExcludeUserInputEvents) {
                // queue user input events
                switch (event.type) {
                case ButtonPress:
                case ButtonRelease:
                case MotionNotify:
                case XKeyPress:
                case XKeyRelease:
                case EnterNotify:
                case LeaveNotify:
                    source->d->queuedUserInputEvents.append(event);
                    continue;

                case ClientMessage:
                    // only keep the wm_take_focus and
                    // _qt_scrolldone protocols, queue all other
                    // client messages
                    if (event.xclient.format == 32) {
                        if (event.xclient.message_type == ATOM(WM_PROTOCOLS) ||
                            (Atom) event.xclient.data.l[0] == ATOM(WM_TAKE_FOCUS)) {
                            break;
                        } else if (event.xclient.message_type == ATOM(_QT_SCROLL_DONE)) {
                            break;
                        }
                    }
                    source->d->queuedUserInputEvents.append(event);
                    continue;

                default:
                    break;
                }
            }
        } else {
            // no event to process
            break;
        }

        // send through event filter
        if (source->q->filterEvent(&event))
            continue;

        if (qApp->x11ProcessEvent(&event) == 1)
            return true;

        if (event.xany.serial >= marker)
            goto out;
    } while (XEventsQueued(X11->display, QueuedAfterFlush));

 out:

    if (callback)
        callback(user_data);
    return true;
}

static GSourceFuncs x11EventSourceFuncs = {
    x11EventSourcePrepare,
    x11EventSourceCheck,
    x11EventSourceDispatch,
    NULL,
    NULL,
    NULL
};

QGuiEventDispatcherGlibPrivate::QGuiEventDispatcherGlibPrivate()
{
    x11EventSource = reinterpret_cast<GX11EventSource *>(g_source_new(&x11EventSourceFuncs,
                                                                      sizeof(GX11EventSource)));
    g_source_set_can_recurse(&x11EventSource->source, true);

    memset(&x11EventSource->pollfd, 0, sizeof(GPollFD));
    x11EventSource->flags = QEventLoop::AllEvents;
    x11EventSource->q = 0;
    x11EventSource->d = 0;

    g_source_attach(&x11EventSource->source, mainContext);
}

QGuiEventDispatcherGlib::QGuiEventDispatcherGlib(QObject *parent)
    : QEventDispatcherGlib(*new QGuiEventDispatcherGlibPrivate, parent)
{
}

QGuiEventDispatcherGlib::~QGuiEventDispatcherGlib()
{
    Q_D(QGuiEventDispatcherGlib);

    g_source_remove_poll(&d->x11EventSource->source, &d->x11EventSource->pollfd);
    g_source_destroy(&d->x11EventSource->source);
    d->x11EventSource = 0;
}

bool QGuiEventDispatcherGlib::processEvents(QEventLoop::ProcessEventsFlags flags)
{
    Q_D(QGuiEventDispatcherGlib);
    QEventLoop::ProcessEventsFlags saved_flags = d->x11EventSource->flags;
    d->x11EventSource->flags = flags;
    bool returnValue = QEventDispatcherGlib::processEvents(flags);
    d->x11EventSource->flags = saved_flags;
    return returnValue;
}

void QGuiEventDispatcherGlib::startingUp()
{
    Q_D(QGuiEventDispatcherGlib);
    d->x11EventSource->pollfd.fd = XConnectionNumber(X11->display);
    d->x11EventSource->pollfd.events = G_IO_IN | G_IO_HUP | G_IO_ERR;
    d->x11EventSource->q = this;
    d->x11EventSource->d = d;
    g_source_add_poll(&d->x11EventSource->source, &d->x11EventSource->pollfd);
}

QT_END_NAMESPACE
