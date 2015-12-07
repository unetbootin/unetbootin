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

#ifndef AUDIOOUTPUT_P_H
#define AUDIOOUTPUT_P_H

#include "audiooutput.h"
#include "abstractaudiooutput_p.h"
#include "platform_p.h"

QT_BEGIN_NAMESPACE

namespace Phonon
{
class AudioOutputAdaptor;

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
            name(Platform::applicationName()),
            volume(Platform::loadVolume(name)),
#ifndef QT_NO_DBUS
            adaptor(0),
#endif
            deviceBeforeFallback(-1),
            outputDeviceOverridden(false),
            muted(false)
        {
        }

        ~AudioOutputPrivate();

        enum DeviceChangeType {
            FallbackChange,
            HigherPreferenceChange
        };
        void handleAutomaticDeviceChange(const AudioOutputDevice &newDev, DeviceChangeType type);

        void _k_volumeChanged(qreal);
        void _k_revertFallback();
        void _k_audioDeviceFailed();
        void _k_deviceListChanged();

    private:
        QString name;
        Phonon::AudioOutputDevice device;
        qreal volume;
#ifndef QT_NO_DBUS
        Phonon::AudioOutputAdaptor *adaptor;
#endif
        Category category;
        int deviceBeforeFallback;
        bool outputDeviceOverridden;
        bool muted;
};
} //namespace Phonon

QT_END_NAMESPACE

#endif // AUDIOOUTPUT_P_H
// vim: sw=4 ts=4 tw=80
