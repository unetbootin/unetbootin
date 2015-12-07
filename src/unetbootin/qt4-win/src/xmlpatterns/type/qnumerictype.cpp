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
