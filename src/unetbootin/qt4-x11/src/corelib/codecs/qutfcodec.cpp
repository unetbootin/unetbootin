/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the QtCore module of the Qt Toolkit.
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

#include "qutfcodec_p.h"
#include "qlist.h"
#include "qendian.h"
#include "qchar.h"

#ifndef QT_NO_TEXTCODEC

QT_BEGIN_NAMESPACE

QUtf8Codec::~QUtf8Codec()
{
}

QByteArray QUtf8Codec::convertFromUnicode(const QChar *uc, int len, ConverterState *state) const
{
    uchar replacement = '?';
    int rlen = 3*len;
    int surrogate_high = -1;
    if (state) {
        if (state->flags & ConvertInvalidToNull)
            replacement = 0;
        if (!(state->flags & IgnoreHeader))
            rlen += 3;
        if (state->remainingChars)
            surrogate_high = state->state_data[0];
    }

    QByteArray rstr;
    rstr.resize(rlen);
    uchar* cursor = (uchar*)rstr.data();
    const QChar *ch = uc;
    int invalid = 0;
    if (state && !(state->flags & IgnoreHeader)) {
        *cursor++ = 0xef;
        *cursor++ = 0xbb;
        *cursor++ = 0xbf;
    }

    const QChar *end = ch + len;
    while (ch < end) {
        uint u = ch->unicode();
        if (surrogate_high >= 0) {
            if (u >= 0xdc00 && u < 0xe000) {
                u = (surrogate_high - 0xd800)*0x400 + (u - 0xdc00) + 0x10000;
                surrogate_high = -1;
            } else {
                // high surrogate without low
                *cursor = replacement;
                ++ch;
                ++invalid;
                surrogate_high = -1;
                continue;
            }
        } else if (u >= 0xdc00 && u < 0xe000) {
            // low surrogate without high
            *cursor = replacement;
            ++ch;
            ++invalid;
            continue;
        } else if (u >= 0xd800 && u < 0xdc00) {
            surrogate_high = u;
            ++ch;
            continue;
        }

        if (u < 0x80) {
            *cursor++ = (uchar)u;
        } else {
            if (u < 0x0800) {
                *cursor++ = 0xc0 | ((uchar) (u >> 6));
            } else {
                if (u > 0xffff) {
                    // see QString::fromUtf8() and QString::utf8() for explanations
                    if (u > 0x10fe00 && u < 0x10ff00) {
                        *cursor++ = (u - 0x10fe00);
                        ++ch;
                        continue;
                    } else {
                        *cursor++ = 0xf0 | ((uchar) (u >> 18));
                        *cursor++ = 0x80 | (((uchar) (u >> 12)) & 0x3f);
                    }
                } else {
                    *cursor++ = 0xe0 | (((uchar) (u >> 12)) & 0x3f);
                }
                *cursor++ = 0x80 | (((uchar) (u >> 6)) & 0x3f);
            }
            *cursor++ = 0x80 | ((uchar) (u&0x3f));
        }
        ++ch;
    }

    rstr.resize(cursor - (const uchar*)rstr.constData());
    if (state) {
        state->invalidChars += invalid;
        state->flags |= IgnoreHeader;
        state->remainingChars = 0;
        if (surrogate_high >= 0) {
            state->remainingChars = 1;
            state->state_data[0] = surrogate_high;
        }
    }
    return rstr;
}

void QUtf8Codec::convertToUnicode(QString *target, const char *chars, int len, ConverterState *state) const
{
    bool headerdone = false;
    QChar replacement = QChar::ReplacementCharacter;
    int need = 0;
    int error = -1;
    uint uc = 0;
    uint min_uc = 0;
    if (state) {
        if (state->flags & IgnoreHeader)
            headerdone = true;
        if (state->flags & ConvertInvalidToNull)
            replacement = QChar::Null;
        need = state->remainingChars;
        if (need) {
            uc = state->state_data[0];
            min_uc = state->state_data[1];
        }
    }
    if (!headerdone && len > 3
        && (uchar)chars[0] == 0xef && (uchar)chars[1] == 0xbb && (uchar)chars[2] == 0xbf) {
        // starts with a byte order mark
        chars += 3;
        len -= 3;
        headerdone = true;
    }

    int originalLength = target->length();
    QString &result = *target;
    result.resize(originalLength + len + 1); // worst case
    QChar *qch = result.data() + originalLength;
    uchar ch;
    int invalid = 0;

    for (int i=0; i<len; i++) {
        ch = chars[i];
        if (need) {
            if ((ch&0xc0) == 0x80) {
                uc = (uc << 6) | (ch & 0x3f);
                need--;
                if (!need) {
                    if (uc > 0xffff && uc < 0x110000) {
                        // surrogate pair
                        uc -= 0x10000;
                        unsigned short high = uc/0x400 + 0xd800;
                        unsigned short low = uc%0x400 + 0xdc00;

                        // resize if necessary
                        long where = qch - result.unicode();
                        if (where + 2 >= result.length()) {
                            result.resize(where + 2);
                            qch = result.data() + where;
                        }

                        *qch++ = QChar(high);
                        *qch++ = QChar(low);
                    } else if ((uc < min_uc) || (uc >= 0xd800 && uc <= 0xdfff) || (uc >= 0xfffe)) {
                        // error
                        *qch++ = replacement;
                        ++invalid;
                    } else {
                        *qch++ = uc;
                    }
                }
            } else {
                // error
                i = error;
                *qch++ = replacement;
                ++invalid;
                need = 0;
            }
        } else {
            if (ch < 128) {
                *qch++ = QLatin1Char(ch);
            } else if ((ch & 0xe0) == 0xc0) {
                uc = ch & 0x1f;
                need = 1;
                error = i;
                min_uc = 0x80;
            } else if ((ch & 0xf0) == 0xe0) {
                uc = ch & 0x0f;
                need = 2;
                error = i;
                min_uc = 0x800;
            } else if ((ch&0xf8) == 0xf0) {
                uc = ch & 0x07;
                need = 3;
                error = i;
                min_uc = 0x10000;
            } else {
                // error
                *qch++ = replacement;
                ++invalid;
            }
        }
    }
    if (!state && need > 0) {
        // unterminated UTF sequence
        for (int i = error; i < len; ++i) {
            *qch++ = replacement;
            ++invalid;
        }
    }
    result.truncate(qch - result.unicode());
    if (state) {
        state->invalidChars += invalid;
        state->remainingChars = need;
        if (headerdone)
            state->flags |= IgnoreHeader;
        state->state_data[0] = need ? uc : 0;
        state->state_data[1] = need ? min_uc : 0;
    }
}

QString QUtf8Codec::convertToUnicode(const char *chars, int len, ConverterState *state) const
{
    QString result;
    convertToUnicode(&result, chars, len, state);
    return result;
}

QByteArray QUtf8Codec::name() const
{
    return "UTF-8";
}

int QUtf8Codec::mibEnum() const
{
    return 106;
}

enum { Endian = 0, Data = 1 };

QUtf16Codec::~QUtf16Codec()
{
}

QByteArray QUtf16Codec::convertFromUnicode(const QChar *uc, int len, ConverterState *state) const
{
    Endianness endian = e;
    int length =  2*len;
    if (!state || (!(state->flags & IgnoreHeader))) {
        length += 2;
    }
    if (e == Detect) {
        endian = (QSysInfo::ByteOrder == QSysInfo::BigEndian) ? BE : LE;
    }

    QByteArray d;
    d.resize(length);
    char *data = d.data();
    if (!state || !(state->flags & IgnoreHeader)) {
        QChar bom(QChar::ByteOrderMark);
        if (endian == BE) {
            data[0] = bom.row();
            data[1] = bom.cell();
        } else {
            data[0] = bom.cell();
            data[1] = bom.row();
        }
        data += 2;
    }
    if (endian == BE) {
        for (int i = 0; i < len; ++i) {
            *(data++) = uc[i].row();
            *(data++) = uc[i].cell();
        }
    } else {
        for (int i = 0; i < len; ++i) {
            *(data++) = uc[i].cell();
            *(data++) = uc[i].row();
        }
    }

    if (state) {
        state->remainingChars = 0;
        state->flags |= IgnoreHeader;
    }
    return d;
}

QString QUtf16Codec::convertToUnicode(const char *chars, int len, ConverterState *state) const
{
    Endianness endian = e;
    bool half = false;
    uchar buf = 0;
    bool headerdone = false;
    if (state) {
        headerdone = state->flags & IgnoreHeader;
        if (endian == Detect)
            endian = (Endianness)state->state_data[Endian];
        if (state->remainingChars) {
            half = true;
            buf = state->state_data[Data];
        }
    }
    if (headerdone && endian == Detect)
        endian = (QSysInfo::ByteOrder == QSysInfo::BigEndian) ? BE : LE;

    QString result;
    result.resize(len); // worst case
    QChar *qch = (QChar *)result.unicode();
    while (len--) {
        if (half) {
            QChar ch;
            if (endian == LE) {
                ch.setRow(*chars++);
                ch.setCell(buf);
            } else {
                ch.setRow(buf);
                ch.setCell(*chars++);
            }
            if (!headerdone) {
                if (endian == Detect) {
                    if (ch == QChar::ByteOrderSwapped && endian != BE) {
                        endian = LE;
                    } else if (ch == QChar::ByteOrderMark && endian != LE) {
                        // ignore BOM
                        endian = BE;
                    } else {
                        if (QSysInfo::ByteOrder == QSysInfo::BigEndian) {
                            endian = BE;
                        } else {
                            endian = LE;
                            ch = QChar((ch.unicode() >> 8) | ((ch.unicode() & 0xff) << 8));
                        }
                        *qch++ = ch;
                    }
                } else if (ch != QChar::ByteOrderMark) {
                    *qch++ = ch;
                }
                headerdone = true;
            } else {
                *qch++ = ch;
            }
            half = false;
        } else {
            buf = *chars++;
            half = true;
        }
    }
    result.truncate(qch - result.unicode());

    if (state) {
        if (endian != Detect)
            state->flags |= IgnoreHeader;
        state->state_data[Endian] = endian;
        if (half) {
            state->remainingChars = 1;
            state->state_data[Data] = buf;
        } else {
            state->remainingChars = 0;
            state->state_data[Data] = 0;
        }
    }
    return result;
}

int QUtf16Codec::mibEnum() const
{
    return 1015;
}

QByteArray QUtf16Codec::name() const
{
    return "UTF-16";
}

QList<QByteArray> QUtf16Codec::aliases() const
{
    QList<QByteArray> list;
    list << "ISO-10646-UCS-2";
    return list;
}

int QUtf16BECodec::mibEnum() const
{
    return 1013;
}

QByteArray QUtf16BECodec::name() const
{
    return "UTF-16BE";
}

QList<QByteArray> QUtf16BECodec::aliases() const
{
    QList<QByteArray> list;
    return list;
}

int QUtf16LECodec::mibEnum() const
{
    return 1014;
}

QByteArray QUtf16LECodec::name() const
{
    return "UTF-16LE";
}

QList<QByteArray> QUtf16LECodec::aliases() const
{
    QList<QByteArray> list;
    return list;
}

QUtf32Codec::~QUtf32Codec()
{
}

QByteArray QUtf32Codec::convertFromUnicode(const QChar *uc, int len, ConverterState *state) const
{
    Endianness endian = e;
    int length =  4*len;
    if (!state || (!(state->flags & IgnoreHeader))) {
        length += 4;
    }
    if (e == Detect) {
        endian = (QSysInfo::ByteOrder == QSysInfo::BigEndian) ? BE : LE;
    }

    QByteArray d;
    d.resize(length);
    char *data = d.data();
    if (!state || !(state->flags & IgnoreHeader)) {
        if (endian == BE) {
            data[0] = 0;
            data[1] = 0;
            data[2] = (char)0xfe;
            data[3] = (char)0xff;
        } else {
            data[0] = (char)0xff;
            data[1] = (char)0xfe;
            data[2] = 0;
            data[3] = 0;
        }
        data += 2;
    }
    if (endian == BE) {
        for (int i = 0; i < len; ++i) {
            uint cp = uc[i].unicode();
            if (uc[i].isHighSurrogate() && i < len - 1)
                cp = QChar::surrogateToUcs4(cp, uc[++i].unicode());
            *(data++) = cp >> 24;
            *(data++) = (cp >> 16) & 0xff;
            *(data++) = (cp >> 8) & 0xff;
            *(data++) = cp & 0xff;
        }
    } else {
        for (int i = 0; i < len; ++i) {
            uint cp = uc[i].unicode();
            if (uc[i].isHighSurrogate() && i < len - 1)
                cp = QChar::surrogateToUcs4(cp, uc[++i].unicode());
            *(data++) = cp & 0xff;
            *(data++) = (cp >> 8) & 0xff;
            *(data++) = (cp >> 16) & 0xff;
            *(data++) = cp >> 24;
        }
    }

    if (state) {
        state->remainingChars = 0;
        state->flags |= IgnoreHeader;
    }
    return d;
}

QString QUtf32Codec::convertToUnicode(const char *chars, int len, ConverterState *state) const
{
    Endianness endian = e;
    uchar tuple[4];
    int num = 0;
    bool headerdone = false;
    if (state) {
        headerdone = state->flags & IgnoreHeader;
        if (endian == Detect) {
            endian = (Endianness)state->state_data[Endian];
        }
        num = state->remainingChars;
        memcpy(tuple, &state->state_data[Data], 4);
    }
    if (headerdone && endian == Detect)
        endian = (QSysInfo::ByteOrder == QSysInfo::BigEndian) ? BE : LE;

    QString result;
    result.resize((num + len) >> 2 << 1); // worst case
    QChar *qch = (QChar *)result.unicode();
    
    const char *end = chars + len;
    while (chars < end) {
        tuple[num++] = *chars++;
        if (num == 4) {
            if (!headerdone) {
                if (endian == Detect) {
                    if (endian == Detect) {
                        if (tuple[0] == 0xff && tuple[1] == 0xfe && tuple[2] == 0 && tuple[3] == 0 && endian != BE) {
                            endian = LE;
                            num = 0;
                            continue;
                        } else if (tuple[0] == 0 && tuple[1] == 0 && tuple[2] == 0xfe && tuple[3] == 0xff && endian != LE) {
                            endian = BE;
                            num = 0;
                            continue;
                        } else if (QSysInfo::ByteOrder == QSysInfo::BigEndian) {
                            endian = BE;
                        } else {
                            endian = LE;
                        }
                    }
                } else if (((endian == BE) ? qFromBigEndian<quint32>(tuple) : qFromLittleEndian<quint32>(tuple)) == QChar::ByteOrderMark) {
                    num = 0;
                    continue;
                }
            }
            uint code = (endian == BE) ? qFromBigEndian<quint32>(tuple) : qFromLittleEndian<quint32>(tuple);
            if (code >= 0x10000) {
                *qch++ = QChar::highSurrogate(code);
                *qch++ = QChar::lowSurrogate(code);
            } else {
                *qch++ = code;
            }
            num = 0;
        }
    }
    result.truncate(qch - result.unicode());
    
    if (state) {
        if (endian != Detect)
            state->flags |= IgnoreHeader;
        state->state_data[Endian] = endian;
        state->remainingChars = num;
        memcpy(&state->state_data[Data], tuple, 4);
    }
    return result;
}

int QUtf32Codec::mibEnum() const
{
    return 1017;
}

QByteArray QUtf32Codec::name() const
{
    return "UTF-32";
}

QList<QByteArray> QUtf32Codec::aliases() const
{
    QList<QByteArray> list;
    return list;
}

int QUtf32BECodec::mibEnum() const
{
    return 1018;
}

QByteArray QUtf32BECodec::name() const
{
    return "UTF-32BE";
}

QList<QByteArray> QUtf32BECodec::aliases() const
{
    QList<QByteArray> list;
    return list;
}

int QUtf32LECodec::mibEnum() const
{
    return 1019;
}

QByteArray QUtf32LECodec::name() const
{
    return "UTF-32LE";
}

QList<QByteArray> QUtf32LECodec::aliases() const
{
    QList<QByteArray> list;
    return list;
}


QT_END_NAMESPACE

#endif //QT_NO_TEXTCODEC
