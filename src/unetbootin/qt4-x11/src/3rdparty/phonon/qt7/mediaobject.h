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

#ifndef Phonon_QT7_MEDIAOBJECT_H
#define Phonon_QT7_MEDIAOBJECT_H

#include <QtCore>
#include <phonon/mediaobjectinterface.h>
#include <phonon/addoninterface.h>

#include "medianode.h"

QT_BEGIN_NAMESPACE

namespace Phonon
{
namespace QT7
{
    class QuickTimeVideoPlayer;
    class QuickTimeAudioPlayer;
    class QuickTimeMetaData;
    class AudioGraph;
    class MediaObjectAudioNode;

    class MediaObject : public MediaNode,
        public Phonon::MediaObjectInterface, public Phonon::AddonInterface
    {
        Q_OBJECT
        Q_INTERFACES(Phonon::MediaObjectInterface Phonon::AddonInterface)

    public:
        MediaObject(QObject *parent);
        ~MediaObject();

        QStringList availableAudioStreams() const;
        QStringList availableVideoStreams() const;
        QStringList availableSubtitleStreams() const;
        QString currentAudioStream(const QObject *audioPath) const;
        QString currentVideoStream(const QObject *videoPath) const;
        QString currentSubtitleStream(const QObject *videoPath) const;

        void setCurrentAudioStream(const QString &streamName,const QObject *audioPath);
        void setCurrentVideoStream(const QString &streamName,const QObject *videoPath);
        void setCurrentSubtitleStream(const QString &streamName,const QObject *videoPath);

        void play();
        void pause();
        void stop();
        void seek(qint64 milliseconds);

        qint32 tickInterval() const;
        void setTickInterval(qint32 interval);
        bool hasVideo() const;
        bool isSeekable() const;
        qint64 currentTime() const;
        Phonon::State state() const;

        QString errorString() const;
        Phonon::ErrorType errorType() const;

        qint64 totalTime() const;
        MediaSource source() const;
        void setSource(const MediaSource &);
        void setNextSource(const MediaSource &source);
        qint32 prefinishMark() const;
        void setPrefinishMark(qint32);
        qint32 transitionTime() const;
        void setTransitionTime(qint32);
        bool hasInterface(Interface interface) const;
        QVariant interfaceCall(Interface interface, int command, const QList<QVariant> &arguments = QList<QVariant>());

        QuickTimeVideoPlayer* videoPlayer() const;
        QuickTimeAudioPlayer* audioPlayer() const;

        void setVolumeOnMovie(float volume);
        bool setAudioDeviceOnMovie(int id);

		int videoOutputCount();
        
    signals:
        void stateChanged(Phonon::State,Phonon::State);
        void tick(qint64);
        void seekableChanged(bool);
        void hasVideoChanged(bool);
        void bufferStatus(int);
        void finished();
        void aboutToFinish();
        void prefinishMarkReached(qint32);
        void totalTimeChanged(qint64);
        void metaDataChanged(QMultiMap<QString,QString>);
        void currentSourceChanged(const MediaSource &newSource);

    protected:
        void mediaNodeEvent(const MediaNodeEvent *event);
        bool event(QEvent *event);

    private:
        enum AudioSystem {AS_Unset, AS_Video, AS_Graph, AS_Silent} m_audioSystem;
        Phonon::State m_state;

        QuickTimeVideoPlayer *m_videoPlayer;
        QuickTimeAudioPlayer *m_audioPlayer;
        QuickTimeVideoPlayer *m_nextVideoPlayer;
        QuickTimeAudioPlayer *m_nextAudioPlayer;
        MediaObjectAudioNode *m_mediaObjectAudioNode;
        QuickTimeMetaData *m_metaData;

        qint32 m_tickInterval;
        qint32 m_transitionTime;
        quint32 m_prefinishMark;
        quint32 m_currentTime;
        float m_percentageLoaded;
        
        int m_tickTimer;
        int m_bufferTimer;
        int m_rapidTimer;

        bool m_waitNextSwap;
        int m_swapTimeLeft;
        QTime m_swapTime;

        void synchAudioVideo();
        void updateCurrentTime();
        void swapCurrentWithNext(qint32 transitionTime);
        bool setState(Phonon::State state);
        void pause_internal();
        void play_internal();
        void setupAudioSystem();
        void updateTimer(int &timer, int interval);
        void bufferAudioVideo();
        void updateRapidly();
        void updateCrossFade();
        void updateAudioBuffers();
        void updateLipSynch(int allowedOffset);
        void updateVideoFrames();
        void updateBufferStatus();
        void setMute(bool mute);
		void inspectAudioGraphRecursive(AudioConnection *connection, int &effectCount, int &outputCount);
		void inspectVideoGraphRecursive(MediaNode *node, int &effectCount, int &outputCount);
        void inspectGraph();
        bool isCrossFading();
        
        QString m_errorString;
        Phonon::ErrorType m_errorType;
        bool checkForError();

	    int m_audioEffectCount;
	    int m_audioOutputCount;
	    int m_videoEffectCount;
	    int m_videoOutputCount;
    };

}} //namespace Phonon::QT7

QT_END_NAMESPACE
#endif // Phonon_QT7_MEDIAOBJECT_H
