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

#ifndef Patternist_ContextItem_H
#define Patternist_ContextItem_H

#include "qemptycontainer_p.h"

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

namespace QPatternist
{
    /**
     * @short Implements the context item, the dot: <tt>.</tt>.
     *
     * @see <a href="http://www.w3.org/TR/xpath20/#id-context-item-expression">XML Path Language
     * (XPath) 2.0, 3.1.4 Context Item Expression</a>
     * @author Frans Englich <fenglich@trolltech.com>
     * @ingroup Patternist_expressions
     */
    class ContextItem : public EmptyContainer
    {
    public:
        /**
         * @p expr is possibly used for error reporting. If this context item has been
         * created implicitly, such as for the expression <tt>fn:string()</tt>, @p expr
         * should be passed a valid pointer to the Expression that this context
         * item is generated for.
         */
        inline ContextItem(const Expression::Ptr &expr = Expression::Ptr()) : m_expr(expr)
        {
        }

        virtual Item evaluateSingleton(const DynamicContext::Ptr &context) const;
        virtual SequenceType::Ptr staticType() const;

        /**
         * @returns always DisableElimination and RequiresContextItem
         */
        virtual Expression::Properties properties() const;

        virtual ExpressionVisitorResult::Ptr accept(const ExpressionVisitor::Ptr &visitor) const;

        /**
         * Overridden to store a pointer to StaticContext::contextItemType().
         */
        virtual Expression::Ptr compress(const StaticContext::Ptr &context);

        /**
         * Overridden to store a pointer to StaticContext::contextItemType().
         */
        virtual Expression::Ptr typeCheck(const StaticContext::Ptr &context,
                                          const SequenceType::Ptr &reqType);

        /**
         * @returns always IDContextItem
         */
        virtual ID id() const;

        /**
         * @returns always BuiltinTypes::item;
         */
        virtual ItemType::Ptr expectedContextItemType() const;

        virtual const SourceLocationReflection *actualReflection() const;
        virtual void announceFocusType(const ItemType::Ptr &type);

    private:
        ItemType::Ptr           m_itemType;
        const Expression::Ptr   m_expr;
    };
}

QT_END_NAMESPACE

QT_END_HEADER

#endif
