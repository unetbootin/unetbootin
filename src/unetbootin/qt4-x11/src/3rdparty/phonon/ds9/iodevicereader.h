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

#include "qbasefilter.h"

#include <phonon/mediasource.h>

QT_BEGIN_NAMESPACE

#ifndef QT_NO_PHONON_ABSTRACTMEDIASTREAM

namespace Phonon
{
    class MediaSource;
    namespace DS9
    {
        class MediaGraph;
        class StreamReader;

        //for the streams...
        //the filter
        class IODeviceReader : public QBaseFilter
        {
        public:
            IODeviceReader(const MediaSource &source, const MediaGraph *);
            ~IODeviceReader();
            STDMETHODIMP Stop();

        private:
            StreamReader *m_streamReader;
        };

    }
}

#endif //QT_NO_PHONON_ABSTRACTMEDIASTREAM

QT_END_NAMESPACE

#endif
