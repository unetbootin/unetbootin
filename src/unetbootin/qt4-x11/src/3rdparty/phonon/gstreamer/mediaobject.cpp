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

#include <cmath>
#include <gst/interfaces/propertyprobe.h>
#include "common.h"
#include "mediaobject.h"
#include "videowidget.h"
#include "message.h"
#include "backend.h"
#include "streamreader.h"
#include "phononsrc.h"

#include <QtCore>
#include <QtCore/QTimer>
#include <QtCore/QVector>
#include <QtCore/QFile>
#include <QtCore/QByteRef>
#include <QtCore/QStringList>
#include <QtCore/QEvent>
#include <QApplication>

#define ABOUT_TO_FINNISH_TIME 2000
#define MAX_QUEUE_TIME 20 * GST_SECOND

QT_BEGIN_NAMESPACE

namespace Phonon
{
namespace Gstreamer
{

MediaObject::MediaObject(Backend *backend, QObject *parent)
        : QObject(parent)
        , MediaNode(backend, AudioSource | VideoSource)
        , m_resumeState(false)
        , m_oldState(Phonon::LoadingState)
        , m_oldPos(0)
        , m_state(Phonon::LoadingState)
        , m_pendingState(Phonon::LoadingState)
        , m_tickTimer(new QTimer(this))
        , m_prefinishMark(0)
        , m_transitionTime(0)
        , m_prefinishMarkReachedNotEmitted(true)
        , m_aboutToFinishEmitted(false)
        , m_loading(false)
        , m_capsHandler(0)
        , m_datasource(0)
        , m_decodebin(0)
        , m_audioPipe(0)
        , m_videoPipe(0)
        , m_totalTime(-1)
        , m_bufferPercent(0)
        , m_hasVideo(false)
        , m_videoStreamFound(false)
        , m_hasAudio(false)
        , m_seekable(false)
        , m_atEndOfStream(false)
        , m_atStartOfStream(false)
        , m_error(Phonon::NoError)
        , m_pipeline(0)
        , m_audioGraph(0)
        , m_videoGraph(0)
        , m_previousTickTime(-1)
        , m_resetNeeded(false)
{
    qRegisterMetaType<GstCaps*>("GstCaps*");
    qRegisterMetaType<State>("State");

    static int count = 0;
    m_name = "MediaObject" + QString::number(count++);

    if (!m_backend->isValid()) {
        setError(tr("Cannot start playback. \n\nCheck your Gstreamer installation and make sure you "
                    "\nhave libgstreamer-plugins-base installed."), Phonon::FatalError);
    } else {
        m_root = this;
        createPipeline();
        m_backend->addBusWatcher(this);
        connect(m_tickTimer, SIGNAL(timeout()), SLOT(emitTick()));
    }
    connect(this, SIGNAL(stateChanged(Phonon::State, Phonon::State)), 
            this, SLOT(notifyStateChange(Phonon::State, Phonon::State)));

}

MediaObject::~MediaObject()
{
    m_backend->removeBusWatcher(this);
    if (m_pipeline) {
        gst_element_set_state(m_pipeline, GST_STATE_NULL);
        gst_object_unref(m_pipeline);
    }
    if (m_audioGraph) {
        gst_element_set_state(m_audioGraph, GST_STATE_NULL);
        gst_object_unref(m_audioGraph);
    }
    if (m_videoGraph) {
        gst_element_set_state(m_videoGraph, GST_STATE_NULL);
        gst_object_unref(m_videoGraph);
    }
}

QString stateString(const Phonon::State &state)
{
    switch (state) {
    case Phonon::LoadingState:
        return QString("LoadingState");
    case Phonon::StoppedState:
        return QString("StoppedState");
    case Phonon::PlayingState:
        return QString("PlayingState");
    case Phonon::BufferingState:
        return QString("BufferingState");
    case Phonon::PausedState:
        return QString("PausedState");
    case Phonon::ErrorState:
        return QString("ErrorState");
    }
    return QString();
}

void MediaObject::saveState()
{
    //Only first resumeState is respected
    if (m_resumeState)
        return;

    if (m_state == Phonon::PlayingState || m_state == Phonon::PausedState) {
        m_resumeState = true;
        m_oldState = m_state;
        m_oldPos = getPipelinePos();
    }
}

void MediaObject::resumeState()
{
    if (m_resumeState)
        QMetaObject::invokeMethod(this, "setState", Qt::QueuedConnection, Q_ARG(State, m_oldState));
}

void MediaObject::newPadAvailable (GstPad *pad)
{
    GstCaps *caps;
    GstStructure *str;
    caps = gst_pad_get_caps (pad);
    if (caps) {
        str = gst_caps_get_structure (caps, 0);
        QString mediaString(gst_structure_get_name (str));

        if (mediaString.startsWith("video")) {
            connectVideo(pad);
        } else if (mediaString.startsWith("audio")) {
            connectAudio(pad);
        } else {
            m_backend->logMessage("Could not connect pad", Backend::Warning);
        }
        gst_caps_unref (caps);
    }
}

void MediaObject::cb_newpad (GstElement *decodebin,
                             GstPad     *pad,
                             gboolean    last,
                             gpointer    data)
{
    Q_UNUSED(decodebin);
    Q_UNUSED(pad);
    Q_UNUSED(last);
    Q_UNUSED(data);

    MediaObject *media = static_cast<MediaObject*>(data);
    Q_ASSERT(media);
    media->newPadAvailable(pad);
}

void MediaObject::noMorePadsAvailable ()
{
    if (m_missingCodecs.size() > 0) {
        bool canPlay = (m_hasAudio || m_videoStreamFound);
        Phonon::ErrorType error = canPlay ? Phonon::NormalError : Phonon::FatalError;
        if (error == Phonon::NormalError && m_hasVideo && !m_videoStreamFound) {
            m_hasVideo = false;
            emit hasVideoChanged(false);
        }
        QString codecs = m_missingCodecs.join(", ");
        setError(QString(tr("A required codec is missing. You need to install the following codec(s) to play this content: %0")).arg(codecs), error);
        m_missingCodecs.clear();
    }
}

void MediaObject::cb_no_more_pads (GstElement * decodebin, gpointer data)
{
    Q_UNUSED(decodebin);
    MediaObject *media = static_cast<MediaObject*>(data);
    Q_ASSERT(media);
    QMetaObject::invokeMethod(media, "noMorePadsAvailable", Qt::QueuedConnection);
}

typedef void (*Ptr_gst_pb_utils_init)();
typedef gchar* (*Ptr_gst_pb_utils_get_codec_description)(const GstCaps *);

void MediaObject::cb_unknown_type (GstElement *decodebin, GstPad *pad, GstCaps *caps, gpointer data)
{
    Q_UNUSED(decodebin);
    Q_UNUSED(pad);
    MediaObject *media = static_cast<MediaObject*>(data);
    Q_ASSERT(media);

    QString value = "unknown codec";

    // These functions require GStreamer > 0.10.12
    static Ptr_gst_pb_utils_init p_gst_pb_utils_init = 0;
    static Ptr_gst_pb_utils_get_codec_description p_gst_pb_utils_get_codec_description = 0;
    if (!p_gst_pb_utils_init) {
        p_gst_pb_utils_init =  (Ptr_gst_pb_utils_init)QLibrary::resolve(QLatin1String("gstpbutils-0.10"), 0, "gst_pb_utils_init");
        p_gst_pb_utils_get_codec_description =  (Ptr_gst_pb_utils_get_codec_description)QLibrary::resolve(QLatin1String("gstpbutils-0.10"), 0, "gst_pb_utils_get_codec_description");
        if (p_gst_pb_utils_init)
	    p_gst_pb_utils_init();
    }
    if (p_gst_pb_utils_get_codec_description) {
        gchar *codecName = NULL;
        codecName = p_gst_pb_utils_get_codec_description (caps);
        value = QString::fromUtf8(codecName);
        g_free (codecName);
    } else {
        // For GStreamer versions < 0.10.12
        GstStructure *str = gst_caps_get_structure (caps, 0);
        value = QString::fromUtf8(gst_structure_get_name (str));
    }
    media->addMissingCodecName(value);
}

static void notifyVideoCaps(GObject *obj, GParamSpec *, gpointer data)
{
    GstPad *pad = GST_PAD(obj);
    GstCaps *caps = gst_pad_get_caps (pad);
    Q_ASSERT(caps);
    MediaObject *media = static_cast<MediaObject*>(data);

    // We do not want any more notifications until the source changes
    g_signal_handler_disconnect(pad, media->capsHandler());

    // setVideoCaps calls loadingComplete(), meaning we cannot call it from
    // the streaming thread
    QMetaObject::invokeMethod(media, "setVideoCaps", Qt::QueuedConnection, Q_ARG(GstCaps *, caps));
}

void MediaObject::setVideoCaps(GstCaps *caps)
{
    GstStructure *str;
    gint width, height;

    if ((str = gst_caps_get_structure (caps, 0))) {
        if (gst_structure_get_int (str, "width", &width) && gst_structure_get_int (str, "height", &height)) {
            // Let child nodes know about our new video size
            QSize size(width, height);
            MediaNodeEvent event(MediaNodeEvent::VideoSizeChanged, &size);
            notify(&event);
        }
    }
    gst_caps_unref(caps);
}

// Adds an element to the pipeline if not previously added
bool MediaObject::addToPipeline(GstElement *elem)
{
    bool success = true;
    if (!GST_ELEMENT_PARENT(elem)) { // If not already in pipeline
        success = gst_bin_add(GST_BIN(m_pipeline), elem);
    }
    return success;
}

void MediaObject::connectVideo(GstPad *pad)
{
    GstState currentState = GST_STATE(m_pipeline);
    if (addToPipeline(m_videoGraph)) {
        GstPad *videopad = gst_element_get_pad (m_videoGraph, "sink");
        if (!GST_PAD_IS_LINKED (videopad) && (gst_pad_link (pad, videopad) == GST_PAD_LINK_OK)) {
            gst_element_set_state(m_videoGraph, currentState == GST_STATE_PLAYING ? GST_STATE_PLAYING : GST_STATE_PAUSED);
            m_videoStreamFound = true;
            m_backend->logMessage("Video track connected", Backend::Info, this);
            // Note that the notify::caps _must_ be installed after linking to work with Dapper
            m_capsHandler = g_signal_connect(pad, "notify::caps", G_CALLBACK(notifyVideoCaps), this);
        }
        gst_object_unref (videopad);
    } else {
        m_backend->logMessage("The video stream could not be plugged.", Backend::Info, this);
    }
}

void MediaObject::connectAudio(GstPad *pad)
{
    GstState currentState = GST_STATE(m_pipeline);
    if (addToPipeline(m_audioGraph)) {
        GstPad *audiopad = gst_element_get_pad (m_audioGraph, "sink");
        if (!GST_PAD_IS_LINKED (audiopad) && (gst_pad_link (pad, audiopad)==GST_PAD_LINK_OK)) {
            gst_element_set_state(m_audioGraph, currentState == GST_STATE_PLAYING ? GST_STATE_PLAYING : GST_STATE_PAUSED);
            m_hasAudio = true;
            m_backend->logMessage("Audio track connected", Backend::Info, this);
        }
        gst_object_unref (audiopad);
    } else {
        m_backend->logMessage("The audio stream could not be plugged.", Backend::Info, this);
    }
}

/**
 * Create a media source from a given URL.
 *
 * returns true if successful
 */
bool MediaObject::createPipefromURL(const QString &encodedUrl)
{
    // Convert back to URL
    QUrl url(encodedUrl, QUrl::StrictMode);

    // Remove any existing data source
    if (m_datasource) {
        gst_bin_remove(GST_BIN(m_pipeline), m_datasource);
        // m_pipeline has the only ref to datasource
        m_datasource = 0;
    }

    // Verify that the uri can be parsed
    if (!url.isValid()) {
        m_backend->logMessage(QString("%1 is not a valid URI").arg(encodedUrl));
        return false;
    }

    // Create a new datasource based on the input URL
    QByteArray encoded_cstr_url = url.toEncoded();
    m_datasource = gst_element_make_from_uri(GST_URI_SRC, encoded_cstr_url.constData(), (const char*)NULL);
    if (!m_datasource)
        return false;

    // Link data source into pipeline
    gst_bin_add(GST_BIN(m_pipeline), m_datasource);
    if (!gst_element_link(m_datasource, m_decodebin)) {
        gst_bin_remove(GST_BIN(m_pipeline), m_datasource);
        return false;
    }
    return true;
}

/**
 * Create a media source from a media stream
 *
 * returns true if successful
 */
bool MediaObject::createPipefromStream(const MediaSource &source)
{
    // Remove any existing data source
    if (m_datasource) {
        gst_bin_remove(GST_BIN(m_pipeline), m_datasource);
        // m_pipeline has the only ref to datasource
        m_datasource = 0;
    }

    m_datasource = GST_ELEMENT(g_object_new(phonon_src_get_type(), NULL));
    if (!m_datasource)
        return false;

    StreamReader *streamReader = new StreamReader(source);
    g_object_set (G_OBJECT (m_datasource), "iodevice", streamReader, (const char*)NULL);

    // Link data source into pipeline
    gst_bin_add(GST_BIN(m_pipeline), m_datasource);
    if (!gst_element_link(m_datasource, m_decodebin)) {
        gst_bin_remove(GST_BIN(m_pipeline), m_datasource);
        return false;
    }
    return true;
}

void MediaObject::createPipeline()
{
    m_pipeline = gst_pipeline_new (NULL);
    gst_object_ref (GST_OBJECT (m_pipeline));
    gst_object_sink (GST_OBJECT (m_pipeline));

    m_decodebin = gst_element_factory_make ("decodebin", NULL);
    g_signal_connect (m_decodebin, "new-decoded-pad", G_CALLBACK (&cb_newpad), this);
    g_signal_connect (m_decodebin, "unknown-type", G_CALLBACK (&cb_unknown_type), this);
    g_signal_connect (m_decodebin, "no-more-pads", G_CALLBACK (&cb_no_more_pads), this);

    gst_bin_add(GST_BIN(m_pipeline), m_decodebin);

    // Create a bin to contain the gst elements for this medianode

    // Set up audio graph
    m_audioGraph = gst_bin_new(NULL);
    gst_object_ref (GST_OBJECT (m_audioGraph));
    gst_object_sink (GST_OBJECT (m_audioGraph));

    // Note that these queues are only required for streaming content
    // And should ideally be created on demand as they will disable
    // pull-mode access. Also note that the max-size-time are increased to
    // reduce buffer overruns as these are not gracefully handled at the moment.
    m_audioPipe = gst_element_factory_make("queue", NULL);
    g_object_set(G_OBJECT(m_audioPipe), "max-size-time",  MAX_QUEUE_TIME, (const char*)NULL);
    gst_bin_add(GST_BIN(m_audioGraph), m_audioPipe);
    GstPad *audiopad = gst_element_get_pad (m_audioPipe, "sink");
    gst_element_add_pad (m_audioGraph, gst_ghost_pad_new ("sink", audiopad));
    gst_object_unref (audiopad);

    // Set up video graph
    m_videoGraph = gst_bin_new(NULL);
    gst_object_ref (GST_OBJECT (m_videoGraph));
    gst_object_sink (GST_OBJECT (m_videoGraph));

    m_videoPipe = gst_element_factory_make("queue", NULL);
    g_object_set(G_OBJECT(m_videoPipe), "max-size-time", MAX_QUEUE_TIME, (const char*)NULL);
    gst_bin_add(GST_BIN(m_videoGraph), m_videoPipe);
    GstPad *videopad = gst_element_get_pad (m_videoPipe, "sink");
    gst_element_add_pad (m_videoGraph, gst_ghost_pad_new ("sink", videopad));
    gst_object_unref (videopad);

    if (m_pipeline && m_decodebin && m_audioGraph && m_videoGraph && m_audioPipe && m_videoPipe)
        m_isValid = true;
    else
        m_backend->logMessage("Could not create pipeline for media object", Backend::Warning);
}

/**
 * !reimp
 */
State MediaObject::state() const
{
    return m_state;
}

/**
 * !reimp
 */
bool MediaObject::hasVideo() const
{
    return m_hasVideo;
}

/**
 * !reimp
 */
bool MediaObject::isSeekable() const
{
    return m_seekable;
}

/**
 * !reimp
 */
qint64 MediaObject::currentTime() const
{
    if (m_resumeState)
        return m_oldPos;

    switch (state()) {
    case Phonon::PausedState:
    case Phonon::BufferingState:
    case Phonon::PlayingState:
        return getPipelinePos();
    case Phonon::StoppedState:
    case Phonon::LoadingState:
        return 0;
    case Phonon::ErrorState:
        break;
    }
    return -1;
}

/**
 * !reimp
 */
qint32 MediaObject::tickInterval() const
{
    return m_tickInterval;
}

/**
 * !reimp
 */
void MediaObject::setTickInterval(qint32 newTickInterval)
{
    m_tickInterval = newTickInterval;
    if (m_tickInterval <= 0)
        m_tickTimer->setInterval(50);
    else
        m_tickTimer->setInterval(newTickInterval);
}

/**
 * !reimp
 */
void MediaObject::play()
{
    setState(Phonon::PlayingState);
    m_resumeState = false;
}

/**
 * !reimp
 */
QString MediaObject::errorString() const
{
    return m_errorString;
}

/**
 * !reimp
 */
Phonon::ErrorType MediaObject::errorType() const
{
    return m_error;
}

/**
 * Set the current state of the mediaObject.
 *
 * !### Note that both Playing and Paused states are set immediately
 *     This should obviously be done in response to actual gstreamer state changes
 */
void MediaObject::setState(State newstate)
{
    if (!isValid())
        return;

    if (m_state == newstate)
        return;

    if (m_loading) {
        // We are still loading. The state will be requested
        // when loading has completed.
        m_pendingState = newstate;
        return;
    }

    GstState currentState;
    gst_element_get_state (m_pipeline, &currentState, NULL, 1000);

    switch (newstate) {
    case Phonon::BufferingState:
        m_backend->logMessage("phonon state request: buffering", Backend::Info, this);
        break;

    case Phonon::PausedState:
        m_backend->logMessage("phonon state request: paused", Backend::Info, this);
        if (currentState == GST_STATE_PAUSED) {
            changeState(Phonon::PausedState);
        } else if (gst_element_set_state(m_pipeline, GST_STATE_PAUSED) != GST_STATE_CHANGE_FAILURE) {
            m_pendingState = Phonon::PausedState;
        } else {
            m_backend->logMessage("phonon state request failed", Backend::Info, this);
        }
        break;

    case Phonon::StoppedState:
        m_backend->logMessage("phonon state request: Stopped", Backend::Info, this);
        if (currentState == GST_STATE_READY) {
            changeState(Phonon::StoppedState);
        } else if (gst_element_set_state(m_pipeline, GST_STATE_READY) != GST_STATE_CHANGE_FAILURE) {
            m_pendingState = Phonon::StoppedState;
        } else {
            m_backend->logMessage("phonon state request failed", Backend::Info, this);
        }
        m_atEndOfStream = false;
        break;

    case Phonon::PlayingState:
       if (m_resetNeeded) {
            // ### Note this is a workaround and it should really be gracefully
            // handled by medianode when we implement live connections.
            // This generally happens if medianodes have been connected after the MediaSource was set
            // Note that a side-effect of this is that we resend all meta data.
            gst_element_set_state(m_pipeline, GST_STATE_NULL);
            m_resetNeeded = false;
            // Send a source change so the X11 renderer
            // will re-set the overlay
            MediaNodeEvent event(MediaNodeEvent::SourceChanged);
            notify(&event);
        }
        m_backend->logMessage("phonon state request: Playing", Backend::Info, this);
        if (m_atEndOfStream) {
            m_backend->logMessage("EOS already reached", Backend::Info, this);
        } else if (currentState == GST_STATE_PLAYING) {
            changeState(Phonon::PlayingState);
        } else if (!m_atEndOfStream && gst_element_set_state(m_pipeline, GST_STATE_PLAYING) != GST_STATE_CHANGE_FAILURE) {
            m_pendingState = Phonon::PlayingState;
        } else {
            m_backend->logMessage("phonon state request failed", Backend::Info, this);
        }
        break;

    case Phonon::ErrorState:
        m_backend->logMessage("phonon state request : Error", Backend::Warning, this);
        m_backend->logMessage(QString("Last error : %0").arg(errorString()) , Backend::Warning, this);
        changeState(Phonon::ErrorState); //immediately set error state
        break;

    case Phonon::LoadingState:
        m_backend->logMessage("phonon state request: Loading", Backend::Info, this);
        changeState(Phonon::LoadingState);
        break;
    }
}

/*
 * Signals that the requested state has completed
 * by emitting stateChanged and updates the internal state.
 */
void MediaObject::changeState(State newstate)
{
    if (newstate == m_state)
        return;

    Phonon::State oldState = m_state;
    m_state = newstate; // m_state must be set before emitting, since 
                        // Error state requires that state() will return the new value
    m_pendingState = newstate;
    emit stateChanged(newstate, oldState);

    switch (newstate) {
    case Phonon::PausedState:
        m_backend->logMessage("phonon state changed: paused", Backend::Info, this);
        break;

    case Phonon::BufferingState:
        m_backend->logMessage("phonon state changed: buffering", Backend::Info, this);
        break;

    case Phonon::PlayingState:
        m_backend->logMessage("phonon state changed: Playing", Backend::Info, this);
        break;

    case Phonon::StoppedState:
        m_backend->logMessage("phonon state changed: Stopped", Backend::Info, this);
        m_tickTimer->stop();
        break;

    case Phonon::ErrorState:
        m_loading = false;
        m_backend->logMessage("phonon state changed : Error", Backend::Info, this);
        m_backend->logMessage(errorString(), Backend::Warning, this);
        break;

    case Phonon::LoadingState:
        m_backend->logMessage("phonon state changed: Loading", Backend::Info, this);
        break;
    }
}

void MediaObject::setError(const QString &errorString, Phonon::ErrorType error)
{
    m_errorString = errorString;
    m_error = error;
    m_tickTimer->stop();

    if (error == Phonon::FatalError) {
        m_hasVideo = false;
        emit hasVideoChanged(false);
        gst_element_set_state(m_pipeline, GST_STATE_READY);
        changeState(Phonon::ErrorState);
    } else {
        if (m_loading) //Flag error only after loading has completed
            m_pendingState = Phonon::ErrorState;
        else
            changeState(Phonon::ErrorState);
    }
}

qint64 MediaObject::totalTime() const
{
    return m_totalTime;
}

qint32 MediaObject::prefinishMark() const
{
    return m_prefinishMark;
}

qint32 MediaObject::transitionTime() const
{
    return m_transitionTime;
}

void MediaObject::setTransitionTime(qint32 time)
{
    m_transitionTime = time;
}

qint64 MediaObject::remainingTime() const
{
    return totalTime() - currentTime();
}

MediaSource MediaObject::source() const
{
    return m_source;
}

void MediaObject::setNextSource(const MediaSource &source)
{
    if (source.type() == MediaSource::Invalid &&
        source.type() == MediaSource::Empty)
        return;
    m_nextSource = source;
}

/**
 * Update total time value from the pipeline
 */
bool MediaObject::updateTotalTime()
{
    GstFormat   format = GST_FORMAT_TIME;
    gint64 duration = 0;
    if (gst_element_query_duration (GST_ELEMENT(m_pipeline), &format, &duration)) {
        setTotalTime(duration / GST_MSECOND);
        return true;
    }
    return false;
}

/**
 * Checks if the current source is seekable
 */
void MediaObject::updateSeekable()
{
    if (!isValid())
        return;

    GstQuery *query;
    gboolean result;
    gint64 start, stop;
    query = gst_query_new_seeking(GST_FORMAT_TIME);
    result = gst_element_query (m_pipeline, query);
    if (result) {
        gboolean seekable;
        GstFormat format;
        gst_query_parse_seeking (query, &format, &seekable, &start, &stop);

        if (m_seekable != seekable) {
            m_seekable = seekable;
            emit seekableChanged(m_seekable);
        }

        if (m_seekable)
            m_backend->logMessage("Stream is seekable", Backend::Info, this);
        else
            m_backend->logMessage("Stream is non-seekable", Backend::Info, this);
    } else {
        m_backend->logMessage("updateSeekable query failed", Backend::Info, this);
    }
    gst_query_unref (query);
}

qint64 MediaObject::getPipelinePos() const
{
    Q_ASSERT(m_pipeline);

    // Note some formats (usually mpeg) do not allow us to accurately seek to the
    // beginning or end of the file so we 'fake' it here rather than exposing the front end to potential issues.
    if (m_atEndOfStream)
        return totalTime();
    if (m_atStartOfStream)
        return 0;

    gint64 pos = 0;
    GstFormat format = GST_FORMAT_TIME;
    gst_element_query_position (GST_ELEMENT(m_pipeline), &format, &pos);
    return (pos / GST_MSECOND);
}

/*
 * Internal method to set a new total time for the media object
 */
void MediaObject::setTotalTime(qint64 newTime)
{

    if (newTime == m_totalTime)
        return;

    m_totalTime = newTime;

    emit totalTimeChanged(m_totalTime);
}

/*
 * !reimp
 */
void MediaObject::setSource(const MediaSource &source)
{
    if (!isValid())
        return;

    // We have to reset the state completely here, otherwise
    // remnants of the old pipeline can result in strangenes
    // such as failing duration queries etc
    GstState state;
    gst_element_set_state(m_pipeline, GST_STATE_NULL);
    gst_element_get_state (m_pipeline, &state, NULL, 2000);

    m_source = source;
    emit currentSourceChanged(m_source);
    m_previousTickTime = -1;
    m_missingCodecs.clear();

    // Go into to loading state
    changeState(Phonon::LoadingState);
    m_loading = true;
    m_resetNeeded = false;
    m_resumeState = false;
    m_pendingState = Phonon::StoppedState;

     // Make sure we start out unconnected
    if (GST_ELEMENT_PARENT(m_audioGraph))
        gst_bin_remove(GST_BIN(m_pipeline), m_audioGraph);
    if (GST_ELEMENT_PARENT(m_videoGraph))
        gst_bin_remove(GST_BIN(m_pipeline), m_videoGraph);

    // Clear any existing errors
    m_aboutToFinishEmitted = false;
    m_error = NoError;
    m_errorString = QString();
    
    m_bufferPercent = 0;
    m_prefinishMarkReachedNotEmitted = true;
    m_aboutToFinishEmitted = false;
    m_hasAudio = false;
    m_videoStreamFound = false;
    setTotalTime(-1);
    m_atEndOfStream = false;

    // Clear exising meta tags
    m_metaData.clear();

    switch (source.type()) {
    case MediaSource::Url: {
            QString urlString = source.url().toEncoded();
            if (!createPipefromURL(urlString)) {
                setError(tr("Could not open media source."));
                return;
            }
        }
        break;

    case MediaSource::LocalFile: {
            QString urlString = QUrl::fromLocalFile(source.fileName()).toString();
            if (!createPipefromURL(urlString)) {
                setError(tr("Could not open media source."));
                return;
            }
        }
        break;

    case MediaSource::Invalid:
        setError(tr("Invalid source type."), Phonon::NormalError);
        break;

    case MediaSource::Empty:
        break;

    case MediaSource::Stream:
        if (!createPipefromStream(source)) {
            setError(tr("Could not open media source."));
            return;
        }
        break;

    case MediaSource::Disc: // CD tracks can be specified by setting the url in the following way uri=cdda:4
        m_backend->logMessage("Source type Disc not currently supported", Backend::Warning, this);
        setError(tr("Could not open media source."), Phonon::NormalError);
        break;

    default:
        m_backend->logMessage("Source type not currently supported", Backend::Warning, this);
        setError(tr("Could not open media source."), Phonon::NormalError);
        return;
    }

    // Setting to state paused will trigger fetching meta data and duration
    MediaNodeEvent event(MediaNodeEvent::SourceChanged);
    notify(&event);

    // We need to link this node to ensure that fake sinks are connected
    // before loading, otherwise the stream will be blocked
    link();

    beginLoad();
}

void MediaObject::beginLoad()
{
    if (gst_element_set_state(m_pipeline, GST_STATE_PAUSED) != GST_STATE_CHANGE_FAILURE) {
        m_backend->logMessage("Begin source load", Backend::Info, this);
    } else {
        setError(tr("Could not open media source."));
    }
}

// Called when we are ready to leave the loading state
void MediaObject::loadingComplete()
{
    if (m_videoStreamFound) {
        MediaNodeEvent event(MediaNodeEvent::VideoAvailable);
        notify(&event);
    }
    getStreamInfo();
    m_loading = false;

    setState(m_pendingState);
    emit metaDataChanged(m_metaData);
}

void MediaObject::getStreamInfo()
{
    updateSeekable();
    updateTotalTime();

    if (m_videoStreamFound != m_hasVideo) {
        m_hasVideo = m_videoStreamFound;
        emit hasVideoChanged(m_hasVideo);
    }
}

void MediaObject::setPrefinishMark(qint32 newPrefinishMark)
{
    m_prefinishMark = newPrefinishMark;
    if (currentTime() < totalTime() - m_prefinishMark) // not about to finish
        m_prefinishMarkReachedNotEmitted = true;
}

void MediaObject::pause()
{
    m_backend->logMessage("pause()", Backend::Info, this);
    if (state() != Phonon::PausedState)
        setState(Phonon::PausedState);
    m_resumeState = false;
}

void MediaObject::stop()
{
    if (state() != Phonon::StoppedState) {
        setState(Phonon::StoppedState);
        m_prefinishMarkReachedNotEmitted = true;
    }
    m_resumeState = false;
}

void MediaObject::seek(qint64 time)
{
    if (!isValid())
        return;

    Phonon::State oldState = state();

    if (isSeekable()) {
        switch (state()) {
        case Phonon::PlayingState:
        case Phonon::StoppedState:
        case Phonon::PausedState:
        case Phonon::BufferingState:
            m_backend->logMessage(QString("Seek to pos %0").arg(time), Backend::Info, this);

            if (time <= 0)
                m_atStartOfStream = true;
            else
                m_atStartOfStream = false;

            // Go to buffering state, we resume paused state when ready
            if (gst_element_seek(m_pipeline, 1.0, GST_FORMAT_TIME,
                                 GST_SEEK_FLAG_FLUSH, GST_SEEK_TYPE_SET,
                                 time * GST_MSECOND, GST_SEEK_TYPE_NONE, GST_CLOCK_TIME_NONE))
            setState(oldState);
            break;
        case Phonon::LoadingState:
        case Phonon::ErrorState:
            return;
        }

        quint64 current = currentTime();
        quint64 total = totalTime(); 

        if (current < total - m_prefinishMark)
            m_prefinishMarkReachedNotEmitted = true;
        if (current < total - ABOUT_TO_FINNISH_TIME)
            m_aboutToFinishEmitted = false;
        m_atEndOfStream = false;
    }
}

void MediaObject::emitTick()
{
    if (m_resumeState) {
        return;
    }

    qint64 currentTime = getPipelinePos();
    qint64 totalTime = m_totalTime;

    if (m_tickInterval > 0 && currentTime != m_previousTickTime) {
        emit tick(currentTime);
        m_previousTickTime = currentTime;        
    }
    if (m_state == Phonon::PlayingState) {
        if (currentTime >= totalTime - m_prefinishMark) {
            if (m_prefinishMarkReachedNotEmitted) {
                m_prefinishMarkReachedNotEmitted = false;
                emit prefinishMarkReached(totalTime - currentTime);
            }
        }
        // Prepare load of next source
        if (currentTime >= totalTime - ABOUT_TO_FINNISH_TIME) {
            if (!m_aboutToFinishEmitted) {
                m_aboutToFinishEmitted = true; // track is about to finish
                emit aboutToFinish();
            }
        }
    }
}


/*
 * Used to iterate through the gst_tag_list and extract values
 */
void foreach_tag_function(const GstTagList *list, const gchar *tag, gpointer user_data)
{
    TagMap *newData = static_cast<TagMap *>(user_data);
    QString value;
    GType type = gst_tag_get_type(tag);
    switch (type) {
    case G_TYPE_STRING: {
            char *str = 0;
            gst_tag_list_get_string(list, tag, &str);
            value = QString::fromUtf8(str);
            g_free(str);
        }
        break;

    case G_TYPE_BOOLEAN: {
            int bval;
            gst_tag_list_get_boolean(list, tag, &bval);
            value = QString::number(bval);
        }
        break;

    case G_TYPE_INT: {
            int ival;
            gst_tag_list_get_int(list, tag, &ival);
            value = QString::number(ival);
        }
        break;

    case G_TYPE_UINT: {
            unsigned int uival;
            gst_tag_list_get_uint(list, tag, &uival);
            value = QString::number(uival);
        }
        break;

    case G_TYPE_FLOAT: {
            float fval;
            gst_tag_list_get_float(list, tag, &fval);
            value = QString::number(fval);
        }
        break;

    case G_TYPE_DOUBLE: {
            double dval;
            gst_tag_list_get_double(list, tag, &dval);
            value = QString::number(dval);
        }
        break;

    default:
        //qDebug("Unsupported tag type: %s", g_type_name(type));
        break;
    }

    QString key = QString(tag).toUpper();
    QString currVal = newData->value(key);
    if (!value.isEmpty() && !(newData->contains(key) && currVal == value))
        newData->insert(key, value);
}

/**
 * Triggers playback after a song has completed in the current media queue
 */
void MediaObject::beginPlay()
{
    setSource(m_nextSource);
    m_nextSource = MediaSource();
    m_pendingState = Phonon::PlayingState;
}

/**
 * Handle GStreamer bus messages
 */
void MediaObject::handleBusMessage(const Message &message)
{

    if (!isValid())
        return;

    GstMessage *gstMessage = message.rawMessage();
    Q_ASSERT(m_pipeline);

    if (m_backend->debugLevel() >= Backend::Debug) {
        int type = GST_MESSAGE_TYPE(gstMessage);
        gchar* name = gst_element_get_name(gstMessage->src);
        QString msgString = QString("Bus: %0 (%1)").arg(gst_message_type_get_name ((GstMessageType)type)).arg(name);
        g_free(name);
        m_backend->logMessage(msgString, Backend::Debug, this);
    }

    switch (GST_MESSAGE_TYPE (gstMessage)) {

    case GST_MESSAGE_EOS: 
        m_backend->logMessage("EOS recieved", Backend::Info, this);
        handleEndOfStream();
        break;

    case GST_MESSAGE_TAG: {
            GstTagList* tag_list = 0;
            gst_message_parse_tag(gstMessage, &tag_list);
            if (tag_list) {
                TagMap oldMap = m_metaData; // Keep a copy of the old one for reference
                // Append any new meta tags to the existing tag list
                gst_tag_list_foreach (tag_list, &foreach_tag_function, &m_metaData);
                m_backend->logMessage("Meta tags found", Backend::Info, this);
                if (oldMap != m_metaData && !m_loading)
                    emit metaDataChanged(m_metaData);
                gst_tag_list_free(tag_list);
            }
        }
        break;

    case GST_MESSAGE_STATE_CHANGED : {

            if (gstMessage->src != GST_OBJECT(m_pipeline))
                return;

            GstState oldState;
            GstState newState;
            gst_message_parse_state_changed (gstMessage, &oldState, &newState, 0);

            switch (newState) {

            case GST_STATE_PLAYING :
                m_atStartOfStream = false;
                m_backend->logMessage("gstreamer: pipeline state set to playing", Backend::Info, this);
                m_tickTimer->start();
                changeState(Phonon::PlayingState);
                if (m_resumeState && m_oldState == Phonon::PlayingState) {
                    seek(m_oldPos);
                    m_resumeState = false;
                }
                break;

            case GST_STATE_NULL:
                m_backend->logMessage("gstreamer: pipeline state set to null", Backend::Info, this);
                m_tickTimer->stop();
                break;

            case GST_STATE_PAUSED :
                m_backend->logMessage("gstreamer: pipeline state set to paused", Backend::Info, this);
                m_tickTimer->start();
                if (state() == Phonon::LoadingState) {
                    // No_more_pads is not emitted from the decodebin in older versions (0.10.4)
                    noMorePadsAvailable();
                    loadingComplete();
                } else if (m_resumeState && m_oldState == Phonon::PausedState) {
                    changeState(Phonon::PausedState);
                    m_resumeState = false;
                    break;
                } else {
                    // A lot of autotests can break if we allow all paused changes through.
                    if (m_pendingState == Phonon::PausedState) {
                        changeState(Phonon::PausedState);
                    }
                }
                break;

            case GST_STATE_READY :
                if (!m_loading && m_pendingState == Phonon::StoppedState)
                    changeState(Phonon::StoppedState);
                m_backend->logMessage("gstreamer: pipeline state set to ready", Backend::Debug, this);
                m_tickTimer->stop();
                break;

            case GST_STATE_VOID_PENDING :
                m_backend->logMessage("gstreamer: pipeline state set to pending (void)", Backend::Debug, this);
                m_tickTimer->stop();
                break;
            }
            break;
        }

    case GST_MESSAGE_ERROR: {
            gchar *debug;
            GError *err;
            QString logMessage;
            gst_message_parse_error (gstMessage, &err, &debug);
            gchar *errorMessage = gst_error_get_message (err->domain, err->code);
            logMessage.sprintf("Error: %s Message:%s (%s) Code:%d", debug, err->message, errorMessage, err->code);
            m_backend->logMessage(logMessage, Backend::Warning);
            g_free(errorMessage);
            g_free (debug);

            if (err->domain == GST_RESOURCE_ERROR) {
                if (err->code == GST_RESOURCE_ERROR_NOT_FOUND) {
                    setError(tr("Could not locate media source."), Phonon::FatalError);
                } else if (err->code == GST_RESOURCE_ERROR_OPEN_READ) {
                    setError(tr("Could not open media source."), Phonon::FatalError);
                } else if (err->code == GST_RESOURCE_ERROR_BUSY) {
                   // We need to check if this comes from an audio device by looking at sink caps
                   GstPad* sinkPad = gst_element_get_static_pad(GST_ELEMENT(gstMessage->src), "sink");
                   if (sinkPad) {
                        GstCaps *caps = gst_pad_get_caps (sinkPad);
                        GstStructure *str = gst_caps_get_structure (caps, 0);
                        if (g_strrstr (gst_structure_get_name (str), "audio"))
                            setError(tr("Could not open audio device. The device is already in use."), Phonon::NormalError);
                        else
                            setError(err->message, Phonon::FatalError);
                        gst_caps_unref (caps);
                        gst_object_unref (sinkPad);
                   } 
               } else {
                    setError(QString(err->message), Phonon::FatalError);
               }
           } else if (err->domain == GST_STREAM_ERROR) {
                switch (err->code) {
                case GST_STREAM_ERROR_WRONG_TYPE:
                case GST_STREAM_ERROR_TYPE_NOT_FOUND:
                    setError(tr("Could not decode media source."), Phonon::FatalError);
                    break;
                default:
                    setError(tr("Could not open media source."), Phonon::FatalError);
                    break;
                }
           } else {
                setError(QString(err->message), Phonon::FatalError);
            }
            g_error_free (err);
            break;
        }

    case GST_MESSAGE_WARNING: {
            gchar *debug;
            GError *err;
            gst_message_parse_warning(gstMessage, &err, &debug);
            QString msgString;
            msgString.sprintf("Warning: %s\nMessage:%s", debug, err->message);
            m_backend->logMessage(msgString, Backend::Warning);
            g_free (debug);
            g_error_free (err);
            break;
        }

    case GST_MESSAGE_ELEMENT: {
            GstMessage *gstMessage = message.rawMessage();
            const GstStructure *gstStruct = gst_message_get_structure(gstMessage); //do not free this
            if (g_strrstr (gst_structure_get_name (gstStruct), "prepare-xwindow-id")) {
                MediaNodeEvent videoHandleEvent(MediaNodeEvent::VideoHandleRequest);
                notify(&videoHandleEvent);
            }
            break;
        }

    case GST_MESSAGE_DURATION: {
            m_backend->logMessage("GST_MESSAGE_DURATION", Backend::Debug, this);
            updateTotalTime();
            break;
        }

    case GST_MESSAGE_BUFFERING: {
            gint percent = 0;
            gst_structure_get_int (gstMessage->structure, "buffer-percent", &percent); //gst_message_parse_buffering was introduced in 0.10.11

            if (m_bufferPercent != percent) {
                emit bufferStatus(percent);
                m_backend->logMessage(QString("Stream buffering %0").arg(percent), Backend::Debug, this);
                m_bufferPercent = percent;
            }

            if (m_state != Phonon::BufferingState)
                emit stateChanged(m_state, Phonon::BufferingState);
            else if (percent == 100)
                emit stateChanged(Phonon::BufferingState, m_state);
            break;
        }
        //case GST_MESSAGE_INFO:
        //case GST_MESSAGE_STREAM_STATUS:
        //case GST_MESSAGE_CLOCK_PROVIDE:
        //case GST_MESSAGE_NEW_CLOCK:
        //case GST_MESSAGE_STEP_DONE:
        //case GST_MESSAGE_LATENCY: only from 0.10.12
        //case GST_MESSAGE_ASYNC_DONE: only from 0.10.13
    default: 
        break; 
    }
}

void MediaObject::handleEndOfStream()
{
    // If the stream is not seekable ignore
    // otherwise chained radio broadcasts would stop
    if (m_atEndOfStream || !isSeekable())
        return;

    m_atEndOfStream = true;

    if (m_nextSource.type() != MediaSource::Invalid
        && m_nextSource.type() != MediaSource::Empty) {  // We only emit finish when the queue is actually empty
        QTimer::singleShot (qMax(0, transitionTime()), this, SLOT(beginPlay()));
    } else {
        m_pendingState = Phonon::PausedState;
        emit finished();
        // Only emit paused if the finished signal
        // did not result in a new state
        if (m_pendingState == Phonon::PausedState)
            setState(m_pendingState);
    }
}

// Notifes the pipeline about state changes in the media object
void MediaObject::notifyStateChange(Phonon::State newstate, Phonon::State oldstate)
{
    Q_UNUSED(oldstate);
    MediaNodeEvent event(MediaNodeEvent::StateChanged, &newstate);
    notify(&event);
}

} // ns Gstreamer
} // ns Phonon

QT_END_NAMESPACE

#include "moc_mediaobject.cpp"
