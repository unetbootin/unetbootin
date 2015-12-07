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

#include <QtDebug>

#include "private/qxmlutils_p.h"
#include "qxpathhelper_p.h"

#include "qnamepool_p.h"

QT_BEGIN_NAMESPACE

using namespace QPatternist;

NamePool::NamePool()
{
    m_localNameMapping  .reserve(DefaultLocalNameCapacity   + StandardLocalNameCount);
    m_localNames        .reserve(DefaultLocalNameCapacity   + StandardLocalNameCount);
    m_namespaceMapping  .reserve(DefaultURICapacity         + StandardNamespaceCount);
    m_namespaces        .reserve(DefaultURICapacity         + StandardNamespaceCount);
    m_prefixes          .reserve(DefaultPrefixCapacity      + StandardPrefixCount);
    m_prefixMapping     .reserve(DefaultPrefixCapacity      + StandardPrefixCount);

    /* Namespaces. */
    {
        unlockedAllocateNamespace(QString());
        unlockedAllocateNamespace(QLatin1String("http://www.w3.org/2005/xpath-functions"));
        unlockedAllocateNamespace(QLatin1String("http://www.w3.org/2005/xquery-local-functions"));
        unlockedAllocateNamespace(QLatin1String("http://www.w3.org/XML/1998/namespace"));
        unlockedAllocateNamespace(QLatin1String("http://www.w3.org/2000/xmlns/"));
        unlockedAllocateNamespace(QLatin1String("http://www.w3.org/2001/XMLSchema"));
        unlockedAllocateNamespace(QLatin1String("http://www.w3.org/2001/XMLSchema-instance"));
        unlockedAllocateNamespace(QLatin1String("http://www.w3.org/1999/XSL/Transform"));

        /* For UndeclarePrefix, StopNamespaceInheritance and InternalXSLT. We use two
         * arbitrary strings that aren't used. For instance, if we just used an
         * empty QString, unlockedAllocateNamespace() would assign it
         * StandardNamespaces::empty. However, it's important that the string
         * is an invalid namespace, since otherwise user strings would get
         * assigned the same IDs. */
        unlockedAllocateNamespace(QLatin1String("  |  1  "));
        unlockedAllocateNamespace(QLatin1String("  |  2  "));
        unlockedAllocateNamespace(QLatin1String("  |  InternalXSLT"));

        Q_ASSERT_X(m_namespaces.count() == StandardNamespaceCount, Q_FUNC_INFO,
                   qPrintable(QString::fromLatin1("Expected is %1, actual is %2.").arg(StandardNamespaceCount).arg(m_namespaces.count())));
    }

    /* Prefixes. */
    {
        unlockedAllocatePrefix(QString());
        unlockedAllocatePrefix(QLatin1String("fn"));
        unlockedAllocatePrefix(QLatin1String("local"));
        unlockedAllocatePrefix(QLatin1String("xml"));
        unlockedAllocatePrefix(QLatin1String("xmlns"));
        unlockedAllocatePrefix(QLatin1String("xs"));
        unlockedAllocatePrefix(QLatin1String("xsi"));
        unlockedAllocatePrefix(QLatin1String("ns0"));
        unlockedAllocatePrefix(QLatin1String("|||")); /* Invalid NCName, for StopNamespaceInheritance. */

        Q_ASSERT_X(m_prefixes.count() == StandardPrefixCount, Q_FUNC_INFO,
                   qPrintable(QString::fromLatin1("Expected is %1, actual is %2.").arg(StandardPrefixCount).arg(m_prefixes.count())));
    }

    /* Local names. */
    {
        /* Same order as the enum in StandardLocalNames. That is, alphabetically. */
        unlockedAllocateLocalName(QLatin1String("abs"));
        unlockedAllocateLocalName(QLatin1String("adjust-dateTime-to-timezone"));
        unlockedAllocateLocalName(QLatin1String("adjust-date-to-timezone"));
        unlockedAllocateLocalName(QLatin1String("adjust-time-to-timezone"));
        unlockedAllocateLocalName(QLatin1String("all"));
        unlockedAllocateLocalName(QLatin1String("arity"));
        unlockedAllocateLocalName(QLatin1String("avg"));
        unlockedAllocateLocalName(QLatin1String("base"));
        unlockedAllocateLocalName(QLatin1String("base-uri"));
        unlockedAllocateLocalName(QLatin1String("boolean"));
        unlockedAllocateLocalName(QLatin1String("ceiling"));
        unlockedAllocateLocalName(QLatin1String("codepoint-equal"));
        unlockedAllocateLocalName(QLatin1String("codepoints-to-string"));
        unlockedAllocateLocalName(QLatin1String("collection"));
        unlockedAllocateLocalName(QLatin1String("compare"));
        unlockedAllocateLocalName(QLatin1String("concat"));
        unlockedAllocateLocalName(QLatin1String("contains"));
        unlockedAllocateLocalName(QLatin1String("count"));
        unlockedAllocateLocalName(QLatin1String("current"));
        unlockedAllocateLocalName(QLatin1String("current-date"));
        unlockedAllocateLocalName(QLatin1String("current-dateTime"));
        unlockedAllocateLocalName(QLatin1String("current-time"));
        unlockedAllocateLocalName(QLatin1String("data"));
        unlockedAllocateLocalName(QLatin1String("dateTime"));
        unlockedAllocateLocalName(QLatin1String("day-from-date"));
        unlockedAllocateLocalName(QLatin1String("day-from-dateTime"));
        unlockedAllocateLocalName(QLatin1String("days-from-duration"));
        unlockedAllocateLocalName(QLatin1String("deep-equal"));
        unlockedAllocateLocalName(QLatin1String("default"));
        unlockedAllocateLocalName(QLatin1String("default-collation"));
        unlockedAllocateLocalName(QLatin1String("distinct-values"));
        unlockedAllocateLocalName(QLatin1String("doc"));
        unlockedAllocateLocalName(QLatin1String("doc-available"));
        unlockedAllocateLocalName(QLatin1String("document"));
        unlockedAllocateLocalName(QLatin1String("document-uri"));
        unlockedAllocateLocalName(QLatin1String("element-available"));
        unlockedAllocateLocalName(QLatin1String("empty"));
        unlockedAllocateLocalName(QLatin1String("encode-for-uri"));
        unlockedAllocateLocalName(QLatin1String("ends-with"));
        unlockedAllocateLocalName(QLatin1String("error"));
        unlockedAllocateLocalName(QLatin1String("escape-html-uri"));
        unlockedAllocateLocalName(QLatin1String("exactly-one"));
        unlockedAllocateLocalName(QLatin1String("exists"));
        unlockedAllocateLocalName(QLatin1String("false"));
        unlockedAllocateLocalName(QLatin1String("floor"));
        unlockedAllocateLocalName(QLatin1String("function-available"));
        unlockedAllocateLocalName(QLatin1String("function-name"));
        unlockedAllocateLocalName(QLatin1String("generate-id"));
        unlockedAllocateLocalName(QLatin1String("generic-string-join"));
        unlockedAllocateLocalName(QLatin1String("hours-from-dateTime"));
        unlockedAllocateLocalName(QLatin1String("hours-from-duration"));
        unlockedAllocateLocalName(QLatin1String("hours-from-time"));
        unlockedAllocateLocalName(QLatin1String("id"));
        unlockedAllocateLocalName(QLatin1String("idref"));
        unlockedAllocateLocalName(QLatin1String("implicit-timezone"));
        unlockedAllocateLocalName(QLatin1String("index-of"));
        unlockedAllocateLocalName(QLatin1String("in-scope-prefixes"));
        unlockedAllocateLocalName(QLatin1String("insert-before"));
        unlockedAllocateLocalName(QLatin1String("iri-to-uri"));
        unlockedAllocateLocalName(QLatin1String("is-schema-aware"));
        unlockedAllocateLocalName(QLatin1String("key"));
        unlockedAllocateLocalName(QLatin1String("lang"));
        unlockedAllocateLocalName(QLatin1String("last"));
        unlockedAllocateLocalName(QLatin1String("local-name"));
        unlockedAllocateLocalName(QLatin1String("local-name-from-QName"));
        unlockedAllocateLocalName(QLatin1String("lower-case"));
        unlockedAllocateLocalName(QLatin1String("matches"));
        unlockedAllocateLocalName(QLatin1String("max"));
        unlockedAllocateLocalName(QLatin1String("min"));
        unlockedAllocateLocalName(QLatin1String("minutes-from-dateTime"));
        unlockedAllocateLocalName(QLatin1String("minutes-from-duration"));
        unlockedAllocateLocalName(QLatin1String("minutes-from-time"));
        unlockedAllocateLocalName(QLatin1String("month-from-date"));
        unlockedAllocateLocalName(QLatin1String("month-from-dateTime"));
        unlockedAllocateLocalName(QLatin1String("months-from-duration"));
        unlockedAllocateLocalName(QLatin1String("name"));
        unlockedAllocateLocalName(QLatin1String("namespace-uri"));
        unlockedAllocateLocalName(QLatin1String("namespace-uri-for-prefix"));
        unlockedAllocateLocalName(QLatin1String("namespace-uri-from-QName"));
        unlockedAllocateLocalName(QLatin1String("nilled"));
        unlockedAllocateLocalName(QLatin1String("node-name"));
        unlockedAllocateLocalName(QLatin1String("normalize-space"));
        unlockedAllocateLocalName(QLatin1String("normalize-unicode"));
        unlockedAllocateLocalName(QLatin1String("not"));
        unlockedAllocateLocalName(QLatin1String("number"));
        unlockedAllocateLocalName(QLatin1String("one-or-more"));
        unlockedAllocateLocalName(QLatin1String("position"));
        unlockedAllocateLocalName(QLatin1String("prefix-from-QName"));
        unlockedAllocateLocalName(QLatin1String("product-name"));
        unlockedAllocateLocalName(QLatin1String("product-version"));
        unlockedAllocateLocalName(QLatin1String("property-name"));
        unlockedAllocateLocalName(QLatin1String("QName"));
        unlockedAllocateLocalName(QLatin1String("remove"));
        unlockedAllocateLocalName(QLatin1String("replace"));
        unlockedAllocateLocalName(QLatin1String("resolve-QName"));
        unlockedAllocateLocalName(QLatin1String("resolve-uri"));
        unlockedAllocateLocalName(QLatin1String("reverse"));
        unlockedAllocateLocalName(QLatin1String("root"));
        unlockedAllocateLocalName(QLatin1String("round"));
        unlockedAllocateLocalName(QLatin1String("round-half-to-even"));
        unlockedAllocateLocalName(QLatin1String("seconds-from-dateTime"));
        unlockedAllocateLocalName(QLatin1String("seconds-from-duration"));
        unlockedAllocateLocalName(QLatin1String("seconds-from-time"));
        unlockedAllocateLocalName(QLatin1String("sourceValue"));
        unlockedAllocateLocalName(QLatin1String("starts-with"));
        unlockedAllocateLocalName(QLatin1String("static-base-uri"));
        unlockedAllocateLocalName(QLatin1String("string"));
        unlockedAllocateLocalName(QLatin1String("string-join"));
        unlockedAllocateLocalName(QLatin1String("string-length"));
        unlockedAllocateLocalName(QLatin1String("string-to-codepoints"));
        unlockedAllocateLocalName(QLatin1String("subsequence"));
        unlockedAllocateLocalName(QLatin1String("substring"));
        unlockedAllocateLocalName(QLatin1String("substring-after"));
        unlockedAllocateLocalName(QLatin1String("substring-before"));
        unlockedAllocateLocalName(QLatin1String("sum"));
        unlockedAllocateLocalName(QLatin1String("supports-backwards-compatibility"));
        unlockedAllocateLocalName(QLatin1String("supports-serialization"));
        unlockedAllocateLocalName(QLatin1String("system-property"));
        unlockedAllocateLocalName(QLatin1String("timezone-from-date"));
        unlockedAllocateLocalName(QLatin1String("timezone-from-dateTime"));
        unlockedAllocateLocalName(QLatin1String("timezone-from-time"));
        unlockedAllocateLocalName(QLatin1String("tokenize"));
        unlockedAllocateLocalName(QLatin1String("trace"));
        unlockedAllocateLocalName(QLatin1String("translate"));
        unlockedAllocateLocalName(QLatin1String("true"));
        unlockedAllocateLocalName(QLatin1String("type-available"));
        unlockedAllocateLocalName(QLatin1String("unordered"));
        unlockedAllocateLocalName(QLatin1String("unparsed-entity-public-id"));
        unlockedAllocateLocalName(QLatin1String("unparsed-entity-uri"));
        unlockedAllocateLocalName(QLatin1String("unparsed-text"));
        unlockedAllocateLocalName(QLatin1String("unparsed-text-available"));
        unlockedAllocateLocalName(QLatin1String("upper-case"));
        unlockedAllocateLocalName(QLatin1String("vendor"));
        unlockedAllocateLocalName(QLatin1String("vendor-url"));
        unlockedAllocateLocalName(QLatin1String("version"));
        unlockedAllocateLocalName(QLatin1String("xml"));
        unlockedAllocateLocalName(QLatin1String("xmlns"));
        unlockedAllocateLocalName(QLatin1String("year-from-date"));
        unlockedAllocateLocalName(QLatin1String("year-from-dateTime"));
        unlockedAllocateLocalName(QLatin1String("years-from-duration"));
        unlockedAllocateLocalName(QLatin1String("zero-or-one"));
        Q_ASSERT(m_localNames.count() == StandardLocalNameCount);
    }
}

QXmlName NamePool::allocateQName(const QString &uri, const QString &localName, const QString &prefix)
{
    QWriteLocker l(&lock);

    /*
    QString codepoints;
    for(int i = 0; i < localName.length(); ++i)
        codepoints.append(QString::number(localName.at(i).unicode()) + QLatin1Char(' '));

    pDebug() << Q_FUNC_INFO << localName << "codepoints:" << codepoints;
    */

    Q_ASSERT_X(QXmlUtils::isNCName(localName), Q_FUNC_INFO,
               qPrintable(QString::fromLatin1("'%1' is an invalid NCName.").arg(localName)));

    const QXmlName::NamespaceCode nsCode = unlockedAllocateNamespace(uri);
    const QXmlName::LocalNameCode localCode  = unlockedAllocateLocalName(localName);

    Q_ASSERT(prefix.isEmpty() || QXmlUtils::isNCName(prefix));
    const QXmlName::PrefixCode prefixCode = unlockedAllocatePrefix(prefix);

    return QXmlName(nsCode, localCode, prefixCode);
}

QXmlName NamePool::allocateBinding(const QString &prefix, const QString &uri)
{
    QWriteLocker l(&lock);

    Q_ASSERT_X(prefix.isEmpty() || QXmlUtils::isNCName(prefix), Q_FUNC_INFO,
               qPrintable(QString::fromLatin1("%1 is an invalid prefix.").arg(prefix)));
    const QXmlName::NamespaceCode nsCode = unlockedAllocateNamespace(uri);

    Q_ASSERT(prefix.isEmpty() || QXmlUtils::isNCName(prefix));
    const QXmlName::PrefixCode prefixCode = unlockedAllocatePrefix(prefix);

    return QXmlName(nsCode, StandardLocalNames::empty, prefixCode);
}

QXmlName::LocalNameCode NamePool::unlockedAllocateLocalName(const QString &ln)
{
    Q_ASSERT_X(QXmlUtils::isNCName(ln), Q_FUNC_INFO,
               qPrintable(QString::fromLatin1("Invalid local name: \"%1\"").arg(ln)));

    int indexInLocalNames = m_localNameMapping.value(ln, NoSuchValue);

    if(indexInLocalNames == NoSuchValue)
    {
        indexInLocalNames = m_localNames.count();
        m_localNames.append(ln);
        m_localNameMapping.insert(ln, indexInLocalNames);
    }

    return indexInLocalNames;
}

QXmlName::PrefixCode NamePool::unlockedAllocatePrefix(const QString &prefix)
{
    int indexInPrefixes = m_prefixMapping.value(prefix, NoSuchValue);

    if(indexInPrefixes == NoSuchValue)
    {
        indexInPrefixes = m_prefixes.count();
        m_prefixes.append(prefix);
        m_prefixMapping.insert(prefix, indexInPrefixes);
    }

    return indexInPrefixes;
}

QXmlName::NamespaceCode NamePool::unlockedAllocateNamespace(const QString &uri)
{
    int indexInURIs = m_namespaceMapping.value(uri, NoSuchValue);

    if(indexInURIs == NoSuchValue)
    {
        indexInURIs = m_namespaces.count();
        m_namespaces.append(uri);
        m_namespaceMapping.insert(uri, indexInURIs);
    }

    return indexInURIs;
}

const QString &NamePool::displayPrefix(const QXmlName::NamespaceCode nc) const
{
    switch(nc)
    {
        case StandardNamespaces::xmlns: return m_prefixes.at(StandardPrefixes::xmlns);
        case StandardNamespaces::local: return m_prefixes.at(StandardPrefixes::local);
        case StandardNamespaces::xs:    return m_prefixes.at(StandardPrefixes::xs);
        case StandardNamespaces::xml:   return m_prefixes.at(StandardPrefixes::xml);
        case StandardNamespaces::fn:    return m_prefixes.at(StandardPrefixes::fn);
        default:                        return m_prefixes.at(StandardPrefixes::empty);
    }
}

QString NamePool::displayName(const QXmlName qName) const
{
    QReadLocker l(&lock);

    if(qName.hasNamespace())
    {
        if(qName.namespaceURI() == StandardNamespaces::InternalXSLT)
            return QLatin1Char('#') + m_localNames.at(qName.localName());

        const QString &p = displayPrefix(qName.namespaceURI());

        if(p.isEmpty())
            return QLatin1Char('{') + m_namespaces.at(qName.namespaceURI()) + QLatin1Char('}') + toLexical(qName);
        else
            return p + QLatin1Char(':') + m_localNames.at(qName.localName());
    }
    else
        return m_localNames.at(qName.localName());
}

QString NamePool::toClarkName(const QXmlName &name) const
{
    if(name.isNull())
        return QLatin1String("QXmlName(null)");
    else
    {
        if(name.hasNamespace())
        {
            const QString ns(stringForNamespace(name.namespaceURI()));
            const QString p(stringForPrefix(name.prefix()));
            const QString l(stringForLocalName(name.localName()));

            return   QChar::fromLatin1('{')
                   + ns
                   + QChar::fromLatin1('}')
                   + (p.isEmpty() ? l : p + QChar::fromLatin1(':') + l);
        }
        else
            return stringForLocalName(name.localName());
    }
}

QXmlName NamePool::fromClarkName(const QString &clarkName)
{
    if(clarkName.isEmpty())
        return QXmlName();

    if(clarkName.at(0) == QLatin1Char('{'))
    {
        const int indexOfRight = clarkName.indexOf(QLatin1Char('}'));
        const QString qName(clarkName.right((clarkName.length() - indexOfRight) - 1));

        if(!XPathHelper::isQName(qName))
            return QXmlName();

        QString localName;
        QString prefix;

        XPathHelper::splitQName(qName, prefix, localName);

        return allocateQName(clarkName.mid(1, indexOfRight - 1),
                                         localName, prefix);
    }
    else
    {
        if(QXmlName::isNCName(clarkName))
            return allocateQName(QString(), clarkName);
        else
            return QXmlName();
    }
}
QT_END_NAMESPACE
