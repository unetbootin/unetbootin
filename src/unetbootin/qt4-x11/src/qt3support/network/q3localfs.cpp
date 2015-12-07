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

#include "q3localfs.h"

#ifndef QT_NO_NETWORKPROTOCOL

#include "qfileinfo.h"
#include "qfile.h"
#include "q3url.h"
#include "qurlinfo.h"
#include "qapplication.h"
#include "q3urloperator.h"
#include "qpointer.h"
#include "q3valuelist.h"

QT_BEGIN_NAMESPACE

//#define QLOCALFS_DEBUG


/*!
    \class Q3LocalFs q3localfs.h
    \brief The Q3LocalFs class is an implementation of a
    QNetworkProtocol that works on the local file system.

    \compat

    This class is derived from QNetworkProtocol. Q3LocalFs is not
    normally used directly, but rather through a QUrlOperator, for
    example:
    \snippet doc/src/snippets/code/src_qt3support_network_q3localfs.cpp 0

    This code will only work if the Q3LocalFs class is registered; to
    register the class, you must call qInitNetworkProtocols() before
    using a QUrlOperator with Q3LocalFs.

    If you really need to use Q3LocalFs directly, don't forget
    to set its QUrlOperator with setUrl().

    \sa Q3NetworkProtocol, Q3UrlOperator
*/

/*!
    Constructor.
*/

Q3LocalFs::Q3LocalFs()
    : Q3NetworkProtocol()
{
}

static int convertPermissions(QFileInfo *fi)
{
    int p = 0;
    if ( fi->permission( QFileInfo::ReadOwner ) )
	p |= QUrlInfo::ReadOwner;
    if ( fi->permission( QFileInfo::WriteOwner ) )
	p |= QUrlInfo::WriteOwner;
    if ( fi->permission( QFileInfo::ExeOwner ) )
	p |= QUrlInfo::ExeOwner;
    if ( fi->permission( QFileInfo::ReadGroup ) )
	p |= QUrlInfo::ReadGroup;
    if ( fi->permission( QFileInfo::WriteGroup ) )
	p |= QUrlInfo::WriteGroup;
    if ( fi->permission( QFileInfo::ExeGroup ) )
	p |= QUrlInfo::ExeGroup;
    if ( fi->permission( QFileInfo::ReadOther ) )
	p |= QUrlInfo::ReadOther;
    if ( fi->permission( QFileInfo::WriteOther ) )
	p |= QUrlInfo::WriteOther;
    if ( fi->permission( QFileInfo::ExeOther ) )
	p |= QUrlInfo::ExeOther;
    return p;
}

/*!
    \reimp
*/

void Q3LocalFs::operationListChildren( Q3NetworkOperation *op )
{
#ifdef QLOCALFS_DEBUG
    qDebug( "Q3LocalFs: operationListChildren" );
#endif
    op->setState( StInProgress );

    dir = QDir( url()->path() );
    dir.setNameFilter( url()->nameFilter() );
    dir.setMatchAllDirs( true );
    if ( !dir.isReadable() ) {
	QString msg = tr( "Could not read directory\n%1" ).arg( url()->path() );
	op->setState( StFailed );
	op->setProtocolDetail( msg );
	op->setErrorCode( (int)ErrListChildren );
	emit finished( op );
	return;
    }

    QFileInfoList filist = dir.entryInfoList(QDir::All | QDir::Hidden | QDir::System);
    if ( filist.isEmpty() ) {
	QString msg = tr( "Could not read directory\n%1" ).arg( url()->path() );
	op->setState( StFailed );
	op->setProtocolDetail( msg );
	op->setErrorCode( (int)ErrListChildren );
	emit finished( op );
	return;
    }

    emit start( op );

    Q3ValueList<QUrlInfo> infos;
    for (int i = 0; i < filist.size(); ++i) {
        QFileInfo fi = filist.at(i);
	infos << QUrlInfo( fi.fileName(), convertPermissions(&fi), fi.owner(), fi.group(),
			   fi.size(), fi.lastModified(), fi.lastRead(), fi.isDir(), fi.isFile(),
			   fi.isSymLink(), fi.isWritable(), fi.isReadable(), fi.isExecutable() );
    }
    emit newChildren( infos, op );
    op->setState( StDone );
    emit finished( op );
}

/*!
    \reimp
*/

void Q3LocalFs::operationMkDir( Q3NetworkOperation *op )
{
#ifdef QLOCALFS_DEBUG
    qDebug( "Q3LocalFs: operationMkDir" );
#endif
    op->setState( StInProgress );
    QString dirname = op->arg( 0 );

    dir = QDir( url()->path() );
    if ( dir.mkdir( dirname ) ) {
	QFileInfo fi( dir, dirname );
	QUrlInfo inf( fi.fileName(), convertPermissions(&fi), fi.owner(), fi.group(),
		      fi.size(), fi.lastModified(), fi.lastRead(), fi.isDir(), fi.isFile(),
		      fi.isSymLink(), fi.isWritable(), fi.isReadable(), fi.isExecutable() );
	emit newChild( inf, op );
	op->setState( StDone );
	emit createdDirectory( inf, op );
	emit finished( op );
    } else {
	QString msg = tr( "Could not create directory\n%1" ).arg( dirname );
	op->setState( StFailed );
	op->setProtocolDetail( msg );
	op->setErrorCode( (int)ErrMkDir );
	emit finished( op );
    }
}

/*!
    \reimp
*/

void Q3LocalFs::operationRemove( Q3NetworkOperation *op )
{
#ifdef QLOCALFS_DEBUG
    qDebug( "Q3LocalFs: operationRemove" );
#endif
    op->setState( StInProgress );
    QString name = Q3Url( op->arg( 0 ) ).path();
    bool deleted = false;

    dir = QDir( url()->path() );

    QFileInfo fi( dir, name );
    if ( fi.isDir() ) {
	if ( dir.rmdir( name ) )
	    deleted = true;
    }

    if ( deleted || dir.remove( name ) ) {
	op->setState( StDone );
	emit removed( op );
	emit finished( op );
    } else {
	QString msg = tr( "Could not remove file or directory\n%1" ).arg( name );
	op->setState( StFailed );
	op->setProtocolDetail( msg );
	op->setErrorCode( (int)ErrRemove );
	emit finished( op );
    }
}

/*!
    \reimp
*/

void Q3LocalFs::operationRename( Q3NetworkOperation *op )
{
#ifdef QLOCALFS_DEBUG
    qDebug( "Q3LocalFs: operationRename" );
#endif
    op->setState( StInProgress );
    QString oldname = op->arg( 0 );
    QString newname = op->arg( 1 );

    dir = QDir( url()->path() );
    if ( dir.rename( oldname, newname ) ) {
	op->setState( StDone );
	emit itemChanged( op );
	emit finished( op );
    } else {
	QString msg = tr( "Could not rename\n%1\nto\n%2" ).arg( oldname ).arg( newname );
	op->setState( StFailed );
	op->setProtocolDetail( msg );
	op->setErrorCode( (int)ErrRename );
	emit finished( op );
    }
}

/*!
    \reimp
*/

void Q3LocalFs::operationGet( Q3NetworkOperation *op )
{
#ifdef QLOCALFS_DEBUG
    qDebug( "Q3LocalFs: operationGet" );
#endif
    op->setState( StInProgress );
    QString from = Q3Url( op->arg( 0 ) ).path();

    QFile f( from );
    if ( !f.open( IO_ReadOnly ) ) {
#ifdef QLOCALFS_DEBUG
	qDebug( "Q3LocalFs: could not open %s", from.latin1() );
#endif
	QString msg = tr( "Could not open\n%1" ).arg( from );
	op->setState( StFailed );
	op->setProtocolDetail( msg );
	op->setErrorCode( (int)ErrGet );
	emit finished( op );
	return;
    }

    QByteArray s;
    emit dataTransferProgress( 0, f.size(), op );
    if ( f.size() != 0 ) {
	int blockSize = calcBlockSize( f.size() );
	if ( (int)f.size() < blockSize ) {
	    s.resize( f.size() );
	    f.readBlock( s.data(), f.size() );
	    emit data( s, op );
	    emit dataTransferProgress( f.size(), f.size(), op );
#ifdef QLOCALFS_DEBUG
	    qDebug( "Q3LocalFs: got all %d bytes at once", f.size() );
#endif
	} else {
	    s.resize( blockSize );
	    int remaining = f.size();
	    QPointer<QObject> that = this;
	    while ( that && remaining > 0 ) {
		if ( operationInProgress() != op )
		    return;
		if ( remaining >= blockSize ) {
		    f.readBlock( s.data(), blockSize );
		    emit data( s, op );
		    emit dataTransferProgress( f.size() - remaining, f.size(), op );
		    remaining -= blockSize;
		} else {
		    s.resize( remaining );
		    f.readBlock( s.data(), remaining );
		    emit data( s, op );
		    emit dataTransferProgress( f.size() - remaining, f.size(), op );
		    remaining -= remaining;
		}
                qApp->processEvents();
	    }
	    if ( !that )
	        return;
#ifdef QLOCALFS_DEBUG
	    qDebug( "Q3LocalFs: got all %d bytes step by step", f.size() );
#endif
	    emit dataTransferProgress( f.size(), f.size(), op );
	}
    }
    op->setState( StDone );
    f.close();
    emit finished( op );
}

/*!
    \reimp
*/

void Q3LocalFs::operationPut( Q3NetworkOperation *op )
{
#ifdef QLOCALFS_DEBUG
    qDebug( "Q3LocalFs: operationPut" );
#endif
    op->setState( StInProgress );
    QString to = Q3Url( op->arg( 0 ) ).path();

    QFile f( to );
    if ( !f.open( IO_WriteOnly ) ) {
	QString msg = tr( "Could not write\n%1" ).arg( to );
	op->setState( StFailed );
	op->setProtocolDetail( msg );
	op->setErrorCode( (int)ErrPut );
	emit finished( op );
	return;
    }

    QByteArray ba( op->rawArg( 1 ) );
    emit dataTransferProgress( 0, ba.size(), op );
    int blockSize = calcBlockSize( ba.size() );
    if ( (int)ba.size() < blockSize ) {
	f.writeBlock( ba.data(), ba.size() );
	emit dataTransferProgress( ba.size(), ba.size(), op );
    } else {
	int i = 0;
	while ( i + blockSize < (int)ba.size() - 1 ) {
	    if ( operationInProgress() != op )
		return;
	    f.writeBlock( &ba.data()[ i ], blockSize );
	    f.flush();
	    emit dataTransferProgress( i + blockSize, ba.size(), op );
	    i += blockSize;
	    QPointer<QObject> that = this;
            qApp->processEvents();
            if (!that)
                return;
	}
	if ( i < (int)ba.size() - 1 )
	    f.writeBlock( &ba.data()[ i ], ba.size() - i );
	emit dataTransferProgress( ba.size(), ba.size(), op );
    }
    op->setState( StDone );
    f.close();
    emit finished( op );
}

/*!
    \reimp
*/

int Q3LocalFs::supportedOperations() const
{
    return OpListChildren | OpMkDir | OpRemove | OpRename | OpGet | OpPut;
}

/*!
    \internal
*/

int Q3LocalFs::calcBlockSize( int totalSize ) const
{
    if ( totalSize == 0 )
	return 1024;
    int s = totalSize / 100;
    // we want a block size between 1KB and 1MB
    if ( s < 1024 )
	s = 1024;
    if ( s > 1048576 )
	s = 1048576;
    return s;
}

QT_END_NAMESPACE

#endif // QT_NO_NETWORKPROTOCOL
