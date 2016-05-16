/*
main.cpp from UNetbootin <http://unetbootin.sourceforge.net>
Copyright (C) 2007-2008 Geza Kovacs <geza0kovacs@gmail.com>

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
	if (QFile::exists(QString("%1grub/grub.cfg.bak").arg(uninstsubDir)))
	{
		if (QFile::exists(QString("%1grub/grub.cfg").arg(uninstsubDir)))
		{
			QFile::remove(QString("%1grub/grub.cfg").arg(uninstsubDir));
		}
		QFile::rename(QString("%1grub/grub.cfg.bak").arg(uninstsubDir), QString("%1grub/grub.cfg").arg(uninstsubDir));
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
	whereiscommand.start(QString("which %1").arg(graphicalsu));
	whereiscommand.waitForFinished(-1);
	QString commandbinpath = QString(whereiscommand.readAll()).trimmed();
	if (!commandbinpath.isEmpty() && QFile::exists(commandbinpath))
		return commandbinpath;
	else
		return "REQCNOTFOUND";
}

int main(int argc, char **argv)
{
	QApplication app(argc, argv, true);
	QTranslator custranldr;
	QTranslator translator;
	QString tnapplang;
	QString tnappcoun;
	QString clangcode = "";
	QStringList allappargs = app.arguments();
	QList<QPair<QString, QString> > oppairs;
	for (QList<QString>::const_iterator i = allappargs.constBegin(); i < allappargs.constEnd(); ++i)
	{
		if (i->count('=') == 1)
			oppairs.append(QPair<QString, QString>(i->section('=', 0, 0).simplified(), i->section('=',1, 1).simplified()));
	}
	for (QList<QPair<QString, QString> >::const_iterator i = oppairs.constBegin(); i < oppairs.constEnd(); ++i)
	{
		if (i->first.contains("lang", Qt::CaseInsensitive))
		{
			clangcode = i->second;
			tnapplang = clangcode.left(2);
			if (clangcode.contains('_') && clangcode.size() == 5)
			{
				tnappcoun = clangcode.section('_', -1, -1);
			}
			break;
		}
	}
	if (clangcode.isEmpty())
	{
		clangcode = QLocale::system().name();
		tnapplang = clangcode.left(2);
		if (clangcode.contains('_') && clangcode.size() == 5)
		{
			tnappcoun = clangcode.section('_', -1, -1);
		}
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
			{
				custqmfilepath = applocfiles.filter("unetbootin").filter(tnapplang).at(0);
				if (!tnappcoun.isEmpty() && !applocfiles.filter("unetbootin").filter(tnapplang).filter(tnappcoun).isEmpty())
					custqmfilepath = applocfiles.filter("unetbootin").filter(tnapplang).filter(tnappcoun).at(0);
			}
		}
		if (custranldr.load(custqmfilepath, app.applicationDirPath()))
			app.installTranslator(&custranldr);
	}
	if (!tnappcoun.isEmpty() && QFile::exists(QString("%1/unetbootin_%2_%3.qm").arg(app.applicationDirPath()).arg(tnapplang).arg(tnappcoun)) && translator.load(QString("%1/unetbootin_%2_%3.qm").arg(app.applicationDirPath()).arg(tnapplang).arg(tnappcoun)))
	{
		app.installTranslator(&translator);
	}
	else if (!tnappcoun.isEmpty() && QFile::exists(QString(":/unetbootin_%1_%2.qm").arg(tnapplang).arg(tnappcoun)) && translator.load(QString(":/unetbootin_%1_%2.qm").arg(tnapplang).arg(tnappcoun)))
	{
		app.installTranslator(&translator);
	}
	else if (!tnappcoun.isEmpty() && QFile::exists(QString("/usr/share/unetbootin/unetbootin_%1_%2.qm").arg(tnapplang).arg(tnappcoun)) && translator.load(QString("/usr/share/unetbootin/unetbootin_%1_%2.qm").arg(tnapplang).arg(tnappcoun)))
	{
		app.installTranslator(&translator);
	}
	else if (QFile::exists(QString("%1/unetbootin_%2.qm").arg(app.applicationDirPath(), tnapplang)) && translator.load(QString("%1/unetbootin_%2.qm").arg(app.applicationDirPath(), tnapplang)))
	{
		app.installTranslator(&translator);
	}
	else if (QFile::exists(QString(":/unetbootin_%1.qm").arg(tnapplang)) && translator.load(QString(":/unetbootin_%1.qm").arg(tnapplang)))
	{
		app.installTranslator(&translator);
	}
	else if (QFile::exists(QString("/usr/share/unetbootin/unetbootin_%1.qm").arg(tnapplang)) && translator.load(QString("/usr/share/unetbootin/unetbootin_%1.qm").arg(tnapplang)))
	{
		app.installTranslator(&translator);
	}
	else
	{
		tnapplang = "en";
		tnappcoun = "US";
		clangcode = "en_US";
	}
	app.installTranslator(&translator);
	if (QObject::tr("LeftToRight") == "RightToLeft")
		app.setLayoutDirection(Qt::RightToLeft);
	#ifdef Q_OS_UNIX
	bool disabledrootcheck = false;
	for (QList<QPair<QString, QString> >::const_iterator i = oppairs.constBegin(); i < oppairs.constEnd(); ++i)
	{
		if (i->first.contains("rootcheck", Qt::CaseInsensitive))
		{
			if (i->second.contains('n', Qt::CaseInsensitive))
				disabledrootcheck = true;
			break;
		}
	}
	if (!disabledrootcheck)
	{
		QProcess whoamip;
		whoamip.start("whoami");
		whoamip.waitForFinished();
		if (QString(whoamip.readAll()).remove("\r").remove("\n") != "root")
		{
			QString argsconc = "";
            QString argsconcSingleQuote = "";
			for (int i = 1; i < allappargs.size(); ++i)
			{
				argsconc += QString("\"%1\" ").arg(allappargs.at(i));
                argsconcSingleQuote += QString("'%1' ").arg(allappargs.at(i));
			}
            argsconc += "\"rootcheck=no\"";
            argsconcSingleQuote += "'rootcheck=no'";
#ifdef Q_OS_LINUX
            QString gksuarg1;
            gksuarg1 += QString("bash -c 'QT_X11_NO_MITSHM=1 ");
            gksuarg1 += QString("%1 %2").arg(app.applicationFilePath()).arg(argsconc);
            gksuarg1 += QString("'");
            QStringList gksuargs;
            gksuargs.append(gksuarg1);

            /*
            QString pkexeclocation = checkforgraphicalsu("pkexec");
            if (pkexeclocation != "REQCNOTFOUND" && app.applicationFilePath() == "/usr/bin/unetbootin" && QFile::exists("/usr/share/polkit-1/actions/org.unetbootin.pkexec.unetbootin.policy"))
            {
                //QProcess::startDetached(QString("%1 %2 %3").arg(gksulocation).arg(app.applicationFilePath()).arg(argsconc));
                QProcess::startDetached(QString("%1 %2 %3").arg(pkexeclocation).arg(app.applicationFilePath()).arg(argsconc));
                return 0;
            }
            */
            QString gksulocation = checkforgraphicalsu("gksu");
			if (gksulocation != "REQCNOTFOUND")
			{
                //QProcess::startDetached(QString("%1 %2 %3").arg(gksulocation).arg(app.applicationFilePath()).arg(argsconc));
                QProcess::startDetached(gksulocation, gksuargs);
				return 0;
			}
			QString kdesulocation = checkforgraphicalsu("kdesu");
			if (kdesulocation != "REQCNOTFOUND")
			{
                //QProcess::startDetached(QString("%1 %2 %3").arg(kdesulocation).arg(app.applicationFilePath()).arg(argsconc));
                QProcess::startDetached(kdesulocation, gksuargs);
				return 0;
			}
			QString gnomesulocation = checkforgraphicalsu("gnomesu");
			if (gnomesulocation != "REQCNOTFOUND")
			{
                //QProcess::startDetached(QString("%1 %2 %3").arg(gnomesulocation).arg(app.applicationFilePath()).arg(argsconc));
                QProcess::startDetached(gnomesulocation, gksuargs);
				return 0;
			}
			QString kdesudolocation = checkforgraphicalsu("kdesudo");
			if (kdesudolocation != "REQCNOTFOUND")
			{
                //QProcess::startDetached(QString("%1 %2 %3").arg(kdesudolocation).arg(app.applicationFilePath()).arg(argsconc));
                QProcess::startDetached(kdesudolocation, gksuargs);
				return 0;
			}
			QMessageBox rootmsgb;
			rootmsgb.setIcon(QMessageBox::Warning);
			rootmsgb.setWindowTitle(uninstaller::tr("Must run as root"));
			rootmsgb.setTextFormat(Qt::RichText);
            rootmsgb.setText(uninstaller::tr("%2 must be run as root. Close it, and re-run using either:<br/><b>sudo QT_X11_NO_MITSHM=1 %1</b><br/>or:<br/><b>su -c 'QT_X11_NO_MITSHM=1 %1'</b>").arg(app.applicationFilePath()).arg(UNETBOOTINB));
			rootmsgb.setStandardButtons(QMessageBox::Ok);
			switch (rootmsgb.exec())
			{
				case QMessageBox::Ok:
					break;
				default:
					break;
			}
#endif
#ifdef Q_OS_MAC
            /*
			QProcess osascriptProc;
			osascriptProc.start("osascript");
			osascriptProc.write(QString("do shell script \""+app.applicationFilePath()+"\" with administrator privileges\n").toAscii().data());
			osascriptProc.closeWriteChannel();
			osascriptProc.waitForFinished(-1);
            */
            //qDebug() << QString("osascript -e 'do shell script \"%1 %2\" with administrator privileges'").arg(app.applicationFilePath()).arg(argsconc);
            //QProcess::startDetached(QString("osascript -e 'do shell script \"%1 %2\" with administrator privileges'").arg(app.applicationFilePath()).arg(argsconc));
            QProcess::startDetached("osascript", QStringList() << "-e" << QString("do shell script \"'%1' %2\" with administrator privileges").arg(app.applicationFilePath()).arg(argsconcSingleQuote));
            return 0;
#endif
		}
	}
	#endif
	#ifdef Q_OS_WIN32
	QSettings chkinst("HKEY_LOCAL_MACHINE\\Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\UNetbootin", QSettings::NativeFormat);
	#endif
	#ifdef Q_OS_LINUX
	QSettings chkinst(QSettings::SystemScope, "UNetbootin");
	#endif
#ifndef Q_OS_MAC
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
#endif
	unetbootin unetbootin;
	unetbootin.appNlang = tnapplang;
	unetbootin.appDir = QDir::toNativeSeparators(QString("%1/").arg(app.applicationDirPath()));
	unetbootin.appLoc = app.applicationFilePath();
	QIcon icon;
	icon.addFile(":/unetbootin_16.png", QSize(16,16));
	icon.addFile(":/unetbootin_22.png", QSize(22,22));
	icon.addFile(":/unetbootin_24.png", QSize(24,24));
	icon.addFile(":/unetbootin_32.png", QSize(32,32));
	icon.addFile(":/unetbootin_48.png", QSize(48,48));
#ifdef Q_OS_LINUX
	icon.addFile("/usr/share/pixmaps/unetbootin.png");
	icon.addFile("/usr/share/pixmaps/unetbootin.xpm");
#endif
	unetbootin.setWindowIcon(icon);
	QObject::connect(&app, SIGNAL(lastWindowClosed()), &unetbootin, SLOT(killApplication()));
	bool automate = unetbootin.ubninitialize(oppairs);
	unetbootin.show();
	if (automate)
		QTimer::singleShot(0, &unetbootin, SLOT(on_okbutton_clicked()));
	return app.exec();
}
