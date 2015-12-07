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

#include "qnetworkreplyimpl_p.h"
#include "qnetworkcookie.h"
#include "QtCore/qcoreapplication.h"
#include "QtCore/qdatetime.h"
#include "QtNetwork/qsslconfiguration.h"

#include <QtCore/QCoreApplication>

QT_BEGIN_NAMESPACE

inline QNetworkReplyImplPrivate::QNetworkReplyImplPrivate()
    : copyDevice(0), networkCache(0),
      bytesDownloaded(0), lastBytesDownloaded(-1), bytesUploaded(-1),
      state(Idle)
{
}

void QNetworkReplyImplPrivate::_q_startOperation()
{
    // This function is called exactly once
    state = Working;
    if (!backend) {
        error(QNetworkReplyImpl::ProtocolUnknownError,
              QCoreApplication::translate("QNetworkReply", "Protocol \"%1\" is unknown").arg(url.scheme())); // not really true!;
        finished();
        return;
    }

    backend->open();
    if (state != Finished) {
        if (operation == QNetworkAccessManager::GetOperation)
            pendingNotifications.append(NotifyDownstreamReadyWrite);
        if (outgoingData) {
            _q_sourceReadyRead();
#if 0 // ### FIXME
            if (outgoingData->atEndOfStream() && writeBuffer.isEmpty())
                // empty upload
                emit q->uploadProgress(0, 0);
#endif
        }

        handleNotifications();
    }
}

void QNetworkReplyImplPrivate::_q_sourceReadyRead()
{
    // read data from the outgoingData QIODevice into our internal buffer
    enum { DesiredBufferSize = 32 * 1024 };

    if (writeBuffer.size() >= DesiredBufferSize)
        return;                 // don't grow the buffer too much

    // read as many bytes are available or up until we fill up the buffer
    // but always read at least one byte
    qint64 bytesToRead = qBound<qint64>(1, outgoingData->bytesAvailable(),
                                        DesiredBufferSize - writeBuffer.size());
    char *ptr = writeBuffer.reserve(bytesToRead);
    qint64 bytesActuallyRead = outgoingData->read(ptr, bytesToRead);
    if (bytesActuallyRead == -1) {
        // EOF
        writeBuffer.chop(bytesToRead);
        backendNotify(NotifyCloseUpstreamChannel);
        return;
    }

    if (bytesActuallyRead < bytesToRead)
        writeBuffer.chop(bytesToRead - bytesActuallyRead);

    // if we did read anything, let the backend know and handle it
    if (bytesActuallyRead)
        backendNotify(NotifyUpstreamReadyRead);

    // check for EOF again
    if (!outgoingData->isSequential() && outgoingData->atEnd())
        backendNotify(NotifyCloseUpstreamChannel);
}

void QNetworkReplyImplPrivate::_q_sourceReadChannelFinished()
{
    _q_sourceReadyRead();
}

void QNetworkReplyImplPrivate::_q_copyReadyRead()
{
    Q_Q(QNetworkReplyImpl);
    if (!copyDevice && !q->isOpen())
        return;

    qint64 bytesToRead = nextDownstreamBlockSize();
    if (bytesToRead == 0)
        // we'll be called again, eventually
        return;

    bytesToRead = qBound<qint64>(1, bytesToRead, copyDevice->bytesAvailable());
    char *ptr = readBuffer.reserve(bytesToRead);
    qint64 bytesRead = copyDevice->read(ptr, bytesToRead);
    if (bytesRead == -1 || (!copyDevice->isSequential() && copyDevice->atEnd())) {
        // EOF
        readBuffer.chop(bytesToRead);
        backendNotify(NotifyCopyFinished);
        return;
    }

    if (bytesRead != bytesToRead)
        readBuffer.chop(bytesToRead - bytesRead);

    bytesDownloaded += bytesRead;
    lastBytesDownloaded = bytesDownloaded;
    QVariant totalSize = cookedHeaders.value(QNetworkRequest::ContentLengthHeader);
    emit q->downloadProgress(bytesDownloaded,
                             totalSize.isNull() ? Q_INT64_C(-1) : totalSize.toLongLong());
    emit q->readyRead();
}

void QNetworkReplyImplPrivate::_q_copyReadChannelFinished()
{
    _q_copyReadyRead();
}

void QNetworkReplyImplPrivate::setup(QNetworkAccessManager::Operation op, const QNetworkRequest &req,
                                     QIODevice *data)
{
    Q_Q(QNetworkReplyImpl);

    outgoingData = data;
    request = req;
    url = request.url();
    operation = op;

    if (outgoingData) {
        q->connect(outgoingData, SIGNAL(readyRead()), SLOT(_q_sourceReadyRead()));
        q->connect(outgoingData, SIGNAL(readChannelFinished()), SLOT(_q_sourceReadChannelFinished()));
    }

    q->setOpenMode(QIODevice::ReadOnly);
    QMetaObject::invokeMethod(q, "_q_startOperation", Qt::QueuedConnection);
}

void QNetworkReplyImplPrivate::setNetworkCache(QAbstractNetworkCache *nc)
{
    networkCache = nc;
}

void QNetworkReplyImplPrivate::backendNotify(InternalNotifications notification)
{
    Q_Q(QNetworkReplyImpl);
    if (!pendingNotifications.contains(notification))
        pendingNotifications.enqueue(notification);

    if (pendingNotifications.size() == 1)
        QCoreApplication::postEvent(q, new QEvent(QEvent::NetworkReplyUpdated));
}

void QNetworkReplyImplPrivate::handleNotifications()
{
    NotificationQueue current = pendingNotifications;
    pendingNotifications.clear();

    if (state != Working)
        return;

    while (!current.isEmpty()) {
        InternalNotifications notification = current.dequeue();
        switch (notification) {
        case NotifyDownstreamReadyWrite:
            if (copyDevice)
                _q_copyReadyRead();
            else
                backend->downstreamReadyWrite();
            break;

        case NotifyUpstreamReadyRead:
            backend->upstreamReadyRead();
            break;

        case NotifyCloseDownstreamChannel:
            backend->closeDownstreamChannel();
            break;

        case NotifyCloseUpstreamChannel:
            backend->closeUpstreamChannel();
            break;

        case NotifyCopyFinished: {
            QIODevice *dev = copyDevice;
            copyDevice = 0;
            backend->copyFinished(dev);
            break;
        }
        }
    }
}

void QNetworkReplyImplPrivate::consume(qint64 count)
{
    Q_Q(QNetworkReplyImpl);
    if (count <= 0) {
        qWarning("QNetworkConnection: backend signalled that it consumed %ld bytes", long(count));
        return;
    }

    if (outgoingData)
        // schedule another read from the source
        QMetaObject::invokeMethod(q_func(), "_q_sourceReadyRead", Qt::QueuedConnection);

    writeBuffer.skip(count);
    if (bytesUploaded == -1)
        bytesUploaded = count;
    else
        bytesUploaded += count;

    QVariant totalSize = request.header(QNetworkRequest::ContentLengthHeader);
    emit q->uploadProgress(bytesUploaded,
                           totalSize.isNull() ? Q_INT64_C(-1) : totalSize.toLongLong());
}

qint64 QNetworkReplyImplPrivate::nextDownstreamBlockSize() const
{
    enum { DesiredBufferSize = 32 * 1024 };
    if (readBufferMaxSize == 0)
        return DesiredBufferSize;

    return qMax<qint64>(0, readBufferMaxSize - readBuffer.size());
}

void QNetworkReplyImplPrivate::feed(const QByteArray &data)
{
    Q_Q(QNetworkReplyImpl);
    if (!q->isOpen())
        return;

    char *ptr = readBuffer.reserve(data.size());
    memcpy(ptr, data.constData(), data.size());

    bytesDownloaded += data.size();
    lastBytesDownloaded = bytesDownloaded;

    QPointer<QNetworkReplyImpl> qq = q;

    QVariant totalSize = cookedHeaders.value(QNetworkRequest::ContentLengthHeader);
    emit q->downloadProgress(bytesDownloaded,
                             totalSize.isNull() ? Q_INT64_C(-1) : totalSize.toLongLong());
    emit q->readyRead();

    // hopefully we haven't been deleted here
    if (!qq.isNull()) {
        // do we still have room in the buffer?
        if (nextDownstreamBlockSize() > 0)
            backendNotify(QNetworkReplyImplPrivate::NotifyDownstreamReadyWrite);
    }
}

void QNetworkReplyImplPrivate::feed(QIODevice *data)
{
    Q_Q(QNetworkReplyImpl);
    if (!q->isOpen())
        return;

    // read until EOF from data
    if (copyDevice) {
        qCritical("QNetworkReplyImpl: copy from QIODevice already in progress -- "
                  "backend probly needs to be fixed");
        return;
    }

    copyDevice = data;
    q->connect(copyDevice, SIGNAL(readyRead()), SLOT(_q_copyReadyRead()));
    q->connect(copyDevice, SIGNAL(readChannelFinished()), SLOT(_q_copyReadChannelFinished()));

    // start the copy:
    _q_copyReadyRead();
}

void QNetworkReplyImplPrivate::finished()
{
    Q_Q(QNetworkReplyImpl);
    Q_ASSERT_X(state != Finished, "QNetworkReplyImpl",
               "Backend called finished/finishedWithError more than once");

    state = Finished;
    pendingNotifications.clear();

    QVariant totalSize = cookedHeaders.value(QNetworkRequest::ContentLengthHeader);
    if (bytesDownloaded != lastBytesDownloaded || totalSize.isNull())
        emit q->downloadProgress(bytesDownloaded, bytesDownloaded);
    if (bytesUploaded == -1 && outgoingData)
        emit q->uploadProgress(0, 0);

    // note: might not be a good idea, since users could decide to delete us
    // which would delete the backend too...
    // maybe we should protect the backend
    emit q->readChannelFinished();
    emit q->finished();
}

void QNetworkReplyImplPrivate::error(QNetworkReplyImpl::NetworkError code, const QString &errorMessage)
{
    Q_Q(QNetworkReplyImpl);

    errorCode = code;
    q->setErrorString(errorMessage);

    // note: might not be a good idea, since users could decide to delete us
    // which would delete the backend too...
    // maybe we should protect the backend
    emit q->error(code);
}

void QNetworkReplyImplPrivate::metaDataChanged()
{
    Q_Q(QNetworkReplyImpl);
    // do we have cookies?
    if (cookedHeaders.contains(QNetworkRequest::SetCookieHeader) && !manager.isNull()) {
        QList<QNetworkCookie> cookies =
            qvariant_cast<QList<QNetworkCookie> >(cookedHeaders.value(QNetworkRequest::SetCookieHeader));
        QNetworkCookieJar *jar = manager->cookieJar();
        if (jar)
            jar->setCookiesFromUrl(cookies, url);
    }
    emit q->metaDataChanged();
}

void QNetworkReplyImplPrivate::redirectionRequested(const QUrl &target)
{
    attributes.insert(QNetworkRequest::RedirectionTargetAttribute, target);
}

void QNetworkReplyImplPrivate::sslErrors(const QList<QSslError> &errors)
{
#ifndef QT_NO_OPENSSL
    Q_Q(QNetworkReplyImpl);
    emit q->sslErrors(errors);
#else
    Q_UNUSED(errors);
#endif
}

QNetworkReplyImpl::QNetworkReplyImpl(QObject *parent)
    : QNetworkReply(*new QNetworkReplyImplPrivate, parent)
{
}

QNetworkReplyImpl::~QNetworkReplyImpl()
{
}

void QNetworkReplyImpl::abort()
{
    Q_D(QNetworkReplyImpl);
    if (d->state == QNetworkReplyImplPrivate::Aborted)
        return;

    // stop both upload and download
    if (d->backend) {
        d->backend->deleteLater();
        d->backend = 0;
    }
    if (d->outgoingData)
        disconnect(d->outgoingData, 0, this, 0);
    if (d->copyDevice)
        disconnect(d->copyDevice, 0, this, 0);

    QNetworkReply::close();

    if (d->state != QNetworkReplyImplPrivate::Finished) {
        // emit signals
        d->error(OperationCanceledError, tr("Operation canceled"));
        d->finished();
    }
    d->state = QNetworkReplyImplPrivate::Aborted;
}

void QNetworkReplyImpl::close()
{
    Q_D(QNetworkReplyImpl);
    if (d->state == QNetworkReplyImplPrivate::Aborted ||
        d->state == QNetworkReplyImplPrivate::Finished)
        return;

    // stop the download
    if (d->backend)
        d->backend->closeDownstreamChannel();
    if (d->copyDevice)
        disconnect(d->copyDevice, 0, this, 0);

    QNetworkReply::close();

    // emit signals
    d->error(OperationCanceledError, tr("Operation canceled"));
    d->finished();
}

/*!
    Returns the number of bytes available for reading with
    QIODevice::read(). The number of bytes available may grow until
    the finished() signal is emitted.
*/
qint64 QNetworkReplyImpl::bytesAvailable() const
{
    return QNetworkReply::bytesAvailable() + d_func()->readBuffer.size();
}

void QNetworkReplyImpl::setReadBufferSize(qint64 size)
{
    Q_D(QNetworkReplyImpl);
    if (size > d->readBufferMaxSize &&
        size == d->readBuffer.size())
        d->backendNotify(QNetworkReplyImplPrivate::NotifyDownstreamReadyWrite);

    QNetworkReply::setReadBufferSize(size);
}

#ifndef QT_NO_OPENSSL
QSslConfiguration QNetworkReplyImpl::sslConfigurationImplementation() const
{
    Q_D(const QNetworkReplyImpl);
    QSslConfiguration config;
    if (d->backend)
        d->backend->fetchSslConfiguration(config);
    return config;
}

void QNetworkReplyImpl::setSslConfigurationImplementation(const QSslConfiguration &config)
{
    Q_D(QNetworkReplyImpl);
    if (d->backend && !config.isNull())
        d->backend->setSslConfiguration(config);
}

void QNetworkReplyImpl::ignoreSslErrors()
{
    Q_D(QNetworkReplyImpl);
    if (d->backend)
        d->backend->ignoreSslErrors();
}

#endif  // QT_NO_OPENSSL

/*!
    \internal
*/
qint64 QNetworkReplyImpl::readData(char *data, qint64 maxlen)
{
    Q_D(QNetworkReplyImpl);
    if (d->readBuffer.isEmpty())
        return d->state == QNetworkReplyImplPrivate::Finished ? -1 : 0;

    d->backendNotify(QNetworkReplyImplPrivate::NotifyDownstreamReadyWrite);
    if (maxlen == 1) {
        // optimization for getChar()
        *data = d->readBuffer.getChar();
        return 1;
    }

    maxlen = qMin<qint64>(maxlen, d->readBuffer.size());
    return d->readBuffer.read(data, maxlen);
}

/*!
   \internal Reimplemented for internal purposes
*/
bool QNetworkReplyImpl::event(QEvent *e)
{
    if (e->type() == QEvent::NetworkReplyUpdated) {
        d_func()->handleNotifications();
        return true;
    }

    return QObject::event(e);
}

QT_END_NAMESPACE

#include "moc_qnetworkreplyimpl_p.cpp"

