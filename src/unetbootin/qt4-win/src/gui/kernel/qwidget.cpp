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

#include "qapplication.h"
#include "qapplication_p.h"
#include "qbrush.h"
#include "qcursor.h"
#include "qdesktopwidget.h"
#include "qevent.h"
#include "qhash.h"
#include "qlayout.h"
#include "qmenu.h"
#include "qmetaobject.h"
#include "qpixmap.h"
#include "qpointer.h"
#include "qstack.h"
#include "qstyle.h"
#include "qstylefactory.h"
#include "qvariant.h"
#include "qwidget.h"
#include "qstyleoption.h"
#ifndef QT_NO_ACCESSIBILITY
# include "qaccessible.h"
#endif
#if defined(Q_WS_WIN)
# include "qt_windows.h"
#endif
#ifdef Q_WS_MAC
# include "qt_mac_p.h"
#endif
#if defined(Q_WS_QWS)
# include "qwsmanager_qws.h"
# include "qpaintengine.h" // for PorterDuff
# include "private/qwindowsurface_qws_p.h"
#endif
#include "qpainter.h"
#include "qtooltip.h"
#include "qwhatsthis.h"
#include "qdebug.h"
#include "private/qstylesheetstyle_p.h"
#include "private/qstyle_p.h"
#include "qinputcontext.h"
#include "qfileinfo.h"

#if defined (Q_WS_WIN)
# include <private/qwininputcontext_p.h>
#endif

#if defined(Q_WS_X11)
# include <private/qpaintengine_x11_p.h>
# include "qx11info_x11.h"
#endif

#include <private/qwindowsurface_p.h>
#ifndef Q_WS_MAC
# include <private/qbackingstore_p.h>
#else
# include <private/qpaintengine_mac_p.h>
# include <private/qpaintengine_raster_p.h>
#endif

#include "qwidget_p.h"
#include "qaction_p.h"
#include "qlayout_p.h"
#include "QtGui/qgraphicsproxywidget.h"
#include "QtGui/qgraphicsscene.h"
#include "private/qgraphicsproxywidget_p.h"
#include "QtGui/qabstractscrollarea.h"
#include "private/qabstractscrollarea_p.h"

// widget/widget data creation count
//#define QWIDGET_EXTRA_DEBUG
//#define ALIEN_DEBUG

QT_BEGIN_NAMESPACE

#if !defined(Q_WS_QWS) && !defined(Q_WS_MAC)
static bool qt_enable_backingstore = true;
#endif
#ifdef Q_WS_X11
// for compatibility with Qt 4.0
Q_GUI_EXPORT void qt_x11_set_global_double_buffer(bool enable)
{
    qt_enable_backingstore = enable;
}
#endif

static inline bool qRectIntersects(const QRect &r1, const QRect &r2)
{
    return (qMax(r1.left(), r2.left()) <= qMin(r1.right(), r2.right()) &&
            qMax(r1.top(), r2.top()) <= qMin(r1.bottom(), r2.bottom()));
}

#ifdef Q_WS_MAC
#  define QT_NO_PAINT_DEBUG
#endif

extern bool qt_sendSpontaneousEvent(QObject*, QEvent*); // qapplication_xxx.cpp

#ifndef QT_NO_PAINT_DEBUG
static bool qt_flushPaint(QWidget *widget, const QRegion &toBePainted)
{
    static int flush_paint = qgetenv("QT_FLUSH_PAINT").toInt();
    if (!flush_paint)
        return false;

    QWidgetBackingStore::qt_showYellowThing(widget, toBePainted, flush_paint * 10, true);

    return true;
}
#endif

QWidgetPrivate::QWidgetPrivate(int version) :
        QObjectPrivate(version), extra(0), focus_child(0)
        ,layout(0), widgetItem(0)
        ,leftmargin(0), topmargin(0), rightmargin(0), bottommargin(0)
        ,leftLayoutItemMargin(0), topLayoutItemMargin(0), rightLayoutItemMargin(0)
        ,bottomLayoutItemMargin(0)
        ,fg_role(QPalette::NoRole)
        ,bg_role(QPalette::NoRole)
        ,hd(0)
#ifdef Q_RATE_LIMIT_PAINTING
        ,timerId(-1)
#endif
#if defined(Q_WS_X11)
        ,picture(0)
#endif
#ifdef Q_WS_MAC
        ,isGLWidget(0)
#endif
        ,polished(0)

        , size_policy(QSizePolicy::Preferred, QSizePolicy::Preferred)
        , redirectDev(0)
{
    if (!qApp) {
        qFatal("QWidget: Must construct a QApplication before a QPaintDevice");
        return;
    }

    if (version != QObjectPrivateVersion)
        qFatal("Cannot mix incompatible Qt libraries");

    dirtyOpaqueChildren = true;

    isWidget = true;
    memset(high_attributes, 0, sizeof(high_attributes));
#ifdef QWIDGET_EXTRA_DEBUG
    static int count = 0;
    qDebug() << "widgets" << ++count;
#endif
}


QWidgetPrivate::~QWidgetPrivate()
{
#ifdef Q_WIDGET_USE_DIRTYLIST
    if (!dirty.isEmpty()) {
        Q_Q(QWidget);

        QWidgetBackingStore *bs = maybeBackingStore();
        if (bs)
            bs->removeDirtyWidget(q);
    }
#endif

    if (widgetItem)
        widgetItem->wid = 0;

    if (extra)
        deleteExtra();
}


/*!
    \internal
    This is an internal function, you should never call this.

    This function is called to focus associated input context. The
    code intends to eliminate duplicate focus for the context even if
    the context is shared between widgets

    \sa QInputContext::setFocus()
 */
void QWidgetPrivate::focusInputContext()
{
#ifndef QT_NO_IM
    Q_Q(QWidget);
    QInputContext *qic = q->inputContext();
    if (qic) {
        if(qic->focusWidget() != q)
            qic->setFocusWidget(q);
    }
#endif // QT_NO_IM
}

/*!
    \internal
*/
void QWidgetPrivate::scrollChildren(int dx, int dy)
{
    Q_Q(QWidget);
    if (q->children().size() > 0) {        // scroll children
        QPoint pd(dx, dy);
        QObjectList childObjects = q->children();
        for (int i = 0; i < childObjects.size(); ++i) { // move all children
            QWidget *w = qobject_cast<QWidget*>(childObjects.at(i));
            if (w && !w->isWindow()) {
                QPoint oldp = w->pos();
                QRect  r(w->pos() + pd, w->size());
                w->data->crect = r;
#ifndef Q_WS_QWS
                if (w->testAttribute(Qt::WA_WState_Created))
                    w->d_func()->setWSGeometry();
#endif
                QMoveEvent e(r.topLeft(), oldp);
                QApplication::sendEvent(w, &e);
            }
        }
    }
}


/*!
    This function returns the QInputContext for this widget. By
    default the input context is inherited from the widgets
    parent. For toplevels it is inherited from QApplication.

    You can override this and set a special input context for this
    widget by using the setInputContext() method.

    \sa setInputContext()
*/
QInputContext *QWidget::inputContext()
{
    Q_D(QWidget);
    if (!testAttribute(Qt::WA_InputMethodEnabled))
        return 0;

#ifndef QT_NO_IM
    if (d->ic)
        return d->ic;
#endif
    return qApp->inputContext();
}

/*!
  This function sets the input context \a context
  on this widget.

  \sa inputContext()
*/
void QWidget::setInputContext(QInputContext *context)
{
    Q_D(QWidget);
    if (!testAttribute(Qt::WA_InputMethodEnabled))
        return;
#ifndef QT_NO_IM
    if (d->ic)
        delete d->ic;
    d->ic = context;
#endif
}


/*!
    This function can be called on the widget that currently has focus
    to reset the input method operating on it.

    \sa QInputContext, QInputContext::reset()
*/
void QWidget::resetInputContext()
{
    if (!hasFocus())
        return;
#ifndef QT_NO_IM
    QInputContext *qic = this->inputContext();
    if( qic )
        qic->reset();
#endif // QT_NO_IM
}

#ifdef QT_KEYPAD_NAVIGATION
QPointer<QWidget> QWidgetPrivate::editingWidget;

/*!
    Returns true if this widget currently has edit focus; otherwise false.

    This feature is only available in Qt for Embedded Linux.

    \sa setEditFocus(), QApplication::keypadNavigationEnabled()
*/
bool QWidget::hasEditFocus() const
{
    const QWidget* w = this;
    while (w->d_func()->extra && w->d_func()->extra->focus_proxy)
        w = w->d_func()->extra->focus_proxy;
    return QWidgetPrivate::editingWidget == w;
}

/*!
    \fn void QWidget::setEditFocus(bool enable)

    If \a enable is true, make this widget have edit focus, in which
    case Qt::Key_Up and Qt::Key_Down will be delivered to the widget
    normally; otherwise, Qt::Key_Up and Qt::Key_Down are used to
    change focus.

    This feature is only available in Qt for Embedded Linux.

    \sa hasEditFocus(), QApplication::keypadNavigationEnabled()
*/
void QWidget::setEditFocus(bool on)
{
    QWidget *f = this;
    while (f->d_func()->extra && f->d_func()->extra->focus_proxy)
        f = f->d_func()->extra->focus_proxy;

    if (QWidgetPrivate::editingWidget && QWidgetPrivate::editingWidget != f)
        QWidgetPrivate::editingWidget->setEditFocus(false);

    if (on && !f->hasFocus())
        f->setFocus();

    if ((!on && !QWidgetPrivate::editingWidget)
        || (on && QWidgetPrivate::editingWidget == f)) {
        return;
    }

    if (!on && QWidgetPrivate::editingWidget == f) {
        QWidgetPrivate::editingWidget = 0;
        QEvent event(QEvent::LeaveEditFocus);
        QApplication::sendEvent(f, &event);
        QApplication::sendEvent(f->style(), &event);
    } else if (on) {
        QWidgetPrivate::editingWidget = f;
        QEvent event(QEvent::EnterEditFocus);
        QApplication::sendEvent(f, &event);
        QApplication::sendEvent(f->style(), &event);
    }
}
#endif

/*!
    \property QWidget::autoFillBackground
    \brief whether the widget background is filled automatically
    \since 4.1

    If enabled, this will cause Qt to fill the background
    using the widget's background role before invoking
    the paint event. The background role is defined by the widget's
    \l{palette}.

    In addition, Windows are always filled with QPalette::Window,
    unless the WA_OpaquePaintEvent or WA_NoSystemBackground
    attributes are set.

    \warning Use this property with caution in conjunction with
    \l{Qt Style Sheets}. When a widget has a style sheet with a valid
    background or a border-image, this property is automatically
    disabled.

    \sa Qt::WA_OpaquePaintEvent, Qt::WA_NoSystemBackground
*/
bool QWidget::autoFillBackground() const
{
    Q_D(const QWidget);
    return d->extra && d->extra->autoFillBackground;
}

void QWidget::setAutoFillBackground(bool enabled)
{
    Q_D(QWidget);
    if (!d->extra)
        d->createExtra();
    if (d->extra->autoFillBackground == enabled)
        return;

    d->extra->autoFillBackground = enabled;
    d->updateIsOpaque();
    update();
    d->updateIsOpaque();
}

/*!
    \class QWidget
    \brief The QWidget class is the base class of all user interface objects.

    \ingroup basicwidgets
    \mainclass

    The widget is the atom of the user interface: it receives mouse,
    keyboard and other events from the window system, and paints a
    representation of itself on the screen. Every widget is
    rectangular, and they are sorted in a Z-order. A widget is
    clipped by its parent and by the widgets in front of it.

    A widget that isn't embedded in a parent widget is called a
    window. Usually, windows have a frame and a title bar, although
    it is also possible to create windows without such decoration using
    suitable \l{Qt::WindowFlags}{window flags}). In Qt,
    QMainWindow and the various subclasses of QDialog are the most
    common window types.

    Every widget's constructor accepts one or two standard arguments:
    \list 1
    \i \c{QWidget *parent = 0} is the parent of the new widget.
    If it is 0 (the default), the new widget will be a window.
    If not, it will be a child of \e parent, and be constrained by \e
    parent's geometry (unless you specify Qt::Window as
    window flag).
    \i \c{Qt::WindowFlags f = 0} (where available) sets the window flags; the
    default is suitable for almost all widgets, but to get, for
    example, a window without a window system frame, you
    must use special flags.
    \endlist

    QWidget has many member functions, but some of them have little
    direct functionality; for example, QWidget has a font property,
    but never uses this itself. There are many subclasses which
    provide real functionality, such as QLabel, QPushButton, QListWidget,
    and QTabWidget.

    \section1 Top-Level and Child Widgets

    A widget without a parent widget is always an independent window
    (top-level widget). For these widgets, setWindowTitle() and
    setWindowIcon() set the title bar and icon respectively.

    Non-window widgets are child widgets, and are displayed within
    their parent widgets. Most widgets in Qt are mainly useful as child
    widgets. For example, it is possible to display a button as a
    top-level window, but most people prefer to put their buttons
    inside other widgets, such as QDialog.

    \image parent-child-widgets.png A parent widget containing various child widgets.

    The above diagram shows a QGroupBox widget being used to hold various child
    widgets in a layout provided by QGridLayout. The QLabel child widgets have
    been outlined to indicate their full sizes.

    If you want to use a QWidget to hold child widgets you will
    usually want to add a layout to the parent QWidget. See
    \l{Layout Classes} for more information about these.

    \section1 Composite Widgets

    When a widgets is used as a container to group a number of child widgets,
    it is known as a composite widget. These can be created by constructing
    a widget with the required visual properties - a QFrame, for example - and
    adding child widgets to it, usually managed by a layout. The above diagram
    shows such a composite widget that was created using \l{Qt Designer}.

    Composite widgets can also be created by subclassing a standard widget,
    such as QWidget or QFrame, and adding the necessary layout and child widgets
    in the constructor of the subclass. Many of the
    \l{Qt Examples}{examples provided with Qt} use this approach, and it is
    also covered in the \l{Qt Tutorial}.

    \section1 Custom Widgets and Painting

    Since QWidget is a subclass of QPaintDevice, subclasses can be used to
    display custom content that is composed using a series of painting
    operations with an instance of the QPainter class. This approach
    contrasts with the canvas-style approach used by the
    \l{Graphics View}{Graphics View Framework} where items are added to a
    scene by the application and are rendered by the framework itself.

    Each widget performs all painting operations from within its
    paintEvent() function. This is called whenever the widget needs to be
    redrawn, either as a result of some external change or when requested
    by the application.

    The \l{widgets/analogclock}{Analog Clock example} shows how a simple
    widget can handle paint events.

    \section1 Size Hints and Size Policies

    When implementing a new widget, it is almost always useful to
    reimplement sizeHint() to provide a reasonable default size for the
    widget and to set the correct size policy with setSizePolicy().

    By default, composite widgets which do not provide a size hint will
    be sized according to the space requirements of their child widgets.

    The size policy lets you supply good default behavior for the layout
    management system, so that other widgets can contain and manage
    yours easily. The default size policy indicates that the size hint
    represents the preferred size of the widget, and this is often good
    enough for many widgets.

    \section1 Events

    Widgets respond to events that are typically caused by user actions.
    Qt delivers events to widgets by calling specific event handler functions
    with instances of QEvent subclasses containing information about each
    event.

    If your widget only contains child widgets, you probably do not need to
    implement any event handlers. If you want to detect a mouse click in
    a child widget call the child's underMouse() function inside the
    widget's mousePressEvent().

    The \l{widgets/scribble}{Scribble example} implements
    a wider set of events to handle mouse movement, button presses, and
    window resizing.

    You will need to supply the behavior and content for your own widgets,
    but here is a brief overview of the events that are relevant to
    QWidget, starting with the most common ones:

    \list

    \i paintEvent() is called whenever the widget needs to be
    repainted. Every widget which displays custom content must implement
    it. Painting using a QPainter can only take place in a paintEvent()
    or a function called by a paintEvent().

    \i resizeEvent() is called when the widget has been resized.

    \i mousePressEvent() is called when a mouse button is pressed when
    the mouse is inside it, or when it has grabbed the mouse using
    grabMouse().

    \i mouseReleaseEvent() is called when a mouse button is released.
    A widget receives mouse release events when it has received the
    corresponding mouse press event. This means that if the user
    presses the mouse inside \e your widget, then drags the mouse to
    somewhere else before releasing the mouse button, \e your widget
    receives the release event. There is one exception: if a popup
    menu appears while the mouse button is held down, this popup
    immediately steals the mouse events.

    \i mouseDoubleClickEvent() is called when the user double clicks
    in the widget. If the user double-clicks, the widget receives a
    mouse press event, a mouse release event and finally this event
    instead of a second mouse press event.
    (Some mouse move events may also be received if the user doesn't
    hold the mouse steady during this operation.)
    It is \e{not possible} to distinguish a click from a double click
    until the second click arrives. (This is one reason why most GUI
    books recommend that double clicks be an extension of single clicks,
    rather than trigger a different action.)

    \endlist

    Widgets that accept keyboard input need to reimplement a few more
    event handlers:

    \list

    \i keyPressEvent() is called whenever a key is pressed, and again
    when a key has been held down long enough for it to auto-repeat.
    Note that the \key Tab and \key Shift+Tab keys are only passed to
    the widget if they are not used by the focus-change mechanisms.
    To force those keys to be processed by your widget, you must
    reimplement QWidget::event().

    \i focusInEvent() is called when the widget gains keyboard focus
    (assuming you have called setFocusPolicy()). Well written widgets
    indicate that they own the keyboard focus in a clear but discreet
    way.

    \i focusOutEvent() is called when the widget loses keyboard focus.

    \endlist

    Some widgets will also need to reimplement some of the less common
    event handlers:

    \list

    \i mouseMoveEvent() is called whenever the mouse moves while a
    button is held down. This can be useful during drag and drop
    operations. If you call setMouseTracking(true), you get mouse move
    events even when no buttons are held down. (See also the guide to
    \l{Drag and Drop}.)

    \i keyReleaseEvent() is called whenever a key is released, and also
    while it is held down if the key is auto-repeating. In that case
    the widget receives a pair of key release and key press events
    for every repeat. Note that the \key Tab and \key Shift+Tab keys are
    only passed to the widget if they are not used by the focus-change
    mechanisms. To force those keys to be processed by your widget, you
    must reimplement QWidget::event().

    \i wheelEvent() is called whenever the user turns the mouse wheel
    while the widget has the focus.

    \i enterEvent() is called when the mouse enters the widget's screen
    space. (This excludes screen space owned by any children of the
    widget.)

    \i leaveEvent() is called when the mouse leaves the widget's screen
    space. Note that if the mouse enters a child widget it will not
    cause a leaveEvent.

    \i moveEvent() is called when the widget has been moved relative to its
    parent.

    \i closeEvent() is called when the user closes the widget (or when
    close() is called).

    \endlist

    There are also some rather obscure events described in the QEvent::Type
    documentation. You need to reimplement event() directly to handle
    these.
    The default implementation of event() handles \key Tab and \key Shift+Tab
    (to move the keyboard focus), and passes on most other events to
    one of the more specialized handlers above.

    Events and the mechanism used to deliver them are covered in the
    \l{Events and Event Filters} document.

    \section1 Groups of Functions and Properties

    \table
    \header \i Context \i Functions and Properties

    \row \i Window functions \i
        show(),
        hide(),
        raise(),
        lower(),
        close().

    \row \i Top-level windows \i
        \l windowModified, \l windowTitle, \l windowIcon, \l windowIconText,
        \l isActiveWindow, activateWindow(), \l minimized, showMinimized(),
        \l maximized, showMaximized(), \l fullScreen, showFullScreen(),
        showNormal().

    \row \i Window contents \i
        update(),
        repaint(),
        scroll().

    \row \i Geometry \i
        \l pos, x(), y(), \l rect, \l size, width(), height(), move(), resize(),
        \l sizePolicy, sizeHint(), minimumSizeHint(),
        updateGeometry(), layout(),
        \l frameGeometry, \l geometry, \l childrenRect, \l childrenRegion,
        adjustSize(),
        mapFromGlobal(), mapToGlobal(),
        mapFromParent(), mapToParent(),
        \l maximumSize, \l minimumSize, \l sizeIncrement,
        \l baseSize, setFixedSize()

    \row \i Mode \i
        \l visible, isVisibleTo(),
        \l enabled, isEnabledTo(),
        \l modal,
        isWindow(),
        \l mouseTracking,
        \l updatesEnabled,
        visibleRegion().

    \row \i Look and feel \i
        style(),
        setStyle(),
        \l styleSheet,
        \l cursor,
        \l font,
        \l palette,
        backgroundRole(), setBackgroundRole(),
        fontInfo(), fontMetrics().

    \row \i Keyboard focus functions \i
        \l focus, \l focusPolicy,
        setFocus(), clearFocus(), setTabOrder(), setFocusProxy(),
        focusNextChild(), focusPreviousChild().

    \row \i Mouse and keyboard grabbing \i
        grabMouse(), releaseMouse(),
        grabKeyboard(), releaseKeyboard(),
        mouseGrabber(), keyboardGrabber().

    \row \i Event handlers \i
        event(),
        mousePressEvent(),
        mouseReleaseEvent(),
        mouseDoubleClickEvent(),
        mouseMoveEvent(),
        keyPressEvent(),
        keyReleaseEvent(),
        focusInEvent(),
        focusOutEvent(),
        wheelEvent(),
        enterEvent(),
        leaveEvent(),
        paintEvent(),
        moveEvent(),
        resizeEvent(),
        closeEvent(),
        dragEnterEvent(),
        dragMoveEvent(),
        dragLeaveEvent(),
        dropEvent(),
        childEvent(),
        showEvent(),
        hideEvent(),
        customEvent().
        changeEvent(),

    \row \i System functions \i
        parentWidget(), window(), setParent(), winId(),
        find(), metric().

    \row \i Interactive help \i
        setToolTip(), setWhatsThis()

    \endtable

    \section1 Widget Style Sheets

    In addition to the standard widget styles for each platform, widgets can
    also be styled according to rules specified in a \l{styleSheet}{style sheet}.
    This feature enables you to customize the appearance of specific widgets
    to provide visual cues to users about their purpose; for example, a button
    could be styled in a particular way to indicate that it performs a
    destructive action.

    The use of widgets style sheets is described in more detail in
    \l{Qt Style Sheets}.

    \section1 Transparency and Double Buffering

    From Qt 4.0, QWidget automatically double-buffers its painting, so
    there's no need to write double-buffering code in paintEvent() to
    avoid flicker. Additionally, it became possible for widgets to
    propagate their contents to children, in order to enable transparency
    effects, by setting the Qt::WA_ContentsPropagated widget attribute -
    this is now deprecated in Qt 4.1.

    In Qt 4.1, the contents of parent widgets are propagated by default
    to each of their children. Custom widgets can be written to take
    advantage of this feature by only updating irregular regions (to
    create non-rectangular child widgets), or by using painting with
    colors that have less than the full alpha component. The following
    diagram shows how attributes and properties of a custom widget can
    be fine-tuned to achieve different effects.

    \image propagation-custom.png

    In the above diagram, a semi-transparent rectangular child widget with
    an area removed is constructed and added to a parent widget (a QLabel
    showing a pixmap) then different properties and widget attributes are
    set to achieve different effects:

    \list
    \o The left widget has no additional properties or widget attributes
       set. This default state suits most custom widgets that use transparency
       or are irregularly-shaped, and that do not paint over their entire area
       with an opaque brush.
    \o The center widget has the \l autoFillBackground property set.
       This property is used with custom widgets that rely on the widget
       to supply a default background, and do not paint over their entire area
       with an opaque brush.
    \o The right widget has the Qt::WA_OpaquePaintEvent widget attribute
       set. This indicates that the widget will paint over its entire area with
       opaque colors. The widget's area will initially be \e{uninitialized}
       (represented in the diagram by a red diagonal grid pattern that shines
       through the overpainted area). This is useful for widgets that need to
       paint their own specialized contents quickly and that do not need a
       default filled background.
    \endlist

    For rapidly updating custom widgets with simple background colors, such
    as real-time plotting or graphing widgets, it is better to define a
    suitable background color (using setBackgroundRole() with the
    QPalette::Window role), set the \l autoFillBackground property, and only
    implement the necessary drawing functionality in the widget's paintEvent().

    For rapidly updating custom widgets that constantly paint over their entire
    areas with opaque content, such as video streaming widgets, it is
    better to set the widget's Qt::WA_OpaquePaintEvent, avoiding any unnecessary
    overhead associated with repainting the widget's background.

    If a widget has both the Qt::WA_OpaquePaintEvent widget attribute \e{and}
    the \l autoFillBackground property set, the Qt::WA_OpaquePaintEvent
    attribute takes precedence. You should choose just one of these
    depending on your requirements.

    In Qt 4.1, the contents of parent widgets are also propagated to standard
    Qt widgets. This can lead to some unexpected results if the parent widget
    is decorated in a non-standard way, as shown in the diagram below.

    \image propagation-standard.png

    The scope for customizing the painting behavior of standard Qt widgets,
    without resorting to subclassing, is slightly less than that possible for
    custom widgets. Usually, the desired appearance of a standard widget can be
    achieved by setting its \l autoFillBackground property.

    \sa QEvent, QPainter, QGridLayout, QBoxLayout
*/

QWidgetMapper *QWidgetPrivate::mapper = 0;                // widget with wid
QWidgetSet *QWidgetPrivate::uncreatedWidgets = 0;         // widgets with no wid


/*****************************************************************************
  QWidget utility functions
 *****************************************************************************/


static QFont qt_naturalWidgetFont(QWidget* w) {
    QFont naturalfont = QApplication::font(w);
    if (w->parentWidget() && !w->testAttribute(Qt::WA_StyleSheet)
        && (!w->isWindow() || w->testAttribute(Qt::WA_WindowPropagation))) {
        if (!naturalfont.isCopyOf(QApplication::font()))
            naturalfont = naturalfont.resolve(w->parentWidget()->font());
        else
            naturalfont = w->parentWidget()->font();
    }
    naturalfont.resolve(0);
    return naturalfont;
}

static QPalette qt_naturalWidgetPalette(QWidget* w) {
    QPalette naturalpalette = QApplication::palette(w);
    if (w->parentWidget() && !w->testAttribute(Qt::WA_StyleSheet)
        && (!w->isWindow() || w->testAttribute(Qt::WA_WindowPropagation))) {
        if (!naturalpalette.isCopyOf(QApplication::palette()))
            naturalpalette = naturalpalette.resolve(w->parentWidget()->palette());
        else
            naturalpalette = w->parentWidget()->palette();
    }
    naturalpalette.resolve(0);
    return naturalpalette;
}

QRegion qt_dirtyRegion(QWidget *widget, bool dirtyInBackingStore)
{
    if (!widget)
        return QRegion();

    Q_UNUSED(dirtyInBackingStore); // Not used on all platforms :-)
#ifndef Q_WS_MAC
    if (dirtyInBackingStore) {
        QWidget *tlw = widget->window();
        QWidgetBackingStore *bs = tlw->d_func()->maybeBackingStore();
        if (!bs)
            return QRegion();
#ifdef Q_WS_QWS
        const QRect tlwRect = tlw->frameGeometry();
#else
        const QRect tlwRect = tlw->geometry();
#endif
        const QRect surfaceRect = bs->windowSurface->geometry();
        if (surfaceRect != tlwRect && surfaceRect.size() != tlwRect.size())
            return QRect(QPoint(), tlwRect.size());
#ifdef Q_WS_QWS
        return bs->dirtyOnScreen;
#else
        return bs->dirty;
#endif
    }
#endif

#if defined(Q_WS_X11) || defined(Q_WS_WIN) || defined(Q_WS_MAC)
    return widget->d_func()->dirtyOnScreen;
#elif defined(Q_WS_QWS)
    if (QWindowSurface *surface = widget->windowSurface())
        return static_cast<QWSWindowSurface *>(surface)->dirtyRegion();
#endif
    // Optimize for other platforms as well.
    return widget->rect();
}

/*****************************************************************************
  QWidget member functions
 *****************************************************************************/

/*
    Widget state flags:
  \list
  \i Qt::WA_WState_Created The widget has a valid winId().
  \i Qt::WA_WState_Visible The widget is currently visible.
  \i Qt::WA_WState_Hidden The widget is hidden, i.e. it won't
  become visible unless you call show() on it. Qt::WA_WState_Hidden
  implies !Qt::WA_WState_Visible.
  \i Qt::WA_WState_CompressKeys Compress keyboard events.
  \i Qt::WA_WState_BlockUpdates Repaints and updates are disabled.
  \i Qt::WA_WState_InPaintEvent Currently processing a paint event.
  \i Qt::WA_WState_Reparented The widget has been reparented.
  \i Qt::WA_WState_ConfigPending A configuration (resize/move) event is pending.
  \i Qt::WA_WState_DND (Deprecated) The widget supports drag and drop, see setAcceptDrops().
  \endlist
*/

/*!
    Constructs a widget which is a child of \a parent, with  widget
    flags set to \a f.

    If \a parent is 0, the new widget becomes a window. If
    \a parent is another widget, this widget becomes a child window
    inside \a parent. The new widget is deleted when its \a parent is
    deleted.

    The widget flags argument, \a f, is normally 0, but it can be set
    to customize the frame of a window (i.e. \a
    parent must be 0). To customize the frame, use a value composed
    from the bitwise OR of any of the \l{Qt::WindowFlags}{window flags}.

    If you add a child widget to an already visible widget you must
    explicitly show the child to make it visible.

    Note that the X11 version of Qt may not be able to deliver all
    combinations of style flags on all systems. This is because on
    X11, Qt can only ask the window manager, and the window manager
    can override the application's settings. On Windows, Qt can set
    whatever flags you want.

    \sa windowFlags
*/

QWidget::QWidget(QWidget *parent, Qt::WindowFlags f)
    : QObject(*new QWidgetPrivate, 0), QPaintDevice()
{
    d_func()->init(parent, f);
}

#ifdef QT3_SUPPORT
/*!
    \overload
    \obsolete
 */
QWidget::QWidget(QWidget *parent, const char *name, Qt::WindowFlags f)
    : QObject(*new QWidgetPrivate, 0), QPaintDevice()
{
    d_func()->init(parent , f);
    setObjectName(QString::fromAscii(name));
}
#endif

/*! \internal
*/
QWidget::QWidget(QWidgetPrivate &dd, QWidget* parent, Qt::WindowFlags f)
    : QObject(dd, 0), QPaintDevice()
{
    d_func()->init(parent, f);
}

/*!
    \internal
*/
int QWidget::devType() const
{
    return QInternal::Widget;
}


//### w is a "this" ptr, passed as a param because QWorkspace needs special logic
void QWidgetPrivate::adjustFlags(Qt::WindowFlags &flags, QWidget *w)
{
    bool customize =  (flags & (Qt::CustomizeWindowHint
            | Qt::FramelessWindowHint
            | Qt::WindowTitleHint
            | Qt::WindowSystemMenuHint
            | Qt::WindowMinimizeButtonHint
            | Qt::WindowMaximizeButtonHint
            | Qt::WindowContextHelpButtonHint));

    uint type = (flags & Qt::WindowType_Mask);

    if ((type == Qt::Widget || type == Qt::SubWindow) && w && !w->parent()) {
        type = Qt::Window;
        flags |= Qt::Window;
    }

    if (customize)
        ;
    else if (type == Qt::Dialog || type == Qt::Sheet)
#ifndef Q_OS_WINCE
        flags |= Qt::WindowTitleHint | Qt::WindowSystemMenuHint | Qt::WindowContextHelpButtonHint;
#else
        flags |= Qt::WindowTitleHint | Qt::WindowSystemMenuHint;
#endif
    else if (type == Qt::Tool)
        flags |= Qt::WindowTitleHint | Qt::WindowSystemMenuHint;
    else
        flags |= Qt::WindowTitleHint | Qt::WindowSystemMenuHint | Qt::WindowMinimizeButtonHint | Qt::WindowMaximizeButtonHint;


}

void QWidgetPrivate::init(QWidget *parentWidget, Qt::WindowFlags f)
{
    Q_Q(QWidget);
    if (qApp->type() == QApplication::Tty)
        qFatal("QWidget: Cannot create a QWidget when no GUI is being used");

    Q_ASSERT(uncreatedWidgets);
    uncreatedWidgets->insert(q);

    QWidget *desktopWidget = 0;
    if (parentWidget && parentWidget->windowType() == Qt::Desktop) {
        desktopWidget = parentWidget;
        parentWidget = 0;
    }

    q->data = &data;

#ifndef QT_NO_THREAD
    if (!q->parent()) {
        Q_ASSERT_X(q->thread() == qApp->thread(), "QWidget",
                   "Widgets must be created in the GUI thread.");
    }
#endif

#if defined(Q_WS_X11)
    if (desktopWidget) {
        // make sure the widget is created on the same screen as the
        // programmer specified desktop widget
        xinfo = desktopWidget->d_func()->xinfo;
    }
#else
    Q_UNUSED(desktopWidget);
#endif

    data.fstrut_dirty = true;

    data.winid = 0;
    data.widget_attributes = 0;
    data.window_flags = f;
    data.window_state = 0;
    data.focus_policy = 0;
    data.context_menu_policy = Qt::DefaultContextMenu;
    data.window_modality = Qt::NonModal;

    data.sizehint_forced = 0;
    data.is_closing = 0;
    data.in_show = 0;
    data.in_set_window_state = 0;
    data.in_destructor = false;

    // Widgets with Qt::MSWindowsOwnDC (typically QGLWidget) must have a window handle.
    if (f & Qt::MSWindowsOwnDC)
        q->setAttribute(Qt::WA_NativeWindow);

#ifdef Q_OS_WINCE
    data.window_state_internal = 0;
#endif

    q->setAttribute(Qt::WA_QuitOnClose); // might be cleared in create()

    q->setAttribute(Qt::WA_WState_Hidden);

    //give potential windows a bigger "pre-initial" size; create_sys() will give them a new size later
    data.crect = parentWidget ? QRect(0,0,100,30) : QRect(0,0,640,480);

    focus_next = focus_prev = q;

    if ((f & Qt::WindowType_Mask) == Qt::Desktop)
        q->create();
    else if (parentWidget)
        q->setParent(parentWidget, data.window_flags);
    else {
        adjustFlags(data.window_flags, q);
        resolveLayoutDirection();
    }
#if defined(Q_WS_X11)
    data.fnt.x11SetScreen(xinfo.screen());
#endif // Q_WS_X11

    q->setAttribute(Qt::WA_PendingMoveEvent);
    q->setAttribute(Qt::WA_PendingResizeEvent);

    if (++QWidgetPrivate::instanceCounter > QWidgetPrivate::maxInstances)
        QWidgetPrivate::maxInstances = QWidgetPrivate::instanceCounter;

    if (QApplicationPrivate::app_compile_version < 0x040200
        || QApplicationPrivate::testAttribute(Qt::AA_ImmediateWidgetCreation))
        q->create();


    QEvent e(QEvent::Create);
    QApplication::sendEvent(q, &e);
    QApplication::postEvent(q, new QEvent(QEvent::PolishRequest));

    extraPaintEngine = 0;
}



void QWidgetPrivate::createRecursively()
{
    Q_Q(QWidget);
    q->create(0, true, true);
    for (int i = 0; i < children.size(); ++i) {
        QWidget *child = qobject_cast<QWidget *>(children.at(i));
        if (child && !child->isHidden() && !child->isWindow() && !child->testAttribute(Qt::WA_WState_Created))
            child->d_func()->createRecursively();
    }
}




/*!
    Creates a new widget window if \a window is 0, otherwise sets the
    widget's window to \a window.

    Initializes the window (sets the geometry etc.) if \a
    initializeWindow is true. If \a initializeWindow is false, no
    initialization is performed. This parameter only makes sense if \a
    window is a valid window.

    Destroys the old window if \a destroyOldWindow is true. If \a
    destroyOldWindow is false, you are responsible for destroying the
    window yourself (using platform native code).

    The QWidget constructor calls create(0,true,true) to create a
    window for this widget.
*/

void QWidget::create(WId window, bool initializeWindow, bool destroyOldWindow)
{
    Q_D(QWidget);
    if (testAttribute(Qt::WA_WState_Created) && window == 0 && internalWinId())
        return;

    if (d->data.in_destructor)
        return;

    Qt::WindowType type = windowType();
    Qt::WindowFlags &flags = data->window_flags;

    if ((type == Qt::Widget || type == Qt::SubWindow) && !parentWidget()) {
        type = Qt::Window;
        flags |= Qt::Window;
    }

    if (QWidget *parent = parentWidget()) {
        if (type & Qt::Window) {
            if (!parent->testAttribute(Qt::WA_WState_Created))
                parent->createWinId();
        } else if (testAttribute(Qt::WA_NativeWindow) && !parent->internalWinId()
                   && !testAttribute(Qt::WA_DontCreateNativeAncestors)) {
            // We're about to create a native child widget that doesn't have a native parent;
            // enforce a native handle for the parent unless the Qt::WA_DontCreateNativeAncestors
            // attribute is set.
            d->createWinId(window);
        }
    }

#ifdef QT3_SUPPORT
    if (flags & Qt::WStaticContents)
        setAttribute(Qt::WA_StaticContents);
    if (flags & Qt::WDestructiveClose)
        setAttribute(Qt::WA_DeleteOnClose);
    if (flags & Qt::WShowModal)
        setWindowModality(Qt::ApplicationModal);
    if (flags & Qt::WMouseNoMask)
        setAttribute(Qt::WA_MouseNoMask);
    if (flags & Qt::WGroupLeader)
        setAttribute(Qt::WA_GroupLeader);
    if (flags & Qt::WNoMousePropagation)
        setAttribute(Qt::WA_NoMousePropagation);
#endif

    if ( type != Qt::Widget && type != Qt::Window && type != Qt::Dialog)
        setAttribute(Qt::WA_QuitOnClose, false);

    static int paintOnScreenEnv = -1;
    if (paintOnScreenEnv == -1)
        paintOnScreenEnv = qgetenv("QT_ONSCREEN_PAINT").toInt() > 0 ? 1 : 0;
    if (paintOnScreenEnv == 1)
        setAttribute(Qt::WA_PaintOnScreen);

    if (QApplicationPrivate::testAttribute(Qt::AA_NativeWindows))
        setAttribute(Qt::WA_NativeWindow);

#ifdef ALIEN_DEBUG
    qDebug() << "QWidget::create:" << this << "parent:" << parentWidget()
             << "Alien?" << !testAttribute(Qt::WA_NativeWindow);
#endif

#if defined (Q_WS_WIN) && !defined(QT_NO_DRAGANDDROP)
    // Unregister the dropsite (if already registered) before we
    // re-create the widget with a native window.
    if (testAttribute(Qt::WA_WState_Created) && !internalWinId() && testAttribute(Qt::WA_NativeWindow)
            && d->extra && d->extra->dropTarget) {
        d->registerDropSite(false);
    }
#endif // defined (Q_WS_WIN) && !defined(QT_NO_DRAGANDDROP)

    setAttribute(Qt::WA_WState_Created);                        // set created flag
    d->create_sys(window, initializeWindow, destroyOldWindow);

    // a real toplevel window needs a backing store
#ifndef Q_WS_MAC
    if (isWindow()) {
        delete d->topData()->backingStore;
        // QWidgetBackingStore will check this variable, hence it must be 0
        d->topData()->backingStore = 0;
        d->topData()->backingStore = new QWidgetBackingStore(this);
    }
#endif

    d->setModal_sys();

    if (!isWindow() && parentWidget() && parentWidget()->testAttribute(Qt::WA_DropSiteRegistered))
        setAttribute(Qt::WA_DropSiteRegistered, true);

#ifdef QT_EVAL
    extern void qt_eval_init_widget(QWidget *w);
    qt_eval_init_widget(this);
#endif

    // need to force the resting of the icon after changing parents
    if (testAttribute(Qt::WA_SetWindowIcon))
        d->setWindowIcon_sys(true);
    if (isWindow() && !d->topData()->iconText.isEmpty())
        d->setWindowIconText_helper(d->topData()->iconText);
    if (windowType() != Qt::Desktop) {
        d->updateSystemBackground();

        if (isWindow() && !testAttribute(Qt::WA_SetWindowIcon))
            d->setWindowIcon_sys();
    }
}

/*!
    Destroys the widget.

    All this widget's children are deleted first. The application
    exits if this widget is the main widget.
*/

QWidget::~QWidget()
{
    Q_D(QWidget);
    d->data.in_destructor = true;

#if defined (QT_CHECK_STATE)
    if (paintingActive())
        qWarning("QWidget: %s (%s) deleted while being painted", className(), name());
#endif

    // force acceptDrops false before winId is destroyed.
    d->registerDropSite(false);

#ifndef QT_NO_ACTION
    // remove all actions from this widget
    for (int i = 0; i < d->actions.size(); ++i) {
        QActionPrivate *apriv = d->actions.at(i)->d_func();
        apriv->widgets.removeAll(this);
    }
    d->actions.clear();
#endif

#ifndef QT_NO_SHORTCUT
    // Remove all shortcuts grabbed by this
    // widget, unless application is closing
    if (!QApplicationPrivate::is_app_closing && testAttribute(Qt::WA_GrabbedShortcut))
        qApp->d_func()->shortcutMap.removeShortcut(0, this, QKeySequence());
#endif

    // delete layout while we still are a valid widget
    delete d->layout;
    // Remove myself from focus list

    Q_ASSERT(d->focus_next->d_func()->focus_prev == this);
    Q_ASSERT(d->focus_prev->d_func()->focus_next == this);

    if (d->focus_next != this) {
        d->focus_next->d_func()->focus_prev = d->focus_prev;
        d->focus_prev->d_func()->focus_next = d->focus_next;
        d->focus_next = d->focus_prev = 0;
    }

#ifdef QT3_SUPPORT
    if (QApplicationPrivate::main_widget == this) {        // reset main widget
        QApplicationPrivate::main_widget = 0;
        qApp->quit();
    }
#endif

    clearFocus();

    d->setDirtyOpaqueRegion();

    if (isWindow() && isVisible() && internalWinId())
        hide();

    // set all QPointers for this object to zero
    QObjectPrivate::clearGuards(this);

    if (!d->children.isEmpty())
        d->deleteChildren();

    QApplication::removePostedEvents(this);

    destroy();                                        // platform-dependent cleanup

    --QWidgetPrivate::instanceCounter;

    if (QWidgetPrivate::uncreatedWidgets) // might have been deleted by ~QApplication
        QWidgetPrivate::uncreatedWidgets->remove(this);

    QEvent e(QEvent::Destroy);
    QCoreApplication::sendEvent(this, &e);
}

int QWidgetPrivate::instanceCounter = 0;  // Current number of widget instances
int QWidgetPrivate::maxInstances = 0;     // Maximum number of widget instances

void QWidgetPrivate::setWinId(WId id)                // set widget identifier
{
    Q_Q(QWidget);
    if (mapper && data.winid) {
        mapper->remove(data.winid);
        uncreatedWidgets->insert(q);
    }

    data.winid = id;
#if defined(Q_WS_X11)
    hd = id; // X11: hd == ident
#endif
    if (mapper && id) {
        mapper->insert(data.winid, q);
        uncreatedWidgets->remove(q);
    }
}

void QWidgetPrivate::createTLExtra()
{
    if (!extra)
        createExtra();
    if (!extra->topextra) {
        QTLWExtra* x = extra->topextra = new QTLWExtra;
        x->windowSurface = 0;
        x->globalOpacity = 1.0;
        x->opacity = 255;
        x->posFromMove = false;
        x->sizeAdjusted = false;
        x->inRenderWithPainter = false;
        x->inTopLevelResize = false;
        x->icon = 0;
        x->iconPixmap = 0;
        x->frameStrut.setCoords(0, 0, 0, 0);
        x->incw = x->inch = 0;
        x->basew = x->baseh = 0;
        x->normalGeometry = QRect(0,0,-1,-1);
#if defined(Q_WS_WIN) || defined(Q_WS_X11)
        x->embedded = 0;
#endif
#if defined(Q_WS_X11)
        x->parentWinId = 0;
        x->spont_unmapped = 0;
        x->dnd = 0;
#endif
        x->savedFlags = 0;
#if defined(Q_WS_QWS)
        x->backingStore = 0;
#if !defined(QT_NO_QWS_MANAGER)
        x->qwsManager = 0;
#endif
#endif
        x->sharedPainter = 0;
        x->proxyWidget = 0;
        createTLSysExtra();
#ifdef QWIDGET_EXTRA_DEBUG
    static int count = 0;
    qDebug() << "tlextra" << ++count;
#endif
    }
}

/*!
  \internal
  Creates the widget extra data.
*/

void QWidgetPrivate::createExtra()
{
    if (!extra) {                                // if not exists
        extra = new QWExtra;
        extra->minw = extra->minh = 0;
        extra->maxw = extra->maxh = QWIDGETSIZE_MAX;
        extra->explicitMinSize = 0;
        extra->autoFillBackground = 0;
#ifndef QT_NO_CURSOR
        extra->curs = 0;
#endif
        extra->style = 0;
        extra->topextra = 0;
        createSysExtra();
#ifdef QWIDGET_EXTRA_DEBUG
    static int count = 0;
    qDebug() << "extra" << ++count;
#endif
    }
}


/*!
  \internal
  Deletes the widget extra data.
*/

void QWidgetPrivate::deleteExtra()
{
    if (extra) {                                // if exists
#ifndef QT_NO_CURSOR
        delete extra->curs;
#endif
        deleteSysExtra();
#ifndef QT_NO_STYLE_STYLESHEET
        // dereference the stylesheet style
        if (QStyleSheetStyle *proxy = qobject_cast<QStyleSheetStyle *>(extra->style))
            proxy->deref();
#endif
        if (extra->topextra) {
            deleteTLSysExtra();
            delete extra->topextra->icon;
            delete extra->topextra->iconPixmap;
#if defined(Q_WS_QWS) && !defined(QT_NO_QWS_MANAGER)
            delete extra->topextra->qwsManager;
#endif
            delete extra->topextra->windowSurface;
            delete extra->topextra->sharedPainter;
            delete extra->topextra;
        }
        delete extra;
        // extra->xic destroyed in QWidget::destroy()
        extra = 0;
    }
}

/*
  Returns true if the background is inherited; otherwise returns
  false.

  Mainly used in the paintOnScreen case.
*/

bool QWidgetPrivate::isBackgroundInherited() const
{
    Q_Q(const QWidget);

    // windows do not inherit their background
    if (q->isWindow() || q->windowType() == Qt::SubWindow)
        return false;

    if (q->testAttribute(Qt::WA_NoSystemBackground) || q->testAttribute(Qt::WA_OpaquePaintEvent))
        return false;

    const QPalette &pal = q->palette();
    QPalette::ColorRole bg = q->backgroundRole();
    QBrush brush = pal.brush(bg);

    // non opaque brushes leaves us no choice, we must inherit
    if (!q->autoFillBackground() || !brush.isOpaque())
        return true;

    if (brush.style() == Qt::SolidPattern) {
        // the background is just a solid color. If there is no
        // propagated contents, then we claim as performance
        // optimization that it was not inheritet. This is the normal
        // case in standard Windows or Motif style.
        const QWidget *w = q->parentWidget();
        if (!w->d_func()->isBackgroundInherited())
            return false;
    }

    return true;
}

#ifndef Q_WS_MAC
/*
  Returns true if there are widgets above this which overlap with
  \a rect, which is in parent's coordinate system (same as crect).
*/

bool QWidgetPrivate::isOverlapped(const QRect &rect) const
{
    Q_Q(const QWidget);

    const QWidget *w = q;
    QRect r = rect;
    while (w) {
        if (w->isWindow())
            return false;
        QWidgetPrivate *pd = w->parentWidget()->d_func();
        bool above = false;
        for (int i = 0; i < pd->children.size(); ++i) {
            QWidget *sibling = qobject_cast<QWidget *>(pd->children.at(i));
            if (!sibling || !sibling->isVisible() || sibling->isWindow())
                continue;
            if (!above) {
                above = (sibling == w);
                continue;
            }
            if (sibling->data->crect.intersects(r))
                return true;
        }
        w = w->parentWidget();
        r.translate(pd->data.crect.topLeft());
    }
    return false;
}
#endif

void QWidgetPrivate::setUpdatesEnabled_helper(bool enable)
{
    Q_Q(QWidget);

    if (enable && !q->isWindow() && q->parentWidget() && !q->parentWidget()->updatesEnabled())
        return; // nothing we can do

    if (enable != q->testAttribute(Qt::WA_UpdatesDisabled))
        return; // nothing to do

    q->setAttribute(Qt::WA_UpdatesDisabled, !enable);
    if (enable)
        q->update();

    Qt::WidgetAttribute attribute = enable ? Qt::WA_ForceUpdatesDisabled : Qt::WA_UpdatesDisabled;
    for (int i = 0; i < children.size(); ++i) {
        QWidget *w = qobject_cast<QWidget *>(children.at(i));
        if (w && !w->isWindow() && !w->testAttribute(attribute))
            w->d_func()->setUpdatesEnabled_helper(enable);
    }
}

void QWidgetPrivate::propagatePaletteChange()
{
    Q_Q(QWidget);
    QEvent pc(QEvent::PaletteChange);
    QApplication::sendEvent(q, &pc);
    for (int i = 0; i < children.size(); ++i) {
        QWidget *w = qobject_cast<QWidget*>(children.at(i));
        if (w && !w->testAttribute(Qt::WA_StyleSheet) && (!w->isWindow()
                  || w->testAttribute(Qt::WA_WindowPropagation)))
            w->d_func()->resolvePalette();
    }
#if defined(QT3_SUPPORT)
    q->paletteChange(q->palette()); // compatibility
#endif
}

/*
  Returns the widget's clipping rectangle.
*/
QRect QWidgetPrivate::clipRect() const
{
    Q_Q(const QWidget);
    const QWidget * w = q;
    if (!w->isVisible())
        return QRect();
    QRect r = q->rect();
    int ox = 0;
    int oy = 0;
    while (w
            && w->isVisible()
            && !w->isWindow()
            && w->parentWidget()) {
        ox -= w->x();
        oy -= w->y();
        w = w->parentWidget();
        r &= QRect(ox, oy, w->width(), w->height());
    }
    return r;
}

/*
  Returns the widget's clipping region (without siblings).
*/
QRegion QWidgetPrivate::clipRegion() const
{
    Q_Q(const QWidget);
    if (!q->isVisible())
        return QRegion();
    QRegion r(q->rect());
    const QWidget * w = q;
    const QWidget *ignoreUpTo;
    int ox = 0;
    int oy = 0;
    while (w
           && w->isVisible()
           && !w->isWindow()
           && w->parentWidget()) {
        ox -= w->x();
        oy -= w->y();
        ignoreUpTo = w;
        w = w->parentWidget();
        r &= QRegion(ox, oy, w->width(), w->height());

        int i = 0;
        while(w->d_func()->children.at(i++) != static_cast<const QObject *>(ignoreUpTo))
            ;
        for ( ; i < w->d_func()->children.size(); ++i) {
            if(QWidget *sibling = qobject_cast<QWidget *>(w->d_func()->children.at(i))) {
                if(sibling->isVisible() && !sibling->isWindow()) {
                    QRect siblingRect(ox+sibling->x(), oy+sibling->y(),
                                      sibling->width(), sibling->height());
                    if(siblingRect.intersects(q->rect()))
                        r -= QRegion(siblingRect);
                }
            }
        }
    }
    return r;
}

void QWidgetPrivate::setDirtyOpaqueRegion()
{
    Q_Q(QWidget);

    dirtyOpaqueChildren = true;

    if (q->isWindow())
        return;

    QWidget *parent = q->parentWidget();
    if (!parent)
        return;

    // TODO: instead of setting dirtyflag, manipulate the dirtyregion directly?
    QWidgetPrivate *pd = parent->d_func();
    if (!pd->dirtyOpaqueChildren)
        pd->setDirtyOpaqueRegion();
}

QRegion QWidgetPrivate::getOpaqueRegion() const
{
    Q_Q(const QWidget);

    QRegion r = isOpaque() ? q->rect() : getOpaqueChildren();
    if (extra && !extra->mask.isEmpty())
        r &= extra->mask;
    if (r.isEmpty())
        return r;
    return r & clipRect();
}

const QRegion &QWidgetPrivate::getOpaqueChildren() const
{
    if (!dirtyOpaqueChildren)
        return opaqueChildren;

    QWidgetPrivate *that = const_cast<QWidgetPrivate*>(this);
    that->opaqueChildren = QRegion();

    for (int i = 0; i < children.size(); ++i) {
        QWidget *child = qobject_cast<QWidget *>(children.at(i));
        if (!child || !child->isVisible() || child->isWindow())
            continue;

        const QPoint offset = child->geometry().topLeft();
        that->opaqueChildren += child->d_func()->getOpaqueRegion().translated(offset);
    }

    that->dirtyOpaqueChildren = false;

    return that->opaqueChildren;
}

// hw: currently disable opaque children cache.
// Doesn't play nice with subtractOpaqueSiblings
#ifdef QT_GREENPHONE_OPT
void QWidgetPrivate::subtractOpaqueChildren(QRegion &rgn, const QRect &clipRgn, const QPoint &offset, int startIdx) const
#else
void QWidgetPrivate::subtractOpaqueChildren(QRegion &rgn, const QRegion &clipRgn, const QPoint &offset, int startIdx) const
#endif
{
    Q_UNUSED(startIdx);

    if (clipRgn.isEmpty())
        return;

    const QRegion &r = getOpaqueChildren();
    if (!r.isEmpty())
        rgn -= (r.translated(offset) & clipRgn);
}

QRegion QWidgetPrivate::getOpaqueSiblings(const QRegion &clipRegion) const
{
    Q_Q(const QWidget);

    if (q->isWindow()) // no siblings
        return QRegion();

    const QPoint myOffset = q->data->crect.topLeft();
    const QWidgetPrivate *pd = q->parentWidget()->d_func();
    const QRegion parentClip = clipRegion.translated(myOffset);

    QRegion opaqueSiblings = pd->getOpaqueSiblings(parentClip);

    const int startIdx = pd->children.indexOf(const_cast<QWidget*>(q)) + 1;
    for (int i = startIdx; i < pd->children.size(); ++i) {
        const QWidget *sibling = qobject_cast<QWidget *>(pd->children.at(i));
        if (!sibling || !sibling->isVisible() || sibling->isWindow())
            continue;

        const QRect rect = sibling->geometry();
        if (!parentClip.boundingRect().intersects(rect))
            continue;

        opaqueSiblings += parentClip & sibling->d_func()->getOpaqueRegion().translated(rect.topLeft());
    }

    return opaqueSiblings.translated(-myOffset);
}

//subtract any relatives that are higher up than me --- this is too expensive !!!
void QWidgetPrivate::subtractOpaqueSiblings(QRegion &rgn, const QPoint &offset) const
{
    static int disableSubtractOpaqueSiblings = qgetenv("QT_NO_SUBTRACTOPAQUESIBLINGS").toInt();
    if (disableSubtractOpaqueSiblings)
        return;

    rgn -= getOpaqueSiblings(rgn).translated(offset);
}

bool QWidgetPrivate::hasBackground() const
{
    Q_Q(const QWidget);
    if (!q->isWindow() && q->parentWidget() && q->parentWidget()->testAttribute(Qt::WA_PaintOnScreen))
        return true;
    if (q->testAttribute(Qt::WA_PaintOnScreen))
        return true;
    if (!q->testAttribute(Qt::WA_OpaquePaintEvent) && !q->testAttribute(Qt::WA_NoSystemBackground)) {
        const QPalette &pal = q->palette();
        QPalette::ColorRole bg = q->backgroundRole();
        QBrush bgBrush = pal.brush(bg);
        return (bgBrush.style() != Qt::NoBrush &&
                ((q->isWindow() || q->windowType() == Qt::SubWindow)
                 || (QPalette::ColorRole(bg_role) != QPalette::NoRole || (pal.resolve() & (1<<bg)))));
    }
    return false;
}

bool QWidgetPrivate::paintOnScreen() const
{
#if defined(Q_WS_QWS) || defined(Q_WS_MAC)
    return false;
#elif  defined(QT_NO_BACKINGSTORE)
    return true;
#else
    Q_Q(const QWidget);
    if (q->testAttribute(Qt::WA_PaintOnScreen)
            || !q->isWindow() && q->window()->testAttribute(Qt::WA_PaintOnScreen)) {
        return true;
    }

    // sanity check for overlarge toplevels. Better: store at least screen size and move offset.
    if (q->isWindow() && (q->width() > 4096  || q->height() > 4096))
        return true;

    return !qt_enable_backingstore;
#endif
}

void QWidgetPrivate::updateIsOpaque()
{
#ifdef Q_WS_MAC
    macUpdateIsOpaque();
#endif
    // hw: todo: only needed if opacity actually changed
    setDirtyOpaqueRegion();
}

bool QWidgetPrivate::isOpaque() const
{
    Q_Q(const QWidget);
#ifdef Q_WS_X11
    if (q->testAttribute(Qt::WA_X11OpenGLOverlay))
        return false;
#endif
    if (q->testAttribute(Qt::WA_OpaquePaintEvent)
        || q->testAttribute(Qt::WA_PaintOnScreen))
        return true;

    const QPalette &pal = q->palette();

    if (q->autoFillBackground()) {
        const QBrush &autoFillBrush = pal.brush(q->backgroundRole());
        if (autoFillBrush.style() != Qt::NoBrush && autoFillBrush.isOpaque()) {
            return true;
        }
    }

    if (q->isWindow() && !q->testAttribute(Qt::WA_NoSystemBackground)) {
        const QBrush &windowBrush = q->palette().brush(QPalette::Window);
        if (windowBrush.style() != Qt::NoBrush && windowBrush.isOpaque())
            return true;
    }

    return false;
}


/*!
    \fn void QPixmap::fill(const QWidget *widget, const QPoint &offset)

    Fills the pixmap with the \a widget's background color or pixmap
    according to the given offset.

    The QPoint \a offset defines a point in widget coordinates to
    which the pixmap's top-left pixel will be mapped to. This is only
    significant if the widget has a background pixmap; otherwise the
    pixmap will simply be filled with the background color of the
    widget.
*/

void QPixmap::fill( const QWidget *widget, const QPoint &off )
{
    QPainter p(this);
    p.translate(-off);
    widget->d_func()->paintBackground(&p, QRect(off, size()));

}

void QWidgetPrivate::paintBackground(QPainter *painter, const QRect &rect, int flags) const
{
// Optimize pattern filling on mac by using HITheme directly
// when filling with the standard widget background.
#ifdef Q_WS_MAC
extern void qt_mac_fill_background(QPainter *painter, const QRect &rect, const QBrush &brush); //qmacstyle_mac
# define FILL_RECT_WORKAROUND(painter, rect, brush)              \
    if (brush.style() == Qt::TexturePattern)                    \
        qt_mac_fill_background(painter, rect, brush);           \
    else                                                        \
        painter->fillRect(rect, brush);
#else
# define FILL_RECT_WORKAROUND(painter, rect, brush)              \
    if (brush.style() == Qt::TexturePattern)                    \
        painter->drawTiledPixmap(rect, brush.texture(), rect.topLeft()); \
    else                                                        \
        painter->fillRect(rect, brush);
#endif
    Q_Q(const QWidget);

    const QBrush autoFillBrush = q->palette().brush(q->backgroundRole());

    if ((flags & DrawAsRoot) && !(q->autoFillBackground() && autoFillBrush.isOpaque())) {
        const QBrush bg = q->palette().brush(QPalette::Window);
#ifdef Q_WS_QWS
        if (!(flags & DontSetCompositionMode) && painter->paintEngine()->hasFeature(QPaintEngine::PorterDuff))
            painter->setCompositionMode(QPainter::CompositionMode_Source); //copy alpha straight in
#endif

        FILL_RECT_WORKAROUND(painter, rect, bg);
    }

    if (q->autoFillBackground()) {
        FILL_RECT_WORKAROUND(painter, rect, autoFillBrush);
    }

    if (q->testAttribute(Qt::WA_StyledBackground)) {
        QStyleOption opt;
        opt.initFrom(q);
        q->style()->drawPrimitive(QStyle::PE_Widget, &opt, painter, q);
    }
}


/*
  \internal
  This function is called when a widget is hidden or destroyed.
  It resets some application global pointers that should only refer active,
  visible widgets.
*/

#ifdef Q_WS_MAC
    extern QPointer<QWidget> qt_button_down;
#else
    extern QWidget *qt_button_down;
#endif

void QWidgetPrivate::deactivateWidgetCleanup()
{
    Q_Q(QWidget);
    // If this was the active application window, reset it
    if (qApp->activeWindow() == q)
        qApp->setActiveWindow(0);
    // If the is the active mouse press widget, reset it
    if (q == qt_button_down)
        qt_button_down = 0;
}


/*!
    Returns a pointer to the widget with window identifer/handle \a
    id.

    The window identifier type depends on the underlying window
    system, see \c qwindowdefs.h for the actual definition. If there
    is no widget with this identifier, 0 is returned.
*/

QWidget *QWidget::find(WId id)
{
    return QWidgetPrivate::mapper ? QWidgetPrivate::mapper->value(id, 0) : 0;
}



/*!
    \fn WId QWidget::internalWinId() const
    \internal
    Returns the window system identifier of the widget, or 0 if the widget is not created yet.

*/

/*!
    \fn WId QWidget::winId() const

    Returns the window system identifier of the widget.

    Portable in principle, but if you use it you are probably about to
    do something non-portable. Be careful.

    \sa find()
*/
WId QWidget::winId() const
{
    if (!testAttribute(Qt::WA_WState_Created) || !internalWinId()) {
        QWidget *that = const_cast<QWidget*>(this);
        that->setAttribute(Qt::WA_NativeWindow);
        that->d_func()->createWinId();
        return that->data->winid;
    }
    return data->winid;
}


void QWidgetPrivate::createWinId(WId winid)
{
    Q_Q(QWidget);
    const bool forceNativeWindow = q->testAttribute(Qt::WA_NativeWindow);
    if (!q->testAttribute(Qt::WA_WState_Created) || (forceNativeWindow && !q->internalWinId())) {
        if (!q->isWindow()) {
            QWidget *parent = q->parentWidget();
            QWidgetPrivate *pd = parent->d_func();
            if (!parent->internalWinId()) {
                if (forceNativeWindow && !q->testAttribute(Qt::WA_DontCreateNativeAncestors))
                    parent->setAttribute(Qt::WA_NativeWindow);
                pd->createWinId();
            }

            for (int i = 0; i < pd->children.size(); ++i) {
                QWidget *w = qobject_cast<QWidget *>(pd->children.at(i));
                if (w && !w->isWindow() && (!w->testAttribute(Qt::WA_WState_Created)
                                            || (!w->internalWinId() && w->testAttribute(Qt::WA_NativeWindow)))) {
                    if (w!=q) {
                        w->create();
                    } else {
                        w->create(winid);
                        // if the window has already been created, we
                        // need to raise it to its proper stacking position
                        if (winid)
                            w->raise();
                    }
                }
            }
        } else {
            q->create();
        }
    }
}


/*!
\internal
Ensures that the widget has a window system identifier, i.e. that it is known to the windowing system.

*/

void QWidget::createWinId()
{
    Q_D(QWidget);
//    qWarning("QWidget::createWinId is obsolete, please fix your code.");
    d->createWinId();
}

/*!
    Returns the effective window system identifier of the widget, i.e.
    the native parent's window system identifier.

    \sa nativeParentWidget()
*/
WId QWidget::effectiveWinId() const
{
    WId id = internalWinId();
    if (id || !testAttribute(Qt::WA_WState_Created))
        return id;
    QWidget *realParent = nativeParentWidget();
    Q_ASSERT(realParent);
    Q_ASSERT(realParent->internalWinId());
    return realParent->internalWinId();
}

#ifndef QT_NO_STYLE_STYLESHEET

/*!
    \property QWidget::styleSheet
    \brief the widget's style sheet
    \since 4.2

    The style sheet contains a textual description of customizations to the
    widget's style, as described in the \l{Qt Style Sheets} document.

    \note Qt style sheets are currently not supported for QMacStyle
    (the default style on Mac OS X). We plan to address this in some future
    release.

    \sa setStyle(), QApplication::styleSheet, {Qt Style Sheets}
*/
QString QWidget::styleSheet() const
{
    Q_D(const QWidget);
    if (!d->extra)
        return QString();
    return d->extra->styleSheet;
}

void QWidget::setStyleSheet(const QString& styleSheet)
{
    Q_D(QWidget);
    d->createExtra();

    QStyleSheetStyle *proxy = qobject_cast<QStyleSheetStyle *>(d->extra->style);
    d->extra->styleSheet = styleSheet;
    if (styleSheet.isEmpty()) { // stylesheet removed
        if (!proxy)
            return;

        d->inheritStyle();
        return;
    }

    if (proxy) { // style sheet update
        proxy->repolish(this);
        return;
    }

    if (testAttribute(Qt::WA_SetStyle)) {
        d->setStyle_helper(new QStyleSheetStyle(d->extra->style), true);
    } else {
        d->setStyle_helper(new QStyleSheetStyle(0), true);
    }
}

#endif // QT_NO_STYLE_STYLESHEET

/*!
    \sa QWidget::setStyle(), QApplication::setStyle(), QApplication::style()
*/

QStyle *QWidget::style() const
{
    Q_D(const QWidget);

    if (d->extra && d->extra->style)
        return d->extra->style;
    return qApp->style();
}

/*!
    Sets the widget's GUI style to \a style. The ownership of the style
    object is not transferred.

    If no style is set, the widget uses the application's style,
    QApplication::style() instead.

    Setting a widget's style has no effect on existing or future child
    widgets.

    \warning This function is particularly useful for demonstration
    purposes, where you want to show Qt's styling capabilities. Real
    applications should avoid it and use one consistent GUI style
    instead.

    \sa style(), QStyle, QApplication::style(), QApplication::setStyle()
*/

void QWidget::setStyle(QStyle *style)
{
    Q_D(QWidget);
    setAttribute(Qt::WA_SetStyle, style != 0);
    d->createExtra();
#ifndef QT_NO_STYLE_STYLESHEET
    // if we have an application stylesheet or have a proxy already, propagate
    if (qobject_cast<QStyleSheetStyle *>(d->extra->style) || !qApp->styleSheet().isEmpty())
        d->setStyle_helper(new QStyleSheetStyle(style), true);
    else
#endif
        d->setStyle_helper(style, false);
}

void QWidgetPrivate::setStyle_helper(QStyle *newStyle, bool propagate, bool
#ifdef Q_WS_MAC
        metalHack
#endif
        )
{
    Q_Q(QWidget);
    createExtra();

    QStyle *oldStyle  = q->style();
#ifndef QT_NO_STYLE_STYLESHEET
    QStyle *origStyle = extra->style;
#endif
    extra->style = newStyle;

    // repolish
    if (q->windowType() != Qt::Desktop) {
        if (polished) {
            oldStyle->unpolish(q);
#ifdef Q_WS_MAC
            if (metalHack)
                macUpdateMetalAttribute();
#endif
            q->style()->polish(q);
#ifdef Q_WS_MAC
        } else if (metalHack) {
            macUpdateMetalAttribute();
#endif
        }
    }

    if (propagate) {
        for (int i = 0; i < children.size(); ++i) {
            QWidget *c = qobject_cast<QWidget*>(children.at(i));
            if (c)
                c->d_func()->inheritStyle();
        }
    }

    QEvent e(QEvent::StyleChange);
    QApplication::sendEvent(q, &e);
#ifdef QT3_SUPPORT
    q->styleChange(*oldStyle);
#endif

#ifndef QT_NO_STYLE_STYLESHEET
    if (!qobject_cast<QStyleSheetStyle*>(newStyle)) {
        if (const QStyleSheetStyle* cssStyle = qobject_cast<QStyleSheetStyle*>(origStyle)) {
            cssStyle->clearWidgetFont(q);
        }
    }
#endif

#ifndef QT_NO_STYLE_STYLESHEET
    // dereference the old stylesheet style
    if (QStyleSheetStyle *proxy = qobject_cast<QStyleSheetStyle *>(origStyle))
        proxy->deref();
#endif
}

// Inherits style from the current parent and propagates it as necessary
void QWidgetPrivate::inheritStyle()
{
#ifndef QT_NO_STYLE_STYLESHEET
    Q_Q(QWidget);

    QStyleSheetStyle *proxy = extra ? qobject_cast<QStyleSheetStyle *>(extra->style) : 0;

    if (!q->styleSheet().isEmpty()) {
        Q_ASSERT(proxy);
        proxy->repolish(q);
        return;
    }

    QStyle *origStyle = proxy ? proxy->base : (extra ? (QStyle*)extra->style : 0);
    QWidget *parent = q->parentWidget();
    QStyle *parentStyle = (parent && parent->d_func()->extra) ? (QStyle*)parent->d_func()->extra->style : 0;
    // If we have stylesheet on app or parent has stylesheet style, we need
    // to be running a proxy
    if (!qApp->styleSheet().isEmpty() || qobject_cast<QStyleSheetStyle *>(parentStyle)) {
        QStyle *newStyle = parentStyle;
        if (q->testAttribute(Qt::WA_SetStyle))
            newStyle = new QStyleSheetStyle(origStyle);
        else if (QStyleSheetStyle *newProxy = qobject_cast<QStyleSheetStyle *>(parentStyle))
            newProxy->ref();

        setStyle_helper(newStyle, true);
        return;
    }

    // So, we have no stylesheet on parent/app and we have an empty stylesheet
    // we just need our original style back
    if (origStyle == (extra ? (QStyle*)extra->style : 0)) // is it any different?
        return;

    // We could have inherited the proxy from our parent (which has a custom style)
    // In such a case we need to start following the application style (i.e revert
    // the propagation behavior of QStyleSheetStyle)
    if (!q->testAttribute(Qt::WA_SetStyle))
        origStyle = 0;

    setStyle_helper(origStyle, true);
#endif // QT_NO_STYLE_STYLESHEET
}

#ifdef QT3_SUPPORT
/*!
    \overload

    Sets the widget's GUI style to \a style using the QStyleFactory.
*/
QStyle* QWidget::setStyle(const QString &style)
{
    QStyle *s = QStyleFactory::create(style);
    setStyle(s);
    return s;
}
#endif

/*!
    \fn bool QWidget::isWindow() const

    Returns true if the widget is an independent window, otherwise
    returns false.

    A window is a widget that isn't visually the child of any other
    widget and that usually has a frame and a
    \l{QWidget::setWindowTitle()}{window title}.

    A window can have a \l{QWidget::parentWidget()}{parent widget}.
    It will then be grouped with its parent and deleted when the
    parent is deleted, minimized when the parent is minimized etc. If
    supported by the window manager, it will also have a common
    taskbar entry with its parent.

    QDialog and QMainWindow widgets are by default windows, even if a
    parent widget is specified in the constructor. This behavior is
    specified by the Qt::Window flag.

    \sa window(), isModal(), parentWidget()
*/

/*!
    \property QWidget::modal
    \brief whether the widget is a modal widget

    This property only makes sense for windows. A modal widget
    prevents widgets in all other windows from getting any input.

    \sa isWindow(), windowModality, QDialog
*/

/*!
    \property QWidget::windowModality
    \brief which windows are blocked by the modal widget
    \since 4.1

    This property only makes sense for windows. A modal widget
    prevents widgets in other windows from getting input. The value of
    this property controls which windows are blocked when the widget
    is visible. Changing this property while the window is visible has
    no effect; you must hide() the widget first, then show() it again.

    By default, this property is Qt::NonModal.

    \sa isWindow(), QWidget::modal, QDialog
*/

Qt::WindowModality QWidget::windowModality() const
{
    return static_cast<Qt::WindowModality>(data->window_modality);
}

void QWidget::setWindowModality(Qt::WindowModality windowModality)
{
    data->window_modality = windowModality;
    // setModal_sys() will be called by setAttribute()
    setAttribute(Qt::WA_ShowModal, (data->window_modality != Qt::NonModal));
}

/*!
    \fn bool QWidget::underMouse() const

    Returns true if the widget is under the mouse cursor; otherwise
    returns false.

    This value is not updated properly during drag and drop
    operations.

    \sa enterEvent(), leaveEvent()
*/

/*!
    \property QWidget::minimized
    \brief whether this widget is minimized (iconified)

    This property is only relevant for windows.

    \sa showMinimized(), visible, show(), hide(), showNormal(), maximized
*/
bool QWidget::isMinimized() const
{ return data->window_state & Qt::WindowMinimized; }

/*!
    Shows the widget minimized, as an icon.

    Calling this function only affects \l{isWindow()}{windows}.

    \sa showNormal(), showMaximized(), show(), hide(), isVisible(),
        isMinimized()
*/
void QWidget::showMinimized()
{
    bool isMin = isMinimized();
    if (isMin && isVisible())
        return;

    ensurePolished();
#ifdef QT3_SUPPORT
    if (parent())
        QApplication::sendPostedEvents(parent(), QEvent::ChildInserted);
#endif

    if (!isMin)
        setWindowState((windowState() & ~Qt::WindowActive) | Qt::WindowMinimized);
    show();
}

/*!
    \property QWidget::maximized
    \brief whether this widget is maximized

    This property is only relevant for windows.

    Note that due to limitations in some window-systems, this does not
    always report the expected results (e.g. if the user on X11
    maximizes the window via the window manager, Qt has no way of
    distinguishing this from any other resize). This is expected to
    improve as window manager protocols evolve.

    \sa windowState(), showMaximized(), visible, show(), hide(), showNormal(), minimized
*/
bool QWidget::isMaximized() const
{ return data->window_state & Qt::WindowMaximized; }



/*!
    Returns the current window state. The window state is a OR'ed
    combination of Qt::WindowState: Qt::WindowMinimized,
    Qt::WindowMaximized, Qt::WindowFullScreen, and Qt::WindowActive.

  \sa Qt::WindowState setWindowState()
 */
Qt::WindowStates QWidget::windowState() const
{
    return (Qt::WindowStates)data->window_state;
}

/*!\internal

   The function sets the window state on child widgets similar to
   setWindowState(). The difference is that the window state changed
   event has the isOverride() flag set. It exists mainly to keep
   Q3Workspace working.
 */
void QWidget::overrideWindowState(Qt::WindowStates newstate)
{
    QWindowStateChangeEvent e((Qt::WindowStates)data->window_state, true);
    data->window_state  = newstate;
    QApplication::sendEvent(this, &e);
}

/*!
    \fn void QWidget::setWindowState(Qt::WindowStates windowState)

    Sets the window state to \a windowState. The window state is a OR'ed
    combination of Qt::WindowState: Qt::WindowMinimized,
    Qt::WindowMaximized, Qt::WindowFullScreen, and Qt::WindowActive.

    If the window is not visible (i.e. isVisible() returns false), the
    window state will take effect when show() is called. For visible
    windows, the change is immediate. For example, to toggle between
    full-screen and normal mode, use the following code:

    \snippet doc/src/snippets/code/src.gui.kernel.qwidget.cpp 0

    In order to restore and activate a minimized window (while
    preserving its maximized and/or full-screen state), use the following:

    \snippet doc/src/snippets/code/src.gui.kernel.qwidget.cpp 1

    Calling this function will hide the widget. You must call show() to make
    the widget visible again.

    \note On some window systems Qt::WindowActive is not immediate, and may be
    ignored in certain cases.

    When the window state changes, the widget receives a changeEvent()
    of type QEvent::WindowStateChange.

    \sa Qt::WindowState windowState()
*/

/*!
    \property QWidget::fullScreen
    \brief whether the widget is full screen

    \sa windowState(), minimized, maximized
*/
bool QWidget::isFullScreen() const
{ return data->window_state & Qt::WindowFullScreen; }

/*!
    Shows the widget in full-screen mode.

    Calling this function only affects \l{isWindow()}{windows}.

    To return from full-screen mode, call showNormal().

    Full-screen mode works fine under Windows, but has certain
    problems under X. These problems are due to limitations of the
    ICCCM protocol that specifies the communication between X11
    clients and the window manager. ICCCM simply does not understand
    the concept of non-decorated full-screen windows. Therefore, the
    best we can do is to request a borderless window and place and
    resize it to fill the entire screen. Depending on the window
    manager, this may or may not work. The borderless window is
    requested using MOTIF hints, which are at least partially
    supported by virtually all modern window managers.

    An alternative would be to bypass the window manager entirely and
    create a window with the Qt::X11BypassWindowManagerHint flag. This
    has other severe problems though, like totally broken keyboard focus
    and very strange effects on desktop changes or when the user raises
    other windows.

    X11 window managers that follow modern post-ICCCM specifications
    support full-screen mode properly.

    \sa showNormal(), showMaximized(), show(), hide(), isVisible()
*/
void QWidget::showFullScreen()
{
    ensurePolished();
#ifdef QT3_SUPPORT
    if (parent())
        QApplication::sendPostedEvents(parent(), QEvent::ChildInserted);
#endif

    setWindowState((windowState() & ~(Qt::WindowMinimized | Qt::WindowMaximized))
                   | Qt::WindowFullScreen);
    show();
    activateWindow();
}

/*!
    Shows the widget maximized.

    Calling this function only affects \l{isWindow()}{windows}.

    On X11, this function may not work properly with certain window
    managers. See \l{geometry.html}{Window Geometry} for an explanation.

    \sa setWindowState(), showNormal(), showMinimized(), show(), hide(), isVisible()
*/
void QWidget::showMaximized()
{
    ensurePolished();
#ifdef QT3_SUPPORT
    if (parent())
        QApplication::sendPostedEvents(parent(), QEvent::ChildInserted);
#endif

    setWindowState((windowState() & ~(Qt::WindowMinimized | Qt::WindowFullScreen))
                   | Qt::WindowMaximized);
    show();
}

/*!
    Restores the widget after it has been maximized or minimized.

    Calling this function only affects \l{isWindow()}{windows}.

    \sa setWindowState(), showMinimized(), showMaximized(), show(), hide(), isVisible()
*/
void QWidget::showNormal()
{
    ensurePolished();
#ifdef QT3_SUPPORT
    if (parent())
        QApplication::sendPostedEvents(parent(), QEvent::ChildInserted);
#endif

    setWindowState(windowState() & ~(Qt::WindowMinimized
                                     | Qt::WindowMaximized
                                     | Qt::WindowFullScreen));
    show();
}

/*!
    Returns true if this widget would become enabled if \a ancestor is
    enabled; otherwise returns false.



    This is the case if neither the widget itself nor every parent up
    to but excluding \a ancestor has been explicitly disabled.

    isEnabledTo(0) is equivalent to isEnabled().

    \sa setEnabled() enabled
*/

bool QWidget::isEnabledTo(QWidget* ancestor) const
{
    const QWidget * w = this;
    while (w && !w->testAttribute(Qt::WA_ForceDisabled)
            && !w->isWindow()
            && w->parentWidget()
            && w->parentWidget() != ancestor)
        w = w->parentWidget();
    return !w->testAttribute(Qt::WA_ForceDisabled);
}

#ifndef QT_NO_ACTION
/*!
    Appends the action \a action to this widget's list of actions.

    All QWidgets have a list of \l{QAction}s, however they can be
    represented graphically in many different ways. The default use of
    the QAction list (as returned by actions()) is to create a context
    QMenu.

    A QWidget should only have one of each action.

    \sa removeAction(), insertAction(), actions(), QMenu
*/
void QWidget::addAction(QAction *action)
{
    insertAction(0, action);
}

/*!
    Appends the actions \a actions to this widget's list of actions.

    \sa removeAction(), QMenu, addAction()
*/
void QWidget::addActions(QList<QAction*> actions)
{
    for(int i = 0; i < actions.count(); i++)
        insertAction(0, actions.at(i));
}

/*!
    Inserts the action \a action to this widget's list of actions,
    before the action \a before. It appends the action if \a before is 0 or
    \a before is not a valid action for this widget.

    A QWidget should only have one of each action.

    \sa removeAction(), addAction(), QMenu, contextMenuPolicy, actions()
*/
void QWidget::insertAction(QAction *before, QAction *action)
{
    if(!action) {
        qWarning("QWidget::insertAction: Attempt to insert null action");
        return;
    }

    Q_D(QWidget);
    if(d->actions.contains(action))
        removeAction(action);

    int pos = d->actions.indexOf(before);
    if (pos < 0) {
        before = 0;
        pos = d->actions.size();
    }
    d->actions.insert(pos, action);

    QActionPrivate *apriv = action->d_func();
    apriv->widgets.append(this);

    QActionEvent e(QEvent::ActionAdded, action, before);
    QApplication::sendEvent(this, &e);
}

/*!
    Inserts the actions \a actions to this widget's list of actions,
    before the action \a before. It appends the action if \a before is 0 or
    \a before is not a valid action for this widget.

    A QWidget can have at most one of each action.

    \sa removeAction(), QMenu, insertAction(), contextMenuPolicy
*/
void QWidget::insertActions(QAction *before, QList<QAction*> actions)
{
    for(int i = 0; i < actions.count(); ++i)
        insertAction(before, actions.at(i));
}

/*!
    Removes the action \a action from this widget's list of actions.
    \sa insertAction(), actions(), insertAction()
*/
void QWidget::removeAction(QAction *action)
{
    if (!action)
        return;

    Q_D(QWidget);

    QActionPrivate *apriv = action->d_func();
    apriv->widgets.removeAll(this);

    if (d->actions.removeAll(action)) {
        QActionEvent e(QEvent::ActionRemoved, action);
        QApplication::sendEvent(this, &e);
    }
}

/*!
    Returns the (possibly empty) list of this widget's actions.

    \sa contextMenuPolicy, insertAction(), removeAction()
*/
QList<QAction*> QWidget::actions() const
{
    Q_D(const QWidget);
    return d->actions;
}
#endif // QT_NO_ACTION

/*!
  \fn bool QWidget::isEnabledToTLW() const
  \obsolete

  This function is deprecated. It is equivalent to isEnabled()
*/

/*!
    \property QWidget::enabled
    \brief whether the widget is enabled

    An enabled widget handles keyboard and mouse events; a disabled
    widget does not.



    Some widgets display themselves differently when they are
    disabled. For example a button might draw its label grayed out. If
    your widget needs to know when it becomes enabled or disabled, you
    can use the changeEvent() with type QEvent::EnabledChange.

    Disabling a widget implicitly disables all its children. Enabling
    respectively enables all child widgets unless they have been
    explicitly disabled.

    \sa isEnabledTo(), QKeyEvent, QMouseEvent, changeEvent()
*/
void QWidget::setEnabled(bool enable)
{
    Q_D(QWidget);
    setAttribute(Qt::WA_ForceDisabled, !enable);
    d->setEnabled_helper(enable);
}

void QWidgetPrivate::setEnabled_helper(bool enable)
{
    Q_Q(QWidget);

    if (enable && !q->isWindow() && q->parentWidget() && !q->parentWidget()->isEnabled())
        return; // nothing we can do

    if (enable != q->testAttribute(Qt::WA_Disabled))
        return; // nothing to do

    q->setAttribute(Qt::WA_Disabled, !enable);
    updateSystemBackground();

    if (!enable && q->window()->focusWidget() == q) {
        bool parentIsEnabled = (!q->parentWidget() || q->parentWidget()->isEnabled());
        if (!parentIsEnabled || !q->focusNextChild())
            q->clearFocus();
    }

    Qt::WidgetAttribute attribute = enable ? Qt::WA_ForceDisabled : Qt::WA_Disabled;
    for (int i = 0; i < children.size(); ++i) {
        QWidget *w = qobject_cast<QWidget *>(children.at(i));
        if (w && !w->testAttribute(attribute))
            w->d_func()->setEnabled_helper(enable);
    }
#if defined(Q_WS_X11)
    if (q->testAttribute(Qt::WA_SetCursor) || q->isWindow()) {
        // enforce the windows behavior of clearing the cursor on
        // disabled widgets
        extern void qt_x11_enforce_cursor(QWidget * w); // defined in qwidget_x11.cpp
        qt_x11_enforce_cursor(q);
    }
#endif
#if defined (Q_WS_WIN)
    if (q->hasFocus())
        QWinInputContext::updateImeStatus(q, true);
#endif
    QEvent e(QEvent::EnabledChange);
    QApplication::sendEvent(q, &e);
#ifdef QT3_SUPPORT
    q->enabledChange(!enable); // compatibility
#endif
}

/*!
    \property QWidget::acceptDrops
    \brief whether drop events are enabled for this widget

    Setting this property to true announces to the system that this
    widget \e may be able to accept drop events.

    If the widget is the desktop (QWidget::(windowType() == Qt::Desktop)), this may
    fail if another application is using the desktop; you can call
    acceptDrops() to test if this occurs.

    \warning
    Do not modify this property in a Drag&Drop event handler.
*/
bool QWidget::acceptDrops() const
{
    return testAttribute(Qt::WA_AcceptDrops);
}

void QWidget::setAcceptDrops(bool on)
{
    setAttribute(Qt::WA_AcceptDrops, on);

}

/*!
    \fn void QWidget::enabledChange(bool)

    \internal
    \obsolete
*/

/*!
    \fn void QWidget::paletteChange(const QPalette &)

    \internal
    \obsolete
*/

/*!
    \fn void QWidget::fontChange(const QFont &)

    \internal
    \obsolete
*/

/*!
    \fn void QWidget::windowActivationChange(bool)

    \internal
    \obsolete
*/

/*!
    \fn void QWidget::languageChange()

    \obsolete
*/

/*!
    \fn void QWidget::styleChange(QStyle& style)

    \internal
    \obsolete
*/

/*!
    Disables widget input events if \a disable is true; otherwise
    enables input events.

    See the \l enabled documentation for more information.

    \sa isEnabledTo(), QKeyEvent, QMouseEvent, changeEvent()
*/
void QWidget::setDisabled(bool disable)
{
    setEnabled(!disable);
}

/*!
    \property QWidget::frameGeometry
    \brief geometry of the widget relative to its parent including any
    window frame

    See the \link geometry.html Window Geometry documentation\endlink
    for an overview of geometry issues with windows.

    \sa geometry() x() y() pos()
*/
QRect QWidget::frameGeometry() const
{
    Q_D(const QWidget);
    if (isWindow() && ! (windowType() == Qt::Popup)) {
        QRect fs = d->frameStrut();
        return QRect(data->crect.x() - fs.left(),
                     data->crect.y() - fs.top(),
                     data->crect.width() + fs.left() + fs.right(),
                     data->crect.height() + fs.top() + fs.bottom());
    }
    return data->crect;
}

/*!
    \property QWidget::x

    \brief the x coordinate of the widget relative to its parent including
    any window frame

    See the \link geometry.html Window Geometry documentation\endlink
    for an overview of window geometry.

    \sa frameGeometry, y, pos
*/
int QWidget::x() const
{
    Q_D(const QWidget);
    if (isWindow() && ! (windowType() == Qt::Popup))
        return data->crect.x() - d->frameStrut().left();
    return data->crect.x();
}

/*!
    \property QWidget::y
    \brief the y coordinate of the widget relative to its parent and
    including any window frame

    See the \link geometry.html Window Geometry documentation\endlink
    for an overview of window geometry.

    \sa frameGeometry, x, pos
*/
int QWidget::y() const
{
    Q_D(const QWidget);
    if (isWindow() && ! (windowType() == Qt::Popup))
        return data->crect.y() - d->frameStrut().top();
    return data->crect.y();
}

/*!
    \property QWidget::pos
    \brief the position of the widget within its parent widget

    If the widget is a window, the position is that of the widget on
    the desktop, including its frame.

    When changing the position, the widget, if visible, receives a
    move event (moveEvent()) immediately. If the widget is not
    currently visible, it is guaranteed to receive an event before it
    is shown.

    \warning Calling move() or setGeometry() inside moveEvent() can
    lead to infinite recursion.

    See the \link geometry.html Window Geometry documentation\endlink
    for an overview of window geometry.

    \sa frameGeometry, size x(), y()
*/
QPoint QWidget::pos() const
{
    Q_D(const QWidget);
    if (isWindow() && ! (windowType() == Qt::Popup)) {
        QRect fs = d->frameStrut();
        return QPoint(data->crect.x() - fs.left(), data->crect.y() - fs.top());
    }
    return data->crect.topLeft();
}

/*!
    \property QWidget::geometry
    \brief the geometry of the widget relative to its parent and
    excluding the window frame

    When changing the geometry, the widget, if visible, receives a
    move event (moveEvent()) and/or a resize event (resizeEvent())
    immediately. If the widget is not currently visible, it is
    guaranteed to receive appropriate events before it is shown.

    The size component is adjusted if it lies outside the range
    defined by minimumSize() and maximumSize().

    \warning Calling setGeometry() inside resizeEvent() or moveEvent()
    can lead to infinite recursion.

    See the \link geometry.html Window Geometry documentation\endlink
    for an overview of window geometry.

    \sa frameGeometry(), rect(), move(), resize(), moveEvent(),
        resizeEvent(), minimumSize(), maximumSize()
*/

/*!
    \property QWidget::normalGeometry

    \brief the geometry of the widget as it will appear when shown as
    a normal (not maximized or fullscreen) toplevel widget

    For child widgets this property always holds an empty rect.

    \sa QWidget::windowState(), QWidget::geometry
*/

/*!
    \property QWidget::size
    \brief the size of the widget excluding any window frame

    If the widget is visible when it is being resized, it receives a resize event
    (resizeEvent()) immediately. If the widget is not currently
    visible, it is guaranteed to receive an event before it is shown.

    The size is adjusted if it lies outside the range defined by
    minimumSize() and maximumSize().

    \warning Calling resize() or setGeometry() inside resizeEvent() can
    lead to infinite recursion.

    Note that setting size to QSize(0, 0) will cause the widget to not
    appear on screen. This also applies to windows.

    \sa pos, geometry, minimumSize, maximumSize, resizeEvent()
*/

/*!
    \property QWidget::width
    \brief the width of the widget excluding any window frame

    See the \link geometry.html Window Geometry documentation\endlink
    for an overview of window geometry.

    \sa geometry, height, size
*/

/*!
    \property QWidget::height
    \brief the height of the widget excluding any window frame

    See the \link geometry.html Window Geometry documentation\endlink
    for an overview of window geometry.

    \sa geometry, width, size
*/

/*!
    \property QWidget::rect
    \brief the internal geometry of the widget excluding any window
    frame

    The rect property equals QRect(0, 0, width(), height()).

    See the \link geometry.html Window Geometry documentation\endlink
    for an overview of window geometry.

    \sa size
*/


QRect QWidget::normalGeometry() const
{
    Q_D(const QWidget);
    if (!d->extra || !d->extra->topextra)
        return QRect();

    if (!isMaximized() && !isFullScreen())
        return geometry();

    return d->topData()->normalGeometry;
}


/*!
    \property QWidget::childrenRect
    \brief the bounding rectangle of the widget's children

    Hidden children are excluded.

    \sa childrenRegion() geometry()
*/

QRect QWidget::childrenRect() const
{
    Q_D(const QWidget);
    QRect r(0, 0, 0, 0);
    for (int i = 0; i < d->children.size(); ++i) {
        QWidget *w = qobject_cast<QWidget *>(d->children.at(i));
        if (w && !w->isWindow() && !w->isHidden())
            r |= w->geometry();
    }
    return r;
}

/*!
    \property QWidget::childrenRegion
    \brief the combined region occupied by the widget's children

    Hidden children are excluded.

    \sa childrenRect() geometry() mask()
*/

QRegion QWidget::childrenRegion() const
{
    Q_D(const QWidget);
    QRegion r;
    for (int i = 0; i < d->children.size(); ++i) {
        QWidget *w = qobject_cast<QWidget *>(d->children.at(i));
        if (w && !w->isWindow() && !w->isHidden()) {
            QRegion mask = w->mask();
            if (mask.isEmpty())
                r |= w->geometry();
            else
                r |= mask.translated(w->pos());
        }
    }
    return r;
}


/*!
    \property QWidget::minimumSize
    \brief the widget's minimum size

    The widget cannot be resized to a smaller size than the minimum
    widget size. The widget's size is forced to the minimum size if
    the current size is smaller.

    The minimum size set by this function will override the minimum size defined by QLayout.
    In order to unset the minimum size, use QSize(0, 0).

    \sa minimumWidth, minimumHeight, maximumSize, sizeIncrement
*/

QSize QWidget::minimumSize() const
{
    Q_D(const QWidget);
    return d->extra ? QSize(d->extra->minw, d->extra->minh) : QSize(0, 0);
}

/*!
    \property QWidget::maximumSize
    \brief the widget's maximum size

    The widget cannot be resized to a larger size than the maximum
    widget size.

    The property is limited by the QWIDGETSIZE_MAX macro, i.e. the
    largest allowed size is QSize(16777215, 16777215).

    \sa maximumWidth, maximumHeight, minimumSize, sizeIncrement
*/

QSize QWidget::maximumSize() const
{
    Q_D(const QWidget);
    return d->extra ? QSize(d->extra->maxw, d->extra->maxh)
                 : QSize(QWIDGETSIZE_MAX, QWIDGETSIZE_MAX);
}


/*!
    \property QWidget::minimumWidth
    \brief the widget's minimum width

    This property corresponds to minimumSize().width().

    \sa minimumSize, minimumHeight
*/

/*!
    \property QWidget::minimumHeight
    \brief the widget's minimum height

    This property corresponds to minimumSize().height().

    \sa minimumSize, minimumWidth
*/

/*!
    \property QWidget::maximumWidth
    \brief the widget's maximum width

    This property corresponds to maximumSize().width(). It is limited
    by the QWIDGETSIZE_MAX macro, i.e. the largest allowed width is
    16777215.

    \sa maximumSize, maximumHeight
*/

/*!
    \property QWidget::maximumHeight
    \brief the widget's maximum height

    This property corresponds to maximumSize().height(). It is limited
    by the QWIDGETSIZE_MAX macro, i.e. the largest allowed height is
    16777215.

    \sa maximumSize, maximumWidth
*/

/*!
    \property QWidget::sizeIncrement
    \brief the size increment of the widget

    When the user resizes the window, the size will move in steps of
    sizeIncrement().width() pixels horizontally and
    sizeIncrement.height() pixels vertically, with baseSize() as the
    basis. Preferred widget sizes are for non-negative integers \e i
    and \e j:
    \snippet doc/src/snippets/code/src.gui.kernel.qwidget.cpp 2

    Note that while you can set the size increment for all widgets, it
    only affects windows.

    \warning The size increment has no effect under Windows, and may
    be disregarded by the window manager on X.

    \sa size, minimumSize, maximumSize
*/
QSize QWidget::sizeIncrement() const
{
    Q_D(const QWidget);
    return (d->extra && d->extra->topextra)
        ? QSize(d->extra->topextra->incw, d->extra->topextra->inch)
        : QSize(0, 0);
}

/*!
    \property QWidget::baseSize
    \brief the base size of the widget

    The base size is used to calculate a proper widget size if the
    widget defines sizeIncrement().

    \sa setSizeIncrement()
*/

QSize QWidget::baseSize() const
{
    Q_D(const QWidget);
    return (d->extra != 0 && d->extra->topextra != 0)
        ? QSize(d->extra->topextra->basew, d->extra->topextra->baseh)
        : QSize(0, 0);
}

bool QWidgetPrivate::setMinimumSize_helper(int &minw, int &minh)
{
    Q_Q(QWidget);

#ifdef Q_WS_QWS
    if (q->isWindow()) {
        const QRect maxWindowRect = QApplication::desktop()->availableGeometry(QApplication::desktop()->screenNumber(q));
        if (!maxWindowRect.isEmpty()) {
            // ### This is really just a work-around. Layout shouldn't be
            // asking for minimum sizes bigger than the screen.
            if (minw > maxWindowRect.width())
                minw = maxWindowRect.width();
            if (minh > maxWindowRect.height())
                minh = maxWindowRect.height();
        }
    }
#endif
    if (minw > QWIDGETSIZE_MAX || minh > QWIDGETSIZE_MAX) {
        qWarning("QWidget::setMinimumSize: (%s/%s) "
                "The largest allowed size is (%d,%d)",
                 q->objectName().toLocal8Bit().data(), q->metaObject()->className(), QWIDGETSIZE_MAX,
                QWIDGETSIZE_MAX);
        minw = qMin<int>(minw, QWIDGETSIZE_MAX);
        minh = qMin<int>(minh, QWIDGETSIZE_MAX);
    }
    if (minw < 0 || minh < 0) {
        qWarning("QWidget::setMinimumSize: (%s/%s) Negative sizes (%d,%d) "
                "are not possible",
                q->objectName().toLocal8Bit().data(), q->metaObject()->className(), minw, minh);
        minw = qMax(minw, 0);
        minh = qMax(minh, 0);
    }
    createExtra();
    if (extra->minw == minw && extra->minh == minh)
        return false;
    extra->minw = minw;
    extra->minh = minh;
    extra->explicitMinSize = (minw ? Qt::Horizontal : 0) | (minh ? Qt::Vertical : 0);
    return true;
}

/*!
    \overload

    This function corresponds to setMinimumSize(QSize(minw, minh)).
    Sets the minimum width to \a minw and the minimum height to \a
    minh.
*/

void QWidget::setMinimumSize(int minw, int minh)
{
    Q_D(QWidget);
    if (!d->setMinimumSize_helper(minw, minh))
        return;

    if (isWindow())
        d->setConstraints_sys();
    if (minw > width() || minh > height()) {
        bool resized = testAttribute(Qt::WA_Resized);
        bool maximized = isMaximized();
        resize(qMax(minw,width()), qMax(minh,height()));
        setAttribute(Qt::WA_Resized, resized); //not a user resize
        if (maximized)
            data->window_state = data->window_state | Qt::WindowMaximized;
    }
#ifndef QT_NO_GRAPHICSVIEW
    if (QTLWExtra *topData = d->maybeTopData()) {
        if (topData->proxyWidget)
            topData->proxyWidget->setMinimumSize(minw, minh);
    }
#endif
    d->updateGeometry_helper(d->extra->minw == d->extra->maxw && d->extra->minh == d->extra->maxh);
}

bool QWidgetPrivate::setMaximumSize_helper(int &maxw, int &maxh)
{
    Q_Q(QWidget);
    if (maxw > QWIDGETSIZE_MAX || maxh > QWIDGETSIZE_MAX) {
        qWarning("QWidget::setMaximumSize: (%s/%s) "
                "The largest allowed size is (%d,%d)",
                 q->objectName().toLocal8Bit().data(), q->metaObject()->className(), QWIDGETSIZE_MAX,
                QWIDGETSIZE_MAX);
        maxw = qMin<int>(maxw, QWIDGETSIZE_MAX);
        maxh = qMin<int>(maxh, QWIDGETSIZE_MAX);
    }
    if (maxw < 0 || maxh < 0) {
        qWarning("QWidget::setMaximumSize: (%s/%s) Negative sizes (%d,%d) "
                "are not possible",
                q->objectName().toLocal8Bit().data(), q->metaObject()->className(), maxw, maxh);
        maxw = qMax(maxw, 0);
        maxh = qMax(maxh, 0);
    }
    createExtra();
    if (extra->maxw == maxw && extra->maxh == maxh)
        return false;
    extra->maxw = maxw;
    extra->maxh = maxh;
    return true;
}

/*!
    \overload

    This function corresponds to setMaximumSize(QSize(\a maxw, \a
    maxh)). Sets the maximum width to \a maxw and the maximum height
    to \a maxh.
*/
void QWidget::setMaximumSize(int maxw, int maxh)
{
    Q_D(QWidget);
    if (!d->setMaximumSize_helper(maxw, maxh))
        return;

    if (isWindow())
        d->setConstraints_sys();
    if (maxw < width() || maxh < height()) {
        bool resized = testAttribute(Qt::WA_Resized);
        resize(qMin(maxw,width()), qMin(maxh,height()));
        setAttribute(Qt::WA_Resized, resized); //not a user resize
    }

#ifndef QT_NO_GRAPHICSVIEW
    if (QTLWExtra *topData = d->maybeTopData()) {
        if (topData->proxyWidget)
            topData->proxyWidget->setMaximumSize(maxw, maxh);
    }
#endif

    d->updateGeometry_helper(d->extra->minw == d->extra->maxw && d->extra->minh == d->extra->maxh);
}

/*!
    \overload

    Sets the x (width) size increment to \a w and the y (height) size
    increment to \a h.
*/
void QWidget::setSizeIncrement(int w, int h)
{
    Q_D(QWidget);
    d->createTLExtra();
    QTLWExtra* x = d->topData();
    if (x->incw == w && x->inch == h)
        return;
    x->incw = w;
    x->inch = h;
    if (isWindow())
        d->setConstraints_sys();
}

/*!
    \overload

    This corresponds to setBaseSize(QSize(\a basew, \a baseh)). Sets
    the widgets base size to width \a basew and height \a baseh.
*/
void QWidget::setBaseSize(int basew, int baseh)
{
    Q_D(QWidget);
    d->createTLExtra();
    QTLWExtra* x = d->topData();
    if (x->basew == basew && x->baseh == baseh)
        return;
    x->basew = basew;
    x->baseh = baseh;
    if (isWindow())
        d->setConstraints_sys();
}

/*!
    Sets both the minimum and maximum sizes of the widget to \a s,
    thereby preventing it from ever growing or shrinking.

    This will override the default size constraints set by QLayout.

    Alternatively, if you want the widget to have a
    fixed size based on its contents, you can call
    QLayout::setSizeConstraint(QLayout::SetFixedSize);

    \sa maximumSize, minimumSize
*/

void QWidget::setFixedSize(const QSize & s)
{
    setFixedSize(s.width(), s.height());
}


/*!
    \fn void QWidget::setFixedSize(int w, int h)
    \overload

    Sets the width of the widget to \a w and the height to \a h.
*/

void QWidget::setFixedSize(int w, int h)
{
    Q_D(QWidget);
#ifdef Q_WS_QWS
    // temporary fix for 4.3.x.
    // Should move the embedded spesific contraints in setMinimumSize_helper into QLayout
    int tmpW = w;
    int tmpH = h;
    bool minSizeSet = d->setMinimumSize_helper(tmpW, tmpH);
#else
    bool minSizeSet = d->setMinimumSize_helper(w, h);
#endif
    bool maxSizeSet = d->setMaximumSize_helper(w, h);
    if (!minSizeSet && !maxSizeSet)
        return;

    if (isWindow())
        d->setConstraints_sys();
    else
        d->updateGeometry_helper(true);

    resize(w, h);
}

void QWidget::setMinimumWidth(int w)
{
    Q_D(QWidget);
    d->createExtra();
    uint expl = d->extra->explicitMinSize | (w ? Qt::Horizontal : 0);
    setMinimumSize(w, minimumSize().height());
    d->extra->explicitMinSize = expl;
}

void QWidget::setMinimumHeight(int h)
{
    Q_D(QWidget);
    d->createExtra();
    uint expl = d->extra->explicitMinSize | (h ? Qt::Vertical : 0);
    setMinimumSize(minimumSize().width(), h);
    d->extra->explicitMinSize = expl;
}

void QWidget::setMaximumWidth(int w)
{
    setMaximumSize(w, maximumSize().height());
}

void QWidget::setMaximumHeight(int h)
{
    setMaximumSize(maximumSize().width(), h);
}

/*!
    Sets both the minimum and maximum width of the widget to \a w
    without changing the heights. Provided for convenience.

    \sa sizeHint() minimumSize() maximumSize() setFixedSize()
*/

void QWidget::setFixedWidth(int w)
{
    Q_D(QWidget);
    d->createExtra();
    uint expl = d->extra->explicitMinSize | Qt::Horizontal;
    setMinimumSize(w, minimumSize().height());
    setMaximumSize(w, maximumSize().height());
    d->extra->explicitMinSize = expl;
}


/*!
    Sets both the minimum and maximum heights of the widget to \a h
    without changing the widths. Provided for convenience.

    \sa sizeHint() minimumSize() maximumSize() setFixedSize()
*/

void QWidget::setFixedHeight(int h)
{
    Q_D(QWidget);
    d->createExtra();
    uint expl = d->extra->explicitMinSize | Qt::Vertical;
    setMinimumSize(minimumSize().width(), h);
    setMaximumSize(maximumSize().width(), h);
    d->extra->explicitMinSize = expl;
}


/*!
    Translates the widget coordinate \a pos to the coordinate system
    of \a parent. The \a parent must not be 0 and must be a parent
    of the calling widget.

    \sa mapFrom() mapToParent() mapToGlobal() underMouse()
*/

QPoint QWidget::mapTo(QWidget * parent, const QPoint & pos) const
{
    QPoint p = pos;
    if (parent) {
        const QWidget * w = this;
        while (w != parent) {
            Q_ASSERT_X(w, "QWidget::mapTo(QWidget *parent, const QPoint &pos)",
                       "parent must be in parent hierarchy");
            p = w->mapToParent(p);
            w = w->parentWidget();
        }
    }
    return p;
}


/*!
    Translates the widget coordinate \a pos from the coordinate system
    of \a parent to this widget's coordinate system. The \a parent
    must not be 0 and must be a parent of the calling widget.

    \sa mapTo() mapFromParent() mapFromGlobal() underMouse()
*/

QPoint QWidget::mapFrom(QWidget * parent, const QPoint & pos) const
{
    QPoint p(pos);
    if (parent) {
        const QWidget * w = this;
        while (w != parent) {
            Q_ASSERT_X(w, "QWidget::mapFrom(QWidget *parent, const QPoint &pos)",
                       "parent must be in parent hierarchy");

            p = w->mapFromParent(p);
            w = w->parentWidget();
        }
    }
    return p;
}


/*!
    Translates the widget coordinate \a pos to a coordinate in the
    parent widget.

    Same as mapToGlobal() if the widget has no parent.

    \sa mapFromParent() mapTo() mapToGlobal() underMouse()
*/

QPoint QWidget::mapToParent(const QPoint &pos) const
{
    return pos + data->crect.topLeft();
}

/*!
    Translates the parent widget coordinate \a pos to widget
    coordinates.

    Same as mapFromGlobal() if the widget has no parent.

    \sa mapToParent() mapFrom() mapFromGlobal() underMouse()
*/

QPoint QWidget::mapFromParent(const QPoint &pos) const
{
    return pos - data->crect.topLeft();
}


/*!
    Returns the window for this widget, i.e. the next ancestor widget
    that has (or could have) a window-system frame.

    If the widget is a window, the widget itself is returned.

    Typical usage is changing the window title:

    \snippet doc/src/snippets/code/src.gui.kernel.qwidget.cpp 3

    \sa isWindow()
*/

QWidget *QWidget::window() const
{
    QWidget *w = (QWidget *)this;
    QWidget *p = w->parentWidget();
    while (!w->isWindow() && p) {
        w = p;
        p = p->parentWidget();
    }
    return w;
}

/*!
    Returns the native parent for this widget, i.e. the next ancestor widget
    that has a system identifier, or 0 if it does not have any native parent.

    \sa effectiveWinId()
*/
QWidget *QWidget::nativeParentWidget() const
{
    QWidget *parent = parentWidget();
    while (parent && !parent->internalWinId())
        parent = parent->parentWidget();
    return parent;
}

/*! \fn QWidget *QWidget::topLevelWidget() const
    \obsolete

    Use window() instead.
*/

#ifdef QT3_SUPPORT
/*!
    Returns the color role used for painting the widget's background.

    Use QPalette(backgroundRole(()) instead.
*/
Qt::BackgroundMode QWidget::backgroundMode() const
{
    if (testAttribute(Qt::WA_NoSystemBackground))
        return Qt::NoBackground;
    switch(backgroundRole()) {
    case QPalette::WindowText:
        return Qt::PaletteForeground;
    case QPalette::Button:
        return Qt::PaletteButton;
    case QPalette::Light:
        return Qt::PaletteLight;
    case QPalette::Midlight:
        return Qt::PaletteMidlight;
    case QPalette::Dark:
        return Qt::PaletteDark;
    case QPalette::Mid:
        return Qt::PaletteMid;
    case QPalette::Text:
        return Qt::PaletteText;
    case QPalette::BrightText:
        return Qt::PaletteBrightText;
    case QPalette::Base:
        return Qt::PaletteBase;
    case QPalette::Window:
        return Qt::PaletteBackground;
    case QPalette::Shadow:
        return Qt::PaletteShadow;
    case QPalette::Highlight:
        return Qt::PaletteHighlight;
    case QPalette::HighlightedText:
        return Qt::PaletteHighlightedText;
    case QPalette::ButtonText:
        return Qt::PaletteButtonText;
    case QPalette::Link:
        return Qt::PaletteLink;
    case QPalette::LinkVisited:
        return Qt::PaletteLinkVisited;
    default:
        break;
    }
    return Qt::NoBackground;
}

/*!
    \fn void QWidget::setBackgroundMode(Qt::BackgroundMode
    widgetBackground, Qt::BackgroundMode paletteBackground)

    Sets the color role used for painting the widget's background to
    background mode \a widgetBackground. The \a paletteBackground mode
    parameter is ignored.
*/
void QWidget::setBackgroundMode(Qt::BackgroundMode m, Qt::BackgroundMode)
{
    Q_D(QWidget);
    if(m == Qt::NoBackground) {
        setAttribute(Qt::WA_NoSystemBackground, true);
        return;
    }
    setAttribute(Qt::WA_NoSystemBackground, false);
    d->fg_role = QPalette::NoRole;
    QPalette::ColorRole role = d->bg_role;
    switch(m) {
    case Qt::FixedColor:
    case Qt::FixedPixmap:
        break;
    case Qt::PaletteForeground:
        role = QPalette::WindowText;
        break;
    case Qt::PaletteButton:
        role = QPalette::Button;
        break;
    case Qt::PaletteLight:
        role = QPalette::Light;
        break;
    case Qt::PaletteMidlight:
        role = QPalette::Midlight;
        break;
    case Qt::PaletteDark:
        role = QPalette::Dark;
        break;
    case Qt::PaletteMid:
        role = QPalette::Mid;
        break;
    case Qt::PaletteText:
        role = QPalette::Text;
        break;
    case Qt::PaletteBrightText:
        role = QPalette::BrightText;
        break;
    case Qt::PaletteBase:
        role = QPalette::Base;
        break;
    case Qt::PaletteBackground:
        role = QPalette::Window;
        break;
    case Qt::PaletteShadow:
        role = QPalette::Shadow;
        break;
    case Qt::PaletteHighlight:
        role = QPalette::Highlight;
        break;
    case Qt::PaletteHighlightedText:
        role = QPalette::HighlightedText;
        break;
    case Qt::PaletteButtonText:
        role = QPalette::ButtonText;
        break;
    case Qt::PaletteLink:
        role = QPalette::Link;
        break;
    case Qt::PaletteLinkVisited:
        role = QPalette::LinkVisited;
        break;
    case Qt::X11ParentRelative:
        d->fg_role = role = QPalette::NoRole;
    default:
        break;
    }
    setBackgroundRole(role);
}

/*!
    The widget mapper is no longer part of the public API.
*/
QT3_SUPPORT QWidgetMapper *QWidget::wmapper() { return QWidgetPrivate::mapper; }

#endif


/*!
  Returns the background role of the widget.

  The background role defines the brush from the widget's \l palette that
  is used to render the background.

  If no explicit background role is set, the widget inherts its parent
  widget's background role.

  \sa setBackgroundRole(), foregroundRole()
 */
QPalette::ColorRole QWidget::backgroundRole() const
{

    const QWidget *w = this;
    do {
        QPalette::ColorRole role = w->d_func()->bg_role;
        if (role != QPalette::NoRole)
            return role;
        if (w->isWindow() || w->windowType() == Qt::SubWindow)
            break;
        w = w->parentWidget();
    } while (w);
    return QPalette::Window;
}

/*!
  Sets the background role of the widget to \a role.

  The background role defines the brush from the widget's \l palette that
  is used to render the background.

  If \a role is QPalette::NoRole, then the widget inherits its
  parent's background role.

  \sa backgroundRole(), foregroundRole()
 */

void QWidget::setBackgroundRole(QPalette::ColorRole role)
{
    Q_D(QWidget);
    d->bg_role = role;
    d->updateSystemBackground();
    d->propagatePaletteChange();
    d->updateIsOpaque();
}

/*!
  Returns the foreground role.

  The foreground role defines the color from the widget's \l palette that
  is used to draw the foreground.

  If no explicit foreground role is set, the function returns a role
  that contrasts with the background role.

  \sa setForegroundRole(), backgroundRole()
 */
QPalette::ColorRole QWidget::foregroundRole() const
{
    Q_D(const QWidget);
    QPalette::ColorRole rl = QPalette::ColorRole(d->fg_role);
    if (rl != QPalette::NoRole)
        return rl;
    QPalette::ColorRole role = QPalette::WindowText;
    switch (backgroundRole()) {
    case QPalette::Button:
        role = QPalette::ButtonText;
        break;
    case QPalette::Base:
        role = QPalette::Text;
        break;
    case QPalette::Dark:
    case QPalette::Shadow:
        role = QPalette::Light;
        break;
    case QPalette::Highlight:
        role = QPalette::HighlightedText;
        break;
    case QPalette::ToolTipBase:
        role = QPalette::ToolTipText;
        break;
    default:
        ;
    }
    return role;
}

/*!
  Sets the foreground role of the widget to \a role.

  The foreground role defines the color from the widget's \l palette that
  is used to draw the foreground.

  If \a role is QPalette::NoRole, the widget uses a foreground role
  that contrasts with the background role.

  \sa foregroundRole(), backgroundRole()
 */
void QWidget::setForegroundRole(QPalette::ColorRole role)
{
    Q_D(QWidget);
    d->fg_role = role;
    d->updateSystemBackground();
    d->propagatePaletteChange();
}

/*!
    \property QWidget::palette
    \brief the widget's palette

    As long as no special palette has been set, this is either a
    special palette for the widget class, the parent's palette or (if
    this widget is a top level widget), the default application
    palette.

    \bold{Note:} The palette's background color will only have an effect on
    the appearance of the widget if the \l autoFillBackground property is
    set.

    \warning Do not use this function in conjunction with \l{Qt Style Sheets}.
    When using style sheets, the palette of a widget can be customized using the "color",
    "background-color", "selection-color", "selection-background-color" and
    "alternate-background-color".

    \sa QApplication::palette()
*/
const QPalette &QWidget::palette() const
{
    if (!isEnabled()) {
        data->pal.setCurrentColorGroup(QPalette::Disabled);
    } else if (!isVisible() || isActiveWindow()) {
        data->pal.setCurrentColorGroup(QPalette::Active);
    } else {
#ifdef Q_WS_MAC
        extern bool qt_mac_can_clickThrough(const QWidget *); //qwidget_mac.cpp
        if (qt_mac_can_clickThrough(this))
            data->pal.setCurrentColorGroup(QPalette::Active);
        else
#endif
            data->pal.setCurrentColorGroup(QPalette::Inactive);
    }
    return data->pal;
}

void QWidget::setPalette(const QPalette &palette)
{
    Q_D(QWidget);
    setAttribute(Qt::WA_SetPalette, palette.resolve() != 0);
    d->setPalette_helper(palette.resolve(qt_naturalWidgetPalette(this)));
}

void QWidgetPrivate::resolvePalette()
{
    Q_Q(QWidget);
    setPalette_helper(data.pal.resolve(qt_naturalWidgetPalette(q)));
}

void QWidgetPrivate::setPalette_helper(const QPalette &palette)
{
    Q_Q(QWidget);
    if (data.pal == palette && data.pal.resolve() == palette.resolve())
        return;
    data.pal = palette;
    updateSystemBackground();
    propagatePaletteChange();
    updateIsOpaque();
    q->update();
    updateIsOpaque();
}


/*!
    \property QWidget::font
    \brief the font currently set for the widget

    The fontInfo() function reports the actual font that is being used
    by the widget.

    As long as no special font has been set, or after setFont(QFont())
    is called, this is either a special font for the widget class, the
    parent's font or (if this widget is a top level widget), the
    default application font.

    This code fragment sets a 12 point helvetica bold font:

    \snippet doc/src/snippets/code/src.gui.kernel.qwidget.cpp 4

    Note that when a child widget is given a different font to that of
    its parent widget, it will still inherit the parent's font \e
    properties unless these have been set explicitly on the child's
    font. For example, if the parent's font is bold, the child
    widget's font will be bold as well if not specified otherwise like
    this:

    \snippet doc/src/snippets/code/src.gui.kernel.qwidget.cpp 5

    In addition to setting the font, setFont() informs all children
    about the change.

    \note If \l{Qt Style Sheets} are used on the same widget as setFont(),
    style sheets will take precedence if the settings conflict.

    \sa fontInfo() fontMetrics()
*/

void QWidget::setFont(const QFont &font)
{
    Q_D(QWidget);

#ifndef QT_NO_STYLE_STYLESHEET
    const QStyleSheetStyle* style;
    if (d->extra && (style = qobject_cast<const QStyleSheetStyle*>(d->extra->style))) {
        style->saveWidgetFont(this, font);
    }
#endif

    setAttribute(Qt::WA_SetFont, font.resolve() != 0);
    d->setFont_helper(font.resolve(qt_naturalWidgetFont(this)));
}

void QWidgetPrivate::resolveFont()
{
    Q_Q(QWidget);
    setFont_helper(data.fnt.resolve(qt_naturalWidgetFont(q)));
}

void QWidgetPrivate::setFont_helper(const QFont &font)
{
    if (data.fnt == font && data.fnt.resolve() == font.resolve())
        return;

    Q_Q(QWidget);
#ifndef QT_NO_STYLE_STYLESHEET
    const QStyleSheetStyle* cssStyle;
    cssStyle = extra ? qobject_cast<const QStyleSheetStyle*>(extra->style) : 0;
#endif

#ifdef QT3_SUPPORT
    QFont old = data.fnt;
#endif
    data.fnt = font;
#if defined(Q_WS_X11)
    // make sure the font set on this widget is associated with the correct screen
    data.fnt.x11SetScreen(xinfo.screen());
#endif
    for (int i = 0; i < children.size(); ++i) {
        QWidget *w = qobject_cast<QWidget*>(children.at(i));
        if (w) {
#ifndef QT_NO_STYLE_STYLESHEET
            if (w->testAttribute(Qt::WA_StyleSheet))
                cssStyle->updateStyleSheetFont(w);
            else
#endif
            if ((!w->isWindow() || w->testAttribute(Qt::WA_WindowPropagation)))
                w->d_func()->resolveFont();
        }
    }

#ifndef QT_NO_STYLE_STYLESHEET
    if (cssStyle) {
        cssStyle->updateStyleSheetFont(q);
    }
#endif

    QEvent e(QEvent::FontChange);
    QApplication::sendEvent(q, &e);
#ifdef QT3_SUPPORT
    q->fontChange(old);
#endif
}

void QWidgetPrivate::setLayoutDirection_helper(Qt::LayoutDirection direction)
{
    Q_Q(QWidget);

    if ( (direction == Qt::RightToLeft) == q->testAttribute(Qt::WA_RightToLeft))
        return;
    q->setAttribute(Qt::WA_RightToLeft, (direction == Qt::RightToLeft));
    if (!children.isEmpty()) {
        for (int i = 0; i < children.size(); ++i) {
            QWidget *w = qobject_cast<QWidget*>(children.at(i));
            if (w && !w->isWindow() && !w->testAttribute(Qt::WA_SetLayoutDirection))
                w->d_func()->setLayoutDirection_helper(direction);
        }
    }
    QEvent e(QEvent::LayoutDirectionChange);
    QApplication::sendEvent(q, &e);
}

void QWidgetPrivate::resolveLayoutDirection()
{
    Q_Q(const QWidget);
    if (!q->testAttribute(Qt::WA_SetLayoutDirection))
        setLayoutDirection_helper(q->isWindow() ? QApplication::layoutDirection() : q->parentWidget()->layoutDirection());
}

/*!
    \property QWidget::layoutDirection

    \brief the layout direction for this widget

    \sa QApplication::layoutDirection
*/
void QWidget::setLayoutDirection(Qt::LayoutDirection direction)
{
    Q_D(QWidget);

    setAttribute(Qt::WA_SetLayoutDirection);
    d->setLayoutDirection_helper(direction);
}

Qt::LayoutDirection QWidget::layoutDirection() const
{
    return testAttribute(Qt::WA_RightToLeft) ? Qt::RightToLeft : Qt::LeftToRight;
}

void QWidget::unsetLayoutDirection()
{
    Q_D(QWidget);
    setAttribute(Qt::WA_SetLayoutDirection, false);
    d->resolveLayoutDirection();
}

/*!
    \fn QFontMetrics QWidget::fontMetrics() const

    Returns the font metrics for the widget's current font.
    Equivalent to QFontMetrics(widget->font()).

    \sa font(), fontInfo(), setFont()
*/

/*!
    \fn QFontInfo QWidget::fontInfo() const

    Returns the font info for the widget's current font.
    Equivalent to QFontInto(widget->font()).

    \sa font(), fontMetrics(), setFont()
*/


/*!
    \property QWidget::cursor
    \brief the cursor shape for this widget

    The mouse cursor will assume this shape when it's over this
    widget. See the \link Qt::CursorShape list of predefined cursor
    objects\endlink for a range of useful shapes.

    An editor widget might use an I-beam cursor:
    \snippet doc/src/snippets/code/src.gui.kernel.qwidget.cpp 6

    If no cursor has been set, or after a call to unsetCursor(), the
    parent's cursor is used.

    \sa QApplication::setOverrideCursor()
*/

#ifndef QT_NO_CURSOR
QCursor QWidget::cursor() const
{
    Q_D(const QWidget);
    if (testAttribute(Qt::WA_SetCursor))
        return (d->extra && d->extra->curs)
            ? *d->extra->curs
            : QCursor(Qt::ArrowCursor);
    if (isWindow() || !parentWidget())
        return QCursor(Qt::ArrowCursor);
    return parentWidget()->cursor();
}

void QWidget::setCursor(const QCursor &cursor)
{
    Q_D(QWidget);
// On Mac we must set the cursor even if it is the ArrowCursor.
#ifndef Q_WS_MAC
    if (cursor.shape() != Qt::ArrowCursor
        || (d->extra && d->extra->curs))
#endif
    {
        d->createExtra();
        delete d->extra->curs;
        d->extra->curs = new QCursor(cursor);
    }
    setAttribute(Qt::WA_SetCursor);
    d->setCursor_sys(cursor);

    QEvent event(QEvent::CursorChange);
    QApplication::sendEvent(this, &event);
}

void QWidget::unsetCursor()
{
    Q_D(QWidget);
    if (d->extra) {
        delete d->extra->curs;
        d->extra->curs = 0;
    }
    if (!isWindow())
        setAttribute(Qt::WA_SetCursor, false);
    d->unsetCursor_sys();

    QEvent event(QEvent::CursorChange);
    QApplication::sendEvent(this, &event);
}

#endif

/*!
    \enum QWidget::RenderFlag

    This enum describes how to render the widget when calling QWidget::render().

    \value DrawWindowBackground If you enable this option, the widget's background
    is rendered into the target even if autoFillBackground is not set. By default,
    this option is enabled.

    \value DrawChildren If you enable this option, the widget's children
    are rendered recursively into the target. By default, this option is enabled.

    \value IgnoreMask If you enable this option, the widget's QWidget::mask()
    is ignored when rendering into the target. By default, this option is disabled.

    \since 4.3
*/

/*!
    Renders the \a sourceRegion of this widget into the \a target
    using \a renderFlags to determine how to render. Rendering
    starts at \a targetOffset in the \a target. For example:

    \snippet doc/src/snippets/code/src.gui.kernel.qwidget.cpp 7

    If \a sourceRegion is a null region, this function will use QWidget::rect() as
    the region, i.e. the entire widget.

    \bold{Note:} Make sure to call QPainter::end() for the given \a target's
    active painter (if any) before rendering. For example:

    \snippet doc/src/snippets/code/src.gui.kernel.qwidget.cpp 8

    \since 4.3
*/
void QWidget::render(QPaintDevice *target, const QPoint &targetOffset,
                     const QRegion &sourceRegion, RenderFlags renderFlags)
{
    Q_D(QWidget);
    if (!target) {
        qWarning("QWidget::render: null pointer to paint device");
        return;
    }

    QWidget *topLevel = window();
    QTLWExtra *tlwExtra = topLevel->d_func()->maybeTopData();

    QRegion paintRegion = (!tlwExtra || !tlwExtra->inRenderWithPainter)
                          ? d->prepareToRender(sourceRegion, renderFlags, topLevel) : sourceRegion;
    if (paintRegion.isEmpty())
        return;

    QPainter *oldSharedPainter = tlwExtra ? tlwExtra->sharedPainter : 0;

    // Use the target's shared painter if set (typically set when doing
    // "other->render(widget);" in the widget's paintEvent.
    if (target->devType() == QInternal::Widget) {
        const QTLWExtra *targetExtra = static_cast<QWidget *>(target)->window()->d_func()->maybeTopData();
        if (targetExtra && targetExtra->sharedPainter && targetExtra->sharedPainter->isActive()) {
            if (!tlwExtra)
                tlwExtra = topLevel->d_func()->topData();
            tlwExtra->sharedPainter = targetExtra->sharedPainter;
        }
    }

    // Use the target's redirected device if set and adjust offset and paint
    // region accordingly. This is typically the case when people call render
    // from the paintEvent.
    QPoint offset = targetOffset;
    QPoint redirectionOffset;
    QPaintDevice *redirected = 0;

    if (target->devType() == QInternal::Widget)
        redirected = static_cast<QWidget *>(target)->d_func()->redirected(&redirectionOffset);
    if (!redirected)
        redirected = QPainter::redirected(target, &redirectionOffset);

    if (redirected) {
        target = redirected;
        offset -= redirectionOffset;
        const QRegion redirectedSystemClip = redirected->paintEngine()->systemClip();
        if (!redirectedSystemClip.isEmpty())
            paintRegion &= redirectedSystemClip.translated(-offset);
    }

    offset -= paintRegion.boundingRect().topLeft();

    // Set backingstore flags.
    int flags = QWidgetPrivate::DrawPaintOnScreen | QWidgetPrivate::DrawInvisible;
    if (renderFlags & DrawWindowBackground)
        flags |= QWidgetPrivate::DrawAsRoot;

    if (renderFlags & DrawChildren)
        flags |= QWidgetPrivate::DrawRecursive;
    else
        flags |= QWidgetPrivate::DontSubtractOpaqueChildren;

#ifdef Q_WS_QWS
    flags |= QWidgetPrivate::DontSetCompositionMode;
#endif

    QPainter *sharedPainter = tlwExtra && tlwExtra->sharedPainter && tlwExtra->sharedPainter->isActive()
                              ? tlwExtra->sharedPainter : 0;

    // Render via backingstore.
    d->drawWidget(target, paintRegion, offset, flags, sharedPainter);

    if (tlwExtra)
        tlwExtra->sharedPainter = oldSharedPainter;
}

/*!
    \overload

    Renders the widget into the \a painter's QPainter::device().

    Transformations and settings applied to the \a painter will be used
    when rendering.

    \note The \a painter must be active. On Mac OS X the widget will be
    rendered into a QPixmap and then drawn by the \a painter.

    \sa QPainter::device()
*/
void QWidget::render(QPainter *painter, const QPoint &targetOffset,
                     const QRegion &sourceRegion, RenderFlags renderFlags)
{
    if (!painter) {
        qWarning("QWidget::render: Null pointer to painter");
        return;
    }

    if (!painter->isActive()) {
        qWarning("QWidget::render: Cannot render with an inactive painter");
        return;
    }

    Q_D(QWidget);
    QWidget *topLevel = window();
    const QRegion toBePainted = d->prepareToRender(sourceRegion, renderFlags, topLevel);
    if (toBePainted.isEmpty())
        return;

    QTLWExtra *extra = topLevel->d_func()->topData();
    extra->inRenderWithPainter = true;
    QPainter *oldPainter = extra->sharedPainter;
#ifndef Q_WS_MAC
    extra->sharedPainter = painter;
#endif

    QPaintEngine *engine = painter->paintEngine();
    Q_ASSERT(engine);
    QPaintEnginePrivate *enginePriv = engine->d_func();
    Q_ASSERT(enginePriv);
    QPaintDevice *target = painter->worldMatrixEnabled() ? engine->paintDevice() : painter->device();
    Q_ASSERT(target);

    // Save current system clip.
    const QRegion oldSystemClip = enginePriv->systemClip;
    enginePriv->oldSystemClipPath.addRegion(oldSystemClip);

#ifdef Q_WS_MAC
    Q_UNUSED(target);
    // Render via a pixmap.
    const QRect rect = toBePainted.boundingRect();
    const QSize size = rect.size();
    if (size.isNull()) {
        extra->inRenderWithPainter = false;
        return;
    }

    QPixmap pixmap(size);
    render(&pixmap, QPoint(), toBePainted, renderFlags);

    const bool restore = painter->renderHints() & QPainter::SmoothPixmapTransform;
    painter->setRenderHints(QPainter::SmoothPixmapTransform, true);

    painter->drawPixmap(QRect(targetOffset, size), pixmap);

    if (restore)
        painter->setRenderHints(QPainter::SmoothPixmapTransform, false);
#else
    const qreal oldGlobalOpacity = extra->globalOpacity;
    extra->globalOpacity = painter->opacity();
    render(target, targetOffset, toBePainted, renderFlags);
    extra->globalOpacity = oldGlobalOpacity;
#endif

    // Restore system clip.
    if (engine->type() == QPaintEngine::Raster)
        static_cast<QRasterPaintEnginePrivate *>(enginePriv)->baseClip = enginePriv->oldSystemClipPath;
    enginePriv->systemClip = oldSystemClip;
    enginePriv->oldSystemClipPath = QPainterPath();

    // Update painter state with new clip.
    QPaintEngineState *state = engine->state;
    Q_ASSERT(state);
    state->dirtyFlags |= QPaintEngine::DirtyClipEnabled | QPaintEngine::DirtyClipRegion;
    engine->updateState(*state);

    extra->sharedPainter = oldPainter;
    extra->inRenderWithPainter = false;
}

QRegion QWidgetPrivate::prepareToRender(const QRegion &region, QWidget::RenderFlags renderFlags, QWidget *topLevel)
{
    if (!topLevel)
        return QRegion();

    Q_Q(QWidget);

    // Make sure the widget is laid out correctly.
    if (!q->isVisible()) {
        if (!topLevel->testAttribute(Qt::WA_WState_Created))
            topLevel->d_func()->createWinId();
        topLevel->ensurePolished();

        // Invalidate the layout of hidden ancestors (incl. myself) and pretend
        // they're not explicitly hidden.
        QWidget *widget = q;
        QWidgetList hiddenWidgets;
        while (widget) {
            if (widget->isHidden()) {
                widget->setAttribute(Qt::WA_WState_Hidden, false);
                hiddenWidgets.append(widget);
                if (!widget->isWindow() && widget->parentWidget()->d_func()->layout)
                    widget->d_func()->updateGeometry_helper(true);
            }
            widget = widget->parentWidget();
        }

        // Activate top-level layout.
        if (topLevel->d_func()->layout)
            topLevel->d_func()->layout->activate();

        // Adjust size if necessary.
        QTLWExtra *topLevelExtra = topLevel->d_func()->maybeTopData();
        if (topLevelExtra && !topLevelExtra->sizeAdjusted
            && !topLevel->testAttribute(Qt::WA_Resized)) {
            topLevel->adjustSize();
            topLevel->setAttribute(Qt::WA_Resized, false);
        }

        // Activate child layouts.
        topLevel->d_func()->activateChildLayoutsRecursively();

        // We're not cheating with WA_WState_Hidden anymore.
        for (int i = 0; i < hiddenWidgets.size(); ++i) {
            QWidget *widget = hiddenWidgets.at(i);
            widget->setAttribute(Qt::WA_WState_Hidden);
            if (!widget->isWindow() && widget->parentWidget()->d_func()->layout)
                widget->parentWidget()->d_func()->layout->invalidate();
        }
    } else {
        topLevel->d_func()->sendPendingMoveAndResizeEvents(true, true);
    }

    // Calculate the region to be painted.
    QRegion toBePainted = !region.isEmpty() ? region : QRegion(q->rect());
    if (!(renderFlags & QWidget::IgnoreMask) && extra && !extra->mask.isEmpty())
        toBePainted &= extra->mask;
    return toBePainted;
}

void QWidgetPrivate::drawWidget(QPaintDevice *pdev, const QRegion &rgn, const QPoint &offset,
                                int flags, QPainter *sharedPainter)
{
    Q_Q(QWidget);
    if (rgn.isEmpty())
        return;

    const bool asRoot = flags & DrawAsRoot;
    const bool alsoOnScreen = flags & DrawPaintOnScreen;
    const bool recursive = flags & DrawRecursive;
    const bool alsoInvisible = flags & DrawInvisible;

    Q_ASSERT(sharedPainter ? sharedPainter->isActive() : true);

    QRegion toBePainted = rgn;
    if (asRoot && !alsoInvisible)
        toBePainted &= clipRect(); //(rgn & visibleRegion());
#ifndef Q_FLATTEN_EXPOSE
    if (!(flags & DontSubtractOpaqueChildren))
        subtractOpaqueChildren(toBePainted, q->rect(), QPoint());
#endif

    if (!toBePainted.isEmpty()) {
        bool onScreen = paintOnScreen();
        if (!onScreen || alsoOnScreen) {
            //update the "in paint event" flag
            if (q->testAttribute(Qt::WA_WState_InPaintEvent))
                qWarning("QWidget::repaint: Recursive repaint detected");
            q->setAttribute(Qt::WA_WState_InPaintEvent);

            //clip away the new area
#ifndef QT_NO_PAINT_DEBUG
            bool flushed = qt_flushPaint(q, toBePainted);
#endif
            QPaintEngine *paintEngine = pdev->paintEngine();
            if (paintEngine) {
                setRedirected(pdev, -offset);

                if (sharedPainter) {
                    if (!sharedPainter->worldMatrixEnabled())
                        paintEngine->d_func()->systemClip = toBePainted.translated(offset);
                    else
                        paintEngine->d_func()->systemClip = toBePainted;
                } else {
                    paintEngine->setSystemRect(q->data->crect);
                    paintEngine->setSystemClip(toBePainted.translated(offset));
                }

                //paint the background
                if ((asRoot || q->autoFillBackground() || onScreen || q->testAttribute(Qt::WA_StyledBackground))
                    && !q->testAttribute(Qt::WA_OpaquePaintEvent) && !q->testAttribute(Qt::WA_NoSystemBackground)) {

                    QPainter p(q);
                    QRect backgroundRect = toBePainted.boundingRect();

#ifndef QT_NO_SCROLLAREA
                    if (QAbstractScrollArea *scrollArea = qobject_cast<QAbstractScrollArea *>(q->parent())) {
                        if (scrollArea->viewport() == q) {
                            QAbstractScrollAreaPrivate *priv = static_cast<QAbstractScrollAreaPrivate *>(static_cast<QWidget *>(scrollArea)->d_ptr);
                            const QPoint offset = priv->contentsOffset();
                            p.translate(-offset);
                            backgroundRect.translate(offset);
                        }
                    }
#endif // QT_NO_SCROLLAREA
                    paintBackground(&p, backgroundRect, (asRoot || onScreen) ? flags | DrawAsRoot : 0);
                }
                if (q->testAttribute(Qt::WA_TintedBackground) && !onScreen && !asRoot && !isOpaque()) {
                    QPainter p(q);
                    QColor tint = q->palette().window().color();
                    tint.setAlphaF(qreal(.6));
                    p.fillRect(toBePainted.boundingRect(), tint);
                }
            }

#if 0
            qDebug() << "painting" << q << "opaque ==" << isOpaque();
            qDebug() << "clipping to" << toBePainted << "location == " << offset
                     << "geometry ==" << QRect(q->mapTo(q->window(), QPoint(0, 0)), q->size());
#endif

            //actually send the paint event
            QPaintEvent e(toBePainted);
            qt_sendSpontaneousEvent(q, &e);

            //restore
            if (paintEngine) {
                restoreRedirected();
                if (!sharedPainter) {
                    paintEngine->setSystemRect(QRect());
                    pdev->paintEngine()->setSystemClip(QRegion());
                } else {
                    paintEngine->d_func()->systemClip = QRegion();
                    paintEngine->d_func()->currentClipWidget = 0;
                }
            }
            q->setAttribute(Qt::WA_WState_InPaintEvent, false);
            if(!q->testAttribute(Qt::WA_PaintOutsidePaintEvent) && q->paintingActive())
                qWarning("QWidget::repaint: It is dangerous to leave painters active on a widget outside of the PaintEvent");

#ifndef QT_NO_PAINT_DEBUG
            if (flushed)
                QWidgetBackingStore::qt_unflushPaint(q, toBePainted);
#endif
        } else if (q->isWindow()) {
            if (pdev->paintEngine()) {
                QPainter p(pdev);
                p.setClipRegion(toBePainted);
                const QBrush bg = q->palette().brush(QPalette::Window);
                if (bg.style() == Qt::TexturePattern)
                    p.drawTiledPixmap(q->rect(), bg.texture());
                else
                    p.fillRect(q->rect(), bg);
            }
        }
    }

    if (recursive) {
        const QObjectList children = q->children();
        if (!children.isEmpty()) {
            paintSiblingsRecursive(pdev, children, children.size() - 1, rgn, offset, flags & ~DrawAsRoot
#ifdef Q_BACKINGSTORE_SUBSURFACES
                                   , q->windowSurface()
#endif
                                   , sharedPainter);
        }
    }
}

void QWidgetPrivate::paintSiblingsRecursive(QPaintDevice *pdev, const QObjectList& siblings, int index, const QRegion &rgn,
                                            const QPoint &offset, int flags
#ifdef Q_BACKINGSTORE_SUBSURFACES
                                            , const QWindowSurface *currentSurface
#endif
                                            , QPainter *sharedPainter
    )
{
    QWidget *w = 0;

    do {
        QWidget *x =  qobject_cast<QWidget*>(siblings.at(index));
        if (x && !x->isWindow() && !x->isHidden() && qRectIntersects(rgn.boundingRect(), x->geometry())) {
#ifdef Q_BACKINGSTORE_SUBSURFACES
            if (x->windowSurface() == currentSurface)
#endif
            {
                w = x;
                break;
            }
        }
        --index;
    } while (index >= 0);


    if (!w)
        return;

    QWExtra *extra = w->d_func()->extraData();

    if (index > 0) {
        QRegion wr = rgn;
        if (w->d_func()->isOpaque()) {
            if (!extra || extra->mask.isEmpty())
                wr -= w->geometry();
            else
                wr -= extra->mask.translated(w->pos());
        }
        paintSiblingsRecursive(pdev, siblings, index - 1, wr, offset, flags
#ifdef Q_BACKINGSTORE_SUBSURFACES
                               , currentSurface
#endif
                               , sharedPainter);
    }

    if (w->updatesEnabled()) {
        QRegion wRegion(rgn & w->geometry());
        wRegion.translate(-w->pos());

        if (extra && !extra->mask.isEmpty())
            wRegion &= extra->mask;
        if (!wRegion.isEmpty())
            w->d_func()->drawWidget(pdev, wRegion, offset + w->pos(), flags, sharedPainter);
    }
}

QPainter *QWidgetPrivate::beginSharedPainter()
{
    static int disableSharedPainter = qgetenv("QT_NO_SHARED_PAINTER").toInt();
    if (disableSharedPainter)
        return 0;

    Q_Q(QWidget);

#if !defined(QT_NO_DIRECT3D) && defined(Q_WS_WIN)
    if (qApp->testAttribute(Qt::AA_MSWindowsUseDirect3DByDefault)
        || q->testAttribute(Qt::WA_MSWindowsUseDirect3D))
    {
        return 0;
    }
#endif

    QWidget *tlw = q->isWindow() ? q : q->window();
    QTLWExtra *tlwExtra = tlw->d_func()->topData();
    Q_ASSERT(tlwExtra);

#ifdef QT_GREENPHONE_OPT
    // Only use shared painter for graphics view widgets
    if (!tlwExtra->proxyWidget)
        return 0;
#endif

    if (tlwExtra->sharedPainter && tlwExtra->sharedPainter->isActive()) {
        qWarning("QWidgetPrivate::beginSharedPainter: Painter is already active");
        return tlwExtra->sharedPainter;
    }

    // Find a paint device to paint on.
    QPaintDevice *paintDevice = 0;
    if (tlwExtra->sharedPainter && tlwExtra->sharedPainter->device())
        paintDevice = tlwExtra->sharedPainter->device();
    else if (QWindowSurface *windowSurface = tlw->windowSurface())
        paintDevice = windowSurface->paintDevice();

    // Verify paint device.
    if (!paintDevice) {
        qWarning("QWidgetPrivate::beginSharedPainter: No paint device found");
        return 0;
    }

    if (!paintDevice->paintEngine()) {
        qWarning("QWidgetPrivate::beginSharedPainter: No paint engine for paint device");
        return 0;
    }

    if (paintDevice->width() <= 0 || paintDevice->height() <= 0) {
        qWarning("QWidgetPrivate::beginSharedPainter: Paint device has invalid size");
        return 0;
    }

    if (!tlwExtra->sharedPainter)
        tlwExtra->sharedPainter = new QPainter(paintDevice);
    else
        tlwExtra->sharedPainter->begin(paintDevice);

    return tlwExtra->sharedPainter->isActive() ? tlwExtra->sharedPainter : 0;
}

bool QWidgetPrivate::endSharedPainter()
{
    QTLWExtra *tlwExtra = q_func()->window()->d_func()->maybeTopData();
    if (!tlwExtra || !tlwExtra->sharedPainter) {
        qWarning("QWidgetPrivate::endSharedPainter: Attempting to end a null painter");
        return false;
    }

    if (!tlwExtra->sharedPainter->isActive()) {
        qWarning("QWidgetPrivate::endSharedPainter: Painter is not active");
        return false;
    }

    delete tlwExtra->sharedPainter;
    tlwExtra->sharedPainter = 0;

    return true;
}

/*!
    \internal

    Finds the nearest widget embedded in a graphics proxy widget along the chain formed by this
    widget and its ancestors. The search starts at \a origin (inclusive).
    If successful, the function returns the proxy that embeds the widget, or 0 if no embedded
    widget was found.
*/
QGraphicsProxyWidget * QWidgetPrivate::nearestGraphicsProxyWidget(QWidget *origin)
{
    if (origin) {
        QTLWExtra *topData = origin->d_func()->maybeTopData();
        if (topData && topData->proxyWidget)
            return topData->proxyWidget;
        return nearestGraphicsProxyWidget(origin->parentWidget());
    }
    return 0;
}

/*!
    \property QWidget::locale
    \brief the widget's locale
    \since 4.3

    As long as no special locale has been set, this is either
    the parent's locale or (if this widget is a top level widget),
    the default locale.

    If the widget displays dates or numbers, these should be formatted
    using the widget's locale.

    \sa QLocale QLocale::setDefault()
*/

void QWidgetPrivate::setLocale_helper(const QLocale &loc)
{
    Q_Q(QWidget);

    if (locale == loc)
        return;

    locale = loc;

    if (!children.isEmpty()) {
        for (int i = 0; i < children.size(); ++i) {
            QWidget *w = qobject_cast<QWidget*>(children.at(i));
            if (!w)
                continue;
            if (w->testAttribute(Qt::WA_SetLocale))
                continue;
            if (w->isWindow() && !w->testAttribute(Qt::WA_WindowPropagation))
                continue;
            w->d_func()->setLocale_helper(loc);
        }
    }

    QEvent e(QEvent::LocaleChange);
    QApplication::sendEvent(q, &e);
}

void QWidget::setLocale(const QLocale &locale)
{
    Q_D(QWidget);

    setAttribute(Qt::WA_SetLocale);
    d->setLocale_helper(locale);
}

QLocale QWidget::locale() const
{
    Q_D(const QWidget);

    return d->locale;
}

void QWidgetPrivate::resolveLocale()
{
    Q_Q(const QWidget);

    if (!q->testAttribute(Qt::WA_SetLocale)) {
        setLocale_helper(q->isWindow()
                            ? QLocale()
                            : q->parentWidget()->locale());
    }
}

void QWidget::unsetLocale()
{
    Q_D(QWidget);
    setAttribute(Qt::WA_SetLocale, false);
    d->resolveLocale();
}

static QString constructWindowTitleFromFilePath(const QString &filePath)
{
    QFileInfo fi(filePath);
    QString windowTitle = fi.fileName() + QLatin1String("[*]");
#ifndef Q_WS_MAC
    QString appName = QApplication::applicationName();
    if (!appName.isEmpty())
        windowTitle += QLatin1String(" ") + QChar(0x2014) + QLatin1String(" ") + appName;
#endif
    return windowTitle;
}

/*!
    \property QWidget::windowTitle
    \brief the window title (caption)

    This property only makes sense for top-level widgets, such as
    windows and dialogs. If no caption has been set, the title is based of the
    \l windowFilePath. If neither of these is set, then the title is
    an empty string.

    If you use the \l windowModified mechanism, the window title must
    contain a "[*]" placeholder, which indicates where the '*' should
    appear. Normally, it should appear right after the file name
    (e.g., "document1.txt[*] - Text Editor"). If the \l
    windowModified property is false (the default), the placeholder
    is simply removed.

    \sa windowIcon, windowIconText, windowModified, windowFilePath
*/
QString QWidget::windowTitle() const
{
    Q_D(const QWidget);
    if (d->extra && d->extra->topextra) {
        if (!d->extra->topextra->caption.isEmpty())
            return d->extra->topextra->caption;
        if (!d->extra->topextra->filePath.isEmpty())
            return constructWindowTitleFromFilePath(d->extra->topextra->filePath);
    }
    return QString();
}

QString qt_setWindowTitle_helperHelper(const QString &title, const QWidget *widget)
{
    Q_ASSERT(widget);

#ifdef QT_EVAL
    extern QString qt_eval_adapt_window_title(const QString &title);
    QString cap = qt_eval_adapt_window_title(title);
#else
    QString cap = title;
#endif

    QString placeHolder(QLatin1String("[*]"));

    int index = cap.indexOf(placeHolder);

    while (index != -1) {
        index += placeHolder.length();
        int count = 1;
        while (cap.indexOf(placeHolder, index) == index) {
            ++count;
            index += placeHolder.length();
        }

        if (count%2) { // odd number of [*] -> replace last one
            int lastIndex = cap.lastIndexOf(placeHolder, index - 1);
            if (widget->isWindowModified()
             && widget->style()->styleHint(QStyle::SH_TitleBar_ModifyNotification, 0, widget))
                cap.replace(lastIndex, 3, QWidget::tr("*"));
            else
                cap.replace(lastIndex, 3, QLatin1String(""));
        }

        index = cap.indexOf(placeHolder, index);
    }

    cap.replace(QLatin1String("[*][*]"), QLatin1String("[*]"));

    return cap;
}

void QWidgetPrivate::setWindowTitle_helper(const QString &title)
{
    Q_Q(QWidget);
    if (!q->testAttribute(Qt::WA_WState_Created))
        createWinId();
    setWindowTitle_sys(qt_setWindowTitle_helperHelper(title, q));
}

void QWidgetPrivate::setWindowIconText_helper(const QString &title)
{
    Q_Q(QWidget);
    if (q->testAttribute(Qt::WA_WState_Created))
        setWindowIconText_sys(qt_setWindowTitle_helperHelper(title, q));
}

void QWidget::setWindowIconText(const QString &iconText)
{
    if (QWidget::windowIconText() == iconText)
        return;

    Q_D(QWidget);
    d->topData()->iconText = iconText;
    d->setWindowIconText_helper(iconText);

    QEvent e(QEvent::IconTextChange);
    QApplication::sendEvent(this, &e);
}

void QWidget::setWindowTitle(const QString &title)
{
    if (QWidget::windowTitle() == title)
        return;

    Q_D(QWidget);
    d->topData()->caption = title;
    d->setWindowTitle_helper(title);

    QEvent e(QEvent::WindowTitleChange);
    QApplication::sendEvent(this, &e);
}


/*!
    \property QWidget::windowIcon
    \brief the widget's icon

    This property only makes sense for windows. If no icon
    has been set, windowIcon() returns the application icon
    (QApplication::windowIcon()).

    \sa windowIconText, windowTitle
*/
QIcon QWidget::windowIcon() const
{
    const QWidget *w = this;
    while (w) {
        const QWidgetPrivate *d = w->d_func();
        if (d->extra && d->extra->topextra && d->extra->topextra->icon)
            return *d->extra->topextra->icon;
        w = w->parentWidget();
    }
    return qApp->windowIcon();
}

void QWidgetPrivate::setWindowIcon_helper()
{
    QEvent e(QEvent::WindowIconChange);
    QApplication::sendEvent(q_func(), &e);
    for (int i = 0; i < children.size(); ++i) {
        QWidget *w = qobject_cast<QWidget *>(children.at(i));
        if (w && !w->isWindow())
            QApplication::sendEvent(w, &e);
    }
}

void QWidget::setWindowIcon(const QIcon &icon)
{
    Q_D(QWidget);

    setAttribute(Qt::WA_SetWindowIcon, !icon.isNull());
    d->createTLExtra();

    if (!d->extra->topextra->icon)
        d->extra->topextra->icon = new QIcon();
    *d->extra->topextra->icon = icon;

    delete d->extra->topextra->iconPixmap;
    d->extra->topextra->iconPixmap = 0;

    d->setWindowIcon_sys();
    d->setWindowIcon_helper();
}


/*!
    \property QWidget::windowIconText
    \brief the widget's icon text

    This property only makes sense for windows. If no icon
    text has been set, this functions returns an empty string.

    \sa windowIcon, windowTitle
*/

QString QWidget::windowIconText() const
{
    Q_D(const QWidget);
    return (d->extra && d->extra->topextra) ? d->extra->topextra->iconText : QString();
}

/*!
    \property QWidget::windowFilePath
    \brief the file path associated with a widget.

    This property only makes sense for windows. It associates a file path with
    a window. If you have set file path, but do not set the window title, Qt
    set's the window title to:

    QFileInfo::fileName()[*], on Mac OS X, as per the Apple Human Interface Guidelines.
    QFileInfo::fileName()[*] \unicode 0x2014 QApplication::applicationName(), on Windows and X11 when QApplication::applicationName() is set.

    If the window title is set at any point, then the
    window title takes precedence.

    Additionally, on Mac OS X, this has an added benefit that it sets the \l {http://developer.apple.com/documentation/UserExperience/Conceptual/OSXHIGuidelines/XHIGWindows/chapter_17_section_3.html}{proxy icon} for the
    window, assuming that the file path exists.

    If no file path is set, this function returns an empty string.

    \sa windowTitle, windowIcon
*/

QString QWidget::windowFilePath() const
{
    Q_D(const QWidget);
    return (d->extra && d->extra->topextra) ? d->extra->topextra->filePath : QString();
}

void QWidget::setWindowFilePath(const QString &filePath)
{
    if (filePath == windowFilePath())
        return;

    Q_D(QWidget);

    QString finalPath = filePath;

    d->createTLExtra();
    d->extra->topextra->filePath = finalPath;
    d->setWindowFilePath_helper(finalPath);
}

void QWidgetPrivate::setWindowFilePath_helper(const QString &filePath)
{
    if (extra->topextra && extra->topextra->caption.isEmpty()) {
        setWindowTitle_helper(filePath);
    }
#ifdef Q_WS_MAC
    setWindowFilePath_sys(filePath);
#endif
}

/*!
    Returns the window's role, or an empty string.

    \sa windowIcon, windowTitle
*/

QString QWidget::windowRole() const
{
    Q_D(const QWidget);
    return (d->extra && d->extra->topextra) ? d->extra->topextra->role : QString();
}

/*!
    Sets the window's role to \a role. This only makes sense for
    windows on X11.
*/
void QWidget::setWindowRole(const QString &role)
{
#if defined(Q_WS_X11)
    Q_D(QWidget);
    d->topData()->role = role;
    d->setWindowRole();
#else
    Q_UNUSED(role)
#endif
}

/*!
    \property QWidget::mouseTracking
    \brief whether mouse tracking is enabled for the widget

    If mouse tracking is disabled (the default), the widget only
    receives mouse move events when at least one mouse button is
    pressed while the mouse is being moved.

    If mouse tracking is enabled, the widget receives mouse move
    events even if no buttons are pressed.

    \sa mouseMoveEvent()
*/


/*!
    Sets the widget's focus proxy to widget \a w. If \a w is 0, the
    function resets this widget to have no focus proxy.

    Some widgets can "have focus", but create a child widget, such as
    QLineEdit, to actually handle the focus. In this case, the widget
    can set the line edit to be its focus proxy.

    setFocusProxy() sets the widget which will actually get focus when
    "this widget" gets it. If there is a focus proxy, setFocus() and
    hasFocus() operate on the focus proxy.

    \sa focusProxy()
*/

void QWidget::setFocusProxy(QWidget * w)
{
    Q_D(QWidget);
    if (!w && !d->extra)
        return;

    for (QWidget* fp  = w; fp; fp = fp->focusProxy()) {
        if (fp == this) {
            qWarning("QWidget: %s (%s) already in focus proxy chain", metaObject()->className(), objectName().toLocal8Bit().constData());
            return;
        }
    }

    d->createExtra();
    d->extra->focus_proxy = w;
}


/*!
    Returns the focus proxy, or 0 if there is no focus proxy.

    \sa setFocusProxy()
*/

QWidget * QWidget::focusProxy() const
{
    Q_D(const QWidget);
    return d->extra ? (QWidget *)d->extra->focus_proxy : 0;
}


/*!
    \property QWidget::focus
    \brief whether this widget (or its focus proxy) has the keyboard
    input focus

    Effectively equivalent to \c {QApplication::focusWidget() == this}.

    \sa setFocus(), clearFocus(), setFocusPolicy(), QApplication::focusWidget()
*/
bool QWidget::hasFocus() const
{
    const QWidget* w = this;
    while (w->d_func()->extra && w->d_func()->extra->focus_proxy)
        w = w->d_func()->extra->focus_proxy;
    if (QWidget *window = w->window()) {
#ifndef QT_NO_GRAPHICSVIEW
        QTLWExtra *e = window->d_func()->maybeTopData();
        if (e && e->proxyWidget && e->proxyWidget->hasFocus() && window->focusWidget() == w)
            return true;
#endif
    }
    return (QApplication::focusWidget() == w);
}

/*!
    Gives the keyboard input focus to this widget (or its focus
    proxy) if this widget or one of its parents is the \link
    isActiveWindow() active window\endlink. The \a reason argument will
    be passed into any focus event sent from this function, it is used
    to give an explanation of what caused the widget to get focus.

    First, a focus out event is sent to the focus widget (if any) to
    tell it that it is about to lose the focus. Then a focus in event
    is sent to this widget to tell it that it just received the focus.
    (Nothing happens if the focus in and focus out widgets are the
    same.)

    setFocus() gives focus to a widget regardless of its focus policy,
    but does not clear any keyboard grab (see grabKeyboard()).

    Be aware that if the widget is hidden, it will not accept focus
    until it is shown.

    \warning If you call setFocus() in a function which may itself be
    called from focusOutEvent() or focusInEvent(), you may get an
    infinite recursion.

    \sa hasFocus() clearFocus() focusInEvent() focusOutEvent()
    setFocusPolicy() QApplication::focusWidget() grabKeyboard()
    grabMouse(), {Keyboard Focus}
*/

void QWidget::setFocus(Qt::FocusReason reason)
{
    if (!isEnabled())
        return;

    QWidget *f = this;
    while (f->d_func()->extra && f->d_func()->extra->focus_proxy)
        f = f->d_func()->extra->focus_proxy;

    if (QApplication::focusWidget() == f
#if defined(Q_WS_WIN)
        && GetFocus() == f->internalWinId()
#endif
       )
        return;

    QWidget *w = f;
    if (isHidden()) {
        while (w && w->isHidden()) {
            w->d_func()->focus_child = f;
            w = w->isWindow() ? 0 : w->parentWidget();
        }
    } else {
        while (w) {
            w->d_func()->focus_child = f;
            w = w->isWindow() ? 0 : w->parentWidget();
        }
    }

#ifndef QT_NO_GRAPHICSVIEW
    // Update proxy state
    if (QTLWExtra *topData = window()->d_func()->maybeTopData()) {
        if (topData->proxyWidget) {
            if (!topData->proxyWidget->hasFocus()) {
                topData->proxyWidget->setFocus(reason);
            } else if (!hasFocus()) {
                // Send event to self
                QFocusEvent event(QEvent::FocusIn, reason);
                QApplication::sendEvent(this, &event);
            }
        }
    }
#endif

    if (f->isActiveWindow()) {
        QApplicationPrivate::setFocusWidget(f, reason);
#ifndef QT_NO_ACCESSIBILITY
# ifdef Q_OS_WIN
        // The negation of the condition in setFocus_sys
        if (!(testAttribute(Qt::WA_WState_Created) && window()->windowType() != Qt::Popup && internalWinId()))
            //setFocusWidget will already post a focus event for us (that the AT client receives) on Windows
# endif
            QAccessible::updateAccessibility(f, 0, QAccessible::Focus);
#endif
    }
}

/*!
    \fn void QWidget::setFocus()
    \overload

    Gives the keyboard input focus to this widget (or its focus
    proxy) if this widget or one of its parents is the
    \l{isActiveWindow()}{active window}.
*/

/*!
    Takes keyboard input focus from the widget.

    If the widget has active focus, a \link focusOutEvent() focus out
    event\endlink is sent to this widget to tell it that it is about
    to lose the focus.

    This widget must enable focus setting in order to get the keyboard
    input focus, i.e. it must call setFocusPolicy().

    \sa hasFocus(), setFocus(), focusInEvent(), focusOutEvent(),
    setFocusPolicy(), QApplication::focusWidget()
*/

void QWidget::clearFocus()
{
    QWidget *w = this;
    while (w && w->d_func()->focus_child == this) {
        w->d_func()->focus_child = 0;
        w = w->parentWidget();
    }
    QTLWExtra *topData = d_func()->maybeTopData();
#ifndef QT_NO_GRAPHICSVIEW
    if (topData && topData->proxyWidget)
        topData->proxyWidget->clearFocus();
#endif

    if (hasFocus()) {
        // Update proxy state
        QApplicationPrivate::setFocusWidget(0, Qt::OtherFocusReason);
#if defined(Q_WS_WIN)
        if (!(windowType() == Qt::Popup) && GetFocus() == internalWinId())
            SetFocus(0);
        else
#endif
        {
#ifndef QT_NO_ACCESSIBILITY
            QAccessible::updateAccessibility(this, 0, QAccessible::Focus);
#endif
        }
    }
}


/*!
    \fn bool QWidget::focusNextChild()

    Finds a new widget to give the keyboard focus to, as appropriate
    for \key Tab, and returns true if it can find a new widget, or
    false if it can't.

    \sa focusPreviousChild()
*/

/*!
    \fn bool QWidget::focusPreviousChild()

    Finds a new widget to give the keyboard focus to, as appropriate
    for \key Shift+Tab, and returns true if it can find a new widget,
    or false if it can't.

    \sa focusNextChild()
*/

/*!
    Finds a new widget to give the keyboard focus to, as appropriate
    for Tab and Shift+Tab, and returns true if it can find a new
    widget, or false if it can't.

    If \a next is true, this function searches forward, if \a next
    is false, it searches backward.

    Sometimes, you will want to reimplement this function. For
    example, a web browser might reimplement it to move its "current
    active link" forward or backward, and call
    focusNextPrevChild() only when it reaches the last or
    first link on the "page".

    Child widgets call focusNextPrevChild() on their parent widgets,
    but only the window that contains the child widgets decides where
    to redirect focus. By reimplementing this function for an object,
    you thus gain control of focus traversal for all child widgets.

    \sa focusNextChild(), focusPreviousChild()
*/

bool QWidget::focusNextPrevChild(bool next)
{
    QWidget* p = parentWidget();
    bool isSubWindow = (windowType() == Qt::SubWindow);
    if (!isWindow() && !isSubWindow && p)
        return p->focusNextPrevChild(next);

    QWidget *w = QApplicationPrivate::focusNextPrevChild_helper(this, next);
    if (!w) return false;

    w->setFocus(next ? Qt::TabFocusReason : Qt::BacktabFocusReason);
    return true;
}

/*!
    Returns the last child of this widget that setFocus had been
    called on.  For top level widgets this is the widget that will get
    focus in case this window gets activated

    This is not the same as QApplication::focusWidget(), which returns
    the focus widget in the currently active window.
*/

QWidget *QWidget::focusWidget() const
{
    return const_cast<QWidget *>(d_func()->focus_child);
}

/*!
    Returns the next widget in this widget's focus chain.
*/
QWidget *QWidget::nextInFocusChain() const
{
    return const_cast<QWidget *>(d_func()->focus_next);
}

/*!
    \property QWidget::isActiveWindow
    \brief whether this widget's window is the active window

    The active window is the window that contains the widget that has
    keyboard focus (The window may still have focus if it has no
    widgets or none of its widgets accepts keyboard focus).

    When popup windows are visible, this property is true for both the
    active window \e and for the popup.

    \sa activateWindow(), QApplication::activeWindow()
*/
bool QWidget::isActiveWindow() const
{
    QWidget *tlw = window();
    if(tlw == qApp->activeWindow() || (isVisible() && (tlw->windowType() == Qt::Popup)))
        return true;

#ifndef QT_NO_GRAPHICSVIEW
    if (QTLWExtra *tlwExtra = tlw->d_func()->maybeTopData()) {
        if (isVisible() && tlwExtra->proxyWidget) {
            if (tlwExtra->proxyWidget->isWindow())
                return tlwExtra->proxyWidget->isActiveWindow();
            return tlwExtra->proxyWidget->hasFocus();
        }
    }
#endif

#ifdef Q_WS_MAC
    { //check process
        Boolean compare;
        ProcessSerialNumber current, front;
        GetCurrentProcess(&current);
        GetFrontProcess(&front);
        if(SameProcess(&current, &front, &compare) == noErr && !compare)
            return false;
    }
    extern bool qt_mac_is_macdrawer(const QWidget *); //qwidget_mac.cpp
    if(qt_mac_is_macdrawer(tlw) &&
       tlw->parentWidget() && tlw->parentWidget()->isActiveWindow())
        return true;
#endif
    if(style()->styleHint(QStyle::SH_Widget_ShareActivation, 0, this)) {
        if(((tlw->windowType() == Qt::Dialog) || (tlw->windowType() == Qt::Tool)) &&
           !tlw->isModal() &&
           (!tlw->parentWidget() || tlw->parentWidget()->isActiveWindow()))
           return true;
        QWidget *w = qApp->activeWindow();
        while(w && ((tlw->windowType() == Qt::Dialog) || (tlw->windowType() == Qt::Tool)) &&
              !w->isModal() && w->parentWidget()) {
            w = w->parentWidget()->window();
            if(w == tlw)
                return true;
        }
    }
#if defined(Q_WS_WIN32)
    HWND active = GetActiveWindow();
    if (!tlw->testAttribute(Qt::WA_WState_Created))
        return false;
    return active == tlw->internalWinId() || ::IsChild(active, tlw->internalWinId());
#else
    return false;
#endif
}

/*!
    Moves the \a second widget around the ring of focus widgets so
    that keyboard focus moves from the \a first widget to the \a
    second widget when the Tab key is pressed.

    Note that since the tab order of the \a second widget is changed,
    you should order a chain like this:

    \snippet doc/src/snippets/code/src.gui.kernel.qwidget.cpp 9

    \e not like this:

    \snippet doc/src/snippets/code/src.gui.kernel.qwidget.cpp 10

    If \a first or \a second has a focus proxy, setTabOrder()
    correctly substitutes the proxy.

    \sa setFocusPolicy(), setFocusProxy(), {Keyboard Focus}
*/
void QWidget::setTabOrder(QWidget* first, QWidget *second)
{
    if (!first || !second || first->focusPolicy() == Qt::NoFocus || second->focusPolicy() == Qt::NoFocus)
        return;

    if (first->window() != second->window()) {
        qWarning("QWidget::setTabOrder: 'first' and 'second' must be in the same window");
        return;
    }

    QWidget *fp = first->focusProxy();
    if (fp) {
        // If first is redirected, set first to the last child of first
        // that can take keyboard focus so that second is inserted after
        // that last child, and the focus order within first is (more
        // likely to be) preserved.
        QList<QWidget *> l = qFindChildren<QWidget *>(first);
        for (int i = l.size()-1; i >= 0; --i) {
            QWidget * next = l.at(i);
            if (next->window() == fp->window()) {
                fp = next;
                if (fp->focusPolicy() != Qt::NoFocus)
                    break;
            }
        }
        first = fp;
    }


    if (QWidget *sp = second->focusProxy())
        second = sp;

//    QWidget *fp = first->d_func()->focus_prev;
    QWidget *fn = first->d_func()->focus_next;

    if (fn == second)
        return;

    QWidget *sp = second->d_func()->focus_prev;
    QWidget *sn = second->d_func()->focus_next;

    fn->d_func()->focus_prev = second;
    first->d_func()->focus_next = second;

    second->d_func()->focus_next = fn;
    second->d_func()->focus_prev = first;

    sp->d_func()->focus_next = sn;
    sn->d_func()->focus_prev = sp;


    Q_ASSERT(first->d_func()->focus_next->d_func()->focus_prev == first);
    Q_ASSERT(first->d_func()->focus_prev->d_func()->focus_next == first);

    Q_ASSERT(second->d_func()->focus_next->d_func()->focus_prev == second);
    Q_ASSERT(second->d_func()->focus_prev->d_func()->focus_next == second);
}

/*!\internal

  Moves the relevant subwidgets of this widget from the \a oldtlw's
  tab chain to that of the new parent, if there's anything to move and
  we're really moving

  This function is called from QWidget::reparent() *after* the widget
  has been reparented.

  \sa reparent()
*/

void QWidgetPrivate::reparentFocusWidgets(QWidget * oldtlw)
{
    Q_Q(QWidget);
    if (oldtlw == q->window())
        return; // nothing to do

    if(focus_child)
        focus_child->clearFocus();

    // separate the focus chain into new (children of myself) and old (the rest)
    QWidget *firstOld = 0;
    //QWidget *firstNew = q; //invariant
    QWidget *o = 0; // last in the old list
    QWidget *n = q; // last in the new list

    bool prevWasNew = true;
    QWidget *w = focus_next;

    //Note: for efficiency, we do not maintain the list invariant inside the loop
    //we append items to the relevant list, and we optimize by not changing pointers
    //when subsequent items are going into the same list.
    while (w  != q) {
        bool currentIsNew =  q->isAncestorOf(w);
        if (currentIsNew) {
            if (!prevWasNew) {
                //prev was old -- append to new list
                n->d_func()->focus_next = w;
                w->d_func()->focus_prev = n;
            }
            n = w;
        } else {
            if (prevWasNew) {
                //prev was new -- append to old list, if there is one
                if (o) {
                    o->d_func()->focus_next = w;
                    w->d_func()->focus_prev = o;
                } else {
                    // "create" the old list
                    firstOld = w;
                }
            }
            o = w;
        }
        w = w->d_func()->focus_next;
        prevWasNew = currentIsNew;
    }

    //repair the old list:
    if (firstOld) {
        o->d_func()->focus_next = firstOld;
        firstOld->d_func()->focus_prev = o;
    }

    if (!q->isWindow()) {
        QWidget *topLevel = q->window();
        //insert new chain into toplevel's chain

        QWidget *prev = topLevel->d_func()->focus_prev;

        topLevel->d_func()->focus_prev = n;
        prev->d_func()->focus_next = q;

        focus_prev = prev;
        n->d_func()->focus_next = topLevel;
    } else {
        //repair the new list
            n->d_func()->focus_next = q;
            focus_prev = n;
    }

}

/*!\internal

  Measures the shortest distance from a point to a rect.

  This function is called from QDesktopwidget::screen(QPoint) to find the
  closest screen for a point.
*/
int QWidgetPrivate::pointToRect(const QPoint &p, const QRect &r)
{
    int dx = 0;
    int dy = 0;
    if (p.x() < r.left())
        dx = r.left() - p.x();
    else if (p.x() > r.right())
        dx = p.x() - r.right();
    if (p.y() < r.top())
        dy = r.top() - p.y();
    else if (p.y() > r.bottom())
        dy = p.y() - r.bottom();
    return dx + dy;
}

QRect QWidgetPrivate::fromOrToLayoutItemRect(const QRect &rect, int sign) const
{
    QRect r = rect;
    r.adjust(-sign * leftLayoutItemMargin, -sign * topLayoutItemMargin,
             +sign * rightLayoutItemMargin, +sign * bottomLayoutItemMargin);
    return r;
}

/*!
    \property QWidget::frameSize
    \brief the size of the widget including any window frame
*/
QSize QWidget::frameSize() const
{
    Q_D(const QWidget);
    if (isWindow() && !(windowType() == Qt::Popup)) {
        QRect fs = d->frameStrut();
        return QSize(data->crect.width() + fs.left() + fs.right(),
                      data->crect.height() + fs.top() + fs.bottom());
    }
    return data->crect.size();
}

/*! \fn void QWidget::move(int x, int y)

    \overload

    This corresponds to move(QPoint(\a x, \a y)).
*/

void QWidget::move(const QPoint &p)
{
    Q_D(QWidget);
    setAttribute(Qt::WA_Moved);
    if (isWindow())
        d->topData()->posFromMove = true;
    if (testAttribute(Qt::WA_WState_Created)) {
        d->setGeometry_sys(p.x() + geometry().x() - QWidget::x(),
                       p.y() + geometry().y() - QWidget::y(),
                       width(), height(), true);
        d->setDirtyOpaqueRegion();
    } else {
        data->crect.moveTopLeft(p); // no frame yet
        setAttribute(Qt::WA_PendingMoveEvent);
    }
}

/*! \fn void QWidget::resize(int w, int h)
    \overload

    This corresponds to resize(QSize(\a w, \a h)).
*/

void QWidget::resize(const QSize &s)
{
    Q_D(QWidget);
    setAttribute(Qt::WA_Resized);
    if (testAttribute(Qt::WA_WState_Created)) {
        d->setGeometry_sys(geometry().x(), geometry().y(), s.width(), s.height(), false);
        d->setDirtyOpaqueRegion();
    } else {
        data->crect.setSize(s);
        setAttribute(Qt::WA_PendingResizeEvent);
    }
}

void QWidget::setGeometry(const QRect &r)
{
    Q_D(QWidget);
    setAttribute(Qt::WA_Resized);
    setAttribute(Qt::WA_Moved);
    if (isWindow())
        d->topData()->posFromMove = false;
    if (testAttribute(Qt::WA_WState_Created)) {
        d->setGeometry_sys(r.x(), r.y(), r.width(), r.height(), true);
        d->setDirtyOpaqueRegion();
    } else {
        data->crect = r;
        setAttribute(Qt::WA_PendingMoveEvent);
        setAttribute(Qt::WA_PendingResizeEvent);
    }
}

/*!
    \since 4.2
    Saves the current geometry and state for top-level widgets.

    To save the geometry when the window closes, you can
    implement a close event like this:

    \snippet doc/src/snippets/code/src.gui.kernel.qwidget.cpp 11

    See the \link geometry.html Window Geometry documentation\endlink
    for an overview of geometry issues with windows.

    \sa restoreGeometry()
*/
QByteArray QWidget::saveGeometry() const
{
    QByteArray array;
    QDataStream stream(&array, QIODevice::WriteOnly);
    stream.setVersion(QDataStream::Qt_4_0);
    const quint32 magicNumber = 0x1D9D0CB;
    quint16 majorVersion = 1;
    quint16 minorVersion = 0;
    stream << magicNumber
           << majorVersion
           << minorVersion
           << frameGeometry()
           << normalGeometry()
           << qint32(QApplication::desktop()->screenNumber(this))
           << quint8(windowState() & Qt::WindowMaximized)
           << quint8(windowState() & Qt::WindowFullScreen);
    return array;
}

/*!
    \since 4.2

    Restores the geometry and state top-level widgets stored in the
    byte array \a geometry. Returns true on success; otherwise
    returns false.

    If the restored geometry is off-screen, it will be modified to be
    inside the the available screen geometry.

    To restore geometry saved using QSettings, you can use code like
    this:

    \snippet doc/src/snippets/code/src.gui.kernel.qwidget.cpp 12

    See the \link geometry.html Window Geometry documentation\endlink
    for an overview of geometry issues with windows.

    \sa saveGeometry(), QSettings
*/
bool QWidget::restoreGeometry(const QByteArray &geometry)
{
    if (geometry.size() < 4)
        return false;
    QDataStream stream(geometry);
    stream.setVersion(QDataStream::Qt_4_0);

    const quint32 magicNumber = 0x1D9D0CB;
    quint32 storedMagicNumber;
    stream >> storedMagicNumber;
    if (storedMagicNumber != magicNumber)
        return false;

    const quint16 currentMajorVersion = 1;
    quint16 majorVersion = 0;
    quint16 minorVersion = 0;

    stream >> majorVersion >> minorVersion;

    if (majorVersion != currentMajorVersion)
        return false;
    // (Allow all minor versions.)

    QRect restoredFrameGeometry;
     QRect restoredNormalGeometry;
    qint32 restoredScreenNumber;
    quint8 maximized;
    quint8 fullScreen;

    stream >> restoredFrameGeometry
           >> restoredNormalGeometry
           >> restoredScreenNumber
           >> maximized
           >> fullScreen;

    const int frameHeight = 20;
    if (!restoredFrameGeometry.isValid())
        restoredFrameGeometry = QRect(QPoint(0,0), sizeHint());

    if (!restoredNormalGeometry.isValid())
        restoredNormalGeometry = QRect(QPoint(0, frameHeight), sizeHint());
    if (!restoredNormalGeometry.isValid()) {
        // use the widget's adjustedSize if the sizeHint() doesn't help
        restoredNormalGeometry.setSize(restoredNormalGeometry
                                       .size()
                                       .expandedTo(d_func()->adjustedSize()));
    }

    const QDesktopWidget * const desktop = QApplication::desktop();
    if (restoredScreenNumber >= desktop->numScreens())
        restoredScreenNumber = desktop->primaryScreen();

    const QRect availableGeometry = desktop->availableGeometry(restoredScreenNumber);

    // Modify the restored geometry if we are about to restore to coordinates
    // that would make the window "lost". This happens if:
    // - The restored geometry is completely oustside the available geometry
    // - The title bar is outside the available geometry.
    // - (Mac only) The window is higher than the available geometry. It must
    //   be possible to bring the size grip on screen by moving the window.
#ifdef Q_WS_MAC
    restoredFrameGeometry.setHeight(qMin(restoredFrameGeometry.height(), availableGeometry.height()));
    restoredNormalGeometry.setHeight(qMin(restoredNormalGeometry.height(), availableGeometry.height() - frameHeight));
#endif

    if (!restoredFrameGeometry.intersects(availableGeometry)) {
        restoredFrameGeometry.moveBottom(qMin(restoredFrameGeometry.bottom(), availableGeometry.bottom()));
        restoredFrameGeometry.moveLeft(qMax(restoredFrameGeometry.left(), availableGeometry.left()));
        restoredFrameGeometry.moveRight(qMin(restoredFrameGeometry.right(), availableGeometry.right()));
    }
    restoredFrameGeometry.moveTop(qMax(restoredFrameGeometry.top(), availableGeometry.top()));

    if (!restoredNormalGeometry.intersects(availableGeometry)) {
        restoredNormalGeometry.moveBottom(qMin(restoredNormalGeometry.bottom(), availableGeometry.bottom()));
        restoredNormalGeometry.moveLeft(qMax(restoredNormalGeometry.left(), availableGeometry.left()));
        restoredNormalGeometry.moveRight(qMin(restoredNormalGeometry.right(), availableGeometry.right()));
    }
    restoredNormalGeometry.moveTop(qMax(restoredNormalGeometry.top(), availableGeometry.top() + frameHeight));

    if (maximized || fullScreen) {
        setGeometry(restoredNormalGeometry);
        Qt::WindowStates ws = windowState();
        if (maximized)
            ws |= Qt::WindowMaximized;
        if (fullScreen)
            ws |= Qt::WindowFullScreen;
            setWindowState(ws);
    } else {
        QPoint offset;
#ifdef Q_WS_X11
        if (isFullScreen())
            offset = d_func()->topData()->fullScreenOffset;
#endif
        setWindowState(windowState() & ~(Qt::WindowMaximized | Qt::WindowFullScreen));
        move(restoredFrameGeometry.topLeft() + offset);
        resize(restoredNormalGeometry.size());
    }
    return true;
}

/*!\fn void QWidget::setGeometry(int x, int y, int w, int h)
    \overload

    This corresponds to setGeometry(QRect(\a x, \a y, \a w, \a h)).
*/

/*!
    Sets the margins around the contents of the widget to have the
    sizes \a left, \a top, \a right, and \a bottom. The margins are
    used by the layout system, and may be used by subclasses to
    specify the area to draw in (e.g. excluding the frame).

    Changing the margins will trigger a resizeEvent().

    \sa contentsRect(), getContentsMargins()
*/
void QWidget::setContentsMargins(int left, int top, int right, int bottom)
{
    Q_D(QWidget);
    if (left == d->leftmargin && top == d->topmargin
         && right == d->rightmargin && bottom == d->bottommargin)
        return;
    d->leftmargin = left;
    d->topmargin = top;
    d->rightmargin = right;
    d->bottommargin = bottom;

    if (QLayout *l=d->layout)
        l->update(); //force activate; will do updateGeometry
    else
        updateGeometry();

    // ### Qt 5: compat, remove
    if (isVisible()) {
        update();
        QResizeEvent e(data->crect.size(), data->crect.size());
        QApplication::sendEvent(this, &e);
    } else {
        setAttribute(Qt::WA_PendingResizeEvent, true);
    }

    QEvent e(QEvent::ContentsRectChange);
    QApplication::sendEvent(this, &e);
}

/*!  Returns the widget's contents margins for \a left, \a top, \a
  right, and \a bottom.

  \sa setContentsMargins(), contentsRect()
 */
void QWidget::getContentsMargins(int *left, int *top, int *right, int *bottom) const
{
    Q_D(const QWidget);
    if (left)
        *left = d->leftmargin;
    if (top)
        *top = d->topmargin;
    if (right)
        *right = d->rightmargin;
    if (bottom)
        *bottom = d->bottommargin;
}

/*!
    Returns the area inside the widget's margins.

    \sa setContentsMargins(), getContentsMargins()
*/
QRect QWidget::contentsRect() const
{
    Q_D(const QWidget);
    return QRect(QPoint(d->leftmargin, d->topmargin),
                 QPoint(data->crect.width() - 1 - d->rightmargin,
                        data->crect.height() - 1 - d->bottommargin));

}



/*!
  \fn void QWidget::customContextMenuRequested(const QPoint &pos)

  This signal is emitted when the widget's \l contextMenuPolicy is
  Qt::CustomContextMenu, and the user has requested a context menu on
  the widget. The position \a pos is the position of the context menu
  event that the widget receives. Normally this is in widget
  coordinates. The exception to this rule is QAbstractScrollArea and
  its subclasses that map the context menu event to coordinates of the
  \link QAbstractScrollArea::viewport() viewport() \endlink .


  \sa mapToGlobal() QMenu contextMenuPolicy
*/


/*!
    \property QWidget::contextMenuPolicy
    \brief how the widget shows a context menu

    The default value of this property is Qt::DefaultContextMenu,
    which means the contextMenuEvent() handler is called. Other values
    are Qt::NoContextMenu, Qt::PreventContextMenu,
    Qt::ActionsContextMenu, and Qt::CustomContextMenu. With
    Qt::CustomContextMenu, the signal customContextMenuRequested() is
    emitted.

    \sa contextMenuEvent(), customContextMenuRequested(), actions()
*/

Qt::ContextMenuPolicy QWidget::contextMenuPolicy() const
{
    return (Qt::ContextMenuPolicy)data->context_menu_policy;
}

void QWidget::setContextMenuPolicy(Qt::ContextMenuPolicy policy)
{
    data->context_menu_policy = (uint) policy;
}

/*!
    \property QWidget::focusPolicy
    \brief the way the widget accepts keyboard focus

    The policy is Qt::TabFocus if the widget accepts keyboard
    focus by tabbing, Qt::ClickFocus if the widget accepts
    focus by clicking, Qt::StrongFocus if it accepts both, and
    Qt::NoFocus (the default) if it does not accept focus at
    all.

    You must enable keyboard focus for a widget if it processes
    keyboard events. This is normally done from the widget's
    constructor. For instance, the QLineEdit constructor calls
    setFocusPolicy(Qt::StrongFocus).

    \sa focusInEvent(), focusOutEvent(), keyPressEvent(), keyReleaseEvent(), enabled
*/


Qt::FocusPolicy QWidget::focusPolicy() const
{
    return (Qt::FocusPolicy)data->focus_policy;
}

void QWidget::setFocusPolicy(Qt::FocusPolicy policy)
{
    data->focus_policy = (uint) policy;
}

/*!
    \property QWidget::updatesEnabled
    \brief whether updates are enabled

    An updates enabled widget receives paint events and has a system
    background; a disabled widget does not. This also implies that
    calling update() and repaint() has no effect if updates are
    disabled.

    setUpdatesEnabled() is normally used to disable updates for a
    short period of time, for instance to avoid screen flicker during
    large changes. In Qt, widgets normally do not generate screen
    flicker, but on X11 the server might erase regions on the screen
    when widgets get hidden before they can be replaced by other
    widgets. Disabling updates solves this.

    Example:
    \snippet doc/src/snippets/code/src.gui.kernel.qwidget.cpp 13

    Disabling a widget implicitly disables all its children. Enabling
    respectively enables all child widgets unless they have been
    explicitly disabled. Re-enabling updates implicitly calls update()
    on the widget.

    \sa paintEvent()
*/
void QWidget::setUpdatesEnabled(bool enable)
{
    Q_D(QWidget);
    setAttribute(Qt::WA_ForceUpdatesDisabled, !enable);
    d->setUpdatesEnabled_helper(enable);
}

/*!  \fn void QWidget::show()

    Shows the widget and its child widgets. This function is
    equivalent to setVisible(true).

    \sa raise(), showEvent(), hide(), setVisible(), showMinimized(), showMaximized(),
    showNormal(), isVisible()
*/


/*! \internal

   Makes the widget visible in the isVisible() meaning of the word.
   It is only called for toplevels or widgets with visible parents.
 */
void QWidgetPrivate::show_recursive()
{
    Q_Q(QWidget);
    // polish if necessary

    if (!q->testAttribute(Qt::WA_WState_Created))
        createRecursively();
    q->ensurePolished();

#ifdef QT3_SUPPORT
    if(sendChildEvents)
        QApplication::sendPostedEvents(q, QEvent::ChildInserted);
#endif
    if (!q->isWindow() && q->parentWidget()->d_func()->layout && !q->parentWidget()->data->in_show)
        q->parentWidget()->d_func()->layout->activate();
    // activate our layout before we and our children become visible
    if (layout)
        layout->activate();

    show_helper();
}

void QWidgetPrivate::sendPendingMoveAndResizeEvents(bool recursive, bool disableUpdates)
{
    Q_Q(QWidget);

    disableUpdates = disableUpdates && q->updatesEnabled();
    if (disableUpdates)
        q->setAttribute(Qt::WA_UpdatesDisabled);

    if (q->testAttribute(Qt::WA_PendingMoveEvent)) {
        QMoveEvent e(data.crect.topLeft(), data.crect.topLeft());
        QApplication::sendEvent(q, &e);
        q->setAttribute(Qt::WA_PendingMoveEvent, false);
    }

    if (q->testAttribute(Qt::WA_PendingResizeEvent)) {
        QResizeEvent e(data.crect.size(), QSize());
        QApplication::sendEvent(q, &e);
        q->setAttribute(Qt::WA_PendingResizeEvent, false);
    }

    if (disableUpdates)
        q->setAttribute(Qt::WA_UpdatesDisabled, false);

    if (!recursive)
        return;

    for (int i = 0; i < children.size(); ++i) {
        if (QWidget *child = qobject_cast<QWidget *>(children.at(i)))
            child->d_func()->sendPendingMoveAndResizeEvents(recursive, disableUpdates);
    }
}

void QWidgetPrivate::activateChildLayoutsRecursively()
{
    sendPendingMoveAndResizeEvents(false, true);

    for (int i = 0; i < children.size(); ++i) {
        QWidget *child = qobject_cast<QWidget *>(children.at(i));
        if (!child || child->isHidden() || child->isWindow())
            continue;

        child->ensurePolished();

        // Activate child's layout
        QWidgetPrivate *childPrivate = child->d_func();
        if (childPrivate->layout)
            childPrivate->layout->activate();

        // Pretend we're visible.
        const bool wasVisible = child->isVisible();
        if (!wasVisible)
            child->setAttribute(Qt::WA_WState_Visible);

        // Do the same for all my children.
        childPrivate->activateChildLayoutsRecursively();

        // We're not cheating anymore.
        if (!wasVisible)
            child->setAttribute(Qt::WA_WState_Visible, false);
    }
}

void QWidgetPrivate::show_helper()
{
    Q_Q(QWidget);
    data.in_show = true; // qws optimization
    // make sure we receive pending move and resize events
    sendPendingMoveAndResizeEvents();

    // become visible before showing all children
    q->setAttribute(Qt::WA_WState_Visible);

    // finally show all children recursively
    showChildren(false);

#ifdef QT3_SUPPORT
    if (q->parentWidget() && sendChildEvents)
        QApplication::sendPostedEvents(q->parentWidget(),
                                        QEvent::ChildInserted);
#endif


    // popup handling: new popups and tools need to be raised, and
    // existing popups must be closed. Also propagate the current
    // windows's KeyboardFocusChange status.
    if (q->isWindow()) {
        if ((q->windowType() == Qt::Tool) || (q->windowType() == Qt::Popup) || q->windowType() == Qt::ToolTip) {
            q->raise();
            if (q->parentWidget() && q->parentWidget()->window()->testAttribute(Qt::WA_KeyboardFocusChange))
                q->setAttribute(Qt::WA_KeyboardFocusChange);
        } else {
            while (QApplication::activePopupWidget()) {
                if (!QApplication::activePopupWidget()->close())
                    break;
            }
        }
    }

    bool isEmbedded = false;
#ifndef QT_NO_GRAPHICSVIEW
    if (q->isWindow()) {
        isEmbedded = graphicsProxyWidget() ? true : false;
        if (!isEmbedded) {
            QGraphicsProxyWidget *ancestorProxy = nearestGraphicsProxyWidget(q->parentWidget());
            if (ancestorProxy) {
                isEmbedded = true;
                ancestorProxy->d_func()->embedSubWindow(q);
            }
        }
    }
#else
    Q_UNUSED(isEmbedded);
#endif

    // On Windows, show the popup now so that our own focus handling
    // stores the correct old focus widget even if it's stolen in the
    // showevent
#if defined(Q_WS_WIN)
    if (!isEmbedded && q->windowType() == Qt::Popup)
        qApp->d_func()->openPopup(q);
#endif

    // send the show event before showing the window
    QShowEvent showEvent;
    QApplication::sendEvent(q, &showEvent);

    if (!isEmbedded && q->isModal())
        // QApplicationPrivate::enterModal *before* show, otherwise the initial
        // stacking might be wrong
        QApplicationPrivate::enterModal(q);


    show_sys();

#if !defined(Q_WS_WIN)
    if (!isEmbedded && q->windowType() == Qt::Popup)
        qApp->d_func()->openPopup(q);
#endif

#ifndef QT_NO_ACCESSIBILITY
    if (q->windowType() != Qt::ToolTip)     // Tooltips are read aloud twice in MS narrator.
        QAccessible::updateAccessibility(q, 0, QAccessible::ObjectShow);
#endif

    if (QApplicationPrivate::hidden_focus_widget == q) {
        QApplicationPrivate::hidden_focus_widget = 0;
        q->setFocus(Qt::OtherFocusReason);
    }

    data.in_show = false;  // reset qws optimization
}

/*! \fn void QWidget::hide()

    Hides the widget. This function is equivalent to
    setVisible(false).


    \note If you are working with QDialog or its subclasses and you invoke
    the show() function after this function, the dialog will be displayed in
    its original position.

    \sa hideEvent(), isHidden(), show(), setVisible(), isVisible(), close()
*/

/*!\internal
 */
void QWidgetPrivate::hide_helper()
{
    Q_Q(QWidget);

    bool isEmbedded = false;
#if !defined QT_NO_GRAPHICSVIEW
    isEmbedded = q->isWindow() && nearestGraphicsProxyWidget(q->parentWidget()) != 0;
#else
    Q_UNUSED(isEmbedded);
#endif

    if (!isEmbedded && (q->windowType() == Qt::Popup))
        qApp->d_func()->closePopup(q);

    // Move test modal here.  Otherwise, a modal dialog could get
    // destroyed and we lose all access to its parent because we haven't
    // left modality.  (Eg. modal Progress Dialog)
    if (!isEmbedded && q->isModal())
        QApplicationPrivate::leaveModal(q);

#if defined(Q_WS_WIN)
    if (q->isWindow() && !(q->windowType() == Qt::Popup) && q->parentWidget()
        && !q->parentWidget()->isHidden() && q->isActiveWindow())
        q->parentWidget()->activateWindow();        // Activate parent
#endif

    q->setAttribute(Qt::WA_Mapped, false);
    hide_sys();

    bool wasVisible = q->testAttribute(Qt::WA_WState_Visible);

    if (wasVisible) {
        q->setAttribute(Qt::WA_WState_Visible, false);

    }

    QHideEvent hideEvent;
    QApplication::sendEvent(q, &hideEvent);
    hideChildren(false);

    // next bit tries to move the focus if the focus widget is now
    // hidden.
    if (wasVisible) {
        QWidget *fw = QApplication::focusWidget();
        while (fw &&  !fw->isWindow()) {
            if (fw == q) {
                q->focusNextPrevChild(true);
                break;
            }
            fw = fw->parentWidget();
        }
    }

#ifdef Q_WIDGET_USE_DIRTYLIST
    QWidgetBackingStore *bs = maybeBackingStore();
    if (bs)
        bs->removeDirtyWidget(q);
#endif

#ifndef QT_NO_ACCESSIBILITY
    if (wasVisible)
        QAccessible::updateAccessibility(q, 0, QAccessible::ObjectHide);
#endif
}

/*!
    \fn bool QWidget::isHidden() const

    Returns true if the widget is hidden, otherwise returns false.

    A hidden widget will only become visible when show() is called on
    it. It will not be automatically shown when the parent is shown.

    To check visiblity, use !isVisible() instead (notice the exclamation mark).

    isHidden() implies !isVisible(), but a widget can be not visible
    and not hidden at the same time. This is the case for widgets that are children of
    widgets that are not visible.


    Widgets are  hidden if they were created as independent
    windows or as children of visible widgets, or if hide() or setVisible(false) was called.

*/


void QWidget::setVisible(bool visible)
{
    if (visible) { // show
        if (testAttribute(Qt::WA_WState_ExplicitShowHide) && !testAttribute(Qt::WA_WState_Hidden))
            return;

        Q_D(QWidget);

        // Designer uses a trick to make grabWidget work without showing
        if (!isWindow() && parentWidget() && parentWidget()->isVisible()
            && !parentWidget()->testAttribute(Qt::WA_WState_Created))
            parentWidget()->window()->d_func()->createRecursively();

        //we have to at least create toplevels before applyX11SpecificCommandLineArguments
        //but not children of non-visible parents
        QWidget *pw = parentWidget();
        if (!testAttribute(Qt::WA_WState_Created)
            && (isWindow() || pw->testAttribute(Qt::WA_WState_Created))) {
            create();
        }

#if defined(Q_WS_X11)
        if (windowType() == Qt::Window)
            QApplicationPrivate::applyX11SpecificCommandLineArguments(this);
#elif defined(Q_WS_QWS)
        if (windowType() == Qt::Window)
            QApplicationPrivate::applyQWSSpecificCommandLineArguments(this);
#endif

        bool wasResized = testAttribute(Qt::WA_Resized);
        Qt::WindowStates initialWindowState = windowState();

        // polish if necessary
        ensurePolished();

        // remember that show was called explicitly
        setAttribute(Qt::WA_WState_ExplicitShowHide);
        // whether we need to inform the parent widget immediately
        bool needUpdateGeometry = !isWindow() && testAttribute(Qt::WA_WState_Hidden);
        // we are no longer hidden
        setAttribute(Qt::WA_WState_Hidden, false);

        if (needUpdateGeometry)
            d->updateGeometry_helper(true);

#ifdef QT3_SUPPORT
        QApplication::sendPostedEvents(this, QEvent::ChildInserted);
#endif
        // activate our layout before we and our children become visible
        if (d->layout)
            d->layout->activate();

        if (!isWindow()) {
            QWidget *parent = parentWidget();
            while (parent && parent->isVisible() && parent->d_func()->layout  && !parent->data->in_show) {
                parent->d_func()->layout->activate();
                if (parent->isWindow())
                    break;
                parent = parent->parentWidget();
            }
            if (parent && !d->getOpaqueRegion().isEmpty())
                parent->d_func()->setDirtyOpaqueRegion();
        }

        // adjust size if necessary
        if (!wasResized
            && (isWindow() || !parentWidget()->d_func()->layout))  {
            if (isWindow()) {
                adjustSize();
                if (windowState() != initialWindowState)
                    setWindowState(initialWindowState);
            } else {
                adjustSize();
            }
            setAttribute(Qt::WA_Resized, false);
        }

        setAttribute(Qt::WA_KeyboardFocusChange, false);

        if (isWindow() || parentWidget()->isVisible()) {
            // remove posted quit events when showing a new window
            QCoreApplication::removePostedEvents(qApp, QEvent::Quit);

            d->show_helper();
        }

        QEvent showToParentEvent(QEvent::ShowToParent);
        QApplication::sendEvent(this, &showToParentEvent);
    } else { // hide
        if (testAttribute(Qt::WA_WState_ExplicitShowHide) && testAttribute(Qt::WA_WState_Hidden))
            return;

        if (QApplicationPrivate::hidden_focus_widget == this)
            QApplicationPrivate::hidden_focus_widget = 0;

        Q_D(QWidget);

        // hw: The test on getOpaqueRegion() needs to be more intelligent
        // currently it doesn't work if the widget is hidden (the region will
        // be clipped). The real check should be testing the cached region
        // (and dirty flag) directly.
        if (!isWindow() && parentWidget()) // && !d->getOpaqueRegion().isEmpty())
            parentWidget()->d_func()->setDirtyOpaqueRegion();

        setAttribute(Qt::WA_WState_Hidden);
        setAttribute(Qt::WA_WState_ExplicitShowHide);
        if (testAttribute(Qt::WA_WState_Created))
            d->hide_helper();

        // invalidate layout similar to updateGeometry()
        if (!isWindow() && parentWidget()) {
            if (parentWidget()->d_func()->layout)
                parentWidget()->d_func()->layout->invalidate();
            else if (parentWidget()->isVisible())
                QApplication::postEvent(parentWidget(), new QEvent(QEvent::LayoutRequest));
        }

        QEvent hideToParentEvent(QEvent::HideToParent);
        QApplication::sendEvent(this, &hideToParentEvent);
    }
}

/*!\fn void QWidget::setHidden(bool hidden)

    Convenience function, equivalent to setVisible(!\a hidden).
*/

/*!\fn void QWidget::setShown(bool shown)

    Use setVisible(\a shown) instead.
*/


void QWidgetPrivate::_q_showIfNotHidden()
{
    Q_Q(QWidget);
    if ( !(q->isHidden() && q->testAttribute(Qt::WA_WState_ExplicitShowHide)) )
        q->setVisible(true);
}

void QWidgetPrivate::showChildren(bool spontaneous)
{
    QList<QObject*> childList = children;
    for (int i = 0; i < childList.size(); ++i) {
        QWidget *widget = qobject_cast<QWidget*>(childList.at(i));
        if (!widget
            || widget->isWindow()
            || widget->testAttribute(Qt::WA_WState_Hidden))
            continue;
        if (spontaneous) {
            widget->setAttribute(Qt::WA_Mapped);
            widget->d_func()->showChildren(true);
            QShowEvent e;
            QApplication::sendSpontaneousEvent(widget, &e);
        } else {
            if (widget->testAttribute(Qt::WA_WState_ExplicitShowHide))
                widget->d_func()->show_recursive();
            else
                widget->show();
        }
    }
}

void QWidgetPrivate::hideChildren(bool spontaneous)
{
    QList<QObject*> childList = children;
    for (int i = 0; i < childList.size(); ++i) {
        QWidget *widget = qobject_cast<QWidget*>(childList.at(i));
        if (!widget || widget->isWindow() || widget->testAttribute(Qt::WA_WState_Hidden))
            continue;
        if (spontaneous)
            widget->setAttribute(Qt::WA_Mapped, false);
        else
            widget->setAttribute(Qt::WA_WState_Visible, false);
        widget->d_func()->hideChildren(spontaneous);
        QHideEvent e;
        if (spontaneous)
            QApplication::sendSpontaneousEvent(widget, &e);
        else
            QApplication::sendEvent(widget, &e);
#ifndef QT_NO_ACCESSIBILITY
        if (!spontaneous)
            QAccessible::updateAccessibility(widget, 0, QAccessible::ObjectHide);
#endif
    }
}

bool QWidgetPrivate::close_helper(CloseMode mode)
{
    if (data.is_closing)
        return true;

    Q_Q(QWidget);
    data.is_closing = 1;

    QPointer<QWidget> that = q;
    QPointer<QWidget> parentWidget = q->parentWidget();

#ifdef QT3_SUPPORT
    bool isMain = (QApplicationPrivate::main_widget == q);
#endif
    bool quitOnClose = q->testAttribute(Qt::WA_QuitOnClose);
    if (mode != CloseNoEvent) {
        QCloseEvent e;
        if (mode == CloseWithSpontaneousEvent)
            QApplication::sendSpontaneousEvent(q, &e);
        else
            QApplication::sendEvent(q, &e);
        if (!that.isNull() && !e.isAccepted()) {
            data.is_closing = 0;
            return false;
        }
    }

    if (!that.isNull() && !q->isHidden())
        q->hide();

#ifdef QT3_SUPPORT
    if (isMain)
        qApp->quit();
#endif
    // Attempt to close the application only if this has WA_QuitOnClose set and a non-visible parent
    quitOnClose = quitOnClose && (parentWidget.isNull() || !parentWidget->isVisible());

    if (quitOnClose) {
        /* if there is no non-withdrawn primary window left (except
           the ones without QuitOnClose), we emit the lastWindowClosed
           signal */
        QWidgetList list = QApplication::topLevelWidgets();
        bool lastWindowClosed = true;
        for (int i = 0; i < list.size(); ++i) {
            QWidget *w = list.at(i);
            if (!w->isVisible() || w->parentWidget() || !w->testAttribute(Qt::WA_QuitOnClose))
                continue;
            lastWindowClosed = false;
            break;
        }
        if (lastWindowClosed)
            QApplicationPrivate::emitLastWindowClosed();
    }

    if (!that.isNull()) {
        data.is_closing = 0;
        if (q->testAttribute(Qt::WA_DeleteOnClose)) {
            q->setAttribute(Qt::WA_DeleteOnClose, false);
            q->deleteLater();
        }
    }
    return true;
}


/*!
    Closes this widget. Returns true if the widget was closed;
    otherwise returns false.

    First it sends the widget a QCloseEvent. The widget is \link
    hide() hidden\endlink if it \link QCloseEvent::accept()
    accepts\endlink the close event. If it \link QCloseEvent::ignore()
    ignores\endlink the event, nothing happens. The default
    implementation of QWidget::closeEvent() accepts the close event.

    If the widget has the Qt::WA_DeleteOnClose flag, the widget
    is also deleted. A close events is delivered to the widget no
    matter if the widget is visible or not.

    The \l QApplication::lastWindowClosed() signal is emitted when the
    last visible primary window (i.e. window with no parent) with the
    Qt::WA_QuitOnClose attribute set is closed. By default this
    attribute is set for all widgets except transient windows such as
    splash screens, tool windows, and popup menus.

*/

bool QWidget::close()
{
    return d_func()->close_helper(QWidgetPrivate::CloseWithEvent);
}

/*!
    \property QWidget::visible
    \brief whether the widget is visible

    Calling setVisible(true) or show() sets the widget to visible
    status if all its parent widgets up to the window are visible. If
    an ancestor is not visible, the widget won't become visible until
    all its ancestors are shown. If its size or position has changed,
    Qt guarantees that a widget gets move and resize events just
    before it is shown. If the widget has not been resized yet, Qt
    will adjust the widget's size to a useful default using
    adjustSize().

    Calling setVisible(false) or hide() hides a widget explicitly. An
    explicitly hidden widget will never become visible, even if all
    its ancestors become visible, unless you show it.

    A widget receives show and hide events when its visibility status
    changes. Between a hide and a show event, there is no need to
    waste CPU cycles preparing or displaying information to the user.
    A video application, for example, might simply stop generating new
    frames.

    A widget that happens to be obscured by other windows on the
    screen is considered to be visible. The same applies to iconified
    windows and windows that exist on another virtual
    desktop (on platforms that support this concept). A widget
    receives spontaneous show and hide events when its mapping status
    is changed by the window system, e.g. a spontaneous hide event
    when the user minimizes the window, and a spontaneous show event
    when the window is restored again.

    You almost never have to reimplement the setVisible() function. If
    you need to change some settings before a widget is shown, use
    showEvent() instead. If you need to do some delayed initialization
    use the Polish event delivered to the event() function.

    \sa show(), hide(), isHidden(), isVisibleTo(), isMinimized(),
    showEvent(), hideEvent()
*/


/*!
    Returns true if this widget would become visible if \a ancestor is
    shown; otherwise returns false.

    The true case occurs if neither the widget itself nor any parent
    up to but excluding \a ancestor has been explicitly hidden.

    This function will still return true if the widget is obscured by
    other windows on the screen, but could be physically visible if it
    or they were to be moved.

    isVisibleTo(0) is identical to isVisible().

    \sa show() hide() isVisible()
*/

bool QWidget::isVisibleTo(QWidget* ancestor) const
{
    if (!ancestor)
        return isVisible();
    const QWidget * w = this;
    while (w
            && !w->isHidden()
            && !w->isWindow()
            && w->parentWidget()
            && w->parentWidget() != ancestor)
        w = w->parentWidget();
    return !w->isHidden();
}

#ifdef QT3_SUPPORT
/*!
    Use visibleRegion() instead.
*/
QRect QWidget::visibleRect() const
{
    return d_func()->clipRect();
}
#endif

/*!
    Returns the unobscured region where paint events can occur.

    For visible widgets, this is an approximation of the area not
    covered by other widgets; otherwise, this is an empty region.

    The repaint() function calls this function if necessary, so in
    general you do not need to call it.

*/
QRegion QWidget::visibleRegion() const
{
    Q_D(const QWidget);

    QRect clipRect = d->clipRect();
    QRegion r(clipRect);
#ifdef QT_GREENPHONE_OPT
    d->subtractOpaqueChildren(r, clipRect, QPoint());
#else
    d->subtractOpaqueChildren(r, r, QPoint());
#endif
//    d->subtractOpaqueSiblings(r, r, QPoint()); // XXX: enable when caching is implemented
#ifdef Q_WS_QWS
    const QWSWindowSurface *surface = static_cast<const QWSWindowSurface*>(windowSurface());
    if (surface) {
        const QPoint offset = mapTo(surface->window(), QPoint());
        r &= surface->clipRegion().translated(-offset);
    }
#endif
    return r;
}


QSize QWidgetPrivate::adjustedSize() const
{
    Q_Q(const QWidget);

    QSize s = q->sizeHint();

    if (q->isWindow()) {
        Qt::Orientations exp;
        if (QLayout *l = q->layout()) {
            if (l->hasHeightForWidth())
                s.setHeight(l->totalHeightForWidth(s.width()));
            exp = l->expandingDirections();
        } else
        {
            if (q->sizePolicy().hasHeightForWidth())
                s.setHeight(q->heightForWidth(s.width()));
            exp = q->sizePolicy().expandingDirections();
        }
        if (exp & Qt::Horizontal)
            s.setWidth(qMax(s.width(), 200));
        if (exp & Qt::Vertical)
            s.setHeight(qMax(s.height(), 100));
#if defined(Q_WS_X11)
        QRect screen = QApplication::desktop()->screenGeometry(q->x11Info().screen());
#else // all others
        QRect screen = QApplication::desktop()->screenGeometry(q->pos());
#endif
#if defined (Q_OS_WINCE)
        s.setWidth(qMin(s.width(), screen.width()));
        s.setHeight(qMin(s.height(), screen.height()));
#else
        s.setWidth(qMin(s.width(), screen.width()*2/3));
        s.setHeight(qMin(s.height(), screen.height()*2/3));
#endif
        if (QTLWExtra *extra = maybeTopData())
            extra->sizeAdjusted = true;
    }

    if (!s.isValid()) {
        QRect r = q->childrenRect(); // get children rectangle
        if (r.isNull())
            return s;
        s = r.size() + QSize(2 * r.x(), 2 * r.y());
    }

    return s;
}

/*!
    Adjusts the size of the widget to fit the contents.

    Uses sizeHint() if valid (i.e if the size hint's width and height
    are \>= 0); otherwise sets the size to the children rectangle that
    covers all child widgets (the union of all child widget rectangles).

    For windows, the screen size is also taken into account, and if the
    sizeHint() is less than (200, 100) and the size policy is expanding,
    the window is made to be at least (200, 100).

    \sa sizeHint(), childrenRect()
*/

void QWidget::adjustSize()
{
    Q_D(QWidget);
    ensurePolished();
    QSize s = d->adjustedSize();
    if (s.isValid())
        resize(s);
}


/*!
    \property QWidget::sizeHint
    \brief the recommended size for the widget

    If the value of this property is an invalid size, no size is
    recommended.

    The default implementation of sizeHint() returns an invalid size
    if there is no layout for this widget, and returns the layout's
    preferred size otherwise.

    \sa QSize::isValid(), minimumSizeHint(), sizePolicy(),
    setMinimumSize(), updateGeometry()
*/

QSize QWidget::sizeHint() const
{
    Q_D(const QWidget);
    if (d->layout)
        return d->layout->totalSizeHint();
    return QSize(-1, -1);
}

/*!
    \property QWidget::minimumSizeHint
    \brief the recommended minimum size for the widget

    If the value of this property is an invalid size, no minimum size
    is recommended.

    The default implementation of minimumSizeHint() returns an invalid
    size if there is no layout for this widget, and returns the
    layout's minimum size otherwise. Most built-in widgets reimplement
    minimumSizeHint().

    \l QLayout will never resize a widget to a size smaller than the
    minimum size hint unless minimumSize() is set or the size policy is
    set to QSizePolicy::Ignore. If minimumSize() is set, the minimum
    size hint will be ignored.

    \sa QSize::isValid(), resize(), setMinimumSize(), sizePolicy()
*/
QSize QWidget::minimumSizeHint() const
{
    Q_D(const QWidget);
    if (d->layout)
        return d->layout->totalMinimumSize();
    return QSize(-1, -1);
}


/*!
    \fn QWidget *QWidget::parentWidget() const

    Returns the parent of this widget, or 0 if it does not have any
    parent widget.
*/


/*!
    Returns true if this widget is a parent, (or grandparent and so on
    to any level), of the given \a child, and both widgets are within
    the same window; otherwise returns false.
*/

bool QWidget::isAncestorOf(const QWidget *child) const
{
    while (child) {
        if (child == this)
            return true;
        if (child->isWindow())
            return false;
        child = child->parentWidget();
    }
    return false;
}



/*****************************************************************************
  QWidget event handling
 *****************************************************************************/

/*!
    This is the main event handler; it handles event \a event. You can
    reimplement this function in a subclass, but we recommend using
    one of the specialized event handlers instead.

    Key press and release events are treated differently from other
    events. event() checks for Tab and Shift+Tab and tries to move the
    focus appropriately. If there is no widget to move the focus to
    (or the key press is not Tab or Shift+Tab), event() calls
    keyPressEvent().

    Mouse and tablet event handling is also slightly special: only
    when the widget is \l enabled, event() will call the specialized
    handlers such as mousePressEvent(); otherwise it will discard the
    event.

    This function returns true if the event was recognized, otherwise
    it returns false.  If the recognized event was accepted (see \l
    QEvent::accepted), any further processing such as event
    propagation to the parent widget stops.

    \sa closeEvent(), focusInEvent(), focusOutEvent(), enterEvent(),
    keyPressEvent(), keyReleaseEvent(), leaveEvent(),
    mouseDoubleClickEvent(), mouseMoveEvent(), mousePressEvent(),
    mouseReleaseEvent(), moveEvent(), paintEvent(), resizeEvent(),
    QObject::event(), QObject::timerEvent()
*/

bool QWidget::event(QEvent *event)
{
    Q_D(QWidget);

    // ignore mouse events when disabled
    if (!isEnabled()) {
        switch(event->type()) {
        case QEvent::TabletPress:
        case QEvent::TabletRelease:
        case QEvent::TabletMove:
        case QEvent::MouseButtonPress:
        case QEvent::MouseButtonRelease:
        case QEvent::MouseButtonDblClick:
        case QEvent::MouseMove:
        case QEvent::ContextMenu:
#ifndef QT_NO_WHEELEVENT
        case QEvent::Wheel:
#endif
            return false;
        default:
            break;
        }
    }
    switch (event->type()) {
#ifdef Q_RATE_LIMIT_PAINTING
    case QEvent::Timer: {
        QTimerEvent *timerEvent = static_cast<QTimerEvent *>(event);
        if (timerEvent->timerId() == d->timerId) {
            killTimer(d->timerId);
            QWidgetBackingStore *bs = d->maybeBackingStore();
            if (bs)
                bs->updateDirtyRegion(this);
            d->timerId = -1;
        } else {
            this->timerEvent(timerEvent);
        }
        break;
    }
#endif
    case QEvent::MouseMove:
        mouseMoveEvent((QMouseEvent*)event);
        break;

    case QEvent::MouseButtonPress:
        // Don't reset input context here. Whether reset or not is
        // a responsibility of input method. reset() will be
        // called by mouseHandler() of input method if necessary
        // via mousePressEvent() of text widgets.
#if 0
        resetInputContext();
#endif
        mousePressEvent((QMouseEvent*)event);
        break;

    case QEvent::MouseButtonRelease:
        mouseReleaseEvent((QMouseEvent*)event);
        break;

    case QEvent::MouseButtonDblClick:
        mouseDoubleClickEvent((QMouseEvent*)event);
        break;
#ifndef QT_NO_WHEELEVENT
    case QEvent::Wheel:
        wheelEvent((QWheelEvent*)event);
        break;
#endif
#ifndef QT_NO_TABLETEVENT
    case QEvent::TabletMove:
    case QEvent::TabletPress:
    case QEvent::TabletRelease:
        tabletEvent((QTabletEvent*)event);
        break;
#endif
#ifdef QT3_SUPPORT
    case QEvent::Accel:
        event->ignore();
        return false;
#endif
    case QEvent::KeyPress: {
        QKeyEvent *k = (QKeyEvent *)event;
        bool res = false;
        if (!(k->modifiers() & (Qt::ControlModifier | Qt::AltModifier))) {  //### Add MetaModifier?
            if (k->key() == Qt::Key_Backtab
                || (k->key() == Qt::Key_Tab && (k->modifiers() & Qt::ShiftModifier)))
                res = focusNextPrevChild(false);
            else if (k->key() == Qt::Key_Tab)
                res = focusNextPrevChild(true);
            if (res)
                break;
        }
        keyPressEvent(k);
#ifdef QT_KEYPAD_NAVIGATION
        if (!k->isAccepted() && QApplication::keypadNavigationEnabled()
            && !(k->modifiers() & (Qt::ControlModifier | Qt::AltModifier | Qt::ShiftModifier))) {
            if (k->key() == Qt::Key_Up)
                res = focusNextPrevChild(false);
            else if (k->key() == Qt::Key_Down)
                res = focusNextPrevChild(true);
            if (res) {
                k->accept();
                break;
            }
        }
#endif
#ifndef QT_NO_WHATSTHIS
        if (!k->isAccepted()
            && k->modifiers() & Qt::ShiftModifier && k->key() == Qt::Key_F1
            && d->whatsThis.size()) {
            QWhatsThis::showText(mapToGlobal(inputMethodQuery(Qt::ImMicroFocus).toRect().center()), d->whatsThis, this);
            k->accept();
        }
#endif
    }
        break;

    case QEvent::KeyRelease:
        keyReleaseEvent((QKeyEvent*)event);
        // fall through
    case QEvent::ShortcutOverride:
        break;

    case QEvent::InputMethod:
        inputMethodEvent((QInputMethodEvent *) event);
        break;

    case QEvent::PolishRequest:
        ensurePolished();
        break;

    case QEvent::Polish: {
        style()->polish(this);
        setAttribute(Qt::WA_WState_Polished);
        if (!testAttribute(Qt::WA_SetFont) && !QApplication::font(this).isCopyOf(QApplication::font()))
            d->resolveFont();
        if (!QApplication::palette(this).isCopyOf(QApplication::palette()))
            d->resolvePalette();
#ifdef QT3_SUPPORT
        if(d->sendChildEvents)
            QApplication::sendPostedEvents(this, QEvent::ChildInserted);
#endif
    }
        break;

    case QEvent::ApplicationWindowIconChange:
        if (isWindow() && !testAttribute(Qt::WA_SetWindowIcon)) {
            d->setWindowIcon_sys();
            d->setWindowIcon_helper();
        }
        break;
    case QEvent::FocusIn:
#if defined(Q_WS_WIN)
        QWinInputContext::updateImeStatus(this, true);
#endif
        focusInEvent((QFocusEvent*)event);
        break;

    case QEvent::FocusOut:
#if defined(Q_WS_WIN)
        QWinInputContext::updateImeStatus(this, false);
#endif
        focusOutEvent((QFocusEvent*)event);
        break;

    case QEvent::Enter:
#ifndef QT_NO_STATUSTIP
        if (d->statusTip.size()) {
            QStatusTipEvent tip(d->statusTip);
            QApplication::sendEvent(const_cast<QWidget *>(this), &tip);
        }
#endif
        enterEvent(event);
        break;

    case QEvent::Leave:
#ifndef QT_NO_STATUSTIP
        if (d->statusTip.size()) {
            QString empty;
            QStatusTipEvent tip(empty);
            QApplication::sendEvent(const_cast<QWidget *>(this), &tip);
        }
#endif
        leaveEvent(event);
        break;

    case QEvent::HoverEnter:
    case QEvent::HoverLeave:
        update();
        break;

    case QEvent::Paint:
        // At this point the event has to be delivered, regardless
        // whether the widget isVisible() or not because it
        // already went through the filters
        paintEvent((QPaintEvent*)event);
        break;

    case QEvent::Move:
        moveEvent((QMoveEvent*)event);
        break;

    case QEvent::Resize:
        resizeEvent((QResizeEvent*)event);
        break;

    case QEvent::Close:
        closeEvent((QCloseEvent *)event);
        break;

    case QEvent::ContextMenu:
        switch (data->context_menu_policy) {
        case Qt::DefaultContextMenu:
            contextMenuEvent(static_cast<QContextMenuEvent *>(event));
            break;
        case Qt::CustomContextMenu:
            emit customContextMenuRequested(static_cast<QContextMenuEvent *>(event)->pos());
            break;
#ifndef QT_NO_MENU
        case Qt::ActionsContextMenu:
            if (d->actions.count()) {
                QMenu::exec(d->actions, static_cast<QContextMenuEvent *>(event)->globalPos());
                break;
            }
            // fall through
#endif
        case Qt::PreventContextMenu:
        default:
            event->ignore();
            break;
        }
        break;

#ifndef QT_NO_DRAGANDDROP
    case QEvent::Drop:
        dropEvent((QDropEvent*) event);
        break;

    case QEvent::DragEnter:
        dragEnterEvent((QDragEnterEvent*) event);
        break;

    case QEvent::DragMove:
        dragMoveEvent((QDragMoveEvent*) event);
        break;

    case QEvent::DragLeave:
        dragLeaveEvent((QDragLeaveEvent*) event);
        break;
#endif

    case QEvent::Show:
        showEvent((QShowEvent*) event);
        break;

    case QEvent::Hide:
        hideEvent((QHideEvent*) event);
        break;

    case QEvent::ShowWindowRequest:
        if (!isHidden())
            d->show_sys();
        break;

    case QEvent::ApplicationFontChange:
        d->resolveFont();
        break;
    case QEvent::ApplicationPaletteChange:
        if (!(windowType() == Qt::Desktop))
            d->resolvePalette();
        break;

    case QEvent::ToolBarChange:
    case QEvent::ActivationChange:
    case QEvent::EnabledChange:
    case QEvent::FontChange:
    case QEvent::StyleChange:
    case QEvent::PaletteChange:
    case QEvent::WindowTitleChange:
    case QEvent::IconTextChange:
    case QEvent::ModifiedChange:
    case QEvent::MouseTrackingChange:
    case QEvent::ParentChange:
    case QEvent::WindowStateChange:
    case QEvent::LocaleChange:
    case QEvent::MacSizeChange:
    case QEvent::ContentsRectChange:
        changeEvent(event);
        break;

    case QEvent::WindowActivate:
    case QEvent::WindowDeactivate: {
#ifdef QT3_SUPPORT
        windowActivationChange(event->type() != QEvent::WindowActivate);
#endif
        if (isVisible() && !palette().isEqual(QPalette::Active, QPalette::Inactive))
            update();
        QList<QObject*> childList = d->children;
        for (int i = 0; i < childList.size(); ++i) {
            QWidget *w = qobject_cast<QWidget *>(childList.at(i));
            if (w && w->isVisible() && !w->isWindow())
                QApplication::sendEvent(w, event);
        }
        break; }

    case QEvent::LanguageChange:
#ifdef QT3_SUPPORT
        languageChange();
#endif
        changeEvent(event);
        {
            QList<QObject*> childList = d->children;
            for (int i = 0; i < childList.size(); ++i) {
                QObject *o = childList.at(i);
                QApplication::sendEvent(o, event);
            }
        }
        update();
        break;

    case QEvent::ApplicationLayoutDirectionChange:
        d->resolveLayoutDirection();
        break;

    case QEvent::LayoutDirectionChange:
        if (d->layout)
            d->layout->invalidate();
        update();
        changeEvent(event);
        break;
#if defined(Q_WS_X11) || defined(Q_WS_QWS) || (defined(Q_WS_WIN) || defined(Q_WS_MAC))
    case QEvent::UpdateRequest: {
#ifdef Q_WS_MAC
        d->cleanWidget_sys(d->dirtyOnScreen);
        break;
#endif
#ifndef Q_WS_WIN
        extern void qt_syncBackingStore(QWidget *widget);
#endif
        qt_syncBackingStore(this);
        break; }
#endif

    case QEvent::UpdateLater:
        update(static_cast<QUpdateLaterEvent*>(event)->region());
        break;

    case QEvent::WindowBlocked:
    case QEvent::WindowUnblocked:
        {
            QList<QObject*> childList = d->children;
            for (int i = 0; i < childList.size(); ++i) {
                QObject *o = childList.at(i);
                if (o != qApp->activeModalWidget()) {
                    if (qobject_cast<QWidget *>(o) && static_cast<QWidget *>(o)->isWindow()) {
                        // do not forward the event to child windows,
                        // QApplication does this for us
                        continue;
                    }
                    QApplication::sendEvent(o, event);
                }
            }
        }
        break;
#ifndef QT_NO_TOOLTIP
    case QEvent::ToolTip:
        if (!d->toolTip.isEmpty())
            QToolTip::showText(static_cast<QHelpEvent*>(event)->globalPos(), d->toolTip, this);
        else
            event->ignore();
        break;
#endif
#ifndef QT_NO_WHATSTHIS
    case QEvent::WhatsThis:
        if (d->whatsThis.size())
            QWhatsThis::showText(static_cast<QHelpEvent *>(event)->globalPos(), d->whatsThis, this);
        else
            event->ignore();
        break;
    case QEvent::QueryWhatsThis:
        if (d->whatsThis.isEmpty())
            event->ignore();
        break;
#endif
#ifndef QT_NO_ACCESSIBILITY
    case QEvent::AccessibilityDescription:
    case QEvent::AccessibilityHelp: {
        QAccessibleEvent *ev = static_cast<QAccessibleEvent *>(event);
        if (ev->child())
            return false;
        switch (ev->type()) {
#ifndef QT_NO_TOOLTIP
        case QEvent::AccessibilityDescription:
            ev->setValue(d->toolTip);
            break;
#endif
#ifndef QT_NO_WHATSTHIS
        case QEvent::AccessibilityHelp:
            ev->setValue(d->whatsThis);
            break;
#endif
        default:
            return false;
        }
        break; }
#endif
    case QEvent::EmbeddingControl:
        d->topData()->frameStrut.setCoords(0 ,0, 0, 0);
        data->fstrut_dirty = false;
#if defined(Q_WS_WIN) || defined(Q_WS_X11)
        d->topData()->embedded = 1;
#endif
        break;
#ifndef QT_NO_ACTION
    case QEvent::ActionAdded:
    case QEvent::ActionRemoved:
    case QEvent::ActionChanged:
        actionEvent((QActionEvent*)event);
        break;
#endif

    case QEvent::KeyboardLayoutChange:
        {
            changeEvent(event);

            // inform children of the change
            QList<QObject*> childList = d->children;
            for (int i = 0; i < childList.size(); ++i) {
                QWidget *w = qobject_cast<QWidget *>(childList.at(i));
                if (w && w->isVisible() && !w->isWindow())
                    QApplication::sendEvent(w, event);
            }
            break;
        }
#ifdef Q_WS_MAC
    case QEvent::MacGLWindowChange:
        d->needWindowChange = false;
        break;
#endif
    default:
        return QObject::event(event);
    }
    return true;
}

/*!
  This event handler can be reimplemented to handle state changes.

  The state being changed in this event can be retrieved through event \a
  event.

  Change events include: QEvent::ToolBarChange,
  QEvent::ActivationChange, QEvent::EnabledChange, QEvent::FontChange,
  QEvent::StyleChange, QEvent::PaletteChange,
  QEvent::WindowTitleChange, QEvent::IconTextChange,
  QEvent::ModifiedChange, QEvent::MouseTrackingChange,
  QEvent::ParentChange, QEvent::WindowStateChange,
  QEvent::LanguageChange, QEvent::LocaleChange,
  QEvent::LayoutDirectionChange.

*/
void QWidget::changeEvent(QEvent * event)
{
    switch(event->type()) {
    case QEvent::EnabledChange:
        update();
#ifndef QT_NO_ACCESSIBILITY
        QAccessible::updateAccessibility(this, 0, QAccessible::StateChanged);
#endif
        break;

    case QEvent::FontChange:
    case QEvent::StyleChange: {
        update();
        updateGeometry();
#ifdef Q_WS_QWS
        Q_D(QWidget);
        if (isWindow())
            d->data.fstrut_dirty = true;
#endif
        break;
    }

    case QEvent::PaletteChange:
        update();
        break;

#ifdef Q_WS_MAC
    case QEvent::MacSizeChange:
        updateGeometry();
        break;
#endif

    default:
        break;
    }
}

/*!
    This event handler, for event \a event, can be reimplemented in a
    subclass to receive mouse move events for the widget.

    If mouse tracking is switched off, mouse move events only occur if
    a mouse button is pressed while the mouse is being moved. If mouse
    tracking is switched on, mouse move events occur even if no mouse
    button is pressed.

    QMouseEvent::pos() reports the position of the mouse cursor,
    relative to this widget. For press and release events, the
    position is usually the same as the position of the last mouse
    move event, but it might be different if the user's hand shakes.
    This is a feature of the underlying window system, not Qt.

    \sa setMouseTracking(), mousePressEvent(), mouseReleaseEvent(),
    mouseDoubleClickEvent(), event(), QMouseEvent, {Scribble Example}
*/

void QWidget::mouseMoveEvent(QMouseEvent *event)
{
    event->ignore();
}

/*!
    This event handler, for event \a event, can be reimplemented in a
    subclass to receive mouse press events for the widget.

    If you create new widgets in the mousePressEvent() the
    mouseReleaseEvent() may not end up where you expect, depending on
    the underlying window system (or X11 window manager), the widgets'
    location and maybe more.

    The default implementation implements the closing of popup widgets
    when you click outside the window. For other widget types it does
    nothing.

    \sa mouseReleaseEvent(), mouseDoubleClickEvent(),
    mouseMoveEvent(), event(), QMouseEvent, {Scribble Example}
*/

void QWidget::mousePressEvent(QMouseEvent *event)
{
    event->ignore();
    if ((windowType() == Qt::Popup)) {
        event->accept();
        QWidget* w;
        while ((w = qApp->activePopupWidget()) && w != this){
            w->close();
            if (qApp->activePopupWidget() == w) // widget does not want to dissappear
                w->hide(); // hide at least
        }
        if (!rect().contains(event->pos())){
            close();
        }
    }
}

/*!
    This event handler, for event \a event, can be reimplemented in a
    subclass to receive mouse release events for the widget.

    \sa mousePressEvent(), mouseDoubleClickEvent(),
    mouseMoveEvent(), event(), QMouseEvent, {Scribble Example}
*/

void QWidget::mouseReleaseEvent(QMouseEvent *event)
{
    event->ignore();
}

/*!
    This event handler, for event \a event, can be reimplemented in a
    subclass to receive mouse double click events for the widget.

    The default implementation generates a normal mouse press event.

    Note that the widgets gets a mousePressEvent() and a
    mouseReleaseEvent() before the mouseDoubleClickEvent().

    \sa mousePressEvent(), mouseReleaseEvent() mouseMoveEvent(),
    event(), QMouseEvent
*/

void QWidget::mouseDoubleClickEvent(QMouseEvent *event)
{
    mousePressEvent(event);                        // try mouse press event
}

#ifndef QT_NO_WHEELEVENT
/*!
    This event handler, for event \a event, can be reimplemented in a
    subclass to receive wheel events for the widget.

    If you reimplement this handler, it is very important that you
    \link QWheelEvent ignore()\endlink the event if you do not handle
    it, so that the widget's parent can interpret it.

    The default implementation ignores the event.

    \sa QWheelEvent::ignore(), QWheelEvent::accept(), event(),
    QWheelEvent
*/

void QWidget::wheelEvent(QWheelEvent *event)
{
    event->ignore();
}
#endif // QT_NO_WHEELEVENT

#ifndef QT_NO_TABLETEVENT
/*!
    This event handler, for event \a event, can be reimplemented in a
    subclass to receive tablet events for the widget.

    If you reimplement this handler, it is very important that you
    \link QTabletEvent ignore()\endlink the event if you do not handle
    it, so that the widget's parent can interpret it.

    The default implementation ignores the event.

    \sa QTabletEvent::ignore(), QTabletEvent::accept(), event(),
    QTabletEvent
*/

void QWidget::tabletEvent(QTabletEvent *event)
{
    event->ignore();
}
#endif // QT_NO_TABLETEVENT

/*!
    This event handler, for event \a event, can be reimplemented in a
    subclass to receive key press events for the widget.

    A widget must call setFocusPolicy() to accept focus initially and
    have focus in order to receive a key press event.

    If you reimplement this handler, it is very important that you
    call the base class implementation if you do not act upon the key.

    The default implementation closes popup widgets if the user
    presses Esc. Otherwise the event is ignored, so that the widget's
    parent can interpret it.

    Note that QKeyEvent starts with isAccepted() == true, so you do not
    need to call QKeyEvent::accept() - just do not call the base class
    implementation if you act upon the key.

    \sa keyReleaseEvent(), setFocusPolicy(),
    focusInEvent(), focusOutEvent(), event(), QKeyEvent, {Tetrix Example}
*/

void QWidget::keyPressEvent(QKeyEvent *event)
{
    if ((windowType() == Qt::Popup) && event->key() == Qt::Key_Escape) {
        event->accept();
        close();
    } else {
        event->ignore();
    }
}

/*!
    This event handler, for event \a event, can be reimplemented in a
    subclass to receive key release events for the widget.

    A widget must \link setFocusPolicy() accept focus\endlink
    initially and \link hasFocus() have focus\endlink in order to
    receive a key release event.

    If you reimplement this handler, it is very important that you
    call the base class implementation if you do not act upon the key.

    The default implementation ignores the event, so that the widget's
    parent can interpret it.

    Note that QKeyEvent starts with isAccepted() == true, so you do not
    need to call QKeyEvent::accept() - just do not call the base class
    implementation if you act upon the key.

    \sa keyPressEvent(), QKeyEvent::ignore(), setFocusPolicy(),
    focusInEvent(), focusOutEvent(), event(), QKeyEvent
*/

void QWidget::keyReleaseEvent(QKeyEvent *event)
{
    event->ignore();
}

/*!
    \fn void QWidget::focusInEvent(QFocusEvent *event)

    This event handler can be reimplemented in a subclass to receive
    keyboard focus events (focus received) for the widget. The event
    is passed in the \a event parameter

    A widget normally must setFocusPolicy() to something other than
    Qt::NoFocus in order to receive focus events. (Note that the
    application programmer can call setFocus() on any widget, even
    those that do not normally accept focus.)

    The default implementation updates the widget (except for windows
    that do not specify a focusPolicy()).

    \sa focusOutEvent(), setFocusPolicy(), keyPressEvent(),
    keyReleaseEvent(), event(), QFocusEvent
*/

void QWidget::focusInEvent(QFocusEvent *)
{
    if (focusPolicy() != Qt::NoFocus || !isWindow()) {
        update();
    }
}

/*!
    \fn void QWidget::focusOutEvent(QFocusEvent *event)

    This event handler can be reimplemented in a subclass to receive
    keyboard focus events (focus lost) for the widget. The events is
    passed in the \a event parameter.

    A widget normally must setFocusPolicy() to something other than
    Qt::NoFocus in order to receive focus events. (Note that the
    application programmer can call setFocus() on any widget, even
    those that do not normally accept focus.)

    The default implementation updates the widget (except for windows
    that do not specify a focusPolicy()).

    \sa focusInEvent(), setFocusPolicy(), keyPressEvent(),
    keyReleaseEvent(), event(), QFocusEvent
*/

void QWidget::focusOutEvent(QFocusEvent *)
{
    if (focusPolicy() != Qt::NoFocus || !isWindow())
        update();
}

/*!
    \fn void QWidget::enterEvent(QEvent *event)

    This event handler can be reimplemented in a subclass to receive
    widget enter events which are passed in the \a event parameter.

    An event is sent to the widget when the mouse cursor enters the
    widget.

    \sa leaveEvent(), mouseMoveEvent(), event()
*/

void QWidget::enterEvent(QEvent *)
{
}

/*!
    \fn void QWidget::leaveEvent(QEvent *event)

    This event handler can be reimplemented in a subclass to receive
    widget leave events which are passed in the \a event parameter.

    A leave event is sent to the widget when the mouse cursor leaves
    the widget.

    \sa enterEvent(), mouseMoveEvent(), event()
*/

void QWidget::leaveEvent(QEvent *)
{
}

/*!
    \fn void QWidget::paintEvent(QPaintEvent *event)

    This event handler can be reimplemented in a subclass to receive
    paint events which are passed in the \a event parameter.

    A paint event is a request to repaint all or part of the widget.
    It can happen as a result of repaint() or update(), or because the
    widget was obscured and has now been uncovered, or for many other
    reasons.

    Many widgets can simply repaint their entire surface when asked
    to, but some slow widgets need to optimize by painting only the
    requested region: QPaintEvent::region(). This speed optimization
    does not change the result, as painting is clipped to that region
    during event processing. QListView and QTableView do this, for
    example.

    Qt also tries to speed up painting by merging multiple paint
    events into one. When update() is called several times or the
    window system sends several paint events, Qt merges these events
    into one event with a larger region (see QRegion::united()).
    repaint() does not permit this optimization, so we suggest using
    update() whenever possible.

    When the paint event occurs, the update region has normally been
    erased, so that you're painting on the widget's background.

    The background can be set using setBackgroundRole() and
    setPalette().

    From Qt 4.0, QWidget automatically double-buffers its painting, so
    there's no need to write double-buffering code in paintEvent() to
    avoid flicker.

    Note: Under X11 it is possible to toggle the global double
    buffering by calling \c qt_x11_set_global_double_buffer().
    Example usage:
    \snippet doc/src/snippets/code/src.gui.kernel.qwidget.cpp 14

    Note: In general, one should refrain from calling update() or repaint()
    \bold {inside} of paintEvent(). For example, calling update() or repaint() on
    children inside a paintEvent() results in undefined behavior; the child may
    or may not get a paint event.

    \sa event(), repaint(), update(), QPainter, QPixmap, QPaintEvent, {Analog Clock Example}
*/

void QWidget::paintEvent(QPaintEvent *)
{
}


/*!
    \fn void QWidget::moveEvent(QMoveEvent *event)

    This event handler can be reimplemented in a subclass to receive
    widget move events which are passed in the \a event parameter.
    When the widget receives this event, it is already at the new
    position.

    The old position is accessible through QMoveEvent::oldPos().

    \sa resizeEvent(), event(), move(), QMoveEvent
*/

void QWidget::moveEvent(QMoveEvent *)
{
}


/*!
    This event handler can be reimplemented in a subclass to receive
    widget resize events which are passed in the \a event parameter.
    When resizeEvent() is called, the widget already has its new
    geometry. The old size is accessible through
    QResizeEvent::oldSize().

    The widget will be erased and receive a paint event immediately
    after processing the resize event. No drawing need be (or should
    be) done inside this handler.


    \sa moveEvent(), event(), resize(), QResizeEvent, paintEvent(),
        {Scribble Example}
*/

void QWidget::resizeEvent(QResizeEvent * /* event */)
{
}

#ifndef QT_NO_ACTION
/*!
    \fn void QWidget::actionEvent(QActionEvent *event)

    This event handler is called with the given \a event whenever the
    widget's actions are changed.

    \sa addAction(), insertAction(), removeAction(), actions(), QActionEvent
*/
void QWidget::actionEvent(QActionEvent *)
{

}
#endif

/*!
    This event handler is called with the given \a event when Qt receives a window
    close request for a top-level widget from the window system.

    By default, the event is accepted and the widget is closed. You can reimplement
    this function to change the way the widget responds to window close requests.
    For example, you can prevent the window from closing by calling \l{QEvent::}{ignore()}
    on all events.

    Main window applications typically use reimplementations of this function to check
    whether the user's work has been saved and ask for permission before closing.
    For example, the \l{Application Example} uses a helper function to determine whether
    or not to close the window:

    \snippet mainwindows/application/mainwindow.cpp 3
    \snippet mainwindows/application/mainwindow.cpp 4

    \sa event(), hide(), close(), QCloseEvent, {Application Example}
*/

void QWidget::closeEvent(QCloseEvent *event)
{
    event->accept();
}


/*!
    This event handler, for event \a event, can be reimplemented in a
    subclass to receive widget context menu events.

    The handler is called when the widget's \l contextMenuPolicy is
    Qt::DefaultContextMenu.

    The default implementation ignores the context event.
    See the \l QContextMenuEvent documentation for more details.

    \sa event(), QContextMenuEvent customContextMenuRequested()
*/

void QWidget::contextMenuEvent(QContextMenuEvent *event)
{
    event->ignore();
}


/*!
    This event handler, for event \a event, can be reimplemented in a
    subclass to receive Input Method composition events. This handler
    is called when the state of the input method changes.

    Note that when creating custom text editing widgets, the
    Qt::WA_InputMethodEnabled window attribute must be set explicitly
    (using the setAttribute() function) in order to receive input
    method events.

    The default implementation calls event->ignore(), which rejects the
    Input Method event. See the \l QInputMethodEvent documentation for more
    details.

    \sa event(), QInputMethodEvent
*/
void QWidget::inputMethodEvent(QInputMethodEvent *event)
{
    event->ignore();
}

/*!
    This method is only relevant for input widgets. It is used by the
    input method to query a set of properties of the widget to be
    able to support complex input method operations as support for
    surrounding text and reconversions.

    \a query specifies which property is queried.

    \sa inputMethodEvent(), QInputMethodEvent, QInputContext
*/
QVariant QWidget::inputMethodQuery(Qt::InputMethodQuery query) const
{
    switch(query) {
    case Qt::ImMicroFocus:
        return QRect(width()/2, 0, 1, height());
    case Qt::ImFont:
        return font();
    default:
        return QVariant();
    }
}

#ifndef QT_NO_DRAGANDDROP

/*!
    \fn void QWidget::dragEnterEvent(QDragEnterEvent *event)

    This event handler is called when a drag is in progress and the
    mouse enters this widget. The event is passed in the \a event parameter.

    If the event is ignored, the widget won't receive any \l{dragMoveEvent()}{drag
    move events}.

    See the \link dnd.html Drag-and-drop documentation\endlink for an
    overview of how to provide drag-and-drop in your application.

    \sa QDrag, QDragEnterEvent
*/
void QWidget::dragEnterEvent(QDragEnterEvent *)
{
}

/*!
    \fn void QWidget::dragMoveEvent(QDragMoveEvent *event)

    This event handler is called if a drag is in progress, and when
    any of the following conditions occur: the cursor enters this widget,
    the cursor moves within this widget, or a modifier key is pressed on
    the keyboard while this widget has the focus. The event is passed
    in the \a event parameter.

    See the \link dnd.html Drag-and-drop documentation\endlink for an
    overview of how to provide drag-and-drop in your application.

    \sa QDrag, QDragMoveEvent
*/
void QWidget::dragMoveEvent(QDragMoveEvent *)
{
}

/*!
    \fn void QWidget::dragLeaveEvent(QDragLeaveEvent *event)

    This event handler is called when a drag is in progress and the
    mouse leaves this widget. The event is passed in the \a event
    parameter.

    See the \link dnd.html Drag-and-drop documentation\endlink for an
    overview of how to provide drag-and-drop in your application.

    \sa QDrag, QDragLeaveEvent
*/
void QWidget::dragLeaveEvent(QDragLeaveEvent *)
{
}

/*!
    \fn void QWidget::dropEvent(QDropEvent *event)

    This event handler is called when the drag is dropped on this
    widget. The event is passed in the \a event parameter.

    See the \link dnd.html Drag-and-drop documentation\endlink for an
    overview of how to provide drag-and-drop in your application.

    \sa QDrag, QDropEvent
*/
void QWidget::dropEvent(QDropEvent *)
{
}

#endif // QT_NO_DRAGANDDROP

/*!
    \fn void QWidget::showEvent(QShowEvent *event)

    This event handler can be reimplemented in a subclass to receive
    widget show events which are passed in the \a event parameter.

    Non-spontaneous show events are sent to widgets immediately
    before they are shown. The spontaneous show events of windows are
    delivered afterwards.

    Note: A widget receives spontaneous show and hide events when its
    mapping status is changed by the window system, e.g. a spontaneous
    hide event when the user minimizes the window, and a spontaneous
    show event when the window is restored again. After receiving a
    spontaneous hide event, a widget is still considered visible in
    the sense of isVisible().

    \sa visible, event(), QShowEvent
*/
void QWidget::showEvent(QShowEvent *)
{
}

/*!
    \fn void QWidget::hideEvent(QHideEvent *event)

    This event handler can be reimplemented in a subclass to receive
    widget hide events. The event is passed in the \a event parameter.

    Hide events are sent to widgets immediately after they have been
    hidden.

    Note: A widget receives spontaneous show and hide events when its
    mapping status is changed by the window system, e.g. a spontaneous
    hide event when the user minimizes the window, and a spontaneous
    show event when the window is restored again. After receiving a
    spontaneous hide event, a widget is still considered visible in
    the sense of isVisible().

    \sa visible, event(), QHideEvent
*/
void QWidget::hideEvent(QHideEvent *)
{
}

/*
    \fn QWidget::x11Event(MSG *)

    This special event handler can be reimplemented in a subclass to
    receive native X11 events.

    In your reimplementation of this function, if you want to stop the
    event being handled by Qt, return true. If you return false, this
    native event is passed back to Qt, which translates the event into
    a Qt event and sends it to the widget.

    \warning This function is not portable.

    \sa QApplication::x11EventFilter()
*/


#if defined(Q_WS_MAC)

/*!
    \fn bool QWidget::macEvent(EventHandlerCallRef caller, EventRef event)

    This special event handler can be reimplemented in a subclass to
    receive native Macintosh events which are passed from the \a
    caller with the event details in the \a event parameter.

    In your reimplementation of this function, if you want to stop the
    event being handled by Qt, return true. If you return false, this
    native event is passed back to Qt, which translates the event into
    a Qt event and sends it to the widget.

    \warning This function is not portable.

    \warning This function was not called inside of Qt until Qt 4.4.
    For compatibility with earlier versions of Qt, consider using QApplication::macEventFilter()
    instead.

    \sa QApplication::macEventFilter()
*/

bool QWidget::macEvent(EventHandlerCallRef, EventRef)
{
    return false;
}

#endif
#if defined(Q_WS_WIN)

/*!
    This special event handler can be reimplemented in a subclass to
    receive native Windows events which are passed in the \a message
    parameter.

    In your reimplementation of this function, if you want to stop the
    event being handled by Qt, return true and set \a result to the value
    that the window procedure should return. If you return false, this
    native event is passed back to Qt, which translates the event into
    a Qt event and sends it to the widget.

    \warning This function is not portable.

    \sa QApplication::winEventFilter()
*/
bool QWidget::winEvent(MSG *message, long *result)
{
    Q_UNUSED(message);
    Q_UNUSED(result);
    return false;
}

#endif
#if defined(Q_WS_X11)

/*!
    \fn bool QWidget::x11Event(XEvent *event)

    This special event handler can be reimplemented in a subclass to
    receive native X11 events which are passed in the \a event
    parameter.

    In your reimplementation of this function, if you want to stop the
    event being handled by Qt, return true. If you return false, this
    native event is passed back to Qt, which translates the event into
    a Qt event and sends it to the widget.

    \warning This function is not portable.

    \sa QApplication::x11EventFilter()
*/
bool QWidget::x11Event(XEvent *)
{
    return false;
}

#endif
#if defined(Q_WS_QWS)

/*!
    \fn bool QWidget::qwsEvent(QWSEvent *event)

    This special event handler can be reimplemented in a subclass to
    receive native Qt for Embedded Linux events which are passed in the
    \a event parameter.

    In your reimplementation of this function, if you want to stop the
    event being handled by Qt, return true. If you return false, this
    native event is passed back to Qt, which translates the event into
    a Qt event and sends it to the widget.

    \warning This function is not portable.

    \sa QApplication::qwsEventFilter()
*/
bool QWidget::qwsEvent(QWSEvent *)
{
    return false;
}

#endif


/*!
    Ensures that the widget has been polished by QStyle (i.e., has a
    proper font and palette).

    QWidget calls this function after it has been fully constructed
    but before it is shown the very first time. You can call this
    function if you want to ensure that the widget is polished before
    doing an operation, e.g., the correct font size might be needed in
    the widget's sizeHint() reimplementation. Note that this function
    \e is called from the default implementation of sizeHint().

    Polishing is useful for final initialization that must happen after
    all constructors (from base classes as well as from subclasses)
    have been called.

    If you need to change some settings when a widget is polished,
    reimplement event() and handle the QEvent::Polish event type.

    \bold{Note:} The function is declared const so that it can be called from
    other const functions (e.g., sizeHint()).

    \sa event()
*/
void QWidget::ensurePolished() const
{
    Q_D(const QWidget);

    const QMetaObject *m = metaObject();
    if (m == d->polished)
        return;
    d->polished = m;

    QEvent e(QEvent::Polish);
    QCoreApplication::sendEvent(const_cast<QWidget *>(this), &e);

    // polish children after 'this'
    QList<QObject*> children = d->children;
    for (int i = 0; i < children.size(); ++i) {
        QObject *o = children.at(i);
        if(!o->isWidgetType())
            continue;
        if (QWidget *w = qobject_cast<QWidget *>(o))
            w->ensurePolished();
    }

    if (d->parent && d->sendChildEvents) {
        QChildEvent e(QEvent::ChildPolished, const_cast<QWidget *>(this));
        QCoreApplication::sendEvent(d->parent, &e);
    }
}

/*!
    Returns the mask currently set on a widget. If no mask is set the
    return value will be an empty region.

    \sa setMask(), clearMask(), QRegion::isEmpty(), {Shaped Clock Example}
*/
QRegion QWidget::mask() const
{
    Q_D(const QWidget);
    return d->extra ? d->extra->mask : QRegion();
}

/*!
    Returns the layout manager that is installed on this widget, or 0
    if no layout manager is installed.

    The layout manager sets the geometry of the widget's children
    that have been added to the layout.

    \sa setLayout(), sizePolicy(), {Layout Classes}
*/
QLayout *QWidget::layout() const
{
    return d_func()->layout;
}


/*!
    \fn void QWidget::setLayout(QLayout *layout)

    Sets the layout manager for this widget to \a layout.

    If there already is a layout manager installed on this widget,
    QWidget won't let you install another. You must first delete the
    existing layout manager (returned by layout()) before you can
    call setLayout() with the new layout.

    Example:

    \snippet examples/tutorials/tutorial/t10/lcdrange.cpp 0

    An alternative to calling this function is to pass this widget to
    the layout's constructor.

    The QWidget will take ownership of \a layout.

    \sa layout(), {Layout Classes}
*/

void QWidget::setLayout(QLayout *l)
{
    if (!l) {
        qWarning("QWidget::setLayout: Cannot set layout to 0");
        return;
    }
    if (layout()) {
        if (layout() != l)
            qWarning("QWidget::setLayout: Attempting to set QLayout \"%s\" on %s \"%s\", which already has a"
                     " layout", l->objectName().toLocal8Bit().data(), metaObject()->className(),
                     objectName().toLocal8Bit().data());
        return;
    }

    QObject *oldParent = l->parent();
    if (oldParent && oldParent != this) {
        qWarning("QWidget::setLayout: Attempting to set QLayout \"%s\" on %s \"%s\", when the QLayout already has a parent",
                 l->objectName().toLocal8Bit().data(), metaObject()->className(),
                 objectName().toLocal8Bit().data());
        return;
    }

    Q_D(QWidget);
    l->d_func()->topLevel = true;
    d->layout = l;
    if (oldParent != this) {
        l->setParent(this);
        l->d_func()->reparentChildWidgets(this);
        l->invalidate();
    }

    if (isWindow() && d->maybeTopData())
        d->topData()->sizeAdjusted = false;
}



/*!
    \property QWidget::sizePolicy
    \brief the default layout behavior of the widget

    If there is a QLayout that manages this widget's children, the
    size policy specified by that layout is used. If there is no such
    QLayout, the result of this function is used.

    The default policy is Preferred/Preferred, which means that the
    widget can be freely resized, but prefers to be the size
    sizeHint() returns. Button-like widgets set the size policy to
    specify that they may stretch horizontally, but are fixed
    vertically. The same applies to lineedit controls (such as
    QLineEdit, QSpinBox or an editable QComboBox) and other
    horizontally orientated widgets (such as QProgressBar).
    QToolButton's are normally square, so they allow growth in both
    directions. Widgets that support different directions (such as
    QSlider, QScrollBar or QHeader) specify stretching in the
    respective direction only. Widgets that can provide scroll bars
    (usually subclasses of QScrollArea) tend to specify that they can
    use additional space, and that they can make do with less than
    sizeHint().

    \sa sizeHint() QLayout QSizePolicy updateGeometry()
*/
QSizePolicy QWidget::sizePolicy() const
{
    Q_D(const QWidget);
    return d->size_policy;
}

void QWidget::setSizePolicy(QSizePolicy policy)
{
    Q_D(QWidget);
    setAttribute(Qt::WA_WState_OwnSizePolicy);
    if (policy == d->size_policy)
        return;
    d->size_policy = policy;

#ifndef QT_NO_GRAPHICSVIEW
    if (QTLWExtra *topData = d->maybeTopData()) {
        if (topData->proxyWidget)
            topData->proxyWidget->setSizePolicy(policy);
    }
#endif

    updateGeometry();

    if (isWindow() && d->maybeTopData())
        d->topData()->sizeAdjusted = false;
}

/*!
    \fn void QWidget::setSizePolicy(QSizePolicy::Policy horizontal, QSizePolicy::Policy vertical)
    \overload

    Sets the size policy of the widget to \a horizontal and \a
    vertical, with standard stretch and no height-for-width.

    \sa QSizePolicy::QSizePolicy()
*/

/*!
    Returns the preferred height for this widget, given the width \a w.

    If this widget has a layout, the default implementation returns
    the layout's preferred height.  if there is no layout, the default
    implementation returns -1 indicating that the preferred height
    does not depend on the width.
*/

int QWidget::heightForWidth(int w) const
{
    if (layout() && layout()->hasHeightForWidth())
        return layout()->totalHeightForWidth(w);
    return -1;
}

/*!
    \fn QWidget *QWidget::childAt(int x, int y) const

    Returns the visible child widget at the position (\a{x}, \a{y})
    in the widget's coordinate system. If there is no visible child
    widget at the specified position, the function returns 0.
*/

/*!
    \overload

    Returns the visible child widget at point \a p in the widget's own
    coordinate system.
*/

QWidget *QWidget::childAt(const QPoint &p) const
{
    if (!rect().contains(p))
        return 0;
    Q_D(const QWidget);
    for (int i = d->children.size(); i > 0 ;) {
        --i;
        QWidget *w = qobject_cast<QWidget *>(d->children.at(i));
        if (w && !w->isWindow() && !w->isHidden() && w->geometry().contains(p)) {
            if (w->testAttribute(Qt::WA_TransparentForMouseEvents))
                continue;
            QPoint childPoint = w->mapFromParent(p);
            if (QWidget *t = w->childAt(childPoint))
                return t;
            // if WMouseNoMask is set the widget mask is ignored, if
            // the widget has no mask then the WMouseNoMask flag has no
            // effect
            if (w->testAttribute(Qt::WA_MouseNoMask) || w->mask().contains(childPoint)
                || w->mask().isEmpty())
                return w;
        }
    }
    return 0;
}

void QWidgetPrivate::updateGeometry_helper(bool forceUpdate)
{
    Q_Q(QWidget);
    if (widgetItem)
        widgetItem->invalidateSizeCache();
    QWidget *parent;
    if (forceUpdate || !extra || extra->minw != extra->maxw || extra->minh != extra->maxh) {
        if (!q->isWindow() && !q->isHidden() && (parent = q->parentWidget())) {
            if (parent->d_func()->layout)
                parent->d_func()->layout->invalidate();
            else if (parent->isVisible())
                QApplication::postEvent(parent, new QEvent(QEvent::LayoutRequest));
        }
    }
}

/*!
    Notifies the layout system that this widget has changed and may
    need to change geometry.

    Call this function if the sizeHint() or sizePolicy() have changed.

    For explicitly hidden widgets, updateGeometry() is a no-op. The
    layout system will be notified as soon as the widget is shown.
*/

void QWidget::updateGeometry()
{
    Q_D(QWidget);
    d->updateGeometry_helper(false);
}

/*! \property QWidget::windowFlags

    Window flags are a combination of a type (e.g. Qt::Dialog) and
    zero or more hints to the window system (e.g.
    Qt::FramelessWindowHint).

    If the widget had type Qt::Widget or Qt::SubWindow and becomes a
    window (Qt::Window, Qt::Dialog, etc.), it is put at position (0,
    0) on the desktop. If the widget is a window and becomes a
    Qt::Widget or Qt::SubWindow, it is put at position (0, 0)
    relative to its parent widget.

    \note This function calls setParent() when changing the flags for
    a window, causing the widget to be hidden. You must call show() to make
    the widget visible again..

    \sa windowType(), {Window Flags Example}
*/
void QWidget::setWindowFlags(Qt::WindowFlags flags)
{
    if (data->window_flags == flags)
        return;

    if ((data->window_flags | flags) & Qt::Window) {
        // the old type was a window and/or the new type is a window
        QPoint oldPos = pos();
        setParent(parentWidget(), flags);

        // if both types are windows or neither of them are, we restore
        // the old position
        if (!((data->window_flags ^ flags) & Qt::Window) && testAttribute(Qt::WA_Moved))
            move(oldPos);
    } else {
        data->window_flags = flags;
    }
}

/*!
    Sets the window flags for the widget to \a flags,
    \e without telling the window system.

    \warning Do not call this function unless you really know what
    you're doing.

    \sa setWindowFlags()
*/
void QWidget::overrideWindowFlags(Qt::WindowFlags flags)
{
    data->window_flags = flags;
}

/*!
    \fn Qt::WindowType QWidget::windowType() const

    Returns the window type of this widget. This is identical to
    windowFlags() & Qt::WindowType_Mask.

    \sa windowFlags
*/

/*!
    Sets the parent of the widget to \a parent, and resets the window
    flags. The widget is moved to position (0, 0) in its new parent.

    If the new parent widget is in a different window, the
    reparented widget and its children are appended to the end of the
    \l{setFocusPolicy()}{tab chain} of the new parent
    widget, in the same internal order as before. If one of the moved
    widgets had keyboard focus, setParent() calls clearFocus() for that
    widget.

    If the new parent widget is in the same window as the
    old parent, setting the parent doesn't change the tab order or
    keyboard focus.

    If the "new" parent widget is the old parent widget, this function
    does nothing.

    \note The widget becomes invisible as part of changing its parent,
    even if it was previously visible. You must call show() to make the
    widget visible again.

    \warning It is very unlikely that you will ever need this
    function. If you have a widget that changes its content
    dynamically, it is far easier to use \l QStackedWidget.

    \sa setWindowFlags()
*/
void QWidget::setParent(QWidget *parent)
{
    if (parent == parentWidget())
        return;
    setParent((QWidget*)parent, windowFlags() & ~Qt::WindowType_Mask);
}

/*!
    \overload

    This function also takes widget flags, \a f as an argument.
*/

void QWidget::setParent(QWidget *parent, Qt::WindowFlags f)
{
    Q_D(QWidget);
    bool resized = testAttribute(Qt::WA_Resized);
    bool wasCreated = testAttribute(Qt::WA_WState_Created);
    QWidget *oldtlw = window();
    bool newParent = (parent != parentWidget()) || !wasCreated;

    if (wasCreated) {
        if (!testAttribute(Qt::WA_WState_Hidden)) {
            hide();
            setAttribute(Qt::WA_WState_ExplicitShowHide, false);
        }
        if (newParent) {
            QEvent e(QEvent::ParentAboutToChange);
            QApplication::sendEvent(this, &e);
        }
    }
    if (newParent && isAncestorOf(focusWidget()))
        focusWidget()->clearFocus();

    d->setParent_sys(parent, f);

#ifdef Q_BACKINGSTORE_SUBSURFACES
    QTLWExtra *extra = d->maybeTopData();
    QWindowSurface *windowSurface = (extra ? extra->windowSurface : 0);
    if (newParent && windowSurface) {
        QWidgetBackingStore *oldBs = oldtlw->d_func()->maybeBackingStore();
        if (oldBs)
            oldBs->subSurfaces.removeAll(windowSurface);

        if (parent) {
            QWidgetBackingStore *newBs = parent->d_func()->maybeBackingStore();
            if (newBs)
                newBs->subSurfaces.append(windowSurface);
        }
    }
#endif

#ifdef Q_WIDGET_USE_DIRTYLIST
    if (newParent) {
        QWidgetBackingStore *oldBs = oldtlw->d_func()->maybeBackingStore();
        if (oldBs)
            oldBs->removeDirtyWidget(this);
    }
#endif

    if ((QApplicationPrivate::app_compile_version < 0x040200
         || QApplicationPrivate::testAttribute(Qt::AA_ImmediateWidgetCreation))
        && !testAttribute(Qt::WA_WState_Created))
        create();

    d->reparentFocusWidgets(oldtlw);
    setAttribute(Qt::WA_Resized, resized);
    if (!testAttribute(Qt::WA_StyleSheet)
        && (!parent || !parent->testAttribute(Qt::WA_StyleSheet))) {
        d->resolveFont();
        d->resolvePalette();
    }
    d->resolveLayoutDirection();
    d->resolveLocale();

    if (QTLWExtra *extra = oldtlw->d_func()->maybeTopData())
        extra->sizeAdjusted = false;
    if (QTLWExtra *extra = window()->d_func()->maybeTopData())
        extra->sizeAdjusted = false;

    // Note: GL widgets under Windows will always need a ParentChange
    // event to handle recreation/rebinding of the GL context, hence
    // the (f & Qt::MSWindowsOwnDC) clause
    if (newParent
#ifdef Q_WS_WIN
        || (f & Qt::MSWindowsOwnDC)
#endif
        ) {
        // propagate enabled updates enabled state to non-windows
        if (!isWindow()) {
            if (!testAttribute(Qt::WA_ForceDisabled))
                d->setEnabled_helper(parent ? parent->isEnabled() : true);
            if (!testAttribute(Qt::WA_ForceUpdatesDisabled))
                d->setUpdatesEnabled_helper(parent ? parent->updatesEnabled() : true);
        }
        d->inheritStyle();

        // send and post remaining QObject events
        if (parent && d->sendChildEvents) {
            QChildEvent e(QEvent::ChildAdded, this);
            QApplication::sendEvent(parent, &e);
#ifdef QT3_SUPPORT
            if (parent->d_func()->pendingChildInsertedEvents.isEmpty()) {
                QApplication::postEvent(parent,
                                        new QEvent(QEvent::ChildInsertedRequest),
                                        Qt::HighEventPriority);
            }
            parent->d_func()->pendingChildInsertedEvents.append(this);
#endif
        }

//### already hidden above ---> must probably do something smart on the mac
// #ifdef Q_WS_MAC
//             extern bool qt_mac_is_macdrawer(const QWidget *); //qwidget_mac.cpp
//             if(!qt_mac_is_macdrawer(q)) //special case
//                 q->setAttribute(Qt::WA_WState_Hidden);
// #else
//             q->setAttribute(Qt::WA_WState_Hidden);
//#endif

        if (parent && d->sendChildEvents && d->polished) {
            QChildEvent e(QEvent::ChildPolished, this);
            QCoreApplication::sendEvent(parent, &e);
        }

        QEvent e(QEvent::ParentChange);
        QApplication::sendEvent(this, &e);
    }

    if (!wasCreated) {
        if (isWindow() || parentWidget()->isVisible())
            setAttribute(Qt::WA_WState_Hidden, true);
        else if (!testAttribute(Qt::WA_WState_ExplicitShowHide))
            setAttribute(Qt::WA_WState_Hidden, false);
    }

#ifndef QT_NO_GRAPHICSVIEW
    // Embed the widget into a proxy if the parent is embedded.
    // ### Doesn't handle reparenting out of an embedded widget.
    if (oldtlw->d_func()->graphicsProxyWidget()) {
        if (QGraphicsProxyWidget *ancestorProxy = d->nearestGraphicsProxyWidget(oldtlw))
            ancestorProxy->d_func()->unembedSubWindow(this);
    }
    if (isWindow() && parent && !d->graphicsProxyWidget()) {
        if (QGraphicsProxyWidget *ancestorProxy = d->nearestGraphicsProxyWidget(parent))
            ancestorProxy->d_func()->embedSubWindow(this);
    }
#endif
}

/*!
    Scrolls the widget including its children \a dx pixels to the
    right and \a dy downward. Both \a dx and \a dy may be negative.

    After scrolling, the widgets will receive paint events for
    the areas that need to be repainted. For widgets that Qt knows to
    be opaque, this is only the newly exposed parts.
    For example, if an opaque widget is scrolled 8 pixels to the left,
    only an 8-pixel wide stripe at the right edge needs updating.

    Since widgets propagate the contents of their parents by default,
    you need to set the \l autoFillBackground property, or use
    setAttribute() to set the Qt::WA_OpaquePaintEvent attribute, to make
    a widget opaque.

    For widgets that use contents propagation, a scroll will cause an
    update of the entire scroll area.

    \sa {Transparency and Double Buffering}
*/

void QWidget::scroll(int dx, int dy)
{
    if (!updatesEnabled() && children().size() == 0 || !isVisible())
        return;
    if (dx == 0 && dy == 0)
        return;
    Q_D(QWidget);
#ifndef QT_NO_GRAPHICSVIEW
    if (QGraphicsProxyWidget *proxy = window()->d_func()->maybeTopData()->proxyWidget) {
        proxy->scroll(dx, dy, proxy->subWidgetRect(this));
        return;
    }
#endif
    d->setDirtyOpaqueRegion();
    d->scroll_sys(dx, dy);
}

/*!
    \overload

    This version only scrolls \a r and does not move the children of
    the widget.

    If \a r is empty or invalid, the result is undefined.

    \sa QScrollArea bitBlt()
*/
void QWidget::scroll(int dx, int dy, const QRect &r)
{

    if (!updatesEnabled() && children().size() == 0 || !isVisible())
        return;
    if (dx == 0 && dy == 0)
        return;
    Q_D(QWidget);
#ifndef QT_NO_GRAPHICSVIEW
    if (QGraphicsProxyWidget *proxy = window()->d_func()->maybeTopData()->proxyWidget) {
        proxy->scroll(dx, dy, r.translated(proxy->subWidgetRect(this).topLeft().toPoint()));
        return;
    }
#endif
    d->scroll_sys(dx, dy, r);
}

/*!
    Repaints the widget directly by calling paintEvent() immediately,
    unless updates are disabled or the widget is hidden.

    We suggest only using repaint() if you need an immediate repaint,
    for example during animation. In almost all circumstances update()
    is better, as it permits Qt to optimize for speed and minimize
    flicker.

    \warning If you call repaint() in a function which may itself be
    called from paintEvent(), you may get infinite recursion. The
    update() function never causes recursion.

    \sa update(), paintEvent(), setUpdatesEnabled()
*/

void QWidget::repaint()
{
#ifdef QT_GREENPHONE_OPT
    repaint(rect());
#else
    Q_D(QWidget);
    repaint(QRegion(d->clipRect()));
#endif
}

/*! \overload

    This version repaints a rectangle (\a x, \a y, \a w, \a h) inside
    the widget.

    If \a w is negative, it is replaced with \c{width() - x}, and if
    \a h is negative, it is replaced width \c{height() - y}.
*/
void QWidget::repaint(int x, int y, int w, int h)
{
    if (x > data->crect.width() || y > data->crect.height())
        return;

    if (w < 0)
        w = data->crect.width()  - x;
    if (h < 0)
        h = data->crect.height() - y;
#ifdef QT_GREENPHONE_OPT
    // At least on embedded, we don't need to intersect against the clipRect
    // here, as we do it within QWidgetBackingStore::dirtyRegion()
    repaint(QRect(x, y, w, h));
#else
    Q_D(QWidget);
    repaint(d->clipRect().intersected(QRect(x, y, w, h)));
#endif
}

/*! \overload

    This version repaints a rectangle \a r inside the widget.
*/
void QWidget::repaint(const QRect &r)
{
#ifdef QT_GREENPHONE_OPT
    // At least on embedded, we don't need to intersect against the clipRect
    // here, as we do it within QWidgetBackingStore::dirtyRegion()
    repaint(QRegion(r));
#else
    Q_D(QWidget);
    repaint(QRegion(d->clipRect().intersected(r)));
#endif
}

/*! \fn void QWidget::repaint(const QRegion &rgn)
    \overload

    This version repaints a region \a rgn inside the widget.
*/

/*!
    \fn void QWidget::update()
    Updates the widget unless updates are disabled or the widget is
    hidden.

    This function does not cause an immediate repaint; instead it
    schedules a paint event for processing when Qt returns to the main
    event loop. This permits Qt to optimize for more speed and less
    flicker than a call to repaint() does.

    Calling update() several times normally results in just one
    paintEvent() call.

    Qt normally erases the widget's area before the paintEvent() call.
    If the Qt::WA_OpaquePaintEvent widget attribute is set, the widget is
    responsible for painting all its pixels with an opaque color.

    \sa repaint() paintEvent(), setUpdatesEnabled(), {Analog Clock Example}
*/

/*! \fn void QWidget::update(int x, int y, int w, int h)
    \overload

    This version updates a rectangle (\a x, \a y, \a w, \a h) inside
    the widget.
*/


/*!
    \fn void QWidget::update(const QRect &r)
    \overload

    This version updates a rectangle \a r inside the widget.
*/

/*! \fn void QWidget::update(const QRegion &rgn)
    \overload

    This version repaints a region \a rgn inside the widget.
*/

#ifdef QT3_SUPPORT
/*!
    Clear the rectangle at point (\a x, \a y) of width \a w and height
    \a h.

    \warning This is best done in a paintEvent().
*/
void QWidget::erase_helper(int x, int y, int w, int h)
{
    if (testAttribute(Qt::WA_NoSystemBackground) || testAttribute(Qt::WA_UpdatesDisabled) ||  !testAttribute(Qt::WA_WState_Visible))
        return;
    if (w < 0)
        w = data->crect.width()  - x;
    if (h < 0)
        h = data->crect.height() - y;
    if (w != 0 && h != 0) {
        QPainter p(this);
        p.eraseRect(QRect(x, y, w, h));
    }
}

/*!
    \overload

    Clear the given region, \a rgn.

    Drawing may only take place in a QPaintEvent. Overload
    paintEvent() to do your erasing and call update() to schedule a
    replaint whenever necessary. See also QPainter.
*/
void QWidget::erase(const QRegion& rgn)
{
    if (testAttribute(Qt::WA_NoSystemBackground) || testAttribute(Qt::WA_UpdatesDisabled) || !testAttribute(Qt::WA_WState_Visible))
        return;

    QPainter p(this);
    p.setClipRegion(rgn);
    p.eraseRect(rgn.boundingRect());
}

void QWidget::drawText_helper(int x, int y, const QString &str)
{
    if(!testAttribute(Qt::WA_WState_Visible))
        return;
    QPainter paint(this);
    paint.drawText(x, y, str);
}


/*!
    Closes the widget.

    Use the no-argument overload instead.
*/
bool QWidget::close(bool alsoDelete)
{
    QPointer<QWidget> that = this;
    bool accepted = close();
    if (alsoDelete && accepted && that)
        deleteLater();
    return accepted;
}

void QWidget::setIcon(const QPixmap &i)
{
    setWindowIcon(i);
}

/*!
    Return's the widget's icon.

    Use windowIcon() instead.
*/
const QPixmap *QWidget::icon() const
{
    Q_D(const QWidget);
    return (d->extra && d->extra->topextra) ? d->extra->topextra->iconPixmap : 0;
}

#endif // QT3_SUPPORT

/*!
    Sets the attribute \a attribute on this widget if \a on is true;
    otherwise clears the attribute.

    \sa testAttribute()
*/
void QWidget::setAttribute(Qt::WidgetAttribute attribute, bool on)
{
    if (testAttribute(attribute) == on)
        return;

    Q_D(QWidget);
    Q_ASSERT_X(sizeof(d->high_attributes)*8 >= (Qt::WA_AttributeCount - sizeof(uint)*8),
               "QWidget::setAttribute(WidgetAttribute, bool)",
               "QWidgetPrivate::high_attributes[] too small to contain all attributes in WidgetAttribute");
    if (attribute < int(8*sizeof(uint))) {
        if (on)
            data->widget_attributes |= (1<<attribute);
        else
            data->widget_attributes &= ~(1<<attribute);
    } else {
        const int x = attribute - 8*sizeof(uint);
        const int int_off = x / (8*sizeof(uint));
        if (on)
            d->high_attributes[int_off] |= (1<<(x-(int_off*8*sizeof(uint))));
        else
            d->high_attributes[int_off] &= ~(1<<(x-(int_off*8*sizeof(uint))));
    }

    switch (attribute) {

#ifndef QT_NO_DRAGANDDROP
    case Qt::WA_AcceptDrops:  {
        if (on && !testAttribute(Qt::WA_DropSiteRegistered))
            setAttribute(Qt::WA_DropSiteRegistered, true);
        else if (!on && (isWindow() || !parentWidget() || !parentWidget()->testAttribute(Qt::WA_DropSiteRegistered)))
            setAttribute(Qt::WA_DropSiteRegistered, false);
        QEvent e(QEvent::AcceptDropsChange);
        QApplication::sendEvent(this, &e);
        break;
    }
    case Qt::WA_DropSiteRegistered:  {
        d->registerDropSite(on);
        for (int i = 0; i < d->children.size(); ++i) {
            QWidget *w = qobject_cast<QWidget *>(d->children.at(i));
            if (w && !w->isWindow() && !w->testAttribute(Qt::WA_AcceptDrops) && w->testAttribute(Qt::WA_DropSiteRegistered) != on)
                w->setAttribute(Qt::WA_DropSiteRegistered, on);
        }
        break;
    }
#endif

    case Qt::WA_NoChildEventsForParent:
        d->sendChildEvents = !on;
        break;
    case Qt::WA_NoChildEventsFromChildren:
        d->receiveChildEvents = !on;
        break;
    case Qt::WA_MacBrushedMetal:
#ifdef Q_WS_MAC
        d->setStyle_helper(style(), false, true);  // Make sure things get unpolished/polished correctly.
        // fall through since changing the metal attribute affects the opaque size grip.
    case Qt::WA_MacOpaqueSizeGrip:
        d->macUpdateOpaqueSizeGrip();
        break;
    case Qt::WA_MacShowFocusRect:
        if (hasFocus()) {
            clearFocus();
            setFocus();
        }
        break;
#endif
    case Qt::WA_MacAlwaysShowToolWindow:
#ifdef Q_WS_MAC
        d->macUpdateHideOnSuspend();
#endif
        break;
    case Qt::WA_MacNormalSize:
    case Qt::WA_MacSmallSize:
    case Qt::WA_MacMiniSize:
#ifdef Q_WS_MAC
        d->macUpdateSizeAttribute();
#endif
        break;
    case Qt::WA_ShowModal:
        if (!on) {
            if (isVisible())
                QApplicationPrivate::leaveModal(this);
            // reset modality type to Modeless when clearing WA_ShowModal
            data->window_modality = Qt::NonModal;
        } else if (data->window_modality == Qt::NonModal) {
            // determine the modality type if it hasn't been set prior
            // to setting WA_ShowModal. set the default to WindowModal
            // if we are the child of a group leader; otherwise use
            // ApplicationModal.
            QWidget *w = parentWidget();
            if (w)
                w = w->window();
            while (w && !w->testAttribute(Qt::WA_GroupLeader)) {
                w = w->parentWidget();
                if (w)
                    w = w->window();
            }
            data->window_modality = (w && w->testAttribute(Qt::WA_GroupLeader))
                                    ? Qt::WindowModal
                                    : Qt::ApplicationModal;
        }
        if (testAttribute(Qt::WA_WState_Created)) {
            // don't call setModal_sys() before create_sys()
            d->setModal_sys();
        }
        break;
    case Qt::WA_MouseTracking: {
        QEvent e(QEvent::MouseTrackingChange);
        QApplication::sendEvent(this, &e);
        break; }
#if !defined(QT_NO_DIRECT3D) && defined(Q_WS_WIN)
    case Qt::WA_MSWindowsUseDirect3D:
        if (!qApp->testAttribute(Qt::AA_MSWindowsUseDirect3DByDefault)) {
            if (on) {
                if (!d->extra)
                    d->createExtra();
                d->extra->had_auto_fill_bg = d->extra->autoFillBackground;
                d->extra->had_no_system_bg = testAttribute(Qt::WA_NoSystemBackground);
                d->extra->had_paint_on_screen = testAttribute(Qt::WA_PaintOnScreen);
                // enforce the opaque widget state D3D needs
                d->extra->autoFillBackground = true;
                setAttribute(Qt::WA_PaintOnScreen);
                setAttribute(Qt::WA_NoSystemBackground);
            } else if (d->extra) {
                d->extra->autoFillBackground = d->extra->had_auto_fill_bg;
                setAttribute(Qt::WA_PaintOnScreen, d->extra->had_paint_on_screen);
                setAttribute(Qt::WA_NoSystemBackground, d->extra->had_no_system_bg);
            }
        }
        break;
#endif
    case Qt::WA_NativeWindow:
        if (on && !internalWinId() && testAttribute(Qt::WA_WState_Created))
            d->createWinId();
        break;
    case Qt::WA_PaintOnScreen:
#if defined(Q_WS_WIN) || defined(Q_WS_X11)
        // Recreate the widget if it's already created as an alien widget and
        // WA_PaintOnScreen is enabled. Paint on screen widgets must have win id.
        if (on && !testAttribute(Qt::WA_NativeWindow))
            setAttribute(Qt::WA_NativeWindow);
#endif
        // fall through
    case Qt::WA_OpaquePaintEvent:
        d->updateIsOpaque();
        break;
    case Qt::WA_NoSystemBackground:
        d->updateIsOpaque();
        // fall through...
    case Qt::WA_UpdatesDisabled:
        d->updateSystemBackground();
        break;
    case Qt::WA_TransparentForMouseEvents:
#ifdef Q_WS_MAC
        d->macUpdateIgnoreMouseEvents();
#endif
        break;
#if defined (Q_WS_WIN)
    case Qt::WA_InputMethodEnabled:
        if (hasFocus())
            QWinInputContext::updateImeStatus(this, true);
        break;
#endif
    case Qt::WA_WindowPropagation:
        d->resolvePalette();
        d->resolveFont();
        d->resolveLocale();
        break;
#ifdef Q_WS_X11
    case Qt::WA_NoX11EventCompression:
        if (!d->extra)
            d->createExtra();
        d->extra->compress_events = on;
        break;
#endif
    case Qt::WA_DontShowOnScreen: {
        if (on && isVisible())
            d->hide_sys();
        break;
    }

#ifdef Q_WS_X11
    case Qt::WA_X11NetWmWindowTypeDesktop:
    case Qt::WA_X11NetWmWindowTypeDock:
    case Qt::WA_X11NetWmWindowTypeToolBar:
    case Qt::WA_X11NetWmWindowTypeMenu:
    case Qt::WA_X11NetWmWindowTypeUtility:
    case Qt::WA_X11NetWmWindowTypeSplash:
    case Qt::WA_X11NetWmWindowTypeDialog:
    case Qt::WA_X11NetWmWindowTypeDropDownMenu:
    case Qt::WA_X11NetWmWindowTypePopupMenu:
    case Qt::WA_X11NetWmWindowTypeToolTip:
    case Qt::WA_X11NetWmWindowTypeNotification:
    case Qt::WA_X11NetWmWindowTypeCombo:
    case Qt::WA_X11NetWmWindowTypeDND:
        if (testAttribute(Qt::WA_WState_Created))
            d->setNetWmWindowTypes();
        break;
#endif

    default:
        break;
    }
}

/*! \fn bool QWidget::testAttribute(Qt::WidgetAttribute attribute) const

  Returns true if attribute \a attribute is set on this widget;
  otherwise returns false.

  \sa setAttribute()
 */
bool QWidget::testAttribute_helper(Qt::WidgetAttribute attribute) const
{
    Q_D(const QWidget);
    const int x = attribute - 8*sizeof(uint);
    const int int_off = x / (8*sizeof(uint));
    return (d->high_attributes[int_off] & (1<<(x-(int_off*8*sizeof(uint)))));
}

/*!
  \property QWidget::windowOpacity

  \brief The level of opacity for the window.

  The valid range of opacity is from 1.0 (completely opaque) to
  0.0 (completely transparent).

  By default the value of this property is 1.0.

  This feature is available on Embedded Linux, Mac OS X, X11 platforms that
  support the Composite extension, and Windows 2000 and later.

  This feature is not available on Windows CE.

  Note that under X11 you need to have a composite manager running,
  and the X11 specific _NET_WM_WINDOW_OPACITY atom needs to be
  supported by the window manager you are using.

  \warning Changing this property from opaque to transparent might issue a
  paint event that needs to be processed before the window is displayed
  correctly. This affects mainly the use of QPixmap::grabWindow(). Also note
  that semi-transparent windows update and resize significantly slower than
  opaque windows.

  \sa setMask()
*/
qreal QWidget::windowOpacity() const
{
    Q_D(const QWidget);
    return (isWindow() && d->maybeTopData()) ? d->maybeTopData()->opacity / 255. : 1.0;
}

void QWidget::setWindowOpacity(qreal opacity)
{
    Q_D(QWidget);
    if (!isWindow())
        return;

    opacity = qBound(qreal(0.0), opacity, qreal(1.0));
    QTLWExtra *extra = d->topData();
    extra->opacity = uint(opacity * 255);

#ifndef Q_WS_QWS
    if (!testAttribute(Qt::WA_WState_Created))
        return;
#endif

#ifndef QT_NO_GRAPHICSVIEW
    if (QGraphicsProxyWidget *proxy = extra->proxyWidget) {
        // Avoid invalidating the cache if set.
        if (proxy->cacheMode() == QGraphicsItem::NoCache)
            proxy->update();
        else if (QGraphicsScene *scene = proxy->scene())
            scene->update(proxy->sceneBoundingRect());
        return;
    }
#endif

    d->setWindowOpacity_sys(opacity);
}

/*!
    \property QWidget::windowModified
    \brief whether the document shown in the window has unsaved changes

    A modified window is a window whose content has changed but has
    not been saved to disk. This flag will have different effects
    varied by the platform. On Mac OS X the close button will have a
    modified look; on other platforms, the window title will have an
    '*' (asterisk).

    The window title must contain a "[*]" placeholder, which
    indicates where the '*' should appear. Normally, it should appear
    right after the file name (e.g., "document1.txt[*] - Text
    Editor"). If the window isn't modified, the placeholder is simply
    removed.

    \sa windowTitle, {Application Example}, {SDI Example}, {MDI Example}
*/
bool QWidget::isWindowModified() const
{
    return testAttribute(Qt::WA_WindowModified);
}

void QWidget::setWindowModified(bool mod)
{
    Q_D(QWidget);
    setAttribute(Qt::WA_WindowModified, mod);

#ifndef Q_WS_MAC
    if (!windowTitle().contains(QLatin1String("[*]")) && mod)
        qWarning("QWidget::setWindowModified: The window title does not contain a '[*]' placeholder");
#endif
    d->setWindowTitle_helper(windowTitle());
    d->setWindowIconText_helper(windowIconText());
#ifdef Q_WS_MAC
    d->setWindowModified_sys(mod);
#endif

    QEvent e(QEvent::ModifiedChange);
    QApplication::sendEvent(this, &e);
}

#ifndef QT_NO_TOOLTIP
/*!
  \property QWidget::toolTip

  \brief the widget's tooltip

  Note that by default tooltips are only shown for widgets that are
  children of the active window. You can change this behavior by
  setting the attribute Qt::WA_AlwaysShowToolTips on the \e window,
  not on the widget with the tooltip.

  If you want to control a tooltip's behavior, you can intercept the
  event() function and catch the QEvent::ToolTip event (e.g., if you
  want to customize the area for which the tooltip should be shown).

  \sa QToolTip statusTip whatsThis
*/
void QWidget::setToolTip(const QString &s)
{
    Q_D(QWidget);
    d->toolTip = s;

    QEvent event(QEvent::ToolTipChange);
    QApplication::sendEvent(this, &event);
}

QString QWidget::toolTip() const
{
    Q_D(const QWidget);
    return d->toolTip;
}
#endif // QT_NO_TOOLTIP


#ifndef QT_NO_STATUSTIP
/*!
  \property QWidget::statusTip

  \brief the widget's status tip

  \sa toolTip whatsThis
*/
void QWidget::setStatusTip(const QString &s)
{
    Q_D(QWidget);
    d->statusTip = s;
}

QString QWidget::statusTip() const
{
    Q_D(const QWidget);
    return d->statusTip;
}
#endif // QT_NO_STATUSTIP

#ifndef QT_NO_WHATSTHIS
/*!
  \property QWidget::whatsThis

  \brief the widget's What's This help text.

  \sa QWhatsThis QWidget::toolTip QWidget::statusTip
*/
void QWidget::setWhatsThis(const QString &s)
{
    Q_D(QWidget);
    d->whatsThis = s;
}

QString QWidget::whatsThis() const
{
    Q_D(const QWidget);
    return d->whatsThis;
}
#endif // QT_NO_WHATSTHIS

#ifndef QT_NO_ACCESSIBILITY
/*!
  \property QWidget::accessibleName

  \brief the widget's name as seen by assistive technologies

    It is be used by accessible clients to identify, find, or announce
    the widget for accessible clients.

  \sa QAccessibleInterface::text()
*/
void QWidget::setAccessibleName(const QString &name)
{
    Q_D(QWidget);
    d->accessibleName = name;
}

QString QWidget::accessibleName() const
{
    Q_D(const QWidget);
    return d->accessibleName;
}

/*!
  \property QWidget::accessibleDescription

  \brief the widget's description as seen by assistive technologies

  \sa QAccessibleInterface::text()
*/
void QWidget::setAccessibleDescription(const QString &description)
{
    Q_D(QWidget);
    d->accessibleDescription = description;
}

QString QWidget::accessibleDescription() const
{
    Q_D(const QWidget);
    return d->accessibleDescription;
}
#endif // QT_NO_ACCESSIBILITY

#ifndef QT_NO_SHORTCUT
/*!
    Adds a shortcut to Qt's shortcut system that watches for the given
    \a key sequence in the given \a context. If the \a context is
    Qt::ApplicationShortcut, the shortcut applies to the application as a
    whole. Otherwise, it is either local to this widget, Qt::WidgetShortcut,
    or to the window itself, Qt::WindowShortcut.

    If the same \a key sequence has been grabbed by several widgets,
    when the \a key sequence occurs a QEvent::Shortcut event is sent
    to all the widgets to which it applies in a non-deterministic
    order, but with the ``ambiguous'' flag set to true.

    \warning You should not normally need to use this function;
    instead create \l{QAction}s with the shortcut key sequences you
    require (if you also want equivalent menu options and toolbar
    buttons), or create \l{QShortcut}s if you just need key sequences.
    Both QAction and QShortcut handle all the event filtering for you,
    and provide signals which are triggered when the user triggers the
    key sequence, so are much easier to use than this low-level
    function.

    \sa releaseShortcut() setShortcutEnabled()
*/
int QWidget::grabShortcut(const QKeySequence &key, Qt::ShortcutContext context)
{
    Q_ASSERT(qApp);
    if (key.isEmpty())
        return 0;
    setAttribute(Qt::WA_GrabbedShortcut);
    return qApp->d_func()->shortcutMap.addShortcut(this, key, context);
}

/*!
    Removes the shortcut with the given \a id from Qt's shortcut
    system. The widget will no longer receive QEvent::Shortcut events
    for the shortcut's key sequence (unless it has other shortcuts
    with the same key sequence).

    \warning You should not normally need to use this function since
    Qt's shortcut system removes shortcuts automatically when their
    parent widget is destroyed. It is best to use QAction or
    QShortcut to handle shortcuts, since they are easier to use than
    this low-level function. Note also that this is an expensive
    operation.

    \sa grabShortcut() setShortcutEnabled()
*/
void QWidget::releaseShortcut(int id)
{
    Q_ASSERT(qApp);
    if (id)
        qApp->d_func()->shortcutMap.removeShortcut(id, this, 0);
}

/*!
    If \a enable is true, the shortcut with the given \a id is
    enabled; otherwise the shortcut is disabled.

    \warning You should not normally need to use this function since
    Qt's shortcut system enables/disables shortcuts automatically as
    widgets become hidden/visible and gain or lose focus. It is best
    to use QAction or QShortcut to handle shortcuts, since they are
    easier to use than this low-level function.

    \sa grabShortcut() releaseShortcut()
*/
void QWidget::setShortcutEnabled(int id, bool enable)
{
    Q_ASSERT(qApp);
    if (id)
        qApp->d_func()->shortcutMap.setShortcutEnabled(enable, id, this, 0);
}

/*!
    \since 4.2

    If \a enable is true, auto repeat of the shortcut with the
    given \a id is enabled; otherwise it is disabled.

    \sa grabShortcut() releaseShortcut()
*/
void QWidget::setShortcutAutoRepeat(int id, bool enable)
{
    Q_ASSERT(qApp);
    if (id)
        qApp->d_func()->shortcutMap.setShortcutAutoRepeat(enable, id, this, 0);
}
#endif // QT_NO_SHORTCUT
/*!
    Updates the widget's micro focus.

    \sa QInputContext
*/
void QWidget::updateMicroFocus()
{
#if !defined(QT_NO_IM) && (defined(Q_WS_X11) || defined(Q_WS_QWS))
    QInputContext *ic = inputContext();
    if (ic)
        ic->update();
#endif
#ifndef QT_NO_ACCESSIBILITY
    // ##### is this correct
    QAccessible::updateAccessibility(this, 0, QAccessible::StateChanged);
#endif
}


#if defined (Q_WS_WIN)
/*!
    Returns the window system handle of the widget, for low-level
    access. Using this function is not portable.

    An HDC acquired with getDC() has to be released with releaseDC().

    \warning Using this function is not portable.
*/
HDC QWidget::getDC() const
{
    Q_D(const QWidget);
    if (d->hd)
        return (HDC) d->hd;
    return GetDC(winId());
}

/*!
    Releases the HDC \a hdc acquired by a previous call to getDC().

    \warning Using this function is not portable.
*/
void QWidget::releaseDC(HDC hdc) const
{
    Q_D(const QWidget);
    // If its the widgets own dc, it will be released elsewhere. If
    // its a different HDC we release it and issue a warning if it
    // fails.
    if (hdc != d->hd && !ReleaseDC(winId(), hdc))
        qErrnoWarning("QWidget::releaseDC(): failed to release HDC");
}
#else
/*!
    Returns the window system handle of the widget, for low-level
    access. Using this function is not portable.

    The HANDLE type varies with platform; see \c qwindowdefs.h for
    details.
*/
Qt::HANDLE QWidget::handle() const
{
    Q_D(const QWidget);
    if (!internalWinId() && testAttribute(Qt::WA_WState_Created))
        (void)winId(); // enforce native window
    return d->hd;
}
#endif


/*!
    Raises this widget to the top of the parent widget's stack.

    After this call the widget will be visually in front of any
    overlapping sibling widgets.

    \note When using activateWindow(), you can call this function to
    ensure that the window is stacked on top.

    \sa lower(), stackUnder()
*/

void QWidget::raise()
{
    Q_D(QWidget);
    if (!isWindow()) {
        QWidget *p = parentWidget();
        const int parentChildCount = p->d_func()->children.size();
        if (parentChildCount < 2)
            return;
        const int from = p->d_func()->children.indexOf(this);
        Q_ASSERT(from >= 0);
        // Do nothing if the widget is already in correct stacking order _and_ created.
        if (from != parentChildCount -1)
            p->d_func()->children.move(from, parentChildCount - 1);
        if (!testAttribute(Qt::WA_WState_Created) && p->testAttribute(Qt::WA_WState_Created))
            create();
        else if (from == parentChildCount - 1)
            return;
    }
    if (testAttribute(Qt::WA_WState_Created))
        d->raise_sys();

    QEvent e(QEvent::ZOrderChange);
    QApplication::sendEvent(this, &e);
}

/*!
    Lowers the widget to the bottom of the parent widget's stack.

    After this call the widget will be visually behind (and therefore
    obscured by) any overlapping sibling widgets.

    \sa raise(), stackUnder()
*/

void QWidget::lower()
{
    Q_D(QWidget);
    if (!isWindow()) {
        QWidget *p = parentWidget();
        const int parentChildCount = p->d_func()->children.size();
        if (parentChildCount < 2)
            return;
        const int from = p->d_func()->children.indexOf(this);
        Q_ASSERT(from >= 0);
        // Do nothing if the widget is already in correct stacking order _and_ created.
        if (from != 0)
            p->d_func()->children.move(from, 0);
        if (!testAttribute(Qt::WA_WState_Created) && p->testAttribute(Qt::WA_WState_Created))
            create();
        else if (from == 0)
            return;
    }
    if (testAttribute(Qt::WA_WState_Created))
        d->lower_sys();

    QEvent e(QEvent::ZOrderChange);
    QApplication::sendEvent(this, &e);
}


/*!
    Places the widget under \a w in the parent widget's stack.

    To make this work, the widget itself and \a w must be siblings.

    \sa raise(), lower()
*/
void QWidget::stackUnder(QWidget* w)
{
    Q_D(QWidget);
    QWidget *p = parentWidget();
    if (!w || isWindow() || p != w->parentWidget() || this == w)
        return;
    if (p) {
        int from = p->d_func()->children.indexOf(this);
        int to = p->d_func()->children.indexOf(w);
        Q_ASSERT(from >= 0);
        Q_ASSERT(to >= 0);
        if (from < to)
            --to;
        // Do nothing if the widget is already in correct stacking order _and_ created.
        if (from != to)
            p->d_func()->children.move(from, to);
        if (!testAttribute(Qt::WA_WState_Created) && p->testAttribute(Qt::WA_WState_Created))
            create();
        else if (from == to)
            return;
    }
    if (testAttribute(Qt::WA_WState_Created))
        d->stackUnder_sys(w);

    QEvent e(QEvent::ZOrderChange);
    QApplication::sendEvent(this, &e);
}

void QWidget::styleChange(QStyle&) { }
void QWidget::enabledChange(bool) { }  // compat
void QWidget::paletteChange(const QPalette &) { }  // compat
void QWidget::fontChange(const QFont &) { }  // compat
void QWidget::windowActivationChange(bool) { }  // compat
void QWidget::languageChange() { }  // compat


/*!
    \enum QWidget::BackgroundOrigin

    \compat

    \value WidgetOrigin
    \value ParentOrigin
    \value WindowOrigin
    \value AncestorOrigin

*/

/*!
    \fn bool QWidget::isVisibleToTLW() const

    Use isVisible() instead.
*/

/*!
    \fn void QWidget::iconify()

    Use showMinimized() instead.
*/

/*!
    \fn void QWidget::constPolish() const

    Use ensurePolished() instead.
*/

/*!
    \fn void QWidget::reparent(QWidget *parent, Qt::WindowFlags f, const QPoint &p, bool showIt)

    Use setParent() to change the parent or the widget's widget flags;
    use move() to move the widget, and use show() to show the widget.
*/

/*!
    \fn void QWidget::reparent(QWidget *parent, const QPoint &p, bool showIt)

    Use setParent() to change the parent; use move() to move the
    widget, and use show() to show the widget.
*/

/*!
    \fn void QWidget::recreate(QWidget *parent, Qt::WindowFlags f, const QPoint & p, bool showIt)

    Use setParent() to change the parent or the widget's widget flags;
    use move() to move the widget, and use show() to show the widget.
*/

/*!
    \fn bool QWidget::hasMouse() const

    Use testAttribute(Qt::WA_UnderMouse) instead.
*/

/*!
    \fn bool QWidget::ownCursor() const

    Use testAttribute(Qt::WA_SetCursor) instead.
*/

/*!
    \fn bool QWidget::ownFont() const

    Use testAttribute(Qt::WA_SetFont) instead.
*/

/*!
    \fn void QWidget::unsetFont()

    Use setFont(QFont()) instead.
*/

/*!
    \fn bool QWidget::ownPalette() const

    Use testAttribute(Qt::WA_SetPalette) instead.
*/

/*!
    \fn void QWidget::unsetPalette()

    Use setPalette(QPalette()) instead.
*/

/*!
    \fn void QWidget::setEraseColor(const QColor &color)

    Use the palette instead.

    \oldcode
    widget->setEraseColor(color);
    \newcode
    QPalette palette;
    palette.setColor(widget->backgroundRole(), color);
    widget->setPalette(palette);
    \endcode
*/

/*!
    \fn void QWidget::setErasePixmap(const QPixmap &pixmap)

    Use the palette instead.

    \oldcode
    widget->setErasePixmap(pixmap);
    \newcode
    QPalette palette;
    palette.setBrush(widget->backgroundRole(), QBrush(pixmap));
    widget->setPalette(palette);
    \endcode
*/

/*!
    \fn void QWidget::setPaletteForegroundColor(const QColor &color)

    Use the palette directly.

    \oldcode
    widget->setPaletteForegroundColor(color);
    \newcode
    QPalette palette;
    palette.setColor(widget->foregroundRole(), color);
    widget->setPalette(palette);
    \endcode
*/

/*!
    \fn void QWidget::setPaletteBackgroundColor(const QColor &color)

    Use the palette directly.

    \oldcode
    widget->setPaletteBackgroundColor(color);
    \newcode
    QPalette palette;
    palette.setColor(widget->backgroundRole(), color);
    widget->setPalette(palette);
    \endcode
*/

/*!
    \fn void QWidget::setPaletteBackgroundPixmap(const QPixmap &pixmap)

    Use the palette directly.

    \oldcode
    widget->setPaletteBackgroundPixmap(pixmap);
    \newcode
    QPalette palette;
    palette.setBrush(widget->backgroundRole(), QBrush(pixmap));
    widget->setPalette(palette);
    \endcode
*/

/*!
    \fn void QWidget::setBackgroundPixmap(const QPixmap &pixmap)

    Use the palette instead.

    \oldcode
    widget->setBackgroundPixmap(pixmap);
    \newcode
    QPalette palette;
    palette.setBrush(widget->backgroundRole(), QBrush(pixmap));
    widget->setPalette(palette);
    \endcode
*/

/*!
    \fn void QWidget::setBackgroundColor(const QColor &color)

    Use the palette instead.

    \oldcode
    widget->setBackgroundColor(color);
    \newcode
    QPalette palette;
    palette.setColor(widget->backgroundRole(), color);
    widget->setPalette(palette);
    \endcode
*/

/*!
    \fn QColorGroup QWidget::colorGroup() const

    Use QColorGroup(palette()) instead.
*/

/*!
    \fn QWidget *QWidget::parentWidget(bool sameWindow) const

    Use the no-argument overload instead.
*/

/*!
    \fn void QWidget::setKeyCompression(bool b)

    Use setAttribute(Qt::WA_KeyCompression, b) instead.
*/

/*!
    \fn void QWidget::setFont(const QFont &f, bool b)

    Use the single-argument overload instead.
*/

/*!
    \fn void QWidget::setPalette(const QPalette &p, bool b)

    Use the single-argument overload instead.
*/

/*!
    \fn void QWidget::setBackgroundOrigin(BackgroundOrigin background)

    \obsolete
*/

/*!
    \fn BackgroundOrigin QWidget::backgroundOrigin() const

    \obsolete

    Always returns \c WindowOrigin.
*/

/*!
    \fn QPoint QWidget::backgroundOffset() const

    \obsolete

    Always returns QPoint().
*/

/*!
    \fn void QWidget::repaint(bool b)

    The boolean parameter \a b is ignored. Use the no-argument overload instead.
*/

/*!
    \fn void QWidget::repaint(int x, int y, int w, int h, bool b)

    The boolean parameter \a b is ignored. Use the four-argument overload instead.
*/

/*!
    \fn void QWidget::repaint(const QRect &r, bool b)

    The boolean parameter \a b is ignored. Use the single rect-argument overload instead.
*/

/*!
    \fn void QWidget::repaint(const QRegion &rgn, bool b)

    The boolean parameter \a b is ignored. Use the single region-argument overload instead.
*/

/*!
    \fn void QWidget::erase()

    Drawing may only take place in a QPaintEvent. Overload
    paintEvent() to do your erasing and call update() to schedule a
    replaint whenever necessary. See also QPainter.
*/

/*!
    \fn void QWidget::erase(int x, int y, int w, int h)

    Drawing may only take place in a QPaintEvent. Overload
    paintEvent() to do your erasing and call update() to schedule a
    replaint whenever necessary. See also QPainter.
*/

/*!
    \fn void QWidget::erase(const QRect &rect)

    Drawing may only take place in a QPaintEvent. Overload
    paintEvent() to do your erasing and call update() to schedule a
    replaint whenever necessary. See also QPainter.
*/

/*!
    \fn void QWidget::drawText(const QPoint &p, const QString &s)

    Drawing may only take place in a QPaintEvent. Overload
    paintEvent() to do your drawing and call update() to schedule a
    replaint whenever necessary. See also QPainter.
*/

/*!
    \fn void QWidget::drawText(int x, int y, const QString &s)

    Drawing may only take place in a QPaintEvent. Overload
    paintEvent() to do your drawing and call update() to schedule a
    replaint whenever necessary. See also QPainter.
*/

/*!
    \fn QWidget *QWidget::childAt(const QPoint &p, bool includeThis) const

    Use the single point argument overload instead.
*/

/*!
    \fn void QWidget::setCaption(const QString &c)

    Use setWindowTitle() instead.
*/

/*!
    \fn void QWidget::setIcon(const QPixmap &i)

    Use setWindowIcon() instead.
*/

/*!
    \fn void QWidget::setIconText(const QString &it)

    Use setWindowIconText() instead.
*/

/*!
    \fn QString QWidget::caption() const

    Use windowTitle() instead.
*/

/*!
    \fn QString QWidget::iconText() const

    Use windowIconText() instead.
*/

/*!
    \fn bool QWidget::isTopLevel() const
    \obsolete

    Use isWindow() instead.
*/

/*!
    \fn bool QWidget::isRightToLeft() const
    \internal
*/

/*!
    \fn bool QWidget::isLeftToRight() const
    \internal
*/

/*!
    \fn void QWidget::setInputMethodEnabled(bool enabled)

    Use setAttribute(Qt::WA_InputMethodEnabled, \a enabled) instead.
*/

/*!
    \fn bool QWidget::isInputMethodEnabled() const

    Use testAttribute(Qt::WA_InputMethodEnabled) instead.
*/

/*!
    \fn void QWidget::setActiveWindow()

    Use activateWindow() instead.
*/

/*!
    \fn bool QWidget::isShown() const

    Use !isHidden() instead (notice the exclamation mark), or use isVisible() to check whether the widget is visible.
*/

/*!
    \fn bool QWidget::isDialog() const

    Use windowType() == Qt::Dialog instead.
*/

/*!
    \fn bool QWidget::isPopup() const

    Use windowType() == Qt::Popup instead.
*/

/*!
    \fn bool QWidget::isDesktop() const

    Use windowType() == Qt::Desktop instead.
*/

/*!
    \fn void QWidget::polish()

    Use ensurePolished() instead.
*/

/*!
    \fn QWidget *QWidget::childAt(int x, int y, bool includeThis) const

    Use the childAt() overload that doesn't have an \a includeThis parameter.

    \oldcode
        return widget->childAt(x, y, true);
    \newcode
        QWidget *child = widget->childAt(x, y, true);
        if (child)
            return child;
        if (widget->rect().contains(x, y))
            return widget;
    \endcode
*/

/*!
    \fn void QWidget::setSizePolicy(QSizePolicy::Policy hor, QSizePolicy::Policy ver, bool hfw)
    \compat

    Use the \l sizePolicy property and heightForWidth() function instead.
*/

/*!
    \fn bool QWidget::isUpdatesEnabled() const
    \compat

    Use the \l updatesEnabled property instead.
*/

/*!
     \macro QWIDGETSIZE_MAX
     \relates QWidget

     Defines the maximum size for a QWidget object.

     The largest allowed size for a widget is QSize(QWIDGETSIZE_MAX,
     QWIDGETSIZE_MAX), i.e. QSize (16777215,16777215).

     \sa QWidget::setMaximumSize()
*/

QRect QWidgetPrivate::frameStrut() const
{
    Q_Q(const QWidget);
    if (q->testAttribute(Qt::WA_DontShowOnScreen))
        return QRect();
    if (!q->isWindow() || (q->windowType() == Qt::Desktop)) {
        // x2 = x1 + w - 1, so w/h = 1
        return QRect(0, 0, 1, 1);
    }

    if (data.fstrut_dirty
#ifndef Q_WS_WIN
        // ### Fix properly for 4.3
        && q->isVisible()
#endif
        && q->testAttribute(Qt::WA_WState_Created))
        const_cast<QWidgetPrivate *>(this)->updateFrameStrut();

    return maybeTopData() ? maybeTopData()->frameStrut : QRect();
}

/*!
    \preliminary
    \since 4.2
    \obsolete

    Sets the window surface to be the \a surface specified.
    The QWidget takes will ownership of the \a surface.
    widget itself is deleted.
*/
void QWidget::setWindowSurface(QWindowSurface *surface)
{
    // ### createWinId() ??

#ifndef Q_BACKINGSTORE_SUBSURFACES
    if (!isTopLevel())
        return;
#endif

    Q_D(QWidget);

    QTLWExtra *topData = d->topData();
    if (topData->windowSurface == surface)
        return;

#ifndef Q_WS_MAC
    const QWindowSurface *oldSurface = topData->windowSurface;
#endif
    delete topData->windowSurface;
    topData->windowSurface = surface;

#ifndef Q_WS_MAC
    QWidgetBackingStore *bs = d->maybeBackingStore();
    if (!bs)
        return;

    if (isTopLevel()) {
        if (bs->windowSurface != oldSurface && bs->windowSurface != surface)
            delete bs->windowSurface;
        bs->windowSurface = surface;
    }
#ifdef Q_BACKINGSTORE_SUBSURFACES
    else {
        bs->subSurfaces.append(surface);
    }
#endif
#endif // Q_WS_MAC
}

/*!
    \preliminary
    \since 4.2

    Returns the QWindowSurface this widget will be drawn into.
*/
QWindowSurface *QWidget::windowSurface() const
{
    Q_D(const QWidget);
    QTLWExtra *extra = d->maybeTopData();
    if (extra && extra->windowSurface)
        return extra->windowSurface;

#ifdef Q_WS_MAC
    return 0;
#else
    QWidgetBackingStore *bs = d->maybeBackingStore();

#ifdef Q_BACKINGSTORE_SUBSURFACES
    if (bs && bs->subSurfaces.isEmpty())
        return bs->windowSurface;

    if (!isTopLevel()) {
        const QWidget *w = parentWidget();
        while (w) {
            QTLWExtra *extra = w->d_func()->maybeTopData();
            if (extra && extra->windowSurface)
                return extra->windowSurface;
            if (w->isTopLevel())
                break;
            w = w->parentWidget();
        }
    }
#endif // Q_BACKINGSTORE_SUBSURFACES

    return bs ? bs->windowSurface : 0;
#endif // Q_WS_MAC
}



QWidgetData *qt_qwidget_data(QWidget *widget)
{
    return widget->data;
}

/*! \internal
*/
QWidgetPrivate * qt_widget_private(QWidget *widget)
{
    return widget->d_func();
}

void QWidgetPrivate::getLayoutItemMargins(int *left, int *top, int *right, int *bottom) const
{
    if (left)
        *left = (int)leftLayoutItemMargin;
    if (top)
        *top = (int)topLayoutItemMargin;
    if (right)
        *right = (int)rightLayoutItemMargin;
    if (bottom)
        *bottom = (int)bottomLayoutItemMargin;
}

void QWidgetPrivate::setLayoutItemMargins(int left, int top, int right, int bottom)
{
    if (leftLayoutItemMargin == left
        && topLayoutItemMargin == top
        && rightLayoutItemMargin == right
        && bottomLayoutItemMargin == bottom)
        return;

    Q_Q(QWidget);
    leftLayoutItemMargin = (signed char)left;
    topLayoutItemMargin = (signed char)top;
    rightLayoutItemMargin = (signed char)right;
    bottomLayoutItemMargin = (signed char)bottom;
    q->updateGeometry();
}

void QWidgetPrivate::setLayoutItemMargins(QStyle::SubElement element, const QStyleOption *opt)
{
    Q_Q(QWidget);
    QStyleOption myOpt;
    if (!opt) {
        myOpt.initFrom(q);
        myOpt.rect.setRect(0, 0, 32768, 32768);     // arbitrary
        opt = &myOpt;
    }

    QRect liRect = q->style()->subElementRect(element, opt, q);
    if (liRect.isValid()) {
        leftLayoutItemMargin = (signed char)(opt->rect.left() - liRect.left());
        topLayoutItemMargin = (signed char)(opt->rect.top() - liRect.top());
        rightLayoutItemMargin = (signed char)(liRect.right() - opt->rect.right());
        bottomLayoutItemMargin = (signed char)(liRect.bottom() - opt->rect.bottom());
    } else {
        leftLayoutItemMargin = 0;
        topLayoutItemMargin = 0;
        rightLayoutItemMargin = 0;
        bottomLayoutItemMargin = 0;
    }
}

/*!
    \typedef QWidgetList
    \relates QWidget

    Synonym for QList<QWidget *>.
*/

QT_END_NAMESPACE

#include "moc_qwidget.cpp"

/*!
    \typedef WId
    \relates QWidget

    Platform dependent window identifier.
*/

