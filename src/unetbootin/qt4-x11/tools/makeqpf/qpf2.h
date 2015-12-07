/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the tools applications of the Qt Toolkit.
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
