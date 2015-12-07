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

#include "qnetworkaccessftpbackend_p.h"
#include "qnetworkaccessmanager_p.h"
#include "QtNetwork/qauthenticator.h"

#ifndef QT_NO_FTP

QT_BEGIN_NAMESPACE

enum {
    DefaultFtpPort = 21
};

static QByteArray makeCacheKey(const QUrl &url)
{
    QUrl copy = url;
    copy.setPort(url.port(DefaultFtpPort));
    return "ftp-connection:" +
        copy.toEncoded(QUrl::RemovePassword | QUrl::RemovePath | QUrl::RemoveQuery |
                       QUrl::RemoveFragment);
}

QNetworkAccessBackend *
QNetworkAccessFtpBackendFactory::create(QNetworkAccessManager::Operation op,
                                        const QNetworkRequest &request) const
{
    // is it an operation we know of?
    switch (op) {
    case QNetworkAccessManager::GetOperation:
    case QNetworkAccessManager::PutOperation:
        break;

    default:
        // no, we can't handle this operation
        return 0;
    }

    QUrl url = request.url();
    if (url.scheme() == QLatin1String("ftp"))
        return new QNetworkAccessFtpBackend;
    return 0;
}

class QNetworkAccessFtpIODevice: public QIODevice
{
    //Q_OBJECT
public:
    QNetworkAccessFtpBackend *backend;
    bool eof;

    inline QNetworkAccessFtpIODevice(QNetworkAccessFtpBackend *parent)
        : QIODevice(parent), backend(parent), eof(false)
        { open(ReadOnly); }

    bool isSequential() const { return true; }
    bool atEnd() const { return backend->upstreamBytesAvailable() == 0; }

    qint64 bytesAvailable() const { return backend->upstreamBytesAvailable(); }
    qint64 bytesToWrite() const { return backend->downstreamBytesToConsume(); }
protected:
    qint64 readData(char *data, qint64 maxlen)
    {
        const QByteArray toSend = backend->readUpstream();
        maxlen = qMin<qint64>(maxlen, toSend.size());
        if (!maxlen)
            return eof ? -1 : 0;

        backend->upstreamBytesConsumed(maxlen);
        memcpy(data, toSend.constData(), maxlen);
        return maxlen;
    }

    qint64 writeData(const char *, qint64)
    { return -1; }

    friend class QNetworkAccessFtpBackend;
};

class QNetworkAccessFtpFtp: public QFtp, public QNetworkAccessCache::CacheableObject
{
    // Q_OBJECT
public:
    QNetworkAccessFtpFtp()
    {
        setExpires(true);
        setShareable(false);
    }

    void dispose()
    {
        connect(this, SIGNAL(done(bool)), this, SLOT(deleteLater()));
        close();
    }
};

QNetworkAccessFtpBackend::QNetworkAccessFtpBackend()
    : ftp(0), uploadDevice(0), totalBytes(0), helpId(-1), sizeId(-1), mdtmId(-1),
    supportsSize(false), supportsMdtm(false), state(Idle)
{
}

QNetworkAccessFtpBackend::~QNetworkAccessFtpBackend()
{
    disconnectFromFtp();
}

void QNetworkAccessFtpBackend::open()
{
#ifndef QT_NO_NETWORKPROXY
    QNetworkProxy proxy;
    foreach (const QNetworkProxy &p, proxyList()) {
        // use the first FTP proxy
        // or no proxy at all
        if (p.type() == QNetworkProxy::FtpCachingProxy
            || p.type() == QNetworkProxy::NoProxy) {
            proxy = p;
            break;
        }
    }

    // did we find an FTP proxy or a NoProxy?
    if (proxy.type() == QNetworkProxy::DefaultProxy) {
        // unsuitable proxies
        error(QNetworkReply::ProxyNotFoundError,
              tr("No suitable proxy found"));
        finished();
        return;
    }

#endif

    QUrl url = this->url();
    if (url.path().isEmpty()) {
        url.setPath(QLatin1String("/"));
        setUrl(url);
    }
    if (url.path().endsWith(QLatin1Char('/'))) {
        error(QNetworkReply::ContentOperationNotPermittedError,
              tr("Cannot open %1: is a directory").arg(url.toString()));
        finished();
        return;
    }
    state = LoggingIn;

    QNetworkAccessCache* cache = QNetworkAccessManagerPrivate::getCache(this);
    QByteArray cacheKey = makeCacheKey(url);
    if (!cache->requestEntry(cacheKey, this,
                             SLOT(ftpConnectionReady(QNetworkAccessCache::CacheableObject*)))) {
        ftp = new QNetworkAccessFtpFtp;
#ifndef QT_NO_NETWORKPROXY
        if (proxy.type() == QNetworkProxy::FtpCachingProxy)
            ftp->setProxy(proxy.hostName(), proxy.port());
#endif
        ftp->connectToHost(url.host(), url.port(DefaultFtpPort));
        ftp->login(url.userName(), url.password());

        cache->addEntry(cacheKey, ftp);
        ftpConnectionReady(ftp);
    }

    uploadDevice = new QNetworkAccessFtpIODevice(this);
}

void QNetworkAccessFtpBackend::closeDownstreamChannel()
{
    state = Disconnecting;
    if (operation() == QNetworkAccessManager::GetOperation)
#ifndef Q_OS_WINCE
        abort();
#else
        exit(3);
#endif
}

void QNetworkAccessFtpBackend::closeUpstreamChannel()
{
    if (operation() == QNetworkAccessManager::PutOperation) {
        Q_ASSERT(uploadDevice);
        uploadDevice->eof = true;
        if (!upstreamBytesAvailable())
            emit uploadDevice->readyRead();
    }
}

bool QNetworkAccessFtpBackend::waitForDownstreamReadyRead(int ms)
{
    if (!ftp)
        return false;

    if (ftp->bytesAvailable()) {
        ftpReadyRead();
        return true;
    }

    if (ms == 0)
        return false;

    qCritical("QNetworkAccess: FTP backend does not support waitForReadyRead()");
    return false;
}

bool QNetworkAccessFtpBackend::waitForUpstreamBytesWritten(int ms)
{
    Q_UNUSED(ms);
    qCritical("QNetworkAccess: FTP backend does not support waitForBytesWritten()");
    return false;
}

void QNetworkAccessFtpBackend::upstreamReadyRead()
{
    // uh... how does QFtp operate?
}

void QNetworkAccessFtpBackend::downstreamReadyWrite()
{
    if (state == Transferring && ftp && ftp->bytesAvailable())
        ftpReadyRead();
}

void QNetworkAccessFtpBackend::ftpConnectionReady(QNetworkAccessCache::CacheableObject *o)
{
    ftp = static_cast<QNetworkAccessFtpFtp *>(o);
    connect(ftp, SIGNAL(done(bool)), SLOT(ftpDone()));
    connect(ftp, SIGNAL(rawCommandReply(int,QString)), SLOT(ftpRawCommandReply(int,QString)));
    connect(ftp, SIGNAL(readyRead()), SLOT(ftpReadyRead()));

    // is the login process done already?
    if (ftp->state() == QFtp::LoggedIn)
        ftpDone();

    // no, defer the actual operation until after we've logged in
}

void QNetworkAccessFtpBackend::disconnectFromFtp()
{
    state = Disconnecting;

    if (ftp) {
        disconnect(ftp, 0, this, 0);

        QByteArray key = makeCacheKey(url());
        QNetworkAccessManagerPrivate::getCache(this)->releaseEntry(key);

        ftp = 0;
    }
}

void QNetworkAccessFtpBackend::ftpDone()
{
    // the last command we sent is done
    if (state == LoggingIn && ftp->state() != QFtp::LoggedIn) {
        if (ftp->state() == QFtp::Connected) {
            // the login did not succeed
            QUrl newUrl = url();
            newUrl.setUserInfo(QString());
            setUrl(newUrl);

            QAuthenticator auth;
            authenticationRequired(&auth);

            if (!auth.isNull()) {
                // try again:
                newUrl.setUserName(auth.user());
                setUrl(newUrl);
                ftp->login(auth.user(), auth.password());
                return;
            }

            error(QNetworkReply::AuthenticationRequiredError,
                  tr("Logging in to %1 failed: authentication required")
                  .arg(url().host()));
        } else {
            // we did not connect
            QNetworkReply::NetworkError code;
            switch (ftp->error()) {
            case QFtp::HostNotFound:
                code = QNetworkReply::HostNotFoundError;
                break;

            case QFtp::ConnectionRefused:
                code = QNetworkReply::ConnectionRefusedError;
                break;

            default:
                code = QNetworkReply::ProtocolFailure;
                break;
            }

            error(code, ftp->errorString());
        }

        // we're not connected, so remove the cache entry:
        QByteArray key = makeCacheKey(url());
        QNetworkAccessManagerPrivate::getCache(this)->removeEntry(key);

        disconnect(ftp, 0, this, 0);
        ftp->dispose();
        ftp = 0;

        state = Disconnecting;
        finished();
        return;
    }

    // check for errors:
    if (ftp->error() != QFtp::NoError) {
        QString msg;
        if (operation() == QNetworkAccessManager::GetOperation)
            msg = tr("Error while downloading %1: %2");
        else
            msg = tr("Error while uploading %1: %2");
        msg = msg.arg(url().toString(), ftp->errorString());

        if (state == Statting)
            // file probably doesn't exist
            error(QNetworkReply::ContentNotFoundError,  msg);
        else
            error(QNetworkReply::ContentAccessDenied, msg);

        disconnectFromFtp();
        finished();
    }

    if (state == LoggingIn) {
        state = CheckingFeatures;
        if (operation() == QNetworkAccessManager::GetOperation) {
            // send help command to find out if server supports "SIZE" and "MDTM"
            QString command = url().path();
            command.prepend(QLatin1String("%1 "));
            helpId = ftp->rawCommand(QLatin1String("HELP")); // get supported commands
        } else {
            ftpDone();
        }
    } else if (state == CheckingFeatures) {
        state = Statting;
        if (operation() == QNetworkAccessManager::GetOperation) {
            // logged in successfully, send the stat requests (if supported)
            QString command = url().path();
            command.prepend(QLatin1String("%1 "));
            if (supportsSize)
                sizeId = ftp->rawCommand(command.arg(QLatin1String("SIZE"))); // get size
            if (supportsMdtm)
                mdtmId = ftp->rawCommand(command.arg(QLatin1String("MDTM"))); // get modified time
            if (!supportsSize && !supportsMdtm)
                ftpDone();      // no commands sent, move to the next state
        } else {
            ftpDone();
        }
    } else if (state == Statting) {
        // statted successfully, send the actual request
        emit metaDataChanged();
        state = Transferring;

        QFtp::TransferType type = QFtp::Binary;
        if (operation() == QNetworkAccessManager::GetOperation) {
            setCachingEnabled(true);
            ftp->get(url().path(), 0, type);
        } else {
            ftp->put(uploadDevice, url().path(), type);
        }

    } else if (state == Transferring) {
        // upload or download finished
        disconnectFromFtp();
        finished();
    }
}

void QNetworkAccessFtpBackend::ftpReadyRead()
{
    writeDownstreamData(ftp->readAll());
}

void QNetworkAccessFtpBackend::ftpRawCommandReply(int code, const QString &text)
{
    //qDebug() << "FTP reply:" << code << text;
    int id = ftp->currentId();

    if ((id == helpId) && ((code == 200) || (code == 214))) {     // supported commands
        // the "FEAT" ftp command would be nice here, but it is not part of the
        // initial FTP RFC 959, neither ar "SIZE" nor "MDTM" (they are all specified
        // in RFC 3659)
        if (text.contains(QLatin1String("SIZE"), Qt::CaseSensitive))
            supportsSize = true;
        if (text.contains(QLatin1String("MDTM"), Qt::CaseSensitive))
            supportsMdtm = true;
    } else if (code == 213) {          // file status
        if (id == sizeId) {
            // reply to the size command
            setHeader(QNetworkRequest::ContentLengthHeader, text.toLongLong());
#ifndef QT_NO_DATESTRING
        } else if (id == mdtmId) {
            QDateTime dt = QDateTime::fromString(text, QLatin1String("yyyyMMddHHmmss"));
            setHeader(QNetworkRequest::LastModifiedHeader, dt);
#endif
        }
    }
}

QT_END_NAMESPACE

#endif // QT_NO_FTP
