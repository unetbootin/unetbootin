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

#ifndef QUTFCODEC_P_H
#define QUTFCODEC_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include "QtCore/qtextcodec.h"

QT_BEGIN_NAMESPACE

#ifndef QT_NO_TEXTCODEC

class QUtf8Codec : public QTextCodec {
public:
    ~QUtf8Codec();

    QByteArray name() const;
    int mibEnum() const;

    QString convertToUnicode(const char *, int, ConverterState *) const;
    QByteArray convertFromUnicode(const QChar *, int, ConverterState *) const;
    void convertToUnicode(QString *target, const char *, int, ConverterState *) const;
};

class QUtf16Codec : public QTextCodec {
protected:
    enum Endianness {
        Detect,
        BE,
        LE
    };
public:
    QUtf16Codec() { e = Detect; }
    ~QUtf16Codec();

    QByteArray name() const;
    QList<QByteArray> aliases() const;
    int mibEnum() const;

    QString convertToUnicode(const char *, int, ConverterState *) const;
    QByteArray convertFromUnicode(const QChar *, int, ConverterState *) const;

protected:
    Endianness e;
};

class QUtf16BECodec : public QUtf16Codec {
public:
    QUtf16BECodec() : QUtf16Codec() { e = BE; }
    QByteArray name() const;
    QList<QByteArray> aliases() const;
    int mibEnum() const;
};

class QUtf16LECodec : public QUtf16Codec {
public:
    QUtf16LECodec() : QUtf16Codec() { e = LE; }
    QByteArray name() const;
    QList<QByteArray> aliases() const;
    int mibEnum() const;
};

class QUtf32Codec : public QTextCodec {
protected:
    enum Endianness {
        Detect,
        BE,
        LE
    };
public:
    QUtf32Codec() { e = Detect; }
    ~QUtf32Codec();

    QByteArray name() const;
    QList<QByteArray> aliases() const;
    int mibEnum() const;

    QString convertToUnicode(const char *, int, ConverterState *) const;
    QByteArray convertFromUnicode(const QChar *, int, ConverterState *) const;

protected:
    Endianness e;
};

class QUtf32BECodec : public QUtf32Codec {
public:
    QUtf32BECodec() : QUtf32Codec() { e = BE; }
    QByteArray name() const;
    QList<QByteArray> aliases() const;
    int mibEnum() const;
};

class QUtf32LECodec : public QUtf32Codec {
public:
    QUtf32LECodec() : QUtf32Codec() { e = LE; }
    QByteArray name() const;
    QList<QByteArray> aliases() const;
    int mibEnum() const;
};


#endif // QT_NO_TEXTCODEC

QT_END_NAMESPACE

#endif // QUTFCODEC_P_H
