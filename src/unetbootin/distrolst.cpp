/*
distrolst.cpp from UNetbootin <http://unetbootin.sourceforge.net>
Copyright (C) Geza Kovacs <geza0kovacs@gmail.com>

This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 2 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License at <http://www.gnu.org/licenses/> for more details.
*/

QString isotmpf = randtmpfile::getrandfilename(ubntmpf, "iso");
QString cpuarch;
QString relname = nameVersion.toLower();

#ifdef AUTOSUPERGRUBDISK

if (nameDistro == "Auto Super Grub Disk")
{
	instIndvfl("memdisk", QString("%1ubnkern").arg(targetPath));
	instIndvfl("asgd.img", QString("%1ubninit").arg(targetPath));
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
	downloadfile(QString("ftp://ftp.archlinux.org/iso/%3/%2/Archlinux-%2-%1.ftp.iso").arg(relname, cpuarch, QString(relname).remove(QRegExp("-\\d{0,}$"))), isotmpf);
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
	postinstmsg = QObject::tr("\n*IMPORTANT* After rebooting, ignore any error messages and select back if prompted for a CD, then go to the main menu, select the 'Start Installation' option, choose 'Network' as the source, choose 'HTTP' as the protocol, enter 'mirrors.kernel.org' when prompted for a server, and enter '/centos/%1/os/%2' when asked for the folder.").arg(nameVersion, cpuarch);
	kernelOpts = "splash=silent showopts";
}

if (nameDistro == "Damn Small Linux")
{
	downloadfile(QString("ftp://ibiblio.org/pub/Linux/distributions/damnsmall/current/dsl-%1-initrd.iso").arg(relname), isotmpf);
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
			postinstmsg = QObject::tr("\n*IMPORTANT* After rebooting, ignore any error messages and select back if prompted for a CD, then go to the main menu, select the 'Start Installation' option, choose 'Network' as the source, choose 'HTTP' as the protocol, enter 'download.fedora.redhat.com' when prompted for a server, and enter '/pub/fedora/linux/development/%1/os' when asked for the folder.").arg(cpuarch);
		}
		else
		{
			downloadfile(QString("http://download.fedora.redhat.com/pub/fedora/linux/releases/%1/Fedora/%2/os/images/pxeboot/vmlinuz").arg(relname, cpuarch), QString("%1ubnkern").arg(targetPath));
			downloadfile(QString("http://download.fedora.redhat.com/pub/fedora/linux/releases/%1/Fedora/%2/os/images/pxeboot/initrd.img").arg(relname, cpuarch), QString("%1ubninit").arg(targetPath));
			postinstmsg = QObject::tr("\n*IMPORTANT* After rebooting, ignore any error messages and select back if prompted for a CD, then go to the main menu, select the 'Start Installation' option, choose 'Network' as the source, choose 'HTTP' as the protocol, enter 'download.fedora.redhat.com' when prompted for a server, and enter '/pub/fedora/linux/releases/%1/Fedora/%2/os' when asked for the folder.").arg(relname, cpuarch);
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
	QString pageurl = QString("http://www10.frugalware.org/pub/linux/frugalware/frugalware-%1/boot/").arg(relname);
	QStringList pagecontents = downloadpagecontents(pageurl).replace("<", "\n").replace(">", "\n").split("\n");
	QString kernpartname, initpartname;
	if (!pagecontents.filter(QRegExp("^\\s{0,}vmlinuz.{0,}"+cpuarch)).isEmpty())
		kernpartname = pagecontents.filter(QRegExp("^\\s{0,}vmlinuz.{0,}"+cpuarch)).at(0);
	if (!pagecontents.filter(QRegExp("^\\s{0,}initrd.{0,}"+cpuarch+".img.gz")).isEmpty())
		initpartname = pagecontents.filter(QRegExp("^\\s{0,}initrd.{0,}"+cpuarch+".img.gz")).at(0);
	downloadfile(QString("%1%2").arg(pageurl, kernpartname), QString("%1ubnkern").arg(targetPath));
	downloadfile(QString("%1%2").arg(pageurl, initpartname), QString("%1ubninit").arg(targetPath));
	kernelOpts = "load_ramdisk=1 prompt_ramdisk=0 ramdisk_size=100000 rw root=/dev/ram quiet vga=791";
}

if (nameDistro == "GAG")
{
	instIndvfl("memdisk", QString("%1ubnkern").arg(targetPath));
	downloadfile(QString("http://downloads.sourceforge.net/sourceforge/lubi/gag-%1.img.gz").arg(relname), QString("%1ubninit").arg(targetPath));
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
		downloadfile(QString("http://exo.enarel.eu/mirror/partedmagic/pmagic-pxe-%1.zip").arg(relname), isotmpf);
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
	downloadfile(QString("ftp://ibiblio.org/pub/linux/distributions/puppylinux/puppy-%1.iso").arg(relname), isotmpf);
	extractiso(isotmpf, targetPath);
}

if (nameDistro == "Slax")
{
	downloadfile("http://www.slax.org/get_slax.php?download=iso", isotmpf);
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
	if (islivecd)
	{
		downloadfile(QString("http://releases.ubuntu.com/%1/ubuntu-%1-desktop-%2.iso").arg(relname, cpuarch), isotmpf);
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

if (nameDistro == "Zenwalk")
{
	downloadfile(QString("ftp://zenwalk.mirrors.tds.net/pub/linux/zenlive/zenwalk-live-%1.iso").arg(relname), isotmpf);
	extractiso(isotmpf, targetPath);
}

#endif
