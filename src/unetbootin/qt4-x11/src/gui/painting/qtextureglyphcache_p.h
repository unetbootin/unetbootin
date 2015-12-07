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

#ifndef QTEXTUREGLYPHCACHE_P_H
#define QTEXTUREGLYPHCACHE_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists for the convenience
// of other Qt classes.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include <qhash.h>
#include <qimage.h>
#include <qobject.h>
#include <qtransform.h>

#include <private/qfontengineglyphcache_p.h>

struct glyph_metrics_t;
typedef unsigned int glyph_t;


QT_BEGIN_NAMESPACE

class QTextItemInt;

class Q_GUI_EXPORT QTextureGlyphCache : public QFontEngineGlyphCache
{
public:
    QTextureGlyphCache(QFontEngineGlyphCache::Type type, const QTransform &matrix)
        : QFontEngineGlyphCache(matrix), m_w(0), m_h(0), m_cx(0), m_cy(0), m_type(type) { }

    virtual ~QTextureGlyphCache() { }

    struct Coord {
        int x;
        int y;
        int w;
        int h;

        int baseLineX;
        int baseLineY;
    };

    void populate(const QTextItemInt &ti,
                  const QVarLengthArray<glyph_t> &glyphs,
                  const QVarLengthArray<QFixedPoint> &positions);

    virtual void createTextureData(int width, int height) = 0;
    virtual void resizeTextureData(int width, int height) = 0;
    virtual int glyphMargin() const { return 0; }

    QFontEngineGlyphCache::Type cacheType() const { return m_type; }

    virtual void fillTexture(const Coord &coord, glyph_t glyph) = 0;

    inline void createCache(int width, int height) {
        m_w = width;
        m_h = height;
        createTextureData(width, height);
    }

    inline bool isNull() const { return m_w <= 0 || m_h <= 0; }

    QHash<glyph_t, Coord> coords;

protected:
    const QTextItemInt *m_current_textitem;

    int m_w; // image width
    int m_h; // image height
    int m_cx; // current x
    int m_cy; // current y
    QFontEngineGlyphCache::Type m_type;

};


class Q_GUI_EXPORT QImageTextureGlyphCache : public QTextureGlyphCache
{
public:
    QImageTextureGlyphCache(QFontEngineGlyphCache::Type type, const QTransform &matrix)
        : QTextureGlyphCache(type, matrix) { }
    virtual int glyphMargin() const;
    virtual void createTextureData(int width, int height);
    virtual void resizeTextureData(int width, int height);
    virtual void fillTexture(const Coord &c, glyph_t glyph);

    inline const QImage &image() const { return m_image; }

private:
    QImage m_image;
};

QT_END_NAMESPACE

#endif
