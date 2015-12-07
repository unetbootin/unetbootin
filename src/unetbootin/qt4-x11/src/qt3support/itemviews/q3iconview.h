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

#ifndef Q3ICONVIEW_H
#define Q3ICONVIEW_H

#include <Qt3Support/q3scrollview.h>
#include <QtCore/qstring.h>
#include <QtCore/qrect.h>
#include <QtCore/qpoint.h>
#include <QtCore/qsize.h>
#include <QtGui/qfont.h> // QString->QFont conversion
#include <Qt3Support/q3dragobject.h>
#include <QtGui/qbitmap.h>
#include <QtGui/qpicture.h>
#include <Qt3Support/q3valuelist.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Qt3SupportLight)

#ifndef QT_NO_ICONVIEW

class Q3IconView;
class QPainter;
class QMimeSource;
class QMouseEvent;
class QDragEnterEvent;
class QDragMoveEvent;
class QDragLeaveEvent;
class QKeyEvent;
class QFocusEvent;
class QShowEvent;
class Q3IconViewItem;
class Q3IconViewItemLineEdit;
class QStringList;
class Q3IconDragPrivate;
class QColorGroup;

#ifndef QT_NO_DRAGANDDROP

class Q_COMPAT_EXPORT Q3IconDragItem
{
public:
    Q3IconDragItem();
    virtual ~Q3IconDragItem();
    virtual QByteArray data() const;
    virtual void setData(const QByteArray &d);
    bool operator== (const Q3IconDragItem&) const;

private:
    QByteArray ba;

};

class Q_COMPAT_EXPORT Q3IconDrag : public Q3DragObject
{
    Q_OBJECT
public:
    Q3IconDrag(QWidget * dragSource, const char* name = 0);
    virtual ~Q3IconDrag();

    void append(const Q3IconDragItem &item, const QRect &pr, const QRect &tr);

    virtual const char* format(int i) const;
    static bool canDecode(QMimeSource* e);
    virtual QByteArray encodedData(const char* mime) const;

private:
    Q_DISABLE_COPY(Q3IconDrag)

    Q3IconDragPrivate *d;
    QChar endMark;

    friend class Q3IconView;
    friend class Q3IconViewPrivate;
};

#endif

class Q3IconViewToolTip;
class Q3IconViewItemPrivate;

class Q_COMPAT_EXPORT Q3IconViewItem
{
    friend class Q3IconView;
    friend class Q3IconViewToolTip;
    friend class Q3IconViewItemLineEdit;

public:
    Q3IconViewItem(Q3IconView *parent);
    Q3IconViewItem(Q3IconView *parent, Q3IconViewItem *after);
    Q3IconViewItem(Q3IconView *parent, const QString &text);
    Q3IconViewItem(Q3IconView *parent, Q3IconViewItem *after, const QString &text);
    Q3IconViewItem(Q3IconView *parent, const QString &text, const QPixmap &icon);
    Q3IconViewItem(Q3IconView *parent, Q3IconViewItem *after, const QString &text, const QPixmap &icon);
#ifndef QT_NO_PICTURE
    Q3IconViewItem(Q3IconView *parent, const QString &text, const QPicture &picture);
    Q3IconViewItem(Q3IconView *parent, Q3IconViewItem *after, const QString &text, const QPicture &picture);
#endif
    virtual ~Q3IconViewItem();

    virtual void setRenameEnabled(bool allow);
    virtual void setDragEnabled(bool allow);
    virtual void setDropEnabled(bool allow);

    virtual QString text() const;
    virtual QPixmap *pixmap() const;
#ifndef QT_NO_PICTURE
    virtual QPicture *picture() const;
#endif
    virtual QString key() const;

    bool renameEnabled() const;
    bool dragEnabled() const;
    bool dropEnabled() const;

    Q3IconView *iconView() const;
    Q3IconViewItem *prevItem() const;
    Q3IconViewItem *nextItem() const;

    int index() const;

    virtual void setSelected(bool s, bool cb);
    virtual void setSelected(bool s);
    virtual void setSelectable(bool s);

    bool isSelected() const;
    bool isSelectable() const;

    virtual void repaint();

    virtual bool move(int x, int y);
    virtual void moveBy(int dx, int dy);
    virtual bool move(const QPoint &pnt);
    virtual void moveBy(const QPoint &pnt);

    QRect rect() const;
    int x() const;
    int y() const;
    int width() const;
    int height() const;
    QSize size() const;
    QPoint pos() const;
    QRect textRect(bool relative = true) const;
    QRect pixmapRect(bool relative = true) const;
    bool contains(const QPoint& pnt) const;
    bool intersects(const QRect& r) const;

    virtual bool acceptDrop(const QMimeSource *mime) const;

#ifndef QT_NO_TEXTEDIT
    void rename();
#endif

    virtual int compare(Q3IconViewItem *i) const;

    virtual void setText(const QString &text);
    virtual void setPixmap(const QPixmap &icon);
#ifndef QT_NO_PICTURE
    virtual void setPicture(const QPicture &icon);
#endif
    virtual void setText(const QString &text, bool recalc, bool redraw = true);
    virtual void setPixmap(const QPixmap &icon, bool recalc, bool redraw = true);
    virtual void setKey(const QString &k);

    virtual int rtti() const;
    static int RTTI;

protected:
#ifndef QT_NO_TEXTEDIT
    virtual void removeRenameBox();
#endif
    virtual void calcRect(const QString &text_ = QString());
    virtual void paintItem(QPainter *p, const QColorGroup &cg);
    virtual void paintFocus(QPainter *p, const QColorGroup &cg);
#ifndef QT_NO_DRAGANDDROP
    virtual void dropped(QDropEvent *e, const Q3ValueList<Q3IconDragItem> &lst);
#endif
    virtual void dragEntered();
    virtual void dragLeft();
    void setItemRect(const QRect &r);
    void setTextRect(const QRect &r);
    void setPixmapRect(const QRect &r);
    void calcTmpText();
    QString tempText() const;

private:
    void init(Q3IconViewItem *after = 0
#ifndef QT_NO_PICTURE
               , QPicture *pic = 0
#endif
              );
#ifndef QT_NO_TEXTEDIT
    void renameItem();
    void cancelRenameItem();
#endif
    void checkRect();

    Q3IconView *view;
    QString itemText, itemKey;
    QString tmpText;
    QPixmap *itemIcon;
#ifndef QT_NO_PICTURE
    QPicture *itemPic;
#endif
    Q3IconViewItem *prev, *next;
    uint allow_rename : 1;
    uint allow_drag : 1;
    uint allow_drop : 1;
    uint selected : 1;
    uint selectable : 1;
    uint dirty : 1;
    uint wordWrapDirty : 1;
    QRect itemRect, itemTextRect, itemIconRect;
#ifndef QT_NO_TEXTEDIT
    Q3IconViewItemLineEdit *renameBox;
#endif
    QRect oldRect;

    Q3IconViewItemPrivate *d;

};

class Q3IconViewPrivate;

class Q_COMPAT_EXPORT Q3IconView : public Q3ScrollView
{
    friend class Q3IconViewItem;
    friend class Q3IconViewPrivate;
    friend class Q3IconViewToolTip;

    Q_OBJECT
    // #### sorting and sort direction do not work
    Q_ENUMS(SelectionMode ItemTextPos Arrangement ResizeMode)
    Q_PROPERTY(bool sorting READ sorting)
    Q_PROPERTY(bool sortDirection READ sortDirection)
    Q_PROPERTY(SelectionMode selectionMode READ selectionMode WRITE setSelectionMode)
    Q_PROPERTY(int gridX READ gridX WRITE setGridX)
    Q_PROPERTY(int gridY READ gridY WRITE setGridY)
    Q_PROPERTY(int spacing READ spacing WRITE setSpacing)
    Q_PROPERTY(ItemTextPos itemTextPos READ itemTextPos WRITE setItemTextPos)
    Q_PROPERTY(QBrush itemTextBackground READ itemTextBackground WRITE setItemTextBackground)
    Q_PROPERTY(Arrangement arrangement READ arrangement WRITE setArrangement)
    Q_PROPERTY(ResizeMode resizeMode READ resizeMode WRITE setResizeMode)
    Q_PROPERTY(int maxItemWidth READ maxItemWidth WRITE setMaxItemWidth)
    Q_PROPERTY(int maxItemTextLength READ maxItemTextLength WRITE setMaxItemTextLength)
    Q_PROPERTY(bool autoArrange READ autoArrange WRITE setAutoArrange)
    Q_PROPERTY(bool itemsMovable READ itemsMovable WRITE setItemsMovable)
    Q_PROPERTY(bool wordWrapIconText READ wordWrapIconText WRITE setWordWrapIconText)
    Q_PROPERTY(bool showToolTips READ showToolTips WRITE setShowToolTips)
    Q_PROPERTY(uint count READ count)

public:
    enum SelectionMode {
        Single = 0,
        Multi,
        Extended,
        NoSelection
    };
    enum Arrangement {
        LeftToRight = 0,
        TopToBottom
    };
    enum ResizeMode {
        Fixed = 0,
        Adjust
    };
    enum ItemTextPos {
        Bottom = 0,
        Right
    };

    Q3IconView(QWidget* parent=0, const char* name=0, Qt::WindowFlags f = 0);
    virtual ~Q3IconView();

    virtual void insertItem(Q3IconViewItem *item, Q3IconViewItem *after = 0L);
    virtual void takeItem(Q3IconViewItem *item);

    int index(const Q3IconViewItem *item) const;

    Q3IconViewItem *firstItem() const;
    Q3IconViewItem *lastItem() const;
    Q3IconViewItem *currentItem() const;
    virtual void setCurrentItem(Q3IconViewItem *item);
    virtual void setSelected(Q3IconViewItem *item, bool s, bool cb = false);

    uint count() const;

public:
    virtual void showEvent(QShowEvent *);

    virtual void setSelectionMode(SelectionMode m);
    SelectionMode selectionMode() const;

    Q3IconViewItem *findItem(const QPoint &pos) const;

    enum StringComparisonMode {
        CaseSensitive   = 0x00001, // 0 0001
        BeginsWith      = 0x00002, // 0 0010
        EndsWith        = 0x00004, // 0 0100
        Contains        = 0x00008, // 0 1000
        ExactMatch      = 0x00010  // 1 0000
    };
    typedef uint ComparisonFlags;
    Q3IconViewItem *findItem(const QString &text, ComparisonFlags = BeginsWith | Qt::CaseSensitive) const;
    virtual void selectAll(bool select);
    virtual void clearSelection();
    virtual void invertSelection();

    virtual void repaintItem(Q3IconViewItem *item);
    void repaintSelectedItems();

    void ensureItemVisible(Q3IconViewItem *item);
    Q3IconViewItem* findFirstVisibleItem(const QRect &r) const;
    Q3IconViewItem* findLastVisibleItem(const QRect &r) const;

    virtual void clear();

    virtual void setGridX(int rx);
    virtual void setGridY(int ry);
    int gridX() const;
    int gridY() const;
    virtual void setSpacing(int sp);
    int spacing() const;
    virtual void setItemTextPos(ItemTextPos pos);
    ItemTextPos itemTextPos() const;
    virtual void setItemTextBackground(const QBrush &b);
    QBrush itemTextBackground() const;
    virtual void setArrangement(Arrangement am);
    Arrangement arrangement() const;
    virtual void setResizeMode(ResizeMode am);
    ResizeMode resizeMode() const;
    virtual void setMaxItemWidth(int w);
    int maxItemWidth() const;
    virtual void setMaxItemTextLength(int w);
    int maxItemTextLength() const;
    virtual void setAutoArrange(bool b);
    bool autoArrange() const;
    virtual void setShowToolTips(bool b);
    bool showToolTips() const;

    void setSorting(bool sort, bool ascending = true);
    bool sorting() const;
    bool sortDirection() const;

    virtual void setItemsMovable(bool b);
    bool itemsMovable() const;
    virtual void setWordWrapIconText(bool b);
    bool wordWrapIconText() const;

    bool eventFilter(QObject * o, QEvent *);

    QSize minimumSizeHint() const;
    QSize sizeHint() const;

    virtual void sort(bool ascending = true);

    bool isRenaming() const;

    QVariant inputMethodQuery(Qt::InputMethodQuery query) const;

public Q_SLOTS:
    virtual void arrangeItemsInGrid(const QSize &grid, bool update = true);
    virtual void arrangeItemsInGrid(bool update = true);
    virtual void setContentsPos(int x, int y);
    virtual void updateContents();

Q_SIGNALS:
    void selectionChanged();
    void selectionChanged(Q3IconViewItem *item);
    void currentChanged(Q3IconViewItem *item);
    void clicked(Q3IconViewItem *);
    void clicked(Q3IconViewItem *, const QPoint &);
    void pressed(Q3IconViewItem *);
    void pressed(Q3IconViewItem *, const QPoint &);

    void doubleClicked(Q3IconViewItem *item);
    void returnPressed(Q3IconViewItem *item);
    void rightButtonClicked(Q3IconViewItem* item, const QPoint& pos);
    void rightButtonPressed(Q3IconViewItem* item, const QPoint& pos);
    void mouseButtonPressed(int button, Q3IconViewItem* item, const QPoint& pos);
    void mouseButtonClicked(int button, Q3IconViewItem* item, const QPoint& pos);
    void contextMenuRequested(Q3IconViewItem* item, const QPoint &pos);

#ifndef QT_NO_DRAGANDDROP
    void dropped(QDropEvent *e, const Q3ValueList<Q3IconDragItem> &lst);
#endif
    void moved();
    void onItem(Q3IconViewItem *item);
    void onViewport();
    void itemRenamed(Q3IconViewItem *item, const QString &);
    void itemRenamed(Q3IconViewItem *item);

protected Q_SLOTS:
    virtual void doAutoScroll();
    virtual void adjustItems();
    virtual void slotUpdate();

private Q_SLOTS:
    void movedContents(int dx, int dy);

protected:
    void drawContents(QPainter *p, int cx, int cy, int cw, int ch);
    void contentsMousePressEvent(QMouseEvent *e);
    void contentsMouseReleaseEvent(QMouseEvent *e);
    void contentsMouseMoveEvent(QMouseEvent *e);
    void contentsMouseDoubleClickEvent(QMouseEvent *e);
    void contentsContextMenuEvent(QContextMenuEvent *e);

#ifndef QT_NO_DRAGANDDROP
    void contentsDragEnterEvent(QDragEnterEvent *e);
    void contentsDragMoveEvent(QDragMoveEvent *e);
    void contentsDragLeaveEvent(QDragLeaveEvent *e);
    void contentsDropEvent(QDropEvent *e);
#endif

    void resizeEvent(QResizeEvent* e);
    void keyPressEvent(QKeyEvent *e);
    void focusInEvent(QFocusEvent *e);
    void focusOutEvent(QFocusEvent *e);
    void enterEvent(QEvent *e);

    virtual void drawRubber(QPainter *p);
#ifndef QT_NO_DRAGANDDROP
    virtual Q3DragObject *dragObject();
    virtual void startDrag();
#endif
    virtual void insertInGrid(Q3IconViewItem *item);
    virtual void drawBackground(QPainter *p, const QRect &r);

    void emitSelectionChanged(Q3IconViewItem * i = 0);
    void emitRenamed(Q3IconViewItem *item);

    Q3IconViewItem *makeRowLayout(Q3IconViewItem *begin, int &y, bool &changed);

    void changeEvent(QEvent *);

private:
    Q_DISABLE_COPY(Q3IconView)

    void contentsMousePressEventEx(QMouseEvent *e);
    virtual void drawDragShapes(const QPoint &pnt);
#ifndef QT_NO_DRAGANDDROP
    virtual void initDragEnter(QDropEvent *e);
#endif
    Q3IconViewItem* findItemByName(Q3IconViewItem *start);
    void handleItemChange(Q3IconViewItem *old, bool shift,
                           bool control, bool homeend = false);

    int calcGridNum(int w, int x) const;
    Q3IconViewItem *rowBegin(Q3IconViewItem *item) const;
    void updateItemContainer(Q3IconViewItem *item);
    void appendItemContainer();
    void rebuildContainers();
    enum Direction {
        DirUp = 0,
        DirDown,
        DirLeft,
        DirRight
    };
    Q3IconViewItem* findItem(Direction dir,
                             const QPoint &relativeTo,
                             const QRect &searchRect) const;
    bool neighbourItem(Direction dir,
                        const QPoint &relativeTo,
                        const Q3IconViewItem *item) const;
    QBitmap mask(QPixmap *pix) const;

    Q3IconViewPrivate *d;
};

#endif // QT_NO_ICONVIEW

QT_END_NAMESPACE

QT_END_HEADER

#endif // QICONVIEW_H
