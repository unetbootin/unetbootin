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

#include <qtextcodecplugin.h>
#include <qtextcodec.h>
#include <qlist.h>

#include "qgb18030codec.h"

#ifndef QT_NO_TEXTCODECPLUGIN

QT_BEGIN_NAMESPACE

class CNTextCodecs : public QTextCodecPlugin
{
public:
    CNTextCodecs() {}

    QList<QByteArray> names() const;
    QList<QByteArray> aliases() const;
    QList<int> mibEnums() const;

    QTextCodec *createForMib(int);
    QTextCodec *createForName(const QByteArray &);
};

QList<QByteArray> CNTextCodecs::names() const
{
    QList<QByteArray> list;
    list += QGb18030Codec::_name();
    list += QGbkCodec::_name();
    list += QGb2312Codec::_name();
#ifdef Q_WS_X11
    list += QFontGb2312Codec::_name();
    list += QFontGbkCodec::_name();
#endif
    return list;
}

QList<QByteArray> CNTextCodecs::aliases() const
{
    QList<QByteArray> list;
    list += QGb18030Codec::_aliases();
    list += QGbkCodec::_aliases();
    list += QGb2312Codec::_aliases();
#ifdef Q_WS_X11
    list += QFontGb2312Codec::_aliases();
    list += QFontGbkCodec::_aliases();
#endif
    return list;
}

QList<int> CNTextCodecs::mibEnums() const
{
    QList<int> list;
    list += QGb18030Codec::_mibEnum();
    list += QGbkCodec::_mibEnum();
    list += QGb2312Codec::_mibEnum();
#ifdef Q_WS_X11
    list += QFontGb2312Codec::_mibEnum();
    list += QFontGbkCodec::_mibEnum();
#endif
    return list;
}

QTextCodec *CNTextCodecs::createForMib(int mib)
{
    if (mib == QGb18030Codec::_mibEnum())
        return new QGb18030Codec;
    if (mib == QGbkCodec::_mibEnum())
        return new QGbkCodec;
    if (mib == QGb2312Codec::_mibEnum())
        return new QGb2312Codec;
#ifdef Q_WS_X11
    if (mib == QFontGbkCodec::_mibEnum())
        return new QFontGbkCodec;
    if (mib == QFontGb2312Codec::_mibEnum())
        return new QFontGb2312Codec;
#endif
    return 0;
}


QTextCodec *CNTextCodecs::createForName(const QByteArray &name)
{
    if (name == QGb18030Codec::_name() || QGb18030Codec::_aliases().contains(name))
        return new QGb18030Codec;
    if (name == QGbkCodec::_name() || QGbkCodec::_aliases().contains(name))
        return new QGbkCodec;
    if (name == QGb2312Codec::_name() || QGb2312Codec::_aliases().contains(name))
        return new QGb2312Codec;
#ifdef Q_WS_X11
    if (name == QFontGbkCodec::_name() || QFontGbkCodec::_aliases().contains(name))
        return new QFontGbkCodec;
    if (name == QFontGb2312Codec::_name() || QFontGb2312Codec::_aliases().contains(name))
        return new QFontGb2312Codec;
#endif
    return 0;
}


Q_EXPORT_STATIC_PLUGIN(CNTextCodecs)
Q_EXPORT_PLUGIN2(qcncodecs, CNTextCodecs)

QT_END_NAMESPACE

#endif // QT_NO_TEXTCODECPLUGIN
