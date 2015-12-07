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

#include "mediasource.h"
#include "mediasource_p.h"
#include "iodevicestream_p.h"
#include "abstractmediastream_p.h"

#include <QtCore/QFileInfo>
#include <QtCore/QFile>
#include <QtCore/QFSFileEngine>

QT_BEGIN_NAMESPACE

namespace Phonon
{

MediaSource::MediaSource(MediaSourcePrivate &dd)
    : d(&dd)
{
}

MediaSource::MediaSource()
    : d(new MediaSourcePrivate(Empty))
{
}

MediaSource::MediaSource(const QString &filename)
    : d(new MediaSourcePrivate(LocalFile))
{
    const QFileInfo fileInfo(filename);
    if (fileInfo.exists()) {
        bool localFs = QAbstractFileEngine::LocalDiskFlag & QFSFileEngine(filename).fileFlags(QAbstractFileEngine::LocalDiskFlag);
        if (localFs) {
            d->url = QUrl::fromLocalFile(fileInfo.absoluteFilePath());
        } else {
#ifndef QT_NO_PHONON_ABSTRACTMEDIASTREAM
            // it's a Qt resource -> use QFile
            d->type = Stream;
            d->ioDevice = new QFile(filename);
            d->setStream(new IODeviceStream(d->ioDevice, d->ioDevice));
#else
            d->type = Invalid;
#endif //QT_NO_PHONON_ABSTRACTMEDIASTREAM
        }
    } else {
        d->url = filename;
        if (d->url.isValid()) {
            d->type = Url;
        } else {
            d->type = Invalid;
        }
    }
}

MediaSource::MediaSource(const QUrl &url)
    : d(new MediaSourcePrivate(Url))
{
    if (url.isValid()) {
        d->url = url;
    } else {
        d->type = Invalid;
    }
}

MediaSource::MediaSource(Phonon::DiscType dt, const QString &deviceName)
    : d(new MediaSourcePrivate(Disc))
{
    if (dt == NoDisc) {
        d->type = Invalid;
        return;
    }
    d->discType = dt;
    d->deviceName = deviceName;
}

#ifndef QT_NO_PHONON_ABSTRACTMEDIASTREAM
MediaSource::MediaSource(AbstractMediaStream *stream)
    : d(new MediaSourcePrivate(Stream))
{
    if (stream) {
        d->setStream(stream);
    } else {
        d->type = Invalid;
    }
}

MediaSource::MediaSource(QIODevice *ioDevice)
    : d(new MediaSourcePrivate(Stream))
{
    if (ioDevice) {
        d->setStream(new IODeviceStream(ioDevice, ioDevice));
        d->ioDevice = ioDevice;
    } else {
        d->type = Invalid;
    }
}
#endif //QT_NO_PHONON_ABSTRACTMEDIASTREAM

/* post 4.0
MediaSource::MediaSource(const QList<MediaSource> &mediaList)
    : d(new MediaSourcePrivate(Link))
{
    d->linkedSources = mediaList;
    foreach (MediaSource ms, mediaList) {
        Q_ASSERT(ms.type() != Link);
    }
}

QList<MediaSource> MediaSource::substreams() const
{
    return d->linkedSources;
}
*/

MediaSource::~MediaSource()
{
}

MediaSourcePrivate::~MediaSourcePrivate()
{
#ifndef QT_NO_PHONON_ABSTRACTMEDIASTREAM
    if (autoDelete) {
        delete stream;
        delete ioDevice;
    }
#endif //QT_NO_PHONON_ABSTRACTMEDIASTREAM
}

MediaSource::MediaSource(const MediaSource &rhs)
    : d(rhs.d)
{
}

MediaSource &MediaSource::operator=(const MediaSource &rhs)
{
    d = rhs.d;
    return *this;
}

bool MediaSource::operator==(const MediaSource &rhs) const
{
    return d == rhs.d;
}

void MediaSource::setAutoDelete(bool autoDelete)
{
    d->autoDelete = autoDelete;
}

bool MediaSource::autoDelete() const
{
    return d->autoDelete;
}

MediaSource::Type MediaSource::type() const
{
#ifndef QT_NO_PHONON_ABSTRACTMEDIASTREAM
    if (d->type == Stream && d->stream == 0) {
        return Invalid;
    }
#endif //QT_NO_PHONON_ABSTRACTMEDIASTREAM
    return d->type;
}

QString MediaSource::fileName() const
{
    return d->url.toLocalFile();
}

QUrl MediaSource::url() const
{
    return d->url;
}

Phonon::DiscType MediaSource::discType() const
{
    return d->discType;
}

QString MediaSource::deviceName() const
{
    return d->deviceName;
}

#ifndef QT_NO_PHONON_ABSTRACTMEDIASTREAM
AbstractMediaStream *MediaSource::stream() const
{
    return d->stream;
}

void MediaSourcePrivate::setStream(AbstractMediaStream *s)
{
    stream = s;
}
#endif //QT_NO_PHONON_ABSTRACTMEDIASTREAM


//X AudioCaptureDevice MediaSource::audioCaptureDevice() const
//X {
//X     return d->audioCaptureDevice;
//X }
//X 
//X VideoCaptureDevice MediaSource::videoCaptureDevice() const
//X {
//X     return d->videoCaptureDevice;
//X }

} // namespace Phonon

QT_END_NAMESPACE

// vim: sw=4 sts=4 et tw=100
