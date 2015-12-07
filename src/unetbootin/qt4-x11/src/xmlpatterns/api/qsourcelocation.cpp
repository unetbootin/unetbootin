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

#include "qdebug_p.h"

#include "qsourcelocation.h"


QT_BEGIN_NAMESPACE

/*!
  \class QSourceLocation
  \reentrant
  \since 4.4
  \brief The QSourceLocation class identifies a location in a resource by URI, line, and column.
  \ingroup xml-tools

  QSourceLocation is a simple value based class that has three
  properties, uri(), line(), and column(), that, taken together,
  identify a certain point in a resource, e.g., a file or an in-memory
  document.

  line() and column() refer to character counts (not byte counts), and
  they both start from 1, as opposed to 0.
 */

/*!
   Construct a QSourceLocation that doesn't identify anything at all.

   For a default constructed QSourceLocation(), isNull() returns \c true.
 */
QSourceLocation::QSourceLocation() : m_line(-1), m_column(-1)
{
}

/*!
  Constructs a QSourceLocation that is a copy of \a other.
 */
QSourceLocation::QSourceLocation(const QSourceLocation &other)
  : m_line(other.m_line), m_column(other.m_column), m_uri(other.m_uri)
{
}

/*!
 Constructs a QSourceLocation with URI \a u, line \a l and column \a c.
 */
QSourceLocation::QSourceLocation(const QUrl &u, int l, int c)
  : m_line(l), m_column(c), m_uri(u)
{
}

/*!
  Destructor.
 */
QSourceLocation::~QSourceLocation()
{
}

/*!
  Returns true if this QSourceLocation is identical to \a other.

  Two QSourceLocation instances are equal if their uri(), line() and
  column() are equal.

  QSourceLocation instances for which isNull() returns true are
  considered equal.
 */
bool QSourceLocation::operator==(const QSourceLocation &other) const
{
    return    m_line == other.m_line
           && m_column == other.m_column
           && m_uri == other.m_uri;
}

/*!
  Returns the opposite of applying operator==() for this QXmlName
  and \a other.
 */
bool QSourceLocation::operator!=(const QSourceLocation &other) const
{
    return operator==(other);
}

/*!
  Assigns this QSourceLocation instance to \a other.
 */
QSourceLocation &QSourceLocation::operator=(const QSourceLocation &other)
{
    if(this != &other)
    {
        m_line = other.m_line;
        m_column = other.m_column;
        m_uri = other.m_uri;
    }

    return *this;
}

/*!
  Returns the current column number. The column number refers to the
  count of characters, not bytes. The first column is column 1, not 0.
  The default value is -1, indicating the column number is unknown.
 */
qint64 QSourceLocation::column() const
{
    return m_column;
}

/*!
  Sets the column number to \a newColumn. 0 is an invalid column
  number. The first column number is 1.
 */
void QSourceLocation::setColumn(qint64 newColumn)
{
    Q_ASSERT_X(newColumn != 0, Q_FUNC_INFO,
               "0 is an invalid column number. The first column number is 1.");
    m_column = newColumn;
}

/*!
  Returns the current line number. The first line number is 1, not 0.
  The default value is -1, indicating the line number is unknown.
 */
qint64 QSourceLocation::line() const
{
    return m_line;
}

/*!
  Sets the line number to \a newLine. 0 is an invalid line
  number. The first line number is 1.
 */
void QSourceLocation::setLine(qint64 newLine)
{
    m_line = newLine;
}

/*!
  Returns the resource that this QSourceLocation refers to. For
  example, the resource could be a file in the local file system,
  if the URI scheme is \c file.
 */
QUrl QSourceLocation::uri() const
{
    return m_uri;
}

/*!
  Sets the URI to \a newUri.
 */
void QSourceLocation::setUri(const QUrl &newUri)
{
    m_uri = newUri;
}

/*!
  \relates QSourceLocation
  \since 4.4

  Prints \a sourceLocation to the debug stream \a debug.
 */
#ifndef QT_NO_DEBUG_STREAM
QDebug operator<<(QDebug debug, const QSourceLocation &sourceLocation)
{
    debug << "QSourceLocation("
          << sourceLocation.uri()
          << ", line:"
          << sourceLocation.line()
          << ", column:"
          << sourceLocation.column()
          << ")";
    return debug;
}
#endif

/*!
  Returns \c true if this QSourceLocation doesn't identify anything.

  For a default constructed QSourceLocation, this function returns \c
  true. The same applies for any other QSourceLocation whose uri() is
  invalid.
 */
bool QSourceLocation::isNull() const
{
    return !m_uri.isValid();
}

/*!
 \since 4.4

 Computes a hash key for the QSourceLocation \a location.

 \relates QSourceLocation
 */
uint qHash(const QSourceLocation &location)
{
    /* Not the world's best hash function exactly. */
    return qHash(location.uri().toString()) + location.line() + location.column();
}

QT_END_NAMESPACE

