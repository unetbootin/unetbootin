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

#ifndef QNETWORKACCESSMANAGER_P_H
#define QNETWORKACCESSMANAGER_P_H

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

#include "qnetworkaccessmanager.h"
#include "qnetworkaccesscache_p.h"
#include "qnetworkaccessbackend_p.h"
#include "private/qobject_p.h"
#include "QtNetwork/qnetworkproxy.h"

QT_BEGIN_NAMESPACE

class QAuthenticator;
class QAbstractNetworkCache;
class QNetworkAuthenticationCredential;
class QNetworkCookieJar;

class QNetworkAccessManagerPrivate: public QObjectPrivate
{
public:
    QNetworkAccessManagerPrivate()
        : networkCache(0), cookieJar(0), cookieJarCreated(false)
    { }

    void _q_replyFinished();
    void _q_replySslErrors(const QList<QSslError> &errors);
    QNetworkReply *postProcess(QNetworkReply *reply);
    void createCookieJar() const;

    void authenticationRequired(QNetworkAccessBackend *backend, QAuthenticator *authenticator);
    void addCredentials(const QUrl &url, const QAuthenticator *auth);
    QNetworkAuthenticationCredential *fetchCachedCredentials(const QUrl &url,
                                                             const QAuthenticator *auth = 0);

#ifndef QT_NO_NETWORKPROXY
    void proxyAuthenticationRequired(QNetworkAccessBackend *backend, const QNetworkProxy &proxy,
                                     QAuthenticator *authenticator);
    void addCredentials(const QNetworkProxy &proxy, const QAuthenticator *auth);
    QNetworkAuthenticationCredential *fetchCachedCredentials(const QNetworkProxy &proxy,
                                                             const QAuthenticator *auth = 0);
#endif

    QNetworkAccessBackend *findBackend(QNetworkAccessManager::Operation op, const QNetworkRequest &request);

    QAbstractNetworkCache *networkCache;
    QNetworkCookieJar *cookieJar;

    QNetworkAccessCache cache;
#ifndef QT_NO_NETWORKPROXY
    QNetworkProxy proxy;
#endif

    bool cookieJarCreated;

    static inline QNetworkAccessCache *getCache(QNetworkAccessBackend *backend)
    { return &backend->manager->cache; }
    Q_AUTOTEST_EXPORT static void clearCache(QNetworkAccessManager *manager);

    Q_DECLARE_PUBLIC(QNetworkAccessManager)
};

QT_END_NAMESPACE

#endif
