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

#ifndef QDYNAMICMAINWINDOW_H
#define QDYNAMICMAINWINDOW_H

#include <QtGui/qwidget.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Gui)

#ifndef QT_NO_MAINWINDOW

class QDockWidget;
class QMainWindowPrivate;
class QMenuBar;
class QStatusBar;
class QToolBar;
class QMenu;

class Q_GUI_EXPORT QMainWindow : public QWidget
{
    Q_OBJECT

    Q_ENUMS(DockOption)
    Q_FLAGS(DockOptions)
    Q_PROPERTY(QSize iconSize READ iconSize WRITE setIconSize)
    Q_PROPERTY(Qt::ToolButtonStyle toolButtonStyle READ toolButtonStyle WRITE setToolButtonStyle)
#ifndef QT_NO_DOCKWIDGET
    Q_PROPERTY(bool animated READ isAnimated WRITE setAnimated)
    Q_PROPERTY(bool dockNestingEnabled READ isDockNestingEnabled WRITE setDockNestingEnabled)
#endif
    Q_PROPERTY(DockOptions dockOptions READ dockOptions WRITE setDockOptions)
#ifndef QT_NO_TOOLBAR
    Q_PROPERTY(bool unifiedTitleAndToolBarOnMac READ unifiedTitleAndToolBarOnMac WRITE setUnifiedTitleAndToolBarOnMac)
#endif

public:
    enum DockOption {
        AnimatedDocks = 0x01,
        AllowNestedDocks = 0x02,
        AllowTabbedDocks = 0x04,
        ForceTabbedDocks = 0x08,  // implies AllowTabbedDocks, !AllowNestedDocks
        VerticalTabs = 0x10       // implies AllowTabbedDocks
    };
    Q_DECLARE_FLAGS(DockOptions, DockOption)

    explicit QMainWindow(QWidget *parent = 0, Qt::WindowFlags flags = 0);
    ~QMainWindow();

    QSize iconSize() const;
    void setIconSize(const QSize &iconSize);

    Qt::ToolButtonStyle toolButtonStyle() const;
    void setToolButtonStyle(Qt::ToolButtonStyle toolButtonStyle);

    bool isAnimated() const;
    bool isDockNestingEnabled() const;

    void setDockOptions(DockOptions options);
    DockOptions dockOptions() const;

    bool isSeparator(const QPoint &pos) const;

#ifndef QT_NO_MENUBAR
    QMenuBar *menuBar() const;
    void setMenuBar(QMenuBar *menubar);

    QWidget  *menuWidget() const;
    void setMenuWidget(QWidget *menubar);
#endif

#ifndef QT_NO_STATUSBAR
    QStatusBar *statusBar() const;
    void setStatusBar(QStatusBar *statusbar);
#endif

    QWidget *centralWidget() const;
    void setCentralWidget(QWidget *widget);

#ifndef QT_NO_DOCKWIDGET
    void setCorner(Qt::Corner corner, Qt::DockWidgetArea area);
    Qt::DockWidgetArea corner(Qt::Corner corner) const;
#endif

#ifndef QT_NO_TOOLBAR
    void addToolBarBreak(Qt::ToolBarArea area = Qt::TopToolBarArea);
    void insertToolBarBreak(QToolBar *before);

    void addToolBar(Qt::ToolBarArea area, QToolBar *toolbar);
    void addToolBar(QToolBar *toolbar);
    QToolBar *addToolBar(const QString &title);
    void insertToolBar(QToolBar *before, QToolBar *toolbar);
    void removeToolBar(QToolBar *toolbar);
    void removeToolBarBreak(QToolBar *before);

    void setUnifiedTitleAndToolBarOnMac(bool set);
    bool unifiedTitleAndToolBarOnMac() const;

    Qt::ToolBarArea toolBarArea(QToolBar *toolbar) const;
    bool toolBarBreak(QToolBar *toolbar) const;
#endif
#ifndef QT_NO_DOCKWIDGET
    void addDockWidget(Qt::DockWidgetArea area, QDockWidget *dockwidget);
    void addDockWidget(Qt::DockWidgetArea area, QDockWidget *dockwidget,
                       Qt::Orientation orientation);
    void splitDockWidget(QDockWidget *after, QDockWidget *dockwidget,
                         Qt::Orientation orientation);
    void tabifyDockWidget(QDockWidget *first, QDockWidget *second);
    void removeDockWidget(QDockWidget *dockwidget);
    bool restoreDockWidget(QDockWidget *dockwidget);

    Qt::DockWidgetArea dockWidgetArea(QDockWidget *dockwidget) const;
#endif // QT_NO_DOCKWIDGET

    QByteArray saveState(int version = 0) const;
    bool restoreState(const QByteArray &state, int version = 0);

#ifndef QT_NO_MENU
    virtual QMenu *createPopupMenu();
#endif

#ifdef QT3_SUPPORT
    QT3_SUPPORT_CONSTRUCTOR QMainWindow(QWidget *parent, const char *name, Qt::WindowFlags flags = 0);
#endif

#ifndef QT_NO_DOCKWIDGET
public Q_SLOTS:
    void setAnimated(bool enabled);
    void setDockNestingEnabled(bool enabled);
#endif

Q_SIGNALS:
    void iconSizeChanged(const QSize &iconSize);
    void toolButtonStyleChanged(Qt::ToolButtonStyle toolButtonStyle);

protected:
    void contextMenuEvent(QContextMenuEvent *event);
    bool event(QEvent *event);

private:
    Q_DECLARE_PRIVATE(QMainWindow)
    Q_DISABLE_COPY(QMainWindow)
};

Q_DECLARE_OPERATORS_FOR_FLAGS(QMainWindow::DockOptions)

#endif // QT_NO_MAINWINDOW

QT_END_NAMESPACE

QT_END_HEADER

#endif // QDYNAMICMAINWINDOW_H
