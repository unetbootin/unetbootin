#include "unetbootin.h"

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
/*
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
    callexternapp(QString("%1downlder.exe").arg(targetPath), "");
*/
//	QProcess dlprocess;
//	dlprocess.start(QString("%1downlder.exe").arg(targetPath));
//	dlprocess.waitForFinished(-1);
/*
	QProgressDialog dlprogress(this);
	dlprogress.setCancelButtonText("Cancel");
	dlprogress.setRange(0, 97979);
	dlprogress.setWindowTitle("Downloading...");
*/
/*
	dloutdest = new QFile(targetfile);
	dloutdest->open(QFile::ReadWrite);
	QUrl dlurl(fileurl);
	QFileInfo dlfileinf(dlurl.path());
	QString dlfilename(dlfileinf.fileName());
//	http = new QHttp(this);
//	connect();
	connect(http, SIGNAL(done(bool)), this, SLOT(instDetType()));
	http->setHost(dlurl.host());
	http->get(fileurl, dloutdest);
//	QHttp http;
//	http.setHost(dlurl.host());
//	http.get(dlurl.path(), &dloutdest);
//	http.get(fileurl, &dloutdest);
	dloutdest->close();
*/
/*
	QUrl url(fileurl);
	QFile outfile(targetfile);
	outfile.open(QIODevice::ReadWrite);
	QHttp http;
	QHttp::ConnectionMode mode = url.scheme().toLower() == "https" ? QHttp::ConnectionModeHttps : QHttp::ConnectionModeHttp;
	http.setHost(url.host(), mode, url.port() == -1 ? 0 : url.port());
	http.get(url.path(), &outfile);
*/
/*
	QHttp *http = new QHttp();
	QUrl *url=new QUrl(fileurl);
	QFileInfo fileinfo(url->path());
	QFile *file = new QFile(fileinfo.fileName());
	file->open(QIODevice::WriteOnly);
	http->setHost(url->host(),80);
	http->get(url->path(),file);
*/
//	Sleep(5000);
//	QByteArray dlcontent = http.readAll();
//	outfile.write(dlcontent);
//	connect(&http, SIGNAL(requestFinished(int,bool)), this, SLOT(httpDone(int,bool)));
//	outfile.close();
	sourcefile = fileurl;
	destinfile = targetfile;
	QHttp dlhttp;
	QUrl dlurl(fileurl);
	dlprogress.setWindowTitle(QObject::tr("Downloading..."));
	dlprogress.setLabelText(QObject::tr("Downloading files, please wait...\nSource: %1\nDestination: %2").arg(fileurl).arg(targetfile));
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
	dlprogress.setLabelText(QObject::tr("Downloading files, please wait...\nSource: %1\nDestination: %2\nCopied: %3 of %4 bytes").arg(sourcefile).arg(destinfile).arg(dlbytes).arg(maxbytes));
}

#ifdef Q_OS_WIN32

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

void unetbootin::callexternapp(QString execFile, QString execParm)
{
//	const char* execParmL = execParm.constData();
//	const char* execFileL = execFile.constData();
//	const char* execVerbL = execVerb.constData();
//	LPCTSTR execFileL, execParamsL, execVerbL;
//	LPWSTR execFileL, execParmL, execVerbL;
//	wchar_t* execFileL = LPWSTR(execFile.utf16());
//	wchar_t* execParmL = {};
//	wchar_t* execVerbL = {};
//	execFile.toWCharArray(execFileL);
//	execParm.toWCharArray(execParmL);
//	execVerb.toWCharArray(execVerbL);
	SHELLEXECUTEINFO ShExecInfo = {0};
	ShExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);
	ShExecInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
	ShExecInfo.hwnd = NULL;
//	ShExecInfo.lpVerb = LPCTSTR(execVerbL);
	ShExecInfo.lpVerb = L"runas";
//	ShExecInfo.lpFile = LPCTSTR(execFile.toAscii().constData());
//	ShExecInfo.lpFile = LPCTSTR(execFileL);
//	ShExecInfo.lpFile = L"notepad";
	ShExecInfo.lpFile = LPWSTR(execFile.utf16());
//	ShExecInfo.lpParameters = LPCTSTR(execParmL);
	ShExecInfo.lpParameters = LPWSTR(execParm.utf16());
	ShExecInfo.lpDirectory = NULL;
	ShExecInfo.nShow = SW_HIDE;
//	ShExecInfo.nShow = SW_SHOW;
	ShExecInfo.hInstApp = NULL;
	ShellExecuteEx(&ShExecInfo);
	WaitForSingleObject(ShExecInfo.hProcess,INFINITE);
}

/*
bool unetbootin::isWinArch64Bit()
{
	SYSTEM_INFO sysinfo;
	GetSystemInfo(&sysinfo);
	if (sysinfo.dwProcessorType = PROCESSOR_ARCHITECTURE_AMD64)
	{
		return true;
	}
	else
	{
		return false;
	}
}
*/

void unetbootin::configsysEdit()
{
//	QFile wldEditF(QString("%1wldedit.bat").arg(targetPath));
//	wldEditF.open(QIODevice::ReadWrite | QIODevice::Text);
//	QTextStream wldEditS(&wldEditF);
//	wldEditS << QDir::toNativeSeparators(QString("attrib -h -s -r %1/config.sys").arg(targetDrive)) << endl;
//	wldEditF.close();
//	callexternapp(QString("%1wldedit.bat").arg(targetPath), "", "");
//	CString str(QString("%1wldedit.bat").arg(targetPath).toLocal8Bit());
	SetFileAttributesA(QDir::toNativeSeparators(QString("%1/config.sys").arg(targetDrive)).toLocal8Bit(), FILE_ATTRIBUTE_NORMAL);
//	callexternapp(QString("%1wldedit.bat").arg(targetPath).toLocal8Bit(), "", "");
//	callexternapp("notepad", "", "");
//	QProcess cfgsattrib;
//	cfgsattrib.start(QString("%1runxfile.exe %1wldedit.bat").arg(targetPath));
//	cfgsattrib.start(QDir::toNativeSeparators(QString("attrib -h -s -r %1/config.sys").arg(targetDrive)));
//	cfgsattrib.waitForFinished(-1);
	QFile::copy(QDir::toNativeSeparators(QString("%1/config.sys").arg(targetDrive)), QString("%1config.sys").arg(targetPath));
	QFile::copy(QDir::toNativeSeparators(QString("%1/config.sys").arg(targetDrive)), QString("%1confignw.txt").arg(targetPath));
	QFile confignwFile(QString("%1confignw.txt").arg(targetPath));
	QFile configsysFile(QDir::toNativeSeparators(QString("%1/config.sys").arg(targetDrive)));
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
	if (!QFile::copy(QString("%1confignw.txt").arg(targetPath), QDir::toNativeSeparators(QString("%1/config.sys").arg(targetDrive))))
	{
		configsysFile.remove();
		QFile::copy(QString("%1confignw.txt").arg(targetPath), QDir::toNativeSeparators(QString("%1/config.sys").arg(targetDrive)));
	}
}

void unetbootin::bootiniEdit()
{
	SetFileAttributesW(LPWSTR(QDir::toNativeSeparators(QString("%1/boot.ini").arg(targetDrive)).utf16()), FILE_ATTRIBUTE_NORMAL);
	QFile::copy(QDir::toNativeSeparators(QString("%1/boot.ini").arg(targetDrive)), QString("%1boot.ini").arg(targetPath));
	QFile::copy(QDir::toNativeSeparators(QString("%1/bootnw.ini").arg(targetDrive)), QString("%1bootnw.txt").arg(targetPath));
	QFile bootnwFile(QString("%1bootnw.txt").arg(targetPath));
	QFile bootiniFile(QDir::toNativeSeparators(QString("%1/boot.ini").arg(targetDrive)));
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
//	QStringList bootiniCurTextL = bootiniOut.readAll().split("\n");
//	int bootiniCurTextI = bootiniCurTextL.indexOf("timeout", Qt::CaseInsensitive);
//	bootiniCurTextL[bootiniCurTextI] = QString("timeout=15");
//	QString bootiniCurText = bootiniCurTextL.join("\n");
	QString bootiniText = QString("%1\n%2=\"UNetbootin\"").arg(bootiniCurText).arg(QDir::toNativeSeparators(QString("%1/ubnldr.mbr").arg(targetDrive)));
	bootnwOut << bootiniText << endl;
	if (!QFile::copy(QString("%1bootnw.txt").arg(targetPath), QDir::toNativeSeparators(QString("%1/boot.ini").arg(targetDrive))))
	{
		bootiniFile.remove();
		QFile::copy(QString("%1bootnw.txt").arg(targetPath), QDir::toNativeSeparators(QString("%1/boot.ini").arg(targetDrive)));
	}
}

void unetbootin::vistabcdEdit()
{
//	#undef _WIN32_WINNT
//	#define _WIN32_WINNT 0x0501
//	BOOL isWinArch64;
//	IsWow64Process(GetCurrentProcess(), &isWinArch64);
//	Wow64DisableWow64FsRedirection();
	QFile vbcdEditF1(QString("%1vbcdedit.bat").arg(targetPath));
	vbcdEditF1.open(QIODevice::ReadWrite | QIODevice::Text);
	QTextStream vbcdEditS1(&vbcdEditF1);
//	vbcdEditS1 << QString("%2 /create /d \"UNetbootin\" /application bootsector >> %1tmpbcdid").arg(QDir::toNativeSeparators(targetPath)).arg(QDir::toNativeSeparators(QString("%1/System32/bcdedit.exe").arg(getenv("WINDIR")))) << endl;
	vbcdEditS1 << QString("bcdedit /create /d \"UNetbootin\" /application bootsector > %1tmpbcdid").arg(targetPath) << endl;
	vbcdEditF1.close();
//	callexternapp(getenv("COMSPEC"), QString("/c %1vbcdedit.bat").arg(targetPath));
/*
	if (isWinArch64)
	{
		callexternapp(QString("%1emtxfile.exe").arg(targetPath), QString("%1vbcdedit.bat runas").arg(targetPath));
	}
	else
	{
		callexternapp(QString("%1vbcdedit.bat").arg(targetPath), "");
	}
*/
/*
	callexternapp(QString("%1emtxfile.exe").arg(targetPath), QString("%1vbcdedit.bat runas").arg(targetPath));
	bool arch64 = false;
	if (QFile::exists(QString("%1tmpbcdid").arg(targetPath)))
	{
		arch64 = true;
	}
	else
	{
		callexternapp(QString("%1vbcdedit.bat").arg(targetPath), "");
	}
*/

/*
	SYSTEM_INFO sysinfo;
	GetSystemInfo(&sysinfo);
	if (sysinfo.wProcessorArchitecture = PROCESSOR_ARCHITECTURE_AMD64)
	{
		callexternapp(QString("%1emtxfile.exe").arg(targetPath), QString("%1vbcdedit.bat runas").arg(targetPath));
	}
	else
	{
		callexternapp(QString("%1vbcdedit.bat").arg(targetPath), "");
	}
*/
/*
	if (isWinArch64)
	{
		callexternapp(QString("%1emtxfile.exe").arg(targetPath), QString("%1vbcdedit.bat runas").arg(targetPath));
	}
	else
	{
		callexternapp(QString("%1vbcdedit.bat").arg(targetPath), "");
	}
*/
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
//	vdtistor.setValue("Bcdid", vbcdIdTL);
	QFile vbcdEditF2(QString("%1vbcdedt2.bat").arg(targetPath));
	vbcdEditF2.open(QIODevice::ReadWrite | QIODevice::Text);
	QTextStream vbcdEditS2(&vbcdEditF2);
/*
	vbcdEditS2 << QString("%2 /set {%1} device boot\n"
	"%2 /set {%1} path \\ubnldr.mbr\n"
	"%2 /displayorder {%1} /addlast\n"
	"%2 /timeout 30").arg(vbcdIdTL).arg(QDir::toNativeSeparators(QString("%1/System32/bcdedit.exe").arg(getenv("WINDIR")))) << endl;
*/
	vbcdEditS2 << QString("bcdedit /set {%1} device boot\n"
	"bcdedit /set {%1} path \\ubnldr.mbr\n"
	"bcdedit /displayorder {%1} /addlast\n"
	"bcdedit /timeout 30").arg(vbcdIdTL) << endl;
	vbcdEditF2.close();
//	callexternapp(getenv("COMSPEC"), QString("/c %1vbcdedt2.bat").arg(targetPath));
/*
	if (isWinArch64)
	{
		callexternapp(QString("%1emtxfile.exe").arg(targetPath), QString("%1vbcdedt2.bat runas").arg(targetPath));
	}
	else
	{
		callexternapp(QString("%1vbcdedt2.bat").arg(targetPath), "");
	}
*/
	if (warch64)
	{
		callexternapp(QString("%1emtxfile.exe").arg(targetPath), QString("%1vbcdedt2.bat runas").arg(targetPath));
	}
	else
	{
		callexternapp(QString("%1vbcdedt2.bat").arg(targetPath), "");
	}
/*
	if (arch64)
	{
		callexternapp(QString("%1emtxfile.exe").arg(targetPath), QString("%1vbcdedt2.bat runas").arg(targetPath));
	}
	else
	{
		callexternapp(QString("%1vbcdedt2.bat").arg(targetPath), "");
	}
*/
	QFile vbcdundoF(QString("%1vbcdundo.bat").arg(targetPath));
	vbcdundoF.open(QIODevice::ReadWrite | QIODevice::Text);
	QTextStream vbcdundoS(&vbcdundoF);
//	vbcdundoS << QString("%2 /delete {%1}").arg(vbcdIdTL).arg(QDir::toNativeSeparators(QString("%1/System32/bcdedit.exe").arg(getenv("WINDIR")))) << endl;
	vbcdundoS << QString("bcdedit /delete {%1}").arg(vbcdIdTL) << endl;
	vbcdundoF.close();
/*
	if (sysinfo.wProcessorArchitecture = PROCESSOR_ARCHITECTURE_AMD64)
	{
		callexternapp(QString("%1emtxfile.exe").arg(targetPath), QString("%1vbcdedt2.bat runas").arg(targetPath));
	}
	else
	{
		callexternapp(QString("%1vbcdedt2.bat").arg(targetPath), "");
	}
*/
/*
	IsWow64Process();
	if (IsWow64Process())
	callexternapp(QString("%1emtxfile.exe").arg(targetPath), QString("%1vbcdedit.bat").arg(targetPath));
*/
}

void unetbootin::wInstfiles()
{
	#include "ubnldr.cpp"
	instIndvfl(QString("ubnldr"), ubnldr);
	#include "ubnldrexe.cpp"
	instIndvfl(QString("ubnldr.exe"), ubnldrexe);
	#include "ubnldrmbr.cpp"
	instIndvfl(QString("ubnldr.mbr"), ubnldrmbr);
	#include "memdisk.cpp"
	instIndvfl(QString("memdisk"), memdisk);
	#include "emtxfileexe.cpp"
	instIndvfl(QString("emtxfile.exe"), emtxfileexe);
}

#endif

void unetbootin::instIndvfl(QString dstfName, QByteArray qbav)
{
	QFile dstFile;
	dstFile.setFileName(QString("%1%2").arg(targetPath).arg(dstfName));
	dstFile.open(QIODevice::ReadWrite);
	dstFile.write(qbav);
	dstFile.close();
}

void unetbootin::runinst()
{
	kernelLine = "kernel";
	kernelLoc = "/unetbtin/ubnkern";
	initrdLine = "initrd";
	initrdLoc = "/unetbtin/ubninit";
	installType = typeselect->currentText();
    targetDrive = driveselect->currentText();
    targetPath = QDir::toNativeSeparators(QString("%1/unetbtin/").arg(targetDrive));
	QDir dir;
    dir.mkpath(targetPath);
	wInstfiles();
	if (QFile::exists(QDir::toNativeSeparators(QString("%1/unetbtin.exe").arg(targetDrive))))
	{
		QFile::remove(QDir::toNativeSeparators(QString("%1/unetbtin.exe").arg(targetDrive)));
	}
	QFile::copy(appLoc, QDir::toNativeSeparators(QString("%1/unetbtin.exe").arg(targetDrive)));
	QFile::setPermissions(QDir::toNativeSeparators(QString("%1/unetbtin.exe").arg(targetDrive)), QFile::WriteOther);
	QFile::copy(QString("%1ubnldr.exe").arg(targetPath), QDir::toNativeSeparators(QString("%1/ubnldr.exe").arg(targetDrive)));
    QFile::copy(QString("%1ubnldr").arg(targetPath), QDir::toNativeSeparators(QString("%1/ubnldr").arg(targetDrive)));
    QFile::copy(QString("%1ubnldr.mbr").arg(targetPath), QDir::toNativeSeparators(QString("%1/ubnldr.mbr").arg(targetDrive)));
    close();
    if (radioFloppy->isChecked())
    {
    	QFile::copy(QString("%1memdisk").arg(targetPath), QString("%1ubnkern").arg(targetPath));
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
//	TODO
}
