/*
distrolst.cpp from UNetbootin <http://unetbootin.sourceforge.net>
Copyright (C) Geza Kovacs <geza0kovacs@gmail.com>

This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 2 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License at <http://www.gnu.org/licenses/> for more details.
*/

#ifdef AUTOSUPERGRUBDISK

if (nameDistro == "Auto Super Grub Disk")
{
	instIndvfl("memdisk", QString("%1ubnkern").arg(targetPath));
	instIndvfl("asgd.img", QString("%1ubninit").arg(targetPath));
}

#endif

#ifdef EEEPCLOS

if (nameDistro == "EeePCLinuxOS")
{
	downloadfile(QString("http://www.eeepclinuxos.com/eeepclos-%1.iso").arg(relname), isotmpf);
	extractiso(isotmpf, targetPath);
}

#endif

#ifdef EEEUBUNTU

if (nameDistro == "Ubuntu Eee")
{
	downloadfile(QString("http://lubi.sourceforge.net/ubuntu-eee-%1.iso").arg(relname), isotmpf);
	extractiso(isotmpf, targetPath);
}

#endif

#ifdef ELIVE

if (nameDistro == "Elive")
{
	downloadfile(fileFilterNetDir(QStringList() << 
	"http://elive.leviathan-avc.com/development/" << 
	"http://elive.cmhacks.com/development/" << 
	"http://elive.icedslash.com/isos/development/"
	, 524288000, 1048576000, QList<QRegExp>() << 
	QRegExp(".iso$", Qt::CaseInsensitive) << 
	QRegExp("elive\\S{0,}.iso$", Qt::CaseInsensitive)
	), isotmpf);
	extractiso(isotmpf, targetPath);
}

#endif

#ifdef SLITAZ

if (nameDistro == "SliTaz")
{
	if (relname == "webboot")
	{
		instIndvfl("gpxe", QString("%1ubnkern").arg(targetPath));
		kernelOpts = "url=http://mirror.slitaz.org/pxe/pxelinux.0";
		slinitrdLine = "";
		initrdLine = "";
		initrdOpts = "";
		initrdLoc = "";
//		downloadfile("http://mirror.slitaz.org/boot/slitaz-boot.iso", isotmpf);
//		extractiso(isotmpf, targetPath);
	}
	else
	{
		downloadfile(QString("http://mirror.slitaz.org/iso/%1/slitaz-%1.iso").arg(relname), isotmpf);
		extractiso(isotmpf, targetPath);
	}

}

#endif

#ifdef STDUNETBOOTIN

if (nameDistro == "Arch Linux")
{
	if (isarch64)
	{
		cpuarch = "x86_64";
	}
	else
	{
		cpuarch = "i686";
	}
	if (relname.contains("2007.08"))
	{
		downloadfile(QString("ftp://ftp.archlinux.org/iso/%3/%2/Archlinux-%2-%1.ftp.iso").arg(relname, cpuarch, QString(relname).remove(QRegExp("-\\d{0,}$"))), isotmpf);
	}
	else if (relname.contains("2008.03"))
	{
		downloadfile(QString("ftp://ftp.archlinux.org/iso/%3/%2/Archlinux-%2-%1-archboot.ftp.iso").arg(relname, cpuarch, QString(relname).remove(QRegExp("-\\d{0,}$"))), isotmpf);
	}
	else
	{
		downloadfile(QString("ftp://ftp.archlinux.org/iso/%3/archlinux-%1-ftp-%2.iso").arg(relname, cpuarch, QString(relname).remove(QRegExp("-\\d{0,}$"))), isotmpf);
	}
	extractiso(isotmpf, targetPath);
}

if (nameDistro == "BackTrack")
{
	downloadfile(QString("http://www.remote-exploit.org/cgi-bin/fileget?version=bt%1-usb").arg(relname), isotmpf);
	extractiso(isotmpf, targetPath);
}

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
	postinstmsg = unetbootin::tr("\n*IMPORTANT* After rebooting, ignore any error messages and select back if prompted for a CD, then go to the main menu, select the 'Start Installation' option, choose 'Network' as the source, choose 'HTTP' as the protocol, enter 'mirrors.kernel.org' when prompted for a server, and enter '/centos/%1/os/%2' when asked for the folder.").arg(nameVersion, cpuarch);
	kernelOpts = "splash=silent showopts";
}

if (nameDistro == "CloneZilla")
{
	downloadfile(QString("http://downloads.sourceforge.net/clonezilla/clonezilla-live-%1.iso").arg(relname), isotmpf);
	extractiso(isotmpf, targetPath);
}

if (nameDistro == "Damn Small Linux")
{
	downloadfile(fileFilterNetDir(QStringList() << 
	"ftp://ibiblio.org/pub/Linux/distributions/damnsmall/current/" << 
	"http://ibiblio.org/pub/Linux/distributions/damnsmall/current/" << 
	"ftp://gd.tuwien.ac.at/opsys/linux/damnsmall/current/" << 
	"http://gd.tuwien.ac.at/opsys/linux/damnsmall/current/" << 
	"ftp://ftp.is.co.za/linux/distributions/damnsmall/current/" << 
//	"ftp://ftp.belnet.be/packages/damnsmalllinux/current/" << 
	"http://ftp.belnet.be/packages/damnsmalllinux/current/" << 
	"ftp://ftp.heanet.ie/mirrors/damnsmalllinux.org/current/" << 
	"http://ftp.heanet.ie/mirrors/damnsmalllinux.org/current/"
//	"ftp://ftp.oss.cc.gatech.edu/pub/linux/distributions/damnsmall/current/" << 
//	"http://ftp.oss.cc.gatech.edu/pub/linux/distributions/damnsmall/current/" <<
//	"ftp://ftp.planetmirror.com/pub/damnsmall/current/" <<
//	"http://planetmirror.com/pub/damnsmall/current/" <<
	, 3072000, 1048576000, QList<QRegExp>() << 
	QRegExp("^dsl", Qt::CaseInsensitive) << 
	QRegExp(".iso$", Qt::CaseInsensitive) << 
	QRegExp("initrd", Qt::CaseInsensitive) << 
	QRegExp("initrd.iso$", Qt::CaseInsensitive) << 
	QRegExp("^dsl-\\S{1,}-initrd.iso$", Qt::CaseInsensitive)
	), isotmpf);
	extractiso(isotmpf, targetPath);
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
	if (islivecd)
	{
		relname.replace("unstable", "sid").replace("testing", "lenny").replace("stable", "etch");
		downloadfile(QString("http://live.debian.net/cdimage/%1-builds/current/%2/debian-live-%1-%2-gnome-desktop.iso").arg(relname, cpuarch), isotmpf);
		extractiso(isotmpf, targetPath);
	}
	else
	{
		downloadfile(QString("http://ftp.debian.org/debian/dists/%1/main/installer-%2/current/images/netboot/gtk/debian-installer/%2/linux").arg(relname, cpuarch), QString("%1ubnkern").arg(targetPath));
		downloadfile(QString("http://ftp.debian.org/debian/dists/%1/main/installer-%2/current/images/netboot/gtk/debian-installer/%2/initrd.gz").arg(relname, cpuarch), QString("%1ubninit").arg(targetPath));
		kernelOpts = "video=vesa:ywrap,mtrr vga=788 installgui";
	}
}

if (nameDistro == "Dreamlinux")
{
	downloadfile(fileFilterNetDir(QStringList() << 
	"ftp://ftp.nluug.nl/pub/os/Linux/distr/dreamlinux/stable/" << 
	"http://ftp.nluug.nl/pub/os/Linux/distr/dreamlinux/stable/" << 
	"ftp://ftp.surfnet.nl/pub/os/Linux/distr/dreamlinux/stable/" << 
	"http://ftp.surfnet.nl/pub/os/Linux/distr/dreamlinux/stable/"
	, 524288000, 1048576000, QList<QRegExp>() << 
	QRegExp(".iso$", Qt::CaseInsensitive) << 
	QRegExp("DL\\S{0,}.iso$", Qt::CaseInsensitive)
	), isotmpf);
	extractiso(isotmpf, targetPath);
}

if (nameDistro == "Elive")
{
	downloadfile(fileFilterNetDir(QStringList() << 
	"http://elive.leviathan-avc.com/development/" << 
	"http://elive.cmhacks.com/development/" << 
	"http://elive.icedslash.com/isos/development/"
	, 524288000, 1048576000, QList<QRegExp>() << 
	QRegExp(".iso$", Qt::CaseInsensitive) << 
	QRegExp("elive\\S{0,}.iso$", Qt::CaseInsensitive)
	), isotmpf);
	extractiso(isotmpf, targetPath);
}

if (nameDistro == "FaunOS")
{
	downloadfile(QString("http://download.faunos.com/FaunOS-%1/FaunOS-%1-dvd.iso").arg(relname), isotmpf);
	extractiso(isotmpf, targetPath);
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
	if (islivecd)
	{
		if (!isarch64)
		{
			cpuarch = "i686";
		}
		if (relname == "8")
		{
			downloadfile(QString("http://download.fedora.redhat.com/pub/fedora/linux/releases/%1/Live/%2/Fedora-%1-Live-%2.iso").arg(relname, cpuarch), isotmpf);
		}
		else
		{
			downloadfile(QString("http://download.fedora.redhat.com/pub/fedora/linux/releases/%1/Live/%2/Fedora-%1-%2-Live.iso").arg(relname, cpuarch), isotmpf);
		}
		extractiso(isotmpf, targetPath);
	}
	else
	{
		if (relname == "rawhide")
		{
			downloadfile(QString("http://download.fedora.redhat.com/pub/fedora/linux/development/%1/os/images/pxeboot/vmlinuz").arg(cpuarch), QString("%1ubnkern").arg(targetPath));
			downloadfile(QString("http://download.fedora.redhat.com/pub/fedora/linux/development/%1/os/images/pxeboot/initrd.img").arg(cpuarch), QString("%1ubninit").arg(targetPath));
			postinstmsg = unetbootin::tr("\n*IMPORTANT* After rebooting, ignore any error messages and select back if prompted for a CD, then go to the main menu, select the 'Start Installation' option, choose 'Network' as the source, choose 'HTTP' as the protocol, enter 'download.fedora.redhat.com' when prompted for a server, and enter '/pub/fedora/linux/development/%1/os' when asked for the folder.").arg(cpuarch);
		}
		else
		{
			downloadfile(QString("http://download.fedora.redhat.com/pub/fedora/linux/releases/%1/Fedora/%2/os/images/pxeboot/vmlinuz").arg(relname, cpuarch), QString("%1ubnkern").arg(targetPath));
			downloadfile(QString("http://download.fedora.redhat.com/pub/fedora/linux/releases/%1/Fedora/%2/os/images/pxeboot/initrd.img").arg(relname, cpuarch), QString("%1ubninit").arg(targetPath));
			postinstmsg = unetbootin::tr("\n*IMPORTANT* After rebooting, ignore any error messages and select back if prompted for a CD, then go to the main menu, select the 'Start Installation' option, choose 'Network' as the source, choose 'HTTP' as the protocol, enter 'download.fedora.redhat.com' when prompted for a server, and enter '/pub/fedora/linux/releases/%1/Fedora/%2/os' when asked for the folder.").arg(relname, cpuarch);
		}
		kernelOpts = "splash=silent showopts";
	}
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
	instIndvfl("memdisk", QString("%1ubnkern").arg(targetPath));
	downloadfile(QString("http://downloads.sourceforge.net/sourceforge/lubi/freebsd-%1%2.img.gz").arg(relname, cpuarch), QString("%1ubninit").arg(targetPath));
}

if (nameDistro == "FreeDOS")
{
	instIndvfl("memdisk", QString("%1ubnkern").arg(targetPath));
	downloadfile(QString("http://www.ibiblio.org/pub/micro/pc-stuff/freedos/files/distributions/%1/fdboot.img").arg(relname), QString("%1ubninit").arg(targetPath));
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
	QStringList frugalwaremirrorsL = QStringList() <<
//	"ftp://ftp5.frugalware.org/packages/frugalware/pub/frugalware-"+relname+"/boot/" <<
//	"http://www5.frugalware.org/packages/frugalware/pub/frugalware-"+relname+"/boot/" <<
	"ftp://ftp8.frugalware.org/distro/frugalware/frugalware-"+relname+"/boot/" <<
	"http://www8.frugalware.org/distro/frugalware/frugalware-"+relname+"/boot/" <<
	"ftp://ftp10.frugalware.org/pub/linux/frugalware/frugalware-"+relname+"/boot/" <<
	"http://www10.frugalware.org/pub/linux/frugalware/frugalware-"+relname+"/boot/" <<
	"ftp://ftp12.frugalware.org/mirrors/ftp.frugalware.org/pub/frugalware/frugalware-"+relname+"/boot/" <<
	"http://www12.frugalware.org/mirrors/ftp.frugalware.org/pub/frugalware/frugalware-"+relname+"/boot/" <<
	"ftp://ftp4.frugalware.org/pub/linux/distributions/frugalware/frugalware-"+relname+"/boot/" <<
	"http://www4.frugalware.org/pub/linux/distributions/frugalware/frugalware-"+relname+"/boot/" <<
	"ftp://ftp2.frugalware.org/frugalware/pub/frugalware/frugalware-"+relname+"/boot/" <<
	"ftp://ftp3.frugalware.org/mirrors/frugalware/pub/frugalware/frugalware-"+relname+"/boot/";
	downloadfile(fileFilterNetDir(frugalwaremirrorsL
	, 307200, 104857600, QList<QRegExp>() << 
	QRegExp("vmlinuz", Qt::CaseInsensitive) <<
	QRegExp("^vmlinuz-", Qt::CaseInsensitive) <<
	QRegExp(cpuarch, Qt::CaseInsensitive) <<
	QRegExp("-"+cpuarch+"$", Qt::CaseInsensitive) <<
	QRegExp("^vmlinuz-\\S{1,}-"+cpuarch+"$", Qt::CaseInsensitive)
	), QString("%1ubnkern").arg(targetPath));
	downloadfile(fileFilterNetDir(frugalwaremirrorsL
	, 3145728, 1048576000, QList<QRegExp>() << 
	QRegExp("initrd", Qt::CaseInsensitive) <<
	QRegExp("^initrd-", Qt::CaseInsensitive) <<
	QRegExp(cpuarch, Qt::CaseInsensitive) <<
	QRegExp("-"+cpuarch+".img", Qt::CaseInsensitive) <<
	QRegExp("-"+cpuarch+".img.gz$", Qt::CaseInsensitive) <<
	QRegExp("^initrd-\\S{0,}"+cpuarch+".img.gz$", Qt::CaseInsensitive)
	), QString("%1ubninit").arg(targetPath));
	kernelOpts = "load_ramdisk=1 prompt_ramdisk=0 ramdisk_size=100000 rw root=/dev/ram quiet vga=791";
}

if (nameDistro == "GAG")
{
	instIndvfl("memdisk", QString("%1ubnkern").arg(targetPath));
	downloadfile(QString("http://downloads.sourceforge.net/sourceforge/lubi/gag-%1.img.gz").arg(relname), QString("%1ubninit").arg(targetPath));
}

if (nameDistro == "Gentoo")
{
	if (isarch64)
	{
		cpuarch = "amd64";
	}
	else
	{
		cpuarch = "x86";
	}
	downloadfile(QString("http://bouncer.gentoo.org/fetch/gentoo-%1-livecd/%2/").arg(relname, cpuarch), isotmpf);
	extractiso(isotmpf, targetPath);
}

if (nameDistro == "Gujin")
{
	instIndvfl("memdisk", QString("%1ubnkern").arg(targetPath));
	downloadfile(QString("http://downloads.sourceforge.net/sourceforge/lubi/gujin-%1.img.gz").arg(relname), QString("%1ubninit").arg(targetPath));
}

if (nameDistro == "Linux Mint")
{
	downloadfile(QString("ftp://mirrors.secution.com/linuxmint.com/%1/LinuxMint-%2.iso").arg(QString(relname).remove(QRegExp("-r\\d{0,}")), relname), isotmpf);
	extractiso(isotmpf, targetPath);
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
	if (islivecd)
	{
		if (relname == "2008.0")
		{
			downloadfile(QString("ftp://ftp.free.fr/mirrors/ftp.mandriva.com/MandrivaLinux/official/iso/%1/mandriva-linux-%3-one-GNOME-cdrom-%2.iso").arg(relname, cpuarch, QString(relname).remove(QRegExp("\\.\\d{0,}$"))), isotmpf);
		}
		else if (relname == "2008.1")
		{
			downloadfile(QString("ftp://ftp.free.fr/mirrors/ftp.mandriva.com/MandrivaLinux/official/iso/%1/mandriva-linux-one-%3-spring-GNOME-int-cdrom-%2.iso").arg(relname, cpuarch, QString(relname).remove(QRegExp("\\.\\d{0,}$"))), isotmpf);
		}
		extractiso(isotmpf, targetPath);
	}
	else
	{
		instIndvfl("memdisk", QString("%1ubnkern").arg(targetPath));
		downloadfile(QString("ftp://ftp.free.fr/mirrors/ftp.mandriva.com/MandrivaLinux/official/%1/%2/install/images/all.img").arg(relname, cpuarch), QString("%1ubninit").arg(targetPath));
	}
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
	slinitrdLine = "";
	initrdLine = "";
	initrdOpts = "";
	initrdLoc = "";
}

if (nameDistro == "NTPasswd")
{
	downloadfile(QString("http://downloads.sourceforge.net/sourceforge/lubi/ntpasswd-%1-kernel").arg(relname), QString("%1ubnkern").arg(targetPath));
	downloadfile(QString("http://downloads.sourceforge.net/sourceforge/lubi/ntpasswd-%1-initrd.gz").arg(relname), QString("%1ubninit").arg(targetPath));
	kernelOpts = "rw vga=1 init=/linuxrc";
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
	if (islivecd)
	{
		downloadfile(QString("http://download.opensuse.org/distribution/%1/iso/cd/openSUSE-%1-GM-GNOME-Live-%2.iso").arg(relname, cpuarch), isotmpf);
		extractiso(isotmpf, targetPath);
	}
	else
	{
		if (relname == "factory")
		{
			downloadfile(QString("http://download.opensuse.org/distribution/SL-OSS-factory/inst-source/boot/%1/loader/linux").arg(cpuarch), QString("%1ubnkern").arg(targetPath));
			downloadfile(QString("http://download.opensuse.org/distribution/SL-OSS-factory/inst-source/boot/%1/loader/initrd").arg(cpuarch), QString("%1ubninit").arg(targetPath));
			postinstmsg = unetbootin::tr("\n*IMPORTANT* After rebooting, ignore any error messages and select back if prompted for a CD, then go to the main menu, select the 'Start Installation' option, choose 'Network' as the source, choose 'HTTP' as the protocol, enter 'download.opensuse.org' when prompted for a server, and enter '/distribution/SL-OSS-factory/inst-source' when asked for the folder.");
		}
		else
		{
			downloadfile(QString("http://download.opensuse.org/distribution/%1/repo/oss/boot/%2/loader/linux").arg(relname, cpuarch), QString("%1ubnkern").arg(targetPath));
			downloadfile(QString("http://download.opensuse.org/distribution/%1/repo/oss/boot/%2/loader/initrd").arg(relname, cpuarch), QString("%1ubninit").arg(targetPath));
			postinstmsg = unetbootin::tr("\n*IMPORTANT* After rebooting, ignore any error messages and select back if prompted for a CD, then go to the main menu, select the 'Start Installation' option, choose 'Network' as the source, choose 'HTTP' as the protocol, enter 'download.opensuse.org' when prompted for a server, and enter '/distribution/%1/repo/oss' when asked for the folder.").arg(relname);
			}
		kernelOpts = "splash=silent showopts";
	}
}

if (nameDistro == "Ophcrack")
{
	downloadfile(QString("http://downloads.sourceforge.net/ophcrack/ophcrack-%1.iso").arg(relname), isotmpf);
	extractiso(isotmpf, targetPath);
}

if (nameDistro == "Parted Magic")
{
	if (relname == "2.1")
	{
		downloadfile("http://downloads.sourceforge.net/sourceforge/lubi/partedmagic-2.1-kernel", QString("%1ubnkern").arg(targetPath));
		downloadfile("http://downloads.sourceforge.net/sourceforge/lubi/partedmagic-2.1-initrd", QString("%1ubninit").arg(targetPath));
		kernelOpts = "noapic root=/dev/ram0 init=/linuxrc ramdisk_size=200000 keymap=us liveusb vga=791 quiet toram";	
	}
	else
	{
		downloadfile(fileFilterNetDir(QStringList() << 
		"http://partedmagic.com/downloads/stable/" << 
		"http://exo.enarel.eu/mirror/partedmagic/" << 
		"http://fulloffacts.com/get/partedmagic/" <<
		"http://www.digitalincursion.net/partedmagic/"
		, 10485760, 209715200, QList<QRegExp>() << 
		QRegExp("^pmagic", Qt::CaseInsensitive) << 
		QRegExp("pxe", Qt::CaseInsensitive) << 
		QRegExp("-pxe", Qt::CaseInsensitive) << 
		QRegExp("^pmagic-pxe", Qt::CaseInsensitive)
		), isotmpf);
		extractiso(isotmpf, targetPath);
	}
}

if (nameDistro == "PCLinuxOS")
{
	if (relname == "2007")
	{
		downloadfile("ftp://distro.ibiblio.org/pub/linux/distributions/texstar/pclinuxos/live-cd/english/preview/pclinuxos-2007.iso", isotmpf);
	}
	if (relname == "2008 gnome")
	{
		downloadfile("ftp://distro.ibiblio.org/pub/linux/distributions/texstar/pclinuxos/live-cd/english/preview/pclos-gnome2008.iso", isotmpf);
	}
	if (relname == "2008 minime")
	{
		downloadfile("ftp://distro.ibiblio.org/pub/linux/distributions/texstar/pclinuxos/live-cd/english/preview/pclinuxos-minime-2008.iso", isotmpf);
	}
	extractiso(isotmpf, targetPath);
}

if (nameDistro == "Puppy Linux")
{
	downloadfile(fileFilterNetDir(QStringList() << 
	"ftp://ibiblio.org/pub/linux/distributions/puppylinux/" << 
	"http://distro.ibiblio.org/pub/linux/distributions/puppylinux/" <<
	"ftp://ftp.nluug.nl/ftp/pub/os/Linux/distr/puppylinux/" << 
	"http://ftp.nluug.nl/ftp/pub/os/Linux/distr/puppylinux/"
	, 61440000, 1048576000, QList<QRegExp>() << 
	QRegExp(".iso$", Qt::CaseInsensitive) << 
	QRegExp("^puppy-\\d{1,}\\S{1,}.iso$", Qt::CaseInsensitive) << 
	QRegExp("^puppy-4\\S{1,}.iso$", Qt::CaseInsensitive)
	), isotmpf);
	extractiso(isotmpf, targetPath);
}

if (nameDistro == "Slax")
{
	downloadfile("http://www.slax.org/get_slax.php?download=iso", isotmpf);
	extractiso(isotmpf, targetPath);
}

if (nameDistro == "SliTaz")
{
	downloadfile(QString("http://mirror.slitaz.org/iso/%1/slitaz-%1.iso").arg(relname), isotmpf);
	extractiso(isotmpf, targetPath);
}

if (nameDistro == "Smart Boot Manager")
{
	instIndvfl("memdisk", QString("%1ubnkern").arg(targetPath));
	downloadfile(QString("http://downloads.sourceforge.net/sourceforge/lubi/sbm-%1.img.gz").arg(relname), QString("%1ubninit").arg(targetPath));
}

if (nameDistro == "Super Grub Disk")
{
	instIndvfl("memdisk", QString("%1ubnkern").arg(targetPath));
	downloadfile(QString("http://lubi.sourceforge.net/sgd-latest.img").arg(relname), QString("%1ubninit").arg(targetPath));
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
	if (relname == "daily")
	{
		downloadfile(fileFilterNetDir(QStringList() << 
		"http://cdimage.ubuntu.com/daily-live/current/"
		, 61440000, 1048576000, QList<QRegExp>() << 
		QRegExp(".iso$", Qt::CaseInsensitive) << 
		QRegExp("desktop\\S{0,}.iso$", Qt::CaseInsensitive) << 
		QRegExp("desktop-"+cpuarch+".iso$", Qt::CaseInsensitive)
		), isotmpf);
		extractiso(isotmpf, targetPath);
	}
	else
	{
		if (islivecd)
		{
			downloadfile(fileFilterNetDir(QStringList() << 
			"http://releases.ubuntu.com/"+relname << 
			"http://releases.ubuntu.com/releases/"+relname <<
			"ftp://releases.ubuntu.com/releases/.pool/" << 
			"http://mirrors.gigenet.com/ubuntu/"+relname <<
			"http://mirrors.easynews.com/linux/ubuntu-releases/"+relname <<
			"http://www.gtlib.gatech.edu/pub/ubuntu-releases/"+relname <<
			"http://ftp.wayne.edu/linux_distributions/ubuntu/"+relname <<
			"http://ubuntu.mirrors.proxad.net/"+relname
			, 524288000, 1048576000, QList<QRegExp>() << 
			QRegExp(".iso$", Qt::CaseInsensitive) << 
			QRegExp(cpuarch+".iso$", Qt::CaseInsensitive) << 
			QRegExp("desktop-"+cpuarch+".iso$", Qt::CaseInsensitive) << 
			QRegExp("desktop-"+cpuarch+".iso$", Qt::CaseInsensitive) << 
			QRegExp("ubuntu\\S{0,}"+relname+"\\S{0,}desktop\\S{0,}"+cpuarch+"\\S{0,}.iso$", Qt::CaseInsensitive) << 
			QRegExp("ubuntu-"+relname+"\\S{0,}-desktop-"+cpuarch+".iso$", Qt::CaseInsensitive)
			), isotmpf);
			extractiso(isotmpf, targetPath);
		}
		else
		{
			relname.replace("8.04", "hardy").replace("7.10", "gutsy").replace("7.04", "feisty").replace("6.10", "edgy").replace("6.06", "dapper");
			downloadfile(QString("http://archive.ubuntu.com/ubuntu/dists/%1/main/installer-%2/current/images/netboot/ubuntu-installer/%2/linux").arg(relname, cpuarch), QString("%1ubnkern").arg(targetPath));
			downloadfile(QString("http://archive.ubuntu.com/ubuntu/dists/%1/main/installer-%2/current/images/netboot/ubuntu-installer/%2/initrd.gz").arg(relname, cpuarch), QString("%1ubninit").arg(targetPath));
			kernelOpts = "vga=normal";
		}
	}
}

if (nameDistro == "Zenwalk")
{
	downloadfile(QString("ftp://zenwalk.mirrors.tds.net/pub/linux/zenlive/zenwalk-live-%1.iso").arg(relname), isotmpf);
	extractiso(isotmpf, targetPath);
}

#endif
