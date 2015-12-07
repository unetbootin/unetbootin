/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the Qt3Support module of the Qt Toolkit.
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

#include "qplatformdefs.h"

// Almost always the same. If not, specify in qplatformdefs.h.
#if !defined(QT_SOCKOPTLEN_T)
# define QT_SOCKOPTLEN_T QT_SOCKLEN_T
#endif

// Tru64 redefines accept -> _accept with _XOPEN_SOURCE_EXTENDED
static inline int qt_socket_accept(int s, struct sockaddr *addr, QT_SOCKLEN_T *addrlen)
{ return ::accept(s, addr, addrlen); }
#if defined(accept)
# undef accept
#endif

// UnixWare 7 redefines listen -> _listen
static inline int qt_socket_listen(int s, int backlog)
{ return ::listen(s, backlog); }
#if defined(listen)
# undef listen
#endif

// UnixWare 7 redefines socket -> _socket
static inline int qt_socket_socket(int domain, int type, int protocol)
{ return ::socket(domain, type, protocol); }
#if defined(socket)
# undef socket
#endif

#include "q3socketdevice.h"

#ifndef QT_NO_NETWORK

#include "qwindowdefs.h"

#include <errno.h>
#include <sys/types.h>

QT_BEGIN_NAMESPACE

static inline void qt_socket_getportaddr( struct sockaddr *sa,
					  Q_UINT16 *port, QHostAddress *addr )
{
#if !defined(QT_NO_IPV6)
    if ( sa->sa_family == AF_INET6 ) {
	struct sockaddr_in6 *sa6 = ( struct sockaddr_in6 * )sa;
	Q_IPV6ADDR tmp;
	memcpy( &tmp, &sa6->sin6_addr.s6_addr, sizeof(tmp) );
	QHostAddress a( tmp );
	*addr = a;
	*port = ntohs( sa6->sin6_port );
	return;
    }
#endif
    struct sockaddr_in *sa4 = (struct sockaddr_in *)sa;
    QHostAddress a( ntohl( sa4->sin_addr.s_addr ) );
    *port = ntohs( sa4->sin_port );
    *addr = QHostAddress( ntohl( sa4->sin_addr.s_addr ) );
    return;
}


//#define QSOCKETDEVICE_DEBUG

// internal
void Q3SocketDevice::init()
{
}


Q3SocketDevice::Protocol Q3SocketDevice::getProtocol() const
{
    if ( isValid() ) {
#if !defined (QT_NO_IPV6)
	struct sockaddr_storage sa;
#else
	struct sockaddr sa;
#endif
	memset( &sa, 0, sizeof(sa) );
	QT_SOCKLEN_T sz = sizeof( sa );
#if !defined (QT_NO_IPV6)
	struct sockaddr *sap = reinterpret_cast<struct sockaddr *>(&sa);
	if ( !::getsockname(fd, sap, &sz) ) {
	    switch ( sap->sa_family ) {
		case AF_INET:
		    return IPv4;
		case AF_INET6:
		    return IPv6;
		default:
		    return Unknown;
	    }
	}
#else
	if ( !::getsockname(fd, &sa, &sz) ) {
	    switch ( sa.sa_family ) {
		case AF_INET:
		    return IPv4;
		default:
		    return Unknown;
	    }
	}
#endif
    }
    return Unknown;
}


int Q3SocketDevice::createNewSocket()
{
#if !defined(QT_NO_IPV6)
    int s = qt_socket_socket( protocol() == IPv6 ? AF_INET6 : AF_INET,
			      t == Datagram ? SOCK_DGRAM : SOCK_STREAM, 0 );
#else
    int s = qt_socket_socket( AF_INET, t==Datagram?SOCK_DGRAM:SOCK_STREAM, 0 );
#endif
    if ( s < 0 ) {
	switch( errno ) {
	case EPROTONOSUPPORT:
	    e = InternalError; // 0 is supposed to work for both types
	    break;
	case ENFILE:
	    e = NoFiles; // special case for this
	    break;
	case EACCES:
	    e = Inaccessible;
	    break;
	case ENOBUFS:
	case ENOMEM:
	    e = NoResources;
	    break;
	case EINVAL:
	    e = Impossible;
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
    ::close( fd );
#if defined(QSOCKETDEVICE_DEBUG)
    qDebug( "Q3SocketDevice::close: Closed socket %x", fd );
#endif
    fd = -1;
    fetchConnectionParameters();
    QIODevice::close();
}


bool Q3SocketDevice::blocking() const
{
    if ( !isValid() )
	return true;
    int s = fcntl(fd, F_GETFL, 0);
    return !(s >= 0 && ((s & O_NDELAY) != 0));
}


void Q3SocketDevice::setBlocking( bool enable )
{
#if defined(QSOCKETDEVICE_DEBUG)
    qDebug( "Q3SocketDevice::setBlocking( %d )", enable );
#endif
    if ( !isValid() )
	return;
    int tmp = ::fcntl(fd, F_GETFL, 0);
    if ( tmp >= 0 )
	tmp = ::fcntl( fd, F_SETFL, enable ? (tmp&~O_NDELAY) : (tmp|O_NDELAY) );
    if ( tmp >= 0 )
	return;
    if ( e )
	return;
    switch( errno ) {
    case EACCES:
    case EBADF:
	e = Impossible;
	break;
    case EFAULT:
    case EAGAIN:
#if EAGAIN != EWOULDBLOCK
    case EWOULDBLOCK:
#endif
    case EDEADLK:
    case EINTR:
    case EINVAL:
    case EMFILE:
    case ENOLCK:
    case EPERM:
    default:
	e = UnknownError;
    }
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
	QT_SOCKOPTLEN_T len;
	len = sizeof(v);
	int r = ::getsockopt( fd, SOL_SOCKET, n, (char*)&v, &len );
	if ( r >= 0 )
	    return v;
	if ( !e ) {
	    Q3SocketDevice *that = (Q3SocketDevice*)this; // mutable function
	    switch( errno ) {
	    case EBADF:
	    case ENOTSOCK:
		that->e = Impossible;
		break;
	    case EFAULT:
		that->e = InternalError;
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
    if ( ::setsockopt( fd, SOL_SOCKET, n, (char*)&v, sizeof(v)) < 0 &&
	 e == NoError ) {
	switch( errno ) {
	case EBADF:
	case ENOTSOCK:
	    e = Impossible;
	    break;
	case EFAULT:
	    e = InternalError;
	    break;
	default:
	    e = UnknownError;
	    break;
	}
    }
}


bool Q3SocketDevice::connect( const QHostAddress &addr, Q_UINT16 port )
{
    if ( !isValid() )
	return false;

    pa = addr;
    pp = port;

    struct sockaddr_in a4;
    struct sockaddr *aa;
    QT_SOCKLEN_T aalen;

#if !defined(QT_NO_IPV6)
    struct sockaddr_in6 a6;

    if ( addr.isIPv6Address() ) {
	memset( &a6, 0, sizeof(a6) );
	a6.sin6_family = AF_INET6;
	a6.sin6_port = htons( port );
	Q_IPV6ADDR ip6 = addr.toIPv6Address();
	memcpy( &a6.sin6_addr.s6_addr, &ip6, sizeof(ip6) );

	aalen = sizeof( a6 );
	aa = (struct sockaddr *)&a6;
    } else
#endif
    if ( addr.isIPv4Address() ) {
	memset( &a4, 0, sizeof(a4) );
	a4.sin_family = AF_INET;
	a4.sin_port = htons( port );
	a4.sin_addr.s_addr = htonl( addr.toIPv4Address() );

	aalen = sizeof(a4);
	aa = (struct sockaddr *)&a4;
    } else {
	e = Impossible;
	return false;
    }

    int r = QT_SOCKET_CONNECT( fd, aa, aalen );
    if ( r == 0 ) {
	fetchConnectionParameters();
	return true;
    }
    if ( errno == EISCONN || errno == EALREADY || errno == EINPROGRESS ) {
	fetchConnectionParameters();
	return true;
    }
    if ( e != NoError || errno == EAGAIN || errno == EWOULDBLOCK ) {
	return false;
    }
    switch( errno ) {
    case EBADF:
    case ENOTSOCK:
	e = Impossible;
	break;
    case EFAULT:
    case EAFNOSUPPORT:
	e = InternalError;
	break;
    case ECONNREFUSED:
	e = ConnectionRefused;
	break;
    case ETIMEDOUT:
    case ENETUNREACH:
	e = NetworkFailure;
	break;
    case EADDRINUSE:
	e = NoResources;
	break;
    case EACCES:
    case EPERM:
	e = Inaccessible;
	break;
    default:
	e = UnknownError;
	break;
    }
    return false;
}


bool Q3SocketDevice::bind( const QHostAddress &address, Q_UINT16 port )
{
    if ( !isValid() )
	return false;
    int r;
    struct sockaddr_in a4;
#if !defined(QT_NO_IPV6)
    struct sockaddr_in6 a6;

    if ( address.isIPv6Address() ) {
	memset( &a6, 0, sizeof(a6) );
	a6.sin6_family = AF_INET6;
	a6.sin6_port = htons( port );
	Q_IPV6ADDR tmp = address.toIPv6Address();
	memcpy( &a6.sin6_addr.s6_addr, &tmp, sizeof(tmp) );

	r = QT_SOCKET_BIND( fd, (struct sockaddr *)&a6, sizeof(a6) );
    } else
#endif
    if ( address.isIPv4Address() ) {
	memset( &a4, 0, sizeof(a4) );
	a4.sin_family = AF_INET;
	a4.sin_port = htons( port );
	a4.sin_addr.s_addr = htonl( address.toIPv4Address() );

	r = QT_SOCKET_BIND( fd, (struct sockaddr*)&a4, sizeof(a4) );
    } else {
	e = Impossible;
	return false;
    }

    if ( r < 0 ) {
	switch( errno ) {
	case EINVAL:
	    e = AlreadyBound;
	    break;
	case EACCES:
	    e = Inaccessible;
	    break;
	case ENOMEM:
	    e = NoResources;
	    break;
	case EFAULT: // a was illegal
	case ENAMETOOLONG: // sz was wrong
	    e = InternalError;
	    break;
	case EBADF: // AF_UNIX only
	case ENOTSOCK: // AF_UNIX only
	case EROFS: // AF_UNIX only
	case ENOENT: // AF_UNIX only
	case ENOTDIR: // AF_UNIX only
	case ELOOP: // AF_UNIX only
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
    if ( qt_socket_listen( fd, backlog ) >= 0 )
	return true;
    if ( !e )
	e = Impossible;
    return false;
}


int Q3SocketDevice::accept()
{
    if ( !isValid() )
	return -1;

#if !defined (QT_NO_IPV6)
    struct sockaddr_storage aa;
#else
    struct sockaddr aa;
#endif
    QT_SOCKLEN_T l = sizeof( aa );
    bool done;
    int s;
    do {
        s = qt_socket_accept( fd, (struct sockaddr*)&aa, &l );
        // we'll blithely throw away the stuff accept() wrote to aa
        done = true;
        if ( s < 0 && e == NoError ) {
            switch( errno ) {
            case EINTR:
                done = false;
                break;
#if defined(EPROTO)
	    case EPROTO:
#endif
#if defined(ENONET)
	    case ENONET:
#endif
	    case ENOPROTOOPT:
	    case EHOSTDOWN:
	    case EOPNOTSUPP:
	    case EHOSTUNREACH:
	    case ENETDOWN:
	    case ENETUNREACH:
	    case ETIMEDOUT:
		// in all these cases, an error happened during connection
		// setup.  we're not interested in what happened, so we
		// just treat it like the client-closed-quickly case.
	    case EPERM:
		// firewalling wouldn't let us accept.  we treat it like
		// the client-closed-quickly case.
	    case EAGAIN:
#if EAGAIN != EWOULDBLOCK
	    case EWOULDBLOCK:
#endif
		// the client closed the connection before we got around
		// to accept()ing it.
		break;
	    case EBADF:
	    case ENOTSOCK:
		e = Impossible;
		break;
	    case EFAULT:
		e = InternalError;
		break;
	    case ENOMEM:
	    case ENOBUFS:
		e = NoResources;
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
    if ( ::ioctl(fd, FIONREAD, (char*)&nbytes) < 0 )
	return -1;
    return (Q_LONG) *((int *) &nbytes) + QIODevice::bytesAvailable();
}


Q_LONG Q3SocketDevice::waitForMore( int msecs, bool *timeout ) const
{
    if ( !isValid() )
	return -1;
    if ( fd >= FD_SETSIZE )
	return -1;

    fd_set fds;
    struct timeval tv;

    FD_ZERO( &fds );
    FD_SET( fd, &fds );

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
    bool done = false;
    int r = 0;
    while ( done == false ) {
	if ( t == Datagram ) {
#if !defined(QT_NO_IPV6)
            struct sockaddr_storage aa;
#else
            struct sockaddr_in aa;
#endif
	    memset( &aa, 0, sizeof(aa) );
	    QT_SOCKLEN_T sz;
	    sz = sizeof( aa );
	    r = ::recvfrom( fd, data, maxlen, 0,
			    (struct sockaddr *)&aa, &sz );

	    qt_socket_getportaddr( (struct sockaddr *)&aa, &pp, &pa);

	} else {
	    r = ::read( fd, data, maxlen );
	}
	done = true;
        if ( r == 0 && t == Stream && maxlen > 0 ) {
            // connection closed
            close();
        } else if ( r >= 0 || errno == EAGAIN || errno == EWOULDBLOCK ) {
	    // nothing
	} else if ( errno == EINTR ) {
	    done = false;
	} else if ( e == NoError ) {
	    switch( errno ) {
	    case EIO:
	    case EISDIR:
	    case EBADF:
	    case EINVAL:
	    case EFAULT:
	    case ENOTCONN:
	    case ENOTSOCK:
		e = Impossible;
		break;
#if defined(ENONET)
	    case ENONET:
#endif
	    case EHOSTUNREACH:
	    case ENETDOWN:
	    case ENETUNREACH:
	    case ETIMEDOUT:
		e = NetworkFailure;
		break;
	    case EPIPE:
	    case ECONNRESET:
		// connection closed
		close();
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
    int r = 0;
    bool timeout;
    while ( !done ) {
	r = ::write( fd, data, len );
	done = true;
	if ( r < 0 && e == NoError &&
	     errno != EAGAIN && errno != EWOULDBLOCK ) {
	    switch( errno ) {
	    case EINTR: // signal - call read() or whatever again
		done = false;
		break;
	    case EPIPE:
            case ECONNRESET:
		// connection closed
		close();
		r = 0;
		break;
	    case ENOSPC:
	    case EIO:
	    case EISDIR:
	    case EBADF:
	    case EINVAL:
	    case EFAULT:
	    case ENOTCONN:
	    case ENOTSOCK:
		e = Impossible;
		break;
#if defined(ENONET)
	    case ENONET:
#endif
	    case EHOSTUNREACH:
	    case ENETDOWN:
	    case ENETUNREACH:
	    case ETIMEDOUT:
		e = NetworkFailure;
		break;
	    default:
		e = UnknownError;
		break;
	    }
	} else if ( waitForMore( 0, &timeout ) == 0 ) {
	    if ( !timeout ) {
		// connection closed
		close();
	    }
	}
    }
    return r;
}


Q_LONG Q3SocketDevice::writeBlock( const char * data, Q_ULONG len,
			       const QHostAddress & host, Q_UINT16 port )
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
    QT_SOCKLEN_T slen;
#if !defined(QT_NO_IPV6)
    struct sockaddr_in6 a6;
    if ( host.isIPv6Address() ) {
	memset( &a6, 0, sizeof(a6) );
	a6.sin6_family = AF_INET6;
	a6.sin6_port = htons( port );

	Q_IPV6ADDR tmp = host.toIPv6Address();
	memcpy( &a6.sin6_addr.s6_addr, &tmp, sizeof(tmp) );
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
    int r = 0;
    while ( !done ) {
	r = ::sendto( fd, data, len, 0, aa, slen);
	done = true;
	if ( r < 0 && e == NoError &&
	     errno != EAGAIN && errno != EWOULDBLOCK ) {
	    switch( errno ) {
	    case EINTR: // signal - call read() or whatever again
		done = false;
		break;
	    case ENOSPC:
	    case EPIPE:
	    case EIO:
	    case EISDIR:
	    case EBADF:
	    case EINVAL:
	    case EFAULT:
	    case ENOTCONN:
	    case ENOTSOCK:
		e = Impossible;
		break;
#if defined(ENONET)
	    case ENONET:
#endif
	    case EHOSTUNREACH:
	    case ENETDOWN:
	    case ENETUNREACH:
	    case ETIMEDOUT:
		e = NetworkFailure;
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
#if !defined(QT_NO_IPV6)
    struct sockaddr_storage sa;
#else
    struct sockaddr_in sa;
#endif
    memset( &sa, 0, sizeof(sa) );
    QT_SOCKLEN_T sz;
    sz = sizeof( sa );
    if ( !::getsockname( fd, (struct sockaddr *)(&sa), &sz ) )
	qt_socket_getportaddr( (struct sockaddr *)&sa, &p, &a );

    sz = sizeof( sa );
    if ( !::getpeername( fd, (struct sockaddr *)(&sa), &sz ) )
	qt_socket_getportaddr( (struct sockaddr *)&sa, &pp, &pa );
}


Q_UINT16 Q3SocketDevice::peerPort() const
{
    return pp;
}


QHostAddress Q3SocketDevice::peerAddress() const
{
    return pa;
}

QT_END_NAMESPACE

#endif //QT_NO_NETWORK
