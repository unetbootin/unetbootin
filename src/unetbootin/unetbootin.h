#ifndef UNETBOOTIN_H
#define UNETBOOTIN_H

#include <QtCore>
#include <QtGui>
#include <QtNetwork>

#include "ui_unetbootin.h"
#ifdef Q_OS_WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <shellapi.h>
#endif

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
    QString postinstmsg;
    QString sourcefile, destinfile;
    QString kernelLine, kernelParam, kernelLoc, kernelOpts, initrdLine, initrdLoc, initrdOpts;
    QProgressDialog dlprogress;
    void downloadfile(QString fileurl, QString targetfile);
    void sysreboot();
	void callexternapp(QString execFile, QString execParm);
    void configsysEdit();
    void bootiniEdit();
    void vistabcdEdit();
    void instIndvfl(QString dstfName, QByteArray qbav);
    void wInstfiles();
    void runinst();
    void instDetType();
    void runinsthdd();
    void runinstusb();

private slots:
    void on_FloppyFileSelector_clicked();
    void on_KernelFileSelector_clicked();
    void on_InitrdFileSelector_clicked();
    void on_cancelbutton_clicked();
    void on_okbutton_clicked();

public slots:
	void dlprogressupdate(int dlbytes, int maxbytes);
};

#endif

