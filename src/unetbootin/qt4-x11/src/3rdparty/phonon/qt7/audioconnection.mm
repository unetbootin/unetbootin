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

#include "audioconnection.h"
#include "medianode.h"
#include "audionode.h"
#include "audiograph.h"

QT_BEGIN_NAMESPACE

namespace Phonon
{
namespace QT7
{

    AudioConnection::AudioConnection()
        :   m_source(0), m_sourceAudioNode(0), m_sourceOutputBus(0),
            m_sink(0), m_sinkAudioNode(0), m_sinkInputBus(0),
            m_sourceChannelLayout(0), m_sinkChannelLayout(0),
            m_hasSourceSpecification(false), m_hasSinkSpecification(false), m_connected(false)
    {}

    AudioConnection::AudioConnection(MediaNode *source, int output, MediaNode *sink, int input)
        : m_source(source), m_sourceAudioNode(source->m_audioNode), m_sourceOutputBus(output),
        m_sink(sink), m_sinkAudioNode(sink->m_audioNode), m_sinkInputBus(input),
        m_sourceChannelLayout(0), m_sinkChannelLayout(0),
        m_hasSourceSpecification(false), m_hasSinkSpecification(false), m_connected(false)
    {}

    AudioConnection::AudioConnection(MediaNode *sink)
        : m_source(0), m_sourceAudioNode(0), m_sourceOutputBus(0),
        m_sink(sink), m_sinkAudioNode(sink->m_audioNode), m_sinkInputBus(0),
        m_sourceChannelLayout(0), m_sinkChannelLayout(0), m_connected(false)
    {}

    AudioConnection::AudioConnection(AudioNode *source, int output, AudioNode *sink, int input)
        : m_source(0), m_sourceAudioNode(source), m_sourceOutputBus(output),
        m_sink(0), m_sinkAudioNode(sink), m_sinkInputBus(input),
        m_sourceChannelLayout(0), m_sinkChannelLayout(0),
        m_hasSourceSpecification(false), m_hasSinkSpecification(false), m_connected(false)
    {}

    AudioConnection::AudioConnection(AudioNode *sink)
        : m_source(0), m_sourceAudioNode(0), m_sourceOutputBus(0),
        m_sink(0), m_sinkAudioNode(sink), m_sinkInputBus(0),
        m_sourceChannelLayout(0), m_sinkChannelLayout(0), m_connected(false)
    {}

    AudioConnection::~AudioConnection()
    {
        freeMemoryAllocations();
    }

    void AudioConnection::freeMemoryAllocations()
    {
        if (m_sinkChannelLayout && m_sourceChannelLayout != m_sinkChannelLayout)
            free(m_sinkChannelLayout);
        if (m_sourceChannelLayout)
            free(m_sourceChannelLayout);
        m_sinkChannelLayout = 0;
        m_sourceChannelLayout = 0;
    }

    bool AudioConnection::updateStreamSpecification()
    {
        m_hasSourceSpecification = false;
        m_hasSinkSpecification = false;
        freeMemoryAllocations();

        bool updateOk;
        if (m_sourceAudioNode){
            updateOk = m_sourceAudioNode->fillInStreamSpecification(this, AudioNode::Source);
            if (!updateOk)
                return false;
            updateOk = m_sourceAudioNode->setStreamSpecification(this, AudioNode::Source);
            if (!updateOk)
                return false;
        }
        updateOk = m_sinkAudioNode->fillInStreamSpecification(this, AudioNode::Sink);
        if (!updateOk)
            return false;
        updateOk = m_sinkAudioNode->setStreamSpecification(this, AudioNode::Sink);
        if (!updateOk)
            return false;
        return true;
    }

    bool AudioConnection::connect(AudioGraph *graph)
    {
        if (m_connected || !m_sourceAudioNode)
            return true;

        DEBUG_AUDIO_GRAPH("Connection" << int(this) << "connect"
        << int(m_sourceAudioNode) << m_sourceOutputBus << "->"
        << int(m_sinkAudioNode) << m_sinkInputBus)

        AUNode sourceOut = m_sourceAudioNode->getOutputAUNode();
        AUNode sinkIn = m_sinkAudioNode->getInputAUNode();
        OSStatus err = AUGraphConnectNodeInput(graph->audioGraphRef(), sourceOut, m_sourceOutputBus, sinkIn, m_sinkInputBus);
        m_connected = (err == noErr) ? true : false;
        return m_connected;
    }

    bool AudioConnection::disconnect(AudioGraph *graph)
    {
        if (!m_connected || !m_sourceAudioNode)
            return true;

        DEBUG_AUDIO_GRAPH("Connection" << int(this) << "disconnect"
        << int(m_sourceAudioNode) << m_sourceOutputBus << "->"
        << int(m_sinkAudioNode) << m_sinkInputBus)

        AUNode sinkIn = m_sinkAudioNode->getInputAUNode();
	    AUGraphDisconnectNodeInput(graph->audioGraphRef(), sinkIn, m_sinkInputBus);
        m_connected = false;
        return true;
    }

    void AudioConnection::invalidate()
    {
        m_connected = false;
    }

    bool AudioConnection::isBetween(MediaNode *source, MediaNode *sink){
        return (source == m_source) && (sink == m_sink);
    }

    bool AudioConnection::isValid(){
        return (m_sourceAudioNode != 0);
    }

    bool AudioConnection::isSinkOnly(){
        return (m_sourceAudioNode == 0) && (m_sinkAudioNode != 0);
    }

}} //namespace Phonon::QT7

QT_END_NAMESPACE
