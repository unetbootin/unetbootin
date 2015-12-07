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

#include "qfontengine_p.h"
#include "qtextengine_p.h"
#include <qglobal.h>
#include "qt_windows.h"
#include <private/qapplication_p.h>

#include <qlibrary.h>
#include <qpaintdevice.h>
#include <qpainter.h>
#include <qlibrary.h>
#include <limits.h>

#include <qendian.h>
#include <qmath.h>
#include <qthreadstorage.h>

#include <private/qunicodetables_p.h>
#include <qbitmap.h>

#include <private/qpainter_p.h>
#include <private/qpdf_p.h>
#include "qpaintengine.h"
#include "qvarlengtharray.h"
#include <private/qpaintengine_raster_p.h>

#if defined(Q_OS_WINCE)
#include "qguifunctions_wince.h"
#endif

//### mingw needed define
#ifndef TT_PRIM_CSPLINE
#define TT_PRIM_CSPLINE 3
#endif

#ifdef MAKE_TAG
#undef MAKE_TAG
#endif
// GetFontData expects the tags in little endian ;(
#define MAKE_TAG(ch1, ch2, ch3, ch4) (\
    (((quint32)(ch4)) << 24) | \
    (((quint32)(ch3)) << 16) | \
    (((quint32)(ch2)) << 8) | \
    ((quint32)(ch1)) \
   )

typedef BOOL (WINAPI *PtrGetCharWidthI)(HDC, UINT, UINT, LPWORD, LPINT);

// common DC for all fonts
    
QT_BEGIN_NAMESPACE
        
class QtHDC
{
    HDC _hdc;
public:
    QtHDC()
    {
        HDC displayDC = GetDC(0);
        _hdc = CreateCompatibleDC(displayDC);
        ReleaseDC(0, displayDC);
    }
    ~QtHDC()
    {
        if (_hdc)
            DeleteDC(_hdc);
    }
    HDC hdc() const
    {
        return _hdc;
    }
};

#ifndef QT_NO_THREAD
Q_GLOBAL_STATIC(QThreadStorage<QtHDC *>, local_shared_dc)
HDC shared_dc()
{
    QtHDC *&hdc = local_shared_dc()->localData();
    if (!hdc)
        hdc = new QtHDC;
    return hdc->hdc();
}
#else
HDC shared_dc()
{
    return 0;
}
#endif

static HFONT stock_sysfont  = 0;

static PtrGetCharWidthI ptrGetCharWidthI = 0;
static bool resolvedGetCharWidthI = false;

static void resolveGetCharWidthI()
{
    if (resolvedGetCharWidthI)
        return;
    resolvedGetCharWidthI = true;
    ptrGetCharWidthI = (PtrGetCharWidthI)QLibrary::resolve(QLatin1String("gdi32"), "GetCharWidthI");
}

// Copy a LOGFONTW struct into a LOGFONTA by converting the face name to an 8 bit value.
// This is needed when calling CreateFontIndirect on non-unicode windowses.
inline static void wa_copy_logfont(LOGFONTW *lfw, LOGFONTA *lfa)
{
    lfa->lfHeight = lfw->lfHeight;
    lfa->lfWidth = lfw->lfWidth;
    lfa->lfEscapement = lfw->lfEscapement;
    lfa->lfOrientation = lfw->lfOrientation;
    lfa->lfWeight = lfw->lfWeight;
    lfa->lfItalic = lfw->lfItalic;
    lfa->lfUnderline = lfw->lfUnderline;
    lfa->lfCharSet = lfw->lfCharSet;
    lfa->lfOutPrecision = lfw->lfOutPrecision;
    lfa->lfClipPrecision = lfw->lfClipPrecision;
    lfa->lfQuality = lfw->lfQuality;
    lfa->lfPitchAndFamily = lfw->lfPitchAndFamily;

    QString fam = QString::fromUtf16((const ushort*)lfw->lfFaceName);
    memcpy(lfa->lfFaceName, fam.toLocal8Bit().constData(), fam.length() + 1);
}

// defined in qtextengine_win.cpp
typedef void *SCRIPT_CACHE;
typedef HRESULT (WINAPI *fScriptFreeCache)(SCRIPT_CACHE *);
extern fScriptFreeCache ScriptFreeCache;

static inline quint32 getUInt(unsigned char *p)
{
    quint32 val;
    val = *p++ << 24;
    val |= *p++ << 16;
    val |= *p++ << 8;
    val |= *p;

    return val;
}

static inline quint16 getUShort(unsigned char *p)
{
    quint16 val;
    val = *p++ << 8;
    val |= *p;

    return val;
}

static inline HFONT systemFont()
{
    if (stock_sysfont == 0)
        stock_sysfont = (HFONT)GetStockObject(SYSTEM_FONT);
    return stock_sysfont;
}


// general font engine

QFixed QFontEngineWin::lineThickness() const
{
    if(lineWidth > 0)
        return lineWidth;

    return QFontEngine::lineThickness();
}

#if defined(Q_OS_WINCE)
static OUTLINETEXTMETRICW *getOutlineTextMetric(HDC hdc)
{
    int size;
    size = GetOutlineTextMetricsW(hdc, 0, 0);
    OUTLINETEXTMETRICW *otm = (OUTLINETEXTMETRICW *)malloc(size);
    GetOutlineTextMetricsW(hdc, size, otm);
    return otm;
}
#else
static OUTLINETEXTMETRICA *getOutlineTextMetric(HDC hdc)
{
    int size;
    size = GetOutlineTextMetricsA(hdc, 0, 0);
    OUTLINETEXTMETRICA *otm = (OUTLINETEXTMETRICA *)malloc(size);
    GetOutlineTextMetricsA(hdc, size, otm);
    return otm;
}
#endif

void QFontEngineWin::getCMap()
{
    QT_WA({
        ttf = (bool)(tm.w.tmPitchAndFamily & TMPF_TRUETYPE);
    } , {
        ttf = (bool)(tm.a.tmPitchAndFamily & TMPF_TRUETYPE);
    });
    HDC hdc = shared_dc();
    SelectObject(hdc, hfont);
    bool symb = false;
    if (ttf) {
        cmapTable = getSfntTable(qbswap<quint32>(MAKE_TAG('c', 'm', 'a', 'p')));
        int size = 0;
        cmap = QFontEngine::getCMap(reinterpret_cast<const uchar *>(cmapTable.constData()),
                       cmapTable.size(), &symb, &size);
    }
    if (!cmap) {
        ttf = false;
        symb = false;
    }
    symbol = symb;
    designToDevice = 1;
    _faceId.index = 0;
    if(cmap) {
#if defined(Q_OS_WINCE)
        OUTLINETEXTMETRICW *otm = getOutlineTextMetric(hdc);
#else
        OUTLINETEXTMETRICA *otm = getOutlineTextMetric(hdc);
#endif
        designToDevice = QFixed((int)otm->otmEMSquare)/int(otm->otmTextMetrics.tmHeight);
        unitsPerEm = otm->otmEMSquare;
        x_height = (int)otm->otmsXHeight;
        loadKerningPairs(designToDevice);
        _faceId.filename = (char *)otm + (int)otm->otmpFullName;
        lineWidth = otm->otmsUnderscoreSize;
        fsType = otm->otmfsType;
        free(otm);
    } else {
        unitsPerEm = tm.w.tmHeight;
    }
}


inline unsigned int getChar(const QChar *str, int &i, const int len)
{
    unsigned int uc = str[i].unicode();
    if (uc >= 0xd800 && uc < 0xdc00 && i < len-1) {
        uint low = str[i+1].unicode();
       if (low >= 0xdc00 && low < 0xe000) {
            uc = (uc - 0xd800)*0x400 + (low - 0xdc00) + 0x10000;
            ++i;
        }
    }
    return uc;
}

int QFontEngineWin::getGlyphIndexes(const QChar *str, int numChars, QGlyphLayout *glyphs, bool mirrored) const
{
    QGlyphLayout *g = glyphs;
    if (mirrored) {
        if (symbol) {
            for (int i = 0; i < numChars; ++i) {
                unsigned int uc = getChar(str, i, numChars);
                glyphs->glyph = getTrueTypeGlyphIndex(cmap, uc);
                if(!glyphs->glyph && uc < 0x100)
                    glyphs->glyph = getTrueTypeGlyphIndex(cmap, uc + 0xf000);
                glyphs++;
            }
        } else if (ttf) {
            for (int i = 0; i < numChars; ++i) {
                unsigned int uc = getChar(str, i, numChars);
                glyphs->glyph = getTrueTypeGlyphIndex(cmap, QChar::mirroredChar(uc));
                glyphs++;
            }
        } else {
            ushort first, last;
            QT_WA({
                first = tm.w.tmFirstChar;
                last = tm.w.tmLastChar;
            }, {
                first = tm.a.tmFirstChar;
                last = tm.a.tmLastChar;
            });
            for (int i = 0; i < numChars; ++i) {
                uint ucs = QChar::mirroredChar(getChar(str, i, numChars));
                if (ucs >= first && ucs <= last)
                    glyphs->glyph = ucs;
                else
                    glyphs->glyph = 0;
                glyphs++;
            }
        }
    } else {
        if (symbol) {
            for (int i = 0; i < numChars; ++i) {
                unsigned int uc = getChar(str, i, numChars);
                glyphs->glyph = getTrueTypeGlyphIndex(cmap, uc);
                if(!glyphs->glyph && uc < 0x100)
                    glyphs->glyph = getTrueTypeGlyphIndex(cmap, uc + 0xf000);
                glyphs++;
            }
        } else if (ttf) {
            for (int i = 0; i < numChars; ++i) {
                unsigned int uc = getChar(str, i, numChars);
                glyphs->glyph = getTrueTypeGlyphIndex(cmap, uc);
                glyphs++;
            }
        } else {
            ushort first, last;
            QT_WA({
                first = tm.w.tmFirstChar;
                last = tm.w.tmLastChar;
            }, {
                first = tm.a.tmFirstChar;
                last = tm.a.tmLastChar;
            });
            for (int i = 0; i < numChars; ++i) {
                uint uc = getChar(str, i, numChars);
                if (uc >= first && uc <= last)
                    glyphs->glyph = uc;
                else
                    glyphs->glyph = 0;
                glyphs++;
            }
        }
    }
    return glyphs - g;
}


QFontEngineWin::QFontEngineWin(const QString &name, HFONT _hfont, bool stockFont, LOGFONT lf)
{
    //qDebug("regular windows font engine created: font='%s', size=%d", name, lf.lfHeight);

    _name = name;

    cmap = 0;
    hfont = _hfont;
    logfont = lf;
    HDC hdc = shared_dc();
    SelectObject(hdc, hfont);
    this->stockFont = stockFont;
    fontDef.pixelSize = -lf.lfHeight;

    lbearing = SHRT_MIN;
    rbearing = SHRT_MIN;
    synthesized_flags = -1;
    lineWidth = -1;
    x_height = -1;

    BOOL res;
    QT_WA({
        res = GetTextMetricsW(hdc, &tm.w);
    } , {
        res = GetTextMetricsA(hdc, &tm.a);
    });
    fontDef.fixedPitch = !(tm.w.tmPitchAndFamily & TMPF_FIXED_PITCH);
    if (!res)
        qErrnoWarning("QFontEngineWin: GetTextMetrics failed");

    cache_cost = tm.w.tmHeight * tm.w.tmAveCharWidth * 2000;
    getCMap();

    useTextOutA = false;
#ifndef Q_OS_WINCE
    // TextOutW doesn't work for symbol fonts on Windows 95!
    // since we're using glyph indices we don't care for ttfs about this!
    if (QSysInfo::WindowsVersion == QSysInfo::WV_95 && !ttf &&
         (_name == QLatin1String("Marlett") || _name == QLatin1String("Symbol") ||
           _name == QLatin1String("Webdings") || _name == QLatin1String("Wingdings")))
            useTextOutA = true;
#endif
    widthCache = 0;
    widthCacheSize = 0;
    designAdvances = 0;
    designAdvancesSize = 0;

    if (!resolvedGetCharWidthI)
        resolveGetCharWidthI();
}

QFontEngineWin::~QFontEngineWin()
{
    if (designAdvances)
        free(designAdvances);

    if (widthCache)
        free(widthCache);

    // make sure we aren't by accident still selected
    SelectObject(shared_dc(), systemFont());

    if (!stockFont) {
        if (!DeleteObject(hfont))
            qErrnoWarning("QFontEngineWin: failed to delete non-stock font...");
    }
}

HGDIOBJ QFontEngineWin::selectDesignFont(QFixed *overhang) const
{
    LOGFONT f = logfont;
    f.lfHeight = unitsPerEm;
    HFONT designFont;
    QT_WA({
        designFont = CreateFontIndirectW(&f);
    }, {
        LOGFONTA fa;
        wa_copy_logfont(&f, &fa);
        designFont = CreateFontIndirectA(&fa);
    });
    HGDIOBJ oldFont = SelectObject(shared_dc(), designFont);

    if (QSysInfo::WindowsVersion & QSysInfo::WV_DOS_based) {
        BOOL res;
        QT_WA({
            TEXTMETRICW tm;
            res = GetTextMetricsW(shared_dc(), &tm);
            if (!res)
                qErrnoWarning("QFontEngineWin: GetTextMetrics failed");
            *overhang = QFixed((int)tm.tmOverhang) / designToDevice;
        } , {
            TEXTMETRICA tm;
            res = GetTextMetricsA(shared_dc(), &tm);
            if (!res)
                qErrnoWarning("QFontEngineWin: GetTextMetrics failed");
            *overhang = QFixed((int)tm.tmOverhang) / designToDevice;
        });
    } else {
        *overhang = 0;
    }
    return oldFont;
}

bool QFontEngineWin::stringToCMap(const QChar *str, int len, QGlyphLayout *glyphs, int *nglyphs, QTextEngine::ShaperFlags flags) const
{
    if (*nglyphs < len) {
        *nglyphs = len;
        return false;
    }

    *nglyphs = getGlyphIndexes(str, len, glyphs, flags & QTextEngine::RightToLeft);

    if (flags & QTextEngine::GlyphIndicesOnly)
        return true;

#if defined(Q_OS_WINCE)
    HDC hdc = shared_dc();
    if (flags & QTextEngine::DesignMetrics) {
        HGDIOBJ oldFont = 0;
        QFixed overhang = 0;

        int glyph_pos = 0;
        for(register int i = 0; i < len; i++) {
            bool surrogate = (str[i].unicode() >= 0xd800 && str[i].unicode() < 0xdc00 && i < len-1
                              && str[i+1].unicode() >= 0xdc00 && str[i+1].unicode() < 0xe000);
            unsigned int glyph = glyphs[glyph_pos].glyph;
            if(int(glyph) >= designAdvancesSize) {
                int newSize = (glyph + 256) >> 8 << 8;
                designAdvances = (QFixed *)realloc(designAdvances, newSize*sizeof(QFixed));
                for(int i = designAdvancesSize; i < newSize; ++i)
                    designAdvances[i] = -1000000;
                designAdvancesSize = newSize;
            }
            if(designAdvances[glyph] < -999999) {
                if(!oldFont)
                    oldFont = selectDesignFont(&overhang);
                SIZE size = {0, 0};
                GetTextExtentPoint32W(hdc, (wchar_t *)(str+i), surrogate ? 2 : 1, &size);
                designAdvances[glyph] = QFixed((int)size.cx)/designToDevice;
            }
            glyphs[glyph_pos].advance.x = designAdvances[glyph];
            glyphs[glyph_pos].advance.y = 0;
            if (surrogate)
                ++i;
            ++glyph_pos;
        }
        if(oldFont)
            DeleteObject(SelectObject(hdc, oldFont));
    } else {
        int overhang = (QSysInfo::WindowsVersion & QSysInfo::WV_DOS_based) ? tm.a.tmOverhang : 0;
        int glyph_pos = 0;
        HGDIOBJ oldFont = 0;

        for(register int i = 0; i < len; i++) {
            bool surrogate = (str[i].unicode() >= 0xd800 && str[i].unicode() < 0xdc00 && i < len-1
                              && str[i+1].unicode() >= 0xdc00 && str[i+1].unicode() < 0xe000);
            unsigned int glyph = glyphs[glyph_pos].glyph;

            glyphs[glyph_pos].advance.y = 0;

            if (glyph >= widthCacheSize) {
                int newSize = (glyph + 256) >> 8 << 8;
                widthCache = (unsigned char *)realloc(widthCache, newSize*sizeof(QFixed));
                memset(widthCache + widthCacheSize, 0, newSize - widthCacheSize);
                widthCacheSize = newSize;
            }
            glyphs[glyph_pos].advance.x = widthCache[glyph];
            // font-width cache failed
            if (glyphs[glyph_pos].advance.x == 0) {
                SIZE size = {0, 0};
                if (!oldFont)
                    oldFont = SelectObject(hdc, hfont);
                GetTextExtentPoint32W(hdc, (wchar_t *)str + i, surrogate ? 2 : 1, &size);
                size.cx -= overhang;
                glyphs[glyph_pos].advance.x = size.cx;
                // if glyph's within cache range, store it for later
                if (size.cx > 0 && size.cx < 0x100)
                    widthCache[glyph] = size.cx;
            }

            if (surrogate)
                ++i;
            ++glyph_pos;
        }

        if (oldFont)
            SelectObject(hdc, oldFont);
    }
#else
    recalcAdvances(*nglyphs, glyphs, flags);
#endif
    return true;
}

void QFontEngineWin::recalcAdvances(int len, QGlyphLayout *glyphs, QTextEngine::ShaperFlags flags) const
{
    HGDIOBJ oldFont = 0;
    HDC hdc = shared_dc();
    if (ttf && (flags & QTextEngine::DesignMetrics)) {
        QFixed overhang = 0;

        for(int i = 0; i < len; i++) {
            unsigned int glyph = glyphs[i].glyph;
            if(int(glyph) >= designAdvancesSize) {
                int newSize = (glyph + 256) >> 8 << 8;
                designAdvances = (QFixed *)realloc(designAdvances, newSize*sizeof(QFixed));
                for(int i = designAdvancesSize; i < newSize; ++i)
                    designAdvances[i] = -1000000;
                designAdvancesSize = newSize;
            }
            if(designAdvances[glyph] < -999999) {
                if(!oldFont)
                    oldFont = selectDesignFont(&overhang);

                if (ptrGetCharWidthI) {
                    int width = 0;
                    ptrGetCharWidthI(hdc, glyph, 1, 0, &width);

                    designAdvances[glyph] = QFixed(width) / designToDevice;
                } else {
                    GLYPHMETRICS gm;
                    DWORD res = GDI_ERROR;
                    MAT2 mat;
                    mat.eM11.value = mat.eM22.value = 1;
                    mat.eM11.fract = mat.eM22.fract = 0;
                    mat.eM21.value = mat.eM12.value = 0;
                    mat.eM21.fract = mat.eM12.fract = 0;
#ifndef Q_OS_WINCE
                    QT_WA({
                        res = GetGlyphOutlineW(hdc, glyph, GGO_METRICS|GGO_GLYPH_INDEX|GGO_NATIVE, &gm, 0, 0, &mat);
                    } , {
                        res = GetGlyphOutlineA(hdc, glyph, GGO_METRICS|GGO_GLYPH_INDEX|GGO_NATIVE, &gm, 0, 0, &mat);
                    });
#endif
                    if (res != GDI_ERROR) {
                        designAdvances[glyph] = QFixed(gm.gmCellIncX) / designToDevice;
                    }
                }
            }
            glyphs[i].advance.x = designAdvances[glyph];
            glyphs[i].advance.y = 0;
        }
        if(oldFont)
            DeleteObject(SelectObject(hdc, oldFont));
    } else {
        int overhang = (QSysInfo::WindowsVersion & QSysInfo::WV_DOS_based) ? tm.a.tmOverhang : 0;

        for(int i = 0; i < len; i++) {
            unsigned int glyph = glyphs[i].glyph;

            glyphs[i].advance.y = 0;

            if (glyph >= widthCacheSize) {
                int newSize = (glyph + 256) >> 8 << 8;
                widthCache = (unsigned char *)realloc(widthCache, newSize*sizeof(QFixed));
                memset(widthCache + widthCacheSize, 0, newSize - widthCacheSize);
                widthCacheSize = newSize;
            }
            glyphs[i].advance.x = widthCache[glyph];
            // font-width cache failed
            if (glyphs[i].advance.x == 0) {
                int width = 0;
                if (!oldFont)
                    oldFont = SelectObject(hdc, hfont);

                if (!ttf) {
                    QChar ch[2] = { ushort(glyph), 0 };
                    int chrLen = 1;
                    if (glyph > 0xffff) {
                        ch[0] = QChar::highSurrogate(glyph);
                        ch[1] = QChar::lowSurrogate(glyph);
                        ++chrLen;
                    }
                    SIZE size = {0, 0};
                    GetTextExtentPoint32W(hdc, (wchar_t *)ch, chrLen, &size);
                    width = size.cx;
                } else if (ptrGetCharWidthI) {
                    ptrGetCharWidthI(hdc, glyph, 1, 0, &width);

                    width -= overhang;
                } else {
                    GLYPHMETRICS gm;
                    DWORD res = GDI_ERROR;
                    MAT2 mat;
                    mat.eM11.value = mat.eM22.value = 1;
                    mat.eM11.fract = mat.eM22.fract = 0;
                    mat.eM21.value = mat.eM12.value = 0;
                    mat.eM21.fract = mat.eM12.fract = 0;
#ifndef Q_OS_WINCE
                    QT_WA({
                        res = GetGlyphOutlineW(hdc, glyph, GGO_METRICS|GGO_GLYPH_INDEX, &gm, 0, 0, &mat);
                    } , {
                        res = GetGlyphOutlineA(hdc, glyph, GGO_METRICS|GGO_GLYPH_INDEX, &gm, 0, 0, &mat);
                    });
#endif
                    if (res != GDI_ERROR) {
                        width = gm.gmCellIncX;
                    }
                }
                glyphs[i].advance.x = width;
                // if glyph's within cache range, store it for later
                if (width > 0 && width < 0x100)
                    widthCache[glyph] = width;
            }
        }

        if (oldFont)
            SelectObject(hdc, oldFont);
    }
}

glyph_metrics_t QFontEngineWin::boundingBox(const QGlyphLayout *glyphs, int numGlyphs)
{
    if (numGlyphs == 0)
        return glyph_metrics_t();

    QFixed w = 0;
    const QGlyphLayout *end = glyphs + numGlyphs;
    while(end > glyphs) {
                --end;
        w += end->effectiveAdvance();
        }

    return glyph_metrics_t(0, -tm.w.tmAscent, w, tm.w.tmHeight, w, 0);
}

#ifndef Q_OS_WINCE
typedef HRESULT (WINAPI *pGetCharABCWidthsFloat)(HDC, UINT, UINT, LPABCFLOAT);
static pGetCharABCWidthsFloat qt_GetCharABCWidthsFloat = 0;
#endif

glyph_metrics_t QFontEngineWin::boundingBox(glyph_t glyph)
{
#ifndef Q_OS_WINCE
    GLYPHMETRICS gm;

    HDC hdc = shared_dc();
    SelectObject(hdc, hfont);
    if(!ttf) {
        SIZE s = {0, 0};
        WCHAR ch = glyph;
        int width;
        int overhang = 0;
        static bool resolved = false;
        if (!resolved) {
            QLibrary lib(QLatin1String("gdi32"));
            qt_GetCharABCWidthsFloat = (pGetCharABCWidthsFloat) lib.resolve("GetCharABCWidthsFloatW");
            resolved = true;
        }
        if (QT_WA_INLINE(true, false) && qt_GetCharABCWidthsFloat) {
            ABCFLOAT abc;
            qt_GetCharABCWidthsFloat(hdc, ch, ch, &abc);
            width = qRound(abc.abcfB);
        } else {
            GetTextExtentPoint32W(hdc, &ch, 1, &s);
            overhang = (QSysInfo::WindowsVersion & QSysInfo::WV_DOS_based) ? tm.a.tmOverhang : 0;
            width = s.cx;
        }
        return glyph_metrics_t(0, -tm.a.tmAscent, width, tm.a.tmHeight, width-overhang, 0);
    } else {
        DWORD res = 0;
        MAT2 mat;
        mat.eM11.value = mat.eM22.value = 1;
        mat.eM11.fract = mat.eM22.fract = 0;
        mat.eM21.value = mat.eM12.value = 0;
        mat.eM21.fract = mat.eM12.fract = 0;
        QT_WA({
            res = GetGlyphOutlineW(hdc, glyph, GGO_METRICS|GGO_GLYPH_INDEX, &gm, 0, 0, &mat);
        } , {
            res = GetGlyphOutlineA(hdc, glyph, GGO_METRICS|GGO_GLYPH_INDEX, &gm, 0, 0, &mat);
        });
        if (res != GDI_ERROR)
            return glyph_metrics_t(gm.gmptGlyphOrigin.x, -gm.gmptGlyphOrigin.y,
                                  (int)gm.gmBlackBoxX, (int)gm.gmBlackBoxY, gm.gmCellIncX, gm.gmCellIncY);
    }
    return glyph_metrics_t();
#else
    SIZE s = {0, 0};
    WCHAR ch = glyph;
    HDC hdc = shared_dc();
    BOOL res = GetTextExtentPoint32W(hdc, &ch, 1, &s);
    Q_UNUSED(res);
    return glyph_metrics_t(0, -tm.a.tmAscent, s.cx, tm.a.tmHeight, s.cx, 0);

#endif
}

QFixed QFontEngineWin::ascent() const
{
    return tm.w.tmAscent;
}

QFixed QFontEngineWin::descent() const
{
    return tm.w.tmDescent;
}

QFixed QFontEngineWin::leading() const
{
    return tm.w.tmExternalLeading;
}


QFixed QFontEngineWin::xHeight() const
{
    if(x_height >= 0)
        return x_height;
    return QFontEngine::xHeight();
}

QFixed QFontEngineWin::averageCharWidth() const
{
    return tm.w.tmAveCharWidth;
}

qreal QFontEngineWin::maxCharWidth() const
{
    return tm.w.tmMaxCharWidth;
}

enum { max_font_count = 256 };
static const ushort char_table[] = {
        40,
        67,
        70,
        75,
        86,
        88,
        89,
        91,
        102,
        114,
        124,
        127,
        205,
        645,
        884,
        922,
        1070,
        12386,
        0
};

static const int char_table_entries = sizeof(char_table)/sizeof(ushort);


qreal QFontEngineWin::minLeftBearing() const
{
    if (lbearing == SHRT_MIN)
        minRightBearing(); // calculates both

    return lbearing;
}

qreal QFontEngineWin::minRightBearing() const
{
#ifdef Q_OS_WINCE
    if (rbearing == SHRT_MIN) {
        int ml = 0;
        int mr = 0;
        HDC hdc = shared_dc();
        SelectObject(hdc, hfont);
        if (ttf) {
            ABC *abc = 0;
            int n = QT_WA_INLINE(tm.w.tmLastChar - tm.w.tmFirstChar, tm.a.tmLastChar - tm.a.tmFirstChar);
            if (n <= max_font_count) {
                abc = new ABC[n+1];               
                GetCharABCWidths(hdc, tm.w.tmFirstChar, tm.w.tmLastChar, abc);                             
            } else {
                abc = new ABC[char_table_entries+1];
                for(int i = 0; i < char_table_entries; i++)
                    GetCharABCWidths(hdc, char_table[i], char_table[i], abc+i);
                n = char_table_entries;
            }
            ml = abc[0].abcA;
            mr = abc[0].abcC;
            for (int i = 1; i < n; i++) {
                if (abc[i].abcA + abc[i].abcB + abc[i].abcC != 0) {
                    ml = qMin(ml,abc[i].abcA);
                    mr = qMin(mr,abc[i].abcC);
                }
            }
            delete [] abc;
        } else {
            ml = 0;
            mr = -tm.a.tmOverhang;
        } 
        lbearing = ml;
        rbearing = mr;
    }

    return rbearing;
#else
    if (rbearing == SHRT_MIN) {
        int ml = 0;
        int mr = 0;
        HDC hdc = shared_dc();
        SelectObject(hdc, hfont);
        if (ttf) {
            ABC *abc = 0;
            int n = QT_WA_INLINE(tm.w.tmLastChar - tm.w.tmFirstChar, tm.a.tmLastChar - tm.a.tmFirstChar);
            if (n <= max_font_count) {
                abc = new ABC[n+1];
                QT_WA({
                    GetCharABCWidths(hdc, tm.w.tmFirstChar, tm.w.tmLastChar, abc);
                }, {
                    GetCharABCWidthsA(hdc,tm.a.tmFirstChar,tm.a.tmLastChar,abc);
                });
            } else {
                abc = new ABC[char_table_entries+1];
                QT_WA({
                    for(int i = 0; i < char_table_entries; i++)
                        GetCharABCWidths(hdc, char_table[i], char_table[i], abc+i);
                }, {
                    for(int i = 0; i < char_table_entries; i++) {
                        QByteArray w = QString(QChar(char_table[i])).toLocal8Bit();
                        if (w.length() == 1) {
                            uint ch8 = (uchar)w[0];
                            GetCharABCWidthsA(hdc, ch8, ch8, abc+i);
                        }
                    }
                });
                n = char_table_entries;
            }
            ml = abc[0].abcA;
            mr = abc[0].abcC;
            for (int i = 1; i < n; i++) {
                if (abc[i].abcA + abc[i].abcB + abc[i].abcC != 0) {
                    ml = qMin(ml,abc[i].abcA);
                    mr = qMin(mr,abc[i].abcC);
                }
            }
            delete [] abc;
        } else {
            QT_WA({
                ABCFLOAT *abc = 0;
                int n = tm.w.tmLastChar - tm.w.tmFirstChar+1;
                if (n <= max_font_count) {
                    abc = new ABCFLOAT[n];
                    GetCharABCWidthsFloat(hdc, tm.w.tmFirstChar, tm.w.tmLastChar, abc);
                } else {
                    abc = new ABCFLOAT[char_table_entries];
                    for(int i = 0; i < char_table_entries; i++)
                        GetCharABCWidthsFloat(hdc, char_table[i], char_table[i], abc+i);
                    n = char_table_entries;
                }
                float fml = abc[0].abcfA;
                float fmr = abc[0].abcfC;
                for (int i=1; i<n; i++) {
                    if (abc[i].abcfA + abc[i].abcfB + abc[i].abcfC != 0) {
                        fml = qMin(fml,abc[i].abcfA);
                        fmr = qMin(fmr,abc[i].abcfC);
                    }
                }
                ml = int(fml-0.9999);
                mr = int(fmr-0.9999);
                delete [] abc;
            } , {
                ml = 0;
                mr = -tm.a.tmOverhang;
            });
        }
        lbearing = ml;
        rbearing = mr;
    }

    return rbearing;
#endif
}


const char *QFontEngineWin::name() const
{
    return 0;
}

bool QFontEngineWin::canRender(const QChar *string,  int len)
{
    if (symbol) {
        for (int i = 0; i < len; ++i) {
            unsigned int uc = getChar(string, i, len);
            if (getTrueTypeGlyphIndex(cmap, uc) == 0) {
                if (uc < 0x100) {
                    if (getTrueTypeGlyphIndex(cmap, uc + 0xf000) == 0)
                        return false;
                } else {
                    return false;
                }
            }
        }
    } else if (ttf) {
        for (int i = 0; i < len; ++i) {
            unsigned int uc = getChar(string, i, len);
            if (getTrueTypeGlyphIndex(cmap, uc) == 0)
                return false;
        }
    } else {
        QT_WA({
            while(len--) {
                if (tm.w.tmFirstChar > string->unicode() || tm.w.tmLastChar < string->unicode())
                    return false;
            }
        }, {
            while(len--) {
                if (tm.a.tmFirstChar > string->unicode() || tm.a.tmLastChar < string->unicode())
                    return false;
            }
        });
    }
    return true;
}

QFontEngine::Type QFontEngineWin::type() const
{
    return QFontEngine::Win;
}

static inline double qt_fixed_to_double(const FIXED &p) {
    return ((p.value << 16) + p.fract) / 65536.0;
}

static inline QPointF qt_to_qpointf(const POINTFX &pt, qreal scale) {
    return QPointF(qt_fixed_to_double(pt.x) * scale, -qt_fixed_to_double(pt.y) * scale);
}

#ifndef GGO_UNHINTED
#define GGO_UNHINTED 0x0100
#endif

static void addGlyphToPath(glyph_t glyph, const QFixedPoint &position, HDC hdc,
                           QPainterPath *path, bool ttf, glyph_metrics_t *metric = 0, qreal scale = 1)
{
#if defined(Q_OS_WINCE)
    Q_UNUSED(glyph);
    Q_UNUSED(hdc);
#endif
    MAT2 mat;
    mat.eM11.value = mat.eM22.value = 1;
    mat.eM11.fract = mat.eM22.fract = 0;
    mat.eM21.value = mat.eM12.value = 0;
    mat.eM21.fract = mat.eM12.fract = 0;
    uint glyphFormat = GGO_NATIVE;

    if (ttf)
        glyphFormat |= GGO_GLYPH_INDEX;

    GLYPHMETRICS gMetric;
    memset(&gMetric, 0, sizeof(GLYPHMETRICS));
    int bufferSize = GDI_ERROR;
#if !defined(Q_OS_WINCE)
    QT_WA( {
        bufferSize = GetGlyphOutlineW(hdc, glyph, glyphFormat, &gMetric, 0, 0, &mat);
    }, {
        bufferSize = GetGlyphOutlineA(hdc, glyph, glyphFormat, &gMetric, 0, 0, &mat);
    });
#endif
    if ((DWORD)bufferSize == GDI_ERROR) {
        qErrnoWarning("QFontEngineWin::addOutlineToPath: GetGlyphOutline(1) failed");
        return;
    }

    void *dataBuffer = new char[bufferSize];
    DWORD ret = GDI_ERROR;
#if !defined(Q_OS_WINCE)
    QT_WA( {
        ret = GetGlyphOutlineW(hdc, glyph, glyphFormat, &gMetric, bufferSize,
                            dataBuffer, &mat);
    }, {
        ret = GetGlyphOutlineA(hdc, glyph, glyphFormat, &gMetric, bufferSize,
                                dataBuffer, &mat);
    } );
#endif
    if (ret == GDI_ERROR) {
        qErrnoWarning("QFontEngineWin::addOutlineToPath: GetGlyphOutline(2) failed");
        delete [](char *)dataBuffer;
        return;
    }

    if(metric) {
        // #### obey scale
        *metric = glyph_metrics_t(gMetric.gmptGlyphOrigin.x, -gMetric.gmptGlyphOrigin.y,
                                  (int)gMetric.gmBlackBoxX, (int)gMetric.gmBlackBoxY,
                                  gMetric.gmCellIncX, gMetric.gmCellIncY);
    }

    int offset = 0;
    int headerOffset = 0;
    TTPOLYGONHEADER *ttph = 0;

    QPointF oset = position.toPointF();
    while (headerOffset < bufferSize) {
        ttph = (TTPOLYGONHEADER*)((char *)dataBuffer + headerOffset);

        QPointF lastPoint(qt_to_qpointf(ttph->pfxStart, scale));
        path->moveTo(lastPoint + oset);
        offset += sizeof(TTPOLYGONHEADER);
        TTPOLYCURVE *curve;
        while (offset<int(headerOffset + ttph->cb)) {
            curve = (TTPOLYCURVE*)((char*)(dataBuffer) + offset);
            switch (curve->wType) {
            case TT_PRIM_LINE: {
                for (int i=0; i<curve->cpfx; ++i) {
                    QPointF p = qt_to_qpointf(curve->apfx[i], scale) + oset;
                    path->lineTo(p);
                }
                break;
            }
            case TT_PRIM_QSPLINE: {
                const QPainterPath::Element &elm = path->elementAt(path->elementCount()-1);
                QPointF prev(elm.x, elm.y);
                QPointF endPoint;
                for (int i=0; i<curve->cpfx - 1; ++i) {
                    QPointF p1 = qt_to_qpointf(curve->apfx[i], scale) + oset;
                    QPointF p2 = qt_to_qpointf(curve->apfx[i+1], scale) + oset;
                    if (i < curve->cpfx - 2) {
                        endPoint = QPointF((p1.x() + p2.x()) / 2, (p1.y() + p2.y()) / 2);
                    } else {
                        endPoint = p2;
                    }

                    path->quadTo(p1, endPoint);
                    prev = endPoint;
                }

                break;
            }
            case TT_PRIM_CSPLINE: {
                for (int i=0; i<curve->cpfx; ) {
                    QPointF p2 = qt_to_qpointf(curve->apfx[i++], scale) + oset;
                    QPointF p3 = qt_to_qpointf(curve->apfx[i++], scale) + oset;
                    QPointF p4 = qt_to_qpointf(curve->apfx[i++], scale) + oset;
                    path->cubicTo(p2, p3, p4);
                }
                break;
            }
            default:
                qWarning("QFontEngineWin::addOutlineToPath, unhandled switch case");
            }
            offset += sizeof(TTPOLYCURVE) + (curve->cpfx-1) * sizeof(POINTFX);
        }
        path->closeSubpath();
        headerOffset += ttph->cb;
    }
    delete [] (char*)dataBuffer;
}

void QFontEngineWin::addGlyphsToPath(glyph_t *glyphs, QFixedPoint *positions, int nglyphs,
                                     QPainterPath *path, QTextItem::RenderFlags)
{
    LOGFONT lf = logfont;
    // The sign must be negative here to make sure we match against character height instead of
    // hinted cell height. This ensures that we get linear matching, and we need this for
    // paths since we later on apply a scaling transform to the glyph outline to get the
    // font at the correct pixel size.
    lf.lfHeight = -unitsPerEm;
    lf.lfWidth = 0;
    HFONT hf;
    QT_WA({
        hf = CreateFontIndirectW(&lf);
    }, {
        LOGFONTA lfa;
        wa_copy_logfont(&lf, &lfa);
        hf = CreateFontIndirectA(&lfa);
    });
    HDC hdc = shared_dc();
    HGDIOBJ oldfont = SelectObject(hdc, hf);

    for(int i = 0; i < nglyphs; ++i)
        addGlyphToPath(glyphs[i], positions[i], hdc, path, ttf, /*metric*/0, qreal(fontDef.pixelSize) / unitsPerEm);

    DeleteObject(SelectObject(hdc, oldfont));
}

void QFontEngineWin::addOutlineToPath(qreal x, qreal y, const QGlyphLayout *glyphs, int numGlyphs,
                                      QPainterPath *path, QTextItem::RenderFlags flags)
{
#if !defined(Q_OS_WINCE)
    if(tm.w.tmPitchAndFamily & (TMPF_TRUETYPE)) {
        QFontEngine::addOutlineToPath(x, y, glyphs, numGlyphs, path, flags);
        return;
    }
#endif
    QFontEngine::addBitmapFontToPath(x, y, glyphs, numGlyphs, path, flags);
}

QFontEngine::FaceId QFontEngineWin::faceId() const
{
    return _faceId;
}

QT_BEGIN_INCLUDE_NAMESPACE
#include <qdebug.h>
QT_END_INCLUDE_NAMESPACE

int QFontEngineWin::synthesized() const
{
    if(synthesized_flags == -1) {
        synthesized_flags = 0;
        if(ttf) {
            const DWORD HEAD = MAKE_TAG('h', 'e', 'a', 'd');
            HDC hdc = shared_dc();
            SelectObject(hdc, hfont);
            uchar data[4];
            GetFontData(hdc, HEAD, 44, &data, 4);
            USHORT macStyle = getUShort(data);
            if (tm.w.tmItalic && !(macStyle & 2))
                synthesized_flags = SynthesizedItalic;
            if (fontDef.stretch != 100 && ttf)
                synthesized_flags |= SynthesizedStretch;
            if (tm.w.tmWeight >= 500 && !(macStyle & 1))
                synthesized_flags |= SynthesizedBold;
            //qDebug() << "font is" << _name <<
            //    "it=" << (macStyle & 2) << fontDef.style << "flags=" << synthesized_flags;
        }
    }
    return synthesized_flags;
}

QFixed QFontEngineWin::emSquareSize() const
{
    return unitsPerEm;
}

QFontEngine::Properties QFontEngineWin::properties() const
{

    LOGFONT lf = logfont;
    lf.lfHeight = unitsPerEm;
    HFONT hf;
    QT_WA({
        hf = CreateFontIndirectW(&lf);
    }, {
        LOGFONTA lfa;
        wa_copy_logfont(&lf, &lfa);
        hf = CreateFontIndirectA(&lfa);
    });
    HDC hdc = shared_dc();
    HGDIOBJ oldfont = SelectObject(hdc, hf);
#if defined(Q_OS_WINCE)
    OUTLINETEXTMETRICW *otm = getOutlineTextMetric(hdc);
#else
    OUTLINETEXTMETRICA *otm = getOutlineTextMetric(hdc);
#endif
    Properties p;
    p.emSquare = unitsPerEm;
    p.italicAngle = otm->otmItalicAngle;
    p.postscriptName = (char *)otm + (int)otm->otmpFamilyName;
    p.postscriptName += (char *)otm + (int)otm->otmpStyleName;
#ifndef QT_NO_PRINTER
    p.postscriptName = QPdf::stripSpecialCharacters(p.postscriptName);
#endif
    p.boundingBox = QRectF(otm->otmrcFontBox.left, -otm->otmrcFontBox.top,
                           otm->otmrcFontBox.right - otm->otmrcFontBox.left,
                           otm->otmrcFontBox.top - otm->otmrcFontBox.bottom);
    p.ascent = otm->otmAscent;
    p.descent = -otm->otmDescent;
    p.leading = (int)otm->otmLineGap;
    p.capHeight = 0;
    p.lineWidth = otm->otmsUnderscoreSize;
    free(otm);
    DeleteObject(SelectObject(hdc, oldfont));
    return p;
}

void QFontEngineWin::getUnscaledGlyph(glyph_t glyph, QPainterPath *path, glyph_metrics_t *metrics)
{
    LOGFONT lf = logfont;
    lf.lfHeight = unitsPerEm;
    int flags = synthesized();
    if(flags & SynthesizedItalic)
        lf.lfItalic = false;
    lf.lfWidth = 0;
    HFONT hf;
    QT_WA({
        hf = CreateFontIndirectW(&lf);
    }, {
        LOGFONTA lfa;
        wa_copy_logfont(&lf, &lfa);
        hf = CreateFontIndirectA(&lfa);
    });
    HDC hdc = shared_dc();
    HGDIOBJ oldfont = SelectObject(hdc, hf);
    QFixedPoint p;
    p.x = 0;
    p.y = 0;
    addGlyphToPath(glyph, p, hdc, path, ttf, metrics);
    DeleteObject(SelectObject(hdc, oldfont));
}

bool QFontEngineWin::getSfntTableData(uint tag, uchar *buffer, uint *length) const
{
    if (!ttf)
        return false;
    HDC hdc = shared_dc();
    SelectObject(hdc, hfont);
    DWORD t = qbswap<quint32>(tag);
    *length = GetFontData(hdc, t, 0, buffer, *length);
    return *length != GDI_ERROR;
}

#if !defined(CLEARTYPE_QUALITY)
#    define CLEARTYPE_QUALITY       5
#endif

QImage QFontEngineWin::alphaMapForGlyph(glyph_t glyph)
{
    glyph_metrics_t gm = boundingBox(glyph);
    int glyph_x = qFloor(gm.x.toReal());
    int glyph_y = qFloor(gm.y.toReal());
    int glyph_width = qCeil((gm.x + gm.width).toReal()) -  glyph_x;
    int glyph_height = qCeil((gm.y + gm.height).toReal()) - glyph_y;

    if (glyph_width + glyph_x <= 0 || glyph_height <= 0)
        return QImage();
    QImage im(glyph_width, glyph_height, QImage::Format_ARGB32_Premultiplied);
    im.fill(0);
    QPainter p(&im);

    HFONT oldHFont = hfont;
    if (QSysInfo::WindowsVersion >= QSysInfo::WV_XP) {
        // try hard to disable cleartype rendering
        static_cast<QRasterPaintEngine *>(p.paintEngine())->disableClearType();
        LOGFONT nonCleartypeFont = logfont;
        nonCleartypeFont.lfQuality = ANTIALIASED_QUALITY;
        hfont = CreateFontIndirect(&nonCleartypeFont);
    }
    p.setPen(Qt::black);
    p.setBrush(Qt::NoBrush);

    QTextItemInt ti;
    ti.ascent = ascent();
    ti.descent = descent();
    ti.width = glyph_width;
    ti.fontEngine = this;

    ti.num_glyphs = 1;
    QGlyphLayout glyphLayout;
    ti.glyphs = &glyphLayout;
    glyphLayout.glyph = glyph;
    memset(&glyphLayout.attributes, 0, sizeof(glyphLayout.attributes));
    glyphLayout.advance.x = glyph_width;

    p.drawTextItem(QPointF(-glyph_x, -glyph_y), ti);

    if (QSysInfo::WindowsVersion >= QSysInfo::WV_XP) {
        DeleteObject(hfont);
        hfont = oldHFont;
    }

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


// -------------------------------------- Multi font engine

QFontEngineMultiWin::QFontEngineMultiWin(QFontEngineWin *first, const QStringList &fallbacks)
        : QFontEngineMulti(fallbacks.size()+1),
          fallbacks(fallbacks)
{
    engines[0] = first;
    first->ref.ref();
    fontDef = engines[0]->fontDef;
}

void QFontEngineMultiWin::loadEngine(int at)
{
    Q_ASSERT(at < engines.size());
    Q_ASSERT(engines.at(at) == 0);

    QString fam = fallbacks.at(at-1);

    LOGFONT lf = static_cast<QFontEngineWin *>(engines.at(0))->logfont;
    HFONT hfont;
    QT_WA({
        memcpy(lf.lfFaceName, fam.utf16(), sizeof(TCHAR)*qMin(fam.length()+1,32));  // 32 = Windows hard-coded
        hfont = CreateFontIndirectW(&lf);
    } , {
        // LOGFONTA and LOGFONTW are binary compatible
        QByteArray lname = fam.toLocal8Bit();
        memcpy(lf.lfFaceName,lname.data(),
            qMin(lname.length()+1,32));  // 32 = Windows hard-coded
        hfont = CreateFontIndirectA((LOGFONTA*)&lf);
    });
    bool stockFont = false;
    if (hfont == 0) {
        hfont = (HFONT)GetStockObject(ANSI_VAR_FONT);
        stockFont = true;
    }
    engines[at] = new QFontEngineWin(fam, hfont, stockFont, lf);
    engines[at]->ref.ref();
    engines[at]->fontDef = fontDef;
}

QT_END_NAMESPACE
