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

#ifndef PHONON_PLATFORMPLUGIN_H
#define PHONON_PLATFORMPLUGIN_H

#include <QtCore/QObject>
#include <QtCore/QStringList>
#include "phonon/phonon_export.h"

QT_BEGIN_HEADER
QT_BEGIN_NAMESPACE

class QUrl;
class QObject;
class QIcon;

namespace Phonon
{
class AbstractMediaStream;

class PlatformPlugin
{
    public:
        virtual ~PlatformPlugin() {}

        /**
         * Creates a AbstractMediaStream object that provides the data for the given \p url. On KDE
         * this uses KIO.
         */
        virtual AbstractMediaStream *createMediaStream(const QUrl &url, QObject *parent) = 0;

        /**
         * Returns the icon for the given icon name.
         */
        virtual QIcon icon(const QString &name) const = 0;

        /**
         * Shows a notification popup
         */
        virtual void notification(const char *notificationName, const QString &text,
                const QStringList &actions = QStringList(), QObject *receiver = 0,
                const char *actionSlot = 0) const = 0;

        /**
         * Returns the name of the application. For most Qt application this is
         * QCoreApplication::applicationName(), but for KDE this is overridden by KAboutData.
         */
        virtual QString applicationName() const = 0;

        /**
         * Creates a backend object. This way the platform can decide the backend preference.
         */
        virtual QObject *createBackend() = 0;

        /**
         * Using the library loader of the platform, loads a given backend.
         */
        virtual QObject *createBackend(const QString &library, const QString &version) = 0;

        /**
         * Tries to check whether the default backend supports a given MIME type without loading the
         * actual backend library. On KDE this reads the MIME type list from the .desktop file of
         * the backend.
         */
        virtual bool isMimeTypeAvailable(const QString &mimeType) const = 0;

        /**
         * Saves the volume for the given output.
         */
        virtual void saveVolume(const QString &outputName, qreal volume) = 0;

        /**
         * Loads the volume for the given output.
         */
        virtual qreal loadVolume(const QString &outputName) const = 0;
};

} // namespace Phonon

Q_DECLARE_INTERFACE(Phonon::PlatformPlugin, "PlatformPlugin1.phonon.kde.org")

QT_END_NAMESPACE
QT_END_HEADER

#endif // PHONON_PLATFORMPLUGIN_H
