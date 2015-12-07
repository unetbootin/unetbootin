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
