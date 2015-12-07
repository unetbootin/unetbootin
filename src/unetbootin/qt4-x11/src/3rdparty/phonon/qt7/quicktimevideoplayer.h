/*  This file is part of the KDE project.

    Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).

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

#include "backendheader.h"

#import <QTKit/QTDataReference.h>
#import <QTKit/QTMovie.h>

#include <phonon/mediasource.h>
#include <Carbon/Carbon.h>
#include <QtCore>
#include <QGLPixelBuffer>
#include "videoframe.h"

QT_BEGIN_NAMESPACE

class QGLContext;

namespace Phonon
{
namespace QT7
{
    class QuickTimeStreamReader;
	class VideoRenderWidgetQTMovieView;
    
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

            bool videoFrameChanged();
            CVOpenGLTextureRef currentFrameAsCVTexture();
            GLuint currentFrameAsGLTexture();
			void *currentFrameAsCIImage();
            QImage currentFrameAsQImage();
            QRect videoRect() const;

            quint64 duration() const;
            quint64 currentTime() const;
            long timeScale() const;
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
            QTMovie *qtMovie() const;
            
            float playbackRate() const;
            float prefferedPlaybackRate() const;

            QuickTimeVideoPlayer::State state() const;

            bool hasAudio() const;
            bool hasVideo() const;
            bool hasMovie() const;
            bool canPlayMedia() const;
            bool isPlaying() const;
            bool isSeekable() const;
            bool isDrmProtected() const;
            bool isDrmAuthorized() const;

            bool preRollMovie(qint64 startTime = 0);
            float percentageLoaded();
            quint64 timeLoaded();

            static QString timeToString(quint64 ms);

			// Help functions when drawing to more that one widget in cocoa 64:
			void *m_primaryRenderingTarget;
            void setPrimaryRenderingTarget(NSObject *target);

			void *primaryRenderingCIImage();
			void setPrimaryRenderingCIImage(void *ciImage);

        private:
            QTMovie *m_QTMovie;
            State m_state;
            QGLPixelBuffer *m_QImagePixelBuffer;

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
			void *m_primaryRenderingCIImage; 
			qreal m_brightness;
			qreal m_contrast;
			qreal m_hue;
			qreal m_saturation;

#ifdef QUICKTIME_C_API_AVAILABLE
            QTVisualContextRef m_visualContext;
#endif
            VideoFrame m_currentFrame;
            QuickTimeStreamReader *m_streamReader;

            void createVisualContext();
            void openMovieFromCurrentMediaSource();
            void openMovieFromDataRef(QTDataReference *dataRef);
            void openMovieFromFile();
            void openMovieFromUrl();
            void openMovieFromStream();
            void openMovieFromData(QByteArray *data, char *fileType);
            void openMovieFromDataGuessType(QByteArray *data);
			QString mediaSourcePath();
			bool codecExistsAccordingToSuffix(const QString &fileName);
            
            void setError(NSError *error);
            bool errorOccured();
            void readProtection();
            void checkIfVideoAwailable();
            bool movieNotLoaded();
            void waitStatePlayable();
    };

    Q_DECLARE_OPERATORS_FOR_FLAGS(QuickTimeVideoPlayer::State);

}} // namespace Phonon::QT7

QT_END_NAMESPACE

#endif // Phonon_QT7_QUICKTIMEVIDEOPLAYER_H
