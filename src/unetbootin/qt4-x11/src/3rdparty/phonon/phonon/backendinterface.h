/*  This file is part of the KDE project
    Copyright (C) 2006-2007 Matthias Kretz <kretz@kde.org>

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

#ifndef PHONON_BACKENDINTERFACE_H
#define PHONON_BACKENDINTERFACE_H

#include "phonon_export.h"
#include "objectdescription.h"

#include <QtCore/QtGlobal>
#include <QtCore/QSet>

QT_BEGIN_HEADER
QT_BEGIN_NAMESPACE

class QVariant;

namespace Phonon
{

/** \class BackendInterface backendinterface.h Phonon/BackendInterface
 * \short Main Backend class interface
 *
 * This interface defines the main factory of the backend. The createObject function creates all the
 * objects needed by the frontend.
 *
 * The objectDescriptionIndexes and objectDescriptionProperties functions return information about
 * available devices, effects and codecs.
 *
 * An implementation could look like this:
 * \code
 * QObject *Backend::createObject(BackendInterface::Class c, QObject *parent, const QList<QVariant> &args)
 * {
 *     switch (c) {
 *     case MediaObjectClass:
 *         return new MediaObject(parent);
 *     case VolumeFaderEffectClass:
 *         return new VolumeFaderEffect(parent);
 *     case AudioOutputClass:
 *         return new AudioOutput(parent);
 *     case AudioDataOutputClass:
 *         return new AudioDataOutput(parent);
 *     case VisualizationClass:
 *         return new Visualization(parent);
 *     case VideoDataOutputClass:
 *         return new VideoDataOutput(parent);
 *     case EffectClass:
 *         return new Effect(args[0].toInt(), parent);
 *     case VideoWidgetClass:
 *         return new VideoWidget(qobject_cast<QWidget *>(parent));
 *     }
 *     return 0;
 * }
 *
 * QSet<int> Backend::objectDescriptionIndexes(ObjectDescriptionType type) const
 * {
 *     QSet<int> set;
 *     switch(type)
 *     {
 *     case Phonon::AudioOutputDeviceType:
 *         // use AudioDeviceEnumerator to list ALSA and OSS devices
 *         set << 10000 << 10001;
 *         break;
 *     case Phonon::AudioCaptureDeviceType:
 *         set << 20000 << 20001;
 *         break;
 *     case Phonon::VideoOutputDeviceType:
 *         break;
 *     case Phonon::VideoCaptureDeviceType:
 *         set << 30000 << 30001;
 *         break;
 *     case Phonon::VisualizationType:
 *     case Phonon::AudioCodecType:
 *     case Phonon::VideoCodecType:
 *     case Phonon::ContainerFormatType:
 *         break;
 *     case Phonon::EffectType:
 *         set << 0x7F000001;
 *         break;
 *     }
 *     return set;
 * }
 *
 * QHash<QByteArray, QVariant> Backend::objectDescriptionProperties(ObjectDescriptionType type, int index) const
 * {
 *     QHash<QByteArray, QVariant> ret;
 *     switch (type) {
 *     case Phonon::AudioOutputDeviceType:
 *         switch (index) {
 *         case 10000:
 *             ret.insert("name", QLatin1String("internal Soundcard"));
 *             break;
 *         case 10001:
 *             ret.insert("name", QLatin1String("USB Headset"));
 *             ret.insert("icon", KIcon("usb-headset"));
 *             ret.insert("available", false);
 *             break;
 *         }
 *         break;
 *     case Phonon::AudioCaptureDeviceType:
 *         switch (index) {
 *         case 20000:
 *             ret.insert("name", QLatin1String("Soundcard"));
 *             ret.insert("description", QLatin1String("first description"));
 *             break;
 *         case 20001:
 *             ret.insert("name", QLatin1String("DV"));
 *             ret.insert("description", QLatin1String("second description"));
 *             break;
 *         }
 *         break;
 *     case Phonon::VideoOutputDeviceType:
 *         break;
 *     case Phonon::VideoCaptureDeviceType:
 *         switch (index) {
 *         case 30000:
 *             ret.insert("name", QLatin1String("USB Webcam"));
 *             ret.insert("description", QLatin1String("first description"));
 *             break;
 *         case 30001:
 *             ret.insert("name", QLatin1String("DV"));
 *             ret.insert("description", QLatin1String("second description"));
 *             break;
 *         }
 *         break;
 *     case Phonon::VisualizationType:
 *         break;
 *     case Phonon::AudioCodecType:
 *         break;
 *     case Phonon::VideoCodecType:
 *         break;
 *     case Phonon::ContainerFormatType:
 *         break;
 *     case Phonon::EffectType:
 *         switch (index) {
 *         case 0x7F000001:
 *             ret.insert("name", QLatin1String("Delay"));
 *             ret.insert("description", QLatin1String("Simple delay effect with time, feedback and level controls."));
 *             break;
 *         }
 *         break;
 *     }
 *     return ret;
 * }
 * \endcode
 *
 * \author Matthias Kretz <kretz@kde.org>
 */
class BackendInterface
{
    public:
        /**
         * \internal
         *
         * Silence gcc's warning.
         */
        virtual ~BackendInterface() {}

        /**
         * Classes that the createObject function has to handle.
         */
        enum Class {
            /**
             * Request to return a %MediaObject object.
             */
            MediaObjectClass,
            /**
             * Request to return a %VolumeFaderEffect object.
             */
            VolumeFaderEffectClass,
            /**
             * Request to return a %AudioOutput object.
             */
            AudioOutputClass,
            /**
             * Request to return a %AudioDataOutput object.
             */
            AudioDataOutputClass,
            /**
             * Request to return a %Visualization object.
             */
            VisualizationClass,
            /**
             * Request to return a %VideoDataOutput object.
             */
            VideoDataOutputClass,
            /**
             * Request to return a %Effect object.
             *
             * Takes an additional int that specifies the effect Id.
             */
            EffectClass,
            /**
             * Request to return a %VideoWidget object.
             */
            VideoWidgetClass
        };

        /**
         * Returns a new instance of the requested class.
         *
         * \param c The requested class.
         * \param parent The parent object.
         * \param args Additional arguments (documented in \ref Class).
         */
        virtual QObject *createObject(Class c, QObject *parent, const QList<QVariant> &args = QList<QVariant>()) = 0;

        /**
         * Returns the unique identifiers for the devices/effects/codecs of the given \p type.
         *
         * \param type see \ref ObjectDescriptionType
         */
        virtual QList<int> objectDescriptionIndexes(ObjectDescriptionType type) const = 0;

        /**
         * Given a unique identifier that was returned from objectDescriptionIndexes this function
         * returns a hash mapping property names to values.
         *
         * The property "name" must always be present. All other properties are optional.
         *
         * List of possible properties:
         * \li \c \b name: The name of the device/effect/codec/...
         * \li \c \b description: A text explaining what this device/effect/codec/... is/can do
         * \li \c \b icon: An icon name (using the freedesktop naming scheme) or a QIcon for this
         * device/effect/codec/...
         * \li \c \b available: A bool telling whether the device is present or unplugged.
         *
         * \param type see \ref ObjectDescriptionType
         * \param index The unique identifier that is returned from objectDescriptionIndexes
         */
        virtual QHash<QByteArray, QVariant> objectDescriptionProperties(ObjectDescriptionType type, int index) const = 0;

        /**
         * When this function is called the nodes given in the parameter list should not lose any
         * signal data when connections are changed.
         */
        virtual bool startConnectionChange(QSet<QObject *>) = 0;

        /**
         * Defines a signal connection between the two given nodes.
         */
        virtual bool connectNodes(QObject *, QObject *) = 0;

        /**
         * Cuts a signal connection between the two given nodes.
         */
        virtual bool disconnectNodes(QObject *, QObject *) = 0;

        /**
         * When this function is called the nodes given in the parameter list may lose
         * signal data when a port is not connected.
         */
        virtual bool endConnectionChange(QSet<QObject *>) = 0;

        /**
         * gets all available mime types
         */
        virtual QStringList availableMimeTypes() const = 0;

};
} // namespace Phonon

Q_DECLARE_INTERFACE(Phonon::BackendInterface, "BackendInterface3.phonon.kde.org")

QT_END_NAMESPACE
QT_END_HEADER

#endif // PHONON_BACKENDINTERFACE_H
