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

#include "fakesource.h"
#include "iodevicereader.h"
#include "qaudiocdreader.h"

#include "mediagraph.h"
#include "mediaobject.h"


#include <QtCore/QUrl>
#include <QtCore/QDebug>

#include <qnetwork.h>


QT_BEGIN_NAMESPACE

namespace Phonon
{
    namespace DS9
    {
        //description of a connection
        struct GraphConnection
        {
            Filter output;
            int outputOffset;
            Filter input;
            int inputOffset;
        };

        static QList<GraphConnection> getConnections(Filter source)
        {
            QList<GraphConnection> ret;
            int outOffset = 0;
            const QList<OutputPin> outputs = BackendNode::pins(source, PINDIR_OUTPUT);
            for (int i = 0; i < outputs.count(); ++i) {
                InputPin input;
                if (outputs.at(i)->ConnectedTo(input.pparam()) == S_OK) {
                    PIN_INFO info;
                    input->QueryPinInfo(&info);
                    Filter current(info.pFilter);
                    if (current) {
                        //this is a valid connection
                        const int inOffset = BackendNode::pins(current, PINDIR_INPUT).indexOf(input);
                        const GraphConnection connection = {source, outOffset, current, inOffset};
                        ret += connection;
                        ret += getConnections(current); //get subsequent connections
                    }
                }
                outOffset++;
            }
            return ret;
        }
                
        static HRESULT saveToFile(Graph graph, const QString &filepath)
        {
            const WCHAR wszStreamName[] = L"ActiveMovieGraph";
            HRESULT hr;
            ComPointer<IStorage> storage;

            // First, create a document file that will hold the GRF file
            hr = StgCreateDocfile((OLECHAR*)filepath.utf16(),
                STGM_CREATE | STGM_TRANSACTED | STGM_READWRITE |
                STGM_SHARE_EXCLUSIVE,
                0, storage.pparam());

            if (FAILED(hr)) {
                return hr;
            }

            // Next, create a stream to store.
            ComPointer<IStream> stream;
            hr = storage->CreateStream(wszStreamName,
                STGM_WRITE | STGM_CREATE | STGM_SHARE_EXCLUSIVE,
                0, 0, stream.pparam());

            if (FAILED(hr)) {
                return hr;
            }

            // The IpersistStream::Save method converts a stream into a persistent object.
            ComPointer<IPersistStream> persist(graph, IID_IPersistStream);
            hr = persist->Save(stream, TRUE);
            if (SUCCEEDED(hr)) {
                hr = storage->Commit(STGC_DEFAULT);
            }

            return hr;
        }


        MediaGraph::MediaGraph(MediaObject *mo, short index) :
            m_graph(CLSID_FilterGraph, IID_IGraphBuilder),
            m_fakeSource(new FakeSource()),
            m_hasVideo(false), m_hasAudio(false), m_connectionsDirty(false), 
            m_isStopping(false), m_isSeekable(false), m_result(S_OK),
            m_index(index), m_renderId(0), m_seekId(0),
            m_currentTime(0), m_totalTime(0), m_mediaObject(mo)
        {
            m_mediaControl = ComPointer<IMediaControl>(m_graph, IID_IMediaControl);
            Q_ASSERT(m_mediaControl);
            m_mediaSeeking = ComPointer<IMediaSeeking>(m_graph, IID_IMediaSeeking);
            Q_ASSERT(m_mediaSeeking);

            HRESULT hr = m_graph->AddFilter(m_fakeSource, 0);
            if (m_mediaObject->catchComError(hr)) {
                return;
            }
        }

        MediaGraph::~MediaGraph()
        {
        }

        short MediaGraph::index() const
        {
            return m_index;
        }

        void MediaGraph::grabNode(BackendNode *node)
        {
            grabFilter(node->filter(m_index));
        }

        void MediaGraph::grabFilter(Filter filter)
        {
            if (filter) {
                FILTER_INFO info;
                filter->QueryFilterInfo(&info);
                if (info.pGraph != m_graph) {
                    if (info.pGraph) {
                        m_mediaObject->catchComError(info.pGraph->RemoveFilter(filter));
                    }
                    m_mediaObject->catchComError(m_graph->AddFilter(filter, 0));
                }
                if (info.pGraph) {
                    info.pGraph->Release();
                }
            }
        }

        void MediaGraph::switchFilters(Filter oldFilter, Filter newFilter)
        {
            OAFilterState state = syncGetRealState();
            if (state != State_Stopped) {
                ensureStopped(); //to do the transaction
            }


            OutputPin connected;
            {
                InputPin pin = BackendNode::pins(oldFilter, PINDIR_INPUT).first();
                pin->ConnectedTo(connected.pparam());
            }

            m_graph->RemoveFilter(oldFilter);
            m_graph->AddFilter(newFilter, 0);

            if (connected) {
                InputPin pin = BackendNode::pins(newFilter, PINDIR_INPUT).first();
                //let's reestablish the connections
                m_graph->Connect(connected, pin);
            }

            switch(state)
            {
            case State_Running:
                play();
                break;
            case State_Paused:
                pause();
                break;
            default:
                break;
            }

        }

        OAFilterState MediaGraph::syncGetRealState() const
        {
            OAFilterState state;
            m_mediaControl->GetState(INFINITE, &state);
            return state;
        }



        void MediaGraph::ensureSourceDisconnected()
        {
            for (int i = 0; i < m_sinkConnections.count(); ++i) {
                const Filter currentFilter = m_sinkConnections.at(i)->filter(m_index);
                const QList<InputPin> inputs = BackendNode::pins(currentFilter, PINDIR_INPUT);
                const QList<InputPin> outputs = BackendNode::pins(m_fakeSource, PINDIR_OUTPUT);

                for (int i = 0; i < inputs.count(); ++i) {
                    for (int o = 0; o < outputs.count(); o++) {
                        tryDisconnect(outputs.at(o), inputs.at(i));
                    }

                    for (int d = 0; d < m_decoderPins.count(); ++d) {
                        tryDisconnect(m_decoderPins.at(d), inputs.at(i));
                    }
                }
            }
        }

        void MediaGraph::ensureSourceConnectedTo(bool force)
        {
            if (m_connectionsDirty == false && force == false) {
                return;
            }

            m_connectionsDirty = false;
            ensureSourceDisconnected();

            //reconnect the pins
            for (int i = 0; i < m_sinkConnections.count(); ++i) {
                const Filter currentFilter = m_sinkConnections.at(i)->filter(m_index);
                const QList<InputPin> inputs = BackendNode::pins(currentFilter, PINDIR_INPUT);
                for(int i = 0; i < inputs.count(); ++i) {
                    //we ensure the filter belongs to the graph
                    grabFilter(currentFilter);

                    for (int d = 0; d < m_decoderPins.count(); ++d) {
                        //a decoder has only one output
                        if (tryConnect(m_decoderPins.at(d), inputs.at(i))) {
                            break;
                        }
                    }
                }
            }
        }

        QList<Filter> MediaGraph::getAllFilters(Graph graph)
        {
            QList<Filter> ret;
            ComPointer<IEnumFilters> enumFilters;
            graph->EnumFilters(enumFilters.pparam());
            Filter current;
            while( enumFilters && enumFilters->Next(1, current.pparam(), 0) == S_OK) {
                ret += current;
            }
            return ret;
        }

        QList<Filter> MediaGraph::getAllFilters() const
        {
            return getAllFilters(m_graph);
        }


        bool MediaGraph::isSeekable() const
        {
            return m_isSeekable;
        }

        qint64 MediaGraph::absoluteTotalTime() const
        {
            if (m_seekId) {
                return m_totalTime;
            } else {
                qint64 ret = 0;
                if (m_mediaSeeking) {
                    m_mediaSeeking->GetDuration(&ret);
                    ret /= 10000; //convert to milliseconds
                }
                return ret;
            }
        }

        qint64 MediaGraph::absoluteCurrentTime() const
        {
            if (m_seekId) {
                return m_currentTime;
            } else {
                qint64 ret = -1;
                if (m_mediaSeeking) {
                    HRESULT hr = m_mediaSeeking->GetCurrentPosition(&ret);
                    if (FAILED(hr)) {
                        return ret;
                    }
                    ret /= 10000; //convert to milliseconds
                }
                return ret;
            }
        }

        Phonon::MediaSource MediaGraph::mediaSource() const
        {
            return m_mediaSource;
        }

        void MediaGraph::play()
        {
            ensureSourceConnectedTo();
            m_mediaObject->workerThread()->addStateChangeRequest(m_graph, State_Running, m_decoders);
        }

        void MediaGraph::pause()
        {
            ensureSourceConnectedTo();
            m_mediaObject->workerThread()->addStateChangeRequest(m_graph, State_Paused, m_decoders);
        }

        HRESULT MediaGraph::renderResult() const
        {
            return m_result;
        }

        bool MediaGraph::isStopping() const
        {
            return m_isStopping;
        }

        Graph MediaGraph::graph() const
        {
            return m_graph;
        }

        void MediaGraph::stop()
        {
            if (!isLoading()) {
                ensureStopped();
                absoluteSeek(0); //resets the clock
			} else {
				m_mediaObject->workerThread()->abortCurrentRender(m_renderId);
     			m_renderId = 0; //cancels current loading
			}
            m_mediaObject->workerThread()->addStateChangeRequest(m_graph, State_Stopped);
        }

        void MediaGraph::ensureStopped()
        {
            m_isStopping = true;
            //special case here because we want stopped to be synchronous
            m_graph->Abort();
            m_mediaControl->Stop(); 
            OAFilterState dummy;
            //this will wait until the change is effective
            m_mediaControl->GetState(INFINITE, &dummy);
            m_isStopping = false;
        }

        bool MediaGraph::isLoading() const
        {
            return m_renderId != 0;
        }

        void MediaGraph::absoluteSeek(qint64 time)
        {
            //this just sends a request
            if (m_seekId == 0) {
                m_currentTime = absoluteCurrentTime();
                m_totalTime = absoluteTotalTime();
            }
            m_seekId = m_mediaObject->workerThread()->addSeekRequest(m_graph, time);
        }

        HRESULT MediaGraph::removeFilter(const Filter& filter)
        {
            FILTER_INFO info;
            filter->QueryFilterInfo(&info);
#ifdef GRAPH_DEBUG
            qDebug() << "removeFilter" << QString::fromUtf16(info.achName);
#endif
            if (info.pGraph) {
                info.pGraph->Release();
                return m_graph->RemoveFilter(filter);
            }

            //already removed
            return S_OK;
        }

        HRESULT MediaGraph::cleanup()
        {
            stop();

            ensureSourceDisconnected();

            QList<Filter> list = m_decoders;
            if (m_demux) {
                list << m_demux;
            }
            if (m_realSource) {
                list << m_realSource;
            }
            list << m_decoders;

            for (int i = 0; i < m_decoders.count(); ++i) {
                list += getFilterChain(m_demux, m_decoders.at(i));
            }

            for (int i = 0; i < list.count(); ++i) {
                removeFilter(list.at(i));
            }

            //Let's reinitialize the internal lists
            m_decoderPins.clear();
            m_decoders.clear();
            m_demux = Filter();
            m_realSource = Filter();
            m_mediaSource = Phonon::MediaSource();

            absoluteSeek(0); //resets the clock

            return S_OK;
        }


        bool MediaGraph::disconnectNodes(BackendNode *source, BackendNode *sink)
        {
            const Filter sinkFilter = sink->filter(m_index);
            const QList<InputPin> inputs = BackendNode::pins(sinkFilter, PINDIR_INPUT);

            QList<OutputPin> outputs;
            if (source == m_mediaObject) {
                outputs = BackendNode::pins(m_fakeSource, PINDIR_OUTPUT);
                outputs += m_decoderPins;
            } else {
                outputs = BackendNode::pins(source->filter(m_index), PINDIR_OUTPUT);
            }


            for (int i = 0; i < inputs.count(); ++i) {
                for (int o = 0; o < outputs.count(); ++o) {
                    tryDisconnect(outputs.at(o), inputs.at(i));
                }
            }

            if (m_sinkConnections.removeOne(sink)) {
                m_connectionsDirty = true;
            }
            return true;
        }

        bool MediaGraph::tryDisconnect(const OutputPin &out, const InputPin &in)
        {
            bool ret = false;

            OutputPin output;
            if (SUCCEEDED(in->ConnectedTo(output.pparam()))) {

                if (output == out) {
                    //we need a simple disconnection
                    ret = SUCCEEDED(out->Disconnect()) && SUCCEEDED(in->Disconnect());
                } else {
                    InputPin in2;
                    if (SUCCEEDED(out->ConnectedTo(in2.pparam()))) {
                        PIN_INFO info;
                        in2->QueryPinInfo(&info);
                        Filter tee(info.pFilter);
                        CLSID clsid;
                        tee->GetClassID(&clsid);
                        if (clsid == CLSID_InfTee) {
                            //we have to remove all intermediate filters between the tee and the sink
                            PIN_INFO info;
                            in->QueryPinInfo(&info);
                            Filter sink(info.pFilter);
                            QList<Filter> list = getFilterChain(tee, sink);
                            out->QueryPinInfo(&info);
                            Filter source(info.pFilter);

                            if (list.isEmpty()) {
                                output->QueryPinInfo(&info);
                                if (Filter(info.pFilter) == tee) {
                                    ret = SUCCEEDED(output->Disconnect()) && SUCCEEDED(in->Disconnect());
                                }
                            } else {
                                ret = true;
                                for (int i = 0; i < list.count(); ++i) {
                                    ret = ret && SUCCEEDED(removeFilter(list.at(i)));
                                }
                            }

                            //Let's try to see if the Tee filter is still useful
                            if (ret) {
                                int connections = 0;
                                const QList<OutputPin> outputs = BackendNode::pins(tee, PINDIR_OUTPUT);
                                for(int i = 0; i < outputs.count(); ++i) {
                                    InputPin p;
                                    if ( SUCCEEDED(outputs.at(i)->ConnectedTo(p.pparam()))) {
                                        connections++;
                                    }
                                }
                                if (connections == 0) {
                                    //this avoids a crash if the filter is destroyed
                                    //by the subsequent call to removeFilter
                                    output = OutputPin();
                                    removeFilter(tee); //there is no more output for the tee, we remove it
                                }
                            }
                        }
                    }
                }
            }
            return ret;
        }

        bool MediaGraph::tryConnect(const OutputPin &out, const InputPin &newIn)
        {


            ///The management of the creation of the Tees is done here (this is the only place where we call IPin::Connect
            InputPin inPin;
            if (SUCCEEDED(out->ConnectedTo(inPin.pparam()))) {

                //the fake source has another mechanism for the connection
                if (BackendNode::pins(m_fakeSource, PINDIR_OUTPUT).contains(out)) {
                    return false;
                }

                //the output pin is already connected
                PIN_INFO info;
                inPin->QueryPinInfo(&info);
                Filter filter(info.pFilter); //this will ensure the interface is "Release"d
                CLSID clsid;
                filter->GetClassID(&clsid);
                if (clsid == CLSID_InfTee) {
                    //there is already a Tee (namely 'filter') in use
                    const QList<OutputPin> outputs = BackendNode::pins(filter, PINDIR_OUTPUT);
                    for(int i = 0; i < outputs.count(); ++i) {
                        const OutputPin &pin = outputs.at(i);
                        if (VFW_E_NOT_CONNECTED == pin->ConnectedTo(inPin.pparam())) {
                            return SUCCEEDED(pin->Connect(newIn, 0));
                        }
                    }
                    //we shoud never go here
                    return false;
                } else {
                    QAMMediaType type;
                    out->ConnectionMediaType(&type);

                    //first we disconnect the current connection (and we save the current media type)
                    if (!tryDisconnect(out, inPin)) {
                        return false;
                    }

                    //..then we try to connect the new node
                    if (SUCCEEDED(out->Connect(newIn, 0))) {

                        //we have to insert the Tee
                        if (!tryDisconnect(out, newIn)) {
                            return false;
                        }

                        Filter filter(CLSID_InfTee, IID_IBaseFilter);
                        if (!filter) {
                            //rollback
                            m_graph->Connect(out, inPin);
                            return false;
                        }

                        if (FAILED(m_graph->AddFilter(filter, 0))) {
                            return false;
                        }


                        InputPin teeIn = BackendNode::pins(filter, PINDIR_INPUT).first(); //a Tee has always one input
                        HRESULT hr = out->Connect(teeIn, &type);
                        if (FAILED(hr)) {
                            hr = m_graph->Connect(out, teeIn);
                        }
                        if (FAILED(hr)) {
                            m_graph->Connect(out, inPin);
                            return false;
                        }

                        OutputPin teeOut = BackendNode::pins(filter, PINDIR_OUTPUT).last(); //the last is always the one that's not connected

                        //we simply reconnect the pins as they
                        hr = m_graph->Connect(teeOut, inPin);
                        if (FAILED(hr)) {
                            m_graph->Connect(out, inPin);
                            return false;
                        }

                        teeOut = BackendNode::pins(filter, PINDIR_OUTPUT).last(); //the last is always the one that's not connected
                        if (FAILED(m_graph->Connect(teeOut, newIn))) {
                            m_graph->Connect(out, inPin);
                            return false;
                        }

                        return true;
                    } else {
                        //we simply reconnect the pins as they
                        m_graph->Connect(out, inPin);
                        return false;
                    }
                }

            } else {
                return SUCCEEDED(m_graph->Connect(out, newIn));
            }
        }

        bool MediaGraph::connectNodes(BackendNode *source, BackendNode *sink)
        {
            bool ret = false;
            const QList<InputPin> inputs = BackendNode::pins(sink->filter(m_index), PINDIR_INPUT);
            QList<OutputPin> outputs = BackendNode::pins(source == m_mediaObject ? m_fakeSource : source->filter(m_index), PINDIR_OUTPUT);
            
            if (source == m_mediaObject) {
                grabFilter(m_fakeSource);
            }

#ifdef GRAPH_DEBUG
            qDebug() << Q_FUNC_INFO << source << sink << this;
#endif

            for (int o = 0; o < outputs.count(); o++) {
                InputPin p;
                for (int i = 0; i < inputs.count(); i++) {
                    const InputPin &inPin = inputs.at(i);
                    if (tryConnect(outputs.at(o), inPin)) {
                        //tell the sink node that it just got a new input
                        sink->connected(source, inPin);
                        ret = true;
                        if (source == m_mediaObject) {
                            m_connectionsDirty = true;
                            m_sinkConnections += sink;
#ifdef GRAPH_DEBUG
                            qDebug() << "found a sink connection" << sink << m_sinkConnections.count();
#endif
                        }
                        break;
                    }
                }
            }

            return ret;
        }


        HRESULT MediaGraph::loadSource(const Phonon::MediaSource &source)
        {
            m_hasVideo = false;
            m_hasAudio = false;
            m_isSeekable = false;


            //cleanup of the previous filters
            m_result = cleanup();
            if (FAILED(m_result)) {
                return m_result;
            }

            m_mediaSource = source;

            switch (source.type())
            {
            case Phonon::MediaSource::Disc:
                if (source.discType() == Phonon::Dvd) {
                    m_result = E_NOTIMPL;
                    /*m_realSource = Filter(CLSID_DVDNavigator, IID_IBaseFilter);
                    if (m_realSource) {
                        return REGDB_E_CLASSNOTREG;
                    }

                    m_result = m_graph->AddFilter(m_realSource, L"DVD Navigator");*/


 #ifndef QT_NO_PHONON_MEDIACONTROLLER
               } else if (source.discType() == Phonon::Cd) {
                    m_realSource = Filter(new QAudioCDPlayer);
                    m_result = m_graph->AddFilter(m_realSource, 0);

#endif //QT_NO_PHONON_MEDIACONTROLLER
                } else {
                    m_result = E_NOTIMPL;
                }
                if (FAILED(m_result)) {
                    return m_result;
                }
                m_renderId = m_mediaObject->workerThread()->addFilterToRender(m_realSource);
                return m_result;
            case Phonon::MediaSource::Invalid:
                return m_result;
            case Phonon::MediaSource::Url:
            case Phonon::MediaSource::LocalFile:
                {
                    QString url;
                    if (source.type() == Phonon::MediaSource::LocalFile) {
                        url = source.fileName();
                    } else {
                        url = source.url().toString();
                    }
                    m_renderId = m_mediaObject->workerThread()->addUrlToRender(url);
                }
                break;
#ifndef QT_NO_PHONON_ABSTRACTMEDIASTREAM
            case Phonon::MediaSource::Stream:
                {
                    m_realSource = Filter(new IODeviceReader(source, this));
                    m_renderId = m_mediaObject->workerThread()->addFilterToRender(m_realSource);
                }
                break;
#endif //QT_NO_PHONON_ABSTRACTMEDIASTREAM
            default:
                m_result = E_FAIL;
            }

            return m_result;
        }

        void MediaGraph::finishSeeking(quint16 workId, qint64 time)
        {
            if (m_seekId == workId) {
                m_currentTime = time;
                m_mediaObject->seekingFinished(this);
                m_seekId = 0;
            } else {
                //it's a queue seek command
                //we're still seeking
            }
        }

        void MediaGraph::finishLoading(quint16 workId, HRESULT hr, Graph graph)
        {
            if (m_renderId == workId) {
                m_renderId = 0;

				//let's determine if the graph is seekable
				{
					ComPointer<IMediaSeeking> mediaSeeking(graph, IID_IMediaSeeking);
					DWORD caps = AM_SEEKING_CanSeekAbsolute;
                    m_isSeekable = mediaSeeking && SUCCEEDED(mediaSeeking->CheckCapabilities(&caps));
				}

                m_result = reallyFinishLoading(hr, graph);
                m_mediaObject->loadingFinished(this);
            }
        }


        HRESULT MediaGraph::reallyFinishLoading(HRESULT hr, const Graph &graph)
        {
            if (FAILED(hr)) {
                return hr;
            }

            const Graph oldGraph = m_graph;
            m_graph = graph;

            //we keep the source and all the way down to the decoders
            QList<Filter> removedFilters;

			const QList<Filter> allFilters = getAllFilters(graph);
            for (int i = 0; i < allFilters.count(); ++i) {
                const Filter &filter = allFilters.at(i);
                if (isSourceFilter(filter)) {
                    m_realSource = filter; //save the source filter
                    if (!m_demux ) {
                        m_demux = filter; //in the WMV case, the demuxer is the source filter itself
                    }
                } else if (isDemuxerFilter(filter)) {
                    m_demux = filter;
                } else if (isDecoderFilter(filter)) {
                    m_decoders += filter;
                    m_decoderPins += BackendNode::pins(filter, PINDIR_OUTPUT).first();
                }  else {
                    removedFilters += filter;
                }
            }

            for (int i = 0; i < m_decoders.count(); ++i) {
                QList<Filter> chain = getFilterChain(m_demux, m_decoders.at(i));
                for (int i = 0; i < chain.count(); ++i) {
                    //we keep those filters
                    removedFilters.removeOne(chain.at(i));
                }
            }

            for (int i = 0; i < removedFilters.count(); ++i) {
                graph->RemoveFilter(removedFilters.at(i));
            }

            m_mediaObject->workerThread()->replaceGraphForEventManagement(graph, oldGraph);

            //let's transfer the nodes from the current graph to the new one
            QList<GraphConnection> connections; //we store the connections that need to be restored

            // First get all the sink nodes (nodes with no input connected)
            for (int i = 0; i < m_sinkConnections.count(); ++i) {
                Filter currentFilter = m_sinkConnections.at(i)->filter(m_index);
                connections += getConnections(currentFilter);
                grabFilter(currentFilter);
            }

            //we need to do something smart to detect if the streams are unencoded
            if (m_demux) {
                const QList<OutputPin> outputs = BackendNode::pins(m_demux, PINDIR_OUTPUT);
                for (int i = 0; i < outputs.count(); ++i) {
                    const OutputPin &out = outputs.at(i);
                    InputPin pin;
                    if (out->ConnectedTo(pin.pparam()) == VFW_E_NOT_CONNECTED) {
                        m_decoderPins += out; //unconnected outputs can be decoded outputs
                    }
                }
            }

            ensureSourceConnectedTo(true);

            //let's reestablish the connections
            for (int i = 0; i < connections.count(); ++i) {
                const GraphConnection &connection = connections.at(i);
                //check if we shoud transfer the sink node

                grabFilter(connection.input);
                grabFilter(connection.output);

                const OutputPin output = BackendNode::pins(connection.output, PINDIR_OUTPUT).at(connection.outputOffset);
                const InputPin input   = BackendNode::pins(connection.input, PINDIR_INPUT).at(connection.inputOffset);
                HRESULT hr = output->Connect(input, 0);
                Q_UNUSED(hr);
                Q_ASSERT( SUCCEEDED(hr));
            }

            //Finally, let's update the interfaces
            m_mediaControl = ComPointer<IMediaControl>(graph, IID_IMediaControl);
            m_mediaSeeking = ComPointer<IMediaSeeking>(graph, IID_IMediaSeeking);
            return hr;
        }

        //utility functions
        //retrieves the filters between source and sink
        QList<Filter> MediaGraph::getFilterChain(const Filter &source, const Filter &sink)
        {
            QList<Filter> ret;
            Filter current = sink;
            while (current && BackendNode::pins(current, PINDIR_INPUT).count() == 1 && current != source) {
                if (current != source)
                    ret += current;
                InputPin pin = BackendNode::pins(current, PINDIR_INPUT).first();
                current = Filter();
                OutputPin output;
                if (pin->ConnectedTo(output.pparam()) == S_OK) {
                    PIN_INFO info;
                    if (SUCCEEDED(output->QueryPinInfo(&info)) && info.pFilter) {
                        current = Filter(info.pFilter); //this will take care of releasing the interface pFilter
                    }
                }
            }
            if (current != source) {
                //the soruce and sink don't seem to be connected
                ret.clear();
            }
            return ret;
        }

        bool MediaGraph::isDecoderFilter(const Filter &filter)
        {
            if (filter == 0) {
                return false;
            }
#ifdef GRAPH_DEBUG
            {
                FILTER_INFO info;
                filter->QueryFilterInfo(&info);
                qDebug() << Q_FUNC_INFO << QString::fromUtf16(info.achName);
                if (info.pGraph) {
                    info.pGraph->Release();
                }
            }
#endif


            QList<InputPin> inputs = BackendNode::pins(filter, PINDIR_INPUT);
            QList<OutputPin> outputs = BackendNode::pins(filter, PINDIR_OUTPUT);

            //TODO: find a better way to detect if a node is a decoder
            if (inputs.count() == 0 || outputs.count() ==0) {
                return false;
            }

            //the input pin must be encoded data
            QAMMediaType type;
            HRESULT hr = inputs.first()->ConnectionMediaType(&type);
            if (FAILED(hr)) {
                return false;
            }


            //...and the output must be decoded
            QAMMediaType type2;
            hr = outputs.first()->ConnectionMediaType(&type2);
            if (FAILED(hr)) {
                return false;
            }

            if (type2.majortype != MEDIATYPE_Video &&
                type2.majortype != MEDIATYPE_Audio) {
                    return false;
            }

            if (type2.majortype == MEDIATYPE_Video) {
                m_hasVideo = true;
            } else {
                m_hasAudio = true;
            }

#ifdef GRAPH_DEBUG
            {
                FILTER_INFO info;
                filter->QueryFilterInfo(&info);
                qDebug() << "found a decoder filter" << QString::fromUtf16(info.achName);
                if (info.pGraph) {
                    info.pGraph->Release();
                }
            }
#endif

            return true;
        }

        bool MediaGraph::isSourceFilter(const Filter &filter) const
        {
#ifdef GRAPH_DEBUG
            {
                FILTER_INFO info;
                filter->QueryFilterInfo(&info);
                qDebug() << Q_FUNC_INFO << QString::fromUtf16(info.achName);
                if (info.pGraph) {
                    info.pGraph->Release();
                }
            }
#endif
            //a source filter is one that has no input
            return BackendNode::pins(filter, PINDIR_INPUT).isEmpty();
        }

        bool MediaGraph::isDemuxerFilter(const Filter &filter) const
        {
            QList<InputPin> inputs = BackendNode::pins(filter, PINDIR_INPUT);
            QList<OutputPin> outputs = BackendNode::pins(filter, PINDIR_OUTPUT);

#ifdef GRAPH_DEBUG
            {
                FILTER_INFO info;
                filter->QueryFilterInfo(&info);
                qDebug() << Q_FUNC_INFO << QString::fromUtf16(info.achName);
                if (info.pGraph) {
                    info.pGraph->Release();
                }
            }
#endif

            if (inputs.count() != 1 || outputs.count() == 0) {
                return false; //a demuxer has only one input
            }

            QAMMediaType type;
            HRESULT hr = inputs.first()->ConnectionMediaType(&type);
            if (FAILED(hr)) {
                return false;
            }

            if (type.majortype != MEDIATYPE_Stream) {
                return false;
            }

            for (int i = 0; i < outputs.count(); ++i) {
                QAMMediaType type;
                //for now we support only video and audio
                hr = outputs.at(i)->ConnectionMediaType(&type);
                if (SUCCEEDED(hr) && 
                    type.majortype != MEDIATYPE_Video && type.majortype != MEDIATYPE_Audio) {
                        return false;
                }
            }
#ifdef GRAPH_DEBUG
            {
                FILTER_INFO info;
                filter->QueryFilterInfo(&info);
                qDebug() << "found a demuxer filter" << QString::fromUtf16(info.achName);
                if (info.pGraph) {
                    info.pGraph->Release();
                }
            }
#endif
            return true;
        }

        QMultiMap<QString, QString> MediaGraph::metadata() const
        {
            QMultiMap<QString, QString> ret;
            ComPointer<IAMMediaContent> mediaContent(m_demux, IID_IAMMediaContent);
            if (mediaContent) {
                //let's get the meta data
                BSTR str;
                HRESULT hr = mediaContent->get_AuthorName(&str);
                if (SUCCEEDED(hr)) {
                    ret.insert(QLatin1String("ARTIST"), QString::fromUtf16((const unsigned short*)str));
                    SysFreeString(str);
                }
                hr = mediaContent->get_Title(&str);
                if (SUCCEEDED(hr)) {
                    ret.insert(QLatin1String("TITLE"), QString::fromUtf16((const unsigned short*)str));
                    SysFreeString(str);
                }
                hr = mediaContent->get_Description(&str);
                if (SUCCEEDED(hr)) {
                    ret.insert(QLatin1String("DESCRIPTION"), QString::fromUtf16((const unsigned short*)str));
                    SysFreeString(str);
                }
                hr = mediaContent->get_Copyright(&str);
                if (SUCCEEDED(hr)) {
                    ret.insert(QLatin1String("COPYRIGHT"), QString::fromUtf16((const unsigned short*)str));
                    SysFreeString(str);
                }
                hr = mediaContent->get_MoreInfoText(&str);
                if (SUCCEEDED(hr)) {
                    ret.insert(QLatin1String("MOREINFO"), QString::fromUtf16((const unsigned short*)str));
                    SysFreeString(str);
                }
            }
            return ret;
        }

        Filter MediaGraph::realSource() const
        {
            return m_realSource;
        }

#ifndef QT_NO_PHONON_MEDIACONTROLLER
        void MediaGraph::setStopPosition(qint64 time)
        {
            qint64 current = 0,
                stop = 0;
            m_mediaSeeking->GetPositions(&current, &stop);

            const bool shouldSeek = current == stop;

            if (time == -1) {
                HRESULT hr = m_mediaSeeking->GetDuration(&time);
                if (FAILED(hr)) {
                    return;
                }
            } else {
                time *= 10000;
            }

            if (time == stop) {
                //the stop position is already at the right place
                return;
            }

            if (shouldSeek) {
                m_mediaSeeking->SetPositions(&current, AM_SEEKING_AbsolutePositioning, 
                    &time, AM_SEEKING_AbsolutePositioning);
            } else {
                m_mediaSeeking->SetPositions(0, AM_SEEKING_NoPositioning, 
                    &time, AM_SEEKING_AbsolutePositioning);
            }
        }

        qint64 MediaGraph::stopPosition() const
        {
            qint64 ret;
            m_mediaSeeking->GetStopPosition(&ret);
            return ret / 10000;

        }

        QList<qint64> MediaGraph::titles() const
        {
            //for now we only manage that for the audio cd
            ComPointer<ITitleInterface> titleIFace(m_realSource, IID_ITitleInterface);
            if (titleIFace) {
                return titleIFace->titles();
            } else {
                // the default value: only one title that starts at position 0
                return QList<qint64>() << 0;
            }
        }
#endif //QT_NO_PHONON_MEDIACONTROLLER



    }
}

QT_END_NAMESPACE
