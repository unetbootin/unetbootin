/*  This file is part of the KDE project
    Copyright (C) 2005-2007 Matthias Kretz <kretz@kde.org>

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
#include "effect.h"
#include "effect_p.h"
#include "effectparameter.h"
#include "effectinterface.h"
#include "factory_p.h"

#define PHONON_INTERFACENAME EffectInterface

QT_BEGIN_NAMESPACE

#ifndef QT_NO_PHONON_EFFECT

namespace Phonon
{
Effect::~Effect()
{
}

Effect::Effect(const EffectDescription &description, QObject *parent)
    : QObject(parent), MediaNode(*new EffectPrivate)
{
    K_D(Effect);
    d->description = description;
    d->createBackendObject();
}

Effect::Effect(EffectPrivate &dd, QObject *parent)
    : QObject(parent), MediaNode(dd)
{
}

void EffectPrivate::createBackendObject()
{
    if (m_backendObject)
        return;
    Q_Q(Effect);
    m_backendObject = Factory::createEffect(description.index(), q);
    if (m_backendObject) {
        setupBackendObject();
    }
}

//X Effect::Type Effect::type() const
//X {
//X     K_D(const Effect);
//X     return d->type;
//X }
//X 
EffectDescription Effect::description() const
{
    K_D(const Effect);
    return d->description;
}

QList<EffectParameter> Effect::parameters() const
{
    K_D(const Effect);
    // there should be an iface object, but better be safe for those backend
    // switching corner-cases: when the backend switches the new backend might
    // not support this effect -> no iface object
    if (d->m_backendObject) {
        return INTERFACE_CALL(parameters());
    }
    return QList<EffectParameter>();
}

QVariant Effect::parameterValue(const EffectParameter &param) const
{
    K_D(const Effect);
    if (!d->m_backendObject) {
        return d->parameterValues[param];
    }
    return INTERFACE_CALL(parameterValue(param));
}

void Effect::setParameterValue(const EffectParameter &param, const QVariant &newValue)
{
    K_D(Effect);
    d->parameterValues[param] = newValue;
    if (d->backendObject()) {
        INTERFACE_CALL(setParameterValue(param, newValue));
    }
}

bool EffectPrivate::aboutToDeleteBackendObject()
{
    if (m_backendObject) {
        const QList<EffectParameter> parameters = pINTERFACE_CALL(parameters());
        foreach (const EffectParameter &p, parameters) {
            parameterValues[p] = pINTERFACE_CALL(parameterValue(p));
        }
    }
    return true;
}

void EffectPrivate::setupBackendObject()
{
    Q_ASSERT(m_backendObject);

    // set up attributes
    const QList<EffectParameter> parameters = pINTERFACE_CALL(parameters());
    foreach (const EffectParameter &p, parameters) {
        pINTERFACE_CALL(setParameterValue(p, parameterValues[p]));
    }
}

} //namespace Phonon

#endif //QT_NO_PHONON_EFFECT

QT_END_NAMESPACE

#include "moc_effect.cpp"

// vim: sw=4 ts=4 tw=80
