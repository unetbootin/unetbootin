/*  This file is part of the KDE project
    Copyright (C) 2005 Matthias Kretz <kretz@kde.org>

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
#ifndef Phonon_MEDIAOBJECT_H
#define Phonon_MEDIAOBJECT_H

#include "medianode.h"
#include "mediasource.h"
#include "phonon_export.h"
#include "phonondefs.h"
#include "phononnamespace.h"

QT_BEGIN_HEADER
QT_BEGIN_NAMESPACE

namespace Phonon
{
    class MediaObjectPrivate;

    /** \class MediaObject mediaobject.h Phonon/MediaObject
     * \short Interface for media playback of a given URL.
     *
     * This class is the most important class in %Phonon. Use it to open a media
     * file at an arbitrary location, a CD or DVD or to stream media data from
     * the application to the backend.
     *
     * This class controls the state (play, pause, stop, seek)
     * and you can use it to get a lot of information about the media data.
     *
     * Notice that most functions of this class are asynchronous.
     * That means if you call play() the object only starts playing when the
     * stateChanged() signal tells you that the object changed into PlayingState.
     * The states you can expect are documented for those methods.
     *
     * A common usage example is the following:
     * \code
     * media = new MediaObject(this);
     * connect(media, SIGNAL(finished()), SLOT(slotFinished());
     * media->setCurrentSource("/home/username/music/filename.ogg");
     * media->play();
     * \endcode
     *
     * If you want to play more that one media file (one after another) you can
     * either tell MediaObject about all those files
     * \code
     * media->setCurrentSource(":/sounds/startsound.ogg");
     * media->enqueue("/home/username/music/song.mp3");
     * media->enqueue(":/sounds/endsound.ogg");
     * \endcode
     * or provide the next file just in time:
     * \code
     *   media->setCurrentSource(":/sounds/startsound.ogg");
     *   connect(media, SIGNAL(aboutToFinish()), SLOT(enqueueNextSource()));
     * }
     *
     * void enqueueNextSource()
     * {
     *   media->enqueue("/home/username/music/song.mp3");
     * }
     * \endcode
     *
     * \ingroup Playback
     * \ingroup Recording
     * \author Matthias Kretz <kretz@kde.org>
     */
    class PHONON_EXPORT MediaObject : public QObject, public MediaNode
    {
        friend class FrontendInterfacePrivate;
        Q_OBJECT
        K_DECLARE_PRIVATE(MediaObject)
        PHONON_OBJECT(MediaObject)
        /**
         * \brief Defines the time between media sources.
         *
         * A positive transition time defines a gap of silence between queued
         * media sources.
         *
         * A transition time of 0 ms requests gapless playback (sample precise
         * queueing of the next source).
         *
         * A negative transition time defines a crossfade between the queued
         * media sources.
         *
         * Defaults to 0 (gapless playback).
         *
         * \warning This feature might not work reliably on every platform.
         */
        Q_PROPERTY(qint32 transitionTime READ transitionTime WRITE setTransitionTime)

        /**
         * \brief Get a signal before playback finishes.
         *
         * This property specifies the time in milliseconds the
         * prefinishMarkReached signal is
         * emitted before the playback finishes. A value of \c 0 disables the
         * signal.
         *
         * Defaults to \c 0 (disabled).
         *
         * \warning For some media data the total time cannot be determined
         * accurately, therefore the accuracy of the prefinishMarkReached signal
         * can be bad sometimes. Still, it is better to use this method than to
         * look at totalTime() and currentTime() to emulate the behaviour
         * because the backend might have more information available than your
         * application does through totalTime and currentTime.
         *
         * \see prefinishMarkReached
         */
        Q_PROPERTY(qint32 prefinishMark READ prefinishMark WRITE setPrefinishMark)

        /**
         * \brief The time interval in milliseconds between two ticks.
         *
         * The %tick interval is the time that elapses between the emission of two tick signals.
         * If you set the interval to \c 0 the tick signal gets disabled.
         *
         * Defaults to \c 0 (disabled).
         *
         * \warning The back-end is free to choose a different tick interval close
         * to what you asked for. This means that the following code \em may \em fail:
         * \code
         * int x = 200;
         * media->setTickInterval(x);
         * Q_ASSERT(x == producer->tickInterval());
         * \endcode
         * On the other hand the following is guaranteed:
         * \code
         * int x = 200;
         * media->setTickInterval(x);
         * Q_ASSERT(x >= producer->tickInterval() &&
         *          x <= 2 * producer->tickInterval());
         * \endcode
         *
         * \see tick
         */
        Q_PROPERTY(qint32 tickInterval READ tickInterval WRITE setTickInterval)
        public:
            /**
             * Destroys the MediaObject.
             */
            ~MediaObject();

            /**
             * Get the current state.
             *
             * @return The state of the object.
             *
             * @see State
             * \see stateChanged
             */
            State state() const;

            /**
             * Check whether the media data includes a video stream.
             *
             * \warning This information cannot be known immediately. It is best
             * to also listen to the hasVideoChanged signal.
             *
             * \code
             *   connect(media, SIGNAL(hasVideoChanged(bool)), hasVideoChanged(bool));
             *   media->setCurrentSource("somevideo.avi");
             *   media->hasVideo(); // returns false;
             * }
             *
             * void hasVideoChanged(bool b)
             * {
             *   // b == true
             *   media->hasVideo(); // returns true;
             * }
             * \endcode
             *
             * \return \c true if the media contains video data. \c false
             * otherwise.
             *
             * \see hasVideoChanged
             */
            bool hasVideo() const;

            /**
             * Check whether the current media may be seeked.
             *
             * \warning This information cannot be known immediately. It is best
             * to also listen to the hasVideoChanged signal.
             *
             * \code
             *   connect(media, SIGNAL(hasVideoChanged(bool)), hasVideoChanged(bool));
             *   media->setCurrentSource("somevideo.avi");
             *   media->hasVideo(); // returns false;
             * }
             *
             * void hasVideoChanged(bool b)
             * {
             *   // b == true
             *   media->hasVideo(); // returns true;
             * }
             * \endcode
             *
             * \return \c true when the current media may be seeked. \c false
             * otherwise.
             *
             * \see seekableChanged()
             */
            bool isSeekable() const;

            qint32 tickInterval() const;

            /**
             * Returns the strings associated with the given \p key.
             *
             * Backends should use the keys specified in the Ogg Vorbis
             * documentation: http://xiph.org/vorbis/doc/v-comment.html
             *
             * Therefore the following should work with every backend:
             *
             * A typical usage looks like this:
             * \code
             * setMetaArtist (media->metaData("ARTIST"     ));
             * setMetaAlbum  (media->metaData("ALBUM"      ));
             * setMetaTitle  (media->metaData("TITLE"      ));
             * setMetaDate   (media->metaData("DATE"       ));
             * setMetaGenre  (media->metaData("GENRE"      ));
             * setMetaTrack  (media->metaData("TRACKNUMBER"));
             * setMetaComment(media->metaData("DESCRIPTION"));
             * \endcode
             *
             * For Audio CDs you can query
             * \code
             * metaData("MUSICBRAINZ_DISCID");
             * \endcode
             * to get a DiscID hash that you can use with the MusicBrainz
             * service:
             * http://musicbrainz.org/doc/ClientHOWTO
             */
            QStringList metaData(const QString &key) const;

            /**
             * Returns the strings associated with the given \p key.
             *
             * Same as above except that the keys are defined in the
             * Phonon::MetaData enum.
             */
            QStringList metaData(Phonon::MetaData key) const;

            /**
             * Returns all meta data.
             */
            QMultiMap<QString, QString> metaData() const;

            /**
             * Returns a human-readable description of the last error that occurred.
             */
            QString errorString() const;

            /**
             * Tells your program what to do about the error.
             *
             * \see Phonon::ErrorType
             */
            ErrorType errorType() const;

            /**
             * Returns the current media source.
             *
             * \see setCurrentSource
             */
            MediaSource currentSource() const;

            /**
             * Set the media source the MediaObject should use.
             *
             * \param source The MediaSource object to the media data. You can
             * just as well use a QUrl or QString (for a local file) here.
             * Setting an empty (invalid) source, will stop and remove the
             * current source.
             *
             * \code
             * QUrl url("http://www.example.com/music.ogg");
             * media->setCurrentSource(url);
             * \endcode
             *
             * \see currentSource
             */
            void setCurrentSource(const MediaSource &source);

            /**
             * Returns the queued media sources. This does list does not include
             * the current source (returned by currentSource).
             */
            QList<MediaSource> queue() const;

            /**
             * Set the MediaSources to play when the current media has finished.
             *
             * This function will overwrite the current queue.
             *
             * \see clearQueue
             * \see enqueue
             */
            void setQueue(const QList<MediaSource> &sources);

            /**
             * Set the MediaSources to play when the current media has finished.
             *
             * This function overwrites the current queue.
             *
             * \see clearQueue
             * \see enqueue
             */
            void setQueue(const QList<QUrl> &urls);

            /**
             * Appends one source to the queue. Use this function to provide
             * the next source just in time after the aboutToFinish signal was
             * emitted.
             *
             * \see aboutToFinish
             * \see setQueue
             * \see clearQueue
             */
            void enqueue(const MediaSource &source);

            /**
             * Appends multiple sources to the queue.
             *
             * \see setQueue
             * \see clearQueue
             */
            void enqueue(const QList<MediaSource> &sources);

            /**
             * Appends multiple sources to the queue.
             *
             * \see setQueue
             * \see clearQueue
             */
            void enqueue(const QList<QUrl> &urls);

            /**
             * Clears the queue of sources.
             */
            void clearQueue();

            /**
             * Get the current time (in milliseconds) of the file currently being played.
             *
             * \return The current time in milliseconds.
             *
             * \see tick
             */
            qint64 currentTime() const;

            /**
             * Get the total time (in milliseconds) of the file currently being played.
             *
             * \return The total time in milliseconds.
             *
             * \see totalTimeChanged
             */
            qint64 totalTime() const;

            /**
             * Get the remaining time (in milliseconds) of the file currently being played.
             *
             * \return The remaining time in milliseconds.
             */
            qint64 remainingTime() const;

            qint32 prefinishMark() const;
            void setPrefinishMark(qint32 msecToEnd);

            qint32 transitionTime() const;
            void setTransitionTime(qint32 msec);

        public Q_SLOTS:

            void setTickInterval(qint32 newTickInterval);

            /**
             * Requests playback of the media data to start. Playback only
             * starts when stateChanged() signals that it goes into PlayingState,
             * though.
             *
             * \par Possible states right after this call:
             * \li BufferingState
             * \li PlayingState
             * \li ErrorState
             */
            void play();

            /**
             * Requests playback to pause. If it was paused before nothing changes.
             *
             * \par Possible states right after this call:
             * \li PlayingState
             * \li PausedState
             * \li ErrorState
             */
            void pause();

            /**
             * Requests playback to stop. If it was stopped before nothing changes.
             *
             * \par Possible states right after this call:
             * \li the state it was in before (e.g. PlayingState)
             * \li StoppedState
             * \li ErrorState
             */
            void stop();

            /**
             * Requests a seek to the time indicated.
             *
             * You can only seek if state() == PlayingState, BufferingState or PausedState.
             *
             * The call is asynchronous, so currentTime can still be the old
             * value right after this method was called. If all you need is a
             * slider that shows the current position and allows the user to
             * seek use the class SeekSlider.
             *
             * @param time The time in milliseconds where to continue playing.
             *
             * \par Possible states right after this call:
             * \li BufferingState
             * \li PlayingState
             * \li ErrorState
             *
             * \see SeekSlider
             */
            void seek(qint64 time);

            /**
             * Stops and removes all playing and enqueued media sources.
             *
             * \see setCurrentSource
             */
            void clear();

        Q_SIGNALS:
            /**
             * Emitted when the state of the MediaObject has changed.
             * In case you're not interested in the old state you can also
             * connect to a slot that only has one State argument.
             *
             * @param newstate The state the Player is in now.
             * @param oldstate The state the Player was in before.
             */
            void stateChanged(Phonon::State newstate, Phonon::State oldstate);

            /**
             * This signal gets emitted every tickInterval milliseconds.
             *
             * @param time The position of the media file in milliseconds.
             *
             * @see setTickInterval, tickInterval
             */
            void tick(qint64 time);

            /**
             * This signal is emitted whenever the audio/video data that is
             * being played is associated with new meta data. E.g. for radio
             * streams this happens when the next song is played.
             *
             * You can get the new meta data with the metaData methods.
             */
            void metaDataChanged();

            /**
             * Emitted whenever the return value of isSeekable() changes.
             *
             * Normally you'll check isSeekable() first and then let this signal
             * tell you whether seeking is possible now or not. That way you
             * don't have to poll isSeekable().
             *
             * \param isSeekable \p true  if the stream is seekable (i.e. calling
             *                            seek() works)
             *                   \p false if the stream is not seekable (i.e.
             *                            all calls to seek() will be ignored)
             */
            void seekableChanged(bool isSeekable);

            /**
             * Emitted whenever the return value of hasVideo() changes.
             *
             * Normally you'll check hasVideo() first and then let this signal
             * tell you whether video is available now or not. That way you
             * don't have to poll hasVideo().
             *
             * \param hasVideo \p true  The stream contains video and adding a
             *                          VideoWidget will show a video.
             *                 \p false There is no video data in the stream and
             *                          adding a VideoWidget will show an empty (black)
             *                          VideoWidget.
             */
#ifndef QT_NO_PHONON_VIDEO
            void hasVideoChanged(bool hasVideo);
#endif //QT_NO_PHONON_VIDEO

            /**
             * Tells about the status of the buffer.
             *
             * You can use this signal to show a progress bar to the user when
             * in BufferingState:
             *
             * \code
             * progressBar->setRange(0, 100); // this is the default
             * connect(media, SIGNAL(bufferStatus(int)), progressBar, SLOT(setValue(int)));
             * \endcode
             *
             * \param percentFilled A number between 0 and 100 telling you how
             *                      much the buffer is filled.
             */ // other names: bufferingProgress
            void bufferStatus(int percentFilled);

            /**
             * Emitted when the object has finished playback.
             * It is not emitted if you call stop(), pause() or
             * load(), but only on end-of-queue or a critical error.
             *
             * \warning This signal is not emitted when the current source has
             * finished and there's another source in the queue. It is only
             * emitted when the queue is empty.
             *
             * \see currentSourceChanged
             * \see aboutToFinish
             * \see prefinishMarkReached
             */
            void finished();

            /**
             * Emitted when the MediaObject makes a transition to the next
             * MediaSource in the queue().
             *
             * In other words, it is emitted when an individual MediaSource is
             * finished.
             *
             * \param newSource The source that starts to play at the time the
             * signal is emitted.
             */
            void currentSourceChanged(const Phonon::MediaSource &newSource);

            /**
             * Emitted before the playback of the whole queue stops. When this
             * signal is emitted you still have time to provide the next
             * MediaSource (using enqueue()) so that playback continues.
             *
             * This signal can be used to provide the next MediaSource just in
             * time for the transition still to work.
             *
             * \see enqueue
             */
            void aboutToFinish();

            /**
             * Emitted when there are only \p msecToEnd milliseconds left
             * for playback.
             *
             * \param msecToEnd The remaining time until the playback queue finishes.
             *
             * \warning This signal is not emitted when there is another source in the queue.
             * It is only emitted when the queue is empty.
             *
             * \see setPrefinishMark
             * \see prefinishMark
             * \see aboutToFinish
             * \see finished
             */
            void prefinishMarkReached(qint32 msecToEnd);

            /**
             * This signal is emitted as soon as the total time of the media file is
             * known or has changed. For most non-local media data the total
             * time of the media can only be known after some time. At that time the
             * totalTime function can not return useful information. You have
             * to wait for this signal to know the real total time.
             *
             * \param newTotalTime The length of the media file in milliseconds.
             *
             * \see totalTime
             */
            void totalTimeChanged(qint64 newTotalTime);

        protected:
            //MediaObject(Phonon::MediaObjectPrivate &dd, QObject *parent);

        private:
            Q_PRIVATE_SLOT(k_func(), void _k_resumePlay())
            Q_PRIVATE_SLOT(k_func(), void _k_resumePause())
            Q_PRIVATE_SLOT(k_func(), void _k_metaDataChanged(const QMultiMap<QString, QString> &))
#ifndef QT_NO_PHONON_ABSTRACTMEDIASTREAM
            Q_PRIVATE_SLOT(k_func(), void _k_stateChanged(Phonon::State, Phonon::State))
#endif //QT_NO_PHONON_ABSTRACTMEDIASTREAM
            Q_PRIVATE_SLOT(k_func(), void _k_aboutToFinish())
            Q_PRIVATE_SLOT(k_func(), void _k_currentSourceChanged(const MediaSource &))
    };

    /**
     * Convenience function to create a MediaObject and AudioOutput connected by
     * a path.
     */
    PHONON_EXPORT MediaObject *createPlayer(Phonon::Category category, const MediaSource &source = MediaSource());
} //namespace Phonon

QT_END_NAMESPACE
QT_END_HEADER

// vim: sw=4 ts=4 tw=80
#endif // Phonon_MEDIAOBJECT_H
