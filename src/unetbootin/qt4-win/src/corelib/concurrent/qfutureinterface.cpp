/****************************************************************************
**
** Copyright (C) 2005-2008 Trolltech ASA. All rights reserved.
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

// qfutureinterface.h included from qfuture.h
#include "qfuture.h"

#ifndef QT_NO_QFUTURE

#include <QtCore/qatomic.h>
#include <QtCore/qthread.h>

#include "qfutureinterface_p.h"

QT_BEGIN_NAMESPACE

enum {
    MaxProgressEmitsPerSecond = 25
};

QFutureInterfaceBase::QFutureInterfaceBase(State initialState)
    : d(new QFutureInterfaceBasePrivate(initialState))
{ }

QFutureInterfaceBase::QFutureInterfaceBase(const QFutureInterfaceBase &other)
    : d(other.d)
{
    d->refCount.ref();
}

QFutureInterfaceBase::~QFutureInterfaceBase()
{
    if (!d->refCount.deref())
        delete d;
}

void QFutureInterfaceBase::cancel()
{
    QMutexLocker locker(&d->m_mutex);
    if (d->state & Canceled)
        return;

    d->state = State((d->state & ~Paused) | Canceled);
    d->waitCondition.wakeAll();
    d->pausedWaitCondition.wakeAll();
    d->sendCallOut(QFutureCallOutEvent(QFutureCallOutEvent::Canceled));
}

void QFutureInterfaceBase::setPaused(bool paused)
{
    QMutexLocker locker(&d->m_mutex);
    if (paused) {
        d->state = State(d->state | Paused);
        d->sendCallOut(QFutureCallOutEvent(QFutureCallOutEvent::Paused));
    } else {
        d->state = State(d->state & ~Paused);
        d->pausedWaitCondition.wakeAll();
        d->sendCallOut(QFutureCallOutEvent(QFutureCallOutEvent::Resumed));
    }
}

void QFutureInterfaceBase::togglePaused()
{
    QMutexLocker locker(&d->m_mutex);
    if (d->state & Paused) {
        d->state = State(d->state & ~Paused);
        d->pausedWaitCondition.wakeAll();
        d->sendCallOut(QFutureCallOutEvent(QFutureCallOutEvent::Resumed));
    } else {
        d->state = State(d->state | Paused);
        d->sendCallOut(QFutureCallOutEvent(QFutureCallOutEvent::Paused));
    }
}

void QFutureInterfaceBase::setThrottled(bool enable)
{
    // bail out if we are not changing the state
    if (enable && (d->state & Throttled) || !enable && !(d->state & Throttled))
        return;

    // lock and change the state
    QMutexLocker lock(&d->m_mutex);
    if (enable) {
        d->state  = State(d->state | Throttled);
    } else {
        d->state  = State(d->state & ~Throttled);
        if (!(d->state & Paused))
            d->pausedWaitCondition.wakeAll();
    }
}


bool QFutureInterfaceBase::isRunning() const
{
    return queryState(Running);
}

bool QFutureInterfaceBase::isStarted() const
{
    return queryState(Started);
}

bool QFutureInterfaceBase::isCanceled() const
{
    return queryState(Canceled);
}

bool QFutureInterfaceBase::isFinished() const
{
    return queryState(Finished);
}

bool QFutureInterfaceBase::isPaused() const
{
    return queryState(Paused);
}

bool QFutureInterfaceBase::isThrottled() const
{
    return queryState(Throttled);
}

bool QFutureInterfaceBase::isResultReadyAt(int index) const
{
    QMutexLocker lock(&d->m_mutex);
    return d->internal_isResultReadyAt(index);
}

bool QFutureInterfaceBase::waitForNextResult()
{
    QMutexLocker lock(&d->m_mutex);
    return d->internal_waitForNextResult();
}

void QFutureInterfaceBase::waitForResume()
{
    QMutexLocker lock(&d->m_mutex);
    if ((d->state & (Paused | Throttled)) == false || (d->state & Canceled))
        return;

    d->pausedWaitCondition.wait(&d->m_mutex);
}

int QFutureInterfaceBase::progressValue() const
{
    return d->m_progressValue;
}

int QFutureInterfaceBase::progressMinimum() const
{
    return d->m_progressMinimum;
}

int QFutureInterfaceBase::progressMaximum() const
{
    return d->m_progressMaximum;
}

int QFutureInterfaceBase::resultCount() const
{
    QMutexLocker lock(&d->m_mutex);
    return d->internal_resultCount();
}

QString QFutureInterfaceBase::progressText() const
{
    QMutexLocker locker(&d->m_mutex);
    return d->m_progressText;
}

bool QFutureInterfaceBase::isProgressUpdateNeeded() const
{
    QMutexLocker locker(&d->m_mutex);
    return (d->progressTime.elapsed() > (1000 / MaxProgressEmitsPerSecond));
}

void QFutureInterfaceBase::reportStarted()
{
    QMutexLocker locker(&d->m_mutex);
    if ((d->state & Started) || (d->state & Canceled) || (d->state & Finished))
        return;

    d->setState(State(Started | Running));
    d->sendCallOut(QFutureCallOutEvent(QFutureCallOutEvent::Started));
}

void QFutureInterfaceBase::reportCanceled()
{
    cancel();
}

#ifndef QT_NO_EXCEPTIONS
void QFutureInterfaceBase::reportException(const QtConcurrent::Exception &exception)
{
    QMutexLocker locker(&d->m_mutex);
    if ((d->state & Canceled) || (d->state & Finished))
        return;

    d->m_exceptionStore.setException(exception);
    d->state = State(d->state | Canceled);
    d->waitCondition.wakeAll();
    d->pausedWaitCondition.wakeAll();
    d->sendCallOut(QFutureCallOutEvent(QFutureCallOutEvent::Canceled));
}
#endif

void QFutureInterfaceBase::reportFinished()
{
    QMutexLocker locker(&d->m_mutex);
    if (!(d->state & Finished)) {
        d->state = State((d->state & ~Running) | Finished);
        d->waitCondition.wakeAll();
        d->sendCallOut(QFutureCallOutEvent(QFutureCallOutEvent::Finished));
    }
}

void QFutureInterfaceBase::setExpectedResultCount(int resultCount)
{
    if (d->manualProgress == false)
        setProgressRange(0, resultCount);
    d->m_expectedResultCount = resultCount;
}

int QFutureInterfaceBase::expectedResultCount()
{
    return d->m_expectedResultCount;
}

bool QFutureInterfaceBase::queryState(State state) const
{
    return (d->state & state);
}

void QFutureInterfaceBase::waitForResult(int resultIndex)
{
    d->m_exceptionStore.throwPossibleException();

    if (!(d->state & Running))
        return;

    QMutexLocker lock(&d->m_mutex);

    if (!(d->state & Running))
        return;

    const int waitIndex = (resultIndex == -1) ? INT_MAX : resultIndex;
    while ((d->state & Running) && d->internal_isResultReadyAt(waitIndex) == false)
        d->waitCondition.wait(&d->m_mutex);

    d->m_exceptionStore.throwPossibleException();
}

void QFutureInterfaceBase::waitForFinished()
{
    QMutexLocker lock(&d->m_mutex);

    while (d->state & Running)
        d->waitCondition.wait(&d->m_mutex);

    d->m_exceptionStore.throwPossibleException();
}

void QFutureInterfaceBase::reportResultsReady(int beginIndex, int endIndex)
{
    if ((d->state & Canceled) || (d->state & Finished) || beginIndex == endIndex)
        return;

    d->waitCondition.wakeAll();

    if (d->manualProgress == false) {
        if (d->internal_updateProgress(d->m_progressValue + endIndex - beginIndex) == false) {
            d->sendCallOut(QFutureCallOutEvent(QFutureCallOutEvent::ResultsReady,
                                               beginIndex,
                                               endIndex));
            return;
        }

        d->sendCallOuts(QFutureCallOutEvent(QFutureCallOutEvent::Progress,
                                            d->m_progressValue,
                                            d->m_progressText),
                        QFutureCallOutEvent(QFutureCallOutEvent::ResultsReady,
                                            beginIndex,
                                            endIndex));
        return;
    }
    d->sendCallOut(QFutureCallOutEvent(QFutureCallOutEvent::ResultsReady, beginIndex, endIndex));
}

void QFutureInterfaceBase::setFilterMode(bool enable)
{
    QMutexLocker locker(&d->m_mutex);
    resultStoreBase().setFilterMode(enable);
}

void QFutureInterfaceBase::setProgressRange(int minimum, int maximum)
{
    QMutexLocker locker(&d->m_mutex);
    d->m_progressMinimum = minimum;
    d->m_progressMaximum = maximum;
    d->sendCallOut(QFutureCallOutEvent(QFutureCallOutEvent::ProgressRange, minimum, maximum));
}

void QFutureInterfaceBase::setProgressValue(int progressValue)
{
    setProgressValueAndText(progressValue, QString());
}

void QFutureInterfaceBase::setProgressValueAndText(int progressValue,
                                                   const QString &progressText)
{
    QMutexLocker locker(&d->m_mutex);
    if (d->manualProgress == false)
        d->manualProgress = true;
    if (d->m_progressValue >= progressValue)
        return;

    if ((d->state & Canceled) || (d->state & Finished))
        return;

    if (d->internal_updateProgress(progressValue, progressText)) {
        d->sendCallOut(QFutureCallOutEvent(QFutureCallOutEvent::Progress,
                                           d->m_progressValue,
                                           d->m_progressText));
    }
}

QMutex *QFutureInterfaceBase::mutex() const
{
    return &d->m_mutex;
}

QtConcurrent::internal::ExceptionStore &QFutureInterfaceBase::exceptionStore()
{
    return d->m_exceptionStore;
}

QtConcurrent::ResultStoreBase &QFutureInterfaceBase::resultStoreBase()
{
    return d->m_results;
}

const QtConcurrent::ResultStoreBase &QFutureInterfaceBase::resultStoreBase() const
{
    return d->m_results;
}

QFutureInterfaceBase &QFutureInterfaceBase::operator=(const QFutureInterfaceBase &other)
{
    other.d->refCount.ref();
    if (!d->refCount.deref())
        delete d;
    d = other.d;
    return *this;
}

bool QFutureInterfaceBase::referenceCountIsOne() const
{
    return d->refCount == 1;
}

QFutureInterfaceBasePrivate::QFutureInterfaceBasePrivate(QFutureInterfaceBase::State initialState)
    : refCount(1), m_progressValue(0), m_progressMinimum(0), m_progressMaximum(0),
      state(initialState), progressTimeStarted(false), pendingResults(0),
      manualProgress(false), m_expectedResultCount(0)
{ }

int QFutureInterfaceBasePrivate::internal_resultCount() const
{
    return m_results.count(); // ### subtract canceled results.
}

bool QFutureInterfaceBasePrivate::internal_isResultReadyAt(int index) const
{
    return (m_results.contains(index));
}

bool QFutureInterfaceBasePrivate::internal_waitForNextResult()
{
    if (m_results.hasNextResult())
        return true;

    while ((state & QFutureInterfaceBase::Running) && m_results.hasNextResult() == false)
        waitCondition.wait(&m_mutex);

    return (!(state & QFutureInterfaceBase::Canceled) && m_results.hasNextResult());
}

bool QFutureInterfaceBasePrivate::internal_updateProgress(int progress,
                                                          const QString &progressText)
{
    if (m_progressValue >= progress)
        return false;

    m_progressValue = progress;
    m_progressText = progressText;

    if (progressTimeStarted == true && m_progressValue != m_progressMaximum) // make sure the first and last steps are emitted.
        if (progressTime.elapsed() < (1000 / MaxProgressEmitsPerSecond))
            return false;

    progressTime.start();
    progressTimeStarted = true;
    return true;
}

void QFutureInterfaceBasePrivate::internal_setThrottled(bool enable)
{
    // bail out if we are not changing the state
    if (enable && (state & QFutureInterfaceBase::Throttled)
        || !enable && !(state & QFutureInterfaceBase::Throttled))
        return;

    // change the state
    if (enable) {
        state  = QFutureInterfaceBase::State(state | QFutureInterfaceBase::Throttled);
    } else {
        state  = QFutureInterfaceBase::State(state & ~QFutureInterfaceBase::Throttled);
        if (!(state & QFutureInterfaceBase::Paused))
            pausedWaitCondition.wakeAll();
    }
}

void QFutureInterfaceBasePrivate::sendCallOut(const QFutureCallOutEvent &callOutEvent)
{
    if (outputConnections.isEmpty())
        return;

    for (int i = 0; i < outputConnections.count(); ++i)
        outputConnections.at(i)->postCallOutEvent(callOutEvent);
}

void QFutureInterfaceBasePrivate::sendCallOuts(const QFutureCallOutEvent &callOutEvent1,
                                     const QFutureCallOutEvent &callOutEvent2)
{
    if (outputConnections.isEmpty())
        return;

    for (int i = 0; i < outputConnections.count(); ++i) {
        QFutureCallOutInterface *interface = outputConnections.at(i);
        interface->postCallOutEvent(callOutEvent1);
        interface->postCallOutEvent(callOutEvent2);
    }
}

// This function connects an output interface (for example a QFutureWatcher)
// to this future. While holding the lock we check the state and ready results
// and add the appropriate callouts to the queue. In order to avoid deadlocks,
// the actual callouts are made at the end while not holding the lock.
void QFutureInterfaceBasePrivate::connectOutputInterface(QFutureCallOutInterface *interface)
{
    QMutexLocker locker(&m_mutex);

    if (state & QFutureInterfaceBase::Started) {
        interface->postCallOutEvent(QFutureCallOutEvent(QFutureCallOutEvent::Started));
        interface->postCallOutEvent(QFutureCallOutEvent(QFutureCallOutEvent::ProgressRange,
                                                        m_progressMinimum,
                                                        m_progressMaximum));
        interface->postCallOutEvent(QFutureCallOutEvent(QFutureCallOutEvent::Progress,
                                                        m_progressValue,
                                                        m_progressText));
    }

    QtConcurrent::ResultIteratorBase it = m_results.begin();
    while (it != m_results.end()) {
        const int begin = it.resultIndex();
        const int end = begin + it.batchSize();
        interface->postCallOutEvent(QFutureCallOutEvent(QFutureCallOutEvent::ResultsReady,
                                                        begin,
                                                        end));
        it.batchedAdvance();
    }

    if (state & QFutureInterfaceBase::Paused)
        interface->postCallOutEvent(QFutureCallOutEvent(QFutureCallOutEvent::Paused));

    if (state & QFutureInterfaceBase::Canceled)
        interface->postCallOutEvent(QFutureCallOutEvent(QFutureCallOutEvent::Canceled));

    if (state & QFutureInterfaceBase::Finished)
        interface->postCallOutEvent(QFutureCallOutEvent(QFutureCallOutEvent::Finished));

    outputConnections.append(interface);
}

void QFutureInterfaceBasePrivate::disconnectOutputInterface(QFutureCallOutInterface *interface)
{
    QMutexLocker lock(&m_mutex);
    const int index = outputConnections.indexOf(interface);
    if (index == -1)
        return;
    outputConnections.removeAt(index);

    interface->callOutInterfaceDisconnected();
}

void QFutureInterfaceBasePrivate::setState(QFutureInterfaceBase::State newState)
{
    state = newState;
}

QT_END_NAMESPACE

#endif // QT_NO_CONCURRENT
