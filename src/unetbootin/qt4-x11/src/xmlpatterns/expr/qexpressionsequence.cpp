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

#include "qcardinalityverifier_p.h"
#include "qcommonsequencetypes_p.h"
#include "qemptysequence_p.h"
#include "qsequencemappingiterator_p.h"

#include "qexpressionsequence_p.h"

QT_BEGIN_NAMESPACE

using namespace QPatternist;

ExpressionSequence::ExpressionSequence(const Expression::List &ops) : UnlimitedContainer(ops)
{
    Q_ASSERT_X(1 < ops.count(), Q_FUNC_INFO,
               "It makes no sense to have an ExpressionSequence containing less than two expressions.");
}

Item::Iterator::Ptr ExpressionSequence::mapToSequence(const Expression::Ptr &expr,
                                                      const DynamicContext::Ptr &context) const
{
    return expr->evaluateSequence(context);
}

Item::Iterator::Ptr ExpressionSequence::evaluateSequence(const DynamicContext::Ptr &context) const
{
    return makeSequenceMappingIterator<Item>(ConstPtr(this),
                                             makeListIterator(m_operands),
                                             context);
}

void ExpressionSequence::evaluateToSequenceReceiver(const DynamicContext::Ptr &context) const
{
    Expression::List::const_iterator it(m_operands.constBegin());
    const Expression::List::const_iterator end(m_operands.constEnd());
    Expression::List result;

    for(; it != end; ++it)
        (*it)->evaluateToSequenceReceiver(context);
}

Expression::Ptr ExpressionSequence::compress(const StaticContext::Ptr &context)
{
    const Expression::Ptr me(UnlimitedContainer::compress(context));

    if(me != this)
        return me;

    Expression::List::const_iterator it(m_operands.constBegin());
    const Expression::List::const_iterator end(m_operands.constEnd());
    Expression::List result;

    for(; it != end; ++it)
    {
        const ID Id = (*it)->id();

        /* Remove empty sequences. This is rather important because we have some steps in the parser that
         * intentionally, unconditionally and for temporary reasons create expressions like (expr, ()). Of course,
         * empty sequences also occur as part of optimizations.
         *
         * User function call sites that are of type empty-sequence() must be avoided since
         * they may contain calls to fn:error(), which we would rewrite away otherwise. */
        if(Id != IDUserFunctionCallsite && (*it)->staticType()->cardinality().isEmpty())
        {
            /* Rewrite "(1, (), 2)" into "(1, 2)" by not
             * adding (*it) to result. */
            continue;
        }
        else if(Id == IDExpressionSequence)
        {
            /* Rewrite "(1, (2, 3), 4)" into "(1, 2, 3, 4)" */
            Expression::List::const_iterator seqIt((*it)->operands().constBegin());
            const Expression::List::const_iterator seqEnd((*it)->operands().constEnd());

            for(; seqIt != seqEnd; ++seqIt)
                result.append(*seqIt);
        }
        else
            result.append(*it);
    }

    if(result.isEmpty())
        return EmptySequence::create(this, context);
    else if(result.count() == 1)
        return result.first();
    else
    {
        m_operands = result;
        return me;
    }
}

Expression::Ptr ExpressionSequence::typeCheck(const StaticContext::Ptr &context,
                                              const SequenceType::Ptr &reqType)
{
    Q_ASSERT(reqType);
    Expression::List::iterator it(m_operands.begin());
    const Expression::List::iterator end(m_operands.end());

    /* We treat the cardinality differently here by allowing the empty sequence
     * for each individual Expression, since the Cardinality can be conformed to by
     * the ExpressionSequence as a whole(which we check for at the end). */
    const SequenceType::Ptr testOnlyIT(makeGenericSequenceType(reqType->itemType(),
                                                               Cardinality::empty() |
                                                               reqType->cardinality()));

    for(; it != end; ++it)
        *it = (*it)->typeCheck(context, testOnlyIT);

    /* The above loop is only guaranteed to find item type errors, but the cardinality
     * can still be wrong since the operands were treated individually. */
    return CardinalityVerifier::verifyCardinality(Expression::Ptr(this), reqType->cardinality(), context);
}

Expression::Properties ExpressionSequence::properties() const
{
    const Expression::List::const_iterator end(m_operands.constEnd());
    Expression::List::const_iterator it;
    bool allEvaled = true;
    Expression::Properties props(DisableElimination); /* Why do we have this flag? */

    for(it = m_operands.constBegin(); it != end; ++it)
    {
        const Expression::Properties newp((*it)->properties());
        props |= newp;

        if((newp & IsEvaluated) != IsEvaluated)
        {
            allEvaled = false;
            break;
        }
    }

    if(!allEvaled)
        props &= ~IsEvaluated; /* Remove IsEvaluated. */

    /* One of our children might need the focus, but we don't, so
     * cut it out. */
    return props & ~RequiresFocus;
}

SequenceType::Ptr ExpressionSequence::staticType() const
{
    return operandsUnionType<ProductOfCardinality>();
}

SequenceType::List ExpressionSequence::expectedOperandTypes() const
{
    SequenceType::List result;
    /* ExpressionSequence is a bit strange type wise since it has an
     * infinite amount of operands. */
    result.append(CommonSequenceTypes::ZeroOrMoreItems);
    return result;
}

ExpressionVisitorResult::Ptr ExpressionSequence::accept(const ExpressionVisitor::Ptr &visitor) const
{
    return visitor->visit(this);
}

Expression::ID ExpressionSequence::id() const
{
    return IDExpressionSequence;
}

QT_END_NAMESPACE
