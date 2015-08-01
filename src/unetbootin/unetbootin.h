/*
unetbootin.h from UNetbootin <http://unetbootin.sourceforge.net>
Copyright (C) 2007-2008 Geza Kovacs <geza0kovacs@gmail.com>

This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 2 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License at <http://www.gnu.org/licenses/> for more details.
*/

#ifndef UNETBOOTIN_H
#define UNETBOOTIN_H

#include <QtCore>
#include <QtGui>
#include <QtNetwork>
#include <time.h>
//#include <QtDebug>

#include "ui_unetbootin.h"

#ifdef Q_OS_WIN32
#define WIN32_LEAN_AND_MEAN
#define _WIN32_WINNT 0x0500
#include <windows.h>
#include <shellapi.h>
#endif

#ifdef Q_OS_MACX
#include <sys/param.h>
#include <sys/mount.h>
#include <sys/statvfs.h>
#endif

#ifdef  Q_OS_LINUX
#include <sys/vfs.h>
#endif

#ifdef AUTOSUPERGRUBDISK
#define UNETBOOTINB "Auto Super Grub Disk"
#define NOEXTERN
//#define NOISO
#define HDDINSTALL
#endif

#ifdef EEEPCLOS
#define UNETBOOTINB "UNetbootin-EeePCLinuxOS"
#define NOMANUAL
#define NOFLOPPY
//#define USBINSTALL
#endif

#ifdef EEEUBUNTU
#define UNETBOOTINB "UNetbootin-Ubuntu Eee"
#define NOMANUAL
#define NOFLOPPY
//#define USBINSTALL
#endif

#ifdef ELIVE
#define UNETBOOTINB "Elive USB Installer"
#define NOMANUAL
#define NOFLOPPY
//#define USBINSTALL
#endif

#ifdef GNEWSENSE
#define UNETBOOTINB "FUSBi"
#define NOMANUAL
#define NOFLOPPY
//#define USBINSTALL
#endif

#ifdef KIWILINUX
#define UNETBOOTINB "Kiwi pe USB"
#define NOMANUAL
#define NOFLOPPY
//#define USBINSTALL
#endif

#ifdef NIMBLEX
#define UNETBOOTINB "NimbleX Installer"
#define NOMANUAL
#define NOFLOPPY
//#define USBINSTALL
#endif

#ifdef SLITAZ
#define UNETBOOTINB "SliTaz Installer"
#define NOMANUAL
#define NOFLOPPY
//#define USBINSTALL
#endif

#ifdef XPUD
#define UNETBOOTINB "xPUD USB Installer"
#define NOMANUAL
#define NOFLOPPY
//#define USBINSTALL
#define NOEXTRACTKERNEL
#define NOEXTRACTINITRD
#define NODEFAULTBOOT
#define NODEFAULTKERNEL
#define NOINITRD
#endif

#ifndef UNETBOOTINB
#define UNETBOOTINB "UNetbootin"
#define STDUNETBOOTIN
//#define USBINSTALL
#endif

class customver : public QObject
{
	Q_OBJECT
};

class uninstaller : public QObject
{
	Q_OBJECT
};

class callexternappT : public QThread
{
public:
	QString execFile;
	QString execParm;
	QString retnValu;
	void run();
};

class callexternappWriteToStdinT : public QThread
{
public:
	QString execFile;
	QString execParm;
	QString writeToStdin;
	QString retnValu;
	void run();
};

class copyfileT : public QThread
{
	Q_OBJECT

public:
	QString source;
	QString destination;
	void run();

signals:
	void datacopied64(qint64 dlbytes, qint64 maxbytes);
	void finished();
};

class ubngetrequestheader : public QHttpRequestHeader
{
public:
	ubngetrequestheader(QString urhost, QString urpath);
};

class randtmpfile : public QFile
{
public:
	randtmpfile(QString rfpath, QString rfextn);
	static QString getrandfilename(QString rfpath, QString rfextn);
};

class nDirListStor : public QObject
{
	Q_OBJECT

public:
	QStringList nDirFileListSL;
	int nMinFileSizeBytes;
	int nMaxFileSizeBytes;
	bool searchsymlinks;

public slots:
	void sAppendSelfUrlInfoList(QUrlInfo curDirUrl);
};

class unetbootin : public QWidget, private Ui::unetbootinui
{
	Q_OBJECT

public:
	unetbootin(QWidget *parent = 0);
	QString trcurrent;
	QString trdone;
	QString appNlang;
	QString appDir;
	QString appLoc;
	QString ubntmpf;
	QString nameDistro;
	QString nameVersion;
    bool skipExtraction;
	bool isarch64;
	bool islivecd;
	bool isnetinstall;
	bool ishdmedia;
	bool overwriteall;
	bool searchsymlinks;
	bool ignoreoutofspace;
	bool dontgeneratesyslinuxcfg;
	bool downloadFailed;
	bool exitOnCompletion;
	bool testingDownload;
	bool issalt;
    bool redundanttopleveldir;
    QString redundantrootdirname;
	QString saltRootDir;
	int persistenceSpaceMB;
	QString extraBootOptions;
	QStringList locatedsyslinuxcfgfiles;
	QString targetDrive;
	QString targetPath;
	QString installType;
	QString targetDev;
	QString rawtargetDev;
	QString devluid;
	QString devlabel;
	QString devuuid;
	QString postinstmsg;
	QString kernelLine, kernelParam, kernelLoc, kernelOpts, initrdLine, initrdLoc, initrdOpts;
	QString slinitrdLine;
	QString sevzcommand;
	QPair<QPair<QStringList, QStringList>, QPair<QStringList, QStringList> > extraoptionsPL;
	QMap<QString, QString> grub2vars;
    QFile *logFile;
    QTextStream *logStream;
    QStringList loggedLinesNotYetWritten;
	#ifdef Q_OS_UNIX
	QString fdiskcommand;
	QString sfdiskcommand;
	QString dfcommand;
	QString blkidcommand;
	QString volidcommand;
	QString syslinuxcommand;
	QString extlinuxcommand;
	QString mke2fscommand;
	QString mlabelcommand;
	QString e2labelcommand;
	bool isext2;
	#endif
#ifdef Q_OS_MAC
	QDir resourceDir;
#endif
	bool ubninitialize(QList<QPair<QString, QString> > oppairs);
	QString displayfisize(quint64 fisize);
	QPair<QPair<QStringList, QList<quint64> >, QStringList> listarchiveconts(QString archivefile);
	bool overwritefileprompt(QString ovwfileloc);
	bool ignoreoutofspaceprompt(QString destindir);
	bool extractfile(QString filepath, QString destinfileL, QString archivefile);
	bool checkifoutofspace(QString destindir);
	QString locatekernel(QString archivefile, QPair<QStringList, QList<quint64> > archivefileconts);
	bool extractkernel(QString archivefile, QString kernoutputfile, QPair<QStringList, QList<quint64> > archivefileconts);
	QString locateinitrd(QString archivefile, QPair<QStringList, QList<quint64> > archivefileconts);
	bool extractinitrd(QString archivefile, QString initoutputfile, QPair<QStringList, QList<quint64> > archivefileconts);
	QString extractcfg(QString archivefile, QStringList archivefileconts);
	QPair<QPair<QStringList, QStringList>, QPair<QStringList, QStringList> > extractcfgL(QString archivefile, QStringList archivefileconts);
	QString getfullarchivepath(QString relativefilepath, QStringList archivefile);
	QString filteroutlist(QString listofdata, QList<QRegExp> listofmatches);
	QString filteroutlist(QStringList listofdata, QList<QRegExp> listofmatches);
	QStringList filteroutlistL(QStringList listofdata, QList<QRegExp> listofmatches);
	void extractiso(QString isofile);
	void extractiso_krd10(QString isofile);
	void copyfilegui(QString src, QString dst);
	QStringList makepathtree(QString dirmkpathw, QStringList pathlist);
	QStringList extractallfiles(QString archivefile, QString dirxfilesto, QPair<QStringList, QList<quint64> > filesizelist, QStringList outputfilelist);
	QString getgrubcfgargs(QString cfgfile);
	QPair<QPair<QStringList, QStringList>, QPair<QStringList, QStringList> > getgrubcfgargsL(QString cfgfile);
	QString getFirstTextBlock(QString fulltext);
	void loadgrub2env(QString cfgfile);
	QString getgrub2cfgargs(QString cfgfile, QString archivefile, QStringList archivefileconts, QStringList visitedincludes);
	QPair<QPair<QStringList, QStringList>, QPair<QStringList, QStringList> > getgrub2cfgargsL(QString cfgfile, QString archivefile, QStringList archivefileconts, QStringList visitedincludes);
	QString getcfgkernargs(QString cfgfile, QString archivefile, QStringList archivefileconts, QStringList visitedincludes);
	QPair<QPair<QStringList, QStringList>, QPair<QStringList, QStringList> > getcfgkernargsL(QString cfgfile, QString archivefile, QStringList archivefileconts, QStringList visitedincludes);
	QString searchforincludesfile(QString includesfile, QString archivefile, QStringList archivefileconts, QStringList visitedincludes);
	QPair<QPair<QStringList, QStringList>, QPair<QStringList, QStringList> > searchforincludesfileL(QString includesfile, QString archivefile, QStringList archivefileconts, QStringList visitedincludes);
	QString searchforgrub2includesfile(QString includesfile, QString archivefile, QStringList archivefileconts, QStringList visitedincludes);
	QPair<QPair<QStringList, QStringList>, QPair<QStringList, QStringList> > searchforgrub2includesfileL(QString includesfile, QString archivefile, QStringList archivefileconts, QStringList visitedincludes);
	void downloadfile(QString fileurl, QString targetfile, int minsize);
	QString downloadpagecontents(QString pageurl);
	QStringList lstFtpDirFiles(QString ldfDirStringUrl, int ldfMinSize, int ldfMaxSize);
	QStringList lstHttpDirFiles(QString ldfDirStringUrl);
	QStringList lstNetDirFiles(QString ldfDirStringUrl, int ldfMinSize, int ldfMaxSize);
	QPair<QString, int> weightedFilterNetDir(QString ldfDirStringUrl, int ldfMinSize, int ldfMaxSize, QList<QRegExp> ldfFileMatchExp);
	QString fileFilterNetDir(QStringList ldfDirStringUrlList, int ldfMinSize, int ldfMaxSize, QList<QRegExp> ldfFileMatchExp);
	QPair<QString, int> filterBestMatch(QStringList ufStringList, QList<QRegExp> filterExpList);
	static QString callexternapp(QString xexecFile, QString xexecParm);
	static QString callexternappWriteToStdin(QString xexecFile, QString xexecParm, QString xwriteToStdin);
	QString getdevluid(QString voldrive);
	QString getlabel(QString voldrive);
	QString getuuid(QString voldrive);
#ifdef Q_OS_MAC
	QString getlabel(QString voldrive, QString diskutilinfo);
	QString getuuid(QString voldrive, QString diskutilinfo);
#endif
	void refreshdriveslist();
	QStringList listcurdrives();
	QStringList listsanedrives();
	QStringList listalldrives();
	void replaceTextInFile(QString repfilepath, QRegExp replaceme, QString replacewith);
	QString fixkernelbootoptions(const QString &cfgfileCL);
	void setLabel(QString devname, QString newlabel);
	#ifdef Q_OS_UNIX
	QString locatecommand(QString commandtolocate, QString reqforinstallmode, QString packagename);
	QString locatedevicenode(QString mountpoint);
	QString locatemountpoint(QString devicenode);
	QString getGrubNotation(QString devicenode);
	QString getGrub2Notation(QString devicenode);
	int letterToNumber(QChar lettertoconvert);
	int getDiskNumber(QString devicenode);
	int getPartitionNumber(QString devicenode);
	#endif
	#ifdef Q_OS_WIN32
	void installsvzip();
	void configsysEdit();
	void bootiniEdit();
	void vistabcdEdit();
	#endif
	void instIndvfl(QString srcfName, QString dstfName);
	QString instTempfl(QString srcfName, QString dstfType);
	void runinst();
	void instDetType();
	#ifdef Q_OS_UNIX
	void writegrub2cfg();
	#endif
	void runinsthdd();
	void runinstusb();
    void logText(const QString &text);
    void finishLogging();
    void writeTextToFile(const QString &text, const QString &filePath);
	void fininstall();
	void rmFile(const QString &fn);
	void rmFile(QFile &fn);
	void mvFile(const QString &fn, const QString &outfn);
	void mvFile(QFile &fn, QFile &outfn);
	void showDownloadFailedScreen(const QString &fileurl);

private slots:
	void on_distroselect_currentIndexChanged(int distroselectIndex);
	void on_typeselect_currentIndexChanged(int typeselectIndex);
	void on_dverselect_currentIndexChanged();
	void on_diskimagetypeselect_currentIndexChanged();
	void on_FloppyFileSelector_clicked();
	void on_KernelFileSelector_clicked();
	void on_InitrdFileSelector_clicked();
	void on_CfgFileSelector_clicked();
	void on_cancelbutton_clicked();
	void on_fexitbutton_clicked();

public slots:
	void dlprogressupdate(int dlbytes, int maxbytes);
	void dlprogressupdate64(qint64 dlbytes, qint64 maxbytes);
	void cpprogressupdate64(qint64 dlbytes, qint64 maxbytes);
	void on_okbutton_clicked();
	void killApplication();
};

#endif

