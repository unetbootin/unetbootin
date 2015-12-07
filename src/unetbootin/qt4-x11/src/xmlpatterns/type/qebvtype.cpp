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
