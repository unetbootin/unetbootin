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
#include "qbuiltintypes_p.h"
#include "qcommonvalues_p.h"
#include "qpatternistlocale_p.h"
#include "qnodenamespaceresolver_p.h"
#include "qqnameconstructor_p.h"
#include "qqnamevalue_p.h"
#include "qatomicstring_p.h"
#include "qxpathhelper_p.h"

#include "qqnamefns_p.h"

QT_BEGIN_NAMESPACE

using namespace QPatternist;

Item QNameFN::evaluateSingleton(const DynamicContext::Ptr &context) const
{
    const Item paramURI(m_operands.first()->evaluateSingleton(context));
    const QString paramQName(m_operands.last()->evaluateSingleton(context).stringValue());

    QString ns;
    if(paramURI)
        ns = paramURI.stringValue();

    if(!XPathHelper::isQName(paramQName))
    {
        context->error(QtXmlPatterns::tr("%1 is an invalid %2").arg(formatData(paramQName),
                                                     formatType(context->namePool(), BuiltinTypes::xsQName)),
                       ReportContext::FOCA0002, this);
        return Item();
    }

    QString prefix;
    QString lname;
    XPathHelper::splitQName(paramQName, prefix, lname);
    const QXmlName n(context->namePool()->allocateQName(ns, lname, prefix));

    if(ns.isEmpty())
    {
        if(prefix.isEmpty())
            return toItem(QNameValue::fromValue(context->namePool(), n));
        else
        {
            context->error(QtXmlPatterns::tr(
                              "If the first argument is the empty sequence or "
                              "a zero-length string (no namespace), a prefix "
                              "cannot be specified. Prefix %1 was specified.")
                           .arg(formatKeyword(prefix)),
                           ReportContext::FOCA0002, this);
            return Item(); /* Silence compiler warning. */
        }
    }
    else
        return toItem(QNameValue::fromValue(context->namePool(), n));
}

Item ResolveQNameFN::evaluateSingleton(const DynamicContext::Ptr &context) const
{
    const Item itemName(m_operands.first()->evaluateSingleton(context));

    if(!itemName)
        return Item();

    const NamespaceResolver::Ptr resolver(new NodeNamespaceResolver(m_operands.last()->evaluateSingleton(context)));
    const QString strName(itemName.stringValue());
    const QXmlName name = QNameConstructor::expandQName<DynamicContext::Ptr,
                                                     ReportContext::FOCA0002,
                                                     ReportContext::FONS0004>(strName,
                                                                              context,
                                                                              resolver,
                                                                              this);

    return toItem(QNameValue::fromValue(context->namePool(), name));
}

Item PrefixFromQNameFN::evaluateSingleton(const DynamicContext::Ptr &context) const
{
    const QNameValue::Ptr arg(m_operands.first()->evaluateSingleton(context).as<QNameValue>());
    if(!arg)
        return Item();

    const QString prefix(context->namePool()->stringForPrefix(arg->qName().prefix()));

    if(prefix.isEmpty())
        return Item();
    else
        return AtomicString::fromValue(context->namePool()->stringForPrefix(arg->qName().prefix()));
}

Item LocalNameFromQNameFN::evaluateSingleton(const DynamicContext::Ptr &context) const
{
    const QNameValue::Ptr arg(m_operands.first()->evaluateSingleton(context).as<QNameValue>());
    return arg ? toItem(AtomicString::fromValue(context->namePool()->stringForLocalName(arg->qName().localName()))) : Item();
}

Item NamespaceURIFromQNameFN::evaluateSingleton(const DynamicContext::Ptr &context) const
{
    const QNameValue::Ptr arg(m_operands.first()->evaluateSingleton(context).as<QNameValue>());
    return arg ? toItem(AnyURI::fromValue(context->namePool()->stringForNamespace(arg->qName().namespaceURI()))) : Item();
}

Item NamespaceURIForPrefixFN::evaluateSingleton(const DynamicContext::Ptr &context) const
{
    const Item prefixItem(m_operands.first()->evaluateSingleton(context));
    QXmlName::PrefixCode prefix;

    if(prefixItem)
        prefix = context->namePool()->allocatePrefix(prefixItem.stringValue());
    else
        prefix = StandardPrefixes::empty;

    const Item eleItem(m_operands.last()->evaluateSingleton(context));
    Q_ASSERT(eleItem);

    const QXmlName::NamespaceCode ns = eleItem.asNode().namespaceForPrefix(prefix);

    if(ns == NamespaceResolver::NoBinding)
    {
        /* This is a bit tricky. The default namespace is not considered an in-scope binding
         * on a node, but the specification for this function do consider it a binding and therefore
         * the empty string. */
        if(prefix == StandardPrefixes::empty)
            return CommonValues::EmptyString;
        else
            return Item();
    }
    else
        return toItem(AnyURI::fromValue(context->namePool()->stringForNamespace(ns)));
}

Item::Iterator::Ptr InScopePrefixesFN::evaluateSequence(const DynamicContext::Ptr &context) const
{
    const Item e(m_operands.first()->evaluateSingleton(context));

    const QVector<QXmlName> nbs(e.asNode().namespaceBindings());
    const int len = nbs.size();
    const NamePool::Ptr np(context->namePool());

    QList<Item> result;

    for(int i = 0; i < len; ++i)
        result.append(AtomicString::fromValue(np->stringForPrefix(nbs.at(i).prefix())));

    return makeListIterator(result);
}

QT_END_NAMESPACE
