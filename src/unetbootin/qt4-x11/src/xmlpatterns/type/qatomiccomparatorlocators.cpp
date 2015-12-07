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

#include "qatomiccomparators_p.h"

#include "qatomiccomparatorlocators_p.h"

QT_BEGIN_NAMESPACE

using namespace QPatternist;

static const AtomicComparator::Operators AllCompOperators(AtomicComparator::OperatorNotEqual            |
                                                          AtomicComparator::OperatorGreaterOrEqual      |
                                                          AtomicComparator::OperatorLessOrEqual         |
                                                          AtomicComparator::OperatorLessThanNaNLeast    |
                                                          AtomicComparator::OperatorLessThanNaNGreatest);
/* --------------------------------------------------------------- */
#define addVisitor(owner, type, comp, validOps)                                 \
AtomicTypeVisitorResult::Ptr                                                    \
owner##ComparatorLocator::visit(const type *,                                   \
                                const qint16 op,                                \
                                const SourceLocationReflection *const) const    \
{                                                                               \
    /* Note the extra paranteses around validOps. */                            \
    if(((validOps) & AtomicComparator::Operator(op)) == op)                     \
        return AtomicTypeVisitorResult::Ptr(new comp());                        \
    else                                                                        \
        return AtomicTypeVisitorResult::Ptr();                                  \
}
/* --------------------------------------------------------------- */
#define visitorForDouble(owner, type)                                                                                           \
AtomicTypeVisitorResult::Ptr                                                                                                    \
owner##ComparatorLocator::visit(const type *,                                                                                   \
                                const qint16 op,                                                                                \
                                const SourceLocationReflection *const) const                                                    \
{                                                                                                                               \
    if(((AtomicComparator::OperatorNotEqual        |                                                                            \
         AtomicComparator::OperatorGreaterOrEqual  |                                                                            \
         AtomicComparator::OperatorLessOrEqual) & AtomicComparator::Operator(op)) == op)                                        \
        return AtomicTypeVisitorResult::Ptr(new AbstractFloatComparator());                                                     \
    else if(op == AtomicComparator::OperatorLessThanNaNLeast)                                                                   \
        return AtomicTypeVisitorResult::Ptr(new AbstractFloatSortComparator<AtomicComparator::OperatorLessThanNaNLeast>());     \
    else if(op == AtomicComparator::OperatorLessThanNaNGreatest)                                                                \
        return AtomicTypeVisitorResult::Ptr(new AbstractFloatSortComparator<AtomicComparator::OperatorLessThanNaNGreatest>());  \
    else                                                                                                                        \
        return AtomicTypeVisitorResult::Ptr();                                                                                  \
}
/* --------------------------------------------------------------- */

/* ----------- xs:string, xs:anyURI, xs:untypedAtomic  ----------- */
addVisitor(String,  StringType,         StringComparator,
           AllCompOperators)
addVisitor(String,  UntypedAtomicType,  StringComparator,
           AllCompOperators)
addVisitor(String,  AnyURIType,         StringComparator,
           AllCompOperators)
/* --------------------------------------------------------------- */

/* ------------------------- xs:hexBinary ------------------------ */
addVisitor(HexBinary,   HexBinaryType,        BinaryDataComparator,
           AtomicComparator::OperatorEqual |
           AtomicComparator::OperatorNotEqual)
/* --------------------------------------------------------------- */

/* ----------------------- xs:base64Binary ----------------------- */
addVisitor(Base64Binary,    Base64BinaryType,    BinaryDataComparator,
           AtomicComparator::OperatorEqual |
           AtomicComparator::OperatorNotEqual)
/* --------------------------------------------------------------- */

/* -------------------------- xs:boolean ------------------------- */
addVisitor(Boolean,     BooleanType,        BooleanComparator,
           AllCompOperators)
/* --------------------------------------------------------------- */

/* -------------------------- xs:double -------------------------- */
visitorForDouble(Double,      DoubleType)
visitorForDouble(Double,      FloatType)
visitorForDouble(Double,      DecimalType)
visitorForDouble(Double,      IntegerType)
/* --------------------------------------------------------------- */

/* --------------------------- xs:float -------------------------- */
visitorForDouble(Float,   DoubleType)
visitorForDouble(Float,   FloatType)
visitorForDouble(Float,   DecimalType)
visitorForDouble(Float,   IntegerType)
/* --------------------------------------------------------------- */

/* -------------------------- xs:decimal ------------------------- */
visitorForDouble(Decimal,     DoubleType)
visitorForDouble(Decimal,     FloatType)
addVisitor(Decimal,     DecimalType,    DecimalComparator,
           AllCompOperators)
addVisitor(Decimal,     IntegerType,    DecimalComparator,
           AllCompOperators)
/* --------------------------------------------------------------- */

/* ------------------------- xs:integer -------------------------- */
visitorForDouble(Integer,     DoubleType)
visitorForDouble(Integer,     FloatType)
addVisitor(Integer,     DecimalType,    DecimalComparator,
           AllCompOperators)
addVisitor(Integer,     IntegerType,    IntegerComparator,
           AllCompOperators)
/* --------------------------------------------------------------- */

/* -------------------------- xs:QName --------------------------- */
addVisitor(QName,       QNameType,          QNameComparator,
           AtomicComparator::OperatorEqual     |
           AtomicComparator::OperatorNotEqual)
/* --------------------------------------------------------------- */

/* -------------------------- xs:gYear --------------------------- */
addVisitor(GYear,       GYearType,          AbstractDateTimeComparator,
           AtomicComparator::OperatorEqual     |
           AtomicComparator::OperatorNotEqual)
/* --------------------------------------------------------------- */

/* -------------------------- xs:gDay ---------------------------- */
addVisitor(GDay,        GDayType,           AbstractDateTimeComparator,
           AtomicComparator::OperatorEqual     |
           AtomicComparator::OperatorNotEqual)
/* --------------------------------------------------------------- */

/* -------------------------- xs:gMonth -------------------------- */
addVisitor(GMonth,      GMonthType,         AbstractDateTimeComparator,
           AtomicComparator::OperatorEqual     |
           AtomicComparator::OperatorNotEqual)
/* --------------------------------------------------------------- */

/* ------------------------ xs:gYearMonth ------------------------ */
addVisitor(GYearMonth,  GYearMonthType,     AbstractDateTimeComparator,
           AtomicComparator::OperatorEqual     |
           AtomicComparator::OperatorNotEqual)
/* --------------------------------------------------------------- */

/* ------------------------ xs:gMonthDay ------------------------- */
addVisitor(GMonthDay,   GMonthDayType,      AbstractDateTimeComparator,
           AtomicComparator::OperatorEqual     |
           AtomicComparator::OperatorNotEqual)
/* --------------------------------------------------------------- */

/* ------------------------ xs:dateTime -------------------------- */
addVisitor(DateTime,    DateTimeType,    AbstractDateTimeComparator,
           AllCompOperators)
/* --------------------------------------------------------------- */

/* -------------------------- xs:time ---------------------------- */
addVisitor(SchemaTime,        SchemaTimeType,       AbstractDateTimeComparator,
           AllCompOperators)
/* --------------------------------------------------------------- */

/* -------------------------- xs:date ---------------------------- */
addVisitor(Date,        DateType,       AbstractDateTimeComparator,
           AllCompOperators)
/* --------------------------------------------------------------- */

/* ------------------------ xs:duration -------------------------- */
addVisitor(Duration,        DayTimeDurationType,        AbstractDurationComparator,
           AtomicComparator::OperatorEqual     |
           AtomicComparator::OperatorNotEqual)
addVisitor(Duration,        DurationType,               AbstractDurationComparator,
           AtomicComparator::OperatorEqual     |
           AtomicComparator::OperatorNotEqual)
addVisitor(Duration,        YearMonthDurationType,      AbstractDurationComparator,
           AtomicComparator::OperatorEqual     |
           AtomicComparator::OperatorNotEqual)
/* --------------------------------------------------------------- */

/* ------------------ xs:dayTimeDuration ------------------------ */
addVisitor(DayTimeDuration,     DayTimeDurationType,    AbstractDurationComparator,
           AllCompOperators)
addVisitor(DayTimeDuration,     DurationType,           AbstractDurationComparator,
           AtomicComparator::OperatorEqual     |
           AtomicComparator::OperatorNotEqual)
addVisitor(DayTimeDuration,     YearMonthDurationType,  AbstractDurationComparator,
           AtomicComparator::OperatorEqual     |
           AtomicComparator::OperatorNotEqual)
/* --------------------------------------------------------------- */

/* ------------------- xs:yearMonthDuration --------------------- */
addVisitor(YearMonthDuration,   DayTimeDurationType,    AbstractDurationComparator,
           AtomicComparator::OperatorEqual     |
           AtomicComparator::OperatorNotEqual)
addVisitor(YearMonthDuration,   DurationType,           AbstractDurationComparator,
           AtomicComparator::OperatorEqual     |
           AtomicComparator::OperatorNotEqual)
addVisitor(YearMonthDuration,   YearMonthDurationType,  AbstractDurationComparator,
           AllCompOperators)
/* --------------------------------------------------------------- */
#undef addVisitor

QT_END_NAMESPACE
