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

#ifndef QNETWORKACCESSHTTPBACKEND_P_H
#define QNETWORKACCESSHTTPBACKEND_P_H

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

#include "qhttpnetworkconnection_p.h"
#include "qnetworkaccessbackend_p.h"
#include "qnetworkrequest.h"
#include "qnetworkreply.h"
#include "qabstractsocket.h"

#include "QtCore/qpointer.h"
#include "QtCore/qdatetime.h"

#ifndef QT_NO_HTTP

QT_BEGIN_NAMESPACE

class QNetworkAccessHttpBackendCache;

class QNetworkAccessHttpBackendIODevice;

class QNetworkAccessHttpBackend: public QNetworkAccessBackend
{
    Q_OBJECT
public:
    QNetworkAccessHttpBackend();
    virtual ~QNetworkAccessHttpBackend();

    virtual void open();
    virtual void closeDownstreamChannel();
    virtual void closeUpstreamChannel();
    virtual bool waitForDownstreamReadyRead(int msecs);
    virtual bool waitForUpstreamBytesWritten(int msecs);

    virtual void upstreamReadyRead();
    virtual void downstreamReadyWrite();
    virtual void copyFinished(QIODevice *);
#ifndef QT_NO_OPENSSL
    virtual void ignoreSslErrors();

    virtual void fetchSslConfiguration(QSslConfiguration &configuration) const;
    virtual void setSslConfiguration(const QSslConfiguration &configuration);
#endif
    QNetworkCacheMetaData fetchCacheMetaData(const QNetworkCacheMetaData &metaData) const;

    qint64 deviceReadData(char *buffer, qint64 maxlen);

private slots:
    void replyReadyRead();
    void replyFinished();
    void replyHeaderChanged();
    void httpAuthenticationRequired(const QHttpNetworkRequest &request, QAuthenticator *auth);
    void httpError(QNetworkReply::NetworkError error, const QString &errorString);
    bool sendCacheContents(const QNetworkCacheMetaData &metaData);

private:
    QHttpNetworkReply *httpReply;
    QPointer<QNetworkAccessHttpBackendCache> http;
    QByteArray cacheKey;
    QNetworkAccessHttpBackendIODevice *uploadDevice;
#ifndef QT_NO_OPENSSL
    QSslConfiguration *pendingSslConfiguration;
    bool pendingIgnoreSslErrors;
#endif

    void disconnectFromHttp();
    void finished();            // override
    void setupConnection();
    void validateCache(QHttpNetworkRequest &httpRequest, bool &loadedFromCache);
    void invalidateCache();
    void postRequest();
    void readFromHttp();
    void checkForRedirect(const int statusCode);

    friend class QNetworkAccessHttpBackendIODevice;
};

class QNetworkAccessHttpBackendFactory : public QNetworkAccessBackendFactory
{
public:
    virtual QNetworkAccessBackend *create(QNetworkAccessManager::Operation op,
                                          const QNetworkRequest &request) const;
};

QT_END_NAMESPACE

#endif // QT_NO_HTTP

#endif
