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

#include "qnetworkaccessfilebackend_p.h"
#include "qfileinfo.h"
#include "qurlinfo.h"
#include "qdir.h"

#include <QtCore/QCoreApplication>

QT_BEGIN_NAMESPACE

QNetworkAccessBackend *
QNetworkAccessFileBackendFactory::create(QNetworkAccessManager::Operation op,
                                         const QNetworkRequest &request) const
{
    // is it an operation we know of?
    switch (op) {
    case QNetworkAccessManager::GetOperation:
    case QNetworkAccessManager::PutOperation:
        break;

    default:
        // no, we can't handle this operation
        return 0;
    }

    QUrl url = request.url();
    if (url.scheme() == QLatin1String("qrc") || !url.toLocalFile().isEmpty())
        return new QNetworkAccessFileBackend;
    else if (!url.isEmpty() && url.authority().isEmpty()) {
        // check if QFile could, in theory, open this URL
        QFileInfo fi(url.toString(QUrl::RemoveAuthority | QUrl::RemoveFragment | QUrl::RemoveQuery));
        if (fi.exists() || (op == QNetworkAccessManager::PutOperation && fi.dir().exists()))
            return new QNetworkAccessFileBackend;
    }

    return 0;
}

QNetworkAccessFileBackend::QNetworkAccessFileBackend()
    : totalBytes(0)
{
}

QNetworkAccessFileBackend::~QNetworkAccessFileBackend()
{
}

void QNetworkAccessFileBackend::open()
{
    QUrl url = this->url();

    if (url.host() == QLatin1String("localhost"))
        url.setHost(QString());
#if !defined(Q_OS_WIN)
    // do not allow UNC paths on Unix
    if (!url.host().isEmpty()) {
        // we handle only local files
        error(QNetworkReply::ProtocolInvalidOperationError,
              QCoreApplication::translate("QNetworkAccessFileBackend", "Request for opening non-local file %1").arg(url.toString()));
        finished();
        return;
    }
#endif // !defined(Q_OS_WIN)
    if (url.path().isEmpty())
        url.setPath(QLatin1String("/"));
    setUrl(url);

    QString fileName = url.toLocalFile();
    if (fileName.isEmpty()) {
        if (url.scheme() == QLatin1String("qrc"))
            fileName = QLatin1String(":") + url.path();
        else
            fileName = url.toString(QUrl::RemoveAuthority | QUrl::RemoveFragment | QUrl::RemoveQuery);
    }
    file.setFileName(fileName);

    if (operation() == QNetworkAccessManager::GetOperation) {
        if (!loadFileInfo())
            return;
    }

    QIODevice::OpenMode mode;
    switch (operation()) {
    case QNetworkAccessManager::GetOperation:
        mode = QIODevice::ReadOnly;
        break;
    case QNetworkAccessManager::PutOperation:
        mode = QIODevice::WriteOnly | QIODevice::Truncate;
        break;
    default:
        Q_ASSERT_X(false, "QNetworkAccessFileBackend::open",
                   "Got a request operation I cannot handle!!");
        return;
    }

    mode |= QIODevice::Unbuffered;
    bool opened = file.open(mode);

    // could we open the file?
    if (!opened) {
        QString msg = QCoreApplication::translate("QNetworkAccessFileBackend", "Error opening %1: %2")
                                                .arg(this->url().toString(), file.errorString());

        // why couldn't we open the file?
        // if we're opening for reading, either it doesn't exist, or it's access denied
        // if we're opening for writing, not existing means it's access denied too
        if (file.exists() || operation() == QNetworkAccessManager::PutOperation)
            error(QNetworkReply::ContentAccessDenied, msg);
        else
            error(QNetworkReply::ContentNotFoundError, msg);
        finished();
    }
}

void QNetworkAccessFileBackend::closeDownstreamChannel()
{
    if (operation() == QNetworkAccessManager::GetOperation) {
        file.close();
        //downstreamChannelClosed();
    }
}

void QNetworkAccessFileBackend::closeUpstreamChannel()
{
    if (operation() == QNetworkAccessManager::PutOperation) {
        file.close();
        finished();
    }
}

bool QNetworkAccessFileBackend::waitForDownstreamReadyRead(int)
{
    Q_ASSERT(operation() == QNetworkAccessManager::GetOperation);
    return readMoreFromFile();
}

bool QNetworkAccessFileBackend::waitForUpstreamBytesWritten(int)
{
    Q_ASSERT_X(false, "QNetworkAccessFileBackend::waitForUpstreamBytesWritten",
               "This function should never have been called, since there is never anything "
               "left to be written!");
    return false;
}

void QNetworkAccessFileBackend::upstreamReadyRead()
{
    Q_ASSERT_X(operation() == QNetworkAccessManager::PutOperation, "QNetworkAccessFileBackend",
               "We're being told to upload data but operation isn't PUT!");

    // there's more data to be written to the file
    while (upstreamBytesAvailable()) {
        // write everything and let QFile handle it
        int written = file.write(readUpstream());

        if (written < 0) {
            // write error!
            QString msg = QCoreApplication::translate("QNetworkAccessFileBackend", "Write error writing to %1: %2")
                                                    .arg(url().toString(), file.errorString());
            error(QNetworkReply::ProtocolFailure, msg);

            finished();
            return;
        }

        // successful write
        file.flush();
        upstreamBytesConsumed(written);
    }
}

void QNetworkAccessFileBackend::downstreamReadyWrite()
{
    Q_ASSERT_X(operation() == QNetworkAccessManager::GetOperation, "QNetworkAccessFileBackend",
               "We're being told to download data but operation isn't GET!");

    readMoreFromFile();
}

bool QNetworkAccessFileBackend::loadFileInfo()
{
    QFileInfo fi(file);
    setHeader(QNetworkRequest::LastModifiedHeader, fi.lastModified());
    setHeader(QNetworkRequest::ContentLengthHeader, fi.size());

    // signal we're open
    metaDataChanged();

    if (fi.isDir()) {
        error(QNetworkReply::ContentOperationNotPermittedError,
              QCoreApplication::translate("QNetworkAccessFileBackend", "Cannot open %1: Path is a directory").arg(url().toString()));
        finished();
        return false;
    }

    return true;
}

bool QNetworkAccessFileBackend::readMoreFromFile()
{
    qint64 wantToRead;
    while ((wantToRead = nextDownstreamBlockSize()) > 0) {
        // ### FIXME!!
        // Obtain a pointer from the ringbuffer!
        // Avoid extra copy
        QByteArray data;
        data.reserve(wantToRead);
        qint64 actuallyRead = file.read(data.data(), wantToRead);
        if (actuallyRead <= 0) {
            // EOF or error
            if (file.error() != QFile::NoError) {
                QString msg = QCoreApplication::translate("QNetworkAccessFileBackend", "Read error reading from %1: %2")
                              .arg(url().toString(), file.errorString());
                error(QNetworkReply::ProtocolFailure, msg);

                finished();
                return false;
            }

            finished();
            return true;
        }

        data.resize(actuallyRead);
        totalBytes += actuallyRead;
        writeDownstreamData(data);
    }
    return true;
}

QT_END_NAMESPACE
