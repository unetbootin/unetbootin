#include "unetbootin.h"

#include "ubnldr.cpp"
#include "memdisk.cpp"
#ifdef Q_OS_WIN32
#include "ubnldrexe.cpp"
#include "ubnldrmbr.cpp"
#include "emtxfileexe.cpp"
#include "syslinuxexe.cpp"
#endif

unetbootin::unetbootin(QWidget *parent)
    : QWidget(parent)
{
    setupUi(this);
    driveselect->addItem(QDir::toNativeSeparators(QDir::rootPath()).toUpper());
/*
    #ifdef Q_OS_WIN32
    if (QSysInfo::WindowsVersion == QSysInfo::WV_32s || QSysInfo::WindowsVersion == QSysInfo::WV_95 || QSysInfo::WindowsVersion == QSysInfo::WV_98 || QSysInfo::WindowsVersion == QSysInfo::WV_Me)
	{
		typeselect->removeItem(1);
	}
	#endif
*/
}

void unetbootin::on_typeselect_currentIndexChanged(int typeselectIndex)
{
	if (typeselectIndex == 0)
	{
		driveselect->clear();
		driveselect->addItem(QDir::toNativeSeparators(QDir::rootPath()).toUpper());
	}
	if (typeselectIndex == 1)
	{
		driveselect->clear();
		#ifdef Q_OS_WIN32
		QFileInfoList extdrivesList = QDir::drives();
		for (int i = 0; i < extdrivesList.size(); ++i)
		{
			if (QDir::toNativeSeparators(extdrivesList.at(i).path().toUpper()) != QDir::toNativeSeparators(QDir::rootPath().toUpper()))
			{
				driveselect->addItem(QDir::toNativeSeparators(extdrivesList.at(i).path().toUpper()));
			}
		}
		#endif
		#ifdef Q_OS_UNIX
		// TODO drive detection via fdisk -l
		#endif
	}
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
	sourcefile = fileurl;
	destinfile = targetfile;
	QHttp dlhttp;
	QUrl dlurl(fileurl);
	dlprogress.setWindowTitle(QObject::tr("Downloading..."));
	dlprogress.setLabelText(QObject::tr("Downloading files, please wait...\nSource: %1\nDestination: %2\nDownloaded: 0 bytes").arg(fileurl).arg(targetfile));
	connect(&dlhttp, SIGNAL(done(bool)), &dlprogress, SLOT(close()));
	connect(&dlhttp, SIGNAL(dataReadProgress(int, int)), this, SLOT(dlprogressupdate(int, int)));
	QFile dloutfile(targetfile);
	dloutfile.open(QIODevice::WriteOnly);
	dlhttp.setHost(dlurl.host());
	dlhttp.get(dlurl.path(), &dloutfile);
	dlprogress.exec();
	dlhttp.close();
}

void unetbootin::dlprogressupdate(int dlbytes, int maxbytes)
{
	dlprogress.setValue(dlbytes);
	dlprogress.setMaximum(maxbytes);
	dlprogress.setLabelText(QObject::tr("Downloading files, please wait...\nSource: %1\nDestination: %2\nDownloaded: %3 of %4 bytes").arg(sourcefile).arg(destinfile).arg(dlbytes).arg(maxbytes));
}

void unetbootin::sysreboot()
{
	#ifdef Q_OS_WIN32
	HANDLE hToken;
	TOKEN_PRIVILEGES tkp;
	OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken);
	LookupPrivilegeValue(NULL, SE_SHUTDOWN_NAME, &tkp.Privileges[0].Luid);
	tkp.PrivilegeCount = 1;
	tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
	AdjustTokenPrivileges(hToken, FALSE, &tkp, 0, (PTOKEN_PRIVILEGES)NULL, 0);
	ExitWindowsEx(EWX_REBOOT, EWX_FORCE);
	#endif
	#ifdef Q_OS_UNIX
	callexternapp("init", "6 &");
	#endif
}

void unetbootin::callexternapp(QString execFile, QString execParm)
{
	#ifdef Q_OS_WIN32
	SHELLEXECUTEINFO ShExecInfo = {0};
	ShExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);
	ShExecInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
	ShExecInfo.hwnd = NULL;
	if (QSysInfo::WindowsVersion == QSysInfo::WV_NT || QSysInfo::WindowsVersion == QSysInfo::WV_2000 || QSysInfo::WindowsVersion == QSysInfo::WV_XP || QSysInfo::WindowsVersion == QSysInfo::WV_2003 )
	{
		ShExecInfo.lpVerb = NULL;
	}
	else
	{
		ShExecInfo.lpVerb = L"runas";
	}
	ShExecInfo.lpFile = LPWSTR(execFile.utf16());
	ShExecInfo.lpParameters = LPWSTR(execParm.utf16());
	ShExecInfo.lpDirectory = NULL;
	ShExecInfo.nShow = SW_HIDE;
	ShExecInfo.hInstApp = NULL;
	ShellExecuteEx(&ShExecInfo);
	WaitForSingleObject(ShExecInfo.hProcess,INFINITE);
	#endif
	#ifdef Q_OS_UNIX
	QProcess lnexternapp;
	lnexternapp.start(QString("%1 %2").arg(execFile).arg(execParm));
	lnexternapp.waitForFinished(-1);
	#endif
}

#ifdef Q_OS_WIN32

void unetbootin::configsysEdit()
{
	SetFileAttributesA(QDir::toNativeSeparators(QString("%1config.sys").arg(targetDrive)).toLocal8Bit(), FILE_ATTRIBUTE_NORMAL);
	QFile::copy(QDir::toNativeSeparators(QString("%1config.sys").arg(targetDrive)), QString("%1config.sys").arg(targetPath));
	QFile::copy(QDir::toNativeSeparators(QString("%1config.sys").arg(targetDrive)), QString("%1confignw.txt").arg(targetPath));
	QFile confignwFile(QString("%1confignw.txt").arg(targetPath));
	QFile configsysFile(QDir::toNativeSeparators(QString("%1config.sys").arg(targetDrive)));
	confignwFile.open(QIODevice::ReadWrite | QIODevice::Text);
	configsysFile.open(QIODevice::ReadOnly | QIODevice::Text);
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
	if (!QFile::copy(QString("%1confignw.txt").arg(targetPath), QDir::toNativeSeparators(QString("%1config.sys").arg(targetDrive))))
	{
		configsysFile.remove();
		QFile::copy(QString("%1confignw.txt").arg(targetPath), QDir::toNativeSeparators(QString("%1config.sys").arg(targetDrive)));
	}
}

void unetbootin::bootiniEdit()
{
	SetFileAttributesW(LPWSTR(QDir::toNativeSeparators(QString("%1boot.ini").arg(targetDrive)).utf16()), FILE_ATTRIBUTE_NORMAL);
	QFile::copy(QDir::toNativeSeparators(QString("%1boot.ini").arg(targetDrive)), QString("%1boot.ini").arg(targetPath));
	QFile::copy(QDir::toNativeSeparators(QString("%1bootnw.ini").arg(targetDrive)), QString("%1bootnw.txt").arg(targetPath));
	QFile bootnwFile(QString("%1bootnw.txt").arg(targetPath));
	QFile bootiniFile(QDir::toNativeSeparators(QString("%1boot.ini").arg(targetDrive)));
	bootnwFile.open(QIODevice::ReadWrite | QIODevice::Text);
	bootiniFile.open(QIODevice::ReadOnly | QIODevice::Text);
	QTextStream bootnwOut(&bootnwFile);
	QTextStream bootiniOut(&bootiniFile);
	QStringList bootiniCurTextL;
	bool btihasreplacedtimeout = false;
	while (!bootiniOut.atEnd())
	{
		QString bootiniCurLine = bootiniOut.readLine();
		if (!btihasreplacedtimeout)
		{
			if (bootiniCurLine.contains("timeout", Qt::CaseInsensitive))
			{
				bootiniCurTextL.append("timeout=15");
				btihasreplacedtimeout = true;
			}
			else
			{
				bootiniCurTextL.append(bootiniCurLine);
			}
		}
		else
		{
			bootiniCurTextL.append(bootiniCurLine);
		}
	}
	QString bootiniCurText = bootiniCurTextL.join("\n");
	QString bootiniText = QString("%1\n%2=\"UNetbootin\"").arg(bootiniCurText).arg(QDir::toNativeSeparators(QString("%1ubnldr.mbr").arg(targetDrive)));
	bootnwOut << bootiniText << endl;
	if (!QFile::copy(QString("%1bootnw.txt").arg(targetPath), QDir::toNativeSeparators(QString("%1boot.ini").arg(targetDrive))))
	{
		bootiniFile.remove();
		QFile::copy(QString("%1bootnw.txt").arg(targetPath), QDir::toNativeSeparators(QString("%1boot.ini").arg(targetDrive)));
	}
}

void unetbootin::vistabcdEdit()
{
	instIndvfl(QString("%1emtxfile.exe").arg(targetPath), emtxfileexe);
	QFile vbcdEditF1(QString("%1vbcdedit.bat").arg(targetPath));
	vbcdEditF1.open(QIODevice::ReadWrite | QIODevice::Text);
	QTextStream vbcdEditS1(&vbcdEditF1);
	vbcdEditS1 << QString("bcdedit /create /d \"UNetbootin\" /application bootsector > %1tmpbcdid").arg(targetPath) << endl;
	vbcdEditF1.close();
	bool warch64;
	callexternapp(QString("%1vbcdedit.bat").arg(targetPath), "");
	QFile vbcdTmpInF(QString("%1tmpbcdid").arg(targetPath));
	vbcdTmpInF.open(QIODevice::ReadOnly | QIODevice::Text);
	QTextStream vbcdTmpInS(&vbcdTmpInF);
	QString qstmpvbcdin = vbcdTmpInS.readAll();
	vbcdTmpInF.close();
	QString vbcdIdTL;
	if (qstmpvbcdin.contains("{") && qstmpvbcdin.contains("}") && qstmpvbcdin.contains("-"))
	{
		warch64 = false;
		vbcdIdTL = qstmpvbcdin.replace("{", "\n").replace("}", "\n").split("\n").filter("-")[0];
	}
	else
	{
		warch64 = true;
		callexternapp(QString("%1emtxfile.exe").arg(targetPath), QString("%1vbcdedit.bat runas").arg(targetPath));
		vbcdTmpInF.open(QIODevice::ReadOnly | QIODevice::Text);
		QTextStream vbcdTmpInS2(&vbcdTmpInF);
		vbcdIdTL = vbcdTmpInS2.readAll().replace("{", "\n").replace("}", "\n").split("\n").filter("-")[0];
		vbcdTmpInF.close();
	}
	QSettings vdtistor("HKEY_LOCAL_MACHINE\\Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\UNetbootin", QSettings::NativeFormat);
	vdtistor.setValue("WArch64", warch64);
	QFile vbcdEditF2(QString("%1vbcdedt2.bat").arg(targetPath));
	vbcdEditF2.open(QIODevice::ReadWrite | QIODevice::Text);
	QTextStream vbcdEditS2(&vbcdEditF2);
	vbcdEditS2 << QString("bcdedit /set {%1} device boot\n"
	"bcdedit /set {%1} path \\ubnldr.mbr\n"
	"bcdedit /displayorder {%1} /addlast\n"
	"bcdedit /timeout 30").arg(vbcdIdTL) << endl;
	vbcdEditF2.close();
	if (warch64)
	{
		callexternapp(QString("%1emtxfile.exe").arg(targetPath), QString("%1vbcdedt2.bat runas").arg(targetPath));
	}
	else
	{
		callexternapp(QString("%1vbcdedt2.bat").arg(targetPath), "");
	}
	QFile vbcdundoF(QString("%1vbcdundo.bat").arg(targetPath));
	vbcdundoF.open(QIODevice::ReadWrite | QIODevice::Text);
	QTextStream vbcdundoS(&vbcdundoF);
	vbcdundoS << QString("bcdedit /delete {%1}").arg(vbcdIdTL) << endl;
	vbcdundoF.close();
}

#endif

void unetbootin::instIndvfl(QString dstfName, QByteArray qbav)
{
	QFile dstFile;
	dstFile.setFileName(dstfName);
	dstFile.open(QIODevice::ReadWrite);
	dstFile.write(qbav);
	dstFile.close();
}

void unetbootin::runinst()
{
	installType = typeselect->currentText();
    targetDrive = driveselect->currentText();
	#ifdef Q_OS_WIN32
	if (installType == "Hard Disk")
	{
		installDir = QDir::toNativeSeparators("unetbtin/");
	}
	if (installType == "USB Drive")
	{
		// TODO change to drive mountpoint
		installDir = "";
	}
	#endif
	#ifdef Q_OS_UNIX
	if (installType == "Hard Disk")
	{
		installDir = QDir::toNativeSeparators("boot/");
	}
	if (installType == "USB Drive")
	{
		// TODO change to drive mountpoint
		installDir = "";
	}
	#endif
	#ifdef Q_OS_WIN32
	targetDev = QString("%1").arg(targetDrive).remove("\\");
	#endif
	#ifdef Q_OS_UNIX
	targetDev = "/dev/sda1"; // TODO USB device detection
	#endif
	kernelLine = "kernel";
	kernelLoc = QString("/%1ubnkern").arg(installDir);
	initrdLine = "initrd";
	initrdLoc = QString("/%1ubninit").arg(installDir);
    targetPath = QDir::toNativeSeparators(QString("%1%2").arg(targetDrive).arg(installDir));
    QDir dir;
    if (!dir.exists(targetPath))
    {
	    dir.mkpath(targetPath);
   	}
    close();
    if (radioFloppy->isChecked())
    {
    	instIndvfl(QString("%1ubnkern").arg(targetPath), memdisk);
    	QFile::copy(FloppyPath->text(), QString("%1ubninit").arg(targetPath));
    	instDetType();
    }
    if (radioManual->isChecked())
    {
    	QFile::copy(KernelPath->text(), QString("%1ubnkern").arg(targetPath));
    	QFile::copy(InitrdPath->text(), QString("%1ubninit").arg(targetPath));
		kernelOpts = OptionEnter->text();
		instDetType();
    }
    if (radioDistro->isChecked())
    {
        nameDistro = distroselect->currentText();
		#include "distrolst.cpp"
		instDetType();
    }
}

void unetbootin::instDetType()
{
    if (installType == "Hard Disk")
    {
    	runinsthdd();
   	}
   	if (installType == "USB Drive")
	{
		runinstusb();
	}
}

void unetbootin::runinsthdd()
{
	#ifdef Q_OS_WIN32
	if (QFile::exists(QDir::toNativeSeparators(QString("%1unetbtin.exe").arg(targetDrive))))
	{
		QFile::remove(QDir::toNativeSeparators(QString("%1unetbtin.exe").arg(targetDrive)));
	}
	QFile::copy(appLoc, QDir::toNativeSeparators(QString("%1unetbtin.exe").arg(targetDrive)));
	QFile::setPermissions(QDir::toNativeSeparators(QString("%1unetbtin.exe").arg(targetDrive)), QFile::WriteOther);
	instIndvfl(QString("%1ubnldr").arg(targetDrive), ubnldr);
	instIndvfl(QString("%1ubnldr.mbr").arg(targetDrive), ubnldrmbr);
    instIndvfl(QString("%1ubnldr.exe").arg(targetDrive), ubnldrexe);
    #endif
   	QFile menulst(QString("%1menu.lst").arg(targetPath));
   	menulst.open(QIODevice::WriteOnly | QIODevice::Text);
	QTextStream menulstout(&menulst);
	QString menulstxt = QString("default 0\n"
	"timeout 3\n"
	"title UNetbootin\n"
	"find --set-root /%8ubnkern\n"
	"%1 %2 %3 %4\n"
	"%5 %6 %7\n"
	"boot").arg(kernelLine, kernelParam, kernelLoc, kernelOpts, initrdLine, initrdLoc, initrdOpts, installDir);
	menulstout << menulstxt << endl;
	menulst.close();
   	QSettings install("HKEY_LOCAL_MACHINE\\Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\UNetbootin", QSettings::NativeFormat);
   	install.setValue("Location", targetDrive);
   	install.setValue("DisplayName", "UNetbootin");
   	install.setValue("UninstallString", QDir::toNativeSeparators(QString("%1unetbtin.exe").arg(targetDrive)));
   	QSettings runonce("HKEY_LOCAL_MACHINE\\Software\\Microsoft\\Windows\\CurrentVersion\\RunOnce", QSettings::NativeFormat);
   	runonce.setValue("UNetbootin Uninstaller", QDir::toNativeSeparators(QString("%1unetbtin.exe").arg(targetDrive)));
	#ifdef Q_OS_WIN32
	if (QSysInfo::WindowsVersion == QSysInfo::WV_32s || QSysInfo::WindowsVersion == QSysInfo::WV_95 || QSysInfo::WindowsVersion == QSysInfo::WV_98 || QSysInfo::WindowsVersion == QSysInfo::WV_Me)
	{
		configsysEdit();
	}
	else if (QSysInfo::WindowsVersion == QSysInfo::WV_NT || QSysInfo::WindowsVersion == QSysInfo::WV_2000 || QSysInfo::WindowsVersion == QSysInfo::WV_XP || QSysInfo::WindowsVersion == QSysInfo::WV_2003 )
	{
		bootiniEdit();
	}
	else if (QSysInfo::WindowsVersion == QSysInfo::WV_VISTA)
	{
		vistabcdEdit();
	}
	else
	{
		configsysEdit();
		bootiniEdit();
		vistabcdEdit();
	}
	#endif
	QMessageBox rebootmsgb;
   	rebootmsgb.setWindowTitle(QObject::tr("Reboot Now?"));
	rebootmsgb.setText(QObject::tr("After rebooting, select the UNetbootin menu entry to boot.%1\nReboot now?").arg(postinstmsg));
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

void unetbootin::runinstusb()
{
	instIndvfl(QString("%1syslinux.exe").arg(targetPath), syslinuxexe);
	QFile syslinuxcfg(QString("%1syslinux.cfg").arg(targetPath));
   	syslinuxcfg.open(QIODevice::WriteOnly | QIODevice::Text);
	QTextStream syslinuxcfgout(&syslinuxcfg);
	QString syslinuxcfgtxt = QString("default unetbootin\n"
	"label unetbootin\n"
	"	kernel %1\n"
	"	append initrd=%2 %3").arg(kernelLoc, initrdLoc, kernelOpts);
	syslinuxcfgout << syslinuxcfgtxt << endl;
	syslinuxcfg.close();
	callexternapp(QString("%1syslinux").arg(targetPath), QString("-ma %1").arg(targetDev));
	QMessageBox usbfinstmsgb;
   	usbfinstmsgb.setWindowTitle(QObject::tr("Reboot Now?"));
	usbfinstmsgb.setText(QObject::tr("After rebooting, select the USB boot option in the BIOS boot menu.%1\nReboot now?").arg(postinstmsg));
	usbfinstmsgb.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
	switch (usbfinstmsgb.exec())
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
