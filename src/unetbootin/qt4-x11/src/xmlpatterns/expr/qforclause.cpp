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

#include "qcommonsequencetypes_p.h"
#include "qemptysequence_p.h"
#include "qgenericsequencetype_p.h"
#include "qitemmappingiterator_p.h"
#include "qoptimizationpasses_p.h"
#include "qsequencemappingiterator_p.h"

#include "qforclause_p.h"

QT_BEGIN_NAMESPACE

using namespace QPatternist;

ForClause::ForClause(const VariableSlotID varSlot,
                     const Expression::Ptr &bindingSequence,
                     const Expression::Ptr &returnExpression,
                     const VariableSlotID positionSlot) : PairContainer(bindingSequence, returnExpression),
                                                          m_varSlot(varSlot),
                                                          m_positionSlot(positionSlot),
                                                          m_allowsMany(true)
{
    Q_ASSERT(m_positionSlot > -2);
}

Item ForClause::mapToItem(const Item &item,
                          const DynamicContext::Ptr &context) const
{
    context->setRangeVariable(m_varSlot, item);
    return m_operand2->evaluateSingleton(context);
}

Item::Iterator::Ptr ForClause::mapToSequence(const Item &item,
                                             const DynamicContext::Ptr &context) const
{
    context->setRangeVariable(m_varSlot, item);
    return m_operand2->evaluateSequence(context);
}

void ForClause::riggPositionalVariable(const DynamicContext::Ptr &context,
                                       const Item::Iterator::Ptr &source) const
{
    if(m_positionSlot > -1)
        context->setPositionIterator(m_positionSlot, source);
}

Item::Iterator::Ptr ForClause::evaluateSequence(const DynamicContext::Ptr &context) const
{
    const Item::Iterator::Ptr source(m_operand1->evaluateSequence(context));

    riggPositionalVariable(context, source);

    if(m_allowsMany)
    {
        return makeSequenceMappingIterator<Item>(ConstPtr(this),
                                                 source,
                                                 context);
    }
    else
    {
        return makeItemMappingIterator<Item>(ConstPtr(this),
                                             source,
                                             context);
    }
}

Item ForClause::evaluateSingleton(const DynamicContext::Ptr &context) const
{
    return evaluateSequence(context)->next();
}

void ForClause::evaluateToSequenceReceiver(const DynamicContext::Ptr &context) const
{
    Item::Iterator::Ptr it;
    const Item::Iterator::Ptr source(m_operand1->evaluateSequence(context));

    riggPositionalVariable(context, source);

    Item next(source->next());

    while(next)
    {
        context->setRangeVariable(m_varSlot, next);
        m_operand2->evaluateToSequenceReceiver(context);
        next = source->next();
    }
}

Expression::Ptr ForClause::typeCheck(const StaticContext::Ptr &context,
                                     const SequenceType::Ptr &reqType)
{
    const Expression::Ptr me(PairContainer::typeCheck(context, reqType));
    const Cardinality card(m_operand1->staticType()->cardinality());

    /* If our source is empty we will always evaluate to the empty sequence, so rewrite. */
    if(card.isEmpty())
        return EmptySequence::create(this, context);
    else
        return me;

    /* This breaks because  the variable references haven't rewritten themselves, so
     * they dangle. When this is fixed, evaluateSingleton can be removed. */
    /*
    else if(card->allowsMany())
        return me;
    else
        return m_operand2;
        */
}

Expression::Ptr ForClause::compress(const StaticContext::Ptr &context)
{
    const Expression::Ptr me(PairContainer::compress(context));
    if(me != this)
        return me;

    /* This is done after calling PairContainer::typeCheck(). The advantage of this is that
     * m_allowsMany is updated according to what the operand says after it has compressed. However,
     * if it was initialized to false(as it once was..), ForClause::evaluateSequence()
     * would potentially have been called by PairContainer::compress(), and it would have
     * used an unsafe branch. */
    m_allowsMany = m_operand2->staticType()->cardinality().allowsMany();

    return me;
}

SequenceType::Ptr ForClause::staticType() const
{
    const SequenceType::Ptr returnType(m_operand2->staticType());

    return makeGenericSequenceType(returnType->itemType(),
                                   m_operand1->staticType()->cardinality()
                                        * /* multiply operator */
                                   returnType->cardinality());
}

SequenceType::List ForClause::expectedOperandTypes() const
{
    SequenceType::List result;
    result.append(CommonSequenceTypes::ZeroOrMoreItems);
    result.append(CommonSequenceTypes::ZeroOrMoreItems);
    return result;
}

ExpressionVisitorResult::Ptr ForClause::accept(const ExpressionVisitor::Ptr &visitor) const
{
    return visitor->visit(this);
}

OptimizationPass::List ForClause::optimizationPasses() const
{
    return OptimizationPasses::forPasses;
}

Expression::ID ForClause::id() const
{
    return IDForClause;
}

QT_END_NAMESPACE
