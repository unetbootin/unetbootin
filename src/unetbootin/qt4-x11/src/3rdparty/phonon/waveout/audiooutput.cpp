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

#include "audiooutput.h"
#include "mediaobject.h"

#include <QtCore/QVector>

#include <cmath>

QT_BEGIN_NAMESPACE

namespace Phonon
{
    namespace WaveOut
    {
        AudioOutput::AudioOutput(Backend *, QObject *parent)
        {
            setParent(parent);
            m_volume = 0xffff;
        }

        AudioOutput::~AudioOutput()
        {
        }

        int AudioOutput::outputDevice() const
        {
            return 0;
        }

        void AudioOutput::setVolume(qreal newVolume)
        {
            m_volume = newVolume;
            emit volumeChanged(newVolume);
        }

        void AudioOutput::setCrossFadingProgress(short currentIndex, qreal progress)
        {
            Q_UNUSED(currentIndex);
            Q_UNUSED(progress);
        }

        bool AudioOutput::setOutputDevice(const AudioOutputDevice & newDevice)
        {
            return setOutputDevice(newDevice.index());
        }

        qreal AudioOutput::volume() const
        {
            return m_volume;
        }

        bool AudioOutput::setOutputDevice(int newDevice)
        {
         
            return (newDevice == 0);
        }

    }
}

QT_END_NAMESPACE

