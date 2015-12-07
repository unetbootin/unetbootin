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

#include "q3urloperator.h"

#ifndef QT_NO_NETWORKPROTOCOL

#include "qurlinfo.h"
#include "q3networkprotocol.h"
#include "qmap.h"
#include "qdir.h"
#include "q3ptrdict.h"
#include "qpointer.h"
#include "q3valuelist.h"

#include "qapplication.h"

QT_BEGIN_NAMESPACE

//#define Q3URLOPERATOR_DEBUG

class Q3UrlOperatorPrivate
{
public:
    Q3UrlOperatorPrivate()
    {
	oldOps.setAutoDelete( false );
	networkProtocol = 0;
	nameFilter = QLatin1String("*");
	currPut = 0;
    }

    ~Q3UrlOperatorPrivate()
    {
	delete networkProtocol;
	while ( oldOps.first() ) {
	    oldOps.first()->free();
	    oldOps.removeFirst();
	}
    }

    QMap<QString, QUrlInfo> entryMap;
    Q3NetworkProtocol *networkProtocol;
    QString nameFilter;
    QDir dir;

    // maps needed for copy/move operations
    Q3PtrDict<Q3NetworkOperation> getOpPutOpMap;
    Q3PtrDict<Q3NetworkProtocol> getOpPutProtMap;
    Q3PtrDict<Q3NetworkProtocol> getOpGetProtMap;
    Q3PtrDict<Q3NetworkOperation> getOpRemoveOpMap;
    QPointer<Q3NetworkProtocol> currPut;
    QStringList waitingCopies;
    QString waitingCopiesDest;
    bool waitingCopiesMove;
    Q3PtrList< Q3NetworkOperation > oldOps;
};

/*!
    \class Q3UrlOperator q3urloperator.h

    \brief The Q3UrlOperator class provides common operations on URLs.

    \compat

    \module network

    This class operates on hierarchical structures (such as
    filesystems) using URLs. Its API facilitates all the common
    operations:
    \table
    \header \i Operation	\i Function
    \row \i List files		\i \l listChildren()
    \row \i Make a directory	\i \l mkdir()
    \row \i Remove a file	\i \l remove()
    \row \i Rename a file	\i \l rename()
    \row \i Get a file		\i \l get()
    \row \i Put a file		\i \l put()
    \row \i Copy a file		\i \l copy()
    \endtable

    You can obtain additional information about the URL with isDir()
    and info(). If a directory is to be traversed using
    listChildren(), a name filter can be set with setNameFilter().

    A Q3UrlOperator can be used like this, for example to download a
    file (and assuming that the FTP protocol is registered):
    \snippet doc/src/snippets/code/src_qt3support_network_q3urloperator.cpp 0

    If you want to be notified about success/failure, progress, etc.,
    you can connect to Q3UrlOperator's signals, e.g. to start(),
    newChildren(), createdDirectory(), removed(), data(),
    dataTransferProgress(), startedNextCopy(),
    connectionStateChanged(), finished(), etc. A network operation can
    be stopped with stop().

    The class uses the functionality of registered network protocols
    to perform these operations. Depending of the protocol of the URL,
    it uses an appropriate network protocol class for the operations.
    Each of the operation functions of Q3UrlOperator creates a
    Q3NetworkOperation object that describes the operation and puts it
    into the operation queue for the network protocol used. If no
    suitable protocol could be found (because no implementation of the
    necessary network protocol is registered), the URL operator emits
    errors. Not every protocol supports every operation, but error
    handling deals with this problem.

    To register the available network protocols, use the
    qInitNetworkProtocols() function. The protocols currently
    supported are:
    \list
    \i \link Q3Ftp FTP\endlink,
    \i \link Q3Http HTTP\endlink,
    \i \link Q3LocalFs local file system\endlink.
    \endlist

    \sa Q3NetworkProtocol, Q3NetworkOperation
*/

/*!
    \fn void Q3UrlOperator::newChildren( const Q3ValueList<QUrlInfo> &i, Q3NetworkOperation *op )

    This signal is emitted after listChildren() was called and new
    children (i.e. files) have been read from a list of files. \a i
    holds the information about the new files. \a op is a pointer
    to the operation object which contains all the information about
    the operation, including the state.

    \sa Q3NetworkOperation, Q3NetworkProtocol
*/


/*!
    \fn void Q3UrlOperator::finished( Q3NetworkOperation *op )

    This signal is emitted when an operation of some sort finishes,
    whether with success or failure. \a op is a pointer to the
    operation object, which contains all the information, including
    the state, of the operation which has been finished. Check the
    state and error code of the operation object to see whether or not
    the operation was successful.

    \sa Q3NetworkOperation, Q3NetworkProtocol
*/

/*!
    \fn void Q3UrlOperator::start( Q3NetworkOperation *op )

    Some operations (such as listChildren()) emit this signal when
    they start processing the operation. \a op is a pointer to the
    operation object which contains all the information about the
    operation, including the state.

    \sa Q3NetworkOperation, Q3NetworkProtocol
*/

/*!
    \fn void Q3UrlOperator::createdDirectory( const QUrlInfo &i, Q3NetworkOperation *op )

    This signal is emitted when mkdir() succeeds and the directory has
    been created. \a i holds the information about the new directory.

    \a op is a pointer to the operation object, which contains all the
    information about the operation, including the state.
    \c op->arg(0) holds the new directory's name.

    \sa Q3NetworkOperation, Q3NetworkProtocol
*/

/*!
    \fn void Q3UrlOperator::removed( Q3NetworkOperation *op )

    This signal is emitted when remove() has been successful and the
    file has been removed.

    \a op is a pointer to the operation object which contains all the
    information about the operation, including the state.
    \c op->arg(0) holds the name of the file that was removed.

    \sa Q3NetworkOperation, Q3NetworkProtocol
*/

/*!
    \fn void Q3UrlOperator::itemChanged( Q3NetworkOperation *op )

    This signal is emitted whenever a file which is a child of the URL
    has been changed, for example by successfully calling rename().
    \a op is a pointer to the operation object which contains all the
    information about the operation, including the state.
    \c op->arg(0) holds the original file name and \c op->arg(1) holds
    the new file name (if it was changed).

    \sa Q3NetworkOperation, Q3NetworkProtocol
*/

/*!
    \fn void Q3UrlOperator::data( const QByteArray &data, Q3NetworkOperation *op )

    This signal is emitted when new \a data has been received after calling
    get() or put().
    \a op is a pointer to the operation object which contains all
    the information about the operation, including the state.
    \c op->arg(0) holds the name of the file whose data is retrieved
    and op->rawArg(1) holds the (raw) data.

    \sa Q3NetworkOperation, Q3NetworkProtocol
*/

/*!
    \fn void Q3UrlOperator::dataTransferProgress( int bytesDone, int bytesTotal, Q3NetworkOperation *op )

    This signal is emitted during data transfer (using put() or
    get()). \a bytesDone specifies how many bytes of \a bytesTotal have
    been transferred. More information about the operation is stored in
    \a op, a pointer to the network operation that is processed.
    \a bytesTotal may be -1, which means that the total number of bytes
    is not known.

    \sa Q3NetworkOperation, Q3NetworkProtocol
*/

/*!
    \fn void Q3UrlOperator::startedNextCopy( const Q3PtrList<Q3NetworkOperation> &lst )

    This signal is emitted if copy() starts a new copy operation. \a
    lst contains all Q3NetworkOperations related to this copy
    operation.

    \sa copy()
*/

/*!
    \fn void Q3UrlOperator::connectionStateChanged( int state, const QString &data )

    This signal is emitted whenever the URL operator's connection
    state changes. \a state describes the new state, which is a
    \l{Q3NetworkProtocol::ConnectionState} value.

    \a data is a string that describes the change of the connection.
    This can be used to display a message to the user.
*/

/*!
    Constructs a Q3UrlOperator with an empty (i.e. invalid) URL.
*/

Q3UrlOperator::Q3UrlOperator()
    : Q3Url()
{
#ifdef Q3URLOPERATOR_DEBUG
    qDebug( "Q3UrlOperator: cstr 1" );
#endif
    d = new Q3UrlOperatorPrivate;
}

/*!
    Constructs a Q3UrlOperator using \a url and parses this string.

    If you pass strings like "/home/qt" the "file" protocol is
    assumed.
*/

Q3UrlOperator::Q3UrlOperator( const QString &url )
    : Q3Url( url )
{
#ifdef Q3URLOPERATOR_DEBUG
    qDebug( "Q3UrlOperator: cstr 2" );
#endif
    d = new Q3UrlOperatorPrivate;
    getNetworkProtocol();
}

/*!
    Constructs a copy of \a url.
*/

Q3UrlOperator::Q3UrlOperator( const Q3UrlOperator& url )
    : QObject(), Q3Url( url )
{
#ifdef Q3URLOPERATOR_DEBUG
    qDebug( "Q3UrlOperator: cstr 3" );
#endif
    d = new Q3UrlOperatorPrivate;
    *d = *url.d;

    d->networkProtocol = 0;
    getNetworkProtocol();
    d->nameFilter = QLatin1String("*");
    d->currPut = 0;
}

/*!
    Constructs a Q3UrlOperator. The URL on which this Q3UrlOperator
    operates is constructed out of the arguments \a url, \a relUrl and
    \a checkSlash: see the corresponding Q3Url constructor for an
    explanation of these arguments.
*/

Q3UrlOperator::Q3UrlOperator( const Q3UrlOperator& url, const QString& relUrl, bool checkSlash )
    : Q3Url( url, relUrl, checkSlash )
{
#ifdef Q3URLOPERATOR_DEBUG
    qDebug( "Q3UrlOperator: cstr 4" );
#endif
    d = new Q3UrlOperatorPrivate;
    if ( relUrl == QLatin1String(".") )
	*d = *url.d;

    d->networkProtocol = 0;
    getNetworkProtocol();
    d->currPut = 0;
}

/*!
    Destructor.
*/

Q3UrlOperator::~Q3UrlOperator()
{
#ifdef Q3URLOPERATOR_DEBUG
    qDebug( "Q3UrlOperator: dstr" );
#endif
    delete d;
}

/*!
    This private function is used by the simple operation functions,
    i.e. listChildren(), mkdir(), remove(), rename(), get() and put(),
    to really start the operation. \a op is a pointer to the network
    operation that should be started. Returns \a op on success;
    otherwise returns 0.
*/
const Q3NetworkOperation *Q3UrlOperator::startOperation( Q3NetworkOperation *op )
{
    if ( !d->networkProtocol )
        getNetworkProtocol();
    
    if ( d->networkProtocol && (d->networkProtocol->supportedOperations()&op->operation()) ) {
	d->networkProtocol->addOperation( op );
	if ( op->operation() == Q3NetworkProtocol::OpListChildren )
	    clearEntries();
        return op;
    }

    // error
    QString msg;
    if ( !d->networkProtocol ) {
	msg = tr( "The protocol `%1' is not supported" ).arg( protocol() );
    } else {
	switch ( op->operation() ) {
	case Q3NetworkProtocol::OpListChildren:
	    msg = tr( "The protocol `%1' does not support listing directories" ).arg( protocol() );
	    break;
	case Q3NetworkProtocol::OpMkDir:
	    msg = tr( "The protocol `%1' does not support creating new directories" ).arg( protocol() );
	    break;
	case Q3NetworkProtocol::OpRemove:
	    msg = tr( "The protocol `%1' does not support removing files or directories" ).arg( protocol() );
	    break;
	case Q3NetworkProtocol::OpRename:
	    msg = tr( "The protocol `%1' does not support renaming files or directories" ).arg( protocol() );
	    break;
	case Q3NetworkProtocol::OpGet:
	    msg = tr( "The protocol `%1' does not support getting files" ).arg( protocol() );
	    break;
	case Q3NetworkProtocol::OpPut:
	    msg = tr( "The protocol `%1' does not support putting files" ).arg( protocol() );
	    break;
	default:
	    // this should never happen
	    break;
	}
    }
    op->setState( Q3NetworkProtocol::StFailed );
    op->setProtocolDetail( msg );
    op->setErrorCode( (int)Q3NetworkProtocol::ErrUnsupported );
    emit finished( op );
    deleteOperation( op );
    return 0;
}

/*!
    Starts listing the children of this URL (e.g. the files in the
    directory). The start() signal is emitted before the first entry
    is listed and finished() is emitted after the last one. The
    newChildren() signal is emitted for each list of new entries. If
    an error occurs, the signal finished() is emitted, so be sure to
    check the state of the network operation pointer.

    Because the operation may not be executed immediately, a pointer
    to the Q3NetworkOperation object created by this function is
    returned. This object contains all the data about the operation
    and is used to refer to this operation later (e.g. in the signals
    that are emitted by the Q3UrlOperator). The return value can also
    be 0 if the operation object couldn't be created.

    The path of this Q3UrlOperator must to point to a directory
    (because the children of this directory will be listed), not to a
    file.
*/

const Q3NetworkOperation *Q3UrlOperator::listChildren()
{
    if ( !checkValid() )
	return 0;

    Q3NetworkOperation *res = new Q3NetworkOperation( Q3NetworkProtocol::OpListChildren, QString(), QString(), QString() );
    return startOperation( res );
}

/*!
    Tries to create a directory (child) with the name \a dirname. If
    it is successful, a newChildren() signal with the new child is
    emitted, and the createdDirectory() signal with the information
    about the new child is also emitted. The finished() signal (with
    success or failure) is emitted after the operation has been
    processed, so check the state of the network operation object to
    see whether or not the operation was successful.

    Because the operation will not be executed immediately, a pointer
    to the Q3NetworkOperation object created by this function is
    returned. This object contains all the data about the operation
    and is used to refer to this operation later (e.g. in the signals
    that are emitted by the Q3UrlOperator). The return value can also
    be 0 if the operation object couldn't be created.

    The path of this Q3UrlOperator must to point to a directory (not a
    file) because the new directory will be created in this path.
*/

const Q3NetworkOperation *Q3UrlOperator::mkdir( const QString &dirname )
{
    if ( !checkValid() )
	return 0;

    Q3NetworkOperation *res = new Q3NetworkOperation( Q3NetworkProtocol::OpMkDir, dirname, QString(), QString() );
    return startOperation( res );
}

/*!
    Tries to remove the file (child) \a filename. If it succeeds the
    removed() signal is emitted. finished() (with success or failure)
    is also emitted after the operation has been processed, so check
    the state of the network operation object to see whether or not
    the operation was successful.

    Because the operation will not be executed immediately, a pointer
    to the Q3NetworkOperation object created by this function is
    returned. This object contains all the data about the operation
    and is used to refer to this operation later (e.g. in the signals
    that are emitted by the Q3UrlOperator). The return value can also
    be 0 if the operation object couldn't be created.

    The path of this Q3UrlOperator must point to a directory; because
    if \a filename is relative, it will try to remove it in this
    directory.
*/

const Q3NetworkOperation *Q3UrlOperator::remove( const QString &filename )
{
    if ( !checkValid() )
	return 0;

    Q3NetworkOperation *res = new Q3NetworkOperation( Q3NetworkProtocol::OpRemove, filename, QString(), QString() );
    return startOperation( res );
}

/*!
    Tries to rename the file (child) called \a oldname to \a newname.
    If it succeeds, the itemChanged() signal is emitted. finished()
    (with success or failure) is also emitted after the operation has
    been processed, so check the state of the network operation object
    to see whether or not the operation was successful.

    Because the operation may not be executed immediately, a pointer
    to the Q3NetworkOperation object created by this function is
    returned. This object contains all the data about the operation
    and is used to refer to this operation later (e.g. in the signals
    that are emitted by the Q3UrlOperator). The return value can also
    be 0 if the operation object couldn't be created.

    This path of this Q3UrlOperator must to point to a directory
    because \a oldname and \a newname are handled relative to this
    directory.
*/

const Q3NetworkOperation *Q3UrlOperator::rename( const QString &oldname, const QString &newname )
{
    if ( !checkValid() )
	return 0;

    Q3NetworkOperation *res = new Q3NetworkOperation( Q3NetworkProtocol::OpRename, oldname, newname, QString() );
    return startOperation( res );
}

/*!
    Copies the file \a from to \a to. If \a move is true, the file is
    moved (copied and removed). \a from must point to a file and \a to
    must point to a directory (into which \a from is copied) unless \a
    toPath is set to false. If \a toPath is set to false then the \a
    to variable is assumed to be the absolute file path (destination
    file path + file name). The copying is done using the get() and
    put() operations. If you want to be notified about the progress of
    the operation, connect to the dataTransferProgress() signal. Bear
    in mind that the get() and put() operations emit this signal
    through the Q3UrlOperator. The number of transferred bytes and the
    total bytes that you receive as arguments in this signal do not
    relate to the the whole copy operation; they relate first to the
    get() and then to the put() operation. Always check what type of
    operation the signal comes from; this is given in the signal's
    last argument.

    At the end, finished() (with success or failure) is emitted, so
    check the state of the network operation object to see whether or
    not the operation was successful.

    Because a move or copy operation consists of multiple operations
    (get(), put() and maybe remove()), this function doesn't return a
    single Q3NetworkOperation, but rather a list of them. They are in
    the order: get(), put() and (if applicable) remove().

    \sa get(), put()
*/

Q3PtrList<Q3NetworkOperation> Q3UrlOperator::copy( const QString &from, const QString &to, bool move, bool toPath )
{
#ifdef Q3URLOPERATOR_DEBUG
    qDebug( "Q3UrlOperator: copy %s %s %d", from.latin1(), to.latin1(), move );
#endif

    Q3PtrList<Q3NetworkOperation> ops;
    ops.setAutoDelete( false );

    Q3UrlOperator *uFrom = new Q3UrlOperator( *this, from );
    Q3UrlOperator *uTo = new Q3UrlOperator( to );

    // prepare some string for later usage
    QString frm = *uFrom;
    QString file = uFrom->fileName();

    if (frm == to + file)
         return ops;
    
    file.prepend( QLatin1String("/") );

    // uFrom and uTo are deleted when the Q3NetworkProtocol deletes itself via
    // autodelete
    uFrom->getNetworkProtocol();
    uTo->getNetworkProtocol();
    Q3NetworkProtocol *gProt = uFrom->d->networkProtocol;
    Q3NetworkProtocol *pProt = uTo->d->networkProtocol;

    uFrom->setPath( uFrom->dirPath() );

    if ( gProt && (gProt->supportedOperations()&Q3NetworkProtocol::OpGet) &&
	 pProt && (pProt->supportedOperations()&Q3NetworkProtocol::OpPut) ) {

	connect( gProt, SIGNAL(data(QByteArray,Q3NetworkOperation*)),
		 this, SLOT(copyGotData(QByteArray,Q3NetworkOperation*)) );
	connect( gProt, SIGNAL(dataTransferProgress(int,int,Q3NetworkOperation*)),
		 this, SIGNAL(dataTransferProgress(int,int,Q3NetworkOperation*)) );
	connect( gProt, SIGNAL(finished(Q3NetworkOperation*)),
		 this, SLOT(continueCopy(Q3NetworkOperation*)) );
	connect( gProt, SIGNAL(finished(Q3NetworkOperation*)),
		 this, SIGNAL(finished(Q3NetworkOperation*)) );
	connect( gProt, SIGNAL(connectionStateChanged(int,QString)),
		 this, SIGNAL(connectionStateChanged(int,QString)) );

	connect( pProt, SIGNAL(dataTransferProgress(int,int,Q3NetworkOperation*)),
		 this, SIGNAL(dataTransferProgress(int,int,Q3NetworkOperation*)) );
	connect( pProt, SIGNAL(finished(Q3NetworkOperation*)),
		 this, SIGNAL(finished(Q3NetworkOperation*)) );
	connect( pProt, SIGNAL(finished(Q3NetworkOperation*)),
		 this, SLOT(finishedCopy()) );

	Q3NetworkOperation *opGet = new Q3NetworkOperation( Q3NetworkProtocol::OpGet, frm, QString(), QString() );
	ops.append( opGet );
	gProt->addOperation( opGet );


	QString toFile = to + file;
	if (!toPath)
	    toFile = to;

	Q3NetworkOperation *opPut = new Q3NetworkOperation( Q3NetworkProtocol::OpPut, toFile, QString(), QString() );
	ops.append( opPut );

	d->getOpPutProtMap.insert( (void*)opGet, pProt );
	d->getOpGetProtMap.insert( (void*)opGet, gProt );
	d->getOpPutOpMap.insert( (void*)opGet, opPut );

	if ( move && (gProt->supportedOperations()&Q3NetworkProtocol::OpRemove) ) {
	    gProt->setAutoDelete( false );

	    Q3NetworkOperation *opRm = new Q3NetworkOperation( Q3NetworkProtocol::OpRemove, frm, QString(), QString() );
	    ops.append( opRm );
	    d->getOpRemoveOpMap.insert( (void*)opGet, opRm );
	} else {
	    gProt->setAutoDelete( true );
	}
#ifdef Q3URLOPERATOR_DEBUG
	qDebug( "Q3UrlOperator: copy operation should start now..." );
#endif
	return ops;
    } else {
	QString msg;
	if ( !gProt ) {
	    msg = tr( "The protocol `%1' is not supported" ).arg( uFrom->protocol() );
	} else if ( gProt->supportedOperations() & Q3NetworkProtocol::OpGet ) {
	    msg = tr( "The protocol `%1' does not support copying or moving files or directories" ).arg( uFrom->protocol() );
	} else if ( !pProt ) {
	    msg = tr( "The protocol `%1' is not supported" ).arg( uTo->protocol() );
	} else {
	    msg = tr( "The protocol `%1' does not support copying or moving files or directories" ).arg( uTo->protocol() );
	}
	delete uFrom;
	delete uTo;
	Q3NetworkOperation *res = new Q3NetworkOperation( Q3NetworkProtocol::OpGet, frm, to, QString() );
	res->setState( Q3NetworkProtocol::StFailed );
	res->setProtocolDetail( msg );
	res->setErrorCode( (int)Q3NetworkProtocol::ErrUnsupported );
	emit finished( res );
	deleteOperation( res );
    }

    return ops;
}

/*!
    \overload

    Copies the \a files to the directory \a dest. If \a move is true
    the files are moved, not copied. \a dest must point to a
    directory.

    This function calls copy() for each entry in \a files in turn. You
    don't get a result from this function; each time a new copy
    begins, startedNextCopy() is emitted, with a list of
    Q3NetworkOperations that describe the new copy operation.
*/

void Q3UrlOperator::copy( const QStringList &files, const QString &dest,
			 bool move )
{
    d->waitingCopies = files;
    d->waitingCopiesDest = dest;
    d->waitingCopiesMove = move;

    finishedCopy();
}

/*!
    Returns true if the URL is a directory; otherwise returns false.
    This may not always work correctly, if the protocol of the URL is
    something other than file (local filesystem). If you pass a bool
    pointer as the \a ok argument, *\a ok is set to true if the result
    of this function is known to be correct, and to false otherwise.
*/

bool Q3UrlOperator::isDir( bool *ok )
{
    if ( ok )
	*ok = true;
    if ( isLocalFile() ) {
	if ( QFileInfo( path() ).isDir() )
	    return true;
	else
	    return false;
    }

    if ( d->entryMap.contains( QLatin1String(".") ) ) {
	return d->entryMap[ QLatin1String(".") ].isDir();
    }
    // #### can assume that we are a directory?
    if ( ok )
	*ok = false;
    return true;
}

/*!
    Tells the network protocol to get data from \a location or, if
    it is empty, to get data from the location to which this
    URL points (see Q3Url::fileName() and Q3Url::encodedPathAndQuery()).
    What happens then depends on the network protocol. The data()
    signal is emitted when data comes in. Because it's unlikely that
    all data will come in at once, it is common for multiple data()
    signals to be emitted. The dataTransferProgress() signal is
    emitted while processing the operation. At the end, finished()
    (with success or failure) is emitted, so check the state of the
    network operation object to see whether or not the operation was
    successful.

    If \a location is empty, the path of this Q3UrlOperator
    should point to a file when you use this operation. If \a location
    is not empty, it can be a relative URL (a child of the path to
    which the Q3UrlOperator points) or an absolute URL.

    For example, to get a web page you might do something like this:

    \snippet doc/src/snippets/code/src_qt3support_network_q3urloperator.cpp 1

    For most other operations, the path of the Q3UrlOperator must point
    to a directory. If you want to download a file you could do the
    following:

    \snippet doc/src/snippets/code/src_qt3support_network_q3urloperator.cpp 2

    This will get the data of ftp://ftp.whatever.org/pub/a_file.txt.

    \e Never do anything like this:
    \snippet doc/src/snippets/code/src_qt3support_network_q3urloperator.cpp 3

    If \a location is not empty and relative it must not contain any
    queries or references, just the name of a child. So if you need to
    specify a query or reference, do it as shown in the first example
    or specify the full URL (such as
    http://www.whatever.org/cgi-bin/search.pl?cmd=Hello) as \a location.

    \sa copy()
*/

const Q3NetworkOperation *Q3UrlOperator::get( const QString &location )
{
    Q3Url u( *this );
    if ( !location.isEmpty() )
	u = Q3Url( *this, location );

    if ( !u.isValid() )
	return 0;

    if ( !d->networkProtocol ) {
	setProtocol( u.protocol() );
	getNetworkProtocol();
    }

    Q3NetworkOperation *res = new Q3NetworkOperation( Q3NetworkProtocol::OpGet, u, QString(), QString() );
    return startOperation( res );
}

/*!
    This function tells the network protocol to put \a data in \a
    location. If \a location is empty, it puts the \a data in the
    location to which the URL points. What happens depends on
    the network protocol. Depending on the network protocol, some
    data might come back after putting data, in which case the data()
    signal is emitted. The dataTransferProgress() signal is emitted
    during processing of the operation. At the end, finished() (with
    success or failure) is emitted, so check the state of the network
    operation object to see whether or not the operation was
    successful.

    If \a location is empty, the path of this Q3UrlOperator should
    point to a file when you use this operation. If \a location
    is not empty, it can be a relative (a child of the path to which
    the Q3UrlOperator points) or an absolute URL.

    For putting some data to a file you can do the following:

    \snippet doc/src/snippets/code/src_qt3support_network_q3urloperator.cpp 4

    For most other operations, the path of the Q3UrlOperator must point
    to a directory. If you want to upload data to a file you could do
    the following:

    \snippet doc/src/snippets/code/src_qt3support_network_q3urloperator.cpp 5

    This will upload the data to ftp://ftp.whatever.com/home/me/filename.dat.

    \sa copy()
*/

const Q3NetworkOperation *Q3UrlOperator::put( const QByteArray &data, const QString &location )
{
    Q3Url u( *this );
    if ( !location.isEmpty() )
	u = Q3Url( *this, location );

    if ( !u.isValid() )
	return 0;

    if ( !d->networkProtocol ) {
	setProtocol( u.protocol() );
	getNetworkProtocol();
    }

    Q3NetworkOperation *res = new Q3NetworkOperation( Q3NetworkProtocol::OpPut, u, QString(), QString() );
    res->setRawArg( 1, data );
    return startOperation( res );
}

/*!
    Sets the name filter of the URL to \a nameFilter.

    \sa QDir::setNameFilter()
*/

void Q3UrlOperator::setNameFilter( const QString &nameFilter )
{
    d->nameFilter = nameFilter;
}

/*!
    Returns the name filter of the URL.

    \sa Q3UrlOperator::setNameFilter() QDir::nameFilter()
*/

QString Q3UrlOperator::nameFilter() const
{
    return d->nameFilter;
}

/*!
    Clears the cache of children.
*/

void Q3UrlOperator::clearEntries()
{
    d->entryMap.clear();
}

/*!
    Adds an entry to the cache of children.
*/

void Q3UrlOperator::addEntry( const Q3ValueList<QUrlInfo> &i )
{
    Q3ValueList<QUrlInfo>::ConstIterator it = i.begin();
    for ( ; it != i.end(); ++it )
	d->entryMap[ ( *it ).name().stripWhiteSpace() ] = *it;
}

/*!
    Returns the URL information for the child \a entry, or returns an
    empty QUrlInfo object if there is no information available about
    \a entry. Information about \a entry is only available after a successfully
    finished listChildren() operation.
*/

QUrlInfo Q3UrlOperator::info( const QString &entry ) const
{
    if ( d->entryMap.contains( entry.stripWhiteSpace() ) ) {
	return d->entryMap[ entry.stripWhiteSpace() ];
    } else if ( entry == QLatin1String(".") || entry == QLatin1String("..") ) {
	 // return a faked QUrlInfo
	 QUrlInfo inf;
	 inf.setName( entry );
	 inf.setDir( true );
	 inf.setFile( false );
	 inf.setSymLink( false );
	 inf.setOwner( tr( "(unknown)" ) );
	 inf.setGroup( tr( "(unknown)" ) );
	 inf.setSize( 0 );
	 inf.setWritable( false );
	 inf.setReadable( true );
	 return inf;
    }
    return QUrlInfo();
}

/*!
    Finds a network protocol for the URL and deletes the old network protocol.
*/

void Q3UrlOperator::getNetworkProtocol()
{
    delete d->networkProtocol;
    Q3NetworkProtocol *p = Q3NetworkProtocol::getNetworkProtocol( protocol() );
    if ( !p ) {
	d->networkProtocol = 0;
	return;
    }

    d->networkProtocol = (Q3NetworkProtocol *)p;
    d->networkProtocol->setUrl( this );
    connect( d->networkProtocol, SIGNAL(itemChanged(Q3NetworkOperation*)),
	     this, SLOT(slotItemChanged(Q3NetworkOperation*)) );
}

/*!
    Deletes the currently used network protocol.
*/

void Q3UrlOperator::deleteNetworkProtocol()
{
    if (d->networkProtocol) {
        d->networkProtocol->deleteLater();
        d->networkProtocol = 0;
    }
}

/*!
    \reimp
*/

void Q3UrlOperator::setPath( const QString& path )
{
    Q3Url::setPath( path );
    if ( d->networkProtocol )
	d->networkProtocol->setUrl( this );
}

/*!
    \reimp
*/

void Q3UrlOperator::reset()
{
    Q3Url::reset();
    deleteNetworkProtocol();
    d->nameFilter = QLatin1String("*");
}

/*!
    \reimp
*/

bool Q3UrlOperator::parse( const QString &url )
{
    bool b = Q3Url::parse( url );
    if ( !b ) {
	return b;
    }

    getNetworkProtocol();

    return b;
}

/*!
    Assigns \a url to this object.
*/

Q3UrlOperator& Q3UrlOperator::operator=( const Q3UrlOperator &url )
{
    deleteNetworkProtocol();
    Q3Url::operator=( url );

    Q3PtrDict<Q3NetworkOperation> getOpPutOpMap = d->getOpPutOpMap;
    Q3PtrDict<Q3NetworkProtocol> getOpPutProtMap = d->getOpPutProtMap;
    Q3PtrDict<Q3NetworkProtocol> getOpGetProtMap = d->getOpGetProtMap;
    Q3PtrDict<Q3NetworkOperation> getOpRemoveOpMap = d->getOpRemoveOpMap;

    *d = *url.d;

    d->oldOps.setAutoDelete( false );
    d->getOpPutOpMap = getOpPutOpMap;
    d->getOpPutProtMap = getOpPutProtMap;
    d->getOpGetProtMap = getOpGetProtMap;
    d->getOpRemoveOpMap = getOpRemoveOpMap;

    d->networkProtocol = 0;
    getNetworkProtocol();
    return *this;
}

/*!
    Assigns \a url to this object.
*/

Q3UrlOperator& Q3UrlOperator::operator=( const QString &url )
{
    deleteNetworkProtocol();
    Q3Url::operator=( url );
    d->oldOps.setAutoDelete( false );
    getNetworkProtocol();
    return *this;
}

/*!
    \internal
*/

bool Q3UrlOperator::cdUp()
{
    bool b = Q3Url::cdUp();
    if ( d->networkProtocol )
	d->networkProtocol->setUrl( this );
    return b;
}

/*!
    \internal
*/

bool Q3UrlOperator::checkValid()
{
    // ######
    if ( !isValid() ) {
	//emit error( ErrValid, tr( "The entered URL is not valid!" ) );
	return false;
    } else
	return true;
}


/*!
    \internal
*/

void Q3UrlOperator::copyGotData( const QByteArray &data_, Q3NetworkOperation *op )
{
#ifdef Q3URLOPERATOR_DEBUG
    qDebug( "Q3UrlOperator: copyGotData: %d new bytes", data_.size() );
#endif
    Q3NetworkOperation *put = d->getOpPutOpMap[ (void*)op ];
    if ( put ) {
	QByteArray &s = put->raw( 1 );
	int size = s.size();
	s.resize( size + data_.size() );
	memcpy( s.data() + size, data_.data(), data_.size() );
    }
    emit data( data_, op );
}

/*!
    \internal
*/

void Q3UrlOperator::continueCopy( Q3NetworkOperation *op )
{
    if ( op->operation() != Q3NetworkProtocol::OpGet )
	return;
    if ( op->state()!=Q3NetworkProtocol::StDone &&  op->state()!=Q3NetworkProtocol::StFailed ) {
	return;
    }

#ifdef Q3URLOPERATOR_DEBUG
    if ( op->state() != Q3NetworkProtocol::StFailed ) {
	qDebug( "Q3UrlOperator: continue copy (get finished, put will start)" );
    }
#endif

    Q3NetworkOperation *put = d->getOpPutOpMap[ (void*)op ];
    Q3NetworkProtocol *gProt = d->getOpGetProtMap[ (void*)op ];
    Q3NetworkProtocol *pProt = d->getOpPutProtMap[ (void*)op ];
    Q3NetworkOperation *rm = d->getOpRemoveOpMap[ (void*)op ];
    d->getOpPutOpMap.take( op );
    d->getOpGetProtMap.take( op );
    d->getOpPutProtMap.take( op );
    d->getOpRemoveOpMap.take( op );
    if ( pProt )
	pProt->setAutoDelete( true );
    if ( put && pProt ) {
	if ( op->state() != Q3NetworkProtocol::StFailed ) {
	    pProt->addOperation( put );
	    d->currPut = pProt;
        if (rm) { // we need the result of the put operation
            qApp->processEvents(); // process posted operations
            if (put->state() == Q3NetworkProtocol::StFailed) {
                deleteOperation( rm );
                rm = 0;
            }
        }
	} else {
	    deleteOperation( put );
	}
    }
    if ( gProt ) {
	gProt->setAutoDelete( true );
    }
    if ( rm && gProt ) {
	if ( op->state() != Q3NetworkProtocol::StFailed ) {
	    gProt->addOperation( rm );
	} else {
	    deleteOperation( rm );
	}
    }
    disconnect( gProt, SIGNAL(data(QByteArray,Q3NetworkOperation*)),
		this, SLOT(copyGotData(QByteArray,Q3NetworkOperation*)) );
    disconnect( gProt, SIGNAL(finished(Q3NetworkOperation*)),
		this, SLOT(continueCopy(Q3NetworkOperation*)) );
}

/*!
    \internal
*/

void Q3UrlOperator::finishedCopy()
{
#ifdef Q3URLOPERATOR_DEBUG
    qDebug( "Q3UrlOperator: finished copy (finished putting)" );
#endif

    if ( d->waitingCopies.isEmpty() )
	return;

    QString cp = d->waitingCopies.first();
    d->waitingCopies.remove( cp );
    Q3PtrList<Q3NetworkOperation> lst = copy( cp, d->waitingCopiesDest, d->waitingCopiesMove );
    emit startedNextCopy( lst );
}

/*!
    Stops the current network operation and removes all this
    Q3UrlOperator's waiting network operations.
*/

void Q3UrlOperator::stop()
{
    d->getOpPutOpMap.clear();
    d->getOpRemoveOpMap.clear();
    d->getOpGetProtMap.setAutoDelete( true );
    d->getOpPutProtMap.setAutoDelete( true );
    Q3PtrDictIterator<Q3NetworkProtocol> it( d->getOpPutProtMap );
    for ( ; it.current(); ++it )
	it.current()->stop();
    d->getOpPutProtMap.clear();
    it = Q3PtrDictIterator<Q3NetworkProtocol>( d->getOpGetProtMap );
    for ( ; it.current(); ++it )
	it.current()->stop();
    d->getOpGetProtMap.clear();
    if ( d->currPut ) {
	d->currPut->stop();
	delete (Q3NetworkProtocol *) d->currPut;
	d->currPut = 0;
    }
    d->waitingCopies.clear();
    if ( d->networkProtocol )
	d->networkProtocol->stop();
    getNetworkProtocol();
}

/*!
    \internal
*/

void Q3UrlOperator::deleteOperation( Q3NetworkOperation *op )
{
    if ( op )
	d->oldOps.append( op );
}

/*!
    \internal
    updates the entryMap after a network operation finished
*/

void Q3UrlOperator::slotItemChanged( Q3NetworkOperation *op )
{
    if ( !op )
	return;

    switch ( op->operation() ) {
    case Q3NetworkProtocol::OpRename :
    {
	if ( op->arg( 0 ) == op->arg( 1 ) )
	    return;

	QMap<QString, QUrlInfo>::iterator mi = d->entryMap.find( op->arg( 0 ) );
	if ( mi != d->entryMap.end() ) {
	    mi.data().setName( op->arg( 1 ) );
	    d->entryMap[ op->arg( 1 ) ] = mi.data();
	    d->entryMap.erase( mi );
	}
	break;
    }
    case Q3NetworkProtocol::OpRemove :
    {
	QMap<QString, QUrlInfo>::iterator mi = d->entryMap.find( op->arg( 0 ) );
	if ( mi != d->entryMap.end() )
	    d->entryMap.erase( mi );
	break;
    }
    default:
	break;
    }
}

QT_END_NAMESPACE

#include "moc_q3urloperator.cpp"

#endif // QT_NO_NETWORKPROTOCOL
