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

#include <qdebug.h>
#include <private/qfontengine_p.h>

#include "qbitmap.h"
#include "qpainter.h"
#include "qpainterpath.h"
#include "qvarlengtharray.h"
#include <private/qpdf_p.h>
#include <qmath.h>
#include <qendian.h>
#include <private/qharfbuzz_p.h>

QT_BEGIN_NAMESPACE

// Harfbuzz helper functions

static HB_Bool hb_stringToGlyphs(HB_Font font, const HB_UChar16 *string, hb_uint32 length, HB_Glyph *glyphs, hb_uint32 *numGlyphs, HB_Bool rightToLeft)
{
    QFontEngine *fe = (QFontEngine *)font->userData;

    QVarLengthArray<QGlyphLayout> qglyphs(*numGlyphs);
    memset(qglyphs.data(), 0, qglyphs.size() * sizeof(QGlyphLayout));

    QTextEngine::ShaperFlags shaperFlags(QTextEngine::GlyphIndicesOnly);
    if (rightToLeft)
        shaperFlags |= QTextEngine::RightToLeft;

    int nGlyphs = *numGlyphs;
    bool result = fe->stringToCMap(reinterpret_cast<const QChar *>(string), length, qglyphs.data(), &nGlyphs, shaperFlags);
    *numGlyphs = nGlyphs;
    if (!result)
        return false;

    for (hb_uint32 i = 0; i < *numGlyphs; ++i)
        glyphs[i] = qglyphs[i].glyph;

    return true;
}

static void hb_getAdvances(HB_Font font, const HB_Glyph *glyphs, hb_uint32 numGlyphs, HB_Fixed *advances, int flags)
{
    QFontEngine *fe = (QFontEngine *)font->userData;

    QVarLengthArray<QGlyphLayout> qglyphs(numGlyphs);
    memset(qglyphs.data(), 0, qglyphs.size() * sizeof(QGlyphLayout));

    for (hb_uint32 i = 0; i < numGlyphs; ++i)
        qglyphs[i].glyph = glyphs[i];

    fe->recalcAdvances(numGlyphs, qglyphs.data(), flags & HB_ShaperFlag_UseDesignMetrics ? QFlags<QTextEngine::ShaperFlag>(QTextEngine::DesignMetrics) : QFlags<QTextEngine::ShaperFlag>(0));

    for (hb_uint32 i = 0; i < numGlyphs; ++i)
        advances[i] = qglyphs[i].advance.x.value();
}

static HB_Bool hb_canRender(HB_Font font, const HB_UChar16 *string, hb_uint32 length)
{
    QFontEngine *fe = (QFontEngine *)font->userData;
    return fe->canRender(reinterpret_cast<const QChar *>(string), length);
}

static void hb_getGlyphMetrics(HB_Font font, HB_Glyph glyph, HB_GlyphMetrics *metrics)
{
    QFontEngine *fe = (QFontEngine *)font->userData;
    glyph_metrics_t m = fe->boundingBox(glyph);
    metrics->x = m.x.value();
    metrics->y = m.y.value();
    metrics->width = m.width.value();
    metrics->height = m.height.value();
    metrics->xOffset = m.xoff.value();
    metrics->yOffset = m.yoff.value();
}

static HB_Fixed hb_getFontMetric(HB_Font font, HB_FontMetric metric)
{
    if (metric == HB_FontAscent) {
        QFontEngine *fe = (QFontEngine *)font->userData;
        return fe->ascent().value();
    }
    return 0;
}

HB_Error QFontEngine::getPointInOutline(HB_Glyph glyph, int flags, hb_uint32 point, HB_Fixed *xpos, HB_Fixed *ypos, hb_uint32 *nPoints)
{
    Q_UNUSED(glyph)
    Q_UNUSED(flags)
    Q_UNUSED(point)
    Q_UNUSED(xpos)
    Q_UNUSED(ypos)
    Q_UNUSED(nPoints)
    return HB_Err_Not_Covered;
}

static HB_Error hb_getPointInOutline(HB_Font font, HB_Glyph glyph, int flags, hb_uint32 point, HB_Fixed *xpos, HB_Fixed *ypos, hb_uint32 *nPoints)
{
    QFontEngine *fe = (QFontEngine *)font->userData;
    return fe->getPointInOutline(glyph, flags, point, xpos, ypos, nPoints);
}

static const HB_FontClass hb_fontClass = {
    hb_stringToGlyphs, hb_getAdvances, hb_canRender, hb_getPointInOutline,
    hb_getGlyphMetrics, hb_getFontMetric
};

static HB_Error hb_getSFntTable(void *font, HB_Tag tableTag, HB_Byte *buffer, HB_UInt *length)
{
    QFontEngine *fe = (QFontEngine *)font;
    if (!fe->getSfntTableData(tableTag, buffer, length))
        return HB_Err_Invalid_Argument;
    return HB_Err_Ok;
}

// QFontEngine

QFontEngine::QFontEngine()
    : QObject()
{
    ref = 0;
    cache_count = 0;
    fsType = 0;
    symbol = false;
    memset(&hbFont, 0, sizeof(hbFont));
    hbFont.klass = &hb_fontClass;
    hbFont.userData = this;

    hbFace = 0;
}

QFontEngine::~QFontEngine()
{
    qHBFreeFace(hbFace);
}

QFixed QFontEngine::lineThickness() const
{
    // ad hoc algorithm
    int score = fontDef.weight * fontDef.pixelSize;
    int lw = score / 700;

    // looks better with thicker line for small pointsizes
    if (lw < 2 && score >= 1050) lw = 2;
    if (lw == 0) lw = 1;

    return lw;
}

QFixed QFontEngine::underlinePosition() const
{
    return ((lineThickness() * 2) + 3) / 6;
}

HB_Font QFontEngine::harfbuzzFont() const
{
    if (!hbFont.x_ppem) {
        QFixed emSquare = emSquareSize();
        hbFont.x_ppem = fontDef.pixelSize;
        hbFont.y_ppem = fontDef.pixelSize * fontDef.stretch / 100;
        hbFont.x_scale = (QFixed(hbFont.x_ppem * (1 << 16)) / emSquare).value();
        hbFont.y_scale = (QFixed(hbFont.y_ppem * (1 << 16)) / emSquare).value();
    }
    return &hbFont;
}

HB_Face QFontEngine::harfbuzzFace() const
{
    if (!hbFace)
        hbFace = qHBNewFace(const_cast<QFontEngine *>(this), hb_getSFntTable);
    return hbFace;
}

QFixed QFontEngine::xHeight() const
{
    QGlyphLayout glyphs[8];
    int nglyphs = 7;
    QChar x((ushort)'x');
    stringToCMap(&x, 1, glyphs, &nglyphs, QTextEngine::GlyphIndicesOnly);

    glyph_metrics_t bb = const_cast<QFontEngine *>(this)->boundingBox(glyphs[0].glyph);
    return bb.height;
}

QFixed QFontEngine::averageCharWidth() const
{
    QGlyphLayout glyphs[8];
    int nglyphs = 7;
    QChar x((ushort)'x');
    stringToCMap(&x, 1, glyphs, &nglyphs, QTextEngine::GlyphIndicesOnly);

    glyph_metrics_t bb = const_cast<QFontEngine *>(this)->boundingBox(glyphs[0].glyph);
    return bb.xoff;
}


void QFontEngine::getGlyphPositions(const QGlyphLayout *glyphs, int nglyphs, const QTransform &matrix, QTextItem::RenderFlags flags,
                                    QVarLengthArray<glyph_t> &glyphs_out, QVarLengthArray<QFixedPoint> &positions)
{
    QFixed xpos;
    QFixed ypos;

    const bool transform = matrix.m11() != 1.
                           || matrix.m12() != 0.
                           || matrix.m21() != 0.
                           || matrix.m22() != 1.;
    if (!transform) {
        xpos = QFixed::fromReal(matrix.dx());
        ypos = QFixed::fromReal(matrix.dy());
    }

    int current = 0;
    if (flags & QTextItem::RightToLeft) {
        int i = nglyphs;
        int totalKashidas = 0;
        while(i--) {
            xpos += glyphs[i].advance.x + QFixed::fromFixed(glyphs[i].justification.space_18d6);
            ypos += glyphs[i].advance.y;
            totalKashidas += glyphs[i].justification.nKashidas;
        }
        positions.resize(nglyphs+totalKashidas);
        glyphs_out.resize(nglyphs+totalKashidas);

        i = 0;
        while(i < nglyphs) {
            if (glyphs[i].attributes.dontPrint) {
                ++i;
                continue;
            }
            xpos -= glyphs[i].advance.x;
            ypos -= glyphs[i].advance.y;

            QFixed gpos_x = xpos + glyphs[i].offset.x;
            QFixed gpos_y = ypos + glyphs[i].offset.y;
            if (transform) {
                QPointF gpos(gpos_x.toReal(), gpos_y.toReal());
                gpos = gpos * matrix;
                gpos_x = QFixed::fromReal(gpos.x());
                gpos_y = QFixed::fromReal(gpos.y());
            }
            positions[current].x = gpos_x;
            positions[current].y = gpos_y;
            glyphs_out[current] = glyphs[i].glyph;
            ++current;
            if (glyphs[i].justification.nKashidas) {
                QChar ch(0x640); // Kashida character
                QGlyphLayout g[8];
                int nglyphs = 7;
                stringToCMap(&ch, 1, g, &nglyphs, 0);
                for (uint k = 0; k < glyphs[i].justification.nKashidas; ++k) {
                    xpos -= g[0].advance.x;
                    ypos -= g[0].advance.y;

                    QFixed gpos_x = xpos + glyphs[i].offset.x;
                    QFixed gpos_y = ypos + glyphs[i].offset.y;
                    if (transform) {
                        QPointF gpos(gpos_x.toReal(), gpos_y.toReal());
                        gpos = gpos * matrix;
                        gpos_x = QFixed::fromReal(gpos.x());
                        gpos_y = QFixed::fromReal(gpos.y());
                    }
                    positions[current].x = gpos_x;
                    positions[current].y = gpos_y;
                    glyphs_out[current] = g[0].glyph;
                    ++current;
                }
            } else {
                xpos -= QFixed::fromFixed(glyphs[i].justification.space_18d6);
            }
            ++i;
        }
    } else {
        positions.resize(nglyphs);
        glyphs_out.resize(nglyphs);
        int i = 0;
        if (!transform) {
            while (i < nglyphs) {
                if (!glyphs[i].attributes.dontPrint) {
                    positions[current].x = xpos + glyphs[i].offset.x;
                    positions[current].y = ypos + glyphs[i].offset.y;
                    glyphs_out[current] = glyphs[i].glyph;
                    xpos += glyphs[i].advance.x + QFixed::fromFixed(glyphs[i].justification.space_18d6);
                    ypos += glyphs[i].advance.y;
                    ++current;
                }
                ++i;
            }
        } else {
            positions.resize(nglyphs);
            glyphs_out.resize(nglyphs);
            int i = 0;
            while (i < nglyphs) {
                if (!glyphs[i].attributes.dontPrint) {
                    QFixed gpos_x = xpos + glyphs[i].offset.x;
                    QFixed gpos_y = ypos + glyphs[i].offset.y;
                    QPointF gpos(gpos_x.toReal(), gpos_y.toReal());
                    gpos = gpos * matrix;
                    positions[current].x = QFixed::fromReal(gpos.x());
                    positions[current].y = QFixed::fromReal(gpos.y());
                    glyphs_out[current] = glyphs[i].glyph;
                    xpos += glyphs[i].advance.x + QFixed::fromFixed(glyphs[i].justification.space_18d6);
                    ypos += glyphs[i].advance.y;
                    ++current;
                }
                ++i;
            }
        }
    }
    positions.resize(current);
    glyphs_out.resize(current);
    Q_ASSERT(positions.size() == glyphs_out.size());
}


glyph_metrics_t QFontEngine::tightBoundingBox(const QGlyphLayout *glyphs, int numGlyphs)
{
    glyph_metrics_t overall;

    QFixed ymax = 0;
    QFixed xmax = 0;
    for (int i = 0; i < numGlyphs; i++) {
        glyph_metrics_t bb = boundingBox(glyphs[i].glyph);
        QFixed x = overall.xoff + glyphs[i].offset.x + bb.x;
        QFixed y = overall.yoff + glyphs[i].offset.y + bb.y;
        overall.x = qMin(overall.x, x);
        overall.y = qMin(overall.y, y);
        xmax = qMax(xmax, x + bb.width);
        ymax = qMax(ymax, y + bb.height);
        overall.xoff += bb.xoff;
        overall.yoff += bb.yoff;
    }
    overall.height = qMax(overall.height, ymax - overall.y);
    overall.width = xmax - overall.x;

    return overall;
}


void QFontEngine::addOutlineToPath(qreal x, qreal y, const QGlyphLayout *glyphs, int numGlyphs, QPainterPath *path,
                                   QTextItem::RenderFlags flags)
{
    if (!numGlyphs)
        return;

    QVarLengthArray<QFixedPoint> positions;
    QVarLengthArray<glyph_t> positioned_glyphs;
    QTransform matrix;
    matrix.translate(x, y);
    getGlyphPositions(glyphs, numGlyphs, matrix, flags, positioned_glyphs, positions);
    addGlyphsToPath(positioned_glyphs.data(), positions.data(), positioned_glyphs.size(), path, flags);
}

#define GRID(x, y) grid[(y)*(w+1) + (x)]
#define SET(x, y) (*(image_data + (y)*bpl + ((x) >> 3)) & (0x80 >> ((x) & 7)))

enum { EdgeRight = 0x1,
       EdgeDown = 0x2,
       EdgeLeft = 0x4,
       EdgeUp = 0x8
};

static void collectSingleContour(qreal x0, qreal y0, uint *grid, int x, int y, int w, int h, QPainterPath *path)
{
    Q_UNUSED(h);

    path->moveTo(x + x0, y + y0);
    while (GRID(x, y)) {
        if (GRID(x, y) & EdgeRight) {
            while (GRID(x, y) & EdgeRight) {
                GRID(x, y) &= ~EdgeRight;
                ++x;
            }
            Q_ASSERT(x <= w);
            path->lineTo(x + x0, y + y0);
            continue;
        }
        if (GRID(x, y) & EdgeDown) {
            while (GRID(x, y) & EdgeDown) {
                GRID(x, y) &= ~EdgeDown;
                ++y;
            }
            Q_ASSERT(y <= h);
            path->lineTo(x + x0, y + y0);
            continue;
        }
        if (GRID(x, y) & EdgeLeft) {
            while (GRID(x, y) & EdgeLeft) {
                GRID(x, y) &= ~EdgeLeft;
                --x;
            }
            Q_ASSERT(x >= 0);
            path->lineTo(x + x0, y + y0);
            continue;
        }
        if (GRID(x, y) & EdgeUp) {
            while (GRID(x, y) & EdgeUp) {
                GRID(x, y) &= ~EdgeUp;
                --y;
            }
            Q_ASSERT(y >= 0);
            path->lineTo(x + x0, y + y0);
            continue;
        }
    }
    path->closeSubpath();
}

void qt_addBitmapToPath(qreal x0, qreal y0, const uchar *image_data, int bpl, int w, int h, QPainterPath *path)
{
    uint *grid = new uint[(w+1)*(h+1)];
    // set up edges
    for (int y = 0; y <= h; ++y) {
        for (int x = 0; x <= w; ++x) {
            bool topLeft = (x == 0)|(y == 0) ? false : SET(x - 1, y - 1);
            bool topRight = (x == w)|(y == 0) ? false : SET(x, y - 1);
            bool bottomLeft = (x == 0)|(y == h) ? false : SET(x - 1, y);
            bool bottomRight = (x == w)|(y == h) ? false : SET(x, y);

            GRID(x, y) = 0;
            if ((!topRight) & bottomRight)
                GRID(x, y) |= EdgeRight;
            if ((!bottomRight) & bottomLeft)
                GRID(x, y) |= EdgeDown;
            if ((!bottomLeft) & topLeft)
                GRID(x, y) |= EdgeLeft;
            if ((!topLeft) & topRight)
                GRID(x, y) |= EdgeUp;
        }
    }

    // collect edges
    for (int y = 0; y < h; ++y) {
        for (int x = 0; x < w; ++x) {
            if (!GRID(x, y))
                continue;
            // found start of a contour, follow it
            collectSingleContour(x0, y0, grid, x, y, w, h, path);
        }
    }
    delete [] grid;
}

#undef GRID
#undef SET


void QFontEngine::addBitmapFontToPath(qreal x, qreal y, const QGlyphLayout *glyphs, int numGlyphs,
                                      QPainterPath *path, QTextItem::RenderFlags flags)
{
    glyph_metrics_t metrics = boundingBox(glyphs, numGlyphs);
    int w = metrics.width.toInt();
    int h = metrics.height.toInt();
    if (w <= 0 || h <= 0)
        return;

    QImage bm(w, h, QImage::Format_Mono);

    QPainter p(&bm);
    p.fillRect(0, 0, w, h, QBrush(bm.color(0)));
    p.setPen(bm.color(1));

    QTextItemInt item;
    item.flags = flags;
    item.ascent = -metrics.y;
    item.descent = metrics.height - item.ascent;
    item.width = metrics.width;
    item.chars = 0;
    item.num_chars = 0;
    item.logClusters = 0;
    item.glyphs = const_cast<QGlyphLayout *>(glyphs);
    item.num_glyphs = numGlyphs;
    item.fontEngine = this;
    item.f = 0;

    p.drawTextItem(QPointF(-metrics.x.toReal(), item.ascent.toReal()), item);
    p.end();

    const uchar *image_data = bm.bits();
    uint bpl = bm.bytesPerLine();
    qt_addBitmapToPath(x, y - item.ascent.toReal(), image_data, bpl, w, h, path);
}

void QFontEngine::addGlyphsToPath(glyph_t *glyphs, QFixedPoint *positions, int nGlyphs,
                                  QPainterPath *path, QTextItem::RenderFlags flags)
{
    qreal x = positions[0].x.toReal();
    qreal y = positions[0].y.toReal();
    QVarLengthArray<QGlyphLayout> g(nGlyphs);
    memset(g.data(), 0, nGlyphs*sizeof(QGlyphLayout));

    for (int i = 0; i < nGlyphs; ++i) {
        g[i].glyph = glyphs[i];
        if (i < nGlyphs - 1) {
            g[i].advance.x = positions[i+1].x - positions[i].x;
            g[i].advance.y = positions[i+1].y - positions[i].y;
        } else {
            g[i].advance.x = QFixed::fromReal(maxCharWidth());
            g[i].advance.y = 0;
        }
    }

    addBitmapFontToPath(x, y, g.data(), nGlyphs, path, flags);
}



QImage QFontEngine::alphaMapForGlyph(glyph_t glyph)
{
    glyph_metrics_t gm = boundingBox(glyph);
    int glyph_x = qFloor(gm.x.toReal());
    int glyph_y = qFloor(gm.y.toReal());
    int glyph_width = qCeil((gm.x + gm.width).toReal()) -  glyph_x;
    int glyph_height = qCeil((gm.y + gm.height).toReal()) - glyph_y;

    if (glyph_width <= 0 || glyph_height <= 0)
        return QImage();
    QFixedPoint pt;
    pt.x = 0;
    pt.y = -glyph_y; // the baseline
    QPainterPath path;
    QImage im(glyph_width + qAbs(glyph_x) + 4, glyph_height, QImage::Format_ARGB32_Premultiplied);
    im.fill(Qt::transparent);
    QPainter p(&im);
    p.setRenderHint(QPainter::Antialiasing);
    addGlyphsToPath(&glyph, &pt, 1, &path, 0);
    p.setPen(Qt::NoPen);
    p.setBrush(Qt::black);
    p.drawPath(path);
    p.end();

    QImage indexed(im.width(), im.height(), QImage::Format_Indexed8);
    QVector<QRgb> colors(256);
    for (int i=0; i<256; ++i)
        colors[i] = qRgba(0, 0, 0, i);
    indexed.setColorTable(colors);

    for (int y=0; y<im.height(); ++y) {
        uchar *dst = (uchar *) indexed.scanLine(y);
        uint *src = (uint *) im.scanLine(y);
        for (int x=0; x<im.width(); ++x)
            dst[x] = qAlpha(src[x]);
    }

    return indexed;
}

void QFontEngine::removeGlyphFromCache(glyph_t)
{
}

QFontEngine::Properties QFontEngine::properties() const
{
    Properties p;
#ifndef QT_NO_PRINTER
    QByteArray psname = QPdf::stripSpecialCharacters(fontDef.family.toUtf8());
#else
    QByteArray psname = fontDef.family.toUtf8();
#endif
    psname += '-';
    psname += QByteArray::number(fontDef.style);
    psname += '-';
    psname += QByteArray::number(fontDef.weight);

    p.postscriptName = psname;
    p.ascent = ascent();
    p.descent = descent();
    p.leading = leading();
    p.emSquare = p.ascent;
    p.boundingBox = QRectF(0, -p.ascent.toReal(), maxCharWidth(), (p.ascent + p.descent).toReal());
    p.italicAngle = 0;
    p.capHeight = p.ascent;
    p.lineWidth = lineThickness();
    return p;
}

void QFontEngine::getUnscaledGlyph(glyph_t glyph, QPainterPath *path, glyph_metrics_t *metrics)
{
    *metrics = boundingBox(glyph);
    QFixedPoint p;
    p.x = 0;
    p.y = 0;
    addGlyphsToPath(&glyph, &p, 1, path, QFlag(0));
}

QByteArray QFontEngine::getSfntTable(uint tag) const
{
    QByteArray table;
    uint len = 0;
    if (!getSfntTableData(tag, 0, &len))
        return table;
    if (!len)
        return table;
    table.resize(len);
    if (!getSfntTableData(tag, reinterpret_cast<uchar *>(table.data()), &len))
        return QByteArray();
    return table;
}

#if defined(Q_WS_WIN) || defined(Q_WS_X11) || defined(Q_WS_QWS)
static inline QFixed kerning(int left, int right, const QFontEngine::KernPair *pairs, int numPairs)
{
    uint left_right = (left << 16) + right;

    left = 0, right = numPairs - 1;
    while (left <= right) {
        int middle = left + ( ( right - left ) >> 1 );

        if(pairs[middle].left_right == left_right)
            return pairs[middle].adjust;

        if (pairs[middle].left_right < left_right)
            left = middle + 1;
        else
            right = middle - 1;
    }
    return 0;
}

void QFontEngine::doKerning(int num_glyphs, QGlyphLayout *glyphs, QTextEngine::ShaperFlags flags) const
{
    int numPairs = kerning_pairs.size();
    if(!numPairs)
        return;

    const KernPair *pairs = kerning_pairs.constData();

    if(flags & QTextEngine::DesignMetrics) {
        for(int i = 0; i < num_glyphs - 1; ++i)
            glyphs[i].advance.x += kerning(glyphs[i].glyph, glyphs[i+1].glyph , pairs, numPairs);
    } else {
        for(int i = 0; i < num_glyphs - 1; ++i)
            glyphs[i].advance.x += qRound(kerning(glyphs[i].glyph, glyphs[i+1].glyph , pairs, numPairs));
    }
}

static inline bool operator<(const QFontEngine::KernPair &p1, const QFontEngine::KernPair &p2)
{
    return p1.left_right < p2.left_right;
}

void QFontEngine::loadKerningPairs(QFixed scalingFactor)
{
    kerning_pairs.clear();

    QByteArray tab = getSfntTable(MAKE_TAG('k', 'e', 'r', 'n'));
    if (tab.isEmpty())
        return;

    const uchar *table = reinterpret_cast<const uchar *>(tab.constData());

    unsigned short version = qFromBigEndian<quint16>(table);
    if (version != 0) {
//        qDebug("wrong version");
       return;
    }

    unsigned short numTables = qFromBigEndian<quint16>(table + 2);
    {
        int offset = 4;
        for(int i = 0; i < numTables; ++i) {
            if (offset + 6 > tab.size()) {
//                qDebug("offset out of bounds");
                goto end;
            }
            const uchar *header = table + offset;

            ushort version = qFromBigEndian<quint16>(header);
            ushort length = qFromBigEndian<quint16>(header+2);
            ushort coverage = qFromBigEndian<quint16>(header+4);
//            qDebug("subtable: version=%d, coverage=%x",version, coverage);
            if(version == 0 && coverage == 0x0001) {
                if (offset + length > tab.size()) {
//                    qDebug("length ouf ot bounds");
                    goto end;
                }
                const uchar *data = table + offset + 6;

                ushort nPairs = qFromBigEndian<quint16>(data);
                if(nPairs * 6 + 8 > length - 6) {
//                    qDebug("corrupt table!");
                    // corrupt table
                    goto end;
                }

                int off = 8;
                for(int i = 0; i < nPairs; ++i) {
                    QFontEngine::KernPair p;
                    p.left_right = (((uint)qFromBigEndian<quint16>(data+off)) << 16) + qFromBigEndian<quint16>(data+off+2);
                    p.adjust = QFixed(((int)(short)qFromBigEndian<quint16>(data+off+4))) / scalingFactor;
                    kerning_pairs.append(p);
                    off += 6;
                }
            }
            offset += length;
        }
    }
end:
    qSort(kerning_pairs);
//    for (int i = 0; i < kerning_pairs.count(); ++i)
//        qDebug() << "i" << i << "left_right" << hex << kerning_pairs.at(i).left_right;
}

#else
void QFontEngine::doKerning(int, QGlyphLayout *, QTextEngine::ShaperFlags) const
{
}
#endif

int QFontEngine::glyphCount() const
{
    QByteArray maxpTable = getSfntTable(MAKE_TAG('m', 'a', 'x', 'p'));
    if (maxpTable.size() < 6)
        return 0;
    return qFromBigEndian<quint16>(reinterpret_cast<const uchar *>(maxpTable.constData() + 4));
}

const uchar *QFontEngine::getCMap(const uchar *table, uint tableSize, bool *isSymbolFont, int *cmapSize)
{
    const uchar *header = table;
    if (tableSize < 4)
        return 0;

    const uchar *endPtr = table + tableSize;

    // version check
    if (qFromBigEndian<quint16>(header) != 0)
        return 0;

    unsigned short numTables = qFromBigEndian<quint16>(header + 2);
    const uchar *maps = table + 4;
    if (maps + 8 * numTables > endPtr)
        return 0;

    int tableToUse = -1;
    int score = 0;
    for (int n = 0; n < numTables; ++n) {
        const quint16 platformId = qFromBigEndian<quint16>(maps + 8 * n);
        const quint16 platformSpecificId = qFromBigEndian<quint16>(maps + 8 * n + 2);
        switch (platformId) {
        case 0: // Unicode
            if (score < 4 &&
                (platformSpecificId == 0 ||
                 platformSpecificId == 2 ||
                 platformSpecificId == 3)) {
                tableToUse = n;
                score = 4;
            } else if (score < 3 && platformSpecificId == 1) {
                tableToUse = n;
                score = 3;
            }
            break;
        case 1: // Apple
            if (score < 2 && platformSpecificId == 0) { // Apple Roman
                tableToUse = n;
                score = 2;
            }
            break;
        case 3: // Microsoft
            switch (platformSpecificId) {
            case 0:
                if (score < 1) {
                    tableToUse = n;
                    score = 1;
                }
                break;
            case 1:
                if (score < 5) {
                    tableToUse = n;
                    score = 5;
                }
                break;
            case 0xa:
                if (score < 6) {
                    tableToUse = n;
                    score = 6;
                }
                break;
            default:
                break;
            }
        default:
            break;
        }
    }
    if(tableToUse < 0)
        return 0;
    *isSymbolFont = (score == 1);

    unsigned int unicode_table = qFromBigEndian<quint32>(maps + 8*tableToUse + 4);

    if (!unicode_table || unicode_table + 8 > tableSize)
        return 0;

    // get the header of the unicode table
    header = table + unicode_table;

    unsigned short format = qFromBigEndian<quint16>(header);
    unsigned int length;
    if(format < 8)
        length = qFromBigEndian<quint16>(header + 2);
    else
        length = qFromBigEndian<quint32>(header + 4);

    if (table + unicode_table + length > endPtr)
        return 0;
    *cmapSize = length;
    return table + unicode_table;
}

quint32 QFontEngine::getTrueTypeGlyphIndex(const uchar *cmap, uint unicode)
{
    unsigned short format = qFromBigEndian<quint16>(cmap);
    if (format == 0) {
        if (unicode < 256)
            return (int) *(cmap+6+unicode);
    } else if (format == 4) {
        /* some fonts come with invalid cmap tables, where the last segment
           specified end = start = rangeoffset = 0xffff, delta = 0x0001
           Since 0xffff is never a valid Unicode char anyway, we just get rid of the issue
           by returning 0 for 0xffff
        */
        if(unicode >= 0xffff)
            return 0;
        quint16 segCountX2 = qFromBigEndian<quint16>(cmap + 6);
        const unsigned char *ends = cmap + 14;
        quint16 endIndex = 0;
        int i = 0;
        for (; i < segCountX2/2 && (endIndex = qFromBigEndian<quint16>(ends + 2*i)) < unicode; i++);

        const unsigned char *idx = ends + segCountX2 + 2 + 2*i;
        quint16 startIndex = qFromBigEndian<quint16>(idx);

        if (startIndex > unicode)
            return 0;

        idx += segCountX2;
        qint16 idDelta = (qint16)qFromBigEndian<quint16>(idx);
        idx += segCountX2;
        quint16 idRangeoffset_t = (quint16)qFromBigEndian<quint16>(idx);

        quint16 glyphIndex;
        if (idRangeoffset_t) {
            quint16 id = qFromBigEndian<quint16>(idRangeoffset_t + 2*(unicode - startIndex) + idx);
            if (id)
                glyphIndex = (idDelta + id) % 0x10000;
            else
                glyphIndex = 0;
        } else {
            glyphIndex = (idDelta + unicode) % 0x10000;
        }
        return glyphIndex;
    } else if (format == 12) {
        quint32 nGroups = qFromBigEndian<quint32>(cmap + 12);

        cmap += 16; // move to start of groups

        int left = 0, right = nGroups - 1;
        while (left <= right) {
            int middle = left + ( ( right - left ) >> 1 );

            quint32 startCharCode = qFromBigEndian<quint32>(cmap + 12*middle);
            if(unicode < startCharCode)
                right = middle - 1;
            else {
                quint32 endCharCode = qFromBigEndian<quint32>(cmap + 12*middle + 4);
                if(unicode <= endCharCode)
                    return qFromBigEndian<quint32>(cmap + 12*middle + 8) + unicode - startCharCode;
                left = middle + 1;
            }
        }
    } else {
        qDebug("cmap table of format %d not implemented", format);
    }

    return 0;
}

// ------------------------------------------------------------------
// The box font engine
// ------------------------------------------------------------------

QFontEngineBox::QFontEngineBox(int size)
    : _size(size)
{
    cache_cost = sizeof(QFontEngineBox);
}

QFontEngineBox::~QFontEngineBox()
{
}

bool QFontEngineBox::stringToCMap(const QChar *, int len, QGlyphLayout *glyphs, int *nglyphs, QTextEngine::ShaperFlags) const
{
    if (*nglyphs < len) {
        *nglyphs = len;
        return false;
    }

    for (int i = 0; i < len; i++) {
        glyphs[i].glyph = 0;
        glyphs[i].advance.x = _size;
        glyphs[i].advance.y = 0;
    }

    *nglyphs = len;
    return true;
}

void QFontEngineBox::recalcAdvances(int len, QGlyphLayout *glyphs, QTextEngine::ShaperFlags) const
{
    for (int i = 0; i < len; i++) {
        glyphs[i].advance.x = _size;
        glyphs[i].advance.y = 0;
    }
}

void QFontEngineBox::addOutlineToPath(qreal x, qreal y, const QGlyphLayout *glyphs, int numGlyphs, QPainterPath *path, QTextItem::RenderFlags flags)
{
    if (!numGlyphs)
        return;

    QVarLengthArray<QFixedPoint> positions;
    QVarLengthArray<glyph_t> positioned_glyphs;
    QTransform matrix;
    matrix.translate(x, y - _size);
    getGlyphPositions(glyphs, numGlyphs, matrix, flags, positioned_glyphs, positions);

    QSize s(_size - 3, _size - 3);
    for (int k = 0; k < positions.size(); k++)
        path->addRect(QRectF(positions[k].toPointF(), s));
}

glyph_metrics_t QFontEngineBox::boundingBox(const QGlyphLayout *, int numGlyphs)
{
    glyph_metrics_t overall;
    overall.width = _size*numGlyphs;
    overall.height = _size;
    overall.xoff = overall.width;
    return overall;
}

#if defined(Q_WS_QWS)
void QFontEngineBox::draw(QPaintEngine *p, qreal x, qreal y, const QTextItemInt &ti)
{
    if (!ti.num_glyphs)
        return;

    // any fixes here should probably also be done in QPaintEnginePrivate::drawBoxTextItem
    QSize s(_size - 3, _size - 3);

    QVarLengthArray<QFixedPoint> positions;
    QVarLengthArray<glyph_t> glyphs;
    QTransform matrix;
    matrix.translate(x, y - _size);
    ti.fontEngine->getGlyphPositions(ti.glyphs, ti.num_glyphs, matrix, ti.flags, glyphs, positions);
    if (glyphs.size() == 0)
        return;


    QPainter *painter = p->painter();
    painter->save();
    painter->setBrush(Qt::NoBrush);
    QPen pen = painter->pen();
    pen.setWidthF(lineThickness().toReal());
    painter->setPen(pen);
    for (int k = 0; k < positions.size(); k++)
        painter->drawRect(QRectF(positions[k].toPointF(), s));
    painter->restore();
}
#endif

glyph_metrics_t QFontEngineBox::boundingBox(glyph_t)
{
    return glyph_metrics_t(0, _size, _size, _size, _size, 0);
}



QFixed QFontEngineBox::ascent() const
{
    return _size;
}

QFixed QFontEngineBox::descent() const
{
    return 0;
}

QFixed QFontEngineBox::leading() const
{
    QFixed l = _size * QFixed::fromReal(qreal(0.15));
    return l.ceil();
}

qreal QFontEngineBox::maxCharWidth() const
{
    return _size;
}

#ifdef Q_WS_X11
int QFontEngineBox::cmap() const
{
    return -1;
}
#endif

const char *QFontEngineBox::name() const
{
    return "null";
}

bool QFontEngineBox::canRender(const QChar *, int)
{
    return true;
}

QFontEngine::Type QFontEngineBox::type() const
{
    return Box;
}


// ------------------------------------------------------------------
// Multi engine
// ------------------------------------------------------------------

static inline uchar highByte(glyph_t glyph)
{ return glyph >> 24; }

// strip high byte from glyph
static inline glyph_t stripped(glyph_t glyph)
{ return glyph & 0x00ffffff; }

QFontEngineMulti::QFontEngineMulti(int engineCount)
{
    engines.fill(0, engineCount);
    cache_cost = 0;
}

QFontEngineMulti::~QFontEngineMulti()
{
    for (int i = 0; i < engines.size(); ++i) {
        QFontEngine *fontEngine = engines.at(i);
        if (fontEngine) {
            fontEngine->ref.deref();
            if (fontEngine->cache_count == 0 && fontEngine->ref == 0)
                delete fontEngine;
        }
    }
}

bool QFontEngineMulti::stringToCMap(const QChar *str, int len,
                                    QGlyphLayout *glyphs, int *nglyphs,
                                    QTextEngine::ShaperFlags flags) const
{
    int ng = *nglyphs;
    if (!engine(0)->stringToCMap(str, len, glyphs, &ng, flags))
        return false;

    int glyph_pos = 0;
    for (int i = 0; i < len; ++i) {
        bool surrogate = (str[i].unicode() >= 0xd800 && str[i].unicode() < 0xdc00 && i < len-1
                          && str[i+1].unicode() >= 0xdc00 && str[i+1].unicode() < 0xe000);
        if (glyphs[glyph_pos].glyph == 0) {
            QGlyphLayout tmp = glyphs[glyph_pos];
            for (int x = 1; x < engines.size(); ++x) {
                QFontEngine *engine = engines.at(x);
                if (!engine) {
                    const_cast<QFontEngineMulti *>(this)->loadEngine(x);
                    engine = engines.at(x);
                }
                Q_ASSERT(engine != 0);
                if (engine->type() == Box)
                    continue;
                glyphs[glyph_pos].advance = glyphs[glyph_pos].offset = QFixedPoint();
                int num = 2;
                engine->stringToCMap(str + i, surrogate ? 2 : 1, glyphs + glyph_pos, &num, flags);
                Q_ASSERT(num == 1); // surrogates only give 1 glyph
                if (glyphs[glyph_pos].glyph) {
                    // set the high byte to indicate which engine the glyph came from
                    glyphs[glyph_pos].glyph |= (x << 24);
                    break;
                }
            }
            // ensure we use metrics from the 1st font when we use the fallback image.
            if (!glyphs[glyph_pos].glyph)
                glyphs[glyph_pos] = tmp;
        }
        if (surrogate)
            ++i;
        ++glyph_pos;
    }

    *nglyphs = ng;
    return true;
}

glyph_metrics_t QFontEngineMulti::boundingBox(const QGlyphLayout *glyphs_const, int numGlyphs)
{
    if (numGlyphs <= 0)
        return glyph_metrics_t();

    glyph_metrics_t overall;

    QGlyphLayout *glyphs = const_cast<QGlyphLayout *>(glyphs_const);
    int which = highByte(glyphs[0].glyph);
    int start = 0;
    int end, i;
    for (end = 0; end < numGlyphs; ++end) {
        const int e = highByte(glyphs[end].glyph);
        if (e == which)
            continue;

        // set the high byte to zero
        for (i = start; i < end; ++i)
            glyphs[i].glyph = stripped(glyphs[i].glyph);

        // merge the bounding box for this run
        const glyph_metrics_t gm = engine(which)->boundingBox(glyphs + start, end - start);

        overall.x = qMin(overall.x, gm.x);
        overall.y = qMin(overall.y, gm.y);
        overall.width = overall.xoff + gm.width;
        overall.height = qMax(overall.height + overall.y, gm.height + gm.y) -
                         qMin(overall.y, gm.y);
        overall.xoff += gm.xoff;
        overall.yoff += gm.yoff;

        // reset the high byte for all glyphs
        const int hi = which << 24;
        for (i = start; i < end; ++i)
            glyphs[i].glyph = hi | glyphs[i].glyph;

        // change engine
        start = end;
        which = e;
    }

    // set the high byte to zero
    for (i = start; i < end; ++i)
        glyphs[i].glyph = stripped(glyphs[i].glyph);

    // merge the bounding box for this run
    const glyph_metrics_t gm = engine(which)->boundingBox(glyphs + start, end - start);

    overall.x = qMin(overall.x, gm.x);
    overall.y = qMin(overall.y, gm.y);
    overall.width = overall.xoff + gm.width;
    overall.height = qMax(overall.height + overall.y, gm.height + gm.y) -
                     qMin(overall.y, gm.y);
    overall.xoff += gm.xoff;
    overall.yoff += gm.yoff;

    // reset the high byte for all glyphs
    const int hi = which << 24;
    for (i = start; i < end; ++i)
        glyphs[i].glyph = hi | glyphs[i].glyph;

    return overall;
}

void QFontEngineMulti::addOutlineToPath(qreal x, qreal y, const QGlyphLayout *glyphs_const, int numGlyphs,
                                        QPainterPath *path, QTextItem::RenderFlags flags)
{
    if (numGlyphs <= 0)
        return;

    QGlyphLayout *glyphs = const_cast<QGlyphLayout *>(glyphs_const);
    int which = highByte(glyphs[0].glyph);
    int start = 0;
    int end, i;
    if (flags & QTextItem::RightToLeft) {
        for (int gl = 0; gl < numGlyphs; gl++) {
            x += glyphs[gl].advance.x.toReal();
            y += glyphs[gl].advance.y.toReal();
        }
    }
    for (end = 0; end < numGlyphs; ++end) {
        const int e = highByte(glyphs[end].glyph);
        if (e == which)
            continue;

        if (flags & QTextItem::RightToLeft) {
            for (i = start; i < end; ++i) {
                x -= glyphs[i].advance.x.toReal();
                y -= glyphs[i].advance.y.toReal();
            }
        }

        // set the high byte to zero
        for (i = start; i < end; ++i)
            glyphs[i].glyph = stripped(glyphs[i].glyph);
        engine(which)->addOutlineToPath(x, y, glyphs + start, end - start, path, flags);
        // reset the high byte for all glyphs and update x and y
        const int hi = which << 24;
        for (i = start; i < end; ++i)
            glyphs[i].glyph = hi | glyphs[i].glyph;

        if (!(flags & QTextItem::RightToLeft)) {
            for (i = start; i < end; ++i) {
                x += glyphs[i].advance.x.toReal();
                y += glyphs[i].advance.y.toReal();
            }
        }

        // change engine
        start = end;
        which = e;
    }

    if (flags & QTextItem::RightToLeft) {
        for (i = start; i < end; ++i) {
            x -= glyphs[i].advance.x.toReal();
            y -= glyphs[i].advance.y.toReal();
        }
    }

    // set the high byte to zero
    for (i = start; i < end; ++i)
        glyphs[i].glyph = stripped(glyphs[i].glyph);

    engine(which)->addOutlineToPath(x, y, glyphs + start, end - start, path, flags);

    // reset the high byte for all glyphs
    const int hi = which << 24;
    for (i = start; i < end; ++i)
        glyphs[i].glyph = hi | glyphs[i].glyph;
}

void QFontEngineMulti::recalcAdvances(int numGlyphs, QGlyphLayout *glyphs, QTextEngine::ShaperFlags flags) const
{
    if (numGlyphs <= 0)
        return;

    int which = highByte(glyphs[0].glyph);
    int start = 0;
    int end, i;
    for (end = 0; end < numGlyphs; ++end) {
        const int e = highByte(glyphs[end].glyph);
        if (e == which)
            continue;

        // set the high byte to zero
        for (i = start; i < end; ++i)
            glyphs[i].glyph = stripped(glyphs[i].glyph);

        engine(which)->recalcAdvances(end - start, glyphs + start, flags);

        // reset the high byte for all glyphs and update x and y
        const int hi = which << 24;
        for (i = start; i < end; ++i)
            glyphs[i].glyph = hi | glyphs[i].glyph;

        // change engine
        start = end;
        which = e;
    }

    // set the high byte to zero
    for (i = start; i < end; ++i)
        glyphs[i].glyph = stripped(glyphs[i].glyph);

    engine(which)->recalcAdvances(end - start, glyphs + start, flags);

    // reset the high byte for all glyphs
    const int hi = which << 24;
    for (i = start; i < end; ++i)
        glyphs[i].glyph = hi | glyphs[i].glyph;
}

void QFontEngineMulti::doKerning(int numGlyphs, QGlyphLayout *glyphs, QTextEngine::ShaperFlags flags) const
{
    if (numGlyphs <= 0)
        return;

    int which = highByte(glyphs[0].glyph);
    int start = 0;
    int end, i;
    for (end = 0; end < numGlyphs; ++end) {
        const int e = highByte(glyphs[end].glyph);
        if (e == which)
            continue;

        // set the high byte to zero
        for (i = start; i < end; ++i)
            glyphs[i].glyph = stripped(glyphs[i].glyph);

        engine(which)->doKerning(end - start, glyphs + start, flags);

        // reset the high byte for all glyphs and update x and y
        const int hi = which << 24;
        for (i = start; i < end; ++i)
            glyphs[i].glyph = hi | glyphs[i].glyph;

        // change engine
        start = end;
        which = e;
    }

    // set the high byte to zero
    for (i = start; i < end; ++i)
        glyphs[i].glyph = stripped(glyphs[i].glyph);

    engine(which)->doKerning(end - start, glyphs + start, flags);

    // reset the high byte for all glyphs
    const int hi = which << 24;
    for (i = start; i < end; ++i)
        glyphs[i].glyph = hi | glyphs[i].glyph;
}

glyph_metrics_t QFontEngineMulti::boundingBox(glyph_t glyph)
{
    const int which = highByte(glyph);
    Q_ASSERT(which < engines.size());
    return engine(which)->boundingBox(stripped(glyph));
}

QFixed QFontEngineMulti::ascent() const
{ return engine(0)->ascent(); }

QFixed QFontEngineMulti::descent() const
{ return engine(0)->descent(); }

QFixed QFontEngineMulti::leading() const
{
    return engine(0)->leading();
}

QFixed QFontEngineMulti::xHeight() const
{
    return engine(0)->xHeight();
}

QFixed QFontEngineMulti::averageCharWidth() const
{
    return engine(0)->averageCharWidth();
}

QFixed QFontEngineMulti::lineThickness() const
{
    return engine(0)->lineThickness();
}

QFixed QFontEngineMulti::underlinePosition() const
{
    return engine(0)->underlinePosition();
}

qreal QFontEngineMulti::maxCharWidth() const
{
    return engine(0)->maxCharWidth();
}

qreal QFontEngineMulti::minLeftBearing() const
{
    return engine(0)->minLeftBearing();
}

qreal QFontEngineMulti::minRightBearing() const
{
    return engine(0)->minRightBearing();
}

bool QFontEngineMulti::canRender(const QChar *string, int len)
{
    if (engine(0)->canRender(string, len))
        return true;

    QVarLengthArray<QGlyphLayout, 256> glyphs(len);
    int nglyphs = len;
    if (stringToCMap(string, len, glyphs.data(), &nglyphs, QTextEngine::GlyphIndicesOnly) == false) {
        glyphs.resize(nglyphs);
        stringToCMap(string, len, glyphs.data(), &nglyphs, QTextEngine::GlyphIndicesOnly);
    }

    bool allExist = true;
    for (int i = 0; i < nglyphs; i++) {
        if (!glyphs[i].glyph) {
            allExist = false;
            break;
        }
    }

    return allExist;
}

QFontEngine *QFontEngineMulti::engine(int at) const
{
    Q_ASSERT(at < engines.size());
    return engines.at(at);
}

QT_END_NAMESPACE
