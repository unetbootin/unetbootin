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

#ifndef QDRAWUTIL_H
#define QDRAWUTIL_H

#include <QtCore/qnamespace.h>
#include <QtCore/qstring.h> // char*->QString conversion

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Gui)

class QPainter;
#ifndef QT3_SUPPORT
class QColorGroup;
#endif
class QPalette;
class QPoint;
class QColor;
class QBrush;
class QRect;
class QPixmap;

//
// Standard shade drawing
//

Q_GUI_EXPORT void qDrawShadeLine(QPainter *p, int x1, int y1, int x2, int y2,
                              const QPalette &pal, bool sunken = true,
                              int lineWidth = 1, int midLineWidth = 0);

Q_GUI_EXPORT void qDrawShadeLine(QPainter *p, const QPoint &p1, const QPoint &p2,
                              const QPalette &pal, bool sunken = true,
                              int lineWidth = 1, int midLineWidth = 0);

Q_GUI_EXPORT void qDrawShadeRect(QPainter *p, int x, int y, int w, int h,
                              const QPalette &pal, bool sunken = false,
                              int lineWidth = 1, int midLineWidth = 0,
                              const QBrush *fill = 0);

Q_GUI_EXPORT void qDrawShadeRect(QPainter *p, const QRect &r,
                              const QPalette &pal, bool sunken = false,
                              int lineWidth = 1, int midLineWidth = 0,
                              const QBrush *fill = 0);

Q_GUI_EXPORT void qDrawShadePanel(QPainter *p, int x, int y, int w, int h,
                               const QPalette &pal, bool sunken = false,
                               int lineWidth = 1, const QBrush *fill = 0);

Q_GUI_EXPORT void qDrawShadePanel(QPainter *p, const QRect &r,
                               const QPalette &pal, bool sunken = false,
                               int lineWidth = 1, const QBrush *fill = 0);

Q_GUI_EXPORT void qDrawWinButton(QPainter *p, int x, int y, int w, int h,
                              const QPalette &pal, bool sunken = false,
                              const QBrush *fill = 0);

Q_GUI_EXPORT void qDrawWinButton(QPainter *p, const QRect &r,
                              const QPalette &pal, bool sunken = false,
                              const QBrush *fill = 0);

Q_GUI_EXPORT void qDrawWinPanel(QPainter *p, int x, int y, int w, int h,
                              const QPalette &pal, bool sunken = false,
                             const QBrush *fill = 0);

Q_GUI_EXPORT void qDrawWinPanel(QPainter *p, const QRect &r,
                              const QPalette &pal, bool sunken = false,
                             const QBrush *fill = 0);

Q_GUI_EXPORT void qDrawPlainRect(QPainter *p, int x, int y, int w, int h, const QColor &,
                              int lineWidth = 1, const QBrush *fill = 0);

Q_GUI_EXPORT void qDrawPlainRect(QPainter *p, const QRect &r, const QColor &,
                              int lineWidth = 1, const QBrush *fill = 0);


#ifdef QT3_SUPPORT
//
// Use QStyle::itemRect(), QStyle::drawItem() and QStyle::drawArrow() instead.
//
Q_GUI_EXPORT QT3_SUPPORT QRect qItemRect(QPainter *p, Qt::GUIStyle gs, int x, int y, int w, int h,
                          int flags, bool enabled,
                          const QPixmap *pixmap, const QString& text, int len=-1);

Q_GUI_EXPORT QT3_SUPPORT void qDrawItem(QPainter *p, Qt::GUIStyle gs, int x, int y, int w, int h,
                         int flags, const QPalette &pal, bool enabled,
                         const QPixmap *pixmap, const QString& text,
                         int len=-1, const QColor* penColor = 0);

Q_GUI_EXPORT QT3_SUPPORT void qDrawArrow(QPainter *p, Qt::ArrowType type, Qt::GUIStyle style, bool down,
                          int x, int y, int w, int h,
                          const QPalette &pal, bool enabled);
#endif

QT_END_NAMESPACE

QT_END_HEADER

#endif // QDRAWUTIL_H
