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

#include "qpixmap.h"
#include "qpixmap_raster_p.h"

#include "qbitmap.h"
#include "qimage.h"
#include "qwidget.h"
#include "qpainter.h"
#include "qdatastream.h"
#include "qbuffer.h"
#include "qapplication.h"
#include "qevent.h"
#include "qfile.h"
#include "qfileinfo.h"
#include "qdatetime.h"
#include "qpixmapcache.h"
#include "qimagereader.h"
#include "qimagewriter.h"
#include "qdebug.h"
#include "qt_windows.h"

#if defined(Q_OS_WINCE)
#include "qguifunctions_wince.h"
#include <winbase.h>
extern bool qt_wince_is_high_dpi();
extern bool qt_wince_is_pocket_pc();
#endif

QT_BEGIN_NAMESPACE

QPixmap QPixmap::grabWindow(WId winId, int x, int y, int w, int h )
{
    RECT r;
    GetClientRect(winId, &r);

    if (w < 0) w = r.right - r.left;
    if (h < 0) h = r.bottom - r.top;

#ifdef Q_OS_WINCE_WM
    if (qt_wince_is_pocket_pc()) {
        QWidget *widget = QWidget::find(winId);
        if (qobject_cast<QDesktopWidget *>(widget)) {
            RECT rect = {0,0,0,0};
            AdjustWindowRectEx(&rect, WS_BORDER | WS_CAPTION, FALSE, 0);
            int magicNumber = qt_wince_is_high_dpi() ? 4 : 2;
            y += rect.top - magicNumber;
        }
    }
#endif

    // Create and setup bitmap
    HDC bitmap_dc = CreateCompatibleDC(qt_win_display_dc());
    HBITMAP bitmap = CreateCompatibleBitmap(qt_win_display_dc(), w, h);
    HGDIOBJ null_bitmap = SelectObject(bitmap_dc, bitmap);

    // copy data
    HDC window_dc = GetDC(winId);
    BitBlt(bitmap_dc, 0, 0, w, h, window_dc, x, y, SRCCOPY);

    // clean up all but bitmap
    ReleaseDC(winId, window_dc);
    SelectObject(bitmap_dc, null_bitmap);
    DeleteDC(bitmap_dc);

    QPixmap pixmap = QPixmap::fromWinHBITMAP(bitmap);

    DeleteObject(bitmap);

    return pixmap;
}



/*!
    \enum QPixmap::HBitmapFormat

    This enum defines how the conversion between \c HBITMAP
    and QPixmap is performed.

    \warning This enum is only available on Windows.

    \value NoAlpha The alpha channel is ignored and always treated as
    being set to fully opaque. This is preferred if the \c HBITMAP is
    used with standard GDI calls, such as \c BitBlt().

    \value PremultipliedAlpha The \c HBITMAP is treated as having a
    alpha channel and premultiplied colors. This is preferred if the
    \c HBITMAP is accessed through the \c AlphaBlend() GDI function.

    \sa fromWinHBITMAP(), toWinHBITMAP()
*/

/*!
    Creates a \c HBITMAP equivalent to the QPixmap, based on the given
    \a format. Returns the \c HBITMAP handle.

    It is the caller's responsibility to free the \c HBITMAP data
    after use.

    \warning This function is only available on Windows.

    \sa fromWinHBITMAP()
*/
HBITMAP QPixmap::toWinHBITMAP(HBitmapFormat format) const
{
    QRasterPixmapData* d = static_cast<QRasterPixmapData*>(data);
    int w = d->image.width();
    int h = d->image.height();

    HDC display_dc = qt_win_display_dc();

    // Define the header
    BITMAPINFO bmi;
    memset(&bmi, 0, sizeof(bmi));
    bmi.bmiHeader.biSize        = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth       = w;
    bmi.bmiHeader.biHeight      = -h;
    bmi.bmiHeader.biPlanes      = 1;
    bmi.bmiHeader.biBitCount    = 32;
    bmi.bmiHeader.biCompression = BI_RGB;
    bmi.bmiHeader.biSizeImage   = w * h * 4;

    // Create the pixmap
    uchar *pixels = 0;
    HBITMAP bitmap = CreateDIBSection(display_dc, &bmi, DIB_RGB_COLORS, (void **) &pixels, 0, 0);
    if (!bitmap) {
        qErrnoWarning("QPixmap::toWinHBITMAP(), failed to create dibsection");
        return 0;
    }
    if (!pixels) {
        qErrnoWarning("QPixmap::toWinHBITMAP(), did not allocate pixel data");
        return 0;
    }

    // Copy over the data
    QImage::Format imageFormat = format == NoAlpha
                                 ? QImage::Format_RGB32
                                 : QImage::Format_ARGB32_Premultiplied;
    const QImage image = d->image.convertToFormat(imageFormat);
    int bytes_per_line = w * 4;
    for (int y=0; y<h; ++y)
        memcpy(pixels + y * bytes_per_line, image.scanLine(y), bytes_per_line);

    return bitmap;
}

/*!
    Returns a QPixmap that is equivalent to the given \a bitmap. The
    conversion is based on the specified \a format.

    \warning This function is only available on Windows.

    \sa toWinHBITMAP(), {QPixmap#Pixmap Conversion}{Pixmap Conversion}

*/
QPixmap QPixmap::fromWinHBITMAP(HBITMAP bitmap, HBitmapFormat format)
{
    // Verify size
    BITMAP bitmap_info;
    memset(&bitmap_info, 0, sizeof(BITMAP));

    int res;
    QT_WA({
            res = GetObjectW(bitmap, sizeof(BITMAP), &bitmap_info);
        } , {
              res = GetObjectA(bitmap, sizeof(BITMAP), &bitmap_info);
          });

    if (!res) {
        qErrnoWarning("QPixmap::fromWinHBITMAP(), failed to get bitmap info");
        return QPixmap();
    }
    int w = bitmap_info.bmWidth;
    int h = bitmap_info.bmHeight;

    BITMAPINFO bmi;
    memset(&bmi, 0, sizeof(bmi));
    bmi.bmiHeader.biSize        = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth       = w;
    bmi.bmiHeader.biHeight      = -h;
    bmi.bmiHeader.biPlanes      = 1;
    bmi.bmiHeader.biBitCount    = 32;
    bmi.bmiHeader.biCompression = BI_RGB;
    bmi.bmiHeader.biSizeImage   = w * h * 4;

    QImage result;
    // Get bitmap bits
    uchar *data = (uchar *) qMalloc(bmi.bmiHeader.biSizeImage);
    if (GetDIBits(qt_win_display_dc(), bitmap, 0, h, data, &bmi, DIB_RGB_COLORS)) {

        QImage::Format imageFormat = QImage::Format_ARGB32_Premultiplied;
        uint mask = 0;
        if (format == NoAlpha) {
            imageFormat = QImage::Format_RGB32;
            mask = 0xff000000;
        }

        // Create image and copy data into image.
        QImage image(w, h, imageFormat);
        if (!image.isNull()) { // failed to alloc?
            int bytes_per_line = w * sizeof(QRgb);
            for (int y=0; y<h; ++y) {
                QRgb *dest = (QRgb *) image.scanLine(y);
                const QRgb *src = (const QRgb *) (data + y * bytes_per_line);
                for (int x=0; x<w; ++x) {
                    dest[x] = src[x] | mask;
                }
            }
        }
        result = image;
    } else {
        qWarning("QPixmap::fromWinHBITMAP(), failed to get bitmap bits");
    }
    qFree(data);
    return fromImage(result);
}

#ifdef Q_WS_WIN
#ifndef Q_OS_WINCE
QPixmap convertHIconToPixmap( const HICON icon)
{
    bool foundAlpha = false;
    HDC screenDevice = qt_win_display_dc();
    HDC hdc = CreateCompatibleDC(screenDevice);

    ICONINFO iconinfo;
    bool result = GetIconInfo(icon, &iconinfo); //x and y Hotspot describes the icon center
    if (!result)
        qWarning("convertHIconToPixmap(), failed to GetIconInfo()");

    BITMAPINFOHEADER bitmapInfo;
    bitmapInfo.biSize        = sizeof(BITMAPINFOHEADER);
    bitmapInfo.biWidth       = iconinfo.xHotspot * 2;
    bitmapInfo.biHeight      = iconinfo.yHotspot * 2;
    bitmapInfo.biPlanes      = 1;
    bitmapInfo.biBitCount    = 32;
    bitmapInfo.biCompression = BI_RGB;
    bitmapInfo.biSizeImage   = 0;
    bitmapInfo.biXPelsPerMeter = 0;
    bitmapInfo.biYPelsPerMeter = 0;
    bitmapInfo.biClrUsed       = 0;
    bitmapInfo.biClrImportant  = 0;
    DWORD* bits;


    HBITMAP winBitmap = CreateDIBSection(hdc, (BITMAPINFO*)&bitmapInfo, DIB_RGB_COLORS, (VOID**)&bits, NULL, 0);
    HGDIOBJ oldhdc = (HBITMAP)SelectObject(hdc, winBitmap);
    DrawIconEx( hdc, 0, 0, icon, iconinfo.xHotspot * 2, iconinfo.yHotspot * 2, 0, 0, DI_NORMAL);

    QPixmap::HBitmapFormat alphaType = QPixmap::PremultipliedAlpha;
    QPixmap iconpixmap = QPixmap::fromWinHBITMAP(winBitmap, alphaType);
    QImage img = iconpixmap.toImage();

    for (int y = 0 ; y < iconpixmap.height() && !foundAlpha ; y++) {
        QRgb *scanLine= reinterpret_cast<QRgb *>(img.scanLine(y));
        for (int x = 0; x < img.width() ; x++) {
            if (qAlpha(scanLine[x]) != 0) {
                foundAlpha = true;
                break;
            }
        }
    }
    if (!foundAlpha) {
        //If no alpha was found, we use the mask to set alpha values
        DrawIconEx( hdc, 0, 0, icon, iconinfo.xHotspot * 2, iconinfo.yHotspot * 2, 0, 0, DI_MASK);
        QPixmap maskPixmap = QPixmap::fromWinHBITMAP(winBitmap, alphaType);
        QImage mask = maskPixmap.toImage();

        for (int y = 0 ; y< iconpixmap.height() ; y++){
            QRgb *scanlineImage = reinterpret_cast<QRgb *>(img.scanLine(y));
            QRgb *scanlineMask = mask.isNull() ? 0 : reinterpret_cast<QRgb *>(mask.scanLine(y));
            for (int x = 0; x < img.width() ; x++){
                if (scanlineMask && qRed(scanlineMask[x]) != 0)
                    scanlineImage[x] = 0; //mask out this pixel
                else
                    scanlineImage[x] |= 0xff000000; // set the alpha channel to 255
            }
        }
    }
    //dispose resources created by iconinfo call
    DeleteObject(iconinfo.hbmMask);
    DeleteObject(iconinfo.hbmColor);

    SelectObject(hdc, oldhdc); //restore state
    DeleteObject(winBitmap);
    DeleteDC(hdc);
    return QPixmap::fromImage(img);
}
#else //ifndef Q_OS_WINCE
QPixmap convertHIconToPixmap( const HICON icon, bool large)
{
    HDC screenDevice = qt_win_display_dc();
    HDC hdc = CreateCompatibleDC(screenDevice);
    int size = large ? 64 : 32;

    BITMAPINFO bmi;
    memset(&bmi, 0, sizeof(bmi));
    bmi.bmiHeader.biSize        = sizeof(BITMAPINFO);
    bmi.bmiHeader.biWidth       = size;
    bmi.bmiHeader.biHeight      = -size;
    bmi.bmiHeader.biPlanes      = 1;
    bmi.bmiHeader.biBitCount    = 32;
    bmi.bmiHeader.biCompression = BI_RGB;
    bmi.bmiHeader.biSizeImage   = size*size*4;

    uchar* bits;

    HBITMAP winBitmap = CreateDIBSection(hdc, &bmi, DIB_RGB_COLORS, (void**) &bits, 0, 0);
    if (winBitmap )
        memset(bits, 0xff, size*size*4);
    if (!winBitmap) {
        qWarning("convertHIconToPixmap(), failed to CreateDIBSection()");
        return QPixmap();
    }

    HGDIOBJ oldhdc = (HBITMAP)SelectObject(hdc, winBitmap);
    if (!DrawIconEx( hdc, 0, 0, icon, size, size, 0, 0, DI_NORMAL))
        qWarning("convertHIconToPixmap(), failed to DrawIcon()");

    uint mask = 0xff000000;
    // Create image and copy data into image.
    QImage image(size, size, QImage::Format_ARGB32);

    if (!image.isNull()) { // failed to alloc?
        int bytes_per_line = size * sizeof(QRgb);
        for (int y=0; y<size; ++y) {
            QRgb *dest = (QRgb *) image.scanLine(y);
            const QRgb *src = (const QRgb *) (bits + y * bytes_per_line);
            for (int x=0; x<size; ++x) {
                dest[x] = src[x];
            }
        }
    }
    if (!DrawIconEx( hdc, 0, 0, icon, size, size, 0, 0, DI_MASK))
        qWarning("convertHIconToPixmap(), failed to DrawIcon()");
    if (!image.isNull()) { // failed to alloc?
        int bytes_per_line = size * sizeof(QRgb);
        for (int y=0; y<size; ++y) {
            QRgb *dest = (QRgb *) image.scanLine(y);
            const QRgb *src = (const QRgb *) (bits + y * bytes_per_line);
            for (int x=0; x<size; ++x) {
                if (!src[x])
                    dest[x] = dest[x] | mask;
            }
        }
    }
    SelectObject(hdc, oldhdc); //restore state
    DeleteObject(winBitmap);
    DeleteDC(hdc);
    return QPixmap::fromImage(image);
}
#endif //ifndef Q_OS_WINCE

QPixmap loadIconFromShell32( int resourceId, int size )
{
#ifdef Q_OS_WINCE
    HMODULE hmod = LoadLibrary((const wchar_t *) QString::fromLatin1("ceshell.dll").utf16());
#else
    HMODULE hmod = LoadLibraryA("shell32.dll");
#endif
    if( hmod ) {
        HICON iconHandle = (HICON)LoadImage(hmod, MAKEINTRESOURCE(resourceId), IMAGE_ICON, size, size, 0);
        if( iconHandle ) {
            QPixmap iconpixmap = convertHIconToPixmap( iconHandle );
            DestroyIcon(iconHandle);
            return iconpixmap;
        }
    }
    return QPixmap();
}

#endif

QT_END_NAMESPACE
