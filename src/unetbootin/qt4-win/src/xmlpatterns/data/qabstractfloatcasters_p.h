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

#ifndef Patternist_AbstractFloatCasters_H
#define Patternist_AbstractFloatCasters_H

#include "qabstractfloat_p.h"
#include "qatomiccaster_p.h"
#include "qnumeric_p.h"

/**
 * @file
 * @short Contains classes sub-classing AtomicCaster and which
 * are responsible of casting an atomic value to AbstractFloat.
 */

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

namespace QPatternist
{

    /**
     * @short Casts a @c numeric value, such as @c xs:integer or @c xs:float, to @c xs:double or xs:float.
     *
     * castFrom() uses Numeric::toDouble() for doing the actual casting.
     *
     * @ingroup Patternist_xdm
     * @author Vincent Ricard <magic@magicninja.org>
     */
    template <const bool isDouble>
    class NumericToAbstractFloatCaster : public AtomicCaster
    {
    public:
        virtual Item castFrom(const Item &from,
                              const QExplicitlySharedDataPointer<DynamicContext> &context) const;
    };

    /**
     * @short Casts a string value, @c xs:string or @c xs:untypedAtomic, to @c xs:double or xs:float.
     *
     * @ingroup Patternist_xdm
     * @author Vincent Ricard <magic@magicninja.org>
     */
    template <const bool isDouble>
    class StringToAbstractFloatCaster : public AtomicCaster
    {
    public:
        virtual Item castFrom(const Item &from,
                              const QExplicitlySharedDataPointer<DynamicContext> &context) const;
    };

    /**
     * @short Casts a value of type @c xs:boolean to @c xs:double or xs:float.
     *
     * @ingroup Patternist_xdm
     * @author Vincent Ricard <magic@magicninja.org>
     */
    template <const bool isDouble>
    class BooleanToAbstractFloatCaster : public AtomicCaster
    {
        public:
            virtual Item castFrom(const Item &from,
                                  const QExplicitlySharedDataPointer<DynamicContext> &context) const;
    };

#include "qabstractfloatcasters.cpp"

   /* * no doxygen
    * @short Casts a @c numeric value, such as @c xs:integer or @c xs:float, to @c xs:double.
    *
    * castFrom() uses Numeric::toDouble() for doing the actual casting.
    *
    * @ingroup Patternist_xdm
    * @author Frans Englich <fenglich@trolltech.com>
    */
    typedef NumericToAbstractFloatCaster<true> NumericToDoubleCaster;

   /* * no doxygen
    * @short Casts a @c numeric value, such as @c xs:double or @c xs:decimal, to @c xs:float.
    *
    * castFrom() uses Numeric::toFloat() for doing the actual casting.
    *
    * @ingroup Patternist_xdm
    * @author Frans Englich <fenglich@trolltech.com>
    */
    typedef NumericToAbstractFloatCaster<false> NumericToFloatCaster;

    /* * no doxygen
     * @short Casts a string value, @c xs:string or @c xs:untypedAtomic, to @c xs:double.
     *
     * @ingroup Patternist_xdm
     * @author Frans Englich <fenglich@trolltech.com>
     */
    typedef StringToAbstractFloatCaster<true> StringToDoubleCaster;

    /* * no doxygen
     * @short Casts a string value, @c xs:string or @c xs:untypedAtomic, to @c xs:float.
     *
     * @ingroup Patternist_xdm
     * @author Frans Englich <fenglich@trolltech.com>
     */
    typedef StringToAbstractFloatCaster<false> StringToFloatCaster;

   /* * no doxygen
    * @short Casts a value of type @c xs:boolean to @c xs:double.
    *
    * @ingroup Patternist_xdm
    * @author Frans Englich <fenglich@trolltech.com>
    */
    typedef BooleanToAbstractFloatCaster<true> BooleanToDoubleCaster;

    /* * no doxygen
     * @short Casts a value of type @c xs:boolean to @c xs:float.
     *
     * @ingroup Patternist_xdm
     * @author Frans Englich <fenglich@trolltech.com>
     */
    typedef BooleanToAbstractFloatCaster<false> BooleanToFloatCaster;
}

QT_END_NAMESPACE

QT_END_HEADER

#endif
