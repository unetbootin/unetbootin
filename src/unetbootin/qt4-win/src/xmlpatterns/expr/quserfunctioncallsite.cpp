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
#include "qdynamiccontextstore_p.h"
#include "qevaluationcache_p.h"

#include "quserfunctioncallsite_p.h"

QT_BEGIN_NAMESPACE

using namespace QPatternist;

UserFunctionCallsite::UserFunctionCallsite(const QXmlName nameP,
                                           const FunctionSignature::Arity ar) : m_name(nameP)
                                                                              , m_arity(ar)
                                                                              , m_expressionSlotOffset(-2)
                                                                              , m_isRecursive(false)

{
    Q_ASSERT(!m_name.isNull());
}

Item::Iterator::Ptr UserFunctionCallsite::evaluateSequence(const DynamicContext::Ptr &context) const
{
    return m_body->evaluateSequence(bindVariables(context));
}

Item UserFunctionCallsite::evaluateSingleton(const DynamicContext::Ptr &context) const
{
    return m_body->evaluateSingleton(bindVariables(context));
}

bool UserFunctionCallsite::evaluateEBV(const DynamicContext::Ptr &context) const
{
    return m_body->evaluateEBV(bindVariables(context));
}

void UserFunctionCallsite::evaluateToSequenceReceiver(const DynamicContext::Ptr &context) const
{
    m_body->evaluateToSequenceReceiver(bindVariables(context));
}

DynamicContext::Ptr UserFunctionCallsite::bindVariables(const DynamicContext::Ptr &context) const
{
    const DynamicContext::Ptr stackContext(context->createStack());
    Q_ASSERT(stackContext);

    const Expression::List::const_iterator end(m_operands.constEnd());
    Expression::List::const_iterator it(m_operands.constBegin());

    VariableSlotID slot = m_expressionSlotOffset;

    for(; it != end; ++it)
    {
        stackContext->setExpressionVariable(slot,
                                            Expression::Ptr(new DynamicContextStore(*it, context)));
        ++slot;
    }

    return stackContext;
}

SequenceType::List UserFunctionCallsite::expectedOperandTypes() const
{
    SequenceType::List result;

    if(m_functionDeclaration)
    {
        const FunctionArgument::List args(m_functionDeclaration->signature()->arguments());
        const FunctionArgument::List::const_iterator end(args.constEnd());
        FunctionArgument::List::const_iterator it(args.constBegin());

        for(; it != end; ++it)
            result.append((*it)->type());
    }
    else
        result.append(CommonSequenceTypes::ZeroOrMoreItems);

    return result;
}

Expression::Ptr UserFunctionCallsite::typeCheck(const StaticContext::Ptr &context,
                                                const SequenceType::Ptr &reqType)
{
    /* Update, such that we use a recent version of the body that has typeCheck()
     * and compress() rewrites included. */
    m_body = m_functionDeclaration->body();

    /* Ensure that the return value of the function is properly
     * converted/does match from where it is called(which is here). */
    if(m_isRecursive)
        return UnlimitedContainer::typeCheck(context, reqType);
    else
    {
        /* Note, we can't assign to m_functionDeclaration->body() because UserFunction can apply
         * to several different callsites. Hence we need our own version. */
        m_body = m_body->typeCheck(context, reqType);

        /* We just act as a pipe for m_body, so we don't have to typecheck ourselves. However,
         * the arguments must match the function declaration. */
        typeCheckOperands(context);
        return Expression::Ptr(this);
    }
}

Expression::Properties UserFunctionCallsite::properties() const
{
    return DisableElimination;
}

SequenceType::Ptr UserFunctionCallsite::staticType() const
{
    /* Our return type, is the static type of the function body. We could have also used
     * m_functionDeclaration->signature()->returnType(), but it doesn't get updated
     * when function conversion is applied.
     * We can't use m_body's type if we're recursive, because m_body computes its type
     * from its children, and we're at least one of the children. Hence, we would
     * recurse infinitely if we did.
     *
     * m_body can be null here if we're called before setSource().
     */
    if(m_isRecursive || !m_body)
        return CommonSequenceTypes::ZeroOrMoreItems; // TODO use the declaration, it can have a type explicitly.
    else
        return m_body->staticType();
}

ExpressionVisitorResult::Ptr UserFunctionCallsite::accept(const ExpressionVisitor::Ptr &visitor) const
{
    return visitor->visit(this);
}

Expression::ID UserFunctionCallsite::id() const
{
    return IDUserFunctionCallsite;
}

bool UserFunctionCallsite::isSignatureValid(const FunctionSignature::Ptr &sign) const
{
    Q_ASSERT(sign);

    return sign->name() == m_name
           &&
           sign->isArityValid(m_arity);
}

bool UserFunctionCallsite::configureRecursion(const FunctionSignature::Ptr &sign)
{
    Q_ASSERT(sign);

    m_isRecursive = isSignatureValid(sign);
    return m_isRecursive;
}

void UserFunctionCallsite::setSource(const UserFunction::Ptr &userFunction,
                                     const VariableSlotID cacheSlotOffset)
{
    m_functionDeclaration = userFunction;
    m_body = userFunction->body();
    m_expressionSlotOffset = userFunction->expressionSlotOffset();

    const int len = m_operands.size();

    const VariableDeclaration::List varDecls(userFunction->argumentDeclarations());

    for(int i = 0; i < len; ++i)
    {
        /* We don't want evaluation caches for range variables, it's not necessary since
         * the item is already cached in DynamicContext::rangeVariable(). */
        if(m_operands.at(i)->is(IDRangeVariableReference))
            continue;

        /* Note that we pass in cacheSlotOffset + i here instead of varDecls.at(i)->slot since
         * we want independent caches for each callsite. */
        m_operands[i] = Expression::Ptr(new EvaluationCache<false>(m_operands.at(i),
                                                                   varDecls.at(i),
                                                                   cacheSlotOffset + i));
    }
}

FunctionSignature::Arity UserFunctionCallsite::arity() const
{
    return m_arity;
}

QXmlName UserFunctionCallsite::name() const
{
    return m_name;
}

FunctionSignature::Ptr UserFunctionCallsite::signature() const
{
    return m_functionDeclaration->signature();
}

QT_END_NAMESPACE
