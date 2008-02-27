#include <QApplication>
#include <QSettings>
#include <QProcess>
#include <QMessageBox>
#include "unetbootin.h"

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
 				QSettings autostrt("HKEY_LOCAL_MACHINE\\Software\\Microsoft\\Windows\\CurrentVersion\\RunOnce", QSettings::NativeFormat);
 				if (autostrt.contains("UNetbootin Uninstaller"))
 				{
 					autostrt.remove("UNetbootin Uninstaller");
				}
 				QProcess uninst;
				QVariant uninstvar(QVariant::String);
				uninstvar = chkinst.value("Location");
				QString uninstpath = uninstvar.value<QString>();
				uninst.start(QString("%1\\unetbtin\\uninst.exe").arg(uninstpath));
				uninst.waitForFinished(-1);
			}
			case QMessageBox::Cancel:
				break;
	 		default:
				break;
 		}
//		chkinst.clear();
		return 0;
	}
    unetbootin unetbootin;
    unetbootin.show();
    unetbootin.appDir = QString("%1\\").arg(app.applicationDirPath());
    return app.exec();
}
