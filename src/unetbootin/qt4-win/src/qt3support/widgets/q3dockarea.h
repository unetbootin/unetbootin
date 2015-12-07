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
