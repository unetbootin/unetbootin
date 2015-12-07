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

#include <QStack>

#include "qabstractxmlreceiver.h"
#include "qacceliterators_p.h"
#include "qacceltree_p.h"
#include "qatomicstring_p.h"
#include "qcommonvalues_p.h"
#include "qcompressedwhitespace_p.h"
#include "qdebug_p.h"
#include "quntypedatomic_p.h"
#include "qxpathhelper_p.h"

QT_BEGIN_NAMESPACE

using namespace QPatternist;

void AccelTree::printStats(const NamePool::Ptr &np) const
{
    Q_ASSERT(np);
#ifdef QT_NO_DEBUG
    Q_UNUSED(np); /* Needed when compiling in release mode. */
#else
    const int len = basicData.count();

    pDebug() << "AccelTree stats for" << (m_documentURI.isEmpty() ? QString::fromLatin1("<empty URI>") : m_documentURI.toString());
    pDebug() << "Maximum pre number:" << maximumPreNumber();
    pDebug() << "+---------------+-------+-------+---------------+-------+--------------+-------+";
    pDebug() << "| Pre number    | Depth | Size  | Post Number   | Kind  | Name         | Value |";
    pDebug() << "+---------------+-------+-------+---------------+-------+--------------+-------+";
    for(int i = 0; i < len; ++i)
    {
        const BasicNodeData &v = basicData.at(i);
        pDebug() << "|" << i
                 << "\t\t|" << v.depth()
                 << "\t|" << v.size()
                 << "\t|" << postNumber(i)
                 << "\t|" << v.kind()
                 << "\t\t|" << (v.name().isNull() ? QString::fromLatin1("(none)") : np->displayName(v.name()))
                 << "\t\t|" << ((v.kind() == QXmlNodeModelIndex::Text && isCompressed(i)) ? CompressedWhitespace::decompress(data.value(i))
                                                                                          : data.value(i))
                 << "\t|";
        /*
        pDebug() << "|" << QString().arg(i, 14)
                 << "|" << QString().arg(v.depth(), 6)
                 << "|" << QString().arg(v.size(), 6)
                 << "|" << QString().arg(postNumber(i), 14)
                 << "|" << QString().arg(v.kind(), 6)
                 << "|";
                 */
    }
    pDebug() << "+---------------+-------+-------+---------------+-------+--------------+";
    pDebug() << "Namespaces(" << namespaces.count() << "):";

    QHashIterator<PreNumber, QVector<QXmlName> > it(namespaces);
    while(it.hasNext())
    {
        it.next();

        pDebug() << "PreNumber: " << QString::number(it.key());
        for(int i = 0; i < it.value().count(); ++i)
            pDebug() << "\t\t" << np->stringForPrefix(it.value().at(i).prefix()) << " = " << np->stringForNamespace(it.value().at(i).namespaceURI());
    }

#endif
}

QUrl AccelTree::baseUri(const QXmlNodeModelIndex &ni) const
{
    switch(kind(toPreNumber(ni)))
    {
        case QXmlNodeModelIndex::Document:
            return baseUri();
        case QXmlNodeModelIndex::Element:
        {
            const QXmlNodeModelIndex::Iterator::Ptr it(iterate(ni, QXmlNodeModelIndex::AxisAttribute));
            QXmlNodeModelIndex next(it->next());

            while(!next.isNull())
            {
                if(next.name() == QXmlName(StandardNamespaces::xml, StandardLocalNames::base))
                {
                    const QUrl candidate(next.stringValue());
                    //  TODO. The xml:base spec says to do URI escaping here.

                    if(!candidate.isValid())
                        return QUrl();
                    else if(candidate.isRelative())
                    {
                        const QXmlNodeModelIndex par(parent(ni));

                        if(par.isNull())
                            return baseUri().resolved(candidate);
                        else
                            return par.baseUri().resolved(candidate);
                    }
                    else
                        return candidate;
                }

                next = it->next();
            }

            /* We have no xml:base-attribute. Can any parent supply us a base URI? */
            const QXmlNodeModelIndex par(parent(ni));

            if(par.isNull())
                return baseUri();
            else
                return par.baseUri();
        }
        case QXmlNodeModelIndex::ProcessingInstruction:
        /* Fallthrough. */
        case QXmlNodeModelIndex::Comment:
        /* Fallthrough. */
        case QXmlNodeModelIndex::Attribute:
        /* Fallthrough. */
        case QXmlNodeModelIndex::Text:
        {
            const QXmlNodeModelIndex par(ni.iterate(QXmlNodeModelIndex::AxisParent)->next());
            if(par.isNull())
                return QUrl();
            else
                return par.baseUri();
        }
        case QXmlNodeModelIndex::Namespace:
            return QUrl();
    }

    Q_ASSERT_X(false, Q_FUNC_INFO, "This line is never supposed to be reached.");
    return QUrl();
}

QUrl AccelTree::documentUri(const QXmlNodeModelIndex &ni) const
{
    if(kind(toPreNumber(ni)) == QXmlNodeModelIndex::Document)
        return documentUri();
    else
        return QUrl();
}

QXmlNodeModelIndex::NodeKind AccelTree::kind(const QXmlNodeModelIndex &ni) const
{
    return kind(toPreNumber(ni));
}

QXmlNodeModelIndex::DocumentOrder AccelTree::compareOrder(const QXmlNodeModelIndex &ni1,
                                                          const QXmlNodeModelIndex &ni2) const
{
    Q_ASSERT_X(ni1.model() == ni2.model(), Q_FUNC_INFO,
               "The API docs guarantees the two nodes are from the same model");

    const PreNumber p1 = ni1.data();
    const PreNumber p2 = ni2.data();

    if(p1 == p2)
        return QXmlNodeModelIndex::Is;
    else if(p1 < p2)
        return QXmlNodeModelIndex::Precedes;
    else
        return QXmlNodeModelIndex::Follows;
}

QXmlNodeModelIndex AccelTree::root(const QXmlNodeModelIndex &) const
{
    return createIndex(qint64(0));
}

QXmlNodeModelIndex AccelTree::parent(const QXmlNodeModelIndex &ni) const
{
    const AccelTree::PreNumber p = basicData.at(toPreNumber(ni)).parent();

    if(p == -1)
        return QXmlNodeModelIndex();
    else
        return createIndex(p);
}

QXmlNodeModelIndex::Iterator::Ptr AccelTree::iterate(const QXmlNodeModelIndex &ni,
                                                     QXmlNodeModelIndex::Axis axis) const
{
    const PreNumber preNumber = toPreNumber(ni);

    switch(axis)
    {
        case QXmlNodeModelIndex::AxisChildOrTop:
        {
            if(!hasParent(preNumber))
            {
                switch(kind(preNumber))
                {
                    case QXmlNodeModelIndex::Comment:
                    /* Fallthrough. */
                    case QXmlNodeModelIndex::ProcessingInstruction:
                    /* Fallthrough. */
                    case QXmlNodeModelIndex::Element:
                    /* Fallthrough. */
                    case QXmlNodeModelIndex::Text:
                        return makeSingletonIterator(ni);
                    case QXmlNodeModelIndex::Attribute:
                    /* Fallthrough. */
                    case QXmlNodeModelIndex::Document:
                    /* Fallthrough. */
                    case QXmlNodeModelIndex::Namespace:
                        /* Do nothing. */;
                }
            }
            /* Else, fallthrough to AxisChild. */
        }
        case QXmlNodeModelIndex::AxisChild:
        {
            if(hasChildren(preNumber))
                return QXmlNodeModelIndex::Iterator::Ptr(new ChildIterator(this, preNumber));
            else
                return makeEmptyIterator<QXmlNodeModelIndex>();
        }
        case QXmlNodeModelIndex::AxisAncestor:
        {
            if(hasParent(preNumber))
                return QXmlNodeModelIndex::Iterator::Ptr(new AncestorIterator<false>(this, preNumber));
            else
                return makeEmptyIterator<QXmlNodeModelIndex>();
        }
        case QXmlNodeModelIndex::AxisAncestorOrSelf:
            return QXmlNodeModelIndex::Iterator::Ptr(new AncestorIterator<true>(this, preNumber));
        case QXmlNodeModelIndex::AxisParent:
        {
            if(hasParent(preNumber))
                return makeSingletonIterator(createIndex(parent(preNumber)));
            else
                return makeEmptyIterator<QXmlNodeModelIndex>();
        }
        case QXmlNodeModelIndex::AxisDescendant:
        {
            if(hasChildren(preNumber))
                return QXmlNodeModelIndex::Iterator::Ptr(new DescendantIterator<false>(this, preNumber));
            else
                return makeEmptyIterator<QXmlNodeModelIndex>();
        }
        case QXmlNodeModelIndex::AxisDescendantOrSelf:
            return QXmlNodeModelIndex::Iterator::Ptr(new DescendantIterator<true>(this, preNumber));
        case QXmlNodeModelIndex::AxisFollowing:
        {
            if(preNumber == maximumPreNumber())
                return makeEmptyIterator<QXmlNodeModelIndex>();
            else
                return QXmlNodeModelIndex::Iterator::Ptr(new FollowingIterator(this, preNumber));
        }
        case QXmlNodeModelIndex::AxisAttributeOrTop:
        {
            if(!hasParent(preNumber) && kind(preNumber) == QXmlNodeModelIndex::Attribute)
                return makeSingletonIterator(ni);
            /* Else, falthrough to AxisAttribute. */
        }
        case QXmlNodeModelIndex::AxisAttribute:
        {
            if(hasChildren(preNumber) && kind(preNumber + 1) == QXmlNodeModelIndex::Attribute)
                return QXmlNodeModelIndex::Iterator::Ptr(new AttributeIterator(this, preNumber));
            else
                return makeEmptyIterator<QXmlNodeModelIndex>();
        }
        case QXmlNodeModelIndex::AxisPreceding:
        {
            if(preNumber == 0)
                return makeEmptyIterator<QXmlNodeModelIndex>();
            else
                return QXmlNodeModelIndex::Iterator::Ptr(new PrecedingIterator(this, preNumber));
        }
        case QXmlNodeModelIndex::AxisSelf:
            return makeSingletonIterator(createIndex(toPreNumber(ni)));
        case QXmlNodeModelIndex::AxisFollowingSibling:
        {
            if(preNumber == maximumPreNumber())
                return makeEmptyIterator<QXmlNodeModelIndex>();
            else
                return QXmlNodeModelIndex::Iterator::Ptr(new SiblingIterator<true>(this, preNumber));
        }
        case QXmlNodeModelIndex::AxisPrecedingSibling:
        {
            if(preNumber == 0)
                return makeEmptyIterator<QXmlNodeModelIndex>();
            else
                return QXmlNodeModelIndex::Iterator::Ptr(new SiblingIterator<false>(this, preNumber));
        }
        case QXmlNodeModelIndex::AxisNamespace:
            return makeEmptyIterator<QXmlNodeModelIndex>();
    }

    Q_ASSERT(false);
    return QXmlNodeModelIndex::Iterator::Ptr();
}

QXmlNodeModelIndex AccelTree::nextFromSimpleAxis(QAbstractXmlNodeModel::SimpleAxis,
                                                 const QXmlNodeModelIndex&) const
{
    Q_ASSERT_X(false, Q_FUNC_INFO, "This function is not supposed to be called.");
    return QXmlNodeModelIndex();
}

QVector<QXmlNodeModelIndex> AccelTree::attributes(const QXmlNodeModelIndex &element) const
{
    Q_ASSERT_X(false, Q_FUNC_INFO, "This function is not supposed to be called.");
    Q_UNUSED(element);
    return QVector<QXmlNodeModelIndex>();
}

QXmlName AccelTree::name(const QXmlNodeModelIndex &ni) const
{
    /* If this node type does not have a name(for instance, it's a comment)
     * we will return the default constructed value, which is conformant with
     * this function's contract. */
    return name(toPreNumber(ni));
}

QVector<QXmlName> AccelTree::namespaceBindings(const QXmlNodeModelIndex &ni) const
{
    /* We get a hold of the ancestor, and loop them in reverse document
     * order(first the parent, then the parent's parent, etc). As soon
     * we find a binding that hasn't already been added, we add it to the
     * result list. In that way, declarations appearing further down override
     * those further up. */

    const PreNumber preNumber = toPreNumber(ni);

    const QXmlNodeModelIndex::Iterator::Ptr it(new AncestorIterator<true>(this, preNumber));
    QVector<QXmlName> result;
    QXmlNodeModelIndex n(it->next());

    /* Whether xmlns="" has been encountered. */
    bool hasUndeclaration = false;

    while(!n.isNull())
    {
        const QVector<QXmlName> &forNode = namespaces.value(toPreNumber(n));
        const int len = forNode.size();
        bool stopInheritance = false;

        for(int i = 0; i < len; ++i)
        {
            const QXmlName &nsb = forNode.at(i);

            if(nsb.namespaceURI() == StandardNamespaces::StopNamespaceInheritance)
            {
                stopInheritance = true;
                continue;
            }

            if(nsb.prefix() == StandardPrefixes::empty &&
               nsb.namespaceURI() == StandardNamespaces::empty)
            {
                hasUndeclaration = true;
                continue;
            }

            if(!hasPrefix(result, nsb.prefix()))
            {
                /* We've already encountered an undeclaration, so we're supposed to skip
                 * them. */
                if(hasUndeclaration && nsb.prefix() == StandardPrefixes::empty)
                    continue;
                else
                    result.append(nsb);
            }
        }

        if(stopInheritance)
            break;
        else
            n = it->next();
    }

    result.append(QXmlName(StandardNamespaces::xml, StandardLocalNames::empty, StandardPrefixes::xml));

    return result;
}

void AccelTree::sendNamespaces(const QXmlNodeModelIndex &n,
                               QAbstractXmlReceiver *const receiver) const
{
    Q_ASSERT(n.kind() == QXmlNodeModelIndex::Element);

    const QXmlNodeModelIndex::Iterator::Ptr it(iterate(n, QXmlNodeModelIndex::AxisAncestorOrSelf));
    QXmlNodeModelIndex next(it->next());
    QVector<QXmlName::PrefixCode> alreadySent;

    while(!next.isNull())
    {
        const PreNumber preNumber = toPreNumber(next);

        const QVector<QXmlName> &nss = namespaces.value(preNumber);

        /* This is by far the most common case. */
        if(nss.isEmpty())
        {
            next = it->next();
            continue;
        }

        const int len = nss.count();
        bool stopInheritance = false;

        for(int i = 0; i < len; ++i)
        {
            const QXmlName &name = nss.at(i);

            if(name.namespaceURI() == StandardNamespaces::StopNamespaceInheritance)
            {
                stopInheritance = true;
                continue;
            }

            if(!alreadySent.contains(name.prefix()))
            {
                alreadySent.append(name.prefix());
                receiver->namespaceBinding(name);
            }
        }

        if(stopInheritance)
            break;
        else
            next = it->next();
    }
}

QString AccelTree::stringValue(const QXmlNodeModelIndex &ni) const
{
    const PreNumber preNumber = toPreNumber(ni);

    switch(kind(preNumber))
    {
        case QXmlNodeModelIndex::Element:
        {
            /* Concatenate all text nodes that are descendants of this node. */
            if(!hasChildren(preNumber))
                return QString();

            const AccelTree::PreNumber stop = preNumber + size(preNumber);
            AccelTree::PreNumber pn = preNumber + 1; /* Jump over ourselves. */
            QString result;

            for(; pn <= stop; ++pn)
            {
                if(kind(pn) == QXmlNodeModelIndex::Text)
                {
                    if(isCompressed(pn))
                        result += CompressedWhitespace::decompress(data.value(pn));
                    else
                        result += data.value(pn);
                }
            }

            return result;
        }
        case QXmlNodeModelIndex::Text:
        {
            if(isCompressed(preNumber))
                return CompressedWhitespace::decompress(data.value(preNumber));
            /* Else, fallthrough. It's not compressed so use it as it is. */
        }
        case QXmlNodeModelIndex::Attribute:
        /* Fallthrough */
        case QXmlNodeModelIndex::ProcessingInstruction:
        /* Fallthrough */
        case QXmlNodeModelIndex::Comment:
            return data.value(preNumber);
        case QXmlNodeModelIndex::Document:
        {
            /* Concatenate all text nodes in the whole document. */

            QString result;
            // Perhaps we can QString::reserve() the result based on the size?
            const AccelTree::PreNumber max = maximumPreNumber();

            for(AccelTree::PreNumber i = 0; i <= max; ++i)
            {
                if(kind(i) == QXmlNodeModelIndex::Text)
                {
                    if(isCompressed(i))
                        result += CompressedWhitespace::decompress(data.value(i));
                    else
                        result += data.value(i);
                }
            }

            return result;
        }
        default:
        {
            Q_ASSERT_X(false, Q_FUNC_INFO,
                       "A node type that doesn't exist in the XPath Data Model was encountered.");
            return QString(); /* Dummy, silence compiler warning. */
        }
    }
}

QVariant AccelTree::typedValue(const QXmlNodeModelIndex &n) const
{
    return stringValue(n);
}

bool AccelTree::hasPrefix(const QVector<QXmlName> &nbs, const QXmlName::PrefixCode prefix)
{
    const int size = nbs.size();

    for(int i = 0; i < size; ++i)
    {
        if(nbs.at(i).prefix() == prefix)
            return true;
    }

    return false;
}

ItemType::Ptr AccelTree::type(const QXmlNodeModelIndex &ni) const
{
    /* kind() is manually inlined here to avoid a virtual call. */
    return XPathHelper::typeFromKind(basicData.at(toPreNumber(ni)).kind());
}

Item::Iterator::Ptr AccelTree::sequencedTypedValue(const QXmlNodeModelIndex &n) const
{
    const PreNumber preNumber = toPreNumber(n);

    switch(kind(preNumber))
    {
        case QXmlNodeModelIndex::Element:
        /* Fallthrough. */
        case QXmlNodeModelIndex::Document:
        /* Fallthrough. */
        case QXmlNodeModelIndex::Attribute:
            return makeSingletonIterator(Item(UntypedAtomic::fromValue(stringValue(n))));

        case QXmlNodeModelIndex::Text:
        /* Fallthrough. */
        case QXmlNodeModelIndex::ProcessingInstruction:
        /* Fallthrough. */
        case QXmlNodeModelIndex::Comment:
            return makeSingletonIterator(Item(AtomicString::fromValue(stringValue(n))));
        default:
        {
            Q_ASSERT_X(false, Q_FUNC_INFO,
                       qPrintable(QString::fromLatin1("A node type that doesn't exist "
                                                      "in the XPath Data Model was encountered.").arg(kind(preNumber))));
            return Item::Iterator::Ptr(); /* Dummy, silence compiler warning. */
        }
    }
}

void AccelTree::copyNodeTo(const QXmlNodeModelIndex &node,
                           QAbstractXmlReceiver *const receiver,
                           const NodeCopySettings &settings) const
{
    /* This code piece can be seen as a customized version of
     * QAbstractXmlReceiver::item/sendAsNode(). */
    Q_ASSERT(receiver);
    Q_ASSERT(!node.isNull());

    typedef QHash<QXmlName::PrefixCode, QXmlName::NamespaceCode> Binding;
    QStack<Binding> outputted;

    switch(node.kind())
    {
        case QXmlNodeModelIndex::Element:
        {
            outputted.push(Binding());

            /* Add the namespace for our element name. */
            const QXmlName elementName(node.name());

            receiver->startElement(elementName);

            if(!settings.testFlag(InheritNamespaces))
                receiver->namespaceBinding(QXmlName(StandardNamespaces::StopNamespaceInheritance, 0,
                                                    StandardPrefixes::StopNamespaceInheritance));

            if(settings.testFlag(PreserveNamespaces))
                node.sendNamespaces(receiver);
            else
            {
                /* Find the namespaces that we actually use and add them to outputted. These are drawn
                 * from the element name, and the node's attributes. */
                outputted.top().insert(elementName.prefix(), elementName.namespaceURI());

                const QXmlNodeModelIndex::Iterator::Ptr attributes(iterate(node, QXmlNodeModelIndex::AxisAttribute));
                QXmlNodeModelIndex attr(attributes->next());

                while(!attr.isNull())
                {
                    const QXmlName &attrName = attr.name();
                    outputted.top().insert(attrName.prefix(), attrName.namespaceURI());
                    attr = attributes->next();
                }

                Binding::const_iterator it(outputted.top().constBegin());
                const Binding::const_iterator end(outputted.top().constEnd());

                for(; it != end; ++it)
                    receiver->namespaceBinding(QXmlName(it.value(), 0, it.key()));
            }

            /* Send the attributes of the element. */
            {
                QXmlNodeModelIndex::Iterator::Ptr attributes(node.iterate(QXmlNodeModelIndex::AxisAttribute));
                QXmlNodeModelIndex attribute(attributes->next());

                while(!attribute.isNull())
                {
                    const QString &v = attribute.stringValue();
                    receiver->attribute(attribute.name(), QStringRef(&v));
                    attribute = attributes->next();
                }
            }

            /* Send the children of the element. */
            copyChildren(node, receiver, settings);

            receiver->endElement();
            outputted.pop();
            break;
        }
        case QXmlNodeModelIndex::Document:
        {
            /* We need to intercept and grab the elements of the document node, such
             * that we preserve/inherit preference applies to them. */
            receiver->startDocument();
            copyChildren(node, receiver, settings);
            receiver->endDocument();
            break;
        }
        default:
            receiver->item(node);
    }

}

void AccelTree::copyChildren(const QXmlNodeModelIndex &node,
                             QAbstractXmlReceiver *const receiver,
                             const NodeCopySettings &settings) const
{
    QXmlNodeModelIndex::Iterator::Ptr children(node.iterate(QXmlNodeModelIndex::AxisChild));
    QXmlNodeModelIndex child(children->next());

    while(!child.isNull())
    {
        copyNodeTo(child, receiver, settings);
        child = children->next();
    }
}

QXmlNodeModelIndex AccelTree::elementById(const QXmlName &id) const
{
    const PreNumber pre = m_IDs.value(id.localName(), -1);
    if(pre == -1)
        return QXmlNodeModelIndex();
    else
        return createIndex(pre);
}

QVector<QXmlNodeModelIndex> AccelTree::nodesByIdref(const QXmlName &) const
{
    return QVector<QXmlNodeModelIndex>();
}

QT_END_NAMESPACE

