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

#include "qcommonsequencetypes_p.h"
#include "qdeduplicateiterator_p.h"
#include "qnodesortexpression_p.h"

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
