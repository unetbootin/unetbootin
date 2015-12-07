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

#ifndef Patternist_NoneType_H
#define Patternist_NoneType_H

#include "qatomictype_p.h"
#include "qsequencetype_p.h"

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

namespace QPatternist
{

    /**
     * @short Represents the special <tt>none</tt> type.
     *
     * @ingroup Patternist_types
     * @see <a href="http://www.w3.org/TR/xquery-semantics/#sec_content_models">XQuery 1.0 and
     * XPath 2.0 Formal Semantics, 2.4.3 Content models</a>
     * @see <a href="http://www.w3.org/TR/xquery-semantics/#sec_fnerror">XQuery 1.0 and XPath 2.0
     * Formal Semantics, 7.2.9 The fn:error function</a>
     * @author Frans Englich <fenglich@trolltech.com>
     */
    class NoneType : public ItemType,
                     public SequenceType
    {
    public:
        typedef QExplicitlySharedDataPointer<NoneType> Ptr;

        virtual bool itemMatches(const Item &item) const;
        virtual bool xdtTypeMatches(const ItemType::Ptr &other) const;

        /**
         * @returns always "none". That is, no namespace prefix
         */
        virtual QString displayName(const NamePool::Ptr &np) const;

        /**
         * @note The semantical meaning of this type's item type can
         * surely be discussed. The function is provided due to
         * it being mandated by the SequenceType base class.
         *
         * @returns always 'this' since NoneType is also an ItemType
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

        /**
         * @returns always @c false
         */
        virtual bool isAtomicType() const;

        /**
         * This can be thought to be a weird function for this type(none). There
         * is no atomized type for none, perhaps the best from a conceptual perspective
         * would be to return @c null.
         *
         * This function returns BuiltinTypes::xsAnyAtomicType because
         * the generic type checking code inserts an Atomizer in the AST
         * when an error() function(or other node which has type none) is part of
         * an operator expression(value/general comparison, arithmetics). The Atomizer
         * returns the atomizedType() of its child, and by here returning xsAnyAtomicType,
         * static operator lookup is postponed to runtime. Subsequently, expressions like error()
         * works properly with other XPath expressions.
         */
        virtual ItemType::Ptr atomizedType() const;

        /**
         * @returns always @c false
         */
        virtual bool isNodeType() const;

        /**
         * @returns always item()
         */
        virtual ItemType::Ptr xdtSuperType() const;

        /**
         * @returns always @p other. The none type can be thought as
         * disappearing when attempting to find the union of it and
         * another type.
         */
        virtual const ItemType &operator|(const ItemType &other) const;

    protected:

        friend class CommonSequenceTypes;
        NoneType();
    };
}

QT_END_NAMESPACE

QT_END_HEADER

#endif
