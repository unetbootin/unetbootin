/*  This file is part of the KDE project.

Copyright (C) 2007 Trolltech ASA. All rights reserved.

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

#include "effect.h"
#include <phonon/effectparameter.h>

#include <medparam.h>
#include <dmo.h>
#include <dmodshow.h>

QT_BEGIN_NAMESPACE

namespace Phonon
{
    namespace DS9
    {
        Effect::Effect(CLSID effectClass, QObject *parent)
            : BackendNode(parent)
        {
            //creation of the filter
            for(QVector<Filter>::iterator it = m_filters.begin(); it != m_filters.end(); ++it) {
                Filter &filter = *it;
                filter = Filter(CLSID_DMOWrapperFilter, IID_IBaseFilter);
                Q_ASSERT(filter);
                ComPointer<IDMOWrapperFilter> wrapper(filter, IID_IDMOWrapperFilter);
                Q_ASSERT(wrapper);
                wrapper->Init(effectClass, DMOCATEGORY_AUDIO_EFFECT);
            }
        }

        Effect::Effect(QObject *parent) : BackendNode(parent)
        {
            //at this point the QVector of Filter should be filled
        }

        Effect::~Effect()
        {
        }

        QList<Phonon::EffectParameter> Effect::parameters() const
        {
            QList<Phonon::EffectParameter> ret;
            if (!m_filters.isEmpty()) {
                ComPointer<IMediaParamInfo> paramInfo(m_filters.first(), IID_IMediaParamInfo);
				if (!paramInfo) {
					return ret;
				}
                DWORD paramCount = 0;
                paramInfo->GetParamCount( &paramCount);

                for(quint32 i = 0; i < paramCount; i++) {
                    MP_PARAMINFO info;
                    HRESULT hr = paramInfo->GetParamInfo(i, &info);
                    Q_ASSERT(SUCCEEDED(hr));
                    WCHAR *name = 0;
                    hr = paramInfo->GetParamText(i, &name);
                    Q_ASSERT(SUCCEEDED(hr));
                    QVariant def, min, max;

                    QVariantList values;

                    switch(info.mpType)
                    {
                    case MPT_ENUM:
                        {
                            WCHAR *current = name;
                            current += wcslen(current) + 1; //skip the name
                            current += wcslen(current) + 1; //skip the unit
                            for(; *current; current += wcslen(current) + 1) {
                                values.append( QString::fromWCharArray(current) );
                            }
                        }
                        //FALLTHROUGH
                    case MPT_INT:
                        def = int(info.mpdNeutralValue);
                        min = int(info.mpdMinValue);
                        max = int(info.mpdMaxValue);
                        break;
                    case MPT_FLOAT:
                        def = info.mpdNeutralValue;
                        min = info.mpdMinValue;
                        max = info.mpdMaxValue;
                        break;
                    case MPT_BOOL:
                        def = bool(info.mpdNeutralValue);
                        break;
                    case MPT_MAX:
                        //Reserved ms-help://MS.PSDKSVR2003R2.1033/directshow/htm/mp_typeenumeration.htm
                        break;
                    }

                    Phonon::EffectParameter::Hints hint = info.mopCaps == MP_CAPS_CURVE_INVSQUARE ?
                        Phonon::EffectParameter::LogarithmicHint : Phonon::EffectParameter::Hints(0);

                    ret.append(Phonon::EffectParameter(i, QString::fromWCharArray(name), hint, def, min, max, values));
					::CoTaskMemFree(name); //let's free the memory
                }
            }
            return ret;
        }

        QVariant Effect::parameterValue(const Phonon::EffectParameter &p) const
        {
            QVariant ret;
            if (!m_filters.isEmpty()) {
                ComPointer<IMediaParams> params(m_filters.first(), IID_IMediaParams);
                Q_ASSERT(params);
                MP_DATA data;
                HRESULT hr = params->GetParam(p.id(), &data);
                Q_ASSERT(SUCCEEDED(hr));
                Q_UNUSED(hr);
                ret = data;
            }
            return ret;
        }

        void Effect::setParameterValue(const Phonon::EffectParameter &p, const QVariant &v)
        {
            if (v.isNull()) {
                return;
            }

            for(QVector<Filter>::iterator it = m_filters.begin(); it != m_filters.end(); ++it) {
                Filter &filter = *it;
                ComPointer<IMediaParams> params(filter, IID_IMediaParams);
                Q_ASSERT(params);

                MP_DATA data = float(v.toDouble());
                params->SetParam(p.id(), data);
            }
        }

    }
}

QT_END_NAMESPACE

#include "moc_effect.cpp"
