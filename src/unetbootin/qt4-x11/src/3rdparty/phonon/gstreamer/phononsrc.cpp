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

#include <gst/gst.h>
#include <gst/base/gstbasesrc.h>
#include "phononsrc.h"
#include "streamreader.h"

QT_BEGIN_NAMESPACE

namespace Phonon
{
namespace Gstreamer
{

static GstStaticPadTemplate srctemplate =
       GST_STATIC_PAD_TEMPLATE ("src",
                                GST_PAD_SRC,
                                GST_PAD_ALWAYS,
                                GST_STATIC_CAPS_ANY);

GST_DEBUG_CATEGORY_STATIC (phonon_src_debug);

// PhononSrc args
enum
{
    ARG_0,
    ARG_PHONONSRC
};

static void phonon_src_finalize (GObject * object);

static void phonon_src_set_property (GObject * object, guint prop_id,
                                        const GValue * value, GParamSpec * pspec);
static void phonon_src_get_property (GObject * object, guint prop_id,
                                        GValue * value, GParamSpec * pspec);

static gboolean phonon_src_start (GstBaseSrc * basesrc);
static gboolean phonon_src_stop (GstBaseSrc * basesrc);

static gboolean phonon_src_is_seekable (GstBaseSrc * src);
static gboolean phonon_src_get_size (GstBaseSrc * src, guint64 * size);
static GstFlowReturn phonon_src_create (GstBaseSrc * src, guint64 offset,
                                           guint length, GstBuffer ** buffer);

static void _do_init (GType filesrc_type)
{
    Q_UNUSED(filesrc_type);
    GST_DEBUG_CATEGORY_INIT (phonon_src_debug, "phononsrc", 0, "QIODevice element");
}

GST_BOILERPLATE_FULL (PhononSrc, phonon_src, GstBaseSrc, GST_TYPE_BASE_SRC, _do_init)

// Register element details
static void phonon_src_base_init (gpointer g_class) {
    GstElementClass *gstelement_class = GST_ELEMENT_CLASS (g_class);
    static gchar longname[] = "Phonon Stream Source",
                    klass[] = "Source/File",
              description[] = "Read from a Phonon StreamInterface",
                   author[] = "Nokia Corporation and/or its subsidiary(-ies) <qt-info@nokia.com>";
    GstElementDetails details = GST_ELEMENT_DETAILS (longname,
                                          klass,
                                          description,
                                          author);
    gst_element_class_set_details (gstelement_class, &details);
    gst_element_class_add_pad_template (gstelement_class, gst_static_pad_template_get (&srctemplate));
}

static void phonon_src_class_init (PhononSrcClass * klass)
{
    GObjectClass *gobject_class;
    GstElementClass *gstelement_class;
    GstBaseSrcClass *gstbasesrc_class;

    gobject_class = G_OBJECT_CLASS (klass);
    gstelement_class = GST_ELEMENT_CLASS (klass);
    gstbasesrc_class = GST_BASE_SRC_CLASS (klass);

    gobject_class->set_property = phonon_src_set_property;
    gobject_class->get_property = phonon_src_get_property;

    g_object_class_install_property (gobject_class, ARG_PHONONSRC,
                                     g_param_spec_pointer ("iodevice", "A Phonon StreamReader",
                                     "A Phonon::GStreamer::StreamReader to read from", GParamFlags(G_PARAM_READWRITE)));

    gobject_class->finalize = GST_DEBUG_FUNCPTR (phonon_src_finalize);

    gstbasesrc_class->start = GST_DEBUG_FUNCPTR (phonon_src_start);
    gstbasesrc_class->stop = GST_DEBUG_FUNCPTR (phonon_src_stop);
    gstbasesrc_class->is_seekable = GST_DEBUG_FUNCPTR (phonon_src_is_seekable);
    gstbasesrc_class->get_size = GST_DEBUG_FUNCPTR (phonon_src_get_size);
    gstbasesrc_class->create = GST_DEBUG_FUNCPTR (phonon_src_create);
}

static void phonon_src_init (PhononSrc * src, PhononSrcClass * g_class)
{
    Q_UNUSED(g_class);
    src->device = 0;
}

static void phonon_src_finalize (GObject * object)
{
    PhononSrc *src;
    src = GST_PHONON_SRC (object);
    delete src->device;
    src->device = 0;
    G_OBJECT_CLASS (parent_class)->finalize (object);
}

static gboolean phonon_src_set_device(PhononSrc * src, StreamReader* device)
{
    GstState state;
    // The element must be stopped in order to do this
    GST_OBJECT_LOCK (src);
    state = GST_STATE (src);

    if (state != GST_STATE_READY && state != GST_STATE_NULL)
        goto wrong_state;

    GST_OBJECT_UNLOCK (src);

    src->device = device;
    g_object_notify (G_OBJECT (src), "iodevice");
    return TRUE;

    // Error
wrong_state:
    {
        //GST_DEBUG_OBJECT (src, "setting location in wrong state");
        GST_OBJECT_UNLOCK (src);
        return FALSE;
    }
}

static void phonon_src_set_property (GObject * object, guint prop_id, const GValue * value, GParamSpec * pspec)
{
    PhononSrc *src;
    g_return_if_fail (GST_IS_PHONON_SRC (object));
    src = GST_PHONON_SRC (object);

    switch (prop_id) {
    case ARG_PHONONSRC:
    {
        StreamReader *dev = (StreamReader*)(g_value_get_pointer(value));
        if (dev)
            phonon_src_set_device(src, dev);
        break;
    }
   default:
       G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
       break;
   }
}

static void phonon_src_get_property (GObject * object, guint prop_id, GValue * value,
                                        GParamSpec * pspec)
{
    PhononSrc *src;
    g_return_if_fail (GST_IS_PHONON_SRC (object));
    src = GST_PHONON_SRC (object);

    switch (prop_id) {
    case ARG_PHONONSRC:
        g_value_set_pointer(value, src->device);
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
        break;
    }
}

static GstFlowReturn phonon_src_create_read (PhononSrc * src, guint64 offset, guint length, GstBuffer ** buffer)
{
    Q_ASSERT(src->device);
    if (!src->device)
        return GST_FLOW_ERROR;

    GstBuffer *buf = gst_buffer_new_and_alloc (length);
    GST_BUFFER_SIZE (buf) = length;
    GST_BUFFER_OFFSET (buf) = offset;
    GST_BUFFER_OFFSET_END (buf) = offset + length;

    bool success = src->device->read(offset, length, (char*)GST_BUFFER_DATA (buf));
    //GST_LOG_OBJECT (src, "Reading %d bytes", length);

    if (success) {
        *buffer = buf;
        return GST_FLOW_OK;
    }

    gst_mini_object_unref(GST_MINI_OBJECT(buf));
    return GST_FLOW_ERROR;
}

static GstFlowReturn phonon_src_create (GstBaseSrc * basesrc, guint64 offset, guint length, GstBuffer ** buffer)
{
    PhononSrc *src;
    GstFlowReturn ret;
    src = GST_PHONON_SRC (basesrc);
    ret = phonon_src_create_read (src, offset, length, buffer);
    return ret;
}

static gboolean phonon_src_is_seekable (GstBaseSrc * basesrc)
{
    PhononSrc *src = GST_PHONON_SRC (basesrc);
    if (src->device)
        return src->device->streamSeekable();
    return false;
}

static gboolean phonon_src_get_size (GstBaseSrc * basesrc, guint64 * size)
{
    PhononSrc *src;
    src = GST_PHONON_SRC (basesrc);
    if (src->device && src->device->streamSeekable()) {
        *size = src->device->streamSize();
        return TRUE;
    }
    *size = 0;
    return FALSE;
}

// Necessary to go to READY state
static gboolean phonon_src_start (GstBaseSrc * basesrc)
{
    Q_UNUSED(basesrc);
    // Opening the device is handled by the frontend
    // We can only assume it is already open
    return TRUE;
}

static gboolean phonon_src_stop (GstBaseSrc * basesrc)
{
    Q_UNUSED(basesrc);
    // Closing the device is handled by the frontend
    return TRUE;
}

}
} //namespace Phonon::Gstreamer

QT_END_NAMESPACE
