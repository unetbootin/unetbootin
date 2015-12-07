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

#include <gst/interfaces/propertyprobe.h>
#include <gst/gst.h>
#include "common.h"
#include "gsthelper.h"

#include <QtCore/QList>

QT_BEGIN_NAMESPACE

namespace Phonon
{
namespace Gstreamer
{

/**
 * Probes a gstElement for a list of settable string-property values
 *
 * @return a QStringList containing a list of allwed string values for the given
 *           element
 */
QList<QByteArray> GstHelper::extractProperties(GstElement *elem, const QByteArray &value)
{
    Q_ASSERT(elem);
    QList<QByteArray> list;

    if (GST_IS_PROPERTY_PROBE(elem)) {
        GstPropertyProbe *probe = GST_PROPERTY_PROBE(elem);
        const GParamSpec *devspec = 0;
        GValueArray *array = NULL;

        if ((devspec = gst_property_probe_get_property (probe, value))) {
            if ((array = gst_property_probe_probe_and_get_values (probe, devspec))) {
                for (unsigned int device = 0; device < array->n_values; device++) {
                    GValue *deviceId = g_value_array_get_nth (array, device);
                    list.append(g_value_get_string(deviceId));
                }
            }
            if (array)
                g_value_array_free (array);
        }
    }
    return list;
}

/**
 * Sets the string value of a GstElement's property
 *
 * @return false if the value could not be set.
 */
bool GstHelper::setProperty(GstElement *elem, const char *propertyName, const QByteArray &propertyValue)
{
    Q_ASSERT(elem);
    Q_ASSERT(propertyName && strlen(propertyName));

    if (GST_IS_PROPERTY_PROBE(elem) && gst_property_probe_get_property( GST_PROPERTY_PROBE( elem), propertyName ) ) {
        g_object_set(G_OBJECT(elem), propertyName, propertyValue.constData(), (const char*)NULL);
        return true;
    }
    return false;
}

/**
 * Queries an element for the value of an object property
 */
QByteArray GstHelper::property(GstElement *elem, const char *propertyName)
{
    Q_ASSERT(elem);
    Q_ASSERT(propertyName && strlen(propertyName));
    QByteArray retVal;

    if (GST_IS_PROPERTY_PROBE(elem) && gst_property_probe_get_property( GST_PROPERTY_PROBE(elem), propertyName)) {
        gchar *value = NULL;
        g_object_get (G_OBJECT(elem), propertyName, &value, (const char*)NULL);
        retVal = QByteArray(value);
        g_free (value);
    }
    return retVal;
}

/**
 * Queries a GstObject for it's name
 */
QByteArray GstHelper::name(GstObject *obj)
{
    Q_ASSERT(obj);
    QByteArray retVal;
    gchar *value = NULL;
    if ((value = gst_object_get_name (obj))) {
        retVal = QByteArray(value);
        g_free (value);
    }
    return retVal;
}


/***
 * Creates an instance of a playbin with "audio-src" and
 * "video-src" ghost pads to allow redirected output streams.
 *
 * ### This function is probably not required now that MediaObject is based
 *     on decodebin directly.
 */
GstElement* GstHelper::createPluggablePlaybin()
{
    GstElement *playbin = 0;
    //init playbin and add to our pipeline
    playbin = gst_element_factory_make("playbin", NULL);

    //Create an identity element to redirect sound
    GstElement *audioSinkBin =  gst_bin_new (NULL);
    GstElement *audioPipe = gst_element_factory_make("identity", NULL);
    gst_bin_add(GST_BIN(audioSinkBin), audioPipe);

    //Create a sinkpad on the identity
    GstPad *audiopad = gst_element_get_pad (audioPipe, "sink");
    gst_element_add_pad (audioSinkBin, gst_ghost_pad_new ("sink", audiopad));
    gst_object_unref (audiopad);

    //Create an "audio_src" source pad on the playbin
    GstPad *audioPlaypad = gst_element_get_pad (audioPipe, "src");
    gst_element_add_pad (playbin, gst_ghost_pad_new ("audio_src", audioPlaypad));
    gst_object_unref (audioPlaypad);

    //Done with our audio redirection
    g_object_set (G_OBJECT(playbin), "audio-sink", audioSinkBin, (const char*)NULL);

    // * * Redirect video to "video_src" pad : * *

    //Create an identity element to redirect sound
    GstElement *videoSinkBin =  gst_bin_new (NULL);
    GstElement *videoPipe = gst_element_factory_make("identity", NULL);
    gst_bin_add(GST_BIN(videoSinkBin), videoPipe);

    //Create a sinkpad on the identity
    GstPad *videopad = gst_element_get_pad (videoPipe, "sink");
    gst_element_add_pad (videoSinkBin, gst_ghost_pad_new ("sink", videopad));
    gst_object_unref (videopad);

    //Create an "audio_src" source pad on the playbin
    GstPad *videoPlaypad = gst_element_get_pad (videoPipe, "src");
    gst_element_add_pad (playbin, gst_ghost_pad_new ("video_src", videoPlaypad));
    gst_object_unref (videoPlaypad);

    //Done with our video redirection
    g_object_set (G_OBJECT(playbin), "video-sink", videoSinkBin, (const char*)NULL);
    return playbin;
}


} //namespace Gstreamer
} //namespace Phonon

QT_END_NAMESPACE
