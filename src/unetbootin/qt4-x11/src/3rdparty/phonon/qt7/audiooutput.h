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

#ifndef Phonon_QT7_AUDIOOUTPUT_H
#define Phonon_QT7_AUDIOOUTPUT_H

#include <QtCore>
#include <phonon/audiooutputinterface.h>
#include <phonon/abstractaudiooutput.h>

#include "medianode.h"
#include "audionode.h"

QT_BEGIN_NAMESPACE

namespace Phonon
{
namespace QT7
{
    class AudioOutputAudioPart : public QObject, AudioNode
    {
        Q_OBJECT
        public:
            AudioOutputAudioPart();

            void setVolume(float volume);
            float volume();

        protected:
            ComponentDescription getAudioNodeDescription() const;
            void initializeAudioUnit();

        signals:
            void volumeChanged(qreal newVolume);
            void audioDeviceFailed();

        private:
            friend class AudioOutput;
            qreal m_volume;
            AudioDeviceID m_audioDevice;
            void setAudioDevice(AudioDeviceID device);
    };

    class AudioOutput : public MediaNode, public AudioOutputInterface
    {
        Q_OBJECT
        Q_INTERFACES(Phonon::AudioOutputInterface)

        public:
            AudioOutput(QObject *parent = 0);
            ~AudioOutput();

            qreal volume() const;
            void setVolume(qreal);
            int outputDevice() const;
            bool setOutputDevice(int);

        signals:
            void volumeChanged(qreal newVolume);
            void audioDeviceFailed();

        protected:
            void mediaNodeEvent(const MediaNodeEvent *event);
            
        private:
            AudioOutputAudioPart *m_audioOutput;
            int m_device;
            bool m_redirectToMovie;
    };

}} //namespace Phonon::QT7

QT_END_NAMESPACE
#endif // Phonon_QT7_AUDIOOUTPUT_H
