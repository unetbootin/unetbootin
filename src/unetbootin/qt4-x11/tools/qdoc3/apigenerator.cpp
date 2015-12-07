/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the tools applications of the Qt Toolkit.
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
            foreach (const RelatedClass &baseClass, classe->baseClasses()) {
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
            foreach (const EnumItem &item, enume->items())
                enumNames << item.name();
            qSort(enumNames);

            foreach (const QString &name, enumNames)
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
