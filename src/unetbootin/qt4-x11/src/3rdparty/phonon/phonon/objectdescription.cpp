/*  This file is part of the KDE project
    Copyright (C) 2006-2008 Matthias Kretz <kretz@kde.org>

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

#include "objectdescription.h"
#include "objectdescription_p.h"

#include <QtCore/QObject>
#include <QtCore/QSet>
#include "factory_p.h"
#include <QtCore/QStringList>
#include "backendinterface.h"
#include "platformplugin.h"

QT_BEGIN_NAMESPACE

namespace Phonon
{

ObjectDescriptionData::ObjectDescriptionData(int index, const QHash<QByteArray, QVariant> &properties)
    : d(new ObjectDescriptionPrivate(index, properties))
{
}

ObjectDescriptionData::ObjectDescriptionData(ObjectDescriptionPrivate *dd)
    : d(dd)
{
}

ObjectDescriptionData::~ObjectDescriptionData()
{
    delete d;
}

bool ObjectDescriptionData::operator==(const ObjectDescriptionData &otherDescription) const
{
    if (!isValid()) {
        return !otherDescription.isValid();
    }
    if (!otherDescription.isValid()) {
        return false;
    }
    return *d == *otherDescription.d;
}

int ObjectDescriptionData::index() const
{
    if (!isValid()) {
        return -1;
    }
    return d->index;
}

QString ObjectDescriptionData::name() const
{
    if (!isValid()) {
        return QString();
    }
    return d->name;
}

QString ObjectDescriptionData::description() const
{
    if (!isValid()) {
        return QString();
    }
    return d->description;
}

QVariant ObjectDescriptionData::property(const char *name) const
{
    if (!isValid()) {
        return QVariant();
    }
    return d->properties.value(name);
}

QList<QByteArray> ObjectDescriptionData::propertyNames() const
{
    if (!isValid()) {
        return QList<QByteArray>();
    }
    return d->properties.keys();
}

bool ObjectDescriptionData::isValid() const
{
    return d != 0;
}

ObjectDescriptionData *ObjectDescriptionData::fromIndex(ObjectDescriptionType type, int index)
{
    // prefer to get the ObjectDescriptionData from the platform plugin for audio devices
#ifndef QT_NO_PHONON_PLATFORMPLUGIN
    if (type == AudioOutputDeviceType || type == AudioCaptureDeviceType) {
        PlatformPlugin *platformPlugin = Factory::platformPlugin();
        if (platformPlugin) {
            QList<int> indexes = platformPlugin->objectDescriptionIndexes(type);
            if (indexes.contains(index)) {
                QHash<QByteArray, QVariant> properties = platformPlugin->objectDescriptionProperties(type, index);
                return new ObjectDescriptionData(index, properties);
            }
        }
    }
#endif //QT_NO_PHONON_PLATFORMPLUGIN

    QObject *b = Factory::backend();
    BackendInterface *iface = qobject_cast<BackendInterface *>(b);
    if (iface) {
        QList<int> indexes = iface->objectDescriptionIndexes(type);
        if (indexes.contains(index)) {
            QHash<QByteArray, QVariant> properties = iface->objectDescriptionProperties(type, index);
            return new ObjectDescriptionData(index, properties);
        }
    }
    return new ObjectDescriptionData(0); // invalid
}

} //namespace Phonon

QT_END_NAMESPACE
// vim: sw=4 ts=4
