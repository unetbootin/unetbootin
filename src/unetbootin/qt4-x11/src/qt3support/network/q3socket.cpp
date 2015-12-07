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

#include "q3socket.h"
#ifndef QT_NO_NETWORK
#include "q3ptrlist.h"
#include "qtimer.h"
#include "q3socketdevice.h"
#include "q3dns.h"
#include "private/q3membuf_p.h"

#include <string.h>
#ifndef NO_ERRNO_H
#if defined(Q_OS_WINCE)
#include "qfunctions_wince.h"
#else
#include <errno.h>
#endif
#endif

QT_BEGIN_NAMESPACE

//#define Q3SOCKET_DEBUG

/*
  Perhaps this private functionality needs to be refactored.

  Comment from Robert D Gatlin (Intel):

    It would be nice to have the functionality inherent in Q3Socket available
    as a separate class as a standard part of the Qt library, something along
    the line of:

      class QByteBuffer : public QIODevice { ... }

    The same class could/would be used within Q3Socket for the Read/Write
    buffers.

    The above class could be used in the following way(s):

	buffer.open( IO_WriteOnly | IO_Append );
	buffer.writeBlock( a ); // a = QByteArray
	buffer.close();

	QByteArray b;
	b.resize( buffer.size() );
	buffer.open( IO_ReadOnly );
	buffer.readBlock( b.data(), b.size() );
	buffer.close();

    But would also be useable with QDataStream (via QIODevice) with:

	buffer.open( IO_WriteOnly | IO_Append );
	QDataStream is( &buffer );
	is << 100;
	buffer.close();

	buffer.open( IO_ReadOnly );
	QDataStream os( &buffer );
	Q_UINT32 x;
	os >> x;
	buffer.close();

    The real usefulness is with any situations where data (QByteArray) arrives
    incrementally (as in Q3Socket and filter case above).

    I tried using QBuffer, but QBuffer does not trim bytes from the front of
    the buffer in cases like:

	QBuffer buf;
	buf.open( IO_ReadOnly );
	QDataStream ds( &buf );
	Q_INT32 x;
	ds >> x;
	buf.close();

    In the above case, buf.size() will be identical before and after the
    operation with QDataStream. Based on the implementation of QBuffer, it
    does not appear well suited for this kind of operation.
*/

// Private class for Q3Socket

class Q3SocketPrivate {
public:
    Q3SocketPrivate();
   ~Q3SocketPrivate();
    void closeSocket();
    void close();
    void connectionClosed();
    void setSocketDevice( Q3Socket *q, Q3SocketDevice *device );

    Q3Socket::State	state;			// connection state
    QString		host;			// host name
    Q_UINT16		port;			// host port
    Q3SocketDevice      *socket;			// connection socket
    QSocketNotifier     *rsn, *wsn;		// socket notifiers
    Q3Membuf		rba;			// read buffer
    Q_ULONG		readBufferSize;		// limit for the read buffer size
    Q3PtrList<QByteArray> wba;			// list of write bufs
    QHostAddress	addr;			// connection address
    Q3ValueList<QHostAddress> addresses;		// alternatives looked up
    QIODevice::Offset	wsize;			// write total buf size
    QIODevice::Offset	windex;			// write index
#ifndef QT_NO_DNS
    Q3Dns	       *dns4;
    Q3Dns	       *dns6;
#endif
    static Q3PtrList<Q3Socket> sn_read_alreadyCalled; // used to avoid unwanted recursion
    Q3ValueList<QHostAddress> l4;
    Q3ValueList<QHostAddress> l6;
};

Q3PtrList<Q3Socket> Q3SocketPrivate::sn_read_alreadyCalled;

Q3SocketPrivate::Q3SocketPrivate()
    : state(Q3Socket::Idle), host(QString::fromLatin1("")), port(0),
      socket(0), rsn(0), wsn(0), readBufferSize(0), wsize(0), windex(0)
{
#ifndef QT_NO_DNS
    dns4 = 0;
    dns6 = 0;
#endif
    wba.setAutoDelete( true );
}

Q3SocketPrivate::~Q3SocketPrivate()
{
    close();
    delete socket;
#ifndef QT_NO_DNS
    delete dns4;
    delete dns6;
#endif
}

extern void qDeleteInEventHandler(QObject *o);
void Q3SocketPrivate::closeSocket()
{
    // Order is important here - the socket notifiers must go away
    // before the socket does, otherwise libc or the kernel will
    // become unhappy.
    if (rsn) {
        qDeleteInEventHandler(rsn);
        rsn = 0;
    }
    if (wsn) {
        qDeleteInEventHandler(wsn);
        wsn = 0;
    }
    if ( socket )
	socket->close();
}

void Q3SocketPrivate::close()
{
    closeSocket();
    wsize = 0;
    rba.clear(); wba.clear();
    windex = 0;
}

void Q3SocketPrivate::connectionClosed()
{
    // We keep the open state in case there's unread incoming data
    state = Q3Socket::Idle;
    closeSocket();
    wba.clear();
    windex = wsize = 0;
}

void Q3SocketPrivate::setSocketDevice( Q3Socket *q, Q3SocketDevice *device )
{
    delete socket;
    delete rsn;
    delete wsn;

    if ( device ) {
	socket = device;
    } else {
	socket = new Q3SocketDevice( Q3SocketDevice::Stream,
				    ( addr.isIPv4Address() ?
				      Q3SocketDevice::IPv4 :
				      Q3SocketDevice::IPv6 ), 0 );
	socket->setBlocking( false );
	socket->setAddressReusable( true );
    }

    rsn = new QSocketNotifier( socket->socket(),
			       QSocketNotifier::Read, q, "read" );
    wsn = new QSocketNotifier( socket->socket(),
			       QSocketNotifier::Write, q, "write" );

    QObject::connect( rsn, SIGNAL(activated(int)), q, SLOT(sn_read()) );
    rsn->setEnabled( false );
    QObject::connect( wsn, SIGNAL(activated(int)), q, SLOT(sn_write()) );
    wsn->setEnabled( false );
}

/*!
    \class Q3Socket q3socket.h
    \brief The Q3Socket class provides a buffered TCP connection.

    \compat

    It provides a totally non-blocking QIODevice, and modifies and
    extends the API of QIODevice with socket-specific code.

    The functions you're likely to call most are connectToHost(),
    bytesAvailable(), canReadLine() and the ones it inherits from
    QIODevice.

    connectToHost() is the most-used function. As its name implies,
    it opens a connection to a named host.

    Most network protocols are either packet-oriented or
    line-oriented. canReadLine() indicates whether a connection
    contains an entire unread line or not, and bytesAvailable()
    returns the number of bytes available for reading.

    The signals error(), connected(), readyRead() and
    connectionClosed() inform you of the progress of the connection.
    There are also some less commonly used signals. hostFound() is
    emitted when connectToHost() has finished its DNS lookup and is
    starting its TCP connection. delayedCloseFinished() is emitted
    when close() succeeds. bytesWritten() is emitted when Q3Socket
    moves data from its "to be written" queue into the TCP
    implementation.

    There are several access functions for the socket: state() returns
    whether the object is idle, is doing a DNS lookup, is connecting,
    has an operational connection, etc. address() and port() return
    the IP address and port used for the connection. The peerAddress()
    and peerPort() functions return the IP address and port used by
    the peer, and peerName() returns the name of the peer (normally
    the name that was passed to connectToHost()). socketDevice()
    returns a pointer to the Q3SocketDevice used for this socket.

    Q3Socket inherits QIODevice, and reimplements some functions. In
    general, you can treat it as a QIODevice for writing, and mostly
    also for reading. The match isn't perfect, since the QIODevice
    API is designed for devices that are controlled by the same
    machine, and an asynchronous peer-to-peer network connection isn't
    quite like that. For example, there is nothing that matches
    QIODevice::size() exactly. The documentation for open(), close(),
    flush(), size(), at(), atEnd(), readBlock(), writeBlock(),
    getch(), putch(), ungetch() and readLine() describes the
    differences in detail.

    \warning Q3Socket is not suitable for use in threads. If you need
    to uses sockets in threads use the lower-level Q3SocketDevice class.

    \sa Q3SocketDevice, QHostAddress, QSocketNotifier
*/


/*!
    Creates a Q3Socket object in Q3Socket::Idle state.

    The \a parent and \a name arguments are passed on to the QObject
    constructor.
*/

Q3Socket::Q3Socket( QObject *parent, const char *name )
    : QIODevice( parent )
{
    setObjectName(QLatin1String(name));
    d = new Q3SocketPrivate;
    setSocketDevice( 0 );
    resetStatus();
}


/*!
    Destroys the socket. Closes the connection if necessary.

    \sa close()
*/

Q3Socket::~Q3Socket()
{
#if defined(Q3SOCKET_DEBUG)
    qDebug( "Q3Socket (%s): Destroy", name() );
#endif
    if ( state() != Idle )
	close();
    Q_ASSERT( d != 0 );
    delete d;
}


/*!
    Returns a pointer to the internal socket device.

    There is normally no need to manipulate the socket device directly
    since this class does the necessary setup for most applications.
*/

Q3SocketDevice *Q3Socket::socketDevice()
{
    return d->socket;
}

/*!
    Sets the internal socket device to \a device. Passing a \a device
    of 0 will cause the internal socket device to be used. Any
    existing connection will be disconnected before using the new \a
    device.

    The new device should not be connected before being associated
    with a Q3Socket; after setting the socket call connectToHost() to
    make the connection.

    This function is useful if you need to subclass Q3SocketDevice and
    want to use the Q3Socket API, for example, to implement Unix domain
    sockets.
*/

void Q3Socket::setSocketDevice( Q3SocketDevice *device )
{
    if ( state() != Idle )
	close();
    d->setSocketDevice( this, device );
}

/*!
    \enum Q3Socket::State

    This enum defines the connection states:

    \value Idle if there is no connection
    \value HostLookup during a DNS lookup
    \value Connecting during TCP connection establishment
    \value Connected when there is an operational connection
    \value Closing if the socket is closing down, but is not yet closed.
    \omitvalue Connection
*/

/*!
    Returns the current state of the socket connection.

    \sa Q3Socket::State
*/

Q3Socket::State Q3Socket::state() const
{
    return d->state;
}


#ifndef QT_NO_DNS

/*!
    Attempts to make a connection to \a host on the specified \a port
    and return immediately.

    Any connection or pending connection is closed immediately, and
    Q3Socket goes into the \c HostLookup state. When the lookup
    succeeds, it emits hostFound(), starts a TCP connection and goes
    into the \c Connecting state. Finally, when the connection
    succeeds, it emits connected() and goes into the \c Connected
    state. If there is an error at any point, it emits error().

    \a host may be an IP address in string form, or it may be a DNS
    name. Q3Socket will do a normal DNS lookup if required. Note that
    \a port is in native byte order, unlike some other libraries.

    \sa state()
*/

void Q3Socket::connectToHost( const QString &host, Q_UINT16 port )
{
#if defined(Q3SOCKET_DEBUG)
    qDebug( "Q3Socket (%s)::connectToHost: host %s, port %d",
	    name(), host.ascii(), port );
#endif
    setSocketIntern( -1 );
    d->state = HostLookup;
    d->host = host;
    d->port = port;
    d->dns4 = new Q3Dns( host, Q3Dns::A );
    d->dns6 = new Q3Dns( host, Q3Dns::Aaaa );

    // try if the address is already available (for faster connecting...)
    tryConnecting();
    if ( d->state == HostLookup ) {
	connect( d->dns4, SIGNAL(resultsReady()),
		 this, SLOT(tryConnecting()) );
	connect( d->dns6, SIGNAL(resultsReady()),
		 this, SLOT(tryConnecting()) );
    }
}

#endif


/*!
    This private slots continues the connection process where
    connectToHost() leaves off.
*/

void Q3Socket::tryConnecting()
{
#if defined(Q3SOCKET_DEBUG)
    qDebug( "Q3Socket (%s)::tryConnecting()", name() );
#endif
    // ### this ifdef isn't correct - addresses() also does /etc/hosts and
    // numeric-address-as-string handling.
#ifndef QT_NO_DNS

    if ( d->dns4 ) {
	d->l4 = d->dns4->addresses();
	if ( !d->l4.isEmpty() || !d->dns4->isWorking() ) {
#if defined(Q3SOCKET_DEBUG)
	    qDebug( "Q3Socket (%s)::tryConnecting: host %s, port %d: "
		    "%d IPv4 addresses",
		    name(), d->host.ascii(), d->port, d->l4.count() );
#endif
	    delete d->dns4;
	    d->dns4 = 0;
	}
    }

    if ( d->dns6 ) {
	d->l6 = d->dns6->addresses();
	if ( !d->l6.isEmpty() || !d->dns6->isWorking() ) {
#if defined(Q3SOCKET_DEBUG)
	    qDebug( "Q3Socket (%s)::tryConnecting: host %s, port %d: "
		    "%d IPv6 addresses",
		    name(), d->host.ascii(), d->port, d->l6.count() );
#endif
	    delete d->dns6;
	    d->dns6 = 0;
	}
    }

    if ( d->state == HostLookup ) {
	if ( d->l4.isEmpty() && d->l6.isEmpty() &&
	     !d->dns4 && !d->dns6 ) {
	    // no results and we're not still looking: give up
	    d->state = Idle;
	    emit error( ErrHostNotFound );
	    return;
	}
	if ( d->l4.isEmpty() && d->l6.isEmpty() ) {
	    // no results (yet): try again later
	    return;
	}

	// we've found something. press on with that. if we later find
	// more, fine.
	emit hostFound();
	d->state = Connecting;
    }

    if ( d->state == Connecting ) {
	d->addresses += d->l4;
	d->addresses += d->l6;
	d->l4.clear();
	d->l6.clear();

	// try one address at a time, falling back to the next one if
	// there is a connection failure. (should also support a timeout,
	// or do multiple TCP-level connects at a time, with staggered
	// starts to avoid bandwidth waste and cause fewer
	// "connect-and-abort" errors. but that later.)
	bool stuck = true;
	while( stuck ) {
	    stuck = false;
	    if ( d->socket &&
		 d->socket->connect( d->addr, d->port ) == false ) {
		if ( d->socket->error() == Q3SocketDevice::NoError ) {
		    if ( d->wsn )
			d->wsn->setEnabled( true );
		    return; // not serious, try again later
		}

#if defined(Q3SOCKET_DEBUG)
		qDebug( "Q3Socket (%s)::tryConnecting: "
			"Gave up on IP address %s",
			name(), d->socket->peerAddress().toString().ascii() );
#endif
		delete d->wsn;
		d->wsn = 0;
		delete d->rsn;
		d->rsn = 0;
		delete d->socket;
		d->socket = 0;

                if(d->addresses.isEmpty()) {
                    emit error( ErrConnectionRefused );
                    return;
                }
            }
	    // if the host has more addresses, try another some.
	    if ( d->socket == 0 && !d->addresses.isEmpty() ) {
		d->addr = *d->addresses.begin();
		d->addresses.remove( d->addresses.begin() );
		d->setSocketDevice( this, 0 );
		stuck = true;
#if defined(Q3SOCKET_DEBUG)
		qDebug( "Q3Socket (%s)::tryConnecting: Trying IP address %s",
			name(), d->addr.toString().ascii() );
#endif
	    }
	};

	// The socket write notifier will fire when the connection succeeds
	if ( d->wsn )
	    d->wsn->setEnabled( true );
    }
#endif
}

/*!
    \enum Q3Socket::Error

    This enum specifies the possible errors:
    \value ErrConnectionRefused if the connection was refused
    \value ErrHostNotFound if the host was not found
    \value ErrSocketRead if a read from the socket failed
*/

/*!
    \fn void Q3Socket::error(int error)

    This signal is emitted after an error occurred. The \a error parameter is
    the \l Error value.
*/

/*!
    \fn void Q3Socket::hostFound()

    This signal is emitted after connectToHost() has been called and
    the host lookup has succeeded.

    \sa connected()
*/


/*!
    \fn void Q3Socket::connected()

    This signal is emitted after connectToHost() has been called and a
    connection has been successfully established.

    \sa connectToHost(), connectionClosed()
*/


/*!
    \fn void Q3Socket::connectionClosed()

    This signal is emitted when the other end has closed the
    connection. The read buffers may contain buffered input data which
    you can read after the connection was closed.

    \sa connectToHost(), close()
*/


/*!
    \fn void Q3Socket::delayedCloseFinished()

    This signal is emitted when a delayed close is finished.

    If you call close() and there is buffered output data to be
    written, Q3Socket goes into the Q3Socket::Closing state and
    returns immediately. It will then keep writing to the socket until
    all the data has been written. Then, the delayedCloseFinished()
    signal is emitted.

    \sa close()
*/


/*!
    \fn void Q3Socket::readyRead()

    This signal is emitted every time there is new incoming data.

    Bear in mind that new incoming data is only reported once; if you do not
    read all the data, this class buffers the data and you can read it later,
    but no signal is emitted unless new data arrives. A good practice is to
    read all data in the slot connected to this signal unless you are sure that
    you need to receive more data to be able to process it.

    \sa readBlock(), readLine(), bytesAvailable()
*/


/*!
    \fn void Q3Socket::bytesWritten( int nbytes )

    This signal is emitted when data has been written to the network.
    The \a nbytes parameter specifies how many bytes were written.

    The bytesToWrite() function is often used in the same context; it
    indicates how many buffered bytes there are left to write.

    \sa writeBlock(), bytesToWrite()
*/


/*!
    Opens the socket using the specified QIODevice file mode \a m.
    This function is called automatically when needed and you should
    not call it yourself.

    \sa close()
*/

bool Q3Socket::open( OpenMode m )
{
    if ( isOpen() ) {
#if defined(QT_CHECK_STATE)
	qWarning( "Q3Socket::open: Already open" );
#endif
	return false;
    }
    QIODevice::setOpenMode( m & ReadWrite );
    return true;
}

/*!
    \fn bool Q3Socket::open(int m)
    \overload
*/

/*!
    Closes the socket.

    The read buffer is cleared.

    If the output buffer is empty, the state is set to \c
    Q3Socket::Idle and the connection is terminated immediately. If the
    output buffer still contains data to be written, Q3Socket goes into
    the Q3Socket::Closing state and the rest of the data will be
    written. When all of the outgoing data have been written, the
    state is set to Q3Socket::Idle and the connection is terminated.
    At this point, the delayedCloseFinished() signal is emitted.

    If you don't want that the data of the output buffer is written, call
    clearPendingData() before you call close().

    \sa state(), bytesToWrite() clearPendingData()
*/

void Q3Socket::close()
{
    if ( !isOpen() || d->state == Idle )	// already closed
	return;
    if ( d->state == Closing )
	return;
    if ( !d->rsn || !d->wsn )
	return;
#if defined(Q3SOCKET_DEBUG)
    qDebug( "Q3Socket (%s): close socket", name() );
#endif
    if ( d->socket && d->wsize ) {		// there's data to be written
	d->state = Closing;
	if ( d->rsn )
	    d->rsn->setEnabled( false );
	if ( d->wsn )
	    d->wsn->setEnabled( true );
	d->rba.clear();				// clear incoming data
	return;
    }
    resetStatus();
    setOpenMode(NotOpen);
    d->close();
    d->state = Idle;
}


/*!
    This function consumes \a nbytes bytes of data from the write
    buffer.
*/

bool Q3Socket::consumeWriteBuf( Q_ULONG nbytes )
{
    if ( nbytes <= 0 || (qint64)nbytes > d->wsize )
	return false;
#if defined(Q3SOCKET_DEBUG)
    qDebug( "Q3Socket (%s): skipWriteBuf %d bytes", name(), (int)nbytes );
#endif
    d->wsize -= nbytes;
    for ( ;; ) {
	QByteArray *a = d->wba.first();
	if ( (qint64)(d->windex + nbytes) >= a->size() ) {
	    nbytes -= a->size() - d->windex;
	    d->wba.remove();
	    d->windex = 0;
	    if ( nbytes == 0 )
		break;
	} else {
	    d->windex += nbytes;
	    break;
	}
    }
    return true;
}



/*!
    Implementation of the abstract virtual QIODevice::flush() function.
    This function always returns true.
*/

bool Q3Socket::flush()
{
    if ( !d->socket )
        return true;
    bool osBufferFull = false;
    int consumed = 0;
    while ( !osBufferFull && d->state >= Connecting && d->wsize > 0 ) {
#if defined(Q3SOCKET_DEBUG)
	qDebug( "Q3Socket (%s): flush: Write data to the socket", name() );
#endif
	QByteArray *a = d->wba.first();
	int nwritten;
	int i = 0;
	if ( (int)a->size() - d->windex < 1460 ) {
	    // Concatenate many smaller blocks.  the first may be
	    // partial, but each subsequent block is copied entirely
	    // or not at all.  the sizes here are picked so that we
	    // generally won't trigger nagle's algorithm in the tcp
	    // implementation: we concatenate if we'd otherwise send
	    // less than PMTU bytes (we assume PMTU is 1460 bytes),
	    // and concatenate up to the largest payload TCP/IP can
	    // carry.  with these precautions, nagle's algorithm
	    // should apply only when really appropriate.
	    QByteArray out( 65536 );
	    int j = d->windex;
	    int s = a->size() - j;
	    while ( a && i+s < (int)out.size() ) {
		memcpy( out.data()+i, a->data()+j, s );
		j = 0;
		i += s;
		a = d->wba.next();
		s = a ? a->size() : 0;
	    }
	    nwritten = d->socket->write( out.data(), i );
	    if ( d->wsn )
		d->wsn->setEnabled( false ); // the QSocketNotifier documentation says so
	} else {
	    // Big block, write it immediately
	    i = a->size() - d->windex;
	    nwritten = d->socket->write( a->data() + d->windex, i );
	    if ( d->wsn )
		d->wsn->setEnabled( false ); // the QSocketNotifier documentation says so
	}
	if ( nwritten > 0 ) {
	    if ( consumeWriteBuf( nwritten ) )
		consumed += nwritten;
	}
	if ( nwritten < i )
	    osBufferFull = true;
    }
    if ( consumed > 0 ) {
#if defined(Q3SOCKET_DEBUG)
	qDebug( "Q3Socket (%s): flush: wrote %d bytes, %d left",
		name(), consumed, (int)d->wsize );
#endif
	emit bytesWritten( consumed );
    }
    if ( d->state == Closing && d->wsize == 0 ) {
#if defined(Q3SOCKET_DEBUG)
	qDebug( "Q3Socket (%s): flush: Delayed close done. Terminating.",
		name() );
#endif
	resetStatus();
	setOpenMode(NotOpen);
	d->close();
	d->state = Idle;
	emit delayedCloseFinished();
	return true;
    }
    if ( !d->socket->isOpen() ) {
	d->connectionClosed();
	emit connectionClosed();
	return true;
    }
    if ( d->wsn )
	d->wsn->setEnabled( d->wsize > 0 ); // write if there's data
    return true;
}


/*!
    Returns the number of incoming bytes that can be read right now
    (like bytesAvailable()).
*/

QIODevice::Offset Q3Socket::size() const
{
    return (Offset)bytesAvailable();
}


/*!
    Returns the current read index. Since Q3Socket is a sequential
    device, the current read index is always zero.
*/

QIODevice::Offset Q3Socket::at() const
{
    return 0;
}


/*!
    \overload

    Moves the read index forward to \a index and returns true if the
    operation was successful; otherwise returns false. Moving the
    index forward means skipping incoming data.
*/

bool Q3Socket::at( Offset index )
{
    if ( index > d->rba.size() )
	return false;
    d->rba.consumeBytes( (Q_ULONG)index, 0 );			// throw away data 0..index-1
    // After we read data from our internal buffer, if we use the
    // setReadBufferSize() to limit our buffer, we might now be able to
    // read more data in our buffer. So enable the read socket notifier,
    // but do this only if we are not in a slot connected to the
    // readyRead() signal since this might cause a bad recursive behavior.
    // We can test for this condition by looking at the
    // sn_read_alreadyCalled flag.
    if ( d->rsn && Q3SocketPrivate::sn_read_alreadyCalled.findRef(this) == -1 )
	d->rsn->setEnabled( true );
    return true;
}


/*!
    Returns true if there is no more data to read; otherwise returns false.
*/

bool Q3Socket::atEnd() const
{
    if ( d->socket == 0 )
	return true;
    Q3Socket * that = (Q3Socket *)this;
    if ( that->d->socket->bytesAvailable() )	// a little slow, perhaps...
	that->sn_read();
    return that->d->rba.size() == 0;
}


/*!
    Returns the number of incoming bytes that can be read, i.e. the
    size of the input buffer. Equivalent to size().

    \sa bytesToWrite()
*/

qint64 Q3Socket::bytesAvailable() const
{
    if ( d->socket == 0 )
	return 0;
    Q3Socket * that = (Q3Socket *)this;
    if ( that->d->socket->bytesAvailable() ) // a little slow, perhaps...
	(void)that->sn_read();
    return that->d->rba.size() + QIODevice::bytesAvailable();
}


/*!
    Wait up to \a msecs milliseconds for more data to be available.

    If \a msecs is -1 the call will block indefinitely.

    Returns the number of bytes available.

    If \a timeout is non-null and no error occurred (i.e. it does not
    return -1): this function sets *\a timeout to true, if the reason
    for returning was that the timeout was reached; otherwise it sets
    *\a timeout to false. This is useful to find out if the peer
    closed the connection.

    \warning This is a blocking call and should be avoided in event
    driven applications.

    \sa bytesAvailable()
*/

Q_ULONG Q3Socket::waitForMore( int msecs, bool *timeout ) const
{
    if ( d->socket == 0 )
	return 0;
    Q3Socket * that = (Q3Socket *)this;
    if ( that->d->socket->waitForMore( msecs, timeout ) > 0 )
	(void)that->sn_read( true );
    return that->d->rba.size();
}

/*! \overload
*/

Q_ULONG Q3Socket::waitForMore( int msecs ) const
{
    return waitForMore( msecs, 0 );
}

/*!
    Returns the number of bytes that are waiting to be written, i.e.
    the size of the output buffer.

    \sa bytesAvailable() clearPendingData()
*/

qint64 Q3Socket::bytesToWrite() const
{
    return d->wsize;
}

/*!
    Deletes the data that is waiting to be written. This is useful if you want
    to close the socket without waiting for all the data to be written.

    \sa bytesToWrite() close() delayedCloseFinished()
*/

void Q3Socket::clearPendingData()
{
    d->wba.clear();
    d->windex = d->wsize = 0;
}

/*!
    Reads \a maxlen bytes from the socket into \a data and returns the
    number of bytes read. Returns -1 if an error occurred.
*/

qint64 Q3Socket::readData( char *data, qint64 maxlen )
{
    if ( data == 0 && maxlen != 0 ) {
#if defined(QT_CHECK_NULL)
	qWarning( "Q3Socket::readBlock: Null pointer error" );
#endif
	return -1;
    }
    if ( !isOpen() ) {
#if defined(QT_CHECK_STATE)
	qWarning( "Q3Socket::readBlock: Socket is not open" );
#endif
	return -1;
    }
    if ( maxlen >= d->rba.size() )
	maxlen = d->rba.size();
#if defined(Q3SOCKET_DEBUG)
    qDebug( "Q3Socket (%s): readBlock %d bytes", name(), (int)maxlen );
#endif
    d->rba.consumeBytes( maxlen, data );
    // After we read data from our internal buffer, if we use the
    // setReadBufferSize() to limit our buffer, we might now be able to
    // read more data in our buffer. So enable the read socket notifier,
    // but do this only if we are not in a slot connected to the
    // readyRead() signal since this might cause a bad recursive behavior.
    // We can test for this condition by looking at the
    // sn_read_alreadyCalled flag.
    if ( d->rsn && Q3SocketPrivate::sn_read_alreadyCalled.findRef(this) == -1 )
	d->rsn->setEnabled( true );
    return maxlen;
}


/*!
    Writes \a len bytes to the socket from \a data and returns the
    number of bytes written. Returns -1 if an error occurred.
*/

qint64 Q3Socket::writeData( const char *data, qint64 len )
{
#if defined(QT_CHECK_NULL)
    if ( data == 0 && len != 0 ) {
	qWarning( "Q3Socket::writeBlock: Null pointer error" );
    }
#endif
#if defined(QT_CHECK_STATE)
    if ( !isOpen() ) {
	qWarning( "Q3Socket::writeBlock: Socket is not open" );
	return -1;
    }
#endif
#if defined(QT_CHECK_STATE)
    if ( d->state == Closing ) {
	qWarning( "Q3Socket::writeBlock: Cannot write, socket is closing" );
    }
#endif
    if ( len == 0 || d->state == Closing || d->state == Idle )
	return 0;
    QByteArray *a = d->wba.last();

    // next bit is sensitive.  if we're writing really small chunks,
    // try to buffer up since system calls are expensive, and nagle's
    // algorithm is even more expensive.  but if anything even
    // remotely large is being written, try to issue a write at once.

    bool writeNow = ( d->wsize + len >= 1400 || len > 512 );

    if ( a && a->size() + len < 128 ) {
	// small buffer, resize
	int i = a->size();
	a->resize( i+len );
	memcpy( a->data()+i, data, len );
    } else {
	// append new buffer
	a = new QByteArray( len );
	memcpy( a->data(), data, len );
	d->wba.append( a );
    }
    d->wsize += len;
    if ( writeNow )
	flush();
    else if ( d->wsn )
	d->wsn->setEnabled( true );
#if defined(Q3SOCKET_DEBUG)
    qDebug( "Q3Socket (%s): writeBlock %d bytes", name(), (int)len );
#endif
    return len;
}


/*!
    Reads a single byte/character from the internal read buffer.
    Returns the byte/character read, or -1 if there is nothing to be
    read.

    \sa bytesAvailable(), putch()
*/

int Q3Socket::getch()
{
    if ( isOpen() && d->rba.size() > 0 ) {
	uchar c;
	d->rba.consumeBytes( 1, (char*)&c );
	// After we read data from our internal buffer, if we use the
	// setReadBufferSize() to limit our buffer, we might now be able to
	// read more data in our buffer. So enable the read socket notifier,
	// but do this only if we are not in a slot connected to the
	// readyRead() signal since this might cause a bad recursive behavior.
	// We can test for this condition by looking at the
	// sn_read_alreadyCalled flag.
	if ( d->rsn && Q3SocketPrivate::sn_read_alreadyCalled.findRef(this) == -1 )
	    d->rsn->setEnabled( true );
	return c;
    }
    return -1;
}


/*!
    Writes the character \a ch to the output buffer.

    Returns \a ch, or -1 if an error occurred.

    \sa getch()
*/

int Q3Socket::putch( int ch )
{
    char buf[2];
    buf[0] = ch;
    return writeBlock(buf, 1) == 1 ? ch : -1;
}


/*!
    This implementation of the virtual function QIODevice::ungetch()
    prepends the character \a ch to the read buffer so that the next
    read returns this character as the first character of the output.
*/

int Q3Socket::ungetch( int ch )
{
#if defined(QT_CHECK_STATE)
    if ( !isOpen() ) {
	qWarning( "Q3Socket::ungetch: Socket not open" );
	return -1;
    }
#endif
    return d->rba.ungetch( ch );
}


/*!
    Returns true if it's possible to read an entire line of text from
    this socket at this time; otherwise returns false.

    Note that if the peer closes the connection unexpectedly, this
    function returns false. This means that loops such as this won't
    work:

    \snippet doc/src/snippets/code/src_qt3support_network_q3socket.cpp 0

    \sa readLine()
*/

bool Q3Socket::canReadLine() const
{
    if ( ((Q3Socket*)this)->d->rba.scanNewline( 0 ) )
	return true;
    return ( bytesAvailable() > 0 &&
	     (((Q3Socket*)this)->d->rba.scanNewline( 0 ) || QIODevice::canReadLine()) );
}

/*!
  \internal
    Internal slot for handling socket read notifications.

    This function has can usually only be entered once (i.e. no
    recursive calls). If the argument \a force is true, the function
    is executed, but no readyRead() signals are emitted. This
    behaviour is useful for the waitForMore() function, so that it is
    possible to call waitForMore() in a slot connected to the
    readyRead() signal.
*/

void Q3Socket::sn_read( bool force )
{
    Q_LONG maxToRead = 0;
    if ( d->readBufferSize > 0 ) {
	maxToRead = d->readBufferSize - d->rba.size();
	if ( maxToRead <= 0 ) {
	    if ( d->rsn )
		d->rsn->setEnabled( false );
	    return;
	}
    }

    // Use Q3SocketPrivate::sn_read_alreadyCalled to avoid recursive calls of
    // sn_read() (and as a result avoid emitting the readyRead() signal in a
    // slot for readyRead(), if you use bytesAvailable()).
    if ( !force && Q3SocketPrivate::sn_read_alreadyCalled.findRef(this) != -1 )
	return;
    Q3SocketPrivate::sn_read_alreadyCalled.append( this );

    char buf[4096];
    Q_LONG nbytes = d->socket->bytesAvailable();
    Q_LONG nread;
    QByteArray *a = 0;

    if ( state() == Connecting ) {
	if ( nbytes > 0 ) {
	    tryConnection();
	} else {
	    // nothing to do, nothing to care about
	    Q3SocketPrivate::sn_read_alreadyCalled.removeRef( this );
	    return;
	}
    }
    if ( state() == Idle ) {
	Q3SocketPrivate::sn_read_alreadyCalled.removeRef( this );
	return;
    }

    if ( nbytes <= 0 ) {			// connection closed?
	// On Windows this may happen when the connection is still open.
	// This happens when the system is heavily loaded and we have
	// read all the data on the socket before a new WSAAsyncSelect
	// event is processed. A new read operation would then block.
	// This code is also useful when Q3Socket is used without an
	// event loop.
	nread = d->socket->readBlock( buf, maxToRead ? QMIN((Q_LONG)sizeof(buf),maxToRead) : sizeof(buf) );
	if ( nread == 0 ) {			// really closed
            if ( !d->socket->isOpen() ) {
#if defined(Q3SOCKET_DEBUG)
                qDebug( "Q3Socket (%s): sn_read: Connection closed", name() );
#endif
                d->connectionClosed();
                emit connectionClosed();
            }
	    Q3SocketPrivate::sn_read_alreadyCalled.removeRef( this );
	    return;
	} else {
	    if ( nread < 0 ) {
		if ( d->socket->error() == Q3SocketDevice::NoError ) {
		    // all is fine
		    Q3SocketPrivate::sn_read_alreadyCalled.removeRef( this );
		    return;
		}
#if defined(Q3SOCKET_DEBUG)
		qWarning( "Q3Socket::sn_read (%s): Close error", name() );
#endif
		if ( d->rsn )
		    d->rsn->setEnabled( false );
		emit error( ErrSocketRead );
		Q3SocketPrivate::sn_read_alreadyCalled.removeRef( this );
		return;
	    }
	    a = new QByteArray( nread );
	    memcpy( a->data(), buf, nread );
	}

    } else {					// data to be read
#if defined(Q3SOCKET_DEBUG)
	qDebug( "Q3Socket (%s): sn_read: %ld incoming bytes", name(), nbytes );
#endif
	if ( nbytes > (int)sizeof(buf) ) {
	    // big
	    a = new QByteArray( nbytes );
	    nread = d->socket->readBlock( a->data(), maxToRead ? QMIN(nbytes,maxToRead) : nbytes );
	} else {
	    a = 0;
	    nread = d->socket->readBlock( buf, maxToRead ? QMIN((Q_LONG)sizeof(buf),maxToRead) : sizeof(buf) );
	    if ( nread > 0 ) {
		// ##### could setRawData
		a = new QByteArray( nread );
		memcpy( a->data(), buf, nread );
	    }
	}
	if ( nread == 0 ) {
#if defined(Q3SOCKET_DEBUG)
	    qDebug( "Q3Socket (%s): sn_read: Connection closed", name() );
#endif
	    // ### we should rather ask the socket device if it is closed
	    d->connectionClosed();
	    emit connectionClosed();
	    Q3SocketPrivate::sn_read_alreadyCalled.removeRef( this );
            delete a;
	    return;
	} else if ( nread < 0 ) {
            delete a;

	    if ( d->socket->error() == Q3SocketDevice::NoError ) {
		// all is fine
		Q3SocketPrivate::sn_read_alreadyCalled.removeRef( this );
		return;
	    }
#if defined(QT_CHECK_RANGE)
	    qWarning( "Q3Socket::sn_read: Read error" );
#endif
	    if ( d->rsn )
		d->rsn->setEnabled( false );
	    emit error( ErrSocketRead );
	    Q3SocketPrivate::sn_read_alreadyCalled.removeRef( this );
	    return;
	}
	if ( nread != (int)a->size() ) {		// unexpected
#if defined(CHECK_RANGE) && !defined(Q_OS_WIN32)
	    qWarning( "Q3Socket::sn_read: Unexpected short read" );
#endif
	    a->resize( nread );
	}
    }
    d->rba.append( a );
    if ( !force ) {
	if ( d->rsn )
	    d->rsn->setEnabled( false );
	emit readyRead();
	if ( d->rsn )
	    d->rsn->setEnabled( true );
    }

    Q3SocketPrivate::sn_read_alreadyCalled.removeRef( this );
}


/*!
  \internal
    Internal slot for handling socket write notifications.
*/

void Q3Socket::sn_write()
{
    if ( d->state == Connecting )		// connection established?
	tryConnection();
    flush();
}

void Q3Socket::emitErrorConnectionRefused()
{
    emit error( ErrConnectionRefused );
}

void Q3Socket::tryConnection()
{
    if ( d->socket->connect( d->addr, d->port ) ) {
	d->state = Connected;
#if defined(Q3SOCKET_DEBUG)
	qDebug( "Q3Socket (%s): sn_write: Got connection to %s",
		name(), peerName().ascii() );
#endif
	if ( d->rsn )
	    d->rsn->setEnabled( true );
	emit connected();
    } else {
	d->state = Idle;
	QTimer::singleShot( 0, this, SLOT(emitErrorConnectionRefused()) );
	return;
    }
}


/*!
    Returns the socket number, or -1 if there is no socket at the moment.
*/

int Q3Socket::socket() const
{
    if ( d->socket == 0 )
	return -1;
    return d->socket->socket();
}

/*!
    Sets the socket to use \a socket and the state() to \c Connected.
    The socket must already be connected.

    This allows us to use the Q3Socket class as a wrapper for other
    socket types (e.g. Unix Domain Sockets).
*/

void Q3Socket::setSocket( int socket )
{
    setSocketIntern( socket );
    d->state = Connection;
    d->rsn->setEnabled( true );
}


/*!
    Sets the socket to \a socket. This is used by both setSocket() and
    connectToHost() and can also be used on unconnected sockets.
*/

void Q3Socket::setSocketIntern( int socket )
{
    if ( state() != Idle ) {
	clearPendingData();
        close();
    }
    Q_ULONG oldBufferSize = d ? d->readBufferSize : 0;
    delete d;

    d = new Q3SocketPrivate;
    if (oldBufferSize)
        d->readBufferSize = oldBufferSize;
    if ( socket >= 0 ) {
	Q3SocketDevice *sd = new Q3SocketDevice( socket, Q3SocketDevice::Stream );
	sd->setBlocking( false );
	sd->setAddressReusable( true );
	d->setSocketDevice( this, sd );
    }
    d->state = Idle;

    // Initialize the IO device flags
    resetStatus();
    open( IO_ReadWrite );

    // hm... this is not very nice.
    d->host.clear();
    d->port = 0;
#ifndef QT_NO_DNS
    delete d->dns4;
    d->dns4 = 0;
    delete d->dns6;
    d->dns6 = 0;
#endif
}


/*!
    Returns the host port number of this socket, in native byte order.
*/

Q_UINT16 Q3Socket::port() const
{
    if ( d->socket == 0 )
	return 0;
    return d->socket->port();
}


/*!
    Returns the peer's host port number, normally as specified to the
    connectToHost() function. If none has been set, this function
    returns 0.

    Note that Qt always uses native byte order, i.e. 67 is 67 in Qt;
    there is no need to call htons().
*/

Q_UINT16 Q3Socket::peerPort() const
{
    if ( d->socket == 0 )
	return 0;
    return d->socket->peerPort();
}


/*!
    Returns the host address of this socket. (This is normally the
    main IP address of the host, but can be e.g. 127.0.0.1 for
    connections to localhost.)
*/

QHostAddress Q3Socket::address() const
{
    if ( d->socket == 0 ) {
	QHostAddress tmp;
	return tmp;
    }
    return d->socket->address();
}


/*!
    Returns the address of the connected peer if the socket is in
    Connected state; otherwise an empty QHostAddress is returned.
*/

QHostAddress Q3Socket::peerAddress() const
{
    if ( d->socket == 0 ) {
	QHostAddress tmp;
	return tmp;
    }
    return d->socket->peerAddress();
}


/*!
    Returns the host name as specified to the connectToHost()
    function. An empty string is returned if none has been set.
*/

QString Q3Socket::peerName() const
{
    return d->host;
}

/*!
    Sets the size of the Q3Socket's internal read buffer to \a bufSize.

    Usually Q3Socket reads all data that is available from the operating
    system's socket. If the buffer size is limited to a certain size, this
    means that the Q3Socket class doesn't buffer more than this size of data.

    If the size of the read buffer is 0, the read buffer is unlimited and all
    incoming data is buffered. This is the default.

    If you read the data in the readyRead() signal, you shouldn't use this
    option since it might slow down your program unnecessary. This option is
    useful if you only need to read the data at certain points in time, like in
    a realtime streaming application.

    \sa readBufferSize()
*/

void Q3Socket::setReadBufferSize( Q_ULONG bufSize )
{
    d->readBufferSize = bufSize;
}

/*!
    Returns the size of the read buffer.

    \sa setReadBufferSize()
*/

Q_ULONG Q3Socket::readBufferSize() const
{
    return d->readBufferSize;
}

/*!
    \fn bool Q3Socket::isSequential() const
    \internal
*/

QT_END_NAMESPACE

#endif //QT_NO_NETWORK
