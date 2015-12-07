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

#ifndef Patternist_AtomicTypeDispatch_H
#define Patternist_AtomicTypeDispatch_H

#include <QSharedData>


QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

namespace QPatternist
{
    class AnyAtomicType;
    class AnyURIType;
    class Base64BinaryType;
    class BooleanType;
    class DateTimeType;
    class DateType;
    class DayTimeDurationType;
    class DecimalType;
    class DoubleType;
    class DurationType;
    class FloatType;
    class GDayType;
    class GMonthDayType;
    class GMonthType;
    class GYearMonthType;
    class GYearType;
    class HexBinaryType;
    class IntegerType;
    class NOTATIONType;
    class QNameType;
    class SourceLocationReflection;
    class StringType;
    class SchemaTimeType;
    class UntypedAtomicType;
    class YearMonthDurationType;

    enum TypeOfDerivedInteger
    {
        TypeByte,
        TypeInt,
        TypeLong,
        TypeNegativeInteger,
        TypeNonNegativeInteger,
        TypeNonPositiveInteger,
        TypePositiveInteger,
        TypeShort,
        TypeUnsignedByte,
        TypeUnsignedInt,
        TypeUnsignedLong,
        TypeUnsignedShort
    };

    template<TypeOfDerivedInteger DerivedType> class DerivedIntegerType;

    enum TypeOfDerivedString
    {
        TypeString,
        TypeNormalizedString,
        TypeToken,
        TypeLanguage,
        TypeNMTOKEN,
        TypeName,
        TypeNCName,
        TypeID,
        TypeIDREF,
        TypeENTITY
    };

    template<TypeOfDerivedString DerivedType> class DerivedStringType;

    /**
     * @todo Documentation's missing:
     * - Add link to wikipedia's "multiple dispatch" and "visitor" page.
     * - Add link to http://www.eptacom.net/pubblicazioni/pub_eng/mdisp.html
     *
     * @defgroup Patternist_types_dispatch Atomic Type Dispatching
     */

    /**
     * @todo Docs missing
     *
     * @ingroup Patternist_types_dispatch
     * @author Frans Englich <fenglich@trolltech.com>
     */
    class AtomicTypeVisitorResult : public QSharedData
    {
    public:
        typedef QExplicitlySharedDataPointer<AtomicTypeVisitorResult> Ptr;
        AtomicTypeVisitorResult() {}
        virtual ~AtomicTypeVisitorResult() {}
    };

    /**
     * @todo Docs missing
     *
     * @see ParameterizedAtomicTypeVisitor
     * @ingroup Patternist_types_dispatch
     * @author Frans Englich <fenglich@trolltech.com>
     */
    class AtomicTypeVisitor : public QSharedData
    {
    public:
        typedef QExplicitlySharedDataPointer<AtomicTypeVisitor> Ptr;
        virtual ~AtomicTypeVisitor() {}

        virtual AtomicTypeVisitorResult::Ptr visit(const AnyAtomicType *,
                                                   const SourceLocationReflection *const reflection) const = 0;
        virtual AtomicTypeVisitorResult::Ptr visit(const AnyURIType *,
                                                   const SourceLocationReflection *const reflection) const = 0;
        virtual AtomicTypeVisitorResult::Ptr visit(const Base64BinaryType *,
                                                   const SourceLocationReflection *const reflection) const = 0;
        virtual AtomicTypeVisitorResult::Ptr visit(const BooleanType *,
                                                   const SourceLocationReflection *const reflection) const = 0;
        virtual AtomicTypeVisitorResult::Ptr visit(const DateTimeType *,
                                                   const SourceLocationReflection *const reflection) const = 0;
        virtual AtomicTypeVisitorResult::Ptr visit(const DateType *,
                                                   const SourceLocationReflection *const reflection) const = 0;
        virtual AtomicTypeVisitorResult::Ptr visit(const DayTimeDurationType *,
                                                   const SourceLocationReflection *const reflection) const = 0;
        virtual AtomicTypeVisitorResult::Ptr visit(const DecimalType *,
                                                   const SourceLocationReflection *const reflection) const = 0;
        virtual AtomicTypeVisitorResult::Ptr visit(const DoubleType *,
                                                   const SourceLocationReflection *const reflection) const = 0;
        virtual AtomicTypeVisitorResult::Ptr visit(const DurationType *,
                                                   const SourceLocationReflection *const reflection) const = 0;
        virtual AtomicTypeVisitorResult::Ptr visit(const FloatType *,
                                                   const SourceLocationReflection *const reflection) const = 0;
        virtual AtomicTypeVisitorResult::Ptr visit(const GDayType *,
                                                   const SourceLocationReflection *const reflection) const = 0;
        virtual AtomicTypeVisitorResult::Ptr visit(const GMonthDayType *,
                                                   const SourceLocationReflection *const reflection) const = 0;
        virtual AtomicTypeVisitorResult::Ptr visit(const GMonthType *,
                                                   const SourceLocationReflection *const reflection) const = 0;
        virtual AtomicTypeVisitorResult::Ptr visit(const GYearMonthType *,
                                                   const SourceLocationReflection *const reflection) const = 0;
        virtual AtomicTypeVisitorResult::Ptr visit(const GYearType *,
                                                   const SourceLocationReflection *const reflection) const = 0;
        virtual AtomicTypeVisitorResult::Ptr visit(const HexBinaryType *,
                                                   const SourceLocationReflection *const reflection) const = 0;
        virtual AtomicTypeVisitorResult::Ptr visit(const IntegerType *,
                                                   const SourceLocationReflection *const reflection) const = 0;
        virtual AtomicTypeVisitorResult::Ptr visit(const NOTATIONType *,
                                                   const SourceLocationReflection *const reflection) const = 0;
        virtual AtomicTypeVisitorResult::Ptr visit(const QNameType *,
                                                   const SourceLocationReflection *const reflection) const = 0;
        virtual AtomicTypeVisitorResult::Ptr visit(const StringType *,
                                                   const SourceLocationReflection *const reflection) const = 0;
        virtual AtomicTypeVisitorResult::Ptr visit(const SchemaTimeType *,
                                                   const SourceLocationReflection *const reflection) const = 0;
        virtual AtomicTypeVisitorResult::Ptr visit(const UntypedAtomicType *,
                                                   const SourceLocationReflection *const reflection) const = 0;
        virtual AtomicTypeVisitorResult::Ptr visit(const YearMonthDurationType *,
                                                   const SourceLocationReflection *const reflection) const = 0;
    };

    /**
     * @todo Docs missing
     *
     * @see AtomicTypeVisitor
     * @ingroup Patternist_types_dispatch
     * @author Frans Englich <fenglich@trolltech.com>
     */
    class ParameterizedAtomicTypeVisitor : public QSharedData
    {
    public:
        typedef QExplicitlySharedDataPointer<ParameterizedAtomicTypeVisitor> Ptr;
        virtual ~ParameterizedAtomicTypeVisitor() {}

        virtual AtomicTypeVisitorResult::Ptr visit(const AnyAtomicType *, const qint16 param,
                                                   const SourceLocationReflection *const reflection) const = 0;
        virtual AtomicTypeVisitorResult::Ptr visit(const AnyURIType *, const qint16 param,
                                                   const SourceLocationReflection *const reflection) const = 0;
        virtual AtomicTypeVisitorResult::Ptr visit(const Base64BinaryType *, const qint16 param,
                                                   const SourceLocationReflection *const reflection) const = 0;
        virtual AtomicTypeVisitorResult::Ptr visit(const BooleanType *, const qint16 param,
                                                   const SourceLocationReflection *const reflection) const = 0;
        virtual AtomicTypeVisitorResult::Ptr visit(const DateTimeType *, const qint16 param,
                                                   const SourceLocationReflection *const reflection) const = 0;
        virtual AtomicTypeVisitorResult::Ptr visit(const DateType *, const qint16 param,
                                                   const SourceLocationReflection *const reflection) const = 0;
        virtual AtomicTypeVisitorResult::Ptr visit(const DayTimeDurationType *, const qint16 param,
                                                   const SourceLocationReflection *const reflection) const = 0;
        virtual AtomicTypeVisitorResult::Ptr visit(const DecimalType *, const qint16 param,
                                                   const SourceLocationReflection *const reflection) const = 0;
        virtual AtomicTypeVisitorResult::Ptr visit(const DoubleType *, const qint16 param,
                                                   const SourceLocationReflection *const reflection) const = 0;
        virtual AtomicTypeVisitorResult::Ptr visit(const DurationType *, const qint16 param,
                                                   const SourceLocationReflection *const reflection) const = 0;
        virtual AtomicTypeVisitorResult::Ptr visit(const FloatType *, const qint16 param,
                                                   const SourceLocationReflection *const reflection) const = 0;
        virtual AtomicTypeVisitorResult::Ptr visit(const GDayType *, const qint16 param,
                                                   const SourceLocationReflection *const reflection) const = 0;
        virtual AtomicTypeVisitorResult::Ptr visit(const GMonthDayType *, const qint16 param,
                                                   const SourceLocationReflection *const reflection) const = 0;
        virtual AtomicTypeVisitorResult::Ptr visit(const GMonthType *, const qint16 param,
                                                   const SourceLocationReflection *const reflection) const = 0;
        virtual AtomicTypeVisitorResult::Ptr visit(const GYearMonthType *, const qint16 param,
                                                   const SourceLocationReflection *const reflection) const = 0;
        virtual AtomicTypeVisitorResult::Ptr visit(const GYearType *, const qint16 param,
                                                   const SourceLocationReflection *const reflection) const = 0;
        virtual AtomicTypeVisitorResult::Ptr visit(const HexBinaryType *, const qint16 param,
                                                   const SourceLocationReflection *const reflection) const = 0;
        virtual AtomicTypeVisitorResult::Ptr visit(const IntegerType *, const qint16 param,
                                                   const SourceLocationReflection *const reflection) const = 0;
        virtual AtomicTypeVisitorResult::Ptr visit(const NOTATIONType *, const qint16 param,
                                                   const SourceLocationReflection *const reflection) const = 0;
        virtual AtomicTypeVisitorResult::Ptr visit(const QNameType *, const qint16 param,
                                                   const SourceLocationReflection *const reflection) const = 0;
        virtual AtomicTypeVisitorResult::Ptr visit(const StringType *, const qint16 param,
                                                   const SourceLocationReflection *const reflection) const = 0;
        virtual AtomicTypeVisitorResult::Ptr visit(const SchemaTimeType *, const qint16 param,
                                                   const SourceLocationReflection *const reflection) const = 0;
        virtual AtomicTypeVisitorResult::Ptr visit(const UntypedAtomicType *, const qint16 param,
                                                   const SourceLocationReflection *const reflection) const = 0;
        virtual AtomicTypeVisitorResult::Ptr visit(const YearMonthDurationType *, const qint16 param,
                                                   const SourceLocationReflection *const reflection) const = 0;
    };
}

QT_END_NAMESPACE

QT_END_HEADER

#endif
