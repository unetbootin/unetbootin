#include <QtGui>
#include "unetbootin.h"
#include <iostream>

unetbootin::unetbootin(QWidget *parent)
    : QWidget(parent)
{
    setupUi(this);
}

void unetbootin::on_FloppyFileSelector_clicked()
{
//    QString fileName;
    fileName = QFileDialog::getOpenFileName();
//    std::cout << qPrintable(fileName);
    FloppyPath->setText(fileName);
}

void unetbootin::on_KernelFileSelector_clicked()
{
//    QString fileName;
    fileName = QFileDialog::getOpenFileName();
//    std::cout << qPrintable(fileName);
    KernelPath->setText(fileName);
}

void unetbootin::on_InitrdFileSelector_clicked()
{
//    QString fileName;
    fileName = QFileDialog::getOpenFileName();
//    std::cout << qPrintable(fileName);
    InitrdPath->setText(fileName);
}

