/*  This file is part of the KDE project
    Copyright (C) 2007 Matthias Kretz <kretz@kde.org>

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

#ifndef PHONON_MEDIASOURCE_H
#define PHONON_MEDIASOURCE_H

#include "phonon_export.h"
#include "phononnamespace.h"
#include "objectdescription.h"
#include <QtCore/QSharedData>
#include <QtCore/QString>

QT_BEGIN_HEADER
QT_BEGIN_NAMESPACE

class QUrl;
class QIODevice;

namespace Phonon
{

class MediaSourcePrivate;
class AbstractMediaStream;

/** \class MediaSource mediasource.h Phonon/MediaSource
 * Note that all constructors of this class are implicit, so that you can simply write
 * \code
 * MediaObject m;
 * QString fileName("/home/foo/bar.ogg");
 * QUrl url("http://www.example.com/stream.mp3");
 * QBuffer *someBuffer;
 * m.setCurrentSource(fileName);
 * m.setCurrentSource(url);
 * m.setCurrentSource(someBuffer);
 * m.setCurrentSource(Phonon::Cd);
 * \endcode
 *
 * \ingroup Playback
 * \ingroup Recording
 * \author Matthias Kretz <kretz@kde.org>
 */
class PHONON_EXPORT MediaSource
{
    friend class StreamInterface;
    public:
        /**
         * Identifies the type of media described by the MediaSource object.
         *
         * \see MediaSource::type()
         */
        enum Type {
            /**
             * The MediaSource object does not describe any valid source.
             */
            Invalid = -1,
            /**
             * The MediaSource object describes a local file.
             */
            LocalFile,
            /**
             * The MediaSource object describes a URL, which can be both a local file and a file on
             * the network.
             */
            Url,
            /**
             * The MediaSource object describes a disc.
             */
            Disc,
            /**
             * The MediaSource object describes a data stream.
             *
             * This is also the type used for QIODevices.
             *
             * \see AbstractMediaStream
             */
            Stream,
            /**
             * An empty MediaSource.
             *
             * It can be used to unload the current media from a MediaObject.
             *
             * \see MediaSource()
             */
            Empty
/*          post 4.0:
            / **
             * Links multiple MediaSource objects together.
             * /
            Link
*/
        };

        /**
         * Creates an empty MediaSource.
         *
         * An empty MediaSource is considered valid and can be set on a MediaObject to unload its
         * current media.
         *
         * \see Empty
         */
        MediaSource();

        /**
         * Creates a MediaSource object for a local file or a Qt resource.
         *
         * \param fileName file name of a local media file or a Qt resource that was compiled in.
         */
        MediaSource(const QString &fileName); //krazy:exclude=explicit

        /**
         * Creates a MediaSource object for a URL.
         *
         * \param url URL to a media file or stream.
         */
        MediaSource(const QUrl &url); //krazy:exclude=explicit

        /**
         * Creates a MediaSource object for discs.
         *
         * \param discType See \ref DiscType
         * \param deviceName A platform dependent device name. This can be useful if the computer
         * has more than one CD drive. It is recommended to use Solid to retrieve the device name in
         * a portable way.
         */
        MediaSource(Phonon::DiscType discType, const QString &deviceName = QString()); //krazy:exclude=explicit

#ifndef QT_NO_PHONON_ABSTRACTMEDIASTREAM
        /**
         * Creates a MediaSource object for a data stream.
         *
         * Your application can provide the media data by subclassing AbstractMediaStream and
         * passing a pointer to that object. %Phonon will never delete the \p stream.
         *
         * \param stream The AbstractMediaStream subclass to provide the media data.
         *
         * \see setAutoDelete
         */
        MediaSource(AbstractMediaStream *stream); //krazy:exclude=explicit

        /**
         * Creates a MediaSource object for a QIODevice.
         *
         * This constructor can be very handy in the combination of QByteArray and QBuffer.
         *
         * \param ioDevice An arbitrary readable QIODevice subclass. If the device is not opened
         * MediaSource will open it as QIODevice::ReadOnly. Sequential I/O devices are possible,
         * too. For those MediaObject::isSeekable() will have to return false obviously.
         *
         * \see setAutoDelete
         */
        MediaSource(QIODevice *ioDevice); //krazy:exclude=explicit
#endif

        /**
         * Creates a MediaSource object for capture devices.
         */
        //MediaSource(const AudioCaptureDevice &, const VideoCaptureDevice &);

        /**
         * Destroys the MediaSource object.
         */
        ~MediaSource();

        /**
         * Constructs a copy of \p rhs.
         *
         * This constructor is fast thanks to explicit sharing.
         */
        MediaSource(const MediaSource &rhs);

        /**
         * Assigns \p rhs to this MediaSource and returns a reference to this MediaSource.
         *
         * This operation is fast thanks to explicit sharing.
         */
        MediaSource &operator=(const MediaSource &rhs);

        /**
         * Returns \p true if this MediaSource is equal to \p rhs; otherwise returns \p false.
         */
        bool operator==(const MediaSource &rhs) const;

        /**
         * Tell the MediaSource to take ownership of the AbstractMediaStream or QIODevice that was
         * passed in the constructor.
         *
         * The default setting is \p false, for safety. If you turn it on, you should only access
         * the AbstractMediaStream/QIODevice object as long as you yourself keep a MediaSource
         * object around. As long as you keep the MediaSource object wrapping the stream/device
         * the object will not get deleted.
         *
         * \see autoDelete
         */
        void setAutoDelete(bool enable);

        /**
         * Returns the setting of the auto-delete option. The default is \p false.
         *
         * \see setAutoDelete
         */
        bool autoDelete() const;

        /**
         * Returns the type of the MediaSource (depends on the constructor that was used).
         *
         * \see Type
         */
        Type type() const;

        /**
         * Returns the file name of the MediaSource if type() == LocalFile; otherwise returns
         * QString().
         */
        QString fileName() const;

        /**
         * Returns the url of the MediaSource if type() == URL or type() == LocalFile; otherwise
         * returns QUrl().
         */
        QUrl url() const;

        /**
         * Returns the disc type of the MediaSource if type() == Disc; otherwise returns \ref
         * NoDisc.
         */
        Phonon::DiscType discType() const;

        /**
         * Returns the device name of the MediaSource if type() == Disc; otherwise returns
         * QString().
         */
        QString deviceName() const;

#ifndef QT_NO_PHONON_ABSTRACTMEDIASTREAM
        /**
         * Returns the media stream of the MediaSource if type() == Stream; otherwise returns 0.
         * QIODevices are handled as streams, too.
         */
        AbstractMediaStream *stream() const;
#endif

        //AudioCaptureDevice audioCaptureDevice() const;
        //VideoCaptureDevice videoCaptureDevice() const;

/*      post 4.0:
        MediaSource(const QList<MediaSource> &mediaList);
        QList<MediaSource> substreams() const;
*/

    protected:
        QExplicitlySharedDataPointer<MediaSourcePrivate> d;
        MediaSource(MediaSourcePrivate &);
};

} // namespace Phonon

QT_END_NAMESPACE
QT_END_HEADER

#endif // PHONON_MEDIASOURCE_H
