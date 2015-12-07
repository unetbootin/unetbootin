//! [0]
    static const char* myimage_data[]={
    "...",
    ...
    "..."};
//! [0]


//! [1]
    Q3MimeSourceFactory::defaultFactory()->setImage("myimage", QImage(myimage_data));
//! [1]


//! [2]
    QLabel* label = new QLabel(
        "Rich text with embedded image:<img source=\"myimage\">"
        "Isn't that <em>cute</em>?");
//! [2]


//! [3]
    delete label;
    Q3MimeSourceFactory::defaultFactory()->setData("myimage", 0);
//! [3]


//! [4]
    setExtensionType("html", "text/html;charset=iso8859-1");
    setExtensionType("htm", "text/html;charset=iso8859-1");
    setExtensionType("txt", "text/plain");
    setExtensionType("xml", "text/xml;charset=UTF-8");
//! [4]


