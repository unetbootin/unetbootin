/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the tools applications of the Qt Toolkit.
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

#include <QFile>

#include "apigenerator.h"
#include "codemarker.h"
#include "tree.h"

QT_BEGIN_NAMESPACE

static QString indentStr(int indent)
{
    QString str;
    str.fill(QLatin1Char(' '), indent * 4);
    return str;
}

static bool lessThanName(Node *node1, Node *node2)
{
    return node1->name() < node2->name();
}

QString ApiGenerator::format()
{
    return QLatin1String("API");
}

void ApiGenerator::generateTree(const Tree *tree, CodeMarker *marker)
{
    QFile outFile(QLatin1String("api"));
    outFile.open(QIODevice::WriteOnly);

    out.setDevice(&outFile);
    generateNode(tree->root(), marker);
    out.flush();
}

void ApiGenerator::generateNode(const Node *node, CodeMarker *marker, int indent)
{
    if (node->access() == Node::Private)
        return;

    switch (node->type()) {
    case Node::Namespace:
        if (!node->name().isEmpty()) {
            out << indentStr(indent) << "Namespace: " << node->name() << "\n";
            ++indent;
        }
        break;
    case Node::Class:
        {
            const ClassNode *classe = static_cast<const ClassNode *>(node);
            out << indentStr(indent) << "Class: " << node->name();
            foreach (RelatedClass baseClass, classe->baseClasses()) {
                if (baseClass.access == Node::Public)
                    out << " inherits " << baseClass.dataTypeWithTemplateArgs;
            }
            out << "\n";
            ++indent;
        }
        break;
    case Node::Enum:
        {
            const EnumNode *enume = static_cast<const EnumNode *>(node);
            out << indentStr(indent) << "Enum: " << node->name() << "\n";
            ++indent;

            QStringList enumNames;
            foreach (EnumItem item, enume->items())
                enumNames << item.name();
            qSort(enumNames);

            foreach (QString name, enumNames)
                out << indentStr(indent) << "Enum value: " << name << "\n";
        }
        break;
    case Node::Typedef:
        out << indentStr(indent) << "Typedef: " << node->name() << "\n";
        ++indent;
        break;
    case Node::Function:
        {
            out << indentStr(indent) << "Function: "
                << plainCode(marker->markedUpSynopsis(node, 0, CodeMarker::Detailed)) << "\n";
            ++indent;
        }
        break;
    case Node::Property:
        {
            const PropertyNode *property = static_cast<const PropertyNode *>(node);
            out << indentStr(indent) << "Property: " << property->name()
                << " type " << property->dataType() << "\n";
            ++indent;
        }
        break;
    default:
        ;
    }

    if (node->isInnerNode()) {
        const InnerNode *inner = static_cast<const InnerNode *>(node);
        NodeList nodes = inner->childNodes();
        qSort(nodes.begin(), nodes.end(), lessThanName);
        foreach (const Node *child, nodes)
            generateNode(child, marker, indent);
    }

    out.flush();
}

QT_END_NAMESPACE
