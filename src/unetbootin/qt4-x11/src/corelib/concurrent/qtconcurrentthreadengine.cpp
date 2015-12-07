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

#include "qtconcurrentthreadengine.h"

#ifndef QT_NO_CONCURRENT

QT_BEGIN_NAMESPACE

namespace QtConcurrent {

ThreadEngineBase::ThreadEngineBase()
:futureInterface(0), threadPool(QThreadPool::globalInstance())
{
    setAutoDelete(false);
}

ThreadEngineBase::~ThreadEngineBase() {}

void ThreadEngineBase::startSingleThreaded()
{
    start();
    while (threadFunction() != ThreadFinished)
        ;
    finish();
}

void ThreadEngineBase::startBlocking()
{
    start();
    semaphore.acquire();
    startThreads();

    bool throttled = false;
#ifndef QT_NO_EXCEPTIONS
    try {
#endif
        while (threadFunction() == ThrottleThread) {
            if (threadThrottleExit()) {
                throttled = true;
                break;
            }
        }
#ifndef QT_NO_EXCEPTIONS
    } catch (QtConcurrent::Exception &e) {
        handleException(e);
    } catch (...) {
        handleException(QtConcurrent::UnhandledException());
    }
#endif

    if (throttled == false) {
        semaphore.release();
    }

    semaphore.wait();
    finish();
    exceptionStore.throwPossibleException();
}

void ThreadEngineBase::startThread()
{
    startThreadInternal();
}

bool ThreadEngineBase::isCanceled()
{
    if (futureInterface)
        return futureInterface->isCanceled();
    else
        return false;
}

void ThreadEngineBase::waitForResume()
{
    if (futureInterface)
        futureInterface->waitForResume();
}

bool ThreadEngineBase::isProgressReportingEnabled()
{
    // If we don't have a QFuture, there is no-one to report the progress to.
    return (futureInterface != 0);
}

void ThreadEngineBase::setProgressValue(int progress)
{
    if (futureInterface)
        futureInterface->setProgressValue(progress);
}

void ThreadEngineBase::setProgressRange(int minimum, int maximum)
{
    if (futureInterface)
        futureInterface->setProgressRange(minimum, maximum);
}

bool ThreadEngineBase::startThreadInternal()
{
    if (this->isCanceled())
        return false;

    semaphore.acquire();
    if (!threadPool->tryStart(this)) {
        semaphore.release();
        return false;
    }
    return true;
}

void ThreadEngineBase::startThreads()
{
    while (shouldStartThread() && startThreadInternal())
        ;
}

void ThreadEngineBase::threadExit()
{
    const bool asynchronous = futureInterface != 0;
    const int lastThread = (semaphore.release() == 0);

    if (lastThread && asynchronous)
        this->asynchronousFinish();
}

// Called by a worker thread that wants to be throttled. If the current number
// of running threads is larger than one the thread is allowed to exit and
// this function returns one.
bool ThreadEngineBase::threadThrottleExit()
{
    return semaphore.releaseUnlessLast();
}

void ThreadEngineBase::run() // implements QRunnable.
{
    if (this->isCanceled()) {
        threadExit();
        return;
    }

    startThreads();

#ifndef QT_NO_EXCEPTIONS
    try {
#endif
        while (threadFunction() == ThrottleThread) {
            // threadFunction returning ThrottleThread means it that the user
            // struct wants to be throttled by making a worker thread exit.
            // Respect that request unless this is the only worker thread left
            // running, in which case it has to keep going.
            if (threadThrottleExit())
                return;
        }

#ifndef QT_NO_EXCEPTIONS
    } catch (QtConcurrent::Exception &e) {
        handleException(e);
    } catch (...) {
        handleException(QtConcurrent::UnhandledException());
    }
#endif
    threadExit();
}

#ifndef QT_NO_EXCEPTIONS

void ThreadEngineBase::handleException(const QtConcurrent::Exception &exception)
{
    if (futureInterface)
        futureInterface->reportException(exception);
    else
        exceptionStore.setException(exception);
}
#endif


} // namepsace QtConcurrent

QT_END_NAMESPACE

#endif // QT_NO_CONCURRENT
