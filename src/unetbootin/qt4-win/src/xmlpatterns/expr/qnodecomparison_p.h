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

#ifndef Patternist_NodeComparison_H
#define Patternist_NodeComparison_H

#include "qpaircontainer_p.h"

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

namespace QPatternist
{

    /**
     * @short Implements the node comparison operators <tt>\>\></tt>, <tt>\<\<</tt>, and @c is.
     *
     * @see <a href="http://www.w3.org/TR/xpath20/#id-node-comparisons">XML Path Language
     * (XPath) 2.0, 3.5.3 QXmlNodeModelIndex Comparisons</a>
     * @author Frans Englich <fenglich@trolltech.com>
     * @ingroup Patternist_expressions
     */
    class Q_AUTOTEST_EXPORT NodeComparison : public PairContainer
    {
    public:
        NodeComparison(const Expression::Ptr &operand1,
                       const QXmlNodeModelIndex::DocumentOrder op,
                       const Expression::Ptr &operand2);

        virtual Item evaluateSingleton(const DynamicContext::Ptr &) const;
        virtual bool evaluateEBV(const DynamicContext::Ptr &) const;

        virtual SequenceType::List expectedOperandTypes() const;

        virtual QXmlNodeModelIndex::DocumentOrder operatorID() const;
        /**
         * If any operator is the empty sequence, the NodeComparison rewrites
         * into that, since the empty sequence is always the result in that case.
         */
        virtual Expression::Ptr compress(const StaticContext::Ptr &context);

        /**
         * @returns either CommonSequenceTypes::ZeroOrOneBoolean or
         * CommonSequenceTypes::ExactlyOneBoolean depending on the static
         * cardinality of its operands.
         */
        virtual SequenceType::Ptr staticType() const;

        /**
         * Determines the string representation for a node comparison operator.
         *
         * @returns
         * - "<<" if @p op is Precedes
         * - ">>" if @p op is Follows
         * - "is" if @p op is Is
         */
        static QString displayName(const QXmlNodeModelIndex::DocumentOrder op);

        virtual ExpressionVisitorResult::Ptr accept(const ExpressionVisitor::Ptr &visitor) const;
    private:
        enum Result
        {
            Empty,
            True,
            False
        };
        inline Result evaluate(const DynamicContext::Ptr &context) const;

        const QXmlNodeModelIndex::DocumentOrder m_op;

    };
}

QT_END_NAMESPACE

QT_END_HEADER

#endif
