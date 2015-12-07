//! [0]
        socket->connectToHost("imap", 143);
        if (socket->waitForConnected(1000))
            qDebug("Connected!");
//! [0]


//! [1]
        socket->disconnectFromHost();
            if (socket->state() == QAbstractSocket::UnconnectedState || 
                socket->waitForDisconnected(1000))
                qDebug("Disconnected!");
//! [1]


//! [2]
         // This slot is connected to QAbstractSocket::readyRead()
         void SocketClass::readyReadSlot()
         {
             while (!socket.atEnd()) {
                 QByteArray data = socket.read(100);
                 ....
             }
         }
//! [2]


//! [3]
        socket->setProxy(QNetworkProxy::NoProxy);
//! [3]


