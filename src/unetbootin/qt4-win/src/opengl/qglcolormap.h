/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the QtOpenGL module of the Qt Toolkit.
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

#ifndef QGLCOLORMAP_H
#define QGLCOLORMAP_H

#include <QtGui/qcolor.h>
#include <QtCore/qvector.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(OpenGL)

class Q_OPENGL_EXPORT QGLColormap
{
public:
    QGLColormap();
    QGLColormap(const QGLColormap &);
    ~QGLColormap();

    QGLColormap &operator=(const QGLColormap &);

    bool   isEmpty() const;
    int    size() const;
    void   detach();

    void   setEntries(int count, const QRgb * colors, int base = 0);
    void   setEntry(int idx, QRgb color);
    void   setEntry(int idx, const QColor & color);
    QRgb   entryRgb(int idx) const;
    QColor entryColor(int idx) const;
    int    find(QRgb color) const;
    int    findNearest(QRgb color) const;

protected:
    Qt::HANDLE handle() { return d ? d->cmapHandle : 0; }
    void setHandle(Qt::HANDLE ahandle) { d->cmapHandle = ahandle; }

private:
    struct QGLColormapData {
        QBasicAtomicInt ref;
        QVector<QRgb> *cells;
        Qt::HANDLE cmapHandle;
    };

    QGLColormapData *d;
    static struct QGLColormapData shared_null;
    static void cleanup(QGLColormapData *x);
    void detach_helper();

    friend class QGLWidget;
    friend class QGLWidgetPrivate;
};

inline void QGLColormap::detach()
{
    if (d->ref != 1)
        detach_helper();
}

QT_END_NAMESPACE

QT_END_HEADER

#endif // QGLCOLORMAP_H
