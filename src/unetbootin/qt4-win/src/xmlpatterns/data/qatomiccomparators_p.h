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

#ifndef Patternist_AtomicComparators_H
#define Patternist_AtomicComparators_H

#include "qabstractfloat_p.h"
#include "qatomiccomparator_p.h"
#include "qprimitives_p.h"

/**
 * @file
 * @short Contains all the classes implementing comparisons between atomic values.
 */

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

namespace QPatternist
{
    /**
     * @short Performs case @em sensitive string comparison
     * between @c xs:anyUri, @c xs:string, and @c xs:untypedAtomic.
     *
     * @ingroup Patternist_xdm
     * @author Frans Englich <fenglich@trolltech.com>
     */
    class StringComparator : public AtomicComparator
    {
    public:
        /**
         * Compares two strings, and returns the appropriate AtomicComparator::ComparisonResult enum. This
         * is a bit simplified version of string comparison as defined in the XPath specifications,
         * since this does not take any string collations into account(which an implementation neither
         * is required to do).
         *
         * @see <a href="http://www.w3.org/TR/xpath-functions/#string-compare">XQuery 1.0 and XPath
         * 2.0 Functions and Operators, 7.3 ValueComparison::Equality and Comparison of Strings</a>
         */
        virtual ComparisonResult compare(const Item &op1,
                                         const AtomicComparator::Operator op,
                                         const Item &op2) const;

        /**
         * Compares two strings, and returns @c true if they are considered equal as per
         * an ordinary string comparison. In other words, this is an implementation with
         * the Unicode code point collation.
         *
         * @see <a href="http://www.w3.org/TR/xpath-functions/#string-compare">XQuery 1.0 and XPath
         * 2.0 Functions and Operators, 7.3 ValueComparison::Equality and Comparison of Strings</a>
         */
        virtual bool equals(const Item &op1,
                            const Item &op2) const;
    };

    /**
     * @short Performs case @em insensitive string comparison
     * between @c xs:anyUri, @c xs:string, and @c xs:untypedAtomic.
     *
     * @ingroup Patternist_xdm
     * @author Frans Englich <fenglich@trolltech.com>
     */
    class CaseInsensitiveStringComparator : public AtomicComparator
    {
    public:
        /**
         * Converts both string values to upper case and afterwards compare them.
         */
        virtual ComparisonResult compare(const Item &op1,
                                         const AtomicComparator::Operator op,
                                         const Item &op2) const;

        /**
         * Converts both string values case insensitively.
         */
        virtual bool equals(const Item &op1,
                            const Item &op2) const;
    };

    /**
     * @short Compares @c xs:base64Binary and @c xs:hexBinary values.
     *
     * @author Frans Englich <fenglich@trolltech.com>
     */
    class BinaryDataComparator : public AtomicComparator
    {
    public:
        virtual bool equals(const Item &op1,
                            const Item &op2) const;
    };

    /**
     * @short Compares @c xs:boolean values.
     *
     * This is done via the object's Boolean::evaluteEBV() function.
     *
     * @author Frans Englich <fenglich@trolltech.com>
     */
    class BooleanComparator : public AtomicComparator
    {
    public:
        virtual ComparisonResult compare(const Item &op1,
                                         const AtomicComparator::Operator op,
                                         const Item &op2) const;

        virtual bool equals(const Item &op1,
                            const Item &op2) const;
    };

    /**
     * @short Compares @c xs:double values.
     *
     * @todo Add docs about numeric promotion
     *
     * @author Frans Englich <fenglich@trolltech.com>
     */
    class AbstractFloatComparator : public AtomicComparator
    {
    public:
        virtual ComparisonResult compare(const Item &op1,
                                         const AtomicComparator::Operator op,
                                         const Item &op2) const;

        virtual bool equals(const Item &op1,
                            const Item &op2) const;
    };

    /**
     * @short Compares @c xs:double values for the purpose of sorting.
     *
     * @todo Add docs about numeric promotion
     *
     * @author Frans Englich <fenglich@trolltech.com>
     */
    template<const AtomicComparator::Operator t_op>
    class AbstractFloatSortComparator : public AbstractFloatComparator
    {
    public:
        virtual ComparisonResult compare(const Item &o1,
                                         const AtomicComparator::Operator op,
                                         const Item &o2) const
        {
            Q_ASSERT_X(t_op == OperatorLessThanNaNLeast || t_op == OperatorLessThanNaNGreatest, Q_FUNC_INFO,
                       "Can only be instantiated with those two.");
            Q_ASSERT(op == t_op);
            Q_UNUSED(op); /* Needed when building in release mode. */

            const xsDouble v1 = o1.template as<Numeric>()->toDouble();
            const xsDouble v2 = o2.template as<Numeric>()->toDouble();

            if(qIsNaN(v1) && !qIsNaN(v2))
                return t_op == OperatorLessThanNaNLeast ? LessThan : GreaterThan;
            if(!qIsNaN(v1) && qIsNaN(v2))
                return t_op == OperatorLessThanNaNLeast ? GreaterThan : LessThan;

            if(Double::isEqual(v1, v2))
                return Equal;
            else if(v1 < v2)
                return LessThan;
            else
                return GreaterThan;
        }

    };

    /**
     * @short Compares @c xs:decimal values.
     *
     * @author Frans Englich <fenglich@trolltech.com>
     */
    class DecimalComparator : public AtomicComparator
    {
    public:
        virtual ComparisonResult compare(const Item &op1,
                                         const AtomicComparator::Operator op,
                                         const Item &op2) const;

        virtual bool equals(const Item &op1,
                            const Item &op2) const;
    };

    /**
     * @short Compares @c xs:integer values.
     *
     * @author Frans Englich <fenglich@trolltech.com>
     */
    class IntegerComparator : public AtomicComparator
    {
    public:
        virtual ComparisonResult compare(const Item &op1,
                                         const AtomicComparator::Operator op,
                                         const Item &op2) const;

        virtual bool equals(const Item &op1,
                            const Item &op2) const;
    };

    /**
     * @short Compares @c xs:QName values.
     *
     * @author Frans Englich <fenglich@trolltech.com>
     */
    class QNameComparator : public AtomicComparator
    {
    public:
        virtual bool equals(const Item &op1,
                            const Item &op2) const;
    };

    /**
     * @short Compares sub-classes of AbstractDateTime.
     *
     * @author Frans Englich <fenglich@trolltech.com>
     */
    class AbstractDateTimeComparator : public AtomicComparator
    {
    public:
        virtual ComparisonResult compare(const Item &op1,
                                         const AtomicComparator::Operator op,
                                         const Item &op2) const;
        virtual bool equals(const Item &op1,
                            const Item &op2) const;
    };

    /**
     * @short Compares sub-classes of AbstractDuration.
     *
     * @author Frans Englich <fenglich@trolltech.com>
     */
    class AbstractDurationComparator : public AtomicComparator
    {
    public:
        virtual ComparisonResult compare(const Item &op1,
                                         const AtomicComparator::Operator op,
                                         const Item &op2) const;
        virtual bool equals(const Item &op1,
                            const Item &op2) const;
    };
}

QT_END_NAMESPACE

QT_END_HEADER

#endif
