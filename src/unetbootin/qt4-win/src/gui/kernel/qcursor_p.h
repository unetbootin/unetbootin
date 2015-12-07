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

#ifndef QCURSOR_P_H
#define QCURSOR_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists for the convenience
// of a number of Qt sources files.  This header file may change from
// version to version without notice, or even be removed.
//
// We mean it.
//

#include "QtCore/qatomic.h"
#include "QtCore/qglobal.h"
#include "QtCore/qnamespace.h"
#include "QtGui/qpixmap.h"

# if defined (Q_WS_MAC)
#  include "private/qt_mac_p.h"
# elif defined(Q_WS_X11)
#  include "private/qt_x11_p.h"
# elif defined(Q_WS_WIN)
#  include "QtCore/qt_windows.h"
#endif

QT_BEGIN_NAMESPACE

#if defined (Q_WS_MAC)
class QMacAnimateCursor;
#endif

class QBitmap;
struct QCursorData {
    QCursorData(Qt::CursorShape s = Qt::ArrowCursor);
    ~QCursorData();

    static void initialize();
    static void cleanup();

    QAtomicInt ref;
    Qt::CursorShape cshape;
    QBitmap  *bm, *bmm;
    QPixmap pixmap;
    short     hx, hy;
#if defined (Q_WS_MAC)
    int mId;
#elif defined(Q_WS_QWS)
    int id;
#endif
#if defined (Q_WS_WIN)
    HCURSOR hcurs;
#elif defined (Q_WS_X11)
    XColor fg, bg;
    Cursor hcurs;
    Pixmap pm, pmm;
#elif defined (Q_WS_MAC)
    enum { TYPE_None, TYPE_ImageCursor, TYPE_ThemeCursor } type;
    union {
        struct {
            uint my_cursor:1;
            void *nscursor;
        } cp;
        struct {
            QMacAnimateCursor *anim;
            ThemeCursor curs;
        } tc;
    } curs;
    void initCursorFromBitmap();
    void initCursorFromPixmap();
#endif
    static bool initialized;
    void update();
    static QCursorData *setBitmap(const QBitmap &bitmap, const QBitmap &mask, int hotX, int hotY);
};

QT_END_NAMESPACE

#endif // QCURSOR_P_H
