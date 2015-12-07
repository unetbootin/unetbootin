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
