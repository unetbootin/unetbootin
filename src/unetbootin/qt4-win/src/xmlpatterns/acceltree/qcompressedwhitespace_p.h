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

#ifndef Patternist_CompressedWhitespace_H
#define Patternist_CompressedWhitespace_H

#include <QtGlobal>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

class QChar;
class QString;
class QStringRef;

namespace QPatternist
{
    /**
     * @short A compression facility for whitespace nodes.
     *
     * CompressedWhitespace compresses and decompresses strings that consists of
     * whitespace only, and do so with a scheme that is designed to do this
     * specialized task in an efficient way. The approach is simple: each
     * sequence of equal whitespace in the input gets coded into one byte,
     * where the first two bits signals the type, CharIdentifier, and the
     * remininding six bits is the count.
     *
     * For instance, this scheme manages to compress a sequence of spaces
     * followed by a new line into 16 bits(one QChar), and QString stores
     * strings of one QChar quite efficiently, by avoiding a heap allocation.
     *
     * There is no way to tell whether a QString is compressed or not.
     *
     * The compression scheme originates from Saxon, by Michael Kay.
     *
     * @author Frans Englich <fenglich@trolltech.com>
     */
    class CompressedWhitespace
    {
        public:
            /**
             * @short Compresses @p input into a compressed format, returned
             * as a QString.
             *
             * The caller guarantees that input is not empty
             * and consists only of whitespace.
             *
             * The returned format is opaque. There is no way to find out
             * whether a QString contains compressed data or not.
             *
             * @see decompress()
             */
            static QString compress(const QStringRef &input);

            /**
             * @short Decompresses @p input into a usual QString.
             *
             * @p input must be a QString as per returned from compress().
             *
             * @see compress()
             */
            static QString decompress(const QString &input);

        private:
            /**
             * We use the two upper bits for communicating what space it is.
             */
            enum CharIdentifier
            {
                Space   = 0x0,

                /**
                 * 0xA, \\r
                 *
                 * Binary: 10000000
                 */
                CR      = 0x80,

                /**
                 * 0xD, \\n
                 *
                 * Binary: 01000000
                 */
                LF      = 0x40,

                /**
                 * Binary: 11000000
                 */
                Tab     = 0xC0
            };

            enum Constants
            {
                /* We can at maximum store this many consecutive characters
                 * of one type. We use 6 bits for the count. */
                MaxCharCount = (1 << 6) - 1,

                /**
                 * Binary: 11111111
                 */
                Lower8Bits = (1 << 8) - 1,

                /**
                 * Binary: 111111
                 */
                Lower6Bits = (1 << 6) - 1,

                /*
                 * Binary: 11000000
                 */
                UpperTwoBits = 3 << 6
            };

            static inline CharIdentifier toIdentifier(const QChar ch);

            static inline quint8 toCompressedChar(const QChar ch, const int len);
            static inline QChar toChar(const CharIdentifier id);

            /**
             * @short Returns @c true if @p number is an even number, otherwise
             * @c false.
             */
            static inline bool isEven(const int number);

            /**
             * @short This class can only be used via its static members.
             */
            inline CompressedWhitespace();
            Q_DISABLE_COPY(CompressedWhitespace)
    };
}

QT_END_NAMESPACE

QT_END_HEADER

#endif
