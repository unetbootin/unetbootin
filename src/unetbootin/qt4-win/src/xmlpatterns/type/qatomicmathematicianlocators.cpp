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

#include "qabstractfloatmathematician_p.h"
#include "qatomicmathematicianlocators_p.h"
#include "qatomicmathematicians_p.h"

QT_BEGIN_NAMESPACE

using namespace QPatternist;

#define implMathVisit(ownerClass, visitor, mather, validOps)                            \
AtomicTypeVisitorResult::Ptr                                                            \
ownerClass##MathematicianLocator::visit(const visitor *, const qint16 op,               \
                                        const SourceLocationReflection *const r) const  \
{                                                                                       \
    Q_UNUSED(r)                                                                         \
    /* Note the extra paranteses around validOps. */                                    \
    if(((validOps) & AtomicMathematician::Operator(op)) == op)                                                         \
        return AtomicTypeVisitorResult::Ptr(new mather());                              \
    else                                                                                \
        return AtomicTypeVisitorResult::Ptr();                                          \
}

#define implReportingMathVisit(ownerClass, visitor, mather, validOps)                   \
AtomicTypeVisitorResult::Ptr                                                            \
ownerClass##MathematicianLocator::visit(const visitor *, const qint16 op,               \
                                        const SourceLocationReflection *const r) const  \
{                                                                                       \
    /* Note the extra paranteses around validOps. */                                    \
    if(((validOps) & AtomicMathematician::Operator(op)) == op)                                                         \
        return AtomicTypeVisitorResult::Ptr(new mather(r));                             \
    else                                                                                \
        return AtomicTypeVisitorResult::Ptr();                                          \
}

#define implRevReportingMathVisit(ownerClass, visitor, mather, validOps)                \
AtomicTypeVisitorResult::Ptr                                                            \
ownerClass##MathematicianLocator::visit(const visitor *, const qint16 op,               \
                                        const SourceLocationReflection *const r) const  \
{                                                                                       \
    /* Note the extra paranteses around validOps. */                                    \
    if(((validOps) & AtomicMathematician::Operator(op)) == op)                                                         \
        return AtomicTypeVisitorResult::Ptr(new OperandSwitcherMathematician(           \
                                            AtomicMathematician::Ptr(new mather(r))));  \
    else                                                                                \
        return AtomicTypeVisitorResult::Ptr();                                          \
}

static const AtomicMathematician::Operators AllMathOperators(AtomicMathematician::Add       |
                                                             AtomicMathematician::Div       |
                                                             AtomicMathematician::IDiv      |
                                                             AtomicMathematician::Mod       |
                                                             AtomicMathematician::Multiply  |
                                                             AtomicMathematician::Substract);

static const AtomicMathematician::Operators DivMultiply(AtomicMathematician::Multiply       |
                                                        AtomicMathematician::Div);

static const AtomicMathematician::Operators DurationOps(AtomicMathematician::Div            |
                                                        AtomicMathematician::Substract      |
                                                        AtomicMathematician::Add);

static const AtomicMathematician::Operators DTOps(AtomicMathematician::Substract            |
                                                  AtomicMathematician::Add);

implReportingMathVisit(Double,           DecimalType,            DoubleMathematician,    AllMathOperators)
implReportingMathVisit(Double,           DoubleType,             DoubleMathematician,    AllMathOperators)
implReportingMathVisit(Double,           FloatType,              DoubleMathematician,    AllMathOperators)
implReportingMathVisit(Double,           IntegerType,            DoubleMathematician,    AllMathOperators)
implRevReportingMathVisit(Double,           YearMonthDurationType,  DurationNumericMathematician,  AtomicMathematician::Multiply)
implRevReportingMathVisit(Double,           DayTimeDurationType,    DurationNumericMathematician,  AtomicMathematician::Multiply)

implReportingMathVisit(Float,            DecimalType,            FloatMathematician,     AllMathOperators)
implReportingMathVisit(Float,            DoubleType,    DoubleMathematician,    AllMathOperators)
implReportingMathVisit(Float,            FloatType,              FloatMathematician,     AllMathOperators)
implReportingMathVisit(Float,            IntegerType,            FloatMathematician,     AllMathOperators)
implRevReportingMathVisit(Float,            YearMonthDurationType,  DurationNumericMathematician,  AtomicMathematician::Multiply)
implRevReportingMathVisit(Float,            DayTimeDurationType,    DurationNumericMathematician,  AtomicMathematician::Multiply)

implReportingMathVisit(Decimal, DecimalType,            DecimalMathematician,   AllMathOperators)
implReportingMathVisit(Decimal,          DoubleType,    DoubleMathematician,    AllMathOperators)
implReportingMathVisit(Decimal,          FloatType,              FloatMathematician,     AllMathOperators)
implReportingMathVisit(Decimal, IntegerType,            DecimalMathematician,   AllMathOperators)
implRevReportingMathVisit(Decimal,          YearMonthDurationType,  DurationNumericMathematician,  AtomicMathematician::Multiply)
implRevReportingMathVisit(Decimal,          DayTimeDurationType,    DurationNumericMathematician,  AtomicMathematician::Multiply)

implReportingMathVisit(Integer, DecimalType,            DecimalMathematician,   AllMathOperators)
implReportingMathVisit(Integer,          DoubleType,    DoubleMathematician,    AllMathOperators)
implReportingMathVisit(Integer,          FloatType,              FloatMathematician,     AllMathOperators)
implReportingMathVisit(Integer, IntegerType,            IntegerMathematician,   AllMathOperators)
implRevReportingMathVisit(Integer,          YearMonthDurationType,  DurationNumericMathematician,  AtomicMathematician::Multiply)
implRevReportingMathVisit(Integer,          DayTimeDurationType,    DurationNumericMathematician,  AtomicMathematician::Multiply)

implRevReportingMathVisit(DayTimeDuration,  DateTimeType,           DateTimeDurationMathematician,       AtomicMathematician::Add)
implRevReportingMathVisit(DayTimeDuration,  DateType,               DateTimeDurationMathematician,       AtomicMathematician::Add)
implMathVisit(DayTimeDuration,  DayTimeDurationType,    DurationDurationMathematician, DurationOps)
implReportingMathVisit(DayTimeDuration,  DecimalType,   DurationNumericMathematician,  DivMultiply)
implReportingMathVisit(DayTimeDuration,  DoubleType,    DurationNumericMathematician,  DivMultiply)
implReportingMathVisit(DayTimeDuration,  FloatType,     DurationNumericMathematician,  DivMultiply)
implReportingMathVisit(DayTimeDuration,  IntegerType,   DurationNumericMathematician,  DivMultiply)
implRevReportingMathVisit(DayTimeDuration,  SchemaTimeType,               DateTimeDurationMathematician,       AtomicMathematician::Add)

implRevReportingMathVisit(YearMonthDuration, DateTimeType,          DateTimeDurationMathematician,       AtomicMathematician::Add)
implRevReportingMathVisit(YearMonthDuration, DateType,              DateTimeDurationMathematician,       AtomicMathematician::Add)
implReportingMathVisit(YearMonthDuration, DecimalType,  DurationNumericMathematician,  DivMultiply)
implReportingMathVisit(YearMonthDuration, DoubleType,   DurationNumericMathematician,  DivMultiply)
implReportingMathVisit(YearMonthDuration, FloatType,    DurationNumericMathematician,  DivMultiply)
implReportingMathVisit(YearMonthDuration, IntegerType,  DurationNumericMathematician,  DivMultiply)
implMathVisit(YearMonthDuration, YearMonthDurationType, DurationDurationMathematician, DurationOps)

implMathVisit(Date,              DateType,              AbstractDateTimeMathematician,
              AtomicMathematician::Substract)
implReportingMathVisit(Date,     YearMonthDurationType, DateTimeDurationMathematician,       DTOps)
implReportingMathVisit(Date,     DayTimeDurationType,   DateTimeDurationMathematician,       DTOps)

implMathVisit(SchemaTime,              SchemaTimeType,              AbstractDateTimeMathematician,
              AtomicMathematician::Substract)
implReportingMathVisit(SchemaTime,     DayTimeDurationType,   DateTimeDurationMathematician,       DTOps)

implMathVisit(DateTime,          DateTimeType,          AbstractDateTimeMathematician,
              AtomicMathematician::Substract)
implReportingMathVisit(DateTime, YearMonthDurationType, DateTimeDurationMathematician,       DTOps)
implReportingMathVisit(DateTime, DayTimeDurationType,   DateTimeDurationMathematician,       DTOps)

#undef implMathVisit
#undef implReportingMathVisit
#undef implRevReportingMathVisit

QT_END_NAMESPACE
