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
