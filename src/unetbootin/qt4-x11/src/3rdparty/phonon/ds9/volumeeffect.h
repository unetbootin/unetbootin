/*  This file is part of the KDE project.

Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).

This library is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation, either version 2.1 or 3 of the License.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with this library.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef PHONON_VOLUMEEFFECT_H
#define PHONON_VOLUMEEFFECT_H

#include "effect.h"
#include <phonon/volumefaderinterface.h>

QT_BEGIN_NAMESPACE

#ifndef QT_NO_PHONON_VOLUMEFADEREFFECT

namespace Phonon
{
    namespace DS9
    {
        class VolumeEffectFilter;
        class VolumeEffect : public Effect, public Phonon::VolumeFaderInterface
        {
            Q_OBJECT
                Q_INTERFACES(Phonon::VolumeFaderInterface)
        public:
            VolumeEffect(QObject *parent);

            //reimplementation
            virtual float volume() const;
            virtual void setVolume(float);
            virtual Phonon::VolumeFaderEffect::FadeCurve fadeCurve() const;
            virtual void setFadeCurve(Phonon::VolumeFaderEffect::FadeCurve);
            virtual void fadeTo(float, int);

        private:
            float m_volume;

            //paramaters used to fade
            Phonon::VolumeFaderEffect::FadeCurve m_fadeCurve;

            bool m_fading; //determines if we should be fading.
            float m_initialVolume;  
            float m_targetVolume;
            int m_fadeDuration;
            int m_fadeSamplePosition;
            qreal (*m_fadeCurveFn)(const qreal, const qreal, const qreal);

            //allow the filter to get access to that
            friend class VolumeEffectFilter;

        };
    }
}

#endif //QT_NO_PHONON_VOLUMEFADEREFFECT

QT_END_NAMESPACE

#endif
