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

#include "q3socketdevice.h"
#ifndef QT_NO_NETWORK

#include "qwindowdefs.h"
#include <string.h>

QT_BEGIN_NAMESPACE

//#define Q3SOCKETDEVICE_DEBUG


class Q3SocketDevicePrivate
{
public:
    Q3SocketDevicePrivate( Q3SocketDevice::Protocol p )
	: protocol(p)
    { }

    Q3SocketDevice::Protocol protocol;
};


/*!
    \class Q3SocketDevice
    \brief The Q3SocketDevice class provides a platform-independent low-level socket API.

    \compat
    \reentrant

    This class provides a low level API for working with sockets.  Users of
    this class are assumed to have networking experience. For most users the
    Q3Socket class provides a much easier and high level alternative, but
    certain things (like UDP) can't be done with Q3Socket and if you need a
    platform-independent API for those, Q3SocketDevice is the right choice.

    The essential purpose of the class is to provide a QIODevice that
    works on sockets, wrapped in a platform-independent API.

    When calling connect() or bind(), Q3SocketDevice detects the
    protocol family (IPv4, IPv6) automatically. Passing the protocol
    family to Q3SocketDevice's constructor or to setSocket() forces
    creation of a socket device of a specific protocol. If not set, the
    protocol will be detected at the first call to connect() or bind().

    \sa Q3Socket, QSocketNotifier, QHostAddress
*/


/*!
    \enum Q3SocketDevice::Protocol

    This enum type describes the protocol family of the socket. Possible values
    are:

    \value IPv4 The socket is an IPv4 socket.
    \value IPv6 The socket is an IPv6 socket.
    \value Unknown The protocol family of the socket is not known. This can
	   happen if you use Q3SocketDevice with an already existing socket; it
	   tries to determine the protocol family, but this can fail if the
	   protocol family is not known to Q3SocketDevice.

    \sa protocol() setSocket()
*/

/*!
    \enum Q3SocketDevice::Error

    This enum type describes the error states of Q3SocketDevice.

    \value NoError  No error has occurred.

    \value AlreadyBound  The device is already bound, according to bind().

    \value Inaccessible  The operating system or firewall prohibited
			the action.

    \value NoResources  The operating system ran out of a resource.

    \value InternalError  An internal error occurred in Q3SocketDevice.

    \value Impossible  An attempt was made to do something which makes
    no sense. For example:
    \snippet doc/src/snippets/code/src_qt3support_network_q3socketdevice.cpp 0
    The libc ::close() closes the socket, but Q3SocketDevice is not aware
    of this. So when you call writeBlock(), the impossible happens.

    \value NoFiles  The operating system will not let Q3SocketDevice open
    another file.

    \value ConnectionRefused  A connection attempt was rejected by the
    peer.

    \value NetworkFailure  There is a network failure.

    \value UnknownError  The operating system did something
    unexpected.

    \omitvalue Bug
*/

/*!
    \enum Q3SocketDevice::Type

    This enum type describes the type of the socket:
    \value Stream  a stream socket (TCP, usually)
    \value Datagram  a datagram socket (UDP, usually)
*/


/*!
    Creates a Q3SocketDevice object for the existing socket \a socket.

    The \a type argument must match the actual socket type; use \c
    Q3SocketDevice::Stream for a reliable, connection-oriented TCP
    socket, or Q3SocketDevice::Datagram for an unreliable,
    connectionless UDP socket.
*/
Q3SocketDevice::Q3SocketDevice( int socket, Type type )
    : fd( socket ), t( type ), p( 0 ), pp( 0 ), e( NoError ),
      d(new Q3SocketDevicePrivate(Unknown))
{
#if defined(Q3SOCKETDEVICE_DEBUG)
    qDebug( "Q3SocketDevice: Created Q3SocketDevice %p (socket %x, type %d)",
	   this, socket, type );
#endif
    init();
    setSocket( socket, type );
}

/*!
    Creates a Q3SocketDevice object for a stream or datagram socket.

    The \a type argument must be either Q3SocketDevice::Stream for a
    reliable, connection-oriented TCP socket, or \c
    Q3SocketDevice::Datagram for an unreliable UDP socket.

    The socket is created as an IPv4 socket.

    \sa blocking() protocol()
*/
Q3SocketDevice::Q3SocketDevice( Type type )
    : fd( -1 ), t( type ), p( 0 ), pp( 0 ), e( NoError ),
      d(new Q3SocketDevicePrivate(IPv4))
{
#if defined(Q3SOCKETDEVICE_DEBUG)
    qDebug( "Q3SocketDevice: Created Q3SocketDevice object %p, type %d",
	    this, type );
#endif
    init();
    setSocket( createNewSocket(), type );
}

/*!
    Creates a Q3SocketDevice object for a stream or datagram socket.

    The \a type argument must be either Q3SocketDevice::Stream for a
    reliable, connection-oriented TCP socket, or \c
    Q3SocketDevice::Datagram for an unreliable UDP socket.

    The \a protocol indicates whether the socket should be of type IPv4
    or IPv6. Passing \c Unknown is not meaningful in this context and you
    should avoid using (it creates an IPv4 socket, but your code is not easily
    readable).

    The argument \a dummy is necessary for compatibility with some
    compilers.

    \sa blocking() protocol()
*/
Q3SocketDevice::Q3SocketDevice( Type type, Protocol protocol, int )
    : fd( -1 ), t( type ), p( 0 ), pp( 0 ), e( NoError ),
      d(new Q3SocketDevicePrivate(protocol))
{
#if defined(Q3SOCKETDEVICE_DEBUG)
    qDebug( "Q3SocketDevice: Created Q3SocketDevice object %p, type %d",
	    this, type );
#endif
    init();
    setSocket( createNewSocket(), type );
}

/*!
    Destroys the socket device and closes the socket if it is open.
*/
Q3SocketDevice::~Q3SocketDevice()
{
    close();
    delete d;
    d = 0;
#if defined(Q3SOCKETDEVICE_DEBUG)
    qDebug( "Q3SocketDevice: Destroyed Q3SocketDevice %p", this );
#endif
}


/*!
    Returns true if this is a valid socket; otherwise returns false.

    \sa socket()
*/
bool Q3SocketDevice::isValid() const
{
    return fd != -1;
}


/*!
    \fn Type Q3SocketDevice::type() const

    Returns the socket type which is either Q3SocketDevice::Stream
    or Q3SocketDevice::Datagram.

    \sa socket()
*/
Q3SocketDevice::Type Q3SocketDevice::type() const
{
    return t;
}

/*!
    Returns the socket's protocol family, which is one of \c Unknown, \c IPv4,
    or \c IPv6.

    Q3SocketDevice either creates a socket with a well known protocol family or
    it uses an already existing socket. In the first case, this function
    returns the protocol family it was constructed with. In the second case, it
    tries to determine the protocol family of the socket; if this fails, it
    returns \c Unknown.

    \sa Protocol setSocket()
*/
Q3SocketDevice::Protocol Q3SocketDevice::protocol() const
{
    if ( d->protocol == Unknown )
	d->protocol = getProtocol();
    return d->protocol;
}

/*!
    Returns the socket number, or -1 if it is an invalid socket.

    \sa isValid(), type()
*/
int Q3SocketDevice::socket() const
{
    return fd;
}


/*!
    Sets the socket device to operate on the existing socket \a
    socket.

    The \a type argument must match the actual socket type; use \c
    Q3SocketDevice::Stream for a reliable, connection-oriented TCP
    socket, or Q3SocketDevice::Datagram for an unreliable,
    connectionless UDP socket.

    Any existing socket is closed.

    \sa isValid(), close()
*/
void Q3SocketDevice::setSocket( int socket, Type type )
{
    if ( fd != -1 )			// close any open socket
	close();
#if defined(Q3SOCKETDEVICE_DEBUG)
    qDebug( "Q3SocketDevice::setSocket: socket %x, type %d", socket, type );
#endif
    t = type;
    fd = socket;
    d->protocol = Unknown;
    e = NoError;
    resetStatus();
    open( ReadWrite );
    fetchConnectionParameters();
}


/*!
    Opens the socket using the specified QIODevice file \a mode. This
    function is called from the Q3SocketDevice constructors and from
    the setSocket() function. You should not call it yourself.

    \sa close()
*/
bool Q3SocketDevice::open( OpenMode mode )
{
    if ( isOpen() || !isValid() )
	return false;
#if defined(Q3SOCKETDEVICE_DEBUG)
    qDebug( "Q3SocketDevice::open: mode %x", mode );
#endif
    setOpenMode( (mode & ReadWrite) | Unbuffered );
    return true;
}

/*!
    \fn bool Q3SocketDevice::open(int mode)
    \overload
*/
/*!
    The current Q3SocketDevice implementation does not buffer at all,
    so this is a no-op. This function always returns true.
*/
bool Q3SocketDevice::flush()
{
    return true;
}


/*!
    \reimp

    The size is meaningless for a socket, therefore this function returns 0.
*/
QIODevice::Offset Q3SocketDevice::size() const
{
    return 0;
}


/*!
    The read/write index is meaningless for a socket, therefore this
    function returns 0.
*/
QIODevice::Offset Q3SocketDevice::at() const
{
    return 0;
}


/*!
    The read/write index is meaningless for a socket, therefore this
    function does nothing and returns true.

    The \a offset parameter is ignored.
*/
bool Q3SocketDevice::at( Offset /* offset */ )
{
    return true;
}


/*!
    \reimp

    Returns true if no data is currently available at the socket;
    otherwise returns false.
*/
bool Q3SocketDevice::atEnd() const
{
    return bytesAvailable() <= 0;
}

/*!
    Returns true if the address of this socket can be used by other
    sockets at the same time, and false if this socket claims
    exclusive ownership.

    \sa setAddressReusable()
*/
bool Q3SocketDevice::addressReusable() const
{
    return option( ReuseAddress );
}


/*!
    Sets the address of this socket to be usable by other sockets too
    if \a enable is true, and to be used exclusively by this socket if
    \a enable is false.

    When a socket is reusable, other sockets can use the same port
    number (and IP address), which is generally useful. Of course
    other sockets cannot use the same
    (address,port,peer-address,peer-port) 4-tuple as this socket, so
    there is no risk of confusing the two TCP connections.

    \sa addressReusable()
*/
void Q3SocketDevice::setAddressReusable( bool enable )
{
    setOption( ReuseAddress, enable );
}


/*!
    Returns the size of the operating system receive buffer.

    \sa setReceiveBufferSize()
*/
int Q3SocketDevice::receiveBufferSize() const
{
    return option( ReceiveBuffer );
}


/*!
    Sets the size of the operating system receive buffer to \a size.

    The operating system receive buffer size effectively limits two
    things: how much data can be in transit at any one moment, and how
    much data can be received in one iteration of the main event loop.

    The default is operating system-dependent. A socket that receives
    large amounts of data is probably best with a buffer size of
    49152.
*/
void Q3SocketDevice::setReceiveBufferSize( uint size )
{
    setOption( ReceiveBuffer, size );
}


/*!
    Returns the size of the operating system send buffer.

    \sa setSendBufferSize()
*/
int Q3SocketDevice::sendBufferSize() const
{
    return option( SendBuffer );
}


/*!
    Sets the size of the operating system send buffer to \a size.

    The operating system send buffer size effectively limits how much
    data can be in transit at any one moment.

    The default is operating system-dependent. A socket that sends
    large amounts of data is probably best with a buffer size of
    49152.
*/
void Q3SocketDevice::setSendBufferSize( uint size )
{
    setOption( SendBuffer, size );
}


/*!
    Returns the port number of this socket device. This may be 0 for a
    while, but is set to something sensible as soon as a sensible
    value is available.

    Note that Qt always uses native byte order, i.e. 67 is 67 in Qt;
    there is no need to call htons().
*/
quint16 Q3SocketDevice::port() const
{
    return p;
}


/*!
    Returns the address of this socket device. This may be 0.0.0.0 for
    a while, but is set to something sensible as soon as a sensible
    value is available.
*/
QHostAddress Q3SocketDevice::address() const
{
    return a;
}


/*!
    Returns the first error seen.
*/
Q3SocketDevice::Error Q3SocketDevice::error() const
{
    return e;
}


/*!
    Allows subclasses to set the error state to \a err.
*/
void Q3SocketDevice::setError( Error err )
{
    e = err;
}

/*! \fn Q3SocketDevice::readBlock(char *data, Q_ULONG maxlen)

    Reads \a maxlen bytes from the socket into \a data and returns the
    number of bytes read. Returns -1 if an error occurred. Returning 0
    is not an error. For Stream sockets, 0 is returned when the remote
    host closes the connection. For Datagram sockets, 0 is a valid
    datagram size.
*/

/*! \fn Q3SocketDevice::writeBlock(const char *data, Q_ULONG len)

    Writes \a len bytes to the socket from \a data and returns the
    number of bytes written. Returns -1 if an error occurred.

    This is used for Q3SocketDevice::Stream sockets.
*/

/*!
    \fn Q_LONG Q3SocketDevice::writeBlock( const char * data, Q_ULONG len,
        const QHostAddress & host, Q_UINT16 port )
    \overload

    Writes \a len bytes to the socket from \a data and returns the
    number of bytes written. Returns -1 if an error occurred.

    This is used for Q3SocketDevice::Datagram sockets. You must
    specify the \a host and \a port of the destination of the data.
*/

/*!
    \fn bool Q3SocketDevice::isSequential() const
    \internal
*/

/*!
    \fn qint64 Q3SocketDevice::readData( char *data, qint64 maxlen )

    Reads \a maxlen bytes from the socket into \a data and returns the
    number of bytes read. Returns -1 if an error occurred.
*/

/*!
    \fn int Q3SocketDevice::createNewSocket()

    Creates a new socket identifier. Returns -1 if there is a failure
    to create the new identifier; error() explains why.

    \sa setSocket()
*/

/*!
    \fn void Q3SocketDevice::close()
    \reimp

    Closes the socket and sets the socket identifier to -1 (invalid).

    (This function ignores errors; if there are any then a file
    descriptor leakage might result. As far as we know, the only error
    that can arise is EBADF, and that would of course not cause
    leakage. There may be OS-specific errors that we haven't come
    across, however.)

    \sa open()
*/

/*!
    \fn bool Q3SocketDevice::blocking() const

    Returns true if the socket is valid and in blocking mode;
    otherwise returns false.

    Note that this function does not set error().

    \warning On Windows, this function always returns true since the
    ioctlsocket() function is broken.

    \sa setBlocking(), isValid()
*/

/*!
    \fn void Q3SocketDevice::setBlocking( bool enable )

    Makes the socket blocking if \a enable is true or nonblocking if
    \a enable is false.

    Sockets are blocking by default, but we recommend using
    nonblocking socket operations, especially for GUI programs that
    need to be responsive.

    \warning On Windows, this function should be used with care since
    whenever you use a QSocketNotifier on Windows, the socket is
    immediately made nonblocking.

    \sa blocking(), isValid()
*/

/*!
    \fn int Q3SocketDevice::option( Option opt ) const

    Returns the value of the socket option \a opt.
*/

/*!
    \fn void Q3SocketDevice::setOption( Option opt, int v )

    Sets the socket option \a opt to \a v.
*/

/*!
    \fn bool Q3SocketDevice::connect( const QHostAddress &addr, Q_UINT16 port )

    Connects to the IP address and port specified by \a addr and \a
    port. Returns true if it establishes a connection; otherwise returns false.
    If it returns false, error() explains why.

    Note that error() commonly returns NoError for non-blocking
    sockets; this just means that you can call connect() again in a
    little while and it'll probably succeed.
*/

/*!
    \fn bool Q3SocketDevice::bind( const QHostAddress &address, Q_UINT16 port )

    Assigns a name to an unnamed socket. The name is the host address
    \a address and the port number \a port. If the operation succeeds,
    bind() returns true; otherwise it returns false without changing
    what port() and address() return.

    bind() is used by servers for setting up incoming connections.
    Call bind() before listen().
*/

/*!
    \fn bool Q3SocketDevice::listen( int backlog )

    Specifies how many pending connections a server socket can have.
    Returns true if the operation was successful; otherwise returns
    false. A \a backlog value of 50 is quite common.

    The listen() call only applies to sockets where type() is \c
    Stream, i.e. not to \c Datagram sockets. listen() must not be
    called before bind() or after accept().

    \sa bind(), accept()
*/

/*!
    \fn int Q3SocketDevice::accept()

    Extracts the first connection from the queue of pending
    connections for this socket and returns a new socket identifier.
    Returns -1 if the operation failed.

    \sa bind(), listen()
*/

/*!
    \fn qint64 Q3SocketDevice::bytesAvailable() const

    Returns the number of bytes available for reading, or -1 if an
    error occurred.

    \warning On Microsoft Windows, we use the ioctlsocket() function
    to determine the number of bytes queued on the socket. According
    to Microsoft (KB Q125486), ioctlsocket() sometimes returns an
    incorrect number. The only safe way to determine the amount of
    data on the socket is to read it using readBlock(). QSocket has
    workarounds to deal with this problem.
*/

/*!
    \fn Q_LONG Q3SocketDevice::waitForMore( int msecs, bool *timeout ) const

    Wait up to \a msecs milliseconds for more data to be available. If
    \a msecs is -1 the call will block indefinitely.

    Returns the number of bytes available for reading, or -1 if an
    error occurred.

    If \a timeout is non-null and no error occurred (i.e. it does not
    return -1): this function sets *\a timeout to true, if the reason
    for returning was that the timeout was reached; otherwise it sets
    *\a timeout to false. This is useful to find out if the peer
    closed the connection.

    \warning This is a blocking call and should be avoided in event
    driven applications.

    \sa bytesAvailable()
*/

/*!
    \fn qint64 Q3SocketDevice::writeData( const char *data, qint64 len )

    Writes \a len bytes to the socket from \a data and returns the
    number of bytes written. Returns -1 if an error occurred.

    This is used for Q3SocketDevice::Stream sockets.
*/

/*!
    \fn void Q3SocketDevice::fetchConnectionParameters()

    Fetches information about both ends of the connection: whatever is
    available.
*/

/*!
    \fn Q_UINT16 Q3SocketDevice::peerPort() const

    Returns the port number of the port this socket device is
    connected to. This may be 0 for a while, but is set to something
    sensible as soon as a sensible value is available.

    Note that for Datagram sockets, this is the source port of the
    last packet received, and that it is in native byte order.
*/

/*!
    \fn QHostAddress Q3SocketDevice::peerAddress() const

    Returns the address of the port this socket device is connected
    to. This may be 0.0.0.0 for a while, but is set to something
    sensible as soon as a sensible value is available.

    Note that for Datagram sockets, this is the source port of the
    last packet received.
*/

QT_END_NAMESPACE

#endif //QT_NO_NETWORK
