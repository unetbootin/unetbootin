/*  This file is part of the KDE project
    Copyright (C) 2004-2007 Matthias Kretz <kretz@kde.org>

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

#ifndef Phonon_VIDEOPLAYER_H
#define Phonon_VIDEOPLAYER_H

#include "phonon/phonon_export.h"
#include "phonon/phononnamespace.h"
#include "phonon/mediasource.h"
#include <QtGui/QWidget>

QT_BEGIN_HEADER
QT_BEGIN_NAMESPACE

namespace Phonon
{
class VideoPlayerPrivate;
class MediaObject;
class AudioOutput;
class VideoWidget;

/** \class VideoPlayer videoplayer.h Phonon/VideoPlayer
 * \short Playback class for simple tasks.
 *
 * With %VideoPlayer you can get results quickly and easily. You can do the standard
 * playback tasks like play, pause and stop, but also set a playback volume and
 * seek (there's no guarantee that the seek will work, though).
 *
 * Keep in mind that when the %VideoPlayer instance is deleted the playback will
 * stop.
 *
 * A play and forget code example:
 * \code
 * VideoPlayer *player = new VideoPlayer(Phonon::VideoCategory, parentWidget);
 * connect(player, SIGNAL(finished()), player, SLOT(deleteLater()));
 * player->play(url);
 * \endcode
 *
 * \ingroup Playback
 * \ingroup PhononVideo
 * \author Matthias Kretz <kretz@kde.org>
 */
class PHONON_EXPORT VideoPlayer : public QWidget
{
    Q_OBJECT
    public:
        /**
         * Constructs a new %VideoPlayer instance.
         *
         * \param category The category used for the audio output device.
         * \param parent The QObject parent.
         */
        explicit VideoPlayer(Phonon::Category category, QWidget *parent = 0);

        /**
         * On destruction the playback is stopped, also the audio output is
         * removed so that the desktop mixer will not show the application
         * anymore. If you need a persistent audio output don't use
         * %VideoPlayer but MediaObject, VideoPath and VideoOutput.
         */
        ~VideoPlayer();

        /**
         * Get the total time (in milliseconds) of the file currently being played.
         */
        qint64 totalTime() const;
        /**
         * Get the current time (in milliseconds) of the file currently being played.
         */
        qint64 currentTime() const;
        /**
         * This is the current volume of the output as voltage factor.
         *
         * 1.0 means 100%, 0.5 means 50% voltage/25% power, 0.0 means 0%
         */
        float volume() const;

        /**
         * \returns \c true if it is currently playing
         * \returns \c false if it is currently stopped or paused
         */
        bool isPlaying() const;
        /**
         * \returns \c true if it is currently paused
         * \returns \c false if it is currently playing or stopped
         */
        bool isPaused() const;

        /**
         * getter for the MediaObject.
         */
        MediaObject *mediaObject() const;

        /**
         * getter for the AudioOutput.
         */
        AudioOutput *audioOutput() const;

        /**
         * getter for the VideoWidget.
         */
        VideoWidget *videoWidget() const;

    public Q_SLOTS:
        /**
         * Starts preloading the media data and fill audiobuffers in the
         * backend.
         *
         * When there's already a media playing (or paused) it will be stopped
         * (the finished signal will not be emitted).
         */
        void load(const Phonon::MediaSource &source);

        /**
         * Play the media at the given URL. Starts playback as fast as possible.
         * This can take a considerable time depending on the URL and the
         * backend.
         *
         * If you need low latency between calling play() and the sound actually
         * starting to play on your output device you need to use MediaObject
         * and be able to set the URL before calling play(). Note that
         * \code
         * audioPlayer->load(url);
         * audioPlayer->play();
         * \endcode
         * doesn't make a difference: the application should be idle between the
         * load and play calls so that the backend can start preloading the
         * media and fill audio buffers.
         */
        void play(const Phonon::MediaSource &source);

        /**
         * Continues playback of a paused media. Restarts playback of a stopped
         * media.
         */
        void play();
        /**
         * Pauses the playback.
         */
        void pause();
        /**
         * Stops the playback.
         */
        void stop();

        /**
         * Seeks to the requested time. Note that the backend is free to ignore
         * the seek request if the media source isn't seekable.
         *
         * \param ms Time in milliseconds from the start of the media.
         */
        void seek(qint64 ms);
        /**
         * Sets the volume of the output as voltage factor.
         *
         * 1.0 means 100%, 0.5 means 50% voltage/25% power, 0.0 means 0%
         */
        void setVolume(float volume);

    Q_SIGNALS:
        /**
         * This signal is emitted when the playback finished.
         */
        void finished();

    protected:
        VideoPlayerPrivate *const d;
};

} //namespace Phonon

QT_END_NAMESPACE
QT_END_HEADER

#endif // Phonon_VIDEOPLAYER_H
// vim: sw=4 ts=4 tw=80
