/*  This file is part of the KDE project
    Copyright (C) 2007 Matthias Kretz <kretz@kde.org>

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

#ifndef PHONON_PLATFORM_P_H
#define PHONON_PLATFORM_P_H


#include <QtCore/QStringList>
#include <QtCore/QtGlobal>
#include <QtCore/QPair>
#include "phonon_export.h"
#include "objectdescription.h"

QT_BEGIN_NAMESPACE

class QIcon;
class QObject;
class QUrl;
class QStyle;

namespace Phonon
{
class AbstractMediaStream;

namespace Platform
{

void saveVolume(const QString &outputName, qreal volume);
qreal loadVolume(const QString &outputName);
AbstractMediaStream *createMediaStream(const QUrl &url, QObject *parent);
QIcon icon(const QString &name, QStyle *style = 0);
void notification(const char *notificationName, const QString &text,
        const QStringList &actions = QStringList(), QObject *receiver = 0,
        const char *actionSlot = 0);
QString applicationName();
QList<QPair<QByteArray, QString> > deviceAccessListFor(const Phonon::AudioOutputDevice &deviceDesc);

} // namespace Platform
} // namespace Phonon

QT_END_NAMESPACE

#endif // PHONON_PLATFORM_P_H
