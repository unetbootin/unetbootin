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

#include "q3url.h"

#ifndef QT_NO_URL

#include "q3cstring.h"
#include "qdir.h"

QT_BEGIN_NAMESPACE

// used by q3filedialog.cpp
bool qt_resolve_symlinks = true;

class Q3UrlPrivate
{
public:
    QString protocol;
    QString user;
    QString pass;
    QString host;
    QString path, cleanPath;
    QString refEncoded;
    QString queryEncoded;
    bool isValid;
    int port;
    bool cleanPathDirty;
};

/*!
    Replaces backslashes with slashes and removes multiple occurrences
    of slashes or backslashes if \c allowMultiple is false.
*/

static void slashify( QString& s, bool allowMultiple = true )
{
    bool justHadSlash = false;
    for ( int i = 0; i < (int)s.length(); i++ ) {
	if ( !allowMultiple && justHadSlash &&
	     ( s[ i ] == QLatin1Char('/') || s[ i ] == QLatin1Char('\\') ) ) {
	    s.remove( i, 1 );
	    --i;
	    continue;
	}
	if ( s[ i ] == QLatin1Char('\\') )
	    s[ i ] = QLatin1Char('/');
#if defined (Q_WS_MAC9)
	if ( s[ i ] == QLatin1Char(':') && (i == (int)s.length()-1 || s[ i + 1 ] != QLatin1Char('/') ) ) //mac colon's go away, unless after a protocol
		s[ i ] = QLatin1Char('/');
#endif
	if ( s[ i ] == QLatin1Char('/') )
	    justHadSlash = true;
	else
	    justHadSlash = false;
    }
}



/*!
    \class Q3Url q3url.h
    \brief The Q3Url class provides a URL parser and simplifies working with URLs.

    \compat

    The Q3Url class is provided for simple work with URLs. It can
    parse, decode, encode, etc.

    Q3Url works with the decoded path and encoded query in turn.

    Example:

    <tt>http://qtsoftware.com:80/cgi-bin/test%20me.pl?cmd=Hello%20you</tt>

    \table
    \header \i Function	    \i Returns
    \row \i \l protocol()   \i "http"
    \row \i \l host()	    \i "qtsoftware.com"
    \row \i \l port()	    \i 80
    \row \i \l path()	    \i "/cgi-bin/test&nbsp;me.pl"
    \row \i \l fileName()   \i "test&nbsp;me.pl"
    \row \i \l query()	    \i "cmd=Hello%20you"
    \endtable

    Example:

    <tt>http://doc.trolltech.com/qdockarea.html#lines</tt>

    \table
    \header \i Function	    \i Returns
    \row \i \l protocol()   \i "http"
    \row \i \l host()	    \i "doc.trolltech.com"
    \row \i \l fileName()   \i "qdockarea.html"
    \row \i \l ref()	    \i "lines"
    \endtable

    The individual parts of a URL can be set with setProtocol(),
    setHost(), setPort(), setPath(), setFileName(), setRef() and
    setQuery(). A URL could contain, for example, an ftp address which
    requires a user name and password; these can be set with setUser()
    and setPassword().

    Because path is always encoded internally you must not use "%00"
    in the path, although this is okay (but not recommended) for the
    query.

    Q3Url is normally used like this:

    \snippet doc/src/snippets/code/src_qt3support_network_q3url.cpp 0

    You can then access and manipulate the various parts of the URL.

    To make it easy to work with Q3Urls and QStrings, Q3Url implements
    the necessary cast and assignment operators so you can do
    following:

    \snippet doc/src/snippets/code/src_qt3support_network_q3url.cpp 1

    Use the static functions, encode() and decode() to encode or
    decode a URL in a string. (They operate on the string in-place.)
    The isRelativeUrl() static function returns true if the given
    string is a relative URL.

    If you want to use a URL to work on a hierarchical structure (e.g.
    a local or remote filesystem), you might want to use the subclass
    Q3UrlOperator.

    \sa Q3UrlOperator
*/


/*!
    Constructs an empty URL that is invalid.
*/

Q3Url::Q3Url()
{
    d = new Q3UrlPrivate;
    d->isValid = false;
    d->port = -1;
    d->cleanPathDirty = true;
}

/*!
    Constructs a URL by parsing the string \a url.

    If you pass a string like "/home/qt", the "file" protocol is
    assumed.
*/

Q3Url::Q3Url( const QString& url )
{
    d = new Q3UrlPrivate;
    d->protocol = QLatin1String("file");
    d->port = -1;
    parse( url );
}

/*!
    Copy constructor. Copies the data of \a url.
*/

Q3Url::Q3Url( const Q3Url& url )
{
    d = new Q3UrlPrivate;
    *d = *url.d;
}

/*!
    Returns true if \a url is relative; otherwise returns false.
*/

bool Q3Url::isRelativeUrl( const QString &url )
{
    int colon = url.find( QLatin1String(":") );
    int slash = url.find( QLatin1String("/") );

    return ( slash != 0 && ( colon == -1 || ( slash != -1 && colon > slash ) ) );
}

/*!
    Constructs an URL taking \a url as the base (context) and
    \a relUrl as a relative URL to \a url. If \a relUrl is not relative,
    \a relUrl is taken as the new URL.

    For example, the path of
    \snippet doc/src/snippets/code/src_qt3support_network_q3url.cpp 2
    will be "/qt/srource/qt-2.1.0.tar.gz".

    On the other hand,
    \snippet doc/src/snippets/code/src_qt3support_network_q3url.cpp 3
    will result in a new URL, "ftp://ftp.trolltech.com/usr/local",
    because "/usr/local" isn't relative.

    Similarly,
    \snippet doc/src/snippets/code/src_qt3support_network_q3url.cpp 4
    will result in a new URL, with "/usr/local" as the path
    and "file" as the protocol.

    Normally it is expected that the path of \a url points to a
    directory, even if the path has no slash at the end. But if you
    want the constructor to handle the last part of the path as a file
    name if there is no slash at the end, and to let it be replaced by
    the file name of \a relUrl (if it contains one), set \a checkSlash
    to true.
*/

Q3Url::Q3Url( const Q3Url& url, const QString& relUrl, bool checkSlash )
{
    d = new Q3UrlPrivate;
    QString rel = relUrl;
    slashify( rel );

    Q3Url urlTmp( url );
    if ( !urlTmp.isValid() ) {
	urlTmp.reset();
    }
    if ( isRelativeUrl( rel ) ) {
	if ( rel[ 0 ] == QLatin1Char('#') ) {
	    *this = urlTmp;
	    rel.remove( (uint)0, 1 );
	    decode( rel );
	    setRef( rel );
	} else if ( rel[ 0 ] == QLatin1Char('?') ) {
	    *this = urlTmp;
	    rel.remove( (uint)0, 1 );
	    setQuery( rel );
	} else {
	    decode( rel );
	    *this = urlTmp;
	    setRef( QString() );
	    if ( checkSlash && d->cleanPath[(int)path().length()-1] != QLatin1Char('/') ) {
		if ( isRelativeUrl( path() ) )
		    setEncodedPathAndQuery( rel );
		else
		    setFileName( rel );
	    } else {
		QString p = urlTmp.path();
		if ( p.isEmpty() ) {
		    // allow URLs like "file:foo"
		    if ( !d->host.isEmpty() && !d->user.isEmpty() && !d->pass.isEmpty() )
			p = QLatin1String("/");
		}
		if ( !p.isEmpty() && p.right(1)!=QLatin1String("/") )
		    p += QLatin1String("/");
		p += rel;
		d->path = p;
		d->cleanPathDirty = true;
	    }
	}
    } else {
	if ( rel[ 0 ] == QChar( QLatin1Char('/') ) ) {
	    *this = urlTmp;
	    setEncodedPathAndQuery( rel );
	} else {
	    *this = rel;
	}
    }
}

/*!
    Destructor.
*/

Q3Url::~Q3Url()
{
    delete d;
    d = 0;
}

/*!
    Returns the protocol of the URL. Typically, "file", "http", "ftp",
    etc.

    \sa setProtocol()
*/

QString Q3Url::protocol() const
{
    return d->protocol;
}

/*!
    Sets the protocol of the URL to \a protocol. Typically, "file",
    "http", "ftp", etc.

    \sa protocol()
*/

void Q3Url::setProtocol( const QString& protocol )
{
    d->protocol = protocol;
    if ( hasHost() )
	d->isValid = true;
}

/*!
    Returns the username of the URL.

    \sa setUser() setPassword()
*/

QString Q3Url::user() const
{
    return  d->user;
}

/*!
    Sets the username of the URL to \a user.

    \sa user() setPassword()
*/

void Q3Url::setUser( const QString& user )
{
    d->user = user;
}

/*!
    Returns true if the URL contains a username; otherwise returns
    false.

    \sa setUser() setPassword()
*/

bool Q3Url::hasUser() const
{
    return !d->user.isEmpty();
}

/*!
    Returns the password of the URL.

    \warning Passwords passed in URLs are normally \e insecure; this
    is due to the mechanism, not because of Qt.

    \sa setPassword() setUser()
*/

QString Q3Url::password() const
{
    return d->pass;
}

/*!
    Sets the password of the URL to \a pass.

    \warning Passwords passed in URLs are normally \e insecure; this
    is due to the mechanism, not because of Qt.

    \sa password() setUser()
*/

void Q3Url::setPassword( const QString& pass )
{
    d->pass = pass;
}

/*!
    Returns true if the URL contains a password; otherwise returns
    false.

    \warning Passwords passed in URLs are normally \e insecure; this
    is due to the mechanism, not because of Qt.

    \sa setPassword() setUser()
*/

bool Q3Url::hasPassword() const
{
    return !d->pass.isEmpty();
}

/*!
    Returns the hostname of the URL.

    \sa setHost() hasHost()
*/

QString Q3Url::host() const
{
    return d->host;
}

/*!
    Sets the hostname of the URL to \a host.

    \sa host() hasHost()
*/

void Q3Url::setHost( const QString& host )
{
    d->host = host;
    if ( !d->protocol.isNull() && d->protocol != QLatin1String("file") )
	d->isValid = true;
}

/*!
    Returns true if the URL contains a hostname; otherwise returns
    false.

    \sa setHost()
*/

bool Q3Url::hasHost() const
{
    return !d->host.isEmpty();
}

/*!
    Returns the port of the URL or -1 if no port has been set.

    \sa setPort()
*/

int Q3Url::port() const
{
    return d->port;
}

/*!
    Sets the port of the URL to \a port.

    \sa port()
*/

void Q3Url::setPort( int port )
{
    d->port = port;
}

/*!
    Returns true if the URL contains a port; otherwise returns false.

    \sa setPort()
*/

bool Q3Url::hasPort() const
{
    return d->port >= 0;
}

/*!
    Sets the path of the URL to \a path.

    \sa path() hasPath()
*/

void Q3Url::setPath( const QString& path )
{
    d->path = path;
    slashify( d->path );
    d->cleanPathDirty = true;
    d->isValid = true;
}

/*!
    Returns true if the URL contains a path; otherwise returns false.

    \sa path() setPath()
*/

bool Q3Url::hasPath() const
{
    return !d->path.isEmpty();
}

/*!
    Sets the query of the URL to \a txt. \a txt must be encoded.

    \sa query() encode()
*/

void Q3Url::setQuery( const QString& txt )
{
    d->queryEncoded = txt;
}

/*!
    Returns the (encoded) query of the URL.

    \sa setQuery() decode()
*/

QString Q3Url::query() const
{
    return d->queryEncoded;
}

/*!
    Returns the (encoded) reference of the URL.

    \sa setRef() hasRef() decode()
*/

QString Q3Url::ref() const
{
    return d->refEncoded;
}

/*!
    Sets the reference of the URL to \a txt. \a txt must be encoded.

    \sa ref() hasRef() encode()
*/

void Q3Url::setRef( const QString& txt )
{
    d->refEncoded = txt;
}

/*!
    Returns true if the URL has a reference; otherwise returns false.

    \sa setRef()
*/

bool Q3Url::hasRef() const
{
    return !d->refEncoded.isEmpty();
}

/*!
    Returns true if the URL is valid; otherwise returns false. A URL
    is invalid if it cannot be parsed, for example.
*/

bool Q3Url::isValid() const
{
    return d->isValid;
}

/*!
    Resets all parts of the URL to their default values and
    invalidates it.
*/

void Q3Url::reset()
{
    d->protocol = QLatin1String("file");
    d->user = QLatin1String("");
    d->pass = QLatin1String("");
    d->host = QLatin1String("");
    d->path = QLatin1String("");
    d->queryEncoded = QLatin1String("");
    d->refEncoded = QLatin1String("");
    d->isValid = true;
    d->port = -1;
    d->cleanPathDirty = true;
}

/*!
    Parses the \a url. Returns true on success; otherwise returns false.
*/

bool Q3Url::parse( const QString& url )
{
    QString url_( url );
    slashify( url_ );

    if ( url_.isEmpty() ) {
	d->isValid = false;
	return false;
    }

    d->cleanPathDirty = true;
    d->isValid = true;
    QString oldProtocol = d->protocol;
    d->protocol.clear();

    const int Init	= 0;
    const int Protocol	= 1;
    const int Separator1= 2; // :
    const int Separator2= 3; // :/
    const int Separator3= 4; // :// or more slashes
    const int User	= 5;
    const int Pass	= 6;
    const int Host	= 7;
    const int Path	= 8;
    const int Ref	= 9;
    const int Query	= 10;
    const int Port	= 11;
    const int Done	= 12;

    const int InputAlpha= 1;
    const int InputDigit= 2;
    const int InputSlash= 3;
    const int InputColon= 4;
    const int InputAt	= 5;
    const int InputHash = 6;
    const int InputQuery= 7;

    static uchar table[ 12 ][ 8 ] = {
     /* None       InputAlpha  InputDigit  InputSlash  InputColon  InputAt     InputHash   InputQuery */
	{ 0,       Protocol,   0,          Path,       0,          0,          0,          0,         }, // Init
	{ 0,       Protocol,   Protocol,   0,          Separator1, 0,          0,          0,         }, // Protocol
	{ 0,       Path,       Path,       Separator2, 0,          0,          0,          0,         }, // Separator1
	{ 0,       Path,       Path,       Separator3, 0,          0,          0,          0,         }, // Separator2
	{ 0,       User,       User,       Separator3, Pass,       Host,       0,          0,         }, // Separator3
	{ 0,       User,       User,       User,       Pass,       Host,       User,       User,      }, // User
	{ 0,       Pass,       Pass,       Pass,       Pass,       Host,       Pass,       Pass,      }, // Pass
	{ 0,       Host,       Host,       Path,       Port,       Host,       Ref,        Query,     }, // Host
	{ 0,       Path,       Path,       Path,       Path,       Path,       Ref,        Query,     }, // Path
	{ 0,       Ref,        Ref,        Ref,        Ref,        Ref,        Ref,        Query,     }, // Ref
	{ 0,       Query,      Query,      Query,      Query,      Query,      Query,      Query,     }, // Query
	{ 0,       0,          Port,       Path,       0,          0,          0,          0,         }  // Port
    };

    bool relPath = false;

    relPath = false;
    bool forceRel = false;

    // If ':' is at pos 1, we have only one letter
    // before that separator => that's a drive letter!
    if ( url_.length() >= 2 && url_[1] == QLatin1Char(':') )
	relPath = forceRel = true;

    int hasNoHost = -1;
    int cs = url_.find( QLatin1String(":/") );
    if ( cs != -1 ) // if a protocol is there, find out if there is a host or directly the path after it
	hasNoHost = url_.find( QLatin1String("///"), cs );
    table[ 4 ][ 1 ] = User;
    table[ 4 ][ 2 ] = User;
    if ( cs == -1 || forceRel ) { // we have a relative file
	if ( url.find( QLatin1Char(':') ) == -1 || forceRel ) {
	    table[ 0 ][ 1 ] = Path;
	    // Filenames may also begin with a digit
	    table[ 0 ][ 2 ] = Path;
	} else {
	    table[ 0 ][ 1 ] = Protocol;
	}
	relPath = true;
    } else { // some checking
	table[ 0 ][ 1 ] = Protocol;

	// find the part between the protocol and the path as the meaning
	// of that part is dependent on some chars
	++cs;
	while ( url_[ cs ] == QLatin1Char('/') )
	    ++cs;
	int slash = url_.find( QLatin1String("/"), cs );
	if ( slash == -1 )
	    slash = url_.length() - 1;
	QString tmp = url_.mid( cs, slash - cs + 1 );

	if ( !tmp.isEmpty() ) { // if this part exists

	    // look for the @ in this part
	    int at = tmp.find( QLatin1String("@") );
	    if ( at != -1 )
		at += cs;
	    // we have no @, which means host[:port], so directly
	    // after the protocol the host starts, or if the protocol
	    // is file or there were more than 2 slashes, it is the
	    // path
	    if ( at == -1 ) {
		if ( url_.left( 4 ) == QLatin1String("file") || hasNoHost != -1 )
		    table[ 4 ][ 1 ] = Path;
		else
		    table[ 4 ][ 1 ] = Host;
		table[ 4 ][ 2 ] = table[ 4 ][ 1 ];
	    }
	}
    }

    int state = Init; // parse state
    int input; // input token

    QChar c = url_[ 0 ];
    int i = 0;
    QString port;

    for ( ;; ) {
	switch ( c.latin1() ) {
	case '?':
	    input = InputQuery;
	    break;
	case '#':
	    input = InputHash;
	    break;
	case '@':
	    input = InputAt;
	    break;
	case ':':
	    input = InputColon;
	    break;
	case '/':
	    input = InputSlash;
	    break;
	case '1': case '2': case '3': case '4': case '5':
	case '6': case '7': case '8': case '9': case '0':
	    input = InputDigit;
	    break;
	default:
	    input = InputAlpha;
	}

	state = table[ state ][ input ];

	switch ( state ) {
	case Protocol:
	    d->protocol += c;
	    break;
	case User:
	    d->user += c;
	    break;
	case Pass:
	    d->pass += c;
	    break;
	case Host:
	    d->host += c;
	    break;
	case Path:
	    d->path += c;
	    break;
	case Ref:
	    d->refEncoded += c;
	    break;
	case Query:
	    d->queryEncoded += c;
	    break;
	case Port:
	    port += c;
	    break;
	default:
	    break;
	}

	++i;
	if ( i > (int)url_.length() - 1 || state == Done || state == 0 )
	    break;
	c = url_[ i ];

    }

    if ( !port.isEmpty() ) {
	port.remove( (uint)0, 1 );
	d->port = port.toInt();
    }

    // error
    if ( i < (int)url_.length() - 1 ) {
	d->isValid = false;
	return false;
    }


    if ( d->protocol.isEmpty() )
	d->protocol = oldProtocol;

    if ( d->path.isEmpty() )
	d->path = QLatin1String("/");

    // hack for windows
    if ( d->path.length() == 2 && d->path[ 1 ] == QLatin1Char(':') )
	d->path += QLatin1String("/");

    // #### do some corrections, should be done nicer too
    if ( !d->pass.isEmpty() ) {
	if ( d->pass[ 0 ] == QLatin1Char(':') )
	    d->pass.remove( (uint)0, 1 );
	decode( d->pass );
    }
    if ( !d->user.isEmpty() ) {
	decode( d->user );
    }
    if ( !d->path.isEmpty() ) {
	if ( d->path[ 0 ] == QLatin1Char('@') || d->path[ 0 ] == QLatin1Char(':') )
	    d->path.remove( (uint)0, 1 );
	if ( d->path[ 0 ] != QLatin1Char('/') && !relPath && d->path[ 1 ] != QLatin1Char(':') )
	    d->path.prepend( QLatin1String("/") );
    }
    if ( !d->refEncoded.isEmpty() && d->refEncoded[ 0 ] == QLatin1Char('#') )
	d->refEncoded.remove( (uint)0, 1 );
    if ( !d->queryEncoded.isEmpty() && d->queryEncoded[ 0 ] == QLatin1Char('?') )
	d->queryEncoded.remove( (uint)0, 1 );
    if ( !d->host.isEmpty() && d->host[ 0 ] == QLatin1Char('@') )
	d->host.remove( (uint)0, 1 );

#if defined(Q_OS_WIN32)
    // hack for windows file://machine/path syntax
    if ( d->protocol == QLatin1String("file") ) {
	if ( url.left( 7 ) == QLatin1String("file://") &&
	     d->path.length() > 1 && d->path[ 1 ] != QLatin1Char(':') )
		 d->path.prepend( QLatin1String("/") );
    }
#endif

    decode( d->path );
    d->cleanPathDirty = true;

#if 0
    qDebug( "URL: %s", url.latin1() );
    qDebug( "protocol: %s", d->protocol.latin1() );
    qDebug( "user: %s", d->user.latin1() );
    qDebug( "pass: %s", d->pass.latin1() );
    qDebug( "host: %s", d->host.latin1() );
    qDebug( "path: %s", path().latin1() );
    qDebug( "ref: %s", d->refEncoded.latin1() );
    qDebug( "query: %s", d->queryEncoded.latin1() );
    qDebug( "port: %d\n\n----------------------------\n\n", d->port );
#endif

    return true;
}

/*!
    \overload

    Parses \a url and assigns the resulting data to this class.

    If you pass a string like "/home/qt" the "file" protocol will be
    assumed.
*/

Q3Url& Q3Url::operator=( const QString& url )
{
    reset();
    parse( url );

    return *this;
}

/*!
    Assigns the data of \a url to this class.
*/

Q3Url& Q3Url::operator=( const Q3Url& url )
{
    *d = *url.d;
    return *this;
}

/*!
    Compares this URL with \a url and returns true if they are equal;
    otherwise returns false.
*/

bool Q3Url::operator==( const Q3Url& url ) const
{
    if ( !isValid() || !url.isValid() )
	return false;

    if ( d->protocol == url.d->protocol &&
	 d->user == url.d->user &&
	 d->pass == url.d->pass &&
	 d->host == url.d->host &&
	 d->path == url.d->path &&
	 d->queryEncoded == url.d->queryEncoded &&
	 d->refEncoded == url.d->refEncoded &&
	 d->isValid == url.d->isValid &&
	 d->port == url.d->port )
	return true;

    return false;
}

/*!
    \overload

    Compares this URL with \a url. \a url is parsed first. Returns
    true if \a url is equal to this url; otherwise returns false.
*/

bool Q3Url::operator==( const QString& url ) const
{
    Q3Url u( url );
    return ( *this == u );
}

/*!
    Sets the file name of the URL to \a name. If this URL contains a
    fileName(), the original file name is replaced by \a name.

    See the documentation of fileName() for a more detailed discussion
    of what is handled as file name and what is handled as a directory
    path.

    \sa fileName()
*/

void Q3Url::setFileName( const QString& name )
{
    QString fn( name );
    slashify( fn );

    while ( fn[ 0 ] == QLatin1Char( '/' ) )
	fn.remove( (uint)0, 1 );

    QString p;
    if ( path().isEmpty() ) {
	p = QLatin1String("/");
    } else {
	p = path();
	int slash = p.findRev( QLatin1Char( '/' ) );
	if ( slash == -1 ) {
	    p = QLatin1String("/");
	} else if ( p[ (int)p.length() - 1 ] != QLatin1Char( '/' ) ) {
	    p.truncate( slash + 1 );
	}
    }

    p += fn;
    if ( !d->queryEncoded.isEmpty() )
	p += QLatin1String("?") + d->queryEncoded;
    setEncodedPathAndQuery( p );
}

/*!
    Returns the encoded path and query.

    \sa decode()
*/

QString Q3Url::encodedPathAndQuery()
{
    QString p = path();
    if ( p.isEmpty() )
	p = QLatin1String("/");

    encode( p );

    if ( !d->queryEncoded.isEmpty() ) {
	p += QLatin1String("?");
	p += d->queryEncoded;
    }

    return p;
}

/*!
    Parses \a pathAndQuery for a path and query and sets those values.
    The whole string must be encoded.

    \sa encode()
*/

void Q3Url::setEncodedPathAndQuery( const QString& pathAndQuery )
{
    d->cleanPathDirty = true;
    int pos = pathAndQuery.find( QLatin1Char('?') );
    if ( pos == -1 ) {
	d->path = pathAndQuery;
	d->queryEncoded = QLatin1String("");
    } else {
	d->path = pathAndQuery.left( pos );
	d->queryEncoded = pathAndQuery.mid( pos + 1 );
    }

    decode( d->path );
    d->cleanPathDirty = true;
}

/*!
    Returns the path of the URL. If \a correct is true, the path is
    cleaned (deals with too many or too few slashes, cleans things
    like "/../..", etc). Otherwise path() returns exactly the path
    that was parsed or set.

    \sa setPath() hasPath()
*/
QString Q3Url::path( bool correct ) const
{
    if ( !correct )
	return d->path;

    if ( d->cleanPathDirty ) {
	bool check = true;
	if ( QDir::isRelativePath( d->path ) ) {
	    d->cleanPath = d->path;
	} else if ( isLocalFile() ) {
#if defined(Q_OS_WIN32)
	    // hack for stuff like \\machine\path and //machine/path on windows
	    if ( ( d->path.left( 1 ) == QLatin1String("/") || d->path.left( 1 ) == QLatin1String("\\") ) &&
		 d->path.length() > 1 ) {
		d->cleanPath = d->path;
		bool share = (d->cleanPath[0] == QLatin1Char('\\') && d->cleanPath[1] == QLatin1Char('\\')) ||
		             (d->cleanPath[0] == QLatin1Char('/') && d->cleanPath[1] == QLatin1Char('/'));
		slashify( d->cleanPath, false );
		d->cleanPath = QDir::cleanDirPath( d->cleanPath );
		if ( share ) {
		    check = false;
		    while (d->cleanPath.at(0) != QLatin1Char('/') || d->cleanPath.at(1) != QLatin1Char('/'))
			d->cleanPath.prepend(QLatin1String("/"));
		}
	    }
#endif
	    if ( check ) {
		QFileInfo fi( d->path );
		if ( !fi.exists() )
		    d->cleanPath = d->path;
		else if ( fi.isDir() ) {
                    QString canPath = QDir( d->path ).canonicalPath();
                    QString dir;
                    if ( qt_resolve_symlinks && !canPath.isNull() )
                       dir = QDir::cleanDirPath( canPath );
                    else
                       dir = QDir::cleanDirPath( QDir( d->path ).absPath() );
                    dir += QLatin1String("/");
		    if ( dir == QLatin1String("//") )
			d->cleanPath = QLatin1String("/");
		    else
			d->cleanPath = dir;
		} else {
		    QString p =
			QDir::cleanDirPath( (qt_resolve_symlinks ?
					    fi.dir().canonicalPath() :
					    fi.dir().absPath()) );
		    d->cleanPath = p + QLatin1String("/") + fi.fileName();
		}
	    }
	} else {
	    if ( d->path != QLatin1String("/") && d->path[ (int)d->path.length() - 1 ] == QLatin1Char('/') )
		d->cleanPath = QDir::cleanDirPath( d->path ) + QLatin1String("/");
	    else
		d->cleanPath = QDir::cleanDirPath( d->path );
	}

	if ( check )
	    slashify( d->cleanPath, false );
	d->cleanPathDirty = false;
    }

    return d->cleanPath;
}

/*!
    Returns true if the URL is a local file; otherwise returns false.
*/

bool Q3Url::isLocalFile() const
{
    return d->protocol == QLatin1String("file");
}

/*!
    Returns the file name of the URL. If the path of the URL doesn't
    have a slash at the end, the part between the last slash and the
    end of the path string is considered to be the file name. If the
    path has a slash at the end, an empty string is returned here.

    \sa setFileName()
*/

QString Q3Url::fileName() const
{
    if ( d->path.isEmpty() || d->path.endsWith( QLatin1String("/") )
#ifdef Q_WS_WIN
	|| d->path.endsWith( QLatin1String("\\") )
#endif
	)
	return QString();

    return QFileInfo( d->path ).fileName();
}

/*!
    Adds the path \a pa to the path of the URL.

    \sa setPath() hasPath()
*/

void Q3Url::addPath( const QString& pa )
{
    if ( pa.isEmpty() )
	return;

    QString p( pa );
    slashify( p );

    if ( path().isEmpty() ) {
	if ( p[ 0 ] != QLatin1Char( '/' ) )
	    d->path = QLatin1String("/") + p;
	else
	    d->path = p;
    } else {
	if ( p[ 0 ] != QLatin1Char( '/' ) && d->path[ (int)d->path.length() - 1 ] != QLatin1Char('/') )
	    d->path += QLatin1String("/") + p;
	else
	    d->path += p;
    }
    d->cleanPathDirty = true;
}

/*!
    Returns the directory path of the URL. This is the part of the
    path of the URL without the fileName(). See the documentation of
    fileName() for a discussion of what is handled as file name and
    what is handled as directory path.

    \sa setPath() hasPath()
*/

QString Q3Url::dirPath() const
{
    if ( path().isEmpty() )
	return QString();

    QString s = path();
    int pos = s.findRev( QLatin1Char('/') );
    if ( pos == -1 ) {
	return QString::fromLatin1( "." );
    } else {
	if ( pos == 0 )
	    return QString::fromLatin1( "/" );
	return s.left( pos );
    }
}

/*!
    Encodes the \a url in-place into UTF-8.  For example

    \snippet doc/src/snippets/code/src_qt3support_network_q3url.cpp 5

  \sa decode()
*/

void Q3Url::encode( QString& url )
{
    if ( url.isEmpty() )
	return;

    Q3CString curl = url.utf8();
    int oldlen = curl.length();

    const Q3CString special( "+<>#@\"&%$:,;?={}|^~[]\'`\\ \n\t\r" );
    QString newUrl;
    int newlen = 0;

    for ( int i = 0; i < oldlen ;++i ) {
	uchar inCh = (uchar)curl[ i ];

	if ( inCh >= 128 || special.contains(inCh) ) {
	    newUrl[ newlen++ ] = QLatin1Char( '%' );

	    ushort c = inCh / 16;
	    c += c > 9 ? 'A' - 10 : '0';
	    newUrl[ newlen++ ] = c;

	    c = inCh % 16;
	    c += c > 9 ? 'A' - 10 : '0';
	    newUrl[ newlen++ ] = c;
	} else {
	    newUrl[ newlen++ ] = inCh;
	}
    }

    url = newUrl;
}

static uchar hex_to_int( uchar c )
{
    if ( c >= 'A' && c <= 'F' )
	return c - 'A' + 10;
    if ( c >= 'a' && c <= 'f')
	return c - 'a' + 10;
    if ( c >= '0' && c <= '9')
	return c - '0';
    return 0;
}

/*!
    Decodes the \a url in-place into UTF-8.  For example

    \snippet doc/src/snippets/code/src_qt3support_network_q3url.cpp 6

    \sa encode()
*/

void Q3Url::decode( QString& url )
{
    if ( url.isEmpty() )
	return;

    int newlen = 0;
    Q3CString curl = url.utf8();
    int oldlen = curl.length();

    Q3CString newUrl(oldlen);

    int i = 0;
    while ( i < oldlen ) {
	uchar c = (uchar)curl[ i++ ];
	if ( c == '%' && i <= oldlen - 2 ) {
	    c = hex_to_int( (uchar)curl[ i ] ) * 16 + hex_to_int( (uchar)curl[ i + 1 ] );
	    i += 2;
	}
	newUrl [ newlen++ ] = c;
    }
    newUrl.truncate( newlen );

    url = QString::fromUtf8(newUrl.data());
}


/*!
    Composes a string version of the URL and returns it. If \a
    encodedPath is true the path in the returned string is encoded. If
    \a forcePrependProtocol is true and \a encodedPath looks like a
    local filename, the "file:/" protocol is also prepended.

    \sa encode() decode()
*/

QString Q3Url::toString( bool encodedPath, bool forcePrependProtocol ) const
{
    QString res, p = path();
    if ( encodedPath )
	encode( p );

    if ( isLocalFile() ) {
	if ( forcePrependProtocol )
	    res = d->protocol + QLatin1String(":") + p;
	else
	    res = p;
    } else if ( d->protocol == QLatin1String("mailto") ) {
	res = d->protocol + QLatin1String(":") + p;
    } else {
	res = d->protocol + QLatin1String("://");
	if ( !d->user.isEmpty() || !d->pass.isEmpty() ) {
	    QString tmp;
	    if ( !d->user.isEmpty() ) {
		tmp = d->user;
		encode( tmp );
		res += tmp;
	    }
	    if ( !d->pass.isEmpty() ) {
		tmp = d->pass;
		encode( tmp );
		res += QLatin1String(":") + tmp;
	    }
	    res += QLatin1String("@");
	}
	res += d->host;
	if ( d->port != -1 )
	    res += QLatin1String(":") + QString( QLatin1String("%1") ).arg( d->port );
	if ( !p.isEmpty() ) {
	    if ( !d->host.isEmpty() && p[0]!= QLatin1Char( '/' ) )
		res += QLatin1String("/");
	    res += p;
	}
    }

    if ( !d->refEncoded.isEmpty() )
	res += QLatin1String("#") + d->refEncoded;
    if ( !d->queryEncoded.isEmpty() )
	res += QLatin1String("?") + d->queryEncoded;

    return res;
}

/*!
    Composes a string version of the URL and returns it.

    \sa Q3Url::toString()
*/

Q3Url::operator QString() const
{
    return toString();
}

/*!
    Changes the directory to one directory up. This function always returns
    true.

    \sa setPath()
*/

bool Q3Url::cdUp()
{
    d->path += QLatin1String("/..");
    d->cleanPathDirty = true;
    return true;
}

QT_END_NAMESPACE

#endif // QT_NO_URL
