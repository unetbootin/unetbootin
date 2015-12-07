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

#include <QtDebug>

#include "qxmlformatter.h"
#include "qxpathhelper_p.h"
#include "qxmlserializer_p.h"

QT_BEGIN_NAMESPACE

using namespace QPatternist;

class QXmlFormatterPrivate : public QXmlSerializerPrivate
{
public:
    inline QXmlFormatterPrivate(const QXmlQuery &q,
                                QIODevice *const outputDevice);

    int             indentationDepth;
    int             currentDepth;
    QString         characterBuffer;
    QString         indentString;

    /**
     * Whether we /have/ sent nodes like processing instructions and comments
     * to QXmlSerializer.
     */
    QStack<bool>    canIndent;
};

QXmlFormatterPrivate::QXmlFormatterPrivate(const QXmlQuery &query,
                                           QIODevice *const outputDevice) : QXmlSerializerPrivate(query, outputDevice)
                                                                          , indentationDepth(4)
                                                                          , currentDepth(0)
{
    indentString.reserve(30);
    indentString.resize(1);
    indentString[0] = QLatin1Char('\n');
    canIndent.push(false);
}

/*!
   \class QXmlFormatter
   \brief The QXmlFormatter class receives an XQuery Sequence and translates it into human-readable XML.
   \reentrant
   \since 4.4
   \ingroup xml-tools

   QXmlFormatter is a subclass of QXmlSerializer that formats the XML
   output to make it easier for humans to read.

   QXmlSerializer outputs XML without adding unnecessary whitespace.
   In particular, it does not add \e {newlines} and indentation.
   To make the XML output easier to read, QXmlFormatter adds \e{newlines}
   and indentation by adding, removing, and modifying
   \l{XQuery Sequence}{sequence nodes} that only consist of whitespace.
   It also modifies whitespace in other places where it is not
   significant; e.g., between attributes and in the document prologue.

   For example, where the base class QXmlSerializer would
   output this:

   \quotefile doc/src/snippets/patternist/notIndented.xml

   QXmlFormatter outputs this:

   \quotefile doc/src/snippets/patternist/indented.xml

   If you just want to serialize your XML in a human-readable
   format, use QXmlFormatter as it is. The default indentation
   level is 4 spaces, but you can set your own indentation value
   setIndentationDepth().

   The \e{newlines} and indentation added by QXmlFormatter are
   suitable for common formats, such as XHTML, SVG, or Docbook,
   where whitespace is not significant. However, if your XML will
   be used as input where whitespace is significant, then you must
   write your own subclass of QXmlSerializer or QAbstractXmlReceiver.

   Note that using QXmlFormatter instead of QXmlSerializer will
   increase computational overhead and document storage size due
   to the insertion of whitespace.

   Note also that the indentation style used by QXmlFormatter
   remains loosely defined and may change in future versions of
   Qt. If a specific indentation style is required then either
   use the base class QXmlSerializer directly, or write your own
   subclass of QXmlSerializer or QAbstractXmlReceiver.
   Alternatively, you can subclass QXmlFormatter and reimplement
   the callbacks there.

   \snippet doc/src/snippets/code/src.xmlpatterns.api.qxmlformatter.cpp 0
*/

/*!
  Constructs a formatter that uses the name pool and message
  handler in \a query, and writes the result to \a outputDevice.

  \a outputDevice is passed directly to QXmlSerializer's constructor,
  and the same semantics and expectations applies, as documented
  in QXmlSerializer's constructor.
 */
QXmlFormatter::QXmlFormatter(const QXmlQuery &query,
                             QIODevice *outputDevice) : QXmlSerializer(new QXmlFormatterPrivate(query, outputDevice))
{
}

/*!
  \internal
 */
void QXmlFormatter::startFormattingContent()
{
    Q_D(QXmlFormatter);
    if(d->canIndent.top() && QPatternist::XPathHelper::isWhitespaceOnly(d->characterBuffer))
    {
        /* Do the hula hula dance. */
        QXmlSerializer::characters(QStringRef(&d->indentString));
    }
    else if(!d->characterBuffer.isEmpty())
    {
        /* Ao ao. Significant data, we don't touch it. */
        QXmlSerializer::characters(QStringRef(&d->characterBuffer));
    }

    d->characterBuffer.clear();
}

/*!
  \reimp
 */
void QXmlFormatter::startElement(const QXmlName &name)
{
    Q_D(QXmlFormatter);
    startFormattingContent();
    ++d->currentDepth;
    d->indentString.append(QString(d->indentationDepth, QLatin1Char(' ')));
    d->canIndent.push(true);

    QXmlSerializer::startElement(name);
}

/*!
  \reimp
 */
void QXmlFormatter::endElement()
{
    Q_D(QXmlFormatter);
    --d->currentDepth;
    d->indentString.chop(d->indentationDepth);

    if(!d->hasClosedElement.top().second)
        d->canIndent.top() = false;

    startFormattingContent();

    d->canIndent.pop();
    d->canIndent.top() = true;
    QXmlSerializer::endElement();
}

/*!
  \reimp
 */
void QXmlFormatter::attribute(const QXmlName &name,
                              const QStringRef &value)
{
    QXmlSerializer::attribute(name, value);
}

/*!
 \reimp
 */
void QXmlFormatter::comment(const QString &value)
{
    Q_D(QXmlFormatter);
    startFormattingContent();
    QXmlSerializer::comment(value);
    d->canIndent.top() = true;
}

/*!
 \reimp
 */
void QXmlFormatter::characters(const QStringRef &value)
{
    Q_D(QXmlFormatter);
    d->isPreviousAtomic = false;
    d->characterBuffer += value.toString();
}

/*!
 \reimp
 */
void QXmlFormatter::processingInstruction(const QXmlName &name,
                                          const QString &value)
{
    Q_D(QXmlFormatter);
    startFormattingContent();
    QXmlSerializer::processingInstruction(name, value);
    d->canIndent.top() = true;
}

/*!
 \reimp
 */
void QXmlFormatter::atomicValue(const QVariant &value)
{
    Q_D(QXmlFormatter);
    d->canIndent.top() = false;
    QXmlSerializer::atomicValue(value);
}

/*!
 \reimp
 */
void QXmlFormatter::startDocument()
{
    QXmlSerializer::startDocument();
}

/*!
 \reimp
 */
void QXmlFormatter::endDocument()
{
    QXmlSerializer::endDocument();
}

/*!
 \reimp
 */
void QXmlFormatter::startOfSequence()
{
    QXmlSerializer::startOfSequence();
}

/*!
 \reimp
 */
void QXmlFormatter::endOfSequence()
{
    Q_D(QXmlFormatter);
    d->write('\n');
    QXmlSerializer::endOfSequence();
}

/*!
 \internal
 */
void QXmlFormatter::item(const QPatternist::Item &item)
{
    Q_D(QXmlFormatter);

    if(item.isAtomicValue())
    {
        if(QPatternist::XPathHelper::isWhitespaceOnly(item.stringValue()))
           return;
        else
        {
            d->canIndent.top() = false;
            startFormattingContent();
        }
    }

    QXmlSerializer::item(item);
}

/*!
  Returns the amount of spaces QXmlFormatter will output for each
  indentation level.

  The default is four.

 \sa setIndentationDepth()
 */
int QXmlFormatter::indentationDepth() const
{
    Q_D(const QXmlFormatter);
    return d->indentationDepth;
}

/*!
  Sets the amount of spaces QXmlFormatter writes out for each
  indentation level to \a depth.

 \sa indentationDepth()
 */
void QXmlFormatter::setIndentationDepth(int depth)
{
    Q_D(QXmlFormatter);
    d->indentationDepth = depth;
}

QT_END_NAMESPACE
