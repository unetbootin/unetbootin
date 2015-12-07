//! [0]
    QKeySequence(QKeySequence::Print}
    QKeySequence(tr("Ctrl+P"))
    QKeySequence(tr("Ctrl+p"))
    QKeySequence(Qt::CTRL + Qt::Key_P)
//! [0]


//! [1]
    QKeySequence(tr("Ctrl+X, Ctrl+C"))
    QKeySequence(Qt::CTRL + Qt::Key_X, Qt::CTRL + Qt::Key_C)
//! [1]


//! [2]
        QMenu *file = new QMenu(this);
        file->addAction(tr("&Open..."), this, SLOT(open()),
                          QKeySequence(tr("Ctrl+O", "File|Open")));
//! [2]


