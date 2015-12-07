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


/*!
    \class QSslError
    \brief The QSslError class provides an SSL error.
    \since 4.3

    \reentrant
    \ingroup io
    \module network

    QSslError provides a simple API for managing errors during QSslSocket's
    SSL handshake.

    \sa QSslSocket, QSslCertificate, QSslCipher
*/

/*!
    \enum QSslError::SslError

    Describes all recognized errors that can occur during an SSL handshake.
    
    \value NoError
    \value UnableToGetIssuerCertificate
    \value UnableToDecryptCertificateSignature
    \value UnableToDecodeIssuerPublicKey
    \value CertificateSignatureFailed
    \value CertificateNotYetValid
    \value CertificateExpired
    \value InvalidNotBeforeField
    \value InvalidNotAfterField
    \value SelfSignedCertificate
    \value SelfSignedCertificateInChain
    \value UnableToGetLocalIssuerCertificate
    \value UnableToVerifyFirstCertificate
    \value CertificateRevoked
    \value InvalidCaCertificate
    \value PathLengthExceeded
    \value InvalidPurpose
    \value CertificateUntrusted
    \value CertificateRejected
    \value SubjectIssuerMismatch
    \value AuthorityIssuerSerialNumberMismatch
    \value NoPeerCertificate
    \value HostNameMismatch
    \value UnspecifiedError
    \value NoSslSupport

    \sa QSslError::errorString()
*/

#include "qsslerror.h"
#ifndef QT_NO_DEBUG_STREAM
#include <QtCore/qdebug.h>

QT_BEGIN_NAMESPACE
#endif

class QSslErrorPrivate
{
public:
    QSslError::SslError error;
    QSslCertificate certificate;
};

/*!
    Constructs a QSslError object. The two optional arguments specify the \a
    error that occurred, and which \a certificate the error relates to.

    \sa QSslCertificate
*/
QSslError::QSslError(SslError error, const QSslCertificate &certificate)
    : d(new QSslErrorPrivate)
{
    d->error = error;
    d->certificate = certificate;
}

/*!
    Constructs an identical copy of \a other.
*/
QSslError::QSslError(const QSslError &other)
    : d(new QSslErrorPrivate)
{
    *d = *other.d;
}

/*!
    Destroys the QSslError object.
*/
QSslError::~QSslError()
{
    delete d;
}

/*!
    Assigns the contents of \a other to this error.
*/
QSslError &QSslError::operator=(const QSslError &other)
{
    *d = *other.d;
    return *this;
}

/*!
    Returns true if this error is equal to \a other; otherwise returns false.
*/
bool QSslError::operator==(const QSslError &other) const
{
    return d->error == other.d->error
        && d->certificate == other.d->certificate;
}

/*!
    \fn bool QSslError::operator!=(const QSslError &other) const

    Returns true if this error is not equal to \a other; otherwise returns
    false.
*/

/*!
    Returns the type of the error.

    \sa errorString(), certificate()
*/
QSslError::SslError QSslError::error() const
{
    return d->error;
}

/*!
    Returns a short localized human-readable description of the error.

    \sa error(), certificate()
*/
QString QSslError::errorString() const
{
    QString errStr;
    switch (d->error) {
    case NoError:
        errStr = QObject::tr(QT_TRANSLATE_NOOP(QSslError, "No error"));
        break;
    case UnableToGetIssuerCertificate:
        errStr = QObject::tr(QT_TRANSLATE_NOOP(QSslError, "The issuer certificate could not be found"));
        break;
    case UnableToDecryptCertificateSignature:
        errStr = QObject::tr(QT_TRANSLATE_NOOP(QSslError, "The certificate signature could not be decrypted"));
        break;
    case UnableToDecodeIssuerPublicKey:
        errStr = QObject::tr(QT_TRANSLATE_NOOP(QSslError, "The public key in the certificate could not be read"));
        break;
    case CertificateSignatureFailed:
        errStr = QObject::tr(QT_TRANSLATE_NOOP(QSslError, "The signature of the certificate is invalid"));
        break;
    case CertificateNotYetValid:
        errStr = QObject::tr(QT_TRANSLATE_NOOP(QSslError, "The certificate is not yet valid"));
        break;
    case CertificateExpired:
        errStr = QObject::tr(QT_TRANSLATE_NOOP(QSslError, "The certificate has expired"));
        break;
    case InvalidNotBeforeField:
        errStr = QObject::tr(QT_TRANSLATE_NOOP(QSslError, "The certificate's notBefore field contains an invalid time"));
        break;
    case InvalidNotAfterField:
        errStr = QObject::tr(QT_TRANSLATE_NOOP(QSslError, "The certificate's notAfter field contains an invalid time"));
        break;
    case SelfSignedCertificate:
        errStr = QObject::tr(QT_TRANSLATE_NOOP(QSslError, "The certificate is self-signed, and untrusted"));
        break;
    case SelfSignedCertificateInChain:
        errStr = QObject::tr(QT_TRANSLATE_NOOP(QSslError, "The root certificate of the certificate chain is self-signed, and untrusted"));
        break;
    case UnableToGetLocalIssuerCertificate:
        errStr = QObject::tr(QT_TRANSLATE_NOOP(QSslError, "The issuer certificate of a locally looked up certificate could not be found"));
        break;
    case UnableToVerifyFirstCertificate:
        errStr = QObject::tr(QT_TRANSLATE_NOOP(QSslError, "No certificates could be verified"));
        break;
    case InvalidCaCertificate:
        errStr = QObject::tr(QT_TRANSLATE_NOOP(QSslError, "One of the CA certificates is invalid"));
        break;
    case PathLengthExceeded:
        errStr = QObject::tr(QT_TRANSLATE_NOOP(QSslError, "The basicConstraints pathlength parameter has been exceeded"));
        break;
    case InvalidPurpose:
        errStr = QObject::tr(QT_TRANSLATE_NOOP(QSslError, "The supplied certificate is unsuited for this purpose"));
        break;
    case CertificateUntrusted:
        errStr = QObject::tr(QT_TRANSLATE_NOOP(QSslError, "The root CA certificate is not trusted for this purpose"));
        break;
    case CertificateRejected:
        errStr = QObject::tr(QT_TRANSLATE_NOOP(QSslError, "The root CA certificate is marked to reject the specified purpose"));
        break;
    case SubjectIssuerMismatch: // hostname mismatch
        errStr = QObject::tr(QT_TRANSLATE_NOOP(QSslError,
                                               "The current candidate issuer certificate was rejected because its"
                                               " subject name did not match the issuer name of the current certificate"));
        break;
    case AuthorityIssuerSerialNumberMismatch:
        errStr = QObject::tr(QT_TRANSLATE_NOOP(QSslError, "The current candidate issuer certificate was rejected because"
                                               " its issuer name and serial number was present and did not match the"
                                               " authority key identifier of the current certificate"));
        break;
    case NoPeerCertificate:
        errStr = QObject::tr(QT_TRANSLATE_NOOP(QSslError, "The peer did not present any certificate"));
        break;
    case HostNameMismatch:
        errStr = QObject::tr(QT_TRANSLATE_NOOP(QSslError,
                                               "The host name did not match any of the valid hosts"
                                               " for this certificate"));
        break;
    case NoSslSupport:
        break;
    default:
        errStr = QObject::tr(QT_TRANSLATE_NOOP(QSslError, "Unknown error"));
        break;
    }

    return errStr;
}

/*!
    Returns the certificate associated with this error, or a null certificate
    if the error does not relate to any certificate.

    \sa error(), errorString()
*/
QSslCertificate QSslError::certificate() const
{
    return d->certificate;
}

#ifndef QT_NO_DEBUG_STREAM
//class QDebug;
QDebug operator<<(QDebug debug, const QSslError &error)
{
    debug << error.errorString();
    return debug;
}
QDebug operator<<(QDebug debug, const QSslError::SslError &error)
{
    debug << QSslError(error).errorString();
    return debug;
}
#endif

QT_END_NAMESPACE
