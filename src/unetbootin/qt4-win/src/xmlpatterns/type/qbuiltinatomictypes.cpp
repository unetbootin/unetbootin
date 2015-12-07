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

#include "qatomicmathematicianlocators_p.h"
#include "qbuiltintypes_p.h"

#include "qbuiltinatomictypes_p.h"

QT_BEGIN_NAMESPACE

using namespace QPatternist;

/* -------------------------------------------------------------- */
#define implAccept(className)                                                                       \
AtomicTypeVisitorResult::Ptr className##Type::accept(const AtomicTypeVisitor::Ptr &v,               \
                                                     const SourceLocationReflection *const r) const \
{                                                                                                   \
    return v->visit(this, r);                                                                       \
}                                                                                                   \
                                                                                                    \
AtomicTypeVisitorResult::Ptr                                                                        \
className##Type::accept(const ParameterizedAtomicTypeVisitor::Ptr &v,                               \
                        const qint16 op,                                                            \
                        const SourceLocationReflection *const r) const                              \
{                                                                                                   \
    return v->visit(this, op, r);                                                                   \
}

#define deployComp(className, qname, parent, comp, mather, caster)                          \
className##Type::className##Type() : BuiltinAtomicType(BuiltinTypes::parent,                \
                                                       comp,                                \
                                                       mather,                              \
                                                       caster)                              \
{                                                                                           \
}                                                                                           \
implAccept(className)

#define deployBase(className, qname, parent) deployComp(className, qname, parent,           \
                                                        AtomicComparatorLocator::Ptr(),     \
                                                        AtomicMathematicianLocator::Ptr(),  \
                                                        AtomicCasterLocator::Ptr())

#define deployFull(className, qname, parent)                                                \
deployComp(className, qname, parent,                                                        \
           AtomicComparatorLocator::Ptr(new className##ComparatorLocator()),                \
           AtomicMathematicianLocator::Ptr(),                                               \
           AtomicCasterLocator::Ptr(new To##className##CasterLocator()))

#define deployMathComp(className, qname, parent)                                            \
deployComp(className, qname, parent,                                                        \
           AtomicComparatorLocator::Ptr(new className##ComparatorLocator()),                \
           AtomicMathematicianLocator::Ptr(new className##MathematicianLocator()),          \
           AtomicCasterLocator::Ptr(new To##className##CasterLocator()))
/* -------------------------------------------------------------- */

/* -------------------------------------------------------------- */
/* xs:anyURI & xs:untypedAtomic are much treated like strings. This ensures
 * they get the correct operators and automatically takes care of type promotion. */
deployComp(UntypedAtomic,       xsUntypedAtomic,       xsAnyAtomicType,
           AtomicComparatorLocator::Ptr(new StringComparatorLocator()),
           AtomicMathematicianLocator::Ptr(),
           AtomicCasterLocator::Ptr(new ToUntypedAtomicCasterLocator()))
deployComp(AnyURI,              xsAnyURI,            xsAnyAtomicType,
           AtomicComparatorLocator::Ptr(new StringComparatorLocator()),
           AtomicMathematicianLocator::Ptr(),
           AtomicCasterLocator::Ptr(new ToAnyURICasterLocator()))

deployBase(NOTATION,            xsNOTATION,                 xsAnyAtomicType)

deployMathComp(Float,               xsFloat,                numeric)
deployMathComp(Double,              xsDouble,               numeric)
deployMathComp(Decimal,             xsDecimal,              numeric)
deployMathComp(DayTimeDuration,     xsDayTimeDuration,      xsDuration)
deployMathComp(YearMonthDuration,   xsYearMonthDuration,    xsDuration)
deployMathComp(Date,                xsDate,                 xsAnyAtomicType)
deployMathComp(DateTime,            xsDateTime,             xsAnyAtomicType)
deployMathComp(SchemaTime,          xsTime,                 xsAnyAtomicType)

deployFull(Base64Binary,        xsBase64Binary,             xsAnyAtomicType)
deployFull(Boolean,             xsBoolean,                  xsAnyAtomicType)
deployFull(Duration,            xsDuration,                 xsAnyAtomicType)
deployFull(GDay,                xsGDay,                     xsAnyAtomicType)
deployFull(GMonth,              xsGMonth,                   xsAnyAtomicType)
deployFull(GMonthDay,           xsGMonthDay,                xsAnyAtomicType)
deployFull(GYear,               xsGYear,                    xsAnyAtomicType)
deployFull(GYearMonth,          xsGYearMonth,               xsAnyAtomicType)
deployFull(HexBinary,           xsHexBinary,                xsAnyAtomicType)
deployFull(QName,               xsQName,                    xsAnyAtomicType)
/* --------------------------------------------------------------- */

/* --------------------------------------------------------------- */
StringType::StringType(const AtomicType::Ptr &pType,
                       const AtomicCasterLocator::Ptr &casterLoc)
: BuiltinAtomicType(pType,
                    AtomicComparatorLocator::Ptr(new StringComparatorLocator()),
                    AtomicMathematicianLocator::Ptr(),
                    casterLoc)
{
}
implAccept(String)
/* --------------------------------------------------------------- */

/* --------------------------------------------------------------- */
IntegerType::IntegerType(const AtomicType::Ptr &pType,
                         const AtomicCasterLocator::Ptr &casterLoc)
: BuiltinAtomicType(pType,
                    AtomicComparatorLocator::Ptr(new IntegerComparatorLocator()),
                    AtomicMathematicianLocator::Ptr(new IntegerMathematicianLocator()),
                    casterLoc)
{
}
implAccept(Integer)
/* --------------------------------------------------------------- */

/* ---------------------- Special Overrides ---------------------- */
AnyAtomicType::AnyAtomicType() : BuiltinAtomicType(AtomicType::Ptr(),
                                                   AtomicComparatorLocator::Ptr(),
                                                   AtomicMathematicianLocator::Ptr(),
                                                   AtomicCasterLocator::Ptr())
{
}
implAccept(AnyAtomic)

ItemType::Ptr AnyAtomicType::xdtSuperType() const
{
    return BuiltinTypes::item;
}

SchemaType::Ptr AnyAtomicType::wxsSuperType() const
{
    return BuiltinTypes::xsAnySimpleType;
}

bool AnyAtomicType::isAbstract() const
{
    return true;
}

bool NOTATIONType::isAbstract() const
{
    return true;
}

#define implementName(className, typeName)                          \
QXmlName className##Type::name(const NamePool::Ptr &np) const          \
{                                                                   \
    return np->allocateQName(StandardNamespaces::xs, typeName);     \
}                                                                   \
                                                                    \
QString className##Type::displayName(const NamePool::Ptr &np) const \
{                                                                   \
    return np->displayName(name(np));                               \
}

implementName(AnyAtomic,            QLatin1String("anyAtomicType"))
implementName(AnyURI,               QLatin1String("anyURI"))
implementName(Base64Binary,         QLatin1String("base64Binary"))
implementName(Boolean,              QLatin1String("boolean"))
implementName(Date,                 QLatin1String("date"))
implementName(DateTime,             QLatin1String("dateTime"))
implementName(DayTimeDuration,      QLatin1String("dayTimeDuration"))
implementName(Decimal,              QLatin1String("decimal"))
implementName(Double,               QLatin1String("double"))
implementName(Duration,             QLatin1String("duration"))
implementName(Float,                QLatin1String("float"))
implementName(GDay,                 QLatin1String("gDay"))
implementName(GMonthDay,            QLatin1String("gMonthDay"))
implementName(GMonth,               QLatin1String("gMonth"))
implementName(GYearMonth,           QLatin1String("gYearMonth"))
implementName(GYear,                QLatin1String("gYear"))
implementName(HexBinary,            QLatin1String("hexBinary"))
implementName(Integer,              QLatin1String("integer"))
implementName(NOTATION,             QLatin1String("NOTATION"))
implementName(QName,                QLatin1String("QName"))
implementName(String,               QLatin1String("string"))
implementName(SchemaTime,                 QLatin1String("time"))
implementName(UntypedAtomic,        QLatin1String("untypedAtomic"))
implementName(YearMonthDuration,    QLatin1String("yearMonthDuration"))
/* --------------------------------------------------------------- */

#undef implAccept
#undef implementName
#undef deployComp
#undef deployBase
#undef deployFull
#undef deployMathComp

QT_END_NAMESPACE
