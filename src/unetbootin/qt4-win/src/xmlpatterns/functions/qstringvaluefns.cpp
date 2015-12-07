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

#include "qabstractfloat_p.h"
#include "qcommonvalues_p.h"
#include "qinteger_p.h"
#include "qpatternistlocale_p.h"
#include "qnumeric_p.h"
#include "qatomicstring_p.h"

#include "qstringvaluefns_p.h"

QT_BEGIN_NAMESPACE

using namespace QPatternist;

Item ConcatFN::evaluateSingleton(const DynamicContext::Ptr &context) const
{
    const Expression::List::const_iterator end(m_operands.constEnd());
    Expression::List::const_iterator it(m_operands.constBegin());
    QString result;

    for(; it != end; ++it)
    {
        Item item((*it)->evaluateSingleton(context));

        if(item)
            result += item.stringValue();
    }

    return AtomicString::fromValue(result);
}

Item StringJoinFN::evaluateSingleton(const DynamicContext::Ptr &context) const
{
    Item::Iterator::Ptr it(m_operands.first()->evaluateSequence(context));
    Q_ASSERT(it);
    Item current(it->next());

    if(!current) /* Exit early, don't evaluate the separator. */
        return CommonValues::EmptyString;

    QString result;
    QString separator;
    const Item isep(m_operands.at(1)->evaluateSingleton(context));

    if(isep)
        separator = isep.stringValue();

    while(true)
    {
        result += current.stringValue();
        current = it->next();

        if(!current)
            break;

        result += separator;
    }

    return result.isEmpty()
            ? toItem(CommonValues::EmptyString)
            : toItem(AtomicString::fromValue(result));
}

Item SubstringFN::evaluateSingleton(const DynamicContext::Ptr &context) const
{
    Item item(m_operands.first()->evaluateSingleton(context));

    if(!item)
        return CommonValues::EmptyString;

    const QString str(item.stringValue());

    const xsDouble dblStart = m_operands.at(1)->evaluateSingleton(context).as<Numeric>()
                                        ->round()->toDouble();
    if(qIsNaN(dblStart))
        return CommonValues::EmptyString;

    /* XPath starts from 1, but C++ starts from 0. */
    xsInteger startingLoc = Double::fromValue(dblStart)->round()->toInteger() - 1;

    xsInteger length = 0;
    if(m_operands.count() == 2)
        length = str.length() - startingLoc;
    else
    {
        const xsDouble dblLen = m_operands.at(2)->evaluateSingleton(context).as<Numeric>()
                ->round()->toDouble();

        if(qIsNaN(dblLen))
            return CommonValues::EmptyString;

        length = Double::fromValue(dblLen)->round()->toInteger();
        if(startingLoc > startingLoc + length)
            return CommonValues::EmptyString;
    }

    if(startingLoc < 0)
    {
        length = length + startingLoc;
        startingLoc = 0;
    }

    return AtomicString::fromValue(str.mid(startingLoc, length));
}

Item StringLengthFN::evaluateSingleton(const DynamicContext::Ptr &context) const
{
    const Item item(m_operands.first()->evaluateSingleton(context));

    /* fn:string() is re-implemented "inline" here. */
    if(item)
        return Integer::fromValue(item.stringValue().length());
    else
        return CommonValues::IntegerZero;
}

NormalizeUnicodeFN::NormalizeUnicodeFN() : m_normForm(QString::NormalizationForm_C)
{
}

Item NormalizeSpaceFN::evaluateSingleton(const DynamicContext::Ptr &context) const
{
    const Item arg(m_operands.first()->evaluateSingleton(context));

    if(!arg)
        return CommonValues::EmptyString;

    return toItem(AtomicString::fromValue(arg.stringValue().simplified()));
}

Item NormalizeUnicodeFN::evaluateSingleton(const DynamicContext::Ptr &context) const
{
    const Item arg(m_operands.first()->evaluateSingleton(context));

    if(!arg)
        return CommonValues::EmptyString;

    int normForm;

    /* The second argument has been removed, if we've already determined the form. */
    if(m_operands.count() == 1)
        normForm = m_normForm;
    else
    {
        normForm = determineNormalizationForm(context);
        if(normForm == -1)
            return toItem(AtomicString::fromValue(arg.stringValue()));
    }

    return AtomicString::fromValue(arg.stringValue().normalized(
            static_cast<QString::NormalizationForm>(normForm)));
}

Expression::Ptr NormalizeUnicodeFN::compress(const StaticContext::Ptr &context)
{
    const Expression::Ptr me(FunctionCall::compress(context));
    if(me.data() != this)
        return me;

    Q_ASSERT(m_operands.count() == 1 || m_operands.count() == 2);

    if(m_operands.count() == 1)
        m_normForm = QString::NormalizationForm_C;
    else if(m_operands.last()->is(IDStringValue))
    {
        m_normForm = static_cast<QString::NormalizationForm>(
                determineNormalizationForm(context->dynamicContext()));

        if(m_normForm == -1)
            return m_operands.first();

        /* Remove the operand since we don't need it anymore. */
        m_operands.removeLast();
    }

    return me;
}

int NormalizeUnicodeFN::determineNormalizationForm(const DynamicContext::Ptr &context) const
{
    const QString strRepr(m_operands.last()->evaluateSingleton(context).stringValue().trimmed().toUpper());

    /* TODO. Put these values in a QHash for faster lookup. Keep thread safety in mind. */
    if(strRepr.isEmpty())
        return -1;
    else if(strRepr == QLatin1String("NFC"))
        return QString::NormalizationForm_C;
    else if(strRepr == QLatin1String("NFD"))
        return QString::NormalizationForm_D;
    else if(strRepr == QLatin1String("NFKC"))
        return QString::NormalizationForm_KC;
    else if(strRepr == QLatin1String("NFKD"))
        return QString::NormalizationForm_KD;
    else
    {
        /* What form is FULLY_NORMALIZED? Is a code path available for that somewhere? */
        context->error(QtXmlPatterns::tr("The normalization form %1 is "
                                         "unsupported. The supported forms are "
                                         "%2, %3, %4, and %5, and none, i.e. "
                                         "the empty string (no normalization).")
                                        .arg(formatKeyword(strRepr))
                                        .arg(formatKeyword("NFC"))
                                        .arg(formatKeyword("NFD"))
                                        .arg(formatKeyword("NFKC"))
                                        .arg(formatKeyword("NFKD")),
                                   ReportContext::FOCH0003,
                                   this);
        return QString::NormalizationForm_C; /* Silence compiler warning. */
    }
}

Item UpperCaseFN::evaluateSingleton(const DynamicContext::Ptr &context) const
{
    const Item item(m_operands.first()->evaluateSingleton(context));

    if(!item)
        return CommonValues::EmptyString;

    return AtomicString::fromValue(item.stringValue().toUpper());
}

Item LowerCaseFN::evaluateSingleton(const DynamicContext::Ptr &context) const
{
    const Item item(m_operands.first()->evaluateSingleton(context));

    if(!item)
        return CommonValues::EmptyString;

    return AtomicString::fromValue(item.stringValue().toLower());
}

Item TranslateFN::evaluateSingleton(const DynamicContext::Ptr &context) const
{
    const Item item(m_operands.first()->evaluateSingleton(context));

    if(!item)
        return CommonValues::EmptyString;

    const QString mapString(m_operands.at(1)->evaluateSingleton(context).stringValue());
    const QString arg(item.stringValue());

    if(mapString.isEmpty())
        return AtomicString::fromValue(arg);

    const QString transString(m_operands.at(2)->evaluateSingleton(context).stringValue());
    const int transLen = transString.length();
    const int argLen = arg.length();

    QString result;
    result.reserve(argLen);
    int outI = 0;

    for(int i = 0; i < argLen; ++i)
    {
        const QChar argCh(arg.at(i));
        const int mapPos = mapString.indexOf(argCh);

        if(mapPos == -1)
        {
            result[outI] = argCh;
            ++outI;
            continue;
        }
        else if(mapPos >= transLen)
            continue;

        const QChar transCh(transString.at(mapPos));

        if(transCh.isNull())
            continue;

        result[outI] = transCh;
        ++outI;
    }

    result.truncate(outI);
    return AtomicString::fromValue(result);
}

EncodeString::EncodeString(const QByteArray &excludeChars,
                           const QByteArray &includeChars) : m_excludeChars(excludeChars),
                                                             m_includeChars(includeChars)
{
}

Item EncodeString::evaluateSingleton(const DynamicContext::Ptr &context) const
{
    const Item item(m_operands.first()->evaluateSingleton(context));

    if(!item)
        return CommonValues::EmptyString;

    return AtomicString::fromValue(QString::fromAscii(QUrl::toPercentEncoding(item.stringValue(),
                                                                        m_excludeChars,
                                                                        m_includeChars).constData()));
}

const char *const EncodeForURIFN::include = "#!*'()";

EncodeForURIFN::EncodeForURIFN() : EncodeString(QByteArray(), QByteArray::fromRawData(include, 6))
{
}

const char *const IriToURIFN::exclude = "#-_!~*'();?@&=+$,[]/:%";

IriToURIFN::IriToURIFN() : EncodeString(QByteArray::fromRawData(exclude, 22), QByteArray())
{
}

const char *const EscapeHtmlURIFN::include = "?&[]%";
const char *const EscapeHtmlURIFN::exclude = " :;=@!./+*()-,#$'";

EscapeHtmlURIFN::EscapeHtmlURIFN() : EncodeString(QByteArray::fromRawData(exclude, 17),
                                                  QByteArray::fromRawData(include, 6))
{
}

QT_END_NAMESPACE
