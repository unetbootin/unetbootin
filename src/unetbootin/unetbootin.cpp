#include <QFile>
#include <QFileDialog>
#include <QTextStream>
#include <QProcess>
#include <QSettings>
#include "unetbootin.h"

//#include <windows.h>
//#include <shellapi.h>

unetbootin::unetbootin(QWidget *parent)
    : QWidget(parent)
{
	QSettings chkinst("HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\UNetbootin", QSettings::NativeFormat);
//	if (chkinst.contains("UninstallString"))
//	{
//		QProcess uninst;
//		QVariant uninstvar = chkinst.value("HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\UNetbootin");
//		QString uninstpath = uninstvar.value<QString>();
//		QString uninstpath = uninstvar.toString();
//		printf(qPrintable(uninstpath));
//		uninst.start(uninstpath);
//		uninst.waitForFinished(-1);
//		uninst.~QProcess();
//	}
//	chkinst.~QSettings();
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
//    file.~QFile();
    QFile file2("outfile.txt");
    if (!file2.open(QIODevice::WriteOnly | QIODevice::Text))
    return;
    QTextStream out2(&file2);
    out2 << targetfile;
    file2.close();
//    file2.~QFile();
    QProcess dlprocess;
    dlprocess.start(QString("%1downlder.exe").arg(targetPath));
    dlprocess.waitForFinished(-1);
//    dlprocess.~QProcess();
//    TODO Replace ShellAPI with QProcess
/*
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
*/
}

void unetbootin::on_okbutton_clicked()
{
    targetDrive = driveselect->currentText();
    targetPath = QString("%1unetbtin\\").arg(targetDrive);
    QString rootPath = QString("%1..\\").arg(targetPath);
    QFile file;
    QDir dir;
    dir.mkpath(targetPath);
//    QSettings ubnset("UNetbootin");
//    ubnset.setValue("location", QString("%1uninst.exe").arg(targetPath));
//    ubnset.~QSettings();
//    file.setFileName(QString("%1\\downlder.exe").arg(dir.currentPath()));
    file.copy(QString("%1\\downlder.exe").arg(appDir), QString("%1downlder.exe").arg(targetPath));
    file.copy(QString("%1\\booteder.exe").arg(appDir), QString("%1booteder.exe").arg(targetPath));
    file.copy(QString("%1\\emtxfile.exe").arg(appDir), QString("%1emtxfile.exe").arg(targetPath));
    file.copy(QString("%1\\runxfile.exe").arg(appDir), QString("%1runxfile.exe").arg(targetPath));
    file.copy(QString("%1\\tr.exe").arg(appDir), QString("%1downlder.exe").arg(targetPath));
    file.copy(QString("%1\\ubnldr.exe").arg(appDir), QString("%1ubnldr.exe").arg(rootPath));
    file.copy(QString("%1\\ubnldr").arg(appDir), QString("%1ubnldr").arg(rootPath));
    file.copy(QString("%1\\ubnldr.mbr").arg(appDir), QString("%1ubnldr.mbr").arg(rootPath));
    file.copy(QString("%1\\bootedit.bat").arg(appDir), QString("%1bootedit.bat").arg(targetPath));
    file.copy(QString("%1\\bootedit.pif").arg(appDir), QString("%1bootedit.pif").arg(targetPath));
    file.copy(QString("%1\\bootundo.bat").arg(appDir), QString("%1bootundo.bat").arg(targetPath));
    file.copy(QString("%1\\bootundo.pif").arg(appDir), QString("%1bootundo.pif").arg(targetPath));
    file.copy(QString("%1\\vbcdedit.bat").arg(appDir), QString("%1vbcdedit.bat").arg(targetPath));
    file.copy(QString("%1\\vbcdedit.pif").arg(appDir), QString("%1vbcdedit.pif").arg(targetPath));
    file.copy(QString("%1\\vbcdundo.bat").arg(appDir), QString("%1vbcdundo.bat").arg(targetPath));
    file.copy(QString("%1\\vbcdundo.pif").arg(appDir), QString("%1vbcdundo.pif").arg(targetPath));
    file.copy(QString("%1\\config.sup").arg(appDir), QString("%1config.sup").arg(targetPath));
    file.copy(QString("%1\\menu.lst").arg(appDir), QString("%1menu.lst").arg(targetPath));
//    CopyFile("unetbootin.exe", "unetbootin2.exe", false);
//    printf(qPrintable(QString("Appdir is %1").arg(appDir)));
//    printf(qPrintable(dir.currentPath()));
//    printf(qPrintable(app.applicationDirPath()));
//    dir.drives();
    dir.setCurrent(targetPath);
    close();
//    file.~QFile();
//    dir.~QDir();
    if (radioFloppy->isChecked())
    {
    	file.copy(QString("%1\\memdisk").arg(appDir), QString("%1\\ubnkern").arg(targetPath));
    	file.copy(FloppyPath->text(), QString("%1\\ubninit").arg(targetPath));
//        printf(qPrintable(nameFloppy));
    }
    if (radioManual->isChecked())
    {
    	file.copy(KernelPath->text(), QString("%1\\ubnkern").arg(targetPath));
    	file.copy(InitrdPath->text(), QString("%1\\ubninit").arg(targetPath));
//        printf(qPrintable(nameKernel));
    }
    if (radioDistro->isChecked())
    {
        nameDistro = distroselect->currentText();
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
    QProcess inprocess;
    inprocess.start(QString("%1booteder.exe").arg(targetPath));
    inprocess.waitForFinished(-1);
//    inprocess.~QProcess();
//    close();
}
