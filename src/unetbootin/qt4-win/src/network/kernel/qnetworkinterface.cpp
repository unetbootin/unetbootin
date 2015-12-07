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

#include "qnetworkinterface.h"
#include "qnetworkinterface_p.h"

#include "qdebug.h"

QT_BEGIN_NAMESPACE

static QList<QNetworkInterfacePrivate *> postProcess(QList<QNetworkInterfacePrivate *> list)
{
    // Some platforms report a netmask but don't report a broadcast address
    // Go through all available addresses and calculate the broadcast address
    // from the IP and the netmask
    //
    // This is an IPv4-only thing -- IPv6 has no concept of broadcasts
    // The math is:
    //    broadcast = IP | ~netmask

    QList<QNetworkInterfacePrivate *>::Iterator it = list.begin();
    const QList<QNetworkInterfacePrivate *>::Iterator end = list.end();
    for ( ; it != end; ++it) {
        QList<QNetworkAddressEntry>::Iterator addr_it = (*it)->addressEntries.begin();
        const QList<QNetworkAddressEntry>::Iterator addr_end = (*it)->addressEntries.end();
        for ( ; addr_it != addr_end; ++addr_it) {
            if (addr_it->ip().protocol() != QAbstractSocket::IPv4Protocol)
                continue;

            if (!addr_it->netmask().isNull() && addr_it->broadcast().isNull()) {
                QHostAddress bcast = addr_it->ip();
                bcast = QHostAddress(bcast.toIPv4Address() | ~addr_it->netmask().toIPv4Address());
                addr_it->setBroadcast(bcast);
            }
        }
    }

    return list;
}

Q_GLOBAL_STATIC(QNetworkInterfaceManager, manager)

QNetworkInterfaceManager::QNetworkInterfaceManager()
{
}

QNetworkInterfaceManager::~QNetworkInterfaceManager()
{
}

QSharedDataPointer<QNetworkInterfacePrivate> QNetworkInterfaceManager::interfaceFromName(const QString &name)
{
    QList<QSharedDataPointer<QNetworkInterfacePrivate> > interfaceList = allInterfaces();
    QList<QSharedDataPointer<QNetworkInterfacePrivate> >::ConstIterator it = interfaceList.constBegin();
    for ( ; it != interfaceList.constEnd(); ++it)
        if ((*it)->name == name)
            return *it;

    return empty;
}

QSharedDataPointer<QNetworkInterfacePrivate> QNetworkInterfaceManager::interfaceFromIndex(int index)
{
    QList<QSharedDataPointer<QNetworkInterfacePrivate> > interfaceList = allInterfaces();
    QList<QSharedDataPointer<QNetworkInterfacePrivate> >::ConstIterator it = interfaceList.constBegin();
    for ( ; it != interfaceList.constEnd(); ++it)
        if ((*it)->index == index)
            return *it;

    return empty;
}

QList<QSharedDataPointer<QNetworkInterfacePrivate> > QNetworkInterfaceManager::allInterfaces()
{
    QList<QNetworkInterfacePrivate *> list = postProcess(scan());
    QList<QSharedDataPointer<QNetworkInterfacePrivate> > result;

    foreach (QNetworkInterfacePrivate *ptr, list)
        result << QSharedDataPointer<QNetworkInterfacePrivate>(ptr);

    return result;
}

QString QNetworkInterfacePrivate::makeHwAddress(int len, uchar *data)
{
    QString result;
    for (int i = 0; i < len; ++i) {
        if (i)
            result += QLatin1Char(':');
        
        char buf[3];
#if defined(Q_OS_WIN) && !defined(Q_OS_WINCE) && defined(_MSC_VER) && _MSC_VER >= 1400
        sprintf_s(buf, 3, "%02hX", ushort(data[i]));
#else
        sprintf(buf, "%02hX", ushort(data[i]));
#endif
        result += QLatin1String(buf);
    }
    return result;
}

/*!
    \class QNetworkAddressEntry
    \brief The QNetworkAddressEntry class stores one IP address
    supported by a network interface, along with its associated
    netmask and broadcast address.

    \since 4.2
    \reentrant
    \ingroup io

    Each network interface can contain zero or more IP addresses, which
    in turn can be associated with a netmask and/or a broadcast
    address (depending on support from the operating system).

    This class represents one such group.
*/

/*!
    Constructs an empty QNetworkAddressEntry object.
*/
QNetworkAddressEntry::QNetworkAddressEntry()
    : d(new QNetworkAddressEntryPrivate)
{
}

/*!
    Constructs a QNetworkAddressEntry object that is a copy of the
    object \a other.
*/
QNetworkAddressEntry::QNetworkAddressEntry(const QNetworkAddressEntry &other)
    : d(new QNetworkAddressEntryPrivate(*other.d))
{
}

/*!
    Makes a copy of the QNetworkAddressEntry object \a other.
*/
QNetworkAddressEntry &QNetworkAddressEntry::operator=(const QNetworkAddressEntry &other)
{
    *d = *other.d;
    return *this;
}

/*!
    Destroys this QNetworkAddressEntry object.
*/
QNetworkAddressEntry::~QNetworkAddressEntry()
{
    delete d;
}

/*!
    This function returns one IPv4 or IPv6 address found, that was
    found in a network interface.
*/
QHostAddress QNetworkAddressEntry::ip() const
{
    return d->address;
}

/*!
    Sets the IP address the QNetworkAddressEntry object contains to \a
    newIp.
*/
void QNetworkAddressEntry::setIp(const QHostAddress &newIp)
{
    d->address = newIp;
}

/*!
    Returns the netmask associated with the IP address. The
    netmask is expressed in the form of an IP address, such as
    255.255.0.0.

    For IPv6 addresses, the prefix length is converted to an address
    where the number of bits set to 1 is equal to the prefix
    length. For a prefix length of 64 bits (the most common value),
    the netmask will be expressed as a QHostAddress holding the
    address FFFF:FFFF:FFFF:FFFF::
*/
QHostAddress QNetworkAddressEntry::netmask() const
{
    return d->netmask;
}

/*!
    Sets the netmask that this QNetworkAddressEntry object contains to
    \a newNetmask.
*/
void QNetworkAddressEntry::setNetmask(const QHostAddress &newNetmask)
{
    d->netmask = newNetmask;
}

/*!
    Returns the broadcast address associated with the IPv4
    address and netmask. It can usually be derived from those two by
    setting to 1 the bits of the IP address where the netmask contains
    a 0. (In other words, by bitwise-OR'ing the IP address with the
    inverse of the netmask)

    This member is always empty for IPv6 addresses, since the concept
    of broadcast has been abandoned in that system in favor of
    multicast. In particular, the group of hosts corresponding to all
    the nodes in the local network can be reached by the "all-nodes"
    special multicast group (address FF02::1).
*/
QHostAddress QNetworkAddressEntry::broadcast() const
{
    return d->broadcast;
}

/*!
    Sets the broadcast IP address of this QNetworkAddressEntry object
    to \a newBroadcast.
*/
void QNetworkAddressEntry::setBroadcast(const QHostAddress &newBroadcast)
{
    d->broadcast = newBroadcast;
}

/*!
    \class QNetworkInterface
    \brief The QNetworkInterface class provides a listing of the host's IP
    addresses and network interfaces.

    \since 4.2
    \reentrant
    \ingroup io

    QNetworkInterface represents one network interface attached to the
    host where the program is being run. Each network interface may
    contain zero or more IP addresses, each of which is optionally
    associated with a netmask and/or a broadcast address. The list of
    such trios can be obtained with addressEntries(). Alternatively,
    when the netmask or the broadcast addresses aren't necessary, use
    the allAddresses() convenience function to obtain just the IP
    addresses.

    QNetworkInterface also reports the interface's hardware address with
    hardwareAddress().

    Not all operating systems support reporting all features. Only the
    IPv4 addresses are guaranteed to be listed by this class in all
    platforms. In particular, IPv6 address listing is only supported
    on Windows XP and more recent versions, Linux, MacOS X and the
    BSDs.

    \sa QNetworkAddressEntry
*/

/*!
    \enum QNetworkInterface::InterfaceFlag
    Specifies the flags associated with this network interface. The
    possible values are:

    \value IsUp                 the network interface is active
    \value IsRunning            the network interface has resources
                                allocated
    \value CanBroadcast         the network interface works in
                                broadcast mode
    \value IsLoopBack           the network interface is a loopback
                                interface: that is, it's a virtual
                                interface whose destination is the
                                host computer itself
    \value IsPointToPoint       the network interface is a
                                point-to-point interface: that is,
                                there is one, single other address
                                that can be directly reached by it.
    \value CanMulticast         the network interface supports
                                multicasting

    Note that one network interface cannot be both broadcast-based and
    point-to-point.
*/

/*!
    Constructs an empty network interface object.
*/
QNetworkInterface::QNetworkInterface()
    : d(0)
{
}

/*!
    Frees the resources associated with the QNetworkInterface object.
*/
QNetworkInterface::~QNetworkInterface()
{
}

/*!
    Creates a copy of the the QNetworkInterface object contained in \a
    other.
*/
QNetworkInterface::QNetworkInterface(const QNetworkInterface &other)
    : d(other.d)
{
}

/*!
    Copies the contents of the QNetworkInterface object contained in \a
    other into this one.
*/
QNetworkInterface &QNetworkInterface::operator=(const QNetworkInterface &other)
{
    d = other.d;
    return *this;
}

/*!
    Returns true if this QNetworkInterface object contains valid
    information about a network interface.
*/
bool QNetworkInterface::isValid() const
{
    return !name().isEmpty();
}

/*!
    Returns the name of this network interface. On Unix systems, this
    is a string containing the type of the interface and optionally a
    sequence number, such as "eth0", "lo" or "pcn0". On Windows, it's
    an internal ID that cannot be changed by the user.
*/
QString QNetworkInterface::name() const
{
    return d ? d->name : QString();
}

/*!
    Returns the flags associated with this network interface.
*/
QNetworkInterface::InterfaceFlags QNetworkInterface::flags() const
{
    return d ? d->flags : InterfaceFlags(0);
}

/*!
    Returns the low-level hardware address for this interface. On
    Ethernet interfaces, this will be a MAC address in string
    representation, separated by colons.

    Other interface types may have other types of hardware
    addresses. Implementations should not depend on this function
    returning a valid MAC address.
*/
QString QNetworkInterface::hardwareAddress() const
{
    return d ? d->hardwareAddress : QString();
}

/*!
    Returns the list of IP addresses that this interface possesses
    along with their associated netmasks and broadcast addresses.

    If the netmask or broadcast address information is not necessary,
    you can call the allAddresses() function to obtain just the IP
    addresses.
*/
QList<QNetworkAddressEntry> QNetworkInterface::addressEntries() const
{
    return d ? d->addressEntries : QList<QNetworkAddressEntry>();
}

/*!
    Returns a QNetworkInterface object for the interface named \a
    name. If no such interface exists, this function returns an
    invalid QNetworkInterface object.

    \sa name(), isValid()
*/
QNetworkInterface QNetworkInterface::interfaceFromName(const QString &name)
{
    QNetworkInterface result;
    result.d = manager()->interfaceFromName(name);
    return result;
}

/*!
    Returns a QNetworkInterface object for the interface whose internal
    ID is \a index. Network interfaces have a unique identifier called
    the "interface index" to distinguish it from other interfaces on
    the system. Often, this value is assigned progressively and
    interfaces being removed and then added again get a different
    value every time.

    This index is also found in the IPv6 address' scope ID field.
*/
QNetworkInterface QNetworkInterface::interfaceFromIndex(int index)
{
    QNetworkInterface result;
    result.d = manager()->interfaceFromIndex(index);
    return result;
}

/*!
    Returns a listing of all the network interfaces found on the host
    machine.
*/
QList<QNetworkInterface> QNetworkInterface::allInterfaces()
{
    QList<QSharedDataPointer<QNetworkInterfacePrivate> > privs = manager()->allInterfaces();
    QList<QNetworkInterface> result;
    foreach (QSharedDataPointer<QNetworkInterfacePrivate> p, privs) {
        QNetworkInterface item;
        item.d = p;
        result << item;
    }

    return result;
}

/*!
    This convenience function returns all IP addresses found on the
    host machine. It is equivalent to calling addressEntries() on all the
    objects returned by allInterfaces() to obtain lists of QHostAddress
    objects then calling QHostAddress::ip() on each of these.
*/
QList<QHostAddress> QNetworkInterface::allAddresses()
{
    QList<QSharedDataPointer<QNetworkInterfacePrivate> > privs = manager()->allInterfaces();
    QList<QHostAddress> result;
    foreach (const QSharedDataPointer<QNetworkInterfacePrivate> p, privs) {
        foreach (const QNetworkAddressEntry &entry, p->addressEntries)
            result += entry.ip();
    }

    return result;
}

#ifndef QT_NO_DEBUG_STREAM
static inline QDebug flagsDebug(QDebug debug, QNetworkInterface::InterfaceFlags flags)
{
    if (flags & QNetworkInterface::IsUp)
        debug.nospace() << "IsUp ";
    if (flags & QNetworkInterface::IsRunning)
        debug.nospace() << "IsRunning ";
    if (flags & QNetworkInterface::CanBroadcast)
        debug.nospace() << "CanBroadcast ";
    if (flags & QNetworkInterface::IsLoopBack)
        debug.nospace() << "IsLoopBack ";
    if (flags & QNetworkInterface::IsPointToPoint)
        debug.nospace() << "IsPointToPoint ";
    if (flags & QNetworkInterface::CanMulticast)
        debug.nospace() << "CanMulticast ";
    return debug.nospace();
}

static inline QDebug operator<<(QDebug debug, const QNetworkAddressEntry &entry)
{
    debug.nospace() << "(address = " << entry.ip();
    if (!entry.netmask().isNull())
        debug.nospace() << ", netmask = " << entry.netmask();
    if (!entry.broadcast().isNull())
        debug.nospace() << ", broadcast = " << entry.broadcast();
    debug.nospace() << ")";
    return debug.space();
}

QDebug operator<<(QDebug debug, const QNetworkInterface &networkInterface)
{
    debug.nospace() << "QNetworkInterface(name = " << networkInterface.name()
                    << ", hardware address = " << networkInterface.hardwareAddress()
                    << ", flags = ";
    flagsDebug(debug, networkInterface.flags());
    debug.nospace() << ", entries = " << networkInterface.addressEntries()
                    << ")\n";
    return debug.space();
}
#endif

QT_END_NAMESPACE
