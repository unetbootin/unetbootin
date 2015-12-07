//! [0]
    QBuffer device;
    device.setData(myQString.toUtf8());
    device.open(QIODevice::ReadOnly));
     
    QXmlQuery query;
    query.setQuery("declare variable $inputDocument external;"
                    "doc($inputDocument)/query[theDocument]");
    query.bindVariable("inputDocument", &device);
//! [0]
