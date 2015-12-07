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

#include "qxmlresultitems.h"
#include "qxmlresultitems_p.h"
#include "qitem_p.h"

QT_BEGIN_NAMESPACE

/*!
  \class QXmlResultItems
  \brief The QXmlResultItems class iterates through the results of evaluating a query.
  \reentrant
  \since 4.4
  \ingroup xml-tools

  QXmlResultItems presents the evaluation of an associated query as a
  sequence of \l{QXmlItem}{QXmlItems}. The sequence is traversed by
  repeatedly calling next(), which actually produces the sequence by
  lazy evaluation of the query.

  \snippet doc/src/snippets/code/src.xmlpatterns.api.qxmlresultitems.cpp 0

  An effect of letting next() produce the sequence by lazy evaluation
  is that a query error can occur on any call to next(). If an error
  occurs, both next() and current() will return the null QXmlItem, and
  hasError() will return true.

  QXmlResultItems can be thought of as an "iterator" that traverses
  the sequence of query results once, in the forward direction. Each
  call to next() advances the iterator to the next QXmlItem in the
  sequence and returns it, and current() always returns the QXmlItem
  that next() returned the last time it was called.

  \sa QXmlItem::isNode(), QXmlItem::isAtomicValue(), QXmlNodeModelIndex
 */

/*!
  Constructs an instance of QXmlResultItems.
 */
QXmlResultItems::QXmlResultItems() : d_ptr(new QXmlResultItemsPrivate())
{
}

/*!
  Destroys this instance of QXmlResultItems.
 */
QXmlResultItems::~QXmlResultItems()
{
    delete d_ptr;
}

/*!
  Returns the next result in the sequence produced by lazy evaluation
  of the associated query. When the returned QXmlItem is null, either
  the evaluation terminated normally without producing another result,
  or an error occurred. Call hasError() to determine whether the null
  item was caused by normal termination or by an error.

  Returns a null QXmlItem if there is no associated QXmlQuery.
 */
QXmlItem QXmlResultItems::next()
{
    Q_D(QXmlResultItems);
    if(d->hasError)
        return QXmlItem();

    try
    {
        d->current = QPatternist::Item::toPublic(d->iterator->next());
        return d->current;
    }
    catch(const QPatternist::Exception)
    {
        d->current = QXmlItem();
        d->hasError = true;
        return QXmlItem();
    }
}

/*!
  Returns the current item. The current item is the last item
  that was produced and returned by next().

  Returns a null QXmlItem if there is no associated QXmlQuery.
 */
QXmlItem QXmlResultItems::current() const
{
    Q_D(const QXmlResultItems);

    if(d->hasError)
        return QXmlItem();
    else
        return d->current;
}

/*!

  If an error occurred during evaluation of the query, true is
  returned.

  Returns false if query evaluation has been done.
 */
bool QXmlResultItems::hasError() const
{
    Q_D(const QXmlResultItems);
    return d->hasError;
}

QT_END_NAMESPACE

