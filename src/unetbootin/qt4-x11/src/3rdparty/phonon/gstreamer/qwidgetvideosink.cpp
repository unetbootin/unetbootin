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

#include <QApplication>
#include "videowidget.h"
#include "qwidgetvideosink.h"

QT_BEGIN_NAMESPACE

namespace Phonon
{
namespace Gstreamer
{

static GstVideoSinkClass*   parentClass;

/*!
    \class gstreamer::QWidgetVideoSink
    \internal
*/

template <VideoFormat FMT>
GstCaps* QWidgetVideoSink<FMT>::get_caps(GstBaseSink* sink)
{
    Q_UNUSED(sink);
    return 0;
}

template <>
const char* QWidgetVideoSinkClass<VideoFormat_YUV>::get_name()
{
    return "QWidgetVideoSinkYUV";
}

template <>
const char* QWidgetVideoSinkClass<VideoFormat_RGB>::get_name()
{
    return "QWidgetVideoSinkRGB";
}

template <VideoFormat FMT>
gboolean QWidgetVideoSink<FMT>::set_caps(GstBaseSink* sink, GstCaps* caps)
{
    GstStructure*       data;
    QWidgetVideoSink<FMT> *self = G_TYPE_CHECK_INSTANCE_CAST(sink, QWidgetVideoSinkClass<FMT>::get_type(), QWidgetVideoSink<FMT>);

    data = gst_caps_get_structure(caps, 0);

    gst_structure_get_int(data, "width", &self->width);
    gst_structure_get_int(data, "height", &self->height);
    gst_structure_get_int(data, "bpp", &self->bpp);
    gst_structure_get_int(data, "depth", &self->depth);
    return TRUE;
}

template <VideoFormat FMT>
GstStateChangeReturn QWidgetVideoSink<FMT>::change_state(GstElement* element, GstStateChange transition)
{
    return GST_ELEMENT_CLASS(parentClass)->change_state(element, transition);
}

template <VideoFormat FMT>
GstFlowReturn QWidgetVideoSink<FMT>::render(GstBaseSink* sink, GstBuffer* buf)
{
    GstFlowReturn rc = GST_FLOW_OK;

    if (buf != 0)
    {
        QWidgetVideoSink<FMT> *self = G_TYPE_CHECK_INSTANCE_CAST(sink, QWidgetVideoSinkClass<FMT>::get_type(), QWidgetVideoSink<FMT>);
        QByteArray frame;
        frame.resize(buf->size);
        memcpy(frame.data(), buf->data, buf->size);
        NewFrameEvent *frameEvent = new NewFrameEvent(frame, self->width, self->height);
        QApplication::postEvent(self->renderWidget, frameEvent);
    }
    else
        rc = GST_FLOW_ERROR;
    return rc;
}

static GstStaticPadTemplate template_factory_yuv =
    GST_STATIC_PAD_TEMPLATE("sink",
                            GST_PAD_SINK,
                            GST_PAD_ALWAYS,
                            GST_STATIC_CAPS("video/x-raw-yuv, "
                                            "framerate = (fraction) [ 0, MAX ], "
                                            "width = (int) [ 1, MAX ], "
                                            "height = (int) [ 1, MAX ],"
                                            "bpp = (int) 32"));

static GstStaticPadTemplate template_factory_rgb =
    GST_STATIC_PAD_TEMPLATE("sink",
                            GST_PAD_SINK,
                            GST_PAD_ALWAYS,
                            GST_STATIC_CAPS("video/x-raw-rgb, "
                                            "framerate = (fraction) [ 0, MAX ], "
                                            "width = (int) [ 1, MAX ], "
                                            "height = (int) [ 1, MAX ],"
                                            "bpp = (int) 32"));

template <VideoFormat FMT>
struct template_factory;


template <>
struct template_factory<VideoFormat_YUV>
{
    static GstStaticPadTemplate *getFactory()
    {
        return &template_factory_yuv;
    }
};

template <>
struct template_factory<VideoFormat_RGB>
{
    static GstStaticPadTemplate *getFactory()
    {
        return &template_factory_rgb;
    }
};

template <VideoFormat FMT>
void QWidgetVideoSink<FMT>::base_init(gpointer g_class)
{
    gst_element_class_add_pad_template(GST_ELEMENT_CLASS(g_class),
                                       gst_static_pad_template_get(template_factory<FMT>::getFactory()));
}

template <VideoFormat FMT>
void QWidgetVideoSink<FMT>::instance_init(GTypeInstance *instance, gpointer g_class)
{
    Q_UNUSED(g_class);

    QWidgetVideoSink<FMT>* self = reinterpret_cast<QWidgetVideoSink<FMT>*>(instance);

    self->renderWidget = 0;
    self->width = 0;
    self->height = 0;
    self->bpp = 0;
    self->depth = 0;
}

// QWidgetVideoSinkClass
template <VideoFormat FMT>
void QWidgetVideoSinkClass<FMT>::class_init(gpointer g_class, gpointer class_data)
{
    Q_UNUSED(class_data);
    GstBaseSinkClass*   gstBaseSinkClass = (GstBaseSinkClass*)g_class;
    GstElementClass*    gstElementClass = (GstElementClass*)g_class;

    parentClass = reinterpret_cast<GstVideoSinkClass*>(g_type_class_peek_parent(g_class));

    // base
    gstBaseSinkClass->set_caps = QWidgetVideoSink<FMT>::set_caps;
    gstBaseSinkClass->preroll = QWidgetVideoSink<FMT>::render;
    gstBaseSinkClass->render = QWidgetVideoSink<FMT>::render;

    // element
    gstElementClass->change_state = QWidgetVideoSink<FMT>::change_state;
}

template <VideoFormat FMT>
GType QWidgetVideoSinkClass<FMT>::get_type()
{
    static GType type = 0;

    if (type == 0)
    {
        static const GTypeInfo info =
        {
            sizeof(QWidgetVideoSinkClass<FMT>),                 // class_size
            QWidgetVideoSink<FMT>::base_init,                   // base init
            NULL,                                               // base_finalize

            QWidgetVideoSinkClass<FMT>::class_init,             // class_init
            NULL,                                               // class_finalize
            NULL,                                               // class_data

            sizeof(QWidgetVideoSink<FMT>),                      // instance_size
            0,                                                  // n_preallocs
            QWidgetVideoSink<FMT>::instance_init,               // instance_init
            0                                                   // value_table
        };

        type = g_type_register_static(GST_TYPE_VIDEO_SINK,
                                     QWidgetVideoSinkClass<FMT>::get_name(),
                                      &info,
                                      GTypeFlags(0));
    }
    return type;
}

GType get_type_YUV()
{
    return QWidgetVideoSinkClass<VideoFormat_YUV>::get_type();
}

GType get_type_RGB()
{
    return QWidgetVideoSinkClass<VideoFormat_RGB>::get_type();
}

}
} //namespace Phonon::Gstreamer

QT_END_NAMESPACE
