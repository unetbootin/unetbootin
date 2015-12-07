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

#include "qboolean_p.h"
#include "qcommonvalues_p.h"
#include "qemptysequence_p.h"
#include "qliteral_p.h"
#include "qliteralsequence_p.h"
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

    if(has(DisableElimination))
        retval = Expression::Ptr(this);
    else
        retval = constantPropagate(context);

    return invokeOptimizers(retval, context);
}

Expression::Ptr Expression::constantPropagate(const StaticContext::Ptr &context) const
{
    Q_ASSERT(context);

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
                return Expression::Ptr(new Literal(result.first()));
            default:
                return Expression::Ptr(new LiteralSequence(result));
        }
    }
    else
    {
        const Item item(evaluateSingleton(context->dynamicContext()));

        if(item)
            return Expression::Ptr(new Literal(item));
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
               "expectedContextItemType() must be overriden when RequiresContextItem is set.");
    return ItemType::Ptr();
}

Expression::Properties Expression::properties() const
{
    return Properties();
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
               "This function must be overriden when CreatesFocusForLast is set.");
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

QT_END_NAMESPACE
