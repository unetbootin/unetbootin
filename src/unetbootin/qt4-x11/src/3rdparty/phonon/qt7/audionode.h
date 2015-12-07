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

#ifndef Phonon_QT7_AudioNode_H
#define Phonon_QT7_AudioNode_H

#include <QObject>
#include "backendheader.h"
#include "audioconnection.h"
#include <AudioToolbox/AudioToolbox.h>
#include <AudioUnit/AudioUnit.h>

QT_BEGIN_NAMESPACE

namespace Phonon
{
namespace QT7
{
    class AudioGraph;
    class MediaNodeEvent;
    class MediaNode;

    class MediaNodeConnection{
        MediaNode *source;
        MediaNode *sink;
        int inputPort;
        int outputPort;
    };

    class AudioNode
    {
        public:
            enum ConnectionSide {Source, Sink};

            AudioNode(int maxInput, int maxOutput);
            virtual ~AudioNode();

            virtual void createAndConnectAUNodes();
            virtual void createAudioUnits();
            virtual void setGraph(AudioGraph *audioGraph);
            virtual AUNode getInputAUNode();
            virtual AUNode getOutputAUNode();
            virtual bool fillInStreamSpecification(AudioConnection *connection, ConnectionSide side);
            virtual bool setStreamSpecification(AudioConnection *connection, ConnectionSide side);
            void notify(const MediaNodeEvent *event);

            virtual void mediaNodeEvent(const MediaNodeEvent *event);
            Float64 getTimeInSamples(int timeProperty);
            
            AudioGraph *m_audioGraph;    
            AudioConnection *m_lastConnectionIn;

            int m_maxInputBusses;
            int m_maxOutputBusses;

        protected:
	        AUNode m_auNode;
            AudioUnit m_audioUnit;

            // Only the following methods needs to
            // be overidden by only_one-audio-unit nodes:
            virtual ComponentDescription getAudioNodeDescription() const;
            virtual void initializeAudioUnit();

        private:
            bool setStreamHelp(AudioConnection *c, int bus, OSType scope, bool fromSource);
    };
}} // namespace Phonon::QT7

QT_END_NAMESPACE

#endif // Phonon_QT7_AudioNode_H
