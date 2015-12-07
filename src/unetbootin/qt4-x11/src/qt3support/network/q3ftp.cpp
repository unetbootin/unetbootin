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
#include "q3ftp.h"

#ifndef QT_NO_NETWORKPROTOCOL_FTP

#include "q3socket.h"
#include "q3socketdevice.h"
#include "qurlinfo.h"
#include "q3urloperator.h"
#include "qstringlist.h"
#include "qregexp.h"
#include "qtimer.h"
#include "qfileinfo.h"
#include "q3ptrdict.h"
#include "q3cstring.h"
#include "qcoreapplication.h"
#include "qftp.h"

#ifndef QT_NO_TEXTCODEC
#include "qtextcodec.h"
#endif

QT_BEGIN_NAMESPACE

//#define Q3FTPPI_DEBUG
//#define Q3FTPDTP_DEBUG

class Q3FtpPI;

class Q3FtpDTP : public QObject
{
    Q_OBJECT

public:
    enum ConnectState {
	CsHostFound,
	CsConnected,
	CsClosed,
	CsHostNotFound,
	CsConnectionRefused
    };

    Q3FtpDTP( Q3FtpPI *p, QObject *parent=0, const char *name=0 );

    void setData( QByteArray * );
    void setDevice( QIODevice * );
    void writeData();

    void setBytesTotal( int bytes )
    {
	bytesTotal = bytes;
	bytesDone = 0;
	emit dataTransferProgress( bytesDone, bytesTotal );
    }

    bool hasError() const;
    QString errorMessage() const;
    void clearError();

    void connectToHost( const QString & host, Q_UINT16 port )
    { socket.connectToHost( host, port ); }

    Q3Socket::State socketState() const
    { return socket.state(); }

    Q_ULONG bytesAvailable() const
    { return socket.bytesAvailable(); }

    Q_LONG readBlock( char *data, Q_ULONG maxlen )
    {
	Q_LONG read = socket.readBlock( data, maxlen );
	bytesDone += read;
	return read;
    }

    QByteArray readAll()
    {
	QByteArray tmp = socket.readAll();
	bytesDone += tmp.size();
	return tmp;
    }

    void abortConnection();

    static bool parseDir( const QString &buffer, const QString &userName, QUrlInfo *info );

signals:
    void listInfo( const QUrlInfo& );
    void readyRead();
    void dataTransferProgress( int, int );

    void connectState( int );

private slots:
    void socketConnected();
    void socketReadyRead();
    void socketError( int );
    void socketConnectionClosed();
    void socketBytesWritten( int );

private:
    void clearData()
    {
	is_ba = false;
	data.dev = 0;
    }

    Q3Socket socket;
    Q3FtpPI *pi;
    QString err;
    int bytesDone;
    int bytesTotal;
    bool callWriteData;

    // If is_ba is true, ba is used; ba is never 0.
    // Otherwise dev is used; dev can be 0 or not.
    union {
	QByteArray *ba;
	QIODevice *dev;
    } data;
    bool is_ba;
};

class Q3FtpPI : public QObject
{
    Q_OBJECT

public:
    Q3FtpPI( QObject *parent = 0 );

    void connectToHost( const QString &host, Q_UINT16 port );

    bool sendCommands( const QStringList &cmds );
    bool sendCommand( const QString &cmd )
    { return sendCommands( QStringList( cmd ) ); }

    void clearPendingCommands();
    void abort();

    QString currentCommand() const
    { return currentCmd; }

    bool rawCommand;

    Q3FtpDTP dtp; // the PI has a DTP which is not the design of RFC 959, but it
		 // makes the design simpler this way
signals:
    void connectState( int );
    void finished( const QString& );
    void error( int, const QString& );
    void rawFtpReply( int, const QString& );

private slots:
    void hostFound();
    void connected();
    void connectionClosed();
    void delayedCloseFinished();
    void readyRead();
    void error( int );

    void dtpConnectState( int );

private:
    // the states are modelled after the generalized state diagram of RFC 959,
    // page 58
    enum State {
	Begin,
	Idle,
	Waiting,
	Success,
	Failure
    };

    enum AbortState {
	None,
	AbortStarted,
	WaitForAbortToFinish
    };

    bool processReply();
    bool startNextCmd();

    Q3Socket commandSocket;
    QString replyText;
    signed char replyCode[3];
    State state;
    AbortState abortState;
    QStringList pendingCommands;
    QString currentCmd;

    bool waitForDtpToConnect;
    bool waitForDtpToClose;
};

/**********************************************************************
 *
 * Q3FtpCommand implemenatation
 *
 *********************************************************************/
class Q3FtpCommand
{
public:
    Q3FtpCommand( Q3Ftp::Command cmd, QStringList raw );
    Q3FtpCommand( Q3Ftp::Command cmd, QStringList raw, const QByteArray &ba );
    Q3FtpCommand( Q3Ftp::Command cmd, QStringList raw, QIODevice *dev );
    ~Q3FtpCommand();

    int id;
    Q3Ftp::Command command;
    QStringList rawCmds;

    // If is_ba is true, ba is used; ba is never 0.
    // Otherwise dev is used; dev can be 0 or not.
    union {
	QByteArray *ba;
	QIODevice *dev;
    } data;
    bool is_ba;

    static int idCounter;
};

int Q3FtpCommand::idCounter = 0;

Q3FtpCommand::Q3FtpCommand( Q3Ftp::Command cmd, QStringList raw )
    : command(cmd), rawCmds(raw), is_ba(false)
{
    id = ++idCounter;
    data.dev = 0;
}

Q3FtpCommand::Q3FtpCommand( Q3Ftp::Command cmd, QStringList raw, const QByteArray &ba )
    : command(cmd), rawCmds(raw), is_ba(true)
{
    id = ++idCounter;
    data.ba = new QByteArray( ba );
}

Q3FtpCommand::Q3FtpCommand( Q3Ftp::Command cmd, QStringList raw, QIODevice *dev )
    : command(cmd), rawCmds(raw), is_ba(false)
{
    id = ++idCounter;
    data.dev = dev;
}

Q3FtpCommand::~Q3FtpCommand()
{
    if ( is_ba )
	delete data.ba;
}

/**********************************************************************
 *
 * Q3FtpDTP implemenatation
 *
 *********************************************************************/
Q3FtpDTP::Q3FtpDTP( Q3FtpPI *p, QObject *parent, const char *name ) :
    QObject( parent, name ),
    socket( 0, "Q3FtpDTP_socket" ),
    pi( p ),
    callWriteData( false )
{
    clearData();

    connect( &socket, SIGNAL(connected()),
	     SLOT(socketConnected()) );
    connect( &socket, SIGNAL(readyRead()),
	     SLOT(socketReadyRead()) );
    connect( &socket, SIGNAL(error(int)),
	     SLOT(socketError(int)) );
    connect( &socket, SIGNAL(connectionClosed()),
	     SLOT(socketConnectionClosed()) );
    connect( &socket, SIGNAL(bytesWritten(int)),
	     SLOT(socketBytesWritten(int)) );
}

void Q3FtpDTP::setData( QByteArray *ba )
{
    is_ba = true;
    data.ba = ba;
}

void Q3FtpDTP::setDevice( QIODevice *dev )
{
    is_ba = false;
    data.dev = dev;
}

void Q3FtpDTP::writeData()
{
    if ( is_ba ) {
#if defined(Q3FTPDTP_DEBUG)
	qDebug( "Q3FtpDTP::writeData: write %d bytes", data.ba->size() );
#endif
	if ( data.ba->size() == 0 )
	    emit dataTransferProgress( 0, bytesTotal );
	else
	    socket.writeBlock( data.ba->data(), data.ba->size() );
	socket.close();
	clearData();
    } else if ( data.dev ) {
	callWriteData = false;
	const int blockSize = 16*1024;
	char buf[blockSize];
	while ( !data.dev->atEnd() && socket.bytesToWrite()==0 ) {
	    Q_LONG read = data.dev->readBlock( buf, blockSize );
#if defined(Q3FTPDTP_DEBUG)
	    qDebug( "Q3FtpDTP::writeData: writeBlock() of size %d bytes", (int)read );
#endif
	    socket.writeBlock( buf, read );
	    if ( !data.dev )
		return; // this can happen when a command is aborted
	}
	if ( data.dev->atEnd() ) {
	    if ( bytesDone==0 && socket.bytesToWrite()==0 )
		emit dataTransferProgress( 0, bytesTotal );
	    socket.close();
	    clearData();
	} else {
	    callWriteData = true;
	}
    }
}

inline bool Q3FtpDTP::hasError() const
{
    return !err.isNull();
}

inline QString Q3FtpDTP::errorMessage() const
{
    return err;
}

inline void Q3FtpDTP::clearError()
{
    err.clear();
}

void Q3FtpDTP::abortConnection()
{
#if defined(Q3FTPDTP_DEBUG)
    qDebug( "Q3FtpDTP::abortConnection" );
#endif
    callWriteData = false;
    clearData();

    socket.clearPendingData();
    socket.close();
}

bool Q3FtpDTP::parseDir( const QString &buffer, const QString &userName, QUrlInfo *info )
{
    QStringList lst = QStringList::split( QLatin1String(" "), buffer );

    if ( lst.count() < 9 )
	return false;

    QString tmp;

    // permissions
    tmp = lst[ 0 ];

    if ( tmp[ 0 ] == QChar( QLatin1Char('d') ) ) {
	info->setDir( true );
	info->setFile( false );
	info->setSymLink( false );
    } else if ( tmp[ 0 ] == QChar( QLatin1Char('-') ) ) {
	info->setDir( false );
	info->setFile( true );
	info->setSymLink( false );
    } else if ( tmp[ 0 ] == QChar( QLatin1Char('l') ) ) {
	info->setDir( true ); // #### todo
	info->setFile( false );
	info->setSymLink( true );
    } else {
	return false;
    }

    static int user = 0;
    static int group = 1;
    static int other = 2;
    static int readable = 0;
    static int writable = 1;
    static int executable = 2;

    bool perms[ 3 ][ 3 ];
    perms[0][0] = (tmp[ 1 ] == QLatin1Char('r'));
    perms[0][1] = (tmp[ 2 ] == QLatin1Char('w'));
    perms[0][2] = (tmp[ 3 ] == QLatin1Char('x'));
    perms[1][0] = (tmp[ 4 ] == QLatin1Char('r'));
    perms[1][1] = (tmp[ 5 ] == QLatin1Char('w'));
    perms[1][2] = (tmp[ 6 ] == QLatin1Char('x'));
    perms[2][0] = (tmp[ 7 ] == QLatin1Char('r'));
    perms[2][1] = (tmp[ 8 ] == QLatin1Char('w'));
    perms[2][2] = (tmp[ 9 ] == QLatin1Char('x'));

    // owner
    tmp = lst[ 2 ];
    info->setOwner( tmp );

    // group
    tmp = lst[ 3 ];
    info->setGroup( tmp );

    // ### not correct
    info->setWritable( ( userName == info->owner() && perms[ user ][ writable ] ) ||
	perms[ other ][ writable ] );
    info->setReadable( ( userName == info->owner() && perms[ user ][ readable ] ) ||
	perms[ other ][ readable ] );

    int p = 0;
    if ( perms[ user ][ readable ] )
	p |= QUrlInfo::ReadOwner;
    if ( perms[ user ][ writable ] )
	p |= QUrlInfo::WriteOwner;
    if ( perms[ user ][ executable ] )
	p |= QUrlInfo::ExeOwner;
    if ( perms[ group ][ readable ] )
	p |= QUrlInfo::ReadGroup;
    if ( perms[ group ][ writable ] )
	p |= QUrlInfo::WriteGroup;
    if ( perms[ group ][ executable ] )
	p |= QUrlInfo::ExeGroup;
    if ( perms[ other ][ readable ] )
	p |= QUrlInfo::ReadOther;
    if ( perms[ other ][ writable ] )
	p |= QUrlInfo::WriteOther;
    if ( perms[ other ][ executable ] )
	p |= QUrlInfo::ExeOther;
    info->setPermissions( p );

    // size
    tmp = lst[ 4 ];
    info->setSize( tmp.toInt() );

    // date and time
    QTime time;
    QString dateStr;
    dateStr += QLatin1String("Sun ");
    lst[ 5 ][ 0 ] = lst[ 5 ][ 0 ].upper();
    dateStr += lst[ 5 ];
    dateStr += QLatin1Char(' ');
    dateStr += lst[ 6 ];
    dateStr += QLatin1Char(' ');

    if ( lst[ 7 ].contains( QLatin1String(":") ) ) {
	time = QTime( lst[ 7 ].left( 2 ).toInt(), lst[ 7 ].right( 2 ).toInt() );
	dateStr += QString::number( QDate::currentDate().year() );
    } else {
	dateStr += lst[ 7 ];
    }

    QDate date = QDate::fromString( dateStr );
    info->setLastModified( QDateTime( date, time ) );

    if ( lst[ 7 ].contains( QLatin1String(":") ) ) {
	const int futureTolerance = 600;
	if( info->lastModified().secsTo( QDateTime::currentDateTime() ) < -futureTolerance ) {
	    QDateTime dt = info->lastModified();
	    QDate d = dt.date();
	    d.setYMD(d.year()-1, d.month(), d.day());
	    dt.setDate(d);
	    info->setLastModified(dt);
	}
    }

    // name
    if ( info->isSymLink() )
	info->setName( lst[ 8 ].stripWhiteSpace() );
    else {
	QString n;
	for ( uint i = 8; i < (uint) lst.count(); ++i )
	    n += lst[ i ] + QLatin1String(" ");
	n = n.stripWhiteSpace();
	info->setName( n );
    }
    return true;
}

void Q3FtpDTP::socketConnected()
{
#if !defined (Q_WS_QWS)
    // Use a large send buffer to reduce the number
    // of writeBlocks when download and uploading files.
    // The actual size used here (128k) is default on most
    // Unixes.
    socket.socketDevice()->setSendBufferSize(128 * 1024);
    socket.socketDevice()->setReceiveBufferSize(128 * 1024);
#endif

    bytesDone = 0;
#if defined(Q3FTPDTP_DEBUG)
    qDebug( "Q3FtpDTP::connectState( CsConnected )" );
#endif
    emit connectState( Q3FtpDTP::CsConnected );
}

void Q3FtpDTP::socketReadyRead()
{
    if ( pi->currentCommand().isEmpty() ) {
	socket.close();
#if defined(Q3FTPDTP_DEBUG)
	qDebug( "Q3FtpDTP::connectState( CsClosed )" );
#endif
	emit connectState( Q3FtpDTP::CsClosed );
	return;
    }

    if ( pi->currentCommand().startsWith(QLatin1String("LIST")) ) {
	while ( socket.canReadLine() ) {
	    QUrlInfo i;
	    QString line = QLatin1String(socket.readLine());
#if defined(Q3FTPDTP_DEBUG)
	    qDebug( "Q3FtpDTP read (list): '%s'", line.latin1() );
#endif
	    if ( parseDir( line, QLatin1String(""), &i ) ) {
		emit listInfo( i );
	    } else {
		// some FTP servers don't return a 550 if the file or directory
		// does not exist, but rather write a text to the data socket
		// -- try to catch these cases
		if ( line.endsWith( QLatin1String("No such file or directory\r\n") ) )
		    err = line;
	    }
	}
    } else {
	if ( !is_ba && data.dev ) {
	    QByteArray ba( socket.bytesAvailable() );
	    Q_LONG bytesRead = socket.readBlock( ba.data(), ba.size() );
	    if ( bytesRead < 0 ) {
		// ### error handling
		return;
	    }
	    ba.resize( bytesRead );
	    bytesDone += bytesRead;
#if defined(Q3FTPDTP_DEBUG)
	    qDebug( "Q3FtpDTP read: %d bytes (total %d bytes)", (int)bytesRead, bytesDone );
#endif
	    emit dataTransferProgress( bytesDone, bytesTotal );
            if (data.dev)       // make sure it wasn't deleted in the slot
                data.dev->writeBlock( ba );
	} else {
#if defined(Q3FTPDTP_DEBUG)
	    qDebug( "Q3FtpDTP readyRead: %d bytes available (total %d bytes read)", (int)bytesAvailable(), bytesDone );
#endif
	    emit dataTransferProgress( bytesDone+socket.bytesAvailable(), bytesTotal );
	    emit readyRead();
	}
    }
}

void Q3FtpDTP::socketError( int e )
{
    if ( e == Q3Socket::ErrHostNotFound ) {
#if defined(Q3FTPDTP_DEBUG)
	qDebug( "Q3FtpDTP::connectState( CsHostNotFound )" );
#endif
	emit connectState( Q3FtpDTP::CsHostNotFound );
    } else if ( e == Q3Socket::ErrConnectionRefused ) {
#if defined(Q3FTPDTP_DEBUG)
	qDebug( "Q3FtpDTP::connectState( CsConnectionRefused )" );
#endif
	emit connectState( Q3FtpDTP::CsConnectionRefused );
    }
}

void Q3FtpDTP::socketConnectionClosed()
{
    if ( !is_ba && data.dev ) {
	clearData();
    }
#if defined(Q3FTPDTP_DEBUG)
    qDebug( "Q3FtpDTP::connectState( CsClosed )" );
#endif
    emit connectState( Q3FtpDTP::CsClosed );
}

void Q3FtpDTP::socketBytesWritten( int bytes )
{
    bytesDone += bytes;
#if defined(Q3FTPDTP_DEBUG)
    qDebug( "Q3FtpDTP::bytesWritten( %d )", bytesDone );
#endif
    emit dataTransferProgress( bytesDone, bytesTotal );
    if ( callWriteData )
	writeData();
}

/**********************************************************************
 *
 * Q3FtpPI implemenatation
 *
 *********************************************************************/
Q3FtpPI::Q3FtpPI( QObject *parent ) :
    QObject( parent ),
    rawCommand(false),
    dtp( this ),
    commandSocket( 0, "Q3FtpPI_socket" ),
    state( Begin ), abortState( None ),
    currentCmd( QString() ),
    waitForDtpToConnect( false ),
    waitForDtpToClose( false )
{
    connect( &commandSocket, SIGNAL(hostFound()),
	    SLOT(hostFound()) );
    connect( &commandSocket, SIGNAL(connected()),
	    SLOT(connected()) );
    connect( &commandSocket, SIGNAL(connectionClosed()),
	    SLOT(connectionClosed()) );
    connect( &commandSocket, SIGNAL(delayedCloseFinished()),
	    SLOT(delayedCloseFinished()) );
    connect( &commandSocket, SIGNAL(readyRead()),
	    SLOT(readyRead()) );
    connect( &commandSocket, SIGNAL(error(int)),
	    SLOT(error(int)) );

    connect( &dtp, SIGNAL(connectState(int)),
	     SLOT(dtpConnectState(int)) );
}

void Q3FtpPI::connectToHost( const QString &host, Q_UINT16 port )
{
    emit connectState( Q3Ftp::HostLookup );
    commandSocket.connectToHost( host, port );
}

/*
  Sends the sequence of commands \a cmds to the FTP server. When the commands
  are all done the finished() signal is emitted. When an error occurs, the
  error() signal is emitted.

  If there are pending commands in the queue this functions returns false and
  the \a cmds are not added to the queue; otherwise it returns true.
*/
bool Q3FtpPI::sendCommands( const QStringList &cmds )
{
    if ( !pendingCommands.isEmpty() )
	return false;

    if ( commandSocket.state()!=Q3Socket::Connected || state!=Idle ) {
	emit error( Q3Ftp::NotConnected, QFtp::tr( "Not connected" ) );
	return true; // there are no pending commands
    }

    pendingCommands = cmds;
    startNextCmd();
    return true;
}

void Q3FtpPI::clearPendingCommands()
{
    pendingCommands.clear();
    dtp.abortConnection();
    currentCmd.clear();
    state = Idle;
}

void Q3FtpPI::abort()
{
    pendingCommands.clear();

    if ( abortState != None )
	// ABOR already sent
	return;

    abortState = AbortStarted;
#if defined(Q3FTPPI_DEBUG)
    qDebug( "Q3FtpPI send: ABOR" );
#endif
    commandSocket.writeBlock( "ABOR\r\n", 6 );

    if ( currentCmd.startsWith(QLatin1String("STOR ")) )
	dtp.abortConnection();
}

void Q3FtpPI::hostFound()
{
    emit connectState( Q3Ftp::Connecting );
}

void Q3FtpPI::connected()
{
    state = Begin;
#if defined(Q3FTPPI_DEBUG)
//    qDebug( "Q3FtpPI state: %d [connected()]", state );
#endif
    emit connectState( Q3Ftp::Connected );
}

void Q3FtpPI::connectionClosed()
{
    commandSocket.close();
    emit connectState( Q3Ftp::Unconnected );
}

void Q3FtpPI::delayedCloseFinished()
{
    emit connectState( Q3Ftp::Unconnected );
}

void Q3FtpPI::error( int e )
{
    if ( e == Q3Socket::ErrHostNotFound ) {
	emit connectState( Q3Ftp::Unconnected );
	emit error( Q3Ftp::HostNotFound,
		    QFtp::tr( "Host %1 not found" ).arg( commandSocket.peerName() ) );
    } else if ( e == Q3Socket::ErrConnectionRefused ) {
	emit connectState( Q3Ftp::Unconnected );
	emit error( Q3Ftp::ConnectionRefused,
		    QFtp::tr( "Connection refused to host %1" ).arg( commandSocket.peerName() ) );
    }
}

void Q3FtpPI::readyRead()
{
    if ( waitForDtpToClose )
	return;

    while ( commandSocket.canReadLine() ) {
	// read line with respect to line continuation
	QString line = QLatin1String(commandSocket.readLine());
	if ( replyText.isEmpty() ) {
	    if ( line.length() < 3 ) {
		// ### protocol error
		return;
	    }
	    const int lowerLimit[3] = {1,0,0};
	    const int upperLimit[3] = {5,5,9};
	    for ( int i=0; i<3; i++ ) {
		replyCode[i] = line[i].digitValue();
		if ( replyCode[i]<lowerLimit[i] || replyCode[i]>upperLimit[i] ) {
		    // ### protocol error
		    return;
		}
	    }
	}
	QString endOfMultiLine;
	endOfMultiLine[0] = '0' + replyCode[0];
	endOfMultiLine[1] = '0' + replyCode[1];
	endOfMultiLine[2] = '0' + replyCode[2];
	endOfMultiLine[3] = ' ';
	QString lineCont( endOfMultiLine );
	lineCont[3] = '-';
	QString lineLeft4 = line.left(4);

	while ( lineLeft4 != endOfMultiLine ) {
	    if ( lineLeft4 == lineCont )
		replyText += line.mid( 4 ); // strip 'xyz-'
	    else
		replyText += line;
	    if ( !commandSocket.canReadLine() )
		return;
	    line = QLatin1String(commandSocket.readLine());
	    lineLeft4 = line.left(4);
	}
	replyText += line.mid( 4 ); // strip reply code 'xyz '
	if ( replyText.endsWith(QLatin1String("\r\n")) )
	    replyText.truncate( replyText.length()-2 );

	if ( processReply() )
	    replyText = QLatin1String("");
    }
}

/*
  Process a reply from the FTP server.

  Returns true if the reply was processed or false if the reply has to be
  processed at a later point.
*/
bool Q3FtpPI::processReply()
{
#if defined(Q3FTPPI_DEBUG)
//    qDebug( "Q3FtpPI state: %d [processReply() begin]", state );
    if ( replyText.length() < 400 )
	qDebug( "Q3FtpPI recv: %d %s", 100*replyCode[0]+10*replyCode[1]+replyCode[2], replyText.latin1() );
    else
	qDebug( "Q3FtpPI recv: %d (text skipped)", 100*replyCode[0]+10*replyCode[1]+replyCode[2] );
#endif

    // process 226 replies ("Closing Data Connection") only when the data
    // connection is really closed to avoid short reads of the DTP
    if ( 100*replyCode[0]+10*replyCode[1]+replyCode[2] == 226 ) {
	if ( dtp.socketState() != Q3Socket::Idle ) {
	    waitForDtpToClose = true;
	    return false;
	}
    }

    switch ( abortState ) {
	case AbortStarted:
	    abortState = WaitForAbortToFinish;
	    break;
	case WaitForAbortToFinish:
	    abortState = None;
	    return true;
	default:
	    break;
    }

    // get new state
    static const State table[5] = {
	/* 1yz   2yz      3yz   4yz      5yz */
	Waiting, Success, Idle, Failure, Failure
    };
    switch ( state ) {
	case Begin:
	    if ( replyCode[0] == 1 ) {
		return true;
	    } else if ( replyCode[0] == 2 ) {
		state = Idle;
		emit finished( QFtp::tr( "Connected to host %1" ).arg( commandSocket.peerName() ) );
		break;
	    }
	    // ### error handling
	    return true;
	case Waiting:
	    if ( replyCode[0]<0 || replyCode[0]>5 )
		state = Failure;
	    else
#if defined(Q_OS_IRIX) && defined(Q_CC_GNU)
            {
                // work around a crash on 64 bit gcc IRIX
                State *t = (State *) table;
                state = t[replyCode[0] - 1];
            }
#else
            state = table[replyCode[0] - 1];
#endif
	    break;
	default:
	    // ### spontaneous message
	    return true;
    }
#if defined(Q3FTPPI_DEBUG)
//    qDebug( "Q3FtpPI state: %d [processReply() intermediate]", state );
#endif

    // special actions on certain replies
    int replyCodeInt = 100*replyCode[0] + 10*replyCode[1] + replyCode[2];
    emit rawFtpReply( replyCodeInt, replyText );
    if ( rawCommand ) {
	rawCommand = false;
    } else if ( replyCodeInt == 227 ) {
	// 227 Entering Passive Mode (h1,h2,h3,h4,p1,p2)
        // rfc959 does not define this response precisely, and gives
        // both examples where the parenthesis are used, and where
        // they are missing. We need to scan for the address and host
        // info.
	QRegExp addrPortPattern(QLatin1String("(\\d+),(\\d+),(\\d+),(\\d+),(\\d+),(\\d+)"));
	if (addrPortPattern.search(replyText) == -1) {
#if defined(Q3FTPPI_DEBUG)
	    qDebug( "Q3Ftp: bad 227 response -- address and port information missing" );
#endif
	    // ### error handling
	} else {
	    QStringList lst = addrPortPattern.capturedTexts();
	    QString host = lst[1] + QLatin1String(".") + lst[2] + QLatin1String(".") + lst[3] + QLatin1String(".") + lst[4];
	    Q_UINT16 port = ( lst[5].toUInt() << 8 ) + lst[6].toUInt();
	    waitForDtpToConnect = true;
	    dtp.connectToHost( host, port );
	}
    } else if ( replyCodeInt == 230 ) {
	if ( currentCmd.startsWith(QLatin1String("USER ")) && pendingCommands.count()>0 &&
		pendingCommands.first().startsWith(QLatin1String("PASS ")) ) {
	    // no need to send the PASS -- we are already logged in
	    pendingCommands.pop_front();
	}
	// 230 User logged in, proceed.
	emit connectState( Q3Ftp::LoggedIn );
    } else if ( replyCodeInt == 213 ) {
	// 213 File status.
	if ( currentCmd.startsWith(QLatin1String("SIZE ")) )
	    dtp.setBytesTotal( replyText.simplifyWhiteSpace().toInt() );
    } else if ( replyCode[0]==1 && currentCmd.startsWith(QLatin1String("STOR ")) ) {
	dtp.writeData();
    }

    // react on new state
    switch ( state ) {
	case Begin:
	    // ### should never happen
	    break;
	case Success:
	    // ### success handling
	    state = Idle;
	    // no break!
	case Idle:
	    if ( dtp.hasError() ) {
		emit error( Q3Ftp::UnknownError, dtp.errorMessage() );
		dtp.clearError();
	    }
	    startNextCmd();
	    break;
	case Waiting:
	    // ### do nothing
	    break;
	case Failure:
	    emit error( Q3Ftp::UnknownError, replyText );
	    state = Idle;
	    startNextCmd();
	    break;
    }
#if defined(Q3FTPPI_DEBUG)
//    qDebug( "Q3FtpPI state: %d [processReply() end]", state );
#endif
    return true;
}

#ifndef QT_NO_TEXTCODEC
Q_COMPAT_EXPORT QTextCodec *qt_ftp_filename_codec = 0;
#endif

/*
  Starts next pending command. Returns false if there are no pending commands,
  otherwise it returns true.
*/
bool Q3FtpPI::startNextCmd()
{
    if ( waitForDtpToConnect )
	// don't process any new commands until we are connected
	return true;

#if defined(Q3FTPPI_DEBUG)
    if ( state != Idle )
	qDebug( "Q3FtpPI startNextCmd: Internal error! Q3FtpPI called in non-Idle state %d", state );
#endif
    if ( pendingCommands.isEmpty() ) {
	currentCmd.clear();
	emit finished( replyText );
	return false;
    }
    currentCmd = pendingCommands.first();
    pendingCommands.pop_front();
#if defined(Q3FTPPI_DEBUG)
    qDebug( "Q3FtpPI send: %s", currentCmd.left( currentCmd.length()-2 ).latin1() );
#endif
    state = Waiting;
#ifndef QT_NO_TEXTCODEC
    if ( qt_ftp_filename_codec ) {
	int len;
	Q3CString enc = qt_ftp_filename_codec->fromUnicode(currentCmd,len);
	commandSocket.writeBlock( enc.data(), len );
    } else
#endif
    {
	commandSocket.writeBlock( currentCmd.latin1(), currentCmd.length() );
    }
    return true;
}

void Q3FtpPI::dtpConnectState( int s )
{
    switch ( s ) {
	case Q3FtpDTP::CsClosed:
	    if ( waitForDtpToClose ) {
		// there is an unprocessed reply
		if ( processReply() )
		    replyText = QLatin1String("");
		else
		    return;
	    }
	    waitForDtpToClose = false;
	    readyRead();
	    return;
	case Q3FtpDTP::CsConnected:
	    waitForDtpToConnect = false;
	    startNextCmd();
	    return;
	case Q3FtpDTP::CsHostNotFound:
	case Q3FtpDTP::CsConnectionRefused:
	    emit error( Q3Ftp::ConnectionRefused,
			QFtp::tr( "Connection refused for data connection" ) );
	    startNextCmd();
	    return;
	default:
	    return;
    }
}

/**********************************************************************
 *
 * Q3FtpPrivate
 *
 *********************************************************************/
class Q3FtpPrivate
{
public:
    Q3FtpPrivate() :
	close_waitForStateChange(false),
	state( Q3Ftp::Unconnected ),
	error( Q3Ftp::NoError ),
	npWaitForLoginDone( false )
    { pending.setAutoDelete( true ); }

    Q3FtpPI pi;
    Q3PtrList<Q3FtpCommand> pending;
    bool close_waitForStateChange;
    Q3Ftp::State state;
    Q3Ftp::Error error;
    QString errorString;

    bool npWaitForLoginDone;
};

static Q3PtrDict<Q3FtpPrivate> *d_ptr = 0;
static void cleanup_d_ptr()
{
    delete d_ptr;
    d_ptr = 0;
}
static Q3FtpPrivate* dHelper( const Q3Ftp* foo )
{
    if ( !d_ptr ) {
	d_ptr = new Q3PtrDict<Q3FtpPrivate>;
	d_ptr->setAutoDelete( true );
	qAddPostRoutine( cleanup_d_ptr );
    }
    Q3FtpPrivate* ret = d_ptr->find( (void*)foo );
    if ( ! ret ) {
	ret = new Q3FtpPrivate;
	d_ptr->replace( (void*) foo, ret );
    }
    return ret;
}

static void delete_d( const Q3Ftp* foo )
{
    if ( d_ptr )
	d_ptr->remove( (void*) foo );
}

/**********************************************************************
 *
 * Q3Ftp implementation
 *
 *********************************************************************/
/*!
    \class Q3Ftp q3ftp.h
    \brief The Q3Ftp class provides an implementation of the FTP protocol.

    \compat

    This class provides two different interfaces: one is the
    QNetworkProtocol interface that allows you to use FTP through the
    QUrlOperator abstraction. The other is a direct interface to FTP
    that gives you lower-level access to the FTP protocol for finer
    control. Using the direct interface you can also execute arbitrary
    FTP commands.

    Don't mix the two interfaces, since the behavior is not
    well-defined.

    If you want to use Q3Ftp with the QNetworkProtocol interface, you
    do not use it directly, but rather through a QUrlOperator, for
    example:

    \snippet doc/src/snippets/code/src_qt3support_network_q3ftp.cpp 0

    This code will only work if the Q3Ftp class is registered; to
    register the class, you must call q3InitNetworkProtocols() before
    using a QUrlOperator with Q3Ftp.

    The rest of this descrption describes the direct interface to FTP.

    The class works asynchronously, so there are no blocking
    functions. If an operation cannot be executed immediately, the
    function will still return straight away and the operation will be
    scheduled for later execution. The results of scheduled operations
    are reported via signals. This approach depends on the event loop
    being in operation.

    The operations that can be scheduled (they are called "commands"
    in the rest of the documentation) are the following:
    connectToHost(), login(), close(), list(), cd(), get(), put(),
    remove(), mkdir(), rmdir(), rename() and rawCommand().

    All of these commands return a unique identifier that allows you
    to keep track of the command that is currently being executed.
    When the execution of a command starts, the commandStarted()
    signal with the command's identifier is emitted. When the command
    is finished, the commandFinished() signal is emitted with the
    command's identifier and a bool that indicates whether the command
    finished with an error.

    In some cases, you might want to execute a sequence of commands,
    e.g. if you want to connect and login to a FTP server. This is
    simply achieved:

    \snippet doc/src/snippets/code/src_qt3support_network_q3ftp.cpp 1

    In this case two FTP commands have been scheduled. When the last
    scheduled command has finished, a done() signal is emitted with
    a bool argument that tells you whether the sequence finished with
    an error.

    If an error occurs during the execution of one of the commands in
    a sequence of commands, all the pending commands (i.e. scheduled,
    but not yet executed commands) are cleared and no signals are
    emitted for them.

    Some commands, e.g. list(), emit additional signals to report
    their results.

    Example: If you want to download the INSTALL file from Trolltech's
    FTP server, you would write this:

    \snippet doc/src/snippets/code/src_qt3support_network_q3ftp.cpp 2

    For this example the following sequence of signals is emitted
    (with small variations, depending on network traffic, etc.):

    \snippet doc/src/snippets/code/src_qt3support_network_q3ftp.cpp 3

    The dataTransferProgress() signal in the above example is useful
    if you want to show a \link QProgressBar progress bar \endlink to
    inform the user about the progress of the download. The
    readyRead() signal tells you that there is data ready to be read.
    The amount of data can be queried then with the bytesAvailable()
    function and it can be read with the readBlock() or readAll()
    function.

    If the login fails for the above example, the signals would look
    like this:

    \snippet doc/src/snippets/code/src_qt3support_network_q3ftp.cpp 4

    You can then get details about the error with the error() and
    errorString() functions.

    The functions currentId() and currentCommand() provide more
    information about the currently executing command.

    The functions hasPendingCommands() and clearPendingCommands()
    allow you to query and clear the list of pending commands.

    The safest and easiest way to use the FTP protocol is to use
    QUrlOperator() or the FTP commands described above. If you are an
    experienced network programmer and want to have complete control
    you can use rawCommand() to execute arbitrary FTP commands.

    \sa Q3NetworkProtocol, Q3UrlOperator Q3Http
*/

/*!
    Constructs a Q3Ftp object.
*/
Q3Ftp::Q3Ftp() : Q3NetworkProtocol()
{
    init();
}

/*!
    Constructs a Q3Ftp object. The \a parent and \a name parameters
    are passed to the QObject constructor.
*/
Q3Ftp::Q3Ftp( QObject *parent, const char *name ) : Q3NetworkProtocol()
{
    if ( parent )
	parent->insertChild( this );
    setName( name );
    init();
}

void Q3Ftp::init()
{
    Q3FtpPrivate *d = dHelper( this );
    d->errorString = QFtp::tr( "Unknown error" );

    connect( &d->pi, SIGNAL(connectState(int)),
	    SLOT(piConnectState(int)) );
    connect( &d->pi, SIGNAL(finished(QString)),
	    SLOT(piFinished(QString)) );
    connect( &d->pi, SIGNAL(error(int,QString)),
	    SLOT(piError(int,QString)) );
    connect( &d->pi, SIGNAL(rawFtpReply(int,QString)),
	    SLOT(piFtpReply(int,QString)) );

    connect( &d->pi.dtp, SIGNAL(readyRead()),
	    SIGNAL(readyRead()) );
    connect( &d->pi.dtp, SIGNAL(dataTransferProgress(int,int)),
	    SIGNAL(dataTransferProgress(int,int)) );
    connect( &d->pi.dtp, SIGNAL(listInfo(QUrlInfo)),
	    SIGNAL(listInfo(QUrlInfo)) );
}

/*!
    \enum Q3Ftp::State

    This enum defines the connection state:

    \value Unconnected There is no connection to the host.
    \value HostLookup A host name lookup is in progress.
    \value Connecting An attempt to connect to the host is in progress.
    \value Connected Connection to the host has been achieved.
    \value LoggedIn Connection and user login have been achieved.
    \value Closing The connection is closing down, but it is not yet
    closed. (The state will be \c Unconnected when the connection is
    closed.)

    \sa stateChanged() state()
*/
/*!
    \enum Q3Ftp::Error

    This enum identifies the error that occurred.

    \value NoError No error occurred.
    \value HostNotFound The host name lookup failed.
    \value ConnectionRefused The server refused the connection.
    \value NotConnected Tried to send a command, but there is no connection to
    a server.
    \value UnknownError An error other than those specified above
    occurred.

    \sa error()
*/

/*!
    \enum Q3Ftp::Command

    This enum is used as the return value for the currentCommand() function.
    This allows you to perform specific actions for particular
    commands, e.g. in a FTP client, you might want to clear the
    directory view when a list() command is started; in this case you
    can simply check in the slot connected to the start() signal if
    the currentCommand() is \c List.

    \value None No command is being executed.
    \value ConnectToHost connectToHost() is being executed.
    \value Login login() is being executed.
    \value Close close() is being executed.
    \value List list() is being executed.
    \value Cd cd() is being executed.
    \value Get get() is being executed.
    \value Put put() is being executed.
    \value Remove remove() is being executed.
    \value Mkdir mkdir() is being executed.
    \value Rmdir rmdir() is being executed.
    \value Rename rename() is being executed.
    \value RawCommand rawCommand() is being executed.

    \sa currentCommand()
*/

/*!
    \fn void Q3Ftp::stateChanged( int state )

    This signal is emitted when the state of the connection changes.
    The argument \a state is the new state of the connection; it is
    one of the \l State values.

    It is usually emitted in response to a connectToHost() or close()
    command, but it can also be emitted "spontaneously", e.g. when the
    server closes the connection unexpectedly.

    \sa connectToHost() close() state() State
*/

/*!
    \fn void Q3Ftp::listInfo( const QUrlInfo &i );

    This signal is emitted for each directory entry the list() command
    finds. The details of the entry are stored in \a i.

    \sa list()
*/

/*!
    \fn void Q3Ftp::commandStarted( int id )

    This signal is emitted when processing the command identified by
    \a id starts.

    \sa commandFinished() done()
*/

/*!
    \fn void Q3Ftp::commandFinished( int id, bool error )

    This signal is emitted when processing the command identified by
    \a id has finished. \a error is true if an error occurred during
    the processing; otherwise \a error is false.

    \sa commandStarted() done() error() errorString()
*/

/*!
    \fn void Q3Ftp::done( bool error )

    This signal is emitted when the last pending command has finished;
    (it is emitted after the last command's commandFinished() signal).
    \a error is true if an error occurred during the processing;
    otherwise \a error is false.

    \sa commandFinished() error() errorString()
*/

/*!
    \fn void Q3Ftp::readyRead()

    This signal is emitted in response to a get() command when there
    is new data to read.

    If you specify a device as the second argument in the get()
    command, this signal is \e not emitted; instead the data is
    written directly to the device.

    You can read the data with the readAll() or readBlock() functions.

    This signal is useful if you want to process the data in chunks as
    soon as it becomes available. If you are only interested in the
    complete data, just connect to the commandFinished() signal and
    read the data then instead.

    \sa get() readBlock() readAll() bytesAvailable()
*/

/*!
    \fn void Q3Ftp::dataTransferProgress( int done, int total )

    This signal is emitted in response to a get() or put() request to
    indicate the current progress of the download or upload.

    \a done is the amount of data that has already been transferred
    and \a total is the total amount of data to be read or written. It
    is possible that the Q3Ftp class is not able to determine the total
    amount of data that should be transferred, in which case \a total
    is 0. (If you connect this signal to a QProgressBar, the progress
    bar shows a busy indicator if the total is 0).

    \warning \a done and \a total are not necessarily the size in
    bytes, since for large files these values might need to be
    "scaled" to avoid overflow.

    \sa get() put()
*/

/*!
    \fn void Q3Ftp::rawCommandReply( int replyCode, const QString &detail );

    This signal is emitted in response to the rawCommand() function.
    \a replyCode is the 3 digit reply code and \a detail is the text
    that follows the reply code.

    \sa rawCommand()
*/

/*!
    Connects to the FTP server \a host using port \a port.

    The stateChanged() signal is emitted when the state of the
    connecting process changes, e.g. to \c HostLookup, then \c
    Connecting, then \c Connected.

    The function does not block and returns immediately. The command
    is scheduled, and its execution is performed asynchronously. The
    function returns a unique identifier which is passed by
    commandStarted() and commandFinished().

    When the command is started the commandStarted() signal is
    emitted. When it is finished the commandFinished() signal is
    emitted.

    \sa stateChanged() commandStarted() commandFinished()
*/
int Q3Ftp::connectToHost( const QString &host, Q_UINT16 port )
{
    QStringList cmds;
    cmds << host;
    cmds << QString::number( (uint)port );
    return addCommand( new Q3FtpCommand( ConnectToHost, cmds ) );
}

/*!
    Logs in to the FTP server with the username \a user and the
    password \a password.

    The stateChanged() signal is emitted when the state of the
    connecting process changes, e.g. to \c LoggedIn.

    The function does not block and returns immediately. The command
    is scheduled, and its execution is performed asynchronously. The
    function returns a unique identifier which is passed by
    commandStarted() and commandFinished().

    When the command is started the commandStarted() signal is
    emitted. When it is finished the commandFinished() signal is
    emitted.

    \sa commandStarted() commandFinished()
*/
int Q3Ftp::login( const QString &user, const QString &password )
{
    QStringList cmds;
    cmds << ( QString(QLatin1String("USER ")) + ( user.isNull() ? QString(QLatin1String("anonymous")) : user ) + QLatin1String("\r\n") );
    cmds << ( QString(QLatin1String("PASS ")) + ( password.isNull() ? QString(QLatin1String("anonymous@")) : password ) + QLatin1String("\r\n") );
    return addCommand( new Q3FtpCommand( Login, cmds ) );
}

/*!
    Closes the connection to the FTP server.

    The stateChanged() signal is emitted when the state of the
    connecting process changes, e.g. to \c Closing, then \c
    Unconnected.

    The function does not block and returns immediately. The command
    is scheduled, and its execution is performed asynchronously. The
    function returns a unique identifier which is passed by
    commandStarted() and commandFinished().

    When the command is started the commandStarted() signal is
    emitted. When it is finished the commandFinished() signal is
    emitted.

    \sa stateChanged() commandStarted() commandFinished()
*/
int Q3Ftp::close()
{
    return addCommand( new Q3FtpCommand( Close, QStringList(QLatin1String("QUIT\r\n")) ) );
}

/*!
    Lists the contents of directory \a dir on the FTP server. If \a
    dir is empty, it lists the contents of the current directory.

    The listInfo() signal is emitted for each directory entry found.

    The function does not block and returns immediately. The command
    is scheduled, and its execution is performed asynchronously. The
    function returns a unique identifier which is passed by
    commandStarted() and commandFinished().

    When the command is started the commandStarted() signal is
    emitted. When it is finished the commandFinished() signal is
    emitted.

    \sa listInfo() commandStarted() commandFinished()
*/
int Q3Ftp::list( const QString &dir )
{
    QStringList cmds;
    cmds << QLatin1String("TYPE A\r\n");
    cmds << QLatin1String("PASV\r\n");
    if ( dir.isEmpty() )
	cmds << QLatin1String("LIST\r\n");
    else
	cmds << ( QLatin1String("LIST ") + dir + QLatin1String("\r\n") );
    return addCommand( new Q3FtpCommand( List, cmds ) );
}

/*!
    Changes the working directory of the server to \a dir.

    The function does not block and returns immediately. The command
    is scheduled, and its execution is performed asynchronously. The
    function returns a unique identifier which is passed by
    commandStarted() and commandFinished().

    When the command is started the commandStarted() signal is
    emitted. When it is finished the commandFinished() signal is
    emitted.

    \sa commandStarted() commandFinished()
*/
int Q3Ftp::cd( const QString &dir )
{
    return addCommand( new Q3FtpCommand( Cd, QStringList(QLatin1String("CWD ")+dir+QLatin1String("\r\n")) ) );
}

/*!
    Downloads the file \a file from the server.

    If \a dev is 0, then the readyRead() signal is emitted when there
    is data available to read. You can then read the data with the
    readBlock() or readAll() functions.

    If \a dev is not 0, the data is written directly to the device \a
    dev. Make sure that the \a dev pointer is valid for the duration
    of the operation (it is safe to delete it when the
    commandFinished() signal is emitted). In this case the readyRead()
    signal is \e not emitted and you cannot read data with the
    readBlock() or readAll() functions.

    If you don't read the data immediately it becomes available, i.e.
    when the readyRead() signal is emitted, it is still available
    until the next command is started.

    For example, if you want to present the data to the user as soon
    as there is something available, connect to the readyRead() signal
    and read the data immediately. On the other hand, if you only want
    to work with the complete data, you can connect to the
    commandFinished() signal and read the data when the get() command
    is finished.

    The function does not block and returns immediately. The command
    is scheduled, and its execution is performed asynchronously. The
    function returns a unique identifier which is passed by
    commandStarted() and commandFinished().

    When the command is started the commandStarted() signal is
    emitted. When it is finished the commandFinished() signal is
    emitted.

    \sa readyRead() dataTransferProgress() commandStarted()
    commandFinished()
*/
int Q3Ftp::get( const QString &file, QIODevice *dev )
{
    QStringList cmds;
    cmds << ( QLatin1String("SIZE ") + file + QLatin1String("\r\n") );
    cmds << QLatin1String("TYPE I\r\n");
    cmds << QLatin1String("PASV\r\n");
    cmds << ( QLatin1String("RETR ") + file + QLatin1String("\r\n") );
    if ( dev )
	return addCommand( new Q3FtpCommand( Get, cmds, dev ) );
    return addCommand( new Q3FtpCommand( Get, cmds ) );
}

/*!
    \overload

    Writes the data \a data to the file called \a file on the server.
    The progress of the upload is reported by the
    dataTransferProgress() signal.

    The function does not block and returns immediately. The command
    is scheduled, and its execution is performed asynchronously. The
    function returns a unique identifier which is passed by
    commandStarted() and commandFinished().

    When the command is started the commandStarted() signal is
    emitted. When it is finished the commandFinished() signal is
    emitted.

    \sa dataTransferProgress() commandStarted() commandFinished()
*/
int Q3Ftp::put( const QByteArray &data, const QString &file )
{
    QStringList cmds;
    cmds << QLatin1String("TYPE I\r\n");
    cmds << QLatin1String("PASV\r\n");
    cmds << ( QLatin1String("ALLO ") + QString::number(data.size()) + QLatin1String("\r\n") );
    cmds << ( QLatin1String("STOR ") + file + QLatin1String("\r\n") );
    return addCommand( new Q3FtpCommand( Put, cmds, data ) );
}

/*!
    Reads the data from the IO device \a dev, and writes it to the
    file called \a file on the server. The data is read in chunks from
    the IO device, so this overload allows you to transmit large
    amounts of data without the need to read all the data into memory
    at once.

    Make sure that the \a dev pointer is valid for the duration of the
    operation (it is safe to delete it when the commandFinished() is
    emitted).
*/
int Q3Ftp::put( QIODevice *dev, const QString &file )
{
    QStringList cmds;
    cmds << QLatin1String("TYPE I\r\n");
    cmds << QLatin1String("PASV\r\n");
    if ( !dev->isSequentialAccess() )
	cmds << ( QLatin1String("ALLO ") + QString::number(dev->size()) + QLatin1String("\r\n") );
    cmds << ( QLatin1String("STOR ") + file + QLatin1String("\r\n") );
    return addCommand( new Q3FtpCommand( Put, cmds, dev ) );
}

/*!
    Deletes the file called \a file from the server.

    The function does not block and returns immediately. The command
    is scheduled, and its execution is performed asynchronously. The
    function returns a unique identifier which is passed by
    commandStarted() and commandFinished().

    When the command is started the commandStarted() signal is
    emitted. When it is finished the commandFinished() signal is
    emitted.

    \sa commandStarted() commandFinished()
*/
int Q3Ftp::remove( const QString &file )
{
    return addCommand( new Q3FtpCommand( Remove, QStringList(QLatin1String("DELE ")+file+QLatin1String("\r\n")) ) );
}

/*!
    Creates a directory called \a dir on the server.

    The function does not block and returns immediately. The command
    is scheduled, and its execution is performed asynchronously. The
    function returns a unique identifier which is passed by
    commandStarted() and commandFinished().

    When the command is started the commandStarted() signal is
    emitted. When it is finished the commandFinished() signal is
    emitted.

    \sa commandStarted() commandFinished()
*/
int Q3Ftp::mkdir( const QString &dir )
{
    return addCommand( new Q3FtpCommand( Mkdir, QStringList(QLatin1String("MKD ")+dir+QLatin1String("\r\n")) ) );
}

/*!
    Removes the directory called \a dir from the server.

    The function does not block and returns immediately. The command
    is scheduled, and its execution is performed asynchronously. The
    function returns a unique identifier which is passed by
    commandStarted() and commandFinished().

    When the command is started the commandStarted() signal is
    emitted. When it is finished the commandFinished() signal is
    emitted.

    \sa commandStarted() commandFinished()
*/
int Q3Ftp::rmdir( const QString &dir )
{
    return addCommand( new Q3FtpCommand( Rmdir, QStringList(QLatin1String("RMD ")+dir+QLatin1String("\r\n")) ) );
}

/*!
    Renames the file called \a oldname to \a newname on the server.

    The function does not block and returns immediately. The command
    is scheduled, and its execution is performed asynchronously. The
    function returns a unique identifier which is passed by
    commandStarted() and commandFinished().

    When the command is started the commandStarted() signal is
    emitted. When it is finished the commandFinished() signal is
    emitted.

    \sa commandStarted() commandFinished()
*/
int Q3Ftp::rename( const QString &oldname, const QString &newname )
{
    QStringList cmds;
    cmds << ( QLatin1String("RNFR ") + oldname + QLatin1String("\r\n") );
    cmds << ( QLatin1String("RNTO ") + newname + QLatin1String("\r\n") );
    return addCommand( new Q3FtpCommand( Rename, cmds ) );
}

/*!
    Sends the raw FTP command \a command to the FTP server. This is
    useful for low-level FTP access. If the operation you wish to
    perform has an equivalent Q3Ftp function, we recommend using the
    function instead of raw FTP commands since the functions are
    easier and safer.

    The function does not block and returns immediately. The command
    is scheduled, and its execution is performed asynchronously. The
    function returns a unique identifier which is passed by
    commandStarted() and commandFinished().

    When the command is started the commandStarted() signal is
    emitted. When it is finished the commandFinished() signal is
    emitted.

    \sa rawCommandReply() commandStarted() commandFinished()
*/
int Q3Ftp::rawCommand( const QString &command )
{
    QString cmd = command.stripWhiteSpace() + QLatin1String("\r\n");
    return addCommand( new Q3FtpCommand( RawCommand, QStringList(cmd) ) );
}

/*!
    Returns the number of bytes that can be read from the data socket
    at the moment.

    \sa get() readyRead() readBlock() readAll()
*/
Q_ULONG Q3Ftp::bytesAvailable() const
{
    Q3FtpPrivate *d = dHelper( this );
    return d->pi.dtp.bytesAvailable();
}

/*!
    Reads \a maxlen bytes from the data socket into \a data and
    returns the number of bytes read. Returns -1 if an error occurred.

    \sa get() readyRead() bytesAvailable() readAll()
*/
Q_LONG Q3Ftp::readBlock( char *data, Q_ULONG maxlen )
{
    Q3FtpPrivate *d = dHelper( this );
    return d->pi.dtp.readBlock( data, maxlen );
}

/*!
    Reads all the bytes available from the data socket and returns
    them.

    \sa get() readyRead() bytesAvailable() readBlock()
*/
QByteArray Q3Ftp::readAll()
{
    Q3FtpPrivate *d = dHelper( this );
    return d->pi.dtp.readAll();
}

/*!
    Aborts the current command and deletes all scheduled commands.

    If there is an unfinished command (i.e. a command for which the
    commandStarted() signal has been emitted, but for which the
    commandFinished() signal has not been emitted), this function
    sends an \c ABORT command to the server. When the server replies
    that the command is aborted, the commandFinished() signal with the
    \c error argument set to \c true is emitted for the command. Due
    to timing issues, it is possible that the command had already
    finished before the abort request reached the server, in which
    case, the commandFinished() signal is emitted with the \c error
    argument set to \c false.

    For all other commands that are affected by the abort(), no
    signals are emitted.

    If you don't start further FTP commands directly after the
    abort(), there won't be any scheduled commands and the done()
    signal is emitted.

    \warning Some FTP servers, for example the BSD FTP daemon (version
    0.3), wrongly return a positive reply even when an abort has
    occurred. For these servers the commandFinished() signal has its
    error flag set to \c false, even though the command did not
    complete successfully.

    \sa clearPendingCommands()
*/
void Q3Ftp::abort()
{
    Q3FtpPrivate *d = dHelper( this );
    if ( d->pending.isEmpty() )
	return;

    clearPendingCommands();
    d->pi.abort();
}

/*!
    Returns the identifier of the FTP command that is being executed
    or 0 if there is no command being executed.

    \sa currentCommand()
*/
int Q3Ftp::currentId() const
{
    Q3FtpPrivate *d = dHelper( this );
    Q3FtpCommand *c = d->pending.getFirst();
    if ( c == 0 )
	return 0;
    return c->id;
}

/*!
    Returns the command type of the FTP command being executed or \c
    None if there is no command being executed.

    \sa currentId()
*/
Q3Ftp::Command Q3Ftp::currentCommand() const
{
    Q3FtpPrivate *d = dHelper( this );
    Q3FtpCommand *c = d->pending.getFirst();
    if ( c == 0 )
	return None;
    return c->command;
}

/*!
    Returns the QIODevice pointer that is used by the FTP command to read data
    from or store data to. If there is no current FTP command being executed or
    if the command does not use an IO device, this function returns 0.

    This function can be used to delete the QIODevice in the slot connected to
    the commandFinished() signal.

    \sa get() put()
*/
QIODevice* Q3Ftp::currentDevice() const
{
    Q3FtpPrivate *d = dHelper( this );
    Q3FtpCommand *c = d->pending.getFirst();
    if ( !c )
	return 0;
    if ( c->is_ba )
	return 0;
    return c->data.dev;
}

/*!
    Returns true if there are any commands scheduled that have not yet
    been executed; otherwise returns false.

    The command that is being executed is \e not considered as a
    scheduled command.

    \sa clearPendingCommands() currentId() currentCommand()
*/
bool Q3Ftp::hasPendingCommands() const
{
    Q3FtpPrivate *d = dHelper( this );
    return d->pending.count() > 1;
}

/*!
    Deletes all pending commands from the list of scheduled commands.
    This does not affect the command that is being executed. If you
    want to stop this this as well, use abort().

    \sa hasPendingCommands() abort()
*/
void Q3Ftp::clearPendingCommands()
{
    Q3FtpPrivate *d = dHelper( this );
    Q3FtpCommand *c = 0;
    if ( d->pending.count() > 0 )
	c = d->pending.take( 0 );
    d->pending.clear();
    if ( c )
	d->pending.append( c );
}

/*!
    Returns the current state of the object. When the state changes,
    the stateChanged() signal is emitted.

    \sa State stateChanged()
*/
Q3Ftp::State Q3Ftp::state() const
{
    Q3FtpPrivate *d = dHelper( this );
    return d->state;
}

/*!
    Returns the last error that occurred. This is useful to find out
    what when wrong when receiving a commandFinished() or a done()
    signal with the \c error argument set to \c true.

    If you start a new command, the error status is reset to \c NoError.
*/
Q3Ftp::Error Q3Ftp::error() const
{
    Q3FtpPrivate *d = dHelper( this );
    return d->error;
}

/*!
    Returns a human-readable description of the last error that
    occurred. This is useful for presenting a error message to the
    user when receiving a commandFinished() or a done() signal with
    the \c error argument set to \c true.

    The error string is often (but not always) the reply from the
    server, so it is not always possible to translate the string. If
    the message comes from Qt, the string has already passed through
    tr().
*/
QString Q3Ftp::errorString() const
{
    Q3FtpPrivate *d = dHelper( this );
    return d->errorString;
}

int Q3Ftp::addCommand( Q3FtpCommand *cmd )
{
    Q3FtpPrivate *d = dHelper( this );
    d->pending.append( cmd );

    if ( d->pending.count() == 1 )
	// don't emit the commandStarted() signal before the id is returned
	QTimer::singleShot( 0, this, SLOT(startNextCommand()) );

    return cmd->id;
}

void Q3Ftp::startNextCommand()
{
    Q3FtpPrivate *d = dHelper( this );

    Q3FtpCommand *c = d->pending.getFirst();
    if ( c == 0 )
	return;

    d->error = NoError;
    d->errorString = QFtp::tr( "Unknown error" );

    if ( bytesAvailable() )
	readAll(); // clear the data
    emit commandStarted( c->id );

    if ( c->command == ConnectToHost ) {
	d->pi.connectToHost( c->rawCmds[0], c->rawCmds[1].toUInt() );
    } else {
	if ( c->command == Put ) {
	    if ( c->is_ba ) {
		d->pi.dtp.setData( c->data.ba );
		d->pi.dtp.setBytesTotal( c->data.ba->size() );
	    } else if ( c->data.dev && (c->data.dev->isOpen() || c->data.dev->open(QIODevice::ReadOnly)) ) {
		d->pi.dtp.setDevice( c->data.dev );
		if ( c->data.dev->isSequentialAccess() )
		    d->pi.dtp.setBytesTotal( 0 );
		else
		    d->pi.dtp.setBytesTotal( c->data.dev->size() );
	    }
	} else if ( c->command == Get ) {
	    if ( !c->is_ba && c->data.dev ) {
		d->pi.dtp.setDevice( c->data.dev );
	    }
	} else if ( c->command == Close ) {
	    d->state = Q3Ftp::Closing;
	    emit stateChanged( d->state );
	}
	if ( !d->pi.sendCommands( c->rawCmds ) ) {
	    // ### error handling (this case should not happen)
	}
    }
}

void Q3Ftp::piFinished( const QString& )
{
    Q3FtpPrivate *d = dHelper( this );
    Q3FtpCommand *c = d->pending.getFirst();
    if ( c == 0 )
	return;

    if ( c->command == Close ) {
	// The order of in which the slots are called is arbitrary, so
	// disconnect the SIGNAL-SIGNAL temporary to make sure that we
	// don't get the commandFinished() signal before the stateChanged()
	// signal.
	if ( d->state != Q3Ftp::Unconnected ) {
	    d->close_waitForStateChange = true;
	    return;
	}
    }
    emit commandFinished( c->id, false );

    d->pending.removeFirst();
    if ( d->pending.isEmpty() ) {
	emit done( false );
    } else {
	startNextCommand();
    }
}

void Q3Ftp::piError( int errorCode, const QString &text )
{
    Q3FtpPrivate *d = dHelper( this );
    Q3FtpCommand *c = d->pending.getFirst();

    // non-fatal errors
    if ( c->command==Get && d->pi.currentCommand().startsWith(QLatin1String("SIZE ")) ) {
	d->pi.dtp.setBytesTotal( -1 );
	return;
    } else if ( c->command==Put && d->pi.currentCommand().startsWith(QLatin1String("ALLO ")) ) {
	return;
    }

    d->error = (Error)errorCode;
    switch ( currentCommand() ) {
	case ConnectToHost:
	    d->errorString = QFtp::tr( "Connecting to host failed:\n%1" ).arg( text );
	    break;
	case Login:
	    d->errorString = QFtp::tr( "Login failed:\n%1" ).arg( text );
	    break;
	case List:
	    d->errorString = QFtp::tr( "Listing directory failed:\n%1" ).arg( text );
	    break;
	case Cd:
	    d->errorString = QFtp::tr( "Changing directory failed:\n%1" ).arg( text );
	    break;
	case Get:
	    d->errorString = QFtp::tr( "Downloading file failed:\n%1" ).arg( text );
	    break;
	case Put:
	    d->errorString = QFtp::tr( "Uploading file failed:\n%1" ).arg( text );
	    break;
	case Remove:
	    d->errorString = QFtp::tr( "Removing file failed:\n%1" ).arg( text );
	    break;
	case Mkdir:
	    d->errorString = QFtp::tr( "Creating directory failed:\n%1" ).arg( text );
	    break;
	case Rmdir:
	    d->errorString = QFtp::tr( "Removing directory failed:\n%1" ).arg( text );
	    break;
	default:
	    d->errorString = text;
	    break;
    }

    d->pi.clearPendingCommands();
    clearPendingCommands();
    emit commandFinished( c->id, true );

    d->pending.removeFirst();
    if ( d->pending.isEmpty() )
	emit done( true );
    else
	startNextCommand();
}

void Q3Ftp::piConnectState( int state )
{
    Q3FtpPrivate *d = dHelper( this );
    d->state = (State)state;
    emit stateChanged( d->state );
    if ( d->close_waitForStateChange ) {
	d->close_waitForStateChange = false;
	piFinished( QFtp::tr( "Connection closed" ) );
    }
}

void Q3Ftp::piFtpReply( int code, const QString &text )
{
    if ( currentCommand() == RawCommand ) {
	Q3FtpPrivate *d = dHelper( this );
	d->pi.rawCommand = true;
	emit rawCommandReply( code, text );
    }
}

/*!
    Destructor.
*/
Q3Ftp::~Q3Ftp()
{
    abort();
    close();
    delete_d( this );
}

/**********************************************************************
 *
 * Q3Ftp implementation of the Q3NetworkProtocol interface
 *
 *********************************************************************/
/*!  \reimp
*/
void Q3Ftp::operationListChildren( Q3NetworkOperation *op )
{
    op->setState( StInProgress );

    cd( ( url()->path().isEmpty() ? QString( QLatin1String("/") ) : url()->path() ) );
    list();
    emit start( op );
}

/*!  \reimp
*/
void Q3Ftp::operationMkDir( Q3NetworkOperation *op )
{
    op->setState( StInProgress );

    mkdir( op->arg( 0 ) );
}

/*!  \reimp
*/
void Q3Ftp::operationRemove( Q3NetworkOperation *op )
{
    op->setState( StInProgress );

    cd( ( url()->path().isEmpty() ? QString( QLatin1String("/") ) : url()->path() ) );
    remove( Q3Url( op->arg( 0 ) ).path() );
}

/*!  \reimp
*/
void Q3Ftp::operationRename( Q3NetworkOperation *op )
{
    op->setState( StInProgress );

    cd( ( url()->path().isEmpty() ? QString( QLatin1String("/") ) : url()->path() ) );
    rename( op->arg( 0 ), op->arg( 1 ));
}

/*!  \reimp
*/
void Q3Ftp::operationGet( Q3NetworkOperation *op )
{
    op->setState( StInProgress );

    Q3Url u( op->arg( 0 ) );
    get( u.path() );
}

/*!  \reimp
*/
void Q3Ftp::operationPut( Q3NetworkOperation *op )
{
    op->setState( StInProgress );

    Q3Url u( op->arg( 0 ) );
    put( op->rawArg(1), u.path() );
}

/*!  \reimp
*/
bool Q3Ftp::checkConnection( Q3NetworkOperation *op )
{
    Q3FtpPrivate *d = dHelper( this );
    if ( state() == Unconnected && !d->npWaitForLoginDone ) {
	connect( this, SIGNAL(listInfo(QUrlInfo)),
		this, SLOT(npListInfo(QUrlInfo)) );
	connect( this, SIGNAL(done(bool)),
		this, SLOT(npDone(bool)) );
	connect( this, SIGNAL(stateChanged(int)),
		this, SLOT(npStateChanged(int)) );
	connect( this, SIGNAL(dataTransferProgress(int,int)),
		this, SLOT(npDataTransferProgress(int,int)) );
	connect( this, SIGNAL(readyRead()),
		this, SLOT(npReadyRead()) );

	d->npWaitForLoginDone = true;
	switch ( op->operation() ) {
	    case OpGet:
	    case OpPut:
		{
		    Q3Url u( op->arg( 0 ) );
		    connectToHost( u.host(), u.port() != -1 ? u.port() : 21 );
		}
		break;
	    default:
		connectToHost( url()->host(), url()->port() != -1 ? url()->port() : 21 );
		break;
	}
	QString user = url()->user().isEmpty() ? QString( QLatin1String("anonymous") ) : url()->user();
	QString pass = url()->password().isEmpty() ? QString( QLatin1String("anonymous@") ) : url()->password();
	login( user, pass );
    }

    if ( state() == LoggedIn )
	return true;
    return false;
}

/*!  \reimp
*/
int Q3Ftp::supportedOperations() const
{
    return OpListChildren | OpMkDir | OpRemove | OpRename | OpGet | OpPut;
}

/*! \internal
    Parses the string, \a buffer, which is one line of a directory
    listing which came from the FTP server, and sets the values which
    have been parsed to the url info object, \a info.
*/
void Q3Ftp::parseDir( const QString &buffer, QUrlInfo &info )
{
    Q3FtpDTP::parseDir( buffer, url()->user(), &info );
}

void Q3Ftp::npListInfo( const QUrlInfo & i )
{
    if ( url() ) {
	QRegExp filt( url()->nameFilter(), false, true );
	if ( i.isDir() || filt.search( i.name() ) != -1 ) {
	    emit newChild( i, operationInProgress() );
	}
    } else {
	emit newChild( i, operationInProgress() );
    }
}

void Q3Ftp::npDone( bool err )
{
    Q3FtpPrivate *d = dHelper( this );

    bool emitFinishedSignal = false;
    Q3NetworkOperation *op = operationInProgress();
    if ( op ) {
	if ( err ) {
	    op->setProtocolDetail( errorString() );
	    op->setState( StFailed );
	    if ( error() == HostNotFound ) {
		op->setErrorCode( (int)ErrHostNotFound );
	    } else {
		switch ( op->operation() ) {
		    case OpListChildren:
			op->setErrorCode( (int)ErrListChildren );
			break;
		    case OpMkDir:
			op->setErrorCode( (int)ErrMkDir );
			break;
		    case OpRemove:
			op->setErrorCode( (int)ErrRemove );
			break;
		    case OpRename:
			op->setErrorCode( (int)ErrRename );
			break;
		    case OpGet:
			op->setErrorCode( (int)ErrGet );
			break;
		    case OpPut:
			op->setErrorCode( (int)ErrPut );
			break;
		}
	    }
	    emitFinishedSignal = true;
	} else if ( !d->npWaitForLoginDone ) {
	    switch ( op->operation() ) {
		case OpRemove:
		    emit removed( op );
		    break;
		case OpMkDir:
		    {
			QUrlInfo inf( op->arg( 0 ), 0, QLatin1String(""), QLatin1String(""), 0, QDateTime(),
				QDateTime(), true, false, false, true, true, true );
			emit newChild( inf, op );
			emit createdDirectory( inf, op );
		    }
		    break;
		case OpRename:
		    emit itemChanged( operationInProgress() );
		    break;
		default:
		    break;
	    }
	    op->setState( StDone );
	    emitFinishedSignal = true;
	}
    }
    d->npWaitForLoginDone = false;

    if ( state() == Unconnected ) {
	disconnect( this, SIGNAL(listInfo(QUrlInfo)),
		    this, SLOT(npListInfo(QUrlInfo)) );
	disconnect( this, SIGNAL(done(bool)),
		    this, SLOT(npDone(bool)) );
	disconnect( this, SIGNAL(stateChanged(int)),
		    this, SLOT(npStateChanged(int)) );
	disconnect( this, SIGNAL(dataTransferProgress(int,int)),
		    this, SLOT(npDataTransferProgress(int,int)) );
	disconnect( this, SIGNAL(readyRead()),
		    this, SLOT(npReadyRead()) );
    }

    // emit the finished() signal at the very end to avoid reentrance problems
    if ( emitFinishedSignal )
	emit finished( op );
}

void Q3Ftp::npStateChanged( int state )
{
    if ( url() ) {
	if ( state == Connecting )
	    emit connectionStateChanged( ConHostFound, QFtp::tr( "Host %1 found" ).arg( url()->host() ) );
	else if ( state == Connected )
	    emit connectionStateChanged( ConConnected, QFtp::tr( "Connected to host %1" ).arg( url()->host() ) );
	else if ( state == Unconnected )
	    emit connectionStateChanged( ConClosed, QFtp::tr( "Connection to %1 closed" ).arg( url()->host() ) );
    } else {
	if ( state == Connecting )
	    emit connectionStateChanged( ConHostFound, QFtp::tr( "Host found" ) );
	else if ( state == Connected )
	    emit connectionStateChanged( ConConnected, QFtp::tr( "Connected to host" ) );
	else if ( state == Unconnected )
	    emit connectionStateChanged( ConClosed, QFtp::tr( "Connection closed" ) );
    }
}

void Q3Ftp::npDataTransferProgress( int bDone, int bTotal )
{
    emit Q3NetworkProtocol::dataTransferProgress( bDone, bTotal, operationInProgress() );
}

void Q3Ftp::npReadyRead()
{
    emit data( readAll(), operationInProgress() );
}

/*!  \internal
*/
void Q3Ftp::hostFound()
{
}
/*!  \internal
*/
void Q3Ftp::connected()
{
}
/*!  \internal
*/
void Q3Ftp::closed()
{
}
/*!  \internal
*/
void Q3Ftp::dataHostFound()
{
}
/*!  \internal
*/
void Q3Ftp::dataConnected()
{
}
/*!  \internal
*/
void Q3Ftp::dataClosed()
{
}
/*!  \internal
*/
void Q3Ftp::dataReadyRead()
{
}
/*!  \internal
*/
void Q3Ftp::dataBytesWritten( int )
{
}
/*!  \internal
*/
void Q3Ftp::error( int )
{
}

QT_END_NAMESPACE

#include "q3ftp.moc"

#endif // QT_NO_NETWORKPROTOCOL_FTP
