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

#ifndef Patternist_InsertionIterator_H
#define Patternist_InsertionIterator_H

#include "qabstractxmlforwarditerator_p.h"
#include "qitem_p.h"

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

namespace QPatternist
{
    /**
     * @short Conceptually inserts one QAbstractXmlForwardIterator into another, make two QAbstractXmlForwardIterator instances appear as one.
     *
     * An InsertionIterator represents a sequence that is the merge of two
     * sequences, where one of the iterators is conceptually inserted at a
     * given position. This is done while retaining the characteristic of being
     * pull-based.
     *
     * InsertionIterator contains the logic for the implementation of the <tt>fn:insert-before()</tt>
     * function, whose definition therefore specifies the detailed behaviors of the
     * InsertionIterator.
     *
     * @see <a href="http://www.w3.org/TR/xpath-functions/#func-insert-before">XQuery 1.0
     * and XPath 2.0 Functions and Operators, 15.1.7 fn:insert-before</a>
     * @author Frans Englich <fenglich@trolltech.com>
     * @ingroup Patternist_iterators
     */
    class InsertionIterator : public Item::Iterator
    {
    public:

        /**
         * Creates an InsertionIterator whose result is a merge of the
         * iterator @p insertIterator into the iterator @p target at position @p
         * position.
         *
         * @param target the iterator containing the items that the
         * item in @p insertIterator will be inserted into.
         * @param position the insertion position. Must be 1 or larger
         * @param insertIterator the iterator containing the items to insert
         * at position @p position
         */
        InsertionIterator(const Item::Iterator::Ptr &target,
                          const xsInteger position,
                          const Item::Iterator::Ptr &insertIterator);

        virtual Item next();
        virtual Item current() const;
        virtual xsInteger position() const;
        virtual xsInteger count();
        virtual Item::Iterator::Ptr copy() const;

    private:
        const Item::Iterator::Ptr m_target;
        const xsInteger m_insertPos;
        const Item::Iterator::Ptr m_inserts;
        Item m_current;
        xsInteger m_position;
        bool m_isInserting;
    };
}

QT_END_NAMESPACE

QT_END_HEADER

#endif
