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

#ifndef Patternist_ComparingAggregator_H
#define Patternist_ComparingAggregator_H

/**
 * @file qcomparingaggregator_p.h
 * @short Contains the implementations for the functions <tt>fn:max()</tt>, MaxFN,
 * and <tt>fn:min()</tt>, MinFN, and the class ComparingAggregator.
 */

#include "qabstractfloat_p.h"
#include "qdecimal_p.h"
#include "qcastingplatform_p.h"
#include "qcomparisonplatform_p.h"
#include "qliteral_p.h"
#include "qaggregator_p.h"
#include "quntypedatomicconverter_p.h"
#include "qpatternistlocale_p.h"

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

namespace QPatternist
{
    /**
     * @short Base class for the implementations of the <tt>fn:min()</tt> and <tt>fn:max()</tt> function.
     *
     * What function that more specifically is
     * followed, depends on how the constructor is called.
     *
     * @see MaxFN
     * @see MinFN
     * @ingroup Patternist_functions
     * @author Frans Englich <fenglich@trolltech.com>
     */
    template <AtomicComparator::Operator oper, AtomicComparator::ComparisonResult result>
    class ComparingAggregator : public Aggregator,
                                public ComparisonPlatform<ComparingAggregator<oper, result>,
                                                          true, AtomicComparator::AsValueComparison,
                                                          ReportContext::FORG0006>,
                                public CastingPlatform<ComparingAggregator<oper, result>, true>
    {
    public:
        virtual Item evaluateSingleton(const DynamicContext::Ptr &context) const;
        virtual Expression::Ptr typeCheck(const StaticContext::Ptr &context,
                                          const SequenceType::Ptr &reqType);

        inline AtomicComparator::Operator operatorID() const
        {
            return oper;
        }

        inline ItemType::Ptr targetType() const
        {
            return BuiltinTypes::xsDouble;
        }

    private:
        inline Item applyNumericPromotion(const Item &old,
                                               const Item &nev,
                                               const Item &newVal) const;

        using ComparisonPlatform<ComparingAggregator<oper, result>,
                                 true,
                                 AtomicComparator::AsValueComparison,
                                 ReportContext::FORG0006>::comparator;
        using ComparisonPlatform<ComparingAggregator<oper, result>,
                                 true,
                                 AtomicComparator::AsValueComparison,
                                 ReportContext::FORG0006>::fetchComparator;
        using CastingPlatform<ComparingAggregator<oper, result>, true>::cast;
    };

#include "qcomparingaggregator.cpp"

    /**
     * @short An instantiation of ComparingAggregator suitable for <tt>fn:max()</tt>.
     *
     * @ingroup Patternist_functions
     */
    typedef ComparingAggregator<AtomicComparator::OperatorGreaterThan, AtomicComparator::GreaterThan> MaxFN;

    /**
     * @short An instantiation of ComparingAggregator suitable for <tt>fn:max()</tt>.
     *
     * @ingroup Patternist_functions
     */
    typedef ComparingAggregator<AtomicComparator::OperatorLessThan, AtomicComparator::LessThan> MinFN;
}

QT_END_NAMESPACE

QT_END_HEADER

#endif
