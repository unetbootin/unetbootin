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

#include "streaminterface.h"
#include "streaminterface_p.h"
#include "abstractmediastream.h"
#include "abstractmediastream_p.h"
#include "mediasource_p.h"

QT_BEGIN_NAMESPACE

#ifndef QT_NO_PHONON_ABSTRACTMEDIASTREAM

namespace Phonon
{

StreamInterface::StreamInterface()
    : d(new StreamInterfacePrivate)
{
    d->q = this;
}

StreamInterface::~StreamInterface()
{
    if (d->connected) {
        AbstractMediaStreamPrivate *dd = d->mediaSource.stream()->d_func();
        dd->setStreamInterface(0);
    }
    delete d;
}

void StreamInterface::connectToSource(const MediaSource &mediaSource)
{
    Q_ASSERT(!d->connected);
    d->connected = true;
    d->mediaSource = mediaSource;
    Q_ASSERT(d->mediaSource.type() == MediaSource::Stream);
    Q_ASSERT(d->mediaSource.stream());
    AbstractMediaStreamPrivate *dd = d->mediaSource.stream()->d_func();
    dd->setStreamInterface(this);
    d->mediaSource.stream()->reset();
}

void StreamInterfacePrivate::disconnectMediaStream()
{
    Q_ASSERT(connected);
    connected = false;

    // if mediaSource has autoDelete set then it will delete the AbstractMediaStream again who's
    // destructor is calling us right now
    mediaSource.setAutoDelete(false);

    mediaSource = MediaSource();
    q->endOfData();
    q->setStreamSeekable(false);
}

void StreamInterface::needData()
{
    if (d->mediaSource.type() == MediaSource::Stream) {
        d->mediaSource.stream()->needData();
    }
}

void StreamInterface::enoughData()
{
    Q_ASSERT(d->connected);
    if (d->mediaSource.type() == MediaSource::Stream) {
        d->mediaSource.stream()->enoughData();
    }
}

void StreamInterface::seekStream(qint64 offset)
{
    Q_ASSERT(d->connected);
    if (d->mediaSource.type() == MediaSource::Stream) {
        d->mediaSource.stream()->seekStream(offset);
    }
}

void StreamInterface::reset()
{
    Q_ASSERT(d->connected);
    if (d->mediaSource.type() == MediaSource::Stream) {
        d->mediaSource.stream()->reset();
    }
}

} // namespace Phonon

#endif //QT_NO_PHONON_ABSTRACTMEDIASTREAM

QT_END_NAMESPACE


