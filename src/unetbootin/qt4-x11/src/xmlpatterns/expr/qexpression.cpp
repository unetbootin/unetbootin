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

#include "qboolean_p.h"
#include "qcommonvalues_p.h"
#include "qemptysequence_p.h"
#include "qliteral_p.h"
#include "qliteralsequence_p.h"
#include "qoperandsiterator_p.h"
#include "qoptimizerframework_p.h"
#include "qstaticfocuscontext_p.h"
#include "qtypechecker_p.h"

#include "qexpression_p.h"

QT_BEGIN_NAMESPACE

using namespace QPatternist;

Expression::~Expression()
{
}

StaticContext::Ptr Expression::finalizeStaticContext(const StaticContext::Ptr &context) const
{
    Q_ASSERT(context);
    const ItemType::Ptr focusType(newFocusType());
    Q_ASSERT(focusType);
    return StaticContext::Ptr(new StaticFocusContext(focusType, context));
}

Expression::Ptr Expression::typeCheck(const StaticContext::Ptr &context,
                                      const SequenceType::Ptr &reqType)
{
    Q_ASSERT(reqType);
    typeCheckOperands(context);
    return TypeChecker::applyFunctionConversion(Expression::Ptr(this), reqType, context);
}

void Expression::typeCheckOperands(const StaticContext::Ptr &context)
{
    const Expression::List ops(operands());

    /* Check if this expression has any operands at all. */
    if(ops.isEmpty())
        return; /* We're done, early exit. */

    const SequenceType::List opTypes(expectedOperandTypes());
    Expression::List result;

    /* If we create a focus, we handle the last one specially, so avoid it in the loop. */
    const bool createsFocus = has(CreatesFocusForLast);
    const SequenceType::List::const_iterator typeEnd(createsFocus ? --opTypes.constEnd()
                                                                  : opTypes.constEnd());
    const Expression::List::const_iterator end(createsFocus ? --ops.constEnd()
                                                            : ops.constEnd());

    SequenceType::List::const_iterator reqType(opTypes.constBegin());
    SequenceType::Ptr t(*reqType);
    // TODO we assign twice to t here(also below in loop) when ops.size() > 1

    Expression::List::const_iterator it(ops.constBegin());

    for(; it != end; ++it)
    {
        /* This ensures that the last expectedOperandType stays, and is
         * used for all other operands. This is used for expressions that
         * have an infinite amount of operands, such as the concat() function. */
        if(reqType != typeEnd)
        {
            t = *reqType;
            ++reqType;
        }

         /* Let the child & its children typecheck. */
        result.append((*it)->typeCheck(context, t));
    }

    if(createsFocus)
    {
        const StaticContext::Ptr newContext(finalizeStaticContext(context));
        result.append(ops.last()->typeCheck(newContext, opTypes.last()));
    }

    setOperands(result);
}

Expression::Ptr Expression::invokeOptimizers(const Expression::Ptr &expr,
                                             const StaticContext::Ptr &context)
{
    Q_ASSERT(expr);

    const OptimizationPass::List opts(expr->optimizationPasses());

    if(opts.isEmpty()) /* Early exit. */
    {
        return expr;
    }

    const OptimizationPass::List::const_iterator passEnd(opts.constEnd());
    const OptimizationPass::List::const_iterator end(opts.constEnd());
    OptimizationPass::List::const_iterator passIt(opts.constBegin());

    for(; passIt != passEnd; ++passIt) /* Invoke each optimization pass. */
    {
        const OptimizationPass::Ptr pass(*passIt); /* Alias, for readability. */
        OptimizationPass::ExpressionMarker sourceMarker(pass->sourceExpression);

        if(pass->startIdentifier && !pass->startIdentifier->matches(expr))
        {
            /* This pass specified a start identifier and it did
             * not match -- let's try the next OptimizationPass. */
            continue;
        }

        const ExpressionIdentifier::List::const_iterator idEnd(pass->operandIdentifiers.constEnd());
        ExpressionIdentifier::List::const_iterator idIt(pass->operandIdentifiers.constBegin());
        const Expression::List ops(expr->operands());
        const Expression::List::const_iterator opEnd(ops.constEnd());
        Expression::List::const_iterator opIt(ops.constBegin());

        switch(pass->operandsMatchMethod)
        {
            case OptimizationPass::Sequential:
            {
                for(; opIt != opEnd; ++opIt)
                {
                    const Expression::Ptr operand(*opIt); /* Alias, for readability. */
                    const ExpressionIdentifier::Ptr opIdentifier(*idIt); /* Alias, for readability. */
                    if(opIdentifier && !opIdentifier->matches(operand))
                    {
                        break;
                    }

                    ++idIt;
                }

                if(opIt == opEnd)
                    break; /* All operands matched, so this pass matched. */
                else
                {
                    /* The loop above did not finish which means all operands did not match.
                       Therefore, this OptimizationPass did not match -- let's try the next one. */
                    continue;
                }
            }
            case OptimizationPass::AnyOrder:
            {
                Q_ASSERT_X(ops.count() == 2, Q_FUNC_INFO,
                           "AnyOrder is currently only supported for Expressions with two operands.");
                if(pass->operandIdentifiers.first()->matches(ops.first()) &&
                   pass->operandIdentifiers.last()->matches(ops.last()))
                {
                    break;
                }
                else if(pass->operandIdentifiers.first()->matches(ops.last()) &&
                        pass->operandIdentifiers.last()->matches(ops.first()))
                {
                    sourceMarker.first() = 1;
                    sourceMarker[1] = 0;
                    break; /* This pass matched. */
                }
                else
                    continue; /* This pass didn't match, let's loop through the next pass. */
            }
        }

        /* Figure out the source Expression, if any. */
        Expression::List operands;
        Expression::Ptr sourceExpr;

        if(!sourceMarker.isEmpty())
        {
            const OptimizationPass::ExpressionMarker::const_iterator mEnd(sourceMarker.constEnd());
            OptimizationPass::ExpressionMarker::const_iterator mIt(sourceMarker.constBegin());
            sourceExpr = expr;

            for(; mIt != mEnd; ++mIt)
            {
                Q_ASSERT(*mIt >= 0);
                sourceExpr = sourceExpr->operands().at(*mIt);
            }

            operands.append(sourceExpr);
        }

        if(operands.isEmpty())
        {
            Q_ASSERT(pass->resultCreator);
            return pass->resultCreator->create(Expression::List(), context, expr.data())->compress(context);
        }
        else if(pass->resultCreator)
            return pass->resultCreator->create(operands, context, expr.data())->compress(context);
        else
        {
            return sourceExpr;
        }
    }

    return expr;
}

Expression::Ptr Expression::compress(const StaticContext::Ptr &context)
{
    if(!compressOperands(context))
    {
        /* At least one of the operands cannot be evaluated at compile, so
         * 'this' Expression cannot const fold. */
        return invokeOptimizers(Expression::Ptr(this), context);
    }

    Expression::Ptr retval;

    if(hasDependency(DisableElimination))
        retval = Expression::Ptr(this);
    else
        retval = constantPropagate(context);

    return invokeOptimizers(retval, context);
}

Expression::Ptr Expression::constantPropagate(const StaticContext::Ptr &context) const
{
    Q_ASSERT(context);

    /* Optimization: We rewrite literals to literals here, which is pointless.
     * Maybe we should have a property which says "doesn't disable elimination
     * but don't eliminate me." */
    if(staticType()->cardinality().allowsMany())
    {
        Item::Iterator::Ptr it(evaluateSequence(context->dynamicContext()));
        Item::List result;
        Item item(it->next());

        while(item)
        {
            result.append(item);
            item = it->next();
        }

        switch(result.count())
        {
            case 0:
                return EmptySequence::create(this, context);
            case 1:
                return rewrite(Expression::Ptr(new Literal(result.first())), context);
            default:
                return rewrite(Expression::Ptr(new LiteralSequence(result)), context);
        }
    }
    else
    {
        const Item item(evaluateSingleton(context->dynamicContext()));

        if(item)
            return rewrite(Expression::Ptr(new Literal(item)), context);
        else
            return EmptySequence::create(this, context);
    }
}

Item::Iterator::Ptr Expression::evaluateSequence(const DynamicContext::Ptr &context) const
{
    const Item item(evaluateSingleton(context));

    if(item)
        return makeSingletonIterator(item);
    else
        return CommonValues::emptyIterator;
}

Item Expression::evaluateSingleton(const DynamicContext::Ptr &context) const
{
    return Boolean::fromValue(evaluateEBV(context));
}

bool Expression::evaluateEBV(const DynamicContext::Ptr &context) const
{
    return Boolean::evaluateEBV(evaluateSequence(context), context);
}

void Expression::evaluateToSequenceReceiver(const DynamicContext::Ptr &context) const
{
    QAbstractXmlReceiver *const receiver = context->outputReceiver();
    const Item::Iterator::Ptr it(evaluateSequence(context));
    Item next(it->next());

    while(next)
    {
        receiver->item(next);
        next = it->next();
    }
}

ItemType::Ptr Expression::expectedContextItemType() const
{
    Q_ASSERT_X(false, Q_FUNC_INFO,
               "expectedContextItemType() must be overridden when RequiresContextItem is set.");
    return ItemType::Ptr();
}

Expression::Properties Expression::properties() const
{
    return Properties();
}

Expression::Properties Expression::dependencies() const
{
    OperandsIterator it(Ptr(const_cast<Expression *>(this)), OperandsIterator::ExcludeParent);
    Expression::Ptr next(it.next());

    Properties dependencies(properties());

    while(next)
    {
        dependencies |= next->dependencies();
        next = it.next();
    }

    return dependencies & (Expression::RequiresFocus | Expression::IsEvaluated | Expression::DisableElimination);
}

void Expression::announceFocusType(const ItemType::Ptr &itemType)
{
    const Expression::List ops(operands());
    const int len = ops.count();

    for(int i = 0; i < len; ++i)
        ops.at(i)->announceFocusType(itemType);
}

Expression::Properties Expression::deepProperties() const
{
    Properties props(properties());
    const Expression::List ops(operands());
    const int len = ops.count();

    for(int i = 0; i < len; ++i)
        props |= ops.at(i)->deepProperties();

    return props;
}

Expression::ID Expression::id() const
{
    return IDIgnorableExpression;
}

OptimizationPass::List Expression::optimizationPasses() const
{
    return OptimizationPass::List();
}

ItemType::Ptr Expression::newFocusType() const
{
    Q_ASSERT_X(false, Q_FUNC_INFO,
               "This function must be overridden when CreatesFocusForLast is set.");
    return ItemType::Ptr();
}

const SourceLocationReflection *Expression::actualReflection() const
{
    return this;
}

QString Expression::description() const
{
    return QString::fromLatin1("Expression, id: %1").arg(QString::number(id()));
}

PatternPriority Expression::patternPriority() const
{
    return 0.5;
}

QT_END_NAMESPACE
