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

#ifndef Patternist_BuiltinAtomicType_H
#define Patternist_BuiltinAtomicType_H

#include "qatomictype_p.h"

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

namespace QPatternist
{

    /**
     * @short Instances of this class represents types that are sub-classes
     * of @c xs:anyAtomicType.
     *
     * Retrieving instances of builtin types is done
     * via BuiltinTypesFactory::createSchemaType(), not by instantiating this
     * class manually.
     *
     * @ingroup Patternist_types
     * @author Frans Englich <fenglich@trolltech.com>
     */
    class BuiltinAtomicType : public AtomicType
    {
    public:

        typedef QExplicitlySharedDataPointer<BuiltinAtomicType> Ptr;

        /**
         * @returns always @c false
         */
        virtual bool isAbstract() const;

        /**
         * @returns the base type as specified in the constructors baseType argument.
         */
        virtual SchemaType::Ptr wxsSuperType() const;

        /**
         * @returns the same type as wxsSuperType(), except for the type @c xs:anyAtomicType, which
         * returns item()
         */
        virtual ItemType::Ptr xdtSuperType() const;

        virtual AtomicComparatorLocator::Ptr comparatorLocator() const;
        virtual AtomicMathematicianLocator::Ptr mathematicianLocator() const;
        virtual AtomicCasterLocator::Ptr casterLocator() const;

    protected:
        friend class BuiltinTypes;

        /**
         * @param baseType the type that is the super type of the constructed
         * atomic type. In the case of AnyAtomicType, @c null is passed.
         * @param comp the AtomicComparatorLocator this type should return. May be @c null.
         * @param mather similar to @p comp, this is the AtomicMathematicianLocator
         * that's appropriate for this type May be @c null.
         * @param casterLocator the CasterLocator that locates classes performing
         * casting with this type. May be @c null.
         */
        BuiltinAtomicType(const AtomicType::Ptr &baseType,
                          const AtomicComparatorLocator::Ptr &comp,
                          const AtomicMathematicianLocator::Ptr &mather,
                          const AtomicCasterLocator::Ptr &casterLocator);

    private:
        const AtomicType::Ptr                   m_superType;
        const AtomicComparatorLocator::Ptr      m_comparatorLocator;
        const AtomicMathematicianLocator::Ptr   m_mathematicianLocator;
        const AtomicCasterLocator::Ptr          m_casterLocator;
    };
}

QT_END_NAMESPACE

QT_END_HEADER

#endif
