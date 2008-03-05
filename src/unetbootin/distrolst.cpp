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

if (nameDistro == "Fedora 9 Alpha")
{
	kernelOpts = "splash=silent showopts";
	downloadfile("http://download.fedora.redhat.com/pub/fedora/linux/releases/test/9-Alpha/Fedora/i386/os/images/pxeboot/vmlinuz", QString("%1ubnkern").arg(targetPath));
	downloadfile("http://download.fedora.redhat.com/pub/fedora/linux/releases/test/9-Alpha/Fedora/i386/os/images/pxeboot/initrd.img", QString("%1ubninit").arg(targetPath));
}

if (nameDistro == "Fedora 9 Alpha x64")
{
	kernelOpts = "splash=silent showopts";
	downloadfile("http://download.fedora.redhat.com/pub/fedora/linux/releases/test/9-Alpha/Fedora/x86_64/os/images/pxeboot/vmlinuz", QString("%1ubnkern").arg(targetPath));
	downloadfile("http://download.fedora.redhat.com/pub/fedora/linux/releases/test/9-Alpha/Fedora/x86_64/os/images/pxeboot/initrd.img", QString("%1ubninit").arg(targetPath));
}

if (nameDistro == "Fedora 8")
{
	kernelOpts = "splash=silent showopts";
	downloadfile("http://download.fedora.redhat.com/pub/fedora/linux/releases/8/Fedora/i386/os/images/pxeboot/vmlinuz", QString("%1ubnkern").arg(targetPath));
	downloadfile("http://download.fedora.redhat.com/pub/fedora/linux/releases/8/Fedora/i386/os/images/pxeboot/initrd.img", QString("%1ubninit").arg(targetPath));
}

if (nameDistro == "Fedora 8 x64")
{
	kernelOpts = "splash=silent showopts";
	downloadfile("http://download.fedora.redhat.com/pub/fedora/linux/releases/8/Fedora/x86_64/os/images/pxeboot/vmlinuz", QString("%1ubnkern").arg(targetPath));
	downloadfile("http://download.fedora.redhat.com/pub/fedora/linux/releases/8/Fedora/x86_64/os/images/pxeboot/initrd.img", QString("%1ubninit").arg(targetPath));
}

if (nameDistro == "Fedora 7")
{
	kernelOpts = "splash=silent showopts";
	downloadfile("http://download.fedora.redhat.com/pub/fedora/linux/releases/7/Fedora/i386/os/images/pxeboot/vmlinuz", QString("%1ubnkern").arg(targetPath));
	downloadfile("http://download.fedora.redhat.com/pub/fedora/linux/releases/7/Fedora/i386/os/images/pxeboot/initrd.img", QString("%1ubninit").arg(targetPath));
}

if (nameDistro == "Fedora 7 x64")
{
	kernelOpts = "splash=silent showopts";
	downloadfile("http://download.fedora.redhat.com/pub/fedora/linux/releases/7/Fedora/x86_64/os/images/pxeboot/vmlinuz", QString("%1ubnkern").arg(targetPath));
	downloadfile("http://download.fedora.redhat.com/pub/fedora/linux/releases/7/Fedora/x86_64/os/images/pxeboot/initrd.img", QString("%1ubninit").arg(targetPath));
}

if (nameDistro == "openSUSE Factory")
{
	kernelOpts = "splash=silent showopts";
	downloadfile("http://download.opensuse.org/distribution/SL-OSS-factory/inst-source/boot/i386/loader/linux", QString("%1ubnkern").arg(targetPath));
	downloadfile("http://download.opensuse.org/distribution/SL-OSS-factory/inst-source/boot/i386/loader/initrd", QString("%1ubninit").arg(targetPath));
}

if (nameDistro == "openSUSE Factory x64")
{
	kernelOpts = "splash=silent showopts";
	downloadfile("http://download.opensuse.org/distribution/SL-OSS-factory/inst-source/boot/x86_64/loader/linux", QString("%1ubnkern").arg(targetPath));
	downloadfile("http://download.opensuse.org/distribution/SL-OSS-factory/inst-source/boot/x86_64/loader/initrd", QString("%1ubninit").arg(targetPath));
}

if (nameDistro == "openSUSE 10.3")
{
	kernelOpts = "splash=silent showopts";
	downloadfile("http://download.opensuse.org/distribution/10.3/repo/oss/boot/i386/loader/linux", QString("%1ubnkern").arg(targetPath));
	downloadfile("http://download.opensuse.org/distribution/10.3/repo/oss/boot/i386/loader/initrd", QString("%1ubninit").arg(targetPath));
}

if (nameDistro == "openSUSE 10.3 x64")
{
	kernelOpts = "splash=silent showopts";
	downloadfile("http://download.opensuse.org/distribution/10.3/repo/oss/boot/x86_64/loader/linux", QString("%1ubnkern").arg(targetPath));
	downloadfile("http://download.opensuse.org/distribution/10.3/repo/oss/boot/x86_64/loader/initrd", QString("%1ubninit").arg(targetPath));
}

if (nameDistro == "openSUSE 10.2")
{
	kernelOpts = "splash=silent showopts";
	downloadfile("http://download.opensuse.org/distribution/10.2/repo/oss/boot/i386/loader/linux", QString("%1ubnkern").arg(targetPath));
	downloadfile("http://download.opensuse.org/distribution/10.2/repo/oss/boot/i386/loader/initrd", QString("%1ubninit").arg(targetPath));
}

if (nameDistro == "openSUSE 10.2 x64")
{
	kernelOpts = "splash=silent showopts";
	downloadfile("http://download.opensuse.org/distribution/10.2/repo/oss/boot/x86_64/loader/linux", QString("%1ubnkern").arg(targetPath));
	downloadfile("http://download.opensuse.org/distribution/10.2/repo/oss/boot/x86_64/loader/initrd", QString("%1ubninit").arg(targetPath));
}

if (nameDistro == "CentOS 5")
{
	kernelOpts = "splash=silent showopts";
	downloadfile("http://isoredirect.centos.org/centos/5/os/i386/images/pxeboot/vmlinuz", QString("%1ubnkern").arg(targetPath));
	downloadfile("http://isoredirect.centos.org/centos/5/os/i386/images/pxeboot/initrd.img", QString("%1ubninit").arg(targetPath));
}

if (nameDistro == "CentOS 5 x64")
{
	kernelOpts = "splash=silent showopts";
	downloadfile("http://isoredirect.centos.org/centos/5/os/x86_64/images/pxeboot/vmlinuz", QString("%1ubnkern").arg(targetPath));
	downloadfile("http://isoredirect.centos.org/centos/5/os/x86_64/images/pxeboot/initrd.img", QString("%1ubninit").arg(targetPath));
}

if (nameDistro == "CentOS 4")
{
	kernelOpts = "splash=silent showopts";
	downloadfile("http://isoredirect.centos.org/centos/4/os/i386/images/pxeboot/vmlinuz", QString("%1ubnkern").arg(targetPath));
	downloadfile("http://isoredirect.centos.org/centos/4/os/i386/images/pxeboot/initrd.img", QString("%1ubninit").arg(targetPath));
}

if (nameDistro == "CentOS 4 x64")
{
	kernelOpts = "splash=silent showopts";
	downloadfile("http://isoredirect.centos.org/centos/4/os/x86_64/images/pxeboot/vmlinuz", QString("%1ubnkern").arg(targetPath));
	downloadfile("http://isoredirect.centos.org/centos/4/os/x86_64/images/pxeboot/initrd.img", QString("%1ubninit").arg(targetPath));
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

if (nameDistro == "NetBSD 4.0")
{
	kernelParam = "--type=netbsd";
	initrdLine = "";
	initrdOpts = "";
	initrdLoc = "";
	downloadfile("http://ftp.netbsd.org/pub/NetBSD/NetBSD-4.0/i386/binary/kernel/netbsd-INSTALL.gz", QString("%1ubnkern").arg(targetPath));
}
