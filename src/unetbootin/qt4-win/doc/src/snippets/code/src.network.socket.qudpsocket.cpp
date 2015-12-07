//! [0]
        void Server::initSocket()
        {
            udpSocket = new QUdpSocket(this);
            udpSocket->bind(QHostAddress::LocalHost, 7755);

            connect(udpSocket, SIGNAL(readyRead()),
                    this, SLOT(readPendingDatagrams()));
        }

        void Server::readPendingDatagrams()
        {
            while (udpSocket->hasPendingDatagrams()) {
                QByteArray datagram;
                datagram.resize(udpSocket->pendingDatagramSize());
                QHostAddress sender;
                quint16 senderPort;

                udpSocket->readDatagram(datagram.data(), datagram.size(),
                                        &sender, &senderPort);

                processTheDatagram(datagram);
            }
        }
//! [0]


