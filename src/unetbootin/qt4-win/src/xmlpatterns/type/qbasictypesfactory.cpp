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
#include "qcommonnamespaces_p.h"

#include "qbasictypesfactory_p.h"

QT_BEGIN_NAMESPACE

using namespace QPatternist;

SchemaTypeFactory::Ptr BasicTypesFactory::self(const NamePool::Ptr &np)
{
    /* We don't store a global static here, because it's dependent on the NamePool. */
    return SchemaTypeFactory::Ptr(new BasicTypesFactory(np));
}

BasicTypesFactory::BasicTypesFactory(const NamePool::Ptr &np)
{
    m_types.reserve(48);

#define add(aName)   m_types.insert(BuiltinTypes::aName->name(np), AtomicType::Ptr(BuiltinTypes::aName))
#define addNA(aName) m_types.insert(BuiltinTypes::aName->name(np), BuiltinTypes::aName)
    add(xsString);
    add(xsBoolean);
    add(xsDecimal);
    add(xsDouble);
    add(xsFloat);
    add(xsDate);
    add(xsTime);
    add(xsDateTime);
    add(xsDuration);
    add(xsAnyURI);
    add(xsGDay);
    add(xsGMonthDay);
    add(xsGMonth);
    add(xsGYearMonth);
    add(xsGYear);
    add(xsBase64Binary);
    add(xsHexBinary);
    add(xsQName);
    add(xsInteger);
    addNA(xsAnyType);
    addNA(xsAnySimpleType);
    add(xsYearMonthDuration);
    add(xsDayTimeDuration);
    add(xsAnyAtomicType);
    addNA(xsUntyped);
    add(xsUntypedAtomic);
    add(xsNOTATION);
    /* Add derived primitives. */
    add(xsNonPositiveInteger);
    add(xsNegativeInteger);
    add(xsLong);
    add(xsInt);
    add(xsShort);
    add(xsByte);
    add(xsNonNegativeInteger);
    add(xsUnsignedLong);
    add(xsUnsignedInt);
    add(xsUnsignedShort);
    add(xsUnsignedByte);
    add(xsPositiveInteger);
    add(xsNormalizedString);
    add(xsToken);
    add(xsLanguage);
    add(xsNMTOKEN);
    add(xsName);
    add(xsNCName);
    add(xsID);
    add(xsIDREF);
    add(xsENTITY);

#undef add
#undef addNA
}

SchemaType::Ptr BasicTypesFactory::createSchemaType(const QXmlName name) const
{
    return m_types.value(name);
}

SchemaType::Hash BasicTypesFactory::types() const
{
    return m_types;
}

QT_END_NAMESPACE
