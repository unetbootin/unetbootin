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

/**
 * @file
 * @short This file is included by BuiltintNodeType.h.
 * If you need includes in this file, put them in BuiltintNodeType.h, outside of the namespace.
 */

template <const QXmlNodeModelIndex::NodeKind kind>
BuiltinNodeType<kind>::BuiltinNodeType()
{
}

template <const QXmlNodeModelIndex::NodeKind kind>
bool BuiltinNodeType<kind>::xdtTypeMatches(const ItemType::Ptr &other) const
{
    if(!other->isNodeType())
        return false;

    return *static_cast<const BuiltinNodeType *>(other.data()) == *this
            ? true
            : xdtTypeMatches(other->xdtSuperType());
}

template <const QXmlNodeModelIndex::NodeKind kind>
bool BuiltinNodeType<kind>::itemMatches(const Item &item) const
{
    Q_ASSERT(item);

    return item.isNode() &&
           item.asNode().kind() == kind;
}

template <const QXmlNodeModelIndex::NodeKind kind>
ItemType::Ptr BuiltinNodeType<kind>::atomizedType() const
{
    switch(kind)
    {
        /* Fallthrough all these. */
        case QXmlNodeModelIndex::Attribute:
        case QXmlNodeModelIndex::Document:
        case QXmlNodeModelIndex::Element:
        case QXmlNodeModelIndex::Text:
            return BuiltinTypes::xsUntypedAtomic;
        case QXmlNodeModelIndex::ProcessingInstruction:
        /* Fallthrough. */
        case QXmlNodeModelIndex::Comment:
            return BuiltinTypes::xsString;
        default:
        {
            Q_ASSERT_X(false, Q_FUNC_INFO,
                       "Encountered invalid XPath Data Model node type.");
            return BuiltinTypes::xsUntypedAtomic;
        }
    }
}

template <const QXmlNodeModelIndex::NodeKind kind>
QString BuiltinNodeType<kind>::displayName(const NamePool::Ptr &) const
{
    switch(kind)
    {
        case QXmlNodeModelIndex::Element:
            return QLatin1String("element()");
        case QXmlNodeModelIndex::Document:
            return QLatin1String("document()");
        case QXmlNodeModelIndex::Attribute:
            return QLatin1String("attribute()");
        case QXmlNodeModelIndex::Text:
            return QLatin1String("text()");
        case QXmlNodeModelIndex::ProcessingInstruction:
            return QLatin1String("processing-instruction()");
        case QXmlNodeModelIndex::Comment:
            return QLatin1String("comment()");
        default:
        {
            Q_ASSERT_X(false, Q_FUNC_INFO,
                       "Encountered invalid XPath Data Model node type.");
            return QString();
        }
    }
}

template <const QXmlNodeModelIndex::NodeKind kind>
ItemType::Ptr BuiltinNodeType<kind>::xdtSuperType() const
{
    return BuiltinTypes::node;
}

template <const QXmlNodeModelIndex::NodeKind kind>
QXmlNodeModelIndex::NodeKind BuiltinNodeType<kind>::nodeKind() const
{
    return kind;
}


