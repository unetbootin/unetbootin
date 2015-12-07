//! [0]
    QSqlQuery q;
    q.prepare("insert into test (i1, i2, s) values (?, ?, ?)");

    QVariantList col1;
    QVariantList col2;
    QVariantList col3;

    col1 << 1 << 3;
    col2 << 2 << 4;
    col3 << "hello" << "world";

    q.bindValue(0, col1);
    q.bindValue(1, col2);
    q.bindValue(2, col3);

    if (!q.execBatch())
        qDebug() << q.lastError();
//! [0]


//! [1]
    QSqlQuery query = ...
    QVariant v = query.result()->handle();
    if (v.isValid() && qstrcmp(v.typeName(), "sqlite3_stmt*")) {
        // v.data() returns a pointer to the handle
        sqlite3_stmt *handle = *static_cast<sqlite3_stmt **>(v.data());
        if (handle != 0) { // check that it is not NULL
            ...
        }
    }
//! [1]


//! [2]
    if (v.typeName() == "PGresult*") {
        PGresult *handle = *static_cast<PGresult **>(v.data());
        if (handle != 0) ...
    }

    if (v.typeName() == "MYSQL_STMT*") {
        MYSQL_STMT *handle = *static_cast<MYSQL_STMT **>(v.data());
        if (handle != 0) ...
    }
//! [2]


