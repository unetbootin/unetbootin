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

#include "qcommonnamespaces_p.h"

#include "qboolean_p.h"
#include "qcommonvalues_p.h"
#include "qinteger_p.h"
#include "qatomicstring_p.h"

#include "qcomparestringfns_p.h"

QT_BEGIN_NAMESPACE

using namespace QPatternist;

Item CodepointEqualFN::evaluateSingleton(const DynamicContext::Ptr &context) const
{
    const Item op1(m_operands.first()->evaluateSingleton(context));
    if(!op1)
        return Item();

    const Item op2(m_operands.last()->evaluateSingleton(context));
    if(!op2)
        return Item();

    if(caseSensitivity() == Qt::CaseSensitive)
        return Boolean::fromValue(op1.stringValue() == op2.stringValue());
    else
    {
        const QString s1(op1.stringValue());
        const QString s2(op2.stringValue());

        return Boolean::fromValue(s1.length() == s2.length() &&
                                  s1.startsWith(s2, Qt::CaseInsensitive));
    }
}

Item CompareFN::evaluateSingleton(const DynamicContext::Ptr &context) const
{
    const Item op1(m_operands.first()->evaluateSingleton(context));
    if(!op1)
        return Item();

    const Item op2(m_operands.at(1)->evaluateSingleton(context));
    if(!op2)
        return Item();

    const int retval = caseSensitivity() == Qt::CaseSensitive
                       ? op1.stringValue().compare(op2.stringValue())
                       : op1.stringValue().toLower().compare(op2.stringValue().toLower());

    if(retval > 0)
        return CommonValues::IntegerOne;
    else if(retval < 0)
        return CommonValues::IntegerOneNegative;
    else
    {
        Q_ASSERT(retval == 0);
        return CommonValues::IntegerZero;
    }
}

QT_END_NAMESPACE
