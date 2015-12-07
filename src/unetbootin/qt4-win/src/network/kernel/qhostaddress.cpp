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

#include "qdebug.h"
#include "qhostaddress.h"
#include "qplatformdefs.h"
#include "qstringlist.h"
#ifndef QT_NO_DATASTREAM
#include <qdatastream.h>
#endif
#if defined(Q_OS_WINCE)
#include <winsock.h>
#endif

#ifdef QT_LSB
#  include <arpa/inet.h>
#endif

QT_BEGIN_NAMESPACE

#define QT_ENSURE_PARSED(a) \
    do { \
        if (!(a)->d->isParsed) \
            (a)->d->parse(); \
    } while (0)

#ifdef Q_OS_WIN
#    if !defined (QT_NO_IPV6)
// sockaddr_in6 size changed between old and new SDK
// Only the new version is the correct one, so always
// use this structure.
#if defined(Q_OS_WINCE)
#  if !defined(u_char)
#    define u_char unsigned char
#  endif
#  if !defined(u_short)
#    define u_short unsigned short
#  endif
#  if !defined(u_long)
#    define u_long unsigned long
#  endif
#endif
struct qt_in6_addr {
    u_char qt_s6_addr[16];
};
typedef struct {
    short   sin6_family;            /* AF_INET6 */
    u_short sin6_port;              /* Transport level port number */
    u_long  sin6_flowinfo;          /* IPv6 flow information */
    struct  qt_in6_addr sin6_addr;  /* IPv6 address */
    u_long  sin6_scope_id;          /* set of interfaces for a scope */
} qt_sockaddr_in6;
#    else
typedef void * qt_sockaddr_in6 ;
#    endif
#    ifndef AF_INET6
#        define AF_INET6        23  /* Internetwork Version 6 */
#    endif
#else
#define qt_sockaddr_in6 sockaddr_in6
#define qt_s6_addr s6_addr
#endif


class QHostAddressPrivate
{
public:
    QHostAddressPrivate();

    void setAddress(quint32 a_ = 0);
    void setAddress(const quint8 *a_);
    void setAddress(const Q_IPV6ADDR &a_);

    bool parse();
    void clear();

private:
    quint32 a;    // IPv4 address
    Q_IPV6ADDR a6; // IPv6 address
    QAbstractSocket::NetworkLayerProtocol protocol;

    QString ipString;
    bool isParsed;
    QString scopeId;

    friend class QHostAddress;
};

QHostAddressPrivate::QHostAddressPrivate()
    : a(0), protocol(QAbstractSocket::UnknownNetworkLayerProtocol), isParsed(true)
{
    memset(&a6, 0, sizeof(a6));
}

void QHostAddressPrivate::setAddress(quint32 a_)
{
    a = a_;
    protocol = QAbstractSocket::IPv4Protocol;
    isParsed = true;
}

void QHostAddressPrivate::setAddress(const quint8 *a_)
{
    for (int i = 0; i < 16; i++)
        a6[i] = a_[i];
    protocol = QAbstractSocket::IPv6Protocol;
    isParsed = true;
}

void QHostAddressPrivate::setAddress(const Q_IPV6ADDR &a_)
{
    a6 = a_;
    a = 0;
    protocol = QAbstractSocket::IPv6Protocol;
    isParsed = true;
}

static bool parseIp4(const QString& address, quint32 *addr)
{
    QStringList ipv4 = address.split(QLatin1String("."));
    if (ipv4.count() != 4)
        return false;

    quint32 ipv4Address = 0;
    for (int i = 0; i < 4; ++i) {
        bool ok = false;
        uint byteValue = ipv4.at(i).toUInt(&ok);
        if (!ok || byteValue > 255)
            return false;

        ipv4Address <<= 8;
        ipv4Address += byteValue;
    }

    *addr = ipv4Address;
    return true;
}

static bool parseIp6(const QString &address, quint8 *addr, QString *scopeId)
{
    QString tmp = address;
    int scopeIdPos = tmp.lastIndexOf(QLatin1Char('%'));
    if (scopeIdPos != -1) {
        *scopeId = tmp.mid(scopeIdPos + 1);
        tmp.chop(tmp.size() - scopeIdPos);
    } else {
        scopeId->clear();
    }

    QStringList ipv6 = tmp.split(QLatin1String(":"));
    int count = ipv6.count();
    if (count < 3 || count > 8)
        return false;

    int mc = 16;
    int fillCount = 9 - count;  // number of 0 words to fill in the middle
    for (int i = count - 1; i >= 0; --i) {
        if (mc <= 0)
            return false;

        if (ipv6.at(i).isEmpty()) {
            if (i == count - 1) {
                // special case: ":" is last character
                if (!ipv6.at(i - 1).isEmpty())
                    return false;
                addr[--mc] = 0;
                addr[--mc] = 0;
            } else if (i == 0) {
                // special case: ":" is first character
                if (!ipv6.at(i + 1).isEmpty())
                    return false;
                addr[--mc] = 0;
                addr[--mc] = 0;
            } else {
                for (int j = 0; j < fillCount; ++j) {
                    if (mc <= 0)
                        return false;
                    addr[--mc] = 0;
                    addr[--mc] = 0;
                }
            }
        } else {
            bool ok = false;
            uint byteValue = ipv6.at(i).toUInt(&ok, 16);
            if (ok && byteValue <= 0xffff) {
                addr[--mc] = byteValue & 0xff;
                addr[--mc] = (byteValue >> 8) & 0xff;
            } else {
                if (i != count - 1)
                    return false;

                // parse the ipv4 part of a mixed type
                quint32 maybeIp4;
                if (!parseIp4(ipv6.at(i), &maybeIp4))
                    return false;

                addr[--mc] = maybeIp4 & 0xff;
                addr[--mc] = (maybeIp4 >> 8) & 0xff;
                addr[--mc] = (maybeIp4 >> 16) & 0xff;
                addr[--mc] = (maybeIp4 >> 24) & 0xff;
                --fillCount;
            }
        }
    }

    return true;
}

bool QHostAddressPrivate::parse()
{
    isParsed = true;
    protocol = QAbstractSocket::UnknownNetworkLayerProtocol;
    QString a = ipString.simplified();

    // All IPv6 addresses contain a ':', and may contain a '.'.
    if (a.contains(QLatin1Char(':'))) {
        quint8 maybeIp6[16];
        if (parseIp6(a, maybeIp6, &scopeId)) {
            setAddress(maybeIp6);
            protocol = QAbstractSocket::IPv6Protocol;
            return true;
        }
    }

    // All IPv4 addresses contain a '.'.
    if (a.contains(QLatin1Char('.'))) {
        quint32 maybeIp4 = 0;
        if (parseIp4(a, &maybeIp4)) {
            setAddress(maybeIp4);
            protocol = QAbstractSocket::IPv4Protocol;
            return true;
        }
    }

    return false;
}

void QHostAddressPrivate::clear()
{
    a = 0;
    protocol = QAbstractSocket::UnknownNetworkLayerProtocol;
    isParsed = true;
    memset(&a6, 0, sizeof(a6));
}

/*!
    \class QHostAddress
    \brief The QHostAddress class provides an IP address.
    \ingroup io
    \module network

    This class holds an IPv4 or IPv6 address in a platform- and
    protocol-independent manner.

    QHostAddress is normally used with the QTcpSocket, QTcpServer,
    and QUdpSocket to connect to a host or to set up a server.

    A host address is set with setAddress(), checked for its type
    using isIPv4Address() or isIPv6Address(), and retrieved with
    toIPv4Address(), toIPv6Address(), or toString().

    The class also supports common predefined addresses: \l Null, \l
    LocalHost, \l LocalHostIPv6, \l Broadcast, and \l Any.

    \sa QTcpSocket, QTcpServer, QUdpSocket
*/

/*! \enum QHostAddress::SpecialAddress

    \value Null The null address object. Equivalent to QHostAddress().
    \value LocalHost The IPv4 localhost address. Equivalent to QHostAddress("127.0.0.1").
    \value LocalHostIPv6 The IPv6 localhost address. Equivalent to QHostAddress("::1").
    \value Broadcast The IPv4 broadcast address. Equivalent to QHostAddress("255.255.255.255").
    \value Any The IPv4 any-address. Equivalent to QHostAddress("0.0.0.0").
    \value AnyIPv6 The IPv6 any-address. Equivalent to QHostAddress("::").
*/

/*!  Constructs a host address object with the IP address 0.0.0.0.

    \sa clear()
*/
QHostAddress::QHostAddress()
    : d(new QHostAddressPrivate)
{
}

/*!
    Constructs a host address object with the IPv4 address \a ip4Addr.
*/
QHostAddress::QHostAddress(quint32 ip4Addr)
    : d(new QHostAddressPrivate)
{
    setAddress(ip4Addr);
}

/*!
    Constructs a host address object with the IPv6 address \a ip6Addr.

    \a ip6Addr must be a 16-byte array in network byte order (big
    endian).
*/
QHostAddress::QHostAddress(quint8 *ip6Addr)
    : d(new QHostAddressPrivate)
{
    setAddress(ip6Addr);
}

/*!
    Constructs a host address object with the IPv6 address \a ip6Addr.
*/
QHostAddress::QHostAddress(const Q_IPV6ADDR &ip6Addr)
    : d(new QHostAddressPrivate)
{
    setAddress(ip6Addr);
}

/*!
    Constructs an IPv4 or IPv6 address based on the string \a address
    (e.g., "127.0.0.1").

    \sa setAddress()
*/
QHostAddress::QHostAddress(const QString &address)
    : d(new QHostAddressPrivate)
{
    d->ipString = address;
    d->isParsed = false;
}

/*!
    \fn QHostAddress::QHostAddress(const sockaddr *sockaddr)

    Constructs an IPv4 or IPv6 address using the address specified by
    the native structure \a sockaddr.

    \sa setAddress()
*/
QHostAddress::QHostAddress(const struct sockaddr *sockaddr)
    : d(new QHostAddressPrivate)
{
    if (sockaddr->sa_family == AF_INET)
        setAddress(htonl(((sockaddr_in *)sockaddr)->sin_addr.s_addr));
#ifndef QT_NO_IPV6
    else if (sockaddr->sa_family == AF_INET6)
        setAddress(((qt_sockaddr_in6 *)sockaddr)->sin6_addr.qt_s6_addr);
#endif
}

/*!
    Constructs a copy of the given \a address.
*/
QHostAddress::QHostAddress(const QHostAddress &address)
    : d(new QHostAddressPrivate(*address.d))
{
}

/*!
    Constructs a QHostAddress object for \a address.
*/
QHostAddress::QHostAddress(SpecialAddress address)
    : d(new QHostAddressPrivate)
{
    switch (address) {
    case Null:
        break;
    case Broadcast:
        setAddress(QLatin1String("255.255.255.255"));
        break;
    case LocalHost:
        setAddress(QLatin1String("127.0.0.1"));
        break;
    case LocalHostIPv6:
        setAddress(QLatin1String("::1"));
        break;
    case Any:
        setAddress(QLatin1String("0.0.0.0"));
        break;
    case AnyIPv6:
        setAddress(QLatin1String("::"));
        break;
    }
}

/*!
    Destroys the host address object.
*/
QHostAddress::~QHostAddress()
{
    delete d;
}

/*!
    Assigns another host \a address to this object, and returns a reference
    to this object.
*/
QHostAddress &QHostAddress::operator=(const QHostAddress &address)
{
    *d = *address.d;
    return *this;
}

/*!
    Assigns the host address \a address to this object, and returns a
    reference to this object.

    \sa setAddress()
*/
QHostAddress &QHostAddress::operator=(const QString &address)
{
    setAddress(address);
    return *this;
}

/*!
    \fn bool QHostAddress::operator!=(const QHostAddress &other) const
    \since 4.2

    Returns true if this host address is not the same as the \a other
    address given; otherwise returns false.
*/

/*!
    \fn bool QHostAddress::operator!=(SpecialAddress other) const

    Returns true if this host address is not the same as the \a other
    address given; otherwise returns false.
*/

/*!
    Sets the host address to 0.0.0.0.
*/
void QHostAddress::clear()
{
    d->clear();
}

/*!
    Set the IPv4 address specified by \a ip4Addr.
*/
void QHostAddress::setAddress(quint32 ip4Addr)
{
    d->setAddress(ip4Addr);
}

/*!
    \overload

    Set the IPv6 address specified by \a ip6Addr.

    \a ip6Addr must be an array of 16 bytes in network byte order
    (high-order byte first).
*/
void QHostAddress::setAddress(quint8 *ip6Addr)
{
    d->setAddress(ip6Addr);
}

/*!
    \overload

    Set the IPv6 address specified by \a ip6Addr.
*/
void QHostAddress::setAddress(const Q_IPV6ADDR &ip6Addr)
{
    d->setAddress(ip6Addr);
}

/*!
    \overload

    Sets the IPv4 or IPv6 address specified by the string
    representation specified by \a address (e.g. "127.0.0.1").
    Returns true and sets the address if the address was successfully
    parsed; otherwise returns false.
*/
bool QHostAddress::setAddress(const QString &address)
{
    d->ipString = address;
    return d->parse();
}

/*!
    \fn void QHostAddress::setAddress(const sockaddr *sockaddr)
    \overload

    Sets the IPv4 or IPv6 address specified by the native structure \a
    sockaddr.  Returns true and sets the address if the address was
    successfully parsed; otherwise returns false.
*/
void QHostAddress::setAddress(const struct sockaddr *sockaddr)
{
    clear();
    if (sockaddr->sa_family == AF_INET)
        setAddress(htonl(((sockaddr_in *)sockaddr)->sin_addr.s_addr));
#ifndef QT_NO_IPV6
    else if (sockaddr->sa_family == AF_INET6)
        setAddress(((qt_sockaddr_in6 *)sockaddr)->sin6_addr.qt_s6_addr);
#endif
}

/*!
    Returns the IPv4 address as a number.

    For example, if the address is 127.0.0.1, the returned value is
    2130706433 (i.e. 0x7f000001).

    This value is only valid if isIp4Addr() returns true.

    \sa toString()
*/
quint32 QHostAddress::toIPv4Address() const
{
    QT_ENSURE_PARSED(this);
    return d->a;
}

/*!
    Returns the network layer protocol of the host address.
*/
QAbstractSocket::NetworkLayerProtocol QHostAddress::protocol() const
{
    QT_ENSURE_PARSED(this);
    return d->protocol;
}

/*!
    Returns the IPv6 address as a Q_IPV6ADDR structure. The structure
    consists of 16 unsigned characters.

    \snippet doc/src/snippets/code/src.network.kernel.qhostaddress.cpp 0

    This value is only valid if isIPv6Address() returns true.

    \sa toString()
*/
Q_IPV6ADDR QHostAddress::toIPv6Address() const
{
    QT_ENSURE_PARSED(this);
    return d->a6;
}

/*!
    Returns the address as a string.

    For example, if the address is the IPv4 address 127.0.0.1, the
    returned string is "127.0.0.1".

    \sa toIPv4Address()
*/
QString QHostAddress::toString() const
{
    QT_ENSURE_PARSED(this);
    if (d->protocol == QAbstractSocket::IPv4Protocol) {
        quint32 i = toIPv4Address();
        QString s;
        s.sprintf("%d.%d.%d.%d", (i>>24) & 0xff, (i>>16) & 0xff,
                (i >> 8) & 0xff, i & 0xff);
        return s;
    }

    if (d->protocol == QAbstractSocket::IPv6Protocol) {
        quint16 ugle[8];
        for (int i = 0; i < 8; i++) {
            ugle[i] = (quint16(d->a6[2*i]) << 8) | quint16(d->a6[2*i+1]);
        }
        QString s;
        s.sprintf("%X:%X:%X:%X:%X:%X:%X:%X",
                  ugle[0], ugle[1], ugle[2], ugle[3], ugle[4], ugle[5], ugle[6], ugle[7]);
        if (!d->scopeId.isEmpty())
            s.append(QLatin1Char('%') + d->scopeId);
        return s;
    }

    return QString();
}

/*!
    \since 4.1

    Returns the scope ID of an IPv6 address. For IPv4 addresses, or if the
    address does not contain a scope ID, an empty QString is returned.

    The IPv6 scope ID specifies the scope of \e reachability for non-global
    IPv6 addresses, limiting the area in which the address can be used. All
    IPv6 addresses are associated with such a reachability scope. The scope ID
    is used to disambiguate addresses that are not guaranteed to be globally
    unique.

    IPv6 specifies the following four levels of reachability:

    \list

    \o Node-local: Addresses that are only used for communicating with
    services on the same interface (e.g., the loopback interface "::1").

    \o Link-local: Addresses that are local to the network interface
    (\e{link}). There is always one link-local address for each IPv6 interface
    on your host. Link-local addresses ("fe80...") are generated from the MAC
    address of the local network adaptor, and are not guaranteed to be unique.

    \o Site-local: Addresses that are local to the site / private network
    (e.g., the company intranet). Site-local addresses ("fec0...")  are
    usually distributed by the site router, and are not guaranteed to be
    unique outside of the local site.

    \o Global: For globally routable addresses, such as public servers on the
    Internet.

    \endlist

    When using a link-local or site-local address for IPv6 connections, you
    must specify the scope ID. The scope ID for a link-local address is
    usually the same as the interface name (e.g., "eth0", "en1") or number
    (e.g., "1", "2").

    \sa setScopeId()
*/
QString QHostAddress::scopeId() const
{
    QT_ENSURE_PARSED(this);
    return (d->protocol == QAbstractSocket::IPv6Protocol) ? d->scopeId : QString();
}

/*!
    \since 4.1

    Sets the IPv6 scope ID of the address to \a id. If the address
    protocol is not IPv6, this function does nothing.
*/
void QHostAddress::setScopeId(const QString &id)
{
    QT_ENSURE_PARSED(this);
    if (d->protocol == QAbstractSocket::IPv6Protocol)
        d->scopeId = id;
}

/*!
    Returns true if this host address is the same as the \a other address
    given; otherwise returns false.
*/
bool QHostAddress::operator==(const QHostAddress &other) const
{
    QT_ENSURE_PARSED(this);
    QT_ENSURE_PARSED(&other);

    if (d->protocol == QAbstractSocket::IPv4Protocol)
        return other.d->protocol == QAbstractSocket::IPv4Protocol && d->a == other.d->a;
    if (d->protocol == QAbstractSocket::IPv6Protocol) {
        return other.d->protocol == QAbstractSocket::IPv6Protocol
               && memcmp(&d->a6, &other.d->a6, sizeof(Q_IPV6ADDR)) == 0;
    }
    return d->protocol == other.d->protocol;
}

/*!
    Returns true if this host address is the same as the \a other
    address given; otherwise returns false.
*/
bool QHostAddress::operator ==(SpecialAddress other) const
{
    QT_ENSURE_PARSED(this);
    QHostAddress otherAddress(other);
    QT_ENSURE_PARSED(&otherAddress);

    if (d->protocol == QAbstractSocket::IPv4Protocol)
        return otherAddress.d->protocol == QAbstractSocket::IPv4Protocol && d->a == otherAddress.d->a;
    if (d->protocol == QAbstractSocket::IPv6Protocol) {
        return otherAddress.d->protocol == QAbstractSocket::IPv6Protocol
               && memcmp(&d->a6, &otherAddress.d->a6, sizeof(Q_IPV6ADDR)) == 0;
    }
    return int(other) == int(Null);
}

/*!
    Returns true if this host address is null (INADDR_ANY or in6addr_any).
    The default constructor creates a null address, and that address is
    not valid for any host or interface.
*/
bool QHostAddress::isNull() const
{
    QT_ENSURE_PARSED(this);
    return d->protocol == QAbstractSocket::UnknownNetworkLayerProtocol;
}

/*!
    \fn quint32 QHostAddress::ip4Addr() const

    Use toIPv4Address() instead.
*/

/*!
    \fn bool QHostAddress::isIp4Addr() const

    Use protocol() instead.
*/

/*!
    \fn bool QHostAddress::isIPv4Address() const

    Use protocol() instead.
*/

/*!
    \fn bool QHostAddress::isIPv6Address() const

    Use protocol() instead.
*/

#ifndef QT_NO_DEBUG_STREAM
QDebug operator<<(QDebug d, const QHostAddress &address)
{
    d.maybeSpace() << "QHostAddress(" << address.toString() << ")";
    return d.space();
}
#endif

uint qHash(const QHostAddress &key)
{
    return qHash(key.toString());
}

#ifndef QT_NO_DATASTREAM

/*! \relates QHostAddress

    Writes host address \a address to the stream \a out and returns a reference
    to the stream.

    \sa {Format of the QDataStream operators}
*/
QDataStream &operator<<(QDataStream &out, const QHostAddress &address)
{
    qint8 prot;
    prot = qint8(address.protocol());
    out << prot;
    switch (address.protocol()) {
    case QAbstractSocket::UnknownNetworkLayerProtocol:
        break;
    case QAbstractSocket::IPv4Protocol:
        out << address.toIPv4Address();
        break;
    case QAbstractSocket::IPv6Protocol:
    {
        Q_IPV6ADDR ipv6 = address.toIPv6Address();
        for (int i = 0; i < 16; ++i)
            out << ipv6[i];
        out << address.scopeId();
    }
        break;
    }
    return out;
}

/*! \relates QHostAddress

    Reads a host address into \a address from the stream \a in and returns a
    reference to the stream.

    \sa {Format of the QDataStream operators}
*/
QDataStream &operator>>(QDataStream &in, QHostAddress &address)
{
    qint8 prot;
    in >> prot;
    switch (QAbstractSocket::NetworkLayerProtocol(prot)) {
    case QAbstractSocket::UnknownNetworkLayerProtocol:
        address.clear();
        break;
    case QAbstractSocket::IPv4Protocol:
    {
        quint32 ipv4;
        in >> ipv4;
        address.setAddress(ipv4);
    }
        break;
    case QAbstractSocket::IPv6Protocol:
    {
        Q_IPV6ADDR ipv6;
        for (int i = 0; i < 16; ++i)
            in >> ipv6[i];
        address.setAddress(ipv6);

        QString scope;
        in >> scope;
        address.setScopeId(scope);
    }
        break;
    default:
        address.clear();
        in.setStatus(QDataStream::ReadCorruptData);
    }
    return in;
}

#endif //QT_NO_DATASTREAM

QT_END_NAMESPACE
