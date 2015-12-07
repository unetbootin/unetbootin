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

#include "qplatformdefs.h"
#include "qnetworkrequest.h"
#include "qnetworkcookie.h"
#include "qnetworkrequest_p.h"
#include "qsslconfiguration.h"
#include "QtCore/qshareddata.h"
#include "QtCore/qlocale.h"
#include "QtCore/qdatetime.h"

QT_BEGIN_NAMESPACE

/*!
    \class QNetworkRequest
    \brief The QNetworkRequest class holds one request to be sent with the Network Access API.
    \since 4.4

    \ingroup io
    \inmodule QtNetwork

    QNetworkRequest is part of the Network Access API and is the class
    holding the information necessary to send a request over the
    network. It contains a URL and some ancillary information that can
    be used to modify the request.

    \sa QNetworkReply, QNetworkAccessManager
*/

/*!
    \enum QNetworkRequest::KnownHeaders

    List of known header types that QNetworkRequest parses. Each known
    header is also represented in raw form with its full HTTP name.

    \value ContentTypeHeader    corresponds to the HTTP Content-Type
    header and contains a string containing the media (MIME) type and
    any auxiliary data (for instance, charset)

    \value ContentLengthHeader  corresponds to the HTTP Content-Length
    header and contains the length in bytes of the data transmitted.

    \value LocationHeader       corresponds to the HTTP Location
    header and contains a URL representing the actual location of the
    data, including the destination URL in case of redirections.

    \value LastModifiedHeader   corresponds to the HTTP Last-Modified
    header and contains a QDateTime representing the last modification
    date of the contents

    \value CookieHeader         corresponds to the HTTP Cookie header
    and contains a QList<QNetworkCookie> representing the cookies to
    be sent back to the server

    \value SetCookieHeader      corresponds to the HTTP Set-Cookie
    header and contains a QList<QNetworkCookie> representing the
    cookies sent by the server to be stored locally

    \sa header(), setHeader(), rawHeader(), setRawHeader()
*/

/*!
    \enum QNetworkRequest::Attribute

    Attribute codes for the QNetworkRequest and QNetworkReply.

    Attributes are extra meta-data that are used to control the
    behavior of the request and to pass further information from the
    reply back to the application. Attributes are also extensible,
    allowing custom implementations to pass custom values.

    The following table explains what the default attribute codes are,
    the QVariant types associated, the default value if said attribute
    is missing and whether it's used in requests or replies.

    \value HttpStatusCodeAttribute
        Replies only, type: QVariant::Int (no default)
        Indicates the HTTP status code received from the HTTP server
        (like 200, 304, 404, 401, etc.). If the connection was not
        HTTP-based, this attribute will not be present.

    \value HttpReasonPhraseAttribute
        Replies only, type: QVariant::ByteArray (no default)
        Indicates the HTTP reason phrase as received from the HTTP
        server (like "Ok", "Found", "Not Found", "Access Denied",
        etc.) This is the human-readable representation of the status
        code (see above). If the connection was not HTTP-based, this
        attribute will not be present.

    \value RedirectionTargetAttribute
        Replies only, type: QVariant::Url (no default)
        If present, it indicates that the server is redirecting the
        request to a different URL. The Network Access API does not by
        default follow redirections: it's up to the application to
        determine if the requested redirection should be allowed,
        according to its security policies.

    \value ConnectionEncryptedAttribute
        Replies only, type: QVariant::Bool (default: false)
        Indicates whether the data was obtained through an encrypted
        (secure) connection.

    \value CacheLoadControlAttribute
        Requests only, type: QVariant::Int (default: QNetworkRequest::PreferNetwork)
        Controls how the cache should be accessed. The possible values
        are those of QNetworkRequest::CacheLoadControl. Note that the
        default QNetworkAccessManager implementation does not support
        caching. However, this attribute may be used by certain
        backends to modify their requests (for example, for caching proxies).

    \value CacheSaveControlAttribute
        Requests only, type: QVariant::Bool (default: true)
        Controls if the data obtained should be saved to cache for
        future uses. If the value is false, the data obtained will not
        be automatically cached. If true, data may be cached, provided
        it is cacheable (what is cacheable depends on the protocol
        being used). Note that the default QNetworkAccessManager
        implementation does not support caching, so it will ignore
        this attribute.

    \value SourceIsFromCacheAttribute
        Replies only, type: QVariant::Bool (default: false)
        Indicates whether the data was obtained from cache
        or not.

    \value User
        Special type. Additional information can be passed in
        QVariants with types ranging from User to UserMax. The default
        implementation of Network Access will ignore any request
        attributes in this range and it will not produce any
        attributes in this range in replies. The range is reserved for
        extensions of QNetworkAccessManager.

    \value UserMax
        Special type. See User.
*/

/*!
    \enum QNetworkRequest::CacheLoadControl

    Controls the caching mechanism of QNetworkAccessManager.

    \value AlwaysNetwork        always load from network and do not
    check if the cache has a valid entry (similar to the
    "Reload" feature in browsers)

    \value PreferNetwork        default value; load from the network
    if the cached entry is older than the network entry

    \value PreferCache          load from cache if available,
    otherwise load from network. Note that this can return possibly
    stale (but not expired) items from cache.

    \value AlwaysCache          only load from cache, indicating error
    if the item was not cached (i.e., off-line mode)
*/

class QNetworkRequestPrivate: public QSharedData, public QNetworkHeadersPrivate
{
public:
    inline QNetworkRequestPrivate()
#ifndef QT_NO_OPENSSL
        : sslConfiguration(0)
#endif
    { qRegisterMetaType<QNetworkRequest>(); }
    ~QNetworkRequestPrivate()
    {
#ifndef QT_NO_OPENSSL
        delete sslConfiguration;
#endif
    }


    QNetworkRequestPrivate(const QNetworkRequestPrivate &other)
        : QSharedData(other), QNetworkHeadersPrivate(other)
    {
        url = other.url;

#ifndef QT_NO_OPENSSL
        if (other.sslConfiguration)
            sslConfiguration = new QSslConfiguration(*other.sslConfiguration);
        else
            sslConfiguration = 0;
#endif
    }

    inline bool operator==(const QNetworkRequestPrivate &other) const
    {
        return url == other.url &&
            rawHeaders == other.rawHeaders &&
            attributes == other.attributes;
        // don't compare cookedHeaders
    }

    QUrl url;
#ifndef QT_NO_OPENSSL
    mutable QSslConfiguration *sslConfiguration;
#endif
};

/*!
    Constructs a QNetworkRequest object with \a url as the URL to be
    requested.

    \sa url(), setUrl()
*/
QNetworkRequest::QNetworkRequest(const QUrl &url)
    : d(new QNetworkRequestPrivate)
{
    d->url = url;
}

/*!
    Creates a copy of \a other.
*/
QNetworkRequest::QNetworkRequest(const QNetworkRequest &other)
    : d(other.d)
{
}

/*!
    Disposes of the QNetworkRequest object.
*/
QNetworkRequest::~QNetworkRequest()
{
    // QSharedDataPointer auto deletes
    d = 0;
}

/*!
    Returns true if this object is the same as \a other (i.e., if they
    have the same URL, same headers and same meta-data settings).

    \sa operator!=()
*/
bool QNetworkRequest::operator==(const QNetworkRequest &other) const
{
    return d == other.d || *d == *other.d;
}

/*!
    \fn bool QNetworkRequest::operator!=(const QNetworkRequest &other) const

    Returns false if this object is not the same as \a other.

    \sa operator==()
*/

/*!
    Creates a copy of \a other
*/
QNetworkRequest &QNetworkRequest::operator=(const QNetworkRequest &other)
{
    d = other.d;
    return *this;
}

/*!
    Returns the URL this network request is referring to.

    \sa setUrl()
*/
QUrl QNetworkRequest::url() const
{
    return d->url;
}

/*!
    Sets the URL this network request is referring to to be \a url.

    \sa url()
*/
void QNetworkRequest::setUrl(const QUrl &url)
{
    d->url = url;
}

/*!
    Returns the value of the known network header \a header if it is
    present in this request. If it is not present, returns QVariant()
    (i.e., an invalid variant).

    \sa KnownHeaders, rawHeader(), setHeader()
*/
QVariant QNetworkRequest::header(KnownHeaders header) const
{
    return d->cookedHeaders.value(header);
}

/*!
    Sets the value of the known header \a header to be \a value,
    overriding any previously set headers. This operation also sets
    the equivalent raw HTTP header.

    \sa KnownHeaders, setRawHeader(), header()
*/
void QNetworkRequest::setHeader(KnownHeaders header, const QVariant &value)
{
    d->setCookedHeader(header, value);
}

/*!
    Returns true if the raw header \a headerName is present in this
    network request.

    \sa rawHeader(), setRawHeader()
*/
bool QNetworkRequest::hasRawHeader(const QByteArray &headerName) const
{
    return d->findRawHeader(headerName) != d->rawHeaders.constEnd();
}

/*!
    Returns the raw form of header \a headerName. If no such header is
    present, an empty QByteArray is returned, which may be
    indistinguishable from a header that is present but has no content
    (use hasRawHeader() to find out if the header exists or not).

    Raw headers can be set with setRawHeader() or with setHeader().

    \sa header(), setRawHeader()
*/
QByteArray QNetworkRequest::rawHeader(const QByteArray &headerName) const
{
    QNetworkHeadersPrivate::RawHeadersList::ConstIterator it =
        d->findRawHeader(headerName);
    if (it != d->rawHeaders.constEnd())
        return it->second;
    return QByteArray();
}

/*!
    Returns a list of all raw headers that are set in this network
    request. The list is in the order that the headers were set.

    \sa hasRawHeader(), rawHeader()
*/
QList<QByteArray> QNetworkRequest::rawHeaderList() const
{
    return d->rawHeadersKeys();
}

/*!
    Sets the header \a headerName to be of value \a headerValue. If \a
    headerName corresponds to a known header (see
    QNetworkRequest::KnownHeaders), the raw format will be parsed and
    the corresponding "cooked" header will be set as well.

    For example:
    \snippet doc/src/snippets/code/src_network_access_qnetworkrequest.cpp 0

    will also set the known header LastModifiedHeader to be the
    QDateTime object of the parsed date.

    Note: setting the same header twice overrides the previous
    setting. To accomplish the behaviour of multiple HTTP headers of
    the same name, you should concatenate the two values, separating
    them with a comma (",") and set one single raw header.

    \sa KnownHeaders, setHeader(), hasRawHeader(), rawHeader()
*/
void QNetworkRequest::setRawHeader(const QByteArray &headerName, const QByteArray &headerValue)
{
    d->setRawHeader(headerName, headerValue);
}

/*!
    Returns the attribute associated with the code \a code. If the
    attribute has not been set, it returns \a defaultValue.

    Note: this function does not apply the defaults listed in
    QNetworkRequest::Attribute.

    \sa setAttribute(), QNetworkRequest::Attribute
*/
QVariant QNetworkRequest::attribute(Attribute code, const QVariant &defaultValue) const
{
    return d->attributes.value(code, defaultValue);
}

/*!
    Sets the attribute associated with code \a code to be value \a
    value. If the attribute is already set, the previous value is
    discarded. In special, if \a value is an invalid QVariant, the
    attribute is unset.

    \sa attribute(), QNetworkRequest::Attribute
*/
void QNetworkRequest::setAttribute(Attribute code, const QVariant &value)
{
    if (value.isValid())
        d->attributes.insert(code, value);
    else
        d->attributes.remove(code);
}

#ifndef QT_NO_OPENSSL
/*!
    Returns this network request's SSL configuration. By default, no
    SSL settings are specified.

    \sa setSslConfiguration()
*/
QSslConfiguration QNetworkRequest::sslConfiguration() const
{
    if (!d->sslConfiguration)
        d->sslConfiguration = new QSslConfiguration;
    return *d->sslConfiguration;
}

/*!
    Sets this network request's SSL configuration to be \a config. The
    settings that apply are the private key, the local certificate,
    the SSL protocol (SSLv2, SSLv3, TLSv1 where applicable) and the
    ciphers that the SSL backend is allowed to use.

    By default, no SSL configuration is set, which allows the backends
    to choose freely what configuration is best for them.

    \sa sslConfiguration(), QSslConfiguration::defaultConfiguration()
*/
void QNetworkRequest::setSslConfiguration(const QSslConfiguration &config)
{
    if (!d->sslConfiguration)
        d->sslConfiguration = new QSslConfiguration(config);
    else
        *d->sslConfiguration = config;
}
#endif

static QByteArray headerName(QNetworkRequest::KnownHeaders header)
{
    switch (header) {
    case QNetworkRequest::ContentTypeHeader:
        return "Content-Type";

    case QNetworkRequest::ContentLengthHeader:
        return "Content-Length";

    case QNetworkRequest::LocationHeader:
        return "Location";

    case QNetworkRequest::LastModifiedHeader:
        return "Last-Modified";

    case QNetworkRequest::CookieHeader:
        return "Cookie";

    case QNetworkRequest::SetCookieHeader:
        return "Set-Cookie";

    // no default:
    // if new values are added, this will generate a compiler warning
    }

    return QByteArray();
}

static QByteArray headerValue(QNetworkRequest::KnownHeaders header, const QVariant &value)
{
    switch (header) {
    case QNetworkRequest::ContentTypeHeader:
    case QNetworkRequest::ContentLengthHeader:
        return value.toByteArray();

    case QNetworkRequest::LocationHeader:
        switch (value.type()) {
        case QVariant::Url:
            return value.toUrl().toEncoded();

        default:
            return value.toByteArray();
        }

    case QNetworkRequest::LastModifiedHeader:
        switch (value.type()) {
        case QVariant::Date:
        case QVariant::DateTime:
            // generate RFC 1123/822 dates:
            return QNetworkHeadersPrivate::toHttpDate(value.toDateTime());

        default:
            return value.toByteArray();
        }

    case QNetworkRequest::CookieHeader: {
        QList<QNetworkCookie> cookies = qvariant_cast<QList<QNetworkCookie> >(value);
        if (cookies.isEmpty() && value.userType() == qMetaTypeId<QNetworkCookie>())
            cookies << qvariant_cast<QNetworkCookie>(value);

        QByteArray result;
        bool first = true;
        foreach (const QNetworkCookie &cookie, cookies) {
            if (!first)
                result += "; ";
            first = false;
            result += cookie.toRawForm(QNetworkCookie::NameAndValueOnly);
        }
        return result;
    }

    case QNetworkRequest::SetCookieHeader: {
        QList<QNetworkCookie> cookies = qvariant_cast<QList<QNetworkCookie> >(value);
        if (cookies.isEmpty() && value.userType() == qMetaTypeId<QNetworkCookie>())
            cookies << qvariant_cast<QNetworkCookie>(value);

        QByteArray result;
        bool first = true;
        foreach (const QNetworkCookie &cookie, cookies) {
            if (!first)
                result += ", ";
            first = false;
            result += cookie.toRawForm(QNetworkCookie::Full);
        }
        return result;
    }
    }

    return QByteArray();
}

static QNetworkRequest::KnownHeaders parseHeaderName(const QByteArray &headerName)
{
    // headerName is not empty here

    QByteArray lower = headerName.toLower();
    switch (lower.at(0)) {
    case 'c':
        if (lower == "content-type")
            return QNetworkRequest::ContentTypeHeader;
        else if (lower == "content-length")
            return QNetworkRequest::ContentLengthHeader;
        else if (lower == "cookie")
            return QNetworkRequest::CookieHeader;
        break;

    case 'l':
        if (lower == "location")
            return QNetworkRequest::LocationHeader;
        else if (lower == "last-modified")
            return QNetworkRequest::LastModifiedHeader;
        break;

    case 's':
        if (lower == "set-cookie")
            return QNetworkRequest::SetCookieHeader;
        break;
    }

    return QNetworkRequest::KnownHeaders(-1); // nothing found
}

static QVariant parseHttpDate(const QByteArray &raw)
{
    QDateTime dt = QNetworkHeadersPrivate::fromHttpDate(raw);
    if (dt.isValid())
        return dt;
    return QVariant();          // transform an invalid QDateTime into a null QVariant
}

static QVariant parseCookieHeader(const QByteArray &raw)
{
    QList<QNetworkCookie> result;
    QList<QByteArray> cookieList = raw.split(';');
    foreach (QByteArray cookie, cookieList) {
        QList<QNetworkCookie> parsed = QNetworkCookie::parseCookies(cookie.trimmed());
        if (parsed.count() != 1)
            return QVariant();  // invalid Cookie: header

        result += parsed;
    }

    return qVariantFromValue(result);
}

static QVariant parseHeaderValue(QNetworkRequest::KnownHeaders header, const QByteArray &value)
{
    // header is always a valid value
    switch (header) {
    case QNetworkRequest::ContentTypeHeader:
        // copy exactly, convert to QString
        return QString::fromLatin1(value);

    case QNetworkRequest::ContentLengthHeader: {
        bool ok;
        qint64 result = value.trimmed().toLongLong(&ok);
        if (ok)
            return result;
        return QVariant();
    }

    case QNetworkRequest::LocationHeader: {
        QUrl result = QUrl::fromEncoded(value, QUrl::StrictMode);
        if (result.isValid() && !result.scheme().isEmpty())
            return result;
        return QVariant();
    }

    case QNetworkRequest::LastModifiedHeader:
        return parseHttpDate(value);

    case QNetworkRequest::CookieHeader:
        return parseCookieHeader(value);

    case QNetworkRequest::SetCookieHeader:
        return qVariantFromValue(QNetworkCookie::parseCookies(value));

    default:
        Q_ASSERT(0);
    }
    return QVariant();
}

QNetworkHeadersPrivate::RawHeadersList::ConstIterator
QNetworkHeadersPrivate::findRawHeader(const QByteArray &key) const
{
    QByteArray lowerKey = key.toLower();
    RawHeadersList::ConstIterator it = rawHeaders.constBegin();
    RawHeadersList::ConstIterator end = rawHeaders.constEnd();
    for ( ; it != end; ++it)
        if (it->first.toLower() == lowerKey)
            return it;

    return end;                 // not found
}

QList<QByteArray> QNetworkHeadersPrivate::rawHeadersKeys() const
{
    QList<QByteArray> result;
    RawHeadersList::ConstIterator it = rawHeaders.constBegin(),
                                 end = rawHeaders.constEnd();
    for ( ; it != end; ++it)
        result << it->first;

    return result;
}

void QNetworkHeadersPrivate::setRawHeader(const QByteArray &key, const QByteArray &value)
{
    if (key.isEmpty())
        // refuse to accept an empty raw header
        return;

    setRawHeaderInternal(key, value);
    parseAndSetHeader(key, value);
}

/*!
    \internal
    Sets the internal raw headers list to match \a list. The cooked headers
    will also be updated.

    If \a list contains duplicates, they will be stored, but only the first one
    is usually accessed.
*/
void QNetworkHeadersPrivate::setAllRawHeaders(const RawHeadersList &list)
{
    cookedHeaders.clear();
    rawHeaders = list;

    RawHeadersList::ConstIterator it = rawHeaders.constBegin();
    RawHeadersList::ConstIterator end = rawHeaders.constEnd();
    for ( ; it != end; ++it)
        parseAndSetHeader(it->first, it->second);
}

void QNetworkHeadersPrivate::setCookedHeader(QNetworkRequest::KnownHeaders header,
                                             const QVariant &value)
{
    QByteArray name = headerName(header);
    if (name.isEmpty()) {
        // headerName verifies that \a header is a known value
        qWarning("QNetworkRequest::setHeader: invalid header value KnownHeader(%d) received", header);
        return;
    }

    if (value.isNull()) {
        setRawHeaderInternal(name, QByteArray());
        cookedHeaders.remove(header);
    } else {
        QByteArray rawValue = headerValue(header, value);
        if (rawValue.isEmpty()) {
            qWarning("QNetworkRequest::setHeader: QVariant of type %s cannot be used with header %s",
                     value.typeName(), name.constData());
            return;
        }

        setRawHeaderInternal(name, rawValue);
        cookedHeaders.insert(header, value);
    }
}

void QNetworkHeadersPrivate::setRawHeaderInternal(const QByteArray &key, const QByteArray &value)
{
    QByteArray lowerKey = key.toLower();
    RawHeadersList::Iterator it = rawHeaders.begin();
    while (it != rawHeaders.end()) {
        if (it->first.toLower() == lowerKey)
            it = rawHeaders.erase(it);
        else
            ++it;
    }

    if (value.isNull())
        return;                 // only wanted to erase key

    RawHeaderPair pair;
    pair.first = key;
    pair.second = value;
    rawHeaders.append(pair);
}

void QNetworkHeadersPrivate::parseAndSetHeader(const QByteArray &key, const QByteArray &value)
{
    // is it a known header?
    QNetworkRequest::KnownHeaders parsedKey = parseHeaderName(key);
    if (parsedKey != QNetworkRequest::KnownHeaders(-1)) {
        if (value.isNull())
            cookedHeaders.remove(parsedKey);
        else
            cookedHeaders.insert(parsedKey, parseHeaderValue(parsedKey, value));
    }
}

QDateTime QNetworkHeadersPrivate::fromHttpDate(const QByteArray &value)
{
    // HTTP dates have three possible formats:
    //  RFC 1123/822      -   ddd, dd MMM yyyy hh:mm:ss "GMT"
    //  RFC 850           -   dddd, dd-MMM-yy hh:mm:ss "GMT"
    //  ANSI C's asctime  -   ddd MMM d hh:mm:ss yyyy
    // We only handle them exactly. If they deviate, we bail out.

    int pos = value.indexOf(',');
    QDateTime dt;
#ifndef QT_NO_DATESTRING
    if (pos == -1) {
        // no comma -> asctime(3) format
        dt = QDateTime::fromString(QString::fromLatin1(value), Qt::TextDate);
    } else {
        // eat the weekday, the comma and the space following it
        QString sansWeekday = QString::fromLatin1(value.constData() + pos + 2);

        QLocale c = QLocale::c();
        if (pos == 3)
            // must be RFC 1123 date
            dt = c.toDateTime(sansWeekday, QLatin1String("dd MMM yyyy hh:mm:ss 'GMT"));
        else
            // must be RFC 850 date
            dt = c.toDateTime(sansWeekday, QLatin1String("dd-MMM-yy hh:mm:ss 'GMT'"));
    }
#endif // QT_NO_DATESTRING

    if (dt.isValid())
        dt.setTimeSpec(Qt::UTC);
    return dt;
}

QByteArray QNetworkHeadersPrivate::toHttpDate(const QDateTime &dt)
{
    return QLocale::c().toString(dt, QLatin1String("ddd, dd MMM yyyy hh:mm:ss 'GMT'"))
        .toLatin1();
}

QT_END_NAMESPACE
