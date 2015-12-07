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

#include "qabstractduration_p.h"
#include "qabstractdatetime_p.h"
#include "qbase64binary_p.h"
#include "qboolean_p.h"
#include "qdynamiccontext_p.h"
#include "qqnamevalue_p.h"

#include "qatomiccomparators_p.h"

QT_BEGIN_NAMESPACE

using namespace QPatternist;

/* -------------------------------------------------- */
AtomicComparator::ComparisonResult
StringComparator::compare(const Item &o1,
                          const AtomicComparator::Operator,
                          const Item &o2) const
{
    const int result = QString::compare(o1.stringValue(), o2.stringValue());

    if(result > 0)
        return GreaterThan;
    else if(result < 0)
        return LessThan;
    else
    {
        Q_ASSERT(result == 0);
        return Equal;
    }
}

bool StringComparator::equals(const Item &o1,
                              const Item &o2) const
{
    return o1.stringValue() == o2.stringValue();
}
/* -------------------------------------------------- */

/* -------------------------------------------------- */
AtomicComparator::ComparisonResult
CaseInsensitiveStringComparator::compare(const Item &o1,
                                           const AtomicComparator::Operator,
                                           const Item &o2) const
{
    const QString i1(o1.stringValue().toLower());
    const QString i2(o2.stringValue().toLower());
    const int result = QString::compare(i1, i2);

    if(result > 0)
        return GreaterThan;
    else if(result < 0)
        return LessThan;
    else
    {
        Q_ASSERT(result == 0);
        return Equal;
    }
}

bool CaseInsensitiveStringComparator::equals(const Item &o1,
                                             const Item &o2) const
{
    const QString s1(o1.stringValue());
    const QString s2(o2.stringValue());

    return s1.length() == s2.length() &&
           s1.startsWith(s2, Qt::CaseInsensitive);
}
/* -------------------------------------------------- */

/* -------------------------------------------------- */
bool BinaryDataComparator::equals(const Item &o1,
                                  const Item &o2) const
{
    return o1.as<Base64Binary>()->asByteArray() ==
           o2.as<Base64Binary>()->asByteArray();
}
/* -------------------------------------------------- */

/* -------------------------------------------------- */
AtomicComparator::ComparisonResult
BooleanComparator::compare(const Item &o1,
                           const AtomicComparator::Operator,
                           const Item &o2) const
{
    /* We know Boolean::evaluateEBV doesn't use the DynamicContext. */
    const bool v1 = o1.as<AtomicValue>()->evaluateEBV(QExplicitlySharedDataPointer<DynamicContext>());
    const bool v2 = o2.as<AtomicValue>()->evaluateEBV(QExplicitlySharedDataPointer<DynamicContext>());

    if(v1 == v2)
        return Equal;
    else if(v1 == false)
    {
        Q_ASSERT(v2 == true);
        return LessThan;
    }
    else
    {
        Q_ASSERT(v1 == true && v2 == false);
        return GreaterThan;
    }
}

bool BooleanComparator::equals(const Item &o1,
                               const Item &o2) const
{
    /* Boolean is an atomic class. */
    return o1.as<AtomicValue>() == o2.as<AtomicValue>();
}
/* -------------------------------------------------- */

/* -------------------------------------------------- */
AtomicComparator::ComparisonResult
AbstractFloatComparator::compare(const Item &o1,
                                 const AtomicComparator::Operator op,
                                 const Item &o2) const
{
    const xsDouble v1 = o1.as<Numeric>()->toDouble();
    const xsDouble v2 = o2.as<Numeric>()->toDouble();

    if(Double::isEqual(v1, v2))
        return Equal;
    else if(v1 < v2)
        return LessThan;
    else if(v1 > v2)
        return GreaterThan;
    else
    {
        /* We have NaN values. Make sure we don't return a result which would
         * signify success for the operator in question. */
        if((op & OperatorGreaterThan) == OperatorGreaterThan)
            return LessThan;
        else
        {
            Q_ASSERT((op & OperatorLessThan) == OperatorLessThan);
            return GreaterThan;
        }
    }
}

bool AbstractFloatComparator::equals(const Item &o1,
                                     const Item &o2) const
{
    return Double::isEqual(o1.as<Numeric>()->toDouble(), o2.as<Numeric>()->toDouble());
}
/* -------------------------------------------------- */

/* -------------------------------------------------- */
AtomicComparator::ComparisonResult
DecimalComparator::compare(const Item &o1,
                           const AtomicComparator::Operator,
                           const Item &o2) const
{
    const xsDecimal v1 = o1.as<Numeric>()->toDecimal();
    const xsDecimal v2 = o2.as<Numeric>()->toDecimal();

    if(Double::isEqual(v1, v2))
        return Equal;
    else if(v1 < v2)
        return LessThan;
    else
        return GreaterThan;
}

bool DecimalComparator::equals(const Item &o1,
                               const Item &o2) const
{
    return Double::isEqual(o1.as<Numeric>()->toDecimal(), o2.as<Numeric>()->toDecimal());
}
/* -------------------------------------------------- */

/* -------------------------------------------------- */
AtomicComparator::ComparisonResult
IntegerComparator::compare(const Item &o1,
                           const AtomicComparator::Operator,
                           const Item &o2) const
{
    const Numeric *const num1 = o1.as<Numeric>();
    const Numeric *const num2 = o1.as<Numeric>();

    /**
     * Consider:
     *  xs:unsignedLong("100") > xs:unsignedLong("18446744073709551615")
     *
     * If we perform math on the values as if they were xsInteger, the right
     * operand overflows, wraps around, and the expression evaluates to false.
     * Hence we have this code to deal with it.
     *
     * This is runtime code, it would have been better if we had separate
     * AtomicComparator classes for signed and unsigned values, but the changes
     * required to the lookup code are extensive.
     */
    if(num1->isSigned() || num2->isSigned())
    {
        const xsInteger v1 = o1.as<Numeric>()->toInteger();
        const xsInteger v2 = o2.as<Numeric>()->toInteger();

        if(v1 == v2)
            return Equal;
        else if(v1 < v2)
            return LessThan;
        else
            return GreaterThan;
    }
    else
    {
        const qulonglong v1 = o1.as<Numeric>()->toUnsignedInteger();
        const qulonglong v2 = o2.as<Numeric>()->toUnsignedInteger();

        if(v1 == v2)
            return Equal;
        else if(v1 < v2)
            return LessThan;
        else
            return GreaterThan;
    }
}

bool IntegerComparator::equals(const Item &o1,
                               const Item &o2) const
{
    return o1.as<Numeric>()->toInteger() == o2.as<Numeric>()->toInteger();
}

/* -------------------------------------------------- */

/* -------------------------------------------------- */
bool QNameComparator::equals(const Item &o1,
                             const Item &o2) const
{
    return o1.as<QNameValue>()->m_qName ==
           o2.as<QNameValue>()->m_qName;
}
/* -------------------------------------------------- */

/* -------------------------------------------------- */
bool AbstractDateTimeComparator::equals(const Item &o1,
                                        const Item &o2) const
{
    const QDateTime dt1(o1.as<AbstractDateTime>()->toDateTime());
    const QDateTime dt2(o2.as<AbstractDateTime>()->toDateTime());

    /*
    pDebug() << "COMPARING:"
        << o1->as<AbstractDateTime>()->toDateTime().toString()
           << o2->as<AbstractDateTime>()->toDateTime().toString();
    pDebug() << "DATE ONLY:"
        << o1->as<AbstractDateTime>()->toDateTime().isDateOnly()
           << o2->as<AbstractDateTime>()->toDateTime().isDateOnly();
           */
    return dt1 == dt2 &&
           dt1.timeSpec() == dt2.timeSpec();
}

AtomicComparator::ComparisonResult
AbstractDateTimeComparator::compare(const Item &operand1,
                                    const AtomicComparator::Operator,
                                    const Item &operand2) const
{
    const QDateTime &dt1 = operand1.as<AbstractDateTime>()->toDateTime();
    const QDateTime &dt2 = operand2.as<AbstractDateTime>()->toDateTime();

    if(dt1 == dt2)
        return Equal;
    else if(dt1 < dt2)
        return LessThan;
    else
        return GreaterThan;
}
/* -------------------------------------------------- */

/* -------------------------------------------------- */
bool AbstractDurationComparator::equals(const Item &o1,
                                        const Item &o2) const
{
    /* We use AbstractDuration::operator==() */
    return *o1.as<AbstractDuration>() ==
           *o2.as<AbstractDuration>();
}

QDateTime AbstractDurationComparator::addDurationToDateTime(const QDateTime &dateTime,
                                                            const AbstractDuration *const duration)
{
    QDateTime result(dateTime);
    qint64 seconds = 0;

    const qint8 signMultiplier = (duration->isPositive() ? 1 : -1);

    result = result.addYears(signMultiplier * duration->years());
    result = result.addMonths(signMultiplier * duration->months());
    result = result.addDays(signMultiplier * duration->days());

    seconds =  60 * 60 * duration->hours();
    seconds += 60 * duration->minutes();
    seconds += duration->seconds();

    result = result.addSecs(signMultiplier * seconds);
    result = result.addMSecs(signMultiplier * duration->mseconds());

    return result;
}

AtomicComparator::ComparisonResult
AbstractDurationComparator::compare(const Item &o1,
                                    const AtomicComparator::Operator,
                                    const Item &o2) const
{
    const AbstractDuration *const duration = o1.as<AbstractDuration>();
    const AbstractDuration *const otherDuration = o2.as<AbstractDuration>();

    const QDateTime dateTime1(QDate(1696, 9, 1), QTime(0, 0, 0), Qt::UTC);
    const QDateTime dateTime2(QDate(1697, 2, 1), QTime(0, 0, 0), Qt::UTC);
    const QDateTime dateTime3(QDate(1903, 3, 1), QTime(0, 0, 0), Qt::UTC);
    const QDateTime dateTime4(QDate(1903, 7, 1), QTime(0, 0, 0), Qt::UTC);

    const QDateTime durationDateTime1 = addDurationToDateTime(dateTime1, duration);
    const QDateTime durationDateTime2 = addDurationToDateTime(dateTime2, duration);
    const QDateTime durationDateTime3 = addDurationToDateTime(dateTime3, duration);
    const QDateTime durationDateTime4 = addDurationToDateTime(dateTime4, duration);

    const QDateTime otherDurationDateTime1 = addDurationToDateTime(dateTime1, otherDuration);
    const QDateTime otherDurationDateTime2 = addDurationToDateTime(dateTime2, otherDuration);
    const QDateTime otherDurationDateTime3 = addDurationToDateTime(dateTime3, otherDuration);
    const QDateTime otherDurationDateTime4 = addDurationToDateTime(dateTime4, otherDuration);

    if (durationDateTime1 > otherDurationDateTime1 &&
        durationDateTime2 > otherDurationDateTime2 &&
        durationDateTime3 > otherDurationDateTime3 &&
        durationDateTime4 > otherDurationDateTime4) {
        return GreaterThan;
    } else if (durationDateTime1 < otherDurationDateTime1 &&
               durationDateTime2 < otherDurationDateTime2 &&
               durationDateTime3 < otherDurationDateTime3 &&
               durationDateTime4 < otherDurationDateTime4) {
        return LessThan;
    } else if (*duration == *otherDuration) {
        return Equal;
    } else {
        return Incomparable;
    }
}

/* -------------------------------------------------- */
QT_END_NAMESPACE
