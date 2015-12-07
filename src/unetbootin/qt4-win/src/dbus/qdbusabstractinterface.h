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

#ifndef QDBUSABSTRACTINTERFACE_H
#define QDBUSABSTRACTINTERFACE_H

#include <QtCore/qstring.h>
#include <QtCore/qvariant.h>
#include <QtCore/qlist.h>
#include <QtCore/qobject.h>

#include <QtDBus/qdbusmessage.h>
#include <QtDBus/qdbusextratypes.h>
#include <QtDBus/qdbusconnection.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(DBus)

class QDBusError;

class QDBusAbstractInterfacePrivate;
class QDBUS_EXPORT QDBusAbstractInterface: public QObject
{
    Q_OBJECT

public:
    virtual ~QDBusAbstractInterface();
    bool isValid() const;

    QDBusConnection connection() const;

    QString service() const;
    QString path() const;
    QString interface() const;

    QDBusError lastError() const;

    QDBusMessage call(const QString &method,
                      const QVariant &arg1 = QVariant(),
                      const QVariant &arg2 = QVariant(),
                      const QVariant &arg3 = QVariant(),
                      const QVariant &arg4 = QVariant(),
                      const QVariant &arg5 = QVariant(),
                      const QVariant &arg6 = QVariant(),
                      const QVariant &arg7 = QVariant(),
                      const QVariant &arg8 = QVariant());

    QDBusMessage call(QDBus::CallMode mode,
                      const QString &method,
                      const QVariant &arg1 = QVariant(),
                      const QVariant &arg2 = QVariant(),
                      const QVariant &arg3 = QVariant(),
                      const QVariant &arg4 = QVariant(),
                      const QVariant &arg5 = QVariant(),
                      const QVariant &arg6 = QVariant(),
                      const QVariant &arg7 = QVariant(),
                      const QVariant &arg8 = QVariant());

    QDBusMessage callWithArgumentList(QDBus::CallMode mode,
                                      const QString &method,
                                      const QList<QVariant> &args);

    bool callWithCallback(const QString &method,
                          const QList<QVariant> &args,
                          QObject *receiver, const char *member, const char *errorSlot);
    bool callWithCallback(const QString &method,
                          const QList<QVariant> &args,
                          QObject *receiver, const char *member);

protected:
    QDBusAbstractInterface(const QString &service, const QString &path, const char *interface,
                           const QDBusConnection &connection, QObject *parent);
    QDBusAbstractInterface(QDBusAbstractInterfacePrivate &, QObject *parent);

    void connectNotify(const char *signal);
    void disconnectNotify(const char *signal);
    QVariant internalPropGet(const char *propname) const;
    void internalPropSet(const char *propname, const QVariant &value);
    QDBusMessage internalConstCall(QDBus::CallMode mode,
                                   const QString &method,
                                   const QList<QVariant> &args = QList<QVariant>()) const;

private:
    Q_DECLARE_PRIVATE(QDBusAbstractInterface)
    Q_PRIVATE_SLOT(d_func(), void _q_serviceOwnerChanged(QString,QString,QString))
};

QT_END_NAMESPACE

QT_END_HEADER

#endif
