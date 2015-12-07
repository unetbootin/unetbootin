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

#ifndef Q3FTP_H
#define Q3FTP_H

#include <QtCore/qstring.h> // char*->QString conversion
#include <QtNetwork/qurlinfo.h>
#include <Qt3Support/q3networkprotocol.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Qt3Support)

#ifndef QT_NO_NETWORKPROTOCOL_FTP

class Q3Socket;
class Q3FtpCommand;

class Q_COMPAT_EXPORT Q3Ftp : public Q3NetworkProtocol
{
    Q_OBJECT

public:
    Q3Ftp(); // ### Qt 4.0: get rid of this overload
    Q3Ftp( QObject *parent, const char *name=0 );
    virtual ~Q3Ftp();

    int supportedOperations() const;

    // non-Q3NetworkProtocol functions:
    enum State {
	Unconnected,
	HostLookup,
	Connecting,
	Connected,
	LoggedIn,
	Closing
    };
    enum Error {
	NoError,
	UnknownError,
	HostNotFound,
	ConnectionRefused,
	NotConnected
    };
    enum Command {
	None,
	ConnectToHost,
	Login,
	Close,
	List,
	Cd,
	Get,
	Put,
	Remove,
	Mkdir,
	Rmdir,
	Rename,
	RawCommand
    };

    int connectToHost( const QString &host, Q_UINT16 port=21 );
    int login( const QString &user=QString(), const QString &password=QString() );
    int close();
    int list( const QString &dir=QString() );
    int cd( const QString &dir );
    int get( const QString &file, QIODevice *dev=0 );
    int put( const QByteArray &data, const QString &file );
    int put( QIODevice *dev, const QString &file );
    int remove( const QString &file );
    int mkdir( const QString &dir );
    int rmdir( const QString &dir );
    int rename( const QString &oldname, const QString &newname );

    int rawCommand( const QString &command );

    Q_ULONG bytesAvailable() const;
    Q_LONG readBlock( char *data, Q_ULONG maxlen );
    QByteArray readAll();

    int currentId() const;
    QIODevice* currentDevice() const;
    Command currentCommand() const;
    bool hasPendingCommands() const;
    void clearPendingCommands();

    State state() const;

    Error error() const;
    QString errorString() const;

public Q_SLOTS:
    void abort();

Q_SIGNALS:
    void stateChanged( int );
    void listInfo( const QUrlInfo& );
    void readyRead();
    void dataTransferProgress( int, int );
    void rawCommandReply( int, const QString& );

    void commandStarted( int );
    void commandFinished( int, bool );
    void done( bool );

protected:
    void parseDir( const QString &buffer, QUrlInfo &info ); // ### Qt 4.0: delete this? (not public API)
    void operationListChildren( Q3NetworkOperation *op );
    void operationMkDir( Q3NetworkOperation *op );
    void operationRemove( Q3NetworkOperation *op );
    void operationRename( Q3NetworkOperation *op );
    void operationGet( Q3NetworkOperation *op );
    void operationPut( Q3NetworkOperation *op );

    // ### Qt 4.0: delete these
    // unused variables:
    Q3Socket *commandSocket, *dataSocket;
    bool connectionReady, passiveMode;
    int getTotalSize, getDoneSize;
    bool startGetOnFail;
    int putToWrite, putWritten;
    bool errorInListChildren;

private:
    void init();
    int addCommand( Q3FtpCommand * );

    bool checkConnection( Q3NetworkOperation *op );

private Q_SLOTS:
    void startNextCommand();
    void piFinished( const QString& );
    void piError( int, const QString& );
    void piConnectState( int );
    void piFtpReply( int, const QString& );

private Q_SLOTS:
    void npListInfo( const QUrlInfo & );
    void npDone( bool );
    void npStateChanged( int );
    void npDataTransferProgress( int, int );
    void npReadyRead();

protected Q_SLOTS:
    // ### Qt 4.0: delete these
    void hostFound();
    void connected();
    void closed();
    void dataHostFound();
    void dataConnected();
    void dataClosed();
    void dataReadyRead();
    void dataBytesWritten( int nbytes );
    void error( int );
};

#endif // QT_NO_NETWORKPROTOCOL_FTP

QT_END_NAMESPACE

QT_END_HEADER

#endif // Q3FTP_H
