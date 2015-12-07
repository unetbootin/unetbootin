//! [0]
    content-type: text/html
//! [0]


//! [1]
    header.setValue("content-type", "text/html");
    QString contentType = header.value("content-type");
//! [1]


//! [2]
    QHttpRequestHeader header("GET", "/index.html");
    header.setValue("Host", "www.trolltech.com");
    http->setHost("www.trolltech.com");
    http->request(header);
//! [2]


//! [3]
    http->setHost("www.trolltech.com"); // id == 1
    http->get("/index.html");           // id == 2
//! [3]


//! [4]
    requestStarted(1)
    requestFinished(1, false)

    requestStarted(2)
    stateChanged(Connecting)
    stateChanged(Sending)
    dataSendProgress(77, 77)
    stateChanged(Reading)
    responseHeaderReceived(responseheader)
    dataReadProgress(5388, 0)
    readyRead(responseheader)
    dataReadProgress(18300, 0)
    readyRead(responseheader)
    stateChanged(Connected)
    requestFinished(2, false)

    done(false)

    stateChanged(Closing)
    stateChanged(Unconnected)
//! [4]


//! [5]
    http->setHost("www.foo.bar");       // id == 1
    http->get("/index.html");           // id == 2
    http->post("register.html", data);  // id == 3
//! [5]


//! [6]
    requestStarted(1)
    requestFinished(1, false)

    requestStarted(2)
    stateChanged(HostLookup)
    requestFinished(2, true)

    done(true)

    stateChanged(Unconnected)
//! [6]


//! [7]
      void Ticker::getTicks()
      {
        http = new QHttp(this);
        connect(http, SIGNAL(done(bool)), this, SLOT(showPage()));
        http->setProxy("proxy.example.com", 3128);
        http->setHost("ticker.example.com");
        http->get("/ticks.asp");
      }

      void Ticker::showPage()
      {
        display(http->readAll());
      }
//! [7]


