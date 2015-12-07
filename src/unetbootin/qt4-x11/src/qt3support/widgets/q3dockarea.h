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

#ifndef Q3DOCKAREA_H
#define Q3DOCKAREA_H

#include <QtGui/qwidget.h>
#include <QtCore/qlist.h>
#include <Qt3Support/q3dockwindow.h>
#include <QtGui/qlayout.h>
#include <QtCore/qpointer.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Qt3SupportLight)

#ifndef QT_NO_MAINWINDOW

class QBoxLayout;
class Q3DockAreaLayout;
class QMouseEvent;
class Q3DockWindowResizeHandle;
class Q3DockAreaPrivate;
class QTextStream;

class Q_COMPAT_EXPORT Q3DockAreaLayout : public QLayout
{
    Q_OBJECT
    friend class Q3DockArea;

public:
    Q3DockAreaLayout(QWidget* parent, Qt::Orientation o, QList<Q3DockWindow *> *wl, int space = -1, int margin = -1, const char *name = 0)
        : QLayout(parent), orient(o), dirty(true), dockWindows(wl), parentWidget(parent)
    {
        if (space != -1)
            setSpacing(space);
        if (margin != -1)
            setMargin(margin);
        setObjectName(QString::fromAscii(name));
        init();
    }
    ~Q3DockAreaLayout() {}

    void addItem(QLayoutItem *) {}
    bool hasHeightForWidth() const;
    int heightForWidth(int) const;
    int widthForHeight(int) const;
    QSize sizeHint() const;
    QSize minimumSize() const;
    QLayoutItem *itemAt(int) const;
    QLayoutItem *takeAt(int);
    int count() const;
    Qt::Orientations expandingDirections() const { return Qt::Orientations(0); }
    void invalidate();
    Qt::Orientation orientation() const { return orient; }
    QList<QRect> lineList() const { return lines; }
    QList<Q3DockWindow *> lineStarts() const { return ls; }

protected:
    void setGeometry(const QRect&);

private:
    Q_DISABLE_COPY(Q3DockAreaLayout)

    void init();
    int layoutItems(const QRect&, bool testonly = false);
    Qt::Orientation orient;
    bool dirty;
    int cached_width, cached_height;
    int cached_hfw, cached_wfh;
    QList<Q3DockWindow *> *dockWindows;
    QWidget *parentWidget;
    QList<QRect> lines;
    QList<Q3DockWindow *> ls;
};

class Q_COMPAT_EXPORT Q3DockArea : public QWidget
{
    Q_OBJECT
    Q_ENUMS(HandlePosition)
    Q_PROPERTY(Qt::Orientation orientation READ orientation)
    Q_PROPERTY(int count READ count)
    Q_PROPERTY(bool empty READ isEmpty)
    Q_PROPERTY(HandlePosition handlePosition READ handlePosition)

    friend class Q3DockWindow;
    friend class Q3DockWindowResizeHandle;
    friend class Q3DockAreaLayout;

public:
    enum HandlePosition { Normal, Reverse };

    Q3DockArea(Qt::Orientation o, HandlePosition h = Normal, QWidget* parent=0, const char* name=0);
    ~Q3DockArea();

    void moveDockWindow(Q3DockWindow *w, const QPoint &globalPos, const QRect &rect, bool swap);
    void removeDockWindow(Q3DockWindow *w, bool makeFloating, bool swap, bool fixNewLines = true);
    void moveDockWindow(Q3DockWindow *w, int index = -1);
    bool hasDockWindow(Q3DockWindow *w, int *index = 0);

    void invalidNextOffset(Q3DockWindow *dw);

    Qt::Orientation orientation() const { return orient; }
    HandlePosition handlePosition() const { return hPos; }

    bool eventFilter(QObject *, QEvent *);
    bool isEmpty() const;
    int count() const;
    QList<Q3DockWindow *> dockWindowList() const;

    bool isDockWindowAccepted(Q3DockWindow *dw);
    void setAcceptDockWindow(Q3DockWindow *dw, bool accept);

public Q_SLOTS:
    void lineUp(bool keepNewLines);

private:
    struct DockWindowData
    {
        int index;
        int offset;
        int line;
        QSize fixedExtent;
        QPointer<Q3DockArea> area;
    };

    int findDockWindow(Q3DockWindow *w);
    int lineOf(int index);
    DockWindowData *dockWindowData(Q3DockWindow *w);
    void dockWindow(Q3DockWindow *dockWindow, DockWindowData *data);
    void updateLayout();
    void invalidateFixedSizes();
    int maxSpace(int hint, Q3DockWindow *dw);
    void setFixedExtent(int d, Q3DockWindow *dw);
    bool isLastDockWindow(Q3DockWindow *dw);

private:
    Q_DISABLE_COPY(Q3DockArea)

    Qt::Orientation orient;
    QList<Q3DockWindow *> dockWindows;
    Q3DockAreaLayout *layout;
    HandlePosition hPos;
    QList<Q3DockWindow *> forbiddenWidgets;
    Q3DockAreaPrivate *d;
};

#ifndef QT_NO_TEXTSTREAM
Q_COMPAT_EXPORT QTextStream &operator<<(QTextStream &, const Q3DockArea &);
Q_COMPAT_EXPORT QTextStream &operator>>(QTextStream &, Q3DockArea &);
#endif

#endif // QT_NO_MAINWINDOW

QT_END_NAMESPACE

QT_END_HEADER

#endif // Q3DOCKAREA_H
