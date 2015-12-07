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

#ifndef PHONON_STREAMINTERFACE_H
#define PHONON_STREAMINTERFACE_H

#include "phonon_export.h"
#include <QtCore/QObject>

QT_BEGIN_HEADER
QT_BEGIN_NAMESPACE

#ifndef QT_NO_PHONON_ABSTRACTMEDIASTREAM

namespace Phonon
{
class StreamInterfacePrivate;
class MediaSource;

/** \class StreamInterface streaminterface.h Phonon/StreamInterface
 * \brief Backend interface to handle media streams (AbstractMediaStream).
 *
 * \author Matthias Kretz <kretz@kde.org>
 */
class PHONON_EXPORT StreamInterface
{
    friend class StreamInterfacePrivate;
    friend class AbstractMediaStreamPrivate;
    public:
        virtual ~StreamInterface();
        /**
         * Called by the application to send a chunk of (encoded) media data.
         *
         * It is recommended to keep the QByteArray object until the data is consumed so that no
         * memcopy is needed.
         */
        virtual void writeData(const QByteArray &data) = 0;
        /**
         * Called when no more media data is available and writeData will not be called anymore.
         */
        virtual void endOfData() = 0;
        /**
         * Called at the start of the stream to tell how many bytes will be sent through writeData
         * (if no seeks happen, of course). If this value is negative the stream size cannot be
         * determined (might be a "theoretically infinite" stream - like webradio).
         */
        virtual void setStreamSize(qint64 newSize) = 0;
        /**
         * Tells whether the stream is seekable.
         */
        virtual void setStreamSeekable(bool s) = 0;

        /**
         * Call this function from the constructor of your StreamInterface implementation (or as
         * soon as you get the MediaSource object). This will connect your object to the
         * AbstractMediaStream object. Only after the connection is done will the following
         * functions have an effect.
         */
        void connectToSource(const MediaSource &mediaSource);

        /**
         * Call this function to tell the AbstractMediaStream that you need more data. The data will
         * arrive through writeData. Don't rely on writeData getting called from needData, though
         * some AbstractMediaStream implementations might do so.
         *
         * Depending on the buffering you need you either treat needData as a replacement for a
         * read call like QIODevice::read, or you start calling needData whenever your buffer
         * reaches a certain lower threshold.
         */
        void needData();

        /**
         * Call this function to tell the AbstractMediaStream that you have enough data in your
         * buffer and that it should pause calling writeData if possible.
         */
        void enoughData();

        /**
         * If the stream is seekable, calling this function will make the next call to writeData
         * pass data that starts at the byte offset \p seekTo.
         */
        void seekStream(qint64 seekTo);

        /**
         * Resets the AbstractMediaStream. E.g. this can be useful for non-seekable streams to start
         * over again.
         */
        void reset();

    protected:
        StreamInterface();

        StreamInterfacePrivate *const d;
};
} // namespace Phonon

Q_DECLARE_INTERFACE(Phonon::StreamInterface, "StreamInterface1.phonon.kde.org")

#endif //QT_NO_PHONON_ABSTRACTMEDIASTREAM

QT_END_NAMESPACE
QT_END_HEADER

#endif // PHONON_STREAMINTERFACE_H
