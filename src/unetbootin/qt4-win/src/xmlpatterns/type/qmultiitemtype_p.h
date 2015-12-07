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

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.

#ifndef Patternist_MultiItemType_H
#define Patternist_MultiItemType_H

#include <QList>

#include "qitemtype_p.h"

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

namespace QPatternist
{
    /**
     * @short Represents multiple types such as <tt>document()</tt> @em or <tt>xs:integer</tt>.
     *
     * In some situations two or more different types are allowed. For example, XQuery's
     * @c validate expression accepts document or element nodes(but not attribute
     * nodes, for example). MultiItemType is useful in such situations, its constructor
     * takes a list of ItemType instances which its member functions treats as a wholeness.
     *
     * For example, xdtTypeMatches() returns @c true if any of the represented types matches.
     *
     * @ingroup Patternist_types
     * @author Frans Englich <fenglich@trolltech.com>
     */
    class MultiItemType : public ItemType
    {
    public:
        /**
         * Creates a MultiItemType representing the types in @p typeList. @p typeList must
         * contain two or more types.
         */
        MultiItemType(const ItemType::List &typeList);

        /**
         * The display name are the names concatenated with "|" as separator. For example,
         * if this MultiItemType represents the types <tt>document()</tt>, <tt>xs:integer</tt>,
         * and <tt>xs:anyAtomicType</tt>, the display name is
         * "document() | xs:integer | xs:anyAtomicType".
         */
        virtual QString displayName(const NamePool::Ptr &np) const;

        /**
         * If any of the types this MultiItemType represents matches @p item, it is
         * considered a match.
         *
         * @returns @c true if any of the housed ItemType instances matches @p item, otherwise @c false
         */
        virtual bool itemMatches(const Item &item) const;

        /**
         * If any of the types this MultiItemType represents matches @p other, it is
         * considered a match.
         *
         * @returns @c true if any of the housed ItemType instances matches @p other, otherwise @c false
         */
        virtual bool xdtTypeMatches(const ItemType::Ptr &other) const;

        /**
         * @returns @c true if any of the represented types is a node type.
         */
        virtual bool isNodeType() const;

        /**
         * @returns @c true if any of the represented types is an atomic type.
         */
        virtual bool isAtomicType() const;

        /**
         * Determines the union type of all the represented types super types. For example,
         * if the represented types are <tt>xs:integer</tt>, <tt>document()</tt>
         * and <tt>xs:string</tt>, <tt>item()</tt> is returned.
         */
        virtual ItemType::Ptr xdtSuperType() const;

        /**
         * Determines the union type of all the represented types atomized types. For example,
         * if the represented types are <tt>xs:integer</tt> and <tt>document()</tt>,
         * <tt>xs:anyAtomicType</tt> is returned, because that's the super type of <tt>xs:integer</tt>
         * and <tt>xs:untypedAtomic</tt>.
         */
        virtual ItemType::Ptr atomizedType() const;

    private:
        const ItemType::List m_types;
        const ItemType::List::const_iterator m_end;
    };
}

QT_END_NAMESPACE

QT_END_HEADER

#endif
