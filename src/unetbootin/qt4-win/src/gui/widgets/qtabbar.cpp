/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the QtGui module of the Qt Toolkit.
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

#include "private/qlayoutengine_p.h"
#include "qabstractitemdelegate.h"
#include "qapplication.h"
#include "qbitmap.h"
#include "qcursor.h"
#include "qevent.h"
#include "qpainter.h"
#include "qstyle.h"
#include "qstyleoption.h"
#include "qstylepainter.h"
#include "qtabwidget.h"
#include "qtooltip.h"
#include "qwhatsthis.h"
#include "private/qtextengine_p.h"
#ifndef QT_NO_ACCESSIBILITY
#include "qaccessible.h"
#endif

#include "qdebug.h"
#include "private/qtabbar_p.h"

#ifndef QT_NO_TABBAR

QT_BEGIN_NAMESPACE

inline static bool verticalTabs(QTabBar::Shape shape)
{
    return shape == QTabBar::RoundedWest
           || shape == QTabBar::RoundedEast
           || shape == QTabBar::TriangularWest
           || shape == QTabBar::TriangularEast;
}

/*!
    Initialize \a option with the values from the tab at \a tabIndex. This method
    is useful for subclasses when they need a QStyleOptionTab or QStyleOptionTabV2, but don't want
    to fill in all the information themselves. This function will check the version
    of the QStyleOptionTab and fill in the additional values for a
    QStyleOptionTabV2.

    \sa QStyleOption::initFrom() QTabWidget::initStyleOption()
*/
void QTabBar::initStyleOption(QStyleOptionTab *option, int tabIndex) const
{
    Q_D(const QTabBar);
    int totalTabs = d->tabList.size();

    if (!option || (tabIndex < 0 || tabIndex >= totalTabs))
        return;

    const QTabBarPrivate::Tab &tab = d->tabList.at(tabIndex);
    option->initFrom(this);
    option->state &= ~(QStyle::State_HasFocus | QStyle::State_MouseOver);
    option->rect = tabRect(tabIndex);
    bool isCurrent = tabIndex == d->currentIndex;
    option->row = 0;
    if (tabIndex == d->pressedIndex)
        option->state |= QStyle::State_Sunken;
    if (isCurrent)
        option->state |= QStyle::State_Selected;
    if (isCurrent && hasFocus())
        option->state |= QStyle::State_HasFocus;
    if (!tab.enabled)
        option->state &= ~QStyle::State_Enabled;
    if (isActiveWindow())
        option->state |= QStyle::State_Active;
    if (option->rect == d->hoverRect)
        option->state |= QStyle::State_MouseOver;
    option->shape = d->shape;
    option->text = tab.text;

    if (tab.textColor.isValid())
        option->palette.setColor(foregroundRole(), tab.textColor);

    option->icon = tab.icon;
    if (QStyleOptionTabV2 *optionV2 = qstyleoption_cast<QStyleOptionTabV2 *>(option))
        optionV2->iconSize = iconSize();  // Will get the default value then.


    if (tabIndex > 0 && tabIndex - 1 == d->currentIndex)
        option->selectedPosition = QStyleOptionTab::PreviousIsSelected;
    else if (tabIndex < totalTabs - 1 && tabIndex + 1 == d->currentIndex)
        option->selectedPosition = QStyleOptionTab::NextIsSelected;
    else
        option->selectedPosition = QStyleOptionTab::NotAdjacent;

    if (tabIndex == 0) {
        if (totalTabs > 1)
            option->position = QStyleOptionTab::Beginning;
        else
            option->position = QStyleOptionTab::OnlyOneTab;
    } else if (tabIndex == totalTabs - 1) {
        option->position = QStyleOptionTab::End;
    } else {
        option->position = QStyleOptionTab::Middle;
    }
#ifndef QT_NO_TABWIDGET
    if (const QTabWidget *tw = qobject_cast<const QTabWidget *>(parentWidget())) {
        if (tw->cornerWidget(Qt::TopLeftCorner) || tw->cornerWidget(Qt::BottomLeftCorner))
            option->cornerWidgets |= QStyleOptionTab::LeftCornerWidget;
        if (tw->cornerWidget(Qt::TopRightCorner) || tw->cornerWidget(Qt::BottomRightCorner))
            option->cornerWidgets |= QStyleOptionTab::RightCornerWidget;
    }
    int hframe  = style()->pixelMetric(QStyle::PM_TabBarTabHSpace, option, this);

    option->text = fontMetrics().elidedText(option->text, d->elideMode,
                        1 + (verticalTabs(d->shape) ? tab.rect.height() : tab.rect.width()) - hframe,
                        Qt::TextShowMnemonic);
#endif
}

/*!
    \class QTabBar
    \brief The QTabBar class provides a tab bar, e.g. for use in tabbed dialogs.

    \ingroup basicwidgets
    \mainclass

    QTabBar is straightforward to use; it draws the tabs using one of
    the predefined \link QTabBar::Shape shapes\endlink, and emits a
    signal when a tab is selected. It can be subclassed to tailor the
    look and feel. Qt also provides a ready-made \l{QTabWidget}.

    Each tab has a tabText(), an optional tabIcon(), an optional
    tabToolTip(), optional tabWhatsThis() and optional tabData().
    The tabs's attributes can be changed with setTabText(), setTabIcon(),
    setTabToolTip(), setTabWhatsThis and setTabData(). Each tabs can be
    enabled or disabled individually with setTabEnabled().

    Each tab can display text in a distinct color. The current text color
    for a tab can be found with the tabTextColor() function. Set the text
    color for a particular tab with setTabTextColor().

    Tabs are added using addTab(), or inserted at particular positions
    using insertTab(). The total number of tabs is given by
    count(). Tabs can be removed from the tab bar with
    removeTab(). Combining removeTab() and insertTab() allows you to
    move tabs to different positions.

    The \l shape property defines the tabs' appearance. The choice of
    shape is a matter of taste, although tab dialogs (for preferences
    and similar) invariably use \l RoundedNorth.
    Tab controls in windows other than dialogs almost
    always use either \l RoundedSouth or \l TriangularSouth. Many
    spreadsheets and other tab controls in which all the pages are
    essentially similar use \l TriangularSouth, whereas \l
    RoundedSouth is used mostly when the pages are different (e.g. a
    multi-page tool palette). The default in QTabBar is \l
    RoundedNorth.

    The most important part of QTabBar's API is the currentChanged()
    signal.  This is emitted whenever the current tab changes (even at
    startup, when the current tab changes from 'none'). There is also
    a slot, setCurrentIndex(), which can be used to select a tab
    programmatically. The function currentIndex() returns the index of
    the current tab, \l count holds the number of tabs.

    QTabBar creates automatic mnemonic keys in the manner of QAbstractButton;
    e.g. if a tab's label is "\&Graphics", Alt+G becomes a shortcut
    key for switching to that tab.

    The following virtual functions may need to be reimplemented in
    order to tailor the look and feel or store extra data with each
    tab:

    \list
    \i tabSizeHint() calcuates the size of a tab.
    \i tabInserted() notifies that a new tab was added.
    \i tabRemoved() notifies that a tab was removed.
    \i tabLayoutChange() notifies that the tabs have been re-laid out.
    \i paintEvent() paints all tabs.
    \endlist

    For subclasses, you might also need the tabRect() functions which
    returns the visual geometry of a single tab.

    \table 100%
    \row \o \inlineimage plastique-tabbar.png Screenshot of a Plastique style tab bar
         \o A tab bar shown in the Plastique widget style.
    \row \o \inlineimage plastique-tabbar-truncated.png Screenshot of a truncated Plastique tab bar
         \o A truncated tab bar shown in the Plastique widget style.
    \endtable

    \sa QTabWidget
*/

/*!
    \enum QTabBar::Shape

    This enum type lists the built-in shapes supported by QTabBar. Treat these
    as hints as some styles may not render some of the shapes. However,
    position should be honored.

    \value RoundedNorth  The normal rounded look above the pages

    \value RoundedSouth  The normal rounded look below the pages

    \value RoundedWest  The normal rounded look on the left side of the pages

    \value RoundedEast  The normal rounded look on the right side the pages

    \value TriangularNorth  Triangular tabs above the pages.

    \value TriangularSouth  Triangular tabs similar to those used in
    the Excel spreadsheet, for example

    \value TriangularWest  Triangular tabs on the left of the pages.

    \value TriangularEast  Triangular tabs on the right of the pages.
    \omitvalue RoundedAbove
    \omitvalue RoundedBelow
    \omitvalue TriangularAbove
    \omitvalue TriangularBelow
*/

/*!
    \fn void QTabBar::currentChanged(int index)

    This signal is emitted when the tab bar's current tab changes. The
    new current has the given \a index, or -1 if there isn't a new one
    (for example, if there are no tab in the QTabBar)
*/

int QTabBarPrivate::extraWidth() const
{
    Q_Q(const QTabBar);
    return 2 * qMax(q->style()->pixelMetric(QStyle::PM_TabBarScrollButtonWidth, 0, q),
                    QApplication::globalStrut().width());
}

void QTabBarPrivate::init()
{
    Q_Q(QTabBar);
    leftB = new QToolButton(q);
    leftB->setAutoRepeat(true);
    QObject::connect(leftB, SIGNAL(clicked()), q, SLOT(_q_scrollTabs()));
    leftB->hide();
    rightB = new QToolButton(q);
    rightB->setAutoRepeat(true);
    QObject::connect(rightB, SIGNAL(clicked()), q, SLOT(_q_scrollTabs()));
    rightB->hide();
#ifdef QT_KEYPAD_NAVIGATION
    if (QApplication::keypadNavigationEnabled()) {
        leftB->setFocusPolicy(Qt::NoFocus);
        rightB->setFocusPolicy(Qt::NoFocus);
        q->setFocusPolicy(Qt::NoFocus);
    } else
#endif
    q->setFocusPolicy(Qt::TabFocus);
    q->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    elideMode = Qt::TextElideMode(q->style()->styleHint(QStyle::SH_TabBar_ElideMode, 0, q));
    useScrollButtons = !q->style()->styleHint(QStyle::SH_TabBar_PreferNoArrows, 0, q);
}

QTabBarPrivate::Tab *QTabBarPrivate::at(int index)
{
    return validIndex(index)?&tabList[index]:0;
}

const QTabBarPrivate::Tab *QTabBarPrivate::at(int index) const
{
    return validIndex(index)?&tabList[index]:0;
}

int QTabBarPrivate::indexAtPos(const QPoint &p) const
{
    Q_Q(const QTabBar);
    if (q->tabRect(currentIndex).contains(p))
        return currentIndex;
    for (int i = 0; i < tabList.count(); ++i)
        if (tabList.at(i).enabled && q->tabRect(i).contains(p))
            return i;
    return -1;
}

void QTabBarPrivate::layoutTabs()
{
    Q_Q(QTabBar);
    scrollOffset = 0;
    layoutDirty = false;
    QSize size = q->size();
    int last, available;
    int maxExtent;
    int i;
    bool vertTabs = verticalTabs(shape);
    int tabChainIndex = 0;
    Qt::Alignment tabAlignment = Qt::Alignment(q->style()->styleHint(QStyle::SH_TabBar_Alignment, 0, q));
    QVector<QLayoutStruct> tabChain(tabList.count() + 2);

    // We put an empty item at the front and back and set its expansive attribute
    // depending on tabAlignment.
    tabChain[tabChainIndex].init();
    tabChain[tabChainIndex].expansive = (tabAlignment != Qt::AlignLeft)
                                        && (tabAlignment != Qt::AlignJustify);
    tabChain[tabChainIndex].empty = true;
    ++tabChainIndex;

    // We now go through our list of tabs and set the minimum size and the size hint
    // This will allow us to elide text if necessary. Since we don't set
    // a maximum size, tabs will EXPAND to fill up the empty space.
    // Since tab widget is rather *ahem* strict about keeping the geometry of the
    // tab bar to its absolute minimum, this won't bleed through, but will show up
    // if you use tab bar on its own (a.k.a. not a bug, but a feature).
    // Update: if squeezeTabs is true, we DO set a maximum size to prevent the tabs
    // being wider than necessary.
    if (!vertTabs) {
        int minx = 0;
        int x = 0;
        int maxHeight = 0;
        for (i = 0; i < tabList.count(); ++i, ++tabChainIndex) {
            QSize sz = q->tabSizeHint(i);
            tabList[i].maxRect = QRect(x, 0, sz.width(), sz.height());
            x += sz.width();
            maxHeight = qMax(maxHeight, sz.height());
            sz = minimumTabSizeHint(i);
            tabList[i].minRect = QRect(minx, 0, sz.width(), sz.height());
            minx += sz.width();
            tabChain[tabChainIndex].init();
            tabChain[tabChainIndex].sizeHint = tabList.at(i).maxRect.width();
            tabChain[tabChainIndex].minimumSize = sz.width();
            tabChain[tabChainIndex].empty = false;
            tabChain[tabChainIndex].expansive = true;

            if (squeezeTabs)
                tabChain[tabChainIndex].maximumSize = tabChain[tabChainIndex].sizeHint;
        }

        last = minx;
        available = size.width();
        maxExtent = maxHeight;
    } else {
        int miny = 0;
        int y = 0;
        int maxWidth = 0;
        for (i = 0; i < tabList.count(); ++i, ++tabChainIndex) {
            QSize sz = q->tabSizeHint(i);
            tabList[i].maxRect = QRect(0, y, sz.width(), sz.height());
            y += sz.height();
            maxWidth = qMax(0, sz.width());
            sz = minimumTabSizeHint(i);
            tabList[i].minRect = QRect(0, miny, sz.width(), sz.height());
            miny += sz.height();
            tabChain[tabChainIndex].init();
            tabChain[tabChainIndex].sizeHint = tabList.at(i).maxRect.height();
            tabChain[tabChainIndex].minimumSize = sz.height();
            tabChain[tabChainIndex].empty = false;
            tabChain[tabChainIndex].expansive = true;

            if (squeezeTabs)
                tabChain[tabChainIndex].maximumSize = tabChain[tabChainIndex].sizeHint;
        }

        last = miny;
        available = size.height();
        maxExtent = maxWidth;
    }

    Q_ASSERT(tabChainIndex == tabChain.count() - 1); // add an assert just to make sure.
    // Mirror our front item.
    tabChain[tabChainIndex].init();
    tabChain[tabChainIndex].expansive = (tabAlignment != Qt::AlignRight)
                                        && (tabAlignment != Qt::AlignJustify);
    tabChain[tabChainIndex].empty = true;

    // Do the calculation
    qGeomCalc(tabChain, 0, tabChain.count(), 0, qMax(available, last), 0);

    // Use the results
    for (i = 0; i < tabList.count(); ++i) {
        const QLayoutStruct &lstruct = tabChain.at(i + 1);
        if (!vertTabs)
            tabList[i].rect.setRect(lstruct.pos, 0, lstruct.size, maxExtent);
        else
            tabList[i].rect.setRect(0, lstruct.pos, maxExtent, lstruct.size);
    }

    if (useScrollButtons && tabList.count() && last > available) {
        int extra = extraWidth();
        if (!vertTabs) {
            Qt::LayoutDirection ld = q->layoutDirection();
            QRect arrows = QStyle::visualRect(ld, q->rect(),
                                              QRect(available - extra, 0, extra, size.height()));
            int buttonOverlap = q->style()->pixelMetric(QStyle::PM_TabBar_ScrollButtonOverlap, 0, q);
            
            if (ld == Qt::LeftToRight) {
                leftB->setGeometry(arrows.left(), arrows.top(), extra/2, arrows.height());
                rightB->setGeometry(arrows.right() - extra/2 + buttonOverlap, arrows.top(),
                                    extra/2, arrows.height());
                leftB->setArrowType(Qt::LeftArrow);
                rightB->setArrowType(Qt::RightArrow);
            } else {
                rightB->setGeometry(arrows.left(), arrows.top(), extra/2, arrows.height());
                leftB->setGeometry(arrows.right() - extra/2 + buttonOverlap, arrows.top(),
                                    extra/2, arrows.height());
                rightB->setArrowType(Qt::LeftArrow);
                leftB->setArrowType(Qt::RightArrow);
            }
        } else {
            QRect arrows = QRect(0, available - extra, size.width(), extra );
            leftB->setGeometry(arrows.left(), arrows.top(), arrows.width(), extra/2);
            leftB->setArrowType(Qt::UpArrow);
            rightB->setGeometry(arrows.left(), arrows.bottom() - extra/2 + 1,
                                arrows.width(), extra/2);
            rightB->setArrowType(Qt::DownArrow);
        }
        leftB->setEnabled(scrollOffset > 0);
        rightB->setEnabled(last - scrollOffset >= available - extra);
        leftB->show();
        rightB->show();
    } else {
        rightB->hide();
        leftB->hide();
    }

    q->tabLayoutChange();
}

void QTabBarPrivate::makeVisible(int index)
{
    Q_Q(QTabBar);
    if (!validIndex(index) || leftB->isHidden())
        return;
    const QRect tabRect = tabList.at(index).rect;

    const int oldScrollOffset = scrollOffset;
    const bool horiz = !verticalTabs(shape);
    const int available = (horiz ? q->width() : q->height()) - extraWidth();
    const int start = horiz ? tabRect.left() : tabRect.top();
    const int end = horiz ? tabRect.right() : tabRect.bottom();
    if (start < scrollOffset) // too far left
        scrollOffset = start - (index?8:0);
    else if (end > scrollOffset + available) // too far right
        scrollOffset = end - available + 1;

    leftB->setEnabled(scrollOffset > 0);
    const int last = horiz ? tabList.last().rect.right() : tabList.last().rect.bottom();
    rightB->setEnabled(last - scrollOffset >= available);
    if (oldScrollOffset != scrollOffset)
        q->update();
}

void QTabBarPrivate::_q_scrollTabs()
{
    Q_Q(QTabBar);
    const QObject *sender = q->sender();
    int i = -1;
    if (!verticalTabs(shape)) {
        if (sender == leftB) {
            for (i = tabList.count() - 1; i >= 0; --i) {
                if (tabList.at(i).rect.left() - scrollOffset < 0) {
                    makeVisible(i);
                    return;
                }
            }
        } else if (sender == rightB) {
            int availableWidth = q->width() - extraWidth();
            for (i = 0; i < tabList.count(); ++i) {
                if (tabList.at(i).rect.right() - scrollOffset > availableWidth) {
                    makeVisible(i);
                    return;
                }
            }
        }
    } else { // vertical
        if (sender == leftB) {
            for (i = tabList.count() - 1; i >= 0; --i) {
                if (tabList.at(i).rect.top() - scrollOffset < 0) {
                    makeVisible(i);
                    return;
                }
            }
        } else if (sender == rightB) {
            int available = q->height() - extraWidth();
            for (i = 0; i < tabList.count(); ++i) {
                if (tabList.at(i).rect.bottom() - scrollOffset > available) {
                    makeVisible(i);
                    return;
                }
            }
        }
    }
}

void QTabBarPrivate::refresh()
{
    Q_Q(QTabBar);
    if (!q->isVisible()) {
        layoutDirty = true;
    } else {
        layoutTabs();
        makeVisible(currentIndex);
        q->update();
        q->updateGeometry();
    }
}

/*!
    Creates a new tab bar with the given \a parent.
*/
QTabBar::QTabBar(QWidget* parent)
    :QWidget(*new QTabBarPrivate, parent, 0)
{
    Q_D(QTabBar);
    d->init();
}


/*!
    Destroys the tab bar.
*/
QTabBar::~QTabBar()
{
}

/*!
    \property QTabBar::shape
    \brief the shape of the tabs in the tab bar

    Possible values for this property are described by the Shape enum.
*/


QTabBar::Shape QTabBar::shape() const
{
    Q_D(const QTabBar);
    return d->shape;
}

void QTabBar::setShape(Shape shape)
{
    Q_D(QTabBar);
    if (d->shape == shape)
        return;
    d->shape = shape;
    d->refresh();
}


/*!
    \property QTabBar::drawBase
    \brief defines whether or not tab bar should draw its base.

    If true then QTabBar draws a base in relation to the styles overlab.
    Otherwise only the tabs are drawn.

    \sa QStyle::pixelMetric() QStyle::PM_TabBarBaseOverlap QStyleOptionTabBarBase
*/

void QTabBar::setDrawBase(bool drawBase)
{
    Q_D(QTabBar);
    d->drawBase = drawBase;
}

bool QTabBar::drawBase() const
{
    Q_D(const QTabBar);
    return d->drawBase;
}


/*!
    Adds a new tab with text \a text. Returns the new
    tab's index.
*/
int QTabBar::addTab(const QString &text)
{
    return insertTab(-1, text);
}

/*!
    \overload

    Adds a new tab with icon \a icon and text \a
    text. Returns the new tab's index.
*/
int QTabBar::addTab(const QIcon& icon, const QString &text)
{
    return insertTab(-1, icon, text);
}

/*!
    Inserts a new tab with text \a text at position \a index. If \a
    index is out of range, the new tab is appened. Returns the new
    tab's index.
*/
int QTabBar::insertTab(int index, const QString &text)
{
    return insertTab(index, QIcon(), text);
}

/*!\overload

    Inserts a new tab with icon \a icon and text \a text at position
    \a index. If \a index is out of range, the new tab is
    appended. Returns the new tab's index.

    If the QTabBar was empty before this function is called, the inserted tab
    becomes the current tab.

    Inserting a new tab at an index less than or equal to the current index
    will increment the current index, but keep the current tab.
*/
int QTabBar::insertTab(int index, const QIcon& icon, const QString &text)
{
    Q_D(QTabBar);
    if (!d->validIndex(index)) {
        index = d->tabList.count();
        d->tabList.append(QTabBarPrivate::Tab(icon, text));
    } else {
        d->tabList.insert(index, QTabBarPrivate::Tab(icon, text));
    }
#ifndef QT_NO_SHORTCUT
    d->tabList[index].shortcutId = grabShortcut(QKeySequence::mnemonic(text));
#endif
    d->refresh();
    if (d->tabList.count() == 1)
        setCurrentIndex(index);
    else if (index <= d->currentIndex)
        ++d->currentIndex;

    tabInserted(index);
    return index;
}


/*!
    Removes the tab at position \a index.
 */
void QTabBar::removeTab(int index)
{
    Q_D(QTabBar);
    if (d->validIndex(index)) {
#ifndef QT_NO_SHORTCUT
        releaseShortcut(d->tabList.at(index).shortcutId);
#endif
        d->tabList.removeAt(index);
        if (index == d->currentIndex) {
            // The current tab is going away, in order to make sure
            // we emit that "current has changed", we need to reset this
            // around.
            d->currentIndex = -1;
            if (d->tabList.size() > 0) {
                int newIndex = (index == d->tabList.size()) ? index-1 : index;
                setCurrentIndex(newIndex);
            } else {
                emit currentChanged(-1);
            }
        } else if (index < d->currentIndex) {
            setCurrentIndex(d->currentIndex - 1);
        }
        d->refresh();
        tabRemoved(index);
    }
}


/*!
    Returns true if the tab at position \a index is enabled; otherwise
    returns false.
*/
bool QTabBar::isTabEnabled(int index) const
{
    Q_D(const QTabBar);
    if (const QTabBarPrivate::Tab *tab = d->at(index))
        return tab->enabled;
    return false;
}

/*!
    If \a enabled is true then the tab at position \a index is
    enabled; otherwise the item at position \a index is disabled.
*/
void QTabBar::setTabEnabled(int index, bool enabled)
{
    Q_D(QTabBar);
    if (QTabBarPrivate::Tab *tab = d->at(index)) {
        tab->enabled = enabled;
#ifndef QT_NO_SHORTCUT
        setShortcutEnabled(tab->shortcutId, enabled);
#endif
        update();
        if (!enabled && index == d->currentIndex)
            setCurrentIndex(d->validIndex(index+1)?index+1:0);
        else if (enabled && !d->validIndex(d->currentIndex))
            setCurrentIndex(index);
    }
}


/*!
    Returns the text of the tab at position \a index, or an empty
    string if \a index is out of range.
*/
QString QTabBar::tabText(int index) const
{
    Q_D(const QTabBar);
    if (const QTabBarPrivate::Tab *tab = d->at(index))
        return tab->text;
    return QString();
}

/*!
    Sets the text of the tab at position \a index to \a text.
*/
void QTabBar::setTabText(int index, const QString &text)
{
    Q_D(QTabBar);
    if (QTabBarPrivate::Tab *tab = d->at(index)) {
        tab->text = text;
#ifndef QT_NO_SHORTCUT
        releaseShortcut(tab->shortcutId);
        tab->shortcutId = grabShortcut(QKeySequence::mnemonic(text));
        setShortcutEnabled(tab->shortcutId, tab->enabled);
#endif
        d->refresh();
    }
}

/*!
    Returns the text color of the tab with the given \a index, or a invalid
    color if \a index is out of range.

    \sa setTabTextColor()
*/
QColor QTabBar::tabTextColor(int index) const
{
    Q_D(const QTabBar);
    if (const QTabBarPrivate::Tab *tab = d->at(index))
        return tab->textColor;
    return QColor();
}

/*!
    Sets the color of the text in the tab with the given \a index to the specified \a color.

    If an invalid color is specified, the tab will use the QTabBar foreground role instead.

    \sa tabTextColor()
*/
void QTabBar::setTabTextColor(int index, const QColor &color)
{
    Q_D(QTabBar);
    if (QTabBarPrivate::Tab *tab = d->at(index)) {
        tab->textColor = color;
        update(tabRect(index));
    }
}

/*!
    Returns the icon of the tab at position \a index, or a null icon
    if \a index is out of range.
*/
QIcon QTabBar::tabIcon(int index) const
{
    Q_D(const QTabBar);
    if (const QTabBarPrivate::Tab *tab = d->at(index))
        return tab->icon;
    return QIcon();
}

/*!
    Sets the icon of the tab at position \a index to \a icon.
*/
void QTabBar::setTabIcon(int index, const QIcon & icon)
{
    Q_D(QTabBar);
    if (QTabBarPrivate::Tab *tab = d->at(index)) {
        bool simpleIconChange = (!icon.isNull() && !tab->icon.isNull());
        tab->icon = icon;
        if (simpleIconChange)
            update(tabRect(index));
        else
            d->refresh();
    }
}

#ifndef QT_NO_TOOLTIP
/*!
    Sets the tool tip of the tab at position \a index to \a tip.
*/
void QTabBar::setTabToolTip(int index, const QString & tip)
{
    Q_D(QTabBar);
    if (QTabBarPrivate::Tab *tab = d->at(index))
        tab->toolTip = tip;
}

/*!
    Returns the tool tip of the tab at position \a index, or an empty
    string if \a index is out of range.
*/
QString QTabBar::tabToolTip(int index) const
{
    Q_D(const QTabBar);
    if (const QTabBarPrivate::Tab *tab = d->at(index))
        return tab->toolTip;
    return QString();
}
#endif // QT_NO_TOOLTIP

#ifndef QT_NO_WHATSTHIS
/*!
    \since 4.1

    Sets the What's This help text of the tab at position \a index
    to \a text.
*/
void QTabBar::setTabWhatsThis(int index, const QString &text)
{
    Q_D(QTabBar);
    if (QTabBarPrivate::Tab *tab = d->at(index))
        tab->whatsThis = text;
}

/*!
    \since 4.1

    Returns the What's This help text of the tab at position \a index,
    or an empty string if \a index is out of range.
*/
QString QTabBar::tabWhatsThis(int index) const
{
    Q_D(const QTabBar);
    if (const QTabBarPrivate::Tab *tab = d->at(index))
        return tab->whatsThis;
    return QString();
}

#endif // QT_NO_WHATSTHIS

/*!
    Sets the data of the tab at position \a index to \a data.
*/
void QTabBar::setTabData(int index, const QVariant & data)
{
    Q_D(QTabBar);
    if (QTabBarPrivate::Tab *tab = d->at(index))
        tab->data = data;
}

/*!
    Returns the datad of the tab at position \a index, or a null
    variant if \a index is out of range.
*/
QVariant QTabBar::tabData(int index) const
{
    Q_D(const QTabBar);
    if (const QTabBarPrivate::Tab *tab = d->at(index))
        return tab->data;
    return QVariant();
}

/*!
    Returns the visual rectangle of the of the tab at position \a
    index, or a null rectangle if \a index is out of range.
*/
QRect QTabBar::tabRect(int index) const
{
    Q_D(const QTabBar);
    if (const QTabBarPrivate::Tab *tab = d->at(index)) {
        if (d->layoutDirty)
            const_cast<QTabBarPrivate*>(d)->layoutTabs();
        QRect r = tab->rect;
        if (verticalTabs(d->shape))
            r.translate(0, -d->scrollOffset);
        else
            r.translate(-d->scrollOffset, 0);
        return QStyle::visualRect(layoutDirection(), rect(), r);
    }
    return QRect();
}

/*!
    \since 4.3
    Returns the index of the tab that covers \a position or -1 if no
    tab covers \a position;
*/

int QTabBar::tabAt(const QPoint &position) const
{
    Q_D(const QTabBar);
    if (d->validIndex(d->currentIndex)
        && tabRect(d->currentIndex).contains(position)) {
        return d->currentIndex;
    }
    const int max = d->tabList.size();
    for (int i = 0; i < max; ++i) {
        if (tabRect(i).contains(position)) {
            return i;
        }
    }
    return -1;
}

/*!
    \property QTabBar::currentIndex
    \brief the index of the tab bar's visible tab

    The current index is -1 if there is no current tab.
*/

int QTabBar::currentIndex() const
{
    Q_D(const QTabBar);
    if (d->validIndex(d->currentIndex))
        return d->currentIndex;
    return -1;
}


void QTabBar::setCurrentIndex(int index)
{
    Q_D(QTabBar);
    if (d->validIndex(index) && d->currentIndex != index) {
        d->currentIndex = index;
        update();
        d->makeVisible(index);
#ifdef QT3_SUPPORT
        emit selected(index);
#endif
        emit currentChanged(index);
    }
}

/*!
    \property QTabBar::iconSize
    \brief The size for icons in the tab bar
    \since 4.1

    The default value is style-dependent. \c iconSize is a maximum
    size; icons that are smaller are not scaled up.

    \sa QTabWidget::iconSize
*/
QSize QTabBar::iconSize() const
{
    Q_D(const QTabBar);
    if (d->iconSize.isValid())
        return d->iconSize;
    int iconExtent = style()->pixelMetric(QStyle::PM_TabBarIconSize, 0, this);
    return QSize(iconExtent, iconExtent);

}

void QTabBar::setIconSize(const QSize &size)
{
    Q_D(QTabBar);
    d->iconSize = size;
    d->layoutDirty = true;
    update();
    updateGeometry();
}

/*!
    \property QTabBar::count
    \brief the number of tabs in the tab bar
*/

int QTabBar::count() const
{
    Q_D(const QTabBar);
    return d->tabList.count();
}


/*!\reimp
 */
QSize QTabBar::sizeHint() const
{
    Q_D(const QTabBar);
    if (d->layoutDirty)
        const_cast<QTabBarPrivate*>(d)->layoutTabs();
    QRect r;
    for (int i = 0; i < d->tabList.count(); ++i)
        r = r.united(d->tabList.at(i).maxRect);
    QSize sz = QApplication::globalStrut();
    return r.size().expandedTo(sz);
}

/*!\reimp
 */
QSize QTabBar::minimumSizeHint() const
{
    Q_D(const QTabBar);
    if (!d->useScrollButtons) {
        QRect r;
        for (int i = 0; i < d->tabList.count(); ++i)
            r = r.united(d->tabList.at(i).minRect);
        return r.size().expandedTo(QApplication::globalStrut());
    }
    if (verticalTabs(d->shape))
        return QSize(sizeHint().width(), d->rightB->sizeHint().height() * 2 + 75);
    else
        return QSize(d->rightB->sizeHint().width() * 2 + 75, sizeHint().height());
}

static QString computeElidedText(Qt::TextElideMode mode, const QString &text)
{
    if (text.length() <= 7)
        return text;

    static const QLatin1String Ellipses("...");
    QString ret;
    switch (mode) {
    case Qt::ElideRight:
        ret = text.left(4) + Ellipses;
        break;
    case Qt::ElideMiddle:
        ret = text.left(2) + Ellipses + text.right(2);
        break;
    case Qt::ElideLeft:
        ret = Ellipses + text.right(4);
        break;
    case Qt::ElideNone:
        ret = text;
        break;
    }
    return ret;
}

QSize QTabBarPrivate::minimumTabSizeHint(int index)
{
    Q_Q(QTabBar);
    // ### Qt 5: make this a protected virtual function in QTabBar
    Tab &tab = tabList[index];
    QString oldText = tab.text;
    tab.text = computeElidedText(elideMode, oldText);
    QSize size = q->tabSizeHint(index);
    tab.text = oldText;
    return size;
}

/*!
    Returns the size hint for the tab at position \a index.
*/
QSize QTabBar::tabSizeHint(int index) const
{
    Q_D(const QTabBar);
    if (const QTabBarPrivate::Tab *tab = d->at(index)) {
        QStyleOptionTabV2 opt;
        initStyleOption(&opt, index);
        opt.text = d->tabList.at(index).text;
        QSize iconSize = tab->icon.isNull() ? QSize() : opt.iconSize;
        int hframe  = style()->pixelMetric(QStyle::PM_TabBarTabHSpace, &opt, this);
        int vframe  = style()->pixelMetric(QStyle::PM_TabBarTabVSpace, &opt, this);
        const QFontMetrics fm = fontMetrics();
        QSize csz(fm.size(Qt::TextShowMnemonic, tab->text).width() + iconSize.width() + hframe,
                  qMax(fm.height(), iconSize.height()) + vframe);
        if (verticalTabs(d->shape))
            csz.transpose();

        QSize retSize = style()->sizeFromContents(QStyle::CT_TabBarTab, &opt, csz, this);
        return retSize;
    }
    return QSize();
}

/*!
  This virtual handler is called after a new tab was added or
  inserted at position \a index.

  \sa tabRemoved()
 */
void QTabBar::tabInserted(int index)
{
    Q_UNUSED(index)
}

/*!
  This virtual handler is called after a tab was removed from
  position \a index.

  \sa tabInserted()
 */
void QTabBar::tabRemoved(int index)
{
    Q_UNUSED(index)
}

/*!
  This virtual handler is called whenever the tab layout changes.

  \sa tabRect()
 */
void QTabBar::tabLayoutChange()
{
}


/*!\reimp
 */
void QTabBar::showEvent(QShowEvent *)
{
    Q_D(QTabBar);
    if (d->layoutDirty)
        d->layoutTabs();
    if (!d->validIndex(d->currentIndex))
        setCurrentIndex(0);

}


/*!\reimp
 */
bool QTabBar::event(QEvent *e)
{
    Q_D(QTabBar);
    if (e->type() == QEvent::HoverMove
        || e->type() == QEvent::HoverEnter) {
        QHoverEvent *he = static_cast<QHoverEvent *>(e);
        if (!d->hoverRect.contains(he->pos())) {
            QRect oldHoverRect = d->hoverRect;
            for (int i = 0; i < d->tabList.count(); ++i) {
                QRect area = tabRect(i);
                if (area.contains(he->pos())) {
                    d->hoverRect = area;
                    break;
                }
            }
            if (he->oldPos() != QPoint(-1, -1))
                update(oldHoverRect);
            update(d->hoverRect);
        }
        return true;
    } else if (e->type() == QEvent::HoverLeave ) {
        QRect oldHoverRect = d->hoverRect;
        d->hoverRect = QRect();
        update(oldHoverRect);
        return true;
#ifndef QT_NO_TOOLTIP
    } else if (e->type() == QEvent::ToolTip) {
        if (const QTabBarPrivate::Tab *tab = d->at(tabAt(static_cast<QHelpEvent*>(e)->pos()))) {
            if (!tab->toolTip.isEmpty()) {
                QToolTip::showText(static_cast<QHelpEvent*>(e)->globalPos(), tab->toolTip, this);
                return true;
            }
        }
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_WHATSTHIS
    } else if (e->type() == QEvent::QueryWhatsThis) {
        const QTabBarPrivate::Tab *tab = d->at(d->indexAtPos(static_cast<QHelpEvent*>(e)->pos()));
        if (!tab || tab->whatsThis.isEmpty())
            e->ignore();
        return true;
    } else if (e->type() == QEvent::WhatsThis) {
        if (const QTabBarPrivate::Tab *tab = d->at(d->indexAtPos(static_cast<QHelpEvent*>(e)->pos()))) {
            if (!tab->whatsThis.isEmpty()) {
                QWhatsThis::showText(static_cast<QHelpEvent*>(e)->globalPos(),
                                     tab->whatsThis, this);
                return true;
            }
        }
#endif // QT_NO_WHATSTHIS
#ifndef QT_NO_SHORTCUT
    } else if (e->type() == QEvent::Shortcut) {
        QShortcutEvent *se = static_cast<QShortcutEvent *>(e);
        for (int i = 0; i < d->tabList.count(); ++i) {
            const QTabBarPrivate::Tab *tab = &d->tabList.at(i);
            if (tab->shortcutId == se->shortcutId()) {
                setCurrentIndex(i);
                return true;
            }
        }
#endif
    }
    return QWidget::event(e);
}

/*!\reimp
 */
void QTabBar::resizeEvent(QResizeEvent *)
{
    Q_D(QTabBar);
    d->layoutTabs();
    d->makeVisible(d->currentIndex);
}

/*!\reimp
 */
void QTabBar::paintEvent(QPaintEvent *)
{
    Q_D(QTabBar);
    QStyleOptionTab tabOverlap;
    tabOverlap.shape = d->shape;
    int overlap = style()->pixelMetric(QStyle::PM_TabBarBaseOverlap, &tabOverlap, this);
    QWidget *theParent = parentWidget();
    QStyleOptionTabBarBase optTabBase;
    optTabBase.init(this);
    optTabBase.shape = d->shape;
    if (theParent && overlap > 0) {
        QRect rect;
        switch (tabOverlap.shape) {
        case QTabBar::RoundedNorth:
        case QTabBar::TriangularNorth:
            rect.setRect(0, height()-overlap, width(), overlap);
            break;
        case QTabBar::RoundedSouth:
        case QTabBar::TriangularSouth:
            rect.setRect(0, 0, width(), overlap);
            break;
        case QTabBar::RoundedEast:
        case QTabBar::TriangularEast:
            rect.setRect(0, 0, overlap, height());
            break;
        case QTabBar::RoundedWest:
        case QTabBar::TriangularWest:
            rect.setRect(width()-overlap, 0, overlap, height());
            break;
        }
        optTabBase.rect = rect;
    }
    QStylePainter p(this);
    int selected = -1;
    int cut = -1;
    bool rtl = optTabBase.direction == Qt::RightToLeft;
    bool verticalTabs = (d->shape == QTabBar::RoundedWest || d->shape == QTabBar::RoundedEast
                         || d->shape == QTabBar::TriangularWest
                         || d->shape == QTabBar::TriangularEast);
    QStyleOptionTab cutTab;
    QStyleOptionTab selectedTab;
    for (int i = 0; i < d->tabList.count(); ++i) {
        QStyleOptionTabV2 tab;
        initStyleOption(&tab, i);
        if (!(tab.state & QStyle::State_Enabled)) {
            tab.palette.setCurrentColorGroup(QPalette::Disabled);
        }
        // If this tab is partially obscured, make a note of it so that we can pass the information
        // along when we draw the tear.
        if (((!verticalTabs && (!rtl && tab.rect.left() < 0)) || (rtl && tab.rect.right() > width()))
            || (verticalTabs && tab.rect.top() < 0)) {
            cut = i;
            cutTab = tab;
        }
        // Don't bother drawing a tab if the entire tab is outside of the visible tab bar.
        if ((!verticalTabs && (tab.rect.right() < 0 || tab.rect.left() > width()))
            || (verticalTabs && (tab.rect.bottom() < 0 || tab.rect.top() > height())))
            continue;

        optTabBase.tabBarRect |= tab.rect;
        if (i == d->currentIndex) {
            selected = i;
            selectedTab = tab;
            optTabBase.selectedTabRect = tab.rect;
            continue;
        }
        p.drawControl(QStyle::CE_TabBarTab, tab);
    }

    // Draw the selected tab last to get it "on top"
    if (selected >= 0) {
        QStyleOptionTabV2 tab;
        initStyleOption(&tab, selected);
        p.drawControl(QStyle::CE_TabBarTab, tab);
    }
    if (d->drawBase)
        p.drawPrimitive(QStyle::PE_FrameTabBarBase, optTabBase);

    // Only draw the tear indicator if necessary. Most of the time we don't need too.
    if (d->leftB->isVisible() && cut >= 0) {
        cutTab.rect = rect();
        cutTab.rect = style()->subElementRect(QStyle::SE_TabBarTearIndicator, &cutTab, this);
        p.drawPrimitive(QStyle::PE_IndicatorTabTear, cutTab);
    }
}

/*!\reimp
 */
void QTabBar::mousePressEvent (QMouseEvent *e)
{
    Q_D(QTabBar);
    if (e->button() != Qt::LeftButton) {
        e->ignore();
        return;
    }
    d->pressedIndex = d->indexAtPos(e->pos());
    if (d->pressedIndex >= 0) {
        if (e->type() == style()->styleHint(QStyle::SH_TabBar_SelectMouseType, 0, this))
            setCurrentIndex(d->pressedIndex);
        else
            repaint(tabRect(d->pressedIndex));
    }
}

/*!\reimp
 */
void QTabBar::mouseMoveEvent (QMouseEvent *e)
{
    Q_D(QTabBar);
    if (e->buttons() != Qt::LeftButton) {
        e->ignore();
        return;
    }
    if (style()->styleHint(QStyle::SH_TabBar_SelectMouseType, 0, this)
            == QEvent::MouseButtonRelease) {
        int i = d->indexAtPos(e->pos());
        if (i != d->pressedIndex) {
            int oldIndex = d->pressedIndex;
            d->pressedIndex = -1;
            if (oldIndex >= 0)
                repaint(tabRect(oldIndex));
            if ((d->pressedIndex = i) >= 0)
                repaint(tabRect(i));
        }
    }
}

/*!\reimp
 */
void QTabBar::mouseReleaseEvent (QMouseEvent *e)
{
    Q_D(QTabBar);
    if (e->button() != Qt::LeftButton)
        e->ignore();

    int i = d->indexAtPos(e->pos()) == d->pressedIndex ? d->pressedIndex : -1;
    d->pressedIndex = -1;
    if (e->type() == style()->styleHint(QStyle::SH_TabBar_SelectMouseType, 0, this))
        setCurrentIndex(i);
}

/*!\reimp
 */
void QTabBar::keyPressEvent(QKeyEvent *e)
{
    Q_D(QTabBar);
    if (e->key() != Qt::Key_Left && e->key() != Qt::Key_Right) {
        e->ignore();
        return;
    }
    int dx = e->key() == (isRightToLeft() ? Qt::Key_Right : Qt::Key_Left) ? -1 : 1;
    for (int index = d->currentIndex + dx; d->validIndex(index); index += dx) {
        if (d->tabList.at(index).enabled) {
            setCurrentIndex(index);
            break;
        }
    }
}

/*!\reimp
 */
void QTabBar::changeEvent(QEvent *e)
{
    Q_D(QTabBar);
    if (e->type() == QEvent::StyleChange) {
        d->elideMode = Qt::TextElideMode(style()->styleHint(QStyle::SH_TabBar_ElideMode, 0, this));
        d->useScrollButtons = !style()->styleHint(QStyle::SH_TabBar_PreferNoArrows, 0, this);
    }
    d->refresh();
    QWidget::changeEvent(e);
}

/*!
    \property QTabBar::elideMode
    \brief how to elide text in the tab bar
    \since 4.2

    This property controls how items are elided when there is not
    enough space to show them for a given tab bar size.

    By default the value is style dependent.

    \sa QTabWidget::elideMode usesScrollButtons QStyle::SH_TabBar_ElideMode
*/

Qt::TextElideMode QTabBar::elideMode() const
{
    Q_D(const QTabBar);
    return d->elideMode;
}

void QTabBar::setElideMode(Qt::TextElideMode mode)
{
    Q_D(QTabBar);
    d->elideMode = mode;
    d->refresh();
}

/*!
    \property QTabBar::usesScrollButtons
    \brief Whether or not a tab bar should use buttons to scroll tabs when it
    has many tabs.
    \since 4.2

    When there are too many tabs in a tab bar for its size, the tab bar can either choose
    to expand it's size or to add buttons that allow you to scroll through the tabs.

    By default the value is style dependant.

    \sa elideMode QTabWidget::usesScrollButtons QStyle::SH_TabBar_PreferNoArrows
*/
bool QTabBar::usesScrollButtons() const
{
    return d_func()->useScrollButtons;
}

void QTabBar::setUsesScrollButtons(bool useButtons)
{
    Q_D(QTabBar);
    if (d->useScrollButtons == useButtons)
        return;
    d->useScrollButtons = useButtons;
    d->refresh();
}

/*!
    \fn void QTabBar::setCurrentTab(int index)

    Use setCurrentIndex() instead.
*/

/*!
    \fn void QTabBar::selected(int index);

    Use currentChanged() instead.
*/


QT_END_NAMESPACE

#include "moc_qtabbar.cpp"

#endif // QT_NO_TABBAR
