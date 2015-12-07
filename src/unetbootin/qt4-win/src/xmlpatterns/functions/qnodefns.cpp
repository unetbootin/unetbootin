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

#include "qabstractfloat_p.h"
#include "qanyuri_p.h"
#include "qboolean_p.h"
#include "qbuiltintypes_p.h"
#include "qcommonnamespaces_p.h"
#include "qcommonvalues_p.h"
#include "qliteral_p.h"
#include "qatomicstring_p.h"

#include "qnodefns_p.h"

QT_BEGIN_NAMESPACE

using namespace QPatternist;

Item NameFN::evaluateSingleton(const DynamicContext::Ptr &context) const
{
    const Item node(m_operands.first()->evaluateSingleton(context));

    if(node)
    {
        const QXmlName name(node.asNode().name());

        if(name.isNull())
            return CommonValues::EmptyString;
        else
            return AtomicString::fromValue(context->namePool()->toLexical(name));
    }
    else
        return CommonValues::EmptyString;
}

Item LocalNameFN::evaluateSingleton(const DynamicContext::Ptr &context) const
{
    const Item node(m_operands.first()->evaluateSingleton(context));

    if(node)
    {
        const QXmlName name(node.asNode().name());

        if(name.isNull())
            return CommonValues::EmptyString;
        else
            return AtomicString::fromValue(context->namePool()->stringForLocalName(name.localName()));
    }
    else
        return CommonValues::EmptyString;
}

Item NamespaceURIFN::evaluateSingleton(const DynamicContext::Ptr &context) const
{
    const Item node(m_operands.first()->evaluateSingleton(context));

    if(node)
    {
        const QXmlName name(node.asNode().name());

        if(name.isNull())
            return CommonValues::EmptyAnyURI;
        else
            return toItem(AnyURI::fromValue(context->namePool()->stringForNamespace(name.namespaceURI())));
    }
    else
        return CommonValues::EmptyAnyURI;
}

Item NumberFN::evaluateSingleton(const DynamicContext::Ptr &context) const
{
    const Item item(m_operands.first()->evaluateSingleton(context));

    if(!item)
        return CommonValues::DoubleNaN;

    const Item val(cast(item, context));
    Q_ASSERT(val);

    if(val.as<AtomicValue>()->hasError())
        return CommonValues::DoubleNaN;
    else
        return val;
}

Expression::Ptr NumberFN::typeCheck(const StaticContext::Ptr &context,
                                    const SequenceType::Ptr &reqType)
{
    const Expression::Ptr me(FunctionCall::typeCheck(context, reqType));
    const ItemType::Ptr sourceType(m_operands.first()->staticType()->itemType());

    if(BuiltinTypes::xsDouble->xdtTypeMatches(sourceType))
    {
        /* The operand is already xs:double, no need for fn:number(). */
        return m_operands.first()->typeCheck(context, reqType);
    }
    else if(prepareCasting(context, sourceType))
        return me;
    else
    {
        /* Casting to xs:double will never succeed and we would always return NaN.*/
        return wrapLiteral(CommonValues::DoubleNaN, context, this)->typeCheck(context, reqType);
    }
}

bool LangFN::isLangMatch(const QString &candidate, const QString &toMatch)
{
    if(QString::compare(candidate, toMatch, Qt::CaseInsensitive) == 0)
        return true;

    return candidate.startsWith(toMatch, Qt::CaseInsensitive)
           && candidate.length() > toMatch.length()
           && candidate.at(toMatch.length()) == QLatin1Char('-');
}

Item LangFN::evaluateSingleton(const DynamicContext::Ptr &context) const
{
    const Item langArg(m_operands.first()->evaluateSingleton(context));
    const QString lang(langArg ? langArg.stringValue() : QString());

    const QXmlName xmlLang(StandardNamespaces::xml, StandardLocalNames::lang, StandardPrefixes::xml);
    const QXmlNodeModelIndex langNode(m_operands.at(1)->evaluateSingleton(context).asNode());

    const QXmlNodeModelIndex::Iterator::Ptr ancestors(langNode.iterate(QXmlNodeModelIndex::AxisAncestorOrSelf));
    QXmlNodeModelIndex ancestor(ancestors->next());

    while(!ancestor.isNull())
    {
        const QXmlNodeModelIndex::Iterator::Ptr attributes(ancestor.iterate(QXmlNodeModelIndex::AxisAttribute));
        QXmlNodeModelIndex attribute(attributes->next());

        while(!attribute.isNull())
        {
            Q_ASSERT(attribute.kind() == QXmlNodeModelIndex::Attribute);

            if(attribute.name() == xmlLang)
            {
                if(isLangMatch(attribute.stringValue(), lang))
                    return CommonValues::BooleanTrue;
                else
                    return CommonValues::BooleanFalse;
            }

            attribute = attributes->next();
        }

        ancestor = ancestors->next();
    }

    return CommonValues::BooleanFalse;
}

Item RootFN::evaluateSingleton(const DynamicContext::Ptr &context) const
{
    const Item arg(m_operands.first()->evaluateSingleton(context));

    if(arg)
        return arg.asNode().root();
    else
        return Item();
}

SequenceType::Ptr RootFN::staticType() const
{
    if(m_operands.isEmpty())
        return makeGenericSequenceType(BuiltinTypes::node, Cardinality::exactlyOne());
    else
        return makeGenericSequenceType(BuiltinTypes::node, m_operands.first()->staticType()->cardinality().toWithoutMany());
}

QT_END_NAMESPACE
