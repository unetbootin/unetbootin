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
#include "qnetworkinterface_win_p.h"
#include <qhash.h>

QT_BEGIN_NAMESPACE

typedef DWORD (WINAPI *PtrGetAdaptersInfo)(PIP_ADAPTER_INFO, PULONG);
PtrGetAdaptersInfo ptrGetAdaptersInfo = 0;
typedef ULONG (WINAPI *PtrGetAdaptersAddresses)(ULONG, ULONG, PVOID, PIP_ADAPTER_ADDRESSES, PULONG);
PtrGetAdaptersAddresses ptrGetAdaptersAddresses = 0;

static void resolveLibs()
{
    // try to find the functions we need from Iphlpapi.dll
    static bool done = false;

    if (!done) {
        done = true;

        HINSTANCE iphlpapiHnd;
        QT_WA({
                iphlpapiHnd = LoadLibraryW(L"iphlpapi");
            }, {
                iphlpapiHnd = LoadLibraryA("iphlpapi");
            });
        if (iphlpapiHnd == NULL)
            return;             // failed to load, probably Windows 95

#if defined(Q_OS_WINCE)
        ptrGetAdaptersInfo = (PtrGetAdaptersInfo)GetProcAddressW(iphlpapiHnd, L"GetAdaptersInfo");
        ptrGetAdaptersAddresses = (PtrGetAdaptersAddresses)GetProcAddressW(iphlpapiHnd, L"GetAdaptersAddresses");
#else
        ptrGetAdaptersInfo = (PtrGetAdaptersInfo)GetProcAddress(iphlpapiHnd, "GetAdaptersInfo");
        ptrGetAdaptersAddresses = (PtrGetAdaptersAddresses)GetProcAddress(iphlpapiHnd, "GetAdaptersAddresses");
#endif
    }
}

static QHostAddress addressFromSockaddr(sockaddr *sa)
{
    QHostAddress address;
    if (!sa)
        return address;

    if (sa->sa_family == AF_INET)
        address.setAddress(htonl(((sockaddr_in *)sa)->sin_addr.s_addr));
    else if (sa->sa_family == AF_INET6)
        address.setAddress(((qt_sockaddr_in6 *)sa)->sin6_addr.qt_s6_addr);
    else
        qWarning("Got unknown socket family %d", sa->sa_family);
    return address;

}

static QHostAddress netmaskFromPrefixLength(uint len, int family)
{
    Q_ASSERT(len <= 128);
    union {
        quint8 u8[16];
        quint32 ipv4;
    } data;

    // initialize data
    memset(&data, 0, sizeof data);

    quint8 *ptr = data.u8;
    while (len > 0) {
        switch (len) {
        default:
            // a byte or more left
            *ptr |= 0x01;
        case 7:
            *ptr |= 0x02;
        case 6:
            *ptr |= 0x04;
        case 5:
            *ptr |= 0x08;
        case 4:
            *ptr |= 0x10;
        case 3:
            *ptr |= 0x20;
        case 2:
            *ptr |= 0x40;
        case 1:
            *ptr |= 0x80;
        }

        if (len < 8)
            break;
        len -= 8;
        ++ptr;
    }

    if (family == AF_INET)
        return QHostAddress(htonl(data.ipv4));
    else if (family == AF_INET6)
        return QHostAddress(data.u8);
    else
        return QHostAddress();
}

static QHash<QHostAddress, QHostAddress> ipv4Netmasks()
{
    //Retrieve all the IPV4 addresses & netmasks
    IP_ADAPTER_INFO staticBuf[2]; // 2 is arbitrary
    PIP_ADAPTER_INFO pAdapter = staticBuf;
    ULONG bufSize = sizeof staticBuf;
    QHash<QHostAddress, QHostAddress> ipv4netmasks; 

    DWORD retval = ptrGetAdaptersInfo(pAdapter, &bufSize);
    if (retval == ERROR_BUFFER_OVERFLOW) {
        // need more memory
        pAdapter = (IP_ADAPTER_INFO *)qMalloc(bufSize);
        // try again
        if (ptrGetAdaptersInfo(pAdapter, &bufSize) != ERROR_SUCCESS) {
            qFree(pAdapter);
            return ipv4netmasks;
        }
    } else if (retval != ERROR_SUCCESS) {
        // error
        return ipv4netmasks;
    }

    // iterate over the list and add the entries to our listing
    for (PIP_ADAPTER_INFO ptr = pAdapter; ptr; ptr = ptr->Next) {
        for (PIP_ADDR_STRING addr = &ptr->IpAddressList; addr; addr = addr->Next) {
            QHostAddress address(QLatin1String(addr->IpAddress.String));
            QHostAddress mask(QLatin1String(addr->IpMask.String));
            ipv4netmasks[address] = mask;
        }
    }
    if (pAdapter != staticBuf)
        qFree(pAdapter);

    return ipv4netmasks;

}

static QList<QNetworkInterfacePrivate *> interfaceListingWinXP()
{
    QList<QNetworkInterfacePrivate *> interfaces;
    IP_ADAPTER_ADDRESSES staticBuf[2]; // 2 is arbitrary
    PIP_ADAPTER_ADDRESSES pAdapter = staticBuf;
    ULONG bufSize = sizeof staticBuf;

    const QHash<QHostAddress, QHostAddress> &ipv4netmasks = ipv4Netmasks();
    ULONG flags = GAA_FLAG_INCLUDE_ALL_INTERFACES |
                  GAA_FLAG_INCLUDE_PREFIX |
                  GAA_FLAG_SKIP_DNS_SERVER |
                  GAA_FLAG_SKIP_MULTICAST;
    ULONG retval = ptrGetAdaptersAddresses(AF_UNSPEC, flags, NULL, pAdapter, &bufSize);
    if (retval == ERROR_BUFFER_OVERFLOW) {
        // need more memory
        pAdapter = (IP_ADAPTER_ADDRESSES *)qMalloc(bufSize);

        // try again
        if (ptrGetAdaptersAddresses(AF_UNSPEC, flags, NULL, pAdapter, &bufSize) != ERROR_SUCCESS) {
            qFree(pAdapter);
            return interfaces;
        }
    } else if (retval != ERROR_SUCCESS) {
        // error
        return interfaces;
    }

    // iterate over the list and add the entries to our listing
    for (PIP_ADAPTER_ADDRESSES ptr = pAdapter; ptr; ptr = ptr->Next) {
        QNetworkInterfacePrivate *iface = new QNetworkInterfacePrivate;
        interfaces << iface;

        iface->index = 0;
        if (ptr->Length >= offsetof(IP_ADAPTER_ADDRESSES, Ipv6IfIndex))
            iface->index = ptr->Ipv6IfIndex;
        else if (ptr->IfIndex != 0)
            iface->index = ptr->IfIndex;

        iface->flags = QNetworkInterface::CanBroadcast;
		if (ptr->OperStatus == IfOperStatusUp)
			iface->flags |= QNetworkInterface::IsUp | QNetworkInterface::IsRunning;
        if ((ptr->Flags & IP_ADAPTER_NO_MULTICAST) == 0)
            iface->flags |= QNetworkInterface::CanMulticast;

        iface->name = QString::fromLocal8Bit(ptr->AdapterName);
        if (ptr->PhysicalAddressLength)
            iface->hardwareAddress = iface->makeHwAddress(ptr->PhysicalAddressLength,
                                                          ptr->PhysicalAddress);
        else
            // loopback if it has no address
            iface->flags |= QNetworkInterface::IsLoopBack;

        // The GetAdaptersAddresses call has an interesting semantic:
        // It can return a number N of addresses and a number M of prefixes.
        // But if you have IPv6 addresses, generally N > M.
        // I cannot find a way to relate the Address to the Prefix, aside from stopping
        // the iteration at the last Prefix entry and assume that it applies to all addresses
        // from that point on.
        PIP_ADAPTER_PREFIX pprefix = 0;
        if (ptr->Length >= offsetof(IP_ADAPTER_ADDRESSES, FirstPrefix))
            pprefix = ptr->FirstPrefix;
        for (PIP_ADAPTER_UNICAST_ADDRESS addr = ptr->FirstUnicastAddress; addr; addr = addr->Next) {
            QNetworkAddressEntry entry;
            entry.setIp(addressFromSockaddr(addr->Address.lpSockaddr));
            if (pprefix) {
                if (entry.ip().protocol() == QAbstractSocket::IPv4Protocol) {
                    entry.setNetmask(ipv4netmasks[entry.ip()]);

                    // broadcast address is set on postProcess()
                } else { //IPV6
                    entry.setNetmask(netmaskFromPrefixLength(pprefix->PrefixLength, addr->Address.lpSockaddr->sa_family));
                }
                pprefix = pprefix->Next ? pprefix->Next : pprefix;
            }
            iface->addressEntries << entry;
        }
    }

    if (pAdapter != staticBuf)
        qFree(pAdapter);

    return interfaces;
}

static QList<QNetworkInterfacePrivate *> interfaceListingWin2k()
{
    QList<QNetworkInterfacePrivate *> interfaces;
    IP_ADAPTER_INFO staticBuf[2]; // 2 is arbitrary
    PIP_ADAPTER_INFO pAdapter = staticBuf;
    ULONG bufSize = sizeof staticBuf;

    DWORD retval = ptrGetAdaptersInfo(pAdapter, &bufSize);
    if (retval == ERROR_BUFFER_OVERFLOW) {
        // need more memory
        pAdapter = (IP_ADAPTER_INFO *)qMalloc(bufSize);

        // try again
        if (ptrGetAdaptersInfo(pAdapter, &bufSize) != ERROR_SUCCESS) {
            qFree(pAdapter);
            return interfaces;
        }
    } else if (retval != ERROR_SUCCESS) {
        // error
        return interfaces;
    }

    // iterate over the list and add the entries to our listing
    for (PIP_ADAPTER_INFO ptr = pAdapter; ptr; ptr = ptr->Next) {
        QNetworkInterfacePrivate *iface = new QNetworkInterfacePrivate;
        interfaces << iface;

        iface->index = ptr->Index;
        iface->flags = QNetworkInterface::IsUp | QNetworkInterface::IsRunning;
        if (ptr->Type == MIB_IF_TYPE_PPP)
            iface->flags |= QNetworkInterface::IsPointToPoint;
        else
            iface->flags |= QNetworkInterface::CanBroadcast;
        iface->name = QString::fromLocal8Bit(ptr->AdapterName);
        iface->hardwareAddress = QNetworkInterfacePrivate::makeHwAddress(ptr->AddressLength,
                                                                         ptr->Address);

        for (PIP_ADDR_STRING addr = &ptr->IpAddressList; addr; addr = addr->Next) {
            QNetworkAddressEntry entry;
            entry.setIp(QHostAddress(QLatin1String(addr->IpAddress.String)));
            entry.setNetmask(QHostAddress(QLatin1String(addr->IpMask.String)));
            // broadcast address is set on postProcess()

            iface->addressEntries << entry;
        }
    }

    if (pAdapter != staticBuf)
        qFree(pAdapter);

    return interfaces;
}

static QList<QNetworkInterfacePrivate *> interfaceListing()
{
    resolveLibs();
    if (ptrGetAdaptersAddresses != NULL)
        return interfaceListingWinXP();
    else if (ptrGetAdaptersInfo != NULL)
        return interfaceListingWin2k();

    // failed
    return QList<QNetworkInterfacePrivate *>();
}

QList<QNetworkInterfacePrivate *> QNetworkInterfaceManager::scan()
{
    return interfaceListing();
}

QT_END_NAMESPACE
