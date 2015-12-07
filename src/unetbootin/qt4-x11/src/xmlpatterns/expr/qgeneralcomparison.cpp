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

#include "qbuiltintypes_p.h"
#include "qcommonsequencetypes_p.h"
#include "qcommonvalues_p.h"
#include "qebvextractor_p.h"
#include "qitem_p.h"
#include "qliteral_p.h"
#include "qoptimizationpasses_p.h"
#include "quntypedatomicconverter_p.h"
#include "qvaluecomparison_p.h"

#include "qgeneralcomparison_p.h"

QT_BEGIN_NAMESPACE

using namespace QPatternist;

GeneralComparison::GeneralComparison(const Expression::Ptr &op1,
                                     const AtomicComparator::Operator op,
                                     const Expression::Ptr &op2,
                                     const bool isBackwardsCompat) : PairContainer(op1, op2)
                                                                   , m_operator(op)
                                                                   , m_isBackwardsCompat(isBackwardsCompat)
{
}

bool GeneralComparison::generalCompare(const Item &op1,
                                       const Item &op2,
                                       const DynamicContext::Ptr &context) const
{
    Q_ASSERT(op1);
    Q_ASSERT(op2);

    if(comparator())
        return compare(op1, op2, comparator(), m_operator);

    Expression::Ptr a1(new Literal(op1));
    Expression::Ptr a2(new Literal(op2));

    const AtomicComparator::Ptr comp(fetchGeneralComparator(a1, a2, context));
    /* The fetchGeneralComparator call may rewrite a1 and/or a2. */
    Q_ASSERT(a1);
    Q_ASSERT(a2);
    Q_ASSERT(comp);

    return compare(a1->evaluateSingleton(context),
                   a2->evaluateSingleton(context),
                   comp,
                   m_operator);
}

bool GeneralComparison::evaluateEBV(const DynamicContext::Ptr &context) const
{
    const Item::Iterator::Ptr it1(m_operand1->evaluateSequence(context));
    Item item1(it1->next());

    if(!item1)
        return false;

    const Item::Iterator::Ptr it2(m_operand2->evaluateSequence(context));
    Item::List cache;
    Item item2;

    while(true)
    {
        item2 = it2->next();
        if(!item2)
            break;

        if(generalCompare(item1, item2, context))
            return true;

        cache.append(item2);
    }

    while(true)
    {
        item1 = it1->next();

        if(!item1)
            return false;

        const Item::List::const_iterator end(cache.constEnd());
        Item::List::const_iterator it(cache.constBegin());

        for(; it != end; ++it)
            if(generalCompare(item1, *it, context))
                return true;
    }

    Q_ASSERT(false);
    return false;
}

Expression::Ptr GeneralComparison::compress(const StaticContext::Ptr &context)
{
    const Expression::Ptr me(PairContainer::compress(context));

    if(me != this)
        return me;

    if(ValueComparison::isCaseInsensitiveCompare(m_operand1, m_operand2))
        useCaseInsensitiveComparator();

    return me;
}

Expression::Ptr GeneralComparison::typeCheck(const StaticContext::Ptr &context,
                                             const SequenceType::Ptr &reqType)
{

    const Expression::Ptr me(PairContainer::typeCheck(context, reqType));

    const ItemType::Ptr t1(m_operand1->staticType()->itemType());
    const ItemType::Ptr t2(m_operand2->staticType()->itemType());

    if(*CommonSequenceTypes::Empty == *t1 ||
       *CommonSequenceTypes::Empty == *t2)
    {
        return wrapLiteral(CommonValues::BooleanFalse, context, this);
    }

    if(*BuiltinTypes::xsAnyAtomicType == *t1 ||
       *BuiltinTypes::xsAnyAtomicType == *t2)
        return me;

    prepareComparison(fetchGeneralComparator(m_operand1, m_operand2, context));

    if(!m_operand1->staticType()->cardinality().allowsMany() &&
       !m_operand2->staticType()->cardinality().allowsMany())
    {
        /* Rewrite to a ValueComparison whose operands uses typing rules
         * as for an general comparison(that's what's done above). */
        return rewrite(Expression::Ptr(new ValueComparison(m_operand1,
                                                           m_operator,
                                                           m_operand2))->typeCheck(context, reqType),
                       context);
    }
    else
        return me;
}

void GeneralComparison::updateType(ItemType::Ptr &type,
                                   const Expression::Ptr &source)
{
    type = source->staticType()->itemType();
}

AtomicComparator::Ptr GeneralComparison::fetchGeneralComparator(Expression::Ptr &op1,
                                                                Expression::Ptr &op2,
                                                                const ReportContext::Ptr &context) const
{
    ItemType::Ptr t1(op1->staticType()->itemType());
    ItemType::Ptr t2(op2->staticType()->itemType());

    /* a. "If one of the atomic values is an instance of xs:untypedAtomic and
     *    the other is an instance of a numeric type, then the xs:untypedAtomic
     *    value is cast to the type xs:double." */
    if(BuiltinTypes::numeric->xdtTypeMatches(t1) &&
       BuiltinTypes::xsUntypedAtomic->xdtTypeMatches(t2))
    {
        op2 = Expression::Ptr(new UntypedAtomicConverter(op2, BuiltinTypes::xsDouble));

        /* The types might have changed, reload. */
        updateType(t2, op2);
    }
    else if(BuiltinTypes::numeric->xdtTypeMatches(t2) &&
            BuiltinTypes::xsUntypedAtomic->xdtTypeMatches(t1))
    {
        op1 = Expression::Ptr(new UntypedAtomicConverter(op1, BuiltinTypes::xsDouble));

        /* The types might have changed, reload. */
        updateType(t1, op1);
    }
    /* "If XPath 1.0 compatibility mode is true, a general comparison is
     *  evaluated by applying the following rules, in order:
     *  1. If either operand is a single atomic value that is an instance of
     *  xs:boolean, then the other operand is converted to xs:boolean by taking
     *  its effective boolean value."
     *
     * Notably, it's not conversion to boolean, it is EBV extraction.
     */
    else if(m_isBackwardsCompat && BuiltinTypes::xsBoolean->xdtTypeMatches(t1))
    {
        op2 = Expression::Ptr(new EBVExtractor(op2));
        updateType(t2, op2);
    }
    else if(m_isBackwardsCompat && BuiltinTypes::xsBoolean->xdtTypeMatches(t2))
    {
        op1 = Expression::Ptr(new EBVExtractor(op1));
        updateType(t1, op1);
    }
    /* b. "If one of the atomic values is an instance of xs:untypedAtomic and
     *    the other is an instance of xs:untypedAtomic or xs:string, then the
     *    xs:untypedAtomic value (or values) is (are) cast to the type xs:string."
     *
     * c. "If one of the atomic values is an instance of xs:untypedAtomic and the
     *    other is not an instance of xs:string, xs:untypedAtomic, or any numeric
     *    type, then the xs:untypedAtomic value is cast to the dynamic type of the
     *    other value." */
    else if(BuiltinTypes::xsUntypedAtomic->xdtTypeMatches(t1) &&
            !BuiltinTypes::xsString->xdtTypeMatches(t2) &&
            !BuiltinTypes::xsUntypedAtomic->xdtTypeMatches(t2) &&
            !BuiltinTypes::xsAnyURI->xdtTypeMatches(t2))
    {
        op1 = Expression::Ptr(new UntypedAtomicConverter(op1, t2));
        updateType(t1, op1);
    }
    else if(BuiltinTypes::xsUntypedAtomic->xdtTypeMatches(t2) &&
            !BuiltinTypes::xsString->xdtTypeMatches(t1) &&
            !BuiltinTypes::xsUntypedAtomic->xdtTypeMatches(t1) &&
            !BuiltinTypes::xsAnyURI->xdtTypeMatches(t1))
    {
        op2 = Expression::Ptr(new UntypedAtomicConverter(op2, t1));
        updateType(t2, op2);
    }

    /* d. "After performing the conversions described above, the atomic
     *     values are compared using one of the value comparison operators
     *     eq, ne, lt, le, gt, or ge, depending on whether the general comparison
     *     operator was =, !=, <, <=, >, or >=. The values have the required
     *     magnitude relationship if and only if the result of this value comparison
     *     is true." */

    return fetchComparator(t1, t2, context);
}

OptimizationPass::List GeneralComparison::optimizationPasses() const
{
    Q_ASSERT(!OptimizationPasses::comparisonPasses.isEmpty());
    return OptimizationPasses::comparisonPasses;
}

SequenceType::List GeneralComparison::expectedOperandTypes() const
{
    SequenceType::List result;
    result.append(CommonSequenceTypes::ZeroOrMoreAtomicTypes);
    result.append(CommonSequenceTypes::ZeroOrMoreAtomicTypes);
    return result;
}

SequenceType::Ptr GeneralComparison::staticType() const
{
    return CommonSequenceTypes::ExactlyOneBoolean;
}

ExpressionVisitorResult::Ptr GeneralComparison::accept(const ExpressionVisitor::Ptr &visitor) const
{
    return visitor->visit(this);
}

Expression::ID GeneralComparison::id() const
{
    return IDGeneralComparison;
}

QT_END_NAMESPACE
