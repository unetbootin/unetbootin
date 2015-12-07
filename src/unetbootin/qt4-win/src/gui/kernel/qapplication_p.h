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

#ifndef QAPPLICATION_P_H
#define QAPPLICATION_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists for the convenience
// of qapplication_*.cpp, qwidget*.cpp, qcolor_x11.cpp, qfiledialog.cpp
// and many other.  This header file may change from version to version
// without notice, or even be removed.
//
// We mean it.
//

#include "QtGui/qapplication.h"
#include "QtGui/qfont.h"
#include "QtGui/qcursor.h"
#include "QtGui/qregion.h"
#include "QtCore/qmutex.h"
#include "QtCore/qtranslator.h"
#include "QtCore/qbasictimer.h"
#include "QtCore/qhash.h"
#include "private/qcoreapplication_p.h"
#include "private/qshortcutmap_p.h"
#include <private/qthread_p.h>
#ifdef Q_WS_QWS
#include "QtGui/qscreen_qws.h"
#endif

QT_BEGIN_NAMESPACE

class QWidget;
class QObject;
class QClipboard;
class QKeyEvent;
class QMouseEvent;
class QWheelEvent;
class QInputContext;

extern bool qt_is_gui_used;
#ifndef QT_NO_CLIPBOARD
extern QClipboard *qt_clipboard;
#endif

#if defined (Q_OS_WIN32) || defined (Q_OS_CYGWIN) || defined(Q_OS_WINCE)
extern QSysInfo::WinVersion qt_winver;
enum { QT_TABLET_NPACKETQSIZE = 128 };
# ifdef Q_OS_WINCE
  extern DWORD qt_cever;
# endif
#elif defined (Q_OS_MAC)
extern QSysInfo::MacVersion qt_macver;
#endif
#if defined(Q_WS_QWS)
class QWSManager;
class QDirectPainter;
#endif

#ifndef QT_NO_TABLET
struct QTabletDeviceData
{
    int minPressure;
    int maxPressure;
    int minTanPressure;
    int maxTanPressure;
    int minX, maxX, minY, maxY, minZ, maxZ;
    inline QPointF scaleCoord(int coordX, int coordY, int outOriginX, int outExtentX,
                              int outOriginY, int outExtentY) const;
#ifdef Q_WS_X11
    int deviceType;
    enum {
        TOTAL_XINPUT_EVENTS = 64
    };
    void *device;
    int eventCount;
    long unsigned int eventList[TOTAL_XINPUT_EVENTS]; // XEventClass is in fact a long unsigned int

    int xinput_motion;
    int xinput_key_press;
    int xinput_key_release;
    int xinput_button_press;
    int xinput_button_release;
    int xinput_proximity_in;
    int xinput_proximity_out;
#elif defined(Q_WS_WIN)
    qint64 llId;
    int currentDevice;
    int currentPointerType;
#endif
};

static inline int sign(int x)
{
    return x >= 0 ? 1 : -1;
}

inline QPointF QTabletDeviceData::scaleCoord(int coordX, int coordY,
                                            int outOriginX, int outExtentX,
                                            int outOriginY, int outExtentY) const
{
    QPointF ret;
    if (sign(outExtentX) == sign(maxX))
        ret.setX(((coordX - minX) * qAbs(outExtentX) / qAbs(qreal(maxX))) + outOriginX);
    else
        ret.setX(((qAbs(maxX) - (coordX - minX)) * qAbs(outExtentX) / qAbs(qreal(maxX)))
                 + outOriginX);

    if (sign(outExtentY) == sign(maxY))
        ret.setY(((coordY - minY) * qAbs(outExtentY) / qAbs(qreal(maxY))) + outOriginY);
    else
        ret.setY(((qAbs(maxY) - (coordY - minY)) * qAbs(outExtentY) / qAbs(qreal(maxY)))
                 + outOriginY);
    return ret;
}

typedef QList<QTabletDeviceData> QTabletDeviceDataList;
QTabletDeviceDataList *qt_tablet_devices();
#endif

#ifdef QT3_SUPPORT
extern "C" {
    typedef bool (*Ptrqt_tryAccelEvent)(QWidget *w, QKeyEvent *e);
    typedef bool (*Ptrqt_tryComposeUnicode)(QWidget *w, QKeyEvent *e);
    typedef bool (*Ptrqt_dispatchAccelEvent)(QWidget *w, QKeyEvent *e);
}
#endif

class QScopedLoopLevelCounter
{
    QThreadData *threadData;
public:
    QScopedLoopLevelCounter(QThreadData *threadData)
        : threadData(threadData)
    { ++threadData->loopLevel; }
    ~QScopedLoopLevelCounter()
    { --threadData->loopLevel; }
};

class Q_GUI_EXPORT QApplicationPrivate : public QCoreApplicationPrivate
{
    Q_DECLARE_PUBLIC(QApplication)
public:
    QApplicationPrivate(int &argc, char **argv, QApplication::Type type);
    ~QApplicationPrivate();

#if defined(Q_WS_X11)
#ifndef QT_NO_SETTINGS
    static bool x11_apply_settings();
#endif
    static void reset_instance_pointer();
#elif defined(Q_WS_QWS)
    static bool qws_apply_settings();
    static QWidget *findWidget(const QObjectList&, const QPoint &, bool rec);
#endif
    static bool quitOnLastWindowClosed;
    static void emitLastWindowClosed();
#ifdef Q_OS_WINCE
    static int autoMaximizeThreshold;
#endif

    void createEventDispatcher();
    QString appName() const;

    static void dispatchEnterLeave(QWidget *enter, QWidget *leave);

    //modality
    static void enterModal(QWidget*);
    static void leaveModal(QWidget*);
    static void enterModal_sys(QWidget*);
    static void leaveModal_sys(QWidget*);
    static bool isBlockedByModal(QWidget *widget);
    static bool modalState();
    static bool tryModalHelper(QWidget *widget, QWidget **rettop = 0);
#ifdef Q_WS_MAC
    static QWidget *tryModalHelper_sys(QWidget *top);
#endif

    bool notify_helper(QObject *receiver, QEvent * e);

    void construct(
#ifdef Q_WS_X11
                   Display *dpy = 0, Qt::HANDLE visual = 0, Qt::HANDLE cmap = 0
#endif
                   );
    void initialize();
    void process_cmdline();

#if defined(Q_WS_X11)
    static void x11_initialize_style();
#endif

    enum KeyPlatform {
        KB_Win = 1,
        KB_Mac = 2,
        KB_X11 = 4,
        KB_KDE = 8,
        KB_Gnome = 16,
        KB_CDE = 32,
        KB_All = 0xffff
    };

    static uint currentPlatform();
    bool inPopupMode() const;
    void closePopup(QWidget *popup);
    void openPopup(QWidget *popup);
    static void setFocusWidget(QWidget *focus, Qt::FocusReason reason);
    static QWidget *focusNextPrevChild_helper(QWidget *toplevel, bool next);

#ifndef QT_NO_SESSIONMANAGER
    QSessionManager *session_manager;
    QString session_id;
    QString session_key;
    bool is_session_restored;
#endif

#ifndef QT_NO_CURSOR
    QList<QCursor> cursor_list;
#endif

    QBasicTimer toolTipWakeUp, toolTipFallAsleep;
    QPoint toolTipPos, toolTipGlobalPos, hoverGlobalPos;
    QPointer<QWidget> toolTipWidget;
#ifndef QT_NO_SHORTCUT
    QShortcutMap shortcutMap;
#endif

#ifdef QT3_SUPPORT
    bool qt_compat_used;
    bool qt_compat_resolved;
    Ptrqt_tryAccelEvent qt_tryAccelEvent;
    Ptrqt_tryComposeUnicode qt_tryComposeUnicode;
    Ptrqt_dispatchAccelEvent qt_dispatchAccelEvent;

    bool use_compat() {
        return qt_tryAccelEvent
               && qt_tryComposeUnicode
               && qt_dispatchAccelEvent;
    }
#endif
    static QInputContext *inputContext;

    static Qt::MouseButtons mouse_buttons;
    static Qt::KeyboardModifiers modifier_buttons;

    static QSize app_strut;
    static QWidgetList *popupWidgets;
    static QStyle *app_style;
    static int app_cspec;
    static QPalette *app_pal;
    static QPalette *sys_pal;
    static QPalette *set_pal;
private:
    static QFont *app_font; // private for a reason! Always use QApplication::font() instead!
public:
    static QFont *sys_font;
    static QFont *set_font;
    static QWidget *main_widget;
    static QWidget *focus_widget;
    static QWidget *hidden_focus_widget;
    static QWidget *active_window;
    static QIcon *app_icon;
    static bool obey_desktop_settings;
    static int  cursor_flash_time;
    static int  mouse_double_click_time;
    static int  keyboard_input_time;
    static int  wheel_scroll_lines;

    static bool animate_ui;
    static bool animate_menu;
    static bool animate_tooltip;
    static bool animate_combo;
    static bool fade_menu;
    static bool fade_tooltip;
    static bool animate_toolbox;
    static bool widgetCount; // Coupled with -widgetcount switch
#ifdef Q_WS_MAC
    static bool native_modal_dialog_active;
#endif
#if defined(Q_WS_WIN) && !defined(Q_OS_WINCE)
    static bool inSizeMove;
#endif

    static void setSystemPalette(const QPalette &pal);
    static void setPalette_helper(const QPalette &palette, const char* className, bool clearWidgetPaletteHash);
    static void initializeWidgetPaletteHash();
    static void setSystemFont(const QFont &font);

#if defined(Q_WS_X11)
    static void applyX11SpecificCommandLineArguments(QWidget *main_widget);
#elif defined(Q_WS_QWS)
    static void applyQWSSpecificCommandLineArguments(QWidget *main_widget);
#endif

#ifdef Q_WS_MAC
    static OSStatus globalEventProcessor(EventHandlerCallRef, EventRef, void *);
    static OSStatus globalAppleEventProcessor(const AppleEvent *, AppleEvent *, long);
    static void qt_context_timer_callbk(EventLoopTimerRef, void *);
    static bool qt_mac_apply_settings();
#endif

#ifdef Q_WS_QWS
    QPointer<QWSManager> last_manager;
# ifndef QT_NO_DIRECTPAINTER
    QMap<WId, QDirectPainter *> *directPainters;
# endif
    QRect maxWindowRect(const QScreen *screen) const { return maxWindowRects[screen]; }
    void setMaxWindowRect(const QScreen *screen, int screenNo, const QRect &rect);
    void setScreenTransformation(QScreen *screen, int screen, int transformation);
#endif

    static QApplicationPrivate *instance() { return self; }

    static QString *styleOverride;

    static int app_compile_version;

#ifdef QT_KEYPAD_NAVIGATION
    static bool keypadNavigation;
    static QWidget *oldEditFocus;
#endif

#if defined(Q_WS_MAC) || defined(Q_WS_X11)
    void _q_alertTimeOut();
    QHash<QWidget *, QTimer *> alertTimerHash;
#endif
#ifndef QT_NO_STYLE_STYLESHEET
    static QString styleSheet;
#endif
    static QPointer<QWidget> leaveAfterRelease;
    static QWidget *pickMouseReceiver(QWidget *candidate, const QPoint &globalPos, QPoint &pos,
                                      QEvent::Type type, Qt::MouseButtons buttons,
                                      QWidget *buttonDown, QWidget *alienWidget);
    static bool sendMouseEvent(QWidget *receiver, QMouseEvent *event, QWidget *alienWidget,
                               QWidget *native, QWidget **buttonDown, QPointer<QWidget> &lastMouseReceiver);

private:
#ifdef Q_WS_QWS
    QMap<const QScreen*, QRect> maxWindowRects;
#endif

    static QApplicationPrivate *self;
};

QT_END_NAMESPACE

#endif // QAPPLICATION_P_H
