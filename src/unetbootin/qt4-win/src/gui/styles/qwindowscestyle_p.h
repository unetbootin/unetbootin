/****************************************************************************
**
** Copyright (C) 2007-2008 Trolltech ASA. All rights reserved.
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

#ifndef QWINDOWSCE_P_H
#define QWINDOWSCE_P_H

#include "qwindowscestyle.h"
#include <private/qwindowsstyle_p.h>

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists for the convenience
// of qapplication_*.cpp, qwidget*.cpp and qfiledialog.cpp.  This header
// file may change from version to version without notice, or even be removed.
//
// We mean it.
//

QT_BEGIN_NAMESPACE

class QPainter;
class QPalette;
class QPoint;
class QColor;
class QBrush;
class QRect;

// Private class
class QWindowsCEStylePrivate : public QWindowsStylePrivate
{   
    Q_DECLARE_PUBLIC(QWindowsCEStyle)
public:
    inline QWindowsCEStylePrivate()
    { }

    
static void drawWinCEButton(QPainter *p, int x, int y, int w, int h,
                            const QPalette &pal, bool sunken = false,
                            const QBrush *fill = 0);

static void drawWinCEButton(QPainter *p, const QRect &r,
                            const QPalette &pal, bool sunken = false,
                            const QBrush *fill = 0);

static void drawWinCEPanel(QPainter *p, int x, int y, int w, int h,
                           const QPalette &pal, bool sunken = false,
                           const QBrush *fill = 0);

static void drawWinCEPanel(QPainter *p, const QRect &r,
                           const QPalette &pal, bool sunken = false,
                           const QBrush *fill = 0);

static void drawWinShades(QPainter *p,
                          int x, int y, int w, int h,
                          const QColor &c1, const QColor &c2,
                          const QColor &c3, const QColor &c4,
                          const QBrush *fill);

static void drawWinCEShades(QPainter *p,
                            int x, int y, int w, int h,
                            const QColor &c1, const QColor &c2,
                            const QColor &c3, const QColor &c4,
                            const QBrush *fill);

static void drawWinCEShadesSunken(QPainter *p,
                                  int x, int y, int w, int h,
                                  const QColor &c1, const QColor &c2,
                                  const QColor &c3, const QColor &c4,
                                  const QBrush *fill);




};

QT_END_NAMESPACE

#endif //QWINDOWSCE_P_H
