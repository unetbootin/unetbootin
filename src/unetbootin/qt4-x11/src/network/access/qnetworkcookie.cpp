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

#include "qnetworkcookie.h"
#include "qnetworkcookie_p.h"

#include "qnetworkrequest.h"
#include "qnetworkreply.h"
#include "QtCore/qbytearray.h"
#include "QtCore/qdebug.h"
#include "QtCore/qlist.h"
#include "QtCore/qlocale.h"
#include "QtCore/qstring.h"
#include "QtCore/qurl.h"
#include "private/qobject_p.h"

QT_BEGIN_NAMESPACE

/*!
    \class QNetworkCookie
    \since 4.4
    \brief The QNetworkCookie class holds one network cookie.

    Cookies are small bits of information that stateless protocols
    like HTTP use to maintain some persistent information across
    requests.

    A cookie is set by a remote server when it replies to a request
    and it expects the same cookie to be sent back when further
    requests are sent.

    QNetworkCookie holds one such cookie as received from the
    network. A cookie has a name and a value, but those are opaque to
    the application (that is, the information stored in them has no
    meaning to the application). A cookie has an associated path name
    and domain, which indicate when the cookie should be sent again to
    the server.

    A cookie can also have an expiration date, indicating its
    validity. If the expiration date is not present, the cookie is
    considered a "session cookie" and should be discarded when the
    application exits (or when its concept of session is over).

    QNetworkCookie provides a way of parsing a cookie from the HTTP
    header format using the QNetworkCookie::parseCookies()
    function. However, when received in a QNetworkReply, the cookie is
    already parsed.

    This class implements cookies as described by the
    \l{Netscape Cookie Specification}{initial cookie specification by
    Netscape}, which is somewhat similar to the \l{RFC 2109} specification,
    plus the \l{Mitigating Cross-site Scripting With HTTP-only Cookies}
    {"HttpOnly" extension}. The more recent \l{RFC 2965} specification
    (which uses the Set-Cookie2 header) is not supported.

    \sa QNetworkCookieJar, QNetworkRequest, QNetworkReply
*/

/*!
    Create a new QNetworkCookie object, initializing the cookie name
    to \a name and its value to \a value.

    A cookie is only valid if it has a name. However, the value is
    opaque to the application and being empty may have significance to
    the remote server.
*/
QNetworkCookie::QNetworkCookie(const QByteArray &name, const QByteArray &value)
    : d(new QNetworkCookiePrivate)
{
    qRegisterMetaType<QNetworkCookie>();
    qRegisterMetaType<QList<QNetworkCookie> >();

    d->name = name;
    d->value = value;
}

/*!
    Creates a new QNetworkCookie object by copying the contents of \a
    other.
*/
QNetworkCookie::QNetworkCookie(const QNetworkCookie &other)
    : d(other.d)
{
}

/*!
    Destroys this QNetworkCookie object.
*/
QNetworkCookie::~QNetworkCookie()
{
    // QSharedDataPointer auto deletes
    d = 0;
}

/*!
    Copies the contents of the QNetworkCookie object \a other to this
    object.
*/
QNetworkCookie &QNetworkCookie::operator=(const QNetworkCookie &other)
{
    d = other.d;
    return *this;
}

/*!
    \fn bool QNetworkCookie::operator!=(const QNetworkCookie &other) const

    Returns true if this cookie is not equal to \a other.

    \sa operator==()
*/

/*!
    Returns true if this cookie is equal to \a other. This function
    only returns true if all fields of the cookie are the same.

    However, in some contexts, two cookies of the same name could be
    considered equal.

    \sa operator!=()
*/
bool QNetworkCookie::operator==(const QNetworkCookie &other) const
{
    if (d == other.d)
        return true;
    return d->name == other.d->name &&
        d->value == other.d->value &&
        d->expirationDate == other.d->expirationDate &&
        d->domain == other.d->domain &&
        d->path == other.d->path &&
        d->secure == other.d->secure &&
        d->comment == other.d->comment;
}

/*!
    Returns true if the "secure" option was specified in the cookie
    string, false otherwise.

    Secure cookies may contain private information and should not be
    resent over unencrypted connections.

    \sa setSecure()
*/
bool QNetworkCookie::isSecure() const
{
    return d->secure;
}

/*!
    Sets the secure flag of this cookie to \a enable.

    Secure cookies may contain private information and should not be
    resent over unencrypted connections.

    \sa isSecure()
*/
void QNetworkCookie::setSecure(bool enable)
{
    d->secure = enable;
}

/*!
    \since 4.5

    Returns true if the "HttpOnly" flag is enabled for this cookie.

    A cookie that is "HttpOnly" is only set and retrieved by the
    network requests and replies; i.e., the HTTP protocol. It is not
    accessible from scripts running on browsers.

    \sa isSecure()
*/
bool QNetworkCookie::isHttpOnly() const
{
    return d->httpOnly;
}

/*!
    \since 4.5

    Sets this cookie's "HttpOnly" flag to \a enable.
*/
void QNetworkCookie::setHttpOnly(bool enable)
{
    d->httpOnly = enable;
}

/*!
    Returns true if this cookie is a session cookie. A session cookie
    is a cookie which has no expiration date, which means it should be
    discarded when the application's concept of session is over
    (usually, when the application exits).

    \sa expirationDate(), setExpirationDate()
*/
bool QNetworkCookie::isSessionCookie() const
{
    return !d->expirationDate.isValid();
}

/*!
    Returns the expiration date for this cookie. If this cookie is a
    session cookie, the QDateTime returned will not be valid. If the
    date is in the past, this cookie has already expired and should
    not be sent again back to a remote server.

    The expiration date corresponds to the parameters of the "expires"
    entry in the cookie string.

    \sa isSessionCookie(), setExpirationDate()
*/
QDateTime QNetworkCookie::expirationDate() const
{
    return d->expirationDate;
}

/*!
    Sets the expiration date of this cookie to \a date. Setting an
    invalid expiration date to this cookie will mean it's a session
    cookie.

    \sa isSessionCookie(), expirationDate()
*/
void QNetworkCookie::setExpirationDate(const QDateTime &date)
{
    d->expirationDate = date;
}

/*!
    Returns the domain this cookie is associated with. This
    corresponds to the "domain" field of the cookie string.

    Note that the domain here may start with a dot, which is not a
    valid hostname. However, it means this cookie matches all
    hostnames ending with that domain name.

    \sa setDomain()
*/
QString QNetworkCookie::domain() const
{
    return d->domain;
}

/*!
    Sets the domain associated with this cookie to be \a domain.

    \sa domain()
*/
void QNetworkCookie::setDomain(const QString &domain)
{
    d->domain = domain;
}

/*!
    Returns the path associated with this cookie. This corresponds to
    the "path" field of the cookie string.

    \sa setPath()
*/
QString QNetworkCookie::path() const
{
    return d->path;
}

/*!
    Sets the path associated with this cookie to be \a path.

    \sa path()
*/
void QNetworkCookie::setPath(const QString &path)
{
    d->path = path;
}

/*!
    Returns the name of this cookie. The only mandatory field of a
    cookie is its name, without which it is not considered valid.

    \sa setName(), value()
*/
QByteArray QNetworkCookie::name() const
{
    return d->name;
}

/*!
    Sets the name of this cookie to be \a cookieName. Note that
    setting a cookie name to an empty QByteArray will make this cookie
    invalid.

    \sa name(), value()
*/
void QNetworkCookie::setName(const QByteArray &cookieName)
{
    d->name = cookieName;
}

/*!
    Returns this cookies value, as specified in the cookie
    string. Note that a cookie is still valid if its value is empty.

    Cookie name-value pairs are considered opaque to the application:
    that is, their values don't mean anything.

    \sa setValue(), name()
*/
QByteArray QNetworkCookie::value() const
{
    return d->value;
}

/*!
    Sets the value of this cookie to be \a value.

    \sa value(), name()
*/
void QNetworkCookie::setValue(const QByteArray &value)
{
    d->value = value;
}

// ### move this to qnetworkcookie_p.h and share with qnetworkaccesshttpbackend
static QPair<QByteArray, QByteArray> nextField(const QByteArray &text, int &position)
{
    // format is one of:
    //    (1)  token
    //    (2)  token = token
    //    (3)  token = quoted-string
    int i;
    const int length = text.length();
    position = nextNonWhitespace(text, position);

    // parse the first part, before the equal sign
    for (i = position; i < length; ++i) {
        register char c = text.at(i);
        if (c == ';' || c == ',' || c == '=')
            break;
    }

    QByteArray first = text.mid(position, i - position).trimmed();
    position = i;

    if (first.isEmpty())
        return qMakePair(QByteArray(), QByteArray());
    if (i == length || text.at(i) != '=')
        // no equal sign, we found format (1)
        return qMakePair(first, QByteArray());

    QByteArray second;
    second.reserve(32);         // arbitrary but works for most cases

    i = nextNonWhitespace(text, position + 1);
    if (i < length && text.at(i) == '"') {
        // a quote, we found format (3), where:
        // quoted-string  = ( <"> *(qdtext | quoted-pair ) <"> )
        // qdtext         = <any TEXT except <">>
        // quoted-pair    = "\" CHAR
        ++i;
        while (i < length) {
            register char c = text.at(i);
            if (c == '"') {
                // end of quoted text
                break;
            } else if (c == '\\') {
                ++i;
                if (i >= length)
                    // broken line
                    return qMakePair(QByteArray(), QByteArray());
                c = text.at(i);
            }

            second += c;
            ++i;
        }

        for ( ; i < length; ++i) {
            register char c = text.at(i);
            if (c == ',' || c == ';')
                break;
        }
        position = i;
    } else {
        // no quote, we found format (2)
        position = i;
        for ( ; i < length; ++i) {
            register char c = text.at(i);
            if (c == ',' || c == ';' || isLWS(c))
                break;
        }

        second = text.mid(position, i - position).trimmed();
        position = i;
    }

    if (second.isNull())
        second.resize(0); // turns into empty-but-not-null
    return qMakePair(first, second);
}

/*!
    \enum QNetworkCookie::RawForm

    This enum is used with the toRawForm() function to declare which
    form of a cookie shall be returned.

    \value NameAndValueOnly     makes toRawForm() return only the
        "NAME=VALUE" part of the cookie, as suitable for sending back
        to a server in a client request's "Cookie:" header. Multiple
        cookies are separated by a semi-colon in the "Cookie:" header
        field.

    \value Full                 makes toRawForm() return the full
        cookie contents, as suitable for sending to a client in a
        server's "Set-Cookie:" header. Multiple cookies are separated
        by commas in a "Set-Cookie:" header.

    Note that only the Full form of the cookie can be parsed back into
    its original contents.

    \sa toRawForm(), parseCookies()
*/

/*!
    Returns the raw form of this QNetworkCookie. The QByteArray
    returned by this function is suitable for an HTTP header, either
    in a server response (the Set-Cookie header) or the client request
    (the Cookie header). You can choose from one of two formats, using
    \a form.

    \sa parseCookies()
*/
QByteArray QNetworkCookie::toRawForm(RawForm form) const
{
    QByteArray result;
    if (d->name.isEmpty())
        return result;          // not a valid cookie

    result = d->name;
    result += '=';
    if (d->value.contains(';') ||
        d->value.contains(',') ||
        d->value.contains(' ') ||
        d->value.contains('"')) {
        result += '"';

        QByteArray value = d->value;
        value.replace('"', "\\\"");
        result += value;

        result += '"';
    } else {
        result += d->value;
    }

    if (form == Full) {
        // same as above, but encoding everything back
        if (isSecure())
            result += "; secure";
        if (isHttpOnly())
            result += "; HttpOnly";
        if (!isSessionCookie()) {
            result += "; expires=";
            result += QLocale::c().toString(d->expirationDate.toUTC(),
                                            QLatin1String("ddd, dd-MMM-yyyy hh:mm:ss 'GMT")).toLatin1();
        }
        if (!d->domain.isEmpty()) {
            result += "; domain=";
            result += QUrl::toAce(d->domain);
        }
        if (!d->path.isEmpty()) {
            result += "; path=";
            result += QUrl::toPercentEncoding(d->path, "/");
        }
    }
    return result;
}

/*!
    Parses the cookie string \a cookieString as received from a server
    response in the "Set-Cookie:" header. If there's a parsing error,
    this function returns an empty list.

    Since the HTTP header can set more than one cookie at the same
    time, this function returns a QList<QNetworkCookie>, one for each
    cookie that is parsed.

    \sa toRawForm()
*/
QList<QNetworkCookie> QNetworkCookie::parseCookies(const QByteArray &cookieString)
{
    // According to http://wp.netscape.com/newsref/std/cookie_spec.html,<
    // the Set-Cookie response header is of the format:
    //
    //   Set-Cookie: NAME=VALUE; expires=DATE; path=PATH; domain=DOMAIN_NAME; secure
    //
    // where only the NAME=VALUE part is mandatory
    //
    // We do not support RFC 2965 Set-Cookie2-style cookies

    QList<QNetworkCookie> result;
    QDateTime now = QDateTime::currentDateTime().toUTC();

    int position = 0;
    const int length = cookieString.length();
    while (position < length) {
        QNetworkCookie cookie;

        // The first part is always the "NAME=VALUE" part
        QPair<QByteArray,QByteArray> field = nextField(cookieString, position);
        if (field.first.isEmpty() || field.second.isNull())
            // parsing error
            return QList<QNetworkCookie>();
        cookie.setName(field.first);
        cookie.setValue(field.second);

        position = nextNonWhitespace(cookieString, position);
        bool endOfCookie = false;
        while (!endOfCookie && position < length)
            switch (cookieString.at(position++)) {
            case ',':
                // end of the cookie
                endOfCookie = true;
                break;

            case ';':
                // new field in the cookie
                field = nextField(cookieString, position);
                field.first = field.first.toLower(); // everything but the NAME=VALUE is case-insensitive

                if (field.first == "expires") {
                    static const char dateFormats[] =
                        "d-MMM-yyyy hh:mm:ss\0"
                        "d MMM yyyy hh:mm:ss\0"
                        "d-MMM-yy hh:mm:ss\0"
                        "\0";

                    // expires is a special case because it contains a naked comma
                    // and naked spaces. The format is:
                    //   expires=ddd(d)?, dd-MMM-yyyy hh:mm:ss GMT
                    // but we also accept standard HTTP dates

                    // make sure we're at the comma
                    if (position >= length || cookieString.at(position) != ',')
                        // invalid cookie string
                        return QList<QNetworkCookie>();

                    ++position;
                    int end;
                    for (end = position; end < length; ++end)
                        if (cookieString.at(end) == ',' || cookieString.at(end) == ';')
                            break;

                    QByteArray datestring = cookieString.mid(position, end - position).trimmed();
                    position = end;
                    if (datestring.endsWith(" GMT") || datestring.endsWith(" UTC"))
                        datestring.chop(4);
                    else if (datestring.endsWith(" +0000"))
                        datestring.chop(6);

                    size_t i = 0;
                    int j = 0;
                    QLocale cLocale = QLocale::c();
                    QDateTime dt;
                    do {
                        QLatin1String df(dateFormats + i);
                        i += strlen(dateFormats + i) + 1;

#ifndef QT_NO_DATESTRING
                        dt = cLocale.toDateTime(QString::fromLatin1(datestring), df);

                        // some cookies are set with a two-digit year
                        // (although this is not allowed); this is interpreted as a year
                        // in the 20th century by QDateTime.
                        // Work around this case here (assuming 00-69 is 21st century,
                        //                                      70-99 is 20th century)
                        QDate date = dt.date();
                        if (j == 2 && date.year() >= 1900 && date.year() < 1970)
                            dt = dt.addYears(100);
                        if (date.year() >= 0 && date.year() < 100)
                            dt = dt.addYears(1900);
#endif
                        j++;
                    } while (!dt.isValid() && i <= sizeof dateFormats - 1);
                    if (!dt.isValid())
                        // invalid cookie string
                        return QList<QNetworkCookie>();

                    dt.setTimeSpec(Qt::UTC);
                    cookie.setExpirationDate(dt);
                } else if (field.first == "domain") {
                    QByteArray rawDomain = field.second;
                    QString maybeLeadingDot;
                    if (rawDomain.startsWith('.')) {
                        maybeLeadingDot = QLatin1Char('.');
                        rawDomain = rawDomain.mid(1);
                    }

                    QString normalizedDomain = QUrl::fromAce(QUrl::toAce(QString::fromUtf8(rawDomain)));
                    cookie.setDomain(maybeLeadingDot + normalizedDomain);
                } else if (field.first == "max-age") {
                    bool ok = false;
                    int secs = field.second.toInt(&ok);
                    if (!ok)
                        // invalid cookie string
                        return QList<QNetworkCookie>();
                    cookie.setExpirationDate(now.addSecs(secs));
                } else if (field.first == "path") {
                    QString path = QUrl::fromPercentEncoding(field.second);
                    cookie.setPath(path);
                } else if (field.first == "secure") {
                    cookie.setSecure(true);
                } else if (field.first == "httponly") {
                    cookie.setHttpOnly(true);
                } else if (field.first == "comment") {
                    //cookie.setComment(QString::fromUtf8(field.second));
                } else if (field.first == "version") {
                    if (field.second != "1") {
                        // oops, we don't know how to handle this cookie
                        cookie = QNetworkCookie();
                        endOfCookie = true;
                        continue;
                    }
                } else {
                    // got an unknown field in the cookie
                    // what do we do?
                }

                position = nextNonWhitespace(cookieString, position);
            }

        result += cookie;
    }

    return result;
}

#ifndef QT_NO_DEBUG_STREAM
QDebug operator<<(QDebug s, const QNetworkCookie &cookie)
{
    s.nospace() << "QNetworkCookie(" << cookie.toRawForm(QNetworkCookie::Full) << ")";
    return s.space();
}
#endif



class QNetworkCookieJarPrivate: public QObjectPrivate
{
public:
    QList<QNetworkCookie> allCookies;

    Q_DECLARE_PUBLIC(QNetworkCookieJar)
};

/*!
    \class QNetworkCookieJar
    \brief The QNetworkCookieJar class implements a simple jar of QNetworkCookie objects
    \since 4.4

    Cookies are small bits of information that stateless protocols
    like HTTP use to maintain some persistent information across
    requests.

    A cookie is set by a remote server when it replies to a request
    and it expects the same cookie to be sent back when further
    requests are sent.

    The cookie jar is the object that holds all cookies set in
    previous requests. Web browsers save their cookie jars to disk in
    order to conserve permanent cookies across invocations of the
    application.

    QNetworkCookieJar does not implement permanent storage: it only
    keeps the cookies in memory. Once the QNetworkCookieJar object is
    deleted, all cookies it held will be discarded as well. If you
    want to save the cookies, you should derive from this class and
    implement the saving to disk to your own storage format.

    This class implements only the basic security recommended by the
    cookie specifications and does not implement any cookie acceptance
    policy (it accepts all cookies set by any requests). In order to
    override those rules, you should reimplement the
    cookiesForUrl() and setCookiesFromUrl() virtual
    functions. They are called by QNetworkReply and
    QNetworkAccessManager when they detect new cookies and when they
    require cookies.

    \sa QNetworkCookie, QNetworkAccessManager, QNetworkReply,
    QNetworkRequest, QNetworkAccessManager::setCookieJar()
*/

/*!
    Creates a QNetworkCookieJar object and sets the parent object to
    be \a parent.

    The cookie jar is initialized to empty.
*/
QNetworkCookieJar::QNetworkCookieJar(QObject *parent)
    : QObject(*new QNetworkCookieJarPrivate, parent)
{
}

/*!
    Destroys this cookie jar object and discards all cookies stored in
    it. Cookies are not saved to disk in the QNetworkCookieJar default
    implementation.

    If you need to save the cookies to disk, you have to derive from
    QNetworkCookieJar and save the cookies to disk yourself.
*/
QNetworkCookieJar::~QNetworkCookieJar()
{
}

/*!
    Returns all cookies stored in this cookie jar. This function is
    suitable for derived classes to save cookies to disk, as well as
    to implement cookie expiration and other policies.

    \sa setAllCookies(), cookiesForUrl()
*/
QList<QNetworkCookie> QNetworkCookieJar::allCookies() const
{
    return d_func()->allCookies;
}

/*!
    Sets the internal list of cookies held by this cookie jar to be \a
    cookieList. This function is suitable for derived classes to
    implement loading cookies from permanent storage, or their own
    cookie acceptance policies by reimplementing
    setCookiesFromUrl().

    \sa allCookies(), setCookiesFromUrl()
*/
void QNetworkCookieJar::setAllCookies(const QList<QNetworkCookie> &cookieList)
{
    Q_D(QNetworkCookieJar);
    d->allCookies = cookieList;
}

static inline bool isParentPath(QString path, QString reference)
{
    if (!path.endsWith(QLatin1Char('/')))
        path += QLatin1Char('/');
    if (!reference.endsWith(QLatin1Char('/')))
        reference += QLatin1Char('/');
    return path.startsWith(reference);
}

static inline bool isParentDomain(QString domain, QString reference)
{
    if (!reference.startsWith(QLatin1Char('.')))
        return domain == reference;

    return domain.endsWith(reference) || domain == reference.mid(1);
}

/*!
    Adds the cookies in the list \a cookieList to this cookie
    jar. Default values for path and domain are taken from the \a
    url object.

    Returns true if one or more cookes are set for url otherwise false.

    If a cookie already exists in the cookie jar, it will be
    overridden by those in \a cookieList.

    The default QNetworkCookieJar class implements only a very basic
    security policy (it makes sure that the cookies' domain and path
    match the reply's). To enhance the security policy with your own
    algorithms, override setCookiesFromUrl().

    Also, QNetworkCookieJar does not have a maximum cookie jar
    size. Reimplement this function to discard older cookies to create
    room for new ones.

    \sa cookiesForUrl(), QNetworkAccessManager::setCookieJar()
*/
bool QNetworkCookieJar::setCookiesFromUrl(const QList<QNetworkCookie> &cookieList,
                                          const QUrl &url)
{
    Q_D(QNetworkCookieJar);
    QString defaultDomain = url.host();
    QString pathAndFileName = url.path();
    QString defaultPath = pathAndFileName.left(pathAndFileName.lastIndexOf(QLatin1Char('/'))+1);
    if (defaultPath.isEmpty())
        defaultPath = QLatin1Char('/');

    int added = 0;
    QDateTime now = QDateTime::currentDateTime();
    foreach (QNetworkCookie cookie, cookieList) {
        bool isDeletion = !cookie.isSessionCookie() &&
                          cookie.expirationDate() < now;

        // validate the cookie & set the defaults if unset
        // (RFC 2965: "The request-URI MUST path-match the Path attribute of the cookie.")
        if (cookie.path().isEmpty())
            cookie.setPath(defaultPath);
        else if (!isParentPath(pathAndFileName, cookie.path()))
            continue;           // not accepted

        if (cookie.domain().isEmpty()) {
            cookie.setDomain(defaultDomain);
        } else {
            QString domain = cookie.domain();
            if (!(isParentDomain(domain, defaultDomain)
                || isParentDomain(defaultDomain, domain))) {
                    continue;           // not accepted
            }
        }

        QList<QNetworkCookie>::Iterator it = d->allCookies.begin(),
                                       end = d->allCookies.end();
        for ( ; it != end; ++it)
            // does this cookie already exist?
            if (cookie.name() == it->name() &&
                cookie.domain() == it->domain() &&
                cookie.path() == it->path()) {
                // found a match
                d->allCookies.erase(it);
                break;
            }

        // did not find a match
        if (!isDeletion) {
            d->allCookies += cookie;
            ++added;
        }
    }
    return (added > 0);
}

/*!
    Returns the cookies to be added to when a request is sent to
    \a url. This function is called by the default
    QNetworkAccessManager::createRequest(), which adds the
    cookies returned by this function to the request being sent.

    If more than one cookie with the same name is found, but with
    differing paths, the one with longer path is returned before the
    one with shorter path. In other words, this function returns
    cookies sorted by path length.

    The default QNetworkCookieJar class implements only a very basic
    security policy (it makes sure that the cookies' domain and path
    match the reply's). To enhance the security policy with your own
    algorithms, override cookiesForUrl().

    \sa setCookiesFromUrl(), QNetworkAccessManager::setCookieJar()
*/
QList<QNetworkCookie> QNetworkCookieJar::cookiesForUrl(const QUrl &url) const
{
//     \b Warning! This is only a dumb implementation!
//     It does NOT follow all of the recommendations from
//     http://wp.netscape.com/newsref/std/cookie_spec.html
//     It does not implement a very good cross-domain verification yet.

    Q_D(const QNetworkCookieJar);
    QDateTime now = QDateTime::currentDateTime();
    QList<QNetworkCookie> result;

    // scan our cookies for something that matches
    QList<QNetworkCookie>::ConstIterator it = d->allCookies.constBegin(),
                                        end = d->allCookies.constEnd();
    for ( ; it != end; ++it) {
        if (!isParentDomain(url.host(), it->domain()))
            continue;
        if (!isParentPath(url.path(), it->path()))
            continue;
        if (!(*it).isSessionCookie() && (*it).expirationDate() < now)
            continue;

        // insert this cookie into result, sorted by path
        QList<QNetworkCookie>::Iterator insertIt = result.begin();
        while (insertIt != result.end()) {
            if (insertIt->path().length() < it->path().length()) {
                // insert here
                insertIt = result.insert(insertIt, *it);
                break;
            } else {
                ++insertIt;
            }
        }

        // this is the shortest path yet, just append
        if (insertIt == result.end())
            result += *it;
    }

    return result;
}

QT_END_NAMESPACE
