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
   \brief The QXmlFormatter class is an implementation of QXmlSerializer for transforming XQuery output into formatted XML.
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

   \snippet doc/src/snippets/code/src_xmlpatterns_api_qxmlformatter.cpp 0
*/

/*!
  Constructs a formatter that uses the name pool and message
  handler in \a query, and writes the result to \a outputDevice
  as formatted XML.

  \a outputDevice is passed directly to QXmlSerializer's constructor.

  \sa QXmlSerializer
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

    if(QPatternist::XPathHelper::isWhitespaceOnly(d->characterBuffer))
    {
        if(d->canIndent.top())
            QXmlSerializer::characters(QStringRef(&d->indentString));
    }
    else
    {
        if(!d->characterBuffer.isEmpty()) /* Significant data, we don't touch it. */
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

    /* Flush any buffered content. */
    if(!d->characterBuffer.isEmpty())
        QXmlSerializer::characters(QStringRef(&d->characterBuffer));

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
  Returns the number of spaces QXmlFormatter will output for each
  indentation level. The default is four.

 \sa setIndentationDepth()
 */
int QXmlFormatter::indentationDepth() const
{
    Q_D(const QXmlFormatter);
    return d->indentationDepth;
}

/*!
  Sets \a depth to be the number of spaces QXmlFormatter will
  output for level of indentation. The default is four.

 \sa indentationDepth()
 */
void QXmlFormatter::setIndentationDepth(int depth)
{
    Q_D(QXmlFormatter);
    d->indentationDepth = depth;
}

QT_END_NAMESPACE
