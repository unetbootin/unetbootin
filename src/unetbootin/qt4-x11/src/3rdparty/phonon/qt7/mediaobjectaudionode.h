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

#ifndef Phonon_QT7_MEDIAOBJECTAUDIONODE_H
#define Phonon_QT7_MEDIAOBJECTAUDIONODE_H

#include <QtCore>
#include "audionode.h"

QT_BEGIN_NAMESPACE

namespace Phonon
{
namespace QT7
{
    class QuickTimeAudioPlayer;
    class AudioMixerAudioNode;
    class AudioConnection;

    class MediaObjectAudioNode : public AudioNode
    {
        public:
            MediaObjectAudioNode(QuickTimeAudioPlayer *player1, QuickTimeAudioPlayer *player2);
            ~MediaObjectAudioNode();

            // Overridden section from AudioNode:
            void createAndConnectAUNodes();
            void createAudioUnits();
            void setGraph(AudioGraph *audioGraph);
            AUNode getOutputAUNode();
            bool fillInStreamSpecification(AudioConnection *connection, ConnectionSide side);
            bool setStreamSpecification(AudioConnection *connection, ConnectionSide side);

            void startCrossFade(qint64 duration);
            void updateCrossFade(qint64 currentTime);
            void cancelCrossFade();
            void setMute(bool mute);
            bool isCrossFading();
            
            QuickTimeAudioPlayer *m_player1;
            QuickTimeAudioPlayer *m_player2;
            AudioMixerAudioNode *m_mixer;

            AudioConnection *m_connection1;
            AudioConnection *m_connection2;

            float m_fadeDuration;
            float m_volume1;
            float m_volume2;
            float m_mute;
            
            float applyCurve(float volume);
            void updateVolume();

            void mediaNodeEvent(const MediaNodeEvent *event);
    };

}} //namespace Phonon::QT7

QT_END_NAMESPACE
#endif // Phonon_QT7_MEDIAOBJECTAUDIONODE_H
