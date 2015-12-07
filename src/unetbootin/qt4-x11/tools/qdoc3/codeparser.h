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
  codeparser.h
*/

#ifndef CODEPARSER_H
#define CODEPARSER_H

#include <QSet>

#include "location.h"

QT_BEGIN_NAMESPACE

class Config;
class Node;
class QString;
class Tree;

class CodeParser
{
public:
    CodeParser();
    virtual ~CodeParser();

    virtual void initializeParser( const Config& config );
    virtual void terminateParser();
    virtual QString language() = 0;
    virtual QString headerFileNameFilter();
    virtual QString sourceFileNameFilter() = 0;
    virtual void parseHeaderFile( const Location& location,
				  const QString& filePath, Tree *tree );
    virtual void parseSourceFile( const Location& location,
				  const QString& filePath, Tree *tree ) = 0;
    virtual void doneParsingHeaderFiles( Tree *tree );
    virtual void doneParsingSourceFiles( Tree *tree ) = 0;

    static void initialize( const Config& config );
    static void terminate();
    static CodeParser *parserForLanguage( const QString& language );

protected:
    QSet<QString> commonMetaCommands();
    void processCommonMetaCommand(const Location& location,
				  const QString& command, const QString& arg,
				  Node *node, Tree *tree);

private:
    static QList<CodeParser *> parsers;
};

QT_END_NAMESPACE

#endif
