#include "unetbootin.h"

unetbootin::unetbootin(QWidget *parent)
	: QWidget(parent)
{
	setupUi(this);
	#ifdef AUTOSUPERGRUBDISK
	diskimagetypeselect->removeItem(diskimagetypeselect->findText("ISO"));
	QFile asgdDescF(":/asgd-en.htm");
	asgdDescF.open(QIODevice::ReadOnly | QIODevice::Text);
	QTextStream asgdDescS(&asgdDescF);
	distroselect->addItem("Auto Super Grub Disk", (QStringList() << "1.0" << 
	asgdDescS.readAll() << 
//	tr("<b>Homepage:</b> <a href=\"http://www.supergrubdisk.org/\">http://www.supergrubdisk.org</a><br/>"
//		"<b>Description:</b> Super Grub Disk allows you to repair your GRUB install and can also perform other bootloader-related tasks.<br/>"
//		"<b>Install Notes:</b> This version loads the \"Auto Super Grub Disk\", which performs the GRUB MBR installation automatically.") << 
	"1.0"));
	#endif
	#ifndef AUTOSUPERGRUBDISK
	distroselect->addItem("== Select Distribution ==", (QStringList() << "== Select Version ==" << 
	tr("Welcome to <a href=\"http://unetbootin.sourceforge.net/\">UNetbootin</a>, the Universal Netboot Installer. Usage:"
		"<ol><li>Select a distribution and version to download from the list above, or manually specify files to load below.</li>"
		"<li>Select an installation type, and press OK to begin installing.</li></ol>") << 
	"== Select Version =="));
	distroselect->addItem("Arch Linux", (QStringList() << "2007.08-2" << 
	tr("<b>Homepage:</b> <a href=\"http://wwww.archlinux.org/\">http://wwww.archlinux.org</a><br/>"
		"<b>Description:</b> Arch Linux is a lightweight distribution optimized for speed and flexibility.<br/>"
		"<b>Install Notes:</b> The default version allows for installation over the internet (FTP).") << 
	"2007.08-2" << "2007.08-2_x64"));
	distroselect->addItem("CentOS", (QStringList() << "5" << 
	tr("<b>Homepage:</b> <a href=\"http://wwww.centos.org/\">http://wwww.centos.org</a><br/>"
		"<b>Description:</b> CentOS is a free Red Hat Enterprise Linux clone.<br/>"
		"<b>Install Notes:</b> The default version allows for both installation over the internet (FTP), or offline installation using pre-downloaded installation ISO files. You may need to pre-partition your disk using Parted Magic beforehand.") << 
	"4" << "4_x64" << "5" << "5_x64"));
	distroselect->addItem("Damn Small Linux", (QStringList() << "4.2.5" << 
	tr("<b>Homepage:</b> <a href=\"http://damnsmalllinux.org/\">http://damnsmalllinux.org</a><br/>"
		"<b>Description:</b> Damn Small Linux is a minimalist distribution with a total size of 50 MB.<br/>"
		"<b>Install Notes:</b> The default version loads the entire system into RAM and boots from memory, so installation is not required.") << 
	"4.2.5"));
	distroselect->addItem("Debian", (QStringList() << "Stable" << 
	tr("<b>Homepage:</b> <a href=\"http://www.debian.org/\">http://www.debian.org</a><br/>"
		"<b>Description:</b> Debian is a community-developed Linux distribution that supports a wide variety of architectures and offers a large repository of packages.<br/>"
		"<b>Install Notes:</b> The default version allows for installation over FTP. Pre-partitioning may be helpful but is not required.") << 
	"Stable" << "Stable_x64" << "Testing" << "Testing_x64" << "Unstable" << "Unstable_x64"));
	distroselect->addItem("Fedora", (QStringList() << "8" << 
	tr("<b>Homepage:</b> <a href=\"http://fedoraproject.org/\">http://fedoraproject.org</a><br/>"
		"<b>Description:</b> Fedora is a Red Hat sponsored community distribution which showcases the latest cutting-edge free/open-source software.<br/>"
		"<b>Install Notes:</b> The default version allows for both installation over the internet (FTP), or offline installation using pre-downloaded installation ISO files. The Live version allows for booting in Live mode. If installing from Live mode, you will need to pre-partition your hard drive using Parted Magic beforehand.") << 
	"7" << "7_x64" << "8" << "8_x64" << "8_Live" << "8_Live_x64" << "9 Beta" << "9 Beta_x64" << "Rawhide" << "Rawhide_x64"));
	distroselect->addItem("FreeBSD", (QStringList() << "7.0" << 
	tr("<b>Homepage:</b> <a href=\"http://www.freebsd.org/\">http://www.freebsd.org</a><br/>"
		"<b>Description:</b> FreeBSD is a general-purpose Unix-like operating system designed for scalability and performance.<br/>"
		"<b>Install Notes:</b> The default version allows for both installation over the internet (FTP), or offline installation using pre-downloaded installation ISO files. You may need to pre-partition your disk using Parted Magic beforehand.") << 
	"6.3" << "6.3_x64" << "7.0" << "7.0_x64"));
	distroselect->addItem("Frugalware", (QStringList() << "Stable" << 
	tr("<b>Homepage:</b> <a href=\"http://frugalware.org/\">http://frugalware.org</a><br/>"
		"<b>Description:</b> Frugalware is a general-purpose Slackware-based distro for advanced users.<br/>"
		"<b>Install Notes:</b> The default option allows for both installation over the internet (FTP), or offline installation using pre-downloaded installation ISO files. You may need to pre-partition your disk using Parted Magic beforehand.") << 
	"Stable" << "Stable_x64" << "Testing" << "Testing_x64" << "Current" << "Current_x64"));
	distroselect->addItem("Linux Mint", (QStringList() << "4.0" << 
	tr("<b>Homepage:</b> <a href=\"http://linuxmint.com/\">http://linuxmint.com/</a><br/>"
		"<b>Description:</b> Linux Mint is a user-friendly Ubuntu-based distribution which includes additional proprietary codecs and other software by default.<br/>"
		"<b>Install Notes:</b> The default version allows for booting in Live mode. If installing from Live mode, you will need to pre-partition your disk using PartedMagic beforehand.") << 
	"3.1" << "4.0"));
	distroselect->addItem("Mandriva", (QStringList() << "2008.1_Live" << 
	tr("<b>Homepage:</b> <a href=\"http://www.mandriva.com/\">http://www.mandriva.com/</a><br/>"
		"<b>Description:</b> Mandriva is a user-friendly distro formerly known as Mandrake Linux.<br/>"
		"<b>Install Notes:</b> The default option allows for installation only via pre-downloaded <a href=\"http://www.mandriva.com/en/download\">\"Free\" iso image files</a>. The Live version allows for booting in Live mode. If installing from Live mode, you will need to pre-partition your hard drive using Parted Magic beforehand.") << 
	"2007.1" << "2007.1_x64" << "2008.0" << "2008.0_x64" << "2008.0_Live" << "2008.1" << "2008.1_x64" << "2008.1_Live"));
	distroselect->addItem("NetBSD", (QStringList() << "4.0" << 
	tr("<b>Homepage:</b> <a href=\"http://www.netbsd.org/\">http://www.netbsd.org</a><br/>"
		"<b>Description:</b> NetBSD is a Unix-like operating system which focuses on portability.<br/>"
		"<b>Install Notes:</b>The default version allows for both installation over the internet (FTP), or using pre-downloaded installation ISO files. You may need to pre-partition your disk using Parted Magic beforehand") << 
	"4.0"));
	distroselect->addItem("openSUSE", (QStringList() << "10.3" << 
	tr("<b>Homepage:</b> <a href=\"http://www.opensuse.org/\">http://www.opensuse.org</a><br/>"
		"<b>Description:</b> openSUSE is a user-friendly Novell sponsored distribution.<br/>"
		"<b>Install Notes:</b> The default version allows for both installation over the internet (FTP), or offline installation using pre-downloaded installation ISO files. The Live version allows for booting in Live mode. If installing from Live mode, you will need to pre-partition your hard drive using Parted Magic beforehand.") << 
	"10.2" << "10.2_x64" << "10.3" << "10.3_x64" << "Factory" << "Factory_x64"));
	distroselect->addItem("Parted Magic", (QStringList() << "2.1" << 
	tr("<b>Homepage:</b> <a href=\"http://partedmagic.com/\">http://partedmagic.com</a><br/>"
		"<b>Description:</b> Parted Magic includes the GParted partition manager and other system utilities which can resize, copy, backup, and manipulate disk partitions.<br/>"
		"<b>Install Notes:</b> Parted Magic is booted and run in live mode; no installation is required to use it.") << 
	"2.1"));
	distroselect->addItem("PCLinuxOS", (QStringList() << "2007" << 
	tr("<b>Homepage:</b> <a href=\"http://www.pclinuxos.com/\">http://www.pclinuxos.com</a><br/>"
		"<b>Description:</b> PCLinuxOS is a user-friendly Mandriva-based distribution.<br/>"
		"<b>Install Notes:</b> The default version allows for booting in Live mode. If installing from Live mode, you will need to pre-partition your disk using PartedMagic beforehand.") << 
	"2007" << "2008 Gnome" << "2008 Minime"));
	distroselect->addItem("Ubuntu", (QStringList() << "7.10" << 
	tr("<b>Homepage:</b> <a href=\"http://www.ubuntu.com/\">http://www.ubuntu.com</a><br/>"
		"<b>Description:</b> Ubuntu is a user-friendly Debian-based distribution. It is currently the most popular Linux desktop distribution.<br/>"
		"<b>Install Notes:</b> Kubuntu and other official Ubuntu derivatives can be installed as well. The default version allows for installation over FTP. The Live version allows for booting in Live mode. If installing from Live mode, you will need to pre-partition your hard drive using Parted Magic beforehand.") << 
	"6.06" << "6.06_x64" << "6.10" << "6.10_x64" << "6.10_Live" << "6.10_Live_x64" << "7.04" << "7.04_x64" << "7.04_Live" << "7.04_Live_x64" << "7.10" << "7.10_x64" << "7.10_Live" << "7.10_Live_x64" << "8.04" << "8.04_x64"));
	#endif
	driveselect->addItem(QDir::toNativeSeparators(QDir::rootPath()).toUpper());
	#ifdef Q_OS_UNIX
	fdiskcommand = locatecommand("fdisk", "either", "util-linux");
	sfdiskcommand = locatecommand("sfdisk", "either", "util-linux");
	mssyscommand = locatecommand("ms-sys", "USB Drive", "ms-sys");
	syslinuxcommand = locatecommand("syslinux", "USB Drive", "syslinux");
	sevzcommand = locatecommand("7z", "either", "p7zip-full");
	ubntmpf = "/tmp/";
	#endif
	#ifdef Q_OS_WIN32
	ubntmpf = QDir::toNativeSeparators(QString("%1/").arg(QDir::tempPath()));
	#endif
}

void unetbootin::on_distroselect_currentIndexChanged(int distroselectIndex)
{
	dverselect->clear();
	QStringList dverL = distroselect->itemData(distroselectIndex).value<QStringList>();
	for (int i = 2; i < dverL.size(); ++i)
	{
		dverselect->addItem(dverL.at(i));
	}
	dverselect->setCurrentIndex(dverselect->findText(dverL.at(0)));
	intromessage->setText(dverL.at(1));
	radioDistro->setChecked(true);
}

void unetbootin::on_typeselect_currentIndexChanged(int typeselectIndex)
{
	if (typeselectIndex == typeselect->findText("Hard Disk"))
	{
		driveselect->clear();
		driveselect->addItem(QDir::toNativeSeparators(QDir::rootPath()).toUpper());
	}
	if (typeselectIndex == typeselect->findText("USB Drive"))
	{
		driveselect->clear();
		#ifdef Q_OS_WIN32
		QFileInfoList extdrivesList = QDir::drives();
		for (int i = 0; i < extdrivesList.size(); ++i)
		{
			if (QDir::toNativeSeparators(extdrivesList.at(i).path().toUpper()) != QDir::toNativeSeparators(QDir::rootPath().toUpper()))
			{
				driveselect->addItem(QDir::toNativeSeparators(extdrivesList.at(i).path().toUpper()));
			}
		}
		#endif
		#ifdef Q_OS_UNIX
		QProcess fdisklusbdevs;
		fdisklusbdevs.start(QString("%1 -l").arg(fdiskcommand));
		fdisklusbdevs.waitForFinished(-1);
		QStringList usbdevsL = QString(fdisklusbdevs.readAll()).split(" ").join("\n").split("\t").join("\n").split("\n").filter("/dev/");
		for (int i = 0; i < usbdevsL.size(); ++i)
		{
			if (!usbdevsL.at(i).contains(":"))
			{
				driveselect->addItem(usbdevsL.at(i));
			}
		}
		#endif
	}
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
	QString nameFloppy = QDir::toNativeSeparators(QFileDialog::getOpenFileName(this, "Open File", QDir::homePath()));
	if (QFileInfo(nameFloppy).completeSuffix().contains("iso"))
	{
		diskimagetypeselect->setCurrentIndex(diskimagetypeselect->findText("ISO"));
	}
	if (QFileInfo(nameFloppy).completeSuffix().contains("img"))
	{
		diskimagetypeselect->setCurrentIndex(diskimagetypeselect->findText("Floppy"));
	}
	FloppyPath->clear();
	FloppyPath->insert(nameFloppy);
	radioFloppy->setChecked(true);
}

void unetbootin::on_KernelFileSelector_clicked()
{
	QString nameKernel = QDir::toNativeSeparators(QFileDialog::getOpenFileName(this, "Open File", QDir::homePath()));
	KernelPath->clear();
	KernelPath->insert(nameKernel);
	radioManual->setChecked(true);
}

void unetbootin::on_InitrdFileSelector_clicked()
{
	QString nameInitrd = QDir::toNativeSeparators(QFileDialog::getOpenFileName(this, "Open File", QDir::homePath()));
	InitrdPath->clear();
	InitrdPath->insert(nameInitrd);
	radioManual->setChecked(true);
}

void unetbootin::on_CfgFileSelector_clicked()
{
	QString nameCfg = QFileDialog::getOpenFileName(this, "Open File", QDir::homePath());
	OptionEnter->clear();
	OptionEnter->insert(getcfgkernargs(nameCfg));
	radioManual->setChecked(true);
}

void unetbootin::on_cancelbutton_clicked()
{
	close();
}

void unetbootin::on_okbutton_clicked()
{
	if (radioDistro->isChecked() && distroselect->currentIndex() == distroselect->findText("== Select Distribution =="))
	{
		QMessageBox notenoughinputmsgb;
		notenoughinputmsgb.setIcon(QMessageBox::Information);
		notenoughinputmsgb.setWindowTitle(QObject::tr("Select a distro"));
		notenoughinputmsgb.setText(QObject::tr("You must select a distribution to load."));
 		notenoughinputmsgb.setStandardButtons(QMessageBox::Ok);
 		switch (notenoughinputmsgb.exec())
 		{
 			case QMessageBox::Ok:
				break;
	 		default:
				break;
 		}
	}
	else if (radioFloppy->isChecked() && FloppyPath->text() == "")
	{
		QMessageBox notenoughinputmsgb;
		notenoughinputmsgb.setIcon(QMessageBox::Information);
		notenoughinputmsgb.setWindowTitle(QObject::tr("Select a disk image file"));
		notenoughinputmsgb.setText(QObject::tr("You must select a disk image file to load."));
 		notenoughinputmsgb.setStandardButtons(QMessageBox::Ok);
 		switch (notenoughinputmsgb.exec())
 		{
 			case QMessageBox::Ok:
				break;
	 		default:
				break;
 		}
	}
	else if (radioManual->isChecked() && KernelPath->text() == "")
	{
		QMessageBox notenoughinputmsgb;
		notenoughinputmsgb.setIcon(QMessageBox::Information);
		notenoughinputmsgb.setWindowTitle(QObject::tr("Select a kernel and/or initrd file"));
		notenoughinputmsgb.setText(QObject::tr("You must select a kernel and/or initrd file to load."));
 		notenoughinputmsgb.setStandardButtons(QMessageBox::Ok);
 		switch (notenoughinputmsgb.exec())
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

QPair<QStringList, QStringList> unetbootin::listarchiveconts(QString archivefile)
{
	#ifdef Q_OS_WIN32
	if (sevzcommand == "")
	{
		installsvzip();
	}
	callexternapp(getenv("COMSPEC"), QString("/c \"\"%1\" -bd -slt l \"%2\" > \"%3\"\"").arg(sevzcommand).arg(archivefile).arg(QString("%1ubntmpls.txt").arg(ubntmpf)));
	QFile tmplsF(QString("%1ubntmpls.txt").arg(ubntmpf));
	tmplsF.open(QIODevice::ReadOnly | QIODevice::Text);
	QTextStream tmplsS(&tmplsF);
	#endif
	#ifdef Q_OS_UNIX
	QProcess sevzlcommand;
	sevzlcommand.start(QString("%1 -bd -slt l %2").arg(sevzcommand, archivefile));
	sevzlcommand.waitForFinished(-1);
	QByteArray sevzlcommandout = sevzlcommand.readAll();
	QTextStream tmplsS(&sevzlcommandout);
	#endif
	QString tmplsL;
	QStringList tmplsSLF;
	QStringList tmplsSLD;
	while (!tmplsS.atEnd())
	{
		tmplsL = tmplsS.readLine();
		if (tmplsL.contains("Path = "))
		{
			QString tmplsN = tmplsS.readLine();
			if (tmplsN.contains("Folder = 1") || tmplsN.contains("Folder = +"))
			{
				tmplsSLD.append(tmplsL.remove("Path = "));
			}
			else
			{
				tmplsSLF.append(tmplsL.remove("Path = "));
			}
		}
	}
	#ifdef Q_OS_WIN32
	tmplsF.close();
	QFile::remove(QString("%1ubntmpls.txt").arg(ubntmpf));
	#endif
	return qMakePair(tmplsSLF, tmplsSLD);
}

bool unetbootin::extractfile(QString filepath, QString destinfile, QString archivefile)
{
	QString destindir = QFileInfo(destinfile).dir().absolutePath();
	QString destinfilename = QString("%1/%2").arg(destindir).arg(QFileInfo(destinfile).fileName());
	QString filepathfilename = QString("%1/%2").arg(destindir).arg(QFileInfo(filepath).fileName());
	#ifdef Q_OS_WIN32
	if (sevzcommand == "")
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

bool unetbootin::extractkernel(QString archivefile, QString kernoutputfile, QStringList archivefileconts)
{
	QStringList kernelnames = QStringList() << "vmlinuz" << "vmlinux" << "bzImage" << "kernel" << "linux";
	for (int i = 0; i < kernelnames.size(); ++i)
	{
		if (!archivefileconts.filter(kernelnames.at(i)).isEmpty())
		{
			return extractfile(archivefileconts.filter(kernelnames.at(i)).at(0), kernoutputfile, archivefile);
		}
	}
	return false;
}

bool unetbootin::extractinitrd(QString archivefile, QString kernoutputfile, QStringList archivefileconts)
{
	QStringList kernelnames = QStringList() << "initrd.img.gz" << "initrd.igz" << "initrd.gz" << "initrd.img" << "initrd";
	for (int i = 0; i < kernelnames.size(); ++i)
	{
		if (!archivefileconts.filter(kernelnames.at(i)).isEmpty())
		{
			return extractfile(archivefileconts.filter(kernelnames.at(i)).at(0), kernoutputfile, archivefile);
		}
	}
	return false;
}

QString unetbootin::extractcfg(QString archivefile, QStringList archivefileconts)
{
	QStringList kernelnames = QStringList() << "syslinux.cfg" << "isolinux.cfg";
	for (int i = 0; i < kernelnames.size(); ++i)
	{
		if (!archivefileconts.filter(kernelnames.at(i)).isEmpty())
		{
			extractfile(archivefileconts.filter(kernelnames.at(i)).at(0), QString("%1ubnctemp.cfg").arg(ubntmpf), archivefile);
			break;
		}
	}
	QString excfg = getcfgkernargs(QString("%1ubnctemp.cfg").arg(ubntmpf));
	QFile::remove(QString("%1ubnctemp.cfg").arg(ubntmpf));
	return excfg;
}

void unetbootin::extractiso(QString isofile, QString exoutputdir)
{
	QPair<QStringList, QStringList> listfiledirpair = listarchiveconts(isofile);
	kernelOpts = extractcfg(isofile, listfiledirpair.first);
	extractkernel(isofile, QString("%1ubnkern").arg(exoutputdir), listfiledirpair.first);
	extractinitrd(isofile, QString("%1ubninit").arg(exoutputdir), listfiledirpair.first);
	QStringList createdpaths = makepathtree(targetDrive, listfiledirpair.second);
	QFile ubnpathlF(QDir::toNativeSeparators(QString("%1ubnpathl.txt").arg(exoutputdir)));
	ubnpathlF.open(QIODevice::WriteOnly | QIODevice::Text);
	QTextStream ubnpathlS(&ubnpathlF);
	for (int i = createdpaths.size() - 1; i > -1; i--)
	{
		ubnpathlS << createdpaths.at(i) << endl;
	}
	ubnpathlF.close();
	QStringList extractedfiles = extractallfiles(isofile, targetDrive, listfiledirpair.first);
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

QStringList unetbootin::extractallfiles(QString archivefile, QString dirxfilesto, QStringList filelist)
{
	QStringList extractedfiles;
	QProgressDialog xprogress;
	xprogress.setWindowTitle(QObject::tr("Extracting..."));
	xprogress.setMaximum(filelist.size());
	xprogress.setMinimum(0);
	xprogress.setValue(0);
	for (int i = 0; i < filelist.size(); ++i)
	{
		xprogress.setLabelText(QObject::tr("Extracting files, please wait...\nArchive: %1\nSource: %2\nDestination: %3\nExtracted: %4 of %5 files").arg(archivefile).arg(filelist.at(i)).arg(QString("%1%2").arg(dirxfilesto).arg(filelist.at(i))).arg(i).arg(filelist.size()));
		if (extractfile(filelist.at(i), QString("%1%2").arg(dirxfilesto).arg(filelist.at(i)), archivefile))
		{
			extractedfiles.append(filelist.at(i));
		}
		xprogress.setValue(i);
	}
	xprogress.close();
	return extractedfiles;
}

QString unetbootin::getcfgkernargs(QString cfgfile)
{
	QFile cfgfileF(cfgfile);
	cfgfileF.open(QIODevice::ReadOnly | QIODevice::Text);
	QTextStream cfgfileS(&cfgfileF);
	QString cfgfileCL;
	while (!cfgfileS.atEnd())
	{
		cfgfileCL = cfgfileS.readLine();
		if (cfgfileCL.contains("append", Qt::CaseInsensitive))
		{
			break;
		}
	}
	return cfgfileCL.remove(QRegExp("\\s{0,}append\\s{0,}", Qt::CaseInsensitive)).remove(QRegExp("\\s{0,1}initrd=\\S{0,}", Qt::CaseInsensitive)).replace("rootfstype=iso9660", "rootfstype=auto").replace(QRegExp("root=CDLABEL=\\S{0,}"), QString("root=UUID=%1").arg(devuuid));
}

void unetbootin::downloadfile(QString fileurl, QString targetfile)
{
	if (QFile::exists(targetfile))
	{
		QFile::remove(targetfile);
	}
	sourcefile = fileurl;
	destinfile = targetfile;
	QUrl dlurl(fileurl);
	bool isftp = false;
	if (dlurl.scheme() == "ftp")
	{
		isftp = true;
	}
	QHttp dlhttp;
	QFtp dlftp;
	dlprogress.setWindowTitle(QObject::tr("Downloading..."));
	dlprogress.setLabelText(QObject::tr("Downloading files, please wait...\nSource: %1\nDestination: %2\nDownloaded: 0 bytes").arg(fileurl).arg(targetfile));
	if (isftp)
	{
		connect(&dlftp, SIGNAL(done(bool)), &dlprogress, SLOT(close()));
		connect(&dlftp, SIGNAL(dataTransferProgress(qint64, qint64)), this, SLOT(dlprogressupdate64(qint64, qint64)));
	}
	else
	{
		connect(&dlhttp, SIGNAL(done(bool)), &dlprogress, SLOT(close()));
		connect(&dlhttp, SIGNAL(dataReadProgress(int, int)), this, SLOT(dlprogressupdate(int, int)));
	}
	QFile dloutfile;
	if (installType == "USB Drive")
	{
		if (QFile::exists(QString("%1ubndlout.tmp").arg(ubntmpf)))
		{
			QFile::remove(QString("%1ubndlout.tmp").arg(ubntmpf));
		}
		dloutfile.setFileName(QString("%1ubndlout.tmp").arg(ubntmpf));
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
		dlftp.get(dlurl.path(), &dloutfile);
	}
	else
	{
		dlhttp.setHost(dlurl.host());
		dlhttp.get(dlurl.path(), &dloutfile);
	}
	dlprogress.exec();
	if (!isftp)
	{
		QHttpResponseHeader dlresponse(dlhttp.lastResponse());
		if (dlresponse.statusCode() == 302)
		{
			downloadfile(dlresponse.value("location"), targetfile);
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
	if (installType == "USB Drive")
	{
		QFile::rename(QString("%1ubndlout.tmp").arg(ubntmpf), targetfile);
	}
}

void unetbootin::dlprogressupdate(int dlbytes, int maxbytes)
{
	dlprogress.setValue(dlbytes);
	dlprogress.setMaximum(maxbytes);
	dlprogress.setLabelText(QObject::tr("Downloading files, please wait...\nSource: %1\nDestination: %2\nDownloaded: %3 of %4 bytes").arg(sourcefile).arg(destinfile).arg(dlbytes).arg(maxbytes));
}

void unetbootin::dlprogressupdate64(qint64 dlbytes, qint64 maxbytes)
{
	dlprogress.setValue(dlbytes);
	dlprogress.setMaximum(maxbytes);
	dlprogress.setLabelText(QObject::tr("Downloading files, please wait...\nSource: %1\nDestination: %2\nDownloaded: %3 of %4 bytes").arg(sourcefile).arg(destinfile).arg(dlbytes).arg(maxbytes));
}

QString unetbootin::downloadpagecontents(QString pageurl)
{
	QUrl pgurl(pageurl);
	QHttp pghttp;
	QEventLoop pgwait;
	connect(&pghttp, SIGNAL(done(bool)), &pgwait, SLOT(quit()));
	pghttp.setHost(pgurl.host());
	pghttp.get(pgurl.path());
	pgwait.exec();
	QHttpResponseHeader pgresponse(pghttp.lastResponse());
	if (pgresponse.statusCode() == 302)
	{
		return downloadpagecontents(pgresponse.value("location"));
	}
	else
	{
		return QString(pghttp.readAll());
	}
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

void unetbootin::callexternapp(QString execFile, QString execParm)
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
	#endif
	#ifdef Q_OS_UNIX
	QProcess lnexternapp;
	lnexternapp.start(QString("%1 %2").arg(execFile).arg(execParm));
	lnexternapp.waitForFinished(-1);
	#endif
}

QString unetbootin::getuuid(QString voldrive)
{
	#ifdef Q_OS_WIN32
	voldrive.append("\\");
	char outpvn[256];
	GetVolumeNameForVolumeMountPointA(voldrive.toAscii(), outpvn, 256);
	return QString(outpvn).remove(QRegExp("^.{0,}\\{")).remove(QRegExp("\\}.{0,}$")).remove("\r").remove("\n");
	#endif
	#ifdef Q_OS_UNIX
	QProcess volidp;
	volidp.start(QString("vol_id -u %1").arg(voldrive));
	volidp.waitForFinished(-1);
	return QString(volidp.readAll()).remove("\r").remove("\n");
	#endif
}

#ifdef Q_OS_UNIX

QString unetbootin::locatecommand(QString commandtolocate, QString reqforinstallmode, QString packagename)
{
	QProcess whereiscommand;
	whereiscommand.start(QString("whereis %1").arg(commandtolocate));
	whereiscommand.waitForFinished(-1);
	QString commandbinpath = QString(whereiscommand.readAll());
	QStringList commandbinpathL = commandbinpath.split(" ").join("\n").split("\t").join("\n").split("\n");
	for (int i = 0; i < commandbinpathL.size(); ++i)
	{
		if (commandbinpathL.at(i).contains("bin/"))
		{
			return commandbinpathL.at(i);
		}
	}
	QMessageBox errorcmdnotfoundmsgbx;
	errorcmdnotfoundmsgbx.setIcon(QMessageBox::Warning);
	errorcmdnotfoundmsgbx.setWindowTitle(QString(QObject::tr("%1 not found")).arg(commandtolocate));
	errorcmdnotfoundmsgbx.setText(QString(QObject::tr("%1 not found. This is required for %2 install mode.\nInstall the \"%3\" package or your distribution's equivalent.")).arg(commandtolocate, reqforinstallmode, packagename));
	errorcmdnotfoundmsgbx.setStandardButtons(QMessageBox::Ok);
	switch (errorcmdnotfoundmsgbx.exec())
	{
		case QMessageBox::Ok:
			break;
		default:
			break;
	}
 	return "ERROR";
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
	while (true)
	{
		procmountsL = procmountsS.readAll().split("\n").filter(devicenode);
		if (!procmountsL.isEmpty())
			break;
		QMessageBox errordevnotmountedmsgbx;
		errordevnotmountedmsgbx.setIcon(QMessageBox::Warning);
		errordevnotmountedmsgbx.setWindowTitle(QString(QObject::tr("%1 not mounted")).arg(devicenode));
		errordevnotmountedmsgbx.setText(QString(QObject::tr("%1 is not mounted. Mount it and press the \"OK\" button to proceed with installation, or press the \"Cancel\" button to abort.")).arg(devicenode));
		errordevnotmountedmsgbx.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
		switch (errordevnotmountedmsgbx.exec())
		{
			case QMessageBox::Cancel:
			{
				return "INSTALL ABORTED";
			}
		}
	}
	return procmountsL.at(0).split("\t").join(" ").split(" ").at(1);
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
	"menuitem=grub,UNetbootin\n"
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
	while (!bootiniOut.atEnd())
	{
		QString bootiniCurLine = bootiniOut.readLine();
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
	QString bootiniText = QString("%1\n%2=\"UNetbootin\"").arg(bootiniCurText).arg(QDir::toNativeSeparators(QString("%1ubnldr.mbr").arg(targetDrive)));
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
	vbcdEditS1 << QString("bcdedit /create /d \"UNetbootin\" /application bootsector > %1tmpbcdid").arg(targetPath) << endl;
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
	QFile dstF(dstfName);
	dstF.open(QIODevice::WriteOnly);
	QFile srcF(QString(":/%1").arg(srcfName));
	srcF.open(QIODevice::ReadOnly);
	dstF.write(srcF.readAll());
	dstF.close();
	srcF.close();
}

void unetbootin::runinst()
{
	installType = typeselect->currentText();
	targetDrive = driveselect->currentText();
	QString ginstallDir;
	QString installDir;
	#ifdef Q_OS_WIN32
	if (installType == "Hard Disk")
	{
		ginstallDir = "unetbtin/";
	}
	if (installType == "USB Drive")
	{
		ginstallDir = "";
	}
	installDir = ginstallDir;
	targetDev = QString("%1").arg(targetDrive).remove("\\");
	#endif
	#ifdef Q_OS_UNIX
	if (installType == "Hard Disk")
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
	if (installType == "USB Drive")
	{
		targetDev = driveselect->currentText();
		ginstallDir = "";
		installDir = ginstallDir;
		targetDrive = QString("%1/").arg(locatemountpoint(targetDev));
		if (targetDrive == "INSTALL ABORTED/")
		{
			this->close();
			return;
		}
	}
	#endif
	devuuid = getuuid(targetDev);
	kernelLine = "kernel";
	kernelLoc = QString("/%1ubnkern").arg(ginstallDir);
	initrdLine = "initrd";
	initrdLoc = QString("/%1ubninit").arg(ginstallDir);
	targetPath = QDir::toNativeSeparators(QString("%1%2").arg(targetDrive).arg(installDir));
	QDir dir;
	if (!dir.exists(targetPath))
	{
		dir.mkpath(targetPath);
	}
	if (QFile::exists(QString("%1ubnkern").arg(targetPath)))
	{
		QFile::remove(QString("%1ubnkern").arg(targetPath));
	}
	if (QFile::exists(QString("%1ubninit").arg(targetPath)))
	{
		QFile::remove(QString("%1ubninit").arg(targetPath));
	}
	hide();
	if (radioFloppy->isChecked())
	{
		if (diskimagetypeselect->currentIndex() == diskimagetypeselect->findText("Floppy") || diskimagetypeselect->currentIndex() == diskimagetypeselect->findText("HDD"))
		{
			instIndvfl("memdisk", QString("%1ubnkern").arg(targetPath));
			QFile::copy(FloppyPath->text(), QString("%1ubninit").arg(targetPath));
   		}
		if (diskimagetypeselect->currentIndex() == diskimagetypeselect->findText("ISO"))
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
		instDetType();
	}
	if (radioManual->isChecked())
	{
		QFile::copy(KernelPath->text(), QString("%1ubnkern").arg(targetPath));
		QFile::copy(InitrdPath->text(), QString("%1ubninit").arg(targetPath));
		kernelOpts = OptionEnter->text();
		instDetType();
	}
	if (radioDistro->isChecked())
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
		if (nameVersion.contains("_x64"))
		{
			nameVersion.remove("_x64");
			isarch64 = true;
	   	}
	   	else
	   	{
	   		isarch64 = false;
	  	}
		#include "distrolst.cpp"
		if (QFile::exists(QString("%1sevnz.exe").arg(ubntmpf)))
		{
			QFile::remove(QString("%1sevnz.exe").arg(ubntmpf));
		}
		if (QFile::exists(QString("%1\\7z.dll").arg(ubntmpf)))
		{
			QFile::remove(QString("%1\\7z.dll").arg(ubntmpf));
		}
		instDetType();
	}
}

void unetbootin::instDetType()
{
	if (installType == "Hard Disk")
	{
		runinsthdd();
   	}
   	if (installType == "USB Drive")
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
	QFile::setPermissions(QDir::toNativeSeparators(QString("%1unetbtin.exe").arg(targetDrive)), QFile::WriteOther);
	if (QFile::exists(QDir::toNativeSeparators(QString("%1ubnldr").arg(targetDrive))))
	{
		QFile::remove(QDir::toNativeSeparators(QString("%1ubnldr").arg(targetDrive)));
	}
	instIndvfl("ubnldr", QString("%1ubnldr").arg(targetDrive));
	if (QFile::exists(QDir::toNativeSeparators(QString("%1ubnldr.mbr").arg(targetDrive))))
	{
		QFile::remove(QDir::toNativeSeparators(QString("%1ubnldr.mbr").arg(targetDrive)));
	}
	instIndvfl("ubnldr.mbr", QString("%1ubnldr.mbr").arg(targetDrive));
	if (QFile::exists(QDir::toNativeSeparators(QString("%1ubnldr.exe").arg(targetDrive))))
	{
		QFile::remove(QDir::toNativeSeparators(QString("%1ubnldr.exe").arg(targetDrive)));
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
	while (!bkmenulstout.atEnd())
	{
		QString menulstCurLine = bkmenulstout.readLine();
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
	"timeout 3\n"
	#endif
	"title UNetbootin\n"
	#ifdef Q_OS_WIN32
	"find --set-root %3\n"
	#endif
	#ifdef Q_OS_UNIX
	"root %8\n"
	#endif
	"%1 %2 %3 %4\n"
	"%5 %6 %7\n"
	"boot").arg(kernelLine, kernelParam, kernelLoc, kernelOpts, initrdLine, initrdLoc, initrdOpts
	#ifdef Q_OS_UNIX
	, getGrubNotation(targetDev), ecurmenulstText
	#endif
	);
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
	QMessageBox rebootmsgb;
	rebootmsgb.setIcon(QMessageBox::Information);
   	rebootmsgb.setWindowTitle(QObject::tr("Reboot Now?"));
	rebootmsgb.setText(QObject::tr("After rebooting, select the UNetbootin menu entry to boot.%1\nReboot now?").arg(postinstmsg));
		rebootmsgb.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
		switch (rebootmsgb.exec())
		{
			case QMessageBox::Ok:
			{
				unetbootin::sysreboot();
		}
		case QMessageBox::Cancel:
			break;
 		default:
			break;
   		}
  	}

void unetbootin::runinstusb()
{
	#ifdef Q_OS_WIN32
	if (QFile::exists(QString("%1syslinux.exe").arg(ubntmpf)))
	{
		QFile::remove(QString("%1syslinux.exe").arg(ubntmpf));
	}
	instIndvfl("syslinux.exe", QString("%1syslinux.exe").arg(ubntmpf));
	callexternapp(QString("%1syslinux.exe").arg(ubntmpf), QString("-ma %1").arg(targetDev));
	QFile::remove(QString("%1syslinux.exe").arg(ubntmpf));
	#endif
	#ifdef Q_OS_UNIX
	callexternapp(syslinuxcommand, targetDev);
	callexternapp(mssyscommand, QString("-s %1").arg(QString(targetDev).remove(-1, 1)));
	callexternapp(sfdiskcommand, QString("%1 -A%2").arg(QString(targetDev).remove(-1, 1), QString(targetDev).at(targetDev.size() - 1)));
	#endif
	if (QFile::exists(QString("%1syslinux.cfg").arg(targetPath)))
	{
		QFile::remove(QString("%1syslinux.cfg").arg(targetPath));
	}
	QFile syslinuxcfg(QString("%1syslinux.cfg").arg(targetPath));
   	syslinuxcfg.open(QIODevice::WriteOnly | QIODevice::Text);
	QTextStream syslinuxcfgout(&syslinuxcfg);
	QString syslinuxcfgtxt = QString("default unetbootin\n"
	"label unetbootin\n"
	"\tkernel %1\n"
	"\tappend initrd=%2 %3").arg(kernelLoc, initrdLoc, kernelOpts);
	syslinuxcfgout << syslinuxcfgtxt << endl;
	syslinuxcfg.close();
	QMessageBox usbfinstmsgb;
	usbfinstmsgb.setIcon(QMessageBox::Information);
   	usbfinstmsgb.setWindowTitle(QObject::tr("Reboot Now?"));
	usbfinstmsgb.setText(QObject::tr("After rebooting, select the USB boot option in the BIOS boot menu.%1\nReboot now?").arg(postinstmsg));
	usbfinstmsgb.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
	switch (usbfinstmsgb.exec())
	{
		case QMessageBox::Ok:
		{
			unetbootin::sysreboot();
		}
		case QMessageBox::Cancel:
			break;
 		default:
			break;
   	}
}
