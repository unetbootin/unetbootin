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

    qint64 deviceReadData(char *buffer, qint64 maxlen);

private slots:
    void replyReadyRead();
    void replyFinished();
    void replyHeaderChanged();
    void httpAuthenticationRequired(const QHttpNetworkRequest &request, QAuthenticator *auth);
    void httpError(QNetworkReply::NetworkError error, const QString &errorString);

private:
    QHttpNetworkReply *httpReply;
    QPointer<QNetworkAccessHttpBackendCache> http;
    QNetworkAccessHttpBackendIODevice *uploadDevice;
#ifndef QT_NO_OPENSSL
    QSslConfiguration *pendingSslConfiguration;
    bool pendingIgnoreSslErrors;
#endif

    void finished();            // override
    void disconnectFromHttp();
    void setupConnection();
    void validateCache(QHttpNetworkRequest &httpRequest);
    void postRequest();
    void readFromHttp();

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
