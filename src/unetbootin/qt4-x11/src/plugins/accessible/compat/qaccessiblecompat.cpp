/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the plugins of the Qt Toolkit.
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

#include "qaccessiblecompat.h"
#include "q3widgetstack.h"

#include <q3listview.h>
#include <q3textedit.h>
#include <q3iconview.h>
#include <q3listbox.h>

QT_BEGIN_NAMESPACE

/*!
\fn Q3AccessibleScrollView::Q3AccessibleScrollView(QWidget* widget, Role role)

Constructs a Q3AccessibleScrollView object for a \a widget.
The \a role is propagated to the QAccessibleWidget constructor.
*/
Q3AccessibleScrollView::Q3AccessibleScrollView(QWidget *w, Role role)
: QAccessibleWidget(w, role)
{
}

/*!
  Returns the ID of the item at viewport position \a x, \a y.
*/
int Q3AccessibleScrollView::itemAt(int /*x*/, int /*y*/) const
{
    return 0;
}

/*!
  Returns the location in viewport coordinates of the item with ID \a
  item.
*/
QRect Q3AccessibleScrollView::itemRect(int /*item*/) const
{
    return QRect();
}

/*!
  Returns the number of items in the scroll view.
*/
int Q3AccessibleScrollView::itemCount() const
{
    return 0;
}

/*!
  \class QAccessibleListView qaccessiblewidget.h
  \brief The QAccessibleListView class implements the QAccessibleInterface for list views.
  \internal
*/

static Q3ListViewItem *findLVItem(Q3ListView* listView, int child)
{
    int id = 1;
    Q3ListViewItemIterator it(listView);
    Q3ListViewItem *item = it.current();
    while (item && id < child) {
        ++it;
        ++id;
        item = it.current();
    }
    return item;
}

/*!
  \fn QAccessibleListView::QAccessibleListView(QWidget* widget)

  Constructs a QAccessibleListView object for a \a widget.
*/
QAccessibleListView::QAccessibleListView(QWidget *o)
    : Q3AccessibleScrollView(o, Tree)
{
}

/*! Returns the list view. */
Q3ListView *QAccessibleListView::listView() const
{
    Q_ASSERT(widget()->inherits("Q3ListView"));
    return (Q3ListView*)widget();
}

/*! \reimp */
int QAccessibleListView::itemAt(int x, int y) const
{
    Q3ListViewItem *item = listView()->itemAt(QPoint(x, y));
    if (!item)
        return 0;

    Q3ListViewItemIterator it(listView());
    int c = 1;
    while (it.current()) {
        if (it.current() == item)
            return c;
        ++c;
        ++it;
    }
    return 0;
}

/*! \reimp */
QRect QAccessibleListView::itemRect(int child) const
{
    Q3ListViewItem *item = findLVItem(listView(), child);
    if (!item)
        return QRect();
    return listView()->itemRect(item);
}

/*! \reimp */
int QAccessibleListView::itemCount() const
{
    Q3ListViewItemIterator it(listView());
    int c = 0;
    while (it.current()) {
        ++c;
        ++it;
    }

    return c;
}

/*! \reimp */
QString QAccessibleListView::text(Text t, int child) const
{
    if (!child || t != Name)
        return Q3AccessibleScrollView::text(t, child);

    Q3ListViewItem *item = findLVItem(listView(), child);
    if (!item)
        return QString();
    return item->text(0);
}

/*! \reimp */
QAccessible::Role QAccessibleListView::role(int child) const
{
    if (!child)
        return Q3AccessibleScrollView::role(child);
    return TreeItem;
}

/*! \reimp */
QAccessible::State QAccessibleListView::state(int child) const
{
    State state = Q3AccessibleScrollView::state(child);
    Q3ListViewItem *item;
    if (!child || !(item = findLVItem(listView(), child)))
        return state;

    if (item->isSelectable()) {
        if (listView()->selectionMode() == Q3ListView::Multi)
            state |= MultiSelectable;
        else if (listView()->selectionMode() == Q3ListView::Extended)
            state |= ExtSelectable;
        else if (listView()->selectionMode() == Q3ListView::Single)
            state |= Selectable;
        if (item->isSelected())
            state |= Selected;
    }
    if (listView()->focusPolicy() != Qt::NoFocus) {
        state |= Focusable;
        if (item == listView()->currentItem())
            state |= Focused;
    }
    if (item->childCount()) {
        if (item->isOpen())
            state |= Expanded;
        else
            state |= Collapsed;
    }
    if (!listView()->itemRect(item).isValid())
        state |= Invisible;

    if (item->rtti() == Q3CheckListItem::RTTI) {
        if (((Q3CheckListItem*)item)->isOn())
            state|=Checked;
    }
    return state;
}

/* \reimp
QAccessibleInterface *QAccessibleListView::focusChild(int *child) const
{
    Q3ListViewItem *item = listView()->currentItem();
    if (!item)
        return 0;

    Q3ListViewItemIterator it(listView());
    int c = 1;
    while (it.current()) {
        if (it.current() == item) {
            *child = c;
            return (QAccessibleInterface*)this;
        }
        ++c;
        ++it;
    }
    return 0;
}
*/
/* \reimp
bool QAccessibleListView::setFocus(int child)
{
    bool res = Q3AccessibleScrollView::setFocus(0);
    if (!child || !res)
        return res;

    Q3ListViewItem *item = findLVItem(listView(), child);
    if (!item)
        return false;
    listView()->setCurrentItem(item);
    return true;
}*/

/*! \internal */
bool QAccessibleListView::setSelected(int child, bool on, bool extend)
{
    if (!child || (extend &&
        listView()->selectionMode() != Q3ListView::Extended &&
        listView()->selectionMode() != Q3ListView::Multi))
        return false;

    Q3ListViewItem *item = findLVItem(listView(), child);
    if (!item)
        return false;
    if (!extend) {
        listView()->setSelected(item, on);
    } else {
        Q3ListViewItem *current = listView()->currentItem();
        if (!current)
            return false;
        bool down = item->itemPos() > current->itemPos();
        Q3ListViewItemIterator it(current);
        while (it.current()) {
            listView()->setSelected(it.current(), on);
            if (it.current() == item)
                break;
            if (down)
                ++it;
            else
                --it;
        }
    }
    return true;
}

/*! \internal */
void QAccessibleListView::clearSelection()
{
    listView()->clearSelection();
}

/*! \internal */
QVector<int> QAccessibleListView::selection() const
{
    QVector<int> array;
    uint size = 0;
    int id = 1;
    array.resize(size);
    Q3ListViewItemIterator it(listView());
    while (it.current()) {
        if (it.current()->isSelected()) {
            ++size;
            array.resize(size);
            array[(int)size-1] = id;
        }
        ++it;
        ++id;
    }
    return array;
}

/*!
  \class QAccessibleIconView qaccessiblewidget.h
  \brief The QAccessibleIconView class implements the QAccessibleInterface for icon views.
  \internal
*/

static Q3IconViewItem *findIVItem(Q3IconView *iconView, int child)
{
    int id = 1;
    Q3IconViewItem *item = iconView->firstItem();
    while (item && id < child) {
        item = item->nextItem();
        ++id;
    }

    return item;
}

/*!
  \fn QAccessibleIconView::QAccessibleIconView(QWidget* widget)

  Constructs a QAccessibleIconView object for a \a widget.
*/
QAccessibleIconView::QAccessibleIconView(QWidget *o)
    : Q3AccessibleScrollView(o, List)
{
    Q_ASSERT(widget()->inherits("Q3IconView"));
}

/*! Returns the icon view. */
Q3IconView *QAccessibleIconView::iconView() const
{
    return (Q3IconView*)widget();
}

/*! \internal */
int QAccessibleIconView::itemAt(int x, int y) const
{
    Q3IconViewItem *item = iconView()->findItem(QPoint(x, y));
    return iconView()->index(item) + 1;
}

/*! \internal */
QRect QAccessibleIconView::itemRect(int child) const
{
    Q3IconViewItem *item = findIVItem(iconView(), child);

    if (!item)
        return QRect();
    return item->rect();
}

/*! \internal */
int QAccessibleIconView::itemCount() const
{
    return iconView()->count();
}

/*! \internal */
QString QAccessibleIconView::text(Text t, int child) const
{
    if (!child || t != Name)
        return Q3AccessibleScrollView::text(t, child);

    Q3IconViewItem *item = findIVItem(iconView(), child);
    if (!item)
        return QString();
    return item->text();
}

/*! \internal */
QAccessible::Role QAccessibleIconView::role(int child) const
{
    if (!child)
        return Q3AccessibleScrollView::role(child);
    return ListItem;
}

/*! \internal */
QAccessible::State QAccessibleIconView::state(int child) const
{
    State state = Q3AccessibleScrollView::state(child);
    Q3IconViewItem *item;
    if (!child || !(item = findIVItem(iconView(), child)))
        return state;

    if (item->isSelectable()) {
        if (iconView()->selectionMode() == Q3IconView::Multi)
            state |= MultiSelectable;
        else if (iconView()->selectionMode() == Q3IconView::Extended)
            state |= ExtSelectable;
        else if (iconView()->selectionMode() == Q3IconView::Single)
            state |= Selectable;
        if (item->isSelected())
            state |= Selected;
    }
    if (iconView()->itemsMovable())
        state |= Movable;
    if (iconView()->focusPolicy() != Qt::NoFocus) {
        state |= Focusable;
        if (item == iconView()->currentItem())
            state |= Focused;
    }

    return state;
}

/* \reimp
QAccessibleInterface *QAccessibleIconView::focusChild(int *child) const
{
    Q3IconViewItem *item = iconView()->currentItem();
    if (!item)
        return 0;

    *child = iconView()->index(item);
    return (QAccessibleInterface*)this;
}
*/
/* \reimp
bool QAccessibleIconView::setFocus(int child)
{
    bool res = Q3AccessibleScrollView::setFocus(0);
    if (!child || !res)
        return res;

    Q3IconViewItem *item = findIVItem(iconView(), child);
    if (!item)
        return false;
    iconView()->setCurrentItem(item);
    return true;
}*/

/*! \internal */
bool QAccessibleIconView::setSelected(int child, bool on, bool extend)
{
    if (!child || (extend &&
        iconView()->selectionMode() != Q3IconView::Extended &&
        iconView()->selectionMode() != Q3IconView::Multi))
        return false;

    Q3IconViewItem *item = findIVItem(iconView(), child);
    if (!item)
        return false;
    if (!extend) {
        iconView()->setSelected(item, on, true);
    } else {
        Q3IconViewItem *current = iconView()->currentItem();
        if (!current)
            return false;
        bool down = false;
        Q3IconViewItem *temp = current;
        while ((temp = temp->nextItem())) {
            if (temp == item) {
                down = true;
                break;
            }
        }
        temp = current;
        if (down) {
            while ((temp = temp->nextItem())) {
                iconView()->setSelected(temp, on, true);
                if (temp == item)
                    break;
            }
        } else {
            while ((temp = temp->prevItem())) {
                iconView()->setSelected(temp, on, true);
                if (temp == item)
                    break;
            }
        }
    }
    return true;
}

/*! \internal */
void QAccessibleIconView::clearSelection()
{
    iconView()->clearSelection();
}

/*! \internal */
QVector<int> QAccessibleIconView::selection() const
{
    QVector<int> array;
    uint size = 0;
    int id = 1;
    array.resize(iconView()->count());
    Q3IconViewItem *item = iconView()->firstItem();
    while (item) {
        if (item->isSelected()) {
            ++size;
            array[(int)size-1] = id;
        }
        item = item->nextItem();
        ++id;
    }
    array.resize(size);
    return array;
}


/*!
  \class Q3AccessibleTextEdit qaccessiblewidget.h
  \brief The Q3AccessibleTextEdit class implements the QAccessibleInterface for richtext editors.
  \internal
*/

/*!
  \fn Q3AccessibleTextEdit::Q3AccessibleTextEdit(QWidget* widget)

  Constructs a Q3AccessibleTextEdit object for a \a widget.
*/
Q3AccessibleTextEdit::Q3AccessibleTextEdit(QWidget *o)
: Q3AccessibleScrollView(o, Pane)
{
    Q_ASSERT(widget()->inherits("Q3TextEdit"));
}

/*! Returns the text edit. */
Q3TextEdit *Q3AccessibleTextEdit::textEdit() const
{

    return (Q3TextEdit*)widget();
}

/*! \reimp */
int Q3AccessibleTextEdit::itemAt(int x, int y) const
{
    int p;
    QPoint cp = textEdit()->viewportToContents(QPoint(x,y));
    textEdit()->charAt(cp , &p);
    return p + 1;
}

/*! \reimp */
QRect Q3AccessibleTextEdit::itemRect(int item) const
{
    QRect rect = textEdit()->paragraphRect(item - 1);
    if (!rect.isValid())
        return QRect();
    QPoint ntl = textEdit()->contentsToViewport(QPoint(rect.x(), rect.y()));
    return QRect(ntl.x(), ntl.y(), rect.width(), rect.height());
}

/*! \reimp */
int Q3AccessibleTextEdit::itemCount() const
{
    return textEdit()->paragraphs();
}

/*! \reimp */
QString Q3AccessibleTextEdit::text(Text t, int child) const
{
    if (t == Name && child > 0)
        return textEdit()->text(child - 1);
    if (t == Value) {
        if (child > 0)
            return textEdit()->text(child - 1);
        else
            return textEdit()->text();
    }

    return Q3AccessibleScrollView::text(t, child);
}

/*! \reimp */
void Q3AccessibleTextEdit::setText(Text t, int control, const QString &text)
{
    if (t != Value || control) {
        Q3AccessibleScrollView::setText(t, control, text);
        return;
    }
    textEdit()->setText(text);
}

/*! \reimp */
QAccessible::Role Q3AccessibleTextEdit::role(int child) const
{
    if (child)
        return EditableText;
    return Q3AccessibleScrollView::role(child);
}

/*!
  \class QAccessibleWidgetStack qaccessible.h
  \brief The QAccessibleWidgetStack class implements the QAccessibleInterface for widget stacks.

  \ingroup accessibility
  \internal
*/

/*!
  \fn QAccessibleWidgetStack::QAccessibleWidgetStack(QWidget* widget)

  Creates a QAccessibleWidgetStack object for a \a widget.
*/
QAccessibleWidgetStack::QAccessibleWidgetStack(QWidget *w)
: QAccessibleWidget(w, LayeredPane)
{
    Q_ASSERT(widgetStack());
    setDescription(QLatin1String("This is a widgetstack"));
}

/*! Returns the widget stack. */
Q3WidgetStack *QAccessibleWidgetStack::widgetStack() const
{
    return qobject_cast<Q3WidgetStack*>(object());
}

/*! \reimp */
int QAccessibleWidgetStack::childCount() const
{
    // a widget stack has always only one accessible widget
    return 1;
}

/*! \reimp */
int QAccessibleWidgetStack::indexOfChild(const QAccessibleInterface *child) const
{
    QObject *childObject = child ? child->object() : 0;
    if (childObject != widgetStack()->visibleWidget())
        return -1;
    return 1;
}

/*! \reimp */
int QAccessibleWidgetStack::childAt(int, int) const
{
    QWidget *curPage = widgetStack()->visibleWidget();
    if (!curPage)
        return 0;
    return 1;
}

/*! \reimp */
int QAccessibleWidgetStack::navigate(RelationFlag rel, int entry,
                                     QAccessibleInterface **target) const
{
    *target = 0;
    QObject *targetObject = 0;
    switch (rel) {
    // Hierarchical
    case Child:
        if (entry != 1)
            return -1;
        targetObject = widgetStack()->visibleWidget();
        break;
    default:
        return QAccessibleWidget::navigate(rel, entry, target);
    }
    *target = QAccessible::queryAccessibleInterface(targetObject);
    return *target ? 0 : -1;
}

/*!
  \class QAccessibleListBox qaccessiblewidget.h
  \brief The QAccessibleListBox class implements the QAccessibleInterface for list boxes.

  \ingroup accessibility
  \internal
*/

/*!
  \fn QAccessibleListBox::QAccessibleListBox(QWidget* widget)

  Constructs a QAccessibleListBox object for a \a widget.
*/
QAccessibleListBox::QAccessibleListBox(QWidget *o)
    : Q3AccessibleScrollView(o, List)
{
    Q_ASSERT(widget()->inherits("Q3ListBox"));
}

/*! Returns the list box. */
Q3ListBox *QAccessibleListBox::listBox() const
{
    return (Q3ListBox*)widget();
}

/*! \reimp */
int QAccessibleListBox::itemAt(int x, int y) const
{
    Q3ListBoxItem *item = listBox()->itemAt(QPoint(x, y));
    return listBox()->index(item) + 1;
}

/*! \reimp */
QRect QAccessibleListBox::itemRect(int item) const
{
    return listBox()->itemRect(listBox()->item(item-1));
}

/*! \reimp */
int QAccessibleListBox::itemCount() const
{
    return listBox()->count();
}

/*! \reimp */
QString QAccessibleListBox::text(Text t, int child) const
{
    if (!child || t != Name)
        return Q3AccessibleScrollView::text(t, child);

    Q3ListBoxItem *item = listBox()->item(child - 1);
    if (item)
        return item->text();
    return QString();
}

/*! \reimp */
QAccessible::Role QAccessibleListBox::role(int child) const
{
    if (!child)
        return Q3AccessibleScrollView::role(child);
    return ListItem;
}

/*! \reimp */
QAccessible::State QAccessibleListBox::state(int child) const
{
    State state = Q3AccessibleScrollView::state(child);
    Q3ListBoxItem *item;
    if (!child || !(item = listBox()->item(child - 1)))
        return state;

    if (item->isSelectable()) {
        if (listBox()->selectionMode() == Q3ListBox::Multi)
            state |= MultiSelectable;
        else if (listBox()->selectionMode() == Q3ListBox::Extended)
            state |= ExtSelectable;
        else if (listBox()->selectionMode() == Q3ListBox::Single)
            state |= Selectable;
        if (item->isSelected())
            state |= Selected;
    }
    if (listBox()->focusPolicy() != Qt::NoFocus) {
        state |= Focusable;
        if (item->isCurrent())
            state |= Focused;
    }
    if (!listBox()->itemVisible(item))
        state |= Invisible;

    return state;
}

/* \reimp
bool QAccessibleListBox::setFocus(int child)
{
    bool res = Q3AccessibleScrollView::setFocus(0);
    if (!child || !res)
        return res;

    Q3ListBoxItem *item = listBox()->item(child -1);
    if (!item)
        return false;
    listBox()->setCurrentItem(item);
    return true;
}*/

/*!
    Selects the item with index \a child if \a on is true; otherwise
    unselects it. If \a extend is true and the selection mode is not
    \c Single and there is an existing selection, the selection is
    extended to include all the items from the existing selection up
    to and including the item with index \a child. Returns true if a
    selection was made or extended; otherwise returns false.

    \sa selection() clearSelection()
*/
bool QAccessibleListBox::setSelected(int child, bool on, bool extend)
{
    if (!child || (extend &&
        listBox()->selectionMode() != Q3ListBox::Extended &&
        listBox()->selectionMode() != Q3ListBox::Multi))
        return false;

    Q3ListBoxItem *item = listBox()->item(child -1);
    if (!item)
        return false;
    if (!extend) {
        listBox()->setSelected(item, on);
    } else {
        int current = listBox()->currentItem();
        bool down = child > current;
        for (int i = current; i != child;) {
            down ? i++ : i--;
            listBox()->setSelected(i, on);
        }

    }
    return true;
}

/*!
    Sets all the items in the list box to be unselected.

    \sa setSelected() selection()
*/
void QAccessibleListBox::clearSelection()
{
    listBox()->clearSelection();
}

/*!
    Returns a (possibly empty) list of indexes of the items selected
    in the list box.

    \sa setSelected() clearSelection()
*/
QVector<int> QAccessibleListBox::selection() const
{
    QVector<int> array;
    uint size = 0;
    const uint c = listBox()->count();
    array.resize(c);
    for (uint i = 0; i < c; ++i) {
        if (listBox()->isSelected(i)) {
            ++size;
            array[(int)size-1] = i+1;
        }
    }
    array.resize(size);
    return array;
}

QT_END_NAMESPACE
