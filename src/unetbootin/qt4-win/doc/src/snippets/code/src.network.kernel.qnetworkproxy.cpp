//! [0]
        QNetworkProxy proxy;
        proxy.setType(QNetworkProxy::Socks5Proxy);
        proxy.setHostName("proxy.example.com");
        proxy.setPort(1080);
        proxy.setUser("username");
        proxy.setPassword("password");
        QNetworkProxy::setApplicationProxy(proxy);
//! [0]


//! [1]
        serverSocket->setProxy(QNetworkProxy::NoProxy);
//! [1]


