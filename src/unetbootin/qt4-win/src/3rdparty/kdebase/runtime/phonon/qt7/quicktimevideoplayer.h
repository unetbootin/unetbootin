/*  This file is part of the KDE project.

    Copyright (C) 2007 Trolltech ASA. All rights reserved.

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

#ifndef Phonon_QT7_QUICKTIMEVIDEOPLAYER_H
#define Phonon_QT7_QUICKTIMEVIDEOPLAYER_H

#include <QuickTime/QuickTime.h>
#undef check // avoid name clash;
#include <AGL/agl.h>

#include <phonon/mediasource.h>
#include <Carbon/Carbon.h>
#include <QtCore>
#include "videoframe.h"

QT_BEGIN_NAMESPACE

class QGLContext;

namespace Phonon
{
namespace QT7
{
    class QuickTimeStreamReader;
    
    class QuickTimeVideoPlayer : QObject
    {
        public:
            enum StateEnum {
                Playing = 0x1,
                Paused = 0x2,
                NoMedia = 0x4,
            };
            Q_DECLARE_FLAGS(State, StateEnum);

            QuickTimeVideoPlayer();
            virtual ~QuickTimeVideoPlayer();

            void setMediaSource(const MediaSource &source);
            MediaSource mediaSource() const;
            void unsetVideo();

            void play();
            void pause();
            void seek(quint64 milliseconds);

            bool setGLContext(const QGLContext *qGLContext);
            bool setCurrentGLContext();

            bool videoFrameChanged(const LinkTimeProxy &timeStamp);
            CVOpenGLTextureRef createCvTexture(const LinkTimeProxy &timeStamp);
            QRect videoRect() const;

            quint64 duration() const;
            quint64 currentTime() const;
            QString currentTimeString();

            void setColors(qreal brightness = 0, qreal contrast = 1, qreal hue = 0, qreal saturation = 1);
            void setMasterVolume(float volume);
            void setRelativeVolume(float volume);
            void setVolume(float masterVolume, float relativeVolume);
            void setMute(bool mute);
            void enableAudio(bool enable);
            bool audioEnabled();
            bool setAudioDevice(int id);
            void setPlaybackRate(float rate);
            
            float playbackRate() const;
            float prefferedPlaybackRate() const;

            Movie movieRef() const;
            QuickTimeVideoPlayer::State state() const;

            bool hasAudio() const;
            bool hasVideo() const;
            bool canPlayMedia() const;
            bool isPlaying() const;
            bool isSeekable() const;
            bool isDrmProtected() const;
            bool isDrmAuthorized() const;

            bool preRollMovie(qint64 startTime = 0);
            float percentageLoaded();
            quint64 timeLoaded();

            static QString timeToString(quint64 ms);

        private:
            Movie m_movieRef;
            State m_state;
            bool m_playbackRateSat;
            bool m_isDrmProtected;
            bool m_isDrmAuthorized;
            bool m_mute;
            bool m_audioEnabled;
            bool m_hasVideo;
            float m_masterVolume;
            float m_relativeVolume;
            float m_playbackRate;
            quint64 m_currentTime;
            MediaSource m_mediaSource;
            QTVisualContextRef m_visualContext;
            VideoFrame m_currentFrame;
            QuickTimeStreamReader *m_streamReader;
            const QGLContext *m_qGLContext;

            void openMovieFromCurrentMediaSource();
            void openMovieFromDataRef(DataReferenceRecord &dataRef);
            void openMovieFromFile();
            void openMovieFromUrl();
            void openMovieFromStream();
            
            bool checkForError();
            void readProtection();
            void getTrackProtection(const Track &track, bool &isProtected, bool &isAuthorized) const;
            bool setGLContext(AGLContext aglContext, AGLPixelFormat aglPixelFormat);
            void checkIfVideoAwailable();
            void waitForState(long waitState);
    };

    Q_DECLARE_OPERATORS_FOR_FLAGS(QuickTimeVideoPlayer::State);

}} // namespace Phonon::QT7

QT_END_NAMESPACE

#endif // Phonon_QT7_QUICKTIMEVIDEOPLAYER_H
