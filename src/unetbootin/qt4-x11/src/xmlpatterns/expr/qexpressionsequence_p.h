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

#ifndef Patternist_ExpressionSequence_H
#define Patternist_ExpressionSequence_H

#include "qunlimitedcontainer_p.h"

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

namespace QPatternist
{
    /**
     * @short Implements the comma(",") operator, the sequence constructor.
     *
     * For example, the expression <tt>alpha, beta</tt> evaluates to a sequence
     * containing the items the nodetest @c alpha evaluates to, concatenated
     * with the items the nodetest @c beta evaluates to.
     *
     * @see <a href="http://www.w3.org/TR/xpath20/#construct_seq">XML Path Language
     * (XPath) 2.0, 3.3.1 Constructing Sequences</a>
     * @author Frans Englich <fenglich@trolltech.com>
     * @ingroup Patternist_expressions
     */
    class ExpressionSequence : public UnlimitedContainer
    {
    public:
        /**
         * Creates an ExpressionSequence with the operands @p operands. @p operands
         * must contain two or more Expression instances.
         */
        ExpressionSequence(const Expression::List &operands);

        virtual Item::Iterator::Ptr evaluateSequence(const DynamicContext::Ptr &) const;

        /**
         * Forwards the call to its children.
         */
        virtual void evaluateToSequenceReceiver(const DynamicContext::Ptr &context) const;

        virtual SequenceType::List expectedOperandTypes() const;
        virtual SequenceType::Ptr staticType() const;

        /**
         * Removes any empty sequences, typically "()", from its list of children. If
         * after that rewrite has no children, it rewrites itself to the CommonValues::empty;
         * if it has only one, it rewrites to the child.
         *
         * This optimization is not very usable by itself, but potentially becomes effective after other
         * optimizations have rewritten themselves into empty sequences. Thus,
         * saving memory consumption and runtime overhead.
         */
        virtual Expression::Ptr compress(const StaticContext::Ptr &context);

        inline Item::Iterator::Ptr mapToSequence(const Expression::Ptr &,
                                                 const DynamicContext::Ptr &) const;

        virtual Expression::Ptr typeCheck(const StaticContext::Ptr &context,
                                          const SequenceType::Ptr &reqType);
        /**
         * @returns Expression::DisableElimination, plus the union
         * of all this ExpressionSequence's children's properties. If any child
         * does not have IsEvaluated, it is removed from the result.
         */
        virtual Expression::Properties properties() const;
        virtual ExpressionVisitorResult::Ptr accept(const ExpressionVisitor::Ptr &visitor) const;
        virtual ID id() const;
    private:
        typedef QExplicitlySharedDataPointer<const ExpressionSequence> ConstPtr;
    };
}

QT_END_NAMESPACE

QT_END_HEADER

#endif
