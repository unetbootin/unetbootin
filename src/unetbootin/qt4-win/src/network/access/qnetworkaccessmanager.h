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

#ifndef QNETWORKACCESSMANAGER_H
#define QNETWORKACCESSMANAGER_H

#include <QtCore/QObject>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Network)

class QIODevice;
class QAuthenticator;
class QByteArray;
template<typename T> class QList;
class QNetworkCookie;
class QNetworkCookieJar;
class QNetworkRequest;
class QNetworkReply;
class QNetworkProxy;
class QSslError;

class QNetworkReplyImplPrivate;
class QNetworkAccessManagerPrivate;
class Q_NETWORK_EXPORT QNetworkAccessManager: public QObject
{
    Q_OBJECT
public:
    enum Operation {
        HeadOperation = 1,
        GetOperation,
        PutOperation,
        PostOperation,

        UnknownOperation = 0
    };

    explicit QNetworkAccessManager(QObject *parent = 0);
    ~QNetworkAccessManager();

#ifndef QT_NO_NETWORKPROXY
    QNetworkProxy proxy() const;
    void setProxy(const QNetworkProxy &proxy);
#endif

    QNetworkCookieJar *cookieJar() const;
    void setCookieJar(QNetworkCookieJar *cookieJar);

    QNetworkReply *head(const QNetworkRequest &request);
    QNetworkReply *get(const QNetworkRequest &request);
    QNetworkReply *post(const QNetworkRequest &request, QIODevice *data);
    QNetworkReply *post(const QNetworkRequest &request, const QByteArray &data);
    QNetworkReply *put(const QNetworkRequest &request, QIODevice *data);
    QNetworkReply *put(const QNetworkRequest &request, const QByteArray &data);

Q_SIGNALS:
#ifndef QT_NO_NETWORKPROXY
    void proxyAuthenticationRequired(const QNetworkProxy &proxy, QAuthenticator *authenticator);
#endif
    void authenticationRequired(QNetworkReply *reply, QAuthenticator *authenticator);
    void finished(QNetworkReply *reply);
#ifndef QT_NO_OPENSSL
    void sslErrors(QNetworkReply *reply, const QList<QSslError> &errors);
#endif

protected:
    virtual QNetworkReply *createRequest(Operation op, const QNetworkRequest &request,
                                         QIODevice *outgoingData = 0);

private:
    friend class QNetworkReplyImplPrivate;
    Q_DECLARE_PRIVATE(QNetworkAccessManager)
    Q_PRIVATE_SLOT(d_func(), void _q_replyFinished())
    Q_PRIVATE_SLOT(d_func(), void _q_replySslErrors(QList<QSslError>))
};

QT_END_NAMESPACE

QT_END_HEADER

#endif
