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

#include "qanyitemtype_p.h"
#include "qderivedinteger_p.h"

#include "qbuiltinatomictypes_p.h"
#include "qbuiltinnodetype_p.h"
#include "qbuiltintypes_p.h"
#include "qxsltnodetest_p.h"

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

const ItemType::Ptr BuiltinTypes::xsltNodeTest(new XSLTNodeTest());

QT_END_NAMESPACE
