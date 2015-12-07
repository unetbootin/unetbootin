/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the QtXMLPatterns module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Commercial License Agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Nokia.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
** package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
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

        /**
         * @short Returns the name of the type.
         *
         * The reason to why we take the name pool argument, is that the basic
         * types, @c xs:anySimpleType and so on, are stored globally in
         * BuiltinTypes and ComonSequenceTypes, and therefore cannot be tied to
         * a certain name pool. Type instances that knows they always will be
         * used with a certain name pool, can therefore ignore @p np and return
         * a QXmlName instance stored as a member.
         *
         * If the type code was refactored to not be store globally and
         * therfore by design would be tied to a name pool, this argument could
         * be removed.
         */
        virtual QXmlName name(const NamePool::Ptr &np) const = 0;

        /**
         * @short Returns a suitable display name for this type.
         *
         * See name() for an explanation to why we take a NamePool as argument.
         */
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
