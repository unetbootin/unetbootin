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

#include "qlocalserver.h"
#include "qlocalserver_p.h"
#include "qlocalsocket.h"

#include <qdebug.h>
#include <qdatetime.h>
#include <qcoreapplication.h>
#include <QMetaType>

// The buffer size need to be 0 otherwise data could be
// lost if the socket that has written data closes the connection
// before it is read.  Pipewriter is used for write buffering.
#define BUFSIZE 0

QT_BEGIN_NAMESPACE

QLocalServerThread::QLocalServerThread(QObject *parent) : QThread(parent),
    maxPendingConnections(1)
{
    stopEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
    gotConnectionEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
}

QLocalServerThread::~QLocalServerThread()
{
    stop();
    closeServer();
    CloseHandle(stopEvent);
    CloseHandle(gotConnectionEvent);
}

void QLocalServerThread::stop()
{
    if (isRunning()) {
        SetEvent(stopEvent);
        wait();
    }
}

void QLocalServerThread::closeServer()
{
    while (!pendingHandles.isEmpty())
        CloseHandle(pendingHandles.dequeue());
}

QString QLocalServerThread::setName(const QString &name)
{
    QString pipePath = QLatin1String("\\\\.\\pipe\\");
    if (name.startsWith(pipePath))
        fullServerName = name;
    else
        fullServerName = pipePath + name;
    for (int i = pendingHandles.count(); i < maxPendingConnections; ++i)
        if (!makeHandle())
            break;
    return fullServerName;
}

bool QLocalServerThread::makeHandle()
{
    if (pendingHandles.count() >= maxPendingConnections)
        return false;

    HANDLE handle = INVALID_HANDLE_VALUE;
    QT_WA({
    handle = CreateNamedPipeW(
                 (TCHAR*)fullServerName.utf16(), // pipe name
                 PIPE_ACCESS_DUPLEX | FILE_FLAG_OVERLAPPED,       // read/write access
                 PIPE_TYPE_MESSAGE |       // message type pipe
                 PIPE_READMODE_MESSAGE |   // message-read mode
                 PIPE_WAIT,                // blocking mode
                 PIPE_UNLIMITED_INSTANCES, // max. instances
                 BUFSIZE,                  // output buffer size
                 BUFSIZE,                  // input buffer size
                 3000,                     // client time-out
                 NULL);
    }, {
    handle = CreateNamedPipeA(
                 fullServerName.toLocal8Bit().constData(), // pipe name
                 PIPE_ACCESS_DUPLEX | FILE_FLAG_OVERLAPPED,       // read/write access
                 PIPE_TYPE_MESSAGE |       // message type pipe
                 PIPE_READMODE_MESSAGE |   // message-read mode
                 PIPE_WAIT,                // blocking mode
                 PIPE_UNLIMITED_INSTANCES, // max. instances
                 BUFSIZE,                  // output buffer size
                 BUFSIZE,                  // input buffer size
                 3000,                     // client time-out
                 NULL);
    });

    if (INVALID_HANDLE_VALUE == handle) {
        return false;
    }
    pendingHandles.enqueue(handle);
    return true;
}

void QLocalServerThread::run()
{
    OVERLAPPED  op;
    HANDLE      handleArray[2];
    memset(&op, 0, sizeof(op));
    handleArray[0] = op.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
    handleArray[1] = stopEvent;
    HANDLE handle = INVALID_HANDLE_VALUE;

    forever {
        if (INVALID_HANDLE_VALUE == handle) {
            makeHandle();
            if (!pendingHandles.isEmpty())
                handle = pendingHandles.dequeue();
        }
        if (INVALID_HANDLE_VALUE == handle) {
            int windowsError = GetLastError();
            QString function = QLatin1String("QLocalServer::run");
            QString errorString = QLocalServer::tr("%1: Unknown error %2").arg(function).arg(windowsError);
            emit error(QAbstractSocket::UnknownSocketError, errorString);
            CloseHandle(handleArray[0]);
            SetEvent(gotConnectionEvent);
            return;
        }

        BOOL isConnected = ConnectNamedPipe(handle, &op) ? TRUE : (GetLastError() == ERROR_PIPE_CONNECTED);
        if (!isConnected) {
            switch (WaitForMultipleObjects(2, handleArray, FALSE, INFINITE))
            {
            case WAIT_OBJECT_0 + 1:
                CloseHandle(handle);
                CloseHandle(handleArray[0]);
                return;
            }
        }
        emit connected(handle);
        handle = INVALID_HANDLE_VALUE;
        ResetEvent(handleArray[0]);
        SetEvent(gotConnectionEvent);
    }
}

void QLocalServerPrivate::init()
{
    Q_Q(QLocalServer);
    qRegisterMetaType<HANDLE>("HANDLE");
    q->connect(&waitForConnection, SIGNAL(connected(HANDLE)),
               q, SLOT(_q_openSocket(HANDLE)), Qt::QueuedConnection);
    q->connect(&waitForConnection, SIGNAL(finished()),
               q, SLOT(_q_stoppedListening()), Qt::QueuedConnection);
    q->connect(&waitForConnection, SIGNAL(terminated()),
               q, SLOT(_q_stoppedListening()), Qt::QueuedConnection);
    q->connect(&waitForConnection, SIGNAL(error(QAbstractSocket::SocketError, const QString &)),
               q, SLOT(_q_setError(QAbstractSocket::SocketError, const QString &)));
}

bool QLocalServerPrivate::listen(const QString &name)
{
    fullServerName = waitForConnection.setName(name);
    serverName = name;
    waitForConnection.start();
    return true;
}

void QLocalServerPrivate::_q_setError(QAbstractSocket::SocketError e, const QString &eString)
{
    error = e;
    errorString = eString;
}

void QLocalServerPrivate::_q_stoppedListening()
{
    Q_Q(QLocalServer);
    if (!inWaitingFunction)
        q->close();
}

void QLocalServerPrivate::_q_openSocket(HANDLE handle)
{
    Q_Q(QLocalServer);
    q->incomingConnection((int)handle);
}

void QLocalServerPrivate::closeServer()
{
    waitForConnection.stop();
    waitForConnection.closeServer();
}

void QLocalServerPrivate::waitForNewConnection(int msecs, bool *timedOut)
{
    Q_Q(QLocalServer);
    if (!pendingConnections.isEmpty() || !q->isListening())
        return;

    DWORD result = WaitForSingleObject(waitForConnection.gotConnectionEvent,
                                       (msecs == -1) ? INFINITE : msecs);
    if (result == WAIT_TIMEOUT) {
        if (timedOut)
            *timedOut = true;
    } else {
        QCoreApplication::instance()->processEvents();
    }
}

QT_END_NAMESPACE
