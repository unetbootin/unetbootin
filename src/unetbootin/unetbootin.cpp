#include <QFile>
#include <QFileDialog>
#include <QTextStream>
#include <QProcess>
#include <QSettings>
#include <QSysInfo>
#include <QMessageBox>
#include "unetbootin.h"

unetbootin::unetbootin(QWidget *parent)
    : QWidget(parent)
{
    setupUi(this);
}

void unetbootin::on_FloppyFileSelector_clicked()
{
    nameFloppy = QFileDialog::getOpenFileName(this, "Open File", QDir::homePath());
    FloppyPath->clear();
    FloppyPath->insert(nameFloppy);
    radioFloppy->setChecked(1);
}

void unetbootin::on_KernelFileSelector_clicked()
{
    nameKernel = QFileDialog::getOpenFileName(this, "Open File", QDir::homePath());
    KernelPath->clear();
    KernelPath->insert(nameKernel);
    radioManual->setChecked(1);
}

void unetbootin::on_InitrdFileSelector_clicked()
{
    nameInitrd = QFileDialog::getOpenFileName(this, "Open File", QDir::homePath());
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
    QFile file1("dlurl.txt");
    if (!file1.open(QIODevice::WriteOnly | QIODevice::Text))
    return;
    QTextStream out1(&file1);
    out1 << fileurl;
    file1.close();
    QFile file2("outfile.txt");
    if (!file2.open(QIODevice::WriteOnly | QIODevice::Text))
    return;
    QTextStream out2(&file2);
    out2 << targetfile;
    file2.close();
    QProcess dlprocess;
    dlprocess.start(QString("%1downlder.exe").arg(targetPath));
    dlprocess.waitForFinished(-1);
}

void unetbootin::on_okbutton_clicked()
{
	runinst();
}

void unetbootin::runinst()
{
	QString kernelLine("kernel /unetbtin/ubnkern");
	QString kernelOpts;
	QString initrdLine("initrd /unetbtin/ubninit");
	QString initrdOpts;
	installType = typeselect->currentText();
    targetDrive = driveselect->currentText();
    targetPath = QString("%1\\unetbtin\\").arg(targetDrive);
    QDir dir;
    dir.mkpath(targetPath);
    QFile::copy(QString("%1downlder.exe").arg(appDir), QString("%1downlder.exe").arg(targetPath));
    QFile::copy(QString("%1booteder.exe").arg(appDir), QString("%1booteder.exe").arg(targetPath));
    QFile::copy(QString("%1emtxfile.exe").arg(appDir), QString("%1emtxfile.exe").arg(targetPath));
    QFile::copy(QString("%1runxfile.exe").arg(appDir), QString("%1runxfile.exe").arg(targetPath));
    QFile::copy(QString("%1tr.exe").arg(appDir), QString("%1tr.exe").arg(targetPath));
    QFile::copy(QString("%1ubnldr.exe").arg(appDir), QString("%1\\ubnldr.exe").arg(targetDrive));
    QFile::copy(QString("%1ubnldr").arg(appDir), QString("%1\\ubnldr").arg(targetDrive));
    QFile::copy(QString("%1ubnldr.mbr").arg(appDir), QString("%1\\ubnldr.mbr").arg(targetDrive));
    QFile::copy(QString("%1bootedit.bat").arg(appDir), QString("%1bootedit.bat").arg(targetPath));
    QFile::copy(QString("%1bootedit.pif").arg(appDir), QString("%1bootedit.pif").arg(targetPath));
    QFile::copy(QString("%1bootundo.bat").arg(appDir), QString("%1bootundo.bat").arg(targetPath));
    QFile::copy(QString("%1bootundo.pif").arg(appDir), QString("%1bootundo.pif").arg(targetPath));
    QFile::copy(QString("%1vbcdedit.bat").arg(appDir), QString("%1vbcdedit.bat").arg(targetPath));
    QFile::copy(QString("%1vbcdedit.pif").arg(appDir), QString("%1vbcdedit.pif").arg(targetPath));
    QFile::copy(QString("%1vbcdundo.bat").arg(appDir), QString("%1vbcdundo.bat").arg(targetPath));
    QFile::copy(QString("%1vbcdundo.pif").arg(appDir), QString("%1vbcdundo.pif").arg(targetPath));
    QFile::copy(QString("%1config.sup").arg(appDir), QString("%1config.sup").arg(targetPath));
    QFile::copy(QString("%1menu.lst").arg(appDir), QString("%1menu.lst").arg(targetPath));
	QFile::copy(QString("%1unetbtin.exe").arg(appDir), QString("%1unetbtin.exe").arg(targetPath));
    dir.setCurrent(targetPath);
    close();
    if (radioFloppy->isChecked())
    {
    	QFile::copy(QString("%1memdisk").arg(appDir), QString("%1ubnkern").arg(targetPath));
    	QFile::copy(FloppyPath->text(), QString("%1ubninit").arg(targetPath));
    }
    if (radioManual->isChecked())
    {
    	QFile::copy(KernelPath->text(), QString("%1ubnkern").arg(targetPath));
    	QFile::copy(InitrdPath->text(), QString("%1ubninit").arg(targetPath));
		kernelOpts = OptionEnter->text();
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
        if (nameDistro == "CentOS 5")
        {
        	kernelOpts = "splash=silent showopts";
            downloadfile("http://isoredirect.centos.org/centos/5/os/i386/images/pxeboot/vmlinuz", QString("%1ubnkern").arg(targetPath));
            downloadfile("http://isoredirect.centos.org/centos/5/os/i386/images/pxeboot/initrd.img", QString("%1ubninit").arg(targetPath));
        }
        if (nameDistro == "CentOS 5 x64")
        {
        	kernelOpts = "splash=silent showopts";
            downloadfile("http://isoredirect.centos.org/centos/5/os/x86_64/images/pxeboot/vmlinuz", QString("%1ubnkern").arg(targetPath));
            downloadfile("http://isoredirect.centos.org/centos/5/os/x86_64/images/pxeboot/initrd.img", QString("%1ubninit").arg(targetPath));
        }
        if (nameDistro == "CentOS 4")
        {
        	kernelOpts = "splash=silent showopts";
            downloadfile("http://isoredirect.centos.org/centos/4/os/i386/images/pxeboot/vmlinuz", QString("%1ubnkern").arg(targetPath));
            downloadfile("http://isoredirect.centos.org/centos/4/os/i386/images/pxeboot/initrd.img", QString("%1ubninit").arg(targetPath));
        }
        if (nameDistro == "CentOS 4 x64")
        {
        	kernelOpts = "splash=silent showopts";
            downloadfile("http://isoredirect.centos.org/centos/4/os/x86_64/images/pxeboot/vmlinuz", QString("%1ubnkern").arg(targetPath));
            downloadfile("http://isoredirect.centos.org/centos/4/os/x86_64/images/pxeboot/initrd.img", QString("%1ubninit").arg(targetPath));
        }
        if (nameDistro == "Debian Stable")
        {
        	kernelOpts = "video=vesa:ywrap,mtrr vga=788 installgui";
            downloadfile("http://ftp.debian.org/debian/dists/stable/main/installer-i386/current/images/netboot/gtk/debian-installer/i386/linux", QString("%1ubnkern").arg(targetPath));
            downloadfile("http://ftp.debian.org/debian/dists/stable/main/installer-i386/current/images/netboot/gtk/debian-installer/i386/initrd.gz", QString("%1ubninit").arg(targetPath));
        }
        if (nameDistro == "Debian Stable x64")
        {
        	kernelOpts = "video=vesa:ywrap,mtrr vga=788 installgui";
            downloadfile("http://ftp.debian.org/debian/dists/stable/main/installer-amd64/current/images/netboot/gtk/debian-installer/amd64/linux", QString("%1ubnkern").arg(targetPath));
            downloadfile("http://ftp.debian.org/debian/dists/stable/main/installer-amd64/current/images/netboot/gtk/debian-installer/amd64/initrd.gz", QString("%1ubninit").arg(targetPath));
        }
        if (nameDistro == "Debian Testing")
        {
        	kernelOpts = "video=vesa:ywrap,mtrr vga=788 installgui";
            downloadfile("http://ftp.debian.org/debian/dists/testing/main/installer-i386/current/images/netboot/gtk/debian-installer/i386/linux", QString("%1ubnkern").arg(targetPath));
            downloadfile("http://ftp.debian.org/debian/dists/testing/main/installer-i386/current/images/netboot/gtk/debian-installer/i386/initrd.gz", QString("%1ubninit").arg(targetPath));
        }
        if (nameDistro == "Debian Testing x64")
        {
        	kernelOpts = "video=vesa:ywrap,mtrr vga=788 installgui";
            downloadfile("http://ftp.debian.org/debian/dists/testing/main/installer-amd64/current/images/netboot/gtk/debian-installer/amd64/linux", QString("%1ubnkern").arg(targetPath));
            downloadfile("http://ftp.debian.org/debian/dists/testing/main/installer-amd64/current/images/netboot/gtk/debian-installer/amd64/initrd.gz", QString("%1ubninit").arg(targetPath));
        }
        if (nameDistro == "Debian Unstable")
        {
        	kernelOpts = "video=vesa:ywrap,mtrr vga=788 installgui";
            downloadfile("http://ftp.debian.org/debian/dists/unstable/main/installer-i386/current/images/netboot/gtk/debian-installer/i386/linux", QString("%1ubnkern").arg(targetPath));
            downloadfile("http://ftp.debian.org/debian/dists/unstable/main/installer-i386/current/images/netboot/gtk/debian-installer/i386/initrd.gz", QString("%1ubninit").arg(targetPath));
        }
        if (nameDistro == "Debian Unstable x64")
        {
        	kernelOpts = "video=vesa:ywrap,mtrr vga=788 installgui";
            downloadfile("http://ftp.debian.org/debian/dists/unstable/main/installer-amd64/current/images/netboot/gtk/debian-installer/amd64/linux", QString("%1ubnkern").arg(targetPath));
            downloadfile("http://ftp.debian.org/debian/dists/unstable/main/installer-amd64/current/images/netboot/gtk/debian-installer/amd64/initrd.gz", QString("%1ubninit").arg(targetPath));
        }
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
    	/*
		QSysInfo::WinVersion wvr = QSysInfo::WindowsVersion;
		if (wvr == QSysInfo::WV_DOS_based)
		{
//			TODO
		}
		else if (wvr == QSysInfo::WV_NT_based)
		{
			if (wvr == QSysInfo::WV_NT || wvr == QSysInfo::WV_2000 || wvr == QSysInfo::WV_XP || wvr == QSysInfo::WV_2003 )
			{
//				TODO
			}
			else if (wvr == QSysInfo::WV_VISTA)
			{
//				TODO
			}
			else
			{
//				TODO
			}
//			TODO
		}
		   	*/
   	}
/*
	if (installType == "USB Drive")
	{
		TODO
	}
*/
}
