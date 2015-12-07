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

#include "qdbusconnection_p.h"

#include <qdbus_symbols_p.h>
#include <QtCore/qcoreapplication.h>
#include <QtCore/qmetaobject.h>
#include <QtCore/qstringlist.h>
#include <QtCore/qthread.h>

#include "qdbusabstractadaptor.h"
#include "qdbusabstractadaptor_p.h"
#include "qdbusconnection.h"
#include "qdbusextratypes.h"
#include "qdbusmessage.h"
#include "qdbusmetatype.h"
#include "qdbusmessage_p.h"
#include "qdbusutil_p.h"

QT_BEGIN_NAMESPACE

// defined in qdbusxmlgenerator.cpp
extern QString qDBusGenerateMetaObjectXml(QString interface, const QMetaObject *mo,
                                          const QMetaObject *base, int flags);

static const char introspectableInterfaceXml[] =
    "  <interface name=\"org.freedesktop.DBus.Introspectable\">\n"
    "    <method name=\"Introspect\">\n"
    "      <arg name=\"xml_data\" type=\"s\" direction=\"out\"/>\n"
    "    </method>\n"
    "  </interface>\n";

static const char propertiesInterfaceXml[] =
    "  <interface name=\"org.freedesktop.DBus.Properties\">\n"
    "    <method name=\"Get\">\n"
    "      <arg name=\"interface_name\" type=\"s\" direction=\"in\"/>\n"
    "      <arg name=\"property_name\" type=\"s\" direction=\"in\"/>\n"
    "      <arg name=\"value\" type=\"v\" direction=\"out\"/>\n"
    "    </method>\n"
    "    <method name=\"Set\">\n"
    "      <arg name=\"interface_name\" type=\"s\" direction=\"in\"/>\n"
    "      <arg name=\"property_name\" type=\"s\" direction=\"in\"/>\n"
    "      <arg name=\"value\" type=\"v\" direction=\"in\"/>\n"
    "    </method>\n"
    "    <method name=\"GetAll\">\n"
    "      <arg name=\"interface_name\" type=\"s\" direction=\"in\"/>\n"
    "      <arg name=\"values\" type=\"a{sv}\" direction=\"out\"/>\n"
    "      <annotation name=\"com.trolltech.QtDBus.QtTypeName.Out0\" value=\"QVariantMap\"/>"
    "    </method>\n"
    "  </interface>\n";

static QString generateSubObjectXml(QObject *object)
{
    QString retval;
    const QObjectList &objs = object->children();
    QObjectList::ConstIterator it = objs.constBegin();
    QObjectList::ConstIterator end = objs.constEnd();
    for ( ; it != end; ++it) {
        QString name = (*it)->objectName();
        if (!name.isEmpty() && QDBusUtil::isValidPartOfObjectPath(name))
            retval += QString::fromLatin1("  <node name=\"%1\"/>\n")
                      .arg(name);
    }
    return retval;
}

// declared as extern in qdbusconnection_p.h

QString qDBusIntrospectObject(const QDBusConnectionPrivate::ObjectTreeNode &node)
{
    // object may be null

    QString xml_data(QLatin1String(DBUS_INTROSPECT_1_0_XML_DOCTYPE_DECL_NODE));
    xml_data += QLatin1String("<node>\n");

    if (node.obj) {
        Q_ASSERT_X(QThread::currentThread() == node.obj->thread(),
                   "QDBusConnection: internal threading error",
                   "function called for an object that is in another thread!!");

        if (node.flags & (QDBusConnection::ExportScriptableContents
                           | QDBusConnection::ExportNonScriptableContents)) {
            // create XML for the object itself
            const QMetaObject *mo = node.obj->metaObject();
            for ( ; mo != &QObject::staticMetaObject; mo = mo->superClass())
                xml_data += qDBusGenerateMetaObjectXml(QString(), mo, mo->superClass(),
                                                       node.flags);
        }

        // does this object have adaptors?
        QDBusAdaptorConnector *connector;
        if (node.flags & QDBusConnection::ExportAdaptors &&
            (connector = qDBusFindAdaptorConnector(node.obj))) {

            // trasverse every adaptor in this object
            QDBusAdaptorConnector::AdaptorMap::ConstIterator it = connector->adaptors.constBegin();
            QDBusAdaptorConnector::AdaptorMap::ConstIterator end = connector->adaptors.constEnd();
            for ( ; it != end; ++it) {
                // add the interface:
                QString ifaceXml = QDBusAbstractAdaptorPrivate::retrieveIntrospectionXml(it->adaptor);
                if (ifaceXml.isEmpty()) {
                    // add the interface's contents:
                    ifaceXml += qDBusGenerateMetaObjectXml(QString::fromLatin1(it->interface),
                                                           it->adaptor->metaObject(),
                                                           &QDBusAbstractAdaptor::staticMetaObject,
                                                           QDBusConnection::ExportScriptableContents
                                                           | QDBusConnection::ExportNonScriptableContents);

                    QDBusAbstractAdaptorPrivate::saveIntrospectionXml(it->adaptor, ifaceXml);
                }

                xml_data += ifaceXml;
            }
        }

        xml_data += QLatin1String( propertiesInterfaceXml );
    }

    xml_data += QLatin1String( introspectableInterfaceXml );

    if (node.flags & QDBusConnection::ExportChildObjects) {
        xml_data += generateSubObjectXml(node.obj);
    } else {
        // generate from the object tree
        QDBusConnectionPrivate::ObjectTreeNode::DataList::ConstIterator it =
            node.children.constBegin();
        QDBusConnectionPrivate::ObjectTreeNode::DataList::ConstIterator end =
            node.children.constEnd();
        for ( ; it != end; ++it)
            if (it->obj || !it->children.isEmpty())
                xml_data += QString::fromLatin1("  <node name=\"%1\"/>\n")
                            .arg(it->name);
    }

    xml_data += QLatin1String("</node>\n");
    return xml_data;
}

// implement the D-Bus interface org.freedesktop.DBus.Properties

static QDBusMessage qDBusPropertyError(const QDBusMessage &msg, const QString &interface_name)
{
    return msg.createErrorReply(QLatin1String(DBUS_ERROR_INVALID_ARGS),
                                QString::fromLatin1("Interface %1 was not found in object %2")
                                .arg(interface_name)
                                .arg(msg.path()));
}

QDBusMessage qDBusPropertyGet(const QDBusConnectionPrivate::ObjectTreeNode &node,
                              const QDBusMessage &msg)
{
    Q_ASSERT(msg.arguments().count() == 2);
    Q_ASSERT_X(!node.obj || QThread::currentThread() == node.obj->thread(),
               "QDBusConnection: internal threading error",
               "function called for an object that is in another thread!!");

    QString interface_name = msg.arguments().at(0).toString();
    QByteArray property_name = msg.arguments().at(1).toString().toUtf8();

    QDBusAdaptorConnector *connector;
    QVariant value;
    if (node.flags & QDBusConnection::ExportAdaptors &&
        (connector = qDBusFindAdaptorConnector(node.obj))) {

        // find the class that implements interface_name or try until we've found the property
        // in case of an empty interface
        if (interface_name.isEmpty()) {
            for (QDBusAdaptorConnector::AdaptorMap::ConstIterator it = connector->adaptors.constBegin(),
                 end = connector->adaptors.constEnd(); it != end; ++it) {
                const QMetaObject *mo = it->adaptor->metaObject();
                int pidx = mo->indexOfProperty(property_name);
                if (pidx != -1) {
                    value = mo->property(pidx).read(it->adaptor);
                    break;
                }
            }
        } else {
            QDBusAdaptorConnector::AdaptorMap::ConstIterator it;
            it = qLowerBound(connector->adaptors.constBegin(), connector->adaptors.constEnd(),
                             interface_name);
            if (it != connector->adaptors.constEnd() && interface_name == QLatin1String(it->interface))
                value = it->adaptor->property(property_name);
        }
    }

    if (!value.isValid() && node.flags & (QDBusConnection::ExportAllProperties |
                                          QDBusConnection::ExportNonScriptableProperties)) {
        // try the object itself
        int pidx = node.obj->metaObject()->indexOfProperty(property_name);
        if (pidx != -1) {
            QMetaProperty mp = node.obj->metaObject()->property(pidx);
            if ((mp.isScriptable() && (node.flags & QDBusConnection::ExportScriptableProperties)) ||
                (!mp.isScriptable() && (node.flags & QDBusConnection::ExportNonScriptableProperties)))
                value = mp.read(node.obj);
        }
    }

    if (!value.isValid()) {
        // the property was not found
        return qDBusPropertyError(msg, interface_name);
    }

    return msg.createReply(qVariantFromValue(QDBusVariant(value)));
}

QDBusMessage qDBusPropertySet(const QDBusConnectionPrivate::ObjectTreeNode &node,
                              const QDBusMessage &msg)
{
    Q_ASSERT(msg.arguments().count() == 3);
    Q_ASSERT_X(!node.obj || QThread::currentThread() == node.obj->thread(),
               "QDBusConnection: internal threading error",
               "function called for an object that is in another thread!!");

    QString interface_name = msg.arguments().at(0).toString();
    QByteArray property_name = msg.arguments().at(1).toString().toUtf8();
    QVariant value = qvariant_cast<QDBusVariant>(msg.arguments().at(2)).variant();

    QDBusAdaptorConnector *connector;
    if (node.flags & QDBusConnection::ExportAdaptors &&
        (connector = qDBusFindAdaptorConnector(node.obj))) {

        // find the class that implements interface_name or try until we've found the property
        // in case of an empty interface
        if (interface_name.isEmpty()) {
            for (QDBusAdaptorConnector::AdaptorMap::ConstIterator it = connector->adaptors.constBegin(),
                 end = connector->adaptors.constEnd(); it != end; ++it) {
                const QMetaObject *mo = it->adaptor->metaObject();
                int pidx = mo->indexOfProperty(property_name);
                if (pidx != -1) {
                    mo->property(pidx).write(it->adaptor, value);
                    return msg.createReply();
                }
            }
        } else {
            QDBusAdaptorConnector::AdaptorMap::ConstIterator it;
            it = qLowerBound(connector->adaptors.constBegin(), connector->adaptors.constEnd(),
                             interface_name);
            if (it != connector->adaptors.end() && interface_name == QLatin1String(it->interface))
                if (it->adaptor->setProperty(property_name, value))
                    return msg.createReply();
        }
    }

    if (node.flags & (QDBusConnection::ExportScriptableProperties |
                      QDBusConnection::ExportNonScriptableProperties)) {
        // try the object itself
        int pidx = node.obj->metaObject()->indexOfProperty(property_name);
        if (pidx != -1) {
            QMetaProperty mp = node.obj->metaObject()->property(pidx);
            if ((mp.isScriptable() && (node.flags & QDBusConnection::ExportScriptableProperties)) ||
                (!mp.isScriptable() && (node.flags & QDBusConnection::ExportNonScriptableProperties)))
                if (mp.write(node.obj, value))
                    return msg.createReply();
        }
    }

    // the property was not found or not written to
    return qDBusPropertyError(msg, interface_name);
}

// unite two QVariantMaps, but don't generate duplicate keys
static QVariantMap &operator+=(QVariantMap &lhs, const QVariantMap &rhs)
{
    QVariantMap::ConstIterator it = rhs.constBegin(),
                              end = rhs.constEnd();
    for ( ; it != end; ++it)
        lhs.insert(it.key(), it.value());
    return lhs;
}

static QVariantMap readAllProperties(QObject *object, int flags)
{
    QVariantMap result;
    const QMetaObject *mo = object->metaObject();

    // QObject has properties, so don't start from 0
    for (int i = QObject::staticMetaObject.propertyCount(); i < mo->propertyCount(); ++i) {
        QMetaProperty mp = mo->property(i);

        // is it readable?
        if (!mp.isReadable())
            continue;

        // is it a registered property?
        int typeId = qDBusNameToTypeId(mp.typeName());
        if (!typeId)
            continue;
        const char *signature = QDBusMetaType::typeToSignature(typeId);
        if (!signature)
            continue;

        // is this property visible from the outside?
        if ((mp.isScriptable() && flags & QDBusConnection::ExportScriptableProperties) ||
            (!mp.isScriptable() && flags & QDBusConnection::ExportNonScriptableProperties)) {
            // yes, it's visible
            QVariant value = mp.read(object);
            if (value.isValid())
                result.insert(QString::fromLatin1(mp.name()), value);
        }
    }

    return result;
}

QDBusMessage qDBusPropertyGetAll(const QDBusConnectionPrivate::ObjectTreeNode &node,
                                 const QDBusMessage &msg)
{
    Q_ASSERT(msg.arguments().count() == 1);
    Q_ASSERT_X(!node.obj || QThread::currentThread() == node.obj->thread(),
               "QDBusConnection: internal threading error",
               "function called for an object that is in another thread!!");

    QString interface_name = msg.arguments().at(0).toString();

    bool interfaceFound = false;
    QVariantMap result;

    QDBusAdaptorConnector *connector;
    if (node.flags & QDBusConnection::ExportAdaptors &&
        (connector = qDBusFindAdaptorConnector(node.obj))) {

        if (interface_name.isEmpty()) {
            // iterate over all interfaces
            for (QDBusAdaptorConnector::AdaptorMap::ConstIterator it = connector->adaptors.constBegin(),
                 end = connector->adaptors.constEnd(); it != end; ++it) {
                result += readAllProperties(it->adaptor, QDBusConnection::ExportAllProperties);
            }
        } else {
            // find the class that implements interface_name
            QDBusAdaptorConnector::AdaptorMap::ConstIterator it;
            it = qLowerBound(connector->adaptors.constBegin(), connector->adaptors.constEnd(),
                             interface_name);
            if (it != connector->adaptors.constEnd() && interface_name == QLatin1String(it->interface)) {
                interfaceFound = true;
                result = readAllProperties(it->adaptor, QDBusConnection::ExportAllProperties);
            }
        }
    }

    if (node.flags & QDBusConnection::ExportAllProperties &&
        (!interfaceFound || interface_name.isEmpty())) {
        // try the object itself
        result += readAllProperties(node.obj, node.flags);
        interfaceFound = true;
    }

    if (!interfaceFound && !interface_name.isEmpty()) {
        // the interface was not found
        return qDBusPropertyError(msg, interface_name);
    }

    return msg.createReply(qVariantFromValue(result));
}

QT_END_NAMESPACE
