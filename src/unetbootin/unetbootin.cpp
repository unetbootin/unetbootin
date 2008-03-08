#include <QFile>
#include <QFileDialog>
#include <QTextStream>
#include <QProcess>
#include <QSettings>
#include <QSysInfo>
#include <QMessageBox>
#include "unetbootin.h"
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
//	ShExecInfo.nShow = SW_HIDE;
	ShExecInfo.nShow = SW_SHOW;
	ShExecInfo.hInstApp = NULL;
	ShellExecuteEx(&ShExecInfo);
	WaitForSingleObject(ShExecInfo.hProcess,INFINITE);
}

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
//	TODO
}

void unetbootin::vistabcdEdit()
{
	QFile vbcdEditF1(QString("%1vbcdedit.bat").arg(targetPath));
	vbcdEditF1.open(QIODevice::ReadWrite | QIODevice::Text);
	QTextStream vbcdEditS1(&vbcdEditF1);
	vbcdEditS1 << QDir::toNativeSeparators(QString("bcdedit /create /d \"UNetbootin\" /application bootsector >> %1tmpbcdid").arg(targetPath)) << endl;
	vbcdEditF1.close();
	SYSTEM_INFO sysinfo;
	if (sysinfo.wProcessorArchitecture != PROCESSOR_ARCHITECTURE_AMD64)
	{
		callexternapp(QString("%1vbcdedit.bat").arg(targetPath), "");
	}
	else
	{
		callexternapp(QString("%1emtxfile.exe").arg(targetPath), QString("%1vbcdedit.bat").arg(targetPath));
	}
	QFile vbcdTmpInF(QString("%1tmpbcdid").arg(targetPath));
	vbcdTmpInF.open(QIODevice::ReadOnly | QIODevice::Text);
	QTextStream vbcdTmpInS(&vbcdTmpInF);
	QString vbcdIdTL = vbcdTmpInS.readAll().replace("{", "\n").replace("}", "\n").split("\n").filter("-")[0];
	vbcdTmpInF.close();
	QFile vbcdSIDF(QString("%1bcdid").arg(targetPath));
	vbcdSIDF.open(QIODevice::ReadWrite | QIODevice::Text);
	QTextStream vbcdSIDS(&vbcdSIDF);
	vbcdSIDS << vbcdIdTL;
	vbcdSIDF.close();
	QFile vbcdEditF2(QString("%1vbcdedt2.bat").arg(targetPath));
	vbcdEditF2.open(QIODevice::ReadWrite | QIODevice::Text);
	QTextStream vbcdEditS2(&vbcdEditF2);
	vbcdEditS2 << QDir::toNativeSeparators(QString("bcdedit /set {%1} device boot\n"
	"bcdedit /set {%1} path \\ubnldr.mbr\n"
	"bcdedit /displayorder {%1} /addlast\n"
	"bcdedit /timeout 30").arg(vbcdIdTL)) << endl;
	vbcdEditF2.close();
	if (sysinfo.wProcessorArchitecture != PROCESSOR_ARCHITECTURE_AMD64)
	{
		callexternapp(QString("%1vbcdedt2.bat").arg(targetPath), "");
	}
	else
	{
		callexternapp(QString("%1emtxfile.exe").arg(targetPath), QString("%1vbcdedt2.bat").arg(targetPath));
	}
//	IsWow64Process();
//	if (IsWow64Process())
//	callexternapp(QString("%1emtxfile.exe").arg(targetPath), QString("%1vbcdedit.bat").arg(targetPath));
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
	#include "ubnldr.cpp"
	instIndvfl(QString("ubnldr"), ubnldr);
	#include "ubnldrexe.cpp"
	instIndvfl(QString("ubnldr.exe"), ubnldrexe);
	#include "ubnldrmbr.cpp"
	instIndvfl(QString("ubnldr.mbr"), ubnldrmbr);
	#include "memdisk.cpp"
	instIndvfl(QString("memdisk"), memdisk);
	#include "downlderexe.cpp"
	instIndvfl(QString("downlder.exe"), downlderexe);
	#include "bootederexe.cpp"
	instIndvfl(QString("booteder.exe"), bootederexe);
//	#include "runxfileexe.cpp"
//	instIndvfl(QString("runxfile.exe"), runxfileexe);
	#include "emtxfileexe.cpp"
	instIndvfl(QString("emtxfile.exe"), emtxfileexe);
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
