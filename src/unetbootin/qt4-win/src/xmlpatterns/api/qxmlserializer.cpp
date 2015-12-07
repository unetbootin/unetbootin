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

#include "qdynamiccontext_p.h"
#include "qpatternistlocale_p.h"
#include "qitem_p.h"
#include "qxmlquery_p.h"
#include "qxmlserializer_p.h"
#include "qxmlserializer.h"

QT_BEGIN_NAMESPACE

using namespace QPatternist;

QXmlSerializerPrivate::QXmlSerializerPrivate(const QXmlQuery &query,
                                             QIODevice *outputDevice)
    : isPreviousAtomic(false),
      state(QXmlSerializer::BeforeDocumentElement),
      np(query.namePool().d),
      device(outputDevice),
      codec(QTextCodec::codecForMib(106)), /* UTF-8 */
      query(query)
{
    hasClosedElement.reserve(EstimatedTreeDepth);
    namespaces.reserve(EstimatedTreeDepth);
    nameCache.reserve(EstimatedNameCount);

    hasClosedElement.push(qMakePair(QXmlName(), true));

    /*
      We push the empty namespace such that first of all
      namespaceBinding() won't assert on an empty QStack,
      and such that the empty namespace is in-scope and
      that the code doesn't attempt to declare it.

      We push the XML namespace. Although we won't receive
      declarations for it, we may output attributes by that
      name.
    */
    QVector<QXmlName> defNss;
    defNss.resize(2);
    defNss[0] = QXmlName(StandardNamespaces::empty,
                         StandardLocalNames::empty,
                         StandardPrefixes::empty);
    defNss[1] = QXmlName(StandardNamespaces::xml,
                         StandardLocalNames::empty,
                         StandardPrefixes::xml);

    namespaces.push(defNss);

    /* If we don't set this flag, QTextCodec will generate a BOM. */
    converterState.flags = QTextCodec::IgnoreHeader;
}

/*!
  \class QXmlSerializer
  \brief The QXmlSerializer class receives an XQuery sequence and translates it into XML.

  \reentrant
  \since 4.4
  \ingroup xml-tools

  QXmlSerializer translates an \l {XQuery Sequence} {XQuery sequence}, usually
  the output of an QXmlQuery, into XML. Consider the example:

  \snippet doc/src/snippets/code/src.xmlpatterns.api.qxmlserializer.cpp 0

  First it constructs a \l {QXmlQuery} {query} that gets the
  first paragraph from document \c index.html. Then it constructs
  an instance of this class with the \l {QXmlQuery} {query} and
  \l {QIODevice} {myOutputDevice}. Finally, it
  \l {QXmlQuery::evaluateTo()} {evaluates} the
  \l {QXmlQuery} {query}, producing an ordered sequence of calls
  to the serializer's callback functions. The sequence of callbacks
  transforms the query output to XML and writes it to
  \l {QIODevice} {myOutputDevice}.

  QXmlSerializer will:

  \list
  \o Declare namespaces when needed,

  \o Use appropriate escaping, when characters can't be
  represented in the XML,

  \o Handle line endings appropriately,

  \o Report errors, when it can't serialize the content, e.g.,
  when asked to serialize an attribute that is a top-level node,
  or when more than one top-level element is encountered.

  \endlist

  If an error occurs during serialization, result is undefined
  unless the serializer is driven through a call to
  QXmlQuery::evaluateTo().

  If the generated XML should be indented and formatted for reading,
  use QXmlFormatter.

  \sa {http://www.w3.org/TR/xslt-xquery-serialization/}{XSLT 2.0 and XQuery 1.0 Serialization}

  \sa QXmlFormatter
 */

/*!
  Constructs a serializer that uses the name pool and message
  handler in \a query, and writes the output to \a outputDevice.

  \a outputDevice must be a valid, non-null device that is open in
  write mode, otherwise behavior is undefined.

  \a outputDevice must not be opened with QIODevice::Text because it
  will cause the output to be incorrect. This class will ensure line endings
  are serialized as according with the XML specification.

 QXmlSerializer does not own \a outputDevice.
 */
QXmlSerializer::QXmlSerializer(const QXmlQuery &query,
                               QIODevice *outputDevice) : QAbstractXmlReceiver(new QXmlSerializerPrivate(query, outputDevice))
{
    Q_ASSERT_X(outputDevice, Q_FUNC_INFO,"outputDevice cannot be null.");
    Q_ASSERT_X(outputDevice->isWritable(),
               Q_FUNC_INFO,
               "outputDevice must be opened in write mode.");
}

/*!
  \internal
 */
QXmlSerializer::QXmlSerializer(QAbstractXmlReceiverPrivate *d) : QAbstractXmlReceiver(d)
{
}

/*!
  \internal
 */
bool QXmlSerializer::atDocumentRoot() const
{
    Q_D(const QXmlSerializer);
    return d->state == BeforeDocumentElement ||
           d->state == InsideDocumentElement && d->hasClosedElement.size() == 1;
}

/*!
  \internal
 */
void QXmlSerializer::startContent()
{
    Q_D(QXmlSerializer);
    if (!d->hasClosedElement.top().second) {
        d->write('>');
        d->hasClosedElement.top().second = true;
    }
}

/*!
  \internal
 */
void QXmlSerializer::writeEscaped(const QString &toEscape)
{
    if(toEscape.isEmpty()) /* Early exit. */
        return;

    QString result;
    result.reserve(int(toEscape.length() * 1.1));
    const int length = toEscape.length();

    for(int i = 0; i < length; ++i)
    {
        const QChar c(toEscape.at(i));

        if(c == QLatin1Char('<'))
            result += QLatin1String("&lt;");
        else if(c == QLatin1Char('>'))
            result += QLatin1String("&gt;");
        else if(c == QLatin1Char('&'))
            result += QLatin1String("&amp;");
        else
            result += toEscape.at(i);
    }

    write(result);
}

/*!
  \internal
 */
void QXmlSerializer::writeEscapedAttribute(const QString &toEscape)
{
    if(toEscape.isEmpty()) /* Early exit. */
        return;

    QString result;
    result.reserve(int(toEscape.length() * 1.1));
    const int length = toEscape.length();

    for(int i = 0; i < length; ++i)
    {
        const QChar c(toEscape.at(i));

        if(c == QLatin1Char('<'))
            result += QLatin1String("&lt;");
        else if(c == QLatin1Char('>'))
            result += QLatin1String("&gt;");
        else if(c == QLatin1Char('&'))
            result += QLatin1String("&amp;");
        else if(c == QLatin1Char('"'))
            result += QLatin1String("&quot;");
        else
            result += toEscape.at(i);
    }

    write(result);
}

/*!
  \internal
 */
void QXmlSerializer::write(const QString &content)
{
    Q_D(QXmlSerializer);
    d->device->write(d->codec->fromUnicode(content.constData(), content.length(), &d->converterState));
}

/*!
  \internal
 */
void QXmlSerializer::write(const QXmlName &name)
{
    Q_D(QXmlSerializer);
    const QByteArray &cell = d->nameCache[name.code()];

    if(cell.isNull())
    {
        QByteArray &mutableCell = d->nameCache[name.code()];

        const QString content(d->np->toLexical(name));
        mutableCell = d->codec->fromUnicode(content.constData(),
                                            content.length(),
                                            &d->converterState);
        d->device->write(mutableCell);
    }
    else
        d->device->write(cell);
}

/*!
  \internal
 */
void QXmlSerializer::write(const char *const chars)
{
    Q_D(QXmlSerializer);
    d->device->write(chars);
}

/*!
  \reimp
 */
void QXmlSerializer::startElement(const QXmlName &name)
{
    Q_D(QXmlSerializer);
    Q_ASSERT(d->device);
    Q_ASSERT(d->device->isWritable());
    Q_ASSERT(d->codec);
    Q_ASSERT(!name.isNull());

    d->namespaces.push(QVector<QXmlName>());

    if(atDocumentRoot())
    {
        if(d->state == BeforeDocumentElement)
            d->state = InsideDocumentElement;
        else if(d->state != InsideDocumentElement)
        {
            d->query.d->staticContext()->error(QtXmlPatterns::tr(
               "Element %1 can't be serialized because it appears outside "
               "the document element.").arg(formatKeyword(d->np, name)),
                                               ReportContext::SENR0001,
                                               d->query.d->expression().data());
        }
    }

    startContent();
    d->write('<');
    write(name);

    /* Ensure that the namespace URI used in the name gets outputted. */
    namespaceBinding(name);

    d->hasClosedElement.push(qMakePair(name, false));
    d->isPreviousAtomic = false;
}

/*!
  \reimp
 */
void QXmlSerializer::endElement()
{
    Q_D(QXmlSerializer);
    const QPair<QXmlName, bool> e(d->hasClosedElement.pop());
    d->namespaces.pop();

    if(e.second)
    {
        write("</");
        write(e.first);
        d->write('>');
    }
    else
        write("/>");

    d->isPreviousAtomic = false;
}

/*!
  \reimp
 */
void QXmlSerializer::attribute(const QXmlName &name,
                               const QStringRef &value)
{
    Q_D(QXmlSerializer);
    Q_ASSERT(!name.isNull());

    /* Ensure that the namespace URI used in the name gets outputted. */
    {
        /* Since attributes doesn't pick up the default namespace, a
         * namespace declaration would cause trouble if we output it. */
        if(name.prefix() != StandardPrefixes::empty)
            namespaceBinding(name);
    }

    if(atDocumentRoot())
    {
        Q_UNUSED(d);
        d->query.d->staticContext()->error(QtXmlPatterns::tr(
           "Attribute element %1 can't be serialized because it appears at "
           "the top level.").arg(formatKeyword(d->np, name)),
                                           ReportContext::SENR0001,
                                           d->query.d->expression().data());
    }
    else
    {
        d->write(' ');
        write(name);
        write("=\"");
        writeEscapedAttribute(value.toString());
        d->write('"');
    }
}

/*!
  \internal
 */
bool QXmlSerializer::isBindingInScope(const QXmlName nb) const
{
    Q_D(const QXmlSerializer);
    const int levelLen = d->namespaces.size();

    if(nb.prefix() == StandardPrefixes::empty)
    {
        for(int lvl = levelLen - 1; lvl >= 0; --lvl)
        {
            const QVector<QXmlName> &scope = d->namespaces.at(lvl);
            const int vectorLen = scope.size();

            for(int s = vectorLen - 1; s >= 0; --s)
            {
                const QXmlName &nsb = scope.at(s);

                if(nsb.prefix() == StandardPrefixes::empty)
                    return nsb.namespaceURI() == nb.namespaceURI();
            }
        }
    }
    else
    {
        for(int lvl = 0; lvl < levelLen; ++lvl)
        {
            const QVector<QXmlName> &scope = d->namespaces.at(lvl);
            const int vectorLen = scope.size();

            for(int s = 0; s < vectorLen; ++s)
            {
                const QXmlName &n = scope.at(s);
                if (n.prefix() == nb.prefix() &&
                    n.namespaceURI() == nb.namespaceURI())
                    return true;
            }
        }
    }

    return false;
}

/*!
 \reimp
 */
void QXmlSerializer::namespaceBinding(const QXmlName &nb)
{
    /*
     * Writes out \a nb.
     *
     * Namespace bindings aren't looked up in a cache, because
     * we typically receive very few.
     */

    Q_D(QXmlSerializer);
    Q_ASSERT_X(!nb.isNull(), Q_FUNC_INFO,
               "It makes no sense to pass a null QXmlName.");

    Q_ASSERT_X((nb.namespaceURI() != StandardNamespaces::empty) ||
               (nb.prefix() == StandardPrefixes::empty),
               Q_FUNC_INFO,
               "Undeclarations of prefixes aren't allowed in XML 1.0 "
               "and aren't supposed to be received.");

    if(nb.namespaceURI() == QPatternist::StandardNamespaces::StopNamespaceInheritance)
        return;

    if(isBindingInScope(nb))
        return;

    d->namespaces.top().append(nb);

    if(nb.prefix() == StandardPrefixes::empty)
        write(" xmlns");
    else
    {
        write(" xmlns:");
        write(d->np->stringForPrefix(nb.prefix()));
    }

    write("=\"");
    writeEscapedAttribute(d->np->stringForNamespace(nb.namespaceURI()));
    d->write('"');
}

/*!
 \reimp
 */
void QXmlSerializer::comment(const QString &value)
{
    Q_D(QXmlSerializer);
    Q_ASSERT_X(!value.contains(QLatin1String("--")),
               Q_FUNC_INFO,
               "Invalid input; it's the caller's responsibility to ensure "
               "the input is correct.");

    startContent();
    write("<!--");
    write(value);
    write("-->");
    d->isPreviousAtomic = false;
}

/*!
 \reimp
 */
void QXmlSerializer::characters(const QStringRef &value)
{
    Q_D(QXmlSerializer);
    d->isPreviousAtomic = false;
    startContent();
    writeEscaped(value.toString());
}

/*!
 \reimp
 */
void QXmlSerializer::processingInstruction(const QXmlName &name,
                                           const QString &value)
{
    Q_D(QXmlSerializer);
    Q_ASSERT_X(!value.contains(QLatin1String("?>")),
               Q_FUNC_INFO,
               "Invalid input; it's the caller's responsibility to ensure "
               "the input is correct.");

    startContent();
    write("<?");
    write(name);
    d->write(' ');
    write(value);
    write("?>");

    d->isPreviousAtomic = false;
}

/*!
  \internal
 */
void QXmlSerializer::item(const QPatternist::Item &outputItem)
{
    Q_D(QXmlSerializer);

    if(outputItem.isAtomicValue())
    {
        if(d->isPreviousAtomic)
        {
            startContent();
            d->write(' ');
            writeEscaped(outputItem.stringValue());
        }
        else
        {
            d->isPreviousAtomic = true;
            const QString value(outputItem.stringValue());

            if(!value.isEmpty())
            {
                startContent();
                writeEscaped(value);
            }
        }
    }
    else
    {
        startContent();
        Q_ASSERT(outputItem.isNode());
        sendAsNode(outputItem);
    }
}

/*!
 \reimp
 */
void QXmlSerializer::atomicValue(const QVariant &value)
{
    Q_UNUSED(value);
}

/*!
 \reimp
 */
void QXmlSerializer::startDocument()
{
    Q_D(QXmlSerializer);
    d->isPreviousAtomic = false;
}

/*!
 \reimp
 */
void QXmlSerializer::endDocument()
{
    Q_D(QXmlSerializer);
    d->isPreviousAtomic = false;
}

/*!
 Returns a pointer to the output device. There is no corresponding
 function to \e set the output device, because the output device
 must be passed to the constructor. The serializer does not own
 the IO device returned.
 */
QIODevice *QXmlSerializer::outputDevice() const
{
    Q_D(const QXmlSerializer);
    return d->device;
}

/*!
  Sets the codec the serializer will use for encoding its XML output.
  The output codec is set to \a outputCodec. By default, the output
  codec is set to the one for \c UTF-8.

  The serializer does not take ownership of the codec.

  \sa codec()

 */
void QXmlSerializer::setCodec(const QTextCodec *outputCodec)
{
    Q_D(QXmlSerializer);
    d->codec = outputCodec;
}

/*!
  Returns the codec being used by the serializer for encoding its
  XML output.

  \sa setCodec()
 */
const QTextCodec *QXmlSerializer::codec() const
{
    Q_D(const QXmlSerializer);
    return d->codec;
}

/*!
 \reimp
 */
void QXmlSerializer::startOfSequence()
{
}

/*!
 \reimp
 */
void QXmlSerializer::endOfSequence()
{
    /* If this function is changed to flush or close or something like that,
     * take into consideration QXmlFormatter, especially
     * QXmlFormatter::endOfSequence(). */
}

QT_END_NAMESPACE
