/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the QtCore module of the Qt Toolkit.
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

#include "qstringmatcher.h"
#include "qunicodetables_p.h"

QT_BEGIN_NAMESPACE

static void bm_init_skiptable(const ushort *uc, int l, uint *skiptable, Qt::CaseSensitivity cs)
{
    int i = 0;
    register uint *st = skiptable;
    while (i++ < 256 / 8) {
        *st++ = l; *st++ = l; *st++ = l; *st++ = l;
        *st++ = l; *st++ = l; *st++ = l; *st++ = l;
    }
    if (cs == Qt::CaseSensitive) {
        while (l--) {
            skiptable[*uc & 0xff] = l;
            uc++;
        }
    } else {
        const ushort *start = uc;
        while (l--) {
            skiptable[foldCase(uc, start) & 0xff] = l;
            uc++;
        }
    }
}

static inline int bm_find(const ushort *uc, uint l, int index, const ushort *puc, uint pl,
                          const uint *skiptable, Qt::CaseSensitivity cs)
{
    if (pl == 0)
        return index > (int)l ? -1 : index;
    const uint pl_minus_one = pl - 1;

    register const ushort *current = uc + index + pl_minus_one;
    const ushort *end = uc + l;
    if (cs == Qt::CaseSensitive) {
        while (current < end) {
            uint skip = skiptable[*current & 0xff];
            if (!skip) {
                // possible match
                while (skip < pl) {
                    if (*(current - skip) != puc[pl_minus_one-skip])
                        break;
                    skip++;
                }
                if (skip > pl_minus_one) // we have a match
                    return (current - uc) - pl_minus_one;

                // in case we don't have a match we are a bit inefficient as we only skip by one
                // when we have the non matching char in the string.
                if (skiptable[*(current - skip) & 0xff] == pl)
                    skip = pl - skip;
                else
                    skip = 1;
            }
            if (current > end - skip)
                break;
            current += skip;
        }
    } else {
        while (current < end) {
            uint skip = skiptable[foldCase(current, uc) & 0xff];
            if (!skip) {
                // possible match
                while (skip < pl) {
                    if (foldCase(current - skip, uc) != foldCase(puc + pl_minus_one - skip, puc))
                        break;
                    skip++;
                }
                if (skip > pl_minus_one) // we have a match
                    return (current - uc) - pl_minus_one;
                // in case we don't have a match we are a bit inefficient as we only skip by one
                // when we have the non matching char in the string.
                if (skiptable[foldCase(current - skip, uc) & 0xff] == pl)
                    skip = pl - skip;
                else
                    skip = 1;
            }
            if (current > end - skip)
                break;
            current += skip;
        }
    }
    return -1; // not found
}

/*! 
    \class QStringMatcher
    \brief The QStringMatcher class holds a sequence of characters that
    can be quickly matched in a Unicode string.

    \ingroup tools
    \ingroup text

    This class is useful when you have a sequence of \l{QChar}s that
    you want to repeatedly match against some strings (perhaps in a
    loop), or when you want to search for the same sequence of
    characters multiple times in the same string. Using a matcher
    object and indexIn() is faster than matching a plain QString with
    QString::indexOf() if repeated matching takes place. This class
    offers no benefit if you are doing one-off string matches.

    Create the QStringMatcher with the QString you want to search
    for. Then call indexIn() on the QString that you want to search.

    \sa QString, QByteArrayMatcher, QRegExp
*/

/*!
    Constructs an empty string matcher that won't match anything.
    Call setPattern() to give it a pattern to match.
*/
QStringMatcher::QStringMatcher()
    : d_ptr(0)
{
    qMemSet(q_skiptable, 0, sizeof(q_skiptable));
}

/*!
    Constructs a string matcher that will search for \a pattern, with
    case sensitivity \a cs.

    Call indexIn() to perform a search.
*/
QStringMatcher::QStringMatcher(const QString &pattern, Qt::CaseSensitivity cs)
    : d_ptr(0), q_pattern(pattern), q_cs(cs)
{
    bm_init_skiptable((const ushort *)pattern.unicode(), pattern.size(), q_skiptable, cs);
}

/*!
    Copies the \a other string matcher to this string matcher.
*/
QStringMatcher::QStringMatcher(const QStringMatcher &other)
    : d_ptr(0)
{
    operator=(other);
}

/*!
    Destroys the string matcher.
*/
QStringMatcher::~QStringMatcher()
{
}

/*!
    Assigns the \a other string matcher to this string matcher.
*/
QStringMatcher &QStringMatcher::operator=(const QStringMatcher &other)
{
    q_pattern = other.q_pattern;
    q_cs = other.q_cs;
    qMemCopy(q_skiptable, other.q_skiptable, sizeof(q_skiptable));
    return *this;
}

/*!
    Sets the string that this string matcher will search for to \a
    pattern.

    \sa pattern(), setCaseSensitivity(), indexIn()
*/
void QStringMatcher::setPattern(const QString &pattern)
{
    bm_init_skiptable((const ushort *)pattern.unicode(), pattern.size(), q_skiptable, q_cs);
    q_pattern = pattern;
}

/*!
    Sets the case sensitivity setting of this string matcher to \a
    cs.

    \sa caseSensitivity(), setPattern(), indexIn()
*/
void QStringMatcher::setCaseSensitivity(Qt::CaseSensitivity cs)
{
    if (cs == q_cs)
        return;
    bm_init_skiptable((const ushort *)q_pattern.unicode(), q_pattern.size(), q_skiptable, cs);
    q_cs = cs;
}

/*!
    Searches the string \a str from character position \a from
    (default 0, i.e. from the first character), for the string
    pattern() that was set in the constructor or in the most recent
    call to setPattern(). Returns the position where the pattern()
    matched in \a str, or -1 if no match was found.

    \sa setPattern(), setCaseSensitivity()
*/
int QStringMatcher::indexIn(const QString &str, int from) const
{
    if (from < 0)
        from = 0;
    return bm_find((const ushort *)str.unicode(), str.size(), from,
                   (const ushort *)q_pattern.unicode(), q_pattern.size(),
                   q_skiptable, q_cs);
}

/*!
    \fn QString QStringMatcher::pattern() const

    Returns the string pattern that this string matcher will search
    for.

    \sa setPattern()
*/

/*!
    \fn Qt::CaseSensitivity QStringMatcher::caseSensitivity() const

    Returns the case sensitivity setting for this string matcher.

    \sa setCaseSensitivity()
*/

/*!
    \fn int qFindStringInStringBoyerMoore(
        const QChar *haystack, int haystackLen, int haystackOffset,
        const QChar *needle, int needleLen, Qt::CaseSensitivity cs)

    \internal
*/

int qFindStringBoyerMoore(
    const QChar *haystack, int haystackLen, int haystackOffset,
    const QChar *needle, int needleLen, Qt::CaseSensitivity cs)
{
    uint skiptable[256];
    bm_init_skiptable((const ushort *)needle, needleLen, skiptable, cs);
    if (haystackOffset < 0)
        haystackOffset = 0;
    return bm_find((const ushort *)haystack, haystackLen, haystackOffset,
                   (const ushort *)needle, needleLen, skiptable, cs);
}

QT_END_NAMESPACE
