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

#include "qdaytimeduration_p.h"

#include "qfocus_p.h"

QT_BEGIN_NAMESPACE

using namespace QPatternist;

Focus::Focus(const DynamicContext::Ptr &prevContext) : DelegatingDynamicContext(prevContext),
                                                       m_contextSizeCached(-1)
{
    Q_ASSERT(prevContext);
    Q_ASSERT(prevContext != this);
}

xsInteger Focus::contextPosition() const
{
    Q_ASSERT(m_focusIterator);
    return m_focusIterator->position();
}

Item Focus::contextItem() const
{
    Q_ASSERT(m_focusIterator);
    return m_focusIterator->current();
}

xsInteger Focus::contextSize()
{
    Q_ASSERT(m_focusIterator);
    if(m_contextSizeCached == -1)
        m_contextSizeCached = m_focusIterator->copy()->count();

    Q_ASSERT_X(m_contextSizeCached == m_focusIterator->copy()->count(), Q_FUNC_INFO,
               "If our cache is not the same as the real count, something is wrong.");

    return m_contextSizeCached;
}

void Focus::setFocusIterator(const Item::Iterator::Ptr &it)
{
    Q_ASSERT(it);
    m_focusIterator = it;
}

Item::Iterator::Ptr Focus::focusIterator() const
{
    return m_focusIterator;
}

Item Focus::currentItem() const
{
    /* In the case that there is no top level expression that creates a focus,
     * fn:current() should return the focus. This logic achieves this.
     * Effectively we traverse up our "context stack" through recursion, and
     * start returning when we've found the top most focus. */

    const Item current(m_prevContext->currentItem());

    if(current.isNull())
        return m_focusIterator->current();
    else
        return current;
}

QT_END_NAMESPACE
