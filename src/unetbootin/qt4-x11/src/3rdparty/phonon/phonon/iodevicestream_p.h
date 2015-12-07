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

#ifndef PHONON_IODEVICESTREAM_P_H
#define PHONON_IODEVICESTREAM_P_H

#include "abstractmediastream.h"

QT_BEGIN_HEADER
QT_BEGIN_NAMESPACE

#ifndef QT_NO_PHONON_ABSTRACTMEDIASTREAM

class QIODevice;

namespace Phonon
{

class IODeviceStreamPrivate;
class IODeviceStream : public AbstractMediaStream
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(IODeviceStream)
    public:
        explicit IODeviceStream(QIODevice *ioDevice, QObject *parent = 0);
        ~IODeviceStream();

        void reset();
        void needData();
        void seekStream(qint64);
};
} // namespace Phonon

#endif //QT_NO_PHONON_ABSTRACTMEDIASTREAM

QT_END_NAMESPACE
QT_END_HEADER

#endif // PHONON_IODEVICESTREAM_P_H
