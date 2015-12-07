/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the QtGui module of the Qt Toolkit.
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

#ifndef QWIDGET_P_H
#define QWIDGET_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists for the convenience
// of qapplication_*.cpp, qwidget*.cpp and qfiledialog.cpp.  This header
// file may change from version to version without notice, or even be removed.
//
// We mean it.
//

#include "QtGui/qwidget.h"
#include "private/qobject_p.h"
#include "QtCore/qrect.h"
#include "QtCore/qlocale.h"
#include "QtGui/qregion.h"
#include "QtGui/qsizepolicy.h"
#include "QtGui/qstyle.h"

#ifdef Q_WS_WIN
#include "QtCore/qt_windows.h"
#include <private/qdnd_p.h>
#endif // Q_WS_WIN

#ifdef Q_WS_X11
#include "QtGui/qx11info_x11.h"
#endif

#ifdef Q_WS_MAC
#include <private/qt_mac_p.h>
#endif

#if defined(Q_WS_QWS)
#include "QtGui/qinputcontext.h"
#include "QtGui/qscreen_qws.h"
#endif

QT_BEGIN_NAMESPACE

// Extra QWidget data
//  - to minimize memory usage for members that are seldom used.
//  - top-level widgets have extra extra data to reduce cost further
#if defined(Q_WS_QWS)
class QWSManager;
#endif
#if defined(Q_WS_MAC)
class QCoreGraphicsPaintEnginePrivate;
#endif
class QPaintEngine;
class QPixmap;
class QWidgetBackingStore;
class QGraphicsProxyWidget;
class QWidgetItemV2;

class QStyle;

struct QTLWExtra {
    QString caption; // widget caption
    QString iconText; // widget icon text
    QString role; // widget role
    QString filePath; // widget file path
    QIcon *icon; // widget icon
    QPixmap *iconPixmap;
    short incw, inch; // size increments
     // frame strut, don't use these directly, use QWidgetPrivate::frameStrut() instead.
    QRect frameStrut;
    uint opacity : 8;
    uint posFromMove : 1;
    uint sizeAdjusted : 1;
    uint inTopLevelResize : 1;
    uint inRepaint : 1;
    QWidgetBackingStore *backingStore;
#if defined(Q_WS_WIN)
    ulong savedFlags; // Save window flags while showing fullscreen
    uint embedded : 1; // window is embedded in another application
#else
    Qt::WindowFlags savedFlags; // Save widget flags while showing fullscreen
#endif
    short basew, baseh; // base sizes
#if defined(Q_WS_X11)
    WId parentWinId; // parent window Id (valid after reparenting)
    uint embedded : 1; // window is embedded in another Qt application
    uint spont_unmapped: 1; // window was spontaneously unmapped
    uint dnd : 1; // DND properties installed
    uint validWMState : 1; // is WM_STATE valid?
    uint waitingForMapNotify : 1; // show() has been called, haven't got the MapNotify yet
    WId userTimeWindow; // window id that contains user-time timestamp when WM supports a _NET_WM_USER_TIME_WINDOW atom
    QPoint fullScreenOffset;
#endif
#if defined(Q_WS_MAC)
    quint32 wattr;
    quint32 wclass;
    WindowGroupRef group;
    IconRef windowIcon; // the current window icon, if set with setWindowIcon_sys.
    quint32 savedWindowAttributesFromMaximized; // Saved attributes from when the calling updateMaximizeButton_sys()
    uint resizer : 4;
    uint isSetGeometry : 1;
    uint isMove : 1;
    uint embedded : 1;
#endif
#if defined(Q_WS_QWS) && !defined (QT_NO_QWS_MANAGER)
    QWSManager *qwsManager;
#endif
#if defined(Q_WS_WIN)
    HICON winIconBig; // internal big Windows icon
    HICON winIconSmall; // internal small Windows icon
#endif
    QRect normalGeometry; // used by showMin/maximized/FullScreen
    QWindowSurface *windowSurface;
    QPainter *sharedPainter;
};

struct QWExtra {
    qint32 minw, minh; // minimum size
    qint32 maxw, maxh; // maximum size
    QPointer<QWidget> focus_proxy;
#ifndef QT_NO_CURSOR
    QCursor *curs;
#endif
    QTLWExtra *topextra; // only useful for TLWs
    QGraphicsProxyWidget *proxyWidget; // if the widget is embedded
    void *glContext; // if the widget is hijacked by QGLWindowSurface
#if defined(Q_WS_WIN) && !defined(QT_NO_DRAGANDDROP)
    QOleDropTarget *dropTarget; // drop target
    QList<QPointer<QWidget> > oleDropWidgets;
#endif
#if defined(Q_WS_X11)
    WId xDndProxy; // XDND forwarding to embedded windows
#endif
    QRegion mask; // widget mask
    QSize staticContentsSize;

//bit flags at the end to improve packing
#if defined(Q_WS_WIN)
    uint shown_mode : 8; // widget show mode
#ifndef QT_NO_DIRECT3D
    uint had_paint_on_screen : 1;
    uint had_no_system_bg : 1;
    uint had_auto_fill_bg : 1;
#endif
#endif
#if defined(Q_WS_X11)
    uint compress_events : 1;
#endif
    uint explicitMinSize : 2;
    uint explicitMaxSize : 2;
    uint autoFillBackground : 1;
    uint nativeChildrenForced : 1;
    uint inRenderWithPainter : 1;
    uint hasMask : 1;

    QPointer<QStyle> style;
    QString styleSheet;

    quint16 customDpiX;
    quint16 customDpiY;
#if defined(Q_WS_MAC) && defined(QT_MAC_USE_COCOA)
    // Cocoa Mask stuff
    QImage maskBits;
    CGImageRef imageMask;
#endif
};

class Q_GUI_EXPORT QWidgetPrivate : public QObjectPrivate
{
    Q_DECLARE_PUBLIC(QWidget)

public:
    explicit QWidgetPrivate(int version = QObjectPrivateVersion);
    ~QWidgetPrivate();

    QWExtra *extraData() const;
    QTLWExtra *topData() const;
    QTLWExtra *maybeTopData() const;
    QPainter *sharedPainter() const;
    void setSharedPainter(QPainter *painter);
    QWidgetBackingStore *maybeBackingStore() const;
#ifdef Q_WS_QWS
    void setMaxWindowState_helper();
    void setFullScreenSize_helper();
#endif
    void init(QWidget *desktopWidget, Qt::WindowFlags f);
    void create_sys(WId window, bool initializeWindow, bool destroyOldWindow);
    void createRecursively();
    void createWinId(WId id = 0);

    void createTLExtra();
    void createExtra();
    void deleteExtra();
    void createSysExtra();
    void deleteSysExtra();
    void createTLSysExtra();
    void deleteTLSysExtra();
    void updateSystemBackground();
    void propagatePaletteChange();

    void setPalette_helper(const QPalette &);
    void resolvePalette();
    QPalette naturalWidgetPalette(uint inheritedMask) const;

    void setMask_sys(const QRegion &);
#ifdef Q_WS_WIN
    bool shouldShowMaximizeButton();
    void winUpdateIsOpaque();
#endif

#ifdef Q_WS_MAC
    void macUpdateSizeAttribute();
    void macUpdateHideOnSuspend();
    void macUpdateOpaqueSizeGrip();
    void macUpdateIgnoreMouseEvents();
    void macUpdateMetalAttribute();
    void macUpdateIsOpaque();
    void setEnabled_helper_sys(bool enable);
    bool isRealWindow() const;
#endif

    void raise_sys();
    void lower_sys();
    void stackUnder_sys(QWidget *);

    void setFocus_sys();

    void updateFont(const QFont &);
    inline void setFont_helper(const QFont &font) {
        if (data.fnt == font && data.fnt.resolve() == font.resolve())
            return;
        updateFont(font);
    }
    void resolveFont();
    QFont naturalWidgetFont(uint inheritedMask) const;

    void setLayoutDirection_helper(Qt::LayoutDirection);
    void resolveLayoutDirection();

    void setLocale_helper(const QLocale &l, bool forceUpdate = false);
    void resolveLocale();

    void setStyle_helper(QStyle *newStyle, bool propagate, bool metalHack = false);
    void inheritStyle();

    bool isBackgroundInherited() const;

    void setUpdatesEnabled_helper(bool );

    void paintBackground(QPainter *, const QRegion &, const QPoint & = QPoint(), int flags = DrawAsRoot) const;
    enum DrawWidgetFlags {
        DrawAsRoot = 0x01,
        DrawPaintOnScreen = 0x02,
        DrawRecursive = 0x04,
        DrawInvisible = 0x08,
        DontSubtractOpaqueChildren = 0x10,
        DontSetCompositionMode = 0x20,
        DontDrawOpaqueChildren = 0x40
    };
    bool isAboutToShow() const;
    QRegion prepareToRender(const QRegion &region, QWidget::RenderFlags renderFlags);
    void render_helper(QPainter *painter, const QPoint &targetOffset, const QRegion &sourceRegion,
                       QWidget::RenderFlags renderFlags);
    void drawWidget(QPaintDevice *pdev, const QRegion &rgn, const QPoint &offset, int flags,
                    QPainter *sharedPainter = 0, QWidgetBackingStore *backingStore = 0);


    void paintSiblingsRecursive(QPaintDevice *pdev, const QObjectList& children, int index,
                                const QRegion &rgn, const QPoint &offset, int flags
#ifdef Q_BACKINGSTORE_SUBSURFACES
                                , const QWindowSurface *currentSurface
#endif
                                , QPainter *sharedPainter, QWidgetBackingStore *backingStore);


    QPainter *beginSharedPainter();
    bool endSharedPainter();
    static QGraphicsProxyWidget * nearestGraphicsProxyWidget(QWidget *origin);
    QWindowSurface *createDefaultWindowSurface();
    QWindowSurface *createDefaultWindowSurface_sys();
    void repaint_sys(const QRegion &rgn);
#ifdef Q_WS_MAC
    void update_sys(const QRect &rect);
    void update_sys(const QRegion &rgn);
#endif

    QRect clipRect() const;
    QRegion clipRegion() const;
    void subtractOpaqueChildren(QRegion &rgn, const QRect &clipRect) const;
    void subtractOpaqueSiblings(QRegion &source, bool *hasDirtySiblingsAbove = 0,
                                bool alsoNonOpaque = false) const;
    void clipToEffectiveMask(QRegion &region) const;
    void updateIsOpaque();
    void setOpaque(bool opaque);
    void updateIsTranslucent();
    bool hasBackground() const;
    bool paintOnScreen() const;

    QRegion getOpaqueRegion() const;
    const QRegion &getOpaqueChildren() const;
    void setDirtyOpaqueRegion();

    QRegion opaqueChildren;

    enum CloseMode {
        CloseNoEvent,
        CloseWithEvent,
        CloseWithSpontaneousEvent
    };
    bool close_helper(CloseMode mode);

    bool compositeEvent(QEvent *e);
    void setWindowIcon_helper();
    void setWindowIcon_sys(bool forceReset = false);
    void setWindowOpacity_sys(qreal opacity);

    void focusInputContext();

    void adjustQuitOnCloseAttribute();

#if defined(Q_WS_X11)
    void setWindowRole();
    void sendStartupMessage(const char *message) const;
    void setNetWmWindowTypes();
    void x11UpdateIsOpaque();
#endif

#if defined (Q_WS_WIN)
    void reparentChildren();
#endif

    void scrollChildren(int dx, int dy);

    void moveRect(const QRect &, int dx, int dy);
    void scrollRect(const QRect &, int dx, int dy);
    void invalidateBuffer_resizeHelper(const QPoint &oldPos, const QSize &oldSize);
    // ### Qt 4.6: Merge into a template function (after MSVC isn't supported anymore).
    void invalidateBuffer(const QRegion &);
    void invalidateBuffer(const QRect &);
    bool isOverlapped(const QRect&) const;
    void syncBackingStore();
    void syncBackingStore(const QRegion &region);

    void reparentFocusWidgets(QWidget *oldtlw);

    static int pointToRect(const QPoint &p, const QRect &r);
    QRect fromOrToLayoutItemRect(const QRect &rect, int sign) const;

    void setWinId(WId);
    void showChildren(bool spontaneous);
    void hideChildren(bool spontaneous);
    void setParent_sys(QWidget *parent, Qt::WindowFlags);
    void scroll_sys(int dx, int dy);
    void scroll_sys(int dx, int dy, const QRect &r);
    void deactivateWidgetCleanup();
    void setGeometry_sys(int, int, int, int, bool);
#ifdef Q_WS_MAC
    void setGeometry_sys_helper(int, int, int, int, bool);
#endif
    void sendPendingMoveAndResizeEvents(bool recursive = false, bool disableUpdates = false);
    void activateChildLayoutsRecursively();
    void show_recursive();
    void show_helper();
    void show_sys();
    void hide_sys();
    void hide_helper();
    void _q_showIfNotHidden();

    void setEnabled_helper(bool);
    void registerDropSite(bool);
#if defined(Q_WS_WIN) && !defined(QT_NO_DRAGANDDROP)
    QOleDropTarget *registerOleDnd(QWidget *widget);
    void unregisterOleDnd(QWidget *widget, QOleDropTarget *target);
#endif
    static void adjustFlags(Qt::WindowFlags &flags, QWidget *w = 0);

    void updateFrameStrut();
    QRect frameStrut() const;

    void setWindowIconText_sys(const QString &cap);
    void setWindowIconText_helper(const QString &cap);
    void setWindowTitle_sys(const QString &cap);

#ifdef Q_OS_WIN
    void grabMouseWhileInWindow();
#endif

#ifndef QT_NO_CURSOR
    void setCursor_sys(const QCursor &cursor);
    void unsetCursor_sys();
#endif

#ifdef Q_WS_MAC
    void setWindowModified_sys(bool b);
    void updateMaximizeButton_sys();
    void setWindowFilePath_sys(const QString &filePath);
    void createWindow_sys();
    void recreateMacWindow();
#ifndef QT_MAC_USE_COCOA
    void initWindowPtr();
    void finishCreateWindow_sys_Carbon(OSWindowRef windowRef);
#else
    void finishCreateWindow_sys_Cocoa(void * /*NSWindow * */ windowRef);
    void finishCocoaMaskSetup();
#endif
    void determineWindowClass();
    void transferChildren();
#endif
    void setWindowTitle_helper(const QString &cap);
    void setWindowFilePath_helper(const QString &filePath);

    bool setMinimumSize_helper(int &minw, int &minh);
    bool setMaximumSize_helper(int &maxw, int &maxh);
    void setConstraints_sys();
    QWidget *childAt_helper(const QPoint &, bool) const;
    void updateGeometry_helper(bool forceUpdate);

    void getLayoutItemMargins(int *left, int *top, int *right, int *bottom) const;
    void setLayoutItemMargins(int left, int top, int right, int bottom);
    void setLayoutItemMargins(QStyle::SubElement element, const QStyleOption *opt = 0);

    QInputContext *inputContext() const;

#if defined(Q_WS_QWS)
    void moveSurface(QWindowSurface *surface, const QPoint &offset);

    QRegion localRequestedRegion() const;
    QRegion localAllocatedRegion() const;

    void blitToScreen(const QRegion &globalrgn);
#ifndef QT_NO_CURSOR
    void updateCursor() const;
#endif

    QScreen* getScreen() const;

    friend class QWSManager;
    friend class QWSManagerPrivate;
    friend class QDecoration;
#endif

    static int instanceCounter; // Current number of widget instances
    static int maxInstances; // Maximum number of widget instances

#ifdef QT_KEYPAD_NAVIGATION
    static QPointer<QWidget> editingWidget;
#endif

    QWidgetData data;

    QWExtra *extra;
    QWidget *focus_next;
    QWidget *focus_prev;
    QWidget *focus_child;
#ifndef QT_NO_ACTION
    QList<QAction*> actions;
#endif
    QLayout *layout;
    QWidgetItemV2 *widgetItem;
#if !defined(QT_NO_IM)
    QPointer<QInputContext> ic;
#endif
    // All widgets are initially added into the uncreatedWidgets set. Once
    // they receive a window id they are removed and added to the mapper
    static QWidgetMapper *mapper;
    static QWidgetSet *uncreatedWidgets;

    short leftmargin, topmargin, rightmargin, bottommargin;

    signed char leftLayoutItemMargin;
    signed char topLayoutItemMargin;
    signed char rightLayoutItemMargin;
    signed char bottomLayoutItemMargin;

    // ### TODO: reorganize private/extra/topextra to save memory
    QPointer<QWidget> compositeChildGrab;
#ifndef QT_NO_TOOLTIP
    QString toolTip;
#endif
#ifndef QT_NO_STATUSTIP
    QString statusTip;
#endif
#ifndef QT_NO_WHATSTHIS
    QString whatsThis;
#endif
    QString accessibleName, accessibleDescription;

    QPalette::ColorRole fg_role : 8;
    QPalette::ColorRole bg_role : 8;
    uint high_attributes[3]; // the low ones are in QWidget::widget_attributes
    Qt::HANDLE hd;
    QRegion dirty;
    QRegion *needsFlush;
    uint dirtyOpaqueChildren : 1;
    uint isOpaque : 1;
    uint inDirtyList : 1;
    uint isScrolled : 1;
    uint isMoved : 1;

#ifdef Q_WS_WIN
    uint noPaintOnScreen : 1; // see qwidget_win.cpp ::paintEngine()
#endif

    uint inheritedFontResolveMask;
    uint inheritedPaletteResolveMask;
#if defined(Q_WS_X11)
    QX11Info xinfo;
    Qt::HANDLE picture;
#endif
#if defined(Q_WS_MAC)
    enum PaintChildrenOPs {
        PC_None = 0x00,
        PC_Now = 0x01,
        PC_NoPaint = 0x04,
        PC_Later = 0x10
    };
    EventHandlerRef window_event;
    bool qt_mac_dnd_event(uint, DragRef);
    void toggleDrawers(bool);
    //mac event functions
    static bool qt_create_root_win();
    static void qt_clean_root_win();
    static bool qt_recreate_root_win();
    static bool qt_mac_update_sizer(QWidget *, int up = 0);
    static OSStatus qt_window_event(EventHandlerCallRef er, EventRef event, void *);
    static OSStatus qt_widget_event(EventHandlerCallRef er, EventRef event, void *);
    static bool qt_widget_rgn(QWidget *, short, RgnHandle, bool);
    static bool qt_widget_shape(QWidget *, short, HIMutableShapeRef, bool);

    // Each wiget keeps a list of all its child and grandchild OpenGL widgets.
    // This list is used to update the gl context whenever a parent and a granparent
    // moves, and also to check for intersections with gl widgets within the window
    // when a widget moves.
    struct GlWidgetInfo
    {
        GlWidgetInfo(QWidget *widget) : widget(widget), lastUpdateWidget(0) { }
        bool operator==(const GlWidgetInfo &other) const { return (widget == other.widget); }
        QWidget * widget;
        QWidget * lastUpdateWidget;
    };
    QList<GlWidgetInfo> glWidgets;

    // dirtyOnWidget contains the areas in the widget that needs to be repained,
    // in the same way as dirtyOnScreen does for the window. Areas are added in
    // dirtyWidget_sys and cleared in the paint event. In scroll_sys we then use
    // this information repaint invalid areas when widgets are scrolled.
    QRegion dirtyOnWidget;

    //these are here just for code compat (HIViews)
    Qt::HANDLE qd_hd;

    // This is new stuff
    uint needWindowChange : 1;
    uint isGLWidget : 1;
#endif

#if defined(Q_WS_X11) || defined (Q_WS_WIN) || defined(Q_WS_MAC)
#ifdef Q_WS_MAC
    void setWSGeometry(bool dontShow=false, const QRect &oldRect = QRect());
#else
    void setWSGeometry(bool dontShow=false);
#endif

    inline QPoint mapToWS(const QPoint &p) const
    { return p - data.wrect.topLeft(); }

    inline QPoint mapFromWS(const QPoint &p) const
    { return p + data.wrect.topLeft(); }

    inline QRect mapToWS(const QRect &r) const
    { QRect rr(r); rr.translate(-data.wrect.topLeft()); return rr; }

    inline QRect mapFromWS(const QRect &r) const
    { QRect rr(r); rr.translate(data.wrect.topLeft()); return rr; }
#endif

    QPaintEngine *extraPaintEngine;

    mutable const QMetaObject *polished;

    void setModal_sys();
    QSizePolicy size_policy;
    QLocale locale;

#ifdef Q_WS_X11
    static QWidget *mouseGrabber;
    static QWidget *keyboardGrabber;
#endif
    QPaintDevice *redirectDev;
    QPoint redirectOffset;

    inline void setRedirected(QPaintDevice *replacement, const QPoint &offset)
    {
        Q_ASSERT(q_func()->testAttribute(Qt::WA_WState_InPaintEvent));
        redirectDev = replacement;
        redirectOffset = offset;
    }

    inline QPaintDevice *redirected(QPoint *offset) const
    {
        if (offset)
            *offset = redirectDev ? redirectOffset : QPoint();
        return redirectDev;
    }

    inline void restoreRedirected()
    { redirectDev = 0; }

    inline void enforceNativeChildren()
    {
        if (!extra)
            createExtra();

        if (extra->nativeChildrenForced)
            return;
        extra->nativeChildrenForced = 1;

        for (int i = 0; i < children.size(); ++i) {
            if (QWidget *child = qobject_cast<QWidget *>(children.at(i)))
                child->setAttribute(Qt::WA_NativeWindow);
        }
    }

    inline bool nativeChildrenForced() const
    {
        return extra ? extra->nativeChildrenForced : false;
    }

    QSize adjustedSize() const;
};

inline QWExtra *QWidgetPrivate::extraData() const
{
    return extra;
}

inline QTLWExtra *QWidgetPrivate::topData() const
{
    const_cast<QWidgetPrivate *>(this)->createTLExtra();
    return extra->topextra;
}

inline QTLWExtra *QWidgetPrivate::maybeTopData() const
{
    return extra ? extra->topextra : 0;
}

inline QPainter *QWidgetPrivate::sharedPainter() const
{
    Q_Q(const QWidget);
    QTLWExtra *x = q->window()->d_func()->maybeTopData();
    return x ? x->sharedPainter : 0;
}

inline void QWidgetPrivate::setSharedPainter(QPainter *painter)
{
    Q_Q(QWidget);
    QTLWExtra *x = q->window()->d_func()->topData();
    x->sharedPainter = painter;
}

inline QWidgetBackingStore *QWidgetPrivate::maybeBackingStore() const
{
    Q_Q(const QWidget);
    QTLWExtra *x = q->window()->d_func()->maybeTopData();
    return x ? x->backingStore : 0;
}

QT_END_NAMESPACE

#endif // QWIDGET_P_H
