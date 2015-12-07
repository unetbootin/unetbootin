//! [0]
    Q3SqlCursor* myCursor = myManager.cursor();
    ...
    QSqlRecord* buf = myCursor->primeUpdate();
    buf->setValue("name", "Ola");
    buf->setValue("city", "Oslo");
    ...
    myCursor->update();  // update current record
    myCursor->select();  // refresh the cursor
    myManager.findBuffer(myCursor->primaryIndex()); // go to the updated record
//! [0]


