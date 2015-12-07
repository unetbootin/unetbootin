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

#include "fakesource.h"
#include "iodevicereader.h"
#include "qaudiocdreader.h"

#include "mediagraph.h"
#include "mediaobject.h"


#include <QtCore/QUrl>

#include <qnetwork.h>


QT_BEGIN_NAMESPACE

uint qHash (const Phonon::DS9::Filter &f)
{
    return uint(static_cast<IBaseFilter*>(f));
}

uint qHash (const Phonon::DS9::OutputPin &p)
{
    return uint(static_cast<IPin*>(p));
}

namespace Phonon
{
    namespace DS9
    {

        class QAMMediaType : public AM_MEDIA_TYPE
        {
        public:
            ~QAMMediaType() 
            {
                QPin::freeMediaType(*this);
            }

        };

        MediaGraph::MediaGraph(MediaObject *mo, short index) : m_fakeSource(new FakeSource()),
            m_hasVideo(false), m_hasAudio(false), m_connectionsDirty(false), 
            m_isStopping(false), m_isSeekable(false), m_result(S_OK),
            m_index(index), m_mediaObject(mo), 
            m_graph(CLSID_FilterGraph, IID_IGraphBuilder),
            m_renderId(0), m_seekId(0), m_currentTime(0)
        {
            m_mediaControl = ComPointer<IMediaControl>(m_graph, IID_IMediaControl);
            Q_ASSERT(m_mediaControl);
            m_mediaSeeking = ComPointer<IMediaSeeking>(m_graph, IID_IMediaSeeking);
            Q_ASSERT(m_mediaSeeking);

            m_mediaObject->workerThread()->addGraphForEventManagement(m_graph);

            HRESULT hr = m_graph->AddFilter(m_fakeSource, L"Fake Source");
            if (m_mediaObject->catchComError(hr)) {
                return;
            }

            connect(mo->workerThread(), SIGNAL(asyncRenderFinished(quint16, HRESULT, QSet<Filter>, bool)),
                SLOT(finishLoading(quint16, HRESULT, QSet<Filter>, bool)));

            connect(mo->workerThread(), SIGNAL(asyncSeekingFinished(quint16, qint64)),
                SLOT(finishSeeking(quint16, qint64)));
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
            FILTER_INFO info;
            const Filter filter = node->filter(m_index);
            if (filter) {
                filter->QueryFilterInfo(&info);
                if (info.pGraph == 0) {
                    HRESULT hr = m_graph->AddFilter(filter, 0);
                    m_mediaObject->catchComError(hr);
                } else {
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
                pin->ConnectedTo(&connected);
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
            foreach(BackendNode *node, m_sinkConnections) {
                const Filter currentFilter = node->filter(m_index);
                foreach(const InputPin inpin, BackendNode::pins(currentFilter, PINDIR_INPUT)) {
                    foreach(const OutputPin outpin, BackendNode::pins(m_fakeSource, PINDIR_OUTPUT)) {
                        tryDisconnect(outpin, inpin);
                    }

                    foreach(const OutputPin outpin, m_decoderPins) {
                        tryDisconnect(outpin, inpin);
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
            foreach(BackendNode *node, m_sinkConnections) {
                const Filter currentFilter = node->filter(m_index);
                foreach(const InputPin &inpin, BackendNode::pins(currentFilter, PINDIR_INPUT)) {
                    //we ensure the filter belongs to the graph
                    FILTER_INFO info;
                    currentFilter->QueryFilterInfo(&info);
                    if (info.pGraph == 0) {
                        m_graph->AddFilter(currentFilter, 0);
                    } else {
                        info.pGraph->Release();
                    }

                    foreach(const OutputPin outpin, m_decoderPins) {
                        //a decoder has only one output
                        if (tryConnect(outpin, inpin)) {
                            break;
                        }
                    }
                }
            }
        }

        QSet<Filter> MediaGraph::getAllFilters(Graph graph)
        {
            QSet<Filter> ret;
            ComPointer<IEnumFilters> enumFilters;
            graph->EnumFilters(&enumFilters);
            Filter current;
            while( enumFilters && enumFilters->Next(1, &current, 0) == S_OK) {
                ret += current;
            }
            return ret;
        }

        QSet<Filter> MediaGraph::getAllFilters() const
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
            m_graph->Abort();
            m_mediaControl->Stop(); //special case here because we want stopped to be synchronous
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
            qDebug() << "removeFilter" << QString::fromWCharArray(info.achName);
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

            QSet<Filter> list = m_decoders;
            if (m_demux) {
                list << m_demux;
            }
            if (m_realSource) {
                list << m_realSource;
            }

            foreach(const Filter &decoder, m_decoders) {
                list += getFilterChain(m_demux, decoder);
            }

            foreach(const Filter &filter, list) {
                HRESULT hr = removeFilter(filter);
                if(FAILED(hr)) {
                    return hr;
                }
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

            QSet<OutputPin> outputs;
            if (source == m_mediaObject) {
                outputs = BackendNode::pins(m_fakeSource, PINDIR_OUTPUT).toSet();
                outputs += m_decoderPins;
            } else {
                outputs = BackendNode::pins(source->filter(m_index), PINDIR_OUTPUT).toSet();
            }


            foreach(InputPin inPin, inputs) {
                foreach(OutputPin outPin, outputs) {
                    tryDisconnect(outPin, inPin);
                }
            }

            if (m_sinkConnections.remove(sink)) {
                m_connectionsDirty = true;
            }
            return true;
        }

        bool MediaGraph::tryDisconnect(const OutputPin &out, const InputPin &in)
        {
            bool ret = false;

            OutputPin output;
            if (SUCCEEDED(in->ConnectedTo(&output))) {

                if (output == out) {
                    //we need a simple disconnection
                    ret = SUCCEEDED(out->Disconnect()) && SUCCEEDED(in->Disconnect());
                } else {
                    InputPin in2;
                    if (SUCCEEDED(out->ConnectedTo(&in2))) {
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
                            QSet<Filter> list = getFilterChain(tee, sink);
                            list -= sink;
                            list -= tee;
                            out->QueryPinInfo(&info);
                            Filter source(info.pFilter);

                            if (list.isEmpty()) {
                                output->QueryPinInfo(&info);
                                if (Filter(info.pFilter) == tee) {
                                    ret = SUCCEEDED(output->Disconnect()) && SUCCEEDED(in->Disconnect());
                                }
                            } else {
                                ret = true;
                                foreach(Filter f, list) {
                                    ret = ret && SUCCEEDED(removeFilter(f));
                                }
                            }

                            //Let's try to see if the Tee filter is still useful
                            if (ret) {
                                int connections = 0;
                                foreach (OutputPin out, BackendNode::pins(tee, PINDIR_OUTPUT)) {
                                    InputPin p;
                                    if ( SUCCEEDED(out->ConnectedTo(&p))) {
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
            if (SUCCEEDED(out->ConnectedTo(&inPin))) {

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
                    foreach(OutputPin pin, BackendNode::pins(filter, PINDIR_OUTPUT)) {
                        if (VFW_E_NOT_CONNECTED == pin->ConnectedTo(&inPin)) {
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
            const QList<OutputPin> outputs = BackendNode::pins(source == m_mediaObject ? m_fakeSource : source->filter(m_index), PINDIR_OUTPUT);

#ifdef GRAPH_DEBUG
            qDebug() << Q_FUNC_INFO << source << sink << this;
#endif

            foreach(OutputPin outPin, outputs) {

                InputPin p;
                foreach(InputPin inPin, inputs) {
                    if (tryConnect(outPin, inPin)) {
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


                } else if (source.discType() == Phonon::Cd) {
                    m_realSource = Filter(new QAudioCDPlayer);
                    m_result = m_graph->AddFilter(m_realSource, L"Audio CD Reader");

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
            case Phonon::MediaSource::Stream:
                {
                    m_realSource = Filter(new IODeviceReader(source, this));
                    m_result = m_graph->AddFilter(m_realSource, L"Phonon Stream Reader");

                    if (FAILED(m_result)) {
                        return m_result;
                    }

                    m_renderId = m_mediaObject->workerThread()->addFilterToRender(m_realSource);
                }
                break;
            }

            return m_result;
        }

        void MediaGraph::finishSeeking(quint16 workId, qint64 time)
        {
            if (m_seekId == workId) {
                m_currentTime = time;
                emit seekingFinished(this);
                m_seekId = 0;
            } else {
                //it's a queue seek command
                //we're still seeking
            }
        }

        void MediaGraph::finishLoading(quint16 workId, HRESULT hr, QSet<Filter> newlyCreated, bool isSeekable)
        {
            if (m_renderId == workId) {
                m_renderId = 0;
                m_isSeekable = isSeekable;
                m_result = reallyFinishLoading(hr, newlyCreated);
                emit loadingFinished(this);
            }
        }


        HRESULT MediaGraph::reallyFinishLoading(HRESULT hr, const QSet<Filter> &newlyCreated)
        {
            if (FAILED(hr)) {
                return hr;
            }

            //we keep the source and all the way down to the decoders
            QSet<Filter> keptFilters;

            foreach(const Filter &filter, newlyCreated) {
                //let's add all the filters
                hr = m_graph->AddFilter(filter, 0);
                if (FAILED(hr)) {
                    return hr;
                }

                if (isSourceFilter(filter)) {
                    m_realSource = filter; //save the source filter
                    if (!m_demux ) {
                        m_demux = filter; //in the WMV case, the demuxer is the source filter itself
                    }
                    keptFilters << filter;
                } else if (isDemuxerFilter(filter)) {
                    m_demux = filter;
                    keptFilters << filter;
                } else if (isDecoderFilter(filter)) {
                    m_decoders += filter;
                    m_decoderPins += BackendNode::pins(filter, PINDIR_OUTPUT).first();
                    keptFilters << filter;
                } 
            }

            foreach(const Filter &decoder, m_decoders) {
                keptFilters += getFilterChain(m_demux, decoder);
            }

            //now we need to decide what are the filters we want to keep
            //usually these are the demuxer and the decoders
            //the ones that are kept are added to the graph

            foreach(const Filter &filter, newlyCreated - keptFilters) {
                //this will also remove the connections to the 'dead" filters
                hr = m_graph->RemoveFilter(filter);
                if (FAILED(hr)) {
                    return hr;
                }
            }

            //we need to do something smart to detect if the streams are unencoded
            if (m_demux) {
                foreach(const OutputPin out, BackendNode::pins(m_demux, PINDIR_OUTPUT)) {
                    InputPin pin;
                    if (out->ConnectedTo(&pin) == VFW_E_NOT_CONNECTED) {
                        m_decoderPins += out; //unconnected outputs can be decoded outputs
                    }
                }
            }

            ensureSourceConnectedTo(true);
            return hr;
        }

        //utility functions
        QSet<Filter> MediaGraph::getFilterChain(const Filter &source, const Filter &sink)
        {
            QSet<Filter> ret;
            Filter current = sink;
            while (current && BackendNode::pins(current, PINDIR_INPUT).count() == 1 && current != source) {
                ret += current;
                InputPin pin = BackendNode::pins(current, PINDIR_INPUT).first();
                current = Filter();
                OutputPin output;
                if (pin->ConnectedTo(&output) == S_OK) {
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
                qDebug() << Q_FUNC_INFO << QString::fromWCharArray(info.achName);
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
                qDebug() << "found a decoder filter" << QString::fromWCharArray(info.achName);
                if (info.pGraph) {
                    info.pGraph->Release();
                }
            }
#endif

            return true;
        }

        bool MediaGraph::isSourceFilter(const Filter &filter) const
        {
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
                qDebug() << Q_FUNC_INFO << QString::fromWCharArray(info.achName);
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

            foreach(const OutputPin &outpin, outputs) {
                QAMMediaType type;
                //for now we support only video and audio
                hr = outpin->ConnectionMediaType(&type);
                if (SUCCEEDED(hr) && 
                    type.majortype != MEDIATYPE_Video && type.majortype != MEDIATYPE_Audio) {
                        return false;
                }
            }
#ifdef GRAPH_DEBUG
            {
                FILTER_INFO info;
                filter->QueryFilterInfo(&info);
                qDebug() << "found a demuxer filter" << QString::fromWCharArray(info.achName);
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
                    ret.insert(QLatin1String("ARTIST"), QString::fromWCharArray(str));
                    SysFreeString(str);
                }
                hr = mediaContent->get_Title(&str);
                if (SUCCEEDED(hr)) {
                    ret.insert(QLatin1String("TITLE"), QString::fromWCharArray(str));
                    SysFreeString(str);
                }
                hr = mediaContent->get_Description(&str);
                if (SUCCEEDED(hr)) {
                    ret.insert(QLatin1String("DESCRIPTION"), QString::fromWCharArray(str));
                    SysFreeString(str);
                }
                hr = mediaContent->get_Copyright(&str);
                if (SUCCEEDED(hr)) {
                    ret.insert(QLatin1String("COPYRIGHT"), QString::fromWCharArray(str));
                    SysFreeString(str);
                }
                hr = mediaContent->get_MoreInfoText(&str);
                if (SUCCEEDED(hr)) {
                    ret.insert(QLatin1String("MOREINFO"), QString::fromWCharArray(str));
                    SysFreeString(str);
                }
            }
            return ret;
        }

        Filter MediaGraph::realSource() const
        {
            return m_realSource;
        }

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



        HRESULT MediaGraph::saveToFile(const QString &filepath) const
        {
            const WCHAR wszStreamName[] = L"ActiveMovieGraph";
            HRESULT hr;
            ComPointer<IStorage> storage;

            // First, create a document file that will hold the GRF file
            hr = StgCreateDocfile(reinterpret_cast<const wchar_t *>(filepath.utf16()),
                STGM_CREATE | STGM_TRANSACTED | STGM_READWRITE |
                STGM_SHARE_EXCLUSIVE,
                0, &storage);

            if (FAILED(hr)) {
                return hr;
            }

            // Next, create a stream to store.
            ComPointer<IStream> stream;
            hr = storage->CreateStream(wszStreamName,
                STGM_WRITE | STGM_CREATE | STGM_SHARE_EXCLUSIVE,
                0, 0, &stream);

            if (FAILED(hr)) {
                return hr;
            }

            // The IpersistStream::Save method converts a stream into a persistent object.
            ComPointer<IPersistStream> persist(m_graph, IID_IPersistStream);
            hr = persist->Save(stream, TRUE);
            if (SUCCEEDED(hr)) {
                hr = storage->Commit(STGC_DEFAULT);
            }

            return hr;
        }

    }
}

QT_END_NAMESPACE

#include "moc_mediagraph.cpp"
