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

#ifndef Q3SOCKETDEVICE_H
#define Q3SOCKETDEVICE_H

#include <QtCore/qiodevice.h>
#include <QtNetwork/qhostaddress.h> // int->QHostAddress conversion

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Qt3Support)

#ifndef QT_NO_NETWORK

class Q3SocketDevicePrivate;

class Q_COMPAT_EXPORT Q3SocketDevice: public QIODevice
{
public:
    enum Type { Stream, Datagram };
    enum Protocol { IPv4, IPv6, Unknown };

    Q3SocketDevice( Type type = Stream );
    Q3SocketDevice( Type type, Protocol protocol, int dummy );
    Q3SocketDevice( int socket, Type type );
    virtual ~Q3SocketDevice();

    bool	 isValid() const;
    Type	 type() const;
    Protocol	 protocol() const;

    int		 socket() const;
    virtual void setSocket( int socket, Type type );

    bool	 open( OpenMode mode );
    bool	 open( int mode ) { return open((OpenMode)mode); }
    void	 close();
    bool	 flush();

    // Implementation of QIODevice abstract virtual functions
    Offset	 size() const;
    Offset	 at() const;
    bool	 at( Offset );
    bool	 atEnd() const;

    bool	 blocking() const;
    virtual void setBlocking( bool );

    bool	 addressReusable() const;
    virtual void setAddressReusable( bool );

    int		 receiveBufferSize() const;
    virtual void setReceiveBufferSize( uint );
    int		 sendBufferSize() const;
    virtual void setSendBufferSize( uint );

    virtual bool connect( const QHostAddress &, Q_UINT16 );

    virtual bool bind( const QHostAddress &, Q_UINT16 );
    virtual bool listen( int backlog );
    virtual int	 accept();

    qint64	 bytesAvailable() const;
    Q_LONG	 waitForMore( int msecs, bool *timeout=0 ) const;
    virtual Q_LONG  writeBlock( const char *data, Q_ULONG len,
			    const QHostAddress & host, Q_UINT16 port );
    inline Q_LONG writeBlock(const char *data, Q_ULONG len)
        { return qint64(write(data, qint64(len))); }
    inline qint64 readBlock(char *data, Q_ULONG maxlen)
        { return qint64(read(data, qint64(maxlen))); }

    Q_UINT16	 port() const;
    Q_UINT16	 peerPort() const;
    QHostAddress address() const;
    QHostAddress peerAddress() const;

    enum Error {
	NoError,
	AlreadyBound,
	Inaccessible,
	NoResources,
	InternalError,
	Bug = InternalError, // ### remove in 4.0?
	Impossible,
	NoFiles,
	ConnectionRefused,
	NetworkFailure,
	UnknownError
    };
    Error	 error() const;

    inline bool isSequential() const { return true; }

protected:
    void setError( Error err );
    qint64 readData(char *data, qint64 maxlen);
    qint64 writeData(const char *data, qint64 len);

private:
    int fd;
    Type t;
    Q_UINT16 p;
    QHostAddress a;
    Q_UINT16 pp;
    QHostAddress pa;
    Q3SocketDevice::Error e;
    Q3SocketDevicePrivate * d;

    enum Option { Broadcast, ReceiveBuffer, ReuseAddress, SendBuffer };

    int		 option( Option ) const;
    virtual void setOption( Option, int );

    void	 fetchConnectionParameters();
#if defined(Q_OS_WIN32) || defined(Q_OS_WINCE)
    void	 fetchPeerConnectionParameters();
#endif

    static void  init();
    int		 createNewSocket();
    Protocol	 getProtocol() const;

private:	// Disabled copy constructor and operator=
#if defined(Q_DISABLE_COPY)
    Q3SocketDevice( const Q3SocketDevice & );
    Q3SocketDevice &operator=( const Q3SocketDevice & );
#endif
};

#endif // QT_NO_NETWORK

QT_END_NAMESPACE

QT_END_HEADER

#endif // Q3SOCKETDEVICE_H
