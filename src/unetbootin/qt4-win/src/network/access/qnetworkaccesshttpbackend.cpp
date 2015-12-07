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

#include "qnetworkaccesshttpbackend_p.h"
#include "qnetworkaccessmanager_p.h"
#include "qnetworkaccesscache_p.h"
#include "qnetworkrequest.h"
#include "qnetworkreply.h"
#include "qnetworkrequest_p.h"
#include "QtCore/qdatetime.h"
#include "QtNetwork/qsslconfiguration.h"

#ifndef QT_NO_HTTP

#include <string.h>             // for strchr

QT_BEGIN_NAMESPACE

enum {
    DefaultHttpPort = 80,
    DefaultHttpsPort = 443
};

static QByteArray makeCacheKey(const QUrl &url)
{
    QUrl copy = url;
    bool isEncrypted = url.scheme() == QLatin1String("https");
    copy.setPort(url.port(isEncrypted ? DefaultHttpsPort : DefaultHttpPort));
    return "http-connection:" + copy.toEncoded(QUrl::RemovePassword | QUrl::RemovePath |
                                               QUrl::RemoveQuery | QUrl::RemoveFragment);
}

QNetworkAccessBackend *
QNetworkAccessHttpBackendFactory::create(QNetworkAccessManager::Operation op,
                                         const QNetworkRequest &request) const
{
    // check the operation
    switch (op) {
    case QNetworkAccessManager::GetOperation:
    case QNetworkAccessManager::PostOperation:
    case QNetworkAccessManager::HeadOperation:
    case QNetworkAccessManager::PutOperation:
        break;

    default:
        // no, we can't handle this request
        return 0;
    }

    QUrl url = request.url();
    if (url.scheme() == QLatin1String("http") || url.scheme() == QLatin1String("https"))
        return new QNetworkAccessHttpBackend;

    return 0;
}

static QNetworkReply::NetworkError statusCodeFromHttp(int httpStatusCode)
{
    QNetworkReply::NetworkError code;
    // we've got an error
    switch (httpStatusCode) {
    case 401:               // Authorization required
        code = QNetworkReply::AuthenticationRequiredError;
        break;

    case 403:               // Access denied
        code = QNetworkReply::ContentOperationNotPermittedError;
        break;

    case 404:               // Not Found
        code = QNetworkReply::ContentNotFoundError;
        break;

    case 407:
        code = QNetworkReply::ProxyAuthenticationRequiredError;
        break;

    default:
        if (httpStatusCode > 500) {
            // some kind of server error
            code = QNetworkReply::ProtocolUnknownError;
        } else if (httpStatusCode > 400) {
            // content error we did not handle above
            code = QNetworkReply::UnknownContentError;
        } else {
            qWarning("QNetworkAccess: got HTTP status code %d which is not expected",
                     httpStatusCode);
            code = QNetworkReply::ProtocolFailure;
        }
    }

    return code;
}

class QNetworkAccessHttpBackendCache: public QHttpNetworkConnection,
                                      public QNetworkAccessCache::CacheableObject
{
    // Q_OBJECT
public:
    QNetworkAccessHttpBackendCache(const QString &hostName, quint16 port, bool encrypt)
        : QHttpNetworkConnection(hostName, port, encrypt)
    {
        setExpires(true);
        setShareable(true);
    }

    virtual void dispose()
    {
#if 0  // sample code; do this right with the API
        Q_ASSERT(!isWorking());
#endif
        delete this;
    }
};

class QNetworkAccessHttpBackendIODevice: public QIODevice
{
    // Q_OBJECT
public:
    bool eof;
    QNetworkAccessHttpBackendIODevice(QNetworkAccessHttpBackend *parent)
        : QIODevice(parent), eof(false)
    {
        setOpenMode(ReadOnly);
    }
    bool isSequential() const { return true; }
    qint64 bytesAvailable() const
    { return static_cast<QNetworkAccessHttpBackend *>(parent())->upstreamBytesAvailable(); }

protected:
    virtual qint64 readData(char *buffer, qint64 maxlen)
    {
        qint64 ret = static_cast<QNetworkAccessHttpBackend *>(parent())->deviceReadData(buffer, maxlen);
        if (!ret && eof)
            return -1;
        return ret;
    }

    virtual qint64 writeData(const char *, qint64)
    {
        return -1;              // cannot write
    }

    friend class QNetworkAccessHttpBackend;
};

QNetworkAccessHttpBackend::QNetworkAccessHttpBackend()
    : QNetworkAccessBackend(), httpReply(0), http(0), uploadDevice(0)
#ifndef QT_NO_OPENSSL
    , pendingSslConfiguration(0), pendingIgnoreSslErrors(false)
#endif
{
}

QNetworkAccessHttpBackend::~QNetworkAccessHttpBackend()
{
    if (http)
        disconnectFromHttp();
#ifndef QT_NO_OPENSSL
    delete pendingSslConfiguration;
#endif
}

void QNetworkAccessHttpBackend::disconnectFromHttp()
{
    if (http) {
        disconnect(http, 0, this, 0);
        QByteArray cacheKey = makeCacheKey(url());
        QNetworkAccessCache *cache = QNetworkAccessManagerPrivate::getCache(this);
        cache->releaseEntry(cacheKey);
    }

    if (httpReply)
        disconnect(httpReply, 0, this, 0);

    http = 0;
    httpReply = 0;
}

void QNetworkAccessHttpBackend::finished()
{
    if (http)
        disconnectFromHttp();
    // call parent
    QNetworkAccessBackend::finished();
}

void QNetworkAccessHttpBackend::setupConnection()
{
#ifndef QT_NO_NETWORKPROXY
    connect(http, SIGNAL(proxyAuthenticationRequired(QNetworkProxy,QAuthenticator*)),
            SLOT(proxyAuthenticationRequired(QNetworkProxy,QAuthenticator*)));
#endif
    connect(http, SIGNAL(authenticationRequired(QHttpNetworkRequest,QAuthenticator*)),
            SLOT(httpAuthenticationRequired(QHttpNetworkRequest,QAuthenticator*)));
    connect(http, SIGNAL(error(QNetworkReply::NetworkError,QString)),
            SLOT(httpError(QNetworkReply::NetworkError,QString)));
#ifndef QT_NO_OPENSSL
    connect(http, SIGNAL(sslErrors(QList<QSslError>)),
            SLOT(sslErrors(QList<QSslError>)));
#endif
}

void QNetworkAccessHttpBackend::validateCache(QHttpNetworkRequest &httpRequest)
{
    if (request().attribute(QNetworkRequest::CacheLoadControlAttribute, QNetworkRequest::PreferNetwork) == QNetworkRequest::AlwaysNetwork) {
        // forced reload from the network
        // tell any caching proxy servers to reload too
        httpRequest.setHeaderField("Cache-Control", "no-cache");
        httpRequest.setHeaderField("Pragma", "no-cache");
        return;
    }
}

void QNetworkAccessHttpBackend::postRequest()
{
    QHttpNetworkRequest httpRequest;
    switch (operation()) {
    case QNetworkAccessManager::GetOperation:
        httpRequest.setOperation(QHttpNetworkRequest::Get);
        validateCache(httpRequest);
        break;

    case QNetworkAccessManager::HeadOperation:
        httpRequest.setOperation(QHttpNetworkRequest::Head);
        validateCache(httpRequest);
        break;

    case QNetworkAccessManager::PostOperation:
        httpRequest.setOperation(QHttpNetworkRequest::Post);
        uploadDevice = new QNetworkAccessHttpBackendIODevice(this);
        break;

    case QNetworkAccessManager::PutOperation:
        httpRequest.setOperation(QHttpNetworkRequest::Put);
        uploadDevice = new QNetworkAccessHttpBackendIODevice(this);
        break;

    default:
        break;                  // can't happen
    }

    httpRequest.setData(uploadDevice);
    httpRequest.setUrl(url());

    QList<QByteArray> headers = request().rawHeaderList();
    foreach (const QByteArray &header, headers)
        httpRequest.setHeaderField(header, request().rawHeader(header));

    httpReply = http->sendRequest(httpRequest);
    httpReply->setParent(this);
#ifndef QT_NO_OPENSSL
    if (pendingSslConfiguration) {
        httpReply->setSslConfiguration(*pendingSslConfiguration);
        delete pendingSslConfiguration;
        pendingSslConfiguration = 0;
    }
    if (pendingIgnoreSslErrors)
        httpReply->ignoreSslErrors();
#endif

    connect(httpReply, SIGNAL(readyRead()), SLOT(replyReadyRead()));
    connect(httpReply, SIGNAL(finished()), SLOT(replyFinished()));
    connect(httpReply, SIGNAL(finishedWithError(QNetworkReply::NetworkError,QString)),
            SLOT(httpError(QNetworkReply::NetworkError,QString)));
    connect(httpReply, SIGNAL(headerChanged()), SLOT(replyHeaderChanged()));
}

void QNetworkAccessHttpBackend::open()
{
    QUrl url = request().url();
    bool encrypt = url.scheme() == QLatin1String("https");
    setAttribute(QNetworkRequest::ConnectionEncryptedAttribute, encrypt);

    // set the port number in the reply if it wasn't set
    url.setPort(url.port(encrypt ? DefaultHttpsPort : DefaultHttpPort));

    // check if we have an open connection to this host
    QByteArray cacheKey = makeCacheKey(this->url());
    QNetworkAccessCache *cache = QNetworkAccessManagerPrivate::getCache(this);
    if ((http = static_cast<QNetworkAccessHttpBackendCache *>(cache->requestEntryNow(cacheKey))) == 0) {
        // no entry in cache; create an object
        http = new QNetworkAccessHttpBackendCache(url.host(), url.port(), encrypt);

#ifndef QT_NO_NETWORKPROXY
        QNetworkProxy networkProxy = proxy();
        if (encrypt || networkProxy.type() == QNetworkProxy::HttpProxy)
            http->setTransparentProxy(networkProxy);
        else
            http->setCacheProxy(networkProxy);
#endif

        cache->addEntry(cacheKey, http);
    }

    setupConnection();
    postRequest();
}

void QNetworkAccessHttpBackend::closeDownstreamChannel()
{
    // this indicates that the user closed the stream while the reply isn't finished yet
}

void QNetworkAccessHttpBackend::closeUpstreamChannel()
{
    // this indicates that the user finished uploading the data for POST
    Q_ASSERT(uploadDevice);
    uploadDevice->eof = true;
    emit uploadDevice->readChannelFinished();
}

#if 0
static inline bool isLWS(register char c)
{
    return c == ' ' || c == '\t' || c == '\r' || c == '\n';
}

static inline bool isSeparator(register char c)
{
    static const char separators[] = "()<>@,;:\\\"/[]?={}";
    return isLWS(c) || strchr(separators, c) != 0;
}

static int nextNonWhitespace(const QByteArray &text, int from)
{
    // RFC 2616 defines linear whitespace as:
    //  LWS = [CRLF] 1*( SP | HT )
    // We ignore the fact that CRLF must come as a pair at this point
    // It's an invalid HTTP header if that happens.
    while (from < text.length()) {
        if (isLWS(text.at(from)))
            ++from;
        else
            return from;        // non-whitespace
    }

    // reached the end
    return text.length();
}

static QHash<QByteArray, QByteArray> parseHttpOptionHeader(const QByteArray &header)
{
    // The HTTP header is of the form:
    // header          = #1(directives)
    // directives      = token | value-directive
    // value-directive = token "=" (token | quoted-string)
    QHash<QByteArray, QByteArray> result;

    int pos = 0;
    while (true) {
        // skip spaces
        pos = nextNonWhitespace(header, pos);
        if (pos == header.length())
            return result;      // end of parsing

        // pos points to a non-whitespace
        int comma = header.indexOf(',', pos);
        int equal = header.indexOf('=', pos);
        if (comma == pos || equal == pos)
            // huh? Broken header.
            return result;

        // The key name is delimited by either a comma, an equal sign or the end
        // of the header, whichever comes first
        int end = comma;
        if (end == -1)
            end = header.length();
        if (equal != -1 && end > equal)
            end = equal;        // equal sign comes before comma/end
        QByteArray key = QByteArray(header.constData() + pos, end - pos).trimmed().toLower();
        pos = end + 1;

        if (equal != -1) {
            // case: token "=" (token | quoted-string)
            // skip spaces
            pos = nextNonWhitespace(header, pos);
            if (pos == header.length())
                // huh? Broken header
                return result;

            QByteArray value;
            value.reserve(header.length() - pos);
            if (header.at(pos) == '"') {
                // case: quoted-string
                // quoted-string  = ( <"> *(qdtext | quoted-pair ) <"> )
                // qdtext         = <any TEXT except <">>
                // quoted-pair    = "\" CHAR
                ++pos;
                while (pos < header.length()) {
                    register char c = header.at(pos);
                    if (c == '"') {
                        // end of quoted text
                        break;
                    } else if (c == '\\') {
                        ++pos;
                        if (pos >= header.length())
                            // broken header
                            return result;
                        c = header.at(pos);
                    }

                    value += c;
                    ++pos;
                }
            } else {
                // case: token
                while (pos < header.length()) {
                    register char c = header.at(pos);
                    if (isSeparator(c))
                        break;
                    value += c;
                    ++pos;
                }
            }

            result.insert(key, value);

            // find the comma now:
            comma = header.indexOf(',', pos);
            if (comma == -1)
                return result;  // end of parsing
            pos = comma + 1;
        } else {
            // case: token
            // key is already set
            result.insert(key, QByteArray());
        }
    }
}
#endif

bool QNetworkAccessHttpBackend::waitForDownstreamReadyRead(int msecs)
{
    Q_ASSERT(http);

    if (httpReply->bytesAvailable()) {
        readFromHttp();
        return true;
    }

    if (msecs == 0) {
        // no bytes available in the socket and no waiting
        return false;
    }

    // ### FIXME
    qCritical("QNetworkAccess: HTTP backend does not support waitForReadyRead()");
    return false;
}

bool QNetworkAccessHttpBackend::waitForUpstreamBytesWritten(int msecs)
{

    // ### FIXME: not implemented in QHttpNetworkAccess
    Q_UNUSED(msecs);
    qCritical("QNetworkAccess: HTTP backend does not support waitForBytesWritten()");
    return false;
}

void QNetworkAccessHttpBackend::upstreamReadyRead()
{
    // There is more data available from the user to be uploaded
    // QHttpNetworkAccess implements the upload rate control:
    //  we simply tell QHttpNetworkAccess that there is more data available
    //  it'll pull from us when it can (through uploadDevice)

    Q_ASSERT(uploadDevice);
    emit uploadDevice->readyRead();
}

qint64 QNetworkAccessHttpBackend::deviceReadData(char *buffer, qint64 maxlen)
{
    QByteArray toBeUploaded = readUpstream();
    if (toBeUploaded.isEmpty())
        return 0;               // nothing to be uploaded

    maxlen = qMin<qint64>(maxlen, toBeUploaded.length());

    memcpy(buffer, toBeUploaded.constData(), maxlen);
    upstreamBytesConsumed(maxlen);
    return maxlen;
}

void QNetworkAccessHttpBackend::downstreamReadyWrite()
{
    readFromHttp();
    if (httpReply && httpReply->bytesAvailable() == 0 && httpReply->isFinished())
        replyFinished();
}

void QNetworkAccessHttpBackend::replyReadyRead()
{
    readFromHttp();
}

void QNetworkAccessHttpBackend::readFromHttp()
{
    // We implement the download rate control
    // Don't read from QHttpNetworkAccess more than QNetworkAccessBackend wants
    // One of the two functions above will be called when we can read again

    qint64 bytesToRead = qBound<qint64>(0, httpReply->bytesAvailable(), nextDownstreamBlockSize());
    if (!bytesToRead)
        return;

    QByteArray data = httpReply->read(bytesToRead);
    writeDownstreamData(data);
}

void QNetworkAccessHttpBackend::replyFinished()
{
    if (httpReply->bytesAvailable())
        // we haven't read everything yet. Wait some more.
        return;

    int statusCode = httpReply->statusCode();
    if (statusCode >= 400) {
        // it's an error reply
        QString msg = QLatin1String(QT_TRANSLATE_NOOP("QNetworkReply",
                                                      "Error downloading %1 - server replied: %2"));
        msg = msg.arg(url().toString(), httpReply->reasonPhrase());
        error(statusCodeFromHttp(httpReply->statusCode()), msg);
    }
    finished();
}

void QNetworkAccessHttpBackend::replyHeaderChanged()
{
    // reconstruct the HTTP header
    QList<QPair<QByteArray, QByteArray> > headerMap = httpReply->header();
    QList<QPair<QByteArray, QByteArray> >::ConstIterator it = headerMap.constBegin(),
                                                        end = headerMap.constEnd();
    QByteArray header;

    for (; it != end; ++it) {
        QByteArray value = rawHeader(it->first);
        if (!value.isEmpty())
            value += ", ";
        value += it->second;
        setRawHeader(it->first, value);
    }

    setAttribute(QNetworkRequest::HttpStatusCodeAttribute, httpReply->statusCode());
    setAttribute(QNetworkRequest::HttpReasonPhraseAttribute, httpReply->reasonPhrase());

    // is it a redirection?
    switch (httpReply->statusCode()) {
    case 301:                   // Moved Permanently
    case 302:                   // Found
    case 303:                   // See Other
    case 307:                   // Temporary Redirect
        // What do we do about the caching of the HTML note?
        // The response to a 303 MUST NOT be cached, while the response to
        // all of the others is cacheable if the headers indicate it to be
        redirectionRequested(QUrl::fromEncoded(rawHeader("location")));
    }

    metaDataChanged();
}

void QNetworkAccessHttpBackend::httpAuthenticationRequired(const QHttpNetworkRequest &,
                                                           QAuthenticator *auth)
{
    authenticationRequired(auth);
}

void QNetworkAccessHttpBackend::httpError(QNetworkReply::NetworkError errorCode,
                                          const QString &errorString)
{
#if 0
    static const QNetworkReply::NetworkError conversionTable[] = {
        QNetworkReply::ConnectionRefusedError,
        QNetworkReply::RemoteHostClosedError,
        QNetworkReply::HostNotFoundError,
        QNetworkReply::UnknownNetworkError, // SocketAccessError
        QNetworkReply::UnknownNetworkError, // SocketResourceError
        QNetworkReply::TimeoutError,        // SocketTimeoutError
        QNetworkReply::UnknownNetworkError, // DatagramTooLargeError
        QNetworkReply::UnknownNetworkError, // NetworkError
        QNetworkReply::UnknownNetworkError, // AddressInUseError
        QNetworkReply::UnknownNetworkError, // SocketAddressNotAvailableError
        QNetworkReply::UnknownNetworkError, // UnsupportedSocketOperationError
        QNetworkReply::UnknownNetworkError, // UnfinishedSocketOperationError
        QNetworkReply::ProxyAuthenticationRequiredError
    };
    QNetworkReply::NetworkError code;
    if (int(errorCode) >= 0 &&
        uint(errorCode) < (sizeof conversionTable / sizeof conversionTable[0]))
        code = conversionTable[errorCode];
    else
        code = QNetworkReply::UnknownNetworkError;
#endif
    error(errorCode, errorString);
    finished();
}

void QNetworkAccessHttpBackend::copyFinished(QIODevice *dev)
{
    delete dev;
    finished();
}

#ifndef QT_NO_OPENSSL
void QNetworkAccessHttpBackend::ignoreSslErrors()
{
    if (httpReply)
        httpReply->ignoreSslErrors();
    else
        pendingIgnoreSslErrors = true;
}

void QNetworkAccessHttpBackend::fetchSslConfiguration(QSslConfiguration &config) const
{
    if (httpReply)
        config = httpReply->sslConfiguration();
    else if (pendingSslConfiguration)
        config = *pendingSslConfiguration;
}

void QNetworkAccessHttpBackend::setSslConfiguration(const QSslConfiguration &newconfig)
{
    if (httpReply)
        httpReply->setSslConfiguration(newconfig);
    else if (pendingSslConfiguration)
        *pendingSslConfiguration = newconfig;
    else
        pendingSslConfiguration = new QSslConfiguration(newconfig);
}
#endif

QT_END_NAMESPACE

#endif // QT_NO_HTTP
