/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the tools applications of the Qt Toolkit.
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

#ifndef QPF2_H
#define QPF2_H

#include <private/qfontengine_qpf_p.h>
#include <qmetatype.h>

QT_BEGIN_NAMESPACE

class QFontEngine;

class QPF
{
public:
    static int debugVerbosity;

    enum GenerationOption {
        IncludeCMap = 0x1,
        RenderGlyphs = 0x2
    };

    struct CharacterRange
    {
        inline CharacterRange() : start(0), end(0x10000) {}
        uint start;
        uint end;
    };

    static QString fileNameForFont(const QFont &f);

    static QByteArray generate(const QFont &font, int options,
                               const QList<CharacterRange> &ranges,
                               QString *originalFontFile = 0);
    static QByteArray generate(QFontEngine *fontEngine, int options, const QList<CharacterRange> &ranges);
    void addHeader(QFontEngine *fontEngine);
    void addCMap(QFontEngine *fontEngine);
    void addGlyphs(QFontEngine *fontEngine, const QList<CharacterRange> &ranges);
    void addBlock(QFontEngineQPF::BlockTag tag, const QByteArray &data);

    void addTaggedString(QFontEngineQPF::HeaderTag tag, const QByteArray &string);
    void addTaggedQFixed(QFontEngineQPF::HeaderTag tag, QFixed value);
    void addTaggedUInt8(QFontEngineQPF::HeaderTag tag, quint8 value);
    void addTaggedInt8(QFontEngineQPF::HeaderTag tag, qint8 value);
    void addTaggedUInt16(QFontEngineQPF::HeaderTag tag, quint16 value);
    void addTaggedUInt32(QFontEngineQPF::HeaderTag tag, quint32 value);

    static void dump(const QByteArray &qpf);
    const uchar *dumpHeader(const uchar *data);
    const uchar *dumpHeaderTag(const uchar *data);
    void dumpGMapBlock(const quint32 *gmap, int glyphCount);
    void dumpGlyphBlock(const quint32 *gmap, int glyphCount, const uchar *data, const uchar *endPtr);
    void dumpGlyph(const uchar *data, const QFontEngineQPF::Glyph *glyph);

    void addUInt16(quint16 value) { qToBigEndian(value, addBytes(sizeof(value))); }
    void addUInt32(quint32 value) { qToBigEndian(value, addBytes(sizeof(value))); }
    void addUInt8(quint8 value) { *addBytes(sizeof(value)) = value; }
    void addInt8(qint8 value) { *addBytes(sizeof(value)) = quint8(value); }
    void addByteArray(const QByteArray &string) {
        uchar *data = addBytes(string.length());
        qMemCopy(data, string.constData(), string.length());
    }

    void align4() { while (qpf.size() & 3) { addUInt8('\0'); } }

    uchar *addBytes(int size) {
        const int oldSize = qpf.size();
        qpf.resize(qpf.size() + size);
        return reinterpret_cast<uchar *>(qpf.data() + oldSize);
    }

    QByteArray qpf;
    int options;
};

QT_END_NAMESPACE

Q_DECLARE_METATYPE(QPF::CharacterRange)

#endif // QPF2_H
