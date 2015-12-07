/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the Qt3Support module of the Qt Toolkit.
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
