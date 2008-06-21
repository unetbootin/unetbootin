/*
unetbootin.h from UNetbootin <http://unetbootin.sourceforge.net>
Copyright (C) Geza Kovacs <geza0kovacs@gmail.com>

This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 2 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License at <http://www.gnu.org/licenses/> for more details.
*/

#ifndef UNETBOOTIN_H
#define UNETBOOTIN_H

#include <QtCore>
#include <QtGui>
#include <QtNetwork>

#include "ui_unetbootin.h"

#ifdef Q_OS_WIN32
#define WIN32_LEAN_AND_MEAN
#define _WIN32_WINNT 0x0500
#include <windows.h>
#include <shellapi.h>
#endif

#ifdef AUTOSUPERGRUBDISK
#define UNETBOOTINB "Auto Super Grub Disk"
#endif

#ifndef UNETBOOTINB
#define STDUNETBOOTIN
#define UNETBOOTINB "UNetbootin"
#endif

class callexternappT : public QThread
{
public:
	QString execFile;
	QString execParm;
	QString retnValu;
	void run();
};

class ubngetrequestheader : public QHttpRequestHeader
{
public:
	ubngetrequestheader(QString urhost, QString urpath);
};

class unetbootin : public QWidget, private Ui::unetbootinui
{
	Q_OBJECT

public:
	unetbootin(QWidget *parent = 0);
	QString appNlang;
	QString appDir;
	QString appLoc;
	QString ubntmpf;
	QString nameDistro;
	QString nameVersion;
	bool isarch64;
	bool islivecd;
	bool isnetinstall;
	bool overwriteall;
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
	QString sevzcommand;
	#ifdef Q_OS_UNIX
	QString fdiskcommand;
	QString sfdiskcommand;
	QString volidcommand;
	QString syslinuxcommand;
	#endif
	void ubninitialize();
	QString displayfisize(quint64 fisize);
	QPair<QPair<QStringList, QList<quint64> >, QStringList> listarchiveconts(QString archivefile);
	bool overwritefileprompt(QString ovwfileloc);	
	bool extractfile(QString filepath, QString destinfileL, QString archivefile);
	bool extractkernel(QString archivefile, QString kernoutputfile, QPair<QStringList, QList<quint64> > archivefileconts);
	bool extractinitrd(QString archivefile, QString initoutputfile, QPair<QStringList, QList<quint64> > archivefileconts);
	QString extractcfg(QString archivefile, QStringList archivefileconts);
	void extractiso(QString isofile, QString exoutputdir);
	QStringList makepathtree(QString dirmkpathw, QStringList pathlist);
	QStringList extractallfiles(QString archivefile, QString dirxfilesto, QPair<QStringList, QList<quint64> > filesizelist);
	QString getgrubcfgargs(QString cfgfile);
	QString getcfgkernargs(QString cfgfile);
	void downloadfile(QString fileurl, QString targetfile);
	QString downloadpagecontents(QString pageurl);
	void sysreboot();
	QString callexternapp(QString xexecFile, QString xexecParm);
	QString getdevluid(QString voldrive);
	QString getlabel(QString voldrive);
	QString getuuid(QString voldrive);
	#ifdef Q_OS_UNIX
	QString locatecommand(QString commandtolocate, QString reqforinstallmode, QString packagename);
	QString locatedevicenode(QString mountpoint);
	QString locatemountpoint(QString devicenode);
	QString getGrubNotation(QString devicenode);
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
	void runinst();
	void instDetType();
	void runinsthdd();
	void runinstusb();
	void fininstall();

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
	void on_okbutton_clicked();
	void on_frebootbutton_clicked();
	void on_fexitbutton_clicked();

public slots:
	void dlprogressupdate(int dlbytes, int maxbytes);
	void dlprogressupdate64(qint64 dlbytes, qint64 maxbytes);
};

#endif

