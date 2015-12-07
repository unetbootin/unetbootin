//! [0]
    QSqlDatabase db = ...;
    QVariant v = db.driver()->handle();
    if (v.isValid() && qstrcmp(v.typeName(), "sqlite3*")==0) {
        // v.data() returns a pointer to the handle
        sqlite3 *handle = *static_cast<sqlite3 **>(v.data());
        if (handle != 0) { // check that it is not NULL
            ...
        }
    }
//! [0]


//! [1]
    if (v.typeName() == "PGconn*") {
        PGconn *handle = *static_cast<PGconn **>(v.data());
        if (handle != 0) ...
    }

    if (v.typeName() == "MYSQL*") {
        MYSQL *handle = *static_cast<MYSQL **>(v.data());
        if (handle != 0) ...
    }
//! [1]


