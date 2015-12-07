/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the qt3to4 porting application of the Qt Toolkit.
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
