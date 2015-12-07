/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the Qt3Support module of the Qt Toolkit.
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

#ifndef Q3MAINWINDOW_P_H
#define Q3MAINWINDOW_P_H

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

#include <private/qwidget_p.h>

QT_BEGIN_NAMESPACE

class Q3MainWindowLayout;

class Q3MainWindowPrivate : public QWidgetPrivate
{
    Q_DECLARE_PUBLIC(Q3MainWindow)
public:
    Q3MainWindowPrivate()
        :  mb(0), sb(0), ttg(0), mc(0), tll(0), mwl(0), ubp(false), utl(false),
           justify(false), movable(true), opaque(false), dockMenu(true)
    {
        docks.insert(Qt::DockTop, true);
        docks.insert(Qt::DockBottom, true);
        docks.insert(Qt::DockLeft, true);
        docks.insert(Qt::DockRight, true);
        docks.insert(Qt::DockMinimized, false);
        docks.insert(Qt::DockTornOff, true);
    }

    ~Q3MainWindowPrivate()
    {
    }

#ifndef QT_NO_MENUBAR
    mutable QMenuBar * mb;
#else
    mutable QWidget * mb;
#endif
    QStatusBar * sb;
    QToolTipGroup * ttg;

    QWidget * mc;

    QBoxLayout * tll;
    Q3MainWindowLayout * mwl;

    uint ubp :1;
    uint utl :1;
    uint justify :1;
    uint movable :1;
    uint opaque :1;
    uint dockMenu :1;

    Q3DockArea *topDock, *bottomDock, *leftDock, *rightDock;

    QList<Q3DockWindow *> dockWindows;
    QMap<Qt::Dock, bool> docks;
    QStringList disabledDocks;
    QHideDock *hideDock;

    QPointer<Q3PopupMenu> rmbMenu, tbMenu, dwMenu;
    QMap<Q3DockWindow*, bool> appropriate;
    mutable QMap<Q3PopupMenu*, Q3MainWindow::DockWindows> dockWindowModes;
};

QT_END_NAMESPACE

#endif // Q3MAINWINDOW_P_H
