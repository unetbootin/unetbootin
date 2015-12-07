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

#include "q3networkprotocol.h"

#ifndef QT_NO_NETWORKPROTOCOL

#include "q3localfs.h"
#include "q3urloperator.h"
#include "qtimer.h"
#include "qmap.h"
#include "q3ptrqueue.h"
#include "q3valuelist.h"
#include "qurlinfo.h"
#include <private/qobject_p.h>

QT_BEGIN_NAMESPACE

//#define Q3NETWORKPROTOCOL_DEBUG
#define NETWORK_OP_DELAY 1000

extern Q_COMPAT_EXPORT Q3NetworkProtocolDict *q3networkProtocolRegister;

Q3NetworkProtocolDict *q3networkProtocolRegister = 0;

class Q3NetworkProtocolPrivate
{
public:
    Q3NetworkProtocolPrivate( Q3NetworkProtocol *p )
    {
	url = 0;
	opInProgress = 0;
	opStartTimer = new QTimer( p );
	removeTimer = new QTimer( p );
	operationQueue.setAutoDelete( false );
	autoDelete = false;
	removeInterval = 10000;
	oldOps.setAutoDelete( false );
    }

    ~Q3NetworkProtocolPrivate()
    {
	removeTimer->stop();
	if ( opInProgress ) {
	    if ( opInProgress == operationQueue.head() )
		operationQueue.dequeue();
	    opInProgress->free();
	}
	while ( operationQueue.head() ) {
	    operationQueue.head()->free();
	    operationQueue.dequeue();
	}
	while ( oldOps.first() ) {
	    oldOps.first()->free();
	    oldOps.removeFirst();
	}
	delete opStartTimer;
    }

    Q3UrlOperator *url;
    Q3PtrQueue< Q3NetworkOperation > operationQueue;
    Q3NetworkOperation *opInProgress;
    QTimer *opStartTimer, *removeTimer;
    int removeInterval;
    bool autoDelete;
    Q3PtrList< Q3NetworkOperation > oldOps;
};

/*!
    \class Q3NetworkProtocol q3networkprotocol.h
    \brief The Q3NetworkProtocol class provides a common API for network protocols.

    \compat

    This is a base class which should be used for network protocols
    implementations that can then be used in Qt (e.g. in the file
    dialog) together with the Q3UrlOperator.

    The easiest way to implement a new network protocol is to
    reimplement the operation*() methods, e.g. operationGet(), etc.
    Only the supported operations should be reimplemented. To specify
    which operations are supported, also reimplement
    supportedOperations() and return an int that is OR'd together
    using the supported operations from the \l
    Q3NetworkProtocol::Operation enum.

    When you implement a network protocol this way, it is important to
    emit the correct signals. Also, always emit the finished() signal
    when an operation is done (on success \e and on failure). Qt
    relies on correctly emitted finished() signals.
*/

/*!
    \fn void Q3NetworkProtocol::newChildren( const Q3ValueList<QUrlInfo> &i, Q3NetworkOperation *op )

    This signal is emitted after listChildren() was called and new
    children (files) have been read from the list of files. \a i holds
    the information about the new children. \a op is the pointer to
    the operation object which contains all the information about the
    operation, including the state, etc.

    When a protocol emits this signal, Q3NetworkProtocol is smart
    enough to let the Q3UrlOperator, which is used by the network
    protocol, emit its corresponding signal.

    When implementing your own network protocol and reading children,
    you usually don't read one child at once, but rather a list of
    them. That's why this signal takes a list of QUrlInfo objects. If
    you prefer to read just one child at a time you can use the
    convenience signal newChild(), which takes a single QUrlInfo
    object.
*/

/*!
    \fn void Q3NetworkProtocol::newChild( const QUrlInfo &i, Q3NetworkOperation *op )

    This signal is emitted if a new child (file) has been read.
    Q3NetworkProtocol automatically connects it to a slot which creates
    a list of QUrlInfo objects (with just one QUrlInfo \a i) and emits
    the newChildren() signal with this list. \a op is the pointer to
    the operation object which contains all the information about the
    operation that has finished, including the state, etc.

    This is just a convenience signal useful for implementing your own
    network protocol. In all other cases connect to the newChildren()
    signal with its list of QUrlInfo objects.
*/

/*!
    \fn void Q3NetworkProtocol::finished( Q3NetworkOperation *op )

    This signal is emitted when an operation finishes. This signal is
    always emitted, for both success and failure. \a op is the pointer
    to the operation object which contains all the information about
    the operation, including the state, etc. Check the state and error
    code of the operation object to determine whether or not the
    operation was successful.

    When a protocol emits this signal, Q3NetworkProtocol is smart
    enough to let the Q3UrlOperator, which is used by the network
    protocol, emit its corresponding signal.
*/

/*!
    \fn void Q3NetworkProtocol::start( Q3NetworkOperation *op )

    Some operations (such as listChildren()) emit this signal when
    they start processing the operation. \a op is the pointer to the
    operation object which contains all the information about the
    operation, including the state, etc.

    When a protocol emits this signal, Q3NetworkProtocol is smart
    enough to let the Q3UrlOperator, which is used by the network
    protocol, emit its corresponding signal.
*/

/*!
    \fn void Q3NetworkProtocol::createdDirectory( const QUrlInfo &i, Q3NetworkOperation *op )

    This signal is emitted when mkdir() has been successful and the
    directory has been created. \a i holds the information about the
    new directory. \a op is the pointer to the operation object which
    contains all the information about the operation, including the
    state, etc. Using op->arg( 0 ), you can get the file name of the
    new directory.

    When a protocol emits this signal, Q3NetworkProtocol is smart
    enough to let the Q3UrlOperator, which is used by the network
    protocol, emit its corresponding signal.
*/

/*!
    \fn void Q3NetworkProtocol::removed( Q3NetworkOperation *op )

    This signal is emitted when remove() has been succesiisful and the
    file has been removed. \a op holds the file name of the removed
    file in the first argument, accessible with op->arg( 0 ). \a op is
    the pointer to the operation object which contains all the
    information about the operation, including the state, etc.

    When a protocol emits this signal, Q3NetworkProtocol is smart
    enough to let the Q3UrlOperator, which is used by the network
    protocol, emit its corresponding signal.
*/

/*!
    \fn void Q3NetworkProtocol::itemChanged( Q3NetworkOperation *op )

    This signal is emitted whenever a file which is a child of this
    URL has been changed, e.g. by successfully calling rename(). \a op
    holds the original and the new file names in the first and second
    arguments, accessible with op->arg( 0 ) and op->arg( 1 )
    respectively. \a op is the pointer to the operation object which
    contains all the information about the operation, including the
    state, etc.

    When a protocol emits this signal, Q3NetworkProtocol is smart
    enough to let the Q3UrlOperator, which is used by the network
    protocol, emit its corresponding signal.
*/

/*!
    \fn void Q3NetworkProtocol::data( const QByteArray &data,
    Q3NetworkOperation *op )

    This signal is emitted when new \a data has been received after
    calling get() or put(). \a op holds the name of the file from
    which data is retrieved or uploaded in its first argument, and the
    (raw) data in its second argument. You can get them with
    op->arg( 0 ) and op->rawArg( 1 ). \a op is the pointer to the
    operation object, which contains all the information about the
    operation, including the state, etc.

    When a protocol emits this signal, Q3NetworkProtocol is smart
    enough to let the Q3UrlOperator (which is used by the network
    protocol) emit its corresponding signal.
*/

/*!
    \fn void Q3NetworkProtocol::dataTransferProgress( int bytesDone, int bytesTotal, Q3NetworkOperation *op )

    This signal is emitted during the transfer of data (using put() or
    get()). \a bytesDone is how many bytes of \a bytesTotal have been
    transferred. \a bytesTotal may be -1, which means that the total
    number of bytes is not known. \a op is the pointer to the
    operation object which contains all the information about the
    operation, including the state, etc.

    When a protocol emits this signal, Q3NetworkProtocol is smart
    enough to let the Q3UrlOperator, which is used by the network
    protocol, emit its corresponding signal.
*/

/*!
    \fn void Q3NetworkProtocol::connectionStateChanged( int state, const QString &data )

    This signal is emitted whenever the state of the connection of the
    network protocol is changed. \a state describes the new state,
    which is one of, \c ConHostFound, \c ConConnected or \c ConClosed.
    \a data is a message text.
*/

/*!
    \enum Q3NetworkProtocol::State

    This enum contains the state that a Q3NetworkOperation can have.

    \value StWaiting  The operation is in the Q3NetworkProtocol's queue
    waiting to be prcessed.

    \value StInProgress  The operation is being processed.

    \value StDone  The operation has been processed successfully.

    \value StFailed  The operation has been processed but an error occurred.

    \value StStopped  The operation has been processed but has been
    stopped before it finished, and is waiting to be processed.

*/

/*!
    \enum Q3NetworkProtocol::Operation

    This enum lists the possible operations that a network protocol
    can support. supportedOperations() returns an int of these that is
    OR'd together. Also, the type() of a Q3NetworkOperation is always
    one of these values.

    \value OpListChildren  List the children of a URL, e.g. of a directory.
    \value OpMkDir  Create a directory.
    \value OpRemove  Remove a child (e.g. a file).
    \value OpRename  Rename a child (e.g. a file).
    \value OpGet  Get data from a location.
    \value OpPut  Put data to a location.
    \omitvalue OpMkdir
*/

/*!
    \enum Q3NetworkProtocol::ConnectionState

    When the connection state of a network protocol changes it emits
    the signal connectionStateChanged(). The first argument is one of
    the following values:

    \value ConHostFound  Host has been found.
    \value ConConnected  Connection to the host has been established.
    \value ConClosed  Connection has been closed.
*/

/*!
    \enum Q3NetworkProtocol::Error

    When an operation fails (finishes unsuccessfully), the
    Q3NetworkOperation of the operation returns an error code which has
    one of the following values:

    \value NoError  No error occurred.

    \value ErrValid  The URL you are operating on is not valid.

    \value ErrUnknownProtocol  There is no protocol implementation
    available for the protocol of the URL you are operating on (e.g.
    if the protocol is http and no http implementation has been
    registered).

    \value ErrUnsupported  The operation is not supported by the
    protocol.

    \value ErrParse  The URL could not be parsed correctly.

    \value ErrLoginIncorrect  You needed to login but the username
    or password is wrong.

    \value ErrHostNotFound  The specified host (in the URL) couldn't
    be found.

    \value ErrListChildren  An error occurred while listing the
    children (files).

    \value ErrMkDir  An error occurred when creating a directory.

    \value ErrRemove  An error occurred when removing a child (file).

    \value ErrRename   An error occurred when renaming a child (file).

    \value ErrGet  An error occurred while getting (retrieving) data.

    \value ErrPut  An error occurred while putting (uploading) data.

    \value ErrFileNotExisting  A file which is needed by the operation
    doesn't exist.

    \value ErrPermissionDenied  Permission for doing the operation has
    been denied.
    \omitvalue ErrMkdir
    \omitvalue ErrListChlidren

    You should also use these error codes when implementing custom
    network protocols. If this is not possible, you can define your own
    error codes by using integer values that don't conflict with any
    of these values.
*/

/*!
    Constructor of the network protocol base class. Does some
    initialization and connecting of signals and slots.
*/

Q3NetworkProtocol::Q3NetworkProtocol()
    : QObject()
{
    d = new Q3NetworkProtocolPrivate( this );

    connect( d->opStartTimer, SIGNAL(timeout()),
	     this, SLOT(startOps()) );
    connect( d->removeTimer, SIGNAL(timeout()),
	     this, SLOT(removeMe()) );

    if ( url() ) {
	connect( this, SIGNAL(data(QByteArray,Q3NetworkOperation*)),
		 url(), SIGNAL(data(QByteArray,Q3NetworkOperation*)) );
	connect( this, SIGNAL(finished(Q3NetworkOperation*)),
		 url(), SIGNAL(finished(Q3NetworkOperation*)) );
	connect( this, SIGNAL(start(Q3NetworkOperation*)),
		 url(), SIGNAL(start(Q3NetworkOperation*)) );
	connect( this, SIGNAL(newChildren(Q3ValueList<QUrlInfo>,Q3NetworkOperation*)),
		 url(), SIGNAL(newChildren(Q3ValueList<QUrlInfo>,Q3NetworkOperation*)) );
	connect( this, SIGNAL(newChildren(Q3ValueList<QUrlInfo>,Q3NetworkOperation*)),
		 url(), SLOT(addEntry(Q3ValueList<QUrlInfo>)) );
	connect( this, SIGNAL(createdDirectory(QUrlInfo,Q3NetworkOperation*)),
		 url(), SIGNAL(createdDirectory(QUrlInfo,Q3NetworkOperation*)) );
	connect( this, SIGNAL(removed(Q3NetworkOperation*)),
		 url(), SIGNAL(removed(Q3NetworkOperation*)) );
	connect( this, SIGNAL(itemChanged(Q3NetworkOperation*)),
		 url(), SIGNAL(itemChanged(Q3NetworkOperation*)) );
	connect( this, SIGNAL(dataTransferProgress(int,int,Q3NetworkOperation*)),
		 url(), SIGNAL(dataTransferProgress(int,int,Q3NetworkOperation*)) );
	connect( this, SIGNAL(connectionStateChanged(int,QString)),
		 url(), SIGNAL(connectionStateChanged(int,QString)) );
    }

    connect( this, SIGNAL(finished(Q3NetworkOperation*)),
	     this, SLOT(processNextOperation(Q3NetworkOperation*)) );
    connect( this, SIGNAL(newChild(QUrlInfo,Q3NetworkOperation*)),
	     this, SLOT(emitNewChildren(QUrlInfo,Q3NetworkOperation*)) );

}

/*!
    Destructor.
*/

Q3NetworkProtocol::~Q3NetworkProtocol()
{
    delete d;
}

/*!
    Sets the Q3UrlOperator, on which the protocol works, to \a u.

    \sa Q3UrlOperator
*/

void Q3NetworkProtocol::setUrl( Q3UrlOperator *u )
{
    if ( url() ) {
	disconnect( this, SIGNAL(data(QByteArray,Q3NetworkOperation*)),
		    url(), SIGNAL(data(QByteArray,Q3NetworkOperation*)) );
	disconnect( this, SIGNAL(finished(Q3NetworkOperation*)),
		    url(), SIGNAL(finished(Q3NetworkOperation*)) );
	disconnect( this, SIGNAL(start(Q3NetworkOperation*)),
		    url(), SIGNAL(start(Q3NetworkOperation*)) );
	disconnect( this, SIGNAL(newChildren(Q3ValueList<QUrlInfo>,Q3NetworkOperation*)),
		    url(), SIGNAL(newChildren(Q3ValueList<QUrlInfo>,Q3NetworkOperation*)) );
	disconnect( this, SIGNAL(newChildren(Q3ValueList<QUrlInfo>,Q3NetworkOperation*)),
		    url(), SLOT(addEntry(Q3ValueList<QUrlInfo>)) );
	disconnect( this, SIGNAL(createdDirectory(QUrlInfo,Q3NetworkOperation*)),
		    url(), SIGNAL(createdDirectory(QUrlInfo,Q3NetworkOperation*)) );
	disconnect( this, SIGNAL(removed(Q3NetworkOperation*)),
		    url(), SIGNAL(removed(Q3NetworkOperation*)) );
	disconnect( this, SIGNAL(itemChanged(Q3NetworkOperation*)),
		    url(), SIGNAL(itemChanged(Q3NetworkOperation*)) );
	disconnect( this, SIGNAL(dataTransferProgress(int,int,Q3NetworkOperation*)),
		    url(), SIGNAL(dataTransferProgress(int,int,Q3NetworkOperation*)) );
	disconnect( this, SIGNAL(connectionStateChanged(int,QString)),
		    url(), SIGNAL(connectionStateChanged(int,QString)) );
    }


    // ### if autoDelete is true, we should delete the Q3UrlOperator (something
    // like below; but that is not possible since it would delete this, too).
    //if ( d->autoDelete && (d->url!=u) ) {
    //    delete d->url; // destructor deletes the network protocol
    //}
    d->url = u;

    if ( url() ) {
	connect( this, SIGNAL(data(QByteArray,Q3NetworkOperation*)),
		 url(), SIGNAL(data(QByteArray,Q3NetworkOperation*)) );
	connect( this, SIGNAL(finished(Q3NetworkOperation*)),
		 url(), SIGNAL(finished(Q3NetworkOperation*)) );
	connect( this, SIGNAL(start(Q3NetworkOperation*)),
		 url(), SIGNAL(start(Q3NetworkOperation*)) );
	connect( this, SIGNAL(newChildren(Q3ValueList<QUrlInfo>,Q3NetworkOperation*)),
		 url(), SIGNAL(newChildren(Q3ValueList<QUrlInfo>,Q3NetworkOperation*)) );
	connect( this, SIGNAL(newChildren(Q3ValueList<QUrlInfo>,Q3NetworkOperation*)),
		 url(), SLOT(addEntry(Q3ValueList<QUrlInfo>)) );
	connect( this, SIGNAL(createdDirectory(QUrlInfo,Q3NetworkOperation*)),
		 url(), SIGNAL(createdDirectory(QUrlInfo,Q3NetworkOperation*)) );
	connect( this, SIGNAL(removed(Q3NetworkOperation*)),
		 url(), SIGNAL(removed(Q3NetworkOperation*)) );
	connect( this, SIGNAL(itemChanged(Q3NetworkOperation*)),
		 url(), SIGNAL(itemChanged(Q3NetworkOperation*)) );
	connect( this, SIGNAL(dataTransferProgress(int,int,Q3NetworkOperation*)),
		 url(), SIGNAL(dataTransferProgress(int,int,Q3NetworkOperation*)) );
	connect( this, SIGNAL(connectionStateChanged(int,QString)),
		 url(), SIGNAL(connectionStateChanged(int,QString)) );
    }

    if ( !d->opInProgress && !d->operationQueue.isEmpty() )
	d->opStartTimer->start( 0, true );
}

/*!
    For processing operations the network protocol base class calls
    this method quite often. This should be reimplemented by new
    network protocols. It should return true if the connection is OK
    (open); otherwise it should return false. If the connection is not
    open the protocol should open it.

    If the connection can't be opened (e.g. because you already tried
    but the host couldn't be found), set the state of \a op to
    Q3NetworkProtocol::StFailed and emit the finished() signal with
    this Q3NetworkOperation as argument.

    \a op is the operation that needs an open connection.
*/

bool Q3NetworkProtocol::checkConnection( Q3NetworkOperation * )
{
    return true;
}

/*!
    Returns an int that is OR'd together using the enum values of
    \l{Q3NetworkProtocol::Operation}, which describes which operations
    are supported by the network protocol. Should be reimplemented by
    new network protocols.
*/

int Q3NetworkProtocol::supportedOperations() const
{
    return 0;
}

/*!
    Adds the operation \a op to the operation queue. The operation
    will be processed as soon as possible. This method returns
    immediately.
*/

void Q3NetworkProtocol::addOperation( Q3NetworkOperation *op )
{
#ifdef Q3NETWORKPROTOCOL_DEBUG
    qDebug( "Q3NetworkOperation: addOperation: %p %d", op, op->operation() );
#endif
    d->operationQueue.enqueue( op );
    if ( !d->opInProgress )
	d->opStartTimer->start( 0, true );
}

/*!
    Static method to register a network protocol for Qt. For example,
    if you have an implementation of NNTP (called Nntp) which is
    derived from Q3NetworkProtocol, call:
    \snippet doc/src/snippets/code/src_qt3support_network_q3networkprotocol.cpp 0
    after which your implementation is registered for future nntp
    operations.

    The name of the protocol is given in \a protocol and a pointer to
    the protocol factory is given in \a protocolFactory.
*/

void Q3NetworkProtocol::registerNetworkProtocol( const QString &protocol,
						Q3NetworkProtocolFactoryBase *protocolFactory )
{
    if ( !q3networkProtocolRegister ) {
	q3networkProtocolRegister = new Q3NetworkProtocolDict;
	Q3NetworkProtocol::registerNetworkProtocol( QLatin1String("file"), new Q3NetworkProtocolFactory< Q3LocalFs > );
    }

    q3networkProtocolRegister->insert( protocol, protocolFactory );
}

/*!
    Static method to get a new instance of the network protocol \a
    protocol. For example, if you need to do some FTP operations, do
    the following:
    \snippet doc/src/snippets/code/src_qt3support_network_q3networkprotocol.cpp 1
    This returns a pointer to a new instance of an ftp implementation
    or null if no protocol for ftp was registered. The ownership of
    the pointer is transferred to you, so you must delete it if you
    don't need it anymore.

    Normally you should not work directly with network protocols, so
    you will not need to call this method yourself. Instead, use
    Q3UrlOperator, which makes working with network protocols much more
    convenient.

    \sa Q3UrlOperator
*/

Q3NetworkProtocol *Q3NetworkProtocol::getNetworkProtocol( const QString &protocol )
{
    if ( !q3networkProtocolRegister ) {
	q3networkProtocolRegister = new Q3NetworkProtocolDict;
	Q3NetworkProtocol::registerNetworkProtocol( QLatin1String("file"), new Q3NetworkProtocolFactory< Q3LocalFs > );
    }

    if ( protocol.isNull() )
	return 0;

    Q3NetworkProtocolFactoryBase *factory = q3networkProtocolRegister->find( protocol );
    if ( factory )
	return factory->createObject();

    return 0;
}

/*!
    Returns true if the only protocol registered is for working on the
    local filesystem; returns false if other network protocols are
    also registered.
*/

bool Q3NetworkProtocol::hasOnlyLocalFileSystem()
{
    if ( !q3networkProtocolRegister )
	return false;

    Q3DictIterator< Q3NetworkProtocolFactoryBase > it( *q3networkProtocolRegister );
    for ( ; it.current(); ++it )
	if ( it.currentKey() != QLatin1String("file") )
	    return false;
    return true;
}

/*!
  \internal
  Starts processing network operations.
*/

void Q3NetworkProtocol::startOps()
{
#ifdef Q3NETWORKPROTOCOL_DEBUG
    qDebug( "Q3NetworkOperation: start processing operations" );
#endif
    processNextOperation( 0 );
}

/*!
  \internal
  Processes the operation \a op. It calls the
  corresponding operation[something]( Q3NetworkOperation * )
  methods.
*/

void Q3NetworkProtocol::processOperation( Q3NetworkOperation *op )
{
    if ( !op )
	return;

    switch ( op->operation() ) {
    case OpListChildren:
	operationListChildren( op );
	break;
    case OpMkDir:
	operationMkDir( op );
	break;
    case OpRemove:
	operationRemove( op );
	break;
    case OpRename:
	operationRename( op );
	break;
    case OpGet:
	operationGet( op );
	break;
    case OpPut:
	operationPut( op );
	break;
    }
}

/*!
    When implementing a new network protocol, this method should be
    reimplemented if the protocol supports listing children (files);
    this method should then process this Q3NetworkOperation.

    \a op is the pointer to the operation object which contains all
    the information on the operation that has finished, including the
    state, etc.
*/

void Q3NetworkProtocol::operationListChildren( Q3NetworkOperation * )
{
}

/*!
    When implementing a new network protocol, this method should be
    reimplemented if the protocol supports making directories; this
    method should then process this Q3NetworkOperation.

    \a op is the pointer to the operation object which contains all
    the information on the operation that has finished, including the
    state, etc.
*/

void Q3NetworkProtocol::operationMkDir( Q3NetworkOperation * )
{
}

/*!
    When implementing a new network protocol, this method should be
    reimplemented if the protocol supports removing children (files);
    this method should then process this Q3NetworkOperation.

    \a op is the pointer to the operation object which contains all
    the information on the operation that has finished, including the
    state, etc.
*/

void Q3NetworkProtocol::operationRemove( Q3NetworkOperation * )
{
}

/*!
    When implementing a new network protocol, this method should be
    reimplemented if the protocol supports renaming children (files);
    this method should then process this Q3NetworkOperation.

    \a op is the pointer to the operation object which contains all
    the information on the operation that has finished, including the
    state, etc.
*/

void Q3NetworkProtocol::operationRename( Q3NetworkOperation * )
{
}

/*!
    When implementing a new network protocol, this method should be
    reimplemented if the protocol supports getting data; this method
    should then process the Q3NetworkOperation.

    \a op is the pointer to the operation object which contains all
    the information on the operation that has finished, including the
    state, etc.
*/

void Q3NetworkProtocol::operationGet( Q3NetworkOperation * )
{
}

/*!
    When implementing a new network protocol, this method should be
    reimplemented if the protocol supports putting (uploading) data;
    this method should then process the Q3NetworkOperation.

    \a op is the pointer to the operation object which contains all
    the information on the operation that has finished, including the
    state, etc.
*/

void Q3NetworkProtocol::operationPut( Q3NetworkOperation * )
{
}

/*! \internal
*/

void Q3NetworkProtocol::operationPutChunk( Q3NetworkOperation * )
{
}

/*!
  \internal
  Handles operations. Deletes the previous operation object and
  tries to process the next operation. It also checks the connection state
  and only processes the next operation, if the connection of the protocol
  is open. Otherwise it waits until the protocol opens the connection.
*/

void Q3NetworkProtocol::processNextOperation( Q3NetworkOperation *old )
{
#ifdef Q3NETWORKPROTOCOL_DEBUG
    qDebug( "Q3NetworkOperation: process next operation, old: %p", old );
#endif
    d->removeTimer->stop();

    if ( old )
	d->oldOps.append( old );
    if ( d->opInProgress && d->opInProgress!=old )
	d->oldOps.append( d->opInProgress );

    if ( d->operationQueue.isEmpty() ) {
	d->opInProgress = 0;
	if ( d->autoDelete )
	    d->removeTimer->start( d->removeInterval, true );
	return;
    }

    Q3NetworkOperation *op = d->operationQueue.head();

    d->opInProgress = op;

    if ( !checkConnection( op ) ) {
	if ( op->state() != Q3NetworkProtocol::StFailed ) {
	    d->opStartTimer->start( 0, true );
	} else {
	    d->operationQueue.dequeue();
	    clearOperationQueue();
	    emit finished( op );
	}

	return;
    }

    d->opInProgress = op;
    d->operationQueue.dequeue();
    processOperation( op );
}

/*!
    Returns the Q3UrlOperator on which the protocol works.
*/

Q3UrlOperator *Q3NetworkProtocol::url() const
{
    return d->url;
}

/*!
    Returns the operation, which is being processed, or 0 of no
    operation is being processed at the moment.
*/

Q3NetworkOperation *Q3NetworkProtocol::operationInProgress() const
{
    return d->opInProgress;
}

/*!
    Clears the operation queue.
*/

void Q3NetworkProtocol::clearOperationQueue()
{
    d->operationQueue.dequeue();
    d->operationQueue.setAutoDelete( true );
    d->operationQueue.clear();
}

/*!
    Stops the current operation that is being processed and clears all
    waiting operations.
*/

void Q3NetworkProtocol::stop()
{
    Q3NetworkOperation *op = d->opInProgress;
    clearOperationQueue();
    if ( op ) {
	op->setState( StStopped );
	op->setProtocolDetail( tr( "Operation stopped by the user" ) );
	emit finished( op );
	setUrl( 0 );
	op->free();
    }
}

/*!
    Because it's sometimes hard to take care of removing network
    protocol instances, Q3NetworkProtocol provides an auto-delete
    mechanism. If you set \a b to true, the network protocol instance
    is removed after it has been inactive for \a i milliseconds (i.e.
    \a i milliseconds after the last operation has been processed).
    If you set \a b to false the auto-delete mechanism is switched
    off.

    If you switch on auto-delete, the Q3NetworkProtocol also deletes
    its Q3UrlOperator.
*/

void Q3NetworkProtocol::setAutoDelete( bool b, int i )
{
    d->autoDelete = b;
    d->removeInterval = i;
}

/*!
    Returns true if auto-deleting is enabled; otherwise returns false.

    \sa Q3NetworkProtocol::setAutoDelete()
*/

bool Q3NetworkProtocol::autoDelete() const
{
    return d->autoDelete;
}

/*!
  \internal
*/

void Q3NetworkProtocol::removeMe()
{
    if ( d->autoDelete ) {
#ifdef Q3NETWORKPROTOCOL_DEBUG
	qDebug( "Q3NetworkOperation:  autodelete of Q3NetworkProtocol %p", this );
#endif
	delete d->url; // destructor deletes the network protocol
    }
}

void Q3NetworkProtocol::emitNewChildren( const QUrlInfo &i, Q3NetworkOperation *op )
{
    Q3ValueList<QUrlInfo> lst;
    lst << i;
    emit newChildren( lst, op );
}

class Q3NetworkOperationPrivate
{
public:
    Q3NetworkProtocol::Operation operation;
    Q3NetworkProtocol::State state;
    QMap<int, QString> args;
    QMap<int, QByteArray> rawArgs;
    QString protocolDetail;
    int errorCode;
    QTimer *deleteTimer;
};

/*!
    \class Q3NetworkOperation

    \brief The Q3NetworkOperation class provides common operations for network protocols.

    \compat

    An object is created to describe the operation and the current
    state for each operation that a network protocol should process.

    \sa Q3NetworkProtocol
*/

/*!
    Constructs a network operation object. \a operation is the type of
    the operation, and \a arg0, \a arg1 and \a arg2 are the first
    three arguments of the operation. The state is initialized to
    Q3NetworkProtocol::StWaiting.

    \sa Q3NetworkProtocol::Operation Q3NetworkProtocol::State
*/

Q3NetworkOperation::Q3NetworkOperation( Q3NetworkProtocol::Operation operation,
				      const QString &arg0, const QString &arg1,
				      const QString &arg2 )
{
    d = new Q3NetworkOperationPrivate;
    d->deleteTimer = new QTimer( this );
    connect( d->deleteTimer, SIGNAL(timeout()),
	     this, SLOT(deleteMe()) );
    d->operation = operation;
    d->state = Q3NetworkProtocol::StWaiting;
    d->args[ 0 ] = arg0;
    d->args[ 1 ] = arg1;
    d->args[ 2 ] = arg2;
    d->rawArgs[ 0 ] = QByteArray( 0 );
    d->rawArgs[ 1 ] = QByteArray( 0 );
    d->rawArgs[ 2 ] = QByteArray( 0 );
    d->protocolDetail.clear();
    d->errorCode = (int)Q3NetworkProtocol::NoError;
}

/*!
    Constructs a network operation object. \a operation is the type of
    the operation, and \a arg0, \a arg1 and \a arg2 are the first
    three raw data arguments of the operation. The state is
    initialized to Q3NetworkProtocol::StWaiting.

    \sa Q3NetworkProtocol::Operation Q3NetworkProtocol::State
*/

Q3NetworkOperation::Q3NetworkOperation( Q3NetworkProtocol::Operation operation,
				      const QByteArray &arg0, const QByteArray &arg1,
				      const QByteArray &arg2 )
{
    d = new Q3NetworkOperationPrivate;
    d->deleteTimer = new QTimer( this );
    connect( d->deleteTimer, SIGNAL(timeout()),
	     this, SLOT(deleteMe()) );
    d->operation = operation;
    d->state = Q3NetworkProtocol::StWaiting;
    d->args[ 0 ].clear();
    d->args[ 1 ].clear();
    d->args[ 2 ].clear();
    d->rawArgs[ 0 ] = arg0;
    d->rawArgs[ 1 ] = arg1;
    d->rawArgs[ 2 ] = arg2;
    d->protocolDetail.clear();
    d->errorCode = (int)Q3NetworkProtocol::NoError;
}

/*!
    Destructor.
*/

Q3NetworkOperation::~Q3NetworkOperation()
{
    qDeleteInEventHandler(d->deleteTimer);
    delete d;
}

/*!
    Sets the \a state of the operation object. This should be done by
    the network protocol during processing; at the end it should be
    set to Q3NetworkProtocol::StDone or Q3NetworkProtocol::StFailed,
    depending on success or failure.

    \sa Q3NetworkProtocol::State
*/

void Q3NetworkOperation::setState( Q3NetworkProtocol::State state )
{
    if ( d->deleteTimer->isActive() ) {
	d->deleteTimer->stop();
	d->deleteTimer->start( NETWORK_OP_DELAY );
    }
    d->state = state;
}

/*!
    If the operation failed, the error message can be specified as \a
    detail.
*/

void Q3NetworkOperation::setProtocolDetail( const QString &detail )
{
    if ( d->deleteTimer->isActive() ) {
	d->deleteTimer->stop();
	d->deleteTimer->start( NETWORK_OP_DELAY );
    }
    d->protocolDetail = detail;
}

/*!
    Sets the error code to \a ec.

    If the operation failed, the protocol should set an error code to
    describe the error in more detail. If possible, one of the error
    codes defined in Q3NetworkProtocol should be used.

    \sa setProtocolDetail() Q3NetworkProtocol::Error
*/

void Q3NetworkOperation::setErrorCode( int ec )
{
    if ( d->deleteTimer->isActive() ) {
	d->deleteTimer->stop();
	d->deleteTimer->start( NETWORK_OP_DELAY );
    }
    d->errorCode = ec;
}

/*!
    Sets the network operation's \a{num}-th argument to \a arg.
*/

void Q3NetworkOperation::setArg( int num, const QString &arg )
{
    if ( d->deleteTimer->isActive() ) {
	d->deleteTimer->stop();
	d->deleteTimer->start( NETWORK_OP_DELAY );
    }
    d->args[ num ] = arg;
}

/*!
    Sets the network operation's \a{num}-th raw data argument to \a arg.
*/

void Q3NetworkOperation::setRawArg( int num, const QByteArray &arg )
{
    if ( d->deleteTimer->isActive() ) {
	d->deleteTimer->stop();
	d->deleteTimer->start( NETWORK_OP_DELAY );
    }
    d->rawArgs[ num ] = arg;
}

/*!
    Returns the type of the operation.
*/

Q3NetworkProtocol::Operation Q3NetworkOperation::operation() const
{
    if ( d->deleteTimer->isActive() ) {
	d->deleteTimer->stop();
	d->deleteTimer->start( NETWORK_OP_DELAY );
    }
    return d->operation;
}

/*!
    Returns the state of the operation. You can determine whether an
    operation is still waiting to be processed, is being processed,
    has been processed successfully, or failed.
*/

Q3NetworkProtocol::State Q3NetworkOperation::state() const
{
    if ( d->deleteTimer->isActive() ) {
	d->deleteTimer->stop();
	d->deleteTimer->start( NETWORK_OP_DELAY );
    }
    return d->state;
}

/*!
    Returns the operation's \a{num}-th argument. If this argument was
    not already set, an empty string is returned.
*/

QString Q3NetworkOperation::arg( int num ) const
{
    if ( d->deleteTimer->isActive() ) {
	d->deleteTimer->stop();
	d->deleteTimer->start( NETWORK_OP_DELAY );
    }
    return d->args[ num ];
}

/*!
    Returns the operation's \a{num}-th raw data argument. If this
    argument was not already set, an empty bytearray is returned.
*/

QByteArray Q3NetworkOperation::rawArg( int num ) const
{
    if ( d->deleteTimer->isActive() ) {
	d->deleteTimer->stop();
	d->deleteTimer->start( NETWORK_OP_DELAY );
    }
    return d->rawArgs[ num ];
}

/*!
    Returns a detailed error message for the last error. This must
    have been set using setProtocolDetail().
*/

QString Q3NetworkOperation::protocolDetail() const
{
    if ( d->deleteTimer->isActive() ) {
	d->deleteTimer->stop();
	d->deleteTimer->start( NETWORK_OP_DELAY );
    }
    return d->protocolDetail;
}

/*!
    Returns the error code for the last error that occurred.
*/

int Q3NetworkOperation::errorCode() const
{
    if ( d->deleteTimer->isActive() ) {
	d->deleteTimer->stop();
	d->deleteTimer->start( NETWORK_OP_DELAY );
    }
    return d->errorCode;
}

/*!
  \internal
*/

QByteArray& Q3NetworkOperation::raw( int num ) const
{
    if ( d->deleteTimer->isActive() ) {
	d->deleteTimer->stop();
	d->deleteTimer->start( NETWORK_OP_DELAY );
    }
    return d->rawArgs[ num ];
}

/*!
    Sets this object to delete itself when it hasn't been used for one
    second.

    Because Q3NetworkOperation pointers are passed around a lot the
    Q3NetworkProtocol generally does not have enough knowledge to
    delete these at the correct time. If a Q3NetworkProtocol doesn't
    need an operation any more it will call this function instead.

    Note: you should never need to call the method yourself.
*/

void Q3NetworkOperation::free()
{
    d->deleteTimer->start( NETWORK_OP_DELAY );
}

/*!
  \internal
  Internal slot for auto-deletion.
*/

void Q3NetworkOperation::deleteMe()
{
    delete this;
}

QT_END_NAMESPACE

#include "moc_q3networkprotocol.cpp"

#endif
