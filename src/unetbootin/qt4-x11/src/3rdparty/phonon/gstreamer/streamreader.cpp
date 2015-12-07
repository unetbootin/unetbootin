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

#include "streamreader.h"
#include <QtCore/QMutex>
#include <phonon/streaminterface.h>

QT_BEGIN_NAMESPACE

namespace Phonon
{
namespace Gstreamer
{

bool StreamReader::read(quint64 pos, int length, char * buffer)
{
     if (currentPos() - currentBufferSize() != pos) {
         if (!streamSeekable())
             return false;
         setCurrentPos(pos);
     }

    while (currentBufferSize() < length) {
        int oldSize = currentBufferSize();
        needData();
        if (oldSize == currentBufferSize())
            return false; // We didn't get any data
    }

    qMemCopy(buffer, m_buffer.data(), length);
    //truncate the buffer
    m_buffer = m_buffer.mid(pos);    
    return true;
}

}
}

QT_END_NAMESPACE
