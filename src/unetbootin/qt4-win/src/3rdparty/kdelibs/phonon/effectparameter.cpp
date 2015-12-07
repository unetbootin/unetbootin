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

#include "effectparameter.h"
#include "effectparameter_p.h"

QT_BEGIN_NAMESPACE

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

QT_END_NAMESPACE

// vim: sw=4 ts=4
