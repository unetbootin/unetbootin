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

#include "qboolean_p.h"
#include "qcommonsequencetypes_p.h"
#include "qdynamiccontextstore_p.h"
#include "qliteral_p.h"

#include "qletclause_p.h"

QT_BEGIN_NAMESPACE

using namespace QPatternist;

LetClause::LetClause(const Expression::Ptr &operand1,
                     const Expression::Ptr &operand2,
                     const VariableDeclaration::Ptr &decl) : PairContainer(operand1, operand2)
                                                           , m_varDecl(decl)
{
    Q_ASSERT(m_varDecl);
}

DynamicContext::Ptr LetClause::bindVariable(const DynamicContext::Ptr &context) const
{
    context->setExpressionVariable(m_varDecl->slot, Expression::Ptr(new DynamicContextStore(m_operand1, context)));
    return context;
}

Item::Iterator::Ptr LetClause::evaluateSequence(const DynamicContext::Ptr &context) const
{
    return m_operand2->evaluateSequence(bindVariable(context));
}

Item LetClause::evaluateSingleton(const DynamicContext::Ptr &context) const
{
    return m_operand2->evaluateSingleton(bindVariable(context));
}

bool LetClause::evaluateEBV(const DynamicContext::Ptr &context) const
{
    return m_operand2->evaluateEBV(bindVariable(context));
}

void LetClause::evaluateToSequenceReceiver(const DynamicContext::Ptr &context) const
{
    m_operand2->evaluateToSequenceReceiver(bindVariable(context));
}

Expression::Ptr LetClause::typeCheck(const StaticContext::Ptr &context,
                                     const SequenceType::Ptr &reqType)
{
    /* Consider the following query:
     *
     * <tt>let $d := \<child type=""/>
     * return $d//\*[let $i := @type
     *              return $d//\*[$i]]</tt>
     *
     * The node test <tt>@type</tt> is referenced from two different places,
     * where each reference have a different focus. So, in the case of that the source
     * uses the focus, we need to use a DynamicContextStore to ensure the variable
     * is always evaluated with the correct focus, regardless of where it is referenced
     * from.
     *
     * We miss out a lot of false positives. For instance, the case of where the focus
     * is identical for everyone. One reason we cannot check this, is that Expression
     * doesn't know about its parent.
     */
    m_varDecl->canSourceRewrite = !m_operand1->deepProperties().testFlag(RequiresFocus);

    if(m_varDecl->canSourceRewrite)
        return m_operand2->typeCheck(context, reqType);
    else
        return PairContainer::typeCheck(context, reqType);
}

Expression::Properties LetClause::properties() const
{
    return m_varDecl->expression()->properties() & (Expression::RequiresFocus | Expression::IsEvaluated | Expression::DisableElimination);
}

SequenceType::List LetClause::expectedOperandTypes() const
{
    SequenceType::List result;
    result.append(CommonSequenceTypes::ZeroOrMoreItems);
    result.append(CommonSequenceTypes::ZeroOrMoreItems);
    return result;
}

SequenceType::Ptr LetClause::staticType() const
{
    return m_operand2->staticType();
}

ExpressionVisitorResult::Ptr LetClause::accept(const ExpressionVisitor::Ptr &visitor) const
{
    return visitor->visit(this);
}

Expression::ID LetClause::id() const
{
    return IDLetClause;
}

QT_END_NAMESPACE
