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

#include "mediaobject.h"
#include "backendheader.h"
#include "videowidget.h"
#include "videoframe.h"
#include "audiooutput.h"
#include "quicktimevideoplayer.h"
#include "quicktimemetadata.h"
#include "audiograph.h"
#include "mediaobjectaudionode.h"
#include "quicktimeaudioplayer.h"

QT_BEGIN_NAMESPACE

namespace Phonon
{
namespace QT7
{

MediaObject::MediaObject(QObject *parent) : MediaNode(AudioSource | VideoSource, parent)
{
    m_owningMediaObject = this;
    m_state = Phonon::LoadingState;

    m_videoPlayer = new QuickTimeVideoPlayer();
    m_audioPlayer = new QuickTimeAudioPlayer();
    m_nextVideoPlayer = new QuickTimeVideoPlayer();
    m_nextAudioPlayer = new QuickTimeAudioPlayer();
    m_mediaObjectAudioNode = new MediaObjectAudioNode(m_audioPlayer, m_nextAudioPlayer);
    setAudioNode(m_mediaObjectAudioNode);

    m_metaData = new QuickTimeMetaData();
    m_audioGraph = new AudioGraph(this);

    m_tickInterval = 0;
    m_prefinishMark = 0;
    m_currentTime = 0;
    m_transitionTime = 0;
    m_percentageLoaded = 0;
    m_waitNextSwap = false;
    m_audioEffectCount = 0;
    m_audioOutputCount = 0;
    m_videoEffectCount = 0;
    m_videoOutputCount = 0;
    m_audioSystem = AS_Unset;
    m_errorType = Phonon::NoError;

    m_tickTimer = 0;
    m_bufferTimer = 0;
    m_rapidTimer = 0;

    checkForError();
}

MediaObject::~MediaObject()
{   
    // m_mediaObjectAudioNode is owned by super class.    
    m_audioPlayer->unsetVideoPlayer();
    m_nextAudioPlayer->unsetVideoPlayer();
    delete m_videoPlayer;
    delete m_nextVideoPlayer;
    delete m_metaData;
    checkForError();
}

bool MediaObject::setState(Phonon::State state)
{
    Phonon::State prevState = m_state;
    m_state = state;
    if (prevState != m_state){
        emit stateChanged(m_state, prevState);
        if (m_state != state){
            // End-application did something
            // upon  receiving the signal. 
            return false;
        }
    }
    return true;
}

void MediaObject::inspectAudioGraphRecursive(AudioConnection *connection, int &effectCount, int &outputCount)
{
    if ((connection->m_sink->m_description & (AudioSource | AudioSink)) == (AudioSource | AudioSink))
        ++effectCount;
	else if (connection->m_sink->m_description & AudioSink)
    	++outputCount;

    for (int i=0; i<connection->m_sink->m_audioSinkList.size(); ++i)
        inspectAudioGraphRecursive(connection->m_sink->m_audioSinkList[i], effectCount, outputCount);
}

void MediaObject::inspectVideoGraphRecursive(MediaNode *node, int &effectCount, int &outputCount)
{
    if ((node->m_description & (VideoSource | VideoSink)) == (VideoSource | VideoSink))
        ++effectCount;
	else if (node->m_description & VideoSink)
    	++outputCount;

    for (int i=0; i<node->m_videoSinkList.size(); ++i)
        inspectVideoGraphRecursive(node->m_videoSinkList[i], effectCount, outputCount);
}

void MediaObject::inspectGraph()
{
    // Inspect the graph to check wether there are any
    // effects or outputs connected. This will have
    // influence on the audio system and video system that ends up beeing used:
    int prevVideoOutputCount = m_videoOutputCount;	
    m_audioEffectCount = 0;
    m_audioOutputCount = 0;
    m_videoEffectCount = 0;
    m_videoOutputCount = 0;
    AudioConnection rootConnection(this);
    inspectAudioGraphRecursive(&rootConnection, m_audioEffectCount, m_audioOutputCount);
    inspectVideoGraphRecursive(this, m_videoEffectCount, m_videoOutputCount);

	if (m_videoOutputCount != prevVideoOutputCount){
	    MediaNodeEvent e1(MediaNodeEvent::VideoOutputCountChanged, &m_videoOutputCount);
	    notify(&e1);
	}	
}

void MediaObject::setupAudioSystem()
{
    // Select which audio system to use:
    AudioSystem newAudioSystem = AS_Unset;
    if (!m_audioOutputCount || !m_videoPlayer->canPlayMedia()){
        newAudioSystem = AS_Silent;
    } else if (m_audioEffectCount == 0){
        newAudioSystem = AS_Video;
    } else if (QSysInfo::MacintoshVersion < QSysInfo::MV_10_4){
        newAudioSystem = AS_Video;
        SET_ERROR("Audio effects are not supported for Mac OS 10.3 and below", NORMAL_ERROR);
    } else if (m_videoPlayer->isDrmProtected()){
        newAudioSystem = AS_Video;
        SET_ERROR("Audio effects are not supported for DRM protected media", NORMAL_ERROR);
    } else if (m_audioGraph->graphCannotPlay()){
        newAudioSystem = AS_Video;
        SET_ERROR("Audio effects are not supported for the current codec", NORMAL_ERROR);
#ifdef QUICKTIME_C_API_AVAILABLE
    } else {
        newAudioSystem = AS_Graph;
    }
#else
    } else {
        newAudioSystem = AS_Video;
        SET_ERROR("Audio effects are not supported for the 64-bit version of the Phonon QT7 backend", NORMAL_ERROR);
    }
#endif

    if (newAudioSystem == m_audioSystem)
        return;
  
    // Enable selected audio system:
    m_audioSystem = newAudioSystem; 
    switch (newAudioSystem){
        case AS_Silent:
            m_audioGraph->stop();
            m_videoPlayer->enableAudio(false);
            m_nextVideoPlayer->enableAudio(false);    
            m_audioPlayer->enableAudio(false);
            m_nextAudioPlayer->enableAudio(false);
        break;
        case AS_Graph:
            if (m_state == Phonon::PausedState)
                m_audioGraph->prepare();
            else
                m_audioGraph->start();
            // Starting the graph can lead to a recursive call
            // telling us that we must direct audio through
            // video. If that has happened, we must not proceed:
            if (m_audioSystem != AS_Graph)
                return;
            m_videoPlayer->enableAudio(false);
            m_nextVideoPlayer->enableAudio(false);
            m_audioPlayer->enableAudio(true);
            m_audioPlayer->seek(m_videoPlayer->currentTime());
            m_nextAudioPlayer->enableAudio(true);
            m_audioPlayer->seek(m_videoPlayer->currentTime());
            m_nextAudioPlayer->seek(m_nextVideoPlayer->currentTime());
        break;
        case AS_Video:
        case AS_Unset:
            m_audioGraph->stop();
            m_videoPlayer->enableAudio(true);
            m_nextVideoPlayer->enableAudio(true);
            m_audioPlayer->enableAudio(false);
            m_nextAudioPlayer->enableAudio(false);
            m_videoPlayer->seek(m_audioPlayer->currentTime());
            m_nextVideoPlayer->seek(m_nextAudioPlayer->currentTime());
        break;
    }
}

void MediaObject::setSource(const MediaSource &source)
{
    IMPLEMENTED;
	PhononAutoReleasePool pool;
    setState(Phonon::LoadingState);
    
    // Save current state for event/signal handling below:
    bool prevHasVideo = m_videoPlayer->hasVideo();
    qint64 prevTotalTime = totalTime();
    m_waitNextSwap = false;
        
    // Cancel cross-fade if any:
    m_nextVideoPlayer->pause();
    m_nextAudioPlayer->pause();
    m_mediaObjectAudioNode->cancelCrossFade();
    
    // Set new source:
    m_audioPlayer->unsetVideoPlayer();
    m_videoPlayer->setMediaSource(source);
    m_audioPlayer->setVideoPlayer(m_videoPlayer);
    m_metaData->setVideo(m_videoPlayer);        

    m_audioGraph->updateStreamSpecifications();        
    m_nextAudioPlayer->unsetVideoPlayer();
    m_nextVideoPlayer->unsetVideo();
    m_currentTime = 0;
        
    // Emit/notify information about the new source:
    QRect videoRect = m_videoPlayer->videoRect();
    MediaNodeEvent e1(MediaNodeEvent::VideoFrameSizeChanged, &videoRect);
    notify(&e1);

    // Clear video widgets:
    VideoFrame emptyFrame;
    updateVideo(emptyFrame);

    emit currentSourceChanged(source);
    emit metaDataChanged(m_metaData->metaData());

    if (prevHasVideo != m_videoPlayer->hasVideo())
        emit hasVideoChanged(m_videoPlayer->hasVideo());        
    if (prevTotalTime != totalTime())
        emit totalTimeChanged(totalTime());        
    if (checkForError())
        return;
    if (!m_videoPlayer->isDrmAuthorized())
        SET_ERROR("This computer is not authorized to play current media (DRM protected).", FATAL_ERROR)
    if (checkForError())
        return;
    if (!m_videoPlayer->canPlayMedia())
        SET_ERROR("Cannot play media.", FATAL_ERROR)
        
    // The state might have changed from LoadingState
    // as a response to an error state change. So we
    // need to check it before stopping: 
    if (m_state == Phonon::LoadingState)
        stop();

    setupAudioSystem();
    checkForError();
}

void MediaObject::setNextSource(const MediaSource &source)
{
    IMPLEMENTED;
    m_nextAudioPlayer->unsetVideoPlayer();
    m_nextVideoPlayer->setMediaSource(source);
    m_nextAudioPlayer->setVideoPlayer(m_nextVideoPlayer);
    checkForError();
}

void MediaObject::swapCurrentWithNext(qint32 transitionTime)
{
	PhononAutoReleasePool pool;
    setState(Phonon::LoadingState);
    // Save current state for event/signal handling below:
    bool prevHasVideo = m_videoPlayer->hasVideo();
    qint64 prevTotalTime = totalTime();

    qSwap(m_audioPlayer, m_nextAudioPlayer);
    qSwap(m_videoPlayer, m_nextVideoPlayer);
    m_mediaObjectAudioNode->startCrossFade(transitionTime);
    m_audioGraph->updateStreamSpecifications();
    m_metaData->setVideo(m_videoPlayer);

    m_waitNextSwap = false;
    m_currentTime = 0;
        
    // Emit/notify information about the new source:
    QRect videoRect = m_videoPlayer->videoRect();
    MediaNodeEvent e1(MediaNodeEvent::VideoFrameSizeChanged, &videoRect);
    notify(&e1);

    emit currentSourceChanged(m_videoPlayer->mediaSource());
    emit metaDataChanged(m_metaData->metaData());

    if (prevHasVideo != m_videoPlayer->hasVideo())
        emit hasVideoChanged(m_videoPlayer->hasVideo());        
    if (prevTotalTime != totalTime())
        emit totalTimeChanged(totalTime());
    if (checkForError())
        return;
    if (!m_videoPlayer->isDrmAuthorized())
        SET_ERROR("This computer is not authorized to play current media (DRM protected).", FATAL_ERROR)
    if (checkForError())
        return;
    if (!m_videoPlayer->canPlayMedia())
        SET_ERROR("Cannot play next media.", FATAL_ERROR)

    setupAudioSystem();
    checkForError();
    if (m_state == Phonon::LoadingState){
        if (setState(Phonon::PlayingState))
            play_internal();
        checkForError();
    }
}

void MediaObject::updateTimer(int &timer, int interval)
{
    if (timer)
        killTimer(timer);
    timer = 0;
    if (interval >= 0)    
        timer = startTimer(interval); 
}

void MediaObject::play_internal()
{
    // Play main audio/video:
    m_videoPlayer->play();
    m_audioPlayer->play();     
    updateLipSynch(0);
    // Play old audio/video to finish cross-fade:
    if (m_nextVideoPlayer->currentTime() > 0){
        m_nextVideoPlayer->play();
        m_nextAudioPlayer->play();
    }
    bufferAudioVideo();
    updateTimer(m_rapidTimer, 100);
}

void MediaObject::pause_internal()
{
    m_audioGraph->stop();
    m_audioPlayer->pause();
    m_nextAudioPlayer->pause();
    m_videoPlayer->pause();
    m_nextVideoPlayer->pause();
    updateTimer(m_rapidTimer, -1);
    updateTimer(m_bufferTimer, -1);

    if (m_waitNextSwap)
        m_swapTimeLeft = m_swapTime.msecsTo(QTime::currentTime());
}

void MediaObject::play()
{
    IMPLEMENTED;
    if (m_state == Phonon::PlayingState)
        return;
    if (m_waitNextSwap){
        // update swap time after pause:
        m_swapTime = QTime::currentTime();
        m_swapTime.addMSecs(m_swapTimeLeft);
        setState(Phonon::PlayingState);
        return;
    }
    if (m_currentTime == m_videoPlayer->duration())
        return;
    if (!m_videoPlayer->canPlayMedia())
        return;
    if (!setState(Phonon::PlayingState))
        return;        
    if (m_audioSystem == AS_Graph){
        m_audioGraph->start();
        m_mediaObjectAudioNode->setMute(true);
    }
	// Inform the graph that we are about to play:
	bool playing = true;
    MediaNodeEvent e1(MediaNodeEvent::MediaPlaying, &playing);
    notify(&e1);
	// Start to play:
    play_internal();
    m_mediaObjectAudioNode->setMute(false);
    checkForError();
}

void MediaObject::pause()
{
    IMPLEMENTED;
    if (m_state == Phonon::PausedState)
        return;
    if (!setState(Phonon::PausedState))
        return;
    pause_internal();
	// Inform the graph that we are no longer playing:
	bool playing = false;
    MediaNodeEvent e1(MediaNodeEvent::MediaPlaying, &playing);
    notify(&e1);
	// But be prepared:
    if (m_audioSystem == AS_Graph)
        m_audioGraph->prepare();
    checkForError();
}

void MediaObject::stop()
{
    IMPLEMENTED;
    if (m_state == Phonon::StoppedState)
        return;
    if (!setState(Phonon::StoppedState))
        return;
    m_waitNextSwap = false;
    m_nextVideoPlayer->unsetVideo();
    m_nextAudioPlayer->unsetVideoPlayer();
    pause_internal();
    seek(0);
    checkForError();
}

void MediaObject::seek(qint64 milliseconds)
{
    IMPLEMENTED;
    if (m_state == Phonon::ErrorState)
        return;
        
    // Stop cross-fade if any:
    m_nextVideoPlayer->unsetVideo();
    m_nextAudioPlayer->unsetVideoPlayer();
    m_mediaObjectAudioNode->cancelCrossFade();

    // Seek to new position:
    m_mediaObjectAudioNode->setMute(true);
    m_videoPlayer->seek(milliseconds);
    m_audioPlayer->seek(m_videoPlayer->currentTime());
    m_mediaObjectAudioNode->setMute(false);
    
    // Update time and cancel pending swap:
    if (m_currentTime < m_videoPlayer->duration())
        m_waitNextSwap = false;

    updateCurrentTime();
	if (m_state != Phonon::PlayingState)
		updateVideoFrames();
    checkForError();
}

QStringList MediaObject::availableAudioStreams() const
{
    NOT_IMPLEMENTED;
    return QStringList();
}

QStringList MediaObject::availableVideoStreams() const
{
    NOT_IMPLEMENTED;
    return QStringList();
}

QStringList MediaObject::availableSubtitleStreams() const
{
    NOT_IMPLEMENTED;
    return QStringList();
}

QString MediaObject::currentAudioStream(const QObject */*audioPath*/) const
{
    NOT_IMPLEMENTED;
    return QString();
}

QString MediaObject::currentVideoStream(const QObject */*videoPath*/) const
{
    NOT_IMPLEMENTED;
    return QString();
}

QString MediaObject::currentSubtitleStream(const QObject */*videoPath*/) const
{
    NOT_IMPLEMENTED;
    return QString();
}

void MediaObject::setCurrentAudioStream(const QString &/*streamName*/,const QObject */*audioPath*/)
{
    NOT_IMPLEMENTED;
}

void MediaObject::setCurrentVideoStream(const QString &/*streamName*/,const QObject */*videoPath*/)
{
    NOT_IMPLEMENTED;
}

void MediaObject::setCurrentSubtitleStream(const QString &/*streamName*/,const QObject */*videoPath*/)
{
    NOT_IMPLEMENTED;
}

int MediaObject::videoOutputCount()
{
	return m_videoOutputCount;
}

void MediaObject::synchAudioVideo()
{
    if (m_state != Phonon::PlayingState)
        return;
    if (m_videoSinkList.isEmpty() || m_audioSinkList.isEmpty())
        return;

    seek(m_currentTime);
    checkForError();
}

qint32 MediaObject::tickInterval() const
{
    IMPLEMENTED;
    return m_tickInterval;
}

void MediaObject::setTickInterval(qint32 interval)
{
    IMPLEMENTED;
    m_tickInterval = interval;
    if (m_tickInterval > 0)
        m_tickTimer = startTimer(m_tickInterval);
    else{
        killTimer(m_tickTimer);
        m_tickTimer = 0;
    }
}

bool MediaObject::hasVideo() const
{
    IMPLEMENTED;
    return m_videoPlayer ? m_videoPlayer->hasVideo() : false;
}

bool MediaObject::isSeekable() const
{
    IMPLEMENTED;
    return m_videoPlayer ? m_videoPlayer->isSeekable() : false;
}

qint64 MediaObject::currentTime() const
{
    IMPLEMENTED_SILENT;
    const_cast<MediaObject *>(this)->updateCurrentTime(); 
    return m_currentTime;
}

void MediaObject::updateCurrentTime()
{
    quint64 lastUpdateTime = m_currentTime;
    m_currentTime = (m_audioSystem == AS_Graph) ? m_audioPlayer->currentTime() : m_videoPlayer->currentTime();
    quint64 total = m_videoPlayer->duration();

    // Check if it's time to emit aboutToFinish:
    quint32 mark = qMax(quint64(0), qMin(total, total + m_transitionTime - 2000));
    if (lastUpdateTime < mark && mark <= m_currentTime)
        emit aboutToFinish();

    // Check if it's time to emit prefinishMarkReached:
    mark = qMax(quint64(0), total - m_prefinishMark);
    if (lastUpdateTime < mark && mark <= m_currentTime)
        emit prefinishMarkReached(total - m_currentTime);

    if (m_nextVideoPlayer->state() == QuickTimeVideoPlayer::NoMedia){
        // There is no next source in que.
        // Check if it's time to emit finished:
        if (lastUpdateTime < m_currentTime && m_currentTime == total){
            emit finished();
            m_currentTime = (m_audioSystem == AS_Graph) ? m_audioPlayer->currentTime() : m_videoPlayer->currentTime();
            if (m_state == Phonon::PlayingState && m_currentTime == total)
                pause();
        }
    } else {
        // We have a next source.
        // Check if it's time to swap to next source:
        mark = qMax(quint64(0), total + m_transitionTime);
        if (m_waitNextSwap && m_state == Phonon::PlayingState &&
            m_transitionTime < m_swapTime.msecsTo(QTime::currentTime())){
            swapCurrentWithNext(0);
        } else if (mark >= total){
            if (lastUpdateTime < total && total == m_currentTime){
                m_swapTime = QTime::currentTime();
                m_swapTime.addMSecs(mark - total);
                m_waitNextSwap = true;
            }
        } else if (lastUpdateTime < mark && mark <= m_currentTime){
            swapCurrentWithNext(total - m_currentTime);
        }
    }
}

qint64 MediaObject::totalTime() const
{
    IMPLEMENTED_SILENT;
    return m_videoPlayer->duration();
}

Phonon::State MediaObject::state() const
{
    IMPLEMENTED;
    return m_state;
}

QString MediaObject::errorString() const
{
    IMPLEMENTED;
    return m_errorString;
}

Phonon::ErrorType MediaObject::errorType() const
{
    IMPLEMENTED;
    return m_errorType;
}

bool MediaObject::checkForError()
{
    int type = gGetErrorType();
    if (type == NO_ERROR)
        return false;

    m_errorType = (type == NORMAL_ERROR) ? Phonon::NormalError : Phonon::FatalError;
    m_errorString = gGetErrorString();
    pause_internal();
    gClearError();
    setState(Phonon::ErrorState);
    return true;
}

QuickTimeVideoPlayer* MediaObject::videoPlayer() const
{
    return m_videoPlayer;
}

MediaSource MediaObject::source() const
{
    IMPLEMENTED;
    return m_videoPlayer->mediaSource();
}

qint32 MediaObject::prefinishMark() const
{
    IMPLEMENTED;
    return m_prefinishMark;
}

void MediaObject::setPrefinishMark(qint32 mark)
{
    IMPLEMENTED;
    m_prefinishMark = mark;
}

qint32 MediaObject::transitionTime() const
{
    IMPLEMENTED;
    return m_transitionTime;
}

void MediaObject::setTransitionTime(qint32 transitionTime)
{
    IMPLEMENTED;
    m_transitionTime = transitionTime;
}

void MediaObject::setVolumeOnMovie(float volume)
{
    m_videoPlayer->setMasterVolume(volume);
    m_nextVideoPlayer->setMasterVolume(volume);
}

bool MediaObject::setAudioDeviceOnMovie(int id)
{
    m_nextVideoPlayer->setAudioDevice(id);
    return m_videoPlayer->setAudioDevice(id);
}

void MediaObject::updateCrossFade()
{
    m_mediaObjectAudioNode->updateCrossFade(m_currentTime);   
    // Clean-up previous movie if done fading:
    if (m_mediaObjectAudioNode->m_fadeDuration == 0){
        if (m_nextVideoPlayer->isPlaying() || m_nextAudioPlayer->isPlaying()){
            m_nextVideoPlayer->unsetVideo();
            m_nextAudioPlayer->unsetVideoPlayer();
        }
    }        
}

void MediaObject::updateBufferStatus()
{
    float percent = m_videoPlayer->percentageLoaded();
    if (percent != m_percentageLoaded){
        m_percentageLoaded = percent;
        emit bufferStatus(m_percentageLoaded * 100);
    }
}

void MediaObject::updateAudioBuffers()
{
    // Schedule audio slices:
    m_audioPlayer->scheduleAudioToGraph();
    m_nextAudioPlayer->scheduleAudioToGraph();
}

bool MediaObject::isCrossFading()
{
    return m_mediaObjectAudioNode->isCrossFading();
}

void MediaObject::updateVideoFrames()
{
    // Draw next frame if awailable:
    if (m_videoPlayer->videoFrameChanged()){
        updateLipSynch(50);
        VideoFrame frame(m_videoPlayer);           
        if (m_nextVideoPlayer->isPlaying()
            && m_nextVideoPlayer->hasVideo()
            && isCrossFading()){
            VideoFrame bgFrame(m_nextVideoPlayer);
            frame.setBackgroundFrame(bgFrame);
            frame.setBaseOpacity(m_mediaObjectAudioNode->m_volume1);
        }
        
        // Send the frame through the graph:
        updateVideo(frame);    
        checkForError();
    }
}

void MediaObject::updateLipSynch(int allowedOffset)
{
    if (m_audioSystem != AS_Graph || !m_audioGraph->isRunning())
        return;
    if (m_videoSinkList.isEmpty() || m_audioSinkList.isEmpty())
        return;
        
    if (m_videoPlayer->hasVideo()){
        qint64 diff = m_audioPlayer->currentTime() - m_videoPlayer->currentTime();
        if (-allowedOffset > diff || diff > allowedOffset)
            m_audioPlayer->seek(m_videoPlayer->currentTime());
    }

    if (isCrossFading() && m_nextVideoPlayer->hasVideo()){
        qint64 diff = m_nextAudioPlayer->currentTime() - m_nextVideoPlayer->currentTime();
        if (-(allowedOffset*2) > diff || diff > (allowedOffset*2))
            m_nextAudioPlayer->seek(m_nextVideoPlayer->currentTime());
    }
}

void MediaObject::bufferAudioVideo()
{
    long nextVideoUpdate = m_videoPlayer->hasVideo() ? 30 : INT_MAX;
    long nextAudioUpdate = m_audioPlayer->regularTaskFrequency();
    updateAudioBuffers();
    updateVideoFrames();
    if (m_state == Phonon::PlayingState)
        updateTimer(m_bufferTimer, qMin(nextVideoUpdate, nextAudioUpdate));
}

void MediaObject::updateRapidly()
{
    updateCurrentTime();
    updateCrossFade();
    updateBufferStatus();
}

void MediaObject::setMute(bool mute)
{
    m_mediaObjectAudioNode->setMute(mute);
    m_videoPlayer->setMute(mute);
    m_nextVideoPlayer->setMute(mute);
}

void MediaObject::mediaNodeEvent(const MediaNodeEvent *event)
{
    switch (event->type()){
        case MediaNodeEvent::EndConnectionChange:
            m_mediaObjectAudioNode->setMute(true);
            inspectGraph();
            setupAudioSystem();
            synchAudioVideo();
            checkForError();
            m_mediaObjectAudioNode->setMute(false);
             if (m_state == Phonon::PlayingState)
                bufferAudioVideo();
            break;
        case MediaNodeEvent::AudioGraphCannotPlay:
        case MediaNodeEvent::AudioGraphInitialized:
            if (m_state != Phonon::LoadingState){
                m_mediaObjectAudioNode->setMute(true);
                setupAudioSystem();
                updateLipSynch(0);
                checkForError();
                m_mediaObjectAudioNode->setMute(false);
            }
            break; 
        default:
            break;
    }
}

bool MediaObject::event(QEvent *event)
{
    switch (event->type()){
        case QEvent::Timer: {
            QTimerEvent *timerEvent = static_cast<QTimerEvent *>(event);
            if (timerEvent->timerId() == m_rapidTimer)
                updateRapidly();
            else if (timerEvent->timerId() == m_tickTimer)
                emit tick(currentTime());
            else if (timerEvent->timerId() == m_bufferTimer)
                bufferAudioVideo();
            }
            break;
        default:
            break;
    }
    return QObject::event(event);
}

bool MediaObject::hasInterface(Interface /*interface*/) const
{
    return false;
}

QVariant MediaObject::interfaceCall(Interface /*interface*/, int /*command*/, const QList<QVariant> &/*arguments*/)
{
    return QVariant();
}

}} // namespace Phonon::QT7

QT_END_NAMESPACE

#include "moc_mediaobject.cpp"

