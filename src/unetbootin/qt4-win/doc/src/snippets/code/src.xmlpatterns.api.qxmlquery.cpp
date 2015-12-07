//! [0]
    QXmlNamePool namePool(query.namePool());
    query.bindVariable(QXmlName(namePool, localName), value);
//! [0]


{
//! [1]
    QByteArray myDocument;
    QBuffer buffer(&myDocument); // This is a QIODevice.
    buffer.open(QIODevice::ReadOnly);
    QXmlQuery query;
    query.bindVariable("myDocument", &buffer);
    query.setQuery("declare variable $myDocument external; doc($myDocument)");
//! [1]
}


QIODevice *device = 0;
{
//! [2]
    QXmlNamePool namePool(query.namePool());
    query.bindVariable(QXmlName(namePool, localName), device);
//! [2]

}

