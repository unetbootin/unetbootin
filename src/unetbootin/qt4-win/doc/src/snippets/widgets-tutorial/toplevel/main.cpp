#include <QtGui>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
//! [create, resize and show]
    QWidget *window = new QWidget();
    window->resize(320, 240);
    window->show();
//! [create, resize and show]
    window->setWindowTitle(tr("Top-level widget"));
    return app.exec();
}
