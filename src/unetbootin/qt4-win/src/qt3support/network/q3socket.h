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

#ifndef Q3SOCKET_H
#define Q3SOCKET_H

#include <QtCore/qiodevice.h>
#include <QtNetwork/qhostaddress.h> // int->QHostAddress conversion

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Qt3Support)

class Q3SocketPrivate;
class Q3SocketDevice;

class Q_COMPAT_EXPORT Q3Socket : public QIODevice
{
    Q_OBJECT
public:
    enum Error {
	ErrConnectionRefused,
	ErrHostNotFound,
	ErrSocketRead
    };

    Q3Socket( QObject *parent=0, const char *name=0 );
    virtual ~Q3Socket();

    enum State { Idle, HostLookup, Connecting,
		 Connected, Closing,
		 Connection=Connected };
    State	 state() const;

    int		 socket() const;
    virtual void setSocket( int );

    Q3SocketDevice *socketDevice();
    virtual void setSocketDevice( Q3SocketDevice * );

#ifndef QT_NO_DNS
    virtual void connectToHost( const QString &host, Q_UINT16 port );
#endif
    QString	 peerName() const;

    // Implementation of QIODevice abstract virtual functions
    bool	 open( OpenMode mode );
    bool      open(int mode) { return open((OpenMode)mode); }
    void	 close();
    bool	 flush();
    Offset	 size() const;
    Offset	 at() const;
    bool	 at( Offset );
    bool	 atEnd() const;

    qint64	 bytesAvailable() const;
    Q_ULONG	 waitForMore( int msecs, bool *timeout  ) const;
    Q_ULONG	 waitForMore( int msecs ) const; // ### Qt 4.0: merge the two overloads
    qint64	 bytesToWrite() const;
    void	 clearPendingData();

    int		 getch();
    int		 putch( int );
    int		 ungetch(int);

    bool	 canReadLine() const;

    Q_UINT16	 port() const;
    Q_UINT16	 peerPort() const;
    QHostAddress address() const;
    QHostAddress peerAddress() const;

    void	 setReadBufferSize( Q_ULONG );
    Q_ULONG	 readBufferSize() const;

    inline bool  isSequential() const { return true; }

Q_SIGNALS:
    void	 hostFound();
    void	 connected();
    void	 connectionClosed();
    void	 delayedCloseFinished();
    void	 readyRead();
    void	 bytesWritten( int nbytes );
    void	 error( int );

protected Q_SLOTS:
    virtual void sn_read( bool force=false );
    virtual void sn_write();

protected:
    qint64 readData(char *data, qint64 maxlen);
    qint64 writeData(const char *data, qint64 len);

private Q_SLOTS:
    void	tryConnecting();
    void	emitErrorConnectionRefused();

private:
    Q3SocketPrivate *d;

    bool	 consumeWriteBuf( Q_ULONG nbytes );
    void	 tryConnection();
    void         setSocketIntern( int socket );

private:	// Disabled copy constructor and operator=
#if defined(Q_DISABLE_COPY)
    Q3Socket( const Q3Socket & );
    Q3Socket &operator=( const Q3Socket & );
#endif
};

QT_END_NAMESPACE

QT_END_HEADER

#endif // Q3SOCKET_H
