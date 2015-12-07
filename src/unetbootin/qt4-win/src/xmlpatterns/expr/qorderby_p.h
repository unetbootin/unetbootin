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

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.

#ifndef Patternist_OrderBy_H
#define Patternist_OrderBy_H

#include "qatomiccomparator_p.h"
#include "qcomparisonplatform_p.h"
#include "qsinglecontainer_p.h"

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

namespace QPatternist
{
    class ReturnOrderBy;

    /**
     * @short Performs the sorting by being a parent to ForClause.
     *
     * The child of the ForClause is a ReturnOrderBy expression, which collects
     * the sort keys and values.
     *
     * @author Frans Englich <fenglich@trolltech.com>
     * @ingroup Patternist_expressions
     */
    class OrderBy : public SingleContainer
    {
    public:
        enum Stability
        {
            StableOrder,
            UnstableOrder
        };

        /**
         * This class is value based.
         */
        class OrderSpec : public ComparisonPlatform<OrderBy::OrderSpec,
                                                    true, /* Yes, issue errors. */
                                                    AtomicComparator::AsValueComparison>
        {
        public:
            /**
             * We want this guy to be public.
             */
            using ComparisonPlatform<OrderBy::OrderSpec, true, AtomicComparator::AsValueComparison>::detailedFlexibleCompare;

            typedef QVector<OrderSpec> Vector;

            enum Direction
            {
                Ascending,
                Descending
            };

            /**
             * @short Default constructor, which is needed by QVector.
             */
            inline OrderSpec()
            {
            }

            inline OrderSpec(const Direction dir,
                             const StaticContext::OrderingEmptySequence orderingEmpty) : direction(dir),
                                                                                         orderingEmptySequence(orderingEmpty)
            {
            }

            void prepare(const Expression::Ptr &source,
                         const StaticContext::Ptr &context);

            const SourceLocationReflection *actualReflection() const
            {
                return m_expr.data();
            }

        private:
            Expression::Ptr m_expr;

        public:
	    /**
             * We place these afterwards, such that m_expr gets aligned at the
             * start of the address.
	     */
            Direction direction;

            StaticContext::OrderingEmptySequence orderingEmptySequence;

            inline AtomicComparator::Operator operatorID() const
            {
                return orderingEmptySequence == StaticContext::Least ? AtomicComparator::OperatorLessThanNaNLeast
                                                                     : AtomicComparator::OperatorLessThanNaNGreatest;
            }

        };

        OrderBy(const Stability stability,
                const OrderSpec::Vector &orderSpecs,
                const Expression::Ptr &operand,
                ReturnOrderBy *const returnOrderBy);

        virtual Item::Iterator::Ptr evaluateSequence(const DynamicContext::Ptr &context) const;
        virtual SequenceType::Ptr staticType() const;
        virtual Expression::Ptr typeCheck(const StaticContext::Ptr &context,
                                          const SequenceType::Ptr &reqType);
        virtual Expression::Ptr compress(const StaticContext::Ptr &context);
        virtual SequenceType::List expectedOperandTypes() const;
        virtual ExpressionVisitorResult::Ptr accept(const ExpressionVisitor::Ptr &visitor) const;

        inline Item::Iterator::Ptr mapToSequence(const Item &i,
                                                 const DynamicContext::Ptr &context) const;
    private:
        /**
         * Needed when calling makeSequenceMappingIterator().
         */
        typedef QExplicitlySharedDataPointer<const OrderBy> ConstPtr;

        const Stability             m_stability;
        OrderSpec::Vector           m_orderSpecs;
        ReturnOrderBy *const        m_returnOrderBy;
    };

    /* TODO Q_DECLARE_TYPEINFO(OrderBy::OrderSpec, Q_MOVABLE_TYPE); Breaks,
     * probably because it's nested. */
}

QT_END_NAMESPACE

QT_END_HEADER

#endif
