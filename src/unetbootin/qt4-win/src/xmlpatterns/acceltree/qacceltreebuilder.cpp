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

/**
 * @file
 * @short This file is included by qacceltreebuilder_p.h.
 * If you need includes in this file, put them in qacceltreebuilder_p.h, outside of the namespace.
 */

template <bool FromDocument>
AccelTreeBuilder<FromDocument>::AccelTreeBuilder(const QUrl &docURI,
                                                 const QUrl &baseURI,
                                                 const NamePool::Ptr &np,
                                                 const ReportContext::Ptr &context) : m_preNumber(-1)
                                                                                    , m_isPreviousAtomic(false)
                                                                                    , m_hasCharacters(false)
                                                                                    , m_isCharactersCompressed(false)
                                                                                    , m_namePool(np)
                                                                                    , m_document(new AccelTree(docURI, baseURI))
                                                                                    , m_skippedDocumentNodes(0)
                                                                                    , m_documentURI(docURI)
                                                                                    , m_context(context)
{
    Q_ASSERT(m_namePool);

    /* TODO Perhaps we can merge m_ancestors and m_size
     * into one, and store a struct for the two instead? */
    m_ancestors.reserve(DefaultNodeStackSize);
    m_ancestors.push(-1);

    m_size.reserve(DefaultNodeStackSize);
    m_size.push(0);
}

template <bool FromDocument>
void AccelTreeBuilder<FromDocument>::startStructure()
{
    if(m_hasCharacters)
    {
        /* We create a node even if m_characters is empty.
         * Remember that `text {""}' creates one text node
         * with string value "". */

        ++m_preNumber;
        m_document->basicData.append(AccelTree::BasicNodeData(currentDepth(),
                                                              currentParent(),
                                                              QXmlNodeModelIndex::Text,
                                                              m_isCharactersCompressed ? AccelTree::IsCompressed : 0));
        m_document->data.insert(m_preNumber, m_characters);
        ++m_size.top();

        m_characters.clear(); /* We don't want it added twice. */
        m_hasCharacters = false;

        if(m_isCharactersCompressed)
            m_isCharactersCompressed = false;
    }
}

template <bool FromDocument>
void AccelTreeBuilder<FromDocument>::item(const Item &it)
{
    Q_ASSERT(it);

    if(it.isAtomicValue())
    {
        if(m_isPreviousAtomic)
        {
            m_characters += QLatin1Char(' ');
            m_characters += it.stringValue();
        }
        else
        {
            m_isPreviousAtomic = true;
            const QString sv(it.stringValue());

            if(!sv.isEmpty())
            {
                m_characters += sv;
                m_hasCharacters = true;
            }
        }
    }
    else
        sendAsNode(it);
}

template <bool FromDocument>
void AccelTreeBuilder<FromDocument>::startElement(const QXmlName &name)
{
    startStructure();

    m_document->basicData.append(AccelTree::BasicNodeData(currentDepth(), currentParent(), QXmlNodeModelIndex::Element, -1, name));

    ++m_preNumber;
    m_ancestors.push(m_preNumber);

    ++m_size.top();
    m_size.push(0);

    /* With node constructors, we can receive names for which we have no namespace
     * constructors, such as in the query '<xs:space/>'. Since the 'xs' prefix has no
     * NamespaceConstructor in this case, we synthesize the namespace.
     *
     * In case we're constructing from an XML document we avoid the call because
     * although it's redundant, it's on extra virtual call for each element. */
    if(!FromDocument)
        namespaceBinding(QXmlName(name.namespaceURI(), 0, name.prefix()));

    m_isPreviousAtomic = false;
}

template <bool FromDocument>
void AccelTreeBuilder<FromDocument>::endElement()
{
    startStructure();
    const AccelTree::PreNumber index = m_ancestors.pop();
    AccelTree::BasicNodeData &data = m_document->basicData[index];

    /* Sub trees needs to be included in upper trees, so we add the count of this element
     * to our parent. */
    m_size[m_size.count() - 2] += m_size.top();

    data.setSize(m_size.pop());
    m_isPreviousAtomic = false;
}

template <bool FromDocument>
void AccelTreeBuilder<FromDocument>::attribute(const QXmlName &name, const QStringRef &value)
{
    /* Attributes adds a namespace binding, so lets synthesize one.
     *
     * We optimize by checking whether we have a namespace for which a binding would
     * be generated. Happens relatively rarely. */
    if(name.hasPrefix())
        namespaceBinding(QXmlName(name.namespaceURI(), 0, name.prefix()));

    m_document->basicData.append(AccelTree::BasicNodeData(currentDepth(), currentParent(), QXmlNodeModelIndex::Attribute, 0, name));
    ++m_preNumber;
    m_document->data.insert(m_preNumber, *m_attributeCompress.insert(value.toString()));
    ++m_size.top();

    m_isPreviousAtomic = false;

    if(name.namespaceURI() == StandardNamespaces::xml && name.localName() == StandardLocalNames::id)
    {
        if(QXmlUtils::isNCName(value))
        {
            const QXmlName::LocalNameCode id = m_namePool->allocateLocalName(value.toString());

            const int oldSize = m_document->m_IDs.count();
            m_document->m_IDs.insert(id, currentParent());

            if(oldSize == m_document->m_IDs.count())
            {
                m_context->error(QtXmlPatterns::tr("An %1-attribute with value %2 has already been declared.")
                                                   .arg(formatKeyword("xml:id"),
                                                        formatType(m_namePool, BuiltinTypes::xsNCName),
                                                        formatData(value.toString())),
                                 FromDocument ? ReportContext::FODC0002 : ReportContext::XQDY0091,
                                 this);
            }
        }
        else
        {
            Q_ASSERT(m_context);

            /* If we're building from an XML Document(e.g, we're fed from QXmlStreamReader, we raise FODC0002,
             * otherwise XQDY0091. */
            m_context->error(QtXmlPatterns::tr("An %1-attribute must have a "
                                               "valid %2 as value, which %3 isn't.").arg(formatKeyword("xml:id"),
                                                                                         formatType(m_namePool, BuiltinTypes::xsNCName),
                                                                                         formatData(value.toString())),
                             FromDocument ? ReportContext::FODC0002 : ReportContext::XQDY0091,
                             this);
        }
    }
}

template <bool FromDocument>
void AccelTreeBuilder<FromDocument>::characters(const QStringRef &ch)
{

    /* If a text node constructor appears by itself, a node needs to
     * be created. Therefore, we set m_hasCharacters
     * if we're the only node.
     * However, if the text node appears as a child of a document or element
     * node it is discarded if it's empty.
     */
    if(m_hasCharacters && m_isCharactersCompressed)
    {
        m_characters = CompressedWhitespace::decompress(m_characters);
        m_isCharactersCompressed = false;
    }

    m_characters += ch;

    m_isPreviousAtomic = false;
    m_hasCharacters = !m_characters.isEmpty() || m_preNumber == -1; /* -1 is our start value. */
}

template <bool FromDocument>
void AccelTreeBuilder<FromDocument>::whitespaceOnly(const QStringRef &ch)
{
    Q_ASSERT(!ch.isEmpty());
    Q_ASSERT(ch.toString().trimmed().isEmpty());

    /* This gets problematic due to how QXmlStreamReader works(which
     * is the only one we get whitespaceOnly() events from). Namely, text intermingled
     * with CDATA gets reported as individual Characters events, and
     * QXmlStreamReader::isWhitespace() can return differently for each of those. However,
     * it will occur very rarely, so this workaround of 1) mistakenly compressing 2) decompressing 3)
     * appending, will happen infrequently.
     */
    if(m_hasCharacters)
    {
        if(m_isCharactersCompressed)
        {
            m_characters = CompressedWhitespace::decompress(m_characters);
            m_isCharactersCompressed = false;
        }

        m_characters.append(ch.toString());
    }
    else
    {
        /* We haven't received a text node previously. */
        m_characters = CompressedWhitespace::compress(ch);
        m_isCharactersCompressed = true;
        m_isPreviousAtomic = false;
        m_hasCharacters = true;
    }
}

template <bool FromDocument>
void AccelTreeBuilder<FromDocument>::processingInstruction(const QXmlName &target,
                                                           const QString &data)
{
    startStructure();
    ++m_preNumber;
    m_document->data.insert(m_preNumber, data);

    m_document->basicData.append(AccelTree::BasicNodeData(currentDepth(),
                                                          currentParent(),
                                                          QXmlNodeModelIndex::ProcessingInstruction,
                                                          0,
                                                          target));
    ++m_size.top();
    m_isPreviousAtomic = false;
}

template <bool FromDocument>
void AccelTreeBuilder<FromDocument>::comment(const QString &content)
{
    startStructure();
    m_document->basicData.append(AccelTree::BasicNodeData(currentDepth(), currentParent(), QXmlNodeModelIndex::Comment, 0));
    ++m_preNumber;
    m_document->data.insert(m_preNumber, content);
    ++m_size.top();
}

template <bool FromDocument>
void AccelTreeBuilder<FromDocument>::namespaceBinding(const QXmlName &nb)
{
    /* Note, because attribute() sometimes generate namespaceBinding() calls, this function
     * can be called after attributes, in contrast to what the class documentation says. This is ok,
     * as long as we're not dealing with public API. */

    /* If we've received attributes, it means the element's size have changed and m_preNumber have advanced,
     * so "reverse back" to the actual element. */
    const AccelTree::PreNumber pn = m_preNumber - m_size.top();

    QVector<QXmlName> &nss = m_document->namespaces[pn];

    /* "xml" hasn't been declared for each node, AccelTree::namespaceBindings() adds it, so avoid it
     * such that we don't get duplicates. */
    if(nb.prefix() == StandardPrefixes::xml)
        return;

    /* If we already have the binding, skip it. */
    const int len = nss.count();
    for(int i = 0; i < len; ++i)
    {
        if(nss.at(i).prefix() == nb.prefix())
            return;
    }

    nss.append(nb);
}

template <bool FromDocument>
void AccelTreeBuilder<FromDocument>::startDocument()
{
    /* If we have already received nodes, we can't add a document node. */
    if(m_preNumber == -1) /* -1 is our start value. */
    {
        m_size.push(0);
        m_document->basicData.append(AccelTree::BasicNodeData(0, -1, QXmlNodeModelIndex::Document, -1));
        ++m_preNumber;
        m_ancestors.push(m_preNumber);
    }
    else
        ++m_skippedDocumentNodes;

    m_isPreviousAtomic = false;
}

template <bool FromDocument>
void AccelTreeBuilder<FromDocument>::endDocument()
{
    if(m_skippedDocumentNodes == 0)
    {
        /* Create text nodes, if we've received any. We do this only if we're the
         * top node because if we're getting this event as being a child of an element,
         * text nodes or atomic values can appear after us, and which must get
         * merged with the previous text.
         *
         * We call startStructure() before we pop the ancestor, such that the text node becomes
         * a child of this document node. */
        startStructure();

        m_document->basicData.first().setSize(m_size.pop());
        m_ancestors.pop();
    }
    else
        --m_skippedDocumentNodes;

    m_isPreviousAtomic = false;
}

template <bool FromDocument>
void AccelTreeBuilder<FromDocument>::atomicValue(const QVariant &value)
{
    Q_UNUSED(value);
    // TODO
}

template <bool FromDocument>
QAbstractXmlNodeModel::Ptr AccelTreeBuilder<FromDocument>::builtDocument()
{
    /* Create a text node, if we have received text in some way. */
    startStructure();
    m_document->printStats(m_namePool);

    return m_document;
}

template <bool FromDocument>
NodeBuilder::Ptr AccelTreeBuilder<FromDocument>::create(const QUrl &baseURI) const
{
    Q_UNUSED(baseURI);
    return NodeBuilder::Ptr(new AccelTreeBuilder(QUrl(), baseURI, m_namePool, m_context));
}

template <bool FromDocument>
void AccelTreeBuilder<FromDocument>::startOfSequence()
{
}

template <bool FromDocument>
void AccelTreeBuilder<FromDocument>::endOfSequence()
{
}

template <bool FromDocument>
const SourceLocationReflection *AccelTreeBuilder<FromDocument>::actualReflection() const
{
    return this;
}

template <bool FromDocument>
QSourceLocation AccelTreeBuilder<FromDocument>::sourceLocation() const
{
    if(m_documentURI.isEmpty())
        return QSourceLocation(QUrl(QLatin1String("AnonymousNodeTree")));
    else
        return QSourceLocation(m_documentURI);
}

