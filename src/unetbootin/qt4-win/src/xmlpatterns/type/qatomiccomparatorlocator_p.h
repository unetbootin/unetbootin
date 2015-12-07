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

#ifndef Patternist_AtomicComparatorLocator_H
#define Patternist_AtomicComparatorLocator_H

#include "qatomictypedispatch_p.h"

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

namespace QPatternist
{

    /**
     * @todo Docs missing
     *
     * @ingroup Patternist_types
     * @author Frans Englich <fenglich@trolltech.com>
     */
    class AtomicComparatorLocator : public ParameterizedAtomicTypeVisitor
    {
    public:
        typedef QExplicitlySharedDataPointer<AtomicComparatorLocator> Ptr;
        AtomicComparatorLocator();
        virtual ~AtomicComparatorLocator();

        virtual AtomicTypeVisitorResult::Ptr visit(const AnyAtomicType *, const qint16 op,
                                                   const SourceLocationReflection *const) const;
        virtual AtomicTypeVisitorResult::Ptr visit(const AnyURIType *, const qint16 op,
                                                   const SourceLocationReflection *const) const;
        virtual AtomicTypeVisitorResult::Ptr visit(const Base64BinaryType *, const qint16 op,
                                                   const SourceLocationReflection *const) const;
        virtual AtomicTypeVisitorResult::Ptr visit(const BooleanType *, const qint16 op,
                                                   const SourceLocationReflection *const) const;
        virtual AtomicTypeVisitorResult::Ptr visit(const DateTimeType *, const qint16 op,
                                                   const SourceLocationReflection *const) const;
        virtual AtomicTypeVisitorResult::Ptr visit(const DateType *, const qint16 op,
                                                   const SourceLocationReflection *const) const;
        virtual AtomicTypeVisitorResult::Ptr visit(const DayTimeDurationType *, const qint16 op,
                                                   const SourceLocationReflection *const) const;
        virtual AtomicTypeVisitorResult::Ptr visit(const DecimalType *, const qint16 op,
                                                   const SourceLocationReflection *const) const;
        virtual AtomicTypeVisitorResult::Ptr visit(const DoubleType *, const qint16 op,
                                                   const SourceLocationReflection *const) const;
        virtual AtomicTypeVisitorResult::Ptr visit(const DurationType *, const qint16 op,
                                                   const SourceLocationReflection *const) const;
        virtual AtomicTypeVisitorResult::Ptr visit(const FloatType *, const qint16 op,
                                                   const SourceLocationReflection *const) const;
        virtual AtomicTypeVisitorResult::Ptr visit(const GDayType *, const qint16 op,
                                                   const SourceLocationReflection *const) const;
        virtual AtomicTypeVisitorResult::Ptr visit(const GMonthDayType *, const qint16 op,
                                                   const SourceLocationReflection *const) const;
        virtual AtomicTypeVisitorResult::Ptr visit(const GMonthType *, const qint16 op,
                                                   const SourceLocationReflection *const) const;
        virtual AtomicTypeVisitorResult::Ptr visit(const GYearMonthType *, const qint16 op,
                                                   const SourceLocationReflection *const) const;
        virtual AtomicTypeVisitorResult::Ptr visit(const GYearType *, const qint16 op,
                                                   const SourceLocationReflection *const) const;
        virtual AtomicTypeVisitorResult::Ptr visit(const HexBinaryType *, const qint16 op,
                                                   const SourceLocationReflection *const) const;
        virtual AtomicTypeVisitorResult::Ptr visit(const IntegerType *, const qint16 op,
                                                   const SourceLocationReflection *const) const;
        virtual AtomicTypeVisitorResult::Ptr visit(const NOTATIONType *, const qint16 op,
                                                   const SourceLocationReflection *const) const;
        virtual AtomicTypeVisitorResult::Ptr visit(const QNameType *, const qint16 op,
                                                   const SourceLocationReflection *const) const;
        virtual AtomicTypeVisitorResult::Ptr visit(const StringType *, const qint16 op,
                                                   const SourceLocationReflection *const) const;
        virtual AtomicTypeVisitorResult::Ptr visit(const SchemaTimeType *, const qint16 op,
                                                   const SourceLocationReflection *const) const;
        virtual AtomicTypeVisitorResult::Ptr visit(const UntypedAtomicType *, const qint16 op,
                                                   const SourceLocationReflection *const) const;
        virtual AtomicTypeVisitorResult::Ptr visit(const YearMonthDurationType *, const qint16 op,
                                                   const SourceLocationReflection *const) const;
    };
}

QT_END_NAMESPACE

QT_END_HEADER

#endif
