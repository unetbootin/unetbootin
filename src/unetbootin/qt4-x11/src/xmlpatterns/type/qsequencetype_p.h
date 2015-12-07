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

#ifndef Patternist_SequenceType_H
#define Patternist_SequenceType_H

template<typename T> class QList;

#include <QSharedData>

#include "qcardinality_p.h"
#include "qitemtype_p.h"

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

namespace QPatternist
{
    class ItemType;

    /**
     * @short A SequenceType instance represents the type of a sequence of Item
     * instances.
     *
     * It carries a Cardinality and ItemType, and is hence conceptually
     * identical to the SequenceType EBNF construct.
     *
     * @ingroup Patternist_types
     * @author Frans Englich <fenglich@trolltech.com>
     * @see <a href="http://www.w3.org/TR/xpath20/#id-sequencetype-syntax">XML
     * Path Language (XPath) 2.0, 2.5.3 SequenceType Syntax</a>
     */
    class SequenceType : public virtual QSharedData
    {
    public:
        inline SequenceType()
        {
        }

        /**
         * A smart pointer wrapping SequenceType instances.
         */
        typedef QExplicitlySharedDataPointer<const SequenceType> Ptr;

        /**
         * A list of SequenceType instances, each wrapped in a smart pointer.
         */
        typedef QList<SequenceType::Ptr> List;

        virtual ~SequenceType();

        /**
         * Generates a name for the sequence type for display purposes. The
         * prefix used for the QName identifying the schema type is conventional.
         * An example of a display name for a SequenceType is "xs:integer?".
         */
        virtual QString displayName(const NamePool::Ptr &np) const = 0;

        virtual Cardinality cardinality() const = 0;

        virtual ItemType::Ptr itemType() const = 0;

        /**
         * Determines whether @p other is identical to, or a sub-type
         * of this SequenceType. For example, if this SequenceType is
         * <tt>xs:anyAtomicType</tt>, @c false is returned if @p other is <tt>element()</tt>,
         * but @c true if @p other is <tt>xs:string</tt>.
         *
         * The return values of cardinality() and itemType() used with ItemType::xdtTypeMatches
         * and Cardinality::isWithinScope() is used for achieving this.
         *
         * @see <a href="http://www.w3.org/TR/xquery/#id-sequencetype-matching">XQuery 1.0:
         * An XML Query Language, 2.5.4 SequenceType Matching</a>
         */
        bool matches(const SequenceType::Ptr other) const;

        bool is(const SequenceType::Ptr &other) const;
    private:
        Q_DISABLE_COPY(SequenceType)
    };
}

Q_DECLARE_TYPEINFO(QPatternist::SequenceType::Ptr, Q_MOVABLE_TYPE);

QT_END_NAMESPACE

QT_END_HEADER

#endif
