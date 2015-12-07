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
#include "qcommonsequencetypes_p.h"
#include "qcommonvalues_p.h"
#include "qliteral_p.h"

#include "qinstanceof_p.h"

QT_BEGIN_NAMESPACE

using namespace QPatternist;

InstanceOf::InstanceOf(const Expression::Ptr &operand,
                       const SequenceType::Ptr &tType) : SingleContainer(operand)
                                                       , m_targetType(tType)
{
    Q_ASSERT(m_targetType);
}

bool InstanceOf::evaluateEBV(const DynamicContext::Ptr &context) const
{
    const Item::Iterator::Ptr it(m_operand->evaluateSequence(context));
    Item item(it->next());
    unsigned int count = 1;

    if(!item)
        return m_targetType->cardinality().allowsEmpty();

    do
    {
        if(!m_targetType->itemType()->itemMatches(item))
            return false;

        if(count == 2 && !m_targetType->cardinality().allowsMany())
            return false;

        item = it->next();
        ++count;
    } while(item);

    return true;
}

Expression::Ptr InstanceOf::compress(const StaticContext::Ptr &context)
{
    const Expression::Ptr me(SingleContainer::compress(context));

    if(me != this || m_operand->has(DisableTypingDeduction))
        return me;

    const SequenceType::Ptr opType(m_operand->staticType());
    const ItemType::Ptr targetType(m_targetType->itemType());
    const ItemType::Ptr operandType(opType->itemType());

    if(m_targetType->cardinality().isMatch(opType->cardinality()))
    {
        if(*operandType == *CommonSequenceTypes::Empty || targetType->xdtTypeMatches(operandType))
            return wrapLiteral(CommonValues::BooleanTrue, context, this);
        else if(!operandType->xdtTypeMatches(targetType))
            return wrapLiteral(CommonValues::BooleanFalse, context, this);
    }
    /* Optimization: rule out the case where instance of will always fail. */

    return me;
}

SequenceType::Ptr InstanceOf::targetType() const
{
    return m_targetType;
}

SequenceType::Ptr InstanceOf::staticType() const
{
    return CommonSequenceTypes::ExactlyOneBoolean;
}

SequenceType::List InstanceOf::expectedOperandTypes() const
{
    SequenceType::List result;
    result.append(CommonSequenceTypes::ZeroOrMoreItems);
    return result;
}

ExpressionVisitorResult::Ptr InstanceOf::accept(const ExpressionVisitor::Ptr &visitor) const
{
    return visitor->visit(this);
}

QT_END_NAMESPACE
