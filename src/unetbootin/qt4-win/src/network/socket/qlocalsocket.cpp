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

#include "qlocalsocket.h"
#include "qlocalsocket_p.h"

QT_BEGIN_NAMESPACE

/*!
    \class QLocalSocket
    \since 4.4

    \brief The QLocalSocket class provides a local socket.

    On Windows this is a named pipe and on Unix this this is
    a local domain socket.

    If an error occurs, socketError() returns the type of error, and
    errorString() can be called to get a human readable description
    of what happened.

    Although QLocalSocket is designed for use with an event loop, it's possible
    to use it without one. In that case, you must use waitForConnected(),
    waitForReadyRead(), waitForBytesWritten(), and waitForDisconnected()
    which blocks until the operation is complete or the timeout expires.

    Note that this feature is not supported on Windows CE.

    \sa QLocalServer
*/

/*!
    \fn void QLocalSocket::connected()

    This signal is emitted after connectToServer() has been called and
    a connection has been successfully established.

    \sa connectToServer(), disconnected()
*/

/*!
    \fn void QLocalSocket::disconnected()

    This signal is emitted when the socket has been disconnected.

    \sa connectToServer(), disconnectFromServer(), abort(), connected()
*/

/*!
    \fn void QLocalSocket::error(QLocalSocket::LocalSocketError socketError)

    This signal is emitted after an error occurred. The \a socketError
    parameter describes the type of error that occurred.

    QLocalSocket::LocalSocketError is not a registered metatype, so for queued
    connections, you will have to register it with Q_REGISTER_METATYPE.

    \sa error(), errorString()
*/

/*!
    \fn void QLocalSocket::stateChanged(QLocalSocket::LocalSocketState socketState)

    This signal is emitted whenever QLocalSocket's state changes.
    The \a socketState parameter is the new state.

    QLocalSocket::SocketState is not a registered metatype, so for queued
    connections, you will have to register it with Q_REGISTER_METATYPE.

    \sa state()
*/

/*!
    Creates a new local socket. The \a parent argument is passed to
    QObject's constructor.
 */
QLocalSocket::QLocalSocket(QObject * parent)
    : QIODevice(*new QLocalSocketPrivate, parent)
{
    Q_D(QLocalSocket);
    d->init();
}

/*!
    Destroys the socket, closing the connection if necessary.
 */
QLocalSocket::~QLocalSocket()
{
    close();
#ifndef Q_OS_WIN
    Q_D(QLocalSocket);
    d->unixSocket.setParent(0);
#endif
}

/*!
    Returns the name of the peer as specified by connectToServer(), or an
    empty QString if connectToServer() has not been called or it failed.

    \sa connectToServer(), fullServerName()

 */
QString QLocalSocket::serverName() const
{
    Q_D(const QLocalSocket);
    return d->serverName;
}

/*!
    Returns the server path that the socket is connected to.

    Note: This is platform specific

    \sa connectToServer(), serverName()
 */
QString QLocalSocket::fullServerName() const
{
    Q_D(const QLocalSocket);
    return d->fullServerName;
}

/*!
    Returns the state of the socket.

    \sa error()
 */
QLocalSocket::LocalSocketState QLocalSocket::state() const
{
    Q_D(const QLocalSocket);
    return d->state;
}

/*! \reimp
*/
bool QLocalSocket::isSequential() const
{
    return true;
}

/*!
    \enum QLocalSocket::LocalSocketError

    The LocalServerError enumeration represents the errors that can occur.
    The most recent error can be retrieved through a call to
    \l QLocalSocket::error().

    \value ConnectionRefusedError The connection was refused by
        the peer (or timed out).
    \value PeerClosedError  The remote socket closed the connection.
        Note that the client socket (i.e., this socket) will be closed
        after the remote close notification has been sent.
    \value ServerNotFoundError  The local socket name was not found.
    \value SocketAccessError The socket operation failed because the
        application lacked the required privileges.
    \value SocketResourceError The local system ran out of resources
        (e.g., too many sockets).
    \value SocketTimeoutError The socket operation timed out.
    \value DatagramTooLargeError The datagram was larger than the operating
        system's limit (which can be as low as 8192 bytes).
    \value ConnectionError An error occurred with the connection.
    \value UnsupportedSocketOperationError The requested socket operation
        is not supported by the local operating system.
    \value UnknownSocketError An unidentified error occurred.
 */

/*!
    \enum QLocalSocket::LocalSocketState

    This enum describes the different states in which a socket can be.

    \sa QLocalSocket::state()

    \value UnconnectedState The socket is not connected.
    \value ConnectingState The socket has started establishing a connection.
    \value ConnectedState A connection is established.
    \value ClosingState The socket is about to close
        (data may still be waiting to be written).
 */

#ifndef QT_NO_DEBUG_STREAM
QDebug operator<<(QDebug debug, QLocalSocket::LocalSocketError error)
{
    switch (error) {
    case QLocalSocket::ConnectionRefusedError:
        debug << "QLocalSocket::ConnectionRefusedError";
        break;
    case QLocalSocket::PeerClosedError:
        debug << "QLocalSocket::PeerClosedError";
        break;
    case QLocalSocket::ServerNotFoundError:
        debug << "QLocalSocket::ServerNotFoundError";
        break;
    case QLocalSocket::SocketAccessError:
        debug << "QLocalSocket::SocketAccessError";
        break;
    case QLocalSocket::SocketResourceError:
        debug << "QLocalSocket::SocketResourceError";
        break;
    case QLocalSocket::SocketTimeoutError:
        debug << "QLocalSocket::SocketTimeoutError";
        break;
    case QLocalSocket::DatagramTooLargeError:
        debug << "QLocalSocket::DatagramTooLargeError";
        break;
    case QLocalSocket::ConnectionError:
        debug << "QLocalSocket::ConnectionError";
        break;
    case QLocalSocket::UnsupportedSocketOperationError:
        debug << "QLocalSocket::UnsupportedSocketOperationError";
        break;
    case QLocalSocket::UnknownSocketError:
        debug << "QLocalSocket::UnknownSocketError";
        break;
    default:
        debug << "QLocalSocket::SocketError(" << int(error) << ")";
        break;
    }
    return debug;
}

QDebug operator<<(QDebug debug, QLocalSocket::LocalSocketState state)
{
    switch (state) {
    case QLocalSocket::UnconnectedState:
        debug << "QLocalSocket::UnconnectedState";
        break;
    case QLocalSocket::ConnectingState:
        debug << "QLocalSocket::ConnectingState";
        break;
    case QLocalSocket::ConnectedState:
        debug << "QLocalSocket::ConnectedState";
        break;
    case QLocalSocket::ClosingState:
        debug << "QLocalSocket::ClosingState";
        break;
    default:
        debug << "QLocalSocket::SocketState(" << int(state) << ")";
        break;
    }
    return debug;
}
#endif

QT_END_NAMESPACE

#include "moc_qlocalsocket.cpp"
