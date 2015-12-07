//! [0]
        DEFINES += QT_NO_CAST_FROM_ASCII \
                   QT_NO_CAST_TO_ASCII
//! [0]


//! [1]
        QString url = QLatin1String("http://www.unicode.org/");
//! [1]


//! [2]
        double d = 12.34;
        QString str = QString("delta: %1").arg(d, 0, 'E', 3);
        // str == "delta: 1.234E+01"
//! [2]


//! [3]
        if (str == "auto" || str == "extern"
                || str == "static" || str == "register") {
            ...
        }
//! [3]


//! [4]
        if (str == QString("auto") || str == QString("extern")
                || str == QString("static") || str == QString("register")) {
            ...
        }
//! [4]


//! [5]
        if (str == QLatin1String("auto")
                || str == QLatin1String("extern")
                || str == QLatin1String("static")
                || str == QLatin1String("register") {
            ...
        }
//! [5]


//! [6]
        QLabel *label = new QLabel(QLatin1String("MOD"), this);
//! [6]


