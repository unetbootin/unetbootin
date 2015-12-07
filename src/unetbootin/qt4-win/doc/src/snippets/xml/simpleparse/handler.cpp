/****************************************************************************
**
** Copyright (C) 2004-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the documentation of the Qt Toolkit.
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

/*
handler.cpp

Provides a handler for processing XML elements found by the reader.

The handler stores the names of elements it finds and their indentation
levels. The indentation level is initially set to zero.
When a starting element is found, the indentation level is increased;
when an ending element is found, the indentation level is decreased.
*/

#include <QDebug>
#include "handler.h"

/*!
    Reset the state of the handler to ensure that new documents are
    read correctly.

    We return true to indicate that parsing should continue.
*/

bool Handler::startDocument()
{
    elementName.clear();
    elementIndentation.clear();
    indentationLevel = 0;

    return true;
}

/*!
    Process each starting element in the XML document.

    Append the element name to the list of elements found; add its
    corresponding indentation level to the list of indentation levels.

    Increase the level of indentation by one column.

    We return true to indicate that parsing should continue.
*/

bool Handler::startElement(const QString &, const QString &,
    const QString & qName, const QXmlAttributes &)
{
    elementName.append(qName);
    elementIndentation.append(indentationLevel);
    indentationLevel += 1;

    return true;
}

/*!
    Process each ending element in the XML document.

    Decrease the level of indentation by one column.

    We return true to indicate that parsing should continue.
*/

bool Handler::endElement(const QString &, const QString &,
    const QString &)
{
    indentationLevel -= 1;

    return true;
}

/*!
    Report a fatal parsing error, and return false to indicate to the reader
    that parsing should stop.
*/

bool Handler::fatalError (const QXmlParseException & exception)
{
    qWarning() << QString("Fatal error on line %1, column %2: %3").arg(
        exception.lineNumber()).arg(exception.columnNumber()).arg(
        exception.message());

    return false;
}

/*!
    Return the list of element names found.
*/

QStringList& Handler::names ()
{
    return elementName;
}

/*!
    Return the list of indentations used for each element found.
*/

QList<int>& Handler::indentations ()
{
    return elementIndentation;
}
