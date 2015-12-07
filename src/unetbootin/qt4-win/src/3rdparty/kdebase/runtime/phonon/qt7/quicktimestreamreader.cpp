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

#include "backendheader.h"
#include "quicktimestreamreader.h"

QT_BEGIN_NAMESPACE

namespace Phonon
{
namespace QT7
{

// Defined in quicktimestreamreader_objc.m:
void *QuickTimeStreamReader_createMovieFromBufferGuessType(QByteArray &buffer);
Movie QuickTimeStreamReader_convertToQuickTime(void *qtmovie);
void QuickTimeStreamReader_release(void *movie);

QuickTimeStreamReader::QuickTimeStreamReader(const Phonon::MediaSource &source)
{
    connectToSource(source);

    // Regretfully, I need to load the whole movie into memory
    // before I can tell QuickTime to play it. (whish 
    // kQTDataLocationPropertyID_MovieUserProc worked...)    
    if(!readAllDataIntoBuffer())
        return;
        
    m_QTMovieRef = QuickTimeStreamReader_createMovieFromBufferGuessType(m_buffer);
}

QuickTimeStreamReader::~QuickTimeStreamReader()
{
    if (m_QTMovieRef)
        QuickTimeStreamReader_release(m_QTMovieRef);
}

bool QuickTimeStreamReader::readAllDataIntoBuffer()
{
    int oldSize = m_buffer.size();
    while (m_buffer.size() < m_size){
        needData();
        if (oldSize == currentBufferSize())
            BACKEND_ASSERT3(oldSize != currentBufferSize(),
                "Could not create new movie from IO stream. Not enough free memory to preload the whole movie.",
                FATAL_ERROR, false)
        oldSize = m_buffer.size();
    }
    return true;
}

Movie QuickTimeStreamReader::movieRef()
{
    return QuickTimeStreamReader_convertToQuickTime(m_QTMovieRef);
}

int QuickTimeStreamReader::readData(long offset, long size, void *data)
{
//    QReadLocker readLocker(&m_lock);
    if (streamSize() != 1 && offset + size > streamSize()){
        size = streamSize() - offset;
    }

    if (currentPos() - currentBufferSize() != offset)
        setCurrentPos(offset);

    int oldSize = currentBufferSize();
    while (currentBufferSize() < int(size)) {
        needData();
        if (oldSize == currentBufferSize())
            break;
        oldSize = currentBufferSize();
    }
    
    int bytesRead = qMin(currentBufferSize(), int(size));
//    QWriteLocker writeLocker(&m_lock);
    qMemCopy(data, m_buffer.data(), bytesRead);
    m_buffer = m_buffer.mid(bytesRead);

    return bytesRead;
}
    
void QuickTimeStreamReader::writeData(const QByteArray &data)
{
    QWriteLocker locker(&m_lock);
    m_pos += data.size();
    m_buffer += data;
}

void QuickTimeStreamReader::endOfData()
{
}

void QuickTimeStreamReader::setStreamSize(qint64 newSize)
{
    m_size = newSize;
}

qint64 QuickTimeStreamReader::streamSize() const
{
    return m_size;
}

void QuickTimeStreamReader::setStreamSeekable(bool s)
{
    m_seekable = s;
}

bool QuickTimeStreamReader::streamSeekable() const
{
    return m_seekable;
}

void QuickTimeStreamReader::setCurrentPos(qint64 pos)
{
    QWriteLocker locker(&m_lock);
    m_pos = pos;
    seekStream(pos);
    m_buffer.clear();
}

qint64 QuickTimeStreamReader::currentPos() const
{
    return m_pos;
}

int QuickTimeStreamReader::currentBufferSize() const
{
    QReadLocker locker(&m_lock);
    return m_buffer.size();
}

}} //namespace Phonon::QT7

QT_END_NAMESPACE

#include "moc_quicktimestreamreader.cpp"

