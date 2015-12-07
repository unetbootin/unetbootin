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

#include "qglobal.h"
#if defined(Q_CC_BOR)
// needed for qsort() because of a std namespace problem on Borland
#include "qplatformdefs.h"
#endif

#include "q3listbox.h"
#ifndef QT_NO_LISTBOX
#include "qapplication.h"
#include "qevent.h"
#include "qfontmetrics.h"
#include "qpainter.h"
#include "qpixmap.h"
#include "qstringlist.h"
#include "qstyle.h"
#include "qstyleoption.h"
#include "qtimer.h"
#include "qvector.h"
#include "qpointer.h"
#ifndef QT_NO_ACCESSIBILITY
#include "qaccessible.h"
#endif

#include <stdlib.h>

QT_BEGIN_NAMESPACE

class Q3ListBoxPrivate
{
public:
    Q3ListBoxPrivate(Q3ListBox *lb):
        head(0), last(0), cache(0), cacheIndex(-1), current(0),
        highlighted(0), tmpCurrent(0), columnPos(1), rowPos(1), rowPosCache(0),
        columnPosOne(0), rowMode(Q3ListBox::FixedNumber),
        columnMode(Q3ListBox::FixedNumber), numRows(1), numColumns(1),
        currentRow(0), currentColumn(0),
        mousePressRow(-1), mousePressColumn(-1),
        mouseMoveRow(-1), mouseMoveColumn(-1), mouseInternalPress(false),
        scrollTimer(0), updateTimer(0), visibleTimer(0),
        selectionMode(Q3ListBox::Single),
        count(0),
        listBox(lb), currInputString(QString()),
        rowModeWins(false),
        ignoreMoves(false),
        layoutDirty(true),
        mustPaintAll(true),
        dragging(false),
        dirtyDrag (false),
        variableHeight(true /* !!! ### false */),
        variableWidth(false),
        inMenuMode(false)
    {}
    int findItemByName(int item, const QString &text);
    ~Q3ListBoxPrivate();

    Q3ListBoxItem * head, *last, *cache;
    int cacheIndex;
    Q3ListBoxItem * current, *highlighted, *tmpCurrent;

    QVector<int> columnPos;
    QVector<int> rowPos;
    int rowPosCache;
    int columnPosOne;

    Q3ListBox::LayoutMode rowMode;
    Q3ListBox::LayoutMode columnMode;
    int numRows;
    int numColumns;

    int currentRow;
    int currentColumn;
    int mousePressRow;
    int mousePressColumn;
    int mouseMoveRow;
    int mouseMoveColumn;
    bool mouseInternalPress;

    QTimer * scrollTimer;
    QTimer * updateTimer;
    QTimer * visibleTimer;
    QTimer * resizeTimer;

    QPoint scrollPos;

    Q3ListBox::SelectionMode selectionMode;

    int count;


    Q3ListBox *listBox;
    QString currInputString;
    QTimer *inputTimer;

    Q3ListBoxItem *pressedItem, *selectAnchor;

    uint select :1;
    uint pressedSelected :1;
    uint rowModeWins :1;
    uint ignoreMoves :1;
    uint clearing :1;
    uint layoutDirty :1;
    uint mustPaintAll :1;
    uint dragging :1;
    uint dirtyDrag :1;
    uint variableHeight :1;
    uint variableWidth :1;
    uint inMenuMode :1;

    QRect *rubber;

    struct SortableItem {
        Q3ListBoxItem *item;
    };
};


Q3ListBoxPrivate::~Q3ListBoxPrivate()
{
    Q_ASSERT(!head);
}


/*!
    \class Q3ListBoxItem qlistbox.h
    \brief The Q3ListBoxItem class is the base class of all list box items.

    \compat

    This class is an abstract base class used for all list box items.
    If you need to insert customized items into a Q3ListBox you must
    inherit this class and reimplement paint(), height() and width().

    \sa Q3ListBox
*/

/*!
    Constructs an empty list box item in the list box \a listbox.
*/

Q3ListBoxItem::Q3ListBoxItem(Q3ListBox* listbox)
{
    lbox = listbox;
    s = false;
    dirty = true;
    custom_highlight = false;
    selectable = true;
    p = n = 0;

    if (listbox)
        listbox->insertItem(this);
}

/*!
    Constructs an empty list box item in the list box \a listbox and
    inserts it after the item \a after or at the beginning if \a after
    is 0.
*/

Q3ListBoxItem::Q3ListBoxItem(Q3ListBox* listbox, Q3ListBoxItem *after)
{
    lbox = listbox;
    s = false;
    dirty = true;
    custom_highlight = false;
    selectable = true;
    p = n = 0;

    if (listbox)
        listbox->insertItem(this, after);
}


/*!
    Destroys the list box item.
*/

Q3ListBoxItem::~Q3ListBoxItem()
{
    if (lbox)
        lbox->takeItem(this);
}


/*!
    Defines whether the list box item is responsible for drawing
    itself in a highlighted state when being selected.

    If \a b is false (the default), the list box will draw some
    default highlight indicator before calling paint().

    \sa isSelected(), paint()
*/
void Q3ListBoxItem::setCustomHighlighting(bool b)
{
    custom_highlight = b;
}

/*!
    \fn void Q3ListBoxItem::paint(QPainter *p)

    Implement this function to draw your item. The painter, \a p, is
    already open for painting.

    \sa height(), width()
*/

/*!
    \fn int Q3ListBoxItem::width(const Q3ListBox* lb) const

    Reimplement this function to return the width of your item. The \a
    lb parameter is the same as listBox() and is provided for
    convenience and compatibility.

    The default implementation returns
    \l{QApplication::globalStrut()}'s width.

    \sa paint(), height()
*/
int Q3ListBoxItem::width(const Q3ListBox*)  const
{
    return QApplication::globalStrut().width();
}

/*!
    \fn int Q3ListBoxItem::height(const Q3ListBox* lb) const

    Implement this function to return the height of your item. The \a
    lb parameter is the same as listBox() and is provided for
    convenience and compatibility.

    The default implementation returns
    \l{QApplication::globalStrut()}'s height.

    \sa paint(), width()
*/
int Q3ListBoxItem::height(const Q3ListBox*)  const
{
    return QApplication::globalStrut().height();
}


/*!
    Returns the text of the item. This text is also used for sorting.

    \sa setText()
*/
QString Q3ListBoxItem::text() const
{
    return txt;
}

/*!
    Returns the pixmap associated with the item, or 0 if there isn't
    one.

    The default implementation returns 0.
*/
const QPixmap *Q3ListBoxItem::pixmap() const
{
    return 0;
}

/*! \fn void Q3ListBoxItem::setSelectable(bool b)

    If \a b is true (the default) then this item can be selected by
    the user; otherwise this item cannot be selected by the user.

    \sa isSelectable()
*/

/*! \fn bool Q3ListBoxItem::isSelectable() const

    Returns true if this item is selectable (the default); otherwise
    returns false.

    \sa setSelectable()
*/


/*!
    \fn void Q3ListBoxItem::setText(const QString &text)

    Sets the text of the Q3ListBoxItem to \a text. This \a text is also
    used for sorting. The text is not shown unless explicitly drawn in
    paint().

    \sa text()
*/


/*!
    \class Q3ListBoxText qlistbox.h
    \brief The Q3ListBoxText class provides list box items that display text.

    \compat

    The text is drawn in the widget's current font. If you need
    several different fonts, you must implement your own subclass of
    Q3ListBoxItem.

    \sa Q3ListBox, Q3ListBoxItem
*/


/*!
    Constructs a list box item in list box \a listbox showing the text
    \a text.
*/
Q3ListBoxText::Q3ListBoxText(Q3ListBox *listbox, const QString &text)
    :Q3ListBoxItem(listbox)
{
    setText(text);
}

/*!
    Constructs a list box item showing the text \a text.
*/

Q3ListBoxText::Q3ListBoxText(const QString &text)
    :Q3ListBoxItem()
{
    setText(text);
}

/*!
    Constructs a list box item in list box \a listbox showing the text
    \a text. The item is inserted after the item \a after, or at the
    beginning if \a after is 0.
*/

Q3ListBoxText::Q3ListBoxText(Q3ListBox* listbox, const QString &text, Q3ListBoxItem *after)
    : Q3ListBoxItem(listbox, after)
{
    setText(text);
}

/*!
    Destroys the item.
*/

Q3ListBoxText::~Q3ListBoxText()
{
}

/*!
    Draws the text using \a painter.
*/

void Q3ListBoxText::paint(QPainter *painter)
{
    int itemHeight = height(listBox());
    QFontMetrics fm = painter->fontMetrics();
    int yPos = ((itemHeight - fm.height()) / 2) + fm.ascent();
    painter->drawText(3, yPos, text());
}

/*!
    Returns the height of a line of text in list box \a lb.

    \sa paint(), width()
*/

int Q3ListBoxText::height(const Q3ListBox* lb) const
{
    int h = lb ? lb->fontMetrics().lineSpacing() + 2 : 0;
    return qMax(h, QApplication::globalStrut().height());
}

/*!
    Returns the width of this line in list box \a lb.

    \sa paint(), height()
*/

int Q3ListBoxText::width(const Q3ListBox* lb) const
{
    int w = lb ? lb->fontMetrics().width(text()) + 6 : 0;
    return qMax(w, QApplication::globalStrut().width());
}

/*!
    \fn int Q3ListBoxText::rtti() const

    \reimp

    Returns 1.

    Make your derived classes return their own values for rtti(), and
    you can distinguish between listbox items. You should use values
    greater than 1000 preferably a large random number, to allow for
    extensions to this class.
*/

int Q3ListBoxText::rtti() const
{
    return RTTI;
}

/*!
    \class Q3ListBoxPixmap qlistbox.h
    \brief The Q3ListBoxPixmap class provides list box items with a
    pixmap and optional text.

    \compat

    Items of this class are drawn with the pixmap on the left with the
    optional text to the right of the pixmap.

    \sa Q3ListBox, Q3ListBoxItem
*/


/*!
    Constructs a new list box item in list box \a listbox showing the
    pixmap \a pixmap.
*/

Q3ListBoxPixmap::Q3ListBoxPixmap(Q3ListBox* listbox, const QPixmap &pixmap)
    : Q3ListBoxItem(listbox)
{
    pm = pixmap;
}

/*!
    Constructs a new list box item showing the pixmap \a pixmap.
*/

Q3ListBoxPixmap::Q3ListBoxPixmap(const QPixmap &pixmap)
    : Q3ListBoxItem()
{
    pm = pixmap;
}

/*!
    Constructs a new list box item in list box \a listbox showing the
    pixmap \a pixmap. The item gets inserted after the item \a after,
    or at the beginning if \a after is 0.
*/

Q3ListBoxPixmap::Q3ListBoxPixmap(Q3ListBox* listbox, const QPixmap &pixmap, Q3ListBoxItem *after)
    : Q3ListBoxItem(listbox, after)
{
    pm = pixmap;
}


/*!
    Destroys the item.
*/

Q3ListBoxPixmap::~Q3ListBoxPixmap()
{
}


/*!
    Constructs a new list box item in list box \a listbox showing the
    pixmap \a pix and the text \a text.
*/
Q3ListBoxPixmap::Q3ListBoxPixmap(Q3ListBox* listbox, const QPixmap &pix, const QString& text)
    : Q3ListBoxItem(listbox)
{
    pm = pix;
    setText(text);
}

/*!
    Constructs a new list box item showing the pixmap \a pix and the
    text to \a text.
*/
Q3ListBoxPixmap::Q3ListBoxPixmap(const QPixmap & pix, const QString& text)
    : Q3ListBoxItem()
{
    pm = pix;
    setText(text);
}

/*!
    Constructs a new list box item in list box \a listbox showing the
    pixmap \a pix and the string \a text. The item gets inserted after
    the item \a after, or at the beginning if \a after is 0.
*/
Q3ListBoxPixmap::Q3ListBoxPixmap(Q3ListBox* listbox, const QPixmap & pix, const QString& text,
                                Q3ListBoxItem *after)
    : Q3ListBoxItem(listbox, after)
{
    pm = pix;
    setText(text);
}

/*!
    \fn const QPixmap *Q3ListBoxPixmap::pixmap() const

    Returns the pixmap associated with the item.
*/


/*!
    Draws the pixmap using \a painter.
*/

void Q3ListBoxPixmap::paint(QPainter *painter)
{
    int itemHeight = height(listBox());
    int yPos;

    const QPixmap *pm = pixmap();
    if (pm && ! pm->isNull()) {
        yPos = (itemHeight - pm->height()) / 2;
        painter->drawPixmap(3, yPos, *pm);
    }

    if (!text().isEmpty()) {
        QFontMetrics fm = painter->fontMetrics();
        yPos = ((itemHeight - fm.height()) / 2) + fm.ascent();
        painter->drawText(pm->width() + 5, yPos, text());
    }
}

/*!
    Returns the height of the pixmap in list box \a lb.

    \sa paint(), width()
*/

int Q3ListBoxPixmap::height(const Q3ListBox* lb) const
{
    int h;
    if (text().isEmpty())
        h = pm.height();
    else
        h = qMax(pm.height(), lb->fontMetrics().lineSpacing() + 2);
    return qMax(h, QApplication::globalStrut().height());
}

/*!
    Returns the width of the pixmap plus some margin in list box \a lb.

    \sa paint(), height()
*/

int Q3ListBoxPixmap::width(const Q3ListBox* lb) const
{
    if (text().isEmpty())
        return qMax(pm.width() + 6, QApplication::globalStrut().width());
    return qMax(pm.width() + lb->fontMetrics().width(text()) + 6,
            QApplication::globalStrut().width());
}

/*!
    \fn int Q3ListBoxPixmap::rtti() const

    \reimp

    Returns 2.

    Make your derived classes return their own values for rtti(), and
    you can distinguish between listbox items. You should use values
    greater than 1000 preferably a large random number, to allow for
    extensions to this class.
*/

int Q3ListBoxPixmap::rtti() const
{
    return RTTI;
}

/*!
    \class Q3ListBox qlistbox.h
    \brief The Q3ListBox widget provides a list of selectable, read-only items.

    \compat

    This is typically a single-column list in which either no item or
    one item is selected, but it can also be used in many other ways.

    Q3ListBox will add scroll bars as necessary, but it isn't intended
    for \e really big lists. If you want more than a few thousand
    items, it's probably better to use a different widget mainly
    because the scroll bars won't provide very good navigation, but
    also because Q3ListBox may become slow with huge lists. (See
    Q3ListView and Q3Table for possible alternatives.)

    There are a variety of selection modes described in the
    Q3ListBox::SelectionMode documentation. The default is \l Single
    selection mode, but you can change it using setSelectionMode().
    (setMultiSelection() is still provided for compatibility with Qt
    1.x. We recommend using setSelectionMode() in all code.)

    Because Q3ListBox offers multiple selection it must display
    keyboard focus and selection state separately. Therefore there are
    functions both to set the selection state of an item, i.e.
    setSelected(), and to set which item displays keyboard focus, i.e.
    setCurrentItem().

    The list box normally arranges its items in a single column and
    adds a vertical scroll bar if required. It is possible to have a
    different fixed number of columns (setColumnMode()), or as many
    columns as will fit in the list box's assigned screen space
    (setColumnMode(FitToWidth)), or to have a fixed number of rows
    (setRowMode()) or as many rows as will fit in the list box's
    assigned screen space (setRowMode(FitToHeight)). In all these
    cases Q3ListBox will add scroll bars, as appropriate, in at least
    one direction.

    If multiple rows are used, each row can be as high as necessary
    (the normal setting), or you can request that all items will have
    the same height by calling setVariableHeight(false). The same
    applies to a column's width, see setVariableWidth().

    The Q3ListBox's items are Q3ListBoxItem objects. Q3ListBox provides
    methods to insert new items as strings, as pixmaps, and as
    Q3ListBoxItem * (insertItem() with various arguments), and to
    replace an existing item with a new string, pixmap or Q3ListBoxItem
    (changeItem() with various arguments). You can also remove items
    singly with removeItem() or clear() the entire list box. Note that
    if you create a Q3ListBoxItem yourself and insert it, Q3ListBox
    takes ownership of the item.

    You can also create a Q3ListBoxItem, such as Q3ListBoxText or
    Q3ListBoxPixmap, with the list box as first parameter. The item
    will then append itself. When you delete an item it is
    automatically removed from the list box.

    The list of items can be arbitrarily large; Q3ListBox will add
    scroll bars if necessary. Q3ListBox can display a single-column
    (the common case) or multiple-columns, and offers both single and
    multiple selection. Q3ListBox does not support multiple-column
    items (but Q3ListView and Q3Table do), or tree hierarchies (but
    Q3ListView does).

    The list box items can be accessed both as Q3ListBoxItem objects
    (recommended) and using integer indexes (the original Q3ListBox
    implementation used an array of strings internally, and the API
    still supports this mode of operation). Everything can be done
    using the new objects, and most things can be done using indexes.

    Each item in a Q3ListBox contains a Q3ListBoxItem. One of the items
    can be the current item. The currentChanged() signal and the
    highlighted() signal are emitted when a new item becomes current,
    e.g. because the user clicks on it or Q3ListBox::setCurrentItem()
    is called. The selected() signal is emitted when the user
    double-clicks on an item or presses Enter on the current item.

    If the user does not select anything, no signals are emitted and
    currentItem() returns -1.

    A list box has Qt::WheelFocus as a default focusPolicy(), i.e. it
    can get keyboard focus by tabbing, clicking and through the use of
    the mouse wheel.

    New items can be inserted using insertItem(), insertStrList() or
    insertStringList().

    By default, vertical and horizontal scroll bars are added and
    removed as necessary. setHScrollBarMode() and setVScrollBarMode()
    can be used to change this policy.

    If you need to insert types other than strings and pixmaps, you
    must define new classes which inherit Q3ListBoxItem.

    \warning The list box assumes ownership of all list box items and
    will delete them when it does not need them any more.

    \inlineimage qlistbox-m.png Screenshot in Motif style
    \inlineimage qlistbox-w.png Screenshot in Windows style

    \sa Q3ListView, QComboBox, QButtonGroup
*/

/*!
    \enum Q3ListBox::SelectionMode

    This enumerated type is used by Q3ListBox to indicate how it reacts
    to selection by the user.

    \value Single  When the user selects an item, any already-selected
    item becomes unselected and the user cannot unselect the selected
    item. This means that the user can never clear the selection, even
    though the selection may be cleared by the application programmer
    using Q3ListBox::clearSelection().

    \value Multi  When the user selects an item the selection status
    of that item is toggled and the other items are left alone.

    \value Extended When the user selects an item the selection is
    cleared and the new item selected. However, if the user presses
    the Ctrl key when clicking on an item, the clicked item gets
    toggled and all other items are left untouched. And if the user
    presses the Shift key while clicking on an item, all items between
    the current item and the clicked item get selected or unselected,
    depending on the state of the clicked item. Also, multiple items
    can be selected by dragging the mouse while the left mouse button
    is kept pressed.

    \value NoSelection  Items cannot be selected.

    In other words, \c Single is a real single-selection list box, \c
    Multi is a real multi-selection list box, \c Extended is a list
    box in which users can select multiple items but usually want to
    select either just one or a range of contiguous items, and \c
    NoSelection is for a list box where the user can look but not
    touch.
*/


/*!
    \enum Q3ListBox::LayoutMode

    This enum type is used to specify how Q3ListBox lays out its rows
    and columns.

    \value FixedNumber  There is a fixed number of rows (or columns).

    \value FitToWidth   There are as many columns as will fit
    on-screen.

    \value FitToHeight  There are as many rows as will fit on-screen.

    \value Variable  There are as many rows as are required by the
    column mode. (Or as many columns as required by the row mode.)

    Example: When you call setRowMode(FitToHeight), columnMode()
    automatically becomes \c Variable to accommodate the row mode
    you've set.
*/

/*!
    \fn void  Q3ListBox::onItem(Q3ListBoxItem *i)

    This signal is emitted when the user moves the mouse cursor onto
    an item, similar to the QWidget::enterEvent() function. \a i is
    the Q3ListBoxItem that the mouse has moved on.
*/

// ### bug here too? enter/leave event may noit considered. move the
// mouse out of the window and back in, to the same item - does it
// work?

/*!
    \fn void  Q3ListBox::onViewport()

    This signal is emitted when the user moves the mouse cursor from
    an item to an empty part of the list box.
*/


/*!
    Constructs a new empty list box called \a name and with parent \a
    parent and widget attributes \a f.

    This constructor sets the Qt::WA_StaticContent and the
    Qt::WA_NoBackground attributes to boost performance when drawing
    Q3ListBoxItems. This may be unsuitable for custom Q3ListBoxItem
    classes, in which case Qt::WA_StaticContents and Qt::WA_NoBackground
    should be cleared on the viewport() after construction.
*/

Q3ListBox::Q3ListBox(QWidget *parent, const char *name, Qt::WindowFlags f)
    : Q3ScrollView(parent, name, f | Qt::WStaticContents | Qt::WNoAutoErase)
{
    d = new Q3ListBoxPrivate(this);
    d->updateTimer = new QTimer(this, "listbox update timer");
    d->visibleTimer = new QTimer(this, "listbox visible timer");
    d->inputTimer = new QTimer(this, "listbox input timer");
    d->resizeTimer = new QTimer(this, "listbox resize timer");
    d->clearing = false;
    d->pressedItem = 0;
    d->selectAnchor = 0;
    d->select = false;
    d->rubber = 0;

    setMouseTracking(true);
    viewport()->setMouseTracking(true);

    connect(d->updateTimer, SIGNAL(timeout()),
             this, SLOT(refreshSlot()));
    connect(d->visibleTimer, SIGNAL(timeout()),
             this, SLOT(ensureCurrentVisible()));
    connect(d->resizeTimer, SIGNAL(timeout()),
             this, SLOT(adjustItems()));
    viewport()->setBackgroundRole(QPalette::Base);
    viewport()->setFocusProxy(this);
    viewport()->setFocusPolicy(Qt::WheelFocus);
    setFocusPolicy(Qt::WheelFocus);
    setAttribute(Qt::WA_MacShowFocusRect);
}


Q3ListBox * Q3ListBox::changedListBox = 0;

/*!
    Destroys the list box. Deletes all list box items.
*/

Q3ListBox::~Q3ListBox()
{
    if (changedListBox == this)
        changedListBox = 0;
    clear();
    delete d;
    d = 0;
}

/*!
    \fn void Q3ListBox::pressed(Q3ListBoxItem *item)

    This signal is emitted when the user presses any mouse button. If
    \a item is not 0, the cursor is on \a item. If \a item is 0, the
    mouse cursor isn't on any item.

    Note that you must not delete any Q3ListBoxItem objects in slots
    connected to this signal.
*/

/*!
    \fn void Q3ListBox::pressed(Q3ListBoxItem *item, const QPoint &pnt)
    \overload

    This signal is emitted when the user presses any mouse button. If
    \a item is not 0, the cursor is on \a item. If \a item is 0, the
    mouse cursor isn't on any item.

    \a pnt is the position of the mouse cursor in the global
    coordinate system (QMouseEvent::globalPos()).

    Note that you must not delete any Q3ListBoxItem objects in slots
    connected to this signal.

    \sa mouseButtonPressed() rightButtonPressed() clicked()
*/

/*!
    \fn void Q3ListBox::clicked(Q3ListBoxItem *item)

    This signal is emitted when the user clicks any mouse button. If
    \a item is not 0, the cursor is on \a item. If \a item is 0, the
    mouse cursor isn't on any item.

    Note that you must not delete any Q3ListBoxItem objects in slots
    connected to this signal.
*/

/*!
    \fn void Q3ListBox::clicked(Q3ListBoxItem *item, const QPoint &pnt)
    \overload

    This signal is emitted when the user clicks any mouse button. If
    \a item is not 0, the cursor is on \a item. If \a item is 0, the
    mouse cursor isn't on any item.

    \a pnt is the position of the mouse cursor in the global
    coordinate system (QMouseEvent::globalPos()). (If the click's
    press and release differs by a pixel or two, \a pnt is the
    position at release time.)

    Note that you must not delete any Q3ListBoxItem objects in slots
    connected to this signal.
*/

/*!
    \fn void Q3ListBox::mouseButtonClicked (int button, Q3ListBoxItem * item, const QPoint & pos)

    This signal is emitted when the user clicks mouse button \a
    button. If \a item is not 0, the cursor is on \a item. If \a item
    is 0, the mouse cursor isn't on any item.

    \a pos is the position of the mouse cursor in the global
    coordinate system (QMouseEvent::globalPos()). (If the click's
    press and release differs by a pixel or two, \a pos is the
    position at release time.)

    Note that you must not delete any Q3ListBoxItem objects in slots
    connected to this signal.
*/

/*!
    \fn void Q3ListBox::mouseButtonPressed (int button, Q3ListBoxItem * item, const QPoint & pos)

    This signal is emitted when the user presses mouse button \a
    button. If \a item is not 0, the cursor is on \a item. If \a item
    is 0, the mouse cursor isn't on any item.

    \a pos is the position of the mouse cursor in the global
    coordinate system (QMouseEvent::globalPos()).

    Note that you must not delete any Q3ListBoxItem objects in slots
    connected to this signal.
*/

/*!
    \fn void Q3ListBox::doubleClicked(Q3ListBoxItem *item)

    This signal is emitted whenever an item is double-clicked. It's
    emitted on the second button press, not the second button release.
    If \a item is not 0, the cursor is on \a item. If \a item is 0,
    the mouse cursor isn't on any item.
*/


/*!
    \fn void Q3ListBox::returnPressed(Q3ListBoxItem *item)

    This signal is emitted when Enter or Return is pressed. The
    \a item passed in the argument is currentItem().
*/

/*!
    \fn void Q3ListBox::rightButtonClicked(Q3ListBoxItem *item, const QPoint& point)

    This signal is emitted when the right button is clicked. The \a
    item is the item that the button was clicked on (which could be
    0 if no item was clicked on), and the \a point is where the
    click took place in global coordinates.
*/


/*!
    \fn void Q3ListBox::rightButtonPressed (Q3ListBoxItem *item, const QPoint &point)

    This signal is emitted when the right button is pressed. The \a
    item is the item that the button was pressed over (which could be
    0 if no item was pressed over), and the \a point is where the
    press took place in global coordinates.
*/

/*!
    \fn void Q3ListBox::contextMenuRequested(Q3ListBoxItem *item, const QPoint & pos)

    This signal is emitted when the user invokes a context menu with
    the right mouse button or with special system keys, with \a item
    being the item under the mouse cursor or the current item,
    respectively.

    \a pos is the position for the context menu in the global
    coordinate system.
*/

/*!
    \fn void Q3ListBox::selectionChanged()

    This signal is emitted when the selection set of a list box
    changes. This signal is emitted in each selection mode. If the
    user selects five items by drag-selecting, Q3ListBox tries to emit
    just one selectionChanged() signal so the signal can be connected
    to computationally expensive slots.

    \sa selected() currentItem()
*/

/*!
    \fn void Q3ListBox::selectionChanged(Q3ListBoxItem *item)
    \overload

    This signal is emitted when the selection in a \l Single selection
    list box changes. \a item is the newly selected list box item.

    \sa selected() currentItem()
*/

/*!
    \fn void Q3ListBox::currentChanged(Q3ListBoxItem *item)

    This signal is emitted when the user makes a new item the current
    item. \a item is the new current list box item.

    \sa setCurrentItem() currentItem()
*/

/*!
    \fn void Q3ListBox::highlighted(int index)

    This signal is emitted when the user makes a new item the current
    item. \a index is the index of the new current item.

    \sa currentChanged() selected() currentItem() selectionChanged()
*/

/*!
    \fn void Q3ListBox::highlighted(Q3ListBoxItem *item)

    \overload

    This signal is emitted when the user makes a new \a item the current
    \a item.

    \sa currentChanged() selected() currentItem() selectionChanged()
*/

/*!
    \fn void Q3ListBox::highlighted(const QString & text)

    \overload

    This signal is emitted when the user makes a new item the current
    item and the item is (or has) as string. The argument is the new
    current item's \a text.

    \sa currentChanged() selected() currentItem() selectionChanged()
*/

/*!
    \fn void Q3ListBox::selected(int index)

    This signal is emitted when the user double-clicks on an item or
    presses Enter on the current item. \a index is the index of the
    selected item.

    \sa currentChanged() highlighted() selectionChanged()
*/

/*!
    \fn void Q3ListBox::selected(Q3ListBoxItem *item)

    \overload

    This signal is emitted when the user double-clicks on an \a item or
    presses Enter on the current \a item.

    \sa currentChanged() highlighted() selectionChanged()
*/

/*!
    \fn void Q3ListBox::selected(const QString &text)

    \overload

    This signal is emitted when the user double-clicks on an item or
    presses Enter on the current item, and the item is (or has) a
    string. The argument is the \a text of the selected item.

    \sa currentChanged() highlighted() selectionChanged()
*/

/*!
    \property Q3ListBox::count
    \brief the number of items in the list box
*/

uint Q3ListBox::count() const
{
    return d->count;
}

#if 0
/*!
    Inserts the string list \a list into the list at position \a
    index.

    If \a index is negative, \a list is inserted at the end of the
    list. If \a index is too large, the operation is ignored.

    \warning This function uses \c{const char *} rather than QString,
    so we recommend against using it. It is provided so that legacy
    code will continue to work, and so that programs that certainly
    will not need to handle code outside a single 8-bit locale can use
    it. See insertStringList() which uses real QStrings.

    \warning This function is never significantly faster than a loop
    around insertItem().

    \sa insertItem(), insertStringList()
*/

void Q3ListBox::insertStrList(const QStrList *list, int index)
{
    if (!list) {
        Q_ASSERT(list != 0);
        return;
    }
    insertStrList(*list, index);
}
#endif


/*!
    Inserts the string list \a list into the list at position \a
    index.

    If \a index is negative, \a list is inserted at the end of the
    list. If \a index is too large, the operation is ignored.

    \warning This function is never significantly faster than a loop
    around insertItem().

    \sa insertItem(), insertStrList()
*/

void Q3ListBox::insertStringList(const QStringList & list, int index)
{
    if (index < 0)
        index = count();
    for (QStringList::ConstIterator it = list.begin(); it != list.end(); ++it)
        insertItem(new Q3ListBoxText(*it), index++);
}


#if 0
/*!
    \overload

    Inserts the string list \a list into the list at position \a
    index.

    If \a index is negative, \a list is inserted at the end of the
    list. If \a index is too large, the operation is ignored.

    \warning This function uses \c{const char *} rather than QString,
    so we recommend against using it. It is provided so that legacy
    code will continue to work, and so that programs that certainly
    will not need to handle code outside a single 8-bit locale can use
    it. See insertStringList() which uses real QStrings.

    \warning This function is never significantly faster than a loop
    around insertItem().

    \sa insertItem(), insertStringList()
*/
void Q3ListBox::insertStrList(const QStrList & list, int index)
{
    QStrListIterator it(list);
    const char* txt;
    if (index < 0)
        index = count();
    while ((txt=it.current())) {
        ++it;
        insertItem(new Q3ListBoxText(QString::fromLatin1(txt)),
                    index++);
    }
    if (hasFocus() && !d->current)
        setCurrentItem(d->head);
}
#endif


/*!
    Inserts the \a numStrings strings of the array \a strings into the
    list at position \a index.

    If \a index is negative, insertStrList() inserts \a strings at the
    end of the list. If \a index is too large, the operation is
    ignored.

    \warning This function uses \c{const char *} rather than QString,
    so we recommend against using it. It is provided so that legacy
    code will continue to work, and so that programs that certainly
    will not need to handle code outside a single 8-bit locale can use
    it. See insertStringList() which uses real QStrings.

    \warning This function is never significantly faster than a loop
    around insertItem().

    \sa insertItem(), insertStringList()
*/

void Q3ListBox::insertStrList(const char **strings, int numStrings, int index)
{
    if (!strings) {
        Q_ASSERT(strings != 0);
        return;
    }
    if (index < 0)
        index = count();
    int i = 0;
    while ((numStrings<0 && strings[i]!=0) || i<numStrings) {
        insertItem(new Q3ListBoxText(QString::fromLatin1(strings[i])),
                    index + i);
        i++;
    }
    if (hasFocus() && !d->current)
        setCurrentItem(d->head);
}

/*!
    Inserts the item \a lbi into the list at position \a index.

    If \a index is negative or larger than the number of items in the
    list box, \a lbi is inserted at the end of the list.

    \sa insertStrList()
*/

void Q3ListBox::insertItem(const Q3ListBoxItem *lbi, int index)
{
    if (!lbi)
        return;

    if (index < 0)
        index = d->count;

    if (index >= d->count) {
        insertItem(lbi, d->last);
        return;
    }

    Q3ListBoxItem * item = (Q3ListBoxItem *)lbi;
    d->count++;
    d->cache = 0;

    item->lbox = this;
    if (!d->head || index == 0) {
        item->n = d->head;
        item->p = 0;
        d->head = item;
        item->dirty = true;
        if (item->n)
            item->n->p = item;
    } else {
        Q3ListBoxItem * i = d->head;
        while (i->n && index > 1) {
            i = i->n;
            index--;
        }
        if (i->n) {
            item->n = i->n;
            item->p = i;
            item->n->p = item;
            item->p->n = item;
        } else {
            i->n = item;
            item->p = i;
            item->n = 0;
        }
    }

    if (hasFocus() && !d->current) {
        d->current = d->head;
        updateItem(d->current);
        emit highlighted(d->current);
        emit highlighted(d->current->text());
        emit highlighted(index);
    }

    triggerUpdate(true);
}

/*!
    \overload

    Inserts the item \a lbi into the list after the item \a after, or
    at the beginning if \a after is 0.

    \sa insertStrList()
*/

void Q3ListBox::insertItem(const Q3ListBoxItem *lbi, const Q3ListBoxItem *after)
{
    if (!lbi)
        return;

    Q3ListBoxItem * item = (Q3ListBoxItem*)lbi;
    d->count++;
    d->cache = 0;

    item->lbox = this;
    if (!d->head || !after) {
        item->n = d->head;
        item->p = 0;
        d->head = item;
        item->dirty = true;
        if (item->n)
            item->n->p = item;
    } else {
        Q3ListBoxItem * i = (Q3ListBoxItem*) after;
        if (i) {
            item->n = i->n;
            item->p = i;
            if (item->n)
                item->n->p = item;
            if (item->p)
                item->p->n = item;
        }
    }

    if (after == d->last)
        d->last = (Q3ListBoxItem*) lbi;

    if (hasFocus() && !d->current) {
        d->current = d->head;
        updateItem(d->current);
        emit highlighted(d->current);
        emit highlighted(d->current->text());
        emit highlighted(index(d->current));
    }

    triggerUpdate(true);
}

/*!
    \overload

    Inserts a new list box text item with the text \a text into the
    list at position \a index.

    If \a index is negative, \a text is inserted at the end of the
    list.

    \sa insertStrList()
*/

void Q3ListBox::insertItem(const QString &text, int index)
{
    insertItem(new Q3ListBoxText(text), index);
}

/*!
    \overload

    Inserts a new list box pixmap item with the pixmap \a pixmap into
    the list at position \a index.

    If \a index is negative, \a pixmap is inserted at the end of the
    list.

    \sa insertStrList()
*/

void Q3ListBox::insertItem(const QPixmap &pixmap, int index)
{
    insertItem(new Q3ListBoxPixmap(pixmap), index);
}

/*!
    \overload

    Inserts a new list box pixmap item with the pixmap \a pixmap and
    the text \a text into the list at position \a index.

    If \a index is negative, \a pixmap is inserted at the end of the
    list.

    \sa insertStrList()
*/

void Q3ListBox::insertItem(const QPixmap &pixmap, const QString &text, int index)
{
    insertItem(new Q3ListBoxPixmap(pixmap, text), index);
}

/*!
    Removes and deletes the item at position \a index. If \a index is
    equal to currentItem(), a new item becomes current and the
    currentChanged() and highlighted() signals are emitted.

    \sa insertItem(), clear()
*/

void Q3ListBox::removeItem(int index)
{
    bool wasVisible = itemVisible(currentItem());
    delete item(index);
    triggerUpdate(true);
    if (wasVisible)
        ensureCurrentVisible();
}


/*!
    Deletes all the items in the list.

    \sa removeItem()
*/

void Q3ListBox::clear()
{
    setContentsPos(0, 0);
    bool blocked = signalsBlocked();
    blockSignals(true);
    d->clearing = true;
    d->current = 0;
    d->tmpCurrent = 0;
    Q3ListBoxItem * i = d->head;
    d->head = 0;
    while (i) {
        Q3ListBoxItem * n = i->n;
        i->n = i->p = 0;
        delete i;
        i = n;
    }
    d->count = 0;
    d->numRows = 1;
    d->numColumns = 1;
    d->currentRow = 0;
    d->currentColumn = 0;
    d->mousePressRow = -1;
    d->mousePressColumn = -1;
    d->mouseMoveRow = -1;
    d->mouseMoveColumn = -1;
    clearSelection();
    d->selectAnchor = 0;
    blockSignals(blocked);
    triggerUpdate(true);
    d->last = 0;
    d->clearing = false;
}


/*!
    Returns the text at position \a index, or an empty string if there
    is no text at that position.

    \sa pixmap()
*/

QString Q3ListBox::text(int index) const
{
    Q3ListBoxItem * i = item(index);
    if (i)
        return i->text();
    return QString();
}


/*!
    Returns a pointer to the pixmap at position \a index, or 0 if
    there is no pixmap there.

    \sa text()
*/

const QPixmap *Q3ListBox::pixmap(int index) const
{
    Q3ListBoxItem * i = item(index);
    if (i)
        return i->pixmap();
    return 0;
}

/*!
    \overload

    Replaces the item at position \a index with a new list box text
    item with text \a text.

    The operation is ignored if \a index is out of range.

    \sa insertItem(), removeItem()
*/

void Q3ListBox::changeItem(const QString &text, int index)
{
    if(index >= 0 && index < (int)count())
        changeItem(new Q3ListBoxText(text), index);
}

/*!
    \overload

    Replaces the item at position \a index with a new list box pixmap
    item with pixmap \a pixmap.

    The operation is ignored if \a index is out of range.

    \sa insertItem(), removeItem()
*/

void Q3ListBox::changeItem(const QPixmap &pixmap, int index)
{
    if(index >= 0 && index < (int)count())
        changeItem(new Q3ListBoxPixmap(pixmap), index);
}

/*!
    \overload

    Replaces the item at position \a index with a new list box pixmap
    item with pixmap \a pixmap and text \a text.

    The operation is ignored if \a index is out of range.

    \sa insertItem(), removeItem()
*/

void Q3ListBox::changeItem(const QPixmap &pixmap, const QString &text, int index)
{
    if(index >= 0 && index < (int)count())
        changeItem(new Q3ListBoxPixmap(pixmap, text), index);
}



/*!
    Replaces the item at position \a index with \a lbi.        If \a index is
    negative or too large, changeItem() does nothing.

    The item that has been changed will become selected.

    \sa insertItem(), removeItem()
*/

void Q3ListBox::changeItem(const Q3ListBoxItem *lbi, int index)
{
    if (!lbi || index < 0 || index >= (int)count())
        return;

    removeItem(index);
    insertItem(lbi, index);
    setCurrentItem(index);
}


/*!
    \property Q3ListBox::numItemsVisible
    \brief the number of visible items.

    Both partially and entirely visible items are counted.
*/

int Q3ListBox::numItemsVisible() const
{
    doLayout();

    int columns = 0;

    int x = contentsX();
    int i=0;
    while (i < (int)d->columnPos.size()-1 &&
           d->columnPos[i] < x)
        i++;
    if (i < (int)d->columnPos.size()-1 &&
         d->columnPos[i] > x)
        columns++;
    x += visibleWidth();
    while (i < (int)d->columnPos.size()-1 &&
           d->columnPos[i] < x) {
        i++;
        columns++;
    }

    int y = contentsY();
    int rows = 0;
    while (i < (int)d->rowPos.size()-1 &&
           d->rowPos[i] < y)
        i++;
    if (i < (int)d->rowPos.size()-1 &&
         d->rowPos[i] > y)
        rows++;
    y += visibleHeight();
    while (i < (int)d->rowPos.size()-1 &&
           d->rowPos[i] < y) {
        i++;
        rows++;
    }

    return rows*columns;
}

int Q3ListBox::currentItem() const
{
    if (!d->current || !d->head)
        return -1;

    return index(d->current);
}


/*!
    \property Q3ListBox::currentText
    \brief the text of the current item.

    This is equivalent to text(currentItem()).
*/


/*!
    \property Q3ListBox::currentItem
    \brief the current highlighted item

    When setting this property, the highlighting is moved to the item
    and the list box scrolled as necessary.

    If no item is current, currentItem() returns -1.
*/

void Q3ListBox::setCurrentItem(int index)
{
    setCurrentItem(item(index));
}

/*!
    \reimp
*/
QVariant Q3ListBox::inputMethodQuery(Qt::InputMethodQuery query) const
{
    if (query == Qt::ImMicroFocus)
        return d->current ? itemRect(d->current) : QRect();
    return QWidget::inputMethodQuery(query);
}

/*!
    \overload

    Sets the current item to the Q3ListBoxItem \a i.
*/
void Q3ListBox::setCurrentItem(Q3ListBoxItem * i)
{
    if (!i || d->current == i)
        return;

    Q3ListBoxItem * o = d->current;
    d->current = i;
    int ind = index(i);

    if (i && selectionMode() == Single) {
        bool changed = false;
        if (o && o->s) {
            changed = true;
            o->s = false;
        }
        if (i && !i->s && d->selectionMode != NoSelection && i->isSelectable()) {
            i->s = true;
            changed = true;
            emit selectionChanged(i);
#ifndef QT_NO_ACCESSIBILITY
            QAccessible::updateAccessibility(viewport(), ind+1, QAccessible::StateChanged);
#endif
        }
        if (changed) {
            emit selectionChanged();
#ifndef QT_NO_ACCESSIBILITY
            QAccessible::updateAccessibility(viewport(), 0, QAccessible::Selection);
#endif
        }
    }

    d->currentColumn = ind / numRows();
    d->currentRow = ind % numRows();
    if (o)
        updateItem(o);
    if (i)
        updateItem(i);
    // scroll after the items are redrawn
    d->visibleTimer->start(1, true);

    QString tmp;
    if (i)
        tmp = i->text();
    emit highlighted(i);
    if (!tmp.isNull())
        emit highlighted(tmp);
    emit highlighted(ind);
    emit currentChanged(i);

#ifndef QT_NO_ACCESSIBILITY
    QAccessible::updateAccessibility(viewport(), ind+1, QAccessible::Focus);
#endif
}


/*!
    Returns a pointer to the item at position \a index, or 0 if \a
    index is out of bounds.

    \sa index()
*/

Q3ListBoxItem *Q3ListBox::item(int index) const
{
    if (index < 0 || index > d->count -1)
        return 0;

    Q3ListBoxItem * i = d->head;

    if (d->cache && index > 0) {
        i = d->cache;
        int idx = d->cacheIndex;
        while (i && idx < index) {
            idx++;
            i = i->n;
        }
        while (i && idx > index) {
            idx--;
            i = i->p;
        }
    } else {
        int idx = index;
        while (i && idx > 0) {
            idx--;
            i = i->n;
        }
    }

    if (index > 0) {
        d->cache = i;
        d->cacheIndex = index;
    }

    return i;
}


/*!
    Returns the index of \a lbi, or -1 if the item is not in this list
    box or \a lbi is 0.

    \sa item()
*/

int Q3ListBox::index(const Q3ListBoxItem * lbi) const
{
    if (!lbi)
        return -1;
    Q3ListBoxItem * i_n = d->head;
    int c_n = 0;
    if (d->cache) {
        i_n = d->cache;
        c_n = d->cacheIndex;
    }
    Q3ListBoxItem* i_p = i_n;
    int c_p = c_n;
    while ((i_n != 0 || i_p != 0) && i_n != lbi && i_p != lbi) {
        if (i_n) {
            c_n++;
            i_n = i_n->n;
        }
        if (i_p) {
            c_p--;
            i_p = i_p->p;
        }
    }
    if (i_p == lbi)
        return c_p;
    if (i_n == lbi)
        return c_n;
    return -1;
}



/*!
    Returns true if the item at position \a index is at least partly
    visible; otherwise returns false.
*/

bool Q3ListBox::itemVisible(int index)
{
    Q3ListBoxItem * i = item(index);
    return i ? itemVisible(i) : false;
}


/*!
    \overload

    Returns true if \a item is at least partly visible; otherwise
    returns false.
*/

bool Q3ListBox::itemVisible(const Q3ListBoxItem * item)
{
    if (d->layoutDirty)
        doLayout();

    int i = index(item);
    int col = i / numRows();
    int row = i % numRows();
    return (d->columnPos[col] < contentsX()+visibleWidth() &&
             d->rowPos[row] < contentsY()+visibleHeight() &&
             d->columnPos[col+1] > contentsX() &&
             d->rowPos[row+1] > contentsY());
}


/*! \reimp */

void Q3ListBox::mousePressEvent(QMouseEvent *e)
{
    mousePressEventEx(e);
}

void Q3ListBox::mousePressEventEx(QMouseEvent *e)
{
    d->mouseInternalPress = true;
    Q3ListBoxItem * i = itemAt(e->pos());

    if (!i && !d->current && d->head) {
        d->current = d->head;
        updateItem(d->head);
    }

    if (!i && (d->selectionMode != Single || e->button() == Qt::RightButton)
         && !(e->state() & Qt::ControlButton))
        clearSelection();

    d->select = d->selectionMode == Multi ? (i ? !i->isSelected() : false) : true;
    d->pressedSelected = i && i->s;

    if (i)
        d->selectAnchor = i;
    if (i) {
        switch(selectionMode()) {
        default:
        case Single:
            if (!i->s || i != d->current) {
                if (i->isSelectable())
                    setSelected(i, true);
                else
                    setCurrentItem(i);
            }
            break;
        case Extended:
            if (i) {
                bool shouldBlock = false;
                if (!(e->state() & Qt::ShiftButton) &&
                    !(e->state() & Qt::ControlButton)) {
                    if (!i->isSelected()) {
                        bool b = signalsBlocked();
                        blockSignals(true);
                        clearSelection();
                        blockSignals(b);
                    }
                    setSelected(i, true);
                    d->dragging = true; // always assume dragging
                    shouldBlock = true;
                } else if (e->state() & Qt::ShiftButton) {
                    d->pressedSelected = false;
                    Q3ListBoxItem *oldCurrent = item(currentItem());
                    bool down = index(oldCurrent) < index(i);

                    Q3ListBoxItem *lit = down ? oldCurrent : i;
                    bool select = d->select;
                    bool blocked = signalsBlocked();
                    blockSignals(true);
                    for (;; lit = lit->n) {
                        if (!lit) {
                            triggerUpdate(false);
                            break;
                        }
                        if (down && lit == i) {
                            setSelected(i, select);
                            triggerUpdate(false);
                            break;
                        }
                        if (!down && lit == oldCurrent) {
                            setSelected(oldCurrent, select);
                            triggerUpdate(false);
                            break;
                        }
                        setSelected(lit, select);
                    }
                    blockSignals(blocked);
                    emit selectionChanged();
                } else if (e->state() & Qt::ControlButton) {
                    setSelected(i, !i->isSelected());
                    shouldBlock = true;
                    d->pressedSelected = false;
                }
                bool blocked = signalsBlocked();
                blockSignals(shouldBlock);
                setCurrentItem(i);
                blockSignals(blocked);
            }
            break;
        case Multi:
	    {
                setSelected(i, !i->s);
                bool b = signalsBlocked();
                blockSignals(true);
                setCurrentItem(i);
                blockSignals(b);
                break;
	    }
        case NoSelection:
            setCurrentItem(i);
            break;
        }
    } else {
        bool unselect = true;
        if (e->button() == Qt::LeftButton) {
            if (d->selectionMode == Multi ||
                 d->selectionMode == Extended) {
                d->tmpCurrent = d->current;
                d->current = 0;
                updateItem(d->tmpCurrent);
                if (d->rubber)
                    delete d->rubber;
                d->rubber = 0;
                d->rubber = new QRect(e->x(), e->y(), 0, 0);

                if (d->selectionMode == Extended && !(e->state() & Qt::ControlButton))
                    selectAll(false);
                unselect = false;
            }
            if (unselect && (e->button() == Qt::RightButton ||
                               (selectionMode() == Multi || selectionMode() == Extended)))
                clearSelection();
        }
    }

    // for sanity, in case people are event-filtering or whatnot
    delete d->scrollTimer;
    d->scrollTimer = 0;
    if (i) {
        d->mousePressColumn = d->currentColumn;
        d->mousePressRow = d->currentRow;
    } else {
        d->mousePressColumn = -1;
        d->mousePressRow = -1;
    }
    d->ignoreMoves = false;

    d->pressedItem = i;

    emit pressed(i);
    emit pressed(i, e->globalPos());
    emit mouseButtonPressed(e->button(), i, e->globalPos());
    if (e->button() == Qt::RightButton)
        emit rightButtonPressed(i, e->globalPos());
}


/*! \reimp */

void Q3ListBox::mouseReleaseEvent(QMouseEvent *e)
{
    if (d->selectionMode == Extended &&
        d->dragging) {
        d->dragging = false;
        if (d->current != d->pressedItem) {
            updateSelection(); // when we drag, we get an update after we release
        }
    }

    if (d->rubber) {
        drawRubber();
        delete d->rubber;
        d->rubber = 0;
        d->current = d->tmpCurrent;
        updateItem(d->current);
    }
    if (d->scrollTimer)
        mouseMoveEvent(e);
    delete d->scrollTimer;
    d->scrollTimer = 0;
    d->ignoreMoves = false;

    if (d->selectionMode == Extended &&
         d->current == d->pressedItem &&
         d->pressedSelected && d->current) {
        bool block = signalsBlocked();
        blockSignals(true);
        clearSelection();
        blockSignals(block);
        d->current->s = true;
        emit selectionChanged();
    }

    Q3ListBoxItem * i = itemAt(e->pos());
    bool emitClicked = (d->mousePressColumn != -1 && d->mousePressRow != -1) || !d->pressedItem;
    emitClicked = emitClicked && d->pressedItem == i;
    d->pressedItem = 0;
    d->mousePressRow = -1;
    d->mousePressColumn = -1;
    d->mouseInternalPress = false;
    if (emitClicked) {
        emit clicked(i);
        emit clicked(i, e->globalPos());
        emit mouseButtonClicked(e->button(), i, e->globalPos());
        if (e->button() == Qt::RightButton)
            emit rightButtonClicked(i, e->globalPos());
    }
}


/*! \reimp */

void Q3ListBox::mouseDoubleClickEvent(QMouseEvent *e)
{
    bool ok = true;
    Q3ListBoxItem *i = itemAt(e->pos());
    if (!i || selectionMode() == NoSelection)
        ok = false;

    d->ignoreMoves = true;

    if (d->current && ok) {
        Q3ListBoxItem * i = d->current;
        QString tmp = d->current->text();
        emit selected(currentItem());
        emit selected(i);
        if (!tmp.isNull())
            emit selected(tmp);
        emit doubleClicked(i);
    }
}


/*! \reimp */

void Q3ListBox::mouseMoveEvent(QMouseEvent *e)
{
    Q3ListBoxItem * i = itemAt(e->pos());
    if (i != d->highlighted) {
        if (i) {
            emit onItem(i);
        } else {
            emit onViewport();
        }
        d->highlighted = i;
    }

    if (d->rubber) {
        QRect r = d->rubber->normalized();
        drawRubber();
        d->rubber->setCoords(d->rubber->x(), d->rubber->y(), e->x(), e->y());
        doRubberSelection(r, d->rubber->normalized());
        drawRubber();
        return;
    }

    if (((e->state() & (Qt::RightButton | Qt::LeftButton | Qt::MidButton)) == 0) ||
         d->ignoreMoves)
        return;

    // hack to keep the combo (and what else?) working: if we get a
    // move outside the listbox without having seen a press, discard
    // it.
    if (!QRect(0, 0, visibleWidth(), visibleHeight()).contains(e->pos()) &&
         ((d->mousePressColumn < 0 && d->mousePressRow < 0)
          || (e->state() == Qt::NoButton && !d->pressedItem)))
        return;

    // figure out in what direction to drag-select and perhaps scroll
    int dx = 0;
    int x = e->x();
    if (x >= visibleWidth()) {
        x = visibleWidth()-1;
        dx = 1;
    } else if (x < 0) {
        x = 0;
        dx = -1;
    }
    d->mouseMoveColumn = columnAt(x + contentsX());

    // sanitize mousePressColumn, if we got here without a mouse press event
    if (d->mousePressColumn < 0 && d->mouseMoveColumn >= 0)
        d->mousePressColumn = d->mouseMoveColumn;
    if (d->mousePressColumn < 0 && d->currentColumn >= 0)
        d->mousePressColumn = d->currentColumn;

    // if it's beyond the last column, use the last one
    if (d->mouseMoveColumn < 0)
        d->mouseMoveColumn = dx >= 0 ? numColumns()-1 : 0;

    // repeat for y
    int dy = 0;
    int y = e->y();
    if (y >= visibleHeight()) {
        y = visibleHeight()-1;
        dy = 1;
    } else if (y < 0) {
        y = 0;
        dy = -1;
    }
    d->mouseMoveRow = rowAt(y + contentsY());

    if (d->mousePressRow < 0 && d->mouseMoveRow >= 0)
        d->mousePressRow = d->mouseMoveRow;
    if (d->mousePressRow < 0 && d->currentRow >= 0)
        d->mousePressRow = d->currentRow;

    if (d->mousePressRow < 0)
        d->mousePressRow = rowAt(x + contentsX());

    d->scrollPos = QPoint(dx, dy);

    if ((dx || dy) && !d->scrollTimer && e->state() == Qt::LeftButton && e->button() != Qt::LeftButton) {
        // start autoscrolling if necessary
        d->scrollTimer = new QTimer(this);
        connect(d->scrollTimer, SIGNAL(timeout()),
                 this, SLOT(doAutoScroll()));
        d->scrollTimer->start(100, false);
        doAutoScroll();
    } else if (!d->scrollTimer) {
        // or just select the required bits
        updateSelection();
    }
}



void Q3ListBox::updateSelection()
{
    if (d->mouseMoveColumn >= 0 && d->mouseMoveRow >= 0 &&
         d->mousePressColumn >= 0 && d->mousePressRow >= 0) {
        Q3ListBoxItem * i = item(d->mouseMoveColumn * numRows() +
                                 d->mouseMoveRow);
#ifndef QT_NO_ACCESSIBILITY
        int ind = index(i);
#endif
        if (selectionMode() == Single || selectionMode() == NoSelection) {
            if (i && (d->mouseInternalPress || (windowType() == Qt::Popup)))
                setCurrentItem(i);
        } else {
            if (d->selectionMode == Extended && (
                 (d->current == d->pressedItem && d->pressedSelected) ||
                (d->dirtyDrag && !d->dragging))) {
                if (d->dirtyDrag && !d->dragging) // emit after dragging stops
                    d->dirtyDrag = false;
                else
                    clearSelection(); // don't reset drag-selected items
                d->pressedItem = 0;
                if (i && i->isSelectable()) {
                    bool block = signalsBlocked();
                    blockSignals(true);
                    i->s = true;
                    blockSignals(block);
                    emit selectionChanged();
#ifndef QT_NO_ACCESSIBILITY
                    QAccessible::updateAccessibility(viewport(), ind+1, QAccessible::StateChanged);
                    QAccessible::updateAccessibility(viewport(), 0, QAccessible::Selection);
                    QAccessible::updateAccessibility(viewport(), ind+1, QAccessible::SelectionAdd);
#endif
                }
                triggerUpdate(false);
            } else {
                int c = qMin(d->mouseMoveColumn, d->mousePressColumn);
                int r = qMin(d->mouseMoveRow, d->mousePressRow);
                int c2 = qMax(d->mouseMoveColumn, d->mousePressColumn);
                int r2 = qMax(d->mouseMoveRow, d->mousePressRow);
                bool changed = false;
                while(c <= c2) {
                    Q3ListBoxItem * i = item(c*numRows()+r);
                    int rtmp = r;
                    while(i && rtmp <= r2) {
                        if ((bool)i->s != (bool)d->select && i->isSelectable()) {
                            i->s = d->select;
#ifndef QT_NO_ACCESSIBILITY
                            QAccessible::updateAccessibility(viewport(), ind+1, QAccessible::StateChanged);
                            QAccessible::updateAccessibility(viewport(), ind+1, d->select ? QAccessible::SelectionAdd : QAccessible::SelectionRemove);
#endif
                            i->dirty = true;
                            d->dirtyDrag = changed = true;
                        }
                        i = i->n;
                        rtmp++;
                    }
                    c++;
                }
                if (changed) {
                    if (!d->dragging) // emit after dragging stops instead
                        emit selectionChanged();
#ifndef QT_NO_ACCESSIBILITY
                    QAccessible::updateAccessibility(viewport(), 0, QAccessible::Selection);
#endif
                    triggerUpdate(false);
                }
            }
            if (i)
                setCurrentItem(i);
        }
    }
}

void Q3ListBox::repaintSelection()
{
    if (d->numColumns == 1) {
        for (uint i = topItem(); itemVisible(i) && i < count(); ++i) {
            Q3ListBoxItem *it = item(i);
            if (!it)
                break;
            if (it->isSelected())
                updateItem(it);
        }
    } else {
        for (uint i = 0; i < count(); ++i) {
            Q3ListBoxItem *it = item(i);
            if (!it)
                break;
            if (it->isSelected())
                updateItem(it);
        }
    }
}

/*! \reimp
*/

void Q3ListBox::contentsContextMenuEvent(QContextMenuEvent *e)
{
    if (!receivers(SIGNAL(contextMenuRequested(Q3ListBoxItem*,QPoint)))) {
        e->ignore();
        return;
    }
    if (e->reason() == QContextMenuEvent::Keyboard) {
        Q3ListBoxItem *i = item(currentItem());
        if (i) {
            QRect r = itemRect(i);
            emit contextMenuRequested(i, mapToGlobal(r.topLeft() + QPoint(width() / 2, r.height() / 2)));
        }
    } else {
        Q3ListBoxItem * i = itemAt(contentsToViewport(e->pos()));
        emit contextMenuRequested(i, e->globalPos());
    }
}

/*!\reimp
*/
void Q3ListBox::keyPressEvent(QKeyEvent *e)
{
    if ((e->key() == Qt::Key_Tab || e->key() == Qt::Key_Backtab)
         && e->state() & Qt::ControlButton)
        e->ignore();

    if (count() == 0) {
        e->ignore();
        return;
    }

    QPointer<Q3ListBox> selfCheck = this;

    Q3ListBoxItem *old = d->current;
    if (!old) {
        setCurrentItem(d->head);
        if (d->selectionMode == Single)
            setSelected(d->head, true);
        e->ignore();
        return;
    }

    bool selectCurrent = false;
    switch (e->key()) {
        case Qt::Key_Up:
            {
                d->currInputString.clear();
                if (currentItem() > 0) {
                    setCurrentItem(currentItem() - 1);
                    handleItemChange(old, e->state() & Qt::ShiftButton, e->state() & Qt::ControlButton);
                }
                if (!(e->state() & Qt::ShiftButton) || !d->selectAnchor)
                    d->selectAnchor = d->current;
            }
            break;
        case Qt::Key_Down:
            {
                d->currInputString.clear();
                if (currentItem() < (int)count() - 1) {
                    setCurrentItem(currentItem() + 1);
                    handleItemChange(old, e->state() & Qt::ShiftButton, e->state() & Qt::ControlButton);
                }
                if (!(e->state() & Qt::ShiftButton) || !d->selectAnchor)
                    d->selectAnchor = d->current;
            }
            break;
        case Qt::Key_Left:
            {
                d->currInputString.clear();
                if (currentColumn() > 0) {
                    setCurrentItem(currentItem() - numRows());
                    handleItemChange(old, e->state() & Qt::ShiftButton, e->state() & Qt::ControlButton);
                } else if (numColumns() > 1 && currentItem() > 0) {
                    int row = currentRow();
                    setCurrentItem(currentRow() - 1 + (numColumns() - 1) * numRows());

                    if (currentItem() == -1)
                        setCurrentItem(row - 1 + (numColumns() - 2) * numRows());

                    handleItemChange(old, e->state() & Qt::ShiftButton, e->state() & Qt::ControlButton);
                } else {
                    QApplication::sendEvent(horizontalScrollBar(), e);
                }
                if (!(e->state() & Qt::ShiftButton) || !d->selectAnchor)
                    d->selectAnchor = d->current;
            }
            break;
        case Qt::Key_Right:
            {
                d->currInputString.clear();
                if (currentColumn() < numColumns()-1) {
                    int row = currentRow();
                    int i = currentItem();
                    Q3ListBoxItem *it = item(i + numRows());
                    if (!it)
                        it = item(count()-1);
                    setCurrentItem(it);

                    if (currentItem() == -1) {
                        if (row < numRows() - 1)
                            setCurrentItem(row + 1);
                        else
                            setCurrentItem(i);
                    }

                    handleItemChange(old, e->state() & Qt::ShiftButton, e->state() & Qt::ControlButton);
                } else if (numColumns() > 1 && currentRow() < numRows()) {
                    if (currentRow() + 1 < numRows()) {
                        setCurrentItem(currentRow() + 1);
                        handleItemChange(old, e->state() & Qt::ShiftButton, e->state() & Qt::ControlButton);
                    }
                } else {
                    QApplication::sendEvent(horizontalScrollBar(), e);
                }
                if (!(e->state() & Qt::ShiftButton) || !d->selectAnchor)
                    d->selectAnchor = d->current;
            }
            break;
        case Qt::Key_Next:
            {
                d->currInputString.clear();
                int i = 0;
                if (numColumns() == 1) {
                    i = currentItem() + numItemsVisible();
                    i = i > (int)count() - 1 ? (int)count() - 1 : i;
                    setCurrentItem(i);
                    setBottomItem(i);
                } else {
                    // I'm not sure about this behavior...
                    if (currentRow() == numRows() - 1)
                        i = currentItem() + numRows();
                    else
                        i = currentItem() + numRows() - currentRow() - 1;
                    i = i > (int)count() - 1 ? (int)count() - 1 : i;
                    setCurrentItem(i);
                }
                handleItemChange(old, e->state() & Qt::ShiftButton, e->state() & Qt::ControlButton);
                if (!(e->state() & Qt::ShiftButton) || !d->selectAnchor)
                    d->selectAnchor = d->current;
            }
            break;
        case Qt::Key_Prior:
            {
                selectCurrent = true;
                d->currInputString.clear();
                int i;
                if (numColumns() == 1) {
                    i = currentItem() - numItemsVisible();
                    i = i < 0 ? 0 : i;
                    setCurrentItem(i);
                    setTopItem(i);
                } else {
                    // I'm not sure about this behavior...
                    if (currentRow() == 0)
                        i = currentItem() - numRows();
                    else
                        i = currentItem() - currentRow();
                    i = i < 0 ? 0 : i;
                    setCurrentItem(i);
                }
                handleItemChange(old, e->state() & Qt::ShiftButton, e->state() & Qt::ControlButton);
                if (!(e->state() & Qt::ShiftButton) || !d->selectAnchor)
                    d->selectAnchor = d->current;
            }
            break;
        case Qt::Key_Space:
            {
                selectCurrent = true;
                d->currInputString.clear();
                toggleCurrentItem();
                if (selectionMode() == Extended && d->current->isSelected())
                    emit highlighted(currentItem());
                if (selfCheck && (!(e->state() & Qt::ShiftButton) || !d->selectAnchor))
                    d->selectAnchor = d->current;
            }
            break;
        case Qt::Key_Return:
        case Qt::Key_Enter:
            {
                selectCurrent = true;
                d->currInputString.clear();
                if (currentItem() >= 0 && selectionMode() != NoSelection) {
                    QString tmp = item(currentItem())->text();
                    emit selected(currentItem());
                    emit selected(item(currentItem()));
                    if (!tmp.isEmpty())
                        emit selected(tmp);
                    emit returnPressed(item(currentItem()));
                }
                if (selfCheck && (!(e->state() & Qt::ShiftButton) || !d->selectAnchor))
                    d->selectAnchor = d->current;
            }
            break;
        case Qt::Key_Home:
            {
                selectCurrent = true;
                d->currInputString.clear();
                setCurrentItem(0);
                handleItemChange(old, e->state() & Qt::ShiftButton, e->state() & Qt::ControlButton);
                if (!(e->state() & Qt::ShiftButton) || !d->selectAnchor)
                    d->selectAnchor = d->current;
            }
            break;
        case Qt::Key_End:
            {
                selectCurrent = true;
                d->currInputString.clear();
                int i = (int)count() - 1;
                setCurrentItem(i);
                handleItemChange(old, e->state() & Qt::ShiftButton, e->state() & Qt::ControlButton);
                if (!(e->state() & Qt::ShiftButton) || !d->selectAnchor)
                    d->selectAnchor = d->current;
            }
            break;
        default:
            {
                if (!e->text().isEmpty() && e->text()[0].isPrint() && count()) {
                    int curItem = currentItem();
                    if (curItem == -1)
                        curItem = 0;
                    if (!d->inputTimer->isActive()) {
                        d->currInputString = e->text();
                        curItem = d->findItemByName(++curItem, d->currInputString);
                    } else {
                        d->inputTimer->stop();
                        d->currInputString += e->text();
                        int oldCurItem = curItem;
                        curItem = d->findItemByName(curItem, d->currInputString);
                        if (curItem < 0) {
                            curItem = d->findItemByName(++oldCurItem, e->text());
                            d->currInputString = e->text();
                        }
                    }
                    if (curItem >= 0)
                        setCurrentItem(curItem);
                    if (curItem >= 0 && selectionMode() == Q3ListBox::Extended) {
                        bool changed = false;
                        bool block = signalsBlocked();
                        blockSignals(true);
                        selectAll(false);
                        blockSignals(block);
                        Q3ListBoxItem *i = item(curItem);
                        if (!i->s && i->isSelectable()) {
                            changed = true;
                            i->s = true;
                            updateItem(i);
                        }
                        if (changed)
                            emit selectionChanged();
                    }
                    d->inputTimer->start(400, true);
                } else {
                    d->currInputString.clear();
                    if (e->state() & Qt::ControlButton) {
                        switch (e->key()) {
                            case Qt::Key_A:
                                selectAll(true);
                                break;
                        }
                    } else {
                        e->ignore();
                    }
                }
            }
    }

    if (selfCheck && selectCurrent && selectionMode() == Single &&
        d->current && !d->current->s) {
            updateItem(d->current);
            setSelected(d->current, true);
        }
}


/*!\reimp
*/
void Q3ListBox::focusInEvent(QFocusEvent *e)
{
    d->mousePressRow = -1;
    d->mousePressColumn = -1;
    d->inMenuMode = false;
    if (e->reason() != Qt::MouseFocusReason && !d->current && d->head) {
        d->current = d->head;
        Q3ListBoxItem *i = d->current;
        QString tmp;
        if (i)
            tmp = i->text();
        int tmp2 = index(i);
        emit highlighted(i);
        if (!tmp.isNull())
            emit highlighted(tmp);
        emit highlighted(tmp2);
        emit currentChanged(i);
    }
    if (style()->styleHint(QStyle::SH_ItemView_ChangeHighlightOnFocus, 0, this))
        repaintSelection();

    if (d->current)
        updateItem(currentItem());
}


/*!\reimp
*/
void Q3ListBox::focusOutEvent(QFocusEvent *e)
{
    if (style()->styleHint(QStyle::SH_ItemView_ChangeHighlightOnFocus, 0, this)) {
        d->inMenuMode =
            e->reason() == Qt::PopupFocusReason ||
            (qApp->focusWidget() && qApp->focusWidget()->inherits("QMenuBar"));
        if (!d->inMenuMode)
            repaintSelection();
    }

    if (d->current)
        updateItem(currentItem());
}

/*!\reimp
*/
bool Q3ListBox::eventFilter(QObject *o, QEvent *e)
{
    return Q3ScrollView::eventFilter(o, e);
}

/*!
    Repaints the item at position \a index in the list.
*/

void Q3ListBox::updateItem(int index)
{
    if (index >= 0)
        updateItem(item(index));
}


/*!
    \overload

    Repaints the Q3ListBoxItem \a i.
*/

void Q3ListBox::updateItem(Q3ListBoxItem * i)
{
    if (!i)
        return;
    i->dirty = true;
    d->updateTimer->start(0, true);
}


/*!
    \property Q3ListBox::selectionMode
    \brief the selection mode of the list box

    Sets the list box's selection mode, which may be one of \c Single
    (the default), \c Extended, \c Multi or \c NoSelection.

    \sa SelectionMode
*/

void Q3ListBox::setSelectionMode(SelectionMode mode)
{
    if (d->selectionMode == mode)
        return;

    if ((selectionMode() == Multi || selectionMode() == Extended)
         && (mode == Q3ListBox::Single || mode == Q3ListBox::NoSelection)){
        clearSelection();
        if ((mode == Q3ListBox::Single) && currentItem())
            setSelected(currentItem(), true);
    }

    d->selectionMode = mode;
    triggerUpdate(true);
}


Q3ListBox::SelectionMode Q3ListBox::selectionMode() const
{
    return d->selectionMode;
}


/*!
  \property Q3ListBox::multiSelection
  \brief whether or not the list box is in Multi selection mode

  Consider using the \l Q3ListBox::selectionMode property instead of
  this property.

  When setting this property, Multi selection mode is used if set to true and
  to Single selection mode if set to false.

  When getting this property, true is returned if the list box is in
  Multi selection mode or Extended selection mode, and false if it is
  in Single selection mode or NoSelection mode.

  \sa selectionMode
*/

bool Q3ListBox::isMultiSelection() const
{
    return selectionMode() == Multi || selectionMode() == Extended;
}

void Q3ListBox::setMultiSelection(bool enable)
{
    setSelectionMode(enable ? Multi : Single);
}


/*!
    Toggles the selection status of currentItem() and repaints if the
    list box is a \c Multi selection list box.

    \sa setMultiSelection()
*/

void Q3ListBox::toggleCurrentItem()
{
    if (selectionMode() == Single ||
         selectionMode() == NoSelection ||
         !d->current)
        return;

    if (d->current->s || d->current->isSelectable()) {
        d->current->s = !d->current->s;
        emit selectionChanged();
#ifndef QT_NO_ACCESSIBILITY
        int ind = index(d->current);
        QAccessible::updateAccessibility(viewport(), 0, QAccessible::Selection);
        QAccessible::updateAccessibility(viewport(), ind+1, QAccessible::StateChanged);
        QAccessible::updateAccessibility(viewport(), ind+1, d->current->s ? QAccessible::SelectionAdd : QAccessible::SelectionRemove);
#endif
    }
    updateItem(d->current);
}


/*!
    \overload

    If \a select is true the item at position \a index is selected;
    otherwise the item is deselected.
*/

void Q3ListBox::setSelected(int index, bool select)
{
    setSelected(item(index), select);
}


/*!
    Selects \a item if \a select is true or unselects it if \a select
    is false, and repaints the item appropriately.

    If the list box is a \c Single selection list box and \a select is
    true, setSelected() calls setCurrentItem().

    If the list box is a \c Single selection list box, \a select is
    false, setSelected() calls clearSelection().

    \sa setMultiSelection(), setCurrentItem(), clearSelection(), currentItem()
*/

void Q3ListBox::setSelected(Q3ListBoxItem * item, bool select)
{
    if (!item || !item->isSelectable() ||
        (bool)item->s == select || d->selectionMode == NoSelection)
        return;

    int ind = index(item);
    bool emitHighlighted = (d->current != item) || ( select && (item->s != (uint) select) );
    if (selectionMode() == Single) {
        if (d->current != item) {
            Q3ListBoxItem *o = d->current;
            if (d->current && d->current->s)
                d->current->s = false;
            d->current = item;
#ifndef QT_NO_ACCESSIBILITY
            QAccessible::updateAccessibility(viewport(), ind+1, QAccessible::Focus);
#endif
            d->currentColumn = ind / numRows();
            d->currentRow = ind % numRows();

            if (o)
                updateItem(o);
        }
    }

    item->s = (uint)select;
    updateItem(item);

    if (d->selectionMode == Single && select) {
        emit selectionChanged(item);
#ifndef QT_NO_ACCESSIBILITY
        QAccessible::updateAccessibility(viewport(), ind+1, QAccessible::StateChanged);
#endif
    }
    emit selectionChanged();
#ifndef QT_NO_ACCESSIBILITY
    QAccessible::updateAccessibility(viewport(), 0, QAccessible::Selection);
    if (d->selectionMode != Single)
        QAccessible::updateAccessibility(viewport(), ind+1, select ? QAccessible::SelectionAdd : QAccessible::SelectionRemove);
#endif

    if (emitHighlighted) {
        QString tmp;
        if (item)
            tmp = item->text();
        int tmp2 = index(item);
        emit highlighted(item);
        if (!tmp.isNull())
            emit highlighted(tmp);
        emit highlighted(tmp2);
        emit currentChanged(item);
    }
}


/*!
    Returns true if item \a i is selected; otherwise returns false.
*/

bool Q3ListBox::isSelected(int i) const
{
    if (selectionMode() == Single && i != currentItem())
        return false;

    Q3ListBoxItem * lbi = item(i);
    if (!lbi)
        return false; // should not happen
    return lbi->s;
}


/*!
    \overload

    Returns true if item \a i is selected; otherwise returns false.
*/

bool Q3ListBox::isSelected(const Q3ListBoxItem * i) const
{
    if (!i)
        return false;

    return i->s;
}

/*!  Returns the selected item if the list box is in
single-selection mode and an item is selected.

If no items are selected or the list box is in another selection mode
this function returns 0.

\sa setSelected() setMultiSelection()
*/

Q3ListBoxItem* Q3ListBox::selectedItem() const
{
    if (d->selectionMode != Single)
        return 0;
    if (isSelected(currentItem()))
        return  d->current;
    return 0;
}


/*!
    Deselects all items, if possible.

    Note that a \c Single selection list box will automatically select
    an item if it has keyboard focus.
*/

void Q3ListBox::clearSelection()
{
    selectAll(false);
}

/*!
    In \c Multi and \c Extended modes, this function sets all items to
    be selected if \a select is true, and to be unselected if \a
    select is false.

    In \c Single and \c NoSelection modes, this function only changes
    the selection status of currentItem().
*/

void Q3ListBox::selectAll(bool select)
{
    if (selectionMode() == Multi || selectionMode() == Extended) {
        bool b = signalsBlocked();
        blockSignals(true);
        for (int i = 0; i < (int)count(); i++)
            setSelected(i, select);
        blockSignals(b);
        emit selectionChanged();
    } else if (d->current) {
        Q3ListBoxItem * i = d->current;
        setSelected(i, select);
    }
}

/*!
    Inverts the selection. Only works in \c Multi and \c Extended
    selection mode.
*/

void Q3ListBox::invertSelection()
{
    if (d->selectionMode == Single ||
         d->selectionMode == NoSelection)
        return;

    bool b = signalsBlocked();
    blockSignals(true);
    for (int i = 0; i < (int)count(); i++)
        setSelected(i, !item(i)->isSelected());
    blockSignals(b);
    emit selectionChanged();
}


/*!
  Not used anymore; provided for compatibility.
*/

void Q3ListBox::emitChangedSignal(bool)
{
}


/*! \reimp */

QSize Q3ListBox::sizeHint() const
{
    if (cachedSizeHint().isValid())
        return cachedSizeHint();

    ensurePolished();
    doLayout();

    int i=0;
    while(i < 10 &&
           i < (int)d->columnPos.size()-1 &&
           d->columnPos[i] < 200)
        i++;
    int x;
    x = qMin(200, d->columnPos[i] +
              2 * style()->pixelMetric(QStyle::PM_DefaultFrameWidth));
    x = qMax(40, x);

    i = 0;
    while(i < 10 &&
           i < (int)d->rowPos.size()-1 &&
           d->rowPos[i] < 200)
        i++;
    int y;
    y = qMin(200, d->rowPos[i] +
              2 * style()->pixelMetric(QStyle::PM_DefaultFrameWidth));
    y = qMax(40, y);

    QSize s(x, y);
    setCachedSizeHint(s);
    return s;
}

/*!
  \reimp
*/

QSize Q3ListBox::minimumSizeHint() const
{
    return Q3ScrollView::minimumSizeHint();
}


/*!
    Ensures that a single paint event will occur at the end of the
    current event loop iteration. If \a doLayout is true, the layout
    is also redone.
*/

void Q3ListBox::triggerUpdate(bool doLayout)
{
    if (doLayout)
        d->layoutDirty = d->mustPaintAll = true;
    d->updateTimer->start(0, true);
}


void Q3ListBox::setColumnMode(LayoutMode mode)
{
    if (mode == Variable)
        return;
    d->rowModeWins = false;
    d->columnMode = mode;
    triggerUpdate(true);
}


void Q3ListBox::setColumnMode(int columns)
{
    if (columns < 1)
        columns = 1;
    d->columnMode = FixedNumber;
    d->numColumns = columns;
    d->rowModeWins = false;
    triggerUpdate(true);
}

void Q3ListBox::setRowMode(LayoutMode mode)
{
    if (mode == Variable)
        return;
    d->rowModeWins = true;
    d->rowMode = mode;
    triggerUpdate(true);
}


void Q3ListBox::setRowMode(int rows)
{
    if (rows < 1)
        rows = 1;
    d->rowMode = FixedNumber;
    d->numRows = rows;
    d->rowModeWins = true;
    triggerUpdate(true);
}

/*!
    \property Q3ListBox::columnMode
    \brief the column layout mode for this list box.

    setColumnMode() sets the layout mode and adjusts the number of
    displayed columns. The row layout mode automatically becomes \c
    Variable, unless the column mode is \c Variable.

    \sa setRowMode() rowMode numColumns
*/


Q3ListBox::LayoutMode Q3ListBox::columnMode() const
{
    if (d->rowModeWins)
        return Variable;
    else
        return d->columnMode;
}


/*!
    \property Q3ListBox::rowMode
    \brief the row layout mode for this list box

    This property is normally \c Variable.

    setRowMode() sets the layout mode and adjusts the number of
    displayed rows. The column layout mode automatically becomes \c
    Variable, unless the row mode is \c Variable.

    \sa columnMode
*/


Q3ListBox::LayoutMode Q3ListBox::rowMode() const
{
    if (d->rowModeWins)
        return d->rowMode;
    else
        return Variable;
}


/*!
    \property Q3ListBox::numColumns
    \brief the number of columns in the list box

    This is normally 1, but can be different if \l
    Q3ListBox::columnMode or \l Q3ListBox::rowMode has been set.

    \sa columnMode rowMode numRows
*/

int Q3ListBox::numColumns() const
{
    if (count() == 0)
        return 0;
    if (!d->rowModeWins && d->columnMode == FixedNumber)
        return d->numColumns;
    doLayout();
    return d->columnPos.size()-1;
}


/*!
    \property Q3ListBox::numRows
    \brief the number of rows in the list box.

    This is equal to the number of items in the default single-column
    layout, but can be different.

    \sa columnMode rowMode numColumns
*/

int Q3ListBox::numRows() const
{
    if (count() == 0)
        return 0;
    if (d->rowModeWins && d->rowMode == FixedNumber)
        return d->numRows;
    doLayout();
    return d->rowPos.size()-1;
}


/*!
    This function does the hard layout work. You should never need to
    call it.
*/

void Q3ListBox::doLayout() const
{
    if (!d->layoutDirty || d->resizeTimer->isActive())
        return;
    ensurePolished();
    int c = count();
    switch(rowMode()) {
    case FixedNumber:
        // columnMode() is known to be Variable
        tryGeometry(d->numRows, (c+d->numRows-1)/d->numRows);
        break;
    case FitToHeight:
        // columnMode() is known to be Variable
        if (d->head) {
            // this is basically the FitToWidth code, but edited to use rows.
            int maxh = 0;
            Q3ListBoxItem * i = d->head;
            while (i) {
                int h = i->height(this);
                if (maxh < h)
                    maxh = h;
                i = i->n;
            }
            int vh = viewportSize(1, 1).height();
            do {
                int rows = vh / maxh;
                if (rows > c)
                    rows = c;
                if (rows < 1)
                    rows = 1;
                if (variableHeight() && rows < c) {
                    do {
                        ++rows;
                        tryGeometry(rows, (c+rows-1)/rows);
                    } while (rows <= c &&
                              d->rowPos[(int)d->rowPos.size()-1] <= vh);
                    --rows;
                }
                tryGeometry(rows, (c+rows-1)/rows);
                int nvh = viewportSize(d->columnPos[(int)d->columnPos.size()-1],
                                        d->rowPos[(int)d->rowPos.size()-1]).height();
                if (nvh < vh)
                    vh = nvh;
            } while (d->rowPos.size() > 2 &&
                      vh < d->rowPos[(int)d->rowPos.size()-1]);
        } else {
            tryGeometry(1, 1);
        }
        break;
    case Variable:
        if (columnMode() == FixedNumber) {
            tryGeometry((count()+d->numColumns-1)/d->numColumns,
                         d->numColumns);
        } else if (d->head) { // FitToWidth, at least one item
            int maxw = 0;
            Q3ListBoxItem * i = d->head;
            while (i) {
                int w = i->width(this);
                if (maxw < w)
                    maxw = w;
                i = i->n;
            }
            int vw = viewportSize(1, 1).width();
            do {
                int cols = vw / maxw;
                if (cols > c)
                    cols = c;
                if (cols < 1)
                    cols = 1;
                if (variableWidth() && cols < c) {
                    do {
                        ++cols;
                        tryGeometry((c+cols-1)/cols, cols);
                    } while (cols <= c &&
                              d->columnPos[(int)d->columnPos.size()-1] <= vw);
                    --cols;
                }
                tryGeometry((c+cols-1)/cols, cols);
                int nvw = viewportSize(d->columnPos[(int)d->columnPos.size()-1],
                                        d->rowPos[(int)d->rowPos.size()-1]).width();
                if (nvw < vw)
                    vw = nvw;
            } while (d->columnPos.size() > 2 &&
                      vw < d->columnPos[(int)d->columnPos.size()-1]);
        } else {
            tryGeometry(1, 1);
        }
        break;
    }

    d->layoutDirty = false;
    int w = d->columnPos[(int)d->columnPos.size()-1];
    int h = d->rowPos[(int)d->rowPos.size()-1];
    QSize s(viewportSize(w, h));
    w = qMax(w, s.width());

    d->columnPosOne = d->columnPos[1];
    // extend the column for simple single-column listboxes
    if (columnMode() == FixedNumber && d->numColumns == 1 &&
         d->columnPos[1] < w)
        d->columnPos[1] = w;
    ((Q3ListBox *)this)->resizeContents(w, h);
}


/*!
    Lay the items out in a \a columns by \a rows array. The array may
    be too big: doLayout() is expected to call this with the right
    values.
*/

void Q3ListBox::tryGeometry(int rows, int columns) const
{
    if (columns < 1)
        columns = 1;
    d->columnPos.resize(columns+1);

    if (rows < 1)
        rows = 1;
    d->rowPos.resize(rows+1);

    // funky hack I: dump the height/width of each column/row in
    // {column,row}Pos for later conversion to positions.
    int c;
    for(c=0; c<=columns; c++)
        d->columnPos[c] = 0;
    int r;
    for(r=0; r<=rows; r++)
        d->rowPos[r] = 0;
    r = c = 0;
    Q3ListBoxItem * i = d->head;
    while (i && c < columns) {
        if (i == d->current) {
            d->currentRow = r;
            d->currentColumn = c;
        }

        int w = i->width(this);
        if (d->columnPos[c] < w)
            d->columnPos[c] = w;
        int h = i->height(this);
        if (d->rowPos[r] < h)
            d->rowPos[r] = h;
        i = i->n;
        r++;
        if (r == rows) {
            r = 0;
            c++;
        }
    }
    // funky hack II: if not variable {width,height}, unvariablify it.
    if (!variableWidth()) {
        int w = 0;
        for(c=0; c<columns; c++)
            if (w < d->columnPos[c])
                w = d->columnPos[c];
        for(c=0; c<columns; c++)
            d->columnPos[c] = w;
    }
    if (!variableHeight()) {
        int h = 0;
        for(r=0; r<rows; r++)
            if (h < d->rowPos[r])
                h = d->rowPos[r];
        for(r=0; r<rows; r++)
            d->rowPos[r] = h;
    }
    // repair the hacking.
    int x = 0;
    for(c=0; c<=columns; c++) {
        int w = d->columnPos[c];
        d->columnPos[c] = x;
        x += w;
    }
    int y = 0;
    for(r=0; r<=rows; r++) {
        int h = d->rowPos[r];
        d->rowPos[r] = y;
        y += h;
    }
}


/*!
    Returns the row index of the current item, or -1 if no item is the
    current item.
*/

int Q3ListBox::currentRow() const
{
    if (!d->current)
        return -1;
    if (d->currentRow < 0)
        d->layoutDirty = true;
    if (d->layoutDirty)
        doLayout();
    return d->currentRow;
}


/*!
    Returns the column index of the current item, or -1 if no item is
    the current item.
*/

int Q3ListBox::currentColumn() const
{
    if (!d->current)
        return -1;
    if (d->currentColumn < 0)
        d->layoutDirty = true;
    if (d->layoutDirty)
        doLayout();
    return d->currentColumn;
}


void Q3ListBox::setTopItem(int index)
{
    if (index >= (int)count() || count() == 0)
        return;
    int col = index / numRows();
    int y = d->rowPos[index-col*numRows()];
    if (d->columnPos[col] >= contentsX() &&
         d->columnPos[col+1] <= contentsX() + visibleWidth())
        setContentsPos(contentsX(), y);
    else
        setContentsPos(d->columnPos[col], y);
}

/*!
    Scrolls the list box so the item at position \a index in the list
    is displayed in the bottom row of the list box.

    \sa setTopItem()
*/

void Q3ListBox::setBottomItem(int index)
{
    if (index >= (int)count() || count() == 0)
        return;
    int col = index / numRows();
    int y = d->rowPos[1+index-col*numRows()] - visibleHeight();
    if (y < 0)
        y = 0;
    if (d->columnPos[col] >= contentsX() &&
         d->columnPos[col+1] <= contentsX() + visibleWidth())
        setContentsPos(contentsX(), y);
    else
        setContentsPos(d->columnPos[col], y);
}


/*!
    Returns the item at point \a p, specified in viewport coordinates,
    or a 0 if there is no item at \a p.

    Use contentsToViewport() to convert between widget coordinates and
    viewport coordinates.
*/

Q3ListBoxItem * Q3ListBox::itemAt(const QPoint& p) const
{
    if (d->layoutDirty)
        doLayout();
    QPoint np = p;

    np -= viewport()->pos();
    if (!viewport()->rect().contains(np))
        return 0;

    // take into account contents position
    np = viewportToContents(np);

    int x = np.x();
    int y = np.y();

    // return 0 when y is below the last row
    if (y > d->rowPos[numRows()])
        return 0;

    int col = columnAt(x);
    int row = rowAt(y);

    Q3ListBoxItem *i = item(col * numRows()  + row);
    if (i && numColumns() > 1) {
        if (d->columnPos[col] + i->width(this) >= x)
            return i;
    } else {
        if (d->columnPos[col + 1] >= x)
            return i;
    }
    return 0;
}


/*!
    Ensures that the current item is visible.
*/

void Q3ListBox::ensureCurrentVisible()
{
    if (!d->current)
        return;

    doLayout();

    int row = currentRow();
    int column = currentColumn();
    int w = (d->columnPos[column+1] - d->columnPos[column]) / 2;
    int h = (d->rowPos[row+1] - d->rowPos[row]) / 2;
    // next four lines are Bad.  they mean that for pure left-to-right
    // languages, textual list box items are displayed better than
    // before when there is little space.  for non-textual items, or
    // other languages, it means... that you really should have enough
    // space in the first place :)
    if (numColumns() == 1)
        w = 0;
    if (w*2 > viewport()->width())
        w = viewport()->width()/2;

    ensureVisible(d->columnPos[column] + w, d->rowPos[row] + h, w, h);
}


/*! \internal */

void Q3ListBox::doAutoScroll()
{
    if (d->scrollPos.x() < 0) {
        // scroll left
        int x = contentsX() - horizontalScrollBar()->singleStep();
        if (x < 0)
            x = 0;
        if (x != contentsX()) {
            d->mouseMoveColumn = columnAt(x);
            updateSelection();
            if (x < contentsX())
                setContentsPos(x, contentsY());
        }
    } else if (d->scrollPos.x() > 0) {
        // scroll right
        int x = contentsX() + horizontalScrollBar()->singleStep();
        if (x + visibleWidth() > contentsWidth())
            x = contentsWidth() - visibleWidth();
        if (x != contentsX()) {
            d->mouseMoveColumn = columnAt(x + visibleWidth() - 1);
            updateSelection();
            if (x > contentsX())
                setContentsPos(x, contentsY());
        }
    }

    if (d->scrollPos.y() < 0) {
        // scroll up
        int y = contentsY() - verticalScrollBar()->singleStep();
        if (y < 0)
            y = 0;
        if (y != contentsY()) {
            y = contentsY() - verticalScrollBar()->singleStep();
            d->mouseMoveRow = rowAt(y);
            updateSelection();
        }
    } else if (d->scrollPos.y() > 0) {
        // scroll down
        int y = contentsY() + verticalScrollBar()->singleStep();
        if (y + visibleHeight() > contentsHeight())
            y = contentsHeight() - visibleHeight();
        if (y != contentsY()) {
            y = contentsY() + verticalScrollBar()->singleStep();
            d->mouseMoveRow = rowAt(y + visibleHeight() - 1);
            updateSelection();
        }
    }

    if (d->scrollPos == QPoint(0, 0)) {
        delete d->scrollTimer;
        d->scrollTimer = 0;
    }
}


/*!
    \property Q3ListBox::topItem
    \brief the index of an item at the top of the screen.

    When getting this property and the listbox has multiple columns,
    an arbitrary item is selected and returned.

    When setting this property, the list box is scrolled so the item
    at position \e index in the list is displayed in the top row of
    the list box.
*/

int Q3ListBox::topItem() const
{
    doLayout();

    // move rightwards to the best column
    int col = columnAt(contentsX());
    int row = rowAt(contentsY());
    return col * numRows() + row;
}


/*!
    \property Q3ListBox::variableHeight
    \brief whether this list box has variable-height rows

    When the list box has variable-height rows (the default), each row
    is as high as the highest item in that row. When it has same-sized
    rows, all rows are as high as the highest item in the list box.

    \sa variableWidth
*/

bool Q3ListBox::variableHeight() const
{
    return d->variableHeight;
}


void Q3ListBox::setVariableHeight(bool enable)
{
    if ((bool)d->variableHeight == enable)
        return;

    d->variableHeight = enable;
    triggerUpdate(true);
}


/*!
    \property Q3ListBox::variableWidth
    \brief whether this list box has variable-width columns

    When the list box has variable-width columns, each column is as
    wide as the widest item in that column. When it has same-sized
    columns (the default), all columns are as wide as the widest item
    in the list box.

    \sa variableHeight
*/

bool Q3ListBox::variableWidth() const
{
    return d->variableWidth;
}


void Q3ListBox::setVariableWidth(bool enable)
{
    if ((bool)d->variableWidth == enable)
        return;

    d->variableWidth = enable;
    triggerUpdate(true);
}


/*!
    Repaints only what really needs to be repainted.
*/
void Q3ListBox::refreshSlot()
{
    if (d->mustPaintAll ||
         d->layoutDirty) {
        d->mustPaintAll = false;
        bool currentItemVisible = itemVisible(currentItem());
        doLayout();
        if (hasFocus() &&
             currentItemVisible &&
             d->currentColumn >= 0 &&
             d->currentRow >= 0 &&
             (d->columnPos[d->currentColumn] < contentsX() ||
               d->columnPos[d->currentColumn+1]>contentsX()+visibleWidth() ||
               d->rowPos[d->currentRow] < contentsY() ||
               d->rowPos[d->currentRow+1] > contentsY()+visibleHeight()))
            ensureCurrentVisible();
        viewport()->repaint();
        return;
    }

    QRegion r;
    int x = contentsX();
    int y = contentsY();
    int col = columnAt(x);
    int row = rowAt(y);
    int top = row;
    while(col < (int)d->columnPos.size()-1 && d->columnPos[col+1] < x)
        col++;
    while(top < (int)d->rowPos.size()-1 && d->rowPos[top+1] < y)
        top++;
    Q3ListBoxItem * i = item(col * numRows() + row);

    while (i && (int)col < numColumns() &&
            d->columnPos[col] < x + visibleWidth() ) {
        int cw = d->columnPos[col+1] - d->columnPos[col];
        while (i && row < numRows() && d->rowPos[row] <
                y + visibleHeight()) {
            if (i->dirty)
                r = r.united(QRect(d->columnPos[col] - x, d->rowPos[row] - y,
                                   cw, d->rowPos[row+1] - d->rowPos[row]));
            row++;
            i = i->n;
        }
        col++;
        if (numColumns() > 1) {
            row = top;
            i = item(col *  numRows() + row);
        }
    }

    if (r.isEmpty())
        viewport()->repaint();
    else
        viewport()->repaint(r);
}


/*! \reimp */

void Q3ListBox::viewportPaintEvent(QPaintEvent * e)
{
    doLayout();
    QWidget* vp = viewport();
    QPainter p(vp);
    QRegion r = e->region();

#if 0
    {
        // this stuff has been useful enough times that from now I'm
        //  leaving it in the source.
        uint i = 0;
        qDebug("%s/%s: %i rects", className(), name(), r.rects().size());
        while(i < r.rects().size()) {
            qDebug("rect %d: %d, %d, %d, %d", i,
                   r.rects()[i].left(), r.rects()[i].top(),
                   r.rects()[i].width(), r.rects()[i].height());
            i++;
        }
        qDebug("");
    }
#endif

    int x = contentsX();
    int y = contentsY();
    int w = vp->width();
    int h = vp->height();

    int col = columnAt(x);
    int top = rowAt(y);
    int row = top;

    Q3ListBoxItem * i = item(col*numRows() + row);

    const QPalette &pal = palette();
    p.setPen(pal.text().color());
    p.setBackground(palette().brush(backgroundRole()).color());
    while (i && (int)col < numColumns() && d->columnPos[col] < x + w) {
        int cw = d->columnPos[col+1] - d->columnPos[col];
        while (i && (int)row < numRows() && d->rowPos[row] < y + h) {
            int ch = d->rowPos[row+1] - d->rowPos[row];
            QRect itemRect(d->columnPos[col]-x, d->rowPos[row]-y, cw, ch);
            QRegion tempRegion(itemRect);
            QRegion itemPaintRegion(tempRegion.intersected(r ));
            if (!itemPaintRegion.isEmpty()) {
                p.save();
                p.setClipRegion(itemPaintRegion);
                p.translate(d->columnPos[col]-x, d->rowPos[row]-y);
                paintCell(&p, row, col);
                p.restore();
                r = r.subtracted(itemPaintRegion);
            }
            row++;
            if (i->dirty) {
                // reset dirty flag only if the entire item was painted
                if (itemPaintRegion == QRegion(itemRect))
                    i->dirty = false;
            }
            i = i->n;
        }
        col++;
        if (numColumns() > 1) {
            row = top;
            i = item(col *  numRows() + row);
        }
    }

    if (r.isEmpty())
        return;
    p.setClipRegion(r);
    p.fillRect(0, 0, w, h, viewport()->palette().brush(viewport()->backgroundRole()));

    if(d->rubber && d->rubber->width() && d->rubber->height()) {
        p.save();
        p.setClipping(false);
        // p.setRasterOp(NotROP); // ### fix - use qrubberband instead
        QStyleOptionRubberBand opt;
        opt.rect = d->rubber->normalized();
        opt.palette = palette();
        opt.shape = QRubberBand::Rectangle;
        opt.opaque = false;
        style()->drawControl(QStyle::CE_RubberBand, &opt, &p, this);
        p.restore();
    }
}


/*!
    Returns the height in pixels of the item with index \a index. \a
    index defaults to 0.

    If \a index is too large, this function returns 0.
*/

int Q3ListBox::itemHeight(int index) const
{
    if (index >= (int)count() || index < 0)
        return 0;
    int r = index % numRows();
    return d->rowPos[r+1] - d->rowPos[r];
}


/*!
    Returns the index of the column at \a x, which is in the listbox's
    coordinates, not in on-screen coordinates.

    If there is no column that spans \a x, columnAt() returns -1.
*/

int Q3ListBox::columnAt(int x) const
{
    if (x < 0)
        return -1;
    if (!d->columnPos.size())
        return -1;
    if (x >= d->columnPos[(int)d->columnPos.size()-1])
        return numColumns() - 1;

    int col = 0;
    while(col < (int)d->columnPos.size()-1 && d->columnPos[col+1] < x)
        col++;
    return col;
}


/*!
    Returns the index of the row at \a y, which is in the listbox's
    coordinates, not in on-screen coordinates.

    If there is no row that spans \a y, rowAt() returns -1.
*/

int Q3ListBox::rowAt(int y) const
{
    if (y < 0)
        return -1;

    // find the top item, use bsearch for speed
    int l = 0;
    int r = d->rowPos.size() - 2;
    if (r < 0)
        return -1;
    if (l <= d->rowPosCache && d->rowPosCache <= r) {
        if (d->rowPos[qMax(l, d->rowPosCache - 10)] <= y
             && y <= d->rowPos[qMin(r, d->rowPosCache + 10)]) {
            l = qMax(l, d->rowPosCache - 10);
            r = qMin(r, d->rowPosCache + 10);
        }
    }
    int i = ((l+r+1) / 2);
    while (r - l) {
        if (d->rowPos[i] > y)
            r = i -1;
        else
            l = i;
        i = ((l+r+1) / 2);
    }
    d->rowPosCache = i;
    if (d->rowPos[i] <= y && y <= d->rowPos[i+1] )
        return  i;

    return d->count - 1;
}


/*!
    Returns the rectangle on the screen that \a item occupies in
    viewport()'s coordinates, or an invalid rectangle if \a item is 0
    or is not currently visible.
*/

QRect Q3ListBox::itemRect(Q3ListBoxItem *item) const
{
    if (d->resizeTimer->isActive())
        return QRect(0, 0, -1, -1);
    if (!item)
        return QRect(0, 0, -1, -1);

    int i = index(item);
    if (i == -1)
        return QRect(0, 0, -1, -1);
    
    int col = i / numRows();
    int row = i % numRows();

    int x = d->columnPos[col] - contentsX();
    int y = d->rowPos[row] - contentsY();

    QRect r(x, y, d->columnPos[col + 1] - d->columnPos[col],
                  d->rowPos[row + 1] - d->rowPos[row]);
    if (r.intersects(QRect(0, 0, visibleWidth(), visibleHeight())))
        return r;
    return QRect(0, 0, -1, -1);
}


/*!
  Using this method is quite inefficient. We suggest to use insertItem()
  for inserting and sort() afterwards.

  Inserts \a lbi at its sorted position in the list box and returns the
  position.

  All items must be inserted with inSort() to maintain the sorting
  order. inSort() treats any pixmap (or user-defined type) as
  lexicographically less than any string.

  \sa insertItem(), sort()
*/
int Q3ListBox::inSort(const Q3ListBoxItem * lbi)
{
    if (!lbi)
        return -1;

    Q3ListBoxItem * i = d->head;
    int c = 0;

    while(i && i->text() < lbi->text()) {
        i = i->n;
        c++;
    }
    insertItem(lbi, c);
    return c;
}

/*!
  \overload
  Using this method is quite inefficient. We suggest to use insertItem()
  for inserting and sort() afterwards.

  Inserts a new item of \a text at its sorted position in the list box and
  returns the position.

  All items must be inserted with inSort() to maintain the sorting
  order. inSort() treats any pixmap (or user-defined type) as
  lexicographically less than any string.

  \sa insertItem(), sort()
*/
int Q3ListBox::inSort(const QString& text)
{
    Q3ListBoxItem *lbi = new Q3ListBoxText(text);

    Q3ListBoxItem * i = d->head;
    int c = 0;

    while(i && i->text() < lbi->text()) {
        i = i->n;
        c++;
    }
    insertItem(lbi, c);
    return c;
}


/*! \reimp */

void Q3ListBox::resizeEvent(QResizeEvent *e)
{
    d->layoutDirty = (d->layoutDirty ||
                       rowMode() == FitToHeight ||
                       columnMode() == FitToWidth);

    if (!d->layoutDirty && columnMode() == FixedNumber &&
         d->numColumns == 1) {
        int w = d->columnPosOne;
        QSize s(viewportSize(w, contentsHeight()));
        w = qMax(w, s.width());
        d->columnPos[1] = qMax(w, d->columnPosOne);
        resizeContents(d->columnPos[1], contentsHeight());
    }

    if (d->resizeTimer->isActive())
        d->resizeTimer->stop();
    if (d->rowMode == FixedNumber && d->columnMode == FixedNumber) {
        bool currentItemVisible = itemVisible(currentItem());
        doLayout();
        Q3ScrollView::resizeEvent(e);
        if (currentItemVisible)
            ensureCurrentVisible();
        if (d->current)
            viewport()->repaint(itemRect(d->current));
    } else if ((d->columnMode == FitToWidth || d->rowMode == FitToHeight) && !(isVisible())) {
        Q3ScrollView::resizeEvent(e);
    } else if (d->layoutDirty) {
        d->resizeTimer->start(100, true);
        resizeContents(contentsWidth() - (e->oldSize().width() - e->size().width()),
                        contentsHeight() - (e->oldSize().height() - e->size().height()));
        Q3ScrollView::resizeEvent(e);
    } else {
        Q3ScrollView::resizeEvent(e);
    }
}

/*!
  \internal
*/

void Q3ListBox::adjustItems()
{
    triggerUpdate(true);
    ensureCurrentVisible();
}


/*!
    Provided for compatibility with the old Q3ListBox. We recommend
    using Q3ListBoxItem::paint() instead.

    Repaints the cell at \a row, \a col using painter \a p.
*/

void Q3ListBox::paintCell(QPainter * p, int row, int col)
{
    bool drawActiveSelection = hasFocus() || d->inMenuMode ||
        !style()->styleHint(QStyle::SH_ItemView_ChangeHighlightOnFocus, 0, this);
    QPalette pal = palette();
    if(!drawActiveSelection)
        pal.setCurrentColorGroup(QPalette::Inactive);

    int cw = d->columnPos[col+1] - d->columnPos[col];
    int ch = d->rowPos[row+1] - d->rowPos[row];
    Q3ListBoxItem * i = item(col*numRows()+row);
    p->save();
    if (i->s) {
        if (i->custom_highlight) {
            p->fillRect(0, 0, cw, ch, pal.brush(viewport()->foregroundRole()));
            p->setPen(pal.highlightedText().color());
            p->setBackground(pal.highlight());
        } else if (numColumns()  == 1) {
            p->fillRect(0, 0, cw, ch, pal.brush(QPalette::Highlight));
            p->setPen(pal.highlightedText().color());
            p->setBackground(pal.highlight());
        } else {
            int iw = i->width(this);
            p->fillRect(0, 0, iw, ch, pal.brush(QPalette::Highlight));
            p->fillRect(iw, 0, cw - iw + 1, ch, viewport()->palette().brush(viewport()->backgroundRole()));
            p->setPen(pal.highlightedText().color());
            p->setBackground(pal.highlight());
        }
    } else {
        p->fillRect(0, 0, cw, ch, viewport()->palette().brush(viewport()->backgroundRole()));
    }

    i->paint(p);

    if (d->current == i && hasFocus() && !i->custom_highlight) {
        if (numColumns() > 1)
            cw = i->width(this);
        QStyleOptionFocusRect opt;
        opt.rect.setRect(0, 0, cw, ch);
        opt.palette = pal;
        opt.state = QStyle::State_FocusAtBorder;
        if (i->isSelected())
            opt.backgroundColor = pal.highlight().color();
        else
            opt.backgroundColor = pal.base().color();
        style()->drawPrimitive(QStyle::PE_FrameFocusRect, &opt, p, this);
    }

    p->restore();
}

/*!
    Returns the width of the widest item in the list box.
*/

long Q3ListBox::maxItemWidth() const
{
    if (d->layoutDirty)
        doLayout();
    long m = 0;
    int i = d->columnPos.size();
    while(i--)
        if (m < d->columnPos[i])
            m = d->columnPos[i];
    return m;
}


/*! \reimp */

void Q3ListBox::showEvent(QShowEvent *)
{
    d->ignoreMoves = false;
    d->mousePressRow = -1;
    d->mousePressColumn = -1;
    d->mustPaintAll = false;
    ensureCurrentVisible();
}

/*!
    \fn bool Q3ListBoxItem::isSelected() const

    Returns true if the item is selected; otherwise returns false.

    \sa Q3ListBox::isSelected(), isCurrent()
*/

/*!
    Returns true if the item is the current item; otherwise returns
    false.

    \sa Q3ListBox::currentItem(), Q3ListBox::item(), isSelected()
*/
bool Q3ListBoxItem::isCurrent() const
{
    return listBox() && listBox()->hasFocus() &&
        listBox()->item(listBox()->currentItem()) == this;
}

/*!
    \fn void Q3ListBox::centerCurrentItem()

    If there is a current item, the list box is scrolled so that this
    item is displayed centered.

    \sa Q3ListBox::ensureCurrentVisible()
*/

/*!
    Returns a pointer to the list box containing this item.
*/

Q3ListBox * Q3ListBoxItem::listBox() const
{
    return lbox;
}


/*!
    Removes \a item from the list box and causes an update of the
    screen display. The item is not deleted. You should normally not
    need to call this function because Q3ListBoxItem::~Q3ListBoxItem()
    calls it. The normal way to delete an item is with \c delete.

    \sa Q3ListBox::insertItem()
*/
void Q3ListBox::takeItem(const Q3ListBoxItem * item)
{
    if (!item || d->clearing)
        return;
    d->cache = 0;
    d->count--;
    if (item == d->last)
        d->last = d->last->p;
    if (item->p && item->p->n == item)
        item->p->n = item->n;
    if (item->n && item->n->p == item)
        item->n->p = item->p;
    if (d->head == item) {
        d->head = item->n;
        d->currentColumn = d->currentRow = -1;
    }

    if (d->current == item) {
        d->current = item->n ? item->n : item->p;
        Q3ListBoxItem *i = d->current;
        QString tmp;
        if (i)
            tmp = i->text();
        int tmp2 = index(i);
        emit highlighted(i);
        if (!tmp.isNull())
            emit highlighted(tmp);
        emit highlighted(tmp2);
        emit currentChanged(i);
    }
    if (d->tmpCurrent == item)
        d->tmpCurrent = d->current;
    if (d->selectAnchor == item)
        d->selectAnchor = d->current;

    if (item->s)
        emit selectionChanged();
    ((Q3ListBoxItem *)item)->lbox = 0;
    triggerUpdate(true);
}

/*!
  \internal
  Finds the next item after start beginning with \a text.
*/

int Q3ListBoxPrivate::findItemByName(int start, const QString &text)
{
    if (start < 0 || (uint)start >= listBox->count())
        start = 0;
    QString match = text.toLower();
    if (match.length() < 1)
        return start;
    QString curText;
    int item = start;
    do {
        curText = listBox->text(item).toLower();
        if (curText.startsWith(match))
            return item;
        item++;
        if ((uint)item == listBox->count())
            item = 0;
    } while (item != start);
    return -1;
}

/*!
  \internal
*/

void Q3ListBox::clearInputString()
{
    d->currInputString.clear();
}

/*!
    Finds the first list box item that has the text \a text and
    returns it, or returns 0 of no such item could be found. If \c
    ComparisonFlags are specified in \a compare then these flags
    are used, otherwise the default is a case-insensitive, "begins
    with" search.
*/

Q3ListBoxItem *Q3ListBox::findItem(const QString &text, ComparisonFlags compare) const
{
    if (text.isEmpty())
        return 0;

    if (compare == CaseSensitive || compare == 0)
        compare |= ExactMatch;

    QString itmtxt;
    QString comtxt = text;
    if (!(compare & CaseSensitive))
        comtxt = text.toLower();

    Q3ListBoxItem *item;
    if (d->current)
        item = d->current;
    else
        item = d->head;

    Q3ListBoxItem *beginsWithItem = 0;
    Q3ListBoxItem *endsWithItem = 0;
    Q3ListBoxItem *containsItem = 0;

    if (item) {
        for (; item; item = item->n) {
            if (!(compare & CaseSensitive))
                itmtxt = item->text().toLower();
            else
                itmtxt = item->text();

            if ((compare & ExactMatch)==ExactMatch && itmtxt == comtxt)
                return item;
            if (compare & BeginsWith && !beginsWithItem && itmtxt.startsWith(comtxt))
                beginsWithItem = containsItem = item;
            if (compare & EndsWith && !endsWithItem && itmtxt.endsWith(comtxt))
                endsWithItem = containsItem = item;
            if ((compare & ExactMatch)==0 && !containsItem && itmtxt.contains(comtxt))
                containsItem = item;
        }

        if (d->current && d->head) {
            item = d->head;
            for (; item && item != d->current; item = item->n) {
                if (!(compare & CaseSensitive))
                    itmtxt = item->text().toLower();
                else
                    itmtxt = item->text();

                if ((compare & ExactMatch)==ExactMatch && itmtxt == comtxt)
                    return item;
                if (compare & BeginsWith && !beginsWithItem && itmtxt.startsWith(comtxt))
                    beginsWithItem = containsItem = item;
                if (compare & EndsWith && !endsWithItem && itmtxt.endsWith(comtxt))
                    endsWithItem = containsItem = item;
                if ((compare & ExactMatch)==0 && !containsItem && itmtxt.contains(comtxt))
                    containsItem = item;
            }
        }
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
  \internal
*/

void Q3ListBox::drawRubber()
{
    if (!d->rubber)
        return;
    if (!d->rubber->width() && !d->rubber->height())
        return;
    update();
}

/*!
  \internal
*/

void Q3ListBox::doRubberSelection(const QRect &old, const QRect &rubber)
{
    Q3ListBoxItem *i = d->head;
    QRect ir, pr;
    bool changed = false;
    for (; i; i = i->n) {
        ir = itemRect(i);
        if (ir == QRect(0, 0, -1, -1))
            continue;
        if (i->isSelected() && !ir.intersects(rubber) && ir.intersects(old)) {
            i->s = false;
            pr = pr.united(ir);
            changed = true;
        } else if (!i->isSelected() && ir.intersects(rubber)) {
            if (i->isSelectable()) {
                i->s = true;
                pr = pr.united(ir);
                changed = true;
            }
        }
    }
    if (changed) {
        emit selectionChanged();
#ifndef QT_NO_ACCESSIBILITY
        QAccessible::updateAccessibility(viewport(), 0, QAccessible::Selection);
#endif
    }
    viewport()->repaint(pr);
}


/*!
    Returns true if the user is selecting items using a rubber band
    rectangle; otherwise returns false.
*/

bool Q3ListBox::isRubberSelecting() const
{
    return d->rubber != 0;
}


/*!
    Returns the item that comes after this in the list box. If this is
    the last item, 0 is returned.

    \sa prev()
*/

Q3ListBoxItem *Q3ListBoxItem::next() const
{
    return n;
}

/*!
    Returns the item which comes before this in the list box. If this
    is the first item, 0 is returned.

    \sa next()
*/

Q3ListBoxItem *Q3ListBoxItem::prev() const
{
    return p;
}

/*!
    Returns the first item in this list box. If the list box is empty,
    returns 0.
*/

Q3ListBoxItem *Q3ListBox::firstItem() const
{
    return d->head;
}

#if defined(Q_C_CALLBACKS)
extern "C" {
#endif

#ifdef Q_OS_WINCE
static int _cdecl cmpListBoxItems(const void *n1, const void *n2)
#else
static int cmpListBoxItems(const void *n1, const void *n2)
#endif
{
    if (!n1 || !n2)
        return 0;

    Q3ListBoxPrivate::SortableItem *i1 = (Q3ListBoxPrivate::SortableItem *)n1;
    Q3ListBoxPrivate::SortableItem *i2 = (Q3ListBoxPrivate::SortableItem *)n2;

    return i1->item->text().localeAwareCompare(i2->item->text());
}

#if defined(Q_C_CALLBACKS)
}
#endif

/*!
    If \a ascending is true sorts the items in ascending order;
    otherwise sorts in descending order.

    To compare the items, the text (Q3ListBoxItem::text()) of the items
    is used.
*/

void Q3ListBox::sort(bool ascending)
{
    if (count() == 0)
        return;

    d->cache = 0;

    Q3ListBoxPrivate::SortableItem *items = new Q3ListBoxPrivate::SortableItem[count()];

    Q3ListBoxItem *item = d->head;
    int i = 0;
    for (; item; item = item->n)
        items[i++].item = item;

    qsort(items, count(), sizeof(Q3ListBoxPrivate::SortableItem), cmpListBoxItems);

    Q3ListBoxItem *prev = 0;
    item = 0;
    if (ascending) {
        for (i = 0; i < (int)count(); ++i) {
            item = items[i].item;
            if (item) {
                item->p = prev;
                item->dirty = true;
                if (item->p)
                    item->p->n = item;
                item->n = 0;
            }
            if (i == 0)
                d->head = item;
            prev = item;
        }
    } else {
        for (i = (int)count() - 1; i >= 0 ; --i) {
            item = items[i].item;
            if (item) {
                item->p = prev;
                item->dirty = true;
                if (item->p)
                    item->p->n = item;
                item->n = 0;
            }
            if (i == (int)count() - 1)
                d->head = item;
            prev = item;
        }
    }
    d->last = item;

    delete [] items;

    // We have to update explicitly in case the current "vieport" overlaps the
    // new viewport we set (starting at (0,0)).
    bool haveToUpdate = contentsX() < visibleWidth() || contentsY() < visibleHeight();
    setContentsPos(0, 0);
    if (haveToUpdate)
        updateContents(0, 0, visibleWidth(), visibleHeight());
}

void Q3ListBox::handleItemChange(Q3ListBoxItem *old, bool shift, bool control)
{
    if (d->selectionMode == Single) {
        // nothing
    } else if (d->selectionMode == Extended) {
        if (shift) {
            selectRange(d->selectAnchor ? d->selectAnchor : old,
                         d->current, false, true, (d->selectAnchor && !control) ? true : false);
        } else if (!control) {
            bool block = signalsBlocked();
            blockSignals(true);
            selectAll(false);
            blockSignals(block);
            setSelected(d->current, true);
        }
    } else if (d->selectionMode == Multi) {
        if (shift)
            selectRange(old, d->current, true, false);
    }
}

void Q3ListBox::selectRange(Q3ListBoxItem *from, Q3ListBoxItem *to, bool invert, bool includeFirst, bool clearSel)
{
    if (!from || !to)
        return;
    if (from == to && !includeFirst)
        return;
    Q3ListBoxItem *i = 0;
    int index =0;
    int f_idx = -1, t_idx = -1;
    for (i = d->head; i; i = i->n, index++) {
        if (i == from)
            f_idx = index;
        if (i == to)
            t_idx = index;
        if (f_idx != -1 && t_idx != -1)
            break;
    }
    if (f_idx > t_idx) {
        i = from;
        from = to;
        to = i;
        if (!includeFirst)
            to = to->prev();
    } else {
        if (!includeFirst)
            from = from->next();
    }

    bool changed = false;
    if (clearSel) {
        for (i = d->head; i && i != from; i = i->n) {
            if (i->s) {
                i->s = false;
                changed = true;
                updateItem(i);
            }
        }
        for (i = to->n; i; i = i->n) {
            if (i->s) {
                i->s = false;
                changed = true;
                updateItem(i);
            }
        }
    }

    for (i = from; i; i = i->next()) {
        if (!invert) {
            if (!i->s && i->isSelectable()) {
                i->s = true;
                changed = true;
                updateItem(i);
            }
        } else {
            bool sel = !i->s;
            if (((bool)i->s != sel && sel && i->isSelectable()) || !sel) {
                i->s = sel;
                changed = true;
                updateItem(i);
            }
        }
        if (i == to)
            break;
    }
    if (changed) {
        emit selectionChanged();
#ifndef QT_NO_ACCESSIBILITY
        QAccessible::updateAccessibility(viewport(), 0, QAccessible::Selection);
#endif
    }
}

/*! \reimp */
void Q3ListBox::changeEvent(QEvent *ev)
{
    if (ev->type() == QEvent::ActivationChange) {
        if (!isActiveWindow() && d->scrollTimer)
            d->scrollTimer->stop();
        if (!palette().isEqual(QPalette::Active, QPalette::Inactive))
            viewport()->update();
    }
    Q3ScrollView::changeEvent(ev);

    if (ev->type() == QEvent::ApplicationFontChange || ev->type() == QEvent::FontChange)
        triggerUpdate(true);
}

/*!
    Returns 0.

    Make your derived classes return their own values for rtti(), and
    you can distinguish between listbox items. You should use values
    greater than 1000 preferably a large random number, to allow for
    extensions to this class.
*/

int Q3ListBoxItem::rtti() const
{
    return RTTI;
}

/*!
    \fn bool Q3ListBox::dragSelect() const

    Returns true. Dragging always selects.
*/

/*!
    \fn void Q3ListBox::setDragSelect(bool b)

    Does nothing. Dragging always selects. The \a b parameter is ignored.
*/

/*!
    \fn bool Q3ListBox::autoScroll() const

    Use dragAutoScroll() instead. This function always returns true.
*/

/*!
    \fn void Q3ListBox::setAutoScroll(bool b)

    Use setDragAutoScroll(\a b) instead.
*/

/*!
    \fn bool Q3ListBox::autoScrollBar() const

    Use vScrollBarMode() instead. Returns true if the vertical
    scrollbar mode is \c Auto.
*/

/*!
    \fn void Q3ListBox::setAutoScrollBar(bool enable)

    Use setVScrollBarMode() instead.

    If \a enable is true, pass \c Auto as the argument to
    setVScrollBarMode(); otherwise, pass \c AlwaysOff.
*/

/*!
    \fn bool Q3ListBox::scrollBar() const

    Use vScrollBarMode() instead. Returns true if the vertical
    scrollbar mode is not \c AlwaysOff.
*/

/*!
    \fn void Q3ListBox::setScrollBar(bool enable)

    Use setVScrollBarMode() instead.

    If \a enable is true, pass \c AlwaysOn as the argument to
    setVScrollBarMode(); otherwise, pass \c AlwaysOff.
*/

/*!
    \fn bool Q3ListBox::autoBottomScrollBar() const

    Use hScrollBarMode() instead. Returns true if the horizontal
    scrollbar mode is set to \c Auto.
*/

/*!
    \fn void Q3ListBox::setAutoBottomScrollBar(bool enable)

    Use setHScrollBarMode() instead.

    If \a enable is true, pass \c Auto as the argument to
    setHScrollBarMode(); otherwise, pass \c AlwaysOff.
*/

/*!
    \fn bool Q3ListBox::bottomScrollBar() const

    Use hScrollBarMode() instead. Returns true if the horizontal
    scrollbar mode is not \c AlwaysOff.
*/

/*!
    \fn void Q3ListBox::setBottomScrollBar(bool enable)

    Use setHScrollBarMode() instead.

    If \a enable is true, pass \c AlwaysOn as the argument to
    setHScrollBarMode(); otherwise, pass \c AlwaysOff.
*/

/*!
    \fn bool Q3ListBox::smoothScrolling() const

    Returns false. Qt always scrolls smoothly.
*/

/*!
    \fn void Q3ListBox::setSmoothScrolling(bool b)

    Does nothing. Qt always scrolls smoothly. The \a b parameter is
    ignored.
*/

/*!
    \fn bool Q3ListBox::autoUpdate() const

    Returns true. Qt always updates automatically.
*/

/*!
    \fn void Q3ListBox::setAutoUpdate(bool b)

    Does nothing. Qt always updates automatically. The \a b parameter
    is ignored.
*/

/*!
    \fn void Q3ListBox::setFixedVisibleLines(int lines)

    Use setRowMode(\a lines) instead.
*/

/*!
    \fn int Q3ListBox::cellHeight(int i) const

    Use itemHeight(\a i) instead.
*/

/*!
    \fn int  Q3ListBox::cellHeight() const

    Use itemHeight() instead.
*/

/*!
    \fn int  Q3ListBox::cellWidth() const

    Use maxItemWidth() instead.
*/

/*!
    \fn int  Q3ListBox::cellWidth(int i) const

    Use maxItemWidth(\a i) instead.
*/

/*!
    \fn int  Q3ListBox::numCols() const

    Use numColumns() instead.
*/

/*!
    \fn void Q3ListBox::updateCellWidth()

    Does nothing. Qt automatically updates.
*/

/*!
    \fn int  Q3ListBox::totalWidth() const

    Use contentsWidth() instead.
*/

/*!
    \fn int  Q3ListBox::totalHeight() const

    Use contentsHeight() instead.
*/

/*!
    \fn int  Q3ListBox::findItem(int yPos) const

    Use index(itemAt(\a yPos)) instead.
*/

/*!
    \fn bool Q3ListBoxItem::selected() const

    Use isSelected() instead. Returns true if isSelected()
    returns true.
*/

/*!
    \fn bool Q3ListBoxItem::current() const

    Use isCurrent() instead. Returns true if isCurrent()
    returns true.
*/

/*!
    \enum Q3ListBox::StringComparisonMode

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
    \typedef Q3ListBox::ComparisonFlags

    This typedef is used in Q3IconView's API for values that are OR'd
    combinations of \l StringComparisonMode values.

    \sa StringComparisonMode
*/

QT_END_NAMESPACE

#endif // QT_NO_LISTBOX
