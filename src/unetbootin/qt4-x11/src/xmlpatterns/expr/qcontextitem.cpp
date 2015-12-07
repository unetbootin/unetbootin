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

#include "qbuiltintypes_p.h"
#include "qcommonsequencetypes_p.h"
#include "qgenericsequencetype_p.h"

#include "qcontextitem_p.h"

QT_BEGIN_NAMESPACE

using namespace QPatternist;

Item ContextItem::evaluateSingleton(const DynamicContext::Ptr &context) const
{
    return context->contextItem();
}

Expression::Ptr ContextItem::compress(const StaticContext::Ptr &context)
{
    m_itemType = context->contextItemType();
    return EmptyContainer::compress(context);
}

Expression::Ptr ContextItem::typeCheck(const StaticContext::Ptr &context,
                                       const SequenceType::Ptr &reqType)
{
    m_itemType = context->contextItemType();
    return EmptyContainer::typeCheck(context, reqType);
}

SequenceType::Ptr ContextItem::staticType() const
{
    /* We test m_itemType here because Patternist View calls staticType() before the typeCheck()
     * stage. */
    if(m_itemType)
        return makeGenericSequenceType(m_itemType, Cardinality::exactlyOne());
    else
        return CommonSequenceTypes::ExactlyOneItem;
}

Expression::Properties ContextItem::properties() const
{
    return DisableElimination | RequiresContextItem | EvaluationCacheRedundant;
}

ExpressionVisitorResult::Ptr ContextItem::accept(const ExpressionVisitor::Ptr &visitor) const
{
    return visitor->visit(this);
}

Expression::ID ContextItem::id() const
{
    return IDContextItem;
}

ItemType::Ptr ContextItem::expectedContextItemType() const
{
    return BuiltinTypes::item;
}

const SourceLocationReflection *ContextItem::actualReflection() const
{
    if(m_expr)
        return m_expr.data();
    else
        return this;
}

void ContextItem::announceFocusType(const ItemType::Ptr &type)
{
    Q_ASSERT(type);
    m_itemType = type;
}

QT_END_NAMESPACE
