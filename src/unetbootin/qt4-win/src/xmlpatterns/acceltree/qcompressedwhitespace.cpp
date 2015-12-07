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

#include <QString>

#include "qcompressedwhitespace_p.h"

QT_BEGIN_NAMESPACE

using namespace QPatternist;

CompressedWhitespace::CharIdentifier CompressedWhitespace::toIdentifier(const QChar ch)
{
    switch(ch.unicode())
    {
        case ' ':
            return Space;
        case '\n':
            return LF;
        case '\r':
            return CR;
        case '\t':
            return Tab;
        default:
        {
            Q_ASSERT_X(false, Q_FUNC_INFO,
                       "The caller must guarantee only whitespace is passed.");
            return Tab;
        }
    }
}

bool CompressedWhitespace::isEven(const int number)
{
    Q_ASSERT(number >= 0);
    return number % 2 == 0;
}

quint8 CompressedWhitespace::toCompressedChar(const QChar ch, const int len)
{
    Q_ASSERT(len > 0);
    Q_ASSERT(len <= MaxCharCount);

    return len + toIdentifier(ch);
}

QChar CompressedWhitespace::toChar(const CharIdentifier id)
{
    switch(id)
    {
        case Space: return QLatin1Char(' ');
        case CR:    return QLatin1Char('\r');
        case LF:    return QLatin1Char('\n');
        case Tab:   return QLatin1Char('\t');
        default:
                    {
                        Q_ASSERT_X(false, Q_FUNC_INFO, "Unexpected input");
                        return QChar();
                    }
    }
}

QString CompressedWhitespace::compress(const QStringRef &input)
{
    Q_ASSERT(!isEven(1) && isEven(0) && isEven(2));
    Q_ASSERT(!input.isEmpty());

    QString result;
    const int len = input.length();

    /* The amount of compressed characters. For instance, if input is
     * four spaces followed by one tab, compressedChars will be 2, and the resulting
     * QString will have a length of 1, two compressedChars stored in one QChar. */
    int compressedChars = 0;

    for(int i = 0; i < len; ++i)
    {
        const QChar c(input.at(i));

        int start = i;

        while(true)
        {
            if(i + 1 == input.length() || input.at(i + 1) != c)
                break;
            else
                ++i;
        }

        /* The length of subsequent whitespace characters in the input. */
        int wsLen = (i - start) + 1;

        /* We might get a sequence of whitespace that is so long, that we can't
         * store it in one unit/byte. In that case we chop it into as many subsequent
         * ones that is needed. */
        while(true)
        {
            const int unitLength = qMin(wsLen, int(MaxCharCount));
            wsLen -= unitLength;

            ushort resultCP = toCompressedChar(c, unitLength);

            if(isEven(compressedChars))
                result += QChar(resultCP);
            else
            {
                resultCP = resultCP << 8;
                resultCP |= result.at(result.size() - 1).unicode();
                result[result.size() - 1] = resultCP;
            }

            ++compressedChars;

            if(wsLen == 0)
                break;
        }
    }

    return result;
}

QString CompressedWhitespace::decompress(const QString &input)
{
    Q_ASSERT(!input.isEmpty());
    const int len = input.length() * 2;
    QString retval;

    for(int i = 0; i < len; ++i)
    {
        ushort cp = input.at(i / 2).unicode();

        if(isEven(i))
            cp &= Lower8Bits;
        else
        {
            cp = cp >> 8;

            if(cp == 0)
                return retval;
        }

        const quint8 wsLen = cp & Lower6Bits;
        const quint8 id = cp & UpperTwoBits;

        /* Resize retval, and fill in on the top. */
        const int oldSize = retval.size();
        const int newSize = retval.size() + wsLen;
        retval.resize(newSize);
        const QChar ch(toChar(CharIdentifier(id)));

        for(int f = oldSize; f < newSize; ++f)
            retval[f] = ch;
    }

    return retval;
}

QT_END_NAMESPACE

