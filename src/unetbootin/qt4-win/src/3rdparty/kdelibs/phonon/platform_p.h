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

#ifndef PHONON_PLATFORM_P_H
#define PHONON_PLATFORM_P_H


#include <QtCore/QStringList>
#include <QtCore/QtGlobal>
#include "phonon/phonon_export.h"

QT_BEGIN_NAMESPACE

class QIcon;
class QObject;
class QUrl;

namespace Phonon
{
class AbstractMediaStream;

namespace Platform
{

void saveVolume(const QString &outputName, qreal volume);
qreal loadVolume(const QString &outputName);
AbstractMediaStream *createMediaStream(const QUrl &url, QObject *parent);
QIcon icon(const QString &name);
void notification(const char *notificationName, const QString &text,
        const QStringList &actions = QStringList(), QObject *receiver = 0,
        const char *actionSlot = 0);
QString applicationName();

} // namespace Platform
} // namespace Phonon

QT_END_NAMESPACE

#endif // PHONON_PLATFORM_P_H
