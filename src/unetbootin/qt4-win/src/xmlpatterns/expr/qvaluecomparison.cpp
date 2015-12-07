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

    if(me.data() != this)
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
