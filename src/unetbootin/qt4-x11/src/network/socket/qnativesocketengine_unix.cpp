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

//#define QNATIVESOCKETENGINE_DEBUG

#include "qnativesocketengine_p.h"
#include "qiodevice.h"
#include "qhostaddress.h"
#include "qvarlengtharray.h"
#include "qdatetime.h"
#include <time.h>
#include <errno.h>
#include <fcntl.h>
#ifndef QT_NO_IPV6IFNAME
#include <net/if.h>
#endif
#ifndef QT_NO_IPV6IFNAME
#include <net/if.h>
#endif
#ifdef QT_LINUXBASE
#include <arpa/inet.h>
#endif

#if defined QNATIVESOCKETENGINE_DEBUG
#include <qstring.h>
#include <ctype.h>
#endif

QT_BEGIN_NAMESPACE

#if defined QNATIVESOCKETENGINE_DEBUG

/*
    Returns a human readable representation of the first \a len
    characters in \a data.
*/
static QByteArray qt_prettyDebug(const char *data, int len, int maxSize)
{
    if (!data) return "(null)";
    QByteArray out;
    for (int i = 0; i < len; ++i) {
        char c = data[i];
        if (isprint(c)) {
            out += c;
        } else switch (c) {
        case '\n': out += "\\n"; break;
        case '\r': out += "\\r"; break;
        case '\t': out += "\\t"; break;
        default:
            QString tmp;
            tmp.sprintf("\\%o", c);
            out += tmp.toLatin1();
        }
    }

    if (len < maxSize)
        out += "...";

    return out;
}
#endif

static void qt_ignore_sigpipe()
{
    // Set to ignore SIGPIPE once only.
    static QBasicAtomicInt atom = Q_BASIC_ATOMIC_INITIALIZER(0);
    if (atom.testAndSetRelaxed(0, 1)) {
        struct sigaction noaction;
        memset(&noaction, 0, sizeof(noaction));
        noaction.sa_handler = SIG_IGN;
        ::sigaction(SIGPIPE, &noaction, 0);
    }
}

/*
    Extracts the port and address from a sockaddr, and stores them in
    \a port and \a addr if they are non-null.
*/
static inline void qt_socket_getPortAndAddress(struct sockaddr *sa, quint16 *port, QHostAddress *addr)
{
#if !defined(QT_NO_IPV6)
    if (sa->sa_family == AF_INET6) {
        struct sockaddr_in6 *sa6 = (struct sockaddr_in6 *)sa;
        Q_IPV6ADDR tmp;
        memcpy(&tmp, &sa6->sin6_addr.s6_addr, sizeof(tmp));
        if (addr) {
            QHostAddress tmpAddress;
            tmpAddress.setAddress(tmp);
            *addr = tmpAddress;
#ifndef QT_NO_IPV6IFNAME
            char scopeid[IFNAMSIZ];
            if (::if_indextoname(sa6->sin6_scope_id, scopeid) > 0) {
                addr->setScopeId(QLatin1String(scopeid));
            } else
#endif
            addr->setScopeId(QString::number(sa6->sin6_scope_id));
        }
        if (port)
            *port = ntohs(sa6->sin6_port);
        return;
    }
#endif
    struct sockaddr_in *sa4 = (struct sockaddr_in *)sa;
    if (port)
        *port = ntohs(sa4->sin_port);
    if (addr) {
        QHostAddress tmpAddress;
        tmpAddress.setAddress(ntohl(sa4->sin_addr.s_addr));
        *addr = tmpAddress;
    }
}

/*! \internal

    Creates and returns a new socket descriptor of type \a socketType
    and \a socketProtocol.  Returns -1 on failure.
*/
bool QNativeSocketEnginePrivate::createNewSocket(QAbstractSocket::SocketType socketType,
                                         QAbstractSocket::NetworkLayerProtocol socketProtocol)
{
#ifndef QT_NO_IPV6
    int protocol = (socketProtocol == QAbstractSocket::IPv6Protocol) ? AF_INET6 : AF_INET;
#else
    Q_UNUSED(socketProtocol);
    int protocol = AF_INET;
#endif
    int type = (socketType == QAbstractSocket::UdpSocket) ? SOCK_DGRAM : SOCK_STREAM;
    int socket = qt_socket_socket(protocol, type, 0);

    if (socket <= 0) {
        switch (errno) {
        case EPROTONOSUPPORT:
        case EAFNOSUPPORT:
        case EINVAL:
            setError(QAbstractSocket::UnsupportedSocketOperationError, ProtocolUnsupportedErrorString);
            break;
        case ENFILE:
        case EMFILE:
        case ENOBUFS:
        case ENOMEM:
            setError(QAbstractSocket::SocketResourceError, ResourceErrorString);
            break;
        case EACCES:
            setError(QAbstractSocket::SocketAccessError, AccessErrorString);
            break;
        default:
            break;
        }

        return false;
    }

    // Ensure that the socket is closed on exec*().
    ::fcntl(socket, F_SETFD, FD_CLOEXEC);
    socketDescriptor = socket;
    return true;
}

/*
    Returns the value of the socket option \a opt.
*/
int QNativeSocketEnginePrivate::option(QNativeSocketEngine::SocketOption opt) const
{
    Q_Q(const QNativeSocketEngine);
    if (!q->isValid())
        return -1;

    int n = -1;
    switch (opt) {
    case QNativeSocketEngine::ReceiveBufferSocketOption:
        n = SO_RCVBUF;
        break;
    case QNativeSocketEngine::SendBufferSocketOption:
        n = SO_SNDBUF;
        break;
    case QNativeSocketEngine::NonBlockingSocketOption:
        break;
    case QNativeSocketEngine::BroadcastSocketOption:
        break;
    case QNativeSocketEngine::AddressReusable:
        n = SO_REUSEADDR;
        break;
    case QNativeSocketEngine::BindExclusively:
        return true;
    case QNativeSocketEngine::ReceiveOutOfBandData:
        n = SO_OOBINLINE;
        break;
    }

    int v = -1;
    QT_SOCKOPTLEN_T len = sizeof(v);
    if (getsockopt(socketDescriptor, SOL_SOCKET, n, (char *) &v, &len) != -1)
        return v;
    return -1;
}


/*
    Sets the socket option \a opt to \a v.
*/
bool QNativeSocketEnginePrivate::setOption(QNativeSocketEngine::SocketOption opt, int v)
{
    Q_Q(QNativeSocketEngine);
    if (!q->isValid())
        return false;

    int n = 0;
    switch (opt) {
    case QNativeSocketEngine::ReceiveBufferSocketOption:
        n = SO_RCVBUF;
        break;
    case QNativeSocketEngine::SendBufferSocketOption:
        n = SO_SNDBUF;
        break;
    case QNativeSocketEngine::BroadcastSocketOption:
        n = SO_BROADCAST;
        break;
    case QNativeSocketEngine::NonBlockingSocketOption: {
        // Make the socket nonblocking.
        int flags = ::fcntl(socketDescriptor, F_GETFL, 0);
        if (flags == -1) {
#ifdef QNATIVESOCKETENGINE_DEBUG
            perror("QNativeSocketEnginePrivate::setOption(): fcntl(F_GETFL) failed");
#endif
            return false;
        }
        if (::fcntl(socketDescriptor, F_SETFL, flags | O_NONBLOCK) == -1) {
#ifdef QNATIVESOCKETENGINE_DEBUG
            perror("QNativeSocketEnginePrivate::setOption(): fcntl(F_SETFL) failed");
#endif
            return false;
        }

        return true;
    }
    case QNativeSocketEngine::AddressReusable:
#ifdef SO_REUSEPORT
        n = SO_REUSEPORT;
#else
        n = SO_REUSEADDR;
#endif
        break;
    case QNativeSocketEngine::BindExclusively:
        return true;
    case QNativeSocketEngine::ReceiveOutOfBandData:
        n = SO_OOBINLINE;
        break;
    }

    return ::setsockopt(socketDescriptor, SOL_SOCKET, n, (char *) &v, sizeof(v)) == 0;
}

bool QNativeSocketEnginePrivate::nativeConnect(const QHostAddress &addr, quint16 port)
{
    struct sockaddr_in sockAddrIPv4;
    struct sockaddr *sockAddrPtr = 0;
    QT_SOCKLEN_T sockAddrSize = 0;

#if !defined(QT_NO_IPV6)
    struct sockaddr_in6 sockAddrIPv6;

    if (addr.protocol() == QAbstractSocket::IPv6Protocol) {
        memset(&sockAddrIPv6, 0, sizeof(sockAddrIPv6));
        sockAddrIPv6.sin6_family = AF_INET6;
        sockAddrIPv6.sin6_port = htons(port);
#ifndef QT_NO_IPV6IFNAME
        sockAddrIPv6.sin6_scope_id = ::if_nametoindex(addr.scopeId().toLatin1().data());
#else
        sockAddrIPv6.sin6_scope_id = addr.scopeId().toInt();
#endif
        Q_IPV6ADDR ip6 = addr.toIPv6Address();
        memcpy(&sockAddrIPv6.sin6_addr.s6_addr, &ip6, sizeof(ip6));

        sockAddrSize = sizeof(sockAddrIPv6);
        sockAddrPtr = (struct sockaddr *) &sockAddrIPv6;
    } else
#if 0
    {}
#endif
#endif
    if (addr.protocol() == QAbstractSocket::IPv4Protocol) {
        memset(&sockAddrIPv4, 0, sizeof(sockAddrIPv4));
        sockAddrIPv4.sin_family = AF_INET;
        sockAddrIPv4.sin_port = htons(port);
        sockAddrIPv4.sin_addr.s_addr = htonl(addr.toIPv4Address());

        sockAddrSize = sizeof(sockAddrIPv4);
        sockAddrPtr = (struct sockaddr *) &sockAddrIPv4;
    } else {
        // unreachable
    }

    int connectResult = QT_SOCKET_CONNECT(socketDescriptor, sockAddrPtr, sockAddrSize);
    if (connectResult == -1) {
        switch (errno) {
        case EISCONN:
            socketState = QAbstractSocket::ConnectedState;
            break;
        case ECONNREFUSED:
        case EINVAL:
            setError(QAbstractSocket::ConnectionRefusedError, ConnectionRefusedErrorString);
            socketState = QAbstractSocket::UnconnectedState;
            break;
        case ETIMEDOUT:
            setError(QAbstractSocket::NetworkError, ConnectionTimeOutErrorString);
            break;
        case EHOSTUNREACH:
            setError(QAbstractSocket::NetworkError, HostUnreachableErrorString);
            socketState = QAbstractSocket::UnconnectedState;
            break;
        case ENETUNREACH:
            setError(QAbstractSocket::NetworkError, NetworkUnreachableErrorString);
            socketState = QAbstractSocket::UnconnectedState;
            break;
        case EADDRINUSE:
            setError(QAbstractSocket::NetworkError, AddressInuseErrorString);
            break;
        case EINPROGRESS:
        case EALREADY:
            setError(QAbstractSocket::UnfinishedSocketOperationError, InvalidSocketErrorString);
            socketState = QAbstractSocket::ConnectingState;
            break;
        case EAGAIN:
            setError(QAbstractSocket::UnfinishedSocketOperationError, InvalidSocketErrorString);
            setError(QAbstractSocket::SocketResourceError, ResourceErrorString);
            break;
        case EACCES:
        case EPERM:
            setError(QAbstractSocket::SocketAccessError, AccessErrorString);
            socketState = QAbstractSocket::UnconnectedState;
            break;
        case EAFNOSUPPORT:
        case EBADF:
        case EFAULT:
        case ENOTSOCK:
            socketState = QAbstractSocket::UnconnectedState;
        default:
            break;
        }

        if (socketState != QAbstractSocket::ConnectedState) {
#if defined (QNATIVESOCKETENGINE_DEBUG)
            qDebug("QNativeSocketEnginePrivate::nativeConnect(%s, %i) == false (%s)",
                   addr.toString().toLatin1().constData(), port,
                   socketState == QAbstractSocket::ConnectingState
                   ? "Connection in progress" : socketErrorString.toLatin1().constData());
#endif
            return false;
        }
    }

#if defined (QNATIVESOCKETENGINE_DEBUG)
    qDebug("QNativeSocketEnginePrivate::nativeConnect(%s, %i) == true",
           addr.toString().toLatin1().constData(), port);
#endif

    socketState = QAbstractSocket::ConnectedState;
    return true;
}

bool QNativeSocketEnginePrivate::nativeBind(const QHostAddress &address, quint16 port)
{
    struct sockaddr_in sockAddrIPv4;
    struct sockaddr *sockAddrPtr = 0;
    QT_SOCKLEN_T sockAddrSize = 0;

#if !defined(QT_NO_IPV6)
    struct sockaddr_in6 sockAddrIPv6;

    if (address.protocol() == QAbstractSocket::IPv6Protocol) {
        memset(&sockAddrIPv6, 0, sizeof(sockAddrIPv6));
        sockAddrIPv6.sin6_family = AF_INET6;
        sockAddrIPv6.sin6_port = htons(port);
#ifndef QT_NO_IPV6IFNAME
        sockAddrIPv6.sin6_scope_id = ::if_nametoindex(address.scopeId().toLatin1().data());
#else
        sockAddrIPv6.sin6_scope_id = address.scopeId().toInt();
#endif
        Q_IPV6ADDR tmp = address.toIPv6Address();
        memcpy(&sockAddrIPv6.sin6_addr.s6_addr, &tmp, sizeof(tmp));
        sockAddrSize = sizeof(sockAddrIPv6);
        sockAddrPtr = (struct sockaddr *) &sockAddrIPv6;
    } else
#endif
        if (address.protocol() == QAbstractSocket::IPv4Protocol) {
            memset(&sockAddrIPv4, 0, sizeof(sockAddrIPv4));
            sockAddrIPv4.sin_family = AF_INET;
            sockAddrIPv4.sin_port = htons(port);
            sockAddrIPv4.sin_addr.s_addr = htonl(address.toIPv4Address());
            sockAddrSize = sizeof(sockAddrIPv4);
            sockAddrPtr = (struct sockaddr *) &sockAddrIPv4;
        } else {
            // unreachable
        }

    int bindResult = QT_SOCKET_BIND(socketDescriptor, sockAddrPtr, sockAddrSize);
    if (bindResult < 0) {
        switch(errno) {
        case EADDRINUSE:
            setError(QAbstractSocket::AddressInUseError, AddressInuseErrorString);
            break;
        case EACCES:
            setError(QAbstractSocket::SocketAccessError, AddressProtectedErrorString);
            break;
        case EINVAL:
            setError(QAbstractSocket::UnsupportedSocketOperationError, OperationUnsupportedErrorString);
            break;
        case EADDRNOTAVAIL:
            setError(QAbstractSocket::SocketAddressNotAvailableError, AddressNotAvailableErrorString);
            break;
        default:
            break;
        }

#if defined (QNATIVESOCKETENGINE_DEBUG)
        qDebug("QNativeSocketEnginePrivate::nativeBind(%s, %i) == false (%s)",
               address.toString().toLatin1().constData(), port, socketErrorString.toLatin1().constData());
#endif

        return false;
    }

#if defined (QNATIVESOCKETENGINE_DEBUG)
    qDebug("QNativeSocketEnginePrivate::nativeBind(%s, %i) == true",
           address.toString().toLatin1().constData(), port);
#endif
    socketState = QAbstractSocket::BoundState;
    return true;
}

bool QNativeSocketEnginePrivate::nativeListen(int backlog)
{
    if (qt_socket_listen(socketDescriptor, backlog) < 0) {
        switch (errno) {
        case EADDRINUSE:
            setError(QAbstractSocket::AddressInUseError,
                     PortInuseErrorString);
            break;
        default:
            break;
        }

#if defined (QNATIVESOCKETENGINE_DEBUG)
        qDebug("QNativeSocketEnginePrivate::nativeListen(%i) == false (%s)",
               backlog, socketErrorString.toLatin1().constData());
#endif
        return false;
    }

#if defined (QNATIVESOCKETENGINE_DEBUG)
    qDebug("QNativeSocketEnginePrivate::nativeListen(%i) == true", backlog);
#endif

    socketState = QAbstractSocket::ListeningState;
    return true;
}

int QNativeSocketEnginePrivate::nativeAccept()
{
    int acceptedDescriptor = qt_socket_accept(socketDescriptor, 0, 0);
#if defined (QNATIVESOCKETENGINE_DEBUG)
    qDebug("QNativeSocketEnginePrivate::nativeAccept() == %i", acceptedDescriptor);
#endif
    // Ensure that the socket is closed on exec*()
    ::fcntl(acceptedDescriptor, F_SETFD, FD_CLOEXEC);
    return acceptedDescriptor;
}

qint64 QNativeSocketEnginePrivate::nativeBytesAvailable() const
{
    /*
      Apparently, there is not consistency among different operating
      systems on how to use FIONREAD.

      FreeBSD, Linux and Solaris all expect the 3rd argument to
      ioctl() to be an int, which is normally 32-bit even on 64-bit
      machines.

      IRIX, on the other hand, expects a size_t, which is 64-bit on
      64-bit machines.

      So, the solution is to use size_t initialized to zero to make
      sure all bits are set to zero, preventing underflow with the
      FreeBSD/Linux/Solaris ioctls.
    */
    size_t nbytes = 0;
    // gives shorter than true amounts on Unix domain sockets.
    qint64 available = 0;
    if (::ioctl(socketDescriptor, FIONREAD, (char *) &nbytes) >= 0)
        available = (qint64) *((int *) &nbytes);

#if defined (QNATIVESOCKETENGINE_DEBUG)
    qDebug("QNativeSocketEnginePrivate::nativeBytesAvailable() == %lli", available);
#endif
    return available;
}

bool QNativeSocketEnginePrivate::nativeHasPendingDatagrams() const
{
    // Create a sockaddr struct and reset its port number.
#if !defined(QT_NO_IPV6)
    struct sockaddr_storage storage;
    sockaddr_in6 *storagePtrIPv6 = reinterpret_cast<sockaddr_in6 *>(&storage);
    storagePtrIPv6->sin6_port = 0;
#else
    struct sockaddr storage;
#endif
    sockaddr *storagePtr = reinterpret_cast<sockaddr *>(&storage);
    storagePtr->sa_family = 0;

    sockaddr_in *storagePtrIPv4 = reinterpret_cast<sockaddr_in *>(&storage);
    storagePtrIPv4->sin_port = 0;
    QT_SOCKLEN_T storageSize = sizeof(storage);

    // Peek 0 bytes into the next message. The size of the message may
    // well be 0, so we can't check recvfrom's return value.
    ssize_t readBytes;
    do {
        char c;
        readBytes = ::recvfrom(socketDescriptor, &c, 1, MSG_PEEK, storagePtr, &storageSize);
    } while (readBytes == -1 && errno == EINTR);

    // If there's no error, or if our buffer was too small, there must be a
    // pending datagram.
    bool result = (readBytes != -1) || errno == EMSGSIZE;

#if defined (QNATIVESOCKETENGINE_DEBUG)
    qDebug("QNativeSocketEnginePrivate::nativeHasPendingDatagrams() == %s",
           result ? "true" : "false");
#endif
    return result;
}

qint64 QNativeSocketEnginePrivate::nativePendingDatagramSize() const
{
    QVarLengthArray<char, 8192> udpMessagePeekBuffer(8192);
    ssize_t recvResult = -1;
    for (;;) {
        // the data written to udpMessagePeekBuffer is discarded, so
        // this function is still reentrant although it might not look
        // so.
        recvResult = ::recv(socketDescriptor, udpMessagePeekBuffer.data(),
                            udpMessagePeekBuffer.size(), MSG_PEEK);
        if (recvResult == -1 && errno == EINTR)
            continue;

        if (recvResult != (ssize_t) udpMessagePeekBuffer.size())
            break;

        udpMessagePeekBuffer.resize(udpMessagePeekBuffer.size() * 2);
    }

#if defined (QNATIVESOCKETENGINE_DEBUG)
    qDebug("QNativeSocketEnginePrivate::nativePendingDatagramSize() == %i", recvResult);
#endif

    return qint64(recvResult);
}

qint64 QNativeSocketEnginePrivate::nativeReceiveDatagram(char *data, qint64 maxSize,
                                                    QHostAddress *address, quint16 *port)
{
#if !defined(QT_NO_IPV6)
    struct sockaddr_storage aa;
#else
    struct sockaddr_in aa;
#endif
    memset(&aa, 0, sizeof(aa));
    QT_SOCKLEN_T sz;
    sz = sizeof(aa);

    ssize_t recvFromResult = 0;
    do {
        char c;
        recvFromResult = ::recvfrom(socketDescriptor, maxSize ? data : &c, maxSize ? maxSize : 1,
                                    0, (struct sockaddr *)&aa, &sz);
    } while (recvFromResult == -1 && errno == EINTR);

    if (recvFromResult == -1) {
        setError(QAbstractSocket::NetworkError, ReceiveDatagramErrorString);
    } else if (port || address) {
        qt_socket_getPortAndAddress((struct sockaddr *) &aa, port, address);
    }

#if defined (QNATIVESOCKETENGINE_DEBUG)
    qDebug("QNativeSocketEnginePrivate::nativeReceiveDatagram(%p \"%s\", %lli, %s, %i) == %lli",
           data, qt_prettyDebug(data, qMin(recvFromResult, ssize_t(16)), recvFromResult).data(), maxSize,
           address ? address->toString().toLatin1().constData() : "(nil)",
           port ? *port : 0, (qint64) recvFromResult);
#endif

    return qint64(maxSize ? recvFromResult : recvFromResult == -1 ? -1 : 0);
}

qint64 QNativeSocketEnginePrivate::nativeSendDatagram(const char *data, qint64 len,
                                                   const QHostAddress &host, quint16 port)
{
    struct sockaddr_in sockAddrIPv4;
    struct sockaddr *sockAddrPtr = 0;
    QT_SOCKLEN_T sockAddrSize = 0;

#if !defined(QT_NO_IPV6)
    struct sockaddr_in6 sockAddrIPv6;
    if (host.protocol() == QAbstractSocket::IPv6Protocol) {
	memset(&sockAddrIPv6, 0, sizeof(sockAddrIPv6));
	sockAddrIPv6.sin6_family = AF_INET6;
	sockAddrIPv6.sin6_port = htons(port);

	Q_IPV6ADDR tmp = host.toIPv6Address();
	memcpy(&sockAddrIPv6.sin6_addr.s6_addr, &tmp, sizeof(tmp));
	sockAddrSize = sizeof(sockAddrIPv6);
	sockAddrPtr = (struct sockaddr *)&sockAddrIPv6;
    } else
#endif
    if (host.protocol() == QAbstractSocket::IPv4Protocol) {
	memset(&sockAddrIPv4, 0, sizeof(sockAddrIPv4));
	sockAddrIPv4.sin_family = AF_INET;
	sockAddrIPv4.sin_port = htons(port);
	sockAddrIPv4.sin_addr.s_addr = htonl(host.toIPv4Address());
	sockAddrSize = sizeof(sockAddrIPv4);
	sockAddrPtr = (struct sockaddr *)&sockAddrIPv4;
    }

    // ignore the SIGPIPE signal
    qt_ignore_sigpipe();

    ssize_t sentBytes;
    do {
        sentBytes = ::sendto(socketDescriptor, data, len,
                             0, sockAddrPtr, sockAddrSize);
    } while (sentBytes == -1 && errno == EINTR);

    if (sentBytes < 0) {
        switch (errno) {
        case EMSGSIZE:
            setError(QAbstractSocket::DatagramTooLargeError, DatagramTooLargeErrorString);
            break;
        default:
            setError(QAbstractSocket::NetworkError, SendDatagramErrorString);
        }
    }

#if defined (QNATIVESOCKETENGINE_DEBUG)
    qDebug("QNativeSocketEngine::sendDatagram(%p \"%s\", %lli, \"%s\", %i) == %lli", data,
           qt_prettyDebug(data, qMin<int>(len, 16), len).data(), len, host.toString().toLatin1().constData(),
           port, (qint64) sentBytes);
#endif

    return qint64(sentBytes);
}

bool QNativeSocketEnginePrivate::fetchConnectionParameters()
{
    localPort = 0;
    localAddress.clear();
    peerPort = 0;
    peerAddress.clear();

    if (socketDescriptor == -1)
        return false;

#if !defined(QT_NO_IPV6)
    struct sockaddr_storage sa;
#else
    struct sockaddr_in sa;
#endif
    struct sockaddr *sockAddrPtr = (struct sockaddr *) &sa;
    QT_SOCKLEN_T sockAddrSize = sizeof(sa);

    // Determine local address
    memset(&sa, 0, sizeof(sa));
    if (::getsockname(socketDescriptor, sockAddrPtr, &sockAddrSize) == 0) {
        qt_socket_getPortAndAddress(sockAddrPtr, &localPort, &localAddress);

        // Determine protocol family
        switch (sockAddrPtr->sa_family) {
        case AF_INET:
            socketProtocol = QAbstractSocket::IPv4Protocol;
            break;
#if !defined (QT_NO_IPV6)
        case AF_INET6:
            socketProtocol = QAbstractSocket::IPv6Protocol;
            break;
#endif
        default:
            socketProtocol = QAbstractSocket::UnknownNetworkLayerProtocol;
            break;
        }

    } else if (errno == EBADF) {
        setError(QAbstractSocket::UnsupportedSocketOperationError, InvalidSocketErrorString);
        return false;
    }

    // Determine the remote address
    if (!::getpeername(socketDescriptor, sockAddrPtr, &sockAddrSize))
        qt_socket_getPortAndAddress(sockAddrPtr, &peerPort, &peerAddress);

    // Determine the socket type (UDP/TCP)
    int value = 0;
    QT_SOCKOPTLEN_T valueSize = sizeof(int);
    if (::getsockopt(socketDescriptor, SOL_SOCKET, SO_TYPE, &value, &valueSize) == 0) {
        if (value == SOCK_STREAM)
            socketType = QAbstractSocket::TcpSocket;
        else if (value == SOCK_DGRAM)
            socketType = QAbstractSocket::UdpSocket;
        else
            socketType = QAbstractSocket::UnknownSocketType;
    }
#if defined (QNATIVESOCKETENGINE_DEBUG)
    QString socketProtocolStr = "UnknownProtocol";
    if (socketProtocol == QAbstractSocket::IPv4Protocol) socketProtocolStr = "IPv4Protocol";
    else if (socketProtocol == QAbstractSocket::IPv6Protocol) socketProtocolStr = "IPv6Protocol";

    QString socketTypeStr = "UnknownSocketType";
    if (socketType == QAbstractSocket::TcpSocket) socketTypeStr = "TcpSocket";
    else if (socketType == QAbstractSocket::UdpSocket) socketTypeStr = "UdpSocket";

    qDebug("QNativeSocketEnginePrivate::fetchConnectionParameters() local == %s:%i,"
           " peer == %s:%i, socket == %s - %s",
           localAddress.toString().toLatin1().constData(), localPort,
           peerAddress.toString().toLatin1().constData(), peerPort,socketTypeStr.toLatin1().constData(),
           socketProtocolStr.toLatin1().constData());
#endif
    return true;
}

void QNativeSocketEnginePrivate::nativeClose()
{
#if defined (QNATIVESOCKETENGINE_DEBUG)
    qDebug("QNativeSocketEngine::nativeClose()");
#endif
    ::close(socketDescriptor);
}

qint64 QNativeSocketEnginePrivate::nativeWrite(const char *data, qint64 len)
{
    Q_Q(QNativeSocketEngine);

    // ignore the SIGPIPE signal
    qt_ignore_sigpipe();

    // loop while ::write() returns -1 and errno == EINTR, in case
    // of an interrupting signal.
    ssize_t writtenBytes;
    do {
        writtenBytes = ::write(socketDescriptor, data, len);
    } while (writtenBytes < 0 && errno == EINTR);

    if (writtenBytes < 0) {
        switch (errno) {
        case EPIPE:
        case ECONNRESET:
            writtenBytes = -1;
            setError(QAbstractSocket::RemoteHostClosedError, RemoteHostClosedErrorString);
            q->close();
            break;
        case EAGAIN:
            writtenBytes = 0;
            break;
        case EMSGSIZE:
            setError(QAbstractSocket::DatagramTooLargeError, DatagramTooLargeErrorString);
            break;
        default:
            break;
        }
    }

#if defined (QNATIVESOCKETENGINE_DEBUG)
    qDebug("QNativeSocketEnginePrivate::nativeWrite(%p \"%s\", %llu) == %i",
           data, qt_prettyDebug(data, qMin((int) len, 16),
                                (int) len).data(), len, (int) writtenBytes);
#endif

    return qint64(writtenBytes);
}
/*
*/
qint64 QNativeSocketEnginePrivate::nativeRead(char *data, qint64 maxSize)
{
    Q_Q(QNativeSocketEngine);
    if (!q->isValid()) {
        qWarning("QNativeSocketEngine::unbufferedRead: Invalid socket");
        return -1;
    }

    ssize_t r = 0;
    do {
        r = ::read(socketDescriptor, data, maxSize);
    } while (r == -1 && errno == EINTR);

    if (r < 0) {
        r = -1;
        switch (errno) {
#if EWOULDBLOCK-0 && EWOULDBLOCK != EAGAIN
        case EWOULDBLOCK:
#endif
        case EAGAIN:
            // No data was available for reading
            r = -2;
            break;
        case EBADF:
        case EINVAL:
        case EIO:
            setError(QAbstractSocket::NetworkError, ReadErrorString);
            break;
        case ECONNRESET:
            r = 0;
            break;
        default:
            break;
        }
    }

#if defined (QNATIVESOCKETENGINE_DEBUG)
    qDebug("QNativeSocketEnginePrivate::nativeRead(%p \"%s\", %llu) == %i",
           data, qt_prettyDebug(data, qMin(r, ssize_t(16)), r).data(),
           maxSize, r);
#endif

    return qint64(r);
}

int QNativeSocketEnginePrivate::nativeSelect(int timeout, bool selectForRead) const
{
    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(socketDescriptor, &fds);

    struct timeval tv;
    tv.tv_sec = timeout / 1000;
    tv.tv_usec = (timeout % 1000) * 1000;

    QTime timer;
    timer.start();

    int retval;
    do {
        if (selectForRead)
            retval = select(socketDescriptor + 1, &fds, 0, 0, timeout < 0 ? 0 : &tv);
        else
            retval = select(socketDescriptor + 1, 0, &fds, 0, timeout < 0 ? 0 : &tv);

        if (retval != -1 || errno != EINTR)
            break;

        if (timeout > 0) {
            // recalculate the timeout
            timeout -= timer.elapsed();
            tv.tv_sec = timeout / 1000;
            tv.tv_usec = (timeout % 1000) * 1000;

            if (timeout < 0) {
                // oops, timeout turned negative?
                retval = -1;
                break;
            }
        }
    } while (true);

    return retval;
}

int QNativeSocketEnginePrivate::nativeSelect(int timeout, bool checkRead, bool checkWrite,
				      bool *selectForRead, bool *selectForWrite) const
{
    fd_set fdread;
    FD_ZERO(&fdread);
    if (checkRead)
        FD_SET(socketDescriptor, &fdread);

    fd_set fdwrite;
    FD_ZERO(&fdwrite);
    if (checkWrite)
        FD_SET(socketDescriptor, &fdwrite);

    struct timeval tv;
    tv.tv_sec = timeout / 1000;
    tv.tv_usec = (timeout % 1000) * 1000;

    QTime timer;
    timer.start();

    int ret;
    do {
        ret = select(socketDescriptor + 1, &fdread, &fdwrite, 0, timeout < 0 ? 0 : &tv);
        if (ret != -1 || errno != EINTR)
            break;

        if (timeout > 0) {
            // recalculate the timeout
            timeout -= timer.elapsed();
            tv.tv_sec = timeout / 1000;
            tv.tv_usec = (timeout % 1000) * 1000;

            if (timeout < 0) {
                // oops, timeout turned negative?
                ret = -1;
                break;
            }
        }
    } while (true);
    if (ret <= 0)
        return ret;

    *selectForRead = FD_ISSET(socketDescriptor, &fdread);
    *selectForWrite = FD_ISSET(socketDescriptor, &fdwrite);
    return ret;
}

QT_END_NAMESPACE
