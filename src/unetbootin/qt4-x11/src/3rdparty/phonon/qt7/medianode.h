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

#ifndef Phonon_QT7_MediaNode_H
#define Phonon_QT7_MediaNode_H

#include <QObject>
#include "backendheader.h"
#include "medianodeevent.h"
#include "audioconnection.h"
#include "videoframe.h"

QT_BEGIN_NAMESPACE

namespace Phonon
{
namespace QT7
{
    class AudioNode;
    class AudioGraph;
    class MediaObject;
    class AudioConnection;

    class MediaNode : public QObject
    {
        Q_OBJECT

        public:
            enum NodeDescriptionEnum {
                AudioSource     = 1,
                AudioSink       = 2,
                VideoSource     = 4,
                VideoSink       = 8,
                AudioGraphNode  = 16
            };
            Q_DECLARE_FLAGS(NodeDescription, NodeDescriptionEnum);

            MediaNode(NodeDescription description, QObject *parent);
            MediaNode(NodeDescription description, AudioNode *audioPart, QObject *parent);
            virtual ~MediaNode();

            void setAudioNode(AudioNode *audioPart);
            bool connectToSink(MediaNode *sink);
            bool disconnectToSink(MediaNode *sink);
            AudioConnection *getAudioConnectionToSink(MediaNode *sink);

            void notify(const MediaNodeEvent *event, bool propagate = true);
            void sendEventToSinks(const MediaNodeEvent *event);
            virtual void mediaNodeEvent(const MediaNodeEvent *event);

            virtual void updateVideo(VideoFrame &frame);
            AudioGraph *m_audioGraph;

            AudioNode *m_audioNode;
            QList<AudioConnection *> m_audioSinkList;
            QList<AudioConnection *> m_audioSourceList;
            QList<MediaNode *> m_videoSinkList;

            int availableAudioInputBus();
            int availableAudioOutputBus();

            NodeDescription m_description;
            MediaObject *m_owningMediaObject;
    };

    Q_DECLARE_OPERATORS_FOR_FLAGS(MediaNode::NodeDescription);

}} // namespace Phonon::QT7

QT_END_NAMESPACE
#endif // Phonon_QT7_MediaNode_H
