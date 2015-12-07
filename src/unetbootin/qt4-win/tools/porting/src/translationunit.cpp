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

#include "translationunit.h"

QT_BEGIN_NAMESPACE

using namespace TokenEngine;
using namespace CodeModel;
using namespace TokenStreamAdapter;

TranslationUnit::TranslationUnit()
{
    TokenSectionSequence empty;
    d = new TranslationUnitData(empty);
}

TranslationUnit::TranslationUnit(const TokenEngine::TokenSectionSequence &tokens)
{ d = new TranslationUnitData(tokens); }

TokenSectionSequence TranslationUnit::tokens() const
{ return d->tokens; }

void TranslationUnit::setCodeModel(NamespaceScope *globalScope)
{ d->globalScope = globalScope; }

NamespaceScope *TranslationUnit::codeModel()
{ return d->globalScope; }

TypedPool<CodeModel::Item> *TranslationUnit::codeModelMemoryPool()
{ return &d->codeModelMemoryPool; }



/*
    Performs C++ parsing and semantic analysis on a translation unit.
    Returns a TranslationUnit, which contains all the data.
*/
TranslationUnit TranslationUnitAnalyzer::analyze
        (const TokenSectionSequence &translationUnitTokens, int targetMaxASTNodes)
{
    TranslationUnit translationUnit(translationUnitTokens);
    CodeModel::NamespaceScope *codeModel =
        CodeModel::Create<CodeModel::NamespaceScope>(translationUnit.codeModelMemoryPool());
    translationUnit.setCodeModel(codeModel);

    // run lexical analysis
    QVector<QT_PREPEND_NAMESPACE(Type)> typeList = lexer.lex(translationUnitTokens);
    TokenStreamAdapter::TokenStream tokenStream(translationUnitTokens, typeList);

    Semantic semantic(codeModel, &tokenStream, translationUnit.codeModelMemoryPool());

    // parse and run semantic on the translation unit
    bool done = false;
    while(!done) {
        pool p;
        TranslationUnitAST *node = parser.parse(&tokenStream, &p, targetMaxASTNodes, done);
        semantic.parseTranslationUnit(node);
    }

    return translationUnit;
}

QT_END_NAMESPACE
