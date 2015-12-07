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

#include "qabstracteventdispatcher.h"
#include "qabstracteventdispatcher_p.h"

#include "qthread.h"
#include <private/qthread_p.h>
#include <private/qcoreapplication_p.h>

QT_BEGIN_NAMESPACE

// we allow for 2^24 = 8^8 = 16777216 simultaneously running timers
enum { NumberOfBuckets = 8, FirstBucketSize = 8 };

static const int BucketSize[NumberOfBuckets] =
    { 8, 64, 512, 4096, 32768, 262144, 2097152, 16777216 - 2396744 };
static const int BucketOffset[NumberOfBuckets] =
    { 0,  8,  72,  584,  4680,  37448,  299592,  2396744 };

static int FirstBucket[FirstBucketSize] = { 1, 2, 3, 4, 5, 6, 7, 8 };
static QBasicAtomicPointer<int> timerIds[NumberOfBuckets] =
    { Q_BASIC_ATOMIC_INITIALIZER(FirstBucket),
      Q_BASIC_ATOMIC_INITIALIZER(0),
      Q_BASIC_ATOMIC_INITIALIZER(0),
      Q_BASIC_ATOMIC_INITIALIZER(0),
      Q_BASIC_ATOMIC_INITIALIZER(0),
      Q_BASIC_ATOMIC_INITIALIZER(0),
      Q_BASIC_ATOMIC_INITIALIZER(0),
      Q_BASIC_ATOMIC_INITIALIZER(0) };

static void timerIdsDestructorFunction()
{
    // start at one, the first bucket is pre-allocated
    for (int i = 1; i < NumberOfBuckets; ++i)
        delete [] static_cast<int *>(timerIds[i]);
}
Q_DESTRUCTOR_FUNCTION(timerIdsDestructorFunction)

static QBasicAtomicInt nextFreeTimerId = Q_BASIC_ATOMIC_INITIALIZER(1);

// avoid the ABA-problem by using 7 of the top 8 bits of the timerId as a serial number
static inline int prepareNewValueWithSerialNumber(int oldId, int newId)
{
    return (newId & 0x00FFFFFF) | ((oldId + 0x01000000) & 0x7f000000);
}

static inline int bucketOffset(int timerId)
{
    for (int i = 0; i < NumberOfBuckets; ++i) {
        if (timerId < BucketSize[i])
            return i;
        timerId -= BucketSize[i];
    }
    qFatal("QAbstractEventDispatcher: INTERNAL ERROR, timer ID %d is too large", timerId);
    return -1;
}

static inline int bucketIndex(int bucket, int timerId)
{
    return timerId - BucketOffset[bucket];
}

static inline int *allocateBucket(int bucket)
{
    // allocate a new bucket
    const int size = BucketSize[bucket];
    const int offset = BucketOffset[bucket];
    int *b = new int[size];
    for (int i = 0; i != size; ++i)
        b[i] = offset + i + 1;
    return b;
}

void QAbstractEventDispatcherPrivate::init()
{
    Q_Q(QAbstractEventDispatcher);
    if (threadData->eventDispatcher != 0) {
        qWarning("QAbstractEventDispatcher: An event dispatcher has already been created for this thread");
    } else {
        threadData->eventDispatcher = q;
    }
}

int QAbstractEventDispatcherPrivate::allocateTimerId()
{
    int timerId, newTimerId;
    do {
        timerId = nextFreeTimerId;

        // which bucket are we looking in?
        int which = timerId & 0x00ffffff;
        int bucket = bucketOffset(which);
        int at = bucketIndex(bucket, which);
        int *b = timerIds[bucket];

        if (!b) {
            // allocate a new bucket
            b = allocateBucket(bucket);
            if (!timerIds[bucket].testAndSetRelease(0, b)) {
                // another thread won the race to allocate the bucket
                delete [] b;
                b = timerIds[bucket];
            }
        }

        newTimerId = b[at];
    } while (!nextFreeTimerId.testAndSetRelaxed(timerId, newTimerId));

    return timerId;
}

void QAbstractEventDispatcherPrivate::releaseTimerId(int timerId)
{
    int which = timerId & 0x00ffffff;
    int bucket = bucketOffset(which);
    int at = bucketIndex(bucket, which);
    int *b = timerIds[bucket];

    int freeId, newTimerId;
    do {
        freeId = nextFreeTimerId;
        b[at] = freeId & 0x00ffffff;

        newTimerId = prepareNewValueWithSerialNumber(freeId, timerId);
    } while (!nextFreeTimerId.testAndSetRelease(freeId, newTimerId));
}

/*!
    \class QAbstractEventDispatcher
    \brief The QAbstractEventDispatcher class provides an interface to manage Qt's event queue.

    \ingroup application
    \ingroup events

    An event dispatcher receives events from the window system and other
    sources. It then sends them to the QCoreApplication or QApplication
    instance for processing and delivery. QAbstractEventDispatcher provides
    fine-grained control over event delivery.

    For simple control of event processing use
    QCoreApplication::processEvents().

    For finer control of the application's event loop, call
    instance() and call functions on the QAbstractEventDispatcher
    object that is returned. If you want to use your own instance of
    QAbstractEventDispatcher or of a QAbstractEventDispatcher
    subclass, you must create your instance \e before you create the
    QApplication object.

    The main event loop is started by calling
    QCoreApplication::exec(), and stopped by calling
    QCoreApplication::exit(). Local event loops can be created using
    QEventLoop.

    Programs that perform long operations can call processEvents()
    with a bitwise OR combination of various QEventLoop::ProcessEventsFlag
    values to control which events should be delivered.

    QAbstractEventDispatcher also allows the integration of an
    external event loop with the Qt event loop. For example, the
    \l{Qt Solutions}{Motif Extension Qt Solution} includes a
    reimplementation of QAbstractEventDispatcher that merges Qt and
    Motif events together.

    \sa QEventLoop, QCoreApplication
*/

/*!
    Constructs a new event dispatcher with the given \a parent.
*/
QAbstractEventDispatcher::QAbstractEventDispatcher(QObject *parent)
    : QObject(*new QAbstractEventDispatcherPrivate, parent)
{
    Q_D(QAbstractEventDispatcher);
    d->init();
}

/*!
    \internal
*/
QAbstractEventDispatcher::QAbstractEventDispatcher(QAbstractEventDispatcherPrivate &dd,
                                                   QObject *parent)
    : QObject(dd, parent)
{
    Q_D(QAbstractEventDispatcher);
    d->init();
}

/*!
    Destroys the event dispatcher.
*/
QAbstractEventDispatcher::~QAbstractEventDispatcher()
{ }

/*!
    Returns a pointer to the event dispatcher object for the specified
    \a thread. If \a thread is zero, the current thread is used. If no
    event dispatcher exists for the specified thread, this function
    returns 0.

    \bold{Note:} If Qt is built without thread support, the \a thread
    argument is ignored.
 */
QAbstractEventDispatcher *QAbstractEventDispatcher::instance(QThread *thread)
{
    QThreadData *data = thread ? QThreadData::get2(thread) : QThreadData::current();
    return data->eventDispatcher;
}

/*!
    \fn bool QAbstractEventDispatcher::processEvents(QEventLoop::ProcessEventsFlags flags)

    Processes pending events that match \a flags until there are no
    more events to process. Returns true if an event was processed;
    otherwise returns false.

    This function is especially useful if you have a long running
    operation and want to show its progress without allowing user
    input; i.e. by using the QEventLoop::ExcludeUserInputEvents flag.

    If the QEventLoop::WaitForMoreEvents flag is set in \a flags, the
    behavior of this function is as follows:

    \list

    \i If events are available, this function returns after processing
    them.

    \i If no events are available, this function will wait until more
    are available and return after processing newly available events.

    \endlist

    If the QEventLoop::WaitForMoreEvents flag is not set in \a flags,
    and no events are available, this function will return
    immediately.

    \bold{Note:} This function does not process events continuously; it
    returns after all available events are processed.

    \sa hasPendingEvents()
*/

/*! \fn bool QAbstractEventDispatcher::hasPendingEvents()

    Returns true if there is an event waiting; otherwise returns
    false.
*/

/*!
    \fn void QAbstractEventDispatcher::registerSocketNotifier(QSocketNotifier *notifier)

    Registers \a notifier with the event loop. Subclasses must
    implement this method to tie a socket notifier into another
    event loop.
*/

/*! \fn void QAbstractEventDispatcher::unregisterSocketNotifier(QSocketNotifier *notifier)

    Unregisters \a notifier from the event dispatcher. Subclasses must
    reimplement this method to tie a socket notifier into another
    event loop. Reimplementations must call the base
    implementation.
*/

/*!
    \fn int QAbstractEventDispatcher::registerTimer(int interval, QObject *object)

    Registers a timer with the specified \a interval for the given \a object.
*/
int QAbstractEventDispatcher::registerTimer(int interval, QObject *object)
{
    int id = QAbstractEventDispatcherPrivate::allocateTimerId();
    registerTimer(id, interval, object);
    return id;
}

/*!
    \fn void QAbstractEventDispatcher::registerTimer(int timerId, int interval, QObject *object)

    Register a timer with the specified \a timerId and \a interval for
    the given \a object.
*/

/*!
    \fn bool QAbstractEventDispatcher::unregisterTimer(int timerId)

    Unregisters the timer with the given \a timerId.
    Returns true if successful; otherwise returns false.

    \sa registerTimer(), unregisterTimers()
*/

/*!
    \fn bool QAbstractEventDispatcher::unregisterTimers(QObject *object)

    Unregisters all the timers associated with the given \a object.
    Returns true if all timers were successful removed; otherwise returns false.

    \sa unregisterTimer(), registeredTimers()
*/

/*!
    \fn QList<TimerInfo> QAbstractEventDispatcher::registeredTimers(QObject *object) const

    Returns a list of registered timers for \a object. The timer ID
    is the first member in each pair; the interval is the second.
*/

/*! \fn void QAbstractEventDispatcher::wakeUp()
    \threadsafe

    Wakes up the event loop.

    \sa awake()
*/

/*!
    \fn void QAbstractEventDispatcher::interrupt()

    Interrupts event dispatching; i.e. the event dispatcher will
    return from processEvents() as soon as possible.
*/

/*! \fn void QAbstractEventDispatcher::flush()

    Flushes the event queue. This normally returns almost
    immediately. Does nothing on platforms other than X11.
*/

// ### DOC: Are these called when the _application_ starts/stops or just
// when the current _event loop_ starts/stops?
/*! \internal */
void QAbstractEventDispatcher::startingUp()
{ }

/*! \internal */
void QAbstractEventDispatcher::closingDown()
{ }

/*!
    \typedef QAbstractEventDispatcher::TimerInfo

    Typedef for QPair<int, int>. The first component of
    the pair is the timer ID; the second component is
    the interval.

    \sa registeredTimers()
*/

/*!
    \typedef QAbstractEventDispatcher::EventFilter

    Typedef for a function with the signature

    \snippet doc/src/snippets/code/src_corelib_kernel_qabstracteventdispatcher.cpp 0

    \sa setEventFilter(), filterEvent()
*/

/*!
    Sets the event filter \a filter. Returns a pointer to the filter
    function previously defined.

    The event filter is a function that receives all messages taken
    from the system event loop before the event is dispatched to the
    respective target. This includes messages that are not sent to Qt
    objects.

    The function can return true to stop the event to be processed by
    Qt, or false to continue with the standard event processing.

    Only one filter can be defined, but the filter can use the return
    value to call the previously set event filter. By default, no
    filter is set (i.e. the function returns 0).
*/
QAbstractEventDispatcher::EventFilter QAbstractEventDispatcher::setEventFilter(EventFilter filter)
{
    Q_D(QAbstractEventDispatcher);
    EventFilter oldFilter = d->event_filter;
    d->event_filter = filter;
    return oldFilter;
}

/*!
    Sends \a message through the event filter that was set by
    setEventFilter().  If no event filter has been set, this function
    returns false; otherwise, this function returns the result of the
    event filter function.

    Subclasses of QAbstractEventDispatcher \e must call this function
    for \e all messages received from the system to ensure
    compatibility with any extensions that may be used in the
    application.

    \sa setEventFilter()
*/
bool QAbstractEventDispatcher::filterEvent(void *message)
{
    Q_D(QAbstractEventDispatcher);
    if (d->event_filter)
        return d->event_filter(message);
    return false;
}

/*! \fn void QAbstractEventDispatcher::awake()

    This signal is emitted after the event loop returns from a
    function that could block.

    \sa wakeUp() aboutToBlock()
*/

/*! \fn void QAbstractEventDispatcher::aboutToBlock()

    This signal is emitted before the event loop calls a function that
    could block.

    \sa awake()
*/

QT_END_NAMESPACE
