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
    if(me.data() != this)
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
