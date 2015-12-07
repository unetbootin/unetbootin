//! [0]
        QPushButton p("&Exit", parent); // automatic shortcut Alt+E
        Q3PopupMenu *fileMenu = new fileMenu(parent);
        fileMenu->insertItem("Undo", parent, SLOT(undo()),
                             Qt::CTRL + Qt::Key_Z);
//! [0]


//! [1]
        accelerator->insertItem(QKeySequence("M"));
//! [1]


//! [2]
        Q3Accel *a = new Q3Accel(myWindow);
        a->connectItem(a->insertItem(Qt::CTRL + Qt::Key_P),
                       myWindow, SLOT(printDoc()));
//! [2]


//! [3]
        Q3Accel *a = new Q3Accel(myWindow);	   // create accels for myWindow
        a->insertItem(CTRL + Key_P, 200);	   // Ctrl+P, e.g. to print document
        a->insertItem(ALT + Key_X, 201);	   // Alt+X, e.g. to quit
        a->insertItem(UNICODE_ACCEL + 'q', 202); // Unicode 'q', e.g. to quit
        a->insertItem(Key_D);			   // gets a unique negative id < -1
        a->insertItem(CTRL + SHIFT + Key_P);	   // gets a unique negative id < -1
//! [3]


//! [4]
        a->connectItem(201, mainView, SLOT(quit()));
//! [4]


//! [5]
    Q3PopupMenu *file = new Q3PopupMenu(this);
    file->insertItem(p1, tr("&Open..."), this, SLOT(open()),
                      Q3Accel::stringToKey(tr("Ctrl+O", "File|Open")));
//! [5]


