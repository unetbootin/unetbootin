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

#ifndef QNETWORKACCESSFTPBACKEND_P_H
#define QNETWORKACCESSFTPBACKEND_P_H

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

#include "qnetworkaccessbackend_p.h"
#include "qnetworkaccesscache_p.h"
#include "qnetworkrequest.h"
#include "qnetworkreply.h"
#include "QtNetwork/qftp.h"

#include "QtCore/qpointer.h"

#ifndef QT_NO_FTP

QT_BEGIN_NAMESPACE

class QNetworkAccessFtpIODevice;
class QNetworkAccessFtpFtp;

class QNetworkAccessFtpBackend: public QNetworkAccessBackend
{
    Q_OBJECT
public:
    enum State {
        Idle,
        //Connecting,
        LoggingIn,
        Statting,
        Transferring,
        Disconnecting
    };

    QNetworkAccessFtpBackend();
    virtual ~QNetworkAccessFtpBackend();

    virtual void open();
    virtual void closeDownstreamChannel();
    virtual void closeUpstreamChannel();
    virtual bool waitForDownstreamReadyRead(int msecs);
    virtual bool waitForUpstreamBytesWritten(int msecs);

    virtual void upstreamReadyRead();
    virtual void downstreamReadyWrite();

    void disconnectFromFtp();

public slots:
    void ftpConnectionReady(QNetworkAccessCache::CacheableObject *object);
    void ftpDone();
    void ftpReadyRead();
    void ftpRawCommandReply(int code, const QString &text);

private:
    friend class QNetworkAccessFtpIODevice;
    QPointer<QNetworkAccessFtpFtp> ftp;
    QNetworkAccessFtpIODevice *uploadDevice;
    qint64 totalBytes;
    int sizeId, mdtmId;
    State state;
};

class QNetworkAccessFtpBackendFactory: public QNetworkAccessBackendFactory
{
public:
    virtual QNetworkAccessBackend *create(QNetworkAccessManager::Operation op,
                                          const QNetworkRequest &request) const;
};

QT_END_NAMESPACE

#endif // QT_NO_FTP

#endif
