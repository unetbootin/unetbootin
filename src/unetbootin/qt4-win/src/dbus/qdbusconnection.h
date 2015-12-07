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

#ifndef QDBUSCONNECTION_H
#define QDBUSCONNECTION_H

#include <QtDBus/qdbusmacros.h>
#include <QtCore/qstring.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(DBus)

namespace QDBus
{
    enum CallMode {
        NoBlock,
        Block,
        BlockWithGui,
        AutoDetect
    };
}

class QDBusAbstractInterfacePrivate;
class QDBusInterface;
class QDBusError;
class QDBusMessage;
class QDBusConnectionInterface;
class QObject;

class QDBusConnectionPrivate;
class QDBUS_EXPORT QDBusConnection
{
    Q_GADGET
    Q_ENUMS(BusType UnregisterMode)
    Q_FLAGS(RegisterOptions)
public:
    enum BusType { SessionBus, SystemBus, ActivationBus };
    enum RegisterOption {
        ExportAdaptors = 0x01,

        ExportScriptableSlots = 0x10,
        ExportScriptableSignals = 0x20,
        ExportScriptableProperties = 0x40,
        ExportScriptableContents = 0xf0,

        ExportNonScriptableSlots = 0x100,
        ExportNonScriptableSignals = 0x200,
        ExportNonScriptableProperties = 0x400,
        ExportNonScriptableContents = 0xf00,

        ExportAllSlots = ExportScriptableSlots|ExportNonScriptableSlots,
        ExportAllSignals = ExportScriptableSignals|ExportNonScriptableSignals,
        ExportAllProperties = ExportScriptableProperties|ExportNonScriptableProperties,
        ExportAllContents = ExportScriptableContents|ExportNonScriptableContents,

#ifndef Q_QDOC
        // Qt 4.2 had a misspelling here
        ExportAllSignal = ExportAllSignals,
#endif

        ExportChildObjects = 0x1000
    };
    enum UnregisterMode {
        UnregisterNode,
        UnregisterTree
    };

    Q_DECLARE_FLAGS(RegisterOptions, RegisterOption)

    QDBusConnection(const QString &name);
    QDBusConnection(const QDBusConnection &other);
    ~QDBusConnection();

    QDBusConnection &operator=(const QDBusConnection &other);

    bool isConnected() const;
    QString baseService() const;
    QDBusError lastError() const;

    bool send(const QDBusMessage &message) const;
    bool callWithCallback(const QDBusMessage &message, QObject *receiver,
                          const char *returnMethod, const char *errorMethod,
                          int timeout = -1) const;
    bool callWithCallback(const QDBusMessage &message, QObject *receiver,
                          const char *slot, int timeout = -1) const;
    QDBusMessage call(const QDBusMessage &message, QDBus::CallMode mode = QDBus::Block,
                      int timeout = -1) const;

    bool connect(const QString &service, const QString &path, const QString &interface,
                 const QString &name, QObject *receiver, const char *slot);
    bool disconnect(const QString &service, const QString &path, const QString &interface,
                    const QString &name, QObject *receiver, const char *slot);

    bool connect(const QString &service, const QString &path, const QString &interface,
                 const QString &name, const QString& signature,
                 QObject *receiver, const char *slot);
    bool disconnect(const QString &service, const QString &path, const QString &interface,
                    const QString &name, const QString& signature,
                    QObject *receiver, const char *slot);

    bool registerObject(const QString &path, QObject *object,
                        RegisterOptions options = ExportAdaptors);
    void unregisterObject(const QString &path, UnregisterMode mode = UnregisterNode);
    QObject *objectRegisteredAt(const QString &path) const;

    bool registerService(const QString &serviceName);
    bool unregisterService(const QString &serviceName);

    QDBusConnectionInterface *interface() const;

    static QDBusConnection connectToBus(BusType type, const QString &name);
    static QDBusConnection connectToBus(const QString &address, const QString &name);
    static void disconnectFromBus(const QString &name);

    static QDBusConnection sessionBus();
    static QDBusConnection systemBus();

    static QDBusConnection sender();

protected:
    explicit QDBusConnection(QDBusConnectionPrivate *dd);

private:
    friend class QDBusConnectionPrivate;
    QDBusConnectionPrivate *d;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(QDBusConnection::RegisterOptions)

QT_END_NAMESPACE

QT_END_HEADER

#endif
