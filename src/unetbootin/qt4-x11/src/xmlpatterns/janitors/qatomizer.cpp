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

#include "qatomictype_p.h"
#include "qbuiltintypes_p.h"
#include "qcommonsequencetypes_p.h"
#include "qgenericsequencetype_p.h"
#include "qsequencemappingiterator_p.h"

#include "qatomizer_p.h"

QT_BEGIN_NAMESPACE

using namespace QPatternist;

Atomizer::Atomizer(const Expression::Ptr &operand) : SingleContainer(operand)
{
}

Item::Iterator::Ptr Atomizer::mapToSequence(const Item &item, const DynamicContext::Ptr &) const
{
    /* Function & Operators, 2.4.2 fn:data, says "If the node does not have a
     * typed value an error is raised [err:FOTY0012]."
     * When does a node not have a typed value? */
    Q_ASSERT(item);
    return item.sequencedTypedValue();
}

Item::Iterator::Ptr Atomizer::evaluateSequence(const DynamicContext::Ptr &context) const
{
    return makeSequenceMappingIterator<Item>(ConstPtr(this),
                                             m_operand->evaluateSequence(context),
                                             context);
}

Item Atomizer::evaluateSingleton(const DynamicContext::Ptr &context) const
{
    const Item item(m_operand->evaluateSingleton(context));

    if(!item) /* Empty is allowed, cardinality is considered '?' */
        return Item();

    const Item::Iterator::Ptr it(mapToSequence(item, context));
    Q_ASSERT_X(it, Q_FUNC_INFO, "A valid QAbstractXmlForwardIterator must always be returned.");

    Item result(it->next());
    Q_ASSERT_X(!it->next(), Q_FUNC_INFO,
               "evaluateSingleton should never be used if the cardinality is two or more");

    return result;
}

Expression::Ptr Atomizer::typeCheck(const StaticContext::Ptr &context,
                                    const SequenceType::Ptr &reqType)
{
    /* Compress -- the earlier the better. */
    if(BuiltinTypes::xsAnyAtomicType->xdtTypeMatches(m_operand->staticType()->itemType()))
        return m_operand->typeCheck(context, reqType);

    return SingleContainer::typeCheck(context, reqType);
}

SequenceType::Ptr Atomizer::staticType() const
{
    const SequenceType::Ptr opt(m_operand->staticType());
    return makeGenericSequenceType(opt->itemType()->atomizedType(),
                                   opt->cardinality());
}

SequenceType::List Atomizer::expectedOperandTypes() const
{
    SequenceType::List result;
    result.append(CommonSequenceTypes::ZeroOrMoreItems);
    return result;
}

ExpressionVisitorResult::Ptr Atomizer::accept(const ExpressionVisitor::Ptr &visitor) const
{
    return visitor->visit(this);
}

const SourceLocationReflection *Atomizer::actualReflection() const
{
    return m_operand->actualReflection();
}

QT_END_NAMESPACE
