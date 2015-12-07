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

#include <qplatformdefs.h>
#include "q3listview.h"
#ifndef QT_NO_LISTVIEW
#include "q3tl.h"
#include "qapplication.h"
#include "qbitmap.h"
#include "q3cleanuphandler.h"
#include "qcursor.h"
#include "qdatetime.h"
#include "q3dragobject.h"
#include "qevent.h"
#include "qhash.h"
#include "q3header.h"
#include "qicon.h"
#include "qlineedit.h"
#include "qpainter.h"
#include "qpixmapcache.h"
#include "qstack.h"
#include "qstyle.h"
#include "qstyleoption.h"
#include "qtimer.h"
#include "qtooltip.h"
#include "qdebug.h"
#ifndef QT_NO_ACCESSIBILITY
#include "qaccessible.h"
#endif

QT_BEGIN_NAMESPACE

const int Unsorted = 16383;

static Q3CleanupHandler<QBitmap> qlv_cleanup_bitmap;


struct Q3ListViewPrivate
{
    // classes that are here to avoid polluting the global name space

    // the magical hidden mother of all items
    class Root: public Q3ListViewItem {
    public:
        Root(Q3ListView * parent);

        void setHeight(int);
        void invalidateHeight();
        void setup();
        Q3ListView * theListView() const;

        Q3ListView * lv;
    };

    // to remember what's on screen
    class DrawableItem {
    public:
        DrawableItem() {}
        DrawableItem(int level, int ypos, Q3ListViewItem * item)
            : l(level), y(ypos), i(item) {};
        int l;
        int y;
        Q3ListViewItem * i;
    };

    // for sorting
    class SortableItem {
    public:
        /*
          We could be smarter and keep a pointer to the Q3ListView
          item instead of numCols, col and asc. This would then allow
          us to use the physical ordering of columns rather than the
          logical. Microsoft uses the logical ordering, so there is
          some virtue in doing so, although it prevents the user from
          choosing the secondary key.
        */
        Q3ListViewItem * item;
        int numCols;
        int col;
        bool asc;

        int cmp(const SortableItem& i) const {
            int diff = item->compare(i.item, col, asc);
            if (diff == 0 && numCols != 1) {
                for (int j = 0; j < numCols; j++) {
                    if (j != col) {
                        diff = item->compare(i.item, j, asc);
                        if (diff != 0)
                            break;
                    }
                }
            }
            return diff;
        }
        bool operator<(const SortableItem& i) const { return cmp(i) < 0; }
        bool operator<=(const SortableItem& i) const { return cmp(i) <= 0; }
        bool operator>(const SortableItem& i) const { return cmp(i) > 0; }
    };

    class ItemColumnInfo {
    public:
        ItemColumnInfo(): pm(0), next(0), truncated(false), dirty(false), allow_rename(false), width(0) {}
        ~ItemColumnInfo() { delete pm; delete next; }
        QString text, tmpText;
        QPixmap * pm;
        ItemColumnInfo * next;
        uint truncated : 1;
        uint dirty : 1;
        uint allow_rename : 1;
        int width;
    };

    class ViewColumnInfo {
    public:
        ViewColumnInfo(): align(Qt::AlignAuto), sortable(true), next(0) {}
        ~ViewColumnInfo() { delete next; }
        int align;
        bool sortable;
        ViewColumnInfo * next;
    };

    // private variables used in Q3ListView
    ViewColumnInfo * vci;
    Q3Header * h;
    Root * r;
    uint rootIsExpandable : 1;
    int margin;

    Q3ListViewItem * focusItem, *highlighted, *oldFocusItem;

    QTimer * timer;
    QTimer * dirtyItemTimer;
    QTimer * visibleTimer;
    int levelWidth;

    // the list of drawables, and the range drawables covers entirely
    // (it may also include a few items above topPixel)
    QList<DrawableItem> drawables;
    int topPixel;
    int bottomPixel;

    QList<const Q3ListViewItem *> dirtyItems;

    Q3ListView::SelectionMode selectionMode;

    // Per-column structure for information not in the Q3Header
    struct Column {
        Q3ListView::WidthMode wmode;
    };
    QVector<Column> column;

    // suggested height for the items
    int fontMetricsHeight;
    int minLeftBearing, minRightBearing;
    int ellipsisWidth;

    // currently typed prefix for the keyboard interface, and the time
    // of the last key-press
    QString currentPrefix;
    QTime currentPrefixTime;

    // holds a list of iterators
    QList<Q3ListViewItemIterator *> iterators;
    Q3ListViewItem *pressedItem, *selectAnchor;

    QTimer *scrollTimer;
    QTimer *renameTimer;
    QTimer *autoopenTimer;

    // sort column and order   #### may need to move to Q3Header [subclass]
    int sortcolumn;
    bool ascending                :1;
    bool sortIndicator                :1;
    // whether to select or deselect during this mouse press.
    bool allColumnsShowFocus        :1;
    bool select                        :1;

    // true if the widget should take notice of mouseReleaseEvent
    bool buttonDown                :1;
    // true if the widget should ignore a double-click
    bool ignoreDoubleClick        :1;

    bool clearing                :1;
    bool pressedSelected        :1;
    bool pressedEmptyArea         :1;

    bool toolTips                :1;
    bool fullRepaintOnComlumnChange:1;
    bool updateHeader                :1;

    bool startEdit : 1;
    bool ignoreEditAfterFocus : 1;
    bool inMenuMode :1;

    Q3ListView::RenameAction defRenameAction;

    Q3ListViewItem *startDragItem;
    QPoint dragStartPos;
    int pressedColumn;
    Q3ListView::ResizeMode resizeMode;
};

Q_DECLARE_TYPEINFO(Q3ListViewPrivate::DrawableItem, Q_PRIMITIVE_TYPE);

// these should probably be in Q3ListViewPrivate, for future thread safety
static bool activatedByClick;
static QPoint activatedP;

#ifndef QT_NO_ACCESSIBILITY
static int indexOfItem(Q3ListViewItem *item)
{
    if (!QAccessible::isActive())
        return 0;

    static Q3ListViewItem *lastItem = 0;
    static int lastIndex = 0;

    if (!item || !item->listView())
        return 0;

    if (item == lastItem)
        return lastIndex;

    lastItem = item;
    int index = 1;

    Q3ListViewItemIterator it(item->listView());
    while (it.current()) {
        if (it.current() == item) {
            lastIndex = index;
            return index;
        }
        ++it;
        ++index;
    }
    lastIndex = 0;
    return 0;
}
#endif

/*!
    Creates a string with ... like "Trollte..." or "...olltech", depending on the alignment.
*/
static QString qEllipsisText(const QString &org, const QFontMetrics &fm, int width, int align)
{
    int ellWidth = fm.width(QLatin1String("..."));
    QString text = QString::fromLatin1("");
    int i = 0;
    int len = org.length();
    int offset = (align & Qt::AlignRight) ? (len-1) - i : i;
    while (i < len && fm.width(text + org[offset]) + ellWidth < width) {
        if (align & Qt::AlignRight)
            text.prepend(org[offset]);
        else
            text += org[offset];
        offset = (align & Qt::AlignRight) ? (len-1) - ++i : ++i;
    }
    if (text.isEmpty())
        text = (align & Qt::AlignRight) ? org.right(1) : text = org.left(1);
    if (align & Qt::AlignRight)
        text.prepend(QLatin1String("..."));
    else
        text += QLatin1String("...");
    return text;
}

/*!
    \class Q3ListViewItem
    \brief The Q3ListViewItem class implements a list view item.

    \compat

    A list view item is a multi-column object capable of displaying
    itself in a Q3ListView.

    The easiest way to use Q3ListViewItem is to construct one with a
    few constant strings, and either a Q3ListView or another
    Q3ListViewItem as parent.
    \snippet doc/src/snippets/code/src_qt3support_itemviews_q3listview.cpp 0
    We've discarded the pointers to the items since we can still access
    them via their parent \e listView. By default, Q3ListView sorts its
    items; this can be switched off with Q3ListView::setSorting(-1).

    The parent must be another Q3ListViewItem or a Q3ListView. If the
    parent is a Q3ListView, the item becomes a top-level item within
    that Q3ListView. If the parent is another Q3ListViewItem, the item
    becomes a child of that list view item.

    If you keep the pointer, you can set or change the texts using
    setText(), add pixmaps using setPixmap(), change its mode using
    setSelectable(), setSelected(), setOpen() and setExpandable().
    You'll also be able to change its height using setHeight(), and
    traverse its sub-items. You don't have to keep the pointer since
    you can get a pointer to any Q3ListViewItem in a Q3ListView using
    Q3ListView::selectedItem(), Q3ListView::currentItem(),
    Q3ListView::firstChild(), Q3ListView::lastItem() and
    Q3ListView::findItem().

    If you call \c delete on a list view item, it will be deleted as
    expected, and as usual for \l{QObject}s, if it has any child items
    (to any depth), all these will be deleted too.

    \l{Q3CheckListItem}s are list view items that have a checkbox or
    radio button and can be used in place of plain Q3ListViewItems.

    You can traverse the tree as if it were a doubly-linked list using
    itemAbove() and itemBelow(); they return pointers to the items
    directly above and below this item on the screen (even if none of
    them are actually visible at the moment).

    Here's how to traverse all of an item's children (but not its
    children's children, etc.):
    Example:
    \snippet doc/src/snippets/code/src_qt3support_itemviews_q3listview.cpp 1

    If you want to iterate over every item, to any level of depth use
    an iterator. To iterate over the entire tree, initialize the
    iterator with the list view itself; to iterate over an item's
    children (and children's children to any depth), initialize the
    iterator with the item:
    \snippet doc/src/snippets/code/src_qt3support_itemviews_q3listview.cpp 2

    Note that the order of the children will change when the sorting
    order changes and is undefined if the items are not visible. You
    can, however, call enforceSortOrder() at any time; Q3ListView will
    always call it before it needs to show an item.

    Many programs will need to reimplement Q3ListViewItem. The most
    commonly reimplemented functions are:
    \table
    \header \i Function \i Description
    \row \i \l text()
         \i Returns the text in a column. Many subclasses will compute
            this on the fly.
    \row \i \l key()
         \i Used for sorting. The default key() simply calls
            text(), but judicious use of key() can give you fine
            control over sorting; for example, QFileDialog
            reimplements key() to sort by date.
    \row \i \l setup()
         \i Called before showing the item and whenever the list
            view's font changes, for example.
    \row \i \l activate()
         \i Called whenever the user clicks on the item or presses
            Space when the item is the current item.
    \endtable

    Some subclasses call setExpandable(true) even when they have no
    children, and populate themselves when setup() or setOpen(true) is
    called. The \c dirview/dirview.cpp example program uses this
    technique to start up quickly: The files and subdirectories in a
    directory aren't inserted into the tree until they're actually
    needed.

    \img qlistviewitems.png List View Items

    \sa Q3CheckListItem Q3ListView
*/

/*!
    \fn int Q3CheckListItem::rtti() const

    Returns 1.

    Make your derived classes return their own values for rtti(), and
    you can distinguish between list view items. You should use values
    greater than 1000, to allow for extensions to this class.
*/

/*!
    Constructs a new top-level list view item in the Q3ListView \a
    parent.
*/

Q3ListViewItem::Q3ListViewItem(Q3ListView * parent)
{
    init();
    parent->insertItem(this);
}


/*!
    Constructs a new list view item that is a child of \a parent and
    first in the parent's list of children.
*/

Q3ListViewItem::Q3ListViewItem(Q3ListViewItem * parent)
{
    init();
    parent->insertItem(this);
}




/*!
    Constructs an empty list view item that is a child of \a parent
    and is after item \a after in the parent's list of children. Since
    \a parent is a Q3ListView the item will be a top-level item.
*/

Q3ListViewItem::Q3ListViewItem(Q3ListView * parent, Q3ListViewItem * after)
{
    init();
    parent->insertItem(this);
    moveToJustAfter(after);
}


/*!
    Constructs an empty list view item that is a child of \a parent
    and is after item \a after in the parent's list of children.
*/

Q3ListViewItem::Q3ListViewItem(Q3ListViewItem * parent, Q3ListViewItem * after)
{
    init();
    parent->insertItem(this);
    moveToJustAfter(after);
}



/*!
    Constructs a new top-level list view item in the Q3ListView \a
    parent, with up to eight constant strings, \a label1, \a label2, \a
    label3, \a label4, \a label5, \a label6, \a label7 and \a label8
    defining its columns' contents.

    \sa setText()
*/

Q3ListViewItem::Q3ListViewItem(Q3ListView * parent,
                              const QString &label1,
                              const QString &label2,
                              const QString &label3,
                              const QString &label4,
                              const QString &label5,
                              const QString &label6,
                              const QString &label7,
                              const QString &label8)
{
    init();
    parent->insertItem(this);

    setText(0, label1);
    setText(1, label2);
    setText(2, label3);
    setText(3, label4);
    setText(4, label5);
    setText(5, label6);
    setText(6, label7);
    setText(7, label8);
}


/*!
    Constructs a new list view item as a child of the Q3ListViewItem \a
    parent with up to eight constant strings, \a label1, \a label2, \a
    label3, \a label4, \a label5, \a label6, \a label7 and \a label8
    as columns' contents.

    \sa setText()
*/

Q3ListViewItem::Q3ListViewItem(Q3ListViewItem * parent,
                              const QString &label1,
                              const QString &label2,
                              const QString &label3,
                              const QString &label4,
                              const QString &label5,
                              const QString &label6,
                              const QString &label7,
                              const QString &label8)
{
    init();
    parent->insertItem(this);

    setText(0, label1);
    setText(1, label2);
    setText(2, label3);
    setText(3, label4);
    setText(4, label5);
    setText(5, label6);
    setText(6, label7);
    setText(7, label8);
}

/*!
    Constructs a new list view item in the Q3ListView \a parent that is
    included after item \a after and that has up to eight column
    texts, \a label1, \a label2, \a label3, \a label4, \a label5, \a
    label6, \a label7 and\a label8.

    Note that the order is changed according to Q3ListViewItem::key()
    unless the list view's sorting is disabled using
    Q3ListView::setSorting(-1).

    \sa setText()
*/

Q3ListViewItem::Q3ListViewItem(Q3ListView * parent, Q3ListViewItem * after,
                              const QString &label1,
                              const QString &label2,
                              const QString &label3,
                              const QString &label4,
                              const QString &label5,
                              const QString &label6,
                              const QString &label7,
                              const QString &label8)
{
    init();
    parent->insertItem(this);
    moveToJustAfter(after);

    setText(0, label1);
    setText(1, label2);
    setText(2, label3);
    setText(3, label4);
    setText(4, label5);
    setText(5, label6);
    setText(6, label7);
    setText(7, label8);
}


/*!
    Constructs a new list view item as a child of the Q3ListViewItem \a
    parent. It is inserted after item \a after and may contain up to
    eight strings, \a label1, \a label2, \a label3, \a label4, \a
    label5, \a label6, \a label7 and \a label8 as column entries.

    Note that the order is changed according to Q3ListViewItem::key()
    unless the list view's sorting is disabled using
    Q3ListView::setSorting(-1).

    \sa setText()
*/

Q3ListViewItem::Q3ListViewItem(Q3ListViewItem * parent, Q3ListViewItem * after,
                              const QString &label1,
                              const QString &label2,
                              const QString &label3,
                              const QString &label4,
                              const QString &label5,
                              const QString &label6,
                              const QString &label7,
                              const QString &label8)
{
    init();
    parent->insertItem(this);
    moveToJustAfter(after);

    setText(0, label1);
    setText(1, label2);
    setText(2, label3);
    setText(3, label4);
    setText(4, label5);
    setText(5, label6);
    setText(6, label7);
    setText(7, label8);
}

/*!
    Sorts all this item's child items using the current sorting
    configuration (sort column and direction).

    \sa enforceSortOrder()
*/

void Q3ListViewItem::sort()
{
    if (!listView())
         return;
    lsc = Unsorted;
    enforceSortOrder();
    listView()->triggerUpdate();
}

/*!
    Returns 0.

    Make your derived classes return their own values for rtti(), so
    that you can distinguish between different kinds of list view
    items. You should use values greater than 1000 to allow for
    extensions to this class.
*/

int Q3ListViewItem::rtti() const
{
    return RTTI;
}

/*
    Performs the initializations that's common to the constructors.
*/

void Q3ListViewItem::init()
{
    ownHeight = 0;
    maybeTotalHeight = -1;
    open = false;

    nChildren = 0;
    parentItem = 0;
    siblingItem = childItem = 0;

    columns = 0;

    selected = 0;
    selectable = true;

    lsc = Unsorted;
    lso = true; // unsorted in ascending order :)
    configured = false;
    expandable = false;
    selectable = true;
    is_root = false;
    allow_drag = false;
    allow_drop = false;
    visible = true;
    renameBox = 0;
    enabled = true;
    mlenabled = false;
}

/*!
    If \a b is true, the item is made visible; otherwise it is hidden.

    If the item is not visible, itemAbove() and itemBelow() will never
    return this item, although you still can reach it by using e.g.
    Q3ListViewItemIterator.
*/

void Q3ListViewItem::setVisible(bool b)
{
    if (b == (bool)visible)
        return;
    Q3ListView *lv = listView();
    if (!lv)
        return;
    if (b && parent() && !parent()->isVisible())
        return;
    visible = b;
    configured = false;
    setHeight(0);
    invalidateHeight();
    if (parent())
        parent()->invalidateHeight();
    else
        lv->d->r->invalidateHeight();
    for (Q3ListViewItem *i = childItem; i; i = i->siblingItem)
        i->setVisible(b);
    if (lv)
        lv->triggerUpdate();
}

/*!
    Returns true if the item is visible; otherwise returns false.

    \sa setVisible()
*/

bool Q3ListViewItem::isVisible() const
{
    return (bool)visible;
}

/*!
    If \a b is true, this item can be in-place renamed in the column
    \a col by the user; otherwise it cannot be renamed in-place.
*/

void Q3ListViewItem::setRenameEnabled(int col, bool b)
{
    Q3ListViewPrivate::ItemColumnInfo * l = (Q3ListViewPrivate::ItemColumnInfo*)columns;
    if (!l) {
        l = new Q3ListViewPrivate::ItemColumnInfo;
        columns = (void*)l;
    }
    for(int c = 0; c < col; c++) {
        if (!l->next)
            l->next = new Q3ListViewPrivate::ItemColumnInfo;
        l = l->next;
    }

    if (!l)
        return;
    l->allow_rename = b;
}

/*!
    Returns true if this item can be in-place renamed in column \a
    col; otherwise returns false.
*/

bool Q3ListViewItem::renameEnabled(int col) const
{
    Q3ListViewPrivate::ItemColumnInfo * l = (Q3ListViewPrivate::ItemColumnInfo*)columns;
    if (!l)
        return false;

    while(col && l) {
        l = l->next;
        col--;
    }

    if (!l)
        return false;
    return (bool)l->allow_rename;
}

/*!
    If \a b is true the item is enabled; otherwise it is disabled.
    Disabled items are drawn differently (e.g. grayed-out) and are not
    accessible by the user.
*/

void Q3ListViewItem::setEnabled(bool b)
{
    if ((bool)enabled == b)
        return;
    enabled = b;
    if (!enabled)
        selected = false;
    Q3ListView *lv = listView();
    if (lv) {
        lv->triggerUpdate();

#ifndef QT_NO_ACCESSIBILITY
    QAccessible::updateAccessibility(lv->viewport(), indexOfItem(this), QAccessible::StateChanged);
#endif
    }
}

/*!
    Returns true if this item is enabled; otherwise returns false.

    \sa setEnabled()
*/

bool Q3ListViewItem::isEnabled() const
{
    return (bool)enabled;
}

/*!
    If in-place renaming of this item is enabled (see
    renameEnabled()), this function starts renaming the item in column
    \a col, by creating and initializing an edit box.
*/

void Q3ListViewItem::startRename(int col)
{
    if (!renameEnabled(col))
        return;
    if (renameBox)
        cancelRename(col);
    Q3ListView *lv = listView();
    if (!lv)
        return;

    if (lv->d->renameTimer)
        lv->d->renameTimer->stop();

    lv->ensureItemVisible(this);

    if (lv->d->timer->isActive()) {
        // make sure that pending calculations get finished
        lv->d->timer->stop();
        lv->updateContents();
    }

    if (lv->currentItem() && lv->currentItem()->renameBox) {
        if (lv->d->defRenameAction == Q3ListView::Reject)
            lv->currentItem()->cancelRename(lv->currentItem()->renameCol);
        else
            lv->currentItem()->okRename(lv->currentItem()->renameCol);
    }

    if (this != lv->currentItem())
        lv->setCurrentItem(this);

    QRect r = lv->itemRect(this);
    r = QRect(lv->viewportToContents(r.topLeft()), r.size());
    r.setLeft(lv->header()->sectionPos(col));
    r.setWidth(qMin(lv->header()->sectionSize(col) - 1,
                    lv->contentsX() + lv->visibleWidth() - r.left()));
    if (col == 0)
        r.setLeft(r.left() + lv->itemMargin() + (depth() + (lv->rootIsDecorated() ? 1 : 0)) * lv->treeStepSize() - 1);
    if (pixmap(col))
        r.setLeft(r.left() + pixmap(col)->width());
    if (r.x() - lv->contentsX() < 0) {
        lv->scrollBy(r.x() - lv->contentsX(), 0);
        r.setX(lv->contentsX());
    } else if ((lv->contentsX() + lv->visibleWidth()) < (r.x() + r.width())) {
        lv->scrollBy((r.x() + r.width()) - (lv->contentsX() + lv->visibleWidth()), 0);
    }
    if (r.width() > lv->visibleWidth())
        r.setWidth(lv->visibleWidth());
    renameBox = new QLineEdit(lv->viewport(), "qt_renamebox");
    renameBox->setFrame(false);
    renameBox->setText(text(col));
    renameBox->selectAll();
    renameBox->installEventFilter(lv);
    lv->addChild(renameBox, r.x(), r.y());
    renameBox->resize(r.size());
    lv->viewport()->setFocusProxy(renameBox);
    renameBox->setFocus();
    renameBox->show();
    renameCol = col;
}

/*!
    This function removes the rename box.
*/

void Q3ListViewItem::removeRenameBox()
{
    // Sanity, it should be checked by the functions calling this first anyway
    Q3ListView *lv = listView();
    if (!lv || !renameBox)
        return;
    const bool resetFocus = lv->viewport()->focusProxy() == renameBox;
    delete renameBox;
    renameBox = 0;
    if (resetFocus) {
        lv->viewport()->setFocusProxy(lv);
        lv->setFocus();
    }
}

/*!
    This function is called if the user presses Enter during in-place
    renaming of the item in column \a col.

    \sa cancelRename()
*/

void Q3ListViewItem::okRename(int col)
{
    Q3ListView *lv = listView();
    if (!lv || !renameBox)
        return;
    setText(col, renameBox->text());
    removeRenameBox();

    // we set the parent lsc to Unsorted if that column is the sorted one
    if (parent() && (int)parent()->lsc == col)
        parent()->lsc = Unsorted;

    emit lv->itemRenamed(this, col);
    emit lv->itemRenamed(this, col, text(col));
}

/*!
    This function is called if the user cancels in-place renaming of
    this item in column \a col (e.g. by pressing Esc).

    \sa okRename()
*/

void Q3ListViewItem::cancelRename(int)
{
    Q3ListView *lv = listView();
    if (!lv || !renameBox)
        return;
    removeRenameBox();
}

/*!
    Destroys the item, deleting all its children and freeing up all
    allocated resources.
*/

Q3ListViewItem::~Q3ListViewItem()
{
    if (renameBox) {
        delete renameBox;
        renameBox = 0;
    }

    Q3ListView *lv = listView();

    if (lv) {
        if (lv->d->oldFocusItem == this)
            lv->d->oldFocusItem = 0;
        if (lv->d->focusItem == this)
            lv->d->focusItem = 0;
        if (lv->d->highlighted == this)
            lv->d->highlighted = 0;
        if (lv->d->pressedItem == this)
            lv->d->pressedItem = 0;
        if (lv->d->selectAnchor == this)
            lv->d->selectAnchor = 0;
        for (int j = 0; j < lv->d->iterators.size(); ++j) {
            Q3ListViewItemIterator *i = lv->d->iterators.at(j);
            if (i->current() == this)
                i->currentRemoved();
        }
    }

    if (parentItem)
        parentItem->takeItem(this);
    Q3ListViewItem * i = childItem;
    childItem = 0;
    while (i) {
        i->parentItem = 0;
        Q3ListViewItem * n = i->siblingItem;
        delete i;
        i = n;
    }
    delete (Q3ListViewPrivate::ItemColumnInfo *)columns;
}


/*!
    If \a b is true each of the item's columns may contain multiple
    lines of text; otherwise each of them may only contain a single
    line.
*/

void Q3ListViewItem::setMultiLinesEnabled(bool b)
{
    mlenabled = b;
}

/*!
    Returns true if the item can display multiple lines of text in its
    columns; otherwise returns false.
*/

bool Q3ListViewItem::multiLinesEnabled() const
{
    return mlenabled;
}

/*!
    If \a allow is true, the list view starts a drag (see
    Q3ListView::dragObject()) when the user presses and moves the mouse
    on this item.
*/


void Q3ListViewItem::setDragEnabled(bool allow)
{
    allow_drag = (uint)allow;
}

/*!
    If \a allow is true, the list view accepts drops onto the item;
    otherwise drops are not allowed.
*/

void Q3ListViewItem::setDropEnabled(bool allow)
{
    allow_drop = (uint)allow;
}

/*!
    Returns true if this item can be dragged; otherwise returns false.

    \sa setDragEnabled()
*/

bool Q3ListViewItem::dragEnabled() const
{
    return (bool)allow_drag;
}

/*!
    Returns true if this item accepts drops; otherwise returns false.

    \sa setDropEnabled(), acceptDrop()
*/

bool Q3ListViewItem::dropEnabled() const
{
    return (bool)allow_drop;
}

/*!
    Returns true if the item can accept drops of type QMimeSource \a
    mime; otherwise returns false.

    The default implementation does nothing and returns false. A
    subclass must reimplement this to accept drops.
*/

bool Q3ListViewItem::acceptDrop(const QMimeSource *) const
{
    return false;
}

#ifndef QT_NO_DRAGANDDROP

/*!
    This function is called when something was dropped on the item. \a e
    contains all the information about the drop.

    The default implementation does nothing, subclasses may need to
    reimplement this function.
*/

void Q3ListViewItem::dropped(QDropEvent *e)
{
    Q_UNUSED(e);
}

#endif

/*!
    This function is called when a drag enters the item's bounding
    rectangle.

    The default implementation does nothing, subclasses may need to
    reimplement this function.
*/

void Q3ListViewItem::dragEntered()
{
}

/*!
    This function is called when a drag leaves the item's bounding
    rectangle.

    The default implementation does nothing, subclasses may need to
    reimplement this function.
*/

void Q3ListViewItem::dragLeft()
{
}

/*!
    Inserts \a newChild into this list view item's list of children.
    You should not need to call this function; it is called
    automatically by the constructor of \a newChild.

    \warning If you are using \c Single selection mode, then you
    should only insert unselected items.
*/

void Q3ListViewItem::insertItem(Q3ListViewItem * newChild)
{
    Q3ListView *lv = listView();
    if (lv && lv->currentItem() && lv->currentItem()->renameBox) {
        if (lv->d->defRenameAction == Q3ListView::Reject)
            lv->currentItem()->cancelRename(lv->currentItem()->renameCol);
        else
            lv->currentItem()->okRename(lv->currentItem()->renameCol);
    }

    if (!newChild || newChild->parentItem == this)
        return;
    if (newChild->parentItem)
        newChild->parentItem->takeItem(newChild);
    if (open)
        invalidateHeight();
    newChild->siblingItem = childItem;
    childItem = newChild;
    nChildren++;
    newChild->parentItem = this;
    lsc = Unsorted;
    newChild->ownHeight = 0;
    newChild->configured = false;

    if (lv && !lv->d->focusItem) {
        lv->d->focusItem = lv->firstChild();
        lv->d->selectAnchor = lv->d->focusItem;
        lv->repaintItem(lv->d->focusItem);
    }
}


/*!
    \fn void Q3ListViewItem::removeItem(Q3ListViewItem *item)

    Removes the given \a item. Use takeItem() instead.
*/


/*!
    Removes \a item from this object's list of children and causes an
    update of the screen display. The item is not deleted. You should
    not normally need to call this function because
    Q3ListViewItem::~Q3ListViewItem() calls it.

    The normal way to delete an item is to use \c delete.

    If you need to move an item from one place in the hierarchy to
    another you can use takeItem() to remove the item from the list
    view and then insertItem() to put the item back in its new
    position.

    If a taken item is part of a selection in \c Single selection
    mode, it is unselected and selectionChanged() is emitted. If a
    taken item is part of a selection in \c Multi or \c Extended
    selection mode, it remains selected.

    \warning This function leaves \a item and its children in a state
    where most member functions are unsafe. Only a few functions work
    correctly on an item in this state, most notably insertItem(). The
    functions that work on taken items are explicitly documented as
    such.

    \sa Q3ListViewItem::insertItem()
*/

void Q3ListViewItem::takeItem(Q3ListViewItem * item)
{
    if (!item)
        return;

    Q3ListView *lv = listView();
    if (lv && lv->currentItem() && lv->currentItem()->renameBox) {
        if (lv->d->defRenameAction == Q3ListView::Reject)
            lv->currentItem()->cancelRename(lv->currentItem()->renameCol);
        else
            lv->currentItem()->okRename(lv->currentItem()->renameCol);
    }
    bool emit_changed = false;
    if (lv && !lv->d->clearing) {
        if (lv->d->oldFocusItem == this)
            lv->d->oldFocusItem = 0;

        for (int j = 0; j < lv->d->iterators.size(); ++j) {
            Q3ListViewItemIterator *i = lv->d->iterators.at(j);
            if (i->current() == item)
                i->currentRemoved();
        }

        invalidateHeight();

        if (lv->d && !lv->d->drawables.isEmpty())
            lv->d->drawables.clear();

        if (!lv->d->dirtyItems.isEmpty()) {
            if (item->childItem) {
                lv->d->dirtyItems.clear();
                lv->d->dirtyItemTimer->stop();
                lv->triggerUpdate();
            } else {
                lv->d->dirtyItems.removeAll(item);
            }
        }

        if (lv->d->focusItem) {
            const Q3ListViewItem * c = lv->d->focusItem;
            while(c && c != item)
                c = c->parentItem;
            if (c == item) {
                if (lv->selectedItem()) {
                    // for Single, setSelected(false) when selectedItem() is taken
                    lv->selectedItem()->setSelected(false);
                    // we don't emit selectionChanged(0)
                    emit lv->selectionChanged();
                }
                if (item->nextSibling())
                    lv->d->focusItem = item->nextSibling();
                else if (item->itemAbove())
                    lv->d->focusItem = item->itemAbove();
                else
                    lv->d->focusItem = 0;
                emit_changed = true;
            }
        }

        // reset anchors etc. if they are set to this or any child
        // items
        const Q3ListViewItem *ptr = lv->d->selectAnchor;
        while (ptr && ptr != item)
            ptr = ptr->parentItem;
	if (ptr == item)
	    lv->d->selectAnchor = lv->d->focusItem;

        ptr = lv->d->startDragItem;
        while (ptr && ptr != item)
            ptr = ptr->parentItem;
	if (ptr == item)
	    lv->d->startDragItem = 0;

        ptr = lv->d->pressedItem;
        while (ptr && ptr != item)
            ptr = ptr->parentItem;
	if (ptr == item)
	    lv->d->pressedItem = 0;

        ptr = lv->d->highlighted;
        while (ptr && ptr != item)
            ptr = ptr->parentItem;
	if (ptr == item)
	    lv->d->highlighted = 0;
    }

    nChildren--;

    Q3ListViewItem ** nextChild = &childItem;
    while(nextChild && *nextChild && item != *nextChild)
        nextChild = &((*nextChild)->siblingItem);

    if (nextChild && item == *nextChild)
        *nextChild = (*nextChild)->siblingItem;
    item->parentItem = 0;
    item->siblingItem = 0;
    item->ownHeight = 0;
    item->maybeTotalHeight = -1;
    item->configured = false;

    if (emit_changed) {
        emit lv->currentChanged(lv->d->focusItem);
#ifndef QT_NO_ACCESSIBILITY
        QAccessible::updateAccessibility(lv->viewport(), 0, QAccessible::Focus);
#endif
    }
}


/*!
    \fn QString Q3ListViewItem::key(int column, bool ascending) const

    Returns a key that can be used for sorting by column \a column.
    The default implementation returns text(). Derived classes may
    also incorporate the order indicated by \a ascending into this
    key, although this is not recommended.

    If you want to sort on non-alphabetical data, e.g. dates, numbers,
    etc., it is more efficient to reimplement compare().

    \sa compare(), sortChildItems()
*/

QString Q3ListViewItem::key(int column, bool) const
{
    return text(column);
}


/*!
    Compares this list view item to \a i using the column \a col in \a
    ascending order. Returns \< 0 if this item is less than \a i, 0 if
    they are equal and \> 0 if this item is greater than \a i.

    This function is used for sorting.

    The default implementation compares the item keys (key()) using
    QString::localeAwareCompare(). A reimplementation can use
    different values and a different comparison function. Here is a
    reimplementation that uses plain Unicode comparison:

    \snippet doc/src/snippets/code/src_qt3support_itemviews_q3listview.cpp 3
    We don't recommend using \a ascending so your code can safely
    ignore it.

    \sa key() QString::localeAwareCompare() QString::compare()
*/

int Q3ListViewItem::compare(Q3ListViewItem *i, int col, bool ascending) const
{
    return key(col, ascending).localeAwareCompare(i->key(col, ascending));
}

/*!
    Sorts this item's children using column \a column. This is done in
    ascending order if \a ascending is true and in descending order if
    \a ascending is false.

    Asks some of the children to sort their children. (Q3ListView and
    Q3ListViewItem ensure that all on-screen objects are properly
    sorted but may avoid or defer sorting other objects in order to be
    more responsive.)

    \sa key() compare()
*/

void Q3ListViewItem::sortChildItems(int column, bool ascending)
{
    // we try HARD not to sort.  if we're already sorted, don't.
    if (column == (int)lsc && ascending == (bool)lso)
        return;

    if (column < 0)
        return;

    lsc = column;
    lso = ascending;

    const int nColumns = (listView() ? listView()->columns() : 0);

    // and don't sort if we already have the right sorting order
    if (column > nColumns || childItem == 0 || childItem->siblingItem == 0)
        return;

    // make an array for qHeapSort()
    Q3ListViewPrivate::SortableItem * siblings
        = new Q3ListViewPrivate::SortableItem[nChildren];
    Q3ListViewItem * s = childItem;
    int i = 0;
    while (s && i < nChildren) {
        siblings[i].numCols = nColumns;
        siblings[i].col = column;
        siblings[i].asc = ascending;
        siblings[i].item = s;
        s = s->siblingItem;
        i++;
    }

    // and sort it.
    qHeapSort(siblings, siblings + nChildren);

    // build the linked list of siblings, in the appropriate
    // direction, and finally set this->childItem to the new top
    // child.
    if (ascending) {
        for(i = 0; i < nChildren - 1; i++)
            siblings[i].item->siblingItem = siblings[i+1].item;
        siblings[nChildren-1].item->siblingItem = 0;
        childItem = siblings[0].item;
    } else {
        for(i = nChildren - 1; i > 0; i--)
            siblings[i].item->siblingItem = siblings[i-1].item;
        siblings[0].item->siblingItem = 0;
        childItem = siblings[nChildren-1].item;
    }
    for (i = 0; i < nChildren; i++) {
        if (siblings[i].item->isOpen())
            siblings[i].item->sort();
    }
    delete[] siblings;
}


/*!
    Sets this item's height to \a height pixels. This implicitly
    changes totalHeight(), too.

    Note that a font change causes this height to be overwritten
    unless you reimplement setup().

    For best results in Windows style we suggest using an even number
    of pixels.

    \sa height() totalHeight() isOpen()
*/

void Q3ListViewItem::setHeight(int height)
{
    if (ownHeight != height) {
        if (visible)
            ownHeight = height;
        else
            ownHeight = 0;
        invalidateHeight();
    }
}


/*!
    Invalidates the cached total height of this item, including all
    open children.

    \sa setHeight() height() totalHeight()
*/

void Q3ListViewItem::invalidateHeight()
{
    if (maybeTotalHeight < 0)
        return;
    maybeTotalHeight = -1;
    if (parentItem && parentItem->isOpen())
        parentItem->invalidateHeight();
}


/*!
    Opens or closes an item, i.e. shows or hides an item's children.

    If \a o is true all child items are shown initially. The user can
    hide them by clicking the \bold{-} icon to the left of the item.
    If \a o is false, the children of this item are initially hidden.
    The user can show them by clicking the \bold{+} icon to the left
    of the item.

    \sa height() totalHeight() isOpen()
*/

void Q3ListViewItem::setOpen(bool o)
{
    if (o == (bool)open || !enabled)
        return;
    open = o;

    // If no children to show simply emit signals and return
    if (!nChildren) {
        Q3ListView *lv = listView();
        if (lv && this != lv->d->r) {
            if (o)
                emit lv->expanded(this);
            else
                emit lv->collapsed(this);
#ifndef QT_NO_ACCESSIBILITY
            QAccessible::updateAccessibility(lv->viewport(), indexOfItem(this), QAccessible::StateChanged);
#endif
        }
        return;
    }
    invalidateHeight();

    if (!configured) {
        Q3ListViewItem * l = this;
        QStack<Q3ListViewItem *> s;
        while(l) {
            if (l->open && l->childItem) {
                s.push(l->childItem);
            } else if (l->childItem) {
                // first invisible child is unconfigured
                Q3ListViewItem * c = l->childItem;
                while(c) {
                    c->configured = false;
                    c = c->siblingItem;
                }
            }
            l->configured = true;
            l->setup();
            l = (l == this) ? 0 : l->siblingItem;
            if (!l && !s.isEmpty())
                l = s.pop();
        }
    }

    Q3ListView *lv = listView();

    if (open && lv)
        enforceSortOrder();

    if (isVisible() && lv && lv->d && !lv->d->drawables.isEmpty())
        lv->buildDrawableList();

    if (lv && this != lv->d->r) {
        if (o)
            emit lv->expanded(this);
        else
            emit lv->collapsed(this);
#ifndef QT_NO_ACCESSIBILITY
        QAccessible::updateAccessibility(lv->viewport(), indexOfItem(this), QAccessible::StateChanged);
#endif
    }
}


/*!
    This virtual function is called before the first time Q3ListView
    needs to know the height or any other graphical attribute of this
    object, and whenever the font, GUI style, or colors of the list
    view change.

    The default calls widthChanged() and sets the item's height to the
    height of a single line of text in the list view's font. (If you
    use icons, multi-line text, etc., you will probably need to call
    setHeight() yourself or reimplement it.)
*/

void Q3ListViewItem::setup()
{
    widthChanged();
    Q3ListView *lv = listView();

    int ph = 0;
    int h = 0;
    if (lv) {
        for (int i = 0; i < lv->d->column.size(); ++i) {
            if (pixmap(i))
                ph = qMax(ph, pixmap(i)->height());
        }

        if (mlenabled) {
            h = ph;
            for (int c = 0; c < lv->columns(); ++c) {
                int lines = text(c).count(QLatin1Char('\n')) + 1;
                int tmph = lv->d->fontMetricsHeight
                           + lv->fontMetrics().lineSpacing() * (lines - 1);
                h = qMax(h, tmph);
            }
            h += 2*lv->itemMargin();
        } else {
            h = qMax(lv->d->fontMetricsHeight, ph) + 2*lv->itemMargin();
        }
    }

    h = qMax(h, QApplication::globalStrut().height());

    if (h % 2 > 0)
        h++;
    setHeight(h);
}




/*!
    This virtual function is called whenever the user presses the mouse
    on this item or presses Space on it.

    \sa activatedPos()
*/

void Q3ListViewItem::activate()
{
}


/*!
    When called from a reimplementation of activate(), this function
    gives information on how the item was activated. Otherwise the
    behavior is undefined.

    If activate() was caused by a mouse press, the function sets \a
    pos to where the user clicked and returns true; otherwise it
    returns false and does not change \a pos.

    \a pos is relative to the top-left corner of this item.

    \sa activate()
*/

bool Q3ListViewItem::activatedPos(QPoint &pos)
{
    if (activatedByClick)
        pos = activatedP;
    return activatedByClick;
}


/*!
    \fn bool Q3ListViewItem::isSelectable() const

    Returns true if the item is selectable (as it is by default);
    otherwise returns false

    \sa setSelectable()
*/


/*!
    Sets this items to be selectable if \a enable is true (the
    default) or not to be selectable if \a enable is false.

    The user is not able to select a non-selectable item using either
    the keyboard or the mouse. The application programmer still can
    though, e.g. using setSelected().

    \sa isSelectable()
*/

void Q3ListViewItem::setSelectable(bool enable)
{
    selectable = enable;
}


/*!
    \fn bool Q3ListViewItem::isExpandable() const

    Returns true if this item is expandable even when it has no
    children; otherwise returns false.
*/

/*!
    Sets this item to be expandable even if it has no children if \a
    enable is true, and to be expandable only if it has children if \a
    enable is false (the default).

    The dirview example uses this in the canonical fashion. It checks
    whether the directory is empty in setup() and calls
    setExpandable(true) if not; in setOpen() it reads the contents of
    the directory and inserts items accordingly. This strategy means
    that dirview can display the entire file system without reading
    very much at startup.

    Note that root items are not expandable by the user unless
    Q3ListView::setRootIsDecorated() is set to true.

    \sa setSelectable()
*/

void Q3ListViewItem::setExpandable(bool enable)
{
    expandable = enable;
}


/*!
    Makes sure that this object's children are sorted appropriately.

    This only works if every item from the root item down to this item
    is already sorted.

    \sa sortChildItems()
*/

void Q3ListViewItem::enforceSortOrder() const
{
    Q3ListView *lv = listView();
    if (!lv || (lv && (lv->d->clearing || lv->d->sortcolumn == Unsorted)))
        return;
    if (parentItem &&
         (parentItem->lsc != lsc || parentItem->lso != lso))
        ((Q3ListViewItem *)this)->sortChildItems((int)parentItem->lsc,
                                                 (bool)parentItem->lso);
    else if (!parentItem &&
              ((int)lsc != lv->d->sortcolumn || (bool)lso != lv->d->ascending))
        ((Q3ListViewItem *)this)->sortChildItems(lv->d->sortcolumn, lv->d->ascending);
}


/*!
    \fn bool Q3ListViewItem::isSelected() const

    Returns true if this item is selected; otherwise returns false.

    \sa setSelected() Q3ListView::setSelected() Q3ListView::selectionChanged()
*/


/*!
    If \a s is true this item is selected; otherwise it is deselected.

    This function does not maintain any invariants or repaint anything
    -- Q3ListView::setSelected() does that.

    \sa height() totalHeight()
*/

void Q3ListViewItem::setSelected(bool s)
{
    bool old = selected;

    Q3ListView *lv = listView();
    if (lv && lv->selectionMode() != Q3ListView::NoSelection) {
        if (s && isSelectable())
            selected = true;
        else
            selected = false;

#ifndef QT_NO_ACCESSIBILITY
        if (old != (bool)selected) {
            int ind = indexOfItem(this);
            QAccessible::updateAccessibility(lv->viewport(), ind, QAccessible::StateChanged);
            QAccessible::updateAccessibility(lv->viewport(), ind, selected ? QAccessible::SelectionAdd : QAccessible::SelectionRemove);
        }
#else
        Q_UNUSED(old);
#endif
    }
}

/*!
    Returns the total height of this object, including any visible
    children. This height is recomputed lazily and cached for as long
    as possible.

    Functions which can affect the total height are, setHeight() which
    is used to set an item's height, setOpen() to show or hide an
    item's children, and invalidateHeight() to invalidate the cached
    height.

    \sa height()
*/

int Q3ListViewItem::totalHeight() const
{
    if (!visible)
        return 0;
    if (maybeTotalHeight >= 0)
        return maybeTotalHeight;
    Q3ListViewItem * that = (Q3ListViewItem *)this;
    if (!that->configured) {
        that->configured = true;
        that->setup(); // ### virtual non-const function called in const
    }
    that->maybeTotalHeight = that->ownHeight;

    if (!that->isOpen() || !that->childCount())
        return that->ownHeight;

    Q3ListViewItem * child = that->childItem;
    while (child != 0) {
        that->maybeTotalHeight += child->totalHeight();
        child = child->siblingItem;
    }
    return that->maybeTotalHeight;
}


/*!
    Returns the text in column \a column, or an empty string if there is
    no text in that column.

    \sa key() paintCell()
*/

QString Q3ListViewItem::text(int column) const
{
    Q3ListViewPrivate::ItemColumnInfo * l
        = (Q3ListViewPrivate::ItemColumnInfo*) columns;

    while(column && l) {
        l = l->next;
        column--;
    }

    return l ? l->text : QString();
}


/*!
    Sets the text in column \a column to \a text, if \a column is a
    valid column number and \a text is different from the existing
    text.

    If the text() function has been reimplemented, this function may
    be a no-op.

    \sa text() key()
*/

void Q3ListViewItem::setText(int column, const QString &text)
{
    if (column < 0)
        return;

    Q3ListViewPrivate::ItemColumnInfo * l
        = (Q3ListViewPrivate::ItemColumnInfo*) columns;
    if (!l) {
        l = new Q3ListViewPrivate::ItemColumnInfo;
        columns = (void*)l;
    }
    for(int c = 0; c < column; c++) {
        if (!l->next)
            l->next = new Q3ListViewPrivate::ItemColumnInfo;
        l = l->next;
    }
    if (l->text == text)
        return;

    int oldLc = 0;
    int newLc = 0;
    if (mlenabled) {
        if (!l->text.isEmpty())
            oldLc = l->text.count(QLatin1Char('\n')) + 1;
        if (!text.isEmpty())
            newLc = text.count(QLatin1Char('\n')) + 1;
    }

    l->dirty = true;
    l->text = text;
    if (column == (int)lsc)
        lsc = Unsorted;

    if (mlenabled && oldLc != newLc)
        setup();
    else
        widthChanged(column);

    Q3ListView * lv = listView();
    if (lv) {
        lv->triggerUpdate();
#ifndef QT_NO_ACCESSIBILITY
        if (lv->isVisible())
            QAccessible::updateAccessibility(lv->viewport(), indexOfItem(this), QAccessible::NameChanged);
#endif
    }
}


/*!
    Sets the pixmap in column \a column to \a pm, if \a pm is non-null
    and different from the current pixmap, and if \a column is
    non-negative.

    \sa pixmap() setText()
*/

void Q3ListViewItem::setPixmap(int column, const QPixmap & pm)
{
    if (column < 0)
        return;

    int oldW = 0;
    int oldH = 0;
    if (pixmap(column)) {
        oldW = pixmap(column)->width();
        oldH = pixmap(column)->height();
    }

    Q3ListViewPrivate::ItemColumnInfo * l
        = (Q3ListViewPrivate::ItemColumnInfo*) columns;
    if (!l) {
        l = new Q3ListViewPrivate::ItemColumnInfo;
        columns = (void*)l;
    }

    for(int c = 0; c < column; c++) {
        if (!l->next)
            l->next = new Q3ListViewPrivate::ItemColumnInfo;
        l = l->next;
    }

    if ((pm.isNull() && (!l->pm || l->pm->isNull())) ||
         (l->pm && pm.serialNumber() == l->pm->serialNumber()))
        return;

    if (pm.isNull()) {
        delete l->pm;
        l->pm = 0;
    } else {
        if (l->pm)
            *(l->pm) = pm;
        else
            l->pm = new QPixmap(pm);
    }

    int newW = 0;
    int newH = 0;
    if (pixmap(column)) {
        newW = pixmap(column)->width();
        newH = pixmap(column)->height();
    }

    if (oldW != newW || oldH != newH) {
        setup();
        widthChanged(column);
        invalidateHeight();
    }
    Q3ListView *lv = listView();
    if (lv) {
        lv->triggerUpdate();
    }
}


/*!
    Returns the pixmap for \a column, or 0 if there is no pixmap for
    \a column.

    \sa setText() setPixmap()
*/

const QPixmap * Q3ListViewItem::pixmap(int column) const
{
    Q3ListViewPrivate::ItemColumnInfo * l
    = (Q3ListViewPrivate::ItemColumnInfo*) columns;

    while(column && l) {
        l = l->next;
        column--;
    }

    return (l && l->pm) ? l->pm : 0;
}


/*
    This function paints the contents of one column of an item
    and aligns it as described by \a align.

    \a p is a QPainter open on the relevant paint device. \a p is
    translated so (0, 0) is the top-left pixel in the cell and \a
    width-1, height()-1 is the bottom-right pixel \e in the cell. The
    other properties of \a p (pen, brush, etc) are undefined. \a pal is
    the color group to use. \a column is the logical column number
    within the item that is to be painted; 0 is the column which may
    contain a tree.

    This function may use Q3ListView::itemMargin() for readability
    spacing on the left and right sides of data such as text, and
    should honor isSelected() and Q3ListView::allColumnsShowFocus().

    If you reimplement this function, you should also reimplement
    width().

    The rectangle to be painted is in an undefined state when this
    function is called, so you \e must draw on all the pixels. The
    painter \a p has the right font on entry.

    \sa paintBranches(), Q3ListView::drawContentsOffset()
*/

static QStyleOptionQ3ListView getStyleOption(const Q3ListView *lv, const Q3ListViewItem *item,
                                             bool hierarchy = false)
{
    QStyleOptionQ3ListView opt;
    opt.init(lv);
    opt.subControls = QStyle::SC_None;
    opt.activeSubControls = QStyle::SC_None;
    QWidget *vp = lv->viewport();
    opt.viewportPalette = vp->palette();
    opt.viewportBGRole = vp->backgroundRole();
    opt.itemMargin = lv->itemMargin();
    opt.sortColumn = 0;
    opt.treeStepSize = lv->treeStepSize();
    opt.rootIsDecorated = lv->rootIsDecorated();
    bool firstItem = true;
    int y = item ? item->itemPos() : 0;
    while (item) {
        QStyleOptionQ3ListViewItem lvi;
        lvi.height = item->height();
        lvi.totalHeight = item->totalHeight();
        lvi.itemY = y;
        lvi.childCount = item->childCount();
        lvi.features = QStyleOptionQ3ListViewItem::None;
        lvi.state = QStyle::State_None;
        if (item->isEnabled())
            lvi.state |= QStyle::State_Enabled;
        if (item->isOpen())
            lvi.state |= QStyle::State_Open;
        if (item->isExpandable())
            lvi.features |= QStyleOptionQ3ListViewItem::Expandable;
        if (item->multiLinesEnabled())
            lvi.features |= QStyleOptionQ3ListViewItem::MultiLine;
        if (item->isVisible())
            lvi.features |= QStyleOptionQ3ListViewItem::Visible;
        if (item->parent() && item->parent()->rtti() == 1
            && static_cast<Q3CheckListItem *>(item->parent())->type() == Q3CheckListItem::Controller)
            lvi.features |= QStyleOptionQ3ListViewItem::ParentControl;
        opt.items.append(lvi);
        // we only care about the children when we are painting the branches
        // this is only enabled by Q3ListViewItem::paintBranches
        if (hierarchy) {
            if (!firstItem) {
                item = item->nextSibling();
            } else {
                firstItem = false;
                item = item->firstChild();
            }
            y += lvi.height;
        } else {
            break;
        }
    }
    return opt;
}

/*!
    \fn void Q3ListViewItem::paintCell(QPainter *painter, const QColorGroup & cg, int column, int width, int align)

    This virtual function paints the contents of one column of an item
    and aligns it as described by \a align.

    The \a painter is a Q3Painter open on the relevant paint
    device. It is translated so (0, 0) is the top-left pixel in the
    cell and \a width - 1, height() - 1 is the bottom-right pixel \e
    in the cell. The other properties of the \a painter (pen, brush, etc) are
    undefined. \a cg is the color group to use. \a column is the
    logical column number within the item that is to be painted; 0 is
    the column which may contain a tree.

    This function may use Q3ListView::itemMargin() for readability
    spacing on the left and right sides of data such as text, and
    should honor \l isSelected() and
    Q3ListView::allColumnsShowFocus().

    If you reimplement this function, you should also reimplement \l
    width().

    The rectangle to be painted is in an undefined state when this
    function is called, so you \e must draw on all the pixels. The
    \a painter has the right font on entry.

    \sa paintBranches(), Q3ListView::drawContentsOffset()
*/
void Q3ListViewItem::paintCell(QPainter * p, const QColorGroup & cg,
                               int column, int width, int align)
{
    // Change width() if you change this.

    QPalette pal = cg;
    if (!p)
        return;

    Q3ListView *lv = listView();
    if (!lv)
        return;
    QFontMetrics fm(p->fontMetrics());

    // had, but we _need_ the column info for the ellipsis thingy!!!
    if (!columns) {
        for (int i = 0; i < lv->d->column.size(); ++i) {
            setText(i, text(i));
        }
    }

    QString t = text(column);

    if (columns) {
        Q3ListViewPrivate::ItemColumnInfo *ci = 0;
        // try until we have a column info....
        while (!ci) {
            ci = (Q3ListViewPrivate::ItemColumnInfo*)columns;
            for (int i = 0; ci && (i < column); ++i)
                ci = ci->next;

            if (!ci) {
                setText(column, t);
                ci = 0;
            }
        }

        // if the column width changed and this item was not painted since this change
        if (ci && (ci->width != width || ci->text != t || ci->dirty)) {
            ci->text = t;
            ci->dirty = false;
            ci->width = width;
            ci->truncated = false;
            // if we have to do the ellipsis thingy calc the truncated text
            int pw = lv->itemMargin()*2 - lv->d->minLeftBearing - lv->d->minRightBearing;
            pw += pixmap(column) ? pixmap(column)->width() + lv->itemMargin() : 0;
            if (!mlenabled && fm.width(t) + pw > width) {
                // take care of arabic shaping in width calculation (lars)
                ci->truncated = true;
                ci->tmpText = qEllipsisText(t, fm, width - pw, align);
            } else if (mlenabled && fm.width(t) + pw > width) {
                QStringList list = t.split(QLatin1Char('\n'));
                for (QStringList::Iterator it = list.begin(); it != list.end(); ++it) {
                    QString z = *it;
                    if (fm.width(z) + pw > width) {
                        ci->truncated = true;
                        *it = qEllipsisText(z, fm, width - pw, align);
                    }
                }

                if (ci->truncated)
                    ci->tmpText = list.join(QString(QLatin1Char('\n')));
            }
        }

        // if we have to draw the ellipsis thingy, use the truncated text
        if (ci && ci->truncated)
            t = ci->tmpText;
    }

    int marg = lv->itemMargin();
    int r = marg;
    const QPixmap * icon = pixmap(column);

    const QPalette::ColorRole crole = lv->viewport()->backgroundRole();
    if (pal.brush(crole) != lv->palette().brush(pal.currentColorGroup(), crole))
        p->fillRect(0, 0, width, height(), pal.brush(crole));
    else
        lv->paintEmptyArea(p, QRect(0, 0, width, height()));

    // (lars) what does this do???
#if 0 // RS: ####
    if (align != Qt::AlignLeft)
        marg -= lv->d->minRightBearing;
#endif
    if (isSelected() &&
         (column == 0 || lv->allColumnsShowFocus())) {
        p->fillRect(r - marg, 0, qMax(0, width - r + marg), height(),
                    pal.brush(QPalette::Highlight));
        if (enabled || !lv)
            p->setPen(pal.highlightedText().color());
        else if (!enabled && lv)
            p->setPen(lv->palette().color(QPalette::Disabled, QPalette::HighlightedText));
    } else {
        if (enabled || !lv)
            p->setPen(pal.text().color());
        else if (!enabled && lv)
            p->setPen(lv->palette().color(QPalette::Disabled, QPalette::Text));
    }


#if 0
    bool reverse = QApplication::reverseLayout();
#else
    bool reverse = false;
#endif
    int iconWidth = 0;

    if (icon) {
        iconWidth = icon->width() + lv->itemMargin();
        int xo = r;
        // we default to Qt::AlignVCenter.
        int yo = (height() - icon->height()) / 2;

        // I guess we may as well always respect vertical alignment.
        if (align & Qt::AlignBottom)
            yo = height() - icon->height();
        else if (align & Qt::AlignTop)
            yo = 0;

        // respect horizontal alignment when there is no text for an item.
        if (text(column).isEmpty()) {
            if (align & Qt::AlignRight)
                xo = width - 2 * marg - iconWidth;
            else if (align & Qt::AlignHCenter)
                xo = (width - iconWidth) / 2;
        }
        if (reverse)
                xo = width - 2 * marg - iconWidth;
        p->drawPixmap(xo, yo, *icon);
    }

    if (!t.isEmpty()) {
        if (!mlenabled) {
            if (!(align & Qt::AlignTop || align & Qt::AlignBottom))
                align |= Qt::AlignVCenter;
        } else {
            if (!(align & Qt::AlignVCenter || align & Qt::AlignBottom))
                align |= Qt::AlignTop;
        }
        if (!reverse)
            r += iconWidth;

        if (!mlenabled) {
            p->drawText(r, 0, width-marg-r, height(), align, t);
        } else {
            p->drawText(r, marg, width-marg-r, height(), align, t);
        }
    }

    if (mlenabled && column == 0 && isOpen() && childCount()) {
        int textheight = fm.size(align, t).height() + 2 * lv->itemMargin();
        textheight = qMax(textheight, QApplication::globalStrut().height());
        if (textheight % 2 > 0)
            textheight++;
        if (textheight < height()) {
            int w = lv->treeStepSize() / 2;
            QStyleOptionQ3ListView opt = getStyleOption(lv, this);
            opt.rect.setRect(0, textheight, w + 1, height() - textheight + 1);
            opt.palette = pal;
            opt.subControls = QStyle::SC_Q3ListViewExpand;
            opt.activeSubControls = QStyle::SC_All;
            lv->style()->drawComplexControl(QStyle::CC_Q3ListView, &opt, p, lv);
        }
    }
}

/*!
    Returns the number of pixels of width required to draw column \a c
    of list view \a lv, using the metrics \a fm without cropping. The
    list view containing this item may use this information depending
    on the Q3ListView::WidthMode settings for the column.

    The default implementation returns the width of the bounding
    rectangle of the text of column \a c.

    \sa listView() widthChanged() Q3ListView::setColumnWidthMode()
    Q3ListView::itemMargin()
*/
int Q3ListViewItem::width(const QFontMetrics& fm,
                          const Q3ListView* lv, int c) const
{
    int w;
    if (mlenabled)
        w = fm.size(Qt::AlignVCenter, text(c)).width() + lv->itemMargin() * 2
            - lv->d->minLeftBearing - lv->d->minRightBearing;
    else
        w = fm.width(text(c)) + lv->itemMargin() * 2
            - lv->d->minLeftBearing - lv->d->minRightBearing;
    const QPixmap * pm = pixmap(c);
    if (pm)
        w += pm->width() + lv->itemMargin(); // ### correct margin stuff?
    return qMax(w, QApplication::globalStrut().width());
}


/*!
    Paints a focus indicator on the rectangle \a r using painter \a p
    and colors \a cg.

    \a p is already clipped.

    \sa paintCell() paintBranches() Q3ListView::setAllColumnsShowFocus()
*/

void Q3ListViewItem::paintFocus(QPainter *p, const QColorGroup &cg, const QRect &r)
{
    QPalette pal = cg;
    Q3ListView *lv = listView();
    if (lv) {
        QStyleOptionFocusRect opt;
        opt.init(lv);
        opt.rect = r;
        opt.palette = pal;
        opt.state |= QStyle::State_KeyboardFocusChange;
        if (isSelected()) {
            opt.state |= QStyle::State_FocusAtBorder;
            opt.backgroundColor = pal.highlight().color();
        } else {
            opt.state |= QStyle::State_None;
            opt.backgroundColor = pal.base().color();
        }
        lv->style()->drawPrimitive(QStyle::PE_FrameFocusRect, &opt, p, lv);
    }
}


/*!
    Paints a set of branches from this item to (some of) its children.

    Painter \a p is set up with clipping and translation so that you
    can only draw in the rectangle that needs redrawing; \a cg is the
    color group to use; the update rectangle is at (0, 0) and has size
    width \a w by height \a h. The top of the rectangle you own is at
    \a y (which is never greater than 0 but can be outside the window
    system's allowed coordinate range).

    The update rectangle is in an undefined state when this function
    is called; this function must draw on \e all of the pixels.

    \sa paintCell(), Q3ListView::drawContentsOffset()
*/

void Q3ListViewItem::paintBranches(QPainter * p, const QColorGroup & cg,
                                   int w, int y, int h)
{
    Q3ListView *lv = listView();
    if (lv)
        lv->paintEmptyArea(p, QRect(0, 0, w, h));
    if (!visible || !lv)
        return;
    QStyleOptionQ3ListView opt = getStyleOption(lv, this, true);
    opt.rect.setRect(0, y, w, h);
    opt.palette = cg;
    opt.subControls = QStyle::SC_Q3ListViewBranch | QStyle::SC_Q3ListViewExpand;
    opt.activeSubControls = QStyle::SC_None;
    lv->style()->drawComplexControl(QStyle::CC_Q3ListView, &opt, p, lv);
}


Q3ListViewPrivate::Root::Root(Q3ListView * parent)
    : Q3ListViewItem(parent)
{
    lv = parent;
    setHeight(0);
    setOpen(true);
}


void Q3ListViewPrivate::Root::setHeight(int)
{
    Q3ListViewItem::setHeight(0);
}


void Q3ListViewPrivate::Root::invalidateHeight()
{
    Q3ListViewItem::invalidateHeight();
    lv->triggerUpdate();
}


Q3ListView * Q3ListViewPrivate::Root::theListView() const
{
    return lv;
}


void Q3ListViewPrivate::Root::setup()
{
    // explicitly nothing
}



/*!
\internal
If called after a mouse click, tells the list view to ignore a
following double click. This state is reset after the next mouse click.
*/

void Q3ListViewItem::ignoreDoubleClick()
{
    Q3ListView *lv = listView();
    if (lv)
        lv->d->ignoreDoubleClick = true;
}



/*!
    \fn void  Q3ListView::onItem(Q3ListViewItem *i)

    This signal is emitted when the user moves the mouse cursor onto
    item \a i, similar to the QWidget::enterEvent() function.
*/

// ### bug here too? see qiconview.cppp onItem/onViewport

/*!
    \fn void  Q3ListView::onViewport()

    This signal is emitted when the user moves the mouse cursor from
    an item to an empty part of the list view.
*/

/*!
    \enum Q3ListView::SelectionMode

    This enumerated type is used by Q3ListView to indicate how it
    reacts to selection by the user.

    \value Single  When the user selects an item, any already-selected
    item becomes unselected, and the user cannot unselect the selected
    item.

    \value Multi  When the user selects an item in the usual way, the
    selection status of that item is toggled and the other items are
    left alone.

    \value Extended When the user selects an item in the usual way,
    the selection is cleared and the new item selected. However, if
    the user presses the Ctrl key when clicking on an item, the
    clicked item gets toggled and all other items are left untouched.
    And if the user presses the Shift key while clicking on an item,
    all items between the current item and the clicked item get
    selected or unselected, depending on the state of the clicked
    item. Also, multiple items can be selected by dragging the mouse
    over them.

    \value NoSelection  Items cannot be selected.

    In other words, \c Single is a real single-selection list view, \c
    Multi a real multi-selection list view, \c Extended is a list view
    where users can select multiple items but usually want to select
    either just one or a range of contiguous items, and \c NoSelection
    is a list view where the user can look but not touch.
*/

/*!
    \enum Q3ListView::ResizeMode

    This enum describes how the list view's header adjusts to resize
    events which affect the width of the list view.

    \value NoColumn The columns do not get resized in resize events.

    \value AllColumns All columns are resized equally to fit the width
    of the list view.

    \value LastColumn The last column is resized to fit the width of
    the list view.
*/

/*!
    \enum Q3ListView::RenameAction

    This enum describes whether a rename operation is accepted if the
    rename editor loses focus without the user pressing Enter.

    \value Accept Rename if Enter is pressed or focus is lost.

    \value Reject Discard the rename operation if focus is lost (and
    Enter has not been pressed).
*/

/*!
    \class Q3ListView
    \brief The Q3ListView class implements a list/tree view.

    \compat

    It can display and control a hierarchy of multi-column items, and
    provides the ability to add new items at any time. The user may
    select one or many items (depending on the \c SelectionMode) and
    sort the list in increasing or decreasing order by any column.

    The simplest pattern of use is to create a Q3ListView, add some
    column headers using addColumn() and create one or more
    Q3ListViewItem or Q3CheckListItem objects with the Q3ListView as
    parent.

    Further nodes can be added to the list view object (the root of the
    tree) or as child nodes to Q3ListViewItems.

    The main setup functions are:
    \table
    \header \i Function \i Action
    \row \i \l addColumn()
         \i Adds a column with a text label and perhaps width. Columns
            are counted from the left starting with column 0.
    \row \i \l setColumnWidthMode()
         \i Sets the column to be resized automatically or not.
    \row \i \l setAllColumnsShowFocus()
         \i Sets whether items should show keyboard focus using all
            columns or just column 0. The default is to show focus
            just using column 0.
    \row \i \l setRootIsDecorated()
         \i Sets whether root items can be opened and closed by the
            user and have open/close decoration to their left. The
            default is false.
    \row \i \l setTreeStepSize()
         \i Sets how many pixels an item's children are indented
            relative to their parent. The default is 20. This is
            mostly a matter of taste.
    \row \i \l setSorting()
         \i Sets whether the items should be sorted, whether it should
            be in ascending or descending order, and by what column
            they should be sorted. By default the list view is sorted
            by the first column; to switch this off call setSorting(-1).
    \endtable

    There are several functions for mapping between items and
    coordinates. itemAt() returns the item at a position on-screen,
    itemRect() returns the rectangle an item occupies on the screen,
    and itemPos() returns the position of any item (whether it is
    on-screen or not). firstChild() returns the list view's first item
    (not necessarily visible on-screen).

    You can iterate over visible items using
    Q3ListViewItem::itemBelow(); over a list view's top-level items
    using Q3ListViewItem::firstChild() and
    Q3ListViewItem::nextSibling(); or every item using a
    Q3ListViewItemIterator. See
    the Q3ListViewItem documentation for examples of traversal.

    An item can be moved amongst its siblings using
    Q3ListViewItem::moveItem(). To move an item in the hierarchy use
    takeItem() and insertItem(). Item's (and all their child items)
    are deleted with \c delete; to delete all the list view's items
    use clear().

    There are a variety of selection modes described in the
    Q3ListView::SelectionMode documentation. The default is \c Single
    selection, which you can change using setSelectionMode().

    Because Q3ListView offers multiple selection it must display
    keyboard focus and selection state separately. Therefore there are
    functions both to set the selection state of an item
    (setSelected()) and to set which item displays keyboard focus
    (setCurrentItem()).

    Q3ListView emits two groups of signals; one group signals changes
    in selection/focus state and one indicates selection. The first
    group consists of selectionChanged() (applicable to all list
    views), selectionChanged(Q3ListViewItem*) (applicable only to a
    \c Single selection list view), and currentChanged(Q3ListViewItem*).
    The second group consists of doubleClicked(Q3ListViewItem*),
    returnPressed(Q3ListViewItem*),
    rightButtonClicked(Q3ListViewItem*, const QPoint&, int), etc.

    Note that changing the state of the list view in a slot connected
    to a list view signal may cause unexpected side effects. If you
    need to change the list view's state in response to a signal, use
    a \link QTimer::singleShot() single shot timer\endlink with a
    time out of 0, and connect this timer to a slot that modifies the
    list view's state.

    In Motif style, Q3ListView deviates fairly strongly from the look
    and feel of the Motif hierarchical tree view. This is done mostly
    to provide a usable keyboard interface and to make the list view
    look better with a white background.

    If selectionMode() is \c Single (the default) the user can select
    one item at a time, e.g. by clicking an item with the mouse, see
    \l Q3ListView::SelectionMode for details.

    The list view can be navigated either using the mouse or the
    keyboard. Clicking a \bold{-} icon closes an item (hides its
    children) and clicking a \bold{+} icon opens an item (shows its
    children). The keyboard controls are these:
    \table
    \header \i Keypress \i Action
    \row \i Home
         \i Make the first item current and visible.
    \row \i End
         \i Make the last item current and visible.
    \row \i Page Up
         \i Make the item above the top visible item current and visible.
    \row \i Page Down
         \i Make the item below the bottom visible item current and visible.
    \row \i Up Arrow
         \i Make the item above the current item current and visible.
    \row \i Down Arrow
         \i Make the item below the current item current and visible.
    \row \i Left Arrow
         \i If the current item is closed (\bold{+} icon) or has no
            children, make its parent item current and visible. If the
            current item is open (\bold{-} icon) close it, i.e. hide its
            children. Exception: if the current item is the first item
            and is closed and the horizontal scroll bar is offset to
            the right the list view will be scrolled left.
    \row \i Right Arrow
         \i If the current item is closed (\bold{+} icon) and has
            children, the item is opened. If the current item is
            opened (\bold{-} icon) and has children the item's first
            child is made current and visible. If the current item has
            no children the list view is scrolled right.
    \endtable

    If the user starts typing letters with the focus in the list view
    an incremental search will occur. For example if the user types
    'd' the current item will change to the first item that begins
    with the letter 'd'; if they then type 'a', the current item will
    change to the first item that begins with 'da', and so on. If no
    item begins with the letters they type the current item doesn't
    change.

    Note that the list view's size hint is calculated taking into
    account the height \e and width to produce a nice aspect ratio.
    This may mean that you need to reimplement sizeHint() in some
    cases.

    \warning The list view assumes ownership of all list view items
    and will delete them when it does not need them any more.

    \sa Q3ListViewItem Q3CheckListItem
*/

/*!
    \fn void Q3ListView::itemRenamed(Q3ListViewItem * item, int col)

    \overload

    This signal is emitted when \a item has been renamed, e.g. by
    in-place renaming, in column \a col.

    \sa Q3ListViewItem::setRenameEnabled()
*/

/*!
    \fn void Q3ListView::itemRenamed(Q3ListViewItem * item, int col, const QString &text)

    This signal is emitted when \a item has been renamed to \a text,
    e.g. by in in-place renaming, in column \a col.

    \sa Q3ListViewItem::setRenameEnabled()
*/

/*!
    Constructs a new empty list view called \a name with parent \a
    parent and widget attributes \a f.

    This constructor sets the \c WA_StaticContent and the \c
    Qt::WA_NoBackground attributes to boost performance when drawing
    Q3ListViewItems. This may be unsuitable for custom Q3ListViewItem
    classes, in which case Qt::WA_StaticContents and Qt::WA_NoBackground
    should be cleared on the viewport() after construction.

    \sa QWidget::setAttribute()
*/
Q3ListView::Q3ListView(QWidget * parent, const char *name, Qt::WindowFlags f)
    : Q3ScrollView(parent, name, f | Qt::WStaticContents | Qt::WNoAutoErase)
{
    init();
}

void Q3ListView::init()
{
    d = new Q3ListViewPrivate;
    d->vci = 0;
    d->timer = new QTimer(this);
    d->levelWidth = 20;
    d->r = 0;
    d->rootIsExpandable = 0;
    d->h = new Q3Header(this, "list view header");
    d->h->installEventFilter(this);
    d->focusItem = 0;
    d->oldFocusItem = 0;
    d->dirtyItemTimer = new QTimer(this);
    d->visibleTimer = new QTimer(this);
    d->renameTimer = new QTimer(this);
    d->autoopenTimer = new QTimer(this);
    d->margin = 1;
    d->selectionMode = Q3ListView::Single;
    d->sortcolumn = 0;
    d->ascending = true;
    d->allColumnsShowFocus = false;
    d->fontMetricsHeight = fontMetrics().height();
    d->h->setTracking(true);
    d->buttonDown = false;
    d->ignoreDoubleClick = false;
    d->scrollTimer = 0;
    d->sortIndicator = false;
    d->clearing = false;
    d->minLeftBearing = fontMetrics().minLeftBearing();
    d->minRightBearing = fontMetrics().minRightBearing();
    d->ellipsisWidth = fontMetrics().width(QLatin1String("...")) * 2;
    d->highlighted = 0;
    d->pressedItem = 0;
    d->selectAnchor = 0;
    d->select = true;
    d->startDragItem = 0;
    d->toolTips = true;
    d->updateHeader = false;
    d->fullRepaintOnComlumnChange = false;
    d->resizeMode = NoColumn;
    d->defRenameAction = Reject;
    d->pressedEmptyArea = false;
    d->startEdit = true;
    d->ignoreEditAfterFocus = false;
    d->inMenuMode = false;
    d->pressedSelected = false;

    setMouseTracking(true);
    viewport()->setMouseTracking(true);

    connect(d->timer, SIGNAL(timeout()),
             this, SLOT(updateContents()));
    connect(d->dirtyItemTimer, SIGNAL(timeout()),
             this, SLOT(updateDirtyItems()));
    connect(d->visibleTimer, SIGNAL(timeout()),
             this, SLOT(makeVisible()));
    connect(d->renameTimer, SIGNAL(timeout()),
             this, SLOT(startRename()));
    connect(d->autoopenTimer, SIGNAL(timeout()),
             this, SLOT(openFocusItem()));

    connect(d->h, SIGNAL(sizeChange(int,int,int)),
             this, SLOT(handleSizeChange(int,int,int)));
    connect(d->h, SIGNAL(indexChange(int,int,int)),
             this, SLOT(handleIndexChange()));
    connect(d->h, SIGNAL(sectionClicked(int)),
             this, SLOT(changeSortColumn(int)));
    connect(d->h, SIGNAL(sectionHandleDoubleClicked(int)),
             this, SLOT(adjustColumn(int)));
    connect(horizontalScrollBar(), SIGNAL(sliderMoved(int)),
             d->h, SLOT(setOffset(int)));
    connect(horizontalScrollBar(), SIGNAL(valueChanged(int)),
             d->h, SLOT(setOffset(int)));

    // will access d->r
    Q3ListViewPrivate::Root * r = new Q3ListViewPrivate::Root(this);
    r->is_root = true;
    d->r = r;
    d->r->setSelectable(false);

    viewport()->setFocusProxy(this);
    viewport()->setFocusPolicy(Qt::WheelFocus);
    setFocusPolicy(Qt::WheelFocus);
    viewport()->setBackgroundRole(QPalette::Base);
    setAttribute(Qt::WA_MacShowFocusRect);
}

/*!
    \property Q3ListView::showSortIndicator
    \brief whether the list view header should display a sort indicator.

    If this property is true, an arrow is drawn in the header of the
    list view to indicate the sort order of the list view contents.
    The arrow will be drawn in the correct column and will point up or
    down, depending on the current sort direction. The default is
    false (don't show an indicator).

    \sa Q3Header::setSortIndicator()
*/

void Q3ListView::setShowSortIndicator(bool show)
{
    if (show == d->sortIndicator)
        return;

    d->sortIndicator = show;
    if (d->sortcolumn != Unsorted && d->sortIndicator)
        d->h->setSortIndicator(d->sortcolumn, d->ascending);
    else
        d->h->setSortIndicator(-1);
}

bool Q3ListView::showSortIndicator() const
{
    return d->sortIndicator;
}

/*!
    \property Q3ListView::showToolTips
    \brief whether this list view should show tooltips for truncated column texts

    The default is true.
*/

void Q3ListView::setShowToolTips(bool b)
{
    d->toolTips = b;
}

bool Q3ListView::showToolTips() const
{
    return d->toolTips;
}

/*!
    \property Q3ListView::resizeMode
    \brief whether all, none or the only the last column should be resized

    Specifies whether all, none or only the last column should be
    resized to fit the full width of the list view. The values for this
    property can be one of: \c NoColumn (the default), \c AllColumns
    or \c LastColumn.

    \warning Setting the resize mode should be done after all necessary
    columns have been added to the list view, otherwise the behavior is
    undefined.

    \sa Q3Header, header()
*/

void Q3ListView::setResizeMode(ResizeMode m)
{
    d->resizeMode = m;
    if (m == NoColumn)
        header()->setStretchEnabled(false);
    else if (m == AllColumns)
        header()->setStretchEnabled(true);
    else
        header()->setStretchEnabled(true, header()->count() - 1);
}

Q3ListView::ResizeMode Q3ListView::resizeMode() const
{
    return d->resizeMode;
}

/*!
    Destroys the list view, deleting all its items, and frees up all
    allocated resources.
*/

Q3ListView::~Q3ListView()
{
    for (int j = 0; j < d->iterators.size(); ++j) {
        Q3ListViewItemIterator *i = d->iterators.at(j);
        i->listView = 0;
    }

    d->focusItem = 0;
    delete d->r;
    d->r = 0;
    delete d->vci;
    d->vci = 0;
#if 0
    delete d->toolTip;
    d->toolTip = 0;
#endif
    delete d;
    d = 0;
}


/*!
    Calls Q3ListViewItem::paintCell() and
    Q3ListViewItem::paintBranches() as necessary for all list view
    items that require repainting in the \a cw pixels wide and \a ch
    pixels high bounding rectangle starting at position \a cx, \a cy
    with offset \a ox, \a oy. Uses the painter \a p.
*/

void Q3ListView::drawContentsOffset(QPainter * p, int ox, int oy,
                                    int cx, int cy, int cw, int ch)
{
    if (columns() == 0) {
        paintEmptyArea(p, QRect(cx, cy, cw, ch));
        return;
    }

    if (d->drawables.isEmpty() ||
         d->topPixel > cy ||
         d->bottomPixel < cy + ch - 1 ||
         d->r->maybeTotalHeight < 0)
        buildDrawableList();

    if (!d->dirtyItems.isEmpty()) {
        QRect br(cx - ox, cy - oy, cw, ch);
        for (int i = 0; i < d->dirtyItems.size(); ++i) {
            const Q3ListViewItem * item = d->dirtyItems.at(i);
            QRect ir = itemRect(item).intersected(viewport()->visibleRect());
            if (ir.isEmpty() || br.contains(ir))
                // we're painting this one, or it needs no painting: forget it
                d->dirtyItems.removeAt(i);
        }
        if (d->dirtyItems.count()) {
            // there are still items left that need repainting
            d->dirtyItemTimer->start(0, true);
        } else {
            // we're painting all items that need to be painted
            d->dirtyItems.clear();
            d->dirtyItemTimer->stop();
        }
    }

    p->setFont(font());

    QRect r;
    int fx = -1, x, fc = 0, lc = 0;
    int tx = -1;

    for (int i = 0; i < d->drawables.size(); ++i) {
        Q3ListViewPrivate::DrawableItem current = d->drawables.at(i);
        if (!current.i->isVisible())
            continue;
        int ih = current.i->height();
        int ith = current.i->totalHeight();
        int c;
        int cs;

        // need to paint current?
        if (ih > 0 && current.y < cy+ch && current.y+ih > cy) {
            if (fx < 0) {
                // find first interesting column, once
                x = 0;
                c = 0;
                cs = d->h->cellSize(0);
                while (x + cs <= cx && c < d->h->count()) {
                    x += cs;
                    c++;
                    if (c < d->h->count())
                        cs = d->h->cellSize(c);
                }
                fx = x;
                fc = c;
                while(x < cx + cw && c < d->h->count()) {
                    x += cs;
                    c++;
                    if (c < d->h->count())
                        cs = d->h->cellSize(c);
                }
                lc = c;
            }

            x = fx;
            c = fc;
            // draw to last interesting column

            bool drawActiveSelection = hasFocus() || d->inMenuMode ||
                            !style()->styleHint(QStyle::SH_ItemView_ChangeHighlightOnFocus, 0, this)
                            || (currentItem() && currentItem()->renameBox
                                && currentItem()->renameBox->hasFocus());
            QPalette pal = palette();
            if(!drawActiveSelection)
                pal.setCurrentColorGroup(QPalette::Inactive);

            while (c < lc) {
                int i = d->h->mapToLogical(c);
                cs = d->h->cellSize(c);
                r.setRect(x - ox, current.y - oy, cs, ih);
                if (i == 0 && current.i->parentItem)
                    r.setLeft(r.left() + current.l * treeStepSize());

                p->save();
                // No need to paint if the cell isn't technically visible
                if (!(r.width() == 0 || r.height() == 0)) {
                    p->translate(r.left(), r.top());
                    int ac = d->h->mapToLogical(c);
                    // map to Left currently. This should change once we
                    // can really reverse the listview.
                    int align = columnAlignment(ac);
                    if (align == Qt::AlignAuto) align = Qt::AlignLeft;
                        current.i->paintCell(p, pal, ac, r.width(), align);
                }
                p->restore();
                x += cs;
                c++;
            }

            if (current.i == d->focusItem && hasFocus() &&
                 !d->allColumnsShowFocus) {
                p->save();
                int cell = d->h->mapToActual(0);
                QRect r(d->h->cellPos(cell) - ox, current.y - oy, d->h->cellSize(cell), ih);
                if (current.i->parentItem)
                    r.setLeft(r.left() + current.l * treeStepSize());
                if (r.left() < r.right())
                    current.i->paintFocus(p, palette(), r);
                p->restore();
            }
        }

        const int cell = d->h->mapToActual(0);

        // does current need focus indication?
        if (current.i == d->focusItem && hasFocus() &&
             d->allColumnsShowFocus) {
            p->save();
            int x = -contentsX();
            int w = header()->cellPos(header()->count() - 1) +
                    header()->cellSize(header()->count() - 1);

            r.setRect(x, current.y - oy, w, ih);
            if (d->h->mapToActual(0) == 0 || (current.l == 0 && !rootIsDecorated())) {
                int offsetx = qMin(current.l * treeStepSize(), d->h->cellSize(cell));
                r.setLeft(r.left() + offsetx);
                current.i->paintFocus(p, palette(), r);
            } else {
                int xdepth = qMin(treeStepSize() * (current.i->depth() + (rootIsDecorated() ? 1 : 0))
                             + itemMargin(), d->h->cellSize(cell));
                xdepth += d->h->cellPos(cell);
                QRect r1(r);
                r1.setRight(d->h->cellPos(cell) - 1);
                QRect r2(r);
                r2.setLeft(xdepth - 1);
                current.i->paintFocus(p, palette(), r1);
                current.i->paintFocus(p, palette(), r2);
            }
            p->restore();
        }

        if (tx < 0)
            tx = d->h->cellPos(cell);

        // do any children of current need to be painted?
        if (ih != ith &&
             (current.i != d->r || d->rootIsExpandable) &&
             current.y + ith > cy &&
             current.y + ih < cy + ch &&
             tx + current.l * treeStepSize() < cx + cw &&
             tx + (current.l+1) * treeStepSize() > cx) {
            // compute the clip rectangle the safe way

            int rtop = current.y + ih;
            int rbottom = current.y + ith;
            int rleft = tx + current.l*treeStepSize();
            int rright = rleft + treeStepSize();

            int crtop = qMax(rtop, cy);
            int crbottom = qMin(rbottom, cy+ch);
            int crleft = qMax(rleft, cx);
            int crright = qMin(rright, cx+cw);

            r.setRect(crleft-ox, crtop-oy,
                       crright-crleft, crbottom-crtop);

            if (r.isValid()) {
                p->save();
                p->setClipRect(QRect(d->h->cellPos(cell), 0, d->h->cellSize(cell), height()));
                p->translate(rleft-ox, crtop-oy);

                current.i->paintBranches(p, palette(), treeStepSize(),
                                           rtop - crtop, r.height());
                p->restore();
            }
        }
    }

    if (d->r->totalHeight() < cy + ch)
        paintEmptyArea(p, QRect(cx - ox, d->r->totalHeight() - oy,
                                  cw, cy + ch - d->r->totalHeight()));

    int c = d->h->count()-1;
    if (c >= 0 &&
         d->h->cellPos(c) + d->h->cellSize(c) < cx + cw) {
        c = d->h->cellPos(c) + d->h->cellSize(c);
        paintEmptyArea(p, QRect(c - ox, cy - oy, cx + cw - c, ch));
    }
}



/*!
    Paints \a rect so that it looks like empty background using
    painter \a p. \a rect is in widget coordinates, ready to be fed to
    \a p.

    The default function fills \a rect with the
    viewport()->backgroundBrush().
*/

void Q3ListView::paintEmptyArea(QPainter * p, const QRect & rect)
{
    QStyleOptionQ3ListView opt = getStyleOption(this, 0);
    opt.rect = rect;
    opt.sortColumn = d->sortcolumn;
    opt.subControls = QStyle::SC_Q3ListView;
    style()->drawComplexControl(QStyle::CC_Q3ListView, &opt, p, this);
}


/*
    Rebuilds the list of drawable Q3ListViewItems. This function is
    const so that const functions can call it without requiring
    d->drawables to be mutable.
*/

void Q3ListView::buildDrawableList() const
{
    d->r->enforceSortOrder();

    QStack<Q3ListViewPrivate::DrawableItem> stack;
    Q3ListViewPrivate::DrawableItem di(((int)d->rootIsExpandable)-1, 0, d->r);
    stack.push(di);

    Q3ListView *that = const_cast<Q3ListView *>(this);

    // could mess with cy and ch in order to speed up vertical
    // scrolling
    int cy = contentsY();
    int ch = that->visibleHeight();
    d->topPixel = cy + ch; // one below bottom
    d->bottomPixel = cy - 1; // one above top

    that->d->drawables.clear();

    while (!stack.isEmpty()) {
        Q3ListViewPrivate::DrawableItem cur = stack.pop();

        int ih = cur.i->height();
        int ith = cur.i->totalHeight();

        // is this item, or its branch symbol, inside the viewport?
        if (cur.y + ith >= cy && cur.y < cy + ch) {
            that->d->drawables.append(cur);
            // perhaps adjust topPixel up to this item?  may be adjusted
            // down again if any children are not to be painted
            if (cur.y < d->topPixel)
                d->topPixel = cur.y;
            // bottompixel is easy: the bottom item drawn contains it
            d->bottomPixel = cur.y + ih - 1;
        }

        // push younger sibling of cur on the stack?
        if (cur.y + ith < cy+ch && cur.i->siblingItem)
            stack.push(Q3ListViewPrivate::DrawableItem(cur.l, cur.y + ith, cur.i->siblingItem));

        // do any children of cur need to be painted?
        if (cur.i->isOpen() && cur.i->childCount() &&
             cur.y + ith > cy &&
             cur.y + ih < cy + ch) {
            cur.i->enforceSortOrder();

            Q3ListViewItem * c = cur.i->childItem;
            int y = cur.y + ih;

            // if any of the children are not to be painted, skip them
            // and invalidate topPixel
            while (c && y + c->totalHeight() <= cy) {
                y += c->totalHeight();
                c = c->siblingItem;
                d->topPixel = cy + ch;
            }

            // push one child on the stack, if there is at least one
            // needing to be painted
            if (c && y < cy+ch)
                stack.push(Q3ListViewPrivate::DrawableItem(cur.l + 1, y, c));
        }
    }
}

/*!
    \property Q3ListView::treeStepSize
    \brief the number of pixels a child is offset from its parent

    The default is 20 pixels.

    Of course, this property is only meaningful for hierarchical list
    views.
*/

int Q3ListView::treeStepSize() const
{
    return d->levelWidth;
}

void Q3ListView::setTreeStepSize(int size)
{
    if (size != d->levelWidth) {
        d->levelWidth = size;
        viewport()->repaint();
    }
}

/*!
    Inserts item \a i into the list view as a top-level item. You do
    not need to call this unless you've called takeItem(\a i) or
    Q3ListViewItem::takeItem(\a i) and need to reinsert \a i elsewhere.

    \sa Q3ListViewItem::takeItem() takeItem()
*/

void Q3ListView::insertItem(Q3ListViewItem * i)
{
    if (d->r) // not for d->r itself
        d->r->insertItem(i);
}


/*!
    Removes and deletes all the items in this list view and triggers
    an update.

    \sa triggerUpdate()
*/

void Q3ListView::clear()
{
    bool wasUpdatesEnabled = viewport()->updatesEnabled();
    if (wasUpdatesEnabled)
        viewport()->setUpdatesEnabled(false);
    setContentsPos(0, 0);
    viewport()->setUpdatesEnabled(wasUpdatesEnabled);	

    bool block = signalsBlocked();
    blockSignals(true);
    d->clearing = true;
    clearSelection();
    for (int j = 0; j < d->iterators.size(); ++j) {
        Q3ListViewItemIterator *i = d->iterators.at(j);
            i->curr = 0;
    }

    d->drawables.clear();
    d->dirtyItems.clear();
    d->dirtyItemTimer->stop();

    d->highlighted = 0;
    d->focusItem = 0;
    d->selectAnchor = 0;
    d->pressedItem = 0;
    d->startDragItem = 0;

    // if it's down its downness makes no sense, so undown it
    d->buttonDown = false;

    Q3ListViewItem *c = (Q3ListViewItem *)d->r->firstChild();
    Q3ListViewItem *n;
    while(c) {
        n = (Q3ListViewItem *)c->nextSibling();
        delete c;
        c = n;
    }
    resizeContents(d->h->sizeHint().width(), contentsHeight());
    delete d->r;
    d->r = 0;
    Q3ListViewPrivate::Root * r = new Q3ListViewPrivate::Root(this);
    r->is_root = true;
    d->r = r;
    d->r->setSelectable(false);
    blockSignals(block);
    triggerUpdate();
    d->clearing = false;
}

/*!
    \reimp
*/

void Q3ListView::setContentsPos(int x, int y)
{
    updateGeometries();
    Q3ScrollView::setContentsPos(x, y);
}

/*!
    Adds a \a width pixels wide column with the column header \a label
    to the list view, and returns the index of the new column.

    All columns apart from the first one are inserted to the right of
    the existing ones.

    If \a width is negative, the new column's \l WidthMode is set to
    \c Maximum instead of \c Manual.

    \sa setColumnText() setColumnWidth() setColumnWidthMode()
*/
int Q3ListView::addColumn(const QString &label, int width)
{
    int c = d->h->addLabel(label, width);
    d->column.resize(c+1);
    d->column[c].wmode = (width >= 0 ? Manual : Maximum);
    updateGeometries();
    updateGeometry();
    return c;
}

/*!
    \overload

    Adds a \a width pixels wide new column with the header \a label
    and the \a icon to the list view, and returns the index of the
    column.

    If \a width is negative, the new column's \l WidthMode is set to
    \c Maximum, and to \c Manual otherwise.

    \sa setColumnText() setColumnWidth() setColumnWidthMode()
*/
int Q3ListView::addColumn(const QIcon& icon, const QString &label, int width)
{
    int c = d->h->addLabel(icon, label, width);
    d->column.resize(c+1);
    d->column[c].wmode = (width >= 0 ? Manual : Maximum);
    updateGeometries();
    updateGeometry();
    return c;
}

/*!
    \property Q3ListView::columns
    \brief the number of columns in this list view

    \sa addColumn(), removeColumn()
*/

int Q3ListView::columns() const
{
    return d->column.count();
}

/*!
    Removes the column at position \a index.
*/

void Q3ListView::removeColumn(int index)
{
    if (index < 0 || index > (int)d->column.count() - 1)
        return;

    if (d->vci) {
        Q3ListViewPrivate::ViewColumnInfo *vi = d->vci, *prev = 0, *next = 0;
        for (int i = 0; i < index; ++i) {
            if (vi) {
                prev = vi;
                vi = vi->next;
            }
        }
        if (vi) {
            next = vi->next;
            if (prev)
                prev->next = next;
            vi->next = 0;
            delete vi;
            if (index == 0)
                d->vci = next;
        }
    }

    Q3ListViewItemIterator it(this);
    for (; it.current(); ++it) {
        Q3ListViewPrivate::ItemColumnInfo *ci = (Q3ListViewPrivate::ItemColumnInfo*)it.current()->columns;
        if (ci) {
            Q3ListViewPrivate::ItemColumnInfo *prev = 0, *next = 0;
            for (int i = 0; i < index; ++i) {
                if (ci) {
                    prev = ci;
                    ci = ci->next;
                }
            }
            if (ci) {
                next = ci->next;
                if (prev)
                    prev->next = next;
                ci->next = 0;
                delete ci;
                if (index == 0)
                    it.current()->columns = next;
            }
        }
    }

    for (int i = index; i < (int)d->column.size() - 1; ++i)
        d->column[i] = d->column[i + 1];
    d->column.resize(d->column.size() - 1);

    d->h->removeLabel(index);
    if (d->resizeMode == LastColumn)
        d->h->setStretchEnabled(true, d->h->count() - 1);

    updateGeometries();
    if (d->column.count() == 0)
        clear();
    updateGeometry();
    viewport()->update();
}

/*!
    Sets the heading of column \a column to \a label.

    \sa columnText()
*/
void Q3ListView::setColumnText(int column, const QString &label)
{
    if (column < d->h->count()) {
        d->h->setLabel(column, label);
        updateGeometries();
        updateGeometry();
    }
}

/*!
    \overload

    Sets the heading of column \a column to \a icon and \a label.

    \sa columnText()
*/
void Q3ListView::setColumnText(int column, const QIcon& icon, const QString &label)
{
    if (column < d->h->count()) {
        d->h->setLabel(column, icon, label);
        updateGeometries();
    }
}

/*!
    Sets the width of column \a column to \a w pixels. Note that if
    the column has a \c WidthMode other than \c Manual, this width
    setting may be subsequently overridden.

    \sa columnWidth()
*/
void Q3ListView::setColumnWidth(int column, int w)
{
    int oldw = d->h->sectionSize(column);
    if (column < d->h->count() && oldw != w) {
        d->h->resizeSection(column, w);
        disconnect(d->h, SIGNAL(sizeChange(int,int,int)),
                 this, SLOT(handleSizeChange(int,int,int)));
        emit d->h->sizeChange(column, oldw, w);
        connect(d->h, SIGNAL(sizeChange(int,int,int)),
                 this, SLOT(handleSizeChange(int,int,int)));
        viewport()->update();
    }
}


/*!
    Returns the text of column \a c.

    \sa setColumnText()
*/

QString Q3ListView::columnText(int c) const
{
    return d->h->label(c);
}

/*!
    Returns the width of column \a c.

    \sa setColumnWidth()
*/

int Q3ListView::columnWidth(int c) const
{
    int actual = d->h->mapToActual(c);
    return d->h->cellSize(actual);
}


/*!
    \enum Q3ListView::WidthMode

    This enum type describes how the width of a column in the view
    changes.

    \value Manual the column width does not change automatically.

    \value Maximum  the column is automatically sized according to the
    widths of all items in the column. (Note: The column never shrinks
    in this case.) This means that the column is always resized to the
    width of the item with the largest width in the column.

    \sa setColumnWidth() setColumnWidthMode() columnWidth()
*/


/*!
    Sets column \a{c}'s width mode to \a mode. The default depends on
    the original width argument to addColumn().

    \sa Q3ListViewItem::width()
*/

void Q3ListView::setColumnWidthMode(int c, WidthMode mode)
{
    if (c >= 0 && c < d->h->count())
         d->column[c].wmode = mode;
}


/*!
    Returns the \c WidthMode for column \a c.

    \sa setColumnWidthMode()
*/

Q3ListView::WidthMode Q3ListView::columnWidthMode(int c) const
{
    if (c >= 0 && c < d->h->count())
        return d->column[c].wmode;
    else
        return Manual;
}


/*!
    Sets column \a{column}'s alignment to \a align. The alignment is
    ultimately passed to Q3ListViewItem::paintCell() for each item in
    the list view. For horizontally aligned text with Qt::AlignLeft or
    Qt::AlignHCenter the ellipsis (...) will be to the right, for
    Qt::AlignRight the ellipsis will be to the left.

    \sa Qt::Alignment
*/

void Q3ListView::setColumnAlignment(int column, int align)
{
    if (column < 0)
        return;
    if (!d->vci)
        d->vci = new Q3ListViewPrivate::ViewColumnInfo;
    Q3ListViewPrivate::ViewColumnInfo * l = d->vci;
    while(column) {
        if (!l->next)
            l->next = new Q3ListViewPrivate::ViewColumnInfo;
        l = l->next;
        column--;
    }
    if (l->align == align)
        return;
    l->align = align;
    triggerUpdate();
}


/*!
    Returns the alignment of column \a column. The default is \c
    Qt::AlignAuto.

    \sa Qt::Alignment
*/

int Q3ListView::columnAlignment(int column) const
{
    if (column < 0 || !d->vci)
        return Qt::AlignAuto;
    Q3ListViewPrivate::ViewColumnInfo * l = d->vci;
    while(column) {
        if (!l->next)
            l->next = new Q3ListViewPrivate::ViewColumnInfo;
        l = l->next;
        column--;
    }
    return l ? l->align : Qt::AlignAuto;
}



/*!
    \internal
*/
void Q3ListView::show()
{
    // Reimplemented to setx the correct background mode and viewed
    // area size.
    if (!isVisible()) {
        reconfigureItems();
        updateGeometries();
    }
    Q3ScrollView::show();
}


/*!
    Updates the sizes of the viewport, header, scroll bars and so on.

    \warning Don't call this directly; call triggerUpdate() instead.
*/

void Q3ListView::updateContents()
{
    if (d->updateHeader)
        header()->adjustHeaderSize();
    d->updateHeader = false;
    if (!isVisible()) {
        // Not in response to a setText/setPixmap any more.
        return;
    }
    d->drawables.clear();
    viewport()->setUpdatesEnabled(false);
    updateGeometries();
    viewport()->setUpdatesEnabled(true);
    viewport()->repaint();
}


void Q3ListView::updateGeometries()
{
    int th = d->r->totalHeight();
    int tw = d->h->headerWidth();
    if (d->h->offset() &&
         tw < d->h->offset() + d->h->width())
        horizontalScrollBar()->setValue(tw - Q3ListView::d->h->width());
#if 0
    if (QApplication::reverseLayout() && d->h->offset() != horizontalScrollBar()->value())
        horizontalScrollBar()->setValue(d->h->offset());
#endif
    verticalScrollBar()->raise();
    resizeContents(tw, th);
    d->drawables.clear();
    if (d->h->isHidden()) {
        setMargins(0, 0, 0, 0);
    } else {
        QSize hs(d->h->sizeHint());
        setMargins(0, hs.height(), 0, 0);
        d->h->setGeometry(viewport()->x(), viewport()->y()-hs.height(),
                           visibleWidth(), hs.height());
    }
}


/*!
    Updates the display when the section \a section has changed size
    from the old size, \a os, to the new size, \a ns.
*/

void Q3ListView::handleSizeChange(int section, int os, int ns)
{
    bool upe = viewport()->updatesEnabled();
    if (upe)
        viewport()->setUpdatesEnabled(false);
    viewport()->setAttribute(Qt::WA_UpdatesDisabled, true);
    int sx = horizontalScrollBar()->value();
    bool sv = horizontalScrollBar()->isVisible();
    updateGeometries();
    bool fullRepaint = d->fullRepaintOnComlumnChange || sx != horizontalScrollBar()->value()
                       || sv != horizontalScrollBar()->isVisible();
    d->fullRepaintOnComlumnChange = false;
    if (upe)
        viewport()->setUpdatesEnabled(true);

    if (fullRepaint) {
        viewport()->repaint();
        return;
    }

    int actual = d->h->mapToActual(section);
    int dx = ns - os;
    int left = d->h->cellPos(actual) - contentsX() + d->h->cellSize(actual);
    if (dx > 0)
        left -= dx;
    if (left < visibleWidth())
        viewport()->scroll(dx, 0, QRect(left, 0, visibleWidth() - left, visibleHeight()));
    viewport()->repaint(left - 4 - d->ellipsisWidth, 0, 4 + d->ellipsisWidth,
                        visibleHeight()); // border between the items and ellipses width

    // map auto to left for now. Need to fix this once we support
    // reverse layout on the listview.
    int align = columnAlignment(section);
    if (align == Qt::AlignAuto) align = Qt::AlignLeft;
    if (align != Qt::AlignAuto && align != Qt::AlignLeft)
        viewport()->repaint(d->h->cellPos(actual) - contentsX(), 0,
                             d->h->cellSize(actual), visibleHeight());

    if (currentItem() && currentItem()->renameBox) {
        QRect r = itemRect(currentItem());
        r = QRect(viewportToContents(r.topLeft()), r.size());
        r.setLeft(header()->sectionPos(currentItem()->renameCol));
        r.setWidth(header()->sectionSize(currentItem()->renameCol) - 1);
        if (currentItem()->renameCol == 0)
            r.setLeft(r.left() + itemMargin() + (currentItem()->depth() +
                                                   (rootIsDecorated() ? 1 : 0)) * treeStepSize() - 1);
        if (currentItem()->pixmap(currentItem()->renameCol))
            r.setLeft(r.left() + currentItem()->pixmap(currentItem()->renameCol)->width());
        if (r.x() - contentsX() < 0)
            r.setX(contentsX());
        if (r.width() > visibleWidth())
            r.setWidth(visibleWidth());
        addChild(currentItem()->renameBox, r.x(), r.y());
        currentItem()->renameBox->resize(r.size());
    }
}


/*
    Very smart internal slot that repaints \e only the items that need
    to be repainted. Don't use this directly; call repaintItem()
    instead.
*/

void Q3ListView::updateDirtyItems()
{
    if (d->timer->isActive() || d->dirtyItems.isEmpty())
        return;
    QRect ir;
    for (int i = 0; i < d->dirtyItems.size(); ++i) {
        const Q3ListViewItem *item = d->dirtyItems.at(i);
        ir = ir.united(itemRect(item));
    }
    d->dirtyItems.clear();
    if (!ir.isEmpty())  {                      // rectangle to be repainted
        if (ir.x() < 0)
            ir.moveBy(-ir.x(), 0);
        viewport()->repaint(ir);
    }
}


void Q3ListView::makeVisible()
{
    if (d->focusItem)
        ensureItemVisible(d->focusItem);
}


/*!
    Ensures that the header is correctly sized and positioned when the
    resize event \a e occurs.
*/

void Q3ListView::resizeEvent(QResizeEvent *e)
{
    Q3ScrollView::resizeEvent(e);
    d->fullRepaintOnComlumnChange = true;
    d->h->resize(visibleWidth(), d->h->height());
    d->h->adjustHeaderSize();
}

/*! \reimp */

void Q3ListView::viewportResizeEvent(QResizeEvent *e)
{
    Q3ScrollView::viewportResizeEvent(e);
    d->h->resize(visibleWidth(), d->h->height());
    if (resizeMode() != NoColumn && currentItem() && currentItem()->renameBox) {
        QRect r = itemRect(currentItem());
        r = QRect(viewportToContents(r.topLeft()), r.size());
        r.setLeft(header()->sectionPos(currentItem()->renameCol));
        r.setWidth(header()->sectionSize(currentItem()->renameCol) - 1);
        if (currentItem()->renameCol == 0)
            r.setLeft(r.left() + itemMargin() + (currentItem()->depth() +
                                                   (rootIsDecorated() ? 1 : 0)) * treeStepSize() - 1);
        if (currentItem()->pixmap(currentItem()->renameCol))
            r.setLeft(r.left() + currentItem()->pixmap(currentItem()->renameCol)->width());
        if (r.x() - contentsX() < 0)
            r.setX(contentsX());
        if (r.width() > visibleWidth())
            r.setWidth(visibleWidth());
        addChild(currentItem()->renameBox, r.x(), r.y());
        currentItem()->renameBox->resize(r.size());
    }
}

/*!
    Triggers a size, geometry and content update during the next
    iteration of the event loop. Ensures that there'll be just one
    update to avoid flicker.
*/

void Q3ListView::triggerUpdate()
{
    if (!isVisible() || !updatesEnabled()) {
        // Not in response to a setText/setPixmap any more.
        return; // it will update when shown, or something.
    }

    d->timer->start(0, true);
}


/*!
    Redirects the event \a e relating to object \a o, for the viewport
    to mousePressEvent(), keyPressEvent() and friends.
*/

bool Q3ListView::eventFilter(QObject * o, QEvent * e)
{
    if (o == d->h &&
         e->type() >= QEvent::MouseButtonPress &&
         e->type() <= QEvent::MouseMove) {
        QMouseEvent * me = (QMouseEvent *)e;
        QMouseEvent me2(me->type(),
                         QPoint(me->pos().x(),
                                 me->pos().y() - d->h->height()),
                         me->button(), me->state());
        switch(me2.type()) {
        case QEvent::MouseButtonDblClick:
            if (me2.button() == Qt::RightButton)
                return true;
            break;
        case QEvent::MouseMove:
            if (me2.state() & Qt::RightButton) {
                viewportMouseMoveEvent(&me2);
                return true;
            }
            break;
        default:
            break;
        }
    } else if (o == viewport()) {
        QFocusEvent * fe = (QFocusEvent *)e;

        switch(e->type()) {
        case QEvent::FocusIn:
            focusInEvent(fe);
            return true;
        case QEvent::FocusOut:
            focusOutEvent(fe);
            return true;
#ifndef QT_NO_TOOLTIP
        case QEvent::ToolTip:
        {
            if (!showToolTips())
                return false;

            QHelpEvent *he = static_cast<QHelpEvent *>(e);
            Q3ListViewItem *item = itemAt(he->pos());
            QPoint contentsPos = viewportToContents(he->pos());
            if (!item || !item->columns) {
                QToolTip::showText(he->globalPos(), QString(), viewport());
                return true;
            }
            int col = header()->sectionAt(contentsPos.x());
            Q3ListViewPrivate::ItemColumnInfo *ci = (Q3ListViewPrivate::ItemColumnInfo*)item->columns;
            for (int i = 0; ci && (i < col); ++i)
                ci = ci->next;

            if (!ci || !ci->truncated)
                QToolTip::showText(he->globalPos(), QString(), viewport());
            else
                QToolTip::showText(he->globalPos(), item->text(col), viewport());
            return true;
        }
#endif
        default:
            // nothing
            break;
        }
    } else if (qobject_cast<QLineEdit*>(o)) {
        if (currentItem() && currentItem()->renameBox) {
            if (e->type() == QEvent::KeyPress) {
                QKeyEvent *ke = (QKeyEvent*)e;
                if (ke->key() == Qt::Key_Return ||
                     ke->key() == Qt::Key_Enter) {
                    currentItem()->okRename(currentItem()->renameCol);
                    return true;
                } else if (ke->key() == Qt::Key_Escape) {
                    currentItem()->cancelRename(currentItem()->renameCol);
                    return true;
                }
            } else if (e->type() == QEvent::FocusOut) {
                if (((QFocusEvent*)e)->reason() != Qt::PopupFocusReason) {
                    QCustomEvent *e = new QCustomEvent(9999);
                    QApplication::postEvent(o, e);
                    return true;
                }
            } else if (e->type() == 9999) {
                if (d->defRenameAction == Reject)
                    currentItem()->cancelRename(currentItem()->renameCol);
                else
                    currentItem()->okRename(currentItem()->renameCol);
                return true;
            }
        }
    }

    return Q3ScrollView::eventFilter(o, e);
}


/*!
    Returns a pointer to the list view containing this item.

    Note that this function traverses the items to the root to find the
    listview. This function will return 0 for taken items - see
    Q3ListViewItem::takeItem()
*/

Q3ListView * Q3ListViewItem::listView() const
{
    const Q3ListViewItem* c = this;
    while (c && !c->is_root)
        c = c->parentItem;
    if (!c)
        return 0;
    return ((Q3ListViewPrivate::Root*)c)->theListView();
}


/*!
    Returns the depth of this item.
*/
int Q3ListViewItem::depth() const
{
    return parentItem ? parentItem->depth()+1 : -1; // -1 == the hidden root
}


/*!
    Returns a pointer to the item immediately above this item on the
    screen. This is usually the item's closest older sibling, but it
    may also be its parent or its next older sibling's youngest child,
    or something else if anyoftheabove->height() returns 0. Returns 0
    if there is no item immediately above this item.

    This function assumes that all parents of this item are open (i.e.
    that this item is visible, or can be made visible by scrolling).

    This function might be relatively slow because of the tree
    traversions needed to find the correct item.

    \sa itemBelow() Q3ListView::itemRect()
*/

Q3ListViewItem * Q3ListViewItem::itemAbove() const
{
    if (!parentItem)
        return 0;

    Q3ListViewItem * c = parentItem;
    if (c->childItem != this) {
        c = c->childItem;
        while(c && c->siblingItem != this)
            c = c->siblingItem;
        if (!c)
            return 0;
        while(c->isOpen() && c->childItem) {
            c = c->childItem;
            while(c->siblingItem)
                c = c->siblingItem;                // assign c's sibling to c
        }
    }
    if (c && (!c->height() || !c->isEnabled()))
        return c->itemAbove();
    return c;
}


/*!
    Returns a pointer to the item immediately below this item on the
    screen. This is usually the item's eldest child, but it may also
    be its next younger sibling, its parent's next younger sibling,
    grandparent's, etc., or something else if anyoftheabove->height()
    returns 0. Returns 0 if there is no item immediately below this
    item.

    This function assumes that all parents of this item are open (i.e.
    that this item is visible or can be made visible by scrolling).

    \sa itemAbove() Q3ListView::itemRect()
*/

Q3ListViewItem * Q3ListViewItem::itemBelow() const
{
    Q3ListViewItem * c = 0;
    if (isOpen() && childItem) {
        c = childItem;
    } else if (siblingItem) {
        c = siblingItem;
    } else if (parentItem) {
        c = const_cast<Q3ListViewItem*>(this);
        do {
            c = c->parentItem;
        } while(c->parentItem && !c->siblingItem);
        if (c)
            c = c->siblingItem;
    }
    if (c && (!c->height() || !c->isEnabled()))
        return c->itemBelow();
    return c;
}


/*!
    \fn bool Q3ListViewItem::isOpen() const

    Returns true if this list view item has children \e and they are
    not explicitly hidden; otherwise returns false.

    \sa setOpen()
*/

/*!
    Returns the first (top) child of this item, or 0 if this item has
    no children.

    Note that the children are not guaranteed to be sorted properly.
    Q3ListView and Q3ListViewItem try to postpone or avoid sorting to
    the greatest degree possible, in order to keep the user interface
    snappy.

    \sa nextSibling() sortChildItems()
*/

Q3ListViewItem* Q3ListViewItem::firstChild() const
{
    enforceSortOrder();
    return childItem;
}


/*!
    Returns the parent of this item, or 0 if this item has no parent.

    \sa firstChild(), nextSibling()
*/

Q3ListViewItem* Q3ListViewItem::parent() const
{
    if (!parentItem || parentItem->is_root) return 0;
    return parentItem;
}


/*!
    \fn Q3ListViewItem* Q3ListViewItem::nextSibling() const

    Returns the sibling item below this item, or 0 if there is no
    sibling item after this item.

    Note that the siblings are not guaranteed to be sorted properly.
    Q3ListView and Q3ListViewItem try to postpone or avoid sorting to
    the greatest degree possible, in order to keep the user interface
    snappy.

    \sa firstChild() sortChildItems()
*/

/*!
    \fn int Q3ListViewItem::childCount () const

    Returns how many children this item has. The count only includes
    the item's immediate children.
*/


/*!
    Returns the height of this item in pixels. This does not include
    the height of any children; totalHeight() returns that.
*/
int Q3ListViewItem::height() const
{
    Q3ListViewItem * that = (Q3ListViewItem *)this;
    if (!that->configured) {
        that->configured = true;
        that->setup(); // ### virtual non-const function called in const
    }

    return visible ? ownHeight : 0;
}

/*!
    Call this function when the value of width() may have changed for
    column \a c. Normally, you should call this if text(c) changes.
    Passing -1 for \a c indicates that all columns may have changed.
    It is more efficient to pass -1 if two or more columns have
    changed than to call widthChanged() separately for each one.

    \sa width()
*/
void Q3ListViewItem::widthChanged(int c) const
{
    Q3ListView *lv = listView();
    if (lv)
        lv->widthChanged(this, c);
}

/*!
    \fn void  Q3ListView::dropped (QDropEvent * e)

    This signal is emitted, when a drop event occurred on the
    viewport (not onto an item).

    \a e provides all the information about the drop.
*/

/*!
    \fn void Q3ListView::selectionChanged()

    This signal is emitted whenever the set of selected items has
    changed (normally before the screen update). It is available both
    in \c Single selection and \c Multi selection mode but is most
    useful in \c Multi selection mode.

    \warning Do not delete any Q3ListViewItem objects in slots
    connected to this signal.

    \sa setSelected() Q3ListViewItem::setSelected()
*/


/*!
    \fn void Q3ListView::pressed(Q3ListViewItem *item)

    This signal is emitted whenever the user presses the mouse button
    in a list view. \a item is the list view item on which the user
    pressed the mouse button, or 0 if the user didn't press the mouse
    on an item.

    \warning Do not delete any Q3ListViewItem objects in slots
    connected to this signal.
*/

/*!
    \fn void Q3ListView::pressed(Q3ListViewItem *item, const QPoint &pnt, int c)

    \overload

    This signal is emitted whenever the user presses the mouse button
    in a list view. \a item is the list view item on which the user
    pressed the mouse button, or 0 if the user didn't press the mouse
    on an item. \a pnt is the position of the mouse cursor in global
    coordinates, and \a c is the column where the mouse cursor was
    when the user pressed the mouse button.

    \warning Do not delete any Q3ListViewItem objects in slots
    connected to this signal.
*/

/*!
    \fn void Q3ListView::clicked(Q3ListViewItem *item)

    This signal is emitted whenever the user clicks (mouse pressed \e
    and mouse released) in the list view. \a item is the clicked list
    view item, or 0 if the user didn't click on an item.

    \warning Do not delete any Q3ListViewItem objects in slots
    connected to this signal.
*/

/*!
    \fn void Q3ListView::mouseButtonClicked(int button, Q3ListViewItem * item, const QPoint & pos, int c)

    This signal is emitted whenever the user clicks (mouse pressed \e
    and mouse released) in the list view at position \a pos. \a button
    is the mouse button that the user pressed, \a item is the clicked
    list view item or 0 if the user didn't click on an item. If \a
    item is not 0, \a c is the list view column into which the user
    pressed; if \a item is 0 \a{c}'s value is undefined.

    \warning Do not delete any Q3ListViewItem objects in slots
    connected to this signal.
*/

/*!
    \fn void Q3ListView::mouseButtonPressed(int button, Q3ListViewItem * item, const QPoint & pos, int c)

    This signal is emitted whenever the user pressed the mouse button
    in the list view at position \a pos. \a button is the mouse button
    which the user pressed, \a item is the pressed list view item or 0
    if the user didn't press on an item. If \a item is not 0, \a c is
    the list view column into which the user pressed; if \a item is 0
    \a{c}'s value is undefined.

    \warning Do not delete any Q3ListViewItem objects in slots
    connected to this signal.
*/

/*!
    \fn void Q3ListView::clicked(Q3ListViewItem *item, const QPoint &pnt, int c)

    \overload

    This signal is emitted whenever the user clicks (mouse pressed \e
    and mouse released) in the list view. \a item is the clicked list
    view item, or 0 if the user didn't click on an item. \a pnt is the
    position where the user has clicked in global coordinates. If \a
    item is not 0, \a c is the list view column into which the user
    pressed; if \a item is 0 \a{c}'s value is undefined.

    \warning Do not delete any Q3ListViewItem objects in slots
    connected to this signal.
*/

/*!
    \fn void Q3ListView::selectionChanged(Q3ListViewItem *item)

    \overload

    This signal is emitted whenever the selected item has changed in
    \c Single selection mode (normally after the screen update). The
    argument is the newly selected \a item.

    In \c Multi selection mode, use the no argument overload of this
    signal.

    \warning Do not delete any Q3ListViewItem objects in slots
    connected to this signal.

    \sa setSelected() Q3ListViewItem::setSelected() currentChanged()
*/


/*!
    \fn void Q3ListView::currentChanged(Q3ListViewItem *item)

    This signal is emitted whenever the current item has changed
    (normally after the screen update). The current item is the item
    responsible for indicating keyboard focus.

    The argument is the newly current \a item, or 0 if the change made
    no item current. This can happen, for example, if all the items in
    the list view are deleted.

    \warning Do not delete any Q3ListViewItem objects in slots
    connected to this signal.

    \sa setCurrentItem() currentItem()
*/


/*!
    \fn void Q3ListView::expanded(Q3ListViewItem *item)

    This signal is emitted when \a item has been expanded, i.e. when
    the children of \a item are shown.

    \sa setOpen() collapsed()
*/

/*!
    \fn void Q3ListView::collapsed(Q3ListViewItem *item)

    This signal is emitted when the \a item has been collapsed, i.e.
    when the children of \a item are hidden.

    \sa setOpen() expanded()
*/

/*!
    Processes the mouse press event \a e on behalf of the viewed widget.
*/
void Q3ListView::contentsMousePressEvent(QMouseEvent * e)
{
    contentsMousePressEventEx(e);
}

void Q3ListView::contentsMousePressEventEx(QMouseEvent * e)
{
    if (!e)
        return;

    if (!d->ignoreEditAfterFocus)
        d->startEdit = true;
    d->ignoreEditAfterFocus = false;

    if (currentItem() && currentItem()->renameBox &&
         !itemRect(currentItem()).contains(e->pos())) {
        d->startEdit = false;
        if (d->defRenameAction == Reject)
            currentItem()->cancelRename(currentItem()->renameCol);
        else
            currentItem()->okRename(currentItem()->renameCol);
    }

    d->startDragItem = 0;
    d->dragStartPos = e->pos();
    QPoint vp = contentsToViewport(e->pos());

    d->ignoreDoubleClick = false;
    d->buttonDown = true;

    Q3ListViewItem * i = itemAt(vp);
    d->pressedEmptyArea = e->y() > contentsHeight();
    if (i && !i->isEnabled())
        return;
    if (d->startEdit && (i != currentItem() || (i && !i->isSelected())))
        d->startEdit = false;
    Q3ListViewItem *oldCurrent = currentItem();

    if (e->button() == Qt::RightButton && (e->state() & Qt::ControlButton))
        goto emit_signals;

    if (!i) {
        if (!(e->state() & Qt::ControlButton))
            clearSelection();
        goto emit_signals;
    } else {
        // No new anchor when using shift
        if (!(e->state() & Qt::ShiftButton))
            d->selectAnchor = i;
    }

    if ((i->isExpandable() || i->childCount()) &&
         d->h->mapToLogical(d->h->cellAt(vp.x())) == 0) {
        int x1 = vp.x() +
                 d->h->offset() -
                 d->h->cellPos(d->h->mapToActual(0));
        int draw = 0;
        for (; draw < d->drawables.size(); ++draw)
            if (d->drawables.at(draw).i == i)
                break;

        if (draw < d->drawables.size()) {
            Q3ListViewPrivate::DrawableItem it = d->drawables.at(draw);
            QStyleOptionQ3ListView opt = getStyleOption(this, i);
            x1 -= treeStepSize() * (it.l - 1);
            QStyle::SubControl ctrl = style()->hitTestComplexControl(QStyle::CC_Q3ListView, &opt,
                                                                     QPoint(x1, e->pos().y()), this);
            if (ctrl == QStyle::SC_Q3ListViewExpand &&
                e->type() == style()->styleHint(QStyle::SH_Q3ListViewExpand_SelectMouseType, 0,
                                               this)) {
                d->buttonDown = false;
                if (e->button() == Qt::LeftButton) {
                    bool close = i->isOpen();
                    setOpen(i, !close);
                    // ### Looks dangerous, removed because of reentrance problems
                    // qApp->processEvents();
                    if (!d->focusItem) {
                        d->focusItem = i;
                        repaintItem(d->focusItem);
                        emit currentChanged(d->focusItem);
                    }
                    if (close) {
                        bool newCurrent = false;
                        Q3ListViewItem *ci = d->focusItem;
                        while (ci) {
                            if (ci->parent() && ci->parent() == i) {
                                newCurrent = true;
                                break;
                            }
                            ci = ci->parent();
                        }
                        if (newCurrent) {
                            setCurrentItem(i);
                        }
                    }
                }
                d->ignoreDoubleClick = true;
                d->buttonDown = false;
                goto emit_signals;
            }
        }
    }

    d->select = d->selectionMode == Multi ? !i->isSelected() : true;

    {// calculate activatedP
        activatedByClick = true;
        QPoint topLeft = itemRect(i).topLeft(); //### inefficient?
        activatedP = vp - topLeft;
        int xdepth = treeStepSize() * (i->depth() + (rootIsDecorated() ? 1 : 0))
                     + itemMargin();
        xdepth += d->h->sectionPos(d->h->mapToSection(0));
        activatedP.rx() -= xdepth;
    }
    i->activate();
    activatedByClick = false;

    if (i != d->focusItem)
        setCurrentItem(i);
    else
        repaintItem(i);

    d->pressedSelected = i && i->isSelected();

    if (i->isSelectable() && selectionMode() != NoSelection) {
        if (selectionMode() == Single)
            setSelected(i, true);
        else if (selectionMode() == Multi)
            setSelected(i, d->select);
        else if (selectionMode() == Extended) {
            bool changed = false;
            if (!(e->state() & (Qt::ControlButton | Qt::ShiftButton))) {
                if (!i->isSelected()) {
                    bool blocked = signalsBlocked();
                    blockSignals(true);
                    clearSelection();
                    blockSignals(blocked);
                    i->setSelected(true);
                    changed = true;
                }
            } else {
                if (e->state() & Qt::ShiftButton)
                    d->pressedSelected = false;
                if ((e->state() & Qt::ControlButton) && !(e->state() & Qt::ShiftButton) && i) {
                    i->setSelected(!i->isSelected());
                    changed = true;
                    d->pressedSelected = false;
                } else if (!oldCurrent || !i || oldCurrent == i) {
                    if ((bool)i->selected != d->select) {
                        changed = true;
                        i->setSelected(d->select);
                    }
                // Shift pressed in Extended mode ---
                } else {
                    changed = selectRange(i, oldCurrent, d->selectAnchor);
                }
            }
            if (changed) {
                triggerUpdate();
                emit selectionChanged();

#ifndef QT_NO_ACCESSIBILITY
                QAccessible::updateAccessibility(viewport(), 0, QAccessible::Selection);
#endif
            }
        }
    }

 emit_signals:

    if (i && !d->buttonDown &&
         vp.x() + contentsX() < itemMargin() + (i->depth() + (rootIsDecorated() ? 1 : 0)) * treeStepSize())
        i = 0;
    d->pressedItem = i;

    int c = i ? d->h->mapToLogical(d->h->cellAt(vp.x())) : -1;
    if (!i || (i && i->isEnabled())) {
        emit pressed(i);
        emit pressed(i, viewport()->mapToGlobal(vp), c);
    }
    emit mouseButtonPressed(e->button(), i, viewport()->mapToGlobal(vp), c);

    if (e->button() == Qt::RightButton && i == d->pressedItem) {
        if (!i && !(e->state() & Qt::ControlButton))
            clearSelection();

        emit rightButtonPressed(i, viewport()->mapToGlobal(vp), c);
    }
}

/*!
    \reimp
*/

void Q3ListView::contentsContextMenuEvent(QContextMenuEvent *e)
{
    if (!receivers(SIGNAL(contextMenuRequested(Q3ListViewItem*,QPoint,int)))) {
        e->ignore();
        return;
    }
    if (e->reason() == QContextMenuEvent::Keyboard) {
        Q3ListViewItem *item = currentItem();
        if (item) {
            QRect r = itemRect(item);
            QPoint p = r.topLeft();
            if (allColumnsShowFocus())
                p += QPoint(width() / 2, (r.height() / 2));
            else
                p += QPoint(columnWidth(0) / 2, (r.height() / 2));
            p.rx() = qMax(0, p.x());
            p.rx() = qMin(visibleWidth(), p.x());
            emit contextMenuRequested(item, viewport()->mapToGlobal(p), -1);
        }
    } else {
        QPoint vp = contentsToViewport(e->pos());
        Q3ListViewItem * i = itemAt(vp);
        int c = i ? d->h->mapToLogical(d->h->cellAt(vp.x())) : -1;
        emit contextMenuRequested(i, viewport()->mapToGlobal(vp), c);
    }
}

/*!
    Processes the mouse release event \a e on behalf of the viewed widget.
*/
void Q3ListView::contentsMouseReleaseEvent(QMouseEvent * e)
{
    contentsMouseReleaseEventEx(e);
}

void Q3ListView::contentsMouseReleaseEventEx(QMouseEvent * e)
{
    d->startDragItem = 0;
    bool emitClicked = !d->pressedItem || d->buttonDown;
    d->buttonDown = false;
    // delete and disconnect autoscroll timer, if we have one
    if (d->scrollTimer) {
        disconnect(d->scrollTimer, SIGNAL(timeout()),
                    this, SLOT(doAutoScroll()));
        d->scrollTimer->stop();
        delete d->scrollTimer;
        d->scrollTimer = 0;
    }

    if (!e)
        return;

    if (d->selectionMode == Extended &&
         d->focusItem == d->pressedItem &&
         d->pressedSelected && d->focusItem &&
         e->button() == Qt::LeftButton) {
        bool block = signalsBlocked();
        blockSignals(true);
        clearSelection();
        blockSignals(block);
        d->focusItem->setSelected(true);
        emit selectionChanged();
#ifndef QT_NO_ACCESSIBILITY
        QAccessible::updateAccessibility(viewport(), 0, QAccessible::Selection);
#endif
    }

    QPoint vp = contentsToViewport(e->pos());
    Q3ListViewItem *i = itemAt(vp);
    if (i && !i->isEnabled())
        return;

    if (i && i == d->pressedItem && (i->isExpandable() || i->childCount()) &&
         !d->h->mapToLogical(d->h->cellAt(vp.x())) && e->button() == Qt::LeftButton &&
         e->type() == style()->styleHint(QStyle::SH_Q3ListViewExpand_SelectMouseType, 0, this)) {
        int draw = 0;
        for (; draw < d->drawables.size(); ++draw)
            if (d->drawables.at(draw).i == i)
                break;
        if (draw < d->drawables.size()) {
            int x1 = vp.x() + d->h->offset() - d->h->cellPos(d->h->mapToActual(0)) -
                     (treeStepSize() * (d->drawables.at(draw).l - 1));
            QStyleOptionQ3ListView opt = getStyleOption(this, i);
            QStyle::SubControl ctrl = style()->hitTestComplexControl(QStyle::CC_Q3ListView, &opt,
                                                                     QPoint(x1, e->pos().y()), this);
            if (ctrl == QStyle::SC_Q3ListViewExpand) {
                bool close = i->isOpen();
                setOpen(i, !close);
                // ### Looks dangerous, removed because of reentrance problems
                // qApp->processEvents();
                if (!d->focusItem) {
                    d->focusItem = i;
                    repaintItem(d->focusItem);
                    emit currentChanged(d->focusItem);
                }
                if (close) {
                    bool newCurrent = false;
                    Q3ListViewItem *ci = d->focusItem;
                    while (ci) {
                        if (ci->parent() && ci->parent() == i) {
                            newCurrent = true;
                            break;
                        }
                        ci = ci->parent();
                    }
                    if (newCurrent)
                        setCurrentItem(i);
                    d->ignoreDoubleClick = true;
                }
            }
        }
    }

    if (i == d->pressedItem && i && i->isSelected() && e->button() == Qt::LeftButton && d->startEdit) {
        QRect r = itemRect(currentItem());
        r = QRect(viewportToContents(r.topLeft()), r.size());
        d->pressedColumn = header()->sectionAt( e->pos().x());
        r.setLeft(header()->sectionPos(d->pressedColumn));
        r.setWidth(header()->sectionSize(d->pressedColumn) - 1);
        if (d->pressedColumn == 0)
            r.setLeft(r.left() + itemMargin() + (currentItem()->depth() +
                                                   (rootIsDecorated() ? 1 : 0)) * treeStepSize() - 1);
        if (r.contains(e->pos()) &&
             !(e->state() & (Qt::ShiftButton | Qt::ControlButton)))
            d->renameTimer->start(QApplication::doubleClickInterval(), true);
    }
    if (i && vp.x() + contentsX() < itemMargin() + (i->depth() + (rootIsDecorated() ? 1 : 0)) * treeStepSize())
        i = 0;
    emitClicked = emitClicked && d->pressedItem == i;
    d->pressedItem = 0;

    if (emitClicked) {
        if (!i || (i && i->isEnabled())) {
            emit clicked(i);
            emit clicked(i, viewport()->mapToGlobal(vp), d->h->mapToLogical(d->h->cellAt(vp.x())));
        }
        emit mouseButtonClicked(e->button(), i, viewport()->mapToGlobal(vp),
                                 i ? d->h->mapToLogical(d->h->cellAt(vp.x())) : -1);

        if (e->button() == Qt::RightButton) {
            if (!i) {
                if (!(e->state() & Qt::ControlButton))
                    clearSelection();
                emit rightButtonClicked(0, viewport()->mapToGlobal(vp), -1);
                return;
            }

            int c = d->h->mapToLogical(d->h->cellAt(vp.x()));
            emit rightButtonClicked(i, viewport()->mapToGlobal(vp), c);
        }
    }
}


/*!
    Processes the mouse double-click event \a e on behalf of the viewed widget.
*/
void Q3ListView::contentsMouseDoubleClickEvent(QMouseEvent * e)
{
    d->renameTimer->stop();
    d->startEdit = false;
    if (!e || e->button() != Qt::LeftButton)
        return;

    // ensure that the following mouse moves and eventual release is
    // ignored.
    d->buttonDown = false;

    if (d->ignoreDoubleClick) {
        d->ignoreDoubleClick = false;
        return;
    }

    QPoint vp = contentsToViewport(e->pos());

    Q3ListViewItem * i = itemAt(vp);

    // we emit doubleClicked when the item is null (or enabled) to be consistent with
    // rightButtonClicked etc.
    if (!i || i->isEnabled()) {
        int c = d->h->mapToLogical(d->h->cellAt(vp.x()));
        emit doubleClicked(i, viewport()->mapToGlobal(vp), c);
    }

    if (!i || !i->isEnabled())
        return;

    if (!i->isOpen()) {
        if (i->isExpandable() || i->childCount())
            setOpen(i, true);
    } else {
        setOpen(i, false);
    }

    // we emit the 'old' obsolete doubleClicked only if the item is not null and enabled
    emit doubleClicked(i);
}


/*!
    Processes the mouse move event \a e on behalf of the viewed widget.
*/
void Q3ListView::contentsMouseMoveEvent(QMouseEvent * e)
{
    if (!e)
        return;

    bool needAutoScroll = false;

    QPoint vp = contentsToViewport(e->pos());

    Q3ListViewItem * i = itemAt(vp);
    if (i && !i->isEnabled())
        return;
    if (i != d->highlighted &&
         !(d->pressedItem &&
           (d->pressedItem->isSelected() || d->selectionMode == NoSelection) &&
           d->pressedItem->dragEnabled())) {

        if (i) {
            emit onItem(i);
        } else {
            emit onViewport();
        }
        d->highlighted = i;
    }

    if (d->startDragItem)
        i = d->startDragItem;

    if (!d->buttonDown ||
         ((e->state() & Qt::LeftButton) != Qt::LeftButton &&
           (e->state() & Qt::MidButton) != Qt::MidButton &&
           (e->state() & Qt::RightButton) != Qt::RightButton))
        return;

    if (d->pressedItem &&
         (d->pressedItem->isSelected() || d->selectionMode == NoSelection) &&
         d->pressedItem->dragEnabled()) {

        if (!d->startDragItem) {
            setSelected(d->pressedItem, true);
            d->startDragItem = d->pressedItem;
        }
        if ((d->dragStartPos - e->pos()).manhattanLength() > QApplication::startDragDistance()) {
            d->buttonDown = false;
#ifndef QT_NO_DRAGANDDROP
            startDrag();
#endif
        }
        return;
    }

    // check, if we need to scroll
    if (vp.y() > visibleHeight() || vp.y() < 0)
        needAutoScroll = true;

    // if we need to scroll and no autoscroll timer is started,
    // connect the timer
    if (needAutoScroll && !d->scrollTimer) {
        d->scrollTimer = new QTimer(this);
        connect(d->scrollTimer, SIGNAL(timeout()),
                 this, SLOT(doAutoScroll()));
        d->scrollTimer->start(100, false);
        // call it once manually
        doAutoScroll(vp);
    }

    // if we don't need to autoscroll
    if (!needAutoScroll) {
        // if there is a autoscroll timer, delete it
        if (d->scrollTimer) {
            disconnect(d->scrollTimer, SIGNAL(timeout()),
                        this, SLOT(doAutoScroll()));
            d->scrollTimer->stop();
            delete d->scrollTimer;
            d->scrollTimer = 0;
        }
        // call this to select an item (using the pos from the event)
        doAutoScroll(vp);
    }
}


/*!
    This slot handles auto-scrolling when the mouse button is pressed
    and the mouse is outside the widget.
*/
void Q3ListView::doAutoScroll()
{
    doAutoScroll(QPoint());
}

/*
  Handles auto-scrolling when the mouse button is pressed
  and the mouse is outside the widget.

  If cursorPos is (0,0) (isNull == true) it uses the current QCursor::pos, otherwise it uses cursorPos
*/
void Q3ListView::doAutoScroll(const QPoint &cursorPos)
{
    QPoint pos = cursorPos.isNull() ? viewport()->mapFromGlobal(QCursor::pos()) :  cursorPos;
    if (!d->focusItem || (d->pressedEmptyArea && pos.y() > contentsHeight()))
        return;

    bool down = pos.y() > itemRect(d->focusItem).y();

    int g = pos.y() + contentsY();

    if (down && pos.y() > height() )
        g = height() + contentsY();
    else if (pos.y() < 0)
        g = contentsY();

    Q3ListViewItem *c = d->focusItem, *old = 0;
    Q3ListViewItem *oldCurrent = c;
    if (down) {
        int y = itemRect(d->focusItem).y() + contentsY();
        while(c && y + c->height() <= g) {
            y += c->height();
            old = c;
            c = c->itemBelow();
        }
        if (!c && old)
            c = old;
    } else {
        int y = itemRect(d->focusItem).y() + contentsY();
        while(c && y >= g) {
            old = c;
            c = c->itemAbove();
            if (c)
                y -= c->height();
        }
        if (!c && old)
            c = old;
    }

    if (!c || c == d->focusItem)
        return;

    if (d->focusItem) {
        if (d->selectionMode == Multi) {
            // also (de)select the ones in between
            Q3ListViewItem * b = d->focusItem;
            bool down = (itemPos(c) > itemPos(b));
            while(b && b != c) {
                if (b->isSelectable())
                    setSelected(b, d->select);
                b = down ? b->itemBelow() : b->itemAbove();
            }
            if (c->isSelectable())
                setSelected(c, d->select);
        } else if (d->selectionMode == Extended) {
            if (selectRange(c, oldCurrent, d->selectAnchor)) {
                triggerUpdate();
                emit selectionChanged();
            }
        }
    }

    setCurrentItem(c);
    d->visibleTimer->start(1, true);
}

/*!
    \reimp
*/

void Q3ListView::focusInEvent(QFocusEvent *e)
{
    d->inMenuMode = false;
    if (d->focusItem) {
        repaintItem(d->focusItem);
    } else if (firstChild() && e->reason() != Qt::MouseFocusReason) {
        d->focusItem = firstChild();
        emit currentChanged(d->focusItem);
        repaintItem(d->focusItem);
    }
    if (e->reason() == Qt::MouseFocusReason) {
        d->ignoreEditAfterFocus = true;
        d->startEdit = false;
    }
    if (style()->styleHint(QStyle::SH_ItemView_ChangeHighlightOnFocus, 0, this)) {
        viewport()->repaint();
    }
}

/*!
    \reimp
*/
QVariant Q3ListView::inputMethodQuery(Qt::InputMethodQuery query) const
{
    if (query == Qt::ImMicroFocus) {
        QRect mfrect = itemRect(d->focusItem);
        if (mfrect.isValid() && header() && header()->isVisible())
            mfrect.moveBy(0, header()->height());
        return mfrect;
    }
    return QWidget::inputMethodQuery(query);
}

/*!
    \reimp
*/

void Q3ListView::focusOutEvent(QFocusEvent *e)
{
    if (e->reason() == Qt::PopupFocusReason && d->buttonDown)
        d->buttonDown = false;
    if (style()->styleHint(QStyle::SH_ItemView_ChangeHighlightOnFocus, 0, this)) {
        d->inMenuMode =
            e->reason() == Qt::PopupFocusReason
            || (qApp->focusWidget() && qApp->focusWidget()->inherits("QMenuBar"));
        if (!d->inMenuMode) {
            viewport()->repaint();
        }
    }

    if (d->focusItem)
        repaintItem(d->focusItem);
}


/*!
    \reimp
*/

void Q3ListView::keyPressEvent(QKeyEvent * e)
{
    if (currentItem() && currentItem()->renameBox)
        return;
    if (!firstChild()) {
        e->ignore();
        return; // subclass bug
    }

    Q3ListViewItem* oldCurrent = currentItem();
    if (!oldCurrent) {
        setCurrentItem(firstChild());
        if (d->selectionMode == Single)
            setSelected(firstChild(), true);
        return;
    }

    Q3ListViewItem * i = currentItem();
    Q3ListViewItem *old = i;

    QRect r(itemRect(i));
    Q3ListViewItem * i2;

    bool singleStep = false;
    bool selectCurrent = true;
    bool wasNavigation = true;

    switch(e->key()) {
    case Qt::Key_Backspace:
    case Qt::Key_Delete:
        d->currentPrefix.truncate(0);
        break;
    case Qt::Key_Enter:
    case Qt::Key_Return:
        d->currentPrefix.truncate(0);
        if (i && !i->isSelectable() && i->isEnabled() &&
             (i->childCount() || i->isExpandable() || i->isOpen())) {
            i->setOpen(!i->isOpen());
            return;
        }
        e->ignore();
        if (currentItem() && !currentItem()->isEnabled())
            break;
        emit returnPressed(currentItem());
        // do NOT accept.  QDialog.
        return;
    case Qt::Key_Down:
        selectCurrent = false;
        i = i->itemBelow();
        d->currentPrefix.truncate(0);
        singleStep = true;
        break;
    case Qt::Key_Up:
        selectCurrent = false;
        i = i->itemAbove();
        d->currentPrefix.truncate(0);
        singleStep = true;
        break;
    case Qt::Key_Home:
        selectCurrent = false;
        i = firstChild();
        if (!i->height() || !i->isEnabled())
            i = i->itemBelow();
        d->currentPrefix.truncate(0);
        break;
    case Qt::Key_End:
        selectCurrent = false;
        i = firstChild();
        while (i->nextSibling() && i->nextSibling()->height() && i->nextSibling()->isEnabled())
            i = i->nextSibling();
        while (i->itemBelow())
            i = i->itemBelow();
        d->currentPrefix.truncate(0);
        break;
    case Qt::Key_Next:
        selectCurrent = false;
        i2 = itemAt(QPoint(0, visibleHeight()-1));
        if (i2 == i || !r.isValid() ||
             visibleHeight() <= itemRect(i).bottom()) {
            if (i2)
                i = i2;
            int left = visibleHeight();
            while((i2 = i->itemBelow()) != 0 && left > i2->height()) {
                left -= i2->height();
                i = i2;
            }
        } else {
            if (!i2) {
                // list is shorter than the view, goto last item
                while((i2 = i->itemBelow()) != 0)
                    i = i2;
            } else {
                i = i2;
            }
        }
        d->currentPrefix.truncate(0);
        break;
    case Qt::Key_Prior:
        selectCurrent = false;
        i2 = itemAt(QPoint(0, 0));
        if (i == i2 || !r.isValid() || r.top() <= 0) {
            if (i2)
                i = i2;
            int left = visibleHeight();
            while((i2 = i->itemAbove()) != 0 && left > i2->height()) {
                left -= i2->height();
                i = i2;
            }
        } else {
            i = i2;
        }
        d->currentPrefix.truncate(0);
        break;
    case Qt::Key_Plus:
        d->currentPrefix.truncate(0);
        if ( !i->isOpen() && (i->isExpandable() || i->childCount()))
            setOpen(i, true);
        else
            return;
        break;
    case Qt::Key_Right:
        d->currentPrefix.truncate(0);
        if (i->isOpen() && i->childItem) {
            i = i->childItem;
        } else if (!i->isOpen() && (i->isExpandable() || i->childCount())) {
            setOpen(i, true);
        } else if (contentsX() + visibleWidth() < contentsWidth()) {
            horizontalScrollBar()->triggerAction(QScrollBar::SliderSingleStepAdd);
            return;
        } else {
            return;
        }
        break;
    case Qt::Key_Minus:
        d->currentPrefix.truncate(0);
        if (i->isOpen())
            setOpen(i, false);
        else
            return;
        break;
    case Qt::Key_Left:
        d->currentPrefix.truncate(0);
        if (i->isOpen()) {
            setOpen(i, false);
        } else if (i->parentItem && i->parentItem != d->r) {
            i = i->parentItem;
        } else if (contentsX()) {
            horizontalScrollBar()->triggerAction(QScrollBar::SliderSingleStepSub);
            return;
        } else {
            return;
        }
        break;
    case Qt::Key_Space:
        activatedByClick = false;
        d->currentPrefix.truncate(0);
        if (currentItem() && !currentItem()->isEnabled())
            break;
        i->activate();
        if (i->isSelectable() && (d->selectionMode == Multi || d->selectionMode == Extended)) {
            setSelected(i, !i->isSelected());
            d->currentPrefix.truncate(0);
        }
        emit spacePressed(currentItem());
        break;
    case Qt::Key_Escape:
        e->ignore(); // For QDialog
        return;
    case Qt::Key_F2:
        if (currentItem() && currentItem()->renameEnabled(0))
            currentItem()->startRename(0);
    default:
        if (e->text().length() > 0 && e->text()[0].isPrint()) {
            selectCurrent = false;
            wasNavigation = false;
            QString input(d->currentPrefix);
            Q3ListViewItem * keyItem = i;
            QTime now(QTime::currentTime());
            bool tryFirst = true;
            while(keyItem) {
                // try twice, first with the previous string and this char
                if (d->currentPrefixTime.msecsTo(now) <= 400)
                    input = input + e->text().toLower();
                else
                    input = e->text().toLower();
                if (input.length() == e->text().length()) {
                    if (keyItem->itemBelow()) {
                        keyItem = keyItem->itemBelow();
                        tryFirst = true;
                    } else {
                        keyItem = firstChild();
                        tryFirst = false;
                    }
                }
                QString keyItemKey;
                QString prefix;
                while(keyItem) {
                    keyItemKey = QString::null;
                    // Look first in the sort column, then left to right
		    if (d->sortcolumn != Unsorted)
			keyItemKey = keyItem->text(d->sortcolumn);
                    for (int col = 0; col < d->h->count() && keyItemKey.isNull(); ++col)
                        keyItemKey = keyItem->text(d->h->mapToSection(col));
                    if (!keyItemKey.isEmpty()) {
                        prefix = keyItemKey;
                        prefix.truncate(input.length());
                        prefix = prefix.toLower();
                        if (prefix == input) {
                            d->currentPrefix = input;
                            d->currentPrefixTime = now;
                            i = keyItem;
                                // nonoptimal double-break...
                            keyItem = 0;
                            input.truncate(0);
                            tryFirst = false;
                        }
                    }
                    if (keyItem)
                        keyItem = keyItem->itemBelow();
                    if (!keyItem && tryFirst) {
                        keyItem = firstChild();
                        tryFirst = false;
                    }
                }
                // then, if appropriate, with just this character
                if (input.length() > e->text().length()) {
                    input.truncate(0);
                    keyItem = i;
                }
            }
        } else {
            d->currentPrefix.truncate(0);
            if (e->state() & Qt::ControlButton) {
                d->currentPrefix.clear();
                switch (e->key()) {
                case Qt::Key_A:
                    selectAll(true);
                    break;
                }
            }
            e->ignore();
            return;
        }
    }

    if (!i)
        return;

    if (!(e->state() & Qt::ShiftButton) || !d->selectAnchor)
        d->selectAnchor = i;

    setCurrentItem(i);
    if (i->isSelectable())
        handleItemChange(old, wasNavigation && (e->state() & Qt::ShiftButton),
                          wasNavigation && (e->state() & Qt::ControlButton));

    if (d->focusItem && !d->focusItem->isSelected() && d->selectionMode == Single && selectCurrent)
        setSelected(d->focusItem, true);

    if (singleStep)
        d->visibleTimer->start(1, true);
    else
        ensureItemVisible(i);
}


/*!
    Returns the list view item at \a viewPos. Note that \a viewPos is
    in the viewport()'s coordinate system, not in the list view's own,
    much larger, coordinate system.

    itemAt() returns 0 if there is no such item.

    Note that you also get the pointer to the item if \a viewPos
    points to the root decoration (see setRootIsDecorated()) of the
    item. To check whether or not \a viewPos is on the root decoration
    of the item, you can do something like this:

    \snippet doc/src/snippets/code/src_qt3support_itemviews_q3listview.cpp 4

    This might be interesting if you use this function to find out
    where the user clicked and if you want to start a drag (which you
    do not want to do if the user clicked onto the root decoration of
    an item).

    \sa itemPos() itemRect() viewportToContents()
*/

Q3ListViewItem * Q3ListView::itemAt(const QPoint & viewPos) const
{
    if (viewPos.x() > contentsWidth() - contentsX())
        return 0;

    if (d->drawables.isEmpty())
        buildDrawableList();

    int g = viewPos.y() + contentsY();

    for (int i = 0; i < d->drawables.size(); ++i) {
        Q3ListViewPrivate::DrawableItem c = d->drawables.at(i);
        if (c.y + c.i->height() > g
            && c.i->isVisible() && (!c.i->parent() || c.i->parent()->isVisible()))
            return c.y <= g ? c.i : 0;
    }
    return 0;
}


/*!
    Returns the y-coordinate of \a item in the list view's coordinate
    system. This function is normally much slower than itemAt() but it
    works for all items, whereas itemAt() normally works only for
    items on the screen.

    This is a thin wrapper around Q3ListViewItem::itemPos().

    \sa itemAt() itemRect()
*/

int Q3ListView::itemPos(const Q3ListViewItem * item)
{
    return item ? item->itemPos() : 0;
}


/*!
    \property Q3ListView::multiSelection
    \brief whether the list view is in multi-selection or extended-selection mode

    If you enable multi-selection, \c Multi, mode, it is possible to
    specify whether or not this mode should be extended. \c Extended
    means that the user can select multiple items only when pressing
    the Shift or Ctrl key at the same time.

    The default selection mode is \c Single.

    \sa selectionMode()
*/

void Q3ListView::setMultiSelection(bool enable)
{
    if (!enable)
        d->selectionMode = Q3ListView::Single;
    else if ( d->selectionMode != Multi && d->selectionMode != Extended)
        d->selectionMode = Q3ListView::Multi;
}

bool Q3ListView::isMultiSelection() const
{
    return d->selectionMode == Q3ListView::Extended || d->selectionMode == Q3ListView::Multi;
}

/*!
    \property Q3ListView::selectionMode
    \brief the list view's selection mode

    The mode can be \c Single (the default), \c Extended, \c Multi or
    \c NoSelection.

    \sa multiSelection
*/

void Q3ListView::setSelectionMode(SelectionMode mode)
{
    if (d->selectionMode == mode)
        return;

    if ((d->selectionMode == Multi || d->selectionMode == Extended) &&
         (mode == Q3ListView::Single || mode == Q3ListView::NoSelection)){
        clearSelection();
        if ((mode == Q3ListView::Single) && currentItem())
            currentItem()->selected = true;
    }

    d->selectionMode = mode;
}

Q3ListView::SelectionMode Q3ListView::selectionMode() const
{
    return d->selectionMode;
}


/*!
    If \a selected is true the \a item is selected; otherwise it is
    unselected.

    If the list view is in \c Single selection mode and \a selected is
    true, the currently selected item is unselected and \a item is
    made current. Unlike Q3ListViewItem::setSelected(), this function
    updates the list view as necessary and emits the
    selectionChanged() signals.

    \sa isSelected() setMultiSelection() isMultiSelection()
    setCurrentItem(), setSelectionAnchor()
*/

void Q3ListView::setSelected(Q3ListViewItem * item, bool selected)
{
    if (!item || item->isSelected() == selected ||
         !item->isSelectable() || selectionMode() == NoSelection)
        return;

    bool emitHighlighted = false;
    if (selectionMode() == Single && d->focusItem != item) {
        Q3ListViewItem *o = d->focusItem;
        if (d->focusItem && d->focusItem->selected)
            d->focusItem->setSelected(false);
        d->focusItem = item;
        if (o)
            repaintItem(o);
        emitHighlighted = true;
    }

    item->setSelected(selected);

    repaintItem(item);

    if (d->selectionMode == Single && selected)
        emit selectionChanged(item);
    emit selectionChanged();

    if (emitHighlighted)
        emit currentChanged(d->focusItem);
}

/*!
    Sets the selection anchor to \a item, if \a item is selectable.

    The selection anchor is the item that remains selected when
    Shift-selecting with either mouse or keyboard in \c Extended
    selection mode.

    \sa setSelected()
*/

void Q3ListView::setSelectionAnchor(Q3ListViewItem *item)
{
    if (item && item->isSelectable())
        d->selectAnchor = item;
}

/*!
    Sets all the items to be not selected, updates the list view as
    necessary, and emits the selectionChanged() signals. Note that for
    \c Multi selection list views this function needs to iterate over
    \e all items.

    \sa setSelected(), setMultiSelection()
*/

void Q3ListView::clearSelection()
{
    selectAll(false);
}

/*!
    If \a select is true, all the items get selected; otherwise all
    the items get unselected. This only works in the selection modes \c
    Multi and \c Extended. In \c Single and \c NoSelection mode the
    selection of the current item is just set to \a select.
*/

void Q3ListView::selectAll(bool select)
{
    if (d->selectionMode == Multi || d->selectionMode == Extended) {
        bool b = signalsBlocked();
        blockSignals(true);
        bool anything = false;
        Q3ListViewItemIterator it(this);
        while (it.current()) {
            Q3ListViewItem *i = it.current();
            if ((bool)i->selected != select) {
                i->setSelected(select);
                anything = true;
            }
            ++it;
        }
        blockSignals(b);
        if (anything) {
            emit selectionChanged();
            triggerUpdate();
        }
    } else if (d->focusItem) {
        Q3ListViewItem * i = d->focusItem;
        setSelected(i, select);
    }
}

/*!
    Inverts the selection. Only works in \c Multi and \c Extended
    selection modes.
*/

void Q3ListView::invertSelection()
{
    if (d->selectionMode == Single ||
         d->selectionMode == NoSelection)
        return;

    bool b = signalsBlocked();
    blockSignals(true);
    Q3ListViewItemIterator it(this);
    for (; it.current(); ++it)
        it.current()->setSelected(!it.current()->isSelected());
    blockSignals(b);
    emit selectionChanged();
    triggerUpdate();
}


/*!
    Returns true if the list view item \a i is selected; otherwise
    returns false.

    \sa Q3ListViewItem::isSelected()
*/

bool Q3ListView::isSelected(const Q3ListViewItem * i) const
{
    return i ? i->isSelected() : false;
}


/*!
    Returns the selected item if the list view is in \c Single
    selection mode and an item is selected.

    If no items are selected or the list view is not in \c Single
    selection mode this function returns 0.

    \sa setSelected() setMultiSelection()
*/

Q3ListViewItem * Q3ListView::selectedItem() const
{
    if (d->selectionMode != Single)
        return 0;
    if (d->focusItem && d->focusItem->isSelected())
        return d->focusItem;
    return 0;
}


/*!
    Sets item \a i to be the current item and repaints appropriately
    (i.e. highlights the item). The current item is used for keyboard
    navigation and focus indication; it is independent of any selected
    items, although a selected item can also be the current item.

    \sa currentItem() setSelected()
*/

void Q3ListView::setCurrentItem(Q3ListViewItem * i)
{
    if (!i || d->focusItem == i || !i->isEnabled())
        return;

    if (currentItem() && currentItem()->renameBox) {
        if (d->defRenameAction == Reject)
            currentItem()->cancelRename(currentItem()->renameCol);
        else
            currentItem()->okRename(currentItem()->renameCol);
    }

    Q3ListViewItem * prev = d->focusItem;
    d->focusItem = i;

    if (i != prev) {
        if (i && d->selectionMode == Single) {
            bool changed = false;
            if (prev && prev->selected) {
                changed = true;
                prev->setSelected(false);
            }
            if (i && !i->selected && d->selectionMode != NoSelection && i->isSelectable()) {
                i->setSelected(true);
                changed = true;
                emit selectionChanged(i);
            }
            if (changed)
                emit selectionChanged();
        }

        if (i)
            repaintItem(i);
        if (prev)
            repaintItem(prev);
        emit currentChanged(i);

#ifndef QT_NO_ACCESSIBILITY
        QAccessible::updateAccessibility(viewport(), indexOfItem(i), QAccessible::Focus);
#endif
    }
}


/*!
    Returns the current item, or 0 if there isn't one.

    \sa setCurrentItem()
*/

Q3ListViewItem * Q3ListView::currentItem() const
{
    return d->focusItem;
}


/*!
    Returns the rectangle on the screen that item \a item occupies in
    viewport()'s coordinates, or an invalid rectangle if \a item is 0 or
    is not currently visible.

    The rectangle returned does not include any children of the
    rectangle (i.e. it uses Q3ListViewItem::height(), rather than
    Q3ListViewItem::totalHeight()). If you want the rectangle to
    include children you can use something like this:

    \snippet doc/src/snippets/code/src_qt3support_itemviews_q3listview.cpp 5

    Note the way it avoids too-high rectangles. totalHeight() can be
    much larger than the window system's coordinate system allows.

    itemRect() is comparatively slow. It's best to call it only for
    items that are probably on-screen.
*/

QRect Q3ListView::itemRect(const Q3ListViewItem * item) const
{
    if (d->drawables.isEmpty())
        buildDrawableList();

    for (int i = 0; i < d->drawables.size(); ++i) {
        const Q3ListViewPrivate::DrawableItem &c = d->drawables.at(i);
        if (c.i == item) {
            int y = c.y - contentsY();
            if (y + c.i->height() >= 0 && y < ((Q3ListView *)this)->visibleHeight()) {
                return QRect(-contentsX(), y, d->h->width(), c.i->height());;
            }
        }
    }

    return QRect(0, 0, -1, -1);
}


/*!
    \fn void Q3ListView::doubleClicked(Q3ListViewItem *item)

    This signal is emitted whenever an item is double-clicked. It's
    emitted on the second button press, not the second button release.
    \a item is the list view item on which the user did the
    double-click.
*/

/*!
    \fn void Q3ListView::doubleClicked(Q3ListViewItem *item, const
    QPoint& point, int column)

    This signal is emitted when a double-click occurs. It's emitted on
    the second button press, not the second button release. The \a
    item is the Q3ListViewItem the button was double-clicked on (which
    could be 0 if it wasn't double-clicked on an item). The \a point
    where the double-click occurred is given in global coordinates. If
    an item was double-clicked on, \a column is the column within the
    item that was double-clicked; otherwise \a column is -1.

    \warning Do not delete any Q3ListViewItem objects in slots
    connected to this signal.
*/


/*!
    \fn void Q3ListView::returnPressed(Q3ListViewItem *item)

    This signal is emitted when Enter or Return is pressed. The
    \a item parameter is the currentItem().
*/

/*!
    \fn void Q3ListView::spacePressed(Q3ListViewItem *item)

    This signal is emitted when Space is pressed. The \a item
    parameter is the currentItem().
*/


/*!
    Sets the list view to be sorted by column \a column in ascending
    order if \a ascending is true or descending order if it is false.

    If \a column is -1, sorting is disabled and the user cannot sort
    columns by clicking on the column headers. If \a column is larger
    than the number of columns the user must click on a column
    header to sort the list view.
*/

void Q3ListView::setSorting(int column, bool ascending)
{
    if (column == -1)
        column = Unsorted;

    if (d->sortcolumn == column && d->ascending == ascending)
        return;

    d->ascending = ascending;
    d->sortcolumn = column;
    if (d->sortcolumn != Unsorted && d->sortIndicator)
        d->h->setSortIndicator(d->sortcolumn, d->ascending);
    else
        d->h->setSortIndicator(-1);

    triggerUpdate();

#ifndef QT_NO_ACCESSIBILITY
    QAccessible::updateAccessibility(viewport(), 0, QAccessible::ObjectReorder);
#endif
}

/*!
    Sets the \a column the list view is sorted by.

    Sorting is triggered by choosing a header section.
*/

void Q3ListView::changeSortColumn(int column)
{
    if (isRenaming()) {
        if (d->defRenameAction == Q3ListView::Reject) {
            currentItem()->cancelRename(currentItem()->renameCol);
        } else {
            currentItem()->okRename(currentItem()->renameCol);
        }
    }
    if (d->sortcolumn != Unsorted) {
        int lcol = d->h->mapToLogical(column);
        setSorting(lcol, d->sortcolumn == lcol ? !d->ascending : true);
    }
}

/*!
  \internal
  Handles renaming when sections are being swapped by the user.
*/

void Q3ListView::handleIndexChange()
{
    if (isRenaming()) {
        if (d->defRenameAction == Q3ListView::Reject) {
            currentItem()->cancelRename(currentItem()->renameCol);
        } else {
            currentItem()->okRename(currentItem()->renameCol);
        }
    }
    triggerUpdate();
}

/*!
    Returns the column by which the list view is sorted, or -1 if
    sorting is disabled.

    \sa sortOrder()
*/

int Q3ListView::sortColumn() const
{
    return d->sortcolumn;
}

/*!
    Sets the sorting column for the list view.

    If \a column is -1, sorting is disabled and the user cannot sort
    columns by clicking on the column headers. If \a column is larger
    than the number of columns the user must click on a column header
    to sort the list view.

    \sa setSorting()
*/
void Q3ListView::setSortColumn(int column)
{
    setSorting(column, d->ascending);
}

/*!
    Returns the sorting order of the list view items.

    \sa sortColumn()
*/
Qt::SortOrder Q3ListView::sortOrder() const
{
    if (d->ascending)
        return Qt::AscendingOrder;
    return Qt::DescendingOrder;
}

/*!
    Sets the sort order for the items in the list view to \a order.

    \sa setSorting()
*/
void Q3ListView::setSortOrder(Qt::SortOrder order)
{
    setSorting(d->sortcolumn, order == Qt::AscendingOrder ? true : false);
}

/*!
    Sorts the list view using the last sorting configuration (sort
    column and ascending/descending).
*/

void Q3ListView::sort()
{
    if (d->r)
        d->r->sort();
}

/*!
    \property Q3ListView::itemMargin
    \brief the advisory item margin that list items may use

    The item margin defaults to one pixel and is the margin between
    the item's edges and the area where it draws its contents.
    Q3ListViewItem::paintFocus() draws in the margin.

    \sa Q3ListViewItem::paintCell()
*/

void Q3ListView::setItemMargin(int m)
{
    if (d->margin == m)
        return;
    d->margin = m;
    if (isVisible()) {
        d->drawables.clear();
        triggerUpdate();
    }
}

int Q3ListView::itemMargin() const
{
    return d->margin;
}


/*!
    \fn void Q3ListView::rightButtonClicked(Q3ListViewItem *item,
    const QPoint& point, int column)

    This signal is emitted when the right button is clicked. The \a
    item is the Q3ListViewItem the button was clicked on (which could
    be 0 if it wasn't clicked on an item). The \a point where the
    click occurred is given in global coordinates. If an item was
    clicked on, \a column is the column within the item that was
    clicked; otherwise \a column is -1.
*/


/*!
    \fn void Q3ListView::rightButtonPressed (Q3ListViewItem *item,
    const QPoint &point, int column)

    This signal is emitted when the right button is pressed. The \a
    item is the Q3ListViewItem the button was pressed on (which could
    be 0 if it wasn't pressed on an item). The \a point where the
    press occurred is given in global coordinates. If an item was
    pressed on, \a column is the column within the item that was
    pressed; otherwise \a column is -1.
*/

/*!
    \fn void Q3ListView::contextMenuRequested(Q3ListViewItem *item, const QPoint & pos, int col)

    This signal is emitted when the user invokes a context menu with
    the right mouse button or with special system keys. If the
    keyboard was used \a item is the current item; if the mouse was
    used, \a item is the item under the mouse pointer or 0 if there is
    no item under the mouse pointer. If no item is clicked, the column
    index emitted is -1.

    \a pos is the position for the context menu in the global
    coordinate system.

    \a col is the column on which the user pressed, or -1 if the
    signal was triggered by a key event.
*/

/*!
    \reimp
*/
void Q3ListView::changeEvent(QEvent *ev)
{
    if(ev->type() == QEvent::StyleChange) {
        reconfigureItems();
    } else if(ev->type() == QEvent::ActivationChange) {
        if (!isActiveWindow() && d->scrollTimer)
            d->scrollTimer->stop();
        if (!palette().isEqual(QPalette::Active, QPalette::Inactive))
            viewport()->update();
    }
    Q3ScrollView::changeEvent(ev);

    if (ev->type() == QEvent::ApplicationFontChange || ev->type() == QEvent::FontChange
        || ev->type() == QEvent::ApplicationPaletteChange || ev->type() == QEvent::PaletteChange)
        reconfigureItems();
}

/*!
    Ensures that setup() is called for all currently visible items,
    and that it will be called for currently invisible items as soon
    as their parents are opened.

    (A visible item, here, is an item whose parents are all open. The
    item may happen to be off-screen.)

    \sa Q3ListViewItem::setup()
*/

void Q3ListView::reconfigureItems()
{
    d->fontMetricsHeight = fontMetrics().height();
    d->minLeftBearing = fontMetrics().minLeftBearing();
    d->minRightBearing = fontMetrics().minRightBearing();
    d->ellipsisWidth = fontMetrics().width(QLatin1String("...")) * 2;
    d->r->setOpen(false);
    d->r->configured = false;
    d->r->setOpen(true);
}

/*!
    Ensures that the width mode of column \a c is updated according to
    the width of \a item.
*/

void Q3ListView::widthChanged(const Q3ListViewItem* item, int c)
{
    if (c >= d->h->count())
        return;


    QFontMetrics fm = fontMetrics();
    int col = c < 0 ? 0 : c;
    while (col == c || (c < 0 && col < d->h->count())) {
        if (d->column[col].wmode == Maximum) {
            int w = item->width(fm, this, col);
            if (showSortIndicator()) {
                int tw = d->h->sectionSizeHint( col, fm ).width();
                tw += 40; //add space for the sort indicator
                w = qMax(w, tw);
            }
            if (col == 0) {
                int indent = treeStepSize() * item->depth();
                if (rootIsDecorated())
                    indent += treeStepSize();
                w += indent;
            }
            if (w > columnWidth(col) && !d->h->isStretchEnabled() && !d->h->isStretchEnabled(col)) {
                d->updateHeader = true;
                setColumnWidth(col, w);
            }
        }
        col++;
    }
}

/*!
    \property Q3ListView::allColumnsShowFocus
    \brief whether items should show keyboard focus using all columns

    If this property is true all columns will show focus and selection
    states, otherwise only column 0 will show focus.

    The default is false.

    Setting this to true if it's not necessary may cause noticeable
    flicker.
*/

void Q3ListView::setAllColumnsShowFocus(bool enable)
{
    d->allColumnsShowFocus = enable;
}

bool Q3ListView::allColumnsShowFocus() const
{
    return d->allColumnsShowFocus;
}


/*!
    Returns the first item in this Q3ListView. Returns 0 if there is no
    first item.

    A list view's items can be traversed using firstChild()
    and nextSibling() or using a Q3ListViewItemIterator.

    \sa itemAt() Q3ListViewItem::itemBelow() Q3ListViewItem::itemAbove()
*/

Q3ListViewItem * Q3ListView::firstChild() const
{
    if (!d->r)
        return 0;

    d->r->enforceSortOrder();
    return d->r->childItem;
}

/*!
    Returns the last item in the list view tree. Returns 0 if there
    are no items in the Q3ListView.

    This function is slow because it traverses the entire tree to find
    the last item.
*/

Q3ListViewItem* Q3ListView::lastItem() const
{
    Q3ListViewItem* item = firstChild();
    if (item) {
        while (item->nextSibling() || item->firstChild()) {
            if (item->nextSibling())
                item = item->nextSibling();
            else
                item = item->firstChild();
        }
    }
    return item;
}

/*!
    Repaints this item on the screen if it is currently visible.
*/

void Q3ListViewItem::repaint() const
{
    Q3ListView *lv = listView();
    if (lv)
        lv->repaintItem(this);
}


/*!
    Repaints \a item on the screen if \a item is currently visible.
    Takes care to avoid multiple repaints.
*/

void Q3ListView::repaintItem(const Q3ListViewItem * item) const
{
    if (!item)
        return;
    d->dirtyItemTimer->start(0, true);
    d->dirtyItems.append(item);
}


struct Q3CheckListItemPrivate
{
    Q3CheckListItemPrivate():
        exclusive(0),
        currentState(Q3CheckListItem::Off),
        tristate(false) {}

    Q3CheckListItem *exclusive;
    Q3CheckListItem::ToggleState currentState;
    QHash<Q3CheckListItem *, Q3CheckListItem::ToggleState> statesDict;
    bool tristate;
};


/*!
    \class Q3CheckListItem
    \brief The Q3CheckListItem class provides checkable list view items.

    \compat

    Q3CheckListItems are used in \l{Q3ListView}s to provide
    \l{Q3ListViewItem}s that are checkboxes, radio buttons or
    controllers.

    Checkbox and controller check list items may be inserted at any
    level in a list view. Radio button check list items must be
    children of a controller check list item.

    The item can be checked or unchecked with setOn(). Its type can be
    retrieved with type() and its text retrieved with text().

    \img qlistviewitems.png List View Items

    \sa Q3ListViewItem Q3ListView
*/

/*!
    \enum Q3CheckListItem::Type

    This enum type specifies a Q3CheckListItem's type:

    \value RadioButton
    \value CheckBox
    \value RadioButtonController
    \value CheckBoxController
    \omitvalue Controller
*/

/*!
    \enum Q3CheckListItem::ToggleState

    This enum specifies a Q3CheckListItem's toggle state.

    \value Off
    \value NoChange
    \value On
*/


/*!
    Constructs a checkable item with parent \a parent, text \a text
    and of type \a tt. Note that a \c RadioButton must be the child of a
    \c RadioButtonController, otherwise it will not toggle.
*/
Q3CheckListItem::Q3CheckListItem(Q3CheckListItem *parent, const QString &text,
                                Type tt)
    : Q3ListViewItem(parent, text, QString())
{
    myType = tt;
    init();
    if (myType == RadioButton) {
        if (parent->type() != RadioButtonController)
            qWarning("Q3CheckListItem::Q3CheckListItem(), radio button must be "
                      "child of a controller");
        else
            d->exclusive = parent;
    }
}

/*!
    Constructs a checkable item with parent \a parent, which is after
    \a after in the parent's list of children, and with text \a text
    and of type \a tt. Note that a \c RadioButton must be the child of
    a \c RadioButtonController, otherwise it will not toggle.
*/
Q3CheckListItem::Q3CheckListItem(Q3CheckListItem *parent, Q3ListViewItem *after,
                                const QString &text, Type tt)
    : Q3ListViewItem(parent, after, text)
{
    myType = tt;
    init();
    if (myType == RadioButton) {
        if (parent->type() != RadioButtonController)
            qWarning("Q3CheckListItem::Q3CheckListItem(), radio button must be "
                      "child of a controller");
        else
            d->exclusive = parent;
    }
}

/*!
    Constructs a checkable item with parent \a parent, text \a text
    and of type \a tt. Note that this item must \e not be a \c
    RadioButton. Radio buttons must be children of a \c
    RadioButtonController.
*/
Q3CheckListItem::Q3CheckListItem(Q3ListViewItem *parent, const QString &text,
                                Type tt)
    : Q3ListViewItem(parent, text, QString())
{
    myType = tt;
    if (myType == RadioButton) {
      qWarning("Q3CheckListItem::Q3CheckListItem(), radio button must be "
               "child of a Q3CheckListItem");
    }
    init();
}

/*!
    Constructs a checkable item with parent \a parent, which is after
    \a after in the parent's list of children, with text \a text and
    of type \a tt. Note that this item must \e not be a \c
    RadioButton. Radio buttons must be children of a \c
    RadioButtonController.
*/
Q3CheckListItem::Q3CheckListItem(Q3ListViewItem *parent, Q3ListViewItem *after,
                                const QString &text, Type tt)
    : Q3ListViewItem(parent, after, text)
{
    myType = tt;
    if (myType == RadioButton) {
        qWarning("Q3CheckListItem::Q3CheckListItem(), radio button must be "
                  "child of a Q3CheckListItem");
    }
    init();
}


/*!
    Constructs a checkable item with parent \a parent, text \a text
    and of type \a tt. Note that \a tt must \e not be \c RadioButton.
    Radio buttons must be children of a \c RadioButtonController.
*/
Q3CheckListItem::Q3CheckListItem(Q3ListView *parent, const QString &text,
                                Type tt)
    : Q3ListViewItem(parent, text)
{
    myType = tt;
    if (tt == RadioButton)
        qWarning("Q3CheckListItem::Q3CheckListItem(), radio button must be "
                 "child of a Q3CheckListItem");
    init();
}

/*!
    Constructs a checkable item with parent \a parent, which is after
    \a after in the parent's list of children, with text \a text and
    of type \a tt. Note that \a tt must \e not be \c RadioButton.
    Radio buttons must be children of a \c RadioButtonController.
*/
Q3CheckListItem::Q3CheckListItem(Q3ListView *parent, Q3ListViewItem *after,
                                const QString &text, Type tt)
    : Q3ListViewItem(parent, after, text)
{
    myType = tt;
    if (tt == RadioButton)
        qWarning("Q3CheckListItem::Q3CheckListItem(), radio button must be "
                  "child of a Q3CheckListItem");
    init();
}


/* \reimp */

int Q3CheckListItem::rtti() const
{
    return RTTI;
}

/*!
    Constructs a \c RadioButtonController item with parent \a parent,
    text \a text and pixmap \a p.
*/
Q3CheckListItem::Q3CheckListItem(Q3ListView *parent, const QString &text,
                                const QPixmap & p)
    : Q3ListViewItem(parent, text)
{
    myType = RadioButtonController;
    setPixmap(0, p);
    init();
}

/*!
    Constructs a \c RadioButtonController item with parent \a parent,
    text \a text and pixmap \a p.
*/
Q3CheckListItem::Q3CheckListItem(Q3ListViewItem *parent, const QString &text,
                                const QPixmap & p)
    : Q3ListViewItem(parent, text)
{
    myType = RadioButtonController;
    setPixmap(0, p);
    init();
}

void Q3CheckListItem::init()
{
    d = new Q3CheckListItemPrivate();
    on = false;
    // CheckBoxControllers by default have tristate set to true
    if (myType == CheckBoxController)
        setTristate(true);
}

/*!
    Destroys the item, and all its children to any depth, freeing up
    all allocated resources.
*/
Q3CheckListItem::~Q3CheckListItem()
{
    if (myType == RadioButton
         && d->exclusive && d->exclusive->d
         && d->exclusive->d->exclusive == this)
        d->exclusive->turnOffChild();
    d->exclusive = 0; // so the children won't try to access us.
    delete d;
    d = 0;
}

/*!
    \fn Q3CheckListItem::Type Q3CheckListItem::type() const

    Returns the type of this item.
*/

/*!
   \fn  bool Q3CheckListItem::isOn() const

    Returns true if the item is toggled on; otherwise returns false.
*/

/*!
   Sets tristate to \a b if the \c Type is either a \c CheckBoxController or
   a \c CheckBox.

   \c CheckBoxControllers are tristate by default.

   \sa state() isTristate()
*/
void Q3CheckListItem::setTristate(bool b)
{
    if ((myType != CheckBoxController) && (myType != CheckBox)) {
        qWarning("Q3CheckListItem::setTristate(), has no effect on RadioButton "
                  "or RadioButtonController.");
        return;
    }
    d->tristate = b;
}

/*!
   Returns true if the item is tristate; otherwise returns false.

   \sa setTristate()
*/
bool Q3CheckListItem::isTristate() const
{
    return d->tristate;
}

/*!
    Returns the state of the item.

    \sa Q3CheckListItem::ToggleState
*/
Q3CheckListItem::ToggleState Q3CheckListItem::state() const
{
    if (!isTristate() && internalState() == NoChange)
        return Off;
    else
        return d->currentState;
}

/*
  Same as the public state() except this one does not mask NoChange into Off
  when tristate is disabled.
*/
Q3CheckListItem::ToggleState Q3CheckListItem::internalState() const
{
    return d->currentState;
}




/*!
    Sets the toggle state of the checklistitem to \a s. \a s can be
    \c Off, \c NoChange or \c On.

    Tristate can only be enabled for \c CheckBox or \c CheckBoxController,
    therefore the \c NoChange only applies to them.

    Setting the state to \c On or \c Off on a \c CheckBoxController
    will recursivly set the states of its children to the same state.

    Setting the state to \c NoChange on a \c CheckBoxController will
    make it recursivly recall the previous stored state of its
    children. If there was no previous stored state the children are
    all set to \c On.
*/
void Q3CheckListItem::setState(ToggleState s)
{
    if (myType == CheckBoxController && state() == NoChange)
        updateStoredState(this);
    setState(s, true, true);
}

/*
  Sets the toggle state of the checklistitems. \a update tells if the
  controller / parent controller should be aware of these changes, \a store
  tells if the parent should store its children if certain conditions arise
*/
void Q3CheckListItem::setState(ToggleState s, bool update, bool store)
{

    if (s == internalState())
        return;

    if (myType == CheckBox) {
        setCurrentState(s);
        stateChange(state());
        if (update && parent() && parent()->rtti() == 1
             && ((Q3CheckListItem*)parent())->type() == CheckBoxController)
            ((Q3CheckListItem*)parent())->updateController(update, store);
    } else if (myType == CheckBoxController) {
        if (s == NoChange && childCount()) {
            restoreState(this);
        } else {
            Q3ListViewItem *item = firstChild();
            int childCount = 0;
            while(item) {
                if (item->rtti() == 1 &&
                     (((Q3CheckListItem*)item)->type() == CheckBox ||
                       ((Q3CheckListItem*)item)->type() == CheckBoxController)) {
                    Q3CheckListItem *checkItem = (Q3CheckListItem*)item;
                    checkItem->setState(s, false, false);
                    childCount++;
                }
                item = item->nextSibling();
            }
            if (update) {
                if (childCount > 0) {
                    ToggleState oldState = internalState();
                    updateController(false, false);
                    if (oldState != internalState() &&
                         parent() && parent()->rtti() == 1 &&
                         ((Q3CheckListItem*)parent())->type() == CheckBoxController)
                        ((Q3CheckListItem*)parent())->updateController(update, store);

                        updateController(update, store);
                } else {
                    // if there are no children we simply set the CheckBoxController and update its parent
                    setCurrentState(s);
                    stateChange(state());
                    if (parent() && parent()->rtti() == 1
                         && ((Q3CheckListItem*)parent())->type() == CheckBoxController)
                        ((Q3CheckListItem*)parent())->updateController(update, store);
                }
            } else {
                setCurrentState(s);
                stateChange(state());
            }

        }
    } else if (myType == RadioButton) {
        if (s == On) {
            if (d->exclusive && d->exclusive->d->exclusive != this)
                d->exclusive->turnOffChild();
            setCurrentState(s);
            if (d->exclusive)
                d->exclusive->d->exclusive = this;
        } else {
            if (d->exclusive && d->exclusive->d->exclusive == this)
                d->exclusive->d->exclusive = 0;
            setCurrentState(Off);
        }
        stateChange(state());
    }
    repaint();
}

/*
  this function is needed because we need to update "on" every time we
  update d->currentState. In order to retain binary compatibility the
  inline function isOn() needs the "on" bool ### should be changed in
  ver 4
*/
void Q3CheckListItem::setCurrentState(ToggleState s)
{
    ToggleState old = d->currentState;
    d->currentState = s;
    if (d->currentState == On)
        on = true;
    else
        on = false;

#ifndef QT_NO_ACCESSIBILITY
    if (old != d->currentState && listView())
        QAccessible::updateAccessibility(listView()->viewport(), indexOfItem(this), QAccessible::StateChanged);
#else
    Q_UNUSED(old);
#endif
}



/*
  updates the internally stored state of this item for the parent (key)
*/
void Q3CheckListItem::setStoredState(ToggleState newState, Q3CheckListItem *key)
{
    if (myType == CheckBox || myType == CheckBoxController)
        d->statesDict[key] = newState;
}

/*
  Returns the stored state for this item for the given key.
  If the key is not found it returns Off.
*/
Q3CheckListItem::ToggleState Q3CheckListItem::storedState(Q3CheckListItem *key) const
{
    QHash<Q3CheckListItem *, Q3CheckListItem::ToggleState>::Iterator it = d->statesDict.find(key);
    if (it != d->statesDict.end())
        return it.value();
    else
        return Off;
}


/*!
    \fn QString Q3CheckListItem::text() const

    Returns the item's text.
*/


/*!
    If this is a \c RadioButtonController that has \c RadioButton
    children, turn off the child that is on.
*/
void Q3CheckListItem::turnOffChild()
{
    if (myType == RadioButtonController && d->exclusive)
        d->exclusive->setOn(false);
}

/*!
    Toggle check box or set radio button to on.
*/
void Q3CheckListItem::activate()
{
    Q3ListView * lv = listView();

    if ((lv && !lv->isEnabled()) || !isEnabled())
        return;

    QPoint pos;
    int boxsize = lv->style()->pixelMetric(QStyle::PM_CheckListButtonSize, 0, lv);
    if (activatedPos(pos)) {
        bool parentControl = false;
        if (parent() && parent()->rtti() == 1  &&
            ((Q3CheckListItem*) parent())->type() == RadioButtonController)
            parentControl = true;

        int x = parentControl ? 0 : 3;
        int align = lv->columnAlignment(0);
        int marg = lv->itemMargin();
        int y = 0;

        if (align & Qt::AlignVCenter)
            y = ((height() - boxsize) / 2) + marg;
        else
            y = (lv->fontMetrics().height() + 2 + marg - boxsize) / 2;

        QRect r(x, y, boxsize-3, boxsize-3);
        // columns might have been swapped
        r.moveBy(lv->header()->sectionPos(0), 0);
        if (!r.contains(pos))
            return;
    }
    if ((myType == CheckBox) || (myType == CheckBoxController))  {
        lv->d->startEdit = FALSE;
        switch (internalState()) {
        case On:
            setState(Off);
            break;
        case Off:
	    if ( (!isTristate() && myType == CheckBox) ||
                 (myType == CheckBoxController && !childCount()) ) {
                setState(On);
            } else {
                setState(NoChange);
                if (myType == CheckBoxController && internalState() != NoChange)
                    setState(On);
            }
            break;
        case NoChange:
            setState(On);
            break;
        }
        ignoreDoubleClick();
    } else if (myType == RadioButton) {
        setOn(true);
        ignoreDoubleClick();
    }
}

/*!
    Sets the button on if \a b is true, otherwise sets it off.
    Maintains radio button exclusivity.
*/
void Q3CheckListItem::setOn(bool b )
{
    if (b)
        setState(On , true, true);
    else
        setState(Off , true, true);
}


/*!
    \fn void Q3CheckListItem::stateChange(bool b)

    This virtual function is called when the item changes its state.
    \a b is true if the state is \c On; otherwise the state is \c Off.
    \c NoChange (if tristate is enabled and the type is either \c
    CheckBox or \c CheckBoxController) reports the same as \c Off, so
    use state() to determine if the state is actually \c Off or \c
    NoChange.
*/
void Q3CheckListItem::stateChange(bool)
{
}

/*
  Calls the public virtual function if the state is changed to either On, NoChange or Off.
  NoChange reports the same as Off - ### should be fixed in ver4
*/
void Q3CheckListItem::stateChange(ToggleState s)
{
    stateChange(s == On);
}

/*
  sets the state of the CheckBox and CheckBoxController back to
  previous stored state
*/
void Q3CheckListItem::restoreState(Q3CheckListItem *key, int depth)
{
    switch (type()) {
    case CheckBox:
        setCurrentState(storedState(key));
        stateChange(state());
        repaint();
        break;
    case CheckBoxController: {
        Q3ListViewItem *item = firstChild();
        int childCount = 0;
        while (item) {
            // recursively calling restoreState for children of type CheckBox and CheckBoxController
            if (item->rtti() == 1 &&
                 (((Q3CheckListItem*)item)->type() == CheckBox ||
                   ((Q3CheckListItem*)item)->type() == CheckBoxController)) {
                ((Q3CheckListItem*)item)->restoreState(key , depth+1);
                childCount++;
            }
            item = item->nextSibling();
        }
        if (childCount > 0) {
            if (depth == 0)
                updateController(true);
            else
                updateController(false);
        } else {
            // if there are no children we retrieve the CheckBoxController state directly.
            setState(storedState(key), true, false);
        }
    }
        break;
    default:
        break;
    }
}


/*
  Checks the childrens state and updates the controllers state
  if necessary. If the controllers state change, then his parent again is
  called to update itself.
*/
void Q3CheckListItem::updateController(bool update , bool store)
{
    if (myType != CheckBoxController)
        return;

    Q3CheckListItem *controller = 0;
    // checks if this CheckBoxController has another CheckBoxController as parent
    if (parent() && parent()->rtti() == 1
         && ((Q3CheckListItem*)parent())->type() == CheckBoxController)
        controller = (Q3CheckListItem*)parent();

    ToggleState theState = Off;
    bool first = true;
    Q3ListViewItem *item = firstChild();
    while(item && theState != NoChange) {
        if (item->rtti() == 1 &&
             (((Q3CheckListItem*)item)->type() == CheckBox ||
               ((Q3CheckListItem*)item)->type() == CheckBoxController)) {
            Q3CheckListItem *checkItem = (Q3CheckListItem*)item;
            if (first) {
                theState = checkItem->internalState();
                first = false;
            } else {
                if (checkItem->internalState() == NoChange ||
                     theState != checkItem->internalState())
                    theState = NoChange;
                else
                    theState = checkItem->internalState();
            }
        }
        item = item->nextSibling();
    }
    if (internalState() != theState) {
        setCurrentState(theState);
        if (store && (internalState() == On || internalState() == Off))
            updateStoredState(this);
        stateChange(state());
        if (update && controller) {
            controller->updateController(update, store);
        }
        repaint();
    }
}


/*
  Makes all the children CheckBoxes update their storedState
*/
void Q3CheckListItem::updateStoredState(Q3CheckListItem *key)
{
    if (myType != CheckBoxController)
        return;

    Q3ListViewItem *item = firstChild();
    while(item) {
        if (item->rtti() == 1) {
            Q3CheckListItem *checkItem = (Q3CheckListItem*)item;
            if (checkItem->type() == CheckBox)
                checkItem->setStoredState(checkItem->internalState(), key);
            else if (checkItem->type() == CheckBoxController)
                checkItem->updateStoredState(key);
        }
        item = item->nextSibling();
    }
    // this state is only needed if the CheckBoxController has no CheckBox / CheckBoxController children.
    setStoredState(internalState() , key);
}


/*!
    \reimp
*/
void Q3CheckListItem::setup()
{
    Q3ListViewItem::setup();
    int h = height();
    Q3ListView *lv = listView();
    if (lv)
        h = qMax(lv->style()->pixelMetric(QStyle::PM_CheckListButtonSize, 0, lv),
                  h);
    h = qMax(h, QApplication::globalStrut().height());
    setHeight(h);
}

/*!
    \reimp
*/

int Q3CheckListItem::width(const QFontMetrics& fm, const Q3ListView* lv, int column) const
{
    int r = Q3ListViewItem::width(fm, lv, column);
    if (column == 0) {
        r += lv->itemMargin();
        if (myType == RadioButtonController && pixmap(0)) {
            //             r += 0;
        } else {
            r +=  lv->style()->pixelMetric(QStyle::PM_CheckListButtonSize, 0, lv) + 4;
        }
    }
    return qMax(r, QApplication::globalStrut().width());
}

/*!
    Paints the item using the painter \a p and the color group \a cg.
    The item is in column \a column, has width \a width and has
    alignment \a align. (See \l Qt::Alignment for valid alignments.)
*/
void Q3CheckListItem::paintCell(QPainter * p, const QColorGroup & cg,
                               int column, int width, int align)
{
    if (!p)
        return;

    Q3ListView *lv = listView();
    if (!lv)
        return;

    const QPalette::ColorRole crole = lv->backgroundRole();
    if (cg.brush(crole) != lv->palette().brush(cg.currentColorGroup(), crole))
        p->fillRect(0, 0, width, height(), cg.brush(crole));
    else
        lv->paintEmptyArea(p, QRect(0, 0, width, height()));

    if (column != 0) {
        // The rest is text, or for subclasses to change.
        Q3ListViewItem::paintCell(p, cg, column, width, align);
        return;
    }

    bool parentControl = false;
    if (parent() && parent()->rtti() == 1  &&
         ((Q3CheckListItem*) parent())->type() == RadioButtonController)
        parentControl = true;

    QFontMetrics fm(lv->fontMetrics());
    int boxsize = lv->style()->pixelMetric(myType == RadioButtonController ? QStyle::PM_CheckListControllerSize :
                                           QStyle::PM_CheckListButtonSize, 0, lv);
    int marg = lv->itemMargin();
    int r = marg;

    // Draw controller / check box / radio button ---------------------
    QStyle::State styleflags = QStyle::State_None;
    if (internalState() == On) {
        styleflags |= QStyle::State_On;
    } else if (internalState() == NoChange) {
        if (myType == CheckBoxController && !isTristate())
            styleflags |= QStyle::State_Off;
        else
            styleflags |= QStyle::State_NoChange;
    } else {
        styleflags |= QStyle::State_Off;
    }
    if (isSelected())
        styleflags |= QStyle::State_Selected;
    if (isEnabled() && lv->isEnabled())
        styleflags |= QStyle::State_Enabled;
    if (lv->window()->isActiveWindow())
        styleflags |= QStyle::State_Active;

    if (myType == RadioButtonController) {
        int x = 0;
        if(!parentControl)
            x += 3;
        if (!pixmap(0)) {
            QStyleOptionQ3ListView opt = getStyleOption(lv, this);
            opt.rect.setRect(x, 0, boxsize, fm.height() + 2 + marg);
            opt.palette = cg;
            opt.state = styleflags;
            lv->style()->drawPrimitive(QStyle::PE_Q3CheckListController, &opt, p, lv);
            r += boxsize + 4;
        }
    } else {
        Q_ASSERT(lv); //###
        int x = 0;
        int y = 0;
        if (!parentControl)
            x += 3;
        if (align & Qt::AlignVCenter)
            y = ((height() - boxsize) / 2) + marg;
        else
            y = (fm.height() + 2 + marg - boxsize) / 2;

        QStyleOptionQ3ListView opt = getStyleOption(lv, this);
        opt.rect.setRect(x, y, boxsize, fm.height() + 2 + marg);
        opt.palette = cg;
        opt.state = styleflags;
        lv->style()->drawPrimitive((myType == CheckBox || myType == CheckBoxController)
                                    ? QStyle::PE_Q3CheckListIndicator
                                    : QStyle::PE_Q3CheckListExclusiveIndicator, &opt, p, lv);
        r += boxsize + 4;
    }

    // Draw text ----------------------------------------------------
    p->translate(r, 0);
    p->setPen(QPen(cg.text()));
    Q3ListViewItem::paintCell(p, cg, column, width - r, align);
}

/*!
    Draws the focus rectangle \a r using the color group \a cg on the
    painter \a p.
*/
void Q3CheckListItem::paintFocus(QPainter *p, const QColorGroup & cg,
                                 const QRect & r)
{
    bool intersect = true;
    Q3ListView *lv = listView();
    if (lv && lv->header()->mapToActual(0) != 0) {
        int xdepth = lv->treeStepSize() * (depth() + (lv->rootIsDecorated() ? 1 : 0)) + lv->itemMargin();
        int p = lv->header()->cellPos(lv->header()->mapToActual(0));
        xdepth += p;
        intersect = r.intersects(QRect(p, r.y(), xdepth - p + 1, r.height()));
    }
    bool parentControl = false;
    if (parent() && parent()->rtti() == 1  &&
         ((Q3CheckListItem*) parent())->type() == RadioButtonController)
        parentControl = true;
    if (myType != RadioButtonController && intersect &&
         (lv->rootIsDecorated() || myType == RadioButton ||
          (myType == CheckBox && parentControl))) {
        QRect rect;
        int boxsize = lv->style()->pixelMetric(QStyle::PM_CheckListButtonSize, 0, lv);
        if (lv->columnAlignment(0) == Qt::AlignCenter) {
            QFontMetrics fm(lv->font());
            int bx = (lv->columnWidth(0) - (boxsize + fm.width(text())))/2 + boxsize;
            if (bx < 0) bx = 0;
            rect.setRect(r.x() + bx + 5, r.y(), r.width() - bx - 5,
                          r.height());
        } else
            rect.setRect(r.x() + boxsize + 5, r.y(), r.width() - boxsize - 5,
                          r.height());
        Q3ListViewItem::paintFocus(p, cg, rect);
    } else {
        Q3ListViewItem::paintFocus(p, cg, r);
    }
}

/*!
    \reimp
*/
QSize Q3ListView::sizeHint() const
{
    if (cachedSizeHint().isValid())
        return cachedSizeHint();

    ensurePolished();

    if (!isVisible() && d->drawables.isEmpty())
        // force the column widths to sanity, if possible
        buildDrawableList();

    QSize s(d->h->sizeHint());
    if (verticalScrollBar()->isVisible())
        s.setWidth(s.width() + style()->pixelMetric(QStyle::PM_ScrollBarExtent));
    s += QSize(frameWidth()*2,frameWidth()*2);
    Q3ListViewItem * l = d->r;
    while(l && !l->height())
        l = l->childItem ? l->childItem : l->siblingItem;

    if (l && l->height())
        s.setHeight(s.height() + 10 * l->height());
    else
        s.setHeight(s.height() + 140);

    if (s.width() > s.height() * 3)
        s.setHeight(s.width() / 3);
    else if (s.width() *3 < s.height())
        s.setHeight(s.width() * 3);

    setCachedSizeHint(s);

    return s;
}


/*!
    \reimp
*/

QSize Q3ListView::minimumSizeHint() const
{
    return Q3ScrollView::minimumSizeHint();
}


/*!
    Sets \a item to be open if \a open is true and \a item is
    expandable, and to be closed if \a open is false. Repaints
    accordingly.

    \sa Q3ListViewItem::setOpen() Q3ListViewItem::setExpandable()
*/

void Q3ListView::setOpen(Q3ListViewItem * item, bool open)
{
    if (!item ||
        item->isOpen() == open ||
        (open && !item->childCount() && !item->isExpandable()))
        return;

    Q3ListViewItem* nextParent = 0;
    if (open)
        nextParent = item->itemBelow();

    item->setOpen(open);

    if (open) {
        Q3ListViewItem* lastChild = item;
        Q3ListViewItem* tmp;
        while (true) {
            tmp = lastChild->itemBelow();
            if (!tmp || tmp == nextParent)
                break;
            lastChild = tmp;
        }
        ensureItemVisible(lastChild);
        ensureItemVisible(item);
    }
    buildDrawableList();

    int i = 0;
    for (; i < d->drawables.size(); ++i) {
        const Q3ListViewPrivate::DrawableItem &c = d->drawables.at(i);
        if(c.i == item)
            break;
    }

    if (i < d->drawables.size()) {
        d->dirtyItemTimer->start(0, true);
        for (; i < d->drawables.size(); ++i) {
            const Q3ListViewPrivate::DrawableItem &c = d->drawables.at(i);
            d->dirtyItems.append(c.i);
        }
    }
}


/*!
    Returns true if this list view item has children \e and they are
    not explicitly hidden; otherwise returns false.

    Identical to \a{item}->isOpen(). Provided for completeness.

    \sa setOpen()
*/

bool Q3ListView::isOpen(const Q3ListViewItem * item) const
{
    return item->isOpen();
}


/*!
    \property Q3ListView::rootIsDecorated
    \brief whether the list view shows open/close signs on root items

    Open/close signs are small \bold{+} or \bold{-} symbols in windows
    style, or arrows in Motif style. The default is false.
*/

void Q3ListView::setRootIsDecorated(bool enable)
{
    if (enable != (bool)d->rootIsExpandable) {
        d->rootIsExpandable = enable;
        if (isVisible())
            triggerUpdate();
    }
}

bool Q3ListView::rootIsDecorated() const
{
    return d->rootIsExpandable;
}


/*!
    Ensures that item \a i is visible, scrolling the list view
    vertically if necessary and opening (expanding) any parent items
    if this is required to show the item.

    \sa itemRect() Q3ScrollView::ensureVisible()
*/

void Q3ListView::ensureItemVisible(const Q3ListViewItem * i)
{
    if (!i || !i->isVisible())
        return;

    Q3ListViewItem *parent = i->parent();
    while (parent) {
        if (!parent->isOpen())
            parent->setOpen(true);
        parent = parent->parent();
    }

    if (d->r->maybeTotalHeight < 0)
        updateGeometries();
    int y = itemPos(i);
    int h = i->height();
    if (isVisible() && y + h > contentsY() + visibleHeight())
        setContentsPos(contentsX(), y - visibleHeight() + h);
    else if (!isVisible() || y < contentsY())
        setContentsPos(contentsX(), y);
}


/*!
    \fn QString Q3CheckListItem::text(int n) const

    \reimp
*/

/*!
    Returns the Q3Header object that manages this list view's columns.
    Please don't modify the header behind the list view's back.

    You may safely call Q3Header::setClickEnabled(),
    Q3Header::setResizeEnabled(), Q3Header::setMovingEnabled(),
    Q3Header::hide() and all the const Q3Header functions.
*/

Q3Header * Q3ListView::header() const
{
    return d->h;
}


/*!
    \property Q3ListView::childCount
    \brief the number of parentless (top-level) Q3ListViewItem objects in this Q3ListView

    Holds the current number of parentless (top-level) Q3ListViewItem
    objects in this Q3ListView.

    \sa Q3ListViewItem::childCount()
*/

int Q3ListView::childCount() const
{
    if (d->r)
        return d->r->childCount();
    return 0;
}


/*
    Moves this item to just after \a olderSibling. \a olderSibling and
    this object must have the same parent.

    If you need to move an item in the hierarchy use takeItem() and
    insertItem().
*/

void Q3ListViewItem::moveToJustAfter(Q3ListViewItem * olderSibling)
{
    if (parentItem && olderSibling &&
         olderSibling->parentItem == parentItem && olderSibling != this) {
        if (parentItem->childItem == this) {
            parentItem->childItem = siblingItem;
        } else {
            Q3ListViewItem * i = parentItem->childItem;
            while(i && i->siblingItem != this)
                i = i->siblingItem;
            if (i)
                i->siblingItem = siblingItem;
        }
        siblingItem = olderSibling->siblingItem;
        olderSibling->siblingItem = this;
        parentItem->lsc = Unsorted;
    }
}

/*!
    Move the item to be after item \a after, which must be one of the
    item's siblings. To move an item in the hierarchy, use takeItem()
    and insertItem().

    Note that this function will have no effect if sorting is enabled
    in the list view.
*/

void Q3ListViewItem::moveItem(Q3ListViewItem *after)
{
    if (!after || after == this)
        return;
    if (parent() != after->parent()) {
        if (parentItem)
            parentItem->takeItem(this);
        if (after->parentItem) {
            int tmpLsc = after->parentItem->lsc;
            after->parentItem->insertItem(this);
            after->parentItem->lsc = tmpLsc;
        }
    }
    moveToJustAfter(after);
    Q3ListView *lv = listView();
    if (lv)
        lv->triggerUpdate();
}

/*
    Recursively sorts items, from the root to this item.
    (enforceSortOrder() won't work the other way around, as
    documented.)
*/
void Q3ListViewItem::enforceSortOrderBackToRoot()
{
    if (parentItem) {
        parentItem->enforceSortOrderBackToRoot();
        parentItem->enforceSortOrder();
    }
}

/*!
    \reimp
*/
void Q3ListView::showEvent(QShowEvent *)
{
    d->drawables.clear();
    d->dirtyItems.clear();
    d->dirtyItemTimer->stop();
    d->fullRepaintOnComlumnChange = true;

    updateGeometries();
}


/*!
    Returns the y coordinate of this item in the list view's
    coordinate system. This function is normally much slower than
    Q3ListView::itemAt(), but it works for all items whereas
    Q3ListView::itemAt() normally only works for items on the screen.

    \sa Q3ListView::itemAt() Q3ListView::itemRect() Q3ListView::itemPos()
*/

int Q3ListViewItem::itemPos() const
{
    QStack<Q3ListViewItem *> s;
    Q3ListViewItem * i = (Q3ListViewItem *)this;
    while(i) {
        s.push(i);
        i = i->parentItem;
    }

    int a = 0;
    Q3ListViewItem * p = 0;
    while(s.count()) {
        i = s.pop();
        if (p) {
            if (!p->configured) {
                p->configured = true;
                p->setup(); // ### virtual non-const function called in const
            }
            a += p->height();
            Q3ListViewItem * s = p->firstChild();
            while(s && s != i) {
                a += s->totalHeight();
                s = s->nextSibling();
            }
        }
        p = i;
    }
    return a;
}


/*!
  \fn void Q3ListView::removeItem(Q3ListViewItem *item)

    Removes the given \a item. Use takeItem() instead.
*/

/*!
    Removes item \a i from the list view; \a i must be a top-level
    item. The warnings regarding Q3ListViewItem::takeItem() apply to
    this function, too.

    \sa insertItem()
*/
void Q3ListView::takeItem(Q3ListViewItem * i)
{
    if (d->r)
        d->r->takeItem(i);
}


void Q3ListView::openFocusItem()
{
    d->autoopenTimer->stop();
    if (d->focusItem && !d->focusItem->isOpen()) {
        d->focusItem->setOpen(true);
        d->focusItem->repaint();
    }
}

static const int autoopenTime = 750;

#ifndef QT_NO_DRAGANDDROP

/*! \reimp */

void Q3ListView::contentsDragEnterEvent(QDragEnterEvent *e)
{
    d->oldFocusItem = d->focusItem;
    Q3ListViewItem *i = d->focusItem;
    d->focusItem = itemAt(contentsToViewport(e->pos()));
    if (i)
        i->repaint();
    if (d->focusItem) {
        d->autoopenTimer->start(autoopenTime);
        d->focusItem->dragEntered();
        d->focusItem->repaint();
    }
    e->accept();
}

/*! \reimp */

void Q3ListView::contentsDragMoveEvent(QDragMoveEvent *e)
{
    Q3ListViewItem *i = d->focusItem;
    d->focusItem = itemAt(contentsToViewport(e->pos()));
    if (i) {
        if (i != d->focusItem)
            i->dragLeft();
        i->repaint();
    }
    if (d->focusItem) {
        if (i != d->focusItem) {
            d->focusItem->dragEntered();
            d->autoopenTimer->stop();
            d->autoopenTimer->start(autoopenTime);
        }
        d->focusItem->repaint();
    } else {
        d->autoopenTimer->stop();
    }
    if ((i && i->dropEnabled() && i->acceptDrop(e)) || acceptDrops())
        e->accept();
    else
        e->ignore();
}

/*! \reimp */

void Q3ListView::contentsDragLeaveEvent(QDragLeaveEvent *)
{
    d->autoopenTimer->stop();

    if (d->focusItem)
        d->focusItem->dragLeft();

    setCurrentItem(d->oldFocusItem);
    d->oldFocusItem = 0;
}

/*! \reimp */

void Q3ListView::contentsDropEvent(QDropEvent *e)
{
    d->autoopenTimer->stop();

    setCurrentItem(d->oldFocusItem);
    Q3ListViewItem *i = itemAt(contentsToViewport(e->pos()));
    if (i && i->dropEnabled() && i->acceptDrop(e)) {
        i->dropped(e);
        e->accept();
    } else if (acceptDrops()) {
        emit dropped(e);
        e->accept();
    }
}

/*!
    If the user presses the mouse on an item and starts moving the
    mouse, and the item allow dragging (see
    Q3ListViewItem::setDragEnabled()), this function is called to get a
    drag object and a drag is started unless dragObject() returns 0.

    By default this function returns 0. You should reimplement it and
    create a Q3DragObject depending on the selected items.
*/

Q3DragObject *Q3ListView::dragObject()
{
    return 0;
}

/*!
    Starts a drag.
*/

void Q3ListView::startDrag()
{
    if (!d->startDragItem)
        return;

    d->startDragItem = 0;
    d->buttonDown = false;

    Q3DragObject *drag = dragObject();
    if (!drag)
        return;

    drag->drag();
}

#endif // QT_NO_DRAGANDDROP

/*!
    \property Q3ListView::defaultRenameAction
    \brief What action to perform when the editor loses focus during renaming

    If this property is \c Accept, and the user renames an item and
    the editor loses focus (without the user pressing Enter), the
    item will still be renamed. If the property's value is \c Reject,
    the item will not be renamed unless the user presses Enter. The
    default is \c Reject.
*/

void Q3ListView::setDefaultRenameAction(RenameAction a)
{
    d->defRenameAction = a;
}

Q3ListView::RenameAction Q3ListView::defaultRenameAction() const
{
    return d->defRenameAction;
}

/*!
    Returns true if an item is being renamed; otherwise returns false.
*/

bool Q3ListView::isRenaming() const
{
    return currentItem() && currentItem()->renameBox;
}

/**********************************************************************
 *
 * Class Q3ListViewItemIterator
 *
 **********************************************************************/


/*!
    \class Q3ListViewItemIterator
    \brief The Q3ListViewItemIterator class provides an iterator for collections of Q3ListViewItems.

    \compat

    Construct an instance of a Q3ListViewItemIterator, with either a
    Q3ListView* or a Q3ListViewItem* as argument, to operate on the tree
    of Q3ListViewItems, starting from the argument.

    A Q3ListViewItemIterator iterates over all the items from its
    starting point. This means that it always makes the first child of
    the current item the new current item. If there is no child, the
    next sibling becomes the new current item; and if there is no next
    sibling, the next sibling of the parent becomes current.

    The following example creates a list of all the items that have
    been selected by the user, storing pointers to the items in a
    QList:
    \snippet doc/src/snippets/code/src_qt3support_itemviews_q3listview.cpp 6

    An alternative approach is to use an \c IteratorFlag:
    \snippet doc/src/snippets/code/src_qt3support_itemviews_q3listview.cpp 7

    A Q3ListViewItemIterator provides a convenient and easy way to
    traverse a hierarchical Q3ListView.

    Multiple Q3ListViewItemIterators can operate on the tree of
    Q3ListViewItems. A Q3ListView knows about all iterators operating on
    its Q3ListViewItems. So when a Q3ListViewItem gets removed all
    iterators that point to this item are updated and point to the
    following item if possible, otherwise to a valid item before the
    current one or to 0. Note however that deleting the parent item of
    an item that an iterator points to is not safe.

    \sa Q3ListView, Q3ListViewItem
*/

/*!
    \enum Q3ListViewItemIterator::IteratorFlag

    These flags can be passed to a Q3ListViewItemIterator constructor
    (OR-ed together if more than one is used), so that the iterator
    will only iterate over items that match the given flags.

    \value Visible
    \value Invisible
    \value Selected
    \value Unselected
    \value Selectable
    \value NotSelectable
    \value DragEnabled
    \value DragDisabled
    \value DropEnabled
    \value DropDisabled
    \value Expandable
    \value NotExpandable
    \value Checked
    \value NotChecked
*/

/*!
    Constructs an empty iterator.
*/

Q3ListViewItemIterator::Q3ListViewItemIterator()
    :  curr(0), listView(0), flags(0)
{
}

/*!
    Constructs an iterator for the Q3ListView that contains the \a
    item. The current iterator item is set to point to the \a item.
*/

Q3ListViewItemIterator::Q3ListViewItemIterator(Q3ListViewItem *item)
    :  curr(item), listView(0), flags(0)
{
    if (item) {
        item->enforceSortOrderBackToRoot();
        listView = item->listView();
    }
    if (listView)
        listView->d->iterators.append(this);
}

/*!
    Constructs an iterator for the Q3ListView that contains the \a item
    using the flags \a iteratorFlags. The current iterator item is set
    to point to \a item or the next matching item if \a item doesn't
    match the flags.

    \sa Q3ListViewItemIterator::IteratorFlag
*/

Q3ListViewItemIterator::Q3ListViewItemIterator(Q3ListViewItem *item, int iteratorFlags)
    :  curr(item), listView(0), flags(iteratorFlags)
{
    // go to next matching item if the current don't match
    if (curr && !matchesFlags(curr))
        ++(*this);

    if (curr) {
        curr->enforceSortOrderBackToRoot();
        listView = curr->listView();
    }
    if (listView)
        listView->d->iterators.append(this);
}


/*!
    Constructs an iterator for the same Q3ListView as \a it. The
    current iterator item is set to point on the current item of \a
    it.
*/

Q3ListViewItemIterator::Q3ListViewItemIterator(const Q3ListViewItemIterator& it)
    : curr(it.curr), listView(it.listView), flags(it.flags)
{
    if (listView)
        listView->d->iterators.append(this);
}

/*!
    Constructs an iterator for the Q3ListView \a lv. The current
    iterator item is set to point on the first child (Q3ListViewItem)
    of \a lv.
*/

Q3ListViewItemIterator::Q3ListViewItemIterator(Q3ListView *lv)
    : curr(lv->firstChild()), listView(lv), flags(0)
{
    if (listView)
        listView->d->iterators.append(this);
}

/*!
    Constructs an iterator for the Q3ListView \a lv with the flags \a
    iteratorFlags. The current iterator item is set to point on the
    first child (Q3ListViewItem) of \a lv that matches the flags.

    \sa Q3ListViewItemIterator::IteratorFlag
*/

Q3ListViewItemIterator::Q3ListViewItemIterator(Q3ListView *lv, int iteratorFlags)
    : curr (lv->firstChild()), listView(lv), flags(iteratorFlags)
{
    if (listView)
        listView->d->iterators.append(this);
    if (!matchesFlags(curr))
        ++(*this);
}



/*!
    Assignment. Makes a copy of \a it and returns a reference to its
    iterator.
*/

Q3ListViewItemIterator &Q3ListViewItemIterator::operator=(const Q3ListViewItemIterator &it)
{
    if (listView)
        listView->d->iterators.removeAll(this);

    listView = it.listView;
    curr = it.curr;
    flags = it.flags;
    if (listView)
        listView->d->iterators.append(this);

    // go to next matching item if the current don't match
    if (curr && !matchesFlags(curr))
        ++(*this);

    return *this;
}

/*!
    Destroys the iterator.
*/

Q3ListViewItemIterator::~Q3ListViewItemIterator()
{
    if (listView)
        listView->d->iterators.removeAll(this);
}

/*!
    Prefix ++. Makes the next item the new current item and returns
    it. Returns 0 if the current item is the last item or the
    Q3ListView is 0.
*/

Q3ListViewItemIterator &Q3ListViewItemIterator::operator++()
{
    if (!curr)
        return *this;

    Q3ListViewItem *item = curr->firstChild();
    if (!item) {
        while ((item = curr->nextSibling()) == 0 ) {
            curr = curr->parent();
            if (curr == 0)
                break;
        }
    }
    curr = item;
    // if the next one doesn't match the flags we try one more ahead
    if (curr && !matchesFlags(curr))
        ++(*this);
    return *this;
}

/*!
    \overload

    Postfix ++. Makes the next item the new current item and returns
    the item that \e was the current item.
*/

const Q3ListViewItemIterator Q3ListViewItemIterator::operator++(int)
{
    Q3ListViewItemIterator oldValue = *this;
    ++(*this);
    return oldValue;
}

/*!
    Sets the current item to the item \a j positions after the current
    item. If that item is beyond the last item, the current item is
    set to 0. Returns the current item.
*/

Q3ListViewItemIterator &Q3ListViewItemIterator::operator+=(int j)
{
    while (curr && j--)
        ++(*this);

    return *this;
}

/*!
    Prefix --. Makes the previous item the new current item and
    returns it. Returns 0 if the current item is the first item or the
    Q3ListView is 0.
*/

Q3ListViewItemIterator &Q3ListViewItemIterator::operator--()
{
    if (!curr)
        return *this;

    if (!curr->parent()) {
        // we are in the first depth
       if (curr->listView()) {
            if (curr->listView()->firstChild() != curr) {
                // go the previous sibling
                Q3ListViewItem *i = curr->listView()->firstChild();
                while (i && i->siblingItem != curr)
                    i = i->siblingItem;

                curr = i;

                if (i && i->firstChild()) {
                    // go to the last child of this item
                    Q3ListViewItemIterator it(curr->firstChild());
                    for (; it.current() && it.current()->parent(); ++it)
                        curr = it.current();
                }

                if (curr && !matchesFlags(curr))
                    --(*this);

                return *this;
            } else {
                //we are already the first child of the list view, so it's over
                curr = 0;
                return *this;
            }
        } else
            return *this;
    } else {
        Q3ListViewItem *parent = curr->parent();

        if (curr != parent->firstChild()) {
            // go to the previous sibling
            Q3ListViewItem *i = parent->firstChild();
            while (i && i->siblingItem != curr)
                i = i->siblingItem;

            curr = i;

            if (i && i->firstChild()) {
                // go to the last child of this item
                Q3ListViewItemIterator it(curr->firstChild());
                for (; it.current() && it.current()->parent() != parent; ++it)
                    curr = it.current();
            }

            if (curr && !matchesFlags(curr))
                --(*this);

            return *this;
        } else {
            // make our parent the current item
            curr = parent;

            if (curr && !matchesFlags(curr))
                --(*this);

            return *this;
        }
    }
}

/*!
    \overload

    Postfix --. Makes the previous item the new current item and
    returns the item that \e was the current item.
*/

const Q3ListViewItemIterator Q3ListViewItemIterator::operator--(int)
{
    Q3ListViewItemIterator oldValue = *this;
    --(*this);
    return oldValue;
}

/*!
    Sets the current item to the item \a j positions before the
    current item. If that item is before the first item, the current
    item is set to 0. Returns the current item.
*/

Q3ListViewItemIterator &Q3ListViewItemIterator::operator-=(int j)
{
    while (curr && j--)
        --(*this);

    return *this;
}

/*!
    Dereference operator. Returns a reference to the current item. The
    same as current().
*/

Q3ListViewItem* Q3ListViewItemIterator::operator*()
{
    if (curr != 0 && !matchesFlags(curr))
        qWarning("Q3ListViewItemIterator::operator*() curr out of sync");
    return curr;
}

/*!
    Returns iterator's current item.
*/

Q3ListViewItem *Q3ListViewItemIterator::current() const
{
    if (curr != 0 && !matchesFlags(curr))
        qWarning("Q3ListViewItemIterator::current() curr out of sync");
    return curr;
}

/*
    This function is called to notify the iterator that the current
    item has been deleted, and sets the current item point to another
    (valid) item or 0.
*/

void Q3ListViewItemIterator::currentRemoved()
{
    if (!curr) return;

    if (curr->parent())
        curr = curr->parent();
    else if (curr->nextSibling())
        curr = curr->nextSibling();
    else if (listView && listView->firstChild() &&
              listView->firstChild() != curr)
        curr = listView->firstChild();
    else
        curr = 0;
}

/*
  returns true if the item \a item matches all of the flags set for the iterator
*/
bool Q3ListViewItemIterator::matchesFlags(const Q3ListViewItem *item) const
{
    if (!item)
        return false;

    if (flags == 0)
        return true;

    if (flags & Visible && !item->isVisible())
        return false;
    if (flags & Invisible && item->isVisible())
        return false;
    if (flags & Selected && !item->isSelected())
        return false;
    if (flags & Unselected && item->isSelected())
        return false;
    if (flags & Selectable && !item->isSelectable())
        return false;
    if (flags & NotSelectable && item->isSelectable())
        return false;
    if (flags & DragEnabled && !item->dragEnabled())
        return false;
    if (flags & DragDisabled && item->dragEnabled())
        return false;
    if (flags & DropEnabled && !item->dropEnabled())
        return false;
    if (flags & DropDisabled && item->dropEnabled())
        return false;
    if (flags & Expandable && !item->isExpandable())
        return false;
    if (flags & NotExpandable && item->isExpandable())
        return false;
    if (flags & Checked && !isChecked(item))
        return false;
    if (flags & NotChecked && isChecked(item))
        return false;

    return true;
}

/*
  we want the iterator to check Q3CheckListItems as well, so we provide this convenience function
  that checks if the rtti() is 1 which means Q3CheckListItem and if isOn is true, returns false otherwise.
*/
bool Q3ListViewItemIterator::isChecked(const Q3ListViewItem *item) const
{
    if (item->rtti() == 1)
        return ((const Q3CheckListItem*)item)->isOn();
    else return false;
}

void Q3ListView::handleItemChange(Q3ListViewItem *old, bool shift, bool control)
{
    if (d->selectionMode == Single) {
        // nothing
    } else if (d->selectionMode == Extended) {
        if (shift) {
            selectRange(d->selectAnchor ? d->selectAnchor : old,
                         d->focusItem, false, true, (d->selectAnchor && !control) ? true : false);
        } else if (!control) {
            bool block = signalsBlocked();
            blockSignals(true);
            selectAll(false);
            blockSignals(block);
            setSelected(d->focusItem, true);
        }
    } else if (d->selectionMode == Multi) {
        if (shift)
            selectRange(old, d->focusItem, true, false);
    }
}

void Q3ListView::startRename()
{
    if (!currentItem())
        return;
    currentItem()->startRename(d->pressedColumn);
    d->buttonDown = false;
}

/* unselects items from to, including children, returns true if any items were unselected */
bool Q3ListView::clearRange(Q3ListViewItem *from, Q3ListViewItem *to, bool includeFirst)
{
    if (!from || !to)
        return false;

    // Swap
    if (from->itemPos() > to->itemPos()) {
        Q3ListViewItem *temp = from;
        from = to;
        to = temp;
    }

    // Start on second?
    if (!includeFirst) {
        Q3ListViewItem *below = (from == to) ? from : from->itemBelow();
        if (below)
            from = below;
    }

    // Clear items <from, to>
    bool changed = false;

    Q3ListViewItemIterator it(from);
    while (it.current()) {
        if (it.current()->isSelected()) {
            it.current()->setSelected(false);
            changed = true;
        }
        if (it.current() == to)
            break;
        ++it;
    }

    // NOTE! This function does _not_ emit
    // any signals about selection changed
    return changed;
}

void Q3ListView::selectRange(Q3ListViewItem *from, Q3ListViewItem *to, bool invert, bool includeFirst, bool clearSel)
{
    if (!from || !to)
        return;
    if (from == to && !includeFirst)
        return;
    bool swap = false;
    if (to == from->itemAbove())
        swap = true;
    if (!swap && from != to && from != to->itemAbove()) {
        Q3ListViewItemIterator it(from);
        bool found = false;
        for (; it.current(); ++it) {
            if (it.current() == to) {
                found = true;
                break;
            }
        }
        if (!found)
            swap = true;
    }
    if (swap) {
        Q3ListViewItem *i = from;
        from = to;
        to = i;
        if (!includeFirst)
            to = to->itemAbove();
    } else {
        if (!includeFirst)
            from = from->itemBelow();
    }

    bool changed = false;
    if (clearSel) {
        Q3ListViewItemIterator it(firstChild());
        for (; it.current(); ++it) {
            if (it.current()->selected) {
                it.current()->setSelected(false);
                changed = true;
            }
        }
        it = Q3ListViewItemIterator(to);
        for (; it.current(); ++it) {
            if (it.current()->selected) {
                it.current()->setSelected(false);
                changed = true;
            }
        }
    }

    for (Q3ListViewItem *i = from; i; i = i->itemBelow()) {
        if (!invert) {
            if (!i->selected && i->isSelectable()) {
                i->setSelected(true);
                changed = true;
            }
        } else {
            bool sel = !i->selected;
            if (((bool)i->selected != sel && sel && i->isSelectable()) || !sel) {
                i->setSelected(sel);
                changed = true;
            }
        }
        if (i == to)
            break;
    }
    if (changed) {
        triggerUpdate();
        emit selectionChanged();
    }
}

/* clears selection from anchor to old, selects from anchor to new, does not emit selectionChanged on change */
bool Q3ListView::selectRange(Q3ListViewItem *newItem, Q3ListViewItem *oldItem, Q3ListViewItem *anchorItem)
{
    if (!newItem || !oldItem || !anchorItem)
        return false;

    int  anchorPos = anchorItem ? anchorItem->itemPos() : 0,
         oldPos    = oldItem ? oldItem->itemPos() : 0,
         newPos    = newItem->itemPos();
    Q3ListViewItem *top=0, *bottom=0;
    if (anchorPos > newPos) {
        top = newItem;
        bottom = anchorItem;
    } else {
        top = anchorItem;
        bottom = newItem;
    }

    // removes the subControls of the old selection that will no longer be selected
    bool changed = false;
    int topPos    = top ? top->itemPos() : 0,
        bottomPos = bottom ? bottom->itemPos() : 0;
    if (!(oldPos > topPos && oldPos < bottomPos)) {
        if (oldPos < topPos)
            changed = clearRange(oldItem, top);
        else
            changed = clearRange(bottom, oldItem);
    }

    // selects the new (not already selected) items
    Q3ListViewItemIterator lit(top);
    for (; lit.current(); ++lit) {
        if ((bool)lit.current()->selected != d->select) {
            lit.current()->setSelected(d->select);
            changed = true;
        }
        // Include bottom, then break
        if (lit.current() == bottom)
            break;
    }

    return changed;
}


/*!
    Finds the first list view item in column \a column, that matches
    \a text and returns the item, or returns 0 of no such item could
    be found. Pass OR-ed together \l ComparisonFlags values
    in the \a compare flag, to control how the matching is performed.
    The default comparison mode is case-sensitive, exact match.
*/

Q3ListViewItem *Q3ListView::findItem(const QString& text, int column,
                                    ComparisonFlags compare) const
{
    if (text.isEmpty() && !(compare & ExactMatch))
        return 0;

    if (compare == Qt::CaseSensitive || compare == 0)
        compare |= ExactMatch;

    QString itmtxt;
    QString comtxt = text;
    if (!(compare & Qt::CaseSensitive))
        comtxt = comtxt.toLower();

    Q3ListViewItemIterator it(d->focusItem ? d->focusItem : firstChild());
    Q3ListViewItem *sentinel = 0;
    Q3ListViewItem *item;
    Q3ListViewItem *beginsWithItem = 0;
    Q3ListViewItem *endsWithItem = 0;
    Q3ListViewItem *containsItem = 0;

    for (int pass = 0; pass < 2; pass++) {
        while ((item = it.current()) != sentinel) {
            itmtxt = item->text(column);
            if (!(compare & CaseSensitive))
                itmtxt = itmtxt.toLower();

            if ((compare & ExactMatch)==ExactMatch && itmtxt == comtxt)
                return item;
            if (compare & BeginsWith && !beginsWithItem && itmtxt.startsWith(comtxt))
                beginsWithItem = containsItem = item;
            if (compare & EndsWith && !endsWithItem && itmtxt.endsWith(comtxt))
                endsWithItem = containsItem = item;
            if ((compare & ExactMatch)==0 && !containsItem && itmtxt.contains(comtxt))
                containsItem = item;
            ++it;
        }

        it = Q3ListViewItemIterator(firstChild());
        sentinel = d->focusItem ? d->focusItem : firstChild();
    }

    // Obey the priorities
    if (beginsWithItem)
        return beginsWithItem;
    else if (endsWithItem)
        return endsWithItem;
    else if (containsItem)
        return containsItem;
    return 0;
}

/*!
    Hides the column specified at \a column. This is a convenience
    function that calls setColumnWidth(column, 0).

    Note: The user may still be able to resize the hidden column using
    the header handles. To prevent this, call setResizeEnabled(false,
    \a column) on the list views header.

    \sa setColumnWidth()
*/

void Q3ListView::hideColumn(int column)
{
    setColumnWidth(column, 0);
}

/*! Adjusts the column \a col to its preferred width */

void Q3ListView::adjustColumn(int col)
{
    if (col < 0 || col > (int)d->column.count() - 1 || d->h->isStretchEnabled(col))
        return;

    int oldw = d->h->sectionSize(col);

    int w = d->h->sectionSizeHint(col, fontMetrics()).width();
    if (d->h->iconSet(col))
        w += d->h->iconSet(col)->pixmap().width();
    w = qMax(w, 20);
    QFontMetrics fm(fontMetrics());
    Q3ListViewItem* item = firstChild();
    int rootDepth = rootIsDecorated() ? treeStepSize() : 0;
    while (item) {
        int iw = item->width(fm, this, col);
        if (0 == col)
            iw += itemMargin() + rootDepth + item->depth()*treeStepSize() - 1;
        w = qMax(w, iw);
        item = item->itemBelow();
    }
    w = qMax(w, QApplication::globalStrut().width());

    d->h->adjustHeaderSize(oldw - w);
    if (oldw != w) {
        d->fullRepaintOnComlumnChange = true;
        d->h->resizeSection(col, w);
        emit d->h->sizeChange(col, oldw, w);
    }
}

/*!
    \enum Q3ListView::StringComparisonMode

    This enum type is used to set the string comparison mode when
    searching for an item. We'll refer to the string being searched
    as the 'target' string.

    \value CaseSensitive The strings must match case sensitively.
    \value ExactMatch The target and search strings must match exactly.
    \value BeginsWith The target string begins with the search string.
    \value EndsWith The target string ends with the search string.
    \value Contains The target string contains the search string.

    If you OR these flags together (excluding \c CaseSensitive), the
    search criteria be applied in the following order: \c ExactMatch,
    \c BeginsWith, \c EndsWith, \c Contains.

    Matching is case-insensitive unless \c CaseSensitive is set. \c
    CaseSensitive can be OR-ed with any combination of the other
    flags.

    \sa ComparisonFlags
*/

/*!
    \typedef Q3ListView::ComparisonFlags

    This typedef is used in Q3ListView's API for values that are OR'd
    combinations of \l StringComparisonMode values.

    \sa StringComparisonMode
*/

QT_END_NAMESPACE

#endif // QT_NO_LISTVIEW
