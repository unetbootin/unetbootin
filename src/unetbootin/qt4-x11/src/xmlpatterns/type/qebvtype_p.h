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

#ifndef Patternist_EBVType_H
#define Patternist_EBVType_H

#include "qatomictype_p.h"
#include "qsequencetype_p.h"

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

namespace QPatternist
{

    /**
     * @short Represents the type for which a value of can an Effective %Boolean Value
     * be extracted from.
     *
     * EBVType is an artificial type. It is not available to users of any host language
     * or is specified in any specification. It is used for implementing static type
     * checking for expressions such as IfThenClause and AndExpression.
     *
     * @ingroup Patternist_types
     * @author Frans Englich <fenglich@trolltech.com>
     */
    class EBVType : public ItemType,
                    public SequenceType
    {
    public:
        typedef QExplicitlySharedDataPointer<EBVType> Ptr;

        /**
         * @todo docs if it's an ebvable type, etc.
         */
        virtual bool itemMatches(const Item &item) const;
        virtual bool xdtTypeMatches(const ItemType::Ptr &other) const;

        virtual QString displayName(const NamePool::Ptr &np) const;

        /**
         * @note The semantical meaning of this type's item type can
         * surely be discussed. The function is provided due to
         * it being mandated by the SequenceType base class.
         *
         * @returns always 'this' since EBVType is also an ItemType
         */
        virtual ItemType::Ptr itemType() const;

        /**
         * @note The semantical meaning of this type's cardinality
         * can surely be discussed. The function is provided due to
         * it being mandated by the SequenceType base class.
         *
         * @returns always Cardinality::zeroOrMore()
         */
        virtual Cardinality cardinality() const;

        virtual bool isAtomicType() const;

        /**
         * @returns always @c null
         */
        virtual ItemType::Ptr atomizedType() const;

        /**
         * @returns always BuiltinTypes::item
         */
        virtual ItemType::Ptr xdtSuperType() const;

        /**
         * @returns always @c false
         */
        virtual bool isNodeType() const;

    protected:
        friend class CommonSequenceTypes;
        EBVType();
    };
}

QT_END_NAMESPACE

QT_END_HEADER

#endif
