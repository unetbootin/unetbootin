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

#ifndef Patternist_UnlimitedContainer_H
#define Patternist_UnlimitedContainer_H

#include <QList>
#include "qexpression_p.h"
#include "qgenericsequencetype_p.h"

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

namespace QPatternist
{
    /**
     * @short Base class for expressions that has any amount of operands.
     *
     * @author Frans Englich <fenglich@trolltech.com>
     * @ingroup Patternist_expressions
     */
    class UnlimitedContainer : public Expression
    {
    public:
        /**
         * Creates an UnlimitedContainer containing the operands @p operands. @p operands
         * may be empty.
         */
        UnlimitedContainer(const Expression::List &operands = Expression::List());

        virtual void setOperands(const Expression::List &list);

        virtual Expression::List operands() const;

        /**
         * @note This function cannot be called before setOperands is called.
         */
        virtual bool compressOperands(const StaticContext::Ptr &);

        /**
         * Tells how operandsUnionType() should compute the cardinality of
         * its children.
         *
         * This type is public because of a bug in the HP-UX aCC compiler.
         */
        enum CardinalityComputation
        {
            ProductOfCardinality,
            UnionOfCardinality
        };

    protected:
        /**
         * Computes and returns the union type of all the Expression instances
         * in this Expression's operands.
         *
         * This implementation is placed inside because CardinalityComputation
         * can't be referenced from the outside(in conforming compilers).
         */
        template<CardinalityComputation suppliedCard>
        inline
        SequenceType::Ptr operandsUnionType() const
        {
            Q_ASSERT(suppliedCard ==  ProductOfCardinality || suppliedCard == UnionOfCardinality);
            const Expression::List::const_iterator end(m_operands.constEnd());
            Expression::List::const_iterator it(m_operands.constBegin());

            /* Load the first one, and jump over it in the loop. */
            SequenceType::Ptr t(m_operands.first()->staticType());
            ItemType::Ptr type(t->itemType());
            Cardinality card(t->cardinality());
            ++it;

            for(; it != end; ++it)
            {
                t = (*it)->staticType();
                type |= t->itemType();

                /* Since this function is a template function, it doesn't
                 * hurt performance that this test is inside the loop. */
                if(suppliedCard == ProductOfCardinality)
                    card += t->cardinality();
                else
                    card |= t->cardinality();
            }

            return makeGenericSequenceType(type, card);
        }

        Expression::List m_operands;
    };


}

QT_END_NAMESPACE

QT_END_HEADER

#endif
