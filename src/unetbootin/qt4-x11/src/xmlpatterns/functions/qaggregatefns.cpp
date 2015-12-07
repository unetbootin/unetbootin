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

#include "qabstractfloat_p.h"
#include "qarithmeticexpression_p.h"
#include "qbuiltintypes_p.h"
#include "qcommonsequencetypes_p.h"
#include "qcommonvalues_p.h"
#include "qdecimal_p.h"
#include "qgenericsequencetype_p.h"
#include "qinteger_p.h"
#include "qoptimizerblocks_p.h"
#include "qsequencefns_p.h"
#include "quntypedatomicconverter_p.h"

#include "qaggregatefns_p.h"

QT_BEGIN_NAMESPACE

using namespace QPatternist;

Item CountFN::evaluateSingleton(const DynamicContext::Ptr &context) const
{
    return Integer::fromValue(m_operands.first()->evaluateSequence(context)->count());
}

Expression::Ptr CountFN::typeCheck(const StaticContext::Ptr &context,
                                   const SequenceType::Ptr &reqType)
{
    if(*CommonSequenceTypes::EBV->itemType() == *reqType->itemType())
    {
        return ByIDCreator::create(IDExistsFN, operands(), context, this)->typeCheck(context, reqType);
    }
    else
        return FunctionCall::typeCheck(context, reqType);
}

Expression::Ptr CountFN::compress(const StaticContext::Ptr &context)
{
    const Expression::Ptr me(FunctionCall::compress(context));
    if(me != this)
        return me;

    const Cardinality card(m_operands.first()->staticType()->cardinality());
    if(card.isExactlyOne())
        return wrapLiteral(CommonValues::IntegerOne, context, this);
    else if(card.isEmpty())
    {
        /* One might think that if the operand is (), that compress() would have
         * evaluated us and therefore this line never be reached, but "()" can
         * be combined with the DisableElimination flag. */
        return wrapLiteral(CommonValues::IntegerZero, context, this);
    }
    else if(card.isExact())
        return wrapLiteral(Integer::fromValue(card.minimum()), context, this);
    else
        return me;
}

Expression::Ptr AddingAggregate::typeCheck(const StaticContext::Ptr &context,
                                           const SequenceType::Ptr &reqType)
{
    const Expression::Ptr me(FunctionCall::typeCheck(context, reqType));
    ItemType::Ptr t1(m_operands.first()->staticType()->itemType());

    if(*CommonSequenceTypes::Empty == *t1)
        return me;
    else if(*BuiltinTypes::xsAnyAtomicType == *t1 ||
            *BuiltinTypes::numeric == *t1)
        return me;
    else if(BuiltinTypes::xsUntypedAtomic->xdtTypeMatches(t1))
    {
        m_operands.replace(0, Expression::Ptr(new UntypedAtomicConverter(m_operands.first(),
                                                                         BuiltinTypes::xsDouble)));
        t1 = m_operands.first()->staticType()->itemType();
    }
    else if(!BuiltinTypes::numeric->xdtTypeMatches(t1) &&
            !BuiltinTypes::xsDayTimeDuration->xdtTypeMatches(t1) &&
            !BuiltinTypes::xsYearMonthDuration->xdtTypeMatches(t1))
    {
        /* Translator, don't translate the type names. */
        context->error(QtXmlPatterns::tr("The first argument to %1 cannot be "
                                         "of type %2. It must be a numeric "
                                         "type, xs:yearMonthDuration or "
                                         "xs:dayTimeDuration.")
                       .arg(formatFunction(context->namePool(), signature()))
                       .arg(formatType(context->namePool(),
                                       m_operands.first()->staticType())),
                       ReportContext::FORG0006, this);
    }

    if(!m_operands.first()->staticType()->cardinality().allowsMany())
        return m_operands.first();

    /* We know fetchMathematician won't attempt a rewrite of the operand, so this is safe. */
    m_mather = ArithmeticExpression::fetchMathematician(m_operands.first(), m_operands.first(),
                                                        AtomicMathematician::Add, true, context,
                                                        this,
                                                        ReportContext::FORG0006);
    return me;
}

Item AvgFN::evaluateSingleton(const DynamicContext::Ptr &context) const
{
    const Item::Iterator::Ptr it(m_operands.first()->evaluateSequence(context));
    Item sum(it->next());

    xsInteger count = 0;
    while(sum)
    {
        ++count;
        const Item next(it->next());
        if(!next)
            break;

        sum = ArithmeticExpression::flexiblyCalculate(sum, AtomicMathematician::Add,
                                                      next, m_adder, context,
                                                      this,
                                                      ReportContext::FORG0006);
    };

    if(!sum)
        return Item();

    /* Note that we use the same m_mather which was used for adding,
     * can be worth to think about. */
    return ArithmeticExpression::flexiblyCalculate(sum, AtomicMathematician::Div,
                                                   Integer::fromValue(count),
                                                   m_divider, context,
                                                   this,
                                                   ReportContext::FORG0006);
}

Expression::Ptr AvgFN::typeCheck(const StaticContext::Ptr &context,
                                 const SequenceType::Ptr &reqType)
{
    const Expression::Ptr me(FunctionCall::typeCheck(context, reqType));
    ItemType::Ptr t1(m_operands.first()->staticType()->itemType());

    if(*CommonSequenceTypes::Empty == *t1)
        return me;
    else if(*BuiltinTypes::xsAnyAtomicType == *t1 ||
            *BuiltinTypes::numeric == *t1)
        return me;
    else if(BuiltinTypes::xsUntypedAtomic->xdtTypeMatches(t1))
    {
        m_operands.replace(0, Expression::Ptr(new UntypedAtomicConverter(m_operands.first(),
                                                                         BuiltinTypes::xsDouble)));
        t1 = m_operands.first()->staticType()->itemType();
    }
    else if(!BuiltinTypes::numeric->xdtTypeMatches(t1) &&
            !BuiltinTypes::xsDayTimeDuration->xdtTypeMatches(t1) &&
            !BuiltinTypes::xsYearMonthDuration->xdtTypeMatches(t1))
    {
        /* Translator, don't translate the type names. */
        context->error(QtXmlPatterns::tr("The first argument to %1 cannot be "
                                         "of type %2. It must be of type %3, "
                                         "%4, or %5.")
                          .arg(signature())
                          .arg(formatType(context->namePool(), m_operands.first()->staticType()))
                          .arg(formatType(context->namePool(), BuiltinTypes::numeric))
                          .arg(formatType(context->namePool(), BuiltinTypes::xsYearMonthDuration))
                          .arg(formatType(context->namePool(), BuiltinTypes::xsDayTimeDuration)),
                       ReportContext::FORG0006, this);
    }

    if(!m_operands.first()->staticType()->cardinality().allowsMany())
        return m_operands.first();

    /* We use CommonValues::IntegerOne here because it is an arbitrary Expression
     * of type xs:integer */
    Expression::Ptr op2(wrapLiteral(CommonValues::IntegerOne, context, this));
    m_adder = ArithmeticExpression::fetchMathematician(m_operands.first(), m_operands.first(),
                                                       AtomicMathematician::Add, true, context, this);
    m_divider = ArithmeticExpression::fetchMathematician(m_operands.first(), op2,
                                                         AtomicMathematician::Div, true, context, this);
    return me;
}

SequenceType::Ptr AvgFN::staticType() const
{
    const SequenceType::Ptr opt(m_operands.first()->staticType());
    ItemType::Ptr t(opt->itemType());

    if(BuiltinTypes::xsUntypedAtomic->xdtTypeMatches(t))
        t = BuiltinTypes::xsDouble; /* xsUntypedAtomics are converted to xsDouble. */
    else if(BuiltinTypes::xsInteger->xdtTypeMatches(t))
        t = BuiltinTypes::xsDecimal;

    /* else, it means the type is xsDayTimeDuration, xsYearMonthDuration,
     * xsDouble, xsFloat or xsAnyAtomicType, which we use as is. */
    return makeGenericSequenceType(BuiltinTypes::xsAnyAtomicType->xdtTypeMatches(t) ? t : ItemType::Ptr(BuiltinTypes::xsAnyAtomicType),
                                   opt->cardinality().toWithoutMany());
}

Item SumFN::evaluateSingleton(const DynamicContext::Ptr &context) const
{
    const Item::Iterator::Ptr it(m_operands.first()->evaluateSequence(context));
    Item sum(it->next());

    while(sum)
    {
        const Item next(it->next());
        if(!next)
            break;

        sum = ArithmeticExpression::flexiblyCalculate(sum, AtomicMathematician::Add,
                                                      next, m_mather, context, this,
                                                      ReportContext::FORG0006);
    };

    if(!sum)
    {
        if(m_operands.count() == 1)
            return CommonValues::IntegerZero;
        else
            return m_operands.last()->evaluateSingleton(context);
    }

    return sum;
}

Expression::Ptr SumFN::typeCheck(const StaticContext::Ptr &context,
                                 const SequenceType::Ptr &reqType)
{
    const Expression::Ptr me(AddingAggregate::typeCheck(context, reqType));

    if(*CommonSequenceTypes::Empty == *m_operands.first()->staticType()->itemType())
    {
        if(m_operands.count() == 1)
            return wrapLiteral(CommonValues::IntegerZero, context, this);
        else
            return m_operands.at(1);
    }

    if(m_operands.count() == 1)
        return me;

    const ItemType::Ptr t(m_operands.at(1)->staticType()->itemType());

    if(!BuiltinTypes::numeric->xdtTypeMatches(t) &&
       !BuiltinTypes::xsAnyAtomicType->xdtTypeMatches(t) &&
       *CommonSequenceTypes::Empty != *t &&
       !BuiltinTypes::xsDayTimeDuration->xdtTypeMatches(t) &&
       !BuiltinTypes::xsYearMonthDuration->xdtTypeMatches(t))
    {
        context->error(QtXmlPatterns::tr("The second argument to %1 cannot be "
                                         "of type %2. It must be of type %3, "
                                         "%4, or %5.")
                          .arg(formatFunction(context->namePool(), signature()))
                          .arg(formatType(context->namePool(), m_operands.at(1)->staticType()))
                          .arg(formatType(context->namePool(), BuiltinTypes::numeric))
                          .arg(formatType(context->namePool(), BuiltinTypes::xsYearMonthDuration))
                          .arg(formatType(context->namePool(), BuiltinTypes::xsDayTimeDuration)),
                       ReportContext::FORG0006, this);
        return me;
    }

    return me;
}

SequenceType::Ptr SumFN::staticType() const
{
    const SequenceType::Ptr t(m_operands.first()->staticType());

    if(m_operands.count() == 1)
    {
        return makeGenericSequenceType(t->itemType() | BuiltinTypes::xsInteger,
                                       Cardinality::exactlyOne());
    }
    else
    {
        return makeGenericSequenceType(t->itemType() | m_operands.at(1)->staticType()->itemType(),
                                       t->cardinality().toWithoutMany());
    }
}

QT_END_NAMESPACE
