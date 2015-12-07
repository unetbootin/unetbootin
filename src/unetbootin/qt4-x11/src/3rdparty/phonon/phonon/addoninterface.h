/*  This file is part of the KDE project
    Copyright (C) 2007-2008 Matthias Kretz <kretz@kde.org>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) version 3, or any
    later version accepted by the membership of KDE e.V. (or its
    successor approved by the membership of KDE e.V.), Trolltech ASA 
    (or its successors, if any) and the KDE Free Qt Foundation, which shall
    act as a proxy defined in Section 6 of version 3 of the license.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public 
    License along with this library.  If not, see <http://www.gnu.org/licenses/>.

*/

#ifndef PHONON_ADDONINTERFACE_H
#define PHONON_ADDONINTERFACE_H

#include "phononnamespace.h"

#include <QtCore/QList>
#include <QtCore/QVariant>

QT_BEGIN_HEADER
QT_BEGIN_NAMESPACE

#ifndef QT_NO_PHONON_MEDIACONTROLLER

namespace Phonon
{
/** \class AddonInterface addoninterface.h Phonon/AddonInterface
 * \short Interface for Menu, Chapter, Angle and Title/Track control.
 *
 * \author Matthias Kretz <kretz@kde.org>
 */
class AddonInterface
{
    public:
        virtual ~AddonInterface() {}

        enum Interface {
            NavigationInterface   = 1,
            ChapterInterface      = 2,
            AngleInterface        = 3,
            TitleInterface        = 4,
            SubtitleInterface     = 5,
            AudioChannelInterface = 6
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
        enum SubtitleCommand {
            availableSubtitles,
            currentSubtitle,
            setCurrentSubtitle
        };
        enum AudioChannelCommand {
            availableAudioChannels,
            currentAudioChannel,
            setCurrentAudioChannel
        };

        virtual bool hasInterface(Interface iface) const = 0;

        virtual QVariant interfaceCall(Interface iface, int command,
                const QList<QVariant> &arguments = QList<QVariant>()) = 0;
};

} // namespace Phonon

Q_DECLARE_INTERFACE(Phonon::AddonInterface, "AddonInterface0.2.phonon.kde.org")

#endif //QT_NO_PHONON_MEDIACONTROLLER

QT_END_NAMESPACE
QT_END_HEADER

#endif // PHONON_ADDONINTERFACE_H
