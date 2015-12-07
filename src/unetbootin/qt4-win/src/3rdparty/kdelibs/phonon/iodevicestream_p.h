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

#ifndef PHONON_IODEVICESTREAM_P_H
#define PHONON_IODEVICESTREAM_P_H

#include "abstractmediastream.h"

QT_BEGIN_HEADER
QT_BEGIN_NAMESPACE

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

QT_END_NAMESPACE
QT_END_HEADER

#endif // PHONON_IODEVICESTREAM_P_H
