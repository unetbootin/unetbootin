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

#include "q3serversocket.h"

#ifndef QT_NO_NETWORK

#include "qsocketnotifier.h"

QT_BEGIN_NAMESPACE

class Q3ServerSocketPrivate {
public:
    Q3ServerSocketPrivate(): s(0), n(0) {}
    ~Q3ServerSocketPrivate() { delete n; delete s; }
    Q3SocketDevice *s;
    QSocketNotifier *n;
};


/*!
    \class Q3ServerSocket qserversocket.h
    \brief The Q3ServerSocket class provides a TCP-based server.

    \compat

    This class is a convenience class for accepting incoming TCP
    connections. You can specify the port or have Q3ServerSocket pick
    one, and listen on just one address or on all the machine's
    addresses.

    Using the API is very simple: subclass Q3ServerSocket, call the
    constructor of your choice, and implement newConnection() to
    handle new incoming connections. There is nothing more to do.

    (Note that due to lack of support in the underlying APIs,
    Q3ServerSocket cannot accept or reject connections conditionally.)

    \sa Q3Socket, Q3SocketDevice, QHostAddress, QSocketNotifier
*/


/*!
    Creates a server socket object, that will serve the given \a port
    on all the addresses of this host. If \a port is 0, Q3ServerSocket
    will pick a suitable port in a system-dependent manner. Use \a
    backlog to specify how many pending connections the server can
    have.

    The \a parent and \a name arguments are passed on to the QObject
    constructor.

    \warning On Tru64 Unix systems a value of 0 for \a backlog means
    that you don't accept any connections at all; you should specify a
    value larger than 0.
*/

Q3ServerSocket::Q3ServerSocket( Q_UINT16 port, int backlog,
			      QObject *parent, const char *name )
    : QObject( parent, name )
{
    d = new Q3ServerSocketPrivate;
    init( QHostAddress(), port, backlog );
}


/*!
    Creates a server socket object, that will serve the given \a port
    only on the given \a address. Use \a backlog to specify how many
    pending connections the server can have.

    The \a parent and \a name arguments are passed on to the QObject
    constructor.

    \warning On Tru64 Unix systems a value of 0 for \a backlog means
    that you don't accept any connections at all; you should specify a
    value larger than 0.
*/

Q3ServerSocket::Q3ServerSocket( const QHostAddress & address, Q_UINT16 port,
			      int backlog,
			      QObject *parent, const char *name )
    : QObject( parent, name )
{
    d = new Q3ServerSocketPrivate;
    init( address, port, backlog );
}


/*!
    Construct an empty server socket.

    This constructor, in combination with setSocket(), allows us to
    use the Q3ServerSocket class as a wrapper for other socket types
    (e.g. Unix Domain Sockets under Unix).

    The \a parent and \a name arguments are passed on to the QObject
    constructor.

    \sa setSocket()
*/

Q3ServerSocket::Q3ServerSocket( QObject *parent, const char *name )
    : QObject( parent, name )
{
    d = new Q3ServerSocketPrivate;
}


/*!
    Returns true if the construction succeeded; otherwise returns false.
*/
bool Q3ServerSocket::ok() const
{
    return !!d->s;
}

/*
  The common bit of the constructors.
 */
void Q3ServerSocket::init( const QHostAddress & address, Q_UINT16 port, int backlog )
{
    d->s = new Q3SocketDevice( Q3SocketDevice::Stream, address.isIPv4Address()
			      ? Q3SocketDevice::IPv4 : Q3SocketDevice::IPv6, 0 );
#if !defined(Q_OS_WIN32)
    // Under Unix, we want to be able to use the port, even if a socket on the
    // same address-port is in TIME_WAIT. Under Windows this is possible anyway
    // -- furthermore, the meaning of reusable is different: it means that you
    // can use the same address-port for multiple listening sockets.
    d->s->setAddressReusable( true );
#endif
    if ( d->s->bind( address, port )
      && d->s->listen( backlog ) )
    {
	d->n = new QSocketNotifier( d->s->socket(), QSocketNotifier::Read,
				    this, "accepting new connections" );
	connect( d->n, SIGNAL(activated(int)),
		 this, SLOT(incomingConnection(int)) );
    } else {
	qWarning( "Q3ServerSocket: failed to bind or listen to the socket" );
	delete d->s;
	d->s = 0;
    }
}


/*!
    Destroys the socket.

    This causes any backlogged connections (connections that have
    reached the host, but not yet been completely set up by calling
    Q3SocketDevice::accept()) to be severed.

    Existing connections continue to exist; this only affects the
    acceptance of new connections.
*/
Q3ServerSocket::~Q3ServerSocket()
{
    delete d;
}


/*!
    \fn void Q3ServerSocket::newConnection( int socket )

    This pure virtual function is responsible for setting up a new
    incoming connection. \a socket is the fd (file descriptor) for the
    newly accepted connection.
*/


void Q3ServerSocket::incomingConnection( int )
{
    int fd = d->s->accept();
    if ( fd >= 0 )
	newConnection( fd );
}


/*!
    Returns the port number on which this server socket listens. This
    is always non-zero; if you specify 0 in the constructor,
    Q3ServerSocket will pick a non-zero port itself. ok() must be true
    before calling this function.

    \sa address() Q3SocketDevice::port()
*/
Q_UINT16 Q3ServerSocket::port() const
{
    if ( !d || !d->s )
	return 0;
    return d->s->port();
}


/*!
    Returns the operating system socket.
*/
int Q3ServerSocket::socket() const
{
    if ( !d || !d->s )
	return -1;

    return d->s->socket();
}

/*!
    Returns the address on which this object listens, or 0.0.0.0 if
    this object listens on more than one address. ok() must be true
    before calling this function.

    \sa port() Q3SocketDevice::address()
*/
QHostAddress Q3ServerSocket::address() const
{
    if ( !d || !d->s )
	return QHostAddress();

    return d->s->address();
}


/*!
    Returns a pointer to the internal socket device. The returned
    pointer is 0 if there is no connection or pending connection.

    There is normally no need to manipulate the socket device directly
    since this class does all the necessary setup for most client or
    server socket applications.
*/
Q3SocketDevice *Q3ServerSocket::socketDevice()
{
    if ( !d )
	return 0;

    return d->s;
}


/*!
    Sets the socket to use \a socket. bind() and listen() should
    already have been called for \a socket.

    This allows us to use the Q3ServerSocket class as a wrapper for
    other socket types (e.g. Unix Domain Sockets).
*/
void Q3ServerSocket::setSocket( int socket )
{
    delete d;
    d = new Q3ServerSocketPrivate;
    d->s = new Q3SocketDevice( socket, Q3SocketDevice::Stream );
    d->n = new QSocketNotifier( d->s->socket(), QSocketNotifier::Read,
	       this, "accepting new connections" );
    connect( d->n, SIGNAL(activated(int)),
	     this, SLOT(incomingConnection(int)) );
}

QT_END_NAMESPACE

#endif //QT_NO_NETWORK
