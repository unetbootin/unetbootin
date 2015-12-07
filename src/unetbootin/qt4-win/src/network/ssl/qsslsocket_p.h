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
** In addition, as a special exception, Trolltech gives permission to
** link the code of its release of Qt with the OpenSSL project's
** "OpenSSL" library (or modified versions of it that use the same
** license as the "OpenSSL" library), and distribute the linked
** executables.  You must comply with the GNU General Public License
** version 2 or the GNU General Public License version 3 in all
** respects for all of the code used other than the "OpenSSL" code.
** If you modify this file, you may extend this exception to your
** version of the file, but you are not obligated to do so.  If you do
** not wish to do so, delete this exception statement from your
** version of this file.
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


#ifndef QSSLSOCKET_P_H
#define QSSLSOCKET_P_H

#include "qsslsocket.h"

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists for the convenience
// of the QLibrary class.  This header file may change from
// version to version without notice, or even be removed.
//
// We mean it.
//

#include <private/qtcpsocket_p.h>
#include "qsslkey.h"
#include "qsslconfiguration_p.h"

#include <QtCore/qstringlist.h>

#include <private/qringbuffer_p.h>

QT_BEGIN_NAMESPACE

class QSslSocketPrivate : public QTcpSocketPrivate
{
    Q_DECLARE_PUBLIC(QSslSocket)
public:
    QSslSocketPrivate();
    virtual ~QSslSocketPrivate();

    void init();
    bool initialized;

    QSslSocket::SslMode mode;
    bool autoStartHandshake;
    bool connectionEncrypted;
    bool ignoreSslErrors;

    QRingBuffer readBuffer;
    QRingBuffer writeBuffer;

    QSslConfigurationPrivate configuration;
    QList<QSslError> sslErrors;

    static bool ensureInitialized();
    static QList<QSslCipher> defaultCiphers();
    static QList<QSslCipher> supportedCiphers();
    static void setDefaultCiphers(const QList<QSslCipher> &ciphers);
    static void setDefaultSupportedCiphers(const QList<QSslCipher> &ciphers);
    static void resetDefaultCiphers();

    static QList<QSslCertificate> defaultCaCertificates();
    static QList<QSslCertificate> systemCaCertificates();
    static void setDefaultCaCertificates(const QList<QSslCertificate> &certs);
    static bool addDefaultCaCertificates(const QString &path, QSsl::EncodingFormat format,
                                         QRegExp::PatternSyntax syntax);
    static void addDefaultCaCertificate(const QSslCertificate &cert);
    static void addDefaultCaCertificates(const QList<QSslCertificate> &certs);

    // The socket itself, including private slots.
    QTcpSocket *plainSocket;
    void createPlainSocket(QIODevice::OpenMode openMode);
    void _q_connectedSlot();
    void _q_hostFoundSlot();
    void _q_disconnectedSlot();
    void _q_stateChangedSlot(QAbstractSocket::SocketState);
    void _q_errorSlot(QAbstractSocket::SocketError);
    void _q_readyReadSlot();
    void _q_bytesWrittenSlot(qint64);
    void _q_flushWriteBuffer();

    // Platform specific functions
    virtual void startClientEncryption() = 0;
    virtual void startServerEncryption() = 0;
    virtual void transmit() = 0;
    virtual void disconnectFromHost() = 0;
    virtual void disconnected() = 0;
    virtual QSslCipher sessionCipher() const = 0;
};

QT_END_NAMESPACE

#endif
