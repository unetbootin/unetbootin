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

#ifndef Patternist_GenericSequenceType_H
#define Patternist_GenericSequenceType_H

#include "qcommonsequencetypes_p.h"
#include "qsequencetype_p.h"

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

namespace QPatternist
{
    /**
     * @todo Documentation is missing.
     *
     * @ingroup Patternist_types
     * @author Frans Englich <fenglich@trolltech.com>
     */
    class GenericSequenceType : public SequenceType
    {
    public:
        GenericSequenceType(const ItemType::Ptr &itemType, const Cardinality &card);

        /**
         * Generates a name for the sequence type for display purposes. The
         * prefix used for the QName identifying the schema type is conventional.
         * An example of a display name for a GenericSequenceType is "xs:integer?".
         */
        virtual QString displayName(const NamePool::Ptr &np) const;

        virtual Cardinality cardinality() const;

        virtual ItemType::Ptr itemType() const;

    private:
        const ItemType::Ptr m_itemType;
        const Cardinality m_cardinality;
    };

    /**
     * @short An object generator for GenericSequenceType.
     *
     * makeGenericSequenceType() is a convenience function for avoiding invoking
     * the @c new operator, and wrapping the result in GenericSequenceType::Ptr.
     *
     * @returns a smart pointer to to a GenericSequenceType instaniated from @p itemType and @p cardinality.
     * @relates GenericSequenceType
     */
    static inline SequenceType::Ptr
    makeGenericSequenceType(const ItemType::Ptr &itemType, const Cardinality &cardinality)
    {
        /* An empty sequence of say integers, is the empty-sequence(). */
        if(cardinality.isEmpty())
            return CommonSequenceTypes::Empty;
        else
            return SequenceType::Ptr(new GenericSequenceType(itemType, cardinality));
    }
}

QT_END_NAMESPACE

QT_END_HEADER

#endif
