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
** This file is provided "AS IS" with NO WARRANTY OF ANY KIND,
** INCLUDING THE WARRANTIES OF DESIGN, MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE. Trolltech reserves all rights not expressly
** granted herein.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

#if defined Q_CC_MSVC && _MSC_VER <=1300
//VC.net 2002 support for templates doesn't match some PSDK requirements
#define _WSPIAPI_COUNTOF(_Array) (sizeof(_Array) / sizeof(_Array[0]))
#endif

#include <winsock2.h>

#include "qhostinfo_p.h"
#include "private/qnativesocketengine_p.h"
#include <ws2tcpip.h>
#include <qlibrary.h>
#include <qtimer.h>
#include <qmutex.h>
#include <private/qmutexpool_p.h>

QT_BEGIN_NAMESPACE

//#define QHOSTINFO_DEBUG

// Older SDKs do not include the addrinfo struct declaration, so we
// include a copy of it here.
struct qt_addrinfo
{
    int ai_flags;
    int ai_family;
    int ai_socktype;
    int ai_protocol;
    size_t ai_addrlen;
    char *ai_canonname;
    sockaddr *ai_addr;
    qt_addrinfo *ai_next;
};

// sockaddr_in6 size changed between old and new SDK
// Only the new version is the correct one, so always
// use this structure.
struct qt_in6_addr {
    uchar qt_s6_addr[16];
};

struct qt_sockaddr_in6 {
    short   sin6_family;            /* AF_INET6 */
    u_short sin6_port;              /* Transport level port number */
    u_long  sin6_flowinfo;          /* IPv6 flow information */
    struct  qt_in6_addr sin6_addr;  /* IPv6 address */
    u_long  sin6_scope_id;          /* set of interfaces for a scope */
};

//###
#define QT_SOCKLEN_T int
#ifndef NI_MAXHOST // already defined to 1025 in ws2tcpip.h?
#define NI_MAXHOST 1024
#endif

typedef int (__stdcall *getnameinfoProto)(const sockaddr *, QT_SOCKLEN_T, const char *, DWORD, const char *, DWORD, int);
typedef int (__stdcall *getaddrinfoProto)(const char *, const char *, const qt_addrinfo *, qt_addrinfo **);
typedef int (__stdcall *freeaddrinfoProto)(qt_addrinfo *);
static getnameinfoProto local_getnameinfo = 0;
static getaddrinfoProto local_getaddrinfo = 0;
static freeaddrinfoProto local_freeaddrinfo = 0;

static void resolveLibrary()
{
    // Attempt to resolve getaddrinfo(); without it we'll have to fall
    // back to gethostbyname(), which has no IPv6 support.
#if !defined(Q_OS_WINCE)
    local_getaddrinfo = (getaddrinfoProto) QLibrary::resolve(QLatin1String("ws2_32.dll"), "getaddrinfo");
    local_freeaddrinfo = (freeaddrinfoProto) QLibrary::resolve(QLatin1String("ws2_32.dll"), "freeaddrinfo");
    local_getnameinfo = (getnameinfoProto) QLibrary::resolve(QLatin1String("ws2_32.dll"), "getnameinfo");
#else
    local_getaddrinfo = (getaddrinfoProto) QLibrary::resolve(QLatin1String("ws2.dll"), "getaddrinfo");
    local_freeaddrinfo = (freeaddrinfoProto) QLibrary::resolve(QLatin1String("ws2.dll"), "freeaddrinfo");
    local_getnameinfo = (getnameinfoProto) QLibrary::resolve(QLatin1String("ws2.dll"), "getnameinfo");
#endif
}

#if defined(Q_OS_WINCE)
#include <qmutex.h>
QMutex qPrivCEMutex;
#endif
/*
    Performs a blocking call to gethostbyname or getaddrinfo, stores
    the results in a QHostInfo structure and emits the
    resultsReady() signal.
*/
QHostInfo QHostInfoAgent::fromName(const QString &hostName)
{
#if defined(Q_OS_WINCE)
    QMutexLocker locker(&qPrivCEMutex);
#endif
    QWindowsSockInit winSock;

    // Load res_init on demand.
    static volatile bool triedResolve = false;
    if (!triedResolve) {
#ifndef QT_NO_THREAD
        QMutexLocker locker(QMutexPool::globalInstanceGet(&local_getaddrinfo));
#endif
        if (!triedResolve) {
            resolveLibrary();
            triedResolve = true;
        }
    }

    QHostInfo results;
    results.setHostName(hostName);

#if defined(QHOSTINFO_DEBUG)
    qDebug("QHostInfoAgent::fromName(%p): looking up \"%s\" (IPv6 support is %s)",
           this, hostName.toLatin1().constData(),
           (local_getaddrinfo && local_freeaddrinfo) ? "enabled" : "disabled");
#endif

    QHostAddress address;
    if (address.setAddress(hostName)) {
        // Reverse lookup
        if (local_getnameinfo) {
            sockaddr_in sa4;
            qt_sockaddr_in6 sa6;
            sockaddr *sa;
            QT_SOCKLEN_T saSize;
            if (address.protocol() == QAbstractSocket::IPv4Protocol) {
                sa = (sockaddr *)&sa4;
                saSize = sizeof(sa4);
                memset(&sa4, 0, sizeof(sa4));
                sa4.sin_family = AF_INET;
                sa4.sin_addr.s_addr = htonl(address.toIPv4Address());
            } else {
                sa = (sockaddr *)&sa6;
                saSize = sizeof(sa6);
                memset(&sa6, 0, sizeof(sa6));
                sa6.sin6_family = AF_INET6;
                memcpy(sa6.sin6_addr.qt_s6_addr, address.toIPv6Address().c, sizeof(sa6.sin6_addr.qt_s6_addr));
            }

            char hbuf[NI_MAXHOST];
            if (local_getnameinfo(sa, saSize, hbuf, sizeof(hbuf), 0, 0, 0) != 0) {
                results.setError(QHostInfo::HostNotFound);
                results.setErrorString(tr("Host not found"));
                return results;
            }
            results.setHostName(QString::fromLatin1(hbuf));
        } else {
            unsigned long addr = inet_addr(hostName.toLatin1().constData());
            struct hostent *ent = gethostbyaddr((const char*)&addr, sizeof(addr), AF_INET);
            if (!ent) {
                results.setError(QHostInfo::HostNotFound);
                results.setErrorString(tr("Host not found"));
                return results;
            }
            results.setHostName(QString::fromLatin1(ent->h_name));
        }
    }

    if (local_getaddrinfo && local_freeaddrinfo) {
        // Call getaddrinfo, and place all IPv4 addresses at the start
        // and the IPv6 addresses at the end of the address list in
        // results.
        qt_addrinfo *res;
        int err = local_getaddrinfo(hostName.toLatin1().constData(), 0, 0, &res);
        if (err == 0) {
            QList<QHostAddress> addresses;
            for (qt_addrinfo *p = res; p != 0; p = p->ai_next) {
                switch (p->ai_family) {
                case AF_INET: {
                    QHostAddress addr;
                    addr.setAddress(ntohl(((sockaddr_in *) p->ai_addr)->sin_addr.s_addr));
                    if (!addresses.contains(addr))
                        addresses.append(addr);
                }
                    break;
                case AF_INET6: {
                    QHostAddress addr;
                    addr.setAddress(((qt_sockaddr_in6 *) p->ai_addr)->sin6_addr.qt_s6_addr);
                    if (!addresses.contains(addr))
                        addresses.append(addr);
                }
                    break;
                default:
                    results.setError(QHostInfo::UnknownError);
                    results.setErrorString(tr("Unknown address type"));
                }
            }
            results.setAddresses(addresses);
            local_freeaddrinfo(res);
        } else if (WSAGetLastError() == WSAHOST_NOT_FOUND || WSAGetLastError() == WSANO_DATA) {
            results.setError(QHostInfo::HostNotFound);
            results.setErrorString(tr("Host not found"));
        } else {
            results.setError(QHostInfo::UnknownError);
            results.setErrorString(tr("Unknown error"));
        }
    } else {
        // Fall back to gethostbyname, which only supports IPv4.
        hostent *ent = gethostbyname(hostName.toLatin1().constData());
        if (ent) {
            char **p;
            QList<QHostAddress> addresses;
            switch (ent->h_addrtype) {
            case AF_INET:
                for (p = ent->h_addr_list; *p != 0; p++) {
                    long *ip4Addr = (long *) *p;
                    QHostAddress temp;
                    temp.setAddress(ntohl(*ip4Addr));
                    addresses << temp;
                }
                break;
            default:
                results.setError(QHostInfo::UnknownError);
                results.setErrorString(tr("Unknown address type"));
                break;
            }
            results.setAddresses(addresses);
        } else if (WSAGetLastError() == 11001) {
            results.setErrorString(tr("Host not found"));
            results.setError(QHostInfo::HostNotFound);
        } else {
            results.setErrorString(tr("Unknown error"));
            results.setError(QHostInfo::UnknownError);
        }
    }

#if defined(QHOSTINFO_DEBUG)
    if (results.error() != QHostInfo::NoError) {
        qDebug("QHostInfoAgent::run(%p): error (%s)",
               this, results.errorString().toLatin1().constData());
    } else {
        QString tmp;
        QList<QHostAddress> addresses = results.addresses();
        for (int i = 0; i < addresses.count(); ++i) {
            if (i != 0) tmp += ", ";
            tmp += addresses.at(i).toString();
        }
        qDebug("QHostInfoAgent::run(%p): found %i entries: {%s}",
               this, addresses.count(), tmp.toLatin1().constData());
    }
#endif
    return results;
}

QString QHostInfo::localHostName()
{
    QWindowsSockInit winSock;

    char hostName[512];
    if (gethostname(hostName, sizeof(hostName)) == -1)
        return QString();
    hostName[sizeof(hostName) - 1] = '\0';
    return QString::fromLocal8Bit(hostName);
}


QT_END_NAMESPACE
