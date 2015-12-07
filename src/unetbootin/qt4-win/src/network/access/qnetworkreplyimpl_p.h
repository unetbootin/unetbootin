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

#ifndef QNETWORKREPLYIMPL_P_H
#define QNETWORKREPLYIMPL_P_H

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

#include "qnetworkreply.h"
#include "qnetworkreply_p.h"
#include "qnetworkaccessmanager.h"
#include "qnetworkaccessbackend_p.h"
#include "qnetworkproxy.h"
#include "QtCore/qmap.h"
#include "QtCore/qqueue.h"
#include "private/qringbuffer_p.h"

QT_BEGIN_NAMESPACE

class QAbstractNetworkCache;
class QNetworkAccessBackend;

class QNetworkReplyImplPrivate;
class QNetworkReplyImpl: public QNetworkReply
{
    Q_OBJECT
public:
    QNetworkReplyImpl(QObject *parent = 0);
    ~QNetworkReplyImpl();
    virtual void abort();

    // reimplemented from QNetworkReply
    virtual void close();
    virtual qint64 bytesAvailable() const;
    virtual void setReadBufferSize(qint64 size);

    virtual qint64 readData(char *data, qint64 maxlen);
    virtual bool event(QEvent *);

#ifndef QT_NO_OPENSSL
    Q_INVOKABLE QSslConfiguration sslConfigurationImplementation() const;
    Q_INVOKABLE void setSslConfigurationImplementation(const QSslConfiguration &configuration);
    virtual void ignoreSslErrors();
#endif

    Q_DECLARE_PRIVATE(QNetworkReplyImpl)
    Q_PRIVATE_SLOT(d_func(), void _q_startOperation())
    Q_PRIVATE_SLOT(d_func(), void _q_sourceReadyRead())
    Q_PRIVATE_SLOT(d_func(), void _q_sourceReadChannelFinished())
    Q_PRIVATE_SLOT(d_func(), void _q_copyReadyRead())
    Q_PRIVATE_SLOT(d_func(), void _q_copyReadChannelFinished())
};

class QNetworkReplyImplPrivate: public QNetworkReplyPrivate
{
public:
    enum InternalNotifications {
        NotifyDownstreamReadyWrite,
        NotifyUpstreamReadyRead,
        NotifyCloseDownstreamChannel,
        NotifyCloseUpstreamChannel,
        NotifyCopyFinished
    };

    enum State {
        Idle,
        Opening,
        Working,
        Finished,
        Aborted
    };

    typedef QQueue<InternalNotifications> NotificationQueue;

    QNetworkReplyImplPrivate();

    void _q_startOperation();
    void _q_sourceReadyRead();
    void _q_sourceReadChannelFinished();
    void _q_copyReadyRead();
    void _q_copyReadChannelFinished();

    void setup(QNetworkAccessManager::Operation op, const QNetworkRequest &request,
               QIODevice *outgoingData);
    void setNetworkCache(QAbstractNetworkCache *networkCache);
    void backendNotify(InternalNotifications notification);
    void handleNotifications();

    // callbacks from the backend (through the manager):
    void setCachingEnabled(bool enable);
    void consume(qint64 count);
    qint64 nextDownstreamBlockSize() const;
    void feed(const QByteArray &data);
    void feed(QIODevice *data);
    void finished();
    void error(QNetworkReply::NetworkError code, const QString &errorString);
    void metaDataChanged();
    void redirectionRequested(const QUrl &target);
    void sslErrors(const QList<QSslError> &errors);

    QNetworkAccessBackend *backend;
    QIODevice *outgoingData;
    QIODevice *copyDevice;
    QAbstractNetworkCache *networkCache;

    NotificationQueue pendingNotifications;
    QUrl urlForLastAuthentication;
#ifndef QT_NO_NETWORKPROXY
    QNetworkProxy networkProxy;
    QNetworkProxy lastProxyAuthentication;
#endif

    QRingBuffer readBuffer;
    QRingBuffer writeBuffer;
    qint64 bytesDownloaded;
    qint64 lastBytesDownloaded;
    qint64 bytesUploaded;

    QString httpReasonPhrase;
    int httpStatusCode;

    State state;
    bool isEncrypted;

    Q_DECLARE_PUBLIC(QNetworkReplyImpl)
};

QT_END_NAMESPACE

#endif
