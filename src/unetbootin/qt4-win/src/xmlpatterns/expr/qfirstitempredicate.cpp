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
#include "qgenericsequencetype_p.h"

#include "qfirstitempredicate_p.h"

QT_BEGIN_NAMESPACE

using namespace QPatternist;

FirstItemPredicate::FirstItemPredicate(const Expression::Ptr &source) : SingleContainer(source)
{
}

Item FirstItemPredicate::evaluateSingleton(const DynamicContext::Ptr &context) const
{
    /* If our operand allows the empty sequence, this function can return Item(), otherwise
     * it returns the first item. As simple as that. */
    return m_operand->evaluateSequence(context)->next();
}

SequenceType::Ptr FirstItemPredicate::staticType() const
{
    const SequenceType::Ptr t(m_operand->staticType());
    return makeGenericSequenceType(t->itemType(), t->cardinality().toWithoutMany());
}

Expression::Ptr FirstItemPredicate::compress(const StaticContext::Ptr &context)
{
    const Expression::Ptr me(SingleContainer::compress(context));
    if(me.data() != this)
        return me;

    if(m_operand->is(IDFirstItemPredicate))
        m_operand = m_operand->operands().first();

    return me;
}

SequenceType::List FirstItemPredicate::expectedOperandTypes() const
{
    SequenceType::List result;
    result.append(CommonSequenceTypes::ZeroOrMoreItems);
    return result;
}

ExpressionVisitorResult::Ptr FirstItemPredicate::accept(const ExpressionVisitor::Ptr &visitor) const
{
    return visitor->visit(this);
}

Expression::ID FirstItemPredicate::id() const
{
    return IDFirstItemPredicate;
}

QT_END_NAMESPACE
