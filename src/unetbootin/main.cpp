#include <QApplication>
#include "unetbootin.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    unetbootin unetbootin;
    unetbootin.show();
    return app.exec();
}
