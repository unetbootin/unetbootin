//! [0]
        QString plain = "#include <QtCore>"
        QString html = Qt::escape(plain);
        // html == "#include &lt;QtCore&gt;"
//! [0]


//! [1]
    <html><head><meta http-equiv="Content-Type" content="text/html; charset=utf-8"></head><body>...
//! [1]


