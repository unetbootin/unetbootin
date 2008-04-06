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
#ifdef Q_OS_UNIX
#endif

class unetbootin : public QWidget, private Ui::unetbootin
{
	Q_OBJECT

public:
	unetbootin(QWidget *parent = 0);
	QString appDir;
	QString appLoc;
	QString nameDistro;
	QString nameVersion;
	bool isarch64;
	QString targetDrive;
	QString targetPath;
	QString installType;
	QString targetDev;
	QString postinstmsg;
	QString sourcefile, destinfile;
	QString kernelLine, kernelParam, kernelLoc, kernelOpts, initrdLine, initrdLoc, initrdOpts;
	QProgressDialog dlprogress;
	QString sevzcommand;
	#ifdef Q_OS_UNIX
	QString fdiskcommand;
	QString sfdiskcommand;
	QString mssyscommand;
	QString syslinuxcommand;
	#endif
	QPair<QStringList, QStringList> listarchiveconts(QString archivefile);
	void extractfile(QString filepath, QString destinfile, QString archivefile);
	void extractkernel(QString archivefile, QString kernoutputfile, QStringList archivefileconts);
	void extractiso(QString isofile, QString exoutputdir);
	QString getcfgkernargs(QString cfgfile);
	void downloadfile(QString fileurl, QString targetfile);
	QString downloadpagecontents(QString pageurl);
	void sysreboot();
	void callexternapp(QString execFile, QString execParm);
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

public slots:
	void dlprogressupdate(int dlbytes, int maxbytes);
	void dlprogressupdate64(qint64 dlbytes, qint64 maxbytes);
};

#endif

