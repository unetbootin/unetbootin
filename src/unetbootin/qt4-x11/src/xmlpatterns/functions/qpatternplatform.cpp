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
    if(m_compiledParts.testFlag(FlagsPrecompiled))
        flags = m_flags;
    else
    {
        const Expression::Ptr flagsOp(m_operands.value(m_flagsPosition));

        if(flagsOp)
            flags = parseFlags(flagsOp->evaluateSingleton(context).stringValue(), context);
        else
            flags = NoFlags;
    }

    /* Compile the pattern, if necessary. */
    if(m_compiledParts.testFlag(PatternPrecompiled))
        retvalPattern = m_pattern;
    else
    {
        retvalPattern = parsePattern(m_operands.at(1)->evaluateSingleton(context).stringValue(),
                                     context);

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

    QString rewrittenPattern(patternP);

    /* We rewrite some well known patterns to QRegExp style here. Note that
     * these character classes only works in the ASCII range, and fail for
     * others. This support needs to be in QRegExp, since it's about checking
     * QChar::category(). */
    rewrittenPattern.replace(QLatin1String("[\\i-[:]]"), QLatin1String("[a-zA-Z_]"));
    rewrittenPattern.replace(QLatin1String("[\\c-[:]]"), QLatin1String("[a-zA-Z0-9_\\-\\.]"));
    rewrittenPattern.replace(QLatin1String("\\i"), QLatin1String("[a-zA-Z:_]"));
    rewrittenPattern.replace(QLatin1String("\\c"), QLatin1String("[a-zA-Z0-9:_\\-\\.]"));
    rewrittenPattern.replace(QLatin1String("\\p{L}"), QLatin1String("[a-zA-Z]"));
    rewrittenPattern.replace(QLatin1String("\\p{Lu}"), QLatin1String("[A-Z]"));
    rewrittenPattern.replace(QLatin1String("\\p{Ll}"), QLatin1String("[a-z]"));
    rewrittenPattern.replace(QLatin1String("\\p{Nd}"), QLatin1String("[0-9]"));

    QRegExp retval(rewrittenPattern);

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
    if(me != this)
        return me;

    if(m_operands.at(1)->is(IDStringValue))
    {
        const DynamicContext::Ptr dynContext(context->dynamicContext());

        m_pattern = parsePattern(m_operands.at(1)->evaluateSingleton(dynContext).stringValue(),
                                 dynContext);
        m_compiledParts |= PatternPrecompiled;
    }

    const Expression::Ptr flagOperand(m_operands.value(m_flagsPosition));

    if(!flagOperand)
    {
        m_flags = NoFlags;
        m_compiledParts |= FlagsPrecompiled;
    }
    else if(flagOperand->is(IDStringValue))
    {
        const DynamicContext::Ptr dynContext(context->dynamicContext());
        m_flags = parseFlags(flagOperand->evaluateSingleton(dynContext).stringValue(),
                             dynContext);
        m_compiledParts |= FlagsPrecompiled;
    }

    if(m_compiledParts == FlagsAndPattern)
        applyFlags(m_flags, m_pattern);

    return me;
}

QT_END_NAMESPACE
