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

#ifndef Phonon_QT7_VIDEOEFFECT_H
#define Phonon_QT7_VIDEOEFFECT_H

#include <QtCore>
#include <phonon/effectinterface.h>
#include "medianode.h"
#include "videoframe.h"

QT_BEGIN_NAMESPACE

namespace Phonon
{
    class EffectParameter;

namespace QT7
{
    class MediaNodeEvent;

    class VideoEffect : public MediaNode, public Phonon::EffectInterface
    {
        Q_OBJECT
        Q_INTERFACES(Phonon::EffectInterface)

        public:
            VideoEffect(int effectId, QObject *parent);
            virtual ~VideoEffect();

            QList<EffectParameter> parameters() const;
            QVariant parameterValue(const EffectParameter &) const;
            void setParameterValue(const EffectParameter &, const QVariant &newValue);

            void updateVideo(VideoFrame &frame);

        protected:
            void mediaNodeEvent(const MediaNodeEvent *event);

        private:
            int effectId;
            void *ciFilter;
            QString filterName;
    };
}} // namespace Phonon::QT7

QT_END_NAMESPACE
#endif // Phonon_QT7_VIDEOEFFECT_H
