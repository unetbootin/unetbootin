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

#include "qsocks5socketengine_p.h"

#ifndef QT_NO_SOCKS5

#include "qtcpsocket.h"
#include "qudpsocket.h"
#include "qtcpserver.h"
#include "qdebug.h"
#include "qhash.h"
#include "qqueue.h"
#include "qdatetime.h"
#include "qmutex.h"
#include "qthread.h"
#include "qcoreapplication.h"
#include <qendian.h>

QT_BEGIN_NAMESPACE

//#define QSOCKS5SOCKETLAYER_DEBUG

#define MAX_DATA_DUMP 256
#if !defined(Q_OS_WINCE)
#define SOCKS5_BLOCKING_BIND_TIMEOUT 5000
#else
#define SOCKS5_BLOCKING_BIND_TIMEOUT 10000
#endif

#define Q_INIT_CHECK(returnValue) do { \
    if (!d->data) { \
        return returnValue; \
    } } while (0)

#ifdef QSOCKS5SOCKETLAYER_DEBUG
#  define QSOCKS5_Q_DEBUG qDebug() << this
#  define QSOCKS5_D_DEBUG qDebug() << q_ptr
#  define QSOCKS5_DEBUG qDebug() << "[QSocks5]"
#else
#  define QSOCKS5_DEBUG if (0) qDebug()
#  define QSOCKS5_Q_DEBUG if (0) qDebug()
#  define QSOCKS5_D_DEBUG if (0) qDebug()
#endif

#define S5_VERSION_5 0x05
#define S5_CONNECT 0x01
#define S5_BIND 0x02
#define S5_UDP_ASSOCIATE 0x03
#define S5_IP_V4 0x01
#define S5_DOMAINNAME 0x03
#define S5_IP_V6 0x04
#define S5_SUCCESS 0x00
#define S5_R_ERROR_SOCKS_FAILURE 0x01
#define S5_R_ERROR_CON_NOT_ALLOWED 0x02
#define S5_R_ERROR_NET_UNREACH 0x03
#define S5_R_ERROR_HOST_UNREACH 0x04
#define S5_R_ERROR_CONN_REFUSED 0x05
#define S5_R_ERROR_TTL 0x06
#define S5_R_ERROR_CMD_NOT_SUPPORTED 0x07
#define S5_R_ERROR_ADD_TYPE_NOT_SUPORTED 0x08

static QString s5RequestErrorToString(int s5_r_error)
{
    QString ret;
    switch(s5_r_error) {
    case 0x01 : ret = QLatin1String("general SOCKS server failure"); break;
    case 0x02 : ret = QLatin1String("connection not allowed by ruleset"); break;
    case 0x03 : ret = QLatin1String("Network unreachable"); break;
    case 0x04 : ret = QLatin1String("Host unreachable"); break;
    case 0x05 : ret = QLatin1String("Connection refused"); break;
    case 0x06 : ret = QLatin1String("TTL expired"); break;
    case 0x07 : ret = QLatin1String("Command not supported"); break;
    case 0x08 : ret = QLatin1String("Address type not supported"); break;
    default   : ret = QLatin1String("unassigned error code"); break;
    }
    return ret;
}

static QString makeErrorString(const QString & e)
{
    return QLatin1String("Socks 5 - ") + e;
}

static QAbstractSocket::SocketError s5RAsSocketError(int s5_r_error)
{
    QAbstractSocket::SocketError ret;
    switch(s5_r_error) {
    case 0x01 : ret = QAbstractSocket::NetworkError; break;
    case 0x02 : ret = QAbstractSocket::SocketAccessError; break;
    case 0x03 : ret = QAbstractSocket::NetworkError; break;
    case 0x04 : ret = QAbstractSocket::HostNotFoundError; break;
    case 0x05 : ret = QAbstractSocket::ConnectionRefusedError; break;
    case 0x06 : ret = QAbstractSocket::NetworkError; break;
    case 0x07 : ret = QAbstractSocket::UnsupportedSocketOperationError; break;
    case 0x08 : ret = QAbstractSocket::UnsupportedSocketOperationError; break;
    default   : ret = QAbstractSocket::NetworkError; break;
    }
    return ret;
}

static QString s5StateToString(QSocks5SocketEnginePrivate::Socks5State s)
{
    switch (s) {
    case QSocks5SocketEnginePrivate::unInitialized: return QLatin1String("unInitialized");
    case QSocks5SocketEnginePrivate::AuthenticationMethodsSent: return QLatin1String("AuthenticationMethodsSent");
    case QSocks5SocketEnginePrivate::Authenticating: return QLatin1String("Authenticating");
    case QSocks5SocketEnginePrivate::RequestMethodSent: return QLatin1String("RequestMethodSent");
    case QSocks5SocketEnginePrivate::RequestSuccess: return QLatin1String("RequestSuccess");
    case QSocks5SocketEnginePrivate::RequestError: return QLatin1String("RequestError");
    case QSocks5SocketEnginePrivate::Connected: return QLatin1String("Connected");
    case QSocks5SocketEnginePrivate::ConnectError: return QLatin1String("BindSuccess");
    case QSocks5SocketEnginePrivate::BindSuccess: return QLatin1String("unInitialized");
    case QSocks5SocketEnginePrivate::BindError: return QLatin1String("BindError");
    case QSocks5SocketEnginePrivate::ControlSocketError: return QLatin1String("ControlSocketError");
    case QSocks5SocketEnginePrivate::SocksError: return QLatin1String("SocksError");
    default: break;
    }
    return QLatin1String("unknown state");
}

static QString dump(const QByteArray &buf)
{
    QString data;
    for (int i = 0; i < qMin<int>(MAX_DATA_DUMP, buf.size()); ++i) {
        if (i) data += QLatin1Char(' ');
        uint val = (unsigned char)buf.at(i);
       // data += QString("0x%1").arg(val, 3, 16, QLatin1Char('0'));
        data += QString::number(val);
    }
    if (buf.size() > MAX_DATA_DUMP)
        data += QLatin1String(" ...");

    return QString::fromLatin1("size: %1 data: { %2 }").arg(buf.size()).arg(data);
}

/*
   inserets the host address in buf at pos and updates pos.
   if the func fails the data in buf and the vallue of pos is undefined
*/
static bool qt_socks5_set_host_address_and_port(const QHostAddress &address, quint16 port, QByteArray *pBuf, int *pPos)
{
    bool ret = false;
    int pos = *pPos;

    QSOCKS5_DEBUG << "setting [" << address << ":" << port << "]";

    if (address.protocol() == QAbstractSocket::IPv4Protocol) {
        int spaceAvailable = pBuf->size() - pos;
        if (spaceAvailable < int(sizeof(quint32) + 1))
            pBuf->resize(pBuf->size() + sizeof(quint32) + 1 - spaceAvailable);
        unsigned char *buf = reinterpret_cast<unsigned char *>(pBuf->data());
        buf[pos++] = S5_IP_V4;
        qToUnaligned(qToBigEndian<quint32>(address.toIPv4Address()), &buf[pos]);
        pos += sizeof(quint32);
        ret = true;
    } else if (address.protocol() == QAbstractSocket::IPv6Protocol) {
        int spaceAvailable = pBuf->size() - pos;
        if (spaceAvailable < 17)
            pBuf->resize(pBuf->size() + 17 - spaceAvailable);
        char *buf = pBuf->data();
        buf[pos++] = S5_IP_V6;
        QIPv6Address ipv6 = address.toIPv6Address();
        for (int i = 0; i < 16; ++i)
            buf[pos++] = ipv6[i];
        ret = true;
    } else {
        // domain name.
        ret = false;
    }

    if (ret) {
        int spaceAvailable = pBuf->size() - pos;
        if (spaceAvailable < int(sizeof(quint16)))
            pBuf->resize(pBuf->size() + sizeof(quint16) - spaceAvailable);
        unsigned char *buf = reinterpret_cast<unsigned char *>(pBuf->data());
        qToUnaligned(qToBigEndian<quint16>(port), &buf[pos]);
        pos += sizeof(quint16);
    }

    if (ret)
        *pPos = pos;

    return ret;
}

/*
   retrives the host address in buf at pos and updates pos.
   if the func fails the value of the address and the pos is undefined
*/
static bool qt_socks5_get_host_address_and_port(const QByteArray &buf, QHostAddress *pAddress, quint16 *pPort, int *pPos)
{
    bool ret = false;
    int pos = *pPos;
    const unsigned char *pBuf = reinterpret_cast<const unsigned char*>(buf.constData());
    QHostAddress address;
    quint16 port = 0;

    if (buf.size() - pos < 1) {
        QSOCKS5_DEBUG << "need more data address/port";
        return false;
    }
    if (pBuf[pos] == S5_IP_V4) {
        pos++;
        if (buf.size() - pos < 4) {
            QSOCKS5_DEBUG << "need more data for ip4 address";
            return false;
        }
        address.setAddress(qFromBigEndian<quint32>(&pBuf[pos]));
        pos += 4;
        ret = true;
    } else if (pBuf[pos] == S5_IP_V6) {
        pos++;
        if (buf.size() - pos < 16) {
            QSOCKS5_DEBUG << "need more data for ip6 address";
            return false;
        }
        QIPv6Address add;
        for (int i = 0; i < 16; ++i)
            add[i] = buf[pos++];
        ret = true;
    } else if (pBuf[pos] == S5_DOMAINNAME){
        pos++;
        // domain name.
        ret = false;
    } else {
        QSOCKS5_DEBUG << "invalid address type" << (int)pBuf[pos];
        ret = false;
    }

    if (ret) {
        if (buf.size() - pos < 2) {
            QSOCKS5_DEBUG << "need more data for port";
            return false;
        }
        port = qFromBigEndian<quint16>(&pBuf[pos]);
        pos += 2;
    }

    if (ret) {
        QSOCKS5_DEBUG << "got [" << address << ":" << port << "]";
        *pAddress = address;
        *pPort = port;
        *pPos = pos;
    }

    return ret;
}

/*
   Returns the difference between msecs and elapsed. If msecs is -1,
   however, -1 is returned.
*/
static int qt_timeout_value(int msecs, int elapsed)
{
    if (msecs == -1)
        return -1;

    int timeout = msecs - elapsed;
    return timeout < 0 ? 0 : timeout;
}


struct QSocks5Data
{
    QTcpSocket *controlSocket;
    QSocks5Authenticator *authenticator;
};

struct QSocks5ConnectData : public QSocks5Data
{
    QByteArray readBuffer;
};

struct QSocks5BindData : public QSocks5Data
{
    QHostAddress localAddress;
    quint16 localPort;
    QHostAddress peerAddress;
    quint16 peerPort;
    QDateTime timeStamp;
};

struct QSocks5RevivedDatagram
{
    QByteArray data;
    QHostAddress address;
    quint16 port;
};

#ifndef QT_NO_UDPSOCKET
struct QSocks5UdpAssociateData : public QSocks5Data
{
    QUdpSocket *udpSocket;
    QHostAddress associateAddress;
    quint16 associatePort;
    QQueue<QSocks5RevivedDatagram> pendingDatagrams;
};
#endif

// needs to be thread safe
class QSocks5BindStore : public QObject
{
public:
    QSocks5BindStore();
    ~QSocks5BindStore();

    void add(int socketDescriptor, QSocks5BindData *bindData);
    bool contains(int socketDescriptor);
    QSocks5BindData *retrieve(int socketDescriptor);

protected:
    void timerEvent(QTimerEvent * event);

    QMutex mutex;
    int sweepTimerId;
    //socket descriptor, data, timestamp
    QHash<int, QSocks5BindData *> store;
};

Q_GLOBAL_STATIC(QSocks5BindStore, socks5BindStore)

QSocks5BindStore::QSocks5BindStore()
    : mutex(QMutex::Recursive)
    , sweepTimerId(-1)
{
    QCoreApplication *app = QCoreApplication::instance();
    if (app && app->thread() != thread())
        moveToThread(app->thread());
}

QSocks5BindStore::~QSocks5BindStore()
{
}

void QSocks5BindStore::add(int socketDescriptor, QSocks5BindData *bindData)
{
    QMutexLocker lock(&mutex);
    if (store.contains(socketDescriptor)) {
        // qDebug() << "delete it";
    }
    bindData->timeStamp = QDateTime::currentDateTime();
    store.insert(socketDescriptor, bindData);
    // start sweep timer if not started
    if (sweepTimerId == -1)
        sweepTimerId = startTimer(60000);
}

bool QSocks5BindStore::contains(int socketDescriptor)
{
    QMutexLocker lock(&mutex);
    return store.contains(socketDescriptor);
}

QSocks5BindData *QSocks5BindStore::retrieve(int socketDescriptor)
{
    QMutexLocker lock(&mutex);
    if (!store.contains(socketDescriptor))
        return 0;
    QSocks5BindData *bindData = store.take(socketDescriptor);
    if (bindData) {
        if (bindData->controlSocket->thread() != QThread::currentThread()) {
            qWarning("Can not access socks5 bind data from different thread");
            return 0;
        }
    } else {
        QSOCKS5_DEBUG << "__ERROR__ binddata == 0";
    }
    // stop the sweep timer if not needed
    if (store.isEmpty()) {
        killTimer(sweepTimerId);
        sweepTimerId = -1;
    }
    return bindData;
}

void QSocks5BindStore::timerEvent(QTimerEvent * event)
{
    QMutexLocker lock(&mutex);
    if (event->timerId() == sweepTimerId) {
        QSOCKS5_DEBUG << "QSocks5BindStore performing sweep";
        QMutableHashIterator<int, QSocks5BindData *> it(store);
        while (it.hasNext()) {
            it.next();
            if (it.value()->timeStamp.secsTo(QDateTime::currentDateTime()) > 350) {
                QSOCKS5_DEBUG << "QSocks5BindStore removing JJJJ";
                it.remove();
            }
        }
    }
}

QSocks5Authenticator::QSocks5Authenticator()
{
}

QSocks5Authenticator::~QSocks5Authenticator()
{
}

char QSocks5Authenticator::methodId()
{
    return 0x00;
}

bool QSocks5Authenticator::beginAuthenticate(QTcpSocket *socket, bool *completed)
{
    Q_UNUSED(socket);
    *completed = true;
    return true;
}

bool QSocks5Authenticator::continueAuthenticate(QTcpSocket *socket, bool *completed)
{
    Q_UNUSED(socket);
    *completed = true;
    return true;
}

bool QSocks5Authenticator::seal(const QByteArray buf, QByteArray *sealedBuf)
{
    *sealedBuf = buf;
    return true;
}

bool QSocks5Authenticator::unSeal(const QByteArray sealedBuf, QByteArray *buf)
{
    *buf = sealedBuf;
    return true;
}

bool QSocks5Authenticator::unSeal(QTcpSocket *sealedSocket, QByteArray *buf)
{
    return unSeal(sealedSocket->readAll(), buf);
}

QSocks5PasswordAuthenticator::QSocks5PasswordAuthenticator(const QString &userName, const QString &password)
{
    this->userName = userName;
    this->password = password;
}

char QSocks5PasswordAuthenticator::methodId()
{
    return 0x02;
}

bool QSocks5PasswordAuthenticator::beginAuthenticate(QTcpSocket *socket, bool *completed)
{
    *completed = false;
    QByteArray uname = userName.toLatin1();
    QByteArray passwd = password.toLatin1();
    QByteArray dataBuf(3 + uname.size() + passwd.size(), 0);
    char *buf = dataBuf.data();
    int pos = 0;
    buf[pos++] = 0x01;
    buf[pos++] = uname.size();
    memcpy(&buf[pos], uname.data(), uname.size());
    pos += uname.size();
    buf[pos++] = passwd.size();
    memcpy(&buf[pos], passwd.data(), passwd.size());
    return socket->write(dataBuf) == dataBuf.size();
}

bool QSocks5PasswordAuthenticator::continueAuthenticate(QTcpSocket *socket, bool *completed)
{
    *completed = false;

    if (socket->bytesAvailable() < 2)
        return true;

    QByteArray buf = socket->read(2);
    if (buf.at(0) == 0x01) {
        *completed = true;
        return buf.at(1) == 0x00;
    }
    return false;
}

QString QSocks5PasswordAuthenticator::errorString()
{
    return QLatin1String("Socks5 user name or password incorrect");
}



QSocks5SocketEnginePrivate::QSocks5SocketEnginePrivate()
    : socks5State(unInitialized)
    , readNotificationEnabled(false)
    , writeNotificationEnabled(false)
    , exceptNotificationEnabled(false)
    , socketDescriptor(-1)
    , data(0)
    , connectData(0)
#ifndef QT_NO_UDPSOCKET
    , udpData(0)
#endif
    , bindData(0)
    , readNotificationActivated(false)
    , writeNotificationActivated(false)
    , readNotificationPending(false)
    , writeNotificationPending(false)
{
    mode = NoMode;
}

QSocks5SocketEnginePrivate::~QSocks5SocketEnginePrivate()
{
}

void QSocks5SocketEnginePrivate::initialize(Socks5Mode socks5Mode)
{
    Q_Q(QSocks5SocketEngine);

    mode = socks5Mode;
    if (mode == ConnectMode) {
        connectData = new QSocks5ConnectData;
        data = connectData;
#ifndef QT_NO_UDPSOCKET
    } else if (mode == UdpAssociateMode) {
        udpData = new QSocks5UdpAssociateData;
        data = udpData;
        udpData->udpSocket = new QUdpSocket(q);
        QNetworkProxy proxy;
        proxy.setType(QNetworkProxy::NoProxy);
        udpData->udpSocket->setProxy(proxy);
        QObject::connect(udpData->udpSocket, SIGNAL(readyRead()),
                         q, SLOT(_q_udpSocketReadNotification()),
                         Qt::DirectConnection);
#endif // QT_NO_UDPSOCKET
    } else if (mode == BindMode) {
        bindData = new QSocks5BindData;
        data = bindData;
    }

    data->controlSocket = new QTcpSocket(q);
    QNetworkProxy proxy;
    proxy.setType(QNetworkProxy::NoProxy);
    data->controlSocket->setProxy(proxy);
    QObject::connect(data->controlSocket, SIGNAL(connected()), q, SLOT(_q_controlSocketConnected()),
                     Qt::DirectConnection);
    QObject::connect(data->controlSocket, SIGNAL(readyRead()), q, SLOT(_q_controlSocketReadNotification()),
                     Qt::DirectConnection);
    QObject::connect(data->controlSocket, SIGNAL(bytesWritten(qint64)), q, SLOT(_q_controlSocketBytesWritten()),
                     Qt::DirectConnection);
    QObject::connect(data->controlSocket, SIGNAL(error(QAbstractSocket::SocketError)),
                     q, SLOT(_q_controlSocketError(QAbstractSocket::SocketError)),
                     Qt::DirectConnection);
    QObject::connect(data->controlSocket, SIGNAL(disconnected()), q, SLOT(_q_controlSocketDisconnected()),
                     Qt::DirectConnection);
    QObject::connect(data->controlSocket, SIGNAL(stateChanged(QAbstractSocket::SocketState)),
                     q, SLOT(_q_controlSocketStateChanged(QAbstractSocket::SocketState)),
                     Qt::DirectConnection);
    //### this should be some where else when authentication methods are public
    if (!proxyInfo.user().isEmpty() || !proxyInfo.password().isEmpty()) {
        data->authenticator = new QSocks5PasswordAuthenticator(proxyInfo.user(), proxyInfo.password());
    } else {
        data->authenticator = new QSocks5Authenticator();
    }
}

void QSocks5SocketEnginePrivate::parseAuthenticationMethodReply()
{
    Q_Q(QSocks5SocketEngine);

    // not enough data to begin
    if (data->controlSocket->bytesAvailable() < 2)
        return;

    QByteArray buf(2, 0);
    if (data->controlSocket->read(buf.data(), 2) != 2) {
        QSOCKS5_D_DEBUG << "Control socket read failure";
        socks5State = AuthenticatingError;
        q->setError(QAbstractSocket::NetworkError, QLatin1String("Socks5 read error on control socket"));
        data->controlSocket->close();
        emitWriteNotification();
        return;
    }
    if (buf.at(0) != S5_VERSION_5) {
        QSOCKS5_D_DEBUG << "Socks5 version incorrect";
        socks5State = AuthenticatingError;
        q->setError(QAbstractSocket::NetworkError, QLatin1String("Socks5 version incorrect"));
        data->controlSocket->close();
        emitWriteNotification();
        return;
    }
    if (uchar(buf.at(1)) == 0xFF) {
        QSOCKS5_D_DEBUG << "Authentication method not supported";
        socks5State = AuthenticatingError;
        q->setError(QAbstractSocket::SocketAccessError, QLatin1String("Socks5 host did not support authentication method."));
        emitWriteNotification();
        return;
    }
    if (buf.at(1) != data->authenticator->methodId()) {
        QSOCKS5_D_DEBUG << "Authentication method was not what we sent";
        socks5State = AuthenticatingError;
        q->setError(QAbstractSocket::SocketAccessError, QLatin1String("Socks5 host did not support authentication method."));
        emitWriteNotification();
        return;
    }
    bool AuthComplete = false;
    if (!data->authenticator->beginAuthenticate(data->controlSocket, &AuthComplete)) {
        QSOCKS5_D_DEBUG << "Authentication faled" << data->authenticator->errorString();
        socks5State = AuthenticatingError;
        q->setError(QAbstractSocket::SocketAccessError, data->authenticator->errorString());
        emitWriteNotification();
        return;
    }
    if (AuthComplete) {
        sendRequestMethod();
        return;
    }
    socks5State = Authenticating;
}

void QSocks5SocketEnginePrivate::parseAuthenticatingReply()
{
    Q_Q(QSocks5SocketEngine);

    bool authComplete = false;
    if (!data->authenticator->continueAuthenticate(data->controlSocket, &authComplete)) {
        QSOCKS5_D_DEBUG << "Authentication faled" << data->authenticator->errorString();
        socks5State = AuthenticatingError;
        q->setError(QAbstractSocket::SocketAccessError, data->authenticator->errorString());
        emitWriteNotification();
        return;
    }
    if (authComplete)
        sendRequestMethod();
}

void QSocks5SocketEnginePrivate::sendRequestMethod()
{
    QHostAddress address;
    quint16 port = 0;
    char command = 0;
    if (mode == ConnectMode) {
        command = S5_CONNECT;
        address = peerAddress;
        port = peerPort;
    } else if (mode == BindMode) {
        command = S5_BIND;
        address = localAddress;
        port = localPort;
    } else {
#ifndef QT_NO_UDPSOCKET
        command = S5_UDP_ASSOCIATE;
        address = localAddress; //data->controlSocket->localAddress();
        port = localPort;
#endif
    }

    QByteArray buf;
    buf.reserve(270); // big enough for domain name;
    int pos = 0;
    buf[pos++] = S5_VERSION_5;
    buf[pos++] = command;
    buf[pos++] = 0x00;
    if (!qt_socks5_set_host_address_and_port(address, port, &buf, &pos)) {
        QSOCKS5_DEBUG << "error setting address" << address << " : " << port;
        //### set error code ....
        return;
    }
    QSOCKS5_DEBUG << "sending" << dump(buf);
    QByteArray sealedBuf;
    if (!data->authenticator->seal(buf, &sealedBuf)) {
        // ### Handle this error.
    }
    data->controlSocket->write(sealedBuf);
    data->controlSocket->flush();
    socks5State = RequestMethodSent;
}

void QSocks5SocketEnginePrivate::parseRequestMethodReply()
{
    QSOCKS5_DEBUG << "parseRequestMethodReply()";

    QByteArray inBuf;
    if (!data->authenticator->unSeal(data->controlSocket, &inBuf)) {
        // ### check error and not just not enough data
        QSOCKS5_DEBUG << "unSeal failed, needs more data";
        return;
    }
    QSOCKS5_DEBUG << dump(inBuf);
    int pos = 0;
    const char *buf = inBuf.constData();
    if (inBuf.size() < 2) {
        QSOCKS5_DEBUG << "need more data for request reply header .. put this data somewhere";
        return;
    }
    if (buf[pos++] != S5_VERSION_5) {
        QSOCKS5_DEBUG << "totally lost";
    }
    if (buf[pos++] != S5_SUCCESS ) {
        socks5Error = Socks5Error(buf[pos-1]);
        socks5State = RequestError;
        socks5ErrorString = s5RequestErrorToString(socks5Error);
        QSOCKS5_DEBUG <<  "Request error :" << s5RequestErrorToString(socks5Error);
        emitWriteNotification();
        return;
    }
    if (buf[pos++] != 0x00) {
        QSOCKS5_DEBUG << "totally lost";
    }
    if (!qt_socks5_get_host_address_and_port(inBuf, &localAddress, &localPort, &pos)) {
        QSOCKS5_DEBUG << "error getting address";
        //### set error code ....
        return;
    }

    // need a better place to keep this stuff and any others untill connect called again
    // should use peek
    inBuf.remove(0, pos);
    for (int i = inBuf.size() - 1; i >= 0 ; --i)
        data->controlSocket->ungetChar(inBuf.at(i));

    socks5State = RequestSuccess;

    // fire writeNotifier for connect and wait for the next call to conectTOHost
    if (mode == ConnectMode)
        emitWriteNotification();
}

void QSocks5SocketEnginePrivate::parseNewConnection()
{
    QSOCKS5_D_DEBUG << "parseNewConnection()";
    // only emit readyRead if in listening state ...
    QByteArray inBuf;
    if (!data->authenticator->unSeal(data->controlSocket, &inBuf)) {
        // ### check error and not just not enough data
        QSOCKS5_DEBUG << "unSeal failed, needs more data";
        return;
    }
    QSOCKS5_D_DEBUG << dump(inBuf);
    int pos = 0;
    const char *buf = inBuf.constData();
    if (inBuf.length() < 2) {
        QSOCKS5_D_DEBUG << "need more data for request reply header .. put this data somewhere";
        return;
    }
    if (buf[pos++] != S5_VERSION_5) {
        QSOCKS5_D_DEBUG << "totally lost";
    }
    if (buf[pos++] != S5_SUCCESS) {
        QSOCKS5_D_DEBUG <<  "Request error :" << s5RequestErrorToString(buf[pos-1]);
        socks5State = BindError;
        socks5Error = Socks5Error(buf[pos-1]);
        socks5ErrorString = s5RequestErrorToString(socks5Error);
        // #### now what
        return;
    }
    if (buf[pos++] != 0x00) {
        QSOCKS5_D_DEBUG << "totally lost";
    }
    if (!qt_socks5_get_host_address_and_port(inBuf, &bindData->peerAddress, &bindData->peerPort, &pos)) {
        QSOCKS5_D_DEBUG << "error getting address";
        //### set error code ....
        return;
    }

    // need a better place to keep this stuff and any others untill connect called again
    // should use peek
    inBuf.remove(0, pos);
    for (int i = inBuf.size() - 1; i >= 0 ; --i)
        data->controlSocket->ungetChar(inBuf.at(i));

    // got a successful reply
    socks5State = BindSuccess;
    if (socketState == QAbstractSocket::ListeningState)
        emitReadNotification();
}

void QSocks5SocketEnginePrivate::_q_emitPendingReadNotification()
{
    Q_Q(QSocks5SocketEngine);
    readNotificationPending = false;
    if (readNotificationEnabled) {
        QSOCKS5_D_DEBUG << "emitting readNotification";
        QPointer<QSocks5SocketEngine> qq = q;
        emit q->readNotification();
        if (!qq)
            return;
        // check if there needs to be a new zero read notifcation
        if (socks5State == ControlSocketError
            && data->controlSocket->error() == QAbstractSocket::RemoteHostClosedError) {
            connectData->readBuffer.clear();
            emitReadNotification();
        }
    }
}

void QSocks5SocketEnginePrivate::emitReadNotification()
{
    Q_Q(QSocks5SocketEngine);
    readNotificationActivated = true;
    if (readNotificationEnabled && !readNotificationPending) {
        QSOCKS5_D_DEBUG << "queueing readNotification";
        readNotificationPending = true;
        QMetaObject::invokeMethod(q, "_q_emitPendingReadNotification", Qt::QueuedConnection);
    }
}

void QSocks5SocketEnginePrivate::_q_emitPendingWriteNotification()
{
    writeNotificationPending = false;
    Q_Q(QSocks5SocketEngine);
    if (writeNotificationEnabled) {
        QSOCKS5_D_DEBUG << "emitting writeNotification";
        emit q->writeNotification();
    }
}

void QSocks5SocketEnginePrivate::emitWriteNotification()
{
    Q_Q(QSocks5SocketEngine);
    writeNotificationActivated = true;
    if (writeNotificationEnabled && !writeNotificationPending) {
        QSOCKS5_D_DEBUG << "queueing writeNotification";
        writeNotificationPending = true;
        QMetaObject::invokeMethod(q, "_q_emitPendingWriteNotification", Qt::QueuedConnection);
    }
}

QSocks5SocketEngine::QSocks5SocketEngine(QObject *parent)
:QAbstractSocketEngine(*new QSocks5SocketEnginePrivate(), parent)
{
}

QSocks5SocketEngine::~QSocks5SocketEngine()
{
    Q_D(QSocks5SocketEngine);

    if (d->data) {
        delete d->data->authenticator;
        delete d->data->controlSocket;
    }
    if (d->connectData)
        delete d->connectData;
#ifndef QT_NO_UDPSOCKET
    if (d->udpData) {
        delete d->udpData->udpSocket;
        delete d->udpData;
    }
#endif
    if (d->bindData)
        delete d->bindData;
}

static QBasicAtomicInt descriptorCounter = Q_BASIC_ATOMIC_INITIALIZER(1);

bool QSocks5SocketEngine::initialize(QAbstractSocket::SocketType type, QAbstractSocket::NetworkLayerProtocol protocol)
{
    Q_D(QSocks5SocketEngine);

    d->socketDescriptor = descriptorCounter.fetchAndAddRelaxed(1);

    d->socketType = type;
    d->socketProtocol = protocol;

    return true;
}

bool QSocks5SocketEngine::initialize(int socketDescriptor, QAbstractSocket::SocketState socketState)
{
    Q_D(QSocks5SocketEngine);

    QSOCKS5_Q_DEBUG << "initialize" << socketDescriptor;

    // this is only valid for the other side of a bind, nothing else is supported

    if (socketState != QAbstractSocket::ConnectedState) {
        //### must be connected state ???
        return false;
    }

    QSocks5BindData *bindData = socks5BindStore()->retrieve(socketDescriptor);
    if (bindData) {

        d->socketState = socketState;
        d->socketType = QAbstractSocket::TcpSocket;
        d->connectData = new QSocks5ConnectData;
        d->data = d->connectData;
        d->mode = QSocks5SocketEnginePrivate::ConnectMode;
        d->data->controlSocket = bindData->controlSocket;
        bindData->controlSocket = 0;
        d->data->controlSocket->setParent(this);
        d->socketProtocol = d->data->controlSocket->localAddress().protocol();
        d->data->authenticator = bindData->authenticator;
        bindData->authenticator = 0;
        d->localPort = bindData->localPort;
        d->localAddress = bindData->localAddress;
        d->peerPort = bindData->peerPort;
        d->peerAddress = bindData->peerAddress;
        delete bindData;

        QObject::connect(d->data->controlSocket, SIGNAL(connected()), this, SLOT(_q_controlSocketConnected()),
                         Qt::DirectConnection);
        QObject::connect(d->data->controlSocket, SIGNAL(readyRead()), this, SLOT(_q_controlSocketReadNotification()),
                         Qt::DirectConnection);
        QObject::connect(d->data->controlSocket, SIGNAL(bytesWritten(qint64)), this, SLOT(_q_controlSocketBytesWritten()),
                         Qt::DirectConnection);
        QObject::connect(d->data->controlSocket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(_q_controlSocketError(QAbstractSocket::SocketError)),
                         Qt::DirectConnection);
        QObject::connect(d->data->controlSocket, SIGNAL(disconnected()), this, SLOT(_q_controlSocketDisconnected()),
                         Qt::DirectConnection);
        QObject::connect(d->data->controlSocket, SIGNAL(stateChanged(QAbstractSocket::SocketState)),
                         this, SLOT(_q_controlSocketStateChanged(QAbstractSocket::SocketState)),
                         Qt::DirectConnection);

        d->socks5State = QSocks5SocketEnginePrivate::Connected;

        if (d->data->controlSocket->bytesAvailable() != 0)
            d->_q_controlSocketReadNotification();
        return true;
    }
    return false;
}

void QSocks5SocketEngine::setProxy(const QNetworkProxy &networkProxy)
{
    Q_D(QSocks5SocketEngine);
    d->proxyInfo = networkProxy;
}

int QSocks5SocketEngine::socketDescriptor() const
{
    Q_D(const QSocks5SocketEngine);
    return d->socketDescriptor;
}

bool QSocks5SocketEngine::isValid() const
{
    Q_D(const QSocks5SocketEngine);
    return d->socketType != QAbstractSocket::UnknownSocketType
           && d->socketProtocol != QAbstractSocket::UnknownNetworkLayerProtocol
           && d->socks5State != QSocks5SocketEnginePrivate::SocksError
           && (d->socketError == QAbstractSocket::UnknownSocketError
               || d->socketError == QAbstractSocket::SocketTimeoutError);
}

bool QSocks5SocketEngine::connectToHost(const QHostAddress &address, quint16 port)
{
    Q_D(QSocks5SocketEngine);

    QSOCKS5_DEBUG << "connectToHost" << address << ":" << port;

    if (!d->data) {
        if (socketType() == QAbstractSocket::TcpSocket) {
            d->initialize(QSocks5SocketEnginePrivate::ConnectMode);
#ifndef QT_NO_UDPSOCKET
        } else if (socketType() == QAbstractSocket::UdpSocket) {
            d->initialize(QSocks5SocketEnginePrivate::UdpAssociateMode);
            // all udp needs to be bound
            if (!bind(QHostAddress(QLatin1String("0.0.0.0")), 0))
                return false;
            d->peerAddress = address;
            d->peerPort = port;
            setState(QAbstractSocket::ConnectedState);
            return true;
#endif
        } else {
            //### something invalied
            return false;
        }
    }
    if (d->socks5State == QSocks5SocketEnginePrivate::unInitialized && d->socketState != QAbstractSocket::ConnectingState) {
        setPeerAddress(address);
        setPeerPort(port);
        setState(QAbstractSocket::ConnectingState);
        d->data->controlSocket->connectToHost(d->proxyInfo.hostName(), d->proxyInfo.port());
        return false;
    } else if (d->socks5State == QSocks5SocketEnginePrivate::RequestSuccess) {
        setState(QAbstractSocket::ConnectedState);
        d->socks5State = QSocks5SocketEnginePrivate::Connected;
        // check for pending data
        if (d->data->controlSocket->bytesAvailable())
            d->_q_controlSocketReadNotification();
        return true;
    } else if (d->socks5State == QSocks5SocketEnginePrivate::RequestError) {
        setError(s5RAsSocketError(d->socks5Error), makeErrorString(d->socks5ErrorString));
        setState(QAbstractSocket::UnconnectedState);
        return false;
    } else if (d->socks5State == QSocks5SocketEnginePrivate::ConnectError) {
        setError(d->data->controlSocket->error(), d->data->controlSocket->errorString());
        setState(QAbstractSocket::UnconnectedState);
        return false;
    } else if (d->socks5State == QSocks5SocketEnginePrivate::AuthenticatingError) {
        setState(QAbstractSocket::UnconnectedState);
        return false;
    } else if (d->socketState == QAbstractSocket::ConnectingState && d->socks5State != QSocks5SocketEnginePrivate::RequestSuccess) {
        QSOCKS5_DEBUG << "not yet connected";
        return false;
    } else {
        // qDebug() << "unexpected call to contectToHost";
    }
    return false;
}

void QSocks5SocketEnginePrivate::_q_controlSocketConnected()
{
    QSOCKS5_DEBUG << "_q_controlSocketConnected";
    QByteArray buf(3, 0);
    buf[0] = S5_VERSION_5;
    buf[1] = 0x01;
    buf[2] = data->authenticator->methodId();
    data->controlSocket->write(buf);
    socks5State = AuthenticationMethodsSent;
}

void QSocks5SocketEnginePrivate::_q_controlSocketReadNotification()
{
    QSOCKS5_D_DEBUG << "_q_controlSocketReadNotification socks5state" <<  s5StateToString(socks5State)
                    << "bytes available" << data->controlSocket->bytesAvailable();

    if (data->controlSocket->bytesAvailable() == 0) {
        QSOCKS5_D_DEBUG << "########## bogus read why do we get these ... on windows only";
        return;
    }

    switch (socks5State) {
        case AuthenticationMethodsSent:
            parseAuthenticationMethodReply();
            break;
        case Authenticating:
            parseAuthenticatingReply();
            break;
        case RequestMethodSent:
            parseRequestMethodReply();
            break;
        case RequestSuccess:
            if (mode == BindMode) {
                // only get here if command is bind
                parseNewConnection();
            }
            // else in conect mode but wating for second call to connectToHost
            break;
        case Connected: {
            QByteArray buf;
            if (!data->authenticator->unSeal(data->controlSocket, &buf)) {
                // qDebug() << "unseal error maybe need to wait for more data";
            }
            if (buf.size()) {
                QSOCKS5_DEBUG << dump(buf);
                connectData->readBuffer += buf;
                emitReadNotification();
            }
            break;
        }
        default:
            QSOCKS5_DEBUG << "why a _q_controlSocketReadNotification ????";
            break;
    };
}

void QSocks5SocketEnginePrivate::_q_controlSocketBytesWritten()
{
    QSOCKS5_DEBUG << "_q_controlSocketBytesWritten";

    if (socks5State != Connected
        || (mode == ConnectMode
        && data->controlSocket->bytesToWrite()))
        return;
    emitWriteNotification();
    writeNotificationActivated = false;
}

void QSocks5SocketEnginePrivate::_q_controlSocketError(QAbstractSocket::SocketError error)
{
    QSOCKS5_D_DEBUG << "controlSocketError" << error << data->controlSocket->errorString();

    if (error == QAbstractSocket::RemoteHostClosedError) {
        // clear the read buffer in connect mode so that bytes available returns 0
        // if there already is a read notification pending then this will be porcessed first
        if (mode == ConnectMode) {
            socks5State = ControlSocketError;
            if (!readNotificationPending)
                connectData->readBuffer.clear();
            emitReadNotification();
        }
    } else if (error == QAbstractSocket::ConnectionRefusedError
        || error == QAbstractSocket::HostNotFoundError) {
        socks5State = ConnectError;
        emitWriteNotification();
    }
}

void QSocks5SocketEnginePrivate::_q_controlSocketDisconnected()
{
    QSOCKS5_D_DEBUG << "_q_controlSocketDisconnected";
}

void QSocks5SocketEnginePrivate::_q_controlSocketStateChanged(QAbstractSocket::SocketState state)
{
    QSOCKS5_D_DEBUG << "_q_controlSocketStateChanged" << state;
}

#ifndef QT_NO_UDPSOCKET
void QSocks5SocketEnginePrivate::checkForDatagrams() const
{
    // udp should be unbuffered so we need to do some polling at certain points
    if (udpData->udpSocket->hasPendingDatagrams())
        const_cast<QSocks5SocketEnginePrivate *>(this)->_q_udpSocketReadNotification();
}

void QSocks5SocketEnginePrivate::_q_udpSocketReadNotification()
{
    QSOCKS5_D_DEBUG << "_q_udpSocketReadNotification()";

    // check some state stuff
    if (!udpData->udpSocket->hasPendingDatagrams()) {
        QSOCKS5_D_DEBUG << "false read ??";
        return;
    }

    while (udpData->udpSocket->hasPendingDatagrams()) {
        QByteArray sealedBuf(udpData->udpSocket->pendingDatagramSize(), 0);
        QSOCKS5_D_DEBUG << "new datagram";
        udpData->udpSocket->readDatagram(sealedBuf.data(), sealedBuf.size());
        QByteArray inBuf;
        if (!data->authenticator->unSeal(sealedBuf, &inBuf)) {
            QSOCKS5_D_DEBUG << "failed unsealing datagram discarding";
            return;
        }
        QSOCKS5_DEBUG << dump(inBuf);
        int pos = 0;
        const char *buf = inBuf.constData();
        if (inBuf.size() < 4) {
            QSOCKS5_D_DEBUG << "bugus udp data, discarding";
            return;
        }
        QSocks5RevivedDatagram datagram;
        if (buf[pos++] != 0 || buf[pos++] != 0) {
            QSOCKS5_D_DEBUG << "invalid datagram discarding";
            return;
        }
        if (buf[pos++] != 0) { //### add fragmentation reading support
            QSOCKS5_D_DEBUG << "don't support fragmentation yet disgarding";
            return;
        }
        if (!qt_socks5_get_host_address_and_port(inBuf, &datagram.address, &datagram.port, &pos)) {
            QSOCKS5_D_DEBUG << "failed to get address from datagram disgarding";
            return;
        }
        datagram.data = QByteArray(&buf[pos], inBuf.size() - pos);
        udpData->pendingDatagrams.enqueue(datagram);
    }
    emitReadNotification();
}
#endif // QT_NO_UDPSOCKET

bool QSocks5SocketEngine::bind(const QHostAddress &address, quint16 port)
{
    Q_D(QSocks5SocketEngine);

    // when bind wee will block until the bind is finished as the info from the proxy server is needed

    if (!d->data) {
        if (socketType() == QAbstractSocket::TcpSocket) {
            d->initialize(QSocks5SocketEnginePrivate::BindMode);
#ifndef QT_NO_UDPSOCKET
        } else if (socketType() == QAbstractSocket::UdpSocket) {
            d->initialize(QSocks5SocketEnginePrivate::UdpAssociateMode);
#endif
        } else {
            //### something invalid
            return false;
        }
    }

#ifndef QT_NO_UDPSOCKET
    if (d->mode == QSocks5SocketEnginePrivate::UdpAssociateMode) {
        if (!d->udpData->udpSocket->bind(address, port)) {
            QSOCKS5_Q_DEBUG << "local udp bind failed";
            setError(d->udpData->udpSocket->error(), d->udpData->udpSocket->errorString());
            return false;
        }
        d->localAddress = d->udpData->udpSocket->localAddress();
        d->localPort = d->udpData->udpSocket->localPort();
    } else
#endif
    if (d->mode == QSocks5SocketEnginePrivate::BindMode) {
        d->localAddress = address;
        d->localPort = port;
    } else {
        //### something invalid
        return false;
    }

    int msecs = SOCKS5_BLOCKING_BIND_TIMEOUT;
    QTime stopWatch;
    stopWatch.start();
    d->data->controlSocket->connectToHost(d->proxyInfo.hostName(), d->proxyInfo.port());
    if (!d->data->controlSocket->waitForConnected(qt_timeout_value(msecs, stopWatch.elapsed()))) {
        if (d->data->controlSocket->error() != QAbstractSocket::SocketTimeoutError) {
            setError(d->data->controlSocket->error(), d->data->controlSocket->errorString());
        } else {
            setError(QAbstractSocket::SocketTimeoutError, tr("Socks5 timeout error connecting to socks server"));
        }
        QSOCKS5_Q_DEBUG << "waitForConnected to proxy server" << d->data->controlSocket->errorString();
        return false;
    }
    while (d->data->controlSocket->waitForReadyRead(qt_timeout_value(msecs, stopWatch.elapsed()))) {
        // check error
        if (d->socks5State == QSocks5SocketEnginePrivate::RequestSuccess) {
            setState(QAbstractSocket::BoundState);
#ifndef QT_NO_UDPSOCKET
            if (d->mode == QSocks5SocketEnginePrivate::UdpAssociateMode) {
                d->udpData->associateAddress = d->localAddress;
                d->localAddress = QHostAddress();
                d->udpData->associatePort = d->localPort;
                d->localPort = 0;
                QUdpSocket dummy;
                QNetworkProxy proxy;
                proxy.setType(QNetworkProxy::NoProxy);
                dummy.setProxy(proxy);
                if (!dummy.bind()
                    || writeDatagram(0,0, d->data->controlSocket->localAddress(), dummy.localPort()) != 0
                    || !dummy.waitForReadyRead(qt_timeout_value(msecs, stopWatch.elapsed()))
                    || dummy.readDatagram(0,0, &d->localAddress, &d->localPort) != 0) {
                    QSOCKS5_DEBUG << "udp actual address and port lookup failed";
                    //### reset and error
                    return false;
                }
                QSOCKS5_DEBUG << "udp actual address and port" << d->localAddress << ":" << d->localPort;
            }
#endif // QT_NO_UDPSOCKET
            return true;
        }
        QSOCKS5_DEBUG << "looping";
    }

    // binding timed out
    setError(QAbstractSocket::SocketTimeoutError,
             QLatin1String(QT_TRANSLATE_NOOP("QSocks5SocketEngine", "Network operation timed out")));

///###    delete d->udpSocket;
///###    d->udpSocket = 0;
    return false;
}


bool QSocks5SocketEngine::listen()
{
    Q_D(QSocks5SocketEngine);

    QSOCKS5_Q_DEBUG << "listen()";

    // check that we are in bound and then go to listening.
    if (d->socketState == QAbstractSocket::BoundState) {
        d->socketState = QAbstractSocket::ListeningState;

        // check if we already have a connection
        if (d->socks5State == QSocks5SocketEnginePrivate::BindSuccess)
            d->emitReadNotification();

        return true;
    }
    return false;
}

int QSocks5SocketEngine::accept()
{
    Q_D(QSocks5SocketEngine);
    // check we are listing ---

    QSOCKS5_Q_DEBUG << "accept()";

    if (d->socks5State == QSocks5SocketEnginePrivate::BindSuccess) {
        QSOCKS5_Q_DEBUG << "BindSuccess adding" << d->socketDescriptor << "to the bind store";
        d->data->controlSocket->disconnect();
        d->data->controlSocket->setParent(0);
        d->bindData->localAddress = d->localAddress;
        d->bindData->localPort = d->localPort;
        int sd = d->socketDescriptor;
        socks5BindStore()->add(sd, d->bindData);
        d->data = 0;
        d->bindData = 0;
        d->socketDescriptor = 0;
        //### do something about this socket layer ... set it closed and an error about why ...
        // reset state and local port/address
        d->socks5State = QSocks5SocketEnginePrivate::unInitialized; // ..??
        d->socketState = QAbstractSocket::UnconnectedState;
        return sd;
    } else if (d->socks5State == QSocks5SocketEnginePrivate::BindError) {
        // what now
    } else if (d->socks5State == QSocks5SocketEnginePrivate::RequestSuccess) {
        // accept was called to early ...
    }
    return -1;
}

void QSocks5SocketEngine::close()
{
    QSOCKS5_Q_DEBUG << "close()";
    Q_D(QSocks5SocketEngine);
    if (d->data && d->data->controlSocket) {
        if (d->data->controlSocket->state() == QAbstractSocket::ConnectedState) {
            int msecs = 100;
            QTime stopWatch;
            stopWatch.start();
            while (!d->data->controlSocket->bytesToWrite()) {
               if (!d->data->controlSocket->waitForBytesWritten(qt_timeout_value(msecs, stopWatch.elapsed())))
                   break;
            }
        }
        d->data->controlSocket->close();
    }
#ifndef QT_NO_UDPSOCKET
    if (d->udpData && d->udpData->udpSocket)
        d->udpData->udpSocket->close();
#endif
}

qint64 QSocks5SocketEngine::bytesAvailable() const
{
    Q_D(const QSocks5SocketEngine);
    if (d->mode == QSocks5SocketEnginePrivate::ConnectMode)
        return d->connectData->readBuffer.size();
#ifndef QT_NO_UDPSOCKET
    else if (d->mode == QSocks5SocketEnginePrivate::UdpAssociateMode
             && !d->udpData->pendingDatagrams.isEmpty())
        return d->udpData->pendingDatagrams.first().data.size();
#endif
    return 0;
}

qint64 QSocks5SocketEngine::read(char *data, qint64 maxlen)
{
    Q_D(QSocks5SocketEngine);
    QSOCKS5_Q_DEBUG << "read( , maxlen = " << maxlen << ")";
    if (d->mode == QSocks5SocketEnginePrivate::ConnectMode) {
        if (d->connectData->readBuffer.size() == 0) {
            if (d->data->controlSocket->state() == QAbstractSocket::UnconnectedState) {
                //imitate remote closed
                close();
                setError(QAbstractSocket::RemoteHostClosedError,
                         QLatin1String("Remote host closed connection###"));
                setState(QAbstractSocket::UnconnectedState);
                return -1;
            } else {
                return 0;       // nothing to be read
            }
        }
        qint64 copy = qMin<qint64>(d->connectData->readBuffer.size(), maxlen);
        memcpy(data, d->connectData->readBuffer.constData(), copy);
        d->connectData->readBuffer.remove(0, copy);
        QSOCKS5_DEBUG << "read" << dump(QByteArray(data, copy));
        return copy;
#ifndef QT_NO_UDPSOCKET
    } else if (d->mode == QSocks5SocketEnginePrivate::UdpAssociateMode) {
        return readDatagram(data, maxlen);
#endif
    }
    return 0;
}

qint64 QSocks5SocketEngine::write(const char *data, qint64 len)
{
    Q_D(QSocks5SocketEngine);
    QSOCKS5_Q_DEBUG << "write" << dump(QByteArray(data, len));

    if (d->mode == QSocks5SocketEnginePrivate::ConnectMode) {

        int msecs = 10;
        QTime stopWatch;
        stopWatch.start();
        qint64 totalWritten = 0;

        while (totalWritten < len
            && stopWatch.elapsed() < msecs) {

            QByteArray buf(data + totalWritten, qMin<int>(len - totalWritten, 49152));
            QByteArray sealedBuf;
            if (!d->data->authenticator->seal(buf, &sealedBuf)) {
                // ### Handle this error.
            }
            int written = d->data->controlSocket->write(sealedBuf);
            if (written != sealedBuf.size()) {
                QSOCKS5_Q_DEBUG << "control socket write failed :" << d->data->controlSocket->errorString();
                setError(d->data->controlSocket->error(), d->data->controlSocket->errorString());
                return -1; //### ?????
            }
            totalWritten += buf.size();
            while(d->data->controlSocket->bytesToWrite()) {
                if (!d->data->controlSocket->waitForBytesWritten(qt_timeout_value(msecs, stopWatch.elapsed()))) {
                    QSOCKS5_Q_DEBUG << "controlSocket->waitForBytesWritten() retruned false";
                    break;
                }
            }
            if (d->data->controlSocket->error() != QAbstractSocket::UnknownSocketError
                && d->data->controlSocket->error() != QAbstractSocket::SocketTimeoutError) {
                QSOCKS5_DEBUG << "control socket error while writing. -- " << d->data->controlSocket->errorString();
                totalWritten = -1;
                break;
            }
        }
        QSOCKS5_DEBUG << "wrote" << totalWritten;
        return totalWritten;
#ifndef QT_NO_UDPSOCKET
    } else if (d->mode == QSocks5SocketEnginePrivate::UdpAssociateMode) {
        // send to connected address
        return writeDatagram(data, len, d->peerAddress, d->peerPort);
#endif
    }
    //### set an error ???
    return -1;
}

#ifndef QT_NO_UDPSOCKET
qint64 QSocks5SocketEngine::readDatagram(char *data, qint64 maxlen, QHostAddress *addr,
                                        quint16 *port)
{
    Q_D(QSocks5SocketEngine);

    d->checkForDatagrams();

    if (d->udpData->pendingDatagrams.isEmpty())
        return 0;

    QSocks5RevivedDatagram datagram = d->udpData->pendingDatagrams.dequeue();
    int copyLen = qMin<int>(maxlen, datagram.data.size());
    memcpy(data, datagram.data.constData(), copyLen);
    if (addr)
        *addr = datagram.address;
    if (port)
        *port = datagram.port;
    return copyLen;
}

qint64 QSocks5SocketEngine::writeDatagram(const char *data, qint64 len, const QHostAddress &address,
                                         quint16 port)
{
    Q_D(QSocks5SocketEngine);

    // it is possible to send with out first binding with udp, but socks5 requires a bind.
    if (!d->data) {
        d->initialize(QSocks5SocketEnginePrivate::UdpAssociateMode);
        // all udp needs to be bound
        if (!bind(QHostAddress(QLatin1String("0.0.0.0")), 0)) {
            //### set error
            return -1;
        }
    }

    QByteArray outBuf;
    outBuf.reserve(270 + len);
    int pos = 0;
    outBuf[pos++] = 0x00;
    outBuf[pos++] = 0x00;
    outBuf[pos++] = 0x00;
    if (!qt_socks5_set_host_address_and_port(address, port, &outBuf, &pos)) {
    }
    outBuf += QByteArray(data, len);
    QSOCKS5_DEBUG << "sending" << dump(outBuf);
    QByteArray sealedBuf;
    if (!d->data->authenticator->seal(outBuf, &sealedBuf)) {
        QSOCKS5_DEBUG << "sealing data failed";
        setError(QAbstractSocket::SocketAccessError, d->data->authenticator->errorString());
        return -1;
    }
    if (d->udpData->udpSocket->writeDatagram(sealedBuf, d->udpData->associateAddress, d->udpData->associatePort) != sealedBuf.size()) {
        //### try frgamenting
        if (d->udpData->udpSocket->error() == QAbstractSocket::DatagramTooLargeError)
            setError(d->udpData->udpSocket->error(), d->udpData->udpSocket->errorString());
        //### else maybe more serious error
        return -1;
    }

    return len;
}

bool QSocks5SocketEngine::hasPendingDatagrams() const
{
    Q_D(const QSocks5SocketEngine);
    Q_INIT_CHECK(false);

    d->checkForDatagrams();

    return !d->udpData->pendingDatagrams.isEmpty();
}

qint64 QSocks5SocketEngine::pendingDatagramSize() const
{
    Q_D(const QSocks5SocketEngine);

    d->checkForDatagrams();

    if (!d->udpData->pendingDatagrams.isEmpty())
        return d->udpData->pendingDatagrams.head().data.size();
    return 0;
}
#endif // QT_NO_UDPSOCKET

int QSocks5SocketEngine::option(SocketOption option) const
{
    Q_UNUSED(option);
    return -1;
}

bool QSocks5SocketEngine::setOption(SocketOption option, int value)
{
    Q_UNUSED(option);
    Q_UNUSED(value);
    return false;
}

bool QSocks5SocketEngine::waitForRead(int msecs, bool *timedOut) const
{
    Q_D(const QSocks5SocketEngine);
    QSOCKS5_DEBUG << "waitForRead" << msecs;

    d->readNotificationActivated = false;

    QTime stopWatch;
    stopWatch.start();

    if (socketType() == QAbstractSocket::TcpSocket) {
        // check for pending data
        if (d->data->controlSocket->bytesAvailable())
            const_cast<QSocks5SocketEnginePrivate*>(d)->_q_controlSocketReadNotification();

        bool success = true;
        while (!d->readNotificationActivated && (success = d->data->controlSocket->waitForReadyRead(qt_timeout_value(msecs, stopWatch.elapsed())))) {
            QSOCKS5_DEBUG << "looping";
        }
        if (!success) {
            setError(d->data->controlSocket->error(), d->data->controlSocket->errorString());
            if (timedOut && d->data->controlSocket->error() == QAbstractSocket::SocketTimeoutError)
                *timedOut = true;
            if (d->data->controlSocket->state() == QAbstractSocket::UnconnectedState)
                d->readNotificationActivated = true;
        }
#ifndef QT_NO_UDPSOCKET
    } else {
        // what about if the tcp socket is disconnected ...
        while (!d->readNotificationActivated && d->udpData->udpSocket->waitForReadyRead(qt_timeout_value(msecs, stopWatch.elapsed()))) {
            QSOCKS5_DEBUG << "looping";
        }
        if (d->udpData->udpSocket->error() != QAbstractSocket::UnknownSocketError) {
            setError(d->udpData->udpSocket->error(), d->udpData->udpSocket->errorString());
            if (timedOut && d->udpData->udpSocket->error() == QAbstractSocket::SocketTimeoutError)
                *timedOut = true;
        }
#endif // QT_NO_UDPSOCKET
    }


    bool ret = d->readNotificationActivated;
    d->readNotificationActivated = false;

    QSOCKS5_DEBUG << "waitForRead returned" << ret;
    return ret;
}


bool QSocks5SocketEngine::waitForWrite(int msecs, bool *timedOut) const
{
    Q_D(const QSocks5SocketEngine);
    QSOCKS5_DEBUG << "waitForWrite" << msecs;

    if (d->socketState == QAbstractSocket::ConnectingState) {
        d->writeNotificationActivated = false;

        QSOCKS5_DEBUG << "waitForWrite ... waiting for connected";
        QTime stopWatch;
        stopWatch.start();

        if (!d->data->controlSocket->waitForConnected(qt_timeout_value(msecs, stopWatch.elapsed()))) {
            // qDebug() << "failed to connect to proxy";
            setError(d->data->controlSocket->error(), d->data->controlSocket->errorString());
            if (timedOut && d->data->controlSocket->error() == QAbstractSocket::SocketTimeoutError)
                *timedOut = true;
            return false;
        }
        QSOCKS5_DEBUG << "waitForWrite ... waiting for proxy init" << msecs;
        while (!d->writeNotificationActivated
               && d->data->controlSocket->waitForReadyRead(qt_timeout_value(msecs, stopWatch.elapsed()))) {
            QSOCKS5_DEBUG << "looping";
        }
        if (timedOut && d->data->controlSocket->error() == QAbstractSocket::SocketTimeoutError) {
            QSOCKS5_DEBUG << "timeout";
            *timedOut = true;
        }

        bool ret = d->writeNotificationActivated;
        d->writeNotificationActivated = false;

        return ret;
    }

    // probably just return true unless we are not set up ??
    if (d->socketState == QAbstractSocket::ConnectedState) {
        if (d->mode == QSocks5SocketEnginePrivate::ConnectMode) {
            //### check for time out;
            while(d->data->controlSocket->bytesToWrite())
                d->data->controlSocket->waitForBytesWritten();
        }
        return true;
    }
    return false;
}


bool QSocks5SocketEngine::waitForReadOrWrite(bool *readyToRead, bool *readyToWrite,
                                            bool checkRead, bool checkWrite,
                                            int msecs, bool *timedOut) const
{
    Q_UNUSED(checkRead);
    if (!checkWrite) {
        bool canRead = waitForRead(msecs, timedOut);
        if (readyToRead)
            *readyToRead = canRead;
        return canRead;
    }

    bool canWrite = waitForWrite(msecs, timedOut);
    if (readyToWrite)
        *readyToWrite = canWrite;
    return canWrite;
}

bool QSocks5SocketEngine::isReadNotificationEnabled() const
{
    Q_D(const QSocks5SocketEngine);
    return d->readNotificationEnabled;
}

void QSocks5SocketEngine::setReadNotificationEnabled(bool enable)
{
    Q_D(QSocks5SocketEngine);

    QSOCKS5_Q_DEBUG << "setReadNotificationEnabled(" << enable << ")";

    bool emitSignal = false;
    if (!d->readNotificationEnabled
        && enable) {
        if (d->mode == QSocks5SocketEnginePrivate::ConnectMode)
            emitSignal = !d->connectData->readBuffer.isEmpty();
#ifndef QT_NO_UDPSOCKET
        else if (d->mode == QSocks5SocketEnginePrivate::UdpAssociateMode)
            emitSignal = !d->udpData->pendingDatagrams.isEmpty();
#endif
        else if (d->mode == QSocks5SocketEnginePrivate::BindMode
            && d->socketState == QAbstractSocket::ListeningState
            && d->socks5State == QSocks5SocketEnginePrivate::BindSuccess)
            emitSignal = true;
    }

    d->readNotificationEnabled = enable;

    if (emitSignal)
        d->emitReadNotification();
}

bool QSocks5SocketEngine::isWriteNotificationEnabled() const
{
    Q_D(const QSocks5SocketEngine);
    return d->writeNotificationEnabled;
}

void QSocks5SocketEngine::setWriteNotificationEnabled(bool enable)
{
    Q_D(QSocks5SocketEngine);
    d->writeNotificationEnabled = enable;
    if (enable && d->socketState == QAbstractSocket::ConnectedState) {
        if (d->mode == QSocks5SocketEnginePrivate::ConnectMode && d->data->controlSocket->bytesToWrite())
            return; // will be emitted as a result of bytes written
       d->emitWriteNotification();
       d->writeNotificationActivated = false;
    }
}

bool QSocks5SocketEngine::isExceptionNotificationEnabled() const
{
    Q_D(const QSocks5SocketEngine);
    return d->exceptNotificationEnabled;
}

void QSocks5SocketEngine::setExceptionNotificationEnabled(bool enable)
{
    Q_D(QSocks5SocketEngine);
    d->exceptNotificationEnabled = enable;
}

QAbstractSocketEngine *QSocks5SocketEngineHandler::createSocketEngine(const QHostAddress &address, QAbstractSocket::SocketType socketType, QObject *parent)
{
    Q_UNUSED(socketType);

    QSOCKS5_DEBUG << "createSocketEngine" << address;

    if (address == QHostAddress::LocalHost || address == QHostAddress::LocalHostIPv6) {
        QSOCKS5_DEBUG << "not proxying";
        return 0;
    }

    QNetworkProxy proxy;
    // find proxy info
    if (qobject_cast<QAbstractSocket *>(parent)) {
        QAbstractSocket *abstractSocket = qobject_cast<QAbstractSocket *>(parent);
        if (abstractSocket->proxy().type() != QNetworkProxy::DefaultProxy)
            proxy = abstractSocket->proxy();
    } else if (qobject_cast<QTcpServer *>(parent)) {
        QTcpServer *server = qobject_cast<QTcpServer *>(parent);
        if (server->proxy().type() != QNetworkProxy::DefaultProxy)
            proxy = server->proxy();
    }
    if (proxy.type() == QNetworkProxy::DefaultProxy) {
        proxy = QNetworkProxy::applicationProxy();
    }
    if (proxy.type() == QNetworkProxy::DefaultProxy || proxy.type() == QNetworkProxy::NoProxy || proxy.type() != QNetworkProxy::Socks5Proxy) {
        QSOCKS5_DEBUG << "not proxying";
        return 0;
    }
    QSOCKS5_DEBUG << "use proxy for" << address;
    QSocks5SocketEngine *engine = new QSocks5SocketEngine(parent);
    engine->setProxy(proxy);
    return engine;
}

QAbstractSocketEngine *QSocks5SocketEngineHandler::createSocketEngine(int socketDescriptor, QObject *parent)
{
    QSOCKS5_DEBUG << "createSocketEngine" << socketDescriptor;
    if (socks5BindStore()->contains(socketDescriptor)) {
        QSOCKS5_DEBUG << "bind store contains" << socketDescriptor;
        return new QSocks5SocketEngine(parent);
    }
    return 0;
}

#endif // QT_NO_SOCKS5

QT_END_NAMESPACE
