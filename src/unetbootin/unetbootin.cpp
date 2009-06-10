/*
unetbootin.cpp from UNetbootin <http://unetbootin.sourceforge.net>
Copyright (C) 2007-2008 Geza Kovacs <geza0kovacs@gmail.com>

This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 2 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License at <http://www.gnu.org/licenses/> for more details.
*/

#include "unetbootin.h"

static const QList<QRegExp> ignoredtypesbothRL = QList<QRegExp>()
<< QRegExp("isolinux.bin$", Qt::CaseInsensitive)
<< QRegExp("isolinux.cfg$", Qt::CaseInsensitive)
<< QRegExp("memtest$", Qt::CaseInsensitive)
<< QRegExp("memtest.bin$", Qt::CaseInsensitive)
<< QRegExp("memtest86$", Qt::CaseInsensitive)
<< QRegExp("memtest86.bin$", Qt::CaseInsensitive)
<< QRegExp("memtest+$", Qt::CaseInsensitive)
<< QRegExp("memtest+.bin$", Qt::CaseInsensitive)
<< QRegExp("memtest86+$", Qt::CaseInsensitive)
<< QRegExp("memtest86+.bin$", Qt::CaseInsensitive)
<< QRegExp("memtestplus$", Qt::CaseInsensitive)
<< QRegExp("memtestplus.bin$", Qt::CaseInsensitive)
<< QRegExp("memtest86plus$", Qt::CaseInsensitive)
<< QRegExp("memtest86plus.bin$", Qt::CaseInsensitive)
<< QRegExp("mt86plus$", Qt::CaseInsensitive)
<< QRegExp("mt86+$", Qt::CaseInsensitive)
<< QRegExp("mt86.bin$", Qt::CaseInsensitive)
<< QRegExp("mt86plus.bin$", Qt::CaseInsensitive)
<< QRegExp("mt86+.bin$", Qt::CaseInsensitive)
<< QRegExp("system.map$", Qt::CaseInsensitive)
<< QRegExp(".efimg$", Qt::CaseInsensitive)
<< QRegExp(".html$", Qt::CaseInsensitive)
<< QRegExp(".jpg$", Qt::CaseInsensitive)
<< QRegExp(".png$", Qt::CaseInsensitive)
<< QRegExp(".pdf$", Qt::CaseInsensitive)
<< QRegExp(".pcx$", Qt::CaseInsensitive)
<< QRegExp(".rle$", Qt::CaseInsensitive)
<< QRegExp(".fnt$", Qt::CaseInsensitive)
<< QRegExp(".psd$", Qt::CaseInsensitive)
<< QRegExp(".xcf$", Qt::CaseInsensitive)
<< QRegExp(".bmp$", Qt::CaseInsensitive)
<< QRegExp(".svg$", Qt::CaseInsensitive)
<< QRegExp(".md5$", Qt::CaseInsensitive)
<< QRegExp(".md5sum$", Qt::CaseInsensitive)
<< QRegExp(".sha1$", Qt::CaseInsensitive)
<< QRegExp(".sha1sum$", Qt::CaseInsensitive)
<< QRegExp(".c32$", Qt::CaseInsensitive)
<< QRegExp(".sig$", Qt::CaseInsensitive)
<< QRegExp(".msg$", Qt::CaseInsensitive)
<< QRegExp(".cat$", Qt::CaseInsensitive)
<< QRegExp(".txt$", Qt::CaseInsensitive)
<< QRegExp(".tar$", Qt::CaseInsensitive)
<< QRegExp(".efi$", Qt::CaseInsensitive)
<< QRegExp(".exe$", Qt::CaseInsensitive)
<< QRegExp("system.map", Qt::CaseInsensitive);

static const QList<QRegExp> ignoredtypeskernelRL = QList<QRegExp>()
<< QRegExp("initrd.gz$", Qt::CaseInsensitive)
<< QRegExp("initrd.img$", Qt::CaseInsensitive);

static const QList<QRegExp> ignoredtypesinitrdRL = QList<QRegExp>()
<< QRegExp("bzImage$", Qt::CaseInsensitive);

void callexternappT::run()
{
	#ifdef Q_OS_WIN32
	SHELLEXECUTEINFO ShExecInfo = {0};
	ShExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);
	ShExecInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
	ShExecInfo.hwnd = NULL;
	if (QSysInfo::WindowsVersion == QSysInfo::WV_NT || QSysInfo::WindowsVersion == QSysInfo::WV_2000 || QSysInfo::WindowsVersion == QSysInfo::WV_XP || QSysInfo::WindowsVersion == QSysInfo::WV_2003 )
	{
		ShExecInfo.lpVerb = NULL;
	}
	else
	{
		ShExecInfo.lpVerb = L"runas";
	}
	ShExecInfo.lpFile = LPWSTR(execFile.utf16());
	ShExecInfo.lpParameters = LPWSTR(execParm.utf16());
	ShExecInfo.lpDirectory = NULL;
	ShExecInfo.nShow = SW_HIDE;
	ShExecInfo.hInstApp = NULL;
	ShellExecuteEx(&ShExecInfo);
	WaitForSingleObject(ShExecInfo.hProcess,INFINITE);
	retnValu = "";
	#endif
	#ifdef Q_OS_UNIX
	QProcess lnexternapp;
	lnexternapp.start(QString("%1 %2").arg(execFile).arg(execParm));
	lnexternapp.waitForFinished(-1);
	retnValu = QString(lnexternapp.readAll());
	#endif
}

ubngetrequestheader::ubngetrequestheader(QString urhost, QString urpath)
{
	this->setRequest("GET", urpath);
	this->setValue("HOST", urhost);
	this->setValue("User-Agent", "UNetbootin/1.1.1");
//	this->setValue("User-Agent", "Wget/1.10.2");
	this->setValue("Accept", "*/*");
	this->setValue("Connection", "Keep-Alive");
}

randtmpfile::randtmpfile(QString rfpath, QString rfextn)
{
	QString basefn = getrandfilename(rfpath, rfextn);
	this->setFileName(basefn);
}

QString randtmpfile::getrandfilename(QString rfpath, QString rfextn)
{
	qsrand((unsigned int)time(0));
	QString basefn = QString("%1un%2.%3").arg(rfpath).arg(qrand() % 999999).arg(rfextn);
	while (QFile::exists(basefn))
	{
		basefn = QString("%1%2.%3").arg(rfpath).arg(qrand() % 999999).arg(rfextn);
	}
	return basefn;
}

void nDirListStor::sAppendSelfUrlInfoList(QUrlInfo curDirUrl)
{
	if (curDirUrl.isValid() && curDirUrl.isReadable() && curDirUrl.isFile() && curDirUrl.size() > nMinFileSizeBytes && curDirUrl.size() < nMaxFileSizeBytes)
	{
		nDirFileListSL.append(curDirUrl.name());
	}
	else if (this->searchsymlinks && curDirUrl.isReadable() && curDirUrl.isSymLink())
	{
		nDirFileListSL.append(curDirUrl.name());
	}
}

unetbootin::unetbootin(QWidget *parent)
	: QWidget(parent)
{
	setupUi(this);
}

void unetbootin::ubninitialize()
{
	this->searchsymlinks = false;
	secondlayer->setEnabled(false);
	secondlayer->hide();
	firstlayer->setEnabled(true);
	firstlayer->show();
	this->setWindowTitle(UNETBOOTINB);
	overwriteall = false;
	formatdrivecheckbox->setEnabled(false);
	formatdrivecheckbox->hide();
	typeselect->addItem(tr("Hard Disk"));
	typeselect->addItem(tr("USB Drive"));
	diskimagetypeselect->addItem(tr("ISO"));
	diskimagetypeselect->addItem(tr("Floppy"));
	#ifdef NOMANUAL
	optionslayer->setEnabled(true);
	optionslayer->show();
	customlayer->setEnabled(false);
	customlayer->hide();
	radioManual->setEnabled(false);
	radioManual->hide();
	diskimagelayer->move(diskimagelayer->x(), diskimagelayer->y() + 80);
	radioFloppy->move(radioFloppy->x(), radioFloppy->y() + 80);
	intromessage->resize(intromessage->width(), intromessage->height() + 80);
	#endif
	#ifdef NOEXTERN
	optionslayer->setEnabled(false);
	optionslayer->hide();
	radioFloppy->setEnabled(false);
	radioFloppy->hide();
	radioManual->setEnabled(false);
	radioManual->hide();
	intromessage->resize(intromessage->width(), intromessage->height() + 135);
	#endif
	#ifdef NOFLOPPY
	if (diskimagetypeselect->findText(tr("Floppy")) != -1)
		diskimagetypeselect->removeItem(diskimagetypeselect->findText(tr("Floppy")));
	#endif
	#ifdef NOISO
	if (diskimagetypeselect->findText(tr("ISO")) != -1)
		diskimagetypeselect->removeItem(diskimagetypeselect->findText(tr("ISO")));
	#endif
        #ifdef AUTOSUPERGRUBDISK
        distroselect->addItem("Super Grub Disk", (QStringList() << "Latest" <<
        tr("<b>Homepage:</b> <a href=\"http://www.supergrubdisk.org\">http://www.supergrubdisk.org</a><br/>"
                "<b>Description:</b> Super Grub Disk is a bootloader which can perform a variety of MBR and bootloader recovery tasks.<br/>"
                "<b>Install Notes:</b> SGD simply boots and runs; no installation is required to use it.") <<
        "Latest"));
	QFile asgdDescF;
	if (QFile::exists(QString(":/asgd_%1.htm").arg(appNlang)))
	{
		asgdDescF.setFileName(QString(":/asgd_%1.htm").arg(appNlang));
	}
	else
	{
		asgdDescF.setFileName(":/asgd_en.htm");
	}
	asgdDescF.open(QIODevice::ReadOnly | QIODevice::Text);
	QTextStream asgdDescS(&asgdDescF);
	distroselect->addItem("Auto Super Grub Disk", (QStringList() << "1.0" << 
	asgdDescS.readAll() << 
	"1.0"));
	#endif
	#ifdef EEEPCLOS
	distroselect->addItem("EeePCLinuxOS", (QStringList() << "pre2008_Live" << 
	tr("<img src=\":/eeepclos.png\" /><br/>"
		"<b>Homepage:</b> <a href=\"http://www.eeepclinuxos.com/\">http://www.eeepclinuxos.com</a><br/>"
		"<b>Description:</b> EeePCLinuxOS is a user-friendly PCLinuxOS based distribution for the EeePC.<br/>"
		"<b>Install Notes:</b> Make sure install media is empty and formatted before proceeding with install.") << 
	"pre2008_Live"));
	#endif
	#ifdef EEEUBUNTU
	distroselect->addItem("Ubuntu Eee", (QStringList() << "8.04" << 
	tr("<img src=\":/eeeubuntu.png\" style=\"float:left;\" /><br/>"
		"<b>Homepage:</b> <a href=\"http://www.ubuntu-eee.com/\">http://www.ubuntu-eee.com</a><br/>"
		"<b>Description:</b> Ubuntu Eee is not only Ubuntu optimized for the Asus Eee PC. It's an operating system, using the Netbook Remix interface, which favors the best software available instead of open source alternatives (ie. Skype instead of Ekiga).<br/>"
		"<b>Install Notes:</b> Make sure install media is empty and formatted before proceeding with install.") << 
//	tr("<img src=\":/eeeubuntu.png\" /><br/>"
//		"<b>Homepage:</b> <a href=\"http://www.ubuntu-eee.com/\">http://www.ubuntu-eee.com</a><br/>"
//		"<b>Description:</b> Ubuntu Eee is a user-friendly Ubuntu based distribution for the EeePC.<br/>"
//		"<b>Install Notes:</b> Make sure install media is empty and formatted before proceeding with install.") << 
	"8.04"));
	#endif
	#ifdef ELIVE
	distroselect->addItem("Elive", (QStringList() << "Unstable_Live" << 
	tr("<img src=\":/elive.png\" /><br/>"
		"<b>Homepage:</b> <a href=\"http://www.elivecd.org/\">http://www.elivecd.org</a><br/>"
		"<b>Description:</b> Elive is a Debian-based distribution featuring the Enlightenment window manager.<br/>"
		"<b>Install Notes:</b> The Live version allows for booting in Live mode, from which the installer can optionally be launched. The Unstable version, not the <a href=\"http://www.elivecd.org/Download/Stable\">Stable version</a> is installed. This installer is based on <a href=\"http://unetbootin.sourceforge.net/\">UNetbootin</a>.") << 
	"Unstable_Live"));
	#endif

        #ifdef KIWILINUX
        distroselect->addItem("Kiwi Linux", (QStringList() << "9.04" <<
        tr("<img src=\":/kiwi_logo_ro.png\" /><br/>"
        "<b>Homepage:</b> <a href=\"http://www.kiwilinux.org/\">http://www.kiwilinux.org</a><br/>"
        "<b>Description:</b> Kiwi Linux is an Ubuntu derivative primarily made for Romanian, Hungarian and English speaking users.<br/>"
        "<b>Install Notes:</b> Make sure install media is empty and formatted before proceeding with install.") <<
        "9.04"));
        #endif



	#ifdef GNEWSENSE
	distroselect->addItem("gNewSense", (QStringList() << "deltah-2.1" << 
	tr("<img src=\":/gnewsense.png\" /><br/>"
		"<b>Homepage:</b> <a href=\"http://www.gnewsense.org/\">http://www.gnewsense.org</a><br/>"
		"<b>Description:</b> gNewSense is a high-quality GNU/Linux distribution that extends and improves Ubuntu to create a completely free operating system without any binary blobs or package trees that contain proprietary software.<br/>"
		"<b>Install Notes:</b> Make sure install media is empty and formatted before proceeding with install.") << 
	"deltah-2.1"));
	#endif
	#ifdef SLITAZ
	distroselect->addItem("SliTaz", (QStringList() << "Stable" << 
	tr("<img src=\":/slitaz.png\" /><br/>"
		"<b>Homepage:</b> <a href=\"http://www.slitaz.org/en/\">http://www.slitaz.org/en</a><br/>"
		"<b>Description:</b> SliTaz is a lightweight, desktop-oriented micro distribution.<br/>"
		"<b>Install Notes:</b> The Live version loads the entire system into RAM and boots from memory, so installation is not required but optional. This installer is based on <a href=\"http://unetbootin.sourceforge.net/\">UNetbootin</a>.") << 
	"Stable" << "Cooking" << "Webboot"));
	#endif
	#ifdef XPUD
	distroselect->addItem("xPUD", (QStringList() << "Latest_Live" <<
	tr("<img src=\":/xpud.png\" /><br/>"
		"<b>Homepage:</b> <a href=\"http://www.xpud.org/\">http://www.xpud.org</a><br/>"
		"<b>Description:</b> xPUD is a lightweight distribution featuring a simple kiosk-like interface with a web browser and media player.<br/>"
		"<b>Install Notes:</b> The Live version loads the entire system into RAM and boots from memory.") << 
	"Latest_Live"));
	#endif
	#ifdef STDUNETBOOTIN
	optionslayer->setEnabled(true);
	optionslayer->show();
	distroselect->addItem("== Select Distribution ==", (QStringList() << "== Select Version ==" << 
	tr("Welcome to <a href=\"http://unetbootin.sourceforge.net/\">UNetbootin</a>, the Universal Netboot Installer. Usage:"
		"<ol><li>Select a distribution and version to download from the list above, or manually specify files to load below.</li>"
		"<li>Select an installation type, and press OK to begin installing.</li></ol>") << 
	"== Select Version =="));
	distroselect->addItem("Arch Linux", (QStringList() << "2008.03-1" << 
	tr("<b>Homepage:</b> <a href=\"http://www.archlinux.org/\">http://www.archlinux.org</a><br/>"
		"<b>Description:</b> Arch Linux is a lightweight distribution optimized for speed and flexibility.<br/>"
		"<b>Install Notes:</b> The default version allows for installation over the internet (FTP). If interested in a LiveUSB version, see FaunOS.") << 
	"2007.08-2" << "2007.08-2_x64" << "2008.03-1" << "2008.03-1_x64"));
//	"2007.08-2" << "2007.08-2_x64" << "2008.03-1" << "2008.03-1_x64" << "2008.06" << "2008.06_x64"));
	distroselect->addItem("BackTrack", (QStringList() << "3" << 
	tr("<b>Homepage:</b> <a href=\"http://www.remote-exploit.org/backtrack.html\">http://www.remote-exploit.org/backtrack.html</a><br/>"
		"<b>Description:</b> BackTrack is a distribution focused on network analysis and penetration testing.<br/>"
		"<b>Install Notes:</b> BackTrack is booted and run in live mode; no installation is required to use it.") << 
	"3"));
	distroselect->addItem("CentOS", (QStringList() << "5" << 
	tr("<b>Homepage:</b> <a href=\"http://www.centos.org/\">http://www.centos.org</a><br/>"
		"<b>Description:</b> CentOS is a free Red Hat Enterprise Linux clone.<br/>"
		"<b>Install Notes:</b> The default version allows for both installation over the internet (FTP), or offline installation using pre-downloaded installation ISO files.") << 
	"4" << "4_x64" << "5" << "5_x64"));
	distroselect->addItem("CloneZilla", (QStringList() << "1.1.0-8" << 
	tr("<b>Homepage:</b> <a href=\"http://clonezilla.org/\">http://clonezilla.org/</a><br/>"
		"<b>Description:</b> CloneZilla is a distribution used for disk backup and imaging.<br/>"
		"<b>Install Notes:</b> CloneZilla is booted and run in live mode; no installation is required to use it.") << 
	"1.1.0-8"));
	distroselect->addItem("Damn Small Linux", (QStringList() << "Latest_Live" << 
	tr("<b>Homepage:</b> <a href=\"http://damnsmalllinux.org/\">http://damnsmalllinux.org</a><br/>"
		"<b>Description:</b> Damn Small Linux is a minimalist distribution designed for older computers.<br/>"
		"<b>Install Notes:</b> The Live version loads the entire system into RAM and boots from memory, so installation is not required but optional.") << 
	"Latest_Live"));
	distroselect->addItem("Debian", (QStringList() << "Stable_NetInstall" << 
	tr("<b>Homepage:</b> <a href=\"http://www.debian.org/\">http://www.debian.org</a><br/>"
		"<b>Description:</b> Debian is a community-developed Linux distribution that supports a wide variety of architectures and offers a large repository of packages.<br/>"
		"<b>Install Notes:</b> The Live version allows for booting in Live mode, from which the installer can optionally be launched. The NetInstall version allows for installation over FTP.") << 
	"Stable_NetInstall" << "Stable_NetInstall_x64" << "Testing_NetInstall" << "Testing_NetInstall_x64" << "Unstable_NetInstall" << "Unstable_NetInstall_x64"));
//	"Stable_NetInstall" << "Stable_NetInstall_x64" << "Stable_Live" << "Testing_NetInstall" << "Testing_NetInstall_x64" << "Testing_Live" << "Unstable_NetInstall" << "Unstable_NetInstall_x64" << "Unstable_Live"));
	distroselect->addItem("Dreamlinux", (QStringList() << "Latest_Live" << 
	tr("<b>Homepage:</b> <a href=\"http://www.dreamlinux.com.br/\">http://www.dreamlinux.com.br</a><br/>"
		"<b>Description:</b> Dreamlinux is a user-friendly Debian-based distribution.<br/>"
		"<b>Install Notes:</b> The Live version allows for booting in Live mode, from which the installer can optionally be launched.") << 
	"Latest_Live"));
	distroselect->addItem("Dr.Web AntiVirus", (QStringList() << "Latest_Live" << 
	tr("<b>Homepage:</b> <a href=\"http://www.freedrweb.com/livecd\">http://www.freedrweb.com/livecd</a><br/>"
		"<b>Description:</b> Dr.Web AntiVirus is an anti-virus emergency kit to restore a system that broke due to malware.<br/>"
		"<b>Install Notes:</b> The Live version allows for booting in Live mode, from which malware scans can be launched.") << 
	"Latest_Live"));
	distroselect->addItem("Elive", (QStringList() << "Unstable_Live" << 
	tr("<b>Homepage:</b> <a href=\"http://www.elivecd.org/\">http://www.elivecd.org</a><br/>"
		"<b>Description:</b> Elive is a Debian-based distribution featuring the Enlightenment window manager.<br/>"
		"<b>Install Notes:</b> The Live version allows for booting in Live mode. The Unstable version does not support Hard Drive installations, though the <a href=\"http://www.elivecd.org/Download/Stable\">Stable version</a> (not freely downloadable) does.") << 
	"Unstable_Live"));
	distroselect->addItem("FaunOS", (QStringList() << "shadow-0.5.4-stable" << 
	tr("<b>Homepage:</b> <a href=\"http://www.faunos.com/\">http://www.faunos.com</a><br/>"
		"<b>Description:</b> FaunOS is a distribution based on Arch Linux.<br/>"
		"<b>Install Notes:</b> The Live version allows for booting in Live mode, from which the installer can optionally be launched.") << 
	"shadow-0.5.4-stable"));
        distroselect->addItem("Fedora", (QStringList() << "10_Live" <<
	tr("<b>Homepage:</b> <a href=\"http://fedoraproject.org/\">http://fedoraproject.org</a><br/>"
		"<b>Description:</b> Fedora is a Red Hat sponsored community distribution which showcases the latest cutting-edge free/open-source software.<br/>"
		"<b>Install Notes:</b> The Live version allows for booting in Live mode, from which the installer can optionally be launched. The NetInstall version allows for both installation over the internet (FTP), or offline installation using pre-downloaded installation ISO files.") << 
        "7_NetInstall" << "7_NetInstall_x64" << "8_NetInstall" << "8_NetInstall_x64" << "8_Live" << "8_Live_x64" << "9_NetInstall" << "9_NetInstall_x64" << "9_Live" << "9_Live_x64" << "10_NetInstall" << "10_NetInstall_x64" << "10_Live" << "10_Live_x64" << "Rawhide_NetInstall" << "Rawhide_NetInstall_x64"));
	distroselect->addItem("FreeBSD", (QStringList() << "7.0" << 
	tr("<b>Homepage:</b> <a href=\"http://www.freebsd.org/\">http://www.freebsd.org</a><br/>"
		"<b>Description:</b> FreeBSD is a general-purpose Unix-like operating system designed for scalability and performance.<br/>"
		"<b>Install Notes:</b> The default version allows for both installation over the internet (FTP), or offline installation using pre-downloaded installation ISO files.") << 
	"6.3" << "6.3_x64" << "7.0" << "7.0_x64"));
	distroselect->addItem("FreeDOS", (QStringList() << "1.0" << 
	tr("<b>Homepage:</b> <a href=\"http://www.freedos.org/\">http://www.freedos.org</a><br/>"
		"<b>Description:</b> FreeDOS is a free MS-DOS compatible operating system.<br/>"
		"<b>Install Notes:</b> See the <a href=\"http://fd-doc.sourceforge.net/wiki/index.php?n=FdDocEn.FdInstall\">manual</a> for installation details.") << 
	"1.0"));
	distroselect->addItem("Frugalware", (QStringList() << "Stable" << 
	tr("<b>Homepage:</b> <a href=\"http://frugalware.org/\">http://frugalware.org</a><br/>"
		"<b>Description:</b> Frugalware is a general-purpose Slackware-based distro for advanced users.<br/>"
		"<b>Install Notes:</b> The default option allows for both installation over the internet (FTP), or offline installation using pre-downloaded installation ISO files.") << 
	"Stable" << "Stable_x64" << "Testing" << "Testing_x64" << "Current" << "Current_x64"));
	distroselect->addItem("F-Secure Rescue CD", (QStringList() << "Latest_Live" << 
	tr("<b>Homepage:</b> <a href=\"http://www.f-secure.com/linux-weblog/\">http://www.f-secure.com/linux-weblog/</a><br/>"
		"<b>Description:</b> F-Secure Rescue CD detects and removes malware from your Windows installation.<br/>"
		"<b>Install Notes:</b> The Live version allows for booting in Live mode, from which malware scans can be launched.") << 
	"Latest_Live"));
	distroselect->addItem("Gentoo", (QStringList() << "2008.0_Live" << 
	tr("<b>Homepage:</b> <a href=\"http://www.gentoo.org/\">http://www.gentoo.org</a><br/>"
		"<b>Description:</b> Gentoo is a flexible source-based distribution designed for advanced users.<br/>"
		"<b>Install Notes:</b> The Live version allows for booting in Live mode, from which the installer can optionally be launched.") << 
	"2007.0_Live" << "2007.0_Live_x64" << "2008.0_Live" << "2008.0_x64_Live"));
//	distroselect->addItem("GAG", (QStringList() << "4.9" << 
//	tr("<b>Homepage:</b> <a href=\"http://gag.sourceforge.net/\">http://gag.sourceforge.net</a><br/>"
//		"<b>Description:</b> GAG is a user-friendly graphical boot manager.<br/>"
//		"<b>Install Notes:</b> GAG simply boots and runs; no installation is required to use it.") << 
//	"4.9"));
	distroselect->addItem("gNewSense", (QStringList() << "Latest_Live" << 
	tr("<b>Homepage:</b> <a href=\"http://www.gnewsense.org/\">http://www.gnewsense.org</a><br/>"
		"<b>Description:</b> gNewSense is an FSF-endorsed distribution based on Ubuntu with all non-free components removed.<br/>"
		"<b>Install Notes:</b> The Live version allows for booting in Live mode, from which the installer can optionally be launched.") << 
	"Latest_Live"));
	distroselect->addItem("Gujin", (QStringList() << "2.4" << 
	tr("<b>Homepage:</b> <a href=\"http://gujin.sourceforge.net/\">http://gujin.sourceforge.net</a><br/>"
		"<b>Description:</b> Gujin is a graphical boot manager which can bootstrap various volumes and files.<br/>"
		"<b>Install Notes:</b> Gujin simply boots and runs; no installation is required to use it.") << 
	"2.4"));
	distroselect->addItem("Kaspersky Rescue Disk", (QStringList() << "Latest_Live" << 
	tr("<b>Homepage:</b> <a href=\"http://ftp.kaspersky.com/devbuilds/RescueDisk/\">http://ftp.kaspersky.com/devbuilds/RescueDisk/</a><br/>"
		"<b>Description:</b> Kaspersky Rescue Disk detects and removes malware from your Windows installation.<br/>"
		"<b>Install Notes:</b> The Live version allows for booting in Live mode, from which malware scans can be launched.") << 
	"Latest_Live"));
        distroselect->addItem("Kubuntu", (QStringList() << "9.04_Live" <<
	tr("<b>Homepage:</b> <a href=\"http://www.kubuntu.org/\">http://www.kubuntu.org</a><br/>"
		"<b>Description:</b> Kubuntu is an official Ubuntu derivative featuring the KDE desktop.<br/>"
		"<b>Install Notes:</b> The Live version allows for booting in Live mode, from which the installer can optionally be launched. The NetInstall version allows for installation over FTP, and can install Xubuntu and other official Ubuntu derivatives.") << 
	"6.06_NetInstall" << "6.06_NetInstall_x64" << "6.06_Live" << "6.06_Live_x64" << "6.10_NetInstall" << "6.10_NetInstall_x64" << "6.10_Live" << "6.10_Live_x64" << "7.04_NetInstall" << "7.04_NetInstall_x64" << "7.04_Live" << "7.04_Live_x64" << "7.10_NetInstall" << "7.10_NetInstall_x64" << "7.10_Live" << "7.10_Live_x64" << "8.04_NetInstall" << "8.04_NetInstall_x64" << "8.04_Live" << "8.04_Live_x64" << "8.10_NetInstall" << "8.10_NetInstall_x64" << "8.10_Live" << "8.10_Live_x64" << "9.04_NetInstall" << "9.04_NetInstall_x64" << "9.04_Live" << "9.04_Live_x64" << "Daily_Live" << "Daily_Live_x64"));
        distroselect->addItem("Linux Mint", (QStringList() << "6_Live" <<
	tr("<b>Homepage:</b> <a href=\"http://linuxmint.com/\">http://linuxmint.com</a><br/>"
		"<b>Description:</b> Linux Mint is a user-friendly Ubuntu-based distribution which includes additional proprietary codecs and other software by default.<br/>"
		"<b>Install Notes:</b> The Live version allows for booting in Live mode, from which the installer can optionally be launched.") << 
        "3.1_Live" << "4.0_Live" << "5-r1_Live" << "6_Live"));
	distroselect->addItem("Mandriva", (QStringList() << "2008.1_NetInstall" << 
	tr("<b>Homepage:</b> <a href=\"http://www.mandriva.com/\">http://www.mandriva.com/</a><br/>"
		"<b>Description:</b> Mandriva is a user-friendly distro formerly known as Mandrake Linux.<br/>"
		"<b>Install Notes:</b> The Live version allows for booting in Live mode, from which the installer can optionally be launched. The NetInstall version allows for installation over the internet (FTP) or via pre-downloaded <a href=\"http://www.mandriva.com/en/download\">\"Free\" iso image files</a>.") << 
	"2007.1_NetInstall" << "2007.1_NetInstall_x64" << "2008.0_NetInstall" << "2008.0_NetInstall_x64" << "2008.1_NetInstall" << "2008.1_NetInstall_x64"));
//	"2007.1_NetInstall" << "2007.1_NetInstall_x64" << "2008.0_NetInstall" << "2008.0_NetInstall_x64" << "2008.0_Live" << "2008.1_NetInstall" << "2008.1_NetInstall_x64" << "2008.1_Live"));
	distroselect->addItem("MEPIS", (QStringList() << "SimplyMEPIS" <<
	tr("<b>Homepage:</b> <a href=\"http://www.mepis.org/\">http://www.mepis.org</a><br/>"
		"<b>Description:</b> MEPIS is a Debian-based distribution. SimplyMEPIS is a user-friendly version based on KDE, while AntiX is a lightweight version for older computers.<br/>"
		"<b>Install Notes:</b> MEPIS supports booting in Live mode, from which the installer can optionally be launched.") << 
        "SimplyMEPIS" << "SimplyMEPIS_x64" << "AntiX"));
	distroselect->addItem("NetBSD", (QStringList() << "4.0" << 
	tr("<b>Homepage:</b> <a href=\"http://www.netbsd.org/\">http://www.netbsd.org</a><br/>"
		"<b>Description:</b> NetBSD is a Unix-like operating system which focuses on portability.<br/>"
		"<b>Install Notes:</b>The default version allows for both installation over the internet (FTP), or using pre-downloaded installation ISO files.") << 
	"4.0"));
	distroselect->addItem("NTPasswd", (QStringList() << "2008.05.26" << 
	tr("<b>Homepage:</b> <a href=\"http://home.eunet.no/pnordahl/ntpasswd/bootdisk.html\">http://home.eunet.no/pnordahl/ntpasswd/bootdisk.html</a><br/>"
		"<b>Description:</b> The Offline NT Password and Registry Editor can reset Windows passwords and edit the registry on Windows 2000-Vista.<br/>"
		"<b>Install Notes:</b> NTPasswd is booted and run in live mode; no installation is required to use it.") << 
	"2008.05.26"));
        distroselect->addItem("openSUSE", (QStringList() << "11.1" <<
	tr("<b>Homepage:</b> <a href=\"http://www.opensuse.org/\">http://www.opensuse.org</a><br/>"
		"<b>Description:</b> openSUSE is a user-friendly Novell sponsored distribution.<br/>"
		"<b>Install Notes:</b> The default version allows for both installation over the internet (FTP), or offline installation using pre-downloaded installation ISO files.") << 
        "10.2" << "10.2_x64" << "10.3" << "10.3_x64" << "11.0" << "11.0_x64" << "11.1" << "11.1_x64" << "Factory" << "Factory_x64"));
	distroselect->addItem("Ophcrack", (QStringList() << "XP-LiveCD-2.0" << 
	tr("<b>Homepage:</b> <a href=\"http://ophcrack.sourceforge.net/\">http://ophcrack.sourceforge.net</a><br/>"
		"<b>Description:</b> Ophcrack can crack Windows passwords.<br/>"
		"<b>Install Notes:</b> Ophcrack is booted and run in live mode; no installation is required to use it.") << 
	"XP-LiveCD-2.0" << "Vista-LiveCD-2.0"));
	distroselect->addItem("Parted Magic", (QStringList() << "Latest_Live" << 
	tr("<b>Homepage:</b> <a href=\"http://partedmagic.com/\">http://partedmagic.com</a><br/>"
		"<b>Description:</b> Parted Magic includes the GParted partition manager and other system utilities which can resize, copy, backup, and manipulate disk partitions.<br/>"
		"<b>Install Notes:</b> Parted Magic is booted and run in live mode; no installation is required to use it.") << 
	"2.1_Live" << "Latest_Live"));
	distroselect->addItem("PCLinuxOS", (QStringList() << "2007_Live" << 
	tr("<b>Homepage:</b> <a href=\"http://www.pclinuxos.com/\">http://www.pclinuxos.com</a><br/>"
		"<b>Description:</b> PCLinuxOS is a user-friendly Mandriva-based distribution.<br/>"
		"<b>Install Notes:</b> The Live version allows for booting in Live mode, from which the installer can optionally be launched.") << 
	"2007_Live" << "2008 Gnome_Live" << "2008 Minime_Live"));
	distroselect->addItem("Puppy Linux", (QStringList() << "Latest_Live" << 
	tr("<b>Homepage:</b> <a href=\"http://www.puppylinux.com/\">http://www.puppylinux.com</a><br/>"
		"<b>Description:</b> Puppy Linux is a lightweight distribution designed for older computers.<br/>"
		"<b>Install Notes:</b> The Live version loads the entire system into RAM and boots from memory, so installation is not required but optional.") << 
        "Latest_Live"));
	distroselect->addItem("Sabayon Linux", (QStringList() << "4-LiteMCE" <<
	tr("<b>Homepage:</b> <a href=\"http://www.sabayonlinux.org/\">http://www.sabayonlinux.org</a><br/>"
		"<b>Description:</b> Sabayon Linux is a Gentoo-based Live DVD distribution which features the Entropy binary package manager in addition to the source-based Portage.<br/>"
		"<b>Install Notes:</b> The Live version allows for booting in Live mode, from which the installer can optionally be launched. The LiteMCE edition is 2 GB, while the full edition will need an 8 GB USB drive") <<
		"4-LiteMCE" << "4-LiteMCE_x64" << "4" << "4_x64"));
	distroselect->addItem("Slax", (QStringList() << "Latest_Live" << 
	tr("<b>Homepage:</b> <a href=\"http://www.slax.org/\">http://www.slax.org</a><br/>"
		"<b>Description:</b> Slax is a Slackware-based distribution featuring the KDE desktop.<br/>"
		"<b>Install Notes:</b> The Live version allows for booting in Live mode, from which the installer can optionally be launched.") << 
	"Latest_Live"));
	distroselect->addItem("SliTaz", (QStringList() << "Stable_Live" << 
	tr("<b>Homepage:</b> <a href=\"http://www.slitaz.org/en/\">http://www.slitaz.org/en</a><br/>"
		"<b>Description:</b> SliTaz is a lightweight, desktop-oriented micro distribution.<br/>"
		"<b>Install Notes:</b> The Live version loads the entire system into RAM and boots from memory, so installation is not required but optional.") << 
	"Stable_Live" << "Cooking_Live"));
	distroselect->addItem("Smart Boot Manager", (QStringList() << "3.7" << 
	tr("<b>Homepage:</b> <a href=\"http://btmgr.sourceforge.net/about.html\">http://btmgr.sourceforge.net/about.html</a><br/>"
		"<b>Description:</b> Smart Boot Manager is a bootloader which can overcome some boot-related BIOS limitations and bugs.<br/>"
		"<b>Install Notes:</b> SBM simply boots and runs; no installation is required to use it.") << 
	"3.7"));
	distroselect->addItem("Super Grub Disk", (QStringList() << "Latest" << 
	tr("<b>Homepage:</b> <a href=\"http://www.supergrubdisk.org\">http://www.supergrubdisk.org</a><br/>"
		"<b>Description:</b> Super Grub Disk is a bootloader which can perform a variety of MBR and bootloader recovery tasks.<br/>"
		"<b>Install Notes:</b> SGD simply boots and runs; no installation is required to use it.") << 
	"Latest"));
	distroselect->addItem("Super Ubuntu", (QStringList() << "Latest_Live" << 
	tr("<b>Homepage:</b> <a href=\"http://hacktolive.org/wiki/Super_Ubuntu\">http://hacktolive.org/wiki/Super_Ubuntu</a><br/>"
		"<b>Description:</b> Super Ubuntu is an unofficial derivative of Ubuntu which includes additional software by default. Requires a 2GB USB drive to install.<br/>"
		"<b>Install Notes:</b> The Live version allows for booting in Live mode, from which the installer can optionally be launched.") << 
	"Latest_Live"));
	distroselect->addItem("SystemRescueCD", (QStringList() << "Latest_Live" << 
	tr("<b>Homepage:</b> <a href=\"http://www.sysresccd.org\">http://www.sysresccd.org</a><br/>"
		"<b>Description:</b> SystemRescueCD includes various partition management and data recovery and backup tools.<br/>"
		"<b>Install Notes:</b> SystemRescueCD is booted and run in live mode; no installation is required to use it.") << 
	"Latest_Live"));
	distroselect->addItem("Ubuntu", (QStringList() << "9.04_Live" <<
	tr("<b>Homepage:</b> <a href=\"http://www.ubuntu.com/\">http://www.ubuntu.com</a><br/>"
		"<b>Description:</b> Ubuntu is a user-friendly Debian-based distribution. It is currently the most popular Linux desktop distribution.<br/>"
		"<b>Install Notes:</b> The Live version allows for booting in Live mode, from which the installer can optionally be launched. The NetInstall version allows for installation over FTP, and can install Kubuntu and other official Ubuntu derivatives.") << 
	"6.06_NetInstall" << "6.06_NetInstall_x64" << "6.06_Live" << "6.06_Live_x64" << "6.10_NetInstall" << "6.10_NetInstall_x64" << "6.10_Live" << "6.10_Live_x64" << "7.04_NetInstall" << "7.04_NetInstall_x64" << "7.04_Live" << "7.04_Live_x64" << "7.10_NetInstall" << "7.10_NetInstall_x64" << "7.10_Live" << "7.10_Live_x64" << "8.04_NetInstall" << "8.04_NetInstall_x64" << "8.04_Live" << "8.04_Live_x64" << "8.10_NetInstall" << "8.10_NetInstall_x64" << "8.10_Live" << "8.10_Live_x64" << "9.04_NetInstall" << "9.04_NetInstall_x64" << "9.04_Live" << "9.04_Live_x64" << "Daily_Live" << "Daily_Live_x64"));
	distroselect->addItem("xPUD", (QStringList() << "Latest_Live" <<
	tr("<b>Homepage:</b> <a href=\"http://www.xpud.org/\">http://www.xpud.org</a><br/>"
		"<b>Description:</b> xPUD is a lightweight distribution featuring a simple kiosk-like interface with a web browser and media player.<br/>"
		"<b>Install Notes:</b> The Live version loads the entire system into RAM and boots from memory.") << 
	"Latest_Live"));
	distroselect->addItem("Xubuntu", (QStringList() << "9.04_Live" <<
	tr("<b>Homepage:</b> <a href=\"http://www.xubuntu.org/\">http://www.xubuntu.org</a><br/>"
		"<b>Description:</b> Xubuntu is an official Ubuntu derivative featuring the XFCE desktop.<br/>"
		"<b>Install Notes:</b> The Live version allows for booting in Live mode, from which the installer can optionally be launched. The NetInstall version allows for installation over FTP, and can install Kubuntu and other official Ubuntu derivatives.") << 
	"6.06_NetInstall" << "6.06_NetInstall_x64" << "6.06_Live" << "6.06_Live_x64" << "6.10_NetInstall" << "6.10_NetInstall_x64" << "6.10_Live" << "6.10_Live_x64" << "7.04_NetInstall" << "7.04_NetInstall_x64" << "7.04_Live" << "7.04_Live_x64" << "7.10_NetInstall" << "7.10_NetInstall_x64" << "7.10_Live" << "7.10_Live_x64" << "8.04_NetInstall" << "8.04_NetInstall_x64" << "8.04_Live" << "8.04_Live_x64" << "8.10_NetInstall" << "8.10_NetInstall_x64" << "8.10_Live" << "8.10_Live_x64" << "9.04_NetInstall" << "9.04_NetInstall_x64" << "9.04_Live" << "9.04_Live_x64" << "Daily_Live" << "Daily_Live_x64"));
	distroselect->addItem("Zenwalk", (QStringList() << "5.2" << 
	tr("<b>Homepage:</b> <a href=\"http://www.zenwalk.org/\">http://www.zenwalk.org</a><br/>"
		"<b>Description:</b> Zenwalk is a Slackware-based distribution featuring the XFCE desktop.<br/>"
		"<b>Install Notes:</b> The Live version allows for booting in Live mode, from which the installer can optionally be launched.") << 
	"5.2"));
	#endif
	#include "customdistselect.cpp"
	#ifdef Q_OS_UNIX
	if (QFile::exists("/sbin/fdisk"))
		fdiskcommand = "/sbin/fdisk";
	else
                fdiskcommand = locatecommand("fdisk", tr("either"), "util-linux");
        if (QFile::exists("/bin/df"))
                dfcommand = "/bin/df";
        else
                dfcommand = locatecommand("df", tr("either"), "util-linux");
	if (QFile::exists("/sbin/sfdisk"))
		sfdiskcommand = "/sbin/sfdisk";
	else
		sfdiskcommand = locatecommand("sfdisk", tr("either"), "util-linux");
	if (QFile::exists("/lib/udev/vol_id"))
		volidcommand = "/lib/udev/vol_id";
	else
		volidcommand = locatecommand("vol_id", tr("either"), "udev");
	locatecommand("mtools", tr("USB Drive"), "mtools");
        syslinuxcommand = "/usr/bin/ubnsylnx";
        extlinuxcommand = "/usr/bin/ubnexlnx";
	#ifdef NOSTATIC
        syslinuxcommand = "/usr/bin/syslinux";
        extlinuxcommand = "/usr/bin/extlinux";
	#endif
//	syslinuxcommand = locatecommand("syslinux", tr("USB Drive"), "syslinux");
	sevzcommand = locatecommand("7z", tr("either"), "p7zip-full");
	ubntmpf = "/tmp/";
	#endif
	#ifdef Q_OS_WIN32
	ubntmpf = QDir::toNativeSeparators(QString("%1/").arg(QDir::tempPath()));
	#endif
	if (typeselect->findText(tr("USB Drive")) != -1)
		typeselect->setCurrentIndex(typeselect->findText(tr("USB Drive")));
	#ifdef HDDINSTALL
	if (typeselect->findText(tr("Hard Disk")) != -1)
		typeselect->setCurrentIndex(typeselect->findText(tr("Hard Disk")));
	#endif
}

void unetbootin::on_distroselect_currentIndexChanged(int distroselectIndex)
{
	dverselect->clear();
	if (distroselectIndex == -1)
		return;
	QStringList dverL = distroselect->itemData(distroselectIndex).value<QStringList>();
	for (int i = 2; i < dverL.size(); ++i)
	{
		if (!dverL.at(i).contains("someotherversion") && !dverL.at(i).isEmpty())
			dverselect->addItem(dverL.at(i));
	}
	if (dverselect->findText(dverL.at(0)) != -1)
		dverselect->setCurrentIndex(dverselect->findText(dverL.at(0)));
	intromessage->setText(dverL.at(1));
	radioDistro->setChecked(true);
}

void unetbootin::on_showalldrivescheckbox_clicked()
{
	refreshdriveslist();
}

void unetbootin::refreshdriveslist()
{
	driveselect->clear();
	QStringList driveslist = listcurdrives();
	for (int i = 0; i < driveslist.size(); ++i)
	{
		driveselect->addItem(driveslist.at(i));
	}
}

QStringList unetbootin::listcurdrives()
{
	if (showalldrivescheckbox->isChecked())
	{
		return listalldrives();
	}
	else
	{
		return listsanedrives();
	}
}

QStringList unetbootin::listsanedrives()
{
	QStringList fulldrivelist;
	if (typeselect->currentText() == tr("Hard Disk"))
	{
		fulldrivelist.append(QDir::toNativeSeparators(QDir::rootPath()).toUpper());
	}
	else if (typeselect->currentText() == tr("USB Drive"))
	{
		#ifdef Q_OS_WIN32
		QFileInfoList extdrivesList = QDir::drives();
		for (int i = 0; i < extdrivesList.size(); ++i)
		{
			if (QDir::toNativeSeparators(extdrivesList.at(i).path().toUpper()) != QDir::toNativeSeparators(QDir::rootPath().toUpper()) && !QDir::toNativeSeparators(extdrivesList.at(i).path().toUpper()).contains("A:") && !QDir::toNativeSeparators(extdrivesList.at(i).path().toUpper()).contains("B:"))
			{
				if (GetDriveType(LPWSTR(extdrivesList.at(i).path().toUpper().utf16())) == 2)
				{
					fulldrivelist.append(QDir::toNativeSeparators(extdrivesList.at(i).path().toUpper()));
				}
			}
		}
		#endif
                #ifdef Q_OS_UNIX
                QDir devlstdir("/dev/disk/by-id/");
                QFileInfoList usbfileinfoL = devlstdir.entryInfoList(QDir::NoDotAndDotDot|QDir::Files);
                for (int i = 0; i < usbfileinfoL.size(); ++i)
                {
//                    if (usbfileinfoL.at(i).contains(QRegExp("^usb-\\S{1,}-part\\d{1,}$")))
//                    {
//                        fulldrivelist.append(usbfileinfoL.at(i).canonicalFilePath());
//                    }
                    if (usbfileinfoL.at(i).fileName().contains(QRegExp("^usb-\\S{1,}$")))
                    {
                        if (QString(callexternapp(volidcommand, QString("-t %2").arg(usbfileinfoL.at(i).canonicalFilePath()))).contains(QRegExp("(vfat|ext2|ext3)")))
                            fulldrivelist.append(usbfileinfoL.at(i).canonicalFilePath());
                    }
                }
                /*
                QString fdisklusbdevsS = callexternapp(fdiskcommand, "-l");
                QStringList usbdevsL = QString(fdisklusbdevsS).split("\n").filter(QRegExp("\\.{0,}FAT|Disk\\.{0,}")).join("\n").split(" ").join("\n").split("\t").join("\n").split("\n").filter("/dev/");
		for (int i = 0; i < usbdevsL.size(); ++i)
		{
			if (usbdevsL.at(i).contains(":"))
			{
				if (!QString(callexternapp(volidcommand, QString("-t %2").arg(QString(usbdevsL.at(i)).remove(":")))).contains("vfat"))
					continue;
			}
			fulldrivelist.append(QString(usbdevsL.at(i)).remove(":"));
                }
                */
		#endif
	}
	return fulldrivelist;
}


QStringList unetbootin::listalldrives()
{
	QStringList fulldrivelist;
	#ifdef Q_OS_WIN32
	QFileInfoList extdrivesList = QDir::drives();
	for (int i = 0; i < extdrivesList.size(); ++i)
	{
		fulldrivelist.append(QDir::toNativeSeparators(extdrivesList.at(i).path().toUpper()));
	}
	#endif
	#ifdef Q_OS_UNIX
        QString fdisklusbdevsS = callexternapp(fdiskcommand, "-l");
        QString dflusbdevsS = callexternapp(dfcommand, "");
        fulldrivelist = QString(dflusbdevsS).split(" ").join("\n").split("\t").join("\n").split("\n").filter("/dev/");
        QStringList fulldrivelist2 = QString(fdisklusbdevsS).split(" ").join("\n").split("\t").join("\n").split("\n").filter("/dev/").replaceInStrings(":", "");
        for (int i = 0; i < fulldrivelist2.size(); ++i)
        {
            if (!fulldrivelist.contains(fulldrivelist2.at(i)))
                fulldrivelist.append(fulldrivelist2.at(i));
        }
	#endif
	return fulldrivelist;
}

void unetbootin::on_typeselect_currentIndexChanged(int typeselectIndex)
{
	showalldrivescheckbox->setChecked(false);
	formatdrivecheckbox->setChecked(false);
	if (typeselectIndex == typeselect->findText(tr("Hard Disk")))
	{
		showalldrivescheckbox->setEnabled(false);
		formatdrivecheckbox->setEnabled(false);
	}
	if (typeselectIndex == typeselect->findText(tr("USB Drive")))
	{
		showalldrivescheckbox->setEnabled(true);
//		formatdrivecheckbox->setEnabled(true);
	}
	refreshdriveslist();
}

void unetbootin::on_dverselect_currentIndexChanged()
{
	radioDistro->setChecked(true);
}

void unetbootin::on_diskimagetypeselect_currentIndexChanged()
{
	radioFloppy->setChecked(true);
}

void unetbootin::on_FloppyFileSelector_clicked()
{
	QString nameFloppy = QDir::toNativeSeparators(QFileDialog::getOpenFileName(this, tr("Open Disk Image File"), QDir::homePath()));
	if (QFileInfo(nameFloppy).completeSuffix().contains("iso", Qt::CaseInsensitive))
	{
		if (diskimagetypeselect->findText(tr("ISO")) != -1)
			diskimagetypeselect->setCurrentIndex(diskimagetypeselect->findText(tr("ISO")));
	}
	if (QFileInfo(nameFloppy).completeSuffix().contains("img", Qt::CaseInsensitive) || QFileInfo(nameFloppy).completeSuffix().contains("flp", Qt::CaseInsensitive))
	{
		if (diskimagetypeselect->findText(tr("Floppy")) != -1)
			diskimagetypeselect->setCurrentIndex(diskimagetypeselect->findText(tr("Floppy")));
	}
	FloppyPath->clear();
	FloppyPath->insert(nameFloppy);
	radioFloppy->setChecked(true);
}

void unetbootin::on_KernelFileSelector_clicked()
{
	QString nameKernel = QDir::toNativeSeparators(QFileDialog::getOpenFileName(this, tr("Open Kernel File"), QDir::homePath()));
	KernelPath->clear();
	KernelPath->insert(nameKernel);
	radioManual->setChecked(true);
}

void unetbootin::on_InitrdFileSelector_clicked()
{
	QString nameInitrd = QDir::toNativeSeparators(QFileDialog::getOpenFileName(this, tr("Open Initrd File"), QDir::homePath()));
	InitrdPath->clear();
	InitrdPath->insert(nameInitrd);
	radioManual->setChecked(true);
}

void unetbootin::on_CfgFileSelector_clicked()
{
	QString nameCfg = QFileDialog::getOpenFileName(this, tr("Open Bootloader Config File"), QDir::homePath());
	OptionEnter->clear();
	QString cfgoptstxt = getcfgkernargs(nameCfg, "", QStringList());
	if (cfgoptstxt.isEmpty())
	{
		cfgoptstxt = getgrubcfgargs(nameCfg);
	}
	OptionEnter->insert(cfgoptstxt);
	radioManual->setChecked(true);
}

void unetbootin::on_cancelbutton_clicked()
{
	close();
}

void unetbootin::on_okbutton_clicked()
{
	if (typeselect->currentIndex() == typeselect->findText(tr("USB Drive")) && driveselect->currentText().isEmpty())
	{
		QMessageBox unotenoughinputmsgb;
		unotenoughinputmsgb.setIcon(QMessageBox::Information);
		unotenoughinputmsgb.setWindowTitle(tr("Insert a USB flash drive"));
		unotenoughinputmsgb.setText(tr("No USB flash drives were found. If you have already inserted a USB drive, try reformatting it as FAT32."));
 		unotenoughinputmsgb.setStandardButtons(QMessageBox::Ok);
 		switch (unotenoughinputmsgb.exec())
 		{
 			case QMessageBox::Ok:
				break;
	 		default:
				break;
 		}
	}
	#ifdef Q_OS_UNIX
	else if (typeselect->currentIndex() == typeselect->findText(tr("USB Drive")) && locatemountpoint(driveselect->currentText()) == "NOT MOUNTED")
	{
		QMessageBox merrordevnotmountedmsgbx;
		merrordevnotmountedmsgbx.setIcon(QMessageBox::Warning);
		merrordevnotmountedmsgbx.setWindowTitle(QString(tr("%1 not mounted")).arg(driveselect->currentText()));
		merrordevnotmountedmsgbx.setText(QString(tr("You must first mount the USB drive %1 to a mountpoint. Most distributions will do this automatically after you remove and reinsert the USB drive.")).arg(driveselect->currentText()));
		merrordevnotmountedmsgbx.setStandardButtons(QMessageBox::Ok);
		switch (merrordevnotmountedmsgbx.exec())
		{
 			case QMessageBox::Ok:
				break;
	 		default:
				break;
		}
	}
	#endif
	else if (radioDistro->isChecked() && distroselect->currentIndex() == distroselect->findText("== Select Distribution =="))
	{
		QMessageBox dnotenoughinputmsgb;
		dnotenoughinputmsgb.setIcon(QMessageBox::Information);
		dnotenoughinputmsgb.setWindowTitle(tr("Select a distro"));
		dnotenoughinputmsgb.setText(tr("You must select a distribution to load."));
 		dnotenoughinputmsgb.setStandardButtons(QMessageBox::Ok);
 		switch (dnotenoughinputmsgb.exec())
 		{
 			case QMessageBox::Ok:
				break;
	 		default:
				break;
 		}
	}
	else if (radioFloppy->isChecked() && FloppyPath->text().isEmpty())
	{
		QMessageBox fnotenoughinputmsgb;
		fnotenoughinputmsgb.setIcon(QMessageBox::Information);
		fnotenoughinputmsgb.setWindowTitle(tr("Select a disk image file"));
		fnotenoughinputmsgb.setText(tr("You must select a disk image file to load."));
 		fnotenoughinputmsgb.setStandardButtons(QMessageBox::Ok);
 		switch (fnotenoughinputmsgb.exec())
 		{
 			case QMessageBox::Ok:
				break;
	 		default:
				break;
 		}
	}
	else if (radioManual->isChecked() && KernelPath->text().isEmpty())
	{
		QMessageBox knotenoughinputmsgb;
		knotenoughinputmsgb.setIcon(QMessageBox::Information);
		knotenoughinputmsgb.setWindowTitle(tr("Select a kernel and/or initrd file"));
		knotenoughinputmsgb.setText(tr("You must select a kernel and/or initrd file to load."));
 		knotenoughinputmsgb.setStandardButtons(QMessageBox::Ok);
 		switch (knotenoughinputmsgb.exec())
 		{
 			case QMessageBox::Ok:
				break;
	 		default:
				break;
 		}
	}
	else if (radioFloppy->isChecked() && !QFile::exists(FloppyPath->text()))
	{
		QMessageBox ffnotexistsmsgb;
		ffnotexistsmsgb.setIcon(QMessageBox::Information);
		ffnotexistsmsgb.setWindowTitle(tr("Diskimage file not found"));
		ffnotexistsmsgb.setText(tr("The specified diskimage file %1 does not exist.").arg(FloppyPath->text()));
 		ffnotexistsmsgb.setStandardButtons(QMessageBox::Ok);
 		switch (ffnotexistsmsgb.exec())
 		{
 			case QMessageBox::Ok:
				break;
	 		default:
				break;
 		}
	}
	else if (radioManual->isChecked() && !QFile::exists(KernelPath->text()))
	{
		QMessageBox kfnotexistsmsgb;
		kfnotexistsmsgb.setIcon(QMessageBox::Information);
		kfnotexistsmsgb.setWindowTitle(tr("Kernel file not found"));
		kfnotexistsmsgb.setText(tr("The specified kernel file %1 does not exist.").arg(KernelPath->text()));
 		kfnotexistsmsgb.setStandardButtons(QMessageBox::Ok);
 		switch (kfnotexistsmsgb.exec())
 		{
 			case QMessageBox::Ok:
				break;
	 		default:
				break;
 		}
	}
	else if (radioManual->isChecked() && InitrdPath->text().trimmed() != "" && !QFile::exists(InitrdPath->text()))
	{
		QMessageBox ifnotexistsmsgb;
		ifnotexistsmsgb.setIcon(QMessageBox::Information);
		ifnotexistsmsgb.setWindowTitle(tr("Initrd file not found"));
		ifnotexistsmsgb.setText(tr("The specified initrd file %1 does not exist.").arg(InitrdPath->text()));
 		ifnotexistsmsgb.setStandardButtons(QMessageBox::Ok);
 		switch (ifnotexistsmsgb.exec())
 		{
 			case QMessageBox::Ok:
				break;
	 		default:
				break;
 		}
	}
	else
	{
		runinst();
	}
}

void unetbootin::on_fexitbutton_clicked()
{
	close();
}

void unetbootin::on_frebootbutton_clicked()
{
	sysreboot();
}

QString unetbootin::displayfisize(quint64 fisize)
{
	if (fisize < 10000)
	{
		return QString("%1 B").arg(fisize);
	}
	else if (fisize < 10240000)
	{
		return QString("%1 KB").arg(fisize / 1024);
	}
	else
	{
		return QString("%1 MB").arg(fisize / 1048576);
	}
}

QPair<QPair<QStringList, QList<quint64> >, QStringList> unetbootin::listarchiveconts(QString archivefile)
{
	#ifdef Q_OS_WIN32
	if (sevzcommand.isEmpty())
	{
		installsvzip();
	}
	callexternapp(getenv("COMSPEC"), QString("/c \"\"%1\" -bd -slt l \"%2\" > \"%3\"\"").arg(sevzcommand).arg(archivefile).arg(QString("%1ubntmpls.txt").arg(ubntmpf)));
	QFile tmplsF(QString("%1ubntmpls.txt").arg(ubntmpf));
	tmplsF.open(QIODevice::ReadOnly | QIODevice::Text);
	QTextStream tmplsS(&tmplsF);
	#endif
	#ifdef Q_OS_UNIX
	QString sevzlcommandout = callexternapp(sevzcommand, QString("-bd -slt l \"%2\"").arg(archivefile));
	QTextStream tmplsS(&sevzlcommandout);
	#endif
	QString tmplsL;
	QStringList tmplsSLF;
	QStringList tmplsSLD;
	QList<quint64> tmplsSLFS;
	QString tmplsN;
	QString tmplsFS;
	while (!tmplsS.atEnd())
	{
		tmplsL = tmplsS.readLine();
		if (tmplsL.contains("Path = "))
		{
			if (tmplsL.contains("Path = [BOOT]"))
				continue;
			tmplsN = tmplsS.readLine();
			if (tmplsN.contains("Folder = 1") || tmplsN.contains("Folder = +"))
			{
				tmplsSLD.append(tmplsL.remove("Path = "));
			}
			else
			{
				tmplsSLF.append(tmplsL.remove("Path = "));
				tmplsFS = QString(tmplsS.readLine()).remove("Size = ").trimmed();
				tmplsSLFS.append(tmplsFS.toULongLong());				
			}
		}
	}
	#ifdef Q_OS_WIN32
	tmplsF.close();
	QFile::remove(QString("%1ubntmpls.txt").arg(ubntmpf));
	#endif
	return qMakePair(qMakePair(tmplsSLF, tmplsSLFS), tmplsSLD);
}

bool unetbootin::overwritefileprompt(QString ovwfileloc)
{
	if (overwriteall)
	{
		QFile::remove(ovwfileloc);
		return true;
	}
	QMessageBox overwritefilemsgbx;
	overwritefilemsgbx.setIcon(QMessageBox::Warning);
	overwritefilemsgbx.setWindowTitle(QString(tr("%1 exists, overwrite?")).arg(ovwfileloc));
	overwritefilemsgbx.setText(QString(tr("The file %1 already exists. Press 'Yes to All' to overwrite it and not be prompted again, 'Yes' to overwrite files on an individual basis, and 'No' to retain your existing version. If in doubt, press 'Yes to All'.")).arg(ovwfileloc));
	overwritefilemsgbx.setStandardButtons(QMessageBox::Yes | QMessageBox::YesToAll | QMessageBox::No);
	switch (overwritefilemsgbx.exec())
	{
		case QMessageBox::Yes:
		{
			QFile::remove(ovwfileloc);
			return true;
		}
		case QMessageBox::YesToAll:
		{
			QFile::remove(ovwfileloc);
			overwriteall = true;
			return true;
		}
		case QMessageBox::No:
			return false;
		default:
			return false;
	}
}

bool unetbootin::extractfile(QString filepath, QString destinfileL, QString archivefile)
{
	QString destindir = QFileInfo(destinfileL).dir().absolutePath();
	QString destinfilename = QString("%1/%2").arg(destindir).arg(QFileInfo(destinfileL).fileName());
	QString filepathfilename = QString("%1/%2").arg(destindir).arg(QFileInfo(filepath).fileName());
	if (QFile::exists(filepathfilename))
	{
		if (!overwritefileprompt(filepathfilename))
			return false;
	}
	if (QFile::exists(destinfilename))
	{
		if (!overwritefileprompt(destinfilename))
			return false;
	}
	#ifdef Q_OS_WIN32
	if (sevzcommand.isEmpty())
	{
		installsvzip();
	}
	#endif
	callexternapp(sevzcommand, QString("-bd  -aos -o\"%1\" e \"%2\" \"%3\"").arg(QDir::toNativeSeparators(destindir), QDir::toNativeSeparators(archivefile), QDir::toNativeSeparators(filepath)));
	if (QFileInfo(filepathfilename).absoluteFilePath() == QFileInfo(destinfilename).absoluteFilePath())
	{
		return true;
	}
	else
	{
		return QFile::rename(filepathfilename, destinfilename);
	}
}

bool unetbootin::extractkernel(QString archivefile, QString kernoutputfile, QPair<QStringList, QList<quint64> > archivefileconts)
{
	pdesc1->setText(QString("Locating kernel file in %1").arg(archivefile));
	QStringList kernelnames = QStringList() << "vmlinuz" << "vmlinux" << "bzImage" << "kernel" << "sabayon" << "gentoo" << "linux26" << "linux24" << "bsd" << "unix" << "linux" << "rescue" << "xpud";
	QStringList tnarchivefileconts;
	QStringList narchivefileconts;
	QString curarcitm;
	for (int i = 0; i < archivefileconts.second.size(); ++i)
	{
//		curarcitm = archivefileconts.first.at(i).right(archivefileconts.first.at(i).size() - archivefileconts.first.at(i).lastIndexOf(QDir::toNativeSeparators("/")) - 1);
//		if (curarcitm.contains("isolinux", Qt::CaseInsensitive) || curarcitm.contains("memtest", Qt::CaseInsensitive) || curarcitm.contains("system.map", Qt::CaseInsensitive) || curarcitm.contains(".efimg", Qt::CaseInsensitive) || curarcitm.contains(".jpg", Qt::CaseInsensitive) || curarcitm.contains(".png", Qt::CaseInsensitive) || curarcitm.contains(".pdf", Qt::CaseInsensitive) || curarcitm.contains(".txt", Qt::CaseInsensitive) || curarcitm.contains(".pcx", Qt::CaseInsensitive) || curarcitm.contains(".rle", Qt::CaseInsensitive) || curarcitm.contains(".fnt", Qt::CaseInsensitive) || curarcitm.contains(".msg", Qt::CaseInsensitive) || curarcitm.contains(".cat", Qt::CaseInsensitive) || curarcitm.contains(".tar", Qt::CaseInsensitive) || curarcitm.contains(".psd", Qt::CaseInsensitive) || curarcitm.contains(".xcf", Qt::CaseInsensitive) || curarcitm.contains(".bmp", Qt::CaseInsensitive) || curarcitm.contains(".svg", Qt::CaseInsensitive))
//		{
//			continue;
//		}
//		if (filteroutlist(curarcitm, ignoredtypesbothRL+ignoredtypeskernelRL).isEmpty())
//		{
//			continue;
//		}
		if (archivefileconts.second.at(i) > 614400 && archivefileconts.second.at(i) < 20971520) // between 600 KB and 20 MB
		{
			tnarchivefileconts.append(archivefileconts.first.at(i));
		}
	}
	narchivefileconts = filteroutlistL(tnarchivefileconts, ignoredtypesbothRL+ignoredtypeskernelRL);
	for (int i = 0; i < kernelnames.size(); ++i)
	{
		for (int j = 0; j < narchivefileconts.size(); ++j)
		{
			if (narchivefileconts.at(j).right(narchivefileconts.at(j).size() - narchivefileconts.at(j).lastIndexOf(QDir::toNativeSeparators("/")) - 1).contains(kernelnames.at(i), Qt::CaseInsensitive))
			{
				pdesc1->setText(QString("Copying kernel file from %1").arg(narchivefileconts.at(j)));
				return extractfile(narchivefileconts.at(j), kernoutputfile, archivefile);
			}
		}
	}
	pdesc1->setText("");
	return false;
}

bool unetbootin::extractinitrd(QString archivefile, QString kernoutputfile, QPair<QStringList, QList<quint64> > archivefileconts)
{
	pdesc1->setText(QString("Locating initrd file in %1").arg(archivefile));
	QStringList kernelnames = QStringList() << "initrd.img.gz" << "initrd.igz" << "initrd.gz" << "initrd.img" << "initramfs.gz" << "initramfs.img" << "initrd" << "initramfs" << "minirt" << "miniroot" << "sabayon.igz" << "gentoo.igz" << "archlive.img" << "rootfs.gz" << ".igz" << ".cgz" << ".img" << "rootfs" << "fs.gz" << "root.gz" << ".gz" << "initram" << "initr" << "init" << "ram";
	QStringList tnarchivefileconts;
	QStringList narchivefileconts;
	QString curarcitm;
	for (int i = 0; i < archivefileconts.second.size(); ++i)
	{
		curarcitm = archivefileconts.first.at(i).right(archivefileconts.first.at(i).size() - archivefileconts.first.at(i).lastIndexOf(QDir::toNativeSeparators("/")) - 1);
//		if (curarcitm.contains("isolinux", Qt::CaseInsensitive) || curarcitm.contains("memtest", Qt::CaseInsensitive) || curarcitm.contains("system.map", Qt::CaseInsensitive) || curarcitm.contains(".efimg", Qt::CaseInsensitive) || curarcitm.contains(".jpg", Qt::CaseInsensitive) || curarcitm.contains(".png", Qt::CaseInsensitive) || curarcitm.contains(".pdf", Qt::CaseInsensitive) || curarcitm.contains(".txt", Qt::CaseInsensitive) || curarcitm.contains(".pcx", Qt::CaseInsensitive) || curarcitm.contains(".rle", Qt::CaseInsensitive) || curarcitm.contains(".fnt", Qt::CaseInsensitive) || curarcitm.contains(".msg", Qt::CaseInsensitive) || curarcitm.contains(".cat", Qt::CaseInsensitive) || curarcitm.contains(".tar", Qt::CaseInsensitive) || curarcitm.contains(".psd", Qt::CaseInsensitive) || curarcitm.contains(".xcf", Qt::CaseInsensitive) || curarcitm.contains(".bmp", Qt::CaseInsensitive) || curarcitm.contains(".svg", Qt::CaseInsensitive))
//		{
//			continue;
//		}
//		if (filteroutlist(curarcitm, ignoredtypesbothRL+ignoredtypesinitrdRL).isEmpty())
//		{
//			continue;
//		}
		if (archivefileconts.second.at(i) > 256000 && archivefileconts.second.at(i) < 209715200) // between 250 KB and 200 MB
		{
			tnarchivefileconts.append(archivefileconts.first.at(i));
		}
	}
	narchivefileconts = filteroutlistL(tnarchivefileconts, ignoredtypesbothRL+ignoredtypesinitrdRL);
	for (int i = 0; i < kernelnames.size(); ++i)
	{
		for (int j = 0; j < narchivefileconts.size(); ++j)
		{
			if (narchivefileconts.at(j).right(narchivefileconts.at(j).size() - narchivefileconts.at(j).lastIndexOf(QDir::toNativeSeparators("/")) - 1).contains(kernelnames.at(i), Qt::CaseInsensitive))
			{
				pdesc1->setText(QString("Copying initrd file from %1").arg(narchivefileconts.at(j)));
				return extractfile(narchivefileconts.at(j), kernoutputfile, archivefile);
			}
		}
	}
	pdesc1->setText("");
	return false;
}

QString unetbootin::extractcfg(QString archivefile, QStringList archivefileconts)
{
	QString grubpcfg;
	QString syslinuxpcfg;
	QStringList grubcfgtypes = QStringList() << "menu.lst" << "grub.conf";
	QStringList mlstfoundfiles;
	for (int i = 0; i < grubcfgtypes.size(); ++i)
	{
		mlstfoundfiles = archivefileconts.filter(grubcfgtypes.at(i), Qt::CaseInsensitive);
		if (!mlstfoundfiles.isEmpty())
		{
			for (int j = 0; j < mlstfoundfiles.size(); ++j)
			{
				randtmpfile mlstftf(ubntmpf, "lst");
				extractfile(archivefileconts.filter(grubcfgtypes.at(i), Qt::CaseInsensitive).at(j), mlstftf.fileName(), archivefile);
				grubpcfg = getgrubcfgargs(mlstftf.fileName()).trimmed();
				mlstftf.remove();
				if (!grubpcfg.isEmpty())
					break;
			}
		}
		if (!grubpcfg.isEmpty())
			break;
	}
	QStringList syslinuxcfgtypes = QStringList() << "syslinux.cfg" << "isolinux.cfg" << "extlinux.cfg" << "pxelinux.cfg" << "menu_en.cfg" << "en.cfg" << ".cfg";
	QStringList lcfgfoundfiles;
	for (int i = 0; i < syslinuxcfgtypes.size(); ++i)
	{
		lcfgfoundfiles = archivefileconts.filter(syslinuxcfgtypes.at(i), Qt::CaseInsensitive);
		if (!lcfgfoundfiles.isEmpty())
		{
			for (int j = 0; j < lcfgfoundfiles.size(); ++j)
			{
				randtmpfile ccfgftf(ubntmpf, "cfg");
				extractfile(archivefileconts.filter(syslinuxcfgtypes.at(i), Qt::CaseInsensitive).at(j), ccfgftf.fileName(), archivefile);
				syslinuxpcfg = getcfgkernargs(ccfgftf.fileName(), archivefile, archivefileconts).trimmed();
				ccfgftf.remove();
				if (!syslinuxpcfg.isEmpty())
					break;
			}
		}
		if (!syslinuxpcfg.isEmpty())
			break;
	}
	if (syslinuxpcfg.isEmpty())
	{
		return grubpcfg;
	}
	else
	{
		return syslinuxpcfg;
	}
}

QPair<QPair<QStringList, QStringList>, QPair<QStringList, QStringList> > unetbootin::extractcfgL(QString archivefile, QStringList archivefileconts)
{
	pdesc1->setText(QString("Extracting bootloader configuration"));
	QPair<QPair<QStringList, QStringList>, QPair<QStringList, QStringList> > grubpcfgPL;
	QPair<QPair<QStringList, QStringList>, QPair<QStringList, QStringList> > syslinuxpcfgPL;
	QPair<QPair<QStringList, QStringList>, QPair<QStringList, QStringList> > combinedcfgPL;
	QPair<QPair<QStringList, QStringList>, QPair<QStringList, QStringList> > filteredcfgPL;
	QStringList grubcfgtypes = QStringList() << "menu.lst" << "grub.conf";
	QStringList mlstfoundfiles;
	for (int i = 0; i < grubcfgtypes.size(); ++i)
	{
		mlstfoundfiles = archivefileconts.filter(grubcfgtypes.at(i), Qt::CaseInsensitive);
		if (!mlstfoundfiles.isEmpty())
		{
			for (int j = 0; j < mlstfoundfiles.size(); ++j)
			{
				randtmpfile mlstftf(ubntmpf, "lst");
				extractfile(archivefileconts.filter(grubcfgtypes.at(i), Qt::CaseInsensitive).at(j), mlstftf.fileName(), archivefile);
				grubpcfgPL = getgrubcfgargsL(mlstftf.fileName());
				mlstftf.remove();
				combinedcfgPL.first.first += grubpcfgPL.first.first;
				combinedcfgPL.first.second += grubpcfgPL.first.second;
				combinedcfgPL.second.first += grubpcfgPL.second.first;
				combinedcfgPL.second.second += grubpcfgPL.second.second;
//				if (!grubpcfg.isEmpty())
//					break;
			}
		}
//		if (!grubpcfg.isEmpty())
//			break;
	}
	QStringList syslinuxcfgtypes = QStringList() << "syslinux.cfg" << "isolinux.cfg" << "extlinux.cfg" << "pxelinux.cfg" << "menu_en.cfg" << "en.cfg" << ".cfg";
	QStringList lcfgfoundfiles;
	for (int i = 0; i < syslinuxcfgtypes.size(); ++i)
	{
		lcfgfoundfiles = archivefileconts.filter(syslinuxcfgtypes.at(i), Qt::CaseInsensitive);
		if (!lcfgfoundfiles.isEmpty())
		{
			for (int j = 0; j < lcfgfoundfiles.size(); ++j)
			{
				randtmpfile ccfgftf(ubntmpf, "cfg");
				extractfile(archivefileconts.filter(syslinuxcfgtypes.at(i), Qt::CaseInsensitive).at(j), ccfgftf.fileName(), archivefile);
				syslinuxpcfgPL = getcfgkernargsL(ccfgftf.fileName(), archivefile, archivefileconts);
				ccfgftf.remove();
				combinedcfgPL.first.first += syslinuxpcfgPL.first.first;
				combinedcfgPL.first.second += syslinuxpcfgPL.first.second;
				combinedcfgPL.second.first += syslinuxpcfgPL.second.first;
				combinedcfgPL.second.second += syslinuxpcfgPL.second.second;
//				if (!syslinuxpcfg.isEmpty())
//					break;
			}
		}
//		if (!syslinuxpcfg.isEmpty())
//			break;
	}
	/*
	if (syslinuxpcfg.isEmpty())
	{
		return grubpcfg;
	}
	else
	{
		return syslinuxpcfg;
	}
	*/
	for (int i = 0; i < combinedcfgPL.first.first.size(); ++i)
	{
		bool isduplicate = false;
		for (int j = 0; j < filteredcfgPL.second.first.size(); ++j)
		{
			if (filteredcfgPL.second.first.at(j) == combinedcfgPL.second.first.at(i))
			{
				isduplicate = true;
				break;
			}
		}
		if (isduplicate)
			break;
		if (combinedcfgPL.first.first.at(i) == kernelLoc && combinedcfgPL.first.second.at(i) == initrdLoc && combinedcfgPL.second.first.at(i).contains("Untitled Entry") && combinedcfgPL.second.second.at(i).isEmpty())
			continue;
//		else if (filteredcfgPL.second.first.contains(combinedcfgPL.second.first.at(i)))
//			continue;
		else
		{
			if (combinedcfgPL.first.first.at(i).isEmpty())
				filteredcfgPL.first.first.append(kernelLoc);
			else
			{
				QString indvkrnloc = getfullarchivepath(combinedcfgPL.first.first.at(i), archivefileconts);
				if (indvkrnloc.isEmpty())
					filteredcfgPL.first.first.append(kernelLoc);
				else
					filteredcfgPL.first.first.append(indvkrnloc);
			}
//			filteredcfgPL.first.first.append(combinedcfgPL.first.first.at(i));
			if (combinedcfgPL.first.second.at(i).isEmpty())
				filteredcfgPL.first.second.append(kernelLoc);
			else
			{
				QString indvitrloc = getfullarchivepath(combinedcfgPL.first.second.at(i), archivefileconts);
				if (indvitrloc.isEmpty())
				{
					filteredcfgPL.first.second.append(initrdLoc);
				}
				else
					filteredcfgPL.first.second.append(indvitrloc);
			}
//			filteredcfgPL.first.second.append(combinedcfgPL.first.second.at(i));
			filteredcfgPL.second.first.append(combinedcfgPL.second.first.at(i));
			filteredcfgPL.second.second.append(combinedcfgPL.second.second.at(i));
		}
	}
#ifdef NOINITRD
	for (int i = 0; i < combinedcfgPL.first.second.size(); ++i)
	{
		combinedcfgPL.first.second[i] = "";
	}
	for (int i = 0; i < combinedcfgPL.second.second.size(); ++i)
	{
		combinedcfgPL.second.second[i] = "";
	}
#endif
	return filteredcfgPL;
}

QString unetbootin::getfullarchivepath(QString relativefilepath, QStringList archivefile)
{
	QStringList pfoundmatches;
	relativefilepath = QDir::fromNativeSeparators(relativefilepath);
	if (relativefilepath.endsWith('/'))
		relativefilepath = relativefilepath.left(relativefilepath.size()-1);
	if (!relativefilepath.startsWith('/'))
		relativefilepath = QString("/%1").arg(relativefilepath);
//	if (!relativefilepath.endsWith('/'))
//		relativefilepath = QString("%1/").arg(relativefilepath);
	for (int i = 0; i < archivefile.size(); ++i)
	{
		QString curarchiveitem = QDir::fromNativeSeparators(archivefile.at(i));
		if (curarchiveitem.endsWith('/'))
			curarchiveitem = curarchiveitem.left(curarchiveitem.size()-1);
		if (!curarchiveitem.startsWith('/'))
			curarchiveitem = QString("/%1").arg(curarchiveitem);
//		if (!curarchiveitem.endsWith('/'))
//			curarchiveitem = QString("%1/").arg(curarchiveitem);
		if (curarchiveitem.contains(QRegExp(relativefilepath+"$", Qt::CaseInsensitive)))
			pfoundmatches.append(curarchiveitem);
	}
	if (pfoundmatches.isEmpty())
		return "";
	else
	{
		return filteroutlist(pfoundmatches, ignoredtypesbothRL);
	}
}

QString unetbootin::filteroutlist(QString listofdata, QList<QRegExp> listofmatches)
{
	if (listofdata.isEmpty())
		return "";
	return filteroutlist(QStringList() << listofdata, listofmatches);
}

QString unetbootin::filteroutlist(QStringList listofdata, QList<QRegExp> listofmatches)
{
	if (listofdata.isEmpty())
		return "";
	QStringList fldatalist = filteroutlistL(listofdata, listofmatches);
	if (!fldatalist.isEmpty())
		return fldatalist.at(0);
	else
		return "";
}

QStringList unetbootin::filteroutlistL(QStringList listofdata, QList<QRegExp> listofmatches)
{
	if (listofdata.isEmpty())
		return QStringList();
	if (listofmatches.isEmpty())
		return listofdata;
	if (listofdata.size() == 1)
		return listofdata;
	QStringList newlistofdata;
	for (int i = 0; i < listofdata.size(); ++i)
	{
		if (!listofdata.at(i).contains(listofmatches.at(0)))
			newlistofdata.append(listofdata.at(i));
	}
//	QStringList newlistofdata = QStringList(listofdata).filter(listofmatches.at(0));
	listofmatches.removeAt(0);
	if (newlistofdata.isEmpty())
	{
		return filteroutlistL(listofdata, listofmatches);
	}
	else
	{
		return filteroutlistL(newlistofdata, listofmatches);
	}
}

void unetbootin::extractiso(QString isofile, QString exoutputdir)
{
	sdesc1->setText(QString(sdesc1->text()).remove("<b>").replace("(Current)</b>", "(Done)"));
	sdesc2->setText(QString("<b>%1 (Current)</b>").arg(sdesc2->text()));
	tprogress->setValue(0);
	QPair<QPair<QStringList, QList<quint64> >, QStringList> listfilesizedirpair = listarchiveconts(isofile);
	if (listfilesizedirpair.first.first.size() == 1)
	{
		QString subarchivename = listfilesizedirpair.first.first.at(0);
		randtmpfile tmpoutsubarchive(ubntmpf, subarchivename.right(3));
		pdesc1->setText(tr("<b>Extracting compressed iso:</b> %1").arg(subarchivename));
		extractfile(subarchivename, tmpoutsubarchive.fileName(), isofile);
		return extractiso(tmpoutsubarchive.fileName(), exoutputdir);
	}
	QFileInfo isofileFI(isofile);
	qint64 isofileSize = isofileFI.size();
	if (listfilesizedirpair.first.first.size() < 10 && isofileSize > 12)
	{
		bool foundiso = false;
		quint64 isofileSizeOneFourth = isofileSize / 4;
		quint64 isofileSizeThreeFourth = isofileSizeOneFourth * 3;
		for (int i = 0; i < listfilesizedirpair.first.first.size() && i < listfilesizedirpair.first.second.size(); ++i)
		{
			if (listfilesizedirpair.first.first.at(i).contains(QRegExp(".iso$", Qt::CaseInsensitive)))
			{
				if (foundiso)
					break;
				foundiso = true;
				if (listfilesizedirpair.first.second.at(i) > isofileSizeThreeFourth)
				{
					QString subarchivename = listfilesizedirpair.first.first.at(i);
					randtmpfile tmpoutsubarchive(ubntmpf, subarchivename.right(3));
					pdesc1->setText(tr("<b>Extracting compressed iso:</b> %1").arg(subarchivename));
					extractfile(subarchivename, tmpoutsubarchive.fileName(), isofile);
					return extractiso(tmpoutsubarchive.fileName(), exoutputdir);
				}
			}
		}
	}
	kernelOpts = extractcfg(isofile, listfilesizedirpair.first.first);
	extraoptionsPL = extractcfgL(isofile, listfilesizedirpair.first.first);
#ifndef NOEXTRACTKERNEL
	extractkernel(isofile, QString("%1ubnkern").arg(exoutputdir), listfilesizedirpair.first);
#endif
#ifndef NOEXTRACTINITRD
	extractinitrd(isofile, QString("%1ubninit").arg(exoutputdir), listfilesizedirpair.first);
#endif
	QStringList createdpaths = makepathtree(targetDrive, listfilesizedirpair.second);
	QFile ubnpathlF(QDir::toNativeSeparators(QString("%1ubnpathl.txt").arg(exoutputdir)));
	ubnpathlF.open(QIODevice::WriteOnly | QIODevice::Text);
	QTextStream ubnpathlS(&ubnpathlF);
	for (int i = createdpaths.size() - 1; i > -1; i--)
	{
		ubnpathlS << createdpaths.at(i) << endl;
	}
	ubnpathlF.close();
	QStringList extractedfiles = extractallfiles(isofile, targetDrive, listfilesizedirpair.first);
	QFile ubnfilelF(QDir::toNativeSeparators(QString("%1ubnfilel.txt").arg(exoutputdir)));
	ubnfilelF.open(QIODevice::WriteOnly | QIODevice::Text);
	QTextStream ubnfilelS(&ubnfilelF);
	for (int i = 0; i < extractedfiles.size(); ++i)
	{
		ubnfilelS << extractedfiles.at(i) << endl;
	}
	ubnfilelF.close();
}

QStringList unetbootin::makepathtree(QString dirmkpathw, QStringList pathlist)
{
	QStringList createdpaths;
	QDir dir(dirmkpathw);
	for (int i = 0; i < pathlist.size(); ++i)
	{
		if (dir.mkdir(pathlist.at(i)))
		{
			createdpaths.append(pathlist.at(i));
		}
	}
	return createdpaths;
}

QStringList unetbootin::extractallfiles(QString archivefile, QString dirxfilesto, QPair<QStringList, QList<quint64> > filesizelist)
{
	QStringList filelist = filesizelist.first;
	QStringList extractedfiles;
	QProgressDialog xprogress;
	tprogress->setMaximum(filelist.size());
	tprogress->setMinimum(0);
	tprogress->setValue(0);
	pdesc5->setText(tr("Extracting files, please wait..."));
	pdesc4->setText(tr("<b>Archive:</b> %1").arg(archivefile));
	pdesc3->setText(tr("<b>Source:</b>"));
	pdesc2->setText(tr("<b>Destination:</b>"));
	pdesc1->setText(tr("<b>Extracted:</b> 0 of %1 files").arg(filelist.size()));
	for (int i = 0; i < filelist.size(); ++i)
	{
		pdesc3->setText(tr("<b>Source:</b> %1 (%2)").arg(filelist.at(i)).arg(displayfisize(filesizelist.second.at(i))));
		pdesc2->setText(tr("<b>Destination:</b> %1%2").arg(dirxfilesto).arg(filelist.at(i)));
		pdesc1->setText(tr("<b>Extracted:</b> %1 of %2 files").arg(i).arg(filelist.size()));
		tprogress->setValue(i);
		if (extractfile(filelist.at(i), QString("%1%2").arg(dirxfilesto).arg(filelist.at(i)), archivefile))
		{
			extractedfiles.append(filelist.at(i));
		}
	}
	pdesc5->setText("");
	pdesc4->setText("");
	pdesc3->setText("");
	pdesc2->setText("");
	pdesc1->setText("");
	tprogress->setValue(0);
	return extractedfiles;
}

QString unetbootin::getgrubcfgargs(QString cfgfile)
{
	QFile cfgfileF(cfgfile);
	cfgfileF.open(QIODevice::ReadOnly | QIODevice::Text);
	QTextStream cfgfileS(&cfgfileF);
	QString cfgfileCL;
	while (!cfgfileS.atEnd())
	{
		cfgfileCL = cfgfileS.readLine().trimmed();
		if (cfgfileCL.contains("#"))
		{
			cfgfileCL = cfgfileCL.left(cfgfileCL.indexOf("#")).trimmed();
		}
		if (cfgfileCL.contains(QRegExp("^kernel\\s{1,}\\S{1,}\\s{1,}\\S{1,}", Qt::CaseInsensitive)))
		{
                        return QString(cfgfileCL).remove(QRegExp("^kernel\\s{1,}\\S{1,}\\s{1,}", Qt::CaseInsensitive)).replace("rootfstype=iso9660", "rootfstype=auto").replace(QRegExp("root=CDLABEL=\\S{0,}"), QString("root=%1").arg(devluid)).replace("theme:sabayon", "theme:sabayon cdroot_type=vfat").trimmed();
		}
	}
	return "";
}

QPair<QPair<QStringList, QStringList>, QPair<QStringList, QStringList> > unetbootin::getgrubcfgargsL(QString cfgfile)
{
	QPair<QStringList, QStringList> kernelandinitrd;
	QPair<QStringList, QStringList> titleandparams;
	int curindex = 0;
	bool kernelpassed = false;
	QFile cfgfileF(cfgfile);
	cfgfileF.open(QIODevice::ReadOnly | QIODevice::Text);
	QTextStream cfgfileS(&cfgfileF);
	QString cfgfileCL;
	kernelandinitrd.first.append(kernelLoc);
	kernelandinitrd.second.append(initrdLoc);
	titleandparams.first.append(QString("Untitled Entry Grub %1").arg(curindex));
	titleandparams.second.append("");
	while (!cfgfileS.atEnd())
	{
		cfgfileCL = cfgfileS.readLine().trimmed();
		if (cfgfileCL.contains("#"))
		{
			cfgfileCL = cfgfileCL.left(cfgfileCL.indexOf("#")).trimmed();
		}
		if (cfgfileCL.contains(QRegExp("^title\\s{1,}\\S{1,}", Qt::CaseInsensitive)))
		{
			if (kernelpassed)
			{
				++curindex;
				kernelandinitrd.first.append(kernelLoc);
				kernelandinitrd.second.append(initrdLoc);
				titleandparams.first.append(QString("Untitled Entry Grub %1").arg(curindex));
				titleandparams.second.append("");
				kernelpassed = false;
			}
			titleandparams.first[curindex] = QString(cfgfileCL).remove("title", Qt::CaseInsensitive).trimmed();
			continue;
		}
		if (cfgfileCL.contains(QRegExp("^initrd\\s{1,}\\S{1,}", Qt::CaseInsensitive)))
		{
			kernelandinitrd.second[curindex] = getFirstTextBlock(QString(cfgfileCL).remove(QRegExp("^initrd", Qt::CaseInsensitive)).trimmed());
//			if (kernelandinitrd.second.at(curindex).isEmpty())
//				kernelandinitrd.second[curindex] = initrdLoc;
			continue;
		}
//		if (cfgfileCL.contains(QRegExp("^module\\s{1,}\\S{1,}", Qt::CaseInsensitive)))
//		{
//			kernelandinitrd.second[curindex] = getFirstTextBlock(QString(cfgfileCL).remove(QRegExp("^module", Qt::CaseInsensitive)).trimmed());
//			if (kernelandinitrd.second.at(curindex).isEmpty())
//				kernelandinitrd.second[curindex] = initrdLoc;
//		}
		if (cfgfileCL.contains(QRegExp("^kernel\\s{1,}\\S{1,}", Qt::CaseInsensitive)))
		{
			if (kernelpassed)
			{
				++curindex;
				kernelandinitrd.first.append(kernelLoc);
				kernelandinitrd.second.append(initrdLoc);
				titleandparams.first.append(QString("Untitled Entry Grub %1").arg(curindex));
				titleandparams.second.append("");
//				kernelpassed = false;
			}
			if (cfgfileCL.contains(QRegExp("^kernel\\s{1,}\\S{1,}\\s{1,}\\S{1,}", Qt::CaseInsensitive)))
			{
                                titleandparams.second[curindex] = QString(cfgfileCL).remove(QRegExp("^kernel\\s{1,}\\S{1,}\\s{1,}", Qt::CaseInsensitive)).replace("rootfstype=iso9660", "rootfstype=auto").replace(QRegExp("root=CDLABEL=\\S{0,}"), QString("root=%1").arg(devluid)).replace("theme:sabayon", "theme:sabayon cdroot_type=vfat").trimmed();
			}
			kernelandinitrd.first[curindex] = getFirstTextBlock(QString(cfgfileCL).remove(QRegExp("^kernel", Qt::CaseInsensitive)).trimmed());
//			if (kernelandinitrd.first.at(curindex).isEmpty())
//				kernelandinitrd.first[curindex] = kernelLoc;
			kernelpassed = true;
			continue;
		}
	}
	return qMakePair(kernelandinitrd, titleandparams);
}

QString unetbootin::getFirstTextBlock(QString fulltext)
{
	QStringList textblockL = fulltext.split(QRegExp("\\s{1,}")).filter(QRegExp("\\S{1,}"));
	if (textblockL.isEmpty())
		return "";
	else
	{
		return textblockL.at(0);
	}
}

QString unetbootin::getcfgkernargs(QString cfgfile, QString archivefile, QStringList archivefileconts)
{
	QFile cfgfileF(cfgfile);
	cfgfileF.open(QIODevice::ReadOnly | QIODevice::Text);
	QTextStream cfgfileS(&cfgfileF);
	QString cfgfileCL;
	QString includesfile;
	QString searchincfrs;
	while (!cfgfileS.atEnd())
	{
		cfgfileCL = cfgfileS.readLine().trimmed();
		if (cfgfileCL.contains("#"))
		{
			cfgfileCL = cfgfileCL.left(cfgfileCL.indexOf("#")).trimmed();
		}
		if (!archivefileconts.isEmpty() && QRegExp("^include\\s{1,}\\S{1,}.cfg$", Qt::CaseInsensitive).exactMatch(cfgfileCL))
		{
			includesfile = QDir::toNativeSeparators(QString(cfgfileCL).remove(QRegExp("^include\\s{1,}", Qt::CaseInsensitive))).trimmed();
			searchincfrs = searchforincludesfile(includesfile, archivefile, archivefileconts).trimmed();
			if (!searchincfrs.isEmpty())
				return searchincfrs;
		}
		if (!archivefileconts.isEmpty() && QRegExp("^append\\s{1,}\\S{1,}.cfg$", Qt::CaseInsensitive).exactMatch(cfgfileCL))
		{
			includesfile = QDir::toNativeSeparators(QString(cfgfileCL).remove(QRegExp("^append\\s{1,}", Qt::CaseInsensitive))).trimmed();
			searchincfrs = searchforincludesfile(includesfile, archivefile, archivefileconts).trimmed();
			if (!searchincfrs.isEmpty())
				return searchincfrs;
		}
		else if (cfgfileCL.contains(QRegExp("^\\s{0,}append\\s{1,}", Qt::CaseInsensitive)))
		{
                        return QString(cfgfileCL).remove(QRegExp("\\s{0,}append\\s{1,}", Qt::CaseInsensitive)).remove(QRegExp("\\s{0,1}initrd=\\S{0,}", Qt::CaseInsensitive)).replace("rootfstype=iso9660", "rootfstype=auto").replace(QRegExp("root=CDLABEL=\\S{0,}"), QString("root=%1").arg(devluid)).replace("theme:sabayon", "theme:sabayon cdroot_type=vfat").trimmed();
		}
	}
	return "";
}

QPair<QPair<QStringList, QStringList>, QPair<QStringList, QStringList> > unetbootin::getcfgkernargsL(QString cfgfile, QString archivefile, QStringList archivefileconts)
{
	/*
	QString cfgfiledir;
	if (cfgfile.contains(QDir::toNativeSeparators("/")))
		cfgfiledir = QDir::fromNativeSeparators(QString(cfgfile).left(cfgfile.lastIndexOf(QDir::toNativeSeparators("/")) + 1));
	if (!cfgfiledir.isEmpty())
	{
		if (!cfgfiledir.startsWith('/'))
			cfgfiledir = QString("/%1").arg(cfgfiledir);
		if (!cfgfiledir.endsWith('/'))
			cfgfiledir = QString("%1/").arg(cfgfiledir);
	}
	*/
	QPair<QStringList, QStringList> kernelandinitrd;
	QPair<QStringList, QStringList> titleandparams;
	int curindex = 0;
	bool kernelpassed = false;
	QFile cfgfileF(cfgfile);
	cfgfileF.open(QIODevice::ReadOnly | QIODevice::Text);
	QTextStream cfgfileS(&cfgfileF);
	QString cfgfileCL;
	kernelandinitrd.first.append(kernelLoc);
	kernelandinitrd.second.append(initrdLoc);
	titleandparams.first.append(QString("Untitled Entry Grub %1").arg(curindex));
	titleandparams.second.append("");
	QString includesfile;
	QPair<QPair<QStringList, QStringList>, QPair<QStringList, QStringList> > searchincfrs;
	while (!cfgfileS.atEnd())
	{
		cfgfileCL = cfgfileS.readLine().trimmed();
		if (cfgfileCL.contains("#"))
		{
			cfgfileCL = cfgfileCL.left(cfgfileCL.indexOf("#")).trimmed();
		}
		if (!archivefileconts.isEmpty() && QRegExp("^include\\s{1,}\\S{1,}.cfg$", Qt::CaseInsensitive).exactMatch(cfgfileCL))
		{
			includesfile = QDir::toNativeSeparators(QString(cfgfileCL).remove(QRegExp("^include\\s{1,}", Qt::CaseInsensitive))).trimmed();
			searchincfrs = searchforincludesfileL(includesfile, archivefile, archivefileconts);
			if (!searchincfrs.first.first.isEmpty())
			{
				kernelandinitrd.first += searchincfrs.first.first;
				kernelandinitrd.second += searchincfrs.first.second;
				titleandparams.first += searchincfrs.second.first;
				titleandparams.second += searchincfrs.second.second;
			}
			continue;
		}
		if (!archivefileconts.isEmpty() && QRegExp("^append\\s{1,}\\S{1,}.cfg$", Qt::CaseInsensitive).exactMatch(cfgfileCL))
		{
			includesfile = QDir::toNativeSeparators(QString(cfgfileCL).remove(QRegExp("^append\\s{1,}", Qt::CaseInsensitive))).trimmed();
			searchincfrs = searchforincludesfileL(includesfile, archivefile, archivefileconts);
			if (!searchincfrs.first.first.isEmpty())
			{
				kernelandinitrd.first += searchincfrs.first.first;
				kernelandinitrd.second += searchincfrs.first.second;
				titleandparams.first += searchincfrs.second.first;
				titleandparams.second += searchincfrs.second.second;
			}
			continue;
		}
		if (cfgfileCL.contains(QRegExp("^menu label\\s{1,}\\S{1,}", Qt::CaseInsensitive)))
		{
			titleandparams.first[curindex] = QString(cfgfileCL).remove(QRegExp("^menu label", Qt::CaseInsensitive)).trimmed();
			continue;
		}
		if (cfgfileCL.contains(QRegExp("^append\\s{1,}\\S{1,}", Qt::CaseInsensitive)))
		{
			QString appendoptsL = QString(cfgfileCL).remove(QRegExp("^append", Qt::CaseInsensitive)).trimmed();
			if (appendoptsL.contains(QRegExp("initrd=\\S{0,}", Qt::CaseInsensitive)))
			{
				kernelandinitrd.second[curindex] = getFirstTextBlock(QString(appendoptsL).remove(QRegExp(".{0,}initrd=", Qt::CaseInsensitive)));
				appendoptsL = QString(appendoptsL).remove(QRegExp("initrd=\\S{0,}", Qt::CaseInsensitive));
//				if (kernelandinitrd.second.at(curindex).isEmpty())
//					kernelandinitrd.second[curindex] = initrdLoc;
//				else if (!kernelandinitrd.second.at(curindex).contains('/'))
//					kernelandinitrd.second[curindex] = QString("%1%2").arg(cfgfiledir, kernelandinitrd.second.at(curindex));
			}
                        titleandparams.second[curindex] = QString(appendoptsL).replace("rootfstype=iso9660", "rootfstype=auto").replace(QRegExp("root=CDLABEL=\\S{0,}"), QString("root=%1").arg(devluid)).replace("theme:sabayon", "theme:sabayon cdroot_type=vfat").trimmed();
			continue;
		}
		if (cfgfileCL.contains(QRegExp("^label\\s{1,}\\S{1,}", Qt::CaseInsensitive)))
		{
			if (kernelpassed)
			{
				++curindex;
				kernelandinitrd.first.append(kernelLoc);
				kernelandinitrd.second.append(initrdLoc);
				titleandparams.first.append(QString("Untitled Entry Syslinux %1").arg(curindex));
				titleandparams.second.append("");
				kernelpassed = false;
			}
			titleandparams.first[curindex] = QString(cfgfileCL).remove(QRegExp("^label", Qt::CaseInsensitive)).trimmed();
			continue;
		}
		if (cfgfileCL.contains(QRegExp("^kernel\\s{1,}\\S{1,}", Qt::CaseInsensitive)))
		{
			if (kernelpassed)
			{
				++curindex;
				kernelandinitrd.first.append(kernelLoc);
				kernelandinitrd.second.append(initrdLoc);
				titleandparams.first.append(QString("Untitled Entry Syslinux %1").arg(curindex));
				titleandparams.second.append("");
//				kernelpassed = false;
			}
			kernelandinitrd.first[curindex] = getFirstTextBlock(QString(cfgfileCL).remove(QRegExp("^kernel", Qt::CaseInsensitive)).trimmed());
//			if (kernelandinitrd.first.at(curindex).isEmpty())
//				kernelandinitrd.first[curindex] = kernelLoc;
//			else if (!kernelandinitrd.first.at(curindex).contains('/'))
//				kernelandinitrd.first[curindex] = QString("%1%2").arg(cfgfiledir, kernelandinitrd.first.at(curindex));
			kernelpassed = true;
			continue;
		}
	}
	return qMakePair(kernelandinitrd, titleandparams);
}

QString unetbootin::searchforincludesfile(QString includesfile, QString archivefile, QStringList archivefileconts)
{
	if (includesfile.startsWith(QDir::toNativeSeparators("/")))
	{
		includesfile = includesfile.right(includesfile.size() - 1).trimmed();
	}
	QStringList includesfileL = archivefileconts.filter(includesfile, Qt::CaseInsensitive);
	if (!includesfileL.isEmpty())
	{
		for (int i = 0; i < includesfileL.size(); ++i)
		{
			randtmpfile tmpoutputcfgf(ubntmpf, "cfg");
			extractfile(includesfileL.at(i), tmpoutputcfgf.fileName(), archivefile);
			QString extractcfgtmp = getcfgkernargs(tmpoutputcfgf.fileName(), archivefile, archivefileconts).trimmed();
			tmpoutputcfgf.remove();
			if (!extractcfgtmp.isEmpty())
			{
				return extractcfgtmp;
			}
		}
	}
	return "";
}

QPair<QPair<QStringList, QStringList>, QPair<QStringList, QStringList> > unetbootin::searchforincludesfileL(QString includesfile, QString archivefile, QStringList archivefileconts)
{
	if (includesfile.startsWith(QDir::toNativeSeparators("/")))
	{
		includesfile = includesfile.right(includesfile.size() - 1).trimmed();
	}
	QStringList includesfileL = archivefileconts.filter(includesfile, Qt::CaseInsensitive);
	if (!includesfileL.isEmpty())
	{
		for (int i = 0; i < includesfileL.size(); ++i)
		{
			randtmpfile tmpoutputcfgf(ubntmpf, "cfg");
			extractfile(includesfileL.at(i), tmpoutputcfgf.fileName(), archivefile);
			QPair<QPair<QStringList, QStringList>, QPair<QStringList, QStringList> > extractcfgtmp = getcfgkernargsL(tmpoutputcfgf.fileName(), archivefile, archivefileconts);
			tmpoutputcfgf.remove();
			if (!extractcfgtmp.first.first.isEmpty())
			{
				return extractcfgtmp;
			}
		}
	}
	return QPair<QPair<QStringList, QStringList>, QPair<QStringList, QStringList> >();
}

void unetbootin::downloadfile(QString fileurl, QString targetfile)
{
	if (fileurl.isEmpty())
		return;
	if (QFile::exists(targetfile))
	{
		QFile::remove(targetfile);
	}
	QUrl dlurl(fileurl);
	bool isftp = false;
	if (dlurl.scheme() == "ftp")
	{
		isftp = true;
	}
	QHttp dlhttp;
	QFtp dlftp;
	QEventLoop dlewait;
	pdesc5->setText("");
	pdesc4->setText(tr("Downloading files, please wait..."));
	pdesc3->setText(tr("<b>Source:</b> <a href=\"%1\">%1</a>").arg(fileurl));
	pdesc2->setText(tr("<b>Destination:</b> %1").arg(targetfile));
	pdesc1->setText(tr("<b>Downloaded:</b> 0 bytes"));
	QString realupath = QString(fileurl).remove(0, fileurl.indexOf(QString("://%1").arg(dlurl.host())) + QString("://%1").arg(dlurl.host()).length());
	if (isftp)
	{
		connect(&dlftp, SIGNAL(done(bool)), &dlewait, SLOT(quit()));
		connect(&dlftp, SIGNAL(dataTransferProgress(qint64, qint64)), this, SLOT(dlprogressupdate64(qint64, qint64)));
	}
	else
	{
		connect(&dlhttp, SIGNAL(done(bool)), &dlewait, SLOT(quit()));
		connect(&dlhttp, SIGNAL(dataReadProgress(int, int)), this, SLOT(dlprogressupdate(int, int)));
	}
	QFile dloutfile;
	if (installType == tr("USB Drive"))
	{
		dloutfile.setFileName(randtmpfile::getrandfilename(ubntmpf, "tmp"));
	}
	else
	{
		dloutfile.setFileName(targetfile);
	}
	dloutfile.open(QIODevice::WriteOnly);
	if (isftp)
	{
		dlftp.connectToHost(dlurl.host());
		dlftp.login();
		dlftp.get(realupath, &dloutfile);
	}
	else
	{
		dlhttp.setHost(dlurl.host());
		ubngetrequestheader dlrequest(dlurl.host(), realupath);
		dlhttp.request(dlrequest, 0, &dloutfile);
	}
	dlewait.exec();
	if (!isftp)
	{
		QHttpResponseHeader dlresponse(dlhttp.lastResponse());
		int dlrstatus = dlresponse.statusCode();
		if (dlrstatus >= 300 && dlrstatus < 400 && dlresponse.hasKey("Location"))
		{
			dloutfile.close();
			dloutfile.remove();
			downloadfile(dlresponse.value("Location"), targetfile);
		}
	}
	if (isftp)
	{
		dlftp.close();
	}
	else
	{
		dlhttp.close();
	}
	dloutfile.close();
	if (installType == tr("USB Drive"))
	{
		dloutfile.rename(targetfile);
	}
	pdesc4->setText("");
	pdesc3->setText("");
	pdesc2->setText("");
	pdesc1->setText("");
	tprogress->setValue(0);
}

void unetbootin::dlprogressupdate(int dlbytes, int maxbytes)
{
	tprogress->setValue(dlbytes);
	tprogress->setMaximum(maxbytes);
	pdesc1->setText(tr("<b>Downloaded:</b> %1 of %2 bytes").arg(dlbytes).arg(maxbytes));
}

void unetbootin::dlprogressupdate64(qint64 dlbytes, qint64 maxbytes)
{
	tprogress->setValue(dlbytes);
	tprogress->setMaximum(maxbytes);
	pdesc1->setText(tr("<b>Downloaded:</b> %1 of %2 bytes").arg(dlbytes).arg(maxbytes));
}

QString unetbootin::downloadpagecontents(QString pageurl)
{
	QUrl pgurl(pageurl);
	QHttp pghttp;
	QEventLoop pgwait;
	connect(&pghttp, SIGNAL(done(bool)), &pgwait, SLOT(quit()));
	pghttp.setHost(pgurl.host());
	QString realpgupath = QString(pageurl).remove(0, pageurl.indexOf(QString("://%1").arg(pgurl.host())) + QString("://%1").arg(pgurl.host()).length());
	ubngetrequestheader pgrequest(pgurl.host(), realpgupath);
	pghttp.request(pgrequest);
	pgwait.exec();
	QHttpResponseHeader pgresponse(pghttp.lastResponse());
	int pgrstatus = pgresponse.statusCode();
	if (pgrstatus >= 300 && pgrstatus < 400 && pgresponse.hasKey("Location"))
	{
		return downloadpagecontents(pgresponse.value("Location"));
	}
	else
	{
		return QString(pghttp.readAll());
	}
}

QStringList unetbootin::lstFtpDirFiles(QString ldfDirStringUrl, int ldfMinSize, int ldfMaxSize)
{
	QUrl ldfDirUrl(ldfDirStringUrl);
	QFtp ldfFtp;
	QEventLoop ldfWait;
	nDirListStor nDirListStorL;
	nDirListStorL.nMinFileSizeBytes = ldfMinSize;
	nDirListStorL.nMaxFileSizeBytes = ldfMaxSize;
	nDirListStorL.searchsymlinks = this->searchsymlinks;
	connect(&ldfFtp, SIGNAL(done(bool)), &ldfWait, SLOT(quit()));
	connect(&ldfFtp, SIGNAL(listInfo(QUrlInfo)), &nDirListStorL, SLOT(sAppendSelfUrlInfoList(QUrlInfo)));
	ldfFtp.connectToHost(ldfDirUrl.host());
	ldfFtp.login();
	ldfFtp.list(ldfDirUrl.path());
	ldfWait.exec();
	ldfFtp.close();
	return nDirListStorL.nDirFileListSL;
}

QStringList unetbootin::lstHttpDirFiles(QString ldfDirStringUrl)
{
	QStringList relativefilelinksL;
	QStringList relativelinksL = downloadpagecontents(ldfDirStringUrl).replace(">", ">\n").replace("<", "\n<").split("\n").filter(QRegExp("<a href=\"(?!\\?)\\S{1,}\">", Qt::CaseInsensitive)).replaceInStrings(QRegExp("<a href=\"", Qt::CaseInsensitive), "").replaceInStrings("\">", "");
	for (int i = 0; i < relativelinksL.size(); ++i)
	{
		if (!relativelinksL.at(i).endsWith('/'))
			relativefilelinksL.append(relativelinksL.at(i));
	}
	return relativefilelinksL;
}

QStringList unetbootin::lstNetDirFiles(QString ldfDirStringUrl, int ldfMinSize, int ldfMaxSize)
{
	if (!ldfDirStringUrl.endsWith('/'))
		ldfDirStringUrl += '/';
	if (ldfDirStringUrl.startsWith("ftp"))
	{
		return lstFtpDirFiles(ldfDirStringUrl, ldfMinSize, ldfMaxSize);
	}
	else
	{
		return lstHttpDirFiles(ldfDirStringUrl);
	}
}

QPair<QString, int> unetbootin::weightedFilterNetDir(QString ldfDirStringUrl, int ldfMinSize, int ldfMaxSize, QList<QRegExp> ldfFileMatchExp)
{
	if (!ldfDirStringUrl.endsWith('/'))
		ldfDirStringUrl += '/';
	pdesc1->setText(tr("Searching in <a href=\"%1\">%1</a>").arg(ldfDirStringUrl));
	QPair<QString, int> relativeFileUrl = filterBestMatch(lstNetDirFiles(ldfDirStringUrl, ldfMinSize, ldfMaxSize), ldfFileMatchExp);
	if (relativeFileUrl.first.startsWith('/'))
		ldfDirStringUrl = ldfDirStringUrl.left(ldfDirStringUrl.indexOf('/', 8));
	pdesc2->setText(tr("%1/%2 matches in <a href=\"%3\">%3</a>").arg(relativeFileUrl.second).arg(ldfFileMatchExp.size()).arg(ldfDirStringUrl));
	return qMakePair(ldfDirStringUrl+relativeFileUrl.first, relativeFileUrl.second);
}

QString unetbootin::fileFilterNetDir(QStringList ldfDirStringUrlList, int ldfMinSize, int ldfMaxSize, QList<QRegExp> ldfFileMatchExp)
{
	QPair<QString, int> curRemoteFileUrlSP;
	int hRegxMatch = 0;
	QString hRegxMatchString;
	for (int i = 0; i < ldfDirStringUrlList.size(); ++i)
	{
		curRemoteFileUrlSP = weightedFilterNetDir(ldfDirStringUrlList.at(i), ldfMinSize, ldfMaxSize, ldfFileMatchExp);
		if (curRemoteFileUrlSP.second == ldfFileMatchExp.size())
			return curRemoteFileUrlSP.first;
		if (curRemoteFileUrlSP.second > hRegxMatch)
		{
			hRegxMatch = curRemoteFileUrlSP.second;
			hRegxMatchString = curRemoteFileUrlSP.first;
		}
	}
	return hRegxMatchString;
}

QPair<QString, int> unetbootin::filterBestMatch(QStringList ufStringList, QList<QRegExp> filterExpList)
{
	QString hRegxMatchString, hRegxMatchStringEnd;
	int hRegxMatch = 0;
	for (int i = 0; i < ufStringList.size(); ++i)
	{
		int regxmatches = 0;
		hRegxMatchStringEnd = ufStringList.at(i).right(ufStringList.at(i).size() - ufStringList.at(i).lastIndexOf('/') - 1);
		for (int j = 0; j < filterExpList.size(); ++j)
		{
			if (hRegxMatchStringEnd.contains(filterExpList.at(j)))
				++regxmatches;
		}
		if (regxmatches >= hRegxMatch)
		{
			hRegxMatchString = ufStringList.at(i);
			hRegxMatch = regxmatches;
		}
	}
	return qMakePair(hRegxMatchString, hRegxMatch);
}

void unetbootin::sysreboot()
{
	#ifdef Q_OS_WIN32
	HANDLE hToken;
	TOKEN_PRIVILEGES tkp;
	OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken);
	LookupPrivilegeValue(NULL, SE_SHUTDOWN_NAME, &tkp.Privileges[0].Luid);
	tkp.PrivilegeCount = 1;
	tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
	AdjustTokenPrivileges(hToken, FALSE, &tkp, 0, (PTOKEN_PRIVILEGES)NULL, 0);
	ExitWindowsEx(EWX_REBOOT, EWX_FORCE);
	#endif
	#ifdef Q_OS_UNIX
	callexternapp("init", "6 &");
	#endif
}

QString unetbootin::callexternapp(QString xexecFile, QString xexecParm)
{
	QEventLoop cxaw;
	callexternappT cxat;
	connect(&cxat, SIGNAL(finished()), &cxaw, SLOT(quit()));
	cxat.execFile = xexecFile;
	cxat.execParm = xexecParm;
	cxat.start();
	cxaw.exec();
	return cxat.retnValu;
}

QString unetbootin::getdevluid(QString voldrive)
{
	QString uuidS = getuuid(voldrive);
	if (uuidS == "None")
	{
		return QString("LABEL=%1").arg(getlabel(voldrive));
	}
	else
	{
		return QString("UUID=%1").arg(uuidS);
	}
}

QString unetbootin::getlabel(QString voldrive)
{
	#ifdef Q_OS_WIN32
	voldrive.append("\\");
	wchar_t vollabel[50];
	GetVolumeInformation(LPWSTR(voldrive.utf16()), vollabel, 50, NULL, NULL, NULL, NULL, NULL);
	QString vollabelS = QString::fromWCharArray(vollabel);
	if (vollabelS.isEmpty())
	{
		return "None";
	}
	else
	{
		return vollabelS;
	}
	#endif
	#ifdef Q_OS_UNIX
	QString volidpS = QString(callexternapp(volidcommand, QString("-l %1").arg(voldrive))).remove("\r").remove("\n").trimmed();
	if (volidpS.isEmpty())
	{
		return "None";
	}
	else
	{
		return volidpS;
	}
	#endif
}

QString unetbootin::getuuid(QString voldrive)
{
	#ifdef Q_OS_WIN32
	voldrive.append("\\");
	DWORD volserialnum;
	GetVolumeInformation(LPWSTR(voldrive.utf16()), NULL, NULL, &volserialnum, NULL, NULL, NULL, NULL);
	if (!volserialnum >= 1)
	{
		return "None";
	}
	QString tvolsernum = QString::number(volserialnum, 16).toUpper();
	if (tvolsernum.size() == 8)
	{
		return QString("%1-%2").arg(tvolsernum.left(4), tvolsernum.right(4));
	}
	else
	{
		return tvolsernum;
	}
	#endif
	#ifdef Q_OS_UNIX
	QString volidpS = QString(callexternapp(volidcommand, QString("-u %1").arg(voldrive))).remove("\r").remove("\n").trimmed();
	if (volidpS.isEmpty())
	{
		return "None";
	}
	else
	{
		return volidpS;
	}
	#endif
}

#ifdef Q_OS_UNIX

QString unetbootin::locatecommand(QString commandtolocate, QString reqforinstallmode, QString packagename)
{
	QString commandbinpath = callexternapp("which", commandtolocate).trimmed();
	if (!commandbinpath.isEmpty() && QFile::exists(commandbinpath))
		return commandbinpath;
//	QString commandbinpath = callexternapp("whereis", commandtolocate);
//	QStringList commandbinpathL = commandbinpath.split(" ").join("\n").split("\t").join("\n").split("\n");
//	for (int i = 0; i < commandbinpathL.size(); ++i)
//	{
//		if (commandbinpathL.at(i).contains("bin/"))
//		{
//			return commandbinpathL.at(i);
//		}
//	}
	QMessageBox errorcmdnotfoundmsgbx;
	errorcmdnotfoundmsgbx.setIcon(QMessageBox::Warning);
	errorcmdnotfoundmsgbx.setWindowTitle(QString(tr("%1 not found")).arg(commandtolocate));
	errorcmdnotfoundmsgbx.setText(QString(tr("%1 not found. This is required for %2 install mode.\nInstall the \"%3\" package or your distribution's equivalent.")).arg(commandtolocate, reqforinstallmode, packagename));
	errorcmdnotfoundmsgbx.setStandardButtons(QMessageBox::Ok);
	switch (errorcmdnotfoundmsgbx.exec())
	{
		case QMessageBox::Ok:
			break;
		default:
			break;
	}
 	return commandtolocate;
}

QString unetbootin::locatedevicenode(QString mountpoint)
{
	QFile procmountsF("/proc/mounts");
	procmountsF.open(QIODevice::ReadOnly | QIODevice::Text);
	QTextStream procmountsS(&procmountsF);
	QStringList rawdeviceL = procmountsS.readAll().replace("\t", " ").split("\n").filter("/dev/").filter(QString(" %1 ").arg(mountpoint));
	if (rawdeviceL.isEmpty())
	{
		return "NOT FOUND";
	}
	else
	{
		return QFileInfo(rawdeviceL.at(0).split(" ").at(0)).canonicalFilePath();
	}
}

QString unetbootin::locatemountpoint(QString devicenode)
{
	QFile procmountsF("/proc/mounts");
	procmountsF.open(QIODevice::ReadOnly | QIODevice::Text);
	QTextStream procmountsS(&procmountsF);
	QStringList procmountsL;
	procmountsL = procmountsS.readAll().split("\n").filter(devicenode);
	if (procmountsL.isEmpty())
	{
		return "NOT MOUNTED";
	}
	else
	{
		if (procmountsL.at(0).split("\t").join(" ").split(" ").size() >= 2)
		{
                        return procmountsL.at(0).split("\t").join(" ").split(" ")[1].replace("\\040", " ");
		}
		else
		{
			return "NOT MOUNTED";
		}
	}
}

QString unetbootin::getGrubNotation(QString devicenode)
{
	return QString("(hd%1,%2)").arg(getDiskNumber(devicenode)).arg(getPartitionNumber(devicenode));
}

int unetbootin::letterToNumber(QChar lettertoconvert)
{
	if (lettertoconvert.isLower())
	{
		return static_cast<int>(lettertoconvert.toAscii() - 'a');
	}
	if (lettertoconvert.isUpper())
	{
		return static_cast<int>(lettertoconvert.toAscii() - 'A');
	}
	else
	{
		return 999;
	}
}

int unetbootin::getDiskNumber(QString devicenode)
{
	QChar disknumchar(devicenode.at(devicenode.size() - 2));
	if (disknumchar.isLetter())
	{
		return letterToNumber(disknumchar);
	}
	else
	{
		return disknumchar.digitValue() - 1;
	}
}

int unetbootin::getPartitionNumber(QString devicenode)
{
	QChar partitionchar(devicenode.at(devicenode.size() - 1));
	if (partitionchar.isLetter())
	{
		return letterToNumber(partitionchar);
	}
	else
	{
		return partitionchar.digitValue() - 1;
	}
}

#endif

#ifdef Q_OS_WIN32

void unetbootin::installsvzip()
{
	if (QFile::exists(QString("%1\\7z.dll").arg(ubntmpf)))
	{
		QFile::remove(QString("%1\\7z.dll").arg(ubntmpf));
	}
	instIndvfl("sevnz.dll", QString("%1\\7z.dll").arg(ubntmpf));
	if (QFile::exists(QString("%1sevnz.exe").arg(ubntmpf)))
	{
		QFile::remove(QString("%1sevnz.exe").arg(ubntmpf));
	}
	instIndvfl("sevnz.exe", QString("%1sevnz.exe").arg(ubntmpf));
	sevzcommand = QString("%1sevnz.exe").arg(ubntmpf);
}

void unetbootin::configsysEdit()
{
	SetFileAttributesA(QDir::toNativeSeparators(QString("%1config.sys").arg(targetDrive)).toLocal8Bit(), FILE_ATTRIBUTE_NORMAL);
	QFile::copy(QDir::toNativeSeparators(QString("%1config.sys").arg(targetDrive)), QString("%1config.sys").arg(targetPath));
	QFile::copy(QDir::toNativeSeparators(QString("%1config.sys").arg(targetDrive)), QString("%1confignw.txt").arg(targetPath));
	QFile confignwFile(QString("%1confignw.txt").arg(targetPath));
	QFile configsysFile(QDir::toNativeSeparators(QString("%1config.sys").arg(targetDrive)));
	confignwFile.open(QIODevice::ReadWrite | QIODevice::Text);
	configsysFile.open(QIODevice::ReadOnly | QIODevice::Text);
	QTextStream confignwOut(&confignwFile);
	QTextStream configsysOut(&configsysFile);
	QString configsysText = QString("[menu]\n"
	"menucolor=15,0\n"
	"menuitem=windows,Windows\n"
	"menuitem=grub,"UNETBOOTINB"\n"
	"menudefault=windows,30\n"
	"[grub]\n"
	"device=ubnldr.exe\n"
	"[windows]\n%1").arg(configsysOut.readAll());
	confignwOut << configsysText << endl;
	if (!QFile::copy(QString("%1confignw.txt").arg(targetPath), QDir::toNativeSeparators(QString("%1config.sys").arg(targetDrive))))
	{
		configsysFile.remove();
		QFile::copy(QString("%1confignw.txt").arg(targetPath), QDir::toNativeSeparators(QString("%1config.sys").arg(targetDrive)));
	}
}

void unetbootin::bootiniEdit()
{
	SetFileAttributesW(LPWSTR(QDir::toNativeSeparators(QString("%1boot.ini").arg(targetDrive)).utf16()), FILE_ATTRIBUTE_NORMAL);
	QFile::copy(QDir::toNativeSeparators(QString("%1boot.ini").arg(targetDrive)), QString("%1boot.ini").arg(targetPath));
	QFile::copy(QDir::toNativeSeparators(QString("%1bootnw.ini").arg(targetDrive)), QString("%1bootnw.txt").arg(targetPath));
	QFile bootnwFile(QString("%1bootnw.txt").arg(targetPath));
	QFile bootiniFile(QDir::toNativeSeparators(QString("%1boot.ini").arg(targetDrive)));
	bootnwFile.open(QIODevice::ReadWrite | QIODevice::Text);
	bootiniFile.open(QIODevice::ReadOnly | QIODevice::Text);
	QTextStream bootnwOut(&bootnwFile);
	QTextStream bootiniOut(&bootiniFile);
	QStringList bootiniCurTextL;
	bool btimustreplacetimeout = true;
	QRegExp btichkistimeout("\\s{0,}timeout.{0,}", Qt::CaseInsensitive);
	QString bootiniCurLine;
	while (!bootiniOut.atEnd())
	{
		bootiniCurLine = bootiniOut.readLine();
		if (btimustreplacetimeout && btichkistimeout.exactMatch(bootiniCurLine))
		{
			bootiniCurTextL.append("timeout=15");
			btimustreplacetimeout = false;
		}
		else
		{
			bootiniCurTextL.append(bootiniCurLine);
		}
	}
	QString bootiniCurText = bootiniCurTextL.join("\n");
	QString bootiniText = QString("%1\n%2=\""UNETBOOTINB"\"").arg(bootiniCurText).arg(QDir::toNativeSeparators(QString("%1ubnldr.mbr").arg(targetDrive)));
	bootnwOut << bootiniText << endl;
	if (!QFile::copy(QString("%1bootnw.txt").arg(targetPath), QDir::toNativeSeparators(QString("%1boot.ini").arg(targetDrive))))
	{
		bootiniFile.remove();
		QFile::copy(QString("%1bootnw.txt").arg(targetPath), QDir::toNativeSeparators(QString("%1boot.ini").arg(targetDrive)));
	}
}

void unetbootin::vistabcdEdit()
{
	instIndvfl("emtxfile.exe", QString("%1emtxfile.exe").arg(targetPath));
	QFile vbcdEditF1(QString("%1vbcdedit.bat").arg(targetPath));
	vbcdEditF1.open(QIODevice::ReadWrite | QIODevice::Text);
	QTextStream vbcdEditS1(&vbcdEditF1);
	vbcdEditS1 << QString("bcdedit /create /d \""UNETBOOTINB"\" /application bootsector > %1tmpbcdid").arg(targetPath) << endl;
	vbcdEditF1.close();
	bool warch64;
	callexternapp(QString("%1vbcdedit.bat").arg(targetPath), "");
	QFile vbcdTmpInF(QString("%1tmpbcdid").arg(targetPath));
	vbcdTmpInF.open(QIODevice::ReadOnly | QIODevice::Text);
	QTextStream vbcdTmpInS(&vbcdTmpInF);
	QString qstmpvbcdin = vbcdTmpInS.readAll();
	vbcdTmpInF.close();
	QString vbcdIdTL;
	QStringList vbcdIdTLSL;
	if (qstmpvbcdin.contains("{") && qstmpvbcdin.contains("}") && qstmpvbcdin.contains("-"))
	{
		warch64 = false;
		vbcdIdTLSL = qstmpvbcdin.replace("{", "\n").replace("}", "\n").split("\n").filter("-");
		if (!vbcdIdTLSL.isEmpty())
			vbcdIdTL = vbcdIdTLSL.at(0);
	}
	else
	{
		warch64 = true;
		callexternapp(QString("%1emtxfile.exe").arg(targetPath), QString("%1vbcdedit.bat runas").arg(targetPath));
		vbcdTmpInF.open(QIODevice::ReadOnly | QIODevice::Text);
		QTextStream vbcdTmpInS2(&vbcdTmpInF);
		vbcdIdTLSL = vbcdTmpInS2.readAll().replace("{", "\n").replace("}", "\n").split("\n").filter("-");
		if (!vbcdIdTLSL.isEmpty())
			vbcdIdTL = vbcdIdTLSL.at(0);
		vbcdTmpInF.close();
	}
	QSettings vdtistor("HKEY_LOCAL_MACHINE\\Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\UNetbootin", QSettings::NativeFormat);
	vdtistor.setValue("WArch64", warch64);
	QFile vbcdEditF2(QString("%1vbcdedt2.bat").arg(targetPath));
	vbcdEditF2.open(QIODevice::ReadWrite | QIODevice::Text);
	QTextStream vbcdEditS2(&vbcdEditF2);
	vbcdEditS2 << QString("bcdedit /set {%1} device boot\n"
	"bcdedit /set {%1} path \\ubnldr.mbr\n"
	"bcdedit /displayorder {%1} /addlast\n"
	"bcdedit /timeout 30").arg(vbcdIdTL) << endl;
	vbcdEditF2.close();
	if (warch64)
	{
		callexternapp(QString("%1emtxfile.exe").arg(targetPath), QString("%1vbcdedt2.bat runas").arg(targetPath));
	}
	else
	{
		callexternapp(QString("%1vbcdedt2.bat").arg(targetPath), "");
	}
	QFile vbcdundoF(QString("%1vbcdundo.bat").arg(targetPath));
	vbcdundoF.open(QIODevice::ReadWrite | QIODevice::Text);
	QTextStream vbcdundoS(&vbcdundoF);
	vbcdundoS << QString("bcdedit /delete {%1}").arg(vbcdIdTL) << endl;
	vbcdundoF.close();
}

#endif

void unetbootin::instIndvfl(QString srcfName, QString dstfName)
{
	if (QFile::exists(dstfName))
	{
		if (!overwritefileprompt(dstfName))
			return;
	}
	QFile dstF(dstfName);
	dstF.open(QIODevice::WriteOnly);
	QFile srcF(QString(":/%1").arg(srcfName));
	#ifdef NOSTATIC
	if (srcfName == "memdisk")
                srcF.setFileName(QFile::exists("/usr/share/syslinux/memdisk") ? "/usr/share/syslinux/memdisk" : "/usr/lib/syslinux/memdisk");
	else if (srcfName == "vesamenu.c32")
                srcF.setFileName(QFile::exists("/usr/share/syslinux/vesamenu.c32") ? "/usr/share/syslinux/vesamenu.c32" : "/usr/lib/syslinux/vesamenu.c32");
	else if (srcfName == "mbr.bin")
                srcF.setFileName(QFile::exists("/usr/share/syslinux/mbr.bin") ? "/usr/share/syslinux/mbr.bin" : "/usr/lib/syslinux/mbr.bin");
	else if (srcfName == "ubnsylnx")
		srcF.setFileName("/usr/bin/syslinux");
//	else
//		srcF.setFileName(QString("/usr/lib/unetbootin/%1").arg(srcfName));
	#endif
	srcF.open(QIODevice::ReadOnly);
	dstF.write(srcF.readAll());
	dstF.close();
	srcF.close();
}

QString unetbootin::instTempfl(QString srcfName, QString dstfType)
{
	QString dstfName = randtmpfile::getrandfilename(ubntmpf, dstfType);
	instIndvfl(srcfName, dstfName);
	return dstfName;
}

void unetbootin::runinst()
{
	firstlayer->setEnabled(false);
	firstlayer->hide();
	secondlayer->setEnabled(true);
	secondlayer->show();
	rebootlayer->setEnabled(false);
	rebootlayer->hide();
	progresslayer->setEnabled(true);
	progresslayer->show();
	sdesc1->setText(QString("<b>%1 (Current)</b>").arg(sdesc1->text()));
	tprogress->setValue(0);
	installType = typeselect->currentText();
	targetDrive = driveselect->currentText();
	QString ginstallDir;
	QString installDir;
	#ifdef Q_OS_WIN32
	if (installType == tr("Hard Disk"))
	{
		ginstallDir = "unetbtin/";
	}
	if (installType == tr("USB Drive"))
	{
		ginstallDir = "";
	}
	installDir = ginstallDir;
	targetDev = QString("%1").arg(targetDrive).remove("\\");
	rawtargetDev = targetDev;
	#endif
	#ifdef Q_OS_UNIX
	if (installType == tr("Hard Disk"))
	{
		QString devnboot = locatedevicenode("/boot");
		if (devnboot == "NOT FOUND")
		{
			ginstallDir = "boot/";
			installDir = ginstallDir;
			targetDev = locatedevicenode("/");
		}
		else
		{
			ginstallDir = "";
			installDir = "boot/";
			targetDev = devnboot;
		}
	}
	if (installType == tr("USB Drive"))
	{
		targetDev = driveselect->currentText();
		ginstallDir = "";
		installDir = ginstallDir;
		targetDrive = QString("%1/").arg(locatemountpoint(targetDev));
	}
	rawtargetDev = QString(targetDev).remove(QRegExp("\\d$"));
	#endif
	devluid = getdevluid(targetDev);
	kernelLine = "kernel";
	kernelLoc = QString("/%1ubnkern").arg(ginstallDir);
	initrdLine = "initrd";
	slinitrdLine = "initrd=";
	initrdLoc = QString("/%1ubninit").arg(ginstallDir);
#ifdef NOINITRD
	initrdLoc = "";
	initrdOpts = "";
	initrdLine = "";
	slinitrdLine = "";
#endif
#ifdef NODEFAULTKERNEL
	kernelLoc = "";
#endif
	targetPath = QDir::toNativeSeparators(QString("%1%2").arg(targetDrive).arg(installDir));
	QDir dir;
	if (!dir.exists(targetPath))
	{
		dir.mkpath(targetPath);
	}
	if (QFile::exists(QString("%1ubnkern").arg(targetPath)))
	{
		overwritefileprompt(QString("%1ubnkern").arg(targetPath));
	}
	if (QFile::exists(QString("%1ubninit").arg(targetPath)))
	{
		overwritefileprompt(QString("%1ubninit").arg(targetPath));
	}
	if (radioFloppy->isChecked())
	{
		if (diskimagetypeselect->currentIndex() == diskimagetypeselect->findText(tr("Floppy")))
		{
			instIndvfl("memdisk", QString("%1ubnkern").arg(targetPath));
			QFile::copy(FloppyPath->text(), QString("%1ubninit").arg(targetPath));
   		}
		if (diskimagetypeselect->currentIndex() == diskimagetypeselect->findText(tr("ISO")))
		{
			extractiso(FloppyPath->text(), targetPath);
			if (QFile::exists(QString("%1sevnz.exe").arg(ubntmpf)))
			{
				QFile::remove(QString("%1sevnz.exe").arg(ubntmpf));
			}
			if (QFile::exists(QString("%1\\7z.dll").arg(ubntmpf)))
			{
				QFile::remove(QString("%1\\7z.dll").arg(ubntmpf));
			}
   		}
	}
	else if (radioManual->isChecked())
	{
		QFile::copy(KernelPath->text(), QString("%1ubnkern").arg(targetPath));
		QFile::copy(InitrdPath->text(), QString("%1ubninit").arg(targetPath));
		kernelOpts = OptionEnter->text();
	}
	else if (radioDistro->isChecked())
	{
		nameDistro = distroselect->currentText();
		nameVersion = dverselect->currentText();
		if (nameVersion.contains("_Live"))
		{
			nameVersion.remove("_Live");
			islivecd = true;
		}
		else
		{
			islivecd = false;
		}
		if (nameVersion.contains("_NetInstall"))
		{
			nameVersion.remove("_NetInstall");
			isnetinstall = true;
		}
		if (nameVersion.contains("_x64"))
		{
			nameVersion.remove("_x64");
			isarch64 = true;
	   	}
	   	else
	   	{
	   		isarch64 = false;
	  	}
	  	QString isotmpf = randtmpfile::getrandfilename(ubntmpf, "iso");
		QString cpuarch;
		QString relname = nameVersion.toLower();
		#include "customdistrolst.cpp"
		if (QFile::exists(QString("%1sevnz.exe").arg(ubntmpf)))
		{
			QFile::remove(QString("%1sevnz.exe").arg(ubntmpf));
		}
		if (QFile::exists(QString("%1\\7z.dll").arg(ubntmpf)))
		{
			QFile::remove(QString("%1\\7z.dll").arg(ubntmpf));
		}
	}
	if (!sdesc1->text().contains("(Done)"))
	{
		sdesc1->setText(QString(sdesc1->text()).remove("<b>").replace("(Current)</b>", "(Done)"));
	}
	if (sdesc2->text().contains("(Current)"))
	{
		sdesc2->setText(QString(sdesc2->text()).remove("<b>").replace("(Current)</b>", "(Done)"));
	}
	else
	{
		sdesc2->setText(QString("%1 (Done)").arg(sdesc2->text()));
	}
	sdesc3->setText(QString("<b>%1 (Current)</b>").arg(sdesc3->text()));
	tprogress->setValue(0);
	instDetType();
}

void unetbootin::instDetType()
{
	if (installType == tr("Hard Disk"))
	{
		runinsthdd();
   	}
   	if (installType == tr("USB Drive"))
	{
		runinstusb();
	}
}

void unetbootin::runinsthdd()
{
	#ifdef Q_OS_WIN32
	if (QFile::exists(QDir::toNativeSeparators(QString("%1unetbtin.exe").arg(targetDrive))))
	{
		QFile::remove(QDir::toNativeSeparators(QString("%1unetbtin.exe").arg(targetDrive)));
	}
	QFile::copy(appLoc, QDir::toNativeSeparators(QString("%1unetbtin.exe").arg(targetDrive)));
	QFile::setPermissions(QDir::toNativeSeparators(QString("%1unetbtin.exe").arg(targetDrive)), QFile::ReadOther|QFile::WriteOther|QFile::ExeOther);
	if (QFile::exists(QDir::toNativeSeparators(QString("%1ubnldr").arg(targetDrive))))
	{
		overwritefileprompt(QDir::toNativeSeparators(QString("%1ubnldr").arg(targetDrive)));
	}
	instIndvfl("ubnldr", QString("%1ubnldr").arg(targetDrive));
	if (QFile::exists(QDir::toNativeSeparators(QString("%1ubnldr.mbr").arg(targetDrive))))
	{
		overwritefileprompt(QDir::toNativeSeparators(QString("%1ubnldr.mbr").arg(targetDrive)));
	}
	instIndvfl("ubnldr.mbr", QString("%1ubnldr.mbr").arg(targetDrive));
	if (QFile::exists(QDir::toNativeSeparators(QString("%1ubnldr.exe").arg(targetDrive))))
	{
		overwritefileprompt(QDir::toNativeSeparators(QString("%1ubnldr.exe").arg(targetDrive)));
	}
	instIndvfl("ubnldr.exe", QString("%1ubnldr.exe").arg(targetDrive));
	#endif
   	QFile menulst;
   	#ifdef Q_OS_WIN32
   	menulst.setFileName(QString("%1menu.lst").arg(targetPath));
   	#endif
   	#ifdef Q_OS_UNIX
   	menulst.setFileName("/boot/grub/menu.lst");
   	if (QFile::exists(QString("%1.bak").arg(menulst.fileName())))
   		QFile::remove(QString("%1.bak").arg(menulst.fileName()));
   	QFile::copy(menulst.fileName(), QString("%1.bak").arg(menulst.fileName()));
   	QFile bkmenulst(QString("%1.bak").arg(menulst.fileName()));
   	bkmenulst.open(QIODevice::ReadOnly | QIODevice::Text);
   	QTextStream bkmenulstout(&bkmenulst);
   	#endif
	menulst.open(QIODevice::WriteOnly | QIODevice::Text);
	QTextStream menulstout(&menulst);
	#ifdef Q_OS_UNIX
	QRegExp mlstchkistimeout("\\s{0,}timeout\\s{1,}\\d{1,}.{0,}", Qt::CaseInsensitive);
	QRegExp mlstchkishiddenmenu("\\s{0,}hiddenmenu.{0,}", Qt::CaseInsensitive);
	QStringList ecurmenulstTextL;
	bool mlstmustreplacetimeout = true;
	bool mlstmustreplacehiddenmenu = true;
	QString menulstCurLine;
	while (!bkmenulstout.atEnd())
	{
		menulstCurLine = bkmenulstout.readLine();
		if (mlstmustreplacetimeout && mlstchkishiddenmenu.exactMatch(menulstCurLine))
		{
			ecurmenulstTextL.append("#hiddenmenu");
			mlstmustreplacetimeout = false;
		}
		else if (mlstmustreplacehiddenmenu && mlstchkistimeout.exactMatch(menulstCurLine))
		{
			ecurmenulstTextL.append("timeout\t\t15");
			mlstmustreplacehiddenmenu = false;
		}
		else
		{
			ecurmenulstTextL.append(menulstCurLine);
		}
	}
	QString ecurmenulstText = ecurmenulstTextL.join("\n");
	#endif
	QString menulstxt = QString(
	#ifdef Q_OS_UNIX
	"%9\n\n"
	#endif
	#ifdef Q_OS_WIN32
	"default 0\n"
	"timeout 10\n"
	#endif
#ifndef NODEFAULTBOOT
	"\ntitle "UNETBOOTINB"\n"
	#ifdef Q_OS_WIN32
	"find --set-root %3\n"
	#endif
	#ifdef Q_OS_UNIX
	"root %8\n"
	#endif
	"%1 %2 %3 %4\n"
	"%5 %6 %7\n"
	"boot\n"
#endif
	).arg(kernelLine).arg(kernelParam).arg(kernelLoc).arg(kernelOpts).arg(initrdLine).arg(initrdLoc).arg(initrdOpts)

	#ifdef Q_OS_UNIX
	.arg(getGrubNotation(targetDev)).arg(ecurmenulstText)
	#endif
	;
	if (!extraoptionsPL.first.first.isEmpty())
	{
		for (int i = 0; i < extraoptionsPL.first.first.size(); ++i)
		{
			menulstxt.append(QString("\ntitle %1\n"
			#ifdef Q_OS_WIN32
			"find --set-root %2\n"
			#endif
			#ifdef Q_OS_UNIX
			"root %5\n"
			#endif
			"kernel %2 %4\n"
			"initrd %3\n"
			"boot\n").arg(QString(extraoptionsPL.second.first.at(i)).remove("^")).arg(extraoptionsPL.first.first.at(i)).arg(extraoptionsPL.first.second.at(i)).arg(extraoptionsPL.second.second.at(i))
			#ifdef Q_OS_UNIX
			.arg(getGrubNotation(targetDev))
			#endif
			);
		}
	}
	menulstout << menulstxt << endl;
	menulst.close();
	#ifdef Q_OS_WIN32
   	QSettings install("HKEY_LOCAL_MACHINE\\Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\UNetbootin", QSettings::NativeFormat);
   	install.setValue("Location", targetDrive);
   	install.setValue("DisplayName", "UNetbootin");
   	install.setValue("UninstallString", QDir::toNativeSeparators(QString("%1unetbtin.exe").arg(targetDrive)));
   	QSettings runonce("HKEY_LOCAL_MACHINE\\Software\\Microsoft\\Windows\\CurrentVersion\\RunOnce", QSettings::NativeFormat);
   	runonce.setValue("UNetbootin Uninstaller", QDir::toNativeSeparators(QString("%1unetbtin.exe").arg(targetDrive)));
	if (QSysInfo::WindowsVersion == QSysInfo::WV_32s || QSysInfo::WindowsVersion == QSysInfo::WV_95 || QSysInfo::WindowsVersion == QSysInfo::WV_98 || QSysInfo::WindowsVersion == QSysInfo::WV_Me)
	{
		configsysEdit();
	}
	else if (QSysInfo::WindowsVersion == QSysInfo::WV_NT || QSysInfo::WindowsVersion == QSysInfo::WV_2000 || QSysInfo::WindowsVersion == QSysInfo::WV_XP || QSysInfo::WindowsVersion == QSysInfo::WV_2003 )
	{
		bootiniEdit();
	}
	else if (QSysInfo::WindowsVersion == QSysInfo::WV_VISTA)
	{
		vistabcdEdit();
	}
	else
	{
		configsysEdit();
		bootiniEdit();
		vistabcdEdit();
	}
	#endif
	#ifdef Q_OS_UNIX
	QSettings install(QSettings::SystemScope, "UNetbootin");
	install.setValue("Location", "/");
	#endif
	fininstall();
  	}

void unetbootin::runinstusb()
{
	#ifdef Q_OS_WIN32
	QString sysltfloc = instTempfl("syslinux.exe", "exe");
	callexternapp(sysltfloc, QString("-ma %1").arg(targetDev));
	QFile::remove(sysltfloc);
	#endif
	#ifdef STATICLINUX
	if (QFile::exists(syslinuxcommand))
		QFile::remove(syslinuxcommand);
	instIndvfl("ubnsylnx", syslinuxcommand);
        QFile::setPermissions(syslinuxcommand, QFile::ReadOwner|QFile::ExeOwner|QFile::ReadGroup|QFile::ExeGroup|QFile::ReadOther|QFile::ExeOther|QFile::WriteOwner);
//	chmod(syslinuxcommand, S_IRUSR|S_IRGRP|S_IROTH|S_IRWXU);
        if (QFile::exists(extlinuxcommand))
                QFile::remove(extlinuxcommand);
        instIndvfl("ubnexlnx", extlinuxcommand);
        QFile::setPermissions(extlinuxcommand, QFile::ReadOwner|QFile::ExeOwner|QFile::ReadGroup|QFile::ExeGroup|QFile::ReadOther|QFile::ExeOther|QFile::WriteOwner);
	#endif
	#ifdef Q_OS_UNIX
        if (callexternapp(volidcommand, QString("-t %2").arg(targetDev)).contains(QRegExp("(ext2|ext3)")))
        {
            isext2 = true;
            callexternapp(extlinuxcommand, QString("-i %1").arg(targetPath));
        }
        else
        {
            isext2 = false;
            callexternapp(syslinuxcommand, targetDev);
        }
	if (rawtargetDev != targetDev)
	{
		callexternapp(sfdiskcommand, QString("%1 -A%2").arg(rawtargetDev, QString(targetDev).remove(rawtargetDev)));
		QFile usbmbrF(rawtargetDev);
		QFile mbrbinF(":/mbr.bin");
		#ifdef NOSTATIC
		mbrbinF.setFileName(QFile::exists("/usr/share/syslinux/mbr.bin") ? "/usr/share/syslinux/mbr.bin" : "/usr/lib/syslinux/mbr.bin");
		#endif
		usbmbrF.open(QIODevice::WriteOnly);
		mbrbinF.open(QIODevice::ReadOnly);
		usbmbrF.write(mbrbinF.readAll());
		mbrbinF.close();
		usbmbrF.close();
	}
	#endif
	if (QFile::exists(QString("%1syslinux.cfg").arg(targetPath)))
	{
		overwritefileprompt(QString("%1syslinux.cfg").arg(targetPath));
	}
	QFile syslinuxcfg(QString("%1syslinux.cfg").arg(targetPath));
   	syslinuxcfg.open(QIODevice::WriteOnly | QIODevice::Text);
	QTextStream syslinuxcfgout(&syslinuxcfg);
	QString syslinuxcfgtxt = QString("default vesamenu.c32\n"
	"prompt 0\n"
	"menu title UNetbootin\n"
	"timeout 100\n\n"
#ifndef NODEFAULTBOOT
	"label unetbootindefault\n"
	"menu label Default\n"
	"kernel %1\n"
	"append %4%2 %3\n"
#endif
	).arg(kernelLoc, initrdLoc, kernelOpts, slinitrdLine);

	if (!extraoptionsPL.first.first.isEmpty())
	{
		for (int i = 0; i < extraoptionsPL.first.first.size(); ++i)
		{
			syslinuxcfgtxt.append(QString("\nlabel %5\n"
			"menu label %1\n"
			"kernel %2\n"
			"append %6%3 %4\n").arg(extraoptionsPL.second.first.at(i)).arg(extraoptionsPL.first.first.at(i)).arg(extraoptionsPL.first.second.at(i)).arg(extraoptionsPL.second.second.at(i)).arg(QString("ubnentry%1").arg(i)).arg(slinitrdLine));
		}
	}
	syslinuxcfgout << syslinuxcfgtxt << endl;
        syslinuxcfg.close();
        #ifdef Q_OS_UNIX
        if (isext2)
            QFile::copy(QString("%1syslinux.cfg").arg(targetPath), QString("%1extlinux.conf").arg(targetPath));
        #endif
        instIndvfl("vesamenu.c32", QString("%1vesamenu.c32").arg(targetPath));
	fininstall();
}

void unetbootin::fininstall()
{
	progresslayer->setEnabled(false);
	progresslayer->hide();
	rebootlayer->setEnabled(true);
	rebootlayer->show();
	sdesc3->setText(QString(sdesc3->text()).remove("<b>").replace("(Current)</b>", "(Done)"));
	sdesc4->setText(QString("<b>%1 (Current)</b>").arg(sdesc4->text()));
	if (installType == tr("Hard Disk"))
	{
		rebootmsgtext->setText(tr("After rebooting, select the "UNETBOOTINB" menu entry to boot.%1\nReboot now?").arg(postinstmsg));
	}
	if (installType == tr("USB Drive"))
	{
		rebootmsgtext->setText(tr("After rebooting, select the USB boot option in the BIOS boot menu.%1\nReboot now?").arg(postinstmsg));
	}
}
