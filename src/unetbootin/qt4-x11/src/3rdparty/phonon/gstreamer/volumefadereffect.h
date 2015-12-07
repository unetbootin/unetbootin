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

#ifndef Phonon_GSTREAMER_VOLUMEFADEREFFECT_H
#define Phonon_GSTREAMER_VOLUMEFADEREFFECT_H

#include "medianode.h"
#include "effect.h"

#include <phonon/effectinterface.h>
#include <phonon/effectparameter.h>
#include <phonon/volumefaderinterface.h>

#include <QtCore>

#include <gst/gst.h>

QT_BEGIN_NAMESPACE

namespace Phonon
{
namespace Gstreamer
{
    class VolumeFaderEffect : public Effect, public VolumeFaderInterface
    {
        Q_OBJECT
        Q_INTERFACES(Phonon::VolumeFaderInterface)

        public:
            VolumeFaderEffect(Backend *backend, QObject *parent = 0);
            ~VolumeFaderEffect();

            GstElement* createEffectBin();
            GstElement *audioElement() { return m_effectBin; }
            bool event(QEvent *);
            void updateFade();

            // VolumeFaderInterface:
            float volume() const;
            void setVolume(float volume);
            Phonon::VolumeFaderEffect::FadeCurve fadeCurve() const;
            void setFadeCurve(Phonon::VolumeFaderEffect::FadeCurve fadeCurve);
            void fadeTo(float volume, int fadeTime);

            Phonon::VolumeFaderEffect::FadeCurve m_fadeCurve;
            int m_fadeTimer;
            int m_fadeDuration;
            float m_fadeFromVolume;
            float m_fadeToVolume;
            QTime m_fadeStartTime;
    };
}} //namespace Phonon::Gstreamer

QT_END_NAMESPACE

#endif // Phonon_GSTREAMER_VOLUMEFADEREFFECT_H
