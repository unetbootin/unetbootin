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
		QMessageBox msgBox;
		msgBox.setWindowTitle("UNetbootin Uninstaller");
		msgBox.setText("UNetbootin is currently installed. Remove the existing version?");
 		msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
 		switch (msgBox.exec())
 		{
 			case QMessageBox::Ok:
 			{
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
//	Q_INIT_RESOURCE(unetbootin);
    unetbootin unetbootin;
    unetbootin.show();
    unetbootin.appDir = QString("%1\\").arg(app.applicationDirPath());
    return app.exec();
}
