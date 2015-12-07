//! [0]
    QtToolBarManager *toolBarManager;

    void MyMainWindow::customize()
        {
            QtToolBarDialog dialog(this);
            dialog.setToolBarManager(toolBarManager);
            dialog.exec();
        }
//! [0]


