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
