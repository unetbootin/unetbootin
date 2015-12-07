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

#include "mediaobject.h"
#include "audiooutput.h"

#include <QtCore/QVector>
#include <QtCore/QTimerEvent>
#include <QtCore/QTimer>
#include <QtCore/QTime>
#include <QtCore/QLibrary>
#include <QtCore/QUrl>
#include <QtCore/QWriteLocker>

#include <phonon/streaminterface.h>


#define WAVEHEADER_OFFSET_FORMATTAG        20
#define WAVEHEADER_OFFSET_CHANNELS         22
#define WAVEHEADER_OFFSET_SAMPLESPERSEC    24
#define WAVEHEADER_OFFSET_AVGBYTESPERSEC   28
#define WAVEHEADER_OFFSET_BLOCKALIGN       32
#define WAVEHEADER_OFFSET_BITSPERSAMPLE    34
#define WAVEHEADER_OFFSET_DATA             44
#define WAVEHEADER_SIZE                    WAVEHEADER_OFFSET_DATA

QT_BEGIN_NAMESPACE

namespace Phonon
{
    namespace WaveOut
    {        
        static unsigned int buffer_size = (16 * 1024 * 4);

        QString getErrorText(MMRESULT error)
        {
            ushort b[256];
            waveOutGetErrorText(error, (LPWSTR)b, 256);
            return QString::fromUtf16(b);
        }

        class WorkerThread : public QThread
        {
         Q_OBJECT
         public slots:
              void stream(QIODevice *file, QByteArray *buffer, bool *finished);
        };

        void WorkerThread::stream(QIODevice *ioStream, QByteArray *buffer, bool *finished)
        {
            (*finished) = false;
            memset((void*) buffer->data(), 0, buffer->size());
            qint64 i = ioStream->read(buffer->data(), buffer_size);
            buffer->resize(i);
            (*finished) = true;
        }


        void CALLBACK MediaObject::WaveOutCallBack(HWAVEOUT m_hWaveOut, UINT uMsg, DWORD dwInstance, DWORD dwParam1, DWORD dwParam2)
        {
            Q_UNUSED(m_hWaveOut);
            Q_UNUSED(dwInstance);
            Q_UNUSED(dwParam2);

            switch(uMsg)
            {
            case WOM_OPEN:
                break;
            case WOM_DONE:
                {
                    WAVEHDR *waveHeader = (WAVEHDR*)dwParam1;
                    MediaObject* mediaObject = reinterpret_cast<MediaObject *>(waveHeader->dwUser);
                    if (mediaObject) {
                        mediaObject->swapBuffers();
                    }
                }
                break;
            case WOM_CLOSE:
                break;
            }
        }

        class StreamReader : public Phonon::StreamInterface
        {
        public:
              StreamReader(QObject *parent, const Phonon::MediaSource &source) :
                  m_seekable(false), m_pos(0), m_size(-1)
              {
                  Q_UNUSED(parent);
                  connectToSource(source);
              }

              //for Phonon::StreamInterface
              void writeData(const QByteArray &data)
              {
                  QWriteLocker locker(&m_lock);
                  m_pos += data.size();
                  m_buffer += data;
              }

              void endOfData()
              {
              }

              void setStreamSize(qint64 newSize)
              {
                  QWriteLocker locker(&m_lock);
                  m_size = newSize;
              }

              qint64 streamSize() const
              {
                  QReadLocker locker(&m_lock);
                  return m_size;
              }

              void setStreamSeekable(bool s)
              {
                  QWriteLocker locker(&m_lock);
                  m_seekable = s;
              }

              bool streamSeekable() const
              {
                  QReadLocker locker(&m_lock);
                  return m_seekable;
              }

              void setCurrentPos(qint64 pos)
              {
                  QWriteLocker locker(&m_lock);
                  m_pos = pos;
                  seekStream(pos);
                  m_buffer.clear();
              }

              qint64 currentPos() const
              {
                  QReadLocker locker(&m_lock);
                  return m_pos;
              }

              int currentBufferSize() const
              {
                  QReadLocker locker(&m_lock);
                  return m_buffer.size();
              }

            //for Phonon::StreamInterface
            QByteArray m_buffer;
            bool m_seekable;
            qint64 m_pos;
            qint64 m_size;
            mutable QReadWriteLock m_lock;
        };

        class IOWrapper : public QIODevice {
        public:
            IOWrapper(QObject *parent, const Phonon::MediaSource &source) : m_streamReader(this, source)
            {
                Q_UNUSED(parent);
                setOpenMode(QIODevice::ReadOnly);
            }
            bool seek(qint64 pos);
            qint64 size() const;
            qint64 pos();
            bool isReadable() const;
         protected:
            qint64 readData (char * data, qint64 maxSize);
            qint64 writeData(const char *,qint64);
        private:
            StreamReader m_streamReader;
        };

        bool IOWrapper::isReadable () const
        {
            return true;
        }

        qint64 IOWrapper::pos()
        {
            return (m_streamReader.streamSeekable() ? m_streamReader.currentPos() : 0);
        }

        bool IOWrapper::seek( qint64 pos)
        {
            if (!m_streamReader.streamSeekable())
                return false;
            m_streamReader.setCurrentPos(pos);
            return true;
        }

        qint64 IOWrapper::size() const
        {
            return m_streamReader.streamSize();
        }

        qint64 IOWrapper::readData(char * data, qint64 maxSize)
        {
            int oldSize = m_streamReader.currentBufferSize();
            while (m_streamReader.currentBufferSize() < maxSize) {
                m_streamReader.needData();
                if (oldSize == m_streamReader.currentBufferSize()) {
                    break; //we didn't get any data
                }
                oldSize = m_streamReader.currentBufferSize();
            }

            qint64 bytesRead = qMin(qint64(m_streamReader.currentBufferSize()), maxSize);
            {
                QWriteLocker locker(&m_streamReader.m_lock);
                qMemCopy(data, m_streamReader.m_buffer.data(), bytesRead);
                //truncate the buffer
                m_streamReader.m_buffer = m_streamReader.m_buffer.mid(bytesRead);
            }
            return bytesRead;
        }

        qint64 IOWrapper::writeData(const char *,qint64)
        {
            return 0;
        }

        MediaObject::MediaObject(QObject *parent) : m_file(0), m_stream(0),
                                                    m_hWaveOut(0), m_nextBufferIndex(1), 
                                                    m_mediaSize(-1), m_bufferingFinished(0),
                                                    m_paused(0), m_tickInterval(0),
                                                    m_hasNextSource(0), m_hasSource(0),
                                                    m_sourceIsValid(0), m_errorType(Phonon::NoError),
                                                    m_currentTime(0), m_transitionTime(0),
                                                    m_tick(0), m_volume(100), m_prefinishMark(0),
                                                    m_tickIntervalResolution(0), m_bufferPrepared(0),
                                                    m_stopped(0)
        {
            m_thread = new WorkerThread();
            connect(this, SIGNAL(outOfData(QIODevice*, QByteArray*, bool*)), m_thread, SLOT(stream(QIODevice*, QByteArray*, bool*)));
            m_thread->start();
            m_soundBuffer1.waveHeader = new WAVEHDR;
            m_soundBuffer2.waveHeader = new WAVEHDR;
            setParent(parent);
            setState(Phonon::LoadingState);            
        }

        MediaObject::~MediaObject()
        {
            stop();
            disconnect(this, SIGNAL(outOfData(QIODevice*, QByteArray*, bool*)), m_thread, SLOT(stream(QIODevice*, QByteArray*, bool*)));
            do { //The event loop of m_thread might not be started, yet
                m_thread->quit(); //If the event loop is not started yet quit() does nothing
                m_thread->wait(100);
            } while (m_thread->isRunning());
            delete m_thread;
            deleteValidWaveOutDevice();
            delete m_soundBuffer1.waveHeader;
            delete m_soundBuffer2.waveHeader;
        }

        Phonon::State MediaObject::state() const
        {
           return m_state;
        }

        bool MediaObject::hasVideo() const
        {
            return false;
        }

        bool MediaObject::isSeekable() const
        {
            return !m_stream->isSequential();
        }

        qint64 MediaObject::totalTime() const
        {
            return m_totalTime;
        }

        qint64 MediaObject::currentTime() const
        {
            //this handles inaccuracy when stopping on a title
            return m_currentTime;
        }

        qint32 MediaObject::tickInterval() const
        {
            return m_tickInterval * m_tickIntervalResolution;
        }

        void MediaObject::setTickInterval(qint32 newTickInterval)
        {
            if ((m_tickIntervalResolution == 0) || (newTickInterval == 0))
                return;
            m_tickInterval = newTickInterval / m_tickIntervalResolution;
            if ((newTickInterval > 0) && (m_tickInterval == 0))
                m_tickInterval = 1;
        }

        void MediaObject::pause()
        {
            if (!m_paused) {
                m_paused = true;
                setState(Phonon::PausedState);
                if (!(waveOutPause(m_hWaveOut) == MMSYSERR_NOERROR))
                {
                    setError(Phonon::NormalError, QLatin1String("cannot pause (system error)"));
                }
            }
        }

        void MediaObject::stop()
        {
            setState(Phonon::StoppedState);
            m_stopped = true;
            m_paused = false;
            seek(0);
            if (!(waveOutReset(m_hWaveOut) == MMSYSERR_NOERROR))
                setError(Phonon::NormalError, QLatin1String("cannot stop (system error)"));
        }

        void MediaObject::play()
        {
            if ((m_state == Phonon::PlayingState) && !m_paused && !m_stopped)
                return;
            if  ((m_state == Phonon::LoadingState) ||
                 (m_state == Phonon::BufferingState) ||
                 (m_state == Phonon::ErrorState)) {
                    setError(Phonon::FatalError, QLatin1String("illegale state for playback"));
                    return;
            }

            if (m_state == Phonon::StoppedState)
                stop();
            if (m_sourceIsValid) {
                setState(Phonon::PlayingState);
                if (!m_paused) {
                    m_nextBufferIndex = true;
                    m_stopped = false;
                    playBuffer(m_soundBuffer1.waveHeader);
                    playBuffer(m_soundBuffer2.waveHeader);
                } else {
                    if (!(waveOutRestart(m_hWaveOut) == MMSYSERR_NOERROR))
                        setError(Phonon::NormalError, QLatin1String("cannot resume (system)"));
                }
            } else {
                setError(Phonon::FatalError, QLatin1String("cannot playback invalid source"));
            }
            m_paused = false;
        }

        QString MediaObject::errorString() const
        {
            
            return m_errorString;
        }

        Phonon::ErrorType MediaObject::errorType() const
        {
            return Phonon::ErrorType();
        }

        qint32 MediaObject::prefinishMark() const
        {
            return m_prefinishMark;
        }

        void MediaObject::setPrefinishMark(qint32 newPrefinishMark)
        {
            m_prefinishMark = newPrefinishMark;
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
            return m_totalTime - m_currentTime;
        }

        Phonon::MediaSource MediaObject::source() const
        {
            return Phonon::MediaSource();
        }

        void MediaObject::setNextSource(const Phonon::MediaSource &source)
        {
            m_nextSource = source;
            m_hasNextSource = true;
        }

        void MediaObject::setSource(const Phonon::MediaSource &source)
        {
            if (m_state == Phonon::PlayingState)
            {
                setError(Phonon::NormalError, QLatin1String("source changed while playing"));
                stop();
            }

            m_source = source;
            m_hasSource = true;
            m_sourceIsValid = false;

            emit currentSourceChanged(source);

            if (source.type() == Phonon::MediaSource::LocalFile) {
                if (!openWaveFile(source.fileName())) {
                  setError(Phonon::FatalError, QLatin1String("cannot open media file"));
                  return ;
                }
            } else if (source.type() == Phonon::MediaSource::Stream) {
                if (m_stream)
                   delete m_stream;
                m_stream = new IOWrapper(this, source);
                m_mediaSize = m_stream->size();
            } else if (source.type() == Phonon::MediaSource::Url) {
                if (!openWaveFile(source.url().toLocalFile())) {
                    setError(Phonon::FatalError, QLatin1String("cannot open media file"));
                    return ;
                }
            } else {
                setError(Phonon::FatalError, QLatin1String("type of source not supported"));
                return ;
            }
            setState(Phonon::LoadingState);

            if (!readHeader())
                setError(Phonon::FatalError, QLatin1String("invalid header"));
            else if (!getWaveOutDevice())
                setError(Phonon::FatalError, QLatin1String("No waveOut device available"));
            else if (!fillBuffers())
                setError(Phonon::FatalError, QLatin1String("no data for buffering"));
            else if (!prepareBuffers())
                setError(Phonon::FatalError, QLatin1String("cannot prepare buffers"));
            else
                m_sourceIsValid = true;

            if (m_sourceIsValid)
                setState(Phonon::StoppedState);
        }

        void MediaObject::seek(qint64 time)
        {
            if (!m_sourceIsValid) {
                setError(Phonon::NormalError, QLatin1String("source is not valid"));
                return;
            }
            if ((time >= 0) && (time < m_totalTime)) {
                int counter = 0;
                while (!m_bufferingFinished && (counter < 200)) {
                  Sleep(20);
                  counter ++;
                }
                if (counter >= 200) {
                   setError(Phonon::NormalError, QLatin1String("buffering timed out"));
                   return;
                }

                m_stream->seek(WAVEHEADER_SIZE + time * m_waveFormatEx.nSamplesPerSec * m_waveFormatEx.wBitsPerSample * m_waveFormatEx.nChannels / 8 / 1000);
                m_currentTime = time;
                if (m_state == Phonon::PlayingState)
                  play();
            } else {
                setError(Phonon::NormalError, QLatin1String("seeking out of range"));
            }
        }

        void MediaObject::unPrepareBuffers()
        {
            if (m_bufferPrepared) {
            DWORD err1 = waveOutUnprepareHeader(m_hWaveOut, m_soundBuffer1.waveHeader, sizeof(WAVEHDR));
            DWORD err2 = waveOutUnprepareHeader(m_hWaveOut, m_soundBuffer2.waveHeader, sizeof(WAVEHDR));
            if (!(err1 == MMSYSERR_NOERROR) || !(err2 == MMSYSERR_NOERROR))
                setError(Phonon::NormalError, QLatin1String("cannot unprepare buffer") + getErrorText(err1) + getErrorText(err2));
            }
            m_bufferPrepared = false;
        }

        bool MediaObject::prepareBuffers()
        {
            memset((void*)m_soundBuffer1.waveHeader, 0, sizeof(WAVEHDR));
            m_soundBuffer1.waveHeader->lpData = m_soundBuffer1.data.data();
            m_soundBuffer1.waveHeader->dwBufferLength = m_soundBuffer1.data.size();
            m_soundBuffer1.waveHeader->dwUser = (DWORD_PTR) this;

            ZeroMemory((void*)m_soundBuffer2.waveHeader, sizeof(WAVEHDR));
            m_soundBuffer2.waveHeader->lpData = m_soundBuffer2.data.data();
            m_soundBuffer2.waveHeader->dwBufferLength = m_soundBuffer1.data.size();
            m_soundBuffer2.waveHeader->dwUser = (DWORD_PTR) this;

            m_bufferPrepared = (waveOutPrepareHeader(m_hWaveOut, m_soundBuffer1.waveHeader, sizeof(WAVEHDR)) == MMSYSERR_NOERROR)
                && (waveOutPrepareHeader(m_hWaveOut, m_soundBuffer2.waveHeader, sizeof(WAVEHDR)) == MMSYSERR_NOERROR);
             return m_bufferPrepared;
        }

        void MediaObject::deleteValidWaveOutDevice()
        {
            if (m_hWaveOut) {
                unPrepareBuffers();
                if (!(waveOutClose(m_hWaveOut)  == MMSYSERR_NOERROR))
                    setError(Phonon::NormalError, QLatin1String("cannot close wave device"));
            }
        }

        bool MediaObject::getWaveOutDevice()
        {
            deleteValidWaveOutDevice();

            for(UINT deviceId = 0; deviceId < waveOutGetNumDevs(); deviceId++)
            {
                if(deviceId == waveOutGetNumDevs())
                    return false;
                if(waveOutOpen(&m_hWaveOut, WAVE_MAPPER, &m_waveFormatEx, (DWORD)WaveOutCallBack, 0, CALLBACK_FUNCTION) == MMSYSERR_NOERROR)
                    return m_hWaveOut; //m_hWaveOut !=0;
            }
            return false;
        }

        bool  MediaObject::openWaveFile(QString fileName)
        {
            if (m_file)
                delete m_file;
            m_file = new QFile(fileName);
            m_file->setParent(this);
            m_stream = m_file;
            m_mediaSize = m_file->size();
            return (m_file->open(QIODevice::ReadOnly));
        }

        bool MediaObject::readHeader()
        {
            QByteArray header = m_stream->read(WAVEHEADER_SIZE);

            if (header.size() == WAVEHEADER_SIZE) {

                m_waveFormatEx.wFormatTag         = *((WORD* )(header.data() + WAVEHEADER_OFFSET_FORMATTAG     ));
                m_waveFormatEx.nChannels          = *((WORD* )(header.data() + WAVEHEADER_OFFSET_CHANNELS      ));
                m_waveFormatEx.nSamplesPerSec     = *((DWORD*)(header.data() + WAVEHEADER_OFFSET_SAMPLESPERSEC ));
                m_waveFormatEx.nAvgBytesPerSec    = *((DWORD*)(header.data() + WAVEHEADER_OFFSET_AVGBYTESPERSEC));
                m_waveFormatEx.nBlockAlign        = *((WORD* )(header.data() + WAVEHEADER_OFFSET_BLOCKALIGN    ));
                m_waveFormatEx.wBitsPerSample     = *((WORD* )(header.data() + WAVEHEADER_OFFSET_BITSPERSAMPLE ));

                m_tickIntervalResolution = (qint64(buffer_size) * 8 * 1000) / m_waveFormatEx.nSamplesPerSec / m_waveFormatEx.wBitsPerSample / m_waveFormatEx.nChannels;
                if (m_mediaSize > 0)
                   m_totalTime = ((m_mediaSize - WAVEHEADER_SIZE) * 8 * 1000) / m_waveFormatEx.nSamplesPerSec / m_waveFormatEx.wBitsPerSample / m_waveFormatEx.nChannels;
                else
                  m_totalTime = -1;
                  emit totalTimeChanged(m_totalTime);
                return true;
            } else {
                return false;
            }
        }
        
        bool MediaObject::fillBuffers()
        {
            
            m_soundBuffer1.data = m_stream->read(buffer_size);
            m_soundBuffer2.data = m_stream->read(buffer_size);

            m_bufferingFinished = true;

            if (!(m_soundBuffer1.data.size() > 0))
                setError(Phonon::NormalError, QLatin1String("cannot read source"));
            return true;
        }

        void MediaObject::setState(Phonon::State newState)
        {
            if (m_state == newState)
                return;
            emit stateChanged(newState, m_state);
            m_state = newState;
        }

        void MediaObject::setError(ErrorType errorType, QString errorMessage)
        {
            m_errorType = errorType;
            setState(Phonon::ErrorState);
            m_errorString = errorMessage;
        }

        void MediaObject::setAudioOutput(QObject *audioOutput)
        {
            m_audioOutput = qobject_cast<AudioOutput*>(audioOutput);

            if (m_audioOutput) {
                m_volume = m_audioOutput->volume();
                connect(m_audioOutput, SIGNAL(volumeChanged(qreal)), this, SLOT(setVolume(qreal)));
            }
        }

        void MediaObject::setVolume(qreal newVolume)
        {
            m_volume = newVolume;
        }

        void MediaObject::swapBuffers()
        {
            if (m_stopped || m_paused)
                return;

            m_currentTime += m_tickIntervalResolution;
            if (m_tickInterval) {
                m_tick ++;
                if (m_tick > (m_tickInterval - 1)) {
                    emit tick(m_currentTime);
                    m_tick = 0;
                }
            }
            if ((m_prefinishMark > 0)&& (m_prefinishMark < m_currentTime))
                emit prefinishMarkReached(m_totalTime - m_currentTime);

            while (!m_bufferingFinished) {
                setState(Phonon::BufferingState);
                qWarning() << QLatin1String("buffer underun");
                Sleep(20);
            }

            setState(Phonon::PlayingState);

            //if size == o then stop...
            if (m_nextBufferIndex) {
                int size = m_soundBuffer1.waveHeader->dwBufferLength = m_soundBuffer1.data.size();
                if (size == buffer_size) {
                    playBuffer(m_soundBuffer1.waveHeader);
                    emit outOfData(m_stream, &m_soundBuffer1.data, &m_bufferingFinished);
                } else {
                    playBuffer(m_soundBuffer1.waveHeader);
                    m_stopped = true;
                    setState(Phonon::StoppedState);
                    emit finished();
                    seek(0);
                }
            } else {
                int size = m_soundBuffer2.waveHeader->dwBufferLength = m_soundBuffer2.data.size();
                if (size == buffer_size) {
                    playBuffer(m_soundBuffer2.waveHeader);
                    emit outOfData(m_stream, &m_soundBuffer2.data, &m_bufferingFinished);
                } else {
                    playBuffer(m_soundBuffer2.waveHeader);
                    m_stopped = true;
                    setState(Phonon::StoppedState);
                    emit finished();
                    seek(0);
                }
            }
            m_nextBufferIndex =! m_nextBufferIndex;
        }


        void MediaObject::playBuffer(WAVEHDR *waveHeader)
        {
            DWORD err = waveOutWrite(m_hWaveOut, waveHeader, sizeof(WAVEHDR));
            if (!err == MMSYSERR_NOERROR) {
                setError(Phonon::FatalError, QLatin1String("cannot play sound buffer (system) ") + getErrorText(err));
                m_stopped = true;
            }
        }
    }
}

QT_END_NAMESPACE

#include "mediaobject.moc"
