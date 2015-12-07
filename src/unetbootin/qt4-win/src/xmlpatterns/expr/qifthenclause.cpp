/****************************************************************************
**
** Copyright (C) 2006-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the QtXMLPatterns module of the Qt Toolkit.
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

#include "qcommonsequencetypes_p.h"
#include "qgenericsequencetype_p.h"
#include "qoptimizationpasses_p.h"

#include "qifthenclause_p.h"

QT_BEGIN_NAMESPACE

using namespace QPatternist;

IfThenClause::IfThenClause(const Expression::Ptr &test,
                           const Expression::Ptr &then,
                           const Expression::Ptr &el) : TripleContainer(test, then, el)
{
}

Item::Iterator::Ptr IfThenClause::evaluateSequence(const DynamicContext::Ptr &context) const
{
    return m_operand1->evaluateEBV(context)
            ? m_operand2->evaluateSequence(context)
            : m_operand3->evaluateSequence(context);
}

Item IfThenClause::evaluateSingleton(const DynamicContext::Ptr &context) const
{
    return m_operand1->evaluateEBV(context)
            ? m_operand2->evaluateSingleton(context)
            : m_operand3->evaluateSingleton(context);
}

bool IfThenClause::evaluateEBV(const DynamicContext::Ptr &context) const
{
    return m_operand1->evaluateEBV(context)
            ? m_operand2->evaluateEBV(context)
            : m_operand3->evaluateEBV(context);
}

void IfThenClause::evaluateToSequenceReceiver(const DynamicContext::Ptr &context) const
{
    if(m_operand1->evaluateEBV(context))
        m_operand2->evaluateToSequenceReceiver(context);
    else
        m_operand3->evaluateToSequenceReceiver(context);
}

Expression::Ptr IfThenClause::compress(const StaticContext::Ptr &context)
{
    const Expression::Ptr me(TripleContainer::compress(context));

    if(me.data() != this)
        return me;

    /* All operands mustn't be evaluated in order for const folding to
     * be possible. Let's see how far we get. */

    if(m_operand1->isEvaluated())
    {
        if(m_operand1->evaluateEBV(context->dynamicContext()))
            return m_operand2;
        else
            return m_operand3;
    }
    else
        return me;
}

QList<QExplicitlySharedDataPointer<OptimizationPass> > IfThenClause::optimizationPasses() const
{
    return OptimizationPasses::ifThenPasses;
}

SequenceType::List IfThenClause::expectedOperandTypes() const
{
    SequenceType::List result;
    result.append(CommonSequenceTypes::EBV);
    result.append(CommonSequenceTypes::ZeroOrMoreItems);
    result.append(CommonSequenceTypes::ZeroOrMoreItems);
    return result;
}

SequenceType::Ptr IfThenClause::staticType() const
{
    const SequenceType::Ptr t1(m_operand2->staticType());
    const SequenceType::Ptr t2(m_operand3->staticType());

    return makeGenericSequenceType(t1->itemType() | t2->itemType(),
                                   t1->cardinality() | t2->cardinality());
}

ExpressionVisitorResult::Ptr IfThenClause::accept(const ExpressionVisitor::Ptr &visitor) const
{
    return visitor->visit(this);
}

Expression::ID IfThenClause::id() const
{
    return IDIfThenClause;
}

QT_END_NAMESPACE
