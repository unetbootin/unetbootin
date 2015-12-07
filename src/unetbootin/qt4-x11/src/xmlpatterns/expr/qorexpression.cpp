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
#include "qliteral_p.h"

#include "qorexpression_p.h"

QT_BEGIN_NAMESPACE

using namespace QPatternist;

OrExpression::OrExpression(const Expression::Ptr &operand1,
                           const Expression::Ptr &operand2) : AndExpression(operand1, operand2)
{
}

bool OrExpression::evaluateEBV(const DynamicContext::Ptr &context) const
{
    return m_operand1->evaluateEBV(context) || m_operand2->evaluateEBV(context);
}

Expression::Ptr OrExpression::compress(const StaticContext::Ptr &context)
{
    const Expression::Ptr newMe(PairContainer::compress(context));

    if(newMe != this)
        return newMe;

    /* Both operands mustn't be evaluated in order to be able to compress. */
    if(m_operand1->isEvaluated() && m_operand1->evaluateEBV(context->dynamicContext()))
        return wrapLiteral(CommonValues::BooleanTrue, context, this);
    else if(m_operand2->isEvaluated() && m_operand2->evaluateEBV(context->dynamicContext()))
        return wrapLiteral(CommonValues::BooleanTrue, context, this);
    else
        return Expression::Ptr(this);
}

ExpressionVisitorResult::Ptr OrExpression::accept(const ExpressionVisitor::Ptr &visitor) const
{
    return visitor->visit(this);
}

QT_END_NAMESPACE
