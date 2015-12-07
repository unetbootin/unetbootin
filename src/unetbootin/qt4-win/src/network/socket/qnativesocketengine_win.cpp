/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the QtNetwork module of the Qt Toolkit.
**
** This file may be used under the terms of the GNU General Public
** License versions 2.0 or 3.0 as published by the Free Software
** Foundation and appearing in the files LICENSE.GPL2 and LICENSE.GPL3
** included in the packaging of this file.  Alternatively you may (at
** your option) use any later version of the GNU General Public
** License if such license has been publicly approved by Trolltech ASA
** (or its successors, if any) and the KDE Free Qt Foundation. In
** addition, as a special exception, Trolltech gives you certain
** additional rights. These rights are described in the Trolltech GPL
** Exception version 1.2, which can be found at
** http://www.trolltech.com/products/qt/gplexception/ and in the file
** GPL_EXCEPTION.txt in this package.
**
** Please review the following information to ensure GNU General
** Public Licensing requirements will be met:
** http://trolltech.com/products/qt/licenses/licensing/opensource/. If
** you are unsure which license is appropriate for your use, please
** review the following information:
** http://trolltech.com/products/qt/licenses/licensing/licensingoverview
** or contact the sales department at sales@trolltech.com.
**
** In addition, as a special exception, Trolltech, as the sole
** copyright holder for Qt Designer, grants users of the Qt/Eclipse
** Integration plug-in the right for the Qt/Eclipse Integration to
** link to functionality provided by Qt Designer and its related
** libraries.
**
** This file is provided "AS IS" with NO WARRANTY OF ANY KIND,
** INCLUDING THE WARRANTIES OF DESIGN, MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE. Trolltech reserves all rights not expressly
** granted herein.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

#include <winsock2.h>

#include "qnativesocketengine_p.h"

#include <qabstracteventdispatcher.h>
#include <qsocketnotifier.h>
#include <qdebug.h>
#include <qdatetime.h>

//#define QNATIVESOCKETENGINE_DEBUG
#if defined(QNATIVESOCKETENGINE_DEBUG)
#   include <qstring.h>
#   include <qbytearray.h>
#endif

QT_BEGIN_NAMESPACE

#if defined(QNATIVESOCKETENGINE_DEBUG)

void verboseWSErrorDebug(int r)
{
    switch (r) {
        case WSANOTINITIALISED : qDebug("WSA error : WSANOTINITIALISED"); break;
        case WSAEINTR: qDebug("WSA error : WSAEINTR"); break;
        case WSAEBADF: qDebug("WSA error : WSAEBADF"); break;
        case WSAEACCES: qDebug("WSA error : WSAEACCES"); break;
        case WSAEFAULT: qDebug("WSA error : WSAEFAULT"); break;
        case WSAEINVAL: qDebug("WSA error : WSAEINVAL"); break;
        case WSAEMFILE: qDebug("WSA error : WSAEMFILE"); break;
        case WSAEWOULDBLOCK: qDebug("WSA error : WSAEWOULDBLOCK"); break;
        case WSAEINPROGRESS: qDebug("WSA error : WSAEINPROGRESS"); break;
        case WSAEALREADY: qDebug("WSA error : WSAEALREADY"); break;
        case WSAENOTSOCK: qDebug("WSA error : WSAENOTSOCK"); break;
        case WSAEDESTADDRREQ: qDebug("WSA error : WSAEDESTADDRREQ"); break;
        case WSAEMSGSIZE: qDebug("WSA error : WSAEMSGSIZE"); break;
        case WSAEPROTOTYPE: qDebug("WSA error : WSAEPROTOTYPE"); break;
        case WSAENOPROTOOPT: qDebug("WSA error : WSAENOPROTOOPT"); break;
        case WSAEPROTONOSUPPORT: qDebug("WSA error : WSAEPROTONOSUPPORT"); break;
        case WSAESOCKTNOSUPPORT: qDebug("WSA error : WSAESOCKTNOSUPPORT"); break;
        case WSAEOPNOTSUPP: qDebug("WSA error : WSAEOPNOTSUPP"); break;
        case WSAEPFNOSUPPORT: qDebug("WSA error : WSAEPFNOSUPPORT"); break;
        case WSAEAFNOSUPPORT: qDebug("WSA error : WSAEAFNOSUPPORT"); break;
        case WSAEADDRINUSE: qDebug("WSA error : WSAEADDRINUSE"); break;
        case WSAEADDRNOTAVAIL: qDebug("WSA error : WSAEADDRNOTAVAIL"); break;
        case WSAENETDOWN: qDebug("WSA error : WSAENETDOWN"); break;
        case WSAENETUNREACH: qDebug("WSA error : WSAENETUNREACH"); break;
        case WSAENETRESET: qDebug("WSA error : WSAENETRESET"); break;
        case WSAECONNABORTED: qDebug("WSA error : WSAECONNABORTED"); break;
        case WSAECONNRESET: qDebug("WSA error : WSAECONNRESET"); break;
        case WSAENOBUFS: qDebug("WSA error : WSAENOBUFS"); break;
        case WSAEISCONN: qDebug("WSA error : WSAEISCONN"); break;
        case WSAENOTCONN: qDebug("WSA error : WSAENOTCONN"); break;
        case WSAESHUTDOWN: qDebug("WSA error : WSAESHUTDOWN"); break;
        case WSAETOOMANYREFS: qDebug("WSA error : WSAETOOMANYREFS"); break;
        case WSAETIMEDOUT: qDebug("WSA error : WSAETIMEDOUT"); break;
        case WSAECONNREFUSED: qDebug("WSA error : WSAECONNREFUSED"); break;
        case WSAELOOP: qDebug("WSA error : WSAELOOP"); break;
        case WSAENAMETOOLONG: qDebug("WSA error : WSAENAMETOOLONG"); break;
        case WSAEHOSTDOWN: qDebug("WSA error : WSAEHOSTDOWN"); break;
        case WSAEHOSTUNREACH: qDebug("WSA error : WSAEHOSTUNREACH"); break;
        case WSAENOTEMPTY: qDebug("WSA error : WSAENOTEMPTY"); break;
        case WSAEPROCLIM: qDebug("WSA error : WSAEPROCLIM"); break;
        case WSAEUSERS: qDebug("WSA error : WSAEUSERS"); break;
        case WSAEDQUOT: qDebug("WSA error : WSAEDQUOT"); break;
        case WSAESTALE: qDebug("WSA error : WSAESTALE"); break;
        case WSAEREMOTE: qDebug("WSA error : WSAEREMOTE"); break;
        case WSAEDISCON: qDebug("WSA error : WSAEDISCON"); break;
        default: qDebug("WSA error : Unknown"); break;
    }
    qErrnoWarning(r, "more details");
}

/*
    Returns a human readable representation of the first \a len
    characters in \a data.
*/
static QByteArray qt_prettyDebug(const char *data, int len, int maxLength)
{
    if (!data) return "(null)";
    QByteArray out;
    for (int i = 0; i < len; ++i) {
        char c = data[i];
        if (isprint(int(uchar(c)))) {
            out += c;
        } else switch (c) {
        case '\n': out += "\\n"; break;
        case '\r': out += "\\r"; break;
        case '\t': out += "\\t"; break;
        default:
            QString tmp;
            tmp.sprintf("\\%o", c);
            out += tmp.toLatin1().constData();
        }
    }

    if (len < maxLength)
        out += "...";

    return out;
}


#define WS_ERROR_DEBUG(x) verboseWSErrorDebug(x);

#else

#define WS_ERROR_DEBUG(x) Q_UNUSED(x)

#endif

#if !defined (QT_NO_IPV6)

// Use our own defines and structs which we know are correct
#  define QT_SS_MAXSIZE 128
#  define QT_SS_ALIGNSIZE (sizeof(__int64))
#  define QT_SS_PAD1SIZE (QT_SS_ALIGNSIZE - sizeof (short))
#  define QT_SS_PAD2SIZE (QT_SS_MAXSIZE - (sizeof (short) + QT_SS_PAD1SIZE + QT_SS_ALIGNSIZE))
struct qt_sockaddr_storage {
      short ss_family;
      char __ss_pad1[QT_SS_PAD1SIZE];
      __int64 __ss_align;
      char __ss_pad2[QT_SS_PAD2SIZE];
};

// sockaddr_in6 size changed between old and new SDK
// Only the new version is the correct one, so always
// use this structure.
struct qt_in6_addr {
    u_char qt_s6_addr[16];
};
typedef struct {
    short   sin6_family;            /* AF_INET6 */
    u_short sin6_port;              /* Transport level port number */
    u_long  sin6_flowinfo;          /* IPv6 flow information */
    struct  qt_in6_addr sin6_addr;  /* IPv6 address */
    u_long  sin6_scope_id;          /* set of interfaces for a scope */
} qt_sockaddr_in6;

#else

typedef void * qt_sockaddr_in6 ;


#endif

#ifndef AF_INET6
#define AF_INET6        23              /* Internetwork Version 6 */
#endif

#ifndef SO_EXCLUSIVEADDRUSE
#define SO_EXCLUSIVEADDRUSE ((int)(~SO_REUSEADDR)) /* disallow local address reuse */
#endif

//###
#define QT_SOCKLEN_T int
#define QT_SOCKOPTLEN_T int


/*
    Extracts the port and address from a sockaddr, and stores them in
    \a port and \a addr if they are non-null.
*/
static inline void qt_socket_getPortAndAddress(SOCKET socketDescriptor, struct sockaddr *sa, quint16 *port, QHostAddress *address)
{
#if !defined (QT_NO_IPV6)
    if (sa->sa_family == AF_INET6) {
        qt_sockaddr_in6 *sa6 = (qt_sockaddr_in6 *)sa;
        Q_IPV6ADDR tmp;
        for (int i = 0; i < 16; ++i)
            tmp.c[i] = sa6->sin6_addr.qt_s6_addr[i];
        QHostAddress a;
	a.setAddress(tmp);
	if (address)
	    *address = a;
        if (port)
	    WSANtohs(socketDescriptor, sa6->sin6_port, port);
    } else
#endif
    if (sa->sa_family == AF_INET) {
        struct sockaddr_in *sa4 = (struct sockaddr_in *)sa;
        unsigned long addr;
        WSANtohl(socketDescriptor, sa4->sin_addr.s_addr, &addr);
        QHostAddress a;
	a.setAddress(addr);
	if (address)
	    *address = a;
        if (port)
	    WSANtohs(socketDescriptor, sa4->sin_port, port);
    }
}


/*! \internal

    Sets the port and address to a sockaddr. Requires that sa point to the IPv6 struct if the address is IPv6.
*/
static inline void qt_socket_setPortAndAddress(SOCKET socketDescriptor, sockaddr_in * sockAddrIPv4, qt_sockaddr_in6 * sockAddrIPv6,
                                               quint16 port, const QHostAddress & address, sockaddr ** sockAddrPtr, QT_SOCKLEN_T *sockAddrSize)
{
#if !defined(QT_NO_IPV6)
    if (address.protocol() == QAbstractSocket::IPv6Protocol) {
        memset(sockAddrIPv6, 0, sizeof(qt_sockaddr_in6));
        sockAddrIPv6->sin6_family = AF_INET6;
        WSAHtons(socketDescriptor, port, &(sockAddrIPv6->sin6_port));
        Q_IPV6ADDR tmp = address.toIPv6Address();
        memcpy(&(sockAddrIPv6->sin6_addr.qt_s6_addr), &tmp, sizeof(tmp));
        *sockAddrSize = sizeof(qt_sockaddr_in6);
        *sockAddrPtr = (struct sockaddr *) sockAddrIPv6;
    } else
#endif
    if (address.protocol() == QAbstractSocket::IPv4Protocol
        || address.protocol() == QAbstractSocket::UnknownNetworkLayerProtocol) {
        memset(sockAddrIPv4, 0, sizeof(sockaddr_in));
        sockAddrIPv4->sin_family = AF_INET;
        WSAHtons(socketDescriptor, port, &(sockAddrIPv4->sin_port));
        WSAHtonl(socketDescriptor, address.toIPv4Address(), &(sockAddrIPv4->sin_addr.s_addr));
        *sockAddrSize = sizeof(sockaddr_in);
        *sockAddrPtr = (struct sockaddr *) sockAddrIPv4;
    } else {
        // unreachable
    }
}

/*! \internal

*/
static inline QAbstractSocket::SocketType qt_socket_getType(int socketDescriptor)
{
    int value = 0;
    QT_SOCKLEN_T valueSize = sizeof(value);
    if (::getsockopt(socketDescriptor, SOL_SOCKET, SO_TYPE, (char *) &value, &valueSize) != 0) {
        WS_ERROR_DEBUG(WSAGetLastError());
    } else {
        if (value == SOCK_STREAM)
            return QAbstractSocket::TcpSocket;
        else if (value == SOCK_DGRAM)
            return QAbstractSocket::UdpSocket;
    }
    return QAbstractSocket::UnknownSocketType;
}

/*! \internal

*/
static inline int qt_socket_getMaxMsgSize(int socketDescriptor)
{
    int value = 0;
    QT_SOCKLEN_T valueSize = sizeof(value);
    if (::getsockopt(socketDescriptor, SOL_SOCKET, SO_MAX_MSG_SIZE, (char *) &value, &valueSize) != 0) {
        WS_ERROR_DEBUG(WSAGetLastError());
    }
    return value;
}

QWindowsSockInit::QWindowsSockInit()
:   version(0)
{
    //### should we try for 2.2 on all platforms ??
    WSAData wsadata;

    // IPv6 requires Winsock v2.0 or better.
    if (WSAStartup(MAKEWORD(2,0), &wsadata) != 0) {
	qWarning("QTcpSocketAPI: WinSock v2.0 initialization failed.");
    } else {
        version = 0x20;
    }
}

QWindowsSockInit::~QWindowsSockInit()
{
    WSACleanup();
}

// MS Transport Provider IOCTL to control
// reporting PORT_UNREACHABLE messages
// on UDP sockets via recv/WSARecv/etc.
// Path TRUE in input buffer to enable (default if supported),
// FALSE to disable.
#ifndef SIO_UDP_CONNRESET
#  ifndef IOC_VENDOR
#    define IOC_VENDOR 0x18000000
#  endif
#  ifndef _WSAIOW
#    define _WSAIOW(x,y) (IOC_IN|(x)|(y))
#  endif
#  define SIO_UDP_CONNRESET _WSAIOW(IOC_VENDOR,12)
#endif

bool QNativeSocketEnginePrivate::createNewSocket(QAbstractSocket::SocketType socketType, QAbstractSocket::NetworkLayerProtocol socketProtocol)
{

    //### no ip6 support on winsocket 1.1 but we will try not to use this !!!!!!!!!!!!1
    /*
    if (winsockVersion < 0x20 && socketProtocol == QAbstractSocket::IPv6Protocol) {
        //### no ip6 support
        return -1;
    }
    */

    int protocol = (socketProtocol == QAbstractSocket::IPv6Protocol) ? AF_INET6 : AF_INET;
    int type = (socketType == QAbstractSocket::UdpSocket) ? SOCK_DGRAM : SOCK_STREAM;
    // MSDN KB179942 states that on winnt 4 WSA_FLAG_OVERLAPPED is needed if socket is to be non blocking
    // and recomends alwasy doing it for cross windows version comapablity.
    SOCKET socket = ::WSASocket(protocol, type, 0, NULL, 0, WSA_FLAG_OVERLAPPED);

    if (socket == INVALID_SOCKET) {
        int err = WSAGetLastError();
        WS_ERROR_DEBUG(err);
        switch (err) {
        case WSANOTINITIALISED:
            //###
            break;
        case WSAEAFNOSUPPORT:
        case WSAESOCKTNOSUPPORT:
        case WSAEPROTOTYPE:
        case WSAEINVAL:
            setError(QAbstractSocket::UnsupportedSocketOperationError, ProtocolUnsupportedErrorString);
            break;
        case WSAEMFILE:
        case WSAENOBUFS:
            setError(QAbstractSocket::SocketResourceError, ResourceErrorString);
            break;
        default:
            break;
        }

        return false;
    }

#if !defined(Q_OS_WINCE)
    // enable new behavior using
    // SIO_UDP_CONNRESET
    DWORD dwBytesReturned = 0;
    int bNewBehavior = 1;
    if (::WSAIoctl(socket, SIO_UDP_CONNRESET, &bNewBehavior, sizeof(bNewBehavior),
                   NULL, 0, &dwBytesReturned, NULL, NULL) == SOCKET_ERROR) {
        // not to worry isBogusUdpReadNotification() should handle this otherwise
        int err = WSAGetLastError();
        WS_ERROR_DEBUG(err);
    }
#endif

    socketDescriptor = socket;
    return true;

}

/*! \internal

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
    case QNativeSocketEngine::BroadcastSocketOption:
        n = SO_BROADCAST;
        break;
    case QNativeSocketEngine::NonBlockingSocketOption: {
        unsigned long buf = 0;
        if (WSAIoctl(socketDescriptor, FIONBIO, 0,0, &buf, sizeof(buf), 0,0,0) == 0)
            return buf;
        else
            return -1;
        break;
    }
    case QNativeSocketEngine::AddressReusable:
        n = SO_REUSEADDR;
        break;
    case QNativeSocketEngine::BindExclusively:
        n = SO_EXCLUSIVEADDRUSE;
        break;
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


/*! \internal
    Sets the socket option \a opt to \a v.
*/
bool QNativeSocketEnginePrivate::setOption(QNativeSocketEngine::SocketOption opt, int v)
{
    Q_Q(const QNativeSocketEngine);
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
    case QNativeSocketEngine::NonBlockingSocketOption:
        {
        unsigned long buf = v;
        unsigned long outBuf;
        DWORD sizeWritten = 0;
        if (::WSAIoctl(socketDescriptor, FIONBIO, &buf, sizeof(unsigned long), &outBuf, sizeof(unsigned long), &sizeWritten, 0,0) == SOCKET_ERROR) {
            WS_ERROR_DEBUG(WSAGetLastError());
            return false;
        }
        return true;
        break;
        }
    case QNativeSocketEngine::AddressReusable:
        n = SO_REUSEADDR;
        break;
    case QNativeSocketEngine::BindExclusively:
        n = SO_EXCLUSIVEADDRUSE;
        break;
    case QNativeSocketEngine::ReceiveOutOfBandData:
        n = SO_OOBINLINE;
        break;
    }

    if (::setsockopt(socketDescriptor, SOL_SOCKET, n, (char*)&v, sizeof(v)) != 0) {
        WS_ERROR_DEBUG(WSAGetLastError());
        return false;
    }
    return true;
}

/*!
    Fetches information about both ends of the connection: whatever is
    available.
*/
bool QNativeSocketEnginePrivate::fetchConnectionParameters()
{
    localPort = 0;
    localAddress.clear();
    peerPort = 0;
    peerAddress.clear();

    if (socketDescriptor == -1)
       return false;

#if !defined (QT_NO_IPV6)
    struct qt_sockaddr_storage sa;
#else
    struct sockaddr_in sa;
#endif
    struct sockaddr *pSa = (struct sockaddr *) &sa;

    QT_SOCKLEN_T sz = sizeof(sa);

    memset(&sa, 0, sizeof(sa));
    if (::getsockname(socketDescriptor, pSa, &sz) == 0) {
        qt_socket_getPortAndAddress(socketDescriptor, pSa, &localPort, &localAddress);
        // Determine protocol family
        switch (pSa->sa_family) {
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
    } else {
        int err = WSAGetLastError();
        WS_ERROR_DEBUG(err);
        if (err == WSAENOTSOCK) {
            setError(QAbstractSocket::UnsupportedSocketOperationError,
                InvalidSocketErrorString);
            return false;
        }
    }

    memset(&sa, 0, sizeof(sa));
    if (::getpeername(socketDescriptor, pSa, &sz) == 0) {
        qt_socket_getPortAndAddress(socketDescriptor, pSa, &peerPort, &peerAddress);
    } else {
        WS_ERROR_DEBUG(WSAGetLastError());
    }

    socketType = qt_socket_getType(socketDescriptor);

#if defined (QNATIVESOCKETENGINE_DEBUG)
    QString socketProtocolStr = "UnknownProtocol";
    if (socketProtocol == QAbstractSocket::IPv4Protocol) socketProtocolStr = "IPv4Protocol";
    else if (socketProtocol == QAbstractSocket::IPv6Protocol) socketProtocolStr = "IPv6Protocol";

    QString socketTypeStr = "UnknownSocketType";
    if (socketType == QAbstractSocket::TcpSocket) socketTypeStr = "TcpSocket";
    else if (socketType == QAbstractSocket::UdpSocket) socketTypeStr = "UdpSocket";

    qDebug("QNativeSocketEnginePrivate::fetchConnectionParameters() localAddress == %s, localPort = %i, peerAddress == %s, peerPort = %i, socketProtocol == %s, socketType == %s", localAddress.toString().toLatin1().constData(), localPort, peerAddress.toString().toLatin1().constData(), peerPort, socketProtocolStr.toLatin1().constData(), socketTypeStr.toLatin1().constData());
#endif

    return true;
}


bool QNativeSocketEnginePrivate::nativeConnect(const QHostAddress &address, quint16 port)
{

#if defined (QNATIVESOCKETENGINE_DEBUG)
    qDebug("QNativeSocketEnginePrivate::nativeConnect() to %s :: %i", address.toString().toLatin1().constData(), port);
#endif

    struct sockaddr_in sockAddrIPv4;
    qt_sockaddr_in6 sockAddrIPv6;
    struct sockaddr *sockAddrPtr;
    QT_SOCKLEN_T sockAddrSize;

    qt_socket_setPortAndAddress(socketDescriptor, &sockAddrIPv4, &sockAddrIPv6, port, address, &sockAddrPtr, &sockAddrSize);

    forever {
        int connectResult = ::WSAConnect(socketDescriptor, sockAddrPtr, sockAddrSize, 0,0,0,0);
        if (connectResult == SOCKET_ERROR) {
            int err = WSAGetLastError();
            WS_ERROR_DEBUG(err);

            switch (err) {
            case WSANOTINITIALISED:
                //###
                break;
            case WSAEISCONN:
                socketState = QAbstractSocket::ConnectedState;
                break;
            case WSAEWOULDBLOCK: {
                // If WSAConnect returns WSAEWOULDBLOCK on the second
                // connection attempt, we have to check SO_ERROR's
                // value to detect ECONNREFUSED. If we don't get
                // ECONNREFUSED, we'll have to treat it as an
                // unfinished operation.
                int value = 0;
                QT_SOCKLEN_T valueSize = sizeof(value);
                if (::getsockopt(socketDescriptor, SOL_SOCKET, SO_ERROR, (char *) &value, &valueSize) == 0) {
                    if (value == WSAECONNREFUSED) {
                        setError(QAbstractSocket::ConnectionRefusedError, ConnectionRefusedErrorString);
                        socketState = QAbstractSocket::UnconnectedState;
                        break;
                    }
                    if (value == WSAETIMEDOUT) {
                        setError(QAbstractSocket::NetworkError, ConnectionTimeOutErrorString);
                        socketState = QAbstractSocket::UnconnectedState;
                        break;
                    }
                }
                // fall through
            }
            case WSAEINPROGRESS:
                setError(QAbstractSocket::UnfinishedSocketOperationError, InvalidSocketErrorString);
                socketState = QAbstractSocket::ConnectingState;
                break;
            case WSAEADDRINUSE:
                setError(QAbstractSocket::NetworkError, AddressInuseErrorString);
                break;
            case WSAECONNREFUSED:
                setError(QAbstractSocket::ConnectionRefusedError, ConnectionRefusedErrorString);
                socketState = QAbstractSocket::UnconnectedState;
                break;
            case WSAETIMEDOUT:
                setError(QAbstractSocket::NetworkError, ConnectionTimeOutErrorString);
                break;
            case WSAEACCES:
                setError(QAbstractSocket::SocketAccessError, AccessErrorString);
                socketState = QAbstractSocket::UnconnectedState;
                break;
            case WSAEHOSTUNREACH:
                setError(QAbstractSocket::NetworkError, HostUnreachableErrorString);
                socketState = QAbstractSocket::UnconnectedState;
                break;
            case WSAENETUNREACH:
                setError(QAbstractSocket::NetworkError, NetworkUnreachableErrorString);
                socketState = QAbstractSocket::UnconnectedState;
                break;
            case WSAEINVAL:
            case WSAEALREADY:
                setError(QAbstractSocket::UnfinishedSocketOperationError, InvalidSocketErrorString);
                break;
            default:
                break;
            }
            if (socketState != QAbstractSocket::ConnectedState) {
#if defined (QNATIVESOCKETENGINE_DEBUG)
                qDebug("QNativeSocketEnginePrivate::nativeConnect(%s, %i) == false (%s)",
                        address.toString().toLatin1().constData(), port,
                        socketState == QAbstractSocket::ConnectingState
                        ? "Connection in progress" : socketErrorString.toLatin1().constData());
#endif
                return false;
            }
        }
        break;
    }

#if defined (QNATIVESOCKETENGINE_DEBUG)
    qDebug("QNativeSocketEnginePrivate::nativeConnect(%s, %i) == true",
           address.toString().toLatin1().constData(), port);
#endif

    socketState = QAbstractSocket::ConnectedState;
    return true;
}


bool QNativeSocketEnginePrivate::nativeBind(const QHostAddress &address, quint16 port)
{
    struct sockaddr_in sockAddrIPv4;
    qt_sockaddr_in6 sockAddrIPv6;
    struct sockaddr *sockAddrPtr;
    QT_SOCKLEN_T sockAddrSize;

    qt_socket_setPortAndAddress(socketDescriptor, &sockAddrIPv4, &sockAddrIPv6, port, address, &sockAddrPtr, &sockAddrSize);


    int bindResult = ::bind(socketDescriptor, sockAddrPtr, sockAddrSize);
    if (bindResult == SOCKET_ERROR) {
        int err = WSAGetLastError();
        WS_ERROR_DEBUG(err);
        switch (err) {
        case WSANOTINITIALISED:
            //###
            break;
        case WSAEADDRINUSE:
        case WSAEINVAL:
            setError(QAbstractSocket::AddressInUseError, AddressInuseErrorString);
            break;
        case WSAEACCES:
            setError(QAbstractSocket::SocketAccessError, AddressProtectedErrorString);
            break;
        case WSAEADDRNOTAVAIL:
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
    if (::listen(socketDescriptor, backlog) == SOCKET_ERROR) {
        int err = WSAGetLastError();
        WS_ERROR_DEBUG(err);
        switch (err) {
        case WSANOTINITIALISED:
            //###
            break;
        case WSAEADDRINUSE:
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
    int acceptedDescriptor = WSAAccept(socketDescriptor, 0,0,0,0);
	if (acceptedDescriptor != -1 && QAbstractEventDispatcher::instance()) {
		// Becuase of WSAAsyncSelect() WSAAccept returns a non blocking socket
		// with the same attributes as the listening socket including the current
		// WSAAsyncSelect(). To be able to change the socket to blocking mode the
		// WSAAsyncSelect() call must be cancled.
		QSocketNotifier n(acceptedDescriptor, QSocketNotifier::Read);
		n.setEnabled(true);
		n.setEnabled(false);
	}
#if defined (QNATIVESOCKETENGINE_DEBUG)
    qDebug("QNativeSocketEnginePrivate::nativeAccept() == %i", acceptedDescriptor);
#endif
    return acceptedDescriptor;
}


qint64 QNativeSocketEnginePrivate::nativeBytesAvailable() const
{
    unsigned long  nbytes = 0;
    unsigned long dummy = 0;
    DWORD sizeWritten = 0;
    if (::WSAIoctl(socketDescriptor, FIONREAD, &dummy, sizeof(dummy), &nbytes, sizeof(nbytes), &sizeWritten, 0,0) == SOCKET_ERROR) {
        WS_ERROR_DEBUG(WSAGetLastError());
        return -1;
    }

    // ioctlsocket sometimes reports 1 byte available for datagrams
    // while the following recvfrom returns -1 and claims connection
    // was reset (udp is connectionless). so we peek one byte to
    // catch this case and return 0 bytes available if recvfrom
    // fails.
    if (nbytes == 1 && socketType == QAbstractSocket::UdpSocket) {
        char c;
        WSABUF buf;
        buf.buf = &c;
        buf.len = sizeof(c);
        DWORD flags = MSG_PEEK;
        if (::WSARecvFrom(socketDescriptor, &buf, 1, 0, &flags, 0,0,0,0) == SOCKET_ERROR)
            return 0;
    }
    return nbytes;
}


bool QNativeSocketEnginePrivate::nativeHasPendingDatagrams() const
{
#if !defined(Q_OS_WINCE)
    // Create a sockaddr struct and reset its port number.
#if !defined(QT_NO_IPV6)
    qt_sockaddr_in6 storage;
    qt_sockaddr_in6 *storagePtrIPv6 = reinterpret_cast<qt_sockaddr_in6 *>(&storage);
    storagePtrIPv6->sin6_port = 0;
#else
    struct sockaddr storage;
#endif
    sockaddr *storagePtr = reinterpret_cast<sockaddr *>(&storage);
    storagePtr->sa_family = 0;

    sockaddr_in *storagePtrIPv4 = reinterpret_cast<sockaddr_in *>(&storage);
    storagePtrIPv4->sin_port = 0;
    QT_SOCKLEN_T storageSize = sizeof(storage);


    bool result = false;

    // Peek 0 bytes into the next message. The size of the message may
    // well be 0, so we check if there was a sender.
    char c;
    WSABUF buf;
    buf.buf = &c;
    buf.len = sizeof(c);
    DWORD available = 0;
    DWORD flags = MSG_PEEK;
    int ret = ::WSARecvFrom(socketDescriptor, &buf, 1, &available, &flags, storagePtr, &storageSize,0,0);
    int err = WSAGetLastError();
    if (ret == SOCKET_ERROR && err !=  WSAEMSGSIZE) {
        WS_ERROR_DEBUG(err);
        if (err == WSAECONNRESET) {
            // Discard error message to prevent QAbstractSocket from
            // getting this message repeatedly after reenabling the
            // notifiers.
            flags = 0;
            ::WSARecvFrom(socketDescriptor, &buf, 1, &available, &flags,
                          storagePtr, &storageSize, 0, 0);
        }
    } else {
        // If there's no error, or if our buffer was too small, there must be
        // a pending datagram.
        result = true;
    }

#else // Q_OS_WINCE
    bool result = false;
    fd_set readS;
    FD_ZERO(&readS);
    FD_SET(socketDescriptor, &readS);
    timeval timeout;
    timeout.tv_sec = 0;
    timeout.tv_usec = 5000;
    int available = ::select(1, &readS, 0, 0, &timeout);
    result = available > 0 ? true : false;
#endif

#if defined (QNATIVESOCKETENGINE_DEBUG)
    qDebug("QNativeSocketEnginePrivate::nativeHasPendingDatagrams() == %s",
           result ? "true" : "false");
#endif
    return result;
}


qint64 QNativeSocketEnginePrivate::nativePendingDatagramSize() const
{
    qint64 ret = -1;
#if !defined(Q_OS_WINCE)
    int recvResult = 0;
    DWORD flags;
    DWORD bufferCount = 5;
    WSABUF * buf = 0;
    for (;;) {
        // the data written to udpMessagePeekBuffer is discarded, so
        // this function is still reentrant although it might not look
        // so.
        static char udpMessagePeekBuffer[8192];

        buf = new WSABUF[bufferCount];
        for (DWORD i=0; i<bufferCount; i++) {
           buf[i].buf = udpMessagePeekBuffer;
           buf[i].len = sizeof(udpMessagePeekBuffer);
        }
        flags = MSG_PEEK;
        DWORD bytesRead = 0;
        recvResult = ::WSARecv(socketDescriptor, buf, bufferCount, &bytesRead, &flags, 0,0);
        int err = WSAGetLastError();
        if (recvResult != SOCKET_ERROR) {
            ret = qint64(bytesRead);
            break;
        } else if (recvResult == SOCKET_ERROR && err == WSAEMSGSIZE) {
           bufferCount += 5;
           delete[] buf;
        } else if (recvResult == SOCKET_ERROR) {
            WS_ERROR_DEBUG(err);
            ret = -1;
            break;
        }
    }

    if (buf)
        delete[] buf;

#else // Q_OS_WINCE
    DWORD size = -1;
    DWORD bytesReturned;
    int ioResult = WSAIoctl(socketDescriptor, FIONREAD, 0,0, &size, sizeof(size), &bytesReturned, 0, 0);
    if (ioResult == SOCKET_ERROR) {
        int err = WSAGetLastError();
        WS_ERROR_DEBUG(err);
    } else {
        ret = qint64(size);
    }
#endif

#if defined (QNATIVESOCKETENGINE_DEBUG)
    qDebug("QNativeSocketEnginePrivate::nativePendingDatagramSize() == %li", ret);
#endif

    return ret;
}


qint64 QNativeSocketEnginePrivate::nativeReceiveDatagram(char *data, qint64 maxLength,
                                                      QHostAddress *address, quint16 *port)
{
    qint64 ret = 0;

#if !defined(QT_NO_IPV6)
    qt_sockaddr_storage aa;
#else
    struct sockaddr_in aa;
#endif
    memset(&aa, 0, sizeof(aa));
    QT_SOCKLEN_T sz;
    sz = sizeof(aa);
    WSABUF buf;
    buf.buf = data;
    buf.len = maxLength;
#if !defined(Q_OS_WINCE)
    buf.buf = data;
    buf.len = maxLength;
#else
    char tmpChar;
    buf.buf = data ? data : &tmpChar;
    buf.len = maxLength;
#endif

    DWORD flags = 0;
    DWORD bytesRead = 0;
    int wsaRet = ::WSARecvFrom(socketDescriptor, &buf, 1, &bytesRead, &flags, (struct sockaddr *) &aa, &sz,0,0);
    if (wsaRet == SOCKET_ERROR) {
        int err = WSAGetLastError();
        if (err == WSAEMSGSIZE) {
            // it is ok the buffer was to small if bytesRead is larger than
            // maxLength (win 9x) then assume bytes read is really maxLenth
            ret = qint64(bytesRead) > maxLength ? maxLength : qint64(bytesRead);
        } else {
            WS_ERROR_DEBUG(err);
            setError(QAbstractSocket::NetworkError, ReceiveDatagramErrorString);
            ret = -1;
        }
    } else {
        ret = qint64(bytesRead);
    }

    qt_socket_getPortAndAddress(socketDescriptor, (struct sockaddr *) &aa, port, address);

#if defined (QNATIVESOCKETENGINE_DEBUG)
    qDebug("QNativeSocketEnginePrivate::nativeReceiveDatagram(%p \"%s\", %li, %s, %i) == %li",
           data, qt_prettyDebug(data, qMin<qint64>(ret, 16), ret).data(), maxLength,
           address ? address->toString().toLatin1().constData() : "(nil)",
           port ? *port : 0, ret);
#endif

    return ret;
}


qint64 QNativeSocketEnginePrivate::nativeSendDatagram(const char *data, qint64 len,
                                                   const QHostAddress &address, quint16 port)
{
    qint64 ret = -1;
    struct sockaddr_in sockAddrIPv4;
    qt_sockaddr_in6 sockAddrIPv6;
    struct sockaddr *sockAddrPtr;
    QT_SOCKLEN_T sockAddrSize;

    qt_socket_setPortAndAddress(socketDescriptor, &sockAddrIPv4, &sockAddrIPv6, port, address, &sockAddrPtr, &sockAddrSize);

    if (QSysInfo::WindowsVersion & QSysInfo::WV_DOS_based && len > qint64(qt_socket_getMaxMsgSize(socketDescriptor))) {
        // WSAEMSGSIZE is not reliable enough (win 9x) so we check max size our self.
        setError(QAbstractSocket::DatagramTooLargeError, DatagramTooLargeErrorString);
    } else {
        WSABUF buf;
#if !defined(Q_OS_WINCE)
        buf.buf = len ? (char*)data : 0;
#else
        char tmp;
        buf.buf = len ? (char*)data : &tmp;
#endif
        buf.len = len;
        DWORD flags = 0;
        DWORD bytesSent = 0;
        if (::WSASendTo(socketDescriptor, &buf, 1, &bytesSent, flags, sockAddrPtr, sockAddrSize, 0,0) ==  SOCKET_ERROR) {
            int err = WSAGetLastError();
            WS_ERROR_DEBUG(err);
            switch (err) {
            case WSAEMSGSIZE:
                setError(QAbstractSocket::DatagramTooLargeError, DatagramTooLargeErrorString);
                break;
            default:
                setError(QAbstractSocket::NetworkError, SendDatagramErrorString);
                break;
            }
            ret = -1;
        } else {
            ret = qint64(bytesSent);
        }
    }
#if defined (QNATIVESOCKETENGINE_DEBUG)
    qDebug("QNativeSocketEnginePrivate::nativeSendDatagram(%p \"%s\", %li, \"%s\", %i) == %li", data,
           qt_prettyDebug(data, qMin<qint64>(len, 16), len).data(), 0, address.toString().toLatin1().constData(),
           port, ret);
#endif

    return ret;
}


qint64 QNativeSocketEnginePrivate::nativeWrite(const char *data, qint64 len)
{
    Q_Q(QNativeSocketEngine);
    qint64 ret = 0;
    // don't send more than 49152 per call to WSASendTo to avoid getting a WSAENOBUFS
    for (;;) {
        qint64 bytesToSend = qMin<qint64>(49152, len - ret);
        WSABUF buf;
        buf.buf = (char*)data + ret;
        buf.len = bytesToSend;
        DWORD flags = 0;
        DWORD bytesWritten = 0;

        int socketRet = ::WSASend(socketDescriptor, &buf, 1, &bytesWritten, flags, 0,0);

        ret += qint64(bytesWritten);

        if (socketRet != SOCKET_ERROR) {
            if (ret == len)
                break;
            else
                continue;
        } else if (WSAGetLastError() == WSAEWOULDBLOCK) {
            break;
        } else {
            int err = WSAGetLastError();
            WS_ERROR_DEBUG(err);
            switch (err) {
            case WSAECONNRESET:
            case WSAECONNABORTED:
                ret = -1;
                setError(QAbstractSocket::NetworkError, WriteErrorString);
                q->close();
                break;
            default:
                break;
            }
            break;
        }
    }

#if defined (QNATIVESOCKETENGINE_DEBUG)
    qDebug("QNativeSocketEnginePrivate::nativeWrite(%p \"%s\", %li) == %li",
           data, qt_prettyDebug(data, qMin((int)ret, 16), (int)ret).data(), (int)len, (int)ret);
#endif

    return ret;
}

qint64 QNativeSocketEnginePrivate::nativeRead(char *data, qint64 maxLength)
{
    qint64 ret = -1;
    WSABUF buf;
    buf.buf = data;
    buf.len = maxLength;
    DWORD flags = 0;
    DWORD bytesRead = 0;
#if defined(Q_OS_WINCE)
    WSASetLastError(0);
#endif
    if (::WSARecv(socketDescriptor, &buf, 1, &bytesRead, &flags, 0,0) ==  SOCKET_ERROR) {
        int err = WSAGetLastError();
        WS_ERROR_DEBUG(err);
        switch (err) {
        case WSAEWOULDBLOCK:
            ret = -2;
            break;
        case WSAEBADF:
        case WSAEINVAL:
            setError(QAbstractSocket::NetworkError, ReadErrorString);
            break;
        case WSAECONNRESET:
        case WSAECONNABORTED:
            // for tcp sockets this will be handled in QNativeSocketEngine::read
            ret = 0;
            break;
        default:
            break;
        }
    } else {
        if (WSAGetLastError() == WSAEWOULDBLOCK)
            ret = -2;
        else
            ret = qint64(bytesRead);
    }

#if defined (QNATIVESOCKETENGINE_DEBUG)
    if (ret != -2) {
        qDebug("QNativeSocketEnginePrivate::nativeRead(%p \"%s\", %l) == %li",
               data, qt_prettyDebug(data, qMin((int)bytesRead, 16), (int)bytesRead).data(), (int)maxLength, (int)ret);
    } else {
        qDebug("QNativeSocketEnginePrivate::nativeRead(%p, %l) == -2 (WOULD BLOCK)",
               data, int(maxLength));
    }
#endif

    return ret;
}

int QNativeSocketEnginePrivate::nativeSelect(int timeout, bool selectForRead) const
{
    bool readEnabled = selectForRead && readNotifier && readNotifier->isEnabled();
    if (readEnabled)
        readNotifier->setEnabled(false);
    
    fd_set fds;

    int ret = 0;

    memset(&fds, 0, sizeof(fd_set));
    fds.fd_count = 1;
    fds.fd_array[0] = socketDescriptor;

    struct timeval tv;
    tv.tv_sec = timeout / 1000;
    tv.tv_usec = (timeout % 1000) * 1000;

    if (selectForRead)
        ret = select(0, &fds, 0, 0, timeout < 0 ? 0 : &tv);
    else
        ret = select(0, 0, &fds, 0, timeout < 0 ? 0 : &tv);

    if (readEnabled)
        readNotifier->setEnabled(true);

    return ret;
}

int QNativeSocketEnginePrivate::nativeSelect(int timeout,
                                      bool checkRead, bool checkWrite,
                                      bool *selectForRead, bool *selectForWrite) const
{
    bool readEnabled = checkRead && readNotifier && readNotifier->isEnabled();
    if (readEnabled)
        readNotifier->setEnabled(false);
    
    fd_set fdread;
    fd_set fdwrite;

    int ret = 0;

    memset(&fdread, 0, sizeof(fd_set));
    if (checkRead) {
        fdread.fd_count = 1;
        fdread.fd_array[0] = socketDescriptor;
    }
    memset(&fdwrite, 0, sizeof(fd_set));
    if (checkWrite) {
        fdwrite.fd_count = 1;
        fdwrite.fd_array[0] = socketDescriptor;
    }

    struct timeval tv;
    tv.tv_sec = timeout / 1000;
    tv.tv_usec = (timeout % 1000) * 1000;

#if !defined(Q_OS_WINCE)
    ret = select(socketDescriptor + 1, &fdread, &fdwrite, 0, timeout < 0 ? 0 : &tv);
#else
    ret = select(1, &fdread, &fdwrite, 0, timeout < 0 ? 0 : &tv);
#endif
    if (ret <= 0)
        return ret;

    *selectForRead = FD_ISSET(socketDescriptor, &fdread);
    *selectForWrite = FD_ISSET(socketDescriptor, &fdwrite);

    if (readEnabled)
        readNotifier->setEnabled(true);

    return ret;
}

void QNativeSocketEnginePrivate::nativeClose()
{
#if defined (QTCPSOCKETENGINE_DEBUG)
    qDebug("QNativeSocketEnginePrivate::nativeClose()");
#endif
    linger l = {1, 0};
    ::setsockopt(socketDescriptor, SOL_SOCKET, SO_DONTLINGER, (char*)&l, sizeof(l));
    ::closesocket(socketDescriptor);
}

QT_END_NAMESPACE
