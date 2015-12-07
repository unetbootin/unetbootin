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

#ifndef PHONON_QAUDIOCDREADER_H
#define PHONON_QAUDIOCDREADER_H

#include "qasyncreader.h"
#include "qbasefilter.h"

QT_BEGIN_NAMESPACE

#ifndef QT_NO_PHONON_MEDIACONTROLLER

namespace Phonon
{
    namespace DS9
    {
        struct CDROM_TOC;
        struct WaveStructure;
        extern const IID IID_ITitleInterface;

        //interface for the Titles
        struct ITitleInterface : public IUnknown
        {
            virtual QList<qint64> titles() const = 0;
        };


        class QAudioCDPlayer : public QBaseFilter
        {
        public:
            QAudioCDPlayer();
            ~QAudioCDPlayer();
            STDMETHODIMP QueryInterface(REFIID iid, void** out);
        };

    }
}

#endif //QT_NO_PHONON_MEDIACONTROLLER

QT_END_NAMESPACE

#endif
