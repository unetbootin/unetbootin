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

#ifndef PHONON_VOLUMEFADEREFFECT_P_H
#define PHONON_VOLUMEFADEREFFECT_P_H

#include "volumefadereffect.h"
#include "effect_p.h"

QT_BEGIN_NAMESPACE

#ifndef QT_NO_PHONON_VOLUMEFADEREFFECT

namespace Phonon
{
class VolumeFaderEffectPrivate : public EffectPrivate
{
    Q_DECLARE_PUBLIC(VolumeFaderEffect)
    PHONON_PRIVATECLASS
    protected:
        VolumeFaderEffectPrivate()
            : currentVolume(1.0)
            , fadeCurve(VolumeFaderEffect::Fade3Decibel)
        {
            // invalid EffectDescription
            // ############# parameter functions are incorrect
        }

        float currentVolume;
        VolumeFaderEffect::FadeCurve fadeCurve;
};
}

#endif //QT_NO_PHONON_VOLUMEFADEREFFECT

QT_END_NAMESPACE

#endif // PHONON_VOLUMEFADEREFFECT_P_H
// vim: sw=4 ts=4 tw=80
