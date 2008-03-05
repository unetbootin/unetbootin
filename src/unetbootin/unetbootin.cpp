#include <QFile>
#include <QFileDialog>
#include <QTextStream>
#include <QProcess>
#include <QSettings>
#include <QSysInfo>
#include <QMessageBox>
#include "unetbootin.h"
//#include "datastor.h"
#include "datastor.cpp"
#include <windows.h>

unetbootin::unetbootin(QWidget *parent)
    : QWidget(parent)
{
    setupUi(this);
}

void unetbootin::on_FloppyFileSelector_clicked()
{
    nameFloppy = QFileDialog::getOpenFileName(this, "Open File", QDir::homePath());
    FloppyPath->clear();
    FloppyPath->insert(nameFloppy);
    radioFloppy->setChecked(1);
}

void unetbootin::on_KernelFileSelector_clicked()
{
    nameKernel = QFileDialog::getOpenFileName(this, "Open File", QDir::homePath());
    KernelPath->clear();
    KernelPath->insert(nameKernel);
    radioManual->setChecked(1);
}

void unetbootin::on_InitrdFileSelector_clicked()
{
    nameInitrd = QFileDialog::getOpenFileName(this, "Open File", QDir::homePath());
    InitrdPath->clear();
    InitrdPath->insert(nameInitrd);
    radioManual->setChecked(1);
}

void unetbootin::on_cancelbutton_clicked()
{
    close();
}

void unetbootin::on_okbutton_clicked()
{
	runinst();
}

void unetbootin::downloadfile(QString fileurl, QString targetfile)
{
    QFile file1(QString("%1dlurl.txt").arg(targetPath));
    file1.open(QIODevice::WriteOnly | QIODevice::Text);
    QTextStream out1(&file1);
    out1 << fileurl;
    file1.close();
    QFile file2(QString("%1outfile.txt").arg(targetPath));
    file2.open(QIODevice::WriteOnly | QIODevice::Text);
    QTextStream out2(&file2);
    out2 << targetfile;
    file2.close();
    QProcess dlprocess;
    dlprocess.start(QString("%1downlder.exe").arg(targetPath));
    dlprocess.waitForFinished(-1);
}

void unetbootin::sysreboot()
{
	HANDLE hToken;
	TOKEN_PRIVILEGES tkp;
	OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken);
	LookupPrivilegeValue(NULL, SE_SHUTDOWN_NAME, &tkp.Privileges[0].Luid);
	tkp.PrivilegeCount = 1;
	tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
	AdjustTokenPrivileges(hToken, FALSE, &tkp, 0, (PTOKEN_PRIVILEGES)NULL, 0);
	ExitWindowsEx(EWX_REBOOT, EWX_FORCE);
}

void unetbootin::configsysEdit()
{
	QFile::copy(QDir::toNativeSeparators(QString("%1/config.sys").arg(targetDrive)), QString("%1config.sys").arg(targetPath));
	QFile::copy(QDir::toNativeSeparators(QString("%1/config.sys").arg(targetDrive)), QString("%1confignw.txt").arg(targetPath));
	QFile confignwFile(QString("%1confignw.txt").arg(targetPath));
	QFile configsysFile(QDir::toNativeSeparators(QString("%1/config.sys").arg(targetDrive)));
	confignwFile.open(QIODevice::ReadWrite | QIODevice::Text);
	configsysFile.open(QIODevice::ReadWrite | QIODevice::Text);
	configsysFile.setPermissions(QFile::ReadOther | QFile::WriteOther);
	QTextStream confignwOut(&confignwFile);
	QTextStream configsysOut(&configsysFile);
	QString configsysText = QString("[menu]\n"
	"menucolor=15,0\n"
	"menuitem=windows,Windows\n"
	"menuitem=grub,UNetbootin\n"
	"menudefault=windows,30\n"
	"[grub]\n"
	"device=ubnldr.exe\n"
	"[windows]\n%1").arg(configsysOut.readAll());
	confignwOut << configsysText << endl;
	if (!QFile::copy(QString("%1confignw.txt").arg(targetPath), QDir::toNativeSeparators(QString("%1/config.sys").arg(targetDrive))))
	{
		configsysFile.remove();
		QFile::copy(QString("%1confignw.txt").arg(targetPath), QDir::toNativeSeparators(QString("%1/config.sys").arg(targetDrive)));
	}
}

void unetbootin::bootiniEdit()
{
//	TODO
}

void unetbootin::vistabcdEdit()
{
//	TODO
}

void unetbootin::instIndvfl(QString dstfName, QByteArray qbav)
{
	QFile dstFile;
	dstFile.setFileName(QString("%1%2").arg(targetPath).arg(dstfName));
	dstFile.open(QIODevice::ReadWrite);
	dstFile.write(qbav);
	dstFile.close();
}

void unetbootin::wInstfiles()
{
	#include "datastor.cpp"
	instIndvfl(QString("ubnldr"), QByteArray::fromRawData(ubnldr, sizeof(ubnldr)));
}

void unetbootin::runinst()
{
	QString kernelLine("kernel");
	QString kernelParam;
	QString kernelLoc("/unetbtin/ubnkern");
	QString kernelOpts;
	QString initrdLine("initrd");
	QString initrdLoc("/unetbtin/ubninit");
	QString initrdOpts;
	installType = typeselect->currentText();
    targetDrive = driveselect->currentText();
    targetPath = QDir::toNativeSeparators(QString("%1/unetbtin/").arg(targetDrive));
	QDir dir;
    dir.mkpath(targetPath);
	wInstfiles();
	QFile dstFile(QString("%1%2").arg(targetPath).arg("ubnldr"));
	dstFile.open(QIODevice::ReadWrite);
	dstFile.write(ubnldr);
	dstFile.close();
    /*
//	QDir copyFileListD(":/");
//	QStringList copyFileList = copyFileListD.entryList(QDir::Files);
//	QListIterator<QString> copyFileListI(copyFileList);
	while (copyFileListI.hasNext())
	{
		char ch;
		while (!QFile(QString(":/%1").arg(copyFileListI.next())).atEnd())
		{
			QFile(QString(":/%1").arg(copyFileListI.next())).getChar(&ch);
			QFile(QString("%1%2").arg(targetPath).arg(copyFileListI.next())).putChar(ch);
		}
	}
//	QStringList copyFileList;
//	copyFileList << "ubnldr";
//	QFile srcFile;
	QFile dstFile;
//	QDataStream byteCopyFiles;
//	char ch;
	for (int i = 0; i < copyFileList.size(); ++i)
	{
//		srcFile.setFileName(QString(":/%1").arg(copyFileList.at(i)));
		dstFile.setFileName(QString("%1%2").arg(targetPath).arg(copyFileList.at(i)));
//		srcFile.open(QIODevice::ReadWrite);
		dstFile.open(QIODevice::ReadWrite);
//		byteCopyFiles.setDevice(&QFile(QString(":/%1").arg(copyFileList.at(i))));
//		char ch;
//		QByteArray bts;
//		QFile(QString(":/%1").arg(copyFileList.at(i))).open(QIODevice::ReadWrite);
//		QFile(QString("%1%2").arg(targetPath).arg(copyFileList.at(i))).open(QIODevice::ReadWrite);
//		while (!QFile(QString(":/%1").arg(copyFileList.at(i))).atEnd())
//		{
//		QFile(QString(":/%1").arg(copyFileList.at(i))).read();
//		QFile(QString("%1%2").arg(targetPath).arg(copyFileList.at(i))).write(QFile(QString(":/%1").arg(copyFileList.at(i))).readAll());
		while(!srcFile.atEnd())
		{
//			srcFile.getChar(&ch);
			dstFile.putChar(ch);
 		}
//		dstFile.write(srcFile.readAll());
		srcFile.close();
		dstFile.close();
//		}
	
//		QFile::copy(QString(":/%1").arg(copyFileList.at(i)), QString("%1%2").arg(targetPath).arg(copyFileList.at(i)));
		QFile::setPermissions(QString("%1%2").arg(targetPath).arg(copyFileList.at(i)), QFile::ReadOther | QFile::WriteOther);
	}
	*/
	/*
	QFile::copy(":/booteder.exe", QString("%1booteder.exe").arg(targetPath));
	QFile::copy(":/bootedit.bat", QString("%1bootedit.bat").arg(targetPath));
	QFile::copy(":/bootedit.pif", QString("%1bootedit.pif").arg(targetPath));
	QFile::copy(":/bootundo.bat", QString("%1bootundo.bat").arg(targetPath));
	QFile::copy(":/bootundo.pif", QString("%1bootundo.pif").arg(targetPath));
	QFile::copy(":/config.sup", QString("%1config.sup").arg(targetPath));
	QFile::copy(":/downlder.exe", QString("%1downlder.exe").arg(targetPath));
	QFile::copy(":/emtxfile.exe", QString("%1emtxfile.exe").arg(targetPath));
	QFile::copy(":/memdisk", QString("%1memdisk").arg(targetPath));
	QFile::copy(":/runxfile.exe", QString("%1runxfile.exe").arg(targetPath));
	QFile::copy(":/tr.exe", QString("%1tr.exe").arg(targetPath));
	QFile::copy(":/ubnldr", QString("%1ubnldr").arg(targetPath));
	QFile::copy(":/ubnldr.exe", QString("%1ubnldr.exe").arg(targetPath));
	QFile::copy(":/ubnldr.mbr", QString("%1ubnldr.mbr").arg(targetPath));
	QFile::copy(":/vbcdedit.bat", QString("%1vbcdedit.bat").arg(targetPath));
	QFile::copy(":/vbcdedit.pif", QString("%1vbcdedit.pif").arg(targetPath));
	QFile::copy(":/vbcdundo.bat", QString("%1vbcdundo.bat").arg(targetPath));
	QFile::copy(":/vbcdundo.pif", QString("%1vbcdundo.pif").arg(targetPath));
	*/
	QFile::copy(appLoc, QDir::toNativeSeparators(QString("%1/unetbtin.exe").arg(targetDrive)));
	QFile::copy(QString("%1ubnldr.exe").arg(targetPath), QDir::toNativeSeparators(QString("%1/ubnldr.exe").arg(targetDrive)));
    QFile::copy(QString("%1ubnldr").arg(targetPath), QDir::toNativeSeparators(QString("%1/ubnldr").arg(targetDrive)));
    QFile::copy(QString("%1ubnldr.mbr").arg(targetPath), QDir::toNativeSeparators(QString("%1/ubnldr.mbr").arg(targetDrive)));
    close();
    if (radioFloppy->isChecked())
    {
    	QFile::copy(QString("%1memdisk").arg(targetPath), QString("%1ubnkern").arg(targetPath));
    	QFile::copy(FloppyPath->text(), QString("%1ubninit").arg(targetPath));
    }
    if (radioManual->isChecked())
    {
    	QFile::copy(KernelPath->text(), QString("%1ubnkern").arg(targetPath));
    	QFile::copy(InitrdPath->text(), QString("%1ubninit").arg(targetPath));
		kernelOpts = OptionEnter->text();
    }
    if (radioDistro->isChecked())
    {
        nameDistro = distroselect->currentText();
		#include "distrolst.cpp"
    }
    if (installType == "Hard Disk")
    {
    	QFile menulst(QString("%1menu.lst").arg(targetPath));
    	menulst.open(QIODevice::WriteOnly | QIODevice::Text);
		QTextStream menulstout(&menulst);
		QString menulstxt = QString("default 0\n"
		"timeout 3\n"
		"title UNetbootin\n"
		"find --set-root /unetbtin/ubnkern\n"
		"%1 %2 %3 %4\n"
		"%5 %6 %7\n"
		"boot").arg(kernelLine, kernelParam, kernelLoc, kernelOpts, initrdLine, initrdLoc, initrdOpts);
		menulstout << menulstxt << endl;
		menulst.close();
//		QProcess inprocess;
//		inprocess.start(QString("%1booteder.exe").arg(targetPath));
//		inprocess.waitForFinished(-1);
    	QSettings install("HKEY_LOCAL_MACHINE\\Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\UNetbootin", QSettings::NativeFormat);
    	install.setValue("Location", targetDrive);
    	install.setValue("DisplayName", "UNetbootin");
    	install.setValue("UninstallString", QDir::toNativeSeparators(QString("%1/unetbtin.exe").arg(targetDrive)));
    	QSettings runonce("HKEY_LOCAL_MACHINE\\Software\\Microsoft\\Windows\\CurrentVersion\\RunOnce", QSettings::NativeFormat);
    	runonce.setValue("UNetbootin Uninstaller", QDir::toNativeSeparators(QString("%1/unetbtin.exe").arg(targetDrive)));
		QSysInfo::WinVersion wvr = QSysInfo::WindowsVersion;
		if (wvr == QSysInfo::WV_32s || wvr == QSysInfo::WV_95 || wvr == QSysInfo::WV_98 || wvr == QSysInfo::WV_Me)
		{
			configsysEdit();
		}
		else if (wvr == QSysInfo::WV_NT || wvr == QSysInfo::WV_2000 || wvr == QSysInfo::WV_XP || wvr == QSysInfo::WV_2003 )
		{
			bootiniEdit();
		}
		else if (wvr == QSysInfo::WV_VISTA)
		{
			vistabcdEdit();
		}
		else
		{
			configsysEdit();
			bootiniEdit();
			vistabcdEdit();
		}
		QMessageBox rebootmsgb;
    	rebootmsgb.setWindowTitle("Reboot Now?");
		rebootmsgb.setText("After rebooting, select the UNetbootin menu entry to boot.\nReboot now?");
 		rebootmsgb.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
 		switch (rebootmsgb.exec())
 		{
 			case QMessageBox::Ok:
 			{
 				unetbootin::sysreboot();
			}
			case QMessageBox::Cancel:
				break;
	 		default:
				break;
   		}
  	}
/*
	if (installType == "USB Drive")
	{
		TODO
	}
*/
}
