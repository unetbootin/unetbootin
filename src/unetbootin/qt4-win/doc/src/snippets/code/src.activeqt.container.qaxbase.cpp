//! [0]
        connect(buttonBack, SIGNAL(clicked()), webBrowser, SLOT(GoBack()));
//! [0]


//! [1]
        activeX->setProperty("text", "some text");
        int value = activeX->property("value");
//! [1]


//! [2]
        webBrowser->dynamicCall("GoHome()");
//! [2]


//! [3]
        connect(webBrowser, SIGNAL(TitleChanged(QString)),
	        this, SLOT(setCaption(QString)));
//! [3]


//! [4]
        dispinterface IControl
        {
        properties:
            [id(1)] BSTR text;
	    [id(2)] IFontDisp *font;

        methods:
	    [id(6)] void showColumn([in] int i);
            [id(3)] bool addColumn([in] BSTR t);
	    [id(4)] int fillList([in, out] SAFEARRAY(VARIANT) *list);
	    [id(5)] IDispatch *item([in] int i);
        };
//! [4]


//! [5]
        QAxObject object("<CLSID>");

        QString text = object.property("text").toString();
        object.setProperty("font", QFont("Times New Roman", 12));

        connect(this, SIGNAL(clicked(int)), &object, SLOT(showColumn(int)));
        bool ok = object.dynamicCall("addColumn(const QString&)", "Column 1").toBool();

        QList<QVariant> varlist;
        QList<QVariant> parameters;
        parameters << QVariant(varlist);
        int n = object.dynamicCall("fillList(QList<QVariant>&)", parameters).toInt();

        QAxObject *item = object.querySubItem("item(int)", 5);
//! [5]


//! [6]
        IUnknown *iface = 0;
        activeX->queryInterface(IID_IUnknown, (void**)&iface);
        if (iface) {
            // use the interface
	    iface->Release();
        }
//! [6]


//! [7]
        ctrl->setControl("{8E27C92B-1264-101C-8A2F-040224009C02}");
//! [7]


//! [8]
        ctrl->setControl("MSCal.Calendar");
//! [8]


//! [9]
        ctrl->setControl("Calendar Control 9.0");
//! [9]


//! [10]
        ctrl->setControl("c:/files/file.doc");
//! [10]


//! [11]
        <domain/username>:<password>@server/{8E27C92B-1264-101C-8A2F-040224009C02}
//! [11]


//! [12]
        {8E27C92B-1264-101C-8A2F-040224009C02}:<LicenseKey>
//! [12]


//! [13]
        {8E27C92B-1264-101C-8A2F-040224009C02}&
//! [13]


//! [14]
        ctrl->setControl("DOMAIN/user:password@server/{8E27C92B-1264-101C-8A2F-040224009C02}:LicenseKey");
//! [14]


//! [15]
        activeX->dynamicCall("Navigate(const QString&)", "www.trolltech.com");
//! [15]


//! [16]
        activeX->dynamicCall("Navigate(\"www.trolltech.com\")");
//! [16]


//! [17]
        activeX->dynamicCall("Value", 5);
        QString text = activeX->dynamicCall("Text").toString();
//! [17]


//! [18]
        IWebBrowser2 *webBrowser = 0;
        activeX->queryInterface(IID_IWebBrowser2, (void **)&webBrowser);
        if (webBrowser) {
            webBrowser->Navigate2(pvarURL);
	    webBrowser->Release();
        }
//! [18]


//! [19]
        QAxWidget outlook("Outlook.Application");
        QAxObject *session = outlook.querySubObject("Session");
        if (session) {
	    QAxObject *defFolder = session->querySubObject(
				    "GetDefaultFolder(OlDefaultFolders)",
				    "olFolderContacts");
	    //...
        }
//! [19]


//! [20]
        void Receiver::slot(const QString &name, int argc, void *argv)
        {
	    VARIANTARG *params = (VARIANTARG*)argv;
	    if (name.startsWith("BeforeNavigate2(")) {
	        IDispatch *pDisp = params[argc-1].pdispVal;
	        VARIANTARG URL = *params[argc-2].pvarVal;
	        VARIANTARG Flags = *params[argc-3].pvarVal;
	        VARIANTARG TargetFrameName = *params[argc-4].pvarVal;
	        VARIANTARG PostData = *params[argc-5].pvarVal;
	        VARIANTARG Headers = *params[argc-6].pvarVal;
	        bool *Cancel = params[argc-7].pboolVal;
	    }
        }
//! [20]


