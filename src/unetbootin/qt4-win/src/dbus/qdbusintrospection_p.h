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

#ifndef QDBUSINTROSPECTION_H
#define QDBUSINTROSPECTION_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists for the convenience
// of the QLibrary class.  This header file may change from
// version to version without notice, or even be removed.
//
// We mean it.
//

#include <QtCore/qstring.h>
#include <QtCore/qlist.h>
#include <QtCore/qstringlist.h>
#include <QtCore/qmap.h>
#include <QtCore/qpair.h>
#include <QtCore/qshareddata.h>
#include <qdbusmacros.h>

QT_BEGIN_NAMESPACE

class QDBUS_EXPORT QDBusIntrospection
{
public:
    // forward declarations
    struct Argument;
    struct Method;
    struct Signal;
    struct Property;
    struct Interface;
    struct Object;
    struct ObjectTree;

    // typedefs
    typedef QMap<QString, QString> Annotations;
    typedef QList<Argument> Arguments;
    typedef QMultiMap<QString, Method> Methods;
    typedef QMultiMap<QString, Signal> Signals;
    typedef QMap<QString, Property> Properties;
    typedef QMap<QString, QSharedDataPointer<Interface> > Interfaces;
    typedef QMap<QString, QSharedDataPointer<ObjectTree> > Objects;

public:
    // the structs

    struct Argument
    {
        QString type;
        QString name;

        inline bool operator==(const Argument& other) const
        { return name == other.name && type == other.type; }
    };
    
    struct Method
    {
        QString name;
        Arguments inputArgs;
        Arguments outputArgs;
        Annotations annotations;

        inline bool operator==(const Method& other) const
        { return name == other.name && annotations == other.annotations &&
                inputArgs == other.inputArgs && outputArgs == other.outputArgs; }
    };

    struct Signal
    {
        QString name;
        Arguments outputArgs;
        Annotations annotations;

        inline bool operator==(const Signal& other) const
        { return name == other.name && annotations == other.annotations &&
                outputArgs == other.outputArgs; }
    };

    struct Property
    {
        enum Access { Read, Write, ReadWrite };
        QString name;
        QString type;
        Access access;
        Annotations annotations;

        inline bool operator==(const Property& other) const
        { return access == other.access && name == other.name &&
                annotations == other.annotations && type == other.type; }
    };

    struct Interface: public QSharedData
    {
        QString name;
        QString introspection;

        Annotations annotations;
        Methods methods;
        Signals signals_;
        Properties properties;

        inline bool operator==(const Interface &other) const
        { return !name.isEmpty() && name == other.name; }
    };

    struct Object: public QSharedData
    {
        QString service;
        QString path;
        QString introspection;

        QStringList interfaces;
        QStringList childObjects;
    };

    struct ObjectTree: public Object
    {
        Interfaces interfaceData;
        Objects childObjectData;
    };

public:
    static Interface parseInterface(const QString &xml);
    static Interfaces parseInterfaces(const QString &xml);
    static Object parseObject(const QString &xml, const QString &service = QString(),
                              const QString &path = QString());
    static ObjectTree parseObjectTree(const QString &xml,
                                      const QString &service,
                                      const QString &path);

private:
    QDBusIntrospection();
};

QT_END_NAMESPACE

#endif
