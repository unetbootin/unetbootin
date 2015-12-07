/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the QtCore module of the Qt Toolkit.
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

#include "qdatastream.h"

#ifndef QT_NO_DATASTREAM
#include "qbuffer.h"
#include "qstring.h"
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>

QT_BEGIN_NAMESPACE

/*!
    \class QDataStream
    \reentrant
    \brief The QDataStream class provides serialization of binary data
    to a QIODevice.

    \ingroup io
    \mainclass

    A data stream is a binary stream of encoded information which is
    100% independent of the host computer's operating system, CPU or
    byte order. For example, a data stream that is written by a PC
    under Windows can be read by a Sun SPARC running Solaris.

    You can also use a data stream to read/write \l{raw}{raw
    unencoded binary data}. If you want a "parsing" input stream, see
    QTextStream.

    The QDataStream class implements the serialization of C++'s basic
    data types, like \c char, \c short, \c int, \c{char *}, etc.
    Serialization of more complex data is accomplished by breaking up
    the data into primitive units.

    A data stream cooperates closely with a QIODevice. A QIODevice
    represents an input/output medium one can read data from and write
    data to. The QFile class is an example of an I/O device.

    Example (write binary data to a stream):

    \snippet doc/src/snippets/code/src_corelib_io_qdatastream.cpp 0

    Example (read binary data from a stream):

    \snippet doc/src/snippets/code/src_corelib_io_qdatastream.cpp 1

    Each item written to the stream is written in a predefined binary
    format that varies depending on the item's type. Supported Qt
    types include QBrush, QColor, QDateTime, QFont, QPixmap, QString,
    QVariant and many others. For the complete list of all Qt types
    supporting data streaming see the \l{Format of the QDataStream
    operators}.

    For integers it is best to always cast to a Qt integer type for
    writing, and to read back into the same Qt integer type. This
    ensures that you get integers of the size you want and insulates
    you from compiler and platform differences.

    To take one example, a \c{char *} string is written as a 32-bit
    integer equal to the length of the string including the '\\0' byte,
    followed by all the characters of the string including the
    '\\0' byte. When reading a \c{char *} string, 4 bytes are read to
    create the 32-bit length value, then that many characters for the
    \c {char *} string including the '\\0' terminator are read.

    The initial I/O device is usually set in the constructor, but can be
    changed with setDevice(). If you've reached the end of the data
    (or if there is no I/O device set) atEnd() will return true.

    \section1 Versioning

    QDataStream's binary format has evolved since Qt 1.0, and is
    likely to continue evolving to reflect changes done in Qt. When
    inputting or outputting complex types, it's very important to
    make sure that the same version of the stream (version()) is used
    for reading and writing. If you need both forward and backward
    compatibility, you can hardcode the version number in the
    application:

    \snippet doc/src/snippets/code/src_corelib_io_qdatastream.cpp 2

    If you are producing a new binary data format, such as a file
    format for documents created by your application, you could use a
    QDataStream to write the data in a portable format. Typically, you
    would write a brief header containing a magic string and a version
    number to give yourself room for future expansion. For example:

    \snippet doc/src/snippets/code/src_corelib_io_qdatastream.cpp 3

    Then read it in with:

    \snippet doc/src/snippets/code/src_corelib_io_qdatastream.cpp 4

    You can select which byte order to use when serializing data. The
    default setting is big endian (MSB first). Changing it to little
    endian breaks the portability (unless the reader also changes to
    little endian). We recommend keeping this setting unless you have
    special requirements.

    \target raw
    \section1 Reading and writing raw binary data

    You may wish to read/write your own raw binary data to/from the
    data stream directly. Data may be read from the stream into a
    preallocated \c{char *} using readRawData(). Similarly data can be
    written to the stream using writeRawData(). Note that any
    encoding/decoding of the data must be done by you.

    A similar pair of functions is readBytes() and writeBytes(). These
    differ from their \e raw counterparts as follows: readBytes()
    reads a quint32 which is taken to be the length of the data to be
    read, then that number of bytes is read into the preallocated
    \c{char *}; writeBytes() writes a quint32 containing the length of the
    data, followed by the data. Note that any encoding/decoding of
    the data (apart from the length quint32) must be done by you.

    \target Serializing Qt Classes
    \section1 Reading and writing other Qt classes.

    In addition to the overloaded stream operators documented here,
    any Qt classes that you might want to serialize to a QDataStream
    will have appropriate stream operators declared as non-member of
    the class:

    \code
    QDataStream &operator<<(QDataStream &, const QXxx &);
    QDataStream &operator>>(QDataStream &, QXxx &);
    \endcode

    For example, here are the stream operators declared as non-members
    of the QImage class:

    \code
    QDataStream & operator<< (QDataStream& stream, const QImage& image);
    QDataStream & operator>> (QDataStream& stream, QImage& image);
    \endcode

    To see if your favorite Qt class has similar stream operators
    defined, check the \bold {Related Non-Members} section of the
    class's documentation page.

    \sa QTextStream QVariant
*/

/*!
    \enum QDataStream::ByteOrder

    The byte order used for reading/writing the data.

    \value BigEndian Most significant byte first (the default)
    \value LittleEndian Least significant byte first
*/

/*!
    \enum QDataStream::Status

    This enum describes the current status of the data stream.

    \value Ok               The data stream is operating normally.
    \value ReadPastEnd      The data stream has read past the end of the
                            data in the underlying device.
    \value ReadCorruptData  The data stream has read corrupt data.
*/

/*****************************************************************************
  QDataStream member functions
 *****************************************************************************/

#undef  CHECK_STREAM_PRECOND
#ifndef QT_NO_DEBUG
#define CHECK_STREAM_PRECOND(retVal) \
    if (!dev) { \
        qWarning("QDataStream: No device"); \
        return retVal; \
    }
#else
#define CHECK_STREAM_PRECOND(retVal) \
    if (!dev) { \
        return retVal; \
    }
#endif

enum {
    DefaultStreamVersion = QDataStream::Qt_4_5
};

// ### 5.0: when streaming invalid QVariants, just the type should
// be written, no "data" after it

/*!
    Constructs a data stream that has no I/O device.

    \sa setDevice()
*/

QDataStream::QDataStream()
{
    dev = 0;
    owndev = false;
    byteorder = BigEndian;
    ver = DefaultStreamVersion;
    noswap = QSysInfo::ByteOrder == QSysInfo::BigEndian;
    q_status = Ok;
}

/*!
    Constructs a data stream that uses the I/O device \a d.

    \warning If you use QSocket or QSocketDevice as the I/O device \a d
    for reading data, you must make sure that enough data is available
    on the socket for the operation to successfully proceed;
    QDataStream does not have any means to handle or recover from
    short-reads.

    \sa setDevice(), device()
*/

QDataStream::QDataStream(QIODevice *d)
{
    dev = d;                                // set device
    owndev = false;
    byteorder = BigEndian;                        // default byte order
    ver = DefaultStreamVersion;
    noswap = QSysInfo::ByteOrder == QSysInfo::BigEndian;
    q_status = Ok;
}

#ifdef QT3_SUPPORT
/*!
    \fn QDataStream::QDataStream(QByteArray *array, int mode)
    \compat

    Constructs a data stream that operates on the given \a array. The
    \a mode specifies how the byte array is to be used, and is
    usually either QIODevice::ReadOnly or QIODevice::WriteOnly.
*/
QDataStream::QDataStream(QByteArray *a, int mode)
{
    QBuffer *buf = new QBuffer(a);
#ifndef QT_NO_QOBJECT
    buf->blockSignals(true);
#endif
    buf->open(QIODevice::OpenMode(mode));
    dev = buf;
    owndev = true;
    byteorder = BigEndian;
    ver = DefaultStreamVersion;
    noswap = QSysInfo::ByteOrder == QSysInfo::BigEndian;
    q_status = Ok;
}
#endif

/*!
    \fn QDataStream::QDataStream(QByteArray *a, QIODevice::OpenMode mode)

    Constructs a data stream that operates on a byte array, \a a. The
    \a mode describes how the device is to be used.

    Alternatively, you can use QDataStream(const QByteArray &) if you
    just want to read from a byte array.

    Since QByteArray is not a QIODevice subclass, internally a QBuffer
    is created to wrap the byte array.
*/

QDataStream::QDataStream(QByteArray *a, QIODevice::OpenMode flags)
{
    QBuffer *buf = new QBuffer(a);
#ifndef QT_NO_QOBJECT
    buf->blockSignals(true);
#endif
    buf->open(flags);
    dev = buf;
    owndev = true;
    byteorder = BigEndian;
    ver = DefaultStreamVersion;
    noswap = QSysInfo::ByteOrder == QSysInfo::BigEndian;
    q_status = Ok;
}

/*!
    Constructs a read-only data stream that operates on byte array \a a.
    Use QDataStream(QByteArray*, int) if you want to write to a byte
    array.

    Since QByteArray is not a QIODevice subclass, internally a QBuffer
    is created to wrap the byte array.
*/
QDataStream::QDataStream(const QByteArray &a)
{
    QBuffer *buf = new QBuffer;
#ifndef QT_NO_QOBJECT
    buf->blockSignals(true);
#endif
    buf->setData(a);
    buf->open(QIODevice::ReadOnly);
    dev = buf;
    owndev = true;
    byteorder = BigEndian;
    ver = DefaultStreamVersion;
    noswap = QSysInfo::ByteOrder == QSysInfo::BigEndian;
    q_status = Ok;
}

/*!
    Destroys the data stream.

    The destructor will not affect the current I/O device, unless it is
    an internal I/O device (e.g. a QBuffer) processing a QByteArray
    passed in the \e constructor, in which case the internal I/O device
    is destroyed.
*/

QDataStream::~QDataStream()
{
    if (owndev)
        delete dev;
}


/*!
    \fn QIODevice *QDataStream::device() const

    Returns the I/O device currently set.

    \sa setDevice(), unsetDevice()
*/

/*!
    void QDataStream::setDevice(QIODevice *d)

    Sets the I/O device to \a d.

    \sa device(), unsetDevice()
*/

void QDataStream::setDevice(QIODevice *d)
{
    if (owndev) {
        delete dev;
        owndev = false;
    }
    dev = d;
}

/*!
    \obsolete
    Unsets the I/O device.
    Use setDevice(0) instead.
*/

void QDataStream::unsetDevice()
{
    setDevice(0);
}


/*!
    \fn bool QDataStream::atEnd() const

    Returns true if the I/O device has reached the end position (end of
    the stream or file) or if there is no I/O device set; otherwise
    returns false.

    \sa QIODevice::atEnd()
*/

bool QDataStream::atEnd() const
{
    return dev ? dev->atEnd() : true;
}

/*!
    Returns the status of the data stream.

    \sa Status setStatus() resetStatus()
*/

QDataStream::Status QDataStream::status() const
{
    return q_status;
}

/*!
    Resets the status of the data stream.

    \sa Status status() setStatus()
*/
void QDataStream::resetStatus()
{
    q_status = Ok;
}

/*!
    Sets the status of the data stream to the \a status given.

    \sa Status status() resetStatus()
*/
void QDataStream::setStatus(Status status)
{
    if (q_status == Ok)
        q_status = status;
}

/*!\fn bool QDataStream::eof() const

    Use atEnd() instead.
*/

/*!
    \fn int QDataStream::byteOrder() const

    Returns the current byte order setting -- either BigEndian or
    LittleEndian.

    \sa setByteOrder()
*/

/*!
    Sets the serialization byte order to \a bo.

    The \a bo parameter can be QDataStream::BigEndian or
    QDataStream::LittleEndian.

    The default setting is big endian. We recommend leaving this
    setting unless you have special requirements.

    \sa byteOrder()
*/

void QDataStream::setByteOrder(ByteOrder bo)
{
    byteorder = bo;
    if (QSysInfo::ByteOrder == QSysInfo::BigEndian)
        noswap = (byteorder == BigEndian);
    else
        noswap = (byteorder == LittleEndian);
}


/*!
    \fn bool QDataStream::isPrintableData() const

    In Qt 4, this function always returns false.

    \sa setPrintableData()
*/

/*!
    \fn void QDataStream::setPrintableData(bool enable)

    In Qt 3, this function enabled output in a human-readable
    format if \a enable was false.

    In Qt 4, QDataStream no longer provides a human-readable output.
    This function does nothing.
*/

/*!
    \enum QDataStream::Version

    This enum provides symbolic synonyms for the data serialization
    format version numbers.

    \value Qt_1_0 Version 1 (Qt 1.x)
    \value Qt_2_0 Version 2 (Qt 2.0)
    \value Qt_2_1 Version 3 (Qt 2.1, 2.2, 2.3)
    \value Qt_3_0 Version 4 (Qt 3.0)
    \value Qt_3_1 Version 5 (Qt 3.1, 3.2)
    \value Qt_3_3 Version 6 (Qt 3.3)
    \value Qt_4_0 Version 7 (Qt 4.0, Qt 4.1)
    \value Qt_4_1 Version 7 (Qt 4.0, Qt 4.1)
    \value Qt_4_2 Version 8 (Qt 4.2)
    \value Qt_4_3 Version 9 (Qt 4.3)
    \value Qt_4_4 Version 10 (Qt 4.4)
    \value Qt_4_5 Version 10 (Qt 4.5)
    \omitvalue Qt_4_6

    \sa setVersion(), version()
*/

/*!
    \fn int QDataStream::version() const

    Returns the version number of the data serialization format.

    \sa setVersion(), Version
*/

/*!
    \fn void QDataStream::setVersion(int v)

    Sets the version number of the data serialization format to \a v.

    You don't \e have to set a version if you are using the current
    version of Qt, but for your own custom binary formats we
    recommend that you do; see \l{Versioning} in the Detailed
    Description.

    In order to accommodate new functionality, the datastream
    serialization format of some Qt classes has changed in some
    versions of Qt. If you want to read data that was created by an
    earlier version of Qt, or write data that can be read by a
    program that was compiled with an earlier version of Qt, use this
    function to modify the serialization format used by QDataStream.

    \table
    \header \i Qt Version       \i QDataStream Version
    \row \i Qt 4.4                  \i 10
    \row \i Qt 4.3                  \i 9
    \row \i Qt 4.2                  \i 8
    \row \i Qt 4.0, 4.1            \i 7
    \row \i Qt 3.3                  \i 6
    \row \i Qt 3.1, 3.2             \i 5
    \row \i Qt 3.0                  \i 4
    \row \i Qt 2.1, 2.2, 2.3      \i 3
    \row \i Qt 2.0                  \i 2
    \row \i Qt 1.x                  \i 1
    \endtable

    The \l Version enum provides symbolic constants for the different
    versions of Qt. For example:

    \snippet doc/src/snippets/code/src_corelib_io_qdatastream.cpp 5

    \sa version(), Version
*/

/*****************************************************************************
  QDataStream read functions
 *****************************************************************************/

/*!
    \fn QDataStream &QDataStream::operator>>(quint8 &i)
    \overload

    Reads an unsigned byte from the stream into \a i, and returns a
    reference to the stream.
*/

/*!
    Reads a signed byte from the stream into \a i, and returns a
    reference to the stream.
*/

QDataStream &QDataStream::operator>>(qint8 &i)
{
    i = 0;
    CHECK_STREAM_PRECOND(*this)
    char c;
    if (!dev->getChar(&c))
        setStatus(ReadPastEnd);
    else
        i = qint8(c);
    return *this;
}


/*!
    \fn QDataStream &QDataStream::operator>>(quint16 &i)
    \overload

    Reads an unsigned 16-bit integer from the stream into \a i, and
    returns a reference to the stream.
*/

/*!
    \overload

    Reads a signed 16-bit integer from the stream into \a i, and
    returns a reference to the stream.
*/

QDataStream &QDataStream::operator>>(qint16 &i)
{
    i = 0;
    CHECK_STREAM_PRECOND(*this)
    if (noswap) {
        if (dev->read((char *)&i, 2) != 2) {
            i = 0;
            setStatus(ReadPastEnd);
        }
    } else {
        register uchar *p = (uchar *)(&i);
        char b[2];
        if (dev->read(b, 2) == 2) {
            *p++ = b[1];
            *p = b[0];
        } else {
            setStatus(ReadPastEnd);
        }
    }
    return *this;
}


/*!
    \fn QDataStream &QDataStream::operator>>(quint32 &i)
    \overload

    Reads an unsigned 32-bit integer from the stream into \a i, and
    returns a reference to the stream.
*/

/*!
    \overload

    Reads a signed 32-bit integer from the stream into \a i, and
    returns a reference to the stream.
*/

QDataStream &QDataStream::operator>>(qint32 &i)
{
    i = 0;
    CHECK_STREAM_PRECOND(*this)
    if (noswap) {
        if (dev->read((char *)&i, 4) != 4) {
            i = 0;
            setStatus(ReadPastEnd);
        }
    } else {                                        // swap bytes
        uchar *p = (uchar *)(&i);
        char b[4];
        if (dev->read(b, 4) == 4) {
            *p++ = b[3];
            *p++ = b[2];
            *p++ = b[1];
            *p   = b[0];
        } else {
            setStatus(ReadPastEnd);
        }
    }
    return *this;
}

/*!
    \fn QDataStream &QDataStream::operator>>(quint64 &i)
    \overload

    Reads an unsigned 64-bit integer from the stream, into \a i, and
    returns a reference to the stream.
*/

/*!
    \overload

    Reads a signed 64-bit integer from the stream into \a i, and
    returns a reference to the stream.
*/

QDataStream &QDataStream::operator>>(qint64 &i)
{
    i = qint64(0);
    CHECK_STREAM_PRECOND(*this)
    if (version() < 6) {
        quint32 i1, i2;
        *this >> i2 >> i1;
        i = ((quint64)i1 << 32) + i2;
    } else if (noswap) {                        // no conversion needed
        if (dev->read((char *)&i, 8) != 8) {
            i = qint64(0);
            setStatus(ReadPastEnd);
        }
    } else {                                        // swap bytes
        uchar *p = (uchar *)(&i);
        char b[8];
        if (dev->read(b, 8) == 8) {
            *p++ = b[7];
            *p++ = b[6];
            *p++ = b[5];
            *p++ = b[4];
            *p++ = b[3];
            *p++ = b[2];
            *p++ = b[1];
            *p   = b[0];
        } else {
            setStatus(ReadPastEnd);
        }
    }
    return *this;
}

/*!
    Reads a boolean value from the stream into \a i. Returns a
    reference to the stream.
*/
QDataStream &QDataStream::operator>>(bool &i)
{
    qint8 v;
    *this >> v;
    i = !!v;
    return *this;
}

/*!
    \overload

    Reads a 32-bit floating point number from the stream into \a f,
    using the standard IEEE 754 format. Returns a reference to the
    stream.
*/

QDataStream &QDataStream::operator>>(float &f)
{
    f = 0.0f;
    CHECK_STREAM_PRECOND(*this)
    if (noswap) {
        if (dev->read((char *)&f, 4) != 4) {
            f = 0.0f;
            setStatus(ReadPastEnd);
        }
    } else {                                        // swap bytes
        uchar *p = (uchar *)(&f);
        char b[4];
        if (dev->read(b, 4) == 4) {
            *p++ = b[3];
            *p++ = b[2];
            *p++ = b[1];
            *p = b[0];
        } else {
            setStatus(ReadPastEnd);
        }
    }
    return *this;
}

#if defined(Q_DOUBLE_FORMAT)
#define Q_DF(x) Q_DOUBLE_FORMAT[(x)] - '0'
#endif

/*!
    \overload

    Reads a 64-bit floating point number from the stream into \a f,
    using the standard IEEE 754 format. Returns a reference to the
    stream.
*/

QDataStream &QDataStream::operator>>(double &f)
{
    f = 0.0;
    CHECK_STREAM_PRECOND(*this)
#ifndef Q_DOUBLE_FORMAT
    if (noswap) {
        if (dev->read((char *)&f, 8) != 8) {
            f = 0.0;
            setStatus(ReadPastEnd);
        }
    } else {                                        // swap bytes
        register uchar *p = (uchar *)(&f);
        char b[8];
        if (dev->read(b, 8) == 8) {
            *p++ = b[7];
            *p++ = b[6];
            *p++ = b[5];
            *p++ = b[4];
            *p++ = b[3];
            *p++ = b[2];
            *p++ = b[1];
            *p   = b[0];
        } else {
            setStatus(ReadPastEnd);
        }
    }
#else
    //non-standard floating point format
    register uchar *p = (uchar *)(&f);
    char b[8];
    if (dev->read(b, 8) == 8) {
        if (noswap) {
            *p++ = b[Q_DF(0)];
            *p++ = b[Q_DF(1)];
            *p++ = b[Q_DF(2)];
            *p++ = b[Q_DF(3)];
            *p++ = b[Q_DF(4)];
            *p++ = b[Q_DF(5)];
            *p++ = b[Q_DF(6)];
            *p = b[Q_DF(7)];
        } else {
            *p++ = b[Q_DF(7)];
            *p++ = b[Q_DF(6)];
            *p++ = b[Q_DF(5)];
            *p++ = b[Q_DF(4)];
            *p++ = b[Q_DF(3)];
            *p++ = b[Q_DF(2)];
            *p++ = b[Q_DF(1)];
            *p = b[Q_DF(0)];
        }
    } else {
        setStatus(ReadPastEnd);
    }
#endif
    return *this;
}


/*!
    \overload

    Reads the '\0'-terminated string \a s from the stream and returns
    a reference to the stream.

    Space for the string is allocated using \c new -- the caller must
    destroy it with \c{delete[]}.
*/

QDataStream &QDataStream::operator>>(char *&s)
{
    uint len = 0;
    return readBytes(s, len);
}


/*!
    Reads the buffer \a s from the stream and returns a reference to
    the stream.

    The buffer \a s is allocated using \c new. Destroy it with the \c
    delete[] operator.

    The \a l parameter is set to the length of the buffer. If the
    string read is empty, \a l is set to 0 and \a s is set to
    a null pointer.

    The serialization format is a quint32 length specifier first,
    then \a l bytes of data.

    \sa readRawData(), writeBytes()
*/

QDataStream &QDataStream::readBytes(char *&s, uint &l)
{
    s = 0;
    l = 0;
    CHECK_STREAM_PRECOND(*this)

    quint32 len;
    *this >> len;
    if (len == 0)
        return *this;

    const quint32 Step = 1024 * 1024;
    quint32 allocated = 0;
    char *prevBuf = 0;
    char *curBuf = 0;

    do {
        int blockSize = qMin(Step, len - allocated);
        prevBuf = curBuf;
        curBuf = new char[allocated + blockSize + 1];
        if (prevBuf) {
            memcpy(curBuf, prevBuf, allocated);
            delete [] prevBuf;
        }
        if (dev->read(curBuf + allocated, blockSize) != blockSize) {
            delete [] curBuf;
            setStatus(ReadPastEnd);
            return *this;
        }
        allocated += blockSize;
    } while (allocated < len);

    s = curBuf;
    s[len] = '\0';
    l = (uint)len;
    return *this;
}

/*!
    Reads at most \a len bytes from the stream into \a s and returns the number of
    bytes read. If an error occurs, this function returns -1.

    The buffer \a s must be preallocated. The data is \e not encoded.

    \sa readBytes(), QIODevice::read(), writeRawData()
*/

int QDataStream::readRawData(char *s, int len)
{
    CHECK_STREAM_PRECOND(-1)
    return dev->read(s, len);
}


/*****************************************************************************
  QDataStream write functions
 *****************************************************************************/


/*!
    \fn QDataStream &QDataStream::operator<<(quint8 i)
    \overload

    Writes an unsigned byte, \a i, to the stream and returns a
    reference to the stream.
*/

/*!
    Writes a signed byte, \a i, to the stream and returns a reference
    to the stream.
*/

QDataStream &QDataStream::operator<<(qint8 i)
{
    CHECK_STREAM_PRECOND(*this)
    dev->putChar(i);
    return *this;
}


/*!
    \fn QDataStream &QDataStream::operator<<(quint16 i)
    \overload

    Writes an unsigned 16-bit integer, \a i, to the stream and returns
    a reference to the stream.
*/

/*!
    \overload

    Writes a signed 16-bit integer, \a i, to the stream and returns a
    reference to the stream.
*/

QDataStream &QDataStream::operator<<(qint16 i)
{
    CHECK_STREAM_PRECOND(*this)
    if (noswap) {
        dev->write((char *)&i, sizeof(qint16));
    } else {                                        // swap bytes
        register uchar *p = (uchar *)(&i);
        char b[2];
        b[1] = *p++;
        b[0] = *p;
        dev->write(b, 2);
    }
    return *this;
}

/*!
    \overload

    Writes a signed 32-bit integer, \a i, to the stream and returns a
    reference to the stream.
*/

QDataStream &QDataStream::operator<<(qint32 i)
{
    CHECK_STREAM_PRECOND(*this)
    if (noswap) {
        dev->write((char *)&i, sizeof(qint32));
    } else {                                        // swap bytes
        register uchar *p = (uchar *)(&i);
        char b[4];
        b[3] = *p++;
        b[2] = *p++;
        b[1] = *p++;
        b[0] = *p;
        dev->write(b, 4);
    }
    return *this;
}

/*!
    \fn QDataStream &QDataStream::operator<<(quint64 i)
    \overload

    Writes an unsigned 64-bit integer, \a i, to the stream and returns a
    reference to the stream.
*/

/*!
    \overload

    Writes a signed 64-bit integer, \a i, to the stream and returns a
    reference to the stream.
*/

QDataStream &QDataStream::operator<<(qint64 i)
{
    CHECK_STREAM_PRECOND(*this)
    if (version() < 6) {
	quint32 i1 = i & 0xffffffff;
	quint32 i2 = i >> 32;
	*this << i2 << i1;
    } else if (noswap) {                        // no conversion needed
        dev->write((char *)&i, sizeof(qint64));
    } else {                                        // swap bytes
        register uchar *p = (uchar *)(&i);
        char b[8];
        b[7] = *p++;
        b[6] = *p++;
        b[5] = *p++;
        b[4] = *p++;
        b[3] = *p++;
        b[2] = *p++;
        b[1] = *p++;
        b[0] = *p;
        dev->write(b, 8);
    }
    return *this;
}

/*!
    \fn QDataStream &QDataStream::operator<<(quint32 i)
    \overload

    Writes an unsigned integer, \a i, to the stream as a 32-bit
    unsigned integer (quint32). Returns a reference to the stream.
*/

/*!
    Writes a boolean value, \a i, to the stream. Returns a reference
    to the stream.
*/

QDataStream &QDataStream::operator<<(bool i)
{
    CHECK_STREAM_PRECOND(*this)
    dev->putChar(qint8(i));
    return *this;
}

/*!
    \overload

    Writes a 32-bit floating point number, \a f, to the stream using
    the standard IEEE 754 format. Returns a reference to the stream.
*/

QDataStream &QDataStream::operator<<(float f)
{
    CHECK_STREAM_PRECOND(*this)
    float g = f;                                // fixes float-on-stack problem
    if (noswap) {                                // no conversion needed
        dev->write((char *)&g, sizeof(float));
    } else {                                // swap bytes
        register uchar *p = (uchar *)(&g);
        char b[4];
        b[3] = *p++;
        b[2] = *p++;
        b[1] = *p++;
        b[0] = *p;
        dev->write(b, 4);
    }
    return *this;
}


/*!
    \overload

    Writes a 64-bit floating point number, \a f, to the stream using
    the standard IEEE 754 format. Returns a reference to the stream.
*/

QDataStream &QDataStream::operator<<(double f)
{
    CHECK_STREAM_PRECOND(*this)
#ifndef Q_DOUBLE_FORMAT
    if (noswap) {
        dev->write((char *)&f, sizeof(double));
    } else {
        register uchar *p = (uchar *)(&f);
        char b[8];
        b[7] = *p++;
        b[6] = *p++;
        b[5] = *p++;
        b[4] = *p++;
        b[3] = *p++;
        b[2] = *p++;
        b[1] = *p++;
        b[0] = *p;
        dev->write(b, 8);
    }
#else
    register uchar *p = (uchar *)(&f);
    char b[8];
    if (noswap) {
        b[Q_DF(0)] = *p++;
        b[Q_DF(1)] = *p++;
        b[Q_DF(2)] = *p++;
        b[Q_DF(3)] = *p++;
        b[Q_DF(4)] = *p++;
        b[Q_DF(5)] = *p++;
        b[Q_DF(6)] = *p++;
        b[Q_DF(7)] = *p;
    } else {
        b[Q_DF(7)] = *p++;
        b[Q_DF(6)] = *p++;
        b[Q_DF(5)] = *p++;
        b[Q_DF(4)] = *p++;
        b[Q_DF(3)] = *p++;
        b[Q_DF(2)] = *p++;
        b[Q_DF(1)] = *p++;
        b[Q_DF(0)] = *p;
    }
    dev->write(b, 8);
#endif
    return *this;
}


/*!
    \overload

    Writes the '\0'-terminated string \a s to the stream and returns a
    reference to the stream.

    The string is serialized using writeBytes().
*/

QDataStream &QDataStream::operator<<(const char *s)
{
    if (!s) {
        *this << (quint32)0;
        return *this;
    }
    uint len = qstrlen(s) + 1;                        // also write null terminator
    *this << (quint32)len;                        // write length specifier
    writeRawData(s, len);
    return *this;
}


/*!
    Writes the length specifier \a len and the buffer \a s to the
    stream and returns a reference to the stream.

    The \a len is serialized as a quint32, followed by \a len bytes
    from \a s. Note that the data is \e not encoded.

    \sa writeRawData(), readBytes()
*/

QDataStream &QDataStream::writeBytes(const char *s, uint len)
{
    CHECK_STREAM_PRECOND(*this)
    *this << (quint32)len;                        // write length specifier
    if (len)
        writeRawData(s, len);
    return *this;
}


/*!
    Writes \a len bytes from \a s to the stream. Returns the
    number of bytes actually written, or -1 on error.
    The data is \e not encoded.

    \sa writeBytes(), QIODevice::write(), readRawData()
*/

int QDataStream::writeRawData(const char *s, int len)
{
    CHECK_STREAM_PRECOND(-1)
    return dev->write(s, len);
}

/*!
    \since 4.1

    Skips \a len bytes from the device. Returns the number of bytes
    actually skipped, or -1 on error.

    This is equivalent to calling readRawData() on a buffer of length
    \a len and ignoring the buffer.

    \sa QIODevice::seek()
*/
int QDataStream::skipRawData(int len)
{
    CHECK_STREAM_PRECOND(-1)

    if (dev->isSequential()) {
        char buf[4096];
        int sumRead = 0;

        while (len > 0) {
            int blockSize = qMin(len, (int)sizeof(buf));
            int n = dev->read(buf, blockSize);
            if (n == -1)
                return -1;
            if (n == 0)
                return sumRead;

            sumRead += n;
            len -= blockSize;
        }
        return sumRead;
    } else {
        qint64 pos = dev->pos();
        qint64 size = dev->size();
        if (pos + len > size)
            len = size - pos;
        if (!dev->seek(pos + len))
            return -1;
        return len;
    }
}

#ifdef QT3_SUPPORT
/*!
    \fn QDataStream &QDataStream::readRawBytes(char *str, uint len)

    Use readRawData() instead.
*/

/*!
    \fn QDataStream &QDataStream::writeRawBytes(const char *str, uint len)

    Use writeRawData() instead.
*/
#endif

QT_END_NAMESPACE

#endif // QT_NO_DATASTREAM
