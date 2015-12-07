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

#include "qxmlresultitems.h"
#include "qxmlresultitems_p.h"
#include "qitem_p.h"

QT_BEGIN_NAMESPACE

/*!
  \class QXmlResultItems
  \brief The QXmlResultItems class iterates through the results of evaluating an XQuery in QXmlQuery.
  \reentrant
  \since 4.4
  \ingroup xml-tools

  QXmlResultItems presents the evaluation of an associated query as a
  sequence of \l{QXmlItem}{QXmlItems}. The sequence is traversed by
  repeatedly calling next(), which actually produces the sequence by
  lazy evaluation of the query.

  \snippet doc/src/snippets/code/src_xmlpatterns_api_qxmlresultitems.cpp 0

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

