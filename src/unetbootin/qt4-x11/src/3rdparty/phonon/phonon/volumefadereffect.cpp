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

#include "volumefadereffect.h"
#include "volumefadereffect_p.h"
#include "volumefaderinterface.h"
#include "factory_p.h"

#include <QtCore/qmath.h>

#define PHONON_CLASSNAME VolumeFaderEffect
#define PHONON_INTERFACENAME VolumeFaderInterface

QT_BEGIN_NAMESPACE

#ifndef QT_NO_PHONON_VOLUMEFADEREFFECT

namespace Phonon
{
PHONON_HEIR_IMPL(Effect)

PHONON_INTERFACE_GETTER(float, volume, d->currentVolume)
PHONON_INTERFACE_SETTER(setVolume, currentVolume, float)
PHONON_INTERFACE_GETTER(Phonon::VolumeFaderEffect::FadeCurve, fadeCurve, d->fadeCurve)
PHONON_INTERFACE_SETTER(setFadeCurve, fadeCurve, Phonon::VolumeFaderEffect::FadeCurve)

#ifndef PHONON_LOG10OVER20
#define PHONON_LOG10OVER20
static const double log10over20 = 0.1151292546497022842; // ln(10) / 20
#endif // PHONON_LOG10OVER20

double VolumeFaderEffect::volumeDecibel() const
{
    return log(volume()) / log10over20;
}

void VolumeFaderEffect::setVolumeDecibel(double newVolumeDecibel)
{
    setVolume(exp(newVolumeDecibel * log10over20));
}


void VolumeFaderEffect::fadeIn(int fadeTime)
{
    fadeTo(1.0, fadeTime);
}

void VolumeFaderEffect::fadeOut(int fadeTime)
{
    fadeTo(0.0, fadeTime);
}

void VolumeFaderEffect::fadeTo(float volume, int fadeTime)
{
    K_D(VolumeFaderEffect);
    if (k_ptr->backendObject())
        INTERFACE_CALL(fadeTo(volume, fadeTime));
    else
        d->currentVolume = volume;
}

bool VolumeFaderEffectPrivate::aboutToDeleteBackendObject()
{
    if (m_backendObject) {
        currentVolume = pINTERFACE_CALL(volume());
        fadeCurve = pINTERFACE_CALL(fadeCurve());
    }
    return true;
}

void VolumeFaderEffectPrivate::setupBackendObject()
{
    Q_ASSERT(m_backendObject);

    // set up attributes
    pINTERFACE_CALL(setVolume(currentVolume));
    pINTERFACE_CALL(setFadeCurve(fadeCurve));
}
}


#endif //QT_NO_PHONON_VOLUMEFADEREFFECT

QT_END_NAMESPACE

#include "moc_volumefadereffect.cpp"

#undef PHONON_CLASSNAME
// vim: sw=4 ts=4
