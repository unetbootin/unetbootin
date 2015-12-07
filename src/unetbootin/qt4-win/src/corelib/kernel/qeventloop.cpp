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

#include "qeventloop.h"

#include "qabstracteventdispatcher.h"
#include "qcoreapplication.h"
#include "qdatetime.h"

#include "qobject_p.h"
#include <private/qthread_p.h>

QT_BEGIN_NAMESPACE

class QEventLoopPrivate : public QObjectPrivate
{
    Q_DECLARE_PUBLIC(QEventLoop)
public:
    inline QEventLoopPrivate()
        : exit(true), inExec(false), returnCode(-1)
    { }
    bool exit, inExec;
    int returnCode;
};

/*!
    \class QEventLoop
    \brief The QEventLoop class provides a means of entering and leaving an event loop.

    At any time, you can create a QEventLoop object and call exec()
    on it to start a local event loop. From within the event loop,
    calling exit() will force exec() to return.

    \sa QAbstractEventDispatcher
*/

/*!
    \enum QEventLoop::ProcessEventsFlag

    This enum controls the types of events processed by the
    processEvents() functions.

    \value AllEvents All events. Note that
    \l{QEvent::DeferredDelete}{DeferredDelete} events are processed
    specially. See QObject::deleteLater() for more details.

    \value ExcludeUserInputEvents Do not process user input events,
    such as ButtonPress and KeyPress. Note that the events are not
    discarded; they will be delivered the next time processEvents() is
    called without the ExcludeUserInputEvents flag.

    \value ExcludeSocketNotifiers Do not process socket notifier
    events. Note that the events are not discarded; they will be
    delivered the next time processEvents() is called without the
    ExcludeSocketNotifiers flag.

    \value WaitForMoreEvents Wait for events if no pending events are
    available.

    \omitvalue X11ExcludeTimers
    \omitvalue ExcludeUserInput
    \omitvalue WaitForMore
    \value DeferredDeletion deprecated - do not use.

    \sa processEvents()
*/

/*!
    Constructs an event loop object with the given \a parent.
*/
QEventLoop::QEventLoop(QObject *parent)
    : QObject(*new QEventLoopPrivate, parent)
{
    Q_D(QEventLoop);
    if (!QCoreApplication::instance()) {
        qWarning("QEventLoop: Cannot be used without QApplication");
    } else if (!d->threadData->eventDispatcher) {
        QThreadPrivate::createEventDispatcher(d->threadData);
    }
}

/*!
    Destroys the event loop object.
*/
QEventLoop::~QEventLoop()
{ }


/*!
    Processes pending events that match \a flags until there are no
    more events to process. Returns true if pending events were handled;
    otherwise returns false.

    This function is especially useful if you have a long running
    operation and want to show its progress without allowing user
    input; i.e. by using the \l ExcludeUserInputEvents flag.

    This function is simply a wrapper for
    QAbstractEventDispatcher::processEvents(). See the documentation
    for that function for details.
*/
bool QEventLoop::processEvents(ProcessEventsFlags flags)
{
    Q_D(QEventLoop);
    if (!d->threadData->eventDispatcher)
        return false;
    if (flags & DeferredDeletion)
        QCoreApplication::sendPostedEvents(0, QEvent::DeferredDelete);
    return d->threadData->eventDispatcher->processEvents(flags);
}

/*!
    Enters the main event loop and waits until exit() is called.
    Returns the value that was passed to exit().

    If \a flags are specified, only events of the types allowed by
    the \a flags will be processed.

    It is necessary to call this function to start event handling. The
    main event loop receives events from the window system and
    dispatches these to the application widgets.

    Generally speaking, no user interaction can take place before
    calling exec(). As a special case, modal widgets like QMessageBox
    can be used before calling exec(), because modal widgets
    use their own local event loop.

    To make your application perform idle processing (i.e. executing a
    special function whenever there are no pending events), use a
    QTimer with 0 timeout. More sophisticated idle processing schemes
    can be achieved using processEvents().

    \sa QApplication::quit(), exit(), processEvents()
*/
int QEventLoop::exec(ProcessEventsFlags flags)
{
    Q_D(QEventLoop);
    if (d->threadData->quitNow)
        return -1;

    if (d->inExec) {
        qWarning("QEventLoop::exec: instance %p has already called exec()", this);
        return -1;
    }
    d->inExec = true;
    d->exit = false;
    ++d->threadData->loopLevel;
    d->threadData->eventLoops.push(this);

    // remove posted quit events when entering a new event loop
    if (qApp->thread() == thread())
        QCoreApplication::removePostedEvents(qApp, QEvent::Quit);

#if defined(QT_NO_EXCEPTIONS)
    while (!d->exit)
        processEvents(flags | WaitForMoreEvents);
#else
    try {
        while (!d->exit)
            processEvents(flags | WaitForMoreEvents);
    } catch (...) {
        qWarning("Qt has caught an exception thrown from an event handler. Throwing\n"
                 "exceptions from an event handler is not supported in Qt. You must\n"
                 "reimplement QApplication::notify() and catch all exceptions there.\n");
        throw;
    }
#endif

    QEventLoop *eventLoop = d->threadData->eventLoops.pop();
    Q_ASSERT_X(eventLoop == this, "QEventLoop::exec()", "internal error");
    Q_UNUSED(eventLoop); // --release warning

    d->inExec = false;
    --d->threadData->loopLevel;

    return d->returnCode;
}

/*!
    Process pending events that match \a flags for a maximum of \a
    maxTime milliseconds, or until there are no more events to
    process, whichever is shorter.
    This function is especially useful if you have a long running
    operation and want to show its progress without allowing user
    input, i.e. by using the \l ExcludeUserInputEvents flag.

    \bold{Notes:}
    \list
    \o This function does not process events continuously; it
       returns after all available events are processed.
    \o Specifying the \l WaitForMoreEvents flag makes no sense
       and will be ignored.
    \endlist
*/
void QEventLoop::processEvents(ProcessEventsFlags flags, int maxTime)
{
    Q_D(QEventLoop);
    if (!d->threadData->eventDispatcher)
        return;

    QTime start;
    start.start();
    if (flags & DeferredDeletion)
        QCoreApplication::sendPostedEvents(0, QEvent::DeferredDelete);
    while (processEvents(flags & ~WaitForMoreEvents)) {
        if (start.elapsed() > maxTime)
            break;
        if (flags & DeferredDeletion)
            QCoreApplication::sendPostedEvents(0, QEvent::DeferredDelete);
    }
}

/*!
    Tells the event loop to exit with a return code.

    After this function has been called, the event loop returns from
    the call to exec(). The exec() function returns \a returnCode.

    By convention, a \a returnCode of 0 means success, and any non-zero
    value indicates an error.

    Note that unlike the C library function of the same name, this
    function \e does return to the caller -- it is event processing that
    stops.

    \sa QCoreApplication::quit(), quit(), exec()
*/
void QEventLoop::exit(int returnCode)
{
    Q_D(QEventLoop);
    if (!d->threadData->eventDispatcher)
        return;

    d->returnCode = returnCode;
    d->exit = true;
    d->threadData->eventDispatcher->interrupt();
}

/*!
    Returns true if the event loop is running; otherwise returns
    false. The event loop is considered running from the time when
    exec() is called until exit() is called.

    \sa exec() exit()
 */
bool QEventLoop::isRunning() const
{
    Q_D(const QEventLoop);
    return !d->exit;
}

/*!
    Wakes up the event loop.

    \sa QAbstractEventDispatcher::wakeUp()
*/
void QEventLoop::wakeUp()
{
    Q_D(QEventLoop);
    if (!d->threadData->eventDispatcher)
        return;
    d->threadData->eventDispatcher->wakeUp();
}

/*!
    Tells the event loop to exit normally.

    Same as exit(0).

    \sa QCoreApplication::quit(), exit()
*/
void QEventLoop::quit()
{ exit(0); }

QT_END_NAMESPACE
