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

#ifndef QBACKINGSTORE_P_H
#define QBACKINGSTORE_P_H

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

#include "private/qpaintengine_raster_p.h"
#include "private/qwidget_p.h"

QT_BEGIN_NAMESPACE

class QWindowSurface;

class Q_AUTOTEST_EXPORT QWidgetBackingStore
{
public:
    QWidgetBackingStore(QWidget *t);
    ~QWidgetBackingStore();
    bool bltRect(const QRect &rect, int dx, int dy, QWidget *widget);
    void dirtyRegion(const QRegion &rgn, QWidget *widget=0, bool updateImmediately = false);
#ifdef Q_RATE_LIMIT_PAINTING
    void updateDirtyRegion(QWidget *widget);
#endif
    void cleanRegion(const QRegion &rgn, QWidget *widget=0, bool recursiveCopyToScreen = true);
#if defined (Q_WS_QWS) || defined (Q_WS_WIN)
    void releaseBuffer();
#endif

    inline QPoint topLevelOffset() const { return tlwOffset; }
    static void copyToScreen(QWidget *, const QRegion &);
    static void qt_unflushPaint(QWidget *widget, const QRegion &rgn);
    static void qt_showYellowThing(QWidget *widget, const QRegion &rgn, int msec, bool);
#ifdef Q_WS_WIN
    static void blitToScreen(const QRegion &rgn, QWidget *w);
#endif
#ifdef Q_WIDGET_USE_DIRTYLIST
    void removeDirtyWidget(QWidget *w);
#endif

    static bool isOpaque(const QWidget *widget);
#ifdef Q_RATE_LIMIT_PAINTING
    static int refreshInterval;
#endif

private:
    QWidget *tlw;
#ifdef Q_WS_QWS
    QRegion dirtyOnScreen;
#else
    QRegion dirty;
#endif
#ifdef Q_WIDGET_USE_DIRTYLIST
    QList<QWidget*> dirtyWidgets;
#endif

    QWindowSurface *windowSurface;
#ifdef Q_BACKINGSTORE_SUBSURFACES
    QList<QWindowSurface*> subSurfaces;
#endif
    QPoint tlwOffset;

    void copyToScreen(const QRegion &rgn, QWidget *widget, const QPoint &offset, bool recursive = true);

    static void updateWidget(QWidget *that, const QRegion &rgn);

    friend void qt_syncBackingStore(QRegion, QWidget *);
#if defined(Q_WS_X11) || defined(Q_WS_QWS) || defined(Q_WS_WIN)
    friend void qt_syncBackingStore(QWidget *);
#endif
    friend QRegion qt_dirtyRegion(QWidget *, bool);
    friend class QWidgetPrivate;
    friend class QWidget;
    friend class QWSManagerPrivate;
    friend class QETWidget;
    friend class QWindowSurface;
    friend class QWSWindowSurface;
};

QT_END_NAMESPACE

#endif // QBACKINGSTORE_P_H
