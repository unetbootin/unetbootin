QString cpuarch;
QString relname = nameVersion.toLower();

if (nameDistro == "CentOS")
{
	if (isarch64)
	{
		cpuarch = "x86_64";
	}
	else
	{
		cpuarch = "i386";
	}
	downloadfile(QString("http://isoredirect.centos.org/centos/%1/os/%2/images/pxeboot/vmlinuz").arg(relname, cpuarch), QString("%1ubnkern").arg(targetPath));
	downloadfile(QString("http://isoredirect.centos.org/centos/%1/os/%2/images/pxeboot/initrd.img").arg(relname, cpuarch), QString("%1ubninit").arg(targetPath));
	postinstmsg = QObject::tr("\n*IMPORTANT* After rebooting, ignore any error messages and select back if prompted for a CD, then go to the main menu, select the 'Start Installation' option, choose 'Network' as the source, choose 'HTTP' as the protocol, enter 'mirrors.kernel.org' when prompted for a server, and enter '/centos/%1/os/%2' when asked for the folder.").arg(nameVersion, cpuarch);
	kernelOpts = "splash=silent showopts";
}

if (nameDistro == "Debian")
{
	if (isarch64)
	{
		cpuarch = "amd64";
	}
	else
	{
		cpuarch = "i386";
	}
	downloadfile(QString("http://ftp.debian.org/debian/dists/%1/main/installer-%2/current/images/netboot/gtk/debian-installer/%2/linux").arg(relname, cpuarch), QString("%1ubnkern").arg(targetPath));
	downloadfile(QString("http://ftp.debian.org/debian/dists/%1/main/installer-%2/current/images/netboot/gtk/debian-installer/%2/initrd.gz").arg(relname, cpuarch), QString("%1ubninit").arg(targetPath));
	kernelOpts = "video=vesa:ywrap,mtrr vga=788 installgui";
}

if (nameDistro == "Fedora")
{
	if (isarch64)
	{
		cpuarch = "x86_64";
	}
	else
	{
		cpuarch = "i386";
	}
	if (relname == "rawhide")
	{
		downloadfile(QString("http://download.fedora.redhat.com/pub/fedora/linux/development/%1/os/images/pxeboot/vmlinuz").arg(cpuarch), QString("%1ubnkern").arg(targetPath));
		downloadfile(QString("http://download.fedora.redhat.com/pub/fedora/linux/development/%1/os/images/pxeboot/initrd.img").arg(cpuarch), QString("%1ubninit").arg(targetPath));
		postinstmsg = QObject::tr("\n*IMPORTANT* After rebooting, ignore any error messages and select back if prompted for a CD, then go to the main menu, select the 'Start Installation' option, choose 'Network' as the source, choose 'HTTP' as the protocol, enter 'download.fedora.redhat.com' when prompted for a server, and enter '/pub/fedora/linux/development/%1/os' when asked for the folder.").arg(cpuarch);
	}
	else if (relname == "9 alpha")
	{
		downloadfile(QString("http://download.fedora.redhat.com/pub/fedora/linux/releases/test/9-Alpha/Fedora/%1/os/images/pxeboot/vmlinuz").arg(cpuarch), QString("%1ubnkern").arg(targetPath));
		downloadfile(QString("http://download.fedora.redhat.com/pub/fedora/linux/releases/test/9-Alpha/Fedora/%1/os/images/pxeboot/initrd.img").arg(cpuarch), QString("%1ubninit").arg(targetPath));
		postinstmsg = QObject::tr("\n*IMPORTANT* After rebooting, ignore any error messages and select back if prompted for a CD, then go to the main menu, select the 'Start Installation' option, choose 'Network' as the source, choose 'HTTP' as the protocol, enter 'download.fedora.redhat.com' when prompted for a server, and enter '/pub/fedora/linux/releases/test/9-Alpha/Fedora/%1/os' when asked for the folder.").arg(cpuarch);
	}
	else
	{
		downloadfile(QString("http://download.fedora.redhat.com/pub/fedora/linux/releases/%1/Fedora/%2/os/images/pxeboot/vmlinuz").arg(relname, cpuarch), QString("%1ubnkern").arg(targetPath));
		downloadfile(QString("http://download.fedora.redhat.com/pub/fedora/linux/releases/%1/Fedora/%2/os/images/pxeboot/initrd.img").arg(relname, cpuarch), QString("%1ubninit").arg(targetPath));
		postinstmsg = QObject::tr("\n*IMPORTANT* After rebooting, ignore any error messages and select back if prompted for a CD, then go to the main menu, select the 'Start Installation' option, choose 'Network' as the source, choose 'HTTP' as the protocol, enter 'download.fedora.redhat.com' when prompted for a server, and enter '/pub/fedora/linux/releases/%1/Fedora/%2/os' when asked for the folder.").arg(relname, cpuarch);
	}
	kernelOpts = "splash=silent showopts";
}

if (nameDistro == "FreeBSD")
{
	if (isarch64)
	{
		cpuarch = "-x64";
	}
	else
	{
		cpuarch = "";
	}
	instIndvfl(QString("%1ubnkern").arg(targetPath), memdisk);
	downloadfile(QString("http://downloads.sourceforge.net/sourceforge/lubi/freebsd-%1%2.img.gz").arg(relname, cpuarch), QString("%1ubninit").arg(targetPath));
}

if (nameDistro == "Frugalware")
{
	if (isarch64)
	{
		cpuarch = "x86_64";
	}
	else
	{
		cpuarch = "i686";
	}
	QString pageurl = QString("http://www10.frugalware.org/pub/linux/frugalware/frugalware-%1/boot/").arg(relname);
	QString pagecontents = downloadpagecontents(pageurl);
	QString kernpartname = pagecontents.replace("<", "\n").replace(">", "\n").split("\n").filter(QRegExp("^\\s{0,}vmlinuz.{0,}"+cpuarch)).at(0);
	QString initpartname = pagecontents.replace("<", "\n").replace(">", "\n").split("\n").filter(QRegExp("^\\s{0,}initrd.{0,}"+cpuarch+".img.gz")).at(0);
	downloadfile(QString("%1%2").arg(pageurl, kernpartname), QString("%1ubnkern").arg(targetPath));
	downloadfile(QString("%1%2").arg(pageurl, initpartname), QString("%1ubninit").arg(targetPath));
	kernelOpts = "load_ramdisk=1 prompt_ramdisk=0 ramdisk_size=100000 rw root=/dev/ram quiet vga=791";
}

if (nameDistro == "Mandriva")
{
	if (isarch64)
	{
		cpuarch = "x86_64";
	}
	else
	{
		cpuarch = "i586";
	}
	instIndvfl(QString("%1ubnkern").arg(targetPath), memdisk);
	downloadfile(QString("ftp://mirrors.kernel.org/mandrake/Mandrakelinux/official/%1/%2/install/images/all.img").arg(relname, cpuarch), QString("%1ubninit").arg(targetPath));
}

if (nameDistro == "NetBSD")
{
	if (isarch64)
	{
		cpuarch = "amd64";
	}
	else
	{
		cpuarch = "i386";
	}
	downloadfile(QString("http://ftp.netbsd.org/pub/NetBSD/NetBSD-%1/%2/binary/kernel/netbsd-INSTALL.gz").arg(relname, cpuarch), QString("%1ubnkern").arg(targetPath));
	kernelParam = "--type=netbsd";
	initrdLine = "";
	initrdOpts = "";
	initrdLoc = "";
}

if (nameDistro == "openSUSE")
{
	if (isarch64)
	{
		cpuarch = "x86_64";
	}
	else
	{
		cpuarch = "i386";
	}
	if (relname == "factory")
	{
		downloadfile(QString("http://download.opensuse.org/distribution/SL-OSS-factory/inst-source/boot/%1/loader/linux").arg(cpuarch), QString("%1ubnkern").arg(targetPath));
		downloadfile(QString("http://download.opensuse.org/distribution/SL-OSS-factory/inst-source/boot/%1/loader/initrd").arg(cpuarch), QString("%1ubninit").arg(targetPath));
		postinstmsg = QObject::tr("\n*IMPORTANT* After rebooting, ignore any error messages and select back if prompted for a CD, then go to the main menu, select the 'Start Installation' option, choose 'Network' as the source, choose 'HTTP' as the protocol, enter 'download.opensuse.org' when prompted for a server, and enter '/distribution/SL-OSS-factory/inst-source' when asked for the folder.");
	}
	else
	{
		downloadfile(QString("http://download.opensuse.org/distribution/%1/repo/oss/boot/%2/loader/linux").arg(relname, cpuarch), QString("%1ubnkern").arg(targetPath));
		downloadfile(QString("http://download.opensuse.org/distribution/%1/repo/oss/boot/%2/loader/initrd").arg(relname, cpuarch), QString("%1ubninit").arg(targetPath));
		postinstmsg = QObject::tr("\n*IMPORTANT* After rebooting, ignore any error messages and select back if prompted for a CD, then go to the main menu, select the 'Start Installation' option, choose 'Network' as the source, choose 'HTTP' as the protocol, enter 'download.opensuse.org' when prompted for a server, and enter '/distribution/%1/repo/oss' when asked for the folder.").arg(relname);
	}
	kernelOpts = "splash=silent showopts";
}

if (nameDistro == "Parted Magic")
{
	if (relname == "2.1")
	{
		downloadfile("http://downloads.sourceforge.net/sourceforge/lubi/partedmagic-2.1-kernel", QString("%1ubnkern").arg(targetPath));
		downloadfile("http://downloads.sourceforge.net/sourceforge/lubi/partedmagic-2.1-initrd", QString("%1ubninit").arg(targetPath));
	}
	kernelOpts = "noapic root=/dev/ram0 init=/linuxrc ramdisk_size=200000 keymap=us liveusb vga=791 quiet toram";
}

if (nameDistro == "Ubuntu")
{
	if (isarch64)
	{
		cpuarch = "amd64";
	}
	else
	{
		cpuarch = "i386";
	}
	if (relname == "8.04")
	{
		relname = "hardy";
	}
	if (relname == "7.10")
	{
		relname = "gutsy";
	}
	if (relname == "7.04")
	{
		relname = "feisty";
	}
	if (relname == "6.10")
	{
		relname = "edgy";
	}
	if (relname == "6.06")
	{
		relname = "dapper";
	}
	downloadfile(QString("http://archive.ubuntu.com/ubuntu/dists/%1/main/installer-%2/current/images/netboot/ubuntu-installer/%2/linux").arg(relname, cpuarch), QString("%1ubnkern").arg(targetPath));
	downloadfile(QString("http://archive.ubuntu.com/ubuntu/dists/%1/main/installer-%2/current/images/netboot/ubuntu-installer/%2/initrd.gz").arg(relname, cpuarch), QString("%1ubninit").arg(targetPath));
	kernelOpts = "vga=normal";
}
