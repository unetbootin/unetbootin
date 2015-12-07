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

#ifndef PHONON_ABSTRACTMEDIASTREAM_H
#define PHONON_ABSTRACTMEDIASTREAM_H

#include "phonon_export.h"
#include "phononnamespace.h"
#include <QtCore/QObject>

QT_BEGIN_HEADER
QT_BEGIN_NAMESPACE

class QByteArray;

#ifndef QT_NO_PHONON_ABSTRACTMEDIASTREAM

namespace Phonon
{
class MediaObject;
class AbstractMediaStreamPrivate;

/** \class AbstractMediaStream abstractmediastream.h Phonon/AbstractMediaStream
 * \brief Base class for custom media data streams.
 *
 * Implement this class to provide a custom data stream to the backend. The class supports both, the
 * push and the pull model.
 *
 * Push:
 * \code
 * PushStream::PushStream(QObject *parent)
 *   : AbstractMediaStream(parent), m_timer(new QTimer(this))
 * {
 *   setStreamSize(getMediaStreamSize());
 *
 *   connect(m_timer, SIGNAL(timeout()), SLOT(moreData()));
 *   m_timer->setInterval(0);
 * }
 *
 * void PushStream::moreData()
 * {
 *   const QByteArray data = getMediaData();
 *   if (data.isEmpty()) {
 *     endOfData();
 *   } else {
 *     writeData(data);
 *   }
 * }
 *
 * void PushStream::needData()
 * {
 *   m_timer->start();
 *   moreData();
 * }
 *
 * void PushStream::enoughData()
 * {
 *   m_timer->stop();
 * }
 * \endcode
 *
 * Pull:
 * \code
 * PullStream::PullStream(QObject *parent)
 *   : AbstractMediaStream(parent)
 * {
 *   setStreamSize(getMediaStreamSize());
 * }
 *
 * void PullStream::needData()
 * {
 *   const QByteArray data = getMediaData();
 *   if (data.isEmpty()) {
 *     endOfData();
 *   } else {
 *     writeData(data);
 *   }
 * }
 * \endcode
 *
 * \ingroup Playback
 * \author Matthias Kretz <kretz@kde.org>
 */
class PHONON_EXPORT AbstractMediaStream : public QObject
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(AbstractMediaStream)
    friend class MediaObject;
    friend class MediaObjectPrivate;
    friend class StreamInterface;
    public:
        virtual ~AbstractMediaStream();

    protected:
        /**
         * Constructs an AbstractMediaStream object with a \p parent.
         */
        explicit AbstractMediaStream(QObject *parent = 0);

        /**
         * Returns the stream size that was set with \ref setStreamSize.
         *
         * A negative value means that the length of the stream cannot be known.
         *
         * Defaults to \c 0.
         */
        qint64 streamSize() const;

        /**
         * Sets the size of the stream in number of bytes.
         *
         * A negative value means that the length of the stream cannot be known.
         *
         * Defaults to 0.
         *
         * This function has to be called. A backend will not call \ref needData() until the
         * stream size is set.
         */
        void setStreamSize(qint64);

        /**
         * Returns whether your data stream is set as seekable.
         *
         * Defaults to \c false.
         */
        bool streamSeekable() const;

        /**
         * Sets whether your data stream is seekable.
         *
         * Defaults to \c false.
         *
         * If you set this to \c true you have to implement the \ref seekStream function.
         */
        void setStreamSeekable(bool);

        /**
         * Sends the media \p data to the backend for decoding.
         *
         * \warning Don't call this function before the first needData() is emitted.
         */
        void writeData(const QByteArray &data);

        /**
         * Tells the backend that the media data stream is at its end.
         *
         * \warning Don't call this function before the first needData() is emitted.
         */
        void endOfData();

        /**
         * If an I/O error occurs you should call this function to make MediaObject go into
         * ErrorState.
         *
         * \see MediaObject::errorType()
         * \see MediaObject::errorString()
         */
        void error(Phonon::ErrorType errorType, const QString &errorString);

        /**
         * Reimplement this function to reset the stream. Subsequent calls to writeData should start
         * from the first position of the data unless a seek is requested.
         *
         * The function is necessary for the case where a non-seekable MediaStream is
         * played more than once. For a seekable stream the implementation can simply call
         * \code
         * seekStream(0);
         * \endcode.
         */
        virtual void reset() = 0;

        /**
         * Reimplement this function to be notified when the backend needs data.
         *
         * When this function is called you should try to call writeData or endOfData before
         * returning.
         */
        virtual void needData() = 0;

        /**
         * Reimplement this function to be notified when the backend has enough data and your stream
         * object may take a break. This method is important for pushing data to the backend in
         * order to not fill the backend buffer unnecessarily.
         */
        virtual void enoughData();

        /**
         * Reimplement this function if your stream is seekable.
         *
         * When this function is called the next call to writeData has to be at the requested \p
         * offset.
         *
         * \warning Do not call the parent implementation.
         */
        virtual void seekStream(qint64 offset);

        AbstractMediaStream(AbstractMediaStreamPrivate &dd, QObject *parent);
        AbstractMediaStreamPrivate *d_ptr;
};

} // namespace Phonon

#endif //QT_NO_PHONON_ABSTRACTMEDIASTREAM

QT_END_NAMESPACE
QT_END_HEADER

#endif // PHONON_ABSTRACTMEDIASTREAM_H
