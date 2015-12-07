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
