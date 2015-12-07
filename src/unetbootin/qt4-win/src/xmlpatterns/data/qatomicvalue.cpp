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

#include <QVariant>

#include "qabstractdatetime_p.h"
#include "qabstractfloat_p.h"
#include "qatomicstring_p.h"
#include "qatomictype_p.h"
#include "qboolean_p.h"
#include "qbuiltintypes_p.h"
#include "qdate_p.h"
#include "qdatetime_p.h"
#include "qderivedinteger_p.h"
#include "qdynamiccontext_p.h"
#include "qgenericsequencetype_p.h"
#include "qhexbinary_p.h"
#include "qinteger_p.h"
#include "qpatternistlocale_p.h"
#include "qqnamevalue_p.h"
#include "qqobjectnodemodel_p.h"
#include "qschematime_p.h"
#include "qvalidationerror_p.h"

#include "qitem_p.h"

QT_BEGIN_NAMESPACE

/**
 * @file
 * @short Contains the implementation for AtomicValue. The defintion is in qitem_p.h.
 */

using namespace QPatternist;

AtomicValue::~AtomicValue()
{
}

bool AtomicValue::evaluateEBV(const QExplicitlySharedDataPointer<DynamicContext> &context) const
{
    context->error(QtXmlPatterns::tr("A value of type %1 cannot have an "
                                     "Effective Boolean Value.")
                      .arg(formatType(context->namePool(), type())),
                      ReportContext::FORG0006,
                      QSourceLocation());
    return false; /* Silence GCC warning. */
}

bool AtomicValue::hasError() const
{
    return false;
}

QVariant AtomicValue::toQt(const AtomicValue *const value)
{
    Q_ASSERT_X(value, Q_FUNC_INFO,
               "Internal error, a null pointer cannot be passed.");

    const ItemType::Ptr t(value->type());

    if(BuiltinTypes::xsString->xdtTypeMatches(t)
       || BuiltinTypes::xsUntypedAtomic->xdtTypeMatches(t)
       || BuiltinTypes::xsAnyURI->xdtTypeMatches(t))
        return value->stringValue();
    /* Note, this occurs before the xsInteger test, since xs:unsignedLong
     * is a subtype of it. */
    else if(*BuiltinTypes::xsUnsignedLong == *t)
        return QVariant(value->as<DerivedInteger<TypeUnsignedLong> >()->storedValue());
    else if(BuiltinTypes::xsInteger->xdtTypeMatches(t))
        return QVariant(value->as<Numeric>()->toInteger());
    else if(BuiltinTypes::xsFloat->xdtTypeMatches(t)
            || BuiltinTypes::xsDouble->xdtTypeMatches(t)
            || BuiltinTypes::xsDecimal->xdtTypeMatches(t))
        return QVariant(value->as<Numeric>()->toDouble());
    /* We currently does not support xs:time. */
    else if(BuiltinTypes::xsDateTime->xdtTypeMatches(t))
        return QVariant(value->as<AbstractDateTime>()->toDateTime());
    else if(BuiltinTypes::xsDate->xdtTypeMatches(t))
        return QVariant(value->as<AbstractDateTime>()->toDateTime().toUTC().date());
    else if(BuiltinTypes::xsBoolean->xdtTypeMatches(t))
        return QVariant(value->as<Boolean>()->value());
    else if(BuiltinTypes::xsBase64Binary->xdtTypeMatches(t)
            || BuiltinTypes::xsHexBinary->xdtTypeMatches(t))
        return QVariant(value->as<Base64Binary>()->asByteArray());
    else if(BuiltinTypes::xsQName->xdtTypeMatches(t))
        return qVariantFromValue(value->as<QNameValue>()->qName());
    else
    {
        /* A type we don't support in Qt. Includes xs:time currently. */
        return QVariant();
    }
}

Item AtomicValue::toXDM(const QVariant &value)
{
    Q_ASSERT_X(value.isValid(), Q_FUNC_INFO,
               "QVariants sent to Patternist must be valid.");

    switch(value.type())
    {
        case QVariant::Char:
        /* Fallthrough. A single codepoint is a string in XQuery. */
        case QVariant::String:
            return AtomicString::fromValue(value.toString());
        case QVariant::Url:
        {
            /* QUrl doesn't follow the spec properly, so we
             * have to let it be an xs:string. Calling QVariant::toString()
             * on a QVariant that contains a QUrl returns, surprisingly,
             * an empty string. */
            return AtomicString::fromValue(value.toUrl().toString());
        }
        case QVariant::ByteArray:
            return HexBinary::fromValue(value.toByteArray());
        case QVariant::Int:
        /* Fallthrough. */
        case QVariant::LongLong:
        /* Fallthrough. */
        case QVariant::UInt:
            return Integer::fromValue(value.toLongLong());
        case QVariant::ULongLong:
            return DerivedInteger<TypeUnsignedLong>::fromValueUnchecked(value.toULongLong());
        case QVariant::Bool:
            return Boolean::fromValue(value.toBool());
        case QVariant::Time:
            return SchemaTime::fromDateTime(value.toDateTime());
        case QVariant::Date:
            return Date::fromDateTime(QDateTime(value.toDate(), QTime(), Qt::UTC));
        case QVariant::DateTime:
            return DateTime::fromDateTime(value.toDateTime());
        case QVariant::Double:
            return Item(Double::fromValue(value.toDouble()));
        default:
        {
            Q_ASSERT_X(false,
                       Q_FUNC_INFO,
                       qPrintable(QString::fromLatin1(
                           "QVariants of type %1 are not supported in "
                           "Patternist, see the documentation")
                                  .arg(QLatin1String(value.typeName()))));
            return AtomicValue::Ptr();
        }
    }
}

ItemType::Ptr AtomicValue::qtToXDMType(const QXmlItem &item)
{
    Q_ASSERT(!item.isNull());

    if(item.isNull())
        return ItemType::Ptr();

    if(item.isNode())
        return BuiltinTypes::node;

    Q_ASSERT(item.isAtomicValue());
    const QVariant v(item.toAtomicValue());

    switch(v.type())
    {
        case QVariant::Char:
        /* Fallthrough. */
        case QVariant::String:
        /* Fallthrough. */
        case QVariant::Url:
            return BuiltinTypes::xsString;
        case QVariant::Bool:
            return BuiltinTypes::xsBoolean;
        case QVariant::ByteArray:
            return BuiltinTypes::xsBase64Binary;
        case QVariant::Int:
        /* Fallthrough. */
        case QVariant::LongLong:
            return BuiltinTypes::xsInteger;
        case QVariant::ULongLong:
            return BuiltinTypes::xsUnsignedLong;
        case QVariant::Date:
            return BuiltinTypes::xsDate;
        case QVariant::DateTime:
        /* Fallthrough. */
        case QVariant::Time:
            return BuiltinTypes::xsDateTime;
        case QVariant::Double:
            return BuiltinTypes::xsDouble;
        default:
            return ItemType::Ptr();
    }
}

QT_END_NAMESPACE
