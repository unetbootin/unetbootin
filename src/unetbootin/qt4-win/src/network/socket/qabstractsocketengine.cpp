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

#include "qabstractsocketengine_p.h"
#include "qnativesocketengine_p.h"
#include "qmutex.h"

QT_BEGIN_NAMESPACE

class QSocketEngineHandlerList : public QList<QSocketEngineHandler*>
{
public:
    QMutex mutex;
};

Q_GLOBAL_STATIC(QSocketEngineHandlerList, socketHandlers)

QSocketEngineHandler::QSocketEngineHandler()
{
    if (!socketHandlers())
        return;
    QMutexLocker locker(&socketHandlers()->mutex);
    socketHandlers()->prepend(this);
}

QSocketEngineHandler::~QSocketEngineHandler()
{
    if (!socketHandlers())
        return;
    QMutexLocker locker(&socketHandlers()->mutex);
    socketHandlers()->removeAll(this);
}

QAbstractSocketEnginePrivate::QAbstractSocketEnginePrivate()
    : socketError(QAbstractSocket::UnknownSocketError)
    , hasSetSocketError(false)
    , socketErrorString(QLatin1String(QT_TRANSLATE_NOOP(QSocketLayer, "Unknown error")))
    , socketState(QAbstractSocket::UnconnectedState)
    , socketType(QAbstractSocket::UnknownSocketType)
    , socketProtocol(QAbstractSocket::UnknownNetworkLayerProtocol)
    , localPort(0)
    , peerPort(0)
    , receiver(0)
{
}

QAbstractSocketEngine::QAbstractSocketEngine(QObject *parent)
    : QObject(*new QAbstractSocketEnginePrivate(), parent)
{
}

QAbstractSocketEngine::QAbstractSocketEngine(QAbstractSocketEnginePrivate &dd, QObject* parent)
    : QObject(dd, parent)
{
}

QAbstractSocketEngine *QAbstractSocketEngine::createSocketEngine(const QHostAddress &address, QAbstractSocket::SocketType socketType, QObject *parent)
{
    QMutexLocker locker(&socketHandlers()->mutex);
    for (int i = 0; i < socketHandlers()->size(); i++) {
        if (QAbstractSocketEngine *ret = socketHandlers()->at(i)->createSocketEngine(address, socketType, parent))
            return ret;
    }
    return new QNativeSocketEngine(parent);
}

QAbstractSocketEngine *QAbstractSocketEngine::createSocketEngine(int socketDescripter, QObject *parent)
{
    QMutexLocker locker(&socketHandlers()->mutex);
    for (int i = 0; i < socketHandlers()->size(); i++) {
        if (QAbstractSocketEngine *ret = socketHandlers()->at(i)->createSocketEngine(socketDescripter, parent))
            return ret;
    }
    return new QNativeSocketEngine(parent);
}

QAbstractSocket::SocketError QAbstractSocketEngine::error() const
{
    return d_func()->socketError;
}

QString QAbstractSocketEngine::errorString() const
{
    return d_func()->socketErrorString;
}

void QAbstractSocketEngine::setError(QAbstractSocket::SocketError error, const QString &errorString) const
{
    Q_D(const QAbstractSocketEngine);
    d->socketError = error;
    d->socketErrorString = errorString;
}

void QAbstractSocketEngine::setReceiver(QAbstractSocketEngineReceiver *receiver)
{
    d_func()->receiver = receiver;
}

void QAbstractSocketEngine::readNotification()
{
    if (QAbstractSocketEngineReceiver *receiver = d_func()->receiver)
        receiver->readNotification();
}

void QAbstractSocketEngine::writeNotification()
{
    if (QAbstractSocketEngineReceiver *receiver = d_func()->receiver)
        receiver->writeNotification();
}

void QAbstractSocketEngine::exceptionNotification()
{
    if (QAbstractSocketEngineReceiver *receiver = d_func()->receiver)
        receiver->exceptionNotification();
}

#ifndef QT_NO_NETWORKPROXY
void QAbstractSocketEngine::proxyAuthenticationRequired(const QNetworkProxy &proxy, QAuthenticator *authenticator)
{
    if (QAbstractSocketEngineReceiver *receiver = d_func()->receiver)
        receiver->proxyAuthenticationRequired(proxy, authenticator);
}
#endif


QAbstractSocket::SocketState QAbstractSocketEngine::state() const
{
    return d_func()->socketState;
}

void QAbstractSocketEngine::setState(QAbstractSocket::SocketState state)
{
    d_func()->socketState = state;
}

QAbstractSocket::SocketType QAbstractSocketEngine::socketType() const
{
    return d_func()->socketType;
}

void QAbstractSocketEngine::setSocketType(QAbstractSocket::SocketType socketType)
{
    d_func()->socketType = socketType;
}

QAbstractSocket::NetworkLayerProtocol QAbstractSocketEngine::protocol() const
{
    return d_func()->socketProtocol;
}

void QAbstractSocketEngine::setProtocol(QAbstractSocket::NetworkLayerProtocol protocol)
{
    d_func()->socketProtocol = protocol;
}

QHostAddress QAbstractSocketEngine::localAddress() const
{
    return d_func()->localAddress;
}

void QAbstractSocketEngine::setLocalAddress(const QHostAddress &address)
{
    d_func()->localAddress = address;
}

quint16 QAbstractSocketEngine::localPort() const
{
    return d_func()->localPort;
}

void QAbstractSocketEngine::setLocalPort(quint16 port)
{
    d_func()->localPort = port;
}

QHostAddress QAbstractSocketEngine::peerAddress() const
{
    return d_func()->peerAddress;
}

void QAbstractSocketEngine::setPeerAddress(const QHostAddress &address)
{
   d_func()->peerAddress = address;
}

quint16 QAbstractSocketEngine::peerPort() const
{
    return d_func()->peerPort;
}

void QAbstractSocketEngine::setPeerPort(quint16 port)
{
    d_func()->peerPort = port;
}

QT_END_NAMESPACE
