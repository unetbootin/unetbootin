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

#include "qaudiocdreader.h"
#include <dshow.h>
#include <initguid.h>

#include <winioctl.h> // needed for FILE_DEVICE_CD_ROM etc

#define IOCTL_CDROM_READ_TOC    CTL_CODE(FILE_DEVICE_CD_ROM, 0x0000, METHOD_BUFFERED, FILE_READ_ACCESS)
#define IOCTL_CDROM_RAW_READ    CTL_CODE(FILE_DEVICE_CD_ROM, 0x000F, METHOD_OUT_DIRECT,  FILE_READ_ACCESS)

QT_BEGIN_NAMESPACE

#ifndef QT_NO_PHONON_MEDIACONTROLLER

namespace Phonon
{
    namespace DS9
    {
        // {CA46BFE1-D55B-4adf-B803-BC2B9AD57824}
        DEFINE_GUID(IID_ITitleInterface, 
            0xca46bfe1, 0xd55b, 0x4adf, 0xb8, 0x3, 0xbc, 0x2b, 0x9a, 0xd5, 0x78, 0x24);

        struct TRACK_DATA {
            UCHAR Reserved;
            UCHAR Control : 4;
            UCHAR Adr : 4;
            UCHAR TrackNumber;
            UCHAR Reserved1;
            UCHAR Address[4];
        };

        struct CDROM_TOC {
            UCHAR Length[2];
            UCHAR FirstTrack;
            UCHAR LastTrack;
            TRACK_DATA TrackData[100];
        };

        struct WaveStructure
        {
            WaveStructure();

            char riff[4];
            qint32 chunksize;
            char wave[4];
            char fmt[4];
            const qint32 chunksize2;
            const quint16 formatTag;
            const quint16 nChannels;
            const quint32 nSamplesPerSec; 
            const quint32 nAvgBytesPerSec;
            const quint16 nBlockAlign;
            const quint16 bitsPerSample;
            char data[4];
            qint32 dataLength;
        };

        enum TRACK_MODE_TYPE {
            YellowMode2,
            XAForm2,
            CDDA
        };


        struct RAW_READ_INFO {
            LARGE_INTEGER DiskOffset;
            ULONG    SectorCount;
            TRACK_MODE_TYPE TrackMode;
        };

        class QAudioCDReader : public QAsyncReader, public ITitleInterface
        {
        public:
            QAudioCDReader(QBaseFilter *parent, QChar drive = QChar());
            ~QAudioCDReader();

            //reimplementation from IUnknown
            STDMETHODIMP_(ULONG) AddRef();
            STDMETHODIMP_(ULONG) Release();

            STDMETHODIMP Length(LONGLONG *,LONGLONG *);
            STDMETHODIMP QueryInterface(REFIID iid, void** out);
            QList<qint64> titles() const;

        protected:
            HRESULT read(LONGLONG pos, LONG length, BYTE *buffer, LONG *actual);

        private:
            HANDLE m_cddrive;
            CDROM_TOC *m_toc;
            WaveStructure *m_waveHeader;
            qint64 m_trackAddress;
        };


#define SECTOR_SIZE 2352
#define NB_SECTORS_READ 20

        static AM_MEDIA_TYPE getAudioCDMediaType()
        {
            AM_MEDIA_TYPE mt;
            qMemSet(&mt, 0, sizeof(AM_MEDIA_TYPE));
            mt.majortype = MEDIATYPE_Stream;
            mt.subtype = MEDIASUBTYPE_WAVE;
            mt.bFixedSizeSamples = TRUE;
            mt.bTemporalCompression = FALSE;
            mt.lSampleSize = 1;
            mt.formattype = GUID_NULL;
            return mt;
        }

        int addressToSectors(UCHAR address[4])
        {
            return ((address[0] * 60 + address[1]) * 60 + address[2]) * 75 + address[3] - 150;
        }

        WaveStructure::WaveStructure() : chunksize(0), chunksize2(16), 
            formatTag(WAVE_FORMAT_PCM), nChannels(2), nSamplesPerSec(44100), nAvgBytesPerSec(176400), nBlockAlign(4), bitsPerSample(16),
            dataLength(0)
        {
            qMemCopy(riff, "RIFF", 4);
            qMemCopy(wave, "WAVE", 4);
            qMemCopy(fmt,  "fmt ", 4);                    
            qMemCopy(data, "data", 4);
        }


        QAudioCDReader::QAudioCDReader(QBaseFilter *parent, QChar drive) : QAsyncReader(parent, QVector<AM_MEDIA_TYPE>() << getAudioCDMediaType())
        {
            m_toc = new CDROM_TOC;
            m_waveHeader = new WaveStructure;

            //now open the cd-drive
            QString path; 
            if (drive.isNull()) {
                path = QString::fromLatin1("\\\\.\\Cdrom0"); 	 
            } else { 	 
                path = QString::fromLatin1("\\\\.\\%1:").arg(drive); 	 
            }

            m_cddrive = QT_WA_INLINE (
			::CreateFile( (TCHAR*)path.utf16(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL ),
			::CreateFileA( path.toLocal8Bit().constData(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL )
	                );

            qMemSet(m_toc, 0, sizeof(CDROM_TOC));
            //read the TOC
            DWORD bytesRead = 0;
            bool tocRead = ::DeviceIoControl(m_cddrive, IOCTL_CDROM_READ_TOC, 0, 0, m_toc, sizeof(CDROM_TOC), &bytesRead, 0);

            if (!tocRead) {
                qWarning("unable to load the TOC from the CD");
                return;
            }

            m_trackAddress = addressToSectors(m_toc->TrackData[0].Address);
            const qint32 nbSectorsToRead = (addressToSectors(m_toc->TrackData[m_toc->LastTrack + 1 - m_toc->FirstTrack].Address) 
                - m_trackAddress);
            const qint32 dataLength = nbSectorsToRead * SECTOR_SIZE;

            m_waveHeader->chunksize = 4 + (8 + m_waveHeader->chunksize2) + (8 + dataLength);
            m_waveHeader->dataLength = dataLength;
        }

        QAudioCDReader::~QAudioCDReader()
        {
            ::CloseHandle(m_cddrive);
            delete m_toc;
            delete m_waveHeader;

        }

        STDMETHODIMP_(ULONG) QAudioCDReader::AddRef()
        {
            return QAsyncReader::AddRef();
        }

        STDMETHODIMP_(ULONG) QAudioCDReader::Release()
        {
            return QAsyncReader::Release();
        }


        STDMETHODIMP QAudioCDReader::Length(LONGLONG *total,LONGLONG *available)
        {
            const LONGLONG length = sizeof(WaveStructure) + m_waveHeader->dataLength;
            if (total) {
                *total = length;
            }
            if (available) {
                *available = length;
            }

            return S_OK;
        }

        STDMETHODIMP QAudioCDReader::QueryInterface(REFIID iid, void** out)
        {
            if (!out) {
                return E_POINTER;
            }

            if (iid == IID_ITitleInterface) {
                //we reroute that to the pin
                *out = static_cast<ITitleInterface*>(this);
                AddRef();
                return S_OK;
            } else {
                return QAsyncReader::QueryInterface(iid, out);
            }
        }


        HRESULT QAudioCDReader::read(LONGLONG pos, LONG length, BYTE *buffer, LONG *actual)
        {
            LONG nbRead = 0;

            if (actual) {
                *actual = 0;
            }

            if (pos < sizeof(WaveStructure)) {
                //we first copy the content of the structure
                nbRead = qMin(LONG(sizeof(WaveStructure) - pos), length);
                qMemCopy(buffer, reinterpret_cast<char*>(m_waveHeader) + pos, nbRead);
            }

            const LONGLONG posInTrack = pos - sizeof(WaveStructure) + nbRead;
            const int bytesLeft = qMin(m_waveHeader->dataLength - posInTrack, LONGLONG(length - nbRead));

            if (bytesLeft > 0) {

                //we need to read again

                const int surplus = posInTrack % SECTOR_SIZE; //how many bytes too much at the beginning
                const int firstSector = posInTrack / SECTOR_SIZE, 
                    lastSector = (posInTrack + length - 1) / SECTOR_SIZE;
                const int sectorsNeeded = lastSector - firstSector + 1;
                int sectorsRead = 0;

                QByteArray ba(sectorsNeeded * SECTOR_SIZE, 0);


                RAW_READ_INFO ReadInfo;
                ReadInfo.TrackMode = CDDA; // Always use CDDA (numerical: 2)
                ReadInfo.DiskOffset.QuadPart = (m_trackAddress + firstSector) * 2048;
                ReadInfo.SectorCount = qMin(sectorsNeeded - sectorsRead, NB_SECTORS_READ);
                while (ReadInfo.SectorCount) {
                    DWORD dummy = 0;
                    if (::DeviceIoControl( m_cddrive, IOCTL_CDROM_RAW_READ,
                        &ReadInfo, sizeof(ReadInfo),
                        ba.data() + sectorsRead * SECTOR_SIZE,
                        ReadInfo.SectorCount * SECTOR_SIZE,
                        &dummy, NULL ) )
                    {
                        ReadInfo.DiskOffset.QuadPart += ReadInfo.SectorCount * 2048;
                        sectorsRead += ReadInfo.SectorCount;
                        ReadInfo.SectorCount = qMin(sectorsNeeded - sectorsRead, NB_SECTORS_READ);
                    }else {
                        qWarning("an error occurred while reading from the media");
                        return S_FALSE;
                    }

                }

                //consume bytes on the buffer
                qMemCopy(buffer + nbRead, ba.data() + surplus, bytesLeft);

                //at this point we have all we need in the buffer
                nbRead += bytesLeft;
            }

            if (actual) {
                *actual = nbRead;
            }

            return nbRead == length ? S_OK : S_FALSE;
        }

        QList<qint64> QAudioCDReader::titles() const
        {
            QList<qint64> ret;
            ret << 0;
            for(int i = m_toc->FirstTrack; i <= m_toc->LastTrack ; ++i) {
                const uchar *address = m_toc->TrackData[i].Address;
                ret << ((address[0] * 60 + address[1]) * 60 + address[2]) * 1000 + address[3]*1000/75 - 2000;

            }
            return ret;
        }


        QAudioCDPlayer::QAudioCDPlayer() : QBaseFilter(CLSID_NULL)
        {
            new QAudioCDReader(this);
        }

        QAudioCDPlayer::~QAudioCDPlayer()
        {
        }

        STDMETHODIMP QAudioCDPlayer::QueryInterface(REFIID iid, void** out)
        {
            if (iid == IID_ITitleInterface) {
                //we reroute that to the pin
                return pins().first()->QueryInterface(iid, out);
            } else {
                return QBaseFilter::QueryInterface(iid, out);
            }
        }
    }
}

#endif //QT_NO_PHONON_MEDIACONTROLLER

QT_END_NAMESPACE
