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

#ifndef QPIXMAPDATA_P_H
#define QPIXMAPDATA_P_H

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

#include <QtGui/qpixmap.h>
#include <QtCore/qatomic.h>

QT_BEGIN_NAMESPACE

class Q_GUI_EXPORT QPixmapData
{
public:
    enum PixelType {
        // WARNING: Do not change the first two
        // Must match QPixmap::Type
        PixmapType, BitmapType
    };
    enum ClassId { RasterClass, X11Class, MacClass, DirectFBClass,
                   OpenGLClass, OpenVGClass, CustomClass = 1024 };

    QPixmapData(PixelType pixelpType, int classId);
    virtual ~QPixmapData();

    virtual void resize(int width, int height) = 0;
    virtual void fromImage(const QImage &image,
                           Qt::ImageConversionFlags flags) = 0;
    virtual void fromFile(const QString &filename, const char *format,
                          Qt::ImageConversionFlags flags);
    virtual void copy(const QPixmapData *data, const QRect &rect);

    virtual int metric(QPaintDevice::PaintDeviceMetric metric) const = 0;
    virtual void fill(const QColor &color) = 0;
    virtual QBitmap mask() const;
    virtual void setMask(const QBitmap &mask);
    virtual bool hasAlphaChannel() const = 0;
    virtual QPixmap transformed(const QTransform &matrix,
                                Qt::TransformationMode mode) const;
    virtual void setAlphaChannel(const QPixmap &alphaChannel);
    virtual QPixmap alphaChannel() const;
    virtual QImage toImage() const = 0;
    virtual QPaintEngine* paintEngine() const = 0;

    inline int serialNumber() const { return ser_no; }

    inline PixelType pixelType() const { return type; }
    inline ClassId classId() const { return static_cast<ClassId>(id); }

    virtual QImage* buffer();

    int width() const { return metric(QPaintDevice::PdmWidth); }
    int height() const { return metric(QPaintDevice::PdmHeight); }
    int numColors() const { return metric(QPaintDevice::PdmNumColors); }
    int depth() const { return metric(QPaintDevice::PdmDepth); }

protected:
    void setSerialNumber(int serNo);

private:
    friend class QPixmap;
    friend class QGLContextPrivate;

    QAtomicInt ref;
    int detach_no;

    PixelType type;
    int id;
    int ser_no;
    uint is_cached;
};

#ifdef Q_WS_WIN
#ifndef Q_OS_WINCE
QPixmap convertHIconToPixmap( const HICON icon);
#else
QPixmap convertHIconToPixmap( const HICON icon, bool large = false);
#endif
QPixmap loadIconFromShell32( int resourceId, int size );
#endif

#  define QT_XFORM_TYPE_MSBFIRST 0
#  define QT_XFORM_TYPE_LSBFIRST 1
#  if defined(Q_WS_WIN)
#    define QT_XFORM_TYPE_WINDOWSPIXMAP 2
#  endif
extern bool qt_xForm_helper(const QTransform&, int, int, int, uchar*, int, int, int, const uchar*, int, int, int);

QT_END_NAMESPACE

#endif // QPIXMAPDATA_P_H
