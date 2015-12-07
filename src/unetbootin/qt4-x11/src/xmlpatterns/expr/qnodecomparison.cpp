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
#include "qcommonvalues_p.h"
#include "qemptysequence_p.h"
#include "qinteger_p.h"
#include "qschemanumeric_p.h"
#include "qrangeiterator_p.h"

#include "qnodecomparison_p.h"

QT_BEGIN_NAMESPACE

using namespace QPatternist;

NodeComparison::NodeComparison(const Expression::Ptr &operand1,
                               const QXmlNodeModelIndex::DocumentOrder op,
                               const Expression::Ptr &operand2)
                               : PairContainer(operand1, operand2)
                               , m_op(op)
{
    Q_ASSERT(op == QXmlNodeModelIndex::Precedes   ||
             op == QXmlNodeModelIndex::Follows    ||
             op == QXmlNodeModelIndex::Is);
}

Item NodeComparison::evaluateSingleton(const DynamicContext::Ptr &context) const
{
    switch(evaluate(context))
    {
        case True:
            return CommonValues::BooleanTrue;
        case False:
            return CommonValues::BooleanFalse;
        default:
            return Item();
    }
}

bool NodeComparison::evaluateEBV(const DynamicContext::Ptr &context) const
{
    switch(evaluate(context))
    {
        case True:
            return true;
        default:
            /* We include the empty sequence here. */
            return false;
    }
}

NodeComparison::Result NodeComparison::evaluate(const DynamicContext::Ptr &context) const
{
    const Item op1(m_operand1->evaluateSingleton(context));
    if(!op1)
        return Empty;

    const Item op2(m_operand2->evaluateSingleton(context));
    if(!op2)
        return Empty;

    /* We just returns an arbitrary value, since there's no order defined for nodes from different
     * models, except for that the return value must be stable. */
    if(op1.asNode().model() != op2.asNode().model())
        return False;

    switch(m_op)
    {
        case QXmlNodeModelIndex::Is:
            return op1.asNode().is(op2.asNode()) ? True : False;
        case QXmlNodeModelIndex::Precedes:
            return op1.asNode().compareOrder(op2.asNode()) == QXmlNodeModelIndex::Precedes ? True : False;
        default:
        {
            Q_ASSERT(m_op == QXmlNodeModelIndex::Follows);
            return op1.asNode().compareOrder(op2.asNode()) == QXmlNodeModelIndex::Follows ? True : False;
        }
    }
}


SequenceType::List NodeComparison::expectedOperandTypes() const
{
    SequenceType::List result;
    result.append(CommonSequenceTypes::ZeroOrOneNode);
    result.append(CommonSequenceTypes::ZeroOrOneNode);
    return result;
}

Expression::Ptr NodeComparison::compress(const StaticContext::Ptr &context)
{
    const Expression::Ptr me(PairContainer::compress(context));

    if(me != this)
    /* We're already rewritten. */
        return me;

    if(m_operand1->staticType()->cardinality().isEmpty() ||
       m_operand2->staticType()->cardinality().isEmpty())
    {
        // TODO issue a warning in the @p context saying that one of the operands
        // were empty, and that the expression always result in the empty sequence
        // (which never is the intent, right?).
        return EmptySequence::create(this, context);
    }

    return Expression::Ptr(this);
}

QString NodeComparison::displayName(const QXmlNodeModelIndex::DocumentOrder op)
{
    switch(op)
    {
        case QXmlNodeModelIndex::Is:
            return QLatin1String("is");
        case QXmlNodeModelIndex::Precedes:
            return QLatin1String("<<");
        default:
        {
            Q_ASSERT(op == QXmlNodeModelIndex::Follows);
            return QLatin1String(">>");
        }
    }
}

SequenceType::Ptr NodeComparison::staticType() const
{
    if(m_operand1->staticType()->cardinality().allowsEmpty() ||
       m_operand2->staticType()->cardinality().allowsEmpty())
        return CommonSequenceTypes::ZeroOrOneBoolean;
    else
        return CommonSequenceTypes::ExactlyOneBoolean;
}

QXmlNodeModelIndex::DocumentOrder NodeComparison::operatorID() const
{
    return m_op;
}

ExpressionVisitorResult::Ptr NodeComparison::accept(const ExpressionVisitor::Ptr &visitor) const
{
    return visitor->visit(this);
}

QT_END_NAMESPACE
