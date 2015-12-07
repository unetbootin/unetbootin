//! [0]
       QPixmap pixmap(":/splash.png");
       QSplashScreen *splash = new QSplashScreen(pixmap);
       splash->show();

       ... // Loading some items
       splash->showMessage("Loaded modules");

       qApp->processEvents();

       ... // Establishing connections
       splash->showMessage("Established connections");

       qApp->processEvents();
//! [0]


