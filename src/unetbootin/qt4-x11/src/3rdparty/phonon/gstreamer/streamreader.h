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

#ifndef PHONON_IODEVICEREADER_H
#define PHONON_IODEVICEREADER_H

#include <phonon/mediasource.h>
#include <phonon/streaminterface.h>

QT_BEGIN_NAMESPACE

namespace Phonon
{
    class MediaSource;
    namespace Gstreamer
    {
        class StreamReader : public Phonon::StreamInterface
        {
        public:

           StreamReader(const Phonon::MediaSource &source)
            :  m_pos(0)
             , m_size(0)
             , m_seekable(false)
            {
                connectToSource(source);
            }

            int currentBufferSize() const
            {
                return m_buffer.size();
            }

            void writeData(const QByteArray &data) {
                m_pos += data.size();
                m_buffer += data;
            }

            void setCurrentPos(qint64 pos)
            {
                m_pos = pos;
                seekStream(pos);
                m_buffer.clear();
            }
            
            quint64 currentPos() const
            {
                return m_pos;
            }

            bool read(quint64 offset, int length, char * buffer);

            void endOfData() {}

            void setStreamSize(qint64 newSize) {
                m_size = newSize;
            }

            qint64 streamSize() const {
                return m_size;
            }

            void setStreamSeekable(bool s) {
                m_seekable = s;
            }

            bool streamSeekable() const {
                return m_seekable;
            }

private:
            QByteArray m_buffer;
            quint64 m_pos;
            quint64 m_size;
            bool m_seekable;
        };
    }
}

QT_END_NAMESPACE

#endif
