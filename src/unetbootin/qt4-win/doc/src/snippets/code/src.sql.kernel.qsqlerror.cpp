//! [0]
    QSqlQueryModel model;
    model.setQuery("select * from myTable");
    if (model.lastError().isValid())
        qDebug() << model.lastError();
//! [0]


