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

#ifndef Patternist_CommonSequenceTypes_H
#define Patternist_CommonSequenceTypes_H

#include "qemptysequencetype_p.h"
#include "qnonetype_p.h"

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

namespace QPatternist
{
    /**
     * @short Provides access to singleton instances of SequenceType sub-classes.
     *
     * @ingroup Patternist_types
     * @author Frans Englich <fenglich@trolltech.com>
     */
    class Q_AUTOTEST_EXPORT CommonSequenceTypes
    {
    public:
        /**
         * <tt>xs:anyAtomicType?</tt>
         */
        static const SequenceType::Ptr ZeroOrOneAtomicType;

        /**
         * <tt>xs:anyAtomicType</tt>
         */
        static const SequenceType::Ptr ExactlyOneAtomicType;

        /**
         * <tt>xs:anyAtomicType*</tt>
         */
        static const SequenceType::Ptr ZeroOrMoreAtomicTypes;

        /**
         * <tt>item()</tt>
         */
        static const SequenceType::Ptr ExactlyOneItem;

        /**
         * <tt>item()*</tt>
         */
        static const SequenceType::Ptr ZeroOrMoreItems;

        /**
         * <tt>item()?</tt>
         */
        static const SequenceType::Ptr ZeroOrOneItem;

        /**
         * <tt>item()+</tt>
         */
        static const SequenceType::Ptr OneOrMoreItems;

        /**
         * The empty sequence, <tt>empty-sequence()</tt>.
         */
        static const EmptySequenceType::Ptr Empty;

        /**
         * The special type @c none. Used for the function <tt>fn:error()</tt>, for example.
         */
        static const NoneType::Ptr None;

        /**
         * <tt>xs:anyURI</tt>
         */
        static const SequenceType::Ptr ExactlyOneAnyURI;

        /**
         * <tt>xs:boolean</tt>
         */
        static const SequenceType::Ptr ExactlyOneBoolean;

        /**
         * <tt>xs:boolean?</tt>
         */
        static const SequenceType::Ptr ZeroOrOneBoolean;

        /**
         * <tt>xs:untypedAtomic</tt>
         */
        static const SequenceType::Ptr ExactlyOneUntypedAtomic;

        /**
         * <tt>xs:integer</tt>
         */
        static const SequenceType::Ptr ExactlyOneInteger;

        /**
         * <tt>xs:integer?</tt>
         */
        static const SequenceType::Ptr ZeroOrOneInteger;

        /**
         * <tt>xs:decimal?</tt>
         */
        static const SequenceType::Ptr ZeroOrOneDecimal;

        /**
         * <tt>xs:integer*</tt>
         */
        static const SequenceType::Ptr ZeroOrMoreIntegers;

        /**
         * <tt>xs:double</tt>
         */
        static const SequenceType::Ptr ExactlyOneDouble;

        /**
         * <tt>xs:decimal</tt>
         */
        static const SequenceType::Ptr ExactlyOneDecimal;

        /**
         * <tt>xs:float</tt>
         */
        static const SequenceType::Ptr ExactlyOneFloat;

        /**
         * <tt>xs:QName</tt>
         */
        static const SequenceType::Ptr ExactlyOneQName;

        /**
         * <tt>xs:string</tt>
         */
        static const SequenceType::Ptr ExactlyOneString;

        /**
         * <tt>xs:string?</tt>
         */
        static const SequenceType::Ptr ZeroOrOneString;

        /**
         * <tt>xs:string*</tt>
         */
        static const SequenceType::Ptr ZeroOrMoreStrings;

        /**
         * <tt>xs:NCName*</tt>
         */
        static const SequenceType::Ptr ZeroOrOneNCName;

        /**
         * <tt>xs:QName?</tt>
         */
        static const SequenceType::Ptr ZeroOrOneQName;

        /**
         * The artificial type in XPath 2.0 that covers @c xs:double, @c xs:float,
         * @c xs:decimal, with cardinality zero or one.
         *
         * @see <a href="http://www.w3.org/TR/xpath20/#dt-numeric">XML Path Language
         * (XPath) 2.0, definition for Numeric</a>
         * @see <a href="http://www.w3.org/TR/xpath-functions/#func-signatures">XQuery 1.0
         * and XPath 2.0 Functions and Operators, 1.3 Function Signatures and Descriptions</a>
         * @see BuiltinTypes::numeric
         */
        static const SequenceType::Ptr ZeroOrOneNumeric;

        /**
         * @c numeric
         */
        static const SequenceType::Ptr ExactlyOneNumeric;

        /**
         * <tt>node()?</tt>
         */
        static const SequenceType::Ptr ZeroOrOneNode;

        /**
         * <tt>node()</tt>
         */
        static const SequenceType::Ptr ExactlyOneNode;

        /**
         * <tt>node()*</tt>
         */
        static const SequenceType::Ptr ZeroOrMoreNodes;

        /**
         * <tt>element()</tt>
         */
        static const SequenceType::Ptr ExactlyOneElement;

        /**
         * <tt>processing-instruction()</tt>
         */
        static const SequenceType::Ptr ExactlyOneProcessingInstruction;

        /**
         * <tt>attribute()</tt>
         */
        static const SequenceType::Ptr ExactlyOneAttribute;

        /**
         * <tt>text()</tt>
         */
        static const SequenceType::Ptr ExactlyOneTextNode;

        /**
         * <tt>text()?</tt>
         */
        static const SequenceType::Ptr ZeroOrOneTextNode;

        /**
         * <tt>comment()</tt>
         */
        static const SequenceType::Ptr ExactlyOneComment;

        /**
         * <tt>element()*</tt>
         */
        static const SequenceType::Ptr ZeroOrMoreElements;

        /**
         * <tt>document-node()?</tt>
         */
        static const SequenceType::Ptr ZeroOrOneDocumentNode;

        /**
         * <tt>document-node()</tt>
         */
        static const SequenceType::Ptr ExactlyOneDocumentNode;

        /**
         * Identifiers all values which the Effective %Boolean Value
         * can be extracted from.
         */
        static const SequenceType::Ptr EBV;

        /**
         * <tt>xs:anyURI?</tt>
         */
        static const SequenceType::Ptr ZeroOrOneAnyURI;

        /**
         * <tt>xs:hexBinary</tt>
         */
        static const SequenceType::Ptr ExactlyOneHexBinary;

        /**
         * <tt>xs:base64Binary</tt>
         */
        static const SequenceType::Ptr ExactlyOneBase64Binary;

        /**
         * <tt>xs:date</tt>
         */
        static const SequenceType::Ptr ExactlyOneDate;

        /**
         * <tt>xs:dateTime</tt>
         */
        static const SequenceType::Ptr ExactlyOneDateTime;

        /**
         * <tt>xs:dayTimeDuration</tt>
         */
        static const SequenceType::Ptr ExactlyOneDayTimeDuration;

        /**
         * <tt>xs:duration</tt>
         */
        static const SequenceType::Ptr ExactlyOneDuration;

        /**
         * <tt>xs:gDay</tt>
         */
        static const SequenceType::Ptr ExactlyOneGDay;

        /**
         * <tt>xs:gMonth</tt>
         */
        static const SequenceType::Ptr ExactlyOneGMonth;

        /**
         * <tt>xs:gMonthDay</tt>
         */
        static const SequenceType::Ptr ExactlyOneGMonthDay;

        /**
         * <tt>xs:gYear</tt>
         */
        static const SequenceType::Ptr ExactlyOneGYear;

        /**
         * <tt>xs:gYearMonth</tt>
         */
        static const SequenceType::Ptr ExactlyOneGYearMonth;

        /**
         * <tt>xs:yearMonthDuration</tt>
         */
        static const SequenceType::Ptr ExactlyOneYearMonthDuration;

        /**
         * <tt>xs:time</tt>
         */
        static const SequenceType::Ptr ExactlyOneTime;

        /**
         * <tt>xs:time?</tt>
         */
        static const SequenceType::Ptr ZeroOrOneDate;

        /**
         * <tt>xs:dateTime?</tt>
         */
        static const SequenceType::Ptr ZeroOrOneDateTime;

        /**
         * <tt>xs:dayTimeDuration?</tt>
         */
        static const SequenceType::Ptr ZeroOrOneDayTimeDuration;

        /**
         * <tt>xs:duration?</tt>
         */
        static const SequenceType::Ptr ZeroOrOneDuration;

        /**
         * <tt>xs:time?</tt>
         */
        static const SequenceType::Ptr ZeroOrOneTime;

        /**
         * <tt>xs:yearMonthDuration?</tt>
         */
        static const SequenceType::Ptr ZeroOrOneYearMonthDuration;

    private:
        /**
         * The constructor is private and has no implementation,
         * because this class is not meant to be instantiated.
         *
         * It should only be used via its static members.
         */
        inline CommonSequenceTypes();

        Q_DISABLE_COPY(CommonSequenceTypes)
    };
}

QT_END_NAMESPACE

QT_END_HEADER

#endif

