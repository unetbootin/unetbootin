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

#include "iodevicestream_p.h"
#include "abstractmediastream_p.h"

QT_BEGIN_NAMESPACE

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
}

IODeviceStream::~IODeviceStream()
{
}

void IODeviceStream::reset()
{
    Q_D(IODeviceStream);
    d->ioDevice->reset();
}

void IODeviceStream::needData()
{
    Q_D(IODeviceStream);
    const QByteArray data = d->ioDevice->read(4096);
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
}

} // namespace Phonon

QT_END_NAMESPACE

#include "moc_iodevicestream_p.cpp"

// vim: sw=4 sts=4 et tw=100
