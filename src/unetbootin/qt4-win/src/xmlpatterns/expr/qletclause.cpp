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
#include "qcommonsequencetypes_p.h"
#include "qdynamiccontextstore_p.h"
#include "qliteral_p.h"

#include "qletclause_p.h"

QT_BEGIN_NAMESPACE

using namespace QPatternist;

LetClause::LetClause(const Expression::Ptr &operand1,
                     const Expression::Ptr &operand2,
                     const VariableDeclaration::Ptr &decl) : PairContainer(operand1, operand2)
                                                           , m_varDecl(decl)
{
    Q_ASSERT(m_varDecl);
}

DynamicContext::Ptr LetClause::bindVariable(const DynamicContext::Ptr &context) const
{
    context->setExpressionVariable(m_varDecl->slot, Expression::Ptr(new DynamicContextStore(m_operand1, context)));
    return context;
}

Item::Iterator::Ptr LetClause::evaluateSequence(const DynamicContext::Ptr &context) const
{
    return m_operand2->evaluateSequence(bindVariable(context));
}

Item LetClause::evaluateSingleton(const DynamicContext::Ptr &context) const
{
    return m_operand2->evaluateSingleton(bindVariable(context));
}

bool LetClause::evaluateEBV(const DynamicContext::Ptr &context) const
{
    return m_operand2->evaluateEBV(bindVariable(context));
}

void LetClause::evaluateToSequenceReceiver(const DynamicContext::Ptr &context) const
{
    m_operand2->evaluateToSequenceReceiver(bindVariable(context));
}

Expression::Ptr LetClause::typeCheck(const StaticContext::Ptr &context,
                                     const SequenceType::Ptr &reqType)
{
    /* Consider the following query:
     *
     * <tt>let $d := \<child type=""/>
     * return $d//\*[let $i := @type
     *              return $d//\*[$i]]</tt>
     *
     * The node test <tt>@type</tt> is referenced from two different places,
     * where each reference have a different focus. So, in the case of that the source
     * uses the focus, we need to use a DynamicContextStore to ensure the variable
     * is always evaluated with the correct focus, regardless of where it is referenced
     * from.
     *
     * We miss out a lot of false positives. For instance, the case of where the focus
     * is identical for everyone. One reason we cannot check this, is that Expression
     * doesn't know about its parent.
     */
    m_varDecl->canSourceRewrite = !m_operand1->deepProperties().testFlag(RequiresFocus);

    if(m_varDecl->canSourceRewrite)
        return m_operand2->typeCheck(context, reqType);
    else
        return PairContainer::typeCheck(context, reqType);
}

SequenceType::List LetClause::expectedOperandTypes() const
{
    SequenceType::List result;
    result.append(CommonSequenceTypes::ZeroOrMoreItems);
    result.append(CommonSequenceTypes::ZeroOrMoreItems);
    return result;
}

SequenceType::Ptr LetClause::staticType() const
{
    return m_operand2->staticType();
}

ExpressionVisitorResult::Ptr LetClause::accept(const ExpressionVisitor::Ptr &visitor) const
{
    return visitor->visit(this);
}

Expression::ID LetClause::id() const
{
    return IDLetClause;
}

QT_END_NAMESPACE
