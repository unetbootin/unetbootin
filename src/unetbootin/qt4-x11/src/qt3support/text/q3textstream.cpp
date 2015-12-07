/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the Qt3Support module of the Qt Toolkit.
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

#include "q3textstream.h"
#include <qdebug.h>

#ifndef QT_NO_TEXTSTREAM
#include "qtextcodec.h"
#include "qregexp.h"
#include "qbuffer.h"
#include "qfile.h"
#include "q3cstring.h"
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#ifndef Q_OS_WINCE
#include <locale.h>
#endif

#if defined(Q_OS_WIN32)
#include "qt_windows.h"
#endif

QT_BEGIN_NAMESPACE

#ifndef QT_NO_TEXTCODEC
static void resetCodecConverterState(QTextCodec::ConverterState *state) {
    state->flags = QTextCodec::DefaultConversion;
    state->remainingChars = state->invalidChars =
           state->state_data[0] = state->state_data[1] = state->state_data[2] = 0;
    if (state->d) qFree(state->d);
    state->d = 0;
}
#endif

/*!
    \class Q3TextStream
    \compat
    \reentrant
    \brief The Q3TextStream class provides basic functions for reading
    and writing text using a QIODevice.

    The text stream class has a functional interface that is very
    similar to that of the standard C++ iostream class.

    Qt provides several global functions similar to the ones in iostream:
    \table
    \header \i Function \i Meaning
    \row \i bin \i sets the Q3TextStream to read/write binary numbers
    \row \i oct \i sets the Q3TextStream to read/write octal numbers
    \row \i dec \i sets the Q3TextStream to read/write decimal numbers
    \row \i hex \i sets the Q3TextStream to read/write hexadecimal numbers
    \row \i endl \i forces a line break
    \row \i flush \i forces the QIODevice to flush any buffered data
    \row \i ws \i eats any available whitespace (on input)
    \row \i reset \i resets the Q3TextStream to its default mode (see reset())
    \row \i qSetW(int) \i sets the \link width() field width \endlink
    to the given argument
    \row \i qSetFill(int) \i sets the \link fill() fill character
    \endlink to the given argument
    \row \i qSetPrecision(int) \i sets the \link precision() precision
    \endlink to the given argument
    \endtable

    \warning By default Q3TextStream will automatically detect whether
    integers in the stream are in decimal, octal, hexadecimal or
    binary format when reading from the stream. In particular, a
    leading '0' signifies octal, i.e. the sequence "0100" will be
    interpreted as 64.

    The Q3TextStream class reads and writes text; it is not appropriate
    for dealing with binary data (but QDataStream is).

    By default, output of Unicode text (i.e. QString) is done using
    the local 8-bit encoding. This can be changed using the
    setEncoding() method. For input, the Q3TextStream will auto-detect
    standard Unicode "byte order marked" text files; otherwise the
    local 8-bit encoding is used.

    The QIODevice is set in the constructor, or later using
    setDevice(). If the end of the input is reached atEnd() returns
    TRUE. Data can be read into variables of the appropriate type
    using the operator>>() overloads, or read in its entirety into a
    single string using read(), or read a line at a time using
    readLine(). Whitespace can be skipped over using skipWhiteSpace().
    You can set flags for the stream using flags() or setf(). The
    stream also supports width(), precision() and fill(); use reset()
    to reset the defaults.

    \sa QDataStream
*/

/*!
    \enum Q3TextStream::Encoding

    \value Locale
    \value Latin1
    \value Unicode
    \value UnicodeNetworkOrder
    \value UnicodeReverse
    \value RawUnicode
    \value UnicodeUTF8

    See setEncoding() for an explanation of the encodings.
*/

/*
  \class QTSManip
  \internal
*/

#if defined(QT_CHECK_STATE)
#undef  CHECK_STREAM_PRECOND
#define CHECK_STREAM_PRECOND  if ( !dev ) {				\
				qWarning( "Q3TextStream: No device" );	\
				return *this; }
#else
#define CHECK_STREAM_PRECOND
#endif


#define I_SHORT		0x0010
#define I_INT		0x0020
#define I_LONG		0x0030
#define I_TYPE_MASK	0x00f0

#define I_BASE_2	Q3TextStream::bin
#define I_BASE_8	Q3TextStream::oct
#define I_BASE_10	Q3TextStream::dec
#define I_BASE_16	Q3TextStream::hex
#define I_BASE_MASK	(Q3TextStream::bin | Q3TextStream::oct | Q3TextStream::dec | Q3TextStream::hex)

#define I_SIGNED	0x0100
#define I_UNSIGNED	0x0200
#define I_SIGN_MASK	0x0f00


static const QChar QEOF = QChar((ushort)0xffff); //guaranteed not to be a character.
static const uint getline_buf_size = 256; // bufsize used by ts_getline()

const int Q3TextStream::basefield   = I_BASE_MASK;
const int Q3TextStream::adjustfield = ( Q3TextStream::left |
				       Q3TextStream::right |
				       Q3TextStream::internal );
const int Q3TextStream::floatfield  = ( Q3TextStream::scientific |
				       Q3TextStream::fixed );


class Q3TextStreamPrivate {
public:
#ifndef QT_NO_TEXTCODEC
    Q3TextStreamPrivate()
	: sourceType( NotSet ) { }
    ~Q3TextStreamPrivate() {
    }
#else
    Q3TextStreamPrivate() : sourceType( NotSet ) { }
    ~Q3TextStreamPrivate() { }
#endif
    QString ungetcBuf;

    enum SourceType { NotSet, IODevice, String, ByteArray, File };
    SourceType sourceType;
};


// skips whitespace and returns the first non-whitespace character
QChar Q3TextStream::eat_ws()
{
    QChar c;
    do { c = ts_getc(); } while ( c != QEOF && ts_isspace(c) );
    return c;
}

void Q3TextStream::init()
{
    // ### ungetcBuf = QEOF;
    dev = 0;
    owndev = FALSE;
    mapper = 0;
#ifndef QT_NO_TEXTCODEC
    resetCodecConverterState(&mapperReadState);
    resetCodecConverterState(&mapperWriteState);
#endif
    d = new Q3TextStreamPrivate;
    doUnicodeHeader = TRUE; // autodetect
    latin1 = TRUE; // should use locale?
    internalOrder = QChar::networkOrdered();
    networkOrder = TRUE;
}

/*!
    Constructs a data stream that has no IO device.
*/

Q3TextStream::Q3TextStream()
{
    init();
    setEncoding( Locale );
    reset();
    d->sourceType = Q3TextStreamPrivate::NotSet;
}

/*!
    Constructs a text stream that uses the IO device \a iod.
*/

Q3TextStream::Q3TextStream( QIODevice *iod )
{
    init();
    setEncoding( Locale );
    dev = iod;
    reset();
    d->sourceType = Q3TextStreamPrivate::IODevice;
}

// TODO: use special-case handling of this case in Q3TextStream, and
//	 simplify this class to only deal with QChar or QString data.
class QStringBuffer : public QIODevice {
public:
    QStringBuffer( QString* str );
    ~QStringBuffer();
    bool  open( OpenMode m );
    void  close();
    qint64 size() const;

protected:
    qint64 readData( char *p, qint64 len );
    qint64 writeData( const char *p, qint64 len );

    QString* s;

private:
    QStringBuffer( const QStringBuffer & );
    QStringBuffer &operator=( const QStringBuffer & );
};


QStringBuffer::QStringBuffer( QString* str )
{
    s = str;
}

QStringBuffer::~QStringBuffer()
{
}


bool QStringBuffer::open( OpenMode m )
{
    if ( !s ) {
#if defined(QT_CHECK_STATE)
	qWarning( "QStringBuffer::open: No string" );
#endif
	return FALSE;
    }
    if ( isOpen() ) {
#if defined(QT_CHECK_STATE)
	qWarning( "QStringBuffer::open: Buffer already open" );
#endif
	return FALSE;
    }
    setOpenMode( m );
    if ( m & QIODevice::Truncate )
	s->truncate( 0 );

    if ( m & QIODevice::Append ) {
	seek(s->length()*sizeof(QChar));
    } else {
	seek(0);
    }
    return TRUE;
}

void QStringBuffer::close()
{
    if ( isOpen() ) {
	seek(0);
        QIODevice::close();
    }
}

qint64 QStringBuffer::size() const
{
    return s ? s->length()*sizeof(QChar) : 0;
}

qint64 QStringBuffer::readData( char *p, qint64 len )
{
#if defined(QT_CHECK_STATE)
    Q_CHECK_PTR( p );
    if ( !isOpen() ) {
	qWarning( "QStringBuffer::readBlock: Buffer not open" );
	return qint64(-1);
    }
    if ( !isReadable() ) {
	qWarning( "QStringBuffer::readBlock: Read operation not permitted" );
	return qint64(-1);
    }
#endif
    if ( pos() + len > qint64(s->length()*sizeof(QChar)) ) {
	// overflow
	if ( pos() >= qint64(s->length()*sizeof(QChar)) ) {
	    return -1;
	} else {
	    len = s->length()*2 - pos();
	}
    }
    memcpy( p, ((const char*)(s->unicode()))+pos(), len );
    return len;
}

qint64 QStringBuffer::writeData( const char *p, qint64 len )
{
#if defined(QT_CHECK_NULL)
    if ( p == 0 && len != 0 )
	qWarning( "QStringBuffer::writeBlock: Null pointer error" );
#endif
#if defined(QT_CHECK_STATE)
    if ( !isOpen() ) {
	qWarning( "QStringBuffer::writeBlock: Buffer not open" );
	return -1;
    }
    if ( !isWritable() ) {
	qWarning( "QStringBuffer::writeBlock: Write operation not permitted" );
	return -1;
    }
    if ( pos()&1 ) {
	qWarning( "QStringBuffer::writeBlock: non-even index - non Unicode" );
	return -1;
    }
    if ( len&1 ) {
	qWarning( "QStringBuffer::writeBlock: non-even length - non Unicode" );
	return -1;
    }
#endif
    s->replace(pos()/2, len/2, (QChar*)p, len/2);
    return len;
}

/*!
    Constructs a text stream that operates on the Unicode QString, \a
    str, through an internal device. The \a filemode argument is
    passed to the device's open() function; see \l{QIODevice::mode()}.

    If you set an encoding or codec with setEncoding() or setCodec(),
    this setting is ignored for text streams that operate on QString.

    Example:
    \snippet doc/src/snippets/code/src_qt3support_text_q3textstream.cpp 0

    Writing data to the text stream will modify the contents of the
    string. The string will be expanded when data is written beyond
    the end of the string. Note that the string will not be truncated:
    \snippet doc/src/snippets/code/src_qt3support_text_q3textstream.cpp 1

    Note that because QString is Unicode, you should not use
    readRawBytes() or writeRawBytes() on such a stream.
*/

Q3TextStream::Q3TextStream( QString* str, int filemode )
{
    // TODO: optimize for this case as it becomes more common
    //        (see QStringBuffer above)
    init();
    dev = new QStringBuffer( str );
    ((QStringBuffer *)dev)->open( QIODevice::OpenMode(filemode) );
    owndev = TRUE;
    setEncoding(RawUnicode);
    reset();
    d->sourceType = Q3TextStreamPrivate::String;
}

/*! \obsolete

  This constructor is equivalent to the constructor taking a QString*
  parameter.
*/

Q3TextStream::Q3TextStream( QString& str, int filemode )
{
    init();
    dev = new QStringBuffer( &str );
    ((QStringBuffer *)dev)->open( QIODevice::OpenMode(filemode) );
    owndev = TRUE;
    setEncoding(RawUnicode);
    reset();
    d->sourceType = Q3TextStreamPrivate::String;
}

/*!
    Constructs a text stream that operates on the byte array, \a a,
    through an internal QBuffer device. The \a mode argument is passed
    to the device's open() function; see \l{QIODevice::mode()}.

    Example:
    \snippet doc/src/snippets/code/src_qt3support_text_q3textstream.cpp 2

    Writing data to the text stream will modify the contents of the
    array. The array will be expanded when data is written beyond the
    end of the string.

    Same example, using a QBuffer:
    \snippet doc/src/snippets/code/src_qt3support_text_q3textstream.cpp 3
*/

Q3TextStream::Q3TextStream( QByteArray &a, int mode )
{
    init();
    QBuffer *buffer = new QBuffer;
    buffer->setBuffer( &a );
    buffer->open( QIODevice::OpenMode(mode) );
    dev = buffer;
    owndev = TRUE;
    setEncoding( Latin1 ); //### Locale???
    reset();
    d->sourceType = Q3TextStreamPrivate::ByteArray;
}

/*!
    Constructs a text stream that operates on an existing file handle
    \a fh through an internal QFile device. The \a mode argument is
    passed to the device's open() function; see \l{QIODevice::mode()}.

    Note that if you create a Q3TextStream \c cout or another name that
    is also used for another variable of a different type, some
    linkers may confuse the two variables, which will often cause
    crashes.
*/

Q3TextStream::Q3TextStream( FILE *fh, int mode )
{
    init();
    setEncoding( Locale ); //###
    dev = new QFile;
    ((QFile *)dev)->open( QIODevice::OpenMode(mode), fh );
    owndev = TRUE;
    reset();
    d->sourceType = Q3TextStreamPrivate::File;
}

/*!
    Destroys the text stream.

    The destructor does not affect the current IO device.
*/

Q3TextStream::~Q3TextStream()
{
    if ( owndev )
	delete dev;
    delete d;
}

/*!
    \since 4.2

    Positions the read pointer at the first non-whitespace character.
*/
void Q3TextStream::skipWhiteSpace()
{
    ts_ungetc( eat_ws() );
}


/*!
    Tries to read \a len characters from the stream and stores them in
    \a buf. Returns the number of characters really read.

    \warning There will no QEOF appended if the read reaches the end
    of the file. EOF is reached when the return value does not equal
    \a len.
*/
uint Q3TextStream::ts_getbuf( QChar* buf, uint len )
{
    if ( len < 1 )
	return 0;

    uint rnum = 0;   // the number of QChars really read

    if ( d && d->ungetcBuf.length() ) {
	while ( rnum < len && rnum < uint(d->ungetcBuf.length()) ) {
	    *buf = d->ungetcBuf.constref( rnum );
	    buf++;
	    rnum++;
	}
	d->ungetcBuf = d->ungetcBuf.mid( rnum );
	if ( rnum >= len )
	    return rnum;
    }

    // we use dev->ungetch() for one of the bytes of the unicode
    // byte-order mark, but a local unget hack for the other byte:
    int ungetHack = EOF;

    if ( doUnicodeHeader ) {
	doUnicodeHeader = FALSE; // only at the top
	int c1 = dev->getch();
	if ( c1 == EOF )
	    return rnum;
	int c2 = dev->getch();
	if ( c1 == 0xfe && c2 == 0xff ) {
	    mapper = 0;
	    latin1 = FALSE;
	    internalOrder = QChar::networkOrdered();
	    networkOrder = TRUE;
	} else if ( c1 == 0xff && c2 == 0xfe ) {
	    mapper = 0;
	    latin1 = FALSE;
	    internalOrder = !QChar::networkOrdered();
	    networkOrder = FALSE;
	} else {
	    if ( c2 != EOF ) {
	 	dev->ungetch( c2 );
		ungetHack = c1;
	    } else {
		/*
		  A small bug might hide here. If only the first byte
		  of a file has made it so far, and that first byte
		  is half of the byte-order mark, then the utfness
		  will not be detected.
		*/
	 	dev->ungetch( c1 );
	    }
	}
    }

#ifndef QT_NO_TEXTCODEC
    if ( mapper ) {
	bool shortRead = FALSE;
        while( rnum < len ) {
	    QString s;
	    bool readBlock = !( len == 1+rnum );
	    for (;;) {
		// for efficiency: normally read a whole block
		if ( readBlock ) {
		    // guess buffersize; this may be wrong (too small or too
		    // big). But we can handle this (either iterate reading
		    // or use ungetcBuf).
		    // Note that this might cause problems for codecs where
		    // one byte can result in >1 Unicode Characters if bytes
		    // are written to the stream in the meantime (loss of
		    // synchronicity).
		    uint rlen = len - rnum;
		    char *cbuf = new char[ rlen ];
		    if ( ungetHack != EOF ) {
			rlen = 1+dev->readBlock( cbuf+1, rlen-1 );
			cbuf[0] = (char)ungetHack;
			ungetHack = EOF;
		    } else {
			rlen = dev->readBlock( cbuf, rlen );
		    }
		    s += mapper->toUnicode( cbuf, rlen, &mapperWriteState );
		    delete[] cbuf;
		    // use buffered reading only for the first time, because we
		    // have to get the stream synchronous again (this is easier
		    // with single character reading)
		    readBlock = FALSE;
		}
		// get stream (and codec) in sync
		int c;
		if ( ungetHack == EOF ) {
		    c = dev->getch();
		} else {
		    c = ungetHack;
		    ungetHack = EOF;
		}
		if ( c == EOF ) {
		    shortRead = TRUE;
		    break;
		}
		char b = c;
		uint lengthBefore = s.length();
		s += mapper->toUnicode( &b, 1, &mapperWriteState );

		if ( uint(s.length()) > lengthBefore )
		    break; // it seems we are in sync now
	    }
	    uint i = 0;
	    uint end = QMIN( len-rnum, uint(s.length()) );
	    while( i < end ) {
		*buf = s.constref(i++);
		buf++;
	    }
	    rnum += end;
	    if ( uint(s.length()) > i ) {
		// could be = but append is clearer
		d->ungetcBuf.append( s.mid( i ) );
	    }
	    if ( shortRead )
		return rnum;
	}
    } else
#endif
    if ( latin1 ) {
	if ( len == 1+rnum ) {
	    // use this method for one character because it is more efficient
	    // (arnt doubts whether it makes a difference, but lets it stand)
	    int c = (ungetHack == EOF) ? dev->getch() : ungetHack;
	    if ( c != EOF ) {
		*buf = QLatin1Char((char)c);
		buf++;
		rnum++;
	    }
	} else {
	    if ( ungetHack != EOF ) {
		*buf = QLatin1Char((char)ungetHack);
		buf++;
		rnum++;
		ungetHack = EOF;
	    }
	    char *cbuf = new char[len - rnum];
	    while ( !dev->atEnd() && rnum < len ) {
		uint rlen = len - rnum;
		rlen = dev->readBlock( cbuf, rlen );
		char *it = cbuf;
		char *end = cbuf + rlen;
		while ( it < end ) {
		    *buf = QLatin1Char(*it);
		    buf++;
		    it++;
		}
		rnum += rlen;
	    }
	    delete[] cbuf;
	}
    } else { // UCS-2 or UTF-16
	if ( len == 1+rnum ) {
	    int c1 = (ungetHack == EOF) ? dev->getch() : ungetHack;


            if ( c1 == EOF )
		return rnum;
	    int c2 = dev->getch();


            if ( c2 == EOF )
		return rnum;

	    if ( networkOrder ) {
		*buf = QChar( c2, c1 );
	    } else {
		*buf = QChar( c1, c2 );
	    }
	    buf++;
	    rnum++;
	} else {
	    char *cbuf = new char[ 2*( len - rnum ) ]; // for paranoids: overflow possible
	    while ( !dev->atEnd() && rnum < len ) {
		uint rlen = 2 * ( len-rnum );
		if ( ungetHack != EOF ) {
		    rlen = 1+dev->readBlock( cbuf+1, rlen-1 );
		    cbuf[0] = (char)ungetHack;
		    ungetHack = EOF;
		} else {
		    rlen = dev->readBlock( cbuf, rlen );
		}
		// We can't use an odd number of bytes, so put it back. But
		// do it only if we are capable of reading more -- normally
		// there should not be an odd number, but the file might be
		// truncated or not in UTF-16...
		if ( (rlen & 1) == 1 )
		    if ( !dev->atEnd() )
			dev->ungetch( cbuf[--rlen] );
		uint i = 0;
		if ( networkOrder ) {
		    while( i < rlen ) {
			*buf = QChar( cbuf[i+1], cbuf[i] );
			buf++;
			i+=2;
		    }
		} else {
		    while( i < rlen ) {
			*buf = QChar( cbuf[i], cbuf[i+1] );
			buf++;
			i+=2;
		    }
		}
		rnum += i/2;
	    }
	    delete[] cbuf;
	}
    }
    return rnum;
}

/*!
    Tries to read one line, but at most len characters from the stream
    and stores them in \a buf.

    Returns the number of characters really read. Newlines are not
    stripped.

    There will be a QEOF appended if the read reaches the end of file;
    this is different to ts_getbuf().

    This function works only if a newline (as byte) is also a newline
    (as resulting character) since it uses QIODevice::readLine(). So
    use it only for such codecs where this is true!

    This function is (almost) a no-op for UTF 16. Don't use it if
    doUnicodeHeader is TRUE!
*/
uint Q3TextStream::ts_getline( QChar* buf )
{
    uint rnum=0;   // the number of QChars really read
    char cbuf[ getline_buf_size+1 ];

    if ( d && d->ungetcBuf.length() ) {
	while( rnum < getline_buf_size && rnum < uint(d->ungetcBuf.length()) ) {
	    buf[rnum] = d->ungetcBuf.constref(rnum);
	    rnum++;
	}
	d->ungetcBuf = d->ungetcBuf.mid( rnum );
	if ( rnum >= getline_buf_size )
	    return rnum;
    }

#ifndef QT_NO_TEXTCODEC
    if ( mapper ) {
	QString s;
	bool readBlock = TRUE;
	for (;;) {
	    // for efficiency: try to read a line
	    if ( readBlock ) {
		int rlen = getline_buf_size - rnum;
		rlen = dev->readLine( cbuf, rlen+1 );
		if ( rlen == -1 )
		    rlen = 0;
		s  += mapper->toUnicode( cbuf, rlen, &mapperWriteState );
		readBlock = FALSE;
	    }
	    if ( dev->atEnd()
		    || s.at( s.length()-1 ) == QLatin1Char('\n')
		    || s.at( s.length()-1 ) == QLatin1Char('\r')
	       ) {
		break;
	    } else {
		// get stream (and codec) in sync
		int c;
		c = dev->getch();
		if ( c == EOF ) {
		    break;
		}
		char b = c;
		uint lengthBefore = s.length();
		s  += mapper->toUnicode( &b, 1, &mapperWriteState );
		if ( uint(s.length()) > lengthBefore )
		    break; // it seems we are in sync now
	    }
	}
	uint i = 0;
	while( rnum < getline_buf_size && i < uint(s.length()) )
	    buf[rnum++] = s.constref(i++);
	if ( uint(s.length()) > i )
	    // could be = but append is clearer
	    d->ungetcBuf.append( s.mid( i ) );
	if ( rnum < getline_buf_size && dev->atEnd() )
	    buf[rnum++] = QEOF;
    } else
#endif
    if ( latin1 ) {
	int rlen = getline_buf_size - rnum;
	rlen = dev->readLine( cbuf, rlen+1 );
	if ( rlen == -1 )
	    rlen = 0;
	char *end = cbuf+rlen;
	char *it = cbuf;
	buf +=rnum;
	while ( it != end ) {
	    buf->setCell( *(it++) );
	    buf->setRow( 0 );
	    buf++;
	}
	rnum += rlen;
	if ( rnum < getline_buf_size && dev->atEnd() )
	    buf[1] = QEOF;
    }
    return rnum;
}


/*!
    Puts one character into the stream.
*/
void Q3TextStream::ts_putc( QChar c )
{
#ifndef QT_NO_TEXTCODEC
    if ( mapper ) {
	int len = 1;
	QString s = c;
	Q3CString block = mapper->fromUnicode( s.data(), len );//, &mapperReadState );
	dev->writeBlock( block );
    } else
#endif
    if ( latin1 ) {
	if ( c.row() )
	    dev->putch( '?' ); // unknown character
	else
	    dev->putch( c.cell() );
    } else {
	if ( doUnicodeHeader ) {
	    doUnicodeHeader = FALSE;
	    ts_putc( QChar::ByteOrderMark );
	}
	if ( internalOrder ) {
	    // this case is needed by QStringBuffer
	    dev->writeBlock( (char*)&c, sizeof(QChar) );
	} else if ( networkOrder ) {
	    dev->putch( c.row() );
	    dev->putch( c.cell() );
	} else {
	    dev->putch( c.cell() );
	    dev->putch( c.row() );
	}
    }
}

/*!
    Puts one character into the stream.
*/
void Q3TextStream::ts_putc( int ch )
{
    ts_putc( QChar((ushort)ch) );
}

bool Q3TextStream::ts_isdigit( QChar c )
{
    return c.isDigit();
}

bool Q3TextStream::ts_isspace( QChar c )
{
    return c.isSpace();
}

void Q3TextStream::ts_ungetc( QChar c )
{
    if ( c.unicode() == 0xffff )
	return;

    d->ungetcBuf.prepend( c );
}

/*!
    \since 4.2

    Reads \a len bytes from the stream into \a s and returns a
    reference to the stream.

    The buffer \a s must be preallocated.

    Note that no encoding is done by this function.

    \warning The behavior of this function is undefined unless the
    stream's encoding is set to Unicode or Latin1.

    \sa QIODevice::readBlock()
*/

Q3TextStream &Q3TextStream::readRawBytes( char *s, uint len )
{
    dev->readBlock( s, len );
    return *this;
}

/*!
    \since 4.2

    Writes the \a len bytes from \a s to the stream and returns a
    reference to the stream.

    Note that no encoding is done by this function.

    \sa QIODevice::writeBlock()
*/

Q3TextStream &Q3TextStream::writeRawBytes( const char* s, uint len )
{
    dev->writeBlock( s, len );
    return *this;
}


Q3TextStream &Q3TextStream::writeBlock( const char* p, uint len )
{
    if ( doUnicodeHeader ) {
	doUnicodeHeader = FALSE;
	if ( !mapper && !latin1 ) {
	    ts_putc( QChar::ByteOrderMark );
        }
    }
    // QCString and const char * are treated as Latin-1
    if ( !mapper && latin1 ) {
	dev->writeBlock( p, len );
    } else if ( !mapper && internalOrder ) {
	QChar *u = new QChar[len];
	for ( uint i = 0; i < len; i++ )
	    u[i] = QLatin1Char(p[i]);
	dev->writeBlock( (char*)u, len * sizeof(QChar) );
	delete [] u;
    }
#ifndef QT_NO_TEXTCODEC
    else if (mapper) {
        QString s = QString::fromLatin1(p, len);
        int l = len;
        Q3CString block = mapper->fromUnicode(s.data(), l );//, &mapperReadState );
        dev->writeBlock( block );
    }
#endif
    else {
	for ( uint i = 0; i < len; i++ )
	    ts_putc( (uchar)p[i] );
    }
    return *this;
}

Q3TextStream &Q3TextStream::writeBlock( const QChar* p, uint len )
{
#ifndef QT_NO_TEXTCODEC
    if ( mapper ) {
	QConstString s( p, len );
	int l = len;
	Q3CString block = mapper->fromUnicode( s.string().data(), l );//, &mapperReadState );
	dev->writeBlock( block );
    } else
#endif
    if ( latin1 ) {
	dev->write(QString( p, len ).toLatin1());
    } else if ( internalOrder ) {
	if ( doUnicodeHeader ) {
	    doUnicodeHeader = FALSE;
	    ts_putc( QChar::ByteOrderMark );
	}
	dev->writeBlock( (char*)p, sizeof(QChar)*len );
    } else {
	for (uint i=0; i<len; i++)
	    ts_putc( p[i] );
    }
    return *this;
}

/*!
    \since 4.2

    Resets the text stream.

    \list
    \i All flags are set to 0.
    \i The field width is set to 0.
    \i The fill character is set to ' ' (Space).
    \i The precision is set to 6.
    \endlist

    \sa setf(), width(), fill(), precision()
*/

void Q3TextStream::reset()
{
    fflags = 0;
    fwidth = 0;
    fillchar = ' ';
    fprec = 6;
}

/*!
    \fn QIODevice *Q3TextStream::device() const
    \since 4.2

    Returns the IO device currently set.

    \sa setDevice(), unsetDevice()
*/

/*!
    \since 4.2

    Sets the IO device to \a iod.

    \sa device(), unsetDevice()
*/

void Q3TextStream::setDevice( QIODevice *iod )
{
    if ( owndev ) {
	delete dev;
	owndev = FALSE;
    }
    dev = iod;
    d->sourceType = Q3TextStreamPrivate::IODevice;
}

/*!
    \since 4.2

    Unsets the IO device. Equivalent to setDevice( 0 ).

    \sa device(), setDevice()
*/

void Q3TextStream::unsetDevice()
{
    setDevice( 0 );
    d->sourceType = Q3TextStreamPrivate::NotSet;
}

/*!
    \fn bool Q3TextStream::atEnd() const
    \since 4.2

    Returns TRUE if the IO device has reached the end position (end of
    the stream or file) or if there is no IO device set; otherwise
    returns FALSE.

    \sa QIODevice::atEnd()
*/

/*!\fn bool Q3TextStream::eof() const

  \obsolete

  This function has been renamed to atEnd().

  \sa QIODevice::atEnd()
*/

/*****************************************************************************
  Q3TextStream read functions
 *****************************************************************************/


/*!
    \overload

    Reads a char \a c from the stream and returns a reference to the
    stream. Note that whitespace is skipped.
*/

Q3TextStream &Q3TextStream::operator>>( char &c )
{
    CHECK_STREAM_PRECOND
    c = eat_ws().toLatin1();
    return *this;
}

/*!
    Reads a char \a c from the stream and returns a reference to the
    stream. Note that whitespace is \e not skipped.
*/

Q3TextStream &Q3TextStream::operator>>( QChar &c )
{
    CHECK_STREAM_PRECOND
    c = ts_getc();
    return *this;
}


ulong Q3TextStream::input_bin()
{
    ulong val = 0;
    QChar ch = eat_ws();
    int dv = ch.digitValue();
    while (  dv == 0 || dv == 1 ) {
	val = ( val << 1 ) + dv;
	ch = ts_getc();
	dv = ch.digitValue();
    }
    if ( ch != QEOF )
	ts_ungetc( ch );
    return val;
}

ulong Q3TextStream::input_oct()
{
    ulong val = 0;
    QChar ch = eat_ws();
    int dv = ch.digitValue();
    while ( dv >= 0 && dv <= 7 ) {
	val = ( val << 3 ) + dv;
	ch = ts_getc();
	dv = ch.digitValue();
    }
    if ( dv == 8 || dv == 9 ) {
	while ( ts_isdigit(ch) )
	    ch = ts_getc();
    }
    if ( ch != QEOF )
	ts_ungetc( ch );
    return val;
}

ulong Q3TextStream::input_dec()
{
    ulong val = 0;
    QChar ch = eat_ws();
    int dv = ch.digitValue();
    while ( ts_isdigit(ch) ) {
	val = val * 10 + dv;
	ch = ts_getc();
	dv = ch.digitValue();
    }
    if ( ch != QEOF )
	ts_ungetc( ch );
    return val;
}

ulong Q3TextStream::input_hex()
{
    ulong val = 0;
    QChar ch = eat_ws();
    char c = ch.toLatin1();
    while ( isxdigit((uchar) c) ) {
	val <<= 4;
	if ( ts_isdigit(QLatin1Char(c)) )
	    val += c - '0';
	else
	    val += 10 + tolower( (uchar) c ) - 'a';
        ch = ts_getc();
	c = ch.toLatin1();
    }
    if ( ch != QEOF )
	ts_ungetc( ch );
    return val;
}

long Q3TextStream::input_int()
{
    long val;
    QChar ch;
    char c;
    switch ( flags() & basefield ) {
    case bin:
	val = (long)input_bin();
	break;
    case oct:
	val = (long)input_oct();
	break;
    case dec:
        ch = eat_ws();
	c = ch.toLatin1();
	if ( ch == QEOF ) {
	    val = 0;
	} else {
	    if ( !(c == '-' || c == '+') )
		ts_ungetc( ch );
	    if ( c == '-' ) {
		ulong v = input_dec();
		if ( v ) {		// ensure that LONG_MIN can be read
		    v--;
		    val = -((long)v) - 1;
		} else {
		    val = 0;
		}
	    } else {
		val = (long)input_dec();
	    }
	}
	break;
    case hex:
	val = (long)input_hex();
	break;
    default:
	val = 0;
        ch = eat_ws();
	c = ch.toLatin1();
	if ( c == '0' ) {		// bin, oct or hex
            ch = ts_getc();
	    c = ch.toLatin1();
	    if ( tolower((uchar) c) == 'x' )
		val = (long)input_hex();
	    else if ( tolower((uchar) c) == 'b' )
		val = (long)input_bin();
	    else {			// octal
		ts_ungetc( ch );
		if ( c >= '0' && c <= '7' ) {
		    val = (long)input_oct();
		} else {
		    val = 0;
		}
	    }
	} else if ( ts_isdigit(ch) ) {
	    ts_ungetc( ch );
	    val = (long)input_dec();
	} else if ( c == '-' || c == '+' ) {
	    ulong v = input_dec();
	    if ( c == '-' ) {
		if ( v ) {		// ensure that LONG_MIN can be read
		    v--;
		    val = -((long)v) - 1;
		} else {
		    val = 0;
		}
	    } else {
		val = (long)v;
	    }
	}
    }
    return val;
}

//
// We use a table-driven FSM to parse floating point numbers
// strtod() cannot be used directly since we're reading from a QIODevice
//

double Q3TextStream::input_double()
{
    const int Init	 = 0;			// states
    const int Sign	 = 1;
    const int Mantissa	 = 2;
    const int Dot	 = 3;
    const int Abscissa	 = 4;
    const int ExpMark	 = 5;
    const int ExpSign	 = 6;
    const int Exponent	 = 7;
    const int Done	 = 8;

    const int InputSign	 = 1;			// input tokens
    const int InputDigit = 2;
    const int InputDot	 = 3;
    const int InputExp	 = 4;

    static const uchar table[8][5] = {
     /* None	 InputSign   InputDigit InputDot InputExp */
	{ 0,	    Sign,     Mantissa,	 Dot,	   0,	   }, // Init
	{ 0,	    0,	      Mantissa,	 Dot,	   0,	   }, // Sign
	{ Done,	    Done,     Mantissa,	 Dot,	   ExpMark,}, // Mantissa
	{ 0,	    0,	      Abscissa,	 0,	   0,	   }, // Dot
	{ Done,	    Done,     Abscissa,	 Done,	   ExpMark,}, // Abscissa
	{ 0,	    ExpSign,  Exponent,	 0,	   0,	   }, // ExpMark
	{ 0,	    0,	      Exponent,	 0,	   0,	   }, // ExpSign
	{ Done,	    Done,     Exponent,	 Done,	   Done	   }  // Exponent
    };

    int state = Init;				// parse state
    int input;					// input token

    char buf[256];
    int i = 0;
    QChar c = eat_ws();

    for (;;) {

	switch ( c.toLatin1() ) {
	    case '+':
	    case '-':
		input = InputSign;
		break;
	    case '0': case '1': case '2': case '3': case '4':
	    case '5': case '6': case '7': case '8': case '9':
		input = InputDigit;
		break;
	    case '.':
		input = InputDot;
		break;
	    case 'e':
	    case 'E':
		input = InputExp;
		break;
	    default:
		input = 0;
		break;
	}

	state = table[state][input];

	if  ( state == 0 || state == Done || i > 250 ) {
	    if ( i > 250 ) {			// ignore rest of digits
		do { c = ts_getc(); } while ( c != QEOF && ts_isdigit(c) );
	    }
	    if ( c != QEOF )
		ts_ungetc( c );
	    buf[i] = '\0';
	    char *end;
	    return strtod( buf, &end );
	}

	buf[i++] = c.toLatin1();
	c = ts_getc();
    }

#if !defined(Q_CC_EDG)
    return 0.0;
#endif
}


/*!
    \overload

    Reads a signed \c short integer \a i from the stream and returns a
    reference to the stream. See flags() for an explanation of the
    expected input format.
*/

Q3TextStream &Q3TextStream::operator>>( signed short &i )
{
    CHECK_STREAM_PRECOND
    i = (signed short)input_int();
    return *this;
}


/*!
    \overload

    Reads an unsigned \c short integer \a i from the stream and
    returns a reference to the stream. See flags() for an explanation
    of the expected input format.
*/

Q3TextStream &Q3TextStream::operator>>( unsigned short &i )
{
    CHECK_STREAM_PRECOND
    i = (unsigned short)input_int();
    return *this;
}


/*!
    \overload

    Reads a signed \c int \a i from the stream and returns a reference
    to the stream. See flags() for an explanation of the expected
    input format.
*/

Q3TextStream &Q3TextStream::operator>>( signed int &i )
{
    CHECK_STREAM_PRECOND
    i = (signed int)input_int();
    return *this;
}


/*!
    \overload

    Reads an unsigned \c int \a i from the stream and returns a
    reference to the stream. See flags() for an explanation of the
    expected input format.
*/

Q3TextStream &Q3TextStream::operator>>( unsigned int &i )
{
    CHECK_STREAM_PRECOND
    i = (unsigned int)input_int();
    return *this;
}


/*!
    \overload

    Reads a signed \c long int \a i from the stream and returns a
    reference to the stream. See flags() for an explanation of the
    expected input format.
*/

Q3TextStream &Q3TextStream::operator>>( signed long &i )
{
    CHECK_STREAM_PRECOND
    i = (signed long)input_int();
    return *this;
}


/*!
    \overload

    Reads an unsigned \c long int \a i from the stream and returns a
    reference to the stream. See flags() for an explanation of the
    expected input format.
*/

Q3TextStream &Q3TextStream::operator>>( unsigned long &i )
{
    CHECK_STREAM_PRECOND
    i = (unsigned long)input_int();
    return *this;
}


/*!
    \overload

    Reads a \c float \a f from the stream and returns a reference to
    the stream. See flags() for an explanation of the expected input
    format.
*/

Q3TextStream &Q3TextStream::operator>>( float &f )
{
    CHECK_STREAM_PRECOND
    f = (float)input_double();
    return *this;
}


/*!
    \overload

    Reads a \c double \a f from the stream and returns a reference to
    the stream. See flags() for an explanation of the expected input
    format.
*/

Q3TextStream &Q3TextStream::operator>>( double &f )
{
    CHECK_STREAM_PRECOND
    f = input_double();
    return *this;
}


/*!
    \overload

    Reads a "word" from the stream into \a s and returns a reference
    to the stream.

    A word consists of characters for which isspace() returns FALSE.
*/

Q3TextStream &Q3TextStream::operator>>( char *s )
{
    CHECK_STREAM_PRECOND
    int maxlen = width( 0 );
    QChar c = eat_ws();
    if ( !maxlen )
	maxlen = -1;
    while ( c != QEOF ) {
	if ( ts_isspace(c) || maxlen-- == 0 ) {
	    ts_ungetc( c );
	    break;
	}
	*s++ = c.toLatin1();
	c = ts_getc();
    }

    *s = '\0';
    return *this;
}

/*!
    \overload

    Reads a "word" from the stream into \a str and returns a reference
    to the stream.

    A word consists of characters for which isspace() returns FALSE.
*/

Q3TextStream &Q3TextStream::operator>>( QString &str )
{
    CHECK_STREAM_PRECOND
    str=QString::fromLatin1("");
    QChar	c = eat_ws();

    while ( c != QEOF ) {
	if ( ts_isspace(c) ) {
	    ts_ungetc( c );
	    break;
	}

	str += c;
	c = ts_getc();
    }

    return *this;
}

/*!
    \overload

    Reads a "word" from the stream into \a str and returns a reference
    to the stream.

    A word consists of characters for which isspace() returns FALSE.
*/

Q3TextStream &Q3TextStream::operator>>( Q3CString &str )
{
    CHECK_STREAM_PRECOND
    Q3CString  *dynbuf = 0;
    const int buflen = 256;
    char      buffer[buflen];
    char     *s = buffer;
    int	      i = 0;
    QChar	      c = eat_ws();

    while ( c != QEOF ) {
	if ( ts_isspace(c) ) {
	    ts_ungetc( c );
	    break;
	}
	if ( i >= buflen-1 ) {
	    if ( !dynbuf )  {			// create dynamic buffer
		dynbuf = new Q3CString(buflen*2);
		memcpy( dynbuf->data(), s, i );	// copy old data
	    } else if ( i >= (int)dynbuf->size()-1 ) {
		dynbuf->resize( dynbuf->size()*2 );
	    }
	    s = dynbuf->data();
	}
	s[i++] = c.toLatin1();
	c = ts_getc();
    }
    str.resize( i );
    memcpy( str.data(), s, i );

    delete dynbuf;
    return *this;
}


/*!
    \since 4.2

    Reads a line from the stream and returns a string containing the
    text.

    The returned string does not contain any trailing newline or
    carriage return. Note that this is different from
    QIODevice::readLine(), which does not strip the newline at the end
    of the line.

    On EOF you will get a QString that is null. On reading an empty
    line the returned QString is empty but not null.

    \sa QIODevice::readLine()
*/

QString Q3TextStream::readLine()
{
#if defined(QT_CHECK_STATE)
    if ( !dev ) {
	qWarning( "Q3TextStream::readLine: No device" );
	return QString::null;
    }
#endif
    bool readCharByChar = TRUE;
    QString result;
#if 0
    if ( !doUnicodeHeader && (
	    (latin1) ||
	    (mapper != 0 && mapper->mibEnum() == 106 ) // UTF 8
       ) ) {
	readCharByChar = FALSE;
	// use optimized read line
	QChar c[getline_buf_size];
	int pos = 0;
	bool eof = FALSE;

	for (;;) {
	    pos = ts_getline( c );
	    if ( pos == 0 ) {
		// something went wrong; try fallback
		readCharByChar = TRUE;
		//dev->resetStatus();
		break;
	    }
	    if ( c[pos-1] == QEOF || c[pos-1] == '\n' ) {
		if ( pos>2 && c[pos-1]==QEOF && c[pos-2]=='\n' ) {
		    result += QString( c, pos-2 );
		} else if ( pos > 1 ) {
		    result += QString( c, pos-1 );
		}
		if ( pos == 1 && c[pos-1] == QEOF )
		    eof = TRUE;
		break;
	    } else {
		result += QString( c, pos );
	    }
	}
	if ( eof && result.isEmpty() )
	    return QString::null;
    }
#endif
    if ( readCharByChar ) {
	const int buf_size = 256;
	QChar c[buf_size];
	int pos = 0;

	c[pos] = ts_getc();
	if ( c[pos] == QEOF )
	    return QString::null;

	while ( c[pos] != QEOF && c[pos] != QLatin1Char('\n') ) {
	    if ( c[pos] == QLatin1Char('\r') ) { // ( handle mac and dos )
		QChar nextc = ts_getc();
		if ( nextc != QLatin1Char('\n') )
		    ts_ungetc( nextc );
		break;
	    }
	    pos++;
	    if ( pos >= buf_size ) {
		result += QString( c, pos );
		pos = 0;
	    }
	    c[pos] = ts_getc();
	}
	result += QString( c, pos );
    }

    return result;
}


/*!
    \since 4.2

    Reads the entire stream from the current position, and returns a string
    containing the text.

    \sa readLine()
*/

QString Q3TextStream::read()
{
#if defined(QT_CHECK_STATE)
    if ( !dev ) {
	qWarning( "Q3TextStream::read: No device" );
	return QString::null;
    }
#endif
    QString    result;
    const uint bufsize = 512;
    QChar      buf[bufsize];
    uint       i, num, start;
    bool       skipped_cr = FALSE;

    for (;;) {
	num = ts_getbuf(buf,bufsize);
	// convert dos (\r\n) and mac (\r) style eol to unix style (\n)
	start = 0;
	for ( i=0; i<num; i++ ) {
	    if ( buf[i] == QLatin1Char('\r') ) {
		// Only skip single cr's preceding lf's
		if ( skipped_cr ) {
		    result += buf[i];
		    start++;
		} else {
		    result += QString( &buf[start], i-start );
		    start = i+1;
		    skipped_cr = TRUE;
		}
	    } else {
		if ( skipped_cr ) {
		    if ( buf[i] != QLatin1Char('\n') ) {
			// Should not have skipped it
			result += QLatin1Char('\n');
		    }
		    skipped_cr = FALSE;
		}
	    }
	}
	if ( start < num )
	    result += QString( &buf[start], i-start );
	if ( num != bufsize ) // if ( EOF )
	    break;
    }
    return result;
}



/*****************************************************************************
  Q3TextStream write functions
 *****************************************************************************/

/*!
    \since 4.2

    Writes character \c char to the stream and returns a reference to
    the stream.

    The character \a c is assumed to be Latin1 encoded independent of
    the Encoding set for the Q3TextStream.
*/
Q3TextStream &Q3TextStream::operator<<( QChar c )
{
    CHECK_STREAM_PRECOND
    ts_putc( c );
    return *this;
}

/*!
    \overload
    \since 4.2

    Writes character \a c to the stream and returns a reference to the
    stream.
*/
Q3TextStream &Q3TextStream::operator<<( char c )
{
    CHECK_STREAM_PRECOND
    unsigned char uc = (unsigned char) c;
    ts_putc( uc );
    return *this;
}

Q3TextStream &Q3TextStream::output_int( int format, ulong n, bool neg )
{
    static const char hexdigits_lower[] = "0123456789abcdef";
    static const char hexdigits_upper[] = "0123456789ABCDEF";
    CHECK_STREAM_PRECOND
    char buf[76];
    register char *p;
    int	  len;
    const char *hexdigits;

    switch ( flags() & I_BASE_MASK ) {

	case I_BASE_2:				// output binary number
	    switch ( format & I_TYPE_MASK ) {
		case I_SHORT: len=16; break;
		case I_INT:   len=sizeof(int)*8; break;
		case I_LONG:  len=32; break;
		default:      len = 0;
	    }
	    p = &buf[74];			// go reverse order
	    *p = '\0';
	    while ( len-- ) {
		*--p = (char)(n&1) + '0';
		n >>= 1;
		if ( !n )
		    break;
	    }
	    if ( flags() & showbase ) {		// show base
		*--p = (flags() & uppercase) ? 'B' : 'b';
		*--p = '0';
	    }
	    break;

	case I_BASE_8:				// output octal number
	    p = &buf[74];
	    *p = '\0';
	    do {
		*--p = (char)(n&7) + '0';
		n >>= 3;
	    } while ( n );
	    if ( flags() & showbase )
		*--p = '0';
	    break;

	case I_BASE_16:				// output hexadecimal number
	    p = &buf[74];
	    *p = '\0';
	    hexdigits = (flags() & uppercase) ?
		hexdigits_upper : hexdigits_lower;
	    do {
		*--p = hexdigits[(int)n&0xf];
		n >>= 4;
	    } while ( n );
	    if ( flags() & showbase ) {
		*--p = (flags() & uppercase) ? 'X' : 'x';
		*--p = '0';
	    }
	    break;

	default:				// decimal base is default
	    p = &buf[74];
	    *p = '\0';
	    if ( neg )
		n = (ulong)(-(long)n);
	    do {
		*--p = ((int)(n%10)) + '0';
		n /= 10;
	    } while ( n );
	    if ( neg )
		*--p = '-';
	    else if ( flags() & showpos )
		*--p = '+';
	    if ( (flags() & internal) && fwidth && !ts_isdigit(QLatin1Char(*p)) ) {
		ts_putc( *p );			// special case for internal
		++p;				//   padding
		fwidth--;
		return *this << (const char*)p;
	    }
    }
    if ( fwidth ) {				// adjustment required
	if ( !(flags() & left) ) {		// but NOT left adjustment
	    len = qstrlen(p);
	    int padlen = fwidth - len;
	    if ( padlen <= 0 ) {		// no padding required
		writeBlock( p, len );
	    } else if ( padlen < (int)(p-buf) ) { // speeds up padding
		memset( p-padlen, (char)fillchar, padlen );
		writeBlock( p-padlen, padlen+len );
	    }
	    else				// standard padding
		*this << (const char*)p;
	}
	else
	    *this << (const char*)p;
	fwidth = 0;				// reset field width
    }
    else {
	writeBlock( p, qstrlen(p) );
    }
    return *this;
}


/*!
    \overload
    \since 4.2

    Writes a \c short integer \a i to the stream and returns a
    reference to the stream.
*/

Q3TextStream &Q3TextStream::operator<<( signed short i )
{
    return output_int( I_SHORT | I_SIGNED, i, i < 0 );
}


/*!
    \overload
    \since 4.2

    Writes an \c unsigned \c short integer \a i to the stream and
    returns a reference to the stream.
*/

Q3TextStream &Q3TextStream::operator<<( unsigned short i )
{
    return output_int( I_SHORT | I_UNSIGNED, i, FALSE );
}


/*!
    \overload
    \since 4.2

    Writes an \c int \a i to the stream and returns a reference to the
    stream.
*/

Q3TextStream &Q3TextStream::operator<<( signed int i )
{
    return output_int( I_INT | I_SIGNED, i, i < 0 );
}


/*!
    \overload
    \since 4.2

    Writes an \c unsigned \c int \a i to the stream and returns a
    reference to the stream.
*/

Q3TextStream &Q3TextStream::operator<<( unsigned int i )
{
    return output_int( I_INT | I_UNSIGNED, i, FALSE );
}


/*!
    \overload
    \since 4.2

    Writes a \c long \c int \a i to the stream and returns a reference
    to the stream.
*/

Q3TextStream &Q3TextStream::operator<<( signed long i )
{
    return output_int( I_LONG | I_SIGNED, i, i < 0 );
}


/*!
    \overload
    \since 4.2

    Writes an \c unsigned \c long \c int \a i to the stream and
    returns a reference to the stream.
*/

Q3TextStream &Q3TextStream::operator<<( unsigned long i )
{
    return output_int( I_LONG | I_UNSIGNED, i, FALSE );
}


/*!
    \overload
    \since 4.2

    Writes a \c float \a f to the stream and returns a reference to
    the stream.
*/

Q3TextStream &Q3TextStream::operator<<( float f )
{
    return *this << (double)f;
}

/*!
    \overload
    \since 4.2

    Writes a \c double \a f to the stream and returns a reference to
    the stream.
*/

Q3TextStream &Q3TextStream::operator<<( double f )
{
    CHECK_STREAM_PRECOND
    char f_char;
    char format[16];
    if ( (flags()&floatfield) == fixed )
	f_char = 'f';
    else if ( (flags()&floatfield) == scientific )
	f_char = (flags() & uppercase) ? 'E' : 'e';
    else
	f_char = (flags() & uppercase) ? 'G' : 'g';
    register char *fs = format;			// generate format string
    *fs++ = '%';				//   "%.<prec>l<f_char>"
    *fs++ = '.';
    int prec = precision();
    if ( prec > 99 )
	prec = 99;
    if ( prec >= 10 ) {
	*fs++ = prec / 10 + '0';
	*fs++ = prec % 10 + '0';
    } else {
	*fs++ = prec + '0';
    }
    *fs++ = 'l';
    *fs++ = f_char;
    *fs = '\0';
    QString num;
    num.sprintf(format, f);			// convert to text
    if ( fwidth )				// padding
	*this << num.latin1();
    else					// just write it
	writeBlock(num.latin1(), num.length());
    return *this;
}


/*!
    \overload
    \since 4.2

    Writes a string to the stream and returns a reference to the
    stream.

    The string \a s is assumed to be Latin1 encoded independent of the
    Encoding set for the Q3TextStream.
*/

Q3TextStream &Q3TextStream::operator<<( const char* s )
{
    CHECK_STREAM_PRECOND
    char padbuf[48];
    uint len = qstrlen( s );			// don't write null terminator
    if ( fwidth ) {				// field width set
	int padlen = fwidth - len;
	fwidth = 0;				// reset width
	if ( padlen > 0 ) {
	    char *ppad;
	    if ( padlen > 46 ) {		// create extra big fill buffer
		ppad = new char[padlen];
		Q_CHECK_PTR( ppad );
	    } else {
		ppad = padbuf;
	    }
	    memset( ppad, (char)fillchar, padlen );	// fill with fillchar
	    if ( !(flags() & left) ) {
		writeBlock( ppad, padlen );
		padlen = 0;
	    }
	    writeBlock( s, len );
	    if ( padlen )
		writeBlock( ppad, padlen );
	    if ( ppad != padbuf )		// delete extra big fill buf
		delete[] ppad;
	    return *this;
	}
    }
    writeBlock( s, len );
    return *this;
}

/*!
    \overload
    \since 4.2

    Writes \a s to the stream and returns a reference to the stream.

    The string \a s is assumed to be Latin1 encoded independent of the
    Encoding set for the Q3TextStream.
*/

Q3TextStream &Q3TextStream::operator<<( const Q3CString & s )
{
    return operator<<(s.data());
}

/*!
    \overload
    \since 4.2

    Writes \a s to the stream and returns a reference to the stream.
*/

Q3TextStream &Q3TextStream::operator<<( const QString& s )
{
    if ( !mapper && latin1 )
	return operator<<(s.latin1());
    CHECK_STREAM_PRECOND
    QString s1 = s;
    if ( fwidth ) {				// field width set
	if ( !(flags() & left) ) {
	    s1 = s.rightJustify(fwidth, QLatin1Char((char)fillchar));
	} else {
	    s1 = s.leftJustify(fwidth, QLatin1Char((char)fillchar));
	}
	fwidth = 0;				// reset width
    }
    writeBlock( s1.unicode(), s1.length() );
    return *this;
}


/*!
    \overload
    \since 4.2

    Writes a pointer to the stream and returns a reference to the
    stream.

    The \a ptr is output as an unsigned long hexadecimal integer.
*/

Q3TextStream &Q3TextStream::operator<<( void *ptr )
{
    int f = flags();
    setf( hex, basefield );
    setf( showbase );
    unsetf( uppercase );
    output_int( I_LONG | I_UNSIGNED, (ulong)ptr, FALSE );
    flags( f );
    return *this;
}


/*!
    \fn int Q3TextStream::flags() const
    \since 4.2

    Returns the current stream flags. The default value is 0.

    \table
    \header \i Flag \i Meaning
    \row \i \c skipws \i Not currently used; whitespace always skipped
    \row \i \c left \i Numeric fields are left-aligned
    \row \i \c right
	 \i Not currently used (by default, numerics are right-aligned)
    \row \i \c internal \i Puts any padding spaces between +/- and value
    \row \i \c bin \i Output \e and input only in binary
    \row \i \c oct \i Output \e and input only in octal
    \row \i \c dec \i Output \e and input only in decimal
    \row \i \c hex \i Output \e and input only in hexadecimal
    \row \i \c showbase
	 \i Annotates numeric outputs with 0b, 0, or 0x if in \c bin,
	 \c oct, or \c hex format
    \row \i \c showpoint \i Not currently used
    \row \i \c uppercase \i Uses 0B and 0X rather than 0b and 0x
    \row \i \c showpos \i Shows + for positive numeric values
    \row \i \c scientific \i Uses scientific notation for floating point values
    \row \i \c fixed \i Uses fixed-point notation for floating point values
    \endtable

    Note that unless \c bin, \c oct, \c dec, or \c hex is set, the
    input base is octal if the value starts with 0, hexadecimal if it
    starts with 0x, binary if it starts with 0b, and decimal
    otherwise.

    \sa setf(), unsetf()
*/

/*!
    \fn int Q3TextStream::flags( int f )

    \overload

    Sets the stream flags to \a f. Returns the previous stream flags.

    \sa setf(), unsetf(), flags()
*/

/*!
    \fn int Q3TextStream::setf( int bits )
    \since 4.2

    Sets the stream flag bits \a bits. Returns the previous stream
    flags.

    Equivalent to \c{flags( flags() | bits )}.

    \sa unsetf()
*/

/*!
    \fn int Q3TextStream::setf( int bits, int mask )

    \overload

    Sets the stream flag bits \a bits with a bit mask \a mask. Returns
    the previous stream flags.

    Equivalent to \c{flags( (flags() & ~mask) | (bits & mask) )}.

    \sa setf(), unsetf()
*/

/*!
    \fn int Q3TextStream::unsetf( int bits )
    \since 4.2

    Clears the stream flag bits \a bits. Returns the previous stream
    flags.

    Equivalent to \c{flags( flags() & ~mask )}.

    \sa setf()
*/

/*!
    \fn int Q3TextStream::width() const
    \since 4.2

    Returns the field width. The default value is 0.
*/

/*!
    \fn int Q3TextStream::width( int w )

    \overload

    Sets the field width to \a w. Returns the previous field width.
*/

/*!
    \fn int Q3TextStream::fill() const
    \since 4.2

    Returns the fill character. The default value is ' ' (space).
*/

/*!
    \fn int Q3TextStream::fill( int f )
    \overload

    Sets the fill character to \a f. Returns the previous fill character.
*/

/*!
    \fn int Q3TextStream::precision() const
    \since 4.2

    Returns the precision. The default value is 6.
*/

/*!
    \fn int Q3TextStream::precision( int p )

    \overload

    Sets the precision to \a p. Returns the previous precision setting.
*/


Q3TextStream &bin( Q3TextStream &s )
{
    s.setf(Q3TextStream::bin,Q3TextStream::basefield);
    return s;
}

Q3TextStream &oct( Q3TextStream &s )
{
    s.setf(Q3TextStream::oct,Q3TextStream::basefield);
    return s;
}

Q3TextStream &dec( Q3TextStream &s )
{
    s.setf(Q3TextStream::dec,Q3TextStream::basefield);
    return s;
}

Q3TextStream &hex( Q3TextStream &s )
{
    s.setf(Q3TextStream::hex,Q3TextStream::basefield);
    return s;
}

Q3TextStream &endl( Q3TextStream &s )
{
    return s << '\n';
}

Q3TextStream &flush( Q3TextStream &s )
{
    return s;
}

Q3TextStream &ws( Q3TextStream &s )
{
    s.skipWhiteSpace();
    return s;
}

Q3TextStream &reset( Q3TextStream &s )
{
    s.reset();
    return s;
}

/*!
  \since 4.2

  Sets the encoding of this stream to \a e, where \a e is one of the
  following values:
  \table
  \header \i Encoding \i Meaning
  \row \i Locale
       \i Uses local file format (Latin1 if locale is not set), but
       autodetecting Unicode(utf16) on input.
  \row \i Unicode
       \i Uses Unicode(utf16) for input and output. Output will be
       written in the order most efficient for the current platform
       (i.e. the order used internally in QString).
  \row \i UnicodeUTF8
       \i Using Unicode(utf8) for input and output. If you use it for
       input it will autodetect utf16 and use it instead of utf8.
  \row \i Latin1
       \i ISO-8859-1. Will not autodetect utf16.
  \row \i UnicodeNetworkOrder
       \i Uses network order Unicode(utf16) for input and output.
       Useful when reading Unicode data that does not start with the
       byte order marker.
  \row \i UnicodeReverse
       \i Uses reverse network order Unicode(utf16) for input and
       output. Useful when reading Unicode data that does not start
       with the byte order marker or when writing data that should be
       read by buggy Windows applications.
  \row \i RawUnicode
       \i Like Unicode, but does not write the byte order marker nor
       does it auto-detect the byte order. Useful only when writing to
       non-persistent storage used by a single process.
  \endtable

  \c Locale and all Unicode encodings, except \c RawUnicode, will look
  at the first two bytes in an input stream to determine the byte
  order. The initial byte order marker will be stripped off before
  data is read.

  Note that this function should be called before any data is read to
  or written from the stream.

  \sa setCodec()
*/

void Q3TextStream::setEncoding( Encoding e )
{
    resetCodecConverterState(&mapperReadState);
    resetCodecConverterState(&mapperWriteState);

    if ( d->sourceType == Q3TextStreamPrivate::String )
	return;

    switch ( e ) {
    case Unicode:
	mapper = 0;
	latin1 = FALSE;
	doUnicodeHeader = TRUE;
	internalOrder = TRUE;
	networkOrder = QChar::networkOrdered();
	break;
    case UnicodeUTF8:
#ifndef QT_NO_TEXTCODEC
	mapper = QTextCodec::codecForMib( 106 );
        mapperWriteState.flags |= QTextCodec::IgnoreHeader;
	latin1 = FALSE;
	doUnicodeHeader = TRUE;
	internalOrder = TRUE;
	networkOrder = QChar::networkOrdered();
#else
	mapper = 0;
	latin1 = TRUE;
	doUnicodeHeader = TRUE;
#endif
	break;
    case UnicodeNetworkOrder:
	mapper = 0;
	latin1 = FALSE;
	doUnicodeHeader = TRUE;
	internalOrder = QChar::networkOrdered();
	networkOrder = TRUE;
	break;
    case UnicodeReverse:
	mapper = 0;
	latin1 = FALSE;
	doUnicodeHeader = TRUE;
	internalOrder = !QChar::networkOrdered();
	networkOrder = FALSE;
	break;
    case RawUnicode:
	mapper = 0;
	latin1 = FALSE;
	doUnicodeHeader = FALSE;
	internalOrder = TRUE;
	networkOrder = QChar::networkOrdered();
	break;
    case Locale:
	latin1 = TRUE; // fallback to Latin-1
#ifndef QT_NO_TEXTCODEC
	mapper = QTextCodec::codecForLocale();
        mapperReadState.flags |= QTextCodec::IgnoreHeader;
        mapperWriteState.flags |= QTextCodec::IgnoreHeader;
	// optimized Latin-1 processing
#if defined(Q_OS_WIN32)
	if ( GetACP() == 1252 )
	    mapper = 0;
#endif
	if ( mapper && mapper->mibEnum() == 4 )
#endif
	    mapper = 0;

	doUnicodeHeader = TRUE; // If it reads as Unicode, accept it
	break;
    case Latin1:
	mapper = 0;
	doUnicodeHeader = FALSE;
	latin1 = TRUE;
	break;
    }
}


#ifndef QT_NO_TEXTCODEC
/*!
    \since 4.2

    Sets the codec for this stream to \a codec. Will not try to
    autodetect Unicode.

    Note that this function should be called before any data is read
    to/written from the stream.

    \sa setEncoding(), codec()
*/

void Q3TextStream::setCodec( QTextCodec *codec )
{
    if ( d->sourceType == Q3TextStreamPrivate::String )
	return; // QString does not need any codec
    mapper = codec;
    latin1 = ( codec->mibEnum() == 4 );
    if ( latin1 )
	mapper = 0;
    doUnicodeHeader = FALSE;
}

/*!
  Returns the codec actually used for this stream.
  \since 4.2

  If Unicode is automatically detected in input, a codec with \link
  QTextCodec::name() name() \endlink "ISO-10646-UCS-2" is returned.

  \sa setCodec()
*/

QTextCodec *Q3TextStream::codec()
{
    if ( mapper ) {
	return mapper;
    } else {
	// 4 is "ISO 8859-1", 1000 is "ISO-10646-UCS-2"
	return QTextCodec::codecForMib( latin1 ? 4 : 1000 );
    }
}

#endif

QT_END_NAMESPACE

#endif // QT_NO_TEXTSTREAM
