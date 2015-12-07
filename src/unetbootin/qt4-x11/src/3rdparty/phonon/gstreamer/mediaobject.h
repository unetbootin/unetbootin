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

#ifndef Phonon_GSTREAMER_MEDIAOBJECT_H
#define Phonon_GSTREAMER_MEDIAOBJECT_H

#include "backend.h"
#include "common.h"
#include "medianode.h"

#include <phonon/mediaobjectinterface.h>
#include <phonon/addoninterface.h>

#include <QtCore/QHash>
#include <QtCore/QString>
#include <QtCore/QVariant>
#include <QtCore/QObject>
#include <QtCore/QDate>
#include <QtCore/QEvent>
#include <QtCore/QUrl>

#include <gst/gst.h>

QT_BEGIN_NAMESPACE

class QTimer;
typedef QMultiMap<QString, QString> TagMap;

namespace Phonon
{
namespace Gstreamer
{

class VideoWidget;
class AudioPath;
class VideoPath;
class AudioOutput;

class MediaObject : public QObject, public MediaObjectInterface, public AddonInterface, public MediaNode
{
    friend class Stream;
    Q_OBJECT
    Q_INTERFACES(Phonon::MediaObjectInterface Phonon::AddonInterface Phonon::Gstreamer::MediaNode)

public:

    MediaObject(Backend *backend, QObject *parent);
    ~MediaObject();
    Phonon::State state() const;

    bool hasVideo() const;
    bool isSeekable() const;

    qint64 currentTime() const;
    qint32 tickInterval() const;

    void setTickInterval(qint32 newTickInterval);

    void play();
    void pause();
    void stop();
    void seek(qint64 time);

    QString errorString() const;
    Phonon::ErrorType errorType() const;

    QUrl url() const;
    qint64 totalTime() const;

    qint32 prefinishMark() const;
    void setPrefinishMark(qint32 newPrefinishMark);

    qint32 transitionTime() const;
    void setTransitionTime(qint32);
    qint64 remainingTime() const;

    void setSource(const MediaSource &source);
    void setNextSource(const MediaSource &source);
    MediaSource source() const;

    // No additional interfaces currently supported
    bool hasInterface(Interface) const
    {
        return false;
    }

    QVariant interfaceCall(Interface, int, const QList<QVariant> &)
    {
        return QVariant();
    }

    bool isLoading()
    {
        return m_loading;
    }

    bool audioAvailable()
    {
        return m_hasAudio;
    }

    bool videoAvailable()
    {
        return m_hasVideo;
    }

    GstElement *audioGraph()
    {
        return m_audioGraph;
    }

    GstElement *videoGraph()
    {
        return m_videoGraph;
    }

    GstElement *pipeline()
    {
        return m_pipeline;
    };

    gulong capsHandler()
    {
        return m_capsHandler;
    };

    void connectVideo(GstPad *videoPad);
    void connectAudio(GstPad *audioPad);
    void handleBusMessage(const Message &msg);
    void handleEndOfStream();
    void addMissingCodecName(const QString &codec) { m_missingCodecs.append(codec); }
    void invalidateGraph() {
        m_resetNeeded = true;
        if (m_state == Phonon::PlayingState || m_state == Phonon::PausedState) {
            changeState(Phonon::StoppedState);
        }
    }
    static void cb_newpad (GstElement *decodebin, GstPad *pad, gboolean last, gpointer data);
    static void cb_unknown_type (GstElement *decodebin, GstPad *pad, GstCaps *caps, gpointer data);
    static void cb_no_more_pads (GstElement * decodebin, gpointer data);
    void saveState();
    void resumeState();

public Q_SLOTS:
    void setState(State);
    
Q_SIGNALS:
    void currentSourceChanged(const MediaSource &newSource);
    void stateChanged(Phonon::State newstate, Phonon::State oldstate);
    void tick(qint64 time);
    void metaDataChanged(QMultiMap<QString, QString>);
    void seekableChanged(bool);
    void hasVideoChanged(bool);

    void finished();
    void prefinishMarkReached(qint32);
    void aboutToFinish();
    void totalTimeChanged(qint64 length);
    void bufferStatus(int percentFilled);

    QMultiMap<QString, QString> metaData();
    void setMetaData(QMultiMap<QString, QString> newData);

protected:
    void beginLoad();
    void loadingComplete();
    void newPadAvailable (GstPad *pad);
    void changeState(State);
    void setError(const QString &errorString, Phonon::ErrorType error = NormalError);
    /*
     * @param encodedUrl percent-encoded QString for source compat reasons.  Should change to QUrl
     */
    bool createPipefromURL(const QString &encodedUrl);
    bool createPipefromStream(const MediaSource &);

private Q_SLOTS:
    void noMorePadsAvailable();    
    void getStreamInfo();
    void emitTick();
    void beginPlay();
    void setVideoCaps(GstCaps *caps);
    void notifyStateChange(Phonon::State newstate, Phonon::State oldstate);
protected:
    GstElement *audioElement()
    {
        Q_ASSERT(m_audioPipe);
        return m_audioPipe;
    }

    GstElement *videoElement()
    {
        Q_ASSERT(m_videoPipe);
        return m_videoPipe;
    }

private:

    // GStreamer specific :
    void createPipeline();
    bool addToPipeline(GstElement *elem);
    void setTotalTime(qint64 newTime);
    void getStreamsInfo();
    bool updateTotalTime();
    void updateSeekable();
    qint64 getPipelinePos() const;

    bool m_resumeState;
    State m_oldState;
    quint64 m_oldPos;

    State m_state;
    State m_pendingState;
    QTimer *m_tickTimer;
    qint32 m_tickInterval;

    MediaSource m_source;
    MediaSource m_nextSource;
    qint32 m_prefinishMark;
    qint32 m_transitionTime;

    bool m_prefinishMarkReachedNotEmitted;
    bool m_aboutToFinishEmitted;
    bool m_loading;
    gulong m_capsHandler;

    GstElement *m_datasource;
    GstElement *m_decodebin;

    GstElement *m_audioPipe;
    GstElement *m_videoPipe;

    qint64 m_totalTime;
    int m_bufferPercent;
    bool m_hasVideo;
    bool m_videoStreamFound;
    bool m_hasAudio;
    bool m_seekable;
    bool m_atEndOfStream;
    bool m_atStartOfStream;
    Phonon::ErrorType m_error;
    QString m_errorString;

    GstElement *m_pipeline;
    GstElement *m_audioGraph;
    GstElement *m_videoGraph;
    int m_previousTickTime;
    bool m_resetNeeded;
    QStringList m_missingCodecs;
    QMultiMap<QString, QString> m_metaData;
};
}
} //namespace Phonon::Gstreamer

QT_END_NAMESPACE

#endif // Phonon_GSTREAMER_MEDIAOBJECT_H
