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

#ifdef Q_OS_WIN
#   include <qt_windows.h>
#   include <qthread.h>
#else
#   include <private/qnativesocketengine_p.h>
#   include <qsocketnotifier.h>
#endif

QT_BEGIN_NAMESPACE

#ifdef Q_OS_WIN

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
#ifdef Q_OS_WIN
            inWaitingFunction(false),
#else
            listenSocket(-1), socketNotifier(0),
#endif
            maxPendingConnections(30), error(QAbstractSocket::UnknownSocketError)
    {
    }

    void init();
    bool listen(const QString &name);
    void closeServer();
    void waitForNewConnection(int msec, bool *timedOut);

#ifdef Q_OS_WIN
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

