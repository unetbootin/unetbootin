/*  This file is part of the KDE project.

    Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).

    This library is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 2.1 or 3 of the License.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with this library.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef Phonon_QT7_AUDIODEVICE_H
#define Phonon_QT7_AUDIODEVICE_H

#include <AudioToolbox/AudioToolbox.h>
#include <AudioUnit/AudioUnit.h>
#include <QtCore>

QT_BEGIN_NAMESPACE

namespace Phonon
{
namespace QT7
{
    class AudioDevice
    {
        public:
            enum Scope {In, Out};

            static QList<AudioDeviceID> devices(Scope scope);
            static AudioDeviceID defaultDevice(Scope scope);
            static AudioDeviceID defaultSystemDevice(Scope scope);
            static AudioDeviceID currentDevice(AudioUnit unit, Scope scope);
            static bool setDevice(AudioUnit unit, AudioDeviceID deviceID, Scope scope);
            static QString deviceName(AudioDeviceID deviceId);
            static QString deviceSourceName(AudioDeviceID deviceID);
            static QString deviceSourceNameElseDeviceName(AudioDeviceID deviceID);
            static QString deviceNameElseDeviceSourceName(AudioDeviceID deviceID);
            static QString deviceUID(AudioDeviceID deviceID);
    };

}} //namespace Phonon::QT7

QT_END_NAMESPACE

#endif // Phonon_QT7_AUDIODEVICE_H
