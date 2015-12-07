/****************************************************************************
**
** Copyright (C) 2007-2008 Trolltech ASA. All rights reserved.
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

#include <QVector>

#include "qabstractxmlnodemodel_p.h"
#include "qabstractxmlreceiver.h"
#include "qcommonvalues_p.h"
#include "qemptyiterator_p.h"
#include "qitemmappingiterator_p.h"
#include "qitem_p.h"
#include "qnamespaceresolver_p.h"
#include "qsequencemappingiterator_p.h"
#include "qsingletoniterator_p.h"

#include "qabstractxmlnodemodel.h"

QT_BEGIN_NAMESPACE

using namespace QPatternist;

typedef QExplicitlySharedDataPointer<QAbstractXmlForwardIterator<QXmlNodeModelIndex> > QXmlNodeModelIndexIteratorPointer;

/**
 * @file
 * @short Contains the implementation of QAbstractXmlNodeModel.
 */

bool QAbstractXmlNodeModel::isIgnorableInDeepEqual(const QXmlNodeModelIndex &n)
{
    Q_ASSERT(!n.isNull());
    const QXmlNodeModelIndex::NodeKind nk = n.kind();
    return nk == QXmlNodeModelIndex::ProcessingInstruction ||
           nk == QXmlNodeModelIndex::Comment;
}


/*!
  \class QAbstractXmlNodeModel
  \brief The QAbstractXmlNodeModel class provides an abstract
         interface for representing arbitrary data as XML.
  \threadsafe
  \since 4.4
  \ingroup xml-tools

  The QAbstractXmlNodeModel specifies the interface that a node model
  must implement for that node model be accessible to XQuery queries.
  A node model represents data as a structure that can be queried as
  if the data were XML.

  \section1 Usage

  QAbstractXmlNodeModel bridges the gap between the arbitrary structure
  of the non-XML data to be queried and the well-defined structure of
  XML data understood by QXmlQuery.

  Consider a chemistry application that reads a \e chemistryData file
  containing non-XML data representing a chemical structure of
  molecules and atoms. The application will query this chemistry data
  with an XQuery it reads from a \e queryFile. We write a custom
  subclass of QAbstractXmlNodeModel (\e ChemistryNodeModel) that reads
  the \e chemistryData file and builds a data structure, perhaps
  composed of objects of our own classes \e molecule and \e atom.
  Clearly, this data structure is not XML. Our custom subclass will
  know how to traverse this non-XML structure and present it through
  the \l {http://www.w3.org/TR/xpath-datamodel/}{XDM interface}.

  \snippet doc/src/snippets/code/src.xmlpatterns.api.qabstractxmlnodemodel.cpp 1

  The application first creates an instance of QXmlQuery and calls \l
  {QXmlQuery::setQuery()}{setQuery()} to read the \e queryFile
  containing the XQuery we want to run. Then it creates an instance of
  our custom node model class, \e ChemistryNodeModel, which is a
  subclass of QAbstractXmlNodeModel. Its constructor is called with
  the \l {QXmlNamePool} {name pool} obtained from our QXmlQuery, and
  with the \e chemistryFile containing the structure of \e molecules
  and \e atoms to be queried. The \l {QXmlNamePool} {name pool} is
  required because our custom node model has the member function \l
  {QAbstractXmlNodeModel::name()} {name()}, which returns the \l
  {QXmlName} {name} of any node in the model. The \l {QXmlQuery}
  {query} and the custom node model must use the same name pool for
  constructing these \l {QXmlName} {names}. The constructor would then
  read the \e chemistryFile and build the custom node model structure.

  To connect the \e query to the custom node model, we must bind a
  variable name used in the query to a node in the model. The variable
  can then be used in the query as a starting node. First, an \l
  {QXmlNodeModelIndex} {index} for the desired starting node is
  retrieved by calling QAbstractXmlNodeModel::createIndex(). Then the
  index is bound to a variable name, in this case \e queryRoot, by
  passing the name and the index to QXmlQuery::bindVariable(). The
  query can then use a variable reference \c $queryRoot to refer to
  the starting node. Note that if the \l {QXmlQuery} {query} uses
  multiple $variable references, a call to QXmlQuery::bindVariable()
  is required to bind each different variable name to a node in the
  model.

  The query is executed when the application calls one of the
  QXmlQuery evaluation functions, \l {QXmlQuery::evaluateTo()}
  {evaluateTo()} in this case, which passes \e myNodeModel to
  the query engine as a delegate the engine can use for traversing the
  non-XML chemistry data as if it were XML. The application uses
  QXmlQuery::evaluateTo(), because it then uses a \l
  {QXmlSerializer} {serializer} to out the query result as XML to \e
  stdout. We could have used QXmlQuery::evaluateTo() to get a
  list of result items, or QXmlQuery::evaluateTo() if the
  query evaluated to a sequence of \e {xs:string} values.

  During query execution, the engine iterates over the node model
  using nextFromSimpleAxis() to get the \l {QXmlNodeModelIndex}
  {index} of the next node to be visited. The engine can get the name
  of a node by calling name() with the node's \l {QXmlNodeModelIndex}
  {index}. stringValue(), baseUri(), documentUri() and kind() are also
  called as needed with a node \l {QXmlNodeModelIndex} {index}.

  The example demonstrates the standard pattern for using a subclass
  of QAbstractXmlNodeModel in combination with QXmlQuery to perform
  an XQuery.

  \list 1

    \o Instantiate QXmlQuery and give it the XQuery to be run;

    \o Instantiate a subclass of QAbstractXmlNodeModel or
    QSimpleXmlNodeModel;

    \o Retrieve a QXmlNodeModelIndex for the node in the model where
    the QXmlQuery should start the query;

    \o Use QXmlQuery::bindVariable() to bind the QXmlNodeModelIndex
    to \e {$variable name};

    \o Call one of the QXmlQuery evaluation functions to run the
    query.

  \endlist

  \section1 Subclassing

  Because the \l {http://www.w3.org/TR/xpath-datamodel/}{XDM
  interface} presented by QAbstractXmlNodeModel allows QXmlQuery to
  operate on non-XML data as if it were XML, implementing subclasses
  of QAbstractXmlNodeModel can involve a significant amount of
  work. The QSimpleXmlNodeModel class is provided to simplify the
  implementation for many common use cases.

  \section1 Thread Safety

  Because the node model can be accessed concurrently by threads in
  the QtXmlPatterns module, subclasses of QAbstractXmlNodeModel must
  be written to be \l{Reentrancy and Thread-Safety} {thread-safe}.
  Classes that simplify implementing thread-safety include QReadLocker
  and QWriteLocker.

  See the example \l{File System Example} for a demonstration.
 */

/*!
  \enum QXmlNodeModelIndex::Constants

  \value ForwardAxis All axes that are forward axes have this value OR'd in.
  \value ReverseAxis All axes that are reverse axes have this value OR'd in.
 */

/*!
  \enum QXmlNodeModelIndex::DocumentOrder

  Identifies what specific node comparison operator that should be used.

  \value Precedes Signifies the \c \<\< operator. Whether the first operand precedes
         the second operand in document order.
  \value Follows Signifies the \c \>\> operator. Whether the first operand follows
                 the second operand in document order.

  \value Is Signifies the \c is operator. Whether two nodes have the same node identity.
 */

/*!
  \enum QAbstractXmlNodeModel::SimpleAxis

  Four axes that only contain one node each.

  \value Parent The parent of the context node
  \value FirstChild The first child of the context node
  \value PreviousSibling The previous sibling of the context node
  \value NextSibling The next sibling of the context node
*/

/*!
 \enum QXmlNodeModelIndex::Axis
 \internal

   Identify axes. An axis navigates from a node in a way specific to that axis.

   The axes AxisChild, AxisDescendant, AxisAttribute, AxisSelf, AxisDescendantOrSelf, AxisFollowingSibling,
   and AxisFollowing are forward axes.

   The axes AxisParent, AxisAncestor, AxisPrecedingSibling, AxisPreceding and AxisAncestorOrSelf are reverse axes.

   \sa {http://www.w3.org/TR/xquery/#axes}{XQuery 1.0: An XML Query Language, 3.2.1.1 Axes}

\value AxisChild                The \c child axis.
\value AxisDescendant           The \c descendant axis.
\value AxisAttribute            The \c attribute axis. This enum value isn't named
                                "Attribute", in order to not clash with node kind by that name.
\value AxisSelf                 The \c self axis.
\value AxisDescendantOrSelf     The \c descendant-or-self axis.
\value AxisFollowingSibling     The \c following-sibling axis.
\value AxisNamespace            The \c namespace axis. It does not exist in XQuery,
                                is deprecated in XPath 2.0(optionally supported), and mandatory in XPath 1.0.
\value AxisFollowing            The \c following axis.
\value AxisParent               The \c parent axis.
\value AxisAncestor             The \c ancestor axis.
\value AxisPrecedingSibling     The \c preceding-sibling axis.
\value AxisPreceding            The \c preceding axis.
\value AxisAncestorOrSelf       The \c ancestor-or-self axis.
*/

using namespace QPatternist;

/*!
  Default constructor.
 */
QAbstractXmlNodeModel::QAbstractXmlNodeModel() : d_ptr(0)
{
}

/*!
 \internal

 Takes the d-pointer.

 */
QAbstractXmlNodeModel::QAbstractXmlNodeModel(QAbstractXmlNodeModelPrivate *d) : d_ptr(d)
{
}

/*!
  Destructor.
 */
QAbstractXmlNodeModel::~QAbstractXmlNodeModel()
{
    delete d_ptr;
}

/*!
  \typedef QAbstractXmlNodeModel::List
  \internal

  A QList of smart  pointers to QAbstractXmlNodeModel instances.
 */

/*!
  \typedef QAbstractXmlNodeModel::Ptr

  A smart pointer to a QAbstractXmlNodeModel instance.
 */

/*!
   \fn QUrl QAbstractXmlNodeModel::baseUri(const QXmlNodeModelIndex &n) const = 0

   Returns the base URI for \a n.

   The caller guarantees that \a n is not \c null and that it belongs to this QAbstractXmlNodeModel instance.

   The base URI of nodes can be extracted using the \c fn:base-uri() function, and is typically used
   for resolving relative URIs appearing directly, or as child of the node. It is conformant to just return the
   document URI, although that might not properly reflect the underlying data.

   This function maps to the \c dm:base-uri accessor, which the specification completely specifies. Here's a summary:

   \list
        \o For document nodes, the base URI equals the document URI.
        \o For elements, the base URI is the URI appearing in its \c xml:base attribute, if any, recursively resolved to
           the parent's base URI.
        \o Namespaces nodes have no base URI.
        \o The base URI for processing instructions, comments, attributes, and text nodes is the base URI of its parent element.
   \endlist

   The implementation guarantees to return a valid QUrl instance, or a default constructed QUrl instance. If a node has no
   base URI, such as in the case of that a comment has no parent, a default constructed QUrl is returned.

   \sa {http://www.w3.org/TR/xpath-datamodel/#dm-base-uri}{XQuery 1.0 and XPath 2.0 Data Model (XDM), 5.2 base-uri Accessor}
 */

/*!
   \fn QUrl QAbstractXmlNodeModel::documentUri(const QXmlNodeModelIndex &n) const = 0

   Returns the document URI of \a n.

   The document URI identifies the resource which is the document. For instance, if the document would be a regular file,
   it would perhaps be the \c file:/ or \c http:// URL of the location. The document URI is used for resolving URIs and to
   simply know where the document is.

   If the data model maps to a URI in a natural way -- use that. Otherwise return the company or product's URI. It can be any URI
   as long as its valid and absolute.

   The caller guarantees that \a n is not \c null and that it belongs to this QAbstractXmlNodeModel instance.

   This function maps to the \c dm:document-uri accessor, which the specification completely specifies. Here's a summary:

   - If \a n is a document node, return an absolute, valid QUrl
     containing the document URI, or a default constructed QUrl. The
     latter signals that no document URI is available for the document
     node.
   - For all other kinds of nodes, return a default constructed QUrl.

   \sa {http://www.w3.org/TR/xpath-datamodel/#dm-document-uri}{XQuery 1.0 and XPath 2.0 Data Model (XDM), 5.4 document-uri Accessor}
   \sa QUrl::isValid(), QUrl::isRelative()
 */

/*!
   \fn QXmlNodeModelIndex::NodeKind QAbstractXmlNodeModel::kind(const QXmlNodeModelIndex &ni) const = 0

   Determines what node kind \a ni is. Simply, whether \a ni is an element or comment for instance.

   The caller guarantees that \a ni is not \c null and that it belongs
   to this QAbstractXmlNodeModel instance.

   This function maps to the \c dm:node-kind() accessor.

   \sa {http://www.w3.org/TR/xpath-datamodel/#dm-node-kind}{XQuery 1.0 and XPath 2.0 Data Model (XDM), 5.10 node-kind Accessor}
 */

/*!
    \fn QXmlNodeModelIndex::DocumentOrder QAbstractXmlNodeModel::compareOrder(const QXmlNodeModelIndex &ni1,
                                                                              const QXmlNodeModelIndex &ni2) const = 0;
   Returns the relative document order between \a ni1 and \a ni2.

   This is used for the \c is operator, and to sort nodes in document order.

   The caller guarantees that \a ni1 and \a ni2 are not \c null and that they belong to this QAbstractXmlNodeModel instance.

   If \a ni1 is identical to \a ni2, QXmlNodeModelIndex::Is is returned.
   If \a ni1 precedes \a ni2 in document order, QXmlNodeModelIndex::Precedes is
   returned. If \a ni1 follows \a ni2 in document order, QXmlNodeModelIndex::Follows
   is returned.
 */

/*!
    \fn QXmlNodeModelIndex QAbstractXmlNodeModel::root(const QXmlNodeModelIndex &n) const = 0;

   Returns the root node of the tree that \a n is part of. This is typically a document
   node. This function is used among other things for \c fn:root() and the root expression, such as seen in the expression \c /html.

   The caller guarantees that \a n is not \c null and that it belong to this QAbstractXmlNodeModel instance.

   If \a n is a direct child of the QXmlNodeModelIndex returned from this function, parent() would
   return the same QXmlNodeModelIndex.
 */

namespace QPatternist
{
    class MergeIterator
    {
    public:
        inline MergeIterator()
        {
        }

        inline
        QXmlNodeModelIndexIteratorPointer
        mapToSequence(const QXmlNodeModelIndexIteratorPointer &it,
                      const DynamicContext::Ptr &) const
        {
            return it;
        }

    private:
        Q_DISABLE_COPY(MergeIterator)
    };

    static const MergeIterator mergeIterator;
};

/*!
 \internal
 This function is not a private member of QAbstractXmlNodeModel because it would
 be messy to forward declare the required types.
*/
static inline QXmlNodeModelIndexIteratorPointer mergeIterators(const QXmlNodeModelIndex &node,
                                                               const QXmlNodeModelIndexIteratorPointer &it2)
{
    QVector<QXmlNodeModelIndexIteratorPointer> iterators;
    iterators.append(makeSingletonIterator(node));
    iterators.append(it2);

    return makeSequenceMappingIterator<QXmlNodeModelIndex>(&mergeIterator,
                                                           makeVectorIterator(iterators),
                                                           DynamicContext::Ptr());
}

inline QAbstractXmlForwardIterator<QXmlNodeModelIndex>::Ptr
QAbstractXmlNodeModel::mapToSequence(const QXmlNodeModelIndex &ni,
                                     const DynamicContext::Ptr &) const
{
    Q_ASSERT(!ni.isNull());
    /* Since we pass in this here, mapToSequence is used recursively. */
    return mergeIterators(ni, makeSequenceMappingIterator<QXmlNodeModelIndex>(this,
                                                                              ni.iterate(QXmlNodeModelIndex::AxisChild),
                                                                              DynamicContext::Ptr()));
}

/*!
  \fn QVector<QXmlNodeModelIndex> QAbstractXmlNodeModel::attributes(const QXmlNodeModelIndex &element) const = 0
  Returns the attributes that element \a element has.

  The caller guarantees that \a element is always an element.
 */

/*!
   \internal

   Performs navigation, starting from \a ni by returning an QAbstractXmlForwardIterator that
   returns nodes that the Axis \a axis reaches from \a ni.

   The implementation returns the nodes on the axis without duplicates and in axis order. This means that if \a axis is a reverse axis,
   which is the case of \c parent, \c ancestor, \c ancestor-or-self, \c preceding, and \c preceding-sibling, the nodes are delivered in
   reverse document order, otherwise it is in document order.

   The implementor guarantees that the nodes delivered for the axes are consistent with the XPath Data Model. This means practically
   common sense, that the AxisChild axis for, say, a comment node does not have any children, or that a document node is not the child
   of an element. Attributes aren't considered children of an element, but are only available on AxisAttribute.

   There is no guarantee for what axes that will be passed to this function based on
   what is used in a query. QtXmlPatterns may call this function at arbitrary amounts with
   arbitrary axes, regardless of how queries reads. This is because QtXmlPatterns may rewrite queries
   into versions that are more efficient, by using axes in different ways.

   QAbstractXmlNodeModel::Axis has a good overview of the axes and what they select.

   The caller guarantees that \a ni is not \c null and that it belong to this QAbstractXmlNodeModel instance.

   Implementing iterate() can be a bit hefty since it requires different iterators for all axes to be delivered. This could
   in worst case result in having to write as many QAbstractXmlForwardIterator subclasses as there are axes, but can typically
   be reduced with clever use of lists and template classes. Nevertheless, it can be interesting to implement QSimpleXmlNodeModel
   which makes providing the node navigation easier, but not necesarilly slower or less flexible.

   \sa QSimpleXmlNodeModel
   \sa QXmlNodeModelIndex::Axis
   \sa {http://www.w3.org/TR/xquery/#axes}{XQuery 1.0: An XML Query Language, 3.2.1.1 Axes}
   \sa {http://www.w3.org/TR/xpath-datamodel/}{W3CXQuery 1.0 and XPath 2.0 Data Model (XDM)}
 */
QExplicitlySharedDataPointer<QAbstractXmlForwardIterator<QXmlNodeModelIndex> >
QAbstractXmlNodeModel::iterate(const QXmlNodeModelIndex &ni,
                               QXmlNodeModelIndex::Axis axis) const
{
    /* Returns iterators that track state and calls nextFromSimpleAxis()
     * iteratively. Typically, when sub-classing QSimpleXmlNodeModel,
     * you don't reimplement this function, but instead implement
     * nextFromSimpleAxis(). */

    switch(axis)
    {
        case QXmlNodeModelIndex::AxisSelf:
            return makeSingletonIterator(ni);
        case QXmlNodeModelIndex::AxisParent:
        {
            if(kind(ni) == QXmlNodeModelIndex::Document)
                return makeEmptyIterator<QXmlNodeModelIndex>();
            else
                return makeSingletonIterator(nextFromSimpleAxis(Parent, ni));
        }
        case QXmlNodeModelIndex::AxisNamespace:
            return makeEmptyIterator<QXmlNodeModelIndex>();
        case QXmlNodeModelIndex::AxisAncestor:
        {
            QList<QXmlNodeModelIndex> ancestors;
            QXmlNodeModelIndex ancestor = nextFromSimpleAxis(Parent, ni);

            while(!ancestor.isNull())
            {
                ancestors.append(ancestor);
                ancestor = nextFromSimpleAxis(Parent, ancestor);
            }

            return makeListIterator(ancestors);
        }
        case QXmlNodeModelIndex::AxisAncestorOrSelf:
        {
            QList<QXmlNodeModelIndex> ancestors;
            ancestors.append(ni);
            QXmlNodeModelIndex ancestor = nextFromSimpleAxis(Parent, ni);

            while(!ancestor.isNull())
            {
                ancestors.append(ancestor);
                ancestor = nextFromSimpleAxis(Parent, ancestor);
            }

            return makeListIterator(ancestors);
        }
        case QXmlNodeModelIndex::AxisPrecedingSibling:
        {
            QList<QXmlNodeModelIndex> preceding;
            QXmlNodeModelIndex sibling = nextFromSimpleAxis(PreviousSibling, ni);

            while(!sibling.isNull())
            {
                preceding.append(sibling);
                sibling = nextFromSimpleAxis(PreviousSibling, sibling);
            }

            return makeListIterator(preceding);
        }
        case QXmlNodeModelIndex::AxisFollowingSibling:
        {
            QList<QXmlNodeModelIndex> preceding;
            QXmlNodeModelIndex sibling = nextFromSimpleAxis(NextSibling, ni);

            while(!sibling.isNull())
            {
                preceding.append(sibling);
                sibling = nextFromSimpleAxis(NextSibling, sibling);
            }

            return makeListIterator(preceding);
        }
        case QXmlNodeModelIndex::AxisChild:
        {
            QList<QXmlNodeModelIndex> children;
            QXmlNodeModelIndex child = nextFromSimpleAxis(FirstChild, ni);

            while(!child.isNull())
            {
                children.append(child);
                child = nextFromSimpleAxis(NextSibling, child);
            }

            return makeListIterator(children);
        }
        case QXmlNodeModelIndex::AxisDescendant:
        {
            return makeSequenceMappingIterator<QXmlNodeModelIndex>(this,
                                                                   ni.iterate(QXmlNodeModelIndex::AxisChild),
                                                                   DynamicContext::Ptr());
        }
        case QXmlNodeModelIndex::AxisAttribute:
            return makeVectorIterator(attributes(ni));
        case QXmlNodeModelIndex::AxisDescendantOrSelf:
            return mergeIterators(ni, iterate(ni, QXmlNodeModelIndex::AxisDescendant));
        case QXmlNodeModelIndex::AxisFollowing:
        /* Fallthrough. */
        case QXmlNodeModelIndex::AxisPreceding:
        {
            /* We walk up along the ancestors, and for each parent, we grab its preceding/following
             * siblings, and evaluate the descendant axis. The descendant axes gets added
             * to a list and we then merge those iterators. */
            QVector<QXmlNodeModelIndexIteratorPointer> descendantIterators;

            QXmlNodeModelIndex current(ni);
            while(!current.isNull())
            {
                QXmlNodeModelIndex candidate(nextFromSimpleAxis(axis == QXmlNodeModelIndex::AxisPreceding ? PreviousSibling : NextSibling, current));
                if(candidate.isNull())
                {
                    /* current is an ancestor. We don't want it, so next iteration we
                     * will grab its preceding sibling. */
                    current = nextFromSimpleAxis(Parent, current);
                }
                else
                {
                    current = candidate;
                    descendantIterators.append(iterate(current, QXmlNodeModelIndex::AxisDescendantOrSelf)->toReversed());
                }
            }

            return makeSequenceMappingIterator<QXmlNodeModelIndex>(&mergeIterator,
                                                                   makeVectorIterator(descendantIterators),
                                                                   DynamicContext::Ptr());
        }
    }

    Q_ASSERT_X(false, Q_FUNC_INFO, "Unknown axis, internal error.");
    return makeEmptyIterator<QXmlNodeModelIndex>();
}

/*!
  \fn QXmlNodeModelIndex QAbstractXmlNodeModel::nextFromSimpleAxis(SimpleAxis axis, const QXmlNodeModelIndex &origin) const = 0

  When QtXmlPatterns calls iterate(), QSimpleXmlNodeModel create iterators that calls nextFromSimpleAxis() and "emulates" real
  XPath axes using QSimpleXmlNodeModel::SimpleAxis. Therefore, the implementation of this function should return
  the node, if any, that appear on axis \a axis, from \a origin.

  If no such node is available, a default constructed QXmlNodeModelIndex is returned.

  QSimpleXmlNodeModel removes the need to handle redundant corner cases by guaranteeing that it will never ask for:

  \list
    \o Children or siblings for attributes.
    \o Children for comments, processing instructions, and text nodes.
    \o Siblings or parents for document nodes.
  \endlist

  A typical implementation does a switch over \a axis:

  \code
  QXmlNodeModelIndex MyTreeModel::nextFromSimpleAxis(SimpleAxis axis, const QXmlNodeModelIndex &origin) const
  {
    // Convert the QXmlNodeModelIndex to a value that is specific to what we represent.
    const MyValue value = toMyValue(ni);

    switch(axis)
    {
        case Parent:
            return toNodeIndex(value.parent());
        case FirstChild:
        case PreviousSibling:
        case NextSibling:
            // and so on
    }
  }
  \endcode

 */

/*!
  \fn QXmlNodeModelIndex QAbstractXmlNodeModel::createIndex(qint64 data) const

  Creates a node index with \a data as its internal data.

  What \a data is, is not constrained.
 */

/*!
  \fn QXmlNodeModelIndex QAbstractXmlNodeModel::createIndex(void *pointer, qint64 additionalData = 0) const

  Creates a node index with \a pointer and \a additionalData as its internal data.

  What \a pointer and \a additionalData is, is not constrained.
 */

/*!
  \fn QXmlNodeModelIndex QAbstractXmlNodeModel::createIndex(qint64 data,
                                                            qint64 additionalData) const;
  \overload

  Creates a QXmlNodeModelIndex that houses \a data and \a additionalData.

 */

/*!
   \fn  QXmlName QAbstractXmlNodeModel::name(const QXmlNodeModelIndex &ni) const = 0;

   Returns the name of \a ni.

   The caller guarantees that \a ni is not \c null and that it belong to this QAbstractXmlNodeModel instance.

   If a node does not have a name, such as a comment code, a null QXmlName is returned. QXmlName instances must be created
   with the same QXmlQuery instance that is used for evaluating queries using this QAbstractXmlNodeModel instance.

   This function maps to the \c dm:node-name() accessor.

   As specified, if \a ni is a processing instruction, a QXmlName is returned
   where the local name is the target name and the namespace URI and prefix is empty.

   \sa {http://www.w3.org/TR/xpath-datamodel/#dm-node-name}{XQuery 1.0 and XPath 2.0 Data Model (XDM), 5.11 node-name Accessor}
   \sa QXmlName
 */

/*!
   \fn QVector<QXmlName> QAbstractXmlNodeModel::namespaceBindings(const QXmlNodeModelIndex &n) const = 0;

   Returns the in-scope namespaces of \a n.

   The caller guarantees that \a n is not \c null and that it belong to this QAbstractXmlNodeModel instance.

   This corresponds to the \c dm:namespace-nodes accessor.

   \note This is not only the namespace declarations that appear on
   this element, but takes also into account namespace bindings of the
   ancestors.

   The caller guarantees that \a n is an Element and belongs to this QAbstractXmlNodeModel instance.
 */

/*!
   \internal
   Sends the namespaces declared on \a n to \a receiver.

   As a consequence, no namespaces are sent except if this node is an
   element and has namespaces declared.

   The caller guarantees that \a n is not \c null and that it belong to this QAbstractXmlNodeModel instance.

   Note that it's not the namespaces that are in scope on \a n, but only the namespaces that are specifically
   declared on \a n.

   \a receiver is the receiver that this node is supposed to send its
   namespaces to. This is guaranteed by the caller to be a valid
   pointer.
   \a n is the node whose namespaces to send.
 */
void QAbstractXmlNodeModel::sendNamespaces(const QXmlNodeModelIndex &n,
                                           QAbstractXmlReceiver *const receiver) const
{
    Q_ASSERT(receiver);
    const QVector<QXmlName> nss(namespaceBindings(n));

    /* This is by far the most common case. */
    if(nss.isEmpty())
        return;

    const int len = nss.size();
    for(int i = 0; i < len; ++i)
        receiver->namespaceBinding(nss.at(i));
}

/*!
   \fn QString QAbstractXmlNodeModel::stringValue(const QXmlNodeModelIndex &n) const = 0;

   Returns the string value for node \a n.

   The caller guarantees that \a n is not \c null and that it belong to this QAbstractXmlNodeModel instance.

   This function maps to the \c dm:string-value() accessor, which the specification completely specifies. Here's a summary:

   \list
        \o For processing instructions, the string value is the data section(excluding any whitespace appearing between the name and the data).
        \o For text nodes, the string value equals the text node.
        \o For comments, the content of the comment
        \o For elements, the concatenation of all text nodes that are descendants. Note, this is not only the children,
           but the childrens' childrens' text nodes, and so forth.
        \o For document nodes, the concatenation of all text nodes in the document.
   \endlist

   \sa {http://www.w3.org/TR/xpath-datamodel/#dm-string-value}{XQuery 1.0 and XPath 2.0 Data Model (XDM), 5.13 string-value Accessor}
 */

/*!
  \fn QVariant QAbstractXmlNodeModel::typedValue(const QXmlNodeModelIndex &node) const = 0;

  Returns the typed value for node \a node.

  The typed value is an atomic value, which an element or attribute contains.

  The caller guarantees that \a node is either an element or an attribute. The implementor guarantees
  that the returned QVariant has a value which is supported in XQuery. It cannot be an arbitrary QVariant value.
  The implementor also guarantees that stringValue() returns a lexical representation of typedValue()(this is guaranteed by QSimpleXmlNodeModel::stringValue()).


  If the return QVariant is a default constructed variant, it signals that \a node has no typed value.
*/

/*!
   \internal
 */
QPatternist::ItemIteratorPtr QAbstractXmlNodeModel::sequencedTypedValue(const QXmlNodeModelIndex &ni) const
{
    const QVariant &candidate = typedValue(ni);
    if(candidate.isNull())
        return QPatternist::CommonValues::emptyIterator;
    else
        return makeSingletonIterator(AtomicValue::toXDM(candidate));
}

/*!
 \internal
 */
QPatternist::ItemTypePtr QAbstractXmlNodeModel::type(const QXmlNodeModelIndex &) const
{
    Q_ASSERT_X(false, Q_FUNC_INFO,
               "This function is internal and must not be called.");
    return QPatternist::ItemTypePtr();
}

/*!
   \internal

   Returns the namespace URI on \a ni that corresponds to \a prefix.

   If \a prefix is StandardPrefixes::empty, the namespace URI for the default
   namespace is returned.

   The default implementation use namespaceBindings(), in a straight
   forward manner.

   If no namespace exists for \a prefix, NamespaceResolver::NoBinding is returned.

   The caller guarantees to only call this function for element nodes.
 */
QXmlName::NamespaceCode QAbstractXmlNodeModel::namespaceForPrefix(const QXmlNodeModelIndex &ni,
                                                                  const QXmlName::PrefixCode prefix) const
{
    Q_ASSERT(kind(ni) == QXmlNodeModelIndex::Element);

    const QVector<QXmlName> nbs(namespaceBindings(ni));
    const int len = nbs.size();

    for(int i = 0; i < len; ++i)
    {
        if(nbs.at(i).prefix() == prefix)
            return nbs.at(i).namespaceURI();
    }

    return NamespaceResolver::NoBinding;
}


/*!
   \internal

   Determines whether \a ni1 is deep equal to \a ni2.

   isDeepEqual() is defined as evaluating the expression \c fn:deep-equal($n1, $n2) where
   \c $n1 is \a ni1 and \c $n1 is \a ni2. This function is associative, meaning
   the same value is returned regardless of if isDeepEqual() is invoked with \a ni1 as first
   argument or second. It is guaranteed that \a ni1 and \a ni2 are nodes, as opposed to the
   definition of \c fn:deep-equal().

   Returns @c true if \a ni1 is deep-equal to \a ni2, otherwise @c false

   \sa {"http://www.w3.org/TR/xpath-functions/#func-deep-equal"}{XQuery 1.0 and XPath 2.0 Functions and Operators, 15.3.1 fn:deep-equal}
 */
bool QAbstractXmlNodeModel::isDeepEqual(const QXmlNodeModelIndex &n1,
                                        const QXmlNodeModelIndex &n2) const
{
    Q_ASSERT(!n1.isNull());
    Q_ASSERT(!n2.isNull());

    const QXmlNodeModelIndex::NodeKind nk = n1.kind();

    if(nk != n2.kind())
        return false;

    if(n1.name() != n2.name())
        return false;

    switch(nk)
    {
        case QXmlNodeModelIndex::Element:
        {
            QXmlNodeModelIndexIteratorPointer atts1(n1.iterate(QXmlNodeModelIndex::AxisAttribute));
            QXmlNodeModelIndex node(atts1->next());

            const QXmlNodeModelIndex::List atts2(n2.iterate(QXmlNodeModelIndex::AxisAttribute)->toList());
            const QXmlNodeModelIndex::List::const_iterator end(atts2.constEnd());

            while(!node.isNull())
            {
                bool equal = false;
                for(QXmlNodeModelIndex::List::const_iterator it = atts2.constBegin(); it != end; ++it)
                {
                    if(isDeepEqual(node, (*it)))
                        equal = true;
                }

                if(!equal)
                    return false;

                node = atts1->next();
            }

            /* Fallthrough, so we check the children. */
        }
        case QXmlNodeModelIndex::Document:
        {
            QXmlNodeModelIndexIteratorPointer itn1(n1.iterate(QXmlNodeModelIndex::AxisChild));
            QXmlNodeModelIndexIteratorPointer itn2(n2.iterate(QXmlNodeModelIndex::AxisChild));

            while(true)
            {
                QXmlNodeModelIndex no1(itn1->next());
                QXmlNodeModelIndex no2(itn2->next());

                while(!no1.isNull() && isIgnorableInDeepEqual(no1))
                    no1 = itn1->next();

                while(!no2.isNull() && isIgnorableInDeepEqual(no2))
                    no2 = itn2->next();

                if(!no1.isNull() && !no2.isNull())
                {
                   if(!isDeepEqual(no1, no2))
                       return false;
                }
                else
                    return no1.isNull() && no2.isNull();
            }

            return true;
        }
        case QXmlNodeModelIndex::Attribute:
        /* Fallthrough */
        case QXmlNodeModelIndex::ProcessingInstruction:
        /* Fallthrough. */
        case QXmlNodeModelIndex::Text:
        /* Fallthrough. */
        case QXmlNodeModelIndex::Comment:
            return n1.stringValue() == n2.stringValue();
        case QXmlNodeModelIndex::Namespace:
        {
            Q_ASSERT_X(false, Q_FUNC_INFO, "Not implemented");
            return false;
        }
    }

    return false;
}

/*!
  \class QXmlItem
  \reentrant
  \since 4.4
  \brief The QXmlItem class represents an item in XQuery and XPath, which is either a node or an atomic value.
  \ingroup xml-tools

  In XQuery, all expressions evaluate to a sequence of items, where each item is an
  atomic value or a XML node. For instance, this query:

  \quotefile doc/src/snippets/patternist/items.xq

  evaluate to a sequence of five items: an element, an atomic value which
  is binary data encoded in base64, a date, a float, and an attribute node.

  QXmlItem is the class that represents XQuery items in the Qt API. It is always either a node or
  an item, which isNode() and isAtomicValue() tells. Atomic values are represented in
  Qt using QVariant, and a QXmlitem can be converted to one by calling toAtomicValue(). Similarly,
  nodes are represented with QXmlNodeModelIndex, and those can be
  retrieved by calling toNodeModelIndex().

  A default constructed QXmlItem instance is neither a node or an atomic value,
  it is \c null, as isNull() tells.

  QXmlItem instances will dangle if the model that QXmlNodeModelIndex
  instances refers to is deleted.
 */

/*!
 \typedef QXmlItem::Iterator
 A QAbstractXmlForwardIterator over QXmlItem.
 */

/*!
   Construct a null QXmlItem, which is neither a node nor an atomic value.

   For a default constructed QXmlItem, isNull() returns \c true.
 */
QXmlItem::QXmlItem()
{
    m_node.model = 0;
    m_node.data = 0;
    m_node.additionalData = 0;
}

bool QXmlItem::internalIsAtomicValue() const
{
    return m_node.model == reinterpret_cast<QAbstractXmlNodeModel *>(~0);
}

/*!
  Constructs a QXmlItem instance that is a copy of \a other.
 */
QXmlItem::QXmlItem(const QXmlItem &other) : m_node(other.m_node)
{
    if(internalIsAtomicValue())
        m_atomicValue->ref.ref();
}

/*!
  Constructs a QXmlItem that is the atomic value \a atomicValue.
 */
QXmlItem::QXmlItem(const QVariant &atomicValue)
{
    if(atomicValue.isNull())
    {
        /* Then we behave just like the default constructor. */
        m_node.model = 0;
        m_node.data = 0;
        m_node.additionalData = 0;
        return;
    }

    /* We can't assign directly to m_atomicValue, because then the temporary will destruct before
     * we've ref'd it. */
    const QPatternist::Item temp(QPatternist::AtomicValue::toXDM(atomicValue));

    if(temp)
    {
        temp.asAtomicValue()->ref.ref();
        m_node.model = reinterpret_cast<const QAbstractXmlNodeModel *>(~0);
        m_atomicValue = temp.asAtomicValue();
    }
    else
    {
        m_atomicValue = 0;
        m_node.model = 0;
    }

    m_node.additionalData = 0;
}

/*!
  Constructs a QXmlItem instance that is a copy of \a node. This item will subsequently be a node.

  \sa QXmlItem::isNode()
 */
QXmlItem::QXmlItem(const QXmlNodeModelIndex &node) : m_node(node.m_storage)
{
}


/*!
  Destructs this QXmlItem instance.
 */
QXmlItem::~QXmlItem()
{
    if(internalIsAtomicValue() && !m_atomicValue->ref.deref())
        delete m_atomicValue;
}

bool QPatternist::NodeIndexStorage::operator!=(const NodeIndexStorage &other) const
{
    return data != other.data
           || additionalData != other.additionalData
           || model != other.model;
}

/*!
  Assigns \a other to this QXmlItem instance.
 */
QXmlItem &QXmlItem::operator=(const QXmlItem &other)
{
    if(m_node != other.m_node)
    {
        if(internalIsAtomicValue() && !m_atomicValue->ref.deref())
            delete m_atomicValue;

        m_node = other.m_node;

        if(internalIsAtomicValue())
            m_atomicValue->ref.ref();
    }

    return *this;
}

/*!
  Returns \c true if this item is a Node, otherwise \c false.

  If this item is null, false is returned.

  \sa isNull(), isAtomicValue()
 */
bool QXmlItem::isNode() const
{
    return QPatternist::Item::fromPublic(*this).isNode();
}

/*!
  Returns \c true if this item is an atomic value, otherwise \c false.

  If this item is null, false is returned.

  \sa isNull(), isNode()
 */
bool QXmlItem::isAtomicValue() const
{
    return internalIsAtomicValue();
}

/*!
 Considers this item an atomic value, and returns it in the form of a QVariant instance.

 If this QXmlItem is not an atomic value, the return value is
 a default constructed QVariant.

 \sa isAtomicValue()
 */
QVariant QXmlItem::toAtomicValue() const
{
    if(isAtomicValue())
        return QPatternist::AtomicValue::toQt(m_atomicValue);
    else
        return QVariant();
}

/*!
 Considers this item an node, and returns it in the form of a QXmlNodeModelIndex instance.

 If this QXmlItem is not a node, the return value is undefined.

 \sa isNode()
 */
QXmlNodeModelIndex QXmlItem::toNodeModelIndex() const
{
    if(isNode())
        return reinterpret_cast<const QXmlNodeModelIndex &>(m_node);
    else
        return QXmlNodeModelIndex();
}

/*!
  Returns \c true if this QXmlItem instance do not represent an item.
 */
bool QXmlItem::isNull() const
{
    return !m_node.model;
}

/*!
  \class QXmlNodeModelIndex
  \brief The QXmlNodeModelIndex class is used to represent a node in a node model.
  \reentrant
  \since 4.4
  \ingroup xml-tools

  QXmlNodeModelIndex is used as an index into QAbstractXmlNodeModel. It contains two things:

  \list
    \o A pointer to a QAbstractXmlNodeModel, retrievable via model()
    \o Data, retrievable via data(), internalPointer() and additionalData().
  \endlist

  QXmlNodeModelIndex is intentionally a simple class. For instance it doesn't
  have a function for getting a node's name or the list of its attributes. Therefore,
  QXmlNodeModelIndex is primarily useful if you are using your own node model, but falls
  short if you to some degree want to spectate the nodes as ordinary XML nodes. Typically
  the best way to achieve the latter is to either send the result out to a
  QAbstractXmlReceiver or to put more of the logic inside the query.

  For instance, let's say you want to populate a list widget with the values of
  a set of attributes. Instead of letting the query return a set of elements and then
  iterate over the their attributes and extract the string values, let the query do
  all this directly and then evaluate it using QXmlQuery::evaluateTo() and pass
  a pointer to QStringList.

  QXmlNodeModelIndex does not impose restrictions on what data QXmlNodeModelIndex should
  contain, that is entirely up to the associated QAbstractXmlNodeModel.

  Conceptually, QXmlNodeModelIndex can be seen as being one of the two types of items in
  XQuery: namely nodes. When the XQuery engine handles nodes, it uses QXmlNodeModelIndex
  in order to delegate the actual functionality to the associated QAbstractXmlNodeModel.

  Since QXmlNodeModelIndex is dependent on a particular QAbstractXmlNodeModel subclass, there is
  no way to create instances, except for via QAbstractXmlNodeModel::createIndex(). Since that function
  is protected, it can probably be good to create a function that creates node indexes from
  arguments that fits the particular node model.

  A default constructed node index \c null, and isNull() will return \c true.

  QXmlNodeModelIndex and QAbstractXmlNodeModel follows the same design pattern that QModelIndex
  QAbstractItemModel do.
 */

/*!
  Computes a hash key from the QXmlNodeModelIndex \a index, and returns it.

  The hash is computed on QXmlNodeModelIndex::data(), QXmlNodeModelIndex::additionalData(),
  and QXmlNodeModelIndex::model(). This means the hash key can be used for node indexes
  from different node models.

  \relates QXmlNodeModelIndex
 */
uint qHash(const QXmlNodeModelIndex &index)
{
    return uint(index.data() + index.additionalData() + quintptr(index.model()));
}

/*!
  \enum QXmlNodeModelIndex::NodeKind

  Identifies the kind of a node.

  \sa QAbstractXmlNodeModel::kind()

  \value Attribute Identifies an attribute node
  \value Text Identifies a text node
  \value Comment Identifies a comment node
  \value Document Identifies a document node
  \value Element Identifies an element node
  \value Namespace Identifies a namespace node
  \value ProcessingInstruction Identifies a processing instruction. Not
         that the optional XML declaration at very beginning of the XML document
         is not a processing instruction
*/

/*!
 \typedef QXmlNodeModelIndex::List

 Typedef for QList<QXmlNodeModelIndex>.
 */

/*!
  Returns true if this node is the same as \a other.

  In other words, this operator does not compare values, children or names of nodes,
  it tells whether two nodes have the same node identity. That is, whether they are
  from the same document and can be found at the exact same place.

 */
bool QXmlNodeModelIndex::operator==(const QXmlNodeModelIndex &other) const
{
    return !(m_storage != other.m_storage);
}

/*!
  Returns true if \a other is the same node as this.
 */
bool QXmlNodeModelIndex::operator!=(const QXmlNodeModelIndex &other) const
{
    return !(operator==(other));
}

/*!
 \fn QXmlNodeModelIndex::QXmlNodeModelIndex()

 Default constructor. Creates an item that is \c null.

 \sa isNull()
 */

/*!
 \fn QXmlNodeModelIndex::QXmlNodeModelIndex(const QXmlNodeModelIndex &other)
 Standard copy constructor. Creates a QXmlNodeModelIndex instance that is a copy of \a other.
 */

/*!
 \fn bool QXmlNodeModelIndex::isNull() const

 Returns \c true if this QXmlNodeModelIndex is a default
 constructed value, otherwise \c false.

 A \c null QXmlNodeModelIndex instance doesn't represent any node and cannot
 be used in conjunction with QAbstractXmlNodeModel.
 */

/*!
 \fn const QAbstractXmlNodeModel *QXmlNodeModelIndex::model() const

 Returns the QAbstractXmlNodeModel that this node index belongs to. QXmlNodeModelIndex
 does not own QAbstractXmlNodeModel nor keep track of its lifetime, so this pointer will
 dangle whenthe QAbstractXmlNodeModel is deallocated.

 There is no setter for this, QXmlNodeModelIndex instances are created
 with QAbstractXmlNodeModel::createIndex().
*/

/*!
 \fn qint64 QXmlNodeModelIndex::data() const

 Returns the first data section that this node index carries.

 \sa additionalData()
*/

/*!
 \fn void *QXmlNodeModelIndex::internalPointer() const

 Returns the first data section typed as a void * pointer.

 \sa additionalData()
*/

/*!
 \fn qint64 QXmlNodeModelIndex::additionalData() const

 Returns the second data section that this node index carries.

 \sa data()
*/

/*!
 \fn void QXmlNodeModelIndex::reset()
 \internal

 Turns this QXmlNodeModelIndex into a \c null QXmlNodeModelIndex instance. It is equivalent
 to writing:

 \snippet doc/src/snippets/code/src.xmlpatterns.api.qabstractxmlnodemodel.cpp 0
 */

/*!
 \fn QXmlName QXmlNodeModelIndex::name() const
 \internal
*/

/*!
 \typedef QXmlNodeModelIndex::Iterator
 \internal

 Typedef for QAbstractXmlForwardIterator<QXmlNodeModelIndex>.
 */
/*!
 \fn QXmlNodeModelIndex QXmlNodeModelIndex::root() const
 \internal
*/

/*!
 \fn QExplicitlySharedDataPointer<QAbstractXmlForwardIterator<QXmlNodeModelIndex> > QXmlNodeModelIndex::iterate(const Axis axis) const
 \internal
*/

/*!
 \fn QExplicitlySharedDataPointer<QAbstractXmlForwardIterator<QPatternist::Item> > QXmlNodeModelIndex::sequencedTypedValue() const
 \internal
*/

/*!
 \fn QUrl QXmlNodeModelIndex::documentUri() const
 \internal
*/

/*!
 \fn QUrl QXmlNodeModelIndex::baseUri() const
 \internal
*/

/*!
 \fn NodeKind QXmlNodeModelIndex::kind() const
 \internal
*/

/*!
 \fn bool QXmlNodeModelIndex::isDeepEqual(const QXmlNodeModelIndex &other) const
 \internal
*/

/*!
 \fn DocumentOrder QXmlNodeModelIndex::compareOrder(const QXmlNodeModelIndex &other) const
 \internal
*/

/*!
 \fn void QXmlNodeModelIndex::sendNamespaces(QAbstractXmlReceiver *const receiver) const
 \internal
*/

/*!
 \fn QVector<QXmlName> QXmlNodeModelIndex::namespaceBindings() const
 \internal
*/

/*!
 \fn QXmlNodeModelIndex QAbstractXmlNodeModel::elementById(const QXmlName &id) const = 0;

 Returns the element that has id \a id. XQuery's \c id() function ends up calling this.

 Returns the element node whose typed value is of type \c ID and equals \a id, or the element
 that has an attribute whose typed value is of type \c ID and equals \a id. If there is no such element,
 a default constructed QXmlNodeModelIndex instance is returned. The implementor guarantees the returned
 node, if not \c null, is an element.

 In effect it's not sufficient for an attribute or element to merely be called \c id in order to be of type
 \c ID. However, the reserved name \c xml:id is always recognized as so.

 In \a id the namespace URI and the prefix is undefined, while the local name is
 the ID that should be looked up.

 \sa {http://www.w3.org/TR/xpath-functions/#func-id}{XQuery 1.0 and XPath 2.0 Functions and Operators, 15.5.2 fn:id}
 */

/*!
 \fn QVector<QXmlNodeModelIndex> QAbstractXmlNodeModel::nodesByIdref(const QXmlName &idref) const = 0;

 Returns the elements and/or attributes that an \c IDREF value equal to \a idref. XQuery's
 \c idref() function ends up calling this.

 The implementor guarantees the returned nodes are elements or attributes.

 In effect it's not sufficient for an attribute or element to merely be called \c idref in order to be of type
 \c IDREF. Elements must be typed as \c xs:IDREF or \c xs:IDREFS, or in the case of attributes only,
 as \c IDREF or \c IDREFS in the schema.

 In \a idref the namespace URI and the prefix is undefined, while the local name is
 the ID that should be looked up.

 \sa {http://www.w3.org/TR/xpath-functions/#func-idref}{XQuery 1.0 and XPath 2.0 Functions and Operators, 15.5.3 fn:idref}
 */

/*!
 \fn QXmlName::NamespaceCode QXmlNodeModelIndex::namespaceForPrefix(const QXmlName::PrefixCode prefix) const
 \internal
*/

/*!
 \fn QString QXmlNodeModelIndex::stringValue() const
 \internal
*/

/*!
 \fn QPatternist::ItemTypePtr QXmlNodeModelIndex::type() const
 \internal
*/

/*!
 \fn bool QXmlNodeModelIndex::is(const QXmlNodeModelIndex &other) const
 \internal
*/

/*!
 \enum QAbstractXmlNodeModel::NodeCopySetting
 \internal

 Controls how nodes are copied with copyNodeTo.

 \value InheritNamespaces Copies the node with the \c copy-namespaces
        setting being \c inherit. If not set, \c no-inherit is assumed.
 \value PreserveNamespaces Copies the node with the \c copy-namespaces
        settings being \c preserve. If not set, \c no-preserve is assumed.
 */

/*!
 \typedef QAbstractXmlNodeModel::NodeCopySettings
 \internal
 */

/*!
 \internal

 Copies node \a node to \a receiver, steered by \a copySettings.

 The caller guarantees that \a node is not \c null, and that is belongs to
 this QAbstractXmlNodeModel instance.

 The caller guarantees that \a receiver is not \c null.
*/
void QAbstractXmlNodeModel::copyNodeTo(const QXmlNodeModelIndex &node,
                                       QAbstractXmlReceiver *const receiver,
                                       const NodeCopySettings &copySettings) const
{
    Q_UNUSED(node);
    Q_UNUSED(receiver);
    Q_UNUSED(copySettings);
    Q_ASSERT_X(false, Q_FUNC_INFO,
               "This function is not expected to be called.");
}

QT_END_NAMESPACE
