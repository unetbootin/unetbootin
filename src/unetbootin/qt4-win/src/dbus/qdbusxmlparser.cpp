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

#include "qdbusxmlparser_p.h"
#include "qdbusinterface.h"
#include "qdbusinterface_p.h"
#include "qdbusconnection_p.h"
#include "qdbusutil_p.h"

#include <QtXml/qdom.h>
#include <QtCore/qmap.h>
#include <QtCore/qvariant.h>
#include <QtCore/qtextstream.h>

QT_BEGIN_NAMESPACE

static QDBusIntrospection::Annotations
parseAnnotations(const QDomElement& elem)
{
    QDBusIntrospection::Annotations retval;
    QDomNodeList list = elem.elementsByTagName(QLatin1String("annotation"));
    for (int i = 0; i < list.count(); ++i)
    {
        QDomElement ann = list.item(i).toElement();
        if (ann.isNull())
            continue;

        QString name = ann.attribute(QLatin1String("name")),
               value = ann.attribute(QLatin1String("value"));

        if (!QDBusUtil::isValidInterfaceName(name)) {
            qWarning("Invalid D-BUS annotation '%s' found while parsing introspection",
                     qPrintable(name));
            continue;
        }

        retval.insert(name, value);
    }

    return retval;
}

static QDBusIntrospection::Arguments
parseArgs(const QDomElement& elem, const QLatin1String& direction, bool acceptEmpty)
{
    QDBusIntrospection::Arguments retval;
    QDomNodeList list = elem.elementsByTagName(QLatin1String("arg"));
    for (int i = 0; i < list.count(); ++i)
    {
        QDomElement arg = list.item(i).toElement();
        if (arg.isNull())
            continue;

        if ((acceptEmpty && !arg.hasAttribute(QLatin1String("direction"))) ||
            arg.attribute(QLatin1String("direction")) == direction) {

            QDBusIntrospection::Argument argData;
            if (arg.hasAttribute(QLatin1String("name")))
                argData.name = arg.attribute(QLatin1String("name")); // can be empty
            argData.type = arg.attribute(QLatin1String("type"));
            if (!QDBusUtil::isValidSingleSignature(argData.type)) {
                qWarning("Invalid D-BUS type signature '%s' found while parsing introspection",
                         qPrintable(argData.type));
                continue;
            }

            retval << argData;
        }
    }
    return retval;
}

QDBusXmlParser::QDBusXmlParser(const QString& service, const QString& path,
                               const QString& xmlData)
    : m_service(service), m_path(path)
{
    QDomDocument doc;
    doc.setContent(xmlData);
    m_node = doc.firstChildElement(QLatin1String("node"));
}

QDBusXmlParser::QDBusXmlParser(const QString& service, const QString& path,
                               const QDomElement& node)
    : m_service(service), m_path(path), m_node(node)
{
}

QDBusIntrospection::Interfaces
QDBusXmlParser::interfaces() const
{
    QDBusIntrospection::Interfaces retval;

    if (m_node.isNull())
        return retval;

    QDomNodeList interfaceList = m_node.elementsByTagName(QLatin1String("interface"));
    for (int i = 0; i < interfaceList.count(); ++i)
    {
        QDomElement iface = interfaceList.item(i).toElement();
        QString ifaceName = iface.attribute(QLatin1String("name"));
        if (iface.isNull())
            continue;           // for whatever reason
        if (!QDBusUtil::isValidInterfaceName(ifaceName)) {
            qWarning("Invalid D-BUS interface name '%s' found while parsing introspection",
                     qPrintable(ifaceName));
            continue;
        }

        QDBusIntrospection::Interface *ifaceData = new QDBusIntrospection::Interface;
        ifaceData->name = ifaceName;
        {
            // save the data
            QTextStream ts(&ifaceData->introspection);
            iface.save(ts,2);
        }

        // parse annotations
        ifaceData->annotations = parseAnnotations(iface);

        // parse methods
        QDomNodeList list = iface.elementsByTagName(QLatin1String("method"));
        for (int j = 0; j < list.count(); ++j)
        {
            QDomElement method = list.item(j).toElement();
            QString methodName = method.attribute(QLatin1String("name"));
            if (method.isNull())
                continue;
            if (!QDBusUtil::isValidMemberName(methodName)) {
                qWarning("Invalid D-BUS member name '%s' found in interface '%s' while parsing introspection",
                         qPrintable(methodName), qPrintable(ifaceName));
                continue;
            }

            QDBusIntrospection::Method methodData;
            methodData.name = methodName;

            // parse arguments
            methodData.inputArgs = parseArgs(method, QLatin1String("in"), true);
            methodData.outputArgs = parseArgs(method, QLatin1String("out"), false);
            methodData.annotations = parseAnnotations(method);

            // add it
            ifaceData->methods.insert(methodName, methodData);
        }

        // parse signals
        list = iface.elementsByTagName(QLatin1String("signal"));
        for (int j = 0; j < list.count(); ++j)
        {
            QDomElement signal = list.item(j).toElement();
            QString signalName = signal.attribute(QLatin1String("name"));
            if (signal.isNull())
                continue;
            if (!QDBusUtil::isValidMemberName(signalName)) {
                qWarning("Invalid D-BUS member name '%s' found in interface '%s' while parsing introspection",
                         qPrintable(signalName), qPrintable(ifaceName));
                continue;
            }

            QDBusIntrospection::Signal signalData;
            signalData.name = signalName;

            // parse data
            signalData.outputArgs = parseArgs(signal, QLatin1String("out"), true);
            signalData.annotations = parseAnnotations(signal);

            // add it
            ifaceData->signals_.insert(signalName, signalData);
        }

        // parse properties
        list = iface.elementsByTagName(QLatin1String("property"));
        for (int j = 0; j < list.count(); ++j)
        {
            QDomElement property = list.item(j).toElement();
            QString propertyName = property.attribute(QLatin1String("name"));
            if (property.isNull())
                continue;
            if (!QDBusUtil::isValidMemberName(propertyName)) {
                qWarning("Invalid D-BUS member name '%s' found in interface '%s' while parsing introspection",
                         qPrintable(propertyName), qPrintable(ifaceName));
                continue;
            }

            QDBusIntrospection::Property propertyData;

            // parse data
            propertyData.name = propertyName;
            propertyData.type = property.attribute(QLatin1String("type"));
            propertyData.annotations = parseAnnotations(property);

            if (!QDBusUtil::isValidSingleSignature(propertyData.type)) {
                // cannot be!
                qWarning("Invalid D-BUS type signature '%s' found in property '%s.%s' while parsing introspection",
                         qPrintable(propertyData.type), qPrintable(ifaceName),
                         qPrintable(propertyName));
                continue;
            }

            QString access = property.attribute(QLatin1String("access"));
            if (access == QLatin1String("read"))
                propertyData.access = QDBusIntrospection::Property::Read;
            else if (access == QLatin1String("write"))
                propertyData.access = QDBusIntrospection::Property::Write;
            else if (access == QLatin1String("readwrite"))
                propertyData.access = QDBusIntrospection::Property::ReadWrite;
            else {
                qWarning("Invalid D-BUS property access '%s' found in property '%s.%s' while parsing introspection",
                         qPrintable(access), qPrintable(ifaceName),
                         qPrintable(propertyName));
                continue;       // invalid one!
            }

            // add it
            ifaceData->properties.insert(propertyName, propertyData);
        }

        // add it
        retval.insert(ifaceName, QSharedDataPointer<QDBusIntrospection::Interface>(ifaceData));
    }

    return retval;
}

QSharedDataPointer<QDBusIntrospection::Object>
QDBusXmlParser::object() const
{
    if (m_node.isNull())
        return QSharedDataPointer<QDBusIntrospection::Object>();

    QDBusIntrospection::Object* objData;
    objData = new QDBusIntrospection::Object;
    objData->service = m_service;
    objData->path = m_path;

    // check if we have anything to process
    if (objData->introspection.isNull() && !m_node.firstChild().isNull()) {
        // yes, introspect this object
        QTextStream ts(&objData->introspection);
        m_node.save(ts,2);

        QDomNodeList objects = m_node.elementsByTagName(QLatin1String("node"));
        for (int i = 0; i < objects.count(); ++i) {
            QDomElement obj = objects.item(i).toElement();
            QString objName = obj.attribute(QLatin1String("name"));
            if (obj.isNull())
                continue;           // for whatever reason
            if (!QDBusUtil::isValidObjectPath(m_path + QLatin1Char('/') + objName)) {
                qWarning("Invalid D-BUS object path '%s/%s' found while parsing introspection",
                         qPrintable(m_path), qPrintable(objName));
                continue;
            }

            objData->childObjects.append(objName);
        }

        QDomNodeList interfaceList = m_node.elementsByTagName(QLatin1String("interface"));
        for (int i = 0; i < interfaceList.count(); ++i) {
            QDomElement iface = interfaceList.item(i).toElement();
            QString ifaceName = iface.attribute(QLatin1String("name"));
            if (iface.isNull())
                continue;
            if (!QDBusUtil::isValidInterfaceName(ifaceName)) {
                qWarning("Invalid D-BUS interface name '%s' found while parsing introspection",
                         qPrintable(ifaceName));
                continue;
            }

            objData->interfaces.append(ifaceName);
        }
    } else {
        objData->introspection = QLatin1String("<node/>\n");
    }

    QSharedDataPointer<QDBusIntrospection::Object> retval;
    retval = objData;
    return retval;
}

QSharedDataPointer<QDBusIntrospection::ObjectTree>
QDBusXmlParser::objectTree() const
{
    QSharedDataPointer<QDBusIntrospection::ObjectTree> retval;

    if (m_node.isNull())
        return retval;

    retval = new QDBusIntrospection::ObjectTree;

    retval->service = m_service;
    retval->path = m_path;

    QTextStream ts(&retval->introspection);
    m_node.save(ts,2);

    // interfaces are easy:
    retval->interfaceData = interfaces();
    retval->interfaces = retval->interfaceData.keys();

    // sub-objects are slightly more difficult:
    QDomNodeList objects = m_node.elementsByTagName(QLatin1String("node"));
    for (int i = 0; i < objects.count(); ++i) {
        QDomElement obj = objects.item(i).toElement();
        QString objName = obj.attribute(QLatin1String("name"));
        if (obj.isNull() || objName.isEmpty())
            continue;           // for whatever reason

        // check if we have anything to process
        if (!obj.firstChild().isNull()) {
            // yes, introspect this object
            QString xml;
            QTextStream ts2(&xml);
            obj.save(ts2,0);

            // parse it
            QString objAbsName = m_path;
            if (!objAbsName.endsWith(QLatin1Char('/')))
                objAbsName.append(QLatin1Char('/'));
            objAbsName += objName;

            QDBusXmlParser parser(m_service, objAbsName, obj);
            retval->childObjectData.insert(objName, parser.objectTree());
        }

        retval->childObjects << objName;
    }

    return QSharedDataPointer<QDBusIntrospection::ObjectTree>( retval );
}

QT_END_NAMESPACE
