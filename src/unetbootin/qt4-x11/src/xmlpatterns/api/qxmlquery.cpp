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

#include <QtCore/QBuffer>
#include <QtCore/QStringList>
#include <QtXmlPatterns/QXmlFormatter>

#include "qacceltreeresourceloader_p.h"
#include "qcommonvalues_p.h"
#include "qxmlresultitems.h"
#include "qxmlresultitems_p.h"
#include "qxmlserializer.h"
#include "qxpathhelper_p.h"

#include "qxmlquery.h"
#include "qxmlquery_p.h"

QT_BEGIN_NAMESPACE

/*!
  \class QXmlQuery

  \brief The QXmlQuery class performs XQueries on XML data, or on non-XML data modeled to look like XML.

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

  \snippet doc/src/snippets/code/src_xmlpatterns_api_qabstractxmlreceiver.cpp 0

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

  \section1 Running XQueries

  To run a query set up with QXmlQuery, call one of the evaluation
  functions.

  \list

  \o evaluateTo(QAbstractXmlReceiver *) is called with a pointer to an
  XML \l {QAbstractXmlReceiver} {receiver}, which receives the query
  results as a sequence of callbacks. The receiver callback class is
  like the callback class used for translating the output of a SAX
  parser. QXmlSerializer, for example, is a receiver callback class
  for translating the sequence of callbacks for output as unformatted
  XML text.

  \endlist

  \list

  \o evaluateTo(QXmlResultItems *) is called with a pointer to an
  iterator for an empty sequence of query \l {QXmlResultItems} {result
  items}.  The Java-like iterator allows the query results to be
  accessed sequentially.

  \endlist

  \list

  \o evaluateTo(QStringList *) is like evaluateTo(QXmlResultItems *),
  but the query must evaluate to a sequence of strings.

  \endlist

  \section1 Running XPath Expressions

  The XPath language is a subset of the XQuery language, so
  running an XPath expression is the same as running an XQuery
  query. Pass the XPath expression to QXmlQuery using setQuery().

  \section1 Running XSLT stylesheets

  Running an XSLT stylesheet is like running an XQuery, except that
  when you construct your QXmlQuery, you must pass QXmlQuery::XSLT20
  to tell QXmlQuery to interpret whatever it gets from setQuery() as
  an XSLT stylesheet instead of as an XQuery. You must also set the
  input document by calling setFocus().

  \snippet doc/src/snippets/code/src_xmlpatterns_api_qxmlquery.cpp 7

  \note Currently, setFocus() must be called \e before setQuery() when
  using XSLT.

  Another way to run an XSLT stylesheet is to use the \c xmlpatterns
  command line utility.

  \code
  xmlpatterns myStylesheet.xsl myInput.xml
  \endcode

  \note For the current release, XSLT support should be considered
        experimental. See section \l{QtXmlPatterns Module#XSLT
        2.0}{XSLT conformance} for details.

  Stylesheet parameters are bound using bindVariable().

  \section1 Binding A Query To A Starting Node

  When a query is run on XML data, as in the snippet above, the
  \c{doc()} function returns the node in the built-in data model where
  the query evaluation will begin. But when a query is run on a custom
  node model containing non-XML data, one of the bindVariable()
  functions must be called to bind a variable name to a starting node
  in the custom model.  A $variable reference is used in the XQuery
  text to access the starting node in the custom model. It is not
  necessary to declare the variable name external in the query. See
  the example in the documentation for QAbstractXmlNodeModel.

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

  \o QXmlResultItems::hasError() will return \c{true}, or
  evaluateTo() will return \c{false};

  \o The results of the evaluation are undefined.

  \endlist

  \section1 Resource Management

  When a query runs, it parses documents, allocating internal data
  structures to hold them, and it may load other resources over the
  network. It reuses these allocated resources when possible, to
  avoid having to reload and reparse them.

  When setQuery() is called, the query text is compiled into an
  internal data structure and optimized. The optimized form can
  then be reused for multiple evaluations of the query. Since the
  compile-and-optimize process can be expensive, repeating it for
  the same query should be avoided by using a separate instance of
  QXmlQuery for each query text.

  Once a document has been parsed, its internal representation is
  maintained in the QXmlQuery instance and shared among multiple
  QXmlQuery instances.

  An instance of QCoreApplication must exist before QXmlQuery can be
  used.

  \section1 Event Handling

  When QXmlQuery accesses resources (e.g., calling \c fn:doc() to load a file,
  or accessing a device via a bound variable), the event loop is used, which
  means events will be processed. To avoid processing events when QXmlQuery
  accesses resources, create your QXmlQuery instance in a separate thread.
 */

/*!
 \enum QXmlQuery::QueryLanguage
 \since 4.5

 Specifies whether you want QXmlQuery to interpret the input to
 setQuery() as an XQuery or as an XSLT stylesheet.

 \value XQuery10 XQuery 1.0.
 \value XSLT20 XSLT 2.0

 \sa setQuery()
 */

// ### Qt5: Merge constructor overloads
/*!
  Constructs an invalid, empty query that cannot be used until
  setQuery() is called.

  \note This constructor must not be used if you intend to use
  this QXmlQuery to process XSL-T stylesheets. The other constructor
  must be used in that case.
 */
QXmlQuery::QXmlQuery() : d(new QXmlQueryPrivate())
{
}

/*!
  Constructs a QXmlQuery that is a copy of \a other. The new
  instance will share resources with the existing query
  to the extent possible.
 */
QXmlQuery::QXmlQuery(const QXmlQuery &other) : d(new QXmlQueryPrivate(*other.d))
{
    /* First we have invoked QXmlQueryPrivate's synthesized copy constructor.
     * Keep this section in sync with QXmlQuery::operator=(). */
    d->detach();
}

/*!
  Constructs a query that will use \a np as its name pool. The query
  cannot be evaluated until setQuery() has been called.
 */
QXmlQuery::QXmlQuery(const QXmlNamePool &np) : d(new QXmlQueryPrivate(np))
{
}

/*!

  Constructs a query that will be used to run Xqueries or XSL-T
  stylesheets, depending on the value of \a queryLanguage. It will use
  \a np as its name pool.

  \note If your QXmlQuery will process XSL-T stylesheets, this
  constructor must be used. The default constructor can only
  create instances of QXmlQuery for running XQueries.

  \note The XSL-T support in this release is considered experimental.
  See the \l{QtXmlPatterns Module#XSLT 2.0}{XSLT conformance} for
  details.

 \since 4.5
 \sa queryLanguage()
 */
QXmlQuery::QXmlQuery(QueryLanguage queryLanguage,
                     const QXmlNamePool &np) : d(new QXmlQueryPrivate(np))
{
    d->queryLanguage = queryLanguage;
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
    /* Keep this section in sync with QXmlQuery::QXmlQuery(const QXmlQuery &).
     */
    if(d != other.d)
    {
        *d = *other.d;
        d->detach();
    }

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
  compiled has no effect, i.e. the query uses the same message handler
  at runtime that it uses at compile time.

  When QXmlQuery calls QAbstractMessageHandler::message(),
  the arguments are as follows:

  \table
  \header
    \o message() argument
    \o Semantics
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
  Sets this QXmlQuery to an XQuery read from the \a sourceCode
  device.  The device must have been opened with at least
  QIODevice::ReadOnly.

  \a documentURI represents the query obtained from the \a sourceCode
  device. It is the base URI of the static context, as defined in the
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

  If \a sourceCode is \c null or not readable, or if \a documentURI is not
  a valid URI, behavior is undefined.
  \sa isValid()
 */
void QXmlQuery::setQuery(QIODevice *sourceCode, const QUrl &documentURI)
{
    if(!sourceCode)
    {
        qWarning("A null QIODevice pointer cannot be passed.");
        return;
    }

    if(!sourceCode->isReadable())
    {
        qWarning("The device must be readable.");
        return;
    }

    d->queryURI = QXmlQueryPrivate::normalizeQueryURI(documentURI);
    d->expression(sourceCode);
}

/*!
  \overload
  The behavior and requirements of this function are the same as for
  setQuery(QIODevice*, const QUrl&), after the XQuery has been read
  from the IO device into a string. Because \a sourceCode is already
  a Unicode string, detection of its encoding is unnecessary.
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
  function \c{fn:doc}, except that queryURI can be the object of
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

    QPatternist::AutoPtr<QIODevice> result;

    try
    {
        result.reset(QPatternist::AccelTreeResourceLoader::load(canonicalURI, d->m_networkAccessDelegator,
                                                                d->staticContext()));
    }
    catch(const QPatternist::Exception)
    {
        /* We do nothing, result will be 0. */
    }

    if(result)
    {
        setQuery(result.data(), d->queryURI);
        result->close();
    }
    else
        d->recompileRequired();
}

/*!
  Binds the variable \a name to the \a value so that $\a name can be
  used from within the query to refer to the \a value.

  \a name must not be \e null. \a {name}.isNull() must return false.
  If \a name has already been bound by a previous bindVariable() call,
  its previous binding will be overridden.

  If \a {value} is null so that \a {value}.isNull() returns true, and
  \a {name} already has a binding, the effect is to remove the
  existing binding for \a {name}.

  To bind a value of type QString or QUrl, wrap the value in a
  QVariant such that QXmlItem's QVariant constructor is called.

  All strings processed by the query must be valid XQuery strings,
  which means they must contain only XML 1.0 characters. However,
  this requirement is not checked. If the query processes an invalid
  string, the behavior is undefined.

  \sa QVariant::isValid(), {QtXDM}{How QVariant maps to XQuery's Data Model},
   QXmlItem::isNull()
 */
void QXmlQuery::bindVariable(const QXmlName &name, const QXmlItem &value)
{
    if(name.isNull())
    {
        qWarning("The variable name cannot be null.");
        return;
    }

    const QPatternist::VariableLoader::Ptr vl(d->variableLoader());
    const QVariant variant(qVariantFromValue(value));

    /* If the type of the variable changed(as opposed to only the value),
     * we will have to recompile. */
    if(vl->invalidationRequired(name, variant) || value.isNull())
        d->recompileRequired();

    vl->addBinding(name, variant);
}

/*!
  \overload

  This function constructs a QXmlName from \a localName using the
  query's \l {QXmlNamePool} {namespace}. The function then behaves as
  the overloaded function. It is equivalent to the following snippet.

  \snippet doc/src/snippets/code/src_xmlpatterns_api_qxmlquery.cpp 0
 */
void QXmlQuery::bindVariable(const QString &localName, const QXmlItem &value)
{
    bindVariable(QXmlName(d->namePool, localName), value);
}

/*!
  Binds the variable \a name to the \a device so that $\a name can be
  used from within the query to refer to the \a device. The QIODevice
  \a device is exposed to the query as a URI of type \c{xs:anyURI},
  which can be passed to the \c{fn:doc()} function to be read. E.g.,
  this function can be used to pass an XML document in memory to
  \c{fn:doc}.

  \snippet doc/src/snippets/code/src_xmlpatterns_api_qxmlquery.cpp 1

  The caller must ensure that \a device has been opened with at least
  QIODevice::ReadOnly prior to this binding. Otherwise, behavior is
  undefined.

  If the query will access an XML document contained in a QString, use
  a QBuffer as shown in the following snippet. Suppose \e myQString
  contains \c{<document>content</document>}

  \snippet doc/src/snippets/qxmlquery/bindingExample.cpp 0

  \a name must not be \e null. \a {name}.isNull() must return false.
  If \a name has already been bound, its previous binding will be
  overridden. The URI that \a name evaluates to is arbitrary and may
  change.

  If the type of the variable binding changes (e.g., if a previous
  binding by the same name was a QVariant, or if there was no previous
  binding), isValid() will return \c{false}, and recompilation of the
  query text is required. To recompile the query, call setQuery(). For
  this reason, bindVariable() should be called before setQuery(), if
  possible.

  \note \a device must not be deleted while this QXmlQuery exists.
*/
void QXmlQuery::bindVariable(const QXmlName &name, QIODevice *device)
{
    if(device && !device->isReadable())
    {
        qWarning("A null, or readable QIODevice must be passed.");
        return;
    }

    if(name.isNull())
    {
        qWarning("The variable name cannot be null.");
        return;
    }

    const QPatternist::VariableLoader::Ptr vl(d->variableLoader());

    if(device)
    {
        const QVariant variant(qVariantFromValue(device));

        if(vl->invalidationRequired(name, variant))
            d->recompileRequired();

        vl->addBinding(name, variant);

        /* We need to tell the resource loader to discard its document, because
         * the underlying QIODevice has changed, but the variable name is the
         * same which means that the URI is the same, and hence the resource
         * loader will return the document for the old QIODevice.
         */
        d->resourceLoader()->clear(QUrl(QLatin1String("tag:trolltech.com,2007:QtXmlPatterns:QIODeviceVariable:") + d->namePool.d->stringForLocalName(name.localName())));
    }
    else
    {
        vl->removeBinding(name);
        d->recompileRequired();
    }
}

/*!
  \overload

  If \a localName is a valid \l {QXmlName::isNCName()} {NCName}, this
  function is equivalent to the following snippet.

  \snippet doc/src/snippets/code/src_xmlpatterns_api_qxmlquery.cpp 2

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
  messageHandler() and \c false is returned.

  If this query \l {isValid()} {is invalid}, \c{false} is returned
  and the behavior is undefined. If \a callback is null,
  behavior is undefined.

  \sa QAbstractXmlReceiver, isValid()
 */
bool QXmlQuery::evaluateTo(QAbstractXmlReceiver *callback) const
{
    if(!callback)
    {
        qWarning("A non-null callback must be passed.");
        return false;
    }

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
  \a target \l {QStringList} {string list}.

  If the query \l {isValid()} {is valid} and the evaluation succeeds,
  true is returned. Otherwise, false is returned and the contents of
  \a target are undefined.

  The query must evaluate to a sequence of \c{xs:string} values. If
  the query does not evaluate to a sequence of strings, the values can
  often be converted by adding a call to \c{string()} at the end of
  the XQuery.

  If \a target is null, the behavior is undefined.
 */
bool QXmlQuery::evaluateTo(QStringList *target) const
{
    if(!target)
    {
        qWarning("A non-null callback must be passed.");
        return false;
    }

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
  Evaluates the query or stylesheet, and writes the output to \a target.

  QXmlSerializer is used to write the output to \a target. In a future
  release, it is expected that this function will be changed to
  respect serialization options set in the stylesheet.

  If an error occurs during the evaluation, error messages are sent to
  messageHandler() and \c false is returned.

  If \a target is \c null, or is not opened in at least
  QIODevice::WriteOnly mode, the behavior is undefined.  QXmlQuery
  does not take ownership of \a target.

  \since 4.5
  \overload
 */
bool QXmlQuery::evaluateTo(QIODevice *target) const
{
    if(!target)
    {
        qWarning("The pointer to the device cannot be null.");
        return false;
    }

    if(!target->isWritable())
    {
        qWarning("The device must be writable.");
        return false;
    }

    QXmlSerializer serializer(*this, target);
    return evaluateTo(&serializer);
}

/*!
  Starts the evaluation and makes it available in \a result.  If \a
  result is null, the behavior is undefined. The evaluation takes
  place incrementally (lazy evaluation), as the caller uses
  QXmlResultItems::next() to get the next result.

  \sa QXmlResultItems::next()
*/
void QXmlQuery::evaluateTo(QXmlResultItems *result) const
{
    if(!result)
    {
        qWarning("A null pointer cannot be passed.");
        return;
    }

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
  Evaluates the query, and serializes the output as XML to \a output.

  If an error occurs during the evaluation, error messages are sent to
  messageHandler(), the content of \a output is undefined and \c false is
  returned, otherwise \c true is returned.

  If \a output is \c null behavior is undefined. QXmlQuery
  does not take ownership of \a output.

  Internally, the class QXmlFormatter is used for this.
 \since 4.5
 */
bool QXmlQuery::evaluateTo(QString *output) const
{
    Q_ASSERT_X(output, Q_FUNC_INFO,
               "The input cannot be null");

    QBuffer outputDevice;
    outputDevice.open(QIODevice::ReadWrite);

    QXmlFormatter formatter(*this, &outputDevice);
    const bool success = evaluateTo(&formatter);

    outputDevice.close();
    *output = QString::fromUtf8(outputDevice.data().constData());

    return success;
}

/*!
  Returns true if this query is valid. Examples of invalid queries
  are ones that contain syntax errors or that have not had setQuery()
  called for them yet.
 */
bool QXmlQuery::isValid() const
{
    return d->isValid();
}

/*!
  Sets the URI resolver to \a resolver. QXmlQuery does not take
  ownership of \a resolver.

  \sa uriResolver()
 */
void QXmlQuery::setUriResolver(const QAbstractUriResolver *resolver)
{
    d->uriResolver = resolver;
}

/*!
  Returns the query's URI resolver. If no URI resolver has been set,
  QtXmlPatterns will use the URIs in queries as they are.

  The URI resolver provides a level of abstraction, or \e{polymorphic
  URIs}. A resolver can rewrite \e{logical} URIs to physical ones, or
  it can translate obsolete or invalid URIs to valid ones.

  QtXmlPatterns calls the URI resolver for all URIs it encounters,
  except for namespaces. Specifically, all builtin functions that deal
  with URIs (\c{fn:doc()}, and \c{fn:doc-available()}).

  In the case of \c{fn:doc()}, the absolute URI is the base URI in the
  static context (which most likely is the location of the query).
  Rather than use the URI the user specified, the return value of
  QAbstractUriResolver::resolve() will be used.

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
  Sets the focus to \a item. The focus is the set of items that the
  context item expression and path expressions navigate from. For
  example, in the expression \e p/span, the element that \e p
  evaluates to is the focus for the following expression, \e span.

  The focus can be accessed using the context item expression, i.e.,
  dot (".").

  By default, the focus is not set and is undefined. It will
  therefore result in a dynamic error, \c XPDY0002, if the focus
  is attempted to be accessed. The focus must be set before the
  query is set with setQuery().

  There is no behavior defined for setting an item which is null.

 */
void QXmlQuery::setFocus(const QXmlItem &item)
{
    d->contextItem = item;
}

/**
 * This function should be a private member function of QXmlQuery,
 * but we don't dare that due to our weird compilers. */
template<typename TInputType>
static bool setFocusHelper(QXmlQuery *const queryInstance,
                           const TInputType &focusValue)
{
    /* We call resourceLoader(), so we have ensured that we have a resourceLoader
     * that we will share in our copy. */
    queryInstance->d->resourceLoader();

    QXmlQuery focusQuery(*queryInstance);

    /* The copy constructor doesn't allow us to copy an existing QXmlQuery and
     * changing the language at the same time so we need to use private API. */
    focusQuery.d->queryLanguage = QXmlQuery::XQuery10;

    Q_ASSERT(focusQuery.queryLanguage() == QXmlQuery::XQuery10);
    focusQuery.bindVariable(QChar::fromLatin1('u'), focusValue);
    focusQuery.setQuery(QLatin1String("doc($u)"));
    Q_ASSERT(focusQuery.isValid());

    QXmlResultItems focusResult;

    focusQuery.evaluateTo(&focusResult);
    const QXmlItem focusItem(focusResult.next());

    if(focusItem.isNull() || focusResult.hasError())
        return false;
    else
    {
        queryInstance->setFocus(focusItem);
        return true;
    }
}

/*!
  \since 4.5
  \overload

  Sets the focus to be the document located at \a documentURI and
  returns true. If \a documentURI cannot be loaded, false is returned.
  It is undefined at what time the document may be loaded. When
  loading the document, the message handler and URI resolver set on
  this QXmlQuery are used.

  If \a documentURI is empty or is not a valid URI, the behavior of
  this function is undefined.
 */
bool QXmlQuery::setFocus(const QUrl &documentURI)
{
    Q_ASSERT_X(documentURI.isValid() && !documentURI.isEmpty(),
               Q_FUNC_INFO,
               "The URI passed must be valid.");

    return setFocusHelper(this, QVariant(documentURI));
}

/*!

  Sets the focus to be the \a document read from the QIODevice and
  returns true. If \a document cannot be loaded, false is returned.

  QXmlQuery does not take ownership of \a document. The user
  guarantees that a document is available from the \a document device
  and that the document is not empty. The device must be opened in at
  least read-only mode. \a document must stay in scope as long as the
  current query is active.

 \since 4.5
 \overload
 */
bool QXmlQuery::setFocus(QIODevice *document)
{
    if(!document)
    {
        qWarning("A null QIODevice pointer cannot be passed.");
        return false;
    }

    if(!document->isReadable())
    {
        qWarning("The device must be readable.");
        return false;
    }

    return setFocusHelper(this, document);
}

/*!
  Returns a value indicating what this QXmlQuery is being used for.
  The default is QXmlQuery::XQuery10, which means the QXmlQuery is
  being used for running XQuery and XPath queries. QXmlQuery::XSLT20
  can also be returned, which indicates the QXmlQuery is for running
  XSL-T spreadsheets.

 \since 4.5
 */
QXmlQuery::QueryLanguage QXmlQuery::queryLanguage() const
{
    return d->queryLanguage;
}

/*!
  Sets the \a name of the initial template. The initial template is
  the one the processor calls first, instead of attempting to match a
  template to the context node (if any). If an initial template is not
  set, the standard order of template invocation will be used.

  This function only applies when using QXmlQuery to process XSL-T
  stylesheets. The name becomes part of the compiled stylesheet.
  Therefore, this function must be called before calling setQuery().

  If the stylesheet has no template named \a name, the processor will
  use the standard order of template invocation.

  \since 4.5
  \sa initialTemplateName()
 */
void QXmlQuery::setInitialTemplateName(const QXmlName &name)
{
    d->initialTemplateName = name;
}

/*!
  \overload

  Sets the name of the initial template to \a localName, which must be
  a valid \l{QXmlName::localName()} {local name}. The initial template
  is the one the processor calls first, instead of attempting to match
  a template to the context node (if any). If an initial template is
  not set, the standard order of template invocation will be used.

  This function only applies when using QXmlQuery to process XSL-T
  stylesheets. The name becomes part of the compiled stylesheet.
  Therefore, this function must be called before calling setQuery().

  If \a localName is not a valid \l{QXmlName::localName()} {local
  name}, the effect is undefined. If the stylesheet has no template
  named \a localName, the processor will use the standard order of
  template invocation.

  \since 4.5
  \sa initialTemplateName()
 */
void QXmlQuery::setInitialTemplateName(const QString &localName)
{
    Q_ASSERT_X(QXmlName::isNCName(localName),
               Q_FUNC_INFO,
               "The name passed must be a valid NCName.");
    setInitialTemplateName(QXmlName(d->namePool, localName));
}

/*!
  Returns the name of the XSL-T stylesheet template that the processor
  will call first when running an XSL-T stylesheet. This function only
  applies when using QXmlQuery to process XSL-T stylesheets. By
  default, no initial template is set. In that case, a default
  constructed QXmlName is returned.

  \since 4.5
 */
QXmlName QXmlQuery::initialTemplateName() const
{
    return d->initialTemplateName;
}

/*!
  Sets the network manager to \a newManager.
  QXmlQuery does not take ownership of \a newManager.

  \sa networkAccessManager()
  \since 4.5
 */
void QXmlQuery::setNetworkAccessManager(QNetworkAccessManager *newManager)
{
    d->m_networkAccessDelegator->m_genericManager = newManager;
}

/*!
  Returns the network manager, or 0 if it has not been set.

  \sa setNetworkAccessManager()
  \since 4.5
 */
QNetworkAccessManager *QXmlQuery::networkAccessManager() const
{
    return d->m_networkAccessDelegator->m_genericManager;
}

/*!
  Binds the result of the query \a query, to a variable by name \a name.

  Evaluation of \a query will be commenced when this function is called.

  If \a query is invalid, behavior is undefined. \a query will be copied.

  \since 4.5
  \sa isValid()
 */
void QXmlQuery::bindVariable(const QXmlName &name, const QXmlQuery &query)
{
    Q_ASSERT_X(query.isValid(), Q_FUNC_INFO, "The query being bound must be valid.");

    const QPatternist::VariableLoader::Ptr vl(d->variableLoader());
    const QVariant variant(qVariantFromValue(query));

    if(vl->invalidationRequired(name, variant))
        d->recompileRequired();

    vl->addBinding(name, variant);
}

/*!
 \overload

 Has the same behavior and effects as the function being overloaded, but takes
 the variable name \a localName as a QString. \a query is used as in the
 overloaded function.

  \since 4.5
 */
void QXmlQuery::bindVariable(const QString &localName, const QXmlQuery &query)
{
    return bindVariable(QXmlName(d->namePool, localName), query);
}

QT_END_NAMESPACE
