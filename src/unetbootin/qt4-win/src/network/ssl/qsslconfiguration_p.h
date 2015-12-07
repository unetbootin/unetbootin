/****************************************************************************
**
** Copyright (C) 2007-2008 Trolltech ASA. All rights reserved.
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

/****************************************************************************
**
** In addition, as a special exception, Trolltech gives permission to link
** the code of its release of Qt with the OpenSSL project's "OpenSSL" library
** (or modified versions of the "OpenSSL" library that use the same license
** as the original version), and distribute the linked executables.
**
** You must comply with the GNU General Public License version 2 in all
** respects for all of the code used other than the "OpenSSL" code.  If you
** modify this file, you may extend this exception to your version of the file,
** but you are not obligated to do so.  If you do not wish to do so, delete
** this exception statement from your version of this file.
**
****************************************************************************/

#ifndef QSSLCONFIGURATION_P_H
#define QSSLCONFIGURATION_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists for the convenience
// of the QSslSocket API.  This header file may change from
// version to version without notice, or even be removed.
//
// We mean it.
//

#include "qsslconfiguration.h"
#include "qlist.h"
#include "qsslcertificate.h"
#include "qsslcipher.h"
#include "qsslkey.h"

QT_BEGIN_NAMESPACE

class QSslConfigurationPrivate
{
public:
    QSslConfigurationPrivate()
        : ref(1),
          protocol(QSsl::SslV3),
          peerVerifyMode(QSslSocket::AutoVerifyPeer),
          peerVerifyDepth(0)
    { }

    QAtomicInt ref;
    QSslCertificate peerCertificate;
    QList<QSslCertificate> peerCertificateChain;
    QSslCertificate localCertificate;

    QSslKey privateKey;
    QSslCipher sessionCipher;
    QList<QSslCipher> ciphers;
    QList<QSslCertificate> caCertificates;

    QSsl::SslProtocol protocol;
    QSslSocket::PeerVerifyMode peerVerifyMode;
    int peerVerifyDepth;

    // in qsslsocket.cpp:
    static QSslConfiguration defaultConfiguration();
    static void setDefaultConfiguration(const QSslConfiguration &configuration);
    static void deepCopyDefaultConfiguration(QSslConfigurationPrivate *config);
};

// implemented here for inlining purposes
inline QSslConfiguration::QSslConfiguration(QSslConfigurationPrivate *dd)
    : d(dd)
{
    d->ref.ref();
}

QT_END_NAMESPACE

#endif
