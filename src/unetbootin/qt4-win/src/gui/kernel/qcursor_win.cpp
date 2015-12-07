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

#include <private/qcursor_p.h>
#include <qbitmap.h>
#include <qcursor.h>

#ifndef QT_NO_CURSOR

#include <qimage.h>
#include <qt_windows.h>

QT_BEGIN_NAMESPACE

extern QCursorData *qt_cursorTable[Qt::LastCursor + 1]; // qcursor.cpp

/*****************************************************************************
  Internal QCursorData class
 *****************************************************************************/

QCursorData::QCursorData(Qt::CursorShape s)
  : cshape(s), bm(0), bmm(0), hx(0), hy(0), hcurs(0)
{
    ref = 1;
}

QCursorData::~QCursorData()
{
    delete bm;
    delete bmm;
#if !defined(Q_OS_WINCE) || defined(GWES_ICONCURS)
    if (hcurs)
        DestroyCursor(hcurs);
#endif
}


QCursorData *QCursorData::setBitmap(const QBitmap &bitmap, const QBitmap &mask, int hotX, int hotY)
{
    if (!QCursorData::initialized)
        QCursorData::initialize();
    if (bitmap.depth() != 1 || mask.depth() != 1 || bitmap.size() != mask.size()) {
        qWarning("QCursor: Cannot create bitmap cursor; invalid bitmap(s)");
        QCursorData *c = qt_cursorTable[0];
        c->ref.ref();
        return c;
    }
    QCursorData *d = new QCursorData;
    d->bm  = new QBitmap(bitmap);
    d->bmm = new QBitmap(mask);
    d->hcurs = 0;
    d->cshape = Qt::BitmapCursor;
    d->hx = hotX >= 0 ? hotX : bitmap.width()/2;
    d->hy = hotY >= 0 ? hotY : bitmap.height()/2;
    return d;
}

HCURSOR QCursor::handle() const
{
    if (!QCursorData::initialized)
        QCursorData::initialize();
    if (!d->hcurs)
        d->update();
    return d->hcurs;
}

QCursor::QCursor(HCURSOR handle)
{
    d = new QCursorData(Qt::CustomCursor);
    d->hcurs = handle;
}

#endif //QT_NO_CURSOR

QPoint QCursor::pos()
{
    POINT p;
    GetCursorPos(&p);
    return QPoint(p.x, p.y);
}

void QCursor::setPos(int x, int y)
{
    SetCursorPos(x, y);
}

#ifndef QT_NO_CURSOR

extern HBITMAP qt_createIconMask(const QBitmap &bitmap);

static HCURSOR create32BitCursor(const QPixmap &pixmap, int hx, int hy)
{
    HCURSOR cur = 0;
#if !defined(Q_OS_WINCE)
    QBitmap mask = pixmap.mask();
    if (mask.isNull()) {
        mask = QBitmap(pixmap.size());
        mask.fill(Qt::color1);
    }

    HBITMAP ic = pixmap.toWinHBITMAP(QPixmap::PremultipliedAlpha);
    HBITMAP im = qt_createIconMask(mask);

    ICONINFO ii;
    ii.fIcon     = 0;
    ii.xHotspot  = hx;
    ii.yHotspot  = hy;
    ii.hbmMask   = im;
    ii.hbmColor  = ic;

    cur = CreateIconIndirect(&ii);

    DeleteObject(ic);
    DeleteObject(im);
#elif defined(GWES_ICONCURS)
    QImage bbits, mbits;
    bool invb, invm;
    bbits = pixmap.toImage().convertToFormat(QImage::Format_Mono);
    mbits = pixmap.toImage().convertToFormat(QImage::Format_Mono);
    invb = bbits.numColors() > 1 && qGray(bbits.color(0)) < qGray(bbits.color(1));
    invm = mbits.numColors() > 1 && qGray(mbits.color(0)) < qGray(mbits.color(1));

    int sysW = GetSystemMetrics(SM_CXCURSOR);
    int sysH = GetSystemMetrics(SM_CYCURSOR);
    int sysN = qMax(1, sysW / 8);
    int n = qMax(1, bbits.width() / 8);
    int h = bbits.height();

    uchar* xBits = new uchar[sysH * sysN];
    uchar* xMask = new uchar[sysH * sysN];
    int x = 0;
    for (int i = 0; i < sysH; ++i) {
        if (i >= h) {
            memset(&xBits[x] , 255, sysN);
            memset(&xMask[x] ,   0, sysN);
            x += sysN;
        } else {
            int fillWidth = n > sysN ? sysN : n;
            uchar *bits = bbits.scanLine(i);
            uchar *mask = mbits.scanLine(i);
            for (int j = 0; j < fillWidth; ++j) {
                uchar b = bits[j];
                uchar m = mask[j];
                if (invb)
                    b ^= 0xFF;
                if (invm)
                    m ^= 0xFF;
                xBits[x] = ~m;
                xMask[x] = b ^ m;
                ++x;
            }
            for (int j = fillWidth; j < sysN; ++j ) {
                xBits[x] = 255;
                xMask[x] = 0;
                ++x;
            }
        }
    }

    cur = CreateCursor(qWinAppInst(), hx, hy, sysW, sysH,
        xBits, xMask);
#else
    Q_UNUSED(pixmap);
    Q_UNUSED(hx);
    Q_UNUSED(hy);
#endif
    return cur;
}

void QCursorData::update()
{
    if (!QCursorData::initialized)
        QCursorData::initialize();
    if (hcurs)
        return;

    if (cshape == Qt::BitmapCursor && !pixmap.isNull()) {
        hcurs = create32BitCursor(pixmap, hx, hy);
        if (hcurs)
            return;
    }


    // Non-standard Windows cursors are created from bitmaps

    static const uchar vsplit_bits[] = {
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x80, 0x00, 0x00, 0x00, 0xc0, 0x01, 0x00, 0x00, 0xe0, 0x03, 0x00,
        0x00, 0x80, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00,
        0x00, 0x80, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0xff, 0x7f, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0x7f, 0x00,
        0x00, 0x80, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00,
        0x00, 0x80, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0xe0, 0x03, 0x00,
        0x00, 0xc0, 0x01, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
    static const uchar vsplitm_bits[] = {
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00,
        0x00, 0xc0, 0x01, 0x00, 0x00, 0xe0, 0x03, 0x00, 0x00, 0xf0, 0x07, 0x00,
        0x00, 0xf8, 0x0f, 0x00, 0x00, 0xc0, 0x01, 0x00, 0x00, 0xc0, 0x01, 0x00,
        0x00, 0xc0, 0x01, 0x00, 0x80, 0xff, 0xff, 0x00, 0x80, 0xff, 0xff, 0x00,
        0x80, 0xff, 0xff, 0x00, 0x80, 0xff, 0xff, 0x00, 0x80, 0xff, 0xff, 0x00,
        0x80, 0xff, 0xff, 0x00, 0x00, 0xc0, 0x01, 0x00, 0x00, 0xc0, 0x01, 0x00,
        0x00, 0xc0, 0x01, 0x00, 0x00, 0xf8, 0x0f, 0x00, 0x00, 0xf0, 0x07, 0x00,
        0x00, 0xe0, 0x03, 0x00, 0x00, 0xc0, 0x01, 0x00, 0x00, 0x80, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
    static const uchar hsplit_bits[] = {
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x02, 0x00, 0x00, 0x40, 0x02, 0x00,
        0x00, 0x40, 0x02, 0x00, 0x00, 0x40, 0x02, 0x00, 0x00, 0x40, 0x02, 0x00,
        0x00, 0x41, 0x82, 0x00, 0x80, 0x41, 0x82, 0x01, 0xc0, 0x7f, 0xfe, 0x03,
        0x80, 0x41, 0x82, 0x01, 0x00, 0x41, 0x82, 0x00, 0x00, 0x40, 0x02, 0x00,
        0x00, 0x40, 0x02, 0x00, 0x00, 0x40, 0x02, 0x00, 0x00, 0x40, 0x02, 0x00,
        0x00, 0x40, 0x02, 0x00, 0x00, 0x40, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
    static const uchar hsplitm_bits[] = {
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0xe0, 0x07, 0x00, 0x00, 0xe0, 0x07, 0x00, 0x00, 0xe0, 0x07, 0x00,
        0x00, 0xe0, 0x07, 0x00, 0x00, 0xe2, 0x47, 0x00, 0x00, 0xe3, 0xc7, 0x00,
        0x80, 0xe3, 0xc7, 0x01, 0xc0, 0xff, 0xff, 0x03, 0xe0, 0xff, 0xff, 0x07,
        0xc0, 0xff, 0xff, 0x03, 0x80, 0xe3, 0xc7, 0x01, 0x00, 0xe3, 0xc7, 0x00,
        0x00, 0xe2, 0x47, 0x00, 0x00, 0xe0, 0x07, 0x00, 0x00, 0xe0, 0x07, 0x00,
        0x00, 0xe0, 0x07, 0x00, 0x00, 0xe0, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
    static const uchar phand_bits[] = {
        0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x80, 0x04, 0x00, 0x00,
        0x80, 0x04, 0x00, 0x00, 0x80, 0x04, 0x00, 0x00, 0x80, 0x04, 0x00, 0x00,
        0x80, 0x1c, 0x00, 0x00, 0x80, 0xe4, 0x00, 0x00, 0x80, 0x24, 0x03, 0x00,
        0x80, 0x24, 0x05, 0x00, 0xb8, 0x24, 0x09, 0x00, 0xc8, 0x00, 0x09, 0x00,
        0x88, 0x00, 0x08, 0x00, 0x90, 0x00, 0x08, 0x00, 0xa0, 0x00, 0x08, 0x00,
        0x20, 0x00, 0x08, 0x00, 0x40, 0x00, 0x08, 0x00, 0x40, 0x00, 0x04, 0x00,
        0x80, 0x00, 0x04, 0x00, 0x80, 0x00, 0x04, 0x00, 0x00, 0x01, 0x02, 0x00,
        0x00, 0x01, 0x02, 0x00, 0x00, 0xff, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

   static const uchar phandm_bits[] = {
        0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x80, 0x07, 0x00, 0x00,
        0x80, 0x07, 0x00, 0x00, 0x80, 0x07, 0x00, 0x00, 0x80, 0x07, 0x00, 0x00,
        0x80, 0x1f, 0x00, 0x00, 0x80, 0xff, 0x00, 0x00, 0x80, 0xff, 0x03, 0x00,
        0x80, 0xff, 0x07, 0x00, 0xb8, 0xff, 0x0f, 0x00, 0xf8, 0xff, 0x0f, 0x00,
        0xf8, 0xff, 0x0f, 0x00, 0xf0, 0xff, 0x0f, 0x00, 0xe0, 0xff, 0x0f, 0x00,
        0xe0, 0xff, 0x0f, 0x00, 0xc0, 0xff, 0x0f, 0x00, 0xc0, 0xff, 0x07, 0x00,
        0x80, 0xff, 0x07, 0x00, 0x80, 0xff, 0x07, 0x00, 0x00, 0xff, 0x03, 0x00,
        0x00, 0xff, 0x03, 0x00, 0x00, 0xff, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

   static const uchar openhand_bits[] = {
        0x80,0x01,0x58,0x0e,0x64,0x12,0x64,0x52,0x48,0xb2,0x48,0x92,
        0x16,0x90,0x19,0x80,0x11,0x40,0x02,0x40,0x04,0x40,0x04,0x20,
        0x08,0x20,0x10,0x10,0x20,0x10,0x00,0x00};
    static const uchar openhandm_bits[] = {
       0x80,0x01,0xd8,0x0f,0xfc,0x1f,0xfc,0x5f,0xf8,0xff,0xf8,0xff,
       0xf6,0xff,0xff,0xff,0xff,0x7f,0xfe,0x7f,0xfc,0x7f,0xfc,0x3f,
       0xf8,0x3f,0xf0,0x1f,0xe0,0x1f,0x00,0x00};
    static const uchar closedhand_bits[] = {
        0x00,0x00,0x00,0x00,0x00,0x00,0xb0,0x0d,0x48,0x32,0x08,0x50,
        0x10,0x40,0x18,0x40,0x04,0x40,0x04,0x20,0x08,0x20,0x10,0x10,
        0x20,0x10,0x20,0x10,0x00,0x00,0x00,0x00};
    static const uchar closedhandm_bits[] = {
        0x00,0x00,0x00,0x00,0x00,0x00,0xb0,0x0d,0xf8,0x3f,0xf8,0x7f,
        0xf0,0x7f,0xf8,0x7f,0xfc,0x7f,0xfc,0x3f,0xf8,0x3f,0xf0,0x1f,
        0xe0,0x1f,0xe0,0x1f,0x00,0x00,0x00,0x00};

    static const uchar * const cursor_bits32[] = {
        vsplit_bits, vsplitm_bits, hsplit_bits, hsplitm_bits,
        phand_bits, phandm_bits
    };

    unsigned short *sh;
    switch (cshape) {                        // map to windows cursor
    case Qt::ArrowCursor:
        sh = (unsigned short*)IDC_ARROW;
        break;
    case Qt::UpArrowCursor:
        sh = (unsigned short*)IDC_UPARROW;
        break;
    case Qt::CrossCursor:
        sh = (unsigned short*)IDC_CROSS;
        break;
    case Qt::WaitCursor:
        sh = (unsigned short*)IDC_WAIT;
        break;
    case Qt::IBeamCursor:
        sh = (unsigned short*)IDC_IBEAM;
        break;
    case Qt::SizeVerCursor:
        sh = (unsigned short*)IDC_SIZENS;
        break;
    case Qt::SizeHorCursor:
        sh = (unsigned short*)IDC_SIZEWE;
        break;
    case Qt::SizeBDiagCursor:
        sh = (unsigned short*)IDC_SIZENESW;
        break;
    case Qt::SizeFDiagCursor:
        sh = (unsigned short*)IDC_SIZENWSE;
        break;
    case Qt::SizeAllCursor:
        sh = (unsigned short*)IDC_SIZEALL;
        break;
    case Qt::ForbiddenCursor:
        sh = (unsigned short*)IDC_NO;
        break;
    case Qt::WhatsThisCursor:
        sh = (unsigned short*)IDC_HELP;
        break;
    case Qt::BusyCursor:
        sh = (unsigned short*)IDC_APPSTARTING;
        break;
    case Qt::PointingHandCursor:
        if ((QSysInfo::WindowsVersion & QSysInfo::WV_DOS_based) > QSysInfo::WV_95 ||
            (QSysInfo::WindowsVersion & QSysInfo::WV_NT_based) > QSysInfo::WV_NT) {
            sh = (unsigned short*)IDC_HAND;
            break;
        }
        // fall through
    case Qt::BlankCursor:
    case Qt::SplitVCursor:
    case Qt::SplitHCursor:
    case Qt::OpenHandCursor:
    case Qt::ClosedHandCursor:
    case Qt::BitmapCursor: {
        QImage bbits, mbits;
        bool invb, invm;
        if (cshape == Qt::BlankCursor) {
            bbits = QImage(32, 32, QImage::Format_Mono);
            bbits.fill(0);                // ignore color table
            mbits = bbits.copy();
            hx = hy = 16;
            invb = invm = false;
        } else if (cshape == Qt::OpenHandCursor || cshape == Qt::ClosedHandCursor) {
            bool open = cshape == Qt::OpenHandCursor;
            QBitmap cb = QBitmap::fromData(QSize(16, 16), open ? openhand_bits : closedhand_bits);
            QBitmap cm = QBitmap::fromData(QSize(16, 16), open ? openhandm_bits : closedhandm_bits);
            bbits = cb.toImage().convertToFormat(QImage::Format_Mono);
            mbits = cm.toImage().convertToFormat(QImage::Format_Mono);
            hx = hy = 8;
            invb = invm = false;
        } else if (cshape != Qt::BitmapCursor) {
            int i = cshape - Qt::SplitVCursor;
            QBitmap cb = QBitmap::fromData(QSize(32, 32), cursor_bits32[i * 2]);
            QBitmap cm = QBitmap::fromData(QSize(32, 32), cursor_bits32[i * 2 + 1]);
            bbits = cb.toImage().convertToFormat(QImage::Format_Mono);
            mbits = cm.toImage().convertToFormat(QImage::Format_Mono);
            if (cshape == Qt::PointingHandCursor) {
                hx = 7;
                hy = 0;
            } else
                hx = hy = 16;
            invb = invm = false;
        } else {
            bbits = bm->toImage().convertToFormat(QImage::Format_Mono);
            mbits = bmm->toImage().convertToFormat(QImage::Format_Mono);
            invb = bbits.numColors() > 1 && qGray(bbits.color(0)) < qGray(bbits.color(1));
            invm = mbits.numColors() > 1 && qGray(mbits.color(0)) < qGray(mbits.color(1));
        }
        int n = qMax(1, bbits.width() / 8);
        int h = bbits.height();
#if !defined(Q_OS_WINCE)
        uchar* xBits = new uchar[h * n];
        uchar* xMask = new uchar[h * n];
        int x = 0;
        for (int i = 0; i < h; ++i) {
            uchar *bits = bbits.scanLine(i);
            uchar *mask = mbits.scanLine(i);
            for (int j = 0; j < n; ++j) {
                uchar b = bits[j];
                uchar m = mask[j];
                if (invb)
                    b ^= 0xff;
                if (invm)
                    m ^= 0xff;
                xBits[x] = ~m;
                xMask[x] = b ^ m;
                ++x;
            }
        }
        hcurs = CreateCursor(qWinAppInst(), hx, hy, bbits.width(), bbits.height(),
                                   xBits, xMask);
    	delete [] xBits;
    	delete [] xMask;
#elif defined(GWES_ICONCURS) // Q_OS_WINCE
        // Windows CE only supports fixed cursor size.
        int sysW = GetSystemMetrics(SM_CXCURSOR);
        int sysH = GetSystemMetrics(SM_CYCURSOR);
        int sysN = qMax(1, sysW / 8);
        uchar* xBits = new uchar[sysH * sysN];
        uchar* xMask = new uchar[sysH * sysN];
        int x = 0;
        for (int i = 0; i < sysH; ++i) {
            if (i >= h) {
                memset(&xBits[x] , 255, sysN);
                memset(&xMask[x] ,   0, sysN);
                x += sysN;
            } else {
                int fillWidth = n > sysN ? sysN : n;
                uchar *bits = bbits.scanLine(i);
                uchar *mask = mbits.scanLine(i);
                for (int j = 0; j < fillWidth; ++j) {
                    uchar b = bits[j];
                    uchar m = mask[j];
                    if (invb)
                        b ^= 0xFF;
                    if (invm)
                        m ^= 0xFF;
                    xBits[x] = ~m;
                    xMask[x] = b ^ m;
                    ++x;
                }
                for (int j = fillWidth; j < sysN; ++j ) {
                    xBits[x] = 255;
                    xMask[x] = 0;
                    ++x;
                }
            }
        }

        hcurs = CreateCursor(qWinAppInst(), hx, hy, sysW, sysH,
                                   xBits, xMask);
    	delete [] xBits;
    	delete [] xMask;
#else
        Q_UNUSED(n);
        Q_UNUSED(h);
#endif
	return;
    }
    default:
        qWarning("QCursor::update: Invalid cursor shape %d", cshape);
        return;
    }
    // ### From MSDN:
    // ### LoadCursor has been superseded by the LoadImage function.
    QT_WA({
        hcurs = LoadCursorW(0, reinterpret_cast<const TCHAR *>(sh));
    } , {
        hcurs = LoadCursorA(0, reinterpret_cast<const char*>(sh));
    });
}

QT_END_NAMESPACE
#endif // QT_NO_CURSOR
