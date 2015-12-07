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

#include <QUrl>
#include <QVector>
#include <QXmlNamePool>

#include "qabstractxmlnodemodel_p.h"
#include "qemptyiterator_p.h"
#include "qitemmappingiterator_p.h"
#include "qsequencemappingiterator_p.h"
#include "qsimplexmlnodemodel.h"
#include "qsingletoniterator_p.h"

QT_BEGIN_NAMESPACE

using namespace QPatternist;

class QSimpleXmlNodeModelPrivate : public QAbstractXmlNodeModelPrivate
{
public:
    QSimpleXmlNodeModelPrivate(const QXmlNamePool &np) : namePool(np)
    {
    }

    mutable QXmlNamePool namePool;
};

/*!
  \class QSimpleXmlNodeModel
  \brief The QSimpleXmlNodeModel class provides a simple yet powerful way of implementing QAbstractXmlNodeModel.
  \reentrant
  \since 4.4
  \ingroup xml-tools

  Since QAbstractXmlNodeModel::iterate() requires a sub-class of QAbstractXmlForwardIterator
  to be returned for each axis, it can be a bit time consuming to implement. For that
  reason QSimpleXmlNodeModel exists which has nextFromSimpleAxis(), that that is a quick
  but often sufficient way of providing the node navigation. Behind the scenes
  QSimpleXmlNodeModel::iterate() "emulates" the full XPath axes using nextFromSimpleAxis().

  Apart from nextFromSimpleAxis(), QSimpleXmlNodeModel provides default implementations
  for many of the members in QAbstractXmlNodeModel that are suitable for a wide range of scenarios.

  Sub-classes of QSimpleXmlNodeModel must be thread-safe, just like sub-classes
  other sub-classes of QAbstractXmlNodeModel.
 */

/*!
  Constructs a QSimpleXmlNodeModel instance that can be used with the name pool \a namePool. It can subsequently be accessed
  with namePool().
 */
QSimpleXmlNodeModel::QSimpleXmlNodeModel(const QXmlNamePool &namePool) : QAbstractXmlNodeModel(new QSimpleXmlNodeModelPrivate(namePool))
{
}

/*!
  Destructor.
 */
QSimpleXmlNodeModel::~QSimpleXmlNodeModel()
{
}

/*!
 If \a node is an element or attribute, typedValue() is called, and the return value
 converted to a string, as per XQuery's rules.

 If \a node is another type of node, the empty string is returned.

 If this function is overriden for for instance comments or processing instructions, it is important
 to remember to call this function for elements and attribute, if they have atomic values that are not
 of type \c xs:string, such that they get formatted according to XQuery.

 */
QString QSimpleXmlNodeModel::stringValue(const QXmlNodeModelIndex &node) const
{
    const QXmlNodeModelIndex::NodeKind k= kind(node);
    if(k == QXmlNodeModelIndex::Element || k == QXmlNodeModelIndex::Attribute)
    {
        const QVariant &candidate = typedValue(node);
        if(candidate.isNull())
            return QString();
        else
            return AtomicValue::toXDM(candidate).stringValue();
    }
    else
        return QString();
}

/*!
  Returns the base URI for \a node. This is always the document URI. This is safe, works and is often ok.

  \sa documentUri()
 */
QUrl QSimpleXmlNodeModel::baseUri(const QXmlNodeModelIndex &node) const
{
    return documentUri(node);
}

/*!
  Returns the name pool that is associated with this model. The implementation of name()
  would use this to create names.
 */
QXmlNamePool &QSimpleXmlNodeModel::namePool() const
{
    Q_D(const QSimpleXmlNodeModel);

    return d->namePool;
}

/*!
  Returns always an empty QVector. This signals that no namespace bindings are in scope for \a node.
 */
QVector<QXmlName> QSimpleXmlNodeModel::namespaceBindings(const QXmlNodeModelIndex &node) const
{
    Q_UNUSED(node);
    return QVector<QXmlName>();
}

/*!
  Always returns a default constructed QXmlNodeModelIndex instance, regardless of \a id.

  This effectively means the model has no elements that has an id.
 */
QXmlNodeModelIndex QSimpleXmlNodeModel::elementById(const QXmlName &id) const
{
    Q_UNUSED(id);
    return QXmlNodeModelIndex();
}

/*!
  Always returns an empty vector, regardless of \a idref.

  This effectively means the model has no elements or attributes of type \c IDREF.
 */
QVector<QXmlNodeModelIndex> QSimpleXmlNodeModel::nodesByIdref(const QXmlName &idref) const
{
    Q_UNUSED(idref);
    return QVector<QXmlNodeModelIndex>();
}

QT_END_NAMESPACE


