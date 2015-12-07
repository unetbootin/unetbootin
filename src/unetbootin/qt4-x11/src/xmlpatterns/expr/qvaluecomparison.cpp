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

#include "qitem_p.h"
#include "qboolean_p.h"
#include "qbuiltintypes_p.h"
#include "qcommonsequencetypes_p.h"
#include "qemptysequence_p.h"
#include "qoptimizationpasses_p.h"

#include "qvaluecomparison_p.h"

QT_BEGIN_NAMESPACE

using namespace QPatternist;

ValueComparison::ValueComparison(const Expression::Ptr &op1,
                                 const AtomicComparator::Operator op,
                                 const Expression::Ptr &op2) : PairContainer(op1, op2),
                                                               m_operator(op)
{
}

Item ValueComparison::evaluateSingleton(const DynamicContext::Ptr &context) const
{
    const Item it1(m_operand1->evaluateSingleton(context));
    if(!it1)
        return Item();

    const Item it2(m_operand2->evaluateSingleton(context));
    if(!it2)
        return Item();

    return Boolean::fromValue(flexibleCompare(it1, it2, context));
}

Expression::Ptr ValueComparison::typeCheck(const StaticContext::Ptr &context,
                                           const SequenceType::Ptr &reqType)
{
    const Expression::Ptr me(PairContainer::typeCheck(context, reqType));
    const ItemType::Ptr t1(m_operand1->staticType()->itemType());
    const ItemType::Ptr t2(m_operand2->staticType()->itemType());
    Q_ASSERT(t1);
    Q_ASSERT(t2);

    if(*CommonSequenceTypes::Empty == *t1 ||
       *CommonSequenceTypes::Empty == *t2)
    {
        return EmptySequence::create(this, context);
    }
    else
    {
        prepareComparison(fetchComparator(t1, t2, context));

        return me;
    }
}

Expression::Ptr ValueComparison::compress(const StaticContext::Ptr &context)
{
    const Expression::Ptr me(PairContainer::compress(context));

    if(me != this)
        return me;

    if(isCaseInsensitiveCompare(m_operand1, m_operand2))
        useCaseInsensitiveComparator();

    return me;
}

bool ValueComparison::isCaseInsensitiveCompare(Expression::Ptr &op1, Expression::Ptr &op2)
{
    Q_ASSERT(op1);
    Q_ASSERT(op2);

    const ID iD = op1->id();

    if((iD == IDLowerCaseFN || iD == IDUpperCaseFN) && iD == op2->id())
    {
        /* Both are either fn:lower-case() or fn:upper-case(). */

        /* Replace the calls to the functions with its operands. */
        op1 = op1->operands().first();
        op2 = op2->operands().first();

        return true;
    }
    else
        return false;
}

OptimizationPass::List ValueComparison::optimizationPasses() const
{
    return OptimizationPasses::comparisonPasses;
}

SequenceType::List ValueComparison::expectedOperandTypes() const
{
    SequenceType::List result;
    result.append(CommonSequenceTypes::ZeroOrOneAtomicType);
    result.append(CommonSequenceTypes::ZeroOrOneAtomicType);
    return result;
}

SequenceType::Ptr ValueComparison::staticType() const
{
    if(m_operand1->staticType()->cardinality().allowsEmpty() ||
       m_operand2->staticType()->cardinality().allowsEmpty())
        return CommonSequenceTypes::ZeroOrOneBoolean;
    else
        return CommonSequenceTypes::ExactlyOneBoolean;
}

ExpressionVisitorResult::Ptr ValueComparison::accept(const ExpressionVisitor::Ptr &visitor) const
{
    return visitor->visit(this);
}

Expression::ID ValueComparison::id() const
{
    return IDValueComparison;
}

QT_END_NAMESPACE
