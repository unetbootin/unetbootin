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

#include "q3widgetstack.h"
#include "qlayout.h"
#include "private/qlayoutengine_p.h"
#include "qapplication.h"
#include "qpainter.h"

QT_BEGIN_NAMESPACE

using namespace Qt;

class Q3WidgetStackPrivate {
public:
    class Invisible: public QWidget
    {
    public:
	    Invisible(Q3WidgetStack * parent): QWidget(parent, "qt_invisible_widgetstack")
	    {
                setBackgroundMode(NoBackground);
	    }
	    const char * className() const
	    {
	        return "Q3WidgetStackPrivate::Invisible";
	    }
    protected:
            void paintEvent(QPaintEvent *)
            {
                QPainter(this).eraseRect(rect());
            }
    };

    int nextNegativeID;
    int nextPositiveID;
};



/*!
    \class Q3WidgetStack
    \brief The Q3WidgetStack class provides a stack of widgets of which
    only the top widget is user-visible.

    \compat

    The application programmer can move any widget to the top of the
    stack at any time using raiseWidget(), and add or remove widgets
    using addWidget() and removeWidget(). It is not sufficient to pass
    the widget stack as parent to a widget which should be inserted into
    the widgetstack.

    visibleWidget() is the \e get equivalent of raiseWidget(); it
    returns a pointer to the widget that is currently at the top of
    the stack.

    Q3WidgetStack also provides the ability to manipulate widgets
    through application-specified integer IDs. You can also translate
    from widget pointers to IDs using id() and from IDs to widget
    pointers using widget(). These numeric IDs are unique (per
    Q3WidgetStack, not globally), but Q3WidgetStack does not attach any
    additional meaning to them.

    The default widget stack is frameless, but you can use the usual
    Q3Frame functions (such as setFrameStyle()) to add a frame.

    Q3WidgetStack provides a signal, aboutToShow(), which is emitted
    just before a managed widget is shown.

    \sa Q3TabDialog QTabWidget QTabBar Q3Frame
*/


/*!
  Constructs an empty widget stack.

  The \a parent, \a name and \a f arguments are passed to the Q3Frame
  constructor.
*/
Q3WidgetStack::Q3WidgetStack(QWidget * parent, const char *name, Qt::WindowFlags f)
    : Q3Frame(parent, name, f) //## merge constructors in 4.0
{
    init();
}

void Q3WidgetStack::init()
{
   d = new Q3WidgetStackPrivate();
   d->nextNegativeID = -2;
   d->nextPositiveID = 0;
   dict = new Q3IntDict<QWidget>;
   focusWidgets = 0;
   topWidget = 0;
   invisible = 0;
   invisible = new Q3WidgetStackPrivate::Invisible(this);
   invisible->hide();
}


/*!
    Destroys the object and frees any allocated resources.
*/

Q3WidgetStack::~Q3WidgetStack()
{
    delete focusWidgets;
    delete d;
    delete dict;
}


/*!
    Adds widget \a w to this stack of widgets, with ID \a id.

    If you pass an id \>= 0 this ID is used. If you pass an \a id of
    -1 (the default), the widgets will be numbered automatically. If
    you pass -2 a unique negative integer will be generated. No widget
    has an ID of -1. Returns the ID or -1 on failure (e.g. \a w is 0).

    If you pass an id that is already used, then a unique negative
    integer will be generated to prevent two widgets having the same
    id.

    If \a w already exists in the stack the widget will be removed first.

    If \a w is not a child of this Q3WidgetStack moves it using
    reparent().
*/

int Q3WidgetStack::addWidget(QWidget * w, int id)
{
    if (!w || w == invisible || invisible == 0)
        return -1;

    // prevent duplicates
    removeWidget(w);

    if (id >= 0 && dict->find(id))
        id = -2;
    if (id < -1)
        id = d->nextNegativeID--;
    else if (id == -1)
        id = d->nextPositiveID++;
    else
        d->nextPositiveID = qMax(d->nextPositiveID, id + 1);
        // use id >= 0 as-is

    dict->insert(id, w);

    // preserve existing focus
    QWidget * f = w->focusWidget();
    while(f && f != w)
        f = f->parentWidget();
    if (f) {
        if (!focusWidgets)
            focusWidgets = new Q3PtrDict<QWidget>(17);
        focusWidgets->replace(w, w->focusWidget());
    }

    w->hide();
    if (w->parent() != this)
        w->reparent(this, contentsRect().topLeft(), false);
    w->setGeometry(contentsRect());
    updateGeometry();
    return id;
}


/*!
    Removes widget \a w from this stack of widgets. Does not delete \a
    w. If \a w is the currently visible widget, no other widget is
    substituted.

    \sa visibleWidget() raiseWidget()
*/

void Q3WidgetStack::removeWidget(QWidget * w)
{
    int i;
    if (!w || (i = id(w)) == -1)
        return ;

    dict->take(i);
    if (w == topWidget)
        topWidget = 0;
    if (dict->isEmpty())
        invisible->hide(); // let background shine through again
    updateGeometry();
}


/*!
    Raises the widget with ID \a id to the top of the widget stack.

    \sa visibleWidget()
*/

void Q3WidgetStack::raiseWidget(int id)
{
    if (id == -1)
        return;
    QWidget * w = dict->find(id);
    if (w)
        raiseWidget(w);
}

static bool isChildOf(QWidget* child, QWidget *parent)
{
    if (!child)
        return false;
    QObjectList list = parent->children();
    for (int i = 0; i < list.size(); ++i) {
        QObject *obj = list.at(i);
        if (!obj->isWidgetType())
            continue;
        QWidget *widget = static_cast<QWidget *>(obj);
        if (!widget->isWindow())
            continue;
        if (widget == child || isChildOf(child, widget))
            return true;
    }
    return false;
}

/*!
    \overload

    Raises widget \a w to the top of the widget stack.
*/

void Q3WidgetStack::raiseWidget(QWidget *w)
{
    if (!w || w == invisible || w->parent() != this || w == topWidget)
        return;

    if (id(w) == -1)
        addWidget(w);
    if (!isVisible()) {
        topWidget = w;
        return;
    }

    if (w->maximumSize().width() < invisible->width()
        || w->maximumSize().height() < invisible->height())
        invisible->setBackgroundMode(backgroundMode());
    else if (invisible->backgroundMode() != NoBackground)
        invisible->setBackgroundMode(NoBackground);

    if (invisible->isHidden()) {
        invisible->setGeometry(contentsRect());
        invisible->lower();
        invisible->show();
        QApplication::sendPostedEvents(invisible, QEvent::ShowWindowRequest);
    }

    // try to move focus onto the incoming widget if focus
    // was somewhere on the outgoing widget.
    if (topWidget) {
        QWidget * fw = window()->focusWidget();
        if (topWidget->isAncestorOf(fw)) { // focus was on old page
            // look for the best focus widget we can find
            QWidget *p = w->focusWidget();
            if (!p) {
                // second best == first child widget in the focus chain
                QWidget *i = fw;
                while ((i = i->nextInFocusChain()) != fw) {
                    if (((i->focusPolicy() & Qt::TabFocus) == Qt::TabFocus)
                        && !i->focusProxy() && i->isVisibleTo(w) && i->isEnabled()
                        && w->isAncestorOf(i)) {
                        p = i;
                        break;
                    }
                }
            }
            if (p)
                p->setFocus();
        } else {
            // the focus wasn't on the old page, so we have to ensure focus doesn't go to
            // the widget in the page that last had focus when we show the page again.
            QWidget *oldfw = topWidget->focusWidget();
            if (oldfw)
                oldfw->clearFocus();
        }
    }

    if (isVisible()) {
        emit aboutToShow(w);
        int i = id(w);
        if (i != -1)
            emit aboutToShow(i);
    }

    topWidget = w;

    QObjectList c = children();
    for (int i = 0; i < c.size(); ++i) {
        QObject * o = c.at(i);
        if (o->isWidgetType() && o != w && o != invisible)
            static_cast<QWidget *>(o)->hide();
    }

    w->setGeometry(invisible->geometry());
    w->show();
}

/*!
    \reimp
*/

void Q3WidgetStack::frameChanged()
{
    Q3Frame::frameChanged();
    setChildGeometries();
}


/*!
    \internal
*/

void Q3WidgetStack::setFrameRect(const QRect & r)
{
    // ### this function used to be virtual in QFrame in Qt 3; it is no longer virtual in Qt 4
    Q3Frame::setFrameRect(r);
    setChildGeometries();
}


/*!
    Fixes up the children's geometries.
*/

void Q3WidgetStack::setChildGeometries()
{
    invisible->setGeometry(contentsRect());
    if (topWidget)
        topWidget->setGeometry(invisible->geometry());
}


/*!
    \reimp
*/
void Q3WidgetStack::setVisible(bool visible)
{
    if (visible) {
        //  Reimplemented in order to set the children's geometries
        //  appropriately and to pick the first widget as d->topWidget if no
        //  topwidget was defined
        QObjectList c = children();
        if (!isVisible() && !c.isEmpty()) {
            for (int i = 0; i < c.size(); ++i) {
                QObject * o = c.at(i);
                if (o->isWidgetType()) {
                    if (!topWidget && o != invisible)
                        topWidget = static_cast<QWidget*>(o);
                    if (o == topWidget)
                        static_cast<QWidget *>(o)->show();
                    else
                        static_cast<QWidget *>(o)->hide();
                }
            }
            setChildGeometries();
        }
    }
    Q3Frame::setVisible(visible);
}


/*!
    Returns the widget with ID \a id. Returns 0 if this widget stack
    does not manage a widget with ID \a id.

    \sa id() addWidget()
*/

QWidget * Q3WidgetStack::widget(int id) const
{
    return id != -1 ? dict->find(id) : 0;
}


/*!
    Returns the ID of the \a widget. Returns -1 if \a widget is 0 or
    is not being managed by this widget stack.

    \sa widget() addWidget()
*/

int Q3WidgetStack::id(QWidget * widget) const
{
    if (!widget)
        return -1;

    Q3IntDictIterator<QWidget> it(*dict);
    while (it.current() && it.current() != widget)
        ++it;
    return it.current() == widget ? it.currentKey() : -1;
}


/*!
    Returns the currently visible widget (the one at the top of the
    stack), or 0 if nothing is currently being shown.

    \sa aboutToShow() id() raiseWidget()
*/

QWidget * Q3WidgetStack::visibleWidget() const
{
    return topWidget;
}


/*!
    \fn void Q3WidgetStack::aboutToShow(int id)

    This signal is emitted just before a managed widget is shown if
    that managed widget has an ID != -1. The \a id parameter is the numeric
    ID of the widget.

    If you call visibleWidget() in a slot connected to aboutToShow(),
    the widget it returns is the one that is currently visible, not
    the one that is about to be shown.
*/


/*!
    \fn void Q3WidgetStack::aboutToShow(QWidget *widget)

    \overload

    This signal is emitted just before a managed widget is shown. The
    argument is a pointer to the \a widget.

    If you call visibleWidget() in a slot connected to aboutToShow(),
    the widget returned is the one that is currently visible, not the
    one that is about to be shown.
*/


/*!
    \reimp
*/

void Q3WidgetStack::resizeEvent(QResizeEvent * e)
{
    Q3Frame::resizeEvent(e);
    setChildGeometries();
}


/*!
    \reimp
*/

QSize Q3WidgetStack::sizeHint() const
{
    constPolish();

    QSize size(0, 0);

    Q3IntDictIterator<QWidget> it(*dict);
    QWidget *w;

    while ((w = it.current()) != 0) {
        ++it;
        QSize sh = w->sizeHint();
        if (w->sizePolicy().horData() == QSizePolicy::Ignored)
            sh.rwidth() = 0;
        if (w->sizePolicy().verData() == QSizePolicy::Ignored)
            sh.rheight() = 0;
#ifndef QT_NO_LAYOUT
        size = size.expandedTo(sh).expandedTo(qSmartMinSize(w));
#endif
    }
    if (size.isNull())
        size = QSize(128, 64);
    size += QSize(2*frameWidth(), 2*frameWidth());
    return size;
}


/*!
    \reimp
*/
QSize Q3WidgetStack::minimumSizeHint() const
{
    constPolish();

    QSize size(0, 0);

    Q3IntDictIterator<QWidget> it(*dict);
    QWidget *w;

    while ((w = it.current()) != 0) {
        ++it;
        QSize sh = w->minimumSizeHint();
        if (w->sizePolicy().horData() == QSizePolicy::Ignored)
            sh.rwidth() = 0;
        if (w->sizePolicy().verData() == QSizePolicy::Ignored)
            sh.rheight() = 0;
#ifndef QT_NO_LAYOUT
        size = size.expandedTo(sh).expandedTo(w->minimumSize());
#endif
    }
    if (size.isNull())
        size = QSize(64, 32);
    size += QSize(2*frameWidth(), 2*frameWidth());
    return size;
}

/*!
    \reimp
*/
void Q3WidgetStack::childEvent(QChildEvent *e)
{
    if (e->child()->isWidgetType() && e->removed())
        removeWidget((QWidget *) e->child());
}


/*!
    \reimp
*/
bool Q3WidgetStack::event(QEvent* e)
{
    if (e->type() == QEvent::LayoutRequest || e->type() == QEvent::LayoutHint )
        updateGeometry(); // propgate layout hints to parent
    return Q3Frame::event(e);
}

QT_END_NAMESPACE
