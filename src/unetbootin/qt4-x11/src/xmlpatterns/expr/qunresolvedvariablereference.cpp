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

#include "qunresolvedvariablereference_p.h"

QT_BEGIN_NAMESPACE

using namespace QPatternist;

UnresolvedVariableReference::UnresolvedVariableReference(const QXmlName &name) : m_name(name)
{
    Q_ASSERT(!m_name.isNull());
}

Expression::Ptr UnresolvedVariableReference::typeCheck(const StaticContext::Ptr &context,
                                                       const SequenceType::Ptr &reqType)
{
    /* We may be called before m_replacement is called, when we're part of a
     * function body whose type checking is performed for. See
     * UserFunctionCallsite::typeCheck(). */
    if(m_replacement)
        return m_replacement->typeCheck(context, reqType);
    else
        return EmptyContainer::typeCheck(context, reqType);
}

SequenceType::Ptr UnresolvedVariableReference::staticType() const
{
    /* We may be called by xmlpatternsview before the typeCheck() stage. */
    if(m_replacement)
        return m_replacement->staticType();
    else
        return CommonSequenceTypes::ZeroOrMoreItems;
}

SequenceType::List UnresolvedVariableReference::expectedOperandTypes() const
{
    return SequenceType::List();
}

ExpressionVisitorResult::Ptr UnresolvedVariableReference::accept(const ExpressionVisitor::Ptr &visitor) const
{
    return visitor->visit(this);
}

Expression::ID UnresolvedVariableReference::id() const
{
    return IDUnresolvedVariableReference;
}

QT_END_NAMESPACE
