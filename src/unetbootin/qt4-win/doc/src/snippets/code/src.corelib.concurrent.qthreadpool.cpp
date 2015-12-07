//! [0]
        class HelloWorldTask : public QRunnable
        {
            void run()
            {
                qDebug() << "Hello world from thread" << QThread::currentThread();
            }
        }

        HelloWorldTask *hello = new HelloWorldTask();
        // QThreadPool takes ownership and deletes 'hello' automatically
        QThreadPool::globalInstance()->start(hello);
//! [0]


