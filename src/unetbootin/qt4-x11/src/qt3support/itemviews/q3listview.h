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

#ifndef Q3LISTVIEW_H
#define Q3LISTVIEW_H

#include <Qt3Support/q3scrollview.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Qt3SupportLight)

#ifndef QT_NO_LISTVIEW

class QPixmap;
class QFont;
class Q3Header;
class QIcon;

class Q3ListView;
struct Q3ListViewPrivate;
struct Q3CheckListItemPrivate;
class Q3ListViewItemIterator;
struct Q3ListViewItemIteratorPrivate;
class Q3DragObject;
class QMimeSource;
class QLineEdit;
class Q3ListViewToolTip;

class Q_COMPAT_EXPORT Q3ListViewItem
{
    friend class Q3ListViewItemIterator;
    friend class Q3ListViewToolTip;

public:
    Q3ListViewItem(Q3ListView * parent);
    Q3ListViewItem(Q3ListViewItem * parent);
    Q3ListViewItem(Q3ListView * parent, Q3ListViewItem * after);
    Q3ListViewItem(Q3ListViewItem * parent, Q3ListViewItem * after);

    Q3ListViewItem(Q3ListView * parent,
                   const QString&,             const QString& = QString(),
                   const QString& = QString(), const QString& = QString(),
                   const QString& = QString(), const QString& = QString(),
                   const QString& = QString(), const QString& = QString());
    Q3ListViewItem(Q3ListViewItem * parent,
                   const QString&,             const QString& = QString(),
                   const QString& = QString(), const QString& = QString(),
                   const QString& = QString(), const QString& = QString(),
                   const QString& = QString(), const QString& = QString());

    Q3ListViewItem(Q3ListView * parent, Q3ListViewItem * after,
                   const QString&,             const QString& = QString(),
                   const QString& = QString(), const QString& = QString(),
                   const QString& = QString(), const QString& = QString(),
                   const QString& = QString(), const QString& = QString());
    Q3ListViewItem(Q3ListViewItem * parent, Q3ListViewItem * after,
                   const QString&,             const QString& = QString(),
                   const QString& = QString(), const QString& = QString(),
                   const QString& = QString(), const QString& = QString(),
                   const QString& = QString(), const QString& = QString());
    virtual ~Q3ListViewItem();

    virtual void insertItem(Q3ListViewItem *);
    virtual void takeItem(Q3ListViewItem *);
    virtual void removeItem(Q3ListViewItem *item) { takeItem(item); }

    int height() const;
    virtual void invalidateHeight();
    int totalHeight() const;
    virtual int width(const QFontMetrics&,
                       const Q3ListView*, int column) const;
    void widthChanged(int column=-1) const;
    int depth() const;

    virtual void setText(int, const QString &);
    virtual QString text(int) const;

    virtual void setPixmap(int, const QPixmap &);
    virtual const QPixmap * pixmap(int) const;

    virtual QString key(int, bool) const;
    virtual int compare(Q3ListViewItem *i, int col, bool) const;
    virtual void sortChildItems(int, bool);

    int childCount() const { return nChildren; }

    bool isOpen() const { return open; }
    virtual void setOpen(bool);
    virtual void setup();

    virtual void setSelected(bool);
    bool isSelected() const { return selected; }

    virtual void paintCell(QPainter *, const QColorGroup & cg,
                            int column, int width, int alignment);
    virtual void paintBranches(QPainter * p, const QColorGroup & cg, int w, int y, int h);
    virtual void paintFocus(QPainter *, const QColorGroup & cg, const QRect & r);

    Q3ListViewItem * firstChild() const;
    Q3ListViewItem * nextSibling() const { return siblingItem; }
    Q3ListViewItem * parent() const;

    Q3ListViewItem * itemAbove() const;
    Q3ListViewItem * itemBelow() const;

    int itemPos() const;

    Q3ListView *listView() const;

    virtual void setSelectable(bool enable);
    bool isSelectable() const { return selectable && enabled; }

    virtual void setExpandable(bool);
    bool isExpandable() const { return expandable; }

    void repaint() const;

    virtual void sort();
    void moveItem(Q3ListViewItem *after);

    virtual void setDragEnabled(bool allow);
    virtual void setDropEnabled(bool allow);
    bool dragEnabled() const;
    bool dropEnabled() const;
    virtual bool acceptDrop(const QMimeSource *mime) const;

    void setVisible(bool b);
    bool isVisible() const;

    virtual void setRenameEnabled(int col, bool b);
    bool renameEnabled(int col) const;
    virtual void startRename(int col);

    virtual void setEnabled(bool b);
    bool isEnabled() const;

    virtual int rtti() const;
    enum { RTTI = 0 };

    virtual void setMultiLinesEnabled(bool b);
    bool multiLinesEnabled() const;

protected:
    virtual void enforceSortOrder() const;
    virtual void setHeight(int);
    virtual void activate();

    bool activatedPos(QPoint &);
#ifndef QT_NO_DRAGANDDROP
    virtual void dropped(QDropEvent *e);
#endif
    virtual void dragEntered();
    virtual void dragLeft();
    virtual void okRename(int col);
    virtual void cancelRename(int col);

    void ignoreDoubleClick();

private:
    void init();
    void moveToJustAfter(Q3ListViewItem *);
    void enforceSortOrderBackToRoot();
    void removeRenameBox();

    int ownHeight;
    int maybeTotalHeight;
    int nChildren;

    uint lsc: 14;
    uint lso: 1;
    uint open : 1;
    uint selected : 1;
    uint selectable: 1;
    uint configured: 1;
    uint expandable: 1;
    uint is_root: 1;
    uint allow_drag : 1;
    uint allow_drop : 1;
    uint visible : 1;
    uint enabled : 1;
    uint mlenabled : 1;

    Q3ListViewItem * parentItem;
    Q3ListViewItem * siblingItem;
    Q3ListViewItem * childItem;
    QLineEdit *renameBox;
    int renameCol;

    void * columns;

    friend class Q3ListView;
};

class Q3CheckListItem;

class Q_COMPAT_EXPORT Q3ListView : public Q3ScrollView
{
    friend class Q3ListViewItemIterator;
    friend class Q3ListViewItem;
    friend class Q3CheckListItem;
    friend class Q3ListViewToolTip;

    Q_OBJECT
    Q_ENUMS(SelectionMode ResizeMode RenameAction)
    Q_PROPERTY(int columns READ columns)
    Q_PROPERTY(bool multiSelection READ isMultiSelection WRITE setMultiSelection DESIGNABLE false)
    Q_PROPERTY(SelectionMode selectionMode READ selectionMode WRITE setSelectionMode)
    Q_PROPERTY(int childCount READ childCount)
    Q_PROPERTY(bool allColumnsShowFocus READ allColumnsShowFocus WRITE setAllColumnsShowFocus)
    Q_PROPERTY(bool showSortIndicator READ showSortIndicator WRITE setShowSortIndicator)
    Q_PROPERTY(int itemMargin READ itemMargin WRITE setItemMargin)
    Q_PROPERTY(bool rootIsDecorated READ rootIsDecorated WRITE setRootIsDecorated)
    Q_PROPERTY(bool showToolTips READ showToolTips WRITE setShowToolTips)
    Q_PROPERTY(ResizeMode resizeMode READ resizeMode WRITE setResizeMode)
    Q_PROPERTY(int treeStepSize READ treeStepSize WRITE setTreeStepSize)
    Q_PROPERTY(RenameAction defaultRenameAction READ defaultRenameAction WRITE setDefaultRenameAction)

public:
    Q3ListView(QWidget* parent=0, const char* name=0, Qt::WindowFlags f = 0);
    ~Q3ListView();

    int treeStepSize() const;
    virtual void setTreeStepSize(int);

    virtual void insertItem(Q3ListViewItem *);
    virtual void takeItem(Q3ListViewItem *);
    virtual void removeItem(Q3ListViewItem *item) { takeItem(item); }

    Q3Header * header() const;

    virtual int addColumn(const QString &label, int size = -1);
    virtual int addColumn(const QIcon& icon, const QString &label, int size = -1);
    virtual void removeColumn(int index);
    virtual void setColumnText(int column, const QString &label);
    virtual void setColumnText(int column, const QIcon& icon, const QString &label);
    QString columnText(int column) const;
    virtual void setColumnWidth(int column, int width);
    int columnWidth(int column) const;
    enum WidthMode { Manual, Maximum };
    virtual void setColumnWidthMode(int column, WidthMode);
    WidthMode columnWidthMode(int column) const;
    int columns() const;

    virtual void setColumnAlignment(int, int);
    int columnAlignment(int) const;

    void show();

    QVariant inputMethodQuery(Qt::InputMethodQuery query) const;
    Q3ListViewItem * itemAt(const QPoint & screenPos) const;
    QRect itemRect(const Q3ListViewItem *) const;
    int itemPos(const Q3ListViewItem *);

    void ensureItemVisible(const Q3ListViewItem *);

    void repaintItem(const Q3ListViewItem *) const;

    virtual void setMultiSelection(bool enable);
    bool isMultiSelection() const;

    enum SelectionMode { Single, Multi, Extended, NoSelection  };
    void setSelectionMode(SelectionMode mode);
    SelectionMode selectionMode() const;

    virtual void clearSelection();
    virtual void setSelected(Q3ListViewItem *, bool);
    void setSelectionAnchor(Q3ListViewItem *);
    bool isSelected(const Q3ListViewItem *) const;
    Q3ListViewItem * selectedItem() const;
    virtual void setOpen(Q3ListViewItem *, bool);
    bool isOpen(const Q3ListViewItem *) const;

    virtual void setCurrentItem(Q3ListViewItem *);
    Q3ListViewItem * currentItem() const;

    Q3ListViewItem * firstChild() const;
    Q3ListViewItem * lastItem() const;

    int childCount() const;

    virtual void setAllColumnsShowFocus(bool);
    bool allColumnsShowFocus() const;

    virtual void setItemMargin(int);
    int itemMargin() const;

    virtual void setRootIsDecorated(bool);
    bool rootIsDecorated() const;

    virtual void setSorting(int column, bool ascending = true);
    int sortColumn() const;
    void setSortColumn(int column);
    Qt::SortOrder sortOrder() const;
    void setSortOrder(Qt::SortOrder order);
    virtual void sort();

    bool eventFilter(QObject * o, QEvent *);

    QSize sizeHint() const;
    QSize minimumSizeHint() const;

    virtual void setShowSortIndicator(bool show);
    bool showSortIndicator() const;
    virtual void setShowToolTips(bool b);
    bool showToolTips() const;

    enum ResizeMode { NoColumn, AllColumns, LastColumn };
    virtual void setResizeMode(ResizeMode m);
    ResizeMode resizeMode() const;

    enum StringComparisonMode {
        CaseSensitive   = 0x00001, // 0 0001
        BeginsWith      = 0x00002, // 0 0010
        EndsWith        = 0x00004, // 0 0100
        Contains        = 0x00008, // 0 1000
        ExactMatch      = 0x00010  // 1 0000
    };
    typedef uint ComparisonFlags;
    Q3ListViewItem * findItem(const QString& text, int column,
                              ComparisonFlags = ExactMatch | Qt::CaseSensitive ) const;

    enum RenameAction { Accept, Reject };
    virtual void setDefaultRenameAction(RenameAction a);
    RenameAction defaultRenameAction() const;
    bool isRenaming() const;

    void hideColumn(int column);

public Q_SLOTS:
    virtual void clear();
    virtual void invertSelection();
    virtual void selectAll(bool select);
    void triggerUpdate();
    void setContentsPos(int x, int y);
    void adjustColumn(int col);

Q_SIGNALS:
    void selectionChanged();
    void selectionChanged(Q3ListViewItem *);
    void currentChanged(Q3ListViewItem *);
    void clicked(Q3ListViewItem *);
    void clicked(Q3ListViewItem *, const QPoint &, int);
    void pressed(Q3ListViewItem *);
    void pressed(Q3ListViewItem *, const QPoint &, int);

    void doubleClicked(Q3ListViewItem *);
    void doubleClicked(Q3ListViewItem *, const QPoint&, int);
    void returnPressed(Q3ListViewItem *);
    void spacePressed(Q3ListViewItem *);
    void rightButtonClicked(Q3ListViewItem *, const QPoint&, int);
    void rightButtonPressed(Q3ListViewItem *, const QPoint&, int);
    void mouseButtonPressed(int, Q3ListViewItem *, const QPoint& , int);
    void mouseButtonClicked(int, Q3ListViewItem *, const QPoint&, int);

    void contextMenuRequested(Q3ListViewItem *, const QPoint &, int);

    void onItem(Q3ListViewItem *item);
    void onViewport();

    void expanded(Q3ListViewItem *item);
    void collapsed(Q3ListViewItem *item);
#ifndef QT_NO_DRAGANDDROP
    void dropped(QDropEvent *e);
#endif
    void itemRenamed(Q3ListViewItem *item, int col, const QString &);
    void itemRenamed(Q3ListViewItem *item, int col );

protected:
    void contentsMousePressEvent(QMouseEvent * e);
    void contentsMouseReleaseEvent(QMouseEvent * e);
    void contentsMouseMoveEvent(QMouseEvent * e);
    void contentsMouseDoubleClickEvent(QMouseEvent * e);
    void contentsContextMenuEvent(QContextMenuEvent * e);
#ifndef QT_NO_DRAGANDDROP
    void contentsDragEnterEvent(QDragEnterEvent *e);
    void contentsDragMoveEvent(QDragMoveEvent *e);
    void contentsDragLeaveEvent(QDragLeaveEvent *e);
    void contentsDropEvent(QDropEvent *e);
    virtual Q3DragObject *dragObject();
    virtual void startDrag();
#endif

    void focusInEvent(QFocusEvent * e);
    void focusOutEvent(QFocusEvent * e);

    void keyPressEvent(QKeyEvent *e);

    void resizeEvent(QResizeEvent *e);
    void viewportResizeEvent(QResizeEvent *e);

    void showEvent(QShowEvent *);

    void drawContentsOffset(QPainter *, int ox, int oy,
                             int cx, int cy, int cw, int ch);

    virtual void paintEmptyArea(QPainter *, const QRect &);
    void changeEvent(QEvent *);

protected Q_SLOTS:
    void updateContents();
    void doAutoScroll();

private Q_SLOTS:
    void changeSortColumn(int);
    void handleIndexChange();
    void updateDirtyItems();
    void makeVisible();
    void handleSizeChange(int, int, int);
    void startRename();
    void openFocusItem();

private:
    Q_DISABLE_COPY(Q3ListView)

    void contentsMousePressEventEx(QMouseEvent * e);
    void contentsMouseReleaseEventEx(QMouseEvent * e);
    void init();
    void updateGeometries();
    void buildDrawableList() const;
    void reconfigureItems();
    void widthChanged(const Q3ListViewItem*, int c);
    void handleItemChange(Q3ListViewItem *old, bool shift, bool control);
    void selectRange(Q3ListViewItem *from, Q3ListViewItem *to, bool invert, bool includeFirst, bool clearSel = false);
    bool selectRange(Q3ListViewItem *newItem, Q3ListViewItem *oldItem, Q3ListViewItem *anchorItem);
    bool clearRange(Q3ListViewItem *from, Q3ListViewItem *to, bool includeFirst = true);
    void doAutoScroll(const QPoint &cursorPos);

    Q3ListViewPrivate *d;
};


class Q_COMPAT_EXPORT Q3CheckListItem : public Q3ListViewItem
{
public:
    enum Type { RadioButton,
                CheckBox,
                Controller,
                RadioButtonController=Controller,
                CheckBoxController };

    enum ToggleState { Off, NoChange, On };

    Q3CheckListItem(Q3CheckListItem *parent, const QString &text,
                    Type = RadioButtonController);
    Q3CheckListItem(Q3CheckListItem *parent, Q3ListViewItem *after,
                    const QString &text, Type = RadioButtonController);
    Q3CheckListItem(Q3ListViewItem *parent, const QString &text,
                    Type = RadioButtonController);
    Q3CheckListItem(Q3ListViewItem *parent, Q3ListViewItem *after,
                    const QString &text, Type = RadioButtonController);
    Q3CheckListItem(Q3ListView *parent, const QString &text,
                    Type = RadioButtonController);
    Q3CheckListItem(Q3ListView *parent, Q3ListViewItem *after,
                    const QString &text, Type = RadioButtonController);
    Q3CheckListItem(Q3ListViewItem *parent, const QString &text,
                    const QPixmap &);
    Q3CheckListItem(Q3ListView *parent, const QString &text,
                    const QPixmap &);
    ~Q3CheckListItem();

    void paintCell(QPainter *, const QColorGroup & cg,
                    int column, int width, int alignment);
    virtual void paintFocus(QPainter *, const QColorGroup &cg,
                             const QRect & r);
    int width(const QFontMetrics&, const Q3ListView*, int column) const;
    void setup();

    virtual void setOn(bool);
    bool isOn() const { return on; }
    Type type() const { return myType; }
    QString text() const { return Q3ListViewItem::text(0); }
    QString text(int n) const { return Q3ListViewItem::text(n); }

    void setTristate(bool);
    bool isTristate() const;
    ToggleState state() const;
    void setState(ToggleState s);

    int rtti() const;
    enum { RTTI = 1 };

protected:
    void activate();
    void turnOffChild();
    virtual void stateChange(bool);

private:
    void init();
    ToggleState internalState() const;
    void setStoredState(ToggleState newState, Q3CheckListItem *key);
    ToggleState storedState(Q3CheckListItem *key) const;
    void stateChange(ToggleState s);
    void restoreState(Q3CheckListItem *key, int depth = 0);
    void updateController(bool update = true , bool store = false);
    void updateStoredState(Q3CheckListItem *key);
    void setState(ToggleState s, bool update, bool store);
    void setCurrentState(ToggleState s);

    Type myType;
    bool on;
    Q3CheckListItemPrivate *d;
};

class Q_COMPAT_EXPORT Q3ListViewItemIterator
{
    friend struct Q3ListViewPrivate;
    friend class Q3ListView;
    friend class Q3ListViewItem;

public:
    enum IteratorFlag {
        Visible =                 0x00000001,
        Invisible =                 0x00000002,
        Selected =                0x00000004,
        Unselected =                 0x00000008,
        Selectable =                0x00000010,
        NotSelectable =                0x00000020,
        DragEnabled =                0x00000040,
        DragDisabled =                0x00000080,
        DropEnabled =                0x00000100,
        DropDisabled =                0x00000200,
        Expandable =                0x00000400,
        NotExpandable =                0x00000800,
        Checked =                0x00001000,
        NotChecked =                0x00002000
    };

    Q3ListViewItemIterator();
    Q3ListViewItemIterator(Q3ListViewItem *item);
    Q3ListViewItemIterator(Q3ListViewItem *item, int iteratorFlags);

    Q3ListViewItemIterator(const Q3ListViewItemIterator &it);
    Q3ListViewItemIterator(Q3ListView *lv);
    Q3ListViewItemIterator(Q3ListView *lv, int iteratorFlags);

    Q3ListViewItemIterator &operator=(const Q3ListViewItemIterator &it);

    ~Q3ListViewItemIterator();

    Q3ListViewItemIterator &operator++();
    const Q3ListViewItemIterator operator++(int);
    Q3ListViewItemIterator &operator+=(int j);

    Q3ListViewItemIterator &operator--();
    const Q3ListViewItemIterator operator--(int);
    Q3ListViewItemIterator &operator-=(int j);

    Q3ListViewItem* operator*();
    Q3ListViewItem *current() const;

private:
    Q3ListViewItem *curr;
    Q3ListView *listView;
    int flags;

    void currentRemoved();
    bool matchesFlags(const Q3ListViewItem*) const;
    bool testPair(Q3ListViewItemIterator::IteratorFlag, Q3ListViewItemIterator::IteratorFlag, bool) const;
    bool isChecked(const Q3ListViewItem*) const;
};

#endif // QT_NO_LISTVIEW

QT_END_NAMESPACE

QT_END_HEADER

#endif // Q3LISTVIEW_H
