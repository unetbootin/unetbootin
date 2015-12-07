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

#include "mediaobjectaudionode.h"
#include "quicktimeaudioplayer.h"
#include "quicktimevideoplayer.h"
#include "audiomixer.h"

QT_BEGIN_NAMESPACE

namespace Phonon
{
namespace QT7
{

MediaObjectAudioNode::MediaObjectAudioNode(QuickTimeAudioPlayer *player1, QuickTimeAudioPlayer *player2) : AudioNode(0, 1)
{
    m_mute = false;
    m_player1 = player1;
    m_player2 = player2;
    m_mixer = new AudioMixerAudioNode();

    m_connection1 = new AudioConnection(m_player1, 0, m_mixer, 0);
    m_connection2 = new AudioConnection(m_player2, 0, m_mixer, 1);

    m_fadeDuration = 0;
}

MediaObjectAudioNode::~MediaObjectAudioNode()
{
    setGraph(0);
    delete m_player1;
    delete m_player2;
    delete m_mixer;
    delete m_connection1;
    delete m_connection2;
}

void MediaObjectAudioNode::createAndConnectAUNodes()
{
    DEBUG_AUDIO_GRAPH("(MediaObjectAudioNode" << int(this) << "createAndConnectAUNodes called)" )
    m_player1->createAndConnectAUNodes();
    m_player2->createAndConnectAUNodes();
    m_mixer->createAndConnectAUNodes();

    m_connection1->connect(m_audioGraph);
    m_connection2->connect(m_audioGraph);
}

void MediaObjectAudioNode::createAudioUnits()
{
    DEBUG_AUDIO_GRAPH("(MediaObjectAudioNode" << int(this) << "createAudioUnits called)" )
    m_player1->createAudioUnits();
    m_player2->createAudioUnits();
    m_mixer->createAudioUnits();
}

void MediaObjectAudioNode::setGraph(AudioGraph *audioGraph)
{
    DEBUG_AUDIO_GRAPH("MediaObjectAudioNode" << int(this) << "is setting graph:" << int(audioGraph))
    m_audioGraph = audioGraph;
    m_player1->setGraph(audioGraph);
    m_player2->setGraph(audioGraph);
    m_mixer->setGraph(audioGraph);
}

AUNode MediaObjectAudioNode::getOutputAUNode()
{
    return m_mixer->getOutputAUNode();
}

bool MediaObjectAudioNode::fillInStreamSpecification(AudioConnection *connection, ConnectionSide side)
{
    if (side == Source){
        DEBUG_AUDIO_STREAM("(MediaObjectAudioNode" << int(this) << "fillInStreamSpecification called, role = source)")
        return m_mixer->fillInStreamSpecification(connection, side);
    } else {
        DEBUG_AUDIO_STREAM("(MediaObjectAudioNode" << int(this) << "fillInStreamSpecification called, role = sink)")
        return (m_connection2->updateStreamSpecification() && m_connection1->updateStreamSpecification());
    }
}

bool MediaObjectAudioNode::setStreamSpecification(AudioConnection *connection, ConnectionSide side)
{
    if (side == Source){
        DEBUG_AUDIO_STREAM("(MediaObjectAudioNode" << int(this) << "setStreamSpecification called, role = source)")
        return m_mixer->setStreamSpecification(connection, side);
    }
    return true;
}

void MediaObjectAudioNode::setMute(bool mute)
{
    m_mute = mute;
    m_mixer->setVolume(m_mute ? 0 : m_volume1, m_connection1->m_sinkInputBus);
    m_mixer->setVolume(m_mute ? 0 : m_volume2, m_connection2->m_sinkInputBus);
}

void MediaObjectAudioNode::updateVolume()
{
    if (m_mute)
        return;
        
    QuickTimeVideoPlayer *player1 = static_cast<QuickTimeAudioPlayer *>(m_connection1->m_sourceAudioNode)->videoPlayer();
    QuickTimeVideoPlayer *player2 = static_cast<QuickTimeAudioPlayer *>(m_connection2->m_sourceAudioNode)->videoPlayer();
    if (player1)
        player1->setRelativeVolume(m_volume1);
    if (player2)
        player2->setRelativeVolume(m_volume2);

    m_mixer->setVolume(m_volume1, m_connection1->m_sinkInputBus);
    m_mixer->setVolume(m_volume2, m_connection2->m_sinkInputBus);
}

void MediaObjectAudioNode::startCrossFade(qint64 duration)
{
    m_fadeDuration = duration;

    // Swap:
    AudioConnection *tmp = m_connection1;
    m_connection1 = m_connection2;
    m_connection2 = tmp;

    // Init volume:
    if (m_fadeDuration > 0){
        m_volume1 = 0;
        m_volume2 = 1;
    } else {
        m_volume1 = 1;
        m_volume2 = 0;
    }
    updateVolume();
}

float MediaObjectAudioNode::applyCurve(float volume)
{
    float newValue = 0;
    if (volume > 0)
        newValue = float(0.5f * (2 + log10(volume)));
    return newValue;
}

void MediaObjectAudioNode::updateCrossFade(qint64 currentTime)
{
    // Assume that currentTime starts at 0 and progress.
    if (m_fadeDuration > 0){
        float volume = float(currentTime) / float(m_fadeDuration);
        if (volume >= 1){
            volume = 1;
            m_fadeDuration = 0;
        }
        m_volume1 = applyCurve(volume);
        m_volume2 = 1 - volume;
        updateVolume();
    }
}

bool MediaObjectAudioNode::isCrossFading()
{
    return (m_fadeDuration > 0);
}

void MediaObjectAudioNode::cancelCrossFade()
{
    m_fadeDuration = 0;
    m_volume1 = 1;
    m_volume2 = 0;
    updateVolume();
}

void MediaObjectAudioNode::mediaNodeEvent(const MediaNodeEvent *event)
{
    switch (event->type()){
    case MediaNodeEvent::AudioGraphAboutToBeDeleted:
        m_connection1->invalidate();
        m_connection2->invalidate();
        break;
    case MediaNodeEvent::AudioGraphCannotPlay:
    case MediaNodeEvent::AudioGraphInitialized:
        updateVolume();
        break;
    default:
        break;
    }

    m_player1->mediaNodeEvent(event);
    m_player2->mediaNodeEvent(event);
    m_mixer->mediaNodeEvent(event);
}

}} //namespace Phonon::QT7

QT_END_NAMESPACE
