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
#include "qatomicstring_p.h"

#include "qsimplecontentconstructor_p.h"

QT_BEGIN_NAMESPACE

using namespace QPatternist;

SimpleContentConstructor::SimpleContentConstructor(const Expression::Ptr &operand) : SingleContainer(operand)
{
}

Item SimpleContentConstructor::evaluateSingleton(const DynamicContext::Ptr &context) const
{
    const Item::Iterator::Ptr it(m_operand->evaluateSequence(context));
    Item next(it->next());
    QString result;

    if(next)
    {
        result = next.stringValue();
        next = it->next();
    }
    else
        return Item();

    while(next)
    {
        result += QLatin1Char(' ');
        result += next.stringValue();
        next = it->next();
    }

    return AtomicString::fromValue(result);
}

Expression::Ptr SimpleContentConstructor::compress(const StaticContext::Ptr &context)
{
    const Expression::Ptr me(SingleContainer::compress(context));

    if(me.data() == this)
    {
        /* Optimization: if we will evaluate to a single string, we're not
         * necessary. */
        if(CommonSequenceTypes::ExactlyOneString->matches(m_operand->staticType()))
            return m_operand;
    }

    return me;
}

SequenceType::List SimpleContentConstructor::expectedOperandTypes() const
{
    SequenceType::List result;
    result.append(CommonSequenceTypes::ZeroOrMoreAtomicTypes);
    return result;
}

SequenceType::Ptr SimpleContentConstructor::staticType() const
{
    if(m_operand->staticType()->cardinality().allowsEmpty())
        return CommonSequenceTypes::ZeroOrOneString;
    else
        return CommonSequenceTypes::ExactlyOneString;
}

ExpressionVisitorResult::Ptr SimpleContentConstructor::accept(const ExpressionVisitor::Ptr &visitor) const
{
    return visitor->visit(this);
}

QT_END_NAMESPACE
