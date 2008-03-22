#include "unetbootin.h"

#include "memdisk.cpp"
#ifdef Q_OS_WIN32
#include "ubnldr.cpp"
#include "ubnldrmbr.cpp"
#include "ubnldrexe.cpp"
#include "emtxfileexe.cpp"
#include "syslinuxexe.cpp"
#endif

unetbootin::unetbootin(QWidget *parent)
    : QWidget(parent)
{
    setupUi(this);
    driveselect->addItem(QDir::toNativeSeparators(QDir::rootPath()).toUpper());
	#ifdef Q_OS_UNIX
	fdiskcommand = locatecommand("fdisk", "either", "util-linux");
	sfdiskcommand = locatecommand("sfdisk", "either", "util-linux");
	mssyscommand = locatecommand("ms-sys", "USB Drive", "ms-sys");
	syslinuxcommand = locatecommand("syslinux", "USB Drive", "syslinux");
	#endif
}

void unetbootin::on_typeselect_currentIndexChanged(int typeselectIndex)
{
	if (typeselectIndex == typeselect->findText("Hard Disk"))
	{
		driveselect->clear();
		driveselect->addItem(QDir::toNativeSeparators(QDir::rootPath()).toUpper());
	}
	if (typeselectIndex == typeselect->findText("USB Drive"))
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
		QProcess fdisklusbdevs;
		fdisklusbdevs.start(QString("%1 -l").arg(fdiskcommand));
		fdisklusbdevs.waitForFinished(-1);
		QStringList usbdevsL = QString(fdisklusbdevs.readAll()).split(" ").join("\n").split("\t").join("\n").split("\n").filter("/dev/");
		for (int i = 0; i < usbdevsL.size(); ++i)
		{
			if (!usbdevsL.at(i).contains(":"))
			{
				driveselect->addItem(usbdevsL.at(i));
			}
		}
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
	if (QFile::exists(targetfile))
	{
		QFile::remove(targetfile);
	}
	sourcefile = fileurl;
	destinfile = targetfile;
	QHttp dlhttp;
	QUrl dlurl(fileurl);
	dlprogress.setWindowTitle(QObject::tr("Downloading..."));
	dlprogress.setLabelText(QObject::tr("Downloading files, please wait...\nSource: %1\nDestination: %2\nDownloaded: 0 bytes").arg(fileurl).arg(targetfile));
	connect(&dlhttp, SIGNAL(done(bool)), &dlprogress, SLOT(close()));
	connect(&dlhttp, SIGNAL(dataReadProgress(int, int)), this, SLOT(dlprogressupdate(int, int)));
	QFile dloutfile;
	if (installType == "USB Drive")
	{
		if (QFile::exists(QDir::toNativeSeparators(QString("%1/ubndlout.tmp").arg(QDir::tempPath()))))
		{
			QFile::remove(QDir::toNativeSeparators(QString("%1/ubndlout.tmp").arg(QDir::tempPath())));
		}
		dloutfile.setFileName(QDir::toNativeSeparators(QString("%1/ubndlout.tmp").arg(QDir::tempPath())));
	}
	else
	{
		dloutfile.setFileName(targetfile);
	}
	dloutfile.open(QIODevice::WriteOnly);
	dlhttp.setHost(dlurl.host());
	dlhttp.get(dlurl.path(), &dloutfile);
	dlprogress.exec();
	dlhttp.close();
	dloutfile.close();
	if (installType == "USB Drive")
	{
		QFile::copy(QDir::toNativeSeparators(QString("%1/ubndlout.tmp").arg(QDir::tempPath())), targetfile);
		QFile::remove(QDir::toNativeSeparators(QString("%1/ubndlout.tmp").arg(QDir::tempPath())));
	}
}

QString unetbootin::downloadpagecontents(QString pageurl)
{
	QUrl pgurl(pageurl);
	QHttp pghttp;
	QEventLoop pgwait;
	connect(&pghttp, SIGNAL(done(bool)), &pgwait, SLOT(quit()));
	pghttp.setHost(pgurl.host());
	pghttp.get(pgurl.path());
	pgwait.exec();
	return QString(pghttp.readAll());
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

#ifdef Q_OS_UNIX

QString unetbootin::locatecommand(QString commandtolocate, QString reqforinstallmode, QString packagename)
{
	QProcess whereiscommand;
	whereiscommand.start(QString("whereis %1").arg(commandtolocate));
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
	QMessageBox errorcmdnotfoundmsgbx;
	errorcmdnotfoundmsgbx.setIcon(QMessageBox::Warning);
	errorcmdnotfoundmsgbx.setWindowTitle(QString(QObject::tr("%1 not found")).arg(commandtolocate));
	errorcmdnotfoundmsgbx.setText(QString(QObject::tr("%1 not found. This is required for %2 install mode.\nInstall the \"%3\" package or your distribution's equivalent.")).arg(commandtolocate, reqforinstallmode, packagename));
	errorcmdnotfoundmsgbx.setStandardButtons(QMessageBox::Ok);
	switch (errorcmdnotfoundmsgbx.exec())
	{
		case QMessageBox::Ok:
			break;
		default:
			break;
	}
 	return "ERROR";
}

QString unetbootin::locatedevicenode(QString mountpoint)
{
	QFile procmountsF("/proc/mounts");
	procmountsF.open(QIODevice::ReadOnly | QIODevice::Text);
	QTextStream procmountsS(&procmountsF);
	QStringList rawdeviceL = procmountsS.readAll().replace("\t", " ").split("\n").filter("/dev/").filter(QString(" %1 ").arg(mountpoint));
	if (rawdeviceL.isEmpty())
	{
		return "NOT FOUND";
	}
	else
	{
		QFileInfo rawdeviceFI(rawdeviceL.at(0).split(" ").at(0));
		return rawdeviceFI.canonicalFilePath();
	}
}

QString unetbootin::locatemountpoint(QString devicenode)
{
	QFile procmountsF("/proc/mounts");
	procmountsF.open(QIODevice::ReadOnly | QIODevice::Text);
	QTextStream procmountsS(&procmountsF);
	QStringList procmountsL;
	while (true)
	{
		procmountsL = procmountsS.readAll().split("\n").filter(devicenode);
		if (!procmountsL.isEmpty())
			break;
		QMessageBox errordevnotmountedmsgbx;
		errordevnotmountedmsgbx.setIcon(QMessageBox::Warning);
		errordevnotmountedmsgbx.setWindowTitle(QString(QObject::tr("%1 not mounted")).arg(devicenode));
		errordevnotmountedmsgbx.setText(QString(QObject::tr("%1 is not mounted. Mount it and press the \"OK\" button to proceed with installation, or press the \"Cancel\" button to abort.")).arg(devicenode));
		errordevnotmountedmsgbx.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
		switch (errordevnotmountedmsgbx.exec())
		{
			case QMessageBox::Cancel:
			{
				return "INSTALL ABORTED";
			}
		}
	}
	return procmountsL.at(0).split("\t").join(" ").split(" ").at(1);
}

QString unetbootin::getGrubNotation(QString devicenode)
{
	return QString("(hd%1,%2)").arg(getDiskNumber(devicenode)).arg(getPartitionNumber(devicenode));
}

int unetbootin::letterToNumber(QChar lettertoconvert)
{
	if (lettertoconvert.isLower())
	{
		return static_cast<int>(lettertoconvert.toAscii() - 'a');
	}
	if (lettertoconvert.isUpper())
	{
		return static_cast<int>(lettertoconvert.toAscii() - 'A');
	}
	else
	{
		return 999;
	}
}

int unetbootin::getDiskNumber(QString devicenode)
{
	QChar disknumchar(devicenode.at(devicenode.size() - 2));
	if (disknumchar.isLetter())
	{
		return letterToNumber(disknumchar);
	}
	else
	{
		return disknumchar.digitValue() - 1;
	}
}

int unetbootin::getPartitionNumber(QString devicenode)
{
	QChar partitionchar(devicenode.at(devicenode.size() - 1));
	if (partitionchar.isLetter())
	{
		return letterToNumber(partitionchar);
	}
	else
	{
		return partitionchar.digitValue() - 1;
	}
}

#endif

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
	bool btimustreplacetimeout = true;
	QRegExp btichkistimeout("\\s{0,}timeout.{0,}", Qt::CaseInsensitive);
	while (!bootiniOut.atEnd())
	{
		QString bootiniCurLine = bootiniOut.readLine();
		if (btimustreplacetimeout && btichkistimeout.exactMatch(bootiniCurLine))
		{
			bootiniCurTextL.append("timeout=15");
			btimustreplacetimeout = false;
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
	dstFile.open(QIODevice::WriteOnly);
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
		installDir = "unetbtin/";
	}
	if (installType == "USB Drive")
	{
		installDir = "";
	}
	targetDev = QString("%1").arg(targetDrive).remove("\\");
	#endif
	#ifdef Q_OS_UNIX
	if (installType == "Hard Disk")
	{
		QString devnboot = locatedevicenode("/boot");
		if (devnboot == "NOT FOUND")
		{
			installDir = "boot/";
			targetDev = locatedevicenode("/");
		}
		else
		{
			installDir = "";
			targetDev = devnboot;
		}
	}
	if (installType == "USB Drive")
	{
		targetDev = driveselect->currentText();
		installDir = "";
		targetDrive = QString("%1/").arg(locatemountpoint(targetDev));
		if (targetDrive == "INSTALL ABORTED/")
		{
			this->close();
			return;
		}
	}
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
   	if (QFile::exists(QString("%1ubnkern").arg(targetPath)))
   	{
   		QFile::remove(QString("%1ubnkern").arg(targetPath));
  	}
  	if (QFile::exists(QString("%1ubninit").arg(targetPath)))
   	{
   		QFile::remove(QString("%1ubninit").arg(targetPath));
  	}
	hide();
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
	if (QFile::exists(QDir::toNativeSeparators(QString("%1ubnldr").arg(targetDrive))))
	{
		QFile::remove(QDir::toNativeSeparators(QString("%1ubnldr").arg(targetDrive)));
	}
	instIndvfl(QString("%1ubnldr").arg(targetDrive), ubnldr);
	if (QFile::exists(QDir::toNativeSeparators(QString("%1ubnldr.mbr").arg(targetDrive))))
	{
		QFile::remove(QDir::toNativeSeparators(QString("%1ubnldr.mbr").arg(targetDrive)));
	}
	instIndvfl(QString("%1ubnldr.mbr").arg(targetDrive), ubnldrmbr);
	if (QFile::exists(QDir::toNativeSeparators(QString("%1ubnldr.exe").arg(targetDrive))))
	{
		QFile::remove(QDir::toNativeSeparators(QString("%1ubnldr.exe").arg(targetDrive)));
	}
	instIndvfl(QString("%1ubnldr.exe").arg(targetDrive), ubnldrexe);
	#endif
   	QFile menulst;
   	#ifdef Q_OS_WIN32
   	menulst.setFileName(QString("%1menu.lst").arg(targetPath));
   	#endif
   	#ifdef Q_OS_UNIX
   	menulst.setFileName("/boot/grub/menu.lst");
   	if (QFile::exists(QString("%1.bak").arg(menulst.fileName())))
   		QFile::remove(QString("%1.bak").arg(menulst.fileName()));
   	QFile::copy(menulst.fileName(), QString("%1.bak").arg(menulst.fileName()));
   	QFile bkmenulst(QString("%1.bak").arg(menulst.fileName()));
   	bkmenulst.open(QIODevice::ReadOnly | QIODevice::Text);
   	QTextStream bkmenulstout(&bkmenulst);
   	#endif
	menulst.open(QIODevice::WriteOnly | QIODevice::Text);
	QTextStream menulstout(&menulst);
	#ifdef Q_OS_UNIX
	QRegExp mlstchkistimeout("\\s{0,}timeout\\s{1,}\\d{1,}.{0,}", Qt::CaseInsensitive);
	QRegExp mlstchkishiddenmenu("\\s{0,}hiddenmenu.{0,}", Qt::CaseInsensitive);
	QStringList ecurmenulstTextL;
	bool mlstmustreplacetimeout = true;
	bool mlstmustreplacehiddenmenu = true;
	while (!bkmenulstout.atEnd())
	{
		QString menulstCurLine = bkmenulstout.readLine();
		if (mlstmustreplacetimeout && mlstchkishiddenmenu.exactMatch(menulstCurLine))
		{
			ecurmenulstTextL.append("#hiddenmenu");
			mlstmustreplacetimeout = false;
		}
		else if (mlstmustreplacehiddenmenu && mlstchkistimeout.exactMatch(menulstCurLine))
		{
			ecurmenulstTextL.append("timeout\t\t15");
			mlstmustreplacehiddenmenu = false;
		}
		else
		{
			ecurmenulstTextL.append(menulstCurLine);
		}
	}
	QString ecurmenulstText = ecurmenulstTextL.join("\n");
    #endif
	QString menulstxt = QString(
	#ifdef Q_OS_UNIX
	"%9\n\n"
	#endif
	#ifdef Q_OS_WIN32
	"default 0\n"
	"timeout 3"
	#endif
	"title UNetbootin\n"
	#ifdef Q_OS_WIN32
	"find --set-root %3\n"
	#endif
	#ifdef Q_OS_UNIX
	"root %8\n"
	#endif
	"%1 %2 %3 %4\n"
	"%5 %6 %7\n"
	"boot").arg(kernelLine, kernelParam, kernelLoc, kernelOpts, initrdLine, initrdLoc, initrdOpts
	#ifdef Q_OS_UNIX
	, getGrubNotation(targetDev), ecurmenulstText
	#endif
	);
	menulstout << menulstxt << endl;
	menulst.close();
	#ifdef Q_OS_WIN32
   	QSettings install("HKEY_LOCAL_MACHINE\\Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\UNetbootin", QSettings::NativeFormat);
   	install.setValue("Location", targetDrive);
   	install.setValue("DisplayName", "UNetbootin");
   	install.setValue("UninstallString", QDir::toNativeSeparators(QString("%1unetbtin.exe").arg(targetDrive)));
   	QSettings runonce("HKEY_LOCAL_MACHINE\\Software\\Microsoft\\Windows\\CurrentVersion\\RunOnce", QSettings::NativeFormat);
   	runonce.setValue("UNetbootin Uninstaller", QDir::toNativeSeparators(QString("%1unetbtin.exe").arg(targetDrive)));
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
	#ifdef Q_OS_UNIX
	QSettings install(QSettings::SystemScope, "UNetbootin");
	install.setValue("Location", "/");
	#endif
	QMessageBox rebootmsgb;
	rebootmsgb.setIcon(QMessageBox::Information);
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
	#ifdef Q_OS_WIN32
	if (QFile::exists(QDir::toNativeSeparators(QString("%1/syslinux.exe").arg(QDir::tempPath()))))
	{
		QFile::remove(QDir::toNativeSeparators(QString("%1/syslinux.exe").arg(QDir::tempPath())));
	}
	instIndvfl(QDir::toNativeSeparators(QString("%1/syslinux.exe").arg(QDir::tempPath())), syslinuxexe);
	callexternapp(QDir::toNativeSeparators(QString("%1/syslinux.exe").arg(QDir::tempPath())), QString("-ma %1").arg(targetDev));
	QFile::remove(QDir::toNativeSeparators(QString("%1/syslinux.exe").arg(QDir::tempPath())));
	#endif
	#ifdef Q_OS_UNIX
	callexternapp(syslinuxcommand, targetDev);
	callexternapp(mssyscommand, QString("-s %1").arg(QString(targetDev).remove(-1, 1)));
	callexternapp(sfdiskcommand, QString("%1 -A%2").arg(QString(targetDev).remove(-1, 1), QString(targetDev).at(targetDev.size() - 1)));
	#endif
	if (QFile::exists(QString("%1syslinux.cfg").arg(targetPath)))
	{
		QFile::remove(QString("%1syslinux.cfg").arg(targetPath));
	}
	QFile syslinuxcfg(QString("%1syslinux.cfg").arg(targetPath));
   	syslinuxcfg.open(QIODevice::WriteOnly | QIODevice::Text);
	QTextStream syslinuxcfgout(&syslinuxcfg);
	QString syslinuxcfgtxt = QString("default unetbootin\n"
	"label unetbootin\n"
	"\tkernel %1\n"
	"\tappend initrd=%2 %3").arg(kernelLoc, initrdLoc, kernelOpts);
	syslinuxcfgout << syslinuxcfgtxt << endl;
	syslinuxcfg.close();
	QMessageBox usbfinstmsgb;
	usbfinstmsgb.setIcon(QMessageBox::Information);
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
