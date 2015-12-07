/*  This file is part of the KDE project
    Copyright (C) 2005-2006 Matthias Kretz <kretz@kde.org>

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
#ifndef Phonon_AUDIOOUTPUT_H
#define Phonon_AUDIOOUTPUT_H

#include "phonon_export.h"
#include "abstractaudiooutput.h"
#include "phonondefs.h"
#include "phononnamespace.h"
#include "objectdescription.h"

QT_BEGIN_HEADER
QT_BEGIN_NAMESPACE

class QString;

class AudioOutputAdaptor;
namespace Phonon
{
    class AudioOutputPrivate;

    /** \class AudioOutput audiooutput.h Phonon/AudioOutput
     * \short Class for audio output to the soundcard.
     *
     * Use this class to define the audio output.
     *
     * \ingroup Frontend
     * \author Matthias Kretz <kretz@kde.org>
     * \see Phonon::Ui::VolumeSlider
     */
    class PHONON_EXPORT AudioOutput : public AbstractAudioOutput
    {
        friend class FactoryPrivate;
        friend class ::AudioOutputAdaptor;
        Q_OBJECT
        K_DECLARE_PRIVATE(AudioOutput)
        /**
         * This is the name that appears in Mixer applications that control
         * the volume of this output.
         *
         * \see category
         */
        Q_PROPERTY(QString name READ name WRITE setName)
        /**
         * This is the current loudness of the output (it is using Stevens' law
         * to calculate the change in voltage internally).
         *
         * \see volumeDecibel
         */
        Q_PROPERTY(qreal volume READ volume WRITE setVolume NOTIFY volumeChanged)
        /**
         * This is the current volume of the output in decibel.
         *
         * 0 dB means no change in volume, -6dB means an attenuation of the
         * voltage to 50% and an attenuation of the power to 25%, -inf dB means
         * silence.
         *
         * \see volume
         */
        Q_PROPERTY(qreal volumeDecibel READ volumeDecibel WRITE setVolumeDecibel)
        /**
         * This property holds the (hardware) destination for the output.
         *
         * The default device is determined by the category and the global
         * configuration for that category of outputs. Normally you don't need
         * to override this setting - letting the user change the global
         * configuration is the right choice. You can still override the
         * device though, if you have good reasons to do so.
         *
         * \see outputDeviceChanged
         */
        Q_PROPERTY(AudioOutputDevice outputDevice READ outputDevice WRITE setOutputDevice)

        /**
         * This property tells whether the output is muted.
         *
         * Muting the output has the same effect as calling setVolume(0.0).
         */
        Q_PROPERTY(bool muted READ isMuted WRITE setMuted NOTIFY mutedChanged)
        public:
            /**
             * Creates a new AudioOutput that defines output to a physical
             * device.
             *
             * \param category The category can be used by mixer applications to group volume
             * controls of applications into categories. That makes it easier for
             * the user to identify the programs.
             * The category is also used for the default output device that is
             * configured centrally. As an example: often users want to have the
             * audio signal of a VoIP application go to their USB headset while
             * all other sounds should go to the internal soundcard.
             *
             * \param parent QObject parent
             *
             * \see Phonon::categoryToString
             * \see outputDevice
             */
            explicit AudioOutput(Phonon::Category category, QObject *parent = 0);
            explicit AudioOutput(QObject *parent = 0);

            QString name() const;
            qreal volume() const;
            qreal volumeDecibel() const;

            /**
             * Returns the category of this output.
             *
             * \see AudioOutput(Phonon::Category, QObject *)
             */
            Phonon::Category category() const;
            AudioOutputDevice outputDevice() const;
            bool isMuted() const;

        public Q_SLOTS:
            void setName(const QString &newName);
            void setVolume(qreal newVolume);
            void setVolumeDecibel(qreal newVolumeDecibel);
            bool setOutputDevice(const Phonon::AudioOutputDevice &newAudioOutputDevice);
            void setMuted(bool mute);

        Q_SIGNALS:
            /**
             * This signal is emitted whenever the volume has changed. As the
             * volume can change without a call to setVolume (calls over dbus)
             * this is important
             * to keep a widget showing the current volume up to date.
             */
            void volumeChanged(qreal newVolume);

            /**
             * This signal is emitted when the muted property has changed. As
             * this property can change by IPC (DBus) calls a UI element showing
             * the muted property should listen to this signal.
             */
            void mutedChanged(bool);

            /**
             * This signal is emitted when the (hardware) device for the output
             * has changed.
             *
             * The change can happen either through setOutputDevice or if the
             * global configuration for the used category has changed.
             *
             * \see outputDevice
             */
            void outputDeviceChanged(const Phonon::AudioOutputDevice &newAudioOutputDevice);

        private:
            Q_PRIVATE_SLOT(k_func(), void _k_volumeChanged(qreal))
            Q_PRIVATE_SLOT(k_func(), void _k_revertFallback())
            Q_PRIVATE_SLOT(k_func(), void _k_audioDeviceFailed())
            Q_PRIVATE_SLOT(k_func(), void _k_deviceListChanged())
    };
} //namespace Phonon

QT_END_NAMESPACE
QT_END_HEADER

// vim: sw=4 ts=4 tw=80
#endif // Phonon_AUDIOOUTPUT_H
