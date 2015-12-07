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

#include "qlatincodec_p.h"
#include "qlist.h"

#ifndef QT_NO_TEXTCODEC

QT_BEGIN_NAMESPACE

QLatin1Codec::~QLatin1Codec()
{
}

QString QLatin1Codec::convertToUnicode(const char *chars, int len, ConverterState *) const
{
    if (chars == 0)
        return QString();

    return QString::fromLatin1(chars, len);
}


QByteArray QLatin1Codec::convertFromUnicode(const QChar *ch, int len, ConverterState *state) const
{
    const char replacement = (state && state->flags & ConvertInvalidToNull) ? 0 : '?';
    QByteArray r;
    r.resize(len);
    char *d = r.data();
    int invalid = 0;
    for (int i = 0; i < len; ++i) {
        if (ch[i] > 0xff) {
            d[i] = replacement;
            ++invalid;
        } else {
            d[i] = (char)ch[i].cell();
        }
    }
    if (state) {
        state->invalidChars += invalid;
    }
    return r;
}

QByteArray QLatin1Codec::name() const
{
    return "ISO-8859-1";
}

QList<QByteArray> QLatin1Codec::aliases() const
{
    QList<QByteArray> list;
    list << "latin1"
         << "CP819"
         << "IBM819"
         << "iso-ir-100"
         << "csISOLatin1";
    return list;
}


int QLatin1Codec::mibEnum() const
{
    return 4;
}


QLatin15Codec::~QLatin15Codec()
{
}

QString QLatin15Codec::convertToUnicode(const char* chars, int len, ConverterState *) const
{
    if (chars == 0)
        return QString();

    QString str = QString::fromLatin1(chars, len);
    QChar *uc = str.data();
    while(len--) {
        switch(uc->unicode()) {
            case 0xa4:
                *uc = 0x20ac;
                break;
            case 0xa6:
                *uc = 0x0160;
                break;
            case 0xa8:
                *uc = 0x0161;
                break;
            case 0xb4:
                *uc = 0x017d;
                break;
            case 0xb8:
                *uc = 0x017e;
                break;
            case 0xbc:
                *uc = 0x0152;
                break;
            case 0xbd:
                *uc = 0x0153;
                break;
            case 0xbe:
                *uc = 0x0178;
                break;
            default:
                break;
        }
        uc++;
    }
    return str;
}

QByteArray QLatin15Codec::convertFromUnicode(const QChar *in, int length, ConverterState *state) const
{
    const char replacement = (state && state->flags & ConvertInvalidToNull) ? 0 : '?';
    QByteArray r;
    r.resize(length);
    char *d = r.data();
    int invalid = 0;
    for (int i = 0; i < length; ++i) {
        uchar c;
        ushort uc = in[i].unicode();
        if (uc < 0x0100) {
            if (uc > 0xa3) {
                switch(uc) {
                case 0xa4:
                case 0xa6:
                case 0xa8:
                case 0xb4:
                case 0xb8:
                case 0xbc:
                case 0xbd:
                case 0xbe:
                    c = replacement;
                    ++invalid;
                    break;
                default:
                    c = (unsigned char) uc;
                    break;
                }
            } else {
                c = (unsigned char) uc;
            }
        } else {
            if (uc == 0x20ac)
                c = 0xa4;
            else if ((uc & 0xff00) == 0x0100) {
                switch(uc) {
                case 0x0160:
                    c = 0xa6;
                    break;
                case 0x0161:
                    c = 0xa8;
                    break;
                case 0x017d:
                    c = 0xb4;
                    break;
                case 0x017e:
                    c = 0xb8;
                    break;
                case 0x0152:
                    c = 0xbc;
                    break;
                case 0x0153:
                    c = 0xbd;
                    break;
                case 0x0178:
                    c = 0xbe;
                    break;
                default:
                    c = replacement;
                    ++invalid;
                }
            } else {
                c = replacement;
                ++invalid;
            }
        }
        d[i] = (char)c;
    }
    if (state) {
        state->remainingChars = 0;
        state->invalidChars += invalid;
    }
    return r;
}


QByteArray QLatin15Codec::name() const
{
    return "ISO-8859-15";
}

QList<QByteArray> QLatin15Codec::aliases() const
{
    QList<QByteArray> list;
    list << "latin9";
    return list;
}

int QLatin15Codec::mibEnum() const
{
    return 111;
}

QT_END_NAMESPACE

#endif // QT_NO_TEXTCODEC
