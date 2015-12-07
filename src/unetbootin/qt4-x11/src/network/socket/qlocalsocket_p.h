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

#ifndef QLOCALSOCKET_P_H
#define QLOCALSOCKET_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists for the convenience
// of the QLocalSocket class.  This header file may change from
// version to version without notice, or even be removed.
//
// We mean it.
//

#ifndef QT_NO_LOCALSOCKET

#include "qlocalsocket.h"
#include "private/qiodevice_p.h"

#include <qtimer.h>

#if defined(QT_LOCALSOCKET_TCP)
#   include "qtcpsocket.h"
#elif defined(Q_OS_WIN)
#   include "private/qwindowspipewriter_p.h"
#   include "private/qringbuffer_p.h"
#else
#   include "private/qnativesocketengine_p.h"
#   include <qtcpsocket.h>
#   include <qsocketnotifier.h>
#   include <errno.h>
#endif

QT_BEGIN_NAMESPACE

#if !defined(Q_OS_WIN) && !defined(QT_LOCALSOCKET_TCP)
static inline int qSocket(int af, int socketype, int proto)
{
    int ret;
    while((ret = qt_socket_socket(af, socketype, proto)) == -1 && errno == EINTR){}
    return ret;
}

static inline int qBind(int fd, const sockaddr *sa, int len)
{
    int ret;
    while((ret = QT_SOCKET_BIND(fd, (sockaddr*)sa, len)) == -1 && errno == EINTR){}
    return ret;
}

static inline int qConnect(int fd, const sockaddr *sa, int len)
{
    int ret;
    while((ret = QT_SOCKET_CONNECT(fd, (sockaddr*)sa, len)) == -1 && errno == EINTR){}
    return ret;
}

static inline int qListen(int fd, int backlog)
{
    int ret;
    while((ret = qt_socket_listen(fd, backlog)) == -1 && errno == EINTR){}
    return ret;
}

static inline int qAccept(int fd, struct sockaddr *addr, QT_SOCKLEN_T *addrlen)
{
    int ret;
    while((ret = qt_socket_accept(fd, addr, addrlen)) == -1 && errno == EINTR){}
    return ret;
}
#endif //#if !defined(Q_OS_WIN) && !defined(QT_LOCALSOCKET_TCP)

#if !defined(Q_OS_WIN) || defined(QT_LOCALSOCKET_TCP)
class QLocalUnixSocket : public QTcpSocket
{

public:
    QLocalUnixSocket() : QTcpSocket()
    {
    };

    inline void setSocketState(QAbstractSocket::SocketState state)
    {
        QTcpSocket::setSocketState(state);
    };

    inline void setErrorString(const QString &string)
    {
        QTcpSocket::setErrorString(string);
    }

    inline void setSocketError(QAbstractSocket::SocketError error)
    {
        QTcpSocket::setSocketError(error);
    }

    inline qint64 readData(char *data, qint64 maxSize)
    {
        return QTcpSocket::readData(data, maxSize);
    }

    inline qint64 writeData(const char *data, qint64 maxSize)
    {
        return QTcpSocket::writeData(data, maxSize);
    }
};
#endif //#if !defined(Q_OS_WIN) || defined(QT_LOCALSOCKET_TCP)

class QLocalSocketPrivate : public QIODevicePrivate
{
    Q_DECLARE_PUBLIC(QLocalSocket)

public:
    QLocalSocketPrivate();
    void init();

#if defined(QT_LOCALSOCKET_TCP)
    QLocalUnixSocket* tcpSocket;
    bool ownsTcpSocket;
    void setSocket(QLocalUnixSocket*);
    QString generateErrorString(QLocalSocket::LocalSocketError, const QString &function) const;
    void errorOccurred(QLocalSocket::LocalSocketError, const QString &function);
    void _q_stateChanged(QAbstractSocket::SocketState newState);
    void _q_error(QAbstractSocket::SocketError newError);
#elif defined(Q_OS_WIN)
    ~QLocalSocketPrivate() {
	CloseHandle(overlapped.hEvent);
    }

    void setErrorString(const QString &function);
    void _q_notified();
    void _q_canWrite();
    void _q_pipeClosed();
    qint64 readData(char *data, qint64 maxSize);
    qint64 bytesAvailable();
    bool readFromSocket();
    HANDLE handle;
    OVERLAPPED overlapped;
    QWindowsPipeWriter *pipeWriter;
    qint64 readBufferMaxSize;
    QRingBuffer readBuffer;
    QTimer dataNotifier;
    QLocalSocket::LocalSocketError error;
    bool readyReadEmitted;
    bool pipeClosed;
#else
    QLocalUnixSocket unixSocket;
    QString generateErrorString(QLocalSocket::LocalSocketError, const QString &function) const;
    void errorOccurred(QLocalSocket::LocalSocketError, const QString &function);
    void _q_stateChanged(QAbstractSocket::SocketState newState);
    void _q_error(QAbstractSocket::SocketError newError);
    void _q_connectToSocket();
    void _q_abortConnectionAttempt();
    QSocketNotifier *delayConnect;
    QTimer *connectTimer;
    int connectingSocket;
    QString connectingName;
    QIODevice::OpenMode connectingOpenMode;
#endif

    QString serverName;
    QString fullServerName;
    QLocalSocket::LocalSocketState state;
};

QT_END_NAMESPACE

#endif // QT_NO_LOCALSOCKET

#endif // QLOCALSOCKET_P_H

