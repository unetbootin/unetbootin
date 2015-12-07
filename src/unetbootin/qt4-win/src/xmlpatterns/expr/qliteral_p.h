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

#ifndef Patternist_Literal_H
#define Patternist_Literal_H

#include "qemptycontainer_p.h"

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

namespace QPatternist
{
    /**
     * @short Houses an AtomicValue, making it available as an Expression.
     *
     * This is not only literals that can be created via the XQuery syntax(strings and numbers), but
     * all other atomic values, such as <tt>xs:date</tt> or <tt>xs:time</tt>.
     *
     * @see <a href="http://www.w3.org/TR/xquery/#id-literals">XQuery 1.0: An XML Query Language,
     * 3.1.1 Literals</a>
     * @author Frans Englich <fenglich@trolltech.com>
     * @ingroup Patternist_expressions
     */
    class Literal : public EmptyContainer
    {
    public:
        /**
         * Creates a Literal that represents @p item.
         *
         * @param item must be non-null and cannot be a QXmlNodeModelIndex.
         */
        Literal(const Item &item);

        virtual Item evaluateSingleton(const DynamicContext::Ptr &context) const;
        virtual bool evaluateEBV(const DynamicContext::Ptr &context) const;
        void evaluateToSequenceReceiver(const DynamicContext::Ptr &context) const;

        virtual SequenceType::Ptr staticType() const;
        virtual ExpressionVisitorResult::Ptr accept(const ExpressionVisitor::Ptr &visitor) const;
        virtual ID id() const;
        virtual QString description() const;

        /**
         * @returns Expression::DisableElimination and Expression::IsEvaluated
         */
        virtual Properties properties() const;

        /**
         * Declaring the return value of this function a const reference, leads
         * to crashes in patternistview, for a to me unknown reason.
         */
        inline Item item() const
        {
            return m_item;
        }

    private:
        const Item m_item;
    };

    /**
     * @short Creates a Literal that wraps @p item, and returns it.
     *
     * This simplifies code. Instead of writing:
     *
     * @code
     * Expression::Ptr(new Literal(item));
     * @endcode
     *
     * One can write:
     *
     * @code
     * wrapLiteral(item);
     * @endcode
     *
     * This function is not declared static, because it breaks the build on
     * at least aix-xlc-64.
     *
     * @relates Literal
     */
    inline Expression::Ptr wrapLiteral(const Item &item,
                                       const StaticContext::Ptr &context,
                                       const SourceLocationReflection *const r)
    {
        Q_ASSERT(item);

        const Expression::Ptr retval(new Literal(item));
        context->addLocation(retval.data(), context->locationFor(r));

        return retval;
    }
}

QT_END_NAMESPACE

QT_END_HEADER

#endif
