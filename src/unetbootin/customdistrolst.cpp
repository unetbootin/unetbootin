/*
customdistrolst.cpp from UNetbootin <http://unetbootin.sourceforge.net>
Copyright (C) Geza Kovacs <geza0kovacs@gmail.com>

This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 2 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License at <http://www.gnu.org/licenses/> for more details.
*/

if (customver::tr("iscustomver-no") == "iscustomver-yes")
{
	if (isarch64)
	{
		cpuarch = customver::tr("amd64");
	}
	else
	{
		cpuarch = customver::tr("i386");
	}
	if (nameDistro == customver::tr("Distro Name 1"))
	{
		downloadfile(customver::tr("http://distro1site.org/distro-release-%1/distro-architecture%2.iso").arg(relname).arg(cpuarch), isotmpf);
	}
	if (nameDistro == customver::tr("Distro Name 2"))
	{
		downloadfile(customver::tr("http://distro2site.org/distro-release-%1/distro-architecture%2.iso").arg(relname).arg(cpuarch), isotmpf);
	}
	if (nameDistro == customver::tr("Distro Name 3"))
	{
		downloadfile(customver::tr("http://distro3site.org/distro-release-%1/distro-architecture%2.iso").arg(relname).arg(cpuarch), isotmpf);
	}
	if (nameDistro == customver::tr("Distro Name 4"))
	{
		downloadfile(customver::tr("http://distro4site.org/distro-release-%1/distro-architecture%2.iso").arg(relname).arg(cpuarch), isotmpf);
	}
	if (nameDistro == customver::tr("Distro Name 5"))
	{
		downloadfile(customver::tr("http://distro5site.org/distro-release-%1/distro-architecture%2.iso").arg(relname).arg(cpuarch), isotmpf);
	}
	if (nameDistro == customver::tr("Distro Name 6"))
	{
		downloadfile(customver::tr("http://distro6site.org/distro-release-%1/distro-architecture%2.iso").arg(relname).arg(cpuarch), isotmpf);
	}
	if (nameDistro == customver::tr("Distro Name 7"))
	{
		downloadfile(customver::tr("http://distro7site.org/distro-release-%1/distro-architecture%2.iso").arg(relname).arg(cpuarch), isotmpf);
	}
	if (nameDistro == customver::tr("Distro Name 8"))
	{
		downloadfile(customver::tr("http://distro8site.org/distro-release-%1/distro-architecture%2.iso").arg(relname).arg(cpuarch), isotmpf);
	}
	if (nameDistro == customver::tr("Distro Name 9"))
	{
		downloadfile(customver::tr("http://distro9site.org/distro-release-%1/distro-architecture%2.iso").arg(relname).arg(cpuarch), isotmpf);
	}
	extractiso(isotmpf, targetPath);
}
else
{
	#include "distrolst.cpp"
}
