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

#include "qatomictype_p.h"
#include "qbuiltintypes_p.h"
#include "qitem_p.h"
#include "qschematype_p.h"

#include "qnumerictype_p.h"

QT_BEGIN_NAMESPACE

using namespace QPatternist;

NumericType::NumericType()
{
}

NumericType::~NumericType()
{
}

bool NumericType::itemMatches(const Item &item) const
{
    if(item.isNode())
        return false;

    return BuiltinTypes::xsDouble->itemMatches(item)    ||
           BuiltinTypes::xsDecimal->itemMatches(item)   ||
           BuiltinTypes::xsFloat->itemMatches(item);
}

bool NumericType::xdtTypeMatches(const ItemType::Ptr &t) const
{
    return BuiltinTypes::xsDouble->xdtTypeMatches(t)    ||
           BuiltinTypes::xsDecimal->xdtTypeMatches(t)   ||
           BuiltinTypes::xsFloat->xdtTypeMatches(t)     ||
           *t == *this; /* If it's NumericType */
}

QString NumericType::displayName(const NamePool::Ptr &) const
{
    return QLatin1String("numeric");
}

SchemaType::Ptr NumericType::wxsSuperType() const
{
    return BuiltinTypes::xsAnyAtomicType;
}

ItemType::Ptr NumericType::xdtSuperType() const
{
    return BuiltinTypes::xsAnyAtomicType;
}

bool NumericType::isAbstract() const
{
    return true;
}

bool NumericType::isNodeType() const
{
    return false;
}

bool NumericType::isAtomicType() const
{
    return true;
}

ItemType::Ptr NumericType::atomizedType() const
{
    return AtomicType::Ptr();
}

AtomicTypeVisitorResult::Ptr NumericType::accept(const AtomicTypeVisitor::Ptr &,
                                                 const SourceLocationReflection *const) const
{
    return AtomicTypeVisitorResult::Ptr();
}

AtomicTypeVisitorResult::Ptr NumericType::accept(const ParameterizedAtomicTypeVisitor::Ptr &,
                                                 const qint16,
                                                 const SourceLocationReflection *const) const
{
    return AtomicTypeVisitorResult::Ptr();
}

AtomicComparatorLocator::Ptr NumericType::comparatorLocator() const
{
    return AtomicComparatorLocator::Ptr();
}

AtomicMathematicianLocator::Ptr NumericType::mathematicianLocator() const
{
    return AtomicMathematicianLocator::Ptr();
}

AtomicCasterLocator::Ptr NumericType::casterLocator() const
{
    return AtomicCasterLocator::Ptr();
}

QT_END_NAMESPACE
