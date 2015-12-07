/*  This file is part of the KDE project
    Copyright (C) 2007 Matthias Kretz <kretz@kde.org>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License version 2 as published by the Free Software Foundation.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.

*/

#ifndef ABSTRACTMEDIASTREAM_P_H
#define ABSTRACTMEDIASTREAM_P_H

#include "phonon_export.h"
#include "abstractmediastream.h"
#include "mediaobject_p.h"
#include "streaminterface.h"

#include "medianodedestructionhandler_p.h"

QT_BEGIN_NAMESPACE

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
            mediaObjectPrivate(0)
        {
        }
        ~AbstractMediaStreamPrivate();

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

QT_END_NAMESPACE

#endif // ABSTRACTMEDIASTREAM_P_H
// vim: sw=4 sts=4 et tw=100
