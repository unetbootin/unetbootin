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

#include "effect.h"
#include "common.h"
#include "audiooutput.h"
#include "backend.h"
#include "medianode.h"
#include "effectmanager.h"
#include "gsthelper.h"

#include <gst/gst.h>

QT_BEGIN_NAMESPACE

namespace Phonon
{
namespace Gstreamer
{
Effect::Effect(Backend *backend, QObject *parent, NodeDescription description)
        : QObject(parent),
        MediaNode(backend, description)
        , m_effectBin(0)
        , m_effectElement(0)
{
}

void Effect::init()
{
    m_effectBin = createEffectBin();
    if (m_effectBin) {
        setupEffectParams();
        gst_object_ref (GST_OBJECT (m_effectBin)); // Take ownership
        gst_object_sink (GST_OBJECT (m_effectBin));
        m_isValid = true;
    }
}

Effect::~Effect()
{
    if (m_effectBin) {
        gst_element_set_state (m_effectBin, GST_STATE_NULL);
        gst_object_unref (m_effectBin);
    }
}

void Effect::setupEffectParams()
{

    Q_ASSERT(m_effectElement);

    //query and store parameters
    if (m_effectElement) {
        GParamSpec **property_specs;
        guint propertyCount, i;
        property_specs = g_object_class_list_properties(G_OBJECT_GET_CLASS (m_effectElement), &propertyCount);
        for (i = 0; i < propertyCount; i++) {
            GParamSpec *param = property_specs[i];
            if (param->flags & G_PARAM_WRITABLE) {
                QString propertyName = g_param_spec_get_name (param);

                // These properties should not be exposed to the front-end
                if (propertyName == "qos" || propertyName == "name" || propertyName == "async-handling")
                    continue;
 
                switch(param->value_type) {
                    case G_TYPE_UINT:
                        m_parameterList.append(Phonon::EffectParameter(i, propertyName,
                            0,   //hints
                            G_PARAM_SPEC_UINT(param)->default_value,
                            G_PARAM_SPEC_UINT(param)->minimum,
                            G_PARAM_SPEC_UINT(param)->maximum));
                        break;

                    case G_TYPE_STRING:
                        m_parameterList.append(Phonon::EffectParameter(i, propertyName,
                            0,   //hints
                            G_PARAM_SPEC_STRING(param)->default_value,
                            0,
                            0));
                        break;

                    case G_TYPE_INT:
                        m_parameterList.append(Phonon::EffectParameter(i, propertyName,
                            EffectParameter::IntegerHint,   //hints
                            QVariant(G_PARAM_SPEC_INT(param)->default_value),
                            QVariant(G_PARAM_SPEC_INT(param)->minimum),
                            QVariant(G_PARAM_SPEC_INT(param)->maximum)));
                        break;

                    case G_TYPE_FLOAT:
                        m_parameterList.append(Phonon::EffectParameter(i, propertyName,
                            0,   //hints
                            QVariant((double)G_PARAM_SPEC_FLOAT(param)->default_value),
                            QVariant((double)G_PARAM_SPEC_FLOAT(param)->minimum),
                            QVariant((double)G_PARAM_SPEC_FLOAT(param)->maximum)));
                        break;

                    case G_TYPE_DOUBLE:
                        m_parameterList.append(Phonon::EffectParameter(i, propertyName,
                            0,   //hints
                            QVariant(G_PARAM_SPEC_DOUBLE(param)->default_value),
                            QVariant(G_PARAM_SPEC_DOUBLE(param)->minimum),
                            QVariant(G_PARAM_SPEC_DOUBLE(param)->maximum)));
                        break;

                    case G_TYPE_BOOLEAN:
                        m_parameterList.append(Phonon::EffectParameter(i, propertyName, 
                            Phonon::EffectParameter::ToggledHint,   //hints
                            QVariant((bool)G_PARAM_SPEC_BOOLEAN(param)->default_value),
                            QVariant((bool)false), QVariant((bool)true)));
                        break;

                    default:
                        break;
                }
            }
        }
    }
}

QList<Phonon::EffectParameter> Effect::parameters() const
{
    return m_parameterList;
}

QVariant Effect::parameterValue(const EffectParameter &p) const
{

    Q_ASSERT(m_effectElement);

    QVariant returnVal;
    switch (p.type()) {
        case QVariant::Int:
            {
                gint val = 0;
                g_object_get(G_OBJECT(m_effectElement), qPrintable(p.name()), &val, (const char*)NULL);
                returnVal = val;
            }
            break;

        case QVariant::Bool:
            {
                gboolean val = 0;
                g_object_get(G_OBJECT(m_effectElement), qPrintable(p.name()), &val, (const char*)NULL);
                returnVal = val;
            }
            break;

        case QVariant::String:
            {
                gchar *val = 0;
                g_object_get(G_OBJECT(m_effectElement), qPrintable(p.name()), &val, (const char*)NULL);
                returnVal = QString::fromUtf8(val);
                g_free(val);
            }
            break;

        case QVariant::Double:
            {
                GParamSpec* spec = g_object_class_find_property(G_OBJECT_GET_CLASS(m_effectElement), p.name().toLatin1().constData());
                Q_ASSERT(spec);
                if (spec && spec->value_type == G_TYPE_FLOAT) {
                    gfloat val = 0;
                    g_object_get(G_OBJECT(m_effectElement), qPrintable(p.name()), &val, (const char*)NULL);
                    returnVal = QVariant((float)val);
                } else {
                    gdouble val = 0;
                    g_object_get(G_OBJECT(m_effectElement), qPrintable(p.name()), &val, (const char*)NULL);
                    returnVal = QVariant((float)val);
                }
            }
            break;

        default:
            Q_ASSERT(0); //not a supported variant type
    }
    return returnVal;
}


void Effect::setParameterValue(const EffectParameter &p, const QVariant &v)
{
    Q_ASSERT(m_effectElement);

    // Note that the frontend currently calls this after creation with a null-value
    // for all parameters.

    if (v.isValid()) {

        switch (p.type()) {
            // ### range values should really be checked by the front end, why isnt it working?
            case QVariant::Int:
                if (v.toInt() >= p.minimumValue().toInt() && v.toInt() <= p.maximumValue().toInt())
                    g_object_set(G_OBJECT(m_effectElement), qPrintable(p.name()), (gint)v.toInt(), (const char*)NULL);
                break;

            case QVariant::Double:
                if (v.toDouble() >= p.minimumValue().toDouble() && v.toDouble() <= p.maximumValue().toDouble()) {
                    GParamSpec* spec = g_object_class_find_property(G_OBJECT_GET_CLASS(m_effectElement), p.name().toLatin1().constData());
                    Q_ASSERT(spec);
                    if (spec && spec->value_type == G_TYPE_FLOAT)
                        g_object_set(G_OBJECT(m_effectElement), qPrintable(p.name()), (gfloat)v.toDouble(), (const char*)NULL);
                    else
                        g_object_set(G_OBJECT(m_effectElement), qPrintable(p.name()), (gdouble)v.toDouble(), (const char*)NULL);
                }
                break;

            case QVariant::UInt:
                if (v.toUInt() >= p.minimumValue().toUInt() && v.toUInt() <= p.maximumValue().toUInt())
                    g_object_set(G_OBJECT(m_effectElement), qPrintable(p.name()), v.toUInt(), (const char*)NULL);
                break;

            case QVariant::String:
                g_object_set(G_OBJECT(m_effectElement), qPrintable(p.name()), v.toString().toUtf8().constData(), (const char*)NULL);
                break;

            case QVariant::Bool:
                g_object_set(G_OBJECT(m_effectElement), qPrintable(p.name()), (gboolean)v.toBool(), (const char*)NULL);
                break;

            default:
                Q_ASSERT(0); //not a supported variant type
        }
    }
}

}
} //namespace Phonon::Gstreamer

QT_END_NAMESPACE
#include "moc_effect.cpp"
