/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the QtNetwork module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Commercial License Agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Nokia.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
** package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qnetworkaccessdebugpipebackend_p.h"
#include "QtCore/qdatastream.h"

QT_BEGIN_NAMESPACE

#ifdef QT_BUILD_INTERNAL

enum {
    ReadBufferSize = 16384,
    WriteBufferSize = ReadBufferSize
};

struct QNetworkAccessDebugPipeBackend::DataPacket
{
    QList<QPair<QByteArray, QByteArray> > headers;
    QByteArray data;
};

QNetworkAccessBackend *
QNetworkAccessDebugPipeBackendFactory::create(QNetworkAccessManager::Operation op,
                                              const QNetworkRequest &request) const
{
    // is it an operation we know of?
    switch (op) {
    case QNetworkAccessManager::GetOperation:
    case QNetworkAccessManager::PutOperation:
        break;

    default:
        // no, we can't handle this operation
        return 0;
    }

    QUrl url = request.url();
    if (url.scheme() == QLatin1String("debugpipe"))
        return new QNetworkAccessDebugPipeBackend;
    return 0;
}

QNetworkAccessDebugPipeBackend::QNetworkAccessDebugPipeBackend()
    : incomingPacketSize(0), bareProtocol(false)
{
}

QNetworkAccessDebugPipeBackend::~QNetworkAccessDebugPipeBackend()
{
    socket.disconnect(this);    // we're not interested in the signals at this point
}

void QNetworkAccessDebugPipeBackend::open()
{
    socket.connectToHost(url().host(), url().port(12345));
    socket.setReadBufferSize(ReadBufferSize);
    connect(&socket, SIGNAL(readyRead()), SLOT(socketReadyRead()));
    connect(&socket, SIGNAL(bytesWritten(qint64)), SLOT(socketBytesWritten(qint64)));
    connect(&socket, SIGNAL(error(QAbstractSocket::SocketError)), SLOT(socketError()));
    connect(&socket, SIGNAL(disconnected()), SLOT(socketDisconnected()));

    bareProtocol = url().queryItemValue(QLatin1String("bare")) == QLatin1String("1");

    if (!bareProtocol) {
        // "Handshake":
        // send outgoing metadata and the URL being requested
        DataPacket packet;
        //packet.metaData = request().metaData();
        packet.data = url().toEncoded();
        send(packet);
    }
}

void QNetworkAccessDebugPipeBackend::closeDownstreamChannel()
{
    if (operation() == QNetworkAccessManager::GetOperation)
        socket.disconnectFromHost();
}

void QNetworkAccessDebugPipeBackend::closeUpstreamChannel()
{
    if (operation() == QNetworkAccessManager::PutOperation)
        socket.disconnectFromHost();
    else if (operation() == QNetworkAccessManager::PostOperation) {
        send(DataPacket());
    }
}

bool QNetworkAccessDebugPipeBackend::waitForDownstreamReadyRead(int ms)
{
    readyReadEmitted = false;
    if (socket.bytesAvailable()) {
        socketReadyRead();
        if (readyReadEmitted)
            return true;
    }
    socket.waitForReadyRead(ms);
    return readyReadEmitted;
}

bool QNetworkAccessDebugPipeBackend::waitForUpstreamBytesWritten(int ms)
{
    bytesWrittenEmitted = false;
    upstreamReadyRead();
    if (bytesWrittenEmitted)
        return true;

    socket.waitForBytesWritten(ms);
    return bytesWrittenEmitted;
}

void QNetworkAccessDebugPipeBackend::upstreamReadyRead()
{
    int maxWrite = WriteBufferSize - socket.bytesToWrite();
    if (maxWrite <= 0)
        return;                 // can't write yet, wait for the socket to write

    if (bareProtocol) {
        QByteArray data = readUpstream();
        if (data.isEmpty())
            return;

        socket.write(data);
        upstreamBytesConsumed(data.size());
        bytesWrittenEmitted = true;
        return;
    }

    DataPacket packet;
    packet.data = readUpstream();
    if (packet.data.isEmpty())
        return;                 // we'll be called again when there's data
    if (packet.data.size() > maxWrite)
        packet.data.truncate(maxWrite);

    if (!send(packet)) {
        QString msg = QObject::tr("Write error writing to %1: %2")
                      .arg(url().toString(), socket.errorString());
        error(QNetworkReply::ProtocolFailure, msg);

        finished();
        return;
    }
    upstreamBytesConsumed(packet.data.size());
    bytesWrittenEmitted = true;
}

void QNetworkAccessDebugPipeBackend::downstreamReadyWrite()
{
    socketReadyRead();
}

void QNetworkAccessDebugPipeBackend::socketReadyRead()
{
    if (bareProtocol) {
        qint64 bytesToRead = socket.bytesAvailable();
        if (bytesToRead) {
            QByteArray buffer;
            buffer.resize(bytesToRead);
            qint64 bytesRead = socket.read(buffer.data(), bytesToRead);
            if (bytesRead < bytesToRead)
                buffer.truncate(bytesRead);
            writeDownstreamData(buffer);
            readyReadEmitted = true;
        }
        return;
    }

    while (canReceive() &&
           (socket.state() == QAbstractSocket::UnconnectedState || nextDownstreamBlockSize())) {
        DataPacket packet;
        if (receive(packet)) {
            if (!packet.headers.isEmpty()) {
                QList<QPair<QByteArray, QByteArray> >::ConstIterator
                    it = packet.headers.constBegin(),
                    end = packet.headers.constEnd();
                for ( ; it != end; ++it)
                    setRawHeader(it->first, it->second);
                metaDataChanged();
            }

            if (!packet.data.isEmpty()) {
                writeDownstreamData(packet.data);
                readyReadEmitted = true;
            }

            if (packet.headers.isEmpty() && packet.data.isEmpty()) {
                // it's an eof
                socket.close();
                readyReadEmitted = true;
            }
        } else {
            // got an error
            QString msg = QObject::tr("Read error reading from %1: %2")
                          .arg(url().toString(), socket.errorString());
            error(QNetworkReply::ProtocolFailure, msg);

            finished();
            return;
        }
    }
}

void QNetworkAccessDebugPipeBackend::socketBytesWritten(qint64)
{
    upstreamReadyRead();
}

void QNetworkAccessDebugPipeBackend::socketError()
{
    QNetworkReply::NetworkError code;
    switch (socket.error()) {
    case QAbstractSocket::RemoteHostClosedError:
        return;                 // socketDisconnected will be called

    case QAbstractSocket::NetworkError:
        code = QNetworkReply::UnknownNetworkError;
        break;

    default:
        code = QNetworkReply::ProtocolFailure;
        break;
    }

    error(code, QObject::tr("Socket error on %1: %2")
          .arg(url().toString(), socket.errorString()));
    finished();
    disconnect(&socket, SIGNAL(disconnected()), this, SLOT(socketDisconnected()));

}

void QNetworkAccessDebugPipeBackend::socketDisconnected()
{
    socketReadyRead();
    if (incomingPacketSize == 0 && socket.bytesToWrite() == 0) {
        // normal close
        finished();
    } else {
        // abnormal close
        QString msg = QObject::tr("Remote host closed the connection prematurely on %1")
                             .arg(url().toString());
        error(QNetworkReply::RemoteHostClosedError, msg);

        finished();
    }
}

bool QNetworkAccessDebugPipeBackend::send(const DataPacket &packet)
{
    QByteArray ba;
    {
        QDataStream stream(&ba, QIODevice::WriteOnly);
        stream.setVersion(QDataStream::Qt_4_4);

        stream << packet.headers << packet.data;
    }

    qint32 outgoingPacketSize = ba.size();
    qint64 written = socket.write((const char*)&outgoingPacketSize, sizeof outgoingPacketSize);
    written += socket.write(ba);
    return quint64(written) == (outgoingPacketSize + sizeof outgoingPacketSize);
}

bool QNetworkAccessDebugPipeBackend::receive(DataPacket &packet)
{
    if (!canReceive())
        return false;

    // canReceive() does the setting up for us
    Q_ASSERT(socket.bytesAvailable() >= incomingPacketSize);
    QByteArray incomingPacket = socket.read(incomingPacketSize);
    QDataStream stream(&incomingPacket, QIODevice::ReadOnly);
    stream.setVersion(QDataStream::Qt_4_4);
    stream >> packet.headers >> packet.data;

    // reset for next packet:
    incomingPacketSize = 0;
    socket.setReadBufferSize(ReadBufferSize);
    return true;
}

bool QNetworkAccessDebugPipeBackend::canReceive()
{
    if (incomingPacketSize == 0) {
        // read the packet size
        if (quint64(socket.bytesAvailable()) >= sizeof incomingPacketSize)
            socket.read((char*)&incomingPacketSize, sizeof incomingPacketSize);
        else
            return false;
    }

    if (incomingPacketSize == 0) {
        QString msg = QObject::tr("Protocol error: packet of size 0 received");
        error(QNetworkReply::ProtocolFailure, msg);
        finished();

        socket.blockSignals(true);
        socket.abort();
        socket.blockSignals(false);
        return false;
    }

    return socket.bytesAvailable() >= incomingPacketSize;
}

#endif

QT_END_NAMESPACE
