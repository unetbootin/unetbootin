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
#include "qbuiltintypes_p.h"
#include "qcommonsequencetypes_p.h"
#include "qgenericsequencetype_p.h"

#include "qliteral_p.h"

QT_BEGIN_NAMESPACE

using namespace QPatternist;

Literal::Literal(const Item &i) : m_item(i)
{
    Q_ASSERT(m_item);
    Q_ASSERT(m_item.isAtomicValue());
}

Item Literal::evaluateSingleton(const DynamicContext::Ptr &) const
{
    return m_item;
}

bool Literal::evaluateEBV(const DynamicContext::Ptr &context) const
{
    return Boolean::evaluateEBV(m_item, context);
}

void Literal::evaluateToSequenceReceiver(const DynamicContext::Ptr &context) const
{
    context->outputReceiver()->item(m_item);
}

SequenceType::Ptr Literal::staticType() const
{
    return makeGenericSequenceType(m_item.type(), Cardinality::exactlyOne());
}

ExpressionVisitorResult::Ptr Literal::accept(const ExpressionVisitor::Ptr &visitor) const
{
    return visitor->visit(this);
}

Expression::ID Literal::id() const
{
    Q_ASSERT(m_item);
    Q_ASSERT(m_item.isAtomicValue());
    const ItemType::Ptr t(m_item.type());

    if(BuiltinTypes::xsBoolean->xdtTypeMatches(t))
        return IDBooleanValue;
    else if(BuiltinTypes::xsString->xdtTypeMatches(t) ||
            BuiltinTypes::xsAnyURI->xdtTypeMatches(t) ||
            BuiltinTypes::xsUntypedAtomic->xdtTypeMatches(t))
    {
        return IDStringValue;
    }
    else if(BuiltinTypes::xsInteger->xdtTypeMatches(t))
        return IDIntegerValue;
    else
        return IDFloat;
}

Expression::Properties Literal::properties() const
{
    return IsEvaluated;
}

QString Literal::description() const
{
    return m_item.stringValue();
}

QT_END_NAMESPACE
