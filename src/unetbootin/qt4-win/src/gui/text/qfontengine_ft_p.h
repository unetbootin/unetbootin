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
#ifndef QFONTENGINE_FT_P_H
#define QFONTENGINE_FT_P_H
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

#include "qfontengine_p.h"

#ifndef QT_NO_FREETYPE

#include <ft2build.h>
#include FT_FREETYPE_H

#if defined(Q_WS_X11)
#include <private/qt_x11_p.h>
#endif

#include <unistd.h>

#ifndef QT_NO_FONTCONFIG
#include <fontconfig/fontconfig.h>
#endif

#include <qmutex.h>

#include <harfbuzz-shaper.h>

QT_BEGIN_NAMESPACE

struct QFreetypeFace
{
    void computeSize(const QFontDef &fontDef, int *xsize, int *ysize, bool *outline_drawing);
    QFontEngine::Properties properties() const;
    bool getSfntTable(uint tag, uchar *buffer, uint *length) const;

    static QFreetypeFace *getFace(const QFontEngine::FaceId &face_id);
    void release(const QFontEngine::FaceId &face_id);

    void lock()
    {
        _lock.lock();
    }
    void unlock()
    {
        _lock.unlock();
    }

    FT_Face face;
    HB_Face hbFace;
#ifndef QT_NO_FONTCONFIG
    FcCharSet *charset;
#endif
    int xsize; // 26.6
    int ysize; // 26.6
    FT_Matrix matrix;
    FT_CharMap unicode_map;
    FT_CharMap symbol_map;

    enum { cmapCacheSize = 0x200 };
    glyph_t cmapCache[cmapCacheSize];

    int fsType() const;

    HB_Error getPointInOutline(HB_Glyph glyph, int flags, hb_uint32 point, HB_Fixed *xpos, HB_Fixed *ypos, hb_uint32 *nPoints);

    static void addGlyphToPath(FT_Face face, FT_GlyphSlot g, const QFixedPoint &point, QPainterPath *path, FT_Fixed x_scale, FT_Fixed y_scale);
    static void addBitmapToPath(FT_GlyphSlot slot, const QFixedPoint &point, QPainterPath *path, bool = false);

private:
    QFreetypeFace() : _lock(QMutex::Recursive) {}
    ~QFreetypeFace() {}
    QAtomicInt ref;
    QMutex _lock;
    QByteArray fontData;
};

class Q_GUI_EXPORT QFontEngineFT : public QFontEngine
{
public:
    enum GlyphFormat {
        Format_None,
        Format_Render = Format_None,
        Format_Mono,
        Format_A8,
        Format_A32
    };

    /* we don't cache glyphs that are too large anyway, so we can make this struct rather small */
    struct Glyph {
        ~Glyph();
        short linearAdvance;
        unsigned char width;
        unsigned char height;
        signed char x;
        signed char y;
        signed char advance;
        signed char format;
        uchar *data;
        unsigned int uploadedToServer : 1;
    };

    enum SubpixelAntialiasingType {
        Subpixel_None,
        Subpixel_RGB,
        Subpixel_BGR,
        Subpixel_VRGB,
        Subpixel_VBGR
    };

#if defined(Q_WS_X11) && !defined(QT_NO_XRENDER)
    typedef XGlyphInfo GlyphInfo;
#else
    struct GlyphInfo {
        unsigned short  width;
        unsigned short  height;
        short           x;
        short           y;
        short           xOff;
        short           yOff;
    };
#endif

    struct QGlyphSet
    {
        QGlyphSet();
        ~QGlyphSet();
        FT_Matrix transformationMatrix;
        unsigned long id; // server sided id, GlyphSet for X11
        mutable QHash<int, Glyph *> glyph_data; // maps from glyph index to glyph data
    };

    QFontEngine::FaceId faceId() const;
    QFontEngine::Properties properties() const;
    QFixed emSquareSize() const;

    bool getSfntTableData(uint tag, uchar *buffer, uint *length) const;
    int synthesized() const;

    QFixed ascent() const;
    QFixed descent() const;
    QFixed leading() const;
    QFixed xHeight() const;
    QFixed averageCharWidth() const;

    qreal maxCharWidth() const;
    qreal minLeftBearing() const;
    qreal minRightBearing() const;
    QFixed lineThickness() const;
    QFixed underlinePosition() const;

    void doKerning(int , QGlyphLayout *, QTextEngine::ShaperFlags) const;

    inline Type type() const
    { return QFontEngine::Freetype; }
    inline const char *name() const
    { return "freetype"; }

    void getUnscaledGlyph(glyph_t glyph, QPainterPath *path, glyph_metrics_t *metrics);

    bool canRender(const QChar *string, int len);

    void addGlyphsToPath(glyph_t *glyphs, QFixedPoint *positions, int nglyphs,
                         QPainterPath *path, QTextItem::RenderFlags flags);
    void addOutlineToPath(qreal x, qreal y, const QGlyphLayout *glyphs, int numGlyphs,
                          QPainterPath *path, QTextItem::RenderFlags flags);

    bool stringToCMap(const QChar *str, int len, QGlyphLayout *glyphs, int *nglyphs,
                      QTextEngine::ShaperFlags flags) const;

    glyph_metrics_t boundingBox(const QGlyphLayout *glyphs, int numGlyphs);
    glyph_metrics_t boundingBox(glyph_t glyph);

    void recalcAdvances(int len, QGlyphLayout *glyphs, QTextEngine::ShaperFlags flags) const;
    virtual QImage alphaMapForGlyph(glyph_t);
    virtual void removeGlyphFromCache(glyph_t glyph);

    virtual int glyphCount() const;

    enum Scaling {
        Scaled,
        Unscaled
    };
    FT_Face lockFace(Scaling scale = Scaled) const;
    void unlockFace() const;

    FT_Face non_locked_face() const;

    inline bool drawAsOutline() const { return outline_drawing; }
    inline bool drawAntialiased() const { return antialias; }
    inline bool invalid() const { return xsize == 0 && ysize == 0; }
    inline bool isBitmapFont() const { return defaultGlyphFormat == Format_Mono; }

    inline Glyph *loadGlyph(uint glyph, GlyphFormat format = Format_None) const
    { return loadGlyph(&defaultGlyphSet, glyph, format); }
    Glyph *loadGlyph(QGlyphSet *set, uint glyph, GlyphFormat = Format_None) const;

    QGlyphSet *defaultGlyphs() { return &defaultGlyphSet; }

    inline Glyph *cachedGlyph(glyph_t g) const { return defaultGlyphSet.glyph_data.value(g); }

    QGlyphSet *loadTransformedGlyphSet(glyph_t *glyphs, int num_glyphs, const QTransform &matrix,
                                       GlyphFormat format = Format_Render);

#if defined(Q_WS_QWS)
    virtual void draw(QPaintEngine * /*p*/, qreal /*x*/, qreal /*y*/, const QTextItemInt & /*si*/) {}
#endif

    QFontEngineFT(const QFontDef &fd);
    virtual ~QFontEngineFT();

    bool init(FaceId faceId, bool antiaalias, GlyphFormat defaultFormat = Format_None);

    virtual HB_Error getPointInOutline(HB_Glyph glyph, int flags, hb_uint32 point, HB_Fixed *xpos, HB_Fixed *ypos, hb_uint32 *nPoints);

protected:

    void freeGlyphSets();

    virtual bool uploadGlyphToServer(QGlyphSet *set, uint glyphid, Glyph *g, GlyphInfo *info, int glyphDataSize) const;
    virtual unsigned long allocateServerGlyphSet();
    virtual void freeServerGlyphSet(unsigned long id);

    QFreetypeFace *freetype;
    int default_load_flags;

    bool antialias;
    bool outline_drawing;
    bool transform;
    SubpixelAntialiasingType subpixelType;
    bool canUploadGlyphsToServer;

private:
    GlyphFormat defaultGlyphFormat;
    FT_Matrix matrix;

    QList<QGlyphSet> transformedGlyphSets;
    mutable QGlyphSet defaultGlyphSet;

    QFontEngine::FaceId face_id;

    int xsize;
    int ysize;

    mutable QFixed lbearing;
    mutable QFixed rbearing;
    QFixed line_thickness;
    QFixed underline_position;

    FT_Size_Metrics metrics;
    mutable bool kerning_pairs_loaded;
};

QT_END_NAMESPACE

#endif // QT_NO_FREETYPE

#endif // QFONTENGINE_FT_P_H
