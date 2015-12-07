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

#ifndef Q3MAINWINDOW_H
#define Q3MAINWINDOW_H

#include <QtGui/qwidget.h>
#include <Qt3Support/q3toolbar.h>
#include <QtCore/qtextstream.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Qt3SupportLight)

#ifndef QT_NO_MAINWINDOW

class QMenuBar;
class QStatusBar;
class QToolTipGroup;
class Q3MainWindowPrivate;
class Q3MainWindowLayout;
class Q3PopupMenu;
class QHideDock;
template<class T> class QList;

class Q_COMPAT_EXPORT Q3MainWindow: public QWidget
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(Q3MainWindow)

    Q_PROPERTY(bool rightJustification READ rightJustification WRITE setRightJustification DESIGNABLE false)
    Q_PROPERTY(bool usesBigPixmaps READ usesBigPixmaps WRITE setUsesBigPixmaps)
    Q_PROPERTY(bool usesTextLabel READ usesTextLabel WRITE setUsesTextLabel)
    Q_PROPERTY(bool dockWindowsMovable READ dockWindowsMovable WRITE setDockWindowsMovable)
    Q_PROPERTY(bool opaqueMoving READ opaqueMoving WRITE setOpaqueMoving)

public:
    Q3MainWindow(QWidget* parent=0, const char* name=0, Qt::WindowFlags f = Qt::WType_TopLevel);
    ~Q3MainWindow();

#ifndef QT_NO_MENUBAR
    QMenuBar * menuBar() const;
#endif
    QStatusBar * statusBar() const;
#if 0
    QToolTipGroup * toolTipGroup() const;
#endif

    virtual void setCentralWidget(QWidget *);
    QWidget * centralWidget() const;

    virtual void setDockEnabled(Qt::Dock dock, bool enable);
    bool isDockEnabled(Qt::Dock dock) const;
    bool isDockEnabled(Q3DockArea *area) const;
    virtual void setDockEnabled(Q3DockWindow *tb, Qt::Dock dock, bool enable);
    bool isDockEnabled(Q3DockWindow *tb, Qt::Dock dock) const;
    bool isDockEnabled(Q3DockWindow *tb, Q3DockArea *area) const;

    virtual void addDockWindow(Q3DockWindow *, Qt::Dock = Qt::DockTop, bool newLine = false);
    virtual void addDockWindow(Q3DockWindow *, const QString &label,
                                Qt::Dock = Qt::DockTop, bool newLine = false);
    virtual void moveDockWindow(Q3DockWindow *, Qt::Dock = Qt::DockTop);
    virtual void moveDockWindow(Q3DockWindow *, Qt::Dock, bool nl, int index, int extraOffset = -1);
    virtual void removeDockWindow(Q3DockWindow *);

    void setVisible(bool);
    QSize sizeHint() const;
    QSize minimumSizeHint() const;

    bool rightJustification() const;
    bool usesBigPixmaps() const;
    bool usesTextLabel() const;
    bool dockWindowsMovable() const;
    bool opaqueMoving() const;

    bool eventFilter(QObject*, QEvent*);

    bool getLocation(Q3DockWindow *tb, Qt::Dock &dock, int &index, bool &nl, int &extraOffset) const;

    QList<Q3DockWindow *> dockWindows(Qt::Dock dock) const;
    QList<Q3DockWindow *> dockWindows() const;
    void lineUpDockWindows(bool keepNewLines = false);

    bool isDockMenuEnabled() const;

    // compatibility stuff
    bool hasDockWindow(Q3DockWindow *dw);
#ifndef QT_NO_TOOLBAR
    void addToolBar(Q3DockWindow *, Qt::Dock = Qt::DockTop, bool newLine = false);
    void addToolBar(Q3DockWindow *, const QString &label,
                     Qt::Dock = Qt::DockTop, bool newLine = false);
    void moveToolBar(Q3DockWindow *, Qt::Dock = Qt::DockTop);
    void moveToolBar(Q3DockWindow *, Qt::Dock, bool nl, int index, int extraOffset = -1);
    void removeToolBar(Q3DockWindow *);

    bool toolBarsMovable() const;
    QList<Q3ToolBar *> toolBars(Qt::Dock dock) const;
    void lineUpToolBars(bool keepNewLines = false);
#endif

    virtual Q3DockArea *dockingArea(const QPoint &p);
    Q3DockArea *leftDock() const;
    Q3DockArea *rightDock() const;
    Q3DockArea *topDock() const;
    Q3DockArea *bottomDock() const;

    virtual bool isCustomizable() const;

    bool appropriate(Q3DockWindow *dw) const;

    enum DockWindows { OnlyToolBars, NoToolBars, AllDockWindows };
    virtual Q3PopupMenu *createDockWindowMenu(DockWindows dockWindows = AllDockWindows) const;

public Q_SLOTS:
    virtual void setRightJustification(bool);
    virtual void setUsesBigPixmaps(bool);
    virtual void setUsesTextLabel(bool);
    virtual void setDockWindowsMovable(bool);
    virtual void setOpaqueMoving(bool);
    virtual void setDockMenuEnabled(bool);
    virtual void whatsThis();
    virtual void setAppropriate(Q3DockWindow *dw, bool a);
    virtual void customize();

    // compatibility stuff
    void setToolBarsMovable(bool);

Q_SIGNALS:
    void pixmapSizeChanged(bool);
    void usesTextLabelChanged(bool);
    void dockWindowPositionChanged(Q3DockWindow *);

#ifndef QT_NO_TOOLBAR
    // compatibility stuff
    void toolBarPositionChanged(Q3ToolBar *);
#endif

protected Q_SLOTS:
    virtual void setUpLayout();
    virtual bool showDockMenu(const QPoint &globalPos);
    void menuAboutToShow();

protected:
    void paintEvent(QPaintEvent *);
    void childEvent(QChildEvent *);
    bool event(QEvent *);

private Q_SLOTS:
    void slotPlaceChanged();
    void doLineUp() { lineUpDockWindows(true); }

private:
    Q_DISABLE_COPY(Q3MainWindow)

    void triggerLayout(bool deleteLayout = true);
    bool dockMainWindow(QObject *dock) const;

#ifndef QT_NO_MENUBAR
    virtual void setMenuBar(QMenuBar *);
#endif
    virtual void setStatusBar(QStatusBar *);
#if 0
    virtual void setToolTipGroup(QToolTipGroup *);
#endif

    friend class Q3DockWindow;
    friend class QMenuBarPrivate;
    friend class QHideDock;
    friend class Q3ToolBar;
    friend class Q3MainWindowLayout;
};

#ifndef QT_NO_TOOLBAR
inline void Q3MainWindow::addToolBar(Q3DockWindow *w, Qt::ToolBarDock dock, bool newLine)
{
    addDockWindow(w, dock, newLine);
}

inline void Q3MainWindow::addToolBar(Q3DockWindow *w, const QString &label,
                              Qt::ToolBarDock dock, bool newLine)
{
    addDockWindow(w, label, dock, newLine);
}

inline void Q3MainWindow::moveToolBar(Q3DockWindow *w, Qt::ToolBarDock dock)
{
    moveDockWindow(w, dock);
}

inline void Q3MainWindow::moveToolBar(Q3DockWindow *w, Qt::ToolBarDock dock, bool nl, int index, int extraOffset)
{
    moveDockWindow(w, dock, nl, index, extraOffset);
}

inline void Q3MainWindow::removeToolBar(Q3DockWindow *w)
{
    removeDockWindow(w);
}

inline bool Q3MainWindow::toolBarsMovable() const
{
    return dockWindowsMovable();
}

inline void Q3MainWindow::lineUpToolBars(bool keepNewLines)
{
    lineUpDockWindows(keepNewLines);
}

inline void Q3MainWindow::setToolBarsMovable(bool b)
{
    setDockWindowsMovable(b);
}
#endif

#ifndef QT_NO_TEXTSTREAM
Q_COMPAT_EXPORT QTextStream &operator<<(QTextStream &, const Q3MainWindow &);
Q_COMPAT_EXPORT QTextStream &operator>>(QTextStream &, Q3MainWindow &);
#endif

#endif // QT_NO_MAINWINDOW

QT_END_NAMESPACE

QT_END_HEADER

#endif // Q3MAINWINDOW_H
