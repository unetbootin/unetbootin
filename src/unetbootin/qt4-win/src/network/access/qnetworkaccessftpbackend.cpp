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
    : ftp(0), uploadDevice(0), totalBytes(0), sizeId(-1), mdtmId(-1), state(Idle)
{
}

QNetworkAccessFtpBackend::~QNetworkAccessFtpBackend()
{
    if (ftp) {
        ftp->close();
        QByteArray key = url().toEncoded(QUrl::RemovePassword | QUrl::RemovePath |
                                         QUrl::RemoveQuery | QUrl::RemoveFragment);
        key.prepend("ftp-connection:");
        QNetworkAccessManagerPrivate::getCache(this)->removeEntry(key);
    }
}

void QNetworkAccessFtpBackend::open()
{
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
        QNetworkProxy p = proxy();
        if (p.type() == QNetworkProxy::FtpCachingProxy)
            ftp->setProxy(p.hostName(), p.port());
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

    if(ftp) {
        disconnect(ftp, 0, this, 0);
        ftp = 0;
    }

    QByteArray key = makeCacheKey(url());
    QNetworkAccessManagerPrivate::getCache(this)->releaseEntry(key);
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
        state = Statting;

        if (operation() == QNetworkAccessManager::GetOperation) {
            // logged in successfully, send the stat requests
            QString command = url().path();
            command.prepend(QLatin1String("%1 "));

            sizeId = ftp->rawCommand(command.arg(QLatin1String("SIZE"))); // get size
            mdtmId = ftp->rawCommand(command.arg(QLatin1String("MDTM"))); // get modified time
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
    if (code == 213) {          // file status
        int id = ftp->currentId();

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
