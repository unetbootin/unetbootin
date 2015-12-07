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

#ifndef PHONON_QSETTINGSGROUP_P_H
#define PHONON_QSETTINGSGROUP_P_H

#include <QtCore/QSettings>
#include <QtCore/QString>
#include <QtCore/QVariant>

QT_BEGIN_HEADER
QT_BEGIN_NAMESPACE

namespace Phonon
{
class QSettingsGroup
{
    public:
        inline QSettingsGroup(QSettings *settings, const QString &name)
            : m_mutableSettings(settings),
            m_settings(settings),
            m_group(name + QLatin1Char('/'))
        {
        }

        inline QSettingsGroup(const QSettings *settings, const QString &name)
            : m_mutableSettings(0),
            m_settings(settings),
            m_group(name + QLatin1Char('/'))
        {
        }

        template<typename T>
        inline T value(const QString &key, const T &def) const
        {
            return qvariant_cast<T>(value(key, qVariantFromValue(def)));
        }

        inline QVariant value(const QString &key, const QVariant &def) const
        {
            return m_settings->value(m_group + key, def);
        }

        template<typename T>
        inline void setValue(const QString &key, const T &value)
        {
            Q_ASSERT(m_mutableSettings);
            m_mutableSettings->setValue(m_group + key, qVariantFromValue(value));
        }

        inline void removeEntry(const QString &key)
        {
            Q_ASSERT(m_mutableSettings);
            m_mutableSettings->remove(m_group + key);
        }

        inline bool hasKey(const QString &key) const
        {
            return m_settings->contains(m_group + key);
        }

    private:
        QSettings *const m_mutableSettings;
        const QSettings *const m_settings;
        QString m_group;
};
} // namespace Phonon

QT_END_NAMESPACE
QT_END_HEADER

#endif // PHONON_QSETTINGSGROUP_P_H
