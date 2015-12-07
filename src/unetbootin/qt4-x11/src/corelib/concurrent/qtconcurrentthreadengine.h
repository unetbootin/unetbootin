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

#ifndef QTCONCURRENT_THREADENGINE_H
#define QTCONCURRENT_THREADENGINE_H

#include <QtCore/qglobal.h>

#ifndef QT_NO_CONCURRENT

#include <QtCore/qthreadpool.h>
#include <QtCore/qfuture.h>
#include <QtCore/qdebug.h>
#include <QtCore/qtconcurrentexception.h>
#include <QtCore/qwaitcondition.h>

QT_BEGIN_HEADER
QT_BEGIN_NAMESPACE

QT_MODULE(Core)

#ifndef qdoc

namespace QtConcurrent {

// A Semaphore that can wait until all resources are returned.
class ThreadEngineSemaphore
{
public:
    ThreadEngineSemaphore()
    :count(0) { }

    void acquire()
    {
        QMutexLocker lock(&mutex);
        ++count;
    }

    int release()
    {
        QMutexLocker lock(&mutex);
        if (--count == 0)
            waitCondition.wakeAll();
        return count;
    }

    // Wait until all resources are released.
    void wait()
    {
        QMutexLocker lock(&mutex);
        if (count != 0)
            waitCondition.wait(&mutex);
    }

    int currentCount()
    {
        return count;
    }

    // releases a resource, unless this is the last resource.
    // returns true if a resource was released.
    bool releaseUnlessLast()
    {
        QMutexLocker lock(&mutex);
        if (count == 1)
            return false;
        --count;
        return true;
    }

private:
    QMutex mutex;
    int count;
    QWaitCondition waitCondition;
};

enum ThreadFunctionResult { ThrottleThread, ThreadFinished };

// The ThreadEngine controls the threads used in the computation.
// Can be run in three modes: single threaded, multi-threaded blocking
// and multi-threaded asynchronous.
// The code for the single threaded mode is
class Q_CORE_EXPORT ThreadEngineBase: public QRunnable
{
public:
    // Public API:
    ThreadEngineBase();
    virtual ~ThreadEngineBase();
    void startSingleThreaded();
    void startBlocking();
    void startThread();
    bool isCanceled();
    void waitForResume();
    bool isProgressReportingEnabled();
    void setProgressValue(int progress);
    void setProgressRange(int minimum, int maximum);

protected: // The user overrides these:
    virtual void start() {}
    virtual void finish() {}
    virtual ThreadFunctionResult threadFunction() { return ThreadFinished; }
    virtual bool shouldStartThread() { return futureInterface ? !futureInterface->isPaused() : true; }
    virtual bool shouldThrottleThread() { return futureInterface ? futureInterface->isPaused() : false; }
private:
    bool startThreadInternal();
    void startThreads();
    void threadExit();
    bool threadThrottleExit();
    void run();
    virtual void asynchronousFinish() = 0;
#ifndef QT_NO_EXCEPTIONS
    void handleException(const QtConcurrent::Exception &exception);
#endif
protected:
    QFutureInterfaceBase *futureInterface;
    QThreadPool *threadPool;
    ThreadEngineSemaphore semaphore;
    QtConcurrent::internal::ExceptionStore exceptionStore;
};


template <typename T>
class ThreadEngine : public virtual ThreadEngineBase
{
public:
    typedef T ResultType;

    virtual T *result() { return 0; }

    QFutureInterface<T> *futureInterfaceTyped()
    {
        return static_cast<QFutureInterface<T> *>(futureInterface);
    }

    // Runs the user algorithm using a single thread.
    T *startSingleThreaded()
    {
        ThreadEngineBase::startSingleThreaded();
        return result();
    }

    // Runs the user algorithm using multiple threads.
    // This function blocks until the algorithm is finished,
    // and then returns the result.
    T *startBlocking()
    {
        ThreadEngineBase::startBlocking();
        return result();
    }

    // Runs the user algorithm using multiple threads.
    // Does not block, returns a future.
    QFuture<T> startAsynchronously()
    {
        futureInterface = new QFutureInterface<T>();

        // reportStart() must be called before starting threads, otherwise the
        // user algorithm might finish while reportStart() is running, which
        // is very bad.
        futureInterface->reportStarted();
        QFuture<T> future = QFuture<T>(futureInterfaceTyped());
        start();

        semaphore.acquire();
        threadPool->start(this);
        return future;
    }

    void asynchronousFinish()
    {
        finish();
        futureInterfaceTyped()->reportFinished(result());
        delete futureInterfaceTyped();
        delete this;
    }


    void reportResult(const T *_result, int index = -1)
    {
        if (futureInterface)
            futureInterfaceTyped()->reportResult(_result, index);
    }

    void reportResults(const QVector<T> &_result, int index = -1, int count = -1)
    {
        if (futureInterface)
            futureInterfaceTyped()->reportResults(_result, index, count);
    }
};

// The ThreadEngineStarter class ecapsulates the return type
// from the thread engine.
// Depending on how the it is used, it will run
// the engine in either blocking mode or asynchronous mode.
template <typename T>
class ThreadEngineStarterBase
{
public:
    ThreadEngineStarterBase(ThreadEngine<T> *_threadEngine)
    : threadEngine(_threadEngine) { }

    inline ThreadEngineStarterBase(const ThreadEngineStarterBase &other)
    : threadEngine(other.threadEngine) { }

    QFuture<T> startAsynchronously()
    {
        return threadEngine->startAsynchronously();
    }

    operator QFuture<T>()
    {
        return startAsynchronously();
    }

protected:
    ThreadEngine<T> *threadEngine;
};


// We need to factor out the code that dereferences the T pointer,
// with a specialization where T is void. (code that dereferences a void *
// won't compile)
template <typename T>
class ThreadEngineStarter : public ThreadEngineStarterBase<T>
{
public:
    ThreadEngineStarter(ThreadEngine<T> *threadEngine)
    :ThreadEngineStarterBase<T>(threadEngine) {}

    T startBlocking()
    {
        T t = *this->threadEngine->startBlocking();
        delete this->threadEngine;
        return t;
    }
};

// Full template specialization where T is void.
template <>
class ThreadEngineStarter<void> : public ThreadEngineStarterBase<void>
{
public:
    ThreadEngineStarter<void>(ThreadEngine<void> *_threadEngine)
    :ThreadEngineStarterBase<void>(_threadEngine) {}

    void startBlocking()
    {
        this->threadEngine->startBlocking();
        delete this->threadEngine;
    }
};

template <typename ThreadEngine>
inline ThreadEngineStarter<typename ThreadEngine::ResultType> startThreadEngine(ThreadEngine *threadEngine)
{
    return ThreadEngineStarter<typename ThreadEngine::ResultType>(threadEngine);
}

} // namespace QtConcurrent

#endif //qdoc

QT_END_NAMESPACE
QT_END_HEADER

#endif // QT_NO_CONCURRENT

#endif
