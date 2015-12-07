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

#include "videoeffect.h"
#include "backendheader.h"
#include "objc_help.h"
#include <phonon/effect.h>
#include <phonon/effectparameter.h>

QT_BEGIN_NAMESPACE

namespace Phonon
{
namespace QT7
{

VideoEffect::VideoEffect(int effectId, QObject *parent) : MediaNode(VideoSink | VideoSource, 0, parent), effectId(effectId)
{
    ciFilter = objc_createCiFilter(effectId);
    if (ciFilter)
        filterName = objc_getCiFilterInfo()->filterDisplayNames[effectId];
}

VideoEffect::~VideoEffect()
{
    if (ciFilter)
        objc_releaseCiFilter(ciFilter);
}

QList<EffectParameter> VideoEffect::parameters() const
{
    IMPLEMENTED;
    return objc_getCiFilterParameterInfo(ciFilter).parameters;
}

QVariant VideoEffect::parameterValue(const EffectParameter &parameter) const
{
    IMPLEMENTED;
    return objc_getCiFilterParameter(ciFilter, parameter.id());
}

void VideoEffect::setParameterValue(const EffectParameter &parameter, const QVariant &newValue)
{
    IMPLEMENTED;
    objc_setCiFilterParameter(ciFilter, parameter.id(), newValue);
}

void VideoEffect::mediaNodeEvent(const MediaNodeEvent */*event*/)
{
}

void VideoEffect::updateVideo(VideoFrame &frame){
    frame.applyCoreImageFilter(ciFilter);
    MediaNode::updateVideo(frame);
}

}}

QT_END_NAMESPACE

#include "moc_videoeffect.cpp"

