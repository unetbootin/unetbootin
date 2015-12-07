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
    bool isProgressReportingEnabled();
    void setProgressValue(int progress);
    void setProgressRange(int minimum, int maximum);

protected: // The user overrides these:
    virtual void start() {}
    virtual void finish() {}
    virtual ThreadFunctionResult threadFunction() { return ThreadFinished; }
    virtual bool shouldStartThread() { return true; }
    virtual bool shouldThrottleThread() { return false; }
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
