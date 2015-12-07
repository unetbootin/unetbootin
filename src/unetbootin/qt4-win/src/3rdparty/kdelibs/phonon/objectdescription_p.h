/*  This file is part of the KDE project
    Copyright (C) 2006-2007 Matthias Kretz <kretz@kde.org>

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

#ifndef PHONON_OBJECTDESCRIPTION_P_H
#define PHONON_OBJECTDESCRIPTION_P_H

#include <QtCore/QByteRef>
#include <QtCore/QHash>
#include <QtCore/QString>
#include <QtCore/QVariant>
#include "phononnamespace_p.h"

QT_BEGIN_NAMESPACE

namespace Phonon
{
    class ObjectDescriptionPrivate
    {
        public:
            ObjectDescriptionPrivate(int _index, const QHash<QByteArray, QVariant> &_properties)
                : index(_index),
                name(_properties["name"].toString()),
                description(_properties["description"].toString()),
                properties(_properties)
            {
            }

            bool operator==(const ObjectDescriptionPrivate &rhs) const
            {
                if (index == rhs.index && (name != rhs.name || description != rhs.description))
                    pError() << "Same index (" << index <<
                        "), but different name/description. This is a bug in the Phonon backend.";
                return index == rhs.index;// && name == rhs.name && description == rhs.description;
            }

            int index;
            QString name, description;
            QHash<QByteArray, QVariant> properties;
    };
} // namespace Phonon

QT_END_NAMESPACE

#endif // PHONON_OBJECTDESCRIPTION_P_H
// vim: sw=4 ts=4 tw=80
