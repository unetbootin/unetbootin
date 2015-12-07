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

#include "qboolean_p.h"
#include "qcommonsequencetypes_p.h"
#include "qcommonvalues_p.h"
#include "qliteral_p.h"

#include "qinstanceof_p.h"

QT_BEGIN_NAMESPACE

using namespace QPatternist;

InstanceOf::InstanceOf(const Expression::Ptr &operand,
                       const SequenceType::Ptr &tType) : SingleContainer(operand),
                                                         m_targetType(tType)
{
    Q_ASSERT(operand);
    Q_ASSERT(m_targetType);
}

bool InstanceOf::evaluateEBV(const DynamicContext::Ptr &context) const
{
    const Item::Iterator::Ptr it(m_operand->evaluateSequence(context));
    Item item(it->next());
    unsigned int count = 1;

    if(!item)
        return m_targetType->cardinality().allowsEmpty();

    do
    {
        if(!m_targetType->itemType()->itemMatches(item))
            return false;

        if(count == 2 && !m_targetType->cardinality().allowsMany())
            return false;

        item = it->next();
        ++count;
    } while(item);

    return true;
}

Expression::Ptr InstanceOf::compress(const StaticContext::Ptr &context)
{
    const Expression::Ptr me(SingleContainer::compress(context));

    if(me.data() != this || m_operand->has(DisableTypingDeduction))
        return me;

    const SequenceType::Ptr opType(m_operand->staticType());
    const ItemType::Ptr itType(m_targetType->itemType());
    const ItemType::Ptr ioType(opType->itemType());

    if(m_targetType->cardinality().isMatch(opType->cardinality()))
    {
        if(itType->xdtTypeMatches(ioType))
            return wrapLiteral(CommonValues::BooleanTrue, context, this);
        else if(!ioType->xdtTypeMatches(itType))
        {
            return wrapLiteral(CommonValues::BooleanFalse, context, this);
        }
    }
    /* The cardinality is not guaranteed to match; it will need testing. */
    else if(!ioType->xdtTypeMatches(itType))
    {
        /* There's no way it's gonna match. The cardinality is not only
         * wrong, but the item type as well. */
        return wrapLiteral(CommonValues::BooleanFalse, context, this);
    }

    return me;
}

SequenceType::Ptr InstanceOf::targetType() const
{
    return m_targetType;
}

SequenceType::Ptr InstanceOf::staticType() const
{
    return CommonSequenceTypes::ExactlyOneBoolean;
}

SequenceType::List InstanceOf::expectedOperandTypes() const
{
    SequenceType::List result;
    result.append(CommonSequenceTypes::ZeroOrMoreItems);
    return result;
}

ExpressionVisitorResult::Ptr InstanceOf::accept(const ExpressionVisitor::Ptr &visitor) const
{
    return visitor->visit(this);
}

QT_END_NAMESPACE
