/*  This file is part of the KDE project
    Copyright (C) 2007 Matthias Kretz <kretz@kde.org>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) version 3, or any
    later version accepted by the membership of KDE e.V. (or its
    successor approved by the membership of KDE e.V.), Trolltech ASA 
    (or its successors, if any) and the KDE Free Qt Foundation, which shall
    act as a proxy defined in Section 6 of version 3 of the license.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public 
    License along with this library.  If not, see <http://www.gnu.org/licenses/>.

*/

#ifndef ABSTRACTMEDIASTREAM_P_H
#define ABSTRACTMEDIASTREAM_P_H

#include "phonon_export.h"
#include "abstractmediastream.h"
#include "mediaobject_p.h"
#include "streaminterface.h"

#include "medianodedestructionhandler_p.h"

QT_BEGIN_NAMESPACE

#ifndef QT_NO_PHONON_ABSTRACTMEDIASTREAM

class MediaObjectPrivate;

namespace Phonon
{
class PHONON_EXPORT AbstractMediaStreamPrivate : private MediaNodeDestructionHandler
{
    friend class MediaObject;
    Q_DECLARE_PUBLIC(AbstractMediaStream)
    public:
        void setStreamInterface(StreamInterface *);
        void setMediaObjectPrivate(MediaObjectPrivate *);

    protected:
        AbstractMediaStreamPrivate()
            : streamSize(0),
            streamSeekable(false),
            ignoreWrites(false),
            streamInterface(0),
            mediaObjectPrivate(0),
            errorType(NoError)
        {
        }
        ~AbstractMediaStreamPrivate();

        virtual void setStreamSize(qint64 newSize);
        virtual void setStreamSeekable(bool s);
        virtual void writeData(const QByteArray &data);
        virtual void endOfData();
        void phononObjectDestroyed(MediaNodePrivate *);

        AbstractMediaStream *q_ptr;
        qint64 streamSize;
        bool streamSeekable;
        bool ignoreWrites;
        StreamInterface *streamInterface;
        MediaObjectPrivate *mediaObjectPrivate;
        Phonon::ErrorType errorType;
        QString errorText;
};
} // namespace Phonon

#endif //QT_NO_PHONON_ABSTRACTMEDIASTREAM

QT_END_NAMESPACE

#endif // ABSTRACTMEDIASTREAM_P_H
// vim: sw=4 sts=4 et tw=100
