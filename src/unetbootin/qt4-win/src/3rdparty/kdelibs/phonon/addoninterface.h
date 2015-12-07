/*  This file is part of the KDE project
    Copyright (C) 2007 Matthias Kretz <kretz@kde.org>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License version 2 as published by the Free Software Foundation.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.

*/

#ifndef PHONON_ADDONINTERFACE_H
#define PHONON_ADDONINTERFACE_H

#include "phonon/phononnamespace.h"

#include <QtCore/QList>
#include <QtCore/QVariant>

QT_BEGIN_HEADER
QT_BEGIN_NAMESPACE

namespace Phonon
{
/** \class AddonInterface addoninterface.h Phonon/AddonInterface
 * \short Interface for Menu, Chapter, Angle and Title/Track control.
 *
 * \ingroup Backend
 * \author Matthias Kretz <kretz@kde.org>
 */
class AddonInterface
{
    public:
        virtual ~AddonInterface() {}

        enum Interface {
            NavigationInterface = 1,
            ChapterInterface    = 2,
            AngleInterface      = 3,
            TitleInterface      = 4
        };

        enum NavigationCommand {
            Menu1Button
        };
        enum ChapterCommand {
            availableChapters,
            chapter,
            setChapter
        };
        enum AngleCommand {
            availableAngles,
            angle,
            setAngle
        };
        enum TitleCommand {
            availableTitles,
            title,
            setTitle,
            autoplayTitles,
            setAutoplayTitles
        };

        virtual bool hasInterface(Interface iface) const = 0;

        virtual QVariant interfaceCall(Interface iface, int command,
                const QList<QVariant> &arguments = QList<QVariant>()) = 0;
};

} // namespace Phonon

Q_DECLARE_INTERFACE(Phonon::AddonInterface, "AddonInterface0.2.phonon.kde.org")

QT_END_NAMESPACE
QT_END_HEADER

#endif // PHONON_ADDONINTERFACE_H
