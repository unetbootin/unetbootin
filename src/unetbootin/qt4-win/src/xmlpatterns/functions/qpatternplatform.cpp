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

#include <QHash>

#include "qpatternistlocale_p.h"

#include "qpatternplatform_p.h"

QT_BEGIN_NAMESPACE

using namespace QPatternist;

namespace QPatternist
{
    /**
     * @short Used internally by PatternPlatform and describes
     * a flag that affects how a pattern is treated.
     *
     * The member variables aren't declared @c const, in order
     * to make the synthesized assignment operator and copy constructor work.
     *
     * @ingroup Patternist_utils
     * @author Frans Englich <fenglich@trolltech.com>
     */
    class PatternFlag
    {
    public:
        typedef QHash<QChar, PatternFlag> Hash;

        inline PatternFlag() : flag(PatternPlatform::NoFlags)
        {
        }

        inline PatternFlag(const PatternPlatform::Flag opt,
                           const QString &descr) : flag(opt),
                                                   description(descr)
        {
        }

        PatternPlatform::Flag   flag;
        QString                 description;

        static inline Hash flagDescriptions();
    };
}

static inline PatternFlag::Hash flagDescriptions()
{
    PatternFlag::Hash retval;

    retval.insert(QChar(QLatin1Char('s')),
                  PatternFlag(PatternPlatform::DotAllMode,
                              QtXmlPatterns::tr("%1 matches newline characters").arg(formatKeyword(QLatin1Char('.')))));

    retval.insert(QChar(QLatin1Char('m')),
                  PatternFlag(PatternPlatform::MultiLineMode,
                              QtXmlPatterns::tr("%1 and %2 match the start and end of a line.")
                                   .arg(formatKeyword(QLatin1Char('^')))
                                   .arg(formatKeyword(QLatin1Char('$')))));

    retval.insert(QChar(QLatin1Char('i')),
                  PatternFlag(PatternPlatform::CaseInsensitive,
                              QtXmlPatterns::tr("Matches are case insensitive")));

    retval.insert(QChar(QLatin1Char('x')),
                  PatternFlag(PatternPlatform::SimplifyWhitespace,
                              QtXmlPatterns::tr("Whitespace characters are removed, except when they appear "
                                 "in character classes")));

    return retval;
}

PatternPlatform::PatternPlatform(const qint8 flagsPosition) : m_compiledParts(NoPart),
                                                              m_flags(NoFlags),
                                                              m_flagsPosition(flagsPosition)
{
}

const QRegExp PatternPlatform::pattern(const DynamicContext::Ptr &context) const
{
    if(m_compiledParts == FlagsAndPattern) /* This is the most common case. */
    {
        Q_ASSERT(m_pattern.isValid());
        return m_pattern;
    }

    QRegExp retvalPattern;
    Flags flags;

    /* Compile the flags, if necessary. */
    if((m_compiledParts & OnlyPattern) == 0)
    {
        retvalPattern = m_pattern; /* Pattern is already compiled, so use it. */

        const Expression::Ptr flagsOp(m_operands.value(m_flagsPosition));

        if(flagsOp)
            flags = parseFlags(flagsOp->evaluateSingleton(context).stringValue(), context);
        else
            flags = NoFlags;
    }

    /* Compile the pattern, if necessary. */
    if((m_compiledParts & OnlyFlags) == 0)
    {
        retvalPattern = parsePattern(m_operands.at(1)->evaluateSingleton(context).stringValue(),
                                     context);

        if((m_compiledParts & OnlyPattern) != 0)
            flags = m_flags;
    }

    applyFlags(flags, retvalPattern);

    Q_ASSERT(m_pattern.isValid());
    return retvalPattern;
}

void PatternPlatform::applyFlags(const Flags flags, QRegExp &patternP)
{
    Q_ASSERT(patternP.isValid());
    if(flags == NoFlags)
        return;

    if(flags & CaseInsensitive)
    {
        patternP.setCaseSensitivity(Qt::CaseInsensitive);
    }
    // TODO Apply the other flags, like 'x'.
}

QRegExp PatternPlatform::parsePattern(const QString &patternP,
                                      const DynamicContext::Ptr &context) const
{

    if(patternP == QLatin1String("(.)\\3") ||
       patternP == QLatin1String("\\3")    ||
       patternP == QLatin1String("(.)\\2"))
    {
        context->error(QLatin1String("We don't want to hang infinitely on K2-MatchesFunc-9, "
                                     "10 and 11. See Trolltech task 148505."),
                       ReportContext::FOER0000, this);
        return QRegExp();
    }

    QRegExp retval(patternP);

    if(retval.isValid())
        return retval;
    else
    {
        context->error(QtXmlPatterns::tr("%1 is an invalid regular expression pattern: %2")
                                        .arg(formatExpression(patternP), retval.errorString()),
                                   ReportContext::FORX0002, this);
        return QRegExp();
    }
}

PatternPlatform::Flags PatternPlatform::parseFlags(const QString &flags,
                                                   const DynamicContext::Ptr &context) const
{

    if(flags.isEmpty())
        return NoFlags;

    const PatternFlag::Hash flagDescrs(flagDescriptions());
    const int len = flags.length();
    Flags retval = NoFlags;

    for(int i = 0; i < len; ++i)
    {
        const QChar flag(flags.at(i));
        const Flag specified = flagDescrs.value(flag).flag;

        if(specified != NoFlags)
        {
            retval |= specified;
            continue;
        }

        /* Generate a nice error message. */
        QString message(QtXmlPatterns::tr("%1 is an invalid flag for regular expressions. Valid flags are:")
                             .arg(formatKeyword(flag)));

        /* This is formatting, so don't bother translators with it. */
        message.append(QLatin1Char('\n'));

        const PatternFlag::Hash::const_iterator end(flagDescrs.constEnd());
        PatternFlag::Hash::const_iterator it(flagDescrs.constBegin());

        for(; it != end;)
        {
            // TODO handle bidi correctly
            // TODO format this with rich text(list/table)
            message.append(formatKeyword(it.key()));
            message.append(QLatin1String(" - "));
            message.append(it.value().description);

            ++it;
            if(it != end)
                message.append(QLatin1Char('\n'));
        }

        context->error(message, ReportContext::FORX0001, this);
        return NoFlags;
    }

    return retval;
}

Expression::Ptr PatternPlatform::compress(const StaticContext::Ptr &context)
{
    const Expression::Ptr me(FunctionCall::compress(context));
    if(me.data() != this)
        return me;

    if(m_operands.at(1)->is(IDStringValue))
    {
        const DynamicContext::Ptr dynContext(context->dynamicContext());

        m_pattern = parsePattern(m_operands.at(1)->evaluateSingleton(dynContext).stringValue(),
                                 dynContext);
        m_compiledParts |= OnlyPattern;
    }

    const Expression::Ptr flagOperand(m_operands.value(m_flagsPosition));

    if(!flagOperand)
    {
        m_flags = NoFlags;
        m_compiledParts |= OnlyFlags;
    }
    else if(flagOperand->is(IDStringValue))
    {
        const DynamicContext::Ptr dynContext(context->dynamicContext());
        m_flags = parseFlags(flagOperand->evaluateSingleton(dynContext).stringValue(),
                             dynContext);
        m_compiledParts |= OnlyFlags;
    }

    if(m_compiledParts == FlagsAndPattern)
        applyFlags(m_flags, m_pattern);

    return me;
}

QT_END_NAMESPACE
