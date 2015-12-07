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

#include "qnamepool_p.h"
#include "qxmlnamepool.h"

QT_BEGIN_NAMESPACE

/*!
  \class QXmlNamePool
  \brief The QXmlNamePool class is a pool of XML names.
  \reentrant
  \since 4.4
  \ingroup xml-tools

  QXmlNamePool is used to optimize operations on instances of
  QXmlName. An instance of QXmlName represents an XML name in
  a way that allows the XML name to be compared and passed around
  efficiently. The efficiency is achieved by storing the strings
  comprising the XML names in an instance of QXmlNamePool, where
  they are mapped to binary identifiers, which are then packed
  into a key which is stored in the QXmlName.

  This means that each instance of QXmlName is tied to the name
  pool it was created with, and that name pool should be kept in
  scope and used to create all instances of QXmlName that might
  be compared. Note also that the name pool is required if you
  must reconstitute the QXmlName as text, or if you must access
  any of its component strings, so although instances of
  QXmlName can be compared without reference to a name pool, the
  name pool must be kept in scope if the name's strings must be
  accessed later.

  \sa QXmlName
  \sa QXmlQuery::namePool()
 */

/*!
  Constructs an empty name pool.
 */
QXmlNamePool::QXmlNamePool() : d(new QPatternist::NamePool())
{
}

/*!
  Constructs a copy of the \a other name pool.
 */
QXmlNamePool::QXmlNamePool(const QXmlNamePool &other) : d(other.d)
{
}

/*!
  Destroys the name pool. Instances of QXmlName constructed
  with this name pool can still be compared after this destructor
  is called, but their text strings cannot be accessed.
 */
QXmlNamePool::~QXmlNamePool()
{
}

/*!
  Assigns the \a other name pool to this one.
 */
QXmlNamePool &QXmlNamePool::operator=(const QXmlNamePool &other)
{
    d = other.d;
    return *this;
}

QT_END_NAMESPACE

