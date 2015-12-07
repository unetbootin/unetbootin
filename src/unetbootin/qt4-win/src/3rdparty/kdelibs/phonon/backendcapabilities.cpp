/*  This file is part of the KDE project
    Copyright (C) 2005-2006 Matthias Kretz <kretz@kde.org>

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

#include "backendcapabilities.h"
#include "backendcapabilities_p.h"

#include "phonondefs_p.h"
#include "backendinterface.h"
#include "factory_p.h"
#include "globalconfig_p.h"
#include "globalstatic_p.h"
#include "objectdescription.h"

#include <QtCore/QList>
#include <QtCore/QSet>
#include <QtCore/QStringList>

QT_BEGIN_NAMESPACE

PHONON_GLOBAL_STATIC(Phonon::BackendCapabilitiesPrivate, globalBCPrivate)

template<class T>
static inline bool sortByInitialPreference(const T &o1, const T &o2)
{
    // QVariant::toInt() returns 0 if the type() is not convertable to int
    const int initialPreference1 = o1.property("initialPreference").toInt();
    const int initialPreference2 = o2.property("initialPreference").toInt();
    // the higher the number the higher it should appear in the list
    return initialPreference1 > initialPreference2;
}

namespace Phonon
{

BackendCapabilities::Notifier *BackendCapabilities::notifier()
{
    return globalBCPrivate;
}

/*
#define SUPPORTS(foo) \
bool BackendCapabilities::supports ## foo() \
{ \
    if (BackendInterface *backendIface = qobject_cast<BackendInterface *>(Factory::backend())) \
        return backendIface->supports ## foo(); \
    else \
        return false; \
}

SUPPORTS(Video)
SUPPORTS(OSD)
SUPPORTS(Subtitles)
*/

QStringList BackendCapabilities::availableMimeTypes()
{
    if (BackendInterface *backendIface = qobject_cast<BackendInterface *>(Factory::backend()))
        return backendIface->availableMimeTypes();
    else
        return QStringList();
}

bool BackendCapabilities::isMimeTypeAvailable(const QString &mimeType)
{
    QObject *m_backendObject = Factory::backend(false);
    if (!m_backendObject) {
        if (!Factory::isMimeTypeAvailable(mimeType)) {
            return false;
        }
        // without loading the backend we found out that the MIME type might be supported, now we
        // want to know for certain. For that we need to load the backend.
        m_backendObject = Factory::backend(true);
    }
    if (!m_backendObject) {
        // no backend == no MIME type supported at all
        return false;
    }
    return availableMimeTypes().contains(mimeType);
}

#define availableDevicesImpl(T) \
QList<T> BackendCapabilities::available ## T ## s() \
{ \
    BackendInterface *backendIface = qobject_cast<BackendInterface *>(Factory::backend()); \
    QList<T> ret; \
    if (backendIface) { \
        QList<int> deviceIndexes = backendIface->objectDescriptionIndexes(Phonon::T ## Type); \
        foreach (int i, deviceIndexes) \
            ret.append(T::fromIndex(i)); \
    } \
    qSort(ret.begin(), ret.end(), sortByInitialPreference<T>); \
    return ret; \
}

#define availableDevicesImpl2(T) \
QList<T ## Description> BackendCapabilities::available ## T ## s() \
{ \
    BackendInterface *backendIface = qobject_cast<BackendInterface *>(Factory::backend()); \
    QList<T ## Description> ret; \
    if (backendIface) { \
        QList<int> deviceIndexes = backendIface->objectDescriptionIndexes(Phonon::T ## Type); \
        foreach (int i, deviceIndexes) \
            ret.append(T ## Description::fromIndex(i)); \
    } \
    qSort(ret.begin(), ret.end(), sortByInitialPreference<T ## Description>); \
    return ret; \
}

QList<AudioOutputDevice> BackendCapabilities::availableAudioOutputDevices()
{
    QList<AudioOutputDevice> ret;
    const QList<int> deviceIndexes = GlobalConfig().audioOutputDeviceListFor(Phonon::NoCategory);
    foreach (int i, deviceIndexes) {
        ret.append(AudioOutputDevice::fromIndex(i));
    }
    return ret;
}

/*availableDevicesImpl(AudioCaptureDevice)
availableDevicesImpl(VideoOutputDevice)
availableDevicesImpl(VideoCaptureDevice)
availableDevicesImpl2(Visualization)
availableDevicesImpl2(AudioCodec)
availableDevicesImpl2(VideoCodec)
availableDevicesImpl2(ContainerFormat)*/

QList<EffectDescription> BackendCapabilities::availableAudioEffects()
{
    BackendInterface *backendIface = qobject_cast<BackendInterface *>(Factory::backend());
    QList<EffectDescription> ret;
    if (backendIface) {
        QList<int> deviceIndexes = backendIface->objectDescriptionIndexes(Phonon::EffectType);
        foreach (int i, deviceIndexes) {
            ret.append(EffectDescription::fromIndex(i));
        }
    }
    return ret;
}

} // namespace Phonon

QT_END_NAMESPACE

#include "moc_backendcapabilities.cpp"
#include "moc_backendcapabilities_p.cpp"

// vim: sw=4 ts=4

