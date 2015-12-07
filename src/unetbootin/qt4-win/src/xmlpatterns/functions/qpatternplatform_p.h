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

#ifndef Patternist_PatternPlatform_H
#define Patternist_PatternPlatform_H

#include <QFlags>
#include <QRegExp>

#include "qfunctioncall_p.h"

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

namespace QPatternist
{
    /**
     * @short Contains functionality for functions and expressions that
     * uses regular expressions.
     *
     * @ingroup Patternist_utils
     * @author Frans Englich <fenglich@trolltech.com>
     */
    class PatternPlatform : public FunctionCall
    {
    public:
        /**
         * @see <a href="http://www.w3.org/TR/xpath-functions/#flags">XQuery 1.0 and
         * XPath 2.0 Functions and Operators, 7.6.1.1 Flags</a>
         */
        enum Flag
        {
            /**
             * No flags are set. Default behavior is used.
             */
            NoFlags             = 0,

            /**
             * Flag @c s
             */
            DotAllMode          = 1,

            /**
             * Flag @c m
             */
            MultiLineMode       = 2,

            /**
             * Flag @c i
             */
            CaseInsensitive     = 4,

            /**
             * Flag @c x
             */
            SimplifyWhitespace  = 8
        };
        typedef QFlags<Flag> Flags;

        /**
         * Enum telling whether the flags, pattern, or both
         * have been compiled at compile time.
         */
        enum PreCompiledPart
        {
            NoPart          = 0,
            OnlyPattern     = 1,
            OnlyFlags       = 2,
            FlagsAndPattern = OnlyPattern | OnlyFlags

        };
        typedef QFlags<PreCompiledPart> PreCompiledParts;

        /**
         * @short This constructor is protected, because this class is supposed to be sub-classed.
         *
         * @param flagsPosition an index position specifying the operand containing the pattern
         * flags.
         */
        PatternPlatform(const qint8 flagsPosition);

        virtual Expression::Ptr compress(const StaticContext::Ptr &context);

        /**
         * Retrieves the pattern supplied in the arguments, taking care of compiling it,
         * settings its flags, and everything else required for getting it ready to use. If an error
         * occurs, an appropriate error is raised via @p context.
         */
        const QRegExp pattern(const DynamicContext::Ptr &context) const;

        /**
         * @returns the number of captures, also called parenthesized sub-expressions, the pattern has.
         *
         * If the pattern isn't precompiled, -1 is returned.
         */
        inline int captureCount() const;

    private:
        Q_DISABLE_COPY(PatternPlatform)

        Flags parseFlags(const QString &flags,
                         const DynamicContext::Ptr &context) const;

        QRegExp parsePattern(const QString &pattern,
                             const DynamicContext::Ptr &context) const;

        static void applyFlags(const Flags flags, QRegExp &pattern);

        /**
         * The parts that have been pre-compiled at compile time.
         */
        PreCompiledParts    m_compiledParts;
        Flags               m_flags;
        QRegExp             m_pattern;
        const qint8         m_flagsPosition;
    };

    inline int PatternPlatform::captureCount() const
    {
        if((m_compiledParts & OnlyPattern) == OnlyPattern)
            return m_pattern.numCaptures();
        else
            return -1;
    }

    Q_DECLARE_OPERATORS_FOR_FLAGS(PatternPlatform::Flags)
}

QT_END_NAMESPACE

QT_END_HEADER

#endif
