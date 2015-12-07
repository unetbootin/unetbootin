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

#include "iodevicestream_p.h"
#include "abstractmediastream_p.h"

QT_BEGIN_NAMESPACE

#ifndef QT_NO_PHONON_ABSTRACTMEDIASTREAM

namespace Phonon
{

class IODeviceStreamPrivate : public AbstractMediaStreamPrivate
{
    Q_DECLARE_PUBLIC(IODeviceStream)
    protected:
        IODeviceStreamPrivate(QIODevice *_ioDevice)
            : ioDevice(_ioDevice)
        {
            if (!ioDevice->isOpen()) {
                ioDevice->open(QIODevice::ReadOnly);
            }
            Q_ASSERT(ioDevice->isOpen());
            Q_ASSERT(ioDevice->isReadable());
            streamSize = ioDevice->size();
            streamSeekable = !ioDevice->isSequential();
        }

    private:
        QIODevice *ioDevice;
};

IODeviceStream::IODeviceStream(QIODevice *ioDevice, QObject *parent)
    : AbstractMediaStream(*new IODeviceStreamPrivate(ioDevice), parent)
{
    Q_D(IODeviceStream);
    d->ioDevice->reset();
}

IODeviceStream::~IODeviceStream()
{
}

void IODeviceStream::reset()
{
    Q_D(IODeviceStream);
    d->ioDevice->reset();
    //resetDone();
}

void IODeviceStream::needData()
{
    quint32 size = 4096;
    Q_D(IODeviceStream);
    const QByteArray data = d->ioDevice->read(size);
    if (data.isEmpty() && !d->ioDevice->atEnd()) {
        error(Phonon::NormalError, d->ioDevice->errorString());
    }
    writeData(data);
    if (d->ioDevice->atEnd()) {
        endOfData();
    }
}

void IODeviceStream::seekStream(qint64 offset)
{
    Q_D(IODeviceStream);
    d->ioDevice->seek(offset);
    //seekStreamDone();
}

} // namespace Phonon

#endif //QT_NO_PHONON_ABSTRACTMEDIASTREAM

QT_END_NAMESPACE

#include "moc_iodevicestream_p.cpp"

// vim: sw=4 sts=4 et tw=100
