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

#ifndef Patternist_ReturnOrderBy_H
#define Patternist_ReturnOrderBy_H

#include "qorderby_p.h"
#include "qunlimitedcontainer_p.h"

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

namespace QPatternist
{
    /**
     * @short Together with OrderBy, it implements XQuery 1.0's <tt>order by</tt> expression.
     *
     * ReturnOrderBy evaluates the sort keys and values, and hands it over to
     * OrderBy, which is an AST ancestor, using SortTuples.
     *
     * @author Frans Englich <fenglich@trolltech.com>
     * @ingroup Patternist_expressions
     */
    class ReturnOrderBy : public UnlimitedContainer
    {
    public:
        /**
         * In @p operands the first item is the return expression, and the
         * rest, which is at least one, are the sort keys.
         */
        ReturnOrderBy(const OrderBy::Stability stability,
                      const OrderBy::OrderSpec::Vector &oSpecs,
                      const Expression::List &operands);

        virtual Item evaluateSingleton(const DynamicContext::Ptr &context) const;
        virtual bool evaluateEBV(const DynamicContext::Ptr &context) const;
        virtual SequenceType::Ptr staticType() const;
        virtual SequenceType::List expectedOperandTypes() const;
        virtual Expression::Ptr compress(const StaticContext::Ptr &context);
        virtual ExpressionVisitorResult::Ptr accept(const ExpressionVisitor::Ptr &visitor) const;
        virtual ID id() const;

        inline OrderBy::OrderSpec::Vector orderSpecs() const
        {
            return m_orderSpecs;
        }

        inline OrderBy::Stability stability() const
        {
            return m_stability;
        }

        /**
         * In the case of that we don't have a for-expression beloning us, but
         * only a let clause, this ReturnOrderBy breaks if it stays in the AST.
         * So, by default we assume that we should write ourselves away, unless
         * this function is called. The associated ForClause will call it
         * during typeCheck(), if it exists.
         */
        inline void setStay(const bool a)
        {
            m_flyAway = !a;
        }

        virtual Properties properties() const;
    private:
        /**
         * This variable is unfortunately only used at compile time. However,
         * it's tricky to get rid of it due to how QueryTransformParser would
         * have to be adapted.
         */
        const OrderBy::Stability    m_stability;

        OrderBy::OrderSpec::Vector  m_orderSpecs;

        /**
         * @see stay()
         */
        bool                        m_flyAway;
    };
}

QT_END_NAMESPACE

QT_END_HEADER

#endif
