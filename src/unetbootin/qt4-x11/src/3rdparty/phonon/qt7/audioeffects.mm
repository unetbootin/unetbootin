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

#include "audioeffects.h"
#include <QtCore>

QT_BEGIN_NAMESPACE

namespace Phonon
{
namespace QT7
{

AudioEffectAudioNode::AudioEffectAudioNode(int effectType)
    : AudioNode(1, 1), m_effectType(effectType)
{
}

ComponentDescription AudioEffectAudioNode::getAudioNodeDescription() const
{
	ComponentDescription d;
	d.componentType = kAudioUnitType_Effect;
	d.componentSubType = m_effectType;
	d.componentManufacturer = kAudioUnitManufacturer_Apple;
	d.componentFlags = 0;
	d.componentFlagsMask = 0;
    return d;
}

void AudioEffectAudioNode::initializeAudioUnit()
{
    if (!m_audioUnit)
        return;
    foreach(int id, m_alteredParameters.keys()){
        Float32 value = m_alteredParameters.value(id);
        ComponentResult res = AudioUnitSetParameter(m_audioUnit, id, kAudioUnitScope_Global, 0, value, 0);
        BACKEND_ASSERT2(res == noErr, "Could not initialize audio effect.", NORMAL_ERROR)
    }
}

QVariant AudioEffectAudioNode::parameterValue(const Phonon::EffectParameter &parameter) const
{
    if (m_audioUnit){
        Float32 value = 0;
        AudioUnitGetParameter(m_audioUnit, parameter.id(), kAudioUnitScope_Global, 0, &value);
        return QVariant(value);
    } else if (m_alteredParameters.contains(parameter.id())){
        return QVariant(m_alteredParameters.value(parameter.id()));
    } else {
        // Use default value:
        AudioUnit tmpAudioUnit;
        ComponentDescription description = getAudioNodeDescription();
        Component component = FindNextComponent(0, &description);
        BACKEND_ASSERT3(component, "Could not get parameters of audio effect.", NORMAL_ERROR, QVariant())
        OSErr err = OpenAComponent(component, &tmpAudioUnit);
        BACKEND_ASSERT3(err == noErr, "Could not get parameters of audio effect.", NORMAL_ERROR, QVariant())
        AudioUnitParameterInfo info;
        UInt32 size = sizeof(info);
        ComponentResult res = AudioUnitGetProperty(tmpAudioUnit,
            kAudioUnitProperty_ParameterInfo, kAudioUnitScope_Global, parameter.id(), &info, &size);
        BACKEND_ASSERT3(res == noErr, "Could not get parameter info from audio effect.", NORMAL_ERROR, QVariant())
        return QVariant(info.defaultValue);
    }
}

void AudioEffectAudioNode::setParameterValue(const Phonon::EffectParameter &parameter, const QVariant &newValue)
{
    Float32 value = 0;
    if (newValue.isValid()){
        value = newValue.toDouble();
        m_alteredParameters.insert(parameter.id(), value);
    } else {
        // Use default value:
        m_alteredParameters.remove(parameter.id());
        if (m_audioUnit){
            AudioUnit tmpAudioUnit;
            ComponentDescription description = getAudioNodeDescription();
            Component component = FindNextComponent(0, &description);
            BACKEND_ASSERT2(component, "Could not get parameters of audio effect.", NORMAL_ERROR)
            OSErr err = OpenAComponent(component, &tmpAudioUnit);
            BACKEND_ASSERT2(err == noErr, "Could not get parameters of audio effect.", NORMAL_ERROR)
            AudioUnitParameterInfo info;
            UInt32 size = sizeof(info);
            ComponentResult res = AudioUnitGetProperty(tmpAudioUnit,
                kAudioUnitProperty_ParameterInfo, kAudioUnitScope_Global, parameter.id(), &info, &size);
            BACKEND_ASSERT2(res == noErr, "Could not get parameter info from audio effect.", NORMAL_ERROR)
            value = info.defaultValue;
        }
    }

    if (m_audioUnit){
        ComponentResult res = AudioUnitSetParameter(m_audioUnit, parameter.id(), kAudioUnitScope_Global, 0, value, 0);
        BACKEND_ASSERT2(res == noErr, "Could not set effect parameter value.", NORMAL_ERROR)
    }
}

///////////////////////////////////////////////////////////////////////

AudioEffect::AudioEffect(int effectType, QObject *parent)
    : MediaNode(AudioSink | AudioSource, 0, parent)
{
    m_audioNode = new AudioEffectAudioNode(effectType);
    setAudioNode(m_audioNode);
}

QList<Phonon::EffectParameter> AudioEffect::parameters() const
{
    QList<Phonon::EffectParameter> effectList;
    // Create a temporary audio unit:
    AudioUnit audioUnit;
    ComponentDescription description = m_audioNode->getAudioNodeDescription();
    Component component = FindNextComponent(0, &description);
    BACKEND_ASSERT3(component, "Could not get parameters of audio effect.", NORMAL_ERROR, effectList)
    OSErr err = OpenAComponent(component, &audioUnit);
    BACKEND_ASSERT3(err == noErr, "Could not get parameters of audio effect.", NORMAL_ERROR, effectList)

    UInt32 size = 0;
    // Get parameter count:
    ComponentResult res = AudioUnitGetProperty(audioUnit, 
        kAudioUnitProperty_ParameterList, kAudioUnitScope_Global, 0, 0, &size);
    BACKEND_ASSERT3(res == noErr, "Could not get parameter count from audio effect.", NORMAL_ERROR, effectList)
    int paramCount = size / sizeof(AudioUnitParameterID);

    // Get parameters:
    AudioUnitParameterID parameters[paramCount];
    res = AudioUnitGetProperty(audioUnit, 
        kAudioUnitProperty_ParameterList, kAudioUnitScope_Global, 0, &parameters, &size);
    BACKEND_ASSERT3(res == noErr, "Could not get parameter list from audio effect.", NORMAL_ERROR, effectList)

    for (int i=0; i<paramCount; ++i)
        effectList << createParameter(audioUnit, parameters[i]);
    
    CloseComponent(audioUnit);
    return effectList;
}

QString AudioEffect::name()
{
    ComponentDescription description = m_audioNode->getAudioNodeDescription();
    Component component = FindNextComponent(0, &description);
    BACKEND_ASSERT3(component, "Could not get audio effect name.", NORMAL_ERROR, QLatin1String("<unknown effect>"))

    ComponentDescription cDesc;
    Handle nameH = NewHandle(0);
    GetComponentInfo(component, &cDesc, nameH, 0, 0); 
    HLock(nameH);
    char *namePtr = *nameH;
    int len = *namePtr++;
    namePtr[len] = 0;
    QString qsName = QString::fromUtf8(namePtr);
    DisposeHandle(nameH);
    return qsName;
}

QString AudioEffect::description()
{
    ComponentDescription description = m_audioNode->getAudioNodeDescription();
    Component component = FindNextComponent(0, &description);
    BACKEND_ASSERT3(component, "Could not get audio effect description.", NORMAL_ERROR, QLatin1String("<unknown effect>"))

    ComponentDescription cDesc;
    Handle descH = NewHandle(0);
    GetComponentInfo(component, &cDesc, 0, descH, 0); 
    HLock(descH);
    char *descPtr = *descH;
    int len = *descPtr++;
    descPtr[len] = 0;
    QString qsDesc = QString::fromUtf8(descPtr);
    DisposeHandle(descH);
    return qsDesc;
}

QList<int> AudioEffect::effectList()
{
    QList<int> effects;

	ComponentDescription d;
	d.componentType = kAudioUnitType_Effect;
	d.componentSubType = 0;
	d.componentManufacturer = 0;
	d.componentFlags = 0;
	d.componentFlagsMask = 0;
    Component component = FindNextComponent(0, &d);
    
    while (component) {
        ComponentDescription cDesc;
        GetComponentInfo(component, &cDesc, 0, 0, 0); 
        effects << cDesc.componentSubType;
        component = FindNextComponent(component, &d);
    }
    return effects;
}

Phonon::EffectParameter AudioEffect::createParameter(const AudioUnit &audioUnit, const AudioUnitParameterID &id) const
{
    AudioUnitParameterInfo info;
    UInt32 size = sizeof(info);
    ComponentResult res = AudioUnitGetProperty(audioUnit,
        kAudioUnitProperty_ParameterInfo, kAudioUnitScope_Global, id, &info, &size);
    BACKEND_ASSERT3(res == noErr, "Could not get parameter info from audio effect.", NORMAL_ERROR, Phonon::EffectParameter())
    
    QString name = info.flags & kAudioUnitParameterFlag_HasCFNameString
        ? PhononCFString::toQString(info.cfNameString) : QLatin1String("<unknown parameter>");
        
    Phonon::EffectParameter::Hint hint;
    switch(info.unit){
    case (kAudioUnitParameterUnit_Indexed):
    case (kAudioUnitParameterUnit_Seconds):
    case (kAudioUnitParameterUnit_SampleFrames):
    case (kAudioUnitParameterUnit_Milliseconds):
        hint = Phonon::EffectParameter::IntegerHint;
        break;
    case (kAudioUnitParameterUnit_Boolean):
        hint = Phonon::EffectParameter::ToggledHint;
        break;
    default:
        hint = Phonon::EffectParameter::LogarithmicHint;
        break;
    }
    
    QVariant def(info.defaultValue);    
    QVariant min(info.minValue);
    QVariant max(info.maxValue);
    return Phonon::EffectParameter(id, name, hint, def, min, max, QVariantList(), name);
}

QVariant AudioEffect::parameterValue(const Phonon::EffectParameter &value) const
{
    return m_audioNode->parameterValue(value);
}

void AudioEffect::setParameterValue(const Phonon::EffectParameter &parameter, const QVariant &newValue)
{
    m_audioNode->setParameterValue(parameter, newValue);
}

}} //namespace Phonon::QT7

QT_END_NAMESPACE

#include "moc_audioeffects.cpp"
