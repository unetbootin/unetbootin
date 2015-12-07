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

#include "qboolean_p.h"
#include "qcommonsequencetypes_p.h"
#include "qcommonvalues_p.h"

#include "qdynamiccontextstore_p.h"

QT_BEGIN_NAMESPACE

using namespace QPatternist;

DynamicContextStore::DynamicContextStore(const Expression::Ptr &operand,
                                         const DynamicContext::Ptr &context) : SingleContainer(operand),
                                                                               m_context(context)
{
    Q_ASSERT(context);
}

bool DynamicContextStore::evaluateEBV(const DynamicContext::Ptr &) const
{
    return m_operand->evaluateEBV(m_context);
}

Item::Iterator::Ptr DynamicContextStore::evaluateSequence(const DynamicContext::Ptr &) const
{
    return m_operand->evaluateSequence(m_context);
}

Item DynamicContextStore::evaluateSingleton(const DynamicContext::Ptr &) const
{
    return m_operand->evaluateSingleton(m_context);
}

SequenceType::Ptr DynamicContextStore::staticType() const
{
    return m_operand->staticType();
}

SequenceType::List DynamicContextStore::expectedOperandTypes() const
{
    SequenceType::List result;
    result.append(CommonSequenceTypes::ZeroOrMoreItems);
    return result;
}

ExpressionVisitorResult::Ptr DynamicContextStore::accept(const ExpressionVisitor::Ptr &visitor) const
{
    return visitor->visit(this);
}

const SourceLocationReflection *DynamicContextStore::actualReflection() const
{
    return m_operand->actualReflection();
}

QT_END_NAMESPACE
