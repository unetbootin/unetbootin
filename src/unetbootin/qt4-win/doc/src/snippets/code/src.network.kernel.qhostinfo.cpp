//! [0]
        // To find the IP address of www.trolltech.com
        QHostInfo::lookupHost("www.trolltech.com",
                              this, SLOT(printResults(QHostInfo)));

        // To find the host name for 4.2.2.1
        QHostInfo::lookupHost("4.2.2.1",
                              this, SLOT(printResults(QHostInfo)));
//! [0]


//! [1]
        QHostInfo info = QHostInfo::fromName("www.trolltech.com");
//! [1]


//! [2]
        QHostInfo::lookupHost("www.kde.org",
                              this, SLOT(lookedUp(QHostInfo)));
//! [2]


//! [3]
        void MyWidget::lookedUp(const QHostInfo &host)
        {
            if (host.error() != QHostInfo::NoError) {
                qDebug() << "Lookup failed:" << host.errorString();
                return;
            }

            foreach (QHostAddress address, host.addresses())
                qDebug() << "Found address:" << address.toString();
        }
//! [3]


//! [4]
        QHostInfo::lookupHost("4.2.2.1",
                              this, SLOT(lookedUp(QHostInfo)));
//! [4]


//! [5]
        QHostInfo info;
        ...
        if (!info.addresses().isEmpty()) {
            QHostAddress address = info.addresses().first();
            // use the first IP address
        }
//! [5]


