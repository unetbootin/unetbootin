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

#include "qhttpnetworkconnection_p.h"
#include <private/qnetworkrequest_p.h>
#include <private/qobject_p.h>
#include <private/qauthenticator_p.h>
#include <qnetworkproxy.h>
#include <qauthenticator.h>
#include <qbytearraymatcher.h>
#include <qbuffer.h>
#include <qpair.h>
#include <qhttp.h>
#include <qdebug.h>

#ifndef QT_NO_HTTP

#ifndef QT_NO_OPENSSL
#    include <QtNetwork/qsslkey.h>
#    include <QtNetwork/qsslcipher.h>
#    include <QtNetwork/qsslconfiguration.h>
#endif

#ifndef QT_NO_COMPRESS
#    include <zlib.h>
static const unsigned char gz_magic[2] = {0x1f, 0x8b}; // gzip magic header
// gzip flag byte
#define HEAD_CRC     0x02 // bit 1 set: header CRC present
#define EXTRA_FIELD  0x04 // bit 2 set: extra field present
#define ORIG_NAME    0x08 // bit 3 set: original file name present
#define COMMENT      0x10 // bit 4 set: file comment present
#define RESERVED     0xE0 // bits 5..7: reserved
#define CHUNK 16384
#endif

QT_BEGIN_NAMESPACE

class QHttpNetworkHeaderPrivate : public QSharedData
{
public:
    QUrl url;
    QList<QPair<QByteArray, QByteArray> > fields;

    QHttpNetworkHeaderPrivate(const QUrl &newUrl = QUrl());
    QHttpNetworkHeaderPrivate(const QHttpNetworkHeaderPrivate &other);
    inline qint64 contentLength() const;
    inline void setContentLength(qint64 length);

    inline QByteArray headerField(const QByteArray &name, const QByteArray &defaultValue = QByteArray()) const;
    inline QList<QByteArray> headerFieldValues(const QByteArray &name) const;
    inline void setHeaderField(const QByteArray &name, const QByteArray &data);
    bool operator==(const QHttpNetworkHeaderPrivate &other) const;

};

QHttpNetworkHeaderPrivate::QHttpNetworkHeaderPrivate(const QUrl &newUrl)
    :url(newUrl)
{
}

QHttpNetworkHeaderPrivate::QHttpNetworkHeaderPrivate(const QHttpNetworkHeaderPrivate &other)
    :QSharedData(other)
{
    url = other.url;
    fields = other.fields;
}

qint64 QHttpNetworkHeaderPrivate::contentLength() const
{
    bool ok = false;
    QByteArray value = headerField("content-length");
    qint64 length = value.toULongLong(&ok);
    if (ok)
        return length;
    return -1; // the header field is not set
}

void QHttpNetworkHeaderPrivate::setContentLength(qint64 length)
{
    setHeaderField("Content-Length", QByteArray::number(length));
}

QByteArray QHttpNetworkHeaderPrivate::headerField(const QByteArray &name, const QByteArray &defaultValue) const
{
    QList<QByteArray> allValues = headerFieldValues(name);
    if (allValues.isEmpty())
        return defaultValue;

    QByteArray result;
    bool first = true;
    foreach (QByteArray value, allValues) {
        if (!first)
            result += ", ";
        first = false;
        result += value;
    }
    return result;
}

QList<QByteArray> QHttpNetworkHeaderPrivate::headerFieldValues(const QByteArray &name) const
{
    QList<QByteArray> result;
    QByteArray lowerName = name.toLower();
    QList<QPair<QByteArray, QByteArray> >::ConstIterator it = fields.constBegin(),
                                                        end = fields.constEnd();
    for ( ; it != end; ++it)
        if (lowerName == it->first.toLower())
            result += it->second;

    return result;
}

void QHttpNetworkHeaderPrivate::setHeaderField(const QByteArray &name, const QByteArray &data)
{
    QByteArray lowerName = name.toLower();
    QList<QPair<QByteArray, QByteArray> >::Iterator it = fields.begin();
    while (it != fields.end()) {
        if (lowerName == it->first.toLower())
            it = fields.erase(it);
        else
            ++it;
    }
    fields.append(qMakePair(name, data));
}

bool QHttpNetworkHeaderPrivate::operator==(const QHttpNetworkHeaderPrivate &other) const
{
   return (url == other.url);
}

// QHttpNetworkRequestPrivate
class QHttpNetworkRequestPrivate : public QHttpNetworkHeaderPrivate
{
public:
    QHttpNetworkRequestPrivate(QHttpNetworkRequest::Operation op,
        QHttpNetworkRequest::Priority pri, const QUrl &newUrl = QUrl());
    QHttpNetworkRequestPrivate(const QHttpNetworkRequestPrivate &other);
    ~QHttpNetworkRequestPrivate();
    bool operator==(const QHttpNetworkRequestPrivate &other) const;
    QByteArray methodName() const;
    QByteArray uri(bool throughProxy) const;

    static QByteArray header(const QHttpNetworkRequest &request, bool throughProxy);

    QHttpNetworkRequest::Operation operation;
    QHttpNetworkRequest::Priority priority;
    mutable QIODevice *data;
    bool autoDecompress;
};

QHttpNetworkRequestPrivate::QHttpNetworkRequestPrivate(QHttpNetworkRequest::Operation op,
        QHttpNetworkRequest::Priority pri, const QUrl &newUrl)
    : QHttpNetworkHeaderPrivate(newUrl), operation(op), priority(pri), data(0),
      autoDecompress(false)
{
}

QHttpNetworkRequestPrivate::QHttpNetworkRequestPrivate(const QHttpNetworkRequestPrivate &other)
    : QHttpNetworkHeaderPrivate(other)
{
    operation = other.operation;
    priority = other.priority;
    data = other.data;
    autoDecompress = other.autoDecompress;
}

QHttpNetworkRequestPrivate::~QHttpNetworkRequestPrivate()
{
}

bool QHttpNetworkRequestPrivate::operator==(const QHttpNetworkRequestPrivate &other) const
{
    return QHttpNetworkHeaderPrivate::operator==(other)
        && (operation == other.operation)
        && (data == other.data);
}

QByteArray QHttpNetworkRequestPrivate::methodName() const
{
    QByteArray ba;
    switch (operation) {
    case QHttpNetworkRequest::Options:
        ba += "OPTIONS";
        break;
    case QHttpNetworkRequest::Get:
        ba += "GET";
        break;
    case QHttpNetworkRequest::Head:
        ba += "HEAD";
        break;
    case QHttpNetworkRequest::Post:
        ba += "POST";
        break;
    case QHttpNetworkRequest::Put:
        ba += "PUT";
        break;
    case QHttpNetworkRequest::Delete:
        ba += "DELETE";
        break;
    case QHttpNetworkRequest::Trace:
        ba += "TRACE";
        break;
    case QHttpNetworkRequest::Connect:
        ba += "CONNECT";
        break;
    default:
        break;
    }
    return ba;
}

QByteArray QHttpNetworkRequestPrivate::uri(bool throughProxy) const
{
    QUrl::FormattingOptions format(QUrl::RemoveFragment);

    // for POST, query data is send as content
    if (operation == QHttpNetworkRequest::Post && !data)
        format |= QUrl::RemoveQuery;
    // for requests through proxy, the Request-URI contains full url
    if (throughProxy)
        format |= QUrl::RemoveUserInfo;
    else
        format |= QUrl::RemoveScheme | QUrl::RemoveAuthority;
    QByteArray uri = url.toEncoded(format);
    if (uri.isEmpty() || (throughProxy && url.path().isEmpty()))
        uri += '/';
    return uri;
}

QByteArray QHttpNetworkRequestPrivate::header(const QHttpNetworkRequest &request, bool throughProxy)
{
    QByteArray ba = request.d->methodName();
    QByteArray uri = request.d->uri(throughProxy);
    ba += " " + uri;

    QString majorVersion = QString::number(request.majorVersion());
    QString minorVersion = QString::number(request.minorVersion());
    ba += " HTTP/" + majorVersion.toLatin1() + "." + minorVersion.toLatin1() + "\r\n";

    QList<QPair<QByteArray, QByteArray> > fields = request.header();
    QList<QPair<QByteArray, QByteArray> >::const_iterator it = fields.constBegin();
    for (; it != fields.constEnd(); ++it)
        ba += it->first + ": " + it->second + "\r\n";
    if (request.d->operation == QHttpNetworkRequest::Post) {
        // add content type, if not set in the request
        if (request.headerField("content-type").isEmpty())
            ba += "Content-Type: application/x-www-form-urlencoded\r\n";
        if (!request.d->data && request.d->url.hasQuery()) {
            QByteArray query = request.d->url.encodedQuery();
            ba += "Content-Length: "+ QByteArray::number(query.size()) + "\r\n";
            ba += "\r\n";
            ba += query;
        } else {
            ba += "\r\n";
        }
    } else {
        ba += "\r\n";
    }
     return ba;
}

class QHttpNetworkReplyPrivate : public QObjectPrivate, public QHttpNetworkHeaderPrivate
{
public:
    QHttpNetworkReplyPrivate(const QUrl &newUrl = QUrl());
    ~QHttpNetworkReplyPrivate();
    qint64 readStatus(QAbstractSocket *socket);
    void parseStatus(const QByteArray &status);
    qint64 readHeader(QAbstractSocket *socket);
    void parseHeader(const QByteArray &header);
    qint64 readBody(QAbstractSocket *socket, QIODevice *out);
    bool findChallenge(bool forProxy, QByteArray &challenge) const;
    QAuthenticatorPrivate::Method authenticationMethod(bool isProxy) const;
    void clear();

    qint64 transferRaw(QIODevice *in, QIODevice *out, qint64 size);
    qint64 transferChunked(QIODevice *in, QIODevice *out);
    qint64 getChunkSize(QIODevice *in, qint64 *chunkSize);

    qint64 bytesAvailable() const;
    bool isChunked();
    bool connectionCloseEnabled();
    bool isGzipped();
#ifndef QT_NO_COMPRESS
    bool gzipCheckHeader(QByteArray &content, int &pos);
    int gunzipBodyPartially(QByteArray &compressed, QByteArray &inflated);
#endif
    void removeAutoDecompressHeader();

    enum ReplyState {
        NothingDoneState,
        ReadingStatusState,
        ReadingHeaderState,
        ReadingDataState,
        AllDoneState
    } state;

    QHttpNetworkRequest request;
    int statusCode;
    int majorVersion;
    int minorVersion;
    QString errorString;
    QString reasonPhrase;
    qint64 bodyLength;
    qint64 contentRead;
    qint64 totalProgress;
    QByteArray fragment;
    qint64 currentChunkSize;
    qint64 currentChunkRead;
    QPointer<QHttpNetworkConnection> connection;
    bool initInflate;
    bool streamEnd;
#ifndef QT_NO_COMPRESS
    z_stream inflateStrm;
#endif
    bool autoDecompress;

    QByteArray responseData; // uncompressed body
    QByteArray compressedData; // compressed body (temporary)
    QBuffer requestDataBuffer;
    bool requestIsBuffering;
    bool requestIsPrepared;
};

QHttpNetworkReplyPrivate::QHttpNetworkReplyPrivate(const QUrl &newUrl)
    : QHttpNetworkHeaderPrivate(newUrl), state(NothingDoneState), statusCode(100),
      majorVersion(0), minorVersion(0), bodyLength(0), contentRead(0), totalProgress(0),
      currentChunkSize(0), currentChunkRead(0), connection(0), initInflate(false),
      autoDecompress(false), requestIsBuffering(false), requestIsPrepared(false)
{
}

QHttpNetworkReplyPrivate::~QHttpNetworkReplyPrivate()
{
}

void QHttpNetworkReplyPrivate::clear()
{
    state = NothingDoneState;
    statusCode = 100;
    bodyLength = 0;
    contentRead = 0;
    totalProgress = 0;
    currentChunkSize = 0;
    currentChunkRead = 0;
    connection = 0;
#ifndef QT_NO_COMPRESS
    if (initInflate)
        inflateEnd(&inflateStrm);
#endif
    initInflate = false;
    streamEnd = false;
    autoDecompress = false;
    fields.clear();
}

// QHttpNetworkReplyPrivate
qint64 QHttpNetworkReplyPrivate::bytesAvailable() const
{
    return (state != ReadingDataState ? 0 : fragment.size());
}

bool QHttpNetworkReplyPrivate::isGzipped()
{
    QByteArray encoding = headerField("content-encoding");
    return encoding.toLower() == "gzip";
}

void QHttpNetworkReplyPrivate::removeAutoDecompressHeader()
{
    // The header "Content-Encoding  = gzip" is retained.
    // Content-Length is removed since the actual one send by the server is for compressed data
    QByteArray name("content-length");
    QByteArray lowerName = name.toLower();
    QList<QPair<QByteArray, QByteArray> >::Iterator it = fields.begin(),
                                                   end = fields.end();
    while (it != end) {
        if (name == it->first.toLower()) {
            fields.erase(it);
            break;
        }
        ++it;
    }

}

bool QHttpNetworkReplyPrivate::findChallenge(bool forProxy, QByteArray &challenge) const
{
    challenge.clear();
    // find out the type of authentication protocol requested.
    QByteArray header = forProxy ? "proxy-authenticate" : "www-authenticate";
    // pick the best protocol (has to match parsing in QAuthenticatorPrivate)
    QList<QByteArray> challenges = headerFieldValues(header);
    for (int i = 0; i<challenges.size(); i++) {
        QByteArray line = challenges.at(i);
        if (!line.toLower().startsWith("negotiate"))
            challenge = line;
    }
    return !challenge.isEmpty();
}

QAuthenticatorPrivate::Method QHttpNetworkReplyPrivate::authenticationMethod(bool isProxy) const
{
    // The logic is same as the one used in void QAuthenticatorPrivate::parseHttpResponse()
    QAuthenticatorPrivate::Method method = QAuthenticatorPrivate::None;
    QByteArray header = isProxy ? "proxy-authenticate" : "www-authenticate";
    QList<QByteArray> challenges = headerFieldValues(header);
    for (int i = 0; i<challenges.size(); i++) {
        QByteArray line = challenges.at(i).trimmed().toLower();
        if (method < QAuthenticatorPrivate::Basic
            && line.startsWith("basic")) {
            method = QAuthenticatorPrivate::Basic;
        } else if (method < QAuthenticatorPrivate::Ntlm
            && line.startsWith("ntlm")) {
            method = QAuthenticatorPrivate::Ntlm;
        } else if (method < QAuthenticatorPrivate::DigestMd5
            && line.startsWith("digest")) {
            method = QAuthenticatorPrivate::DigestMd5;
        }
    }
    return method;
}

#ifndef QT_NO_COMPRESS
bool QHttpNetworkReplyPrivate::gzipCheckHeader(QByteArray &content, int &pos)
{
    int method = 0; // method byte
    int flags = 0;  // flags byte
    bool ret = false;

    // Assure two bytes in the buffer so we can peek ahead -- handle case
    // where first byte of header is at the end of the buffer after the last
    // gzip segment
    pos = -1;
    QByteArray &body = content;
    int maxPos = body.size()-1;
    if (maxPos < 1) {
        return ret;
    }

    // Peek ahead to check the gzip magic header
    if (body[0] != char(gz_magic[0]) ||
        body[1] != char(gz_magic[1])) {
        return ret;
    }
    pos += 2;
    // Check the rest of the gzip header
    if (++pos <= maxPos)
        method = body[pos];
    if (pos++ <= maxPos)
        flags = body[pos];
    if (method != Z_DEFLATED || (flags & RESERVED) != 0) {
        return ret;
    }

    // Discard time, xflags and OS code:
    pos += 6;
    if (pos > maxPos)
        return ret;
    if ((flags & EXTRA_FIELD) && ((pos+2) <= maxPos)) { // skip the extra field
        unsigned len =  (unsigned)body[++pos];
        len += ((unsigned)body[++pos])<<8;
        pos += len;
        if (pos > maxPos)
            return ret;
    }
    if ((flags & ORIG_NAME) != 0) { // skip the original file name
        while(++pos <= maxPos && body[pos]) {}
    }
    if ((flags & COMMENT) != 0) {   // skip the .gz file comment
        while(++pos <= maxPos && body[pos]) {}
    }
    if ((flags & HEAD_CRC) != 0) {  // skip the header crc
        pos += 2;
        if (pos > maxPos)
            return ret;
    }
    ret = (pos < maxPos); // return failed, if no more bytes left
    return ret;
}

int QHttpNetworkReplyPrivate::gunzipBodyPartially(QByteArray &compressed, QByteArray &inflated)
{
    int ret = Z_DATA_ERROR;
    unsigned have;
    unsigned char out[CHUNK];
    int pos = -1;

    if (!initInflate) {
        // check the header
        if (!gzipCheckHeader(compressed, pos))
            return ret;
        // allocate inflate state
        inflateStrm.zalloc = Z_NULL;
        inflateStrm.zfree = Z_NULL;
        inflateStrm.opaque = Z_NULL;
        inflateStrm.avail_in = 0;
        inflateStrm.next_in = Z_NULL;
        ret = inflateInit2(&inflateStrm, -MAX_WBITS);
        if (ret != Z_OK)
            return ret;
        initInflate = true;
        streamEnd = false;
    }

    //remove the header.
    compressed.remove(0, pos+1);
    // expand until deflate stream ends
    inflateStrm.next_in = (unsigned char *)compressed.data();
    inflateStrm.avail_in = compressed.size();
    do {
        inflateStrm.avail_out = sizeof(out);
        inflateStrm.next_out = out;
        ret = inflate(&inflateStrm, Z_NO_FLUSH);
        switch (ret) {
        case Z_NEED_DICT:
            ret = Z_DATA_ERROR;
            // and fall through
        case Z_DATA_ERROR:
        case Z_MEM_ERROR:
            inflateEnd(&inflateStrm);
            initInflate = false;
            return ret;
        }
        have = sizeof(out) - inflateStrm.avail_out;
        inflated.append(QByteArray((const char *)out, have));
     } while (inflateStrm.avail_out == 0);
    // clean up and return
    if (ret <= Z_ERRNO || ret == Z_STREAM_END) {
        inflateEnd(&inflateStrm);
        initInflate = false;
    }
    streamEnd = (ret == Z_STREAM_END);
    return ret;
}
#endif

qint64 QHttpNetworkReplyPrivate::readStatus(QAbstractSocket *socket)
{
    qint64 bytes = 0;
    char c;

    while (socket->bytesAvailable()) {
        // allow both CRLF & LF (only) line endings
        if (socket->peek(&c, 1) == 1 && c == '\n') {
            bytes += socket->read(&c, 1); // read the "n"
            // remove the CR at the end
            if (fragment.endsWith('\r')) {
                fragment.truncate(fragment.length()-1);
            }
            parseStatus(fragment);
            state = ReadingHeaderState;
            fragment.clear(); // next fragment
            break;
        } else {
            c = 0;
            bytes += socket->read(&c, 1);
            fragment.append(c);
        }
    }
    return bytes;
}

void QHttpNetworkReplyPrivate::parseStatus(const QByteArray &status)
{
    const QByteArrayMatcher sp(" ");
    int i = sp.indexIn(status);
    const QByteArray version = status.mid(0, i);
    int j = sp.indexIn(status, i + 1);
    const QByteArray code = status.mid(i + 1, j - i - 1);
    const QByteArray reason = status.mid(j + 1, status.count() - j);

    const QByteArrayMatcher slash("/");
    int k = slash.indexIn(version);
    const QByteArrayMatcher dot(".");
    int l = dot.indexIn(version, k);
    const QByteArray major = version.mid(k + 1, l - k - 1);
    const QByteArray minor = version.mid(l + 1, version.count() - l);

    majorVersion = QString::fromAscii(major.constData()).toInt();
    minorVersion = QString::fromAscii(minor.constData()).toInt();
    statusCode = QString::fromAscii(code.constData()).toInt();
    reasonPhrase = QString::fromAscii(reason.constData());
}

qint64 QHttpNetworkReplyPrivate::readHeader(QAbstractSocket *socket)
{
    qint64 bytes = 0;
    char crlfcrlf[5];
    crlfcrlf[4] = '\0';
    char c = 0;
    bool allHeaders = false;
    while (!allHeaders && socket->bytesAvailable()) {
        if (socket->peek(&c, 1) == 1 && c == '\n') {
            // check for possible header endings. As per HTTP rfc,
            // the header endings will be marked by CRLFCRLF. But
            // we will allow CRLFLF, LFLF & CRLFCRLF
            if (fragment.endsWith("\n\r") || fragment.endsWith('\n'))
                allHeaders = true;
        }
        bytes += socket->read(&c, 1);
        fragment.append(c);
    }
    // we received all headers now parse them
    if (allHeaders) {
        parseHeader(fragment);
        state = ReadingDataState;
        fragment.clear(); // next fragment
        bodyLength = contentLength(); // cache the length
    }
    return bytes;
}

void QHttpNetworkReplyPrivate::parseHeader(const QByteArray &header)
{
    // see rfc2616, sec 4 for information about HTTP/1.1 headers.
    // allows relaxed parsing here, accepts both CRLF & LF line endings
    const QByteArrayMatcher lf("\n");
    const QByteArrayMatcher colon(":");
    int i = 0;
    while (i < header.count()) {
        int j = colon.indexIn(header, i); // field-name
        if (j == -1)
            break;
        const QByteArray field = header.mid(i, j - i).trimmed();
        j++;
        // any number of LWS is allowed before and after the value
        QByteArray value;
        do {
            i = lf.indexIn(header, j);
            if (i == -1)
                break;
            if (!value.isEmpty())
                value += ' ';
            // check if we have CRLF or only LF
            bool hasCR = (i && header[i-1] == '\r');
            int length = i -(hasCR ? 1: 0) - j;
            value += header.mid(j, length).trimmed();
            j = ++i;
        } while (i < header.count() && (header.at(i) == ' ' || header.at(i) == '\t'));
        if (i == -1)
            break; // something is wrong

        fields.append(qMakePair(field, value));
    }
}

bool QHttpNetworkReplyPrivate::isChunked()
{
    return headerField("transfer-encoding").toLower().contains("chunked");
}

bool QHttpNetworkReplyPrivate::connectionCloseEnabled()
{
    return (headerField("connection").toLower().contains("close") ||
            headerField("proxy-connection").toLower().contains("close"));
}

qint64 QHttpNetworkReplyPrivate::readBody(QAbstractSocket *socket, QIODevice *out)
{
    qint64 bytes = 0;
    if (isChunked()) {
        bytes += transferChunked(socket, out); // chunked transfer encoding (rfc 2616, sec 3.6)
    } else if (bodyLength > 0) { // we have a Content-Length
        bytes += transferRaw(socket, out, bodyLength - contentRead);
        if (contentRead + bytes == bodyLength)
            state = AllDoneState;
    } else {
        bytes += transferRaw(socket, out, socket->bytesAvailable());
    }
    if (state == AllDoneState)
        socket->readAll(); // Read the rest to clean (CRLF)
    contentRead += bytes;
    return bytes;
}

qint64 QHttpNetworkReplyPrivate::transferRaw(QIODevice *in, QIODevice *out, qint64 size)
{
    qint64 bytes = 0;
    Q_ASSERT(in);
    Q_ASSERT(out);

    int toBeRead = qMin<qint64>(128*1024, qMin<qint64>(size, in->bytesAvailable()));
    QByteArray raw(toBeRead, 0);
    while (size > 0) {
        qint64 read = in->read(raw.data(), raw.size());
        if (read == 0)
            return bytes;
        // ### error checking here
        qint64 written = out->write(raw.data(), read);
        if (written == 0)
            return bytes;
        if (read != written)
            qDebug() << "### read" << read << "written" << written;
        bytes += read;
        size -= read;
        out->waitForBytesWritten(-1); // throttle
    }
    return bytes;

}

qint64 QHttpNetworkReplyPrivate::transferChunked(QIODevice *in, QIODevice *out)
{
    qint64 bytes = 0;
    while (in->bytesAvailable()) { // while we can read from input
        // if we are done with the current chunk, get the size of the new chunk
        if (currentChunkRead >= currentChunkSize) {
            currentChunkSize = 0;
            currentChunkRead = 0;
            if (bytes) {
                char crlf[2];
                bytes += in->read(crlf, 2); // read the "\r\n" after the chunk
            }
            bytes += getChunkSize(in, &currentChunkSize);
            if (currentChunkSize == -1)
                break;
        }
        // if the chunk size is 0, end of the stream
        if (currentChunkSize == 0) {
            state = AllDoneState;
            break;
        }
        // otherwise, read data
        qint64 readSize = qMin(in->bytesAvailable(), currentChunkSize - currentChunkRead);
        QByteArray buffer(readSize, 0);
        qint64 read = in->read(buffer.data(), readSize);
        bytes += read;
        currentChunkRead += read;
        qint64 written = out->write(buffer);
        Q_UNUSED(written); // Avoid compile warning when building release
        Q_ASSERT(read == written);
        // ### error checking here
        out->waitForBytesWritten(-1);
    }
    return bytes;
}

qint64 QHttpNetworkReplyPrivate::getChunkSize(QIODevice *in, qint64 *chunkSize)
{
    qint64 bytes = 0;
    char crlf[2];
    *chunkSize = -1;
    int bytesAvailable = in->bytesAvailable();
    while (bytesAvailable > bytes) {
        qint64 sniffedBytes =  in->peek(crlf, 2);
        int fragmentSize = fragment.size();
        // check the next two bytes for a "\r\n", skip blank lines
        if ((fragmentSize && sniffedBytes == 2 && crlf[0] == '\r' && crlf[1] == '\n')
           ||(fragmentSize > 1 && fragment.endsWith('\r')  && crlf[0] == '\n'))
        {
            bytes += in->read(crlf, 1);     // read the \r or \n
            if (crlf[0] == '\r')
                bytes += in->read(crlf, 1); // read the \n
            bool ok = false;
            // ignore the chunk-extension
            fragment = fragment.mid(0, fragment.indexOf(';')).trimmed();
            *chunkSize = fragment.toLong(&ok, 16);
            fragment.clear();
            break; // size done
        } else {
            // read the fragment to the buffer
            char c = 0;
            bytes += in->read(&c, 1);
            fragment.append(c);
        }
    }
    return bytes;
}

// QHttpNetworkConnectionPrivate

typedef QPair<QHttpNetworkRequest, QHttpNetworkReply*> HttpMessagePair;

class QHttpNetworkConnectionPrivate : public QObjectPrivate
{
    Q_DECLARE_PUBLIC(QHttpNetworkConnection)
public:
    QHttpNetworkConnectionPrivate(const QString &hostName, quint16 port, bool encrypt);
    ~QHttpNetworkConnectionPrivate();
    void init();
    void connectSignals(QAbstractSocket *socket);

    enum SocketState {
        IdleState = 0,          // ready to send request
        ConnectingState = 1,    // connecting to host
        WritingState = 2,       // writing the data
        WaitingState = 4,       // waiting for reply
        ReadingState = 8,       // reading the reply
        Wait4AuthState = 0x10,  // blocked for send till the current authentication slot is done
        BusyState = (ConnectingState|WritingState|WaitingState|ReadingState|Wait4AuthState)
    };

    enum { ChunkSize = 4096 };

    int indexOf(QAbstractSocket *socket) const;
    bool isSocketBusy(QAbstractSocket *socket) const;
    bool isSocketWriting(QAbstractSocket *socket) const;
    bool isSocketWaiting(QAbstractSocket *socket) const;
    bool isSocketReading(QAbstractSocket *socket) const;

    QHttpNetworkReply *queueRequest(const QHttpNetworkRequest &request);
    void unqueueRequest(QAbstractSocket *socket);
    void prepareRequest(HttpMessagePair &request);
    bool sendRequest(QAbstractSocket *socket);
    void receiveReply(QAbstractSocket *socket, QHttpNetworkReply *reply);
    void resendCurrentRequest(QAbstractSocket *socket);
    void closeChannel(int channel);
    void copyCredentials(int fromChannel, QAuthenticator *auth, bool isProxy);

    // private slots
    void _q_bytesWritten(qint64 bytes); // proceed sending
    void _q_readyRead(); // pending data to read
    void _q_disconnected(); // disconnected from host
    void _q_startNextRequest(); // send the next request from the queue
    void _q_restartPendingRequest(); // send the currently blocked request
    void _q_connected(); // start sending request
    void _q_error(QAbstractSocket::SocketError); // error from socket
#ifndef QT_NO_NETWORKPROXY
    void _q_proxyAuthenticationRequired(const QNetworkProxy &proxy, QAuthenticator *auth); // from transparent proxy
#endif
    void _q_dataReadyReadNoBuffer();
    void _q_dataReadyReadBuffer();

    void createAuthorization(QAbstractSocket *socket, QHttpNetworkRequest &request);
    bool ensureConnection(QAbstractSocket *socket);
    QString errorDetail(QNetworkReply::NetworkError errorCode, QAbstractSocket *socket);
    void eraseData(QHttpNetworkReply *reply);
#ifndef QT_NO_COMPRESS
    bool expand(QAbstractSocket *socket, QHttpNetworkReply *reply, bool dataComplete);
#endif
    void bufferData(HttpMessagePair &request);
    void removeReply(QHttpNetworkReply *reply);

    QString hostName;
    quint16 port;
    bool encrypt;

    struct Channel {
        QAbstractSocket *socket;
        SocketState state;
        QHttpNetworkRequest request; // current request
        QHttpNetworkReply *reply; // current reply for this request
        qint64 written;
        qint64 bytesTotal;
        bool resendCurrent;
        int lastStatus; // last status received on this channel
        bool pendingEncrypt; // for https (send after encrypted)
        int reconnectAttempts; // maximum 2 reconnection attempts
        QAuthenticatorPrivate::Method authMehtod;
        QAuthenticatorPrivate::Method proxyAuthMehtod;
        QAuthenticator authenticator;
        QAuthenticator proxyAuthenticator;
#ifndef QT_NO_OPENSSL
        bool ignoreSSLErrors;
#endif
        Channel() :state(IdleState), reply(0), written(0), bytesTotal(0), resendCurrent(false), reconnectAttempts(2),
            authMehtod(QAuthenticatorPrivate::None), proxyAuthMehtod(QAuthenticatorPrivate::None)
#ifndef QT_NO_OPENSSL
            , ignoreSSLErrors(false)
#endif
        {}
    };
    static const int channelCount;
    Channel channels[2]; // maximum of 2 socket connections to the server
    bool pendingAuthSignal; // there is an incomplete authentication signal
    bool pendingProxyAuthSignal; // there is an incomplete proxy authentication signal

    void appendData(QHttpNetworkReply &reply, const QByteArray &fragment, bool compressed);
    qint64 bytesAvailable(const QHttpNetworkReply &reply, bool compressed = false) const;
    qint64 read(QHttpNetworkReply &reply, QByteArray &data, qint64 maxSize, bool compressed);
    void emitReplyError(QAbstractSocket *socket, QHttpNetworkReply *reply, QNetworkReply::NetworkError errorCode);
    bool handleAuthenticateChallenge(QAbstractSocket *socket, QHttpNetworkReply *reply, bool isProxy, bool &resend);
    void allDone(QAbstractSocket *socket, QHttpNetworkReply *reply);
    void handleStatus(QAbstractSocket *socket, QHttpNetworkReply *reply);
    inline bool emitSignals(QHttpNetworkReply *reply);
    inline bool expectContent(QHttpNetworkReply *reply);

#ifndef QT_NO_OPENSSL
    void _q_encrypted(); // start sending request (https)
    void _q_sslErrors(const QList<QSslError> &errors); // ssl errors from the socket
    QSslConfiguration sslConfiguration(const QHttpNetworkReply &reply) const;
#endif

#ifndef QT_NO_NETWORKPROXY
    QNetworkProxy networkProxy;
#endif

    //The request queues
    QList<HttpMessagePair> highPriorityQueue;
    QList<HttpMessagePair> lowPriorityQueue;
};

const int QHttpNetworkConnectionPrivate::channelCount = 2;

QHttpNetworkConnectionPrivate::QHttpNetworkConnectionPrivate(const QString &hostName, quint16 port, bool encrypt)
: hostName(hostName), port(port), encrypt(encrypt),
  pendingAuthSignal(false), pendingProxyAuthSignal(false)
#ifndef QT_NO_NETWORKPROXY
  , networkProxy(QNetworkProxy::NoProxy)
#endif
{
}

QHttpNetworkConnectionPrivate::~QHttpNetworkConnectionPrivate()
{
    for (int i = 0; i < channelCount; ++i) {
        channels[i].socket->close();
        delete channels[i].socket;
    }
}

void QHttpNetworkConnectionPrivate::connectSignals(QAbstractSocket *socket)
{
    Q_Q(QHttpNetworkConnection);

    QObject::connect(socket, SIGNAL(bytesWritten(qint64)),
               q, SLOT(_q_bytesWritten(qint64)),
               Qt::DirectConnection);
    QObject::connect(socket, SIGNAL(connected()),
               q, SLOT(_q_connected()),
               Qt::DirectConnection);
    QObject::connect(socket, SIGNAL(readyRead()),
               q, SLOT(_q_readyRead()),
               Qt::DirectConnection);
    QObject::connect(socket, SIGNAL(disconnected()),
               q, SLOT(_q_disconnected()),
               Qt::DirectConnection);
    QObject::connect(socket, SIGNAL(error(QAbstractSocket::SocketError)),
               q, SLOT(_q_error(QAbstractSocket::SocketError)),
               Qt::DirectConnection);
#ifndef QT_NO_NETWORKPROXY
    QObject::connect(socket, SIGNAL(proxyAuthenticationRequired(const QNetworkProxy&, QAuthenticator*)),
               q, SLOT(_q_proxyAuthenticationRequired(const QNetworkProxy&, QAuthenticator*)),
               Qt::DirectConnection);
#endif

#ifndef QT_NO_OPENSSL
    QSslSocket *sslSocket = qobject_cast<QSslSocket*>(socket);
    QObject::connect(sslSocket, SIGNAL(encrypted()),
                     q, SLOT(_q_encrypted()),
                     Qt::DirectConnection);
    QObject::connect(sslSocket, SIGNAL(sslErrors(const QList<QSslError>&)),
               q, SLOT(_q_sslErrors(const QList<QSslError>&)),
               Qt::DirectConnection);
#endif
}

void QHttpNetworkConnectionPrivate::init()
{
    for (int i = 0; i < channelCount; ++i) {
#ifndef QT_NO_OPENSSL
        channels[i].socket = new QSslSocket;
#else
        channels[i].socket = new QTcpSocket;
#endif
        connectSignals(channels[i].socket);
    }
}

int QHttpNetworkConnectionPrivate::indexOf(QAbstractSocket *socket) const
{
    for (int i = 0; i < channelCount; ++i)
        if (channels[i].socket == socket)
            return i;
    return -1;
}

bool QHttpNetworkConnectionPrivate::isSocketBusy(QAbstractSocket *socket) const
{
    int i = indexOf(socket);
    return (channels[i].state & BusyState);
}

bool QHttpNetworkConnectionPrivate::isSocketWriting(QAbstractSocket *socket) const
{
    int i = indexOf(socket);
    return (i != -1 && (channels[i].state & WritingState));
}

bool QHttpNetworkConnectionPrivate::isSocketWaiting(QAbstractSocket *socket) const
{
    int i = indexOf(socket);
    return (i != -1 && (channels[i].state & WaitingState));
}

bool QHttpNetworkConnectionPrivate::isSocketReading(QAbstractSocket *socket) const
{
    int i = indexOf(socket);
    return (i != -1 && (channels[i].state & ReadingState));
}


void QHttpNetworkConnectionPrivate::appendData(QHttpNetworkReply &reply, const QByteArray &fragment, bool compressed)
{
    QByteArray *ba = (compressed) ? &reply.d_func()->compressedData : &reply.d_func()->responseData;
    ba->append(fragment);
    return;
}

qint64 QHttpNetworkConnectionPrivate::bytesAvailable(const QHttpNetworkReply &reply, bool compressed) const
{
    const QByteArray *ba = (compressed) ? &reply.d_func()->compressedData : &reply.d_func()->responseData;
    return ba->size();
}

qint64 QHttpNetworkConnectionPrivate::read(QHttpNetworkReply &reply, QByteArray &data, qint64 maxSize, bool compressed)
{
    QByteArray *ba = (compressed) ? &reply.d_func()->compressedData : &reply.d_func()->responseData;
    if (maxSize == -1 || maxSize >= ba->size()) {
        // read the whole data
        data = *ba;
        ba->clear();
    } else {
        // read only the requested length
        data = ba->mid(0, maxSize);
        ba->remove(0, maxSize);
    }
    return data.size();
}

void QHttpNetworkConnectionPrivate::eraseData(QHttpNetworkReply *reply)
{
    reply->d_func()->compressedData.clear();
    reply->d_func()->responseData.clear();
}

void QHttpNetworkConnectionPrivate::prepareRequest(HttpMessagePair &messagePair)
{
    QHttpNetworkRequest &request = messagePair.first;
    QHttpNetworkReply *reply = messagePair.second;

    // add missing fields for the request
    QByteArray value;
    // check if Content-Length is provided
    QIODevice *data = request.data();
    if (data && request.contentLength() == -1) {
        if (!data->isSequential())
            request.setContentLength(data->size());
        else
            bufferData(messagePair); // ### or do chunked upload
    }
    // set the Connection/Proxy-Connection: Keep-Alive headers
#ifndef QT_NO_NETWORKPROXY
    if (networkProxy.type() == QNetworkProxy::HttpCachingProxy)  {
        value = request.headerField("proxy-connection");
        if (value.isEmpty())
            request.setHeaderField("Proxy-Connection", "Keep-Alive");
    } else {
#endif
        value = request.headerField("connection");
        if (value.isEmpty())
            request.setHeaderField("Connection", "Keep-Alive");
#ifndef QT_NO_NETWORKPROXY
    }
#endif
    // set the gzip header
    value = request.headerField("accept-encoding");
    if (value.isEmpty()) {
#ifndef QT_NO_COMPRESS
        request.setHeaderField("Accept-Encoding", "gzip");
        request.d->autoDecompress = true;
#else
        // if zlib is not available set this to false always
        request.d->autoDecompress = false;
#endif
    }
    // set the User Agent
    value = request.headerField("user-agent");
    if (value.isEmpty())
        request.setHeaderField("User-Agent", "Mozilla/5.0");
    // set the host
    value = request.headerField("host");
    if (value.isEmpty()) {
        QByteArray host = QUrl::toAce(hostName);

        int port = request.url().port();
        if (port != -1) {
            host += ':';
            host += QByteArray::number(port);
        }

        request.setHeaderField("Host", host);
    }

    reply->d_func()->requestIsPrepared = true;
}

bool QHttpNetworkConnectionPrivate::ensureConnection(QAbstractSocket *socket)
{
    // make sure that this socket is in a connected state, if not initiate
    // connection to the host.
    if (socket->state() != QAbstractSocket::ConnectedState) {
        // connect to the host if not already connected.
        int index = indexOf(socket);
        channels[index].state = ConnectingState;
        channels[index].pendingEncrypt = encrypt;

        // This workaround is needed since we use QAuthenticator for NTLM authentication. The "phase == Done"
        // is the usual criteria for emitting authentication signals. The "phase" is set to "Done" when the
        // last header for Authorization is generated by the QAuthenticator. Basic & Digest logic does not
        // check the "phase" for generating the Authorization header. NTLM authentication is a two stage
        // process & needs the "phase". To make sure the QAuthenticator uses the current username/password
        // the phase is reset to Start.
        QAuthenticatorPrivate *priv = QAuthenticatorPrivate::getPrivate(channels[index].authenticator);
        if (priv && priv->phase == QAuthenticatorPrivate::Done)
            priv->phase = QAuthenticatorPrivate::Start;
        priv = QAuthenticatorPrivate::getPrivate(channels[index].proxyAuthenticator);
        if (priv && priv->phase == QAuthenticatorPrivate::Done)
            priv->phase = QAuthenticatorPrivate::Start;

        QString connectHost = hostName;
        qint16 connectPort = port;

#ifndef QT_NO_NETWORKPROXY
        // HTTPS always use transparent proxy.
        if (networkProxy.type() != QNetworkProxy::NoProxy && !encrypt) {
            connectHost = networkProxy.hostName();
            connectPort = networkProxy.port();
        }
#endif
        if (encrypt) {
#ifndef QT_NO_OPENSSL
            QSslSocket *sslSocket = qobject_cast<QSslSocket*>(socket);
            sslSocket->connectToHostEncrypted(connectHost, connectPort);
            if (channels[index].ignoreSSLErrors)
                sslSocket->ignoreSslErrors();
#else
            emitReplyError(socket, channels[index].reply, QNetworkReply::ProtocolUnknownError);
#endif
        } else {
            socket->connectToHost(connectHost, connectPort);
        }
        return false;
    }
    return true;
}


bool QHttpNetworkConnectionPrivate::sendRequest(QAbstractSocket *socket)
{
    Q_Q(QHttpNetworkConnection);

    int i = indexOf(socket);
    switch (channels[i].state) {
    case IdleState: { // write the header
        if (!ensureConnection(socket)) {
            // wait for the connection (and encryption) to be done
            // sendRequest will be called again from either
            // _q_connected or _q_encrypted
            return false;
        }
        channels[i].written = 0; // excluding the header
        channels[i].bytesTotal = 0;
        if (channels[i].reply) {
            channels[i].reply->d_func()->clear();
            channels[i].reply->d_func()->connection = q;
            channels[i].reply->d_func()->autoDecompress = channels[i].request.d->autoDecompress;
        }
        channels[i].state = WritingState;
        channels[i].pendingEncrypt = false;
        // if the url contains authentication parameters, use the new ones
        // both channels will use the new authentication parameters
        if (!channels[i].request.url().userInfo().isEmpty()) {
            QUrl url = channels[i].request.url();
            QAuthenticator &auth = channels[i].authenticator;
            if (url.userName() != auth.user()
                || (!url.password().isEmpty() && url.password() != auth.password())) {
                auth.setUser(url.userName());
                auth.setPassword(url.password());
                copyCredentials(i, &auth, false);
            }
            // clear the userinfo,  since we use the same request for resending
            // userinfo in url can conflict with the one in the authenticator
            url.setUserInfo(QString());
            channels[i].request.setUrl(url);
        }
        createAuthorization(socket, channels[i].request);
#ifndef QT_NO_NETWORKPROXY
        QByteArray header = QHttpNetworkRequestPrivate::header(channels[i].request,
            (networkProxy.type() != QNetworkProxy::NoProxy));
#else
        QByteArray header = QHttpNetworkRequestPrivate::header(channels[i].request,
            false);
#endif
        socket->write(header);
        QIODevice *data = channels[i].request.d->data;
        QHttpNetworkReply *reply = channels[i].reply;
        if (reply && reply->d_func()->requestDataBuffer.size())
            data = &channels[i].reply->d_func()->requestDataBuffer;
        if (data && (data->isOpen() || data->open(QIODevice::ReadOnly))) {
            if (data->isSequential()) {
                channels[i].bytesTotal = -1;
                QObject::connect(data, SIGNAL(readyRead()), q, SLOT(_q_dataReadyReadNoBuffer()));
                QObject::connect(data, SIGNAL(readChannelFinished()), q, SLOT(_q_dataReadyReadNoBuffer()));
            } else {
                channels[i].bytesTotal = data->size();
            }
        } else {
            channels[i].state = WaitingState;
            break;
        }
        // write the initial chunk together with the headers
        // fall through
    }
    case WritingState: { // write the data
        QIODevice *data = channels[i].request.d->data;
        if (channels[i].reply->d_func()->requestDataBuffer.size())
            data = &channels[i].reply->d_func()->requestDataBuffer;
        if (!data || channels[i].bytesTotal == channels[i].written) {
            channels[i].state = WaitingState; // now wait for response
            break;
        }

        QByteArray chunk;
        chunk.resize(ChunkSize);
        qint64 readSize = data->read(chunk.data(), ChunkSize);
        if (readSize == -1) {
            // source has reached EOF
            channels[i].state = WaitingState; // now wait for response
        } else if (readSize > 0) {
            // source gave us something useful
            channels[i].written += socket->write(chunk.data(), readSize);
            if (channels[i].reply)
                emit channels[i].reply->dataSendProgress(channels[i].written, channels[i].bytesTotal);
        }
        break;
    }
    case WaitingState:
    case ReadingState:
    case Wait4AuthState:
        // ignore _q_bytesWritten in these states
        // fall through
    default:
        break;
    }
    return true;
}

bool QHttpNetworkConnectionPrivate::emitSignals(QHttpNetworkReply *reply)
{
    // for 401 & 407 don't emit the data signals. Content along with these
    // responses are send only if the authentication fails.
    return (reply && reply->d_func()->statusCode != 401 && reply->d_func()->statusCode != 407);
}

bool QHttpNetworkConnectionPrivate::expectContent(QHttpNetworkReply *reply)
{
    // check whether we can expect content after the headers (rfc 2616, sec4.4)
    if (!reply)
        return false;
    if ((reply->d_func()->statusCode >= 100 && reply->d_func()->statusCode < 200)
        || reply->d_func()->statusCode == 204 || reply->d_func()->statusCode == 304)
        return false;
    if (reply->d_func()->request.operation() == QHttpNetworkRequest::Head)
        return !emitSignals(reply);
    if (reply->d_func()->contentLength() == 0)
        return false;
    return true;
}

void QHttpNetworkConnectionPrivate::emitReplyError(QAbstractSocket *socket,
                                                   QHttpNetworkReply *reply,
                                                   QNetworkReply::NetworkError errorCode)
{
    Q_Q(QHttpNetworkConnection);
    if (socket && reply) {
        // this error matters only to this reply
        reply->d_func()->errorString = errorDetail(errorCode, socket);
        emit reply->finishedWithError(errorCode, reply->d_func()->errorString);
        int i = indexOf(socket);
        // remove the corrupt data if any
        eraseData(channels[i].reply);
        closeChannel(i);
        // send the next request
        QMetaObject::invokeMethod(q, "_q_startNextRequest", Qt::QueuedConnection);
    }
}

#ifndef QT_NO_COMPRESS
bool QHttpNetworkConnectionPrivate::expand(QAbstractSocket *socket, QHttpNetworkReply *reply, bool dataComplete)
{
    Q_ASSERT(socket);
    Q_ASSERT(reply);

    qint64 total = bytesAvailable(*reply, true);
    if (total >= CHUNK || dataComplete) {
        int i = indexOf(socket);
         // uncompress the data
        QByteArray content, inflated;
        read(*reply, content, -1, true);
        int ret = Z_OK;
        if (content.size())
            ret = reply->d_func()->gunzipBodyPartially(content, inflated);
        int retCheck = (dataComplete) ? Z_STREAM_END : Z_OK;
        if (ret >= retCheck) {
            if (inflated.size()) {
                reply->d_func()->totalProgress += inflated.size();
                appendData(*reply, inflated, false);
                if (emitSignals(reply)) {
                    emit reply->readyRead();
                    // make sure that the reply is valid
                    if (channels[i].reply != reply)
                        return true;
                    emit reply->dataReadProgress(reply->d_func()->totalProgress, 0);
                    // make sure that the reply is valid
                    if (channels[i].reply != reply)
                        return true;

                }
            }
        } else {
            emitReplyError(socket, reply, QNetworkReply::ProtocolFailure);
            return false;
        }
    }
    return true;
}
#endif

void QHttpNetworkConnectionPrivate::receiveReply(QAbstractSocket *socket, QHttpNetworkReply *reply)
{
    Q_ASSERT(socket);

    Q_Q(QHttpNetworkConnection);
    qint64 bytes = 0;
    QAbstractSocket::SocketState state = socket->state();
    int i = indexOf(socket);

    // connection might be closed to signal the end of data
    if (state == QAbstractSocket::UnconnectedState) {
        if (!socket->bytesAvailable()) {
            if (reply && reply->d_func()->state == QHttpNetworkReplyPrivate::ReadingDataState) {
                reply->d_func()->state = QHttpNetworkReplyPrivate::AllDoneState;
                channels[i].state = IdleState;
                allDone(socket, reply);
            } else {
                // try to reconnect/resend before sending an error.
                if (channels[i].reconnectAttempts-- > 0) {
                    resendCurrentRequest(socket);
                } else {
                    reply->d_func()->errorString = errorDetail(QNetworkReply::RemoteHostClosedError, socket);
                    emit reply->finishedWithError(QNetworkReply::RemoteHostClosedError, reply->d_func()->errorString);
                    QMetaObject::invokeMethod(q, "_q_startNextRequest", Qt::QueuedConnection);
                }
            }
        }
    }

    // read loop for the response
    while (socket->bytesAvailable()) {
        QHttpNetworkReplyPrivate::ReplyState state = reply ? reply->d_func()->state : QHttpNetworkReplyPrivate::AllDoneState;
        switch (state) {
        case QHttpNetworkReplyPrivate::NothingDoneState:
        case QHttpNetworkReplyPrivate::ReadingStatusState:
            bytes += reply->d_func()->readStatus(socket);
            channels[i].lastStatus = reply->d_func()->statusCode;
            break;
        case QHttpNetworkReplyPrivate::ReadingHeaderState:
            bytes += reply->d_func()->readHeader(socket);
            if (reply->d_func()->state == QHttpNetworkReplyPrivate::ReadingDataState) {
                if (reply->d_func()->isGzipped() && reply->d_func()->autoDecompress) {
                    // remove the Content-Length from header
                    reply->d_func()->removeAutoDecompressHeader();
                } else {
                    reply->d_func()->autoDecompress = false;
                }
                if (reply && reply->d_func()->statusCode == 100) {
                    reply->d_func()->state = QHttpNetworkReplyPrivate::ReadingStatusState;
                    break; // ignore
                }
                if (emitSignals(reply))
                    emit reply->headerChanged();
                if (!expectContent(reply)) {
                    reply->d_func()->state = QHttpNetworkReplyPrivate::AllDoneState;
                    channels[i].state = IdleState;
                    allDone(socket, reply);
                    return;
                }
            }
            break;
        case QHttpNetworkReplyPrivate::ReadingDataState: {
            QBuffer fragment;
            fragment.open(QIODevice::WriteOnly);
            bytes = reply->d_func()->readBody(socket, &fragment);
            if (bytes) {
                appendData(*reply, fragment.data(), reply->d_func()->autoDecompress);
                if (!reply->d_func()->autoDecompress) {
                    reply->d_func()->totalProgress += fragment.size();
                    if (emitSignals(reply)) {
                        emit reply->readyRead();
                        // make sure that the reply is valid
                        if (channels[i].reply != reply)
                            return;
                        emit reply->dataReadProgress(reply->d_func()->totalProgress, reply->d_func()->bodyLength);
                        // make sure that the reply is valid
                        if (channels[i].reply != reply)
                            return;
                    }
                }
#ifndef QT_NO_COMPRESS
                else if (!expand(socket, reply, false)) { // expand a chunk if possible
                    return; // ### expand failed
                }
#endif
            }
            if (reply->d_func()->state == QHttpNetworkReplyPrivate::ReadingDataState)
                break;
            // everything done, fall through
            }
      case QHttpNetworkReplyPrivate::AllDoneState:
            channels[i].state = IdleState;
            allDone(socket, reply);
            break;
        default:
            break;
        }
    }
}

void QHttpNetworkConnectionPrivate::allDone(QAbstractSocket *socket, QHttpNetworkReply *reply)
{
#ifndef QT_NO_COMPRESS
    // expand the whole data.
    if (expectContent(reply) && reply->d_func()->autoDecompress && !reply->d_func()->streamEnd)
        expand(socket, reply, true); // ### if expand returns false, its an error
#endif
    // while handling 401 & 407, we might reset the status code, so save this.
    bool emitFinished = emitSignals(reply);
    handleStatus(socket, reply);
    // ### at this point there should be no more data on the socket
    // close if server requested
    int i = indexOf(socket);
    if (reply->d_func()->connectionCloseEnabled())
        closeChannel(i);
    // queue the finished signal, this is required since we might send new requests from
    // slot connected to it. The socket will not fire readyRead signal, if we are already
    // in the slot connected to readyRead
    if (emitFinished)
        QMetaObject::invokeMethod(reply, "finished", Qt::QueuedConnection);
    // reset the reconnection attempts after we receive a complete reply.
    // in case of failures, each channel will attempt two reconnects before emitting error.
    channels[i].reconnectAttempts = 2;
}

void QHttpNetworkConnectionPrivate::handleStatus(QAbstractSocket *socket, QHttpNetworkReply *reply)
{
    Q_ASSERT(socket);
    Q_ASSERT(reply);

    Q_Q(QHttpNetworkConnection);

    int statusCode = reply->statusCode();
    bool resend = false;

    switch (statusCode) {
    case 401:
    case 407:
        handleAuthenticateChallenge(socket, reply, (statusCode == 407), resend);
        if (resend) {
            eraseData(reply);
            sendRequest(socket);
        }
        break;
    default:
        QMetaObject::invokeMethod(q, "_q_startNextRequest", Qt::QueuedConnection);
    }
}

void QHttpNetworkConnectionPrivate::copyCredentials(int fromChannel, QAuthenticator *auth, bool isProxy)
{
    Q_ASSERT(auth);

    // select another channel
    QAuthenticator* otherAuth = 0;
    for (int i = 0; i < channelCount; ++i) {
        if (i == fromChannel)
            continue;
        if (isProxy)
            otherAuth = &channels[i].proxyAuthenticator;
        else
            otherAuth = &channels[i].authenticator;
        // if the credentials are different, copy them
        if (otherAuth->user().compare(auth->user()))
            otherAuth->setUser(auth->user());
        if (otherAuth->password().compare(auth->password()))
            otherAuth->setPassword(auth->password());
    }
}


bool QHttpNetworkConnectionPrivate::handleAuthenticateChallenge(QAbstractSocket *socket, QHttpNetworkReply *reply,
                                                                bool isProxy, bool &resend)
{
    Q_ASSERT(socket);
    Q_ASSERT(reply);

    Q_Q(QHttpNetworkConnection);

    resend = false;
    //create the response header to be used with QAuthenticatorPrivate.
    QHttpResponseHeader responseHeader;
    QList<QPair<QByteArray, QByteArray> > fields = reply->header();
    QList<QPair<QByteArray, QByteArray> >::const_iterator it = fields.constBegin();
    while (it != fields.constEnd()) {
        responseHeader.addValue(QString::fromLatin1(it->first), QString::fromUtf8(it->second));
        it++;
    }
    //find out the type of authentication protocol requested.
    QAuthenticatorPrivate::Method authMethod = reply->d_func()->authenticationMethod(isProxy);
    if (authMethod != QAuthenticatorPrivate::None) {
        int i = indexOf(socket);
        //Use a single authenticator for all domains. ### change later to use domain/realm
        QAuthenticator* auth = 0;
        if (isProxy) {
            auth = &channels[i].proxyAuthenticator;
            channels[i].proxyAuthMehtod = authMethod;
        } else {
            auth = &channels[i].authenticator;
            channels[i].authMehtod = authMethod;
        }
        //proceed with the authentication.
        if (auth->isNull())
            auth->detach();
        QAuthenticatorPrivate *priv = QAuthenticatorPrivate::getPrivate(*auth);
        priv->parseHttpResponse(responseHeader, isProxy);

        if (priv->phase == QAuthenticatorPrivate::Done) {
            if ((isProxy && pendingProxyAuthSignal) ||(!isProxy && pendingAuthSignal)) {
                // drop the request
                eraseData(channels[i].reply);
                closeChannel(i);
                channels[i].lastStatus = 0;
                channels[i].state =  Wait4AuthState;
                return false;
            }
            // cannot use this socket until the slot returns
            channels[i].state = WaitingState;
            socket->blockSignals(true);
            if (!isProxy) {
                pendingAuthSignal = true;
                emit q->authenticationRequired(reply->request(), auth, q);
                pendingAuthSignal = false;
#ifndef QT_NO_NETWORKPROXY
            } else {
                pendingProxyAuthSignal = true;
                emit q->proxyAuthenticationRequired(networkProxy, auth, q);
                pendingProxyAuthSignal = false;
#endif
            }
            socket->blockSignals(false);
            // socket free to use
            channels[i].state = IdleState;
            if (priv->phase != QAuthenticatorPrivate::Done) {
                // send any pending requests
                copyCredentials(i,  auth, isProxy);
                QMetaObject::invokeMethod(q, "_q_restartPendingRequest", Qt::QueuedConnection);
            }
        }
        // changing values in QAuthenticator will reset the 'phase'
        if (priv->phase == QAuthenticatorPrivate::Done) {
            // authentication is cancelled, send the current contents to the user.
            emit channels[i].reply->headerChanged();
            emit channels[i].reply->readyRead();
            emit channels[i].reply->finished();
            QNetworkReply::NetworkError errorCode =
                isProxy
                ? QNetworkReply::ProxyAuthenticationRequiredError
                : QNetworkReply::AuthenticationRequiredError;
            reply->d_func()->errorString = errorDetail(errorCode, socket);
            emit q->error(errorCode, reply->d_func()->errorString);
            emit channels[i].reply->finished();
            // ### at this point the reply could be deleted
            socket->close();
            // remove pending request on the other channels
            for (int j = 0; j < channelCount; ++j) {
                if (j != i && channels[j].state ==  Wait4AuthState)
                    channels[j].state = IdleState;
            }
            return true;
        }
        //resend the request
        resend = true;
        return true;
    }
    return false;
}

void QHttpNetworkConnectionPrivate::createAuthorization(QAbstractSocket *socket, QHttpNetworkRequest &request)
{
    Q_ASSERT(socket);

    int i = indexOf(socket);
    if (channels[i].authMehtod != QAuthenticatorPrivate::None) {
        if (!(channels[i].authMehtod == QAuthenticatorPrivate::Ntlm && channels[i].lastStatus != 401)) {
            QAuthenticatorPrivate *priv = QAuthenticatorPrivate::getPrivate(channels[i].authenticator);
            if (priv && priv->method != QAuthenticatorPrivate::None) {
                QByteArray response = priv->calculateResponse(request.d->methodName(), request.d->uri(false));
                request.setHeaderField("authorization", response);
            }
        }
    }
    if (channels[i].proxyAuthMehtod != QAuthenticatorPrivate::None) {
        if (!(channels[i].proxyAuthMehtod == QAuthenticatorPrivate::Ntlm && channels[i].lastStatus != 407)) {
            QAuthenticatorPrivate *priv = QAuthenticatorPrivate::getPrivate(channels[i].proxyAuthenticator);
            if (priv && priv->method != QAuthenticatorPrivate::None) {
                QByteArray response = priv->calculateResponse(request.d->methodName(), request.d->uri(false));
                request.setHeaderField("proxy-authorization", response);
            }
        }
    }
}

QHttpNetworkReply* QHttpNetworkConnectionPrivate::queueRequest(const QHttpNetworkRequest &request)
{
    Q_Q(QHttpNetworkConnection);

    // The reply component of the pair is created initially.
    QHttpNetworkReply *reply = new QHttpNetworkReply(request.url());
    reply->setRequest(request);
    reply->d_func()->connection = q;
    HttpMessagePair pair = qMakePair(request, reply);

    switch (request.priority()) {
    case QHttpNetworkRequest::HighPriority:
        highPriorityQueue.prepend(pair);
        break;
    case QHttpNetworkRequest::NormalPriority:
    case QHttpNetworkRequest::LowPriority:
        lowPriorityQueue.prepend(pair);
        break;
    }
    QMetaObject::invokeMethod(q, "_q_startNextRequest", Qt::QueuedConnection);
    return reply;
}

void QHttpNetworkConnectionPrivate::unqueueRequest(QAbstractSocket *socket)
{
    Q_ASSERT(socket);

    int i = indexOf(socket);

    if (!highPriorityQueue.isEmpty()) {
        for (int j = highPriorityQueue.count() - 1; j >= 0; --j) {
            HttpMessagePair &messagePair = highPriorityQueue[j];
            if (!messagePair.second->d_func()->requestIsPrepared)
                prepareRequest(messagePair);
            if (!messagePair.second->d_func()->requestIsBuffering) {
                channels[i].request = messagePair.first;
                channels[i].reply = messagePair.second;
                sendRequest(socket);
                highPriorityQueue.removeAt(j);
                return;
            }
        }
    }

    if (!lowPriorityQueue.isEmpty()) {
        for (int j = lowPriorityQueue.count() - 1; j >= 0; --j) {
            HttpMessagePair &messagePair = lowPriorityQueue[j];
            if (!messagePair.second->d_func()->requestIsPrepared)
                prepareRequest(messagePair);
            if (!messagePair.second->d_func()->requestIsBuffering) {
                channels[i].request = messagePair.first;
                channels[i].reply = messagePair.second;
                sendRequest(socket);
                lowPriorityQueue.removeAt(j);
                return;
            }
        }
    }
}

void QHttpNetworkConnectionPrivate::closeChannel(int channel)
{
    QAbstractSocket *socket = channels[channel].socket;
    socket->blockSignals(true);
    socket->close();
    socket->blockSignals(false);
    channels[channel].state = IdleState;
}

void QHttpNetworkConnectionPrivate::resendCurrentRequest(QAbstractSocket *socket)
{
    Q_Q(QHttpNetworkConnection);
    Q_ASSERT(socket);
    int i = indexOf(socket);
    closeChannel(i);
    channels[i].resendCurrent = true;
    QMetaObject::invokeMethod(q, "_q_startNextRequest", Qt::QueuedConnection);
}

QString QHttpNetworkConnectionPrivate::errorDetail(QNetworkReply::NetworkError errorCode, QAbstractSocket* socket)
{
    Q_ASSERT(socket);

    QString errorString;
    switch (errorCode) {
    case QNetworkReply::HostNotFoundError:
        errorString = QString::fromLatin1(QT_TRANSLATE_NOOP("QHttp", "Host %1 not found"))
                              .arg(socket->peerName());
        break;
    case QNetworkReply::ConnectionRefusedError:
        errorString = QLatin1String(QT_TRANSLATE_NOOP("QHttp", "Connection refused"));
        break;
    case QNetworkReply::RemoteHostClosedError:
        errorString = QLatin1String(QT_TRANSLATE_NOOP("QHttp", "Connection closed"));
        break;
    case QNetworkReply::TimeoutError:
        errorString = QLatin1String(QT_TRANSLATE_NOOP("QHttp", "HTTP request failed"));
        break;
    case QNetworkReply::ProxyAuthenticationRequiredError:
        errorString = QLatin1String(QT_TRANSLATE_NOOP("QHttp", "Proxy requires authentication"));
        break;
    case QNetworkReply::AuthenticationRequiredError:
        errorString = QLatin1String(QT_TRANSLATE_NOOP("QHttp", "Host requires authentication"));
        break;
    case QNetworkReply::ProtocolFailure:
        errorString = QLatin1String(QT_TRANSLATE_NOOP("QHttp", "Data corrupted"));
        break;
    case QNetworkReply::ProtocolUnknownError:
        errorString = QLatin1String(QT_TRANSLATE_NOOP("QHttp", "Unknown protocol specified"));
        break;
    case QNetworkReply::SslHandshakeFailedError:
        errorString = QLatin1String(QT_TRANSLATE_NOOP("QHttp", "SSL handshake failed"));
        break;
    default:
        // all other errors are treated as QNetworkReply::UnknownNetworkError
        errorString = QLatin1String(QT_TRANSLATE_NOOP("QHttp", "HTTP request failed"));
        break;
    }
    return errorString;
}

void QHttpNetworkConnectionPrivate::removeReply(QHttpNetworkReply *reply)
{
    // remove the from active list.
    for (int i = 0; i < channelCount; ++i) {
        if (channels[i].reply == reply) {
            channels[i].reply = 0;
            closeChannel(i);
            return;
        }
    }
    // remove from the high priority queue
    if (!highPriorityQueue.isEmpty()) {
        for (int j = highPriorityQueue.count() - 1; j >= 0; --j) {
            HttpMessagePair messagePair = highPriorityQueue.at(j);
            if (messagePair.second == reply) {
                highPriorityQueue.removeAt(j);
                return;
            }
        }
    }
    // remove from the low priority queue
    if (!lowPriorityQueue.isEmpty()) {
        for (int j = lowPriorityQueue.count() - 1; j >= 0; --j) {
            HttpMessagePair messagePair = lowPriorityQueue.at(j);
            if (messagePair.second == reply) {
                lowPriorityQueue.removeAt(j);
                return;
            }
        }
    }
}


//private slots
void QHttpNetworkConnectionPrivate::_q_readyRead()
{
    Q_Q(QHttpNetworkConnection);
    QAbstractSocket *socket = qobject_cast<QAbstractSocket*>(q->sender());
    if (!socket)
        return; // ### error
    if (isSocketWaiting(socket) || isSocketReading(socket)) {
        int i = indexOf(socket);
        channels[i].state = ReadingState;
        if (channels[i].reply)
            receiveReply(socket, channels[i].reply);
    }
    // ### error
}

void QHttpNetworkConnectionPrivate::_q_bytesWritten(qint64 bytes)
{
    Q_UNUSED(bytes);
    Q_Q(QHttpNetworkConnection);
    QAbstractSocket *socket = qobject_cast<QAbstractSocket*>(q->sender());
    if (!socket)
        return; // ### error
    if (isSocketWriting(socket))
        sendRequest(socket);
    // otherwise we do nothing
}

void QHttpNetworkConnectionPrivate::_q_disconnected()
{
    Q_Q(QHttpNetworkConnection);
    QAbstractSocket *socket = qobject_cast<QAbstractSocket*>(q->sender());
    if (!socket)
        return; // ### error
    // read the available data before closing
    int i = indexOf(socket);
    if (isSocketWaiting(socket) || isSocketReading(socket)) {
        channels[i].state = ReadingState;
        if (channels[i].reply)
            receiveReply(socket, channels[i].reply);
    }
    channels[i].state = IdleState;
}

void QHttpNetworkConnectionPrivate::_q_startNextRequest()
{
    // send the current request again
    if (channels[0].resendCurrent || channels[1].resendCurrent) {
        int i = channels[0].resendCurrent ? 0:1;
        QAbstractSocket *socket = channels[i].socket;
        channels[i].resendCurrent = false;
        channels[i].state = IdleState;
        if (channels[i].reply)
            sendRequest(socket);
        return;
    }
    // send the request using the idle socket
    QAbstractSocket *socket = channels[0].socket;
    if (isSocketBusy(socket)) {
        socket = (isSocketBusy(channels[1].socket) ? 0 :channels[1].socket);
    }

    if (!socket) {
        return; // this will be called after finishing current request.
    }
    unqueueRequest(socket);
}

void QHttpNetworkConnectionPrivate::_q_restartPendingRequest()
{
    // send the request using the idle socket
    for (int i = 0 ; i < channelCount; ++i) {
        QAbstractSocket *socket = channels[i].socket;
        if (channels[i].state ==  Wait4AuthState) {
            channels[i].state = IdleState;
            if (channels[i].reply)
                sendRequest(socket);
        }
    }
}

void QHttpNetworkConnectionPrivate::_q_connected()
{
    Q_Q(QHttpNetworkConnection);
    QAbstractSocket *socket = qobject_cast<QAbstractSocket*>(q->sender());
    if (!socket)
        return; // ### error
    int i = indexOf(socket);
    // ### FIXME: if the server closes the connection unexpectedly, we shouldn't send the same broken request again!
    //channels[i].reconnectAttempts = 2;
    if (!channels[i].pendingEncrypt) {
        channels[i].state = IdleState;
        if (channels[i].reply)
            sendRequest(socket);
        else
            closeChannel(i);
    }
}


void QHttpNetworkConnectionPrivate::_q_error(QAbstractSocket::SocketError socketError)
{
    Q_Q(QHttpNetworkConnection);
    QAbstractSocket *socket = qobject_cast<QAbstractSocket*>(q->sender());
    if (!socket)
        return;
    bool send2Reply = false;
    int i = indexOf(socket);
    QNetworkReply::NetworkError errorCode = QNetworkReply::UnknownNetworkError;

    switch (socketError) {
    case QAbstractSocket::HostNotFoundError:
        errorCode = QNetworkReply::HostNotFoundError;
        break;
    case QAbstractSocket::ConnectionRefusedError:
        errorCode = QNetworkReply::ConnectionRefusedError;
        break;
    case QAbstractSocket::RemoteHostClosedError:
        // try to reconnect/resend before sending an error.
        // while "Reading" the _q_disconnected() will handle this.
        if (channels[i].state != IdleState && channels[i].state != ReadingState) {
            if (channels[i].reconnectAttempts-- > 0) {
                resendCurrentRequest(socket);
                return;
            } else {
                send2Reply = true;
                errorCode = QNetworkReply::RemoteHostClosedError;
            }
        } else {
            return;
        }
        break;
    case QAbstractSocket::SocketTimeoutError:
        // try to reconnect/resend before sending an error.
        if (channels[i].state == WritingState &&  (channels[i].reconnectAttempts-- > 0)) {
            resendCurrentRequest(socket);
            return;
        }
        send2Reply = true;
        errorCode = QNetworkReply::TimeoutError;
        break;
    case QAbstractSocket::ProxyAuthenticationRequiredError:
        errorCode = QNetworkReply::ProxyAuthenticationRequiredError;
        break;
    case QAbstractSocket::SslHandshakeFailedError:
        errorCode = QNetworkReply::SslHandshakeFailedError;
        break;
    default:
        // all other errors are treated as NetworkError
        errorCode = QNetworkReply::UnknownNetworkError;
        break;
    }
    QPointer<QObject> that = q;
    QString errorString = errorDetail(errorCode, socket);
    if (send2Reply) {
        if (channels[i].reply) {
            channels[i].reply->d_func()->errorString = errorString;
            // this error matters only to this reply
            emit channels[i].reply->finishedWithError(errorCode, errorString);
        }
        // send the next request
        QMetaObject::invokeMethod(that, "_q_startNextRequest", Qt::QueuedConnection);
    } else {
        // the failure affects all requests.
        emit q->error(errorCode, errorString);
    }
    if (that) //signals make enter the event loop
        closeChannel(i);
}

#ifndef QT_NO_NETWORKPROXY
void QHttpNetworkConnectionPrivate::_q_proxyAuthenticationRequired(const QNetworkProxy &proxy, QAuthenticator* auth)
{
    Q_Q(QHttpNetworkConnection);
    emit q->proxyAuthenticationRequired(proxy, auth, q);
}
#endif

void QHttpNetworkConnectionPrivate::_q_dataReadyReadNoBuffer()
{
    Q_Q(QHttpNetworkConnection);
    // data emitted either readyRead()
    // find out which channel it is for
    QIODevice *sender = qobject_cast<QIODevice *>(q->sender());

    // won't match anything if the qobject_cast above failed
    for (int i = 0; i < channelCount; ++i) {
        if (sender == channels[i].request.data()) {
            sendRequest(channels[i].socket);
            break;
        }
    }
}

void QHttpNetworkConnectionPrivate::_q_dataReadyReadBuffer()
{
    Q_Q(QHttpNetworkConnection);
    QIODevice *sender = qobject_cast<QIODevice *>(q->sender());
    HttpMessagePair *thePair = 0;
    for (int i = 0; !thePair && i < lowPriorityQueue.size(); ++i)
        if (lowPriorityQueue.at(i).first.data() == sender)
            thePair = &lowPriorityQueue[i];

    for (int i = 0; !thePair && i < highPriorityQueue.size(); ++i)
        if (highPriorityQueue.at(i).first.data() == sender)
            thePair = &highPriorityQueue[i];

    if (thePair) {
        bufferData(*thePair);

        // are we finished buffering?
        if (!thePair->second->d_func()->requestIsBuffering)
            _q_startNextRequest();
    }
}

void QHttpNetworkConnectionPrivate::bufferData(HttpMessagePair &messagePair)
{
    Q_Q(QHttpNetworkConnection);
    QHttpNetworkRequest &request = messagePair.first;
    QHttpNetworkReply *reply = messagePair.second;
    Q_ASSERT(request.data());
    if (!reply->d_func()->requestIsBuffering) { // first time
        QObject::connect(request.data(), SIGNAL(readyRead()), q, SLOT(_q_dataReadyReadBuffer()));
        QObject::connect(request.data(), SIGNAL(readChannelFinished()), q, SLOT(_q_dataReadyReadBuffer()));
        reply->d_func()->requestIsBuffering = true;
        reply->d_func()->requestDataBuffer.open(QIODevice::ReadWrite);
    }

    // always try to read at least one byte
    // ### FIXME! use a QRingBuffer
    qint64 bytesToRead = qMax<qint64>(1, request.data()->bytesAvailable());
    QByteArray newData;
    newData.resize(bytesToRead);
    qint64 bytesActuallyRead = request.data()->read(newData.data(), bytesToRead);

    if (bytesActuallyRead > 0) {
        // we read something
        newData.chop(bytesToRead - bytesActuallyRead);
        reply->d_func()->requestDataBuffer.write(newData);
    } else if (bytesActuallyRead == -1) { // last time
        QObject::disconnect(request.data(), SIGNAL(readyRead()), q, SLOT(_q_dataReadyReadBuffer()));
        QObject::disconnect(request.data(), SIGNAL(readChannelFinished()), q, SLOT(_q_dataReadyReadBuffer()));

        request.setContentLength(reply->d_func()->requestDataBuffer.size());
        reply->d_func()->requestDataBuffer.seek(0);
        reply->d_func()->requestIsBuffering = false;
    }
}

// QHttpNetworkConnection

QHttpNetworkConnection::QHttpNetworkConnection(const QString &hostName, quint16 port, bool encrypt, QObject *parent)
    : QObject(*(new QHttpNetworkConnectionPrivate(hostName, port, encrypt)), parent)
{
    Q_D(QHttpNetworkConnection);
    d->init();
}

QHttpNetworkConnection::~QHttpNetworkConnection()
{
}

QString QHttpNetworkConnection::hostName() const
{
    Q_D(const QHttpNetworkConnection);
    return d->hostName;
}

quint16 QHttpNetworkConnection::port() const
{
    Q_D(const QHttpNetworkConnection);
    return d->port;
}

QHttpNetworkReply* QHttpNetworkConnection::sendRequest(const QHttpNetworkRequest &request)
{
    Q_D(QHttpNetworkConnection);
    return d->queueRequest(request);
}

void QHttpNetworkConnection::enableEncryption()
{
    Q_D(QHttpNetworkConnection);
    d->encrypt = true;
}

bool QHttpNetworkConnection::isEncrypted() const
{
    Q_D(const QHttpNetworkConnection);
    return d->encrypt;
}

void QHttpNetworkConnection::setProxyAuthentication(QAuthenticator *authenticator)
{
    Q_D(QHttpNetworkConnection);
    for (int i = 0; i < d->channelCount; ++i)
        d->channels[i].proxyAuthenticator = *authenticator;
}

void QHttpNetworkConnection::setAuthentication(const QString &domain, QAuthenticator *authenticator)
{
    Q_UNUSED(domain); // ### domain ?
    Q_D(QHttpNetworkConnection);
    for (int i = 0; i < d->channelCount; ++i)
        d->channels[i].authenticator = *authenticator;
}

#ifndef QT_NO_NETWORKPROXY
void QHttpNetworkConnection::setCacheProxy(const QNetworkProxy &networkProxy)
{
    Q_D(QHttpNetworkConnection);
    d->networkProxy = networkProxy;
    // update the authenticator
    if (!d->networkProxy.user().isEmpty()) {
        for (int i = 0; i < d->channelCount; ++i) {
            d->channels[i].proxyAuthenticator.setUser(d->networkProxy.user());
            d->channels[i].proxyAuthenticator.setPassword(d->networkProxy.password());
        }
    }
}

QNetworkProxy QHttpNetworkConnection::cacheProxy() const
{
    Q_D(const QHttpNetworkConnection);
    return d->networkProxy;
}

void QHttpNetworkConnection::setTransparentProxy(const QNetworkProxy &networkProxy)
{
    Q_D(QHttpNetworkConnection);
    for (int i = 0; i < d->channelCount; ++i)
        d->channels[i].socket->setProxy(networkProxy);
}

QNetworkProxy QHttpNetworkConnection::transparentProxy() const
{
    Q_D(const QHttpNetworkConnection);
    return d->channels[0].socket->proxy();
}
#endif

// QHttpNetworkRequest

QHttpNetworkRequest::QHttpNetworkRequest(const QUrl &url, Operation operation, Priority priority)
    : d(new QHttpNetworkRequestPrivate(operation, priority, url))
{
}

QHttpNetworkRequest::QHttpNetworkRequest(const QHttpNetworkRequest &other)
    : QHttpNetworkHeader(other), d(other.d)
{
}

QHttpNetworkRequest::~QHttpNetworkRequest()
{
}

QUrl QHttpNetworkRequest::url() const
{
    return d->url;
}
void QHttpNetworkRequest::setUrl(const QUrl &url)
{
    d->url = url;
}

qint64 QHttpNetworkRequest::contentLength() const
{
    return d->contentLength();
}

void QHttpNetworkRequest::setContentLength(qint64 length)
{
    d->setContentLength(length);
}

QList<QPair<QByteArray, QByteArray> > QHttpNetworkRequest::header() const
{
    return d->fields;
}

QByteArray QHttpNetworkRequest::headerField(const QByteArray &name, const QByteArray &defaultValue) const
{
    return d->headerField(name, defaultValue);
}

void QHttpNetworkRequest::setHeaderField(const QByteArray &name, const QByteArray &data)
{
    d->setHeaderField(name, data);
}

QHttpNetworkRequest &QHttpNetworkRequest::operator=(const QHttpNetworkRequest &other)
{
    d = other.d;
    return *this;
}

bool QHttpNetworkRequest::operator==(const QHttpNetworkRequest &other) const
{
    return d->operator==(*other.d);
}

QHttpNetworkRequest::Operation QHttpNetworkRequest::operation() const
{
    return d->operation;
}

void QHttpNetworkRequest::setOperation(Operation operation)
{
    d->operation = operation;
}

QHttpNetworkRequest::Priority QHttpNetworkRequest::priority() const
{
    return d->priority;
}

void QHttpNetworkRequest::setPriority(Priority priority)
{
    d->priority = priority;
}

QIODevice *QHttpNetworkRequest::data() const
{
    return d->data;
}

void QHttpNetworkRequest::setData(QIODevice *data)
{
    d->data = data;
}

int QHttpNetworkRequest::majorVersion() const
{
    return 1;
}

int QHttpNetworkRequest::minorVersion() const
{
    return 1;
}

// QHttpNetworkReply

QHttpNetworkReply::QHttpNetworkReply(const QUrl &url, QObject *parent)
    : QObject(*new QHttpNetworkReplyPrivate(url), parent)
{
}

QHttpNetworkReply::~QHttpNetworkReply()
{
    Q_D(QHttpNetworkReply);
    if (d->connection) {
        d->connection->d_func()->removeReply(this);
    }
}

QUrl QHttpNetworkReply::url() const
{
    return d_func()->url;
}
void QHttpNetworkReply::setUrl(const QUrl &url)
{
    Q_D(QHttpNetworkReply);
    d->url = url;
}

qint64 QHttpNetworkReply::contentLength() const
{
    return d_func()->contentLength();
}

void QHttpNetworkReply::setContentLength(qint64 length)
{
    Q_D(QHttpNetworkReply);
    d->setContentLength(length);
}

QList<QPair<QByteArray, QByteArray> > QHttpNetworkReply::header() const
{
    return d_func()->fields;
}

QByteArray QHttpNetworkReply::headerField(const QByteArray &name, const QByteArray &defaultValue) const
{
    return d_func()->headerField(name, defaultValue);
}

void QHttpNetworkReply::setHeaderField(const QByteArray &name, const QByteArray &data)
{
    Q_D(QHttpNetworkReply);
    d->setHeaderField(name, data);
}

void QHttpNetworkReply::parseHeader(const QByteArray &header)
{
    Q_D(QHttpNetworkReply);
    d->parseHeader(header);
}

QHttpNetworkRequest QHttpNetworkReply::request() const
{
    return d_func()->request;
}

void QHttpNetworkReply::setRequest(const QHttpNetworkRequest &request)
{
    Q_D(QHttpNetworkReply);
    d->request = request;
}

int QHttpNetworkReply::statusCode() const
{
    return d_func()->statusCode;
}

void QHttpNetworkReply::setStatusCode(int code)
{
    Q_D(QHttpNetworkReply);
    d->statusCode = code;
}

QString QHttpNetworkReply::errorString() const
{
    return d_func()->errorString;
}

QString QHttpNetworkReply::reasonPhrase() const
{
    return d_func()->reasonPhrase;
}

void QHttpNetworkReply::setErrorString(const QString &error)
{
    Q_D(QHttpNetworkReply);
    d->errorString = error;
}

int QHttpNetworkReply::majorVersion() const
{
    return d_func()->majorVersion;
}

int QHttpNetworkReply::minorVersion() const
{
    return d_func()->minorVersion;
}

qint64 QHttpNetworkReply::bytesAvailable() const
{
    Q_D(const QHttpNetworkReply);
    if (d->connection)
        return d->connection->d_func()->bytesAvailable(*this);
    else
        return -1;
}

QByteArray QHttpNetworkReply::read(qint64 maxSize)
{
    Q_D(QHttpNetworkReply);
    QByteArray data;
    if (d->connection)
        d->connection->d_func()->read(*this, data, maxSize, false);
    return data;
}

bool QHttpNetworkReply::isFinished() const
{
    return d_func()->state == QHttpNetworkReplyPrivate::AllDoneState;
}

// SSL support below
#ifndef QT_NO_OPENSSL
void QHttpNetworkConnectionPrivate::_q_encrypted()
{
    Q_Q(QHttpNetworkConnection);
    QAbstractSocket *socket = qobject_cast<QAbstractSocket*>(q->sender());
    if (!socket)
        return; // ### error
    channels[indexOf(socket)].state = IdleState;
    sendRequest(socket);
}

void QHttpNetworkConnectionPrivate::_q_sslErrors(const QList<QSslError> &errors)
{
    Q_Q(QHttpNetworkConnection);
    QAbstractSocket *socket = qobject_cast<QAbstractSocket*>(q->sender());
    if (!socket)
        return;
    //QNetworkReply::NetworkError errorCode = QNetworkReply::ProtocolFailure;
    emit q->sslErrors(errors);
}

QSslConfiguration QHttpNetworkConnectionPrivate::sslConfiguration(const QHttpNetworkReply &reply) const
{
    for (int i = 0; i < channelCount; ++i)
        if (channels[i].reply == &reply)
            return static_cast<QSslSocket *>(channels[0].socket)->sslConfiguration();
    return QSslConfiguration(); // pending or done request
}

void QHttpNetworkConnection::setSslConfiguration(const QSslConfiguration &config)
{
    Q_D(QHttpNetworkConnection);
    // set the config on all channels
    for (int i = 0; i < d->channelCount; ++i)
        static_cast<QSslSocket *>(d->channels[i].socket)->setSslConfiguration(config);
}

void QHttpNetworkConnection::ignoreSslErrors(int channel)
{
    Q_D(QHttpNetworkConnection);
    if (channel == -1) { // ignore for all channels
        for (int i = 0; i < d->channelCount; ++i) {
            static_cast<QSslSocket *>(d->channels[i].socket)->ignoreSslErrors();
            d->channels[i].ignoreSSLErrors = true;
        }

    } else {
        static_cast<QSslSocket *>(d->channels[channel].socket)->ignoreSslErrors();
        d->channels[channel].ignoreSSLErrors = true;
    }
}

QSslConfiguration QHttpNetworkReply::sslConfiguration() const
{
    Q_D(const QHttpNetworkReply);
    if (d->connection)
        return d->connection->d_func()->sslConfiguration(*this);
    return QSslConfiguration();
}

void QHttpNetworkReply::setSslConfiguration(const QSslConfiguration &config)
{
    Q_D(QHttpNetworkReply);
    if (d->connection)
        d->connection->setSslConfiguration(config);
}

void QHttpNetworkReply::ignoreSslErrors()
{
    Q_D(QHttpNetworkReply);
    if (d->connection)
        d->connection->ignoreSslErrors();
}
#endif //QT_NO_OPENSSL


QT_END_NAMESPACE

#include "moc_qhttpnetworkconnection_p.cpp"

#endif // QT_NO_HTTP
