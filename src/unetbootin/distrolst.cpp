if (nameDistro == "CentOS 5")
{
	kernelOpts = "splash=silent showopts";
	downloadfile("http://isoredirect.centos.org/centos/5/os/i386/images/pxeboot/vmlinuz", QString("%1ubnkern").arg(targetPath));
	downloadfile("http://isoredirect.centos.org/centos/5/os/i386/images/pxeboot/initrd.img", QString("%1ubninit").arg(targetPath));
	postinstmsg = QObject::tr("\n*IMPORTANT* After rebooting, ignore any error messages and select back if prompted for a CD, then go to the main menu, select the 'Start Installation' option, choose 'Network' as the source, choose 'HTTP' as the protocol, enter 'mirrors.kernel.org' when prompted for a server, and enter '/centos/5/os/i386' when asked for the folder.");
}

if (nameDistro == "CentOS 5 x64")
{
	kernelOpts = "splash=silent showopts";
	downloadfile("http://isoredirect.centos.org/centos/5/os/x86_64/images/pxeboot/vmlinuz", QString("%1ubnkern").arg(targetPath));
	downloadfile("http://isoredirect.centos.org/centos/5/os/x86_64/images/pxeboot/initrd.img", QString("%1ubninit").arg(targetPath));
	postinstmsg = QObject::tr("\n*IMPORTANT* After rebooting, ignore any error messages and select back if prompted for a CD, then go to the main menu, select the 'Start Installation' option, choose 'Network' as the source, choose 'HTTP' as the protocol, enter 'mirrors.kernel.org' when prompted for a server, and enter '/centos/5/os/x86_64' when asked for the folder.");
}

if (nameDistro == "CentOS 4")
{
	kernelOpts = "splash=silent showopts";
	downloadfile("http://isoredirect.centos.org/centos/4/os/i386/images/pxeboot/vmlinuz", QString("%1ubnkern").arg(targetPath));
	downloadfile("http://isoredirect.centos.org/centos/4/os/i386/images/pxeboot/initrd.img", QString("%1ubninit").arg(targetPath));
	postinstmsg = QObject::tr("\n*IMPORTANT* After rebooting, ignore any error messages and select back if prompted for a CD, then go to the main menu, select the 'Start Installation' option, choose 'Network' as the source, choose 'HTTP' as the protocol, enter 'mirrors.kernel.org' when prompted for a server, and enter '/centos/4/os/i386' when asked for the folder.");
}

if (nameDistro == "CentOS 4 x64")
{
	kernelOpts = "splash=silent showopts";
	downloadfile("http://isoredirect.centos.org/centos/4/os/x86_64/images/pxeboot/vmlinuz", QString("%1ubnkern").arg(targetPath));
	downloadfile("http://isoredirect.centos.org/centos/4/os/x86_64/images/pxeboot/initrd.img", QString("%1ubninit").arg(targetPath));
	postinstmsg = QObject::tr("\n*IMPORTANT* After rebooting, ignore any error messages and select back if prompted for a CD, then go to the main menu, select the 'Start Installation' option, choose 'Network' as the source, choose 'HTTP' as the protocol, enter 'mirrors.kernel.org' when prompted for a server, and enter '/centos/4/os/x86_64' when asked for the folder.");
}

if (nameDistro == "Debian Stable")
{
	kernelOpts = "video=vesa:ywrap,mtrr vga=788 installgui";
	downloadfile("http://ftp.debian.org/debian/dists/stable/main/installer-i386/current/images/netboot/gtk/debian-installer/i386/linux", QString("%1ubnkern").arg(targetPath));
	downloadfile("http://ftp.debian.org/debian/dists/stable/main/installer-i386/current/images/netboot/gtk/debian-installer/i386/initrd.gz", QString("%1ubninit").arg(targetPath));
}

if (nameDistro == "Debian Stable x64")
{
	kernelOpts = "video=vesa:ywrap,mtrr vga=788 installgui";
	downloadfile("http://ftp.debian.org/debian/dists/stable/main/installer-amd64/current/images/netboot/gtk/debian-installer/amd64/linux", QString("%1ubnkern").arg(targetPath));
	downloadfile("http://ftp.debian.org/debian/dists/stable/main/installer-amd64/current/images/netboot/gtk/debian-installer/amd64/initrd.gz", QString("%1ubninit").arg(targetPath));
}

if (nameDistro == "Debian Testing")
{
	kernelOpts = "video=vesa:ywrap,mtrr vga=788 installgui";
	downloadfile("http://ftp.debian.org/debian/dists/testing/main/installer-i386/current/images/netboot/gtk/debian-installer/i386/linux", QString("%1ubnkern").arg(targetPath));
	downloadfile("http://ftp.debian.org/debian/dists/testing/main/installer-i386/current/images/netboot/gtk/debian-installer/i386/initrd.gz", QString("%1ubninit").arg(targetPath));
}

if (nameDistro == "Debian Testing x64")
{
	kernelOpts = "video=vesa:ywrap,mtrr vga=788 installgui";
	downloadfile("http://ftp.debian.org/debian/dists/testing/main/installer-amd64/current/images/netboot/gtk/debian-installer/amd64/linux", QString("%1ubnkern").arg(targetPath));
	downloadfile("http://ftp.debian.org/debian/dists/testing/main/installer-amd64/current/images/netboot/gtk/debian-installer/amd64/initrd.gz", QString("%1ubninit").arg(targetPath));
}

if (nameDistro == "Debian Unstable")
{
	kernelOpts = "video=vesa:ywrap,mtrr vga=788 installgui";
	downloadfile("http://ftp.debian.org/debian/dists/unstable/main/installer-i386/current/images/netboot/gtk/debian-installer/i386/linux", QString("%1ubnkern").arg(targetPath));
	downloadfile("http://ftp.debian.org/debian/dists/unstable/main/installer-i386/current/images/netboot/gtk/debian-installer/i386/initrd.gz", QString("%1ubninit").arg(targetPath));
}

if (nameDistro == "Debian Unstable x64")
{
	kernelOpts = "video=vesa:ywrap,mtrr vga=788 installgui";
	downloadfile("http://ftp.debian.org/debian/dists/unstable/main/installer-amd64/current/images/netboot/gtk/debian-installer/amd64/linux", QString("%1ubnkern").arg(targetPath));
	downloadfile("http://ftp.debian.org/debian/dists/unstable/main/installer-amd64/current/images/netboot/gtk/debian-installer/amd64/initrd.gz", QString("%1ubninit").arg(targetPath));
}

if (nameDistro == "Fedora Rawhide")
{
	kernelOpts = "splash=silent showopts";
	downloadfile("http://download.fedora.redhat.com/pub/fedora/linux/development/i386/os/images/pxeboot/vmlinuz", QString("%1ubnkern").arg(targetPath));
	downloadfile("http://download.fedora.redhat.com/pub/fedora/linux/development/i386/os/images/pxeboot/initrd.img", QString("%1ubninit").arg(targetPath));
	postinstmsg = QObject::tr("\n*IMPORTANT* After rebooting, ignore any error messages and select back if prompted for a CD, then go to the main menu, select the 'Start Installation' option, choose 'Network' as the source, choose 'HTTP' as the protocol, enter 'download.fedora.redhat.com' when prompted for a server, and enter '/pub/fedora/linux/development/i386/os' when asked for the folder.");
}

if (nameDistro == "Fedora Rawhide x64")
{
	kernelOpts = "splash=silent showopts";
	downloadfile("http://download.fedora.redhat.com/pub/fedora/linux/development/x86_64/os/images/pxeboot/vmlinuz", QString("%1ubnkern").arg(targetPath));
	downloadfile("http://download.fedora.redhat.com/pub/fedora/linux/development/x86_64/os/images/pxeboot/initrd.img", QString("%1ubninit").arg(targetPath));
	postinstmsg = QObject::tr("\n*IMPORTANT* After rebooting, ignore any error messages and select back if prompted for a CD, then go to the main menu, select the 'Start Installation' option, choose 'Network' as the source, choose 'HTTP' as the protocol, enter 'download.fedora.redhat.com' when prompted for a server, and enter '/pub/fedora/linux/development/x86_64/os' when asked for the folder.");
}

if (nameDistro == "Fedora 9 Alpha")
{
	kernelOpts = "splash=silent showopts";
	downloadfile("http://download.fedora.redhat.com/pub/fedora/linux/releases/test/9-Alpha/Fedora/i386/os/images/pxeboot/vmlinuz", QString("%1ubnkern").arg(targetPath));
	downloadfile("http://download.fedora.redhat.com/pub/fedora/linux/releases/test/9-Alpha/Fedora/i386/os/images/pxeboot/initrd.img", QString("%1ubninit").arg(targetPath));
	postinstmsg = QObject::tr("\n*IMPORTANT* After rebooting, ignore any error messages and select back if prompted for a CD, then go to the main menu, select the 'Start Installation' option, choose 'Network' as the source, choose 'HTTP' as the protocol, enter 'download.fedora.redhat.com' when prompted for a server, and enter '/pub/fedora/linux/releases/test/9-Alpha/Fedora/i386/os' when asked for the folder.");
}

if (nameDistro == "Fedora 9 Alpha x64")
{
	kernelOpts = "splash=silent showopts";
	downloadfile("http://download.fedora.redhat.com/pub/fedora/linux/releases/test/9-Alpha/Fedora/x86_64/os/images/pxeboot/vmlinuz", QString("%1ubnkern").arg(targetPath));
	downloadfile("http://download.fedora.redhat.com/pub/fedora/linux/releases/test/9-Alpha/Fedora/x86_64/os/images/pxeboot/initrd.img", QString("%1ubninit").arg(targetPath));
	postinstmsg = QObject::tr("\n*IMPORTANT* After rebooting, ignore any error messages and select back if prompted for a CD, then go to the main menu, select the 'Start Installation' option, choose 'Network' as the source, choose 'HTTP' as the protocol, enter 'download.fedora.redhat.com' when prompted for a server, and enter '/pub/fedora/linux/releases/test/9-Alpha/Fedora/x86_64/os' when asked for the folder.");
}

if (nameDistro == "Fedora 8")
{
	kernelOpts = "splash=silent showopts";
	downloadfile("http://download.fedora.redhat.com/pub/fedora/linux/releases/8/Fedora/i386/os/images/pxeboot/vmlinuz", QString("%1ubnkern").arg(targetPath));
	downloadfile("http://download.fedora.redhat.com/pub/fedora/linux/releases/8/Fedora/i386/os/images/pxeboot/initrd.img", QString("%1ubninit").arg(targetPath));
	postinstmsg = QObject::tr("\n*IMPORTANT* After rebooting, ignore any error messages and select back if prompted for a CD, then go to the main menu, select the 'Start Installation' option, choose 'Network' as the source, choose 'HTTP' as the protocol, enter 'download.fedora.redhat.com' when prompted for a server, and enter '/pub/fedora/linux/releases/8/Fedora/i386/os' when asked for the folder.");
}

if (nameDistro == "Fedora 8 x64")
{
	kernelOpts = "splash=silent showopts";
	downloadfile("http://download.fedora.redhat.com/pub/fedora/linux/releases/8/Fedora/x86_64/os/images/pxeboot/vmlinuz", QString("%1ubnkern").arg(targetPath));
	downloadfile("http://download.fedora.redhat.com/pub/fedora/linux/releases/8/Fedora/x86_64/os/images/pxeboot/initrd.img", QString("%1ubninit").arg(targetPath));
	postinstmsg = QObject::tr("\n*IMPORTANT* After rebooting, ignore any error messages and select back if prompted for a CD, then go to the main menu, select the 'Start Installation' option, choose 'Network' as the source, choose 'HTTP' as the protocol, enter 'download.fedora.redhat.com' when prompted for a server, and enter '/pub/fedora/linux/releases/8/Fedora/x86_64/os' when asked for the folder.");
}

if (nameDistro == "Fedora 7")
{
	kernelOpts = "splash=silent showopts";
	downloadfile("http://download.fedora.redhat.com/pub/fedora/linux/releases/7/Fedora/i386/os/images/pxeboot/vmlinuz", QString("%1ubnkern").arg(targetPath));
	downloadfile("http://download.fedora.redhat.com/pub/fedora/linux/releases/7/Fedora/i386/os/images/pxeboot/initrd.img", QString("%1ubninit").arg(targetPath));
	postinstmsg = QObject::tr("\n*IMPORTANT* After rebooting, ignore any error messages and select back if prompted for a CD, then go to the main menu, select the 'Start Installation' option, choose 'Network' as the source, choose 'HTTP' as the protocol, enter 'download.fedora.redhat.com' when prompted for a server, and enter '/pub/fedora/linux/releases/7/Fedora/i386/os' when asked for the folder.");
}

if (nameDistro == "Fedora 7 x64")
{
	kernelOpts = "splash=silent showopts";
	downloadfile("http://download.fedora.redhat.com/pub/fedora/linux/releases/7/Fedora/x86_64/os/images/pxeboot/vmlinuz", QString("%1ubnkern").arg(targetPath));
	downloadfile("http://download.fedora.redhat.com/pub/fedora/linux/releases/7/Fedora/x86_64/os/images/pxeboot/initrd.img", QString("%1ubninit").arg(targetPath));
	postinstmsg = QObject::tr("\n*IMPORTANT* After rebooting, ignore any error messages and select back if prompted for a CD, then go to the main menu, select the 'Start Installation' option, choose 'Network' as the source, choose 'HTTP' as the protocol, enter 'download.fedora.redhat.com' when prompted for a server, and enter '/pub/fedora/linux/releases/7/Fedora/x86_64/os' when asked for the folder.");
}

if (nameDistro == "FreeBSD 7.0")
{
	instIndvfl(QString("%1ubnkern").arg(targetPath), memdisk);
	downloadfile("http://superb-east.dl.sourceforge.net/sourceforge/lubi/freebsd-7.0.img.gz", QString("%1ubninit").arg(targetPath));
}

if (nameDistro == "FreeBSD 7.0 x64")
{
	instIndvfl(QString("%1ubnkern").arg(targetPath), memdisk);
	downloadfile("http://superb-east.dl.sourceforge.net/sourceforge/lubi/freebsd-7.0-x64.img.gz", QString("%1ubninit").arg(targetPath));
}

if (nameDistro == "FreeBSD 6.3")
{
	instIndvfl(QString("%1ubnkern").arg(targetPath), memdisk);
	downloadfile("http://superb-east.dl.sourceforge.net/sourceforge/lubi/freebsd-6.3.img.gz", QString("%1ubninit").arg(targetPath));
}

if (nameDistro == "FreeBSD 6.3 x64")
{
	instIndvfl(QString("%1ubnkern").arg(targetPath), memdisk);
	downloadfile("http://superb-east.dl.sourceforge.net/sourceforge/lubi/freebsd-6.3-x64.img.gz", QString("%1ubninit").arg(targetPath));
}

if (nameDistro == "Frugalware Stable")
{
	QString pageurl = "http://www10.frugalware.org/pub/linux/frugalware/frugalware-stable/boot/";
	QString pagecontents = downloadpagecontents(pageurl);
	QString kernpartname = pagecontents.replace("<", "\n").replace(">", "\n").split("\n").filter(QRegExp("^\\s{0,}vmlinuz.{0,}i686")).at(0);
	QString initpartname = pagecontents.replace("<", "\n").replace(">", "\n").split("\n").filter(QRegExp("^\\s{0,}initrd.{0,}i686.img.gz")).at(0);	
	kernelOpts = "load_ramdisk=1 prompt_ramdisk=0 ramdisk_size=41864 rw root=/dev/ram quiet vga=791";
	downloadfile(QString("%1%2").arg(pageurl, kernpartname), QString("%1ubnkern").arg(targetPath));
	downloadfile(QString("%1%2").arg(pageurl, initpartname), QString("%1ubninit").arg(targetPath));
}

if (nameDistro == "Frugalware Stable x64")
{
	QString pageurl = "http://www10.frugalware.org/pub/linux/frugalware/frugalware-stable/boot/";
	QString pagecontents = downloadpagecontents(pageurl);
	QString kernpartname = pagecontents.replace("<", "\n").replace(">", "\n").split("\n").filter(QRegExp("^\\s{0,}vmlinuz.{0,}x86_64")).at(0);
	QString initpartname = pagecontents.replace("<", "\n").replace(">", "\n").split("\n").filter(QRegExp("^\\s{0,}initrd.{0,}x86_64.img.gz")).at(0);	
	kernelOpts = "load_ramdisk=1 prompt_ramdisk=0 ramdisk_size=41864 rw root=/dev/ram quiet vga=791";
	downloadfile(QString("%1%2").arg(pageurl, kernpartname), QString("%1ubnkern").arg(targetPath));
	downloadfile(QString("%1%2").arg(pageurl, initpartname), QString("%1ubninit").arg(targetPath));
}

if (nameDistro == "Frugalware Testing")
{
	QString pageurl = "http://www10.frugalware.org/pub/linux/frugalware/frugalware-testing/boot/";
	QString pagecontents = downloadpagecontents(pageurl);
	QString kernpartname = pagecontents.replace("<", "\n").replace(">", "\n").split("\n").filter(QRegExp("^\\s{0,}vmlinuz.{0,}i686")).at(0);
	QString initpartname = pagecontents.replace("<", "\n").replace(">", "\n").split("\n").filter(QRegExp("^\\s{0,}initrd.{0,}i686.img.gz")).at(0);	
	kernelOpts = "load_ramdisk=1 prompt_ramdisk=0 ramdisk_size=41864 rw root=/dev/ram quiet vga=791";
	downloadfile(QString("%1%2").arg(pageurl, kernpartname), QString("%1ubnkern").arg(targetPath));
	downloadfile(QString("%1%2").arg(pageurl, initpartname), QString("%1ubninit").arg(targetPath));
}

if (nameDistro == "Frugalware Testing x64")
{
	QString pageurl = "http://www10.frugalware.org/pub/linux/frugalware/frugalware-testing/boot/";
	QString pagecontents = downloadpagecontents(pageurl);
	QString kernpartname = pagecontents.replace("<", "\n").replace(">", "\n").split("\n").filter(QRegExp("^\\s{0,}vmlinuz.{0,}x86_64")).at(0);
	QString initpartname = pagecontents.replace("<", "\n").replace(">", "\n").split("\n").filter(QRegExp("^\\s{0,}initrd.{0,}x86_64.img.gz")).at(0);	
	kernelOpts = "load_ramdisk=1 prompt_ramdisk=0 ramdisk_size=41864 rw root=/dev/ram quiet vga=791";
	downloadfile(QString("%1%2").arg(pageurl, kernpartname), QString("%1ubnkern").arg(targetPath));
	downloadfile(QString("%1%2").arg(pageurl, initpartname), QString("%1ubninit").arg(targetPath));
}

if (nameDistro == "Frugalware Current")
{
	QString pageurl = "http://www10.frugalware.org/pub/linux/frugalware/frugalware-current/boot/";
	QString pagecontents = downloadpagecontents(pageurl);
	QString kernpartname = pagecontents.replace("<", "\n").replace(">", "\n").split("\n").filter(QRegExp("^\\s{0,}vmlinuz.{0,}i686")).at(0);
	QString initpartname = pagecontents.replace("<", "\n").replace(">", "\n").split("\n").filter(QRegExp("^\\s{0,}initrd.{0,}i686.img.gz")).at(0);	
	kernelOpts = "load_ramdisk=1 prompt_ramdisk=0 ramdisk_size=41864 rw root=/dev/ram quiet vga=791";
	downloadfile(QString("%1%2").arg(pageurl, kernpartname), QString("%1ubnkern").arg(targetPath));
	downloadfile(QString("%1%2").arg(pageurl, initpartname), QString("%1ubninit").arg(targetPath));
}

if (nameDistro == "Frugalware Current x64")
{
	QString pageurl = "http://www10.frugalware.org/pub/linux/frugalware/frugalware-current/boot/";
	QString pagecontents = downloadpagecontents(pageurl);
	QString kernpartname = pagecontents.replace("<", "\n").replace(">", "\n").split("\n").filter(QRegExp("^\\s{0,}vmlinuz.{0,}x86_64")).at(0);
	QString initpartname = pagecontents.replace("<", "\n").replace(">", "\n").split("\n").filter(QRegExp("^\\s{0,}initrd.{0,}x86_64.img.gz")).at(0);	
	kernelOpts = "load_ramdisk=1 prompt_ramdisk=0 ramdisk_size=41864 rw root=/dev/ram quiet vga=791";
	downloadfile(QString("%1%2").arg(pageurl, kernpartname), QString("%1ubnkern").arg(targetPath));
	downloadfile(QString("%1%2").arg(pageurl, initpartname), QString("%1ubninit").arg(targetPath));
}

if (nameDistro == "NetBSD 4.0")
{
	kernelParam = "--type=netbsd";
	initrdLine = "";
	initrdOpts = "";
	initrdLoc = "";
	downloadfile("http://ftp.netbsd.org/pub/NetBSD/NetBSD-4.0/i386/binary/kernel/netbsd-INSTALL.gz", QString("%1ubnkern").arg(targetPath));
}

if (nameDistro == "openSUSE Factory")
{
	kernelOpts = "splash=silent showopts";
	downloadfile("http://download.opensuse.org/distribution/SL-OSS-factory/inst-source/boot/i386/loader/linux", QString("%1ubnkern").arg(targetPath));
	downloadfile("http://download.opensuse.org/distribution/SL-OSS-factory/inst-source/boot/i386/loader/initrd", QString("%1ubninit").arg(targetPath));
	postinstmsg = QObject::tr("\n*IMPORTANT* After rebooting, ignore any error messages and select back if prompted for a CD, then go to the main menu, select the 'Start Installation' option, choose 'Network' as the source, choose 'HTTP' as the protocol, enter 'download.opensuse.org' when prompted for a server, and enter '/distribution/SL-OSS-factory/inst-source' when asked for the folder.");
}

if (nameDistro == "openSUSE Factory x64")
{
	kernelOpts = "splash=silent showopts";
	downloadfile("http://download.opensuse.org/distribution/SL-OSS-factory/inst-source/boot/x86_64/loader/linux", QString("%1ubnkern").arg(targetPath));
	downloadfile("http://download.opensuse.org/distribution/SL-OSS-factory/inst-source/boot/x86_64/loader/initrd", QString("%1ubninit").arg(targetPath));
	postinstmsg = QObject::tr("\n*IMPORTANT* After rebooting, ignore any error messages and select back if prompted for a CD, then go to the main menu, select the 'Start Installation' option, choose 'Network' as the source, choose 'HTTP' as the protocol, enter 'download.opensuse.org' when prompted for a server, and enter '/distribution/SL-OSS-factory/inst-source' when asked for the folder.");
}

if (nameDistro == "openSUSE 10.3")
{
	kernelOpts = "splash=silent showopts";
	downloadfile("http://download.opensuse.org/distribution/10.3/repo/oss/boot/i386/loader/linux", QString("%1ubnkern").arg(targetPath));
	downloadfile("http://download.opensuse.org/distribution/10.3/repo/oss/boot/i386/loader/initrd", QString("%1ubninit").arg(targetPath));
	postinstmsg = QObject::tr("\n*IMPORTANT* After rebooting, ignore any error messages and select back if prompted for a CD, then go to the main menu, select the 'Start Installation' option, choose 'Network' as the source, choose 'HTTP' as the protocol, enter 'download.opensuse.org' when prompted for a server, and enter '/distribution/10.3/repo/oss' when asked for the folder.");
}

if (nameDistro == "openSUSE 10.3 x64")
{
	kernelOpts = "splash=silent showopts";
	downloadfile("http://download.opensuse.org/distribution/10.3/repo/oss/boot/x86_64/loader/linux", QString("%1ubnkern").arg(targetPath));
	downloadfile("http://download.opensuse.org/distribution/10.3/repo/oss/boot/x86_64/loader/initrd", QString("%1ubninit").arg(targetPath));
	postinstmsg = QObject::tr("\n*IMPORTANT* After rebooting, ignore any error messages and select back if prompted for a CD, then go to the main menu, select the 'Start Installation' option, choose 'Network' as the source, choose 'HTTP' as the protocol, enter 'download.opensuse.org' when prompted for a server, and enter '/distribution/10.3/repo/oss' when asked for the folder.");
}

if (nameDistro == "openSUSE 10.2")
{
	kernelOpts = "splash=silent showopts";
	downloadfile("http://download.opensuse.org/distribution/10.2/repo/oss/boot/i386/loader/linux", QString("%1ubnkern").arg(targetPath));
	downloadfile("http://download.opensuse.org/distribution/10.2/repo/oss/boot/i386/loader/initrd", QString("%1ubninit").arg(targetPath));
	postinstmsg = QObject::tr("\n*IMPORTANT* After rebooting, ignore any error messages and select back if prompted for a CD, then go to the main menu, select the 'Start Installation' option, choose 'Network' as the source, choose 'HTTP' as the protocol, enter 'download.opensuse.org' when prompted for a server, and enter '/distribution/10.2/repo/oss' when asked for the folder.");
}

if (nameDistro == "openSUSE 10.2 x64")
{
	kernelOpts = "splash=silent showopts";
	downloadfile("http://download.opensuse.org/distribution/10.2/repo/oss/boot/x86_64/loader/linux", QString("%1ubnkern").arg(targetPath));
	downloadfile("http://download.opensuse.org/distribution/10.2/repo/oss/boot/x86_64/loader/initrd", QString("%1ubninit").arg(targetPath));
	postinstmsg = QObject::tr("\n*IMPORTANT* After rebooting, ignore any error messages and select back if prompted for a CD, then go to the main menu, select the 'Start Installation' option, choose 'Network' as the source, choose 'HTTP' as the protocol, enter 'download.opensuse.org' when prompted for a server, and enter '/distribution/10.2/repo/oss' when asked for the folder.");
}

if (nameDistro == "Parted Magic 2.1")
{
	kernelOpts = "noapic root=/dev/ram0 init=/linuxrc ramdisk_size=200000 keymap=us liveusb vga=791 quiet toram";
	downloadfile("http://superb-east.dl.sourceforge.net/sourceforge/lubi/partedmagic-2.1-kernel", QString("%1ubnkern").arg(targetPath));
	downloadfile("http://superb-east.dl.sourceforge.net/sourceforge/lubi/partedmagic-2.1-initrd", QString("%1ubninit").arg(targetPath));
}

if (nameDistro == "Ubuntu 8.04")
{
	kernelOpts = "vga=normal";
	downloadfile("http://archive.ubuntu.com/ubuntu/dists/hardy/main/installer-i386/current/images/netboot/ubuntu-installer/i386/linux", QString("%1ubnkern").arg(targetPath));
	downloadfile("http://archive.ubuntu.com/ubuntu/dists/gutsy/main/installer-i386/current/images/netboot/ubuntu-installer/i386/initrd.gz", QString("%1ubninit").arg(targetPath));
}

if (nameDistro == "Ubuntu 8.04 x64")
{
	kernelOpts = "vga=normal";
	downloadfile("http://archive.ubuntu.com/ubuntu/dists/hardy/main/installer-amd64/current/images/netboot/ubuntu-installer/amd64/linux", QString("%1ubnkern").arg(targetPath));
	downloadfile("http://archive.ubuntu.com/ubuntu/dists/hardy/main/installer-amd64/current/images/netboot/ubuntu-installer/amd64/initrd.gz", QString("%1ubninit").arg(targetPath));
}

if (nameDistro == "Ubuntu 7.10")
{
	kernelOpts = "vga=normal";
	downloadfile("http://archive.ubuntu.com/ubuntu/dists/gutsy/main/installer-i386/current/images/netboot/ubuntu-installer/i386/linux", QString("%1ubnkern").arg(targetPath));
	downloadfile("http://archive.ubuntu.com/ubuntu/dists/gutsy/main/installer-i386/current/images/netboot/ubuntu-installer/i386/initrd.gz", QString("%1ubninit").arg(targetPath));
}

if (nameDistro == "Ubuntu 7.10 x64")
{
	kernelOpts = "vga=normal";
	downloadfile("http://archive.ubuntu.com/ubuntu/dists/gutsy/main/installer-amd64/current/images/netboot/ubuntu-installer/amd64/linux", QString("%1ubnkern").arg(targetPath));
	downloadfile("http://archive.ubuntu.com/ubuntu/dists/gutsy/main/installer-amd64/current/images/netboot/ubuntu-installer/amd64/initrd.gz", QString("%1ubninit").arg(targetPath));
}

if (nameDistro == "Ubuntu 7.04")
{
	kernelOpts = "vga=normal";
	downloadfile("http://archive.ubuntu.com/ubuntu/dists/feisty/main/installer-i386/current/images/netboot/ubuntu-installer/i386/linux", QString("%1ubnkern").arg(targetPath));
	downloadfile("http://archive.ubuntu.com/ubuntu/dists/feisty/main/installer-i386/current/images/netboot/ubuntu-installer/i386/initrd.gz", QString("%1ubninit").arg(targetPath));
}

if (nameDistro == "Ubuntu 7.04 x64")
{
	kernelOpts = "vga=normal";
	downloadfile("http://archive.ubuntu.com/ubuntu/dists/feisty/main/installer-amd64/current/images/netboot/ubuntu-installer/amd64/linux", QString("%1ubnkern").arg(targetPath));
	downloadfile("http://archive.ubuntu.com/ubuntu/dists/feisty/main/installer-amd64/current/images/netboot/ubuntu-installer/amd64/initrd.gz", QString("%1ubninit").arg(targetPath));
}

if (nameDistro == "Ubuntu 6.10")
{
	kernelOpts = "vga=normal";
	downloadfile("http://archive.ubuntu.com/ubuntu/dists/edgy/main/installer-i386/current/images/netboot/ubuntu-installer/i386/linux", QString("%1ubnkern").arg(targetPath));
	downloadfile("http://archive.ubuntu.com/ubuntu/dists/edgy/main/installer-i386/current/images/netboot/ubuntu-installer/i386/initrd.gz", QString("%1ubninit").arg(targetPath));
}

if (nameDistro == "Ubuntu 6.10 x64")
{
	kernelOpts = "vga=normal";
	downloadfile("http://archive.ubuntu.com/ubuntu/dists/edgy/main/installer-amd64/current/images/netboot/ubuntu-installer/amd64/linux", QString("%1ubnkern").arg(targetPath));
	downloadfile("http://archive.ubuntu.com/ubuntu/dists/edgy/main/installer-amd64/current/images/netboot/ubuntu-installer/amd64/initrd.gz", QString("%1ubninit").arg(targetPath));
}

if (nameDistro == "Ubuntu 6.06")
{
	kernelOpts = "vga=normal";
	downloadfile("http://archive.ubuntu.com/ubuntu/dists/dapper/main/installer-i386/current/images/netboot/ubuntu-installer/i386/linux", QString("%1ubnkern").arg(targetPath));
	downloadfile("http://archive.ubuntu.com/ubuntu/dists/dapper/main/installer-i386/current/images/netboot/ubuntu-installer/i386/initrd.gz", QString("%1ubninit").arg(targetPath));
}

if (nameDistro == "Ubuntu 6.06 x64")
{
	kernelOpts = "vga=normal";
	downloadfile("http://archive.ubuntu.com/ubuntu/dists/dapper/main/installer-amd64/current/images/netboot/ubuntu-installer/amd64/linux", QString("%1ubnkern").arg(targetPath));
	downloadfile("http://archive.ubuntu.com/ubuntu/dists/dapper/main/installer-amd64/current/images/netboot/ubuntu-installer/amd64/initrd.gz", QString("%1ubninit").arg(targetPath));
}
