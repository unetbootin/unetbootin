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
