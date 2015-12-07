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

#include "q3table.h"


#include <qpainter.h>
#include <qlineedit.h>
#include <qcursor.h>
#include <qapplication.h>
#include <qtimer.h>
#include <qicon.h>
#include <q3combobox.h>
#include <qstyleoption.h>
#include <qcheckbox.h>
#include <q3dragobject.h>
#include <qevent.h>
#include <q3listbox.h>
#include <qstyle.h>
#include <q3datatable.h>
#include <qvalidator.h>
#include <q3button.h>

#include <stdlib.h>
#include <limits.h>

QT_BEGIN_NAMESPACE

using namespace Qt;

class Q3HeaderData;
extern bool qt_get_null_label_bit(Q3HeaderData *data, int section);
extern void qt_set_null_label_bit(Q3HeaderData *data, int section, bool b);

static bool qt_update_cell_widget = true;
static bool qt_table_clipper_enabled = true;
#ifndef QT_INTERNAL_TABLE
Q_COMPAT_EXPORT
#endif
void qt_set_table_clipper_enabled(bool enabled)
{
    qt_table_clipper_enabled = enabled;
}

class Q_COMPAT_EXPORT Q3TableHeader : public Q3Header
{
    friend class Q3Table;
    Q_OBJECT

public:
    enum SectionState {
        Normal,
        Bold,
        Selected
    };

    Q3TableHeader(int, Q3Table *t, QWidget* parent=0, const char* name=0);
    ~Q3TableHeader() {};
    void addLabel(const QString &s, int size);
    void setLabel(int section, const QString & s, int size = -1);
    void setLabel(int section, const QIconSet & iconset, const QString & s,
                   int size = -1);

    void setLabels(const QStringList & labels);

    void removeLabel(int section);

    void setSectionState(int s, SectionState state);
    void setSectionStateToAll(SectionState state);
    SectionState sectionState(int s) const;

    int sectionSize(int section) const;
    int sectionPos(int section) const;
    int sectionAt(int section) const;

    void setSectionStretchable(int s, bool b);
    bool isSectionStretchable(int s) const;

    void updateCache();

signals:
    void sectionSizeChanged(int s);

protected:
    void paintEvent(QPaintEvent *e);
    void paintSection(QPainter *p, int index, const QRect& fr);
    void mousePressEvent(QMouseEvent *e);
    void mouseMoveEvent(QMouseEvent *e);
    void mouseReleaseEvent(QMouseEvent *e);
    void mouseDoubleClickEvent(QMouseEvent *e);
    void resizeEvent(QResizeEvent *e);

private slots:
    void doAutoScroll();
    void sectionWidthChanged(int col, int os, int ns);
    void indexChanged(int sec, int oldIdx, int newIdx);
    void updateStretches();
    void updateWidgetStretches();

private:
    void updateSelections();
    void saveStates();
    void setCaching(bool b);
    void swapSections(int oldIdx, int newIdx, bool swapTable = true);
    bool doSelection(QMouseEvent *e);
    void sectionLabelChanged(int section);
    void resizeArrays(int n);

private:
    Q3MemArray<int> states, oldStates;
    Q3MemArray<bool> stretchable;
    Q3MemArray<int> sectionSizes, sectionPoses;
    bool mousePressed;
    int pressPos, startPos, endPos;
    Q3Table *table;
    QTimer *autoScrollTimer;
    QWidget *line1, *line2;
    bool caching;
    int resizedSection;
    bool isResizing;
    int numStretches;
    QTimer *stretchTimer, *widgetStretchTimer;
    Q3TableHeaderPrivate *d;

};

#ifdef _WS_QWS_
# define NO_LINE_WIDGET
#endif



struct Q3TablePrivate
{
    Q3TablePrivate() : hasRowSpan(false), hasColSpan(false),
                      inMenuMode(false), redirectMouseEvent(false)
    {
        hiddenRows.setAutoDelete(true);
        hiddenCols.setAutoDelete(true);
    }
    uint hasRowSpan : 1;
    uint hasColSpan : 1;
    uint inMenuMode : 1;
    uint redirectMouseEvent : 1;
    Q3IntDict<int> hiddenRows, hiddenCols;
    QTimer *geomTimer;
    int lastVisRow;
    int lastVisCol;
};

struct Q3TableHeaderPrivate
{
#ifdef NO_LINE_WIDGET
    int oldLinePos;
#endif
};

static bool isRowSelection(Q3Table::SelectionMode selMode)
{
    return selMode == Q3Table::SingleRow || selMode == Q3Table::MultiRow;
}

/*!
    \class Q3TableSelection
    \brief The Q3TableSelection class provides access to a selected area in a
    Q3Table.

    \compat

    The selection is a rectangular set of cells in a Q3Table. One of
    the rectangle's cells is called the anchor cell; this is the cell
    that was selected first. The init() function sets the anchor and
    the selection rectangle to exactly this cell; the expandTo()
    function expands the selection rectangle to include additional
    cells.

    There are various access functions to find out about the area:
    anchorRow() and anchorCol() return the anchor's position;
    leftCol(), rightCol(), topRow() and bottomRow() return the
    rectangle's four edges. All four are part of the selection.

    A newly created Q3TableSelection is inactive -- isActive() returns
    false. You must use init() and expandTo() to activate it.

    \sa Q3Table Q3Table::addSelection() Q3Table::selection()
    Q3Table::selectCells() Q3Table::selectRow() Q3Table::selectColumn()
*/

/*!
    Creates an inactive selection. Use init() and expandTo() to
    activate it.
*/

Q3TableSelection::Q3TableSelection()
    : active(false), inited(false), tRow(-1), lCol(-1),
      bRow(-1), rCol(-1), aRow(-1), aCol(-1)
{
}

/*!
    Creates an active selection, starting at \a start_row and \a
    start_col, ending at \a end_row and \a end_col.
*/

Q3TableSelection::Q3TableSelection(int start_row, int start_col, int end_row, int end_col)
    : active(false), inited(false), tRow(-1), lCol(-1),
      bRow(-1), rCol(-1), aRow(-1), aCol(-1)
{
    init(start_row, start_col);
    expandTo(end_row, end_col);
}

/*!
    Sets the selection anchor to cell \a row, \a col and the selection
    to only contain this cell. The selection is not active until
    expandTo() is called.

    To extend the selection to include additional cells, call
    expandTo().

    \sa isActive()
*/

void Q3TableSelection::init(int row, int col)
{
    aCol = lCol = rCol = col;
    aRow = tRow = bRow = row;
    active = false;
    inited = true;
}

/*!
    Expands the selection to include cell \a row, \a col. The new
    selection rectangle is the bounding rectangle of \a row, \a col
    and the previous selection rectangle. After calling this function
    the selection is active.

    If you haven't called init(), this function does nothing.

    \sa init() isActive()
*/

void Q3TableSelection::expandTo(int row, int col)
{
    if (!inited)
        return;
    active = true;

    if (row < aRow) {
        tRow = row;
        bRow = aRow;
    } else {
        tRow = aRow;
        bRow = row;
    }

    if (col < aCol) {
        lCol = col;
        rCol = aCol;
    } else {
        lCol = aCol;
        rCol = col;
    }
}

/*!
    Returns true if \a s includes the same cells as the selection;
    otherwise returns false.
*/

bool Q3TableSelection::operator==(const Q3TableSelection &s) const
{
    return (s.active == active &&
             s.tRow == tRow && s.bRow == bRow &&
             s.lCol == lCol && s.rCol == rCol);
}

/*!
    \fn bool Q3TableSelection::operator!=(const Q3TableSelection &s) const

    Returns true if \a s does not include the same cells as the
    selection; otherwise returns false.
*/


/*!
    \fn int Q3TableSelection::topRow() const

    Returns the top row of the selection.

    \sa bottomRow() leftCol() rightCol()
*/

/*!
    \fn int Q3TableSelection::bottomRow() const

    Returns the bottom row of the selection.

    \sa topRow() leftCol() rightCol()
*/

/*!
    \fn int Q3TableSelection::leftCol() const

    Returns the left column of the selection.

    \sa topRow() bottomRow() rightCol()
*/

/*!
    \fn int Q3TableSelection::rightCol() const

    Returns the right column of the selection.

    \sa topRow() bottomRow() leftCol()
*/

/*!
    \fn int Q3TableSelection::anchorRow() const

    Returns the anchor row of the selection.

    \sa anchorCol() expandTo()
*/

/*!
    \fn int Q3TableSelection::anchorCol() const

    Returns the anchor column of the selection.

    \sa anchorRow() expandTo()
*/

/*!
    \fn int Q3TableSelection::numRows() const

    Returns the number of rows in the selection.

    \sa numCols()
*/
int Q3TableSelection::numRows() const
{
    return (tRow < 0) ? 0 : bRow - tRow + 1;
}

/*!
    Returns the number of columns in the selection.

    \sa numRows()
*/
int Q3TableSelection::numCols() const
{
    return (lCol < 0) ? 0 : rCol - lCol + 1;
}

/*!
    \fn bool Q3TableSelection::isActive() const

    Returns whether the selection is active or not. A selection is
    active after init() \e and expandTo() have been called.
*/

/*!
    \fn bool Q3TableSelection::isEmpty() const

    Returns whether the selection is empty or not.

    \sa numRows(), numCols()
*/

/*!
    \class Q3TableItem
    \brief The Q3TableItem class provides the cell content for Q3Table cells.

    \compat

    For many applications Q3TableItems are ideal for presenting and
    editing the contents of Q3Table cells. In situations where you need
    to create very large tables you may prefer an alternative approach
    to using Q3TableItems: see the notes on large tables.

    A Q3TableItem contains a cell's data, by default, a string and a
    pixmap. The table item also holds the cell's display size and how
    the data should be aligned. The table item specifies the cell's
    \l EditType and the editor used for in-place editing (by default a
    QLineEdit). If you want checkboxes use \l{Q3CheckTableItem}, and if
    you want comboboxes use \l{Q3ComboTableItem}. The \l EditType (set
    in the constructor) determines whether the cell's contents may be
    edited.

    If a pixmap is specified it is displayed to the left of any text.
    You can change the text or pixmap with setText() and setPixmap()
    respectively. For text you can use setWordWrap().

    When sorting table items the key() function is used; by default
    this returns the table item's text(). Reimplement key() to
    customize how your table items will sort.

    Table items are inserted into a table using Q3Table::setItem(). If
    you insert an item into a cell that already contains a table item
    the original item will be deleted.

    Example:
    \snippet doc/src/snippets/code/src_qt3support_itemviews_q3table.cpp 0

    You can move a table item from one cell to another, in the same or
    a different table, using Q3Table::takeItem() and Q3Table::setItem()
    but see also Q3Table::swapCells().

    Table items can be deleted with delete in the standard way; the
    table and cell will be updated accordingly.

    Note, that if you have a table item that is not currently in a table
    then anything you do to that item other than insert it into a table
    will result in undefined behaviour.

    Reimplement createEditor() and setContentFromEditor() if you want
    to use your own widget instead of a QLineEdit for editing cell
    contents. Reimplement paint() if you want to display custom
    content.

    It is important to ensure that your custom widget can accept the
    keyboard focus, so that the user can use the tab key to navigate the
    table as normal. Therefore, if the widget returned by createEditor()
    does not itself accept the keyboard focus, it is necessary to
    nominate a child widget to do so on its behalf. For example, a
    QHBox with two child QLineEdit widgets may use one of them to
    accept the keyboard focus:

    \snippet doc/src/snippets/code/src_qt3support_itemviews_q3table.cpp 1

    By default, table items may be replaced by new Q3TableItems
    during the lifetime of a Q3Table. Therefore, if you create your
    own subclass of Q3TableItem, and you want to ensure that
    this does not happen, you must call setReplaceable(false)
    in the constructor of your subclass.

    \img qtableitems.png Table Items

    \sa Q3CheckTableItem Q3ComboTableItem

*/

/*!
    \fn Q3Table *Q3TableItem::table() const

    Returns the Q3Table the table item belongs to.

    \sa Q3Table::setItem() Q3TableItem()
*/

/*!
    \enum Q3TableItem::EditType

    \target wheneditable
    This enum is used to define whether a cell is editable or
    read-only (in conjunction with other settings), and how the cell
    should be displayed.

    \value Always
    The cell always \e looks editable.

    Using this EditType ensures that the editor created with
    createEditor() (by default a QLineEdit) is always visible. This
    has implications for the alignment of the content: the default
    editor aligns everything (even numbers) to the left whilst
    numerical values in the cell are by default aligned to the right.

    If a cell with the edit type \c Always looks misaligned you could
    reimplement createEditor() for these items.

    \value WhenCurrent
    The cell \e looks editable only when it has keyboard focus (see
    Q3Table::setCurrentCell()).

    \value OnTyping
    The cell \e looks editable only when the user types in it or
    double-clicks it. It resembles the \c WhenCurrent functionality
    but is, perhaps, nicer.

    The \c OnTyping edit type is the default when Q3TableItem objects
    are created by the convenience functions Q3Table::setText() and
    Q3Table::setPixmap().

    \value Never  The cell is not editable.

    The cell is actually editable only if Q3Table::isRowReadOnly() is
    false for its row, Q3Table::isColumnReadOnly() is false for its
    column, and Q3Table::isReadOnly() is false.

    Q3ComboTableItems have an isEditable() property. This property is
    used to indicate whether the user may enter their own text or are
    restricted to choosing one of the choices in the list.
    Q3ComboTableItems may be interacted with only if they are editable
    in accordance with their EditType as described above.

*/

/*!
    Creates a table item that is a child of table \a table with no
    text. The item has the \l EditType \a et.

    The table item will use a QLineEdit for its editor, will not
    word-wrap and will occupy a single cell. Insert the table item
    into a table with Q3Table::setItem().

    The table takes ownership of the table item, so a table item
    should not be inserted into more than one table at a time.
*/

Q3TableItem::Q3TableItem(Q3Table *table, EditType et)
    : txt(), pix(), t(table), edType(et), wordwrap(false),
      tcha(true), rw(-1), cl(-1), rowspan(1), colspan(1)
{
    enabled = true;
}

/*!
    Creates a table item that is a child of table \a table with text
    \a text. The item has the \l EditType \a et.

    The table item will use a QLineEdit for its editor, will not
    word-wrap and will occupy a single cell. Insert the table item
    into a table with Q3Table::setItem().

    The table takes ownership of the table item, so a table item
    should not be inserted into more than one table at a time.
*/

Q3TableItem::Q3TableItem(Q3Table *table, EditType et, const QString &text)
    : txt(text), pix(), t(table), edType(et), wordwrap(false),
      tcha(true), rw(-1), cl(-1), rowspan(1), colspan(1)
{
    enabled = true;
}

/*!
    Creates a table item that is a child of table \a table with text
    \a text and pixmap \a p. The item has the \l EditType \a et.

    The table item will display the pixmap to the left of the text. It
    will use a QLineEdit for editing the text, will not word-wrap and
    will occupy a single cell. Insert the table item into a table with
    Q3Table::setItem().

    The table takes ownership of the table item, so a table item
    should not be inserted in more than one table at a time.
*/

Q3TableItem::Q3TableItem(Q3Table *table, EditType et,
                        const QString &text, const QPixmap &p)
    : txt(text), pix(p), t(table), edType(et), wordwrap(false),
      tcha(true), rw(-1), cl(-1), rowspan(1), colspan(1)
{
    enabled = true;
}

/*!
    The destructor deletes this item and frees all allocated
    resources.

    If the table item is in a table (i.e. was inserted with
    setItem()), it will be removed from the table and the cell it
    occupied.
*/

Q3TableItem::~Q3TableItem()
{
    if (table())
        table()->takeItem(this);
}

int Q3TableItem::RTTI = 0;

/*!
    Returns the Run Time Type Identification value for this table item
    which for Q3TableItems is 0.

    When you create subclasses based on Q3TableItem make sure that each
    subclass returns a unique rtti() value. It is advisable to use
    values greater than 1000, preferably large random numbers, to
    allow for extensions to this class.

    \sa Q3CheckTableItem::rtti() Q3ComboTableItem::rtti()
*/

int Q3TableItem::rtti() const
{
    return RTTI;
}

/*!
    Returns the table item's pixmap or a null pixmap if no pixmap has
    been set.

    \sa setPixmap() text()
*/

QPixmap Q3TableItem::pixmap() const
{
    return pix;
}


/*!
    Returns the text of the table item or an empty string if there is
    no text.

    To ensure that the current value of the editor is returned,
    setContentFromEditor() is called:
    \list 1
    \i if the editMode() is \c Always, or
    \i if editMode() is \e not \c Always but the editor of the cell is
    active and the editor is not a QLineEdit.
    \endlist

    This means that text() returns the original text value of the item
    if the editor is a line edit, until the user commits an edit (e.g.
    by pressing Enter or Tab) in which case the new text is returned.
    For other editors (e.g. a combobox) setContentFromEditor() is
    always called so the currently display value is the one returned.

    \sa setText() pixmap()
*/

QString Q3TableItem::text() const
{
    QWidget *w = table()->cellWidget(rw, cl);
    if (w && (edType == Always ||
                rtti() == Q3ComboTableItem::RTTI ||
                rtti() == Q3CheckTableItem::RTTI))
        ((Q3TableItem*)this)->setContentFromEditor(w);
    return txt;
}

/*!
    Sets pixmap \a p to be this item's pixmap.

    Note that setPixmap() does not update the cell the table item
    belongs to. Use Q3Table::updateCell() to repaint the cell's
    contents.

    For \l{Q3ComboTableItem}s and \l{Q3CheckTableItem}s this function
    has no visible effect.

    \sa Q3Table::setPixmap() pixmap() setText()
*/

void Q3TableItem::setPixmap(const QPixmap &p)
{
    pix = p;
}

/*!
    Changes the table item's text to \a str.

    Note that setText() does not update the cell the table item
    belongs to. Use Q3Table::updateCell() to repaint the cell's
    contents.

    \sa Q3Table::setText() text() setPixmap() Q3Table::updateCell()
*/

void Q3TableItem::setText(const QString &str)
{
    txt = str;
}

/*!
    This virtual function is used to paint the contents of an item
    using the painter \a p in the rectangular area \a cr using the
    color group \a cg.

    If \a selected is true the cell is displayed in a way that
    indicates that it is highlighted.

    You don't usually need to use this function but if you want to
    draw custom content in a cell you will need to reimplement it.

    The painter passed to this function is translated so that 0, 0
    is the top-left corner of the item that is being painted.

    Note that the painter is not clipped by default in order to get
    maximum efficiency. If you want clipping, use

    \snippet doc/src/snippets/code/src_qt3support_itemviews_q3table.cpp 2

*/

void Q3TableItem::paint(QPainter *p, const QColorGroup &cg,
                        const QRect &cr, bool selected)
{
    p->fillRect(0, 0, cr.width(), cr.height(),
                 selected ? cg.brush(QColorGroup::Highlight)
                          : cg.brush(QColorGroup::Base));

    int w = cr.width();
    int h = cr.height();

    int x = 0;
    if (!pix.isNull()) {
        p->drawPixmap(0, (cr.height() - pix.height()) / 2, pix);
        x = pix.width() + 2;
    }

    if (selected)
        p->setPen(cg.highlightedText());
    else
        p->setPen(cg.text());
    p->drawText(x + 2, 0, w - x - 4, h,
                 wordwrap ? (alignment() | WordBreak) : alignment(), text());
}

/*!
This virtual function creates an editor which the user can
interact with to edit the cell's contents. The default
implementation creates a QLineEdit.

If the function returns 0, the cell is read-only.

The returned widget should preferably be invisible, ideally with
Q3Table::viewport() as parent.

If you reimplement this function you'll almost certainly need to
reimplement setContentFromEditor(), and may need to reimplement
sizeHint().

\sa Q3Table::createEditor() setContentFromEditor() Q3Table::viewport() setReplaceable()
*/

QWidget *Q3TableItem::createEditor() const
{
    QLineEdit *e = new QLineEdit(table()->viewport(), "qt_tableeditor");
    e->setFrame(false);
    e->setText(text());
    return e;
}

/*!
Whenever the content of a cell has been edited by the editor \a w,
Q3Table calls this virtual function to copy the new values into the
Q3TableItem.

If you reimplement createEditor() and return something that is not
a QLineEdit you will need to reimplement this function.

\sa Q3Table::setCellContentFromEditor()
*/

void Q3TableItem::setContentFromEditor(QWidget *w)
{
    QLineEdit *le = qobject_cast<QLineEdit*>(w);
    if (le) {
        QString input = le->text();
        if (le->validator())
            le->validator()->fixup(input);
        setText(input);
    }
}

/*!
    The alignment function returns how the text contents of the cell
    are aligned when drawn. The default implementation aligns numbers
    to the right and any other text to the left.

    \sa Qt::Alignment
*/

// ed: For consistency reasons a setAlignment() should be provided
// as well.

int Q3TableItem::alignment() const
{
    bool num;
    bool ok1 = false, ok2 = false;
    (void)text().toInt(&ok1);
    if (!ok1)
        (void)text().toDouble(&ok2); // ### should be .-aligned
    num = ok1 || ok2;

    return (num ? AlignRight : AlignLeft) | AlignVCenter;
}

/*!
    If \a b is true, the cell's text will be wrapped over multiple
    lines, when necessary, to fit the width of the cell; otherwise the
    text will be written as a single line.

    \sa wordWrap() Q3Table::adjustColumn() Q3Table::setColumnStretchable()
*/

void Q3TableItem::setWordWrap(bool b)
{
    wordwrap = b;
}

/*!
    Returns true if word wrap is enabled for the cell; otherwise
    returns false.

    \sa setWordWrap()
*/

bool Q3TableItem::wordWrap() const
{
    return wordwrap;
}

/*! \internal */

void Q3TableItem::updateEditor(int oldRow, int oldCol)
{
    if (edType != Always)
        return;
    if (oldRow != -1 && oldCol != -1)
        table()->clearCellWidget(oldRow, oldCol);
    if (rw != -1 && cl != -1)
        table()->setCellWidget(rw, cl, createEditor());
}

/*!
    Returns the table item's edit type.

    This is set when the table item is constructed.

    \sa EditType Q3TableItem()
*/

Q3TableItem::EditType Q3TableItem::editType() const
{
    return edType;
}

/*!
    If \a b is true it is acceptable to replace the contents of the
    cell with the contents of another Q3TableItem. If \a b is false the
    contents of the cell may not be replaced by the contents of
    another table item. Table items that span more than one cell may
    not have their contents replaced by another table item.

    (This differs from \l EditType because EditType is concerned with
    whether the \e user is able to change the contents of a cell.)

    \sa isReplaceable()
*/

void Q3TableItem::setReplaceable(bool b)
{
    tcha = b;
}

/*!
    This function returns whether the contents of the cell may be
    replaced with the contents of another table item. Regardless of
    this setting, table items that span more than one cell may not
    have their contents replaced by another table item.

    (This differs from \l EditType because EditType is concerned with
    whether the \e user is able to change the contents of a cell.)

    \sa setReplaceable() EditType
*/

bool Q3TableItem::isReplaceable() const
{
    if (rowspan > 1 || colspan > 1)
        return false;
    return tcha;
}

/*!
    This virtual function returns the key that should be used for
    sorting. The default implementation returns the text() of the
    relevant item.

    \sa Q3Table::setSorting()
*/

QString Q3TableItem::key() const
{
    return text();
}

/*!
    This virtual function returns the size a cell needs to show its
    entire content.

    If you subclass Q3TableItem you will often need to reimplement this
    function.
*/

QSize Q3TableItem::sizeHint() const
{
    QSize strutSize = QApplication::globalStrut();
    if (edType == Always && table()->cellWidget(rw, cl))
        return table()->cellWidget(rw, cl)->sizeHint().expandedTo(strutSize);

    QSize s;
    int x = 0;
    if (!pix.isNull()) {
        s = pix.size();
        s.setWidth(s.width() + 2);
        x = pix.width() + 2;
    }

    QString t = text();
    if (!wordwrap && t.find(QLatin1Char('\n')) == -1)
        return QSize(s.width() + table()->fontMetrics().width(text()) + 10,
                      QMAX(s.height(), table()->fontMetrics().height())).expandedTo(strutSize);

    QRect r = table()->fontMetrics().boundingRect(x + 2, 0, table()->columnWidth(col()) - x - 4, 0,
                                                   wordwrap ? (alignment() | WordBreak) : alignment(),
                                                   text());
    r.setWidth(QMAX(r.width() + 10, table()->columnWidth(col())));
    return QSize(r.width(), QMAX(s.height(), r.height())).expandedTo(strutSize);
}

/*!
    Changes the extent of the Q3TableItem so that it spans multiple
    cells covering \a rs rows and \a cs columns. The top left cell is
    the original cell.

    \warning This function only works if the item has already been
    inserted into the table using e.g. Q3Table::setItem(). This
    function also checks to make sure if \a rs and \a cs are within
    the bounds of the table and returns without changing the span if
    they are not. In addition swapping, inserting or removing rows and
    columns that cross Q3TableItems spanning more than one cell is not
    supported.

    \sa rowSpan() colSpan()
*/

void Q3TableItem::setSpan(int rs, int cs)
{
    if (rs == rowspan && cs == colspan)
        return;

    if (!table()->d->hasRowSpan)
        table()->d->hasRowSpan = rs > 1;
    if (!table()->d->hasColSpan)
        table()->d->hasColSpan = cs > 1;
    // return if we are thinking too big...
    if (rw + rs > table()->numRows())
        return;

    if (cl + cs > table()->numCols())
        return;

    if (rw == -1 || cl == -1)
        return;

    int rrow = rw;
    int rcol = cl;
    if (rowspan > 1 || colspan > 1) {
        Q3Table* t = table();
        t->takeItem(this);
        t->setItem(rrow, rcol, this);
    }

    rowspan = rs;
    colspan = cs;

    for (int r = 0; r < rowspan; ++r) {
        for (int c = 0; c < colspan; ++c) {
            if (r == 0 && c == 0)
                continue;
            qt_update_cell_widget = false;
            table()->setItem(r + rw, c + cl, this);
            qt_update_cell_widget = true;
            rw = rrow;
            cl = rcol;
        }
    }

    table()->updateCell(rw, cl);
    QWidget *w = table()->cellWidget(rw, cl);
    if (w)
        w->resize(table()->cellGeometry(rw, cl).size());
}

/*!
    Returns the row span of the table item, usually 1.

    \sa setSpan() colSpan()
*/

int Q3TableItem::rowSpan() const
{
    return rowspan;
}

/*!
    Returns the column span of the table item, usually 1.

    \sa setSpan() rowSpan()
*/

int Q3TableItem::colSpan() const
{
    return colspan;
}

/*!
    Sets row \a r as the table item's row. Usually you do not need to
    call this function.

    If the cell spans multiple rows, this function sets the top row
    and retains the height of the multi-cell table item.

    \sa row() setCol() rowSpan()
*/

void Q3TableItem::setRow(int r)
{
    rw = r;
}

/*!
    Sets column \a c as the table item's column. Usually you will not
    need to call this function.

    If the cell spans multiple columns, this function sets the
    left-most column and retains the width of the multi-cell table
    item.

    \sa col() setRow() colSpan()
*/

void Q3TableItem::setCol(int c)
{
    cl = c;
}

/*!
    Returns the row where the table item is located. If the cell spans
    multiple rows, this function returns the top-most row.

    \sa col() setRow()
*/

int Q3TableItem::row() const
{
    return rw;
}

/*!
    Returns the column where the table item is located. If the cell
    spans multiple columns, this function returns the left-most
    column.

    \sa row() setCol()
*/

int Q3TableItem::col() const
{
    return cl;
}

/*!
    If \a b is true, the table item is enabled; if \a b is false the
    table item is disabled.

    A disabled item doesn't respond to user interaction.

    \sa isEnabled()
*/

void Q3TableItem::setEnabled(bool b)
{
    if (b == (bool)enabled)
        return;
    enabled = b;
    table()->updateCell(row(), col());
}

/*!
    Returns true if the table item is enabled; otherwise returns false.

    \sa setEnabled()
*/

bool Q3TableItem::isEnabled() const
{
    return (bool)enabled;
}

/*!
    \class Q3ComboTableItem
    \brief The Q3ComboTableItem class provides a means of using
    comboboxes in Q3Tables.

    \compat

    A Q3ComboTableItem is a table item which looks and behaves like a
    combobox. The advantage of using Q3ComboTableItems rather than real
    comboboxes is that a Q3ComboTableItem uses far less resources than
    real comboboxes in \l{Q3Table}s. When the cell has the focus it
    displays a real combobox which the user can interact with. When
    the cell does not have the focus the cell \e looks like a
    combobox. Only text items (i.e. no pixmaps) may be used in
    Q3ComboTableItems.

    Q3ComboTableItem items have the edit type \c WhenCurrent (see
    \l{EditType}). The Q3ComboTableItem's list of items is provided by
    a QStringList passed to the constructor.

    The list of items may be changed using setStringList(). The
    current item can be set with setCurrentItem() and retrieved with
    currentItem(). The text of the current item can be obtained with
    currentText(), and the text of a particular item can be retrieved
    with text().

    If isEditable() is true the Q3ComboTableItem will permit the user
    to either choose an existing list item, or create a new list item
    by entering their own text; otherwise the user may only choose one
    of the existing list items.

    To populate a table cell with a Q3ComboTableItem use
    Q3Table::setItem().

    Q3ComboTableItems may be deleted with Q3Table::clearCell().

    Q3ComboTableItems can be distinguished from \l{Q3TableItem}s and
    \l{Q3CheckTableItem}s using their Run Time Type Identification
    number (see rtti()).

    \img qtableitems.png Table Items

    \sa Q3CheckTableItem Q3TableItem Q3ComboBox
*/

Q3ComboBox *Q3ComboTableItem::fakeCombo = 0;
QWidget *Q3ComboTableItem::fakeComboWidget = 0;
int Q3ComboTableItem::fakeRef = 0;

/*!
    Creates a combo table item for the table \a table. The combobox's
    list of items is passed in the \a list argument. If \a editable is
    true the user may type in new list items; if \a editable is false
    the user may only select from the list of items provided.

    By default Q3ComboTableItems cannot be replaced by other table
    items since isReplaceable() returns false by default.

    \sa Q3Table::clearCell() EditType
*/

Q3ComboTableItem::Q3ComboTableItem(Q3Table *table, const QStringList &list, bool editable)
    : Q3TableItem(table, WhenCurrent, QLatin1String("")), entries(list), current(0), edit(editable)
{
    setReplaceable(false);
    if (!Q3ComboTableItem::fakeCombo) {
        Q3ComboTableItem::fakeComboWidget = new QWidget(0, 0);
        Q3ComboTableItem::fakeCombo = new Q3ComboBox(false, Q3ComboTableItem::fakeComboWidget, 0);
        Q3ComboTableItem::fakeCombo->hide();
    }
    ++Q3ComboTableItem::fakeRef;
    if (entries.count())
        setText(entries.at(current));
}

/*!
    Q3ComboTableItem destructor.
*/
Q3ComboTableItem::~Q3ComboTableItem()
{
    if (--Q3ComboTableItem::fakeRef <= 0) {
        delete Q3ComboTableItem::fakeComboWidget;
        Q3ComboTableItem::fakeComboWidget = 0;
        Q3ComboTableItem::fakeCombo = 0;
    }
}

/*!
    Sets the list items of this Q3ComboTableItem to the strings in the
    string list \a l.
*/

void Q3ComboTableItem::setStringList(const QStringList &l)
{
    entries = l;
    current = 0;
    if (entries.count())
        setText(entries.at(current));
    if (table()->cellWidget(row(), col())) {
        cb->clear();
        cb->insertStringList(entries);
    }
    table()->updateCell(row(), col());
}

/*! \reimp */

QWidget *Q3ComboTableItem::createEditor() const
{
    // create an editor - a combobox in our case
    ((Q3ComboTableItem*)this)->cb = new Q3ComboBox(edit, table()->viewport(), "qt_editor_cb");
    cb->insertStringList(entries);
    cb->setCurrentItem(current);
    QObject::connect(cb, SIGNAL(activated(int)), table(), SLOT(doValueChanged()));
    return cb;
}

/*! \reimp */

void Q3ComboTableItem::setContentFromEditor(QWidget *w)
{
    Q3ComboBox *cb = qobject_cast<Q3ComboBox*>(w);
    if (cb) {
        entries.clear();
        for (int i = 0; i < cb->count(); ++i)
            entries << cb->text(i);
        current = cb->currentItem();
        setText(cb->currentText());
    }
}

/*! \reimp */

void Q3ComboTableItem::paint(QPainter *p, const QColorGroup &cg,
                           const QRect &cr, bool selected)
{
    fakeCombo->resize(cr.width(), cr.height());

    QPalette pal2(cg);
    if (selected) {
        pal2.setBrush(QPalette::Base, cg.QPalette::brush(QPalette::Highlight));
        pal2.setColor(QPalette::Text, cg.highlightedText());
    }

    QStyle::State flags = QStyle::State_None;
    if(isEnabled() && table()->isEnabled())
        flags |= QStyle::State_Enabled;
    // Since we still have the "fakeCombo" may as well use it in this case.
    QStyleOptionComboBox opt;
    opt.initFrom(table());
    opt.rect = fakeCombo->rect();
    opt.palette = pal2;
    opt.state &= ~QStyle::State_HasFocus;
    opt.state &= ~QStyle::State_MouseOver;
    opt.state |= flags;
    opt.subControls = QStyle::SC_All;
    opt.activeSubControls = QStyle::SC_None;
    opt.editable = fakeCombo->editable();
    table()->style()->drawComplexControl(QStyle::CC_ComboBox, &opt, p, fakeCombo);

    p->save();
    QRect textR = table()->style()->subControlRect(QStyle::CC_ComboBox, &opt,
                                                   QStyle::SC_ComboBoxEditField, fakeCombo);
    int align = alignment(); // alignment() changes entries
    p->drawText(textR, wordWrap() ? (align | Qt::WordBreak) : align, entries.value(current));
    p->restore();
}

/*!
    Sets the list item \a i to be the combo table item's current list
    item.

    \sa currentItem()
*/

void Q3ComboTableItem::setCurrentItem(int i)
{
    QWidget *w = table()->cellWidget(row(), col());
    Q3ComboBox *cb = qobject_cast<Q3ComboBox*>(w);
    if (cb) {
        cb->setCurrentItem(i);
        current = cb->currentItem();
        setText(cb->currentText());
    } else {
        if (i < 0 || i >= entries.count())
            return;
        current = i;
        setText(entries.at(i));
        table()->updateCell(row(), col());
    }
}

/*!
    \overload

    Sets the list item whose text is \a s to be the combo table item's
    current list item. Does nothing if no list item has the text \a s.

    \sa currentItem()
*/

void Q3ComboTableItem::setCurrentItem(const QString &s)
{
    int i = entries.findIndex(s);
    if (i != -1)
        setCurrentItem(i);
}

/*!
    Returns the index of the combo table item's current list item.

    \sa setCurrentItem()
*/

int Q3ComboTableItem::currentItem() const
{
    QWidget *w = table()->cellWidget(row(), col());
    Q3ComboBox *cb = qobject_cast<Q3ComboBox*>(w);
    if (cb)
        return cb->currentItem();
    return current;
}

/*!
    Returns the text of the combo table item's current list item.

    \sa currentItem() text()
*/

QString Q3ComboTableItem::currentText() const
{
    QWidget *w = table()->cellWidget(row(), col());
    Q3ComboBox *cb = qobject_cast<Q3ComboBox*>(w);
    if (cb)
        return cb->currentText();
    return entries.value(current);
}

/*!
    Returns the total number of list items in the combo table item.
*/

int Q3ComboTableItem::count() const
{
    QWidget *w = table()->cellWidget(row(), col());
    Q3ComboBox *cb = qobject_cast<Q3ComboBox*>(w);
    if (cb)
        return cb->count();
    return (int)entries.count();
}

/*!
    Returns the text of the combo's list item at index \a i.

    \sa currentText()
*/

QString Q3ComboTableItem::text(int i) const
{
    QWidget *w = table()->cellWidget(row(), col());
    Q3ComboBox *cb = qobject_cast<Q3ComboBox*>(w);
    if (cb)
        return cb->text(i);
    return entries.value(i);
}

/*!
    If \a b is true the combo table item can be edited, i.e. the user
    may enter a new text item themselves. If \a b is false the user may
    may only choose one of the existing items.

    \sa isEditable()
*/

void Q3ComboTableItem::setEditable(bool b)
{
    edit = b;
}

/*!
    Returns true if the user can add their own list items to the
    combobox's list of items; otherwise returns false.

    \sa setEditable()
*/

bool Q3ComboTableItem::isEditable() const
{
    return edit;
}

int Q3ComboTableItem::RTTI = 1;

/*!
    \fn int Q3ComboTableItem::rtti() const

    Returns 1.

    Make your derived classes return their own values for rtti()to
    distinguish between different table item subclasses. You should
    use values greater than 1000, preferably a large random number, to
    allow for extensions to this class.


    \sa Q3TableItem::rtti()
*/

int Q3ComboTableItem::rtti() const
{
    return RTTI;
}

/*! \reimp */

QSize Q3ComboTableItem::sizeHint() const
{
    fakeCombo->insertItem(currentText());
    fakeCombo->setCurrentItem(fakeCombo->count() - 1);
    QSize sh = fakeCombo->sizeHint();
    fakeCombo->removeItem(fakeCombo->count() - 1);
    return sh.expandedTo(QApplication::globalStrut());
}

/*!
    \fn QString Q3ComboTableItem::text() const

    Returns the text of the table item or an empty string if there is
    no text.

    \sa Q3TableItem::text()
*/

/*!
    \class Q3CheckTableItem
    \brief The Q3CheckTableItem class provides checkboxes in Q3Tables.

    \compat

    A Q3CheckTableItem is a table item which looks and behaves like a
    checkbox. The advantage of using Q3CheckTableItems rather than real
    checkboxes is that a Q3CheckTableItem uses far less resources than
    a real checkbox would in a \l{Q3Table}. When the cell has the focus
    it displays a real checkbox which the user can interact with. When
    the cell does not have the focus the cell \e looks like a
    checkbox. Pixmaps may not be used in Q3CheckTableItems.

    Q3CheckTableItem items have the edit type \c WhenCurrent (see
    \l{EditType}).

    To change the checkbox's label use setText(). The checkbox can be
    checked and unchecked with setChecked() and its state retrieved
    using isChecked().

    To populate a table cell with a Q3CheckTableItem use
    Q3Table::setItem().

    Q3CheckTableItems can be distinguished from \l{Q3TableItem}s and
    \l{Q3ComboTableItem}s using their Run Time Type Identification
    (rtti) value.

    \img qtableitems.png Table Items

    \sa rtti() EditType Q3ComboTableItem Q3TableItem QCheckBox
*/

/*!
    Creates a Q3CheckTableItem with an \l{EditType} of \c WhenCurrent
    as a child of \a table. The checkbox is initially unchecked and
    its label is set to the string \a txt.
*/

Q3CheckTableItem::Q3CheckTableItem(Q3Table *table, const QString &txt)
    : Q3TableItem(table, WhenCurrent, txt), checked(false)
{
}

/*! \reimp */

void Q3CheckTableItem::setText(const QString &t)
{
    Q3TableItem::setText(t);
    QWidget *w = table()->cellWidget(row(), col());
    QCheckBox *cb = qobject_cast<QCheckBox*>(w);
    if (cb)
        cb->setText(t);
}


/*! \reimp */

QWidget *Q3CheckTableItem::createEditor() const
{
    // create an editor - a combobox in our case
    ((Q3CheckTableItem*)this)->cb = new QCheckBox(table()->viewport(), "qt_editor_checkbox");
    cb->setChecked(checked);
    cb->setText(text());
    cb->setBackgroundColor(table()->viewport()->backgroundColor());
    cb->setAutoFillBackground(true);
    QObject::connect(cb, SIGNAL(toggled(bool)), table(), SLOT(doValueChanged()));
    return cb;
}

/*! \reimp */

void Q3CheckTableItem::setContentFromEditor(QWidget *w)
{
    QCheckBox *cb = qobject_cast<QCheckBox*>(w);
    if (cb)
        checked = cb->isChecked();
}

/*! \reimp */

void Q3CheckTableItem::paint(QPainter *p, const QColorGroup &cg,
                                const QRect &cr, bool selected)
{
    QPalette pal = cg;

    p->fillRect(0, 0, cr.width(), cr.height(),
                 selected ? pal.brush(QPalette::Highlight)
                          : pal.brush(QPalette::Base));

    QSize sz = QSize(table()->style()->pixelMetric(QStyle::PM_IndicatorWidth),
                      table()->style()->pixelMetric(QStyle::PM_IndicatorHeight));
    QPalette pal2(pal);
    pal2.setBrush(QPalette::Window, pal.brush(QPalette::Base));
    QStyleOptionButton opt;
    opt.initFrom(table());
    opt.rect.setRect(0, (cr.height() - sz.height()) / 2, sz.width(), sz.height());
    opt.palette = pal2;
    opt.state &= ~QStyle::State_HasFocus;
    opt.state &= ~QStyle::State_MouseOver;
    if(isEnabled())
        opt.state |= QStyle::State_Enabled;
    if (checked)
        opt.state |= QStyle::State_On;
    else
        opt.state |= QStyle::State_Off;
    if (isEnabled() && table()->isEnabled())
        opt.state |= QStyle::State_Enabled;
    table()->style()->drawPrimitive(QStyle::PE_IndicatorCheckBox, &opt, p, table());
    if (selected)
        p->setPen(pal.highlightedText().color());
    else
        p->setPen(pal.text().color());
    opt.rect.setRect(0, 0, cr.width(), cr.height());
    QRect textRect = table()->style()->subElementRect(QStyle::SE_CheckBoxContents, &opt, table());
    p->drawText(textRect, wordWrap() ? (alignment() | Qt::WordBreak) : alignment(), text());
}

/*!
    If \a b is true the checkbox is checked; if \a b is false the
    checkbox is unchecked.

    \sa isChecked()
*/

void Q3CheckTableItem::setChecked(bool b)
{
    checked = b;
    table()->updateCell(row(), col());
    QWidget *w = table()->cellWidget(row(), col());
    QCheckBox *cb = qobject_cast<QCheckBox*>(w);
    if (cb)
        cb->setChecked(b);
}

/*!
    Returns true if the checkbox table item is checked; otherwise
    returns false.

    \sa setChecked()
*/

bool Q3CheckTableItem::isChecked() const
{
    // #### why was this next line here. It must not be here, as
    // #### people want to call isChecked() from within paintCell()
    // #### and end up in an infinite loop that way
    // table()->updateCell(row(), col());
    QWidget *w = table()->cellWidget(row(), col());
    QCheckBox *cb = qobject_cast<QCheckBox*>(w);
    if (cb)
        return cb->isChecked();
    return checked;
}

int Q3CheckTableItem::RTTI = 2;

/*!
    \fn int Q3CheckTableItem::rtti() const

    Returns 2.

    Make your derived classes return their own values for rtti()to
    distinguish between different table item subclasses. You should
    use values greater than 1000, preferably a large random number, to
    allow for extensions to this class.

    \sa Q3TableItem::rtti()
*/

int Q3CheckTableItem::rtti() const
{
    return RTTI;
}

/*! \reimp */

QSize Q3CheckTableItem::sizeHint() const
{
    QSize sz = QSize(table()->style()->pixelMetric(QStyle::PM_IndicatorWidth),
                      table()->style()->pixelMetric(QStyle::PM_IndicatorHeight));
    sz.setWidth(sz.width() + 6);
    QSize sh(Q3TableItem::sizeHint());
    return QSize(sh.width() + sz.width(), QMAX(sh.height(), sz.height())).
        expandedTo(QApplication::globalStrut());
}

/*!
    \class Q3Table
    \brief The Q3Table class provides a flexible editable table widget.

    \compat

    Q3Table is easy to use, although it does have a large API because
    of the comprehensive functionality that it provides. Q3Table
    includes functions for manipulating \link #headers
    headers\endlink, \link #columnsrows rows and columns\endlink,
    \link #cells cells\endlink and \link #selections
    selections\endlink. Q3Table also provides in-place editing and
    drag and drop, as well as a useful set of
    \link #signals signals\endlink. Q3Table efficiently supports very
    large tables, for example, tables one million by one million cells
    are perfectly possible. Q3Table is economical with memory, using
    none for unused cells.

    \snippet doc/src/snippets/code/src_qt3support_itemviews_q3table.cpp 3

    The first line constructs the table specifying its size in rows
    and columns. We then insert a pixmap and some text into the \e
    same \link #cells cell\endlink, with the pixmap appearing to the
    left of the text. Q3Table cells can be populated with
    \l{Q3TableItem}s, \l{Q3ComboTableItem}s or by \l{Q3CheckTableItem}s.
    By default a vertical header appears at the left of the table
    showing row numbers and a horizontal header appears at the top of
    the table showing column numbers. (The numbers displayed start at
    1, although row and column numbers within Q3Table begin at 0.)

    If you want to use mouse tracking call setMouseTracking(true) on
    the \e viewport.

    \img qtableitems.png Table Items

    \target headers
    \section1 Headers

    Q3Table supports a header column, e.g. to display row numbers, and
    a header row, e.g to display column titles. To set row or column
    labels use Q3Header::setLabel() on the pointers returned by
    verticalHeader() and horizontalHeader() respectively. The vertical
    header is displayed within the table's left margin whose width is
    set with setLeftMargin(). The horizontal header is displayed
    within the table's top margin whose height is set with
    setTopMargin(). The table's grid can be switched off with
    setShowGrid(). If you want to hide a horizontal header call
    hide(), and call setTopMargin(0) so that the area the header
    would have occupied is reduced to zero size.

    Header labels are indexed via their section numbers. Note that the
    default behavior of Q3Header regarding section numbers is overridden
    for Q3Table. See the explanation below in the Rows and Columns
    section in the discussion of moving columns and rows.

    \target columnsrows
    \section1 Rows and Columns

    Row and column sizes are set with setRowHeight() and
    setColumnWidth(). If you want a row high enough to show the
    tallest item in its entirety, use adjustRow(). Similarly, to make
    a column wide enough to show the widest item use adjustColumn().
    If you want the row height and column width to adjust
    automatically as the height and width of the table changes use
    setRowStretchable() and setColumnStretchable().

    Rows and columns can be hidden and shown with hideRow(),
    hideColumn(), showRow() and showColumn(). New rows and columns are
    inserted using insertRows() and insertColumns(). Additional rows
    and columns are added at the  bottom (rows) or right (columns) if
    you set setNumRows() or setNumCols() to be larger than numRows()
    or numCols(). Existing rows and columns are removed with
    removeRow() and removeColumn(). Multiple rows and columns can be
    removed with removeRows() and removeColumns().

    Rows and columns can be set to be movable using
    rowMovingEnabled() and columnMovingEnabled(). The user can drag
    them to reorder them holding down the Ctrl key and dragging the
    mouse. For performance reasons, the default behavior of Q3Header
    section numbers is overridden by Q3Table. Currently in Q3Table, when
    a row or column is dragged and reordered, the section number is
    also changed to its new position. Therefore, there is no
    difference between the section and the index fields in Q3Header.
    The Q3Table Q3Header classes do not provide a mechanism for indexing
    independently of the user interface ordering.

    The table can be sorted using sortColumn(). Users can sort a
    column by clicking its header if setSorting() is set to true. Rows
    can be swapped with swapRows(), columns with swapColumns() and
    cells with swapCells().

    For editable tables (see setReadOnly()) you can set the read-only
    property of individual rows and columns with setRowReadOnly() and
    setColumnReadOnly(). (Whether a cell is editable or read-only
    depends on these settings and the cell's Q3TableItem.

    The row and column which have the focus are returned by
    currentRow() and currentColumn() respectively.

    Although many Q3Table functions operate in terms of rows and
    columns the indexOf() function returns a single integer
    identifying a particular cell.

    \target cells
    \section1 Cells

    All of a Q3Table's cells are empty when the table is constructed.

    There are two approaches to populating the table's cells. The
    first and simplest approach is to use Q3TableItems or Q3TableItem
    subclasses. The second approach doesn't use Q3TableItems at all
    which is useful for very large sparse tables but requires you to
    reimplement a number of functions. We'll look at each approach in
    turn.

    To put a string in a cell use setText(). This function will create
    a new Q3TableItem for the cell if one doesn't already exist, and
    displays the text in it. By default the table item's widget will
    be a QLineEdit. A pixmap may be put in a cell with setPixmap(),
    which also creates a table item if required. A cell may contain \e
    both a pixmap and text; the pixmap is displayed to the left of the
    text. Another approach is to construct a Q3TableItem or Q3TableItem
    subclass, set its properties, then insert it into a cell with
    setItem().

    If you want cells which contain comboboxes use the Q3ComboTableItem
    class. Similarly if you require cells containing checkboxes use
    the Q3CheckTableItem class. These table items look and behave just
    like the combobox or checkbox widgets but consume far less memory.

    Q3Table takes ownership of its Q3TableItems and will delete them
    when the table itself is destroyed. You can take ownership of a
    table item using takeItem() which you use to move a cell's
    contents from one cell to another, either within the same table,
    or from one table to another. (See also, swapCells()).

    In-place editing of the text in Q3TableItems, and the values in
    Q3ComboTableItems and Q3CheckTableItems works automatically. Cells
    may be editable or read-only, see Q3TableItem::EditType. If you
    want fine control over editing see beginEdit() and endEdit().

    The contents of a cell can be retrieved as a Q3TableItem using
    item(), or as a string with text() or as a pixmap (if there is
    one) with pixmap(). A cell's bounding rectangle is given by
    cellGeometry(). Use updateCell() to repaint a cell, for example to
    clear away a cell's visual representation after it has been
    deleted with clearCell(). The table can be forced to scroll to
    show a particular cell with ensureCellVisible(). The isSelected()
    function indicates if a cell is selected.

    It is possible to use your own widget as a cell's widget using
    setCellWidget(), but subclassing Q3TableItem might be a simpler
    approach. The cell's widget (if there is one) can be removed with
    clearCellWidget().

    \keyword notes on large tables
    \target bigtables
    \section2 Large tables

    For large, sparse, tables using Q3TableItems or other widgets is
    inefficient. The solution is to \e draw the cell as it should
    appear and to create and destroy cell editors on demand.

    This approach requires that you reimplement various functions.
    Reimplement paintCell() to display your data, and createEditor()
    and setCellContentFromEditor() to support in-place editing. It
    is very important to reimplement resizeData() to have no
    functionality, to prevent Q3Table from attempting to create a huge
    array. You will also need to reimplement item(), setItem(),
    takeItem(), clearCell(), and insertWidget(), cellWidget() and
    clearCellWidget(). In almost every circumstance (for sorting,
    removing and inserting columns and rows, etc.), you also need
    to reimplement swapRows(), swapCells() and swapColumns(), including
    header handling.

    If you represent active cells with a dictionary of Q3TableItems and
    QWidgets, i.e. only store references to cells that are actually
    used, many of the functions can be implemented with a single line
    of code.

    For more information on cells see the Q3TableItem documenation.

    \target selections
    \section1 Selections

    Q3Table's support single selection, multi-selection (multiple
    cells) or no selection. The selection mode is set with
    setSelectionMode(). Use isSelected() to determine if a particular
    cell is selected, and isRowSelected() and isColumnSelected() to
    see if a row or column is selected.

    Q3Table's support many simultaneous selections. You can
    programmatically select cells with addSelection(). The number of
    selections is given by numSelections(). The current selection is
    returned by currentSelection(). You can remove a selection with
    removeSelection() and remove all selections with
    clearSelection(). Selections are Q3TableSelection objects.

    To easily add a new selection use selectCells(), selectRow() or
    selectColumn().

    Alternatively, use addSelection() to add new selections using
    Q3TableSelection objects. The advantage of using Q3TableSelection
    objects is that you can call Q3TableSelection::expandTo() to resize
    the selection and can query and compare them.

    The number of selections is given by numSelections(). The current
    selection is returned by currentSelection(). You can remove a
    selection with removeSelection() and remove all selections with
    clearSelection().

    \target signals
    \section1 Signals

    When the user clicks a cell the currentChanged() signal is
    emitted. You can also connect to the lower level clicked(),
    doubleClicked() and pressed() signals. If the user changes the
    selection the selectionChanged() signal is emitted; similarly if
    the user changes a cell's value the valueChanged() signal is
    emitted. If the user right-clicks (or presses the appropriate
    platform-specific key sequence) the contextMenuRequested() signal
    is emitted. If the user drops a drag and drop object the dropped()
    signal is emitted with the drop event.
*/

/*!
    \fn void Q3Table::currentChanged(int row, int col)

    This signal is emitted when the current cell has changed to \a
    row, \a col.
*/

/*!
    \fn void Q3Table::valueChanged(int row, int col)

    This signal is emitted when the user changed the value in the cell
    at \a row, \a col.
*/

/*!
    \fn int Q3Table::currentRow() const

    Returns the current row.

    \sa currentColumn()
*/

/*!
    \fn int Q3Table::currentColumn() const

    Returns the current column.

    \sa currentRow()
*/

/*!
    \enum Q3Table::EditMode

    \value NotEditing  No cell is currently being edited.

    \value Editing  A cell is currently being edited. The editor was
    initialised with the cell's contents.

    \value Replacing  A cell is currently being edited. The editor was
    not initialised with the cell's contents.
*/

/*!
    \enum Q3Table::SelectionMode

    \value NoSelection No cell can be selected by the user.

    \value Single The user may only select a single range of cells.

    \value Multi The user may select multiple ranges of cells.

    \value SingleRow The user may select one row at once.

    \value MultiRow The user may select multiple rows.
*/

/*!
    \enum Q3Table::FocusStyle

    Specifies how the current cell (focus cell) is drawn.

    \value FollowStyle The current cell is drawn according to the
    current style and the cell's background is also drawn selected, if
    the current cell is within a selection

    \value SpreadSheet The current cell is drawn as in a spreadsheet.
    This means, it is signified by a black rectangle around the cell,
    and the background of the current cell is always drawn with the
    widget's base color - even when selected.

*/

/*!
    \fn void Q3Table::clicked(int row, int col, int button, const QPoint &mousePos)

    This signal is emitted when mouse button \a button is clicked. The
    cell where the event took place is at \a row, \a col, and the
    mouse's position is in \a mousePos.

    \sa Qt::MouseButton
*/

/*!
    \fn void Q3Table::doubleClicked(int row, int col, int button, const QPoint &mousePos)

    This signal is emitted when mouse button \a button is
    double-clicked. The cell where the event took place is at \a row,
    \a col, and the mouse's position is in \a mousePos.

    \sa Qt::MouseButton
*/

/*!
    \fn void Q3Table::pressed(int row, int col, int button, const QPoint &mousePos)

    This signal is emitted when mouse button \a button is pressed. The
    cell where the event took place is at \a row, \a col, and the
    mouse's position is in \a mousePos.

    \sa Qt::MouseButton
*/

/*!
    \fn void Q3Table::selectionChanged()

    This signal is emitted whenever a selection changes.

    \sa Q3TableSelection
*/

/*!
    \fn void Q3Table::contextMenuRequested(int row, int col, const QPoint & pos)

    This signal is emitted when the user invokes a context menu with
    the right mouse button (or with a system-specific keypress). The
    cell where the event took place is at \a row, \a col. \a pos is
    the position where the context menu will appear in the global
    coordinate system. This signal is always emitted, even if the
    contents of the cell are disabled.
*/

/*!
    Creates an empty table object called \a name as a child of \a
    parent.

    Call setNumRows() and setNumCols() to set the table size before
    populating the table if you're using Q3TableItems.
*/

Q3Table::Q3Table(QWidget *parent, const char *name)
    : Q3ScrollView(parent, name, WNoAutoErase | WStaticContents),
      leftHeader(0), topHeader(0),
      currentSel(0), lastSortCol(-1), sGrid(true), mRows(false), mCols(false),
      asc(true), doSort(true), readOnly(false)
{
    init(0, 0);
}

/*!
    Constructs an empty table called \a name with \a numRows rows and
    \a numCols columns. The table is a child of \a parent.

    If you're using \l{Q3TableItem}s to populate the table's cells, you
    can create Q3TableItem, Q3ComboTableItem and Q3CheckTableItem items
    and insert them into the table using setItem(). (See the notes on
    large tables for an alternative to using Q3TableItems.)
*/

Q3Table::Q3Table(int numRows, int numCols, QWidget *parent, const char *name)
    : Q3ScrollView(parent, name, WNoAutoErase | WStaticContents),
      leftHeader(0), topHeader(0),
      currentSel(0), lastSortCol(-1), sGrid(true), mRows(false), mCols(false),
      asc(true), doSort(true), readOnly(false)
{
    init(numRows, numCols);
}

/*! \internal
*/

void Q3Table::init(int rows, int cols)
{
#ifndef QT_NO_DRAGANDDROP
    setDragAutoScroll(false);
#endif
    d = new Q3TablePrivate;
    d->geomTimer = new QTimer(this);
    d->lastVisCol = 0;
    d->lastVisRow = 0;
    connect(d->geomTimer, SIGNAL(timeout()), this, SLOT(updateGeometriesSlot()));
    shouldClearSelection = false;
    dEnabled = false;
    roRows.setAutoDelete(true);
    roCols.setAutoDelete(true);
    setSorting(false);

    unused = true; // It's unused, ain't it? :)

    selMode = Multi;

    contents.setAutoDelete(true);
    widgets.setAutoDelete(true);

    // Enable clipper and set background mode
    enableClipper(qt_table_clipper_enabled);

    viewport()->setFocusProxy(this);
    viewport()->setFocusPolicy(Qt::WheelFocus);
    setFocusPolicy(Qt::WheelFocus);

    viewport()->setBackgroundMode(PaletteBase);
    setBackgroundMode(PaletteBackground, PaletteBase);
    setResizePolicy(Manual);
    selections.setAutoDelete(true);

    // Create headers
    leftHeader = new Q3TableHeader(rows, this, this, "left table header");
    leftHeader->setOrientation(Vertical);
    leftHeader->setTracking(true);
    leftHeader->setMovingEnabled(true);
    topHeader = new Q3TableHeader(cols, this, this, "right table header");
    topHeader->setOrientation(Horizontal);
    topHeader->setTracking(true);
    topHeader->setMovingEnabled(true);
    if (QApplication::reverseLayout())
        setMargins(0, fontMetrics().height() + 4, 30, 0);
    else
        setMargins(30, fontMetrics().height() + 4, 0, 0);

    topHeader->setUpdatesEnabled(false);
    leftHeader->setUpdatesEnabled(false);
    // Initialize headers
    int i = 0;
    for (i = 0; i < numCols(); ++i)
        topHeader->resizeSection(i, QMAX(100, QApplication::globalStrut().height()));
    for (i = 0; i < numRows(); ++i)
        leftHeader->resizeSection(i, QMAX(20, QApplication::globalStrut().width()));
    topHeader->setUpdatesEnabled(true);
    leftHeader->setUpdatesEnabled(true);

    // Prepare for contents
    contents.setAutoDelete(false);

    // Connect header, table and scroll bars
    connect(horizontalScrollBar(), SIGNAL(valueChanged(int)),
             topHeader, SLOT(setOffset(int)));
    connect(verticalScrollBar(), SIGNAL(valueChanged(int)),
             leftHeader, SLOT(setOffset(int)));
    connect(topHeader, SIGNAL(sectionSizeChanged(int)),
             this, SLOT(columnWidthChanged(int)));
    connect(topHeader, SIGNAL(indexChange(int,int,int)),
             this, SLOT(columnIndexChanged(int,int,int)));
    connect(topHeader, SIGNAL(sectionClicked(int)),
             this, SLOT(columnClicked(int)));
    connect(leftHeader, SIGNAL(sectionSizeChanged(int)),
             this, SLOT(rowHeightChanged(int)));
    connect(leftHeader, SIGNAL(indexChange(int,int,int)),
             this, SLOT(rowIndexChanged(int,int,int)));

    // Initialize variables
    autoScrollTimer = new QTimer(this);
    connect(autoScrollTimer, SIGNAL(timeout()),
             this, SLOT(doAutoScroll()));
    curRow = curCol = 0;
    topHeader->setSectionState(curCol, Q3TableHeader::Bold);
    leftHeader->setSectionState(curRow, Q3TableHeader::Bold);
    edMode = NotEditing;
    editRow = editCol = -1;

    drawActiveSelection = true;

    installEventFilter(this);

    focusStl = SpreadSheet;

    was_visible = false;

    // initial size
    resize(640, 480);
}

/*!
    Releases all the resources used by the Q3Table object,
    including all \l{Q3TableItem}s and their widgets.
*/

Q3Table::~Q3Table()
{
    setUpdatesEnabled(false);
    contents.setAutoDelete(true);
    contents.clear();
    widgets.clear();

    delete d;
}

void Q3Table::setReadOnly(bool b)
{
    readOnly = b;

    Q3TableItem *i = item(curRow, curCol);
    if (readOnly && isEditing()) {
        endEdit(editRow, editCol, true, false);
    } else if (!readOnly && i && (i->editType() == Q3TableItem::WhenCurrent
                                  || i->editType() == Q3TableItem::Always)) {
        editCell(curRow, curCol);
    }
}

/*!
    If \a ro is true, row \a row is set to be read-only; otherwise the
    row is set to be editable.

    Whether a cell in this row is editable or read-only depends on the
    cell's EditType, and this setting.

    \sa isRowReadOnly() setColumnReadOnly() setReadOnly()
*/

void Q3Table::setRowReadOnly(int row, bool ro)
{
    if (ro)
        roRows.replace(row, new int(0));
    else
        roRows.remove(row);

    if (curRow == row) {
        Q3TableItem *i = item(curRow, curCol);
        if (ro && isEditing()) {
            endEdit(editRow, editCol, true, false);
        } else if (!ro && i && (i->editType() == Q3TableItem::WhenCurrent
                                      || i->editType() == Q3TableItem::Always)) {
            editCell(curRow, curCol);
        }
    }
}

/*!
    If \a ro is true, column \a col is set to be read-only; otherwise
    the column is set to be editable.

    Whether a cell in this column is editable or read-only depends on
    the cell's EditType, and this setting.

    \sa isColumnReadOnly() setRowReadOnly() setReadOnly()

*/

void Q3Table::setColumnReadOnly(int col, bool ro)
{
    if (ro)
        roCols.replace(col, new int(0));
    else
        roCols.remove(col);

    if (curCol == col) {
        Q3TableItem *i = item(curRow, curCol);
        if (ro && isEditing()) {
            endEdit(editRow, editCol, true, false);
        } else if (!ro && i && (i->editType() == Q3TableItem::WhenCurrent
                                      || i->editType() == Q3TableItem::Always)) {
            editCell(curRow, curCol);
        }
    }
}

/*!
    \property Q3Table::readOnly
    \brief whether the table is read-only

    Whether a cell in the table is editable or read-only depends on
    the cell's \link Q3TableItem::EditType EditType\endlink, and this setting.

    \sa QWidget::enabled setColumnReadOnly() setRowReadOnly()
*/

bool Q3Table::isReadOnly() const
{
    return readOnly;
}

/*!
    Returns true if row \a row is read-only; otherwise returns false.

    Whether a cell in this row is editable or read-only depends on the
    cell's \link Q3TableItem::EditType EditType\endlink, and this
    setting.

    \sa setRowReadOnly() isColumnReadOnly()
*/

bool Q3Table::isRowReadOnly(int row) const
{
    return (roRows.find(row) != 0);
}

/*!
    Returns true if column \a col is read-only; otherwise returns
    false.

    Whether a cell in this column is editable or read-only depends on
    the cell's EditType, and this setting.

    \sa setColumnReadOnly() isRowReadOnly()
*/

bool Q3Table::isColumnReadOnly(int col) const
{
    return (roCols.find(col) != 0);
}

void Q3Table::setSelectionMode(SelectionMode mode)
{
    if (mode == selMode)
        return;
    selMode = mode;
    clearSelection();
    if (isRowSelection(selMode) && numRows() > 0 && numCols() > 0) {
        currentSel = new Q3TableSelection();
        selections.append(currentSel);
        currentSel->init(curRow, 0);
        currentSel->expandTo(curRow, numCols() - 1);
        repaintSelections(0, currentSel);
    }
}

/*!
    \property Q3Table::selectionMode
    \brief the current selection mode

    The default mode is \c Multi which allows the user to select
    multiple ranges of cells.
*/

Q3Table::SelectionMode Q3Table::selectionMode() const
{
    return selMode;
}

/*!
    \property Q3Table::focusStyle
    \brief how the current (focus) cell is drawn

    The default style is \c SpreadSheet.

    \sa Q3Table::FocusStyle
*/

void Q3Table::setFocusStyle(FocusStyle fs)
{
    focusStl = fs;
    updateCell(curRow, curCol);
}

Q3Table::FocusStyle Q3Table::focusStyle() const
{
    return focusStl;
}

/*!
    This functions updates all the header states to be in sync with
    the current selections. This should be called after
    programmatically changing, adding or removing selections, so that
    the headers are updated.
*/

void Q3Table::updateHeaderStates()
{
    horizontalHeader()->setUpdatesEnabled(false);
    verticalHeader()->setUpdatesEnabled(false);

    ((Q3TableHeader*)verticalHeader())->setSectionStateToAll(Q3TableHeader::Normal);
    ((Q3TableHeader*)horizontalHeader())->setSectionStateToAll(Q3TableHeader::Normal);

    Q3PtrListIterator<Q3TableSelection> it(selections);
    Q3TableSelection *s;
    while ((s = it.current()) != 0) {
        ++it;
        if (s->isActive()) {
            if (s->leftCol() == 0 &&
                 s->rightCol() == numCols() - 1) {
                for (int i = 0; i < s->bottomRow() - s->topRow() + 1; ++i)
                    leftHeader->setSectionState(s->topRow() + i, Q3TableHeader::Selected);
            }
            if (s->topRow() == 0 &&
                 s->bottomRow() == numRows() - 1) {
                for (int i = 0; i < s->rightCol() - s->leftCol() + 1; ++i)
                    topHeader->setSectionState(s->leftCol() + i, Q3TableHeader::Selected);
            }
        }
    }

    horizontalHeader()->setUpdatesEnabled(true);
    verticalHeader()->setUpdatesEnabled(true);
    horizontalHeader()->repaint(false);
    verticalHeader()->repaint(false);
}

/*!
    Returns the table's top Q3Header.

    This header contains the column labels.

    To modify a column label use Q3Header::setLabel().

    \sa verticalHeader() setTopMargin() Q3Header
*/

Q3Header *Q3Table::horizontalHeader() const
{
    return (Q3Header*)topHeader;
}

/*!
    Returns the table's vertical Q3Header.

    This header contains the row labels.

    \sa horizontalHeader() setLeftMargin() Q3Header
*/

Q3Header *Q3Table::verticalHeader() const
{
    return (Q3Header*)leftHeader;
}

void Q3Table::setShowGrid(bool b)
{
    if (sGrid == b)
        return;
    sGrid = b;
    updateContents();
}

/*!
    \property Q3Table::showGrid
    \brief whether the table's grid is displayed

    The grid is shown by default.
*/

bool Q3Table::showGrid() const
{
    return sGrid;
}

/*!
    \property Q3Table::columnMovingEnabled
    \brief whether columns can be moved by the user

    The default is false. Columns are moved by dragging whilst holding
    down the Ctrl key.

    \sa rowMovingEnabled
*/

void Q3Table::setColumnMovingEnabled(bool b)
{
    mCols = b;
}

bool Q3Table::columnMovingEnabled() const
{
    return mCols;
}

/*!
    \property Q3Table::rowMovingEnabled
    \brief whether rows can be moved by the user

    The default is false. Rows are moved by dragging whilst holding
    down the Ctrl key.


    \sa columnMovingEnabled
*/

void Q3Table::setRowMovingEnabled(bool b)
{
    mRows = b;
}

bool Q3Table::rowMovingEnabled() const
{
    return mRows;
}

/*!
    This is called when Q3Table's internal array needs to be resized to
    \a len elements.

    If you don't use Q3TableItems you should reimplement this as an
    empty method to avoid wasting memory. See the notes on large
    tables for further details.
*/

void Q3Table::resizeData(int len)
{
    contents.resize(len);
    widgets.resize(len);
}

/*!
    Swaps the data in \a row1 and \a row2.

    This function is used to swap the positions of two rows. It is
    called when the user changes the order of rows (see
    setRowMovingEnabled()), and when rows are sorted.

    If you don't use \l{Q3TableItem}s and want your users to be able to
    swap rows, e.g. for sorting, you will need to reimplement this
    function. (See the notes on large tables.)

    If \a swapHeader is true, the rows' header contents is also
    swapped.

    This function will not update the Q3Table, you will have to do
    this manually, e.g. by calling updateContents().

    \sa swapColumns() swapCells()
*/

void Q3Table::swapRows(int row1, int row2, bool swapHeader)
{
    if (swapHeader)
        leftHeader->swapSections(row1, row2, false);

    Q3PtrVector<Q3TableItem> tmpContents;
    tmpContents.resize(numCols());
    Q3PtrVector<QWidget> tmpWidgets;
    tmpWidgets.resize(numCols());
    int i;

    contents.setAutoDelete(false);
    widgets.setAutoDelete(false);
    for (i = 0; i < numCols(); ++i) {
        Q3TableItem *i1, *i2;
        i1 = item(row1, i);
        i2 = item(row2, i);
        if (i1 || i2) {
            tmpContents.insert(i, i1);
            contents.remove(indexOf(row1, i));
            contents.insert(indexOf(row1, i), i2);
            contents.remove(indexOf(row2, i));
            contents.insert(indexOf(row2, i), tmpContents[ i ]);
            if (contents[ indexOf(row1, i) ])
                contents[ indexOf(row1, i) ]->setRow(row1);
            if (contents[ indexOf(row2, i) ])
                contents[ indexOf(row2, i) ]->setRow(row2);
        }

        QWidget *w1, *w2;
	w1 = cellWidget(row1, i);
        w2 = cellWidget(row2, i);
        if (w1 || w2) {
            tmpWidgets.insert(i, w1);
            widgets.remove(indexOf(row1, i));
            widgets.insert(indexOf(row1, i), w2);
            widgets.remove(indexOf(row2, i));
            widgets.insert(indexOf(row2, i), tmpWidgets[ i ]);
        }
    }
    contents.setAutoDelete(false);
    widgets.setAutoDelete(true);

    updateRowWidgets(row1);
    updateRowWidgets(row2);
    if (curRow == row1)
        curRow = row2;
    else if (curRow == row2)
        curRow = row1;
    if (editRow == row1)
        editRow = row2;
    else if (editRow == row2)
        editRow = row1;
}

/*!
    Sets the left margin to be \a m pixels wide.

    The verticalHeader(), which displays row labels, occupies this
    margin.

    In an Arabic or Hebrew localization, the verticalHeader() will
    appear on the right side of the table, and this call will set the
    right margin.

    \sa leftMargin() setTopMargin() verticalHeader()
*/

void Q3Table::setLeftMargin(int m)
{
    if (QApplication::reverseLayout())
        setMargins(leftMargin(), topMargin(), m, bottomMargin());
    else
        setMargins(m, topMargin(), rightMargin(), bottomMargin());
    updateGeometries();
}

/*!
    Sets the top margin to be \a m pixels high.

    The horizontalHeader(), which displays column labels, occupies
    this margin.

    \sa topMargin() setLeftMargin()
*/

void Q3Table::setTopMargin(int m)
{
    setMargins(leftMargin(), m, rightMargin(), bottomMargin());
    updateGeometries();
}

/*!
    Swaps the data in \a col1 with \a col2.

    This function is used to swap the positions of two columns. It is
    called when the user changes the order of columns (see
    setColumnMovingEnabled(), and when columns are sorted.

    If you don't use \l{Q3TableItem}s and want your users to be able to
    swap columns you will need to reimplement this function. (See the
    notes on large tables.)

    If \a swapHeader is true, the columns' header contents is also
    swapped.

    \sa swapCells()
*/

void Q3Table::swapColumns(int col1, int col2, bool swapHeader)
{
    if (swapHeader)
        topHeader->swapSections(col1, col2, false);

    Q3PtrVector<Q3TableItem> tmpContents;
    tmpContents.resize(numRows());
    Q3PtrVector<QWidget> tmpWidgets;
    tmpWidgets.resize(numRows());
    int i;

    contents.setAutoDelete(false);
    widgets.setAutoDelete(false);
    for (i = 0; i < numRows(); ++i) {
        Q3TableItem *i1, *i2;
        i1 = item(i, col1);
        i2 = item(i, col2);
        if (i1 || i2) {
            tmpContents.insert(i, i1);
            contents.remove(indexOf(i, col1));
            contents.insert(indexOf(i, col1), i2);
            contents.remove(indexOf(i, col2));
            contents.insert(indexOf(i, col2), tmpContents[ i ]);
            if (contents[ indexOf(i, col1) ])
                contents[ indexOf(i, col1) ]->setCol(col1);
            if (contents[ indexOf(i, col2) ])
                contents[ indexOf(i, col2) ]->setCol(col2);
        }

        QWidget *w1, *w2;
        w1 = cellWidget(i, col1);
        w2 = cellWidget(i, col2);
        if (w1 || w2) {
            tmpWidgets.insert(i, w1);
            widgets.remove(indexOf(i, col1));
            widgets.insert(indexOf(i, col1), w2);
            widgets.remove(indexOf(i, col2));
            widgets.insert(indexOf(i, col2), tmpWidgets[ i ]);
        }
    }
    contents.setAutoDelete(false);
    widgets.setAutoDelete(true);

    columnWidthChanged(col1);
    columnWidthChanged(col2);
    if (curCol == col1)
        curCol = col2;
    else if (curCol == col2)
        curCol = col1;
    if (editCol == col1)
        editCol = col2;
    else if (editCol == col2)
        editCol = col1;
}

/*!
    Swaps the contents of the cell at \a row1, \a col1 with the
    contents of the cell at \a row2, \a col2.

    This function is also called when the table is sorted.

    If you don't use \l{Q3TableItem}s and want your users to be able to
    swap cells, you will need to reimplement this function. (See the
    notes on large tables.)

    \sa swapColumns() swapRows()
*/

void Q3Table::swapCells(int row1, int col1, int row2, int col2)
{
    contents.setAutoDelete(false);
    widgets.setAutoDelete(false);
    Q3TableItem *i1, *i2;
    i1 = item(row1, col1);
    i2 = item(row2, col2);
    if (i1 || i2) {
        Q3TableItem *tmp = i1;
        contents.remove(indexOf(row1, col1));
        contents.insert(indexOf(row1, col1), i2);
        contents.remove(indexOf(row2, col2));
        contents.insert(indexOf(row2, col2), tmp);
        if (contents[ indexOf(row1, col1) ]) {
            contents[ indexOf(row1, col1) ]->setRow(row1);
            contents[ indexOf(row1, col1) ]->setCol(col1);
        }
        if (contents[ indexOf(row2, col2) ]) {
            contents[ indexOf(row2, col2) ]->setRow(row2);
            contents[ indexOf(row2, col2) ]->setCol(col2);
        }
    }

    QWidget *w1, *w2;
    w1 = cellWidget(row1, col1);
    w2 = cellWidget(row2, col2);
    if (w1 || w2) {
        QWidget *tmp = w1;
        widgets.remove(indexOf(row1, col1));
        widgets.insert(indexOf(row1, col1), w2);
        widgets.remove(indexOf(row2, col2));
        widgets.insert(indexOf(row2, col2), tmp);
    }

    updateRowWidgets(row1);
    updateRowWidgets(row2);
    updateColWidgets(col1);
    updateColWidgets(col2);
    contents.setAutoDelete(false);
    widgets.setAutoDelete(true);
}

static bool is_child_of(QWidget *child, QWidget *parent)
{
    while (child) {
        if (child == parent)
            return true;
        child = child->parentWidget();
    }
    return false;
}

/*!
    Draws the table contents on the painter \a p. This function is
    optimized so that it only draws the cells inside the \a cw pixels
    wide and \a ch pixels high clipping rectangle at position \a cx,
    \a cy.

    Additionally, drawContents() highlights the current cell.
*/

void Q3Table::drawContents(QPainter *p, int cx, int cy, int cw, int ch)
{
    int colfirst = columnAt(cx);
    int collast = columnAt(cx + cw);
    int rowfirst = rowAt(cy);
    int rowlast = rowAt(cy + ch);

    if (rowfirst == -1 || colfirst == -1) {
        paintEmptyArea(p, cx, cy, cw, ch);
        return;
    }

    drawActiveSelection = hasFocus() || viewport()->hasFocus() || d->inMenuMode
                        || is_child_of(qApp->focusWidget(), viewport())
                        || !style()->styleHint(QStyle::SH_ItemView_ChangeHighlightOnFocus, 0, this);
    if (rowlast == -1)
        rowlast = numRows() - 1;
    if (collast == -1)
        collast = numCols() - 1;

    bool currentInSelection = false;

    Q3PtrListIterator<Q3TableSelection> it( selections );
    Q3TableSelection *s;
    while ( ( s = it.current() ) != 0 ) {
        ++it;
        if (s->isActive() &&
             curRow >= s->topRow() &&
             curRow <= s->bottomRow() &&
             curCol >= s->leftCol() &&
             curCol <= s->rightCol()) {
            currentInSelection = s->topRow() != curRow || s->bottomRow() != curRow || s->leftCol() != curCol || s->rightCol() != curCol;
            break;
        }
    }

    // Go through the rows
    for (int r = rowfirst; r <= rowlast; ++r) {
        // get row position and height
        int rowp = rowPos(r);
        int rowh = rowHeight(r);

        // Go through the columns in row r
        // if we know from where to where, go through [colfirst, collast],
        // else go through all of them
        for (int c = colfirst; c <= collast; ++c) {
            // get position and width of column c
            int colp, colw;
            colp = columnPos(c);
            colw = columnWidth(c);
            int oldrp = rowp;
            int oldrh = rowh;

            Q3TableItem *itm = item(r, c);
            if (itm &&
                 (itm->colSpan() > 1 || itm->rowSpan() > 1)) {
                bool goon = (r == itm->row() && c == itm->col())
                            || (r == rowfirst && c == itm->col())
                            || (r == itm->row() && c == colfirst);
                if (!goon)
                    continue;
                rowp = rowPos(itm->row());
                rowh = 0;
                int i;
                for (i = 0; i < itm->rowSpan(); ++i)
                    rowh += rowHeight(i + itm->row());
                colp = columnPos(itm->col());
                colw = 0;
                for (i = 0; i < itm->colSpan(); ++i)
                    colw += columnWidth(i + itm->col());
            }

            // Translate painter and draw the cell
            p->translate(colp, rowp);
            bool selected = isSelected(r, c);
            if (focusStl != FollowStyle && selected && !currentInSelection &&
                 r == curRow && c == curCol )
                selected = false;
            paintCell(p, r, c, QRect(colp, rowp, colw, rowh), selected);
            p->translate(-colp, -rowp);

            rowp = oldrp;
            rowh = oldrh;

            QWidget *w = cellWidget(r, c);
            QRect cg(cellGeometry(r, c));
            if (w && w->geometry() != QRect(contentsToViewport(cg.topLeft()), cg.size() - QSize(1, 1))) {
                moveChild(w, colp, rowp);
                w->resize(cg.size() - QSize(1, 1));
            }
        }
    }
    d->lastVisCol = collast;
    d->lastVisRow = rowlast;

    // draw indication of current cell
    QRect focusRect = cellGeometry(curRow, curCol);
    p->translate(focusRect.x(), focusRect.y());
    paintFocus(p, focusRect);
    p->translate(-focusRect.x(), -focusRect.y());

    // Paint empty rects
    paintEmptyArea(p, cx, cy, cw, ch);

    drawActiveSelection = true;
}

/*!
    \reimp

    (Implemented to get rid of a compiler warning.)
*/

void Q3Table::drawContents(QPainter *)
{
}

/*!
    Returns the geometry of cell \a row, \a col in the cell's
    coordinate system. This is a convenience function useful in
    paintCell(). It is equivalent to QRect(QPoint(0,0), cellGeometry(
    row, col).size());

    \sa cellGeometry()
*/

QRect Q3Table::cellRect(int row, int col) const
{
    return QRect(QPoint(0,0), cellGeometry(row, col).size());
}

/*!
    \overload

    Use the other paintCell() function. This function is only included
    for backwards compatibility.
*/

void Q3Table::paintCell(QPainter* p, int row, int col,
                        const QRect &cr, bool selected)
{
    if (cr.width() == 0 || cr.height() == 0)
        return;
#if defined(Q_WS_WIN)
    const QColorGroup &cg = (!drawActiveSelection && style()->styleHint(QStyle::SH_ItemView_ChangeHighlightOnFocus) ? palette().inactive() : colorGroup());
#else
    const QColorGroup &cg = colorGroup();
#endif

    Q3TableItem *itm = item(row, col);
    QColorGroup cg2(cg);
    if (itm && !itm->isEnabled())
        cg2 = palette().disabled();

    paintCell(p, row, col, cr, selected, cg2);
}

/*!
    Paints the cell at \a row, \a col on the painter \a p. The painter
    has already been translated to the cell's origin. \a cr describes
    the cell coordinates in the content coordinate system.

    If \a selected is true the cell is highlighted.

    \a cg is the colorgroup which should be used to draw the cell
    content.

    If you want to draw custom cell content, for example right-aligned
    text, you must either reimplement paintCell(), or subclass
    Q3TableItem and reimplement Q3TableItem::paint() to do the custom
    drawing.

    If you're using a Q3TableItem subclass, for example, to store a
    data structure, then reimplementing Q3TableItem::paint() may be the
    best approach. For data you want to draw immediately, e.g. data
    retrieved from a database, it is probably best to reimplement
    paintCell(). Note that if you reimplement paintCell(), i.e. don't
    use \l{Q3TableItem}s, you must reimplement other functions: see the
    notes on large tables.

    Note that the painter is not clipped by default in order to get
    maximum efficiency. If you want clipping, use code like this:

    \snippet doc/src/snippets/code/src_qt3support_itemviews_q3table.cpp 4
*/

void Q3Table::paintCell(QPainter *p, int row, int col,
                        const QRect &cr, bool selected, const QColorGroup &cg)
{
    if (focusStl == SpreadSheet && selected &&
         row == curRow &&
         col == curCol && (hasFocus() || viewport()->hasFocus()))
        selected = false;

    QPalette pal = cg;
    int w = cr.width();
    int h = cr.height();
    int x2 = w - 1;
    int y2 = h - 1;


    Q3TableItem *itm = item(row, col);
    if (itm) {
        p->save();
        itm->paint(p, pal, cr, selected);
        p->restore();
    } else {
        p->fillRect(0, 0, w, h, selected ? pal.brush(QPalette::Highlight) : pal.brush(QPalette::Base));
    }

    if (sGrid) {
        // Draw our lines
        QPen pen(p->pen());
        int gridColor =        style()->styleHint(QStyle::SH_Table_GridLineColor, 0, this);
        if (gridColor != -1) {
            if (palette() != pal)
                p->setPen(pal.mid().color());
            else
                p->setPen((QRgb)gridColor);
        } else {
            p->setPen(pal.mid().color());
        }
        p->drawLine(x2, 0, x2, y2);
        p->drawLine(0, y2, x2, y2);
        p->setPen(pen);
    }
}

/*!
    Draws the focus rectangle of the current cell (see currentRow(),
    currentColumn()).

    The painter \a p is already translated to the cell's origin, while
    \a cr specifies the cell's geometry in content coordinates.
*/

void Q3Table::paintFocus(QPainter *p, const QRect &cr)
{
    if (!hasFocus() && !viewport()->hasFocus())
        return;
    QRect focusRect(0, 0, cr.width(), cr.height());
    if (focusStyle() == SpreadSheet) {
        p->setPen(QPen(Qt::black, 1));
        p->setBrush(Qt::NoBrush);
        p->drawRect(focusRect.x(), focusRect.y(), focusRect.width() - 1, focusRect.height() - 1);
        p->drawRect(focusRect.x() - 1, focusRect.y() - 1, focusRect.width() + 1, focusRect.height() + 1);
    } else {
        QStyleOptionFocusRect opt;
        opt.init(this);
        opt.rect = focusRect;
        opt.palette = palette();
        opt.state |= QStyle::State_KeyboardFocusChange;
        if (isSelected(curRow, curCol, false)) {
            opt.state |= QStyle::State_FocusAtBorder;
            opt.backgroundColor = palette().highlight().color();
        } else {
            opt.state |= QStyle::State_None;
            opt.backgroundColor = palette().base().color();
        }
        style()->drawPrimitive(QStyle::PE_FrameFocusRect, &opt, p, this);
    }
}

/*!
    This function fills the \a cw pixels wide and \a ch pixels high
    rectangle starting at position \a cx, \a cy with the background
    color using the painter \a p.

    paintEmptyArea() is invoked by drawContents() to erase or fill
    unused areas.
*/

void Q3Table::paintEmptyArea(QPainter *p, int cx, int cy, int cw, int ch)
{
    // Regions work with shorts, so avoid an overflow and adjust the
    // table size to the visible size
    QSize ts(tableSize());
    ts.setWidth(QMIN(ts.width(), visibleWidth()));
    ts.setHeight(QMIN(ts.height(), visibleHeight()));

    // Region of the rect we should draw, calculated in viewport
    // coordinates, as a region can't handle bigger coordinates
    contentsToViewport2(cx, cy, cx, cy);
    QRegion reg(QRect(cx, cy, cw, ch));

    // Subtract the table from it
    reg = reg.subtracted(QRect(QPoint(0, 0), ts));

    // And draw the rectangles (transformed inc contents coordinates as needed)
    Q3MemArray<QRect> r = reg.rects();
    for (int i = 0; i < (int)r.count(); ++i)
        p->fillRect(QRect(viewportToContents2(r[i].topLeft()),r[i].size()), viewport()->backgroundBrush());
}

/*!
    Returns the Q3TableItem representing the contents of the cell at \a
    row, \a col.

    If \a row or \a col are out of range or no content has been set
    for this cell, item() returns 0.

    If you don't use \l{Q3TableItem}s you may need to reimplement this
    function: see the notes on large tables.

    \sa setItem()
*/

Q3TableItem *Q3Table::item(int row, int col) const
{
    if (row < 0 || col < 0 || row > numRows() - 1 ||
         col > numCols() - 1 || row * col >= (int)contents.size())
        return 0;

    return contents[ indexOf(row, col) ];        // contents array lookup
}

/*!
    Inserts the table item \a item into the table at row \a row,
    column \a col, and repaints the cell. If a table item already
    exists in this cell it is deleted and replaced with \a item. The
    table takes ownership of the table item.

    If you don't use \l{Q3TableItem}s you may need to reimplement this
    function: see the notes on large tables.

    \sa item() takeItem()
*/

void Q3Table::setItem(int row, int col, Q3TableItem *item)
{
    if (!item)
        return;

    if ((int)contents.size() != numRows() * numCols())
        resizeData(numRows() * numCols());

    int orow = item->row();
    int ocol = item->col();
    clearCell(row, col);

    contents.insert(indexOf(row, col), item);
    item->setRow(row);
    item->setCol(col);
    item->t = this;
    updateCell(row, col);
    if (qt_update_cell_widget)
        item->updateEditor(orow, ocol);

    if (row == curRow && col == curCol && item->editType() == Q3TableItem::WhenCurrent) {
        if (beginEdit(row, col, false))
            setEditMode(Editing, row, col);
    }
}

/*!
    Removes the Q3TableItem at \a row, \a col.

    If you don't use \l{Q3TableItem}s you may need to reimplement this
    function: see the notes on large tables.
*/

void Q3Table::clearCell(int row, int col)
{
    if ((int)contents.size() != numRows() * numCols())
        resizeData(numRows() * numCols());
    clearCellWidget(row, col);
    contents.setAutoDelete(true);
    contents.remove(indexOf(row, col));
    contents.setAutoDelete(false);
}

/*!
    Sets the text in the cell at \a row, \a col to \a text.

    If the cell does not contain a table item a Q3TableItem is created
    with an \link Q3TableItem::EditType EditType\endlink of \c OnTyping,
    otherwise the existing table item's text (if any) is replaced with
    \a text.

    \sa text() setPixmap() setItem() Q3TableItem::setText()
*/

void Q3Table::setText(int row, int col, const QString &text)
{
    Q3TableItem *itm = item(row, col);
    if (itm) {
        itm->setText(text);
        itm->updateEditor(row, col);
        updateCell(row, col);
    } else {
        Q3TableItem *i = new Q3TableItem(this, Q3TableItem::OnTyping,
                                        text, QPixmap());
        setItem(row, col, i);
    }
}

/*!
    Sets the pixmap in the cell at \a row, \a col to \a pix.

    If the cell does not contain a table item a Q3TableItem is created
    with an \link Q3TableItem::EditType EditType\endlink of \c OnTyping,
    otherwise the existing table item's pixmap (if any) is replaced
    with \a pix.

    Note that \l{Q3ComboTableItem}s and \l{Q3CheckTableItem}s don't show
    pixmaps.

    \sa pixmap() setText() setItem() Q3TableItem::setPixmap()
*/

void Q3Table::setPixmap(int row, int col, const QPixmap &pix)
{
    Q3TableItem *itm = item(row, col);
    if (itm) {
        itm->setPixmap(pix);
        updateCell(row, col);
    } else {
        Q3TableItem *i = new Q3TableItem(this, Q3TableItem::OnTyping,
                                        QString(), pix);
        setItem(row, col, i);
    }
}

/*!
    Returns the text in the cell at \a row, \a col, or an empty string
    if the relevant item does not exist or has no text.

    \sa setText() setPixmap()
*/

QString Q3Table::text(int row, int col) const
{
    Q3TableItem *itm = item(row, col);
    if (itm)
        return itm->text();
    return QString();
}

/*!
    Returns the pixmap set for the cell at \a row, \a col, or a
    null-pixmap if the cell contains no pixmap.

    \sa setPixmap()
*/

QPixmap Q3Table::pixmap(int row, int col) const
{
    Q3TableItem *itm = item(row, col);
    if (itm)
        return itm->pixmap();
    return QPixmap();
}

/*!
    Moves the focus to the cell at \a row, \a col.

    \sa currentRow() currentColumn()
*/

void Q3Table::setCurrentCell(int row, int col)
{
    setCurrentCell(row, col, true, true);
}

// need to use a define, as leftMargin() is protected
#define VERTICALMARGIN \
(QApplication::reverseLayout() ? \
       rightMargin() \
       : \
       leftMargin() \
)

/*!
    \reimp
*/
QVariant Q3Table::inputMethodQuery(Qt::InputMethodQuery query) const
{
    if (query == Qt::ImMicroFocus)
        return QRect(columnPos(curCol) + leftMargin() - contentsX(), rowPos(curRow) + topMargin() - contentsY(),
                     columnWidth(curCol), rowHeight(curRow));
    return QWidget::inputMethodQuery(query);

}

/*! \internal */

void Q3Table::setCurrentCell(int row, int col, bool updateSelections, bool ensureVisible)
{
    Q3TableItem *oldItem = item(curRow, curCol);

    if (row > numRows() - 1)
        row = numRows() - 1;
    if (col > numCols() - 1)
        col = numCols() - 1;

    if (curRow == row && curCol == col)
        return;


    Q3TableItem *itm = oldItem;
    if (itm && itm->editType() != Q3TableItem::Always && itm->editType() != Q3TableItem::Never)
        endEdit(curRow, curCol, true, false);
    int oldRow = curRow;
    int oldCol = curCol;
    curRow = row;
    curCol = col;
    repaintCell(oldRow, oldCol);
    repaintCell(curRow, curCol);
    if (ensureVisible)
        ensureCellVisible(curRow, curCol);
    emit currentChanged(row, col);

    if (oldCol != curCol) {
        if (!isColumnSelected(oldCol))
            topHeader->setSectionState(oldCol, Q3TableHeader::Normal);
        else if (isRowSelection(selectionMode()))
            topHeader->setSectionState(oldCol, Q3TableHeader::Selected);
        topHeader->setSectionState(curCol, isColumnSelected(curCol, true) ?
                                    Q3TableHeader::Selected : Q3TableHeader::Bold);
    }

    if (oldRow != curRow) {
        if (!isRowSelected(oldRow))
            leftHeader->setSectionState(oldRow, Q3TableHeader::Normal);
        leftHeader->setSectionState(curRow, isRowSelected(curRow, true) ?
                                     Q3TableHeader::Selected : Q3TableHeader::Bold);
    }

    itm = item(curRow, curCol);


    if (cellWidget(oldRow, oldCol) &&
         cellWidget(oldRow, oldCol)->hasFocus())
        viewport()->setFocus();

    if (itm && itm->editType() == Q3TableItem::WhenCurrent) {
        if (beginEdit(curRow, curCol, false))
            setEditMode(Editing, row, col);
    } else if (itm && itm->editType() == Q3TableItem::Always) {
        if (cellWidget(itm->row(), itm->col()))
            cellWidget(itm->row(), itm->col())->setFocus();
    }

    if (updateSelections && isRowSelection(selectionMode()) &&
         !isSelected(curRow, curCol, false)) {
        if (selectionMode() == Q3Table::SingleRow)
            clearSelection();
        currentSel = new Q3TableSelection();
        selections.append(currentSel);
        currentSel->init(curRow, 0);
        currentSel->expandTo(curRow, numCols() - 1);
        repaintSelections(0, currentSel);
    }
}

/*!
    Scrolls the table until the cell at \a row, \a col becomes
    visible.
*/

void Q3Table::ensureCellVisible(int row, int col)
{
    if (!updatesEnabled() || !viewport()->updatesEnabled())
        return;
    int cw = columnWidth(col);
    int rh = rowHeight(row);
    if (cw < visibleWidth())
        ensureVisible(columnPos(col) + cw / 2, rowPos(row) + rh / 2, cw / 2, rh / 2);
    else
        ensureVisible(columnPos(col), rowPos(row) + rh / 2, 0, rh / 2);
}

/*!
    Returns true if the cell at \a row, \a col is selected; otherwise
    returns false.

    \sa isRowSelected() isColumnSelected()
*/

bool Q3Table::isSelected(int row, int col) const
{
    return isSelected(row, col, true);
}

/*! \internal */

bool Q3Table::isSelected(int row, int col, bool includeCurrent) const
{
    Q3PtrListIterator<Q3TableSelection> it(selections);
    Q3TableSelection *s;
    while ((s = it.current()) != 0) {
        ++it;
        if (s->isActive() &&
             row >= s->topRow() &&
             row <= s->bottomRow() &&
             col >= s->leftCol() &&
             col <= s->rightCol())
            return true;
        if (includeCurrent && row == currentRow() && col == currentColumn())
            return true;
    }
    return false;
}

/*!
    Returns true if row \a row is selected; otherwise returns false.

    If \a full is false (the default), 'row is selected' means that at
    least one cell in the row is selected. If \a full is true, then 'row
    is selected' means every cell in the row is selected.

    \sa isColumnSelected() isSelected()
*/

bool Q3Table::isRowSelected(int row, bool full) const
{
    if (!full) {
        Q3PtrListIterator<Q3TableSelection> it(selections);
        Q3TableSelection *s;
        while ((s = it.current()) != 0) {
            ++it;
            if (s->isActive() &&
                 row >= s->topRow() &&
                 row <= s->bottomRow())
            return true;
        if (row == currentRow())
            return true;
        }
    } else {
        Q3PtrListIterator<Q3TableSelection> it(selections);
        Q3TableSelection *s;
        while ((s = it.current()) != 0) {
            ++it;
            if (s->isActive() &&
                 row >= s->topRow() &&
                 row <= s->bottomRow() &&
                 s->leftCol() == 0 &&
                 s->rightCol() == numCols() - 1)
                return true;
        }
    }
    return false;
}

/*!
    Returns true if column \a col is selected; otherwise returns false.

    If \a full is false (the default), 'column is selected' means that
    at least one cell in the column is selected. If \a full is true,
    then 'column is selected' means every cell in the column is
    selected.

    \sa isRowSelected() isSelected()
*/

bool Q3Table::isColumnSelected(int col, bool full) const
{
    if (!full) {
        Q3PtrListIterator<Q3TableSelection> it(selections);
        Q3TableSelection *s;
        while ((s = it.current()) != 0) {
            ++it;
            if (s->isActive() &&
                 col >= s->leftCol() &&
                 col <= s->rightCol())
            return true;
        if (col == currentColumn())
            return true;
        }
    } else {
        Q3PtrListIterator<Q3TableSelection> it(selections);
        Q3TableSelection *s;
        while ((s = it.current()) != 0) {
            ++it;
            if (s->isActive() &&
                 col >= s->leftCol() &&
                 col <= s->rightCol() &&
                 s->topRow() == 0 &&
                 s->bottomRow() == numRows() - 1)
                return true;
        }
    }
    return false;
}

/*!
    \property Q3Table::numSelections
    \brief The number of selections.

    \sa currentSelection()
*/

int Q3Table::numSelections() const
{
    return selections.count();
}

/*!
    Returns selection number \a num, or an inactive Q3TableSelection if \a
    num is out of range (see Q3TableSelection::isActive()).
*/

Q3TableSelection Q3Table::selection(int num) const
{
    if (num < 0 || num >= (int)selections.count())
        return Q3TableSelection();

    Q3TableSelection *s = ((Q3Table*)this)->selections.at(num);
    return *s;
}

/*!
    Adds a selection described by \a s to the table and returns its
    number or -1 if the selection is invalid.

    Remember to call Q3TableSelection::init() and
    Q3TableSelection::expandTo() to make the selection valid (see also
    Q3TableSelection::isActive(), or use the
    Q3TableSelection(int,int,int,int) constructor).

    \sa numSelections() removeSelection() clearSelection()
*/

int Q3Table::addSelection(const Q3TableSelection &s)
{
    if (!s.isActive())
        return -1;

    const int maxr = numRows()-1;
    const int maxc = numCols()-1;
    currentSel = new Q3TableSelection(QMIN(s.anchorRow(), maxr), QMIN(s.anchorCol(), maxc),
                                    QMIN(s.bottomRow(), maxr), QMIN(s.rightCol(), maxc));

    selections.append(currentSel);

    repaintSelections(0, currentSel, true, true);

    emit selectionChanged();

    return selections.count() - 1;
}

/*!
    If the table has a selection, \a s, this selection is removed from
    the table.

    \sa addSelection() numSelections()
*/

void Q3Table::removeSelection(const Q3TableSelection &s)
{
    selections.setAutoDelete(false);
    for (Q3TableSelection *sel = selections.first(); sel; sel = selections.next()) {
        if (s == *sel) {
            selections.removeRef(sel);
            repaintSelections(sel, 0, true, true);
            if (sel == currentSel)
                currentSel = 0;
            delete sel;
        }
    }
    selections.setAutoDelete(true);
    emit selectionChanged();
}

/*!
    \overload

    Removes selection number \a num from the table.

    \sa numSelections() addSelection() clearSelection()
*/

void Q3Table::removeSelection(int num)
{
    if (num < 0 || num >= (int)selections.count())
        return;

    Q3TableSelection *s = selections.at(num);
    if (s == currentSel)
        currentSel = 0;
    selections.removeRef(s);
    repaintContents(false);
}

/*!
    Returns the number of the current selection or -1 if there is no
    current selection.

    \sa numSelections()
*/

int Q3Table::currentSelection() const
{
    if (!currentSel)
        return -1;
    return ((Q3Table*)this)->selections.findRef(currentSel);
}

/*! Selects the range starting at \a start_row and \a start_col and
  ending at \a end_row and \a end_col.

  \sa Q3TableSelection
*/

void Q3Table::selectCells(int start_row, int start_col, int end_row, int end_col)
{
    const int maxr = numRows()-1;
    const int maxc = numCols()-1;

    start_row = QMIN(maxr, QMAX(0, start_row));
    start_col = QMIN(maxc, QMAX(0, start_col));
    end_row = QMIN(maxr, end_row);
    end_col = QMIN(maxc, end_col);
    Q3TableSelection sel(start_row, start_col, end_row, end_col);
    addSelection(sel);
}

/*! Selects the row \a row.

  \sa Q3TableSelection
*/

void Q3Table::selectRow(int row)
{
    row = QMIN(numRows()-1, row);
    if (row < 0)
        return;
    if (selectionMode() == SingleRow) {
        setCurrentCell(row, currentColumn());
    } else {
        Q3TableSelection sel(row, 0, row, numCols() - 1);
        addSelection(sel);
    }
}

/*! Selects the column \a col.

  \sa Q3TableSelection
*/

void Q3Table::selectColumn(int col)
{
    col = QMIN(numCols()-1, col);
    if (col < 0)
        return;
    Q3TableSelection sel(0, col, numRows() - 1, col);
    addSelection(sel);
}

/*! \reimp
*/
void Q3Table::contentsMousePressEvent(QMouseEvent* e)
{
    contentsMousePressEventEx(e);
}

void Q3Table::contentsMousePressEventEx(QMouseEvent* e)
{
    shouldClearSelection = false;
    if (isEditing()) {
        if (!cellGeometry(editRow, editCol).contains(e->pos())) {
            endEdit(editRow, editCol, true, edMode != Editing);
        } else {
            e->ignore();
            return;
        }
    }

    d->redirectMouseEvent = false;

    int tmpRow = rowAt(e->pos().y());
    int tmpCol = columnAt(e->pos().x());
    pressedRow = tmpRow;
    pressedCol = tmpCol;
    fixRow(tmpRow, e->pos().y());
    fixCol(tmpCol, e->pos().x());
    startDragCol = -1;
    startDragRow = -1;

    if (isSelected(tmpRow, tmpCol)) {
        startDragCol = tmpCol;
        startDragRow = tmpRow;
        dragStartPos = e->pos();
    }

    Q3TableItem *itm = item(pressedRow, pressedCol);
    if (itm && !itm->isEnabled()) {
        emit pressed(tmpRow, tmpCol, e->button(), e->pos());
        return;
    }

    if ((e->state() & ShiftButton) == ShiftButton) {
          int oldRow = curRow;
          int oldCol = curCol;
        setCurrentCell(tmpRow, tmpCol, selMode == SingleRow, true);
        if (selMode != NoSelection && selMode != SingleRow) {
            if (!currentSel) {
                currentSel = new Q3TableSelection();
                selections.append(currentSel);
                if (!isRowSelection(selectionMode()))
                    currentSel->init(oldRow, oldCol);
                else
                    currentSel->init(oldRow, 0);
            }
            Q3TableSelection oldSelection = *currentSel;
            if (!isRowSelection(selectionMode()))
                currentSel->expandTo(tmpRow, tmpCol);
            else
                currentSel->expandTo(tmpRow, numCols() - 1);
            repaintSelections(&oldSelection, currentSel);
            emit selectionChanged();
        }
    } else if ((e->state() & ControlButton) == ControlButton) {
        setCurrentCell(tmpRow, tmpCol, false, true);
        if (selMode != NoSelection) {
            if (selMode == Single || (selMode == SingleRow && !isSelected(tmpRow, tmpCol, false)))
                clearSelection();
            if (!(selMode == SingleRow && isSelected(tmpRow, tmpCol, false))) {
                currentSel = new Q3TableSelection();
                selections.append(currentSel);
                if (!isRowSelection(selectionMode())) {
                    currentSel->init(tmpRow, tmpCol);
                    currentSel->expandTo(tmpRow, tmpCol);
                } else {
                    currentSel->init(tmpRow, 0);
                    currentSel->expandTo(tmpRow, numCols() - 1);
                    repaintSelections(0, currentSel);
                }
                emit selectionChanged();
            }
        }
    } else {
        setCurrentCell(tmpRow, tmpCol, false, true);
        Q3TableItem *itm = item(tmpRow, tmpCol);
        if (itm && itm->editType() == Q3TableItem::WhenCurrent) {
            QWidget *w = cellWidget(tmpRow, tmpCol);
            if (qobject_cast<Q3ComboBox*>(w) || qobject_cast<QAbstractButton*>(w)) {
                QMouseEvent ev(e->type(), w->mapFromGlobal(e->globalPos()),
                                e->globalPos(), e->button(), e->state());
                QApplication::sendPostedEvents(w, 0);
                QApplication::sendEvent(w, &ev);
                d->redirectMouseEvent = true;
            }
        }
        if (isSelected(tmpRow, tmpCol, false)) {
            shouldClearSelection = true;
        } else {
            bool b = signalsBlocked();
            if (selMode != NoSelection)
                blockSignals(true);
            clearSelection();
            blockSignals(b);
            if (selMode != NoSelection) {
                currentSel = new Q3TableSelection();
                selections.append(currentSel);
                if (!isRowSelection(selectionMode())) {
                    currentSel->init(tmpRow, tmpCol);
                    currentSel->expandTo(tmpRow, tmpCol);
                } else {
                    currentSel->init(tmpRow, 0);
                    currentSel->expandTo(tmpRow, numCols() - 1);
                    repaintSelections(0, currentSel);
                }
                emit selectionChanged();
            }
        }
    }

    emit pressed(tmpRow, tmpCol, e->button(), e->pos());
}

/*! \reimp
*/

void Q3Table::contentsMouseDoubleClickEvent(QMouseEvent *e)
{
    if (e->button() != LeftButton)
        return;
    if (!isRowSelection(selectionMode()))
        clearSelection();
    int tmpRow = rowAt(e->pos().y());
    int tmpCol = columnAt(e->pos().x());
    Q3TableItem *itm = item(tmpRow, tmpCol);
    if (itm && !itm->isEnabled())
        return;
    if (tmpRow != -1 && tmpCol != -1) {
        if (beginEdit(tmpRow, tmpCol, false))
            setEditMode(Editing, tmpRow, tmpCol);
    }

    emit doubleClicked(tmpRow, tmpCol, e->button(), e->pos());
}

/*!
    Sets the current edit mode to \a mode, the current edit row to \a
    row and the current edit column to \a col.

    \sa EditMode
*/

void Q3Table::setEditMode(EditMode mode, int row, int col)
{
    edMode = mode;
    editRow = row;
    editCol = col;
}


/*! \reimp
*/

void Q3Table::contentsMouseMoveEvent(QMouseEvent *e)
{
    if ((e->state() & MouseButtonMask) == NoButton)
        return;
    int tmpRow = rowAt(e->pos().y());
    int tmpCol = columnAt(e->pos().x());
    fixRow(tmpRow, e->pos().y());
    fixCol(tmpCol, e->pos().x());

#ifndef QT_NO_DRAGANDDROP
    if (dragEnabled() && startDragRow != -1 && startDragCol != -1) {
        if (QPoint(dragStartPos - e->pos()).manhattanLength() > QApplication::startDragDistance())
            startDrag();
        return;
    }
#endif
    if (selectionMode() == MultiRow && (e->state() & ControlButton) == ControlButton)
        shouldClearSelection = false;

    if (shouldClearSelection) {
        clearSelection();
        if (selMode != NoSelection) {
            currentSel = new Q3TableSelection();
            selections.append(currentSel);
            if (!isRowSelection(selectionMode()))
                currentSel->init(tmpRow, tmpCol);
            else
                currentSel->init(tmpRow, 0);
            emit selectionChanged();
        }
        shouldClearSelection = false;
    }

    QPoint pos = mapFromGlobal(e->globalPos());
    pos -= QPoint(leftHeader->width(), topHeader->height());
    autoScrollTimer->stop();
    doAutoScroll();
    if (pos.x() < 0 || pos.x() > visibleWidth() || pos.y() < 0 || pos.y() > visibleHeight())
        autoScrollTimer->start(100, true);
}

/*! \internal
 */

void Q3Table::doValueChanged()
{
    emit valueChanged(editRow, editCol);
}

/*! \internal
*/

void Q3Table::doAutoScroll()
{
    QPoint pos = QCursor::pos();
    pos = mapFromGlobal(pos);
    pos -= QPoint(leftHeader->width(), topHeader->height());

    int tmpRow = curRow;
    int tmpCol = curCol;
    if (pos.y() < 0)
        tmpRow--;
    else if (pos.y() > visibleHeight())
        tmpRow++;
    if (pos.x() < 0)
        tmpCol--;
    else if (pos.x() > visibleWidth())
        tmpCol++;

    pos += QPoint(contentsX(), contentsY());
    if (tmpRow == curRow)
        tmpRow = rowAt(pos.y());
    if (tmpCol == curCol)
        tmpCol = columnAt(pos.x());
    pos -= QPoint(contentsX(), contentsY());

    fixRow(tmpRow, pos.y());
    fixCol(tmpCol, pos.x());

    if (tmpRow < 0 || tmpRow > numRows() - 1)
        tmpRow = currentRow();
    if (tmpCol < 0 || tmpCol > numCols() - 1)
        tmpCol = currentColumn();

    ensureCellVisible(tmpRow, tmpCol);

    if (currentSel && selMode != NoSelection) {
        Q3TableSelection oldSelection = *currentSel;
        bool useOld = true;
        if (selMode != SingleRow) {
            if (!isRowSelection(selectionMode())) {
                currentSel->expandTo(tmpRow, tmpCol);
            } else {
                currentSel->expandTo(tmpRow, numCols() - 1);
            }
        } else {
            bool currentInSelection = tmpRow == curRow && isSelected(tmpRow, tmpCol);
            if (!currentInSelection) {
                useOld = false;
                clearSelection();
                currentSel = new Q3TableSelection();
                selections.append(currentSel);
                currentSel->init(tmpRow, 0);
                currentSel->expandTo(tmpRow, numCols() - 1);
                repaintSelections(0, currentSel);
            } else {
                currentSel->expandTo(tmpRow, numCols() - 1);
            }
        }
        setCurrentCell(tmpRow, tmpCol, false, true);
        repaintSelections(useOld ? &oldSelection : 0, currentSel);
        if (currentSel && oldSelection != *currentSel)
            emit selectionChanged();
    } else {
        setCurrentCell(tmpRow, tmpCol, false, true);
    }

    if (pos.x() < 0 || pos.x() > visibleWidth() || pos.y() < 0 || pos.y() > visibleHeight())
        autoScrollTimer->start(100, true);
}

/*! \reimp
*/

void Q3Table::contentsMouseReleaseEvent(QMouseEvent *e)
{
    if (pressedRow == curRow && pressedCol == curCol)
        emit clicked(curRow, curCol, e->button(), e->pos());

    if (e->button() != LeftButton)
        return;
    if (shouldClearSelection) {
        int tmpRow = rowAt(e->pos().y());
        int tmpCol = columnAt(e->pos().x());
        fixRow(tmpRow, e->pos().y());
        fixCol(tmpCol, e->pos().x());
        clearSelection();
        if (selMode != NoSelection) {
            currentSel = new Q3TableSelection();
            selections.append(currentSel);
            if (!isRowSelection(selectionMode())) {
                currentSel->init(tmpRow, tmpCol);
            } else {
                currentSel->init(tmpRow, 0);
                currentSel->expandTo(tmpRow, numCols() - 1);
                repaintSelections(0, currentSel);
            }
            emit selectionChanged();
        }
        shouldClearSelection = false;
    }
    autoScrollTimer->stop();

    if (d->redirectMouseEvent && pressedRow == curRow && pressedCol == curCol &&
         item(pressedRow, pressedCol) && item(pressedRow, pressedCol)->editType() ==
         Q3TableItem::WhenCurrent) {
        QWidget *w = cellWidget(pressedRow, pressedCol);
        if (w) {
            QMouseEvent ev(e->type(), w->mapFromGlobal(e->globalPos()),
                            e->globalPos(), e->button(), e->state());
            QApplication::sendPostedEvents(w, 0);
            bool old = w->testAttribute(Qt::WA_NoMousePropagation);
            w->setAttribute(Qt::WA_NoMousePropagation, true);
            QApplication::sendEvent(w, &ev);
            w->setAttribute(Qt::WA_NoMousePropagation, old);
        }
    }
}

/*!
  \reimp
*/

void Q3Table::contentsContextMenuEvent(QContextMenuEvent *e)
{
    if (!receivers(SIGNAL(contextMenuRequested(int,int,QPoint)))) {
        e->ignore();
        return;
    }
    if (e->reason() == QContextMenuEvent::Keyboard) {
        QRect r = cellGeometry(curRow, curCol);
        emit contextMenuRequested(curRow, curCol, viewport()->mapToGlobal(contentsToViewport(r.center())));
    } else {
        int tmpRow = rowAt(e->pos().y());
        int tmpCol = columnAt(e->pos().x());
        emit contextMenuRequested(tmpRow, tmpCol, e->globalPos());
    }
}


/*! \reimp
*/

bool Q3Table::eventFilter(QObject *o, QEvent *e)
{
    switch (e->type()) {
    case QEvent::KeyPress: {
        Q3TableItem *itm = item(curRow, curCol);
        QWidget *editorWidget = cellWidget(editRow, editCol);

        if (isEditing() && editorWidget && o == editorWidget) {
            itm = item(editRow, editCol);
            QKeyEvent *ke = (QKeyEvent*)e;
            if (ke->key() == Key_Escape) {
                if (!itm || itm->editType() == Q3TableItem::OnTyping)
                    endEdit(editRow, editCol, false, edMode != Editing);
                return true;
            }

            if ((ke->state() == NoButton || ke->state() == Keypad)
                && (ke->key() == Key_Return || ke->key() == Key_Enter)) {
                if (!itm || itm->editType() == Q3TableItem::OnTyping)
                    endEdit(editRow, editCol, true, edMode != Editing);
                activateNextCell();
                return true;
            }

            if (ke->key() == Key_Tab || ke->key() == Key_BackTab) {
                if (ke->state() & Qt::ControlButton)
                    return false;
                if (!itm || itm->editType() == Q3TableItem::OnTyping)
                    endEdit(editRow, editCol, true, edMode != Editing);
                if ((ke->key() == Key_Tab) && !(ke->state() & ShiftButton)) {
                    if (currentColumn() >= numCols() - 1)
                        return true;
                    int cc  = QMIN(numCols() - 1, currentColumn() + 1);
                    while (cc < numCols()) {
                        Q3TableItem *i = item(currentRow(), cc);
                        if (!d->hiddenCols.find(cc) && !isColumnReadOnly(cc) && (!i || i->isEnabled()))
                            break;
                        ++cc;
                    }
                    setCurrentCell(currentRow(), cc);
                } else { // Key_BackTab
                    if (currentColumn() == 0)
                        return true;
                    int cc  = QMAX(0, currentColumn() - 1);
                    while (cc >= 0) {
                        Q3TableItem *i = item(currentRow(), cc);
                        if (!d->hiddenCols.find(cc) && !isColumnReadOnly(cc) && (!i || i->isEnabled()))
                            break;
                        --cc;
                    }
                    setCurrentCell(currentRow(), cc);
                }
                itm = item(curRow, curCol);
                if (beginEdit(curRow, curCol, false))
                    setEditMode(Editing, curRow, curCol);
                return true;
            }

            if ((edMode == Replacing ||
                   (itm && itm->editType() == Q3TableItem::WhenCurrent)) &&
                 (ke->key() == Key_Up || ke->key() == Key_Prior ||
                   ke->key() == Key_Home || ke->key() == Key_Down ||
                   ke->key() == Key_Next || ke->key() == Key_End ||
                   ke->key() == Key_Left || ke->key() == Key_Right)) {
                if (!itm || itm->editType() == Q3TableItem::OnTyping) {
                    endEdit(editRow, editCol, true, edMode != Editing);
                }
                keyPressEvent(ke);
                return true;
            }
        } else {
            QObjectList l = viewport()->queryList("QWidget");
            if (l.contains(o)) {
                QKeyEvent *ke = (QKeyEvent*)e;
                if ((ke->state() & ControlButton) == ControlButton ||
                     (ke->key() != Key_Left && ke->key() != Key_Right &&
                       ke->key() != Key_Up && ke->key() != Key_Down &&
                       ke->key() != Key_Prior && ke->key() != Key_Next &&
                       ke->key() != Key_Home && ke->key() != Key_End))
                    return false;
                keyPressEvent((QKeyEvent*)e);
                return true;
            }
        }

        } break;
    case QEvent::FocusOut: {
        QWidget *editorWidget = cellWidget(editRow, editCol);
        if (isEditing() && editorWidget && o == editorWidget && ((QFocusEvent*)e)->reason() != Qt::PopupFocusReason) {
            // if the editor is the parent of the new focus widget, do nothing
            QWidget *w = QApplication::focusWidget();
            while (w) {
                w = w->parentWidget();
                if (w == editorWidget)
                    break;
            }
            if (w)
                break;
            // otherwise, end editing
            Q3TableItem *itm = item(editRow, editCol);
            if (!itm || itm->editType() == Q3TableItem::OnTyping) {
                endEdit(editRow, editCol, true, edMode != Editing);
                return true;
            }
        }
        break;
    }
#ifndef QT_NO_WHEELEVENT
    case QEvent::Wheel:
        if (o == this || o == viewport()) {
            QWheelEvent* we = (QWheelEvent*)e;
            scrollBy(0, -we->delta());
            we->accept();
            return true;
        }
#endif
    default:
        break;
    }

    return Q3ScrollView::eventFilter(o, e);
}

void Q3Table::fixCell(int &row, int &col, int key)
{
    if (rowHeight(row) > 0 && columnWidth(col) > 0)
        return;
    if (rowHeight(row) <= 0) {
        if (key == Key_Down ||
             key == Key_Next ||
             key == Key_End) {
            while (row < numRows() && rowHeight(row) <= 0)
                row++;
            if (rowHeight(row) <= 0)
                row = curRow;
        } else if (key == Key_Up ||
                    key == Key_Prior ||
                    key == Key_Home)
            while (row >= 0 && rowHeight(row) <= 0)
                row--;
            if (rowHeight(row) <= 0)
                row = curRow;
    } else if (columnWidth(col) <= 0) {
        if (key == Key_Left) {
            while (col >= 0 && columnWidth(col) <= 0)
                col--;
            if (columnWidth(col) <= 0)
                col = curCol;
        } else if (key == Key_Right) {
            while (col < numCols() && columnWidth(col) <= 0)
                col++;
            if (columnWidth(col) <= 0)
                col = curCol;
        }
    }
}

/*! \reimp
*/

void Q3Table::keyPressEvent(QKeyEvent* e)
{
    if (isEditing() && item(editRow, editCol) &&
         item(editRow, editCol)->editType() == Q3TableItem::OnTyping)
        return;

    int tmpRow = curRow;
    int tmpCol = curCol;
    int oldRow = tmpRow;
    int oldCol = tmpCol;

    bool navigationKey = false;
    int r;
    switch (e->key()) {
    case Key_Left:
        tmpCol = QMAX(0, tmpCol - 1);
        navigationKey = true;
        break;
    case Key_Right:
        tmpCol = QMIN(numCols() - 1, tmpCol + 1);
        navigationKey = true;
        break;
    case Key_Up:
        tmpRow = QMAX(0, tmpRow - 1);
        navigationKey = true;
        break;
    case Key_Down:
        tmpRow = QMIN(numRows() - 1, tmpRow + 1);
        navigationKey = true;
        break;
    case Key_Prior:
        r = QMAX(0, rowAt(rowPos(tmpRow) - visibleHeight()));
        if (r < tmpRow || tmpRow < 0)
            tmpRow = r;
        navigationKey = true;
        break;
    case Key_Next:
        r = QMIN(numRows() - 1, rowAt(rowPos(tmpRow) + visibleHeight()));
        if (r > tmpRow)
            tmpRow = r;
        else
            tmpRow = numRows() - 1;
        navigationKey = true;
        break;
    case Key_Home:
        tmpRow = 0;
        navigationKey = true;
        break;
    case Key_End:
        tmpRow = numRows() - 1;
        navigationKey = true;
        break;
    case Key_F2:
        if (beginEdit(tmpRow, tmpCol, false))
            setEditMode(Editing, tmpRow, tmpCol);
        break;
    case Key_Enter: case Key_Return:
        activateNextCell();
        return;
    case Key_Tab: case Key_BackTab:
        if ((e->key() == Key_Tab) && !(e->state() & ShiftButton)) {
            if (currentColumn() >= numCols() - 1)
                return;
            int cc  = QMIN(numCols() - 1, currentColumn() + 1);
            while (cc < numCols()) {
                Q3TableItem *i = item(currentRow(), cc);
                if (!d->hiddenCols.find(cc) && !isColumnReadOnly(cc) && (!i || i->isEnabled()))
                    break;
                ++cc;
            }
            setCurrentCell(currentRow(), cc);
        } else { // Key_BackTab
            if (currentColumn() == 0)
                return;
            int cc  = QMAX(0, currentColumn() - 1);
            while (cc >= 0) {
                Q3TableItem *i = item(currentRow(), cc);
                if (!d->hiddenCols.find(cc) && !isColumnReadOnly(cc) && (!i || i->isEnabled()))
                    break;
                --cc;
            }
            setCurrentCell(currentRow(), cc);
        }
        return;
    case Key_Escape:
        e->ignore();
        return;
    default: // ... or start in-place editing
        if (e->text()[ 0 ].isPrint()) {
            Q3TableItem *itm = item(tmpRow, tmpCol);
            if (!itm || itm->editType() == Q3TableItem::OnTyping) {
                QWidget *w = beginEdit(tmpRow, tmpCol,
                                        itm ? itm->isReplaceable() : true);
                if (w) {
                    setEditMode((!itm || (itm && itm->isReplaceable())
                                   ? Replacing : Editing), tmpRow, tmpCol);
                    QApplication::sendEvent(w, e);
                    return;
                }
            }
        }
        e->ignore();
        return;
    }

    if (navigationKey) {
        fixCell(tmpRow, tmpCol, e->key());
        if ((e->state() & ShiftButton) == ShiftButton &&
             selMode != NoSelection && selMode != SingleRow) {
            bool justCreated = false;
            setCurrentCell(tmpRow, tmpCol, false, true);
            if (!currentSel) {
                justCreated = true;
                currentSel = new Q3TableSelection();
                selections.append(currentSel);
                if (!isRowSelection(selectionMode()))
                    currentSel->init(oldRow, oldCol);
                else
                    currentSel->init(oldRow < 0 ? 0 : oldRow, 0);
            }
            Q3TableSelection oldSelection = *currentSel;
            if (!isRowSelection(selectionMode()))
                currentSel->expandTo(tmpRow, tmpCol);
            else
                currentSel->expandTo(tmpRow, numCols() - 1);
            repaintSelections(justCreated ? 0 : &oldSelection, currentSel);
            emit selectionChanged();
        } else {
            setCurrentCell(tmpRow, tmpCol, false, true);
            if (!isRowSelection(selectionMode())) {
                clearSelection();
            } else {
                bool currentInSelection = tmpRow == oldRow && isSelected(tmpRow, tmpCol, false);
                if (!currentInSelection) {
                    bool hasOldSel = false;
                    Q3TableSelection oldSelection;
                    if (selectionMode() == MultiRow) {
                        bool b = signalsBlocked();
                        blockSignals(true);
                        clearSelection();
                        blockSignals(b);
                    } else {
                        if (currentSel) {
                            oldSelection = *currentSel;
                            hasOldSel = true;
                            selections.removeRef(currentSel);
                            leftHeader->setSectionState(oldSelection.topRow(), Q3TableHeader::Normal);
                        }
                    }
                    currentSel = new Q3TableSelection();
                    selections.append(currentSel);
                    currentSel->init(tmpRow, 0);
                    currentSel->expandTo(tmpRow, numCols() - 1);
                    repaintSelections(hasOldSel ? &oldSelection : 0, currentSel, !hasOldSel);
                    emit selectionChanged();
                }
            }
        }
    } else {
        setCurrentCell(tmpRow, tmpCol, false, true);
    }
}

/*! \reimp
*/

void Q3Table::focusInEvent(QFocusEvent*)
{
    d->inMenuMode = false;
    QWidget *editorWidget = cellWidget(editRow, editCol);
    updateCell(curRow, curCol);
    if (style()->styleHint(QStyle::SH_ItemView_ChangeHighlightOnFocus, 0, this))
        repaintSelections();
    if (isEditing() && editorWidget)
        editorWidget->setFocus();

}


/*! \reimp
*/

void Q3Table::focusOutEvent(QFocusEvent *e)
{
    updateCell(curRow, curCol);
    if (style()->styleHint(QStyle::SH_ItemView_ChangeHighlightOnFocus, 0, this)) {
        d->inMenuMode =
            e->reason() == Qt::PopupFocusReason ||
            (qApp->focusWidget() && qApp->focusWidget()->inherits("QMenuBar"));
        if (!d->inMenuMode)
            repaintSelections();
    }
}

/*! \reimp
*/

QSize Q3Table::sizeHint() const
{
    if (cachedSizeHint().isValid())
        return cachedSizeHint();

    constPolish();

    QSize s = tableSize();
    QSize sh;
    if (s.width() < 500 && s.height() < 500) {
        sh = QSize(tableSize().width() + VERTICALMARGIN + 5,
                    tableSize().height() + topMargin() + 5);
    } else {
            sh = Q3ScrollView::sizeHint();
            if (!topHeader->isHidden())
                sh.setHeight(sh.height() + topHeader->height());
            if (!leftHeader->isHidden())
                sh.setWidth(sh.width() + leftHeader->width());
    }
    setCachedSizeHint(sh);
    return sh;
}

/*! \reimp
*/

void Q3Table::viewportResizeEvent(QResizeEvent *e)
{
    Q3ScrollView::viewportResizeEvent(e);
    updateGeometries();
}

/*! \reimp
*/

void Q3Table::showEvent(QShowEvent *e)
{
    Q3ScrollView::showEvent(e);
    QRect r(cellGeometry(numRows() - 1, numCols() - 1));
    resizeContents(r.right() + 1, r.bottom() + 1);
    updateGeometries();
}

/*! \reimp
*/

void Q3Table::paintEvent(QPaintEvent *e)
{
    QRect topLeftCorner = QStyle::visualRect(layoutDirection(), rect(), QRect(frameWidth(), frameWidth(), VERTICALMARGIN, topMargin()));
    erase(topLeftCorner); // erase instead of widget on top
    Q3ScrollView::paintEvent(e);

#ifdef Q_OS_WINCE
    QPainter p(this);
    p.drawLine(topLeftCorner.bottomLeft(), topLeftCorner.bottomRight());
    p.drawLine(topLeftCorner.bottomRight(), topLeftCorner.topRight());
#endif
}

static bool inUpdateCell = false;

/*!
    Repaints the cell at \a row, \a col.
*/

void Q3Table::updateCell(int row, int col)
{
    if (inUpdateCell || row < 0 || col < 0)
        return;
    inUpdateCell = true;
    QRect cg = cellGeometry(row, col);
    QRect r(contentsToViewport(QPoint(cg.x() - 2, cg.y() - 2)),
             QSize(cg.width() + 4, cg.height() + 4));
    viewport()->update(r);
    inUpdateCell = false;
}

void Q3Table::repaintCell(int row, int col)
{
    if (row == -1 || col == -1)
        return;
    QRect cg = cellGeometry(row, col);
    QRect r(QPoint(cg.x() - 2, cg.y() - 2),
             QSize(cg.width() + 4, cg.height() + 4));
    repaintContents(r, false);
}

void Q3Table::contentsToViewport2(int x, int y, int& vx, int& vy)
{
    const QPoint v = contentsToViewport2(QPoint(x, y));
    vx = v.x();
    vy = v.y();
}

QPoint Q3Table::contentsToViewport2(const QPoint &p)
{
    return QPoint(p.x() - contentsX(),
                   p.y() - contentsY());
}

QPoint Q3Table::viewportToContents2(const QPoint& vp)
{
    return QPoint(vp.x() + contentsX(),
                   vp.y() + contentsY());
}

void Q3Table::viewportToContents2(int vx, int vy, int& x, int& y)
{
    const QPoint c = viewportToContents2(QPoint(vx, vy));
    x = c.x();
    y = c.y();
}

/*!
    This function should be called whenever the column width of \a col
    has been changed. It updates the geometry of any affected columns
    and repaints the table to reflect the changes it has made.
*/

void Q3Table::columnWidthChanged(int col)
{
    int p = columnPos(col);
    if (d->hasColSpan)
        p = contentsX();
    updateContents(p, contentsY(), contentsWidth(), visibleHeight());
    QSize s(tableSize());
    int w = contentsWidth();
    resizeContents(s.width(), s.height());
    if (contentsWidth() < w)
        repaintContents(s.width(), contentsY(),
                         w - s.width() + 1, visibleHeight(), true);
    else
        repaintContents(w, contentsY(),
                         s.width() - w + 1, visibleHeight(), false);

    // update widgets that are affected by this change
    if (widgets.size()) {
        int last = isHidden() ? numCols() - 1 : d->lastVisCol;
        for (int c = col; c <= last; ++c)
            updateColWidgets(c);
    }
    delayedUpdateGeometries();
}

/*!
    This function should be called whenever the row height of \a row
    has been changed. It updates the geometry of any affected rows and
    repaints the table to reflect the changes it has made.
*/

void Q3Table::rowHeightChanged(int row)
{
    int p = rowPos(row);
    if (d->hasRowSpan)
        p = contentsY();
    updateContents(contentsX(), p, visibleWidth(), contentsHeight());
    QSize s(tableSize());
    int h = contentsHeight();
    resizeContents(s.width(), s.height());
    if (contentsHeight() < h) {
        repaintContents(contentsX(), contentsHeight(),
                         visibleWidth(), h - s.height() + 1, true);
    } else {
        repaintContents(contentsX(), h,
                         visibleWidth(), s.height() - h + 1, false);
    }

    // update widgets that are affected by this change
    if (widgets.size()) {
        d->lastVisRow = rowAt(contentsY() + visibleHeight() + (s.height() - h + 1));
        int last = isHidden() ? numRows() - 1 : d->lastVisRow;
        for (int r = row; r <= last; ++r)
            updateRowWidgets(r);
    }
    delayedUpdateGeometries();
}

/*! \internal */

void Q3Table::updateRowWidgets(int row)
{
    for (int i = 0; i < numCols(); ++i) {
        QWidget *w = cellWidget(row, i);
        if (!w)
            continue;
        moveChild(w, columnPos(i), rowPos(row));
        w->resize(columnWidth(i) - 1, rowHeight(row) - 1);
    }
}

/*! \internal */

void Q3Table::updateColWidgets(int col)
{
    for (int i = 0; i < numRows(); ++i) {
        QWidget *w = cellWidget(i, col);
        if (!w)
            continue;
        moveChild(w, columnPos(col), rowPos(i));
        w->resize(columnWidth(col) - 1, rowHeight(i) - 1);
    }
}

/*!
    This function is called when column order is to be changed, i.e.
    when the user moved the column header \a section from \a fromIndex
    to \a toIndex.

    If you want to change the column order programmatically, call
    swapRows() or swapColumns();

    \sa Q3Header::indexChange() rowIndexChanged()
*/

void Q3Table::columnIndexChanged(int, int fromIndex, int toIndex)
{
    if (doSort && lastSortCol == fromIndex && topHeader)
        topHeader->setSortIndicator(toIndex, topHeader->sortIndicatorOrder());
    repaintContents(contentsX(), contentsY(),
                     visibleWidth(), visibleHeight(), false);
}

/*!
    This function is called when the order of the rows is to be
    changed, i.e. the user moved the row header section \a section
    from \a fromIndex to \a toIndex.

    If you want to change the order programmatically, call swapRows()
    or swapColumns();

    \sa Q3Header::indexChange() columnIndexChanged()
*/

void Q3Table::rowIndexChanged(int, int, int)
{
    repaintContents(contentsX(), contentsY(),
                     visibleWidth(), visibleHeight(), false);
}

/*!
    This function is called when the column \a col has been clicked.
    The default implementation sorts this column if sorting() is true.
*/

void Q3Table::columnClicked(int col)
{
    if (!sorting())
        return;

    if (col == lastSortCol) {
        asc = !asc;
    } else {
        lastSortCol = col;
        asc = true;
    }
    sortColumn(lastSortCol, asc);
}

/*!
    \property Q3Table::sorting
    \brief whether a click on the header of a column sorts that column

    \sa sortColumn()
*/

void Q3Table::setSorting(bool b)
{
    doSort = b;
    if (topHeader)
         topHeader->setSortIndicator(b ? lastSortCol : -1);
}

bool Q3Table::sorting() const
{
    return doSort;
}

static bool inUpdateGeometries = false;

void Q3Table::delayedUpdateGeometries()
{
    d->geomTimer->start(0, true);
}

void Q3Table::updateGeometriesSlot()
{
    updateGeometries();
}

/*!
    This function updates the geometries of the left and top header.
    You do not normally need to call this function.
*/

void Q3Table::updateGeometries()
{
    if (inUpdateGeometries)
        return;
    inUpdateGeometries = true;
    QSize ts = tableSize();
    if (topHeader->offset() &&
         ts.width() < topHeader->offset() + topHeader->width())
        horizontalScrollBar()->setValue(ts.width() - topHeader->width());
    if (leftHeader->offset() &&
         ts.height() < leftHeader->offset() + leftHeader->height())
        verticalScrollBar()->setValue(ts.height() - leftHeader->height());

    leftHeader->setGeometry(QStyle::visualRect(layoutDirection(), rect(), QRect(frameWidth(), topMargin() + frameWidth(),
                             VERTICALMARGIN, visibleHeight())));
    topHeader->setGeometry(QStyle::visualRect(layoutDirection(), rect(), QRect(VERTICALMARGIN + frameWidth(), frameWidth(),
                                                      visibleWidth(), topMargin())));
    horizontalScrollBar()->raise();
    verticalScrollBar()->raise();
    topHeader->updateStretches();
    leftHeader->updateStretches();
    inUpdateGeometries = false;
}

/*!
    Returns the width of column \a col.

    \sa setColumnWidth() rowHeight()
*/

int Q3Table::columnWidth(int col) const
{
    return topHeader->sectionSize(col);
}

/*!
    Returns the height of row \a row.

    \sa setRowHeight() columnWidth()
*/

int Q3Table::rowHeight(int row) const
{
    return leftHeader->sectionSize(row);
}

/*!
    Returns the x-coordinate of the column \a col in content
    coordinates.

    \sa columnAt() rowPos()
*/

int Q3Table::columnPos(int col) const
{
    return topHeader->sectionPos(col);
}

/*!
    Returns the y-coordinate of the row \a row in content coordinates.

    \sa rowAt() columnPos()
*/

int Q3Table::rowPos(int row) const
{
    return leftHeader->sectionPos(row);
}

/*!
    Returns the number of the column at position \a x. \a x must be
    given in content coordinates.

    \sa columnPos() rowAt()
*/

int Q3Table::columnAt(int x) const
{
    return topHeader->sectionAt(x);
}

/*!
    Returns the number of the row at position \a y. \a y must be given
    in content coordinates.

    \sa rowPos() columnAt()
*/

int Q3Table::rowAt(int y) const
{
    return leftHeader->sectionAt(y);
}

/*!
    Returns the bounding rectangle of the cell at \a row, \a col in
    content coordinates.
*/

QRect Q3Table::cellGeometry(int row, int col) const
{
    Q3TableItem *itm = item(row, col);

    if (!itm || (itm->rowSpan() == 1 && itm->colSpan() == 1))
        return QRect(columnPos(col), rowPos(row),
                      columnWidth(col), rowHeight(row));

    while (row != itm->row())
        row--;
    while (col != itm->col())
        col--;

    QRect rect(columnPos(col), rowPos(row),
                columnWidth(col), rowHeight(row));

    for (int r = 1; r < itm->rowSpan(); ++r)
        rect.setHeight(rect.height() + rowHeight(r + row));

    for (int c = 1; c < itm->colSpan(); ++c)
        rect.setWidth(rect.width() + columnWidth(c + col));

    return rect;
}

/*!
    Returns the size of the table.

    This is the same as the coordinates of the bottom-right edge of
    the last table cell.
*/

QSize Q3Table::tableSize() const
{
    return QSize(columnPos(numCols() - 1) + columnWidth(numCols() - 1),
                  rowPos(numRows() - 1) + rowHeight(numRows() - 1));
}

/*!
    \property Q3Table::numRows
    \brief The number of rows in the table

    \sa numCols
*/

int Q3Table::numRows() const
{
    return leftHeader->count();
}

/*!
    \property Q3Table::numCols
    \brief The number of columns in the table

    \sa numRows
*/

int Q3Table::numCols() const
{
    return topHeader->count();
}

void Q3Table::saveContents(Q3PtrVector<Q3TableItem> &tmp,
                           Q3PtrVector<Q3Table::TableWidget> &tmp2)
{
    int nCols = numCols();
    if (editRow != -1 && editCol != -1)
        endEdit(editRow, editCol, false, edMode != Editing);
    tmp.resize(contents.size());
    tmp2.resize(widgets.size());
    int i;
    for (i = 0; i < (int)tmp.size(); ++i) {
        Q3TableItem *item = contents[ i ];
        if (item && (item->row() * nCols) + item->col() == i)
            tmp.insert(i, item);
        else
            tmp.insert(i, 0);
    }
    for (i = 0; i < (int)tmp2.size(); ++i) {
        QWidget *w = widgets[ i ];
        if (w)
            tmp2.insert(i, new TableWidget(w, i / nCols, i % nCols));
        else
            tmp2.insert(i, 0);
    }
}

void Q3Table::updateHeaderAndResizeContents(Q3TableHeader *header,
                                            int num, int rowCol,
                                            int width, bool &updateBefore)
{
    updateBefore = rowCol < num;
    if (rowCol > num) {
        header->Q3Header::resizeArrays(rowCol);
        header->Q3TableHeader::resizeArrays(rowCol);
        int old = num;
        clearSelection(false);
        int i = 0;
        for (i = old; i < rowCol; ++i)
            header->addLabel(QString(), width);
    } else {
        clearSelection(false);
        if (header == leftHeader) {
            while (numRows() > rowCol)
                header->removeLabel(numRows() - 1);
        } else {
            while (numCols() > rowCol)
                header->removeLabel(numCols() - 1);
        }
    }

    contents.setAutoDelete(false);
    contents.clear();
    contents.setAutoDelete(true);
    widgets.setAutoDelete(false);
    widgets.clear();
    widgets.setAutoDelete(true);
    resizeData(numRows() * numCols());

    // keep numStretches in sync
    int n = 0;
    for (uint i = 0; i < header->stretchable.size(); i++)
        n += (header->stretchable.at(i) & 1); // avoid cmp
     header->numStretches = n;
}

void Q3Table::restoreContents(Q3PtrVector<Q3TableItem> &tmp,
                              Q3PtrVector<Q3Table::TableWidget> &tmp2)
{
    int i;
    int nCols = numCols();
    for (i = 0; i < (int)tmp.size(); ++i) {
        Q3TableItem *it = tmp[ i ];
        if (it) {
            int idx = (it->row() * nCols) + it->col();
            if ((uint)idx < contents.size() &&
                 it->row() == idx /  nCols && it->col() == idx % nCols) {
                contents.insert(idx, it);
                if (it->rowSpan() > 1 || it->colSpan() > 1) {
                    int ridx, iidx;
                    for (int irow = 0; irow < it->rowSpan(); irow++) {
                        ridx = idx + irow * nCols;
                        for (int icol = 0; icol < it->colSpan(); icol++) {
                            iidx = ridx + icol;
                            if (idx != iidx && (uint)iidx < contents.size())
                                contents.insert(iidx, it);
                        }
                    }

                }
            } else {
                delete it;
            }
        }
    }
    for (i = 0; i < (int)tmp2.size(); ++i) {
        TableWidget *w = tmp2[ i ];
        if (w) {
            int idx = (w->row * nCols) + w->col;
            if ((uint)idx < widgets.size() &&
                 w->row == idx / nCols && w->col == idx % nCols)
                widgets.insert(idx, w->wid);
            else
                delete w->wid;
            delete w;
        }
    }
}

void Q3Table::finishContentsResze(bool updateBefore)
{
    QRect r(cellGeometry(numRows() - 1, numCols() - 1));
    resizeContents(r.right() + 1, r.bottom() + 1);
    updateGeometries();
    if (updateBefore)
        repaintContents(contentsX(), contentsY(),
                         visibleWidth(), visibleHeight(), true);
    else
        repaintContents(contentsX(), contentsY(),
                         visibleWidth(), visibleHeight(), false);

    if (isRowSelection(selectionMode())) {
        int r = curRow;
        curRow = -1;
        setCurrentCell(r, curCol);
    }
}

void Q3Table::setNumRows(int r)
{
    if (r < 0)
        return;

    if (r < numRows()) {
        // Removed rows are no longer hidden, and should thus be removed from "hiddenRows"
        for (int rr = numRows()-1; rr >= r; --rr) {
            if (d->hiddenRows.find(rr))
                d->hiddenRows.remove(rr);
        }
    }

    fontChange(font()); // invalidate the sizeHintCache

    Q3PtrVector<Q3TableItem> tmp;
    Q3PtrVector<TableWidget> tmp2;
    saveContents(tmp, tmp2);

    bool updatesEnabled = leftHeader->updatesEnabled();
    if (updatesEnabled)
        leftHeader->setUpdatesEnabled(false);

    bool updateBefore;
    updateHeaderAndResizeContents(leftHeader, numRows(), r, 20, updateBefore);

    int w = fontMetrics().width(QString::number(r) + QLatin1Char('W'));
    if (VERTICALMARGIN > 0 && w > VERTICALMARGIN)
        setLeftMargin(w);

    restoreContents(tmp, tmp2);

    leftHeader->calculatePositions();
    finishContentsResze(updateBefore);
    if (updatesEnabled) {
        leftHeader->setUpdatesEnabled(true);
        leftHeader->update();
    }
    leftHeader->updateCache();
    if (curRow >= numRows()) {
        curRow = numRows() - 1;
        if (curRow < 0)
            curCol = -1;
        else
            repaintCell(curRow, curCol);
    }

    if (curRow > numRows())
        curRow = numRows();
}

void Q3Table::setNumCols(int c)
{
    if (c < 0)
        return;

    if (c < numCols()) {
        // Removed columns are no longer hidden, and should thus be removed from "hiddenCols"
        for (int cc = numCols()-1; cc >= c; --cc) {
            if (d->hiddenCols.find(cc))
                d->hiddenCols.remove(cc);
        }
    }

    fontChange(font()); // invalidate the sizeHintCache

    Q3PtrVector<Q3TableItem> tmp;
    Q3PtrVector<TableWidget> tmp2;
    saveContents(tmp, tmp2);

    bool updatesEnabled = topHeader->updatesEnabled();
    if (updatesEnabled)
        topHeader->setUpdatesEnabled(false);

    bool updateBefore;
    updateHeaderAndResizeContents(topHeader, numCols(), c, 100, updateBefore);

    restoreContents(tmp, tmp2);

    topHeader->calculatePositions();
    finishContentsResze(updateBefore);
    if (updatesEnabled) {
        topHeader->setUpdatesEnabled(true);
        topHeader->update();
    }
    topHeader->updateCache();
    if (curCol >= numCols()) {
        curCol = numCols() - 1;
        if (curCol < 0)
            curRow = -1;
        else
            repaintCell(curRow, curCol);
    }
}

/*! Sets the section labels of the verticalHeader() to \a labels */

void Q3Table::setRowLabels(const QStringList &labels)
{
    leftHeader->setLabels(labels);
}

/*! Sets the section labels of the horizontalHeader() to \a labels */

void Q3Table::setColumnLabels(const QStringList &labels)
{
   topHeader->setLabels(labels);
}

/*!
    This function returns the widget which should be used as an editor
    for the contents of the cell at \a row, \a col.

    If \a initFromCell is true, the editor is used to edit the current
    contents of the cell (so the editor widget should be initialized
    with this content). If \a initFromCell is false, the content of
    the cell is replaced with the new content which the user entered
    into the widget created by this function.

    The default functionality is as follows: if \a initFromCell is
    true or the cell has a Q3TableItem and the table item's
    Q3TableItem::isReplaceable() is false then the cell is asked to
    create an appropriate editor (using Q3TableItem::createEditor()).
    Otherwise a QLineEdit is used as the editor.

    If you want to create your own editor for certain cells, implement
    a custom Q3TableItem subclass and reimplement
    Q3TableItem::createEditor().

    If you are not using \l{Q3TableItem}s and you don't want to use a
    QLineEdit as the default editor, subclass Q3Table and reimplement
    this function with code like this:
    \snippet doc/src/snippets/code/src_qt3support_itemviews_q3table.cpp 5
    Ownership of the editor widget is transferred to the caller.

    If you reimplement this function return 0 for read-only cells. You
    will need to reimplement setCellContentFromEditor() to retrieve
    the data the user entered.

    \sa Q3TableItem::createEditor()
*/

QWidget *Q3Table::createEditor(int row, int col, bool initFromCell) const
{
    if (isReadOnly() || isRowReadOnly(row) || isColumnReadOnly(col))
        return 0;

    QWidget *e = 0;

    // the current item in the cell should be edited if possible
    Q3TableItem *i = item(row, col);
    if (initFromCell || (i && !i->isReplaceable())) {
        if (i) {
            if (i->editType() == Q3TableItem::Never)
                return 0;

            e = i->createEditor();
            if (!e)
                return 0;
        }
    }

    // no contents in the cell yet, so open the default editor
    if (!e) {
        e = new QLineEdit(viewport(), "qt_lineeditor");
        ((QLineEdit*)e)->setFrame(false);
    }

    return e;
}

/*!
    This function is called to start in-place editing of the cell at
    \a row, \a col. Editing is achieved by creating an editor
    (createEditor() is called) and setting the cell's editor with
    setCellWidget() to the newly created editor. (After editing is
    complete endEdit() will be called to replace the cell's content
    with the editor's content.) If \a replace is true the editor will
    start empty; otherwise it will be initialized with the cell's
    content (if any), i.e. the user will be modifying the original
    cell content.

    \sa endEdit()
*/

QWidget *Q3Table::beginEdit(int row, int col, bool replace)
{
    if (isReadOnly() || isRowReadOnly(row) || isColumnReadOnly(col))
        return 0;
    if ( row < 0 || row >= numRows() || col < 0 || col >= numCols() )
        return 0;
    Q3TableItem *itm = item(row, col);
    if (itm && !itm->isEnabled())
        return 0;
    if (cellWidget(row, col))
        return 0;
    ensureCellVisible(row, col);
    QWidget *e = createEditor(row, col, !replace);
    if (!e)
        return 0;
    setCellWidget(row, col, e);
    e->setActiveWindow();
    e->setFocus();
    updateCell(row, col);
    return e;
}

/*!
    This function is called when in-place editing of the cell at \a
    row, \a col is requested to stop.

    If the cell is not being edited or \a accept is false the function
    returns and the cell's contents are left unchanged.

    If \a accept is true the content of the editor must be transferred
    to the relevant cell. If \a replace is true the current content of
    this cell should be replaced by the content of the editor (this
    means removing the current Q3TableItem of the cell and creating a
    new one for the cell). Otherwise (if possible) the content of the
    editor should just be set to the existing Q3TableItem of this cell.

    setCellContentFromEditor() is called to replace the contents of
    the cell with the contents of the cell's editor.

    Finally clearCellWidget() is called to remove the editor widget.

    \sa setCellContentFromEditor(), beginEdit()
*/

void Q3Table::endEdit(int row, int col, bool accept, bool replace)
{
    QWidget *editor = cellWidget(row, col);
    if (!editor)
        return;

    if (!accept) {
        if (row == editRow && col == editCol)
            setEditMode(NotEditing, -1, -1);
        clearCellWidget(row, col);
        updateCell(row, col);
        viewport()->setFocus();
        updateCell(row, col);
        return;
    }

    Q3TableItem *i = item(row, col);
    QString oldContent;
    if (i)
        oldContent = i->text();

    if (!i || replace) {
        setCellContentFromEditor(row, col);
        i = item(row, col);
    } else {
        i->setContentFromEditor(editor);
    }

    if (row == editRow && col == editCol)
        setEditMode(NotEditing, -1, -1);

    viewport()->setFocus();
    updateCell(row, col);

    if (!i || (oldContent != i->text()))
        emit valueChanged(row, col);

    clearCellWidget(row, col);
}

/*!
    This function is called to replace the contents of the cell at \a
    row, \a col with the contents of the cell's editor.

    If there already exists a Q3TableItem for the cell,
    it calls Q3TableItem::setContentFromEditor() on this Q3TableItem.

    If, for example, you want to create different \l{Q3TableItem}s
    depending on the contents of the editor, you might reimplement
    this function.

    If you want to work without \l{Q3TableItem}s, you will need to
    reimplement this function to save the data the user entered into
    your data structure. (See the notes on large tables.)

    \sa Q3TableItem::setContentFromEditor() createEditor()
*/

void Q3Table::setCellContentFromEditor(int row, int col)
{
    QWidget *editor = cellWidget(row, col);
    if (!editor)
        return;

    Q3TableItem *i = item(row, col);
    if (i) {
        i->setContentFromEditor(editor);
    } else {
        QLineEdit *le = qobject_cast<QLineEdit*>(editor);
        if (le)
            setText(row, col, le->text());
    }
}

/*!
    Returns true if the \l EditMode is \c Editing or \c Replacing;
    otherwise (i.e. the \l EditMode is \c NotEditing) returns false.

    \sa Q3Table::EditMode
*/

bool Q3Table::isEditing() const
{
    return edMode != NotEditing;
}

/*!
    Returns the current edit mode

    \sa Q3Table::EditMode
*/

Q3Table::EditMode Q3Table::editMode() const
{
    return edMode;
}

/*!
    Returns the current edited row
*/

int Q3Table::currEditRow() const
{
    return editRow;
}

/*!
    Returns the current edited column
*/

int Q3Table::currEditCol() const
{
    return editCol;
}

/*!
    Returns a single integer which identifies a particular \a row and \a
    col by mapping the 2D table to a 1D array.

    This is useful, for example, if you have a sparse table and want to
    use a Q3IntDict to map integers to the cells that are used.
*/

int Q3Table::indexOf(int row, int col) const
{
    return (row * numCols()) + col;
}

/*! \internal
*/

void Q3Table::repaintSelections(Q3TableSelection *oldSelection,
                                Q3TableSelection *newSelection,
                                bool updateVertical, bool updateHorizontal)
{
    if (!oldSelection && !newSelection)
        return;
    if (oldSelection && newSelection && *oldSelection == *newSelection)
        return;
    if (oldSelection && !oldSelection->isActive())
         oldSelection = 0;

    bool optimizeOld = false;
    bool optimizeNew = false;

    QRect old;
    if (oldSelection)
        old = rangeGeometry(oldSelection->topRow(),
                             oldSelection->leftCol(),
                             oldSelection->bottomRow(),
                             oldSelection->rightCol(),
                             optimizeOld);
    else
        old = QRect(0, 0, 0, 0);

    QRect cur;
    if (newSelection)
        cur = rangeGeometry(newSelection->topRow(),
                             newSelection->leftCol(),
                             newSelection->bottomRow(),
                             newSelection->rightCol(),
                             optimizeNew);
    else
        cur = QRect(0, 0, 0, 0);
    int i;

    if (!optimizeOld || !optimizeNew ||
         old.width() > SHRT_MAX || old.height() > SHRT_MAX ||
         cur.width() > SHRT_MAX || cur.height() > SHRT_MAX) {
        QRect rr = cur.united(old);
        repaintContents(rr, false);
    } else {
        old = QRect(contentsToViewport2(old.topLeft()), old.size());
        cur = QRect(contentsToViewport2(cur.topLeft()), cur.size());
        QRegion r1(old);
        QRegion r2(cur);
        QRegion r3 = r1.subtracted(r2);
        QRegion r4 = r2.subtracted(r1);

        for (i = 0; i < (int)r3.rects().count(); ++i) {
            QRect r(r3.rects()[ i ]);
            r = QRect(viewportToContents2(r.topLeft()), r.size());
            repaintContents(r, false);
        }
        for (i = 0; i < (int)r4.rects().count(); ++i) {
            QRect r(r4.rects()[ i ]);
            r = QRect(viewportToContents2(r.topLeft()), r.size());
            repaintContents(r, false);
        }
    }

    int top, left, bottom, right;
    {
        int oldTopRow = oldSelection ? oldSelection->topRow() : numRows() - 1;
        int newTopRow = newSelection ? newSelection->topRow() : numRows() - 1;
        top = QMIN(oldTopRow, newTopRow);
    }

    {
        int oldLeftCol = oldSelection ? oldSelection->leftCol() : numCols() - 1;
        int newLeftCol = newSelection ? newSelection->leftCol() : numCols() - 1;
        left = QMIN(oldLeftCol, newLeftCol);
    }

    {
        int oldBottomRow = oldSelection ? oldSelection->bottomRow() : 0;
        int newBottomRow = newSelection ? newSelection->bottomRow() : 0;
        bottom = QMAX(oldBottomRow, newBottomRow);
    }

    {
        int oldRightCol = oldSelection ? oldSelection->rightCol() : 0;
        int newRightCol = newSelection ? newSelection->rightCol() : 0;
        right = QMAX(oldRightCol, newRightCol);
    }

    if (updateHorizontal && numCols() > 0 && left >= 0 && !isRowSelection(selectionMode())) {
        register int *s = &topHeader->states.data()[left];
        for (i = left; i <= right; ++i) {
            if (!isColumnSelected(i))
                *s = Q3TableHeader::Normal;
            else if (isColumnSelected(i, true))
                *s = Q3TableHeader::Selected;
            else
                *s = Q3TableHeader::Bold;
            ++s;
        }
        topHeader->repaint(false);
    }

    if (updateVertical && numRows() > 0 && top >= 0) {
        register int *s = &leftHeader->states.data()[top];
        for (i = top; i <= bottom; ++i) {
            if (!isRowSelected(i))
                *s = Q3TableHeader::Normal;
            else if (isRowSelected(i, true))
                *s = Q3TableHeader::Selected;
            else
                *s = Q3TableHeader::Bold;
            ++s;
        }
        leftHeader->repaint(false);
    }
}

/*!
    Repaints all selections
*/

void Q3Table::repaintSelections()
{
    if (selections.isEmpty())
        return;

    QRect r;
    for (Q3TableSelection *s = selections.first(); s; s = selections.next()) {
        bool b;
        r = r.united(rangeGeometry(s->topRow(),
                                    s->leftCol(),
                                    s->bottomRow(),
                                    s->rightCol(), b));
    }

    repaintContents(r, false);
}

/*!
    Clears all selections and repaints the appropriate regions if \a
    repaint is true.

    \sa removeSelection()
*/

void Q3Table::clearSelection(bool repaint)
{
    if (selections.isEmpty())
        return;
    bool needRepaint = !selections.isEmpty();

    QRect r;
    for (Q3TableSelection *s = selections.first(); s; s = selections.next()) {
        bool b;
        r = r.united(rangeGeometry(s->topRow(),
                                   s->leftCol(),
                                   s->bottomRow(),
                                   s->rightCol(), b));
    }

    currentSel = 0;
    selections.clear();

    if (needRepaint && repaint)
        repaintContents(r, false);

    leftHeader->setSectionStateToAll(Q3TableHeader::Normal);
    leftHeader->repaint(false);
    if (!isRowSelection(selectionMode())) {
        topHeader->setSectionStateToAll(Q3TableHeader::Normal);
        topHeader->repaint(false);
    }
    topHeader->setSectionState(curCol, Q3TableHeader::Bold);
    leftHeader->setSectionState(curRow, Q3TableHeader::Bold);
    emit selectionChanged();
}

/*! \internal
*/

QRect Q3Table::rangeGeometry(int topRow, int leftCol,
                             int bottomRow, int rightCol, bool &optimize)
{
    topRow = QMAX(topRow, rowAt(contentsY()));
    leftCol = QMAX(leftCol, columnAt(contentsX()));
    int ra = rowAt(contentsY() + visibleHeight());
    if (ra != -1)
        bottomRow = QMIN(bottomRow, ra);
    int ca = columnAt(contentsX() + visibleWidth());
    if (ca != -1)
        rightCol = QMIN(rightCol, ca);
    optimize = true;
    QRect rect;
    for (int r = topRow; r <= bottomRow; ++r) {
        for (int c = leftCol; c <= rightCol; ++c) {
            rect = rect.united(cellGeometry(r, c));
            Q3TableItem *i = item(r, c);
            if (i && (i->rowSpan() > 1 || i->colSpan() > 1))
                optimize = false;
        }
    }
    return rect;
}

/*!
    This function is called to activate the next cell if in-place
    editing was finished by pressing the Enter key.

    The default behaviour is to move from top to bottom, i.e. move to
    the cell beneath the cell being edited. Reimplement this function
    if you want different behaviour, e.g. moving from left to right.
*/

void Q3Table::activateNextCell()
{
    int firstRow = 0;
    while (d->hiddenRows.find(firstRow))
        firstRow++;
    int firstCol = 0;
    while (d->hiddenCols.find(firstCol))
        firstCol++;
    int nextRow = curRow;
    int nextCol = curCol;
    while (d->hiddenRows.find(++nextRow)) {}
    if (nextRow >= numRows()) {
        nextRow = firstRow;
        while (d->hiddenCols.find(++nextCol)) {}
        if (nextCol >= numCols())
            nextCol = firstCol;
    }

    if (!currentSel || !currentSel->isActive() ||
         (currentSel->leftCol() == currentSel->rightCol() &&
           currentSel->topRow() == currentSel->bottomRow())) {
        clearSelection();
        setCurrentCell(nextRow, nextCol);
    } else {
        if (curRow < currentSel->bottomRow())
            setCurrentCell(nextRow, curCol);
        else if (curCol < currentSel->rightCol())
            setCurrentCell(currentSel->topRow(), nextCol);
        else
            setCurrentCell(currentSel->topRow(), currentSel->leftCol());
    }

}

/*! \internal
*/

void Q3Table::fixRow(int &row, int y)
{
    if (row == -1) {
        if (y < 0)
            row = 0;
        else
            row = numRows() - 1;
    }
}

/*! \internal
*/

void Q3Table::fixCol(int &col, int x)
{
    if (col == -1) {
        if (x < 0)
            col = 0;
        else
            col = numCols() - 1;
    }
}

struct SortableTableItem
{
    Q3TableItem *item;
};

#if defined(Q_C_CALLBACKS)
extern "C" {
#endif

#ifdef Q_OS_WINCE
static int _cdecl cmpTableItems(const void *n1, const void *n2)
#else
static int cmpTableItems(const void *n1, const void *n2)
#endif
{
    if (!n1 || !n2)
        return 0;

    SortableTableItem *i1 = (SortableTableItem *)n1;
    SortableTableItem *i2 = (SortableTableItem *)n2;

    return i1->item->key().localeAwareCompare(i2->item->key());
}

#if defined(Q_C_CALLBACKS)
}
#endif

/*!
    Sorts column \a col. If \a ascending is true the sort is in
    ascending order, otherwise the sort is in descending order.

    If \a wholeRows is true, entire rows are sorted using swapRows();
    otherwise only cells in the column are sorted using swapCells().

    Note that if you are not using Q3TableItems you will need to
    reimplement swapRows() and swapCells(). (See the notes on large
    tables.)

    \sa swapRows()
*/

void Q3Table::sortColumn(int col, bool ascending, bool wholeRows)
{
    int filledRows = 0, i;
    for (i = 0; i < numRows(); ++i) {
        Q3TableItem *itm = item(i, col);
        if (itm)
            filledRows++;
    }

    if (!filledRows)
        return;

    SortableTableItem *items = new SortableTableItem[ filledRows ];
    int j = 0;
    for (i = 0; i < numRows(); ++i) {
        Q3TableItem *itm = item(i, col);
        if (!itm)
            continue;
        items[ j++ ].item = itm;
    }

    qsort(items, filledRows, sizeof(SortableTableItem), cmpTableItems);

    bool updatesWereEnabled = updatesEnabled();
    if (updatesWereEnabled)
        setUpdatesEnabled(false);
    for (i = 0; i < numRows(); ++i) {
        if (i < filledRows) {
            if (ascending) {
                if (items[ i ].item->row() == i)
                    continue;
                if (wholeRows)
                    swapRows(items[ i ].item->row(), i);
                else
                    swapCells(items[ i ].item->row(), col, i, col);
            } else {
                if (items[ i ].item->row() == filledRows - i - 1)
                    continue;
                if (wholeRows)
                    swapRows(items[ i ].item->row(), filledRows - i - 1);
                else
                    swapCells(items[ i ].item->row(), col,
                               filledRows - i - 1, col);
            }
        }
    }
    if (updatesWereEnabled)
        setUpdatesEnabled(true);
    if (topHeader)
         topHeader->setSortIndicator(col, ascending ? Qt::Ascending : Qt::Descending);

    if (!wholeRows)
        repaintContents(columnPos(col), contentsY(),
                         columnWidth(col), visibleHeight(), false);
    else
        repaintContents(contentsX(), contentsY(),
                         visibleWidth(), visibleHeight(), false);

    delete [] items;
}

/*!
    Hides row \a row.

    \sa showRow() hideColumn()
*/

void Q3Table::hideRow(int row)
{
    if (d->hiddenRows.find(row))
        return;
    d->hiddenRows.replace(row, new int(leftHeader->sectionSize(row)));
    leftHeader->resizeSection(row, 0);
    leftHeader->setResizeEnabled(false, row);
    if (isRowStretchable(row))
        leftHeader->numStretches--;
    rowHeightChanged(row);
    if (curRow == row) {
        int r = curRow;
        int c = curCol;
        int k = (r >= numRows() - 1 ? Key_Up : Key_Down);
        fixCell(r, c, k);
        if (numRows() > 0)
            setCurrentCell(r, c);
    }
}

/*!
    Hides column \a col.

    \sa showColumn() hideRow()
*/

void Q3Table::hideColumn(int col)
{
    if (!numCols() || d->hiddenCols.find(col))
        return;
    d->hiddenCols.replace(col, new int(topHeader->sectionSize(col)));
    topHeader->resizeSection(col, 0);
    topHeader->setResizeEnabled(false, col);
    if (isColumnStretchable(col))
        topHeader->numStretches--;
    columnWidthChanged(col);
    if (curCol == col) {
        int r = curRow;
        int c = curCol;
        int k = (c >= numCols() - 1 ? Key_Left : Key_Right);
        fixCell(r, c, k);
        if (numCols() > 0)
            setCurrentCell(r, c);
    }
}

/*!
    Shows row \a row.

    \sa hideRow() showColumn()
*/

void Q3Table::showRow(int row)
{
    int *h = d->hiddenRows.find(row);
    if (h) {
        int rh = *h;
        d->hiddenRows.remove(row);
        setRowHeight(row, rh);
        if (isRowStretchable(row))
            leftHeader->numStretches++;
    } else if (rowHeight(row) == 0) {
        setRowHeight(row, 20);
    }
    leftHeader->setResizeEnabled(true, row);
}

/*!
    Shows column \a col.

    \sa hideColumn() showRow()
*/

void Q3Table::showColumn(int col)
{
    int *w = d->hiddenCols.find(col);
    if (w) {
        int cw = *w;
        d->hiddenCols.remove(col);
        setColumnWidth(col, cw);
        if (isColumnStretchable(col))
            topHeader->numStretches++;
    } else if (columnWidth(col) == 0) {
        setColumnWidth(col, 20);
    }
    topHeader->setResizeEnabled(true, col);
}

/*!
    Returns true if row \a row is hidden; otherwise returns
    false.

    \sa hideRow(), isColumnHidden()
*/
bool Q3Table::isRowHidden(int row) const
{
    return d->hiddenRows.find(row);
}

/*!
    Returns true if column \a col is hidden; otherwise returns
    false.

    \sa hideColumn(), isRowHidden()
*/
bool Q3Table::isColumnHidden(int col) const
{
    return d->hiddenCols.find(col);
}

/*!
    Resizes column \a col to be \a w pixels wide.

    \sa columnWidth() setRowHeight()
*/

void Q3Table::setColumnWidth(int col, int w)
{
    int *ow = d->hiddenCols.find(col);
    if (ow) {
        d->hiddenCols.replace(col, new int(w));
    } else {
        topHeader->resizeSection(col, w);
        columnWidthChanged(col);
    }
}

/*!
    Resizes row \a row to be \a h pixels high.

    \sa rowHeight() setColumnWidth()
*/

void Q3Table::setRowHeight(int row, int h)
{
    int *oh = d->hiddenRows.find(row);
    if (oh) {
        d->hiddenRows.replace(row, new int(h));
    } else {
        leftHeader->resizeSection(row, h);
        rowHeightChanged(row);
    }
}

/*!
    Resizes column \a col so that the column width is wide enough to
    display the widest item the column contains.

    \sa adjustRow()
*/

void Q3Table::adjustColumn(int col)
{
    int w;
    if ( currentColumn() == col ) {
        QFont f = font();
        f.setBold(true);
        w = topHeader->sectionSizeHint( col, QFontMetrics(f) ).width();
    } else {
        w = topHeader->sectionSizeHint( col, fontMetrics() ).width();
    }
    if (topHeader->iconSet(col))
        w += topHeader->iconSet(col)->pixmap().width();
    w = QMAX(w, 20);
    for (int i = 0; i < numRows(); ++i) {
        Q3TableItem *itm = item(i, col);
        if (!itm) {
            QWidget *widget = cellWidget(i, col);
            if (widget)
                w = QMAX(w, widget->sizeHint().width());
        } else {
            if (itm->colSpan() > 1)
                w = QMAX(w, itm->sizeHint().width() / itm->colSpan());
            else
                w = QMAX(w, itm->sizeHint().width());
        }
    }
    w = QMAX(w, QApplication::globalStrut().width());
    setColumnWidth(col, w);
}

/*!
    Resizes row \a row so that the row height is tall enough to
    display the tallest item the row contains.

    \sa adjustColumn()
*/

void Q3Table::adjustRow(int row)
{
    int h = 20;
    h = QMAX(h, leftHeader->sectionSizeHint(row, leftHeader->fontMetrics()).height());
    if (leftHeader->iconSet(row))
        h = QMAX(h, leftHeader->iconSet(row)->pixmap().height());
    for (int i = 0; i < numCols(); ++i) {
        Q3TableItem *itm = item(row, i);
        if (!itm) {
            QWidget *widget = cellWidget(row, i);
            if (widget)
                h = QMAX(h, widget->sizeHint().height());
        } else {
            if (itm->rowSpan() > 1)
                h = QMAX(h, itm->sizeHint().height() / itm->rowSpan());
            else
                h = QMAX(h, itm->sizeHint().height());
        }
    }
    h = QMAX(h, QApplication::globalStrut().height());
    setRowHeight(row, h);
}

/*!
    If \a stretch is true, column \a col is set to be stretchable;
    otherwise column \a col is set to be unstretchable.

    If the table widget's width decreases or increases stretchable
    columns will grow narrower or wider to fit the space available as
    completely as possible. The user cannot manually resize stretchable
    columns.

    \sa isColumnStretchable() setRowStretchable() adjustColumn()
*/

void Q3Table::setColumnStretchable(int col, bool stretch)
{
    topHeader->setSectionStretchable(col, stretch);

    if (stretch && d->hiddenCols.find(col))
        topHeader->numStretches--;
}

/*!
    If \a stretch is true, row \a row is set to be stretchable;
    otherwise row \a row is set to be unstretchable.

    If the table widget's height decreases or increases stretchable
    rows will grow shorter or taller to fit the space available as
    completely as possible. The user cannot manually resize
    stretchable rows.

    \sa isRowStretchable() setColumnStretchable()
*/

void Q3Table::setRowStretchable(int row, bool stretch)
{
    leftHeader->setSectionStretchable(row, stretch);

    if (stretch && d->hiddenRows.find(row))
        leftHeader->numStretches--;
}

/*!
    Returns true if column \a col is stretchable; otherwise returns
    false.

    \sa setColumnStretchable() isRowStretchable()
*/

bool Q3Table::isColumnStretchable(int col) const
{
    return topHeader->isSectionStretchable(col);
}

/*!
    Returns true if row \a row is stretchable; otherwise returns
    false.

    \sa setRowStretchable() isColumnStretchable()
*/

bool Q3Table::isRowStretchable(int row) const
{
    return leftHeader->isSectionStretchable(row);
}

/*!
    Takes the table item \a i out of the table. This function does \e
    not delete the table item. You must either delete the table item
    yourself or put it into a table (using setItem()) which will then
    take ownership of it.

    Use this function if you want to move an item from one cell in a
    table to another, or to move an item from one table to another,
    reinserting the item with setItem().

    If you want to exchange two cells use swapCells().
*/

void Q3Table::takeItem(Q3TableItem *i)
{
    if (!i)
        return;
    if (i->row() != -1 && i->col() != -1) {
        QRect rect = cellGeometry(i->row(), i->col());
        contents.setAutoDelete(false);
        int bottom = i->row() + i->rowSpan();
        if (bottom > numRows())
            bottom = numRows();
        int right = i->col() + i->colSpan();
        if (right > numCols())
            right = numCols();
        for (int r = i->row(); r < bottom; ++r) {
            for (int c = i->col(); c < right; ++c)
                contents.remove(indexOf(r, c));
        }
        contents.setAutoDelete(true);
        repaintContents(rect, false);
        int orow = i->row();
        int ocol = i->col();
        i->setRow(-1);
        i->setCol(-1);
        i->updateEditor(orow, ocol);
    }
    i->t = 0;
}

/*!
    Sets the widget \a e to the cell at \a row, \a col and takes care of
    placing and resizing the widget when the cell geometry changes.

    By default widgets are inserted into a vector with numRows() *
    numCols() elements. In very large tables you will probably want to
    store the widgets in a data structure that consumes less memory (see
    the notes on large tables). To support the use of your own data
    structure this function calls insertWidget() to add the widget to
    the internal data structure. To use your own data structure
    reimplement insertWidget(), cellWidget() and clearCellWidget().

    Cell widgets are created dynamically with the \c new operator. The
    cell widgets are destroyed automatically once the table is
    destroyed; the table takes ownership of the widget when using
    setCellWidget.

*/

void Q3Table::setCellWidget(int row, int col, QWidget *e)
{
    if (!e || row >= numRows() || col >= numCols())
        return;

    QWidget *w = cellWidget(row, col);
    if (w && row == editRow && col == editCol)
        endEdit(editRow, editCol, false, edMode != Editing);

    e->installEventFilter(this);
    clearCellWidget(row, col);
    if (e->parent() != viewport())
        e->reparent(viewport(), QPoint(0,0));
    Q3TableItem *itm = item(row, col);
    if (itm && itm->row() >= 0 && itm->col() >= 0) { // get the correct row and col if the item is spanning
        row = itm->row();
        col = itm->col();
    }
    insertWidget(row, col, e);
    QRect cr = cellGeometry(row, col);
    e->resize(cr.size());
    moveChild(e, cr.x(), cr.y());
    e->show();
}

/*!
    Inserts widget \a w at \a row, \a col into the internal
    data structure. See the documentation of setCellWidget() for
    further details.

    If you don't use \l{Q3TableItem}s you may need to reimplement this
    function: see the notes on large tables.
*/

void Q3Table::insertWidget(int row, int col, QWidget *w)
{
    if (row < 0 || col < 0 || row > numRows() - 1 || col > numCols() - 1)
        return;

    if ((int)widgets.size() != numRows() * numCols())
        widgets.resize(numRows() * numCols());

    widgets.insert(indexOf(row, col), w);
}

/*!
    Returns the widget that has been set for the cell at \a row, \a
    col, or 0 if no widget has been set.

    If you don't use \l{Q3TableItem}s you may need to reimplement this
    function: see the notes on large tables.

    \sa clearCellWidget() setCellWidget()
*/

QWidget *Q3Table::cellWidget(int row, int col) const
{
    if (row < 0 || col < 0 || row > numRows() - 1 || col > numCols() - 1)
        return 0;

    if ((int)widgets.size() != numRows() * numCols())
        ((Q3Table*)this)->widgets.resize(numRows() * numCols());

    return widgets[ indexOf(row, col) ];
}

/*!
    Removes the widget (if there is one) set for the cell at \a row,
    \a col.

    If you don't use \l{Q3TableItem}s you may need to reimplement this
    function: see the notes on large tables.

    This function deletes the widget at \a row, \a col. Note that the
    widget is not deleted immediately; instead QObject::deleteLater()
    is called on the widget to avoid problems with timing issues.

    \sa cellWidget() setCellWidget()
*/

void Q3Table::clearCellWidget(int row, int col)
{
    if (row < 0 || col < 0 || row > numRows() - 1 || col > numCols() - 1)
        return;

    if ((int)widgets.size() != numRows() * numCols())
        widgets.resize(numRows() * numCols());

    QWidget *w = cellWidget(row, col);
    if (w) {
        w->removeEventFilter(this);
        w->hide();
        w->deleteLater();
    }
    widgets.setAutoDelete(false);
    widgets.remove(indexOf(row, col));
    widgets.setAutoDelete(true);
}

/*!
    \fn void Q3Table::dropped (QDropEvent * e)

    This signal is emitted when a drop event occurred on the table.

    \a e contains information about the drop.
*/

/*!
    If \a b is true, the table starts a drag (see dragObject()) when
    the user presses and moves the mouse on a selected cell.
*/

void Q3Table::setDragEnabled(bool b)
{
    dEnabled = b;
}

/*!
    If this function returns true, the table supports dragging.

    \sa setDragEnabled()
*/

bool Q3Table::dragEnabled() const
{
    return dEnabled;
}

/*!
    Inserts \a count empty rows at row \a row. Also clears the selection(s).

    \sa insertColumns() removeRow()
*/

void Q3Table::insertRows(int row, int count)
{
    // special case, so a call like insertRow(currentRow(), 1) also
    // works, when we have 0 rows and currentRow() is -1
    if (row == -1 && curRow == -1)
        row = 0;
    if (row < 0 || count <= 0)
        return;

    if (curRow >= row && curRow < row + count)
        curRow = row + count;

    --row;
    if (row >= numRows())
        return;

    bool updatesWereEnabled = updatesEnabled();
    if (updatesWereEnabled)
        setUpdatesEnabled(false);
    bool leftHeaderUpdatesEnabled = leftHeader->updatesEnabled();
    if (leftHeaderUpdatesEnabled)
        leftHeader->setUpdatesEnabled(false);
    int oldLeftMargin = leftMargin();

    setNumRows(numRows() + count);

    for (int i = numRows() - count - 1; i > row; --i)
        leftHeader->swapSections(i, i + count);

    if (leftHeaderUpdatesEnabled)
        leftHeader->setUpdatesEnabled(leftHeaderUpdatesEnabled);

    if (updatesWereEnabled)
        setUpdatesEnabled(true);

    int cr = QMAX(0, currentRow());
    int cc = QMAX(0, currentColumn());
    if (curRow > row)
        curRow -= count; // this is where curRow was
    setCurrentCell(cr, cc, true, false); // without ensureCellVisible

    // Repaint the header
    if (leftHeaderUpdatesEnabled) {
        int y = rowPos(row) - contentsY();
        if (leftMargin() != oldLeftMargin || d->hasRowSpan)
            y = 0; // full repaint
        QRect rect(0, y, leftHeader->width(), contentsHeight());
        leftHeader->update(rect);
    }

    if (updatesWereEnabled) {
        int p = rowPos(row);
        if (d->hasRowSpan)
            p = contentsY();
        updateContents(contentsX(), p, visibleWidth(), contentsHeight() + 1);
    }
}

/*!
    Inserts \a count empty columns at column \a col.  Also clears the selection(s).

    \sa insertRows() removeColumn()
*/

void Q3Table::insertColumns(int col, int count)
{
    // see comment in insertRows()
    if (col == -1 && curCol == -1)
        col = 0;
    if (col < 0 || count <= 0)
        return;

    if (curCol >= col && curCol < col + count)
        curCol = col + count;

    --col;
    if (col >= numCols())
        return;

    bool updatesWereEnabled = updatesEnabled();
    if (updatesWereEnabled)
        setUpdatesEnabled(false);
    bool topHeaderUpdatesEnabled = topHeader->updatesEnabled();
    if (topHeaderUpdatesEnabled)
        topHeader->setUpdatesEnabled(false);
    int oldTopMargin = topMargin();

    setNumCols(numCols() + count);

    for (int i = numCols() - count - 1; i > col; --i)
        topHeader->swapSections(i, i + count);

    if (topHeaderUpdatesEnabled)
        topHeader->setUpdatesEnabled(true);
    if (updatesWereEnabled)
        setUpdatesEnabled(true);

    int cr = QMAX(0, currentRow());
    int cc = QMAX(0, currentColumn());
    if (curCol > col)
        curCol -= count; // this is where curCol was
    setCurrentCell(cr, cc, true, false); // without ensureCellVisible

    // Repaint the header
    if (topHeaderUpdatesEnabled) {
        int x = columnPos(col) - contentsX();
        if (topMargin() != oldTopMargin || d->hasColSpan)
            x = 0; // full repaint
        QRect rect(x, 0, contentsWidth(), topHeader->height());
        topHeader->update(rect);
    }

    if (updatesWereEnabled) {
        int p = columnPos(col);
        if (d->hasColSpan)
            p = contentsX();
        updateContents(p, contentsY(), contentsWidth() + 1, visibleHeight());
    }
}

/*!
    Removes row \a row, and deletes all its cells including any table
    items and widgets the cells may contain. Also clears the selection(s).

    \sa hideRow() insertRows() removeColumn() removeRows()
*/

void Q3Table::removeRow(int row)
{
    if (row < 0 || row >= numRows())
        return;
    if (row < numRows() - 1) {
        if (d->hiddenRows.find(row))
            d->hiddenRows.remove(row);

        for (int i = row; i < numRows() - 1; ++i)
            ((Q3TableHeader*)verticalHeader())->swapSections(i, i + 1);
    }
    setNumRows(numRows() - 1);
}

/*!
    Removes the rows listed in the array \a rows, and deletes all their
    cells including any table items and widgets the cells may contain.

    The array passed in must only contain valid rows (in the range
    from 0 to numRows() - 1) with no duplicates, and must be sorted in
    ascending order. Also clears the selection(s).

    \sa removeRow() insertRows() removeColumns()
*/

void Q3Table::removeRows(const Q3MemArray<int> &rows)
{
    if (rows.count() == 0)
        return;
    int i;
    for (i = 0; i < (int)rows.count() - 1; ++i) {
        for (int j = rows[i] - i; j < rows[i + 1] - i - 1; j++) {
            ((Q3TableHeader*)verticalHeader())->swapSections(j, j + i + 1);
        }
    }

    for (int j = rows[i] - i; j < numRows() - (int)rows.size(); j++)
        ((Q3TableHeader*)verticalHeader())->swapSections(j, j + rows.count());

    setNumRows(numRows() - rows.count());
}

/*!
    Removes column \a col, and deletes all its cells including any
    table items and widgets the cells may contain. Also clears the
    selection(s).

    \sa removeColumns() hideColumn() insertColumns() removeRow()
*/

void Q3Table::removeColumn(int col)
{
    if (col < 0 || col >= numCols())
        return;
    if (col < numCols() - 1) {
        if (d->hiddenCols.find(col))
            d->hiddenCols.remove(col);

        for (int i = col; i < numCols() - 1; ++i)
            ((Q3TableHeader*)horizontalHeader())->swapSections(i, i + 1);
    }
    setNumCols(numCols() - 1);
}

/*!
    Removes the columns listed in the array \a cols, and deletes all
    their cells including any table items and widgets the cells may
    contain.

    The array passed in must only contain valid columns (in the range
    from 0 to numCols() - 1) with no duplicates, and must be sorted in
    ascending order. Also clears the selection(s).

   \sa removeColumn() insertColumns() removeRows()
*/

void Q3Table::removeColumns(const Q3MemArray<int> &cols)
{
    if (cols.count() == 0)
        return;
    int i;
    for (i = 0; i < (int)cols.count() - 1; ++i) {
        for (int j = cols[i] - i; j < cols[i + 1] - i - 1; j++) {
            ((Q3TableHeader*)horizontalHeader())->swapSections(j, j + i + 1);
        }
    }

    for (int j = cols[i] - i; j < numCols() - (int)cols.size(); j++)
        ((Q3TableHeader*)horizontalHeader())->swapSections(j, j + cols.count());

    setNumCols(numCols() - cols.count());
}

/*!
    Starts editing the cell at \a row, \a col.

    If \a replace is true the content of this cell will be replaced by
    the content of the editor when editing is finished, i.e. the user
    will be entering new data; otherwise the current content of the
    cell (if any) will be modified in the editor.

    \sa beginEdit()
*/

void Q3Table::editCell(int row, int col, bool replace)
{
    if (row < 0 || col < 0 || row > numRows() - 1 || col > numCols() - 1)
        return;

    if (beginEdit(row, col, replace)) {
        edMode = Editing;
        editRow = row;
        editCol = col;
    }
}

#ifndef QT_NO_DRAGANDDROP

/*!
    This event handler is called whenever a Q3Table object receives a
    \l QDragEnterEvent \a e, i.e. when the user pressed the mouse
    button to drag something.

    The focus is moved to the cell where the QDragEnterEvent occurred.
*/

void Q3Table::contentsDragEnterEvent(QDragEnterEvent *e)
{
    oldCurrentRow = curRow;
    oldCurrentCol = curCol;
    int tmpRow = rowAt(e->pos().y());
    int tmpCol = columnAt(e->pos().x());
    fixRow(tmpRow, e->pos().y());
    fixCol(tmpCol, e->pos().x());
    if (e->source() != (QObject*)cellWidget(currentRow(), currentColumn()))
        setCurrentCell(tmpRow, tmpCol, false, true);
    e->accept();
}

/*!
    This event handler is called whenever a Q3Table object receives a
    \l QDragMoveEvent \a e, i.e. when the user actually drags the
    mouse.

    The focus is moved to the cell where the QDragMoveEvent occurred.
*/

void Q3Table::contentsDragMoveEvent(QDragMoveEvent *e)
{
    int tmpRow = rowAt(e->pos().y());
    int tmpCol = columnAt(e->pos().x());
    fixRow(tmpRow, e->pos().y());
    fixCol(tmpCol, e->pos().x());
    if (e->source() != (QObject*)cellWidget(currentRow(), currentColumn()))
        setCurrentCell(tmpRow, tmpCol, false, true);
    e->accept();
}

/*!
    This event handler is called when a drag activity leaves \e this
    Q3Table object with event \a e.
*/

void Q3Table::contentsDragLeaveEvent(QDragLeaveEvent *)
{
    setCurrentCell(oldCurrentRow, oldCurrentCol, false, true);
}

/*!
    This event handler is called when the user ends a drag and drop by
    dropping something onto \e this Q3Table and thus triggers the drop
    event, \a e.
*/

void Q3Table::contentsDropEvent(QDropEvent *e)
{
    setCurrentCell(oldCurrentRow, oldCurrentCol, false, true);
    emit dropped(e);
}

/*!
    If the user presses the mouse on a selected cell, starts moving
    (i.e. dragging), and dragEnabled() is true, this function is
    called to obtain a drag object. A drag using this object begins
    immediately unless dragObject() returns 0.

    By default this function returns 0. You might reimplement it and
    create a Q3DragObject depending on the selected items.

    \sa dropped()
*/

Q3DragObject *Q3Table::dragObject()
{
    return 0;
}

/*!
    Starts a drag.

    Usually you don't need to call or reimplement this function yourself.

    \sa dragObject()
*/

void Q3Table::startDrag()
{
    if (startDragRow == -1 || startDragCol == -1)
        return;

    startDragRow = startDragCol = -1;

    Q3DragObject *drag = dragObject();
    if (!drag)
        return;

    drag->drag();
}

#endif

/*! \reimp */
void Q3Table::windowActivationChange(bool oldActive)
{
    if (oldActive && autoScrollTimer)
        autoScrollTimer->stop();

    if (!isVisible())
        return;

    if (palette().active() != palette().inactive())
        updateContents();
}

/*!
    \internal
*/
void Q3Table::setEnabled(bool b)
{
    if (!b) {
        // editor will lose focus, causing a crash deep in setEnabled(),
        // so we'll end the edit early.
        endEdit(editRow, editCol, true, edMode != Editing);
    }
    Q3ScrollView::setEnabled(b);
}


/*
    \class Q3TableHeader
    \brief The Q3TableHeader class allows for creation and manipulation
    of table headers.

    \compat

   Q3Table uses this subclass of Q3Header for its headers. Q3Table has a
   horizontalHeader() for displaying column labels, and a
   verticalHeader() for displaying row labels.

*/

/*
    \enum Q3TableHeader::SectionState

    This enum type denotes the state of the header's text

    \value Normal the default
    \value Bold
    \value Selected  typically represented by showing the section "sunken"
    or "pressed in"
*/

/*!
    Creates a new table header called \a name with \a i sections. It
    is a child of widget \a parent and attached to table \a t.
*/

Q3TableHeader::Q3TableHeader(int i, Q3Table *t,
                            QWidget *parent, const char *name)
    : Q3Header(i, parent, name), mousePressed(false), startPos(-1),
      table(t), caching(false), resizedSection(-1),
      numStretches(0)
{
    setIsATableHeader(true);
    d = 0;
    states.resize(i);
    stretchable.resize(i);
    states.fill(Normal, -1);
    stretchable.fill(false, -1);
    autoScrollTimer = new QTimer(this);
    connect(autoScrollTimer, SIGNAL(timeout()),
             this, SLOT(doAutoScroll()));
#ifndef NO_LINE_WIDGET
    line1 = new QWidget(table->viewport(), "qt_line1");
    line1->hide();
    line1->setBackgroundMode(PaletteText);
    table->addChild(line1);
    line2 = new QWidget(table->viewport(), "qt_line2");
    line2->hide();
    line2->setBackgroundMode(PaletteText);
    table->addChild(line2);
#else
    d = new Q3TableHeaderPrivate;
    d->oldLinePos = -1; //outside, in contents coords
#endif
    connect(this, SIGNAL(sizeChange(int,int,int)),
             this, SLOT(sectionWidthChanged(int,int,int)));
    connect(this, SIGNAL(indexChange(int,int,int)),
             this, SLOT(indexChanged(int,int,int)));

    stretchTimer = new QTimer(this);
    widgetStretchTimer = new QTimer(this);
    connect(stretchTimer, SIGNAL(timeout()),
             this, SLOT(updateStretches()));
    connect(widgetStretchTimer, SIGNAL(timeout()),
             this, SLOT(updateWidgetStretches()));
    startPos = -1;
}

/*!
    Adds a new section, \a size pixels wide (or high for vertical
    headers) with the label \a s. If \a size is negative the section's
    size is calculated based on the width (or height) of the label's
    text.
*/

void Q3TableHeader::addLabel(const QString &s , int size)
{
    Q3Header::addLabel(s, size);
    if (count() > (int)states.size()) {
        int s = states.size();
        states.resize(count());
        stretchable.resize(count());
        for (; s < count(); ++s) {
            states[ s ] = Normal;
            stretchable[ s ] = false;
        }
    }
}

void Q3TableHeader::removeLabel(int section)
{
    Q3Header::removeLabel(section);
    if (section == (int)states.size() - 1) {
        states.resize(states.size() - 1);
        stretchable.resize(stretchable.size() - 1);
    }
}

void Q3TableHeader::resizeArrays(int n)
{
    int old = states.size();
    states.resize(n);
    stretchable.resize(n);
    if (n > old) {
        for (int i = old; i < n; ++i) {
            stretchable[ i ] = false;
            states[ i ] = Normal;
        }
    }
}

void Q3TableHeader::setLabel(int section, const QString & s, int size)
{
    Q3Header::setLabel(section, s, size);
    sectionLabelChanged(section);
}

void Q3TableHeader::setLabel(int section, const QIconSet & iconset,
                             const QString & s, int size)
{
    Q3Header::setLabel(section, iconset, s, size);
    sectionLabelChanged(section);
}

/*!
    Sets the SectionState of section \a s to \a astate.

    \sa sectionState()
*/

void Q3TableHeader::setSectionState(int s, SectionState astate)
{
    if (s < 0 || s >= (int)states.count())
        return;
    if (states.data()[ s ] == astate)
        return;
    if (isRowSelection(table->selectionMode()) && orientation() == Horizontal)
        return;

    states.data()[ s ] = astate;
    if (updatesEnabled()) {
        if (orientation() == Horizontal)
            repaint(sectionPos(s) - offset(), 0, sectionSize(s), height(), false);
        else
            repaint(0, sectionPos(s) - offset(), width(), sectionSize(s), false);
    }
}

void Q3TableHeader::setSectionStateToAll(SectionState state)
{
    if (isRowSelection(table->selectionMode()) && orientation() == Horizontal)
        return;

    register int *d = (int *) states.data();
    int n = count();

    while (n >= 4) {
        d[0] = state;
        d[1] = state;
        d[2] = state;
        d[3] = state;
        d += 4;
        n -= 4;
    }

    if (n > 0) {
        d[0] = state;
        if (n > 1) {
            d[1] = state;
            if (n > 2) {
                d[2] = state;
            }
        }
    }
}

/*!
    Returns the SectionState of section \a s.

    \sa setSectionState()
*/

Q3TableHeader::SectionState Q3TableHeader::sectionState(int s) const
{
    return (s < 0 || s >= (int)states.count() ? Normal : (Q3TableHeader::SectionState)states[s]);
}

/*! \reimp
*/

void Q3TableHeader::paintEvent(QPaintEvent *e)
{
    QPainter p(this);
    p.setPen(colorGroup().buttonText());
    int pos = orientation() == Horizontal
                     ? e->rect().left()
                     : e->rect().top();
    int id = mapToIndex(sectionAt(pos + offset()));
    if (id < 0) {
        if (pos > 0)
            return;
        else
            id = 0;
    }

    QRegion reg = e->region();
    for (int i = id; i < count(); i++) {
        QRect r = sRect(i);
        reg -= r;
        p.save();
        if (!(orientation() == Horizontal && isRowSelection(table->selectionMode())) &&
             (sectionState(i) == Bold || sectionState(i) == Selected)) {
            QFont f(font());
            f.setBold(true);
            p.setFont(f);
        }
        paintSection(&p, i, r);
        p.restore();
        if ((orientation() == Horizontal && r. right() >= e->rect().right())
            || (orientation() == Vertical && r. bottom() >= e->rect().bottom()))
            return;
    }
    p.end();
    if (!reg.isEmpty())
        erase(reg);
}

/*!
    \reimp

    Paints the header section with index \a index into the rectangular
    region \a fr on the painter \a p.
*/

void Q3TableHeader::paintSection(QPainter *p, int index, const QRect& fr)
{
    int section = mapToSection(index);
    if (section < 0 || cellSize(section) <= 0)
        return;

   if (sectionState(index) != Selected ||
         (orientation() == Horizontal && isRowSelection(table->selectionMode()))) {
        Q3Header::paintSection(p, index, fr);
   } else {
       QStyleOptionHeader opt;
       opt.palette = palette();
       opt.rect = fr;
       opt.state = QStyle::State_Off | (orient == Qt::Horizontal ? QStyle::State_Horizontal
                                                                 : QStyle::State_None);
       if (isEnabled())
           opt.state |= QStyle::State_Enabled;
       if (isClickEnabled()) {
           if (sectionState(index) == Selected) {
               opt.state |= QStyle::State_Sunken;
               if (!mousePressed)
                   opt.state |= QStyle::State_On;
           }
       }
       if (!(opt.state & QStyle::State_Sunken))
           opt.state |= QStyle::State_Raised;
       style()->drawControl(QStyle::CE_HeaderSection, &opt, p, this);
       paintSectionLabel(p, index, fr);
   }
}

static int real_pos(const QPoint &p, Qt::Orientation o)
{
    if (o == Qt::Horizontal)
        return p.x();
    return p.y();
}

/*! \reimp
*/

void Q3TableHeader::mousePressEvent(QMouseEvent *e)
{
    if (e->button() != LeftButton)
        return;
    Q3Header::mousePressEvent(e);
    mousePressed = true;
    pressPos = real_pos(e->pos(), orientation());
    if (!table->currentSel || (e->state() & ShiftButton) != ShiftButton)
        startPos = -1;
    setCaching(true);
    resizedSection = -1;
#ifdef QT_NO_CURSOR
    isResizing = false;
#else
    isResizing = cursor().shape() != ArrowCursor;
    if (!isResizing && sectionAt(pressPos) != -1)
        doSelection(e);
#endif
}

/*! \reimp
*/

void Q3TableHeader::mouseMoveEvent(QMouseEvent *e)
{
    if ((e->state() & MouseButtonMask) != LeftButton // Using LeftButton simulates old behavior.
#ifndef QT_NO_CURSOR
         || cursor().shape() != ArrowCursor
#endif
         || ((e->state() & ControlButton) == ControlButton &&
              (orientation() == Horizontal
             ? table->columnMovingEnabled() : table->rowMovingEnabled()))) {
        Q3Header::mouseMoveEvent(e);
        return;
    }

    if (!doSelection(e))
        Q3Header::mouseMoveEvent(e);
}

bool Q3TableHeader::doSelection(QMouseEvent *e)
{
    int p = real_pos(e->pos(), orientation()) + offset();

    if (isRowSelection(table->selectionMode())) {
        if (orientation() == Horizontal)
            return true;
        if (table->selectionMode() == Q3Table::SingleRow) {
            int secAt = sectionAt(p);
            if (secAt == -1)
                return true;
            table->setCurrentCell(secAt, table->currentColumn());
            return true;
        }
    }

    if (startPos == -1) {
         int secAt = sectionAt(p);
        if (((e->state() & ControlButton) != ControlButton && (e->state() & ShiftButton) != ShiftButton)
            || table->selectionMode() == Q3Table::Single
            || table->selectionMode() == Q3Table::SingleRow) {
            startPos = p;
            bool b = table->signalsBlocked();
            table->blockSignals(true);
            table->clearSelection();
            table->blockSignals(b);
        }
        saveStates();

        if (table->selectionMode() != Q3Table::NoSelection) {
            startPos = p;
            Q3TableSelection *oldSelection = table->currentSel;

            if (orientation() == Vertical) {
                if (!table->isRowSelected(secAt, true)) {
                    table->currentSel = new Q3TableSelection();
                    table->selections.append(table->currentSel);
                    table->currentSel->init(secAt, 0);
                    table->currentSel->expandTo(secAt, table->numCols() - 1);
                    emit table->selectionChanged();
                }
                table->setCurrentCell(secAt, 0);
            } else { // orientation == Horizontal
                if (!table->isColumnSelected(secAt, true)) {
                    table->currentSel = new Q3TableSelection();
                    table->selections.append(table->currentSel);
                    table->currentSel->init(0, secAt);
                    table->currentSel->expandTo(table->numRows() - 1, secAt);
                    emit table->selectionChanged();
                }
                table->setCurrentCell(0, secAt);
            }

            if ((orientation() == Horizontal && table->isColumnSelected(secAt))
                || (orientation() == Vertical && table->isRowSelected(secAt))) {
                setSectionState(secAt, Selected);
            }

             table->repaintSelections(oldSelection, table->currentSel,
                                       orientation() == Horizontal,
                                       orientation() == Vertical);
            if (sectionAt(p) != -1)
                 endPos = p;

             return true;
        }
    }

    if (sectionAt(p) != -1)
        endPos = p;
    if (startPos != -1) {
        updateSelections();
        p -= offset();
        if (orientation() == Horizontal && (p < 0 || p > width())) {
            doAutoScroll();
            autoScrollTimer->start(100, true);
        } else if (orientation() == Vertical && (p < 0 || p > height())) {
            doAutoScroll();
            autoScrollTimer->start(100, true);
        }
        return true;
    }
    return table->selectionMode() == Q3Table::NoSelection;
}

static inline bool mayOverwriteMargin(int before, int after)
{
    /*
      0 is the only user value that we always respect. We also never
      shrink a margin, in case the user wanted it that way.
    */
    return before != 0 && before < after;
}

void Q3TableHeader::sectionLabelChanged(int section)
{
    emit sectionSizeChanged(section);

    // this does not really belong here
    if (orientation() == Horizontal) {
        int h = sizeHint().height();
        if (h != height() && mayOverwriteMargin(table->topMargin(), h))
            table->setTopMargin(h);
    } else {
        int w = sizeHint().width();
        if (w != width() && mayOverwriteMargin((QApplication::reverseLayout() ? table->rightMargin() : table->leftMargin()), w))
            table->setLeftMargin(w);
    }
}

/*! \reimp */
void Q3TableHeader::mouseReleaseEvent(QMouseEvent *e)
{
    if (e->button() != LeftButton)
        return;
    autoScrollTimer->stop();
    mousePressed = false;
    setCaching(false);
    Q3Header::mouseReleaseEvent(e);
#ifndef NO_LINE_WIDGET
    line1->hide();
    line2->hide();
#else
    if (d->oldLinePos >= 0)
        if (orientation() == Horizontal)
            table->updateContents(d->oldLinePos, table->contentsY(),
                                   1, table->visibleHeight());
        else
            table->updateContents( table->contentsX(), d->oldLinePos,
                                    table->visibleWidth(), 1);
    d->oldLinePos = -1;
#endif
    if (resizedSection != -1) {
        emit sectionSizeChanged(resizedSection);
        updateStretches();
    }

    //Make sure all newly selected sections are painted one last time
    QRect selectedRects;
    for (int i = 0; i < count(); i++) {
        if(sectionState(i) == Selected)
            selectedRects |= sRect(i);
    }
    if(!selectedRects.isNull())
        repaint(selectedRects);
}

/*! \reimp
*/

void Q3TableHeader::mouseDoubleClickEvent(QMouseEvent *e)
{
    if (e->button() != LeftButton)
        return;
    if (isResizing) {
        int p = real_pos(e->pos(), orientation()) + offset();
        int section = sectionAt(p);
        if (section == -1)
            return;
        section--;
        if (p >= sectionPos(count() - 1) + sectionSize(count() - 1))
            ++section;
        while (sectionSize(section) == 0)
            section--;
        if (section < 0)
            return;
        int oldSize = sectionSize(section);
        if (orientation() == Horizontal) {
            table->adjustColumn(section);
            int newSize = sectionSize(section);
            if (oldSize != newSize)
                emit sizeChange(section, oldSize, newSize);
            for (int i = 0; i < table->numCols(); ++i) {
                if (table->isColumnSelected(i) && sectionSize(i) != 0)
                    table->adjustColumn(i);
            }
        } else {
            table->adjustRow(section);
            int newSize = sectionSize(section);
            if (oldSize != newSize)
                emit sizeChange(section, oldSize, newSize);
            for (int i = 0; i < table->numRows(); ++i) {
                if (table->isRowSelected(i)  && sectionSize(i) != 0)
                    table->adjustRow(i);
            }
        }
    }
}

/*! \reimp
*/

void Q3TableHeader::resizeEvent(QResizeEvent *e)
{
    stretchTimer->stop();
    widgetStretchTimer->stop();
    Q3Header::resizeEvent(e);
    if (numStretches == 0)
        return;
    stretchTimer->start(0, true);
}

void Q3TableHeader::updateStretches()
{
    if (numStretches == 0)
        return;

    int dim = orientation() == Horizontal ? width() : height();
    if (sectionPos(count() - 1) + sectionSize(count() - 1) == dim)
        return;
    int i;
    int pd = dim - (sectionPos(count() - 1)
                     + sectionSize(count() - 1));
    bool block = signalsBlocked();
    blockSignals(true);
    for (i = 0; i < (int)stretchable.count(); ++i) {
        if (!stretchable[i] ||
             (stretchable[i] && table->d->hiddenCols[i]))
            continue;
        pd += sectionSize(i);
    }
    pd /= numStretches;
    for (i = 0; i < (int)stretchable.count(); ++i) {
        if (!stretchable[i] ||
             (stretchable[i] && table->d->hiddenCols[i]))
            continue;
        if (i == (int)stretchable.count() - 1 &&
             sectionPos(i) + pd < dim)
            pd = dim - sectionPos(i);
        resizeSection(i, QMAX(20, pd));
    }
    blockSignals(block);
    table->repaintContents(false);
    widgetStretchTimer->start(100, true);
}

void Q3TableHeader::updateWidgetStretches()
{
    QSize s = table->tableSize();
    table->resizeContents(s.width(), s.height());
    for (int i = 0; i < table->numCols(); ++i)
        table->updateColWidgets(i);
}

void Q3TableHeader::updateSelections()
{
    if (table->selectionMode() == Q3Table::NoSelection ||
         (isRowSelection(table->selectionMode()) && orientation() != Vertical ))
        return;
    int a = sectionAt(startPos);
    int b = sectionAt(endPos);
    int start = QMIN(a, b);
    int end = QMAX(a, b);
    register int *s = states.data();
    for (int i = 0; i < count(); ++i) {
        if (i < start || i > end)
            *s = oldStates.data()[ i ];
        else
            *s = Selected;
        ++s;
    }
    repaint(false);

    if (table->currentSel) {
        Q3TableSelection oldSelection = *table->currentSel;
        if (orientation() == Vertical)
            table->currentSel->expandTo(b, table->horizontalHeader()->count() - 1);
        else
            table->currentSel->expandTo(table->verticalHeader()->count() - 1, b);
        table->repaintSelections(&oldSelection, table->currentSel,
                                  orientation() == Horizontal,
                                  orientation() == Vertical);
    }
    emit table->selectionChanged();
}

void Q3TableHeader::saveStates()
{
    oldStates.resize(count());
    register int *s = states.data();
    register int *s2 = oldStates.data();
    for (int i = 0; i < count(); ++i) {
        *s2 = *s;
        ++s2;
        ++s;
    }
}

void Q3TableHeader::doAutoScroll()
{
    QPoint pos = mapFromGlobal(QCursor::pos());
    int p = real_pos(pos, orientation()) + offset();
    if (sectionAt(p) != -1)
        endPos = p;
    if (orientation() == Horizontal)
        table->ensureVisible(endPos, table->contentsY());
    else
        table->ensureVisible(table->contentsX(), endPos);
    updateSelections();
    autoScrollTimer->start(100, true);
}

void Q3TableHeader::sectionWidthChanged(int col, int, int)
{
    resizedSection = col;
    if (orientation() == Horizontal) {
#ifndef NO_LINE_WIDGET
        table->moveChild(line1, Q3Header::sectionPos(col) - 1,
                          table->contentsY());
        line1->resize(1, table->visibleHeight());
        line1->show();
        line1->raise();
        table->moveChild(line2,
                          Q3Header::sectionPos(col) + Q3Header::sectionSize(col) - 1,
                          table->contentsY());
        line2->resize(1, table->visibleHeight());
        line2->show();
        line2->raise();
#else
        QPainter p(table->viewport());
        int lx = Q3Header::sectionPos(col) + Q3Header::sectionSize(col) - 1;
        int ly = table->contentsY();

        if (lx != d->oldLinePos) {
            QPoint pt = table->contentsToViewport(QPoint(lx, ly));
            p.drawLine(pt.x(), pt.y()+1,
                        pt.x(), pt.y()+ table->visibleHeight());
            if (d->oldLinePos >= 0)
                table->repaintContents(d->oldLinePos, table->contentsY(),
                                       1, table->visibleHeight());

            d->oldLinePos = lx;
        }
#endif
    } else {
#ifndef NO_LINE_WIDGET
        table->moveChild(line1, table->contentsX(),
                          Q3Header::sectionPos(col) - 1);
        line1->resize(table->visibleWidth(), 1);
        line1->show();
        line1->raise();
        table->moveChild(line2, table->contentsX(),
                          Q3Header::sectionPos(col) + Q3Header::sectionSize(col) - 1);
        line2->resize(table->visibleWidth(), 1);
        line2->show();
        line2->raise();

#else
        QPainter p(table->viewport());
        int lx = table->contentsX();
        int ly = Q3Header::sectionPos(col) + Q3Header::sectionSize(col) - 1;

        if (ly != d->oldLinePos) {
            QPoint pt = table->contentsToViewport(QPoint(lx, ly));
            p.drawLine(pt.x()+1, pt.y(),
                        pt.x() + table->visibleWidth(), pt.y());
            if (d->oldLinePos >= 0)
                table->repaintContents( table->contentsX(), d->oldLinePos,
                                        table->visibleWidth(), 1);
            d->oldLinePos = ly;
        }

#endif
    }
}

/*!
    \reimp

    Returns the size of section \a section in pixels or -1 if \a
    section is out of range.
*/

int Q3TableHeader::sectionSize(int section) const
{
    if (count() <= 0 || section < 0 || section >= count())
        return -1;
    if (caching && section < (int)sectionSizes.count())
         return sectionSizes[ section ];
    return Q3Header::sectionSize(section);
}

/*!
    \reimp

    Returns the start position of section \a section in pixels or -1
    if \a section is out of range.

    \sa sectionAt()
*/

int Q3TableHeader::sectionPos(int section) const
{
    if (count() <= 0 || section < 0 || section >= count())
        return -1;
    if (caching && section < (int)sectionPoses.count())
        return sectionPoses[ section ];
    return Q3Header::sectionPos(section);
}

/*!
    \reimp

    Returns the number of the section at index position \a pos or -1
    if there is no section at the position given.

    \sa sectionPos()
*/

int Q3TableHeader::sectionAt(int pos) const
{
    if (!caching || sectionSizes.count() <= 0 || sectionPoses.count() <= 0)
        return Q3Header::sectionAt(pos);
    if (count() <= 0 || pos > sectionPoses[ count() - 1 ] + sectionSizes[ count() - 1 ])
        return -1;
    int l = 0;
    int r = count() - 1;
    int i = ((l+r+1) / 2);
    while (r - l) {
        if (sectionPoses[i] > pos)
            r = i -1;
        else
            l = i;
        i = ((l+r+1) / 2);
    }
    if (sectionPoses[i] <= pos &&
         pos <= sectionPoses[i] + sectionSizes[ mapToSection(i) ])
        return mapToSection(i);
    return -1;
}

void Q3TableHeader::updateCache()
{
    sectionPoses.resize(count());
    sectionSizes.resize(count());
    if (!caching)
        return;
    for (int i = 0; i < count(); ++i) {
        sectionSizes[ i ] = Q3Header::sectionSize(i);
        sectionPoses[ i ] = Q3Header::sectionPos(i);
    }
}

void Q3TableHeader::setCaching(bool b)
{
    if (caching == b)
        return;
    caching = b;
    sectionPoses.resize(count());
    sectionSizes.resize(count());
    if (b) {
        for (int i = 0; i < count(); ++i) {
            sectionSizes[ i ] = Q3Header::sectionSize(i);
            sectionPoses[ i ] = Q3Header::sectionPos(i);
        }
    }
}

/*!
    If \a b is true, section \a s is stretchable; otherwise the
    section is not stretchable.

    \sa isSectionStretchable()
*/

void Q3TableHeader::setSectionStretchable(int s, bool b)
{
    if (stretchable[ s ] == b)
        return;
    stretchable[ s ] = b;
    if (b)
        numStretches++;
    else
        numStretches--;
}

/*!
    Returns true if section \a s is stretcheable; otherwise returns
    false.

    \sa setSectionStretchable()
*/

bool Q3TableHeader::isSectionStretchable(int s) const
{
    return stretchable[ s ];
}

void Q3TableHeader::swapSections(int oldIdx, int newIdx, bool swapTable)
{
    extern bool qt_qheader_label_return_null_strings; // qheader.cpp
    qt_qheader_label_return_null_strings = true;

    QIconSet oldIconSet, newIconSet;
    if (iconSet(oldIdx))
        oldIconSet = *iconSet(oldIdx);
    if (iconSet(newIdx))
        newIconSet = *iconSet(newIdx);
    QString oldLabel = label(oldIdx);
    QString newLabel = label(newIdx);
    bool sectionsHasContent = !(oldIconSet.isNull() && newIconSet.isNull()
                            && oldLabel.isNull() && newLabel.isNull());
    if (sectionsHasContent) {
        Q3HeaderData *data = static_cast<Q3Header*>(this)->d;
        bool oldNullLabel = qt_get_null_label_bit(data, oldIdx);
        bool newNullLabel = qt_get_null_label_bit(data, newIdx);
        setLabel(oldIdx, newIconSet, newLabel);
        setLabel(newIdx, oldIconSet, oldLabel);
        qt_set_null_label_bit(data, oldIdx, newNullLabel);
        qt_set_null_label_bit(data, newIdx, oldNullLabel);
    }

    qt_qheader_label_return_null_strings = false;

    int w1 = sectionSize(oldIdx);
    int w2 = sectionSize(newIdx);
    if (w1 != w2) {
        resizeSection(oldIdx, w2);
        resizeSection(newIdx, w1);
    }

    if (!swapTable)
        return;
    if (orientation() == Horizontal)
        table->swapColumns(oldIdx, newIdx);
    else
        table->swapRows(oldIdx, newIdx);
}

void Q3TableHeader::indexChanged(int sec, int oldIdx, int newIdx)
{
    newIdx = mapToIndex(sec);
    if (oldIdx > newIdx)
        moveSection(sec, oldIdx + 1);
    else
        moveSection(sec, oldIdx);

    if (oldIdx < newIdx) {
        while (oldIdx < newIdx) {
            swapSections(oldIdx, oldIdx + 1);
            oldIdx++;
        }
    } else {
        while (oldIdx > newIdx) {
            swapSections(oldIdx - 1, oldIdx);
            oldIdx--;
        }
    }

    table->repaintContents(table->contentsX(), table->contentsY(),
                            table->visibleWidth(), table->visibleHeight());
}

void Q3TableHeader::setLabels(const QStringList & labels)
{
    int i = 0;
    const int c = QMIN(count(), (int)labels.count());
    bool updates = updatesEnabled();
    if (updates)
        setUpdatesEnabled(false);
    for (QStringList::ConstIterator it = labels.begin(); i < c; ++i, ++it) {
        if (i == c - 1) {
            if (updates)
                setUpdatesEnabled(true);
            setLabel(i, *it);
        } else {
            Q3Header::setLabel(i, *it);
            emit sectionSizeChanged(i);
        }
    }
}

QT_END_NAMESPACE

#include "q3table.moc"
