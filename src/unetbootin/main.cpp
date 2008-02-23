#include <QApplication>
#include <QSettings>
#include <QProcess>
#include "unetbootin.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QSettings chkinst("HKEY_LOCAL_MACHINE\\Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\UNetbootin", QSettings::NativeFormat);
	if (chkinst.contains("Location"))
	{
//		QSettings chkinst(QSettings::SystemScope, "UNetbootin", "UNetbootin");
//		chkinst.setValue("Location", "C:");
		QProcess uninst;
		QVariant uninstvar(QVariant::String);
		uninstvar = chkinst.value("Location");
		QString uninstpath = uninstvar.value<QString>();
//		QString uninstpath = uninstvar.toString();
//		printf(qPrintable(uninstpath));
//		printf("meomeomeomeomeo");
		uninst.start(QString("%1\\unetbtin\\booteder.exe").arg(uninstpath));
		uninst.waitForFinished(-1);
//		uninst.~QProcess();
//		chkinst.~QSettings();
		return 0;
	}
    unetbootin unetbootin;
    unetbootin.show();
    unetbootin.appDir = app.applicationDirPath();
    return app.exec();
}
