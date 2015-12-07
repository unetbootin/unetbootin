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
#include "qexpressionsequence_p.h"
#include "qsorttuple_p.h"

#include "qreturnorderby_p.h"

QT_BEGIN_NAMESPACE

using namespace QPatternist;

ReturnOrderBy::ReturnOrderBy(const OrderBy::Stability aStability,
                             const OrderBy::OrderSpec::Vector &oSpecs,
                             const Expression::List &ops) : UnlimitedContainer(ops)
                                                          , m_stability(aStability)
                                                          , m_orderSpecs(oSpecs)
                                                          , m_flyAway(true)
{
    Q_ASSERT_X(m_operands.size() >= 2, Q_FUNC_INFO,
               "ReturnOrderBy must have the return expression, and at least one sort key.");
    Q_ASSERT(m_orderSpecs.size() == ops.size() - 1);
}

Item ReturnOrderBy::evaluateSingleton(const DynamicContext::Ptr &context) const
{
    Q_ASSERT(m_operands.size() > 1);
    const Item::Iterator::Ptr value(makeListIterator(m_operands.first()->evaluateSequence(context)->toList()));
    Item::Vector sortKeys;

    /* We're skipping the first operand. */
    const int len = m_operands.size() - 1;
    sortKeys.resize(len);

    for(int i = 1; i <= len; ++i)
        sortKeys[i - 1] = m_operands.at(i)->evaluateSingleton(context);

    return Item(new SortTuple(value, sortKeys));
}

bool ReturnOrderBy::evaluateEBV(const DynamicContext::Ptr &context) const
{
    // TODO This is temporary code.
    return m_operands.first()->evaluateEBV(context);
}

Expression::Ptr ReturnOrderBy::compress(const StaticContext::Ptr &context)
{
    /* We first did this in typeCheck(), but that broke due to that type checks were
     * missed, which other pieces relied on. */
    if(m_flyAway)
    {
        /* We only want the return expression, not the sort keys. */
        return m_operands.first()->compress(context);
    }
    else
    {
        /* We don't need the members, so don't keep a reference to them. */
        m_orderSpecs.clear();

        return UnlimitedContainer::compress(context);
    }
}

Expression::Properties ReturnOrderBy::properties() const
{
    /* For some unknown reason this is necessary for XQTS test case orderBy18. */
    return DisableElimination;
}

ExpressionVisitorResult::Ptr ReturnOrderBy::accept(const ExpressionVisitor::Ptr &visitor) const
{
    return visitor->visit(this);
}

SequenceType::Ptr ReturnOrderBy::staticType() const
{
    return m_operands.first()->staticType();
}

SequenceType::List ReturnOrderBy::expectedOperandTypes() const
{
    SequenceType::List result;
    result.append(CommonSequenceTypes::ZeroOrMoreItems);
    result.append(CommonSequenceTypes::ZeroOrOneAtomicType);
    return result;
}

Expression::ID ReturnOrderBy::id() const
{
    return IDReturnOrderBy;
}

QT_END_NAMESPACE
