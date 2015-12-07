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

#ifndef PHONON_GLOBALCONFIG_P_H
#define PHONON_GLOBALCONFIG_P_H

#include <QtCore/QObject>
#include <QtCore/QSettings>

#include "phonon/phononnamespace.h"

QT_BEGIN_HEADER
QT_BEGIN_NAMESPACE

namespace Phonon
{
    class GlobalConfig : public QObject
    {
        Q_OBJECT
    public:
        GlobalConfig(QObject *parent = 0);
        ~GlobalConfig();

        enum HideAdvancedDevicesOverride {
            ShowAdvancedDevices = 0,
            HideAdvancedDevices = 1,
            FromSettings = 2
        };
        QList<int> audioOutputDeviceListFor(Phonon::Category category, HideAdvancedDevicesOverride override = FromSettings) const;
        int audioOutputDeviceFor(Phonon::Category category) const;

//        QList<int> audioCaptureDeviceList() const;
//        int audioCaptureDevice() const;

Q_SIGNALS:
        void audioOutputDeviceConfigChanged();

    private:
        QSettings m_config;
    };
} // namespace Phonon

QT_END_NAMESPACE
QT_END_HEADER

#endif // PHONON_GLOBALCONFIG_P_H
