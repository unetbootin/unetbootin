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

#ifndef PHONON_VOLUMEFADEREFFECT_H
#define PHONON_VOLUMEFADEREFFECT_H

#include "phonon_export.h"
#include "effect.h"

QT_BEGIN_HEADER
QT_BEGIN_NAMESPACE

#ifndef QT_NO_PHONON_VOLUMEFADEREFFECT

namespace Phonon
{
    class VolumeFaderEffectPrivate;

    /** \class VolumeFaderEffect volumefadereffect.h Phonon/VolumeFaderEffect
     * Audio effect to gradually fade the audio volume.
     *
     * This effect differs from gradually changing the output volume in that
     * a dedicated effect can change the volume in the smallest possible
     * steps while every other volume control will make more or less
     * noticeable steps.
     *
     * \ingroup PhononEffects
     * \author Matthias Kretz <kretz@kde.org>
     * \see AudioOutput::volume
     */
    class PHONON_EXPORT VolumeFaderEffect : public Effect
    {
        Q_OBJECT
        K_DECLARE_PRIVATE(VolumeFaderEffect)
        PHONON_HEIR(VolumeFaderEffect)
        Q_ENUMS(FadeCurve)
        /**
         * This is the current volume of the output as voltage factor.
         * Setting this property changes the volume immediately.
         *
         * 1.0 means 100%, 0.5 means 50% voltage/25% power, 0.0 means 0%
         *
         * \see volumeDecibel
         */
        Q_PROPERTY(float volume READ volume WRITE setVolume)
        /**
         * This is the current volume of the output in decibel.
         * Setting this property changes the volume immediately.
         *
         * 0 dB means no change in volume, -6dB means an attenuation of the
         * voltage to 50% and an attenuation of the power to 25%, -inf dB means
         * silence.
         *
         * \see volume
         */
        Q_PROPERTY(double volumeDecibel READ volumeDecibel WRITE setVolumeDecibel)
        /**
         * This property holds the fade curve to be used for the fadeIn(), fadeOut()
         * and fadeTo() slots.
         *
         * Defaults to Fade3Decibel.
         *
         * \see FadeCurve
         */
        Q_PROPERTY(FadeCurve fadeCurve READ fadeCurve WRITE setFadeCurve)
        public:
            /**
             * Determines the curve of the volume change.
             */
            enum FadeCurve {
                /**
                 * "Crossfade curve" / "fast" fade out
                 *
                 * Often the best fade for a crossfade, as after half of the
                 * time the volume reached -3dB. This means that half the
                 * possible power (which is proportional to the square of the
                 * voltage) is reached. Summed, the maximum power of two audio
                 * signals fading with a -3dB curve will always be equal.
                 *
                 * For fading in or out the -3dB curve is too abrupt in the end.
                 *
                 * This is the default fade curve.
                 */
                Fade3Decibel,
                /**
                 * "Linear" fade out
                 *
                 * With a -6dB fade curve after half of the fading time -6dB has
                 * been reached. -6dB is equal to half of the voltage meaning
                 * that the voltage multiplier changes linearly from the start
                 * of the fade to the end.
                 */
                Fade6Decibel,
                /**
                 * "slow" fade out
                 *
                 * After half of the fade time -9dB are reached. So the fade is
                 * fast in the beginning and slow in the end. This is a good
                 * fade for ending music.
                 */
                Fade9Decibel,
                /**
                 * more extreme version of the -9dB fade
                 */
                Fade12Decibel
            };

            float volume() const;
            double volumeDecibel() const;

            FadeCurve fadeCurve() const;

        public Q_SLOTS:
            /**
             * Tells the Fader to change the volume from the current volume to 100%
             * in \p fadeTime milliseconds.
             * Short for \c fadeTo(1.0, fadeTime).
             *
             * \param fadeTime the fade duration in milliseconds
             *
             * \see fadeTo
             * \see volume
             */
            void fadeIn(int fadeTime);

            /**
             * Tells the Fader to change the volume from the current volume to 0%
             * in \p fadeTime milliseconds.
             * Short for \c fadeTo(0.0, fadeTime).
             *
             * \param fadeTime the fade duration in milliseconds
             *
             * \see fadeTo
             */
            void fadeOut(int fadeTime);

            void setVolume(float volume);
            void setVolumeDecibel(double volumeDecibel);

            void setFadeCurve(FadeCurve curve);

            /**
             * Tells the Fader to change the volume from the current value to
             * \p volume in \p fadeTime milliseconds
             *
             * \see fadeIn
             * \see fadeOut
             */
            void fadeTo(float volume, int fadeTime);
    };
} //namespace Phonon

#endif //QT_NO_PHONON_VOLUMEFADEREFFECT

QT_END_NAMESPACE
QT_END_HEADER

// vim: sw=4 ts=4 tw=80
#endif // PHONON_VOLUMEFADEREFFECT_H
