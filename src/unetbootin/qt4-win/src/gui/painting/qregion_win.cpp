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

#include "qbitmap.h"
#include "qbuffer.h"
#include "qimage.h"
#include "qpolygon.h"
#include "qregion.h"
#include "qt_windows.h"

QT_BEGIN_NAMESPACE


/*
    In Windows versions before Windows Vista CreateRectRgn - when called in a multi-threaded
    environment - might return an invalid handle. This function works around this limitation
    by verifying the handle with a quick GetRegionData() call and re-creates the region
    if necessary.
*/
HRGN qt_tryCreateRegion(QRegion::RegionType type, int left, int top, int right, int bottom)
{
    const int tries = 10;
    for (int i = 0; i < tries; ++i) {
        HRGN region;
        switch (type) {
        case QRegion::Rectangle:
            region = CreateRectRgn(left, top, right, bottom);
            break;
        case QRegion::Ellipse:
#ifndef Q_OS_WINCE
            region = CreateEllipticRgn(left, top, right, bottom);
#endif
            break;
        }
        if (region) {
            if (GetRegionData(region, 0, 0))
                return region;
            else
                DeleteObject(region);
        }
    }
    return 0;
}

#ifndef Q_OS_WINCE
HRGN qt_tryCreatePolygonRegion(const QPolygon &a, Qt::FillRule fillRule)
{
    const int tries = 10;
    for (int i = 0; i < tries; ++i) {
        HRGN region = CreatePolygonRgn(reinterpret_cast<const POINT*>(a.data()), a.size(),
                                       fillRule == Qt::OddEvenFill ? ALTERNATE : WINDING);
        if (region) {
            if (GetRegionData(region, 0, 0))
                return region;
            else
                DeleteObject(region);
        }
    }
    return 0;
}
#endif

QRegion::QRegionData QRegion::shared_empty = { Q_BASIC_ATOMIC_INITIALIZER(1), 0 };

QRegion::QRegion()
    : d(&shared_empty)
{
    d->ref.ref();
}

#ifndef Q_OS_WINCE //implementation for WindowsCE in qregion_wce.cpp
QRegion::QRegion(const QRect &r, RegionType t)
{
    if (r.isEmpty()) {
        d = &shared_empty;
        d->ref.ref();
    } else {
        d = new QRegionData;
        d->ref = 1;
        if (t == Rectangle)
            d->rgn = qt_tryCreateRegion(t, r.left(), r.top(), r.x() + r.width(), r.y() + r.height());
        else if (t == Ellipse) {
            // need to add 1 to width/height for the ellipse to have correct boundingrect.
            d->rgn = qt_tryCreateRegion(t, r.x(), r.y(), r.x() + r.width() + 1, r.y() + r.height() + 1);
        }
    }
}
#endif

#ifndef Q_OS_WINCE //implementation for WindowsCE in qregion_wce.cpp
QRegion::QRegion(const QPolygon &a, Qt::FillRule fillRule)
{
    if (a.size() < 3) {
        d = &shared_empty;
        d->ref.ref();
    } else {
        d = new QRegionData;
        d->ref = 1;
        d->rgn = qt_tryCreatePolygonRegion(a, fillRule);
    }
}
#endif

QRegion::QRegion(const QRegion &r)
{
    d = r.d;
    d->ref.ref();
}

HRGN qt_win_bitmapToRegion(const QBitmap& bitmap)
{
    HRGN region=0;
    QImage image = bitmap.toImage();
    const int MAXRECT = 256;
    struct RData {
        RGNDATAHEADER header;
        RECT rect[MAXRECT];
    };
    RData data;

#define FlushSpans \
    { \
                data.header.dwSize = sizeof(RGNDATAHEADER); \
                data.header.iType = RDH_RECTANGLES; \
                data.header.nCount = n; \
                data.header.nRgnSize = 0; \
                data.header.rcBound.bottom = y; \
                HRGN r = ExtCreateRegion(0, \
                    sizeof(RGNDATAHEADER)+n*sizeof(RECT),(RGNDATA*)&data); \
                if (region) { \
                    CombineRgn(region, region, r, RGN_OR); \
                    DeleteObject(r); \
                } else { \
                    region = r; \
                } \
                data.header.rcBound.top = y; \
        }

#define AddSpan \
        { \
            data.rect[n].left=prev1; \
            data.rect[n].top=y; \
            data.rect[n].right=x-1+1; \
            data.rect[n].bottom=y+1; \
            n++; \
            if (n == MAXRECT) { \
                FlushSpans \
                n=0; \
            } \
        }

    data.header.rcBound.top = 0;
    data.header.rcBound.left = 0;
    data.header.rcBound.right = image.width()-1;
    int n = 0;

    int zero = 0x00;

    int x, y;
    for (y = 0; y < image.height(); ++y) {
        uchar *line = image.scanLine(y);
        int w = image.width();
        uchar all=zero;
        int prev1 = -1;
        for (x = 0; x < w;) {
            uchar byte = line[x/8];
            if (x > w - 8 || byte != all) {
                for (int b = 8; b > 0 && x < w; --b) {
                    if (!(byte & 0x01) == !all) {
                        // More of the same
                    } else {
                        // A change.
                        if (all != zero) {
                            AddSpan;
                            all = zero;
                        } else {
                            prev1 = x;
                            all = ~zero;
                        }
                    }
                    byte >>= 1;
                    ++x;
                }
            } else {
                x += 8;
            }
        }
        if (all != zero) {
            AddSpan;
        }
    }
    if (n) {
        FlushSpans;
    }

    if (!region) {
        // Surely there is some better way.
        region = qt_tryCreateRegion(QRegion::Rectangle, 0,0,1,1);
        CombineRgn(region, region, region, RGN_XOR);
    }
    return region;
}

#ifndef Q_OS_WINCE //implementation for WindowsCE in qregion_wce.cpp
QRegion::QRegion(const QBitmap &bm)
{
    if (bm.isNull()) {
        d = &shared_empty;
        d->ref.ref();
    } else {
        d = new QRegionData;
        d->ref = 1;
        d->rgn = qt_win_bitmapToRegion(bm);
    }
}
#endif

void QRegion::cleanUp(QRegion::QRegionData *x)
{
    if (x->rgn)
        DeleteObject(x->rgn);
    delete x;
}

QRegion::~QRegion()
{
    if (!d->ref.deref())
        cleanUp(d);
}

QRegion &QRegion::operator=(const QRegion &r)
{
    r.d->ref.ref();
    if (!d->ref.deref())
        cleanUp(d);
    d = r.d;
    return *this;
}


QRegion QRegion::copy() const
{
    QRegion r;
    QRegionData *x = new QRegionData;
    x->ref = 1;
    if (d->rgn) {
        x->rgn = qt_tryCreateRegion(QRegion::Rectangle, 0, 0, 2, 2);
        CombineRgn(x->rgn, d->rgn, 0, RGN_COPY);
    } else {
        x->rgn = 0;
    }
    if (!r.d->ref.deref())
        cleanUp(r.d);
    r.d = x;
    return r;
}

bool QRegion::isEmpty() const
{
    return d == &shared_empty || d->rgn == 0;
}


bool QRegion::contains(const QPoint &p) const
{
    return d->rgn ? PtInRegion(d->rgn, p.x(), p.y()) : false;
}

bool QRegion::contains(const QRect &r) const
{
    if (!d->rgn)
        return false;
    RECT rect;
    SetRect(&rect, r.left(), r.top(), r.right(), r.bottom());
    return RectInRegion(d->rgn, &rect);
}


void QRegion::translate(int dx, int dy)
{
    if (!d->rgn || (dx == 0 && dy == 0))
        return;
    detach();
    OffsetRgn(d->rgn, dx, dy);
}


#define RGN_NOP -1

// Duplicates of those in qregion.cpp
#define QRGN_OR               6
#define QRGN_AND              7
#define QRGN_SUB              8
#define QRGN_XOR              9

/*
  Performs the actual OR, AND, SUB and XOR operation between regions.
  Sets the resulting region handle to 0 to indicate an empty region.
*/

QRegion QRegion::winCombine(const QRegion &r, int op) const
{
    int both=RGN_NOP,
        left=RGN_NOP,
        right=RGN_NOP;
    switch (op) {
        case QRGN_OR:
            both = RGN_OR;
            left = right = RGN_COPY;
            break;
        case QRGN_AND:
            both = RGN_AND;
            break;
        case QRGN_SUB:
            both = RGN_DIFF;
            left = RGN_COPY;
            break;
        case QRGN_XOR:
            both = RGN_XOR;
            left = right = RGN_COPY;
            break;
        default:
            qWarning("QRegion: Internal error in winCombine");
    }

    int allCombineRgnResults = NULLREGION;
    QRegion result;
    result.detach();
    result.d->rgn = qt_tryCreateRegion(QRegion::Rectangle, 0, 0, 0, 0);
    if (d->rgn && r.d->rgn)
        allCombineRgnResults = CombineRgn(result.d->rgn, d->rgn, r.d->rgn, both);
    else if (d->rgn && left != RGN_NOP)
        allCombineRgnResults = CombineRgn(result.d->rgn, d->rgn, d->rgn, left);
    else if (r.d->rgn && right != RGN_NOP)
        allCombineRgnResults = CombineRgn(result.d->rgn, r.d->rgn, r.d->rgn, right);

    if (allCombineRgnResults == NULLREGION || allCombineRgnResults == ERROR)
        result = QRegion();

    //##### do not delete this. A null pointer is different from an empty region in SelectClipRgn in qpainter_win! (M)
//     if (allCombineRgnResults == NULLREGION) {
//         if (result.data->rgn)
//             DeleteObject(result.data->rgn);
//         result.data->rgn = 0;                        // empty region
//     }
    return result;
}

QRegion QRegion::unite(const QRegion &r) const
{
    if (!d->rgn)
        return r;
    if (!r.d->rgn)
        return *this;
    return winCombine(r, QRGN_OR);
}

QRegion QRegion::unite(const QRect &r) const
{
    return unite(QRegion(r));
}

QRegion QRegion::intersect(const QRegion &r) const
{
    if (!r.d->rgn || !d->rgn)
        return QRegion();
     return winCombine(r, QRGN_AND);
}

QRegion QRegion::subtract(const QRegion &r) const
{
    if (!r.d->rgn || !d->rgn)
        return *this;
    return winCombine(r, QRGN_SUB);
}

QRegion QRegion::eor(const QRegion &r) const
{
    if (!d->rgn)
        return r;
    if (!r.d->rgn)
        return *this;
    return winCombine(r, QRGN_XOR);
}


QRect QRegion::boundingRect() const
{
    if (!d->rgn)
        return QRect();
    RECT r;
    if (GetRgnBox(d->rgn, &r) == NULLREGION)
        return QRect();
    else
        return QRect(r.left, r.top, r.right - r.left, r.bottom - r.top);
}

QVector<QRect> QRegion::rects() const
{
    if (d->rgn == 0)
        return QVector<QRect>();

    int numBytes = GetRegionData(d->rgn, 0, 0);
    if (numBytes == 0)
        return QVector<QRect>();

    char *buf = new char[numBytes];
    if (buf == 0)
        return QVector<QRect>();

    RGNDATA *rd = reinterpret_cast<RGNDATA*>(buf);
    if (GetRegionData(d->rgn, numBytes, rd) == 0) {
        delete [] buf;
        return QVector<QRect>();
    }

    QVector<QRect> a(rd->rdh.nCount);
    RECT *r = reinterpret_cast<RECT*>(rd->Buffer);
    for (int i = 0; i < a.size(); ++i) {
        a[i].setCoords(r->left, r->top, r->right - 1, r->bottom - 1);
        ++r;
    }

    delete [] buf;
    return a;
}

void QRegion::setRects(const QRect *rects, int num)
{
    *this = QRegion();
    if (!rects || num == 0 || (num == 1 && rects->isEmpty()))
        return;
    for (int i = 0; i < num; ++i)
        *this |= rects[i];
}

int QRegion::numRects() const
{
    if (d->rgn == 0)
        return 0;

    const int numBytes = GetRegionData(d->rgn, 0, 0);
    if (numBytes == 0)
        return 0;

    char *buf = new char[numBytes];
    if (buf == 0)
        return 0;

    RGNDATA *rd = reinterpret_cast<RGNDATA*>(buf);
    if (GetRegionData(d->rgn, numBytes, rd) == 0) {
        delete[] buf;
        return 0;
    }

    const int n = rd->rdh.nCount;
    delete[] buf;

    return n;
}

bool QRegion::operator==(const QRegion &r) const
{
    if (d == r.d)
        return true;
    if ((d->rgn == 0) ^ (r.d->rgn == 0)) // one is empty, not both
        return false;
    return d->rgn == 0 ? true // both empty
                       : EqualRgn(d->rgn, r.d->rgn); // both non-empty
}

QRegion& QRegion::operator+=(const QRegion &r)
{
    if (!r.d->rgn)
        return *this;

    if (!d->rgn) {
        *this = r;
        return *this;
    }

    detach();

    int result;
    result = CombineRgn(d->rgn, d->rgn, r.d->rgn, RGN_OR);
    if (result == NULLREGION || result == ERROR)
        *this = QRegion();

    return *this;
}

QRegion& QRegion::operator-=(const QRegion &r)
{
    if (!r.d->rgn || !d->rgn)
        return *this;

    detach();

    int result;
    result = CombineRgn(d->rgn, d->rgn, r.d->rgn, RGN_DIFF);
    if (result == NULLREGION || result == ERROR)
        *this = QRegion();

    return *this;
}

QRegion& QRegion::operator&=(const QRegion &r)
{
    if (!d->rgn)
        return *this;

    if (!r.d->rgn) {
        *this = QRegion();
        return *this;
    }

    detach();

    int result;
    result = CombineRgn(d->rgn, d->rgn, r.d->rgn, RGN_AND);
    if (result == NULLREGION || result == ERROR)
        *this = QRegion();

    return *this;
}

bool qt_region_strictContains(const QRegion &region, const QRect &rect)
{
    Q_UNUSED(region);
    Q_UNUSED(rect);
    return false;
}

void QRegion::ensureHandle() const
{
}

QT_END_NAMESPACE
