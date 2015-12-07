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

#include "qbytearraymatcher.h"

QT_BEGIN_NAMESPACE

static inline void bm_init_skiptable(const uchar *cc, int l, uint *skiptable)
{
    int i = 0;
    register uint *st = skiptable;
    while (i++ < 256 / 8) {
        *st++ = l; *st++ = l; *st++ = l; *st++ = l;
        *st++ = l; *st++ = l; *st++ = l; *st++ = l;
    }
    while (l--)
        skiptable[*cc++] = l;
}

static inline int bm_find(const uchar *cc, int l, int index, const uchar *puc, uint pl,
                          const uint *skiptable)
{
    if (pl == 0)
        return index > l ? -1 : index;
    const uint pl_minus_one = pl - 1;

    register const uchar *current = cc + index + pl_minus_one;
    const uchar *end = cc + l;
    while (current < end) {
        uint skip = skiptable[*current];
        if (!skip) {
            // possible match
            while (skip < pl) {
                if (*(current - skip) != puc[pl_minus_one - skip])
                    break;
                skip++;
            }
            if (skip > pl_minus_one) // we have a match
                return (current - cc) - skip + 1;

            // in case we don't have a match we are a bit inefficient as we only skip by one
            // when we have the non matching char in the string.
            if (skiptable[*(current - skip)] == pl)
                skip = pl - skip;
            else
                skip = 1;
        }
        if (current > end - skip)
            break;
        current += skip;
    }
    return -1; // not found
}

/*! \class QByteArrayMatcher
    \brief The QByteArrayMatcher class holds a sequence of bytes that
    can be quickly matched in a byte array.

    \ingroup tools
    \ingroup text

    This class is useful when you have a sequence of bytes that you
    want to repeatedly match against some byte arrays (perhaps in a
    loop), or when you want to search for the same sequence of bytes
    multiple times in the same byte array. Using a matcher object and
    indexIn() is faster than matching a plain QByteArray with
    QByteArray::indexOf() if repeated matching takes place. This
    class offers no benefit if you are doing one-off byte array
    matches.

    Create the QByteArrayMatcher with the QByteArray you want to
    search for. Then call indexIn() on the QByteArray that you want to
    search.

    \sa QByteArray, QStringMatcher
*/

/*!
    Constructs an empty byte array matcher that won't match anything.
    Call setPattern() to give it a pattern to match.
*/
QByteArrayMatcher::QByteArrayMatcher()
    : d(0)
{
    qMemSet(q_skiptable, 0, sizeof(q_skiptable));
}

/*!
    Constructs a byte array matcher that will search for \a pattern.
    Call indexIn() to perform a search.
*/
QByteArrayMatcher::QByteArrayMatcher(const QByteArray &pattern)
    : d(0)
{
    setPattern(pattern);
}

/*!
    Copies the \a other byte array matcher to this byte array matcher.
*/
QByteArrayMatcher::QByteArrayMatcher(const QByteArrayMatcher &other)
    : d(0)
{
    operator=(other);
}

/*!
    Destroys the byte array matcher.
*/
QByteArrayMatcher::~QByteArrayMatcher()
{
}

/*!
    Assigns the \a other byte array matcher to this byte array matcher.
*/
QByteArrayMatcher &QByteArrayMatcher::operator=(const QByteArrayMatcher &other)
{
    q_pattern = other.q_pattern;
    qMemCopy(q_skiptable, other.q_skiptable, sizeof(q_skiptable));
    return *this;
}

/*!
    Sets the byte array that this byte array matcher will search for
    to \a pattern.

    \sa pattern(), indexIn()
*/
void QByteArrayMatcher::setPattern(const QByteArray &pattern)
{
    bm_init_skiptable(reinterpret_cast<const uchar *>(pattern.constData()), pattern.size(),
                      q_skiptable);
    q_pattern = pattern;
}

/*!
    Searches the byte array \a ba, from byte position \a from (default
    0, i.e. from the first byte), for the byte array pattern() that
    was set in the constructor or in the most recent call to
    setPattern(). Returns the position where the pattern() matched in
    \a ba, or -1 if no match was found.
*/
int QByteArrayMatcher::indexIn(const QByteArray &ba, int from) const
{
    if (from < 0)
        from = 0;
    return bm_find(reinterpret_cast<const uchar *>(ba.constData()), ba.size(), from,
                   reinterpret_cast<const uchar *>(q_pattern.constData()), q_pattern.size(),
                   q_skiptable);
}

/*!
    \fn QByteArray QByteArrayMatcher::pattern() const

    Returns the byte array pattern that this byte array matcher will
    search for.

    \sa setPattern()
*/

QT_END_NAMESPACE
