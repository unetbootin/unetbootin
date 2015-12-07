/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the QtCore module of the Qt Toolkit.
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

#ifndef QURL_H
#define QURL_H

#include <QtCore/qbytearray.h>
#include <QtCore/qobjectdefs.h>
#include <QtCore/qpair.h>
#include <QtCore/qstring.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Core)

class QUrlPrivate;
class QDataStream;

class Q_CORE_EXPORT QUrl
{
public:
    enum ParsingMode {
        TolerantMode,
        StrictMode
    };

    // encoding / toString values
    enum FormattingOption {
        None = 0x0,
        RemoveScheme = 0x1,
        RemovePassword = 0x2,
        RemoveUserInfo = RemovePassword | 0x4,
        RemovePort = 0x8,
        RemoveAuthority = RemoveUserInfo | RemovePort | 0x10,
        RemovePath = 0x20,
        RemoveQuery = 0x40,
        RemoveFragment = 0x80,

        StripTrailingSlash = 0x10000
    };
    Q_DECLARE_FLAGS(FormattingOptions, FormattingOption)

    QUrl();
    QUrl(const QString &url);
    QUrl(const QString &url, ParsingMode mode);
    // ### Qt 5: merge the two constructors, with mode = TolerantMode
    QUrl(const QUrl &copy);
    QUrl &operator =(const QUrl &copy);
    QUrl &operator =(const QString &url);
    ~QUrl();

    void setUrl(const QString &url);
    void setUrl(const QString &url, ParsingMode mode);
    // ### Qt 5: merge the two setUrl() functions, with mode = TolerantMode
    void setEncodedUrl(const QByteArray &url);
    void setEncodedUrl(const QByteArray &url, ParsingMode mode);
    // ### Qt 5: merge the two setEncodedUrl() functions, with mode = TolerantMode

    bool isValid() const;

    bool isEmpty() const;

    void clear();

    void setScheme(const QString &scheme);
    QString scheme() const;

    void setAuthority(const QString &authority);
    QString authority() const;

    void setUserInfo(const QString &userInfo);
    QString userInfo() const;

    void setUserName(const QString &userName);
    QString userName() const;
    void setEncodedUserName(const QByteArray &userName);
    QByteArray encodedUserName() const;

    void setPassword(const QString &password);
    QString password() const;
    void setEncodedPassword(const QByteArray &password);
    QByteArray encodedPassword() const;

    void setHost(const QString &host);
    QString host() const;
    void setEncodedHost(const QByteArray &host);
    QByteArray encodedHost() const;

    void setPort(int port);
    int port() const;
    int port(int defaultPort) const;
    // ### Qt 5: merge the two port() functions, with defaultPort = -1

    void setPath(const QString &path);
    QString path() const;
    void setEncodedPath(const QByteArray &path);
    QByteArray encodedPath() const;

    bool hasQuery() const;

    void setEncodedQuery(const QByteArray &query);
    QByteArray encodedQuery() const;

    void setQueryDelimiters(char valueDelimiter, char pairDelimiter);
    char queryValueDelimiter() const;
    char queryPairDelimiter() const;

    void setQueryItems(const QList<QPair<QString, QString> > &query);
    void addQueryItem(const QString &key, const QString &value);
    QList<QPair<QString, QString> > queryItems() const;
    bool hasQueryItem(const QString &key) const;
    QString queryItemValue(const QString &key) const;
    QStringList allQueryItemValues(const QString &key) const;
    void removeQueryItem(const QString &key);
    void removeAllQueryItems(const QString &key);

    void setEncodedQueryItems(const QList<QPair<QByteArray, QByteArray> > &query);
    void addEncodedQueryItem(const QByteArray &key, const QByteArray &value);
    QList<QPair<QByteArray, QByteArray> > encodedQueryItems() const;
    bool hasEncodedQueryItem(const QByteArray &key) const;
    QByteArray encodedQueryItemValue(const QByteArray &key) const;
    QList<QByteArray> allEncodedQueryItemValues(const QByteArray &key) const;
    void removeEncodedQueryItem(const QByteArray &key);
    void removeAllEncodedQueryItems(const QByteArray &key);

    void setFragment(const QString &fragment);
    QString fragment() const;
    void setEncodedFragment(const QByteArray &fragment);
    QByteArray encodedFragment() const;
    bool hasFragment() const;

    QUrl resolved(const QUrl &relative) const;

    bool isRelative() const;
    bool isParentOf(const QUrl &url) const;

    static QUrl fromLocalFile(const QString &localfile);
    QString toLocalFile() const;

    QString toString(FormattingOptions options = None) const;

    QByteArray toEncoded(FormattingOptions options = None) const;
    static QUrl fromEncoded(const QByteArray &url);
    static QUrl fromEncoded(const QByteArray &url, ParsingMode mode);
    // ### Qt 5: merge the two fromEncoded() functions, with mode = TolerantMode

    void detach();
    bool isDetached() const;

    bool operator <(const QUrl &url) const;
    bool operator ==(const QUrl &url) const;
    bool operator !=(const QUrl &url) const;

    static QString fromPercentEncoding(const QByteArray &);
    static QByteArray toPercentEncoding(const QString &,
                                        const QByteArray &exclude = QByteArray(),
                                        const QByteArray &include = QByteArray());
    static QString fromPunycode(const QByteArray &);
    static QByteArray toPunycode(const QString &);
    static QString fromAce(const QByteArray &);
    static QByteArray toAce(const QString &);
    static QStringList idnWhitelist();
    static void setIdnWhitelist(const QStringList &);

#if defined QT3_SUPPORT
    inline QT3_SUPPORT QString protocol() const { return scheme(); }
    inline QT3_SUPPORT void setProtocol(const QString &s) { setScheme(s); }
    inline QT3_SUPPORT void setUser(const QString &s) { setUserName(s); }
    inline QT3_SUPPORT QString user() const { return userName(); }
    inline QT3_SUPPORT bool hasUser() const { return !userName().isEmpty(); }
    inline QT3_SUPPORT bool hasPassword() const { return !password().isEmpty(); }
    inline QT3_SUPPORT bool hasHost() const { return !host().isEmpty(); }
    inline QT3_SUPPORT bool hasPort() const { return port() != -1; }
    inline QT3_SUPPORT bool hasPath() const { return !path().isEmpty(); }
    inline QT3_SUPPORT void setQuery(const QString &txt)
    {
        setEncodedQuery(txt.toLatin1());
    }
    inline QT3_SUPPORT QString query() const
    {
        return QString::fromLatin1(encodedQuery());
    }
    inline QT3_SUPPORT QString ref() const { return fragment(); }
    inline QT3_SUPPORT void setRef(const QString &txt) { setFragment(txt); }
    inline QT3_SUPPORT bool hasRef() const { return !fragment().isEmpty(); }
    inline QT3_SUPPORT void addPath(const QString &p) { setPath(path() + QLatin1String("/") + p); }
    QT3_SUPPORT void setFileName(const QString &txt);
    QT3_SUPPORT QString fileName() const;
    QT3_SUPPORT QString dirPath() const;
    static inline QT3_SUPPORT void decode(QString &url)
    {
        url = QUrl::fromPercentEncoding(url.toLatin1());
    }
    static inline QT3_SUPPORT void encode(QString &url)
    {
        url = QString::fromLatin1(QUrl::toPercentEncoding(url));
    }
    inline QT3_SUPPORT operator QString() const { return toString(); }
    inline QT3_SUPPORT bool cdUp()
    {
        *this = resolved(QUrl(QLatin1String("..")));
        return true;
    }
    static inline QT3_SUPPORT bool isRelativeUrl(const QString &url)
    {
        return QUrl(url).isRelative();
    }
#endif

    QString errorString() const;

protected:
#if defined (QT3_SUPPORT)
    inline QT3_SUPPORT void reset() { clear(); }
#endif

private:
    QUrlPrivate *d;
public:
    typedef QUrlPrivate * DataPtr;
    inline DataPtr &data_ptr() { return d; }
};

Q_DECLARE_TYPEINFO(QUrl, Q_MOVABLE_TYPE);
Q_DECLARE_SHARED(QUrl)
Q_DECLARE_OPERATORS_FOR_FLAGS(QUrl::FormattingOptions)

#ifndef QT_NO_DATASTREAM
Q_CORE_EXPORT QDataStream &operator<<(QDataStream &, const QUrl &);
Q_CORE_EXPORT QDataStream &operator>>(QDataStream &, QUrl &);
#endif

#ifndef QT_NO_DEBUG_STREAM
Q_CORE_EXPORT QDebug operator<<(QDebug, const QUrl &);
#endif

QT_END_NAMESPACE

QT_END_HEADER

#endif // QURL_H
