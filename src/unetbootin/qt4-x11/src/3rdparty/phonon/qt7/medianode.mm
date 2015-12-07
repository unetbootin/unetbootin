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

#include "medianode.h"
#include "audiograph.h"
#include "audionode.h"
#include "backendheader.h"

#include "mediaobject.h"
#include "audiooutput.h"
#include "quicktimevideoplayer.h"

QT_BEGIN_NAMESPACE

namespace Phonon
{
namespace QT7
{

MediaNode::MediaNode(NodeDescription description, QObject *parent)
    : QObject(parent), m_audioGraph(0), m_audioNode(0), m_description(description), m_owningMediaObject(0)
{
}

MediaNode::MediaNode(NodeDescription description, AudioNode *audioPart, QObject *parent)
    : QObject(parent), m_audioGraph(0), m_audioNode(audioPart), m_description(description)
{
}

void MediaNode::setAudioNode(AudioNode *audioPart)
{
    if (m_audioNode)
        delete m_audioNode;
    m_audioNode = audioPart;
}

MediaNode::~MediaNode()
{
   delete m_audioNode;
   qDeleteAll(m_audioSinkList);
}

AudioConnection *MediaNode::getAudioConnectionToSink(MediaNode *sink)
{
    AudioConnection *connection = 0;
    for (int i=0; i<m_audioSinkList.size(); ++i){
        if (m_audioSinkList[i]->isBetween(this, sink)){
            connection = m_audioSinkList[i];
            break;
        }
    }
    return connection;
}

bool MediaNode::connectToSink(MediaNode *sink)
{
    if ((m_description & AudioSource) && (sink->m_description & AudioSink)){
        // Check that they don't belong to different graphs. If they do, but
        // sink is not connected to any source, accept it:
        if (m_owningMediaObject && sink->m_owningMediaObject
            && m_owningMediaObject != sink->m_owningMediaObject
            && !sink->m_audioSourceList.isEmpty()){
            return false;
        }

        // Check that the connection doesn't already exists:
        AudioConnection *connection = getAudioConnectionToSink(sink);
        if (connection)
            return true;

        // Check that there are awailable input/output busses:
        int inputBus = sink->availableAudioInputBus();
        int outputBus = availableAudioOutputBus();
        if (inputBus >= sink->m_audioNode->m_maxInputBusses || outputBus >= m_audioNode->m_maxOutputBusses)
            return false;

        // All OK. Create connection:
        connection = new AudioConnection(this, outputBus, sink, inputBus);
        m_audioSinkList << connection;
        sink->m_audioSourceList << connection;

        if (m_audioNode->m_audioGraph)
            m_audioNode->m_audioGraph->connectLate(connection);

        MediaNodeEvent event1(MediaNodeEvent::AudioSinkAdded, connection);
        notify(&event1, false);
        MediaNodeEvent event2(MediaNodeEvent::AudioSourceAdded, connection);
        sink->notify(&event2, false);
        return true;
    }

    if ((m_description & VideoSource) && (sink->m_description & VideoSink)){
        // Check that the connection doesn't already exists:
        if (m_videoSinkList.contains(sink))
            return true;

        m_videoSinkList << sink;
        MediaNodeEvent event1(MediaNodeEvent::VideoSinkAdded, sink);
        notify(&event1, false);
        MediaNodeEvent event2(MediaNodeEvent::VideoSourceAdded, this);
        sink->notify(&event2, false);
        return true;
    }

    return false;
}

bool MediaNode::disconnectToSink(MediaNode *sink)
{
    if ((m_description & AudioSource) && (sink->m_description & AudioSink)){
        AudioConnection *connection = getAudioConnectionToSink(sink);
        if (!connection)
            return false;

        m_audioSinkList.removeOne(connection);
        sink->m_audioSourceList.removeOne(connection);

        if (m_audioNode->m_audioGraph)
            m_audioNode->m_audioGraph->disconnectLate(connection);

        MediaNodeEvent event1(MediaNodeEvent::AudioSinkRemoved, connection);
        notify(&event1, false);
        MediaNodeEvent event2(MediaNodeEvent::AudioSourceRemoved, connection);
        sink->notify(&event2, false);
        
        delete connection;
        return true;
    }

    if ((m_description & VideoSource) && (sink->m_description & VideoSink)){
        m_videoSinkList.removeOne(sink);

        MediaNodeEvent event1(MediaNodeEvent::VideoSinkRemoved, sink);
        notify(&event1, false);
        MediaNodeEvent event2(MediaNodeEvent::VideoSourceRemoved, this);
        sink->notify(&event2, false);
        return true;
    }

    return false;
}

int MediaNode::availableAudioInputBus()
{
    // Scan through all the connection <u>in</u> to this
    // node, and find an awailable index:
    int index = -1;
    bool available = false;

    while (!available){
        ++index;
        available = true;
        for (int i=0; i<m_audioSourceList.size(); ++i){
            if (m_audioSourceList[i]->m_sinkInputBus == index){
                available = false;
                break;
            }
        }
    }
    return index;
}

int MediaNode::availableAudioOutputBus()
{
    // Scan through all the connection <u>out</u> from this
    // node, and find an awailable index:
    int bus = -1;
    bool available = false;

    while (!available){
        ++bus;
        available = true;
        for (int i=0; i<m_audioSinkList.size(); ++i){
            if (m_audioSinkList[i]->m_sourceOutputBus == bus){
                available = false;
                break;
            }
        }
    }
    return bus;
}

void MediaNode::notify(const MediaNodeEvent *event, bool propagate)
{
    // Let subclass handle the event first:
    mediaNodeEvent(event);

    switch(event->type()){
    case MediaNodeEvent::AudioGraphAboutToBeDeleted:
        if (m_audioNode){
            foreach(AudioConnection *connection, m_audioSinkList)
                connection->invalidate();
        }
        break;
    case MediaNodeEvent::NewAudioGraph:
        m_audioGraph = static_cast<AudioGraph *>(event->data());
        break;
    case MediaNodeEvent::AudioSinkAdded:
    case MediaNodeEvent::VideoSinkAdded:
        if (m_owningMediaObject){
            MediaNodeEvent e1(MediaNodeEvent::SetMediaObject, m_owningMediaObject);
            sendEventToSinks(&e1);
            QRect videoRect = m_owningMediaObject->videoPlayer()->videoRect();
            MediaNodeEvent e2(MediaNodeEvent::VideoFrameSizeChanged, &videoRect);
            sendEventToSinks(&e2);
        }
        break;
    case MediaNodeEvent::SetMediaObject:
        m_owningMediaObject = static_cast<MediaObject *>(event->data());
        break;
    default:
        break;
    }

    // Inform the audio node as well:
    if (m_audioNode)
        m_audioNode->notify(event);

    // And perhaps the sinks:
    if (propagate)
        sendEventToSinks(event);
}

void MediaNode::sendEventToSinks(const MediaNodeEvent *event)
{
    for (int i=0; i<m_audioSinkList.size(); ++i)
        m_audioSinkList[i]->m_sink->notify(event);
    for (int i=0; i<m_videoSinkList.size(); ++i)
        m_videoSinkList[i]->notify(event);
}

void MediaNode::updateVideo(VideoFrame &frame){
    for (int i=0; i<m_videoSinkList.size(); ++i)
        m_videoSinkList[i]->updateVideo(frame);
}

void MediaNode::mediaNodeEvent(const MediaNodeEvent */*event*/)
{
   // Override if needed.
}

}} //namespace Phonon::QT7

QT_END_NAMESPACE

#include "moc_medianode.cpp"

