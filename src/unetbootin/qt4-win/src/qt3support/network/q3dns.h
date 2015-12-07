/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the Qt3Support module of the Qt Toolkit.
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

#ifndef Q3DNS_H
#define Q3DNS_H

#include <QtCore/qobject.h>
#include <QtNetwork/qhostaddress.h>
#include <QtCore/qsocketnotifier.h>
#include <QtCore/qstringlist.h>
#include <Qt3Support/q3valuelist.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Qt3Support)

#ifndef QT_NO_DNS

//#define Q_DNS_SYNCHRONOUS

class Q3DnsPrivate;

class Q_COMPAT_EXPORT Q3Dns: public QObject {
    Q_OBJECT
public:
    enum RecordType {
	None,
	A, Aaaa,
	Mx, Srv,
	Cname,
	Ptr,
	Txt
    };

    Q3Dns();
    Q3Dns( const QString & label, RecordType rr = A );
    Q3Dns( const QHostAddress & address, RecordType rr = Ptr );
    virtual ~Q3Dns();

    // to set/change the query
    virtual void setLabel( const QString & label );
    virtual void setLabel( const QHostAddress & address );
    QString label() const { return l; }

    virtual void setRecordType( RecordType rr = A );
    RecordType recordType() const { return t; }

    // whether something is happening behind the scenes
    bool isWorking() const;

    // to query for replies
    Q3ValueList<QHostAddress> addresses() const;

    class Q_COMPAT_EXPORT MailServer {
    public:
	MailServer( const QString & n=QString(), Q_UINT16 p=0 )
	    :name(n), priority(p) {}
	QString name;
	Q_UINT16 priority;
	Q_DUMMY_COMPARISON_OPERATOR(MailServer)
    };
    Q3ValueList<MailServer> mailServers() const;

    class Q_COMPAT_EXPORT Server {
    public:
	Server(const QString & n=QString(), Q_UINT16 p=0, Q_UINT16 w=0, Q_UINT16 po=0 )
	    : name(n), priority(p), weight(w), port(po) {}
	QString name;
	Q_UINT16 priority;
	Q_UINT16 weight;
	Q_UINT16 port;
	Q_DUMMY_COMPARISON_OPERATOR(Server)
    };
    Q3ValueList<Server> servers() const;

    QStringList hostNames() const;

    QStringList texts() const;

    QString canonicalName() const; // ### real-world but uncommon: QStringList

    QStringList qualifiedNames() const { return n; }

#if defined(Q_DNS_SYNCHRONOUS)
protected:
    void connectNotify( const char *signal );
#endif

Q_SIGNALS:
    void resultsReady();

private Q_SLOTS:
    void startQuery();

private:
    static void doResInit();
    void setStartQueryTimer();
    static QString toInAddrArpaDomain( const QHostAddress &address );
#if defined(Q_DNS_SYNCHRONOUS)
    void doSynchronousLookup();
#endif

    QString l;
    QStringList n;
    RecordType t;
    Q3DnsPrivate * d;

    friend class Q3DnsAnswer;
    friend class Q3DnsManager;
};


// Q3DnsSocket are sockets that are used for DNS lookup

class Q3DnsSocket: public QObject {
    Q_OBJECT
    // note: Private not public.  This class contains NO public API.
protected:
    Q3DnsSocket( QObject *, const char * );
    virtual ~Q3DnsSocket();

private Q_SLOTS:
    virtual void cleanCache();
    virtual void retransmit();
    virtual void answer();
};

#endif // QT_NO_DNS

QT_END_NAMESPACE

QT_END_HEADER

#endif // Q3DNS_H
