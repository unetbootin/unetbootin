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


#include "qexpressionvariablereference_p.h"

QT_BEGIN_NAMESPACE

using namespace QPatternist;

ExpressionVariableReference::ExpressionVariableReference(const VariableSlotID slotP,
                                                         const VariableDeclaration::Ptr &varDecl) : VariableReference(slotP)
                                                                                                  , m_varDecl(varDecl)
{
}

bool ExpressionVariableReference::evaluateEBV(const DynamicContext::Ptr &context) const
{
    return context->expressionVariable(slot())->evaluateEBV(context);
}

Item ExpressionVariableReference::evaluateSingleton(const DynamicContext::Ptr &context) const
{
    return context->expressionVariable(slot())->evaluateSingleton(context);
}

Item::Iterator::Ptr ExpressionVariableReference::evaluateSequence(const DynamicContext::Ptr &context) const
{
    return context->expressionVariable(slot())->evaluateSequence(context);
}
Expression::Ptr ExpressionVariableReference::typeCheck(const StaticContext::Ptr &context,
                                                       const SequenceType::Ptr &reqType)
{
    if(m_varDecl->canSourceRewrite)
        return m_varDecl->expression()->typeCheck(context, reqType);
    else
        return VariableReference::typeCheck(context, reqType);
}

Expression::ID ExpressionVariableReference::id() const
{
    return IDExpressionVariableReference;
}

ExpressionVisitorResult::Ptr ExpressionVariableReference::accept(const ExpressionVisitor::Ptr &visitor) const
{
    return visitor->visit(this);
}

SequenceType::Ptr ExpressionVariableReference::staticType() const
{
    return m_varDecl->expression()->staticType();
}

QT_END_NAMESPACE
