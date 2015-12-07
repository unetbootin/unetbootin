/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the QtDBus module of the Qt Toolkit.
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

#include "qdbusinterface.h"

#include <qdbus_symbols_p.h>
#include <QtCore/qpointer.h>
#include <QtCore/qstringlist.h>

#include "qdbusmetatype_p.h"
#include "qdbusinterface_p.h"
#include "qdbusconnection_p.h"

QT_BEGIN_NAMESPACE

QDBusInterfacePrivate::QDBusInterfacePrivate(const QString &serv, const QString &p,
                                             const QString &iface, const QDBusConnection &con)
    : QDBusAbstractInterfacePrivate(serv, p, iface, con, true), metaObject(0)
{
    // QDBusAbstractInterfacePrivate's constructor checked the parameters for us
    if (connection.isConnected()) {
        metaObject = connectionPrivate()->findMetaObject(service, path, interface, lastError);

        if (!metaObject) {
            // creation failed, somehow
            isValid = false;
            if (!lastError.isValid())
                lastError = QDBusError(QDBusError::InternalError, QLatin1String("Unknown error"));
        }
    }
}

QDBusInterfacePrivate::~QDBusInterfacePrivate()
{
    if (metaObject && !metaObject->cached)
        delete metaObject;
}


/*!
    \class QDBusInterface
    \inmodule QtDBus
    \since 4.2

    \brief The QDBusInterface class is a proxy for interfaces on remote objects.

    QDBusInterface is a generic accessor class that is used to place calls to remote objects,
    connect to signals exported by remote objects and get/set the value of remote properties. This
    class is useful for dynamic access to remote objects: that is, when you do not have a generated
    code that represents the remote interface.

    Calls are usually placed by using the call() function, which constructs the message, sends it
    over the bus, waits for the reply and decodes the reply. Signals are connected to by using the
    normal QObject::connect() function. Finally, properties are accessed using the
    QObject::property() and QObject::setProperty() functions.

    The following code snippet demonstrates how to perform a
    mathematical operation of \tt{"2 + 2"} in a remote application
    called \c com.example.Calculator, accessed via the session bus.

    \snippet doc/src/snippets/code/src_qdbus_qdbusinterface.cpp 0

    \sa {QtDBus XML compiler (qdbusxml2cpp)}
*/

/*!
    Creates a dynamic QDBusInterface object associated with the
    interface \a interface on object at path \a path on service \a
    service, using the given \a connection. If \a interface is an
    empty string, the object created will refer to the merging of all
    interfaces found in that object.

    \a parent is passed to the base class constructor.

    If the remote service \a service is not present or if an error
    occurs trying to obtain the description of the remote interface
    \a interface, the object created will not be valid (see
    isValid()).
*/
QDBusInterface::QDBusInterface(const QString &service, const QString &path, const QString &interface,
                               const QDBusConnection &connection, QObject *parent)
    : QDBusAbstractInterface(*new QDBusInterfacePrivate(service, path, interface, connection),
                             parent)
{
}

/*!
    Destroy the object interface and frees up any resource used.
*/
QDBusInterface::~QDBusInterface()
{
    // resources are freed in QDBusInterfacePrivate::~QDBusInterfacePrivate()
}

/*!
    \internal
    Overrides QObject::metaObject to return our own copy.
*/
const QMetaObject *QDBusInterface::metaObject() const
{
    return d_func()->isValid ? d_func()->metaObject : &QDBusAbstractInterface::staticMetaObject;
}

/*!
    \internal
    Override QObject::qt_metacast to catch the interface name too.
*/
void *QDBusInterface::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, "QDBusInterface"))
        return static_cast<void*>(const_cast<QDBusInterface*>(this));
    if (d_func()->interface.toLatin1() == _clname)
        return static_cast<void*>(const_cast<QDBusInterface*>(this));
    return QDBusAbstractInterface::qt_metacast(_clname);
}

/*!
    \internal
    Dispatch the call through the private.
*/
int QDBusInterface::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDBusAbstractInterface::qt_metacall(_c, _id, _a);
    if (_id < 0 || !d_func()->isValid)
        return _id;
    return d_func()->metacall(_c, _id, _a);
}

int QDBusInterfacePrivate::metacall(QMetaObject::Call c, int id, void **argv)
{
    Q_Q(QDBusInterface);

    if (c == QMetaObject::InvokeMetaMethod) {
        int offset = metaObject->methodOffset();
        QMetaMethod mm = metaObject->method(id + offset);

        if (mm.methodType() == QMetaMethod::Signal) {
            // signal relay from D-Bus world to Qt world
            QMetaObject::activate(q, metaObject, id, argv);

        } else if (mm.methodType() == QMetaMethod::Slot) {
            // method call relay from Qt world to D-Bus world
            // get D-Bus equivalent signature
            QString methodName = QLatin1String(metaObject->dbusNameForMethod(id));
            const int *inputTypes = metaObject->inputTypesForMethod(id);
            int inputTypesCount = *inputTypes;

            // we will assume that the input arguments were passed correctly
            QVariantList args;
            for (int i = 1; i <= inputTypesCount; ++i)
                args << QVariant(inputTypes[i], argv[i]);

            // make the call
            QPointer<QDBusInterface> qq = q;
            QDBusMessage reply = q->callWithArgumentList(QDBus::Block, methodName, args);
            args.clear();

            // we ignore return values

            // access to "this" or to "q" below this point must check for "qq"
            // we may have been deleted!

            if (!qq.isNull())
                lastError = reply;

            // done
            return -1;
        }
    } else if (c == QMetaObject::ReadProperty) {
        // Qt doesn't support non-readable properties
        // we have to re-check
        QMetaProperty mp = metaObject->property(id + metaObject->propertyOffset());
        if (!mp.isReadable())
            return -1;          // don't read

        QVariant *value = reinterpret_cast<QVariant*>(argv[1]);
        argv[1] = 0;
        *value = property(mp);

        return -1;              // handled, error or not
    } else if (c == QMetaObject::WriteProperty) {
        // QMetaProperty::write has already checked that we're writable
        // it has also checked that the type is right
        QVariant *value = reinterpret_cast<QVariant *>(argv[1]);
        QMetaProperty mp = metaObject->property(id + metaObject->propertyOffset());

        setProperty(mp, *value);
        return -1;
    }
    return id;
}

QT_END_NAMESPACE
