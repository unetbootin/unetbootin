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

#include <QStringList>

#include "qboolean_p.h"
#include "qcommonvalues_p.h"
#include "qitemmappingiterator_p.h"
#include "qpatternistlocale_p.h"
#include "qatomicstring_p.h"

#include "qpatternmatchingfns_p.h"

QT_BEGIN_NAMESPACE

using namespace QPatternist;

MatchesFN::MatchesFN() : PatternPlatform(2)
{
}

Item MatchesFN::evaluateSingleton(const DynamicContext::Ptr &context) const
{
    const QRegExp regexp(pattern(context));
    QString input;

    const Item arg(m_operands.first()->evaluateSingleton(context));
    if(arg)
        input = arg.stringValue();

    return Boolean::fromValue(input.contains(regexp));
}

ReplaceFN::ReplaceFN() : PatternPlatform(3)
{
}

Item ReplaceFN::evaluateSingleton(const DynamicContext::Ptr &context) const
{
    const QRegExp regexp(pattern(context));
    QString input;

    const Item arg(m_operands.first()->evaluateSingleton(context));
    if(arg)
        input = arg.stringValue();

    const QString replacement(m_replacementString.isNull() ? parseReplacement(regexp.numCaptures(), context)
                                                           : m_replacementString);


    return AtomicString::fromValue(input.replace(regexp, replacement));
}

QString ReplaceFN::errorAtEnd(const char ch)
{
    return QtXmlPatterns::tr("%1 must be followed by %2 or %3, not at "
                             "the end of the replacement string.")
                             .arg(formatKeyword(QLatin1Char(ch)))
                             .arg(formatKeyword(QLatin1Char('\\')))
                             .arg(formatKeyword(QLatin1Char('$')));
}

QString ReplaceFN::parseReplacement(const int,
                                    const DynamicContext::Ptr &context) const
{
    // TODO what if there is no groups, can one rewrite to the replacement then?
    const QString input(m_operands.at(2)->evaluateSingleton(context).stringValue());

    QString retval;
    retval.reserve(input.size());
    const int len = input.length();

    for(int i = 0; i < len; ++i)
    {
        const QChar ch(input.at(i));
        switch(ch.toAscii())
        {
            case '$':
            {
                /* QRegExp uses '\' as opposed to '$' for marking sub groups. */
                retval.append(QLatin1Char('\\'));

                ++i;
                if(i == len)
                {
                    context->error(errorAtEnd('$'), ReportContext::FORX0004, this);
                    return QString();
                }

                const QChar nextCh(input.at(i));
                if(nextCh.isDigit())
                    retval.append(nextCh);
                else
                {
                    context->error(QtXmlPatterns::tr("In the replacement string, %1 must be "
                                                     "followed by at least one digit when not escaped.")
                                                     .arg(formatKeyword(QLatin1Char('$'))),
                                                ReportContext::FORX0004, this);
                    return QString();
                }

                break;
            }
            case '\\':
            {
                ++i;
                if(i == len)
                {
                    /* error, we've reached the end. */;
                    context->error(errorAtEnd('\\'), ReportContext::FORX0004, this);
                }

                const QChar nextCh(input.at(i));
                if(nextCh == QLatin1Char('\\') || nextCh == QLatin1Char('$'))
                {
                    retval.append(ch);
                    break;
                }
                else
                {
                    context->error(QtXmlPatterns::tr("In the replacement string, %1 can only be used to "
                                                     "escape itself or %2, not %3")
                                                     .arg(formatKeyword(QLatin1Char('\\')))
                                                     .arg(formatKeyword(QLatin1Char('$')))
                                                     .arg(formatKeyword(nextCh)),
                                               ReportContext::FORX0004, this);
                    return QString();
                }
            }
            default:
                retval.append(ch);
        }
    }

    return retval;
}

Expression::Ptr ReplaceFN::compress(const StaticContext::Ptr &context)
{
    const Expression::Ptr me(PatternPlatform::compress(context));

    if(me != this)
        return me;

    if(m_operands.at(2)->is(IDStringValue))
    {
        const int capt = captureCount();
        if(capt == -1)
            return me;
        else
            m_replacementString = parseReplacement(captureCount(), context->dynamicContext());
    }

    return me;
}

TokenizeFN::TokenizeFN() : PatternPlatform(2)
{
}

/**
 * Used by QAbstractXmlForwardIterator.
 */
static inline bool qIsForwardIteratorEnd(const QString &item)
{
    return item.isNull();
}

Item TokenizeFN::mapToItem(const QString &subject, const DynamicContext::Ptr &) const
{
    return AtomicString::fromValue(subject);
}

Item::Iterator::Ptr TokenizeFN::evaluateSequence(const DynamicContext::Ptr &context) const
{
    const Item arg(m_operands.first()->evaluateSingleton(context));
    if(!arg)
        return CommonValues::emptyIterator;

    const QString input(arg.stringValue());
    if(input.isEmpty())
        return CommonValues::emptyIterator;

    const QRegExp regExp(pattern(context));
    const QStringList result(input.split(regExp, QString::KeepEmptyParts));

    return makeItemMappingIterator<Item>(ConstPtr(this),
                                         makeListIterator(result),
                                         DynamicContext::Ptr());
}

QT_END_NAMESPACE
