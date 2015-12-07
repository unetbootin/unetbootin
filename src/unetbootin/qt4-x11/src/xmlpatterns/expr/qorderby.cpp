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

#include <QtAlgorithms>

#include "qcommonsequencetypes_p.h"
#include "qnodebuilder_p.h"
#include "qschemanumeric_p.h"
#include "qpatternistlocale_p.h"
#include "qreturnorderby_p.h"
#include "qsorttuple_p.h"
#include "qsequencemappingiterator_p.h"

#include "qorderby_p.h"

QT_BEGIN_NAMESPACE

using namespace QPatternist;

OrderBy::OrderBy(const Stability stability,
                 const OrderSpec::Vector &aOrderSpecs,
                 const Expression::Ptr &op,
                 ReturnOrderBy *const returnOrderBy) : SingleContainer(op)
                                                     , m_stability(stability)
                                                     , m_orderSpecs(aOrderSpecs)
                                                     , m_returnOrderBy(returnOrderBy)
{
    Q_ASSERT(m_returnOrderBy);
}

void OrderBy::OrderSpec::prepare(const Expression::Ptr &source,
                                 const StaticContext::Ptr &context)
{
    m_expr = source;
    const ItemType::Ptr t(source->staticType()->itemType());
    prepareComparison(fetchComparator(t, t, context));
}

/**
 * @short Functor used by Qt's qSort() and qStableSort(). Used for FLWOR's
 * <tt>order by</tt> expression.
 *
 * This must be in the global namespace, since it is specializing qLess(), which
 * is in the global namespace. Hence it can't be in QPatternist.
 */
template<>
class qLess<Item::List>
{
private:

    static inline bool isNaN(const Item &i)
    {
        return BuiltinTypes::xsDouble->xdtTypeMatches(i.type()) &&
               i.as<Numeric>()->isNaN();
    }

public:
    inline qLess(const OrderBy::OrderSpec::Vector &orderspecs,
                 const DynamicContext::Ptr &context) : m_orderSpecs(orderspecs)
                                                     , m_context(context)
    {
        Q_ASSERT(!m_orderSpecs.isEmpty());
        Q_ASSERT(context);
    }

    inline bool operator()(const Item &item1, const Item &item2) const
    {
        const SortTuple *const s1 = item1.as<SortTuple>();
        const SortTuple *const s2 = item2.as<SortTuple>();

        const Item::Vector &sortKeys1 = s1->sortKeys();
        const Item::Vector &sortKeys2 = s2->sortKeys();
        const int len = sortKeys1.count();
        Q_ASSERT(sortKeys1.count() == sortKeys2.count());

        for(int i = 0; i < len; ++i)
        {
            const Item &i1 = sortKeys1.at(i);
            const Item &i2 = sortKeys2.at(i);
            const OrderBy::OrderSpec &orderSpec = m_orderSpecs.at(i);

            if(!i1)
            {
                if(i2 && !isNaN(i2))
                {
                    /* We got ((), item()). */
                    return orderSpec.orderingEmptySequence == StaticContext::Least ? orderSpec.direction == OrderBy::OrderSpec::Ascending
                                                                                   : orderSpec.direction != OrderBy::OrderSpec::Ascending;
                }
                else
                    return false;
            }

            if(!i2)
            {
                if(i1 && !isNaN(i1))
                    /* We got (item(), ()). */
                    return orderSpec.orderingEmptySequence == StaticContext::Greatest ? orderSpec.direction == OrderBy::OrderSpec::Ascending
                                                                                      : orderSpec.direction != OrderBy::OrderSpec::Ascending;
                else
                    return false;
            }

            Q_ASSERT(orderSpec.direction == OrderBy::OrderSpec::Ascending ||
                     orderSpec.direction == OrderBy::OrderSpec::Descending);
            const AtomicComparator::ComparisonResult result = orderSpec.detailedFlexibleCompare(i1, i2, m_context);

            switch(result)
            {
                case AtomicComparator::LessThan:
                    return orderSpec.direction == OrderBy::OrderSpec::Ascending;
                case AtomicComparator::GreaterThan:
                    return orderSpec.direction != OrderBy::OrderSpec::Ascending;
                case AtomicComparator::Equal:
                    continue;
                case AtomicComparator::Incomparable:
                    Q_ASSERT_X(false, Q_FUNC_INFO, "This code path assume values are always comparable.");
            }
        }

        return false;
    }

private:
    /* Yes, we store references here. */
    const OrderBy::OrderSpec::Vector & m_orderSpecs;
    const DynamicContext::Ptr & m_context;
};

Item::Iterator::Ptr OrderBy::mapToSequence(const Item &i,
                                           const DynamicContext::Ptr &) const
{
    return i.as<SortTuple>()->value();
}

Item::Iterator::Ptr OrderBy::evaluateSequence(const DynamicContext::Ptr &context) const
{
    Item::List tuples(m_operand->evaluateSequence(context)->toList());

    const qLess<Item::List> sorter(m_orderSpecs, context);

    Q_ASSERT(m_stability == StableOrder || m_stability == UnstableOrder);

    /* On one hand we could just disregard stability and always use qStableSort(), but maybe qSort()
     * is a bit faster? */
    if(m_stability == StableOrder)
        qStableSort(tuples.begin(), tuples.end(), sorter);
    else
    {
        Q_ASSERT(m_stability == UnstableOrder);
        qSort(tuples.begin(), tuples.end(), sorter);
    }

    return makeSequenceMappingIterator<Item>(ConstPtr(this),
                                             makeListIterator(tuples),
                                             context);
}

Expression::Ptr OrderBy::typeCheck(const StaticContext::Ptr &context,
                                   const SequenceType::Ptr &reqType)
{
    m_returnOrderBy->setStay(true);

    /* It's important we do the typeCheck() before calling OrderSpec::prepare(), since
     * atomizers must first be inserted. */
    const Expression::Ptr me(SingleContainer::typeCheck(context, reqType));

    const Expression::List ops(m_returnOrderBy->operands());
    const int len = ops.count();
    Q_ASSERT(ops.count() > 1);
    Q_ASSERT(m_orderSpecs.count() == ops.count() - 1);

    for(int i = 1; i < len; ++i)
        m_orderSpecs[i - 1].prepare(ops.at(i), context);

    return me;

    /* It's not meaningful to sort a single item or less, so rewrite ourselves
     * away if that is the case. This is an optimization. */
    /* TODO: How do we remove ReturnOrderBy?
    if(Cardinality::zeroOrOne().isMatch(m_operand->staticType()->cardinality()))
        return m_operand->typeCheck(context, reqType);
    else
        return SingleContainer::typeCheck(context, reqType);
     */
}

Expression::Properties OrderBy::properties() const
{
    return m_operand->properties() & DisableElimination;
}

Expression::Ptr OrderBy::compress(const StaticContext::Ptr &context)
{
    /* If we only will produce one item, there's no point in sorting. */
    if(m_operand->staticType()->cardinality().allowsMany())
        return SingleContainer::compress(context);
    else
    {
        m_returnOrderBy->setStay(false);
        return m_operand->compress(context);
    }
}

SequenceType::Ptr OrderBy::staticType() const
{
    return m_operand->staticType();
}

SequenceType::List OrderBy::expectedOperandTypes() const
{
    SequenceType::List result;
    result.append(CommonSequenceTypes::ZeroOrMoreItems);
    return result;
}

ExpressionVisitorResult::Ptr
OrderBy::accept(const ExpressionVisitor::Ptr &visitor) const
{
    return visitor->visit(this);
}

QT_END_NAMESPACE
