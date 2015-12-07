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

#include "qebvtype_p.h"
#include "qgenericsequencetype_p.h"
#include "qnonetype_p.h"

#include "qcommonsequencetypes_p.h"

/* To avoid the static initialization fiasco, we put the builtin types in this compilation unit, since
 * the sequence types depends on them. */
#include "qbuiltintypes.cpp"

QT_BEGIN_NAMESPACE

using namespace QPatternist;

// STATIC DATA
#define st(var, type, card)                                             \
const SequenceType::Ptr                                                 \
CommonSequenceTypes::var(new GenericSequenceType(BuiltinTypes::type,    \
                                                 Cardinality::card()))

/* Alphabetically. */
st(ExactlyOneAnyURI,                xsAnyURI,               exactlyOne);
st(ExactlyOneAtomicType,            xsAnyAtomicType,        exactlyOne);
st(ExactlyOneAttribute,             attribute,              exactlyOne);
st(ExactlyOneBase64Binary,          xsBase64Binary,         exactlyOne);
st(ExactlyOneBoolean,               xsBoolean,              exactlyOne);
st(ExactlyOneComment,               comment,                exactlyOne);
st(ExactlyOneDate,                  xsDate,                 exactlyOne);
st(ExactlyOneDateTime,              xsDateTime,             exactlyOne);
st(ExactlyOneDayTimeDuration,       xsDayTimeDuration,      exactlyOne);
st(ExactlyOneDecimal,               xsDecimal,              exactlyOne);
st(ExactlyOneDocumentNode,          document,               exactlyOne);
st(ExactlyOneDouble,                xsDouble,               exactlyOne);
st(ExactlyOneDuration,              xsDuration,             exactlyOne);
st(ExactlyOneElement,               element,                exactlyOne);
st(ExactlyOneFloat,                 xsFloat,                exactlyOne);
st(ExactlyOneGDay,                  xsGDay,                 exactlyOne);
st(ExactlyOneGMonth,                xsGMonth,               exactlyOne);
st(ExactlyOneGMonthDay,             xsGMonthDay,            exactlyOne);
st(ExactlyOneGYear,                 xsGYear,                exactlyOne);
st(ExactlyOneGYearMonth,            xsGYearMonth,           exactlyOne);
st(ExactlyOneHexBinary,             xsHexBinary,            exactlyOne);
st(ExactlyOneInteger,               xsInteger,              exactlyOne);
st(ExactlyOneItem,                  item,                   exactlyOne);
st(ExactlyOneNode,                  node,                   exactlyOne);
st(ExactlyOneNumeric,               numeric,                exactlyOne);
st(ExactlyOneProcessingInstruction, pi,                     exactlyOne);
st(ExactlyOneQName,                 xsQName,                exactlyOne);
st(ExactlyOneString,                xsString,               exactlyOne);
st(ExactlyOneTextNode,              text,                   exactlyOne);
st(ExactlyOneTime,                  xsTime,                 exactlyOne);
st(ExactlyOneUntypedAtomic,         xsUntypedAtomic,        exactlyOne);
st(ExactlyOneYearMonthDuration,     xsYearMonthDuration,    exactlyOne);
st(OneOrMoreItems,                  item,                   oneOrMore);
st(ZeroOrMoreAtomicTypes,           xsAnyAtomicType,        zeroOrMore);
st(ZeroOrMoreElements,              element,                zeroOrMore);
st(ZeroOrMoreIntegers,              xsInteger,              zeroOrMore);
st(ZeroOrMoreItems,                 item,                   zeroOrMore);
st(ZeroOrMoreNodes,                 node,                   zeroOrMore);
st(ZeroOrMoreStrings,               xsString,               zeroOrMore);
st(ZeroOrOneAnyURI,                 xsAnyURI,               zeroOrOne);
st(ZeroOrOneAtomicType,             xsAnyAtomicType,        zeroOrOne);
st(ZeroOrOneBoolean,                xsBoolean,              zeroOrOne);
st(ZeroOrOneDate,                   xsDate,                 zeroOrOne);
st(ZeroOrOneDateTime,               xsDateTime,             zeroOrOne);
st(ZeroOrOneDayTimeDuration,        xsDayTimeDuration,      zeroOrOne);
st(ZeroOrOneDecimal,                xsDecimal,              zeroOrOne);
st(ZeroOrOneDocumentNode,           document,               zeroOrOne);
st(ZeroOrOneDuration,               xsDuration,             zeroOrOne);
st(ZeroOrOneInteger,                xsInteger,              zeroOrOne);
st(ZeroOrOneItem,                   item,                   zeroOrOne);
st(ZeroOrOneNCName,                 xsNCName,               zeroOrOne);
st(ZeroOrOneNode,                   node,                   zeroOrOne);
st(ZeroOrOneNumeric,                numeric,                zeroOrOne);
st(ZeroOrOneQName,                  xsQName,                zeroOrOne);
st(ZeroOrOneString,                 xsString,               zeroOrOne);
st(ZeroOrOneTextNode,               text,                   zeroOrOne);
st(ZeroOrOneTime,                   xsTime,                 zeroOrOne);
st(ZeroOrOneYearMonthDuration,      xsYearMonthDuration,    zeroOrOne);

#undef st

/* Special cases. */
const EmptySequenceType::Ptr    CommonSequenceTypes::Empty  (new EmptySequenceType());
const NoneType::Ptr             CommonSequenceTypes::None   (new NoneType());
const SequenceType::Ptr         CommonSequenceTypes::EBV    (new EBVType());


QT_END_NAMESPACE
