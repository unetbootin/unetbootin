//! [0]
        QSslConfiguration config = sslSocket.sslConfiguration();
        config.setProtocol(QSsl::TlsV1);
        sslSocket.setSslConfiguration(config);
//! [0]


