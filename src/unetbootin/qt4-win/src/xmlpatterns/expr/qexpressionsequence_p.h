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
