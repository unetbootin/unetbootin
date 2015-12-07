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

#ifndef QNETWORKACCESSMANAGER_H
#define QNETWORKACCESSMANAGER_H

#include <QtCore/QObject>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Network)

class QIODevice;
class QAbstractNetworkCache;
class QAuthenticator;
class QByteArray;
template<typename T> class QList;
class QNetworkCookie;
class QNetworkCookieJar;
class QNetworkRequest;
class QNetworkReply;
class QNetworkProxy;
class QNetworkProxyFactory;
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
    QNetworkProxyFactory *proxyFactory() const;
    void setProxyFactory(QNetworkProxyFactory *factory);
#endif

    QAbstractNetworkCache *cache() const;
    void setCache(QAbstractNetworkCache *cache);

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
