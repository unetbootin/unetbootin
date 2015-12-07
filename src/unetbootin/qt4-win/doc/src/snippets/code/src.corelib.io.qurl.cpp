//! [0]
        QUrl url("http://www.example.com/List of holidays.xml");
        // url.toEncoded() == "http://www.example.com/List%20of%20holidays.xml"
//! [0]


//! [1]
        QUrl url = QUrl::fromEncoded("http://www.trolltech.com/List%20of%20holidays.xml");
//! [1]


//! [2]
        bool checkUrl(const QUrl &url) {
            if (!url.isValid()) {
                qDebug(QString("Invalid URL: %1").arg(url.toString()));
                return false;
            }

            return true;
        }
//! [2]


//! [3]
        QFtp ftp;
        ftp.connectToHost(url.host(), url.port(21));
//! [3]


//! [4]
        http://www.example.com/cgi-bin/drawgraph.cgi?type-pie/color-green
//! [4]


//! [5]
         QUrl baseUrl("http://www.trolltech.com/support");
         QUrl relativeUrl("../products/solutions");
         qDebug(baseUrl.resolved(relativeUrl).toString());
         // prints "http://www.trolltech.com/products/solutions"
//! [5]


//! [6]
         QByteArray ba = QUrl::toPercentEncoding("{a fishy string?}", "{}", "s");
         qDebug(ba.constData());
         // prints "{a fi%73hy %73tring%3F}"
//! [6]


