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

#include <QtCore/QBuffer>
#include <QtCore/QStringList>

#include "qacceltreeresourceloader_p.h"
#include "qcommonvalues_p.h"
#include "qxmlresultitems.h"
#include "qxmlresultitems_p.h"
#include "qxmlserializer.h"
#include "qxpathhelper_p.h"

#include "qxmlquery.h"
#include "qxmlquery_p.h"

Q_DECLARE_METATYPE(QIODevice *)

QT_BEGIN_NAMESPACE

/*!
  \class QXmlQuery

  \brief The QXmlQuery class is used for performing queries on XML
  data, or on non-XML data modeled to look like XML.

  \reentrant
  \since 4.4
  \ingroup xml-tools

  The QXmlQuery class compiles and executes queries written in the
  \l {http://www.w3.org/TR/xquery/}{XQuery language}. QXmlQuery is
  typically used to query XML data, but it can also query non-XML
  data that has been modeled to look like XML.

  Using QXmlQuery to query XML data, as in the snippet below, is
  simple because it can use the built-in \l {QAbstractXmlNodeModel}
  {XML data model} as its delegate to the underlying query engine for
  traversing the data. The built-in data model is specified in \l
  {http://www.w3.org/TR/xpath-datamodel/} {XQuery 1.0 and XPath 2.0
  Data Model}.

  \snippet doc/src/snippets/code/src.xmlpatterns.api.qabstractxmlreceiver.cpp 0

  The example uses QXmlQuery to match the first paragraph of an XML
  document and then \l {QXmlSerializer} {output the result} to a
  device as XML.

  Using QXmlQuery to query \e {non-XML} data requires writing a
  subclass of QAbstractXmlNodeModel to use as a replacement for the
  built-in XML data model. The custom data model will be able to
  traverse the non-XML data as required by the QAbstractXmlNodeModel
  interface. An instance of this custom data model then becomes the
  delegate used by the query engine to traverse the non-XML data. For
  an example of how to use QXmlQuery to query non-XML data, see the
  documentation for QAbstractXmlNodeModel.

  \section1 Running Queries

  To run a query set up with QXmlQuery, call one of the evaluation
  functions.

  \list

  \o evaluateTo() is called with a pointer to an XML
  \l {QAbstractXmlReceiver} {receiver}, which receives the query
  results as a sequence of callbacks. The receiver callback class
  is like the callback class used for translating the output of a
  SAX parser. QXmlSerializer, for example, is a receiver callback
  class for translating the sequence of callbacks for output as
  XML text.

  \endlist

  \list

  \o evaluateTo() is called with a pointer to an iterator for
  an empty sequence of query \l {QXmlResultItems} {result items}.
  The Java-like iterator allows the query results to be accessed
  sequentially.

  \endlist

  \list

  \o evaluateTo() is similar to evaluateTo(), but the
  query must evaluate to a sequence of strings.

  \endlist

  \section1 Binding A Query To A Starting Node

  When a query is run on XML data, as in the snippet above, the \e
  doc() function returns the node in the built-in data model where the
  query evaluation will begin. But when a query is run on a custom
  node model representing non-XML data, one of the bindVariable()
  functions must be called to bind a variable name to a starting node.
  $variable name can then be used in the written query to access the
  starting node in the custom data model. It is not necessary to
  declare the variable name external in the query. See the example in
  the documentation for QAbstractXmlNodeModel.

  \section1 Reentrancy and Thread-Safety

  QXmlQuery is reentrant but not thread-safe. It is safe to use the
  QxmlQuery copy constructor to create a copy of a query and run the
  same query multiple times. Behind the scenes, QXmlQuery will reuse
  resources such as opened files and compiled queries to the extent
  possible. But it is not safe to use the same instance of QXmlQuery
  in multiple threads.

  \section1 Error Handling

  Errors can occur during query evaluation. Examples include type
  errors and file loading errors. When an error occurs:

  \list

  \o The error message is sent to the messageHandler().

  \o QXmlResultItems::hasError() will return \c true, or
  evaluateTo() will return \c false;

  \o The results of the evaluation function called are undefined.

  \endlist

  \section1 Resource Management

  A query can create \l {QXmlItem}{query result items}, open
  documents, and allocate other resources. All resources created,
  opened, or allocated by QtPatterns are normally managed and
  deallocated automatically, when they are no longer needed. If it
  should become necessary to deallocate resources manually, ensure
  that the relevant instances of QXmlQuery, QAbstractXmlReceiver, and
  QAbstractXmlForwardIterator have been destroyed.

  An instance of QCoreApplication must exist before QXmlQuery can be
  used.
 */

/*!
  Constructs an empty query. A default constructed query cannot be
  evaluated until setQuery() has been called.
 */
QXmlQuery::QXmlQuery() : d(new QXmlQueryPrivate())
{
}

/*!
  Constructs a QXmlQuery that is a copy of \a other.
 */
QXmlQuery::QXmlQuery(const QXmlQuery &other) : d(new QXmlQueryPrivate(*other.d))
{
}

/*!
  Constructs a QXmlQuery instance that will use \a np as its
  name pool. The query cannot be evaluated until setQuery()
  has been called.
 */
QXmlQuery::QXmlQuery(const QXmlNamePool &np) : d(new QXmlQueryPrivate())
{
    d->namePool = np;
}

/*!
  Destroys this QXmlQuery.
 */
QXmlQuery::~QXmlQuery()
{
    delete d;
}

/*!
  Assigns \a other to this QXmlQuery instance.
 */
QXmlQuery &QXmlQuery::operator=(const QXmlQuery &other)
{
    if(d != other.d)
        *d = *other.d;

    return *this;
}

/*!
  Changes the \l {QAbstractMessageHandler}{message handler} for this
  QXmlQuery to \a aMessageHandler. The query sends all compile and
  runtime messages to this message handler. QXmlQuery does not take
  ownership of \a aMessageHandler.

  Normally, the default message handler is sufficient. It writes
  compile and runtime messages to \e stderr. The default message
  handler includes color codes if \e stderr can render colors.

  Note that changing the message handler after the query has been
  compiled has no effect, ie the query uses the same message handler
  at runtime that it uses at compile time.

  When QXmlQuery calls QAbstractMessageHandler::message(),
  the arguments are as follows:

  \table
  \header
    \o QAbstractMessageHandler::handle() argument
    \o Passed
  \row
    \o QtMsgType type
    \o Only QtWarningMsg and QtFatalMsg are used. The former
       identifies a compile or runtime warning, while the
       latter identifies a dynamic or static error.
  \row
    \o const QString & description
    \o An XHTML document which is the actual message. It is translated
       into the current language.
  \row
    \o const QUrl &identifier
    \o Identifies the error with a URI, where the fragment is
       the error code, and the rest of the URI is the error namespace.
  \row
    \o const QSourceLocation & sourceLocation
    \o Identifies where the error occurred.
  \endtable

 */
void QXmlQuery::setMessageHandler(QAbstractMessageHandler *aMessageHandler)
{
    d->messageHandler = aMessageHandler;
}

/*!
    Returns the message handler that handles compile and runtime
    messages for this QXmlQuery.
 */
QAbstractMessageHandler *QXmlQuery::messageHandler() const
{
    return d->messageHandler;
}

/*!
  Sets this QXmlQuery to the XQuery read from the \a sourceCode
  device.  The device must have been opened with at least
  QIODevice::ReadOnly.

  \a documentURI represents the query obtained from the \a sourceCode
  device. It is the Base URI of the static context, as defined in the
  \l {http://www.w3.org/TR/xquery/}{XQuery language}. It is used
  internally to resolve relative URIs that appear in the query, and
  for message reporting. \a documentURI can be empty. If it is empty,
  the \l{QCoreApplication::applicationFilePath()} {application file
  path} is used. If it is not empty, it may be either relative or
  absolute. If it is relative, it is resolved itself against the
  \l {QCoreApplication::applicationFilePath()} {application file
  path} before it is used. If \a documentURI is neither a valid URI
  nor empty, the result is undefined.

  If the query contains a static error (e.g. syntax error), an error
  message is sent to the messageHandler(), and isValid() will return
  \e false.

  Variables must be bound before setQuery() is called.

  The encoding of the XQuery in \a sourceCode is detected internally
  using the rules for setting and detecting encoding of XQuery files,
  which are explained in the \l {http://www.w3.org/TR/xquery/}
  {XQuery language}.

  \sa isValid()
 */
void QXmlQuery::setQuery(QIODevice *sourceCode, const QUrl &documentURI)
{
    Q_ASSERT_X(sourceCode, Q_FUNC_INFO, "A null QIODevice pointer cannot be passed.");
    Q_ASSERT_X(sourceCode->isReadable(), Q_FUNC_INFO, "The device must be readable.");

    d->queryURI = QXmlQueryPrivate::normalizeQueryURI(documentURI);
    d->expression(sourceCode);
}

/*!
  \overload
  The behavior and requirements of this function are the same as for
  setQuery(QIODevice *, const QUrl &) after the XQuery has been read
  from the IO device into a string. Because \a sourceCode is already
  a Unicode string detection of its encoding is unnecessary.
*/
void QXmlQuery::setQuery(const QString &sourceCode, const QUrl &documentURI)
{
    Q_ASSERT_X(documentURI.isEmpty() || documentURI.isValid(), Q_FUNC_INFO,
               "The document URI must be valid.");

    QByteArray query(sourceCode.toUtf8());
    QBuffer buffer(&query);
    buffer.open(QIODevice::ReadOnly);

    setQuery(&buffer, documentURI);
}

/*!
  Sets this QXmlQuery to the XQuery read from the \a queryURI.  Use
  isValid() after calling this function. If an error occurred reading
  \a queryURI, e.g., the query does not exist, cannot be read, or is
  invalid, isValid() will return \e false.

  The supported URI schemes are the same as those in the XQuery
  function \c fn:doc, except that queryURI can be the object of
  a variable binding.

  \a baseURI is the Base URI of the static context, as defined in the
  \l {http://www.w3.org/TR/xquery/}{XQuery language}. It is used
  internally to resolve relative URIs that appear in the query, and
  for message reporting. If \a baseURI is empty, \a queryURI is used.
  Otherwise, \a baseURI is used, and it is resolved against the \l
  {QCoreApplication::applicationFilePath()} {application file path} if
  it is relative.

  If \a queryURI is empty or invalid, or if \a baseURI is invalid,
  the behavior of this function is undefined.
 */
void QXmlQuery::setQuery(const QUrl &queryURI, const QUrl &baseURI)
{
    Q_ASSERT_X(queryURI.isValid(), Q_FUNC_INFO, "The passed URI must be valid.");

    const QUrl canonicalURI(QXmlQueryPrivate::normalizeQueryURI(queryURI));
    Q_ASSERT(canonicalURI.isValid());
    Q_ASSERT(!canonicalURI.isRelative());
    Q_ASSERT(baseURI.isValid() || baseURI.isEmpty());

    d->queryURI = QXmlQueryPrivate::normalizeQueryURI(baseURI.isEmpty() ? queryURI : baseURI);

    QNetworkAccessManager networkManager;
    QIODevice * result = 0;

    try
    {
        result = QPatternist::AccelTreeResourceLoader::load(canonicalURI, &networkManager,
                                                            d->staticContext());
    }
    catch(const QPatternist::Exception)
    {
        /* We do nothing, result will be 0. */
    }

    if(result)
    {
        setQuery(result, d->queryURI);
        result->close();
    }
    else
        d->recompileRequired();

    delete result;
}

/*!
  Binds the variable \a name to the \a value so that $\a name can be
  used from within the query to refer to the \a value.

  \a name must not be \e null, i.e., \a {name}.isNull() must return
  \e false. If \a name has already been bound, its previous binding
  will be overriden.

  If \a {value}.isNull() returns \e true, the existing binding for
  the \a name is removed.

  For example, in order to bind a value of type QString or QUrl, wrap
  the value in a QVariant such that QXmlItem's QVariant constructor is
  called.

  All strings fed into the query must be valid XQuery strings. More specifically, they
  must consist of only XML 1.0 characters. This is not checked. If they are not, the
  behavior is undefined.

  \sa QVariant::isValid(), {QtXDM}{How QVariant maps to XQuery's Data Model},
   QXmlItem::isNull()
 */
void QXmlQuery::bindVariable(const QXmlName &name, const QXmlItem &value)
{
    Q_ASSERT_X(!name.isNull(), Q_FUNC_INFO, "The name cannot be null.");

    if(d->variableBindings.contains(name))
    {
        /* If the type of the variable changed(as opposed to only the value),
         * we will have to recompile. */
        if(QXmlQueryPrivate::isSameType(d->variableBindings.value(name), value) || value.isNull())
            d->updateVariableValues = true;
        else
            d->recompileRequired();
    }

    if(value.isNull() || d->deviceBindings.contains(name))
        d->recompileRequired();

    d->deviceBindings.take(name);
    d->variableBindings.insert(name, value);
}

/*!
  \overload

  This function constructs a QXmlName from \a localName using the
  query's \l {QXmlNamePool} {namespace}. The function then behaves as
  the overloaded function. It is equivalent to the following snippet.

  \snippet doc/src/snippets/code/src.xmlpatterns.api.qxmlquery.cpp 0
 */
void QXmlQuery::bindVariable(const QString &localName, const QXmlItem &value)
{
    bindVariable(QXmlName(d->namePool, localName), value);
}

/*!
  Binds the variable \a name to the \a device so that $\a name can be
  used from within the query to refer to the \a device. The QIODevice
  \a device is exposed to the query as a URI of type \c xs:anyURI,
  which can be passed to the \c fn:doc() function to be read. e.g.,
  this function can be used to pass an XML document in the memory to
  \c fn:doc.

  \snippet doc/src/snippets/code/src.xmlpatterns.api.qxmlquery.cpp 1

  The caller must ensure that \c device has been opened with at least
  QIODevice::ReadOnly prior to this binding, or else the behavior is
  undefined.

  If the query will access an XML document contained in a QString, you
  can do so with QBuffer, as shown in the code snippet below. Suppose
  \e myQString contains \e {<document>content</document>}

  \snippet doc/src/snippets/qxmlquery/bindingExample.cpp 0

  \a name must not be \e null, i.e., \a {name}.isNull() must return \e
  false. If \a name has already been bound, its previous binding will
  be overriden. The URI that \a name evaluates to is arbitrary and may
  change.

  For as long as this QXmlQuery instance is allocated, \a device must not
  be deleted.
*/
void QXmlQuery::bindVariable(const QXmlName &name, QIODevice *device)
{
    Q_ASSERT_X(!device || device->isReadable(), Q_FUNC_INFO, "A readable QIODevice must be passed.");

    if(!device)
    {
        d->variableBindings.take(name);
        d->deviceBindings.take(name);
        d->recompileRequired();
        return;
    }

    if(d->deviceBindings.contains(name))
        d->updateVariableValues = true;
    else
    {
        d->recompileRequired();
    }

    d->variableBindings.take(name);
    d->deviceBindings.insert(name, device);
}

/*!
  \overload

  If \a localName is a valid \l {QXmlName::isNCName()} {NCName}, this
  function is equivalent to the following snippet.

  \snippet doc/src/snippets/code/src.xmlpatterns.api.qxmlquery.cpp 2

  A QXmlName is constructed from \a localName, and is passed
  to the appropriate overload along with \a device.

  \sa QXmlName::isNCName()
 */
void QXmlQuery::bindVariable(const QString &localName, QIODevice *device)
{
    bindVariable(QXmlName(d->namePool, localName), device);
}

/*!

  Evaluates this query and sends the result as a sequence of callbacks
  to the \l {QAbstractXmlReceiver} {receiver} \a callback. QXmlQuery
  does not take ownership of \a callback.

  If an error occurs during the evaluation, error messages are sent to
  messageHandler() and \e false is returned.

  If \a callback is null, or if this query \l {isValid()} {is invalid},
  \e false is returned and the behavior is undefined.

  \sa QAbstractXmlReceiver,  isValid()
 */
bool QXmlQuery::evaluateTo(QAbstractXmlReceiver *callback) const
{
    Q_ASSERT_X(callback, Q_FUNC_INFO,
               "A valid callback must be passed. Otherwise the result cannot be sent anywhere.");

    if(isValid())
    {
        try
        {
            /*
             * This order is significant. expression() might cause
             * query recompilation, and as part of that it recreates
             * the static context. However, if we create the dynamic
             * context before the query recompilation has been
             * triggered, it will use the old static context, and
             * hence old source locations.
             */
            const QPatternist::Expression::Ptr expr(d->expression());
            const QPatternist::DynamicContext::Ptr dynContext(d->dynamicContext(callback));
            callback->startOfSequence();
            expr->evaluateToSequenceReceiver(dynContext);
            callback->endOfSequence();
            return true;
        }
        catch(const QPatternist::Exception)
        {
            return false;
        }
    }
    else
        return false;
}

/*!

  Attempts to evaluate the query and returns the results in the
  \a target \l {QStringList} {string list}..

  If the query \l {isValid()} {is valid} and the evaluation succeeds,
  \e true is returned. Otherwise, \e false is returned and the
  contents of \a target are undefined.

  The query must evaluate to a sequence of \c xs:string values, and
  this is checked by compiling the query as if it had been passed to a
  function accepting \c xs:string*. This means that the result nodes
  will be converted to strings, and values that cannot be promoted to
  \c xs:string will trigger type errors.

  If \a target is \c null, the behavior is undefined.
 */
bool QXmlQuery::evaluateTo(QStringList *target) const
{
    Q_ASSERT_X(target, Q_FUNC_INFO, "The pointer to the QStringList cannot be null.");

    if(isValid())
    {
        try
        {
            /*
             * This order is significant. expression() might cause
             * query recompilation, and as part of that it recreates
             * the static context. However, if we create the dynamic
             * context before the query recompilation has been
             * triggered, it will use the old static context, and
             * hence old source locations.
             */
            const QPatternist::Expression::Ptr expr(d->expression());
            if(!expr)
                return false;

            QPatternist::DynamicContext::Ptr dynContext(d->dynamicContext());

            if(!QPatternist::BuiltinTypes::xsString->xdtTypeMatches(expr->staticType()->itemType()))
                return false;

            const QPatternist::Item::Iterator::Ptr it(expr->evaluateSequence(dynContext));
            QPatternist::Item next(it->next());

            while(!next.isNull())
            {
                target->append(next.stringValue());
                next = it->next();
            }

            return true;
        }
        catch(const QPatternist::Exception)
        {
            return false;
        }
    }
    else
        return false;
}

/*!
  Starts the evaluation and makes it available in \a result.  If \a
  result is \c null, the behavior is undefined. The evaluation takes
  place incrementally (lazy evaluation), when QXmlResultItems::next()
  is called.

  \sa QXmlResultItems::next()
*/
void QXmlQuery::evaluateTo(QXmlResultItems *result) const
{
    Q_ASSERT_X(result, Q_FUNC_INFO, "A null pointer cannot be passed to QXmlQuery::evaluateTo.");

    if(isValid())
    {
        try
        {
            /*
             * We don't have the d->expression() calls and
             * d->dynamicContext() calls in the same order as seen in
             * QXmlQuery::evaluateTo(), and the reason to why
             * that isn't a problem, is that we call isValid().
             */
            const QPatternist::DynamicContext::Ptr dynContext(d->dynamicContext());
            result->d_ptr->setDynamicContext(dynContext);
            result->d_ptr->iterator = d->expression()->evaluateSequence(dynContext);
        }
        catch(const QPatternist::Exception)
        {
            result->d_ptr->iterator = QPatternist::CommonValues::emptyIterator;
            result->d_ptr->hasError = true;
        }
    }
    else
    {
        result->d_ptr->iterator= QPatternist::CommonValues::emptyIterator;
        result->d_ptr->hasError = true;
    }
}

/*!
  Returns \c true if this query is valid. Examples of invalid queries
  are ones that contain syntax errors or that have not had setQuery()
  called for them yet.
 */
bool QXmlQuery::isValid() const
{
    return d->isValid();
}

/*!
  Sets the URI resolver to \a resolver.

  QXmlQuery does not take ownership of \a resolver.

  \sa uriResolver()
 */
void QXmlQuery::setUriResolver(const QAbstractUriResolver *resolver)
{
    d->uriResolver = resolver;
}

/*!
  Returns the URI resolver in use. If no URI resolver has been set,
  QtXmlPatterns will use the URI in queries as is.

  The URI resolver provides a level of abstraction or "polymorphic
  URIs." A resolver can rewrite "logical" URIs to physical ones, or it
  can translate obsolete or invalid URIs to valid ones.

  QtXmlPatterns calls the URI resolver for all URIs it encounters,
  except for namespaces. Specifically:

  \list
    \o All builtin functions that deals with URIs. This is \c fn:doc(),
       and \c fn:doc-available()
  \endlist

  For instance, in the case of \c fn:doc(), the absolute URI is the
  base URI in the static context (which most likely is the location of
  the query). Instead of using the URI that the user specified, the
  return value of QAbstractUriResolver::resolve() will be used.

  When QtXmlPatterns calls QAbstractUriResolver::resolve() the
  absolute URI is the URI mandated by the XQuery language, and the
  relative URI is the URI specified by the user.

  \sa setUriResolver()
 */
const QAbstractUriResolver *QXmlQuery::uriResolver() const
{
    return d->uriResolver;
}

/*!
  Returns the name pool used by this QXmlQuery for constructing \l
  {QXmlName} {names}. There is no setter for the name pool, because
  mixing name pools causes errors due to name confusion.
 */
QXmlNamePool QXmlQuery::namePool() const
{
    return d->namePool;
}

/*!
  Sets the focus to \a item.

  The focus is the set of items that the context
  item expression and path expressions navigate from. For example, in
  the expression \e p/span, the element that \e p evaluates to, is the
  focus for the following expression, \e span.

  The focus can be accessed using the context item expression, a dot, \e ".".

  By default the focus is not set, it is undefined, and will therefore
  result in a dynamic error, \c XPDY0002, if attempted to be used.

  The focus must be set before the query is set with setQuery().
 */
void QXmlQuery::setFocus(const QXmlItem &item)
{
    d->contextItem = item;
}

QT_END_NAMESPACE
