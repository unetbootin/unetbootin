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

#ifndef QNETWORKACCESSBACKEND_P_H
#define QNETWORKACCESSBACKEND_P_H

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

#include "qnetworkreplyimpl_p.h"
#include "QtCore/qobject.h"

QT_BEGIN_NAMESPACE

class QAuthenticator;
class QNetworkProxy;
class QNetworkRequest;
class QUrl;
class QUrlInfo;
class QSslConfiguration;

class QNetworkAccessManagerPrivate;
class QNetworkReplyImplPrivate;
class QAbstractNetworkCache;

// Should support direct file upload from disk or download to disk.
//
// - The HTTP handler will use two QIODevices for communication (pull mechanism)
// - KIO uses a pull mechanism too (data/dataReq signals)
class QNetworkAccessBackend : public QObject
{
    Q_OBJECT
public:
    QNetworkAccessBackend();
    virtual ~QNetworkAccessBackend();

    // To avoid mistaking with QIODevice names, the functions here
    // have different names. The Connection has two streams:
    //
    // - Upstream:
    //   Upstream is data that is being written into this connection,
    //   from the user. Upstream operates in a "pull" mechanism: the
    //   connection will be notified that there is more data available
    //   by a call to "upstreamReadyRead". The number of bytes
    //   available is given by upstreamBytesAvailable(). A call to
    //   readUpstream() always yields the entire upstream buffer. When
    //   the connection has processed a certain amount of bytes from
    //   that buffer, it should call upstreamBytesConsumed().
    //
    // - Downstream:
    //   Downstream is the data that is being read from this
    //   connection and is given to the user. Downstream operates in a
    //   semi-"push" mechanism: the Connection object pushes the data
    //   it gets from the network, but it should avoid writing too
    //   much if the data isn't being used fast enough. The value
    //   returned by suggestedDownstreamBlockSize() can be used to
    //   determine how much should be written at a time. The
    //   downstreamBytesConsumed() function will be called when the
    //   downstream buffer is consumed by the user -- the Connection
    //   may choose to re-fill it with more data it has ready or get
    //   more data from the network (for instance, by reading from its
    //   socket).

    virtual void open() = 0;
    virtual void closeDownstreamChannel() = 0;
    virtual void closeUpstreamChannel() = 0;
    virtual bool waitForDownstreamReadyRead(int msecs) = 0;
    virtual bool waitForUpstreamBytesWritten(int msecs) = 0;

    // slot-like:
    virtual void upstreamReadyRead();
    virtual void downstreamReadyWrite();
    virtual void copyFinished(QIODevice *);
    virtual void ignoreSslErrors();

    virtual void fetchSslConfiguration(QSslConfiguration &configuration) const;
    virtual void setSslConfiguration(const QSslConfiguration &configuration);

    // information about the request
    QNetworkAccessManager::Operation operation() const;
    QNetworkRequest request() const;
#ifndef QT_NO_NETWORKPROXY
    QNetworkProxy proxy() const;
#endif

    QAbstractNetworkCache *networkCache() const;
    void setCachingEnabled(bool enable);

    // information about the reply
    QUrl url() const;
    void setUrl(const QUrl &url);

    // "cooked" headers
    QVariant header(QNetworkRequest::KnownHeaders header) const;
    void setHeader(QNetworkRequest::KnownHeaders header, const QVariant &value);

    // raw headers:
    bool hasRawHeader(const QByteArray &headerName) const;
    QList<QByteArray> rawHeaderList() const;
    QByteArray rawHeader(const QByteArray &headerName) const;
    void setRawHeader(const QByteArray &headerName, const QByteArray &value);

    // attributes:
    QVariant attribute(QNetworkRequest::Attribute code) const;
    void setAttribute(QNetworkRequest::Attribute code, const QVariant &value);

protected:
    // these functions control the upstream mechanism
    // that is, data coming into the backend and out via the connection
    qint64 upstreamBytesAvailable() const;
    void upstreamBytesConsumed(qint64 count);
    QByteArray readUpstream();

    // these functions control the downstream mechanism
    // that is, data that has come via the connection and is going out the backend
    qint64 nextDownstreamBlockSize() const;
    qint64 downstreamBytesToConsume() const;
    void writeDownstreamData(const QByteArray &data);
    void writeDownstreamData(QIODevice *data);

protected slots:
    void finished();
    void error(QNetworkReply::NetworkError code, const QString &errorString);
#ifndef QT_NO_NETWORKPROXY
    void proxyAuthenticationRequired(const QNetworkProxy &proxy, QAuthenticator *auth);
#endif
    void authenticationRequired(QAuthenticator *auth);
    void metaDataChanged();
    void redirectionRequested(const QUrl &destination);
    void sslErrors(const QList<QSslError> &errors);

private:
    friend class QNetworkAccessManager;
    friend class QNetworkAccessManagerPrivate;
    QNetworkAccessManagerPrivate *manager;
    QNetworkReplyImplPrivate *reply;
};

class QNetworkAccessBackendFactory
{
public:
    QNetworkAccessBackendFactory();
    virtual ~QNetworkAccessBackendFactory();
    virtual QNetworkAccessBackend *create(QNetworkAccessManager::Operation op,
                                          const QNetworkRequest &request) const = 0;
};

QT_END_NAMESPACE

#endif

