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

#include "common.h"
#include "backend.h"
#include "medianode.h"
#include "effectmanager.h"
#include "audioeffect.h"
#include "gsthelper.h"

#include <gst/gst.h>

QT_BEGIN_NAMESPACE

namespace Phonon
{
namespace Gstreamer
{
AudioEffect::AudioEffect(Backend *backend, int effectId, QObject *parent)
        : Effect(backend, parent, AudioSource | AudioSink)
{
    static int count = 0;
    m_name = "AudioEffect" + QString::number(count++);
    QList<EffectInfo*> audioEffects = backend->effectManager()->audioEffects();
    if (effectId >= 0 && effectId < audioEffects.size()) {
        m_effectName = audioEffects[effectId]->name();
        init();
    } else {
        Q_ASSERT(0); // Effect ID out of range
    }
}

GstElement* AudioEffect::createEffectBin()
{
    GstElement *audioBin = gst_bin_new(NULL);

    // We need a queue to handle tee-connections from parent node
    GstElement *queue= gst_element_factory_make ("queue", NULL);
    gst_bin_add(GST_BIN(audioBin), queue);

    GstElement *mconv= gst_element_factory_make ("audioconvert", NULL);
    gst_bin_add(GST_BIN(audioBin), mconv);

    m_effectElement = gst_element_factory_make (qPrintable(m_effectName), NULL);
    gst_bin_add(GST_BIN(audioBin), m_effectElement);

    //Link src pad
    GstPad *srcPad= gst_element_get_pad (m_effectElement, "src");
    gst_element_add_pad (audioBin, gst_ghost_pad_new ("src", srcPad));
    gst_object_unref (srcPad);

    //Link sink pad
    gst_element_link_many(queue, mconv, m_effectElement, (const char*)NULL);
    GstPad *sinkpad = gst_element_get_pad (queue, "sink");
    gst_element_add_pad (audioBin, gst_ghost_pad_new ("sink", sinkpad));
    gst_object_unref (sinkpad);
    return audioBin;
}

}
} //namespace Phonon::Gstreamer

QT_END_NAMESPACE
#include "moc_audioeffect.cpp"
