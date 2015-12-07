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

#ifndef Patternist_StringValueFNs_H
#define Patternist_StringValueFNs_H

#include <QByteArray>

#include "qfunctioncall_p.h"

/**
 * @file
 * @short Contains classes implementing the functions found in
 * <a href="http://www.w3.org/TR/xpath-functions/#string-value-functions">XQuery 1.0 and
 * XPath 2.0 Functions and Operators, 7.4 Functions on AtomicString Values</a>.
 *
 * @ingroup Patternist_functions
 */

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

namespace QPatternist
{

    /**
     * @short Implements the function <tt>fn:concat()</tt>.
     *
     * @ingroup Patternist_functions
     * @author Frans Englich <fenglich@trolltech.com>
     */
    class ConcatFN : public FunctionCall
    {
    public:
        virtual Item evaluateSingleton(const DynamicContext::Ptr &context) const;
    };

    /**
     * @short Implements the function <tt>fn:string-join()</tt>.
     *
     * @ingroup Patternist_functions
     * @author Frans Englich <fenglich@trolltech.com>
     */
    class StringJoinFN : public FunctionCall
    {
    public:
        virtual Item evaluateSingleton(const DynamicContext::Ptr &context) const;
    };

    /**
     * @short Implements the function <tt>fn:substring()</tt>.
     *
     * @ingroup Patternist_functions
     * @author Frans Englich <fenglich@trolltech.com>
     */
    class SubstringFN : public FunctionCall
    {
    public:
        virtual Item evaluateSingleton(const DynamicContext::Ptr &context) const;
    };

    /**
     * @short Implements the function <tt>fn:string-length()</tt>.
     *
     * @ingroup Patternist_functions
     * @author Frans Englich <fenglich@trolltech.com>
     */
    class StringLengthFN : public FunctionCall
    {
    public:
        virtual Item evaluateSingleton(const DynamicContext::Ptr &context) const;
    };

    /**
     * @short Implements the function <tt>fn:normalize-space()</tt>.
     *
     * @ingroup Patternist_functions
     * @author Frans Englich <fenglich@trolltech.com>
     */
    class NormalizeSpaceFN : public FunctionCall
    {
    public:
        virtual Item evaluateSingleton(const DynamicContext::Ptr &context) const;
    };

    /**
     * @short Implements the function <tt>fn:normalize-unicode()</tt>.
     *
     * What perhaps can be said significant with the implementation, is that it
     * attempts to determine the normalization form at compile time, in order to
     * reduce string work at runtime.
     *
     * @ingroup Patternist_functions
     * @author Frans Englich <fenglich@trolltech.com>
     */
    class NormalizeUnicodeFN : public FunctionCall
    {
    public:
        /**
         * Initializes private data.
         */
        NormalizeUnicodeFN();
        virtual Item evaluateSingleton(const DynamicContext::Ptr &context) const;
        virtual Expression::Ptr compress(const StaticContext::Ptr &context);

    private:
        int determineNormalizationForm(const DynamicContext::Ptr &context) const;
        QString::NormalizationForm m_normForm;
    };

    /**
     * @short Implements the function <tt>fn:upper-case()</tt>.
     *
     * @ingroup Patternist_functions
     * @author Frans Englich <fenglich@trolltech.com>
     */
    class UpperCaseFN : public FunctionCall
    {
    public:
        virtual Item evaluateSingleton(const DynamicContext::Ptr &context) const;
    };

    /**
     * @short Implements the function <tt>fn:lower-case()</tt>.
     *
     * @short Implements the function <tt>fn:concat()</tt>.
     * @ingroup Patternist_functions
     * @author Frans Englich <fenglich@trolltech.com>
     */
    class LowerCaseFN : public FunctionCall
    {
    public:
        virtual Item evaluateSingleton(const DynamicContext::Ptr &context) const;
    };

    /**
     * @short Implements the function <tt>fn:translate()</tt>.
     *
     * @ingroup Patternist_functions
     * @author Frans Englich <fenglich@trolltech.com>
     */
    class TranslateFN : public FunctionCall
    {
    public:
        virtual Item evaluateSingleton(const DynamicContext::Ptr &context) const;
    };

    /**
     * @short Provides functionality for encoding strings. Sub-classed by various
     * function implementations.
     *
     * @ingroup Patternist_functions
     * @author Frans Englich <fenglich@trolltech.com>
     */
    class EncodeString : public FunctionCall
    {
    public:
        /**
         * Evaluates its first operand. If it is the empty sequence, an empty string
         * is returned. Otherwise, the item's string value is returned percent encoded
         * as specified in this class's constructor.
         */
        virtual Item evaluateSingleton(const DynamicContext::Ptr &context) const;

    protected:
        /**
         * Encodes its operand with QUrl::toPercentEncoding(), with @p includeChars as
         * the characters to encode, and @p excludeChars as the characters to not encode.
         */
        EncodeString(const QByteArray &excludeChars, const QByteArray &includeChars);

    private:
        const QByteArray m_excludeChars;
        const QByteArray m_includeChars;
    };

    /**
     * @short Implements the function <tt>fn:encode-for-uri()</tt>.
     *
     * @ingroup Patternist_functions
     * @author Frans Englich <fenglich@trolltech.com>
     */
    class EncodeForURIFN : public EncodeString
    {
    public:
        /**
         * Performs internal initialization.
         */
        EncodeForURIFN();

    private:
        static const char *const include;
    };

    /**
     * @short Implements the function <tt>fn:iri-to-uri()</tt>.
     *
     * @ingroup Patternist_functions
     * @author Frans Englich <fenglich@trolltech.com>
     */
    class IriToURIFN : public EncodeString
    {
    public:
        /**
         * Performs internal initialization.
         */
        IriToURIFN();

    private:
        static const char *const exclude;
    };

    /**
     * @short Implements the function <tt>fn:escape-html-uri()</tt>.
     *
     * @ingroup Patternist_functions
     * @author Frans Englich <fenglich@trolltech.com>
     */
    class EscapeHtmlURIFN : public EncodeString
    {
    public:
        /**
         * Performs internal initialization.
         */
        EscapeHtmlURIFN();

    private:
        static const char *const include;
        static const char *const exclude;
    };
}

QT_END_NAMESPACE

QT_END_HEADER

#endif
