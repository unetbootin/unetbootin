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

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.

#ifndef Patternist_BuiltinTypes_H
#define Patternist_BuiltinTypes_H

#include "qanynodetype_p.h"
#include "qanysimpletype_p.h"
#include "qanytype_p.h"
#include "qbuiltinatomictype_p.h"
#include "qitemtype_p.h"
#include "qnumerictype_p.h"
#include "quntyped_p.h"

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

namespace QPatternist
{

    /**
     * @short Provides access to singleton instances of ItemType and SchemaType sub-classes.
     *
     * @ingroup Patternist_types
     * @author Frans Englich <fenglich@trolltech.com>
     */
    class Q_AUTOTEST_EXPORT BuiltinTypes
    {
    public:
        static const SchemaType::Ptr        xsAnyType;
        static const SchemaType::Ptr        xsAnySimpleType;
        static const SchemaType::Ptr        xsUntyped;

        static const AtomicType::Ptr        xsAnyAtomicType;
        static const AtomicType::Ptr        xsUntypedAtomic;
        static const AtomicType::Ptr        xsDateTime;
        static const AtomicType::Ptr        xsDate;
        static const AtomicType::Ptr        xsTime;
        static const AtomicType::Ptr        xsDuration;
        static const AtomicType::Ptr        xsYearMonthDuration;
        static const AtomicType::Ptr        xsDayTimeDuration;

        /**
         * An artificial type for implementation purposes
         * that represents the XPath type @c numeric.
         */
        static const AtomicType::Ptr        numeric;
        static const AtomicType::Ptr        xsFloat;
        static const AtomicType::Ptr        xsDouble;
        static const AtomicType::Ptr        xsInteger;
        static const AtomicType::Ptr        xsDecimal;
        static const AtomicType::Ptr        xsNonPositiveInteger;
        static const AtomicType::Ptr        xsNegativeInteger;
        static const AtomicType::Ptr        xsLong;
        static const AtomicType::Ptr        xsInt;
        static const AtomicType::Ptr        xsShort;
        static const AtomicType::Ptr        xsByte;
        static const AtomicType::Ptr        xsNonNegativeInteger;
        static const AtomicType::Ptr        xsUnsignedLong;
        static const AtomicType::Ptr        xsUnsignedInt;
        static const AtomicType::Ptr        xsUnsignedShort;
        static const AtomicType::Ptr        xsUnsignedByte;
        static const AtomicType::Ptr        xsPositiveInteger;


        static const AtomicType::Ptr        xsGYearMonth;
        static const AtomicType::Ptr        xsGYear;
        static const AtomicType::Ptr        xsGMonthDay;
        static const AtomicType::Ptr        xsGDay;
        static const AtomicType::Ptr        xsGMonth;

        static const AtomicType::Ptr        xsBoolean;

        static const AtomicType::Ptr        xsBase64Binary;
        static const AtomicType::Ptr        xsHexBinary;
        static const AtomicType::Ptr        xsAnyURI;
        static const AtomicType::Ptr        xsQName;
        static const AtomicType::Ptr        xsString;
        static const AtomicType::Ptr        xsNormalizedString;
        static const AtomicType::Ptr        xsToken;
        static const AtomicType::Ptr        xsLanguage;
        static const AtomicType::Ptr        xsNMTOKEN;
        static const AtomicType::Ptr        xsName;
        static const AtomicType::Ptr        xsNCName;
        static const AtomicType::Ptr        xsID;
        static const AtomicType::Ptr        xsIDREF;
        static const AtomicType::Ptr        xsENTITY;

        static const AtomicType::Ptr        xsNOTATION;
        static const ItemType::Ptr          item;

        static const AnyNodeType::Ptr       node;
        static const ItemType::Ptr          attribute;
        static const ItemType::Ptr          comment;
        static const ItemType::Ptr          document;
        static const ItemType::Ptr          element;
        static const ItemType::Ptr          pi;
        static const ItemType::Ptr          text;

    private:
        /**
         * The constructor is protected because this class is not meant to be instantiated,
         * but should only be used via its static const members.
         */
        BuiltinTypes();
        Q_DISABLE_COPY(BuiltinTypes)
    };
}

QT_END_NAMESPACE

QT_END_HEADER

#endif

