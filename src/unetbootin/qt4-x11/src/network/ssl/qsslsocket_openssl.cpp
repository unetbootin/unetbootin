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

//#define QSSLSOCKET_DEBUG

#include "qsslsocket_openssl_p.h"
#include "qsslsocket_openssl_symbols_p.h"
#include "qsslsocket.h"
#include "qsslcertificate_p.h"
#include "qsslcipher_p.h"

#include <QtCore/qdatetime.h>
#include <QtCore/qdebug.h>
#include <QtCore/qdir.h>
#include <QtCore/qdiriterator.h>
#include <QtCore/qfile.h>
#include <QtCore/qfileinfo.h>
#include <QtCore/qmutex.h>
#include <QtCore/qthread.h>
#include <QtCore/qurl.h>
#include <QtCore/qvarlengtharray.h>

static void initNetworkResources()
{
    // Initialize resources
    Q_INIT_RESOURCE(network);
}

QT_BEGIN_NAMESPACE

// Useful defines
#define SSL_ERRORSTR() QString::fromLocal8Bit(q_ERR_error_string(q_ERR_get_error(), NULL))

/* \internal

    From OpenSSL's thread(3) manual page:

    OpenSSL can safely be used in multi-threaded applications provided that at
    least two callback functions are set.

    locking_function(int mode, int n, const char *file, int line) is needed to
    perform locking on shared data structures.  (Note that OpenSSL uses a
    number of global data structures that will be implicitly shared
    when-whenever ever multiple threads use OpenSSL.)  Multi-threaded
    applications will crash at random if it is not set.  ...
    ...
    id_function(void) is a function that returns a thread ID. It is not
    needed on Windows nor on platforms where getpid() returns a different
    ID for each thread (most notably Linux)
*/
class QOpenSslLocks
{
public:
    inline QOpenSslLocks()
        : initLocker(QMutex::Recursive),
          locksLocker(QMutex::Recursive)
    {
        QMutexLocker locker(&locksLocker);
        int numLocks = q_CRYPTO_num_locks();
        locks = new QMutex *[numLocks];
        memset(locks, 0, numLocks * sizeof(QMutex *));
    }
    inline ~QOpenSslLocks()
    {
        QMutexLocker locker(&locksLocker);
        for (int i = 0; i < q_CRYPTO_num_locks(); ++i)
            delete locks[i];
        delete [] locks;

        QSslSocketPrivate::deinitialize();
    }
    inline QMutex *lock(int num)
    {
        QMutexLocker locker(&locksLocker);
        QMutex *tmp = locks[num];
        if (!tmp)
            tmp = locks[num] = new QMutex(QMutex::Recursive);
        return tmp;
    }

    QMutex *globalLock()
    {
        return &locksLocker;
    }

    QMutex *initLock()
    {
        return &initLocker;
    }

private:
    QMutex initLocker;
    QMutex locksLocker;
    QMutex **locks;
};
Q_GLOBAL_STATIC(QOpenSslLocks, openssl_locks)

extern "C" {
static void locking_function(int mode, int lockNumber, const char *, int)
{
    QMutex *mutex = openssl_locks()->lock(lockNumber);

    // Lock or unlock it
    if (mode & CRYPTO_LOCK)
        mutex->lock();
    else
        mutex->unlock();
}
static unsigned long id_function()
{
    return (unsigned long)QThread::currentThreadId();
}
} // extern "C"

QSslSocketBackendPrivate::QSslSocketBackendPrivate()
    : ssl(0),
      ctx(0),
      readBio(0),
      writeBio(0),
      session(0)
{
    // Calls SSL_library_init().
    ensureInitialized();
}

QSslSocketBackendPrivate::~QSslSocketBackendPrivate()
{
}

QSslCipher QSslSocketBackendPrivate::QSslCipher_from_SSL_CIPHER(SSL_CIPHER *cipher)
{
    QSslCipher ciph;

    char buf [256];
    QString descriptionOneLine = QString::fromLatin1(q_SSL_CIPHER_description(cipher, buf, sizeof(buf)));

    QStringList descriptionList = descriptionOneLine.split(QLatin1String(" "), QString::SkipEmptyParts);
    if (descriptionList.size() > 5) {
        // ### crude code.
        ciph.d->isNull = false;
        ciph.d->name = descriptionList.at(0);

        QString protoString = descriptionList.at(1);
        ciph.d->protocolString = protoString;
        ciph.d->protocol = QSsl::UnknownProtocol;
        if (protoString == QLatin1String("SSLv3"))
            ciph.d->protocol = QSsl::SslV3;
        else if (protoString == QLatin1String("SSLv2"))
            ciph.d->protocol = QSsl::SslV2;
        else if (protoString == QLatin1String("TLSv1"))
            ciph.d->protocol = QSsl::TlsV1;
        
        if (descriptionList.at(2).startsWith(QLatin1String("Kx=")))
            ciph.d->keyExchangeMethod = descriptionList.at(2).mid(3);
        if (descriptionList.at(3).startsWith(QLatin1String("Au=")))
            ciph.d->authenticationMethod = descriptionList.at(3).mid(3);
        if (descriptionList.at(4).startsWith(QLatin1String("Enc=")))
            ciph.d->encryptionMethod = descriptionList.at(4).mid(4);
        ciph.d->exportable = (descriptionList.size() > 6 && descriptionList.at(6) == QLatin1String("export"));

        ciph.d->bits = cipher->strength_bits;
        ciph.d->supportedBits = cipher->alg_bits;

    }
    return ciph;
}

// ### This list is shared between all threads, and protected by a
// mutex. Investigate using thread local storage instead.
struct QSslErrorList
{
    QMutex mutex;
    QList<QPair<int, int> > errors;
};
Q_GLOBAL_STATIC(QSslErrorList, _q_sslErrorList)
static int q_X509Callback(int ok, X509_STORE_CTX *ctx)
{
    if (!ok) {
        // Store the error and at which depth the error was detected.
        _q_sslErrorList()->errors << qMakePair<int, int>(ctx->error, ctx->error_depth);
    }
    // Always return OK to allow verification to continue. We're handle the
    // errors gracefully after collecting all errors, after verification has
    // completed.
    return 1;
}

bool QSslSocketBackendPrivate::initSslContext()
{
    Q_Q(QSslSocket);

    // Create and initialize SSL context. Accept SSLv2, SSLv3 and TLSv1.
    bool client = (mode == QSslSocket::SslClientMode);

    bool reinitialized = false;
init_context:
    switch (configuration.protocol) {
    case QSsl::SslV2:
        ctx = q_SSL_CTX_new(client ? q_SSLv2_client_method() : q_SSLv2_server_method());
        break;
    case QSsl::SslV3:
        ctx = q_SSL_CTX_new(client ? q_SSLv3_client_method() : q_SSLv3_server_method());
        break;
    case QSsl::AnyProtocol:
    default:
        ctx = q_SSL_CTX_new(client ? q_SSLv23_client_method() : q_SSLv23_server_method());
        break;
    case QSsl::TlsV1:
        ctx = q_SSL_CTX_new(client ? q_TLSv1_client_method() : q_TLSv1_server_method());
        break;
    }
    if (!ctx) {
        // After stopping Flash 10 the SSL library looses its ciphers. Try re-adding them
        // by re-initializing the library.
        if (!reinitialized) {
            reinitialized = true;
            if (q_SSL_library_init() == 1)
                goto init_context;
        }

        // ### Bad error code
        q->setErrorString(QSslSocket::tr("Error creating SSL context (%1)").arg(SSL_ERRORSTR()));
        q->setSocketError(QAbstractSocket::UnknownSocketError);
        emit q->error(QAbstractSocket::UnknownSocketError);
        return false;
    }

    // Enable all bug workarounds.
    q_SSL_CTX_set_options(ctx, SSL_OP_ALL);

    // Initialize ciphers
    QByteArray cipherString;
    int first = true;
    QList<QSslCipher> ciphers = configuration.ciphers;
    if (ciphers.isEmpty())
        ciphers = defaultCiphers();
    foreach (const QSslCipher &cipher, ciphers) {
        if (first)
            first = false;
        else
            cipherString.append(":");
        cipherString.append(cipher.name().toLatin1());
    }

    if (!q_SSL_CTX_set_cipher_list(ctx, cipherString.data())) {
        // ### Bad error code
        q->setErrorString(QSslSocket::tr("Invalid or empty cipher list (%1)").arg(SSL_ERRORSTR()));
        q->setSocketError(QAbstractSocket::UnknownSocketError);
        emit q->error(QAbstractSocket::UnknownSocketError);
        return false;
    }

    // Add all our CAs to this store.
    foreach (const QSslCertificate &caCertificate, q->caCertificates())
        q_X509_STORE_add_cert(ctx->cert_store, (X509 *)caCertificate.handle());

    // Register a custom callback to get all verification errors.
    X509_STORE_set_verify_cb_func(ctx->cert_store, q_X509Callback);

    if (!configuration.localCertificate.isNull()) {
        // Require a private key as well.
        if (configuration.privateKey.isNull()) {
            q->setErrorString(QSslSocket::tr("Cannot provide a certificate with no key, %1").arg(SSL_ERRORSTR()));
            emit q->error(QAbstractSocket::UnknownSocketError);
            return false;
        }

        // Load certificate
        if (!q_SSL_CTX_use_certificate(ctx, (X509 *)configuration.localCertificate.handle())) {
            q->setErrorString(QSslSocket::tr("Error loading local certificate, %1").arg(SSL_ERRORSTR()));
            emit q->error(QAbstractSocket::UnknownSocketError);
            return false;
        }

        // Load private key
        EVP_PKEY *pkey = q_EVP_PKEY_new();
        if (configuration.privateKey.algorithm() == QSsl::Rsa)
            q_EVP_PKEY_assign_RSA(pkey, (RSA *)configuration.privateKey.handle());
        else
            q_EVP_PKEY_assign_DSA(pkey, (DSA *)configuration.privateKey.handle());
        if (!q_SSL_CTX_use_PrivateKey(ctx, pkey)) {
            q->setErrorString(QSslSocket::tr("Error loading private key, %1").arg(SSL_ERRORSTR()));
            emit q->error(QAbstractSocket::UnknownSocketError);
            return false;
        }

        // Check if the certificate matches the private key.
        if (!q_SSL_CTX_check_private_key(ctx)) {
            q->setErrorString(QSslSocket::tr("Private key does not certificate public key, %1").arg(SSL_ERRORSTR()));
            emit q->error(QAbstractSocket::UnknownSocketError);
            return false;
        }
    }

    // Initialize peer verification.
    if (configuration.peerVerifyMode == QSslSocket::VerifyNone) {
        q_SSL_CTX_set_verify(ctx, SSL_VERIFY_NONE, 0);
    } else {
        q_SSL_CTX_set_verify(ctx, SSL_VERIFY_PEER, q_X509Callback);
    }

    // Set verification depth.
    if (configuration.peerVerifyDepth != 0)
        q_SSL_CTX_set_verify_depth(ctx, configuration.peerVerifyDepth);
    
    // Create and initialize SSL session
    if (!(ssl = q_SSL_new(ctx))) {
        // ### Bad error code
        q->setErrorString(QSslSocket::tr("Error creating SSL session, %1").arg(SSL_ERRORSTR()));
        q->setSocketError(QAbstractSocket::UnknownSocketError);
        emit q->error(QAbstractSocket::UnknownSocketError);
        return false;
    }

    // Clear the session.
    q_SSL_clear(ssl);
    errorList.clear();

    // Initialize memory BIOs for encryption and decryption.
    readBio = q_BIO_new(q_BIO_s_mem());
    writeBio = q_BIO_new(q_BIO_s_mem());
    if (!readBio || !writeBio) {
        // ### Bad error code
        q->setErrorString(QSslSocket::tr("Error creating SSL session: %1").arg(SSL_ERRORSTR()));
        q->setSocketError(QAbstractSocket::UnknownSocketError);
        emit q->error(QAbstractSocket::UnknownSocketError);
        return false;
    }

    // Assign the bios.
    q_SSL_set_bio(ssl, readBio, writeBio);

    if (mode == QSslSocket::SslClientMode)
        q_SSL_set_connect_state(ssl);
    else
        q_SSL_set_accept_state(ssl);

    return true;
}

/*!
    \internal
*/
void QSslSocketPrivate::deinitialize()
{
    q_CRYPTO_set_id_callback(0);
    q_CRYPTO_set_locking_callback(0);
}

/*!
    \internal

    Declared static in QSslSocketPrivate, makes sure the SSL libraries have
    been initialized.
*/
bool QSslSocketPrivate::ensureInitialized()
{
    if (!q_resolveOpenSslSymbols())
        return false;

    // Check if the library itself needs to be initialized.
    QMutexLocker locker(openssl_locks()->initLock());
    static int q_initialized = false;
    if (!q_initialized) {
        q_initialized = true;

        // Initialize resources
        initNetworkResources();

        // Initialize OpenSSL.
        q_CRYPTO_set_id_callback(id_function);
        q_CRYPTO_set_locking_callback(locking_function);
        if (q_SSL_library_init() != 1)
            return false;
        q_SSL_load_error_strings();
        q_OpenSSL_add_all_algorithms();

        // Initialize OpenSSL's random seed.
        if (!q_RAND_status()) {
            struct {
                int msec;
                int sec;
                void *stack;
            } randomish;

            int attempts = 500;
            do {
                if (attempts < 500) {
#ifdef Q_OS_UNIX
                    struct timespec ts = {0, 33333333};
                    nanosleep(&ts, 0);
#else
                    Sleep(3);
#endif
                    randomish.msec = attempts;
                }
                randomish.stack = (void *)&randomish;
                randomish.msec = QTime::currentTime().msec();
                randomish.sec = QTime::currentTime().second();
                q_RAND_seed((const char *)&randomish, sizeof(randomish));
            } while (!q_RAND_status() && --attempts);
            if (!attempts)
                return false;
        }

        resetDefaultCiphers();
        setDefaultCaCertificates(systemCaCertificates());
    }
    return true;
}

/*!
    \internal

    Declared static in QSslSocketPrivate, backend-dependent loading of
    application-wide global ciphers.
*/
void QSslSocketPrivate::resetDefaultCiphers()
{
    SSL_CTX *myCtx = q_SSL_CTX_new(q_SSLv23_client_method());
    SSL *mySsl = q_SSL_new(myCtx);

    QList<QSslCipher> ciphers;

    STACK_OF(SSL_CIPHER) *supportedCiphers = q_SSL_get_ciphers(mySsl);
    for (int i = 0; i < q_sk_SSL_CIPHER_num(supportedCiphers); ++i) {
        if (SSL_CIPHER *cipher = q_sk_SSL_CIPHER_value(supportedCiphers, i)) {
            if (cipher->valid) {
                QSslCipher ciph = QSslSocketBackendPrivate::QSslCipher_from_SSL_CIPHER(cipher);
                if (!ciph.isNull()) {
                    if (!ciph.name().toLower().startsWith(QLatin1String("adh")))
                        ciphers << ciph;
                }
            }
        }
    }

    q_SSL_CTX_free(myCtx);
    q_SSL_free(mySsl);

    setDefaultSupportedCiphers(ciphers);
    setDefaultCiphers(ciphers);
}

QList<QSslCertificate> QSslSocketPrivate::systemCaCertificates()
{
#ifdef QQ_OS_UNIX
    // Check known locations for the system's default bundle.  ### On Windows,
    // we should use CAPI to find the bundle, and not rely on default unix
    // locations.
    const char *standardLocations[] = {"/etc/ssl/certs/",
#if 0
                                       // KDE uses KConfig for its SSL store,
                                       // but it also stores the bundle at
                                       // this location
                                       "$HOME/.kde/share/apps/kssl/ca-bundle.crt",
#endif
                                       0};
    const char **it = standardLocations;
    QStringList nameFilter;
    nameFilter << QLatin1String("*.pem") << QLatin1String("*.crt");
    while (*it) {
        if (QDirIterator(QLatin1String(*it), nameFilter).hasNext())
            return certificatesFromPath(QLatin1String(*it));
        ++it;
    }
#endif

    // Qt provides a default bundle when we cannot detect the system's default
    // bundle.
    QFile caBundle(QLatin1String(":/trolltech/network/ssl/qt-ca-bundle.crt"));
    if (caBundle.open(QIODevice::ReadOnly | QIODevice::Text))
        return QSslCertificate::fromDevice(&caBundle);

    // Unreachable; return no bundle.
    return QList<QSslCertificate>();
}

void QSslSocketBackendPrivate::startClientEncryption()
{
    if (!initSslContext()) {
        // ### report error: internal OpenSSL failure
        return;
    }

    // Start connecting. This will place outgoing data in the BIO, so we
    // follow up with calling transmit().
    testConnection();
    transmit();
}

void QSslSocketBackendPrivate::startServerEncryption()
{
    if (!initSslContext()) {
        // ### report error: internal OpenSSL failure
        return;
    }

    // Start connecting. This will place outgoing data in the BIO, so we
    // follow up with calling transmit().
    testConnection();
    transmit();
}

/*!
    \internal

    Transmits encrypted data between the BIOs and the socket.
*/
void QSslSocketBackendPrivate::transmit()
{
    Q_Q(QSslSocket);

    // If we don't have any SSL context, don't bother transmitting.
    if (!ssl)
        return;

    bool transmitting;
    do {
        transmitting = false;
        
        // If the connection is secure, we can transfer data from the write
        // buffer (in plain text) to the write BIO through SSL_write.
        if (connectionEncrypted && !writeBuffer.isEmpty()) {
            qint64 totalBytesWritten = 0;
            int nextDataBlockSize;
            while ((nextDataBlockSize = writeBuffer.nextDataBlockSize()) > 0) {
                int writtenBytes = q_SSL_write(ssl, writeBuffer.readPointer(), nextDataBlockSize);
                if (writtenBytes <= 0) {
                    // ### Better error handling.
                    q->setErrorString(QSslSocket::tr("Unable to write data: %1").arg(SSL_ERRORSTR()));
                    q->setSocketError(QAbstractSocket::UnknownSocketError);
                    emit q->error(QAbstractSocket::UnknownSocketError);
                    return;
                }
#ifdef QSSLSOCKET_DEBUG
                qDebug() << "QSslSocketBackendPrivate::transmit: encrypted" << writtenBytes << "bytes";
#endif
                writeBuffer.free(writtenBytes);
                totalBytesWritten += writtenBytes;
            }

            if (totalBytesWritten > 0) {
                // Don't emit bytesWritten() recursively.
                if (!emittedBytesWritten) {
                    emittedBytesWritten = true;
                    emit q->bytesWritten(totalBytesWritten);
                    emittedBytesWritten = false;
                }
            }
        }

        // Check if we've got any data to be written to the socket.
        QVarLengthArray<char, 4096> data;
        int pendingBytes;
        while (plainSocket->isValid() && (pendingBytes = q_BIO_pending(writeBio)) > 0) {
            // Read encrypted data from the write BIO into a buffer.
            data.resize(pendingBytes);
            int encryptedBytesRead = q_BIO_read(writeBio, data.data(), pendingBytes);

            // Write encrypted data from the buffer to the socket.
            plainSocket->write(data.constData(), encryptedBytesRead);
#ifdef QSSLSOCKET_DEBUG
            qDebug() << "QSslSocketBackendPrivate::transmit: wrote" << encryptedBytesRead << "encrypted bytes to the socket";
#endif
            transmitting = true;
        }

        // Check if we've got any data to be read from the socket.
        if (!connectionEncrypted || !readBufferMaxSize || readBuffer.size() < readBufferMaxSize)
            while ((pendingBytes = plainSocket->bytesAvailable()) > 0) {
                // Read encrypted data from the socket into a buffer.
                data.resize(pendingBytes);
                int decryptedBytesRead = plainSocket->read(data.data(), pendingBytes);
#ifdef QSSLSOCKET_DEBUG
                qDebug() << "QSslSocketBackendPrivate::transmit: read" << decryptedBytesRead << "encrypted bytes from the socket";
#endif
                // Write encrypted data from the buffer into the read BIO.
                q_BIO_write(readBio, data.constData(), decryptedBytesRead);
                transmitting = true;
            }

        // If the connection isn't secured yet, this is the time to retry the
        // connect / accept.
        if (!connectionEncrypted) {
#ifdef QSSLSOCKET_DEBUG
            qDebug() << "QSslSocketBackendPrivate::transmit: testing encryption";
#endif
            if (testConnection()) {
#ifdef QSSLSOCKET_DEBUG
                qDebug() << "QSslSocketBackendPrivate::transmit: encryption established";
#endif
                connectionEncrypted = true;
                transmitting = true;
            } else if (plainSocket->state() != QAbstractSocket::ConnectedState) {
#ifdef QSSLSOCKET_DEBUG
                qDebug() << "QSslSocketBackendPrivate::transmit: connection lost";
#endif
                break;
            } else {
#ifdef QSSLSOCKET_DEBUG
                qDebug() << "QSslSocketBackendPrivate::transmit: encryption not done yet";
#endif
            }
        }

        // If the request is small and the remote host closes the transmission
        // after sending, there's a chance that testConnection() will already
        // have triggered a shutdown.
        if (!ssl)
            continue;

        // We always read everything from the SSL decryption buffers, even if
        // we have a readBufferMaxSize. There's no point in leaving data there
        // just so that readBuffer.size() == readBufferMaxSize.
        int readBytes = 0;
        data.resize(4096);
        ::memset(data.data(), 0, data.size());
        do {
            // Don't use SSL_pending(). It's very unreliable.
            if ((readBytes = q_SSL_read(ssl, data.data(), data.size())) > 0) {
#ifdef QSSLSOCKET_DEBUG
                qDebug() << "QSslSocketBackendPrivate::transmit: decrypted" << readBytes << "bytes";
#endif
                char *ptr = readBuffer.reserve(readBytes);
                ::memcpy(ptr, data.data(), readBytes);

                if (readyReadEmittedPointer)
                    *readyReadEmittedPointer = true;
                emit q->readyRead();
                transmitting = true;
                continue;
            }

            // Error.
            switch (q_SSL_get_error(ssl, readBytes)) {
            case SSL_ERROR_WANT_READ:
            case SSL_ERROR_WANT_WRITE:
                // Out of data.
                break;
            case SSL_ERROR_ZERO_RETURN:
                // The remote host closed the connection.
#ifdef QSSLSOCKET_DEBUG
                qDebug() << "QSslSocketBackendPrivate::transmit: remote disconnect";
#endif
                plainSocket->disconnectFromHost();
                break;
            default:
                // ### Handle errors better.
                q->setErrorString(QSslSocket::tr("Error while reading: %1").arg(SSL_ERRORSTR()));
                q->setSocketError(QAbstractSocket::UnknownSocketError);
                emit q->error(QAbstractSocket::UnknownSocketError);
                break;
            }
        } while (ssl && readBytes > 0);
    } while (ssl && ctx && transmitting);
}

static QSslError _q_OpenSSL_to_QSslError(int errorCode, const QSslCertificate &cert)
{
    QSslError error;
    switch (errorCode) {
    case X509_V_OK:
        // X509_V_OK is also reported if the peer had no certificate.
        break;
    case X509_V_ERR_UNABLE_TO_GET_ISSUER_CERT:
        error = QSslError(QSslError::UnableToGetIssuerCertificate, cert); break;
    case X509_V_ERR_UNABLE_TO_DECRYPT_CERT_SIGNATURE:
        error = QSslError(QSslError::UnableToDecryptCertificateSignature, cert); break;
    case X509_V_ERR_UNABLE_TO_DECODE_ISSUER_PUBLIC_KEY:
        error = QSslError(QSslError::UnableToDecodeIssuerPublicKey, cert); break;
    case X509_V_ERR_CERT_SIGNATURE_FAILURE:
        error = QSslError(QSslError::CertificateSignatureFailed, cert); break;
    case X509_V_ERR_CERT_NOT_YET_VALID:
        error = QSslError(QSslError::CertificateNotYetValid, cert); break;
    case X509_V_ERR_CERT_HAS_EXPIRED:
        error = QSslError(QSslError::CertificateExpired, cert); break;
    case X509_V_ERR_ERROR_IN_CERT_NOT_BEFORE_FIELD:
        error = QSslError(QSslError::InvalidNotBeforeField, cert); break;
    case X509_V_ERR_ERROR_IN_CERT_NOT_AFTER_FIELD:
        error = QSslError(QSslError::InvalidNotAfterField, cert); break;
    case X509_V_ERR_DEPTH_ZERO_SELF_SIGNED_CERT:
        error = QSslError(QSslError::SelfSignedCertificate, cert); break;
    case X509_V_ERR_SELF_SIGNED_CERT_IN_CHAIN:
        error = QSslError(QSslError::SelfSignedCertificateInChain, cert); break;
    case X509_V_ERR_UNABLE_TO_GET_ISSUER_CERT_LOCALLY:
        error = QSslError(QSslError::UnableToGetLocalIssuerCertificate, cert); break;
    case X509_V_ERR_UNABLE_TO_VERIFY_LEAF_SIGNATURE:
        error = QSslError(QSslError::UnableToVerifyFirstCertificate, cert); break;
    case X509_V_ERR_CERT_REVOKED:
        error = QSslError(QSslError::CertificateRevoked, cert); break;
    case X509_V_ERR_INVALID_CA:
        error = QSslError(QSslError::InvalidCaCertificate, cert); break;
    case X509_V_ERR_PATH_LENGTH_EXCEEDED:
        error = QSslError(QSslError::PathLengthExceeded, cert); break;
    case X509_V_ERR_INVALID_PURPOSE:
        error = QSslError(QSslError::InvalidPurpose, cert); break;
    case X509_V_ERR_CERT_UNTRUSTED:
        error = QSslError(QSslError::CertificateUntrusted, cert); break;
    case X509_V_ERR_CERT_REJECTED:
        error = QSslError(QSslError::CertificateRejected, cert); break;
    default:
        error = QSslError(QSslError::UnspecifiedError, cert); break;
    }
    return error;
}

bool QSslSocketBackendPrivate::testConnection()
{
    Q_Q(QSslSocket);

    // Check if the connection has been established. Get all errors from the
    // verification stage.
    _q_sslErrorList()->mutex.lock();
    _q_sslErrorList()->errors.clear();
    int result = (mode == QSslSocket::SslClientMode) ? q_SSL_connect(ssl) : q_SSL_accept(ssl);

    const QList<QPair<int, int> > &lastErrors = _q_sslErrorList()->errors;
    for (int i = 0; i < lastErrors.size(); ++i) {
        const QPair<int, int> &currentError = lastErrors.at(i);
        // Initialize the peer certificate chain in order to find which certificate caused this error
        if (configuration.peerCertificateChain.isEmpty())
            configuration.peerCertificateChain = STACKOFX509_to_QSslCertificates(q_SSL_get_peer_cert_chain(ssl));
        emit q->peerVerifyError(_q_OpenSSL_to_QSslError(currentError.first,
                                configuration.peerCertificateChain.value(currentError.second)));
        if (q->state() != QAbstractSocket::ConnectedState)
            break;
    }

    errorList << lastErrors;
    _q_sslErrorList()->mutex.unlock();

    // Connection aborted during handshake phase.
    if (q->state() != QAbstractSocket::ConnectedState)
        return false;

    // Check if we're encrypted or not.
    if (result <= 0) {
        switch (q_SSL_get_error(ssl, result)) {
        case SSL_ERROR_WANT_READ:
        case SSL_ERROR_WANT_WRITE:
            // The handshake is not yet complete.
            break;
        default:
            // ### Handle errors better
            q->setErrorString(QSslSocket::tr("Error during SSL handshake: %1").arg(SSL_ERRORSTR()));
            q->setSocketError(QAbstractSocket::SslHandshakeFailedError);
#ifdef QSSLSOCKET_DEBUG
            qDebug() << "QSslSocketBackendPrivate::testConnection: error!" << q->errorString();
#endif
            emit q->error(QAbstractSocket::SslHandshakeFailedError);
            q->abort();
        }
        return false;
    }

    // Store the peer certificate and chain. For clients, the peer certificate
    // chain includes the peer certificate; for servers, it doesn't. Both the
    // peer certificate and the chain may be empty if the peer didn't present
    // any certificate.
    if (configuration.peerCertificateChain.isEmpty())
        configuration.peerCertificateChain = STACKOFX509_to_QSslCertificates(q_SSL_get_peer_cert_chain(ssl));
    X509 *x509 = q_SSL_get_peer_certificate(ssl);
    configuration.peerCertificate = QSslCertificatePrivate::QSslCertificate_from_X509(x509);
    q_X509_free(x509);

    // Start translating errors.
    QList<QSslError> errors;
    bool doVerifyPeer = configuration.peerVerifyMode == QSslSocket::VerifyPeer
                        || (configuration.peerVerifyMode == QSslSocket::AutoVerifyPeer
                            && mode == QSslSocket::SslClientMode);

    // Check the peer certificate itself. First try the subject's common name
    // (CN) as a wildcard, then try all alternate subject name DNS entries the
    // same way.
    if (!configuration.peerCertificate.isNull()) {
        // but only if we're a client connecting to a server
        // if we're the server, don't check CN
        if (mode == QSslSocket::SslClientMode) {
            QString peerName = q->peerName();
            QString commonName = configuration.peerCertificate.subjectInfo(QSslCertificate::CommonName);

            QRegExp regexp(commonName, Qt::CaseInsensitive, QRegExp::Wildcard);
            if (!regexp.exactMatch(peerName)) {
                bool matched = false;
                foreach (QString altName, configuration.peerCertificate
                         .alternateSubjectNames().values(QSsl::DnsEntry)) {
                    regexp.setPattern(altName);
                    if (regexp.exactMatch(peerName)) {
                        matched = true;
                        break;
                    }
                }
                if (!matched) {
                    // No matches in common names or alternate names.
                    QSslError error(QSslError::HostNameMismatch, configuration.peerCertificate);
                    errors << error;
                    emit q->peerVerifyError(error);
                    if (q->state() != QAbstractSocket::ConnectedState)
                        return false;
                }
            }
        }
    } else {
        // No peer certificate presented. Report as error if the socket
        // expected one.
        if (doVerifyPeer) {
            QSslError error(QSslError::NoPeerCertificate);
            errors << error;
            emit q->peerVerifyError(error);
            if (q->state() != QAbstractSocket::ConnectedState)
                return false;
        }
    }

    // Translate errors from the error list into QSslErrors.
    for (int i = 0; i < errorList.size(); ++i) {
        const QPair<int, int> &errorAndDepth = errorList.at(i);
        int err = errorAndDepth.first;
        int depth = errorAndDepth.second;
        errors << _q_OpenSSL_to_QSslError(err, configuration.peerCertificateChain.value(depth));
    }

    if (!errors.isEmpty()) {
        sslErrors = errors;
        emit q->sslErrors(errors);
        if (doVerifyPeer && !ignoreSslErrors) {
            q->setErrorString(sslErrors.first().errorString());
            q->setSocketError(QAbstractSocket::SslHandshakeFailedError);
            emit q->error(QAbstractSocket::SslHandshakeFailedError);
            plainSocket->disconnectFromHost();
            return false;
        }
    } else {
        sslErrors.clear();
    }

    // if we have a max read buffer size, reset the plain socket's to 1k
    if (readBufferMaxSize)
        plainSocket->setReadBufferSize(1024);

    connectionEncrypted = true;
    emit q->encrypted();
    if (autoStartHandshake && pendingClose) {
        pendingClose = false;
        q->disconnectFromHost();
    }
    return true;
}

void QSslSocketBackendPrivate::disconnectFromHost()
{
    if (ssl) {
        q_SSL_shutdown(ssl);
        transmit();
    }
    plainSocket->disconnectFromHost();
}

void QSslSocketBackendPrivate::disconnected()
{
    if (ssl) {
        q_SSL_free(ssl);
        ssl = 0;
    }
    if (ctx) {
        q_SSL_CTX_free(ctx);
        ctx = 0;
    }
}

QSslCipher QSslSocketBackendPrivate::sessionCipher() const
{
    if (!ssl || !ctx)
        return QSslCipher();
    SSL_CIPHER *sessionCipher = q_SSL_get_current_cipher(ssl);
    return sessionCipher ? QSslCipher_from_SSL_CIPHER(sessionCipher) : QSslCipher();
}

QList<QSslCertificate> QSslSocketBackendPrivate::STACKOFX509_to_QSslCertificates(STACK_OF(X509) *x509)
{
    ensureInitialized();
    QList<QSslCertificate> certificates;
    for (int i = 0; i < q_sk_X509_num(x509); ++i) {
        if (X509 *entry = q_sk_X509_value(x509, i))
            certificates << QSslCertificatePrivate::QSslCertificate_from_X509(entry);
    }
    return certificates;
}

QT_END_NAMESPACE
