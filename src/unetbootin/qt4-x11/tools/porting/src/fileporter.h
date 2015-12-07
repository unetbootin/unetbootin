/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the qt3to4 porting application of the Qt Toolkit.
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

#ifndef FILEPORTER_H
#define FILEPORTER_H

#include "portingrules.h"
#include "replacetoken.h"
#include "filewriter.h"
#include "preprocessorcontrol.h"
#include <QString>
#include <QSet>
#include <QMap>

QT_BEGIN_NAMESPACE

class FilePorter
{
public:
    FilePorter(PreprocessorCache &preprocessorCache);
    void port(QString fileName);
    QSet<QByteArray> usedQtModules();
private:
    QByteArray loadFile(const QString &fileName);
    QByteArray includeAnalyse(QByteArray fileContents);
    TextReplacements includeDirectiveReplacements();

    PreprocessorCache &preprocessorCache;
    const QList<TokenReplacement*> tokenReplacementRules;
    const QHash<QByteArray, QByteArray> headerReplacements;

    ReplaceToken replaceToken;
    Tokenizer tokenizer;    //used by includeAnalyse

    QSet<QByteArray> qt4HeaderNames;
    QSet<QByteArray> m_usedQtModules;
};

class PreprocessReplace : public Rpp::RppTreeWalker
{
public:
    PreprocessReplace(const Rpp::Source *source, const QHash<QByteArray, QByteArray> &headerReplacements);
    TextReplacements getReplacements();
private:
    void evaluateIncludeDirective(const Rpp::IncludeDirective *directive);
    void evaluateText(const Rpp::Text *textLine);
    const QHash<QByteArray, QByteArray> headerReplacements;
    TextReplacements replacements;
    
};

class IncludeDirectiveAnalyzer : public Rpp::RppTreeWalker
{
public:
    IncludeDirectiveAnalyzer(const TokenEngine::TokenContainer &fileContents);
    int insertPos();
    QSet<QByteArray> includedHeaders();
    QSet<QByteArray> usedClasses();
private:
    void evaluateIncludeDirective(const Rpp::IncludeDirective *directive);
    void evaluateIfSection(const Rpp::IfSection *ifSection);
    void evaluateText(const Rpp::Text *textLine);

    int insertTokenIndex;
    bool foundInsertPos;
    bool foundQtHeader;
    int ifSectionCount;

    const TokenEngine::TokenContainer &fileContents;
    Rpp::Source *source;
    TypedPool<Rpp::Item> mempool;
    QSet<QByteArray> m_includedHeaders;
    QSet<QByteArray> m_usedClasses;
};

QT_END_NAMESPACE

#endif
