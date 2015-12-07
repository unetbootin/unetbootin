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

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.

#ifndef Patternist_AtomicComparatorLocators_H
#define Patternist_AtomicComparatorLocators_H

#include "qatomiccomparatorlocator_p.h"

/**
 * @file
 * @short Contains AtomicComparatorLocator sub-classes that finds classes
 * which can compare atomic values.
 */

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

namespace QPatternist
{

    /**
     * @author Frans Englich <fenglich@trolltech.com>
     */
    class DoubleComparatorLocator : public AtomicComparatorLocator
    {
        using AtomicComparatorLocator::visit;
        virtual AtomicTypeVisitorResult::Ptr visit(const DoubleType *,
                                                   const qint16 op,
                                                   const SourceLocationReflection *const) const;
        virtual AtomicTypeVisitorResult::Ptr visit(const FloatType *,
                                                   const qint16 op,
                                                   const SourceLocationReflection *const) const;
        virtual AtomicTypeVisitorResult::Ptr visit(const DecimalType *,
                                                   const qint16 op,
                                                   const SourceLocationReflection *const) const;
        virtual AtomicTypeVisitorResult::Ptr visit(const IntegerType *,
                                                   const qint16 op,
                                                   const SourceLocationReflection *const) const;
    };

    /**
     * @author Frans Englich <fenglich@trolltech.com>
     */
    class FloatComparatorLocator : public AtomicComparatorLocator
    {
        using AtomicComparatorLocator::visit;
        virtual AtomicTypeVisitorResult::Ptr visit(const DoubleType *,
                                                   const qint16 op,
                                                   const SourceLocationReflection *const) const;
        virtual AtomicTypeVisitorResult::Ptr visit(const FloatType *,
                                                   const qint16 op,
                                                   const SourceLocationReflection *const) const;
        virtual AtomicTypeVisitorResult::Ptr visit(const DecimalType *,
                                                   const qint16 op,
                                                   const SourceLocationReflection *const) const;
        virtual AtomicTypeVisitorResult::Ptr visit(const IntegerType *,
                                                   const qint16 op,
                                                   const SourceLocationReflection *const) const;
    };

    /**
     * @author Frans Englich <fenglich@trolltech.com>
     */
    class DecimalComparatorLocator : public AtomicComparatorLocator
    {
        using AtomicComparatorLocator::visit;
        virtual AtomicTypeVisitorResult::Ptr visit(const DoubleType *,
                                                   const qint16 op,
                                                   const SourceLocationReflection *const) const;
        virtual AtomicTypeVisitorResult::Ptr visit(const FloatType *,
                                                   const qint16 op,
                                                   const SourceLocationReflection *const) const;
        virtual AtomicTypeVisitorResult::Ptr visit(const DecimalType *,
                                                   const qint16 op,
                                                   const SourceLocationReflection *const) const;
        virtual AtomicTypeVisitorResult::Ptr visit(const IntegerType *,
                                                   const qint16 op,
                                                   const SourceLocationReflection *const) const;
    };

    /**
     * @author Frans Englich <fenglich@trolltech.com>
     */
    class IntegerComparatorLocator : public AtomicComparatorLocator
    {
        using AtomicComparatorLocator::visit;
        virtual AtomicTypeVisitorResult::Ptr visit(const DoubleType *,
                                                   const qint16 op,
                                                   const SourceLocationReflection *const) const;
        virtual AtomicTypeVisitorResult::Ptr visit(const FloatType *,
                                                   const qint16 op,
                                                   const SourceLocationReflection *const) const;
        virtual AtomicTypeVisitorResult::Ptr visit(const DecimalType *,
                                                   const qint16 op,
                                                   const SourceLocationReflection *const) const;
        virtual AtomicTypeVisitorResult::Ptr visit(const IntegerType *,
                                                   const qint16 op,
                                                   const SourceLocationReflection *const) const;
    };

    /**
     * @author Frans Englich <fenglich@trolltech.com>
     */
    class BooleanComparatorLocator : public AtomicComparatorLocator
    {
        using AtomicComparatorLocator::visit;
        virtual AtomicTypeVisitorResult::Ptr visit(const BooleanType *,
                                                   const qint16 op,
                                                   const SourceLocationReflection *const) const;
    };

    /**
     * @author Frans Englich <fenglich@trolltech.com>
     */
    class Base64BinaryComparatorLocator : public AtomicComparatorLocator
    {
        using AtomicComparatorLocator::visit;
        virtual AtomicTypeVisitorResult::Ptr visit(const Base64BinaryType *,
                                                   const qint16 op,
                                                   const SourceLocationReflection *const) const;
    };

    /**
     * @author Frans Englich <fenglich@trolltech.com>
     */
    class HexBinaryComparatorLocator : public AtomicComparatorLocator
    {
        using AtomicComparatorLocator::visit;
        virtual AtomicTypeVisitorResult::Ptr visit(const HexBinaryType *,
                                                   const qint16 op,
                                                   const SourceLocationReflection *const) const;
    };

    /**
     * @author Frans Englich <fenglich@trolltech.com>
     */
    class QNameComparatorLocator : public AtomicComparatorLocator
    {
        using AtomicComparatorLocator::visit;
        virtual AtomicTypeVisitorResult::Ptr visit(const QNameType *,
                                                   const qint16 op,
                                                   const SourceLocationReflection *const) const;
    };

    /**
     * @author Frans Englich <fenglich@trolltech.com>
     */
    class StringComparatorLocator : public AtomicComparatorLocator
    {
        using AtomicComparatorLocator::visit;
        virtual AtomicTypeVisitorResult::Ptr visit(const StringType *,
                                                   const qint16 op,
                                                   const SourceLocationReflection *const) const;
        virtual AtomicTypeVisitorResult::Ptr visit(const UntypedAtomicType *,
                                                   const qint16 op,
                                                   const SourceLocationReflection *const) const;
        virtual AtomicTypeVisitorResult::Ptr visit(const AnyURIType *,
                                                   const qint16 op,
                                                   const SourceLocationReflection *const) const;
    };


    /**
     * @author Frans Englich <fenglich@trolltech.com>
     */
    class GYearComparatorLocator : public AtomicComparatorLocator
    {
        using AtomicComparatorLocator::visit;
        virtual AtomicTypeVisitorResult::Ptr visit(const GYearType *,
                                                   const qint16 op,
                                                   const SourceLocationReflection *const) const;
    };

    /**
     * @author Frans Englich <fenglich@trolltech.com>
     */
    class GMonthComparatorLocator : public AtomicComparatorLocator
    {
        using AtomicComparatorLocator::visit;
        virtual AtomicTypeVisitorResult::Ptr visit(const GMonthType *,
                                                   const qint16 op,
                                                   const SourceLocationReflection *const) const;
    };

    /**
     * @author Frans Englich <fenglich@trolltech.com>
     */
    class GYearMonthComparatorLocator : public AtomicComparatorLocator
    {
        using AtomicComparatorLocator::visit;
        virtual AtomicTypeVisitorResult::Ptr visit(const GYearMonthType *,
                                                   const qint16 op,
                                                   const SourceLocationReflection *const) const;
    };

    /**
     * @author Frans Englich <fenglich@trolltech.com>
     */
    class GMonthDayComparatorLocator : public AtomicComparatorLocator
    {
        using AtomicComparatorLocator::visit;
        virtual AtomicTypeVisitorResult::Ptr visit(const GMonthDayType *,
                                                   const qint16 op,
                                                   const SourceLocationReflection *const) const;
    };

    /**
     * @author Frans Englich <fenglich@trolltech.com>
     */
    class GDayComparatorLocator : public AtomicComparatorLocator
    {
        using AtomicComparatorLocator::visit;
        virtual AtomicTypeVisitorResult::Ptr visit(const GDayType *,
                                                   const qint16 op,
                                                   const SourceLocationReflection *const) const;
    };

    /**
     * @author Frans Englich <fenglich@trolltech.com>
     */
    class DateTimeComparatorLocator : public AtomicComparatorLocator
    {
        using AtomicComparatorLocator::visit;
        virtual AtomicTypeVisitorResult::Ptr visit(const DateTimeType *,
                                                   const qint16 op,
                                                   const SourceLocationReflection *const) const;
    };

    /**
     * @author Frans Englich <fenglich@trolltech.com>
     */
    class SchemaTimeComparatorLocator : public AtomicComparatorLocator
    {
        using AtomicComparatorLocator::visit;
        virtual AtomicTypeVisitorResult::Ptr visit(const SchemaTimeType *,
                                                   const qint16 op,
                                                   const SourceLocationReflection *const) const;
    };

    /**
     * @author Frans Englich <fenglich@trolltech.com>
     */
    class DateComparatorLocator : public AtomicComparatorLocator
    {
        using AtomicComparatorLocator::visit;
        virtual AtomicTypeVisitorResult::Ptr visit(const DateType *,
                                                   const qint16 op,
                                                   const SourceLocationReflection *const) const;
    };

    /**
     * @author Frans Englich <fenglich@trolltech.com>
     */
    class DurationComparatorLocator : public AtomicComparatorLocator
    {
        using AtomicComparatorLocator::visit;
        virtual AtomicTypeVisitorResult::Ptr visit(const DayTimeDurationType *,
                                                   const qint16 op,
                                                   const SourceLocationReflection *const) const;
        virtual AtomicTypeVisitorResult::Ptr visit(const DurationType *,
                                                   const qint16 op,
                                                   const SourceLocationReflection *const) const;
        virtual AtomicTypeVisitorResult::Ptr visit(const YearMonthDurationType *,
                                                   const qint16 op,
                                                   const SourceLocationReflection *const) const;
    };

    /**
     * @author Frans Englich <fenglich@trolltech.com>
     */
    class DayTimeDurationComparatorLocator : public AtomicComparatorLocator
    {
        using AtomicComparatorLocator::visit;
        virtual AtomicTypeVisitorResult::Ptr visit(const DayTimeDurationType *,
                                                   const qint16 op,
                                                   const SourceLocationReflection *const) const;
        virtual AtomicTypeVisitorResult::Ptr visit(const DurationType *,
                                                   const qint16 op,
                                                   const SourceLocationReflection *const) const;
        virtual AtomicTypeVisitorResult::Ptr visit(const YearMonthDurationType *,
                                                   const qint16 op,
                                                   const SourceLocationReflection *const) const;
    };

    /**
     * @author Frans Englich <fenglich@trolltech.com>
     */
    class YearMonthDurationComparatorLocator : public AtomicComparatorLocator
    {
        using AtomicComparatorLocator::visit;
        virtual AtomicTypeVisitorResult::Ptr visit(const DayTimeDurationType *,
                                                   const qint16 op,
                                                   const SourceLocationReflection *const) const;
        virtual AtomicTypeVisitorResult::Ptr visit(const DurationType *,
                                                   const qint16 op,
                                                   const SourceLocationReflection *const) const;
        virtual AtomicTypeVisitorResult::Ptr visit(const YearMonthDurationType *,
                                                   const qint16 op,
                                                   const SourceLocationReflection *const) const;
    };
}

QT_END_NAMESPACE

QT_END_HEADER

#endif
