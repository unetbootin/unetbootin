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
#include "qitemmappingiterator_p.h"
#include "qgenericsequencetype_p.h"
#include "qparentnodeaxis_p.h"

#include "qaxisstep_p.h"

QT_BEGIN_NAMESPACE

using namespace QPatternist;

namespace QPatternist
{
    /**
     * This operator is needed for the s_whenAxisNodeKindEmpty array. The @c int constructors
     * ensure we invoke another operator| such that we don't get an infinite loop.
     */
    static inline QXmlNodeModelIndex::NodeKind operator|(const QXmlNodeModelIndex::NodeKind &op1, const QXmlNodeModelIndex::NodeKind &op2)
    {
        return QXmlNodeModelIndex::NodeKind(int(op1) | int(op2));
    }
}

/**
 * @note The order is significant. It is of the same order as the values in QXmlNodeModelIndex::Axis is declared.
 */
const QXmlNodeModelIndex::NodeKind AxisStep::s_whenAxisNodeKindEmpty[] =
{
    QXmlNodeModelIndex::Attribute|QXmlNodeModelIndex::Text|QXmlNodeModelIndex::ProcessingInstruction|QXmlNodeModelIndex::Comment|QXmlNodeModelIndex::Namespace,   // child;
    QXmlNodeModelIndex::Attribute|QXmlNodeModelIndex::Text|QXmlNodeModelIndex::ProcessingInstruction|QXmlNodeModelIndex::Comment|QXmlNodeModelIndex::Namespace,   // descendant;
    QXmlNodeModelIndex::Document|QXmlNodeModelIndex::Attribute|QXmlNodeModelIndex::Text|QXmlNodeModelIndex::ProcessingInstruction|QXmlNodeModelIndex::Comment|QXmlNodeModelIndex::Namespace,// attribute;
    QXmlNodeModelIndex::NodeKind(0),                         // self;
    QXmlNodeModelIndex::NodeKind(0),                         // descendant-or-self;
    QXmlNodeModelIndex::Document|QXmlNodeModelIndex::Attribute|QXmlNodeModelIndex::Text|QXmlNodeModelIndex::ProcessingInstruction|QXmlNodeModelIndex::Comment|QXmlNodeModelIndex::Namespace,    // namespace;
    QXmlNodeModelIndex::Document,                                         // following;
    QXmlNodeModelIndex::Document,                                         // parent;
    QXmlNodeModelIndex::Document,                                         // ancestor
    QXmlNodeModelIndex::Document|QXmlNodeModelIndex::Attribute|QXmlNodeModelIndex::Namespace,         // preceding-sibling;
    QXmlNodeModelIndex::Document|QXmlNodeModelIndex::Attribute|QXmlNodeModelIndex::Namespace,         // following-sibling;
    QXmlNodeModelIndex::Document,                                         // preceding;
    QXmlNodeModelIndex::NodeKind(0)                          // ancestor-or-self;
};

bool AxisStep::isAlwaysEmpty(const QXmlNodeModelIndex::Axis axis, const QXmlNodeModelIndex::NodeKind nodeKind)
{
    return (s_whenAxisNodeKindEmpty[(1 >> axis) - 1] & nodeKind) != 0;
}

AxisStep::AxisStep(const QXmlNodeModelIndex::Axis a,
                   const ItemType::Ptr &nt) : m_axis(a),
                                              m_nodeTest(nt)
{
    Q_ASSERT(m_nodeTest);
    Q_ASSERT_X(BuiltinTypes::node->xdtTypeMatches(m_nodeTest), Q_FUNC_INFO,
               "We assume we're a node type.");
}

Item AxisStep::mapToItem(const QXmlNodeModelIndex &node,
                         const DynamicContext::Ptr &context) const
{
    Q_ASSERT(!node.isNull());
    Q_ASSERT(Item(node).isNode());
    Q_ASSERT(Item(node));
    Q_UNUSED(context);

    if(m_nodeTest->itemMatches(Item(node)))
        return Item(node);
    else
        return Item();
}

Item::Iterator::Ptr AxisStep::evaluateSequence(const DynamicContext::Ptr &context) const
{
    /* If we don't have a focus, it's either a bug or our parent isn't a Path
     * that have advanced the focus iterator. Hence, attempt to advance the focus on our own. */
    if(!context->contextItem())
        context->focusIterator()->next();

    Q_ASSERT(context->contextItem());

    const QXmlNodeModelIndex::Iterator::Ptr source(context->contextItem().asNode().iterate(m_axis));

    return makeItemMappingIterator<Item>(ConstPtr(this), source, context);
}

Item AxisStep::evaluateSingleton(const DynamicContext::Ptr &context) const
{
    /* If we don't have a focus, it's either a bug or our parent isn't a Path
     * that have advanced the focus iterator. Hence, attempt to advance the focus on our own. */
    if(!context->contextItem())
        context->focusIterator()->next();

    Q_ASSERT(context->contextItem());

    const QXmlNodeModelIndex::Iterator::Ptr it(context->contextItem().asNode().iterate(m_axis));
    QXmlNodeModelIndex next(it->next());

    while(!next.isNull())
    {
        const Item candidate(mapToItem(next, context));

        if(candidate)
            return candidate;
        else
            next = it->next();
    };

    return Item();
}

Expression::Ptr AxisStep::typeCheck(const StaticContext::Ptr &context,
                                    const SequenceType::Ptr &reqType)
{
    if(m_axis == QXmlNodeModelIndex::AxisParent && *m_nodeTest == *BuiltinTypes::node)
    {
        /* We only rewrite parent::node() to ParentNodeAxis. */
        return rewrite(Expression::Ptr(new ParentNodeAxis()), context)->typeCheck(context, reqType);
    }
    /* TODO temporarily disabled
    else if(isAlwaysEmpty(m_axis, static_cast<const AnyNodeType *>(m_nodeTest.data())->nodeKind()))
        return EmptySequence::create(this, context);
        */
    else
        return EmptyContainer::typeCheck(context, reqType);
}

SequenceType::Ptr AxisStep::staticType() const
{
    Cardinality cardinality;

    if(m_axis == QXmlNodeModelIndex::AxisSelf || m_axis == QXmlNodeModelIndex::AxisParent)
        cardinality = Cardinality::zeroOrOne();
    else
        cardinality = Cardinality::zeroOrMore();

    return makeGenericSequenceType(m_nodeTest,
                                   cardinality);
}

SequenceType::List AxisStep::expectedOperandTypes() const
{
    SequenceType::List result;
    result.append(CommonSequenceTypes::ZeroOrMoreNodes);
    return result;
}

Expression::Properties AxisStep::properties() const
{
    return RequiresContextItem | DisableElimination;
}

ItemType::Ptr AxisStep::expectedContextItemType() const
{
    return BuiltinTypes::node;
}

ExpressionVisitorResult::Ptr AxisStep::accept(const ExpressionVisitor::Ptr &visitor) const
{
    return visitor->visit(this);
}

QXmlNodeModelIndex::Axis AxisStep::axis() const
{
    return m_axis;
}

QString AxisStep::axisName(const QXmlNodeModelIndex::Axis axis)
{
    const char *result = 0;

    switch(axis)
    {
        /* These must not be translated. */
        case QXmlNodeModelIndex::AxisAncestorOrSelf:    result = "ancestor-or-self";    break;
        case QXmlNodeModelIndex::AxisAncestor:          result = "ancestor";            break;
        case QXmlNodeModelIndex::AxisAttributeOrTop:    result = "attribute-or-top";    break;
        case QXmlNodeModelIndex::AxisAttribute:         result = "attribute";           break;
        case QXmlNodeModelIndex::AxisChildOrTop:        result = "child-or-top";        break;
        case QXmlNodeModelIndex::AxisChild:             result = "child";               break;
        case QXmlNodeModelIndex::AxisDescendantOrSelf:  result = "descendant-or-self";  break;
        case QXmlNodeModelIndex::AxisDescendant:        result = "descendant";          break;
        case QXmlNodeModelIndex::AxisFollowing:         result = "following";           break;
        case QXmlNodeModelIndex::AxisFollowingSibling:  result = "following-sibling";   break;
        case QXmlNodeModelIndex::AxisNamespace:         result = "namespace";           break;
        case QXmlNodeModelIndex::AxisParent:            result = "parent";              break;
        case QXmlNodeModelIndex::AxisPreceding:         result = "preceding";           break;
        case QXmlNodeModelIndex::AxisPrecedingSibling:  result = "preceding-sibling";   break;
        case QXmlNodeModelIndex::AxisSelf:              result = "self";                break;
    }

    Q_ASSERT_X(result, Q_FUNC_INFO, "An unknown axis type was apparently encountered.");
    return QString::fromLatin1(result);
}

PatternPriority AxisStep::patternPriority() const
{
    return static_cast<const AnyNodeType *>(m_nodeTest.data())->patternPriority();
}

Expression::ID AxisStep::id() const
{
    return IDAxisStep;
}

QT_END_NAMESPACE
