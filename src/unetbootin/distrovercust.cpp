/*
distrovercust.cpp from UNetbootin <http://unetbootin.sourceforge.net>
Copyright (C) 2007-2008 Geza Kovacs <geza0kovacs@gmail.com>

This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 2 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License at <http://www.gnu.org/licenses/> for more details.
*/

#ifdef AUTOSUPERGRUBDISK
distroselect->addItem("Super Grub Disk", (QStringList() << "Latest" <<
unetbootin::tr("<b>Homepage:</b> <a href=\"http://www.supergrubdisk.org\">http://www.supergrubdisk.org</a><br/>"
		"<b>Description:</b> Super Grub Disk is a bootloader which can perform a variety of MBR and bootloader recovery tasks.<br/>"
		"<b>Install Notes:</b> SGD simply boots and runs; no installation is required to use it.") <<
"Latest"));
QFile asgdDescF;
if (QFile::exists(QString(":/asgd_%1.htm").arg(appNlang)))
{
	asgdDescF.setFileName(QString(":/asgd_%1.htm").arg(appNlang));
}
else
{
	asgdDescF.setFileName(":/asgd_en.htm");
}
asgdDescF.open(QIODevice::ReadOnly | QIODevice::Text);
QTextStream asgdDescS(&asgdDescF);
distroselect->addItem("Auto Super Grub Disk", (QStringList() << "1.0" <<
asgdDescS.readAll() <<
"1.0"));
#endif
#ifdef EEEPCLOS
distroselect->addItem("EeePCLinuxOS", (QStringList() << "pre2008_Live" <<
unetbootin::tr("<img src=\":/eeepclos.png\" /><br/>"
	"<b>Homepage:</b> <a href=\"http://www.eeepclinuxos.com/\">http://www.eeepclinuxos.com</a><br/>"
	"<b>Description:</b> EeePCLinuxOS is a user-friendly PCLinuxOS based distribution for the EeePC.<br/>"
	"<b>Install Notes:</b> Make sure install media is empty and formatted before proceeding with install.") <<
"pre2008_Live"));
#endif
#ifdef EEEUBUNTU
distroselect->addItem("Ubuntu Eee", (QStringList() << "8.04" <<
unetbootin::tr("<img src=\":/eeeubuntu.png\" style=\"float:left;\" /><br/>"
	"<b>Homepage:</b> <a href=\"http://www.ubuntu-eee.com/\">http://www.ubuntu-eee.com</a><br/>"
	"<b>Description:</b> Ubuntu Eee is not only Ubuntu optimized for the Asus Eee PC. It's an operating system, using the Netbook Remix interface, which favors the best software available instead of open source alternatives (ie. Skype instead of Ekiga).<br/>"
	"<b>Install Notes:</b> Make sure install media is empty and formatted before proceeding with install.") <<
//	tr("<img src=\":/eeeubuntu.png\" /><br/>"
//		"<b>Homepage:</b> <a href=\"http://www.ubuntu-eee.com/\">http://www.ubuntu-eee.com</a><br/>"
//		"<b>Description:</b> Ubuntu Eee is a user-friendly Ubuntu based distribution for the EeePC.<br/>"
//		"<b>Install Notes:</b> Make sure install media is empty and formatted before proceeding with install.") <<
"8.04"));
#endif
#ifdef ELIVE
distroselect->addItem("Elive", (QStringList() << "Stable_Live" <<
unetbootin::tr("<img src=\":/elive.png\" /><br/>"
	"<b>Homepage:</b> <a href=\"http://www.elivecd.org/\">http://www.elivecd.org</a><br/>"
	"<b>Description:</b> Elive is a Debian-based distribution featuring the Enlightenment window manager.<br/>"
	"<b>Install Notes:</b> The Live version allows for booting in Live mode, from which the installer can optionally be launched.") <<
	"Stable_Live" << "Unstable_Live"));
#endif
#ifdef KIWILINUX
distroselect->addItem("Kiwi Linux", (QStringList() << "9.04" <<
unetbootin::tr("<img src=\":/kiwi_logo_ro.png\" /><br/>"
	"<b>Homepage:</b> <a href=\"http://www.kiwilinux.org/\">http://www.kiwilinux.org</a><br/>"
	"<b>Description:</b> Kiwi Linux is an Ubuntu derivative primarily made for Romanian, Hungarian and English speaking users.<br/>"
	"<b>Install Notes:</b> Make sure install media is empty and formatted before proceeding with install.") <<
	"9.04"));
#endif
#ifdef GNEWSENSE
distroselect->addItem("gNewSense", (QStringList() << "deltah-2.1" <<
unetbootin::tr("<img src=\":/gnewsense.png\" /><br/>"
	"<b>Homepage:</b> <a href=\"http://www.gnewsense.org/\">http://www.gnewsense.org</a><br/>"
	"<b>Description:</b> gNewSense is a high-quality GNU/Linux distribution that extends and improves Ubuntu to create a completely free operating system without any binary blobs or package trees that contain proprietary software.<br/>"
	"<b>Install Notes:</b> Make sure install media is empty and formatted before proceeding with install.") <<
"deltah-2.1"));
#endif
#ifdef NIMBLEX
distroselect->addItem("NimbleX", (QStringList() << "Latest_Live" <<
unetbootin::tr("<img src=\":/nimblex.png\" /><br/>"
	"<b>Homepage:</b> <a href=\"http://www.nimblex.net/\">http://www.nimblex.net</a><br/>"
	"<b>Description:</b> NimbleX is a small, versatile Slackware-based distribution. It is built using the linux-live scripts, and features the KDE desktop. It can be booted from CD or flash memory (USB pens or MP3 players), and can easily be customized and extended.<br/>"
	"<b>Install Notes:</b> NimbleX boots in Live mode.") <<
"Latest_Live"));
#endif
#ifdef SLITAZ
distroselect->addItem("SliTaz", (QStringList() << "Stable" <<
unetbootin::tr("<img src=\":/slitaz.png\" /><br/>"
	"<b>Homepage:</b> <a href=\"http://www.slitaz.org/en/\">http://www.slitaz.org/en</a><br/>"
	"<b>Description:</b> SliTaz is a lightweight, desktop-oriented micro distribution.<br/>"
	"<b>Install Notes:</b> The Live version loads the entire system into RAM and boots from memory, so installation is not required but optional. This installer is based on <a href=\"http://unetbootin.sourceforge.net/\">UNetbootin</a>.") <<
"Stable" << "Cooking" << "Webboot"));
#endif
#ifdef XPUD
distroselect->addItem("xPUD", (QStringList() << "Stable_Live" <<
unetbootin::tr("<img src=\":/xpud.png\" /><br/>"
	"<b>Homepage:</b> <a href=\"http://www.xpud.org/\">http://www.xpud.org</a><br/>"
	"<b>Description:</b> xPUD is a lightweight distribution featuring a simple kiosk-like interface with a web browser and media player.<br/>"
	"<b>Install Notes:</b> The Live version loads the entire system into RAM and boots from memory.") <<
"Stable_Live" << "Unstable_Live"));
#endif
