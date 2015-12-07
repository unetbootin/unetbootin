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

#ifndef QLOCALSERVER_P_H
#define QLOCALSERVER_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists for the convenience
// of the QLocalServer class.  This header file may change from
// version to version without notice, or even be removed.
//
// We mean it.
//

#ifndef QT_NO_LOCALSERVER

#include "qlocalserver.h"
#include "private/qobject_p.h"
#include <qqueue.h>

#if defined(QT_LOCALSOCKET_TCP)
#   include <qtcpserver.h>
#elif defined(Q_OS_WIN)
#   include <qt_windows.h>
#   include <qthread.h>
#else
#   include <private/qnativesocketengine_p.h>
#   include <qsocketnotifier.h>
#endif

QT_BEGIN_NAMESPACE

#if defined(Q_OS_WIN) && !defined(QT_LOCALSOCKET_TCP)

/*!
    \internal
    QLocalServerThread exists because Windows does not have a
    way to provide notifications when there is a new connections to
    the server.
  */
class QLocalServerThread : public QThread
{
    Q_OBJECT

Q_SIGNALS:
    void connected(HANDLE newSocket);
    void error(QAbstractSocket::SocketError error, const QString &errorString);

public:
    QLocalServerThread(QObject *parent = 0);
    ~QLocalServerThread();
    void closeServer();

public:
    QString setName(const QString &name);
    void run();
    void stop();
    bool makeHandle();

    HANDLE gotConnectionEvent;
    QQueue<HANDLE> pendingHandles;
    int maxPendingConnections;
private:
    HANDLE stopEvent;
    QString fullServerName;
};

#endif

class QLocalServerPrivate : public QObjectPrivate
{
    Q_DECLARE_PUBLIC(QLocalServer)

public:
    QLocalServerPrivate() :
#if defined(Q_OS_WIN) && !defined(QT_LOCALSOCKET_TCP)
            inWaitingFunction(false),
#elif !defined(QT_LOCALSOCKET_TCP)
            listenSocket(-1), socketNotifier(0),
#endif
            maxPendingConnections(30), error(QAbstractSocket::UnknownSocketError)
    {
    }

    void init();
    bool listen(const QString &name);
    static bool removeServer(const QString &name);
    void closeServer();
    void waitForNewConnection(int msec, bool *timedOut);

#if defined(QT_LOCALSOCKET_TCP)
    void _q_onNewConnection();

    QTcpServer tcpServer;
    QMap<quintptr, QTcpSocket*> socketMap;
#elif defined(Q_OS_WIN)
    void _q_openSocket(HANDLE socket);
    void _q_stoppedListening();
    void _q_setError(QAbstractSocket::SocketError error, const QString &errorString);

    QLocalServerThread waitForConnection;
    bool inWaitingFunction;
#else
    void setError(const QString &function);
    void _q_socketActivated();

    int listenSocket;
    QSocketNotifier *socketNotifier;
#endif

    QString serverName;
    QString fullServerName;
    int maxPendingConnections;
    QQueue<QLocalSocket*> pendingConnections;
    QString errorString;
    QAbstractSocket::SocketError error;
};

QT_END_NAMESPACE

#endif // QT_NO_LOCALSERVER

#endif // QLOCALSERVER_P_H

