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
	QString kernelLine("kernel /unetbtin/ubnkern");
	QString kernelOpts;
	QString initrdLine("initrd /unetbtin/ubninit");
	QString initrdOpts;
	installType = typeselect->currentText();
    targetDrive = driveselect->currentText();
    targetPath = QString("%1\\unetbtin\\").arg(targetDrive);
    QFile file;
    QDir dir;
    dir.mkpath(targetPath);
//    QSettings ubnset("UNetbootin");
//    ubnset.setValue("location", QString("%1uninst.exe").arg(targetPath));
//    ubnset.~QSettings();
//    file.setFileName(QString("%1\\downlder.exe").arg(dir.currentPath()));
    file.copy(":/downlder.exe", QString("%1downlder.exe").arg(targetPath));
    file.copy(":/booteder.exe", QString("%1booteder.exe").arg(targetPath));
    file.copy(":/emtxfile.exe", QString("%1emtxfile.exe").arg(targetPath));
    file.copy(":/runxfile.exe", QString("%1runxfile.exe").arg(targetPath));
    file.copy(":/tr.exe", QString("%1downlder.exe").arg(targetPath));
    file.copy(":/ubnldr.exe", QString("%1\\ubnldr.exe").arg(targetDrive));
    file.copy(":/ubnldr", QString("%1\\ubnldr").arg(targetDrive));
    file.copy(":/ubnldr.mbr", QString("%1\\ubnldr.mbr").arg(targetDrive));
    file.copy(":/bootedit.bat", QString("%1bootedit.bat").arg(targetPath));
    file.copy(":/bootedit.pif", QString("%1bootedit.pif").arg(targetPath));
    file.copy(":/bootundo.bat", QString("%1bootundo.bat").arg(targetPath));
    file.copy(":/bootundo.pif", QString("%1bootundo.pif").arg(targetPath));
    file.copy(":/vbcdedit.bat", QString("%1vbcdedit.bat").arg(targetPath));
    file.copy(":/vbcdedit.pif", QString("%1vbcdedit.pif").arg(targetPath));
    file.copy(":/vbcdundo.bat", QString("%1vbcdundo.bat").arg(targetPath));
    file.copy(":/vbcdundo.pif", QString("%1vbcdundo.pif").arg(targetPath));
    file.copy(":/config.sup", QString("%1config.sup").arg(targetPath));
    file.copy(":/menu.lst", QString("%1menu.lst").arg(targetPath));
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
    	file.copy(":/memdisk", QString("%1ubnkern").arg(targetPath));
    	file.copy(FloppyPath->text(), QString("%1ubninit").arg(targetPath));
//        printf(qPrintable(nameFloppy));
    }
    if (radioManual->isChecked())
    {
    	file.copy(KernelPath->text(), QString("%1ubnkern").arg(targetPath));
    	file.copy(InitrdPath->text(), QString("%1ubninit").arg(targetPath));
//        printf(qPrintable(nameKernel));
    }
    if (radioDistro->isChecked())
    {
        nameDistro = distroselect->currentText();
        if (nameDistro == "Ubuntu 8.04")
        {
        	kernelOpts = "vga=normal";
            downloadfile("http://archive.ubuntu.com/ubuntu/dists/hardy/main/installer-i386/current/images/netboot/ubuntu-installer/i386/linux", QString("%1ubnkern").arg(targetPath));
            downloadfile("http://archive.ubuntu.com/ubuntu/dists/gutsy/main/installer-i386/current/images/netboot/ubuntu-installer/i386/initrd.gz", QString("%1ubninit").arg(targetPath));
        }
        if (nameDistro == "Ubuntu 8.04 x64")
        {
        	kernelOpts = "vga=normal";
            downloadfile("http://archive.ubuntu.com/ubuntu/dists/hardy/main/installer-amd64/current/images/netboot/ubuntu-installer/amd64/linux", QString("%1ubnkern").arg(targetPath));
            downloadfile("http://archive.ubuntu.com/ubuntu/dists/hardy/main/installer-amd64/current/images/netboot/ubuntu-installer/amd64/initrd.gz", QString("%1ubninit").arg(targetPath));
        }
        if (nameDistro == "Ubuntu 7.10")
        {
        	kernelOpts = "vga=normal";
            downloadfile("http://archive.ubuntu.com/ubuntu/dists/gutsy/main/installer-i386/current/images/netboot/ubuntu-installer/i386/linux", QString("%1ubnkern").arg(targetPath));
            downloadfile("http://archive.ubuntu.com/ubuntu/dists/gutsy/main/installer-i386/current/images/netboot/ubuntu-installer/i386/initrd.gz", QString("%1ubninit").arg(targetPath));
        }
        if (nameDistro == "Ubuntu 7.10 x64")
        {
        	kernelOpts = "vga=normal";
            downloadfile("http://archive.ubuntu.com/ubuntu/dists/gutsy/main/installer-amd64/current/images/netboot/ubuntu-installer/amd64/linux", QString("%1ubnkern").arg(targetPath));
            downloadfile("http://archive.ubuntu.com/ubuntu/dists/gutsy/main/installer-amd64/current/images/netboot/ubuntu-installer/amd64/initrd.gz", QString("%1ubninit").arg(targetPath));
        }
        if (nameDistro == "Ubuntu 7.04")
        {
        	kernelOpts = "vga=normal";
            downloadfile("http://archive.ubuntu.com/ubuntu/dists/feisty/main/installer-i386/current/images/netboot/ubuntu-installer/i386/linux", QString("%1ubnkern").arg(targetPath));
            downloadfile("http://archive.ubuntu.com/ubuntu/dists/feisty/main/installer-i386/current/images/netboot/ubuntu-installer/i386/initrd.gz", QString("%1ubninit").arg(targetPath));
        }
        if (nameDistro == "Ubuntu 7.04 x64")
        {
        	kernelOpts = "vga=normal";
            downloadfile("http://archive.ubuntu.com/ubuntu/dists/feisty/main/installer-amd64/current/images/netboot/ubuntu-installer/amd64/linux", QString("%1ubnkern").arg(targetPath));
            downloadfile("http://archive.ubuntu.com/ubuntu/dists/feisty/main/installer-amd64/current/images/netboot/ubuntu-installer/amd64/initrd.gz", QString("%1ubninit").arg(targetPath));
        }
        if (nameDistro == "Ubuntu 6.10")
        {
        	kernelOpts = "vga=normal";
            downloadfile("http://archive.ubuntu.com/ubuntu/dists/edgy/main/installer-i386/current/images/netboot/ubuntu-installer/i386/linux", QString("%1ubnkern").arg(targetPath));
            downloadfile("http://archive.ubuntu.com/ubuntu/dists/edgy/main/installer-i386/current/images/netboot/ubuntu-installer/i386/initrd.gz", QString("%1ubninit").arg(targetPath));
        }
        if (nameDistro == "Ubuntu 6.10 x64")
        {
        	kernelOpts = "vga=normal";
            downloadfile("http://archive.ubuntu.com/ubuntu/dists/edgy/main/installer-amd64/current/images/netboot/ubuntu-installer/amd64/linux", QString("%1ubnkern").arg(targetPath));
            downloadfile("http://archive.ubuntu.com/ubuntu/dists/edgy/main/installer-amd64/current/images/netboot/ubuntu-installer/amd64/initrd.gz", QString("%1ubninit").arg(targetPath));
        }
        if (nameDistro == "Ubuntu 6.06")
        {
        	kernelOpts = "vga=normal";
            downloadfile("http://archive.ubuntu.com/ubuntu/dists/dapper/main/installer-i386/current/images/netboot/ubuntu-installer/i386/linux", QString("%1ubnkern").arg(targetPath));
            downloadfile("http://archive.ubuntu.com/ubuntu/dists/dapper/main/installer-i386/current/images/netboot/ubuntu-installer/i386/initrd.gz", QString("%1ubninit").arg(targetPath));
        }
        if (nameDistro == "Ubuntu 6.06 x64")
        {
        	kernelOpts = "vga=normal";
            downloadfile("http://archive.ubuntu.com/ubuntu/dists/dapper/main/installer-amd64/current/images/netboot/ubuntu-installer/amd64/linux", QString("%1ubnkern").arg(targetPath));
            downloadfile("http://archive.ubuntu.com/ubuntu/dists/dapper/main/installer-amd64/current/images/netboot/ubuntu-installer/amd64/initrd.gz", QString("%1ubninit").arg(targetPath));
        }
        if (nameDistro == "Fedora 9 Alpha")
        {
        	kernelOpts = "splash=silent showopts";
            downloadfile("http://download.fedora.redhat.com/pub/fedora/linux/releases/test/9-Alpha/Fedora/i386/os/images/pxeboot/vmlinuz", QString("%1ubnkern").arg(targetPath));
            downloadfile("http://download.fedora.redhat.com/pub/fedora/linux/releases/test/9-Alpha/Fedora/i386/os/images/pxeboot/initrd.img", QString("%1ubninit").arg(targetPath));
        }
        if (nameDistro == "Fedora 9 Alpha x64")
        {
        	kernelOpts = "splash=silent showopts";
            downloadfile("http://download.fedora.redhat.com/pub/fedora/linux/releases/test/9-Alpha/Fedora/x86_64/os/images/pxeboot/vmlinuz", QString("%1ubnkern").arg(targetPath));
            downloadfile("http://download.fedora.redhat.com/pub/fedora/linux/releases/test/9-Alpha/Fedora/x86_64/os/images/pxeboot/initrd.img", QString("%1ubninit").arg(targetPath));
        }
        if (nameDistro == "Fedora 8")
        {
        	kernelOpts = "splash=silent showopts";
            downloadfile("http://download.fedora.redhat.com/pub/fedora/linux/releases/8/Fedora/i386/os/images/pxeboot/vmlinuz", QString("%1ubnkern").arg(targetPath));
            downloadfile("http://download.fedora.redhat.com/pub/fedora/linux/releases/8/Fedora/i386/os/images/pxeboot/initrd.img", QString("%1ubninit").arg(targetPath));
        }
        if (nameDistro == "Fedora 8 x64")
        {
        	kernelOpts = "splash=silent showopts";
            downloadfile("http://download.fedora.redhat.com/pub/fedora/linux/releases/8/Fedora/x86_64/os/images/pxeboot/vmlinuz", QString("%1ubnkern").arg(targetPath));
            downloadfile("http://download.fedora.redhat.com/pub/fedora/linux/releases/8/Fedora/x86_64/os/images/pxeboot/initrd.img", QString("%1ubninit").arg(targetPath));
        }
        if (nameDistro == "Fedora 7")
        {
        	kernelOpts = "splash=silent showopts";
            downloadfile("http://download.fedora.redhat.com/pub/fedora/linux/releases/7/Fedora/i386/os/images/pxeboot/vmlinuz", QString("%1ubnkern").arg(targetPath));
            downloadfile("http://download.fedora.redhat.com/pub/fedora/linux/releases/7/Fedora/i386/os/images/pxeboot/initrd.img", QString("%1ubninit").arg(targetPath));
        }
        if (nameDistro == "Fedora 7 x64")
        {
        	kernelOpts = "splash=silent showopts";
            downloadfile("http://download.fedora.redhat.com/pub/fedora/linux/releases/7/Fedora/x86_64/os/images/pxeboot/vmlinuz", QString("%1ubnkern").arg(targetPath));
            downloadfile("http://download.fedora.redhat.com/pub/fedora/linux/releases/7/Fedora/x86_64/os/images/pxeboot/initrd.img", QString("%1ubninit").arg(targetPath));
        }
        if (nameDistro == "openSUSE Factory")
        {
        	kernelOpts = "splash=silent showopts";
            downloadfile("http://download.opensuse.org/distribution/SL-OSS-factory/inst-source/boot/i386/loader/linux", QString("%1ubnkern").arg(targetPath));
            downloadfile("http://download.opensuse.org/distribution/SL-OSS-factory/inst-source/boot/i386/loader/initrd", QString("%1ubninit").arg(targetPath));
        }
        if (nameDistro == "openSUSE Factory x64")
        {
        	kernelOpts = "splash=silent showopts";
            downloadfile("http://download.opensuse.org/distribution/SL-OSS-factory/inst-source/boot/x86_64/loader/linux", QString("%1ubnkern").arg(targetPath));
            downloadfile("http://download.opensuse.org/distribution/SL-OSS-factory/inst-source/boot/x86_64/loader/initrd", QString("%1ubninit").arg(targetPath));
        }
        if (nameDistro == "openSUSE 10.3")
        {
        	kernelOpts = "splash=silent showopts";
            downloadfile("http://download.opensuse.org/distribution/10.3/repo/oss/boot/i386/loader/linux", QString("%1ubnkern").arg(targetPath));
            downloadfile("http://download.opensuse.org/distribution/10.3/repo/oss/boot/i386/loader/initrd", QString("%1ubninit").arg(targetPath));
        }
        if (nameDistro == "openSUSE 10.3 x64")
        {
        	kernelOpts = "splash=silent showopts";
            downloadfile("http://download.opensuse.org/distribution/10.3/repo/oss/boot/x86_64/loader/linux", QString("%1ubnkern").arg(targetPath));
            downloadfile("http://download.opensuse.org/distribution/10.3/repo/oss/boot/x86_64/loader/initrd", QString("%1ubninit").arg(targetPath));
        }
        if (nameDistro == "openSUSE 10.2")
        {
        	kernelOpts = "splash=silent showopts";
            downloadfile("http://download.opensuse.org/distribution/10.2/repo/oss/boot/i386/loader/linux", QString("%1ubnkern").arg(targetPath));
            downloadfile("http://download.opensuse.org/distribution/10.2/repo/oss/boot/i386/loader/initrd", QString("%1ubninit").arg(targetPath));
        }
        if (nameDistro == "openSUSE 10.2 x64")
        {
        	kernelOpts = "splash=silent showopts";
            downloadfile("http://download.opensuse.org/distribution/10.2/repo/oss/boot/x86_64/loader/linux", QString("%1ubnkern").arg(targetPath));
            downloadfile("http://download.opensuse.org/distribution/10.2/repo/oss/boot/x86_64/loader/initrd", QString("%1ubninit").arg(targetPath));
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
    if (installType == "Hard Disk")
    {
    	QFile menulst(QString("%1menu.lst").arg(targetPath));
    	if (!menulst.open(QIODevice::WriteOnly | QIODevice::Text))
		return;
		QTextStream menulstout(&menulst);
		QString menulstxt = QString("default 0\n"
		"timeout 3\n"
		"title UNetbootin\n"
		"find --set-root /unetbtin/ubnkern\n"
		"%1 %2\n"
		"%3 %4\n"
		"boot").arg(kernelLine, kernelOpts, initrdLine, initrdOpts);
		menulstout << menulstxt << endl;
    	QProcess inprocess;
    	inprocess.start(QString("%1booteder.exe").arg(targetPath));
    	inprocess.waitForFinished(-1);
    	QSettings install("HKEY_LOCAL_MACHINE\\Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\UNetbootin", QSettings::NativeFormat);
    	install.setValue("Location", targetDrive);
    	install.setValue("DisplayName", "UNetbootin");
    	install.setValue("UninstallString", QString("%1uninst.exe").arg(targetPath));
    	QSettings runonce("HKEY_LOCAL_MACHINE\\Software\\Microsoft\\Windows\\CurrentVersion\\RunOnce", QSettings::NativeFormat);
    	runonce.setValue("UNetbootin Uninstaller", QString("%1uninst.exe").arg(targetPath));
//		inprocess.~QProcess();
   	}
//    close();
//	if (installType == "USB Drive")
//	{
//		TODO
//	}
}
