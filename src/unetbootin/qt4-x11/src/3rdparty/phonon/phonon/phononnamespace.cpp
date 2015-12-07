/*  This file is part of the KDE project
    Copyright (C) 2006-2007 Matthias Kretz <kretz@kde.org>

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

#include "phononnamespace.h"
#include "phononnamespace_p.h"
#include "phonondefs_p.h"

#include "factory_p.h"

#include <QtCore/QCoreApplication>
#include <QtCore/QList>

QT_BEGIN_NAMESPACE

namespace Phonon
{
    /*!
        Returns the version number of Phonon at run-time as a string (for
        example, "4.0.0"). This may be a different version than the
        version the application was compiled against.

        \sa PHONON_VERSION_STR
    */
    const char *phononVersion()
    {
        return PHONON_VERSION_STR;
    }

    QString categoryToString(Category c)
    {
        switch(c)
        {
        case Phonon::NoCategory:
            break;
        case Phonon::NotificationCategory:
            return QCoreApplication::translate("Phonon::", "Notifications");
        case Phonon::MusicCategory:
            return QCoreApplication::translate("Phonon::", "Music");
        case Phonon::VideoCategory:
            return QCoreApplication::translate("Phonon::", "Video");
        case Phonon::CommunicationCategory:
            return QCoreApplication::translate("Phonon::", "Communication");
        case Phonon::GameCategory:
            return QCoreApplication::translate("Phonon::", "Games");
        case Phonon::AccessibilityCategory:
            return QCoreApplication::translate("Phonon::", "Accessibility");
        }
        return QString();
    }
}

static int registerPhononMetaTypes()
{
    qRegisterMetaType<Phonon::State>();
    qRegisterMetaType<Phonon::ErrorType>();
    qRegisterMetaType<Phonon::Category>();

    // need those for QSettings
    qRegisterMetaType<QList<int> >();
    qRegisterMetaTypeStreamOperators<QList<int> >("QList<int>");

    return 0; // something
}

#ifdef Q_CONSTRUCTOR_FUNCTION
Q_CONSTRUCTOR_FUNCTION(registerPhononMetaTypes)
#else
static const int _Phonon_registerMetaTypes = registerPhononMetaTypes();
#endif

QT_END_NAMESPACE

// vim: sw=4 ts=4
