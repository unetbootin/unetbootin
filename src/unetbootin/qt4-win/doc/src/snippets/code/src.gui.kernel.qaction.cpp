//! [0]
    QApplication app(argc, argv);
    app.setAttribute(Qt::AA_DontShowIconsInMenus);  // Icons are *no longer shown* in menus
    // ...
    QAction *myAction = new QAction();
    // ...
    myAction->setIcon(SomeIcon);
    myAction->setIconVisibleInMenu(true);   // Icon *will* be shown in menus for *this* action.
//! [0]


