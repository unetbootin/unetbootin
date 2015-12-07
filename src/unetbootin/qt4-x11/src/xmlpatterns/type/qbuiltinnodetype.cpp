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

template <const QXmlNodeModelIndex::NodeKind kind>
PatternPriority BuiltinNodeType<kind>::patternPriority() const
{
    /* See XSL Transformations (XSLT) Version 2.0, 6.4 Conflict Resolution for
     * Template Rules */
    switch(kind)
    {
        case QXmlNodeModelIndex::Text:
        /* Fallthrough. */
        case QXmlNodeModelIndex::ProcessingInstruction:
        /* Fallthrough. */
        case QXmlNodeModelIndex::Comment:
            /* "If the pattern is any other NodeTest, optionally preceded by a
             * PatternAxis, then the priority is 0.5."
             * Fallthrough. */
        case QXmlNodeModelIndex::Attribute:
        /* Fallthrough. */
        case QXmlNodeModelIndex::Element:
        /* Fallthrough. */
        case QXmlNodeModelIndex::Document:
            /* "If the pattern has the form /, then the priority is -0.5.". */
            return -0.5;
        default:
        {
            Q_ASSERT_X(false, Q_FUNC_INFO, "Unknown node type");
            return 0;
        }
    }

}

