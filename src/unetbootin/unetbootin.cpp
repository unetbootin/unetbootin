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
<< QRegExp(".exe$", Qt::CaseInsensitive)
<< QRegExp(".deb$", Qt::CaseInsensitive)
<< QRegExp(".udeb$", Qt::CaseInsensitive)
<< QRegExp("system.map", Qt::CaseInsensitive);

static const QList<QRegExp> ignoredtypeskernelRL = QList<QRegExp>()
<< QRegExp("initrd.gz$", Qt::CaseInsensitive)
<< QRegExp("initrd.img$", Qt::CaseInsensitive);

static const QList<QRegExp> ignoredtypesinitrdRL = QList<QRegExp>()
<< QRegExp("bzImage$", Qt::CaseInsensitive);

static const QString SALT_DETECTED = "*SaLT*";

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
    lnexternapp.start("\"" + execFile + "\" " + execParm);
	lnexternapp.waitForFinished(-1);
	retnValu = QString(lnexternapp.readAll());
	#endif
}

void callexternappWriteToStdinT::run()
{
	QProcess lnexternapp;
    lnexternapp.start("\"" + execFile + "\" " + execParm);
	lnexternapp.write(writeToStdin.toAscii().data());
	lnexternapp.closeWriteChannel();
	lnexternapp.waitForFinished(-1);
	retnValu = QString(lnexternapp.readAll());
}

void copyfileT::run()
{
	QFile srcF(source);
	srcF.open(QIODevice::ReadOnly);
	QFile dstF(destination);
	dstF.open(QIODevice::WriteOnly);
	qint64 maxbytes = srcF.size();
	qint64 dlbytes = 0;
	char *buf = new char[262144];
	while (!srcF.atEnd())
	{
		qint64 bytesread = srcF.read(buf, 262144);
		dstF.write(buf, bytesread);
		dlbytes += bytesread;
		emit datacopied64(dlbytes, maxbytes);
#ifdef Q_OS_UNIX
		unetbootin::callexternapp("sync", "");
#endif
	}
	delete[] buf;
	srcF.close();
	dstF.close();
	emit finished();
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

bool unetbootin::ubninitialize(QList<QPair<QString, QString> > oppairs)
{
    skipExtraction = false;
    redundanttopleveldir = false;
	isarch64 = false;
	islivecd = false;
	isnetinstall = false;
	ishdmedia = false;
	overwriteall = false;
	searchsymlinks = false;
	ignoreoutofspace = false;
	downloadFailed = false;
	exitOnCompletion = false;
	testingDownload = false;
	issalt = false;
	persistenceSpaceMB = 0;
    logFile = 0;
    logStream = 0;
#ifdef Q_OS_MAC
	ignoreoutofspace = true;
#endif
	dontgeneratesyslinuxcfg = false;
	#ifdef Q_OS_UNIX
	isext2 = false;
	#endif
	secondlayer->setEnabled(false);
	secondlayer->hide();
	firstlayer->setEnabled(true);
	firstlayer->show();
	this->setWindowTitle(UNETBOOTINB);
	overwriteall = false;
#ifndef Q_OS_MAC
	typeselect->addItem(tr("Hard Disk"));
#endif
	typeselect->addItem(tr("USB Drive"));
	diskimagetypeselect->addItem(tr("ISO"));
	diskimagetypeselect->addItem(tr("Floppy"));
	bool hideCustom = true;
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
	#include "distrovercust.cpp"
	#ifdef STDUNETBOOTIN
	optionslayer->setEnabled(true);
	optionslayer->show();
	#include "distrover.cpp"
	#endif
	#include "customdistselect.cpp"
#ifdef Q_OS_MAC
	resourceDir = QDir(QApplication::applicationDirPath());
	resourceDir.cdUp();
	resourceDir.cd("Resources");
	syslinuxcommand = resourceDir.absoluteFilePath("syslinux-mac");
	sevzcommand = resourceDir.absoluteFilePath("7z-mac");
	mke2fscommand = resourceDir.absoluteFilePath("mke2fs");
	fdiskcommand = locatecommand("fdisk", tr("either"), "util-linux");
#endif
	#ifdef Q_OS_LINUX
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
		sfdiskcommand = locatecommand("sfdisk", tr("either"), "silent");
	if (QFile::exists("/lib/udev/vol_id"))
		volidcommand = "/lib/udev/vol_id";
	else
		volidcommand = locatecommand("vol_id", tr("either"), "silent");
	if (volidcommand.isEmpty())
	{
		if (QFile::exists("/sbin/blkid"))
			blkidcommand = "/sbin/blkid";
		else
			blkidcommand = locatecommand("blkid", tr("either"), "e2fsprogs");
	}
	else
		blkidcommand = "/sbin/blkid";
	if (QFile::exists("/sbin/mke2fs"))
		mke2fscommand = "/sbin/mke2fs";
	else
		mke2fscommand = locatecommand("mke2fs", tr("LiveUSB persistence"), "e2fsprogs");
	if (QFile::exists("/sbin/e2label"))
		e2labelcommand = "/sbin/e2label";
	else
		e2labelcommand = locatecommand("e2label", "Arch Linux", "e2fsprogs");
	if (QFile::exists("/usr/bin/mlabel"))
		mlabelcommand = "/usr/bin/mlabel";
	else
		mlabelcommand = locatecommand("mlabel", "Arch Linux", "mtools");
	syslinuxcommand = "/usr/bin/ubnsylnx";
	extlinuxcommand = "/usr/bin/ubnexlnx";
	#ifdef NOSTATIC
	if (QFile::exists("/usr/bin/syslinux"))
		syslinuxcommand = "/usr/bin/syslinux";
	else
		syslinuxcommand = locatecommand("syslinux", tr("FAT32-formatted USB drive"), "syslinux");
	if (QFile::exists("/usr/bin/extlinux"))
		extlinuxcommand = "/usr/bin/extlinux";
	else
        extlinuxcommand = locatecommand("extlinux", tr("EXT2-formatted USB drive"), "extlinux");
	#endif
	sevzcommand = locatecommand("7z", tr("either"), "p7zip-full");
	#endif
	ubntmpf = QDir::toNativeSeparators(QString("%1/").arg(QDir::tempPath()));
    #ifdef Q_OS_LINUX
    if (ubntmpf.isEmpty() || ubntmpf == "/")
    {
        ubntmpf = "/tmp/";
    }
    #endif
    if (typeselect->findText(tr("USB Drive")) != -1)
		typeselect->setCurrentIndex(typeselect->findText(tr("USB Drive")));
	#ifdef HDDINSTALL
	if (typeselect->findText(tr("Hard Disk")) != -1)
		typeselect->setCurrentIndex(typeselect->findText(tr("Hard Disk")));
	#endif
	for (QList<QPair<QString, QString> >::const_iterator i = oppairs.constBegin(); i < oppairs.constEnd(); ++i)
	{
		QString pfirst(i->first);
		QString psecond(i->second);
		if (pfirst.contains("method", Qt::CaseInsensitive))
		{
			if (psecond.contains("distribution", Qt::CaseInsensitive))
				this->radioDistro->setChecked(true);
			else if (psecond.contains("diskimage", Qt::CaseInsensitive))
				this->radioFloppy->setChecked(true);
			else if (psecond.contains("custom", Qt::CaseInsensitive))
				this->radioManual->setChecked(true);
		}
		else if (pfirst.contains("distribution", Qt::CaseInsensitive))
		{
			int distidx = this->distroselect->findText(psecond, Qt::MatchFixedString);
			if (distidx != -1)
				this->distroselect->setCurrentIndex(distidx);
		}
		else if (pfirst.contains("version", Qt::CaseInsensitive))
		{
			QStringList verlist = this->distroselect->itemData(this->distroselect->currentIndex()).value<QStringList>();
			for (int j = 2; j < verlist.size(); ++j)
			{
				if (verlist[j].contains(psecond, Qt::CaseInsensitive))
				{
					this->dverselect->setCurrentIndex(j-2);
					break;
				}
			}
		}
		else if (pfirst.contains("isofile", Qt::CaseInsensitive))
		{
			this->diskimagetypeselect->setCurrentIndex(diskimagetypeselect->findText(tr("ISO")));
			this->FloppyPath->setText(psecond);
            this->radioFloppy->setChecked(true);
		}
		else if (pfirst.contains("imgfile", Qt::CaseInsensitive))
		{
			this->diskimagetypeselect->setCurrentIndex(diskimagetypeselect->findText(tr("Floppy")));
			this->FloppyPath->setText(psecond);
            this->radioFloppy->setChecked(true);
		}
		else if (pfirst.contains("kernelfile", Qt::CaseInsensitive))
		{
			this->KernelPath->setText(psecond);
		}
		else if (pfirst.contains("initrdfile", Qt::CaseInsensitive))
		{
			this->InitrdPath->setText(psecond);
		}
		else if (pfirst.contains("cfgfile", Qt::CaseInsensitive))
		{
			QString cfgoptstxt = getcfgkernargs(psecond, "", QStringList(), QStringList());
			if (cfgoptstxt.isEmpty())
			{
				cfgoptstxt = getgrubcfgargs(psecond);
			}
			this->OptionEnter->setText(cfgoptstxt);
		}
		else if (pfirst.contains("kernelopts", Qt::CaseInsensitive))
		{
			this->OptionEnter->setText(psecond);
		}
		else if (pfirst.contains("installtype", Qt::CaseInsensitive))
		{
			if (psecond.contains("Hard", Qt::CaseInsensitive) || psecond.contains("HDD", Qt::CaseInsensitive))
				this->typeselect->setCurrentIndex(this->typeselect->findText(tr("Hard Disk")));
			else
				this->typeselect->setCurrentIndex(this->typeselect->findText(tr("USB Drive")));
		}
		else if (pfirst.contains("targetdrive", Qt::CaseInsensitive))
		{
#ifdef Q_OS_WIN32
			if (!psecond.endsWith('\\'))
			{
				psecond = psecond + '\\';
			}
#endif
			int driveidx = this->driveselect->findText(psecond, Qt::MatchFixedString);
			if (driveidx != -1)
			{
				this->driveselect->setCurrentIndex(driveidx);
			}
			else
			{
				this->driveselect->addItem(psecond);
				this->driveselect->setCurrentIndex(this->driveselect->findText(psecond, Qt::MatchFixedString));
			}
		}
		else if (pfirst.contains("showcustom", Qt::CaseInsensitive))
		{
			if (psecond.contains('y', Qt::CaseInsensitive))
			{
				hideCustom = false;
			}
		}
		else if (pfirst.contains("nodistro", Qt::CaseInsensitive))
		{
			if (psecond.contains('y', Qt::CaseInsensitive))
			{
				radioDistro->setEnabled(false);
				radioDistro->hide();
				distroselect->setEnabled(false);
				distroselect->hide();
				dverselect->setEnabled(false);
				dverselect->hide();
				intromessage->move(intromessage->x(), intromessage->y()-20);
				intromessage->resize(intromessage->width(), intromessage->height() + 20);
			}
		}
		else if (pfirst.contains("message", Qt::CaseInsensitive))
		{
			intromessage->setText(psecond);
		}
		else if (pfirst.contains("persistentspace", Qt::CaseInsensitive))
		{
			bool isInt = false;
			int numMBpersistentSpace = psecond.toInt(&isInt, 10);
			if (isInt)
				this->persistencevalue->setValue(numMBpersistentSpace);
		}
		else if (pfirst.contains("testingdownload", Qt::CaseInsensitive))
		{
			if (psecond.contains('y', Qt::CaseInsensitive))
			{
				testingDownload = true;
				exitOnCompletion = true;
			}
		}
		else if (pfirst.contains("exitoncompletion", Qt::CaseInsensitive))
		{
			if (psecond.contains('y', Qt::CaseInsensitive))
			{
				exitOnCompletion = true;
			}
		}
        else if (pfirst.contains("skipextraction", Qt::CaseInsensitive))
        {
            if (psecond.contains('y', Qt::CaseInsensitive))
            {
                skipExtraction = true;
            }
        }
		else if (pfirst.contains("autoinstall", Qt::CaseInsensitive))
		{
			if (psecond.contains('y', Qt::CaseInsensitive))
			{
				exitOnCompletion = true;
				overwriteall = true;
				return true;
			}
		}
		else if (pfirst.contains("action", Qt::CaseInsensitive))
		{
			if (psecond.contains("listdistros", Qt::CaseInsensitive))
			{
				for (int i = 1; i < this->distroselect->count(); ++i)
				{
					printf("%s\n", this->distroselect->itemText(i).toAscii().constData());
				}
				QApplication::exit();
				exit(0);
			}
			else if (psecond.contains("listversions", Qt::CaseInsensitive))
			{
				for (int i = 0; i < this->dverselect->count(); ++i)
				{
					printf("%s\n", this->dverselect->itemText(i).toAscii().constData());
				}
				QApplication::exit();
				exit(0);
			}
		}
	}
	if (hideCustom)
	{
		radioManual->setEnabled(false);
		radioManual->hide();
		labelkernel->setEnabled(false);
		labelkernel->hide();
		labelinitrd->setEnabled(false);
		labelinitrd->hide();
		labeloption->setEnabled(false);
		labeloption->hide();
		KernelFileSelector->setEnabled(false);
		KernelFileSelector->hide();
		InitrdFileSelector->setEnabled(false);
		InitrdFileSelector->hide();
		CfgFileSelector->setEnabled(false);
		CfgFileSelector->hide();
		OptionEnter->setEnabled(false);
		OptionEnter->hide();
		KernelPath->setEnabled(false);
		KernelPath->hide();
		InitrdPath->setEnabled(false);
		InitrdPath->hide();
		KernelFileSelector->setEnabled(false);
		KernelFileSelector->hide();
		InitrdFileSelector->setEnabled(false);
		InitrdFileSelector->hide();
		radioLayout->removeItem(verticalSpacer);
	}
	return false;
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
	return listsanedrives();
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
				#ifdef Q_OS_LINUX
				QDir devlstdir("/dev/disk/by-id/");
				QFileInfoList usbfileinfoL = devlstdir.entryInfoList(QDir::NoDotAndDotDot|QDir::Files);
				for (int i = 0; i < usbfileinfoL.size(); ++i)
				{
//                    if (usbfileinfoL.at(i).contains(QRegExp("^usb-\\S{1,}-part\\d{1,}$")))
//                    {
//                        fulldrivelist.append(usbfileinfoL.at(i).canonicalFilePath());
//                    }
                    if (usbfileinfoL.at(i).fileName().contains(QRegExp("^usb-\\S{1,}$")) ||
                        usbfileinfoL.at(i).fileName().contains(QRegExp("^mmc-\\S{1,}$")))
                    {
						if (!volidcommand.isEmpty())
						{
							if (QString(callexternapp(volidcommand, QString("-t %2").arg(usbfileinfoL.at(i).canonicalFilePath()))).contains(QRegExp("(vfat|ext2|ext3|ext4)")))
								fulldrivelist.append(usbfileinfoL.at(i).canonicalFilePath());
						}
						else
						{
							QString tstrblk = QString(callexternapp(blkidcommand, QString("-s TYPE %2").arg(usbfileinfoL.at(i).canonicalFilePath())));
							if (tstrblk.contains('='))
							{
								if (tstrblk.section('=', -1, -1).remove('"').contains(QRegExp("(vfat|ext2|ext3|ext4)")))
									fulldrivelist.append(usbfileinfoL.at(i).canonicalFilePath());
							}
						}
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
#ifdef Q_OS_MAC
QString diskutilList = callexternapp("diskutil", "list");
QStringList usbdevsL = diskutilList.split("\n").filter(QRegExp("(FAT|Microsoft)")).join(" ").split(" ").filter("disk");
for (int i = 0; i < usbdevsL.size(); ++i)
{
	fulldrivelist.append("/dev/"+usbdevsL.at(i));
}
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
	#ifdef Q_OS_LINUX
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
#ifdef Q_OS_MAC
		return listsanedrives();
#endif
	return fulldrivelist;
}

void unetbootin::on_typeselect_currentIndexChanged(int typeselectIndex)
{
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
    QString nameFloppy = QDir::toNativeSeparators(QFileDialog::getOpenFileName(this, tr("Open Disk Image File"), QDir::homePath(), tr("All Files") + " (*);;" + tr("ISO") + " (*.iso);;" + tr("Floppy") + " (*.img)", 0
    #if (QT_VERSION >= QT_VERSION_CHECK(4, 5, 0))
    , QFileDialog::ReadOnly
    #endif
    ));
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
    QString nameKernel = QDir::toNativeSeparators(QFileDialog::getOpenFileName(this, tr("Open Kernel File"), QDir::homePath(), tr("All Files (*)"), 0
    #if (QT_VERSION >= QT_VERSION_CHECK(4, 5, 0))
    , QFileDialog::ReadOnly
    #endif
    ));
	KernelPath->clear();
	KernelPath->insert(nameKernel);
	radioManual->setChecked(true);
}

void unetbootin::on_InitrdFileSelector_clicked()
{
    QString nameInitrd = QDir::toNativeSeparators(QFileDialog::getOpenFileName(this, tr("Open Initrd File"), QDir::homePath(), tr("All Files (*)"), 0
    #if (QT_VERSION >= QT_VERSION_CHECK(4, 5, 0))
    , QFileDialog::ReadOnly
    #endif
    ));
	InitrdPath->clear();
	InitrdPath->insert(nameInitrd);
	radioManual->setChecked(true);
}

void unetbootin::on_CfgFileSelector_clicked()
{
    QString nameCfg = QFileDialog::getOpenFileName(this, tr("Open Bootloader Config File"), QDir::homePath(), tr("All Files (*)"), 0
    #if (QT_VERSION >= QT_VERSION_CHECK(4, 5, 0))
    , QFileDialog::ReadOnly
    #endif
    );
	OptionEnter->clear();
	QString cfgoptstxt = getcfgkernargs(nameCfg, "", QStringList(), QStringList());
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
    if (typeselect->currentIndex() == typeselect->findText(tr("USB Drive")) && driveselect->currentText().isEmpty() && !testingDownload)
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
#ifdef Q_OS_MAC
    if (locatemountpoint(driveselect->currentText()) == "NOT MOUNTED" && !testingDownload)
		callexternapp("diskutil", "mount "+driveselect->currentText());
#endif
	#ifdef Q_OS_LINUX
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
	else if (radioDistro->isChecked() && distroselect->currentIndex() == distroselect->findText(unetbootin::tr("== Select Distribution ==")))
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
	else if (radioFloppy->isChecked() && !QFile::exists(FloppyPath->text()) && !FloppyPath->text().startsWith("http://") && !FloppyPath->text().startsWith("ftp://"))
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
	else if (radioManual->isChecked() && !QFile::exists(KernelPath->text()) && !KernelPath->text().startsWith("http://") && !KernelPath->text().startsWith("ftp://"))
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
	else if (radioManual->isChecked() && InitrdPath->text().trimmed() != "" && !QFile::exists(InitrdPath->text())  && !InitrdPath->text().startsWith("http://") && !InitrdPath->text().startsWith("ftp://"))
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
	randtmpfile tmplsF(ubntmpf, "ufl");
	callexternapp(getenv("COMSPEC"), QString("/c \"\"%1\" -bd -slt l \"%2\" > \"%3\"\"").arg(sevzcommand).arg(QFileInfo(archivefile).absoluteFilePath()).arg(QFileInfo(tmplsF).absoluteFilePath()));
	tmplsF.open(QIODevice::ReadOnly | QIODevice::Text);
	QTextStream tmplsS(&tmplsF);
	#endif
	#ifdef Q_OS_UNIX
    QString sevzlcommandout = callexternapp(sevzcommand, QString("-bd -slt l \"%1\"").arg(QFileInfo(archivefile).absoluteFilePath()));
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
			if (tmplsL == QString("Path = %1").arg(QFileInfo(archivefile).absoluteFilePath()))
				continue;
			if (tmplsL == QString("Path = %1").arg(QDir::toNativeSeparators(QFileInfo(archivefile).absoluteFilePath())))
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
	rmFile(tmplsF);
	#endif
	return qMakePair(qMakePair(tmplsSLF, tmplsSLFS), tmplsSLD);
}

bool unetbootin::overwritefileprompt(QString ovwfileloc)
{
	if (overwriteall)
	{
		rmFile(ovwfileloc);
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
			rmFile(ovwfileloc);
			return true;
		}
		case QMessageBox::YesToAll:
		{
			rmFile(ovwfileloc);
			overwriteall = true;
			return true;
		}
		case QMessageBox::No:
			return false;
		default:
			return false;
	}
}

bool unetbootin::ignoreoutofspaceprompt(QString destindir)
{
	QMessageBox overwritefilemsgbx;
	overwritefilemsgbx.setIcon(QMessageBox::Warning);
	overwritefilemsgbx.setWindowTitle(QString(tr("%1 is out of space, abort installation?")).arg(destindir));
	overwritefilemsgbx.setText(QString(tr("The directory %1 is out of space. Press 'Yes' to abort installation, 'No' to ignore this error and attempt to continue installation, and 'No to All' to ignore all out-of-space errors.")).arg(destindir));
	overwritefilemsgbx.setStandardButtons(QMessageBox::Yes | QMessageBox::NoToAll | QMessageBox::No);
	switch (overwritefilemsgbx.exec())
	{
		case QMessageBox::Yes:
		{
			QApplication::quit();
			return false;
		}
		case QMessageBox::No:
		{
			return true;
		}
		case QMessageBox::NoToAll:
		{
			this->ignoreoutofspace = true;
			return true;
		}
		default:
			return true;
	}
}

bool unetbootin::extractfile(QString filepath, QString destinfileL, QString archivefile)
{
	#ifdef Q_OS_UNIX
	if (installType != tr("USB Drive") && filepath.contains("boot/grub")) // don't nuke grub config
		return false;
	#endif
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
	int retv;
	if (QFileInfo(filepathfilename).absoluteFilePath() == QFileInfo(destinfilename).absoluteFilePath())
	{
		retv = true;
	}
	else
	{
		retv = QFile::rename(filepathfilename, destinfilename);
	}
	this->checkifoutofspace(destindir);
	return retv;
}

bool unetbootin::checkifoutofspace(QString destindir)
{
	if  (ignoreoutofspace == true)
		return false;
	bool outofspace = false;
	#ifdef Q_OS_UNIX
	struct statfs diskstatS;
	if (!statfs(QString(destindir+"/.").toAscii(), &diskstatS))
	{
		if (diskstatS.f_bavail * diskstatS.f_bfree < 1024)
			outofspace = true;
	}
	#endif
	#ifdef Q_OS_WIN32
	ULARGE_INTEGER FreeBytesAvailable;
	ULARGE_INTEGER TotalNumberOfBytes;
	ULARGE_INTEGER TotalNumberOfFreeBytes;
	if (GetDiskFreeSpaceExA(destindir.toAscii(), &FreeBytesAvailable, &TotalNumberOfBytes, &TotalNumberOfFreeBytes))
	{
		if (FreeBytesAvailable.QuadPart < 1024)
			outofspace = true;
	}
	#endif
	if (outofspace == true)
	{
		return !ignoreoutofspaceprompt(destindir);
	}
	return false;
}

QString unetbootin::locatekernel(QString archivefile, QPair<QStringList, QList<quint64> > archivefileconts)
{
	pdesc1->setText(tr("Locating kernel file in %1").arg(archivefile));
	if (issalt) {
		// The grub2 boot loader is return,
		// it is prepended with a Linux header,
		// so that syslinux can chainload to it.
		// The embeded configuration file will find the correct prefix using the .live file at the root of the USB key.
		return "boot/grub2-linux.img";
	}
	else
	{
        QStringList kernelnames = QStringList() << "vmlinuz" << "vmlinux" << "bzImage" << "kernel" << "sabayon" << "gentoo" << "linux26" << "linux24" << "bsd" << "unix" << "linux" << "rescue" << "xpud" << "bzI" << "kexec" << "vmlinuz.efi";
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
				return narchivefileconts.at(j);
			}
		}
	}
	pdesc1->setText("");
	return "";
}
}

bool unetbootin::extractkernel(QString archivefile, QString kernoutputfile, QPair<QStringList, QList<quint64> > archivefileconts)
{
	QString kfloc = locatekernel(archivefile, archivefileconts);
	if (kfloc == "")
		return false;
	pdesc1->setText(tr("Copying kernel file from %1").arg(kfloc));
	return extractfile(kfloc, kernoutputfile, archivefile);
}

QString unetbootin::locateinitrd(QString archivefile, QPair<QStringList, QList<quint64> > archivefileconts)
{
	pdesc1->setText(tr("Locating initrd file in %1").arg(archivefile));
	QStringList kernelnames = QStringList() << "initrd.img.gz" << "initrd.lz" << "initrd.igz" << "initrd.gz" << "initrd.xz" << "initrd.lzma" << "initrd.img" << "initramfs.gz" << "initramfs.img" << "initrd" << "initramfs" << "minirt" << "miniroot" << "sabayon.igz" << "gentoo.igz" << "archlive.img" << "rootfs.gz" << ".igz" << ".cgz" << ".img" << "rootfs" << "fs.gz" << "root.gz" << ".gz" << "initram" << "initr" << "init" << "ram" << ".lz" << ".lzma" << ".xz";
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
        if (archivefileconts.second.at(i) >= 128000 && archivefileconts.second.at(i) < 314572800) // between 128 KB and 300 MB
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
				return narchivefileconts.at(j);
			}
		}
	}
	pdesc1->setText("");
	return "";
}

bool unetbootin::extractinitrd(QString archivefile, QString kernoutputfile, QPair<QStringList, QList<quint64> > archivefileconts)
{
	QString kfloc = locateinitrd(archivefile, archivefileconts);
	if (kfloc == "")
		return false;
	pdesc1->setText(tr("Copying initrd file from %1").arg(kfloc));
	return extractfile(kfloc, kernoutputfile, archivefile);
}

QString unetbootin::extractcfg(QString archivefile, QStringList archivefileconts)
{
	pdesc1->setText(tr("Extracting bootloader configuration"));
	QString saltpcfg;
	QString grubpcfg;
	QString syslinuxpcfg;
	QStringList saltdetectfiles = QStringList() << "boot/grub2-linux.img" << "boot/grub/salt.env";
	QStringList grubcfgtypes = QStringList() << "menu.lst" << "grub.conf";
	QStringList mlstfoundfiles;
	int saltDetected = 0; // must be equals to saltdetectfiles.size() to be considered as detected
	for (int i = 0; i < saltdetectfiles.size(); ++i)
	{
		mlstfoundfiles = archivefileconts.filter(saltdetectfiles.at(i), Qt::CaseInsensitive);
		if (!mlstfoundfiles.isEmpty())
		{
			saltDetected++;
		}
	}
	if (saltDetected == saltdetectfiles.size()) {
		saltpcfg = SALT_DETECTED; // not a real config, but used to inform that SaLT has been detected.
	}
	else
	{
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
				rmFile(mlstftf);
				if (!grubpcfg.isEmpty())
					break;
			}
		}
		if (!grubpcfg.isEmpty())
			break;
	}
	QStringList syslinuxcfgtypes = QStringList() << "syslinux.cfg" << "isolinux.cfg" << "extlinux.cfg" << "pxelinux.cfg" << "grubenv" << "menu_en.cfg" << "en.cfg" << "extlinux.conf" << "grub.cfg" << ".cfg";
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
				if (lcfgfoundfiles.at(j).contains("grubenv"))
					loadgrub2env(ccfgftf.fileName());
				else if (lcfgfoundfiles.at(j).contains("grub"))
					syslinuxpcfg = getgrub2cfgargs(ccfgftf.fileName(), archivefile, archivefileconts, QStringList() << lcfgfoundfiles.at(j)).trimmed();
				else
					syslinuxpcfg = getcfgkernargs(ccfgftf.fileName(), archivefile, archivefileconts, QStringList() << lcfgfoundfiles.at(j)).trimmed();
				rmFile(ccfgftf);
				if (!syslinuxpcfg.isEmpty())
					break;
			}
		}
		if (!syslinuxpcfg.isEmpty())
			break;
	}
	}
	if (!saltpcfg.isEmpty()) {
		return saltpcfg;
	}
	else if (syslinuxpcfg.isEmpty())
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
	pdesc1->setText(tr("Extracting bootloader configuration"));
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
				rmFile(mlstftf);
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
	QStringList syslinuxcfgtypes = QStringList() << "syslinux.cfg" << "isolinux.cfg" << "extlinux.cfg" << "pxelinux.cfg" << "grubenv" << "extlinux.conf" << "grub.cfg";
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
				if (lcfgfoundfiles.at(j).contains("grubenv"))
					loadgrub2env(ccfgftf.fileName());
				else if (lcfgfoundfiles.at(j).contains("grub"))
					syslinuxpcfgPL = getgrub2cfgargsL(ccfgftf.fileName(), archivefile, archivefileconts, QStringList() << lcfgfoundfiles.at(j));
				else
					syslinuxpcfgPL = getcfgkernargsL(ccfgftf.fileName(), archivefile, archivefileconts, QStringList() << lcfgfoundfiles.at(j));
				rmFile(ccfgftf);
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
	for (int i = 0; i < combinedcfgPL.first.first.size(); ++i)
	{
		bool isduplicate = false;
		for (int j = 0; j < filteredcfgPL.second.first.size(); ++j)
		{
			if (filteredcfgPL.second.first.at(j) == combinedcfgPL.second.first.at(i)) // duplicate title
			{
				isduplicate = true;
				break;
			}
		}
		if (isduplicate)
			continue;
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
    if (redundanttopleveldir && !redundantrootdirname.isEmpty())
    {
        for (int i = 0; i < filteredcfgPL.first.second.size(); ++i)
        {
            if (filteredcfgPL.first.second.at(i).startsWith(redundantrootdirname))
            {
                filteredcfgPL.first.second[i] = filteredcfgPL.first.second[i].mid(redundantrootdirname.length());
            }
            if (filteredcfgPL.first.second.at(i).startsWith("/" + redundantrootdirname))
            {
                filteredcfgPL.first.second[i] = filteredcfgPL.first.second[i].mid(redundantrootdirname.length() + 1);
            }
        }
    }
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

void unetbootin::extractiso(QString isofile)
{
	if (!sdesc2->text().contains(trcurrent))
	{
		sdesc1->setText(QString(sdesc1->text()).remove("<b>").replace(trcurrent+"</b>", trdone));
		sdesc2->setText(QString("<b>%1 %2</b>").arg(sdesc2->text()).arg(trcurrent));
	}
	tprogress->setValue(0);
	QPair<QPair<QStringList, QList<quint64> >, QStringList> listfilesizedirpair = listarchiveconts(isofile);
	if (listfilesizedirpair.first.first.size() == 1)
	{
		QString subarchivename = listfilesizedirpair.first.first.at(0);
		randtmpfile tmpoutsubarchive(ubntmpf, subarchivename.right(3));
		pdesc1->setText(tr("<b>Extracting compressed iso:</b> %1").arg(subarchivename));
		extractfile(subarchivename, tmpoutsubarchive.fileName(), isofile);
		return extractiso(tmpoutsubarchive.fileName());
	}
	if (listfilesizedirpair.first.first.contains(QDir::toNativeSeparators("rescue/KRD.VERSION"), Qt::CaseInsensitive))
	{
		return extractiso_krd10(isofile);
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
					return extractiso(tmpoutsubarchive.fileName());
				}
			}
		}
	}
	QStringList filepathnames = listfilesizedirpair.first.first;
	QStringList directorypathnames;
	if (listfilesizedirpair.second.size() > 0)
	{
        redundanttopleveldir = true;
        redundantrootdirname = listfilesizedirpair.second.at(0);
		for (int i = 0; i < listfilesizedirpair.second.size(); ++i)
		{
			if (listfilesizedirpair.second.at(i).size() < redundantrootdirname.size())
				redundantrootdirname = listfilesizedirpair.second.at(i);
		}
		if (redundantrootdirname == "boot" ||
			redundantrootdirname == "syslinux" ||
			redundantrootdirname == "grub" ||
			redundantrootdirname == "isolinux" ||
			redundantrootdirname == "extlinux" ||
			redundantrootdirname == "pxelinux")
			redundanttopleveldir = false;
		for (int i = 0; i < listfilesizedirpair.second.size(); ++i) // redundant toplevel path in dirs
		{
			if (!listfilesizedirpair.second.at(i).startsWith(redundantrootdirname))
				redundanttopleveldir = false;
		}
		if (redundanttopleveldir)
		{
			for (int i = 0; i < listfilesizedirpair.first.first.size(); ++i) // redundant toplevel path in files
			{
				if (!listfilesizedirpair.first.first.at(i).startsWith(redundantrootdirname))
					redundanttopleveldir = false;
			}
		}
		if (redundanttopleveldir)
		{
			int rmnumcharsrootdir = redundantrootdirname.size();
			for (int i = 0; i < listfilesizedirpair.second.size(); ++i)
			{
				QString tmpdirectpath = listfilesizedirpair.second[i];
				tmpdirectpath.remove(0, rmnumcharsrootdir);
				if (tmpdirectpath.startsWith("/") || tmpdirectpath.startsWith(QDir::toNativeSeparators("/")))
				{
					tmpdirectpath.remove(0, 1);
				}
				if (!tmpdirectpath.isEmpty())
				{
					directorypathnames.append(tmpdirectpath);
				}
			}
			for (int i = 0; i < listfilesizedirpair.first.first.size(); ++i)
			{
				filepathnames[i].remove(0, rmnumcharsrootdir);
				if (filepathnames[i].startsWith("/") || filepathnames[i].startsWith(QDir::toNativeSeparators("/")))
				{
					filepathnames[i].remove(0, 1);
				}
			}
		}
		else
		{
			directorypathnames = listfilesizedirpair.second;
		}
	}
	if (installType == tr("USB Drive"))
	{
		QStringList syslinuxfilepaths = QStringList() << "boot/syslinux/syslinux.cfg" << "syslinux/syslinux.cfg" << "syslinux.cfg" << "isolinux.cfg" << "extlinux.conf";
		if (QString(QDir::toNativeSeparators("/")) != QString("/"))
		{
            syslinuxfilepaths << QString("boot%1syslinux%1syslinux.cfg").arg(QDir::toNativeSeparators("/")) <<  QString("syslinux%1syslinux.cfg").arg(QDir::toNativeSeparators("/"));
		}
		for (int j = 0; j < syslinuxfilepaths.size(); ++j)
		{
			rmFile(QString("%1%2").arg(targetPath).arg(syslinuxfilepaths.at(j)));
			for (int i = 0; i < filepathnames.size(); ++i)
			{
				if (filepathnames.at(i) == syslinuxfilepaths.at(j))
				{
					dontgeneratesyslinuxcfg = true;
					locatedsyslinuxcfgfiles.append(syslinuxfilepaths.at(j));
				}
			}
		}
	}
	if (!dontgeneratesyslinuxcfg)
	{
	kernelOpts = extractcfg(isofile, listfilesizedirpair.first.first);
	issalt = (kernelOpts == SALT_DETECTED);
	if (issalt)
	{
		QStringList mlstfoundfiles = listfilesizedirpair.second.filter(QRegExp("/modules$", Qt::CaseInsensitive));
		if (!mlstfoundfiles.isEmpty())
		{
      QString match = mlstfoundfiles.at(0);
			saltRootDir = match.replace(QRegExp("(.*)/modules"), "\\1");
		}
		kernelLine = "linux";
		kernelOpts = "";
		initrdLoc = "";
	  initrdOpts = "";
	  initrdLine = "";
	  slinitrdLine = "";
	}
	else
	{
	extraoptionsPL = extractcfgL(isofile, listfilesizedirpair.first.first);
	}

#ifndef NOEXTRACTKERNEL
	extractkernel(isofile, QString("%1ubnkern").arg(targetPath), listfilesizedirpair.first);
#endif
#ifndef NOEXTRACTINITRD
	if (!issalt)
	{
	extractinitrd(isofile, QString("%1ubninit").arg(targetPath), listfilesizedirpair.first);
	}
#endif
	}
	QStringList createdpaths = makepathtree(targetDrive, directorypathnames);
	QFile ubnpathlF(QDir::toNativeSeparators(QString("%1ubnpathl.txt").arg(targetPath)));
	if (ubnpathlF.exists())
	{
		rmFile(ubnpathlF);
	}
	ubnpathlF.open(QIODevice::WriteOnly | QIODevice::Text);
	QTextStream ubnpathlS(&ubnpathlF);
	for (int i = createdpaths.size() - 1; i > -1; i--)
	{
		ubnpathlS << createdpaths.at(i) << endl;
	}
	ubnpathlF.close();
    QStringList extractedfiles;
    if (!skipExtraction)
      extractedfiles = extractallfiles(isofile, targetDrive, listfilesizedirpair.first, filepathnames);
	QFile ubnfilelF(QDir::toNativeSeparators(QString("%1ubnfilel.txt").arg(targetPath)));
	if (ubnfilelF.exists())
	{
		rmFile(ubnfilelF);
	}
	ubnfilelF.open(QIODevice::WriteOnly | QIODevice::Text);
	QTextStream ubnfilelS(&ubnfilelF);
	for (int i = 0; i < extractedfiles.size(); ++i)
	{
		ubnfilelS << extractedfiles.at(i) << endl;
	}
	ubnfilelF.close();
#ifdef XPUD
	rmFile(QString("%1boot.cat").arg(targetPath));
	rmFile(QString("%1isolinux.bin").arg(targetPath));
	rmFile(QString("%1syslinux.cfg").arg(targetPath));
	QFile::rename(QString("%1isolinux.cfg").arg(targetPath), QString("%1syslinux.cfg").arg(targetPath));
	if (installType == tr("USB Drive"))
	{
		rmFile(QString("%1ubnfilel.txt").arg(targetPath));
		rmFile(QString("%1ubnpathl.txt").arg(targetPath));
	}
#endif
}

void unetbootin::extractiso_krd10(QString isofile)
{
	if (!sdesc2->text().contains(trcurrent))
	{
		sdesc1->setText(QString(sdesc1->text()).remove("<b>").replace(trcurrent+"</b>", trdone));
		sdesc2->setText(QString("<b>%1 %2</b>").arg(sdesc2->text()).arg(trdone));
	}
	tprogress->setValue(0);
	QPair<QPair<QStringList, QList<quint64> >, QStringList> listfilesizedirpair = listarchiveconts(isofile);
	kernelLoc = QDir::fromNativeSeparators(locatekernel(isofile, listfilesizedirpair.first));
	if (!kernelLoc.startsWith("/")) kernelLoc.prepend("/");
	initrdLoc = QDir::fromNativeSeparators(locateinitrd(isofile, listfilesizedirpair.first));
	if (!initrdLoc.startsWith("/")) initrdLoc.prepend("/");
	kernelOpts = extractcfg(isofile, listfilesizedirpair.first.first);
	extraoptionsPL.first.first.clear();
	extraoptionsPL.first.second.clear();
	extraoptionsPL.second.first.clear();
	extraoptionsPL.second.second.clear();
	QPair<QPair<QStringList, QStringList>, QPair<QStringList, QStringList> > tmpoptionsL = extractcfgL(isofile, listfilesizedirpair.first.first);
	for (int i = 0; i < tmpoptionsL.second.second.size(); ++i)
	{
		if (!tmpoptionsL.second.second.at(i).isEmpty())
		{
			extraoptionsPL.first.first.append(tmpoptionsL.first.first.at(i));
			extraoptionsPL.first.second.append(tmpoptionsL.first.second.at(i));
			extraoptionsPL.second.first.append(tmpoptionsL.second.first.at(i));
			extraoptionsPL.second.second.append(tmpoptionsL.second.second.at(i));
		}
	}
	QPair<QStringList, QList<quint64> > bootfiles;
	for (int i = 0; i < listfilesizedirpair.first.first.size(); ++i)
	{
		if (listfilesizedirpair.first.first.at(i).startsWith("boot", Qt::CaseInsensitive))
		{
			bootfiles.first.append(listfilesizedirpair.first.first.at(i));
			bootfiles.second.append(listfilesizedirpair.first.second.at(i));
		}
	}
	QStringList bootpaths;
	for (int i = 0; i < listfilesizedirpair.second.size(); ++i)
	{
		if (listfilesizedirpair.second.at(i).startsWith("boot", Qt::CaseInsensitive))
		{
			bootpaths.append(listfilesizedirpair.second.at(i));
		}
	}
	if (!bootpaths.contains("rescue"))
		bootpaths.append("rescue");
	QStringList createdpaths = makepathtree(targetDrive, bootpaths);
	QFile ubnpathlF(QDir::toNativeSeparators(QString("%1ubnpathl.txt").arg(targetPath)));
	if (ubnpathlF.exists())
	{
		rmFile(ubnpathlF);
	}
	ubnpathlF.open(QIODevice::WriteOnly | QIODevice::Text);
	QTextStream ubnpathlS(&ubnpathlF);
	for (int i = createdpaths.size() - 1; i > -1; i--)
	{
		ubnpathlS << createdpaths.at(i) << endl;
	}
	ubnpathlF.close();
	QStringList extractedfiles = extractallfiles(isofile, targetDrive, bootfiles, listfilesizedirpair.first.first);
	if (QFile::exists(QString("%1liveusb").arg(targetDrive)))
		overwritefileprompt(QString("%1liveusb").arg(targetDrive));
	else
		extractedfiles.append(QString("%1liveusb").arg(targetDrive));
	QFile(QString("%1liveusb").arg(targetDrive)).open(QIODevice::WriteOnly);
	pdesc1->setText(QString("Copying %1 to %2").arg(isofile).arg(QString("%1rescue%2rescue.iso").arg(targetDrive).arg(QDir::toNativeSeparators("/"))));
	if (QFile::exists(QString("%1rescue%2rescue.iso").arg(targetDrive).arg(QDir::toNativeSeparators("/"))))
		overwritefileprompt(QString("%1rescue%2rescue.iso").arg(targetDrive).arg(QDir::toNativeSeparators("/")));
	else
		extractedfiles.append(QString("%1rescue%2rescue.iso").arg(targetDrive).arg(QDir::toNativeSeparators("/")));
	//QFile::copy(isofile, QString("%1rescue%2rescue.iso").arg(targetDrive).arg(QDir::toNativeSeparators("/")));
	copyfilegui(isofile, QString("%1rescue%2rescue.iso").arg(targetDrive).arg(QDir::toNativeSeparators("/")));
	QFile ubnfilelF(QDir::toNativeSeparators(QString("%1ubnfilel.txt").arg(targetPath)));
	if (ubnfilelF.exists())
	{
		rmFile(ubnfilelF);
	}
	ubnfilelF.open(QIODevice::WriteOnly | QIODevice::Text);
	QTextStream ubnfilelS(&ubnfilelF);
	for (int i = 0; i < extractedfiles.size(); ++i)
	{
		ubnfilelS << extractedfiles.at(i) << endl;
	}
	ubnfilelF.close();
}

void unetbootin::copyfilegui(QString src, QString dst)
{
	pdesc5->setText("");
	pdesc4->setText(tr("Copying file, please wait..."));
	pdesc3->setText(tr("<b>Source:</b> <a href=\"%1\">%1</a>").arg(src));
	pdesc2->setText(tr("<b>Destination:</b> %1").arg(dst));
	pdesc1->setText(tr("<b>Copied:</b> 0 bytes"));
	QEventLoop cpfw;
	copyfileT cpft;
	cpft.source = src;
	cpft.destination = dst;
	connect(&cpft, SIGNAL(datacopied64(qint64,qint64)), this, SLOT(cpprogressupdate64(qint64,qint64)));
	connect(&cpft, SIGNAL(finished()), &cpfw, SLOT(quit()));
	cpft.start();
	cpfw.exec();
	pdesc4->setText("");
	pdesc3->setText("");
	pdesc2->setText("");
	pdesc1->setText("");
	tprogress->setValue(0);
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

QStringList unetbootin::extractallfiles(QString archivefile, QString dirxfilesto, QPair<QStringList, QList<quint64> > filesizelist, QStringList outputfilelist)
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
		pdesc2->setText(tr("<b>Destination:</b> %1%2").arg(dirxfilesto).arg(outputfilelist.at(i)));
		pdesc1->setText(tr("<b>Extracted:</b> %1 of %2 files").arg(i).arg(filelist.size()));
		tprogress->setValue(i);
		if (extractfile(filelist.at(i), QString("%1%2").arg(dirxfilesto).arg(outputfilelist.at(i)), archivefile))
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
			return fixkernelbootoptions(QString(cfgfileCL).remove(QRegExp("^kernel\\s{1,}\\S{1,}\\s{1,}", Qt::CaseInsensitive)));
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
				titleandparams.second[curindex] = fixkernelbootoptions(QString(cfgfileCL).remove(QRegExp("^kernel\\s{1,}\\S{1,}\\s{1,}", Qt::CaseInsensitive)));
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

void unetbootin::loadgrub2env(QString cfgfile)
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
		if (cfgfileCL.contains("${"))
		{
			for (QMap<QString, QString>::const_iterator i = grub2vars.begin(); i != grub2vars.end(); ++i)
			{
				if (cfgfileCL.contains(QString("${%1}").arg(i.key())))
					cfgfileCL.replace(QString("${%1}").arg(i.key()), i.value());
			}
		}
		if (cfgfileCL.contains(QRegExp("^set\\s{1,}\\S{1,}\\s{0,}=\\s{0,}\".{1,}\"")))
		{
			QRegExp setrg("^set\\s{1,}(\\S{1,})\\s{0,}=\\s{0,}\"(.{1,})\"");
			setrg.indexIn(cfgfileCL);
			QStringList captxt = setrg.capturedTexts();
			if (captxt.size() >= 2)
			{
				grub2vars[captxt.at(captxt.size()-2)] = captxt.at(captxt.size()-1);
				continue;
			}
		}
		if (cfgfileCL.contains(QRegExp("^set\\s{1,}\\S{1,}\\s{0,}=\\s{0,}\\S{1,}")))
		{
			QRegExp setrg("^set\\s{1,}(\\S{1,})\\s{0,}=\\s{0,}(\\S{1,})");
			setrg.indexIn(cfgfileCL);
			QStringList captxt = setrg.capturedTexts();
			if (captxt.size() >= 2)
			{
				grub2vars[captxt.at(captxt.size()-2)] = captxt.at(captxt.size()-1);
				continue;
			}
		}
		if (cfgfileCL.count("=") == 1)
		{
			QStringList splp = cfgfileCL.split("=");
			if (splp.size() == 2)
			{
				grub2vars[splp.at(0).trimmed()] = QString(splp.at(1)).remove("\"").trimmed();
			}
		}
	}
}

QString unetbootin::getgrub2cfgargs(QString cfgfile, QString archivefile, QStringList archivefileconts, QStringList visitedincludes)
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
		if (cfgfileCL.contains("${"))
		{
			for (QMap<QString, QString>::const_iterator i = grub2vars.begin(); i != grub2vars.end(); ++i)
			{
				if (cfgfileCL.contains(QString("${%1}").arg(i.key())))
					cfgfileCL.replace(QString("${%1}").arg(i.key()), i.value());
			}
		}
		if (cfgfileCL.contains(QRegExp("^set\\s{1,}\\S{1,}\\s{0,}=\\s{0,}\".{1,}\"")))
		{
			QRegExp setrg("^set\\s{1,}(\\S{1,})\\s{0,}=\\s{0,}\"(.{1,})\"");
			setrg.indexIn(cfgfileCL);
			QStringList captxt = setrg.capturedTexts();
			if (captxt.size() >= 2)
			{
				grub2vars[captxt.at(captxt.size()-2)] = captxt.at(captxt.size()-1);
				continue;
			}
		}
		if (cfgfileCL.contains(QRegExp("^set\\s{1,}\\S{1,}\\s{0,}=\\s{0,}\\S{1,}")))
		{
			QRegExp setrg("^set\\s{1,}(\\S{1,})\\s{0,}=\\s{0,}(\\S{1,})");
			setrg.indexIn(cfgfileCL);
			QStringList captxt = setrg.capturedTexts();
			if (captxt.size() >= 2)
			{
				grub2vars[captxt.at(captxt.size()-2)] = captxt.at(captxt.size()-1);
				continue;
			}
		}
		if (!archivefileconts.isEmpty() && QRegExp("^configfile\\s{1,}\\S{1,}.cfg$", Qt::CaseInsensitive).exactMatch(cfgfileCL))
		{
			includesfile = QDir::toNativeSeparators(QString(cfgfileCL).remove(QRegExp("^configfile\\s{1,}", Qt::CaseInsensitive))).trimmed();
			searchincfrs = searchforgrub2includesfile(includesfile, archivefile, archivefileconts, visitedincludes).trimmed();
			if (!searchincfrs.isEmpty())
				return searchincfrs;
		}
		if (!archivefileconts.isEmpty() && QRegExp("^source\\s{1,}\\S{1,}.cfg$", Qt::CaseInsensitive).exactMatch(cfgfileCL))
		{
			includesfile = QDir::toNativeSeparators(QString(cfgfileCL).remove(QRegExp("^source\\s{1,}", Qt::CaseInsensitive))).trimmed();
			searchincfrs = searchforgrub2includesfile(includesfile, archivefile, archivefileconts, visitedincludes).trimmed();
			if (!searchincfrs.isEmpty())
				return searchincfrs;
		}
		if (cfgfileCL.contains(QRegExp("^linux\\s{1,}\\S{1,}\\s{1,}\\S{1,}", Qt::CaseInsensitive)))
		{
			return fixkernelbootoptions(QString(cfgfileCL).remove(QRegExp("^linux\\s{1,}\\S{1,}\\s{1,}", Qt::CaseInsensitive))).remove(QRegExp("initrd=\\S{0,}")).trimmed();
		}
	}
	return "";
}

QPair<QPair<QStringList, QStringList>, QPair<QStringList, QStringList> > unetbootin::getgrub2cfgargsL(QString cfgfile, QString archivefile, QStringList archivefileconts, QStringList visitedincludes)
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
	QString includesfile;
	QPair<QPair<QStringList, QStringList>, QPair<QStringList, QStringList> > searchincfrs;
	while (!cfgfileS.atEnd())
	{
		cfgfileCL = cfgfileS.readLine().trimmed();
		if (cfgfileCL.contains("#"))
		{
			cfgfileCL = cfgfileCL.left(cfgfileCL.indexOf("#")).trimmed();
		}
		if (cfgfileCL.contains("${"))
		{
			for (QMap<QString, QString>::const_iterator i = grub2vars.begin(); i != grub2vars.end(); ++i)
			{
				if (cfgfileCL.contains(QString("${%1}").arg(i.key())))
					cfgfileCL.replace(QString("${%1}").arg(i.key()), i.value());
			}
		}
		if (cfgfileCL.contains(QRegExp("^set\\s{1,}\\S{1,}\\s{0,}=\\s{0,}\".{1,}\"")))
		{
			QRegExp setrg("^set\\s{1,}(\\S{1,})\\s{0,}=\\s{0,}\"(.{1,})\"");
			setrg.indexIn(cfgfileCL);
			QStringList captxt = setrg.capturedTexts();
			if (captxt.size() >= 2)
			{
				grub2vars[captxt.at(captxt.size()-2)] = captxt.at(captxt.size()-1);
				continue;
			}
		}
		if (cfgfileCL.contains(QRegExp("^set\\s{1,}\\S{1,}\\s{0,}=\\s{0,}\\S{1,}")))
		{
			QRegExp setrg("^set\\s{1,}(\\S{1,})\\s{0,}=\\s{0,}(\\S{1,})");
			setrg.indexIn(cfgfileCL);
			QStringList captxt = setrg.capturedTexts();
			if (captxt.size() >= 2)
			{
				grub2vars[captxt.at(captxt.size()-2)] = captxt.at(captxt.size()-1);
				continue;
			}
		}
		if (!archivefileconts.isEmpty() && QRegExp("^configfile\\s{1,}\\S{1,}.cfg$", Qt::CaseInsensitive).exactMatch(cfgfileCL))
		{
			includesfile = QDir::toNativeSeparators(QString(cfgfileCL).remove(QRegExp("^configfile\\s{1,}", Qt::CaseInsensitive))).trimmed();
			searchincfrs = searchforgrub2includesfileL(includesfile, archivefile, archivefileconts, visitedincludes);
			if (!searchincfrs.first.first.isEmpty())
			{
				kernelandinitrd.first += searchincfrs.first.first;
				kernelandinitrd.second += searchincfrs.first.second;
				titleandparams.first += searchincfrs.second.first;
				titleandparams.second += searchincfrs.second.second;
			}
			continue;
		}
		if (!archivefileconts.isEmpty() && QRegExp("^source\\s{1,}\\S{1,}.cfg$", Qt::CaseInsensitive).exactMatch(cfgfileCL))
		{
			includesfile = QDir::toNativeSeparators(QString(cfgfileCL).remove(QRegExp("^source\\s{1,}", Qt::CaseInsensitive))).trimmed();
			searchincfrs = searchforgrub2includesfileL(includesfile, archivefile, archivefileconts, visitedincludes);
			if (!searchincfrs.first.first.isEmpty())
			{
				kernelandinitrd.first += searchincfrs.first.first;
				kernelandinitrd.second += searchincfrs.first.second;
				titleandparams.first += searchincfrs.second.first;
				titleandparams.second += searchincfrs.second.second;
			}
			continue;
		}
		if (cfgfileCL.contains(QRegExp("^menuentry\\s{1,}\".{1,}\"", Qt::CaseInsensitive)))
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
			titleandparams.first[curindex] = QString(cfgfileCL).remove("menuentry", Qt::CaseInsensitive).remove("\"").remove("{").remove("}").trimmed();
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
		if (cfgfileCL.contains(QRegExp("^linux\\s{1,}\\S{1,}", Qt::CaseInsensitive)))
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
			if (cfgfileCL.contains(QRegExp("^linux\\s{1,}\\S{1,}\\s{1,}\\S{1,}", Qt::CaseInsensitive)))
			{
				titleandparams.second[curindex] = fixkernelbootoptions(QString(cfgfileCL).remove(QRegExp("^linux\\s{1,}\\S{1,}\\s{1,}", Qt::CaseInsensitive))).remove(QRegExp("initrd=\\S{0,}")).trimmed();
			}
			kernelandinitrd.first[curindex] = getFirstTextBlock(QString(cfgfileCL).remove(QRegExp("^linux", Qt::CaseInsensitive)).trimmed());
//			if (kernelandinitrd.first.at(curindex).isEmpty())
//				kernelandinitrd.first[curindex] = kernelLoc;
			kernelpassed = true;
			continue;
		}
	}
	return qMakePair(kernelandinitrd, titleandparams);
}

QString unetbootin::getcfgkernargs(QString cfgfile, QString archivefile, QStringList archivefileconts, QStringList visitedincludes)
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
			searchincfrs = searchforincludesfile(includesfile, archivefile, archivefileconts, visitedincludes).trimmed();
			if (!searchincfrs.isEmpty())
				return searchincfrs;
		}
		if (!archivefileconts.isEmpty() && QRegExp("^append\\s{1,}\\S{1,}.cfg$", Qt::CaseInsensitive).exactMatch(cfgfileCL))
		{
			includesfile = QDir::toNativeSeparators(QString(cfgfileCL).remove(QRegExp("^append\\s{1,}", Qt::CaseInsensitive))).trimmed();
			searchincfrs = searchforincludesfile(includesfile, archivefile, archivefileconts, visitedincludes).trimmed();
			if (!searchincfrs.isEmpty())
				return searchincfrs;
		}
		else if (cfgfileCL.contains(QRegExp("^\\s{0,}append\\s{1,}", Qt::CaseInsensitive)))
		{
			return fixkernelbootoptions(QString(cfgfileCL).remove(QRegExp("\\s{0,}append\\s{1,}", Qt::CaseInsensitive)).remove(QRegExp("\\s{0,1}initrd=\\S{0,}", Qt::CaseInsensitive)));
		}
	}
	return "";
}

QPair<QPair<QStringList, QStringList>, QPair<QStringList, QStringList> > unetbootin::getcfgkernargsL(QString cfgfile, QString archivefile, QStringList archivefileconts, QStringList visitedincludes)
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
			searchincfrs = searchforincludesfileL(includesfile, archivefile, archivefileconts, visitedincludes);
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
			searchincfrs = searchforincludesfileL(includesfile, archivefile, archivefileconts, visitedincludes);
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
			titleandparams.second[curindex] = fixkernelbootoptions(appendoptsL);
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
		if (cfgfileCL.contains(QRegExp("^initrd\\s{1,}\\S{1,}", Qt::CaseInsensitive)))
		{
			if (kernelandinitrd.second[curindex] == initrdLoc) {
				kernelandinitrd.second[curindex] = getFirstTextBlock(QString(cfgfileCL).remove(QRegExp("^initrd", Qt::CaseInsensitive)).trimmed());
			}
			continue;
		}
	}
	return qMakePair(kernelandinitrd, titleandparams);
}

QString unetbootin::searchforincludesfile(QString includesfile, QString archivefile, QStringList archivefileconts, QStringList visitedincludes)
{
	if (includesfile.startsWith(QDir::toNativeSeparators("/")))
	{
		includesfile = includesfile.right(includesfile.size() - 1).trimmed();
	}
	QStringList includesfileL;
	for (int i = 0; i < archivefileconts.size(); ++i)
	{
		QString curentry = archivefileconts.at(i);
		if (curentry.endsWith("/") || curentry.endsWith(QDir::toNativeSeparators("/")))
		{
			curentry = curentry.left(curentry.size() - 1).trimmed();
		}
		if (curentry.contains("/"))
		{
			curentry = curentry.right(curentry.size() - 1 - curentry.indexOf("/"));
		}
		else if (curentry.contains(QDir::toNativeSeparators("/")))
		{
			curentry = curentry.right(curentry.size() - 1 - curentry.indexOf(QDir::toNativeSeparators("/")));
		}
		if (includesfile.compare(curentry, Qt::CaseInsensitive) == 0)
		{
			includesfileL.append(archivefileconts.at(i));
		}
	}
	if (includesfileL.isEmpty())
	{
		includesfileL = archivefileconts.filter(includesfile, Qt::CaseInsensitive);
	}
	if (!includesfileL.isEmpty())
	{
		for (int i = 0; i < includesfileL.size(); ++i)
		{
			if (visitedincludes.contains(includesfileL.at(i)))
				continue;
			randtmpfile tmpoutputcfgf(ubntmpf, "cfg");
            extractfile(includesfileL.at(i), tmpoutputcfgf.fileName(), archivefile);
			QStringList nextinclude = visitedincludes;
			nextinclude.append(includesfileL.at(i));
			QString extractcfgtmp = getcfgkernargs(tmpoutputcfgf.fileName(), archivefile, archivefileconts, nextinclude).trimmed();
			rmFile(tmpoutputcfgf);
			if (!extractcfgtmp.isEmpty())
			{
				return extractcfgtmp;
			}
		}
	}
	return "";
}

QPair<QPair<QStringList, QStringList>, QPair<QStringList, QStringList> > unetbootin::searchforincludesfileL(QString includesfile, QString archivefile, QStringList archivefileconts, QStringList visitedincludes)
{
	if (includesfile.startsWith(QDir::toNativeSeparators("/")))
	{
		includesfile = includesfile.right(includesfile.size() - 1).trimmed();
	}
	QStringList includesfileL;
	for (int i = 0; i < archivefileconts.size(); ++i)
	{
		QString curentry = archivefileconts.at(i);
		if (curentry.endsWith("/") || curentry.endsWith(QDir::toNativeSeparators("/")))
		{
			curentry = curentry.left(curentry.size() - 1).trimmed();
		}
		if (curentry.contains("/"))
		{
			curentry = curentry.right(curentry.size() - 1 - curentry.indexOf("/"));
		}
		else if (curentry.contains(QDir::toNativeSeparators("/")))
		{
			curentry = curentry.right(curentry.size() - 1 - curentry.indexOf(QDir::toNativeSeparators("/")));
		}
		if (includesfile.compare(curentry, Qt::CaseInsensitive) == 0)
		{
			includesfileL.append(archivefileconts.at(i));
		}
	}
	if (includesfileL.isEmpty())
	{
		includesfileL = archivefileconts.filter(includesfile, Qt::CaseInsensitive);
	}
	if (!includesfileL.isEmpty())
	{
		for (int i = 0; i < includesfileL.size(); ++i)
		{
			if (visitedincludes.contains(includesfileL.at(i)))
				continue;
			randtmpfile tmpoutputcfgf(ubntmpf, "cfg");
			extractfile(includesfileL.at(i), tmpoutputcfgf.fileName(), archivefile);
			QStringList nextinclude = visitedincludes;
			nextinclude.append(includesfileL.at(i));
            QPair<QPair<QStringList, QStringList>, QPair<QStringList, QStringList> > extractcfgtmp = getcfgkernargsL(tmpoutputcfgf.fileName(), archivefile, archivefileconts, nextinclude);
			rmFile(tmpoutputcfgf);
			if (!extractcfgtmp.first.first.isEmpty())
			{
				return extractcfgtmp;
			}
		}
	}
	return QPair<QPair<QStringList, QStringList>, QPair<QStringList, QStringList> >();
}

QString unetbootin::searchforgrub2includesfile(QString includesfile, QString archivefile, QStringList archivefileconts, QStringList visitedincludes)
{
	if (includesfile.startsWith(QDir::toNativeSeparators("/")))
	{
		includesfile = includesfile.right(includesfile.size() - 1).trimmed();
	}
	QStringList includesfileL;
	for (int i = 0; i < archivefileconts.size(); ++i)
	{
		QString curentry = archivefileconts.at(i);
		if (curentry.endsWith("/") || curentry.endsWith(QDir::toNativeSeparators("/")))
		{
			curentry = curentry.left(curentry.size() - 1).trimmed();
		}
		if (curentry.contains("/"))
		{
			curentry = curentry.right(curentry.size() - 1 - curentry.indexOf("/"));
		}
		else if (curentry.contains(QDir::toNativeSeparators("/")))
		{
			curentry = curentry.right(curentry.size() - 1 - curentry.indexOf(QDir::toNativeSeparators("/")));
		}
		if (includesfile.compare(curentry, Qt::CaseInsensitive) == 0)
		{
			includesfileL.append(archivefileconts.at(i));
		}
	}
	if (includesfileL.isEmpty())
	{
		includesfileL = archivefileconts.filter(includesfile, Qt::CaseInsensitive);
	}
	if (!includesfileL.isEmpty())
	{
		for (int i = 0; i < includesfileL.size(); ++i)
		{
			if (visitedincludes.contains(includesfileL.at(i)))
				continue;
			randtmpfile tmpoutputcfgf(ubntmpf, "cfg");
			extractfile(includesfileL.at(i), tmpoutputcfgf.fileName(), archivefile);
			QStringList nextinclude = visitedincludes;
			nextinclude.append(includesfileL.at(i));
			QString extractcfgtmp = getgrub2cfgargs(tmpoutputcfgf.fileName(), archivefile, archivefileconts, nextinclude).trimmed();
			rmFile(tmpoutputcfgf);
			if (!extractcfgtmp.isEmpty())
			{
				return extractcfgtmp;
			}
		}
	}
	return "";
}

QPair<QPair<QStringList, QStringList>, QPair<QStringList, QStringList> > unetbootin::searchforgrub2includesfileL(QString includesfile, QString archivefile, QStringList archivefileconts, QStringList visitedincludes)
{
	if (includesfile.startsWith(QDir::toNativeSeparators("/")))
	{
		includesfile = includesfile.right(includesfile.size() - 1).trimmed();
	}
	QStringList includesfileL;
	for (int i = 0; i < archivefileconts.size(); ++i)
	{
		QString curentry = archivefileconts.at(i);
		if (curentry.endsWith("/") || curentry.endsWith(QDir::toNativeSeparators("/")))
		{
			curentry = curentry.left(curentry.size() - 1).trimmed();
		}
		if (curentry.contains("/"))
		{
			curentry = curentry.right(curentry.size() - 1 - curentry.indexOf("/"));
		}
		else if (curentry.contains(QDir::toNativeSeparators("/")))
		{
			curentry = curentry.right(curentry.size() - 1 - curentry.indexOf(QDir::toNativeSeparators("/")));
		}
		if (includesfile.compare(curentry, Qt::CaseInsensitive) == 0)
		{
			includesfileL.append(archivefileconts.at(i));
		}
	}
	if (includesfileL.isEmpty())
	{
		includesfileL = archivefileconts.filter(includesfile, Qt::CaseInsensitive);
	}
	if (!includesfileL.isEmpty())
	{
		for (int i = 0; i < includesfileL.size(); ++i)
		{
			if (visitedincludes.contains(includesfileL.at(i)))
				continue;
			randtmpfile tmpoutputcfgf(ubntmpf, "cfg");
			extractfile(includesfileL.at(i), tmpoutputcfgf.fileName(), archivefile);
			QStringList nextinclude = visitedincludes;
			nextinclude.append(includesfileL.at(i));
			QPair<QPair<QStringList, QStringList>, QPair<QStringList, QStringList> > extractcfgtmp = getgrub2cfgargsL(tmpoutputcfgf.fileName(), archivefile, archivefileconts, nextinclude);
			rmFile(tmpoutputcfgf);
			if (!extractcfgtmp.first.first.isEmpty())
			{
				return extractcfgtmp;
			}
		}
	}
	return QPair<QPair<QStringList, QStringList>, QPair<QStringList, QStringList> >();
}

void unetbootin::downloadfile(QString fileurl, QString targetfile, int minsize=524288)
{
	if (fileurl.isEmpty())
	{
		showDownloadFailedScreen(fileurl);
		return;
	}
	if (QFile::exists(targetfile))
	{
		rmFile(targetfile);
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
			rmFile(dloutfile);
			downloadfile(dlresponse.value("Location"), targetfile, minsize);
			return;
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
    if (QFile(targetfile).size() <= 4096)
    {
        QString redirectTargetURL;
        QFile seeRedirect(targetfile);
        seeRedirect.open(QIODevice::ReadOnly | QIODevice::Text);
        QTextStream seeRedirectTextStream(&seeRedirect);
        while (!seeRedirectTextStream.atEnd())
        {
            QString curline = seeRedirectTextStream.readLine();
            if (curline.contains("content=\"0;url="))
            {
                int urlstartidx = curline.indexOf("content=\"0;url=") + QString("content=\"0;url=").size();
                redirectTargetURL = curline.mid(urlstartidx);
                if (redirectTargetURL.contains("\""))
                {
                    redirectTargetURL = redirectTargetURL.left(redirectTargetURL.indexOf("\""));
                }
                break;
            }
            if (curline.contains("content='0;url="))
            {
                int urlstartidx = curline.indexOf("content='0;url=") + QString("content='0;url=").size();
                redirectTargetURL = curline.mid(urlstartidx);
                if (redirectTargetURL.contains("'"))
                {
                    redirectTargetURL = redirectTargetURL.left(redirectTargetURL.indexOf("'"));
                }
                break;
            }
        }
        seeRedirect.close();
        if (!redirectTargetURL.isEmpty())
        {
            rmFile(targetfile);
            downloadfile(redirectTargetURL, targetfile, minsize);
            return;
        }
    }
	if (QFile(targetfile).size() < minsize)
	{
		// download failed
		showDownloadFailedScreen(fileurl);
		return;
	}
	pdesc4->setText("");
	pdesc3->setText("");
	pdesc2->setText("");
	pdesc1->setText("");
	tprogress->setValue(0);
	if (testingDownload)
	{
		// Note that this only tests that the first download succeeds
		printf("exitstatus:downloadcomplete\n");
		QApplication::exit();
		exit(0);
	}
}

void unetbootin::showDownloadFailedScreen(const QString &fileurl)
{
	progresslayer->setEnabled(false);
	progresslayer->hide();
	rebootlayer->setEnabled(true);
	rebootlayer->show();
	rebootmsgtext->setText(tr("Download of %1 %2 from %3 failed. Please try downloading the ISO file from the website directly and supply it via the diskimage option.").arg(nameDistro).arg(nameVersion).arg(fileurl));
	this->downloadFailed = true;
	if (exitOnCompletion)
	{
		printf("exitstatus:downloadfailed\n");
		QApplication::exit();
		exit(0);
	}
}

void unetbootin::dlprogressupdate(int dlbytes, int maxbytes)
{
 QTime time = QTime::currentTime();
 static int oldsec = 0;
 // refresh the progress bar every second
 if(oldsec != time.second())
 {
   oldsec = time.second();
     tprogress->setValue(dlbytes);
     tprogress->setMaximum(maxbytes);
   // display the downloaded size with suffix
     pdesc1->setText(tr("<b>Downloaded:</b> %1 of %2").arg(displayfisize(dlbytes)).arg(displayfisize(maxbytes)));
 }
}

void unetbootin::dlprogressupdate64(qint64 dlbytes, qint64 maxbytes)
{
 QTime time = QTime::currentTime();
 static int oldsec = 0;
 // refresh the progress bar every second
 if(oldsec != time.second())
 {
   oldsec = time.second();
     tprogress->setValue(dlbytes);
     tprogress->setMaximum(maxbytes);
   // display the downloaded size with suffix
     pdesc1->setText(tr("<b>Downloaded:</b> %1 of %2").arg(displayfisize(dlbytes)).arg(displayfisize(maxbytes)));
 }
}

void unetbootin::cpprogressupdate64(qint64 dlbytes, qint64 maxbytes)
{
 QTime time = QTime::currentTime();
 static int oldsec = 0;
 // refresh the progress bar every second
 if(oldsec != time.second())
 {
   oldsec = time.second();
	 tprogress->setValue(dlbytes);
	 tprogress->setMaximum(maxbytes);
   // display the downloaded size with suffix
	 pdesc1->setText(tr("<b>Copied:</b> %1 of %2").arg(displayfisize(dlbytes)).arg(displayfisize(maxbytes)));
 }
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
	QStringList relativelinksLPreFilter =
		downloadpagecontents(ldfDirStringUrl)
		.replace(">", ">\n")
		.replace("<", "\n<")
		.split("\n");
	QStringList relativelinksLPart1 =
		relativelinksLPreFilter
		.filter(QRegExp("<a href=\"(?!\\?)\\S{1,}\">", Qt::CaseInsensitive))
		.replaceInStrings(QRegExp("<a href=\"", Qt::CaseInsensitive), "")
		.replaceInStrings("\">", "");
	QStringList relativelinksLPart2 =
		relativelinksLPreFilter
		.filter(QRegExp("<a href=\'(?!\\?)\\S{1,}\'>", Qt::CaseInsensitive))
		.replaceInStrings(QRegExp("<a href=\'", Qt::CaseInsensitive), "")
		.replaceInStrings("\'>", "");
	QStringList relativelinksL = relativelinksLPart1 << relativelinksLPart2;
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

QString unetbootin::callexternappWriteToStdin(QString xexecFile, QString xexecParm, QString xwriteToStdin)
{
	QEventLoop cxaw;
	callexternappWriteToStdinT cxat;
	connect(&cxat, SIGNAL(finished()), &cxaw, SLOT(quit()));
	cxat.execFile = xexecFile;
	cxat.execParm = xexecParm;
	cxat.writeToStdin = xwriteToStdin;
	cxat.start();
	cxaw.exec();
	return cxat.retnValu;
}

QString unetbootin::getdevluid(QString voldrive)
{
#ifdef Q_OS_MAC
	QString diskutilinfo = callexternapp("diskutil", "info " + voldrive);
	QString uuidS = getuuid(voldrive, diskutilinfo);
	if (uuidS == "None")
	{
		return QString("LABEL=%1").arg(getlabel(voldrive, diskutilinfo));
	}
	else
	{
		return QString("UUID=%1").arg(uuidS);
	}
#endif
#ifndef Q_OS_MAC
	QString uuidS = getuuid(voldrive);
	if (uuidS == "None")
	{
		return QString("LABEL=%1").arg(getlabel(voldrive));
	}
	else
	{
		return QString("UUID=%1").arg(uuidS);
	}
#endif
}

#ifdef Q_OS_MAC
QString unetbootin::getlabel(QString voldrive, QString diskutilinfo)
{
	QStringList diskutiloutput = diskutilinfo.split("\n");
	QStringList labelLines = diskutiloutput.filter("Volume Name");
	if (labelLines.size() == 0)
		return "None";
	QStringList labelAtEnd = labelLines.at(0).split(":");
	if (labelAtEnd.size() < 2)
		return "None";
	return labelAtEnd.at(labelAtEnd.size()-1).trimmed();
}
#endif

QString unetbootin::getlabel(QString voldrive)
{
#ifdef Q_OS_MAC
	return getlabel(voldrive, callexternapp("diskutil", "info " + voldrive));
#endif
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
	#ifdef Q_OS_LINUX
	QString volidpS = "";
	if (!volidcommand.isEmpty())
		volidpS = QString(callexternapp(volidcommand, QString("-l %1").arg(voldrive))).remove("\r").remove("\n").trimmed();
	else
	{
		QString tstrblk = QString(callexternapp(blkidcommand, QString("-s LABEL %1").arg(voldrive)));
		if (tstrblk.contains('='))
			volidpS = tstrblk.section('=', -1, -1).remove('"').remove("\r").remove("\n").trimmed();
	}
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

#ifdef Q_OS_MAC
QString unetbootin::getuuid(QString voldrive, QString diskutilinfo)
{
	QStringList diskutiloutput = diskutilinfo.split("\n");
	QStringList uuidList = diskutiloutput.filter("UUID"); // TODO untested
	if (uuidList.size() > 0)
	{
		uuidList = uuidList.at(0).split(" ");
		return uuidList.at(uuidList.size()-1).trimmed();
	}
	// otherwise FAT32 or FAT16; return serial number
	bool isFat32 = diskutiloutput.filter("FAT32").size() > 0;
	if (!isFat32)
	{
		if (diskutiloutput.filter("FAT16").size() == 0 && diskutiloutput.filter("FAT12").size() == 0)
			return "None";
	}
	callexternapp("diskutil", "umount "+targetDev);
	QFile rawDevice(voldrive);
	rawDevice.open(QIODevice::ReadOnly);
	if (isFat32)
	{
		rawDevice.seek(67);
	}
	else // FAT16 or FAT12
	{
		rawDevice.seek(39);
	}
	unsigned char pserial[4];
	rawDevice.read((char*)pserial, 4);
	QString serialNumber = QString::number(pserial[3], 16).rightJustified(2, '0')+QString::number(pserial[2], 16).rightJustified(2, '0')+"-"+QString::number(pserial[1], 16).rightJustified(2, '0')+QString::number(pserial[0], 16).rightJustified(2, '0');
	rawDevice.close();
	callexternapp("diskutil", "mount "+targetDev);
	return serialNumber.toUpper();
}

#endif

QString unetbootin::getuuid(QString voldrive)
{
#ifdef Q_OS_MAC
	return getuuid(voldrive, callexternapp("diskutil", "info " + voldrive));
#endif
	#ifdef Q_OS_WIN32
	voldrive.append("\\");
    DWORD volserialnum = 0;
	GetVolumeInformation(LPWSTR(voldrive.utf16()), NULL, NULL, &volserialnum, NULL, NULL, NULL, NULL);
    if (!(volserialnum >= 1))
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
	#ifdef Q_OS_LINUX
	QString volidpS = "";
	if (!volidcommand.isEmpty())
		volidpS = QString(callexternapp(volidcommand, QString("-u %1").arg(voldrive))).remove("\r").remove("\n").trimmed();
	else
	{
		QString tstrblk = QString(callexternapp(blkidcommand, QString("-s UUID %1").arg(voldrive)));
		if (tstrblk.contains('='))
			volidpS = tstrblk.section('=', -1, -1).remove('"').remove("\r").remove("\n").trimmed();
	}
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
	if (packagename == "silent")
		return "";
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
	QStringList rawdeviceL = QString(callexternapp("mount", "")).replace("\t", " ").split("\n").filter("/dev/").filter(QString(" %1 ").arg(mountpoint));
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
	QStringList procmountsL = callexternapp("mount", "").split("\n");
	for (int i = 0; i < procmountsL.size(); ++i)
	{
		QString mountinfo = procmountsL.at(i).split("\t").join(" ");
		QStringList deviceAndRest = mountinfo.split(" on ");
		if (deviceAndRest.size() < 2)
			continue;
		if (deviceAndRest.at(0).trimmed() != devicenode)
			continue;
		QStringList mountpointAndOptions = deviceAndRest.at(1).split(" (").join(" type ").split(" type ");
		if (mountpointAndOptions.size() < 1)
			continue;
		QString mountpoint = mountpointAndOptions.at(0).trimmed();
		if (QDir(mountpoint).exists())
			return mountpoint;
	}
	return "NOT MOUNTED";
}

QString unetbootin::getGrubNotation(QString devicenode)
{
	return QString("(hd%1,%2)").arg(getDiskNumber(devicenode)).arg(getPartitionNumber(devicenode));
}

QString unetbootin::getGrub2Notation(QString devicenode)
{
	return QString("(hd%1,%2)").arg(getDiskNumber(devicenode)).arg(getPartitionNumber(devicenode)+1);
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
		rmFile(QString("%1\\7z.dll").arg(ubntmpf));
	}
	instIndvfl("sevnz.dll", QString("%1\\7z.dll").arg(ubntmpf));
	if (QFile::exists(QString("%1sevnz.exe").arg(ubntmpf)))
	{
		rmFile(QString("%1sevnz.exe").arg(ubntmpf));
	}
	instIndvfl("sevnz.exe", QString("%1sevnz.exe").arg(ubntmpf));
	sevzcommand = QString("%1sevnz.exe").arg(ubntmpf);
}

void unetbootin::configsysEdit()
{
	SetFileAttributesA(QDir::toNativeSeparators(QString("%1config.sys").arg(targetDrive)).toAscii(), FILE_ATTRIBUTE_NORMAL);
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
		rmFile(configsysFile);
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
		rmFile(bootiniFile);
		QFile::copy(QString("%1bootnw.txt").arg(targetPath), QDir::toNativeSeparators(QString("%1boot.ini").arg(targetDrive)));
	}
}

void unetbootin::vistabcdEdit()
{
	bool warch64 = false;
	if (!QProcess::systemEnvironment().filter("ProgramW6432").isEmpty())
		warch64 = true;
	instIndvfl("emtxfile.exe", QString("%1emtxfile.exe").arg(targetPath));
	QFile vbcdEditF1(QString("%1vbcdedit.bat").arg(targetPath));
	vbcdEditF1.open(QIODevice::ReadWrite | QIODevice::Text);
	QTextStream vbcdEditS1(&vbcdEditF1);
	vbcdEditS1 << QString("bcdedit /create /d \""UNETBOOTINB"\" /application bootsector > %1tmpbcdid").arg(targetPath) << endl;
	vbcdEditF1.close();
	if (!warch64)
		callexternapp(QString("%1vbcdedit.bat").arg(targetPath), "");
	QFile vbcdTmpInF(QString("%1tmpbcdid").arg(targetPath));
	vbcdTmpInF.open(QIODevice::ReadOnly | QIODevice::Text);
	QTextStream vbcdTmpInS(&vbcdTmpInF);
	QString qstmpvbcdin = vbcdTmpInS.readAll();
	vbcdTmpInF.close();
	QString vbcdIdTL;
	QStringList vbcdIdTLSL;
	if (!warch64)
	{
		vbcdIdTLSL = qstmpvbcdin.replace("{", "\n").replace("}", "\n").split("\n").filter("-");
		if (!vbcdIdTLSL.isEmpty())
			vbcdIdTL = vbcdIdTLSL.at(0);
	}
	else
	{
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
	"bcdedit /set {%1} device partition=%2\n"
	"bcdedit /displayorder {%1} /addlast\n"
	"bcdedit /timeout 30").arg(vbcdIdTL).arg(targetDev) << endl;
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
	QFile srcF(QString(":/%1").arg(srcfName));
	#ifdef NOSTATIC
	if (srcfName == "memdisk")
				srcF.setFileName(QFile::exists("/usr/share/syslinux/memdisk") ? "/usr/share/syslinux/memdisk" : "/usr/lib/syslinux/memdisk");
	else if (srcfName == "menu.c32")
	{
				srcF.setFileName(QFile::exists("/usr/share/syslinux/menu.c32") ? "/usr/share/syslinux/menu.c32" : "/usr/lib/syslinux/menu.c32");
				if (QFile::exists("/usr/lib/syslinux/modules/bios/menu.c32"))
					srcF.setFileName("/usr/lib/syslinux/modules/bios/menu.c32");
	}
    else if (srcfName == "libutil.c32")
	{
                srcF.setFileName(QFile::exists("/usr/share/syslinux/libutil.c32") ? "/usr/share/syslinux/libutil.c32" : "/usr/lib/syslinux/libutil.c32");
		if (QFile::exists("/usr/lib/syslinux/modules/bios/libutil.c32"))
			srcF.setFileName("/usr/lib/syslinux/modules/bios/libutil.c32");
	}
    else if (srcfName == "libcom32.c32")
	{
                srcF.setFileName(QFile::exists("/usr/share/syslinux/libcom32.c32") ? "/usr/share/syslinux/libcom32.c32" : "/usr/lib/syslinux/libcom32.c32");
		if (QFile::exists("/usr/lib/syslinux/modules/bios/libcom32.c32"))
			srcF.setFileName("/usr/lib/syslinux/modules/bios/libcom32.c32");
	}
    else if (srcfName == "mbr.bin")
	{
			srcF.setFileName(QFile::exists("/usr/share/syslinux/mbr.bin") ? "/usr/share/syslinux/mbr.bin" : "/usr/lib/syslinux/mbr.bin");
		if (QFile::exists("/usr/lib/syslinux/mbr/mbr.bin"))
			srcF.setFileName("/usr/lib/syslinux/mbr/mbr.bin");
	}
	else if (srcfName == "ubnsylnx")
		srcF.setFileName("/usr/bin/syslinux");
//	else
//		srcF.setFileName(QString("/usr/lib/unetbootin/%1").arg(srcfName));
	#endif
    if (!srcF.exists())
    {
        return;
    }
    if (QFile::exists(dstfName))
    {
        if (!overwritefileprompt(dstfName))
            return;
    }
    QFile dstF(dstfName);
    dstF.open(QIODevice::WriteOnly);
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
	this->trcurrent = tr("(Current)");
	this->trdone = tr("(Done)");
	firstlayer->setEnabled(false);
	firstlayer->hide();
	secondlayer->setEnabled(true);
	secondlayer->show();
	rebootlayer->setEnabled(false);
	rebootlayer->hide();
	progresslayer->setEnabled(true);
	progresslayer->show();
	sdesc1->setText(QString("<b>%1 %2</b>").arg(sdesc1->text()).arg(trcurrent));
	tprogress->setValue(0);
	installType = typeselect->currentText();
	targetDrive = driveselect->currentText();
	persistenceSpaceMB = this->persistencevalue->value();
	QString ginstallDir;
	QString installDir;
	QString isotmpf = randtmpfile::getrandfilename(ubntmpf, "iso");
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
		devluid = getdevluid(targetDev);
	}
	if (installType == tr("USB Drive"))
	{
		targetDev = driveselect->currentText();
		devluid = getdevluid(targetDev);
		ginstallDir = "";
		installDir = ginstallDir;
		targetDrive = QString("%1/").arg(locatemountpoint(targetDev));
	}
#ifdef Q_OS_LINUX
	if (targetDev.contains(QRegExp("p\\d$")))
		rawtargetDev = QString(targetDev).remove(QRegExp("p\\d$"));
	else
		rawtargetDev = QString(targetDev).remove(QRegExp("\\d$"));
#endif
#ifdef Q_OS_MAC
	rawtargetDev = QString(targetDev).remove(QRegExp("s\\d$"));
#endif
	#endif
#ifndef Q_OS_UNIX
	devluid = getdevluid(targetDev);
#endif
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
			if (!FloppyPath->text().startsWith("http://") && !FloppyPath->text().startsWith("ftp://"))
				QFile::copy(FloppyPath->text(), QString("%1ubninit").arg(targetPath));
			else
				downloadfile(FloppyPath->text(), QString("%1ubninit").arg(targetPath));
		}
		if (diskimagetypeselect->currentIndex() == diskimagetypeselect->findText(tr("ISO")))
		{
			if (!FloppyPath->text().startsWith("http://") && !FloppyPath->text().startsWith("ftp://"))
				extractiso(FloppyPath->text());
			else
			{
				downloadfile(FloppyPath->text(), isotmpf);
				extractiso(isotmpf);
			}
			if (QFile::exists(QString("%1sevnz.exe").arg(ubntmpf)))
			{
				rmFile(QString("%1sevnz.exe").arg(ubntmpf));
			}
			if (QFile::exists(QString("%1\\7z.dll").arg(ubntmpf)))
			{
				rmFile(QString("%1\\7z.dll").arg(ubntmpf));
			}
		}
	}
	else if (radioManual->isChecked())
	{
		if (!KernelPath->text().startsWith("http://") && !KernelPath->text().startsWith("ftp://"))
			QFile::copy(KernelPath->text(), QString("%1ubnkern").arg(targetPath));
		else
			downloadfile(KernelPath->text(), QString("%1ubnkern").arg(targetPath));
		if (!InitrdPath->text().startsWith("http://") && !InitrdPath->text().startsWith("ftp://"))
			QFile::copy(InitrdPath->text(), QString("%1ubninit").arg(targetPath));
		else
			downloadfile(InitrdPath->text(), QString("%1ubninit").arg(targetPath));
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
		if (nameVersion.contains("_HdMedia"))
		{
			nameVersion.remove("_HdMedia");
			ishdmedia = true;
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
		QString cpuarch;
		QString relname = nameVersion.toLower();
		#include "customdistrolst.cpp"
		if (QFile::exists(QString("%1sevnz.exe").arg(ubntmpf)))
		{
			rmFile(QString("%1sevnz.exe").arg(ubntmpf));
		}
		if (QFile::exists(QString("%1\\7z.dll").arg(ubntmpf)))
		{
			rmFile(QString("%1\\7z.dll").arg(ubntmpf));
		}
		if (downloadFailed)
		{
			return;
		}
	}
	if (!sdesc1->text().contains(trdone))
	{
		sdesc1->setText(QString(sdesc1->text()).remove("<b>").replace(trcurrent+"</b>", trdone));
	}
	if (sdesc2->text().contains(trcurrent))
	{
		sdesc2->setText(QString(sdesc2->text()).remove("<b>").replace(trcurrent+"</b>", trdone));
	}
	else
	{
		sdesc2->setText(QString("%1 %2").arg(sdesc2->text()).arg(trdone));
	}
	sdesc3->setText(QString("<b>%1 %2</b>").arg(sdesc3->text()).arg(trcurrent));
	tprogress->setValue(0);
	if (this->persistenceSpaceMB > 0 && !issalt)
	{
		this->kernelOpts += " persistent";
		for (int i = 0; i < this->extraoptionsPL.second.second.size(); ++i)
		{
			this->extraoptionsPL.second.second[i] += " persistent";
		}
	}
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

#ifdef Q_OS_UNIX

void unetbootin::writegrub2cfg()
{
	QFile menulst;
	menulst.setFileName("/boot/grub/grub.cfg");
	if (QFile::exists(QString("%1.bak").arg(menulst.fileName())))
		rmFile(QString("%1.bak").arg(menulst.fileName()));
	QFile::copy(menulst.fileName(), QString("%1.bak").arg(menulst.fileName()));
	QFile bkmenulst(QString("%1.bak").arg(menulst.fileName()));
	bkmenulst.open(QIODevice::ReadOnly | QIODevice::Text);
	QTextStream bkmenulstout(&bkmenulst);
	menulst.open(QIODevice::WriteOnly | QIODevice::Text);
	QTextStream menulstout(&menulst);
	QRegExp mlstchkistimeout("\\s{0,}set\\s{1,}timeout=\\d{1,}.{0,}", Qt::CaseInsensitive);
	QStringList ecurmenulstTextL;
	bool mlstmustreplacetimeout = true;
	QString menulstCurLine;
	while (!bkmenulstout.atEnd())
	{
		menulstCurLine = bkmenulstout.readLine();
		if (mlstmustreplacetimeout && mlstchkistimeout.exactMatch(menulstCurLine))
		{
			ecurmenulstTextL.append("set timeout=15");
			mlstmustreplacetimeout = false;
		}
		else
		{
			ecurmenulstTextL.append(menulstCurLine);
		}
	}
	QString ecurmenulstText = ecurmenulstTextL.join("\n");
	QString menulstxt = QString(
	"%9\n\n"
#ifndef NODEFAULTBOOT
	"\nmenuentry \""UNETBOOTINB"\" {\n"
	"\tset root=%8\n"
	"\t%1 %2 %3 %4\n"
	"\t%5 %6 %7\n"
	"}\n"
#endif
	).arg(kernelLine.replace("kernel", "linux")).arg(kernelParam).arg(kernelLoc).arg(kernelOpts).arg(initrdLine).arg(initrdLoc).arg(initrdOpts)
	.arg(getGrub2Notation(targetDev)).arg(ecurmenulstText)
	;
	if (!extraoptionsPL.first.first.isEmpty())
	{
		for (int i = 0; i < extraoptionsPL.first.first.size(); ++i)
		{
			menulstxt.append(QString("\nmenuentry \"%1\" {\n"
			"\tset root=%5\n"
			"\tlinux %2 %4\n"
			"\tinitrd %3\n"
			"}\n").arg(QString(extraoptionsPL.second.first.at(i)).remove("^")).arg(extraoptionsPL.first.first.at(i)).arg(extraoptionsPL.first.second.at(i)).arg(extraoptionsPL.second.second.at(i))
			.arg(getGrubNotation(targetDev))
			);
		}
	}
	menulstout << menulstxt << endl;
	menulst.close();
}

#endif

void unetbootin::runinsthdd()
{
	this->tprogress->setValue(this->tprogress->maximum()/3);
	#ifdef Q_OS_UNIX
	if (QFile::exists("/boot/grub/grub.cfg")) // has grub2
	{
		pdesc1->setText(tr("Configuring grub2 on %1").arg(targetDev));
		writegrub2cfg();
		if (!QFile::exists("/boot/grub/menu.lst")) // grub2-only
		{
			QSettings install(QSettings::SystemScope, "UNetbootin");
			install.setValue("Location", "/");
			fininstall();
			return;
		}
	}
	#endif
	#ifdef Q_OS_WIN32
	pdesc1->setText(tr("Configuring grldr on %1").arg(targetDev));
	if (QFile::exists(QDir::toNativeSeparators(QString("%1unetbtin.exe").arg(targetDrive))))
	{
		rmFile(QDir::toNativeSeparators(QString("%1unetbtin.exe").arg(targetDrive)));
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
	pdesc1->setText(tr("Configuring grub on %1").arg(targetDev));
	menulst.setFileName("/boot/grub/menu.lst");
	if (QFile::exists(QString("%1.bak").arg(menulst.fileName())))
		rmFile(QString("%1.bak").arg(menulst.fileName()));
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
		if (mlstmustreplacehiddenmenu && mlstchkishiddenmenu.exactMatch(menulstCurLine))
		{
			ecurmenulstTextL.append("#hiddenmenu");
			mlstmustreplacehiddenmenu = false;
		}
		else if (mlstmustreplacetimeout && mlstchkistimeout.exactMatch(menulstCurLine))
		{
			ecurmenulstTextL.append("timeout\t\t15");
			mlstmustreplacetimeout = false;
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
	//QSettings runonce("HKEY_LOCAL_MACHINE\\Software\\Microsoft\\Windows\\CurrentVersion\\RunOnce", QSettings::NativeFormat);
	//runonce.setValue("UNetbootin Uninstaller", QDir::toNativeSeparators(QString("%1unetbtin.exe").arg(targetDrive)));
	if (QSysInfo::WindowsVersion == QSysInfo::WV_32s || QSysInfo::WindowsVersion == QSysInfo::WV_95 || QSysInfo::WindowsVersion == QSysInfo::WV_98 || QSysInfo::WindowsVersion == QSysInfo::WV_Me)
	{
		configsysEdit();
	}
	else if (QSysInfo::WindowsVersion == QSysInfo::WV_NT || QSysInfo::WindowsVersion == QSysInfo::WV_2000 || QSysInfo::WindowsVersion == QSysInfo::WV_XP || QSysInfo::WindowsVersion == QSysInfo::WV_2003 )
	{
		bootiniEdit();
	}
	else if (QSysInfo::WindowsVersion == QSysInfo::WV_VISTA) //|| QSysInfo::WindowsVersion == QSysInfo::WV_WINDOWS7) // TODO when upgrading to latest Qt
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

void unetbootin::rmFile(QFile &fn)
{
	if (!fn.exists()) return;
	fn.setPermissions(QFile::WriteUser);
	fn.remove();
#ifdef Q_OS_UNIX
	callexternapp("sync", "");
#endif
}

void unetbootin::rmFile(const QString &fn)
{
	if (!QFile::exists(fn)) return;
	QFile::setPermissions(fn, QFile::WriteUser);
	QFile::remove(fn);
#ifdef Q_OS_UNIX
	callexternapp("sync", "");
#endif
}

void unetbootin::mvFile(QFile &fn, QFile &outfn)
{
	rmFile(outfn);
	fn.rename(outfn.fileName());
#ifdef Q_OS_UNIX
	callexternapp("sync", "");
#endif
}

void unetbootin::mvFile(const QString &fn, const QString &outfn)
{
	rmFile(outfn);
	QFile::rename(fn, outfn);
#ifdef Q_OS_UNIX
	callexternapp("sync", "");
#endif
}

void unetbootin::replaceTextInFile(QString repfilepath, QRegExp replaceme, QString replacewith)
{
	QFile repfileF(repfilepath);
	randtmpfile nrepfileF(QFileInfo(repfilepath).canonicalPath(), "cfg");
	QString nrepfilepath = QFileInfo(nrepfileF).canonicalFilePath();
	repfileF.open(QIODevice::ReadOnly | QIODevice::Text);
	nrepfileF.open(QIODevice::WriteOnly | QIODevice::Text);
	QTextStream repfileTS(&repfileF);
	QTextStream nrepfileTS(&nrepfileF);
	while (!repfileTS.atEnd())
	{
		nrepfileTS << repfileTS.readLine().replace(replaceme, replacewith) << "\n";
	}
	repfileF.close();
	rmFile(repfilepath);
	nrepfileF.rename(repfilepath);
	nrepfileF.close();
	//mvFile(nrepfilepath, repfilepath);
}

void unetbootin::setLabel(QString devname, QString newlabel)
{
#ifdef Q_OS_LINUX
	if (isext2)
	{
		callexternapp(e2labelcommand, devname+" "+newlabel);
	}
	else
	{
		callexternapp(mlabelcommand, "-i "+devname+" ::"+newlabel);
	}
#endif
#ifdef Q_OS_MAC
	callexternapp("diskutil", "rename "+devname+" "+newlabel);
#endif
#ifdef Q_OS_WIN32
	callexternapp("label", devname+" "+newlabel);
#endif
	this->devlabel = QString(newlabel);
	if (this->devluid.startsWith("LABEL="))
		this->devluid = "LABEL="+newlabel;
#ifdef Q_OS_MAC
	this->targetPath = this->locatemountpoint(devname) + "/";
	this->targetDrive = this->targetPath;
#endif
}

QString unetbootin::fixkernelbootoptions(const QString &cfgfileCL)
{
	if (cfgfileCL.contains("archisolabel=") && (this->devlabel == "" || this->devlabel == "None"))
	{
		this->devlabel = this->getlabel(this->targetDev);
		if  (this->installType == tr("USB Drive") && (this->devlabel == "" || this->devlabel == "None"))
		{
			QString isolabelopt = QString(cfgfileCL).trimmed();
			int startIdx = isolabelopt.indexOf("archisolabel=");
			if (startIdx >= 0)
			{
				isolabelopt = isolabelopt.right(isolabelopt.size() - startIdx - QString("archisolabel=").size()).trimmed();
				int endIdx = isolabelopt.indexOf(" ");
				if (endIdx > 0)
				{
					isolabelopt = isolabelopt.left(endIdx);
					setLabel(this->targetDev, isolabelopt);
				}
			}
		}
	}
    if ((this->devlabel == "" || this->devlabel == "None") && devluid.contains("LABEL") && (cfgfileCL.contains(QRegExp("root=\\S{0,}LABEL=\\S{0,}")) || cfgfileCL.contains(QRegExp("root=\\S{0,}CDLABEL=\\S{0,}"))))
    {
        setLabel(this->targetDev, "LIVE");
    }
    QString ncfgfileCL = cfgfileCL;
    if (ncfgfileCL.contains("root=live:CDLABEL"))
    {
        ncfgfileCL = QString(ncfgfileCL)
        .replace(QRegExp("root=\\S{0,}LABEL=\\S{0,}"), QString("root=live:%1").arg(devluid))
        .replace(QRegExp("root=\\S{0,}CDLABEL=\\S{0,}"), QString("root=live:%1").arg(devluid));
    }
    else
    {
        ncfgfileCL = QString(ncfgfileCL)
        .replace(QRegExp("root=\\S{0,}LABEL=\\S{0,}"), QString("root=%1").arg(devluid))
        .replace(QRegExp("root=\\S{0,}CDLABEL=\\S{0,}"), QString("root=%1").arg(devluid));
    }
    return QString(ncfgfileCL)
	.replace("rootfstype=iso9660", "rootfstype=auto")
    .replace("theme:sabayon", "theme:sabayon cdroot_type=vfat")
	.replace("pmedia=cd", "pmedia=usbflash")
	.replace(QRegExp("archisolabel=\\S{0,}"), QString("archisolabel=%1").arg(devlabel))
	.trimmed();
}

void unetbootin::logText(const QString &text)
{
    return;
    /*
    if (targetPath.isNull() || targetPath.isEmpty())
    {
        loggedLinesNotYetWritten.append(text);
        return;
    }
    if (logStream == 0)
    {
        logFile = new QFile(QString("%1unetbootin-log.txt").arg(targetPath));
        logFile->open(QIODevice::WriteOnly | QIODevice::Text);
        logStream = new QTextStream(logFile);
        for (int i = 0; i < loggedLinesNotYetWritten.size(); ++i)
        {
            *logStream << loggedLinesNotYetWritten.at(i) << endl;
        }
        loggedLinesNotYetWritten.clear();
    }
    *logStream << text << endl;
    */
}

void unetbootin::finishLogging()
{
    if (logFile != 0)
    {
        logFile->close();
    }
}

void unetbootin::writeTextToFile(const QString &text, const QString &filePath)
{
    QFile syslinuxcfg(filePath);
    if (syslinuxcfg.exists())
    {
        rmFile(syslinuxcfg);
    }
    syslinuxcfg.open(QIODevice::WriteOnly | QIODevice::Text);
    QTextStream syslinuxcfgout(&syslinuxcfg);
    syslinuxcfgout << text << endl;
    syslinuxcfg.close();
}

void unetbootin::runinstusb()
{
	this->tprogress->setValue(this->tprogress->maximum()/3);
	pdesc1->setText(tr("Installing syslinux to %1").arg(targetDev));
	#ifdef Q_OS_WIN32
	QString sysltfloc = instTempfl("syslinux.exe", "exe");
	callexternapp(sysltfloc, QString("-ma %1").arg(targetDev));
	rmFile(sysltfloc);
	#endif
	#ifdef STATICLINUX
	if (QFile::exists(syslinuxcommand))
		rmFile(syslinuxcommand);
	instIndvfl("ubnsylnx", syslinuxcommand);
		QFile::setPermissions(syslinuxcommand, QFile::ReadOwner|QFile::ExeOwner|QFile::ReadGroup|QFile::ExeGroup|QFile::ReadOther|QFile::ExeOther|QFile::WriteOwner);
//	chmod(syslinuxcommand, S_IRUSR|S_IRGRP|S_IROTH|S_IRWXU);
		if (QFile::exists(extlinuxcommand))
				rmFile(extlinuxcommand);
		instIndvfl("ubnexlnx", extlinuxcommand);
		QFile::setPermissions(extlinuxcommand, QFile::ReadOwner|QFile::ExeOwner|QFile::ReadGroup|QFile::ExeGroup|QFile::ReadOther|QFile::ExeOther|QFile::WriteOwner);
	#endif
#ifdef Q_OS_LINUX
		isext2 = false;
		if (!volidcommand.isEmpty())
		{
			if (callexternapp(volidcommand, QString("-t %2").arg(targetDev)).contains(QRegExp("(ext2|ext3|ext4)")))
				isext2 = true;
		}
		else
		{
			QString tstrblk = callexternapp(blkidcommand, QString("-s TYPE %2").arg(targetDev));
			if (tstrblk.contains('='))
			{
				if (tstrblk.contains(QRegExp("(ext2|ext3|ext4)")))
					isext2 = true;
			}
		}
		if (isext2)
		{
			pdesc1->setText(tr("Installing extlinux to %1").arg(targetDev));
			callexternapp(extlinuxcommand, QString("-i \"%1\"").arg(targetPath));
		}
		else
			callexternapp(syslinuxcommand, targetDev);
		if (rawtargetDev != targetDev)
		{
			// make active
			if (sfdiskcommand != "") {
				// use sfdisk if available
                callexternapp(sfdiskcommand, QString("%1 -A%2").arg(rawtargetDev, QString(targetDev).remove(rawtargetDev).remove("p")));
			} else {
				// use fdisk if sfdisk is unavailable
				bool isOk = false;
                int partitionNumber = QString(targetDev).remove(rawtargetDev).remove("p").toInt(&isOk, 10);
				if (isOk)
				{
					QString output = callexternapp("fdisk", "-l");
					QStringList outputL = output.split('\n');
					outputL = outputL.filter(targetDev);
					if (outputL.size() > 0)
					{
						outputL = outputL.filter("*");
						bool isActive = outputL.size() > 0;
						if (!isActive)
						{
							QString fdiskWriteToStdin = "a\n";
							fdiskWriteToStdin += (QString::number(partitionNumber) + "\n");
							fdiskWriteToStdin += "w\n";
							callexternappWriteToStdin("fdisk", rawtargetDev, fdiskWriteToStdin);
						}
					}
				}
			}
			QFile usbmbrF(rawtargetDev);
			QFile mbrbinF(":/mbr.bin");
			#ifdef NOSTATIC
			mbrbinF.setFileName(QFile::exists("/usr/share/syslinux/mbr.bin") ? "/usr/share/syslinux/mbr.bin" : "/usr/lib/syslinux/mbr.bin");
			if (QFile::exists("/usr/lib/syslinux/mbr/mbr.bin"))
				mbrbinF.setFileName("/usr/lib/syslinux/mbr/mbr.bin");
			#endif
			usbmbrF.open(QIODevice::WriteOnly);
			mbrbinF.open(QIODevice::ReadOnly);
			usbmbrF.write(mbrbinF.readAll());
			mbrbinF.close();
			usbmbrF.close();
		}
#endif
#ifdef Q_OS_MAC
        callexternapp("sync", "");
        callexternapp("diskutil", "umount "+targetDev);
        callexternapp("sync", "");
        callexternapp("hdiutil", "unmount "+targetDev);
        callexternapp("sync", "");
        callexternapp(resourceDir.absoluteFilePath("mkbootable"), targetDev);
        /*
        callexternapp("sync", "");
        callexternapp("diskutil", "umount "+targetDev);
        callexternapp("sync", "");
        callexternapp("hdiutil", "unmount "+targetDev);
        callexternapp("sync", "");
		QFile usbmbrF(rawtargetDev);
		QFile mbrbinF(resourceDir.absoluteFilePath("mbr.bin"));
		usbmbrF.open(QIODevice::WriteOnly);
		mbrbinF.open(QIODevice::ReadOnly);
		usbmbrF.write(mbrbinF.readAll());
		mbrbinF.close();
		usbmbrF.close();
        */
        callexternapp("sync", "");
		callexternapp("diskutil", "mount "+targetDev);
        callexternapp("sync", "");
#endif
#ifndef XPUD
	if (!dontgeneratesyslinuxcfg)
	{
	QString syslinuxcfgtxt = QString("default menu.c32\n"
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
    writeTextToFile(syslinuxcfgtxt, QString("%1syslinux.cfg").arg(targetPath));
	}
	else
	{
		for (int j = 0; j < locatedsyslinuxcfgfiles.size(); ++j)
		{
			QString syslpathloc = QFileInfo(locatedsyslinuxcfgfiles.at(j)).path();
			if (syslpathloc == ".") syslpathloc = "";
			if (syslpathloc.contains(QDir::toNativeSeparators("/")))
			{
				if (!syslpathloc.endsWith(QDir::toNativeSeparators("/")))
					syslpathloc.append(QDir::toNativeSeparators("/"));
			}
			else
			{
				if (!syslpathloc.endsWith("/"))
					syslpathloc.append("/");
			}
			QString abssyslpathloc = QDir::fromNativeSeparators(QString(syslpathloc));
			if (!abssyslpathloc.startsWith("/"))
				abssyslpathloc.prepend("/");
			instIndvfl("menu.c32", QString("%1%2menu.c32").arg(targetPath).arg(syslpathloc));
            instIndvfl("libutil.c32", QString("%1%2libutil.c32").arg(targetPath).arg(syslpathloc));
            instIndvfl("libcom32.c32", QString("%1%2libcom32.c32").arg(targetPath).arg(syslpathloc));
			QString syslrealcfgloc = QString(locatedsyslinuxcfgfiles.at(j)).replace("isolinux.cfg", "syslinux.cfg").replace("extlinux.conf", "syslinux.cfg");
			if (syslrealcfgloc != locatedsyslinuxcfgfiles.at(j))
			{
				QFile::copy(QString("%1%2").arg(targetPath).arg(locatedsyslinuxcfgfiles.at(j)), QString("%1%2").arg(targetPath).arg(syslrealcfgloc));
			}
			replaceTextInFile(QString("%1%2").arg(targetPath).arg(syslrealcfgloc), QRegExp("\\S{0,}vesamenu.c32"), QString("%1menu.c32").arg(abssyslpathloc));
			#ifdef Q_OS_UNIX
			if (isext2)
			{
				QFile::copy(QString("%1%2").arg(targetPath).arg(locatedsyslinuxcfgfiles.at(j)), QString("%1%2extlinux.conf").arg(targetPath).arg(syslpathloc));
				QString extlpathloc = QString(syslpathloc).replace("syslinux", "extlinux");
				if (syslpathloc != extlpathloc)
					callexternapp("ln", QString("-s %1 %2").arg(syslpathloc).arg(extlpathloc));
			}
			#endif
		}
	}
#endif
		#ifdef Q_OS_UNIX
		if (!dontgeneratesyslinuxcfg && isext2)
			QFile::copy(QString("%1syslinux.cfg").arg(targetPath), QString("%1extlinux.conf").arg(targetPath));
		#endif
		if (!dontgeneratesyslinuxcfg)
        {
            instIndvfl("menu.c32", QString("%1menu.c32").arg(targetPath));
            instIndvfl("libutil.c32", QString("%1libutil.c32").arg(targetPath));
            instIndvfl("libcom32.c32", QString("%1libcom32.c32").arg(targetPath));
        }
	fininstall();
}

void unetbootin::killApplication()
{
	exit(0);
}

void unetbootin::fininstall()
{
	#ifdef Q_OS_UNIX
	this->tprogress->setValue(this->tprogress->maximum()*2/3);
	pdesc1->setText(tr("Syncing filesystems"));
	callexternapp("sync", "");
	#endif
	if (this->persistenceSpaceMB > 0)
	{
		pdesc1->setText(tr("Setting up persistence"));
		this->tprogress->setMaximum(persistenceSpaceMB);
		this->tprogress->setValue(0);
		QString persfile = "casper-rw";
		if (issalt && !saltRootDir.isEmpty()) {
			QStringList persistencedir;
			persistencedir.append("persistence");
			makepathtree(QString("%1%2").arg(targetPath).arg(saltRootDir), persistencedir);
			persfile = QString("%1/persistence/%1.save").arg(saltRootDir);
		}
#ifdef Q_OS_WIN32
		QString mke2fscommand = instTempfl("mke2fs.exe", "exe");
#endif
		if (QFile::exists(QString("%1%2").arg(targetPath).arg(persfile)))
		{
			rmFile(QString("%1%2").arg(targetPath).arg(persfile));
		}
		QFile persistenceFile(QString("%1%2").arg(targetPath).arg(persfile));
		persistenceFile.open(QFile::WriteOnly);
		int bytesWritten = 1048576;
		char writeEmpty[1048576];
		memset(writeEmpty, 0, 1048576);
		for (int i = 0; i < persistenceSpaceMB && bytesWritten == 1048576; ++i)
		{
			this->tprogress->setValue(i);
			bytesWritten = persistenceFile.write(writeEmpty, 1048576);
		}
		this->tprogress->setValue(this->tprogress->maximum());
#ifdef Q_OS_UNIX
		callexternapp(mke2fscommand, QString("-F \"%1%2\"").arg(targetPath).arg(persfile));
#endif
#ifdef Q_OS_WIN32
		callexternappWriteToStdin(mke2fscommand, QString("\"%1%2\"").arg(targetPath).arg(persfile), "\n");
		rmFile(mke2fscommand);
#endif
	}
	pdesc1->setText("");
	progresslayer->setEnabled(false);
	progresslayer->hide();
	rebootlayer->setEnabled(true);
	rebootlayer->show();
	sdesc3->setText(QString(sdesc3->text()).remove("<b>").replace(trcurrent+"</b>", trdone));
	sdesc4->setText(QString("<b>%1 %2</b>").arg(sdesc4->text()).arg(trcurrent));
	if (installType == tr("Hard Disk"))
	{
		rebootmsgtext->setText(tr("After rebooting, select the "UNETBOOTINB" menu entry to boot.%1").arg(postinstmsg));
	}
	if (installType == tr("USB Drive"))
	{
#ifndef Q_OS_MAC
		rebootmsgtext->setText(tr("After rebooting, select the USB boot option in the BIOS boot menu.%1").arg(postinstmsg));
#endif
#ifdef Q_OS_MAC
		rebootmsgtext->setText(tr("The created USB device will not boot off a Mac. Insert it into a PC, and select the USB boot option in the BIOS boot menu.%1").arg(postinstmsg));
#endif
	}
    finishLogging();
	if (exitOnCompletion)
	{
		printf("exitstatus:success\n");
		QApplication::exit();
		exit(0);
	}
}
