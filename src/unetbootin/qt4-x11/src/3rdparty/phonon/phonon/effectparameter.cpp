/*  This file is part of the KDE project
    Copyright (C) 2006 Matthias Kretz <kretz@kde.org>

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

#include "effectparameter.h"
#include "effectparameter_p.h"

QT_BEGIN_NAMESPACE

#ifndef QT_NO_PHONON_EFFECT

namespace Phonon
{

uint qHash(const Phonon::EffectParameter &param)
{
    return param.id();
}

EffectParameter::EffectParameter()
    : d(new EffectParameterPrivate)
{
}

EffectParameter::EffectParameter(int parameterId, const QString &name, Hints hints,
        const QVariant &defaultValue, const QVariant &min, const QVariant &max,
        const QVariantList &values, const QString &description)
    : d(new EffectParameterPrivate)
{
    d->parameterId = parameterId;
    d->min = min;
    d->max = max;
    d->defaultValue = defaultValue;
    d->name = name;
    d->possibleValues = values;
    d->description = description;
    d->hints = hints;
}

EffectParameter::~EffectParameter()
{
}

EffectParameter::EffectParameter(const EffectParameter &rhs)
    : d(rhs.d)
{
}

EffectParameter &EffectParameter::operator=(const EffectParameter &rhs)
{
    d = rhs.d;
    return *this;
}

bool EffectParameter::operator<(const EffectParameter &rhs) const
{
    return d->parameterId < rhs.d->parameterId;
}

bool EffectParameter::operator==(const EffectParameter &rhs) const
{
    return d->parameterId == rhs.d->parameterId;
}

bool EffectParameter::operator>(const EffectParameter &rhs) const
{
    return d->parameterId > rhs.d->parameterId;
}

const QString &EffectParameter::name() const
{
    return d->name;
}

const QString &EffectParameter::description() const
{
    return d->description;
}

bool EffectParameter::isLogarithmicControl() const
{
    return d->hints  & LogarithmicHint;
}

QVariant::Type EffectParameter::type() const
{
    if (d->possibleValues.isEmpty()) {
        return d->defaultValue.type();
    }
    return QVariant::String;
}

QVariantList EffectParameter::possibleValues() const
{
    return d->possibleValues;
}

QVariant EffectParameter::minimumValue() const
{
    return d->min;
}

QVariant EffectParameter::maximumValue() const
{
    return d->max;
}

QVariant EffectParameter::defaultValue() const
{
    return d->defaultValue;
}

int EffectParameter::id() const
{
    return d->parameterId;
}

}

#endif //QT_NO_PHONON_EFFECT

QT_END_NAMESPACE

// vim: sw=4 ts=4
