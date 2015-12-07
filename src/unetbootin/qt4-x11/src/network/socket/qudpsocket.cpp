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

//#define QUDPSOCKET_DEBUG

/*! \class QUdpSocket

    \reentrant
    \brief The QUdpSocket class provides a UDP socket.

    \ingroup io
    \inmodule QtNetwork

    UDP (User Datagram Protocol) is a lightweight, unreliable,
    datagram-oriented, connectionless protocol. It can be used when
    reliability isn't important. QUdpSocket is a subclass of
    QAbstractSocket that allows you to send and receive UDP
    datagrams.

    The most common way to use this class is to bind to an address and port
    using bind(), then call writeDatagram() and readDatagram() to transfer
    data. If you want to use the standard QIODevice functions read(),
    readLine(), write(), etc., you must first connect the socket directly to a
    peer by calling connectToHost().

    The socket emits the bytesWritten() signal every time a datagram
    is written to the network. If you just want to send datagrams,
    you don't need to call bind().

    The readyRead() signal is emitted whenever datagrams arrive. In
    that case, hasPendingDatagrams() returns true. Call
    pendingDatagramSize() to obtain the size of the first pending
    datagram, and readDatagram() to read it.

    Example:

    \snippet doc/src/snippets/code/src_network_socket_qudpsocket.cpp 0

    With QUdpSocket, you can also establish a virtual connection to a
    UDP server using connectToHost() and then use read() and write()
    to exchange datagrams without specifying the receiver for each
    datagram.

    The \l{network/broadcastsender}{Broadcast Sender} and
    \l{network/broadcastreceiver}{Broadcast Receiver} examples
    illustrate how to use QUdpSocket in applications.

    \sa QTcpSocket
*/

/*! \enum QUdpSocket::BindFlag
    \since 4.1

    This enum describes the different flags you can pass to modify the
    behavior of QUdpSocket::bind().

    \value ShareAddress Allow other services to bind to the same address
    and port. This is useful when multiple processes share
    the load of a single service by listening to the same address and port
    (e.g., a web server with several pre-forked listeners can greatly
    improve response time). However, because any service is allowed to
    rebind, this option is subject to certain security considerations.
    Note that by combining this option with ReuseAddressHint, you will
    also allow your service to rebind an existing shared address. On
    Unix, this is equivalent to the SO_REUSEADDR socket option. On Windows,
    this option is ignored.

    \value DontShareAddress Bind the address and port exclusively, so that
    no other services are allowed to rebind. By passing this option to
    QUdpSocket::bind(), you are guaranteed that on successs, your service
    is the only one that listens to the address and port. No services are
    allowed to rebind, even if they pass ReuseAddressHint. This option
    provides more security than ShareAddress, but on certain operating
    systems, it requires you to run the server with administrator privileges.
    On Unix and Mac OS X, not sharing is the default behavior for binding
    an address and port, so this option is ignored. On Windows, this
    option uses the SO_EXCLUSIVEADDRUSE socket option.

    \value ReuseAddressHint Provides a hint to QUdpSocket that it should try
    to rebind the service even if the address and port are already bound by
    another socket. On Windows, this is equivalent to the SO_REUSEADDR
    socket option. On Unix, this option is ignored.

    \value DefaultForPlatform The default option for the current platform.
    On Unix and Mac OS X, this is equivalent to (DontShareAddress
    + ReuseAddressHint), and on Windows, its equivalent to ShareAddress.
*/

#include "qhostaddress.h"
#include "qabstractsocket_p.h"
#include "qudpsocket.h"

QT_BEGIN_NAMESPACE

#ifndef QT_NO_UDPSOCKET

#define QT_CHECK_BOUND(function, a) do { \
    if (!isValid()) { \
        qWarning(function" called on a QUdpSocket when not in QUdpSocket::BoundState"); \
        return (a); \
    } } while (0)

class QUdpSocketPrivate : public QAbstractSocketPrivate
{
    Q_DECLARE_PUBLIC(QUdpSocket)

    bool doEnsureInitialized(const QHostAddress &bindAddress, quint16 bindPort,
                             const QHostAddress &remoteAddress);
public:
    inline bool ensureInitialized(const QHostAddress &bindAddress, quint16 bindPort)
    { return doEnsureInitialized(bindAddress, bindPort, QHostAddress()); }

    inline bool ensureInitialized(const QHostAddress &remoteAddress)
    { return doEnsureInitialized(QHostAddress(), 0, remoteAddress); }
};

bool QUdpSocketPrivate::doEnsureInitialized(const QHostAddress &bindAddress, quint16 bindPort,
                                            const QHostAddress &remoteAddress)
{
    const QHostAddress *address = &bindAddress;
    QAbstractSocket::NetworkLayerProtocol proto = address->protocol();
    if (proto == QUdpSocket::UnknownNetworkLayerProtocol) {
        address = &remoteAddress;
        proto = address->protocol();
    }

#if defined(QT_NO_IPV6)
    Q_Q(QUdpSocket);
    if (proto == QUdpSocket::IPv6Protocol) {
        socketError = QUdpSocket::UnsupportedSocketOperationError;
        q->setErrorString(QUdpSocket::tr("This platform does not support IPv6"));
        return false;
    }
#endif

    // now check if the socket engine is initialized and to the right type
    if (!socketEngine || !socketEngine->isValid() || socketEngine->protocol() != proto) {
        resolveProxy(remoteAddress.toString(), bindPort);
        if (!initSocketLayer(address->protocol()))
            return false;
    }

    return true;
}

/*!
    Creates a QUdpSocket object.

    \a parent is passed to the QObject constructor.

    \sa socketType()
*/
QUdpSocket::QUdpSocket(QObject *parent)
    : QAbstractSocket(UdpSocket, *new QUdpSocketPrivate, parent)
{
    d_func()->isBuffered = false;
}

/*!
    Destroys the socket, closing the connection if necessary.

    \sa close()
*/
QUdpSocket::~QUdpSocket()
{
}

/*!
    Binds this socket to the address \a address and the port \a port.
    When bound, the signal readyRead() is emitted whenever a UDP
    datagram arrives on the specified address and port. This function
    is useful to write UDP servers.

    On success, the functions returns true and the socket enters
    BoundState; otherwise it returns false.

    The socket is bound using the DefaultForPlatform BindMode.

    \sa readDatagram()
*/
bool QUdpSocket::bind(const QHostAddress &address, quint16 port)
{
    Q_D(QUdpSocket);
    if (!d->ensureInitialized(address, port))
        return false;

    bool result = d_func()->socketEngine->bind(address, port);
    d->cachedSocketDescriptor = d->socketEngine->socketDescriptor();

    if (!result) {
        d->socketError = d_func()->socketEngine->error();
        setErrorString(d_func()->socketEngine->errorString());
        emit error(d_func()->socketError);
        return false;
    }

    d->state = BoundState;
    d->localAddress = d->socketEngine->localAddress();
    d->localPort = d->socketEngine->localPort();

    emit stateChanged(d_func()->state);
    d_func()->socketEngine->setReadNotificationEnabled(true);
    return true;
}

/*!
    \since 4.1
    \overload

    Binds to \a address on port \a port, using the BindMode \a mode.
*/
bool QUdpSocket::bind(const QHostAddress &address, quint16 port, BindMode mode)
{
    Q_D(QUdpSocket);
    if (!d->ensureInitialized(address, port))
        return false;

#ifdef Q_OS_UNIX
    if ((mode & ShareAddress) || (mode & ReuseAddressHint))
        d->socketEngine->setOption(QAbstractSocketEngine::AddressReusable, 1);
    else
        d->socketEngine->setOption(QAbstractSocketEngine::AddressReusable, 0);
#endif
#ifdef Q_OS_WIN
    if (mode & ReuseAddressHint)
        d->socketEngine->setOption(QAbstractSocketEngine::AddressReusable, 1);
    else
        d->socketEngine->setOption(QAbstractSocketEngine::AddressReusable, 0);
    if (mode & DontShareAddress)
        d->socketEngine->setOption(QAbstractSocketEngine::BindExclusively, 1);
    else
        d->socketEngine->setOption(QAbstractSocketEngine::BindExclusively, 0);
#endif
    bool result = d_func()->socketEngine->bind(address, port);
    d->cachedSocketDescriptor = d->socketEngine->socketDescriptor();

    if (!result) {
        d->socketError = d_func()->socketEngine->error();
        setErrorString(d_func()->socketEngine->errorString());
        emit error(d_func()->socketError);
        return false;
    }

    d->state = BoundState;
    d->localAddress = d->socketEngine->localAddress();
    d->localPort = d->socketEngine->localPort();

    emit stateChanged(d_func()->state);
    d_func()->socketEngine->setReadNotificationEnabled(true);
    return true;
}

/*! \overload

    Binds to QHostAddress:Any on port \a port.
*/
bool QUdpSocket::bind(quint16 port)
{
    return bind(QHostAddress::Any, port);
}

/*!
    \since 4.1
    \overload

    Binds to QHostAddress:Any on port \a port, using the BindMode \a mode.
*/
bool QUdpSocket::bind(quint16 port, BindMode mode)
{
    return bind(QHostAddress::Any, port, mode);
}

/*!
    Returns true if at least one datagram is waiting to be read;
    otherwise returns false.

    \sa pendingDatagramSize(), readDatagram()
*/
bool QUdpSocket::hasPendingDatagrams() const
{
    QT_CHECK_BOUND("QUdpSocket::hasPendingDatagrams()", false);
    return d_func()->socketEngine->hasPendingDatagrams();
}

/*!
    Returns the size of the first pending UDP datagram. If there is
    no datagram available, this function returns -1.

    \sa hasPendingDatagrams(), readDatagram()
*/
qint64 QUdpSocket::pendingDatagramSize() const
{
    QT_CHECK_BOUND("QUdpSocket::pendingDatagramSize()", -1);
    return d_func()->socketEngine->pendingDatagramSize();
}

/*!
    Sends the datagram at \a data of size \a size to the host
    address \a address at port \a port. Returns the number of
    bytes sent on success; otherwise returns -1.

    Datagrams are always written as one block. The maximum size of a
    datagram is highly platform-dependent, but can be as low as 8192
    bytes. If the datagram is too large, this function will return -1
    and error() will return DatagramTooLargeError.

    Sending datagrams larger than 512 bytes is in general disadvised,
    as even if they are sent successfully, they are likely to be
    fragmented by the IP layer before arriving at their final
    destination.

    \warning Calling this function on a connected UDP socket may
    result in an error and no packet being sent. If you are using a
    connected socket, use write() to send datagrams.

    \sa readDatagram(), write()
*/
qint64 QUdpSocket::writeDatagram(const char *data, qint64 size, const QHostAddress &address,
                                  quint16 port)
{
    Q_D(QUdpSocket);
#if defined QUDPSOCKET_DEBUG
    qDebug("QUdpSocket::writeDatagram(%p, %llu, \"%s\", %i)", data, size,
           address.toString().toLatin1().constData(), port);
#endif
    if (!d->ensureInitialized(address))
        return -1;

    qint64 sent = d->socketEngine->writeDatagram(data, size, address, port);
    d->cachedSocketDescriptor = d->socketEngine->socketDescriptor();

    if (sent >= 0) {
        emit bytesWritten(sent);
    } else {
        d->socketError = d->socketEngine->error();
        setErrorString(d->socketEngine->errorString());
        emit error(d->socketError);
    }
    return sent;
}

/*! 
    \fn qint64 QUdpSocket::writeDatagram(const QByteArray &datagram,
                                             const QHostAddress &host, quint16 port)
    \overload

    Sends the datagram \a datagram to the host address \a host and at
    port \a port.
*/

/*!
    Receives a datagram no larger than \a maxSize bytes and stores
    it in \a data. The sender's host address and port is stored in
    *\a address and *\a port (unless the pointers are 0).

    Returns the size of the datagram on success; otherwise returns
    -1.

    If \a maxSize is too small, the rest of the datagram will be
    lost. To avoid loss of data, call pendingDatagramSize() to
    determine the size of the pending datagram before attempting to
    read it. If \a maxSize is 0, the datagram will be discarded.

    \sa writeDatagram(), hasPendingDatagrams(), pendingDatagramSize()
*/
qint64 QUdpSocket::readDatagram(char *data, qint64 maxSize, QHostAddress *address,
                                    quint16 *port)
{
    Q_D(QUdpSocket);

#if defined QUDPSOCKET_DEBUG
    qDebug("QUdpSocket::readDatagram(%p, %llu, %p, %p)", data, maxSize, address, port);
#endif
    QT_CHECK_BOUND("QUdpSocket::readDatagram()", -1);
    qint64 readBytes = d->socketEngine->readDatagram(data, maxSize, address, port);
    d_func()->socketEngine->setReadNotificationEnabled(true);
    if (readBytes < 0) {
        d->socketError = d->socketEngine->error();
        setErrorString(d->socketEngine->errorString());
        emit error(d->socketError);
    }
    return readBytes;
}
#endif // QT_NO_UDPSOCKET

QT_END_NAMESPACE
