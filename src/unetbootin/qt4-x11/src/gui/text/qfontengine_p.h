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

#ifndef QFONTENGINE_P_H
#define QFONTENGINE_P_H

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

#include "QtCore/qglobal.h"
#include "QtCore/qatomic.h"
#include <QtCore/qvarlengtharray.h>
#include "private/qtextengine_p.h"
#include "private/qfont_p.h"

#ifdef Q_WS_WIN
#   include "QtCore/qt_windows.h"
#endif

#ifdef Q_WS_MAC
#   include "private/qt_mac_p.h"
#   include "QtCore/qmap.h"
#   include "QtCore/qcache.h"
#   include "private/qcore_mac_p.h"
#endif

#include "qfontengineglyphcache_p.h"

struct glyph_metrics_t;
typedef unsigned int glyph_t;

QT_BEGIN_NAMESPACE

class QChar;
class QPainterPath;

class QTextEngine;
struct QGlyphLayout;

#define MAKE_TAG(ch1, ch2, ch3, ch4) (\
    (((quint32)(ch1)) << 24) | \
    (((quint32)(ch2)) << 16) | \
    (((quint32)(ch3)) << 8) | \
    ((quint32)(ch4)) \
   )


class Q_GUI_EXPORT QFontEngine : public QObject
{
    Q_OBJECT
public:
    enum Type {
        Box,
        Multi,

        // X11 types
        XLFD,

        // MS Windows types
        Win,

        // Apple Mac OS types
        Mac,

        // Trolltech QWS types
        Freetype,
        QPF1,
        QPF2,
        Proxy,
        TestFontEngine = 0x1000
    };

    QFontEngine();
    virtual ~QFontEngine();

    // all of these are in unscaled metrics if the engine supports uncsaled metrics,
    // otherwise in design metrics
    struct Properties {
        QByteArray postscriptName;
        QByteArray copyright;
        QRectF boundingBox;
        QFixed emSquare;
        QFixed ascent;
        QFixed descent;
        QFixed leading;
        QFixed italicAngle;
        QFixed capHeight;
        QFixed lineWidth;
    };
    virtual Properties properties() const;
    virtual void getUnscaledGlyph(glyph_t glyph, QPainterPath *path, glyph_metrics_t *metrics);
    QByteArray getSfntTable(uint /*tag*/) const;
    virtual bool getSfntTableData(uint /*tag*/, uchar * /*buffer*/, uint * /*length*/) const { return false; }

    struct FaceId {
        FaceId() : index(0), encoding(0) {}
        QByteArray filename;
        int index;
        int encoding;
    };
    virtual FaceId faceId() const { return FaceId(); }
    enum SynthesizedFlags {
        SynthesizedItalic = 0x1,
        SynthesizedBold = 0x2,
        SynthesizedStretch = 0x4
    };
    virtual int synthesized() const { return 0; }

    virtual QFixed emSquareSize() const { return ascent(); }

    /* returns 0 as glyph index for non existant glyphs */
    virtual bool stringToCMap(const QChar *str, int len, QGlyphLayout *glyphs, int *nglyphs, QTextEngine::ShaperFlags flags) const = 0;

    /**
     * This is a callback from harfbuzz. The font engine uses the font-system in use to find out the
     * advances of each glyph and set it on the layout.
     */
    virtual void recalcAdvances(QGlyphLayout *, QTextEngine::ShaperFlags) const {}
    virtual void doKerning(QGlyphLayout *, QTextEngine::ShaperFlags) const;

#if !defined(Q_WS_X11) && !defined(Q_WS_WIN) && !defined(Q_WS_MAC)
    virtual void draw(QPaintEngine *p, qreal x, qreal y, const QTextItemInt &si) = 0;
#endif
    virtual void addGlyphsToPath(glyph_t *glyphs, QFixedPoint *positions, int nglyphs,
                                 QPainterPath *path, QTextItem::RenderFlags flags);
    void getGlyphPositions(const QGlyphLayout &glyphs, const QTransform &matrix, QTextItem::RenderFlags flags,
                           QVarLengthArray<glyph_t> &glyphs_out, QVarLengthArray<QFixedPoint> &positions);

    virtual void addOutlineToPath(qreal, qreal, const QGlyphLayout &, QPainterPath *, QTextItem::RenderFlags flags);
    void addBitmapFontToPath(qreal x, qreal y, const QGlyphLayout &, QPainterPath *, QTextItem::RenderFlags);
    /**
     * Create a qimage with the alpha values for the glyph.
     * Returns an image indexed_8 with index values ranging from 0=fully transparant to 255=opaque
     */
    virtual QImage alphaMapForGlyph(glyph_t) = 0;
    virtual QImage alphaMapForGlyph(glyph_t, const QTransform &t);
    virtual QImage alphaRGBMapForGlyph(glyph_t, int margin, const QTransform &t);

    virtual void removeGlyphFromCache(glyph_t);

    virtual glyph_metrics_t boundingBox(const QGlyphLayout &glyphs) = 0;
    virtual glyph_metrics_t boundingBox(glyph_t glyph) = 0;
    virtual glyph_metrics_t boundingBox(glyph_t glyph, const QTransform &matrix);
    glyph_metrics_t tightBoundingBox(const QGlyphLayout &glyphs);

    virtual QFixed ascent() const = 0;
    virtual QFixed descent() const = 0;
    virtual QFixed leading() const = 0;
    virtual QFixed xHeight() const;
    virtual QFixed averageCharWidth() const;

    virtual QFixed lineThickness() const;
    virtual QFixed underlinePosition() const;

    virtual qreal maxCharWidth() const = 0;
    virtual qreal minLeftBearing() const { return qreal(); }
    virtual qreal minRightBearing() const { return qreal(); }

    virtual const char *name() const = 0;

    virtual bool canRender(const QChar *string, int len) = 0;

    virtual Type type() const = 0;

    virtual int glyphCount() const;

    HB_Font harfbuzzFont() const;
    HB_Face harfbuzzFace() const;

    virtual HB_Error getPointInOutline(HB_Glyph glyph, int flags, hb_uint32 point, HB_Fixed *xpos, HB_Fixed *ypos, hb_uint32 *nPoints);

    void setGlyphCache(void *key, QFontEngineGlyphCache *data);
    void setGlyphCache(QFontEngineGlyphCache::Type key, QFontEngineGlyphCache *data);
    QFontEngineGlyphCache *glyphCache(void *key, const QTransform &transform) const;
    QFontEngineGlyphCache *glyphCache(QFontEngineGlyphCache::Type key, const QTransform &transform) const;

    static const uchar *getCMap(const uchar *table, uint tableSize, bool *isSymbolFont, int *cmapSize);
    static quint32 getTrueTypeGlyphIndex(const uchar *cmap, uint unicode);

    QAtomicInt ref;
    QFontDef fontDef;
    uint cache_cost; // amount of mem used in kb by the font
    int cache_count;
    uint fsType : 16;
    bool symbol;
    mutable HB_FontRec hbFont;
    mutable HB_Face hbFace;
#if defined(Q_WS_WIN) || defined(Q_WS_X11) || defined(Q_WS_QWS)
    struct KernPair {
        uint left_right;
        QFixed adjust;

        inline bool operator<(const KernPair &other) const
        {
            return left_right < other.left_right;
        }
    };
    QVector<KernPair> kerning_pairs;
    void loadKerningPairs(QFixed scalingFactor);
#endif

    int glyphFormat;

private:
    /// remove old entries from the glyph cache. Helper method for the setGlyphCache ones.
    void expireGlyphCache();

    GlyphPointerHash m_glyphPointerHash;
    GlyphIntHash m_glyphIntHash;
    mutable QList<QFontEngineGlyphCache*> m_glyphCacheQueue;
};

inline bool operator ==(const QFontEngine::FaceId &f1, const QFontEngine::FaceId &f2)
{
    return (f1.index == f2.index) && (f1.encoding == f2.encoding) && (f1.filename == f2.filename);
}

inline uint qHash(const QFontEngine::FaceId &f)
{
    return qHash((f.index << 16) + f.encoding) + qHash(f.filename);
}


class QGlyph;

#if defined(Q_WS_QWS)

#ifndef QT_NO_QWS_QPF

class QFontEngineQPF1Data;

class QFontEngineQPF1 : public QFontEngine
{
public:
    QFontEngineQPF1(const QFontDef&, const QString &fn);
   ~QFontEngineQPF1();

    virtual bool stringToCMap(const QChar *str, int len, QGlyphLayout *glyphs, int *nglyphs, QTextEngine::ShaperFlags flags) const;
    virtual void recalcAdvances(QGlyphLayout *, QTextEngine::ShaperFlags) const;

    virtual void draw(QPaintEngine *p, qreal x, qreal y, const QTextItemInt &si);
    virtual void addOutlineToPath(qreal x, qreal y, const QGlyphLayout &glyphs, QPainterPath *path, QTextItem::RenderFlags flags);

    virtual glyph_metrics_t boundingBox(const QGlyphLayout &glyphs);
    virtual glyph_metrics_t boundingBox(glyph_t glyph);

    virtual QFixed ascent() const;
    virtual QFixed descent() const;
    virtual QFixed leading() const;
    virtual qreal maxCharWidth() const;
    virtual qreal minLeftBearing() const;
    virtual qreal minRightBearing() const;
    virtual QFixed underlinePosition() const;
    virtual QFixed lineThickness() const;

    virtual Type type() const;

    virtual bool canRender(const QChar *string, int len);
    inline const char *name() const { return 0; }
    virtual QImage alphaMapForGlyph(glyph_t);


    QFontEngineQPF1Data *d;
};
#endif // QT_NO_QWS_QPF

#endif // QWS


class QFontEngineBox : public QFontEngine
{
public:
    QFontEngineBox(int size);
    ~QFontEngineBox();

    virtual bool stringToCMap(const QChar *str, int len, QGlyphLayout *glyphs, int *nglyphs, QTextEngine::ShaperFlags flags) const;
    virtual void recalcAdvances(QGlyphLayout *, QTextEngine::ShaperFlags) const;

#if !defined(Q_WS_X11) && !defined(Q_WS_WIN) && !defined(Q_WS_MAC)
    void draw(QPaintEngine *p, qreal x, qreal y, const QTextItemInt &si);
#endif
    virtual void addOutlineToPath(qreal x, qreal y, const QGlyphLayout &glyphs, QPainterPath *path, QTextItem::RenderFlags flags);

    virtual glyph_metrics_t boundingBox(const QGlyphLayout &glyphs);
    virtual glyph_metrics_t boundingBox(glyph_t glyph);

    virtual QFixed ascent() const;
    virtual QFixed descent() const;
    virtual QFixed leading() const;
    virtual qreal maxCharWidth() const;
    virtual qreal minLeftBearing() const { return 0; }
    virtual qreal minRightBearing() const { return 0; }
    virtual QImage alphaMapForGlyph(glyph_t);

#ifdef Q_WS_X11
    int cmap() const;
#endif
    virtual const char *name() const;

    virtual bool canRender(const QChar *string, int len);

    virtual Type type() const;
    inline int size() const { return _size; }

private:
    friend class QFontPrivate;
    int _size;
};

class Q_GUI_EXPORT QFontEngineMulti : public QFontEngine
{
public:
    explicit QFontEngineMulti(int engineCount);
    ~QFontEngineMulti();

    virtual bool stringToCMap(const QChar *str, int len, QGlyphLayout *glyphs, int *nglyphs,
                      QTextEngine::ShaperFlags flags) const;

    virtual glyph_metrics_t boundingBox(const QGlyphLayout &glyphs);
    virtual glyph_metrics_t boundingBox(glyph_t glyph);

    virtual void recalcAdvances(QGlyphLayout *, QTextEngine::ShaperFlags) const;
    virtual void doKerning(QGlyphLayout *, QTextEngine::ShaperFlags) const;
    virtual void addOutlineToPath(qreal, qreal, const QGlyphLayout &, QPainterPath *, QTextItem::RenderFlags flags);

    virtual QFixed ascent() const;
    virtual QFixed descent() const;
    virtual QFixed leading() const;
    virtual QFixed xHeight() const;
    virtual QFixed averageCharWidth() const;
    virtual QImage alphaMapForGlyph(glyph_t);

    virtual QFixed lineThickness() const;
    virtual QFixed underlinePosition() const;
    virtual qreal maxCharWidth() const;
    virtual qreal minLeftBearing() const;
    virtual qreal minRightBearing() const;

    virtual inline Type type() const
    { return QFontEngine::Multi; }

    virtual bool canRender(const QChar *string, int len);
    inline virtual const char *name() const
    { return "Multi"; }

    QFontEngine *engine(int at) const;

protected:
    friend class QPSPrintEnginePrivate;
    friend class QPSPrintEngineFontMulti;
    virtual void loadEngine(int at) = 0;
    QVector<QFontEngine *> engines;
};

#if defined(Q_WS_MAC)

struct QCharAttributes;
class QFontEngineMacMulti;
#  if MAC_OS_X_VERSION_MAX_ALLOWED >= MAC_OS_X_VERSION_10_5
class QCoreTextFontEngineMulti;
class QCoreTextFontEngine : public QFontEngine
{
public:
    QCoreTextFontEngine(CTFontRef font, const QFontDef &def,
                        QCoreTextFontEngineMulti *multiEngine = 0);
    ~QCoreTextFontEngine();
    virtual bool stringToCMap(const QChar *str, int len, QGlyphLayout *glyphs, int *nglyphs, QTextEngine::ShaperFlags flags) const;
    virtual void recalcAdvances(int , QGlyphLayout *, QTextEngine::ShaperFlags) const;

    virtual glyph_metrics_t boundingBox(const QGlyphLayout &glyphs);
    virtual glyph_metrics_t boundingBox(glyph_t glyph);

    virtual QFixed ascent() const;
    virtual QFixed descent() const;
    virtual QFixed leading() const;
    virtual QFixed xHeight() const;
    virtual qreal maxCharWidth() const;
    virtual QFixed averageCharWidth() const;

    virtual void addGlyphsToPath(glyph_t *glyphs, QFixedPoint *positions, int numGlyphs,
                                 QPainterPath *path, QTextItem::RenderFlags);

    virtual const char *name() const { return "QCoreTextFontEngine"; }

    virtual bool canRender(const QChar *string, int len);

    virtual int synthesized() const { return synthesisFlags; }

    virtual Type type() const { return QFontEngine::Mac; }

    void draw(CGContextRef ctx, qreal x, qreal y, const QTextItemInt &ti, int paintDeviceHeight);

    virtual FaceId faceId() const;
    virtual bool getSfntTableData(uint /*tag*/, uchar * /*buffer*/, uint * /*length*/) const;
    virtual void getUnscaledGlyph(glyph_t glyph, QPainterPath *path, glyph_metrics_t *metrics);
    virtual QImage alphaMapForGlyph(glyph_t);
    virtual qreal minRightBearing() const;
    virtual qreal minLeftBearing() const;



private:
    CTFontRef ctfont;
    CGFontRef cgFont;
    QCoreTextFontEngineMulti *parentEngine;
    int synthesisFlags;
    friend class QCoreTextFontEngineMulti;
};

class QCoreTextFontEngineMulti : public QFontEngineMulti
{
public:
    QCoreTextFontEngineMulti(const ATSFontFamilyRef &atsFamily, const ATSFontRef &atsFontRef,
                             const QFontDef &fontDef, bool kerning);
    ~QCoreTextFontEngineMulti();

    virtual bool stringToCMap(const QChar *str, int len, QGlyphLayout *glyphs, int *nglyphs,
                              QTextEngine::ShaperFlags flags) const;
    bool stringToCMap(const QChar *str, int len, QGlyphLayout *glyphs, int *nglyphs,
                      QTextEngine::ShaperFlags flags,
                      unsigned short *logClusters, const HB_CharAttributes *charAttributes) const;


    virtual void recalcAdvances(int , QGlyphLayout *, QTextEngine::ShaperFlags) const;
    virtual void doKerning(int , QGlyphLayout *, QTextEngine::ShaperFlags) const;

    virtual const char *name() const { return "CoreText"; }
protected:
    virtual void loadEngine(int at);

private:
    inline const QCoreTextFontEngine *engineAt(int i) const
    { return static_cast<const QCoreTextFontEngine *>(engines.at(i)); }

    uint fontIndexForFont(CTFontRef id) const;
    CTFontRef ctfont;
    mutable QCFType<CFDictionaryRef> attributeDict;

    friend class QFontDialogPrivate;
};
#  endif //MAC_OS_X_VERSION_MAX_ALLOWED >= MAC_OS_X_VERSION_10_5

#ifndef QT_MAC_USE_COCOA
class QFontEngineMac : public QFontEngine
{
    friend class QFontEngineMacMulti;
public:
    QFontEngineMac(ATSUStyle baseStyle, ATSUFontID fontID, const QFontDef &def, QFontEngineMacMulti *multiEngine = 0);
    virtual ~QFontEngineMac();

    virtual bool stringToCMap(const QChar *str, int len, QGlyphLayout *glyphs, int *numGlyphs, QTextEngine::ShaperFlags flags) const;
    virtual void recalcAdvances(QGlyphLayout *, QTextEngine::ShaperFlags) const;

    virtual glyph_metrics_t boundingBox(const QGlyphLayout &glyphs);
    virtual glyph_metrics_t boundingBox(glyph_t glyph);

    virtual QFixed ascent() const;
    virtual QFixed descent() const;
    virtual QFixed leading() const;
    virtual QFixed xHeight() const;
    virtual qreal maxCharWidth() const;
    virtual QFixed averageCharWidth() const;

    virtual void addGlyphsToPath(glyph_t *glyphs, QFixedPoint *positions, int numGlyphs,
                                 QPainterPath *path, QTextItem::RenderFlags);

    virtual const char *name() const { return "QFontEngineMac"; }

    virtual bool canRender(const QChar *string, int len);

    virtual int synthesized() const { return synthesisFlags; }

    virtual Type type() const { return QFontEngine::Mac; }

    void draw(CGContextRef ctx, qreal x, qreal y, const QTextItemInt &ti, int paintDeviceHeight);

    virtual FaceId faceId() const;
    virtual QByteArray getSfntTable(uint tag) const;
    virtual Properties properties() const;
    virtual void getUnscaledGlyph(glyph_t glyph, QPainterPath *path, glyph_metrics_t *metrics);
    virtual QImage alphaMapForGlyph(glyph_t);

private:
    ATSUFontID fontID;
    QCFType<CGFontRef> cgFont;
    ATSUStyle style;
    int synthesisFlags;
    mutable QGlyphLayout kashidaGlyph;
    QFontEngineMacMulti *multiEngine;
    mutable const unsigned char *cmap;
    mutable bool symbolCMap;
    mutable QByteArray cmapTable;
    CGAffineTransform transform;
    QFixed m_ascent;
    QFixed m_descent;
    QFixed m_leading;
    qreal m_maxCharWidth;
    QFixed m_xHeight;
    QFixed m_averageCharWidth;
};

class QFontEngineMacMulti : public QFontEngineMulti
{
    friend class QFontEngineMac;
public:
    // internal
    struct ShaperItem
    {
        inline ShaperItem() : string(0), from(0), length(0),
        log_clusters(0), charAttributes(0) {}

        const QChar *string;
        int from;
        int length;
        QGlyphLayout glyphs;
        unsigned short *log_clusters;
        const HB_CharAttributes *charAttributes;
        QTextEngine::ShaperFlags flags;
    };

    QFontEngineMacMulti(const ATSFontFamilyRef &atsFamily, const ATSFontRef &atsFontRef, const QFontDef &fontDef, bool kerning);
    virtual ~QFontEngineMacMulti();

    virtual bool stringToCMap(const QChar *str, int len, QGlyphLayout *glyphs, int *nglyphs, QTextEngine::ShaperFlags flags) const;
    bool stringToCMap(const QChar *str, int len, QGlyphLayout *glyphs, int *nglyphs, QTextEngine::ShaperFlags flags,
                      unsigned short *logClusters, const HB_CharAttributes *charAttributes) const;

    virtual void recalcAdvances(QGlyphLayout *, QTextEngine::ShaperFlags) const;
    virtual void doKerning(QGlyphLayout *, QTextEngine::ShaperFlags) const;

    virtual const char *name() const { return "ATSUI"; }

    virtual bool canRender(const QChar *string, int len);

    inline ATSUFontID macFontID() const { return fontID; }

protected:
    virtual void loadEngine(int at);

private:
    inline const QFontEngineMac *engineAt(int i) const
    { return static_cast<const QFontEngineMac *>(engines.at(i)); }

    bool stringToCMapInternal(const QChar *str, int len, QGlyphLayout *glyphs, int *nglyphs, QTextEngine::ShaperFlags flags, ShaperItem *item) const;

    int fontIndexForFontID(ATSUFontID id) const;

    ATSUFontID fontID;
    uint kerning : 1;

    mutable ATSUTextLayout textLayout;
    mutable ATSUStyle style;
    CGAffineTransform transform;
};
#endif //!QT_MAC_USE_COCOA
#endif

class QTestFontEngine : public QFontEngineBox
{
public:
    QTestFontEngine(int size) : QFontEngineBox(size) {}
    virtual Type type() const { return TestFontEngine; }
};

QT_END_NAMESPACE

#ifdef Q_WS_WIN
#   include "private/qfontengine_win_p.h"
#endif

#endif // QFONTENGINE_P_H
