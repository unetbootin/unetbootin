/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the QtGui module of the Qt Toolkit.
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

#ifndef QFONTENGINE_WIN_P_H
#define QFONTENGINE_WIN_P_H

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

#include <QtCore/qconfig.h>

QT_BEGIN_NAMESPACE

class QFontEngineWin : public QFontEngine
{
public:
    QFontEngineWin(const QString &name, HFONT, bool, LOGFONT);
    ~QFontEngineWin();

    virtual QFixed lineThickness() const;
    virtual Properties properties() const;
    virtual void getUnscaledGlyph(glyph_t glyph, QPainterPath *path, glyph_metrics_t *metrics);
    virtual FaceId faceId() const;
    bool getSfntTableData(uint tag, uchar *buffer, uint *length) const;
    virtual int synthesized() const;
    virtual QFixed emSquareSize() const;

    bool stringToCMap(const QChar *str, int len, QGlyphLayout *glyphs, int *nglyphs, QTextEngine::ShaperFlags flags) const;
    void recalcAdvances(int len, QGlyphLayout *glyphs, QTextEngine::ShaperFlags) const;

    void addOutlineToPath(qreal x, qreal y, const QGlyphLayout *glyphs, int numGlyphs, QPainterPath *path, QTextItem::RenderFlags flags);
    void addGlyphsToPath(glyph_t *glyphs, QFixedPoint *positions, int nglyphs,
                         QPainterPath *path, QTextItem::RenderFlags flags);

    HGDIOBJ selectDesignFont(QFixed *) const;

    glyph_metrics_t boundingBox(const QGlyphLayout *glyphs, int numGlyphs);
    glyph_metrics_t boundingBox(glyph_t glyph);

    QFixed ascent() const;
    QFixed descent() const;
    QFixed leading() const;
    QFixed xHeight() const;
    QFixed averageCharWidth() const;
    qreal maxCharWidth() const;
    qreal minLeftBearing() const;
    qreal minRightBearing() const;

    const char *name() const;

    bool canRender(const QChar *string, int len);

    Type type() const;

    virtual QImage alphaMapForGlyph(glyph_t);

    int getGlyphIndexes(const QChar *ch, int numChars, QGlyphLayout *glyphs, bool mirrored) const;
    void getCMap();

    QString        _name;
    HFONT        hfont;
    LOGFONT     logfont;
    uint        stockFont   : 1;
    uint        useTextOutA : 1;
    uint        ttf         : 1;
    union {
        TEXTMETRICW        w;
        TEXTMETRICA        a;
    } tm;
    int                lw;
    const unsigned char *cmap;
    QByteArray cmapTable;
    mutable qreal lbearing;
    mutable qreal rbearing;
    QFixed designToDevice;
    int unitsPerEm;
    QFixed x_height;
    FaceId _faceId;

    mutable int synthesized_flags;
    mutable QFixed lineWidth;
    mutable unsigned char *widthCache;
    mutable uint widthCacheSize;
    mutable QFixed *designAdvances;
    mutable int designAdvancesSize;
};

class QFontEngineMultiWin : public QFontEngineMulti
{
public:
    QFontEngineMultiWin(QFontEngineWin *first, const QStringList &fallbacks);
    void loadEngine(int at);

    QStringList fallbacks;
};

QT_END_NAMESPACE

#endif // QFONTENGINE_WIN_P_H
