/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the plugins of the Qt Toolkit.
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

// Most of the code here was originally written by Serika Kurusugawa
// a.k.a. Junji Takagi, and is included in Qt with the author's permission,
// and the grateful thanks of the Trolltech team.

/*! \class QSjisCodec qsjiscodec.h
    \reentrant
    \internal
*/

#include "qsjiscodec.h"
#include "qlist.h"

QT_BEGIN_NAMESPACE

#ifndef QT_NO_TEXTCODEC
enum {
    Esc = 0x1b
};

#define        IsKana(c)        (((c) >= 0xa1) && ((c) <= 0xdf))
#define        IsSjisChar1(c)        ((((c) >= 0x81) && ((c) <= 0x9f)) ||        \
                         (((c) >= 0xe0) && ((c) <= 0xfc)))
#define        IsSjisChar2(c)        (((c) >= 0x40) && ((c) != 0x7f) && ((c) <= 0xfc))
#define        IsUserDefinedChar1(c)        (((c) >= 0xf0) && ((c) <= 0xfc))

#define        QValidChar(u)        ((u) ? QChar((ushort)(u)) : QChar(QChar::ReplacementCharacter))

/*!
  Creates a Shift-JIS codec. Note that this is done automatically by
  the QApplication, you do not need construct your own.
*/
QSjisCodec::QSjisCodec() : conv(QJpUnicodeConv::newConverter(QJpUnicodeConv::Default))
{
}


/*!
  Destroys the Shift-JIS codec.
*/
QSjisCodec::~QSjisCodec()
{
    delete (QJpUnicodeConv*)conv;
    conv = 0;
}


QByteArray QSjisCodec::convertFromUnicode(const QChar *uc, int len, ConverterState *state) const
{
    char replacement = '?';
    if (state) {
        if (state->flags & ConvertInvalidToNull)
            replacement = 0;
    }
    int invalid = 0;

    int rlen = 2*len + 1;
    QByteArray rstr;
    rstr.resize(rlen);
    uchar* cursor = (uchar*)rstr.data();
    for (int i = 0; i < len; i++) {
        QChar ch = uc[i];
        uint j;
        if (ch.row() == 0x00 && ch.cell() < 0x80) {
            // ASCII
            *cursor++ = ch.cell();
        } else if ((j = conv->unicodeToJisx0201(ch.row(), ch.cell())) != 0) {
            // JIS X 0201 Latin or JIS X 0201 Kana
            *cursor++ = j;
        } else if ((j = conv->unicodeToSjis(ch.row(), ch.cell())) != 0) {
            // JIS X 0208
            *cursor++ = (j >> 8);
            *cursor++ = (j & 0xff);
        } else if ((j = conv->unicodeToSjisibmvdc(ch.row(), ch.cell())) != 0) {
            // JIS X 0208 IBM VDC
            *cursor++ = (j >> 8);
            *cursor++ = (j & 0xff);
        } else if ((j = conv->unicodeToCp932(ch.row(), ch.cell())) != 0) {
            // CP932 (for lead bytes 87, ee & ed)
            *cursor++ = (j >> 8);
            *cursor++ = (j & 0xff);
        } else if ((j = conv->unicodeToJisx0212(ch.row(), ch.cell())) != 0) {
            // JIS X 0212 (can't be encoded in ShiftJIS !)
            *cursor++ = 0x81;        // white square
            *cursor++ = 0xa0;        // white square
        } else {
            // Error
            *cursor++ = replacement;
            ++invalid;
        }
    }
    rstr.resize(cursor - (const uchar*)rstr.constData());

    if (state) {
        state->invalidChars += invalid;
    }
    return rstr;
}

QString QSjisCodec::convertToUnicode(const char* chars, int len, ConverterState *state) const
{
    uchar buf[1] = {0};
    int nbuf = 0;
    QChar replacement = QChar::ReplacementCharacter;
    if (state) {
        if (state->flags & ConvertInvalidToNull)
            replacement = QChar::Null;
        nbuf = state->remainingChars;
        buf[0] = state->state_data[0];
    }
    int invalid = 0;
    uint u= 0;
    QString result;
    for (int i=0; i<len; i++) {
        uchar ch = chars[i];
        switch (nbuf) {
        case 0:
            if (ch < 0x80 || IsKana(ch)) {
                // JIS X 0201 Latin or JIS X 0201 Kana
                u = conv->jisx0201ToUnicode(ch);
                result += QValidChar(u);
            } else if (IsSjisChar1(ch)) {
                // JIS X 0208
                buf[0] = ch;
                nbuf = 1;
            } else {
                // Invalid
                result += replacement;
                ++invalid;
            }
            break;
        case 1:
            // JIS X 0208
            if (IsSjisChar2(ch)) {
                if ((u = conv->sjisibmvdcToUnicode(buf[0], ch))) {
                    result += QValidChar(u);
                } else if ((u = conv->cp932ToUnicode(buf[0], ch))) {
                    result += QValidChar(u);
                }
                else if (IsUserDefinedChar1(buf[0])) {
                    result += QChar::ReplacementCharacter;
                } else {
                    u = conv->sjisToUnicode(buf[0], ch);
                    result += QValidChar(u);
                }
            } else {
                // Invalid
                result += replacement;
                ++invalid;
            }
            nbuf = 0;
            break;
        }
    }

    if (state) {
        state->remainingChars = nbuf;
        state->state_data[0] = buf[0];
        state->invalidChars += invalid;
    }
    return result;
}


int QSjisCodec::_mibEnum()
{
    return 17;
}

QByteArray QSjisCodec::_name()
{
    return "Shift_JIS";
}

/*!
    Returns the codec's mime name.
*/
QList<QByteArray> QSjisCodec::_aliases()
{
    QList<QByteArray> list;
    list << "SJIS" // Qt 3 compat
         << "MS_Kanji";
    return list;
}
#endif // QT_NO_TEXTCODEC

QT_END_NAMESPACE
