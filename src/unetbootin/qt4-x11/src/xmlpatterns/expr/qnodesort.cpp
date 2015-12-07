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
#include "qdeduplicateiterator_p.h"
#include "qnodesort_p.h"

QT_BEGIN_NAMESPACE

using namespace QPatternist;

NodeSortExpression::NodeSortExpression(const Expression::Ptr &op) : SingleContainer(op)
{
}

bool NodeSortExpression::lessThanUsingNodeModel(const Item &n1,
                                                const Item &n2)
{
    Q_ASSERT(n1.isNode());
    Q_ASSERT(n2.isNode());

    if(n1.asNode().model() == n2.asNode().model())
        return n1.asNode().compareOrder(n2.asNode()) == QXmlNodeModelIndex::Precedes;
    else
    {
        /* The two nodes are from different trees. The sort order is implementation
         * defined, but it must be stable.
         *
         * We do this by looking at the pointer difference. The value means nothing,
         * but it is stable, and that's what we're looking for. */
        return n1.asNode().model() - n2.asNode().model() < 0;
    }
}

Item::Iterator::Ptr NodeSortExpression::evaluateSequence(const DynamicContext::Ptr &context) const
{
    Q_ASSERT_X(m_operand->staticType()->cardinality().allowsMany(), Q_FUNC_INFO,
               "It makes no sense to sort a single node.");

    Item::List nodes(m_operand->evaluateSequence(context)->toList());

    if(nodes.isEmpty())
        return CommonValues::emptyIterator;
    else if(nodes.first().isAtomicValue())
        return makeListIterator(nodes);
    else
    {
        qSort(nodes.begin(), nodes.end(), lessThanUsingNodeModel);

        return Item::Iterator::Ptr(new DeduplicateIterator(nodes));
    }
}

Expression::Ptr NodeSortExpression::wrapAround(const Expression::Ptr &operand,
                                               const StaticContext::Ptr &context)
{
    Q_ASSERT(operand);
    Q_ASSERT(context);

    const Expression::Ptr sort(new NodeSortExpression(operand));
    context->addLocation(sort.data(), context->locationFor(operand.data()));
    return sort;
}

Expression::Ptr NodeSortExpression::compress(const StaticContext::Ptr &context)
{
    const Expression::Ptr me(SingleContainer::compress(context));

    /* It make no sense to sort & deduplicate a single node. */
    if(m_operand->staticType()->cardinality().allowsMany())
        return me;
    else
        return m_operand;
}

SequenceType::Ptr NodeSortExpression::staticType() const
{
    return m_operand->staticType();
}

SequenceType::List NodeSortExpression::expectedOperandTypes() const
{
    SequenceType::List result;
    result.append(CommonSequenceTypes::ZeroOrMoreItems);
    return result;
}

ExpressionVisitorResult::Ptr
NodeSortExpression::accept(const ExpressionVisitor::Ptr &visitor) const
{
    return visitor->visit(this);
}

Expression::Properties NodeSortExpression::properties() const
{
    /* The reason we disable elimination is that the assert for sorting a
     * single node in evaluateSequence() triggers unless our compress() routine
     * has been run. Anyhow, it's not that we would manage to write away anyway,
     * since the node source in most(all?) cases prevents it.
     */
    return AffectsOrderOnly | DisableElimination;
}

QT_END_NAMESPACE
