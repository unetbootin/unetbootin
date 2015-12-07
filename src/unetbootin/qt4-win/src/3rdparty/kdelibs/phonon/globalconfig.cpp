/*  This file is part of the KDE project
    Copyright (C) 2006 Matthias Kretz <kretz@kde.org>

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

#include "globalconfig_p.h"

#include "factory_p.h"
#include "objectdescription.h"
#include "phonondefs_p.h"
#include "backendinterface.h"
#include "qsettingsgroup_p.h"

#include <QtCore/QList>
#include <QtCore/QVariant>

QT_BEGIN_NAMESPACE

namespace Phonon
{

GlobalConfig::GlobalConfig(QObject *parent)
    : QObject(parent)
    , m_config(QLatin1String("kde.org"), QLatin1String("libphonon"))
{
}

GlobalConfig::~GlobalConfig()
{
}

static void filterAdvanced(BackendInterface *backendIface, QList<int> *list)
{
    QMutableListIterator<int> it(*list);
    while (it.hasNext()) {
        const QHash<QByteArray, QVariant> properties = backendIface->objectDescriptionProperties(Phonon::AudioOutputDeviceType, it.next());
        QVariant var = properties.value("isAdvanced");
        if (var.isValid() && var.toBool()) {
            it.remove();
        }
    }
}

QList<int> GlobalConfig::audioOutputDeviceListFor(Phonon::Category category, HideAdvancedDevicesOverride override) const
{
    //The devices need to be stored independently for every backend
    const QSettingsGroup backendConfig(&m_config, QLatin1String("AudioOutputDevice")); // + Factory::identifier());
    const QSettingsGroup generalGroup(&m_config, QLatin1String("General"));
    const bool hideAdvancedDevices = (override == FromSettings
            ? generalGroup.value(QLatin1String("HideAdvancedDevices"), true)
            : static_cast<bool>(override));

    //First we lookup the available devices directly from the backend
    BackendInterface *backendIface = qobject_cast<BackendInterface *>(Factory::backend());
    if (!backendIface) {
        return QList<int>();
    }

    // this list already is in default order (as defined by the backend)
    QList<int> defaultList = backendIface->objectDescriptionIndexes(Phonon::AudioOutputDeviceType);
    if (hideAdvancedDevices) {
        filterAdvanced(backendIface, &defaultList);
    }

    QString categoryKey = QLatin1String("Category") + QString::number(static_cast<int>(category));
    if (!backendConfig.hasKey(categoryKey)) {
        // no list in config for the given category
        QString categoryKey = QLatin1String("Category") + QString::number(static_cast<int>(Phonon::NoCategory));
        if (!backendConfig.hasKey(categoryKey)) {
            // no list in config for NoCategory
            return defaultList;
        }
    }

    //Now the list from m_config
    QList<int> deviceList = backendConfig.value(categoryKey, QList<int>());

    //if there are devices in m_config that the backend doesn't report, remove them from the list
    QMutableListIterator<int> i(deviceList);
    while (i.hasNext()) {
        if (0 == defaultList.removeAll(i.next())) {
            i.remove();
        }
    }

    //if the backend reports more devices that are not in m_config append them to the list
    deviceList += defaultList;

    return deviceList;
}

int GlobalConfig::audioOutputDeviceFor(Phonon::Category category) const
{
    QList<int> ret = audioOutputDeviceListFor(category);
    if (ret.isEmpty())
        return -1;
    return ret.first();
}

/*QList<int> GlobalConfig::audioCaptureDeviceList() const
{
    //First we lookup the available devices directly from the backend
    BackendInterface *backendIface = qobject_cast<BackendInterface *>(Factory::backend());
    if (!backendIface) {
        return QList<int>();
    }
    QList<int> deviceIndexes = backendIface->objectDescriptionIndexes(Phonon::AudioCaptureDeviceType);

    QList<int> defaultList = deviceIndexes;

    //Now the list from m_config
    //The devices need to be stored independently for every backend
    const QSettingsGroup backendConfig(&m_config, QLatin1String("AudioCaptureDevice_") +
            Factory::identifier());
    QList<int> deviceList = backendConfig.value(QLatin1String("DeviceOrder"), QList<int>());
    if (deviceList.isEmpty()) {
        //try to read from global group for defaults
        const QSettingsGroup globalConfig(&m_config, QLatin1String("AudioCaptureDevice"));
        deviceList = globalConfig.value(QLatin1String("DeviceOrder"), defaultList);
    }

    QMutableListIterator<int> i(deviceList);
    while (i.hasNext()) {
        if (0 == defaultList.removeAll(i.next())) {
            //if there are devices in m_config that the backend doesn't report, remove them from the list
            i.remove();
        }
    }
    //if the backend reports more devices that are not in m_config append them to the list
    deviceList += defaultList;

    return deviceList;
}

int GlobalConfig::audioCaptureDevice() const
{
    return audioCaptureDeviceList().first();
}*/

} // namespace Phonon

QT_END_NAMESPACE

#include "moc_globalconfig_p.cpp"

// vim: sw=4 ts=4
