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

#ifndef Q3HTTP_H
#define Q3HTTP_H

#include <QtCore/qobject.h>
#include <Qt3Support/q3networkprotocol.h>
#include <QtCore/qmap.h>
#include <QtCore/qstringlist.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Qt3Support)

#ifndef QT_NO_NETWORKPROTOCOL_HTTP

class Q3Socket;
class QTimerEvent;
class QTextStream;
class QIODevice;

class Q3HttpPrivate;
class Q3HttpRequest;

class Q_COMPAT_EXPORT Q3HttpHeader
{
public:
    Q3HttpHeader();
    Q3HttpHeader( const Q3HttpHeader& header );
    Q3HttpHeader( const QString& str );
    virtual ~Q3HttpHeader();

    Q3HttpHeader& operator=( const Q3HttpHeader& h );

    QString value( const QString& key ) const;
    void setValue( const QString& key, const QString& value );
    void removeValue( const QString& key );

    QStringList keys() const;
    bool hasKey( const QString& key ) const;

    bool hasContentLength() const;
    uint contentLength() const;
    void setContentLength( int len );

    bool hasContentType() const;
    QString contentType() const;
    void setContentType( const QString& type );

    virtual QString toString() const;
    bool isValid() const;

    virtual int majorVersion() const = 0;
    virtual int minorVersion() const = 0;

protected:
    virtual bool parseLine( const QString& line, int number );
    bool parse( const QString& str );
    void setValid( bool );

private:
    QMap<QString,QString> values;
    bool valid;
};

class Q_COMPAT_EXPORT Q3HttpResponseHeader : public Q3HttpHeader
{
private:
    Q3HttpResponseHeader( int code, const QString& text = QString(), int majorVer = 1, int minorVer = 1 );
    Q3HttpResponseHeader( const QString& str );

    void setStatusLine( int code, const QString& text = QString(), int majorVer = 1, int minorVer = 1 );

public:
    Q3HttpResponseHeader();
    Q3HttpResponseHeader( const Q3HttpResponseHeader& header );

    int statusCode() const;
    QString reasonPhrase() const;

    int majorVersion() const;
    int minorVersion() const;

    QString toString() const;

protected:
    bool parseLine( const QString& line, int number );

private:
    int statCode;
    QString reasonPhr;
    int majVer;
    int minVer;

    friend class Q3Http;
};

class Q_COMPAT_EXPORT Q3HttpRequestHeader : public Q3HttpHeader
{
public:
    Q3HttpRequestHeader();
    Q3HttpRequestHeader( const QString& method, const QString& path, int majorVer = 1, int minorVer = 1 );
    Q3HttpRequestHeader( const Q3HttpRequestHeader& header );
    Q3HttpRequestHeader( const QString& str );

    void setRequest( const QString& method, const QString& path, int majorVer = 1, int minorVer = 1 );

    QString method() const;
    QString path() const;

    int majorVersion() const;
    int minorVersion() const;

    QString toString() const;

protected:
    bool parseLine( const QString& line, int number );

private:
    QString m;
    QString p;
    int majVer;
    int minVer;
};

class Q_COMPAT_EXPORT Q3Http : public Q3NetworkProtocol
{
    Q_OBJECT

public:
    Q3Http();
    Q3Http( QObject* parent, const char* name = 0 ); // ### Qt 4.0: make parent=0 and get rid of the Q3Http() constructor
    Q3Http( const QString &hostname, Q_UINT16 port=80, QObject* parent=0, const char* name = 0 );
    virtual ~Q3Http();

    int supportedOperations() const;

    enum State { Unconnected, HostLookup, Connecting, Sending, Reading, Connected, Closing };
    enum Error {
	NoError,
	UnknownError,
	HostNotFound,
	ConnectionRefused,
	UnexpectedClose,
	InvalidResponseHeader,
	WrongContentLength,
	Aborted
    };

    int setHost(const QString &hostname, Q_UINT16 port=80 );

    int get( const QString& path, QIODevice* to=0 );
    int post( const QString& path, QIODevice* data, QIODevice* to=0  );
    int post( const QString& path, const QByteArray& data, QIODevice* to=0 );
    int head( const QString& path );
    int request( const Q3HttpRequestHeader &header, QIODevice *device=0, QIODevice *to=0 );
    int request( const Q3HttpRequestHeader &header, const QByteArray &data, QIODevice *to=0 );

    int closeConnection();

    Q_ULONG bytesAvailable() const;
    Q_LONG readBlock( char *data, Q_ULONG maxlen );
    QByteArray readAll();

    int currentId() const;
    QIODevice* currentSourceDevice() const;
    QIODevice* currentDestinationDevice() const;
    Q3HttpRequestHeader currentRequest() const;
    bool hasPendingRequests() const;
    void clearPendingRequests();

    State state() const;

    Error error() const;
    QString errorString() const;

public Q_SLOTS:
    void abort();

Q_SIGNALS:
    void stateChanged( int );
    void responseHeaderReceived( const Q3HttpResponseHeader& resp );
    void readyRead( const Q3HttpResponseHeader& resp );
    void dataSendProgress( int, int );
    void dataReadProgress( int, int );

    void requestStarted( int );
    void requestFinished( int, bool );
    void done( bool );

protected:
    void operationGet( Q3NetworkOperation *op );
    void operationPut( Q3NetworkOperation *op );

    void timerEvent( QTimerEvent * );

private Q_SLOTS:
    void clientReply( const Q3HttpResponseHeader &rep );
    void clientDone( bool );
    void clientStateChanged( int );

    void startNextRequest();
    void slotReadyRead();
    void slotConnected();
    void slotError( int );
    void slotClosed();
    void slotBytesWritten( int );

private:
    Q3HttpPrivate *d;
    void *unused; // ### Qt 4.0: remove this (in for binary compatibility)
    int bytesRead;

    int addRequest( Q3HttpRequest * );
    void sendRequest();
    void finishedWithSuccess();
    void finishedWithError( const QString& detail, int errorCode );

    void killIdleTimer();

    void init();
    void setState( int );
    void close();

    friend class Q3HttpNormalRequest;
    friend class Q3HttpSetHostRequest;
    friend class Q3HttpCloseRequest;
    friend class Q3HttpPGHRequest;
};

#endif // QT_NO_NETWORKPROTOCOL_HTTP

QT_END_NAMESPACE

QT_END_HEADER

#endif // Q3HTTP_H
