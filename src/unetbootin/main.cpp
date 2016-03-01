/*
main.cpp from UNetbootin <http://unetbootin.sourceforge.net>
Copyright (C) 2007-2008 Geza Kovacs <geza0kovacs@gmail.com>

This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 2 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License at <http://www.gnu.org/licenses/> for more details.
*/

#include "unetbootin.h"
static const QString REMIXOS_HDD_INSTALL_DIR = "RemixOS";

#ifdef Q_OS_WIN32




#endif

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
			QString gksulocation = checkforgraphicalsu("gksu");
			if (gksulocation != "REQCNOTFOUND")
			{
				QProcess::startDetached(QString("%1 %2 %3").arg(gksulocation).arg(app.applicationFilePath()).arg(argsconc));
				return 0;
			}
			QString kdesulocation = checkforgraphicalsu("kdesu");
			if (kdesulocation != "REQCNOTFOUND")
			{
				QProcess::startDetached(QString("%1 %2 %3").arg(kdesulocation).arg(app.applicationFilePath()).arg(argsconc));
				return 0;
			}
			QString gnomesulocation = checkforgraphicalsu("gnomesu");
			if (gnomesulocation != "REQCNOTFOUND")
			{
				QProcess::startDetached(QString("%1 %2 %3").arg(gnomesulocation).arg(app.applicationFilePath()).arg(argsconc));
				return 0;
			}
			QString kdesudolocation = checkforgraphicalsu("kdesudo");
			if (kdesudolocation != "REQCNOTFOUND")
			{
				QProcess::startDetached(QString("%1 %2 %3").arg(kdesudolocation).arg(app.applicationFilePath()).arg(argsconc));
				return 0;
			}
			QMessageBox rootmsgb;
			rootmsgb.setIcon(QMessageBox::Warning);
			rootmsgb.setWindowTitle(uninstaller::tr("Must run as root"));
			rootmsgb.setTextFormat(Qt::RichText);
			rootmsgb.setText(uninstaller::tr("%2 must be run as root. Close it, and re-run using either:<br/><b>sudo %1</b><br/>or:<br/><b>su - -c '%1'</b>").arg(app.applicationFilePath()).arg(UNETBOOTINB));
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
	unetbootin unetbootin;
    QString selfName = QString(argv[0]);
	if (selfName.contains("Uninstall")) {
        if (unetbootin::CHECK_INSTALL_NO_INSTALLED == unetbootin.checkInstall()) {
            QMessageBox uninstmsgb;
            uninstmsgb.setIcon(QMessageBox::Warning);
            uninstmsgb.setWindowTitle(uninstaller::tr("%1 Uninstaller").arg(UNETBOOTINB));
            uninstmsgb.setText(uninstaller::tr("%1 is not installed.").arg(UNETBOOTINB));
            uninstmsgb.setStandardButtons(QMessageBox::Ok);
            uninstmsgb.exec();
        }
        return 0;
	}
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
