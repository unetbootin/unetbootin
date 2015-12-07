/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the QtXMLPatterns module of the Qt Toolkit.
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

#include <QtGlobal>

#include "qexpression_p.h"
#include "qstaticcontext_p.h"
#include "qtokenizer_p.h"

#include "qparsercontext_p.h"

QT_BEGIN_NAMESPACE

using namespace QPatternist;

ParserContext::ParserContext(const StaticContext::Ptr &context,
                             const QXmlQuery::QueryLanguage lang,
                             Tokenizer *const tokener) : staticContext(context)
                                                       , tokenizer(tokener)
                                                       , languageAccent(lang)
                                                       , nodeTestSource(BuiltinTypes::element)
                                                       , moduleNamespace(StandardNamespaces::empty)
                                                       , isPreviousEnclosedExpr(false)
                                                       , elementConstructorDepth(0)
                                                       , hasSecondPrologPart(false)
                                                       , preserveNamespacesMode(true)
                                                       , inheritNamespacesMode(true)
                                                       , isParsingPattern(false)
                                                       , currentImportPrecedence(1)
                                                       , m_evaluationCacheSlot(-1)
                                                       , m_expressionSlot(0)
                                                       , m_positionSlot(-1)
                                                       , m_globalVariableSlot(-1)
                                                       , m_currentTemplateID(InitialTemplateID)
{
    resolvers.push(context->namespaceBindings());
    Q_ASSERT(tokenizer);
    Q_ASSERT(context);
    m_isParsingWithParam.push(false);
    isBackwardsCompat.push(false);
}

void ParserContext::finalizePushedVariable(const int amount,
                                           const bool shouldPop)
{
    for(int i = 0; i < amount; ++i)
    {
        const VariableDeclaration::Ptr var(shouldPop ? variables.pop() : variables.top());
        Q_ASSERT(var);

        if(var->isUsed())
            continue;
        else
        {
            staticContext->warning(QtXmlPatterns::tr("The variable %1 is unused")
                                                     .arg(formatKeyword(var, staticContext->namePool())));
        }
    }
}

QT_END_NAMESPACE

