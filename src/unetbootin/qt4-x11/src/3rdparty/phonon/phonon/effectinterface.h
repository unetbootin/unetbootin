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

#ifndef PHONON_EFFECTINTERFACE_H
#define PHONON_EFFECTINTERFACE_H

#include "phononnamespace.h"
#include <QtCore/QVariant>

QT_BEGIN_HEADER
QT_BEGIN_NAMESPACE

#ifndef QT_NO_PHONON_EFFECT

namespace Phonon
{
    class EffectParameter;
    /** \class EffectInterface effectinterface.h Phonon/EffectInterface
     * \short Interface for Effect objects
     *
     * \author Matthias Kretz <kretz@kde.org>
     */
    class EffectInterface
    {
        public:
            virtual ~EffectInterface() {}
            /**
             * Returns the EffectParameter objects to describe the parameters of this effect.
             */
            virtual QList<EffectParameter> parameters() const = 0;
            /**
             * Returns the value for the selected parameter.
             */
            virtual QVariant parameterValue(const EffectParameter &) const = 0;
            /**
             * Sets the value for the selected parameter.
             */
            virtual void setParameterValue(const EffectParameter &, const QVariant &newValue) = 0;
    };
} //namespace Phonon

Q_DECLARE_INTERFACE(Phonon::EffectInterface, "EffectInterface0.phonon.kde.org")

#endif //QT_NO_PHONON_EFFECT

QT_END_NAMESPACE
QT_END_HEADER

#endif // PHONON_EFFECTINTERFACE_H
