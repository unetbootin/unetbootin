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

#ifndef Patternist_SchemaType_H
#define Patternist_SchemaType_H

#include "qnamepool_p.h"
#include "qschemacomponent_p.h"
#include "qxmlname.h"

template<typename N, typename M> class QHash;

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

namespace QPatternist
{
    class AtomicType;

    /**
     * @short Base class for W3C XML Schema types.
     *
     * This is the base class of all data types in a W3C XML Schema.
     *
     * @ingroup Patternist_types
     * @author Frans Englich <fenglich@trolltech.com>
     */
    class SchemaType : public SchemaComponent
    {
    public:

        typedef QExplicitlySharedDataPointer<SchemaType> Ptr;
        typedef QHash<QXmlName, SchemaType::Ptr> Hash;

        /**
         * Schema types are divided into different categories such as
         * complex type, atomic imple type, union simple type, and so forth. This
         * enumerator, which category() returns a value of, identifies what
         * category the type belong to.
         *
         * @todo Add docs & links for the enums
         */
        enum TypeCategory
        {
            None = 0,
            /**
             * A simple atomic type. These are also sometimes
             * referred to as primitive types. An example of this type is
             * xs:string.
             *
             * Formally speaking, a simple type with variety atomic.
             */
            SimpleTypeAtomic,
            SimpleTypeList,
            SimpleTypeUnion,
            ComplexType
        };

        enum DerivationMethod
        {
            DerivationRestriction   = 1,
            DerivationExtension     = 2,
            DerivationUnion         = 4,
            DerivationList          = 8,
            /**
             * Used for <tt>xs:anyType</tt>.
             */
            NoDerivation            = 16
        };

        SchemaType();
        virtual ~SchemaType();

        /**
         * Determines how this SchemaType is derived from its super type.
         *
         * @note Despite that DerivationMethod is designed for being
         * used for bitwise OR'd value, this function may only return one enum
         * value. If the type does not derive from any type, which is the case of
         * <tt>xs:anyType</tt>, this function returns NoDerivation.
         *
         * @see SchemaType::wxsSuperType()
         * @see <a href="http://www.w3.org/TR/DOM-Level-3-Core/core.html#TypeInfo-DerivationMethods">Document
         * Object Model (DOM) Level 3 Core Specification, Definition group DerivationMethods</a>
         * @returns a DerivationMethod enumerator signifiying how
         * this SchemaType is derived from its base type
         */
        virtual DerivationMethod derivationMethod() const = 0;

        /**
         * Determines whether the type is an abstract type.
         *
         * @note It is important a correct value is returned, since
         * abstract types must not be instantiated.
         */
        virtual bool isAbstract() const = 0;

        virtual QXmlName name(const NamePool::Ptr &np) const = 0;
        virtual QString displayName(const NamePool::Ptr &np) const = 0;

        /**
         * @returns the W3C XML Schema base type that this type derives from. All types
         * returns an instance, except for the xs:anyType since it
         * is the very base type of all types, and it returns 0. Hence,
         * one can walk the hierarchy of a schema type by recursively calling
         * wxsSuperType, until zero is returned.
         *
         * This function walks the Schema hierarchy. Some simple types, the atomic types,
         * is also part of the XPath Data Model hierarchy, and their super type in that
         * hierarchy can be introspected with xdtSuperType().
         *
         * wxsSuperType() can be said to correspond to the {base type definition} property
         * in the Post Schema Valid Infoset(PSVI).
         *
         * @see ItemType::xdtSuperType()
         */
        virtual SchemaType::Ptr wxsSuperType() const = 0;

        /**
         * @returns @c true if @p other is identical to 'this' schema type or if @p other
         * is either directly or indirectly a base type of 'this'. Hence, calling
         * AnyType::wxsTypeMatches() with @p other as argument returns @c true for all types,
         * since all types have @c xs:anyType as super type.
         */
        virtual bool wxsTypeMatches(const SchemaType::Ptr &other) const = 0;

        virtual TypeCategory category() const = 0;

        /**
         * Determines whether the type is a simple type, by introspecting
         * the result of category().
         *
         * @note Do not re-implement this function, but instead override category()
         * and let it return an appropriate value.
         */
        bool isSimpleType() const;

        /**
         * Determines whether the type is a complex type, by introspecting
         * the result of category().
         *
         * @note Do not re-implement this function, but instead override category()
         * and let it return an appropriate value.
         */
        bool isComplexType() const;
    };
}

QT_END_NAMESPACE

QT_END_HEADER

#endif
