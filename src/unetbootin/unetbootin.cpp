#include <QtGui>
#include "unetbootin.h"

#include <windows.h>
#include <shellapi.h>

unetbootin::unetbootin(QWidget *parent)
    : QWidget(parent)
{
    setupUi(this);
}

void unetbootin::on_FloppyFileSelector_clicked()
{
    nameFloppy = QFileDialog::getOpenFileName();
    FloppyPath->clear();
    FloppyPath->insert(nameFloppy);
    radioFloppy->setChecked(1);
}

void unetbootin::on_KernelFileSelector_clicked()
{
    nameKernel = QFileDialog::getOpenFileName();
    KernelPath->clear();
    KernelPath->insert(nameKernel);
    radioManual->setChecked(1);
}

void unetbootin::on_InitrdFileSelector_clicked()
{
    nameInitrd = QFileDialog::getOpenFileName();
    InitrdPath->clear();
    InitrdPath->insert(nameInitrd);
    radioManual->setChecked(1);
}

void unetbootin::on_cancelbutton_clicked()
{
    close();
}

void unetbootin::downloadfile(QString fileurl, QString targetfile)
{
//    TODO Replace nsis downloader with one based on QNetwork
    QFile file("dlurl.txt");
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
    return;
    QTextStream out(&file);
    out << fileurl;
    file.close();
    QFile file2("outfile.txt");
    if (!file2.open(QIODevice::WriteOnly | QIODevice::Text))
    return;
    QTextStream out2(&file2);
    out2 << targetfile;
    file2.close();
//    TODO Replace ShellAPI with QProcess
    SHELLEXECUTEINFO ShExecInfo = {0};
    ShExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);
    ShExecInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
    ShExecInfo.hwnd = NULL;
    ShExecInfo.lpVerb = NULL;
    ShExecInfo.lpFile = L"downlder.exe";
    ShExecInfo.lpParameters = L"";
    ShExecInfo.lpDirectory = NULL;
    ShExecInfo.nShow = SW_SHOW;
    ShExecInfo.hInstApp = NULL;
    ShellExecuteEx(&ShExecInfo);
    WaitForSingleObject(ShExecInfo.hProcess,INFINITE);
}

void unetbootin::on_okbutton_clicked()
{
    if (radioFloppy->isChecked())
    {
//        printf(qPrintable(nameFloppy));
    }
    if (radioManual->isChecked())
    {
//        printf(qPrintable(nameKernel));
    }
    if (radioDistro->isChecked())
    {
        nameDistro = distroselect->currentText();
        targetDrive = driveselect->currentText();
        QString targetPath = QString("%1unetbtin\\").arg(targetDrive);
        QDir dir;
        dir.mkpath(targetPath);
//        printf(qPrintable(dir.currentPath()));
//        dir.drives();
        dir.setCurrent(targetPath);
        if (nameDistro == "Ubuntu 7.10")
        {
            downloadfile("http://archive.ubuntu.com/ubuntu/dists/gutsy/main/installer-i386/current/images/netboot/ubuntu-installer/i386/linux", QString("%1ubnkern").arg(targetPath));
            downloadfile("http://archive.ubuntu.com/ubuntu/dists/gutsy/main/installer-i386/current/images/netboot/ubuntu-installer/i386/initrd.gz", QString("%1ubninit").arg(targetPath));
        }
        if (nameDistro == "Ubuntu 7.10 x64")
        {
            downloadfile("http://archive.ubuntu.com/ubuntu/dists/gutsy/main/installer-amd64/current/images/netboot/ubuntu-installer/amd64/linux", QString("%1ubnkern").arg(targetPath));
            downloadfile("http://archive.ubuntu.com/ubuntu/dists/gutsy/main/installer-amd64/current/images/netboot/ubuntu-installer/amd64/initrd.gz", QString("%1ubninit").arg(targetPath));
        }
        if (nameDistro == "Ubuntu 8.04")
        {
            downloadfile("http://archive.ubuntu.com/ubuntu/dists/hardy/main/installer-i386/current/images/netboot/ubuntu-installer/i386/linux", QString("%1ubnkern").arg(targetPath));
            downloadfile("http://archive.ubuntu.com/ubuntu/dists/gutsy/main/installer-i386/current/images/netboot/ubuntu-installer/i386/initrd.gz", QString("%1ubninit").arg(targetPath));
        }
        if (nameDistro == "Ubuntu 8.04 x64")
        {
            downloadfile("http://archive.ubuntu.com/ubuntu/dists/hardy/main/installer-amd64/current/images/netboot/ubuntu-installer/amd64/linux", QString("%1ubnkern").arg(targetPath));
            downloadfile("http://archive.ubuntu.com/ubuntu/dists/hardy/main/installer-amd64/current/images/netboot/ubuntu-installer/amd64/initrd.gz", QString("%1ubninit").arg(targetPath));
        }
//        QString args = QApplication::arguments()[1];
//        printf(qPrintable(args));
//        for (int i = 0; i < args.size(); ++i)
//            printf(args.at(i).toLocal8Bit().constData());
//        printf(qPrintable(nameDistro));
//        if (nameDistro == "Ubuntu 7.10")
//        {
//            QUrl kernurl("http://archive.ubuntu.com/ubuntu/dists/gutsy/main/installer-i386/current/images/netboot/ubuntu-installer/i386/linux")
//        }
    }
    close();
}
