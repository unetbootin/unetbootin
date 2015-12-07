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

#include "audiodevice.h"
#include "audiograph.h"
#include <QtCore/QVector>
#include "backendheader.h"

QT_BEGIN_NAMESPACE

namespace Phonon
{
namespace QT7
{

QList<AudioDeviceID> AudioDevice::devices(Scope scope)
{
    QList<AudioDeviceID> devices;

    // Insert the default device explicit
    if (AudioDeviceID defdev = defaultDevice(scope))
        devices << defdev;

    // How many input/output devices are awailable:
    UInt32 deviceCount = 0;
	OSStatus err = AudioHardwareGetPropertyInfo(kAudioHardwarePropertyDevices, &deviceCount, 0);
    BACKEND_ASSERT3(err == noErr, "Could not get number of audio devices awailable.", FATAL_ERROR, devices)

    // Get list of all devices:
    AudioDeviceID deviceArray[deviceCount];
    err = AudioHardwareGetProperty(kAudioHardwarePropertyDevices, &deviceCount, &deviceArray);
    BACKEND_ASSERT3(err == noErr, "Could not get audio devices list.", FATAL_ERROR, devices)

    for (uint i=0; i<deviceCount; i++){
        if (!devices.contains(deviceArray[i])){
            // Check if the current device is input or output:
            UInt32 size;
	        err = AudioDeviceGetPropertyInfo(deviceArray[i], 0, scope == In, kAudioDevicePropertyStreams, &size, 0);
	        if (err == noErr && size > 0)
                devices << deviceArray[i];
        }
    }
    return devices;
}

AudioDeviceID AudioDevice::defaultSystemDevice(Scope scope)
{
    ARGUMENT_UNSUPPORTED(scope, In, NORMAL_ERROR, 0)
    AudioDeviceID deviceID = 0;
    UInt32 size = sizeof(deviceID);
    OSStatus err = AudioHardwareGetProperty(kAudioHardwarePropertyDefaultSystemOutputDevice, &size, &deviceID);
    BACKEND_ASSERT3(err == noErr, "Could not get default system audio device.", FATAL_ERROR, 0)
    return deviceID;
}

AudioDeviceID AudioDevice::defaultDevice(Scope scope)
{
    ARGUMENT_UNSUPPORTED(scope, In, NORMAL_ERROR, 0)
    AudioDeviceID deviceID = 0;
    UInt32 size = sizeof(deviceID);
    OSStatus err = AudioHardwareGetProperty(kAudioHardwarePropertyDefaultOutputDevice, &size, &deviceID);
    BACKEND_ASSERT3(err == noErr, "Could not get default output audio device.", FATAL_ERROR, 0)
    return deviceID;
}

AudioDeviceID AudioDevice::currentDevice(AudioUnit /*unit*/, Scope /*scope*/)
{
    return 0;
#if 0

kAudioDevicePropertyDeviceUID

    if (!m_audioUnit)
        return 0;
    AudioDeviceID deviceID = 0;
    UInt32 size = sizeof(deviceID);
	OSStatus err = AudioUnitGetProperty(m_audioUnit, kAudioOutputUnitProperty_CurrentDevice, kAudioUnitScope_Global, 0, &size, &deviceID);
    BACKEND_ASSERT3(err == noErr, "Could not get current audio device.", FATAL_ERROR, 0)
    return deviceID;
#endif
}

bool AudioDevice::setDevice(AudioUnit unit, AudioDeviceID deviceID, Scope scope)
{
    ARGUMENT_UNSUPPORTED(scope, In, NORMAL_ERROR, false)
    UInt32 size = sizeof(deviceID);
	OSStatus err = AudioUnitSetProperty(unit, kAudioOutputUnitProperty_CurrentDevice, kAudioUnitScope_Global, 0, &deviceID, size);
    if (err != noErr)
        return false;
    return true;
}

QString AudioDevice::deviceSourceNameElseDeviceName(AudioDeviceID deviceID)
{
    QString name = deviceSourceName(deviceID);
    if (name.isEmpty())
        name = deviceName(deviceID);
    return name;
}

QString AudioDevice::deviceNameElseDeviceSourceName(AudioDeviceID deviceID)
{
    QString name = deviceName(deviceID);
    if (name.isEmpty())
        name = deviceSourceName(deviceID);
    return name;
}

QString AudioDevice::deviceName(AudioDeviceID deviceID)
{
    if (!deviceID)
        return QString();
    CFStringRef cfString = 0;
    UInt32 size = sizeof(cfString);
	OSStatus err = AudioDeviceGetProperty(deviceID, 0, 0, kAudioDevicePropertyDeviceNameCFString, &size, &cfString);
	if (err != noErr)
        return QString();
    QString name = PhononCFString::toQString(cfString);
    CFRelease(cfString);
    return name;
}

QString AudioDevice::deviceSourceName(AudioDeviceID deviceID)
{
    if (!deviceID)
        return QString();
    UInt32 dataSource = 0;
    UInt32 size = sizeof(dataSource);
    OSStatus err = AudioDeviceGetProperty(deviceID, 0, 0, kAudioDevicePropertyDataSource, &size, &dataSource);
	if (err != noErr)
        return QString();

    CFStringRef cfName = 0;
    AudioValueTranslation translation = {&dataSource, sizeof(dataSource), &cfName, sizeof(cfName)};
    size = sizeof(translation);
    err = AudioDeviceGetProperty(deviceID, 0, 0, kAudioDevicePropertyDataSourceNameForIDCFString, &size, &translation);
	if (err != noErr){
        CFRelease(cfName);
        return QString();
    }
	QString name = PhononCFString::toQString(cfName);
    CFRelease(cfName);
    return name;
}

QString AudioDevice::deviceUID(AudioDeviceID deviceID)
{
    if (!deviceID)
        return QString();
        
    CFStringRef cfString = 0;
    UInt32 size = sizeof(cfString);
	OSStatus err = AudioDeviceGetProperty(deviceID, 0, 0, kAudioDevicePropertyDeviceUID, &size, &cfString);
	if (err != noErr)
        return QString();
    QString uid = PhononCFString::toQString(cfString);
    CFRelease(cfString);
    return uid;
}

}} //namespace Phonon::QT7

QT_END_NAMESPACE
