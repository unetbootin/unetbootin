/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the QtDBus module of the Qt Toolkit.
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

#ifndef QDBUSBUS_H
#define QDBUSBUS_H

#include <QtCore/qstringlist.h>

#include <QtDBus/qdbusabstractinterface.h>
#include <QtDBus/qdbusreply.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(DBus)

class QDBusConnection;
class QString;
class QByteArray;

/*
 * Proxy class for interface org.freedesktop.DBus
 */
class QDBUS_EXPORT QDBusConnectionInterface: public QDBusAbstractInterface
{
    Q_OBJECT
    Q_ENUMS(ServiceQueueOptions ServiceReplacementOptions RegisterServiceReply)
    friend class QDBusConnectionPrivate;
    static inline const char *staticInterfaceName();

    explicit QDBusConnectionInterface(const QDBusConnection &connection, QObject *parent);
    ~QDBusConnectionInterface();

    Q_PROPERTY(QDBusReply<QStringList> registeredServiceNames READ registeredServiceNames)

public:
    enum ServiceQueueOptions {
        DontQueueService,
        QueueService,
        ReplaceExistingService
    };
    enum ServiceReplacementOptions {
        DontAllowReplacement,
        AllowReplacement
    };
    enum RegisterServiceReply {
        ServiceNotRegistered = 0,
        ServiceRegistered,
        ServiceQueued
    };

public Q_SLOTS:
    QDBusReply<QStringList> registeredServiceNames() const;
    QDBusReply<bool> isServiceRegistered(const QString &serviceName) const;
    QDBusReply<QString> serviceOwner(const QString &name) const;
    QDBusReply<bool> unregisterService(const QString &serviceName);
    QDBusReply<QDBusConnectionInterface::RegisterServiceReply> registerService(const QString &serviceName,
                                                     ServiceQueueOptions qoption = DontQueueService,
                                                     ServiceReplacementOptions roption = DontAllowReplacement);

    QDBusReply<uint> servicePid(const QString &serviceName) const;
    QDBusReply<uint> serviceUid(const QString &serviceName) const;

    QDBusReply<void> startService(const QString &name);

Q_SIGNALS:
    void serviceRegistered(const QString &service);
    void serviceUnregistered(const QString &service);
    void serviceOwnerChanged(const QString &name, const QString &oldOwner, const QString &newOwner);
    void callWithCallbackFailed(const QDBusError &error, const QDBusMessage &call);

#ifndef Q_QDOC
    // internal signals
    // do not use
    void NameAcquired(const QString &);
    void NameLost(const QString &);
    void NameOwnerChanged(const QString &, const QString &, const QString &);
protected:
    void connectNotify(const char *);
    void disconnectNotify(const char *);
#endif
};

QT_END_NAMESPACE

Q_DECLARE_BUILTIN_METATYPE(QDBusConnectionInterface::RegisterServiceReply, UInt)

QT_END_HEADER

#endif
