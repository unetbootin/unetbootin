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

#include "qitemmappingiterator_p.h"
#include "qsequencemappingiterator_p.h"

#include "qargumentconverter_p.h"

QT_BEGIN_NAMESPACE

using namespace QPatternist;

ArgumentConverter::ArgumentConverter(const Expression::Ptr &operand,
                                     const ItemType::Ptr &reqType) : UntypedAtomicConverter(operand, reqType)
{
}

ExpressionVisitorResult::Ptr ArgumentConverter::accept(const ExpressionVisitor::Ptr &visitor) const
{
    return visitor->visit(this);
}

Item::Iterator::Ptr ArgumentConverter::mapToSequence(const Item &item,
                                                     const DynamicContext::Ptr &context) const
{
    if(item.isAtomicValue() && !BuiltinTypes::xsUntypedAtomic->xdtTypeMatches(item.type()))
        return makeSingletonIterator(item);
    else
    {
        /* We're using UntypedAtomicConverter::mapToItem(). */
        return makeItemMappingIterator<Item>(ConstPtr(this),
                                             item.sequencedTypedValue(),
                                             context);
    }
}

Item::Iterator::Ptr ArgumentConverter::evaluateSequence(const DynamicContext::Ptr &context) const
{
    return makeSequenceMappingIterator<Item>(ConstPtr(this),
                                             m_operand->evaluateSequence(context),
                                             context);
}

Item ArgumentConverter::evaluateSingleton(const DynamicContext::Ptr &context) const
{
    const Item item(m_operand->evaluateSingleton(context));

    if(item)
        return mapToItem(item, context);
    else /* Empty is allowed. ArgumentConverter doesn't care about cardinality. */
        return Item();
}

SequenceType::List ArgumentConverter::expectedOperandTypes() const
{
    SequenceType::List result;
    result.append(CommonSequenceTypes::ZeroOrMoreItems);
    return result;
}

SequenceType::Ptr ArgumentConverter::staticType() const
{
    return CommonSequenceTypes::ZeroOrMoreAtomicTypes;
}

QT_END_NAMESPACE
