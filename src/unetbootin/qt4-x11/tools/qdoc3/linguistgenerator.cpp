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

/*
  linguistgenerator.cpp
*/

#include "codemarker.h"
#include "pagegenerator.h"
#include "linguistgenerator.h"
#include "node.h"
#include "separator.h"
#include "tree.h"
#include <ctype.h>

#include <qlist.h>
#include <qiterator.h>

QT_BEGIN_NAMESPACE

#define COMMAND_VERSION                 Doc::alias("version")

LinguistGenerator::LinguistGenerator()
    : PageGenerator()
{
}

LinguistGenerator::~LinguistGenerator()
{
}

void LinguistGenerator::initializeGenerator(const Config &config)
{
    Generator::initializeGenerator(config);
}

void LinguistGenerator::terminateGenerator()
{
    PageGenerator::terminateGenerator();
}

QString LinguistGenerator::format()
{
    return "Linguist";
}

QString LinguistGenerator::fileExtension(const Node * /* node */)
{
    return "ts";
}

void LinguistGenerator::generateClassLikeNode(const InnerNode *inner, CodeMarker *marker)
{
    out().setCodec("utf-8");

    QDomDocument document("TS");
    QDomElement documentElement = document.createElement("TS");
    documentElement.setAttribute("version", "1.1");

    QList<QDomElement> contextElements = generateIndexSections(document, inner, marker);
    foreach (const QDomElement &element, contextElements)
        documentElement.appendChild(element);

    QDomProcessingInstruction process = document.createProcessingInstruction(
        "xml", QString("version=\"1.0\" encoding=\"%1\"").arg("utf-8"));
    document.appendChild(process);
    document.appendChild(documentElement);

    out() << document;
    out().flush();
}

void LinguistGenerator::generateFakeNode( const FakeNode *fake, CodeMarker *marker )
{
    out().setCodec("utf-8");

    QDomDocument document("TS");
    QDomElement documentElement = document.createElement("TS");
    documentElement.setAttribute("version", "1.1");

    QList<QDomElement> contextElements = generateIndexSections(document, fake, marker);
    foreach (const QDomElement &element, contextElements)
        documentElement.appendChild(element);

    QDomProcessingInstruction process = document.createProcessingInstruction(
        "xml", QString("version=\"1.0\" encoding=\"%1\"").arg("utf-8"));
    document.appendChild(process);
    document.appendChild(documentElement);

    out() << document;
    out().flush();
}

QList<QDomElement> LinguistGenerator::generateIndexSections(
                   QDomDocument &document, const Node *node, CodeMarker *marker)
{
    QList<QDomElement> contexts;

    if (node->isInnerNode()) {
        const InnerNode *inner = static_cast<const InnerNode *>(node);

        foreach (const Node *child, inner->childNodes()) {
            // Recurse to generate a DOM element for this child node and all
            // its children.
            contexts += generateIndexSections(document, child, marker);
        }
/*
        foreach (const Node *child, inner->relatedNodes()) {
            QDomElement childElement = generateIndexSections(document, child, marker);
            element.appendChild(childElement);
        }
*/
    }

    // Add documentation to this node if it exists.
    if (!node->doc().isEmpty()) {

        QString nodeName = fullName(node);
        QString signature;

        if (node->type() == Node::Function) {
            QStringList pieces;
            const FunctionNode *functionNode = static_cast<const FunctionNode*>(node);
            foreach (const Parameter &parameter, functionNode->parameters()) {
                QString typeString = parameter.leftType() + parameter.rightType();
                if (typeString.split(" ").size() > 1)
                    pieces.append(typeString + parameter.name());
                else
                    pieces.append(typeString + " " + parameter.name());
            }
            signature = "(" + pieces.join(", ") + ")";
        }

        QDomElement contextElement = document.createElement("context");
        QDomElement nameElement = document.createElement("name");
        nameElement.appendChild(document.createTextNode(nodeName + signature));
        contextElement.appendChild(nameElement);

        QDomElement messageElement = document.createElement("message");
        contextElement.appendChild(messageElement);

        QDomElement sourceElement = document.createElement("source");
        QString sourceText = simplified(node->doc().source());
        if (!signature.isEmpty() && signature != "()" && !sourceText.contains("\\fn"))
            sourceText.prepend(QString("\\fn %1%2\n").arg(nodeName).arg(signature));
        sourceElement.appendChild(document.createTextNode(sourceText));
        messageElement.appendChild(sourceElement);

        QDomElement translationElement = document.createElement("translation");
        translationElement.setAttribute("type", "unfinished");
        messageElement.appendChild(translationElement);

        QDomElement locationElement = document.createElement("location");
        locationElement.setAttribute("filename", node->doc().location().filePath());
        locationElement.setAttribute("line", node->doc().location().lineNo());
        messageElement.appendChild(locationElement);

        contexts.append(contextElement);
    }

    return contexts;
}

QString LinguistGenerator::fullName(const Node *node) const
{
    if (!node)
        return "";
    else if (node->parent() && !node->parent()->name().isEmpty())
        return fullName(node->parent()) + "::" + node->name();
    else
        return node->name();
}

QString LinguistGenerator::simplified(const QString &text) const
{
    QStringList lines = text.split("\n");

    while (lines.size() > 0 && lines.first().trimmed().isEmpty())
        lines.pop_front();

    while (lines.size() > 0 && lines.last().trimmed().isEmpty())
        lines.pop_back();

    int min = 0;
    bool set = false;
    foreach (const QString &line, lines) {
        int j = 0;
        while (j < line.length()) {
            if (line[j] != ' ')
                break;
            ++j;
        }
        if (j < line.length()) {
            if (!set) {
                min = j;
                set = true;
            } else
                min = qMin(min, j);
        }
    }
    for (int i = 0; i < lines.size(); ++i)
        lines[i] = lines[i].mid(min);

    return lines.join("\n");
}

QT_END_NAMESPACE
