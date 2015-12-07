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

#ifndef PHONON_MEDIAGRAPH_H
#define PHONON_MEDIAGRAPH_H

#include "backendnode.h"
#include <QtCore/QSet>
#include <QtCore/QMultiMap>

#include <phonon/mediasource.h>

//#define GRAPH_DEBUG

QT_BEGIN_NAMESPACE

uint qHash (const Phonon::DS9::Filter &f);

namespace Phonon
{
    namespace DS9
    {
        class MediaObject;

        //in the end we should probably have no more inheritance here: everything should be in the interface of the class
        //could be nice to then remove all the "*this" in the code of this class
        class MediaGraph : public QObject
        {
            Q_OBJECT
        public:
            MediaGraph(MediaObject *mo, short index);
            ~MediaGraph();
            bool isSeekable() const;
            qint64 absoluteTotalTime() const;
            qint64 absoluteCurrentTime() const;
            void play();
            void stop();
            void pause();
            void absoluteSeek(qint64);
            HRESULT saveToFile(const QString &filepath) const;
            QMultiMap<QString, QString> metadata() const;

            static QSet<Filter> getAllFilters(Graph graph);
            QSet<Filter> getAllFilters() const;

            HRESULT loadSource(const Phonon::MediaSource &);

            bool hasVideo() const { return m_hasVideo; }
            void grabNode(BackendNode *node);

            //connections of the nodes
            bool connectNodes(BackendNode *source, BackendNode *sink);
            bool disconnectNodes(BackendNode *source, BackendNode *sink);

            Phonon::MediaSource mediaSource() const;

            //before loading a source, and after its playback this will be called
            HRESULT cleanup();
            void ensureStopped();

            short index() const;

            Filter realSource() const;
            QList<qint64> titles() const;

            void setStopPosition(qint64 time);
            qint64 stopPosition() const;

            void switchFilters(Filter oldFilter, Filter newFilter);
            OAFilterState syncGetRealState() const;

            bool isLoading() const;
            bool isStopping() const;
            HRESULT renderResult() const;

            Graph graph() const;

        Q_SIGNALS:
            void loadingFinished(MediaGraph*);
            void seekingFinished(MediaGraph*);

        private Q_SLOTS:
            void finishLoading(quint16 workId, HRESULT hr, QSet<Filter> newlyCreated, bool isSeekable);
            void finishSeeking(quint16 workId, qint64 time);


            bool isSourceFilter(const Filter &filter) const;
            bool isDemuxerFilter(const Filter &filter) const;
            bool isDecoderFilter(const Filter &filter);
            static QSet<Filter> getFilterChain(const Filter &source, const Filter &sink);

        private:
            HRESULT reallyFinishLoading(HRESULT, const QSet<Filter> &);


            //utility functions
            void ensureSourceConnectedTo(bool force = false);
            void ensureSourceDisconnected();
            bool tryConnect(const OutputPin &, const InputPin &);
            bool tryDisconnect(const OutputPin &, const InputPin &);
            HRESULT removeFilter(const Filter& filter);

            //after loading, removes the decoders that are not linked to a sink
            HRESULT removeUselessDecoders();

            //COM objects
            Graph m_graph;
            ComPointer<IMediaControl> m_mediaControl;
            ComPointer<IMediaSeeking> m_mediaSeeking;
            Filter m_fakeSource, m_realSource;
            Filter m_demux;
            QSet<OutputPin> m_decoderPins;
            QSet<Filter> m_decoders;

            bool m_hasVideo;
            bool m_hasAudio;
            bool m_connectionsDirty;
            bool m_isStopping;
            bool m_isSeekable;
            HRESULT m_result;
            quint16 m_index;
            quint16 m_renderId;
            quint16 m_seekId;

            //while seeking we need to store the current time
            qint64 m_currentTime;
            qint64 m_totalTime;

            MediaObject *m_mediaObject;
            Phonon::MediaSource m_mediaSource;
            QSet<BackendNode*> m_sinkConnections; //connections to the source

            Q_DISABLE_COPY(MediaGraph);
        };
    }
}

QT_END_NAMESPACE

#endif // PHONON_MEDIAGRAPH_H
