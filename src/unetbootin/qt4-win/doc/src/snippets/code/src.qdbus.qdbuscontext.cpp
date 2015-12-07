//! [0]
    class MyObject: public QObject,
                    protected QDBusContext
    {
        Q_OBJECT
        QDBusConnection conn;
        QDBusMessage msg;

    ...

    protected slots:
        void process();
    public slots:
        void methodWithError();
        QString methodWithDelayedReply();
    };

    void MyObject::methodWithError()
    {
        sendErrorReply(QDBusError::NotSupported,
                       "The method call 'methodWithError()' is not supported");
    }

    QString MyObject::methodWithDelayedReply()
    {
        conn = connection();
        msg = message();
        setDelayedReply(true);
        QMetaObject::invokeMethod(this, "process", Qt::QueuedConnection);
        return QString();
    }
//! [0]


