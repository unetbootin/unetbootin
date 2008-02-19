#ifndef UNETBOOTIN_H
#define UNETBOOTIN_H

#include "ui_unetbootin.h"

class unetbootin : public QWidget, private Ui::unetbootin
{
    Q_OBJECT

public:
    unetbootin(QWidget *parent = 0);
    QString nameFloppy;
    QString nameKernel;
    QString nameInitrd;
    QString nameDistro;

private slots:
    void on_FloppyFileSelector_clicked();
    void on_KernelFileSelector_clicked();
    void on_InitrdFileSelector_clicked();
    void on_cancelbutton_clicked();
    void downloadfile(QString fileurl, QString targetfile);
    void on_okbutton_clicked();
};

#endif

