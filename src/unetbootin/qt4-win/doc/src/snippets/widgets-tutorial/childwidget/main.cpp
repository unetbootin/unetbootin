#include <QtGui>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QWidget *window = new QWidget();
    window->resize(320, 240);
    window->setWindowTitle(tr("Child widget"));
    window->show();

//! [create, position and show]
    QPushButton *button = new QPushButton(tr("Press me"), window);
    button->move(100, 100);
    button->show();
//! [create, position and show]
    return app.exec();
}
