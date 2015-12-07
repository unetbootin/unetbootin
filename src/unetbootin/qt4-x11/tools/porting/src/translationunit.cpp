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
