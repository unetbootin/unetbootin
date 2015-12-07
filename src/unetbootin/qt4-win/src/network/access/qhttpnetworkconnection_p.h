/****************************************************************************
**
** Copyright (C) 2007-2008 Trolltech ASA. All rights reserved.
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

#ifndef QHTTPNETWORKCONNECTION_H
#define QHTTPNETWORKCONNECTION_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists for the convenience
// of the Network Access API.  This header file may change from
// version to version without notice, or even be removed.
//
// We mean it.
//
#include <QtNetwork/qnetworkrequest.h>
#include <QtNetwork/qnetworkreply.h>
#include <QtNetwork/qabstractsocket.h>

#ifndef QT_NO_HTTP

#ifndef QT_NO_OPENSSL
#    include <QtNetwork/qsslsocket.h>
#    include <QtNetwork/qsslerror.h>
#else
#   include <QtNetwork/qtcpsocket.h>
#endif

QT_BEGIN_NAMESPACE

class QHttpNetworkRequest;
class QHttpNetworkReply;

class QHttpNetworkConnectionPrivate;
class Q_AUTOTEST_EXPORT QHttpNetworkConnection : public QObject
{
    Q_OBJECT
public:

    QHttpNetworkConnection(const QString &hostName, quint16 port = 80, bool encrypt = false, QObject *parent = 0);
    ~QHttpNetworkConnection();

    //The hostname to which this is connected to.
    QString hostName() const;
    //The HTTP port in use.
    quint16 port() const;

    //add a new HTTP request through this connection
    QHttpNetworkReply* sendRequest(const QHttpNetworkRequest &request);

#ifndef QT_NO_NETWORKPROXY
    //set the proxy for this connection
    void setCacheProxy(const QNetworkProxy &networkProxy);
    QNetworkProxy cacheProxy() const;
    void setTransparentProxy(const QNetworkProxy &networkProxy);
    QNetworkProxy transparentProxy() const;
#endif

    //enable encryption
    void enableEncryption();
    bool isEncrypted() const;

    //authentication parameters
    void setProxyAuthentication(QAuthenticator *authenticator);
    void setAuthentication(const QString &domain, QAuthenticator *authenticator);

#ifndef QT_NO_OPENSSL
    void setSslConfiguration(const QSslConfiguration &config);
    void ignoreSslErrors(int channel = -1);

Q_SIGNALS:
    void sslErrors(const QList<QSslError> &errors);
#endif

Q_SIGNALS:
#ifndef QT_NO_NETWORKPROXY
    //cannot be used with queued connection.
    void proxyAuthenticationRequired(const QNetworkProxy &proxy, QAuthenticator *authenticator,
                                     const QHttpNetworkConnection *connection = 0);
#endif
    void authenticationRequired(const QHttpNetworkRequest &request, QAuthenticator *authenticator,
                                const QHttpNetworkConnection *connection = 0);
    void error(QNetworkReply::NetworkError errorCode, const QString &detail = QString());

private:
    Q_DECLARE_PRIVATE(QHttpNetworkConnection)
    Q_DISABLE_COPY(QHttpNetworkConnection)
    friend class QHttpNetworkReply;

    Q_PRIVATE_SLOT(d_func(), void _q_bytesWritten(qint64))
    Q_PRIVATE_SLOT(d_func(), void _q_readyRead())
    Q_PRIVATE_SLOT(d_func(), void _q_disconnected())
    Q_PRIVATE_SLOT(d_func(), void _q_startNextRequest())
    Q_PRIVATE_SLOT(d_func(), void _q_restartPendingRequest())
    Q_PRIVATE_SLOT(d_func(), void _q_connected())
    Q_PRIVATE_SLOT(d_func(), void _q_error(QAbstractSocket::SocketError))
#ifndef QT_NO_NETWORKPROXY
    Q_PRIVATE_SLOT(d_func(), void _q_proxyAuthenticationRequired(const QNetworkProxy&, QAuthenticator*))
#endif
    Q_PRIVATE_SLOT(d_func(), void _q_dataReadyReadBuffer())
    Q_PRIVATE_SLOT(d_func(), void _q_dataReadyReadNoBuffer())

#ifndef QT_NO_OPENSSL
    Q_PRIVATE_SLOT(d_func(), void _q_encrypted())
    Q_PRIVATE_SLOT(d_func(), void _q_sslErrors(const QList<QSslError>&))
#endif
};

class Q_AUTOTEST_EXPORT QHttpNetworkHeader
{
public:
    virtual ~QHttpNetworkHeader() {};
    virtual QUrl url() const = 0;
    virtual void setUrl(const QUrl &url) = 0;

    virtual int majorVersion() const = 0;
    virtual int minorVersion() const = 0;

    virtual qint64 contentLength() const = 0;
    virtual void setContentLength(qint64 length) = 0;

    virtual QList<QPair<QByteArray, QByteArray> > header() const = 0;
    virtual QByteArray headerField(const QByteArray &name, const QByteArray &defaultValue = QByteArray()) const = 0;
    virtual void setHeaderField(const QByteArray &name, const QByteArray &data) = 0;
};

class QHttpNetworkRequestPrivate;
class Q_AUTOTEST_EXPORT QHttpNetworkRequest: public QHttpNetworkHeader
{
public:
    enum Operation {
        Options,
        Get,
        Head,
        Post,
        Put,
        Delete,
        Trace,
        Connect
    };

    enum Priority {
        HighPriority,
        NormalPriority,
        LowPriority
    };

    QHttpNetworkRequest(const QUrl &url = QUrl(), Operation operation = Get, Priority priority = NormalPriority);
    QHttpNetworkRequest(const QHttpNetworkRequest &other);
    virtual ~QHttpNetworkRequest();
    QHttpNetworkRequest &operator=(const QHttpNetworkRequest &other);
    bool operator==(const QHttpNetworkRequest &other) const;

    QUrl url() const;
    void setUrl(const QUrl &url);

    int majorVersion() const;
    int minorVersion() const;

    qint64 contentLength() const;
    void setContentLength(qint64 length);

    QList<QPair<QByteArray, QByteArray> > header() const;
    QByteArray headerField(const QByteArray &name, const QByteArray &defaultValue = QByteArray()) const;
    void setHeaderField(const QByteArray &name, const QByteArray &data);

    Operation operation() const;
    void setOperation(Operation operation);

    Priority priority() const;
    void setPriority(Priority priority);

    QIODevice *data() const;
    void setData(QIODevice *data);

private:
    QSharedDataPointer<QHttpNetworkRequestPrivate> d;
    friend class QHttpNetworkRequestPrivate;
    friend class QHttpNetworkConnectionPrivate;
};

class QHttpNetworkReplyPrivate;
class Q_AUTOTEST_EXPORT QHttpNetworkReply : public QObject, public QHttpNetworkHeader
{
    Q_OBJECT
public:

    explicit QHttpNetworkReply(const QUrl &url = QUrl(), QObject *parent = 0);
    virtual ~QHttpNetworkReply();

    QUrl url() const;
    void setUrl(const QUrl &url);

    int majorVersion() const;
    int minorVersion() const;

    qint64 contentLength() const;
    void setContentLength(qint64 length);

    QList<QPair<QByteArray, QByteArray> > header() const;
    QByteArray headerField(const QByteArray &name, const QByteArray &defaultValue = QByteArray()) const;
    void setHeaderField(const QByteArray &name, const QByteArray &data);
    void parseHeader(const QByteArray &header); // mainly for testing

    QHttpNetworkRequest request() const;
    void setRequest(const QHttpNetworkRequest &request);

    int statusCode() const;
    void setStatusCode(int code);

    QString errorString() const;
    void setErrorString(const QString &error);

    QString reasonPhrase() const;

    qint64 bytesAvailable() const;
    QByteArray read(qint64 maxSize = -1);

    bool isFinished() const;

#ifndef QT_NO_OPENSSL
    QSslConfiguration sslConfiguration() const;
    void setSslConfiguration(const QSslConfiguration &config);
    void ignoreSslErrors();

Q_SIGNALS:
    void sslErrors(const QList<QSslError> &errors);
#endif

Q_SIGNALS:
    void readyRead();
    void finished();
    void finishedWithError(QNetworkReply::NetworkError errorCode, const QString &detail = QString());
    void headerChanged();
    void dataReadProgress(int done, int total);
    void dataSendProgress(int done, int total);

private:
    Q_DECLARE_PRIVATE(QHttpNetworkReply)
    friend class QHttpNetworkConnection;
    friend class QHttpNetworkConnectionPrivate;
};

QT_END_NAMESPACE

Q_DECLARE_METATYPE(QHttpNetworkRequest)
//Q_DECLARE_METATYPE(QHttpNetworkReply)

#endif // QT_NO_HTTP

#endif
