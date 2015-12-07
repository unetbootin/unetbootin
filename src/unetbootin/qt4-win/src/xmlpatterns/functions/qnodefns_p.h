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

#ifndef Patternist_NodeFNs_H
#define Patternist_NodeFNs_H

#include "qfunctioncall_p.h"
#include "qcastingplatform_p.h"

/**
 * @file
 * @short Contains classes implementing the functions found in
 * <a href="http://www.w3.org/TR/xpath-functions/#node-functions">XQuery 1.0 and
 * XPath 2.0 Functions and Operators, 14 Functions and Operators on Nodes</a>.
 *
 * @ingroup Patternist_functions
 */

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

namespace QPatternist
{
    /**
     * @short Implements the function <tt>fn:name()</tt>.
     *
     * @ingroup Patternist_functions
     * @author Frans Englich <fenglich@trolltech.com>
     */
    class NameFN : public FunctionCall
    {
    public:
        virtual Item evaluateSingleton(const DynamicContext::Ptr &context) const;
    };

    /**
     * @short Implements the function <tt>fn:local-name()</tt>.
     *
     * @ingroup Patternist_functions
     * @author Frans Englich <fenglich@trolltech.com>
     */
    class LocalNameFN : public FunctionCall
    {
    public:
        virtual Item evaluateSingleton(const DynamicContext::Ptr &context) const;
    };

    /**
     * @short Implements the function <tt>fn:namespace-uri()</tt>.
     *
     * @ingroup Patternist_functions
     * @author Frans Englich <fenglich@trolltech.com>
     */
    class NamespaceURIFN : public FunctionCall
    {
    public:
        virtual Item evaluateSingleton(const DynamicContext::Ptr &context) const;
    };

    /**
     * @short Implements the function <tt>fn:number()</tt>.
     *
     * NumberFN uses CastingPlatform for performing the actual casting.
     *
     * @ingroup Patternist_functions
     * @author Frans Englich <fenglich@trolltech.com>
     */
    class NumberFN : public FunctionCall,
                     public CastingPlatform<NumberFN, false>
    {
    public:
        virtual Item evaluateSingleton(const DynamicContext::Ptr &context) const;

        /**
         * Overriden in order to call CastingPlatform::prepareCasting(). It also
         * implements the optimization of rewriting to its operand if its
         * type is xs:double(since the <tt>fn:number()</tt> call is in that case superflorous).
         */
        virtual Expression::Ptr typeCheck(const StaticContext::Ptr &context,
                                          const SequenceType::Ptr &reqType);

        /**
         * @returns always BuiltinTypes::xsDouble.
         */
        inline ItemType::Ptr targetType() const
        {
            return BuiltinTypes::xsDouble;
        }
    };

    /**
     * @short Implements the function <tt>fn:lang()</tt>.
     *
     * @ingroup Patternist_functions
     * @author Frans Englich <fenglich@trolltech.com>
     */
    class LangFN : public FunctionCall
    {
    public:
        virtual Item evaluateSingleton(const DynamicContext::Ptr &context) const;

    private:
        static inline bool isLangMatch(const QString &candidate, const QString &toMatch);
    };

    /**
     * @short Implements the function <tt>fn:root()</tt>.
     *
     * @ingroup Patternist_functions
     * @author Frans Englich <fenglich@trolltech.com>
     */
    class RootFN : public FunctionCall
    {
    public:
        virtual Item evaluateSingleton(const DynamicContext::Ptr &context) const;
        /**
         * Infers its cardinality from the argument.
         */
        virtual SequenceType::Ptr staticType() const;
    };
}

QT_END_NAMESPACE

QT_END_HEADER

#endif
