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

#ifndef PHONON_MEDIAOBJECT_H
#define PHONON_MEDIAOBJECT_H

#include <phonon/mediaobjectinterface.h>
#include <phonon/addoninterface.h>

#include <QtCore/QHash>
#include <QtCore/QObject>
#include <QtCore/QQueue>
#include <QtCore/QBasicTimer>
#include <QtCore/QMutex>
#include <QtCore/QThread>

#include "backendnode.h"
#include "mediagraph.h"

QT_BEGIN_NAMESPACE

namespace Phonon
{
    class MediaSource;

    namespace DS9
    {
        class VideoWidget;
        class AudioOutput;

        class QWinWaitCondition
        {
        public:
            QWinWaitCondition() : m_handle(::CreateEvent(0,0,0,0))
            {
            }

            ~QWinWaitCondition()
            {
                ::CloseHandle(m_handle);
            }

            void reset()
            {
                //will block
                ::ResetEvent(m_handle);
            }

            void set()
            {
                //will unblock
                ::SetEvent(m_handle);
            }

            operator HANDLE()
            {
                return m_handle;
            }

            operator HEVENT()
            {
                return reinterpret_cast<HEVENT>(m_handle);
            }


        private:
            HANDLE m_handle;
        };

        class WorkerThread : public QThread
        {
            Q_OBJECT
        public:
            WorkerThread();
            ~WorkerThread();

            virtual void run();

            //wants to know as soon as the state is set
            void addStateChangeRequest(Graph graph, OAFilterState, QList<Filter> = QList<Filter>());

            quint16 addSeekRequest(Graph graph, qint64 time);
            quint16 addUrlToRender(const QString &url);
            quint16 addFilterToRender(const Filter &filter);

            void replaceGraphForEventManagement(Graph newGraph, Graph oldGraph);

    		void abortCurrentRender(qint16 renderId);

            //tells the thread to stop processing
            void signalStop();

        Q_SIGNALS:
            void asyncRenderFinished(quint16, HRESULT, Graph);
            void asyncSeekingFinished(quint16, qint64);
            void stateReady(Graph, Phonon::State);
            void eventReady(Graph, long eventCode, long param1);

        private:

            enum Task
            {
                Render,
                Seek,
                ChangeState,
                ReplaceGraph //just updates recalls WaitForMultipleObject
            };

            struct Work
            {
                Task task;
                quint16 id;
                Graph graph;
                Graph oldGraph;
                Filter filter;
                QString url;
                union
                {
                    qint64 time;
                    OAFilterState state;
                };
                QList<Filter> decoders; //for the state change requests
            };
            Work dequeueWork();
            void handleTask();

            Graph m_currentRender;
			qint16 m_currentRenderId;
            QQueue<Work> m_queue;
            bool m_finished;
            quint16 m_currentWorkId;
            QWinWaitCondition m_waitCondition;
            QMutex m_mutex;

            //this is for WaitForMultipleObjects
            struct
            {
                Graph graph;
                HANDLE handle;
            } m_graphHandle[FILTER_COUNT];
        };


        class MediaObject : public BackendNode, public Phonon::MediaObjectInterface
#ifndef QT_NO_PHONON_ABSTRACTMEDIASTREAM
            , public Phonon::AddonInterface
#endif //QT_NO_PHONON_ABSTRACTMEDIASTREAM
        {
            friend class Stream;
            Q_OBJECT
                Q_INTERFACES(Phonon::MediaObjectInterface 
#ifndef QT_NO_PHONON_ABSTRACTMEDIASTREAM
                    Phonon::AddonInterface
#endif //QT_NO_PHONON_ABSTRACTMEDIASTREAM
                    )
        public:
            MediaObject(QObject *parent);
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
            void ensureStopped();
            void seek(qint64 time);

            QString errorString() const;
            Phonon::ErrorType errorType() const;

#ifndef QT_NO_PHONON_ABSTRACTMEDIASTREAM
            bool hasInterface(Interface) const;
            QVariant interfaceCall(Interface iface, int command, const QList<QVariant> &params);
#endif //QT_NO_PHONON_ABSTRACTMEDIASTREAM

            qint64 totalTime() const;
            qint32 prefinishMark() const;
            void setPrefinishMark(qint32 newPrefinishMark);

            qint32 transitionTime() const;
            void setTransitionTime(qint32);

            qint64 remainingTime() const;

            MediaSource source() const;
            void setSource(const MediaSource &source);
            void setNextSource(const MediaSource &source);


            //COM error management
            bool catchComError(HRESULT hr);

            void grabNode(BackendNode *node);
            bool connectNodes(BackendNode *source, BackendNode *sink);
            bool disconnectNodes(BackendNode *source, BackendNode *sink);

            void switchFilters(int index, Filter oldFilter, Filter newFilter);

            WorkerThread *workerThread();
            void loadingFinished(MediaGraph *mg);
            void seekingFinished(MediaGraph *mg);
            MediaGraph *currentGraph() const;

            //this is used by the backend only
            Phonon::State transactionState;

         private Q_SLOTS:
            void switchToNextSource();
            void slotStateReady(Graph, Phonon::State);
            void handleEvents(Graph, long eventCode, long param1);
            void finishLoading(quint16 workId, HRESULT hr, Graph);
            void finishSeeking(quint16 workId, qint64 time);

         Q_SIGNALS:
            void stateChanged(Phonon::State newstate, Phonon::State oldstate);
            void tick(qint64 time);
            void metaDataChanged(QMultiMap<QString, QString>);
            void seekableChanged(bool);
            void hasVideoChanged(bool);
            void bufferStatus(int);

            // AddonInterface:
            void titleChanged(int);
            void availableTitlesChanged(int);
            void chapterChanged(int);
            void availableChaptersChanged(int);
            void angleChanged(int);
            void availableAnglesChanged(int);

            void finished();
            void prefinishMarkReached(qint32);
            void aboutToFinish();
            void totalTimeChanged(qint64 length) const;
            void currentSourceChanged(const MediaSource &);

        protected:
            void setState(Phonon::State);
            void timerEvent(QTimerEvent *e);

        private:
#ifndef QT_NO_PHONON_VIDEO
            void updateVideoGeometry();
#endif // QT_NO_PHONON_VIDEO
            void handleComplete(IGraphBuilder *graph);
            MediaGraph *nextGraph() const;

            void updateTargetTick();
            void updateStopPosition();

            mutable QString m_errorString;
            mutable Phonon::ErrorType m_errorType;

            Phonon::State m_state;
            Phonon::State m_nextState;
            qint32 m_transitionTime;

            qint32 m_prefinishMark;

            QBasicTimer m_tickTimer;
            qint32 m_tickInterval;

            //the graph(s)
            MediaGraph* m_graphs[FILTER_COUNT];

            //...the videowidgets in the graph
            QList<VideoWidget*> m_videoWidgets;
            QList<AudioOutput*> m_audioOutputs;

            bool m_buffering:1;
            bool m_oldHasVideo:1;
            bool m_prefinishMarkSent:1;
            bool m_aboutToFinishSent:1;
            bool m_nextSourceReadyToStart:1;

            //for TitleInterface (and commands)
#ifndef QT_NO_PHONON_MEDIACONTROLLER
            bool m_autoplayTitles:1;
            QList<qint64> m_titles;
            int m_currentTitle;
            int _iface_availableTitles() const;
            int _iface_currentTitle() const;
            void _iface_setCurrentTitle(int title, bool bseek = true);
            void setTitles(const QList<qint64> &titles);
            qint64 titleAbsolutePosition(int title) const;
#endif //QT_NO_PHONON_MEDIACONTROLLER
            qint64 m_targetTick;

            WorkerThread m_thread;
        };
    }
}

QT_END_NAMESPACE

#endif // PHONON_MEDIAOBJECT_H
