/*  This file is part of the KDE project
    Copyright (C) 2007-2008 Matthias Kretz <kretz@kde.org>

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

#ifndef PHONON_AUDIOOUTPUTINTERFACE_H
#define PHONON_AUDIOOUTPUTINTERFACE_H

#include "phonon/phononnamespace.h"
#include <QtCore/QtGlobal>

QT_BEGIN_HEADER
QT_BEGIN_NAMESPACE

namespace Phonon
{
/** \class AudioOutputInterface audiooutputinterface.h Phonon/AudioOutputInterface
 * \short Interface for AudioOutput objects
 *
 * \author Matthias Kretz <kretz@kde.org>
 */
class AudioOutputInterface
{
    public:
        virtual ~AudioOutputInterface() {}

        /**
         * Returns the current software volume.
         *
         * A value of 0.0 means muted, 1.0 means unchanged, 2.0 means double voltage (i.e. all
         * samples are multiplied by 2).
         */
        virtual qreal volume() const = 0;
        /**
         * Sets the new current software volume.
         *
         * A value of 0.0 means muted, 1.0 means unchanged, 2.0 means double voltage (i.e. all
         * samples are multiplied by 2).
         */
        virtual void setVolume(qreal) = 0;

        /**
         * Returns the index of the device that is used. The index is the number returned from
         * BackendInterface::objectDescriptionIndexes(AudioOutputDeviceType).
         */
        virtual int outputDevice() const = 0;
        /**
         * Requests to change the current output device to the one identified by the passed index.
         *
         * The index is the number returned from
         * BackendInterface::objectDescriptionIndexes(AudioOutputDeviceType).
         *
         * \returns \c true if the requested device works and is used after this call.
         * \returns \c false if something failed and the device is not used after this call.
         */
        virtual bool setOutputDevice(int) = 0;
};
} // namespace Phonon

Q_DECLARE_INTERFACE(Phonon::AudioOutputInterface, "AudioOutputInterface2.phonon.kde.org")

QT_END_NAMESPACE
QT_END_HEADER

#endif // PHONON_AUDIOOUTPUTINTERFACE_H
