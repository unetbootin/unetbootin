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

#ifndef QBITMAP_H
#define QBITMAP_H

#include <QtGui/qpixmap.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Gui)

class QVariant;

class Q_GUI_EXPORT QBitmap : public QPixmap
{
public:
    QBitmap();
    QBitmap(const QPixmap &);
    QBitmap(int w, int h);
    explicit QBitmap(const QSize &);
    explicit QBitmap(const QString &fileName, const char *format=0);
    ~QBitmap();

    QBitmap &operator=(const QPixmap &);
    operator QVariant() const;

    inline void clear() { fill(Qt::color0); }

    static QBitmap fromImage(const QImage &image, Qt::ImageConversionFlags flags = Qt::AutoColor);
    static QBitmap fromData(const QSize &size, const uchar *bits,
                            QImage::Format monoFormat = QImage::Format_MonoLSB);

    QBitmap transformed(const QMatrix &) const;
    QBitmap transformed(const QTransform &matrix) const;
#ifdef QT3_SUPPORT
    inline QT3_SUPPORT_CONSTRUCTOR QBitmap(int w, int h, bool clear);
    inline QT3_SUPPORT_CONSTRUCTOR QBitmap(const QSize &, bool clear);
    QT3_SUPPORT_CONSTRUCTOR QBitmap(int w, int h, const uchar *bits, bool isXbitmap=false);
    QT3_SUPPORT_CONSTRUCTOR QBitmap(const QSize &, const uchar *bits, bool isXbitmap=false);
    inline QT3_SUPPORT QBitmap xForm(const QMatrix &matrix) const { return transformed(QTransform(matrix)); }
    QT3_SUPPORT_CONSTRUCTOR QBitmap(const QImage &image) { *this = fromImage(image); }
    QT3_SUPPORT QBitmap &operator=(const QImage &image) { *this = fromImage(image); return *this; }
#endif
};
Q_DECLARE_SHARED(QBitmap)

#ifdef QT3_SUPPORT
inline QBitmap::QBitmap(int w, int h, bool clear)
    : QPixmap(QSize(w, h), 1)
{
    if (clear) this->clear();
}

inline QBitmap::QBitmap(const QSize &size, bool clear)
    : QPixmap(size, 1)
{
    if (clear) this->clear();
}
#endif

QT_END_NAMESPACE

QT_END_HEADER

#endif // QBITMAP_H
