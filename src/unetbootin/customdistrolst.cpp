/*
customdistrolst.cpp from UNetbootin <http://unetbootin.sourceforge.net>
Copyright (C) 2007-2008 Geza Kovacs <geza0kovacs@gmail.com>

This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 2 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License at <http://www.gnu.org/licenses/> for more details.
*/

if (customver::tr("iscustomver-no") == "iscustomver-yes")
{
	if (isarch64)
	{
		if (!customver::tr("amd64").isEmpty())
			cpuarch = customver::tr("amd64");
		else
			cpuarch = "amd64";
	}
	else
	{
		if (!customver::tr("i386").isEmpty())
			cpuarch = customver::tr("i386");
		else
			cpuarch = "i386";
	}
	if (nameDistro == customver::tr("Distro Name 1"))
	{
		if (customver::tr("distro1format-iso").contains("img"))
		{
			instIndvfl("memdisk", QString("%1ubnkern").arg(targetPath));
			downloadfile(customver::tr("http://distro1site.org/distro-release-%1/distro-architecture%2.iso").arg(relname).arg(cpuarch), QString("%1ubninit").arg(targetPath));
		}
		else
		{
			downloadfile(customver::tr("http://distro1site.org/distro-release-%1/distro-architecture%2.iso").arg(relname).arg(cpuarch), isotmpf);
			extractiso(isotmpf);
		}
	}
	if (nameDistro == customver::tr("Distro Name 2"))
	{
		if (customver::tr("distro2format-iso").contains("img"))
		{
			instIndvfl("memdisk", QString("%1ubnkern").arg(targetPath));
			downloadfile(customver::tr("http://distro2site.org/distro-release-%1/distro-architecture%2.iso").arg(relname).arg(cpuarch), QString("%1ubninit").arg(targetPath));
		}
		else
		{
			downloadfile(customver::tr("http://distro2site.org/distro-release-%1/distro-architecture%2.iso").arg(relname).arg(cpuarch), isotmpf);
			extractiso(isotmpf);
		}
	}
	if (nameDistro == customver::tr("Distro Name 3"))
	{
		if (customver::tr("distro3format-iso").contains("img"))
		{
			instIndvfl("memdisk", QString("%1ubnkern").arg(targetPath));
			downloadfile(customver::tr("http://distro3site.org/distro-release-%1/distro-architecture%2.iso").arg(relname).arg(cpuarch), QString("%1ubninit").arg(targetPath));
		}
		else
		{
			downloadfile(customver::tr("http://distro3site.org/distro-release-%1/distro-architecture%2.iso").arg(relname).arg(cpuarch), isotmpf);
			extractiso(isotmpf);
		}
	}
	if (nameDistro == customver::tr("Distro Name 4"))
	{
		if (customver::tr("distro4format-iso").contains("img"))
		{
			instIndvfl("memdisk", QString("%1ubnkern").arg(targetPath));
			downloadfile(customver::tr("http://distro4site.org/distro-release-%1/distro-architecture%2.iso").arg(relname).arg(cpuarch), QString("%1ubninit").arg(targetPath));
		}
		else
		{
			downloadfile(customver::tr("http://distro4site.org/distro-release-%1/distro-architecture%2.iso").arg(relname).arg(cpuarch), isotmpf);
			extractiso(isotmpf);
		}
	}
	if (nameDistro == customver::tr("Distro Name 5"))
	{
		if (customver::tr("distro5format-iso").contains("img"))
		{
			instIndvfl("memdisk", QString("%1ubnkern").arg(targetPath));
			downloadfile(customver::tr("http://distro5site.org/distro-release-%1/distro-architecture%2.iso").arg(relname).arg(cpuarch), QString("%1ubninit").arg(targetPath));
		}
		else
		{
			downloadfile(customver::tr("http://distro5site.org/distro-release-%1/distro-architecture%2.iso").arg(relname).arg(cpuarch), isotmpf);
			extractiso(isotmpf);
		}
	}
	if (nameDistro == customver::tr("Distro Name 6"))
	{
		if (customver::tr("distro6format-iso").contains("img"))
		{
			instIndvfl("memdisk", QString("%1ubnkern").arg(targetPath));
			downloadfile(customver::tr("http://distro6site.org/distro-release-%1/distro-architecture%2.iso").arg(relname).arg(cpuarch), QString("%1ubninit").arg(targetPath));
		}
		else
		{
			downloadfile(customver::tr("http://distro6site.org/distro-release-%1/distro-architecture%2.iso").arg(relname).arg(cpuarch), isotmpf);
			extractiso(isotmpf);
		}
	}
	if (nameDistro == customver::tr("Distro Name 7"))
	{
		if (customver::tr("distro7format-iso").contains("img"))
		{
			instIndvfl("memdisk", QString("%1ubnkern").arg(targetPath));
			downloadfile(customver::tr("http://distro7site.org/distro-release-%1/distro-architecture%2.iso").arg(relname).arg(cpuarch), QString("%1ubninit").arg(targetPath));
		}
		else
		{
			downloadfile(customver::tr("http://distro7site.org/distro-release-%1/distro-architecture%2.iso").arg(relname).arg(cpuarch), isotmpf);
			extractiso(isotmpf);
		}
	}
	if (nameDistro == customver::tr("Distro Name 8"))
	{
		if (customver::tr("distro8format-iso").contains("img"))
		{
			instIndvfl("memdisk", QString("%1ubnkern").arg(targetPath));
			downloadfile(customver::tr("http://distro8site.org/distro-release-%1/distro-architecture%2.iso").arg(relname).arg(cpuarch), QString("%1ubninit").arg(targetPath));
		}
		else
		{
			downloadfile(customver::tr("http://distro8site.org/distro-release-%1/distro-architecture%2.iso").arg(relname).arg(cpuarch), isotmpf);
			extractiso(isotmpf);
		}
	}
	if (nameDistro == customver::tr("Distro Name 9"))
	{
		if (customver::tr("distro9format-iso").contains("img"))
		{
			instIndvfl("memdisk", QString("%1ubnkern").arg(targetPath));
			downloadfile(customver::tr("http://distro9site.org/distro-release-%1/distro-architecture%2.iso").arg(relname).arg(cpuarch), QString("%1ubninit").arg(targetPath));
		}
		else
		{
			downloadfile(customver::tr("http://distro9site.org/distro-release-%1/distro-architecture%2.iso").arg(relname).arg(cpuarch), isotmpf);
			extractiso(isotmpf);
		}
	}
}
else
{
	#include "distrolst.cpp"
}
