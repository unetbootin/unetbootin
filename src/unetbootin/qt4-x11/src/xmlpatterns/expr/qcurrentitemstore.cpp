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
#include "qcurrentitemcontext_p.h"
#include "qstaticcurrentcontext_p.h"

#include "qcurrentitemstore_p.h"

QT_BEGIN_NAMESPACE

using namespace QPatternist;

CurrentItemStore::CurrentItemStore(const Expression::Ptr &operand) : SingleContainer(operand)
{
}

DynamicContext::Ptr CurrentItemStore::createContext(const DynamicContext::Ptr &old) const
{
    return DynamicContext::Ptr(new CurrentItemContext(old->contextItem(), old));
}

bool CurrentItemStore::evaluateEBV(const DynamicContext::Ptr &context) const
{
    return m_operand->evaluateEBV(createContext(context));
}

Item::Iterator::Ptr CurrentItemStore::evaluateSequence(const DynamicContext::Ptr &context) const
{
    return m_operand->evaluateSequence(createContext(context));
}

Item CurrentItemStore::evaluateSingleton(const DynamicContext::Ptr &context) const
{
    return m_operand->evaluateSingleton(createContext(context));
}

SequenceType::Ptr CurrentItemStore::staticType() const
{
    return m_operand->staticType();
}

SequenceType::List CurrentItemStore::expectedOperandTypes() const
{
    SequenceType::List result;
    result.append(CommonSequenceTypes::ZeroOrMoreItems);
    return result;
}

StaticContext::Ptr CurrentItemStore::newStaticContext(const StaticContext::Ptr &context)
{
    /* It might be we are generated despite there is no focus. In that case
     * an error will reported in case current() is used, but in any case we cannot
     * crash, so use item() in case we have no focus.
     *
     * Such a case is when we're inside a named template, and it's invoked
     * without focus. */
    const ItemType::Ptr t(context->contextItemType());
    return StaticContext::Ptr(new StaticCurrentContext(t ? t : BuiltinTypes::item, context));
}

Expression::Ptr CurrentItemStore::compress(const StaticContext::Ptr &context)
{
    const Expression::Ptr me(SingleContainer::compress(newStaticContext(context)));

    if(me != this)
        return me;
    else
    {
        /* If fn:current() isn't called, there's no point in us sticking
         * around. */
        if(m_operand->deepProperties().testFlag(RequiresCurrentItem))
            return me;
        else
            return m_operand;
    }
}

Expression::Ptr CurrentItemStore::typeCheck(const StaticContext::Ptr &context,
                                            const SequenceType::Ptr &reqType)
{
    return SingleContainer::typeCheck(newStaticContext(context), reqType);
}

ExpressionVisitorResult::Ptr CurrentItemStore::accept(const ExpressionVisitor::Ptr &visitor) const
{
    return visitor->visit(this);
}

const SourceLocationReflection *CurrentItemStore::actualReflection() const
{
    return m_operand->actualReflection();
}

Expression::Properties CurrentItemStore::properties() const
{
    return m_operand->properties() & (RequiresFocus | IsEvaluated | DisableElimination);
}

QT_END_NAMESPACE
