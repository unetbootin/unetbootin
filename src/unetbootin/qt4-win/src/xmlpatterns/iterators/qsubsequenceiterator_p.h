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

#ifndef Patternist_SubsequenceIterator_H
#define Patternist_SubsequenceIterator_H

#include "qitem_p.h"

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

namespace QPatternist
{
    /**
     * @short Picks out a slice from another QAbstractXmlForwardIterator, specified by a start and end position.
     *
     * SubsequenceIterator allows a "slice", a subsequence, from an QAbstractXmlForwardIterator to
     * be extracted. The SubsequenceIterator's constructor takes a source QAbstractXmlForwardIterator,
     * a start position, and the length of the subsequence to be extracted.
     *
     * SubsequenceIterator contains the central business logic to implement
     * the <tt>fn:subsequence()</tt> function. The detailed behavior, such as how it behaves
     * if the source QAbstractXmlForwardIterator is empty or if the specified subsequence stretches
     * beyond the source QAbstractXmlForwardIterator, is therefore consistent with the definition of
     * the <tt>fn:subsequence()</tt> function.
     *
     * @see <a href="http://www.w3.org/TR/xpath-functions/#func-subsequence">XQuery 1.0
     * and XPath 2.0 Functions and Operators, 15.1.10 fn:subsequence</a>
     * @author Frans Englich <fenglich@trolltech.com>
     * @ingroup Patternist_iterators
     */
    class SubsequenceIterator : public Item::Iterator
    {
    public:
        /**
         * Creates a SubsequenceIterator that extracts a subsequence from the sequence
         * in @p iterator, as specified by the @p start position and @p length parameter.
         *
         * @param iterator the iterator which the subsequence should
         * be extracted from
         * @param start the start position of extraction. Must be 1 or larger.
         * @param length the length of the subsequence to extract. If it is
         * -1, to the end is returned. The value must be -1 or 1 or larger.
         */
        SubsequenceIterator(const Item::Iterator::Ptr &iterator,
                            const xsInteger start,
                            const xsInteger length);

        virtual Item next();
        virtual Item current() const;
        virtual xsInteger position() const;
        virtual Item::Iterator::Ptr copy() const;

    private:
        xsInteger m_position;
        Item m_current;
        const Item::Iterator::Ptr m_it;
        xsInteger m_counter;
        const xsInteger m_start;
        const xsInteger m_len;
        const xsInteger m_stop;
    };
}

QT_END_NAMESPACE

QT_END_HEADER

#endif
