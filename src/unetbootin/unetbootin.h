#ifndef UNETBOOTIN_H
#define UNETBOOTIN_H

#include "ui_unetbootin.h"

class unetbootin : public QWidget, private Ui::unetbootin
{
    Q_OBJECT

public:
    unetbootin(QWidget *parent = 0);
    QString appDir;
    QString appLoc;
    QString nameFloppy;
    QString nameKernel;
    QString nameInitrd;
    QString nameDistro;
    QString targetDrive;
    QString targetPath;
    QString installType;
    void downloadfile(QString fileurl, QString targetfile);
    void sysreboot();
	void callexternapp(QString execFile, QString execParm);
    void configsysEdit();
    void bootiniEdit();
    void vistabcdEdit();
    void instIndvfl(QString dstfName, QByteArray qbav);
    void wInstfiles();
    void runinst();

private slots:
    void on_FloppyFileSelector_clicked();
    void on_KernelFileSelector_clicked();
    void on_InitrdFileSelector_clicked();
    void on_cancelbutton_clicked();
    void on_okbutton_clicked();
};

#endif

