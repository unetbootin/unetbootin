#include <QtGui>
#include "mainwindow.h"

//! [0]
int main(int argc, char * argv[])
{
    QApplication app(argc, argv);
    MainWindow *mainWindow = new MainWindow;
    mainWindow->show();
    return app.exec();
}
//! [0]