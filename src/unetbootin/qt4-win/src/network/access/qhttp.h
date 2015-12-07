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

#ifndef QHTTP_H
#define QHTTP_H

#include <QtCore/qobject.h>
#include <QtCore/qstringlist.h>
#include <QtCore/qmap.h>
#include <QtCore/qpair.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Network)

#ifndef QT_NO_HTTP

class QTcpSocket;
class QTimerEvent;
class QIODevice;
class QAuthenticator;
class QNetworkProxy;
class QSslError;

class QHttpPrivate;

class QHttpHeaderPrivate;
class Q_NETWORK_EXPORT QHttpHeader
{
public:
    QHttpHeader();
    QHttpHeader(const QHttpHeader &header);
    QHttpHeader(const QString &str);
    virtual ~QHttpHeader();

    QHttpHeader &operator=(const QHttpHeader &h);

    void setValue(const QString &key, const QString &value);
    void setValues(const QList<QPair<QString, QString> > &values);
    void addValue(const QString &key, const QString &value);
    QList<QPair<QString, QString> > values() const;
    bool hasKey(const QString &key) const;
    QStringList keys() const;
    QString value(const QString &key) const;
    QStringList allValues(const QString &key) const;
    void removeValue(const QString &key);
    void removeAllValues(const QString &key);

    // ### Qt 5: change to qint64
    bool hasContentLength() const;
    uint contentLength() const;
    void setContentLength(int len);

    bool hasContentType() const;
    QString contentType() const;
    void setContentType(const QString &type);

    virtual QString toString() const;
    bool isValid() const;

    virtual int majorVersion() const = 0;
    virtual int minorVersion() const = 0;

protected:
    virtual bool parseLine(const QString &line, int number);
    bool parse(const QString &str);
    void setValid(bool);

    QHttpHeader(QHttpHeaderPrivate &dd, const QString &str = QString());
    QHttpHeader(QHttpHeaderPrivate &dd, const QHttpHeader &header);
    QHttpHeaderPrivate *d_ptr;

private:
    Q_DECLARE_PRIVATE(QHttpHeader)
};

class QHttpResponseHeaderPrivate;
class Q_NETWORK_EXPORT QHttpResponseHeader : public QHttpHeader
{
public:
    QHttpResponseHeader();
    QHttpResponseHeader(const QHttpResponseHeader &header);
    QHttpResponseHeader(const QString &str);
    QHttpResponseHeader(int code, const QString &text = QString(), int majorVer = 1, int minorVer = 1);
    QHttpResponseHeader &operator=(const QHttpResponseHeader &header);

    void setStatusLine(int code, const QString &text = QString(), int majorVer = 1, int minorVer = 1);

    int statusCode() const;
    QString reasonPhrase() const;

    int majorVersion() const;
    int minorVersion() const;

    QString toString() const;

protected:
    bool parseLine(const QString &line, int number);

private:
    Q_DECLARE_PRIVATE(QHttpResponseHeader)
    friend class QHttpPrivate;
};

class QHttpRequestHeaderPrivate;
class Q_NETWORK_EXPORT QHttpRequestHeader : public QHttpHeader
{
public:
    QHttpRequestHeader();
    QHttpRequestHeader(const QString &method, const QString &path, int majorVer = 1, int minorVer = 1);
    QHttpRequestHeader(const QHttpRequestHeader &header);
    QHttpRequestHeader(const QString &str);
    QHttpRequestHeader &operator=(const QHttpRequestHeader &header);

    void setRequest(const QString &method, const QString &path, int majorVer = 1, int minorVer = 1);

    QString method() const;
    QString path() const;

    int majorVersion() const;
    int minorVersion() const;

    QString toString() const;

protected:
    bool parseLine(const QString &line, int number);

private:
    Q_DECLARE_PRIVATE(QHttpRequestHeader)
};

class Q_NETWORK_EXPORT QHttp : public QObject
{
    Q_OBJECT

public:
    enum ConnectionMode {
        ConnectionModeHttp,
        ConnectionModeHttps
    };
    
    explicit QHttp(QObject *parent = 0);
    QHttp(const QString &hostname, quint16 port = 80, QObject *parent = 0);
    QHttp(const QString &hostname, ConnectionMode mode, quint16 port = 0, QObject *parent = 0);
    virtual ~QHttp();

    enum State {
        Unconnected,
        HostLookup,
        Connecting,
        Sending,
        Reading,
        Connected,
        Closing
    };
    enum Error {
        NoError,
        UnknownError,
        HostNotFound,
        ConnectionRefused,
        UnexpectedClose,
        InvalidResponseHeader,
        WrongContentLength,
        Aborted, 
        AuthenticationRequiredError,
        ProxyAuthenticationRequiredError
    };

    int setHost(const QString &hostname, quint16 port = 80);
    int setHost(const QString &hostname, ConnectionMode mode, quint16 port = 0);
    
    int setSocket(QTcpSocket *socket);
    int setUser(const QString &username, const QString &password = QString());

#ifndef QT_NO_NETWORKPROXY
    int setProxy(const QString &host, int port,
                 const QString &username = QString(),
                 const QString &password = QString());
    int setProxy(const QNetworkProxy &proxy);
#endif

    int get(const QString &path, QIODevice *to=0);
    int post(const QString &path, QIODevice *data, QIODevice *to=0 );
    int post(const QString &path, const QByteArray &data, QIODevice *to=0);
    int head(const QString &path);
    int request(const QHttpRequestHeader &header, QIODevice *device=0, QIODevice *to=0);
    int request(const QHttpRequestHeader &header, const QByteArray &data, QIODevice *to=0);

    int closeConnection();
    int close();

    qint64 bytesAvailable() const;
    qint64 read(char *data, qint64 maxlen);
#ifdef QT3_SUPPORT
    inline QT3_SUPPORT qint64 readBlock(char *data, quint64 maxlen)
    { return read(data, qint64(maxlen)); }
#endif
    QByteArray readAll();

    int currentId() const;
    QIODevice *currentSourceDevice() const;
    QIODevice *currentDestinationDevice() const;
    QHttpRequestHeader currentRequest() const;
    QHttpResponseHeader lastResponse() const;
    bool hasPendingRequests() const;
    void clearPendingRequests();

    State state() const;

    Error error() const;
    QString errorString() const;

public Q_SLOTS:
    void abort();

#ifndef QT_NO_OPENSSL
    void ignoreSslErrors();
#endif

Q_SIGNALS:
    void stateChanged(int);
    void responseHeaderReceived(const QHttpResponseHeader &resp);
    void readyRead(const QHttpResponseHeader &resp);

    // ### Qt 5: change to qint64
    void dataSendProgress(int, int);
    void dataReadProgress(int, int);

    void requestStarted(int);
    void requestFinished(int, bool);
    void done(bool);

#ifndef QT_NO_NETWORKPROXY
    void proxyAuthenticationRequired(const QNetworkProxy &proxy, QAuthenticator *);
#endif
    void authenticationRequired(const QString &hostname, quint16 port, QAuthenticator *);

#ifndef QT_NO_OPENSSL
    void sslErrors(const QList<QSslError> &errors);
#endif

private:
    Q_DISABLE_COPY(QHttp)
    Q_DECLARE_PRIVATE(QHttp)

    Q_PRIVATE_SLOT(d_func(), void _q_startNextRequest())
    Q_PRIVATE_SLOT(d_func(), void _q_slotReadyRead())
    Q_PRIVATE_SLOT(d_func(), void _q_slotConnected())
    Q_PRIVATE_SLOT(d_func(), void _q_slotError(QAbstractSocket::SocketError))
    Q_PRIVATE_SLOT(d_func(), void _q_slotClosed())
    Q_PRIVATE_SLOT(d_func(), void _q_slotBytesWritten(qint64 numBytes))
    Q_PRIVATE_SLOT(d_func(), void _q_slotDoFinished())
    Q_PRIVATE_SLOT(d_func(), void _q_slotSendRequest())

    friend class QHttpNormalRequest;
    friend class QHttpSetHostRequest;
    friend class QHttpSetSocketRequest;
    friend class QHttpSetUserRequest;
    friend class QHttpSetProxyRequest;
    friend class QHttpCloseRequest;
    friend class QHttpPGHRequest;
};

#endif // QT_NO_HTTP

QT_END_NAMESPACE

QT_END_HEADER

#endif // QHTTP_H
