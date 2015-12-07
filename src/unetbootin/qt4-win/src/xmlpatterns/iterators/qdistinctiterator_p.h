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

#ifndef Patternist_DistinctIterator_H
#define Patternist_DistinctIterator_H

#include <QList>

#include "qexpression_p.h"
#include "qitem_p.h"
#include "qatomiccomparator_p.h"
#include "qcomparisonplatform_p.h"
#include "qsourcelocationreflection_p.h"

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

namespace QPatternist
{

    /**
     * @short Filters another sequence by removing duplicates such that the items are unique.
     *
     * DistinctIterator takes an input sequence, and returns a sequence where each
     * item is unique. Thus, DistinctIterator removes the duplicates of items
     * in a sequence. DistinctIterator is central in the implementation of the
     * <tt>fn:distinct-values()</tt> function.
     *
     * @see <a href="http://www.w3.org/TR/xpath-functions/#func-distinct-values">XQuery 1.0
     * and XPath 2.0 Functions and Operators, 15.1.6 fn:distinct-values</a>
     * @author Frans Englich <fenglich@trolltech.com>
     * @ingroup Patternist_iterators
     */
    class DistinctIterator : public Item::Iterator
                           , public ComparisonPlatform<DistinctIterator, false>
                           , public SourceLocationReflection
    {
    public:
        /**
         * Creates a DistinctIterator.
         * @param comp the AtomicComparator to be used for comparing values. This may be @c null,
         * meaning the IndexOfIterator iterator will dynamically determine what comparator to use
         * @param seq the sequence whose duplicates should be filtered out
         * @param context the usual context, used for error reporting and by AtomicComparators.
         * @param expression the Expression that this DistinctIterator is
         * evaluating for. It is used for error reporting, via
         * actualReflection().
         */
        DistinctIterator(const Item::Iterator::Ptr &seq,
                         const AtomicComparator::Ptr &comp,
                         const Expression::ConstPtr &expression,
                         const DynamicContext::Ptr &context);

        virtual Item next();
        virtual Item current() const;
        virtual xsInteger position() const;
        virtual Item::Iterator::Ptr copy() const;
        virtual const SourceLocationReflection *actualReflection() const;

        inline AtomicComparator::Operator operatorID() const
        {
            return AtomicComparator::OperatorEqual;
        }

    private:
        const Item::Iterator::Ptr   m_seq;
        const DynamicContext::Ptr   m_context;
        const Expression::ConstPtr  m_expr;
        Item                        m_current;
        xsInteger                   m_position;
        Item::List                  m_processed;
    };
}

QT_END_NAMESPACE

QT_END_HEADER

#endif
