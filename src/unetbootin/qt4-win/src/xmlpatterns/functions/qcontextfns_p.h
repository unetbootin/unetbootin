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

#ifndef Patternist_ContextFNs_H
#define Patternist_ContextFNs_H

#include "qfunctioncall_p.h"

/**
 * @file
 * @short Contains classes implementing the functions found in
 * <a href="http://www.w3.org/TR/xpath-functions/#context">XQuery 1.0 and
 * XPath 2.0 Functions and Operators, 16 Context Functions</a>.
 *
 * @ingroup Patternist_functions
 */

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

namespace QPatternist
{
    /**
     * @short Implements the function <tt>fn:position()</tt>.
     *
     * @see <a href="http://www.w3.org/TR/xpath-functions/#func-position">XQuery 1.0
     * and XPath 2.0 Functions and Operators, 16.1 fn:position</a>
     * @ingroup Patternist_functions
     * @author Frans Englich <fenglich@trolltech.com>
     */
    class PositionFN : public FunctionCall
    {
    public:
        virtual Item evaluateSingleton(const DynamicContext::Ptr &context) const;
    };

    /**
     * @short Implements the function <tt>fn:last()</tt>.
     *
     * @see <a href="http://www.w3.org/TR/xpath-functions/#func-last">XQuery 1.0
     * and XPath 2.0 Functions and Operators, 16.2 fn:last</a>
     * @ingroup Patternist_functions
     * @author Frans Englich <fenglich@trolltech.com>
     */
    class LastFN : public FunctionCall
    {
    public:
        virtual Item evaluateSingleton(const DynamicContext::Ptr &context) const;
    };

    /**
     * @short Implements the function <tt>fn:implicit-timezone()</tt>.
     *
     * @see <a href="http://www.w3.org/TR/xpath-functions/#func-implicit-timezone">XQuery 1.0
     * and XPath 2.0 Functions and Operators, 16.6 fn:implicit-timezone</a>
     * @ingroup Patternist_functions
     * @author Frans Englich <fenglich@trolltech.com>
     */
    class ImplicitTimezoneFN : public FunctionCall
    {
    public:
        virtual Item evaluateSingleton(const DynamicContext::Ptr &context) const;
    };

    /**
     * @short Implements the function <tt>fn:current-dateTime()</tt>.
     *
     * @see <a href="http://www.w3.org/TR/xpath-functions/#func-current-dateTime">XQuery 1.0
     * and XPath 2.0 Functions and Operators, 16.3 fn:current-dateTime</a>
     * @ingroup Patternist_functions
     * @author Frans Englich <fenglich@trolltech.com>
     */
    class CurrentDateTimeFN : public FunctionCall
    {
    public:
        virtual Item evaluateSingleton(const DynamicContext::Ptr &context) const;
    };

    /**
     * @short Implements the function <tt>fn:current-date()</tt>.
     *
     * @see <a href="http://www.w3.org/TR/xpath-functions/#func-current-date">XQuery 1.0
     * and XPath 2.0 Functions and Operators, 16.4 fn:current-date</a>
     * @ingroup Patternist_functions
     * @author Frans Englich <fenglich@trolltech.com>
     */
    class CurrentDateFN : public FunctionCall
    {
    public:
        virtual Item evaluateSingleton(const DynamicContext::Ptr &context) const;
    };

    /**
     * @short Implements the function <tt>fn:current-time()</tt>.
     *
     * @see <a href="http://www.w3.org/TR/xpath-functions/#func-current-time">XQuery 1.0
     * and XPath 2.0 Functions and Operators, 16.5 fn:current-date</a>
     * @ingroup Patternist_functions
     * @author Frans Englich <fenglich@trolltech.com>
     */
    class CurrentTimeFN : public FunctionCall
    {
    public:
        virtual Item evaluateSingleton(const DynamicContext::Ptr &context) const;
    };

    /**
     * @short Implements the function <tt>fn:default-collation()</tt>.
     *
     * This is done by rewriting to StaticContext::defaultCollation() at the typeCheck() stage.
     *
     * @see <a href="http://www.w3.org/TR/xpath-functions/#func-default-collation">XQuery 1.0
     * and XPath 2.0 Functions and Operators, 16.7 fn:default-collation</a>
     * @ingroup Patternist_functions
     * @author Frans Englich <fenglich@trolltech.com>
     */
    class DefaultCollationFN : public FunctionCall
    {
    public:
        virtual Expression::Ptr typeCheck(const StaticContext::Ptr &context,
                                          const SequenceType::Ptr &reqType);
    };

    /**
     * @short Implements the function <tt>fn:static-base-uri()</tt>.
     *
     * This is done by rewriting to StaticContext::baseURI() at the typeCheck() stage.
     *
     * @see <a href="http://www.w3.org/TR/xpath-functions/#func-static-base-uri">XQuery 1.0
     * and XPath 2.0 Functions and Operators, 16.8 fn:static-base-uri</a>
     * @ingroup Patternist_functions
     * @author Frans Englich <fenglich@trolltech.com>
     */
    class StaticBaseURIFN : public FunctionCall
    {
    public:
        virtual Expression::Ptr typeCheck(const StaticContext::Ptr &context,
                                          const SequenceType::Ptr &reqType);
    };
}

QT_END_NAMESPACE

QT_END_HEADER

#endif
