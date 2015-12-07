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

#include "qfontlaocodec_p.h"
#include "qlist.h"

#ifndef QT_NO_CODECS
#ifndef QT_NO_BIG_CODECS

QT_BEGIN_NAMESPACE

static unsigned char const unicode_to_mulelao[256] =
    {
        // U+0E80
        0x00, 0xa1, 0xa2, 0x00, 0xa4, 0x00, 0x00, 0xa7,
        0xa8, 0x00, 0xaa, 0x00, 0x00, 0xad, 0x00, 0x00,
        // U+0E90
        0x00, 0x00, 0x00, 0x00, 0xb4, 0xb5, 0xb6, 0xb7,
        0x00, 0xb9, 0xba, 0xbb, 0xbc, 0xbd, 0xbe, 0xbf,
        // U+0EA0
        0x00, 0xc1, 0xc2, 0xc3, 0x00, 0xc5, 0x00, 0xc7,
        0x00, 0x00, 0xca, 0xcb, 0x00, 0xcd, 0xce, 0xcf,
        // U+0EB0
        0xd0, 0xd1, 0xd2, 0xd3, 0xd4, 0xd5, 0xd6, 0xd7,
        0xd8, 0xd9, 0x00, 0xdb, 0xdc, 0xdd, 0x00, 0x00,
        // U+0EC0
        0xe0, 0xe1, 0xe2, 0xe3, 0xe4, 0x00, 0xe6, 0x00,
        0xe8, 0xe9, 0xea, 0xeb, 0xec, 0xed, 0x00, 0x00,
        // U+0ED0
        0xf0, 0xf1, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7,
        0xf8, 0xf9, 0x00, 0x00, 0xfc, 0xfd, 0x00, 0x00,
        // U+0EE0
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        // U+0EF0
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    };


QFontLaoCodec::~QFontLaoCodec()
{
}

QByteArray QFontLaoCodec::name() const
{
    return "mulelao-1";
}

int QFontLaoCodec::mibEnum() const
{
    return -4242;
}

QString QFontLaoCodec::convertToUnicode(const char *, int, ConverterState *) const
{
    return QString();
}

QByteArray QFontLaoCodec::convertFromUnicode(const QChar *uc, int len, ConverterState *) const
{
    QByteArray rstring;
    rstring.resize(len);
    uchar *rdata = (uchar *) rstring.data();
    const QChar *sdata = uc;
    int i = 0;
    for (; i < len; ++i, ++sdata, ++rdata) {
        if (sdata->unicode() < 0x80) {
            *rdata = (uchar) sdata->unicode();
        } else if (sdata->unicode() >= 0x0e80 && sdata->unicode() <= 0x0eff) {
            uchar lao = unicode_to_mulelao[sdata->unicode() - 0x0e80];
            if (lao)
                *rdata = lao;
            else
                *rdata = 0;
        } else {
            *rdata = 0;
        }
    }
    return rstring;
}

QT_END_NAMESPACE

#endif // QT_NO_BIG_CODECS
#endif // QT_NO_CODECS
