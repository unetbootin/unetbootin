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

#ifndef EFFECT_P_H
#define EFFECT_P_H

#include "effect.h"
#include "effectparameter.h"
#include "medianode_p.h"
#include <QtCore/QHash>
#include <QtCore/QVariant>
#include "phonondefs_p.h"

QT_BEGIN_NAMESPACE

namespace Phonon
{
class EffectPrivate : public MediaNodePrivate
{
    Q_DECLARE_PUBLIC(Effect)
    PHONON_PRIVATECLASS
    public:
        virtual QObject *qObject() { return q_func(); }
    protected:
        EffectPrivate()
        {
        }

//X         Effect::Type type;
        EffectDescription description;
        QHash<EffectParameter, QVariant> parameterValues;
};
} //namespace Phonon

QT_END_NAMESPACE

#endif // EFFECT_P_H
// vim: sw=4 ts=4 tw=80
