#include <QApplication>
#include <QSettings>
#include <QProcess>
#include <QMessageBox>
#include <QFile>
#include <QDir>
#include <QSysInfo>
#include <QStringList>
#include "unetbootin.h"

void configsysUndo(QString uninstPathL)
{
	if (!QFile::copy(QDir::toNativeSeparators(QString("%1/unetbtin/config.sys").arg(uninstPathL)), QDir::toNativeSeparators(QString("%1/config.sys").arg(uninstPathL))))
	{
		QFile::remove(QDir::toNativeSeparators(QString("%1/config.sys").arg(uninstPathL)));
		QFile::copy(QDir::toNativeSeparators(QString("%1/unetbtin/config.sys").arg(uninstPathL)), QDir::toNativeSeparators(QString("%1/config.sys").arg(uninstPathL)));
	}
}

void bootiniUndo(QString uninstPathL)
{
//	TODO
}

void vistabcdUndo(QString uninstPathL)
{
//	TODO
}

void clearOutDir(QString pDirToDel)
{
	QDir dirToDel(pDirToDel);
	QStringList rmfileList = dirToDel.entryList(QDir::Files);
	for (int i = 0; i < rmfileList.size(); ++i)
	{
		QFile::remove(QDir::toNativeSeparators(QString("%1/%2").arg(pDirToDel).arg(rmfileList.at(i))));
	}
	dirToDel.rmdir(pDirToDel);
}

void ubnUninst()
{
	QSettings autostrt("HKEY_LOCAL_MACHINE\\Software\\Microsoft\\Windows\\CurrentVersion\\RunOnce", QSettings::NativeFormat);
 	if (autostrt.contains("UNetbootin Uninstaller"))
 	{
 		autostrt.remove("UNetbootin Uninstaller");
	}
	QSettings chkinstL("HKEY_LOCAL_MACHINE\\Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\UNetbootin", QSettings::NativeFormat);
	QVariant uninstvar(QVariant::String);
	uninstvar = chkinstL.value("Location");
	QString uninstPath = uninstvar.value<QString>();
	QSysInfo::WinVersion wvr = QSysInfo::WindowsVersion;
	if (wvr == QSysInfo::WV_32s || wvr == QSysInfo::WV_95 || wvr == QSysInfo::WV_98 || wvr == QSysInfo::WV_Me)
	{
		configsysUndo(uninstPath);
	}
	else if (wvr == QSysInfo::WV_NT || wvr == QSysInfo::WV_2000 || wvr == QSysInfo::WV_XP || wvr == QSysInfo::WV_2003 )
	{
		bootiniUndo(uninstPath);
	}
	else if (wvr == QSysInfo::WV_VISTA)
	{
		vistabcdUndo(uninstPath);
	}
	else
	{
		configsysUndo(uninstPath);
		bootiniUndo(uninstPath);
		vistabcdUndo(uninstPath);
	}
	clearOutDir(QDir::toNativeSeparators(QString("%1/unetbtin").arg(uninstPath)));
	QFile::remove(QDir::toNativeSeparators(QString("%1/ubnldr.exe").arg(uninstPath)));
	QFile::remove(QDir::toNativeSeparators(QString("%1/ubnldr").arg(uninstPath)));
	QFile::remove(QDir::toNativeSeparators(QString("%1/ubnldr.mbr").arg(uninstPath)));
	chkinstL.clear();
	QMessageBox finmsgb;
	finmsgb.setWindowTitle("Uninstallation Complete");
	finmsgb.setText("UNetbootin has been uninstalled.");
 	finmsgb.setStandardButtons(QMessageBox::Ok);
 	switch (finmsgb.exec())
 	{
 		case QMessageBox::Ok:
 			break;
		default:
			break;
 	}
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QSettings chkinst("HKEY_LOCAL_MACHINE\\Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\UNetbootin", QSettings::NativeFormat);
	if (chkinst.contains("Location"))
	{
		QMessageBox uninstmsgb;
		uninstmsgb.setWindowTitle("UNetbootin Uninstaller");
		uninstmsgb.setText("UNetbootin is currently installed. Remove the existing version?");
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
    unetbootin.show();
    unetbootin.appDir = QDir::toNativeSeparators(QString("%1/").arg(app.applicationDirPath()));
    unetbootin.appLoc = app.applicationFilePath();
    return app.exec();
}
