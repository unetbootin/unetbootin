/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the QtNetwork module of the Qt Toolkit.
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

#ifdef Q_OS_WIN
#   include "private/qwindowspipewriter_p.h"
#   include "private/qringbuffer_p.h"
#else
#   include "private/qnativesocketengine_p.h"
#   include <qtcpsocket.h>
#   include <qsocketnotifier.h>
#   include <errno.h>
#endif

QT_BEGIN_NAMESPACE

#ifndef Q_OS_WIN
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
#endif //#ifndef Q_OS_WIN

class QLocalSocketPrivate : public QIODevicePrivate
{
    Q_DECLARE_PUBLIC(QLocalSocket)

public:
    QLocalSocketPrivate();
    void init();

#ifdef Q_OS_WIN
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

