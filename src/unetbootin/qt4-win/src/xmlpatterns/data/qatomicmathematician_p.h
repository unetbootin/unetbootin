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

#ifndef Patternist_AtomicMathematician_H
#define Patternist_AtomicMathematician_H

#include <QFlags>

#include "qdynamiccontext_p.h"
#include "qitem_p.h"
#include "qatomictypedispatch_p.h"

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

namespace QPatternist
{
    /**
     * @short Base class for classes that performs arithmetic operations between atomic values.
     *
     * @ingroup Patternist_xdm
     * @author Frans Englich <fenglich@trolltech.com>
     */
    class Q_AUTOTEST_EXPORT AtomicMathematician : public AtomicTypeVisitorResult
    {
    public:
        virtual ~AtomicMathematician();

        typedef QExplicitlySharedDataPointer<AtomicMathematician> Ptr;

        enum Operator
        {
            /**
             * @see <a href="http://www.w3.org/TR/xpath-functions/#func-numeric-divide">XQuery 1.0
             * and XPath 2.0 Functions and Operators, 6.2.4 op:numeric-divide</a>
             */
            Div         = 1,

            /**
             * @see <a href="http://www.w3.org/TR/xpath-functions/#func-numeric-integer-divide">XQuery 1.0
             * and XPath 2.0 Functions and Operators, 6.2.5 op:numeric-integer-divide</a>
             */
            IDiv        = 2,

            /**
             * @see <a href="http://www.w3.org/TR/xpath-functions/#func-numeric-subtract">XQuery 1.0
             * and XPath 2.0 Functions and Operators, 6.2.2 op:numeric-subtract</a>
             */
            Substract   = 4,

            /**
             * @see <a href="http://www.w3.org/TR/xpath-functions/#func-numeric-mod">XQuery 1.0
             * and XPath 2.0 Functions and Operators, 6.2.6 op:numeric-mod</a>
             */
            Mod         = 8,

            /**
             * @see <a href="http://www.w3.org/TR/xpath-functions/#func-numeric-multiply">XQuery 1.0
             * and XPath 2.0 Functions and Operators, 6.2.3 op:numeric-multiply</a>
             */
            Multiply    = 16,

            /**
             * @see <a href="http://www.w3.org/TR/xpath-functions/#func-numeric-add">XQuery 1.0
             * and XPath 2.0 Functions and Operators, 6.2.1 op:numeric-add</a>
             */
            Add         = 32
        };

        typedef QFlags<Operator> Operators;

        virtual Item calculate(const Item &operand1,
                                    const Operator op,
                                    const Item &operand2,
                                    const QExplicitlySharedDataPointer<DynamicContext> &context) const = 0;

        static QString displayName(const AtomicMathematician::Operator op);

    };
    Q_DECLARE_OPERATORS_FOR_FLAGS(AtomicMathematician::Operators)
}

QT_END_NAMESPACE

QT_END_HEADER

#endif
