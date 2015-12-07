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
  \brief The QAbstractXmlNodeModel class is an abstract base class for modeling non-XML data to look like XML for QXmlQuery.
  \threadsafe
  \since 4.4
  \ingroup xml-tools

  The QAbstractXmlNodeModel specifies the interface that a node model
  must implement for that node model be accessible to the query engine
  for processing XQuery queries.  A node model represents data as a
  structure that can be queried as if the data were XML.

  The node model represented by a subclass of QAbstractXmlNodeModel is
  meant to be accessed by the QtXmlPatterns query engine. If the API
  seems a little strange in a few places, it is because the member
  functions are called by the query engine as it evaluates an
  XQuery. They aren't meant to be used programatically.

  \section1 Usage

  QAbstractXmlNodeModel bridges the gap between the arbitrary structure
  of the non-XML data to be queried and the well-defined structure of
  XML data understood by QXmlQuery.

  Consider a chemistry application that reads the file \c
  chemistryData, which contains non-XML data that represents a
  chemical structure composed of molecules and atoms. The application
  will query this chemistry data with an XQuery it reads from file \c
  queryFile. We write a custom subclass of QAbstractXmlNodeModel (\c
  ChemistryNodeModel) that reads \c chemistryData and builds a data
  structure, perhaps composed of objects of our own classes \c
  molecule and \c atom.  Clearly, this data structure is not XML. Our
  custom subclass will know how to traverse this non-XML structure and
  present it through the \l
  {http://www.w3.org/TR/xpath-datamodel/}{XPath Data Model interface}.

  \snippet doc/src/snippets/code/src_xmlpatterns_api_qabstractxmlnodemodel.cpp 1

  The application first creates an instance of QXmlQuery and calls \l
  {QXmlQuery::setQuery()}{setQuery()} to read \c queryFile containing
  the XQuery we want to run. Then it creates an instance of our custom
  node model class, \c ChemistryNodeModel, which is a subclass of
  QAbstractXmlNodeModel. Its constructor is called with the \l
  {QXmlNamePool} {name pool} obtained from our QXmlQuery, and with the
  \c chemistryFile containing the structure of molecules and atoms to
  be queried. The \l {QXmlNamePool} {name pool} is required because
  our custom node model has the member function \l
  {QAbstractXmlNodeModel::name()} {name()}, which returns the \l
  {QXmlName} {name} of any node in the model. The \l {QXmlQuery}
  {query} and the custom node model must use the same name pool for
  constructing these \l {QXmlName} {names}. The constructor would then
  read \c chemistryFile and build the custom node model structure.

  To connect the \c query to the custom node model, we must bind a
  variable name used in the query to a node in the model. The variable
  can then be used in the query as a starting node. First, an \l
  {QXmlNodeModelIndex} {index} for the desired starting node is
  retrieved by calling QAbstractXmlNodeModel::createIndex(). Then the
  index is bound to a variable name, in this case \c queryRoot, by
  passing the name and the index to QXmlQuery::bindVariable(). The
  query can then use a variable reference \c $queryRoot to refer to
  the starting node. Note that if the \l {QXmlQuery} {query} uses
  multiple variable references, a call to QXmlQuery::bindVariable()
  is required to bind each different variable name to a node in the
  model.

  The query is executed when the application calls one of the
  QXmlQuery evaluation functions. The application uses
  QXmlQuery::evaluateTo(QAbstractXmlReceiver *), because it then uses
  a \l {QXmlSerializer} {serializer} to out the query result as XML to
  \c stdout. We could have used QXmlQuery::evaluateTo(QXmlResultItems
  *) to get a list of result items, or
  QXmlQuery::evaluateTo(QStringList *) if the query evaluated to a
  sequence of \c {xs:string} values.

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
    to \c {$variable name};

    \o Call one of the QXmlQuery evaluation functions to run the
    query.

  \endlist

  \section1 Subclassing

  Because the \l {http://www.w3.org/TR/xpath-datamodel/}{XPath Data Model
  interface} presented by QAbstractXmlNodeModel allows QXmlQuery to
  operate on non-XML data as if it were XML, implementing subclasses
  of QAbstractXmlNodeModel can involve a significant amount of
  work. The QSimpleXmlNodeModel class is provided to simplify the
  implementation for many common use cases.

  \section1 Thread Safety

  Because the node model can be accessed concurrently by threads in
  the QtXmlPatterns module, subclasses of QAbstractXmlNodeModel must
  be written to be \l{Thread Support in Qt#Reentrancy and Thread-Safety}
  {thread-safe}.
  Classes that simplify implementing thread-safety include QReadLocker
  and QWriteLocker.

  See the example \l{File System Example} for a demonstration.
 */

/*!
  \enum QXmlNodeModelIndex::Constants

  \value ForwardAxis All forward axes include this flag.
  \value ReverseAxis All reverse axes include this flag.
 */

/*!
  \enum QXmlNodeModelIndex::DocumentOrder

  Identifies the specific node comparison operator that should be
  used.

  \value Precedes Signifies the \c \<\< operator. Test whether the
         first operand precedes the second in the document.

  \value Follows Signifies the \c \>\> operator. Test whether the
                 first operand follows the second in the document.

  \value Is Signifies the \c is operator. Test whether two nodes have
  the same node identity.
 */

/*!
  \enum QAbstractXmlNodeModel::SimpleAxis

  Four axes that each contain one node only.

  \value Parent The parent of the context node
  \value FirstChild The first child of the context node
  \value PreviousSibling The previous child of the context node
  \value NextSibling The next child of the context node
*/

/*!
 \enum QXmlNodeModelIndex::Axis
 \internal

   Identify the axes emanating from a node.

   The axes AxisChild, AxisDescendant, AxisAttribute, AxisSelf,
   AxisDescendantOrSelf, AxisFollowingSibling, and AxisFollowing are
   forward axes.

   The axes AxisParent, AxisAncestor, AxisPrecedingSibling,
   AxisPreceding and AxisAncestorOrSelf are reverse axes.

   \sa {http://www.w3.org/TR/xquery/#axes}{XQuery 1.0: An XML Query Language, 3.2.1.1 Axes}

   \value AxisChild                The \c child axis.

   \value AxisDescendant           The \c descendant axis.

   \value AxisAttribute            The \c attribute axis. Note: There
                                   is a node kind named \c{Attribute}.

   \value AxisSelf                 The \c self axis.

   \value AxisDescendantOrSelf     The \c descendant-or-self axis.

   \value AxisFollowingSibling     The \c following-sibling axis.

   \value AxisNamespace            The \c namespace axis. Note: Does
                                   not exist in XQuery; deprecated in
                                   XPath 2.0 (optionally supported);
                                   mandatory in XPath 1.0.

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

  A \l{QList}{list} of \l{QExplicitlySharedDataPointer} {smart
  pointers} to instances of QAbstractXmlNodeModel.

  \sa QExplicitlySharedDataPointer
 */

/*!
  \typedef QAbstractXmlNodeModel::Ptr

  A \l {QExplicitlySharedDataPointer} {smart pointer} to an
  instance of QAbstractXmlNodeModel.

  \sa QExplicitlySharedDataPointer
 */

/*!
  \fn QUrl QAbstractXmlNodeModel::baseUri(const QXmlNodeModelIndex &n) const

  Returns the base URI for the node whose index is \a n. The caller
  guarantees that \a n is not \c null and that it belongs to a node
  in this node model.

  The base URI of a node can be extracted using the \c fn:base-uri()
  function. The base URI is typically used for resolving relative URIs
  that appear in the node or its children. It is conformant to just
  return the document URI, although that might not properly reflect
  the underlying data.

  This function maps to the \c dm:base-uri accessor, which returns
  a base URI according to the following:

  \list

  \o For document nodes, the base URI and the document URI are the same.

  \o For elements, the base URI is the URI appearing in the element's
     \c xml:base attribute, if present, or it is resolved to the
     parent element's base URI.

  \o Namespace nodes have no base URI.

  \o The base URI for a processing instruction, comment, attribute,
  or text node is the base URI of the node's parent element.

  \endlist

  The implementation guarantees to return a valid QUrl, or a default
  constructed QUrl. If a node has no base URI, as in the case where a
  comment has no parent, a default constructed QUrl is returned.

   \sa {http://www.w3.org/TR/xpath-datamodel/#dm-base-uri}{XQuery 1.0 and XPath 2.0 Data Model (XDM), 5.2 base-uri Accessor}
 */

/*!
  \fn QUrl QAbstractXmlNodeModel::documentUri(const QXmlNodeModelIndex &n) const

  Returns the document URI of \a n. The document URI identifies the
  resource which is the document. For example, the document could be a
  regular file, e.g., \c{file:/}, or it could be the \c{http://} URL of
  the location of a file. The document URI is used for resolving URIs
  and to simply know where the document is.

  If the node model maps to a URI in a natural way, return that URI.
  Otherwise, return the company or product URI. The document URI can
  be any URI as long as its valid and absolute.

  The caller guarantees that \a n is not \c null and that it belongs
  to this QAbstractXmlNodeModel.

  This function maps to the \c dm:document-uri accessor, which
  returns a document URI according to the following:

  \list

  \o If \a n is a document node, return an absolute QUrl containing
     the document URI, or a default constructed QUrl. The latter
     signals that no document URI is available for the document node.

  \o For all other nodes, return a default constructed QUrl.

  \endlist

  \sa {http://www.w3.org/TR/xpath-datamodel/#dm-document-uri}{XQuery 1.0 and XPath 2.0 Data Model (XDM), 5.4 document-uri Accessor}
  \sa QUrl::isValid(), QUrl::isRelative()
 */

/*
### Qt 5:

Add the function:

    virtual QSourceLocation sourceLocation(const QXmlNodeModelIndex &nodeIndex) const = 0;

Such that the data model can communicate back source locations.
 */

/*!
  \fn QXmlNodeModelIndex::NodeKind QAbstractXmlNodeModel::kind(const QXmlNodeModelIndex &ni) const

  Returns a value indicating the kind of node identified by \a ni.
  The caller guarantees that \a ni is not null and that it identifies
  a node in this node model. This function maps to the \c
  dm:node-kind() accessor.

  \sa {http://www.w3.org/TR/xpath-datamodel/#dm-node-kind}{XQuery 1.0 and XPath 2.0 Data Model (XDM), 5.10 node-kind Accessor}
 */

/*!
  \fn QXmlNodeModelIndex::DocumentOrder QAbstractXmlNodeModel::compareOrder(const QXmlNodeModelIndex &ni1, const QXmlNodeModelIndex &ni2) const

  This function returns the relative document order for the
  nodes indexed by \a ni1 and \a ni2. It is used for the \c Is
  operator and for sorting nodes in document order.

  The caller guarantees that \a ni1 and \a ni2 are not \c null and
  that both identify nodes in this node model.

  If \a ni1 is identical to \a ni2, QXmlNodeModelIndex::Is is returned.
  If \a ni1 precedes \a ni2 in document order, QXmlNodeModelIndex::Precedes
  is returned. If \a ni1 follows \a ni2 in document order,
  QXmlNodeModelIndex::Follows is returned.

  \sa {http://www.w3.org/TR/xpath-datamodel/#document-order}{XQuery 1.0 and XPath 2.0 Data Model (XDM), 2.4 Document Order}
 */

/*!
  \fn QXmlNodeModelIndex QAbstractXmlNodeModel::root(const QXmlNodeModelIndex &n) const

  Returns the root node of the tree that contains the node whose index
  is \a n. The caller guarantees that \a n is not \c null and that it
  identifies a node in this node model.

  If \a n identifies a node that is a direct child of the root,
  parent() would return the same QXmlNodeModelIndex returned by
  this function.
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

    /**
     * One might wonder, why not use makeVectorIterator() directly on a QVector
     * with iterators?
     *
     * A problem emerges QAbstractXmlForwardIterator::copy(). All "meta
     * iterators" that contain other iterators and so forth, propagate the
     * copy() call such that all involved iterators are copied. However, if we
     * have a ListIterator of iterators it isn't aware of that it contains
     * iterators. Hence, we have this class which is specialized(not in the
     * template sense) on iterators, and hence copies them appropriately.
     */
    class IteratorVector : public ListIterator<QXmlNodeModelIndexIteratorPointer, QVector<QXmlNodeModelIndexIteratorPointer> >
    {
        typedef QVector<QXmlNodeModelIndexIteratorPointer> ItVector;
    public:
        typedef QAbstractXmlForwardIterator<QXmlNodeModelIndexIteratorPointer>::Ptr Ptr;

        IteratorVector(const ItVector &in) : ListIterator<QXmlNodeModelIndexIteratorPointer, QVector<QXmlNodeModelIndexIteratorPointer> >(in)
        {
        }

        virtual QAbstractXmlForwardIterator<QXmlNodeModelIndexIteratorPointer>::Ptr copy() const
        {
            ItVector result;

            for(int i = 0; i < m_list.count(); ++i)
                result.append(m_list.at(i)->copy());

            return Ptr(new IteratorVector(result));
        }
    };
}

/*!
 \internal
 This function is not a private member of QAbstractXmlNodeModel
 because it would be messy to forward declare the required types.
*/
static inline QXmlNodeModelIndexIteratorPointer mergeIterators(const QXmlNodeModelIndex &node,
                                                               const QXmlNodeModelIndexIteratorPointer &it2)
{
    QVector<QXmlNodeModelIndexIteratorPointer> iterators;
    iterators.append(makeSingletonIterator(node));
    iterators.append(it2);

    return makeSequenceMappingIterator<QXmlNodeModelIndex>(&mergeIterator,
                                                           IteratorVector::Ptr(new IteratorVector(iterators)),
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
  \fn QVector<QXmlNodeModelIndex> QAbstractXmlNodeModel::attributes(const QXmlNodeModelIndex &element) const

  Returns the attributes of \a element. The caller guarantees
  that \a element is an element in this node model.
 */

/*!
  \internal

  Performs navigation, starting from \a ni, by returning an
  QAbstractXmlForwardIterator that returns nodes the \a axis emanating
  from \a ni.

  The implementation returns the nodes on the \a axis, without
  duplicates and in \a axis order. This means that if \a axis is a
  reverse axis, which is the case for the \c parent, \c ancestor, \c
  ancestor-or-self, \c preceding, and \c preceding-sibling, the nodes
  are delivered in reverse document order. Otherwise the nodes are
  delivered in document order.

  The implementor guarantees that the nodes delivered for the axes are
  consistent with the XPath Data Model. This just implies common
  sense, e.g., The child axis for a comment node can't contain any
  children; a document node can't be a child of an element, etc.
  Attributes aren't considered children of an element, but are only
  available on AxisAttribute.

  The value past in \a axis is not guaranteed based on what is used in
  a query. QtXmlPatterns may call this function arbitrarily with any
  value for \a axis. This is because QtXmlPatterns may rewrite queries
  to be more efficient, using axes in different ways from the original
  query.

  QAbstractXmlNodeModel::Axis has a good overview of the axes and what
  they select.

  The caller guarantees that \a ni is not \c null and that it belongs
  to this QAbstractXmlNodeModel instance.

  Implementing iterate() can involve significant work, since it
  requires different iterators for all the axes used. In the worst
  case, it could require writing as many QAbstractXmlForwardIterator
  subclasses as there are axes, but the number can often be reduced
  with clever use of lists and template classes. It is better to use
  or subclass QSimpleXmlNodeModel, which makes it easier to write the
  node navigation code without loss of efficiency or flexibility.

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
        case QXmlNodeModelIndex::AxisChildOrTop:
        {
            if(nextFromSimpleAxis(Parent, ni).isNull())
            {
                switch(kind(ni))
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
        case QXmlNodeModelIndex::AxisAttributeOrTop:
        {
            if(kind(ni) == QXmlNodeModelIndex::Attribute && nextFromSimpleAxis(Parent, ni).isNull())
                return makeSingletonIterator(ni);

            /* Else, fallthrough to AxisAttribute. */
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
                                                                   IteratorVector::Ptr(new IteratorVector(descendantIterators)),
                                                                   DynamicContext::Ptr());
        }
    }

    Q_ASSERT_X(false, Q_FUNC_INFO, "Unknown axis, internal error.");
    return makeEmptyIterator<QXmlNodeModelIndex>();
}

/*!
  \fn QXmlNodeModelIndex QAbstractXmlNodeModel::nextFromSimpleAxis(SimpleAxis axis, const QXmlNodeModelIndex &origin) const

  When QtXmlPatterns evaluate path expressions, it emulate them through a
  combination of calls with QSimpleXmlNodeModel::SimpleAxis values. Therefore,
  the implementation of this function must return the node, if any, that
  appears on the \a axis emanating from the \a origin.

  If no such node is available, a default constructed
  QXmlNodeModelIndex is returned.

  QSimpleXmlNodeModel eliminates the need to handle redundant corner
  cases by guaranteeing that it will never ask for:

  \list
    \o Children or siblings for attributes.
    \o Children for comments, processing instructions, and text nodes.
    \o Siblings or parents for document nodes.
  \endlist

  A typical implementation performs a \c switch on the value of \a
  axis:

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

  Creates a node index with \a data as its internal data. \a data is
  not constrained.
 */

/*!
  \fn QXmlNodeModelIndex QAbstractXmlNodeModel::createIndex(void *pointer, qint64 additionalData) const

  Creates a node index with \a pointer and \a additionalData as
  its internal data.

  What \a pointer and \a additionalData is, is not constrained.
 */

/*!
  \fn QXmlNodeModelIndex QAbstractXmlNodeModel::createIndex(qint64 data, qint64 additionalData) const;
  \overload

  Creates a QXmlNodeModelIndex containing \a data and \a
  additionalData.
 */

/*!
  \fn  QXmlName QAbstractXmlNodeModel::name(const QXmlNodeModelIndex &ni) const

  Returns the name of \a ni. The caller guarantees that \a ni is not
  \c null and that it belongs to this QAbstractXmlNodeModel.

  If a node does not have a name, e.g., comment nodes, a null QXmlName
  is returned. QXmlNames must be created with the instance of
  QXmlQuery that is being used for evaluating queries using this
  QAbstractXmlNodeModel.

  This function maps to the \c dm:node-name() accessor.

  If \a ni is a processing instruction, a QXmlName is returned with
  the local name as the target name and the namespace URI and prefix
  both empty.

  \sa {http://www.w3.org/TR/xpath-datamodel/#dm-node-name}{XQuery 1.0 and XPath 2.0 Data Model (XDM), 5.11 node-name Accessor}
  \sa QXmlName
 */

/*!
  \fn QVector<QXmlName> QAbstractXmlNodeModel::namespaceBindings(const QXmlNodeModelIndex &n) const

  Returns the in-scope namespaces of \a n. The caller guarantees that
  \a n is not \c null and that it belongs to this QAbstractXmlNodeModel.

  This function corresponds to the \c dm:namespace-nodes accessor.

  The returned vector of namespace declarations includes namespaces
  of the ancestors of \a n.

  The caller guarantees that \a n is an Element that belongs to this
  QAbstractXmlNodeModel.
 */

/*!
  \internal
  Sends the namespaces declared on \a n to \a receiver.

  As a consequence, no namespaces are sent unless this node is an
  element and has namespaces declared.

  The caller guarantees that \a n is not \c null and that it belongs
  to this QAbstractXmlNodeModel instance.

  Note that it is not the namespaces that are in scope on \a n, but
  only the namespaces that are specifically declared on \a n.

  \a receiver is the receiver that this node is supposed to send its
  namespaces to. This is guaranteed by the caller to be a valid
  pointer.  \a n is the index of the node whose namespaces are to
  be sent.
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
  \fn QString QAbstractXmlNodeModel::stringValue(const QXmlNodeModelIndex &n) const

  Returns the string value for node \a n.

  The caller guarantees that \a n is not \c null and that it belong to
  this QAbstractXmlNodeModel instance.

  This function maps to the \c dm:string-value() accessor, which the
  specification completely specifies. Here's a summary:

  \list

  \o For processing instructions, the string value is the data
  section(excluding any whitespace appearing between the name and the
  data).

  \o For text nodes, the string value equals the text node.

  \o For comments, the content of the comment

  \o For elements, the concatenation of all text nodes that are
  descendants. Note, this is not only the children, but the
  childrens' childrens' text nodes, and so forth.

  \o For document nodes, the concatenation of all text nodes in the
  document.

  \endlist

  \sa {http://www.w3.org/TR/xpath-datamodel/#dm-string-value}{XQuery 1.0 and XPath 2.0 Data Model (XDM), 5.13 string-value Accessor}
 */

/*!
  \fn QVariant QAbstractXmlNodeModel::typedValue(const QXmlNodeModelIndex &node) const

  Returns the typed value for node \a node.

  The typed value is an atomic value, which an element or attribute
  contains.

  The caller guarantees that \a node is either an element or an
  attribute. The implementor guarantees that the returned QVariant has
  a value which is supported in XQuery. It cannot be an arbitrary
  QVariant value.  The implementor also guarantees that stringValue()
  returns a lexical representation of typedValue()(this is guaranteed
  by QSimpleXmlNodeModel::stringValue()).

  If the return QVariant is a default constructed variant, it signals
  that \a node has no typed value.
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

  If \a prefix is StandardPrefixes::empty, the namespace URI for the
  default namespace is returned.

  The default implementation use namespaceBindings(), in a straight
  forward manner.

  If no namespace exists for \a prefix, NamespaceResolver::NoBinding
  is returned.

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

  isDeepEqual() is defined as evaluating the expression \c
  fn:deep-equal($n1, $n2) where \c $n1 is \a ni1 and \c $n1 is \a
  ni2. This function is associative, meaning the same value is
  returned regardless of if isDeepEqual() is invoked with \a ni1 as
  first argument or second. It is guaranteed that \a ni1 and \a ni2
  are nodes, as opposed to the definition of \c fn:deep-equal().

  Returns true if \a ni1 is deep-equal to \a ni2, otherwise false

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
  \brief The QXmlItem class contains either an XML node or an atomic value.
  \ingroup xml-tools

  In XQuery, all expressions evaluate to a sequence of items, where
  each item is either an XML node or an atomic value. The query in the
  following snippet evaluates to sequence of five items.

  \quotefile doc/src/snippets/patternist/items.xq

  The five items are: An element, an atomic value (binary data encoded
  in base64), a date, a float, and an attribute.

  QXmlItem is the class that represents these XQuery items in the
  QtXmlPatterns API. A non-null instance of QXmlItem is either a node
  or an atomic value. Calling isNode() or isAtomicValue() tells you
  which it is. Atomic values are represented elsewhere in the Qt API
  as instances of QVariant, and an instance of QXmlItem that
  represents an atomic value can be converted to a QVariant by calling
  toAtomicValue(). A QXmlItem that wraps a node is represented
  elsewhere as an instance of QXmlNodeModelIndex. A node QXmlItem can
  be converted to a QXmlNodeModelIndex by calling toNodeModelIndex().

  A default constructed QXmlItem instance is neither a node nor an
  atomic value. It is considered null, in which case isNull() returns
  true.

  An instance of QXmlItem will be left dangling if the
  \l{QAbstractXmlNodeModel} {XML node model} it
  refers to is deleted, if it is a QXmlNodeModelIndex.
 */

/*!
  \typedef QXmlItem::Iterator
  A QAbstractXmlForwardIterator over QXmlItem.
 */

/*!
  Constructs a null QXmlItem that is neither a node nor an atomic
  value. isNull() returns true for a default constructed instance.
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
  The copy constructor constructs a copy of \a other.
 */
QXmlItem::QXmlItem(const QXmlItem &other) : m_node(other.m_node)
{
    if(internalIsAtomicValue())
        m_atomicValue->ref.ref();
}

/*!
  Constructs an atomic value QXmlItem with \a atomicValue.

  \sa isAtomicValue()
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

    /*
      We can't assign directly to m_atomicValue, because the
      temporary will self-destruct before we've ref'd it.
    */
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
  Constructs a node QXmlItem that is a copy of \a node.

  \sa isNode()
 */
QXmlItem::QXmlItem(const QXmlNodeModelIndex &node) : m_node(node.m_storage)
{
}


/*!
  Destructor.
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
  Assigns \a other to \c this.
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
  Returns true if this item is a Node. Returns false if it
  is an atomic value or null.

  \sa isNull(), isAtomicValue()
 */
bool QXmlItem::isNode() const
{
    return QPatternist::Item::fromPublic(*this).isNode();
}

/*!
  Returns true if this item is an atomic value. Returns false
  if it is a node or null.

  \sa isNull(), isNode()
 */
bool QXmlItem::isAtomicValue() const
{
    return internalIsAtomicValue();
}

/*!
  If this QXmlItem represents an atomic value, it is converted
  to an appropriate QVariant and returned. If this QXmlItem is
  not an atomic value, the return value is a default constructed
  QVariant. You can call isAtomicValue() to test whether the
  item is an atomic value.

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
  If this QXmlItem represents a node, it returns the item as a
  QXmlNodeModelIndex. If this QXmlItem is not a node, the return
  value is undefined. You can call isNode() to test whether the
  item is a node.

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
  Returns true if this QXmlItem is neither a node nor an
  atomic value. Default constructed instances of QXmlItem
  are null.
 */
bool QXmlItem::isNull() const
{
    return !m_node.model;
}

/*!
  \class QXmlNodeModelIndex
  \brief The QXmlNodeModelIndex class identifies a node in an XML node model subclassed from QAbstractXmlNodeModel.
  \reentrant
  \since 4.4
  \ingroup xml-tools

  QXmlNodeModelIndex is an index into an \l{QAbstractXmlNodeModel}
  {XML node model}. It contains:

  \list
    \o A pointer to an \l{QAbstractXmlNodeModel} {XML node model},
    which is returned by model(), and
    \o Some data, which is returned by data(), internalPointer(),
    and additionalData().
  \endlist

  Because QXmlNodeModelIndex is intentionally a simple class, it
  doesn't have member functions for accessing the properties of
  nodes. For example, it doesn't have functions for getting a
  node's name or its list of attributes or child nodes. If you find
  that you need to retrieve this kind of information from your
  query results, there are two ways to proceed.

  \list

  \o Send the output of your XQuery to an \l{QAbstractXmlReceiver}
  {XML receiver}, or

  \o Let your XQuery do all the work to produce the desired result.

  \endlist

  The second case is explained by example. Suppose you want to
  populate a list widget with the values of certain attributes from a
  set of result elements. You could write an XQuery to return the set
  of elements, and then you would write the code to iterate over the
  result elements, get their attributes, and extract the desired
  string values. But the simpler way is to just augment your XQuery to
  finding the desired attribute values. Then all you have to do is
  evaluate the XQuery using the version of QXmlQuery::evaluateTo()
  that populates a QStringList, which you can send directly to your
  widget.

  QXmlNodeModelIndex doesn't impose any restrictions on the \c data
  value an QXmlNodeModelIndex should contain. The meaning of the data
  left to the associated \l {QAbstractXmlNodeModel} {node model}.
  Because QXmlNodeModelIndex depends on a particular subclass of
  QAbstractXmlNodeModel for its existence, the only way you can create
  an instance of QXmlNodeModelIndex is by asking the node model to
  create one for you with QAbstractXmlNodeModel::createIndex(). Since
  that function is protected, it is usually a good ide to write a
  public function that creates a QXmlNodeModelIndex from arguments that
  are appropriate for your particular node model.

  A default constructed node index is said to be null, i.e., isNull()
  returns true.

  QXmlNodeModelIndex and QAbstractXmlNodeModel follow the same design
  pattern used for QModelIndex and QAbstractItemModel.
 */

/*!
  \since 4.4
  \relates QHash

  Computes a hash key from the QXmlNodeModelIndex \a index, and
  returns it. This function would be used by QHash if you wanted
  to build a hash table for instances of QXmlNodeModelIndex.

  The hash is computed on QXmlNodeModelIndex::data(),
  QXmlNodeModelIndex::additionalData(), and
  QXmlNodeModelIndex::model(). This means the hash key can be used for
  node indexes from different node models.
 */
uint qHash(const QXmlNodeModelIndex &index)
{
    return uint(index.data() + index.additionalData() + quintptr(index.model()));
}

/*!
  \enum QXmlNodeModelIndex::NodeKind

  Identifies a kind of node.

  \value Attribute Identifies an attribute node
  \value Text Identifies a text node
  \value Comment Identifies a comment node
  \value Document Identifies a document node
  \value Element Identifies an element node
  \value Namespace Identifies a namespace node
  \value ProcessingInstruction Identifies a processing instruction.

  Note that the optional XML declaration at very beginning of the XML
  document is not a processing instruction

  \sa QAbstractXmlNodeModel::kind()
*/

/*!
 \typedef QXmlNodeModelIndex::List

 Typedef for QList<QXmlNodeModelIndex>.
 */

/*!
  Returns true if this node is the same as \a other. This operator
  does not compare values, children, or names of nodes. It compares
  node identities, i.e., whether two nodes are from the same document
  and are found at the exact same place.
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

 Standard copy constructor. Creates a QXmlNodeModelIndex instance that
 is a copy of \a other.
 */

/*!
 \fn bool QXmlNodeModelIndex::isNull() const

 Returns true if this QXmlNodeModelIndex is a default constructed
 value, otherwise false.

 A null QXmlNodeModelIndex doesn't represent any node and cannot
 be used in conjunction with QAbstractXmlNodeModel.
 */

/*!
 \fn const QAbstractXmlNodeModel *QXmlNodeModelIndex::model() const

 Returns the QAbstractXmlNodeModel that this node index refers to.
 QXmlNodeModelIndex does not own QAbstractXmlNodeModel and does not
 keep track of its lifetime, so this pointer will dangle if the
 QAbstractXmlNodeModel is deallocated first.

 There is no setter for the node model because instances of
 QXmlNodeModelIndex instances are only created with
 QAbstractXmlNodeModel::createIndex().
*/

/*!
 \fn qint64 QXmlNodeModelIndex::data() const

 Returns the first data value. The node index holds two data values.
 additionalData() returns the second one.

 \sa additionalData()
*/

/*!
 \fn void *QXmlNodeModelIndex::internalPointer() const

 Returns the first data value as a void* pointer.

 \sa additionalData()
*/

/*!
 \fn qint64 QXmlNodeModelIndex::additionalData() const

 Returns the second data value. The node index holds two data values.
 data() returns the first one.

 \sa data()
*/

/*!
 \fn void QXmlNodeModelIndex::reset()
 \internal

 Resets this QXmlNodeModelIndex to be null. It is equivalent to
 writing:

 \snippet doc/src/snippets/code/src_xmlpatterns_api_qabstractxmlnodemodel.cpp 0
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
 \fn QXmlNodeModelIndex QAbstractXmlNodeModel::elementById(const QXmlName &id) const

 Returns the index of the element identified as \a id. XQuery's \c
 id() function calls this function.

 The node index returned will be the element node whose value is of
 type \c ID and equals \a id, or it will be the element node that has
 an attribute whose typed value is of type \c ID and equals \a id. If
 there is no such element, a default constructed QXmlNodeModelIndex
 instance is returned. The implementor guarantees that if the returned
 node index is not null, it identifies an element.

 It is not sufficient for an attribute or element to merely be called
 \c id. Its value type must also be \c ID. However, the reserved name
 \c xml:id is sufficient.

 In \a id, the \c{namespace URI} and the \c{prefix} are undefined, and
 the \c{local name} is the ID that should be looked up.

 \sa {http://www.w3.org/TR/xpath-functions/#func-id}{XQuery 1.0 and XPath 2.0 Functions and Operators, 15.5.2 fn:id}
 */

/*!
 \fn QVector<QXmlNodeModelIndex> QAbstractXmlNodeModel::nodesByIdref(const QXmlName &idref) const

 Returns the elements and/or attributes that have an \c IDREF value
 equal to \a idref. XQuery's \c idref() function calls this function.

 The implementor guarantees that the nodes identified by the returned
 indexes are elements or attributes.

 It is not sufficient for an attribute or element to merely be called
 \c idref. It must also be of type \c IDREF. Elements must be typed as
 \c xs:IDREF or \c xs:IDREFS, or, in the case of attributes, as \c
 IDREF or \c IDREFS in the schema.

 In \a idref, the \c{namespace URI} and the \c{prefix} are undefined,
 and the \c{local name} is the ID that should be looked up.

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

 The caller guarantees that \a node is not \c null, and that is
 belongs to this QAbstractXmlNodeModel instance.

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
