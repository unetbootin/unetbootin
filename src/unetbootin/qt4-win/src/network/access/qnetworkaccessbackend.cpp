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

#include "qnetworkaccessbackend_p.h"
#include "qnetworkaccessmanager_p.h"
#include "qnetworkrequest.h"
#include "qnetworkreply.h"
#include "qnetworkreply_p.h"
#include "QtCore/qhash.h"
#include "QtCore/qmutex.h"

QT_BEGIN_NAMESPACE

static bool factoryDataShutdown = false;
class QNetworkAccessBackendFactoryData: public QList<QNetworkAccessBackendFactory *>
{
public:
    QNetworkAccessBackendFactoryData() : mutex(QMutex::Recursive) { }
    ~QNetworkAccessBackendFactoryData()
    {
        QMutexLocker locker(&mutex); // why do we need to lock?
        factoryDataShutdown = true;
    }

    QMutex mutex;
};
Q_GLOBAL_STATIC(QNetworkAccessBackendFactoryData, factoryData)

QNetworkAccessBackendFactory::QNetworkAccessBackendFactory()
{
    QMutexLocker locker(&factoryData()->mutex);
    factoryData()->prepend(this);
}

QNetworkAccessBackendFactory::~QNetworkAccessBackendFactory()
{
    if (!factoryDataShutdown) {
        QMutexLocker locker(&factoryData()->mutex);
        factoryData()->removeAll(this);
    }
}

QNetworkAccessBackend *QNetworkAccessManagerPrivate::findBackend(QNetworkAccessManager::Operation op,
                                                                 const QNetworkRequest &request)
{
    QNetworkRequest::CacheLoadControl mode =
        static_cast<QNetworkRequest::CacheLoadControl>(
            request.attribute(QNetworkRequest::CacheLoadControlAttribute,
                              QNetworkRequest::PreferNetwork).toInt());
    if (mode == QNetworkRequest::AlwaysCache)
        return 0;               // not found in cache

    if (!factoryDataShutdown) {
        QMutexLocker locker(&factoryData()->mutex);
        QNetworkAccessBackendFactoryData::ConstIterator it = factoryData()->constBegin(),
                                                           end = factoryData()->constEnd();
        while (it != end) {
            QNetworkAccessBackend *backend = (*it)->create(op, request);
            if (backend) {
                backend->manager = this;
                return backend; // found a factory that handled our request
            }
            ++it;
        }
    }
    return 0;
}

QNetworkAccessBackend::QNetworkAccessBackend()
{
}

QNetworkAccessBackend::~QNetworkAccessBackend()
{
}

void QNetworkAccessBackend::upstreamReadyRead()
{
    // do nothing
}

void QNetworkAccessBackend::downstreamReadyWrite()
{
    // do nothing
}

void QNetworkAccessBackend::copyFinished(QIODevice *)
{
    // do nothing
}

void QNetworkAccessBackend::ignoreSslErrors()
{
    // do nothing
}

void QNetworkAccessBackend::fetchSslConfiguration(QSslConfiguration &) const
{
    // do nothing
}

void QNetworkAccessBackend::setSslConfiguration(const QSslConfiguration &)
{
    // do nothing
}

QNetworkAccessManager::Operation QNetworkAccessBackend::operation() const
{
    return reply->operation;
}

QNetworkRequest QNetworkAccessBackend::request() const
{
    return reply->request;
}

#ifndef QT_NO_NETWORKPROXY
QNetworkProxy QNetworkAccessBackend::proxy() const
{
    return reply->networkProxy;
}
#endif

QAbstractNetworkCache *QNetworkAccessBackend::networkCache() const
{
    return reply->networkCache; // should be the same as manager->networkCache
}

void QNetworkAccessBackend::setCachingEnabled(bool)
{
    // for the future
}

qint64 QNetworkAccessBackend::upstreamBytesAvailable() const
{
    return reply->writeBuffer.size();
}

void QNetworkAccessBackend::upstreamBytesConsumed(qint64 count)
{
    // remove count bytes from the write buffer
    reply->consume(count);
}

QByteArray QNetworkAccessBackend::readUpstream()
{
    // ### this is expensive. Consider making QRingBuffer::peekAll keep the buffer it returns
    return reply->writeBuffer.peek(upstreamBytesAvailable());
}

qint64 QNetworkAccessBackend::nextDownstreamBlockSize() const
{
    return reply->nextDownstreamBlockSize();
}

qint64 QNetworkAccessBackend::downstreamBytesToConsume() const
{
    return reply->writeBuffer.size();
}

void QNetworkAccessBackend::writeDownstreamData(const QByteArray &data)
{
    reply->feed(data);
}

void QNetworkAccessBackend::writeDownstreamData(QIODevice *data)
{
    reply->feed(data);
}

QVariant QNetworkAccessBackend::header(QNetworkRequest::KnownHeaders header) const
{
    return reply->q_func()->header(header);
}

void QNetworkAccessBackend::setHeader(QNetworkRequest::KnownHeaders header, const QVariant &value)
{
    reply->setCookedHeader(header, value);
}

bool QNetworkAccessBackend::hasRawHeader(const QByteArray &headerName) const
{
    return reply->q_func()->hasRawHeader(headerName);
}

QByteArray QNetworkAccessBackend::rawHeader(const QByteArray &headerName) const
{
    return reply->q_func()->rawHeader(headerName);
}

QList<QByteArray> QNetworkAccessBackend::rawHeaderList() const
{
    return reply->q_func()->rawHeaderList();
}

void QNetworkAccessBackend::setRawHeader(const QByteArray &headerName, const QByteArray &headerValue)
{
    reply->setRawHeader(headerName, headerValue);
}

QVariant QNetworkAccessBackend::attribute(QNetworkRequest::Attribute code) const
{
    return reply->q_func()->attribute(code);
}

void QNetworkAccessBackend::setAttribute(QNetworkRequest::Attribute code, const QVariant &value)
{
    if (value.isValid())
        reply->attributes.insert(code, value);
    else
        reply->attributes.remove(code);
}
QUrl QNetworkAccessBackend::url() const
{
    return reply->url;
}

void QNetworkAccessBackend::setUrl(const QUrl &url)
{
    reply->url = url;
}

void QNetworkAccessBackend::finished()
{
    reply->finished();
}

void QNetworkAccessBackend::error(QNetworkReply::NetworkError code, const QString &errorString)
{
    reply->error(code, errorString);
}

#ifndef QT_NO_NETWORKPROXY
void QNetworkAccessBackend::proxyAuthenticationRequired(const QNetworkProxy &proxy,
                                                        QAuthenticator *authenticator)
{
    manager->proxyAuthenticationRequired(this, proxy, authenticator);
}
#endif

void QNetworkAccessBackend::authenticationRequired(QAuthenticator *authenticator)
{
    manager->authenticationRequired(this, authenticator);
}

void QNetworkAccessBackend::metaDataChanged()
{
    reply->metaDataChanged();
}

void QNetworkAccessBackend::redirectionRequested(const QUrl &target)
{
    reply->redirectionRequested(target);
}

void QNetworkAccessBackend::sslErrors(const QList<QSslError> &errors)
{
#ifndef QT_NO_OPENSSL
    reply->sslErrors(errors);
#else
    Q_UNUSED(errors);
#endif
}

QT_END_NAMESPACE
