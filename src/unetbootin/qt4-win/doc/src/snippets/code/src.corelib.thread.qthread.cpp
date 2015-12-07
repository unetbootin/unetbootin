//! [0]
        class MyThread : public QThread
        {
        public:
            void run();
        };

        void MyThread::run()
        {
            QTcpSocket socket;
            // connect QTcpSocket's signals somewhere meaningful
            ...
            socket.connectToHost(hostName, portNumber);
            exec();
        }
//! [0]


