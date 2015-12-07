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

#ifndef QNETWORKREPLY_H
#define QNETWORKREPLY_H

#include <QtCore/QIODevice>
#include <QtCore/QString>
#include <QtCore/QVariant>

#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkAccessManager>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Network)

class QUrl;
class QVariant;
class QAuthenticator;
class QSslConfiguration;
class QSslError;

class QNetworkReplyPrivate;
class Q_NETWORK_EXPORT QNetworkReply: public QIODevice
{
    Q_OBJECT
    Q_ENUMS(NetworkError)
public:
    enum NetworkError {
        NoError = 0,

        // network layer errors [relating to the destination server] (1-99):
        ConnectionRefusedError = 1,
        RemoteHostClosedError,
        HostNotFoundError,
        TimeoutError,
        OperationCanceledError,
        SslHandshakeFailedError,
        UnknownNetworkError = 99,

        // proxy errors (101-199):
        ProxyConnectionRefusedError = 101,
        ProxyConnectionClosedError,
        ProxyNotFoundError,
        ProxyTimeoutError,
        ProxyAuthenticationRequiredError,
        UnknownProxyError = 199,

        // content errors (201-299):
        ContentAccessDenied = 201,
        ContentOperationNotPermittedError,
        ContentNotFoundError,
        AuthenticationRequiredError,
        UnknownContentError = 299,

        // protocol errors
        ProtocolUnknownError = 301,
        ProtocolInvalidOperationError,
        ProtocolFailure = 399
    };

    ~QNetworkReply();
    virtual void abort() = 0;

    // reimplemented from QIODevice
    virtual void close();
    virtual bool isSequential() const;

    // like QAbstractSocket:
    qint64 readBufferSize() const;
    virtual void setReadBufferSize(qint64 size);

    QNetworkAccessManager *manager() const;
    QNetworkAccessManager::Operation operation() const;
    QNetworkRequest request() const;
    NetworkError error() const;
    QUrl url() const;

    // "cooked" headers
    QVariant header(QNetworkRequest::KnownHeaders header) const;

    // raw headers:
    bool hasRawHeader(const QByteArray &headerName) const;
    QList<QByteArray> rawHeaderList() const;
    QByteArray rawHeader(const QByteArray &headerName) const;

    // attributes
    QVariant attribute(QNetworkRequest::Attribute code) const;

#ifndef QT_NO_OPENSSL
    QSslConfiguration sslConfiguration() const;
    void setSslConfiguration(const QSslConfiguration &configuration);
#endif

public Q_SLOTS:
    virtual void ignoreSslErrors();

Q_SIGNALS:
    void metaDataChanged();
    void finished();
    void error(QNetworkReply::NetworkError);
#ifndef QT_NO_OPENSSL
    void sslErrors(const QList<QSslError> &errors);
#endif

    void uploadProgress(qint64 bytesSent, qint64 bytesTotal);
    void downloadProgress(qint64 bytesReceived, qint64 bytesTotal);

protected:
    QNetworkReply(QObject *parent = 0);
    QNetworkReply(QNetworkReplyPrivate &dd, QObject *parent);
    virtual qint64 writeData(const char *data, qint64 len);

    void setOperation(QNetworkAccessManager::Operation operation);
    void setRequest(const QNetworkRequest &request);
    void setError(NetworkError errorCode, const QString &errorString);
    void setUrl(const QUrl &url);
    void setHeader(QNetworkRequest::KnownHeaders header, const QVariant &value);
    void setRawHeader(const QByteArray &headerName, const QByteArray &value);
    void setAttribute(QNetworkRequest::Attribute code, const QVariant &value);

private:
    Q_DECLARE_PRIVATE(QNetworkReply)
};

QT_END_NAMESPACE

QT_END_HEADER

#endif
