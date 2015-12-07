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

#include "qbuiltintypes_p.h"
#include "qcommonsequencetypes_p.h"
#include "qitem_p.h"

#include "qebvtype_p.h"

QT_BEGIN_NAMESPACE

using namespace QPatternist;

EBVType::EBVType()
{
}

bool EBVType::itemMatches(const Item &item) const
{
    if(item.isNode())
        return false;

    return BuiltinTypes::xsBoolean->itemMatches(item)       ||
           BuiltinTypes::numeric->itemMatches(item)         ||
           BuiltinTypes::xsString->itemMatches(item)        ||
           BuiltinTypes::xsAnyURI->itemMatches(item)        ||
           CommonSequenceTypes::Empty->itemMatches(item)    ||
           BuiltinTypes::xsUntypedAtomic->itemMatches(item);
}

bool EBVType::xdtTypeMatches(const ItemType::Ptr &t) const
{
    return BuiltinTypes::node->xdtTypeMatches(t)            ||
           BuiltinTypes::xsBoolean->xdtTypeMatches(t)       ||
           BuiltinTypes::numeric->xdtTypeMatches(t)         ||
           BuiltinTypes::xsString->xdtTypeMatches(t)        ||
           BuiltinTypes::xsAnyURI->xdtTypeMatches(t)        ||
           *CommonSequenceTypes::Empty == *t                ||
           BuiltinTypes::xsUntypedAtomic->xdtTypeMatches(t) ||
           /* Item & xs:anyAtomicType is ok, we do the checking at runtime. */
           *BuiltinTypes::item == *t                        ||
           *BuiltinTypes::xsAnyAtomicType == *t;
}

QString EBVType::displayName(const NamePool::Ptr &) const
{
    /* Some QName-lexical is not used here because it makes little sense
     * for this strange type. Instead the operand type of the fn:boolean()'s
     * argument is used. */
    return QLatin1String("item()*(: EBV extractable type :)");
}

Cardinality EBVType::cardinality() const
{
    return Cardinality::zeroOrMore();
}

ItemType::Ptr EBVType::xdtSuperType() const
{
    return BuiltinTypes::item;
}

ItemType::Ptr EBVType::itemType() const
{
    return ItemType::Ptr(const_cast<EBVType *>(this));
}

bool EBVType::isAtomicType() const
{
    return false;
}

bool EBVType::isNodeType() const
{
    return true;
}

ItemType::Ptr EBVType::atomizedType() const
{
    Q_ASSERT_X(false, Q_FUNC_INFO,
               "That this function is called makes no sense.");
    return AtomicType::Ptr();
}

QT_END_NAMESPACE
