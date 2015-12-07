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

#include "volumeeffect.h"
#include "qbasefilter.h"
#include "qmeminputpin.h"

#include <QtCore/qmath.h> //for sqrt

QT_BEGIN_NAMESPACE

#ifndef QT_NO_PHONON_VOLUMEFADEREFFECT


namespace Phonon
{
    namespace DS9
    {
        /**************************************************************************
        * curve functions
        *************************************************************************/

        static qreal curveValueFadeIn3dB(const qreal fadeStart, const qreal fadeDiff, const qreal completed)
        {
            return (fadeStart + fadeDiff * sqrt(completed));
        }
        static qreal curveValueFadeOut3dB(const qreal fadeStart, const qreal fadeDiff, const qreal completed)
        {
            return (fadeStart + fadeDiff * (1.0 - sqrt(1.0 - completed)));
        }
        // in == out for a linear fade
        static qreal curveValueFade6dB(const qreal fadeStart, const qreal fadeDiff, const qreal completed)
        {
            return (fadeStart + fadeDiff * completed);
        }
        static qreal curveValueFadeIn9dB(const qreal fadeStart, const qreal fadeDiff, const qreal completed)
        {
            return (fadeStart + fadeDiff * pow(completed, 1.5));
        }
        static qreal curveValueFadeOut9dB(const qreal fadeStart, const qreal fadeDiff, const qreal completed)
        {
            return (fadeStart + fadeDiff * (1.0 - pow(1.0 - completed, 1.5)));
        }
        static qreal curveValueFadeIn12dB(const qreal fadeStart, const qreal fadeDiff, const qreal completed)
        {
            return (fadeStart + fadeDiff * completed * completed);
        }
        static qreal curveValueFadeOut12dB(const qreal fadeStart, const qreal fadeDiff, const qreal completed)
        {
            const qreal x = 1.0 - completed;
            return (fadeStart + fadeDiff * (1.0 - x * x));
        }

        static const QVector<AM_MEDIA_TYPE> audioMediaType()
        {
            QVector<AM_MEDIA_TYPE> ret;

            AM_MEDIA_TYPE mt;
            mt.majortype = MEDIATYPE_Audio;
            mt.subtype = MEDIASUBTYPE_PCM;
            mt.bFixedSizeSamples = 1;
            mt.bTemporalCompression = 0;
            mt.pUnk = 0;
            mt.lSampleSize = 1;
            mt.cbFormat = 0;
            mt.pbFormat = 0;
            mt.formattype = GUID_NULL;
            ret << mt;
            return ret;
        }

        class VolumeMemInputPin : public QMemInputPin
        {
        public:
            VolumeMemInputPin(QBaseFilter *parent, const QVector<AM_MEDIA_TYPE> &mt) : QMemInputPin(parent, mt, true /*transform*/)
            {
            }

            ~VolumeMemInputPin()
            {
            }

            STDMETHODIMP NotifyAllocator(IMemAllocator *alloc, BOOL b)
            {
                ALLOCATOR_PROPERTIES prop;
                HRESULT hr = alloc->GetProperties(&prop);
                if (SUCCEEDED(hr) && prop.cBuffers > 1) {
                    //this allows to reduce the latency for sound
                    //the problem is that too low numbers makes the whole thing fail...
                    ALLOCATOR_PROPERTIES actual;
                    prop.cBuffers = 1;
                    alloc->SetProperties(&prop, &actual);
                }
                return QMemInputPin::NotifyAllocator(alloc, b);
            }

        };

        class VolumeMemOutputPin : public QPin
        {
        public:
            VolumeMemOutputPin(QBaseFilter *parent, const QVector<AM_MEDIA_TYPE> &mt) : QPin(parent, PINDIR_OUTPUT, mt)
            {
            }

            ~VolumeMemOutputPin()
            {
            }

        };

        class VolumeEffectFilter : public QBaseFilter
        {
        public:
            VolumeEffectFilter(VolumeEffect *);

            //reimplementation
            virtual HRESULT processSample(IMediaSample *);

        private:
            void treatOneSamplePerChannel(BYTE **buffer, int sampleSize, int channelCount, int frequency);

            QMemInputPin *m_input;
            QPin *m_output;
            VolumeEffect *m_volumeEffect;
        };

        VolumeEffectFilter::VolumeEffectFilter(VolumeEffect *ve) : QBaseFilter(CLSID_NULL),
            m_volumeEffect(ve)
        {
            QVector<AM_MEDIA_TYPE> mt;

            //creating the output
            m_output = new VolumeMemOutputPin(this, mt);

            //then creating the input
            mt << audioMediaType();
            m_input = new VolumeMemInputPin(this, mt);
            m_input->addOutput(m_output); //make the connection here
        }

        void VolumeEffectFilter::treatOneSamplePerChannel(BYTE **buffer, int sampleSize, int channelCount, int frequency)
        {
            float volume = m_volumeEffect->volume();
            if (m_volumeEffect->m_fading) {
                const qreal lastSample = m_volumeEffect->m_fadeDuration * frequency / 1000;
                const qreal completed = qreal(m_volumeEffect->m_fadeSamplePosition++) / lastSample;

                if (qFuzzyCompare(completed, qreal(1.))) {
                    m_volumeEffect->setVolume(m_volumeEffect->m_targetVolume);
                    m_volumeEffect->m_fading = false; //we end the fading effect
                } else {
                    volume = m_volumeEffect->m_fadeCurveFn(m_volumeEffect->m_initialVolume, 
                        m_volumeEffect->m_targetVolume - m_volumeEffect->m_initialVolume,
                        completed);
                }
            }

            for(int c = 0; c < channelCount; ++c) {
                switch (sampleSize)
                {
                case 2:
                    {
                        short *shortBuffer = reinterpret_cast<short*>(*buffer);
                        *shortBuffer *= qRound(volume);
                    }
                    break;
                case 1:
                    **buffer *= qRound(volume);
                    break;
                default:
                    break;
                }

                *buffer += sampleSize;
            }
        }

        HRESULT VolumeEffectFilter::processSample(IMediaSample * ms)
        {
            BYTE *buffer = 0;
            ms->GetPointer(&buffer);
            if (buffer) {
                const AM_MEDIA_TYPE &mt = m_output->connectedType();
                if (mt.formattype != FORMAT_WaveFormatEx) {
                    return VFW_E_INVALIDMEDIATYPE;
                }
                WAVEFORMATEX *format = reinterpret_cast<WAVEFORMATEX*>(mt.pbFormat);
                const int channelCount = format->nChannels;
                const int sampleSize = format->wBitsPerSample / 8; //...in bytes


                const BYTE *end = buffer + ms->GetActualDataLength();
                while (buffer < end) {
                    treatOneSamplePerChannel(&buffer, sampleSize, channelCount, format->nSamplesPerSec);
                }
            }

            return S_OK;
        }

        VolumeEffect::VolumeEffect(QObject *parent) : Effect(parent), 
            m_volume(1), m_fadeCurve(Phonon::VolumeFaderEffect::Fade3Decibel),
            m_fading(false), m_initialVolume(0), m_targetVolume(0), m_fadeDuration(0),
            m_fadeSamplePosition(0)
        {
            //creation of the effects
            for(int i = 0; i < FILTER_COUNT; ++i) {
                VolumeEffectFilter *f = new VolumeEffectFilter(this);
                m_filters[i] = Filter(f);
            }
        }

        float VolumeEffect::volume() const
        {
            return m_volume;
        }

        void VolumeEffect::setVolume(float newVolume)
        {
            m_volume = newVolume;
        }

        Phonon::VolumeFaderEffect::FadeCurve VolumeEffect::fadeCurve() const
        {
            return m_fadeCurve;
        }

        void VolumeEffect::setFadeCurve(Phonon::VolumeFaderEffect::FadeCurve curve)
        {
            m_fadeCurve = curve;
        }


        void VolumeEffect::fadeTo(float vol, int duration)
        {
            m_fading = true; //will be set back to false when fading is finished
            m_targetVolume = vol;
            m_fadeSamplePosition = 0;
            m_initialVolume = m_volume;
            m_fadeDuration = duration;

            //in or out?
            const bool in = vol > m_volume;

            switch(m_fadeCurve)
            {
            case Phonon::VolumeFaderEffect::Fade6Decibel:
                m_fadeCurveFn = curveValueFade6dB;
                break;
            case Phonon::VolumeFaderEffect::Fade9Decibel:
                if (in) {
                    m_fadeCurveFn = curveValueFadeIn9dB;
                } else {
                    m_fadeCurveFn = curveValueFadeOut9dB;
                }
                break;
            case Phonon::VolumeFaderEffect::Fade12Decibel:
                if (in) {
                    m_fadeCurveFn = curveValueFadeIn12dB;
                } else {
                    m_fadeCurveFn = curveValueFadeOut12dB;
                }
                break;
            case Phonon::VolumeFaderEffect::Fade3Decibel:
            default:
                if (in) {
                    m_fadeCurveFn = curveValueFadeIn3dB;
                } else {
                    m_fadeCurveFn = curveValueFadeOut3dB;
                }
                break;
            }
        }
    }
}

#endif //QT_NO_PHONON_VOLUMEFADEREFFECT

QT_END_NAMESPACE

#include "moc_volumeeffect.cpp"
