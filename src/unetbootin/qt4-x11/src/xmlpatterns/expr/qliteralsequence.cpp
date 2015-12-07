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
#include "qgenericsequencetype_p.h"

#include "qliteralsequence_p.h"

QT_BEGIN_NAMESPACE

using namespace QPatternist;

LiteralSequence::LiteralSequence(const Item::List &list) : m_list(list)
{
    Q_ASSERT(list.size() >= 2);
}

Item::Iterator::Ptr LiteralSequence::evaluateSequence(const DynamicContext::Ptr &) const
{
    return makeListIterator(m_list);
}

SequenceType::Ptr LiteralSequence::staticType() const
{
    const Item::List::const_iterator end(m_list.constEnd());
    Item::List::const_iterator it(m_list.constBegin());

    /* Load the first item. */
    ItemType::Ptr t((*it).type());
    ++it;

    for(; end != it; ++it)
        t |= (*it).type();

    return makeGenericSequenceType(t, Cardinality::fromCount(m_list.size()));
}

ExpressionVisitorResult::Ptr LiteralSequence::accept(const ExpressionVisitor::Ptr &visitor) const
{
    return visitor->visit(this);
}

Expression::ID LiteralSequence::id() const
{
    return IDExpressionSequence;
}

Expression::Properties LiteralSequence::properties() const
{
    return IsEvaluated;
}

QT_END_NAMESPACE
