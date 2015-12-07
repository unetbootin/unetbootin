//! [0]
    while (myModel->canFetchMore())
        myModel->fetchMore();
//! [0]


//! [1]
    QSqlQueryModel model;
    model.setQuery("select * from MyTable");
    if (model.lastError().isValid())
        qDebug() << model.lastError();
//! [1]


