/*
main.cpp from UNetbootin <http://unetbootin.sourceforge.net>
Copyright (C) Geza Kovacs <geza0kovacs@gmail.com>

This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 2 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License at <http://www.gnu.org/licenses/> for more details.
*/

#include "unetbootin.h"

#ifdef Q_OS_WIN32

void configsysUndo(QString uninstPathL)
{
	if (!QFile::copy(QDir::toNativeSeparators(QString("%1unetbtin/config.sys").arg(uninstPathL)), QDir::toNativeSeparators(QString("%1config.sys").arg(uninstPathL))))
	{
		QFile::remove(QDir::toNativeSeparators(QString("%1config.sys").arg(uninstPathL)));
		QFile::copy(QDir::toNativeSeparators(QString("%1unetbtin/config.sys").arg(uninstPathL)), QDir::toNativeSeparators(QString("%1config.sys").arg(uninstPathL)));
	}
	SetFileAttributesA(QDir::toNativeSeparators(QString("%1config.sys").arg(uninstPathL)).toLocal8Bit(), FILE_ATTRIBUTE_READONLY | FILE_ATTRIBUTE_SYSTEM | FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_ARCHIVE);
}

void bootiniUndo(QString uninstPathL)
{
	if (!QFile::copy(QDir::toNativeSeparators(QString("%1unetbtin/boot.ini").arg(uninstPathL)), QDir::toNativeSeparators(QString("%1boot.ini").arg(uninstPathL))))
		{
			QFile::remove(QDir::toNativeSeparators(QString("%1boot.ini").arg(uninstPathL)));
			QFile::copy(QDir::toNativeSeparators(QString("%1unetbtin/boot.ini").arg(uninstPathL)), QDir::toNativeSeparators(QString("%1boot.ini").arg(uninstPathL)));
		}
	SetFileAttributesW(LPWSTR(QDir::toNativeSeparators(QString("%1boot.ini").arg(uninstPathL)).utf16()), FILE_ATTRIBUTE_READONLY | FILE_ATTRIBUTE_SYSTEM | FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_ARCHIVE);
}

void vistabcdUndo(QString uninstPathL)
{
	QSettings vdtustor("HKEY_LOCAL_MACHINE\\Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\UNetbootin", QSettings::NativeFormat);
	QVariant warch64varL(QVariant::Bool);
	warch64varL = vdtustor.value("WArch64");
	bool warch64L = warch64varL.value<bool>();
	if (warch64L)
	{
		unetbootin::callexternapp(QDir::toNativeSeparators(QString("%1unetbtin/emtxfile.exe").arg(uninstPathL)), QDir::toNativeSeparators(QString("%1unetbtin/vbcdundo.bat runas").arg(uninstPathL)));
	}
	else
	{
		unetbootin::callexternapp(QDir::toNativeSeparators(QString("%1unetbtin/vbcdundo.bat").arg(uninstPathL)), "");
	}
	vdtustor.remove("WArch64");
}

#endif

void clearOutDir(QString pDirToDel)
{
	QDir dirToDel(pDirToDel);
	QStringList rmfileList = dirToDel.entryList(QDir::Files);
	for (int i = 0; i < rmfileList.size(); ++i)
	{
		QFile::setPermissions(QDir::toNativeSeparators(QString("%1/%2").arg(pDirToDel).arg(rmfileList.at(i))), QFile::WriteOther);
		QFile::remove(QDir::toNativeSeparators(QString("%1/%2").arg(pDirToDel).arg(rmfileList.at(i))));
	}
	dirToDel.rmdir(pDirToDel);
}

void ubnUninst()
{
	#ifdef Q_OS_UNIX
	QSettings chkinstL(QSettings::SystemScope, "UNetbootin");
	QString uninstPath = "/";
	QString uninstsubDir = QDir::toNativeSeparators(QString("%1boot/").arg(uninstPath));
	if (QFile::exists(QString("%1ubninit").arg(uninstsubDir)))
		QFile::remove(QString("%1ubninit").arg(uninstsubDir));
	if (QFile::exists(QString("%1ubnkern").arg(uninstsubDir)))
		QFile::remove(QString("%1ubnkern").arg(uninstsubDir));
	if (QFile::exists(QString("%1grub/menu.lst.bak").arg(uninstsubDir)))
	{
		if (QFile::exists(QString("%1grub/menu.lst").arg(uninstsubDir)))
		{
			QFile::remove(QString("%1grub/menu.lst").arg(uninstsubDir));
		}
		QFile::rename(QString("%1grub/menu.lst.bak").arg(uninstsubDir), QString("%1grub/menu.lst").arg(uninstsubDir));
	}
	#endif
	#ifdef Q_OS_WIN32
	QSettings autostrt("HKEY_LOCAL_MACHINE\\Software\\Microsoft\\Windows\\CurrentVersion\\RunOnce", QSettings::NativeFormat);
 	if (autostrt.contains("UNetbootin Uninstaller"))
 	{
 		autostrt.remove("UNetbootin Uninstaller");
	}
	QSettings chkinstL("HKEY_LOCAL_MACHINE\\Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\UNetbootin", QSettings::NativeFormat);
	QVariant uninstvar(QVariant::String);
	uninstvar = chkinstL.value("Location");
	QString uninstPath = uninstvar.value<QString>();
	QString uninstsubDir = QDir::toNativeSeparators(QString("%1unetbtin/").arg(uninstPath));
	if (QSysInfo::WindowsVersion == QSysInfo::WV_32s || QSysInfo::WindowsVersion == QSysInfo::WV_95 || QSysInfo::WindowsVersion == QSysInfo::WV_98 || QSysInfo::WindowsVersion == QSysInfo::WV_Me)
	{
		configsysUndo(uninstPath);
	}
	else if (QSysInfo::WindowsVersion == QSysInfo::WV_NT || QSysInfo::WindowsVersion == QSysInfo::WV_2000 || QSysInfo::WindowsVersion == QSysInfo::WV_XP || QSysInfo::WindowsVersion == QSysInfo::WV_2003 )
	{
		bootiniUndo(uninstPath);
	}
	else if (QSysInfo::WindowsVersion == QSysInfo::WV_VISTA)
	{
		vistabcdUndo(uninstPath);
	}
	else
	{
		configsysUndo(uninstPath);
		bootiniUndo(uninstPath);
		vistabcdUndo(uninstPath);
	}
	#endif
	if (QFile::exists(QString("%1ubnfilel.txt").arg(uninstsubDir)))
	{
		QFile ubnfilelF(QString("%1ubnfilel.txt").arg(uninstsubDir));
		ubnfilelF.open(QIODevice::ReadOnly | QIODevice::Text);
		QTextStream ubnfilelS(&ubnfilelF);
		while (!ubnfilelS.atEnd())
		{
			QFile::remove(QString("%1%2").arg(uninstPath).arg(ubnfilelS.readLine()));
		}
		ubnfilelF.close();
		QFile::remove(QString("%1ubnfilel.txt").arg(uninstsubDir));
	}
	if (QFile::exists(QString("%1ubnpathl.txt").arg(uninstsubDir)))
	{
		QFile ubnpathlF(QString("%1ubnpathl.txt").arg(uninstsubDir));
		ubnpathlF.open(QIODevice::ReadOnly | QIODevice::Text);
		QTextStream ubnpathlS(&ubnpathlF);
		QDir unrdir(uninstPath);
		while (!ubnpathlS.atEnd())
		{
			unrdir.rmdir(ubnpathlS.readLine());
		}
		ubnpathlF.close();
		QFile::remove(QString("%1ubnpathl.txt").arg(uninstsubDir));
	}
	#ifdef Q_OS_WIN32
	clearOutDir(QDir::toNativeSeparators(QString("%1unetbtin").arg(uninstPath)));
	QFile::remove(QDir::toNativeSeparators(QString("%1ubnldr.exe").arg(uninstPath)));
	QFile::remove(QDir::toNativeSeparators(QString("%1ubnldr").arg(uninstPath)));
	QFile::remove(QDir::toNativeSeparators(QString("%1ubnldr.mbr").arg(uninstPath)));
	#endif
	chkinstL.clear();
	QMessageBox finmsgb;
	finmsgb.setIcon(QMessageBox::Information);
	finmsgb.setWindowTitle(uninstaller::tr("Uninstallation Complete"));
	finmsgb.setText(uninstaller::tr("%1 has been uninstalled.").arg(UNETBOOTINB));
 	finmsgb.setStandardButtons(QMessageBox::Ok);
 	switch (finmsgb.exec())
 	{
 		case QMessageBox::Ok:
 			break;
		default:
			break;
 	}
}

QString checkforgraphicalsu(QString graphicalsu)
{
	QProcess whereiscommand;
	whereiscommand.start(QString("whereis %1").arg(graphicalsu));
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
	return "REQCNOTFOUND";
}

int main(int argc, char *argv[])
{
	QApplication app(argc, argv, true);
	QTranslator custranldr;
	QTranslator translator;
	QString tnapplang;
	QStringList ubnappargs = app.arguments().filter(QRegExp("lang=\\w{2,}"));
	if (!ubnappargs.isEmpty())
	{
		tnapplang = ubnappargs.at(0).simplified().remove("lang=").left(2);
	}
	else
	{
		tnapplang = QLocale::system().name().remove(QRegExp("_\\S{0,}")).simplified();
	}
	QDir applocdir(app.applicationDirPath());
	QStringList applocfiles = applocdir.entryList(QStringList() << "*.qm", QDir::Files);
	if (!applocfiles.isEmpty())
	{
		QString custqmfilepath = applocfiles.at(0);
		if (!applocfiles.filter("unetbootin").isEmpty())
		{
			custqmfilepath = applocfiles.filter("unetbootin").at(0);
			if (!applocfiles.filter("unetbootin").filter(tnapplang).isEmpty())
				custqmfilepath = applocfiles.filter("unetbootin").filter(tnapplang).at(0);
		}
		if (custranldr.load(custqmfilepath, app.applicationDirPath()))
			app.installTranslator(&custranldr);
	}
	if (QFile::exists(QString("%1/unetbootin_%2.qm").arg(app.applicationDirPath(), tnapplang)) && translator.load(QString("%1/unetbootin_%2.qm").arg(app.applicationDirPath(), tnapplang)))
	{
		app.installTranslator(&translator);
	}
	else if (QFile::exists(QString(":/unetbootin_%1.qm").arg(tnapplang)) && translator.load(QString(":/unetbootin_%1.qm").arg(tnapplang)))
	{
		app.installTranslator(&translator);
	}
	else
	{
		tnapplang = "en";
	}
	app.installTranslator(&translator);
	#ifdef Q_OS_UNIX
	QProcess whoamip;
	whoamip.start("whoami");
	whoamip.waitForFinished();
	if (QString(whoamip.readAll()).remove("\r").remove("\n") != "root")
	{
		QString gksulocation = checkforgraphicalsu("gksu");
		if (gksulocation != "REQCNOTFOUND")
		{
			QProcess::startDetached(QString("%1 %2").arg(gksulocation, app.applicationFilePath()));
			return 0;
		}
		QString kdesulocation = checkforgraphicalsu("kdesu");
		if (kdesulocation != "REQCNOTFOUND")
		{
			QProcess::startDetached(QString("%1 %2").arg(kdesulocation, app.applicationFilePath()));
			return 0;
		}
		QMessageBox rootmsgb;
		rootmsgb.setIcon(QMessageBox::Warning);
		rootmsgb.setWindowTitle(uninstaller::tr("Must run as root"));
		rootmsgb.setTextFormat(Qt::RichText);
		rootmsgb.setText(uninstaller::tr("%2 must be run as root. Close it, and re-run using either:<br/><b>sudo %1</b><br/>or:<br/><b>su -c '%1'</b>").arg(app.applicationFilePath()).arg(UNETBOOTINB));
		rootmsgb.setStandardButtons(QMessageBox::Ok);
		switch (rootmsgb.exec())
		{
			case QMessageBox::Ok:
				break;
			default:
				break;
		}
	}
	#endif
	#ifdef Q_OS_WIN32
	QSettings chkinst("HKEY_LOCAL_MACHINE\\Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\UNetbootin", QSettings::NativeFormat);
	#endif
	#ifdef Q_OS_UNIX
	QSettings chkinst(QSettings::SystemScope, "UNetbootin");
	#endif
	if (chkinst.contains("Location"))
	{
		QMessageBox uninstmsgb;
		uninstmsgb.setIcon(QMessageBox::Information);
		uninstmsgb.setWindowTitle(uninstaller::tr("%1 Uninstaller").arg(UNETBOOTINB));
		uninstmsgb.setText(uninstaller::tr("%1 is currently installed. Remove the existing version?").arg(UNETBOOTINB));
 		uninstmsgb.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
 		switch (uninstmsgb.exec())
 		{
 			case QMessageBox::Ok:
 			{
 				ubnUninst();
			}
			case QMessageBox::Cancel:
				break;
	 		default:
				break;
 		}
		return 0;
	}
	unetbootin unetbootin;
	unetbootin.appNlang = tnapplang;
	unetbootin.appDir = QDir::toNativeSeparators(QString("%1/").arg(app.applicationDirPath()));
	unetbootin.appLoc = app.applicationFilePath();
	unetbootin.ubninitialize();
	unetbootin.show();
	return app.exec();
}
