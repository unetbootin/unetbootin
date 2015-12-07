//! [0]
        int main(int argc, char **argv)
        {
        #ifdef Q_WS_X11
            bool useGUI = getenv("DISPLAY") != 0;
        #else
            bool useGUI = true;
        #endif
            QApplication app(argc, argv, useGUI);

            if (useGUI) {
               // start GUI version
               ...
            } else {
               // start non-GUI version
               ...
            }
            return app.exec();
        }
//! [0]


//! [1]
        QApplication::setStyle(new QWindowsStyle);
//! [1]


//! [2]
        int main(int argc, char *argv[])
        {
            QApplication::setColorSpec(QApplication::ManyColor);
            QApplication app(argc, argv);
            ...
            return app.exec();
        }
//! [2]


//! [3]
        QSize MyWidget::sizeHint() const
        {
            return QSize(80, 25).expandedTo(QApplication::globalStrut());
        }
//! [3]


//! [4]
        void showAllHiddenTopLevelWidgets()
        {
            foreach (QWidget *widget, QApplication::topLevelWidgets()) {
                if (widget->isHidden())
                    widget->show();
            }
        }
//! [4]


//! [5]
        void updateAllWidgets()
        {
            foreach (QWidget *widget, QApplication::allWidgets())
                widget->update();
        }
//! [5]


//! [6]
        int main(int argc, char *argv[])
        {
            QApplication::setDesktopSettingsAware(false);
            QApplication app(argc, argv);
            ...
            return app.exec();
        }
//! [6]


//! [7]
        if ((startPos - currentPos).manhattanLength() >=
                QApplication::startDragDistance())
            startTheDrag();
//! [7]


//! [8]
        void MyApplication::commitData(QSessionManager& manager)
        {
            if (manager.allowsInteraction()) {
                int ret = QMessageBox::warning(
                            mainWindow,
                            tr("My Application"),
                            tr("Save changes to document?"),
                            QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);

                switch (ret) {
                case QMessageBox::Save:
                    manager.release();
                    if (!saveDocument())
                        manager.cancel();
                    break;
                case QMessageBox::Discard:
                    break;
                case QMessageBox::Cancel:
                default:
                    manager.cancel();
                }
            } else {
                // we did not get permission to interact, then
                // do something reasonable instead
            }
        }
//! [8]


//! [9]
        appname -session id
//! [9]


//! [10]
        foreach (QString command, mySession.restartCommand())
            do_something(command);
//! [10]


//! [11]
        foreach (QString command, mySession.discardCommand())
            do_something(command);
//! [11]


//! [12]
        QWidget *widget = qApp->widgetAt(x, y);
        if (widget)
            widget = widget->window();
//! [12]


//! [13]
        QWidget *widget = qApp->widgetAt(point);
        if (widget)
            widget = widget->window();
//! [13]


