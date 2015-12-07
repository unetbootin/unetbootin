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

#include "qfontjpcodec.h"

#include "qjpunicode.h"

QT_BEGIN_NAMESPACE

#ifdef Q_WS_X11
// JIS X 0201

QFontJis0201Codec::QFontJis0201Codec()
{
}

QByteArray QFontJis0201Codec::_name()
{
    return "jisx0201*-0";
}

int QFontJis0201Codec::_mibEnum()
{
    return 15;
}

QByteArray QFontJis0201Codec::convertFromUnicode(const QChar *uc, int len,  ConverterState *) const
{
    QByteArray rstring;
    rstring.resize(len);
    uchar *rdata = (uchar *) rstring.data();
    const QChar *sdata = uc;
    int i = 0;
    for (; i < len; ++i, ++sdata, ++rdata) {
        if (sdata->unicode() < 0x80) {
            *rdata = (uchar) sdata->unicode();
        } else if (sdata->unicode() >= 0xff61 && sdata->unicode() <= 0xff9f) {
            *rdata = (uchar) (sdata->unicode() - 0xff61 + 0xa1);
        } else {
            *rdata = 0;
        }
    }
    return rstring;
}

QString QFontJis0201Codec::convertToUnicode(const char *, int,  ConverterState *) const
{
    return QString();
}

// JIS X 0208

QFontJis0208Codec::QFontJis0208Codec()
{
    convJP = QJpUnicodeConv::newConverter(QJpUnicodeConv::Default);
}


QFontJis0208Codec::~QFontJis0208Codec()
{
    delete convJP;
    convJP = 0;
}


QByteArray QFontJis0208Codec::_name()
{
    return "jisx0208*-0";
}


int QFontJis0208Codec::_mibEnum()
{
    return 63;
}


QString QFontJis0208Codec::convertToUnicode(const char* /*chars*/, int /*len*/, ConverterState *) const
{
    return QString();
}

QByteArray QFontJis0208Codec::convertFromUnicode(const QChar *uc, int len, ConverterState *) const
{
    QByteArray result;
    result.resize(len * 2);
    uchar *rdata = (uchar *) result.data();
    const QChar *ucp = uc;

    for (int i = 0; i < len; i++) {
        QChar ch(*ucp++);
        ch = convJP->unicodeToJisx0208(ch.unicode());

        if (!ch.isNull()) {
            *rdata++ = ch.row();
            *rdata++ = ch.cell();
        } else {
            *rdata++ = 0;
            *rdata++ = 0;
        }
    }
    return result;
}

#endif

QT_END_NAMESPACE
