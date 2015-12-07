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
    jambiapiparser.h
*/

#ifndef JAMBIAPIPARSER_H
#define JAMBIAPIPARSER_H

#include <QStack>
#include <QXmlDefaultHandler>

#include "codeparser.h"

QT_BEGIN_NAMESPACE

struct ClassOrEnumInfo
{
    QString tag;
    QString javaName;
    QString cppName;
    Node *javaNode;
    Node *cppNode;

    ClassOrEnumInfo() : javaNode(0), cppNode(0) {}
};

class JambiApiParser : public CodeParser, private QXmlDefaultHandler
{
public:
    JambiApiParser(Tree *cppTree);
    ~JambiApiParser();

    void initializeParser(const Config &config);
    void terminateParser();
    QString language();
    QString sourceFileNameFilter();
    void parseSourceFile(const Location &location, const QString &filePath, Tree *tree);
    virtual void doneParsingSourceFiles(Tree *tree);

private:
    bool startElement(const QString &namespaceURI, const QString &localName,
                      const QString &qName, const QXmlAttributes &attributes);
    bool endElement(const QString &namespaceURI, const QString &localName,
                    const QString &qName);
    bool fatalError(const QXmlParseException &exception);
    void jambifyDocsPass2(Node *node);
    bool makeFunctionNode(InnerNode *parent, const QString &synopsis, FunctionNode **funcPtr);

    Tree *cppTre;
    Tree *javaTre;

    bool metJapiTag;
    Location japiLocation;
    QStack<ClassOrEnumInfo> classAndEnumStack;
};

QT_END_NAMESPACE

#endif
