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
        return static_cast<QXmlNodeModelIndex::NodeKind>(int(op1) | int(op2));
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
    static_cast<QXmlNodeModelIndex::NodeKind>(0),                         // self;
    static_cast<QXmlNodeModelIndex::NodeKind>(0),                         // descendant-or-self;
    QXmlNodeModelIndex::Document|QXmlNodeModelIndex::Attribute|QXmlNodeModelIndex::Text|QXmlNodeModelIndex::ProcessingInstruction|QXmlNodeModelIndex::Comment|QXmlNodeModelIndex::Namespace,    // namespace;
    QXmlNodeModelIndex::Document,                                         // following;
    QXmlNodeModelIndex::Document,                                         // parent;
    QXmlNodeModelIndex::Document,                                         // ancestor
    QXmlNodeModelIndex::Document|QXmlNodeModelIndex::Attribute|QXmlNodeModelIndex::Namespace,         // preceding-sibling;
    QXmlNodeModelIndex::Document|QXmlNodeModelIndex::Attribute|QXmlNodeModelIndex::Namespace,         // following-sibling;
    QXmlNodeModelIndex::Document,                                         // preceding;
    static_cast<QXmlNodeModelIndex::NodeKind>(0)                          // ancestor-or-self;
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
}

Item AxisStep::mapToItem(const QXmlNodeModelIndex &node,
                         const DynamicContext::Ptr &) const
{
    Q_ASSERT(!node.isNull());
    Q_ASSERT(Item(node).isNode());
    Q_ASSERT(Item(node));

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
    return makeGenericSequenceType(m_nodeTest,
                                   m_axis == QXmlNodeModelIndex::AxisAttribute ? Cardinality::zeroOrOne() : Cardinality::zeroOrMore());
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
        case QXmlNodeModelIndex::AxisChild:             result = "child";               break;
        case QXmlNodeModelIndex::AxisDescendant:        result = "descendant";          break;
        case QXmlNodeModelIndex::AxisAttribute:         result = "attribute";           break;
        case QXmlNodeModelIndex::AxisSelf:              result = "self";                break;
        case QXmlNodeModelIndex::AxisDescendantOrSelf:  result = "descendant-or-self";  break;
        case QXmlNodeModelIndex::AxisFollowingSibling:  result = "following-sibling";   break;
        case QXmlNodeModelIndex::AxisNamespace:         result = "namespace";           break;
        case QXmlNodeModelIndex::AxisFollowing:         result = "following";           break;
        case QXmlNodeModelIndex::AxisParent:            result = "parent";              break;
        case QXmlNodeModelIndex::AxisAncestor:          result = "ancestor";            break;
        case QXmlNodeModelIndex::AxisPrecedingSibling:  result = "preceding-sibling";   break;
        case QXmlNodeModelIndex::AxisPreceding:         result = "preceding";           break;
        case QXmlNodeModelIndex::AxisAncestorOrSelf:    result = "ancestor-or-self";    break;
    }

    Q_ASSERT_X(result, Q_FUNC_INFO, "An unknown axis type was apparently encountered.");
    return QString::fromLatin1(result);
}

QT_END_NAMESPACE
