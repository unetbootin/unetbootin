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

#include "platform_p.h"
#include "platformplugin.h"
#include "factory_p.h"
#include <QtCore/QCoreApplication>
#include <QtGui/QIcon>

QT_BEGIN_NAMESPACE

namespace Phonon
{

void Platform::saveVolume(const QString &outputName, qreal volume)
{
    PlatformPlugin *f = Factory::platformPlugin();
    if (f) {
        f->saveVolume(outputName, volume);
    }
}

qreal Platform::loadVolume(const QString &outputName)
{
    const PlatformPlugin *f = Factory::platformPlugin();
    if (f) {
        return f->loadVolume(outputName);
    }
    return 1.0;
}

AbstractMediaStream *Platform::createMediaStream(const QUrl &url, QObject *parent)
{
    PlatformPlugin *f = Factory::platformPlugin();
    if (!f) {
        return 0;
    }
    return f->createMediaStream(url, parent);
}

QIcon Platform::icon(const QString &name)
{
    const PlatformPlugin *f = Factory::platformPlugin();
    if (!f) {
        return QIcon();
    }
    return f->icon(name);
}

void Platform::notification(const char *notificationName, const QString &text,
        const QStringList &actions, QObject *receiver,
        const char *actionSlot)
{
    const PlatformPlugin *f = Factory::platformPlugin();
    if (f) {
        f->notification(notificationName, text, actions, receiver, actionSlot);
    }
}

QString Platform::applicationName()
{
    const PlatformPlugin *f = Factory::platformPlugin();
    if (f) {
        return f->applicationName();
    }
    QString ret = QCoreApplication::applicationName();
    if (ret.isEmpty())
        ret = QCoreApplication::applicationFilePath();
    return ret;
}

} // namespace Phonon

QT_END_NAMESPACE
