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


#include "qcontextitem_p.h"
#include "qcommonsequencetypes_p.h"
#include "qemptysequence_p.h"
#include "qfunctionsignature_p.h"
#include "qgenericsequencetype_p.h"
#include "qcollationchecker_p.h"
#include "qcommonnamespaces_p.h"

#include "qfunctioncall_p.h"

QT_BEGIN_NAMESPACE

using namespace QPatternist;

SequenceType::List FunctionCall::expectedOperandTypes() const
{
    const FunctionArgument::List args(signature()->arguments());
    FunctionArgument::List::const_iterator it(args.constBegin());
    const FunctionArgument::List::const_iterator end(args.constEnd());
    // TODO reserve/resize()
    SequenceType::List result;

    for(; it != end; ++it)
        result.append((*it)->type());

    return result;
}

Expression::Ptr FunctionCall::typeCheck(const StaticContext::Ptr &context,
                                        const SequenceType::Ptr &reqType)
{
    /* We don't cache properties() at some stages because it can be invalidated
     * by the typeCheck(). */

    const FunctionSignature::Arity maxArgs = signature()->maximumArguments();
    /* We do this before the typeCheck() such that the appropriate conversions
     * are applied to the ContextItem. */
    if(m_operands.count() < maxArgs &&
       has(UseContextItem))
    {
        m_operands.append(Expression::Ptr(new ContextItem()));
        context->addLocation(m_operands.last().data(), context->locationFor(this));
    }

    const Expression::Ptr me(UnlimitedContainer::typeCheck(context, reqType));
    if(me.data() != this)
        return me;

    const Properties props(properties());

    if((props & RewriteToEmptyOnEmpty) == RewriteToEmptyOnEmpty &&
       *CommonSequenceTypes::Empty == *m_operands.first()->staticType()->itemType())
    {
        return EmptySequence::create(this, context);
    }

    if((props & LastOperandIsCollation) == LastOperandIsCollation &&
       m_operands.count() == maxArgs)
    {
        m_operands.last() = Expression::Ptr(new CollationChecker(m_operands.last()));
        context->addLocation(m_operands.last().data(), context->locationFor(this));
    }

    return me;
}

void FunctionCall::setSignature(const FunctionSignature::Ptr &sign)
{
    m_signature = sign;
}

FunctionSignature::Ptr FunctionCall::signature() const
{
    Q_ASSERT(m_signature); /* It really should be set. */
    return m_signature;
}

SequenceType::Ptr FunctionCall::staticType() const
{
    Q_ASSERT(m_signature);
    if(has(EmptynessFollowsChild))
    {
        if(m_operands.isEmpty())
        {
            /* This is a function which uses the context item when having no arguments. */
            return signature()->returnType();
        }
        const Cardinality card(m_operands.first()->staticType()->cardinality());
        if(card.allowsEmpty())
            return signature()->returnType();
        else
        {
            /* Remove empty. */
            return makeGenericSequenceType(signature()->returnType()->itemType(),
                                           card & Cardinality::oneOrMore());
        }
    }
    return signature()->returnType();
}

Expression::Properties FunctionCall::properties() const
{
    Q_ASSERT(m_signature);
    return signature()->properties();
}

ExpressionVisitorResult::Ptr FunctionCall::accept(const ExpressionVisitor::Ptr &visitor) const
{
    return visitor->visit(this);
}

Expression::ID FunctionCall::id() const
{
    Q_ASSERT(m_signature);
    return m_signature->id();
}

QT_END_NAMESPACE
