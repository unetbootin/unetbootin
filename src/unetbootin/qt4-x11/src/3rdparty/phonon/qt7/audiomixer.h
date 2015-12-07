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

#ifndef Phonon_QT7_AUDIOMIXER_H
#define Phonon_QT7_AUDIOMIXER_H

#include <QtCore>
#include <phonon/effectinterface.h>
#include <phonon/effectparameter.h>
#include <phonon/volumefaderinterface.h>
#include "medianode.h"
#include "audionode.h"

QT_BEGIN_NAMESPACE

namespace Phonon
{
namespace QT7
{
    class AudioMixerAudioNode : public AudioNode
    {
        public:
            AudioMixerAudioNode();
            void setVolume(float volume, int bus = 0);
            float volume(int bus = 0);

        protected:
            ComponentDescription getAudioNodeDescription() const;
            void initializeAudioUnit();

        private:
            friend class AudioMixer;
            int m_numberOfBusses;
            float m_volume;
    };

    class AudioMixer : public MediaNode, Phonon::EffectInterface, Phonon::VolumeFaderInterface
    {
        Q_OBJECT
        Q_INTERFACES(Phonon::EffectInterface Phonon::VolumeFaderInterface)

        public:
            AudioMixer(QObject *parent = 0);
            ~AudioMixer();
            AudioMixerAudioNode *m_audioNode;
            Phonon::VolumeFaderEffect::FadeCurve m_fadeCurve;

            int m_fadeTimer;
            int m_fadeDuration;
            float m_fadeToVolume;
            float m_fadeFromVolume;
            QTime m_fadeStartTime;
            
            // EffectInterface:
            QList<Phonon::EffectParameter> parameters() const;
            QVariant parameterValue(const Phonon::EffectParameter &parameter) const;
            void setParameterValue(const Phonon::EffectParameter &parameter, const QVariant &newValue);

            // VolumeFaderInterface:
            float volume() const;
            void setVolume(float volume);
            Phonon::VolumeFaderEffect::FadeCurve fadeCurve() const;
            void setFadeCurve(Phonon::VolumeFaderEffect::FadeCurve fadeCurve);
            void fadeTo(float volume, int fadeTime);
            void updateFade();

        protected:
            bool event(QEvent *event);
    };

}} //namespace Phonon::QT7

QT_END_NAMESPACE

#endif // Phonon_QT7_AUDIOMIXER_H
