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

#include "qanyitemtype_p.h"
#include "qderivedinteger_p.h"

#include "qbuiltinatomictypes_p.h"
#include "qbuiltintypes_p.h"
#include "qbuiltinnodetype_p.h"

/* Included here to avoid the static initialization failure. */
#include "qatomiccasterlocators.cpp"
#include "qatomiccomparatorlocators.cpp"
#include "qatomicmathematicianlocators.cpp"

QT_BEGIN_NAMESPACE

using namespace QPatternist;

// STATIC DATA
/* Special cases. */
#define initType(var, cls) const cls::Ptr BuiltinTypes::var(new cls())
initType(item,                  AnyItemType);
initType(node,                  AnyNodeType);
#undef initType

#define initSType(var, cls) const SchemaType::Ptr BuiltinTypes::var(new cls())
initSType(xsAnyType,            AnyType);
initSType(xsAnySimpleType,      AnySimpleType);
initSType(xsUntyped,            Untyped);
#undef initSType

/* The primitive atomic types. */
#define at(className, varName) const AtomicType::Ptr BuiltinTypes::varName(new className());
at(AnyAtomicType,               xsAnyAtomicType)
at(UntypedAtomicType,           xsUntypedAtomic)
at(DateTimeType,                xsDateTime)
at(DateType,                    xsDate)
at(SchemaTimeType,                    xsTime)
at(DurationType,                xsDuration)
at(YearMonthDurationType,       xsYearMonthDuration)
at(DayTimeDurationType,         xsDayTimeDuration)

at(NumericType,                 numeric)
at(DecimalType,                 xsDecimal)
at(GYearMonthType,              xsGYearMonth)
at(GYearType,                   xsGYear)
at(GMonthDayType,               xsGMonthDay)
at(GDayType,                    xsGDay)
at(GMonthType,                  xsGMonth)

at(BooleanType,                 xsBoolean)
at(Base64BinaryType,            xsBase64Binary)
at(AnyURIType,                  xsAnyURI)

#define it(className, varName) const ItemType::Ptr BuiltinTypes::varName(new className());
at(QNameType,                   xsQName)
at(HexBinaryType,               xsHexBinary)
at(FloatType,                   xsFloat)
at(DoubleType,                  xsDouble)
#undef it

const AtomicType::Ptr BuiltinTypes::xsString(new StringType(BuiltinTypes::xsAnyAtomicType,
                                                            AtomicCasterLocator::Ptr(new ToStringCasterLocator())));

#define dsType(varName, parent)                                             \
    const AtomicType::Ptr BuiltinTypes::xs ## varName                       \
    (new DerivedStringType<Type ## varName>(BuiltinTypes::parent,           \
                           AtomicCasterLocator::Ptr(new ToDerivedStringCasterLocator<Type ## varName>())))

dsType(NormalizedString,    xsString);
dsType(Token,               xsNormalizedString);
dsType(Language,            xsToken);
dsType(NMTOKEN,             xsToken);
dsType(Name,                xsToken);
dsType(NCName,              xsName);
dsType(ID,                  xsNCName);
dsType(IDREF,               xsNCName);
dsType(ENTITY,              xsNCName);
#undef sType

const AtomicType::Ptr BuiltinTypes::xsInteger(new IntegerType(BuiltinTypes::xsDecimal,
                                                              AtomicCasterLocator::Ptr(new ToIntegerCasterLocator())));

#define iType(varName, parent)                                              \
    const AtomicType::Ptr BuiltinTypes::xs ## varName                       \
    (new DerivedIntegerType<Type ## varName>(parent,                        \
                                             AtomicCasterLocator::Ptr(new ToDerivedIntegerCasterLocator<Type ## varName>())))

/* Initialize derived integers. The order of initialization is significant. */
iType(NonPositiveInteger,   xsInteger);
iType(NegativeInteger,      xsNonPositiveInteger);
iType(Long,                 xsInteger);
iType(Int,                  xsLong);
iType(Short,                xsInt);
iType(Byte,                 xsShort);
iType(NonNegativeInteger,   xsInteger);
iType(UnsignedLong,         xsNonNegativeInteger);
iType(UnsignedInt,          xsUnsignedLong);
iType(UnsignedShort,        xsUnsignedInt);
iType(UnsignedByte,         xsUnsignedShort);
iType(PositiveInteger,      xsNonNegativeInteger);
#undef iType

at(NOTATIONType,            xsNOTATION)
#undef at

/* QXmlNodeModelIndex types */
#define nt(var, enu) const ItemType::Ptr BuiltinTypes::var = \
                           ItemType::Ptr(new BuiltinNodeType<QXmlNodeModelIndex::enu>())

nt(comment,     Comment);
nt(attribute,   Attribute);
nt(document,    Document);
nt(element,     Element);
nt(text,        Text);
nt(pi,          ProcessingInstruction);
#undef nt

QT_END_NAMESPACE
