/*  This file is part of the KDE project.

    Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
    Copyright (C) 2008 Matthias Kretz <kretz@kde.org>

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

#ifndef Phonon_GSTREAMER_AUDIOOUTPUT_H
#define Phonon_GSTREAMER_AUDIOOUTPUT_H

#include "common.h"
#include "medianode.h"

#include <phonon/audiooutputinterface.h>
#include <phonon/phononnamespace.h>

#include <QtCore/QFile>

#include <gst/gst.h>

QT_BEGIN_NAMESPACE

namespace Phonon
{
namespace Gstreamer
{
class AudioOutput : public QObject, public AudioOutputInterface, public MediaNode
{
    Q_OBJECT
    Q_INTERFACES(Phonon::AudioOutputInterface Phonon::Gstreamer::MediaNode)
public:
    AudioOutput(Backend *backend, QObject *parent);
    ~AudioOutput();

    qreal volume() const;
    int outputDevice() const;
    void setVolume(qreal newVolume);
    bool setOutputDevice(int newDevice);
#if (PHONON_VERSION >= PHONON_VERSION_CHECK(4, 2, 0))
    bool setOutputDevice(const AudioOutputDevice &newDevice);
#endif

public:
    GstElement *audioElement()
    {
        Q_ASSERT(m_audioBin);
        return m_audioBin;
    }

    void mediaNodeEvent(const MediaNodeEvent *event);

Q_SIGNALS:
    void volumeChanged(qreal newVolume);
    void audioDeviceFailed();

private:

    qreal m_volumeLevel;
    int m_device;

    GstElement *m_volumeElement;
    GstElement *m_audioBin;
    GstElement *m_audioSink;
    GstElement *m_conv;
};
}
} //namespace Phonon::Gstreamer

QT_END_NAMESPACE

#endif // Phonon_GSTREAMER_AUDIOOUTPUT_H
