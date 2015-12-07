/*  This file is part of the KDE project
    Copyright (C) 2005-2006 Matthias Kretz <kretz@kde.org>

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

#ifndef Phonon_BACKENDCAPABILITIES_H
#define Phonon_BACKENDCAPABILITIES_H

#include "phonon/phonon_export.h"
#include "phonon/objectdescription.h"

#include <QtCore/QObject>

QT_BEGIN_HEADER
QT_BEGIN_NAMESPACE

template<class T> class QList;
class QStringList;

namespace Phonon
{

/**
 * Collection of functions describing the capabilities of the Backend.
 *
 * \ingroup BackendInformation
 * \author Matthias Kretz <kretz@kde.org>
 */
namespace BackendCapabilities
{
    /** \class Notifier backendcapabilities.h Phonon/BackendCapabilities
     * Notifications about backend capabilities.
     *
     * \ingroup BackendInformation
     */
    class Notifier : public QObject
    {
        Q_OBJECT
        Q_SIGNALS:
            /**
             * This signal is emitted if the capabilities have changed. This can
             * happen if the user has requested a backend change.
             */
            void capabilitiesChanged();

            /**
             * This signal is emitted when audio output devices were plugged or
             * unplugged.
             *
             * Check BackendCapabilities::availableAudioOutputDevices to get the
             * current list of available devices.
             */
            void availableAudioOutputDevicesChanged();
    };

    /**
     * Use this function to get a QObject pointer to connect to the capabilitiesChanged signal.
     *
     * \return a pointer to a QObject.
     *
     * The capabilitiesChanged signal is emitted if the capabilities have changed. This can
     * happen if the user has requested a backend change.
     *
     * To connect to this signal do the following:
     * \code
     * QObject::connect(BackendCapabilities::notifier(), SIGNAL(capabilitiesChanged()), ...
     * \endcode
     *
     * \see Notifier::capabilitiesChanged()
     */
    PHONON_EXPORT Notifier *notifier();

//X     /**
//X      * Tells whether the backend is audio only or can handle video files
//X      * and display of videos.
//X      *
//X      * If the backend does not support video all AbstractVideoOutput subclasses won't
//X      * do anything.
//X      */
//X     PHONON_EXPORT bool supportsVideo();
//X
//X     /**
//X      * Tells whether the backend implements the OSD interfaces.
//X      *
//X      * \todo there's no interfaces for it at this point
//X      */
//X     PHONON_EXPORT bool supportsOSD();
//X
//X     /**
//X      * Tells whether the backend supports subtitle rendering on the
//X      * video output.
//X      *
//X      * \todo there's no interfaces for it at this point
//X      */
//X     PHONON_EXPORT bool supportsSubtitles();

    /**
     * Returns a list of mime types that the Backend can decode.
     *
     * \see isMimeTypeAvailable()
     */
    PHONON_EXPORT QStringList availableMimeTypes();

    /**
     * Often all you want to know is whether one given MIME type can be
     * decoded by the backend. Use this method in favor of availableMimeTypes()
     * as it can give you a negative answer without having a backend loaded.
     *
     * \see availableMimeTypes();
     */
    PHONON_EXPORT bool isMimeTypeAvailable(const QString &mimeType);

#if 0
    /**
     * Returns the audio output devices the backend reports as usable. That
     * doesn't have to say the device will certainly work as the backend
     * might not be able to open the device if it's blocked by another
     * application. But at least the device is physically available.
     *
     * \return A list of AudioOutputDevice objects that give a name and
     * description for every supported audio output device.
     *
     * \see knownAudioOutputDevices
     */
    PHONON_EXPORT QList<AudioOutputDevice> usableAudioOutputDevices();

    /**
     * Returns the audio output devices the backend has ever reported as
     * usable.
     *
     * \return A list of AudioOutputDevice objects that give a name and
     * description for every supported audio output device.
     *
     * \see usableAudioOutputDevices
     */
    PHONON_EXPORT QList<AudioOutputDevice> knownAudioOutputDevices();
#endif
    /**
     * Returns the audio output devices the backend supports.
     *
     * \return A list of AudioOutputDevice objects that give a name and
     * description for every supported audio output device.
     */
    PHONON_EXPORT QList<AudioOutputDevice> availableAudioOutputDevices();

    /**
     * Returns the audio capture devices the backend supports.
     *
     * \return A list of AudioCaptureDevice objects that give a name and
     * description for every supported audio capture device.
     */
//    PHONON_EXPORT QList<AudioCaptureDevice> availableAudioCaptureDevices();

    /**
     * Returns the video output devices the backend supports.
     *
     * \return A list of VideoOutputDevice objects that give a name and
     * description for every supported video output device.
     */
//    PHONON_EXPORT QList<VideoOutputDevice> availableVideoOutputDevices();

    /**
     * Returns the video capture devices the backend supports.
     *
     * \return A list of VideoCaptureDevice objects that give a name and
     * description for every supported video capture device.
     */
//    PHONON_EXPORT QList<VideoCaptureDevice> availableVideoCaptureDevices();

    /**
     * Returns the visualization effects the backend supports.
     *
     * \return A list of VisualizationEffect objects that give a name and
     * description for every supported visualization effect.
     */
//    PHONON_EXPORT QList<VisualizationDescription> availableVisualizations();

    /**
     * Returns descriptions for the audio effects the backend supports.
     *
     * \return A list of AudioEffectDescription objects that give a name and
     * description for every supported audio effect.
     */
    PHONON_EXPORT QList<EffectDescription> availableAudioEffects();

//X     /**
//X      * Returns descriptions for the video effects the backend supports.
//X      *
//X      * \return A list of VideoEffectDescription objects that give a name and
//X      * description for every supported video effect.
//X      */
//X     PHONON_EXPORT QList<EffectDescription> availableVideoEffects();

    /**
     * Returns descriptions for the audio codecs the backend supports.
     *
     * \return A list of AudioCodec objects that give a name and
     * description for every supported audio codec.
     */
//    PHONON_EXPORT QList<AudioCodecDescription> availableAudioCodecs();

    /**
     * Returns descriptions for the video codecs the backend supports.
     *
     * \return A list of VideoCodec objects that give a name and
     * description for every supported video codec.
     */
//    PHONON_EXPORT QList<VideoCodecDescription> availableVideoCodecs();

    /**
     * Returns descriptions for the container formats the backend supports.
     *
     * \return A list of ContainerFormat objects that give a name and
     * description for every supported container format.
     */
//    PHONON_EXPORT QList<ContainerFormatDescription> availableContainerFormats();
} // namespace BackendCapabilities
} // namespace Phonon

QT_END_NAMESPACE
QT_END_HEADER

#endif // Phonon_BACKENDCAPABILITIES_H
// vim: sw=4 ts=4 tw=80
