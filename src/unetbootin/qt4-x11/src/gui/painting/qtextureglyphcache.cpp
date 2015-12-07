/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the QtGui module of the Qt Toolkit.
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

#include <qmath.h>

#include "qtextureglyphcache_p.h"

#include "private/qnumeric_p.h"
#include "private/qnativeimage_p.h"
#include "private/qfontengine_ft_p.h"

QT_BEGIN_NAMESPACE

// #define CACHE_DEBUG

void QTextureGlyphCache::populate(const QTextItemInt &ti,
                                  const QVarLengthArray<glyph_t> &glyphs,
                                  const QVarLengthArray<QFixedPoint> &)
{
#ifdef CACHE_DEBUG
    printf("Populating with '%s'\n", QString::fromRawData(ti.chars, ti.num_chars).toLatin1().data());
    qDebug() << " -> current transformation: " << m_transform;
#endif

    m_current_textitem = &ti;
    const int margin = glyphMargin();

    QHash<glyph_t, Coord> listItemCoordinates;
    int rowHeight = 0;

    // check each glyph for its metrics and get the required rowHeight.
    for (int i=0; i < glyphs.size(); ++i) {
        const glyph_t glyph = glyphs[i];
        if (coords.contains(glyph))
            continue;
        if (listItemCoordinates.contains(glyph))
            continue;
        glyph_metrics_t metrics = ti.fontEngine->boundingBox(glyph, m_transform);

#ifdef CACHE_DEBUG
        printf("'%c' (%4x): w=%.2f, h=%.2f, xoff=%.2f, yoff=%.2f, x=%.2f, y=%.2f, ti.ascent=%.2f, ti.descent=%.2f\n",
               ti.chars[i].toLatin1(),
               glyph,
               metrics.width.toReal(),
               metrics.height.toReal(),
               metrics.xoff.toReal(),
               metrics.yoff.toReal(),
               metrics.x.toReal(),
               metrics.y.toReal(),
               ti.ascent.toReal(),
               ti.descent.toReal());
#endif
        int glyph_width = metrics.width.ceil().toInt() + margin * 2;
        int glyph_height = metrics.height.ceil().toInt() + margin * 2;
        if (glyph_height == 0 || glyph_width == 0)
            continue;

        // align to 8-bit boundary
        if (m_type == QFontEngineGlyphCache::Raster_Mono)
            glyph_width = (glyph_width+7)&~7;

        Coord c = { 0, 0, // will be filled in later
                    glyph_width,
                    glyph_height, // texture coords
                    metrics.x.truncate(),
                    -metrics.y.truncate() }; // baseline for horizontal scripts

        listItemCoordinates.insert(glyph, c);
        rowHeight = qMax(rowHeight, glyph_height);
    }
    if (listItemCoordinates.isEmpty())
        return;

    rowHeight += margin * 2;
    if (isNull())
        createCache(256, rowHeight);

    // now actually use the coords and paint the wanted glyps into cache.
    QHash<glyph_t, Coord>::iterator iter = listItemCoordinates.begin();
    while (iter != listItemCoordinates.end()) {
        Coord c = iter.value();

        if (m_cx + c.w > m_w) {
            // no room on the current line, start new glyph strip
            m_cx = 0;
            m_cy = m_h;
        }
        if (m_cy + c.h > m_h) {
            int new_height;
            if (m_cx == 0) { // add a whole row
                new_height = m_h + rowHeight;
                m_cy = m_h;
            } else { // just extend row
                new_height = m_cy + rowHeight;
            }
            // if no room in the current texture - realloc a larger texture
            resizeTextureData(m_w, new_height);
            m_h = new_height;
        }

        c.x = m_cx;
        c.y = m_cy;

        fillTexture(c, iter.key());
        coords.insert(iter.key(), c);

        if (m_cx + c.w > m_w) {
            m_cx = 0;
            m_cy += rowHeight;
        } else {
            // for the Mono case, glyph_width is 8-bit aligned,
            // and therefore so will m_cx
            m_cx += c.w;
        }
        ++iter;
    }


}

/************************************************************************
 * QImageTextureGlyphCache
 */

void QImageTextureGlyphCache::resizeTextureData(int width, int height)
{
    m_image = m_image.copy(0, 0, width, height);
}

void QImageTextureGlyphCache::createTextureData(int width, int height)
{
    switch (m_type) {
    case QFontEngineGlyphCache::Raster_Mono:
        m_image = QImage(width, height, QImage::Format_Mono);
        break;
    case QFontEngineGlyphCache::Raster_A8: {
        m_image = QImage(width, height, QImage::Format_Indexed8);
        m_image.fill(0);
        QVector<QRgb> colors(256);
        QRgb *it = colors.data();
        for (int i=0; i<256; ++i, ++it)
            *it = 0xff000000 | i | (i<<8) | (i<<16);
        m_image.setColorTable(colors);
        break;   }
    case QFontEngineGlyphCache::Raster_RGBMask:
        m_image = QImage(width, height, QImage::Format_RGB32);
        break;
    }
}

int QImageTextureGlyphCache::glyphMargin() const
{
#ifdef Q_WS_MAC
    return 2;
#else
    return m_type == QFontEngineGlyphCache::Raster_RGBMask ? 2 : 0;
#endif
}

void QImageTextureGlyphCache::fillTexture(const Coord &c, glyph_t g)
{
    QImage mask;
#if defined(Q_WS_X11)
    if (m_transform.type() > QTransform::TxTranslate) {
        QFontEngineFT::GlyphFormat format = QFontEngineFT::Format_None;
        switch (m_type) {
        case Raster_RGBMask:
            format = QFontEngineFT::Format_A32; break;
        case Raster_A8:
            format = QFontEngineFT::Format_A8; break;
        case Raster_Mono:
            format = QFontEngineFT::Format_Mono; break;
        };

        QFontEngineFT *ft = static_cast<QFontEngineFT*> (m_current_textitem->fontEngine);
        QFontEngineFT::QGlyphSet *gset = ft->loadTransformedGlyphSet(m_transform);

        if (gset && ft->loadGlyphs(gset, &g, 1, format)) {
            QFontEngineFT::Glyph *glyph = gset->glyph_data.value(g);
            const int bytesPerLine = (format == QFontEngineFT::Format_Mono ? ((glyph->width + 31) & ~31) >> 3
                               : (glyph->width + 3) & ~3);
            mask = QImage(glyph->data, glyph->width, glyph->height, bytesPerLine, m_image.format());
        }
    } else
#endif
    if (m_type == QFontEngineGlyphCache::Raster_RGBMask)
        mask = m_current_textitem->fontEngine->alphaRGBMapForGlyph(g, glyphMargin(), m_transform);
    else
        mask = m_current_textitem->fontEngine->alphaMapForGlyph(g, m_transform);

#ifdef CACHE_DEBUG
    printf("fillTexture of %dx%d at %d,%d in the cache of %dx%d\n", c.w, c.h, c.x, c.y, m_image.width(), m_image.height());
    if (mask.width() > c.w || mask.height() > c.h) {
        printf("   ERROR; mask is bigger than reserved space! %dx%d instead of %dx%d\n", mask.width(), mask.height(), c.w,c.h);
        return;
    }
#endif

    if (m_type == QFontEngineGlyphCache::Raster_RGBMask) {
        QPainter p(&m_image);
        p.setCompositionMode(QPainter::CompositionMode_Source);
        p.fillRect(c.x, c.y, c.w, c.h, QColor(0,0,0,0)); // TODO optimize this
        p.drawImage(c.x, c.y, mask);
        p.end();
    } else if (m_type == QFontEngineGlyphCache::Raster_Mono) {
        if (mask.depth() > 1) {
            // TODO optimize this
            mask = mask.alphaChannel();
            mask.invertPixels();
            mask = mask.convertToFormat(QImage::Format_Mono);
        }

        int mw = qMin(mask.width(), c.w);
        int mh = qMin(mask.height(), c.h);
        uchar *d = m_image.bits();
        int dbpl = m_image.bytesPerLine();

        for (int y = 0; y < c.h; ++y) {
            uchar *dest = d + (c.y + y) *dbpl + c.x/8;

            if (y < mh) {
                uchar *src = mask.scanLine(y);
                for (int x = 0; x < c.w/8; ++x) {
                    if (x < (mw+7)/8)
                        dest[x] = src[x];
                    else
                        dest[x] = 0;
                }
            } else {
                for (int x = 0; x < c.w/8; ++x)
                    dest[x] = 0;
            }
        }
    } else { // A8
        int mw = qMin(mask.width(), c.w);
        int mh = qMin(mask.height(), c.h);
        uchar *d = m_image.bits();
        int dbpl = m_image.bytesPerLine();

        if (mask.depth() == 1) {
            for (int y = 0; y < c.h; ++y) {
                uchar *dest = d + (c.y + y) *dbpl + c.x;
                if (y < mh) {
                    uchar *src = (uchar *) mask.scanLine(y);
                    for (int x = 0; x < c.w; ++x) {
                        if (x < mw)
                            dest[x] = (src[x >> 3] & (1 << (7 - (x & 7)))) > 0 ? 255 : 0;
                    }
                }
            }
        } else if (mask.depth() == 8) {
            for (int y = 0; y < c.h; ++y) {
                uchar *dest = d + (c.y + y) *dbpl + c.x;
                if (y < mh) {
                    uchar *src = (uchar *) mask.scanLine(y);
                    for (int x = 0; x < c.w; ++x) {
                        if (x < mw)
                            dest[x] = src[x];
                    }
                }
            }
        }
    }

#ifdef CACHE_DEBUG
//     QPainter p(&m_image);
//     p.drawLine(
    QPoint base(c.x + glyphMargin(), c.y + glyphMargin() + c.baseLineY-1);
    if (m_image.rect().contains(base))
        m_image.setPixel(base, 255);
    m_image.save(QString::fromLatin1("cache-%1-%2-%3.png")
                 .arg(m_current_textitem->font().family())
                 .arg(m_current_textitem->font().pointSize())
                 .arg(m_transform.type()));
#endif
}

QT_END_NAMESPACE
