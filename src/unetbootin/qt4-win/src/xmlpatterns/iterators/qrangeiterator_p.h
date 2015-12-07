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

#ifndef Patternist_RangeIterator_H
#define Patternist_RangeIterator_H

#include "qitem_p.h"

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

namespace QPatternist
{
    /**
     * @short RangeIterator represents a sequence of integers between a
     * start and end value.
     *
     * The RangeIterator contains the evaluation logic for the range expression, <tt>N to M</tt>,
     * and its behavior is therefore consistent with the definition of that XPath expression.
     * Hence, the detailed behavior of the RangeIterator can be found in the XPath 2.0
     * specification.
     *
     * @see <a href="http://www.w3.org/TR/xpath20/\#doc-xpath-RangeExpr">XML Path Language
     * (XPath) 2.0, 3.3 Sequence Expressions, RangeExpr</a>
     * @see RangeExpression
     * @author Frans Englich <fenglich@trolltech.com>
     * @ingroup Patternist_iterators
     * @todo Documentation is missing
     */
    class Q_AUTOTEST_EXPORT RangeIterator : public Item::Iterator
    {
    public:

        /**
         * RangeIterator can iterate in both directions.
         * This enumerator exist for identifying different directions.
         */
        enum Direction
        {
            /**
             * Signifies that the QAbstractXmlForwardIterator operates in a reverse direction, where the
             * first item returned by the next() function is from the beginning of the
             * source sequence.
             */
            Backward = 0,

            /**
             * Signifies the forward direction. Iterators do conceptually operate
             * in the forward direction by default.
             */
            Forward = 1
        };

        /**
         * Creates an QAbstractXmlForwardIterator that returns integer values from consequtive sequence
         * of integers between @p start and @p end, where the the step taken
         * between each integer is 1 with polarity as specified in @p direction.
         *
         * @note @p start must be smaller than @p end, not larger
         * or equal. This is not checked.
         */
        RangeIterator(const xsInteger start,
                      const Direction direction,
                      const xsInteger end);

        virtual Item next();
        virtual Item current() const;
        virtual xsInteger position() const;
        virtual xsInteger count();
        virtual Item::Iterator::Ptr toReversed();
        virtual Item::Iterator::Ptr copy() const;

    private:
        xsInteger m_start;
        xsInteger m_end;
        Item m_current;
        xsInteger m_position;
        xsInteger m_count;
        const Direction m_direction;

        /**
         * We only need to store -1 or 1, so save memory with a bit field.
         */
        const qint8 m_increment : 2;
    };
}

QT_END_NAMESPACE

QT_END_HEADER

#endif
