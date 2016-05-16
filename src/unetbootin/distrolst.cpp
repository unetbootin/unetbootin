/*
distrolst.cpp from UNetbootin <http://unetbootin.sourceforge.net>
Copyright (C) 2007-2008 Geza Kovacs <geza0kovacs@gmail.com>

This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 2 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License at <http://www.gnu.org/licenses/> for more details.
*/

#ifndef debianrelnamereplace
#define debianrelnamereplace \
	relname \
	.replace("unstable", "sid") \
    .replace("testing", "stretch") \
    .replace("stable", "jessie");
#endif

#ifndef ubunturelnamereplace
#define ubunturelnamereplace \
	relname \
    .replace("16.10", "yakkety") \
    .replace("16.04", "xenial") \
    .replace("15.10", "wily") \
    .replace("15.04", "vivid") \
    .replace("14.10", "utopic") \
    .replace("14.04", "trusty") \
	.replace("13.10", "saucy") \
	.replace("13.04", "raring") \
	.replace("12.10", "quantal") \
	.replace("12.04", "precise") \
	.replace("11.10", "oneiric") \
	.replace("11.04", "natty") \
	.replace("10.10", "maverick") \
	.replace("10.04", "lucid") \
	.replace("9.10", "karmic") \
	.replace("9.04", "jaunty") \
	.replace("8.10", "intrepid") \
	.replace("8.04", "hardy") \
	.replace("7.10", "gutsy") \
	.replace("7.04", "feisty") \
	.replace("6.10", "edgy") \
	.replace("6.06", "dapper");
#endif

#ifndef ubuntunetinst
#define ubuntunetinst \
	if (ishdmedia) \
	{ \
		ubunturelnamereplace \
		downloadfile(QString("http://archive.ubuntu.com/ubuntu/dists/%1/main/installer-%2/current/images/hd-media/vmlinuz").arg(relname, cpuarch), QString("%1ubnkern").arg(targetPath)); \
		downloadfile(QString("http://archive.ubuntu.com/ubuntu/dists/%1/main/installer-%2/current/images/hd-media/initrd.gz").arg(relname, cpuarch), QString("%1ubninit").arg(targetPath)); \
		postinstmsg = unetbootin::tr("\n*IMPORTANT* Before rebooting, place an Ubuntu alternate (not desktop) install iso file on the root directory of your hard drive or USB drive. These can be obtained from cdimage.ubuntu.com"); \
	} \
	else if (isnetinstall) \
	{ \
		ubunturelnamereplace \
		downloadfile(QString("http://archive.ubuntu.com/ubuntu/dists/%1/main/installer-%2/current/images/netboot/ubuntu-installer/%2/linux").arg(relname, cpuarch), QString("%1ubnkern").arg(targetPath)); \
		downloadfile(QString("http://archive.ubuntu.com/ubuntu/dists/%1/main/installer-%2/current/images/netboot/ubuntu-installer/%2/initrd.gz").arg(relname, cpuarch), QString("%1ubninit").arg(targetPath)); \
	}
#endif

#ifdef AUTOSUPERGRUBDISK

if (nameDistro == "Auto Super Grub Disk")
{
	instIndvfl("memdisk", QString("%1ubnkern").arg(targetPath));
	instIndvfl("asgd.img", QString("%1ubninit").arg(targetPath));
}

if (nameDistro == "Super Grub Disk")
{
        instIndvfl("memdisk", QString("%1ubnkern").arg(targetPath));
        instIndvfl("sgd.img", QString("%1ubninit").arg(targetPath));
}

#endif

#ifdef EEEPCLOS

if (nameDistro == "EeePCLinuxOS")
{
	downloadfile(QString("http://www.eeepclinuxos.com/eeepclos-%1.iso").arg(relname), isotmpf);
	extractiso(isotmpf);
}

#endif

#ifdef EEEUBUNTU

if (nameDistro == "Ubuntu Eee")
{
	downloadfile(QString("http://lubi.sourceforge.net/ubuntu-eee-%1.iso").arg(relname), isotmpf);
	extractiso(isotmpf);
}

#endif

#ifdef ELIVE

if (nameDistro == "Elive")
{
	if (relname == "unstable")
		relname = "development";
	downloadfile(fileFilterNetDir(QStringList() << 
	"http://elive.icedslash.com/isos/"+relname+"/" <<
	"http://elive.leviathan-avc.com/"+relname+"/" <<
	"http://elive.jumbef.net/"+relname+"/" <<
	"http://elive.homogenica.com/"+relname+"/" <<
	"http://elive.evryanz.net/isos/"+relname+"/" <<
	"http://elive.7ds.pl/isos/"+relname+"/"
	, 524288000, 1048576000, QList<QRegExp>() << 
	QRegExp(".iso$", Qt::CaseInsensitive) << 
	QRegExp("elive\\S{0,}.iso$", Qt::CaseInsensitive)
    ), isotmpf);
	extractiso(isotmpf);
}

#endif

#ifdef GNEWSENSE

if (nameDistro == "gNewSense")
{
	downloadfile(QString("http://cdimage.gnewsense.org/gnewsense-livecd-%1.iso").arg(relname), isotmpf);
	extractiso(isotmpf);
}

#endif

#ifdef KIWILINUX

if (nameDistro == "Kiwi Linux")
{
        downloadfile(QString("http://depo.osn.ro/content/distributii/linux/romanesti/kiwilinux-%1.iso").arg(relname), isotmpf);
        extractiso(isotmpf);

}

#endif

#ifdef NIMBLEX

if (nameDistro == "NimbleX")
{
	downloadfile("http://public.nimblex.net/Download/NimbleX-latest.iso", isotmpf);
	extractiso(isotmpf);
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
//		extractiso(isotmpf);
	}
	else
	{
		downloadfile(fileFilterNetDir(QStringList() <<
		QString("http://mirror.slitaz.org/iso/%1/").arg(relname)
		, 3072000, 1048576000, QList<QRegExp>() <<
		QRegExp("^slitaz", Qt::CaseInsensitive) <<
		QRegExp(".iso$", Qt::CaseInsensitive) <<
		QRegExp("^slitaz-\\S{1,}.iso$", Qt::CaseInsensitive)
		), isotmpf);
		extractiso(isotmpf);
	}

}

#endif

#ifdef XPUD

if (nameDistro == "xPUD")
{
	if (relname == "stable")
	{
		downloadfile("http://xpud.org/xpud-latest-iso.php", isotmpf);
	}
	else if (relname == "unstable")
	{
		downloadfile("http://xpud.org/xpud-latest-snapshot.php", isotmpf);
	}
	extractiso(isotmpf);
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
	downloadfile(fileFilterNetDir(QStringList() <<
	"http://mirrors.kernel.org/archlinux/iso/latest/" <<
	"http://distro.ibiblio.org/archlinux/iso/latest/" <<
	"http://mirror.rit.edu/archlinux/iso/latest/"
	, 3072000, 2048576000, QList<QRegExp>() <<
	QRegExp("^arch", Qt::CaseInsensitive) <<
	QRegExp("^archlinux", Qt::CaseInsensitive) <<
	QRegExp(".iso$", Qt::CaseInsensitive) <<
	QRegExp(cpuarch, Qt::CaseInsensitive) <<
	QRegExp(relname, Qt::CaseInsensitive)
	), isotmpf);
	extractiso(isotmpf);
}

if (nameDistro == "BackTrack")
{
	if (isarch64)
	{
		cpuarch = "64";
	}
	else
	{
		cpuarch = "32";
	}
	downloadfile(QString("http://www.backtrack-linux.org/ajax/download_redirect.php?id=BT%1-%2.iso").arg(nameVersion).arg(cpuarch), isotmpf);
	extractiso(isotmpf);
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
	if (islivecd)
	{
		downloadfile(fileFilterNetDir(QStringList() <<
		QString("http://mirrors.kernel.org/centos/%1/isos/%2/").arg(relname, cpuarch) <<
		QString("http://mirror.stanford.edu/yum/pub/centos/%1/isos/%2/").arg(relname, cpuarch) <<
		QString("http://ftp.osuosl.org/pub/centos/%1/isos/%2/").arg(relname, cpuarch) <<
		QString("http://mirrors.usc.edu/pub/linux/distributions/centos/%1/isos/%2/").arg(relname, cpuarch)
		, 524288000, 1048576000, QList<QRegExp>() <<
		QRegExp(".iso$", Qt::CaseInsensitive) <<
		QRegExp("LiveCD\\S{0,}.iso$", Qt::CaseInsensitive)
		), isotmpf);
		extractiso(isotmpf);
	}
	else
	{
		downloadfile(QString("http://isoredirect.centos.org/centos/%1/os/%2/images/pxeboot/vmlinuz").arg(relname, cpuarch), QString("%1ubnkern").arg(targetPath));
		downloadfile(QString("http://isoredirect.centos.org/centos/%1/os/%2/images/pxeboot/initrd.img").arg(relname, cpuarch), QString("%1ubninit").arg(targetPath));
		postinstmsg = unetbootin::tr("\n*IMPORTANT* After rebooting, ignore any error messages and select back if prompted for a CD, then go to the main menu, select the 'Start Installation' option, choose 'Network' as the source, choose 'HTTP' as the protocol, enter 'mirrors.kernel.org' when prompted for a server, and enter '/centos/%1/os/%2' when asked for the folder.").arg(nameVersion, cpuarch);
		kernelOpts = "splash=silent showopts";
	}
}

if (nameDistro == "CloneZilla")
{
	downloadfile(QString("http://downloads.sourceforge.net/clonezilla/clonezilla-live-%1.iso").arg(relname), isotmpf);
	extractiso(isotmpf);
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
	extractiso(isotmpf);
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
		debianrelnamereplace
		downloadfile(QString("http://live.debian.net/cdimage/%1-builds/current/%2/debian-live-%1-%2-gnome-desktop.iso").arg(relname, cpuarch), isotmpf);
		extractiso(isotmpf);
	}
	else if (ishdmedia)
	{
		downloadfile(QString("http://ftp.debian.org/debian/dists/%1/main/installer-%2/current/images/hd-media/vmlinuz").arg(relname, cpuarch), QString("%1ubnkern").arg(targetPath));
		downloadfile(QString("http://ftp.debian.org/debian/dists/%1/main/installer-%2/current/images/hd-media/initrd.gz").arg(relname, cpuarch), QString("%1ubninit").arg(targetPath));
		postinstmsg = unetbootin::tr("\n*IMPORTANT* Before rebooting, place a Debian install iso file on the root directory of your hard drive or USB drive. These can be obtained from cdimage.debian.org");
	}
	else
	{
		downloadfile(QString("http://ftp.debian.org/debian/dists/%1/main/installer-%2/current/images/netboot/debian-installer/%2/linux").arg(relname, cpuarch), QString("%1ubnkern").arg(targetPath));
		downloadfile(QString("http://ftp.debian.org/debian/dists/%1/main/installer-%2/current/images/netboot/debian-installer/%2/initrd.gz").arg(relname, cpuarch), QString("%1ubninit").arg(targetPath));
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
	extractiso(isotmpf);
}

if (nameDistro == "Dr.Web AntiVirus")
{
	this->searchsymlinks = true;
	downloadfile(fileFilterNetDir(QStringList() << 
	"ftp://ftp.drweb.com/pub/drweb/livecd/"
	, 9288000, 1048576000, QList<QRegExp>() << 
	QRegExp(".iso$", Qt::CaseInsensitive) << 
	QRegExp("DrWeb\\S{0,}.iso$", Qt::CaseInsensitive)
	), isotmpf);
	extractiso(isotmpf);
}

if (nameDistro == "Elive")
{
	if (relname == "unstable")
		relname = "development";
	downloadfile(fileFilterNetDir(QStringList() <<
	"http://elive.icedslash.com/isos/"+relname+"/" <<
	"http://elive.leviathan-avc.com/"+relname+"/" <<
	"http://elive.jumbef.net/"+relname+"/" <<
	"http://elive.homogenica.com/"+relname+"/" <<
	"http://elive.evryanz.net/isos/"+relname+"/" <<
	"http://elive.7ds.pl/isos/"+relname+"/"
	, 524288000, 1048576000, QList<QRegExp>() <<
	QRegExp(".iso$", Qt::CaseInsensitive) <<
	QRegExp("elive\\S{0,}.iso$", Qt::CaseInsensitive)
	), isotmpf);
	extractiso(isotmpf);
}

if (nameDistro == "Fedora")
{
    QString minorarch = "";
	if (isarch64)
	{
		cpuarch = "x86_64";
        minorarch = "x86_64";
	}
	else
	{
		cpuarch = "i386";
        minorarch = "i686";
	}
	if (islivecd)
	{
        downloadfile(fileFilterNetDir(QStringList() <<
        "http://download.fedoraproject.org/pub/fedora/linux/releases/"+relname+"/Live/"+cpuarch+"/"
        , 524288000, 2233125376, QList<QRegExp>() <<
        QRegExp(".iso$", Qt::CaseInsensitive) <<
        QRegExp("Fedora", Qt::CaseInsensitive) <<
        QRegExp("Live", Qt::CaseInsensitive) <<
        QRegExp("LXDE", Qt::CaseInsensitive)
        ), isotmpf);
        extractiso(isotmpf);
	}
	else
	{
		if (relname == "rawhide")
		{
            downloadfile(QString("download.fedoraproject.org/pub/fedora/linux/development/%1/os/images/pxeboot/vmlinuz").arg(cpuarch), QString("%1ubnkern").arg(targetPath));
            downloadfile(QString("download.fedoraproject.org/pub/fedora/linux/development/%1/os/images/pxeboot/initrd.img").arg(cpuarch), QString("%1ubninit").arg(targetPath));
			postinstmsg = unetbootin::tr("\n*IMPORTANT* After rebooting, ignore any error messages and select back if prompted for a CD, then go to the main menu, select the 'Start Installation' option, choose 'Network' as the source, choose 'HTTP' as the protocol, enter 'download.fedora.redhat.com' when prompted for a server, and enter '/pub/fedora/linux/development/%1/os' when asked for the folder.").arg(cpuarch);
        }
		else
		{
            downloadfile(QString("http://download.fedoraproject.org/pub/fedora/linux/releases/%1/Fedora/%2/os/images/pxeboot/vmlinuz").arg(relname, cpuarch), QString("%1ubnkern").arg(targetPath));
            downloadfile(QString("http://download.fedoraproject.org/pub/fedora/linux/releases/%1/Fedora/%2/os/images/pxeboot/initrd.img").arg(relname, cpuarch), QString("%1ubninit").arg(targetPath));
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

if (nameDistro == "FreeNAS")
{
	if (isarch64)
	{
		cpuarch = "amd64";
	}
	else
	{
		cpuarch = "i386";
	}
	instIndvfl("memdisk", QString("%1ubnkern").arg(targetPath));
	if (islivecd)
	{
		downloadfile(QString("http://downloads.sourceforge.net/sourceforge/lubi/FreeNAS-%1-LiveCD-%2.img.gz").arg(cpuarch, relname), QString("%1ubninit").arg(targetPath));
	}
	else
	{
		downloadfile(QString("http://sourceforge.net/projects/freenas/files/stable/0.7/FreeNAS-%1-embedded-%2.img/download").arg(cpuarch, relname), QString("%1ubninit").arg(targetPath));
	}
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

if (nameDistro == "F-Secure Rescue CD")
{
	downloadfile("http://unetbootin.sourceforge.net/f-secure-rescue-cd.zip", isotmpf);
	extractiso(isotmpf);
}

if (nameDistro == "GAG")
{
	instIndvfl("memdisk", QString("%1ubnkern").arg(targetPath));
	downloadfile(QString("http://downloads.sourceforge.net/sourceforge/lubi/gag-%1.img.gz").arg(relname), QString("%1ubninit").arg(targetPath));
}

if (nameDistro == "GeeXboX")
{
        downloadfile(QString("http://www.geexbox.org/wp-content/plugins/download-monitor/download.php?id=geexbox-%1.iso").arg(relname), isotmpf);
	extractiso(isotmpf);
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
	extractiso(isotmpf);
}

if (nameDistro == "gNewSense")
{
	downloadfile(fileFilterNetDir(QStringList() << 
	"http://cdimage.gnewsense.org/" << 
	"http://heanet.archive.gnewsense.org/gnewsense/cdimage/" <<
	"http://mirror.softwarelibre.nl/gnewsense/cdimage/"
	, 61440000, 1048576000, QList<QRegExp>() << 
	QRegExp("gnewsense\\S{0,}livecd\\S{0,}.iso$", Qt::CaseInsensitive) << 
	QRegExp("livecd\\S{0,}.iso$", Qt::CaseInsensitive) <<
	QRegExp("gnewsense\\S{0,}.iso$", Qt::CaseInsensitive) <<
	QRegExp(".iso$", Qt::CaseInsensitive)
	), isotmpf);
	extractiso(isotmpf);
}

if (nameDistro == "Kaspersky Rescue Disk")
{
	if (relname == "8")
	{
		downloadfile(fileFilterNetDir(QStringList() <<
		"http://devbuilds.kaspersky-labs.com/devbuilds/RescueDisk/" <<
		"http://ftp.kaspersky.com/devbuilds/RescueDisk/" <<
		"ftp://ftp.kaspersky.com/devbuilds/RescueDisk/"
		, 9288000, 1048576000, QList<QRegExp>() <<
		QRegExp(".iso$", Qt::CaseInsensitive) <<
		QRegExp("k\\S{0,}.iso$", Qt::CaseInsensitive)
		), isotmpf);
	}
	else
	{
		downloadfile(fileFilterNetDir(QStringList() <<
		"http://devbuilds.kaspersky-labs.com/devbuilds/RescueDisk10/" <<
		"http://ftp.kaspersky.com/devbuilds/RescueDisk10/" <<
		"ftp://ftp.kaspersky.com/devbuilds/RescueDisk10/"
		, 9288000, 1048576000, QList<QRegExp>() <<
		QRegExp(".iso$", Qt::CaseInsensitive) <<
		QRegExp("k\\S{0,}.iso$", Qt::CaseInsensitive)
		), isotmpf);
	}
	extractiso(isotmpf);
}

if (nameDistro == "Gujin")
{
	instIndvfl("memdisk", QString("%1ubnkern").arg(targetPath));
	downloadfile(QString("http://downloads.sourceforge.net/sourceforge/lubi/gujin-%1.img.gz").arg(relname), QString("%1ubninit").arg(targetPath), 81920);
}

if (nameDistro == "LinuxConsole")
{
  downloadfile(QString("http://jukebox.linuxconsole.org/official/linuxconsole%1.iso").arg(QString(relname)), isotmpf);
  extractiso(isotmpf);
}

if (nameDistro == "Linux Mint")
{
    if (isarch64)
    {
        cpuarch = "64";
    }
	else
    {
        cpuarch = "32";
	}
	QList<QRegExp> mintregex = QList<QRegExp>() <<
		QRegExp(".iso$", Qt::CaseInsensitive) <<
		QRegExp("linuxmint", Qt::CaseInsensitive) <<
        QRegExp(cpuarch, Qt::CaseInsensitive) <<
        QRegExp("^((?!nocodecs).)*$", Qt::CaseInsensitive) <<
        QRegExp("cinnamon", Qt::CaseInsensitive);
	downloadfile(fileFilterNetDir(QStringList() <<
	QString("http://ftp.heanet.ie/pub/linuxmint.com/stable/%1/").arg(relname) <<
	QString("http://mira.sunsite.utk.edu/linuxmint/stable/%1/").arg(relname) <<
	QString("http://mirror.yellowfiber.net/linuxmint/stable/%1/").arg(relname) <<
	QString("http://mirror.optus.net/linuxmint/isos/stable/%1/").arg(relname) <<
	QString("http://mint.ez.by/linuxmint.com/stable/%1/").arg(relname) <<
	QString("http://linuxmint.secsup.org/stable/%1/").arg(relname) <<
	QString("http://ftp.jaist.ac.jp/pub/Linux/LinuxMint-ISO/stable/%1/").arg(relname) <<
	QString("http://ftp.riken.jp/pub/Linux/linuxmint/stable/%1/").arg(relname) <<
	QString("http://ftp.oss.tw/pub/Mint/LinuxMint-ISO/stable/%1/").arg(relname) <<
	QString("http://gd.tuwien.ac.at/linux/mint/isos/stable/%1/").arg(relname) <<
	QString("http://ftp.mgts.by/pub/linuxmint/isos/stable/%1/").arg(relname) <<
	QString("http://mirrors.cytanet.com.cy/linux/mint/stable/%1/").arg(relname) <<
	QString("http://ftp.klid.dk/ftp/linuxmint/stable/%1/").arg(relname) <<
	QString("http://ftp5.gwdg.de/pub/linux/debian/mint/stable/%1/").arg(relname) <<
	QString("http://mirror.netcologne.de/mint/stable/%1/").arg(relname) <<
	QString("http://ftp.cc.uoc.gr/mirrors/linux/linuxmint/stable/%1/").arg(relname) <<
	QString("http://ftp.heanet.ie/pub/linuxmint.com/stable/%1/").arg(relname) <<
	QString("http://ftp.akl.lt/Linux/Mint/stable/%1/").arg(relname) <<
	QString("http://cesium.di.uminho.pt/pub/linuxmint/stable/%1/").arg(relname) <<
	QString("http://ftp.df.lth.se/pub/linuxmint/stable/%1/").arg(relname) <<
	QString("http://mirror.switch.ch/ftp/mirror/linuxmint/stable/%1/").arg(relname) <<
	QString("http://mirror.sov.uk.goscomb.net/linuxmint.com/stable/%1/").arg(relname) <<
	QString("http://mirror.csclub.uwaterloo.ca/linuxmint/stable/%1/").arg(relname) <<
	QString("http://mirror.aarnet.edu.au/pub/linuxmint/stable/%1/").arg(relname) <<
	QString("http://mirror.waia.asn.au/pub/linux/linuxmint/linuxmint-isos/linuxmint.com/stable/%1/").arg(relname) <<
	QString("ftp://mirrors.secution.com/linuxmint.com/stable/%1/").arg(relname) <<
	QString("ftp://ftp.is.co.za/mirror/linuxmint.com/stable/%1/").arg(relname) <<
	QString("ftp://ftp.tpnet.pl/pub/linux/linuxmint/isos/stable/%1/").arg(relname) <<
	QString("ftp://mirror.unej.ac.id/pub/iso/linux-mint/stable/%1/").arg(relname)
	, 61440000, 1048576000, mintregex), isotmpf);
	extractiso(isotmpf);
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
		extractiso(isotmpf);
	}
	else
	{
		instIndvfl("memdisk", QString("%1ubnkern").arg(targetPath));
		downloadfile(QString("ftp://ftp.free.fr/mirrors/ftp.mandriva.com/MandrivaLinux/official/%1/%2/install/images/all.img").arg(relname, cpuarch), QString("%1ubninit").arg(targetPath));
	}
}

if (nameDistro == "MEPIS")
{
	if (isarch64)
	{
		cpuarch = "64";
	}
	else
	{
		cpuarch = "32";
	}
	if (relname == "antix")
	{
		downloadfile(fileFilterNetDir(QStringList() << 
		"ftp://ftp.ibiblio.org/pub/linux/distributions/mepis/released/antix/" << 
		"http://distro.ibiblio.org/pub/linux/distributions/mepis/released/antix/" <<
		"ftp://ftp-linux.cc.gatech.edu/pub/linux/distributions/mepis/released/antix/" << 
		"http://ftp.uwsg.indiana.edu/linux/mepis/released/antix/" <<
		"ftp://ftp.ussg.iu.edu/pub/linux/mepis/released/antix/" <<
		"http://mirror.cs.vt.edu/pub/MEPIS/antix/"
		, 61440000, 1048576000, QList<QRegExp>() << 
		QRegExp("antiX\\S{0,}.iso$", Qt::CaseInsensitive) << 
		QRegExp(".iso$", Qt::CaseInsensitive)
		), isotmpf);
	}
	else
	{
		downloadfile(fileFilterNetDir(QStringList() << 
		"ftp://ftp.ibiblio.org/pub/linux/distributions/mepis/released/" << 
		"http://distro.ibiblio.org/pub/linux/distributions/mepis/released/" <<
		"ftp://ftp-linux.cc.gatech.edu/pub/linux/distributions/mepis/released/" << 
		"http://ftp.uwsg.indiana.edu/linux/mepis/released/" <<
		"ftp://ftp.ussg.iu.edu/pub/linux/mepis/released/" <<
		"http://mirror.cs.vt.edu/pub/MEPIS/"
		, 61440000, 1048576000, QList<QRegExp>() << 
		QRegExp("MEPIS\\S{0,}.iso$", Qt::CaseInsensitive) << 
		QRegExp(".iso$", Qt::CaseInsensitive) << 
		QRegExp("\\S{0,}"+cpuarch+".iso$", Qt::CaseInsensitive)
		), isotmpf);
	}
	extractiso(isotmpf);
}

if (nameDistro == "NetbootCD")
{
	downloadfile("http://download.tuxfamily.org/netbootcd/NetbootCD-current.iso", isotmpf);
	extractiso(isotmpf);
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

if (nameDistro == "NimbleX")
{
	downloadfile("http://public.nimblex.net/Download/NimbleX-latest.iso", isotmpf);
	extractiso(isotmpf);
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
		extractiso(isotmpf);
	}
	else
	{
		if (relname == "factory")
		{
			downloadfile(QString("http://download.opensuse.org/factory/repo/oss/boot/%1/loader/linux").arg(cpuarch), QString("%1ubnkern").arg(targetPath));
			downloadfile(QString("http://download.opensuse.org/factory/repo/oss/boot/%1/loader/initrd").arg(cpuarch), QString("%1ubninit").arg(targetPath));
			postinstmsg = unetbootin::tr("\n*IMPORTANT* After rebooting, ignore any error messages and select back if prompted for a CD, then go to the main menu, select the 'Start Installation' option, choose 'Network' as the source, choose 'HTTP' as the protocol, enter 'download.opensuse.org' when prompted for a server, and enter '/factory/repo/oss' when asked for the folder.");
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
	extractiso(isotmpf);
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
        downloadfile("http://partedmagic.com/partedmagic-latest.iso", isotmpf);
		extractiso(isotmpf);
	}
}

if (nameDistro == "PCLinuxOS")
{
	QString deskn = relname.toLower().split(" ").first();
	QString vern = relname.toLower().split(" ").last();
	downloadfile(fileFilterNetDir(QStringList() <<
	"http://distro.ibiblio.org/pub/linux/distributions/texstar/pclinuxos/live-cd/english/preview/"
	, 10485760, 2147483647, QList<QRegExp>() <<
	QRegExp("^pclinuxos", Qt::CaseInsensitive) <<
	QRegExp(".iso$", Qt::CaseInsensitive) <<
	QRegExp(deskn, Qt::CaseInsensitive) <<
	QRegExp(vern, Qt::CaseInsensitive)
	), isotmpf);
	extractiso(isotmpf);
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
	extractiso(isotmpf);
}

if (nameDistro == "Sabayon Linux")
{
        if (isarch64)
        {
			cpuarch = "amd64";
        }
        else
        {
			cpuarch = "x86";
        }
        QString relnamenum = nameVersion;
        QString relnamepart = "";
        if (nameVersion.contains('-'))
        {
            relnamenum = nameVersion.section('-', 0, 0);
            relnamepart = nameVersion.section('-', -1, -1);
        }
        downloadfile(fileFilterNetDir(QStringList() <<
		"http://cross-lfs.sabayonlinux.org/iso/" <<
		"http://mirror.cs.vt.edu/pub/SabayonLinux/iso/" <<
		"http://mirror.umoss.org/sabayonlinux/iso/" <<
        "http://distro.ibiblio.org/pub/linux/distributions/sabayonlinux/" <<
		"http://ftp.nluug.nl/pub/os/Linux/distr/sabayonlinux/iso/" <<
		"http://mirror.aarnet.edu.au/pub/SabayonLinux/iso/" <<
		"http://na.mirror.garr.it/mirrors/sabayonlinux/iso/" <<
		"http://cesium.di.uminho.pt/pub/sabayon/iso/" <<
		"http://ftp.fsn.hu/pub/linux/distributions/sabayon/iso/"
        , 61440000, 2147483647, QList<QRegExp>() << // need to store as unsigned long long to use FTP (HTTP doesn't check size)
        QRegExp(".iso$", Qt::CaseInsensitive) <<
		QRegExp("^Sabayon\\S{0,}Linux\\S{0,}.iso$", Qt::CaseInsensitive) <<
		QRegExp("^Sabayon\\S{0,}Linux\\S{0,}"+cpuarch+"\\S{0,}.iso$", Qt::CaseInsensitive) <<
		QRegExp("^Sabayon\\S{0,}Linux\\S{0,}"+relnamenum+"\\S{0,}.iso$", Qt::CaseInsensitive) <<
		QRegExp("^Sabayon\\S{0,}Linux\\S{0,1}"+relnamenum+"\\S{0,}.iso$", Qt::CaseInsensitive) <<
		QRegExp("^Sabayon\\S{0,}Linux\\S{0,}"+relnamenum+"\\S{0,}"+relnamepart+"\\S{0,}.iso$", Qt::CaseInsensitive) <<
		QRegExp("^Sabayon\\S{0,}Linux\\S{0,}"+relnamenum+"\\S{0,}"+cpuarch+"\\S{0,}.iso$", Qt::CaseInsensitive) <<
		QRegExp("^Sabayon\\S{0,}Linux\\S{0,}"+relnamenum+"\\S{0,}"+relnamepart+"\\S{0,}"+cpuarch+"\\S{0,}.iso$", Qt::CaseInsensitive)
        ), isotmpf);
        extractiso(isotmpf);
}

if (nameDistro == "Salix")
{
	QString edition = "NONE"; // for old Salix
	if (relname.contains("_xfce"))
	{
		relname.remove("_xfce");
		edition = "xfce";
	}
	else if (relname.contains("_lxde"))
	{
		relname.remove("_lxde");
		edition = "lxde";
	}
	else if (relname.contains("_kde"))
	{
		relname.remove("_kde");
		edition = "kde";
	}
	else if (relname.contains("_fluxbox"))
	{
		relname.remove("_fluxbox");
		edition = "fluxbox";
	}
	else if (relname.contains("_ratpoison"))
	{
		relname.remove("_ratpoison");
		edition = "ratpoison";
	}
	// relname should only contains the version from there, i.e. 13.0.2a, 13.1.2, 13.37, 14.0, ... or latest
    QString version = relname;
	if (version == "latest") {
        QString archfolder;
        if (isarch64)
        {
            archfolder = "x86_64";
            cpuarch = "64";
        }
        else
        {
            archfolder = "i486";
            cpuarch = "";
        }
		if (islivecd) {
            downloadfile(QString("http://latestsalix.enialis.net/%1/salixlive%2-%3.iso").arg(archfolder).arg(cpuarch).arg(edition), isotmpf);
        }
        else
        {
            downloadfile(QString("http://latestsalix.enialis.net/%1/salix%2-%3.iso").arg(archfolder).arg(cpuarch).arg(edition), isotmpf);
        }
    } else {
        QStringList decomposedVersion = version.split(".");
        QString simpleversion = decomposedVersion.at(0); // simple version contains only the two first numbers, i.e. 13.1 not 13.1.2
        if (decomposedVersion.size() > 1) {
            simpleversion.append(".").append(decomposedVersion.at(1));
        }
		if (islivecd) {
            if (isarch64)
            {
                cpuarch = "64";
            }
            else
            {
                cpuarch = "32";
            }
            downloadfile(QString("http://downloads.sourceforge.net/salix/%1/salixlive-%4-%2-%3.iso").arg(simpleversion).arg(version).arg(cpuarch).arg(edition), isotmpf);
        } else {
            if (isarch64)
            {
                cpuarch = "64";
            }
            else
            {
                cpuarch = "";
            }
            if (edition == "NONE") { // Salix 13.0 does not have any edition, Xfce was the default.
                downloadfile(QString("http://downloads.sourceforge.net/salix/%1/salix%3-%2.iso").arg(simpleversion).arg(version).arg(cpuarch), isotmpf);
            } else {
                downloadfile(QString("http://downloads.sourceforge.net/salix/%1/salix%3-%4-%2.iso").arg(simpleversion).arg(version).arg(cpuarch).arg(edition), isotmpf);
            }
        }
    }
	extractiso(isotmpf);
}
if (nameDistro == "Slax")
{
	downloadfile("http://www.slax.org/get_slax.php?download=iso", isotmpf);
	extractiso(isotmpf);
}

if (nameDistro == "SliTaz")
{
	downloadfile(fileFilterNetDir(QStringList() <<
	QString("http://mirror.slitaz.org/iso/%1/").arg(relname)
	, 3072000, 1048576000, QList<QRegExp>() <<
	QRegExp("^slitaz", Qt::CaseInsensitive) <<
	QRegExp(".iso$", Qt::CaseInsensitive) <<
	QRegExp("^slitaz-\\S{1,}.iso$", Qt::CaseInsensitive)
	), isotmpf);
	extractiso(isotmpf);
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

if (nameDistro == "Super OS")
{
	downloadfile("http://hacktolive.org/download/os", isotmpf);
	extractiso(isotmpf);
}

if (nameDistro == "SystemRescueCD")
{
	downloadfile(fileFilterNetDir(QStringList() << 
	"ftp://ftp.mirrorservice.org/sites/download.sourceforge.net/pub/sourceforge/s/project/sy/systemrescuecd/" <<
	"http://www.mirrorservice.org/sites/download.sourceforge.net/pub/sourceforge/s/project/sy/systemrescuecd/"
	, 9440000, 1147483647, QList<QRegExp>() << 
	QRegExp("systemrescuecd\\S{0,}.iso$", Qt::CaseInsensitive) << 
	QRegExp("x86", Qt::CaseInsensitive) <<
	QRegExp("systemrescuecd-x86\\S{0,}.iso$", Qt::CaseInsensitive) << 
	QRegExp(".iso$", Qt::CaseInsensitive)
	), isotmpf);
	extractiso(isotmpf);
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
		extractiso(isotmpf);
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
			extractiso(isotmpf);
		}
	}
	ubuntunetinst
}

if (nameDistro == "Kubuntu")
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
		"http://cdimage.ubuntu.com/kubuntu/daily-live/current/"
		, 61440000, 1048576000, QList<QRegExp>() << 
		QRegExp(".iso$", Qt::CaseInsensitive) << 
		QRegExp("desktop\\S{0,}.iso$", Qt::CaseInsensitive) << 
		QRegExp("desktop-"+cpuarch+".iso$", Qt::CaseInsensitive)
		), isotmpf);
		extractiso(isotmpf);
	}
	else
	{
		if (islivecd)
		{
			downloadfile(fileFilterNetDir(QStringList() << 
			"http://cdimage.ubuntu.com/kubuntu/releases/"+relname+"/release"
			, 524288000, 1048576000, QList<QRegExp>() << 
			QRegExp(".iso$", Qt::CaseInsensitive) << 
			QRegExp(cpuarch+".iso$", Qt::CaseInsensitive) << 
			QRegExp("desktop-"+cpuarch+".iso$", Qt::CaseInsensitive) << 
			QRegExp("desktop-"+cpuarch+".iso$", Qt::CaseInsensitive) << 
			QRegExp("kubuntu\\S{0,}"+relname+"\\S{0,}desktop\\S{0,}"+cpuarch+"\\S{0,}.iso$", Qt::CaseInsensitive) << 
			QRegExp("kubuntu-"+relname+"\\S{0,}-desktop-"+cpuarch+".iso$", Qt::CaseInsensitive)
			), isotmpf);
			extractiso(isotmpf);
		}
	}
	ubuntunetinst
}

if (nameDistro == "Lubuntu")
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
		"http://cdimage.ubuntu.com/lubuntu/daily-live/current/"
		, 61440000, 1048576000, QList<QRegExp>() <<
		QRegExp(".iso$", Qt::CaseInsensitive) <<
		QRegExp("desktop\\S{0,}.iso$", Qt::CaseInsensitive) <<
		QRegExp("desktop-"+cpuarch+".iso$", Qt::CaseInsensitive)
		), isotmpf);
		extractiso(isotmpf);
	}
	else
	{
		if (islivecd)
		{
			downloadfile(fileFilterNetDir(QStringList() <<
			"http://cdimage.ubuntu.com/lubuntu/releases/"+relname+"/release"
			, 524288000, 1048576000, QList<QRegExp>() <<
			QRegExp(".iso$", Qt::CaseInsensitive) <<
			QRegExp(cpuarch+".iso$", Qt::CaseInsensitive) <<
			QRegExp("desktop-"+cpuarch+".iso$", Qt::CaseInsensitive) <<
			QRegExp("desktop-"+cpuarch+".iso$", Qt::CaseInsensitive) <<
			QRegExp("lubuntu\\S{0,}"+relname+"\\S{0,}desktop\\S{0,}"+cpuarch+"\\S{0,}.iso$", Qt::CaseInsensitive) <<
			QRegExp("lubuntu-"+relname+"\\S{0,}-desktop-"+cpuarch+".iso$", Qt::CaseInsensitive)
			), isotmpf);
			extractiso(isotmpf);
		}
	}
	ubuntunetinst
}

if (nameDistro == "xPUD")
{
	if (relname == "stable")
	{
		downloadfile("http://xpud.org/xpud-latest-iso.php", isotmpf);
	}
	else if (relname == "unstable")
	{
		downloadfile("http://xpud.org/xpud-latest-snapshot.php", isotmpf);
	}
	/*
	initrdLoc = "";
	kernelLoc = "/boot/xpud";
	initrdOpts = "";
	initrdLine = "";
	slinitrdLine = "";
	*/
	extractiso(isotmpf);
}

if (nameDistro == "Xubuntu")
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
		"http://cdimage.ubuntu.com/xubuntu/daily-live/current/"
		, 61440000, 1048576000, QList<QRegExp>() << 
		QRegExp(".iso$", Qt::CaseInsensitive) << 
		QRegExp("desktop\\S{0,}.iso$", Qt::CaseInsensitive) << 
		QRegExp("desktop-"+cpuarch+".iso$", Qt::CaseInsensitive)
		), isotmpf);
		extractiso(isotmpf);
	}
	else
	{
		if (islivecd)
		{
			downloadfile(fileFilterNetDir(QStringList() << 
			"http://cdimage.ubuntu.com/xubuntu/releases/"+relname+"/release"
			, 524288000, 1048576000, QList<QRegExp>() << 
			QRegExp(".iso$", Qt::CaseInsensitive) << 
			QRegExp(cpuarch+".iso$", Qt::CaseInsensitive) << 
			QRegExp("desktop-"+cpuarch+".iso$", Qt::CaseInsensitive) << 
			QRegExp("desktop-"+cpuarch+".iso$", Qt::CaseInsensitive) << 
			QRegExp("xubuntu\\S{0,}"+relname+"\\S{0,}desktop\\S{0,}"+cpuarch+"\\S{0,}.iso$", Qt::CaseInsensitive) << 
			QRegExp("xubuntu-"+relname+"\\S{0,}-desktop-"+cpuarch+".iso$", Qt::CaseInsensitive)
			), isotmpf);
			extractiso(isotmpf);
		}
	}
	ubuntunetinst
}

if (nameDistro == "Zenwalk")
{
	downloadfile(QString("ftp://zenwalk.mirrors.tds.net/pub/linux/zenlive/zenwalk-live-%1.iso").arg(relname), isotmpf);
	extractiso(isotmpf);
}

#endif
