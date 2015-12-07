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

#ifndef Q3LISTBOX_H
#define Q3LISTBOX_H

#include <Qt3Support/q3scrollview.h>
#include <QtGui/qpixmap.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Qt3SupportLight)

#ifndef QT_NO_LISTBOX

class Q3ListBoxPrivate;
class Q3ListBoxItem;
class QString;
class QStringList;

class Q_COMPAT_EXPORT Q3ListBox : public Q3ScrollView
{
    friend class Q3ListBoxItem;
    friend class Q3ListBoxPrivate;

    Q_OBJECT
    Q_ENUMS(SelectionMode LayoutMode)
    Q_PROPERTY(uint count READ count)
    Q_PROPERTY(int numItemsVisible READ numItemsVisible)
    Q_PROPERTY(int currentItem READ currentItem WRITE setCurrentItem USER true)
    Q_PROPERTY(QString currentText READ currentText)
    Q_PROPERTY(int topItem READ topItem WRITE setTopItem DESIGNABLE false)
    Q_PROPERTY(SelectionMode selectionMode READ selectionMode WRITE setSelectionMode)
    Q_PROPERTY(bool multiSelection READ isMultiSelection WRITE setMultiSelection DESIGNABLE false)
    Q_PROPERTY(LayoutMode columnMode READ columnMode WRITE setColumnMode)
    Q_PROPERTY(LayoutMode rowMode READ rowMode WRITE setRowMode)
    Q_PROPERTY(int numColumns READ numColumns)
    Q_PROPERTY(int numRows READ numRows)
    Q_PROPERTY(bool variableWidth READ variableWidth WRITE setVariableWidth)
    Q_PROPERTY(bool variableHeight READ variableHeight WRITE setVariableHeight)

public:
    Q3ListBox(QWidget* parent=0, const char* name=0, Qt::WindowFlags f=0 );
    ~Q3ListBox();

    uint count() const;

    void insertStringList(const QStringList&, int index=-1);
// ### fix before Qt 4.0
#if 0
    void insertStrList(const QStrList *, int index=-1);
    void insertStrList(const QStrList &, int index=-1);
#endif
    void insertStrList(const char **,
                        int numStrings=-1, int index=-1);

    void insertItem(const Q3ListBoxItem *, int index=-1);
    void insertItem(const Q3ListBoxItem *, const Q3ListBoxItem *after);
    void insertItem(const QString &text, int index=-1);
    void insertItem(const QPixmap &pixmap, int index=-1);
    void insertItem(const QPixmap &pixmap, const QString &text, int index=-1);

    void removeItem(int index);

    QString text(int index)        const;
    const QPixmap *pixmap(int index)        const;

    void changeItem(const Q3ListBoxItem *, int index);
    void changeItem(const QString &text, int index);
    void changeItem(const QPixmap &pixmap, int index);
    void changeItem(const QPixmap &pixmap, const QString &text, int index);

    void takeItem(const Q3ListBoxItem *);

    int numItemsVisible() const;

    int currentItem() const;
    QString currentText() const { return text(currentItem()); }
    virtual void setCurrentItem(int index);
    virtual void setCurrentItem(Q3ListBoxItem *);
    void centerCurrentItem() { ensureCurrentVisible(); }
    int topItem() const;
    virtual void setTopItem(int index);
    virtual void setBottomItem(int index);

    long maxItemWidth() const;

    enum SelectionMode { Single, Multi, Extended, NoSelection };
    virtual void setSelectionMode(SelectionMode);
    SelectionMode selectionMode() const;

    void setMultiSelection(bool multi);
    bool isMultiSelection() const;

    QVariant inputMethodQuery(Qt::InputMethodQuery query) const;

    virtual void setSelected(Q3ListBoxItem *, bool);
    void setSelected(int, bool);
    bool isSelected(int) const;
    bool isSelected(const Q3ListBoxItem *) const;
    Q3ListBoxItem* selectedItem() const;

    QSize sizeHint() const;
    QSize        minimumSizeHint() const;

    Q3ListBoxItem *item(int index) const;
    int index(const Q3ListBoxItem *) const;

    enum StringComparisonMode {
        CaseSensitive   = 0x00001, // 0 0001
        BeginsWith      = 0x00002, // 0 0010
        EndsWith        = 0x00004, // 0 0100
        Contains        = 0x00008, // 0 1000
        ExactMatch      = 0x00010  // 1 0000
    };
    typedef uint ComparisonFlags;
    Q3ListBoxItem *findItem(const QString &text, ComparisonFlags compare = BeginsWith) const;

    void triggerUpdate(bool doLayout);

    bool itemVisible(int index);
    bool itemVisible(const Q3ListBoxItem *);

    enum LayoutMode { FixedNumber,
                      FitToWidth, FitToHeight = FitToWidth,
                      Variable };
    virtual void setColumnMode(LayoutMode);
    virtual void setColumnMode(int);
    virtual void setRowMode(LayoutMode);
    virtual void setRowMode(int);

    LayoutMode columnMode() const;
    LayoutMode rowMode() const;

    int numColumns() const;
    int numRows() const;

    bool variableWidth() const;
    virtual void setVariableWidth(bool);

    bool variableHeight() const;
    virtual void setVariableHeight(bool);

    void viewportPaintEvent(QPaintEvent *);

    bool dragSelect() const { return true; }
    void setDragSelect(bool) {}
    bool autoScroll() const { return true; }
    void setAutoScroll(bool) {}
    bool autoScrollBar() const { return vScrollBarMode() == Auto; }
    void setAutoScrollBar(bool enable) { setVScrollBarMode(enable ? Auto : AlwaysOff); }
    bool scrollBar() const { return vScrollBarMode() != AlwaysOff; }
    void setScrollBar(bool enable) { setVScrollBarMode(enable ? AlwaysOn : AlwaysOff); }
    bool autoBottomScrollBar() const { return hScrollBarMode() == Auto; }
    void setAutoBottomScrollBar(bool enable) { setHScrollBarMode(enable ? Auto : AlwaysOff); }
    bool bottomScrollBar() const { return hScrollBarMode() != AlwaysOff; }
    void setBottomScrollBar(bool enable) { setHScrollBarMode(enable ? AlwaysOn : AlwaysOff); }
    bool smoothScrolling() const { return false; }
    void setSmoothScrolling(bool) {}
    bool autoUpdate() const { return true; }
    void setAutoUpdate(bool) {}
    void setFixedVisibleLines(int lines) { setRowMode(lines); }
    int inSort(const Q3ListBoxItem *);
    int inSort(const QString& text);
    int cellHeight(int i) const { return itemHeight(i); }
    int cellHeight() const { return itemHeight(); }
    int cellWidth() const { return maxItemWidth(); }
    int cellWidth(int i) const { Q_ASSERT(i==0); Q_UNUSED(i) return maxItemWidth(); }
    int numCols() const { return numColumns(); }

    int itemHeight(int index = 0) const;
    Q3ListBoxItem * itemAt(const QPoint &) const;

    QRect itemRect(Q3ListBoxItem *item) const;

    Q3ListBoxItem *firstItem() const;

    void sort(bool ascending = true);

public Q_SLOTS:
    void clear();
    virtual void ensureCurrentVisible();
    virtual void clearSelection();
    virtual void selectAll(bool select);
    virtual void invertSelection();

Q_SIGNALS:
    void highlighted(int index);
    void selected(int index);
    void highlighted(const QString &);
    void selected(const QString &);
    void highlighted(Q3ListBoxItem *);
    void selected(Q3ListBoxItem *);

    void selectionChanged();
    void selectionChanged(Q3ListBoxItem *);
    void currentChanged(Q3ListBoxItem *);
    void clicked(Q3ListBoxItem *);
    void clicked(Q3ListBoxItem *, const QPoint &);
    void pressed(Q3ListBoxItem *);
    void pressed(Q3ListBoxItem *, const QPoint &);

    void doubleClicked(Q3ListBoxItem *);
    void returnPressed(Q3ListBoxItem *);
    void rightButtonClicked(Q3ListBoxItem *, const QPoint &);
    void rightButtonPressed(Q3ListBoxItem *, const QPoint &);
    void mouseButtonPressed(int, Q3ListBoxItem*, const QPoint&);
    void mouseButtonClicked(int, Q3ListBoxItem*, const QPoint&);

    void contextMenuRequested(Q3ListBoxItem *, const QPoint &);

    void onItem(Q3ListBoxItem *item);
    void onViewport();

protected:
    void changeEvent(QEvent *);
    void mousePressEvent(QMouseEvent *);
    void mouseReleaseEvent(QMouseEvent *);
    void mouseDoubleClickEvent(QMouseEvent *);
    void mouseMoveEvent(QMouseEvent *);
    void contentsContextMenuEvent(QContextMenuEvent *);

    void keyPressEvent(QKeyEvent *e);
    void focusInEvent(QFocusEvent *e);
    void focusOutEvent(QFocusEvent *e);
    void resizeEvent(QResizeEvent *);
    void showEvent(QShowEvent *);

    bool eventFilter(QObject *o, QEvent *e);

    void updateItem(int index);
    void updateItem(Q3ListBoxItem *);

    void updateCellWidth() { }
    int totalWidth() const { return contentsWidth(); }
    int totalHeight() const { return contentsHeight(); }

    virtual void paintCell(QPainter *, int row, int col);

    void toggleCurrentItem();
    bool isRubberSelecting() const;

    void doLayout() const;

    int findItem(int yPos) const { return index(itemAt(QPoint(0,yPos))); }

protected Q_SLOTS:
    void clearInputString();

private Q_SLOTS:
    void refreshSlot();
    void doAutoScroll();
    void adjustItems();

private:
    Q_DISABLE_COPY(Q3ListBox)

    void mousePressEventEx(QMouseEvent *);
    void tryGeometry(int, int) const;
    int currentRow() const;
    int currentColumn() const;
    void updateSelection();
    void repaintSelection();
    void drawRubber();
    void doRubberSelection(const QRect &old, const QRect &rubber);
    void handleItemChange(Q3ListBoxItem *old, bool shift, bool control);
    void selectRange(Q3ListBoxItem *from, Q3ListBoxItem *to, bool invert, bool includeFirst, bool clearSel = false);

    void emitChangedSignal(bool);

    int columnAt(int) const;
    int rowAt(int) const;

    Q3ListBoxPrivate * d;

    static Q3ListBox * changedListBox;
};


class Q_COMPAT_EXPORT Q3ListBoxItem
{
public:
    Q3ListBoxItem(Q3ListBox* listbox = 0);
    Q3ListBoxItem(Q3ListBox* listbox, Q3ListBoxItem *after);
    virtual ~Q3ListBoxItem();

    virtual QString text() const;
    virtual const QPixmap *pixmap() const;

    virtual int         height(const Q3ListBox *) const;
    virtual int         width(const Q3ListBox *)  const;

    bool isSelected() const { return s; }
    bool isCurrent() const;

    bool selected() const { return isSelected(); }
    bool current() const { return isCurrent(); }

    Q3ListBox *listBox() const;

    void setSelectable(bool b) { selectable = b; }
    bool isSelectable() const { return selectable; }

    Q3ListBoxItem *next() const;
    Q3ListBoxItem *prev() const;

    virtual int rtti() const;
    enum { RTTI = 0 };

protected:
    virtual void paint(QPainter *) = 0;
    virtual void setText(const QString &text) { txt = text; }
    void setCustomHighlighting(bool);

private:
    Q_DISABLE_COPY(Q3ListBoxItem)

    QString txt;
    uint selectable : 1;
    uint s : 1;
    uint dirty:1;
    uint custom_highlight : 1;
    uint unused : 28;
    Q3ListBoxItem * p, * n;
    Q3ListBox* lbox;
    friend class Q3ListBox;
    friend class Q3ListBoxPrivate;
    friend class Q3ComboBox;
    friend class Q3ComboBoxPopupItem;
};


class Q_COMPAT_EXPORT Q3ListBoxText : public Q3ListBoxItem
{
public:
    Q3ListBoxText(Q3ListBox* listbox, const QString & text=QString());
    Q3ListBoxText(const QString & text=QString());
    Q3ListBoxText(Q3ListBox* listbox, const QString & text, Q3ListBoxItem *after);
   ~Q3ListBoxText();

    int         height(const Q3ListBox *) const;
    int         width(const Q3ListBox *)  const;

    int rtti() const;
    enum { RTTI = 1 };

protected:
    void  paint(QPainter *);

private:
    Q_DISABLE_COPY(Q3ListBoxText)
};


class Q_COMPAT_EXPORT Q3ListBoxPixmap : public Q3ListBoxItem
{
public:
    Q3ListBoxPixmap(Q3ListBox* listbox, const QPixmap &);
    Q3ListBoxPixmap(const QPixmap &);
    Q3ListBoxPixmap(Q3ListBox* listbox, const QPixmap & pix, Q3ListBoxItem *after);
    Q3ListBoxPixmap(Q3ListBox* listbox, const QPixmap &, const QString&);
    Q3ListBoxPixmap(const QPixmap &, const QString&);
    Q3ListBoxPixmap(Q3ListBox* listbox, const QPixmap & pix, const QString&, Q3ListBoxItem *after);
   ~Q3ListBoxPixmap();

    const QPixmap *pixmap() const { return &pm; }

    int         height(const Q3ListBox *) const;
    int         width(const Q3ListBox *)  const;

    int rtti() const;
    enum { RTTI = 2 };

protected:
    void paint(QPainter *);

private:
    Q_DISABLE_COPY(Q3ListBoxPixmap)

    QPixmap pm;
};

#endif // QT_NO_LISTBOX

QT_END_NAMESPACE

QT_END_HEADER

#endif // Q3LISTBOX_H
