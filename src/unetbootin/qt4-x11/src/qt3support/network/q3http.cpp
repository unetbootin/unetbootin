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

#include <qplatformdefs.h>
#include "q3http.h"

#ifndef QT_NO_NETWORKPROTOCOL_HTTP

#include "q3socket.h"
#include "qtextstream.h"
#include "qmap.h"
#include "qstring.h"
#include "qstringlist.h"
#include "q3cstring.h"
#include "qbuffer.h"
#include "q3urloperator.h"
#include "qtimer.h"
#include "private/q3membuf_p.h"
#include "qevent.h"
#include "q3url.h"
#include "qhttp.h"

QT_BEGIN_NAMESPACE

//#define Q3HTTP_DEBUG

class Q3HttpPrivate
{
public:
    Q3HttpPrivate() :
	state( Q3Http::Unconnected ),
	error( Q3Http::NoError ),
	hostname( QString() ),
	port( 0 ),
	toDevice( 0 ),
	postDevice( 0 ),
	bytesDone( 0 ),
	chunkedSize( -1 ),
	idleTimer( 0 )
    {
	pending.setAutoDelete( true );
    }

    Q3Socket socket;
    Q3PtrList<Q3HttpRequest> pending;

    Q3Http::State state;
    Q3Http::Error error;
    QString errorString;

    QString hostname;
    Q_UINT16 port;

    QByteArray buffer;
    QIODevice* toDevice;
    QIODevice* postDevice;

    uint bytesDone;
    uint bytesTotal;
    Q_LONG chunkedSize;

    Q3HttpRequestHeader header;

    bool readHeader;
    QString headerStr;
    Q3HttpResponseHeader response;

    int idleTimer;

    Q3Membuf rba;
};

class Q3HttpRequest
{
public:
    Q3HttpRequest()
    {
	id = ++idCounter;
    }
    virtual ~Q3HttpRequest()
    { }

    virtual void start( Q3Http * ) = 0;
    virtual bool hasRequestHeader();
    virtual Q3HttpRequestHeader requestHeader();

    virtual QIODevice* sourceDevice() = 0;
    virtual QIODevice* destinationDevice() = 0;

    int id;

private:
    static int idCounter;
};

int Q3HttpRequest::idCounter = 0;

bool Q3HttpRequest::hasRequestHeader()
{
    return false;
}

Q3HttpRequestHeader Q3HttpRequest::requestHeader()
{
    return Q3HttpRequestHeader();
}

/****************************************************
 *
 * Q3HttpNormalRequest
 *
 ****************************************************/

class Q3HttpNormalRequest : public Q3HttpRequest
{
public:
    Q3HttpNormalRequest( const Q3HttpRequestHeader &h, QIODevice *d, QIODevice *t ) :
	header(h), to(t)
    {
	is_ba = false;
	data.dev = d;
    }

    Q3HttpNormalRequest( const Q3HttpRequestHeader &h, QByteArray *d, QIODevice *t ) :
	header(h), to(t)
    {
	is_ba = true;
	data.ba = d;
    }

    ~Q3HttpNormalRequest()
    {
	if ( is_ba )
	    delete data.ba;
    }

    void start( Q3Http * );
    bool hasRequestHeader();
    Q3HttpRequestHeader requestHeader();

    QIODevice* sourceDevice();
    QIODevice* destinationDevice();

protected:
    Q3HttpRequestHeader header;

private:
    union {
	QByteArray *ba;
	QIODevice *dev;
    } data;
    bool is_ba;
    QIODevice *to;
};

void Q3HttpNormalRequest::start( Q3Http *http )
{
    http->d->header = header;

    if ( is_ba ) {
	http->d->buffer = *data.ba;
	if ( http->d->buffer.size() > 0 )
	    http->d->header.setContentLength( http->d->buffer.size() );

	http->d->postDevice = 0;
    } else {
	http->d->buffer = QByteArray();

	if ( data.dev && ( data.dev->isOpen() || data.dev->open(IO_ReadOnly) ) ) {
	    http->d->postDevice = data.dev;
	    if ( http->d->postDevice->size() > 0 )
		http->d->header.setContentLength( http->d->postDevice->size() );
	} else {
	    http->d->postDevice = 0;
	}
    }

    if ( to && ( to->isOpen() || to->open(IO_WriteOnly) ) )
	http->d->toDevice = to;
    else
	http->d->toDevice = 0;

    http->sendRequest();
}

bool Q3HttpNormalRequest::hasRequestHeader()
{
    return true;
}

Q3HttpRequestHeader Q3HttpNormalRequest::requestHeader()
{
    return header;
}

QIODevice* Q3HttpNormalRequest::sourceDevice()
{
    if ( is_ba )
	return 0;
    return data.dev;
}

QIODevice* Q3HttpNormalRequest::destinationDevice()
{
    return to;
}

/****************************************************
 *
 * Q3HttpPGHRequest
 * (like a Q3HttpNormalRequest, but for the convenience
 * functions put(), get() and head() -- i.e. set the
 * host header field correctly before sending the
 * request)
 *
 ****************************************************/

class Q3HttpPGHRequest : public Q3HttpNormalRequest
{
public:
    Q3HttpPGHRequest( const Q3HttpRequestHeader &h, QIODevice *d, QIODevice *t ) :
	Q3HttpNormalRequest( h, d, t )
    { }

    Q3HttpPGHRequest( const Q3HttpRequestHeader &h, QByteArray *d, QIODevice *t ) :
	Q3HttpNormalRequest( h, d, t )
    { }

    ~Q3HttpPGHRequest()
    { }

    void start( Q3Http * );
};

void Q3HttpPGHRequest::start( Q3Http *http )
{
    header.setValue( QLatin1String("Host"), http->d->hostname );
    Q3HttpNormalRequest::start( http );
}

/****************************************************
 *
 * Q3HttpSetHostRequest
 *
 ****************************************************/

class Q3HttpSetHostRequest : public Q3HttpRequest
{
public:
    Q3HttpSetHostRequest( const QString &h, Q_UINT16 p ) :
	hostname(h), port(p)
    { }

    void start( Q3Http * );

    QIODevice* sourceDevice()
    { return 0; }
    QIODevice* destinationDevice()
    { return 0; }

private:
    QString hostname;
    Q_UINT16 port;
};

void Q3HttpSetHostRequest::start( Q3Http *http )
{
    http->d->hostname = hostname;
    http->d->port = port;
    http->finishedWithSuccess();
}

/****************************************************
 *
 * Q3HttpCloseRequest
 *
 ****************************************************/

class Q3HttpCloseRequest : public Q3HttpRequest
{
public:
    Q3HttpCloseRequest()
    { }
    void start( Q3Http * );

    QIODevice* sourceDevice()
    { return 0; }
    QIODevice* destinationDevice()
    { return 0; }
};

void Q3HttpCloseRequest::start( Q3Http *http )
{
    http->close();
}

/****************************************************
 *
 * Q3HttpHeader
 *
 ****************************************************/

/*!
    \class Q3HttpHeader q3http.h
    \brief The Q3HttpHeader class contains header information for HTTP.

    \compat

    In most cases you should use the more specialized derivatives of
    this class, Q3HttpResponseHeader and Q3HttpRequestHeader, rather
    than directly using Q3HttpHeader.

    Q3HttpHeader provides the HTTP header fields. A HTTP header field
    consists of a name followed by a colon, a single space, and the
    field value. (See RFC 1945.) Field names are case-insensitive. A
    typical header field looks like this:
    \snippet doc/src/snippets/code/src_qt3support_network_q3http.cpp 0

    In the API the header field name is called the "key" and the
    content is called the "value". You can get and set a header
    field's value by using its key with value() and setValue(), e.g.
    \snippet doc/src/snippets/code/src_qt3support_network_q3http.cpp 1

    Some fields are so common that getters and setters are provided
    for them as a convenient alternative to using \l value() and
    \l setValue(), e.g. contentLength() and contentType(),
    setContentLength() and setContentType().

    Each header key has a \e single value associated with it. If you
    set the value for a key which already exists the previous value
    will be discarded.

    \sa Q3HttpRequestHeader Q3HttpResponseHeader
*/

/*!
    \fn int Q3HttpHeader::majorVersion() const

    Returns the major protocol-version of the HTTP header.
*/

/*!
    \fn int Q3HttpHeader::minorVersion() const

    Returns the minor protocol-version of the HTTP header.
*/

/*!
	Constructs an empty HTTP header.
*/
Q3HttpHeader::Q3HttpHeader()
    : valid( true )
{
}

/*!
	Constructs a copy of \a header.
*/
Q3HttpHeader::Q3HttpHeader( const Q3HttpHeader& header )
    : valid( header.valid )
{
    values = header.values;
}

/*!
    Constructs a HTTP header for \a str.

    This constructor parses the string \a str for header fields and
    adds this information. The \a str should consist of one or more
    "\r\n" delimited lines; each of these lines should have the format
    key, colon, space, value.
*/
Q3HttpHeader::Q3HttpHeader( const QString& str )
    : valid( true )
{
    parse( str );
}

/*!
    Destructor.
*/
Q3HttpHeader::~Q3HttpHeader()
{
}

/*!
    Assigns \a h and returns a reference to this http header.
*/
Q3HttpHeader& Q3HttpHeader::operator=( const Q3HttpHeader& h )
{
    values = h.values;
    valid = h.valid;
    return *this;
}

/*!
    Returns true if the HTTP header is valid; otherwise returns false.

    A Q3HttpHeader is invalid if it was created by parsing a malformed string.
*/
bool Q3HttpHeader::isValid() const
{
    return valid;
}

/*! \internal
    Parses the HTTP header string \a str for header fields and adds
    the keys/values it finds. If the string is not parsed successfully
    the Q3HttpHeader becomes \link isValid() invalid\endlink.

    Returns true if \a str was successfully parsed; otherwise returns false.

    \sa toString()
*/
bool Q3HttpHeader::parse( const QString& str )
{
    QStringList lst;
    int pos = str.find( QLatin1Char('\n') );
    if ( pos > 0 && str.at( pos - 1 ) == QLatin1Char('\r') )
	lst = QStringList::split( QLatin1String("\r\n"), str.stripWhiteSpace(), false );
    else
	lst = QStringList::split( QLatin1String("\n"), str.stripWhiteSpace(), false );

    if ( lst.isEmpty() )
	return true;

    QStringList lines;
    QStringList::Iterator it = lst.begin();
    for( ; it != lst.end(); ++it ) {
	if ( !(*it).isEmpty() ) {
	    if ( (*it)[0].isSpace() ) {
		if ( !lines.isEmpty() ) {
		    lines.last() += QLatin1String(" ");
		    lines.last() += (*it).stripWhiteSpace();
		}
	    } else {
		lines.append( (*it) );
	    }
	}
    }

    int number = 0;
    it = lines.begin();
    for( ; it != lines.end(); ++it ) {
	if ( !parseLine( *it, number++ ) ) {
	    valid = false;
	    return false;
	}
    }
    return true;
}

/*! \internal
*/
void Q3HttpHeader::setValid( bool v )
{
    valid = v;
}

/*!
    Returns the value for the entry with the given \a key. If no entry
    has this \a key, an empty string is returned.

    \sa setValue() removeValue() hasKey() keys()
*/
QString Q3HttpHeader::value( const QString& key ) const
{
    return values[ key.lower() ];
}

/*!
    Returns a list of the keys in the HTTP header.

    \sa hasKey()
*/
QStringList Q3HttpHeader::keys() const
{
    return values.keys();
}

/*!
    Returns true if the HTTP header has an entry with the given \a
    key; otherwise returns false.

    \sa value() setValue() keys()
*/
bool Q3HttpHeader::hasKey( const QString& key ) const
{
    return values.contains( key.lower() );
}

/*!
    Sets the value of the entry with the \a key to \a value.

    If no entry with \a key exists, a new entry with the given \a key
    and \a value is created. If an entry with the \a key already
    exists, its value is discarded and replaced with the given \a
    value.

    \sa value() hasKey() removeValue()
*/
void Q3HttpHeader::setValue( const QString& key, const QString& value )
{
    values[ key.lower() ] = value;
}

/*!
    Removes the entry with the key \a key from the HTTP header.

    \sa value() setValue()
*/
void Q3HttpHeader::removeValue( const QString& key )
{
    values.remove( key.lower() );
}

/*! \internal
    Parses the single HTTP header line \a line which has the format
    key, colon, space, value, and adds key/value to the headers. The
    linenumber is \a number. Returns true if the line was successfully
    parsed and the key/value added; otherwise returns false.

    \sa parse()
*/
bool Q3HttpHeader::parseLine( const QString& line, int )
{
    int i = line.find( QLatin1String(":") );
    if ( i == -1 )
	return false;

    values.insert( line.left( i ).stripWhiteSpace().lower(), line.mid( i + 1 ).stripWhiteSpace() );

    return true;
}

/*!
    Returns a string representation of the HTTP header.

    The string is suitable for use by the constructor that takes a
    QString. It consists of lines with the format: key, colon, space,
    value, "\r\n".
*/
QString Q3HttpHeader::toString() const
{
    if ( !isValid() )
	return QLatin1String("");

    QString ret = QLatin1String("");

    QMap<QString,QString>::ConstIterator it = values.begin();
    for( ; it != values.end(); ++it )
	ret += it.key() + QLatin1String(": ") + it.data() + QLatin1String("\r\n");

    return ret;
}

/*!
    Returns true if the header has an entry for the special HTTP
    header field \c content-length; otherwise returns false.

    \sa contentLength() setContentLength()
*/
bool Q3HttpHeader::hasContentLength() const
{
    return hasKey( QLatin1String("content-length") );
}

/*!
    Returns the value of the special HTTP header field \c
    content-length.

    \sa setContentLength() hasContentLength()
*/
uint Q3HttpHeader::contentLength() const
{
    return values[ QLatin1String("content-length") ].toUInt();
}

/*!
    Sets the value of the special HTTP header field \c content-length
    to \a len.

    \sa contentLength() hasContentLength()
*/
void Q3HttpHeader::setContentLength( int len )
{
    values[ QLatin1String("content-length") ] = QString::number( len );
}

/*!
    Returns true if the header has an entry for the the special HTTP
    header field \c content-type; otherwise returns false.

    \sa contentType() setContentType()
*/
bool Q3HttpHeader::hasContentType() const
{
    return hasKey( QLatin1String("content-type") );
}

/*!
    Returns the value of the special HTTP header field \c content-type.

    \sa setContentType() hasContentType()
*/
QString Q3HttpHeader::contentType() const
{
    QString type = values[ QLatin1String("content-type") ];
    if ( type.isEmpty() )
	return QString();

    int pos = type.find( QLatin1String(";") );
    if ( pos == -1 )
	return type;

    return type.left( pos ).stripWhiteSpace();
}

/*!
    Sets the value of the special HTTP header field \c content-type to
    \a type.

    \sa contentType() hasContentType()
*/
void Q3HttpHeader::setContentType( const QString& type )
{
    values[ QLatin1String("content-type") ] = type;
}

/****************************************************
 *
 * Q3HttpResponseHeader
 *
 ****************************************************/

/*!
    \class Q3HttpResponseHeader q3http.h
    \brief The Q3HttpResponseHeader class contains response header information for HTTP.

    \compat

    This class is used by the Q3Http class to report the header
    information that the client received from the server.

    HTTP responses have a status code that indicates the status of the
    response. This code is a 3-digit integer result code (for details
    see to RFC 1945). In addition to the status code, you can also
    specify a human-readable text that describes the reason for the
    code ("reason phrase"). This class allows you to get the status
    code and the reason phrase.

    \sa Q3HttpRequestHeader Q3Http
*/

/*!
    Constructs an empty HTTP response header.
*/
Q3HttpResponseHeader::Q3HttpResponseHeader()
{
    setValid( false );
}

/*!
    Constructs a HTTP response header with the status code \a code,
    the reason phrase \a text and the protocol-version \a majorVer and
    \a minorVer.
*/
Q3HttpResponseHeader::Q3HttpResponseHeader( int code, const QString& text, int majorVer, int minorVer )
    : Q3HttpHeader(), statCode( code ), reasonPhr( text ), majVer( majorVer ), minVer( minorVer )
{
}

/*!
    Constructs a copy of \a header.
*/
Q3HttpResponseHeader::Q3HttpResponseHeader( const Q3HttpResponseHeader& header )
    : Q3HttpHeader( header ), statCode( header.statCode ), reasonPhr( header.reasonPhr ), majVer( header.majVer ), minVer( header.minVer )
{
}

/*!
    Constructs a HTTP response header from the string \a str. The
    string is parsed and the information is set. The \a str should
    consist of one or more "\r\n" delimited lines; the first line should be the
    status-line (format: HTTP-version, space, status-code, space,
    reason-phrase); each of remaining lines should have the format key, colon,
    space, value.
*/
Q3HttpResponseHeader::Q3HttpResponseHeader( const QString& str )
    : Q3HttpHeader()
{
    parse( str );
}

/*!
    Sets the status code to \a code, the reason phrase to \a text and
    the protocol-version to \a majorVer and \a minorVer.

    \sa statusCode() reasonPhrase() majorVersion() minorVersion()
*/
void Q3HttpResponseHeader::setStatusLine( int code, const QString& text, int majorVer, int minorVer )
{
    setValid( true );
    statCode = code;
    reasonPhr = text;
    majVer = majorVer;
    minVer = minorVer;
}

/*!
    Returns the status code of the HTTP response header.

    \sa reasonPhrase() majorVersion() minorVersion()
*/
int Q3HttpResponseHeader::statusCode() const
{
    return statCode;
}

/*!
    Returns the reason phrase of the HTTP response header.

    \sa statusCode() majorVersion() minorVersion()
*/
QString Q3HttpResponseHeader::reasonPhrase() const
{
    return reasonPhr;
}

/*!
    Returns the major protocol-version of the HTTP response header.

    \sa minorVersion() statusCode() reasonPhrase()
*/
int Q3HttpResponseHeader::majorVersion() const
{
    return majVer;
}

/*!
    Returns the minor protocol-version of the HTTP response header.

    \sa majorVersion() statusCode() reasonPhrase()
*/
int Q3HttpResponseHeader::minorVersion() const
{
    return minVer;
}

/*! \reimp
*/
bool Q3HttpResponseHeader::parseLine( const QString& line, int number )
{
    if ( number != 0 )
	return Q3HttpHeader::parseLine( line, number );

    QString l = line.simplifyWhiteSpace();
    if ( l.length() < 10 )
	return false;

    if ( l.left( 5 ) == QLatin1String("HTTP/") && l[5].isDigit() && l[6] == QLatin1Char('.') &&
	    l[7].isDigit() && l[8] == QLatin1Char(' ') && l[9].isDigit() ) {
	majVer = l[5].latin1() - '0';
	minVer = l[7].latin1() - '0';

	int pos = l.find( QLatin1Char(' '), 9 );
	if ( pos != -1 ) {
	    reasonPhr = l.mid( pos + 1 );
	    statCode = l.mid( 9, pos - 9 ).toInt();
	} else {
	    statCode = l.mid( 9 ).toInt();
	    reasonPhr.clear();
	}
    } else {
	return false;
    }

    return true;
}

/*! \reimp
*/
QString Q3HttpResponseHeader::toString() const
{
    QString ret( QLatin1String("HTTP/%1.%2 %3 %4\r\n%5\r\n") );
    return ret.arg( majVer ).arg ( minVer ).arg( statCode ).arg( reasonPhr ).arg( Q3HttpHeader::toString() );
}

/****************************************************
 *
 * Q3HttpRequestHeader
 *
 ****************************************************/

/*!
    \class Q3HttpRequestHeader q3http.h
    \brief The Q3HttpRequestHeader class contains request header information for
    HTTP.

    \compat

    This class is used in the Q3Http class to report the header
    information if the client requests something from the server.

    HTTP requests have a method which describes the request's action.
    The most common requests are "GET" and "POST". In addition to the
    request method the header also includes a request-URI to specify
    the location for the method to use.

    The method, request-URI and protocol-version can be set using a
    constructor or later using setRequest(). The values can be
    obtained using method(), path(), majorVersion() and
    minorVersion().

    This class is a Q3HttpHeader subclass so that class's functions,
    e.g. \link Q3HttpHeader::setValue() setValue()\endlink, \link
    Q3HttpHeader::value() value()\endlink, etc. are also available.

    \sa Q3HttpResponseHeader Q3Http
*/

/*!
    Constructs an empty HTTP request header.
*/
Q3HttpRequestHeader::Q3HttpRequestHeader()
    : Q3HttpHeader()
{
    setValid( false );
}

/*!
    Constructs a HTTP request header for the method \a method, the
    request-URI \a path and the protocol-version \a majorVer and \a minorVer.
*/
Q3HttpRequestHeader::Q3HttpRequestHeader( const QString& method, const QString& path, int majorVer, int minorVer )
    : Q3HttpHeader(), m( method ), p( path ), majVer( majorVer ), minVer( minorVer )
{
}

/*!
    Constructs a copy of \a header.
*/
Q3HttpRequestHeader::Q3HttpRequestHeader( const Q3HttpRequestHeader& header )
    : Q3HttpHeader( header ), m( header.m ), p( header.p ), majVer( header.majVer ), minVer( header.minVer )
{
}

/*!
    Constructs a HTTP request header from the string \a str. The \a
    str should consist of one or more "\r\n" delimited lines; the first line
    should be the request-line (format: method, space, request-URI, space
    HTTP-version); each of the remaining lines should have the format key,
    colon, space, value.
*/
Q3HttpRequestHeader::Q3HttpRequestHeader( const QString& str )
    : Q3HttpHeader()
{
    parse( str );
}

/*!
    This function sets the request method to \a method, the
    request-URI to \a path and the protocol-version to \a majorVer and
    \a minorVer.

    \sa method() path() majorVersion() minorVersion()
*/
void Q3HttpRequestHeader::setRequest( const QString& method, const QString& path, int majorVer, int minorVer )
{
    setValid( true );
    m = method;
    p = path;
    majVer = majorVer;
    minVer = minorVer;
}

/*!
    Returns the method of the HTTP request header.

    \sa path() majorVersion() minorVersion() setRequest()
*/
QString Q3HttpRequestHeader::method() const
{
    return m;
}

/*!
    Returns the request-URI of the HTTP request header.

    \sa method() majorVersion() minorVersion() setRequest()
*/
QString Q3HttpRequestHeader::path() const
{
    return p;
}

/*!
    Returns the major protocol-version of the HTTP request header.

    \sa minorVersion() method() path() setRequest()
*/
int Q3HttpRequestHeader::majorVersion() const
{
    return majVer;
}

/*!
    Returns the minor protocol-version of the HTTP request header.

    \sa majorVersion() method() path() setRequest()
*/
int Q3HttpRequestHeader::minorVersion() const
{
    return minVer;
}

/*! \reimp
*/
bool Q3HttpRequestHeader::parseLine( const QString& line, int number )
{
    if ( number != 0 )
	return Q3HttpHeader::parseLine( line, number );

    QStringList lst = QStringList::split( QLatin1String(" "), line.simplifyWhiteSpace() );
    if ( lst.count() > 0 ) {
	m = lst[0];
	if ( lst.count() > 1 ) {
	    p = lst[1];
	    if ( lst.count() > 2 ) {
		QString v = lst[2];
		if ( v.length() >= 8 && v.left( 5 ) == QLatin1String("HTTP/") &&
			v[5].isDigit() && v[6] == QLatin1Char('.') && v[7].isDigit() ) {
		    majVer = v[5].latin1() - '0';
		    minVer = v[7].latin1() - '0';
		    return true;
		}
	    }
	}
    }

    return false;
}

/*! \reimp
*/
QString Q3HttpRequestHeader::toString() const
{
    QString first( QLatin1String("%1 %2"));
    QString last(QLatin1String(" HTTP/%3.%4\r\n%5\r\n") );
    return first.arg( m ).arg( p ) +
	last.arg( majVer ).arg( minVer ).arg( Q3HttpHeader::toString());
}


/****************************************************
 *
 * Q3Http
 *
 ****************************************************/
/*!
    \class Q3Http q3http.h
    \brief The Q3Http class provides an implementation of the HTTP protocol.

    \compat

    This class provides two different interfaces: one is the
    Q3NetworkProtocol interface that allows you to use HTTP through the
    QUrlOperator abstraction. The other is a direct interface to HTTP
    that allows you to have more control over the requests and that
    allows you to access the response header fields.

    Don't mix the two interfaces, since the behavior is not
    well-defined.

    If you want to use Q3Http with the Q3NetworkProtocol interface, you
    do not use it directly, but rather through a QUrlOperator, for
    example:

    \snippet doc/src/snippets/code/src_qt3support_network_q3http.cpp 2

    This code will only work if the Q3Http class is registered; to
    register the class, you must call q3InitNetworkProtocols() before
    using a QUrlOperator with HTTP.

    The Q3NetworkProtocol interface for HTTP only supports the
    operations operationGet() and operationPut(), i.e.
    QUrlOperator::get() and QUrlOperator::put(), if you use it with a
    QUrlOperator.

    The rest of this descrption describes the direct interface to
    HTTP.

    The class works asynchronously, so there are no blocking
    functions. If an operation cannot be executed immediately, the
    function will still return straight away and the operation will be
    scheduled for later execution. The results of scheduled operations
    are reported via signals. This approach depends on the event loop
    being in operation.

    The operations that can be scheduled (they are called "requests"
    in the rest of the documentation) are the following: setHost(),
    get(), post(), head() and request().

    All of these requests return a unique identifier that allows you
    to keep track of the request that is currently executed. When the
    execution of a request starts, the requestStarted() signal with
    the identifier is emitted and when the request is finished, the
    requestFinished() signal is emitted with the identifier and a bool
    that indicates if the request finished with an error.

    To make an HTTP request you must set up suitable HTTP headers. The
    following example demonstrates, how to request the main HTML page
    from the Trolltech home page (i.e. the URL
    http://qtsoftware.com/index.html):

    \snippet doc/src/snippets/code/src_qt3support_network_q3http.cpp 3

    For the common HTTP requests \c GET, \c POST and \c HEAD, Q3Http
    provides the convenience functions get(), post() and head(). They
    already use a reasonable header and if you don't have to set
    special header fields, they are easier to use. The above example
    can also be written as:

    \snippet doc/src/snippets/code/src_qt3support_network_q3http.cpp 4

    For this example the following sequence of signals is emitted
    (with small variations, depending on network traffic, etc.):

    \snippet doc/src/snippets/code/src_qt3support_network_q3http.cpp 5

    The dataSendProgress() and dataReadProgress() signals in the above
    example are useful if you want to show a \link QProgressBar
    progress bar\endlink to inform the user about the progress of the
    download. The second argument is the total size of data. In
    certain cases it is not possible to know the total amount in
    advance, in which case the second argument is 0. (If you connect
    to a QProgressBar a total of 0 results in a busy indicator.)

    When the response header is read, it is reported with the
    responseHeaderReceived() signal.

    The readyRead() signal tells you that there is data ready to be
    read. The amount of data can then be queried with the
    bytesAvailable() function and it can be read with the readBlock()
    or readAll() functions.

    If an error occurs during the execution of one of the commands in
    a sequence of commands, all the pending commands (i.e. scheduled,
    but not yet executed commands) are cleared and no signals are
    emitted for them.

    For example, if you have the following sequence of reqeusts

    \snippet doc/src/snippets/code/src_qt3support_network_q3http.cpp 6

    and the get() request fails because the host lookup fails, then
    the post() request is never executed and the signals would look
    like this:

    \snippet doc/src/snippets/code/src_qt3support_network_q3http.cpp 7

    You can then get details about the error with the error() and
    errorString() functions. Note that only unexpected behaviour, like
    network failure is considered as an error. If the server response
    contains an error status, like a 404 response, this is reported as
    a normal response case. So you should always check the \link
    Q3HttpResponseHeader::statusCode() status code \endlink of the
    response header.

    The functions currentId() and currentRequest() provide more
    information about the currently executing request.

    The functions hasPendingRequests() and clearPendingRequests()
    allow you to query and clear the list of pending requests.

    \sa Q3NetworkProtocol, Q3UrlOperator, Q3Ftp
*/

/*!
    Constructs a Q3Http object.
*/
Q3Http::Q3Http()
{
    init();
}

/*!
    Constructs a Q3Http object. The parameters \a parent and \a name
    are passed on to the QObject constructor.
*/
Q3Http::Q3Http( QObject* parent, const char* name )
{
    if ( parent )
	parent->insertChild( this );
    setName( name );
    init();
}

/*!
    Constructs a Q3Http object. Subsequent requests are done by
    connecting to the server \a hostname on port \a port. The
    parameters \a parent and \a name are passed on to the QObject
    constructor.

    \sa setHost()
*/
Q3Http::Q3Http( const QString &hostname, Q_UINT16 port, QObject* parent, const char* name )
{
    if ( parent )
	parent->insertChild( this );
    setName( name );
    init();

    d->hostname = hostname;
    d->port = port;
}

void Q3Http::init()
{
    bytesRead = 0;
    d = new Q3HttpPrivate;
    d->errorString = QHttp::tr( "Unknown error" );

    connect( &d->socket, SIGNAL(connected()),
	    this, SLOT(slotConnected()) );
    connect( &d->socket, SIGNAL(connectionClosed()),
	    this, SLOT(slotClosed()) );
    connect( &d->socket, SIGNAL(delayedCloseFinished()),
	    this, SLOT(slotClosed()) );
    connect( &d->socket, SIGNAL(readyRead()),
	    this, SLOT(slotReadyRead()) );
    connect( &d->socket, SIGNAL(error(int)),
	    this, SLOT(slotError(int)) );
    connect( &d->socket, SIGNAL(bytesWritten(int)),
	    this, SLOT(slotBytesWritten(int)) );

    d->idleTimer = startTimer( 0 );
}

/*!
    Destroys the Q3Http object. If there is an open connection, it is
    closed.
*/
Q3Http::~Q3Http()
{
    abort();
    delete d;
}

/*!
    \enum Q3Http::State

    This enum is used to specify the state the client is in:

    \value Unconnected There is no connection to the host.
    \value HostLookup A host name lookup is in progress.
    \value Connecting An attempt to connect to the host is in progress.
    \value Sending The client is sending its request to the server.
    \value Reading The client's request has been sent and the client
    is reading the server's response.
    \value Connected The connection to the host is open, but the client is
    neither sending a request, nor waiting for a response.
    \value Closing The connection is closing down, but is not yet
    closed. (The state will be \c Unconnected when the connection is
    closed.)

    \sa stateChanged() state()
*/

/*!  \enum Q3Http::Error

    This enum identifies the error that occurred.

    \value NoError No error occurred.
    \value HostNotFound The host name lookup failed.
    \value ConnectionRefused The server refused the connection.
    \value UnexpectedClose The server closed the connection unexpectedly.
    \value InvalidResponseHeader The server sent an invalid response header.
    \value WrongContentLength The client could not read the content correctly
    because an error with respect to the content length occurred.
    \value Aborted The request was aborted with abort().
    \value UnknownError An error other than those specified above
    occurred.

    \sa error()
*/

/*!
    \fn void Q3Http::stateChanged( int state )

    This signal is emitted when the state of the Q3Http object changes.
    The argument \a state is the new state of the connection; it is
    one of the \l State values.

    This usually happens when a request is started, but it can also
    happen when the server closes the connection or when a call to
    closeConnection() succeeded.

    \sa get() post() head() request() closeConnection() state() State
*/

/*!
    \fn void Q3Http::responseHeaderReceived( const Q3HttpResponseHeader& resp )

    This signal is emitted when the HTTP header of a server response
    is available. The header is passed in \a resp.

    \sa get() post() head() request() readyRead()
*/

/*!
    \fn void Q3Http::readyRead( const Q3HttpResponseHeader& resp )

    This signal is emitted when there is new response data to read.

    If you specified a device in the request where the data should be
    written to, then this signal is \e not emitted; instead the data
    is written directly to the device.

    The response header is passed in \a resp.

    You can read the data with the readAll() or readBlock() functions

    This signal is useful if you want to process the data in chunks as
    soon as it becomes available. If you are only interested in the
    complete data, just connect to the requestFinished() signal and
    read the data then instead.

    \sa get() post() request() readAll() readBlock() bytesAvailable()
*/

/*!
    \fn void Q3Http::dataSendProgress( int done, int total )

    This signal is emitted when this object sends data to a HTTP
    server to inform it about the progress of the upload.

    \a done is the amount of data that has already arrived and \a
    total is the total amount of data. It is possible that the total
    amount of data that should be transferred cannot be determined, in
    which case \a total is 0.(If you connect to a QProgressBar, the
    progress bar shows a busy indicator if the total is 0).

    \warning \a done and \a total are not necessarily the size in
    bytes, since for large files these values might need to be
    "scaled" to avoid overflow.

    \sa dataReadProgress() post() request() QProgressBar::setValue()
*/

/*!
    \fn void Q3Http::dataReadProgress( int done, int total )

    This signal is emitted when this object reads data from a HTTP
    server to indicate the current progress of the download.

    \a done is the amount of data that has already arrived and \a
    total is the total amount of data. It is possible that the total
    amount of data that should be transferred cannot be determined, in
    which case \a total is 0.(If you connect to a QProgressBar, the
    progress bar shows a busy indicator if the total is 0).

    \warning \a done and \a total are not necessarily the size in
    bytes, since for large files these values might need to be
    "scaled" to avoid overflow.

    \sa dataSendProgress() get() post() request() QProgressBar::setValue()
*/

/*!
    \fn void Q3Http::requestStarted( int id )

    This signal is emitted when processing the request identified by
    \a id starts.

    \sa requestFinished() done()
*/

/*!
    \fn void Q3Http::requestFinished( int id, bool error )

    This signal is emitted when processing the request identified by
    \a id has finished. \a error is true if an error occurred during
    the processing; otherwise \a error is false.

    \sa requestStarted() done() error() errorString()
*/

/*!
    \fn void Q3Http::done( bool error )

    This signal is emitted when the last pending request has finished;
    (it is emitted after the last request's requestFinished() signal).
    \a error is true if an error occurred during the processing;
    otherwise \a error is false.

    \sa requestFinished() error() errorString()
*/

/*!
    Aborts the current request and deletes all scheduled requests.

    For the current request, the requestFinished() signal with the \c
    error argument \c true is emitted. For all other requests that are
    affected by the abort(), no signals are emitted.

    Since this slot also deletes the scheduled requests, there are no
    requests left and the done() signal is emitted (with the \c error
    argument \c true).

    \sa clearPendingRequests()
*/
void Q3Http::abort()
{
    Q3HttpRequest *r = d->pending.getFirst();
    if ( r == 0 )
	return;

    finishedWithError( QHttp::tr("Request aborted"), Aborted );
    clearPendingRequests();
    d->socket.clearPendingData();
    close();
}

/*!
    Returns the number of bytes that can be read from the response
    content at the moment.

    \sa get() post() request() readyRead() readBlock() readAll()
*/
Q_ULONG Q3Http::bytesAvailable() const
{
#if defined(Q3HTTP_DEBUG)
    qDebug( "Q3Http::bytesAvailable(): %d bytes", (int)d->rba.size() );
#endif
    return d->rba.size();
}

/*!
    Reads \a maxlen bytes from the response content into \a data and
    returns the number of bytes read. Returns -1 if an error occurred.

    \sa get() post() request() readyRead() bytesAvailable() readAll()
*/
Q_LONG Q3Http::readBlock( char *data, Q_ULONG maxlen )
{
    if ( data == 0 && maxlen != 0 ) {
#if defined(QT_CHECK_NULL)
	qWarning( "Q3Http::readBlock: Null pointer error" );
#endif
	return -1;
    }
    if ( maxlen >= (Q_ULONG)d->rba.size() )
	maxlen = d->rba.size();
    d->rba.consumeBytes( maxlen, data );

    d->bytesDone += maxlen;
#if defined(Q3HTTP_DEBUG)
    qDebug( "Q3Http::readBlock(): read %d bytes (%d bytes done)", (int)maxlen, d->bytesDone );
#endif
    return maxlen;
}

/*!
    Reads all the bytes from the response content and returns them.

    \sa get() post() request() readyRead() bytesAvailable() readBlock()
*/
QByteArray Q3Http::readAll()
{
    Q_ULONG avail = bytesAvailable();
    QByteArray tmp( avail );
    Q_LONG read = readBlock( tmp.data(), avail );
    tmp.resize( read );
    return tmp;
}

/*!
    Returns the identifier of the HTTP request being executed or 0 if
    there is no request being executed (i.e. they've all finished).

    \sa currentRequest()
*/
int Q3Http::currentId() const
{
    Q3HttpRequest *r = d->pending.getFirst();
    if ( r == 0 )
	return 0;
    return r->id;
}

/*!
    Returns the request header of the HTTP request being executed. If
    the request is one issued by setHost() or closeConnection(), it
    returns an invalid request header, i.e.
    Q3HttpRequestHeader::isValid() returns false.

    \sa currentId()
*/
Q3HttpRequestHeader Q3Http::currentRequest() const
{
    Q3HttpRequest *r = d->pending.getFirst();
    if ( r != 0 && r->hasRequestHeader() )
	return r->requestHeader();
    return Q3HttpRequestHeader();
}

/*!
    Returns the QIODevice pointer that is used as the data source of the HTTP
    request being executed. If there is no current request or if the request
    does not use an IO device as the data source, this function returns 0.

    This function can be used to delete the QIODevice in the slot connected to
    the requestFinished() signal.

    \sa currentDestinationDevice() post() request()
*/
QIODevice* Q3Http::currentSourceDevice() const
{
    Q3HttpRequest *r = d->pending.getFirst();
    if ( !r )
	return 0;
    return r->sourceDevice();
}

/*!
    Returns the QIODevice pointer that is used as to store the data of the HTTP
    request being executed. If there is no current request or if the request
    does not store the data to an IO device, this function returns 0.

    This function can be used to delete the QIODevice in the slot connected to
    the requestFinished() signal.

    \sa get() post() request()
*/
QIODevice* Q3Http::currentDestinationDevice() const
{
    Q3HttpRequest *r = d->pending.getFirst();
    if ( !r )
	return 0;
    return r->destinationDevice();
}

/*!
    Returns true if there are any requests scheduled that have not yet
    been executed; otherwise returns false.

    The request that is being executed is \e not considered as a
    scheduled request.

    \sa clearPendingRequests() currentId() currentRequest()
*/
bool Q3Http::hasPendingRequests() const
{
    return d->pending.count() > 1;
}

/*!
    Deletes all pending requests from the list of scheduled requests.
    This does not affect the request that is being executed. If
    you want to stop this this as well, use abort().

    \sa hasPendingRequests() abort()
*/
void Q3Http::clearPendingRequests()
{
    Q3HttpRequest *r = 0;
    if ( d->pending.count() > 0 )
	r = d->pending.take( 0 );
    d->pending.clear();
    if ( r )
	d->pending.append( r );
}

/*!
    Sets the HTTP server that is used for requests to \a hostname on
    port \a port.

    The function does not block and returns immediately. The request
    is scheduled, and its execution is performed asynchronously. The
    function returns a unique identifier which is passed by
    requestStarted() and requestFinished().

    When the request is started the requestStarted() signal is
    emitted. When it is finished the requestFinished() signal is
    emitted.

    \sa get() post() head() request() requestStarted() requestFinished() done()
*/
int Q3Http::setHost(const QString &hostname, Q_UINT16 port )
{
    return addRequest( new Q3HttpSetHostRequest( hostname, port ) );
}

/*!
    Sends a get request for \a path to the server set by setHost() or
    as specified in the constructor.

    \a path must be an absolute path like \c /index.html or an
    absolute URI like \c http://qtsoftware.com/index.html.

    If the IO device \a to is 0 the readyRead() signal is emitted
    every time new content data is available to read.

    If the IO device \a to is not 0, the content data of the response
    is written directly to the device. Make sure that the \a to
    pointer is valid for the duration of the operation (it is safe to
    delete it when the requestFinished() signal is emitted).

    The function does not block and returns immediately. The request
    is scheduled, and its execution is performed asynchronously. The
    function returns a unique identifier which is passed by
    requestStarted() and requestFinished().

    When the request is started the requestStarted() signal is
    emitted. When it is finished the requestFinished() signal is
    emitted.

    \sa setHost() post() head() request() requestStarted() requestFinished() done()
*/
int Q3Http::get( const QString& path, QIODevice* to )
{
    Q3HttpRequestHeader header( QLatin1String("GET"), path );
    header.setValue( QLatin1String("Connection"), QLatin1String("Keep-Alive") );
    return addRequest( new Q3HttpPGHRequest( header, (QIODevice*)0, to ) );
}

/*!
    Sends a post request for \a path to the server set by setHost() or
    as specified in the constructor.

    \a path must be an absolute path like \c /index.html or an
    absolute URI like \c http://qtsoftware.com/index.html.

    The incoming data comes via the \a data IO device.

    If the IO device \a to is 0 the readyRead() signal is emitted
    every time new content data is available to read.

    If the IO device \a to is not 0, the content data of the response
    is written directly to the device. Make sure that the \a to
    pointer is valid for the duration of the operation (it is safe to
    delete it when the requestFinished() signal is emitted).

    The function does not block and returns immediately. The request
    is scheduled, and its execution is performed asynchronously. The
    function returns a unique identifier which is passed by
    requestStarted() and requestFinished().

    When the request is started the requestStarted() signal is
    emitted. When it is finished the requestFinished() signal is
    emitted.

    \sa setHost() get() head() request() requestStarted() requestFinished() done()
*/
int Q3Http::post( const QString& path, QIODevice* data, QIODevice* to  )
{
    Q3HttpRequestHeader header( QLatin1String("POST"), path );
    header.setValue( QLatin1String("Connection"), QLatin1String("Keep-Alive") );
    return addRequest( new Q3HttpPGHRequest( header, data, to ) );
}

/*!
    \overload

    \a data is used as the content data of the HTTP request.
*/
int Q3Http::post( const QString& path, const QByteArray& data, QIODevice* to )
{
    Q3HttpRequestHeader header( QLatin1String("POST"), path );
    header.setValue( QLatin1String("Connection"), QLatin1String("Keep-Alive") );
    return addRequest( new Q3HttpPGHRequest( header, new QByteArray(data), to ) );
}

/*!
    Sends a header request for \a path to the server set by setHost()
    or as specified in the constructor.

    \a path must be an absolute path like \c /index.html or an
    absolute URI like \c http://qtsoftware.com/index.html.

    The function does not block and returns immediately. The request
    is scheduled, and its execution is performed asynchronously. The
    function returns a unique identifier which is passed by
    requestStarted() and requestFinished().

    When the request is started the requestStarted() signal is
    emitted. When it is finished the requestFinished() signal is
    emitted.

    \sa setHost() get() post() request() requestStarted() requestFinished() done()
*/
int Q3Http::head( const QString& path )
{
    Q3HttpRequestHeader header( QLatin1String("HEAD"), path );
    header.setValue( QLatin1String("Connection"), QLatin1String("Keep-Alive") );
    return addRequest( new Q3HttpPGHRequest( header, (QIODevice*)0, 0 ) );
}

/*!
    Sends a request to the server set by setHost() or as specified in
    the constructor. Uses the \a header as the HTTP request header.
    You are responsible for setting up a header that is appropriate
    for your request.

    The incoming data comes via the \a data IO device.

    If the IO device \a to is 0 the readyRead() signal is emitted
    every time new content data is available to read.

    If the IO device \a to is not 0, the content data of the response
    is written directly to the device. Make sure that the \a to
    pointer is valid for the duration of the operation (it is safe to
    delete it when the requestFinished() signal is emitted).

    The function does not block and returns immediately. The request
    is scheduled, and its execution is performed asynchronously. The
    function returns a unique identifier which is passed by
    requestStarted() and requestFinished().

    When the request is started the requestStarted() signal is
    emitted. When it is finished the requestFinished() signal is
    emitted.

    \sa setHost() get() post() head() requestStarted() requestFinished() done()
*/
int Q3Http::request( const Q3HttpRequestHeader &header, QIODevice *data, QIODevice *to )
{
    return addRequest( new Q3HttpNormalRequest( header, data, to ) );
}

/*!
    \overload

    \a data is used as the content data of the HTTP request.
*/
int Q3Http::request( const Q3HttpRequestHeader &header, const QByteArray &data, QIODevice *to  )
{
    return addRequest( new Q3HttpNormalRequest( header, new QByteArray(data), to ) );
}

/*!
    Closes the connection; this is useful if you have a keep-alive
    connection and want to close it.

    For the requests issued with get(), post() and head(), Q3Http sets
    the connection to be keep-alive. You can also do this using the
    header you pass to the request() function. Q3Http only closes the
    connection to the HTTP server if the response header requires it
    to do so.

    The function does not block and returns immediately. The request
    is scheduled, and its execution is performed asynchronously. The
    function returns a unique identifier which is passed by
    requestStarted() and requestFinished().

    When the request is started the requestStarted() signal is
    emitted. When it is finished the requestFinished() signal is
    emitted.

    If you want to close the connection immediately, you have to use
    abort() instead.

    \sa stateChanged() abort() requestStarted() requestFinished() done()
*/
int Q3Http::closeConnection()
{
    return addRequest( new Q3HttpCloseRequest() );
}

int Q3Http::addRequest( Q3HttpRequest *req )
{
    d->pending.append( req );

    if ( d->pending.count() == 1 )
	// don't emit the requestStarted() signal before the id is returned
	QTimer::singleShot( 0, this, SLOT(startNextRequest()) );

    return req->id;
}

void Q3Http::startNextRequest()
{
    Q3HttpRequest *r = d->pending.getFirst();
    if ( r == 0 )
	return;

    d->error = NoError;
    d->errorString = QHttp::tr( "Unknown error" );

    if ( bytesAvailable() )
	readAll(); // clear the data
    emit requestStarted( r->id );
    r->start( this );
}

void Q3Http::sendRequest()
{
    if ( d->hostname.isNull() ) {
	finishedWithError( QHttp::tr("No server set to connect to"), UnknownError );
	return;
    }

    killIdleTimer();

    // Do we need to setup a new connection or can we reuse an
    // existing one ?
    if ( d->socket.peerName() != d->hostname || d->socket.peerPort() != d->port
        || d->socket.state() != Q3Socket::Connection ) {
	setState( Q3Http::Connecting );
	d->socket.connectToHost( d->hostname, d->port );
    } else {
        slotConnected();
    }

}

void Q3Http::finishedWithSuccess()
{
    Q3HttpRequest *r = d->pending.getFirst();
    if ( r == 0 )
	return;

    emit requestFinished( r->id, false );
    d->pending.removeFirst();
    if ( d->pending.isEmpty() ) {
	emit done( false );
    } else {
	startNextRequest();
    }
}

void Q3Http::finishedWithError( const QString& detail, int errorCode )
{
    Q3HttpRequest *r = d->pending.getFirst();
    if ( r == 0 )
	return;

    d->error = (Error)errorCode;
    d->errorString = detail;
    emit requestFinished( r->id, true );

    d->pending.clear();
    emit done( true );
}

void Q3Http::slotClosed()
{
    if ( d->state == Closing )
	return;

    if ( d->state == Reading ) {
	if ( d->response.hasKey( QLatin1String("content-length") ) ) {
	    // We got Content-Length, so did we get all bytes?
	    if ( d->bytesDone+bytesAvailable() != d->response.contentLength() ) {
		finishedWithError( QHttp::tr("Wrong content length"), WrongContentLength );
	    }
	}
    } else if ( d->state == Connecting || d->state == Sending ) {
	finishedWithError( QHttp::tr("Server closed connection unexpectedly"), UnexpectedClose );
    }

    d->postDevice = 0;
    setState( Closing );
    d->idleTimer = startTimer( 0 );
}

void Q3Http::slotConnected()
{
    if ( d->state != Sending ) {
	d->bytesDone = 0;
	setState( Sending );
    }

    QString str = d->header.toString();
    d->bytesTotal = str.length();
    d->socket.writeBlock( str.latin1(), d->bytesTotal );
#if defined(Q3HTTP_DEBUG)
    qDebug( "Q3Http: write request header:\n---{\n%s}---", str.latin1() );
#endif

    if ( d->postDevice ) {
	d->bytesTotal += d->postDevice->size();
    } else {
	d->bytesTotal += d->buffer.size();
	d->socket.writeBlock( d->buffer.data(), d->buffer.size() );
	d->buffer = QByteArray(); // save memory
    }
}

void Q3Http::slotError( int err )
{
    d->postDevice = 0;

    if ( d->state == Connecting || d->state == Reading || d->state == Sending ) {
	switch ( err ) {
	    case Q3Socket::ErrConnectionRefused:
		finishedWithError( QHttp::tr("Connection refused"), ConnectionRefused );
		break;
	    case Q3Socket::ErrHostNotFound:
		finishedWithError( QHttp::tr("Host %1 not found").arg(d->socket.peerName()), HostNotFound );
		break;
	    default:
		finishedWithError( QHttp::tr("HTTP request failed"), UnknownError );
		break;
	}
    }

    close();
}

void Q3Http::slotBytesWritten( int written )
{
    d->bytesDone += written;
    emit dataSendProgress( d->bytesDone, d->bytesTotal );

    if ( !d->postDevice )
	return;

    if ( d->socket.bytesToWrite() == 0 ) {
	int max = qMin<int>( 4096, d->postDevice->size() - d->postDevice->at() );
	QByteArray arr( max );

	int n = d->postDevice->readBlock( arr.data(), max );
	if ( n != max ) {
	    qWarning("Could not read enough bytes from the device");
	    close();
	    return;
	}
	if ( d->postDevice->atEnd() ) {
	    d->postDevice = 0;
	}

	d->socket.writeBlock( arr.data(), max );
    }
}

void Q3Http::slotReadyRead()
{
    if ( d->state != Reading ) {
	setState( Reading );
	d->buffer = QByteArray();
	d->readHeader = true;
	d->headerStr = QLatin1String("");
	d->bytesDone = 0;
	d->chunkedSize = -1;
    }

    while ( d->readHeader ) {
	bool end = false;
	QString tmp;
	while ( !end && d->socket.canReadLine() ) {
	    tmp = QLatin1String(d->socket.readLine());
	    if ( tmp == QLatin1String("\r\n") || tmp == QLatin1String("\n") )
		end = true;
	    else
		d->headerStr += tmp;
	}

	if ( !end )
	    return;

#if defined(Q3HTTP_DEBUG)
	qDebug( "Q3Http: read response header:\n---{\n%s}---", d->headerStr.latin1() );
#endif
	d->response = Q3HttpResponseHeader( d->headerStr );
	d->headerStr = QLatin1String("");
#if defined(Q3HTTP_DEBUG)
	qDebug( "Q3Http: read response header:\n---{\n%s}---", d->response.toString().latin1() );
#endif
	// Check header
	if ( !d->response.isValid() ) {
	    finishedWithError( QHttp::tr("Invalid HTTP response header"), InvalidResponseHeader );
	    close();
	    return;
	}

	// The 100-continue header is ignored, because when using the
	// POST method, we send both the request header and data in
	// one chunk.
	if (d->response.statusCode() != 100) {
	    d->readHeader = false;
	    if ( d->response.hasKey( QLatin1String("transfer-encoding") ) &&
		 d->response.value( QLatin1String("transfer-encoding") ).lower().contains( QLatin1String("chunked") ) )
		d->chunkedSize = 0;

	    emit responseHeaderReceived( d->response );
	}
    }

    if ( !d->readHeader ) {
	bool everythingRead = false;

	if ( currentRequest().method() == QLatin1String("HEAD") ) {
	    everythingRead = true;
	} else {
	    Q_ULONG n = d->socket.bytesAvailable();
	    QByteArray *arr = 0;
	    if ( d->chunkedSize != -1 ) {
		// transfer-encoding is chunked
		for ( ;; ) {
		    // get chunk size
		    if ( d->chunkedSize == 0 ) {
			if ( !d->socket.canReadLine() )
			    break;
			QString sizeString = QLatin1String(d->socket.readLine());
			int tPos = sizeString.find( QLatin1Char(';') );
			if ( tPos != -1 )
			    sizeString.truncate( tPos );
			bool ok;
			d->chunkedSize = sizeString.toInt( &ok, 16 );
			if ( !ok ) {
			    finishedWithError( QHttp::tr("Invalid HTTP chunked body"), WrongContentLength );
			    close();
                            delete arr;
			    return;
			}
			if ( d->chunkedSize == 0 ) // last-chunk
			    d->chunkedSize = -2;
		    }

		    // read trailer
		    while ( d->chunkedSize == -2 && d->socket.canReadLine() ) {
			QString read = QLatin1String(d->socket.readLine());
			if ( read == QLatin1String("\r\n") || read == QLatin1String("\n") )
			    d->chunkedSize = -1;
		    }
		    if ( d->chunkedSize == -1 ) {
			everythingRead = true;
			break;
		    }

		    // make sure that you can read the terminating CRLF,
		    // otherwise wait until next time...
		    n = d->socket.bytesAvailable();
		    if ( n == 0 )
			break;
		    if ( (Q_LONG)n == d->chunkedSize || (Q_LONG)n == d->chunkedSize+1 ) {
			n = d->chunkedSize - 1;
			if ( n == 0 )
			    break;
		    }

		    // read data
		    uint toRead = QMIN( (Q_LONG)n, (d->chunkedSize < 0 ? (Q_LONG)n : d->chunkedSize) );
		    if ( !arr )
			arr = new QByteArray( 0 );
		    uint oldArrSize = arr->size();
		    arr->resize( oldArrSize + toRead );
		    Q_LONG read = d->socket.readBlock( arr->data()+oldArrSize, toRead );
		    arr->resize( oldArrSize + read );

		    d->chunkedSize -= read;

		    if ( d->chunkedSize == 0 && n - read >= 2 ) {
			// read terminating CRLF
			char tmp[2];
			d->socket.readBlock( tmp, 2 );
			if ( tmp[0] != '\r' || tmp[1] != '\n' ) {
			    finishedWithError( QHttp::tr("Invalid HTTP chunked body"), WrongContentLength );
			    close();
                            delete arr;
			    return;
			}
		    }
		}
	    } else if ( d->response.hasContentLength() ) {
		n = qMin<ulong>( d->response.contentLength() - d->bytesDone, n );
		if ( n > 0 ) {
		    arr = new QByteArray( n );
		    Q_LONG read = d->socket.readBlock( arr->data(), n );
		    arr->resize( read );
		}
		if ( d->bytesDone + bytesAvailable() + n == d->response.contentLength() )
		    everythingRead = true;
	    } else if ( n > 0 ) {
		// workaround for VC++ bug
		QByteArray temp = d->socket.readAll();
		arr = new QByteArray( temp );
	    }

	    if ( arr ) {
		n = arr->size();
		if ( d->toDevice ) {
		    d->toDevice->writeBlock( arr->data(), n );
		    delete arr;
		    d->bytesDone += n;
#if defined(Q3HTTP_DEBUG)
		    qDebug( "Q3Http::slotReadyRead(): read %ld bytes (%d bytes done)", n, d->bytesDone );
#endif
		    if ( d->response.hasContentLength() )
			emit dataReadProgress( d->bytesDone, d->response.contentLength() );
		    else
			emit dataReadProgress( d->bytesDone, 0 );
		} else {
		    d->rba.append( arr );
#if defined(Q3HTTP_DEBUG)
		    qDebug( "Q3Http::slotReadyRead(): read %ld bytes (%ld bytes done)", n, d->bytesDone + bytesAvailable() );
#endif
		    if ( d->response.hasContentLength() )
			emit dataReadProgress( d->bytesDone + bytesAvailable(), d->response.contentLength() );
		    else
			emit dataReadProgress( d->bytesDone + bytesAvailable(), 0 );
		    emit readyRead( d->response );
		}
	    }
	}

	if ( everythingRead ) {
	    // Handle "Connection: close"
	    if ( d->response.value(QLatin1String("connection")).lower() == QLatin1String("close") ) {
		close();
	    } else {
		setState( Connected );
		// Start a timer, so that we emit the keep alive signal
		// "after" this method returned.
		d->idleTimer = startTimer( 0 );
	    }
	}
    }
}

/*!
    Returns the current state of the object. When the state changes,
    the stateChanged() signal is emitted.

    \sa State stateChanged()
*/
Q3Http::State Q3Http::state() const
{
    return d->state;
}

/*!
    Returns the last error that occurred. This is useful to find out
    what happened when receiving a requestFinished() or a done()
    signal with the \c error argument \c true.

    If you start a new request, the error status is reset to \c NoError.
*/
Q3Http::Error Q3Http::error() const
{
    return d->error;
}

/*!
    Returns a human-readable description of the last error that
    occurred. This is useful to present a error message to the user
    when receiving a requestFinished() or a done() signal with the \c
    error argument \c true.
*/
QString Q3Http::errorString() const
{
    return d->errorString;
}

/*! \reimp
*/
void Q3Http::timerEvent( QTimerEvent *e )
{
    if ( e->timerId() == d->idleTimer ) {
	killTimer( d->idleTimer );
	d->idleTimer = 0;

	if ( d->state == Connected ) {
	    finishedWithSuccess();
	} else if ( d->state != Unconnected ) {
	    setState( Unconnected );
	    finishedWithSuccess();
	}
    } else {
	QObject::timerEvent( e );
    }
}

void Q3Http::killIdleTimer()
{
    if (d->idleTimer)
        killTimer( d->idleTimer );
    d->idleTimer = 0;
}

void Q3Http::setState( int s )
{
#if defined(Q3HTTP_DEBUG)
    qDebug( "Q3Http state changed %d -> %d", d->state, s );
#endif
    d->state = (State)s;
    emit stateChanged( s );
}

void Q3Http::close()
{
    // If no connection is open -> ignore
    if ( d->state == Closing || d->state == Unconnected )
	return;

    d->postDevice = 0;
    setState( Closing );

    // Already closed ?
    if ( !d->socket.isOpen() ) {
	d->idleTimer = startTimer( 0 );
    } else {
	// Close now.
	d->socket.close();

	// Did close succeed immediately ?
	if ( d->socket.state() == Q3Socket::Idle ) {
	    // Prepare to emit the requestFinished() signal.
	    d->idleTimer = startTimer( 0 );
	}
    }
}

/**********************************************************************
 *
 * Q3Http implementation of the Q3NetworkProtocol interface
 *
 *********************************************************************/
/*! \reimp
*/
int Q3Http::supportedOperations() const
{
    return OpGet | OpPut;
}

/*! \reimp
*/
void Q3Http::operationGet( Q3NetworkOperation *op )
{
    connect( this, SIGNAL(readyRead(Q3HttpResponseHeader)),
	    this, SLOT(clientReply(Q3HttpResponseHeader)) );
    connect( this, SIGNAL(done(bool)),
	    this, SLOT(clientDone(bool)) );
    connect( this, SIGNAL(stateChanged(int)),
	    this, SLOT(clientStateChanged(int)) );

    bytesRead = 0;
    op->setState( StInProgress );
    Q3Url u( operationInProgress()->arg( 0 ) );
    Q3HttpRequestHeader header( QLatin1String("GET"), u.encodedPathAndQuery(), 1, 0 );
    header.setValue( QLatin1String("Host"), u.host() );
    setHost( u.host(), u.port() != -1 ? u.port() : 80 );
    request( header );
}

/*! \reimp
*/
void Q3Http::operationPut( Q3NetworkOperation *op )
{
    connect( this, SIGNAL(readyRead(Q3HttpResponseHeader)),
	    this, SLOT(clientReply(Q3HttpResponseHeader)) );
    connect( this, SIGNAL(done(bool)),
	    this, SLOT(clientDone(bool)) );
    connect( this, SIGNAL(stateChanged(int)),
	    this, SLOT(clientStateChanged(int)) );

    bytesRead = 0;
    op->setState( StInProgress );
    Q3Url u( operationInProgress()->arg( 0 ) );
    Q3HttpRequestHeader header( QLatin1String("POST"), u.encodedPathAndQuery(), 1, 0 );
    header.setValue( QLatin1String("Host"), u.host() );
    setHost( u.host(), u.port() != -1 ? u.port() : 80 );
    request( header, op->rawArg(1) );
}

void Q3Http::clientReply( const Q3HttpResponseHeader &rep )
{
    Q3NetworkOperation *op = operationInProgress();
    if ( op ) {
	if ( rep.statusCode() >= 400 && rep.statusCode() < 600 ) {
	    op->setState( StFailed );
	    op->setProtocolDetail(
		    QString(QLatin1String("%1 %2")).arg(rep.statusCode()).arg(rep.reasonPhrase())
						    );
	    switch ( rep.statusCode() ) {
		case 401:
		case 403:
		case 405:
		    op->setErrorCode( ErrPermissionDenied );
		    break;
		case 404:
		    op->setErrorCode(ErrFileNotExisting );
		    break;
		default:
		    if ( op->operation() == OpGet )
			op->setErrorCode( ErrGet );
		    else
			op->setErrorCode( ErrPut );
		    break;
	    }
	}
	// ### In cases of an error, should we still emit the data() signals?
	if ( op->operation() == OpGet && bytesAvailable() > 0 ) {
	    QByteArray ba = readAll();
	    emit data( ba, op );
	    bytesRead += ba.size();
	    if ( rep.hasContentLength() ) {
		emit dataTransferProgress( bytesRead, rep.contentLength(), op );
	    }
	}
    }
}

void Q3Http::clientDone( bool err )
{
    disconnect( this, SIGNAL(readyRead(Q3HttpResponseHeader)),
	    this, SLOT(clientReply(Q3HttpResponseHeader)) );
    disconnect( this, SIGNAL(done(bool)),
	    this, SLOT(clientDone(bool)) );
    disconnect( this, SIGNAL(stateChanged(int)),
	    this, SLOT(clientStateChanged(int)) );

    if ( err ) {
	Q3NetworkOperation *op = operationInProgress();
	if ( op ) {
	    op->setState( Q3NetworkProtocol::StFailed );
	    op->setProtocolDetail( errorString() );
	    switch ( error() ) {
		case ConnectionRefused:
		    op->setErrorCode( ErrHostNotFound );
		    break;
		case HostNotFound:
		    op->setErrorCode( ErrHostNotFound );
		    break;
		default:
		    if ( op->operation() == OpGet )
			op->setErrorCode( ErrGet );
		    else
			op->setErrorCode( ErrPut );
		    break;
	    }
	    emit finished( op );
	}
    } else {
	Q3NetworkOperation *op = operationInProgress();
	if ( op ) {
	    if ( op->state() != StFailed ) {
		op->setState( Q3NetworkProtocol::StDone );
		op->setErrorCode( Q3NetworkProtocol::NoError );
	    }
	    emit finished( op );
	}
    }

}

void Q3Http::clientStateChanged( int state )
{
    if ( url() ) {
	switch ( (State)state ) {
	    case Connecting:
		emit connectionStateChanged( ConHostFound, QHttp::tr( "Host %1 found" ).arg( url()->host() ) );
		break;
	    case Sending:
		emit connectionStateChanged( ConConnected, QHttp::tr( "Connected to host %1" ).arg( url()->host() ) );
		break;
	    case Unconnected:
		emit connectionStateChanged( ConClosed, QHttp::tr( "Connection to %1 closed" ).arg( url()->host() ) );
		break;
	    default:
		break;
	}
    } else {
	switch ( (State)state ) {
	    case Connecting:
		emit connectionStateChanged( ConHostFound, QHttp::tr( "Host found" ) );
		break;
	    case Sending:
		emit connectionStateChanged( ConConnected, QHttp::tr( "Connected to host" ) );
		break;
	    case Unconnected:
		emit connectionStateChanged( ConClosed, QHttp::tr( "Connection closed" ) );
		break;
	    default:
		break;
	}
    }
}

QT_END_NAMESPACE

#endif
