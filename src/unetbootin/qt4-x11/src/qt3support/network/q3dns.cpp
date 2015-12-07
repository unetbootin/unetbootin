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

#include "qplatformdefs.h"
#include "qbytearray.h"
#if defined(Q_OS_WIN32) || defined(Q_OS_WINCE) || defined(Q_OS_CYGWIN)
# include "qt_windows.h"
#else
# include <sys/types.h>
# include <netinet/in.h>
# include <arpa/nameser.h>
# include <resolv.h>
extern "C" int res_init();
#endif

// POSIX Large File Support redefines open -> open64
#if defined(open)
# undef open
#endif

// POSIX Large File Support redefines truncate -> truncate64
#if defined(truncate)
# undef truncate
#endif

// Solaris redefines connect -> __xnet_connect with _XOPEN_SOURCE_EXTENDED.
#if defined(connect)
# undef connect
#endif

// UnixWare 7 redefines socket -> _socket
#if defined(socket)
# undef socket
#endif

#include "q3dns.h"

#ifndef QT_NO_DNS

#include "qdatetime.h"
#include "q3dict.h"
#include "q3ptrlist.h"
#include "qstring.h"
#include "qtimer.h"
#include "qapplication.h"
#include "q3ptrvector.h"
#include "q3strlist.h"
#include "q3ptrdict.h"
#include "qfile.h"
#include "qtextstream.h"
#include "q3socketdevice.h"
#include "q3cleanuphandler.h"
#include <limits.h>

QT_BEGIN_NAMESPACE

//#define Q3DNS_DEBUG

static Q_UINT16 theId; // ### seeded started by now()


static QDateTime * originOfTime = 0;

static Q3CleanupHandler<QDateTime> q3dns_cleanup_time;

static Q_UINT32 now()
{
    if ( originOfTime )
	return originOfTime->secsTo( QDateTime::currentDateTime() );

    originOfTime = new QDateTime( QDateTime::currentDateTime() );
    theId = originOfTime->time().msec() * 60 + originOfTime->time().second();
    q3dns_cleanup_time.add( &originOfTime );
    return 0;
}


static Q3PtrList<QHostAddress> * theNs = 0;
static Q3StrList * theDomains = 0;
static bool ipv6support = false;

class Q3DnsPrivate {
public:
    Q3DnsPrivate() : queryTimer( 0 ), noNames(false)
    {
#if defined(Q_DNS_SYNCHRONOUS)
#if defined(Q_OS_UNIX)
	noEventLoop = qApp==0 || qApp->loopLevel()==0;
#else
	noEventLoop = false;
#endif
#endif
    }
    ~Q3DnsPrivate()
    {
	delete queryTimer;
    }
private:
    QTimer * queryTimer;
    bool noNames;
#if defined(Q_DNS_SYNCHRONOUS)
    bool noEventLoop;
#endif

    friend class Q3Dns;
    friend class Q3DnsAnswer;
};


class Q3DnsRR;
class Q3DnsDomain;



// Q3DnsRR is the class used to store a single RR.  Q3DnsRR can store
// all of the supported RR types.  a Q3DnsRR is always cached.

// Q3DnsRR is mostly constructed from the outside.  a but hacky, but
// permissible since the entire class is internal.

class Q3DnsRR {
public:
    Q3DnsRR( const QString & label );
    ~Q3DnsRR();

public:
    Q3DnsDomain * domain;
    Q3Dns::RecordType t;
    bool nxdomain;
    bool current;
    Q_UINT32 expireTime;
    Q_UINT32 deleteTime;
    // somewhat space-wasting per-type data
    // a / aaaa
    QHostAddress address;
    // cname / mx / srv / ptr
    QString target;
    // mx / srv
    Q_UINT16 priority;
    // srv
    Q_UINT16 weight;
    Q_UINT16 port;
    // txt
    QString text; // could be overloaded into target...
private:

};


class Q3DnsDomain {
public:
    Q3DnsDomain( const QString & label );
    ~Q3DnsDomain();

    static void add( const QString & label, Q3DnsRR * );
    static Q3PtrList<Q3DnsRR> * cached( const Q3Dns * );

    void take( Q3DnsRR * );

    void sweep( Q_UINT32 thisSweep );

    bool isEmpty() const { return rrs == 0 || rrs->isEmpty(); }

    QString name() const { return l; }

public:
    QString l;
    Q3PtrList<Q3DnsRR> * rrs;
};


class Q3DnsQuery: public QTimer { // this inheritance is a very evil hack
public:
    Q3DnsQuery():
	id( 0 ), t( Q3Dns::None ), step(0), started(0),
	dns( new Q3PtrDict<void>(17) ) {}
    ~Q3DnsQuery() { delete dns; }
    Q_UINT16 id;
    Q3Dns::RecordType t;
    QString l;

    uint step;
    Q_UINT32 started;

    Q3PtrDict<void> * dns;
};



class Q3DnsAnswer {
public:
    Q3DnsAnswer( Q3DnsQuery * );
    Q3DnsAnswer( const QByteArray &, Q3DnsQuery * );
    ~Q3DnsAnswer();

    void parse();
    void notify();

    bool ok;

private:
    Q3DnsQuery * query;

    Q_UINT8 * answer;
    int size;
    int pp;

    Q3PtrList<Q3DnsRR> * rrs;

    // convenience
    int next;
    int ttl;
    QString label;
    Q3DnsRR * rr;

    QString readString(bool multipleLabels = true);
    void parseA();
    void parseAaaa();
    void parseMx();
    void parseSrv();
    void parseCname();
    void parsePtr();
    void parseTxt();
    void parseNs();
};


Q3DnsRR::Q3DnsRR( const QString & label )
    : domain( 0 ), t( Q3Dns::None ),
      nxdomain( false ), current( false ),
      expireTime( 0 ), deleteTime( 0 ),
      priority( 0 ), weight( 0 ), port( 0 )
{
    Q3DnsDomain::add( label, this );
}


// not supposed to be deleted except by Q3DnsDomain
Q3DnsRR::~Q3DnsRR()
{
    // nothing is necessary
}


// this one just sticks in a NXDomain
Q3DnsAnswer::Q3DnsAnswer( Q3DnsQuery * query_ )
{
    ok = true;

    answer = 0;
    size = 0;
    query = query_;
    pp = 0;
    rrs = new Q3PtrList<Q3DnsRR>;
    rrs->setAutoDelete( false );
    next = size;
    ttl = 0;
    label.clear();
    rr = 0;

    Q3DnsRR * newrr = new Q3DnsRR( query->l );
    newrr->t = query->t;
    newrr->deleteTime = query->started + 10;
    newrr->expireTime = query->started + 10;
    newrr->nxdomain = true;
    newrr->current = true;
    rrs->append( newrr );
}


Q3DnsAnswer::Q3DnsAnswer( const QByteArray& answer_,
			Q3DnsQuery * query_ )
{
    ok = true;

    answer = (Q_UINT8 *)(answer_.data());
    size = (int)answer_.size();
    query = query_;
    pp = 0;
    rrs = new Q3PtrList<Q3DnsRR>;
    rrs->setAutoDelete( false );
    next = size;
    ttl = 0;
    label.clear();
    rr = 0;
}


Q3DnsAnswer::~Q3DnsAnswer()
{
    if ( !ok && rrs ) {
	Q3PtrListIterator<Q3DnsRR> it( *rrs );
	Q3DnsRR * tmprr;
	while( (tmprr=it.current()) != 0 ) {
	    ++it;
	    tmprr->t = Q3Dns::None; // will be deleted soonish
	}
    }
    delete rrs;
}


QString Q3DnsAnswer::readString(bool multipleLabels)
{
    int p = pp;
    QString r;
    Q_UINT8 b;
    for( ;; ) {
	b = 128;
        // Read one character
        if ( p >= 0 && p < size )
	    b = answer[p];

	switch( b >> 6 ) {
	case 0:
            // b is less than 64
	    p++;

            // Detect end of data
	    if ( b == 0 ) {
		if ( p > pp )
		    pp = p;
                return r.isNull() ? QLatin1String( "." ) : r;
	    }

            // Read a label of size 'b' characters
            if ( !r.isNull() )
		r += QLatin1Char('.');
	    while( b-- > 0 )
                r += QLatin1Char( answer[p++] );

            // Return immediately if we were only supposed to read one
            // label.
            if (!multipleLabels)
                return r;

	    break;
	default:
            // Ignore unrecognized control character, or p was out of
            // range.
	    goto not_ok;
	case 3:
            // Use the next character to determine the relative offset
            // to jump to before continuing the packet parsing.
	    int q = ( (answer[p] & 0x3f) << 8 ) + answer[p+1];

	    if ( q >= pp || q >= p )
		goto not_ok;
	    if ( p >= pp )
		pp = p + 2;
	    p = q;
        }
    }
not_ok:
    ok = false;
    return QString();
}



void Q3DnsAnswer::parseA()
{
    if ( next != pp + 4 ) {
#if defined(Q3DNS_DEBUG)
	qDebug( "Q3Dns: saw %d bytes long IN A for %s",
		next - pp, label.ascii() );
#endif
	return;
    }

    rr = new Q3DnsRR( label );
    rr->t = Q3Dns::A;
    rr->address = QHostAddress( ( answer[pp+0] << 24 ) +
				( answer[pp+1] << 16 ) +
				( answer[pp+2] <<  8 ) +
				( answer[pp+3] ) );
#if defined(Q3DNS_DEBUG)
    qDebug( "Q3Dns: saw %s IN A %s (ttl %d)", label.ascii(),
	    rr->address.toString().ascii(), ttl );
#endif
}


void Q3DnsAnswer::parseAaaa()
{
    if ( next != pp + 16 ) {
#if defined(Q3DNS_DEBUG)
	qDebug( "Q3Dns: saw %d bytes long IN Aaaa for %s",
		next - pp, label.ascii() );
#endif
	return;
    }

    rr = new Q3DnsRR( label );
    rr->t = Q3Dns::Aaaa;
    rr->address = QHostAddress( answer+pp );
#if defined(Q3DNS_DEBUG)
    qDebug( "Q3Dns: saw %s IN Aaaa %s (ttl %d)", label.ascii(),
	    rr->address.toString().ascii(), ttl );
#endif
}



void Q3DnsAnswer::parseMx()
{
    if ( next < pp + 2 ) {
#if defined(Q3DNS_DEBUG)
	qDebug( "Q3Dns: saw %d bytes long IN MX for %s",
		next - pp, label.ascii() );
#endif
	return;
    }

    rr = new Q3DnsRR( label );
    rr->priority = (answer[pp] << 8) + answer[pp+1];
    pp += 2;
    rr->target = readString().lower();
    if ( !ok ) {
#if defined(Q3DNS_DEBUG)
	qDebug( "Q3Dns: saw bad string in MX for %s", label.ascii() );
#endif
	return;
    }
    rr->t = Q3Dns::Mx;
#if defined(Q3DNS_DEBUG)
    qDebug( "Q3Dns: saw %s IN MX %d %s (ttl %d)", label.ascii(),
	    rr->priority, rr->target.ascii(), ttl );
#endif
}


void Q3DnsAnswer::parseSrv()
{
    if ( next < pp + 6 ) {
#if defined(Q3DNS_DEBUG)
	qDebug( "Q3Dns: saw %d bytes long IN SRV for %s",
		next - pp, label.ascii() );
#endif
	return;
    }

    rr = new Q3DnsRR( label );
    rr->priority = (answer[pp] << 8) + answer[pp+1];
    rr->weight = (answer[pp+2] << 8) + answer[pp+3];
    rr->port = (answer[pp+4] << 8) + answer[pp+5];
    pp += 6;
    rr->target = readString().lower();
    if ( !ok ) {
#if defined(Q3DNS_DEBUG)
	qDebug( "Q3Dns: saw bad string in SRV for %s", label.ascii() );
#endif
	return;
    }
    rr->t = Q3Dns::Srv;
#if defined(Q3DNS_DEBUG)
    qDebug( "Q3Dns: saw %s IN SRV %d %d %d %s (ttl %d)", label.ascii(),
	    rr->priority, rr->weight, rr->port, rr->target.ascii(), ttl );
#endif
}


void Q3DnsAnswer::parseCname()
{
    QString target = readString().lower();
    if ( !ok ) {
#if defined(Q3DNS_DEBUG)
	qDebug( "Q3Dns: saw bad cname for for %s", label.ascii() );
#endif
	return;
    }

    rr = new Q3DnsRR( label );
    rr->t = Q3Dns::Cname;
    rr->target = target;
#if defined(Q3DNS_DEBUG)
    qDebug( "Q3Dns: saw %s IN CNAME %s (ttl %d)", label.ascii(),
	    rr->target.ascii(), ttl );
#endif
}


void Q3DnsAnswer::parseNs()
{
    QString target = readString().lower();
    if ( !ok ) {
#if defined(Q3DNS_DEBUG)
	qDebug( "Q3Dns: saw bad cname for for %s", label.ascii() );
#endif
	return;
    }

    // parse, but ignore

#if defined(Q3DNS_DEBUG)
    qDebug( "Q3Dns: saw %s IN NS %s (ttl %d)", label.ascii(),
	    target.ascii(), ttl );
#endif
}


void Q3DnsAnswer::parsePtr()
{
    QString target = readString().lower();
    if ( !ok ) {
#if defined(Q3DNS_DEBUG)
	qDebug( "Q3Dns: saw bad PTR for for %s", label.ascii() );
#endif
	return;
    }

    rr = new Q3DnsRR( label );
    rr->t = Q3Dns::Ptr;
    rr->target = target;
#if defined(Q3DNS_DEBUG)
    qDebug( "Q3Dns: saw %s IN PTR %s (ttl %d)", label.ascii(),
	    rr->target.ascii(), ttl );
#endif
}


void Q3DnsAnswer::parseTxt()
{
    QString text = readString(false);
    if ( !ok ) {
#if defined(Q3DNS_DEBUG)
	qDebug( "Q3Dns: saw bad TXT for for %s", label.ascii() );
#endif
	return;
    }

    rr = new Q3DnsRR( label );
    rr->t = Q3Dns::Txt;
    rr->text = text;
#if defined(Q3DNS_DEBUG)
    qDebug( "Q3Dns: saw %s IN TXT \"%s\" (ttl %d)", label.ascii(),
	    rr->text.ascii(), ttl );
#endif
}


void Q3DnsAnswer::parse()
{
    // okay, do the work...
    if ( (answer[2] & 0x78) != 0 ) {
#if defined(Q3DNS_DEBUG)
	qDebug( "DNS Manager: answer to wrong query type (%d)", answer[1] );
#endif
	ok = false;
	return;
    }

    // AA
    bool aa = (answer[2] & 4) != 0;

    // TC
    if ( (answer[2] & 2) != 0 ) {
#if defined(Q3DNS_DEBUG)
	qDebug( "DNS Manager: truncated answer; pressing on" );
#endif
    }

    // RD
    bool rd = (answer[2] & 1) != 0;

    // we don't test RA
    // we don't test the MBZ fields

    if ( (answer[3] & 0x0f) == 3 ) {
#if defined(Q3DNS_DEBUG)
	qDebug( "DNS Manager: saw NXDomain for %s", query->l.ascii() );
#endif
	// NXDomain.  cache that for one minute.
	rr = new Q3DnsRR( query->l );
	rr->t = query->t;
	rr->deleteTime = query->started + 60;
	rr->expireTime = query->started + 60;
	rr->nxdomain = true;
	rr->current = true;
	rrs->append( rr );
	return;
    }

    if ( (answer[3] & 0x0f) != 0 ) {
#if defined(Q3DNS_DEBUG)
	qDebug( "DNS Manager: error code %d", answer[3] & 0x0f );
#endif
	ok = false;
	return;
    }

    int qdcount = ( answer[4] << 8 ) + answer[5];
    int ancount = ( answer[6] << 8 ) + answer[7];
    int nscount = ( answer[8] << 8 ) + answer[9];
    int adcount = (answer[10] << 8 ) +answer[11];

    pp = 12;

    // read query
    while( qdcount > 0 && pp < size ) {
	// should I compare the string against query->l?
	(void)readString();
	if ( !ok )
	    return;
	pp += 4;
	qdcount--;
    }

    // answers and stuff
    int rrno = 0;
    // if we parse the answer completely, but there are no answers,
    // ignore the entire thing.
    int answers = 0;
    while( ( rrno < ancount ||
	     ( ok && answers >0 && rrno < ancount + nscount + adcount ) ) &&
	   pp < size ) {
	label = readString().lower();
	if ( !ok )
	    return;
	int rdlength = 0;
	if ( pp + 10 <= size )
	    rdlength = ( answer[pp+8] << 8 ) + answer[pp+9];
	if ( pp + 10 + rdlength > size ) {
#if defined(Q3DNS_DEBUG)
	    qDebug( "DNS Manager: ran out of stuff to parse (%d+%d>%d (%d)",
		    pp, rdlength, size, rrno < ancount );
#endif
	    // if we're still in the AN section, we should go back and
	    // at least down the TTLs.  probably best to invalidate
	    // the results.
	    // the rrs list is good for this
	    ok = ( rrno < ancount );
	    return;
	}
	uint type, clas;
	type = ( answer[pp+0] << 8 ) + answer[pp+1];
	clas = ( answer[pp+2] << 8 ) + answer[pp+3];
	ttl = ( answer[pp+4] << 24 ) + ( answer[pp+5] << 16 ) +
	      ( answer[pp+6] <<  8 ) + answer[pp+7];
	pp = pp + 10;
	if ( clas != 1 ) {
#if defined(Q3DNS_DEBUG)
	    qDebug( "DNS Manager: class %d (not internet) for %s",
		    clas, label.isNull() ? "." : label.ascii() );
#endif
	} else {
            next = pp + rdlength;
	    rr = 0;
	    switch( type ) {
	    case 1:
		parseA();
		break;
	    case 28:
		parseAaaa();
		break;
	    case 15:
		parseMx();
		break;
	    case 33:
		parseSrv();
		break;
	    case 5:
		parseCname();
		break;
	    case 12:
		parsePtr();
		break;
	    case 16:
		parseTxt();
		break;
	    case 2:
		parseNs();
		break;
	    default:
		// something we don't know
#if defined(Q3DNS_DEBUG)
		qDebug( "DNS Manager: type %d for %s", type,
			label.isNull() ? "." : label.ascii() );
#endif
		break;
	    }
	    if ( rr ) {
		rr->deleteTime = 0;
		if ( ttl > 0 )
		    rr->expireTime = query->started + ttl;
		else
		    rr->expireTime = query->started + 20;
		if ( rrno < ancount ) {
		    answers++;
		    rr->deleteTime = rr->expireTime;
		}
		rr->current = true;
		rrs->append( rr );
	    }
        }
	if ( !ok )
	    return;
	pp = next;
	next = size;
	rrno++;
    }
    if ( answers == 0 ) {
#if defined(Q3DNS_DEBUG)
	qDebug( "DNS Manager: answer contained no answers" );
#endif
	ok = ( aa && rd );
    }

    // now go through the list and mark all the As that are referenced
    // by something we care about.  we want to cache such As.
    rrs->first();
    Q3Dict<void> used( 17 );
    used.setAutoDelete( false );
    while( (rr=rrs->current()) != 0 ) {
	rrs->next();
	if ( rr->target.length() && rr->deleteTime > 0 && rr->current )
	    used.insert( rr->target, (void*)42 );
	if ( ( rr->t == Q3Dns::A || rr->t == Q3Dns::Aaaa ) &&
	     used.find( rr->domain->name() ) != 0 )
	    rr->deleteTime = rr->expireTime;
    }

    // next, for each RR, delete any older RRs that are equal to it
    rrs->first();
    while( (rr=rrs->current()) != 0 ) {
	rrs->next();
	if ( rr && rr->domain && rr->domain->rrs ) {
	    Q3PtrList<Q3DnsRR> * drrs = rr->domain->rrs;
	    drrs->first();
	    Q3DnsRR * older;
	    while( (older=drrs->current()) != 0 ) {
		if ( older != rr &&
		     older->t == rr->t &&
		     older->nxdomain == rr->nxdomain &&
		     older->address == rr->address &&
		     older->target == rr->target &&
		     older->priority == rr->priority &&
		     older->weight == rr->weight &&
		     older->port == rr->port &&
		     older->text == rr->text ) {
		    // well, it's equal, but it's not the same. so we kill it,
		    // but use its expiry time.
#if defined(Q3DNS_DEBUG)
		    qDebug( "killing off old %d for %s, expire was %d",
                            older->t, older->domain->name().latin1(),
                            rr->expireTime );
#endif
		    older->t = Q3Dns::None;
		    rr->expireTime = QMAX( older->expireTime, rr->expireTime );
		    rr->deleteTime = QMAX( older->deleteTime, rr->deleteTime );
		    older->deleteTime = 0;
#if defined(Q3DNS_DEBUG)
		    qDebug( "    adjusted expire is %d", rr->expireTime );
#endif
		}
		drrs->next();
	    }
	}
    }

#if defined(Q3DNS_DEBUG)
    //qDebug( "DNS Manager: ()" );
#endif
}


class Q3DnsUgleHack: public Q3Dns {
public:
    void ugle( bool emitAnyway=false );
};


void Q3DnsAnswer::notify()
{
    if ( !rrs || !ok || !query || !query->dns )
	return;

    Q3PtrDict<void> notified;
    notified.setAutoDelete( false );

    Q3PtrDictIterator<void> it( *query->dns );
    Q3Dns * dns;
    it.toFirst();
    while( (dns=(Q3Dns*)(it.current())) != 0 ) {
	++it;
	if ( notified.find( (void*)dns ) == 0 ) {
	    notified.insert( (void*)dns, (void*)42 );
	    if ( rrs->count() == 0 ) {
#if defined(Q3DNS_DEBUG)
		qDebug( "DNS Manager: found no answers!" );
#endif
		dns->d->noNames = true;
		((Q3DnsUgleHack*)dns)->ugle( true );
	    } else {
		QStringList n = dns->qualifiedNames();
		if ( query && n.contains(query->l) )
		    ((Q3DnsUgleHack*)dns)->ugle();
#if defined(Q3DNS_DEBUG)
		else
		    qDebug( "DNS Manager: DNS thing %s not notified for %s",
			    dns->label().ascii(), query->l.ascii() );
#endif
	    }
	}
    }
}


//
//
// Q3DnsManager
//
//


class Q3DnsManager: public Q3DnsSocket {
private:
public: // just to silence the moronic g++.
    Q3DnsManager();
    ~Q3DnsManager();
public:
    static Q3DnsManager * manager();

    Q3DnsDomain * domain( const QString & );

    void transmitQuery( Q3DnsQuery * );
    void transmitQuery( int );

    // reimplementation of the slots
    void cleanCache();
    void retransmit();
    void answer();

public:
    Q3PtrVector<Q3DnsQuery> queries;
    Q3Dict<Q3DnsDomain> cache;
    Q3SocketDevice * ipv4Socket;
#if !defined (QT_NO_IPV6)
    Q3SocketDevice * ipv6Socket;
#endif
};



static Q3DnsManager * globalManager = 0;

static void cleanupDns()
{
    delete globalManager;
    globalManager = 0;
}

Q3DnsManager * Q3DnsManager::manager()
{
    if ( !globalManager ) {
        qAddPostRoutine(cleanupDns);
	new Q3DnsManager();
    }
    return globalManager;
}


void Q3DnsUgleHack::ugle( bool emitAnyway)
{
    if ( emitAnyway || !isWorking() ) {
#if defined(Q3DNS_DEBUG)
	qDebug( "DNS Manager: status change for %s (type %d)",
		label().ascii(), recordType() );
#endif
	emit resultsReady();
    }
}


Q3DnsManager::Q3DnsManager()
    : Q3DnsSocket( qApp, "Internal DNS manager" ),
      queries( Q3PtrVector<Q3DnsQuery>( 0 ) ),
      cache( Q3Dict<Q3DnsDomain>( 83, false ) ),
      ipv4Socket( new Q3SocketDevice( Q3SocketDevice::Datagram, Q3SocketDevice::IPv4, 0 ) )
#if !defined (QT_NO_IPV6)
      , ipv6Socket( new Q3SocketDevice( Q3SocketDevice::Datagram, Q3SocketDevice::IPv6, 0 ) )
#endif
{
    cache.setAutoDelete( true );
    globalManager = this;

    QTimer * sweepTimer = new QTimer( this );
    sweepTimer->start( 1000 * 60 * 3 );
    connect( sweepTimer, SIGNAL(timeout()),
	     this, SLOT(cleanCache()) );

    QSocketNotifier * rn4 = new QSocketNotifier( ipv4Socket->socket(),
						 QSocketNotifier::Read,
						 this, "dns IPv4 socket watcher" );
    ipv4Socket->setAddressReusable( false );
    ipv4Socket->setBlocking( false );
    connect( rn4, SIGNAL(activated(int)), SLOT(answer()) );

#if !defined (QT_NO_IPV6)
    // Don't connect the IPv6 socket notifier if the host does not
    // support IPv6.
    if ( ipv6Socket->socket() != -1 ) {
	QSocketNotifier * rn6 = new QSocketNotifier( ipv6Socket->socket(),
						     QSocketNotifier::Read,
						     this, "dns IPv6 socket watcher" );

	ipv6support = true;
	ipv6Socket->setAddressReusable( false );
	ipv6Socket->setBlocking( false );
	connect( rn6, SIGNAL(activated(int)), SLOT(answer()) );
    }
#endif

    if ( !theNs )
	Q3Dns::doResInit();

    // O(n*n) stuff here.  but for 3 and 6, O(n*n) with a low k should
    // be perfect.  the point is to eliminate any duplicates that
    // might be hidden in the lists.
    Q3PtrList<QHostAddress> * ns = new Q3PtrList<QHostAddress>;

    theNs->first();
    QHostAddress * h;
    while( (h=theNs->current()) != 0 ) {
	ns->first();
	while( ns->current() != 0 && !(*ns->current() == *h) )
	    ns->next();
	if ( !ns->current() ) {
	    ns->append( new QHostAddress(*h) );
#if defined(Q3DNS_DEBUG)
	    qDebug( "using name server %s", h->toString().latin1() );
	} else {
	    qDebug( "skipping address %s", h->toString().latin1() );
#endif
	}
	theNs->next();
    }

    delete theNs;
    theNs = ns;
    theNs->setAutoDelete( true );

    Q3StrList * domains = new Q3StrList( true );

    theDomains->first();
    const char * s;
    while( (s=theDomains->current()) != 0 ) {
	domains->first();
	while( domains->current() != 0 && qstrcmp( domains->current(), s ) )
	    domains->next();
	if ( !domains->current() ) {
	    domains->append( s );
#if defined(Q3DNS_DEBUG)
	    qDebug( "searching domain %s", s );
	} else {
	    qDebug( "skipping domain %s", s );
#endif
	}
	theDomains->next();
    }

    delete theDomains;
    theDomains = domains;
    theDomains->setAutoDelete( true );
}


Q3DnsManager::~Q3DnsManager()
{
    if ( globalManager )
	globalManager = 0;
    queries.setAutoDelete( true );
    cache.setAutoDelete( true );
    delete ipv4Socket;
#if !defined (QT_NO_IPV6)
    delete ipv6Socket;
#endif
}

static Q_UINT32 lastSweep = 0;

void Q3DnsManager::cleanCache()
{
    bool again = false;
    Q3DictIterator<Q3DnsDomain> it( cache );
    Q3DnsDomain * d;
    Q_UINT32 thisSweep = now();
#if defined(Q3DNS_DEBUG)
    qDebug( "Q3DnsManager::cleanCache(: Called, time is %u, last was %u",
	   thisSweep, lastSweep );
#endif

    while( (d=it.current()) != 0 ) {
	++it;
	d->sweep( thisSweep ); // after this, d may be empty
	if ( !again )
	    again = !d->isEmpty();
    }
    if ( !again )
	delete this;
    lastSweep = thisSweep;
}


void Q3DnsManager::retransmit()
{
    const QObject * o = sender();
    if ( o == 0 || globalManager == 0 || this != globalManager )
	return;
    uint q = 0;
    while( q < queries.size() && queries[q] != o )
	q++;
    if ( q < queries.size() )
	transmitQuery( q );
}


void Q3DnsManager::answer()
{
    QByteArray a( 16383 ); // large enough for anything, one suspects

    int r;
#if defined (QT_NO_IPV6)
    r = ipv4Socket->readBlock(a.data(), a.size());
#else
    if (((QSocketNotifier *)sender())->socket() == ipv4Socket->socket())
        r = ipv4Socket->readBlock(a.data(), a.size());
    else
        r = ipv6Socket->readBlock(a.data(), a.size());
#endif
#if defined(Q3DNS_DEBUG)
#if !defined (QT_NO_IPV6)
    qDebug("DNS Manager: answer arrived: %d bytes from %s:%d", r,
	   useIpv4Socket ? ipv4Socket->peerAddress().toString().ascii()
	   : ipv6Socket->peerAddress().toString().ascii(),
	   useIpv4Socket ? ipv4Socket->peerPort() : ipv6Socket->peerPort() );
#else
    qDebug("DNS Manager: answer arrived: %d bytes from %s:%d", r,
           ipv4Socket->peerAddress().toString().ascii(), ipv4Socket->peerPort());;
#endif
#endif
    if ( r < 12 )
	return;
    // maybe we should check that the answer comes from port 53 on one
    // of our name servers...
    a.resize( r );

    Q_UINT16 aid = (((Q_UINT8)a[0]) << 8) + ((Q_UINT8)a[1]);
    uint i = 0;
    while( i < queries.size() &&
	   !( queries[i] && queries[i]->id == aid ) )
	i++;
    if ( i == queries.size() ) {
#if defined(Q3DNS_DEBUG)
	qDebug( "DNS Manager: bad id (0x%04x) %d", aid, i );
#endif
	return;
    }

    // at this point queries[i] is whatever we asked for.

    if ( ( (Q_UINT8)(a[2]) & 0x80 ) == 0 ) {
#if defined(Q3DNS_DEBUG)
	qDebug( "DNS Manager: received a query" );
#endif
	return;
    }

    Q3DnsQuery * q = queries[i];
    Q3DnsAnswer answer( a, q );
    answer.parse();
    if ( answer.ok ) {
	queries.take( i );
	answer.notify();
	delete q;
    }
}


void Q3DnsManager::transmitQuery( Q3DnsQuery * query_ )
{
    if ( !query_ )
	return;

    uint i = 0;
    while( i < queries.size() && queries[i] != 0 )
	i++;
    if ( i == queries.size() )
	queries.resize( i+1 );
    queries.insert( i, query_ );
    transmitQuery( i );
}


void Q3DnsManager::transmitQuery( int i )
{
    if ( i < 0 || i >= (int)queries.size() )
	return;
    Q3DnsQuery * q = queries[i];

    if ( q && q->step > 8 ) {
	// okay, we've run out of retransmissions. we fake an NXDomain
	// with a very short life time...
	Q3DnsAnswer answer( q );
	answer.notify();
	// and then get rid of the query
	queries.take( i );
#if defined(Q3DNS_DEBUG)
	qDebug( "DNS Manager: giving up on query 0x%04x", q->id );
#endif
	delete q;
	QTimer::singleShot( 0, Q3DnsManager::manager(), SLOT(cleanCache()) );
	// and don't process anything more
	return;
    }

    if ((q && !q->dns) || q->dns->isEmpty())
	// no one currently wants the answer, so there's no point in
	// retransmitting the query. we keep it, though. an answer may
	// arrive for an earlier query transmission, and if it does we
	// may benefit from caching the result.
	return;

    QByteArray p( 12 + q->l.length() + 2 + 4 );
    if ( p.size() > 500 )
	return; // way over the limit, so don't even try

    // header
    // id
    p[0] = (q->id & 0xff00) >> 8;
    p[1] =  q->id & 0x00ff;
    p[2] = 1; // recursion desired, rest is 0
    p[3] = 0; // all is 0
    // one query
    p[4] = 0;
    p[5] = 1;
    // no answers, name servers or additional data
    p[6] = p[7] = p[8] = p[9] = p[10] = p[11] = 0;

    // the name is composed of several components.  each needs to be
    // written by itself... so we write...
    // oh, and we assume that there's no funky characters in there.
    int pp = 12;
    uint lp = 0;
    while( lp < (uint) q->l.length() ) {
	int le = q->l.find( QLatin1Char('.'), lp );
	if ( le < 0 )
	    le = q->l.length();
	QString component = q->l.mid( lp, le-lp );
	p[pp++] = component.length();
	int cp;
	for( cp=0; cp < (int)component.length(); cp++ )
	    p[pp++] = component[cp].latin1();
	lp = le + 1;
    }
    // final null
    p[pp++] = 0;
    // query type
    p[pp++] = 0;
    switch( q->t ) {
    case Q3Dns::A:
	p[pp++] = 1;
	break;
    case Q3Dns::Aaaa:
	p[pp++] = 28;
	break;
    case Q3Dns::Mx:
	p[pp++] = 15;
	break;
    case Q3Dns::Srv:
	p[pp++] = 33;
	break;
    case Q3Dns::Cname:
	p[pp++] = 5;
	break;
    case Q3Dns::Ptr:
	p[pp++] = 12;
	break;
    case Q3Dns::Txt:
	p[pp++] = 16;
	break;
    default:
	p[pp++] = (char)255; // any
	break;
    }
    // query class (always internet)
    p[pp++] = 0;
    p[pp++] = 1;

    // if we have no name servers, we should regenerate ns in case
    // name servers have recently been defined (like on windows,
    // plugging/unplugging the network cable will change the name
    // server entries)
    if ( !theNs || theNs->isEmpty() )
        Q3Dns::doResInit();

    if ( !theNs || theNs->isEmpty() ) {
	// we don't find any name servers. We fake an NXDomain
	// with a very short life time...
	Q3DnsAnswer answer( q );
	answer.notify();
	// and then get rid of the query
	queries.take( i );
#if defined(Q3DNS_DEBUG)
	qDebug( "DNS Manager: no DNS server found on query 0x%04x", q->id );
#endif
	delete q;
	QTimer::singleShot( 1000*10, Q3DnsManager::manager(), SLOT(cleanCache()) );
	// and don't process anything more
	return;
    }

    QHostAddress receiver = *theNs->at( q->step % theNs->count() );
    if (receiver.isIPv4Address())
	ipv4Socket->writeBlock( p.data(), pp, receiver, 53 );
#if !defined (QT_NO_IPV6)
    else
	ipv6Socket->writeBlock( p.data(), pp, receiver, 53 );
#endif
#if defined(Q3DNS_DEBUG)
    qDebug( "issuing query 0x%04x (%d) about %s type %d to %s",
	    q->id, q->step, q->l.ascii(), q->t,
	    ns->at( q->step % ns->count() )->toString().ascii() );
#endif
    if ( theNs->count() > 1 && q->step == 0 && queries.count() == 1 ) {
	// if it's the first time, and we don't have any other
	// outstanding queries, send nonrecursive queries to the other
	// name servers too.
	p[2] = 0;
	QHostAddress * server;
	while( (server=theNs->next()) != 0 ) {
	    if (server->isIPv4Address())
		ipv4Socket->writeBlock( p.data(), pp, *server, 53 );
#if !defined (QT_NO_IPV6)
	    else
		ipv6Socket->writeBlock( p.data(), pp, *server, 53 );
#endif
#if defined(Q3DNS_DEBUG)
	    qDebug( "copying query to %s", server->toString().ascii() );
#endif
	}
    }
    q->step++;
    // some testing indicates that normal dns queries take up to 0.6
    // seconds.  the graph becomes steep around that point, and the
    // number of errors rises... so it seems good to retry at that
    // point.
    q->start( q->step < theNs->count() ? 800 : 1500, true );
}


Q3DnsDomain * Q3DnsManager::domain( const QString & label )
{
    Q3DnsDomain * d = cache.find( label );
    if ( !d ) {
	d = new Q3DnsDomain( label );
	cache.insert( label, d );
    }
    return d;
}


//
//
// the Q3DnsDomain class looks after and coordinates queries for Q3DnsRRs for
// each domain, and the cached Q3DnsRRs.  (A domain, in DNS terminology, is
// a node in the DNS.  "no", "trolltech.com" and "lupinella.troll.no" are
// all domains.)
//
//


Q3DnsDomain::Q3DnsDomain( const QString & label )
{
    l = label;
    rrs = 0;
}


Q3DnsDomain::~Q3DnsDomain()
{
    delete rrs;
    rrs = 0;
}


void Q3DnsDomain::add( const QString & label, Q3DnsRR * rr )
{
    Q3DnsDomain * d = Q3DnsManager::manager()->domain( label );
    if ( !d->rrs ) {
	d->rrs = new Q3PtrList<Q3DnsRR>;
	d->rrs->setAutoDelete( true );
    }
    d->rrs->append( rr );
    rr->domain = d;
}


Q3PtrList<Q3DnsRR> * Q3DnsDomain::cached( const Q3Dns * r )
{
    Q3PtrList<Q3DnsRR> * l = new Q3PtrList<Q3DnsRR>;

    // test at first if you have to start a query at all
    if ( r->recordType() == Q3Dns::A ) {
	if ( r->label().lower() == QLatin1String("localhost") ) {
	    // undocumented hack. ipv4-specific. also, may be a memory
	    // leak? not sure. would be better to do this in doResInit(),
	    // anyway.
	    Q3DnsRR *rrTmp = new Q3DnsRR( r->label() );
	    rrTmp->t = Q3Dns::A;
	    rrTmp->address = QHostAddress( 0x7f000001 );
	    rrTmp->current = true;
	    l->append( rrTmp );
	    return l;
	}
	QHostAddress tmp;
	if ( tmp.setAddress( r->label() ) ) {
	    Q3DnsRR *rrTmp = new Q3DnsRR( r->label() );
	    if ( tmp.isIPv4Address() ) {
		rrTmp->t = Q3Dns::A;
                rrTmp->address = tmp;
                rrTmp->current = true;
                l->append( rrTmp );
            } else {
                rrTmp->nxdomain = true;
            }
	    return l;
	}
    }
    if ( r->recordType() == Q3Dns::Aaaa ) {
	QHostAddress tmp;
	if ( tmp.setAddress(r->label()) ) {
	    Q3DnsRR *rrTmp = new Q3DnsRR( r->label() );
	    if ( tmp.isIPv6Address() ) {
		rrTmp->t = Q3Dns::Aaaa;
                rrTmp->address = tmp;
                rrTmp->current = true;
                l->append( rrTmp );
            } else {
                rrTmp->nxdomain = true;
            }
	    return l;
	}
    }

    // if you reach this point, you have to do the query
    Q3DnsManager * m = Q3DnsManager::manager();
    QStringList n = r->qualifiedNames();
    bool nxdomain;
    int cnamecount = 0;
    int it = 0;
    while( it < n.count() ) {
	QString s = n.at(it++);
	nxdomain = false;
#if defined(Q3DNS_DEBUG)
	qDebug( "looking at cache for %s (%s %d)",
		s.ascii(), r->label().ascii(), r->recordType() );
#endif
	Q3DnsDomain * d = m->domain( s );
#if defined(Q3DNS_DEBUG)
	qDebug( " - found %d RRs", d && d->rrs ? d->rrs->count() : 0 );
#endif
	if ( d->rrs )
	    d->rrs->first();
	Q3DnsRR * rr;
	bool answer = false;
	while( d->rrs && (rr=d->rrs->current()) != 0 ) {
	    if ( rr->t == Q3Dns::Cname && r->recordType() != Q3Dns::Cname &&
		 !rr->nxdomain && cnamecount < 16 ) {
		// cname.  if the code is ugly, that may just
		// possibly be because the concept is.
#if defined(Q3DNS_DEBUG)
		qDebug( "found cname from %s to %s",
			r->label().ascii(), rr->target.ascii() );
#endif
		s = rr->target;
		d = m->domain( s );
		if ( d->rrs )
		    d->rrs->first();
		it = n.count();
		// we've elegantly moved over to whatever the cname
		// pointed to.  well, not elegantly.  let's remember
		// that we've done something, anyway, so we can't be
		// fooled into an infinte loop as well.
		cnamecount++;
	    } else {
		if ( rr->t == r->recordType() ) {
		    if ( rr->nxdomain )
			nxdomain = true;
		    else
			answer = true;
		    l->append( rr );
		    if ( rr->deleteTime <= lastSweep ) {
			// we're returning something that'll be
			// deleted soon.  we assume that if the client
			// wanted it twice, it'll want it again, so we
			// ask the name server again right now.
			Q3DnsQuery * query = new Q3DnsQuery;
			query->started = now();
			query->id = ++theId;
			query->t = rr->t;
			query->l = rr->domain->name();
			// note that here, we don't bother about
			// notification. but we do bother about
			// timeouts: we make sure to use high timeouts
			// and few tramsissions.
			query->step = theNs->count();
			QObject::connect( query, SIGNAL(timeout()),
					  Q3DnsManager::manager(),
					  SLOT(retransmit()) );
			Q3DnsManager::manager()->transmitQuery( query );
		    }
		}
		d->rrs->next();
	    }
	}
	// if we found a positive result, return quickly
	if ( answer && l->count() ) {
#if defined(Q3DNS_DEBUG)
	    qDebug( "found %d records for %s",
		    l->count(), r->label().ascii() );
	    l->first();
	    while( l->current() ) {
		qDebug( "  type %d target %s address %s",
		       l->current()->t,
		       l->current()->target.latin1(),
		       l->current()->address.toString().latin1() );
		l->next();
	    }
#endif
	    l->first();
	    return l;
	}

#if defined(Q3DNS_DEBUG)
	if ( nxdomain )
	    qDebug( "found NXDomain %s", s.ascii() );
#endif

	if ( !nxdomain ) {
	    // if we didn't, and not a negative result either, perhaps
	    // we need to transmit a query.
	    uint q = 0;
	    while ( q < m->queries.size() &&
		    ( m->queries[q] == 0 ||
		      m->queries[q]->t != r->recordType() ||
		      m->queries[q]->l != s ) )
		q++;
	    // we haven't done it before, so maybe we should.  but
	    // wait - if it's an unqualified name, only ask when all
	    // the other alternatives are exhausted.
	    if ( q == m->queries.size() && ( s.find( QLatin1Char('.') ) >= 0 ||
					     int(l->count()) >= n.count()-1 ) ) {
		Q3DnsQuery * query = new Q3DnsQuery;
		query->started = now();
		query->id = ++theId;
		query->t = r->recordType();
		query->l = s;
		query->dns->replace( (void*)r, (void*)r );
		QObject::connect( query, SIGNAL(timeout()),
				  Q3DnsManager::manager(), SLOT(retransmit()) );
		Q3DnsManager::manager()->transmitQuery( query );
	    } else if ( q < m->queries.size() ) {
		// if we've found an earlier query for the same
		// domain/type, subscribe to its answer
		m->queries[q]->dns->replace( (void*)r, (void*)r );
	    }
	}
    }
    l->first();
    return l;
}


void Q3DnsDomain::sweep( Q_UINT32 thisSweep )
{
    if ( !rrs )
	return;

    Q3DnsRR * rr;
    rrs->first();
    while( (rr=rrs->current()) != 0 ) {
	if ( !rr->deleteTime )
	    rr->deleteTime = thisSweep; // will hit next time around

#if defined(Q3DNS_DEBUG)
	qDebug( "Q3Dns::sweep: %s type %d expires %u %u - %s / %s",
	       rr->domain->name().latin1(), rr->t,
	       rr->expireTime, rr->deleteTime,
	       rr->target.latin1(), rr->address.toString().latin1());
#endif
	if ( rr->current == false ||
	     rr->t == Q3Dns::None ||
	     rr->deleteTime <= thisSweep ||
	     rr->expireTime <= thisSweep )
	    rrs->remove();
	else
	    rrs->next();
    }

    if ( rrs->isEmpty() ) {
	delete rrs;
	rrs = 0;
    }
}




// the itsy-bitsy little socket class I don't really need except for
// so I can subclass and reimplement the slots.


Q3DnsSocket::Q3DnsSocket( QObject * parent, const char * name )
    : QObject( parent, name )
{
    // nothing
}


Q3DnsSocket::~Q3DnsSocket()
{
    // nothing
}


void Q3DnsSocket::cleanCache()
{
    // nothing
}


void Q3DnsSocket::retransmit()
{
    // nothing
}


void Q3DnsSocket::answer()
{
    // nothing
}


/*!
    \class Q3Dns q3dns.h
    \brief The Q3Dns class provides asynchronous DNS lookups.

    \compat

    Both Windows and Unix provide synchronous DNS lookups; Windows
    provides some asynchronous support too. At the time of writing
    neither operating system provides asynchronous support for
    anything other than hostname-to-address mapping.

    Q3Dns rectifies this shortcoming, by providing asynchronous caching
    lookups for the record types that we expect modern GUI
    applications to need in the near future.

    The class is \e not straightforward to use (although it is much
    simpler than the native APIs); Q3Socket provides much easier to use
    TCP connection facilities. The aim of Q3Dns is to provide a correct
    and small API to the DNS and nothing more. (We use "correctness"
    to mean that the DNS information is correctly cached, and
    correctly timed out.)

    The API comprises a constructor, functions to set the DNS node
    (the domain in DNS terminology) and record type (setLabel() and
    setRecordType()), the corresponding get functions, an isWorking()
    function to determine whether Q3Dns is working or reading, a
    resultsReady() signal and query functions for the result.

    There is one query function for each RecordType, namely
    addresses(), mailServers(), servers(), hostNames() and texts().
    There are also two generic query functions: canonicalName()
    returns the name you'll presumably end up using (the exact meaning
    of this depends on the record type) and qualifiedNames() returns a
    list of the fully qualified names label() maps to.

    \sa Q3Socket
*/

/*!
    Constructs a DNS query object with invalid settings for both the
    label and the search type.
*/

Q3Dns::Q3Dns()
{
    d = new Q3DnsPrivate;
    t = None;
}




/*!
    Constructs a DNS query object that will return record type \a rr
    information about \a label.

    The DNS lookup is started the next time the application enters the
    event loop. When the result is found the signal resultsReady() is
    emitted.

    \a rr defaults to \c A, IPv4 addresses.
*/

Q3Dns::Q3Dns( const QString & label, RecordType rr )
{
    d = new Q3DnsPrivate;
    t = rr;
    setLabel( label );
    setStartQueryTimer(); // start query the next time we enter event loop
}



/*!
    Constructs a DNS query object that will return record type \a rr
    information about host address \a address. The label is set to the
    IN-ADDR.ARPA domain name. This is useful in combination with the
    \c Ptr record type (e.g. if you want to look up a hostname for a
    given address).

    The DNS lookup is started the next time the application enters the
    event loop. When the result is found the signal resultsReady() is
    emitted.

    \a rr defaults to \c Ptr, that maps addresses to hostnames.
*/

Q3Dns::Q3Dns( const QHostAddress & address, RecordType rr )
{
    d = new Q3DnsPrivate;
    t = rr;
    setLabel( address );
    setStartQueryTimer(); // start query the next time we enter event loop
}




/*!
    Destroys the DNS query object and frees its allocated resources.
*/

Q3Dns::~Q3Dns()
{
    if ( globalManager ) {
	uint q = 0;
	Q3DnsManager * m = globalManager;
	while( q < m->queries.size() ) {
	    Q3DnsQuery * query=m->queries[q];
	    if ( query && query->dns )
		    (void)query->dns->take( (void*) this );
		q++;
	}

    }

    delete d;
    d = 0;
}




/*!
    Sets this DNS query object to query for information about \a
    label.

    This does not change the recordType(), but its isWorking() status
    will probably change as a result.

    The DNS lookup is started the next time the application enters the
    event loop. When the result is found the signal resultsReady() is
    emitted.
*/

void Q3Dns::setLabel( const QString & label )
{
    l = label;
    d->noNames = false;

    // construct a list of qualified names
    n.clear();
    if ( l.length() > 1 && l[(int)l.length()-1] == QLatin1Char('.') ) {
	n.append( l.left( l.length()-1 ).lower() );
    } else {
	int i = l.length();
	int dots = 0;
	const int maxDots = 2;
	while( i && dots < maxDots ) {
	    if ( l[--i] == QLatin1Char('.') )
		dots++;
	}
	if ( dots < maxDots ) {
	    (void)Q3DnsManager::manager(); // create a Q3DnsManager, if it is not already there
	    Q3StrListIterator it( *theDomains );
	    const char * dom;
	    while( (dom=it.current()) != 0 ) {
		++it;
		n.append( l.lower() + QLatin1String(".") + QLatin1String(dom) );
	    }
	}
	n.append( l.lower() );
    }

#if defined(Q_DNS_SYNCHRONOUS)
    if ( d->noEventLoop ) {
	doSynchronousLookup();
    } else {
	setStartQueryTimer(); // start query the next time we enter event loop
    }
#else
    setStartQueryTimer(); // start query the next time we enter event loop
#endif
#if defined(Q3DNS_DEBUG)
    qDebug( "Q3Dns::setLabel: %d address(es) for %s", n.count(), l.ascii() );
    int i = 0;
    for( i = 0; i < (int)n.count(); i++ )
	qDebug( "Q3Dns::setLabel: %d: %s", i, n[i].ascii() );
#endif
}


/*!
    \overload

    Sets this DNS query object to query for information about the host
    address \a address. The label is set to the IN-ADDR.ARPA domain
    name. This is useful in combination with the \c Ptr record type
    (e.g. if you want to look up a hostname for a given address).
*/

void Q3Dns::setLabel( const QHostAddress & address )
{
    setLabel( toInAddrArpaDomain( address ) );
}


/*!
    \fn QStringList Q3Dns::qualifiedNames() const

    Returns a list of the fully qualified names label() maps to.

    Note that if you want to iterate over the list, you should iterate
    over a copy, e.g.
    \snippet doc/src/snippets/code/src_qt3support_network_q3dns.cpp 0

*/


/*!
    \fn QString Q3Dns::label() const

    Returns the domain name for which this object returns information.

    \sa setLabel()
*/

/*!
    \enum Q3Dns::RecordType

    This enum type defines the record types Q3Dns can handle. The DNS
    provides many more; these are the ones we've judged to be in
    current use, useful for GUI programs and important enough to
    support right away:

    \value None  No information. This exists only so that Q3Dns can
    have a default.

    \value A  IPv4 addresses. By far the most common type.

    \value Aaaa  IPv6 addresses. So far mostly unused.

    \value Mx  Mail eXchanger names. Used for mail delivery.

    \value Srv  SeRVer names. Generic record type for finding
    servers. So far mostly unused.

    \value Cname  Canonical names. Maps from nicknames to the true
    name (the canonical name) for a host.

    \value Ptr  name PoinTeRs. Maps from IPv4 or IPv6 addresses to hostnames.

    \value Txt  arbitrary TeXT for domains.

    We expect that some support for the
    \l{http://www.rfc-editor.org/rfc/rfc2535.txt}{RFC 2535}
    extensions will be added in future versions.
*/

/*!
    Sets this object to query for record type \a rr records.

    The DNS lookup is started the next time the application enters the
    event loop. When the result is found the signal resultsReady() is
    emitted.

    \sa RecordType
*/

void Q3Dns::setRecordType( RecordType rr )
{
    t = rr;
    d->noNames = false;
    setStartQueryTimer(); // start query the next time we enter event loop
}

/*!
  \internal

  Private slot for starting the query.
*/
void Q3Dns::startQuery()
{
    // isWorking() starts the query (if necessary)
    if ( !isWorking() )
	emit resultsReady();
}

/*!
    The three functions Q3Dns::Q3Dns(QString, RecordType),
    Q3Dns::setLabel() and Q3Dns::setRecordType() may start a DNS lookup.
    This function handles setting up the single shot timer.
*/
void Q3Dns::setStartQueryTimer()
{
#if defined(Q_DNS_SYNCHRONOUS)
    if ( !d->queryTimer && !d->noEventLoop )
#else
    if ( !d->queryTimer )
#endif
    {
	// start the query the next time we enter event loop
	d->queryTimer = new QTimer( this );
	connect( d->queryTimer, SIGNAL(timeout()),
		 this, SLOT(startQuery()) );
	d->queryTimer->start( 0, true );
    }
}

/*
    Transforms the host address \a address to the IN-ADDR.ARPA domain
    name. Returns something indeterminate if you're sloppy or
    naughty. This function has an IPv4-specific name, but works for
    IPv6 too.
*/
QString Q3Dns::toInAddrArpaDomain( const QHostAddress &address )
{
    QString s;
    if ( address.isNull() ) {
	// if the address isn't valid, neither of the other two make
	// cases make sense. better to just return.
    } else if ( address.isIp4Addr() ) {
	Q_UINT32 i = address.ip4Addr();
	s.sprintf( "%d.%d.%d.%d.IN-ADDR.ARPA",
		   i & 0xff, (i >> 8) & 0xff, (i>>16) & 0xff, (i>>24) & 0xff );
    } else {
	// RFC 3152. (1886 is deprecated, and clients no longer need to
	// support it, in practice).
	Q_IPV6ADDR i = address.toIPv6Address();
	s = QLatin1String("ip6.arpa");
	uint b = 0;
	while( b < 16 ) {
	    s = QString::number( i.c[b]%16, 16 ) + QLatin1String(".") +
		QString::number( i.c[b]/16, 16 ) + QLatin1String(".") + s;
	    b++;
	}
    }
    return s;
}


/*!
    \fn Q3Dns::RecordType Q3Dns::recordType() const

    Returns the record type of this DNS query object.

    \sa setRecordType() RecordType
*/

/*!
    \fn void Q3Dns::resultsReady()

    This signal is emitted when results are available for one of the
    qualifiedNames().
*/

/*!
    Returns true if Q3Dns is doing a lookup for this object (i.e. if it
    does not already have the necessary information); otherwise
    returns false.

    Q3Dns emits the resultsReady() signal when the status changes to false.
*/

bool Q3Dns::isWorking() const
{
#if defined(Q3DNS_DEBUG)
    qDebug( "Q3Dns::isWorking (%s, %d)", l.ascii(), t );
#endif
    if ( t == None )
	return false;

#if defined(Q_DNS_SYNCHRONOUS)
    if ( d->noEventLoop )
	return true;
#endif

    Q3PtrList<Q3DnsRR> * ll = Q3DnsDomain::cached( this );
    Q_LONG queries = n.count();
    while( ll->current() != 0 ) {
	if ( ll->current()->nxdomain ) {
	    queries--;
	} else {
	    delete ll;
	    return false;
	}
	ll->next();
    }
    delete ll;

    if ( queries <= 0 )
	return false;
    if ( d->noNames )
	return false;
    return true;
}


/*!
    Returns a list of the addresses for this name if this Q3Dns object
    has a recordType() of Q3Dns::A or Q3Dns::Aaaa and the answer
    is available; otherwise returns an empty list.

    As a special case, if label() is a valid numeric IP address, this
    function returns that address.

    Note that if you want to iterate over the list, you should iterate
    over a copy, e.g.
    \snippet doc/src/snippets/code/src_qt3support_network_q3dns.cpp 1

*/

Q3ValueList<QHostAddress> Q3Dns::addresses() const
{
#if defined(Q3DNS_DEBUG)
    qDebug( "Q3Dns::addresses (%s)", l.ascii() );
#endif
    Q3ValueList<QHostAddress> result;
    if ( t != A && t != Aaaa )
	return result;

    Q3PtrList<Q3DnsRR> * cached = Q3DnsDomain::cached( this );

    Q3DnsRR * rr;
    while( (rr=cached->current()) != 0 ) {
	if ( rr->current && !rr->nxdomain )
	    result.append( rr->address );
	cached->next();
    }
    delete cached;
    return result;
}


/*!
    \class Q3Dns::MailServer
    \brief The Q3Dns::MailServer class is  described in Q3Dns::mailServers().

    \internal
*/

/*!
    Returns a list of mail servers if the record type is \c Mx. The
    class Q3Dns::MailServer contains the following public variables:
    \list
    \i QString Q3Dns::MailServer::name
    \i Q_UINT16 Q3Dns::MailServer::priority
    \endlist

    Note that if you want to iterate over the list, you should iterate
    over a copy, e.g.
    \snippet doc/src/snippets/code/src_qt3support_network_q3dns.cpp 2

*/
Q3ValueList<Q3Dns::MailServer> Q3Dns::mailServers() const
{
#if defined(Q3DNS_DEBUG)
    qDebug( "Q3Dns::mailServers (%s)", l.ascii() );
#endif
    Q3ValueList<Q3Dns::MailServer> result;
    if ( t != Mx )
	return result;

    Q3PtrList<Q3DnsRR> * cached = Q3DnsDomain::cached( this );

    Q3DnsRR * rr;
    while( (rr=cached->current()) != 0 ) {
	if ( rr->current && !rr->nxdomain ) {
	    MailServer ms( rr->target, rr->priority );
	    result.append( ms );
	}
	cached->next();
    }
    delete cached;
    return result;
}


/*!
    \class Q3Dns::Server
    \brief The Q3Dns::Server class is described in Q3Dns::servers().

    \internal
*/

/*!
    Returns a list of servers if the record type is \c Srv. The class
    Q3Dns::Server contains the following public variables:
    \list
    \i QString Q3Dns::Server::name
    \i Q_UINT16 Q3Dns::Server::priority
    \i Q_UINT16 Q3Dns::Server::weight
    \i Q_UINT16 Q3Dns::Server::port
    \endlist

    Note that if you want to iterate over the list, you should iterate
    over a copy, e.g.
    \snippet doc/src/snippets/code/src_qt3support_network_q3dns.cpp 3
*/
Q3ValueList<Q3Dns::Server> Q3Dns::servers() const
{
#if defined(Q3DNS_DEBUG)
    qDebug( "Q3Dns::servers (%s)", l.ascii() );
#endif
    Q3ValueList<Q3Dns::Server> result;
    if ( t != Srv )
	return result;

    Q3PtrList<Q3DnsRR> * cached = Q3DnsDomain::cached( this );

    Q3DnsRR * rr;
    while( (rr=cached->current()) != 0 ) {
	if ( rr->current && !rr->nxdomain ) {
	    Server s( rr->target, rr->priority, rr->weight, rr->port );
	    result.append( s );
	}
	cached->next();
    }
    delete cached;
    return result;
}


/*!
    Returns a list of host names if the record type is \c Ptr.

    Note that if you want to iterate over the list, you should iterate
    over a copy, e.g.
    \snippet doc/src/snippets/code/src_qt3support_network_q3dns.cpp 4

*/
QStringList Q3Dns::hostNames() const
{
#if defined(Q3DNS_DEBUG)
    qDebug( "Q3Dns::hostNames (%s)", l.ascii() );
#endif
    QStringList result;
    if ( t != Ptr )
	return result;

    Q3PtrList<Q3DnsRR> * cached = Q3DnsDomain::cached( this );

    Q3DnsRR * rr;
    while( (rr=cached->current()) != 0 ) {
	if ( rr->current && !rr->nxdomain ) {
	    QString str( rr->target );
	    result.append( str );
	}
	cached->next();
    }
    delete cached;
    return result;
}


/*!
    Returns a list of texts if the record type is \c Txt.

    Note that if you want to iterate over the list, you should iterate
    over a copy, e.g.
    \snippet doc/src/snippets/code/src_qt3support_network_q3dns.cpp 5
*/
QStringList Q3Dns::texts() const
{
#if defined(Q3DNS_DEBUG)
    qDebug( "Q3Dns::texts (%s)", l.ascii() );
#endif
    QStringList result;
    if ( t != Txt )
	return result;

    Q3PtrList<Q3DnsRR> * cached = Q3DnsDomain::cached( this );

    Q3DnsRR * rr;
    while( (rr=cached->current()) != 0 ) {
	if ( rr->current && !rr->nxdomain ) {
	    QString str( rr->text );
	    result.append( str );
	}
	cached->next();
    }
    delete cached;
    return result;
}


/*!
    Returns the canonical name for this DNS node. (This works
    regardless of what recordType() is set to.)

    If the canonical name isn't known, this function returns a null
    string.

    The canonical name of a DNS node is its full name, or the full
    name of the target of its CNAME. For example, if l.trolltech.com
    is a CNAME to lillian.troll.no, and the search path for Q3Dns is
    "trolltech.com", then the canonical name for all of "lillian",
    "l", "lillian.troll.no." and "l.trolltech.com" is
    "lillian.troll.no.".
*/

QString Q3Dns::canonicalName() const
{
    // the cname should work regardless of the recordType(), so set the record
    // type temporarily to cname when you look at the cache
    Q3Dns *that = (Q3Dns*) this; // mutable function
    RecordType oldType = t;
    that->t = Cname;
    Q3PtrList<Q3DnsRR> * cached = Q3DnsDomain::cached( that );
    that->t = oldType;

    Q3DnsRR * rr;
    while( (rr=cached->current()) != 0 ) {
	if ( rr->current && !rr->nxdomain && rr->domain ) {
	    delete cached;
	    return rr->target;
	}
	cached->next();
    }
    delete cached;
    return QString();
}

#if defined(Q_DNS_SYNCHRONOUS)
/*! \reimp
*/
void Q3Dns::connectNotify( const char *signal )
{
    if ( d->noEventLoop && qstrcmp(signal,SIGNAL(resultsReady()) )==0 ) {
	doSynchronousLookup();
    }
}
#endif

#if defined(Q_OS_WIN32) || defined(Q_OS_CYGWIN)

#if defined(Q_DNS_SYNCHRONOUS)
void Q3Dns::doSynchronousLookup()
{
    // ### not implemented yet
}
#endif

// the following typedefs are needed for GetNetworkParams() API call
#ifndef IP_TYPES_INCLUDED
#define MAX_HOSTNAME_LEN    128
#define MAX_DOMAIN_NAME_LEN 128
#define MAX_SCOPE_ID_LEN    256
typedef struct {
    char String[4 * 4];
} IP_ADDRESS_STRING, *PIP_ADDRESS_STRING, IP_MASK_STRING, *PIP_MASK_STRING;
typedef struct _IP_ADDR_STRING {
    struct _IP_ADDR_STRING* Next;
    IP_ADDRESS_STRING IpAddress;
    IP_MASK_STRING IpMask;
    DWORD Context;
} IP_ADDR_STRING, *PIP_ADDR_STRING;
typedef struct {
    char HostName[MAX_HOSTNAME_LEN + 4] ;
    char DomainName[MAX_DOMAIN_NAME_LEN + 4];
    PIP_ADDR_STRING CurrentDnsServer;
    IP_ADDR_STRING DnsServerList;
    UINT NodeType;
    char ScopeId[MAX_SCOPE_ID_LEN + 4];
    UINT EnableRouting;
    UINT EnableProxy;
    UINT EnableDns;
} FIXED_INFO, *PFIXED_INFO;
#endif
typedef DWORD (WINAPI *GNP)( PFIXED_INFO, PULONG );

// ### FIXME: this code is duplicated in qfiledialog.cpp
static QString getWindowsRegString( HKEY key, const QString &subKey )
{
    QString s;
    QT_WA( {
	char buf[1024];
	DWORD bsz = sizeof(buf);
	int r = RegQueryValueEx( key, (TCHAR*)subKey.ucs2(), 0, 0, (LPBYTE)buf, &bsz );
	if ( r == ERROR_SUCCESS ) {
	    s = QString::fromUcs2( (unsigned short *)buf );
	} else if ( r == ERROR_MORE_DATA ) {
	    char *ptr = new char[bsz+1];
	    r = RegQueryValueEx( key, (TCHAR*)subKey.ucs2(), 0, 0, (LPBYTE)ptr, &bsz );
	    if ( r == ERROR_SUCCESS )
		s = QLatin1String(ptr);
	    delete [] ptr;
	}
    } , {
	char buf[512];
	DWORD bsz = sizeof(buf);
	int r = RegQueryValueExA( key, subKey.local8Bit(), 0, 0, (LPBYTE)buf, &bsz );
	if ( r == ERROR_SUCCESS ) {
	    s = QLatin1String(buf);
	} else if ( r == ERROR_MORE_DATA ) {
	    char *ptr = new char[bsz+1];
	    r = RegQueryValueExA( key, subKey.local8Bit(), 0, 0, (LPBYTE)ptr, &bsz );
	    if ( r == ERROR_SUCCESS )
		s = QLatin1String(ptr);
	    delete [] ptr;
	}
    } );
    return s;
}

static bool getDnsParamsFromRegistry( const QString &path,
	QString *domainName, QString *nameServer, QString *searchList )
{
    HKEY k;
    int r;
    QT_WA( {
	r = RegOpenKeyEx( HKEY_LOCAL_MACHINE,
			  (TCHAR*)path.ucs2(),
			  0, KEY_READ, &k );
    } , {
	r = RegOpenKeyExA( HKEY_LOCAL_MACHINE,
			   path.latin1(),
			   0, KEY_READ, &k );
    } );

    if ( r == ERROR_SUCCESS ) {
	*domainName = getWindowsRegString( k, QLatin1String("DhcpDomain") );
	if ( domainName->isEmpty() )
	    *domainName = getWindowsRegString( k, QLatin1String("Domain") );

	*nameServer = getWindowsRegString( k, QLatin1String("DhcpNameServer") );
	if ( nameServer->isEmpty() )
	    *nameServer = getWindowsRegString( k, QLatin1String("NameServer") );

	*searchList = getWindowsRegString( k, QLatin1String("SearchList") );
    }
    RegCloseKey( k );
    return r == ERROR_SUCCESS;
}

void Q3Dns::doResInit()
{
    char separator = 0;

    if ( theNs )
        delete theNs;
    theNs = new Q3PtrList<QHostAddress>;
    theNs->setAutoDelete( true );
    theDomains = new Q3StrList( true );
    theDomains->setAutoDelete( true );

    QString domainName, nameServer, searchList;

    bool gotNetworkParams = false;
    // try the API call GetNetworkParams() first and use registry lookup only
    // as a fallback
#ifdef Q_OS_WINCE
    HINSTANCE hinstLib = LoadLibraryW( L"iphlpapi" );
#else
    HINSTANCE hinstLib = LoadLibraryA( "iphlpapi" );
#endif
    if ( hinstLib != 0 ) {
#ifdef Q_OS_WINCE
	GNP getNetworkParams = (GNP) GetProcAddressW( hinstLib, L"GetNetworkParams" );
#else
	GNP getNetworkParams = (GNP) GetProcAddress( hinstLib, "GetNetworkParams" );
#endif
	if ( getNetworkParams != 0 ) {
	    ULONG l = 0;
	    DWORD res;
	    res = getNetworkParams( 0, &l );
	    if ( res == ERROR_BUFFER_OVERFLOW ) {
		FIXED_INFO *finfo = (FIXED_INFO*)new char[l];
		res = getNetworkParams( finfo, &l );
		if ( res == ERROR_SUCCESS ) {
		    domainName = QLatin1String(finfo->DomainName);
		    nameServer = QLatin1String("");
		    IP_ADDR_STRING *dnsServer = &finfo->DnsServerList;
		    while ( dnsServer != 0 ) {
			nameServer += QLatin1String(dnsServer->IpAddress.String);
			dnsServer = dnsServer->Next;
			if ( dnsServer != 0 )
			    nameServer += QLatin1String(" ");
		    }
		    searchList = QLatin1String("");
		    separator = ' ';
		    gotNetworkParams = true;
		}
		delete[] finfo;
	    }
	}
	FreeLibrary( hinstLib );
    }
    if ( !gotNetworkParams ) {
	if ( getDnsParamsFromRegistry(
	    QString( QLatin1String("System\\CurrentControlSet\\Services\\Tcpip\\Parameters") ),
		    &domainName, &nameServer, &searchList )) {
	    // for NT
	    separator = ' ';
	} else if ( getDnsParamsFromRegistry(
	    QString( QLatin1String("System\\CurrentControlSet\\Services\\VxD\\MSTCP") ),
		    &domainName, &nameServer, &searchList )) {
	    // for Windows 98
	    separator = ',';
	} else {
	    // Could not access the TCP/IP parameters
	    domainName = QLatin1String("");
	    nameServer = QLatin1String("127.0.0.1");
	    searchList = QLatin1String("");
	    separator = ' ';
	}
    }

    nameServer = nameServer.simplifyWhiteSpace();
    int first, last;
    if ( !nameServer.isEmpty() ) {
	first = 0;
	do {
	    last = nameServer.find( QLatin1Char(separator), first );
	    if ( last < 0 )
		last = nameServer.length();
	    Q3Dns tmp( nameServer.mid( first, last-first ), Q3Dns::A );
	    Q3ValueList<QHostAddress> address = tmp.addresses();
	    Q_LONG i = address.count();
	    while( i )
		theNs->append( new QHostAddress(address[--i]) );
	    first = last+1;
	} while( first < (int)nameServer.length() );
    }

    searchList = searchList + QLatin1String(" ") + domainName;
    searchList = searchList.simplifyWhiteSpace().lower();
    first = 0;
    do {
	last = searchList.find( QLatin1Char(separator), first );
	if ( last < 0 )
	    last = searchList.length();
	theDomains->append( qstrdup( searchList.mid( first, last-first ).latin1() ) );
	first = last+1;
    } while( first < (int)searchList.length() );
}

#elif defined(Q_OS_UNIX)

#if defined(Q_DNS_SYNCHRONOUS)
void Q3Dns::doSynchronousLookup()
{
    if ( t!=None && !l.isEmpty() ) {
	Q3ValueListIterator<QString> it = n.begin();
	Q3ValueListIterator<QString> end = n.end();
	int type;
	switch( t ) {
	    case Q3Dns::A:
		type = 1;
		break;
	    case Q3Dns::Aaaa:
		type = 28;
		break;
	    case Q3Dns::Mx:
		type = 15;
		break;
	    case Q3Dns::Srv:
		type = 33;
		break;
	    case Q3Dns::Cname:
		type = 5;
		break;
	    case Q3Dns::Ptr:
		type = 12;
		break;
	    case Q3Dns::Txt:
		type = 16;
		break;
	    default:
		type = (char)255; // any
		break;
	}
	while( it != end ) {
	    QString s = *it;
	    it++;
	    QByteArray ba( 512 );
	    int len = res_search( s.latin1(), 1, type, (uchar*)ba.data(), ba.size() );
	    if ( len > 0 ) {
		ba.resize( len );

		Q3DnsQuery * query = new Q3DnsQuery;
		query->started = now();
		query->id = ++theId;
		query->t = t;
		query->l = s;
		Q3DnsAnswer a( ba, query );
		a.parse();
	    } else if ( len == -1 ) {
		// res_search error
	    }
	}
	emit resultsReady();
    }
}
#endif

#if defined(__GLIBC__) && ((__GLIBC__ > 2) || ((__GLIBC__ == 2) && (__GLIBC_MINOR__ >= 3)))
#define Q_MODERN_RES_API
#endif

void Q3Dns::doResInit()
{
    if ( theNs )
	return;
    theNs = new Q3PtrList<QHostAddress>;
    theNs->setAutoDelete( true );
    theDomains = new Q3StrList( true );
    theDomains->setAutoDelete( true );

    // read resolv.conf manually.
    QFile resolvConf(QLatin1String("/etc/resolv.conf"));
    if (resolvConf.open(QIODevice::ReadOnly)) {
        QTextStream stream( &resolvConf );
	QString line;

	while ( !stream.atEnd() ) {
            line = stream.readLine();
	    QStringList list = QStringList::split( QLatin1String(" "), line );
	    if( line.startsWith( QLatin1String("#") ) || list.size() < 2 )
	       continue;
	    const QString type = list[0].lower();

	    if ( type == QLatin1String("nameserver") ) {
		QHostAddress *address = new QHostAddress();
		if ( address->setAddress( QString(list[1]) ) ) {
		    // only add ipv6 addresses from resolv.conf if
		    // this host supports ipv6.
		    if ( address->isIPv4Address() || ipv6support )
			theNs->append( address );
                    else
                        delete address;
		} else {
		    delete address;
		}
	    } else if ( type == QLatin1String("search") ) {
		QStringList srch = QStringList::split( QLatin1String(" "), list[1] );
		for ( QStringList::Iterator i = srch.begin(); i != srch.end(); ++i )
		    theDomains->append( (*i).lower().local8Bit() );

	    } else if ( type == QLatin1String("domain") ) {
		theDomains->append( list[1].lower().local8Bit() );
	    }
	}
    }

    if (theNs->isEmpty()) {
#if defined(Q_MODERN_RES_API)
	struct __res_state res;
	res_ninit( &res );
	int i;
	// find the name servers to use
	for( i=0; i < MAXNS && i < res.nscount; i++ )
	    theNs->append( new QHostAddress( ntohl( res.nsaddr_list[i].sin_addr.s_addr ) ) );
#  if defined(MAXDFLSRCH)
	for( i=0; i < MAXDFLSRCH; i++ ) {
	    if ( res.dnsrch[i] && *(res.dnsrch[i]) )
		theDomains->append( QString::fromLatin1( res.dnsrch[i] ).lower().local8Bit() );
	    else
		break;
	}
#  endif
	if ( *res.defdname )
	    theDomains->append( QString::fromLatin1( res.defdname ).lower().local8Bit() );
#else
	res_init();
	int i;
	// find the name servers to use
	for( i=0; i < MAXNS && i < _res.nscount; i++ )
	    theNs->append( new QHostAddress( ntohl( _res.nsaddr_list[i].sin_addr.s_addr ) ) );
#  if defined(MAXDFLSRCH)
	for( i=0; i < MAXDFLSRCH; i++ ) {
	    if ( _res.dnsrch[i] && *(_res.dnsrch[i]) )
		theDomains->append( QString::fromLatin1( _res.dnsrch[i] ).lower().local8Bit() );
	    else
		break;
	}
#  endif
	if ( *_res.defdname )
	    theDomains->append( QString::fromLatin1( _res.defdname ).lower().local8Bit() );
#endif

	// the code above adds "0.0.0.0" as a name server at the slightest
	// hint of trouble. so remove those again.
	theNs->first();
	while( theNs->current() ) {
	    if ( theNs->current()->isNull() )
		delete theNs->take();
	    else
		theNs->next();
	}
    }

    QFile hosts( QString::fromLatin1( "/etc/hosts" ) );
    if ( hosts.open( QIODevice::ReadOnly ) ) {
	// read the /etc/hosts file, creating long-life A and PTR RRs
	// for the things we find.
	QTextStream i( &hosts );
	QString line;
	while( !i.atEnd() ) {
	    line = i.readLine().simplifyWhiteSpace().lower();
	    uint n = 0;
	    while( (int) n < line.length() && line[(int)n] != QLatin1Char('#') )
		n++;
	    line.truncate( n );
	    n = 0;
	    while( (int) n < line.length() && !line[(int)n].isSpace() )
		n++;
	    QString ip = line.left( n );
	    QHostAddress a;
	    a.setAddress( ip );
	    if ( ( a.isIPv4Address() || a.isIPv6Address() ) && !a.isNull() ) {
		bool first = true;
		line = line.mid( n+1 );
		n = 0;
		while( (int) n < line.length() && !line[(int)n].isSpace() )
		    n++;
		QString hostname = line.left( n );
		// ### in case of bad syntax, hostname is invalid. do we care?
		if ( n ) {
		    Q3DnsRR * rr = new Q3DnsRR( hostname );
		    if ( a.isIPv4Address() )
			rr->t = Q3Dns::A;
		    else
			rr->t = Q3Dns::Aaaa;
		    rr->address = a;
		    rr->deleteTime = UINT_MAX;
		    rr->expireTime = UINT_MAX;
		    rr->current = true;
		    if ( first ) {
			first = false;
			Q3DnsRR * ptr = new Q3DnsRR( Q3Dns::toInAddrArpaDomain( a ) );
			ptr->t = Q3Dns::Ptr;
			ptr->target = hostname;
			ptr->deleteTime = UINT_MAX;
			ptr->expireTime = UINT_MAX;
			ptr->current = true;
		    }
		}
	    }
	}
    }
}

#endif

QT_END_NAMESPACE

#endif // QT_NO_DNS
