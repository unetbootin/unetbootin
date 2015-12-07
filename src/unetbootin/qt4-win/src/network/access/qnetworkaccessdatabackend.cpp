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

#include "qnetworkaccessdatabackend_p.h"
#include "qnetworkrequest.h"
#include "qnetworkreply.h"
#include "qurlinfo.h"

QT_BEGIN_NAMESPACE

QNetworkAccessBackend *
QNetworkAccessDataBackendFactory::create(QNetworkAccessManager::Operation,
                                         const QNetworkRequest &request) const
{
    if (request.url().scheme() == QLatin1String("data"))
        return new QNetworkAccessDataBackend;

    return 0;
}

QNetworkAccessDataBackend::QNetworkAccessDataBackend()
{
}

QNetworkAccessDataBackend::~QNetworkAccessDataBackend()
{
}

void QNetworkAccessDataBackend::open()
{
    QUrl uri = request().url();

    if (operation() != QNetworkAccessManager::GetOperation &&
        operation() != QNetworkAccessManager::HeadOperation) {
        // data: doesn't support anything but GET
        QString msg = QObject::tr("Operation not supported on %1")
                      .arg(uri.toString());
        error(QNetworkReply::ContentOperationNotPermittedError, msg);
        finished();
        return;
    }

    if (uri.host().isEmpty()) {
        setHeader(QNetworkRequest::ContentTypeHeader, QLatin1String("text/plain;charset=US-ASCII"));

        // the following would have been the correct thing, but
        // reality often differs from the specification. People have
        // data: URIs with ? and #
        //QByteArray data = QByteArray::fromPercentEncoding(uri.encodedPath());
        QByteArray data = QByteArray::fromPercentEncoding(uri.toEncoded());

        // remove the data: scheme
        data.remove(0, 5);

        // parse it:
        int pos = data.indexOf(',');
        if (pos != -1) {
            QByteArray payload = data.mid(pos + 1);
            data.truncate(pos);
            data = data.trimmed();

            // find out if the payload is encoded in Base64
            if (data.endsWith(";base64")) {
                payload = QByteArray::fromBase64(payload);
                data.chop(7);
            }

            if (data.toLower().startsWith("charset")) {
                int i = 7;      // strlen("charset")
                while (data.at(i) == ' ')
                    ++i;
                if (data.at(i) == '=')
                    data.prepend("text/plain;");
            }

            if (!data.isEmpty())
                setHeader(QNetworkRequest::ContentTypeHeader, data.trimmed());

            setHeader(QNetworkRequest::ContentLengthHeader, payload.size());
            emit metaDataChanged();

            writeDownstreamData(payload);
            finished();
            return;
        }
    }

    // something wrong with this URI
    QString msg = QObject::tr("Invalid URI: %1").arg(uri.toString());
    error(QNetworkReply::ProtocolFailure, msg);
    finished();
}

void QNetworkAccessDataBackend::closeDownstreamChannel()
{
}

void QNetworkAccessDataBackend::closeUpstreamChannel()
{
}

bool QNetworkAccessDataBackend::waitForDownstreamReadyRead(int)
{
    return false;
}

bool QNetworkAccessDataBackend::waitForUpstreamBytesWritten(int)
{
    return false;
}

QT_END_NAMESPACE
