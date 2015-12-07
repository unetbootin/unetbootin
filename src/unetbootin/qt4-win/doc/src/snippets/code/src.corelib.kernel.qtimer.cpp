//! [0]
        #include <QApplication>
        #include <QTimer>

        int main(int argc, char *argv[])
        {
            QApplication app(argc, argv);
            QTimer::singleShot(600000, &app, SLOT(quit()));
            ...
            return app.exec();
        }
//! [0]


