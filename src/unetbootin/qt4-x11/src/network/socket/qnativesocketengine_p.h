/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the QtNetwork module of the Qt Toolkit.
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

#ifndef QNATIVESOCKETENGINE_P_H
#define QNATIVESOCKETENGINE_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists for the convenience
// of the QLibrary class.  This header file may change from
// version to version without notice, or even be removed.
//
// We mean it.
//

#include "QtNetwork/qhostaddress.h"
#include "private/qabstractsocketengine_p.h"
#ifndef Q_OS_WIN
#include "qplatformdefs.h"
#endif

QT_BEGIN_NAMESPACE

#ifndef Q_OS_WIN
// Almost always the same. If not, specify in qplatformdefs.h.
#if !defined(QT_SOCKOPTLEN_T)
# define QT_SOCKOPTLEN_T QT_SOCKLEN_T
#endif

// Tru64 redefines accept -> _accept with _XOPEN_SOURCE_EXTENDED
static inline int qt_socket_accept(int s, struct sockaddr *addr, QT_SOCKLEN_T *addrlen)
{ return ::accept(s, addr, static_cast<QT_SOCKLEN_T *>(addrlen)); }
#if defined(accept)
# undef accept
#endif

// UnixWare 7 redefines listen -> _listen
static inline int qt_socket_listen(int s, int backlog)
{ return ::listen(s, backlog); }
#if defined(listen)
# undef listen
#endif

// UnixWare 7 redefines socket -> _socket
static inline int qt_socket_socket(int domain, int type, int protocol)
{ return ::socket(domain, type, protocol); }
#if defined(socket)
# undef socket
#endif

#endif

class QNativeSocketEnginePrivate;

class Q_AUTOTEST_EXPORT QNativeSocketEngine : public QAbstractSocketEngine
{
    Q_OBJECT
public:
    QNativeSocketEngine(QObject *parent = 0);
    ~QNativeSocketEngine();

    bool initialize(QAbstractSocket::SocketType type, QAbstractSocket::NetworkLayerProtocol protocol = QAbstractSocket::IPv4Protocol);
    bool initialize(int socketDescriptor, QAbstractSocket::SocketState socketState = QAbstractSocket::ConnectedState);

    int socketDescriptor() const;

    bool isValid() const;

    bool connectToHost(const QHostAddress &address, quint16 port);
    bool connectToHostByName(const QString &name, quint16 port);
    bool bind(const QHostAddress &address, quint16 port);
    bool listen();
    int accept();
    void close();

    qint64 bytesAvailable() const;

    qint64 read(char *data, qint64 maxlen);
    qint64 write(const char *data, qint64 len);

    qint64 readDatagram(char *data, qint64 maxlen, QHostAddress *addr = 0,
                            quint16 *port = 0);
    qint64 writeDatagram(const char *data, qint64 len, const QHostAddress &addr,
                             quint16 port);
    bool hasPendingDatagrams() const;
    qint64 pendingDatagramSize() const;

    qint64 receiveBufferSize() const;
    void setReceiveBufferSize(qint64 bufferSize);

    qint64 sendBufferSize() const;
    void setSendBufferSize(qint64 bufferSize);

    int option(SocketOption option) const;
    bool setOption(SocketOption option, int value);

    bool waitForRead(int msecs = 30000, bool *timedOut = 0);
    bool waitForWrite(int msecs = 30000, bool *timedOut = 0);
    bool waitForReadOrWrite(bool *readyToRead, bool *readyToWrite,
			    bool checkRead, bool checkWrite,
			    int msecs = 30000, bool *timedOut = 0);

    bool isReadNotificationEnabled() const;
    void setReadNotificationEnabled(bool enable);
    bool isWriteNotificationEnabled() const;
    void setWriteNotificationEnabled(bool enable);
    bool isExceptionNotificationEnabled() const;
    void setExceptionNotificationEnabled(bool enable);

public Q_SLOTS:
    // non-virtual override;
    void connectionNotification();

private:
    Q_DECLARE_PRIVATE(QNativeSocketEngine)
    Q_DISABLE_COPY(QNativeSocketEngine)
};

#ifdef Q_OS_WIN
class QWindowsSockInit
{
public:
    QWindowsSockInit();
    ~QWindowsSockInit();
    int version;
};
#endif

class QSocketNotifier;

class QNativeSocketEnginePrivate : public QAbstractSocketEnginePrivate
{
    Q_DECLARE_PUBLIC(QNativeSocketEngine)
public:
    QNativeSocketEnginePrivate();
    ~QNativeSocketEnginePrivate();

    int socketDescriptor;

    QSocketNotifier *readNotifier, *writeNotifier, *exceptNotifier;

#ifdef Q_OS_WIN
    QWindowsSockInit winSock;
#endif

    enum ErrorString {
        NonBlockingInitFailedErrorString,
        BroadcastingInitFailedErrorString,
        NoIpV6ErrorString,
        RemoteHostClosedErrorString,
        TimeOutErrorString,
        ResourceErrorString,
        OperationUnsupportedErrorString,
        ProtocolUnsupportedErrorString,
        InvalidSocketErrorString,
        HostUnreachableErrorString,
        NetworkUnreachableErrorString,
        AccessErrorString,
        ConnectionTimeOutErrorString,
        ConnectionRefusedErrorString,
        AddressInuseErrorString,
        AddressNotAvailableErrorString,
        AddressProtectedErrorString,
        DatagramTooLargeErrorString,
        SendDatagramErrorString,
        ReceiveDatagramErrorString,
        WriteErrorString,
        ReadErrorString,
        PortInuseErrorString,
        NotSocketErrorString,
        InvalidProxyTypeString,

        UnknownSocketErrorString = -1
    };

    void setError(QAbstractSocket::SocketError error, ErrorString errorString) const;

    // native functions
    int option(QNativeSocketEngine::SocketOption option) const;
    bool setOption(QNativeSocketEngine::SocketOption option, int value);

    bool createNewSocket(QAbstractSocket::SocketType type, QAbstractSocket::NetworkLayerProtocol protocol);

    bool nativeConnect(const QHostAddress &address, quint16 port);
    bool nativeBind(const QHostAddress &address, quint16 port);
    bool nativeListen(int backlog);
    int nativeAccept();
    qint64 nativeBytesAvailable() const;

    bool nativeHasPendingDatagrams() const;
    qint64 nativePendingDatagramSize() const;
    qint64 nativeReceiveDatagram(char *data, qint64 maxLength,
                                     QHostAddress *address, quint16 *port);
    qint64 nativeSendDatagram(const char *data, qint64 length,
                                  const QHostAddress &host, quint16 port);
    qint64 nativeRead(char *data, qint64 maxLength);
    qint64 nativeWrite(const char *data, qint64 length);
    int nativeSelect(int timeout, bool selectForRead) const;
    int nativeSelect(int timeout, bool checkRead, bool checkWrite,
		     bool *selectForRead, bool *selectForWrite) const;

    void nativeClose();

    bool checkProxy(const QHostAddress &address);
    bool fetchConnectionParameters();
};

QT_END_NAMESPACE

#endif // QNATIVESOCKETENGINE_P_H
