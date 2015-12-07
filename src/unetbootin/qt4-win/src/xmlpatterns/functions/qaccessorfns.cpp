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
    if(me.data() != this)
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
