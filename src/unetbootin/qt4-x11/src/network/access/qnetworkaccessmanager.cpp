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

#include "qnetworkaccessmanager.h"
#include "qnetworkaccessmanager_p.h"
#include "qnetworkrequest.h"
#include "qnetworkreply.h"
#include "qnetworkreply_p.h"
#include "qnetworkcookie.h"
#include "qabstractnetworkcache.h"

#include "qnetworkaccesshttpbackend_p.h"
#include "qnetworkaccessftpbackend_p.h"
#include "qnetworkaccessfilebackend_p.h"
#include "qnetworkaccessdatabackend_p.h"
#include "qnetworkaccessdebugpipebackend_p.h"

#include "QtCore/qbuffer.h"
#include "QtCore/qurl.h"
#include "QtCore/qvector.h"
#include "QtNetwork/qauthenticator.h"
#include "QtNetwork/qsslconfiguration.h"

QT_BEGIN_NAMESPACE

#ifndef QT_NO_HTTP
Q_GLOBAL_STATIC(QNetworkAccessHttpBackendFactory, httpBackend)
#endif // QT_NO_HTTP
Q_GLOBAL_STATIC(QNetworkAccessFileBackendFactory, fileBackend)
Q_GLOBAL_STATIC(QNetworkAccessDataBackendFactory, dataBackend)
#ifndef QT_NO_FTP
Q_GLOBAL_STATIC(QNetworkAccessFtpBackendFactory, ftpBackend)
#endif // QT_NO_FTP

#ifdef QT_BUILD_INTERNAL
Q_GLOBAL_STATIC(QNetworkAccessDebugPipeBackendFactory, debugpipeBackend)
#endif

static void ensureInitialized()
{
#ifndef QT_NO_HTTP
    (void) httpBackend();
#endif // QT_NO_HTTP
    (void) dataBackend();
#ifndef QT_NO_FTP
    (void) ftpBackend();
#endif

#ifdef QT_BUILD_INTERNAL
    (void) debugpipeBackend();
#endif

    // leave this one last since it will query the special QAbstractFileEngines
    (void) fileBackend();
}

/*!
    \class QNetworkAccessManager
    \brief The QNetworkAccessManager class allows the application to
    post network requests and receive replies
    \since 4.4

    \inmodule QtNetwork
    \reentrant

    The Network Access API is constructed around one QNetworkAccessManager
    object, which holds the common configuration and settings for the requests
    it sends. It contains the proxy and cache configuration, as well as the
    signals related to such issues, and reply signals that can be used to
    monitor the progress of a network operation.

    Once a QNetworkAccessManager object has been created, the application can
    use it to send requests over the network. A group of standard functions
    are supplied that take a request and optional data, and each return a
    QNetworkReply object. The returned object is used to obtain any data
    returned in response to the corresponding request.
    the reply to is where most of the signals as well
    as the downloaded data are posted.

    A simple download off the network could be accomplished with:
    \snippet doc/src/snippets/code/src_network_access_qnetworkaccessmanager.cpp 0

    When the \tt replyFinished slot above is called, the parameter it
    takes is the QNetworkReply object containing the downloaded data
    as well as meta-data (headers, etc.).

    \note The slot is responsible for deleting the object at that point.

    A more involved example, assuming the manager is already existent,
    can be:
    \snippet doc/src/snippets/code/src_network_access_qnetworkaccessmanager.cpp 1

    \sa QNetworkRequest, QNetworkReply, QNetworkProxy
*/

/*!
    \enum QNetworkAccessManager::Operation

    Indicates the operation this reply is processing.

    \value HeadOperation        retrieve headers operation (created
    with head())

    \value GetOperation         retrieve headers and download contents
    (created with get())

    \value PutOperation         upload contents operation (created
    with put())

    \value PostOperation        send the contents of an HTML form for
    processing via HTTP POST (created with post())

    \omitvalue UnknownOperation

    \sa QNetworkReply::operation()
*/

/*!
    \fn void QNetworkAccessManager::proxyAuthenticationRequired(const QNetworkProxy &proxy, QAuthenticator *authenticator)

    This signal is emitted whenever a proxy requests authentication
    and QNetworkAccessManager cannot find a valid, cached
    credential. The slot connected to this signal should fill in the
    credentials for the proxy \a proxy in the \a authenticator object.

    QNetworkAccessManager will cache the credentials internally. The
    next time the proxy requests authentication, QNetworkAccessManager
    will automatically send the same credential without emitting the
    proxyAuthenticationRequired signal again.

    If the proxy rejects the credentials, QNetworkAccessManager will
    emit the signal again.

    \sa proxy(), setProxy(), authenticationRequired()
*/

/*!
    \fn void QNetworkAccessManager::authenticationRequired(QNetworkReply *reply, QAuthenticator *authenticator)

    This signal is emitted whenever a final server requests
    authentication before it delivers the requested contents. The slot
    connected to this signal should fill the credentials for the
    contents (which can be determined by inspecting the \a reply
    object) in the \a authenticator object.

    QNetworkAccessManager will cache the credentials internally and
    will send the same values if the server requires authentication
    again, without emitting the authenticationRequired() signal. If it
    rejects the credentials, this signal will be emitted again.

    \sa proxyAuthenticationRequired()
*/

/*!
    \fn void QNetworkAccessManager::finished(QNetworkReply *reply)

    This signal is emitted whenever a pending network reply is
    finished. The \a reply parameter will contain a pointer to the
    reply that has just finished. This signal is emitted in tandem
    with the QNetworkReply::finished() signal.

    See QNetworkReply::finished() for information on the status that
    the object will be in.

    \sa QNetworkReply::finished(), QNetworkReply::error()
*/

/*!
    \fn void QNetworkAccessManager::sslErrors(QNetworkReply *reply, const QList<QSslError> &errors)

    This signal is emitted if the SSL/TLS session encountered errors
    during the set up, including certificate verification errors. The
    \a errors parameter contains the list of errors and \a reply is
    the QNetworkReply that is encountering these errors.

    To indicate that the errors are not fatal and that the connection
    should proceed, the QNetworkReply::ignoreSslErrors() function should be called
    from the slot connected to this signal. If it is not called, the
    SSL session will be torn down before any data is exchanged
    (including the URL).

    This signal can be used to display an error message to the user
    indicating that security may be compromised and display the
    SSL settings (see sslConfiguration() to obtain it). If the user
    decides to proceed after analyzing the remote certificate, the
    slot should call ignoreSslErrors().

    \sa QSslSocket::sslErrors(), QNetworkReply::sslErrors(),
    QNetworkReply::sslConfiguration(), QNetworkReply::ignoreSslErrors()
*/

class QNetworkAuthenticationCredential
{
public:
    QString domain;
    QString user;
    QString password;
};
Q_DECLARE_TYPEINFO(QNetworkAuthenticationCredential, Q_MOVABLE_TYPE);
inline bool operator<(const QNetworkAuthenticationCredential &t1, const QString &t2)
{ return t1.domain < t2; }

class QNetworkAuthenticationCache: private QVector<QNetworkAuthenticationCredential>,
                                   public QNetworkAccessCache::CacheableObject
{
public:
    QNetworkAuthenticationCache()
    {
        setExpires(false);
        setShareable(true);
        reserve(1);
    }

    QNetworkAuthenticationCredential *findClosestMatch(const QString &domain)
    {
        iterator it = qLowerBound(begin(), end(), domain);
        if (it == end() && !isEmpty())
            --it;
        if (it == end() || !domain.startsWith(it->domain))
            return 0;
        return &*it;
    }

    void insert(const QString &domain, const QString &user, const QString &password)
    {
        QNetworkAuthenticationCredential *closestMatch = findClosestMatch(domain);
        if (closestMatch && closestMatch->domain == domain) {
            // we're overriding the current credentials
            closestMatch->user = user;
            closestMatch->password = password;
        } else {
            QNetworkAuthenticationCredential newCredential;
            newCredential.domain = domain;
            newCredential.user = user;
            newCredential.password = password;

            if (closestMatch)
                QVector<QNetworkAuthenticationCredential>::insert(++closestMatch, newCredential);
            else
                QVector<QNetworkAuthenticationCredential>::insert(end(), newCredential);
        }
    }

    virtual void dispose() { delete this; }
};

#ifndef QT_NO_NETWORKPROXY
static QByteArray proxyAuthenticationKey(const QNetworkProxy &proxy, const QString &realm)
{
    QUrl key;

    switch (proxy.type()) {
    case QNetworkProxy::Socks5Proxy:
        key.setScheme(QLatin1String("proxy-socks5"));
        break;

    case QNetworkProxy::HttpProxy:
    case QNetworkProxy::HttpCachingProxy:
        key.setScheme(QLatin1String("proxy-http"));
        break;

    case QNetworkProxy::FtpCachingProxy:
        key.setScheme(QLatin1String("proxy-ftp"));

    case QNetworkProxy::DefaultProxy:
    case QNetworkProxy::NoProxy:
        // shouldn't happen
        return QByteArray();

        // no default:
        // let there be errors if a new proxy type is added in the future
    }

    if (key.scheme().isEmpty())
        // proxy type not handled
        return QByteArray();

    key.setUserName(proxy.user());
    key.setHost(proxy.hostName());
    key.setPort(proxy.port());
    key.setFragment(realm);
    return "auth:" + key.toEncoded();
}
#endif

static inline QByteArray authenticationKey(const QUrl &url, const QString &realm)
{
    QUrl copy = url;
    copy.setFragment(realm);
    return "auth:" + copy.toEncoded(QUrl::RemovePassword | QUrl::RemovePath | QUrl::RemoveQuery);
}

/*!
    Constructs a QNetworkAccessManager object that is the center of
    the Network Access API and sets \a parent as the parent object.
*/
QNetworkAccessManager::QNetworkAccessManager(QObject *parent)
    : QObject(*new QNetworkAccessManagerPrivate, parent)
{
    ensureInitialized();
}

/*!
    Destroys the QNetworkAccessManager object and frees up any
    resources. Note that QNetworkReply objects that are returned from
    this class have this object set as their parents, which means that
    they will be deleted along with it if you don't call
    QObject::setParent() on them.
*/
QNetworkAccessManager::~QNetworkAccessManager()
{
#ifndef QT_NO_NETWORKPROXY
    delete d_func()->proxyFactory;
#endif
}

#ifndef QT_NO_NETWORKPROXY
/*!
    Returns the QNetworkProxy that the requests sent using this
    QNetworkAccessManager object will use. The default value for the
    proxy is QNetworkProxy::DefaultProxy.

    \sa setProxy(), setProxyFactory(), proxyAuthenticationRequired()
*/
QNetworkProxy QNetworkAccessManager::proxy() const
{
    return d_func()->proxy;
}

/*!
    Sets the proxy to be used in future requests to be \a proxy. This
    does not affect requests that have already been sent. The
    proxyAuthenticationRequired() signal will be emitted if the proxy
    requests authentication.

    A proxy set with this function will be used for all requests
    issued by QNetworkAccessManager. In some cases, it might be
    necessary to select different proxies depending on the type of
    request being sent or the destination host. If that's the case,
    you should consider using setProxyFactory().

    \sa proxy(), proxyAuthenticationRequired()
*/
void QNetworkAccessManager::setProxy(const QNetworkProxy &proxy)
{
    Q_D(QNetworkAccessManager);
    delete d->proxyFactory;
    d->proxy = proxy;
    d->proxyFactory = 0;
}

/*!
    \fn QNetworkProxyFactory *QNetworkAccessManager::proxyFactory() const
    \since 4.5

    Returns the proxy factory that this QNetworkAccessManager object
    is using to determine the proxies to be used for requests.

    Note that the pointer returned by this function is managed by
    QNetworkAccessManager and could be deleted at any time.

    \sa setProxyFactory(), proxy()
*/
QNetworkProxyFactory *QNetworkAccessManager::proxyFactory() const
{
    return d_func()->proxyFactory;
}

/*!
    \since 4.5

    Sets the proxy factory for this class to be \a factory. A proxy
    factory is used to determine a more specific list of proxies to be
    used for a given request, instead of trying to use the same proxy
    value for all requests.

    All queries sent by QNetworkAccessManager will have type
    QNetworkProxyQuery::UrlRequest.

    For example, a proxy factory could apply the following rules:
    \list
      \o if the target address is in the local network (for example,
         if the hostname contains no dots or if it's an IP address in
         the organization's range), return QNetworkProxy::NoProxy
      \o if the request is FTP, return an FTP proxy
      \o if the request is HTTP or HTTPS, then return an HTTP proxy
      \o otherwise, return a SOCKSv5 proxy server
    \endlist

    The lifetime of the object \a factory will be managed by
    QNetworkAccessManager. It will delete the object when necessary.

    \note If a specific proxy is set with setProxy(), the factory will not
    be used.

    \sa proxyFactory(), setProxy(), QNetworkProxyQuery
*/
void QNetworkAccessManager::setProxyFactory(QNetworkProxyFactory *factory)
{
    Q_D(QNetworkAccessManager);
    delete d->proxyFactory;
    d->proxyFactory = factory;
    d->proxy = QNetworkProxy();
}
#endif

/*!
    \since 4.5

    Returns the cache that is used to store data obtained from the network.

    \sa setCache()
*/
QAbstractNetworkCache *QNetworkAccessManager::cache() const
{
    Q_D(const QNetworkAccessManager);
    return d->networkCache;
}

/*!
    \since 4.5

    Sets the manager's network cache to be the \a cache specified. The cache
    is used for all requests dispatched by the manager.

    Use this function to set the network cache object to a class that implements
    additional features, like saving the cookies to permanent storage.

    \note QNetworkAccessManager takes ownership of the \a cache object.

    QNetworkAccessManager by default does not have a set cache.
    Qt provides a simple disk cache, QNetworkDiskCache, which can be used.

    \sa cache(), QNetworkRequest::CacheLoadControl
*/
void QNetworkAccessManager::setCache(QAbstractNetworkCache *cache)
{
    Q_D(QNetworkAccessManager);
    if (d->networkCache != cache) {
        delete d->networkCache;
        d->networkCache = cache;
        d->networkCache->setParent(this);
    }
}

/*!
    Returns the QNetworkCookieJar that is used to store cookies
    obtained from the network as well as cookies that are about to be
    sent.

    \sa setCookieJar()
*/
QNetworkCookieJar *QNetworkAccessManager::cookieJar() const
{
    Q_D(const QNetworkAccessManager);
    if (!d->cookieJar)
        d->createCookieJar();
    return d->cookieJar;
}

/*!
    Sets the manager's cookie jar to be the \a cookieJar specified.
    The cookie jar is used by all requests dispatched by the manager.

    Use this function to set the cookie jar object to a class that
    implements additional features, like saving the cookies to permanent
    storage.

    \note QNetworkAccessManager takes ownership of the \a cookieJar object.

    QNetworkAccessManager will set the parent of the \a cookieJar
    passed to itself, so that the cookie jar is deleted when this
    object is deleted as well. If you want to share cookie jars
    between different QNetworkAccessManager objects, you may want to
    set the cookie jar's parent to 0 after calling this function.

    QNetworkAccessManager by default does not implement any cookie
    policy of its own: it accepts all cookies sent by the server, as
    long as they are well formed and meet the minimum security
    requirements (cookie domain matches the request's and cookie path
    matches the request's). In order to implement your own security
    policy, override the QNetworkCookieJar::cookiesForUrl() and
    QNetworkCookieJar::setCookiesFromUrl() virtual functions. Those
    functions are called by QNetworkAccessManager when it detects a
    new cookie.

    \sa cookieJar(), QNetworkCookieJar::cookiesForUrl(), QNetworkCookieJar::setCookiesFromUrl()
*/
void QNetworkAccessManager::setCookieJar(QNetworkCookieJar *cookieJar)
{
    Q_D(QNetworkAccessManager);
    d->cookieJarCreated = true;
    if (d->cookieJar != cookieJar) {
        if (d->cookieJar && d->cookieJar->parent() == this)
            delete d->cookieJar;
        d->cookieJar = cookieJar;
        d->cookieJar->setParent(this);
    }
}

/*!
    This function is used to post a request to obtain the network
    headers for \a request. It takes its name after the HTTP request
    associated (HEAD). It returns a new QNetworkReply object which
    will contain such headers.
*/
QNetworkReply *QNetworkAccessManager::head(const QNetworkRequest &request)
{
    return d_func()->postProcess(createRequest(QNetworkAccessManager::HeadOperation, request));
}

/*!
    This function is used to post a request to obtain the contents of
    the target \a request. It will cause the contents to be
    downloaded, along with the headers associated with it. It returns
    a new QNetworkReply object opened for reading which emits its
    QIODevice::readyRead() signal whenever new data arrives.

    \sa post(), put()
*/
QNetworkReply *QNetworkAccessManager::get(const QNetworkRequest &request)
{
    return d_func()->postProcess(createRequest(QNetworkAccessManager::GetOperation, request));
}

/*!
    This function is used to send an HTTP POST request to the
    destination specified by \a request. The contents of the \a data
    device will be uploaded to the server.

    \a data must be opened for reading when this function is called
    and must remain valid until the finished() signal is emitted for
    this reply.

    The returned QNetworkReply object will be open for reading and
    will contain the reply sent by the server to the POST request.

    Note: sending a POST request on protocols other than HTTP and
    HTTPS is undefined and will probably fail.

    \sa get(), put()
*/
QNetworkReply *QNetworkAccessManager::post(const QNetworkRequest &request, QIODevice *data)
{
    return d_func()->postProcess(createRequest(QNetworkAccessManager::PostOperation, request, data));
}

/*!
    \overload
    This function sends the contents of the \a data byte array to the
    destination specified by \a request.
*/
QNetworkReply *QNetworkAccessManager::post(const QNetworkRequest &request, const QByteArray &data)
{
    QBuffer *buffer = new QBuffer;
    buffer->setData(data);
    buffer->open(QIODevice::ReadOnly);

    QNetworkReply *reply = post(request, buffer);
    buffer->setParent(reply);
    return reply;
}

/*!
    This function is used to upload the contents of \a data to the
    destination \a request.

    \a data must be opened for reading when this function is called
    and must remain valid until the finished() signal is emitted for
    this reply.

    The returned QNetworkReply object will be open for reply, but
    whether anything will be available for reading is protocol
    dependent. For HTTP, the server may send a small HTML page
    indicating the upload was successful (or not). Other protocols
    will probably have content in their replies.

    For HTTP, this request will send a PUT request, which most servers
    do not allow. Form upload mechanisms, including that of uploading
    files through HTML forms, use the POST mechanism.

    \sa get(), post()
*/
QNetworkReply *QNetworkAccessManager::put(const QNetworkRequest &request, QIODevice *data)
{
    return d_func()->postProcess(createRequest(QNetworkAccessManager::PutOperation, request, data));
}

/*!
    \overload
    This function sends the contents of the \a data byte array to the
    destination specified by \a request.
*/
QNetworkReply *QNetworkAccessManager::put(const QNetworkRequest &request, const QByteArray &data)
{
    QBuffer *buffer = new QBuffer;
    buffer->setData(data);
    buffer->open(QIODevice::ReadOnly);

    QNetworkReply *reply = put(request, buffer);
    buffer->setParent(reply);
    return reply;
}

/*!
    Returns a new QNetworkReply object to handle the operation \a op
    and request \a req. The device \a outgoingData is always 0 for Get and
    Head requests, but is the value passed to post() and put() in
    those operations (the QByteArray variants will pass a QBuffer
    object).

    The default implementation calls QNetworkCookieJar::cookiesForUrl()
    on the cookie jar set with setCookieJar() to obtain the cookies to
    be sent to the remote server.

    The returned object must be in an open state.
*/
QNetworkReply *QNetworkAccessManager::createRequest(QNetworkAccessManager::Operation op,
                                                    const QNetworkRequest &req,
                                                    QIODevice *outgoingData)
{
    Q_D(QNetworkAccessManager);
    QNetworkRequest request = req;
    if (!request.header(QNetworkRequest::ContentLengthHeader).isValid() &&
        outgoingData && !outgoingData->isSequential()) {
        // request has no Content-Length
        // but the data that is outgoing is random-access
        request.setHeader(QNetworkRequest::ContentLengthHeader, outgoingData->size());
    }
    if (d->cookieJar) {
        QList<QNetworkCookie> cookies = d->cookieJar->cookiesForUrl(request.url());
        if (!cookies.isEmpty())
            request.setHeader(QNetworkRequest::CookieHeader, qVariantFromValue(cookies));
    }

    // first step: create the reply
    QUrl url = request.url();
    QNetworkReplyImpl *reply = new QNetworkReplyImpl(this);
    QNetworkReplyImplPrivate *priv = reply->d_func();
    priv->manager = this;

    // second step: fetch cached credentials
    QNetworkAuthenticationCredential *cred = d->fetchCachedCredentials(url);
    if (cred) {
        url.setUserName(cred->user);
        url.setPassword(cred->password);
        priv->urlForLastAuthentication = url;
    }

    // third step: setup the reply
    priv->setup(op, request, outgoingData);
    if (request.attribute(QNetworkRequest::CacheLoadControlAttribute, QNetworkRequest::PreferNetwork).toInt() !=
        QNetworkRequest::AlwaysNetwork)
        priv->setNetworkCache(d->networkCache);
#ifndef QT_NO_NETWORKPROXY
    QList<QNetworkProxy> proxyList = d->queryProxy(QNetworkProxyQuery(request.url()));
    priv->proxyList = proxyList;
#endif

    // fourth step: find a backend
    priv->backend = d->findBackend(op, request);
    if (priv->backend) {
        priv->backend->setParent(reply);
        priv->backend->reply = priv;
    }

#ifndef QT_NO_OPENSSL
    reply->setSslConfiguration(request.sslConfiguration());
#endif
    return reply;
}

void QNetworkAccessManagerPrivate::_q_replyFinished()
{
    Q_Q(QNetworkAccessManager);
    QNetworkReply *reply = qobject_cast<QNetworkReply *>(q->sender());
    if (reply)
        emit q->finished(reply);
}

void QNetworkAccessManagerPrivate::_q_replySslErrors(const QList<QSslError> &errors)
{
#ifndef QT_NO_OPENSSL
    Q_Q(QNetworkAccessManager);
    QNetworkReply *reply = qobject_cast<QNetworkReply *>(q->sender());
    if (reply)
        emit q->sslErrors(reply, errors);
#else
    Q_UNUSED(errors);
#endif
}

QNetworkReply *QNetworkAccessManagerPrivate::postProcess(QNetworkReply *reply)
{
    Q_Q(QNetworkAccessManager);
    QNetworkReplyPrivate::setManager(reply, q);
    q->connect(reply, SIGNAL(finished()), SLOT(_q_replyFinished()));
#ifndef QT_NO_OPENSSL
    /* In case we're compiled without SSL support, we don't have this signal and we need to
     * avoid getting a connection error. */
    q->connect(reply, SIGNAL(sslErrors(QList<QSslError>)), SLOT(_q_replySslErrors(QList<QSslError>)));
#endif

    return reply;
}

void QNetworkAccessManagerPrivate::createCookieJar() const
{
    if (!cookieJarCreated) {
        // keep the ugly hack in here
        QNetworkAccessManagerPrivate *that = const_cast<QNetworkAccessManagerPrivate *>(this);
        that->cookieJarCreated = true;
        that->cookieJar = new QNetworkCookieJar(that->q_func());
    }
}

void QNetworkAccessManagerPrivate::authenticationRequired(QNetworkAccessBackend *backend,
                                                          QAuthenticator *authenticator)
{
    Q_Q(QNetworkAccessManager);

    // FIXME: Add support for domains (i.e., the leading path)
    QUrl url = backend->reply->url;

    // don't try the cache for the same URL twice in a row
    // being called twice for the same URL means the authentication failed
    if (url != backend->reply->urlForLastAuthentication) {
        QNetworkAuthenticationCredential *cred = fetchCachedCredentials(url, authenticator);
        if (cred) {
            authenticator->setUser(cred->user);
            authenticator->setPassword(cred->password);
            backend->reply->urlForLastAuthentication = url;
            return;
        }
    }

    backend->reply->urlForLastAuthentication = url;
    emit q->authenticationRequired(backend->reply->q_func(), authenticator);
    addCredentials(url, authenticator);
}

#ifndef QT_NO_NETWORKPROXY
void QNetworkAccessManagerPrivate::proxyAuthenticationRequired(QNetworkAccessBackend *backend,
                                                               const QNetworkProxy &proxy,
                                                               QAuthenticator *authenticator)
{
    Q_Q(QNetworkAccessManager);

    if (proxy != backend->reply->lastProxyAuthentication) {
        QNetworkAuthenticationCredential *cred = fetchCachedCredentials(proxy);
        if (cred) {
            authenticator->setUser(cred->user);
            authenticator->setPassword(cred->password);
            return;
        }
    }

    backend->reply->lastProxyAuthentication = proxy;
    emit q->proxyAuthenticationRequired(proxy, authenticator);
    addCredentials(proxy, authenticator);
}

void QNetworkAccessManagerPrivate::addCredentials(const QNetworkProxy &p,
                                                  const QAuthenticator *authenticator)
{
    Q_ASSERT(authenticator);
    Q_ASSERT(p.type() != QNetworkProxy::DefaultProxy);
    Q_ASSERT(p.type() != QNetworkProxy::NoProxy);

    QString realm = authenticator->realm();
    QNetworkProxy proxy = p;
    proxy.setUser(authenticator->user());
    // Set two credentials: one with the username and one without
    do {
        // Set two credentials actually: one with and one without the realm
        do {
            QByteArray cacheKey = proxyAuthenticationKey(proxy, realm);
            if (cacheKey.isEmpty())
                return;             // should not happen

            QNetworkAuthenticationCache *auth = new QNetworkAuthenticationCache;
            auth->insert(QString(), authenticator->user(), authenticator->password());
            cache.addEntry(cacheKey, auth); // replace the existing one, if there's any

            if (realm.isEmpty()) {
                break;
            } else {
                realm.clear();
            }
        } while (true);

        if (proxy.user().isEmpty())
            break;
        else
            proxy.setUser(QString());
    } while (true);
}

QNetworkAuthenticationCredential *
QNetworkAccessManagerPrivate::fetchCachedCredentials(const QNetworkProxy &p,
                                                     const QAuthenticator *authenticator)
{
    QNetworkProxy proxy = p;
    if (proxy.type() == QNetworkProxy::DefaultProxy) {
        proxy = QNetworkProxy::applicationProxy();
    }
    if (!proxy.password().isEmpty())
        return 0;               // no need to set credentials if it already has them

    QString realm;
    if (authenticator)
        realm = authenticator->realm();

    QByteArray cacheKey = proxyAuthenticationKey(proxy, realm);
    if (cacheKey.isEmpty())
        return 0;
    if (!cache.hasEntry(cacheKey))
        return 0;

    QNetworkAuthenticationCache *auth =
        static_cast<QNetworkAuthenticationCache *>(cache.requestEntryNow(cacheKey));
    QNetworkAuthenticationCredential *cred = auth->findClosestMatch(QString());
    cache.releaseEntry(cacheKey);

    // proxy cache credentials always have exactly one item
    Q_ASSERT_X(cred, "QNetworkAccessManager",
               "Internal inconsistency: found a cache key for a proxy, but it's empty");
    return cred;
}

QList<QNetworkProxy> QNetworkAccessManagerPrivate::queryProxy(const QNetworkProxyQuery &query)
{
    QList<QNetworkProxy> proxies;
    if (proxyFactory) {
        proxies = proxyFactory->queryProxy(query);
        if (proxies.isEmpty()) {
            qWarning("QNetworkAccessManager: factory %p has returned an empty result set",
                     proxyFactory);
            proxies << QNetworkProxy::NoProxy;
        }
    } else if (proxy.type() == QNetworkProxy::DefaultProxy) {
        // no proxy set, query the application
        return QNetworkProxyFactory::proxyForQuery(query);
    } else {
        proxies << proxy;
    }

    return proxies;
}
#endif

void QNetworkAccessManagerPrivate::addCredentials(const QUrl &url,
                                                  const QAuthenticator *authenticator)
{
    Q_ASSERT(authenticator);
    QString domain = QString::fromLatin1("/"); // FIXME: make QAuthenticator return the domain
    QString realm = authenticator->realm();

    // Set two credentials actually: one with and one without the username in the URL
    QUrl copy = url;
    copy.setUserName(authenticator->user());
    do {
        QByteArray cacheKey = authenticationKey(copy, realm);
        if (cache.hasEntry(cacheKey)) {
            QNetworkAuthenticationCache *auth =
                static_cast<QNetworkAuthenticationCache *>(cache.requestEntryNow(cacheKey));
            auth->insert(domain, authenticator->user(), authenticator->password());
            cache.releaseEntry(cacheKey);
        } else {
            QNetworkAuthenticationCache *auth = new QNetworkAuthenticationCache;
            auth->insert(domain, authenticator->user(), authenticator->password());
            cache.addEntry(cacheKey, auth);
        }

        if (copy.userName().isEmpty()) {
            break;
        } else {
            copy.setUserName(QString());
        }
    } while (true);
}

/*!
    Fetch the credential data from the credential cache.

    If auth is 0 (as it is when called from createRequest()), this will try to
    look up with an empty realm. That fails in most cases for HTTP (because the
    realm is seldom empty for HTTP challenges). In any case, QHttpNetworkConnection
    never sends the credentials on the first attempt: it needs to find out what
    authentication methods the server supports.

    For FTP, realm is always empty.
*/
QNetworkAuthenticationCredential *
QNetworkAccessManagerPrivate::fetchCachedCredentials(const QUrl &url,
                                                     const QAuthenticator *authentication)
{
    if (!url.password().isEmpty())
        return 0;               // no need to set credentials if it already has them

    QString realm;
    if (authentication)
        realm = authentication->realm();

    QByteArray cacheKey = authenticationKey(url, realm);
    if (!cache.hasEntry(cacheKey))
        return 0;

    QNetworkAuthenticationCache *auth =
        static_cast<QNetworkAuthenticationCache *>(cache.requestEntryNow(cacheKey));
    QNetworkAuthenticationCredential *cred = auth->findClosestMatch(url.path());
    cache.releaseEntry(cacheKey);
    return cred;
}

void QNetworkAccessManagerPrivate::clearCache(QNetworkAccessManager *manager)
{
    manager->d_func()->cache.clear();
}

QT_END_NAMESPACE

#include "moc_qnetworkaccessmanager.cpp"
