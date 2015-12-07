//! [0]
        Q3MainWindow *mw = new Q3MainWindow;
        QTextEdit *edit = new QTextEdit(mw, "editor");
        edit->setFocus();
        mw->setWindowTitle("Main Window");
        mw->setCentralWidget(edit);
        mw->show();
//! [0]


//! [1]
    Q3ToolBar *tb = new Q3ToolBar(this);
    addDockWindow(tb, tr("Menubar"), Top, false);
    QMenuBar *mb = new QMenuBar(tb);
    mb->setFrameStyle(QFrame::NoFrame);
    tb->setStretchableWidget(mb);
    setDockEnabled(tb, Left, false);
    setDockEnabled(tb, Right, false);
//! [1]


//! [2]
    QFile file(filename);
    if (file.open(IO_WriteOnly)) {
        QTextStream stream(&file);
        stream << *mainWindow;
        file.close();
    }
//! [2]


//! [3]
    QFile file(filename);
    if (file.open(IO_ReadOnly)) {
        QTextStream stream(&file);
        stream >> *mainWindow;
        file.close();
    }
//! [3]


//! [4]
    Q3PopupMenu * help = new Q3PopupMenu(this);
    help->insertItem("What's &This", this , SLOT(enterWhatsThis()), Qt::SHIFT+Qt::Key_F1);
//! [4]


