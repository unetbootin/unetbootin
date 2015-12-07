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

#ifndef Patternist_AnySimpleType_H
#define Patternist_AnySimpleType_H

#include "qanytype_p.h"

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

namespace QPatternist
{
    class AtomicType;

    /**
     * @short Represents the @c xs:anySimpleType item type.
     *
     * @ingroup Patternist_types
     * @see <a href="http://www.w3.org/TR/xmlschema-2/#dt-anySimpleType">XML Schema Part 2:
     * Datatypes Second Edition, The simple ur-type definition</a>
     * @author Frans Englich <fenglich@trolltech.com>
     */
    class AnySimpleType : public AnyType
    {
    public:
        friend class BuiltinTypes;

        virtual ~AnySimpleType();

        virtual QXmlName name(const NamePool::Ptr &np) const;

        /**
         * @returns always @c xs:anySimpleType
         */
        virtual QString displayName(const NamePool::Ptr &np) const;

        /**
         * @returns always BuiltinTypes::xsAnyType
         */
        virtual SchemaType::Ptr wxsSuperType() const;

        /**
         * xs:anySimpleType is the special "simple ur-type". Therefore this function
         * returns SchemaType::None
         *
         * @returns SchemaType::None
         */
        virtual TypeCategory category() const;

        /**
         * The simple ur-type is a "special restriction of the ur-type definition",
         * according to XML Schema Part 2: Datatypes Second Edition about xs:anySimpleType
         *
         * @returns DERIVATION_RESTRICTION
         */
        virtual SchemaType::DerivationMethod derivationMethod() const;

    protected:
        AnySimpleType();

    };
}

QT_END_NAMESPACE

QT_END_HEADER

#endif
