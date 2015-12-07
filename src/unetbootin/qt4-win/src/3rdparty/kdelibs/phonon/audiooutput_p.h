/*  This file is part of the KDE project
    Copyright (C) 2006 Matthias Kretz <kretz@kde.org>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License version 2 as published by the Free Software Foundation.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.

*/

#ifndef AUDIOOUTPUT_P_H
#define AUDIOOUTPUT_P_H

#include "audiooutput.h"
#include "abstractaudiooutput_p.h"
#include "platform_p.h"

QT_BEGIN_NAMESPACE

namespace Phonon
{
class AudioOutputPrivate : public AbstractAudioOutputPrivate
{
    Q_DECLARE_PUBLIC(AudioOutput)
    PHONON_PRIVATECLASS
    public:
        inline static AudioOutputPrivate *cast(MediaNodePrivate *x)
        {
            if (x && x->castId == MediaNodePrivate::AudioOutputType) {
                return static_cast<AudioOutputPrivate *>(x);
            }
            return 0;
        }
        void init(Phonon::Category c);


    protected:
        AudioOutputPrivate(CastId castId = MediaNodePrivate::AudioOutputType)
            : AbstractAudioOutputPrivate(castId),
            volume(1.0),
            outputDeviceIndex(-1),
            deviceBeforeFallback(-1),
            outputDeviceOverridden(false),
            muted(false)
        {
            name = Platform::applicationName();
            volume = Platform::loadVolume(name);
        }

        enum DeviceChangeType {
            FallbackChange,
            HigherPreferenceChange
        };
        void handleAutomaticDeviceChange(int newIndex, DeviceChangeType type);

        void _k_volumeChanged(qreal);
        void _k_revertFallback();
        void _k_audioDeviceFailed();
        void _k_deviceListChanged();

    private:
        qreal volume;
        Category category;
        QString name;
        int outputDeviceIndex;
        int deviceBeforeFallback;
        bool outputDeviceOverridden;
        bool muted;
};
} //namespace Phonon

QT_END_NAMESPACE

#endif // AUDIOOUTPUT_P_H
// vim: sw=4 ts=4 tw=80
