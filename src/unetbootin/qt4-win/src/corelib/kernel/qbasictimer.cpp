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

#include "qbasictimer.h"
#include "qcoreapplication.h"
#include "qabstracteventdispatcher.h"

QT_BEGIN_NAMESPACE

/*!
    \class QBasicTimer
    \brief The QBasicTimer class provides timer events for objects.

    \ingroup time
    \ingroup events

    This is a fast, lightweight, and low-level class used by Qt
    internally. We recommend using the higher-level QTimer class
    rather than this class if you want to use timers in your
    applications.

    To use this class, create a QBasicTimer, and call its start()
    function with a timeout interval and with a pointer to a QObject
    subclass. When the timer times out it will send a timer event to
    the QObject subclass. The timer can be stopped at any time using
    stop(). isActive() returns true for a timer that is running;
    i.e. it has been started, has not reached the timeout time, and
    has not been stopped. The timer's ID can be retrieved using
    timerId().

    The \l{widgets/wiggly}{Wiggly} example uses QBasicTimer to repaint
    a widget at regular intervals.

    \sa QTimer, QTimerEvent, QObject::timerEvent(), Timers, {Wiggly Example}
*/


/*!
    \fn QBasicTimer::QBasicTimer()

    Contructs a basic timer.

    \sa start()
*/
/*!
    \fn QBasicTimer::~QBasicTimer()

    Destroys the basic timer.
*/

/*!
    \fn bool QBasicTimer::isActive() const

    Returns true if the timer is running, has not yet timed
    out, and has not been stopped; otherwise returns false.

    \sa start() stop()
*/

/*!
    \fn int QBasicTimer::timerId() const

    Returns the timer's ID.

    \sa QTimerEvent::timerId()
*/

/*!
    \fn void QBasicTimer::start(int msec, QObject *object)

    Starts (or restarts) the timer with a \a msec milliseconds
    timeout.

    The given \a object will receive timer events.

    \sa stop() isActive() QObject::timerEvent()
 */
void QBasicTimer::start(int msec, QObject *obj)
{
   stop();
   if (obj)
       id = obj->startTimer(msec);
}

/*!
    Stops the timer.

    \sa start() isActive()
*/
void QBasicTimer::stop()
{
    if (id) {
        QAbstractEventDispatcher *eventDispatcher = QAbstractEventDispatcher::instance();
        if (eventDispatcher)
            eventDispatcher->unregisterTimer(id);
    }
    id = 0;
}

QT_END_NAMESPACE
