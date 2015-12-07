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

#ifndef QNETWORKINTERFACE_H
#define QNETWORKINTERFACE_H

#include <QtCore/qshareddata.h>
#include <QtNetwork/qhostaddress.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Network)

template<typename T> class QList;

class QNetworkAddressEntryPrivate;
class Q_NETWORK_EXPORT QNetworkAddressEntry
{
public:
    QNetworkAddressEntry();
    QNetworkAddressEntry(const QNetworkAddressEntry &other);
    QNetworkAddressEntry &operator=(const QNetworkAddressEntry &other);
    ~QNetworkAddressEntry();
    
    QHostAddress ip() const;
    void setIp(const QHostAddress &newIp);
    
    QHostAddress netmask() const;
    void setNetmask(const QHostAddress &newNetmask);
    
    QHostAddress broadcast() const;
    void setBroadcast(const QHostAddress &newBroadcast);

private:
    QNetworkAddressEntryPrivate *d;
};

class QNetworkInterfacePrivate;
class Q_NETWORK_EXPORT QNetworkInterface
{
public:
    enum InterfaceFlag {
        IsUp = 0x1,
        IsRunning = 0x2,
        CanBroadcast = 0x4,
        IsLoopBack = 0x8,
        IsPointToPoint = 0x10,
        CanMulticast = 0x20
    };
    Q_DECLARE_FLAGS(InterfaceFlags, InterfaceFlag)
    
    QNetworkInterface();
    QNetworkInterface(const QNetworkInterface &other);
    QNetworkInterface &operator=(const QNetworkInterface &other);
    ~QNetworkInterface();

    bool isValid() const;

    QString name() const;
    InterfaceFlags flags() const;
    QString hardwareAddress() const;
    QList<QNetworkAddressEntry> addressEntries() const;

    static QNetworkInterface interfaceFromName(const QString &name);
    static QNetworkInterface interfaceFromIndex(int index);
    static QList<QNetworkInterface> allInterfaces();
    static QList<QHostAddress> allAddresses();

private:
    friend class QNetworkInterfacePrivate;
    QSharedDataPointer<QNetworkInterfacePrivate> d;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(QNetworkInterface::InterfaceFlags)

#ifndef QT_NO_DEBUG_STREAM
Q_NETWORK_EXPORT QDebug operator<<(QDebug debug, const QNetworkInterface &networkInterface);
#endif

QT_END_NAMESPACE

QT_END_HEADER

#endif
