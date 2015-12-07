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

#ifndef Patternist_IndexOfIterator_H
#define Patternist_IndexOfIterator_H

#include "qitem_p.h"
#include "qatomiccomparator_p.h"
#include "qcomparisonplatform_p.h"
#include "qdynamiccontext_p.h"
#include "qexpression_p.h"

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

namespace QPatternist
{
    /**
     * @short Forms an QAbstractXmlForwardIterator over a sequence of integers, which each is the position
     * of where a search parameter appeared in another QAbstractXmlForwardIterator.
     *
     * @see <a href="http://www.w3.org/TR/xpath-functions/#func-index-of">XQuery 1.0
     * and XPath 2.0 Functions and Operators, 15.1.3 fn:index-of</a>
     * @author Frans Englich <fenglich@trolltech.com>
     * @ingroup Patternist_iterators
     */
    class IndexOfIterator : public Item::Iterator
                          , public ComparisonPlatform<IndexOfIterator, false>
                          , public SourceLocationReflection
    {
    public:

        /**
         * Creates an IndexOfIterator, whose next() function returns integers being
         * the index positions of where @p searchParam was found in @p inputSequence.
         *
         * @param comp the AtomicComparator to be used for comparing values. This may be @c null,
         * meaning the IndexOfIterator iterator will dynamically determine what comparator to use
         * on an item per item basis, which is slower.
         * @param searchParam the item which should be compared to the items in @p inputSequence.
         * @param inputSequence the input sequence which indexes of the @p searchParam should
         * be returned for.
         * @param context the usual DynamicContext
         * @param expr the Expression that this IndexOfIterator is evaluating
         * for. It is used for error reporting, via actualReflection().
         */
        IndexOfIterator(const Item::Iterator::Ptr &inputSequence,
                        const Item &searchParam,
                        const AtomicComparator::Ptr &comp,
                        const DynamicContext::Ptr &context,
                        const Expression::ConstPtr &expr);

        virtual Item next();
        virtual Item current() const;
        virtual xsInteger position() const;
        virtual Item::Iterator::Ptr copy() const;

        inline AtomicComparator::Operator operatorID() const
        {
            return AtomicComparator::OperatorEqual;
        }

        virtual const SourceLocationReflection *actualReflection() const;

    private:
        const Item::Iterator::Ptr   m_seq;
        const Item                  m_searchParam;
        const DynamicContext::Ptr   m_context;
        const Expression::ConstPtr  m_expr;
        Item                        m_current;
        xsInteger                   m_position;
        xsInteger                   m_seqPos;
    };
}

QT_END_NAMESPACE

QT_END_HEADER

#endif
