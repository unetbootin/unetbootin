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
#include "qgenericsequencetype_p.h"
#include "qnodesortexpression_p.h"
#include "qpatternistlocale_p.h"
#include "qsequencemappingiterator_p.h"
#include "qtypechecker_p.h"

#include "qpath_p.h"

QT_BEGIN_NAMESPACE

using namespace QPatternist;

Path::Path(const Expression::Ptr &operand1,
           const Expression::Ptr &operand2) : PairContainer(operand1, operand2)
                                            , m_hasCreatedSorter(false)
                                            , m_isLast(false)
                                            , m_checkXPTY0018(false)
{
}

Item::Iterator::Ptr Path::mapToSequence(const Item &item,
                                        const DynamicContext::Ptr &context) const
{
    /* item is the focus here. That is in <e/>/1, item is <e/>. However,
     * we don't use it, since the context item is accessed through
     * DynamicContext::focusIterator() and friends. */
    Q_ASSERT(item);
    Q_UNUSED(item); /* Needed when compiling in release mode. */
    return m_operand2->evaluateSequence(context);
}

Item::Iterator::Ptr Path::evaluateSequence(const DynamicContext::Ptr &context) const
{
    /* Note, we use the old context for m_operand1. */
    const Item::Iterator::Ptr source(m_operand1->evaluateSequence(context));

    const DynamicContext::Ptr focus(context->createFocus());
    focus->setFocusIterator(source);

    const Item::Iterator::Ptr result(makeSequenceMappingIterator<Item>(ConstPtr(this), source, focus));

    if(m_checkXPTY0018)
    {
        /* This is an expensive code path, but it should happen very rarely. */

        enum FoundItem
        {
            FoundNone,
            FoundNode,
            FoundAtomicValue
        } hasFound = FoundNone;

        Item::List whenChecked;

        Item next(result->next());

        while(next)
        {
            const FoundItem found = next.isAtomicValue() ? FoundAtomicValue : FoundNode;

            if(hasFound != FoundNone && hasFound != found)
            {
                /* It's an atomic value and we've already found a node. Mixed content. */
                context->error(QtXmlPatterns::tr("The last step in a path must contain either nodes "
                                                 "or atomic values. It cannot be a mixture between the two."),
                               ReportContext::XPTY0018, this);
            }
            else
                hasFound = found;

            whenChecked.append(next);
            next = result->next();
        }

        return makeListIterator(whenChecked);
    }
    else
        return result;
}

Item Path::evaluateSingleton(const DynamicContext::Ptr &context) const
{
    /* This function is called if both operands' cardinality is exactly-one. Therefore
     * we manually go forward in the focus by calling next().
     *
     * We don't check for XPTY0018, only in evaluateSequence(), since if we're guaranteed
     * to evaluate to one item, we can only evaluate to one node or one atomic value.
     */

    /* Note, we use the old context for m_operand1. */
    const Item::Iterator::Ptr source(m_operand1->evaluateSequence(context));

    const DynamicContext::Ptr focus(context->createFocus());
    focus->setFocusIterator(source);

    /* This test is needed because if the focus is empty, we don't want to(nor can't) evaluate
     * the next step. */
    if(source->next())
        return m_operand2->evaluateSingleton(focus);
    else
        return Item();
}

Expression::Ptr Path::compress(const StaticContext::Ptr &context)
{
    const Expression::Ptr me(PairContainer::compress(context));

    /* We do this as late as we can, such that we pick up the most recent type
     * from the operand. */
    if(m_isLast && m_operand2->staticType()->itemType() == BuiltinTypes::item)
        m_checkXPTY0018 = true;

    return me;
}

Expression::Ptr Path::typeCheck(const StaticContext::Ptr &context,
                                const SequenceType::Ptr &reqType)
{
    m_operand2->announceFocusType(newFocusType());

    /* Here we apply the function conversion first, and with the error code that we
     * want -- XPTY0019 instead of XPTY0004. Unfortunately PairContainer::typeCheck()
     * will do the type check again, which is redundant.
     *
     * The type we expect, CommonSequenceTypes::ZeroOrMoreNodes() needs to be in sync with
     * what we return in expectedOperandTypes(). */
    m_operand1 = TypeChecker::applyFunctionConversion(m_operand1,
                                                      CommonSequenceTypes::ZeroOrMoreNodes,
                                                      context,
                                                      ReportContext::XPTY0019);

    /* If our step ends with atomic values, we cannot sort.
     *
     * We must smack the NodeSortExpression ontop before calling typeCheck(), since the latter
     * may insert an Atomizer, as possibly mandated by reqType. By doing it after, the Atomizer
     * will be a parent to NodeSortExpression, as opposed to a child.
     */
    if(!m_hasCreatedSorter)
    {
        m_hasCreatedSorter = true;

        return NodeSortExpression::wrapAround(Expression::Ptr(this), context)->typeCheck(context, reqType);
    }
    else
        return PairContainer::typeCheck(context, reqType);
}

SequenceType::List Path::expectedOperandTypes() const
{
    SequenceType::List result;

    /* This value needs to be in sync with what we pass to
     * applyFunctionConversion() in typeCheck() above. */
    result.append(CommonSequenceTypes::ZeroOrMoreNodes);

    result.append(CommonSequenceTypes::ZeroOrMoreItems);
    return result;
}

SequenceType::Ptr Path::staticType() const
{
    const SequenceType::Ptr opType(m_operand2->staticType());

    /* For each parent step, we evaluate the child step. So multiply the two
     * cardinalities. */
    return makeGenericSequenceType(opType->itemType(),
                                   m_operand1->staticType()->cardinality() * opType->cardinality());
}

ExpressionVisitorResult::Ptr Path::accept(const ExpressionVisitor::Ptr &visitor) const
{
    return visitor->visit(this);
}

Expression::Properties Path::properties() const
{
    return CreatesFocusForLast;
}

ItemType::Ptr Path::newFocusType() const
{
    return m_operand1->staticType()->itemType();
}

Expression::ID Path::id() const
{
    return IDPath;
}

QT_END_NAMESPACE
