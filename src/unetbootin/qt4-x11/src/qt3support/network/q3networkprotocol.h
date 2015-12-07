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

#ifndef Q3NETWORKPROTOCOL_H
#define Q3NETWORKPROTOCOL_H

#include <QtCore/qstring.h>
#include <QtCore/qobject.h>
#include <Qt3Support/q3dict.h>
#include <Qt3Support/q3valuelist.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Qt3SupportLight)

#ifndef QT_NO_NETWORKPROTOCOL

class Q3NetworkProtocol;
class Q3NetworkOperation;
class QTimer;
class Q3UrlOperator;
class Q3NetworkProtocolPrivate;
class QUrlInfo;
template <class T> class Q3ValueList;

class Q_COMPAT_EXPORT Q3NetworkProtocolFactoryBase
{
public:
    virtual ~Q3NetworkProtocolFactoryBase() {}
    virtual Q3NetworkProtocol *createObject() = 0;
};

template< class T >
class Q3NetworkProtocolFactory : public Q3NetworkProtocolFactoryBase
{
public:
    Q3NetworkProtocol *createObject() {
	return new T;
    }

};

typedef Q3Dict< Q3NetworkProtocolFactoryBase > Q3NetworkProtocolDict;

class Q_COMPAT_EXPORT Q3NetworkProtocol : public QObject
{
    Q_OBJECT

public:
    enum State {
	StWaiting = 0,
	StInProgress,
	StDone,
	StFailed,
	StStopped
    };

    enum Operation {
	OpListChildren = 1,
	OpMkDir = 2,
	OpMkdir = OpMkDir, // ### remove in 4.0
	OpRemove = 4,
	OpRename = 8,
	OpGet = 32,
	OpPut = 64
    };

    enum ConnectionState {
	ConHostFound,
	ConConnected,
	ConClosed
    };

    enum Error {
	// no error
	NoError = 0,
	// general errors
	ErrValid,
	ErrUnknownProtocol,
	ErrUnsupported,
	ErrParse,
	// errors on connect
	ErrLoginIncorrect,
	ErrHostNotFound,
	// protocol errors
	ErrListChildren,
	ErrListChlidren = ErrListChildren, // ### remove in 4.0
	ErrMkDir,
	ErrMkdir = ErrMkDir, // ### remove in 4.0
	ErrRemove,
	ErrRename,
	ErrGet,
	ErrPut,
	ErrFileNotExisting,
	ErrPermissionDenied
    };

    Q3NetworkProtocol();
    virtual ~Q3NetworkProtocol();

    virtual void setUrl( Q3UrlOperator *u );

    virtual void setAutoDelete( bool b, int i = 10000 );
    bool autoDelete() const;

    static void registerNetworkProtocol( const QString &protocol,
					 Q3NetworkProtocolFactoryBase *protocolFactory );
    static Q3NetworkProtocol *getNetworkProtocol( const QString &protocol );
    static bool hasOnlyLocalFileSystem();

    virtual int supportedOperations() const;
    virtual void addOperation( Q3NetworkOperation *op );

    Q3UrlOperator *url() const;
    Q3NetworkOperation *operationInProgress() const;
    virtual void clearOperationQueue();
    virtual void stop();

Q_SIGNALS:
    void data( const QByteArray &, Q3NetworkOperation *res );
    void connectionStateChanged( int state, const QString &data );
    void finished( Q3NetworkOperation *res );
    void start( Q3NetworkOperation *res );
    void newChildren( const Q3ValueList<QUrlInfo> &, Q3NetworkOperation *res );
    void newChild( const QUrlInfo &, Q3NetworkOperation *res );
    void createdDirectory( const QUrlInfo &, Q3NetworkOperation *res );
    void removed( Q3NetworkOperation *res );
    void itemChanged( Q3NetworkOperation *res );
    void dataTransferProgress( int bytesDone, int bytesTotal, Q3NetworkOperation *res );

protected:
    virtual void processOperation( Q3NetworkOperation *op );
    virtual void operationListChildren( Q3NetworkOperation *op );
    virtual void operationMkDir( Q3NetworkOperation *op );
    virtual void operationRemove( Q3NetworkOperation *op );
    virtual void operationRename( Q3NetworkOperation *op );
    virtual void operationGet( Q3NetworkOperation *op );
    virtual void operationPut( Q3NetworkOperation *op );
    virtual void operationPutChunk( Q3NetworkOperation *op );
    virtual bool checkConnection( Q3NetworkOperation *op );

private:
    Q3NetworkProtocolPrivate *d;

private Q_SLOTS:
    void processNextOperation( Q3NetworkOperation *old );
    void startOps();
    void emitNewChildren( const QUrlInfo &i, Q3NetworkOperation *op );

    void removeMe();

private:	// Disabled copy constructor and operator=
#if defined(Q_DISABLE_COPY)
    Q3NetworkProtocol( const Q3NetworkProtocol & );
    Q3NetworkProtocol &operator=( const Q3NetworkProtocol & );
#endif
};

class Q3NetworkOperationPrivate;

class Q_COMPAT_EXPORT Q3NetworkOperation : public QObject
{
    Q_OBJECT
    friend class Q3UrlOperator;

public:
    Q3NetworkOperation( Q3NetworkProtocol::Operation operation,
		    const QString &arg0, const QString &arg1,
		    const QString &arg2 );
    Q3NetworkOperation( Q3NetworkProtocol::Operation operation,
		    const QByteArray &arg0, const QByteArray &arg1,
		    const QByteArray &arg2 );
    ~Q3NetworkOperation();

    void setState( Q3NetworkProtocol::State state );
    void setProtocolDetail( const QString &detail );
    void setErrorCode( int ec );
    void setArg( int num, const QString &arg );
    void setRawArg( int num, const QByteArray &arg );

    Q3NetworkProtocol::Operation operation() const;
    Q3NetworkProtocol::State state() const;
    QString arg( int num ) const;
    QByteArray rawArg( int num ) const;
    QString protocolDetail() const;
    int errorCode() const;

    void free();

private Q_SLOTS:
    void deleteMe();

private:
    QByteArray &raw( int num ) const;
    Q3NetworkOperationPrivate *d;

private:	// Disabled copy constructor and operator=
#if defined(Q_DISABLE_COPY)
    Q3NetworkOperation( const Q3NetworkOperation & );
    Q3NetworkOperation &operator=( const Q3NetworkOperation & );
#endif
};

#endif // QT_NO_NETWORKPROTOCOL

QT_END_NAMESPACE

QT_END_HEADER

#endif // Q3NETWORKPROTOCOL_H
