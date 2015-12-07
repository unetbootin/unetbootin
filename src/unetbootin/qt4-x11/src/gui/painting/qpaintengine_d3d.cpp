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

#include <qdebug.h>
#include "qpaintengine_d3d_p.h"

#include "private/qdrawhelper_p.h"
#include "private/qfont_p.h"
#include "private/qfontengine_p.h"
#include "private/qpaintengine_p.h"
#include "private/qtessellator_p.h"
#include <private/qbezier_p.h>
#include <private/qpainter_p.h>
#include <private/qpixmap_raster_p.h>
#include <private/qpolygonclipper_p.h>
#include <qbuffer.h>
#include <qcache.h>
#include <qdir.h>
#include <qfileinfo.h>
#include <qlibrary.h>
#include <qlibraryinfo.h>
#include <qmath.h>
#include <qpaintdevice.h>
#include <qpixmapcache.h>

#include <qwidget.h>
#include <d3d9.h>
#include <d3dx9.h>

#include <mmintrin.h>
#include <xmmintrin.h>

QT_BEGIN_NAMESPACE

#ifndef M_PI
    #define M_PI 3.14159265358979323846
#endif

#define QD3D_MASK_MARGIN 1
#define QD3D_BATCH_SIZE 256

// for the ClearType detection stuff..
#ifndef SPI_GETFONTSMOOTHINGTYPE
#define SPI_GETFONTSMOOTHINGTYPE 0x200A
#endif

#ifndef FE_FONTSMOOTHINGCLEARTYPE
#define FE_FONTSMOOTHINGCLEARTYPE 0x0002
#endif

//#include <performance.h>
#define PM_INIT
#define PM_MEASURE(A)
#define PM_DISPLAY

//debugging
//#define QT_DEBUG_VERTEXBUFFER_ACCESS
//#define QT_DEBUG_D3D
//#define QT_DEBUG_D3D_CALLS

#define QD3D_SET_MARK(output) \
    D3DPERF_SetMarker(0, QString(output).utf16());

#define QT_VERTEX_RESET_LIMIT   24576
#define QT_VERTEX_BUF_SIZE      32768
#define QD3DFVF_CSVERTEX (D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX2 | D3DFVF_TEXCOORDSIZE4(0) |  D3DFVF_TEXCOORDSIZE4(1))

// this is a different usage of the effect framework than intended,
// but it's convenient for us to use (See effect file)
#define PASS_STENCIL_ODDEVEN                0
#define PASS_STENCIL_WINDING                1
#define PASS_STENCIL_DRAW                   2
#define PASS_STENCIL_DRAW_DIRECT            3
#define PASS_STENCIL_CLIP                   4
#define PASS_STENCIL_NOSTENCILCHECK         5
#define PASS_STENCIL_NOSTENCILCHECK_DIRECT  6
#define PASS_TEXT                           7
#define PASS_CLEARTYPE_TEXT                 8
#define PASS_ALIASED_LINES                  9
#define PASS_ALIASED_LINES_DIRECT          10

#define PASS_AA_CREATEMASK                  0
#define PASS_AA_DRAW                        1
#define PASS_AA_DRAW_DIRECT                 2

#define D3D_STAGE_COUNT             2
#define D3D_RENDER_STATES           210
#define D3D_TEXTURE_STATES          33
#define D3D_SAMPLE_STATES           14


typedef HRESULT (APIENTRY *PFND3DXCREATEBUFFER)(DWORD, LPD3DXBUFFER *);
typedef HRESULT (APIENTRY *PFND3DXCREATEEFFECT)(LPDIRECT3DDEVICE9, LPCVOID, UINT, CONST D3DXMACRO *,
                                                LPD3DXINCLUDE, DWORD, LPD3DXEFFECTPOOL,
                                                LPD3DXEFFECT *, LPD3DXBUFFER *);
typedef D3DXMATRIX *(APIENTRY *PFND3DXMATRIXORTHOOFFCENTERLH)(D3DMATRIX *, FLOAT, FLOAT,
                                                              FLOAT, FLOAT, FLOAT, FLOAT);
typedef IDirect3D9 *(APIENTRY *PFNDIRECT3DCREATE9)(uint);

static PFNDIRECT3DCREATE9 pDirect3DCreate9 = 0;
static PFND3DXCREATEBUFFER pD3DXCreateBuffer = 0;
static PFND3DXCREATEEFFECT pD3DXCreateEffect = 0;
static PFND3DXMATRIXORTHOOFFCENTERLH pD3DXMatrixOrthoOffCenterLH = 0;


class QD3DSurfaceManager : public QObject {
    Q_OBJECT

public:
    enum QD3DSurfaceManagerStatus {
        NoStatus = 0,
        NeedsResetting = 0x01,
        MaxSizeChanged = 0x02
    };

    QD3DSurfaceManager();
    ~QD3DSurfaceManager();

    void init(LPDIRECT3D9 object);

    void setPaintDevice(QPaintDevice *pd);

    int status() const;
    void reset();

    LPDIRECT3DSURFACE9 renderTarget();

    LPDIRECT3DSURFACE9 surface(QPaintDevice *pd);
    LPDIRECT3DSWAPCHAIN9 swapChain(QPaintDevice *pd);
    void releasePaintDevice(QPaintDevice *pd);

    LPDIRECT3DDEVICE9 device();
    void cleanup();

    QSize maxSize() const;

private:
    struct D3DSwapChain {
        QSize size;
        LPDIRECT3DSWAPCHAIN9 swapchain;
        LPDIRECT3DSURFACE9 surface;
    };

    void updateMaxSize();
    void initPresentParameters(D3DPRESENT_PARAMETERS *params);
    D3DSwapChain *createSwapChain(QWidget *w);

    QSize m_max_size;
    int m_status;
    QMap<QPaintDevice *, D3DSwapChain *> m_swapchains;

    LPDIRECT3DDEVICE9 m_device;
    QPaintDevice *m_pd;
    HWND m_dummy;
    D3DSwapChain *m_current;

private Q_SLOTS:
    void cleanupPaintDevice(QObject *);
};

struct vertex {
    D3DVECTOR pos;
    DWORD color;
    FLOAT s0, t0, r0, q0;
    FLOAT s1, t1, r1, q1;
};

struct QD3DMaskPosition {
    int x, y, channel;
};


struct QD3DBatchItem {
    enum QD3DBatchInfo {
        BI_WINDING          = 0x0001,
        BI_AA               = 0x0002,
        BI_BRECT            = 0x0004,
        BI_MASKFULL         = 0x0008,
        BI_TEXT             = 0x0010,
        BI_MASK             = 0x0020,
        BI_CLIP             = 0x0040,
        BI_SCISSOR          = 0x0080,

        BI_PIXMAP           = 0x0100,
        BI_IMAGE            = 0x0200,
        BI_COMPLEXBRUSH     = 0x0400,

        BI_CLEARCLIP        = 0x0800, // clip nothing (filling the clip mask with 0)
        BI_TRANSFORM        = 0x1000,
        BI_MASKSCISSOR      = 0x2000,
        BI_FASTLINE         = 0x4000,
        BI_COSMETICPEN      = 0x8000
    };

    int m_info;

    int m_count;
    int m_offset;

    QD3DMaskPosition m_maskpos;
    qreal m_xoffset;
    qreal m_yoffset;
    qreal m_opacity;

    QPixmap m_pixmap;
    QRectF m_brect;
    QBrush m_brush;

    IDirect3DTexture9 *m_texture;

    qreal m_width;
    qreal m_distance;

    QTransform m_matrix;
    QPainter::CompositionMode m_cmode;

    QVector<int> m_pointstops;
};

struct QD3DBatch {
    int m_item_index;
    QD3DBatchItem items[QD3D_BATCH_SIZE];
};

class QD3DStateManager;
class QD3DFontCache;
class QD3DDrawHelper;
class QD3DGradientCache;

class QDirect3DPaintEnginePrivate : public QPaintEnginePrivate
{
    Q_DECLARE_PUBLIC(QDirect3DPaintEngine)

public:
    enum RenderTechnique {
        RT_NoTechnique,
        RT_Antialiased,
        RT_Aliased,
    };

    QDirect3DPaintEnginePrivate()
        : m_d3d_object(0)
        , m_d3d_device(0)
        , m_txop(QTransform::TxNone)
        , m_effect(0)
        , m_flush_on_end(0)
    { init(); }

    ~QDirect3DPaintEnginePrivate();

    bool init();
    void initDevice();

    inline QD3DBatchItem *nextBatchItem();

    QPolygonF brushCoordinates(const QRectF &r, bool stroke, qreal *fp) const;
    void fillAliasedPath(QPainterPath path, const QRectF &brect, const QTransform &txform);
    void fillAntialiasedPath(const QPainterPath &path, const QRectF &brect,
        const QTransform &txform, bool stroke);
    void fillPath(const QPainterPath &path, QRectF brect);

    void strokePath(const QPainterPath &path, QRectF brect, bool simple = false);
    QPainterPath strokePathFastPen(const QPainterPath &path);
    void strokeAliasedPath(QPainterPath path, const QRectF &brect, const QTransform &txform);

    void flushBatch();
    int flushAntialiased(int offset);
    void flushAliased(QD3DBatchItem *item, int offset);
    void flushText(QD3DBatchItem *item, int offset);
    void flushLines(QD3DBatchItem *item, int offset);

    void updateTransform(const QTransform &matrix);
    void updatePen(const QPen &pen);
    void updateBrush(const QBrush &pen);
    void updateClipRegion(const QRegion &clipregion, Qt::ClipOperation op = Qt::ReplaceClip);
    void updateClipPath(const QPainterPath &clipregion, Qt::ClipOperation op = Qt::ReplaceClip);
    void updateFont(const QFont &font);

    void setRenderTechnique(RenderTechnique technique);

    QPointF transformPoint(const QPointF &p, qreal *w) const;

    bool prepareBatch(QD3DBatchItem *item, int offset);
    void prepareItem(QD3DBatchItem *item);
    void cleanupItem(QD3DBatchItem *item);
    void setCompositionMode(QPainter::CompositionMode mode);

    void verifyTexture(const QPixmap &pixmap);

    bool isFastRect(const QRectF &rect);

    void releaseDC();

    void cleanup();
    bool testCaps();

    QPixmap getPattern(Qt::BrushStyle style) const;

    // clipping
    QPainterPath m_sysclip_path;
    QPainterPath m_clip_path;
    QRegion m_sysclip_region;
    QRegion m_clip_region;

    qreal m_opacity;
    D3DCOLOR m_opacity_color;

    int m_current_state;

    ID3DXEffect* m_effect;

    RenderTechnique m_current_technique;

    QTransform m_matrix;
    qreal m_inv_scale;

    QPen m_pen;
    Qt::BrushStyle m_pen_brush_style;
    QTransform m_inv_pen_matrix;
    D3DCOLOR m_pen_color;
    qreal m_pen_width;

    QBrush m_brush;
    Qt::BrushStyle m_brush_style;
    QTransform m_inv_brush_matrix;
    D3DCOLOR m_brush_color;
    QTransform m_brush_origin;

    uint m_clipping_enabled : 1;
    uint m_has_complex_clipping : 1;
    uint m_cleartype_text: 1;
    uint m_has_pen : 1;
    uint m_has_cosmetic_pen : 1;
    uint m_has_brush : 1;
    uint m_has_fast_pen : 1;
    uint m_has_aa_fast_pen : 1;
    uint m_flush_on_end : 1;
    uint m_supports_d3d : 1;

    QTransform::TransformationType m_txop;

    QPainter::CompositionMode m_cmode;

    QD3DSurfaceManager m_surface_manager;
    QSize m_surface_size;

    LPDIRECT3D9 m_d3d_object;
    LPDIRECT3DDEVICE9 m_d3d_device;
    IDirect3DSurface9 *m_current_surface;
    bool m_in_scene;

    QD3DGradientCache *m_gradient_cache;
    QD3DDrawHelper *m_draw_helper;
    QD3DBatch m_batch;
    QD3DStateManager *m_statemanager;

    HDC m_dc;
    IDirect3DSurface9 *m_dcsurface;

    QMap<Qt::BrushStyle, QPixmap> m_patterns;
};


class QD3DStateManager : public ID3DXEffectStateManager {
public:
    QD3DStateManager(LPDIRECT3DDEVICE9 pDevice, ID3DXEffect *effect);
    void reset();

    inline void startStateBlock();
    inline void endStateBlock();

    inline void setCosmeticPen(bool enabled);
    inline void setBrushMode(int mode);
    inline void setTexture(LPDIRECT3DBASETEXTURE9 pTexture);
    inline void setTexture(LPDIRECT3DBASETEXTURE9 pTexture, QGradient::Spread spread);
    inline void setTransformation(const QTransform *matrix = 0);
    inline void setProjection(const D3DXMATRIX *pMatrix);
    inline void setMaskChannel(int channel);
    inline void setMaskOffset(qreal x, qreal y);
    inline void setFocalDistance(const qreal &fd);

    inline void beginPass(int pass);
    inline void endPass();

    STDMETHOD(QueryInterface)(REFIID iid, LPVOID *ppv);
    STDMETHOD_(ULONG, AddRef)();
    STDMETHOD_(ULONG, Release)();

    STDMETHOD(SetTransform)(D3DTRANSFORMSTATETYPE State, CONST D3DMATRIX *pMatrix);
    STDMETHOD(SetMaterial)(CONST D3DMATERIAL9 *pMaterial);
    STDMETHOD(SetLight)(DWORD Index, CONST D3DLIGHT9 *pLight);
    STDMETHOD(LightEnable)(DWORD Index, BOOL Enable);
    STDMETHOD(SetRenderState)(D3DRENDERSTATETYPE State, DWORD Value);
    STDMETHOD(SetTexture)(DWORD Stage, LPDIRECT3DBASETEXTURE9 pTexture);
    STDMETHOD(SetTextureStageState)(DWORD Stage, D3DTEXTURESTAGESTATETYPE Type, DWORD Value);
    STDMETHOD(SetSamplerState)(DWORD Sampler, D3DSAMPLERSTATETYPE Type, DWORD Value);
    STDMETHOD(SetNPatchMode)(FLOAT NumSegments);
    STDMETHOD(SetFVF)(DWORD FVF);
    STDMETHOD(SetVertexShader)(LPDIRECT3DVERTEXSHADER9 pShader);
    STDMETHOD(SetVertexShaderConstantF)(UINT RegisterIndex, CONST FLOAT *pConstantData, UINT RegisterCount);
    STDMETHOD(SetVertexShaderConstantI)(UINT RegisterIndex, CONST INT *pConstantData, UINT RegisterCount);
    STDMETHOD(SetVertexShaderConstantB)(UINT RegisterIndex, CONST BOOL *pConstantData, UINT RegisterCount);
    STDMETHOD(SetPixelShader)(LPDIRECT3DPIXELSHADER9 pShader);
    STDMETHOD(SetPixelShaderConstantF)(UINT RegisterIndex, CONST FLOAT *pConstantData, UINT RegisterCount);
    STDMETHOD(SetPixelShaderConstantI)(UINT RegisterIndex, CONST INT *pConstantData, UINT RegisterCount);
    STDMETHOD(SetPixelShaderConstantB)(UINT RegisterIndex, CONST BOOL *pConstantData, UINT RegisterCount);
private:
    LPDIRECT3DVERTEXSHADER9 m_vertexshader;
    LPDIRECT3DPIXELSHADER9 m_pixelshader;

    LPDIRECT3DBASETEXTURE9 m_textures[D3D_STAGE_COUNT];
    DWORD m_texturestates[D3D_STAGE_COUNT][D3D_TEXTURE_STATES];
    DWORD m_samplerstates[D3D_STAGE_COUNT][D3D_SAMPLE_STATES];
    DWORD m_renderstate[D3D_RENDER_STATES];

    qreal m_radgradfd;

    bool m_cosmetic_pen;
    int m_pass;
    int m_maskchannel;
    int m_brushmode;
    LPDIRECT3DBASETEXTURE9 m_texture;
    D3DXMATRIX m_projection;

    D3DXMATRIX m_d3dIdentityMatrix;
    bool m_isIdentity;
    QTransform m_transformation;

    LPDIRECT3DDEVICE9 m_pDevice;
    ID3DXEffect *m_effect;

    LONG m_refs;
    bool m_changed;
    qreal m_xoffset, m_yoffset;
    static int m_mask_channels[4][4];
};

//
// font cache stuff
//

struct QD3DGlyphCoord {
    // stores the offset and size of a glyph texture
    qreal x;
    qreal y;
    qreal width;
    qreal height;
    qreal log_width;
    qreal log_height;
    QFixed x_offset;
    QFixed y_offset;
};

struct QD3DFontTexture {
    int x_offset; // current glyph offset within the texture
    int y_offset;
    int width;
    int height;
    IDirect3DTexture9 *texture;
};

typedef QHash<glyph_t, QD3DGlyphCoord*>  QD3DGlyphHash;
typedef QHash<QFontEngine*, QD3DGlyphHash*> QD3DFontGlyphHash;
typedef QHash<quint64, QD3DFontTexture*> QD3DFontTexHash;

class QD3DGlyphCache : public QObject
{
    Q_OBJECT
public:
    QD3DGlyphCache()
        : QObject(0)
        , current_cache(0) {}
    ~QD3DGlyphCache();
    QD3DGlyphCoord *lookup(QFontEngine *, glyph_t);
    void cacheGlyphs(QDirect3DPaintEngine *, const QTextItemInt &, const QVarLengthArray<glyph_t> &,
                     bool);
    void cleanCache();
    inline QD3DFontTexture *fontTexture(QFontEngine *engine) {
        return font_textures.constFind(reinterpret_cast<quint64>(engine)).value();
    }

public slots:
    void fontEngineDestroyed(QObject *);

private:
    QImage clearTypeGlyph(QFontEngine *, glyph_t glyph);
    QD3DGlyphHash *current_cache;
    QD3DFontTexHash font_textures;
    QD3DFontGlyphHash font_cache;
};

QD3DGlyphCache::~QD3DGlyphCache()
{
}

QD3DGlyphCoord *QD3DGlyphCache::lookup(QFontEngine *, glyph_t g)
{
    Q_ASSERT(current_cache != 0);
    QD3DGlyphHash::const_iterator it = current_cache->constFind(g);
    if (it == current_cache->constEnd())
        return 0;
    return it.value();
}

void QD3DGlyphCache::cleanCache()
{
    QList<quint64> keys = font_textures.keys();
    for (int i=0; i<keys.size(); ++i)
        font_textures.value(keys.at(i))->texture->Release();

    qDeleteAll(font_textures);
    qDeleteAll(font_cache);
    font_textures.clear();
    font_cache.clear();
    current_cache = 0;
}

void QD3DGlyphCache::fontEngineDestroyed(QObject *object)
{
//     qDebug() << "=> font engine destroyed: " << object;
    QFontEngine *engine = static_cast<QFontEngine *>(object);

    QD3DFontGlyphHash::iterator cache_it = font_cache.find(engine);
    if (cache_it != font_cache.end()) {
        QD3DGlyphHash *cache = font_cache.take(engine);
        delete cache;
    }

    quint64 font_key = reinterpret_cast<quint64>(engine);
    QD3DFontTexture *tex = font_textures.take(font_key);
    if (tex) {
        tex->texture->Release();
        delete tex;
    }
}

QImage QD3DGlyphCache::clearTypeGlyph(QFontEngine *engine, glyph_t glyph)
{
    glyph_metrics_t gm = engine->boundingBox(glyph);
    int glyph_x = qFloor(gm.x.toReal());
    int glyph_y = qFloor(gm.y.toReal());
    int glyph_width = qCeil((gm.x + gm.width).toReal()) -  glyph_x + 2;
    int glyph_height = qCeil((gm.y + gm.height).toReal()) - glyph_y + 2;

    if (glyph_width + glyph_x <= 0 || glyph_height <= 0)
        return QImage();
    QImage im(glyph_width + glyph_x, glyph_height, QImage::Format_ARGB32_Premultiplied);
    im.fill(0xff000000); // solid black
    QPainter p(&im);

    p.setPen(Qt::white);
    p.setBrush(Qt::NoBrush);

    QTextItemInt ti;
    ti.ascent = engine->ascent();
    ti.descent = engine->descent();
    ti.width = glyph_width;
    ti.fontEngine = engine;

    QGlyphLayoutArray<1> glyphLayout;
    ti.glyphs = glyphLayout;
    ti.glyphs.glyphs[0] = glyph;
    ti.glyphs.advances_x[0] = glyph_width;
    p.drawTextItem(QPointF(-glyph_x, -glyph_y), ti);
    p.end();
    return im;
}

#if 0
static void dump_font_texture(QD3DFontTexture *tex)
{
    QColor color(Qt::red);
    D3DLOCKED_RECT rect;
    if (FAILED(tex->texture->LockRect(0, &rect, 0, 0))) {
        qDebug() << "debug: unable to lock texture rect.";
        return;
    }

// cleartype version
//     uint *tex_data = (uint *) rect.pBits;
//     QImage im(tex->width, tex->height, QImage::Format_ARGB32);
//     for (int y=0; y<tex->height; ++y) {
//         for (int x=0; x<tex->width; ++x) {
//             im.setPixel(x, y, ((*(tex_data+x+y*tex->width))));
//         }
//     }
    uchar *tex_data = (uchar *) rect.pBits;
    QImage im(rect.Pitch, tex->height, QImage::Format_ARGB32);
    for (int y=0; y<tex->height; ++y) {
        for (int x=0; x<rect.Pitch; ++x) {
            uchar val = ((*(tex_data+x+y*rect.Pitch)));
            im.setPixel(x, y, 0xff000000 | (val << 16) | (val << 8) | val);
        }
    }
    tex->texture->UnlockRect(0);
    static  int i= 0;
    im.save(QString("tx%1.png").arg(i++));
}
#endif

void QD3DGlyphCache::cacheGlyphs(QDirect3DPaintEngine *engine, const QTextItemInt &ti,
                                 const QVarLengthArray<glyph_t> &glyphs, bool clearType)
{
    IDirect3DDevice9 *device = engine->d_func()->m_d3d_device;
    QD3DFontGlyphHash::const_iterator cache_it = font_cache.constFind(ti.fontEngine);
    QD3DGlyphHash *cache = 0;
    if (cache_it == font_cache.constEnd()) {
        cache = new QD3DGlyphHash;
        font_cache.insert(ti.fontEngine, cache);
        connect(ti.fontEngine, SIGNAL(destroyed(QObject *)), SLOT(fontEngineDestroyed(QObject *)));
    } else {
        cache = cache_it.value();
    }

    current_cache = cache;

    D3DFORMAT tex_format = clearType ? D3DFMT_A8R8G8B8 : D3DFMT_A8;
    quint64 font_key = reinterpret_cast<quint64>(ti.fontEngine);
    QD3DFontTexHash::const_iterator it = font_textures.constFind(font_key);
    QD3DFontTexture *font_tex = 0;
    if (it == font_textures.constEnd()) {
        // alloc a new texture, put it into the cache
        int tex_height = qCeil(ti.ascent.toReal() + ti.descent.toReal()) + 5;
        int tex_width = tex_height * 30; // ###
        IDirect3DTexture9 *tex;
        if (FAILED(device->CreateTexture(tex_width, tex_height, 1, 0,
                                         tex_format, D3DPOOL_MANAGED, &tex, NULL)))
        {
            qWarning("QD3DGlyphCache::cacheGlyphs(): can't allocate font texture (%dx%d).",
                     tex_width, tex_height);
            return;
        } else {
//             qDebug() << "=> new font texture: " << QSize(tex_width,tex_height);
            font_tex = new QD3DFontTexture;
            font_tex->texture = tex;
            font_tex->x_offset = 0;
            font_tex->y_offset = 0;
            font_tex->width = tex_width;
            font_tex->height = tex_height;
            font_textures.insert(font_key, font_tex);
        }
    } else {
        font_tex = it.value();
        // make it current render target..
    }

    // cache each glyph
    for (int i=0; i<glyphs.size(); ++i) {
        QD3DGlyphHash::const_iterator it = cache->constFind(glyphs[i]);
        if (it == cache->constEnd()) {
            glyph_metrics_t metrics = ti.fontEngine->boundingBox(glyphs[i]);
            int glyph_width = qCeil(metrics.width.toReal()) + 5;
            int glyph_height = qCeil(ti.ascent.toReal() + ti.descent.toReal()) + 5;
            if (font_tex->x_offset + glyph_width > font_tex->width) {
                // no room on the current line, start new glyph strip
                int strip_height = glyph_height;
                font_tex->x_offset = 0;
                font_tex->y_offset += strip_height;
                if (font_tex->y_offset >= font_tex->height) {
                    // if no room in the current texture - realloc a larger texture
                    int old_tex_height = font_tex->height;
                    font_tex->height += strip_height;

                    IDirect3DTexture9 *new_tex;
                    if (FAILED(device->CreateTexture(font_tex->width, font_tex->height, 1, 0,
                                                     tex_format, D3DPOOL_MANAGED, &new_tex, NULL)))
                    {
                        qWarning("QD3DGlyphCache(): can't re-allocate font texture.");
                        return;
                    } else {
//                         qDebug() << " -> new glyph strip added:" << QSize(font_tex->width,font_tex->height);

                        D3DLOCKED_RECT new_rect, old_rect;
                        if (FAILED(font_tex->texture->LockRect(0, &old_rect, 0, D3DLOCK_READONLY))) {
                            qDebug() << "QD3DGlyphCache: unable to lock texture rect.";
                            return;
                        }
                        if (FAILED(new_tex->LockRect(0, &new_rect, 0, 0))) {
                            qDebug() << "QD3DGlyphCache: unable to lock texture rect.";
                            return;
                        }
                        memcpy(new_rect.pBits, old_rect.pBits, new_rect.Pitch * old_tex_height);
                        font_tex->texture->UnlockRect(0);
                        new_tex->UnlockRect(0);
                        engine->d_func()->flushBatch();
                        font_tex->texture->Release();
                        font_tex->texture = new_tex;
                    }

                    // update the texture coords and the y offset for the existing glyphs in
                    // the cache, because of the texture size change
                    QD3DGlyphHash::iterator it = cache->begin();
                    while (it != cache->end()) {
                        it.value()->height = (it.value()->height * old_tex_height) / font_tex->height;
                        it.value()->y = (it.value()->y * old_tex_height) / font_tex->height;
                        ++it;
                    }
                }
            }
            QD3DGlyphCoord *d3d_glyph = new QD3DGlyphCoord;
            d3d_glyph->x = qreal(font_tex->x_offset) / font_tex->width;
            d3d_glyph->y = qreal(font_tex->y_offset) / font_tex->height;
            d3d_glyph->width = qreal(glyph_width) / font_tex->width;
            d3d_glyph->height = qreal(glyph_height) / font_tex->height;
            d3d_glyph->log_width = d3d_glyph->width * font_tex->width;
            d3d_glyph->log_height = d3d_glyph->height * font_tex->height;
            d3d_glyph->x_offset = -metrics.x;
            d3d_glyph->y_offset = metrics.y;

            QImage glyph_im;
            if (clearType)
                glyph_im = clearTypeGlyph(ti.fontEngine, glyphs[i]);
            else
                glyph_im = ti.fontEngine->alphaMapForGlyph(glyphs[i]).convertToFormat(QImage::Format_Indexed8);

            // write glyph to texture
            D3DLOCKED_RECT rect;
            RECT glyph_rect = { font_tex->x_offset, font_tex->y_offset,
                                font_tex->x_offset + glyph_im.width(),
                                font_tex->y_offset + glyph_im.height() };

//             qDebug() << "  > new glyph char added:" << QSize(glyph_im.width(), glyph_im.height());
            if (FAILED(font_tex->texture->LockRect(0, &rect, &glyph_rect, 0))) {
                qDebug() << "QD3DGlyphCache: unable to lock texture rect.";
                return;
            }

            // ### unify these loops
            if (clearType) {
                int ppl = rect.Pitch / 4;
                uint *tex_data = (uint *) rect.pBits;
                for (int y=0; y<glyph_im.height(); ++y) {
                    uint *s = (uint *) glyph_im.scanLine(y);
                    for (int x=0; x<glyph_im.width(); ++x) {
                        tex_data[ppl*y + x] = *s;
                        ++s;
                    }
                }
            } else {
                int ppl = rect.Pitch;
                uchar *tex_data = (uchar *) rect.pBits;
                for (int y=0; y<glyph_im.height(); ++y) {
                    uchar *s = (uchar *) glyph_im.scanLine(y);
                    for (int x=0; x<glyph_im.width(); ++x) {
                        tex_data[ppl*y + x] = *s;
                        ++s;
                    }
                }
            }
            font_tex->texture->UnlockRect(0);

            // debug
//             dump_font_texture(font_tex);

            if (font_tex->x_offset + glyph_width > font_tex->width) {
                font_tex->x_offset = 0;
                font_tex->y_offset += glyph_height;
            } else {
                font_tex->x_offset += glyph_width;
            }

            cache->insert(glyphs[i], d3d_glyph);
        }
    }
}

Q_GLOBAL_STATIC(QD3DGlyphCache, qd3d_glyph_cache)

//
// end font caching stuff
//


//
//  D3D image cache stuff
//

// ### keep the GL stuff in mind..
typedef void (*_qt_image_cleanup_hook_64)(qint64);
extern Q_GUI_EXPORT _qt_image_cleanup_hook_64 qt_image_cleanup_hook_64;

static void qd3d_image_cleanup(qint64 key);

class QD3DImage
{
public:
    QD3DImage(IDirect3DDevice9 *device, const QImage &image);
    ~QD3DImage();

    IDirect3DTexture9 *texture;
};

static QList<IDirect3DTexture9 *> qd3d_release_list;

QD3DImage::QD3DImage(IDirect3DDevice9 *device, const QImage &image)
{
    texture = 0;
    Q_ASSERT(device);
    QImage im = image.convertToFormat(QImage::Format_ARGB32);
    if (FAILED(device->CreateTexture(im.width(), im.height(), 1, 0,
                                     D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, &texture, 0))) {
        qWarning("QD3DImage(): unable to create Direct3D texture.");
        return;
    }
//     qDebug(" -> created image texture: %p - 0x%08x%08x",texture,uint (image.cacheKey() >> 32),uint (image.cacheKey() & 0xffffffff));
    D3DLOCKED_RECT rect;
    if (FAILED(texture->LockRect(0, &rect, 0, 0))) {
        qDebug() << "QD3DImage: unable to lock texture rect.";
        return;
    }
    DWORD *dst = (DWORD *) rect.pBits;
    DWORD *src = (DWORD *) im.scanLine(0);

    Q_ASSERT((rect.Pitch/4) == (im.bytesPerLine()/4));
    memcpy(dst, src, rect.Pitch*im.height());
    texture->UnlockRect(0);
}

QD3DImage::~QD3DImage()
{
    if (texture)
        qd3d_release_list.append(texture);
}

static int qd3d_cache_limit = 64*1024; // cache ~64 MB worth of textures
typedef QCache<quint64, QD3DImage> QD3DImageCache;

class QD3DImageManager
{
public:
    QD3DImageManager() {
        // ### GL does the same!
        qt_image_cleanup_hook_64 = qd3d_image_cleanup;
        cache.setMaxCost(qd3d_cache_limit);
    }
    ~QD3DImageManager() {
//         qDebug() << "unhooking d3d image cache";
        qt_image_cleanup_hook_64 = 0;
        cache.clear();
    }

    IDirect3DTexture9 *lookup(IDirect3DDevice9 *device, const QImage &image);
    void remove(quint64 key);

private:
    QD3DImageCache cache;
};

IDirect3DTexture9 *QD3DImageManager::lookup(IDirect3DDevice9 *device, const QImage &image)
{
    QD3DImage *tex_image = 0;

    tex_image = cache.object(image.cacheKey());
    if (!tex_image) {
        // to avoid cache thrashing we remove images from the cache
        // that have the same serial no as the cached image, since
        // that image is most likely destoyed already, and we got a
        // stale cache entry
        uint serial = (uint) (image.cacheKey() >> 32);
        QList<quint64> keys = cache.keys();
        for (int i=0; i<keys.size(); ++i) {
            if ((uint)(keys.at(i) >> 32) == serial) {
                cache.remove(keys.at(i));
                break;
            }
        }
//         qDebug(" => cached: %d, adding cache image: 0x%08x%08x",cache.size(), uint (image.cacheKey() >> 32),uint (image.cacheKey() & 0xffffffff));
        // add cache entry
        int cost = image.width()*image.height()*4/1024;
        if (cache.totalCost() + cost > cache.maxCost()) {
            // no room for new entries? kick out half the cached images
            int old_max_cost = cache.maxCost();
            cache.setMaxCost(old_max_cost/2);
            cache.setMaxCost(old_max_cost);
        }
        tex_image = new QD3DImage(device, image);
        cache.insert(image.cacheKey(), tex_image, cost);
//         qDebug() << "==> total cache cost: " << cache.totalCost() << cost;
    }

    return tex_image->texture;
}

void QD3DImageManager::remove(quint64 key)
{
//     QList<quint64> keys = cache.keys();
//     if (keys.contains(key))
//         qDebug() << "entery removed from cache";
    cache.remove(key);
}

Q_GLOBAL_STATIC(QD3DImageManager, qd3d_image_cache)

static void qd3d_image_cleanup(qint64 key)
{
//     qDebug() << "qd3d_image_cleanup:";
//     qDebug("  => key: 0x%08x%08x", (uint) (key >> 32), (uint)(key & 0xffffffff));
    qd3d_image_cache()->remove(key);
}

//
// end D3D image cache stuff
//

class QD3DDrawHelper : public QTessellator
{
public:
    QD3DDrawHelper(QDirect3DPaintEnginePrivate *pe);
    ~QD3DDrawHelper();

    bool needsFlushing() const;
    QD3DMaskPosition allocateMaskPosition(const QRectF &brect, bool *breakbatch);

    void setClipPath(const QPainterPath &path, QD3DBatchItem **item);

    void queueAntialiasedMask(const QPolygonF &poly, QD3DBatchItem **item, const QRectF &brect);
    QRectF queueAliasedMask(const QPainterPath &path, QD3DBatchItem **item, D3DCOLOR color);

    void queueRect(const QRectF &rect, QD3DBatchItem *item, D3DCOLOR color, const QPolygonF &trect);
    void queueRect(const QRectF &rect, QD3DBatchItem *item, D3DCOLOR color);

    void queueTextGlyph(const QRectF &rect, const qreal *tex_coords, QD3DBatchItem *item,
                        D3DCOLOR color);

    void queueAntialiasedLines(const QPainterPath &path, QD3DBatchItem **item, const QRectF &brect);
    void queueAliasedLines(const QLineF *lines, int lineCount, QD3DBatchItem **item);

    int drawAntialiasedMask(int offset, int maxoffset);
    void drawAliasedMask(int offset);
    void drawAntialiasedBoundingRect(QD3DBatchItem *item);
    void drawAliasedBoundingRect(QD3DBatchItem *item);
    void drawTextItem(QD3DBatchItem *item);
    void drawAliasedLines(QD3DBatchItem *item);

    void setMaskSize(QSize size);

    void beforeReset();
    void afterReset();

    IDirect3DSurface9 *freeMaskSurface();

    inline void lockVertexBuffer();
    inline void unlockVertexBuffer();

    inline int index() { return m_index; }

#ifdef QT_DEBUG_VERTEXBUFFER_ACCESS
    enum VertexBufferAccess {
        CLEAR   = 0x00,
        READ    = 0x01,
        WRITE   = 0x02
    };
    int accesscontrol[QT_VERTEX_BUF_SIZE];
#endif

private:
    void addTrap(const Trapezoid &trap);
    void tessellate(const QPolygonF &poly);
    inline void lineToStencil(qreal x, qreal y);
    inline void curveToStencil(const QPointF &cp1, const QPointF &cp2, const QPointF &ep);
    QRectF pathToVertexArrays(const QPainterPath &path);
    void resetMask();

    QDirect3DPaintEnginePrivate *m_pe;

    qreal m_xoffset, m_yoffset;
    int m_startindex;
    int m_index;
    int m_height, m_width;
    LPDIRECT3DVERTEXBUFFER9 m_d3dvbuff;
    vertex *m_vbuff;
    QD3DBatchItem *m_item;
    QRectF m_boundingRect;

    qreal max_x;
    qreal max_y;
    qreal min_x;
    qreal min_y;
    qreal firstx;
    qreal firsty;

    QPointF tess_lastpoint;
    int tess_index;

    bool m_locked;
    IDirect3DTexture9 *m_mask;
    IDirect3DSurface9 *m_maskSurface;
    IDirect3DSurface9 *m_depthStencilSurface;

    D3DCOLOR m_color;
    bool m_clearmask;
    bool m_isLine;
    bool m_firstPoint;

    QD3DMaskPosition m_mask_position;
    int m_mask_offsetX2;
    int m_mask_offsetY2;
};

QD3DStateManager::QD3DStateManager(LPDIRECT3DDEVICE9 pDevice, ID3DXEffect *effect)
    : m_pDevice(pDevice), m_effect(effect), m_refs(0)
{
    if (FAILED(D3DXMatrixIdentity(&m_d3dIdentityMatrix))) {
        qWarning("QDirect3DPaintEngine: D3DXMatrixIdentity failed");
    }
    reset();
}

void QD3DStateManager::reset()
{
    m_radgradfd = -1;

    m_cosmetic_pen = false;
    m_pass = -1;
    m_maskchannel = -1;
    m_brushmode = -1;
    m_texture = 0;
    m_xoffset = INT_MAX;
    m_yoffset = INT_MAX;

    m_vertexshader = 0;
    m_pixelshader = 0;

    m_isIdentity = true;
    m_transformation = QTransform();
    m_effect->SetMatrix("g_mTransformation", &m_d3dIdentityMatrix);

    ZeroMemory(&m_projection, sizeof(D3DMATRIX));
    ZeroMemory(m_textures, sizeof(LPDIRECT3DBASETEXTURE9) * D3D_STAGE_COUNT);
    FillMemory(m_samplerstates, sizeof(DWORD) * D3D_SAMPLE_STATES * D3D_STAGE_COUNT, 0xFFFFFFFE);
    FillMemory(m_texturestates, sizeof(DWORD) * D3D_TEXTURE_STATES * D3D_STAGE_COUNT, 0xFFFFFFFE);
    FillMemory(m_renderstate, sizeof(DWORD) * D3D_RENDER_STATES, 0xFFFFFFFE);
}

inline void QD3DStateManager::beginPass(int pass)
{
    if (pass != m_pass) {
        if (m_pass != -1)
            m_effect->EndPass();
        m_effect->BeginPass(pass);
        m_pass = pass;
    }
}

inline void QD3DStateManager::endPass()
{
    if (m_pass != -1) {
        m_pass = -1;
        m_effect->EndPass();
    }
}

inline void QD3DStateManager::startStateBlock() {
    m_changed = false;
}

inline void QD3DStateManager::setCosmeticPen(bool enabled)
{
    if (enabled != m_cosmetic_pen) {
        m_effect->SetBool("g_mCosmeticPen", enabled);
        m_cosmetic_pen = enabled;
        m_changed = true;
    }
}

inline void QD3DStateManager::setBrushMode(int mode)
{
    if (mode != m_brushmode) {
        m_effect->SetInt("g_mBrushMode", mode);
        m_brushmode = mode;
        m_changed = true;
    }
}

inline void QD3DStateManager::setTexture(LPDIRECT3DBASETEXTURE9 pTexture)
{
    SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_BORDER);
    SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_BORDER);

    if (pTexture != m_texture) {
        m_texture = pTexture;
        m_effect->SetTexture("g_mTexture", pTexture);
        m_changed = true;
    }
}

inline void QD3DStateManager::setTexture(LPDIRECT3DBASETEXTURE9 pTexture, QGradient::Spread spread)
{
    switch(spread) {
        case QGradient::RepeatSpread:
            SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
            SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);
            break;
        case QGradient::ReflectSpread:
            SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_MIRROR);
            SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_MIRROR);
            break;
        default:
            SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
            SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);
            break;
    };

    if (pTexture != m_texture) {
        m_texture = pTexture;
        m_effect->SetTexture("g_mTexture", pTexture);
        m_changed = true;
    }
}

inline void QD3DStateManager::setTransformation(const QTransform *matrix)
{
    if (matrix) {
        if (*matrix != m_transformation) {
            D3DXMATRIX dxmatrix(matrix->m11(), matrix->m12(), 0, matrix->m13(),
                    matrix->m21(), matrix->m22(), 0, matrix->m23(),
                    0, 0, 1, 0,
                    matrix->dx(), matrix->dy(), 0, 1);
            m_effect->SetMatrix("g_mTransformation", &dxmatrix);
            m_transformation = *matrix;
            m_changed = true;
            m_isIdentity = false;
        }
    } else if (!m_isIdentity) {
        m_effect->SetMatrix("g_mTransformation", &m_d3dIdentityMatrix);
        m_transformation = QTransform();
        m_changed = true;
        m_isIdentity = true;
    }
}

inline void QD3DStateManager::setProjection(const D3DXMATRIX *pMatrix)
{
    if (*pMatrix != m_projection) {
        m_effect->SetMatrix("g_mViewProjection", pMatrix);
        m_projection = *pMatrix;
        m_changed = true;
    }
}

inline void QD3DStateManager::setFocalDistance(const qreal &fd)
{
    if (fd != m_radgradfd) {
        m_effect->SetFloat("g_mFocalDist", fd);
        m_changed = true;
        m_radgradfd = fd;
    }
}

inline void QD3DStateManager::setMaskOffset(qreal x, qreal y)
{
    if (x != m_xoffset || y != m_yoffset) {
        float offset[2] = {x, y};
        m_effect->SetFloatArray("g_mMaskOffset", offset, 2);
        m_xoffset = x;
        m_yoffset = y;
        m_changed = true;
    }
}

inline void QD3DStateManager::setMaskChannel(int channel)
{
    if (m_maskchannel != channel) {
        m_effect->SetIntArray("g_mChannel", m_mask_channels[channel], 4);
        m_maskchannel = channel;
        m_changed = true;
    }
}

inline void QD3DStateManager::endStateBlock()
{
    if (m_changed) {
        m_effect->CommitChanges();
        m_changed = false;
    }
}

STDMETHODIMP QD3DStateManager::QueryInterface(REFIID iid, LPVOID *ppv)
{
    if(iid == IID_IUnknown || iid == IID_ID3DXEffectStateManager)
    {
        *ppv = this;
        ++m_refs;
        return NOERROR;
    }
    *ppv = NULL;
    return ResultFromScode(E_NOINTERFACE);
}

STDMETHODIMP_(ULONG) QD3DStateManager::AddRef(void)
{
    return (ULONG)InterlockedIncrement( &m_refs );
}


STDMETHODIMP_(ULONG) QD3DStateManager::Release(void)
{
    if( 0L == InterlockedDecrement( &m_refs ) ) {
        delete this;
        return 0L;
    }

    return m_refs;
}
STDMETHODIMP QD3DStateManager::SetTransform(D3DTRANSFORMSTATETYPE State, CONST D3DMATRIX *pMatrix)
{
    return m_pDevice->SetTransform(State, pMatrix);
}

STDMETHODIMP QD3DStateManager::SetMaterial(CONST D3DMATERIAL9 *pMaterial)
{
    return m_pDevice->SetMaterial(pMaterial);
}

STDMETHODIMP QD3DStateManager::SetLight(DWORD Index, CONST D3DLIGHT9 *pLight)
{
    return m_pDevice->SetLight(Index, pLight);
}

STDMETHODIMP QD3DStateManager::LightEnable(DWORD Index, BOOL Enable)
{
    return m_pDevice->LightEnable(Index, Enable);
}

STDMETHODIMP QD3DStateManager::SetRenderState(D3DRENDERSTATETYPE State, DWORD Value)
{
    if (State < D3D_RENDER_STATES) {
        if (m_renderstate[State] == Value)
            return S_OK;
        m_renderstate[State] = Value;
    }
    return m_pDevice->SetRenderState(State, Value);
}

STDMETHODIMP QD3DStateManager::SetTexture(DWORD Stage, LPDIRECT3DBASETEXTURE9 pTexture)
{
    if (Stage < D3D_STAGE_COUNT) {
        if (m_textures[Stage] == pTexture)
            return S_OK;
        m_textures[Stage] = pTexture;
    }
    return m_pDevice->SetTexture(Stage, pTexture);
}

STDMETHODIMP QD3DStateManager::SetTextureStageState(DWORD Stage, D3DTEXTURESTAGESTATETYPE Type, DWORD Value)
{
    if (Stage < D3D_STAGE_COUNT && Type < D3D_TEXTURE_STATES) {
        if (m_texturestates[Stage][Type] == Value)
            return S_OK;
        m_texturestates[Stage][Type] = Value;
    }
    return m_pDevice->SetTextureStageState(Stage, Type, Value);
}

STDMETHODIMP QD3DStateManager::SetSamplerState(DWORD Sampler, D3DSAMPLERSTATETYPE Type, DWORD Value)
{
    if (Sampler < D3D_STAGE_COUNT && Type < D3D_SAMPLE_STATES) {
        if (m_samplerstates[Sampler][Type] == Value)
            return S_OK;
        m_samplerstates[Sampler][Type] = Value;
    }
    return m_pDevice->SetSamplerState(Sampler, Type, Value);
}

STDMETHODIMP QD3DStateManager::SetNPatchMode(FLOAT NumSegments)
{
    return m_pDevice->SetNPatchMode(NumSegments);
}

STDMETHODIMP QD3DStateManager::SetFVF(DWORD FVF)
{
    return m_pDevice->SetFVF(FVF);
}

STDMETHODIMP QD3DStateManager::SetVertexShader(LPDIRECT3DVERTEXSHADER9 pShader)
{
    if (m_vertexshader == pShader)
        return S_OK;
    m_vertexshader = pShader;
    return m_pDevice->SetVertexShader(pShader);
}

STDMETHODIMP QD3DStateManager::SetVertexShaderConstantF(UINT RegisterIndex, CONST FLOAT *pConstantData, UINT RegisterCount)
{
    return m_pDevice->SetVertexShaderConstantF(RegisterIndex, pConstantData, RegisterCount);
}

STDMETHODIMP QD3DStateManager::SetVertexShaderConstantI(UINT RegisterIndex, CONST INT *pConstantData, UINT RegisterCount)
{
    return m_pDevice->SetVertexShaderConstantI(RegisterIndex, pConstantData, RegisterCount);
}

STDMETHODIMP QD3DStateManager::SetVertexShaderConstantB(UINT RegisterIndex, CONST BOOL *pConstantData, UINT RegisterCount)
{
    return m_pDevice->SetVertexShaderConstantB(RegisterIndex, pConstantData, RegisterCount);
}

STDMETHODIMP QD3DStateManager::SetPixelShader(LPDIRECT3DPIXELSHADER9 pShader)
{
    if (m_pixelshader == pShader)
        return S_OK;
    m_pixelshader = pShader;
    return m_pDevice->SetPixelShader(pShader);
}

STDMETHODIMP QD3DStateManager::SetPixelShaderConstantF(UINT RegisterIndex, CONST FLOAT *pConstantData, UINT RegisterCount)
{
    return m_pDevice->SetPixelShaderConstantF(RegisterIndex, pConstantData, RegisterCount);
}

STDMETHODIMP QD3DStateManager::SetPixelShaderConstantI(UINT RegisterIndex, CONST INT *pConstantData, UINT RegisterCount)
{
    return m_pDevice->SetPixelShaderConstantI(RegisterIndex, pConstantData, RegisterCount);
}

STDMETHODIMP QD3DStateManager::SetPixelShaderConstantB(UINT RegisterIndex, CONST BOOL *pConstantData, UINT RegisterCount)
{
    return m_pDevice->SetPixelShaderConstantB(RegisterIndex, pConstantData, RegisterCount);
}

#define QD3D_GRADIENT_CACHE_SIZE    60
#define QD3D_GRADIENT_PALETTE_SIZE  1024

class QD3DGradientCache
{
    struct CacheInfo
    {
        inline CacheInfo(QGradientStops s, qreal op) :
            stops(s), opacity(op) {}

        IDirect3DTexture9 *texture;
        QGradientStops stops;
        qreal opacity;
    };

    typedef QMultiHash<quint64, CacheInfo> QD3DGradientColorTableHash;

public:
    QD3DGradientCache(LPDIRECT3DDEVICE9 device);
    ~QD3DGradientCache();

    inline IDirect3DTexture9 *getBuffer(const QGradientStops &stops, qreal opacity);

protected:
    inline void generateGradientColorTable(const QGradientStops& s,
                                           uint *colorTable,
                                           int size, qreal opacity) const;
    IDirect3DTexture9 *addCacheElement(quint64 hash_val, const QGradientStops &stops, qreal opacity);
    void cleanCache();

    QD3DGradientColorTableHash cache;
    LPDIRECT3DDEVICE9 m_device;
};

QD3DGradientCache::QD3DGradientCache(LPDIRECT3DDEVICE9 device)
    : m_device(device)
{

}

QD3DGradientCache::~QD3DGradientCache()
{
    cleanCache();
}

inline IDirect3DTexture9 *QD3DGradientCache::getBuffer(const QGradientStops &stops, qreal opacity)
{
    quint64 hash_val = 0;

    for (int i = 0; i < stops.size() && i <= 2; i++)
        hash_val += stops[i].second.rgba();

    QD3DGradientColorTableHash::const_iterator it = cache.constFind(hash_val);

    if (it == cache.constEnd())
        return addCacheElement(hash_val, stops, opacity);
    else {
        do {
            const CacheInfo &cache_info = it.value();
            if (cache_info.stops == stops && cache_info.opacity == opacity) {
                return cache_info.texture;
            }
            ++it;
        } while (it != cache.constEnd() && it.key() == hash_val);
        // an exact match for these stops and opacity was not found, create new cache
        return addCacheElement(hash_val, stops, opacity);
    }
}

void QD3DGradientCache::generateGradientColorTable(const QGradientStops& s, uint *colorTable, int size, qreal opacity) const
{
    int pos = 0;
    qreal fpos = 0.0;
    qreal incr = 1.0 / qreal(size);
    QVector<uint> colors(s.size());

    for (int i = 0; i < s.size(); ++i)
        colors[i] = s[i].second.rgba();

    uint alpha = qRound(opacity * 255);
    while (fpos < s.first().first) {
        colorTable[pos] = ARGB_COMBINE_ALPHA(colors[0], alpha);
        pos++;
        fpos += incr;
    }

    for (int i = 0; i < s.size() - 1; ++i) {
        qreal delta = 1/(s[i+1].first - s[i].first);
        while (fpos < s[i+1].first && pos < size) {
            int dist = int(256 * ((fpos - s[i].first) * delta));
            int idist = 256 - dist;
            uint current_color = ARGB_COMBINE_ALPHA(colors[i], alpha);
            uint next_color = ARGB_COMBINE_ALPHA(colors[i+1], alpha);
#if Q_BYTE_ORDER == Q_LITTLE_ENDIAN
            colorTable[pos] = INTERPOLATE_PIXEL_256(current_color, idist, next_color, dist);
#else
            uint c = INTERPOLATE_PIXEL_256(current_color, idist, next_color, dist);
            colorTable[pos] = ( (c << 24) & 0xff000000)
                              | ((c >> 24) & 0x000000ff)
                              | ((c << 8) & 0x00ff0000)
                              | ((c >> 8) & 0x0000ff00);
#endif // Q_BYTE_ORDER
            ++pos;
            fpos += incr;
        }
    }
    for (;pos < size; ++pos)
        colorTable[pos] = colors[s.size() - 1];
}

IDirect3DTexture9 *QD3DGradientCache::addCacheElement(quint64 hash_val, const QGradientStops &stops, qreal opacity)
{
    if (cache.size() == QD3D_GRADIENT_CACHE_SIZE) {
        int elem_to_remove = qrand() % QD3D_GRADIENT_CACHE_SIZE;
        uint key = cache.keys()[elem_to_remove];

        // need to call release on each removed cache entry:
        QD3DGradientColorTableHash::const_iterator it = cache.constFind(key);
        do {
            it.value().texture->Release();
        } while (++it != cache.constEnd() && it.key() == key);

        cache.remove(key); // may remove more than 1, but OK
    }

    CacheInfo cache_entry(stops, opacity);
    uint buffer[QD3D_GRADIENT_PALETTE_SIZE];
    generateGradientColorTable(stops, buffer, QD3D_GRADIENT_PALETTE_SIZE, opacity);

    if (FAILED(m_device->CreateTexture(QD3D_GRADIENT_PALETTE_SIZE, 1, 1, 0,
                    D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, &cache_entry.texture, 0))) {
        qWarning("QD3DGradientCache::addCacheElement(): unable to create Direct3D texture.");
        return 0;
    }

    D3DLOCKED_RECT rect;
    if (FAILED(cache_entry.texture->LockRect(0, &rect, 0, 0))) {
        qDebug() << "QD3DGradientCache::addCacheElement(): unable to lock texture rect.";
        return 0;
    }
    memcpy(rect.pBits, buffer, rect.Pitch);
    cache_entry.texture->UnlockRect(0);

    return cache.insert(hash_val, cache_entry).value().texture;
}

void QD3DGradientCache::cleanCache()
{
    QD3DGradientColorTableHash::const_iterator it = cache.constBegin();
    for (; it != cache.constEnd(); ++it) {
        const CacheInfo &cache_info = it.value();
        cache_info.texture->Release();
    }
    cache.clear();
}

QD3DSurfaceManager::QD3DSurfaceManager() :
    m_status(NoStatus), m_dummy(0), m_device(0), m_pd(0), m_current(0)
{

}

QD3DSurfaceManager::~QD3DSurfaceManager()
{
}

void QD3DSurfaceManager::setPaintDevice(QPaintDevice *pd)
{
    m_status = NoStatus;
    m_pd = pd;
    m_current = 0;

    if (m_device->TestCooperativeLevel() != D3D_OK) {
        m_status = NeedsResetting;
        return;
    }

    m_current = m_swapchains.value(pd, 0);
    QWidget *w = static_cast<QWidget*>(pd);

    if (m_current) {
        if (m_current->size != w->size()) {
            m_swapchains.remove(pd);
            m_current->surface->Release();
            m_current->swapchain->Release();
            delete m_current;
            m_current = 0;
        }
    }

    if (!m_current) {
        m_current = createSwapChain(w);
        updateMaxSize();
    }
}

int QD3DSurfaceManager::status() const
{
    return m_status;
}

void QD3DSurfaceManager::reset()
{
    QList<QPaintDevice *> pds = m_swapchains.keys();

    QMap<QPaintDevice *, D3DSwapChain *>::const_iterator i = m_swapchains.constBegin();
    while (i != m_swapchains.constEnd()) {
        i.value()->surface->Release();
        i.value()->swapchain->Release();
        ++i;
    }
    qDeleteAll(m_swapchains.values());
    m_swapchains.clear();

    D3DPRESENT_PARAMETERS params;
    initPresentParameters(&params);
    params.hDeviceWindow = m_dummy;

    HRESULT res = m_device->Reset(&params);
    if (FAILED(res)) {
        switch (res) {
            case D3DERR_DEVICELOST:
                qWarning("QDirect3DPaintEngine: Reset failed (D3DERR_DEVICELOST)");
                break;
            case D3DERR_DRIVERINTERNALERROR:
                qWarning("QDirect3DPaintEngine: Reset failed (D3DERR_DRIVERINTERNALERROR)");
                break;
            case D3DERR_OUTOFVIDEOMEMORY:
                qWarning("QDirect3DPaintEngine: Reset failed (D3DERR_OUTOFVIDEOMEMORY)");
                break;
            default:
                qWarning("QDirect3DPaintEngine: Reset failed");
        };
    }

    for (int i=0; i<pds.count(); ++i) {
        QWidget *w = static_cast<QWidget*>(pds.at(i));
        createSwapChain(w);
    }

    // reset the mask as well
    m_status = MaxSizeChanged;

    setPaintDevice(m_pd);
    updateMaxSize();
}

LPDIRECT3DSURFACE9 QD3DSurfaceManager::renderTarget()
{
    return m_current ? m_current->surface : 0;
}

LPDIRECT3DSURFACE9 QD3DSurfaceManager::surface(QPaintDevice *pd)
{
    D3DSwapChain *swapchain = m_swapchains.value(pd, 0);
    return swapchain ? swapchain->surface : 0;
}

LPDIRECT3DSWAPCHAIN9 QD3DSurfaceManager::swapChain(QPaintDevice *pd)
{
    D3DSwapChain *swapchain = m_swapchains.value(pd, 0);
    return swapchain ? swapchain->swapchain : 0;
}

void QD3DSurfaceManager::releasePaintDevice(QPaintDevice *pd)
{
    D3DSwapChain *swapchain = m_swapchains.take(pd);

    if (swapchain) {
        swapchain->surface->Release();
        swapchain->swapchain->Release();
        delete swapchain;
        if (swapchain == m_current)
            m_current = 0;
    }
}

LPDIRECT3DDEVICE9 QD3DSurfaceManager::device()
{
    return m_device;
}

void QD3DSurfaceManager::cleanup()
{
    QPixmapCache::clear();
    qd3d_glyph_cache()->cleanCache();

    // release doomed textures
    for (int k=0; k<qd3d_release_list.size(); ++k)
        qd3d_release_list.at(k)->Release();
    qd3d_release_list.clear();

    QMap<QPaintDevice *, D3DSwapChain *>::const_iterator i = m_swapchains.constBegin();
    while (i != m_swapchains.constEnd()) {
        i.value()->surface->Release();
        i.value()->swapchain->Release();
        ++i;
    }
    qDeleteAll(m_swapchains.values());

    if (m_device)
        m_device->Release();

    DestroyWindow(m_dummy);
    QString cname(QLatin1String("qt_d3d_dummy"));
    QT_WA({
        UnregisterClass((TCHAR*)cname.utf16(), (HINSTANCE)qWinAppInst());
    } , {
        UnregisterClassA(cname.toLatin1(), (HINSTANCE)qWinAppInst());
    });
}

QSize QD3DSurfaceManager::maxSize() const
{
    return m_max_size;
}

extern "C" {
    LRESULT CALLBACK QtWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
};

void QD3DSurfaceManager::init(LPDIRECT3D9 object)
{
    QString cname(QLatin1String("qt_d3d_dummy"));
    uint style = CS_DBLCLKS | CS_SAVEBITS;
    ATOM atom;
    QT_WA({
        WNDCLASS wc;
        wc.style         = style;
        wc.lpfnWndProc   = (WNDPROC)QtWndProc;
        wc.cbClsExtra    = 0;
        wc.cbWndExtra    = 0;
        wc.hInstance     = (HINSTANCE)qWinAppInst();
        wc.hIcon         = 0;
        wc.hCursor       = 0;
        wc.hbrBackground = 0;
        wc.lpszMenuName  = 0;
        wc.lpszClassName = (TCHAR*)cname.utf16();
        atom = RegisterClass(&wc);
    } , {
        WNDCLASSA wc;
        wc.style         = style;
        wc.lpfnWndProc   = (WNDPROC)QtWndProc;
        wc.cbClsExtra    = 0;
        wc.cbWndExtra    = 0;
        wc.hInstance     = (HINSTANCE)qWinAppInst();
        wc.hIcon         = 0;
        wc.hCursor       = 0;
        wc.hbrBackground = 0;
        wc.lpszMenuName  = 0;
        QByteArray tempArray = cname.toLatin1();
        wc.lpszClassName = tempArray;
        atom = RegisterClassA(&wc);
    });

    QT_WA({
        const TCHAR *className = (TCHAR*)cname.utf16();
        m_dummy = CreateWindow(className, className, 0,
                               0, 0, 1, 1,
                               0, 0, qWinAppInst(), 0);
    } , {
        m_dummy = CreateWindowA(cname.toLatin1(), cname.toLatin1(), 0,
                                0, 0, 1, 1,
                                0, 0, qWinAppInst(), 0);
    });

    D3DPRESENT_PARAMETERS params;
    initPresentParameters(&params);
    params.hDeviceWindow = m_dummy;

    HRESULT res = object->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, 0,
        D3DCREATE_PUREDEVICE|D3DCREATE_HARDWARE_VERTEXPROCESSING|D3DCREATE_NOWINDOWCHANGES|D3DCREATE_FPU_PRESERVE,
        &params, &m_device);

    if (FAILED(res) || m_device == 0)
        qWarning("QDirect3DPaintEngine: failed to create Direct3D device (error=0x%x).", res);
}

void QD3DSurfaceManager::updateMaxSize()
{
    int w = 0, h = 0;
    QMap<QPaintDevice *, D3DSwapChain *>::const_iterator i = m_swapchains.constBegin();
    while (i != m_swapchains.constEnd()) {

        int nw = i.key()->width();
        if (nw > w)
            w = nw;

        int nh = i.key()->height();
        if (nh > h)
            h = nh;

        ++i;
    }

    QSize newsize = QSize(w, h);
    if (newsize != m_max_size) {
        m_status |= MaxSizeChanged;
        m_max_size = newsize;
    }
}

void QD3DSurfaceManager::initPresentParameters(D3DPRESENT_PARAMETERS *params)
{
    ZeroMemory(params, sizeof(D3DPRESENT_PARAMETERS));
    params->Windowed = true;
    params->SwapEffect = D3DSWAPEFFECT_COPY;
    params->BackBufferFormat = D3DFMT_UNKNOWN;
    params->PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
    params->Flags = D3DPRESENTFLAG_LOCKABLE_BACKBUFFER;
}

QD3DSurfaceManager::D3DSwapChain *QD3DSurfaceManager::createSwapChain(QWidget *w)
{
    D3DPRESENT_PARAMETERS params;
    initPresentParameters(&params);
    params.hDeviceWindow = w->winId();
    D3DSwapChain *swapchain = new D3DSwapChain();
    swapchain->size = w->size();
    if (FAILED(m_device->CreateAdditionalSwapChain(&params, &swapchain->swapchain)))
        qWarning("QDirect3DPaintEngine: CreateAdditionalSwapChain failed");
    if (FAILED(swapchain->swapchain->GetBackBuffer(0, D3DBACKBUFFER_TYPE_MONO, &swapchain->surface)))
        qWarning("QDirect3DPaintEngine: GetBackBuffer failed");
    m_swapchains.insert(w, swapchain);
    connect(w, SIGNAL(destroyed(QObject *)), SLOT(cleanupPaintDevice(QObject *)));

    // init with background color
    QColor bg = w->palette().color(QPalette::Background);
    m_device->ColorFill(swapchain->surface, 0, D3DCOLOR_ARGB(bg.alpha(), bg.red(),bg.green(),bg.blue()));

    return swapchain;
}

void QD3DSurfaceManager::cleanupPaintDevice(QObject *object)
{
    QWidget *w = static_cast<QWidget *>(object);
    releasePaintDevice(w);
}

int QD3DStateManager::m_mask_channels[4][4] =
    {{1,0,0,0},{0,1,0,0},{0,0,1,0},{0,0,0,1}};

QD3DDrawHelper::QD3DDrawHelper(QDirect3DPaintEnginePrivate *pe)
    : m_pe(pe), m_d3dvbuff(0), m_maskSurface(0), m_depthStencilSurface(0),
      m_locked(false), m_mask(0), m_startindex(0), m_index(0), m_vbuff(0), m_clearmask(true),
      m_isLine(false), m_firstPoint(true)
{
    resetMask();
#ifdef QT_DEBUG_VERTEXBUFFER_ACCESS
    memset(accesscontrol, 0, QT_VERTEX_BUF_SIZE * sizeof(VertexBufferAccess));
#endif

    // create vertex buffer
    afterReset();
}

QD3DDrawHelper::~QD3DDrawHelper()
{
    if (m_maskSurface)
        m_maskSurface->Release();

    if (m_mask)
        m_mask->Release();

    if (m_depthStencilSurface)
        m_depthStencilSurface->Release();

    if (m_d3dvbuff)
        m_d3dvbuff->Release();
}

inline void QD3DDrawHelper::lockVertexBuffer()
{
    if (!m_locked) {
        DWORD lockflags = D3DLOCK_NOOVERWRITE;
        if (m_startindex >= QT_VERTEX_RESET_LIMIT) {
            m_startindex = 0;
            m_index = 0;

#ifdef QT_DEBUG_VERTEXBUFFER_ACCESS
            for (int i=0; i<QT_VERTEX_BUF_SIZE; ++i) {
                if (accesscontrol[i] != (WRITE|READ) && accesscontrol[i] != CLEAR)
                    qDebug() << "Vertex Buffer: Access Error";
            }
            memset(accesscontrol, 0, QT_VERTEX_BUF_SIZE * sizeof(VertexBufferAccess));
#endif

            lockflags = D3DLOCK_DISCARD;
        }

        if (FAILED(m_d3dvbuff->Lock(0, 0, (void**)&m_vbuff, lockflags))) {
            qWarning() << "QDirect3DPaintEngine: unable to lock vertex buffer.";
        }
        m_locked = true;
    }
}

inline void QD3DDrawHelper::unlockVertexBuffer()
{
    if (m_locked) {
        if (FAILED(m_d3dvbuff->Unlock())) {
            qWarning() << "QDirect3DPaintEngine: unable to unlock vertex buffer.";
        }
        m_locked = false;
    }
}

void QD3DDrawHelper::setClipPath(const QPainterPath &path, QD3DBatchItem **item)
{
    lockVertexBuffer();

    m_item = *item;
    m_item->m_maskpos.x = m_item->m_maskpos.y = 0;
    m_item->m_maskpos.channel = 3;
    m_item->m_info |= QD3DBatchItem::BI_CLIP;

    bool winding = (path.fillRule() == Qt::WindingFill);
    if (winding)
        m_item->m_info |= QD3DBatchItem::BI_WINDING;

    if (!path.isEmpty()) {
        m_item->m_info |= QD3DBatchItem::BI_MASK;
        m_item->m_info &= ~QD3DBatchItem::BI_AA;
        m_color = 0;
        QRectF brect = pathToVertexArrays(path);
        queueRect(brect, m_item, 0);
    }

    *item = m_item;
}



void QD3DDrawHelper::queueAntialiasedMask(const QPolygonF &poly, QD3DBatchItem **item, const QRectF &brect)
{
    lockVertexBuffer();

    m_item = *item;
    m_item->m_info |= QD3DBatchItem::BI_MASK;
    setWinding(m_item->m_info & QD3DBatchItem::BI_WINDING);

    int xoffset = m_item->m_maskpos.x;
    int yoffset = m_item->m_maskpos.y;

    int x = brect.left();
    int y = brect.top();

    m_item->m_xoffset = (xoffset - x) + 1;
    m_item->m_yoffset = (yoffset - y) + 1;

    m_boundingRect = brect;
    tessellate(poly);

    *item = m_item;
}

QRectF QD3DDrawHelper::queueAliasedMask(const QPainterPath &path, QD3DBatchItem **item, D3DCOLOR color)
{
    lockVertexBuffer();

    m_color = color;
    m_item = *item;
    m_item->m_info |= QD3DBatchItem::BI_MASK;

    bool winding = (path.fillRule() == Qt::WindingFill);
    if (winding)
        m_item->m_info |= QD3DBatchItem::BI_WINDING;

    QRectF result = pathToVertexArrays(path);
    *item = m_item;
    return result;
}

// used for drawing aliased transformed rects directly
// don't use for antialiased or masked drawing
void QD3DDrawHelper::queueRect(const QRectF &rect, QD3DBatchItem *item, D3DCOLOR color, const QPolygonF &trect)
{
    lockVertexBuffer();

    qreal zval = (item->m_info & QD3DBatchItem::BI_CLIP) ? 0.0f : 0.5f;
    item->m_info |= QD3DBatchItem::BI_BRECT;

    // if the item does not have a mask, the offset is different
    if (!(item->m_info & QD3DBatchItem::BI_MASK)) {
        item->m_offset = m_index;
        item->m_count = (item->m_info & QD3DBatchItem::BI_AA) ? 0 : -2;
    }

    qreal x1 = rect.left();
    qreal y1 = rect.top();
    qreal x2 = rect.right();
    qreal y2 = rect.bottom();

    QPointF tc = trect.at(0);
    vertex v1 = { {x1, y1, zval} , color,
          tc.x(), tc.y(), 0.f, 0.f,
          0.f   , 0.f   , 0.f, 0.f };

    tc = trect.at(1);
    vertex v2 = { {x2, y1, zval} , color,
          tc.x(), tc.y(), 0.f, 0.f,
          0.f   , 0.f   , 0.f, 0.f};

    tc = trect.at(2);
    vertex v3 = { {x2, y2, zval} , color,
          tc.x(), tc.y(), 0.f, 0.f,
          0.f   , 0.f   , 0.f, 0.f};;

    tc = trect.at(3);
    vertex v4 = { {x1, y2, zval} , color,
          tc.x(), tc.y(), 0.f, 0.f,
          0.f   , 0.f   , 0.f, 0.f};

#ifdef QT_DEBUG_VERTEXBUFFER_ACCESS
    for (int i=m_index; i<(m_index + 4); ++i) {
        if ((m_index + 4) > QT_VERTEX_BUF_SIZE)
            qDebug() << "Vertex Buffer: Buffer overflow";
        if (accesscontrol[i] != CLEAR)
            qDebug() << "Vertex Buffer: Access Error";
        accesscontrol[i] |= WRITE;
    }
#endif

    m_vbuff[m_index++] = v1;
    m_vbuff[m_index++] = v2;
    m_vbuff[m_index++] = v3;
    m_vbuff[m_index++] = v4;

    m_startindex = m_index;
}


QD3DMaskPosition QD3DDrawHelper::allocateMaskPosition(const QRectF &brect, bool *breakbatch)
{
    int w = brect.width();
    int h = brect.height();

    w += 3;
    h += 3;

    if (w > m_width)
        w = m_width;
    if (h > m_height)
        h = m_height;

    *breakbatch = false;

    if ((m_height - m_mask_offsetY2) >= h && (m_width - m_mask_position.x) >= w) {
        m_mask_position.y = m_mask_offsetY2;
    } else if ((m_width - m_mask_offsetX2) >= w) {
        m_mask_position.y = QD3D_MASK_MARGIN;
        m_mask_position.x = m_mask_offsetX2;
    } else if (m_mask_position.channel < 3) {
        ++m_mask_position.channel;
        m_mask_position.x = m_mask_position.y = QD3D_MASK_MARGIN;
        m_mask_offsetX2 = m_mask_offsetY2 = QD3D_MASK_MARGIN;
    } else {
        resetMask();
        *breakbatch = true;
    }

    int newoffset = m_mask_position.x + w;
    if (m_mask_offsetX2 < newoffset)
        m_mask_offsetX2 = newoffset;
    m_mask_offsetY2 = (m_mask_position.y + h);

    return m_mask_position;

}

void QD3DDrawHelper::queueRect(const QRectF &rect, QD3DBatchItem *item, D3DCOLOR color)
{
    lockVertexBuffer();

    QRectF brect;
    item->m_info |= QD3DBatchItem::BI_BRECT;
    qreal zval = (item->m_info & QD3DBatchItem::BI_CLIP) ? 0.0f : 0.5f;

    if (item->m_info & QD3DBatchItem::BI_AA) {
        int xoffset = item->m_maskpos.x;
        int yoffset = item->m_maskpos.y;

        int x = rect.left();
        int y = rect.top();

        brect = QRectF(x, y, rect.width() + 1, rect.height() + 1);

        item->m_xoffset = (xoffset - x) + 1;
        item->m_yoffset = (yoffset - y) + 1;

        // if the item does not have a mask, the offset is different
        if (!(item->m_info & QD3DBatchItem::BI_MASK)) {
            item->m_offset = m_index;
            item->m_count = 0;
        }
    } else {
        brect = rect;

        if (!(item->m_info & QD3DBatchItem::BI_MASK)) {
            item->m_offset = m_index;
            item->m_count = -2;
        }
    }

    qreal left = brect.left();
    qreal right = brect.right();
    qreal top = brect.top();
    qreal bottom = brect.bottom();

    vertex v1 = { {left, bottom, zval}, color,
        0.f, 0.f, 0.f, 0.f,
        0.f, 0.f, 0.f, 0.f};
    vertex v2 = { {left, top, zval}, color,
        0.f, 0.f, 0.f, 0.f,
        0.f, 0.f, 0.f, 0.f};
    vertex v3 = { {right, top, zval}, color,
        0.f, 0.f, 0.f, 0.f,
        0.f, 0.f, 0.f, 0.f};
    vertex v4 = { {right, bottom, zval}, color,
        0.f, 0.f, 0.f, 0.f,
        0.f, 0.f, 0.f, 0.f};

#ifdef QT_DEBUG_VERTEXBUFFER_ACCESS
    for (int i=m_index; i<(m_index + 4); ++i) {
        if ((m_index + 4) > QT_VERTEX_BUF_SIZE)
            qDebug() << "Vertex Buffer: Buffer overflow";
        if (accesscontrol[i] != CLEAR)
            qDebug() << "Vertex Buffer: Access Error";
        accesscontrol[i] |= WRITE;
    }
#endif

    m_vbuff[m_index++] = v1;
    m_vbuff[m_index++] = v2;
    m_vbuff[m_index++] = v3;
    m_vbuff[m_index++] = v4;

    m_startindex = m_index;
}

void QD3DDrawHelper::queueAntialiasedLines(const QPainterPath &path, QD3DBatchItem **item, const QRectF &brect)
{
    lockVertexBuffer();

    m_item = *item;
    m_item->m_info |= QD3DBatchItem::BI_MASK;
    setWinding(m_item->m_info & QD3DBatchItem::BI_WINDING);

    int xoffset = m_item->m_maskpos.x;
    int yoffset = m_item->m_maskpos.y;
    int x = brect.left();
    int y = brect.top();

    m_item->m_xoffset = (xoffset - x) + 1;
    m_item->m_yoffset = (yoffset - y) + 1;

    m_boundingRect = brect;

    m_xoffset = (x - xoffset) + 0.5f;
    m_yoffset = (y - yoffset) + 0.5f;

    QPointF last;
    for (int i = 0; i < path.elementCount(); ++i) {
        QPainterPath::Element element = path.elementAt(i);

        //Q_ASSERT(!element.isCurveTo());

        if (element.isLineTo())
            QTessellator::tessellateRect(last, element, m_item->m_width);

        last = element;
    }

    m_item->m_offset = m_startindex;
    m_item->m_count = ( m_index - m_startindex ) / 3;
    m_startindex = m_index;

    *item = m_item;
}

void QD3DDrawHelper::queueAliasedLines(const QLineF *lines, int lineCount, QD3DBatchItem **item)
{
    lockVertexBuffer();

    m_item = *item;
    m_item->m_info |= QD3DBatchItem::BI_FASTLINE;

    for (int i=0; i<lineCount; ++i) {
        const QLineF line = lines[i];
        qreal p1x = line.p1().x();
        qreal p1y = line.p1().y();
        qreal p2x = line.p2().x();
        qreal p2y = line.p2().y();

        vertex v1 = { {p1x, p1y, m_pe->m_pen_width} , m_pe->m_pen_color,
                    -1.f, -1.f, p2x, p2y,
                    0.f, 0.f, 0.f, 0.f };
        vertex v2 = { {p1x, p1y, m_pe->m_pen_width} , m_pe->m_pen_color,
                    1.f, -1.f, p2x, p2y,
                    0.f, 0.f, 0.f, 0.f };
        vertex v3 = { {p1x, p1y, m_pe->m_pen_width} , m_pe->m_pen_color,
                    1.f, 1.f, p2x, p2y,
                    0.f, 0.f, 0.f, 0.f };
        vertex v4 = { {p1x, p1y, m_pe->m_pen_width} , m_pe->m_pen_color,
                    -1.f, 1.f, p2x, p2y,
                    0.f, 0.f, 0.f, 0.f };

        m_vbuff[m_index++] = v1;
        m_vbuff[m_index++] = v2;
        m_vbuff[m_index++] = v4;
        m_vbuff[m_index++] = v4;
        m_vbuff[m_index++] = v2;
        m_vbuff[m_index++] = v3;

        if (m_index >= (QT_VERTEX_BUF_SIZE - 16)) {
            m_item->m_offset = m_startindex;
            m_item->m_count = ( m_index - m_startindex ) / 2;
            m_startindex = m_index;

            QD3DBatchItem itemcopy = *m_item;
            m_item = m_pe->nextBatchItem();
            *m_item = itemcopy;

            lockVertexBuffer();
        }
    }

    m_item->m_offset = m_startindex;
    m_item->m_count = ( m_index - m_startindex ) - 2;
    m_startindex = m_index;

    *item = m_item;
}

void QD3DDrawHelper::queueTextGlyph(const QRectF &rect, const qreal *tex_coords,
                                      QD3DBatchItem *item, D3DCOLOR color)
{
    lockVertexBuffer();

    qreal x1 = rect.left();
    qreal y1 = rect.top();
    qreal x2 = rect.right();
    qreal y2 = rect.bottom();

    vertex v1 = { {x1, y1, 0.5f}, color,
                 tex_coords[0], tex_coords[1], 0.f, 0.f,
                 0.f   , 0.f   , 0.f, 0.f};
    vertex v2 = { {x2, y1, 0.5f}, color,
                 tex_coords[2], tex_coords[1], 0.f, 0.f,
                 0.f   , 0.f   , 0.f, 0.f};
    vertex v3 = { {x2, y2, 0.5f}, color,
                 tex_coords[2], tex_coords[3], 0.f, 0.f,
                 0.f   , 0.f   , 0.f, 0.f};
    vertex v4 = { {x1, y1, 0.5f}, color,
                 tex_coords[0], tex_coords[1], 0.f, 0.f,
                 0.f   , 0.f   , 0.f, 0.f};
    vertex v5 = { {x2, y2, 0.5f}, color,
                 tex_coords[2], tex_coords[3], 0.f, 0.f,
                 0.f   , 0.f   , 0.f, 0.f};
    vertex v6 = { {x1, y2, 0.5f}, color,
                 tex_coords[0], tex_coords[3], 0.f, 0.f,
                 0.f   , 0.f   , 0.f, 0.f};

#ifdef QT_DEBUG_VERTEXBUFFER_ACCESS
    for (int i=m_index; i<(m_index + 6); ++i) {
        if ((m_index + 6) > QT_VERTEX_BUF_SIZE)
            qDebug() << "Vertex Buffer: Buffer overflow";
        if (accesscontrol[i] != CLEAR)
            qDebug() << "Vertex Buffer: Access Error";
        accesscontrol[i] |= WRITE;
    }
#endif

    m_vbuff[m_index++] = v1;
    m_vbuff[m_index++] = v2;
    m_vbuff[m_index++] = v3;
    m_vbuff[m_index++] = v4;
    m_vbuff[m_index++] = v5;
    m_vbuff[m_index++] = v6;

    m_startindex = m_index;
    ++item->m_count;
}

bool QD3DDrawHelper::needsFlushing() const
{
    return (m_pe->m_batch.m_item_index >= QD3D_BATCH_SIZE || m_startindex >= QT_VERTEX_RESET_LIMIT);
}

void QD3DDrawHelper::setMaskSize(QSize size)
{
    m_width = size.width();
    m_height = size.height();

    if (m_maskSurface)
        m_maskSurface->Release();

    if (m_mask)
        m_mask->Release();

    if (FAILED(m_pe->m_d3d_device->CreateTexture(m_width, m_height, 1, D3DUSAGE_RENDERTARGET,
        D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &m_mask, NULL))) {
            qWarning() << "QDirect3DPaintEngine: CreateTexture() failed.";
    }

    if (m_depthStencilSurface)
        m_depthStencilSurface->Release();

    if (FAILED(m_pe->m_d3d_device->CreateDepthStencilSurface(m_width, m_height, D3DFMT_D24S8, D3DMULTISAMPLE_NONE, 0,
                                                   TRUE, &m_depthStencilSurface, NULL))) {
            qWarning() << "QDirect3DPaintEngine: CreateDepthStencilSurface() failed.";
    }

    m_pe->m_d3d_device->SetDepthStencilSurface(m_depthStencilSurface);
    m_pe->m_d3d_device->Clear(0, 0, D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL, 0, 0.0f, 0);

    if (FAILED(m_mask->GetSurfaceLevel(0, &m_maskSurface))) {
        qWarning() << "QDirect3DPaintEngine: GetSurfaceLevel() failed.";
    }

    m_pe->m_d3d_device->ColorFill(m_maskSurface, 0, D3DCOLOR_ARGB(0,0,0,0));
    D3DXMATRIX projMatrix;
    pD3DXMatrixOrthoOffCenterLH(&projMatrix, 0, m_width, m_height, 0, 0, 1);
    m_pe->m_effect->SetMatrix("g_mMaskProjection", &projMatrix);
    m_pe->m_effect->SetTexture("g_mAAMask", m_mask);
}

void QD3DDrawHelper::beforeReset()
{
    resetMask();
    m_clearmask = true;

    if (m_maskSurface) {
        m_maskSurface->Release();
        m_maskSurface = 0;
    }

    if (m_mask) {
        m_mask->Release();
        m_mask = 0;
    }

    if (m_depthStencilSurface) {
        m_depthStencilSurface->Release();
        m_depthStencilSurface = 0;
    }

    if (m_d3dvbuff)
        m_d3dvbuff->Release();
}

void QD3DDrawHelper::afterReset()
{
    if (FAILED(m_pe->m_d3d_device->CreateVertexBuffer(QT_VERTEX_BUF_SIZE*sizeof(vertex), D3DUSAGE_DYNAMIC|D3DUSAGE_WRITEONLY,
        QD3DFVF_CSVERTEX,
        D3DPOOL_DEFAULT, &m_d3dvbuff, NULL))) {
            qWarning() << "QDirect3DPaintEngine: failed to create vertex buffer.";
    }

    m_pe->m_d3d_device->SetStreamSource(0, m_d3dvbuff, 0, sizeof(vertex));
    m_pe->m_d3d_device->SetFVF(QD3DFVF_CSVERTEX);

    m_startindex = 0;
    m_index = 0;
}

IDirect3DSurface9 *QD3DDrawHelper::freeMaskSurface()
{
     // we need to make sure the mask is cleared when it's used for something else
    resetMask();
    m_clearmask = true;

    return m_maskSurface;
}

int QD3DDrawHelper::drawAntialiasedMask(int offset, int maxoffset)
{
    int newoffset = offset;
    QD3DBatchItem *item = &(m_pe->m_batch.items[offset]);

    // set mask as render target
    if (FAILED(m_pe->m_d3d_device->SetRenderTarget(0, m_maskSurface)))
        qWarning() << "QDirect3DPaintEngine: SetRenderTarget failed!";

    if (m_clearmask) {
        m_pe->m_d3d_device->Clear(0, 0, D3DCLEAR_TARGET,D3DCOLOR_ARGB(0,0,0,0), 0, 0);
        m_clearmask = false;
    }

    // fill the mask
    m_pe->m_statemanager->beginPass(PASS_AA_CREATEMASK);
    for (; newoffset<maxoffset; ++newoffset) {
        item = &(m_pe->m_batch.items[newoffset]);
        if (!(item->m_info & QD3DBatchItem::BI_AA) || !(item->m_info & QD3DBatchItem::BI_MASK)) {
            break;
        } else if (item->m_info & QD3DBatchItem::BI_MASKFULL) {
            item->m_info &= ~QD3DBatchItem::BI_MASKFULL;
            m_clearmask = true;
            break;
        }

        m_pe->m_statemanager->startStateBlock();
        if (item->m_info & QD3DBatchItem::BI_MASKSCISSOR) {
            RECT rect;
            QRectF srect = item->m_brect.adjusted(item->m_xoffset, item->m_yoffset,
                item->m_xoffset, item->m_yoffset);
            rect.left = qMax(qRound(srect.left()), 0);
            rect.top = qMax(qRound(srect.top()), 0);
            rect.bottom = qMin(m_height, qRound(srect.bottom()));
            rect.right = qMin(m_width, qRound(srect.right()));
            m_pe->m_d3d_device->SetScissorRect(&rect);
            m_pe->m_statemanager->SetRenderState(D3DRS_SCISSORTESTENABLE, TRUE);
        }
        m_pe->m_statemanager->setMaskChannel(item->m_maskpos.channel);
        m_pe->m_statemanager->endStateBlock();

#ifdef QT_DEBUG_VERTEXBUFFER_ACCESS
        int vbstart = item->m_offset;
        for (int i=vbstart; i<(vbstart + (item->m_count * 3)); ++i) {
            if (accesscontrol[i] != WRITE)
                qDebug() << "Vertex Buffer: Access Error";
            accesscontrol[i] |= READ;
        }
#endif

        m_pe->m_d3d_device->DrawPrimitive(D3DPT_TRIANGLELIST, item->m_offset, item->m_count);

        if (item->m_info & QD3DBatchItem::BI_MASKSCISSOR) {
            m_pe->m_statemanager->SetRenderState(D3DRS_SCISSORTESTENABLE, FALSE);
        }
    }
    m_pe->m_statemanager->endPass();

    return newoffset;
}

void QD3DDrawHelper::drawAliasedMask(int offset)
{
    QD3DBatchItem *item = &(m_pe->m_batch.items[offset]);
    if (item->m_info & QD3DBatchItem::BI_MASK) {
        m_pe->m_statemanager->beginPass( (item->m_info & QD3DBatchItem::BI_WINDING) ? PASS_STENCIL_WINDING : PASS_STENCIL_ODDEVEN );
        int prev_stop = 0;
        for (int i=0; i<item->m_pointstops.count(); ++i) {
            int stop = item->m_pointstops.at(i);

#ifdef QT_DEBUG_VERTEXBUFFER_ACCESS
            int vbstart = (item->m_offset + prev_stop);
            for (int j=vbstart; j<(vbstart+(stop - prev_stop)); ++j) {
                if (accesscontrol[j] != WRITE)
                    qDebug() << "Vertex Buffer: Access Error";
                accesscontrol[j] |= READ;
            }
#endif
            m_pe->m_d3d_device->DrawPrimitive(D3DPT_TRIANGLEFAN, item->m_offset + prev_stop, (stop - prev_stop) - 2);
            prev_stop = stop;
        }
        m_pe->m_statemanager->endPass();
    }
}

void QD3DDrawHelper::drawTextItem(QD3DBatchItem *item)
{
#ifdef QT_DEBUG_VERTEXBUFFER_ACCESS
    int vbstart = item->m_offset;
    for (int j=vbstart; j<(vbstart + ((item->m_count * 2) * 3)); ++j) {
        if (accesscontrol[j] != WRITE)
            qDebug() << "Vertex Buffer: Access Error";
        accesscontrol[j] |= READ;
    }
#endif
    m_pe->m_d3d_device->DrawPrimitive(D3DPT_TRIANGLELIST, item->m_offset, item->m_count*2);
}

void QD3DDrawHelper::drawAliasedLines(QD3DBatchItem *item)
{
    m_pe->m_statemanager->setCosmeticPen(item->m_info & QD3DBatchItem::BI_COSMETICPEN);
    if (item->m_info & QD3DBatchItem::BI_TRANSFORM) {
        m_pe->m_statemanager->setTransformation(&item->m_matrix);
    } else {
        m_pe->m_statemanager->setTransformation();
    }
    int pass = (item->m_info & QD3DBatchItem::BI_MASK)
               ? PASS_ALIASED_LINES
               : PASS_ALIASED_LINES_DIRECT;
    m_pe->m_statemanager->beginPass(pass);
    m_pe->m_d3d_device->DrawPrimitive(D3DPT_TRIANGLELIST, item->m_offset, (item->m_count + 2) / 3);
    m_pe->m_statemanager->endPass();
}

void QD3DDrawHelper::drawAntialiasedBoundingRect(QD3DBatchItem *item)
{
    if (item->m_info & QD3DBatchItem::BI_SCISSOR) {
        RECT rect;
        rect.left = qMax(qRound(item->m_brect.left()), 0);
        rect.top = qMax(qRound(item->m_brect.top()), 0);
        rect.bottom = qMin(m_height, qRound(item->m_brect.bottom()));
        rect.right = qMin(m_width, qRound(item->m_brect.right()));
        m_pe->m_d3d_device->SetScissorRect(&rect);
        m_pe->m_statemanager->SetRenderState(D3DRS_SCISSORTESTENABLE, TRUE);
    }

#ifdef QT_DEBUG_VERTEXBUFFER_ACCESS
    int vbstart = item->m_offset + (item->m_count * 3);
    for (int j=vbstart; j<(vbstart + 4); ++j) {
        if (accesscontrol[j] != WRITE)
            qDebug() << "Vertex Buffer: Access Error";
        accesscontrol[j] |= READ;
    }
#endif

    m_pe->m_d3d_device->DrawPrimitive(D3DPT_TRIANGLEFAN, item->m_offset + (item->m_count * 3), 2);

    if (item->m_info & QD3DBatchItem::BI_SCISSOR) {
        m_pe->m_statemanager->SetRenderState(D3DRS_SCISSORTESTENABLE, FALSE);
    }
}

void QD3DDrawHelper::drawAliasedBoundingRect(QD3DBatchItem *item)
{
#ifdef QT_DEBUG_VERTEXBUFFER_ACCESS
    int vbstart = (item->m_offset + item->m_count + 2);
    for (int j=vbstart; j<(vbstart + 4); ++j) {
        if (accesscontrol[j] != WRITE)
            qDebug() << "Vertex Buffer: Access Error";
        accesscontrol[j] |= READ;
    }
#endif

    m_pe->m_d3d_device->DrawPrimitive(D3DPT_TRIANGLEFAN, item->m_offset + item->m_count + 2, 2);
}

void QD3DDrawHelper::addTrap(const Trapezoid &trap)
{
    qreal topLeftY = Q27Dot5ToDouble(trap.topLeft->y) - m_yoffset;
    qreal topLeftX = Q27Dot5ToDouble(trap.topLeft->x) - m_xoffset;
    qreal topRightY = Q27Dot5ToDouble(trap.topRight->y) - m_yoffset;
    qreal topRightX = Q27Dot5ToDouble(trap.topRight->x) - m_xoffset;
    qreal top = Q27Dot5ToDouble(trap.top) - m_yoffset;
    qreal bottom = Q27Dot5ToDouble(trap.bottom) - m_yoffset;

    Q27Dot5 _h = trap.topLeft->y - trap.bottomLeft->y;
    Q27Dot5 _w = trap.topLeft->x - trap.bottomLeft->x;
    qreal _leftA = (qreal)_w/_h;
    qreal _leftB = topLeftX - _leftA * topLeftY;

    _h = trap.topRight->y - trap.bottomRight->y;
    _w = trap.topRight->x - trap.bottomRight->x;
    qreal _rightA = (qreal)_w/_h;
    qreal _rightB = topRightX - _rightA * topRightY;

    qreal invLeftA = qFuzzyCompare(_leftA + 1, 1) ? 0.0 : 1.0 / _leftA;
    qreal invRightA = qFuzzyCompare(_rightA + 1, 1) ? 0.0 : 1.0 / _rightA;

    vertex v1 = { {1.f, top - 1.f, 0.5f}, 0.f,
        top, bottom, invLeftA, -invRightA,
        _leftA, _leftB, _rightA, _rightB};
    vertex v2 = { {0.f, top - 1.f, 0.5f}, 0.f,
        top, bottom, invLeftA, -invRightA,
        _leftA, _leftB, _rightA, _rightB};
    vertex v3 = { {0.f, bottom + 1.f, 0.5f}, 0.f,
        top, bottom, invLeftA, -invRightA,
        _leftA, _leftB, _rightA, _rightB};

    vertex v4 = { {1.f, top - 1.f, 0.5f}, 0.f,
        top, bottom, invLeftA, -invRightA,
        _leftA, _leftB, _rightA, _rightB};
    vertex v5 = { {0.f, bottom + 1.f, 0.5f}, 0.f,
        top, bottom, invLeftA, -invRightA,
        _leftA, _leftB, _rightA, _rightB};
    vertex v6 = { {1.f, bottom + 1.f, 0.5f}, 0.f,
        top, bottom, invLeftA, -invRightA,
        _leftA, _leftB, _rightA, _rightB};

#ifdef QT_DEBUG_VERTEXBUFFER_ACCESS
    for (int i=m_index; i<(m_index + 6); ++i) {
        if ((m_index + 6) > QT_VERTEX_BUF_SIZE)
            qDebug() << "Vertex Buffer: Buffer overflow";
        if (accesscontrol[i] != CLEAR)
            qDebug() << "Vertex Buffer: Access Error";
        accesscontrol[i] |= WRITE;
    }
#endif

    m_vbuff[m_index++] = v1;
    m_vbuff[m_index++] = v2;
    m_vbuff[m_index++] = v3;
    m_vbuff[m_index++] = v4;
    m_vbuff[m_index++] = v5;
    m_vbuff[m_index++] = v6;

    // check if buffer is full
    if (m_index >= (QT_VERTEX_BUF_SIZE - 16)) {
        m_item->m_offset = m_startindex;
        m_item->m_count = ( m_index - m_startindex ) / 3;
        m_startindex = m_index;

        QD3DBatchItem itemcopy = *m_item;
        m_item = m_pe->nextBatchItem();
        *m_item = itemcopy;
        m_item->m_info &= ~QD3DBatchItem::BI_MASKFULL;

        lockVertexBuffer();
    }
}

void QD3DDrawHelper::tessellate(const QPolygonF &poly) {
    int xoffset = m_item->m_maskpos.x;
    int yoffset = m_item->m_maskpos.y;

    int x = m_boundingRect.left();
    int y = m_boundingRect.top();
    m_xoffset = (x - xoffset) + 0.5f;
    m_yoffset = (y - yoffset) + 0.5f;

    QTessellator::tessellate(poly.data(), poly.count());

    m_item->m_offset = m_startindex;
    m_item->m_count = ( m_index - m_startindex ) / 3;
    m_startindex = m_index;
}

inline void QD3DDrawHelper::lineToStencil(qreal x, qreal y)
{
    QPointF lastPt = tess_lastpoint;
    tess_lastpoint = QPointF(x, y);

    if (m_isLine && m_firstPoint)
        return;


#ifdef QT_DEBUG_VERTEXBUFFER_ACCESS
    if (m_index > QT_VERTEX_BUF_SIZE)
        qDebug() << "Vertex Buffer: Buffer overflow";
    if (accesscontrol[m_index] != CLEAR)
        qDebug() << "Vertex Buffer: Access Error";
    accesscontrol[m_index] |= WRITE;
#endif

    vertex v;
    if (m_isLine) {
        vertex v1 = { {lastPt.x(), lastPt.y(), m_pe->m_pen_width }, m_color,
                     -1.f, -1.f, x, y,
                     0.f, 0.f, 0.f, 0.f};
        vertex v2 = { {lastPt.x(), lastPt.y(), m_pe->m_pen_width }, m_color,
                     1.f, -1.f, x, y,
                     0.f, 0.f, 0.f, 0.f};
        vertex v3 = { {lastPt.x(), lastPt.y(), m_pe->m_pen_width }, m_color,
                     1.f, 1.f, x, y,
                     0.f, 0.f, 0.f, 0.f};
        vertex v4 = { {lastPt.x(), lastPt.y(), m_pe->m_pen_width }, m_color,
                     -1.f, 1.f, x, y,
                     0.f, 0.f, 0.f, 0.f};
        m_vbuff[m_index++] = v1;
        m_vbuff[m_index++] = v2;
        m_vbuff[m_index++] = v4;
        m_vbuff[m_index++] = v4;
        m_vbuff[m_index++] = v2;
        m_vbuff[m_index++] = v3;
    } else {
        vertex v1 = { {x, y, 0.5f}, m_color,
                     0.f, 0.f, 0.f, 0.f,
                     0.f, 0.f, 0.f, 0.f};
        m_vbuff[m_index++] = v1;
        v = v1;
    }
    ++tess_index;

    // check if buffer is full
    if (m_index >= (QT_VERTEX_BUF_SIZE - 16)) {
        int firstindex = m_startindex;
        if (!m_item->m_pointstops.isEmpty())
            firstindex = m_item->m_pointstops.last();

        vertex first = m_vbuff[firstindex];

        // finish current polygon
        m_item->m_pointstops.append(tess_index);
        m_item->m_offset = m_startindex;
        m_startindex = m_index;

        // copy item
        QD3DBatchItem itemcopy = *m_item;
        m_item = m_pe->nextBatchItem();
        *m_item = itemcopy;

        // start new polygon
        lockVertexBuffer();
        m_item->m_pointstops.clear();
        if (!m_isLine) {
            tess_index = 2;

#ifdef QT_DEBUG_VERTEXBUFFER_ACCESS
            if (accesscontrol[m_index] != CLEAR)
                qDebug() << "Vertex Buffer: Access Error";
            accesscontrol[m_index] |= WRITE;
#endif

            m_vbuff[m_index++] = first;

#ifdef QT_DEBUG_VERTEXBUFFER_ACCESS
            if (accesscontrol[m_index] != CLEAR)
                qDebug() << "Vertex Buffer: Access Error";
            accesscontrol[m_index] |= WRITE;
#endif

            m_vbuff[m_index++] = v;
        } else {
            tess_index = 0;
        }
    }

    if (x > max_x)
        max_x = x;
    else if (x < min_x)
        min_x = x;
    if (y > max_y)
        max_y = y;
    else if (y < min_y)
        min_y = y;
}

inline void QD3DDrawHelper::curveToStencil(const QPointF &cp1, const QPointF &cp2,
                                             const QPointF &ep)
{
    qreal inverseScale = 0.5f;
    qreal inverseScaleHalf = inverseScale / 2;

    QBezier beziers[32];
    beziers[0] = QBezier::fromPoints(tess_lastpoint, cp1, cp2, ep);
    QBezier *b = beziers;
    while (b >= beziers) {
        // check if we can pop the top bezier curve from the stack
        qreal l = qAbs(b->x4 - b->x1) + qAbs(b->y4 - b->y1);
        qreal d;
        if (l > inverseScale) {
            d = qAbs( (b->x4 - b->x1)*(b->y1 - b->y2) - (b->y4 - b->y1)*(b->x1 - b->x2) )
                + qAbs( (b->x4 - b->x1)*(b->y1 - b->y3) - (b->y4 - b->y1)*(b->x1 - b->x3) );
            d /= l;
        } else {
            d = qAbs(b->x1 - b->x2) + qAbs(b->y1 - b->y2) +
                qAbs(b->x1 - b->x3) + qAbs(b->y1 - b->y3);
        }
        if (d < inverseScaleHalf || b == beziers + 31) {
            // good enough, we pop it off and add the endpoint
            lineToStencil(b->x4, b->y4);
            --b;
        } else {
            // split, second half of the polygon goes lower into the stack
            b->split(b+1, b);
           ++b;
        }
    }
}

QRectF QD3DDrawHelper::pathToVertexArrays(const QPainterPath &path)
{
    m_isLine = (m_item->m_info & QD3DBatchItem::BI_FASTLINE);
    const QPainterPath::Element &first = path.elementAt(0);
    firstx = first.x;
    firsty = first.y;
    min_x = max_x = firstx;
    min_y = max_y = firsty;

    m_firstPoint = true;
    tess_index = 0;
    m_item->m_pointstops.clear();
    lineToStencil(firstx, firsty);
    m_firstPoint = false;

    for (int i=1; i<path.elementCount(); ++i) {
        const QPainterPath::Element &e = path.elementAt(i);
        switch (e.type) {
        case QPainterPath::MoveToElement:
            m_item->m_pointstops.append(tess_index);
            m_firstPoint = true;
            lineToStencil(e.x, e.y);
            m_firstPoint = false;
            break;
        case QPainterPath::LineToElement:
            lineToStencil(e.x, e.y);
            break;
        case QPainterPath::CurveToElement:
            curveToStencil(e, path.elementAt(i+1), path.elementAt(i+2));
            i+=2;
            break;
        default:
            break;
        }
    }

    if (!m_isLine)
        lineToStencil(firstx, firsty);

    m_item->m_pointstops.append(tess_index);

    m_item->m_offset = m_startindex;
    m_item->m_count = ( m_index - m_startindex ) - 2;
    m_startindex = m_index;

    QRectF result;
    result.setLeft(min_x);
    result.setRight(max_x);
    result.setTop(min_y);
    result.setBottom(max_y);

    if (m_isLine)
        result.adjust(0,0,1,1);

    return result;
}

void QD3DDrawHelper::resetMask()
{
    m_mask_position.x = m_mask_position.y = QD3D_MASK_MARGIN;
    m_mask_position.channel = 0;
    m_mask_offsetX2 = m_mask_offsetY2 = QD3D_MASK_MARGIN;
}


static inline QPainterPath strokeForPath(const QPainterPath &path, const QPen &cpen) {
    QPainterPathStroker stroker;
    if (cpen.style() == Qt::CustomDashLine)
        stroker.setDashPattern(cpen.dashPattern());
    else
        stroker.setDashPattern(cpen.style());

    stroker.setCapStyle(cpen.capStyle());
    stroker.setJoinStyle(cpen.joinStyle());
    stroker.setMiterLimit(cpen.miterLimit());
    stroker.setWidth(cpen.widthF());

    QPainterPath stroke = stroker.createStroke(path);
    stroke.setFillRule(Qt::WindingFill);
    return stroke;
}


QDirect3DPaintEnginePrivate::~QDirect3DPaintEnginePrivate()
{

}

void QDirect3DPaintEnginePrivate::updateClipPath(const QPainterPath &path, Qt::ClipOperation op)
{
    //#### remove me
    QRegion r(path.toFillPolygon().toPolygon(), path.fillRule());
    updateClipRegion(r, op);

/*    if (m_draw_helper->needsFlushing())
        flushBatch();

    if (op == Qt::IntersectClip && !has_clipping)
        op = Qt::ReplaceClip;

    // switch to paths
    if (!m_has_complex_clipping) {
        m_clip_path = QPainterPath();
        m_clip_path.addRegion(m_clip_region);
        m_clip_region = QRegion();
        m_sysclip_path = QPainterPath();
        m_sysclip_path.addRegion(m_sysclip_region);
        m_sysclip_region = QRegion();
        m_has_complex_clipping = true;
    }

    QPainterPath cpath = m_matrix.map(path);

    QD3DBatchItem *item = &m_batch.items[m_batch.m_item_index++];
    item->m_info = QD3DBatchItem::BI_COMPLEXCLIP;

    switch (op) {
        case Qt::UniteClip:
            has_clipping = true;
            m_clip_path = m_clip_path.united(cpath);
            break;
        case Qt::ReplaceClip:
            has_clipping = true;
            m_clip_path = cpath;
            break;
        case Qt::NoClip:
            m_has_complex_clipping = false;
            has_clipping = false;
            item->m_info |= QD3DBatchItem::BI_CLEARCLIP;
            break;
        default: // intersect clip
            has_clipping = true;
            m_clip_path = m_clip_path.intersected(cpath);
            break;
    }

    if (!m_sysclip_path.isEmpty()) {
        item->m_info &= ~QD3DBatchItem::BI_CLEARCLIP;
        if (has_clipping)
            m_clip_path = m_clip_path.intersected(m_sysclip_path);
        else
            m_clip_path = m_sysclip_path;
    }

    // update the aliased clipping mask
    m_draw_helper->setClipPath(m_clip_path, item);

    // update the antialiased clipping mask
    if (m_draw_helper->needsFlushing())
        flushBatch();

    QD3DBatchItem *aaitem = &m_batch.items[m_batch.m_item_index++];
    aaitem->m_info = item->m_info|QD3DBatchItem::BI_AA;
    m_draw_helper->setClipPath(m_clip_path, aaitem); */
}

void QDirect3DPaintEnginePrivate::updateClipRegion(const QRegion &clipregion, Qt::ClipOperation op)
{
    if (m_draw_helper->needsFlushing())
        flushBatch();
    if (m_has_complex_clipping) {
        QPainterPath path;
        path.addRegion(clipregion);
        updateClipPath(path, op);
        return;
    }

    if (op == Qt::IntersectClip && m_clip_region.isEmpty())
        op = Qt::ReplaceClip;

    QRegion cregion = m_matrix.map(clipregion);

    QD3DBatchItem *item = nextBatchItem();
    item->m_info &= ~QD3DBatchItem::BI_AA;

    switch (op) {
        case Qt::UniteClip:
            m_clip_region = m_clip_region.united(cregion);
            break;
        case Qt::ReplaceClip:
            m_clip_region = cregion;
            break;
        case Qt::NoClip:
            m_clip_region = QRegion();
            item->m_info |= QD3DBatchItem::BI_CLEARCLIP;
            break;
        default: // intersect clip
            m_clip_region = m_clip_region.intersected(cregion);
            break;
    }

    QRegion crgn = m_clip_region;
    if (!m_sysclip_region.isEmpty()) {
        item->m_info &= ~QD3DBatchItem::BI_CLEARCLIP;
        if (!crgn.isEmpty())
            crgn = crgn.intersected(m_sysclip_region);
        else
            crgn = m_sysclip_region;
    }

    QPainterPath path;
    path.addRegion(crgn);
    m_draw_helper->setClipPath(path, &item);
}

void QDirect3DPaintEnginePrivate::updateFont(const QFont &)
{
}

void QDirect3DPaintEnginePrivate::setRenderTechnique(RenderTechnique technique)
{
    if (m_current_technique != technique) {
        if (m_current_technique != RT_NoTechnique)
            m_effect->End();

        if (technique == RT_Aliased) {
            m_effect->SetTechnique("Aliased");
            m_effect->Begin(0,D3DXFX_DONOTSAVESTATE);
        } else if (technique == RT_Antialiased) {
            m_effect->SetTechnique("Antialiased");
            m_effect->Begin(0,D3DXFX_DONOTSAVESTATE);
        }
    }

    m_current_technique = technique;
}

/*QPolygonF QDirect3DPaintEnginePrivate::transformedRect(const QRectF &brect) const
{
    QPolygonF poly(brect);
    return m_matrix.map(poly);
}

QPolygonF QDirect3DPaintEnginePrivate::calcTextureCoords(const QPolygonF &trect) const
{
    QPolygonF result(4);
    QRectF brect = trect.boundingRect();
    qreal angle = atan(trect.at(0).x() -
}

QPolygonF QDirect3DPaintEnginePrivate::offsetTextureCoords(const QRectF &brect, const QPolygonF &trect) const
{

}*/

inline QD3DBatchItem *QDirect3DPaintEnginePrivate::nextBatchItem()
{
    if (m_draw_helper->needsFlushing())
        flushBatch();

    QD3DBatchItem *item = &m_batch.items[m_batch.m_item_index++];
    item->m_info = m_current_state;
    item->m_cmode = m_cmode;
    return item;
}

qreal calculateAngle(qreal dx, qreal dy)
{
    qreal angle;

    if (qFuzzyCompare(dx + 1, 1)) {
        angle = (dy < 0) ? -M_PI/2 : M_PI/2;
    } else {
        angle = atanf(dy/dx);
        if (dx < 0)
            angle += M_PI;
    }

    return angle;
}

QPolygonF QDirect3DPaintEnginePrivate::brushCoordinates(const QRectF &r, bool stroke, qreal *fd) const
{
    QBrush brush;
    QTransform matrix;
    Qt::BrushStyle style;

    if (stroke) {
        brush = m_pen.brush();
        matrix = m_inv_pen_matrix;
        style = m_pen_brush_style;
    } else {
        brush = m_brush;
        matrix = m_inv_brush_matrix;
        style = m_brush_style;
    }

    QPolygonF bpoly;
    switch(style) {
        case Qt::TexturePattern: {
            QTransform totxcoords;
            QRectF adj_brect = r.adjusted(-0.5f, -0.5f, -0.5f, -0.5f);
            totxcoords.scale(1.0f/brush.texture().width(),
                1.0f/brush.texture().height());
            bpoly = matrix.map(QPolygonF(adj_brect));
            bpoly = totxcoords.map(bpoly);
            break; }
        case Qt::LinearGradientPattern: {
            const QLinearGradient *g = static_cast<const QLinearGradient *>(brush.gradient());
            QPointF start = g->start();
            QPointF stop = g->finalStop();
            qreal dx = stop.x() - start.x();
            qreal dy = stop.y() - start.y();
            qreal length = sqrt(dx * dx + dy * dy);
            qreal angle = calculateAngle(dx, dy);
            QTransform totxcoords;
            QRectF adj_brect = r.adjusted(-0.5f, -0.5f, -0.5f, -0.5f);
            totxcoords.scale(1.0f/length, 1.0f/length);
            totxcoords.rotateRadians(-angle);
            totxcoords.translate(-start.x(), -start.y());
            bpoly = matrix.map(QPolygonF(adj_brect));
            bpoly = totxcoords.map(bpoly);
            break; }
        case Qt::ConicalGradientPattern: {
            const QConicalGradient *g = static_cast<const QConicalGradient *>(brush.gradient());
            QPointF center = g->center();
            qreal angle = g->angle();
            QTransform totxcoords;
            totxcoords.rotate(angle);
            totxcoords.translate(-center.x(), -center.y());
            bpoly = matrix.map(QPolygonF(r));
            bpoly = totxcoords.map(bpoly);
            break; }
        case Qt::RadialGradientPattern: {
            const QRadialGradient *g = static_cast<const QRadialGradient *>(brush.gradient());
            QPointF center = g->center();
            QPointF focalpoint = g->focalPoint();
            qreal dx = focalpoint.x() - center.x();
            qreal dy = focalpoint.y() - center.y();
            qreal radius = g->radius();
            *fd = sqrt(dx * dx + dy * dy) / radius;
            qreal angle = calculateAngle(dx, dy);
            QTransform totxcoords;
            totxcoords.scale(1.0f/radius, 1.0f/radius);
            totxcoords.rotateRadians(-angle);
            totxcoords.translate(-center.x(), -center.y());
            bpoly = matrix.map(QPolygonF(r));
            bpoly = totxcoords.map(bpoly);
            break; }
        default: {
            QTransform totxcoords;
            QRectF adj_brect = r.adjusted(-0.5f, -0.5f, -0.5f, -0.5f);
            QPixmap pat = getPattern(style);
            totxcoords.scale(1.0f/pat.width(),
                1.0f/pat.height());
            bpoly = matrix.map(QPolygonF(adj_brect));
            bpoly = totxcoords.map(bpoly); }
    };

    return bpoly;
}

void QDirect3DPaintEnginePrivate::strokeAliasedPath(QPainterPath path, const QRectF &brect, const QTransform &txform)
{
    D3DCOLOR solid_color;
    QD3DBatchItem *item = nextBatchItem();

    if (!txform.isIdentity())
        path = txform.map(path);

    QRectF trect;
    QPolygonF txcoord;

    solid_color = m_pen_color;
    bool has_complex_brush = false;
    if (m_pen_brush_style != Qt::SolidPattern) {
        has_complex_brush = true;
        item->m_brush = m_pen.brush();
        item->m_info |= QD3DBatchItem::BI_COMPLEXBRUSH;
        item->m_opacity = m_opacity;
    }

    if (m_has_fast_pen) {
        item->m_info |= QD3DBatchItem::BI_FASTLINE;
        if (m_pen_brush_style == Qt::SolidPattern) {
            m_draw_helper->queueAliasedMask(path, &item, solid_color);
            item->m_info &= ~QD3DBatchItem::BI_MASK; // bypass stencil buffer
            return;
        }
    }

    QRectF txrect = m_draw_helper->queueAliasedMask(path, &item, 0);

    if (has_complex_brush) {
        trect = brect;
        txcoord = brushCoordinates(brect, true, &item->m_distance);
        item->m_info |= QD3DBatchItem::BI_TRANSFORM;
        item->m_matrix = m_matrix;
    } else {
        trect = txrect;
        static const QPolygonF empty_poly(4);
        txcoord = empty_poly;
    }

    m_draw_helper->queueRect(trect, item, solid_color, txcoord);
}

void QDirect3DPaintEnginePrivate::fillAliasedPath(QPainterPath path, const QRectF &brect, const QTransform &txform)
{
    D3DCOLOR solid_color;
    QD3DBatchItem *item = nextBatchItem();

    if (!txform.isIdentity())
        path = txform.map(path);

    QRectF trect;
    QPolygonF txcoord;

    solid_color = m_brush_color;
    bool has_complex_brush = false;
    if (m_brush_style != Qt::SolidPattern) {
        has_complex_brush = true;
        item->m_brush = m_brush;
        item->m_info |= QD3DBatchItem::BI_COMPLEXBRUSH;
        item->m_opacity = m_opacity;
    }

    QRectF txrect = m_draw_helper->queueAliasedMask(path, &item, 0);

    if (has_complex_brush) {
        trect = brect;
        txcoord = brushCoordinates(brect, false, &item->m_distance);
        item->m_info |= QD3DBatchItem::BI_TRANSFORM;
        item->m_matrix = m_matrix;
    } else {
        trect = txrect;
        static const QPolygonF empty_poly(4);
        txcoord = empty_poly;
    }

    m_draw_helper->queueRect(trect, item, solid_color, txcoord);
}

void QDirect3DPaintEnginePrivate::fillAntialiasedPath(const QPainterPath &path, const QRectF &brect,
                                                      const QTransform &txform, bool stroke)
{
    D3DCOLOR solid_color;
    bool winding = (path.fillRule() == Qt::WindingFill);
    QPolygonF poly;
    QRectF txrect;
    QPainterPath tpath;

    if (m_has_aa_fast_pen && stroke) {
        tpath = txform.map(path);
        txrect = tpath.controlPointRect();
        txrect.adjust(-(m_pen_width/2),-(m_pen_width/2), m_pen_width, m_pen_width);
    } else {
        poly = path.toFillPolygon(txform);
        txrect = poly.boundingRect();
    }

    // brect = approx. bounding rect before transformation
    // txrect = exact bounding rect after transformation
    // trect = the rectangle to be drawn
    // txcoord = the texture coordinates
    // adj_txrect = adjusted rect to include aliased outline

    bool use_scissor = false;
    if (txrect.left() < 0) {
        txrect.adjust(-txrect.left(),0,0,0);
        use_scissor = true;
    }
    if (txrect.top() < 0) {
        txrect.adjust(0,-txrect.top(),0,0);
        use_scissor = true;
    }

    if (!txrect.isValid())
        return;

    QD3DBatchItem *item = nextBatchItem();

    QRectF adj_txrect = txrect.adjusted(-1,-1,1,1);
    QRectF trect;
    QPolygonF txcoord;

    bool has_complex_brush = false;
    if (stroke) {
        solid_color = m_pen_color;
        if (m_pen_brush_style != Qt::SolidPattern) {
            has_complex_brush = true;
            item->m_brush = m_pen.brush();
        }
        item->m_width = m_pen_width;
    } else {
        solid_color = m_brush_color;
        if (m_brush_style != Qt::SolidPattern) {
            has_complex_brush = true;
            item->m_brush = m_brush;
        }
    }

    qreal focaldist = 0;
    if (has_complex_brush) {
        trect = brect;
        txcoord = brushCoordinates(brect, stroke, &focaldist);
    } else {
        trect = adj_txrect;
        static const QPolygonF empty_poly(4);
        txcoord = empty_poly;
    }

    bool maskfull;
    item->m_maskpos = m_draw_helper->allocateMaskPosition(txrect, &maskfull);
    if (maskfull)
        item->m_info |= QD3DBatchItem::BI_MASKFULL;
    item->m_distance = focaldist;

    if (winding)
        item->m_info |= QD3DBatchItem::BI_WINDING;

    if (has_complex_brush) {
        item->m_info |= QD3DBatchItem::BI_SCISSOR|QD3DBatchItem::BI_COMPLEXBRUSH|
            QD3DBatchItem::BI_TRANSFORM;
        item->m_brect = adj_txrect;
        item->m_matrix = m_matrix;
        item->m_opacity = m_opacity;
    }
    if (use_scissor) {
        item->m_info |= QD3DBatchItem::BI_MASKSCISSOR;
        item->m_brect = adj_txrect;
    }

    if (m_has_aa_fast_pen && stroke) {
        m_draw_helper->queueAntialiasedLines(tpath, &item, txrect);
    } else {
        m_draw_helper->queueAntialiasedMask(poly, &item, txrect);
    }

    m_draw_helper->queueRect(trect, item, solid_color, txcoord);
}

QPainterPath QDirect3DPaintEnginePrivate::strokePathFastPen(const QPainterPath &path)
{
    QPainterPath result;
    QBezier beziers[32];
    for (int i=0; i<path.elementCount(); ++i) {
        const QPainterPath::Element &e = path.elementAt(i);
        switch (e.type) {
        case QPainterPath::MoveToElement:
            result.moveTo(e.x, e.y);
            break;
        case QPainterPath::LineToElement:
            result.lineTo(e.x, e.y);
            break;

        case QPainterPath::CurveToElement:
        {
            QPointF sp = path.elementAt(i-1);
            QPointF cp2 = path.elementAt(i+1);
            QPointF ep = path.elementAt(i+2);
            i+=2;

            qreal inverseScaleHalf = m_inv_scale / 2;
            beziers[0] = QBezier::fromPoints(sp, e, cp2, ep);
            QBezier *b = beziers;
            while (b >= beziers) {
                // check if we can pop the top bezier curve from the stack
                qreal l = qAbs(b->x4 - b->x1) + qAbs(b->y4 - b->y1);
                qreal d;
                if (l > m_inv_scale) {
                    d = qAbs( (b->x4 - b->x1)*(b->y1 - b->y2)
                              - (b->y4 - b->y1)*(b->x1 - b->x2) )
                        + qAbs( (b->x4 - b->x1)*(b->y1 - b->y3)
                                - (b->y4 - b->y1)*(b->x1 - b->x3) );
                    d /= l;
                } else {
                    d = qAbs(b->x1 - b->x2) + qAbs(b->y1 - b->y2) +
                        qAbs(b->x1 - b->x3) + qAbs(b->y1 - b->y3);
                }
                if (d < inverseScaleHalf || b == beziers + 31) {
                    // good enough, we pop it off and add the endpoint
                    result.lineTo(b->x4, b->y4);
                    --b;
                } else {
                    // split, second half of the polygon goes lower into the stack
                    b->split(b+1, b);
                    ++b;
                }
            }
        } // case CurveToElement
        default:
            break;
        } // end of switch
    }
    return result;
}

void QDirect3DPaintEnginePrivate::strokePath(const QPainterPath &path, QRectF brect, bool simple)
{
    QTransform txform;
    QPainterPath tpath;

    if (m_has_fast_pen || m_has_aa_fast_pen) {
        if (!simple)
            tpath = strokePathFastPen(path);
        else
            tpath = path;  //already only lines
    } else {
        tpath = strokeForPath(path, m_pen);
    }

    if (tpath.isEmpty())
        return;

    //brect is null if the path is not transformed
    if (brect.isNull())
        txform = m_matrix;

    if (!brect.isNull()) {
        // brect is set when the path is transformed already,
        // this is the case when we have a cosmetic pen.
        brect.adjust(-(m_pen_width/2),-(m_pen_width/2), m_pen_width, m_pen_width);
    }

    if (brect.isNull())
        brect = tpath.controlPointRect();
    brect.adjust(-m_inv_scale,-m_inv_scale,m_inv_scale,m_inv_scale); //adjust for antialiasing

    if (m_current_state & QD3DBatchItem::BI_AA) {
        fillAntialiasedPath(tpath, brect, txform, true);
    } else {
        strokeAliasedPath(tpath, brect, txform);
    }
}

void QDirect3DPaintEnginePrivate::fillPath(const QPainterPath &path, QRectF brect)
{
    QTransform txform;

    //brect is null if the path is not transformed
    if (brect.isNull())
        txform = m_matrix;

    if (brect.isNull())
        brect = path.controlPointRect();
    brect.adjust(-m_inv_scale,-m_inv_scale,m_inv_scale,m_inv_scale); //adjust for antialiasing

    if (m_current_state & QD3DBatchItem::BI_AA) {
        fillAntialiasedPath(path, brect, txform, false);
    } else {
        fillAliasedPath(path, brect, txform);
    }
}


bool QDirect3DPaintEnginePrivate::init()
{
#ifdef QT_DEBUG_D3D_CALLS
    qDebug() << "QDirect3DPaintEnginePrivate::init()";
#endif

    m_draw_helper = 0;
    m_gradient_cache = 0;
    m_dc = 0;
    m_dcsurface = 0;

    m_supports_d3d = false;
    m_current_state = 0;
    m_in_scene = false;
    m_has_fast_pen = false;
    m_has_aa_fast_pen = false;
    m_has_pen = false;
    m_has_brush = false;
    m_pen_color = 0;
    m_brush_color = 0;
    m_current_surface = 0;
    m_batch.m_item_index = 0;
    m_current_technique = RT_NoTechnique;

    if (!pDirect3DCreate9) {
        QLibrary d3d_lib(QLatin1String("d3d9.dll"));
        pDirect3DCreate9 = (PFNDIRECT3DCREATE9) d3d_lib.resolve("Direct3DCreate9");
        if (!pDirect3DCreate9) {
            qWarning("QDirect3DPaintEngine: failed to resolve symbols from d3d9.dll.\n"
                     "Make sure you have the DirectX run-time installed.");
            return false;
        }
    }

    if (!pD3DXCreateBuffer || !pD3DXCreateEffect || !pD3DXMatrixOrthoOffCenterLH) {
        QLibrary d3dx_lib(QLatin1String("d3dx9_32.dll"));
        pD3DXCreateBuffer = (PFND3DXCREATEBUFFER) d3dx_lib.resolve("D3DXCreateBuffer");
        pD3DXCreateEffect = (PFND3DXCREATEEFFECT) d3dx_lib.resolve("D3DXCreateEffect");
        pD3DXMatrixOrthoOffCenterLH = (PFND3DXMATRIXORTHOOFFCENTERLH)
                                        d3dx_lib.resolve("D3DXMatrixOrthoOffCenterLH");
        if (!(pD3DXCreateBuffer && pD3DXCreateEffect && pD3DXMatrixOrthoOffCenterLH)) {
            qWarning("QDirect3DPaintEngine: failed to resolve symbols from d3dx9_32.dll.\n"
                     "Make sure you have the DirectX run-time installed.");
            return false;
        }
    }

    if (!m_d3d_object) {
        m_d3d_object = pDirect3DCreate9(D3D_SDK_VERSION);
        if (!m_d3d_object) {
            qWarning("QDirect3DPaintEngine: failed to create Direct3D object.\n"
                     "Direct3D support in Qt will be disabled.");
            return false;
        }
    }

    m_supports_d3d = testCaps();
    if (!m_supports_d3d)
        return false;

    m_surface_manager.init(m_d3d_object);
    m_d3d_device = m_surface_manager.device();

    if (!m_d3d_device)
        return false;

    /* load shaders */
    QFile file(QLatin1String(":/qpaintengine_d3d.fx"));
    QByteArray fxFile;
    if (file.open(QFile::ReadOnly))
        fxFile = file.readAll();

    if (fxFile.size() > 0) {
        LPD3DXBUFFER compout;
        pD3DXCreateBuffer(4096, &compout);
        DWORD dwShaderFlags = D3DXFX_NOT_CLONEABLE|D3DXFX_DONOTSAVESTATE|D3DXSHADER_OPTIMIZATION_LEVEL3;
        if(FAILED(pD3DXCreateEffect(m_d3d_device, fxFile.constData(), fxFile.size(),
                                    NULL, NULL, dwShaderFlags, NULL, &m_effect, &compout))) {
            qWarning("QDirect3DPaintEngine: failed to compile effect file");
            if (compout)
                qWarning((char *)compout->GetBufferPointer());
            m_supports_d3d = false;
            return false;
        }
        if (m_effect) {
            m_statemanager = new QD3DStateManager(m_d3d_device, m_effect);
            m_effect->SetStateManager(m_statemanager);
            m_draw_helper = new QD3DDrawHelper(this);
            initDevice();
            m_gradient_cache = new QD3DGradientCache(m_d3d_device);
        }
    } else {
        return false;
    }

    return true;
}

QPixmap QDirect3DPaintEnginePrivate::getPattern(Qt::BrushStyle style) const
{
    if (!m_patterns.contains(style)) {
        QImage img(16,16,QImage::Format_ARGB32);
        img.fill(0);
        QPainter p(&img);
        p.setBrush(QBrush(Qt::white, style));
        p.setPen(Qt::NoPen);
        p.drawRect(0,0,16,16);
        p.end();
        QPixmap pattern(QPixmap::fromImage(img));
        QDirect3DPaintEnginePrivate *ct = const_cast<QDirect3DPaintEnginePrivate *>(this);
        ct->verifyTexture(pattern);
        ct->m_patterns.insert(style, pattern);
    }

    return m_patterns.value(style);
}

bool QDirect3DPaintEnginePrivate::testCaps()
{
    D3DCAPS9 caps;
    if (FAILED(m_d3d_object->GetDeviceCaps(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, &caps)))
        return false;

    if ((caps.PresentationIntervals & D3DPRESENT_INTERVAL_IMMEDIATE)
        && (caps.DevCaps & D3DDEVCAPS_PUREDEVICE)
        && (caps.RasterCaps & D3DPRASTERCAPS_SCISSORTEST)
        && (caps.StencilCaps & D3DSTENCILCAPS_TWOSIDED))
        return true;
#if 0
    qDebug() << "Direct3D caps:";
    qDebug() << "D3DPRESENT_INTERVAL_IMMEDIATE:" << ((caps.PresentationIntervals & D3DPRESENT_INTERVAL_IMMEDIATE) != 0);
    qDebug() << "D3DDEVCAPS_PUREDEVICE:" << ((caps.DevCaps & D3DDEVCAPS_PUREDEVICE) != 0);
    qDebug() << "D3DPRASTERCAPS_SCISSORTEST:" << ((caps.RasterCaps & D3DPRASTERCAPS_SCISSORTEST) != 0);
    qDebug() << "D3DSTENCILCAPS_TWOSIDED:" << ((caps.StencilCaps & D3DSTENCILCAPS_TWOSIDED) != 0);
#endif
    return false;
}

void QDirect3DPaintEnginePrivate::initDevice()
{
    m_statemanager->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
    m_statemanager->SetRenderState(D3DRS_ZENABLE, D3DZB_FALSE);
    m_statemanager->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
    m_statemanager->SetRenderState(D3DRS_LIGHTING, FALSE);
    m_statemanager->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
    m_statemanager->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
    m_statemanager->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_DESTALPHA);
}

void QDirect3DPaintEnginePrivate::updatePen(const QPen &pen)
{
#ifdef QT_DEBUG_D3D_CALLS
    qDebug() << "QDirect3DPaintEngine::updatePen";
#endif
    m_pen = pen;
    m_has_cosmetic_pen = false;
    m_has_pen = (m_pen.style() != Qt::NoPen);
    if (m_has_pen) {
        m_pen_brush_style = m_pen.brush().style();

        if (m_pen_brush_style >= Qt::SolidPattern && m_pen_brush_style <= Qt::DiagCrossPattern) {
            int a, r, g, b;
            m_pen.color().getRgb(&r, &g, &b, &a);
            m_pen_color = D3DCOLOR_ARGB((int)(a * m_opacity),r,g,b);
        } else {
            m_pen_color = m_opacity_color;
        }

        m_has_cosmetic_pen = m_pen.isCosmetic();

        if (m_pen_brush_style != Qt::NoBrush &&
            m_pen_brush_style != Qt::SolidPattern) {
            bool ok;
            m_inv_pen_matrix = m_pen.brush().transform().inverted(&ok);
            if (!ok)
                qWarning() << "QDirect3DPaintEngine: No inverse matix for pen brush matrix.";
        }

        m_pen_width = m_pen.widthF();
        if (m_pen_width == 0.0f)
            m_pen_width = 1.0f;
    }
}

void QDirect3DPaintEnginePrivate::updateBrush(const QBrush &brush)
{
#ifdef QT_DEBUG_D3D_CALLS
    qDebug() << "QDirect3DPaintEngine::updateBrush";
#endif
    m_brush = brush;
    m_brush_style = m_brush.style();
    m_has_brush = (m_brush_style != Qt::NoBrush);
    if (m_has_brush) {
        if (m_brush_style >= Qt::SolidPattern && m_brush_style <= Qt::DiagCrossPattern) {
            int a, r, g, b;
            m_brush.color().getRgb(&r, &g, &b, &a);
            m_brush_color = D3DCOLOR_ARGB((int)(a * m_opacity),r,g,b);
        } else {
            m_brush_color = m_opacity_color;
        }

        if (m_brush_style != Qt::SolidPattern) {
            bool ok;
            m_inv_brush_matrix = (m_brush.transform() * m_brush_origin).inverted(&ok);
            if (!ok)
                qWarning() << "QDirect3DPaintEngine: No inverse matix for brush matrix.";

            // make sure the texture is loaded as a texture
            if (m_brush_style == Qt::TexturePattern)
                verifyTexture(m_brush.texture());


        }
    }
}

void QDirect3DPaintEnginePrivate::updateTransform(const QTransform &matrix)
{
#ifdef QT_DEBUG_D3D_CALLS
    qDebug() << "QDirect3DPaintEngine::updateTransform";
#endif
    m_matrix = matrix;
    m_inv_scale = qMax(1 / qMax( qMax(qAbs(m_matrix.m11()), qAbs(m_matrix.m22())),
        qMax(qAbs(m_matrix.m12()), qAbs(m_matrix.m21())) ), 0.0001);
    m_txop = matrix.type();
}

int QDirect3DPaintEnginePrivate::flushAntialiased(int offset)
{
    // fills the mask (returns number of items added to the mask)
    int newoffset = m_draw_helper->drawAntialiasedMask(offset, m_batch.m_item_index);

    // set the render target to the current output surface
    if (FAILED(m_d3d_device->SetRenderTarget(0, m_current_surface)))
        qWarning() << "QDirect3DPaintEngine: SetRenderTarget failed!";

    // draw the bounding boxes (using the mask generated by drawAntialiasedMask)
    for (int i=offset; i<newoffset; ++i) {
        QD3DBatchItem *item = &(m_batch.items[i]);
        int pass = (item->m_info & QD3DBatchItem::BI_COMPLEXBRUSH) ? PASS_AA_DRAW : PASS_AA_DRAW_DIRECT;
        m_statemanager->beginPass(pass);
        prepareItem(item);
        if (item->m_info & QD3DBatchItem::BI_BRECT)
            m_draw_helper->drawAntialiasedBoundingRect(item);
        cleanupItem(item);
    }

    m_statemanager->endPass();

    return newoffset;
}

bool QDirect3DPaintEnginePrivate::prepareBatch(QD3DBatchItem *item, int offset)
{
    if (item->m_info & QD3DBatchItem::BI_CLIP) {
        setRenderTechnique(RT_Aliased);
        if (item->m_info & QD3DBatchItem::BI_CLEARCLIP) {
            m_d3d_device->Clear(0, 0, D3DCLEAR_ZBUFFER, 0, 0.0f, 0);
            return true;
        }

        m_draw_helper->drawAliasedMask(offset);
        m_d3d_device->Clear(0, 0, D3DCLEAR_ZBUFFER, 0, 1.0f, 0);
        if (item->m_info & QD3DBatchItem::BI_BRECT) {
            m_statemanager->beginPass(PASS_STENCIL_CLIP);
            m_draw_helper->drawAliasedBoundingRect(item);
            m_statemanager->endPass();
        }

        return true;
    }

    if (item->m_info & QD3DBatchItem::BI_AA) {
        setRenderTechnique(RT_Antialiased);
    } else {
        setRenderTechnique(RT_Aliased);
    }

    return false;
}

void QDirect3DPaintEnginePrivate::prepareItem(QD3DBatchItem *item) {
    // pixmap
    int brushmode = 0;
    m_statemanager->startStateBlock();
    if ((item->m_info & QD3DBatchItem::BI_PIXMAP) || (item->m_info & QD3DBatchItem::BI_IMAGE)) {
        QRasterPixmapData *data = static_cast<QRasterPixmapData*>(item->m_pixmap.data);
        IDirect3DTexture9 *tex = (item->m_info & QD3DBatchItem::BI_PIXMAP) ?
                                 data->texture : item->m_texture;
        m_statemanager->setTexture(tex);
        brushmode = 5;
    }

    if (item->m_info & QD3DBatchItem::BI_AA) {
        m_statemanager->setMaskChannel(item->m_maskpos.channel);
        m_statemanager->setMaskOffset(item->m_xoffset, item->m_yoffset);
    }

    if (item->m_info & QD3DBatchItem::BI_COMPLEXBRUSH) {
        const QBrush brush = item->m_brush;
        switch (brush.style()) {
    	    case Qt::TexturePattern: {
                QRasterPixmapData *data = static_cast<QRasterPixmapData*>(brush.texture().data);
                m_statemanager->setTexture(data->texture, QGradient::RepeatSpread);
                brushmode = 1;
                break;
	    }
            case Qt::LinearGradientPattern:
                m_statemanager->setTexture(m_gradient_cache->
                    getBuffer(brush.gradient()->stops(), item->m_opacity),
                    brush.gradient()->spread());
                brushmode = 2;
                break;
            case Qt::ConicalGradientPattern:
                m_statemanager->setTexture(m_gradient_cache->
                    getBuffer(brush.gradient()->stops(), item->m_opacity),
                    brush.gradient()->spread());
                brushmode = 3;
                break;
            case Qt::RadialGradientPattern:
                m_statemanager->setTexture(m_gradient_cache->
                    getBuffer(brush.gradient()->stops(), item->m_opacity),
                    brush.gradient()->spread());
                m_statemanager->setFocalDistance(item->m_distance);
                brushmode = 4;
                break;
    	    default: {
                QRasterPixmapData *data = static_cast<QRasterPixmapData*>(getPattern(brush.style()).data);
                m_statemanager->setTexture(data->texture, QGradient::RepeatSpread);
                brushmode = 5;
	    }
        };
    }

    if (item->m_info & QD3DBatchItem::BI_TRANSFORM) {
        m_statemanager->setTransformation(&item->m_matrix);
    } else {
        m_statemanager->setTransformation();
    }

    m_statemanager->setBrushMode(brushmode);
    setCompositionMode(item->m_cmode);
    m_statemanager->endStateBlock();
}


void QDirect3DPaintEnginePrivate::releaseDC()
{
    if (m_dc) {
        m_dcsurface->ReleaseDC(m_dc);
        m_dcsurface = 0;
        m_dc = 0;
    }
}


void QDirect3DPaintEnginePrivate::cleanupItem(QD3DBatchItem *item)
{
    if (item->m_info & QD3DBatchItem::BI_PIXMAP)
        item->m_pixmap = QPixmap();
    item->m_brush = QBrush();
}

void QDirect3DPaintEnginePrivate::verifyTexture(const QPixmap &pm)
{
    QRasterPixmapData *pmData = static_cast<QRasterPixmapData*>(pm.data);
    if (!pmData->texture) {
        QImage im = pmData->image;
        // bitmaps are drawn with the current pen color
        if (im.depth() == 1) {
            QVector<QRgb> colors(2);
            colors[0] = 0;
            colors[1] = m_pen.color().rgba();
            im.setColorTable(colors);
        }
        im = im.convertToFormat(QImage::Format_ARGB32);
        if (FAILED(m_d3d_device->CreateTexture(im.width(), im.height(), 1, 0,
                                                 D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, &pmData->texture, 0)))
        {
            qWarning("QDirect3DPaintEngine: unable to create Direct3D texture from pixmap.");
            return;
        }
        D3DLOCKED_RECT rect;
        if (FAILED(pmData->texture->LockRect(0, &rect, 0, 0))) {
            qDebug() << "QDirect3DPaintEngine: unable to lock texture rect.";
            return;
        }
        DWORD *dst = (DWORD *) rect.pBits;
        DWORD *src = (DWORD *) im.scanLine(0);

        Q_ASSERT((rect.Pitch/4) == (im.bytesPerLine()/4));
        memcpy(dst, src, rect.Pitch*im.height());
        pmData->texture->UnlockRect(0);
    }
}

bool QDirect3DPaintEnginePrivate::isFastRect(const QRectF &rect)
{
    if (m_matrix.type() < QTransform::TxRotate) {
        QRectF r = m_matrix.mapRect(rect);
        return r.topLeft().toPoint() == r.topLeft()
            && r.bottomRight().toPoint() == r.bottomRight();
    }

    return false;
}

void QDirect3DPaintEnginePrivate::setCompositionMode(QPainter::CompositionMode mode)
{
    switch(mode) {
        case QPainter::CompositionMode_SourceOver:
        default:
            m_statemanager->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
            m_statemanager->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
    };
}

void QDirect3DPaintEnginePrivate::cleanup()
{
    // clean batch
    for(int i=0; i<QD3D_BATCH_SIZE; ++i) {
        m_batch.items[i].m_brush = QBrush();
        m_batch.items[i].m_pixmap = QPixmap();
    }

    m_surface_manager.cleanup();
    m_patterns.clear();

    delete m_gradient_cache;
    delete m_draw_helper;

    if (m_effect)
        m_effect->Release();

    if (m_d3d_object)
        m_d3d_object->Release();

    m_effect = 0;
    m_d3d_object = 0;
    m_gradient_cache = 0;
    m_draw_helper = 0;
}

void QDirect3DPaintEnginePrivate::flushAliased(QD3DBatchItem *item, int offset)
{
    m_draw_helper->drawAliasedMask(offset);

    if (item->m_info & QD3DBatchItem::BI_BRECT) {
        int pass = (item->m_info & QD3DBatchItem::BI_MASK) ? PASS_STENCIL_DRAW_DIRECT : PASS_STENCIL_NOSTENCILCHECK_DIRECT;
        if (item->m_info & (QD3DBatchItem::BI_COMPLEXBRUSH|QD3DBatchItem::BI_IMAGE|QD3DBatchItem::BI_PIXMAP) )
            pass = (item->m_info & QD3DBatchItem::BI_MASK) ? PASS_STENCIL_DRAW : PASS_STENCIL_NOSTENCILCHECK;
        m_statemanager->beginPass(pass);
        prepareItem(item);
        m_draw_helper->drawAliasedBoundingRect(item);
        cleanupItem(item);
        m_statemanager->endPass();
    }
}

void QDirect3DPaintEnginePrivate::flushText(QD3DBatchItem *item, int)
{
    prepareItem(item);
    m_statemanager->setTexture(item->m_texture);
    m_statemanager->setBrushMode(1);
//     m_statemanager->SetRenderState(D3DRS_BLENDFACTOR, item->m_brush.color().rgba());
    m_statemanager->beginPass(m_cleartype_text ? PASS_CLEARTYPE_TEXT : PASS_TEXT);
    m_draw_helper->drawTextItem(item);
    m_statemanager->endPass();
    cleanupItem(item);
}

void QDirect3DPaintEnginePrivate::flushLines(QD3DBatchItem *item, int)
{
    m_draw_helper->drawAliasedLines(item);

    if (item->m_info & QD3DBatchItem::BI_BRECT) {
        int pass = (item->m_info & QD3DBatchItem::BI_COMPLEXBRUSH) ? PASS_STENCIL_DRAW : PASS_STENCIL_DRAW_DIRECT;
        m_statemanager->beginPass(pass);
        prepareItem(item);
        m_draw_helper->drawAliasedBoundingRect(item);
        cleanupItem(item);
        m_statemanager->endPass();
    }
}

void QDirect3DPaintEnginePrivate::flushBatch()
{
//     static int dbgcounter = 0;
//     ++dbgcounter;
//     qDebug() << " -> flush" << dbgcounter;

    int offset = 0;
    m_draw_helper->unlockVertexBuffer();
    releaseDC();

    // iterate over all items in the batch
    while (offset != m_batch.m_item_index) {
        QD3DBatchItem *item = &(m_batch.items[offset]);

        if (prepareBatch(item, offset)) {
            ++offset;
            continue;
        }

        if (item->m_info & QD3DBatchItem::BI_FASTLINE) {
            flushLines(item, offset++);
        } else if (item->m_info & QD3DBatchItem::BI_AA) {
            offset = flushAntialiased(offset);
        } else if (item->m_info & QD3DBatchItem::BI_TEXT) {
            flushText(item, offset++);
        } else {
            flushAliased(item, offset++);
        }
    }

    // reset batch
    m_batch.m_item_index = 0;

    // release doomed textures
    for (int i=0; i<qd3d_release_list.size(); ++i)
        qd3d_release_list.at(i)->Release();
    qd3d_release_list.clear();
}

QDirect3DPaintEngine::QDirect3DPaintEngine()
    : QPaintEngine(*(new QDirect3DPaintEnginePrivate),
                   PaintEngineFeatures(AllFeatures & ~ObjectBoundingModeGradients))
{ }

QDirect3DPaintEngine::~QDirect3DPaintEngine()
{
}

bool QDirect3DPaintEngine::begin(QPaintDevice *device)
{
#ifdef QT_DEBUG_D3D_CALLS
    qDebug() << "QDirect3DPaintEngine::begin";
#endif
    Q_D(QDirect3DPaintEngine);
    setActive(true);

    QSize old_size = d->m_surface_size;
    d->m_surface_size = QRect(0, 0, device->width(), device->height()).size();

    d->m_current_state = 0;
    d->m_inv_scale = 1;
    d->m_opacity = 1.0f;
    d->m_opacity_color = D3DCOLOR_ARGB(255,255,255,255);
    d->m_matrix = QTransform();
    d->m_brush_origin = QTransform();
    d->m_txop = QTransform::TxNone;
    d->m_cmode = QPainter::CompositionMode_SourceOver;

    Q_ASSERT(device && device->devType() == QInternal::Widget);
    if (d->m_d3d_device == 0) {
        qWarning() << "QDirect3DPaintEngine: No Device!";
        return false;
    }

    d->m_cleartype_text = false;
//     QT_WA({
//         UINT result;
//         BOOL ok;
//         ok = SystemParametersInfoW(SPI_GETFONTSMOOTHINGTYPE, 0, &result, 0);
//         if (ok)
//             d->m_cleartype_text = (result == FE_FONTSMOOTHINGCLEARTYPE);
//     }, {
//         UINT result;
//         BOOL ok;
//         ok = SystemParametersInfoA(SPI_GETFONTSMOOTHINGTYPE, 0, &result, 0);
//         if (ok)
//             d->m_cleartype_text = (result == FE_FONTSMOOTHINGCLEARTYPE);
//     });

    d->m_surface_manager.setPaintDevice(device);
    int status = d->m_surface_manager.status();
    if (status & QD3DSurfaceManager::NeedsResetting) {
        d->m_effect->OnLostDevice();
        d->m_draw_helper->beforeReset();
        d->m_statemanager->reset();
        d->m_surface_manager.reset();
        d->m_draw_helper->afterReset();
        d->m_effect->OnResetDevice();
        d->initDevice();
    }

    LPDIRECT3DSURFACE9 newsurface = d->m_surface_manager.renderTarget();
    if (d->m_current_surface != newsurface) {
        d->m_current_surface = newsurface;
        if (FAILED(d->m_d3d_device->SetRenderTarget(0, newsurface)))
            qWarning() << "QDirect3DPaintEngine: SetRenderTarget failed!";
    }

    status = d->m_surface_manager.status();
    if (status & QD3DSurfaceManager::MaxSizeChanged) {
        QSize maxsize = d->m_surface_manager.maxSize();
        d->m_draw_helper->setMaskSize(maxsize);
        int masksize[2] = {maxsize.width(), maxsize.height()};
        d->m_effect->SetIntArray("g_mMaskSize", masksize, 2);
    }

    if (old_size != d->m_surface_size) {
        D3DXMATRIX projMatrix;
        pD3DXMatrixOrthoOffCenterLH(&projMatrix, 0, d->m_surface_size.width(), d->m_surface_size.height(), 0, 0.0f, 1.0f);
        d->m_statemanager->setProjection(&projMatrix);
    }

    if (!d->m_in_scene) {
        if (FAILED(d->m_d3d_device->BeginScene())) {
            qWarning() << "QDirect3DPaintEngine: BeginScene() failed.";
            return false;
        }
        QWidget *widget = static_cast<QWidget *>(device);
        if (widget->autoFillBackground() == true) {
            QColor color = widget->palette().brush(widget->backgroundRole()).color();
            RECT rect = {0, 0, widget->width(), widget->height()};
            d->m_d3d_device->ColorFill(d->m_current_surface, &rect,
                D3DCOLOR_ARGB(color.alpha(), color.red(), color.green(), color.blue()));
        }
        d->m_in_scene = true;
    }

    // set system clip
    d->m_clipping_enabled = false;
    d->m_has_complex_clipping = false;

    d->m_sysclip_region = systemClip();
    QVector<QRect> rects = d->m_sysclip_region.rects();
    if (rects.count() == 1 && rects.at(0).size() == d->m_surface_size)
        d->m_sysclip_region = QRegion();

    d->updateClipRegion(QRegion(), Qt::NoClip);

    return true;
}

void QDirect3DPaintEngine::drawEllipse(const QRectF &rect)
{
#ifdef QT_DEBUG_D3D_CALLS
    qDebug() << "QDirect3DPaintEngine::drawEllipse (float)";
#endif
    QPaintEngine::drawEllipse(rect);
}

void QDirect3DPaintEngine::drawEllipse(const QRect &rect)
{
#ifdef QT_DEBUG_D3D_CALLS
    qDebug() << "QDirect3DPaintEngine::drawEllipse";
#endif
    QPaintEngine::drawEllipse(rect);
}

void QDirect3DPaintEngine::drawImage(const QRectF &r, const QImage &image, const QRectF &sr,
                                     Qt::ImageConversionFlags)
{
#ifdef QT_DEBUG_D3D_CALLS
    qDebug() << "QDirect3DPaintEngine::drawImage";
#endif

    Q_D(QDirect3DPaintEngine);
    int width = image.width();
    int height = image.height();

    // transform rectangle
    QPolygonF txrect(QRectF(sr.left() / width, sr.top() / height,
        sr.width() / width, sr.height() / height));

    QD3DBatchItem *item = d->nextBatchItem();
    item->m_info = QD3DBatchItem::BI_IMAGE | QD3DBatchItem::BI_TRANSFORM;
    item->m_texture = qd3d_image_cache()->lookup(d->m_d3d_device, image);
    item->m_matrix = d->m_matrix;
    d->m_draw_helper->queueRect(r.adjusted(-0.5f,-0.5f,-0.5f,-0.5f), item, d->m_opacity_color, txrect);
}

void QDirect3DPaintEngine::drawLines(const QLineF *lines, int lineCount)
{
#ifdef QT_DEBUG_D3D_CALLS
    qDebug() << "QDirect3DPaintEngine::drawLines (float)";
#endif
    Q_D(QDirect3DPaintEngine);

    if (!d->m_has_pen)
        return;

    if (d->m_has_fast_pen && (d->m_pen_brush_style == Qt::SolidPattern)) {
        QD3DBatchItem *item = d->nextBatchItem();
        if (d->m_pen.isCosmetic())
            item->m_info |= QD3DBatchItem::BI_COSMETICPEN;
        item->m_info |= QD3DBatchItem::BI_TRANSFORM;
        item->m_matrix = d->m_matrix;
        d->m_draw_helper->queueAliasedLines(lines, lineCount, &item);
    } else {
        QRectF brect;
        QPainterPath path;

        // creates a path with the lines
        path.moveTo(lines[0].x1(), lines[0].y1());
        qreal lastx = lines[0].x2();
        qreal lasty = lines[0].y2();
        path.lineTo(lastx, lasty);

        for (int i=1; i<lineCount; ++i) {
            qreal x = lines[i].x1();
            qreal y = lines[i].y1();
            if (lastx != x || lasty != y) {
                path.moveTo(x, y);
            }
            path.lineTo(lines[i].x2(), lines[i].y2());
        }

        if (d->m_has_cosmetic_pen) {
            brect = path.controlPointRect();
            path = d->m_matrix.map(path);
        }

        d->strokePath(path, brect, true);
    }
}

void QDirect3DPaintEngine::drawLines(const QLine *lines, int lineCount)
{
#ifdef QT_DEBUG_D3D_CALLS
    qDebug() << "QDirect3DPaintEngine::drawLines";
#endif
    QPaintEngine::drawLines(lines, lineCount);
}

void QDirect3DPaintEngine::drawPath(const QPainterPath &path)
{
#ifdef QT_DEBUG_D3D_CALLS
    qDebug() << "QDirect3DPaintEngine::drawPath";
#endif
    Q_D(QDirect3DPaintEngine);

    if (path.isEmpty())
        return;

    QRectF brect;
    QPainterPath tpath;

    if (d->m_has_cosmetic_pen) {
        brect = path.controlPointRect();
        tpath = d->m_matrix.map(path);
    } else {
        tpath = path;
    }

    if (d->m_has_brush)
        d->fillPath(tpath, brect);

    if (d->m_has_pen)
        d->strokePath(tpath, brect);
}


QPointF QDirect3DPaintEnginePrivate::transformPoint(const QPointF &p, qreal *w) const
{
    (*w) = 1.0f;
    qreal fx = p.x();
    qreal fy = p.y();
    qreal nx = m_matrix.m11()*fx + m_matrix.m21()*fy + m_matrix.m31();
    qreal ny = m_matrix.m12()*fx + m_matrix.m22()*fy + m_matrix.m32();
    if (!m_matrix.isAffine()) {
        *w = m_matrix.m13()*fx + m_matrix.m23()*fy + m_matrix.m33();
        //*w = 1/(*w);
        nx = nx/(*w);
        ny = ny/(*w);
    }
    return QPointF(nx, ny);
}

void QDirect3DPaintEngine::drawPixmap(const QRectF &r, const QPixmap &pm, const QRectF &sr)
{
#ifdef QT_DEBUG_D3D_CALLS
    qDebug() << "QDirect3DPaintEngine::drawPixmap";
#endif
    Q_D(QDirect3DPaintEngine);

    if (d->m_draw_helper->needsFlushing())
        d->flushBatch();

    int width = pm.width();
    int height = pm.height();

    // transform rectangle
    QPolygonF txrect(QRectF(sr.left() / width, sr.top() / height,
        sr.width() / width, sr.height() / height));

    QD3DBatchItem *item = d->nextBatchItem();
    item->m_info = QD3DBatchItem::BI_PIXMAP|QD3DBatchItem::BI_TRANSFORM;

    item->m_pixmap = pm;
    d->verifyTexture(item->m_pixmap);

    item->m_matrix = d->m_matrix;
    d->m_draw_helper->queueRect(r.adjusted(-0.5f,-0.5f,-0.5f,-0.5f), item, d->m_opacity_color, txrect);
}

void QDirect3DPaintEngine::drawPoints(const QPointF *points, int pointCount)
{
#ifdef QT_DEBUG_D3D_CALLS
    qDebug() << "QDirect3DPaintEngine::drawPoints (float)";
#endif
    QPaintEngine::drawPoints(points, pointCount);
}

void QDirect3DPaintEngine::drawPoints(const QPoint *points, int pointCount)
{
#ifdef QT_DEBUG_D3D_CALLS
    qDebug() << "QDirect3DPaintEngine::drawPoints";
#endif
    QPaintEngine::drawPoints(points, pointCount);
}

void QDirect3DPaintEngine::drawPolygon(const QPointF *points, int pointCount, PolygonDrawMode mode)
{
#ifdef QT_DEBUG_D3D_CALLS
    qDebug() << "QDirect3DPaintEngine::drawPolygon";
#endif
    Q_D(QDirect3DPaintEngine);

    if (d->m_has_brush && mode != PolylineMode) {
        QPainterPath path;
        path.setFillRule(mode == WindingMode ? Qt::WindingFill : Qt::OddEvenFill);
        path.moveTo(points[0]);
        for (int i=1; i<pointCount; ++i)
            path.lineTo(points[i]);
        if (path.isEmpty())
            return;
        d->fillPath(path, QRectF());
    }

    if (d->m_has_pen) {
        QPainterPath path(points[0]);
        for (int i = 1; i < pointCount; ++i)
            path.lineTo(points[i]);
        if (mode != PolylineMode)
            path.lineTo(points[0]);

        if (path.isEmpty())
            return;
        QRectF brect;
        if (d->m_has_cosmetic_pen) {
            brect = path.controlPointRect();
            path = d->m_matrix.map(path);
        }

        d->strokePath(path, brect);
    }
}

void QDirect3DPaintEngine::drawPolygon(const QPoint *points, int pointCount, PolygonDrawMode mode)
{
#ifdef QT_DEBUG_D3D_CALLS
    qDebug() << "QDirect3DPaintEngine::drawPolygon";
#endif
    QPaintEngine::drawPolygon(points, pointCount, mode);
}

void QDirect3DPaintEngine::drawRects(const QRectF *rects, int rectCount)
{
    Q_D(QDirect3DPaintEngine);
#ifdef QT_DEBUG_D3D_CALLS
    qDebug() << "QDirect3DPaintEngine::drawRects (float)";
#endif
    for (int i=0; i<rectCount; ++i) {
        if ((d->m_brush_style == Qt::SolidPattern) &&
            (!(d->m_current_state & QD3DBatchItem::BI_AA) || d->isFastRect(rects[i]))) {
            QD3DBatchItem *item = d->nextBatchItem();
            item->m_info |= QD3DBatchItem::BI_TRANSFORM;
            item->m_info &= ~QD3DBatchItem::BI_AA;
            item->m_matrix = d->m_matrix;
            const QRectF rect = rects[i];
            d->m_draw_helper->queueRect(rect, item, d->m_brush_color);

            if (d->m_has_pen) {
                if (d->m_has_fast_pen && (d->m_pen_brush_style == Qt::SolidPattern)) {
                    QLineF lines[4];
                    qreal x1 = rect.x();
                    qreal y1 = rect.y();
                    qreal x2 = rect.width() + x1;
                    qreal y2 = rect.height() + y1;
                    lines[0] = QLineF(x1, y1, x2, y1);
                    lines[1] = QLineF(x2, y1, x2, y2);
                    lines[2] = QLineF(x2, y2, x1, y2);
                    lines[3] = QLineF(x1, y2, x1, y1);
                    QD3DBatchItem *item = d->nextBatchItem();
                    if (d->m_pen.isCosmetic())
                        item->m_info |= QD3DBatchItem::BI_COSMETICPEN;
                    item->m_info |= QD3DBatchItem::BI_TRANSFORM;
                    item->m_matrix = d->m_matrix;
                    d->m_draw_helper->queueAliasedLines(lines, 4, &item);
                } else {
                    QPainterPath path;
                    QRectF brect;

                    path.addRect(rects[i]);
                    if (d->m_has_cosmetic_pen) {
                        brect = path.controlPointRect();
                        path = d->m_matrix.map(path);
                    }

                    d->strokePath(path, brect, true);
                }
            }
        } else {
            QPainterPath path;
            QRectF brect;

            path.addRect(rects[i]);
            if (d->m_has_cosmetic_pen) {
                brect = path.controlPointRect();
                path = d->m_matrix.map(path);
            }

            if (d->m_has_brush)
                d->fillPath(path, brect);

            if (d->m_has_pen)
                d->strokePath(path, brect, true);
        }
    }
}

void QDirect3DPaintEngine::drawRects(const QRect *rects, int rectCount)
{
#ifdef QT_DEBUG_D3D_CALLS
    qDebug() << "QDirect3DPaintEngine::drawRects";
#endif
    QPaintEngine::drawRects(rects, rectCount);
}


void QDirect3DPaintEngine::drawTextItem(const QPointF &p, const QTextItem &textItem)
{
    Q_D(QDirect3DPaintEngine);

#ifdef QT_DEBUG_D3D_CALLS
    qDebug() << "QDirect3DPaintEngine::drawTextItem";
#endif
//     if (d->m_matrix.isScaling() || (d->m_pen_brush_style >= Qt::LinearGradientPattern
//                                     && d->m_pen_brush_style <= Qt::ConicalGradientPattern)) {
//         QPaintEngine::drawTextItem(p, textItem);
//         return;
//     }

    const QTextItemInt &ti = static_cast<const QTextItemInt &>(textItem);
    QVarLengthArray<QFixedPoint> positions;
    QVarLengthArray<glyph_t> glyphs;
    QTransform matrix;
    matrix.translate(p.x(), p.y());
    ti.fontEngine->getGlyphPositions(ti.glyphs, matrix, ti.flags, glyphs, positions);

    qd3d_glyph_cache()->cacheGlyphs(this, ti, glyphs, d->m_cleartype_text);
    QD3DFontTexture *font_tex = qd3d_glyph_cache()->fontTexture(ti.fontEngine);

    QD3DBatchItem *item = d->nextBatchItem();
    d->m_draw_helper->lockVertexBuffer();

    item->m_info = QD3DBatchItem::BI_TEXT
        | (d->m_current_state & ~QD3DBatchItem::BI_AA) | QD3DBatchItem::BI_TRANSFORM;
    item->m_texture = font_tex->texture;
    item->m_offset = d->m_draw_helper->index();
    item->m_matrix = d->m_matrix;
    item->m_count = 0;
    item->m_brush = d->m_pen.brush();

    for (int i=0; i< glyphs.size(); ++i) {
        QD3DGlyphCoord *g = qd3d_glyph_cache()->lookup(ti.fontEngine, glyphs[i]);

        // we don't cache glyphs with no width/height
        if (!g)
            continue;

        // texture coords
        qreal tex_coords[] = { g->x, g->y, g->x + g->width, g->y + g->height };
        QPointF logical_pos(qRound((positions[i].x - g->x_offset).toReal()) - 0.5f,
                            qRound((positions[i].y + g->y_offset).toReal()) - 0.5f);

        QRectF glyph_rect(logical_pos, QSizeF(g->log_width, g->log_height));
        d->m_draw_helper->queueTextGlyph(glyph_rect, tex_coords, item, d->m_pen_color);
    }
}

void QDirect3DPaintEngine::drawTiledPixmap(const QRectF &rect, const QPixmap &pixmap, const QPointF &p)
{
#ifdef QT_DEBUG_D3D_CALLS
    qDebug() << "QDirect3DPaintEngine::drawTiledPixmap";
#endif
    QPaintEngine::drawTiledPixmap(rect, pixmap, p);
}

bool QDirect3DPaintEngine::end()
{
    Q_D(QDirect3DPaintEngine);

    d->flushBatch();

    if (d->m_flush_on_end) {
        QPaintDevice *pdev = paintDevice();
        LPDIRECT3DSWAPCHAIN9 swapchain = swapChain(pdev);


        QWidget *w = 0;
        if (pdev->devType() == QInternal::Widget) {
            w = static_cast<QWidget *>(pdev);
        }

        if (w && swapchain) {
            QRect br = w->rect();
            QRect wbr = br;//.translated(-w->pos());

            RECT destrect;
            destrect.left   = wbr.x();
            destrect.top    = wbr.y();
            destrect.right  = destrect.left + wbr.width();
            destrect.bottom = destrect.top  + wbr.height();

            RECT srcrect;
            srcrect.left    = br.x();// + w->x();
            srcrect.top     = br.y();// + w->y();
            srcrect.right   = wbr.width() + srcrect.left;
            srcrect.bottom  = wbr.height() + srcrect.top;
            int devwidth = w->width();
            int devheight = w->height();

            if (devwidth <= srcrect.right) {
                int diff = srcrect.right - devwidth;
                srcrect.right -= diff;
                destrect.right -= diff;
                if (srcrect.right <= srcrect.left)
                    return false;
            }
            if (devheight <= srcrect.bottom) {
                int diff = srcrect.bottom - devheight;
                srcrect.bottom -= diff;
                destrect.bottom -= diff;
                if (srcrect.bottom <= srcrect.top)
                    return false;
            }

            if (FAILED(swapchain->Present(&srcrect, &destrect, w->winId(), 0, 0)))
                qWarning("QDirect3DPaintEngine: failed to present back buffer.");
        }
    }


    return true;
}

void QDirect3DPaintEngine::updateState(const QPaintEngineState &state)
{
#ifdef QT_DEBUG_D3D_CALLS
    qDebug() << "QDirect3DPaintEngine::updateState";
#endif
    Q_D(QDirect3DPaintEngine);

    bool update_fast_pen = false;
    DirtyFlags flags = state.state();

    if (flags & DirtyOpacity) {
        d->m_opacity = state.opacity();
        if (d->m_opacity > 1.0f)
            d->m_opacity = 1.0f;
        if (d->m_opacity < 0.f)
            d->m_opacity = 0.f;
        uint c = (d->m_opacity * 255);
        d->m_opacity_color = D3DCOLOR_ARGB(c,c,c,c);
        flags |= (DirtyPen | DirtyBrush);
    }

    if (flags & DirtyCompositionMode) {
        d->m_cmode = state.compositionMode();
    }

    if (flags & DirtyTransform) {
        d->updateTransform(state.transform());
        update_fast_pen = true;
    }

    if (flags & DirtyHints) {
        if (state.renderHints() & QPainter::Antialiasing)
            d->m_current_state |= QD3DBatchItem::BI_AA;
        else
            d->m_current_state &= ~QD3DBatchItem::BI_AA;
        update_fast_pen = true;
    }

    if (flags & DirtyFont) {
        d->updateFont(state.font());
    }

    if (state.state() & DirtyClipEnabled) {
        if (state.isClipEnabled() && !d->m_clipping_enabled) {
            d->m_clipping_enabled = true;
            if (d->m_has_complex_clipping)
                d->updateClipPath(painter()->clipPath(), Qt::ReplaceClip);
            else
                d->updateClipRegion(painter()->clipRegion(), Qt::ReplaceClip);
        } else if (!state.isClipEnabled() && d->m_clipping_enabled) {
            d->m_clipping_enabled = false;
            if (d->m_has_complex_clipping)
                d->updateClipPath(QPainterPath(), Qt::NoClip);
            else
                d->updateClipRegion(QRegion(), Qt::NoClip);
        }
    }

    if (flags & DirtyClipRegion) {
        d->updateClipRegion(state.clipRegion(), state.clipOperation());
    }

    if (flags & DirtyClipPath) {
        d->updateClipPath(state.clipPath(), state.clipOperation());
    }

    if (flags & DirtyBrushOrigin) {
        d->m_brush_origin = QTransform();
        d->m_brush_origin.translate(-state.brushOrigin().x(),
            -state.brushOrigin().y());
        flags |= DirtyBrush;
    }

    if (flags & DirtyPen) {
        d->updatePen(state.pen());
        update_fast_pen = true;
    }

    if (flags & DirtyBrush)
        d->updateBrush(state.brush());

    if (update_fast_pen && d->m_has_pen) {
        if (d->m_current_state & QD3DBatchItem::BI_AA) {
            d->m_has_fast_pen = false;
            d->m_has_aa_fast_pen = ((d->m_txop <= QTransform::TxTranslate) || d->m_has_cosmetic_pen)
                && (d->m_pen_width <= 1.0f)
                && (d->m_pen.style() == Qt::SolidLine);
        } else {
            d->m_has_aa_fast_pen = false;
            d->m_has_fast_pen = ((d->m_txop <= QTransform::TxTranslate) || d->m_has_cosmetic_pen)
                              && (d->m_pen.style() == Qt::SolidLine)
                              && (d->m_pen.capStyle() == Qt::SquareCap);
        }
    }
}

void QDirect3DPaintEngine::cleanup()
{
    Q_D(QDirect3DPaintEngine);
    d->cleanup();
}

void QDirect3DPaintEngine::scroll(QPaintDevice *pd, const RECT &srcrect, const RECT &destrect)
{
    Q_D(QDirect3DPaintEngine);
    LPDIRECT3DSURFACE9 srcsurf = d->m_surface_manager.surface(pd);
    LPDIRECT3DSURFACE9 masksurf = d->m_draw_helper->freeMaskSurface();
    if (FAILED(d->m_d3d_device->StretchRect(srcsurf, &srcrect, masksurf, &srcrect, D3DTEXF_NONE)))
        qWarning("QDirect3DPaintEngine: StretchRect failed.");
    if (FAILED(d->m_d3d_device->StretchRect(masksurf, &srcrect, srcsurf, &destrect, D3DTEXF_NONE)))
        qWarning("QDirect3DPaintEngine: StretchRect failed.");
}

LPDIRECT3DSWAPCHAIN9 QDirect3DPaintEngine::swapChain(QPaintDevice *pd)
{
    Q_D(QDirect3DPaintEngine);

    if (d->m_in_scene) {
        if (d->m_d3d_device == 0) {
            qWarning("QDirect3DPaintEngine: No device!");
            return false;
        }

        d->setRenderTechnique(QDirect3DPaintEnginePrivate::RT_NoTechnique);
        if (FAILED(d->m_d3d_device->EndScene()))
            qWarning("QDirect3DPaintEngine: failed to end scene.");

        d->m_in_scene = false;
    }

    return d->m_surface_manager.swapChain(pd);
}

void QDirect3DPaintEngine::releaseSwapChain(QPaintDevice *pd)
{
    Q_D(QDirect3DPaintEngine);
    d->m_surface_manager.releasePaintDevice(pd);
}

HDC QDirect3DPaintEngine::getDC() const
{
    QDirect3DPaintEnginePrivate *d = const_cast<QDirect3DPaintEnginePrivate *>(d_func());

    if (!d->m_dc && d->m_current_surface) {
        d->m_dcsurface = d->m_current_surface;
        if (FAILED(d->m_current_surface->GetDC(&d->m_dc)))
            qWarning() << "QDirect3DPaintEngine::getDC() failed!";
    }

    return d->m_dc;
}

void QDirect3DPaintEngine::setFlushOnEnd(bool flushOnEnd)
{
    Q_D(QDirect3DPaintEngine);

    d->m_flush_on_end = flushOnEnd;
}

bool QDirect3DPaintEngine::hasDirect3DSupport()
{
    Q_D(QDirect3DPaintEngine);
    return d->m_supports_d3d;
}

QT_END_NAMESPACE

#include "qpaintengine_d3d.moc"
