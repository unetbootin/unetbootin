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

#ifndef QNETWORKPROXY_H
#define QNETWORKPROXY_H

#include <QtNetwork/qhostaddress.h>

#ifndef QT_NO_NETWORKPROXY

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Network)

class QNetworkProxyPrivate;

class Q_NETWORK_EXPORT QNetworkProxy
{
    Q_DECLARE_PRIVATE(QNetworkProxy)

public:
    enum ProxyType {
        DefaultProxy,
        Socks5Proxy,
        NoProxy,
        HttpProxy,
        HttpCachingProxy,
        FtpCachingProxy
    };

    QNetworkProxy();
    QNetworkProxy(ProxyType type, const QString &hostName = QString(), quint16 port = 0,
                  const QString &user = QString(), const QString &password = QString());
    QNetworkProxy(const QNetworkProxy &other);
    QNetworkProxy &operator=(const QNetworkProxy &other);
    ~QNetworkProxy();
    bool operator==(const QNetworkProxy &other) const;
    inline bool operator!=(const QNetworkProxy &other) const
    { return !(*this == other); }

    void setType(QNetworkProxy::ProxyType type);
    QNetworkProxy::ProxyType type() const;
    bool isCachingProxy() const;
    bool isTransparentProxy() const;

    void setUser(const QString &userName);
    QString user() const;

    void setPassword(const QString &password);
    QString password() const;

    void setHostName(const QString &hostName);
    QString hostName() const;

    void setPort(quint16 port);
    quint16 port() const;

    static void setApplicationProxy(const QNetworkProxy &proxy);
    static QNetworkProxy applicationProxy();

private:
    QNetworkProxyPrivate *d_ptr;
};

QT_END_NAMESPACE

QT_END_HEADER

#endif // QT_NO_NETWORKPROXY

#endif // QHOSTINFO_H
