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

#include "abstractmediastream.h"
#include "abstractmediastream_p.h"
#include "mediaobjectinterface.h"
#include "mediaobject_p.h"
#include "streaminterface_p.h"

QT_BEGIN_NAMESPACE

namespace Phonon
{

AbstractMediaStream::AbstractMediaStream(QObject *parent)
    : QObject(parent),
    d_ptr(new AbstractMediaStreamPrivate)
{
    d_ptr->q_ptr = this;
}

AbstractMediaStream::AbstractMediaStream(AbstractMediaStreamPrivate &dd, QObject *parent)
    : QObject(parent),
    d_ptr(&dd)
{
    d_ptr->q_ptr = this;
}

AbstractMediaStream::~AbstractMediaStream()
{
    delete d_ptr;
}

qint64 AbstractMediaStream::streamSize() const
{
    return d_ptr->streamSize;
}

void AbstractMediaStream::setStreamSize(qint64 newSize)
{
    Q_D(AbstractMediaStream);
    d->streamSize = newSize;
    if (d->streamInterface) {
        d->streamInterface->setStreamSize(newSize);
    }
}

bool AbstractMediaStream::streamSeekable() const
{
    return d_ptr->streamSeekable;
}

void AbstractMediaStream::setStreamSeekable(bool s)
{
    Q_D(AbstractMediaStream);
    d->streamSeekable = s;
    if (d->streamInterface) {
        d->streamInterface->setStreamSeekable(s);
    }
}

void AbstractMediaStream::writeData(const QByteArray &data)
{
    Q_D(AbstractMediaStream);
    if (d->ignoreWrites) {
        return;
    }
    Q_ASSERT(d->streamInterface);
    d->streamInterface->writeData(data);
}

void AbstractMediaStream::endOfData()
{
    Q_D(AbstractMediaStream);
    if (d->streamInterface) {
        d->streamInterface->endOfData();
    }
}

void AbstractMediaStream::error(Phonon::ErrorType type, const QString &text)
{
    Q_D(AbstractMediaStream);
    d->errorType = type;
    d->errorText = text;
    if (d->mediaObjectPrivate) {
        d->mediaObjectPrivate->streamError(type, text);
    }
}

void AbstractMediaStream::enoughData()
{
}

void AbstractMediaStream::seekStream(qint64)
{
    Q_ASSERT(!d_ptr->streamSeekable);
}

AbstractMediaStreamPrivate::~AbstractMediaStreamPrivate()
{
    if (mediaObjectPrivate) {
        mediaObjectPrivate->removeDestructionHandler(this);
    }
    if (streamInterface) {
        streamInterface->d->disconnectMediaStream();
    }
}

void AbstractMediaStreamPrivate::setStreamInterface(StreamInterface *iface)
{
    Q_Q(AbstractMediaStream);
    streamInterface = iface;
    if (!iface) {
        // our subclass might be just about to call writeData, so tell it we have enoughData and
        // ignore the next writeData calls
        q->enoughData();
        ignoreWrites = true;
        return;
    }
    if (ignoreWrites) {
        ignoreWrites = false;
        // we had a StreamInterface before. The new StreamInterface expects us to start reading from
        // position 0
        q->reset();
    } else {
        iface->setStreamSize(streamSize);
        iface->setStreamSeekable(streamSeekable);
    }
}

void AbstractMediaStreamPrivate::setMediaObjectPrivate(MediaObjectPrivate *mop)
{
    mediaObjectPrivate = mop;
    mediaObjectPrivate->addDestructionHandler(this);
    if (!errorText.isEmpty()) {
        mediaObjectPrivate->streamError(errorType, errorText);
    }
}

void AbstractMediaStreamPrivate::phononObjectDestroyed(MediaNodePrivate *bp)
{
    Q_ASSERT(bp == mediaObjectPrivate);
    Q_UNUSED(bp);
    mediaObjectPrivate = 0;
}

} // namespace Phonon

QT_END_NAMESPACE

#include "moc_abstractmediastream.cpp"

// vim: sw=4 sts=4 et tw=100
