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

#include <QString>

#include "qitem_p.h"

#include "qabstractxmlreceiver_p.h"
#include "qabstractxmlreceiver.h"

QT_BEGIN_NAMESPACE

/*!
  \class QAbstractXmlReceiver
  \brief The QAbstractXmlReceiver class provides a callback interface
         for transforming the output of a QXmlQuery.
  \reentrant
  \since 4.4
  \ingroup xml-tools

  QAbstractXmlReceiver is an abstract base class that provides
  a callback interface for receiving an \l {XQuery Sequence}
  {XQuery sequence}, usually the output of an QXmlQuery, and
  transforming that sequence into a structure of your choosing,
  usually XML. Consider the example:

  \snippet doc/src/snippets/code/src.xmlpatterns.api.qabstractxmlreceiver.cpp 0

  First it constructs a \l {QXmlQuery} {query} that gets the
  first paragraph from document \c index.html. Then it constructs
  an \l {QXmlSerializer} {XML serializer} with the \l {QXmlQuery}
  {query} and \l {QIODevice} {myOutputDevice} (Note the
  \l {QXmlSerializer} {serializer} is an \e {XML receiver},
  ie a subclass of QAbstractXmlReceiver). Finally, it
  \l {QXmlQuery::evaluateTo()} {evaluates} the
  \l {QXmlQuery} {query}, producing an ordered sequence of calls
  to the \l {QXmlSerializer} {serializer's} callback functions.
  The sequence of callbacks transforms the query output to XML
  and writes it to \l {QIODevice} {myOutputDevice}.

  Although the example uses \l {QXmlQuery} to produce the sequence
  of callbacks to functions in QAbstractXmlReceiver, you can call
  the callback functions directly as long as your sequence of
  calls represents a valid \l {XQuery Sequence} {XQuery sequence}.

  \target XQuery Sequence
  \section1 XQuery Sequences

  An XQuery \a sequence is an ordered collection of zero, one,
  or many \e items. Each \e item is either an \e {atomic value}
  or a \e {node}. An \e {atomic value} is a simple data value.

  There are six kinds of \e nodes.

  \list

  \o An \e {Element Node} represents an XML element.

  \o An \e {Attribute Node} represents an XML attribute.

  \o A \e {Document Node} represents an entire XML document.

  \o A \e {Text Node} represents character data (element content).

  \o A \e {Processing Instruction Node} represents an XML
  processing instruction, which is used in an XML document
  to tell the application reading the document to perform
  some action. A typical example is to use a processing
  instruction to tell the application to use a particular
  XSLT stylesheet to display the document.

  \o And a \e {Comment node} represents an XML comment.

  \endlist

  The \e sequence of \e nodes and \e {atomic values} obeys
  the following rules. Note that \e {Namespace Node} refers
  to a special \e {Attribute Node} with name \e {xmlns}.

  \list

  \o Each \e node appears in the \e sequence before its children
  and their descendants appear.

  \o A \e node's descendants appear in the \e sequence before
  any of its siblings appear.

  \o A \e {Document Node} represents an entire document. Zero or
  more \e {Document Nodes} can appear in a \e sequence, but they
  can only be top level items (i.e., a \e {Document Node} can't
  be a child of another \e node.

  \o \e {Namespace Nodes} immediately follow the \e {Element Node}
   with which they are associated.

  \o \e {Attribute Nodes} immediately follow the \e {Namespace Nodes}
   of the element with which they are associated, or...

   \o If there are no \e {Namespace Nodes} following an element, then
   the \e {Attribute Nodes} immediately follow the element.

   \o An \e {atomic value} can only appear as a top level \e item,
   i.e., it can't appear as a child of a \e node.

   \o \e {Processing Instruction Nodes} do not have children, and
   their parent is either a \e {Document Node} or an \e {Element
   Node}.

  \endlist

  The \e sequence of \e nodes and \e {atomic values} is sent to
  an QAbstractXmlReceiver (QXmlSerializer in
  the example above) as a sequence of calls to the receiver's
  callback functions. The mapping of callback functions to
  sequence items is as follows.

  \list

  \o startDocument() and endDocument() are called for each
  \e {Document Node} in the \e sequence. endDocument() is not
  called until all the \e {Document Node's} children have
  appeared in the \e sequence.

  \o startElement() and endElement() are called for each
  \e {Element Node}. endElement() is not called until all the
  \e {Element Node's} children have appeared in the \e sequence.

  \o attribute() is called for each \e {Attribute Node}.

  \o comment() is called for each \e {Comment Node}.

  \o characters() is called for each \e {Text Node}.

  \o processingInstruction() is called for each \e {Processing
  Instruction Node}.

  \o namespaceBinding() is called for each \e {Namespace Node}.

  \o atomicValue() is called for each \e {atomic value}.

  \endlist

  For a complete explanation of XQuery sequences, visit
  \l {http://www.w3.org/TR/xpath-datamodel/}{XQuery Data Model}.

  \sa {http://www.w3.org/TR/xpath-datamodel/}{W3C XQuery 1.0 and XPath 2.0 Data Model (XDM)}
  \sa QXmlSerializer
  \sa QXmlResultItems
 */

template<const QXmlNodeModelIndex::Axis axis>
void QAbstractXmlReceiver::sendFromAxis(const QXmlNodeModelIndex &node)
{
    Q_ASSERT(!node.isNull());
    const QXmlNodeModelIndex::Iterator::Ptr it(node.iterate(axis));
    QXmlNodeModelIndex next(it->next());

    while(!next.isNull())
    {
        sendAsNode(next);
        next = it->next();
    }
}

/*!
 \internal
 */
QAbstractXmlReceiver::QAbstractXmlReceiver(QAbstractXmlReceiverPrivate *d)
  : d_ptr(d)
{
}

/*!
  The default constructor is sufficient.
 */
QAbstractXmlReceiver::QAbstractXmlReceiver() : d_ptr(0)
{
}

/*!
  The destructor is virtual.
 */
QAbstractXmlReceiver::~QAbstractXmlReceiver()
{
    delete d_ptr;
}

/*!
  \fn void QAbstractXmlReceiver::startElement(const QXmlName &name)

  This callback is called when a new element node appears
  in the \l {XQuery Sequence} {sequence}. \a name is the
  valid \l {QXmlName} {name} of the node element.
 */

/*!
  \fn void QAbstractXmlReceiver::endElement()

  This callback is called when the end of an element node
  appears in the \l {XQuery Sequence} {sequence}.
*/

/*!
  \fn void QAbstractXmlReceiver::attribute(const QXmlName &name,
                                           const QStringRef &value)
  This callback is called when an attribute node
  appears in the \l {XQuery Sequence} {sequence}.
  \a name is the \l {QXmlName} {attribute name} and
  the \a value string contains the attribute value.
 */

/*!
  \fn void QAbstractXmlReceiver::comment(const QString &value)

  This callback is called when a comment node appears
  in the \l {XQuery Sequence} {sequence}. The \a value
  is the comment text, which must not contain the string
  "--".
 */

/*!
  \fn void QAbstractXmlReceiver::characters(const QStringRef &value)

  This callback is called when a text node appears in the
  \l {XQuery Sequence} {sequence}. The \a value contains
  the text. Adjacent text nodes may not occur in the
  \l {XQuery Sequence} {sequence}, i.e., this callback must not
  be called twice in a row.
 */

/*!
  \fn void QAbstractXmlReceiver::startDocument()

  This callback is called when a document node appears
  in the \l {XQuery Sequence} {sequence}.
 */

/*!
  \fn void QAbstractXmlReceiver::endDocument()

  This callback is called when the end of a document node
  appears in the \l {XQuery Sequence} {sequence}.
 */

/*!
  \fn void QAbstractXmlReceiver::processingInstruction(const QXmlName &target,
                                                       const QString &value)

  This callback is called when a processing instruction
  appears in the \l {XQuery Sequence} {sequence}.
  A processing instruction is used in an XML document
  to tell the application reading the document to
  perform some action. A typical example is to use a
  processing instruction to tell the application to use a
  particular XSLT stylesheet to process the document.

  \quotefile doc/src/snippets/patternist/xmlStylesheet.xq

  \a target is the \l {QXmlName} {name} of the processing
  instruction. Its \e prefix and \e {namespace URI} must both
  be empty. Its \e {local name} is the target. In the above
  example, the name is \e {xml-stylesheet}.

  The \a value specifies the action to be taken. Note that
  the \a value must not contain the string "?>". In the above
  example, the \a value is \e{type="test/xsl" href="formatter.xsl}.

  Genrally, use of processing instructions should be avoided,
  because they are not namespace aware and in many contexts
  are stripped out anyway. Processing instructions can often
  be replaced with elements from a custom namespace."
 */

/*!
  \fn void QAbstractXmlReceiver::atomicValue(const QVariant &value)

  This callback is called when an atomic value appears
  in the \l {XQuery Sequence} {sequence}. The \a value is
  a simple \l {QVariant} {value} {data value}. It is
  guaranteed to be \l {QVariant::isValid()} {valid}.
 */

/*!
  \fn virtual void QAbstractXmlReceiver::namespaceBinding(const QXmlName &name)

  This callback is called when a namespace binding
  is in scope of an element.

  A namespace is defined by a URI. In the \l {QXmlName}
  \a name, the value of \l {QXmlName::namespaceUri()} is that URI. The value
  of \l {QXmlName::prefix()} is the prefix that the URI is bound to. The local name
  is insignificant and can be an arbitrary value.
 */

/*!
  \internal

   Treats \a outputItem as an node and calls the appropriate function,
   such as attribute() or comment(), depending on its
   QXmlNodeModelIndex::NodeKind.

   This a helper function sub-classes can use to multi-plex Nodes received
   via item().
 */
void QAbstractXmlReceiver::sendAsNode(const QPatternist::Item &outputItem)
{
    Q_ASSERT(outputItem);
    Q_ASSERT(outputItem.isNode());
    const QXmlNodeModelIndex asNode = outputItem.asNode();

    switch(asNode.kind())
    {
        case QXmlNodeModelIndex::Attribute:
        {
            const QString &v = outputItem.stringValue();
            attribute(asNode.name(), QStringRef(&v));
            return;
        }
        case QXmlNodeModelIndex::Element:
        {
            startElement(asNode.name());

            /* First the namespaces, then attributes, then the children. */
            asNode.sendNamespaces(this);
            sendFromAxis<QXmlNodeModelIndex::AxisAttribute>(asNode);
            sendFromAxis<QXmlNodeModelIndex::AxisChild>(asNode);

            endElement();

            return;
        }
        case QXmlNodeModelIndex::Text:
        {
            const QString &v = outputItem.stringValue();
            characters(QStringRef(&v));
            return;
        }
        case QXmlNodeModelIndex::ProcessingInstruction:
        {
            processingInstruction(asNode.name(), outputItem.stringValue());
            return;
        }
        case QXmlNodeModelIndex::Comment:
        {
            comment(outputItem.stringValue());
            return;
        }
        case QXmlNodeModelIndex::Document:
        {
            startDocument();
            sendFromAxis<QXmlNodeModelIndex::AxisChild>(asNode);
            endDocument();
            return;
        }
        case QXmlNodeModelIndex::Namespace:
            Q_ASSERT_X(false, Q_FUNC_INFO, "Not implemented");
    }

    Q_ASSERT_X(false, Q_FUNC_INFO,
               QString::fromLatin1("Unknown node type: %1").arg(asNode.kind()).toUtf8().constData());
}

/*!
  \internal

   This function may be called instead of characters() if, and only if,
   \a value consists only of whitespace.

   The caller gurantees that \a value, is not empty.

   By whitespace is meant a sequence of characters that are either
   spaces, tabs, or the two new line characters, in any order. In
   other words, the whole of Unicode's whitespace category is not
   considered whitespace.

   However, there's no guarantee or requirement that whitespaceOnly()
   is called for text nodes containing whitespace only, characters()
   may be called just as well. This is why the default implementation
   for whitespaceOnly() calls characters().

   @see characters()
 */
void QAbstractXmlReceiver::whitespaceOnly(const QStringRef &value)
{
    Q_ASSERT_X(value.toString().trimmed().isEmpty(), Q_FUNC_INFO,
               "The caller must guarantee only whitespace is passed. Use characters() in other cases.");
    const QString &v = value.toString();
    characters(QStringRef(&v));
}

/*!
  \internal
 */
void QAbstractXmlReceiver::item(const QPatternist::Item &item)
{
    if(item.isNode())
        return sendAsNode(item);
    else
        atomicValue(QPatternist::AtomicValue::toQt(item.asAtomicValue()));
}

/*!
 \fn void QAbstractXmlReceiver::startOfSequence()

 This callback is called once only, right before the
 \l {XQuery Sequence} {sequence} begins.
 */

/*!
  \fn void QAbstractXmlReceiver::endOfSequence()

 This callback is called once only, right after the
 \l {XQuery Sequence} {sequence} ends.
 */

QT_END_NAMESPACE

