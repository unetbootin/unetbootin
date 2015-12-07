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

/*****************************************
 *
 *  This is an aRts plugin for GStreamer
 *
 ****************************************/

#include <gst/gst.h>
#include <gst/audio/audio.h>
#include <gst/audio/gstaudiosink.h>
#include "artssink.h"

QT_BEGIN_NAMESPACE

namespace Phonon
{
namespace Gstreamer
{

static GstStaticPadTemplate sinktemplate =
GST_STATIC_PAD_TEMPLATE ("sink",
    GST_PAD_SINK,
    GST_PAD_ALWAYS,
    GST_STATIC_CAPS (
                     "audio/x-raw-int, "
                     "width = (int) { 8, 16 }, "
                     "depth = (int) { 8, 16 }, "
                     "endianness = (int) BYTE_ORDER, "
                     "channels = (int) { 1, 2 }, "
                     "rate = (int) [ 8000, 96000 ]"
                    )
);

typedef int (*Ptr_arts_init)();
typedef arts_stream_t (*Ptr_arts_play_stream)(int, int, int, const char*);
typedef int (*Ptr_arts_close_stream)(arts_stream_t);
typedef int (*Ptr_arts_stream_get)(arts_stream_t, arts_parameter_t_enum);
typedef int (*Ptr_arts_stream_set)(arts_stream_t, arts_parameter_t_enum, int value);
typedef int (*Ptr_arts_write)(arts_stream_t, const void *, int);
typedef int (*Ptr_arts_suspended)();
typedef void (*Ptr_arts_free)();

static Ptr_arts_init p_arts_init = 0;
static Ptr_arts_play_stream p_arts_play_stream = 0;
static Ptr_arts_close_stream p_arts_close_stream = 0;
static Ptr_arts_stream_get p_arts_stream_get= 0;
static Ptr_arts_stream_set p_arts_stream_set= 0;
static Ptr_arts_write p_arts_write = 0;
static Ptr_arts_suspended p_arts_suspended = 0;
static Ptr_arts_free p_arts_free = 0;

static void arts_sink_dispose (GObject * object);
static void arts_sink_reset (GstAudioSink * asink);
static void arts_sink_finalize (GObject * object);
static GstCaps *arts_sink_get_caps (GstBaseSink * bsink);
static gboolean arts_sink_open (GstAudioSink * asink);
static gboolean arts_sink_close (GstAudioSink * asink);
static gboolean arts_sink_prepare (GstAudioSink * asink, GstRingBufferSpec * spec);
static gboolean arts_sink_unprepare (GstAudioSink * asink);
static guint arts_sink_write (GstAudioSink * asink, gpointer data, guint length);
static guint arts_sink_delay (GstAudioSink * asink);

static gboolean connected = false;
static gboolean init = false;
static int sinkCount;

GST_BOILERPLATE (ArtsSink, arts_sink, GstAudioSink, GST_TYPE_AUDIO_SINK)

// ArtsSink args
enum
{
    ARG_0,
    ARG_ARTSSINK
};

/* open the device with given specs */
gboolean arts_sink_open(GstAudioSink *sink)
{
    Q_UNUSED(sink);

    // We already have an open connection to this device
    if (!init) {
        GST_ELEMENT_ERROR (sink, RESOURCE, OPEN_WRITE, (NULL), ("Could not connect to aRts", NULL));
        return false;
    } else if (connected) {
        GST_ELEMENT_ERROR (sink, RESOURCE, BUSY, (NULL), ("Device is busy", NULL));
        return false;
    }

    // Check if all symbols were resolved
    if (!(p_arts_init && p_arts_play_stream && p_arts_close_stream
         && p_arts_stream_get && p_arts_stream_set && p_arts_write && p_arts_free))
        return FALSE;

    // Check if arts_init succeeded
    if (!init)
        return false;

    return true;
}

/* prepare resources and state to operate with the given specs */
static gboolean arts_sink_prepare(GstAudioSink *sink, GstRingBufferSpec *spec)
{
    ArtsSink *asink = (ArtsSink*)sink;

    if (!init)
        return false;

    asink->samplerate = spec->rate;
    asink->samplebits = spec->depth;
    asink->channels = spec->channels;
    asink->bytes_per_sample = spec->bytes_per_sample;

    static int id = 0;
    asink->stream = p_arts_play_stream(spec->rate, spec->depth, spec->channels,
                                        QString("gstreamer-%0").arg(id++).toLatin1().constData());
    if (asink->stream)
        connected = true;

    return connected;
}

/* undo anything that was done in prepare() */
static gboolean arts_sink_unprepare(GstAudioSink *sink)
{
    Q_UNUSED(sink);
    ArtsSink *asink = (ArtsSink*)sink;
    if (init && connected) {
        p_arts_close_stream(asink->stream);
        connected = false;
    }
    return true;
}

/* close the device */
static gboolean arts_sink_close(GstAudioSink *sink)
{
    Q_UNUSED(sink);
    return true;
}

/* write samples to the device */
static guint arts_sink_write(GstAudioSink *sink, gpointer data, guint length)
{
    ArtsSink *asink = (ArtsSink*)sink;

    if (!init)
        return 0;

    int errorcode = p_arts_write(asink->stream, (char*)data, length);

    if (errorcode < 0)
        GST_ELEMENT_ERROR (sink, RESOURCE, WRITE, (NULL), ("Could not write to device.", NULL));

    return errorcode > 0 ? errorcode : 0;
}

/* get number of samples queued in the device */
static guint arts_sink_delay(GstAudioSink *sink)
{
    ArtsSink *asink = (ArtsSink*)sink;
    if (!init)
        return 0;

    // We get results in millisecons so we have to caculate the approximate size in samples
    guint delay = p_arts_stream_get(asink->stream, ARTS_P_SERVER_LATENCY) * (asink->samplerate / 1000);
    return delay;
}

/* reset the audio device, unblock from a write */
static void arts_sink_reset(GstAudioSink *sink)
{
    // ### We are currently unable to gracefully recover
    // after artsd has been restarted or killed.
    Q_UNUSED(sink);
}

// Register element details
static void arts_sink_base_init (gpointer g_class) {
    GstElementClass *gstelement_class = GST_ELEMENT_CLASS (g_class);
    static gchar longname[] = "Experimental aRts sink",
                    klass[] = "Sink/Audio",
              description[] = "aRts Audio Output Device",
                   author[] = "Nokia Corporation and/or its subsidiary(-ies) <qt-info@nokia.com>";
    GstElementDetails details = GST_ELEMENT_DETAILS (longname,
                                          klass,
                                          description,
                                          author);
    gst_element_class_add_pad_template (gstelement_class, gst_static_pad_template_get (&sinktemplate));
    gst_element_class_set_details (gstelement_class, &details);
}

static void arts_sink_class_init (ArtsSinkClass * klass)
{
    parent_class = (GstAudioSinkClass*)g_type_class_peek_parent(klass);

    GObjectClass *gobject_class = G_OBJECT_CLASS (klass);
    gobject_class->finalize = GST_DEBUG_FUNCPTR (arts_sink_finalize);
    gobject_class->dispose = GST_DEBUG_FUNCPTR (arts_sink_dispose);

    GstBaseSinkClass *gstbasesink_class = (GstBaseSinkClass *) klass;
    gstbasesink_class->get_caps = GST_DEBUG_FUNCPTR (arts_sink_get_caps);

    GstAudioSinkClass *gstaudiosink_class = (GstAudioSinkClass*)klass;
    gstaudiosink_class->open =      GST_DEBUG_FUNCPTR(arts_sink_open);
    gstaudiosink_class->prepare =   GST_DEBUG_FUNCPTR(arts_sink_prepare);
    gstaudiosink_class->unprepare = GST_DEBUG_FUNCPTR(arts_sink_unprepare);
    gstaudiosink_class->close =     GST_DEBUG_FUNCPTR(arts_sink_close);
    gstaudiosink_class->write =     GST_DEBUG_FUNCPTR(arts_sink_write);
    gstaudiosink_class->delay =     GST_DEBUG_FUNCPTR(arts_sink_delay);
    gstaudiosink_class->reset =     GST_DEBUG_FUNCPTR(arts_sink_reset);
}

static void arts_sink_init (ArtsSink * src, ArtsSinkClass * g_class)
{
    Q_UNUSED(g_class);
    GST_DEBUG_OBJECT (src, "initializing artssink");
    src->stream = 0;

    p_arts_init =  (Ptr_arts_init)QLibrary::resolve(QLatin1String("artsc"), 0, "arts_init");
    p_arts_play_stream =  (Ptr_arts_play_stream)QLibrary::resolve(QLatin1String("artsc"), 0, "arts_play_stream");
    p_arts_close_stream =  (Ptr_arts_close_stream)QLibrary::resolve(QLatin1String("artsc"), 0, "arts_close_stream");
    p_arts_stream_get =  (Ptr_arts_stream_get)QLibrary::resolve(QLatin1String("artsc"), 0, "arts_stream_get");
    p_arts_stream_set =  (Ptr_arts_stream_set)QLibrary::resolve(QLatin1String("artsc"), 0, "arts_stream_set");
    p_arts_write =  (Ptr_arts_write)QLibrary::resolve(QLatin1String("artsc"), 0, "arts_write");
    p_arts_suspended =  (Ptr_arts_suspended)QLibrary::resolve(QLatin1String("artsc"), 0, "arts_suspended");
    p_arts_free =  (Ptr_arts_free)QLibrary::resolve(QLatin1String("artsc"), 0, "arts_free");

    if (!sinkCount) {
        int errorcode = p_arts_init();
        if (!errorcode) {
            init = TRUE;
        }
    }
    sinkCount ++;
}

static void arts_sink_dispose (GObject * object)
{
    Q_UNUSED(object);
    if (--sinkCount == 0) {
        p_arts_free();
    }
}

static void arts_sink_finalize (GObject * object)
{
    G_OBJECT_CLASS (parent_class)->finalize (object);
}

static GstCaps *arts_sink_get_caps (GstBaseSink * bsink)
{
    Q_UNUSED(bsink);
    return NULL;
}

}
} //namespace Phonon::Gstreamer

QT_END_NAMESPACE
