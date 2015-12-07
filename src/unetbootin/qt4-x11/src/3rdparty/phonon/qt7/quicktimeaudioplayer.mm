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

#include "quicktimeaudioplayer.h"
#include "quicktimevideoplayer.h"
#include "audiograph.h"
#include "medianodeevent.h"
#include "medianode.h"

QT_BEGIN_NAMESPACE

namespace Phonon
{
namespace QT7
{

QuickTimeAudioPlayer::QuickTimeAudioPlayer() : AudioNode(0, 1)
{
    m_state = NoMedia;
    m_videoPlayer = 0;
    m_audioChannelLayout = 0;
    m_sliceList = 0;
    m_sliceCount = 30;
    m_maxExtractionPacketCount = 4096;
    m_audioExtractionComplete = false;
    m_audioEnabled = true;
    m_samplesRemaining = -1;
    m_startTime = 0;
    m_sampleTimeStamp = 0;
    m_audioUnitIsReset = true;

#ifdef QUICKTIME_C_API_AVAILABLE
    m_audioExtractionRef = 0;
#endif
}

QuickTimeAudioPlayer::~QuickTimeAudioPlayer()
{
    unsetVideoPlayer();
}

void QuickTimeAudioPlayer::unsetVideoPlayer()
{
    if (m_audioUnit){
        OSStatus err = AudioUnitReset(m_audioUnit, kAudioUnitScope_Global, 0);
        BACKEND_ASSERT2(err == noErr, "Could not reset audio player unit when unsetting movie", FATAL_ERROR)
    }

#ifdef QUICKTIME_C_API_AVAILABLE
    if (m_audioExtractionRef && m_videoPlayer && m_videoPlayer->hasMovie())
         MovieAudioExtractionEnd(m_audioExtractionRef);
    m_audioExtractionRef = 0;
#endif

    if (m_audioChannelLayout){
        free(m_audioChannelLayout);
        m_audioChannelLayout = 0;
    }
    
    if (m_sliceList){
        for (int i=0; i<m_sliceCount; i++)
	        free(m_sliceList[i].mBufferList);
        free(m_sliceList);
        m_sliceList = 0;
    }
    
    m_videoPlayer = 0;
    m_audioExtractionComplete = false;
    m_samplesRemaining = -1;
    m_sampleTimeStamp = 0;
    m_state = NoMedia;
}

void QuickTimeAudioPlayer::enableAudio(bool enable)
{
    // Remember to seek after enabling audio.
    if (enable == m_audioEnabled)
        return;
        
    m_audioEnabled = enable;
    if (!enable)
        flush();
}

bool QuickTimeAudioPlayer::audioEnabled()
{
    return m_audioEnabled;
}

void QuickTimeAudioPlayer::setVideoPlayer(QuickTimeVideoPlayer *videoPlayer)
{
    unsetVideoPlayer();
    if (videoPlayer && videoPlayer->hasMovie()){
        m_videoPlayer = videoPlayer;
        initSoundExtraction();
        allocateSoundSlices();
        m_state = Paused;
        seek(0);
    }
}

QuickTimeVideoPlayer *QuickTimeAudioPlayer::videoPlayer()
{
    return m_videoPlayer;
}

void QuickTimeAudioPlayer::scheduleAudioToGraph()
{
    if (!m_videoPlayer || !m_audioEnabled || m_audioExtractionComplete || m_state != Playing)
        return;

    // Schedule audio slices, and detect if everything went OK.
    // If not, flag the need for another audio system, but let
    // the end app know about it:
    gClearError();
    scheduleSoundSlices();    
    if (gGetErrorType() != NO_ERROR){
        gClearError();
        if (m_audioGraph)
            m_audioGraph->setStatusCannotPlay();
    }
}

void QuickTimeAudioPlayer::flush()
{
    // Empty scheduled audio data, so playback
    // will stop. Call seek to refill data again.
    if (m_audioUnit){
        m_startTime = currentTime();
        OSStatus err = AudioUnitReset(m_audioUnit, kAudioUnitScope_Global, 0);
        BACKEND_ASSERT2(err == noErr, "Could not reset audio player unit on pause", FATAL_ERROR)
        m_audioUnitIsReset = true;
    }
}

void QuickTimeAudioPlayer::pause()
{
    m_state = Paused;
    flush();
}

void QuickTimeAudioPlayer::play()
{
    m_state = Playing;
    if (!m_audioEnabled)
        return;
    if (m_audioUnitIsReset)
        seek(m_startTime);
    else
        scheduleAudioToGraph();
}

bool QuickTimeAudioPlayer::isPlaying()
{
    return m_videoPlayer && m_state == Playing;
}

void QuickTimeAudioPlayer::seek(quint64 milliseconds)
{
    if (!m_videoPlayer || !m_videoPlayer->hasMovie())
        return;    
    if (milliseconds > m_videoPlayer->duration())
        milliseconds = m_videoPlayer->duration();
    if (!m_audioUnitIsReset && milliseconds == currentTime())
        return;
        
    m_startTime = milliseconds;
    
    // Since the graph may be running (advancing time), there is
    // no point in seeking if were not going to play immidiatly:
    if (m_state != Playing)
        return;
    if (!m_audioUnit)
        return;
    if (!m_audioEnabled || !m_videoPlayer->isSeekable())
        return;

    // Reset (and stop playing):
    OSStatus err;
    if (!m_audioUnitIsReset){
        err = AudioUnitReset(m_audioUnit, kAudioUnitScope_Global, 0);
        BACKEND_ASSERT2(err == noErr, "Could not reset audio player unit before seek", FATAL_ERROR)
    }
    m_sampleTimeStamp = 0;
    for (int i = 0; i < m_sliceCount; i++)
	    m_sliceList[i].mFlags = kScheduledAudioSliceFlag_Complete;

    // Start to play again immidiatly:
    AudioTimeStamp timeStamp;
    memset(&timeStamp, 0, sizeof(timeStamp));
	timeStamp.mFlags = kAudioTimeStampSampleTimeValid;
    timeStamp.mSampleTime = -1;
	err = AudioUnitSetProperty(m_audioUnit,
        kAudioUnitProperty_ScheduleStartTimeStamp, kAudioUnitScope_Global,
        0, &timeStamp, sizeof(timeStamp));
    BACKEND_ASSERT2(err == noErr, "Could not set schedule start time stamp on audio player unit", FATAL_ERROR)

    // Seek back to 'now' in the movie:
    TimeRecord timeRec;
	timeRec.scale = m_videoPlayer->timeScale();
    timeRec.base = 0;
	timeRec.value.hi = 0;
	timeRec.value.lo = (milliseconds / 1000.0f) * timeRec.scale;

#ifdef QUICKTIME_C_API_AVAILABLE
	err = MovieAudioExtractionSetProperty(m_audioExtractionRef,
        kQTPropertyClass_MovieAudioExtraction_Movie,
        kQTMovieAudioExtractionMoviePropertyID_CurrentTime,
        sizeof(TimeRecord), &timeRec);
    BACKEND_ASSERT2(err == noErr, "Could not set current time on audio player unit", FATAL_ERROR)
#endif

    float durationLeftSec = float(m_videoPlayer->duration() - milliseconds) / 1000.0f;
    m_samplesRemaining = (durationLeftSec > 0) ? (durationLeftSec * m_audioStreamDescription.mSampleRate) : -1;
    m_audioExtractionComplete = false;
    m_audioUnitIsReset = false;    
    scheduleAudioToGraph();

}

quint64 QuickTimeAudioPlayer::currentTime()
{
    if (!m_audioUnit){
        if (m_videoPlayer)
            return m_videoPlayer->currentTime();
        else
            return m_startTime;
    }

    Float64 currentUnitTime = getTimeInSamples(kAudioUnitProperty_CurrentPlayTime);
    if (currentUnitTime == -1)
        currentUnitTime = 0;

    quint64 cTime = quint64(m_startTime +
        float(currentUnitTime / float(m_audioStreamDescription.mSampleRate)) * 1000.0f);
    return (m_videoPlayer && cTime > m_videoPlayer->duration()) ? m_videoPlayer->duration() : cTime;
}

QString QuickTimeAudioPlayer::currentTimeString()
{
    return QuickTimeVideoPlayer::timeToString(currentTime());
}

bool QuickTimeAudioPlayer::hasAudio()
{
    if (!m_videoPlayer)
        return false;
        
    return m_videoPlayer->hasAudio();
}

bool QuickTimeAudioPlayer::soundPlayerIsAwailable()
{
    QuickTimeAudioPlayer player;
    ComponentDescription d = player.getAudioNodeDescription();
    return FindNextComponent(0, &d);
}

ComponentDescription QuickTimeAudioPlayer::getAudioNodeDescription() const
{
	ComponentDescription description;
	description.componentType = kAudioUnitType_Generator;
	description.componentSubType = kAudioUnitSubType_ScheduledSoundPlayer;
	description.componentManufacturer = kAudioUnitManufacturer_Apple;
	description.componentFlags = 0;
	description.componentFlagsMask = 0;
    return description;
}

void QuickTimeAudioPlayer::initializeAudioUnit()
{
}

bool QuickTimeAudioPlayer::fillInStreamSpecification(AudioConnection *connection, ConnectionSide side)
{
    if (!m_videoPlayer){
        if (side == Source)
            DEBUG_AUDIO_STREAM("QuickTimeAudioPlayer" << int(this) << "is source, but has no movie to use for stream spec fill.")
        return true;
    }

    if (side == Source){
        DEBUG_AUDIO_STREAM("QuickTimeAudioPlayer" << int(this) << "is source, and fills in stream spec from movie.")
        connection->m_sourceStreamDescription = m_audioStreamDescription;
        connection->m_sourceChannelLayout = (AudioChannelLayout *) malloc(m_audioChannelLayoutSize);
        memcpy(connection->m_sourceChannelLayout, m_audioChannelLayout, m_audioChannelLayoutSize);
        connection->m_sourceChannelLayoutSize = m_audioChannelLayoutSize;
        connection->m_hasSourceSpecification = true;
    }
    return true;
}

long QuickTimeAudioPlayer::regularTaskFrequency(){
    if (!m_audioEnabled || !m_audioUnit || (m_audioGraph && m_audioGraph->graphCannotPlay()))
        return INT_MAX;

    // Calculate how much audio in
    // milliseconds our slices can hold:
    int packetNeedPerSecond = m_audioStreamDescription.mSampleRate / m_maxExtractionPacketCount;
    long bufferTimeLengthSec = float(m_sliceCount) / float(packetNeedPerSecond);
    // Make sure we also get some time to fill the
    // buffer, so divide the time by two:
    return (bufferTimeLengthSec * (1000 / 2));
}

void QuickTimeAudioPlayer::initSoundExtraction()
{
#ifdef QUICKTIME_C_API_AVAILABLE

    // Initilize the extraction:
	OSStatus err = noErr;
	err = MovieAudioExtractionBegin([m_videoPlayer->qtMovie() quickTimeMovie], 0, &m_audioExtractionRef);
    BACKEND_ASSERT2(err == noErr, "Could not start audio extraction on audio player unit", FATAL_ERROR)
	m_discrete = false;
#if 0
    // Extract all channels as descrete:
    err = MovieAudioExtractionSetProperty(audioExtractionRef,
        kQTPropertyClass_MovieAudioExtraction_Movie,
        kQTMovieAudioExtractionMoviePropertyID_AllChannelsDiscrete,
        sizeof (discrete),
        &discrete);
    BACKEND_ASSERT2(err == noErr, "Could not set channels discrete on audio player unit", FATAL_ERROR)
#endif

	// Get the size of the audio channel layout (may include offset):
	err = MovieAudioExtractionGetPropertyInfo(m_audioExtractionRef,
	    kQTPropertyClass_MovieAudioExtraction_Audio,
        kQTMovieAudioExtractionAudioPropertyID_AudioChannelLayout,
        0, &m_audioChannelLayoutSize, 0);
    BACKEND_ASSERT2(err == noErr, "Could not get channel layout size from audio extraction", FATAL_ERROR)

	// Allocate memory for the layout
	m_audioChannelLayout = (AudioChannelLayout *) calloc(1, m_audioChannelLayoutSize);
    BACKEND_ASSERT2(m_audioChannelLayout, "Could not allocate memory for channel layout on audio player unit", FATAL_ERROR)

	// Get the layout:
	err = MovieAudioExtractionGetProperty(m_audioExtractionRef,
	    kQTPropertyClass_MovieAudioExtraction_Audio,
		kQTMovieAudioExtractionAudioPropertyID_AudioChannelLayout,
		m_audioChannelLayoutSize, m_audioChannelLayout, 0);
    BACKEND_ASSERT2(err == noErr, "Could not get channel layout from audio extraction", FATAL_ERROR)

	// Get audio stream description:
	err = MovieAudioExtractionGetProperty(m_audioExtractionRef,
        kQTPropertyClass_MovieAudioExtraction_Audio,
        kQTMovieAudioExtractionAudioPropertyID_AudioStreamBasicDescription,
        sizeof(m_audioStreamDescription), &m_audioStreamDescription, 0);
    BACKEND_ASSERT2(err == noErr, "Could not get audio stream description from audio extraction", FATAL_ERROR)
    
#endif // QUICKTIME_C_API_AVAILABLE
}

void QuickTimeAudioPlayer::allocateSoundSlices()
{
#ifdef QUICKTIME_C_API_AVAILABLE

    // m_sliceList will contain a specified number of ScheduledAudioSlice-s that each can
    // carry audio from extraction, and be scheduled for playback at an audio unit.
    // Each ScheduledAudioSlice will contain several audio buffers, one for each sound channel.
    // Each buffer will carry (at most) a specified number of sound packets, and each packet can
    // contain one or more frames.

    // Create a list of ScheduledAudioSlices:
	m_sliceList = (ScheduledAudioSlice *) calloc(m_sliceCount, sizeof(ScheduledAudioSlice));
    BACKEND_ASSERT2(m_sliceList, "Could not allocate memory for audio slices", FATAL_ERROR)
	bzero(m_sliceList, m_sliceCount * sizeof(ScheduledAudioSlice));

    // Calculate the size of the different structures needed:
	int packetsBufferSize = m_maxExtractionPacketCount * m_audioStreamDescription.mBytesPerPacket;
    int channels = m_audioStreamDescription.mChannelsPerFrame;
	int audioBufferListSize = int(sizeof(AudioBufferList) + (channels-1) * sizeof(AudioBuffer));
	int mallocSize = audioBufferListSize + (packetsBufferSize * m_audioStreamDescription.mChannelsPerFrame);

    // Round off to Altivec sizes:
    packetsBufferSize = int(((packetsBufferSize + 15) / 16) * 16);
    audioBufferListSize = int(((audioBufferListSize + 15) / 16) * 16);

 	for (int sliceIndex = 0; sliceIndex < m_sliceCount; ++sliceIndex){
        // Create the memory chunk for this audio slice:
		AudioBufferList	*audioBufferList = (AudioBufferList*) calloc(1, mallocSize);
        BACKEND_ASSERT2(audioBufferList, "Could not allocate memory for audio buffer list", FATAL_ERROR)

        // The AudioBufferList contains an AudioBuffer for each channel in the audio stream:
		audioBufferList->mNumberBuffers = m_audioStreamDescription.mChannelsPerFrame;
		for (uint i = 0; i < audioBufferList->mNumberBuffers; ++i){
			audioBufferList->mBuffers[i].mNumberChannels = 1;
			audioBufferList->mBuffers[i].mData = (char *) audioBufferList + audioBufferListSize + (i * packetsBufferSize);
			audioBufferList->mBuffers[i].mDataByteSize = packetsBufferSize;
		}

		m_sliceList[sliceIndex].mBufferList = audioBufferList;
		m_sliceList[sliceIndex].mNumberFrames = m_maxExtractionPacketCount;
		m_sliceList[sliceIndex].mTimeStamp.mFlags = kAudioTimeStampSampleTimeValid;
		m_sliceList[sliceIndex].mCompletionProcUserData = 0;
		m_sliceList[sliceIndex].mCompletionProc = 0;
		m_sliceList[sliceIndex].mFlags = kScheduledAudioSliceFlag_Complete;
		m_sliceList[sliceIndex].mReserved = 0;
	}
	
#endif // QUICKTIME_C_API_AVAILABLE
}

void QuickTimeAudioPlayer::scheduleSoundSlices()
{
#ifdef QUICKTIME_C_API_AVAILABLE

    PhononAutoReleasePool pool;
	// For each completed (or never used) slice, fill and schedule it.
	for (int sliceIndex = 0; sliceIndex < m_sliceCount; ++sliceIndex){
		if (m_sliceList[sliceIndex].mFlags & kScheduledAudioSliceFlag_Complete){
			if (m_samplesRemaining == 0)
				m_audioExtractionComplete = true;

			if (!m_audioExtractionComplete){
			    // Determine how many samples to read:
				int samplesCount = m_samplesRemaining;
				if ((samplesCount > m_maxExtractionPacketCount) || (samplesCount == -1))
					samplesCount = m_maxExtractionPacketCount;
				m_sliceList[sliceIndex].mTimeStamp.mSampleTime = m_sampleTimeStamp;

	            // Reset buffer sizes:
	            int byteSize = samplesCount * m_audioStreamDescription.mBytesPerPacket;
	            for (uint i = 0; i < m_sliceList[sliceIndex].mBufferList->mNumberBuffers; ++i)
                    m_sliceList[sliceIndex].mBufferList->mBuffers[i].mDataByteSize = byteSize;

	            // Do the extraction:
                UInt32 flags = 0;
                UInt32 samplesRead = samplesCount;
	            OSStatus err = MovieAudioExtractionFillBuffer(
	                m_audioExtractionRef, &samplesRead, m_sliceList[sliceIndex].mBufferList, &flags);
                BACKEND_ASSERT2(err == noErr, "Could not fill audio buffers from audio extraction", FATAL_ERROR)
	            m_audioExtractionComplete = (flags & kQTMovieAudioExtractionComplete);

	            // Play the slice:
	            if (samplesRead != 0 && m_audioUnit != 0){
                    m_sliceList[sliceIndex].mNumberFrames = samplesRead;
                    err = AudioUnitSetProperty(m_audioUnit,
                        kAudioUnitProperty_ScheduleAudioSlice, kAudioUnitScope_Global,
                        0, &m_sliceList[sliceIndex], sizeof(ScheduledAudioSlice));
                    BACKEND_ASSERT2(err == noErr, "Could not schedule audio buffers on audio unit", FATAL_ERROR)
 	            } else
					m_sliceList[sliceIndex].mFlags = kScheduledAudioSliceFlag_Complete;

                // Move the window:
				m_sampleTimeStamp += samplesRead;
				if (m_samplesRemaining != -1)
					m_samplesRemaining -= samplesRead;
			}
		}
	}

#endif // QUICKTIME_C_API_AVAILABLE
}

void QuickTimeAudioPlayer::mediaNodeEvent(const MediaNodeEvent *event)
{
    switch (event->type()){
        case MediaNodeEvent::AudioGraphAboutToBeDeleted:
        case MediaNodeEvent::AboutToRestartAudioStream:
        case MediaNodeEvent::StartConnectionChange:
            m_startTime = currentTime();
            break;
        case MediaNodeEvent::AudioGraphInitialized:
        case MediaNodeEvent::RestartAudioStreamRequest:
        case MediaNodeEvent::EndConnectionChange:
            if (m_state == Playing)
                seek(m_startTime);
            break;
       default:
            break;
    }
}

}}

QT_END_NAMESPACE

