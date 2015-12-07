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

#ifndef PHONON_MEDIAOBJECT_H
#define PHONON_MEDIAOBJECT_H

#include <phonon/mediaobjectinterface.h>
#include <phonon/addoninterface.h>

#include <QtCore/QHash>
#include <QtCore/QObject>
#include <QtCore/QQueue>
#include <QtCore/QBasicTimer>
#include <QtCore/QWaitCondition>
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

            virtual void run();

            //wants to know as soon as the state is set
            void addStateChangeRequest(Graph graph, OAFilterState, QSet<Filter> = QSet<Filter>());

            quint16 addSeekRequest(Graph graph, qint64 time);
            quint16 addUrlToRender(const QString &url);
            quint16 addFilterToRender(const Filter &filter);

            void addGraphForEventManagement(Graph graph);

    		void abortCurrentRender(qint16 renderId);

            //tells the thread to stop processing
            void signalStop();

        Q_SIGNALS:
            void asyncRenderFinished(quint16, HRESULT, QSet<Filter>, bool);
            void asyncSeekingFinished(quint16, qint64);
            void stateReady(IGraphBuilder*, Phonon::State);
            void eventReady(IGraphBuilder*, long eventCode, long param1);

        private:

            enum Task
            {
                Render,
                Seek,
                ChangeState,
                AddGraph //just updates recalls WaitForMultipleObject
            };

            struct Work
            {
                Task task;
                quint16 id;
                Graph graph;
                Filter filter;
                QString url;
                union
                {
                    qint64 time;
                    OAFilterState state;
                };
                QSet<Filter> decoders; //for the state change requests
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
            QVector<Graph> m_graphs;

            //this is for WaitForMultipleObjects
            QVector<HANDLE> m_handles;
        };


        class MediaObject : public BackendNode, public Phonon::MediaObjectInterface, public Phonon::AddonInterface
        {
            friend class Stream;
            Q_OBJECT
                Q_INTERFACES(Phonon::MediaObjectInterface Phonon::AddonInterface)
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

            bool hasInterface(Interface) const;
            QVariant interfaceCall(Interface iface, int command, const QList<QVariant> &params);

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

         private Q_SLOTS:
            void switchToNextSource();
            void loadingFinished(MediaGraph *mg);
            void seekingFinished(MediaGraph *mg);
            void slotStateReady(IGraphBuilder*, Phonon::State);
            void handleEvents(IGraphBuilder *graph, long eventCode, long param1);

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
            void updateVideoGeometry();
            void handleComplete(IGraphBuilder *graph);
            MediaGraph *currentGraph() const;
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
            QList<MediaGraph*> m_graphs;

            //...the videowidgets in the graph
            QSet<VideoWidget*> m_videoWidgets;
            QSet<AudioOutput*> m_audioOutputs;

            bool m_buffering:1;
            bool m_oldHasVideo:1;
            bool m_prefinishMarkSent:1;
            bool m_aboutToFinishSent:1;

            //for TitleInterface (and commands)
            bool m_autoplayTitles:1;
            QList<qint64> m_titles;
            int m_currentTitle;
            int _iface_availableTitles() const;
            int _iface_currentTitle() const;
            void _iface_setCurrentTitle(int title, bool bseek = true);
            void setTitles(const QList<qint64> &titles);
            qint64 titleAbsolutePosition(int title) const;

            qint64 m_targetTick;

            WorkerThread m_thread;
        };
    }
}

QT_END_NAMESPACE

#endif // PHONON_MEDIAOBJECT_H
