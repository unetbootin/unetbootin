#include <QtGui>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QWidget *window = new QWidget();
//! [create, lay out widgets and show]
    QLabel *label = new QLabel(tr("Name:"));
    QLineEdit *lineEdit = new QLineEdit();

    QHBoxLayout *layout = new QHBoxLayout();
    layout->addWidget(label);
    layout->addWidget(lineEdit);
    window->setLayout(layout);
//! [create, lay out widgets and show]
    window->setWindowTitle(tr("Window layout"));
    window->show();
    return app.exec();
}
