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

#include "qbuiltintypes_p.h"
#include "qcommonsequencetypes_p.h"
#include "qcommonvalues_p.h"
#include "qgenericsequencetype_p.h"
#include "qinteger_p.h"
#include "qliteral_p.h"
#include "qrangeiterator_p.h"

#include "qrangeexpression_p.h"

QT_BEGIN_NAMESPACE

using namespace QPatternist;

RangeExpression::RangeExpression(const Expression::Ptr &operand1,
                                 const Expression::Ptr &operand2) : PairContainer(operand1, operand2)
{
}

Item::Iterator::Ptr RangeExpression::evaluateSequence(const DynamicContext::Ptr &context) const
{
    const Item s(m_operand1->evaluateSingleton(context));

    if(!s)
        return CommonValues::emptyIterator;

    const Item e(m_operand2->evaluateSingleton(context));
    if(!e)
        return CommonValues::emptyIterator;

    const xsInteger start = s.as<Numeric>()->toInteger();
    const xsInteger end = e.as<Numeric>()->toInteger();

    if(start > end)
        return CommonValues::emptyIterator;
    else if(start == end)
        return makeSingletonIterator(s);
    else
        return Item::Iterator::Ptr(new RangeIterator(start, RangeIterator::Forward, end));
}

Item RangeExpression::evaluateSingleton(const DynamicContext::Ptr &context) const
{
    return m_operand1->evaluateSingleton(context);
}

SequenceType::List RangeExpression::expectedOperandTypes() const
{
    SequenceType::List result;
    result.append(CommonSequenceTypes::ZeroOrOneInteger);
    result.append(CommonSequenceTypes::ZeroOrOneInteger);
    return result;
}

SequenceType::Ptr RangeExpression::staticType() const
{
    /* This logic makes the Cardinality more specific. */
    Cardinality::Count from;
    bool hasFrom;

    if(m_operand1->is(IDIntegerValue))
    {
        from = m_operand1->as<Literal>()->item().as<Integer>()->toInteger();
        hasFrom = true;
    }
    else
    {
        hasFrom = false;
        from = 0;
    }

    /* We can't check whether to is -1 since maybe the user wrote -1. Hence
     * hasTo is required. */
    bool hasTo;
    Cardinality::Count to;

    if(m_operand2->is(IDIntegerValue))
    {
        const xsInteger asInt = m_operand2->as<Literal>()->item().as<Integer>()->toInteger();
        to = asInt;

        if(to == asInt)
            hasTo = true;
        else
        {
            /* Cardinality::Count is not the same as type xsInteger. We had overflow. */
            to = -1;
            hasTo = false;
        }
    }
    else
    {
        to = -1;
        hasTo = false;
    }

    if(hasTo && hasFrom)
    {
        if(from > to)
        {
            /* The query is incorrectly written, we'll evaluate to the empty sequence.
             * Just return what's correct. */
            return CommonSequenceTypes::ZeroOrMoreIntegers;
        }
        else
        {
            Cardinality::Count count = (to - from) + 1; /* + 1, since it's inclusive. */
            return makeGenericSequenceType(BuiltinTypes::xsInteger, Cardinality::fromExact(count));
        }
    }
    else
    {
        /* We can't do fromExact(from, -1) since the latter can evaluate to a value that actually is
         * lower than from, although that unfortunately is very unlikely. */
        return CommonSequenceTypes::ZeroOrMoreIntegers;
    }
}

Expression::Properties RangeExpression::properties() const
{
    return Expression::DisableElimination;
}

ExpressionVisitorResult::Ptr RangeExpression::accept(const ExpressionVisitor::Ptr &visitor) const
{
    return visitor->visit(this);
}

QT_END_NAMESPACE
