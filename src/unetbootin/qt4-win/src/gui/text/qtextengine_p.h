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

#ifndef QTEXTENGINE_P_H
#define QTEXTENGINE_P_H

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

#include "QtCore/qglobal.h"
#include "QtCore/qstring.h"
#include "QtCore/qnamespace.h"
#include "QtGui/qtextlayout.h"
#include "private/qtextformat_p.h"
#include "private/qfont_p.h"
#include "QtCore/qvector.h"
#include "QtGui/qpaintengine.h"
#include "QtGui/qtextobject.h"
#include "QtGui/qtextoption.h"
#include "QtCore/qset.h"
#include "QtCore/qdebug.h"
#ifndef QT_BUILD_COMPAT_LIB
#include "private/qtextdocument_p.h"
#endif
#include "private/qharfbuzz_p.h"
#include "private/qfixed_p.h"

#include <stdlib.h>

QT_BEGIN_NAMESPACE

class QFontPrivate;
class QFontEngine;

class QString;
class QPainter;

class QAbstractTextDocumentLayout;


struct QScriptItem;
/// Internal QTextItem
class QTextItemInt : public QTextItem
{
public:
    inline QTextItemInt()
        : justified(false), underlineStyle(QTextCharFormat::NoUnderline), num_chars(0), chars(0),
          logClusters(0), f(0), glyphs(0), num_glyphs(0), fontEngine(0)
    {}
    QTextItemInt(const QScriptItem &si, QFont *font, const QTextCharFormat &format = QTextCharFormat());

    QFixed descent;
    QFixed ascent;
    QFixed width;

    RenderFlags flags;
    bool justified;
    QTextCharFormat::UnderlineStyle underlineStyle;
    const QTextCharFormat charFormat;
    int num_chars;
    const QChar *chars;
    const unsigned short *logClusters;
    const QFont *f;

    QGlyphLayout *glyphs;
    int num_glyphs;
    QFontEngine *fontEngine;
};


// this uses the same coordinate system as Qt, but a different one to freetype.
// * y is usually negative, and is equal to the ascent.
// * negative yoff means the following stuff is drawn higher up.
// the characters bounding rect is given by QRect(x,y,width,height), it's advance by
// xoo and yoff
struct glyph_metrics_t
{
    inline glyph_metrics_t()
        : x(100000),  y(100000) {}
    inline glyph_metrics_t(QFixed _x, QFixed _y, QFixed _width, QFixed _height, QFixed _xoff, QFixed _yoff)
        : x(_x),
          y(_y),
          width(_width),
          height(_height),
          xoff(_xoff),
          yoff(_yoff)
        {}
    QFixed x;
    QFixed y;
    QFixed width;
    QFixed height;
    QFixed xoff;
    QFixed yoff;
};
Q_DECLARE_TYPEINFO(glyph_metrics_t, Q_PRIMITIVE_TYPE);

struct Q_AUTOTEST_EXPORT QScriptAnalysis
{
    enum Flags {
        None = 0,
        Lowercase = 1,
        Uppercase = 2,
        SmallCaps = 3,
        Tab = 4,
        TabOrObject = Tab,
        Object = 5
    };
    unsigned short script    : 8;
    unsigned short bidiLevel : 6;  // Unicode Bidi algorithm embedding level (0-61)
    unsigned short flags     : 3;
    inline bool operator == (const QScriptAnalysis &other) const {
        return script == other.script && bidiLevel == other.bidiLevel && flags == other.flags;
    }
};
Q_DECLARE_TYPEINFO(QScriptAnalysis, Q_PRIMITIVE_TYPE);

struct QGlyphJustification
{
    inline QGlyphJustification()
        : type(0), nKashidas(0), space_18d6(0)
    {}

    enum JustificationType {
        JustifyNone,
        JustifySpace,
        JustifyKashida
    };

    uint type :2;
    uint nKashidas : 6; // more do not make sense...
    uint space_18d6 : 24;
};
Q_DECLARE_TYPEINFO(QGlyphJustification, Q_PRIMITIVE_TYPE);

struct QGlyphLayout
{
    inline QGlyphLayout()
        : glyph(0)
        {}

    HB_Glyph glyph;
    HB_GlyphAttributes attributes;
    QFixedPoint advance;
    QFixedPoint offset;
    QGlyphJustification justification;

    inline QFixed effectiveAdvance() const
    { return (advance.x + QFixed::fromFixed(justification.space_18d6)) * !attributes.dontPrint; }
};
Q_DECLARE_TYPEINFO(QGlyphLayout, Q_PRIMITIVE_TYPE);

inline bool qIsControlChar(ushort uc)
{
    return uc >= 0x200b && uc <= 0x206f
        && (uc <= 0x200f /* ZW Space, ZWNJ, ZWJ, LRM and RLM */
            || (uc >= 0x2028 && uc <= 0x202f /* LS, PS, LRE, RLE, PDF, LRO, RLO, NNBSP */)
            || uc >= 0x206a /* ISS, ASS, IAFS, AFS, NADS, NODS */);
}

struct Q_AUTOTEST_EXPORT QScriptItem
{
    inline QScriptItem()
        : position(0),
          num_glyphs(0), descent(-1), ascent(-1), width(-1),
          glyph_data_offset(0) {}
    inline QScriptItem(int p, const QScriptAnalysis &a)
        : position(p), analysis(a),
          num_glyphs(0), descent(-1), ascent(-1), width(-1),
          glyph_data_offset(0) {}

    int position;
    QScriptAnalysis analysis;
    unsigned short num_glyphs;
    QFixed descent;
    QFixed ascent;
    QFixed width;
    int glyph_data_offset;
    QFixed height() const { return ascent + descent; }
};


Q_DECLARE_TYPEINFO(QScriptItem, Q_MOVABLE_TYPE);

typedef QVector<QScriptItem> QScriptItemArray;

struct Q_AUTOTEST_EXPORT QScriptLine
{
    QScriptLine()
        : from(0), length(0),
        justified(0), gridfitted(0),
        hasTrailingSpaces(0) {}
    QFixed descent;
    QFixed ascent;
    QFixed x;
    QFixed y;
    QFixed width;
    QFixed textWidth;
    int from;
    signed int length : 29;
    mutable uint justified : 1;
    mutable uint gridfitted : 1;
    uint hasTrailingSpaces : 1;
    QFixed height() const { return ascent + descent + 1; }
    void setDefaultHeight(QTextEngine *eng);
    void operator+=(const QScriptLine &other);
};
Q_DECLARE_TYPEINFO(QScriptLine, Q_PRIMITIVE_TYPE);


inline void QScriptLine::operator+=(const QScriptLine &other)
{
    descent = qMax(descent, other.descent);
    ascent = qMax(ascent, other.ascent);
    textWidth += other.textWidth;
    length += other.length;
}

typedef QVector<QScriptLine> QScriptLineArray;

class QFontPrivate;
class QTextFormatCollection;

class Q_GUI_EXPORT QTextEngine {
public:
    struct LayoutData {
        LayoutData(const QString &str, void **stack_memory, int mem_size);
        LayoutData();
        ~LayoutData();
        mutable QScriptItemArray items;
        int allocated;
        int available_glyphs;
        void **memory;
        unsigned short *logClustersPtr;
        QGlyphLayout *glyphPtr;
        int num_glyphs;
        mutable int used;
        uint hasBidi : 1;
        uint inLayout : 1;
        uint memory_on_stack : 1;
        bool haveCharAttributes;
        QString string;
        void reallocate(int totalGlyphs);
    };

    QTextEngine(LayoutData *data);
    QTextEngine();
    QTextEngine(const QString &str, const QFont &f);
    ~QTextEngine();

    enum Mode {
        WidthOnly = 0x07
    };

    // keep in sync with QAbstractFontEngine::TextShapingFlag!!
    enum ShaperFlag {
        RightToLeft = 0x0001,
        DesignMetrics = 0x0002,
        GlyphIndicesOnly = 0x0004
    };
    Q_DECLARE_FLAGS(ShaperFlags, ShaperFlag)

    void invalidate();
    void clearLineData();

    void validate() const;
    void itemize() const;

    static void bidiReorder(int numRuns, const quint8 *levels, int *visualOrder);

    const HB_CharAttributes *attributes() const;

    void shape(int item) const;

    void justify(const QScriptLine &si);

    QFixed width(int charFrom, int numChars) const;
    glyph_metrics_t boundingBox(int from,  int len) const;
    glyph_metrics_t tightBoundingBox(int from,  int len) const;

    int length(int item) const {
        const QScriptItem &si = layoutData->items[item];
        int from = si.position;
        item++;
        return (item < layoutData->items.size() ? layoutData->items[item].position : layoutData->string.length()) - from;
    }
    int length(const QScriptItem *si) const {
        int end;
        if (si + 1 < layoutData->items.constData()+ layoutData->items.size())
            end = (si+1)->position;
        else
            end = layoutData->string.length();
        return end - si->position;
    }

    QFontEngine *fontEngine(const QScriptItem &si, QFixed *ascent = 0, QFixed *descent = 0) const;
    QFont font(const QScriptItem &si) const;
    inline QFont font() const { return fnt; }

    inline unsigned short *logClusters(const QScriptItem *si) const
        { return layoutData->logClustersPtr+si->position; }
    inline QGlyphLayout *glyphs(const QScriptItem *si) const
        { return layoutData->glyphPtr + si->glyph_data_offset; }

    inline void ensureSpace(int nGlyphs) const {
        if (layoutData->num_glyphs - layoutData->used < nGlyphs)
            layoutData->reallocate((((layoutData->used + nGlyphs)*3/2 + 15) >> 4) << 4);
    }

    void freeMemory();

    int findItem(int strPos) const;
    inline QTextFormatCollection *formats() const {
#ifdef QT_BUILD_COMPAT_LIB
        return 0; // Compat should never reference this symbol
#else
        return block.docHandle()->formatCollection();
#endif
    }
    QTextCharFormat format(const QScriptItem *si) const;
    inline QAbstractTextDocumentLayout *docLayout() const {
#ifdef QT_BUILD_COMPAT_LIB
        return 0; // Compat should never reference this symbol
#else
        return block.docHandle()->document()->documentLayout();
#endif
    }
    int formatIndex(const QScriptItem *si) const;

    /// returns the width of tab at index (in the tabs array) with the tab-start at position x
    QFixed calculateTabWidth(int index, QFixed x) const;

    mutable QScriptLineArray lines;

    QString text;
    QFont fnt;
    QTextBlock block;

    QTextOption option;

    QFixed minWidth;
    QFixed maxWidth;
    QPointF position;
    uint ignoreBidi : 1;
    uint cacheGlyphs : 1;
    uint stackEngine : 1;
    uint forceJustification : 1;

    int *underlinePositions;

    mutable LayoutData *layoutData;

    inline bool hasFormats() const { return (block.docHandle() || specialData); }

    struct SpecialData {
        int preeditPosition;
        QString preeditText;
        QList<QTextLayout::FormatRange> addFormats;
        QVector<int> addFormatIndices;
        QVector<int> resolvedFormatIndices;
    };
    SpecialData *specialData;

    bool atWordSeparator(int position) const;
    void indexAdditionalFormats();

    QString elidedText(Qt::TextElideMode mode, const QFixed &width, int flags = 0) const;

    void shapeLine(const QScriptLine &line);

private:
    void setBoundary(int strPos) const;
    void addRequiredBoundaries() const;
    void shapeText(int item) const;
    void shapeTextWithHarfbuzz(int item) const;
#if defined(Q_OS_WINCE)
    void shapeTextWithCE(int item) const;
#endif
#if defined(Q_WS_MAC)
    void shapeTextWithAtsui(int item) const;
#endif
    void splitItem(int item, int pos) const;

    void resolveAdditionalFormats() const;
};

class QStackTextEngine : public QTextEngine {
public:
    enum { MemSize = 256*40/sizeof(void *) };
    QStackTextEngine(const QString &string, const QFont &f);
    LayoutData _layoutData;
    void *_memory[MemSize];
};


Q_DECLARE_OPERATORS_FOR_FLAGS(QTextEngine::ShaperFlags)

QT_END_NAMESPACE

#endif // QTEXTENGINE_P_H
