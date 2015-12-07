/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the Qt3Support module of the Qt Toolkit.
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

#include "q3socketdevice.h"
#include "qwindowdefs.h"
#include "qdatetime.h"

#include <qcoreapplication.h>

#include <string.h>

#if defined (QT_NO_IPV6)
#  include <windows.h>
#  include <winsock.h>
#else
#  if defined (Q_CC_BOR) || defined (Q_CC_GNU)
#    include <winsock2.h>
#  elif defined (Q_CC_INTEL)
#    include <winsock.h>
#  else
#    include <windows.h>
#  if defined(Q_OS_WINCE)
#    include <winsock.h>
#  endif
#  endif
// Use our own defines and structs which we know are correct
#  define QT_SS_MAXSIZE 128
#  define QT_SS_ALIGNSIZE (sizeof(__int64))
#  define QT_SS_PAD1SIZE (QT_SS_ALIGNSIZE - sizeof (short))
#  define QT_SS_PAD2SIZE (QT_SS_MAXSIZE - (sizeof (short) + QT_SS_PAD1SIZE + QT_SS_ALIGNSIZE))

QT_BEGIN_NAMESPACE

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
#endif

#ifndef AF_INET6
#define AF_INET6        23              /* Internetwork Version 6 */
#endif

#ifndef NO_ERRNO_H
QT_BEGIN_INCLUDE_NAMESPACE
#  if defined(Q_OS_WINCE)
#     include "qfunctions_wince.h"
#  else
#     include <errno.h>
#  endif
QT_END_INCLUDE_NAMESPACE
#endif


#if defined(SOCKLEN_T)
#undef SOCKLEN_T
#endif

#define SOCKLEN_T int // #### Winsock 1.1

static int initialized = 0x00; // Holds the Winsock version

static void cleanupWinSock() // post-routine
{
    WSACleanup();
    initialized = 0x00;
}

static inline void qt_socket_getportaddr( struct sockaddr *sa,
					  quint16 *port, QHostAddress *addr )
{
#if !defined (QT_NO_IPV6)
    if (sa->sa_family == AF_INET6) {
	qt_sockaddr_in6 *sa6 = (qt_sockaddr_in6 *)sa;
	Q_IPV6ADDR tmp;
	for ( int i = 0; i < 16; ++i )
	    tmp.c[i] = sa6->sin6_addr.qt_s6_addr[i];
	QHostAddress a( tmp );
        *addr = a;
        *port = ntohs( sa6->sin6_port );
	return;
    }
#endif
    struct sockaddr_in *sa4 = (struct sockaddr_in *)sa;
    QHostAddress a( ntohl( sa4->sin_addr.s_addr ) );
    *port = ntohs( sa4->sin_port );
    *addr = a;
}

void Q3SocketDevice::init()
{
#if !defined(QT_NO_IPV6)
    if ( !initialized ) {
	WSAData wsadata;
	// IPv6 requires Winsock v2.0 or better.
	if ( WSAStartup( MAKEWORD(2,0), &wsadata ) != 0 ) {
#  if defined(QSOCKETDEVICE_DEBUG)
	    qDebug( "Q3SocketDevice: WinSock v2.0 initialization failed, disabling IPv6 support." );
#  endif
	} else {
	    qAddPostRoutine( cleanupWinSock );
	    initialized = 0x20;
	    return;
	}
    }
#endif

    if (!initialized) {
	WSAData wsadata;
	if ( WSAStartup( MAKEWORD(1,1), &wsadata ) != 0 ) {
#if defined(QT_CHECK_NULL)
	    qWarning( "Q3SocketDevice: WinSock initialization failed" );
#endif
#if defined(QSOCKETDEVICE_DEBUG)
	    qDebug( "Q3SocketDevice: WinSock initialization failed"  );
#endif
	    return;
	}
	qAddPostRoutine( cleanupWinSock );
	initialized = 0x11;
    }
}

Q3SocketDevice::Protocol Q3SocketDevice::getProtocol() const
{
    if ( isValid() ) {
#if !defined (QT_NO_IPV6)
	struct qt_sockaddr_storage sa;
#else
	struct sockaddr_in sa;
#endif
	memset( &sa, 0, sizeof(sa) );
	SOCKLEN_T sz = sizeof( sa );
	if ( !::getsockname(fd, (struct sockaddr *)&sa, &sz) ) {
#if !defined (QT_NO_IPV6)
	    switch ( sa.ss_family ) {
		case AF_INET:
		    return IPv4;
		case AF_INET6:
		    return IPv6;
		default:
		    return Unknown;
	    }
#else
	    switch ( sa.sin_family ) {
		case AF_INET:
		    return IPv4;
		default:
		    return Unknown;
	    }
#endif
	}
    }
    return Unknown;
}

int Q3SocketDevice::createNewSocket( )
{
#if !defined(QT_NO_IPV6)
    SOCKET s;
    // Support IPv6 for Winsock v2.0++
    if ( initialized >= 0x20 && protocol() == IPv6 ) {
	s = ::socket( AF_INET6, t==Datagram?SOCK_DGRAM:SOCK_STREAM, 0 );
    } else {
	s = ::socket( AF_INET, t==Datagram?SOCK_DGRAM:SOCK_STREAM, 0 );
    }
#else
    SOCKET s = ::socket( AF_INET, t==Datagram?SOCK_DGRAM:SOCK_STREAM, 0 );
#endif
    if ( s == INVALID_SOCKET ) {
	switch( WSAGetLastError() ) {
	    case WSANOTINITIALISED:
		e = Impossible;
		break;
	    case WSAENETDOWN:
		// ### what to use here?
		e = NetworkFailure;
		//e = Inaccessible;
		break;
	    case WSAEMFILE:
		e = NoFiles; // special case for this
		break;
	    case WSAEINPROGRESS:
	    case WSAENOBUFS:
		e = NoResources;
		break;
	    case WSAEAFNOSUPPORT:
	    case WSAEPROTOTYPE:
	    case WSAEPROTONOSUPPORT:
	    case WSAESOCKTNOSUPPORT:
		e = InternalError;
		break;
	    default:
		e = UnknownError;
		break;
	}
    } else {
	return s;
    }
    return -1;
}


void Q3SocketDevice::close()
{
    if ( fd == -1 || !isOpen() )		// already closed
	return;
    resetStatus();
    setOpenMode(NotOpen);
    ::closesocket( fd );
#if defined(QSOCKETDEVICE_DEBUG)
    qDebug( "Q3SocketDevice::close: Closed socket %x", fd );
#endif
    fd = -1;
    fetchConnectionParameters();
    QIODevice::close();
}


bool Q3SocketDevice::blocking() const
{
    return true;
}


void Q3SocketDevice::setBlocking( bool enable )
{
#if defined(QSOCKETDEVICE_DEBUG)
    qDebug( "Q3SocketDevice::setBlocking( %d )", enable );
#endif
    if ( !isValid() )
	return;

    unsigned long dummy = enable ? 0 : 1;
    ioctlsocket( fd, FIONBIO, &dummy );
}


int Q3SocketDevice::option( Option opt ) const
{
    if ( !isValid() )
	return -1;
    int n = -1;
    int v = -1;
    switch ( opt ) {
	case Broadcast:
	    n = SO_BROADCAST;
	    break;
	case ReceiveBuffer:
	    n = SO_RCVBUF;
	    break;
	case ReuseAddress:
	    n = SO_REUSEADDR;
	    break;
	case SendBuffer:
	    n = SO_SNDBUF;
	    break;
    }
    if ( n != -1 ) {
	SOCKLEN_T len = sizeof(v);
	int r = ::getsockopt( fd, SOL_SOCKET, n, (char*)&v, &len );
	if ( r != SOCKET_ERROR )
	    return v;
	if ( !e ) {
            Q3SocketDevice *that = (Q3SocketDevice*)this; // mutable function
	    switch( WSAGetLastError() ) {
		case WSANOTINITIALISED:
		    that->e = Impossible;
		    break;
		case WSAENETDOWN:
		    that->e = NetworkFailure;
		    break;
		case WSAEFAULT:
		case WSAEINVAL:
		case WSAENOPROTOOPT:
		    that->e = InternalError;
		    break;
		case WSAEINPROGRESS:
		    that->e = NoResources;
		    break;
		case WSAENOTSOCK:
		    that->e = Impossible;
		    break;
		default:
		    that->e = UnknownError;
		    break;
	    }
	}
	return -1;
    }
    return v;
}


void Q3SocketDevice::setOption( Option opt, int v )
{
    if ( !isValid() )
	return;
    int n = -1; // for really, really bad compilers
    switch ( opt ) {
	case Broadcast:
	    n = SO_BROADCAST;
	    break;
	case ReceiveBuffer:
	    n = SO_RCVBUF;
	    break;
	case ReuseAddress:
	    n = SO_REUSEADDR;
	    break;
	case SendBuffer:
	    n = SO_SNDBUF;
	    break;
	default:
	    return;
    }
    int r = ::setsockopt( fd, SOL_SOCKET, n, (char*)&v, sizeof(v) );
    if ( r == SOCKET_ERROR && e == NoError ) {
	switch( WSAGetLastError() ) {
	    case WSANOTINITIALISED:
		e = Impossible;
		break;
	    case WSAENETDOWN:
		e = NetworkFailure;
		break;
	    case WSAEFAULT:
	    case WSAEINVAL:
	    case WSAENOPROTOOPT:
		e = InternalError;
		break;
	    case WSAEINPROGRESS:
		e = NoResources;
		break;
	    case WSAENETRESET:
	    case WSAENOTCONN:
		e =  Impossible; // ### ?
		break;
	    case WSAENOTSOCK:
		e = Impossible;
		break;
	    default:
		e = UnknownError;
		break;
	}
    }
}


bool Q3SocketDevice::connect( const QHostAddress &addr, quint16 port )
{
    if ( !isValid() )
	return false;

    pa = addr;
    pp = port;

    struct sockaddr_in a4;
    struct sockaddr *aa;
    SOCKLEN_T aalen;

#if !defined(QT_NO_IPV6)
    qt_sockaddr_in6 a6;

    if ( initialized >= 0x20 && addr.isIPv6Address() ) {
        memset(&a6, 0, sizeof(a6));
	a6.sin6_family = AF_INET6;
	a6.sin6_port = htons( port );
	Q_IPV6ADDR ip6 = addr.toIPv6Address();
	memcpy( &a6.sin6_addr.qt_s6_addr, &ip6, sizeof(ip6) );

	aalen = sizeof( a6 );
	aa = (struct sockaddr *)&a6;
    } else
#endif
    if ( addr.isIPv4Address() ) {
	memset(&a4, 0, sizeof(a4));
	a4.sin_family = AF_INET;
	a4.sin_port = htons(port);
	a4.sin_addr.s_addr = htonl(addr.toIPv4Address());

	aalen = sizeof(a4);
	aa = (struct sockaddr *)&a4;
    } else {
	e = Impossible;
	return false;
    }

    int r = ::connect( fd, aa, aalen );

    if ( r == SOCKET_ERROR )
    {
	switch( WSAGetLastError() ) {
	    case WSANOTINITIALISED:
		e = Impossible;
		break;
	    case WSAENETDOWN:
		e = NetworkFailure;
		break;
	    case WSAEADDRINUSE:
	    case WSAEINPROGRESS:
	    case WSAENOBUFS:
		e = NoResources;
		break;
	    case WSAEINTR:
		e = UnknownError; // ### ?
		break;
	    case WSAEALREADY:
		// ### ?
		break;
	    case WSAEADDRNOTAVAIL:
		e = ConnectionRefused; // ### ?
		break;
	    case WSAEAFNOSUPPORT:
	    case WSAEFAULT:
		e = InternalError;
		break;
	    case WSAEINVAL:
		break;
	    case WSAECONNREFUSED:
		e = ConnectionRefused;
		break;
	    case WSAEISCONN:
		goto successful;
	    case WSAENETUNREACH:
	    case WSAETIMEDOUT:
		e = NetworkFailure;
		break;
	    case WSAENOTSOCK:
		e = Impossible;
		break;
	    case WSAEWOULDBLOCK:
		break;
	    case WSAEACCES:
		e = Inaccessible;
		break;
	    case 10107:
		// Workaround for a problem with the WinSock Proxy Server. See
		// also support/arc-12/25557 for details on the problem.
		goto successful;
	    default:
		e = UnknownError;
		break;
	}
	return false;
    }
successful:
    fetchConnectionParameters();
    return true;
}


bool Q3SocketDevice::bind( const QHostAddress &address, quint16 port )
{
    if ( !isValid() )
	return false;
    int r;
    struct sockaddr_in a4;
#if !defined(QT_NO_IPV6)
    qt_sockaddr_in6 a6;

    if ( initialized >= 0x20 && address.isIPv6Address() ) {
	memset( &a6, 0, sizeof(a6) );
	a6.sin6_family = AF_INET6;
	a6.sin6_port = htons( port );
	Q_IPV6ADDR tmp = address.toIPv6Address();
	memcpy( &a6.sin6_addr.qt_s6_addr, &tmp, sizeof(tmp) );

	r = ::bind( fd, (struct sockaddr *)&a6, sizeof(struct qt_sockaddr_storage) );
    } else
#endif
    if ( address.isIPv4Address() ) {
	memset( &a4, 0, sizeof(a4) );
	a4.sin_family = AF_INET;
	a4.sin_port = htons( port );
	a4.sin_addr.s_addr = htonl( address.toIPv4Address() );

	r = ::bind( fd, (struct sockaddr*)&a4, sizeof(struct sockaddr_in) );
    } else {
	e = Impossible;
	return false;
    }

    if ( r == SOCKET_ERROR ) {
	switch( WSAGetLastError() ) {
	    case WSANOTINITIALISED:
		e = Impossible;
		break;
	    case WSAENETDOWN:
		e = NetworkFailure;
		break;
	    case WSAEACCES:
		e = Inaccessible;
		break;
	    case WSAEADDRNOTAVAIL:
		e = Inaccessible;
		break;
	    case WSAEFAULT:
		e = InternalError;
		break;
	    case WSAEINPROGRESS:
	    case WSAENOBUFS:
		e = NoResources;
		break;
	    case WSAEADDRINUSE:
	    case WSAEINVAL:
		e = AlreadyBound;
		break;
	    case WSAENOTSOCK:
		e = Impossible;
		break;
	    default:
		e = UnknownError;
		break;
	}
	return false;
    }
    fetchConnectionParameters();
    return true;
}


bool Q3SocketDevice::listen( int backlog )
{
    if ( !isValid() )
	return false;
    if ( ::listen( fd, backlog ) >= 0 )
	return true;
    if ( !e )
	e = Impossible;
    return false;
}


int Q3SocketDevice::accept()
{
    if ( !isValid() )
	return -1;
#if !defined(QT_NO_IPV6)
    struct qt_sockaddr_storage a;
#else
    struct sockaddr a;
#endif
    SOCKLEN_T l = sizeof(a);
    bool done;
    SOCKET s;
    do {
        s = ::accept( fd, (struct sockaddr*)&a, &l );
        // we'll blithely throw away the stuff accept() wrote to a
        done = true;
        if ( s == INVALID_SOCKET && e == NoError ) {
	    switch( WSAGetLastError() ) {
                case WSAEINTR:
                    done = false;
                    break;
		case WSANOTINITIALISED:
		    e = Impossible;
		    break;
		case WSAENETDOWN:
		case WSAEOPNOTSUPP:
		    // in all these cases, an error happened during connection
		    // setup.  we're not interested in what happened, so we
		    // just treat it like the client-closed-quickly case.
		    break;
		case WSAEFAULT:
		    e = InternalError;
		    break;
		case WSAEMFILE:
		case WSAEINPROGRESS:
		case WSAENOBUFS:
		    e = NoResources;
		    break;
		case WSAEINVAL:
		case WSAENOTSOCK:
		    e = Impossible;
		    break;
		case WSAEWOULDBLOCK:
		    break;
		default:
		    e = UnknownError;
		    break;
            }
	}
    } while (!done);
    return s;
}


qint64 Q3SocketDevice::bytesAvailable() const
{
    if ( !isValid() )
	return -1;
    u_long nbytes = 0;
    if ( ::ioctlsocket(fd, FIONREAD, &nbytes) < 0 )
	return -1;

     // ioctlsocket sometimes reports 1 byte available for datagrams
     // while the following recvfrom returns -1 and claims connection
     // was reset (udp is connectionless). so we peek one byte to
     // catch this case and return 0 bytes available if recvfrom
     // fails.
     if (nbytes == 1 && t == Datagram) {
         char c;
         if (::recvfrom(fd, &c, sizeof(c), MSG_PEEK, 0, 0) == SOCKET_ERROR)
             return 0;
     }

    return nbytes;
}


Q_LONG Q3SocketDevice::waitForMore( int msecs, bool *timeout ) const
{
    if ( !isValid() )
	return -1;

    fd_set fds;
    memset(&fds, 0, sizeof(fd_set));
    fds.fd_count = 1;
    fds.fd_array[0] = fd;

    struct timeval tv;

    tv.tv_sec = msecs / 1000;
    tv.tv_usec = (msecs % 1000) * 1000;

    int rv = select( fd+1, &fds, 0, 0, msecs < 0 ? 0 : &tv );

    if ( rv < 0 )
	return -1;

    if ( timeout ) {
	if ( rv == 0 )
	    *timeout = true;
	else
	    *timeout = false;
    }

    return bytesAvailable();
}


qint64 Q3SocketDevice::readData( char *data, qint64 maxlen )
{
#if defined(QT_CHECK_NULL)
    if ( data == 0 && maxlen != 0 ) {
	qWarning( "Q3SocketDevice::readBlock: Null pointer error" );
    }
#endif
#if defined(QT_CHECK_STATE)
    if ( !isValid() ) {
	qWarning( "Q3SocketDevice::readBlock: Invalid socket" );
	return -1;
    }
    if ( !isOpen() ) {
	qWarning( "Q3SocketDevice::readBlock: Device is not open" );
	return -1;
    }
    if ( !isReadable() ) {
	qWarning( "Q3SocketDevice::readBlock: Read operation not permitted" );
	return -1;
    }
#endif
    qint64 r = 0;
    if ( t == Datagram ) {
#if !defined(QT_NO_IPV6)
	// With IPv6 support, we must be prepared to receive both IPv4
	// and IPv6 packets. The generic SOCKADDR_STORAGE (struct
	// sockaddr_storage on unix) replaces struct sockaddr.
	struct qt_sockaddr_storage a;
#else
	struct sockaddr_in a;
#endif
	memset( &a, 0, sizeof(a) );
	SOCKLEN_T sz;
	sz = sizeof( a );
	r = ::recvfrom( fd, data, maxlen, 0, (struct sockaddr *)&a, &sz );
	qt_socket_getportaddr( (struct sockaddr *)(&a), &pp, &pa );
    } else {
	r = ::recv( fd, data, maxlen, 0 );
    }
    if ( r == 0 && t == Stream && maxlen > 0 ) {
        if ( WSAGetLastError() != WSAEWOULDBLOCK ) {
            // connection closed
            close();
        }
    } else if ( r == SOCKET_ERROR && e == NoError ) {
	switch( WSAGetLastError() ) {
	    case WSANOTINITIALISED:
		e = Impossible;
		break;
	    case WSAECONNABORTED:
	    	close();
		r = 0;
		break;
	    case WSAETIMEDOUT:
	    case WSAECONNRESET:
		/*
		From msdn doc:
		On a UDP datagram socket this error would indicate that a previous
		send operation resulted in an ICMP "Port Unreachable" message.

		So we should not close this socket just because one sendto failed.
		*/
		if ( t != Datagram )
		    close(); // connection closed
		r = 0;
		break;
	    case WSAENETDOWN:
	    case WSAENETRESET:
		e = NetworkFailure;
		break;
	    case WSAEFAULT:
	    case WSAENOTCONN:
	    case WSAESHUTDOWN:
	    case WSAEINVAL:
		e = Impossible;
		break;
	    case WSAEINTR:
		// ### ?
		r = 0;
		break;
	    case WSAEINPROGRESS:
		e = NoResources;
		break;
	    case WSAENOTSOCK:
		e = Impossible;
		break;
	    case WSAEOPNOTSUPP:
		e = InternalError; // ### ?
		break;
	    case WSAEWOULDBLOCK:
		break;
	    case WSAEMSGSIZE:
		e = NoResources; // ### ?
		break;
	    case WSAEISCONN:
		// ### ?
		r = 0;
		break;
	    default:
		e = UnknownError;
		break;
	}
    }
    return r;
}


qint64 Q3SocketDevice::writeData( const char *data, qint64 len )
{
    if ( data == 0 && len != 0 ) {
#if defined(QT_CHECK_NULL) || defined(QSOCKETDEVICE_DEBUG)
	qWarning( "Q3SocketDevice::writeBlock: Null pointer error" );
#endif
	return -1;
    }
    if ( !isValid() ) {
#if defined(QT_CHECK_STATE) || defined(QSOCKETDEVICE_DEBUG)
	qWarning( "Q3SocketDevice::writeBlock: Invalid socket" );
#endif
	return -1;
    }
    if ( !isOpen() ) {
#if defined(QT_CHECK_STATE) || defined(QSOCKETDEVICE_DEBUG)
	qWarning( "Q3SocketDevice::writeBlock: Device is not open" );
#endif
	return -1;
    }
    if ( !isWritable() ) {
#if defined(QT_CHECK_STATE) || defined(QSOCKETDEVICE_DEBUG)
	qWarning( "Q3SocketDevice::writeBlock: Write operation not permitted" );
#endif
	return -1;
    }
    bool done = false;
    qint64 r = 0;
    while ( !done ) {
	// Don't write more than 64K (see Knowledge Base Q201213).
	r = ::send( fd, data, ( len>64*1024 ? 64*1024 : len ), 0 );
	done = true;
	if ( r == SOCKET_ERROR && e == NoError ) {//&& errno != WSAEAGAIN ) {
	    switch( WSAGetLastError() ) {
		case WSANOTINITIALISED:
		    e = Impossible;
		    break;
		case WSAENETDOWN:
		case WSAEACCES:
		case WSAENETRESET:
		case WSAESHUTDOWN:
		case WSAEHOSTUNREACH:
		    e = NetworkFailure;
		    break;
		case WSAECONNABORTED:
		case WSAECONNRESET:
		    // connection closed
		    close();
		    r = 0;
		    break;
		case WSAEINTR:
		    done = false;
		    break;
		case WSAEINPROGRESS:
		    e = NoResources;
		    // ### perhaps try it later?
		    break;
		case WSAEFAULT:
		case WSAEOPNOTSUPP:
		    e = InternalError;
		    break;
		case WSAENOBUFS:
		    // ### try later?
		    break;
		case WSAEMSGSIZE:
		    e = NoResources;
		    break;
		case WSAENOTCONN:
		case WSAENOTSOCK:
		case WSAEINVAL:
		    e = Impossible;
		    break;
		case WSAEWOULDBLOCK:
		    r = 0;
		    break;
		default:
		    e = UnknownError;
		    break;
	    }
	}
    }
    return r;
}


Q_LONG Q3SocketDevice::writeBlock( const char * data, Q_ULONG len,
			       const QHostAddress & host, quint16 port )
{
    if ( t != Datagram ) {
#if defined(QT_CHECK_STATE) || defined(QSOCKETDEVICE_DEBUG)
	qWarning( "Q3SocketDevice::sendBlock: Not datagram" );
#endif
	return -1; // for now - later we can do t/tcp
    }

    if ( data == 0 && len != 0 ) {
#if defined(QT_CHECK_NULL) || defined(QSOCKETDEVICE_DEBUG)
	qWarning( "Q3SocketDevice::sendBlock: Null pointer error" );
#endif
	return -1;
    }
    if ( !isValid() ) {
#if defined(QT_CHECK_STATE) || defined(QSOCKETDEVICE_DEBUG)
	qWarning( "Q3SocketDevice::sendBlock: Invalid socket" );
#endif
	return -1;
    }
    if ( !isOpen() ) {
#if defined(QT_CHECK_STATE) || defined(QSOCKETDEVICE_DEBUG)
	qWarning( "Q3SocketDevice::sendBlock: Device is not open" );
#endif
	return -1;
    }
    if ( !isWritable() ) {
#if defined(QT_CHECK_STATE) || defined(QSOCKETDEVICE_DEBUG)
	qWarning( "Q3SocketDevice::sendBlock: Write operation not permitted" );
#endif
	return -1;
    }
    struct sockaddr_in a4;
    struct sockaddr *aa;
    SOCKLEN_T slen;
#if !defined(QT_NO_IPV6)
    qt_sockaddr_in6 a6;
    if ( initialized >= 0x20 && host.isIPv6Address() ) {
	memset( &a6, 0, sizeof(a6) );
	a6.sin6_family = AF_INET6;
	a6.sin6_port = htons( port );

	Q_IPV6ADDR tmp = host.toIPv6Address();
	memcpy( &a6.sin6_addr.qt_s6_addr, &tmp, sizeof(tmp) );
	slen = sizeof( a6 );
	aa = (struct sockaddr *)&a6;
    } else
#endif
    if ( host.isIPv4Address() ) {

	memset( &a4, 0, sizeof(a4) );
	a4.sin_family = AF_INET;
	a4.sin_port = htons( port );
	a4.sin_addr.s_addr = htonl( host.toIPv4Address() );
	slen = sizeof(a4);
	aa = (struct sockaddr *)&a4;
    } else {
	e = Impossible;
	return -1;
    }

    // we'd use MSG_DONTWAIT + MSG_NOSIGNAL if Stevens were right.
    // but apparently Stevens and most implementors disagree
    bool done = false;
    qint64 r = 0;
    while ( !done ) {
	r = ::sendto( fd, data, len, 0, aa, slen );
	done = true;
	if ( r == SOCKET_ERROR && e == NoError ) {//&& e != EAGAIN ) {
	    switch( WSAGetLastError() ) {
		case WSANOTINITIALISED:
		    e = Impossible;
		    break;
		case WSAENETDOWN:
		case WSAEACCES:
		case WSAENETRESET:
		case WSAESHUTDOWN:
		case WSAEHOSTUNREACH:
		case WSAECONNABORTED:
		case WSAECONNRESET:
		case WSAEADDRNOTAVAIL:
		case WSAENETUNREACH:
		case WSAETIMEDOUT:
		    e = NetworkFailure;
		    break;
		case WSAEINTR:
		    done = false;
		    break;
		case WSAEINPROGRESS:
		    e = NoResources;
		    // ### perhaps try it later?
		    break;
		case WSAEFAULT:
		case WSAEOPNOTSUPP:
		case WSAEAFNOSUPPORT:
		    e = InternalError;
		    break;
		case WSAENOBUFS:
		case WSAEMSGSIZE:
		    e = NoResources;
		    break;
		case WSAENOTCONN:
		case WSAENOTSOCK:
		case WSAEINVAL:
		case WSAEDESTADDRREQ:
		    e = Impossible;
		    break;
		case WSAEWOULDBLOCK:
		    r = 0;
		    break;
		default:
		    e = UnknownError;
		    break;
	    }
	}
    }
    return r;
}


void Q3SocketDevice::fetchConnectionParameters()
{
    if ( !isValid() ) {
	p = 0;
	a = QHostAddress();
	pp = 0;
	pa = QHostAddress();
	return;
    }
#if !defined (QT_NO_IPV6)
    struct qt_sockaddr_storage sa;
#else
    struct sockaddr_in sa;
#endif
    memset( &sa, 0, sizeof(sa) );
    SOCKLEN_T sz;
    sz = sizeof( sa );
    if ( !::getsockname( fd, (struct sockaddr *)(&sa), &sz ) )
	qt_socket_getportaddr( (struct sockaddr *)(&sa), &p, &a );
    pp = 0;
    pa = QHostAddress();
}


void Q3SocketDevice::fetchPeerConnectionParameters()
{
    // do the getpeername() lazy on Windows (sales/arc-18/37759 claims that
    // there will be problems otherwise if you use MS Proxy server)
#if !defined (QT_NO_IPV6)
    struct qt_sockaddr_storage sa;
#else
    struct sockaddr_in sa;
#endif
    memset( &sa, 0, sizeof(sa) );
    SOCKLEN_T sz;
    sz = sizeof( sa );
    if ( !::getpeername( fd, (struct sockaddr *)(&sa), &sz ) )
	qt_socket_getportaddr( (struct sockaddr *)(&sa), &pp, &pa );
}

quint16 Q3SocketDevice::peerPort() const
{
    if ( pp==0 && isValid() ) {
	Q3SocketDevice *that = (Q3SocketDevice*)this; // mutable
	that->fetchPeerConnectionParameters();
    }
    return pp;
}


QHostAddress Q3SocketDevice::peerAddress() const
{
    if ( pp==0 && isValid() ) {
	Q3SocketDevice *that = (Q3SocketDevice*)this; // mutable
	that->fetchPeerConnectionParameters();
    }
    return pa;
}

QT_END_NAMESPACE
