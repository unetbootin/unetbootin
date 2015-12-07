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

#include "qanyuri_p.h"
#include "qboolean_p.h"
#include "qbuiltintypes_p.h"
#include "qcommonvalues_p.h"
#include "qliteral_p.h"
#include "qitem_p.h"
#include "qqnamevalue_p.h"
#include "qatomicstring_p.h"

#include "qaccessorfns_p.h"

QT_BEGIN_NAMESPACE

using namespace QPatternist;

Item NodeNameFN::evaluateSingleton(const DynamicContext::Ptr &context) const
{
    const Item item(m_operands.first()->evaluateSingleton(context));

    if(item)
    {
        const QXmlName name(item.asNode().name());

        if(name.isNull())
            return Item();
        else
            return toItem(QNameValue::fromValue(context->namePool(), name));
    }
    else
        return Item();
}

Item NilledFN::evaluateSingleton(const DynamicContext::Ptr &context) const
{
    const Item item(m_operands.first()->evaluateSingleton(context));

    if(item && item.asNode().kind() == QXmlNodeModelIndex::Element)
    {
        /* We have no access to the PSVI -- always return false. */
        return CommonValues::BooleanFalse;
    }
    else
        return Item();
}

Item StringFN::evaluateSingleton(const DynamicContext::Ptr &context) const
{
    const Item item(m_operands.first()->evaluateSingleton(context));

    if(item)
        return AtomicString::fromValue(item.stringValue());
    else
        return CommonValues::EmptyString;
}

Expression::Ptr StringFN::typeCheck(const StaticContext::Ptr &context,
                                    const SequenceType::Ptr &reqType)
{
    const Expression::Ptr me(FunctionCall::typeCheck(context, reqType));
    if(me != this)
        return me;

    if(BuiltinTypes::xsString->xdtTypeMatches(m_operands.first()->staticType()->itemType()))
        return m_operands.first(); /* No need for string(), it's already a string. */
    else
        return me;
}

Item BaseURIFN::evaluateSingleton(const DynamicContext::Ptr &context) const
{
    const Item node(m_operands.first()->evaluateSingleton(context));

    if(node)
    {
        const QUrl base(node.asNode().baseUri());

        if(base.isEmpty())
            return Item();
        else if(base.isValid())
        {
            Q_ASSERT_X(!base.isRelative(), Q_FUNC_INFO,
                       "The base URI must be absolute.");
            return toItem(AnyURI::fromValue(base));
        }
        else
            return Item();
    }
    else
        return Item();
}

Item DocumentURIFN::evaluateSingleton(const DynamicContext::Ptr &context) const
{
    const Item node(m_operands.first()->evaluateSingleton(context));

    if(node)
    {
        const QUrl documentURI(node.asNode().documentUri());

        if(documentURI.isValid())
        {
            if(documentURI.isEmpty())
                return Item();
            else
            {
                Q_ASSERT_X(!documentURI.isRelative(), Q_FUNC_INFO,
                           "The document URI must be absolute.");
                return toItem(AnyURI::fromValue(documentURI));
            }
        }
        else
            return Item();
    }
    else
        return Item();
}

QT_END_NAMESPACE
