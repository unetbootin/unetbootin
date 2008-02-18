#include <QApplication>
#include "unetbootin.h"
#include <iostream>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    unetbootin unetbootin;
    unetbootin.show();
    return app.exec();
//    std::cout << qPrintable(unetbootin::fileName);
//    std::cout << unetbootin::fileName;
}
