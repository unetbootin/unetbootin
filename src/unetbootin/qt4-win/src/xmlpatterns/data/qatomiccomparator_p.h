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

#ifndef Patternist_AtomicComparator_H
#define Patternist_AtomicComparator_H

#include <QFlags>

#include "qitem_p.h"
#include "qatomictypedispatch_p.h"

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

class QString;

namespace QPatternist
{

    /**
     * @short Base class for classes responsible of comparing two atomic values.
     *
     * This class is also known as the AtomicParrot.
     *
     * @ingroup Patternist_xdm
     * @author Frans Englich <fenglich@trolltech.com>
     */
    class Q_AUTOTEST_EXPORT AtomicComparator : public AtomicTypeVisitorResult
    {
    public:
        AtomicComparator();
        virtual ~AtomicComparator();

        typedef QExplicitlySharedDataPointer<AtomicComparator> Ptr;

        /**
         * Identifies operators used in value comparisons.
         *
         * The enum values are bit-significant.
         *
         * @see <a href="http://www.w3.org/TR/xpath20/#id-value-comparisons">W3C XML Path
         * Language (XPath) 2.0, 3.5.1 Value Comparisons</a>
         */
        enum Operator
        {
            /**
             * Operator <tt>eq</tt> and <tt>=</tt>.
             */
            OperatorEqual           = 1,

            /**
             * Operator <tt>ne</tt> and <tt>!=</tt>.
             */
            OperatorNotEqual        = 1 << 1,

            /**
             * Operator <tt>gt</tt> and <tt>\></tt>.
             */
            OperatorGreaterThan     = 1 << 2,

            /**
             * Operator <tt>lt</tt> and <tt>\<</tt>.
             */
            OperatorLessThan        = 1 << 3,

            /**
             * One of the operators we use for sorting. The only difference from
             * OperatorLessThan is that it sees NaN as ordered and smaller than
             * other numbers.
             */
            OperatorLessThanNaNLeast    = 1 << 4,

            /**
             * One of the operators we use for sorting. The only difference from
             * OperatorLessThanLeast is that it sees NaN as ordered and larger than
             * other numbers.
             */
            OperatorLessThanNaNGreatest    = 1 << 5,

            /**
             * Operator <tt>ge</tt> and <tt>\>=</tt>.
             */
            OperatorGreaterOrEqual  = OperatorEqual | OperatorGreaterThan,

            /**
             * Operator <tt>le</tt> and <tt>\<=</tt>.
             */
            OperatorLessOrEqual     = OperatorEqual | OperatorLessThan
        };

        typedef QFlags<Operator> Operators;

        /**
         * Signifies the result of a value comparison. This is used for value comparisons,
         * and in the future likely also for sorting.
         *
         * @see <a href="http://www.w3.org/TR/xpath20/#id-value-comparisons">W3C XML Path
         * Language (XPath) 2.0, 3.5.1 Value Comparisons</a>
         *
         */
        enum ComparisonResult
        {
            LessThan    = 1,
            Equal       = 2,
            GreaterThan = 4
        };

        /**
         * Compares @p op1 and @p op2 and determines the relationship between the two. This
         * is used for sorting and comparisons. The implementation performs an assert crash,
         * and must therefore be re-implemented if comparing the relevant values should be
         * possible.
         *
         * @param op1 the first operand
         * @param op the operator. How a comparison is carried out shouldn't depend on what the
         * operator is, but in some cases it is of interest.
         * @param op2 the second operand
         */
        virtual ComparisonResult compare(const Item &op1,
                                         const AtomicComparator::Operator op,
                                         const Item &op2) const;

        /**
         * Determines whether @p op1 and @p op2 are equal. It is the same as calling compare()
         * and checking whether the return value is Equal, but since comparison testing is such
         * a common operation, this specialized function exists.
         *
         * @returns true if @p op1 and @p op2 are equal.
         *
         * @param op1 the first operand
         * @param op2 the second operand
         */
        virtual bool equals(const Item &op1,
                            const Item &op2) const = 0;

        /**
         * Identifies the kind of comparison.
         */
        enum ComparisonType
        {
            /**
             * Identifies a general comparison; operator @c =, @c >, @c <=, and so on.
             */
            AsGeneralComparison = 1,

            /**
             * Identifies a value comparison; operator @c eq, @c lt, @c le, and so on.
             */
            AsValueComparison
        };

        /**
         * Utility function for getting the lexical representation for
         * the comparison operator @p op. Depending on the @p type argument,
         * the string returned is either a general comparison or a value comparison
         * operator.
         *
         * An example:
         * @include Example-AtomicComparator-displayName.cpp
         *
         * @param op the operator which the display name should be determined for.
         * @param type signifies whether the returned display name should be for
         * a value comparison or a general comparison. For example, if @p op is
         * OperatorEqual and @p type is AsValueComparision, "eq" is returned.
         */
        static QString displayName(const AtomicComparator::Operator op,
                                   const ComparisonType type);

    };
    Q_DECLARE_OPERATORS_FOR_FLAGS(AtomicComparator::Operators)
}

QT_END_NAMESPACE

QT_END_HEADER

#endif
