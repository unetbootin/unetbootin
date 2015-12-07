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

#include "audiomixer.h"
#include <QtCore>

QT_BEGIN_NAMESPACE

namespace Phonon
{
namespace QT7
{

AudioMixerAudioNode::AudioMixerAudioNode() : AudioNode(30, 1)
{
    m_numberOfBusses = 2;
    m_volume = 1.0f;
}

ComponentDescription AudioMixerAudioNode::getAudioNodeDescription() const
{
	ComponentDescription description;
	description.componentType = kAudioUnitType_Mixer;
	description.componentSubType = kAudioUnitSubType_StereoMixer;
	description.componentManufacturer = kAudioUnitManufacturer_Apple;
	description.componentFlags = 0;
	description.componentFlagsMask = 0;
    return description;
}

void AudioMixerAudioNode::initializeAudioUnit()
{
    // Set bus count:
    OSStatus err = AudioUnitSetProperty(m_audioUnit,
        kAudioUnitProperty_BusCount, kAudioUnitScope_Input, 0, &m_numberOfBusses, sizeof(int));
    BACKEND_ASSERT2(err == noErr, "Could not set number of busses on audio mixer node.", FATAL_ERROR)
}

void AudioMixerAudioNode::setVolume(float volume, int bus)
{
    if (volume < 0)
        m_volume = 0;
    else if (volume > 1)
        m_volume = 1;
    else
        m_volume = volume;

    if (m_audioUnit){
//        Float32 db = Float32(volume);//Float32(20.0 * log10(volume)); // convert to db
        Float32 db = Float32(volume);
        OSStatus err = AudioUnitSetParameter(m_audioUnit, kStereoMixerParam_Volume, kAudioUnitScope_Input, bus, db, 0);
        BACKEND_ASSERT2(err == noErr, "Could not set volume on audio mixer node.", NORMAL_ERROR)
    }
}

float AudioMixerAudioNode::volume(int bus)
{
    if (!m_audioUnit)
        return 0;

    Float32 db;
    OSStatus err = AudioUnitGetParameter(m_audioUnit, kStereoMixerParam_Volume, kAudioUnitScope_Input, bus, &db);
    BACKEND_ASSERT3(err == noErr, "Could not get volume on audio mixer node.", NORMAL_ERROR, 0)
    return float(db);
}

///////////////////////////////////////////////////////////////////////

AudioMixer::AudioMixer(QObject *parent) : MediaNode(AudioSink | AudioSource, 0, parent)
{
    m_audioNode = new AudioMixerAudioNode();
    setAudioNode(m_audioNode);
    m_fadeCurve = Phonon::VolumeFaderEffect::Fade3Decibel;
    m_fadeTimer = 0;
    m_fadeDuration = 0;
    m_fadeFromVolume = 0;
    m_fadeToVolume = 0;
}

AudioMixer::~AudioMixer()
{
    if (m_fadeTimer)
        killTimer(m_fadeTimer);
}

QList<Phonon::EffectParameter> AudioMixer::parameters() const
{
    QList<Phonon::EffectParameter> ret;
    return ret;
}

QVariant AudioMixer::parameterValue(const Phonon::EffectParameter &value) const
{
    NOT_IMPLEMENTED;
    Q_UNUSED(value);
    return QVariant();
}

void AudioMixer::setParameterValue(const Phonon::EffectParameter &parameter, const QVariant &newValue)
{
    NOT_IMPLEMENTED;
    Q_UNUSED(parameter);
    Q_UNUSED(newValue);
}

float AudioMixer::volume() const
{
    return m_audioNode->volume(0);
}

void AudioMixer::setVolume(float volume)
{
    m_audioNode->setVolume(volume, 0);
}

Phonon::VolumeFaderEffect::FadeCurve AudioMixer::fadeCurve() const
{
    return m_fadeCurve;
}

void AudioMixer::setFadeCurve(Phonon::VolumeFaderEffect::FadeCurve fadeCurve)
{
    m_fadeCurve = fadeCurve;
}

void AudioMixer::fadeTo(float volume, int fadeTime)
{
    m_fadeToVolume = volume;
    m_fadeDuration = fadeTime;
    m_fadeFromVolume = m_audioNode->volume(0);
    
    m_fadeStartTime.start();
    if (m_fadeTimer)
        killTimer(m_fadeTimer);
    m_fadeTimer = startTimer(100);
}

void AudioMixer::updateFade()
{
    float step = float(m_fadeStartTime.elapsed()) / float(m_fadeDuration);
    if (step > 1){
        step = 1;
        if (m_fadeTimer)
            killTimer(m_fadeTimer);
    }
    float volume = m_fadeFromVolume + ((m_fadeToVolume - m_fadeFromVolume) * step);
    m_audioNode->setVolume(volume, 0);
}

bool AudioMixer::event(QEvent *event)
{
    switch (event->type()){
        case QEvent::Timer:{
            QTimerEvent *timerEvent = static_cast<QTimerEvent *>(event);
            if (timerEvent->timerId() == m_fadeTimer)
                updateFade();
            break; }
        default:
            break;
    }
    return MediaNode::event(event);
}

}} //namespace Phonon::QT7

QT_END_NAMESPACE

#include "moc_audiomixer.cpp"
