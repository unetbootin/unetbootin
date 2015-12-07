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

#include <math.h>

#include <qnumeric.h>

#include "qabstractdatetime_p.h"
#include "qabstractduration_p.h"
#include "qabstractfloat_p.h"
#include "qdaytimeduration_p.h"
#include "qdecimal_p.h"
#include "qinteger_p.h"
#include "qpatternistlocale_p.h"

#include "qatomicmathematicians_p.h"

QT_BEGIN_NAMESPACE

using namespace QPatternist;

/* The translation strings is place here once, in order to reduce work for translators,
 * and provide consistency. */

static inline QString idivZeroInvalid()
{
    return QtXmlPatterns::tr("Integer division (%1) by zero (%2) is undefined.")
             .arg(formatKeyword("idiv"))
             .arg(formatData("0"));
}

static inline QString divZeroInvalid()
{
    return QtXmlPatterns::tr("Division (%1) by zero (%2) is undefined.")
             .arg(formatKeyword("div"))
             .arg(formatData("0"));
}

static inline QString modZeroInvalid()
{
    return QtXmlPatterns::tr("Modulus division (%1) by zero (%2) is undefined.")
             .arg(formatKeyword("mod"))
             .arg(formatData("0"));
}

Item DecimalMathematician::calculate(const Item &o1,
                                     const Operator op,
                                     const Item &o2,
                                     const QExplicitlySharedDataPointer<DynamicContext> &context) const
{
    switch(op)
    {
        case Div:
        {
            if(o2.as<Numeric>()->toInteger() == 0)
            {
                context->error(divZeroInvalid(), ReportContext::FOAR0001, this);
                return Item(); /* Silences source code analyzer warning. */
            }
            else
                return toItem(Decimal::fromValue(o1.as<Numeric>()->toDecimal() / o2.as<Numeric>()->toDecimal()));
        }
        case IDiv:
        {
            if(o2.as<Numeric>()->toInteger() == 0)
            {
                context->error(idivZeroInvalid(), ReportContext::FOAR0001, this);
                return Item(); /* Silences source code analyzer warning. */
            }
            else
                return Integer::fromValue(static_cast<xsInteger>(o1.as<Numeric>()->toDecimal() /
                                                                 o2.as<Numeric>()->toDecimal()));
        }
        case Substract:
            return toItem(Decimal::fromValue(o1.as<Numeric>()->toDecimal() - o2.as<Numeric>()->toDecimal()));
        case Mod:
        {
            if(o2.as<Numeric>()->toInteger() == 0)
            {
                context->error(modZeroInvalid(), ReportContext::FOAR0001, this);
                return Item(); /* Silences source code analyzer warning. */
            }
            else
                return toItem(Decimal::fromValue(::fmod(o1.as<Numeric>()->toDecimal(), o2.as<Numeric>()->toDecimal())));
        }
        case Multiply:
            return toItem(Decimal::fromValue(o1.as<Numeric>()->toDecimal() * o2.as<Numeric>()->toDecimal()));
        case Add:
            return toItem(Decimal::fromValue(o1.as<Numeric>()->toDecimal() + o2.as<Numeric>()->toDecimal()));
    }

    Q_ASSERT(false);
    return Item(); /* GCC unbarfer. */
}

Item IntegerMathematician::calculate(const Item &o1,
                                     const Operator op,
                                     const Item &o2,
                                     const QExplicitlySharedDataPointer<DynamicContext> &context) const
{
    switch(op)
    {
        case Div:
            if(o2.as<Numeric>()->toInteger() == 0)
            {
                context->error(divZeroInvalid(), ReportContext::FOAR0001, this);
                return Item(); /* Silences source code analyzer warning. */
            }
            else /* C++ automatically performs truncation of long integer(xsInteger). */
                return toItem(Decimal::fromValue(o1.as<Numeric>()->toDecimal() / o2.as<Numeric>()->toDecimal()));
        case IDiv:
        {
            if(o2.as<Numeric>()->toInteger() == 0)
            {
                context->error(idivZeroInvalid(), ReportContext::FOAR0001, this);
                return Item(); /* Silences source code analyzer warning. */
            }
            else /* C++ automatically performs truncation of long integer(xsInteger). */
                return Integer::fromValue(o1.as<Numeric>()->toInteger() / o2.as<Numeric>()->toInteger());
        }
        case Substract:
            return Integer::fromValue(o1.as<Numeric>()->toInteger() - o2.as<Numeric>()->toInteger());
        case Mod:
        {
            const xsInteger divisor = o2.as<Numeric>()->toInteger();

            if(divisor == 0)
            {
                context->error(modZeroInvalid(), ReportContext::FOAR0001, this);
                return Item(); /* Silences source code analyzer warning. */
            }
            else
                return Integer::fromValue(o1.as<Numeric>()->toInteger() % divisor);
        }
        case Multiply:
            return Integer::fromValue(o1.as<Numeric>()->toInteger() * o2.as<Numeric>()->toInteger());
        case Add:
            return Integer::fromValue(o1.as<Numeric>()->toInteger() + o2.as<Numeric>()->toInteger());
    }

    Q_ASSERT(false);
    return Item(); /* GCC unbarfer. */
}

Item DurationNumericMathematician::calculate(const Item &o1,
                                             const Operator op,
                                             const Item &o2,
                                             const QExplicitlySharedDataPointer<DynamicContext> &context) const
{
    Q_ASSERT(op == Div || op == Multiply);

    const AbstractDuration::Ptr duration(o1.as<AbstractDuration>());
    const xsDouble dbl = o2.as<Numeric>()->toDouble();

    switch(op)
    {
        case Div:
        {
            if(qIsInf(dbl))
                return duration->fromValue(0);
            else if(qIsNaN(dbl))
            {
                context->error(QtXmlPatterns::tr(
                           "Dividing a value of type %1 by %2 (not-a-number) "
                           "is not allowed.")
                               .arg(formatType(context->namePool(),
                                               duration->type()))
                               .arg(formatData("NaN")),
                               ReportContext::FOCA0005,
                               this);
                return Item();
            }
            else if(Double::isEqual(dbl, 0))
            {
                context->error(QtXmlPatterns::tr(
                           "Dividing a value of type %1 by %2 or %3 (plus or "
                           "minus zero) is not allowed.")
                               .arg(formatType(context->namePool(),
                                               duration->type()))
                               .arg(formatData("-0"))
                               .arg(formatData("0")),
                               ReportContext::FODT0002,
                               this);
                return Item();
            }

            return duration->fromValue(static_cast<AbstractDuration::Value>(duration->value() / dbl));
        }
        case Multiply:
        {
            if(Double::isEqual(dbl, 0))
                return duration->fromValue(0);
            else if(qIsNaN(dbl))
            {
                context->error(QtXmlPatterns::tr(
                           "Dividing a value of type %1 by %2 (not-a-number) "
                           "is not allowed.")
                               .arg(formatType(context->namePool(),
                                               duration->type()))
                               .arg(formatData("NaN")),
                               ReportContext::FOCA0005,
                               this);
                return Item();
            }
            else if(qIsInf(dbl))
            {
                context->error(QtXmlPatterns::tr(
                           "Multiplication of a value of type %1 by %2 or %3 "
                           "(plus or minus infinity) is not allowed.")
                               .arg(formatType(context->namePool(),
                                               duration->type()))
                               .arg(formatData("-INF"))
                               .arg(formatData("INF")),
                               ReportContext::FODT0002,
                               this);
                return Item();
            }

            return duration->fromValue(static_cast<AbstractDuration::Value>(duration->value() * dbl));
        }
        default:
        {
            Q_ASSERT(false);
            return Item(); /* Silence warning. */
        }
    }
}

Item DurationDurationMathematician::calculate(const Item &o1,
                                                   const Operator op,
                                                   const Item &o2,
                                                   const QExplicitlySharedDataPointer<DynamicContext> &) const
{
    const AbstractDuration::Ptr duration(o1.as<AbstractDuration>());
    const AbstractDuration::Value op2 = o2.as<AbstractDuration>()->value();

    switch(op)
    {
        case Div:
            return toItem(Decimal::fromValue(static_cast<xsDecimal>(duration->value()) / op2));
        case Substract:
            return duration->fromValue(duration->value() - op2);
        case Add:
            return duration->fromValue(duration->value() + op2);
        default:
        {
            Q_ASSERT(false);
            return Item(); /* Silence warning. */
        }
    }
}

OperandSwitcherMathematician::
OperandSwitcherMathematician(const AtomicMathematician::Ptr &mathematician) : m_mather(mathematician)
{
    Q_ASSERT(mathematician);
}

Item OperandSwitcherMathematician::calculate(const Item &o1,
                                                  const Operator op,
                                                  const Item &o2,
                                                  const QExplicitlySharedDataPointer<DynamicContext> &context) const
{
    return m_mather->calculate(o2, op, o1, context);
}


Item DateTimeDurationMathematician::calculate(const Item &o1,
                                                   const Operator op,
                                                   const Item &o2,
                                                   const QExplicitlySharedDataPointer<DynamicContext> &context) const
{
    Q_ASSERT(op == Substract || op == Add);

    const AbstractDateTime::Ptr adt(o1.as<AbstractDateTime>());
    const AbstractDuration::Ptr dur(o2.as<AbstractDuration>());
    QDateTime dt(adt->toDateTime());
    //pDebug() << "DateTimeDurationMathematician::calculate():" << dt.toString();
    //dt.setDateOnly(false);
    const qint8 sign = (op == Add ? 1 : -1) * (dur->isPositive() ? 1 : -1);

    // TODO milli seconds
    dt = dt.addSecs(sign * (dur->seconds() + dur->minutes() * 60 + dur->hours() * 60 * 60));
    dt = dt.addDays(sign * dur->days());
    dt = dt.addMonths(sign * dur->months());
    dt = dt.addYears(sign * dur->years());

    QString msg;

    if(AbstractDateTime::isRangeValid(dt.date(), msg))
        return adt->fromValue(dt);
    else
    {
        context->error(msg, ReportContext::FODT0001,
                       this);
        return Item();
    }
}

Item AbstractDateTimeMathematician::calculate(const Item &o1,
                                                   const Operator op,
                                                   const Item &o2,
                                                   const QExplicitlySharedDataPointer<DynamicContext> &) const
{
    Q_ASSERT(op == Substract || op == Add);
    QDateTime dt1(o1.as<AbstractDateTime>()->toDateTime());
    QDateTime dt2(o2.as<AbstractDateTime>()->toDateTime());
    /*
    */
    //dt1.setDateOnly(false);
    //dt2.setDateOnly(false);

    int diff = op == Add ? dt1.secsTo(dt2) : dt2.secsTo(dt1);

    return toItem(DayTimeDuration::fromSeconds(diff));
}

QT_END_NAMESPACE
