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

#include "audiopartoutput.h"

QT_BEGIN_NAMESPACE

namespace Phonon
{
namespace QT7
{

AudioPartOutput::AudioPartOutput()
    : AudioNode()
{
}

AudioPartOutput::~AudioPartOutput()
{
}

ComponentDescription AudioPartOutput::getAudioNodeDescription() const
{
	ComponentDescription description;
	description.componentType = kAudioUnitType_Output;
	description.componentSubType = kAudioUnitSubType_DefaultOutput;
	description.componentManufacturer = kAudioUnitManufacturer_Apple;
	description.componentFlags = 0;
	description.componentFlagsMask = 0;
    return description;
}

void AudioPartOutput::initializeAudioUnit(AudioNode *source)
{
    m_audioStreamDescription = source->outputStreamDescription();
    m_audioChannelLayout = source->outputChannelLayout();
    m_audioChannelLayoutSize = source->outputChannelLayoutSize();

    // Specify the stream format:
    OSStatus err;
	err = AudioUnitSetProperty(m_audioUnit,
	    kAudioUnitProperty_StreamFormat, kAudioUnitScope_Input,
	    0, m_audioStreamDescription, sizeof(AudioStreamBasicDescription));
    BACKEND_ASSERT2(err == noErr, "Could not set stream format on audio output unit.", FATAL_ERROR)

    // Set the channel layout:
	err = AudioUnitSetProperty(m_audioUnit,
	    kAudioUnitProperty_AudioChannelLayout, kAudioUnitScope_Input,
	    0, m_audioChannelLayout, m_audioChannelLayoutSize);
    BACKEND_ASSERT2(err == noErr, "Could not set channel layout on audio output unit.", FATAL_ERROR)
}

}} // namespace Phonon::QT7

QT_END_NAMESPACE
