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

#ifdef Q_OS_WINCE
#include "qguifunctions_wince.h"
#include "qmenubar.h"
extern bool qt_wince_is_mobile();             //defined in qguifunctions_wce.cpp
extern bool qt_wince_is_high_dpi();           //defined in qguifunctions_wce.cpp
extern bool qt_wince_is_smartphone();         //defined in qguifunctions_wce.cpp
extern bool qt_wince_is_pocket_pc();          //defined in qguifunctions_wce.cpp
extern void qt_wince_hide_taskbar(HWND hwnd); //defined in qguifunctions_wce.cpp
#endif
#ifdef Q_OS_WINCE_WM
#include <windowsm.h>
#include <tpcshell.h>
#endif

#include "qapplication.h"
#include "qdesktopwidget.h"
#include "qevent.h"
#include "private/qeventdispatcher_win_p.h"
#include "qeventloop.h"
#include "qclipboard.h"
#include "qcursor.h"
#include "qdatetime.h"
#include "qpointer.h"
#include "qhash.h"
#include "qlibrary.h"
#include "qmetaobject.h"
#include "qmime.h"
#include "qpainter.h"
#include "qpixmapcache.h"
#include "qsessionmanager.h"
#include "qstyle.h"
#include "qwhatsthis.h" // ######## dependency
#include "qwidget.h"
#include "qcolormap.h"
#include "qlayout.h"
#include "qtooltip.h"
#include "qt_windows.h"
#if defined(QT_NON_COMMERCIAL)
#include "qnc_win.h"
#endif
#include "private/qwininputcontext_p.h"
#include "private/qcursor_p.h"
#include "private/qmath_p.h"
#include "private/qapplication_p.h"
#include "private/qbackingstore_p.h"
#include "qdebug.h"
#include <private/qkeymapper_p.h>

//#define ALIEN_DEBUG

#ifndef QT_NO_THREAD
#include "qmutex.h"
#endif

#ifndef QT_NO_ACCESSIBILITY
#include "qaccessible.h"

#include <oleacc.h>
#ifndef WM_GETOBJECT
#define WM_GETOBJECT                    0x003D
#endif
#endif // QT_NO_ACCESSIBILITY

#if WINVER >= 0x0600
#include <winuser.h>
#else
#  if defined(Q_OS_WINCE)
#    include <bldver.h>
#  endif
#include <winable.h>
#endif


#ifndef FLASHW_STOP
typedef struct {
    UINT  cbSize;
    HWND  hwnd;
    DWORD dwFlags;
    UINT  uCount;
    DWORD dwTimeout;
} FLASHWINFO, *PFLASHWINFO;
#define FLASHW_STOP         0
#define FLASHW_CAPTION      0x00000001
#define FLASHW_TRAY         0x00000002
#define FLASHW_ALL          (FLASHW_CAPTION | FLASHW_TRAY)
#define FLASHW_TIMER        0x00000004
#define FLASHW_TIMERNOFG    0x0000000C
#endif /* FLASHW_STOP */
typedef BOOL (WINAPI *PtrFlashWindowEx)(PFLASHWINFO pfwi);
static PtrFlashWindowEx pFlashWindowEx = 0;

#include <windowsx.h>
#include <limits.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include <math.h>

#define PACKETDATA  (PK_X | PK_Y | PK_BUTTONS | PK_NORMAL_PRESSURE | PK_TANGENT_PRESSURE \
                     | PK_ORIENTATION | PK_CURSOR | PK_Z)
#define PACKETMODE  0

#include <wintab.h>
#ifndef CSR_TYPE
#define CSR_TYPE 20 // Some old Wacom wintab.h may not provide this constant.
#endif
#include <pktdef.h>

#if defined(__CYGWIN32__)
#define __INSIDE_CYGWIN32__
#include <mywinsock.h>
#endif

QT_BEGIN_NAMESPACE

#ifdef Q_OS_WINCE
#ifndef SHRG_RETURNCMD
struct SHRGINFO {
    DWORD cbSize;
    HWND hwndClient;
    POINT ptDown;
    DWORD dwFlags;
}
#define  GN_CONTEXTMENU       1000
#define  SHRG_RETURNCMD       0x00000001
#define  SHRG_NOANIMATION     0x00000010
#endif
typedef DWORD (WINSHELLAPI *AygRecognizeGesture)(SHRGINFO*);
static AygRecognizeGesture ptrRecognizeGesture = 0;
static bool aygResolved = false;
static void resolveAygLibs()
{
    if (!aygResolved) {
        aygResolved = true;
        QLibrary ayglib(QLatin1String("aygshell"));
        if (!ayglib.load())
            return;
        ptrRecognizeGesture = (AygRecognizeGesture) ayglib.resolve("SHRecognizeGesture");
    }
}

#endif

typedef HCTX (API *PtrWTOpen)(HWND, LPLOGCONTEXT, BOOL);
typedef BOOL (API *PtrWTClose)(HCTX);
typedef UINT (API *PtrWTInfo)(UINT, UINT, LPVOID);
typedef BOOL (API *PtrWTEnable)(HCTX, BOOL);
typedef BOOL (API *PtrWTOverlap)(HCTX, BOOL);
typedef int  (API *PtrWTPacketsGet)(HCTX, int, LPVOID);
typedef BOOL (API *PtrWTGet)(HCTX, LPLOGCONTEXT);
typedef int  (API *PtrWTQueueSizeGet)(HCTX);
typedef BOOL (API *PtrWTQueueSizeSet)(HCTX, int);

static PtrWTInfo ptrWTInfo = 0;
static PtrWTEnable ptrWTEnable = 0;
static PtrWTOverlap ptrWTOverlap = 0;
static PtrWTPacketsGet ptrWTPacketsGet = 0;
static PtrWTGet ptrWTGet = 0;

static PACKET localPacketBuf[QT_TABLET_NPACKETQSIZE];  // our own tablet packet queue.
HCTX qt_tablet_context;  // the hardware context for the tablet (like a window handle)
bool qt_tablet_tilt_support;
static void tabletInit(UINT wActiveCsr, HCTX hTab);
static void initWinTabFunctions();        // resolve the WINTAB api functions


#ifndef QT_NO_ACCESSIBILITY
extern IAccessible *qt_createWindowsAccessible(QAccessibleInterface *object);
#endif // QT_NO_ACCESSIBILITY

extern bool qt_tabletChokeMouse;
extern QWidget* qt_get_tablet_widget();

typedef QHash<UINT, QTabletDeviceData> QTabletCursorInfo;
Q_GLOBAL_STATIC(QTabletCursorInfo, tCursorInfo)
QTabletDeviceData currentTabletPointer;

// from qregion_win.cpp
extern HRGN qt_tryCreateRegion(QRegion::RegionType type, int left, int top, int right, int bottom);

Q_CORE_EXPORT bool winPeekMessage(MSG* msg, HWND hWnd, UINT wMsgFilterMin,
                            UINT wMsgFilterMax, UINT wRemoveMsg);
Q_CORE_EXPORT bool winPostMessage(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// support for on-the-fly changes of the XP theme engine
#ifndef WM_THEMECHANGED
#define WM_THEMECHANGED                 0x031A
#endif
#ifndef COLOR_MENUHILIGHT
#define COLOR_MENUHILIGHT                29
#define COLOR_MENUBAR                        30
#endif

// support for xbuttons
#ifndef WM_XBUTTONDOWN
#define WM_XBUTTONDOWN                  0x020B
#define WM_XBUTTONUP                    0x020C
#define WM_XBUTTONDBLCLK                0x020D
#define GET_KEYSTATE_WPARAM(wParam)     (LOWORD(wParam))
#define GET_XBUTTON_WPARAM(wParam)      (HIWORD(wParam))
#define XBUTTON1      0x0001
#define XBUTTON2      0x0002
#define MK_XBUTTON1         0x0020
#define MK_XBUTTON2         0x0040
#endif

#ifdef Q_OS_WINCE
#define GET_KEYSTATE_WPARAM(wParam)     (LOWORD(wParam))
#endif

// support for multi-media-keys on ME/2000/XP
#ifndef WM_APPCOMMAND
#define WM_APPCOMMAND                   0x0319

#define FAPPCOMMAND_MOUSE 0x8000
#define FAPPCOMMAND_KEY   0
#define FAPPCOMMAND_OEM   0x1000
#define FAPPCOMMAND_MASK  0xF000
#define GET_APPCOMMAND_LPARAM(lParam) ((short)(HIWORD(lParam) & ~FAPPCOMMAND_MASK))
#define GET_DEVICE_LPARAM(lParam)     ((WORD)(HIWORD(lParam) & FAPPCOMMAND_MASK))
#define GET_MOUSEORKEY_LPARAM         GET_DEVICE_LPARAM
#define GET_FLAGS_LPARAM(lParam)      (LOWORD(lParam))
#define GET_KEYSTATE_LPARAM(lParam)   GET_FLAGS_LPARAM(lParam)

#define APPCOMMAND_BROWSER_BACKWARD       1
#define APPCOMMAND_BROWSER_FORWARD        2
#define APPCOMMAND_BROWSER_REFRESH        3
#define APPCOMMAND_BROWSER_STOP           4
#define APPCOMMAND_BROWSER_SEARCH         5
#define APPCOMMAND_BROWSER_FAVORITES      6
#define APPCOMMAND_BROWSER_HOME           7
#define APPCOMMAND_VOLUME_MUTE            8
#define APPCOMMAND_VOLUME_DOWN            9
#define APPCOMMAND_VOLUME_UP              10
#define APPCOMMAND_MEDIA_NEXTTRACK        11
#define APPCOMMAND_MEDIA_PREVIOUSTRACK    12
#define APPCOMMAND_MEDIA_STOP             13
#define APPCOMMAND_MEDIA_PLAY_PAUSE       14
#define APPCOMMAND_LAUNCH_MAIL            15
#define APPCOMMAND_LAUNCH_MEDIA_SELECT    16
#define APPCOMMAND_LAUNCH_APP1            17
#define APPCOMMAND_LAUNCH_APP2            18
#define APPCOMMAND_BASS_DOWN              19
#define APPCOMMAND_BASS_BOOST             20
#define APPCOMMAND_BASS_UP                21
#define APPCOMMAND_TREBLE_DOWN            22
#define APPCOMMAND_TREBLE_UP              23

// New commands from Windows XP (some even Sp1)
#ifndef APPCOMMAND_MICROPHONE_VOLUME_MUTE
#define APPCOMMAND_MICROPHONE_VOLUME_MUTE 24
#define APPCOMMAND_MICROPHONE_VOLUME_DOWN 25
#define APPCOMMAND_MICROPHONE_VOLUME_UP   26
#define APPCOMMAND_HELP                   27
#define APPCOMMAND_FIND                   28
#define APPCOMMAND_NEW                    29
#define APPCOMMAND_OPEN                   30
#define APPCOMMAND_CLOSE                  31
#define APPCOMMAND_SAVE                   32
#define APPCOMMAND_PRINT                  33
#define APPCOMMAND_UNDO                   34
#define APPCOMMAND_REDO                   35
#define APPCOMMAND_COPY                   36
#define APPCOMMAND_CUT                    37
#define APPCOMMAND_PASTE                  38
#define APPCOMMAND_REPLY_TO_MAIL          39
#define APPCOMMAND_FORWARD_MAIL           40
#define APPCOMMAND_SEND_MAIL              41
#define APPCOMMAND_SPELL_CHECK            42
#define APPCOMMAND_DICTATE_OR_COMMAND_CONTROL_TOGGLE    43
#define APPCOMMAND_MIC_ON_OFF_TOGGLE      44
#define APPCOMMAND_CORRECTION_LIST        45
#define APPCOMMAND_MEDIA_PLAY             46
#define APPCOMMAND_MEDIA_PAUSE            47
#define APPCOMMAND_MEDIA_RECORD           48
#define APPCOMMAND_MEDIA_FAST_FORWARD     49
#define APPCOMMAND_MEDIA_REWIND           50
#define APPCOMMAND_MEDIA_CHANNEL_UP       51
#define APPCOMMAND_MEDIA_CHANNEL_DOWN     52
#endif // APPCOMMAND_MICROPHONE_VOLUME_MUTE

#endif // WM_APPCOMMAND

static UINT WM95_MOUSEWHEEL = 0;

#if (_WIN32_WINNT < 0x0400)
// This struct is defined in winuser.h if the _WIN32_WINNT >= 0x0400 -- in the
// other cases we have to define it on our own.
typedef struct tagTRACKMOUSEEVENT {
    DWORD cbSize;
    DWORD dwFlags;
    HWND  hwndTrack;
    DWORD dwHoverTime;
} TRACKMOUSEEVENT, *LPTRACKMOUSEEVENT;
#endif
#ifndef WM_MOUSELEAVE
#define WM_MOUSELEAVE                   0x02A3
#endif

QT_BEGIN_INCLUDE_NAMESPACE
#include "private/qwidget_p.h"
QT_END_INCLUDE_NAMESPACE

static int translateButtonState(int s, int type, int button);

// ##### get rid of this!
QRgb qt_colorref2qrgb(COLORREF col)
{
    return qRgb(GetRValue(col),GetGValue(col),GetBValue(col));
}


/*****************************************************************************
  Internal variables and functions
 *****************************************************************************/

extern void qt_syncBackingStore(QRegion rgn, QWidget *widget);
extern Q_CORE_EXPORT char      theAppName[];
extern Q_CORE_EXPORT char      appFileName[];
extern Q_CORE_EXPORT HINSTANCE appInst;                        // handle to app instance
extern Q_CORE_EXPORT HINSTANCE appPrevInst;                        // handle to prev app instance
extern Q_CORE_EXPORT int appCmdShow;                                // main window show command
static HWND         curWin                = 0;                // current window
static HDC         displayDC        = 0;                // display device context

// Session management
static bool        sm_blockUserInput    = false;
static bool        sm_smActive             = false;
extern QSessionManager* qt_session_manager_self;
static bool        sm_cancel;

static bool replayPopupMouseEvent = false; // replay handling when popups close

// ignore the next release event if return from a modal widget
Q_GUI_EXPORT bool qt_win_ignoreNextMouseReleaseEvent = false;


#if defined(QT_DEBUG)
static bool        appNoGrab        = false;        // mouse/keyboard grabbing
#endif

static bool        app_do_modal           = false;        // modal mode
extern QWidgetList *qt_modal_stack;
extern QDesktopWidget *qt_desktopWidget;
static QWidget *popupButtonFocus   = 0;
static bool        qt_try_modal(QWidget *, MSG *, int& ret);

QWidget               *qt_button_down = 0;                // widget got last button-down
QPointer<QWidget> qt_last_mouse_receiver = 0;

static HWND        autoCaptureWnd = 0;
static HWND        imeParentWnd = 0;
static void        setAutoCapture(HWND);                // automatic capture
static void        releaseAutoCapture();

static void     unregWinClasses();

extern QCursor *qt_grab_cursor();

#if defined(Q_WS_WIN)
#define __export
#endif

extern "C" LRESULT CALLBACK QtWndProc(HWND, UINT, WPARAM, LPARAM);

class QETWidget : public QWidget                // event translator widget
{
public:
    QWExtra    *xtra() { return d_func()->extraData(); }
    QTLWExtra  *topData() { return d_func()->topData(); }
    QTLWExtra  *maybeTopData() { return d_func()->maybeTopData(); }
    QWidgetData *dataPtr() { return data; }
    QRect frameStrut() const { return d_func()->frameStrut(); }
    bool        winEvent(MSG *m, long *r)        { return QWidget::winEvent(m, r); }
    void        markFrameStrutDirty()        { data->fstrut_dirty = 1; }
    bool        translateMouseEvent(const MSG &msg);
    bool        translateWheelEvent(const MSG &msg);
    bool        translatePaintEvent(const MSG &msg);
    bool        translateConfigEvent(const MSG &msg);
    bool        translateCloseEvent(const MSG &msg);
    bool        translateTabletEvent(const MSG &msg, PACKET *localPacketBuf, int numPackets);
    void        repolishStyle(QStyle &style);
    inline void showChildren(bool spontaneous) { d_func()->showChildren(spontaneous); }
    inline void hideChildren(bool spontaneous) { d_func()->hideChildren(spontaneous); }
    inline uint testWindowState(uint teststate){ return dataPtr()->window_state & teststate; }
};

// need to get default font?
extern bool qt_app_has_font;

extern QFont qt_LOGFONTtoQFont(LOGFONT& lf,bool scale);

static void qt_set_windows_color_resources()
{
    // Do the color settings
    QPalette pal;
    pal.setColor(QPalette::WindowText,
                 QColor(qt_colorref2qrgb(GetSysColor(COLOR_WINDOWTEXT))));
    pal.setColor(QPalette::Button,
                 QColor(qt_colorref2qrgb(GetSysColor(COLOR_BTNFACE))));
    pal.setColor(QPalette::Light,
                 QColor(qt_colorref2qrgb(GetSysColor(COLOR_BTNHIGHLIGHT))));
    pal.setColor(QPalette::Dark,
                 QColor(qt_colorref2qrgb(GetSysColor(COLOR_BTNSHADOW))));
    pal.setColor(QPalette::Mid, pal.button().color().darker(150));
    pal.setColor(QPalette::Text,
                 QColor(qt_colorref2qrgb(GetSysColor(COLOR_WINDOWTEXT))));
    pal.setColor(QPalette::BrightText,
                 QColor(qt_colorref2qrgb(GetSysColor(COLOR_BTNHIGHLIGHT))));
    pal.setColor(QPalette::Base,
                 QColor(qt_colorref2qrgb(GetSysColor(COLOR_WINDOW))));
    pal.setColor(QPalette::Window,
                 QColor(qt_colorref2qrgb(GetSysColor(COLOR_BTNFACE))));
    pal.setColor(QPalette::ButtonText,
                 QColor(qt_colorref2qrgb(GetSysColor(COLOR_BTNTEXT))));
    pal.setColor(QPalette::Midlight,
                 QColor(qt_colorref2qrgb(GetSysColor(COLOR_3DLIGHT))));
    pal.setColor(QPalette::Shadow,
                 QColor(qt_colorref2qrgb(GetSysColor(COLOR_3DDKSHADOW))));
    pal.setColor(QPalette::Highlight,
                 QColor(qt_colorref2qrgb(GetSysColor(COLOR_HIGHLIGHT))));
    pal.setColor(QPalette::HighlightedText,
                 QColor(qt_colorref2qrgb(GetSysColor(COLOR_HIGHLIGHTTEXT))));

#if defined(Q_OS_WINCE)
    // ### hardcoded until I find out how to get it from the system settings.
    pal.setColor(QPalette::LinkVisited, pal.highlight().color().dark(150));
    pal.setColor(QPalette::Link, pal.highlight().color().light(130));
    // Background == Base on Windows CE
    if (qt_wince_is_smartphone() || qt_wince_is_pocket_pc())
      pal.setColor(QPalette::Background, pal.base().color());
#else
    pal.setColor(QPalette::Link, Qt::blue);
    pal.setColor(QPalette::LinkVisited, Qt::magenta);
#endif

    

    pal.setColor(QPalette::Inactive, QPalette::Button, pal.button().color());
    pal.setColor(QPalette::Inactive, QPalette::Window, pal.background().color());
    pal.setColor(QPalette::Inactive, QPalette::Light, pal.light().color());
    pal.setColor(QPalette::Inactive, QPalette::Dark, pal.dark().color());

    if (QSysInfo::WindowsVersion != QSysInfo::WV_NT && QSysInfo::WindowsVersion != QSysInfo::WV_95) {
        if (pal.midlight() == pal.button())
            pal.setColor(QPalette::Midlight, pal.button().color().lighter(110));
        if (pal.background() != pal.base()) {
            pal.setColor(QPalette::Inactive, QPalette::Highlight, pal.color(QPalette::Inactive, QPalette::Window));
            pal.setColor(QPalette::Inactive, QPalette::HighlightedText, pal.color(QPalette::Inactive, QPalette::Text));
        }
    }
    const QColor bg = pal.background().color();
    const QColor fg = pal.foreground().color(), btn = pal.button().color();
    QColor disabled((fg.red()+btn.red())/2,(fg.green()+btn.green())/2,
                     (fg.blue()+btn.blue())/2);
    pal.setColorGroup(QPalette::Disabled, pal.foreground(), pal.button(), pal.light(),
        pal.dark(), pal.mid(), pal.text(), pal.brightText(), pal.base(), pal.background() );
    pal.setColor(QPalette::Disabled, QPalette::WindowText, disabled);
    pal.setColor(QPalette::Disabled, QPalette::Text, disabled);
    pal.setColor(QPalette::Disabled, QPalette::ButtonText, disabled);
    pal.setColor(QPalette::Disabled, QPalette::Highlight,
                  QColor(qt_colorref2qrgb(GetSysColor(COLOR_HIGHLIGHT))));
    pal.setColor(QPalette::Disabled, QPalette::HighlightedText,
                  QColor(qt_colorref2qrgb(GetSysColor(COLOR_HIGHLIGHTTEXT))));
    pal.setColor(QPalette::Disabled, QPalette::Base, bg);

    QApplicationPrivate::setSystemPalette(pal);

    QApplicationPrivate::initializeWidgetPaletteHash();

    QColor ttip(qt_colorref2qrgb(GetSysColor(COLOR_INFOBK)));

    QColor ttipText(qt_colorref2qrgb(GetSysColor(COLOR_INFOTEXT)));
    {
#ifndef QT_NO_TOOLTIP
        QPalette tiplabel(pal);
        tiplabel.setColor(QPalette::All, QPalette::Button, ttip);
        tiplabel.setColor(QPalette::All, QPalette::Window, ttip);
        tiplabel.setColor(QPalette::All, QPalette::Text, ttipText);
        tiplabel.setColor(QPalette::All, QPalette::WindowText, ttipText);
        tiplabel.setColor(QPalette::All, QPalette::ButtonText, ttipText);
        tiplabel.setColor(QPalette::All, QPalette::Button, ttip);
        tiplabel.setColor(QPalette::All, QPalette::Window, ttip);
        tiplabel.setColor(QPalette::All, QPalette::Text, ttipText);
        tiplabel.setColor(QPalette::All, QPalette::WindowText, ttipText);
        tiplabel.setColor(QPalette::All, QPalette::ButtonText, ttipText);
        const QColor fg = tiplabel.foreground().color(), btn = tiplabel.button().color();
        QColor disabled((fg.red()+btn.red())/2,(fg.green()+btn.green())/2,
                         (fg.blue()+btn.blue())/2);
        tiplabel.setColor(QPalette::Disabled, QPalette::WindowText, disabled);
        tiplabel.setColor(QPalette::Disabled, QPalette::Text, disabled);
        tiplabel.setColor(QPalette::Disabled, QPalette::Base, Qt::white);
        tiplabel.setColor(QPalette::Disabled, QPalette::BrightText, Qt::white);
        QToolTip::setPalette(tiplabel);
#endif //QT_NO_TOOLTIP
    }
}

static void qt_set_windows_font_resources()
{
#ifndef Q_OS_WINCE
    QFont menuFont;
    QFont messageFont;
    QFont statusFont;
    QFont titleFont;
    QFont iconTitleFont;
    QT_WA({
        NONCLIENTMETRICS ncm;
        ncm.cbSize = FIELD_OFFSET(NONCLIENTMETRICS, lfMessageFont) + sizeof(LOGFONTW);
        SystemParametersInfo(SPI_GETNONCLIENTMETRICS, ncm.cbSize , &ncm, 0);
        menuFont = qt_LOGFONTtoQFont(ncm.lfMenuFont,true);
        messageFont = qt_LOGFONTtoQFont(ncm.lfMessageFont,true);
        statusFont = qt_LOGFONTtoQFont(ncm.lfStatusFont,true);
        titleFont = qt_LOGFONTtoQFont(ncm.lfCaptionFont,true);
        LOGFONTW lfIconTitleFont;
        SystemParametersInfoW(SPI_GETICONTITLELOGFONT, sizeof(lfIconTitleFont), &lfIconTitleFont, 0);
        iconTitleFont = qt_LOGFONTtoQFont(lfIconTitleFont,true);
    } , {
        // A version
        NONCLIENTMETRICSA ncm;
        ncm.cbSize = FIELD_OFFSET(NONCLIENTMETRICSA, lfMessageFont) + sizeof(LOGFONTA);
        SystemParametersInfoA(SPI_GETNONCLIENTMETRICS, ncm.cbSize, &ncm, 0);
        menuFont = qt_LOGFONTtoQFont((LOGFONT&)ncm.lfMenuFont,true);
        messageFont = qt_LOGFONTtoQFont((LOGFONT&)ncm.lfMessageFont,true);
        statusFont = qt_LOGFONTtoQFont((LOGFONT&)ncm.lfStatusFont,true);
        titleFont = qt_LOGFONTtoQFont((LOGFONT&)ncm.lfCaptionFont,true);
        LOGFONTA lfIconTitleFont;
        SystemParametersInfoA(SPI_GETICONTITLELOGFONT, sizeof(lfIconTitleFont), &lfIconTitleFont, 0);
        iconTitleFont = qt_LOGFONTtoQFont((LOGFONT&)lfIconTitleFont,true);
    });

    QApplication::setFont(menuFont, "QMenu");
    QApplication::setFont(menuFont, "QMenuBar");
    QApplication::setFont(messageFont, "QMessageBox");
    QApplication::setFont(statusFont, "QTipLabel");
    QApplication::setFont(statusFont, "QStatusBar");
    QApplication::setFont(titleFont, "Q3TitleBar");
    QApplication::setFont(titleFont, "QWorkspaceTitleBar");
    QApplication::setFont(iconTitleFont, "QAbstractItemView");
    QApplication::setFont(iconTitleFont, "QDockWidgetTitle");

#else
    LOGFONT lf;
    HGDIOBJ stockFont = GetStockObject(SYSTEM_FONT);
    GetObject(stockFont, sizeof(lf), &lf);
    QFont systemFont = qt_LOGFONTtoQFont(lf, true);
    QApplicationPrivate::setSystemFont(systemFont);
    QFont smallerFont = systemFont;
    if (qt_wince_is_mobile()) {
        if (qt_wince_is_high_dpi())
            smallerFont.setPointSize(systemFont.pointSize()-1);
        else
            smallerFont.setPointSize(systemFont.pointSize()-1);
        QApplication::setFont(smallerFont, "QTabBar");
    }
#endif// Q_OS_WINCE
}


static void qt_set_windows_resources()
{
    if (QApplication::type() != QApplication::Tty)
        (void) QApplication::style(); // trigger creation of application style
    qt_set_windows_font_resources();
    qt_set_windows_color_resources();
}

void QApplicationPrivate::initializeWidgetPaletteHash()
{
    QPalette pal = *QApplicationPrivate::sys_pal;
    QColor menuCol(qt_colorref2qrgb(GetSysColor(COLOR_MENU)));
    QColor menuText(qt_colorref2qrgb(GetSysColor(COLOR_MENUTEXT)));
    BOOL isFlat = 0;
    if ((QSysInfo::WindowsVersion >= QSysInfo::WV_XP
        && QSysInfo::WindowsVersion < QSysInfo::WV_NT_based))
        SystemParametersInfo(0x1022 /*SPI_GETFLATMENU*/, 0, &isFlat, 0);
    QPalette menu(pal);
    // we might need a special color group for the menu.
    menu.setColor(QPalette::Active, QPalette::Button, menuCol);
    menu.setColor(QPalette::Active, QPalette::Text, menuText);
    menu.setColor(QPalette::Active, QPalette::WindowText, menuText);
    menu.setColor(QPalette::Active, QPalette::ButtonText, menuText);
    const QColor fg = menu.foreground().color(), btn = menu.button().color();
    QColor disabled(qt_colorref2qrgb(GetSysColor(COLOR_GRAYTEXT)));
    menu.setColor(QPalette::Disabled, QPalette::WindowText, disabled);
    menu.setColor(QPalette::Disabled, QPalette::Text, disabled);
    menu.setColor(QPalette::Disabled, QPalette::Highlight,
                    QColor(qt_colorref2qrgb(GetSysColor(
                                            (QSysInfo::WindowsVersion >= QSysInfo::WV_XP
                                            && QSysInfo::WindowsVersion < QSysInfo::WV_NT_based)
                                            && isFlat ? COLOR_MENUHILIGHT
                                                        : COLOR_HIGHLIGHT))));
    menu.setColor(QPalette::Disabled, QPalette::HighlightedText, disabled);
    menu.setColor(QPalette::Disabled, QPalette::Button,
                    menu.color(QPalette::Active, QPalette::Button));
    menu.setColor(QPalette::Inactive, QPalette::Button,
                    menu.color(QPalette::Active, QPalette::Button));
    menu.setColor(QPalette::Inactive, QPalette::Text,
                    menu.color(QPalette::Active, QPalette::Text));
    menu.setColor(QPalette::Inactive, QPalette::WindowText,
                    menu.color(QPalette::Active, QPalette::WindowText));
    menu.setColor(QPalette::Inactive, QPalette::ButtonText,
                    menu.color(QPalette::Active, QPalette::ButtonText));
    menu.setColor(QPalette::Inactive, QPalette::Highlight,
                    menu.color(QPalette::Active, QPalette::Highlight));
    menu.setColor(QPalette::Inactive, QPalette::HighlightedText,
                    menu.color(QPalette::Active, QPalette::HighlightedText));

    if (QSysInfo::WindowsVersion != QSysInfo::WV_NT && QSysInfo::WindowsVersion != QSysInfo::WV_95)
        menu.setColor(QPalette::Inactive, QPalette::ButtonText,
                        pal.color(QPalette::Inactive, QPalette::Dark));
    QApplication::setPalette(menu, "QMenu");

    if ((QSysInfo::WindowsVersion >= QSysInfo::WV_XP
        && QSysInfo::WindowsVersion < QSysInfo::WV_NT_based) && isFlat) {
        QColor menubar(qt_colorref2qrgb(GetSysColor(COLOR_MENUBAR)));
        menu.setColor(QPalette::Active, QPalette::Button, menubar);
        menu.setColor(QPalette::Disabled, QPalette::Button, menubar);
        menu.setColor(QPalette::Inactive, QPalette::Button, menubar);
    }
    QApplication::setPalette(menu, "QMenuBar");
}

/*****************************************************************************
  qt_init() - initializes Qt for Windows
 *****************************************************************************/

void qt_init(QApplicationPrivate *priv, int)
{

    int argc = priv->argc;
    char **argv = priv->argv;
    int i, j;

  // Get command line params

    j = argc ? 1 : 0;
    for (i=1; i<argc; i++) {
        if (argv[i] && *argv[i] != '-') {
            argv[j++] = argv[i];
            continue;
        }
#if defined(QT_DEBUG)
        if (qstrcmp(argv[i], "-nograb") == 0)
            appNoGrab = !appNoGrab;
        else
#endif // QT_DEBUG
        if (qstrcmp(argv[i], "-direct3d") == 0)
            QApplication::setAttribute(Qt::AA_MSWindowsUseDirect3DByDefault);
        else
            argv[j++] = argv[i];
    }
    if(j < priv->argc) {
        priv->argv[j] = 0;
        priv->argc = j;
    }

    // Get the application name/instance if qWinMain() was not invoked
#ifndef Q_OS_WINCE
    // No message boxes but important ones
    SetErrorMode(SetErrorMode(0) | SEM_FAILCRITICALERRORS|SEM_NOOPENFILEERRORBOX);
#endif

    if (appInst == 0) {
        QT_WA({
            appInst = GetModuleHandle(0);
        }, {
            appInst = GetModuleHandleA(0);
        });
    }

#ifndef Q_OS_WINCE
    // Initialize OLE/COM
    //         S_OK means success and S_FALSE means that it has already
    //         been initialized
    HRESULT r;
    r = OleInitialize(0);
    if (r != S_OK && r != S_FALSE) {
        qWarning("Qt: Could not initialize OLE (error %x)", (unsigned int)r);
    }
#endif

    // Misc. initialization
#if defined(QT_DEBUG) && !defined(Q_OS_WINCE)
    GdiSetBatchLimit(1);
#endif

    // initialize key mapper
    QKeyMapper::changeKeyboard();

    QColormap::initialize();
    QFont::initialize();
#ifndef QT_NO_CURSOR
    QCursorData::initialize();
#endif
    qApp->setObjectName(QLatin1String(theAppName));

#if !defined(Q_OS_WINCE)
    // default font
    HFONT hfont = (HFONT)GetStockObject(DEFAULT_GUI_FONT);
    QFont f(QLatin1String("MS Sans Serif"),8);
    int result = 0;
    QT_WA({
            LOGFONT lf;
            if (result = GetObject(hfont, sizeof(lf), &lf))
                f = qt_LOGFONTtoQFont((LOGFONT&)lf,true);
        } , {
              LOGFONTA lf;
              if (result = GetObjectA(hfont, sizeof(lf), &lf))
                  f = qt_LOGFONTtoQFont((LOGFONT&)lf,true);
          });
    if (result
        && QSysInfo::WindowsVersion >= QSysInfo::WV_2000
        && QSysInfo::WindowsVersion <= QSysInfo::WV_NT_based
        && f.family() == QLatin1String("MS Shell Dlg"))
        f.setFamily(QLatin1String("MS Shell Dlg 2"));
    QApplicationPrivate::setSystemFont(f);
#else //Q_OS_WINCE
        LOGFONT lf;
        HGDIOBJ stockFont = GetStockObject(SYSTEM_FONT);
        int result = 0;
        result = GetObject(stockFont, sizeof(lf), &lf);
        QFont font = qt_LOGFONTtoQFont(lf, true);
        if (result)
          QApplicationPrivate::setSystemFont(font);
#endif //Q_OS_WINCE

    // QFont::locale_init();  ### Uncomment when it does something on Windows

    if (QApplication::desktopSettingsAware())
        qt_set_windows_resources();

    QT_WA({
        WM95_MOUSEWHEEL = RegisterWindowMessage(L"MSWHEEL_ROLLMSG");
    } , {
        WM95_MOUSEWHEEL = RegisterWindowMessageA("MSWHEEL_ROLLMSG");
    });
    initWinTabFunctions();
    QApplicationPrivate::inputContext = new QWinInputContext(0);
}

/*****************************************************************************
  qt_cleanup() - cleans up when the application is finished
 *****************************************************************************/

void qt_cleanup()
{
    unregWinClasses();
    QPixmapCache::clear();

#ifndef QT_NO_CURSOR
    QCursorData::cleanup();
#endif
    QFont::cleanup();
    QColormap::cleanup();
    if (displayDC) {
        ReleaseDC(0, displayDC);
        displayDC = 0;
    }

    delete QApplicationPrivate::inputContext;
    QApplicationPrivate::inputContext = 0;

#ifndef Q_OS_WINCE
  // Deinitialize OLE/COM
    OleUninitialize();
#endif
}


/*****************************************************************************
  Platform specific global and internal functions
 *****************************************************************************/

Q_GUI_EXPORT int qWinAppCmdShow()                        // get main window show command
{
    return appCmdShow;
}


Q_GUI_EXPORT HDC qt_win_display_dc()                        // get display DC
{
    Q_ASSERT(qApp && qApp->thread() == QThread::currentThread());
    if (!displayDC)
        displayDC = GetDC(0);
    return displayDC;
}

bool qt_nograb()                                // application no-grab option
{
#if defined(QT_DEBUG)
    return appNoGrab;
#else
    return false;
#endif
}

typedef QHash<QString, int> WinClassNameHash;
Q_GLOBAL_STATIC(WinClassNameHash, winclassNames)

const QString qt_reg_winclass(QWidget *w)        // register window class
{
    int flags = w->windowFlags();
    int type = flags & Qt::WindowType_Mask;

    uint style;
    bool icon;
    QString cname;
    if (flags & Qt::MSWindowsOwnDC) {
        cname = QLatin1String("QWidgetOwnDC");
        style = CS_DBLCLKS;
#ifndef Q_OS_WINCE
        style |= CS_OWNDC;
#endif
        icon  = true;
    } else if (type == Qt::Tool || type == Qt::ToolTip){
        style = CS_DBLCLKS;
        if (w->inherits("QTipLabel") || w->inherits("QAlphaWidget")) {
            if ((QSysInfo::WindowsVersion >= QSysInfo::WV_XP
                && QSysInfo::WindowsVersion < QSysInfo::WV_NT_based)) {
                style |= 0x00020000;                // CS_DROPSHADOW
            }
            cname = QLatin1String("QToolTip");
        } else {
            cname = QLatin1String("QTool");
        }
#ifndef Q_OS_WINCE
        style |= CS_SAVEBITS;
#endif
        icon = false;
    } else if (type == Qt::Popup) {
        cname = QLatin1String("QPopup");
        style = CS_DBLCLKS;
#ifndef Q_OS_WINCE
        style |= CS_SAVEBITS;
#endif
        if ((QSysInfo::WindowsVersion >= QSysInfo::WV_XP
            && QSysInfo::WindowsVersion < QSysInfo::WV_NT_based))
            style |= 0x00020000;                // CS_DROPSHADOW
        icon = false;
    } else {
        cname = QLatin1String("QWidget");
        style = CS_DBLCLKS;
        icon  = true;
    }

#ifdef Q_OS_WINCE
    // We need to register the classes with the
    // unique ID on WinCE to make sure we can
    // move the windows to the front when starting
    // a second instance.
    wchar_t uniqueAppID[256];
    GetModuleFileNameW(0, uniqueAppID, 255);
    cname = QString::number(RegisterWindowMessageW(
              (const wchar_t *) QString::fromUtf16((const ushort *)uniqueAppID).toLower().replace(QString(QString::fromLatin1("\\")), 
              QString(QString::fromLatin1("_"))).utf16()));
#endif

    // since multiple Qt versions can be used in one process
    // each one has to have window class names with a unique name
    // The first instance gets the unmodified name; if the class
    // has already been registered by another instance of Qt then
    // add an instance-specific ID, the address of the window proc.
    static int classExists = -1;

    if (classExists == -1) {
        QT_WA({
            WNDCLASS wcinfo;
            classExists = GetClassInfo((HINSTANCE)qWinAppInst(), (TCHAR*)cname.utf16(), &wcinfo);
            classExists = classExists && wcinfo.lpfnWndProc != QtWndProc;
        }, {
            WNDCLASSA wcinfo;
            classExists = GetClassInfoA((HINSTANCE)qWinAppInst(), cname.toLatin1(), &wcinfo);
            classExists = classExists && wcinfo.lpfnWndProc != QtWndProc;
        });
    }

    if (classExists)
        cname += QString::number((quintptr)QtWndProc);

    if (winclassNames()->contains(cname))        // already registered in our list
        return cname;

    ATOM atom;
#ifndef Q_OS_WINCE
    HBRUSH bgBrush = (HBRUSH)GetSysColorBrush(COLOR_WINDOW);
    QT_WA({
        WNDCLASS wc;
        wc.style        = style;
        wc.lpfnWndProc        = (WNDPROC)QtWndProc;
        wc.cbClsExtra        = 0;
        wc.cbWndExtra        = 0;
        wc.hInstance        = (HINSTANCE)qWinAppInst();
        if (icon) {
            wc.hIcon = LoadIcon(appInst, L"IDI_ICON1");
            if (!wc.hIcon)
                wc.hIcon = LoadIcon(0, IDI_APPLICATION);
        } else {
            wc.hIcon = 0;
        }
        wc.hCursor        = 0;
        wc.hbrBackground= bgBrush;
        wc.lpszMenuName        = 0;
        wc.lpszClassName= (TCHAR*)cname.utf16();
        atom = RegisterClass(&wc);
    } , {
        WNDCLASSA wc;
        wc.style        = style;
        wc.lpfnWndProc        = (WNDPROC)QtWndProc;
        wc.cbClsExtra        = 0;
        wc.cbWndExtra        = 0;
        wc.hInstance        = (HINSTANCE)qWinAppInst();
        if (icon) {
            wc.hIcon = LoadIconA(appInst, (char*)"IDI_ICON1");
            if (!wc.hIcon)
                wc.hIcon = LoadIconA(0, (char*)IDI_APPLICATION);
        } else {
            wc.hIcon = 0;
        }
        wc.hCursor        = 0;
        wc.hbrBackground= bgBrush;
        wc.lpszMenuName        = 0;
          QByteArray tempArray = cname.toLatin1();
        wc.lpszClassName= tempArray;
        atom = RegisterClassA(&wc);
    });
#else
        WNDCLASS wc;
        wc.style        = style;
        wc.lpfnWndProc        = (WNDPROC)QtWndProc;
        wc.cbClsExtra        = 0;
        wc.cbWndExtra        = 0;
        wc.hInstance        = (HINSTANCE)qWinAppInst();
        if (icon) {
            wc.hIcon = LoadIcon(appInst, L"IDI_ICON1");
//            if (!wc.hIcon)
//                wc.hIcon = LoadIcon(0, IDI_APPLICATION);
        } else {
            wc.hIcon = 0;
        }
        wc.hCursor        = 0;
        wc.hbrBackground= 0;
        wc.lpszMenuName        = 0;
        wc.lpszClassName= (TCHAR*)cname.utf16();
        atom = RegisterClass(&wc);
#endif

#ifndef QT_NO_DEBUG
    if (!atom)
        qErrnoWarning("QApplication::regClass: Registering window class failed.");
#endif

    winclassNames()->insert(cname, 1);
    return cname;
}

static void unregWinClasses()
{
    WinClassNameHash *hash = winclassNames();
    QHash<QString, int>::ConstIterator it = hash->constBegin();
    while (it != hash->constEnd()) {
        QT_WA({
            UnregisterClass((TCHAR*)it.key().utf16(), (HINSTANCE)qWinAppInst());
        } , {
            UnregisterClassA(it.key().toLatin1(), (HINSTANCE)qWinAppInst());
        });
        ++it;
    }
    hash->clear();
}


/*****************************************************************************
  Safe configuration (move,resize,setGeometry) mechanism to avoid
  recursion when processing messages.
 *****************************************************************************/

struct QWinConfigRequest {
    WId         id;                                        // widget to be configured
    int         req;                                        // 0=move, 1=resize, 2=setGeo
    int         x, y, w, h;                                // request parameters
};

static QList<QWinConfigRequest*> *configRequests = 0;

void qWinRequestConfig(WId id, int req, int x, int y, int w, int h)
{
    if (!configRequests)                        // create queue
        configRequests = new QList<QWinConfigRequest*>;
    QWinConfigRequest *r = new QWinConfigRequest;
    r->id = id;                                        // create new request
    r->req = req;
    r->x = x;
    r->y = y;
    r->w = w;
    r->h = h;
    configRequests->append(r);                // store request in queue
}

Q_GUI_EXPORT void qWinProcessConfigRequests()                // perform requests in queue
{
    if (!configRequests)
        return;
    QWinConfigRequest *r;
    for (;;) {
        if (configRequests->isEmpty())
            break;
        r = configRequests->takeLast();
        QWidget *w = QWidget::find(r->id);
        QRect rect(r->x, r->y, r->w, r->h);
        int req = r->req;
        delete r;

        if ( w ) {                              // widget exists
            if (w->testAttribute(Qt::WA_WState_ConfigPending))
                return;                         // biting our tail
            if (req == 0)
                w->move(rect.topLeft());
            else if (req == 1)
                w->resize(rect.size());
            else
                w->setGeometry(rect);
        }
    }
    delete configRequests;
    configRequests = 0;
}


/*****************************************************************************
    GUI event dispatcher
 *****************************************************************************/

class QGuiEventDispatcherWin32 : public QEventDispatcherWin32
{
    Q_DECLARE_PRIVATE(QEventDispatcherWin32)
public:
    QGuiEventDispatcherWin32(QObject *parent = 0);
    bool processEvents(QEventLoop::ProcessEventsFlags flags);
};

QGuiEventDispatcherWin32::QGuiEventDispatcherWin32(QObject *parent)
    : QEventDispatcherWin32(parent)
{
    createInternalHwnd();
}

bool QGuiEventDispatcherWin32::processEvents(QEventLoop::ProcessEventsFlags flags)
{
    if (!QEventDispatcherWin32::processEvents(flags))
        return false;

    if (configRequests)                        // any pending configs?
        qWinProcessConfigRequests();

    return true;
}

void QApplicationPrivate::createEventDispatcher()
{
    Q_Q(QApplication);
    if (q->type() != QApplication::Tty)
        eventDispatcher = new QGuiEventDispatcherWin32(q);
    else
        eventDispatcher = new QEventDispatcherWin32(q);
}

/*****************************************************************************
  Platform specific QApplication members
 *****************************************************************************/

#ifdef QT3_SUPPORT
void QApplication::setMainWidget(QWidget *mainWidget)
{
    QApplicationPrivate::main_widget = mainWidget;
    if (QApplicationPrivate::main_widget && windowIcon().isNull()
        && QApplicationPrivate::main_widget->testAttribute(Qt::WA_SetWindowIcon))
        setWindowIcon(QApplicationPrivate::main_widget->windowIcon());
}
#endif

#ifndef QT_NO_CURSOR

/*****************************************************************************
  QApplication cursor stack
 *****************************************************************************/

void QApplication::setOverrideCursor(const QCursor &cursor)
{
    qApp->d_func()->cursor_list.prepend(cursor);
    SetCursor(qApp->d_func()->cursor_list.first().handle());
}

void QApplication::restoreOverrideCursor()
{
    if (qApp->d_func()->cursor_list.isEmpty())
        return;
    qApp->d_func()->cursor_list.removeFirst();

    if (!qApp->d_func()->cursor_list.isEmpty()) {
        SetCursor(qApp->d_func()->cursor_list.first().handle());
    } else {
        QWidget *w = QWidget::find(curWin);
        if (w)
            SetCursor(w->cursor().handle());
        else
            SetCursor(QCursor(Qt::ArrowCursor).handle());
    }
}

#endif

/*
  Internal function called from QWidget::setCursor()
   force is true if this function is called from dispatchEnterLeave, it means that the
   mouse is actually directly under this widget.
*/

#ifndef QT_NO_CURSOR
void qt_win_set_cursor(QWidget *w, bool force)
{
    static QPointer<QWidget> lastUnderMouse = 0;
    if (force) {
        lastUnderMouse = w;
    } else if (w->testAttribute(Qt::WA_WState_Created) && lastUnderMouse
               && lastUnderMouse->effectiveWinId() == w->effectiveWinId()) {
        w = lastUnderMouse;
    }

    if (!curWin && w && w->internalWinId())
        return;
    QWidget* cW = w && !w->internalWinId() ? w : QWidget::find(curWin);
    if (!cW || cW->window() != w->window() ||
         !cW->isVisible() || !cW->underMouse() || QApplication::overrideCursor())
        return;

    SetCursor(cW->cursor().handle());
}
#endif // QT_NO_CURSOR

Qt::KeyboardModifiers qt_win_getKeyboardModifiers()
{
    Qt::KeyboardModifiers modifiers = Qt::NoModifier;
    if (GetKeyState(VK_SHIFT) < 0)
        modifiers |= Qt::ShiftModifier;
    if (GetKeyState(VK_CONTROL) < 0)
        modifiers |= Qt::ControlModifier;
    if (GetKeyState(VK_MENU) < 0)
        modifiers |= Qt::AltModifier;
    return modifiers;
}

/*****************************************************************************
  Routines to find a Qt widget from a screen position
 *****************************************************************************/

QWidget *QApplication::topLevelAt(const QPoint &pos)
{
    POINT p;
    HWND  win;
    QWidget *w;
    p.x = pos.x();
    p.y = pos.y();
    win = WindowFromPoint(p);
    if (!win)
        return 0;

    w = QWidget::find(win);
    while (!w && win) {
        win = GetParent(win);
        w = QWidget::find(win);
    }
    return w ? w->window() : 0;
}

void QApplication::beep()
{
    MessageBeep(MB_OK);
}

static void alert_widget(QWidget *widget, int duration)
{
    bool stopFlash = duration < 0;

    if (!pFlashWindowEx) {
#ifndef Q_OS_WINCE
        QLibrary themeLib(QLatin1String("user32"));
        pFlashWindowEx  = (PtrFlashWindowEx)themeLib.resolve("FlashWindowEx");
#endif
    }

    if (pFlashWindowEx && widget && (!widget->isActiveWindow() || stopFlash)) {
        DWORD timeOut = GetCaretBlinkTime();
        if (timeOut <= 0)
            timeOut = 250;

        UINT flashCount;
        if (duration == 0)
            flashCount = 10;
        else
            flashCount = duration/timeOut;

        FLASHWINFO info;
        info.cbSize = sizeof(info);
        info.hwnd = widget->window()->winId();
        info.dwFlags = stopFlash ? FLASHW_STOP : FLASHW_TRAY;
        info.dwTimeout = timeOut;
        info.uCount = flashCount;

        pFlashWindowEx(&info);
    }
}

void QApplication::alert(QWidget *widget, int duration)
{
    if (!QApplicationPrivate::checkInstance("alert"))
        return;

    if (widget) {
        alert_widget(widget, duration);
    } else {
        const QWidgetList toplevels(topLevelWidgets());
        for (int i = 0; i < toplevels.count(); ++i) {
            QWidget *topLevel = toplevels.at(i);
            alert_widget(topLevel, duration);
        }
    }
}

QString QApplicationPrivate::appName() const
{
    return QCoreApplicationPrivate::appName();
}


/*****************************************************************************
  Main event loop
 *****************************************************************************/

extern uint qGlobalPostedEventsCount();

/*!
    \internal
    \since 4.1

    If \a gotFocus is true, \a widget will become the active window.
    Otherwise the active window is reset to 0.
*/
void QApplication::winFocus(QWidget *widget, bool gotFocus)
{
    if (d_func()->inPopupMode()) // some delayed focus event to ignore
        return;
    if (gotFocus) {
        setActiveWindow(widget);
        if (QApplicationPrivate::active_window
            && (QApplicationPrivate::active_window->windowType() == Qt::Dialog)) {
            // raise the entire application, not just the dialog
            QWidget* mw = QApplicationPrivate::active_window;
            while(mw->parentWidget() && (mw->windowType() == Qt::Dialog))
                mw = mw->parentWidget()->window();
            if (mw->testAttribute(Qt::WA_WState_Created) && mw != QApplicationPrivate::active_window)
                SetWindowPos(mw->internalWinId(), HWND_TOP, 0, 0, 0, 0, SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOSIZE);
        }
    } else {
        setActiveWindow(0);
    }
}


//
// QtWndProc() receives all messages from the main event loop
//

static bool inLoop = false;
static int inputcharset = CP_ACP;

#define RETURN(x) { inLoop=false;return x; }

bool qt_sendSpontaneousEvent(QObject *receiver, QEvent *event)
{
    return QCoreApplication::sendSpontaneousEvent(receiver, event);
}

static bool qt_is_translatable_mouse_event(UINT message)
{
    return (message >= WM_MOUSEFIRST && message <= WM_MOUSELAST ||
                message >= WM_XBUTTONDOWN && message <= WM_XBUTTONDBLCLK)
            && message != WM_MOUSEWHEEL

#ifndef Q_OS_WINCE
            || message >= WM_NCMOUSEMOVE && message <= WM_NCMBUTTONDBLCLK
#endif
            ;
}

extern "C"
LRESULT CALLBACK QtWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    bool result = true;
    QEvent::Type evt_type = QEvent::None;
    QETWidget *widget = 0;

        // there is no need to process pakcets from tablet unless
        // it is actually on the tablet, a flag to let us know...
        int nPackets;        // the number of packets we get from the queue

    long res = 0;
    if (!qApp)                                // unstable app state
        RETURN(QWinInputContext::DefWindowProc(hwnd,message,wParam,lParam))

    QScopedLoopLevelCounter loopLevelCounter(QThreadData::get2(qApp->thread()));

#if 0
    // make sure we update widgets also when the user resizes
    if (inLoop && qApp->loopLevel())
        qApp->sendPostedEvents(0, QEvent::Paint);
#endif

    inLoop = true;

    MSG msg;
    msg.hwnd = hwnd;                                // create MSG structure
    msg.message = message;                        // time and pt fields ignored
    msg.wParam = wParam;
    msg.lParam = lParam;
    msg.pt.x = GET_X_LPARAM(lParam);
    msg.pt.y = GET_Y_LPARAM(lParam);
    // If it's a non-client-area message the coords are screen coords, otherwise they are
    // client coords.
#ifndef Q_OS_WINCE
    if (message < WM_NCMOUSEMOVE || message > WM_NCMBUTTONDBLCLK)
#endif
        ClientToScreen(msg.hwnd, &msg.pt);

    /*
    // sometimes the autograb is not released, so the clickevent is sent
    // to the wrong window. We ignore this for now, because it doesn't
    // cause any problems.
    if (msg.message == WM_LBUTTONDOWN || msg.message == WM_RBUTTONDOWN || msg.message == WM_MBUTTONDOWN) {
        HWND handle = WindowFromPoint(msg.pt);
        if (msg.hwnd != handle) {
            msg.hwnd = handle;
            hwnd = handle;
        }
    }
    */

#if defined(QT_NON_COMMERCIAL)
    QT_NC_WNDPROC
#endif

    // send through app filter
    if (qApp->filterEvent(&msg, &res))
        return res;

    // close any opened ime candidate window (enabled only on a popup widget)
    if (imeParentWnd  && QApplication::activePopupWidget()
        && (message == WM_MBUTTONDOWN || message == WM_XBUTTONDOWN 
        || message == WM_LBUTTONDOWN || message == WM_RBUTTONDOWN 
#ifndef Q_OS_WINCE
        || message == WM_NCMBUTTONDOWN || message == WM_NCLBUTTONDOWN 
        || message == WM_NCRBUTTONDOWN)) {
#else 
                                      )) {
#endif
            ::SendMessage(imeParentWnd, WM_IME_ENDCOMPOSITION, 0, 0);
    }

    switch (message) {
#ifndef Q_OS_WINCE
    case WM_QUERYENDSESSION: {
        if (sm_smActive) // bogus message from windows
            RETURN(true);

        sm_smActive = true;
        sm_blockUserInput = true; // prevent user-interaction outside interaction windows
        sm_cancel = false;
        if (qt_session_manager_self)
            qApp->commitData(*qt_session_manager_self);
        if (lParam & ENDSESSION_LOGOFF) {
            _flushall();
        }
        RETURN(!sm_cancel);
    }
    case WM_ENDSESSION: {
        sm_smActive = false;
        sm_blockUserInput = false;
        bool endsession = (bool) wParam;

        if (endsession) {
            // since the process will be killed immediately quit() has no real effect
            int index = QApplication::staticMetaObject.indexOfSignal("aboutToQuit()");
            qApp->qt_metacall(QMetaObject::InvokeMetaMethod, index,0);
            qApp->quit();
        }

        RETURN(0);
    }
    case WM_DISPLAYCHANGE:
        if (qApp->type() == QApplication::Tty)
            break;
        if (qt_desktopWidget) {
            qt_desktopWidget->move(GetSystemMetrics(76), GetSystemMetrics(77));
            QSize sz(GetSystemMetrics(78), GetSystemMetrics(79));
            if (sz == qt_desktopWidget->size()) {
                 // a screen resized without changing size of the virtual desktop
                QResizeEvent rs(sz, qt_desktopWidget->size());
                QApplication::sendEvent(qt_desktopWidget, &rs);
            } else {
                qt_desktopWidget->resize(sz);
            }
        }
        break;
#endif

    case WM_SETTINGCHANGE:
#ifdef Q_OS_WINCE
        // CE SIP hide/show
        if (wParam == SPI_SETSIPINFO) {
            QResizeEvent re(QSize(0, 0), QSize(0, 0)); // Calculated by QDesktopWidget
            QApplication::sendEvent(qt_desktopWidget, &re);
            break;
        }
#endif
        // ignore spurious XP message when user logs in again after locking
        if (qApp->type() == QApplication::Tty)
            break;
        if (QApplication::desktopSettingsAware() && wParam != SPI_SETWORKAREA) {
            widget = (QETWidget*)QWidget::find(hwnd);
            if (widget) {
                if (wParam == SPI_SETNONCLIENTMETRICS)
                    widget->markFrameStrutDirty();
            }
        }
        else if (qt_desktopWidget && wParam == SPI_SETWORKAREA) {
            qt_desktopWidget->move(GetSystemMetrics(76), GetSystemMetrics(77));
            QSize sz(GetSystemMetrics(78), GetSystemMetrics(79));
            if (sz == qt_desktopWidget->size()) {
                 // a screen resized without changing size of the virtual desktop
                QResizeEvent rs(sz, qt_desktopWidget->size());
                QApplication::sendEvent(qt_desktopWidget, &rs);
            } else {
                qt_desktopWidget->resize(sz);
            }
        }

        break;
    case WM_SYSCOLORCHANGE:
        if (qApp->type() == QApplication::Tty)
            break;
        if (QApplication::desktopSettingsAware()) {
            widget = (QETWidget*)QWidget::find(hwnd);
            if (widget && !widget->parentWidget())
                qt_set_windows_color_resources();
        }
        break;

    case WM_LBUTTONDOWN: 
#if defined(Q_OS_WINCE)
        {
            QWidget *widget = QWidget::find(hwnd);
            if (widget && (widget != qApp->activePopupWidget())) {
                QPoint pos = QPoint(short(LOWORD(lParam)), short(HIWORD(lParam)));
                QPoint globalPos = widget->mapToGlobal(pos);
                // In case we are using Alien, then we the widget to
                // send the context menu event is a different one
                if (!widget->testAttribute(Qt::WA_NativeWindow) && !widget->testAttribute(Qt::WA_PaintOnScreen)) {
                    widget = QApplication::widgetAt(globalPos);
                    pos = widget->mapFromGlobal(globalPos);
                }
                SHRGINFO    shrg;
                shrg.cbSize = sizeof(shrg);
                shrg.hwndClient = hwnd;
                shrg.ptDown.x = LOWORD(lParam);
                shrg.ptDown.y = HIWORD(lParam);
                shrg.dwFlags = SHRG_RETURNCMD | SHRG_NOANIMATION;
                resolveAygLibs();
                if (ptrRecognizeGesture && (ptrRecognizeGesture(&shrg) == GN_CONTEXTMENU)) {
                    if (qApp->activePopupWidget())
                        qApp->activePopupWidget()->close();
                    QContextMenuEvent e(QContextMenuEvent::Mouse, pos, globalPos);
                    result = qt_sendSpontaneousEvent(widget, &e);
                }
            }
        }
#endif
    case WM_MBUTTONDOWN:
    case WM_RBUTTONDOWN:
    case WM_XBUTTONDOWN:
        if (qt_win_ignoreNextMouseReleaseEvent)
            qt_win_ignoreNextMouseReleaseEvent = false;
        break;

    case WM_LBUTTONUP:
    case WM_MBUTTONUP:
    case WM_RBUTTONUP:
    case WM_XBUTTONUP:
        if (qt_win_ignoreNextMouseReleaseEvent) {
            qt_win_ignoreNextMouseReleaseEvent = false;
            if (qt_button_down && qt_button_down->internalWinId() == autoCaptureWnd) {
                releaseAutoCapture();
                qt_button_down = 0;
            }

            RETURN(0);
        }
        break;

    default:
        break;
    }

    if (!widget)
        widget = (QETWidget*)QWidget::find(hwnd);
    if (!widget)                                // don't know this widget
        goto do_default;

    if (app_do_modal)        {                        // modal event handling
        int ret = 0;
        if (!qt_try_modal(widget, &msg, ret))
            RETURN(ret);
    }

    res = 0;
    if (widget->winEvent(&msg, &res))                // send through widget filter
        RETURN(res);

    if (qt_is_translatable_mouse_event(message)) {
        if (qApp->activePopupWidget() != 0) { // in popup mode
            POINT curPos = msg.pt;
            QWidget* w = QApplication::widgetAt(curPos.x, curPos.y);
            if (w)
                widget = (QETWidget*)w;
        }

        if (!qt_tabletChokeMouse) {
            result = widget->translateMouseEvent(msg);        // mouse event
        } else {
            // Sometimes we only get a WM_MOUSEMOVE message
            // and sometimes we get both a WM_MOUSEMOVE and
            // a WM_LBUTTONDOWN/UP, this creates a spurious mouse
            // press/release event, using the winPeekMessage
            // will help us fix this.  This leaves us with a
            // question:
            //    This effectively kills using the mouse AND the
            //    tablet simultaneously, well creates wacky input.
            //    Is this going to be a problem? (probably not)
            bool next_is_button = false;
            bool is_mouse_move = (message == WM_MOUSEMOVE);
            if (is_mouse_move) {
                MSG msg1;
                if (winPeekMessage(&msg1, msg.hwnd, WM_MOUSEFIRST,
                                    WM_MOUSELAST, PM_NOREMOVE))
                    next_is_button = (msg1.message == WM_LBUTTONUP
                                       || msg1.message == WM_LBUTTONDOWN);
            }
            if (!is_mouse_move || (is_mouse_move && !next_is_button))
                qt_tabletChokeMouse = false;
        }
    } else if (message == WM95_MOUSEWHEEL) {
        result = widget->translateWheelEvent(msg);
    } else {
        switch (message) {
        case WM_KEYDOWN:                        // keyboard event
        case WM_SYSKEYDOWN:
            qt_keymapper_private()->updateKeyMap(msg);
            // fall-through intended
        case WM_KEYUP:
        case WM_SYSKEYUP:
        case WM_IME_CHAR:
        case WM_IME_KEYDOWN:
        case WM_CHAR: {
            MSG msg1;
            bool anyMsg = winPeekMessage(&msg1, msg.hwnd, 0, 0, PM_NOREMOVE);
            if (anyMsg && msg1.message == WM_DEADCHAR) {
                result = true; // consume event since there is a dead char next
                break;
            }
            QWidget *g = QWidget::keyboardGrabber();
            if (g && qt_get_tablet_widget() && hwnd == qt_get_tablet_widget()->winId()) {
                // if we get an event for the internal tablet widget,
                // then don't send it to the keyboard grabber, but
                // send it to the widget itself (we don't use it right
                // now, just in case).
                g = 0;
            }
            if (g)
                widget = (QETWidget*)g;
            else if (QApplication::activePopupWidget())
                widget = (QETWidget*)QApplication::activePopupWidget()->focusWidget()
                       ? (QETWidget*)QApplication::activePopupWidget()->focusWidget()
                       : (QETWidget*)QApplication::activePopupWidget();
            else if (qApp->focusWidget())
                widget = (QETWidget*)QApplication::focusWidget();
            else if (!widget || widget->internalWinId() == GetFocus()) // We faked the message to go to exactly that widget.
                widget = (QETWidget*)widget->window();
            if (widget->isEnabled())
                result = sm_blockUserInput
                            ? true
                            : qt_keymapper_private()->translateKeyEvent(widget, msg, g != 0);
            break;
        }
        case WM_SYSCHAR:
            result = true;                        // consume event
            break;

        case WM_MOUSEWHEEL:
            result = widget->translateWheelEvent(msg);
            break;

        case WM_APPCOMMAND:
            {
                uint cmd = GET_APPCOMMAND_LPARAM(lParam);
                uint uDevice = GET_DEVICE_LPARAM(lParam);
                uint dwKeys = GET_KEYSTATE_LPARAM(lParam);

                int state = translateButtonState(dwKeys, QEvent::KeyPress, 0);

                switch (uDevice) {
                case FAPPCOMMAND_KEY:
                    {
                        int key = 0;

                        switch(cmd) {
                        case APPCOMMAND_BASS_BOOST:
                            key = Qt::Key_BassBoost;
                            break;
                        case APPCOMMAND_BASS_DOWN:
                            key = Qt::Key_BassDown;
                            break;
                        case APPCOMMAND_BASS_UP:
                            key = Qt::Key_BassUp;
                            break;
                        case APPCOMMAND_BROWSER_BACKWARD:
                            key = Qt::Key_Back;
                            break;
                        case APPCOMMAND_BROWSER_FAVORITES:
                            key = Qt::Key_Favorites;
                            break;
                        case APPCOMMAND_BROWSER_FORWARD:
                            key = Qt::Key_Forward;
                            break;
                        case APPCOMMAND_BROWSER_HOME:
                            key = Qt::Key_HomePage;
                            break;
                        case APPCOMMAND_BROWSER_REFRESH:
                            key = Qt::Key_Refresh;
                            break;
                        case APPCOMMAND_BROWSER_SEARCH:
                            key = Qt::Key_Search;
                            break;
                        case APPCOMMAND_BROWSER_STOP:
                            key = Qt::Key_Stop;
                            break;
                        case APPCOMMAND_LAUNCH_APP1:
                            key = Qt::Key_Launch0;
                            break;
                        case APPCOMMAND_LAUNCH_APP2:
                            key = Qt::Key_Launch1;
                            break;
                        case APPCOMMAND_LAUNCH_MAIL:
                            key = Qt::Key_LaunchMail;
                            break;
                        case APPCOMMAND_LAUNCH_MEDIA_SELECT:
                            key = Qt::Key_LaunchMedia;
                            break;
                        case APPCOMMAND_MEDIA_NEXTTRACK:
                            key = Qt::Key_MediaNext;
                            break;
                        case APPCOMMAND_MEDIA_PLAY_PAUSE:
                            key = Qt::Key_MediaPlay;
                            break;
                        case APPCOMMAND_MEDIA_PREVIOUSTRACK:
                            key = Qt::Key_MediaPrevious;
                            break;
                        case APPCOMMAND_MEDIA_STOP:
                            key = Qt::Key_MediaStop;
                            break;
                        case APPCOMMAND_TREBLE_DOWN:
                            key = Qt::Key_TrebleDown;
                            break;
                        case APPCOMMAND_TREBLE_UP:
                            key = Qt::Key_TrebleUp;
                            break;
                        case APPCOMMAND_VOLUME_DOWN:
                            key = Qt::Key_VolumeDown;
                            break;
                        case APPCOMMAND_VOLUME_MUTE:
                            key = Qt::Key_VolumeMute;
                            break;
                        case APPCOMMAND_VOLUME_UP:
                            key = Qt::Key_VolumeUp;
                            break;
                        // Commands new in Windows XP
                        case APPCOMMAND_HELP:
                            key = Qt::Key_Help;
                            break;
                        case APPCOMMAND_FIND:
                            key = Qt::Key_Search;
                            break;
                        case APPCOMMAND_PRINT:
                            key = Qt::Key_Print;
                            break;
                        case APPCOMMAND_MEDIA_PLAY:
                            key = Qt::Key_MediaPlay;
                            break;
                        default:
                            break;
                        }
                        if (key) {
                            bool res = false;
                            QWidget *g = QWidget::keyboardGrabber();
                            if (g)
                                widget = (QETWidget*)g;
                            else if (qApp->focusWidget())
                                widget = (QETWidget*)qApp->focusWidget();
                            else
                                widget = (QETWidget*)widget->window();
                            if (widget->isEnabled()) {
                                res = QKeyMapper::sendKeyEvent(widget, g != 0, QEvent::KeyPress, key,
                                                               Qt::KeyboardModifier(state),
                                                               QString(), false, 0, 0, 0, 0);
                            }
                            if (res)
                                return true;
                        }
                    }
                    break;

                default:
                    break;
                }

                result = false;
            }
            break;

#ifndef Q_OS_WINCE
        case WM_NCHITTEST:
            if (widget->isWindow()) {
                QPoint pos = widget->mapFromGlobal(QPoint(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)));
                // don't show resize-cursors for fixed-size widgets
                QRect fs = widget->frameStrut();
                if (!widget->isMinimized()) {
                    if (widget->minimumWidth() == widget->maximumWidth() && (pos.x() < 0 || pos.x() >= widget->width()))
                        break;
                    if (widget->minimumHeight() == widget->maximumHeight() && (pos.y() < -(fs.top() - fs.left()) || pos.y() >= widget->height()))
                        break;
                }
            }

            result = false;
            break;
#endif

        case WM_SYSCOMMAND: {
#ifndef Q_OS_WINCE
            bool window_state_change = false;
            Qt::WindowStates oldstate = Qt::WindowStates(widget->dataPtr()->window_state);
            // MSDN:In WM_SYSCOMMAND messages, the four low-order bits of the wParam parameter are
            // used internally by the system. To obtain the correct result when testing the value of
            // wParam, an application must combine the value 0xFFF0 with the wParam value by using
            // the bitwise AND operator.
            switch(0xfff0 & wParam) {
            case SC_CONTEXTHELP:
#ifndef QT_NO_WHATSTHIS
                QWhatsThis::enterWhatsThisMode();
#endif
                QT_WA({
                    DefWindowProc(hwnd, WM_NCPAINT, 1, 0);
                } , {
                    DefWindowProcA(hwnd, WM_NCPAINT, 1, 0);
                });
                break;
#if defined(QT_NON_COMMERCIAL)
                QT_NC_SYSCOMMAND
#endif
            case SC_MINIMIZE:
                window_state_change = true;
                widget->dataPtr()->window_state |= Qt::WindowMinimized;
                if (widget->isVisible()) {
                    QHideEvent e;
                    qt_sendSpontaneousEvent(widget, &e);
                    widget->hideChildren(true);
                }
                result = false;
                break;
            case SC_MAXIMIZE:
                if(widget->isWindow())
                    widget->topData()->normalGeometry = widget->geometry();
            case SC_RESTORE:
                window_state_change = true;
                if ((0xfff0 & wParam) == SC_MAXIMIZE)
                    widget->dataPtr()->window_state |= Qt::WindowMaximized;
                else if (!widget->isMinimized())
                    widget->dataPtr()->window_state &= ~Qt::WindowMaximized;

                if (widget->isMinimized()) {
                    widget->dataPtr()->window_state &= ~Qt::WindowMinimized;
                    widget->showChildren(true);
                    QShowEvent e;
                    qt_sendSpontaneousEvent(widget, &e);
                }
                result = false;
                break;
            default:
                result = false;
                break;
            }

            if (window_state_change) {
                QWindowStateChangeEvent e(oldstate);
                qt_sendSpontaneousEvent(widget, &e);
            }
#endif

            break;
        }

        case WM_SETTINGCHANGE:
            if ( qApp->type() == QApplication::Tty )
                break;

            if (!msg.wParam) {
                QString area = QT_WA_INLINE(QString::fromUtf16((unsigned short *)msg.lParam),
                                             QString::fromLocal8Bit((char*)msg.lParam));
                if (area == QLatin1String("intl"))
                    QApplication::postEvent(widget, new QEvent(QEvent::LocaleChange));
            }
            else if (msg.wParam == SPI_SETICONTITLELOGFONT) {
                if (qApp->desktopSettingsAware()) {
                    widget = (QETWidget*)QWidget::find(hwnd);
                    if (widget && !widget->parentWidget()) {
                        qt_set_windows_font_resources();
                    }
                }
            }
            break;

        case WM_PAINT:                                // paint event
        case WM_ERASEBKGND:                        // erase window background
            result = widget->translatePaintEvent(msg);
            break;

#ifndef Q_OS_WINCE
        case WM_ENTERSIZEMOVE:
            QApplicationPrivate::inSizeMove = true;
            break;
        case WM_EXITSIZEMOVE:
            QApplicationPrivate::inSizeMove = false;
            break;
#endif
        case WM_MOVE:                                // move window
        case WM_SIZE:                                // resize window
            result = widget->translateConfigEvent(msg);
            break;

        case WM_ACTIVATEAPP:
            if (wParam == FALSE) {
                QApplication::setActiveWindow(0);
                // Another application was activated while our popups are open,
                // then close all popups.  In case some popup refuses to close,
                // we give up after 1024 attempts (to avoid an infinite loop).
                int maxiter = 1024;
                QWidget *popup;
                while ((popup=QApplication::activePopupWidget()) && maxiter--)
                    popup->close();
            }
            break;

        case WM_ACTIVATE:
            if ( qApp->type() == QApplication::Tty )
                break;

            if (ptrWTOverlap && ptrWTEnable) {
                // cooperate with other tablet applications, but when
                // we get focus, I want to use the tablet...
                if (qt_tablet_context && GET_WM_ACTIVATE_STATE(wParam, lParam)) {
                    if (ptrWTEnable(qt_tablet_context, true))
                        ptrWTOverlap(qt_tablet_context, true);
                }
            }
            if (QApplication::activePopupWidget() && LOWORD(wParam) == WA_INACTIVE &&
                QWidget::find((HWND)lParam) == 0) {
                // Another application was activated while our popups are open,
                // then close all popups.  In case some popup refuses to close,
                // we give up after 1024 attempts (to avoid an infinite loop).
                int maxiter = 1024;
                QWidget *popup;
                while ((popup=QApplication::activePopupWidget()) && maxiter--)
                    popup->close();
            }

            if (LOWORD(wParam) != WA_INACTIVE) {
                // WM_ACTIVATEAPP handles the "true" false case, as this is only when the application
                // loses focus. Doing it here would result in the widget getting focus to not know
                // where it got it from; it would simply get a 0 value as the old focus widget.
                if (!(widget->windowState() & Qt::WindowMinimized)) {
                    // Ignore the activate message send by WindowsXP to a minimized window
#ifdef Q_OS_WINCE_WM
                    if  (widget->windowState() & Qt::WindowFullScreen)
                        qt_wince_hide_taskbar(widget->winId());
#endif
                    qApp->winFocus(widget, true);
                    // reset any window alert flashes
                    alert_widget(widget, -1);
                }
            }

            // Windows tries to activate a modally blocked window.
            // This happens when restoring an application after "Show Desktop"
            if (app_do_modal && LOWORD(wParam) == WA_ACTIVE) {
                QWidget *top = 0;
                if (!QApplicationPrivate::tryModalHelper(widget, &top) && top && widget != top && top->isVisible())
                    top->activateWindow();
            }
            break;

#ifndef Q_OS_WINCE
            case WM_MOUSEACTIVATE:
                if (widget->window()->windowType() == Qt::Tool) {
                    QWidget *w = widget;
                    if (!w->window()->focusWidget()) {
                        while (w && (w->focusPolicy() & Qt::ClickFocus) == 0) {
                            if (w->isWindow()) {
                                QWidget *fw = w;
                                while ((fw = fw->nextInFocusChain()) != w && fw->focusPolicy() == Qt::NoFocus)
                                    ;
                                if (fw != w)
                                   break;
                                QWidget *pw = w->parentWidget();
                                while (pw) {
                                    pw = pw->window();
                                    if (pw && pw->isVisible() && pw->focusWidget()) {
                                        Q_ASSERT(pw->testAttribute(Qt::WA_WState_Created));
                                        SetWindowPos(pw->internalWinId(), HWND_TOP, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
                                        break;
                                    }
                                    pw = pw->parentWidget();
                                }
                                RETURN(MA_NOACTIVATE);
                            }
                            w = w->parentWidget();
                        }
                    }
                }
                RETURN(MA_ACTIVATE);
                break;
#endif
            case WM_SHOWWINDOW:
                if (lParam == SW_PARENTOPENING) {
                    if (widget->testAttribute(Qt::WA_WState_Hidden))
                        RETURN(0);
                }
                if (widget->isWindow() && widget->testAttribute(Qt::WA_WState_Visible)
                    && !widget->testWindowState(Qt::WindowMinimized)) {
                    if (lParam == SW_PARENTOPENING) {
                        QShowEvent e;
                        qt_sendSpontaneousEvent(widget, &e);
                        widget->showChildren(true);
                    } else if (lParam == SW_PARENTCLOSING) {
                        QHideEvent e;
                        qt_sendSpontaneousEvent(widget, &e);
                        widget->hideChildren(true);
                    }
                }
                if  (!wParam && autoCaptureWnd == widget->internalWinId())
                    releaseAutoCapture();
                result = false;
                break;

        case WM_PALETTECHANGED:                        // our window changed palette
            if (QColormap::hPal() && (WId)wParam == widget->internalWinId())
                RETURN(0);                        // otherwise: FALL THROUGH!
            // FALL THROUGH
        case WM_QUERYNEWPALETTE:                // realize own palette
            if (QColormap::hPal()) {
                Q_ASSERT(widget->testAttribute(Qt::WA_WState_Created));
                HDC hdc = GetDC(widget->internalWinId());
                HPALETTE hpalOld = SelectPalette(hdc, QColormap::hPal(), FALSE);
                uint n = RealizePalette(hdc);
                if (n)
                    InvalidateRect(widget->internalWinId(), 0, TRUE);
                SelectPalette(hdc, hpalOld, TRUE);
                RealizePalette(hdc);
                ReleaseDC(widget->internalWinId(), hdc);
                RETURN(n);
            }
            break;
        case WM_CLOSE:                                // close window
            widget->translateCloseEvent(msg);
            RETURN(0);                                // always handled

        case WM_DESTROY:                        // destroy window
            if (hwnd == curWin) {
                QWidget *enter = QWidget::mouseGrabber();
                if (enter == widget)
                    enter = 0;
                QApplicationPrivate::dispatchEnterLeave(enter, widget);
                curWin = enter ? enter->effectiveWinId() : 0;
                qt_last_mouse_receiver = enter;
            }
            if (widget == popupButtonFocus)
                popupButtonFocus = 0;
            result = false;
            break;

#ifndef Q_OS_WINCE
        case WM_WINDOWPOSCHANGING:
            {
                result = false;
                if (widget->isWindow()
                    && widget->layout()
                    && widget->layout()->hasHeightForWidth()) {
                    WINDOWPOS *winPos = (WINDOWPOS *)lParam;
                    QRect fs = widget->frameStrut();
                    QRect rect = widget->geometry();
                    QRect newRect = QRect(winPos->x + fs.left(),
                                          winPos->y + fs.top(),
                                          winPos->cx - fs.left() - fs.right(),
                                          winPos->cy - fs.top() - fs.bottom());

                    QSize newSize = QLayout::closestAcceptableSize(widget, newRect.size());

                    int dh = newSize.height() - newRect.height();
                    int dw = newSize.width() - newRect.width();
                    if (!dw && ! dh)
                        break; // Size OK

                    if (rect.y() != newRect.y()) {
                        newRect.setTop(newRect.top() - dh);
                    } else {
                        newRect.setBottom(newRect.bottom() + dh);
                    }

                    if (rect.x() != newRect.x()) {
                        newRect.setLeft(newRect.left() - dw);
                    } else {
                        newRect.setRight(newRect.right() + dw);
                    }

                    winPos->x = newRect.x() - fs.left();
                    winPos->y = newRect.y() - fs.top();
                    winPos->cx = newRect.width() + fs.left() + fs.right();
                    winPos->cy = newRect.height() + fs.top() + fs.bottom();

                    RETURN(0);
                }
            }
            break;

        case WM_GETMINMAXINFO:
            if (widget->xtra()) {
                MINMAXINFO *mmi = (MINMAXINFO *)lParam;
                QWExtra *x = widget->xtra();
                QRect fs = widget->frameStrut();
                if ( x->minw > 0 )
                    mmi->ptMinTrackSize.x = x->minw + fs.right() + fs.left();
                if ( x->minh > 0 )
                    mmi->ptMinTrackSize.y = x->minh + fs.top() + fs.bottom();
                qint32 maxw = (x->maxw >= x->minw) ? x->maxw : x->minw;
                qint32 maxh = (x->maxh >= x->minh) ? x->maxh : x->minh;
                if ( maxw < QWIDGETSIZE_MAX ) {
                    mmi->ptMaxTrackSize.x = maxw + fs.right() + fs.left();
                    // windows with title bar have an implicit size limit of 112 pixels
                    if (widget->windowFlags() & Qt::WindowTitleHint)
                        mmi->ptMaxTrackSize.x = qMax<long>(mmi->ptMaxTrackSize.x, 112);
                }
                if ( maxh < QWIDGETSIZE_MAX )
                    mmi->ptMaxTrackSize.y = maxh + fs.top() + fs.bottom();
                RETURN(0);
            }
            break;

            case WM_CONTEXTMENU:
            {
                // it's not VK_APPS or Shift+F10, but a click in the NC area
                if (lParam != (int)0xffffffff) {
                    result = false;
                    break;
                }

                QWidget *fw = QWidget::keyboardGrabber();
                if (!fw) {
                    if (qApp->activePopupWidget())
                        fw = (qApp->activePopupWidget()->focusWidget()
                                                  ? qApp->activePopupWidget()->focusWidget()
                                                  : qApp->activePopupWidget());
                    else if (qApp->focusWidget())
                        fw = qApp->focusWidget();
                    else if (widget)
                        fw = widget->window();
                }
                if (fw && fw->isEnabled()) {
                    QPoint pos = fw->inputMethodQuery(Qt::ImMicroFocus).toRect().center();
                    QContextMenuEvent e(QContextMenuEvent::Keyboard, pos, fw->mapToGlobal(pos),
                                      qt_win_getKeyboardModifiers());
                    result = qt_sendSpontaneousEvent(fw, &e);
                }
            }
            break;
#endif

        case WM_IME_STARTCOMPOSITION:
        case WM_IME_ENDCOMPOSITION:
        case WM_IME_COMPOSITION: {
            QWidget *fw = qApp->focusWidget();
            QWinInputContext *im = fw ? qobject_cast<QWinInputContext *>(fw->inputContext()) : 0;
            if (fw && im) {
                if(message == WM_IME_STARTCOMPOSITION)
                    result = im->startComposition();
                else if (message == WM_IME_ENDCOMPOSITION)
                    result = im->endComposition();
                else if (message == WM_IME_COMPOSITION)
                    result = im->composition(lParam);
            }
            break;
        }

#ifndef Q_OS_WINCE
        case WM_CHANGECBCHAIN:
        case WM_DRAWCLIPBOARD:
#endif
        case WM_RENDERFORMAT:
        case WM_RENDERALLFORMATS:
#ifndef QT_NO_CLIPBOARD
        case WM_DESTROYCLIPBOARD:
            if (qt_clipboard) {
                QClipboardEvent e(reinterpret_cast<QEventPrivate *>(&msg));
                qt_sendSpontaneousEvent(qt_clipboard, &e);
                RETURN(0);
            }
            result = false;
            break;
#endif //QT_NO_CLIPBOARD
#ifndef QT_NO_ACCESSIBILITY
        case WM_GETOBJECT:
            {
                // Ignoring all requests while starting up
                if (qApp->startingUp() || qApp->closingDown() || (DWORD)lParam != OBJID_CLIENT) {
                    result = false;
                    break;
                }

                typedef LRESULT (WINAPI *PtrLresultFromObject)(REFIID, WPARAM, LPUNKNOWN);
                static PtrLresultFromObject ptrLresultFromObject = 0;
                static bool oleaccChecked = false;

                if (!oleaccChecked) {
                    oleaccChecked = true;
#if !defined(Q_OS_WINCE)
                    ptrLresultFromObject = (PtrLresultFromObject)QLibrary::resolve(QLatin1String("oleacc.dll"), "LresultFromObject");
#endif
                }
                if (ptrLresultFromObject) {
                    QAccessibleInterface *acc = QAccessible::queryAccessibleInterface(widget);
                    if (!acc) {
                        result = false;
                        break;
                    }

                    // and get an instance of the IAccessibile implementation
                    IAccessible *iface = qt_createWindowsAccessible(acc);
                    res = ptrLresultFromObject(IID_IAccessible, wParam, iface);  // ref == 2
                    iface->Release(); // the client will release the object again, and then it will destroy itself

                    if (res > 0)
                        RETURN(res);
                }
            }
            result = false;
            break;
        case WM_GETTEXT:
            if (!widget->isWindow()) {
                int ret = 0;
                QAccessibleInterface *acc = QAccessible::queryAccessibleInterface(widget);
                if (acc) {
                    QString text = acc->text(QAccessible::Name, 0);
                    if (text.isEmpty())
                        text = widget->objectName();
                    ret = qMin<int>(wParam - 1, text.size());
                    text.resize(ret);
                    QT_WA({
                        memcpy((void *)lParam, text.utf16(), (text.size() + 1) * 2);
                    }, {
                        memcpy((void *)lParam, text.toLocal8Bit().data(), text.size() + 1);
                    });
                    delete acc;
                }
                if (!ret) {
                    result = false;
                    break;
                }
                RETURN(ret);
            }
            result = false;
            break;
#endif
        case WT_PACKET:
            if (ptrWTPacketsGet) {
                if ((nPackets = ptrWTPacketsGet(qt_tablet_context, QT_TABLET_NPACKETQSIZE, &localPacketBuf))) {
                    result = widget->translateTabletEvent(msg, localPacketBuf, nPackets);
                }
            }
            break;
        case WT_PROXIMITY:
            if (ptrWTPacketsGet) {
                bool enteredProximity = LOWORD(lParam) != 0;
                PACKET proximityBuffer[QT_TABLET_NPACKETQSIZE];
                int totalPacks = ptrWTPacketsGet(qt_tablet_context, QT_TABLET_NPACKETQSIZE, proximityBuffer);
                if (totalPacks > 0 && enteredProximity) {
                    uint currentCursor = proximityBuffer[0].pkCursor;
                    if (!tCursorInfo()->contains(currentCursor))
                        tabletInit(currentCursor, qt_tablet_context);
                    currentTabletPointer = tCursorInfo()->value(currentCursor);
                }
                qt_tabletChokeMouse = false;
#ifndef QT_NO_TABLETEVENT
                QTabletEvent tabletProximity(enteredProximity ? QEvent::TabletEnterProximity
                                                              : QEvent::TabletLeaveProximity,
                                             QPoint(), QPoint(), QPointF(), currentTabletPointer.currentDevice, currentTabletPointer.currentPointerType, 0, 0,
                                             0, 0, 0, 0, 0, currentTabletPointer.llId);
                QApplication::sendEvent(qApp, &tabletProximity);
#endif // QT_NO_TABLETEVENT
            }
            break;
#ifdef Q_OS_WINCE_WM
        case WM_SETFOCUS: {
            HIMC hC;
            Edit_SetInputMode(hwnd, EIM_TEXT);
            hC = ImmGetContext(hwnd);
            ImmSetOpenStatus(hC, TRUE);
            result = false;
        }
        break;
#endif
        case WM_KILLFOCUS:
            if (!QWidget::find((HWND)wParam)) { // we don't get focus, so unset it now
                if (!widget->hasFocus()) // work around Windows bug after minimizing/restoring
                    widget = (QETWidget*)qApp->focusWidget();
                HWND focus = ::GetFocus();
                if (!widget || (focus && ::IsChild(widget->internalWinId(), focus))) {
                    result = false;
                } else {
                    widget->clearFocus();
                    result = true;
                }
            } else {
                result = false;
            }
            break;
        case WM_THEMECHANGED:
            if ((widget->windowType() == Qt::Desktop) || !qApp || qApp->closingDown()
                                                         || qApp->type() == QApplication::Tty)
                break;

            if (widget->testAttribute(Qt::WA_WState_Polished))
                qApp->style()->unpolish(widget);

            if (widget->testAttribute(Qt::WA_WState_Polished))
                qApp->style()->polish(widget);
            widget->repolishStyle(*qApp->style());
            if (widget->isVisible())
                widget->update();
            break;

#ifndef Q_OS_WINCE
        case WM_INPUTLANGCHANGE: {
            char info[7];
            if (!GetLocaleInfoA(MAKELCID(lParam, SORT_DEFAULT), LOCALE_IDEFAULTANSICODEPAGE, info, 6)) {
                inputcharset = CP_ACP;
            } else {
                inputcharset = QString::fromLatin1(info).toInt();
            }
            QKeyMapper::changeKeyboard();
            break;
        }
#else
        case WM_COMMAND: {
            bool OkCommand = (LOWORD(wParam) == 0x1);
            bool CancelCommand = (LOWORD(wParam) == 0x2);
            if (OkCommand)
                QApplication::postEvent(widget, new QEvent(QEvent::OkRequest));
            if (CancelCommand)
                QApplication::postEvent(widget, new QEvent(QEvent::Close));
            else
#ifndef QT_NO_MENUBAR
                QMenuBar::wceCommands(LOWORD(wParam), (HWND) lParam);
#endif
            result = true;
        }
            break;
        case WM_HELP:
            QApplication::postEvent(widget, new QEvent(QEvent::HelpRequest));
            result = true;
            break;
#endif

        case WM_MOUSELEAVE:
            // We receive a mouse leave for curWin, meaning
            // the mouse was moved outside our widgets
            if (widget->internalWinId() == curWin) {
                bool dispatch = !widget->underMouse();
                // hasMouse is updated when dispatching enter/leave,
                // so test if it is actually up-to-date
                if (!dispatch) {
                    QRect geom = widget->geometry();
                    if (widget->parentWidget() && !widget->isWindow()) {
                        QPoint gp = widget->parentWidget()->mapToGlobal(widget->pos());
                        geom.setX(gp.x());
                        geom.setY(gp.y());
                    }
                    QPoint cpos = QCursor::pos();
                    dispatch = !geom.contains(cpos);
                    if ( !dispatch && !QWidget::mouseGrabber()) {
                        QWidget *hittest = QApplication::widgetAt(cpos);
                        dispatch = !hittest || hittest->internalWinId() != curWin;
                    }
                    if (!dispatch) {
                        HRGN hrgn = qt_tryCreateRegion(QRegion::Rectangle, 0,0,0,0);
                        if (GetWindowRgn(curWin, hrgn) != ERROR) {
                            QPoint lcpos = widget->mapFromGlobal(cpos);
                            dispatch = !PtInRegion(hrgn, lcpos.x(), lcpos.y());
                        }
                        DeleteObject(hrgn);
                    }
                }
                if (dispatch) {
                    if (qt_last_mouse_receiver && !qt_last_mouse_receiver->internalWinId())
                        QApplicationPrivate::dispatchEnterLeave(0, qt_last_mouse_receiver);
                    else
                        QApplicationPrivate::dispatchEnterLeave(0, QWidget::find((WId)curWin));
                    curWin = 0;
                    qt_last_mouse_receiver = 0;
                }
            }
            break;

        case WM_CANCELMODE:
            {
                // this goes through QMenuBar's event filter
                QEvent e(QEvent::ActivationChange);
                QApplication::sendEvent(qApp, &e);
            }
            break;

        case WM_IME_NOTIFY: 
            // special handling for ime, only for widgets in a popup
            if (wParam  == IMN_OPENCANDIDATE) {
                imeParentWnd = hwnd;
                if (QApplication::activePopupWidget()) {
                    // temporarily disable the mouse grab to allow mouse input in 
                    // the ime candidate window. The actual handle is untouched
                    if (autoCaptureWnd)
                        ReleaseCapture();
                }
            } else if (wParam  == IMN_CLOSECANDIDATE) {
                imeParentWnd = 0;
                if (QApplication::activePopupWidget()) {
                    // undo the action above, when candidate window is closed
                    if (autoCaptureWnd)
                        SetCapture(autoCaptureWnd);
                }
            }
            result = false;
            break;
        default:
            result = false;                        // event was not processed
            break;
        }
    }

    if (evt_type != QEvent::None) {                // simple event
        QEvent e(evt_type);
        result = qt_sendSpontaneousEvent(widget, &e);
    }

    if (result)
        RETURN(false);

do_default:
    RETURN(QWinInputContext::DefWindowProc(hwnd,message,wParam,lParam))
}


/*****************************************************************************
  Modal widgets; We have implemented our own modal widget mechanism
  to get total control.
  A modal widget without a parent becomes application-modal.
  A modal widget with a parent becomes modal to its parent and grandparents..

  QApplicationPrivate::enterModal()
        Enters modal state
        Arguments:
            QWidget *widget        A modal widget

  QApplicationPrivate::leaveModal()
        Leaves modal state for a widget
        Arguments:
            QWidget *widget        A modal widget
 *****************************************************************************/

bool QApplicationPrivate::modalState()
{
    return app_do_modal;
}

inline void setDisabledStyle(QWidget *w, bool setStyle)
{
    // set/reset WS_DISABLED style.
    if(w && w->isWindow() && w->isVisible() && w->isEnabled()) {
        LONG dwStyle = GetWindowLong(w->winId(), GWL_STYLE);
        if (setStyle)
            dwStyle |= WS_DISABLED;
        else
            dwStyle &= ~WS_DISABLED;
        SetWindowLong(w->winId(), GWL_STYLE, dwStyle);
        // we might need to repaint in some situations (eg. menu)
        if (setStyle)
            w->repaint();
    }
}

void QApplicationPrivate::enterModal_sys(QWidget *widget)
{
    if (!qt_modal_stack)
        qt_modal_stack = new QWidgetList;

    releaseAutoCapture();
    QWidget *leave = qt_last_mouse_receiver;
    if (!leave)
        leave = QWidget::find((WId)curWin);
    QApplicationPrivate::dispatchEnterLeave(0, leave);
    qt_modal_stack->insert(0, widget);
    app_do_modal = true;
    curWin = 0;
    qt_last_mouse_receiver = 0;
    qt_win_ignoreNextMouseReleaseEvent = false;
    // set the WS_DISABLED style for all relevant top level windows
    foreach(QWidget *window, QApplication::topLevelWidgets()) {
        if(window != widget) {
            setDisabledStyle(window, true);
        }
    }
}

void QApplicationPrivate::leaveModal_sys(QWidget *widget)
{
    if (qt_modal_stack && qt_modal_stack->removeAll(widget)) {
        if (qt_modal_stack->isEmpty()) {
            delete qt_modal_stack;
            qt_modal_stack = 0;
            QPoint p(QCursor::pos());
            app_do_modal = false; // necessary, we may get recursively into qt_try_modal below
            QWidget* w = QApplication::widgetAt(p.x(), p.y());
            QWidget *leave = qt_last_mouse_receiver;
            if (!leave)
                leave = QWidget::find((WId)curWin);
            if (QWidget *grabber = QWidget::mouseGrabber()) {
                w = grabber;
                if (leave == w)
                    leave = 0;
            }
            QApplicationPrivate::dispatchEnterLeave(w, leave); // send synthetic enter event
            curWin = w ? w->effectiveWinId() : 0;
            qt_last_mouse_receiver = w;
            // reset the WS_DISABLED style for all relevant top level windows
            foreach(QWidget *window, QApplication::topLevelWidgets()) {
                if(window != widget) {
                    setDisabledStyle(window, false);
                }
            }
        } else {
            // reset the WS_DISABLED style for the last widget in the modal stack
            QWidget *window = qt_modal_stack->at(0);
            if(window != widget) {
                setDisabledStyle(window, false);
            }
        }
        qt_win_ignoreNextMouseReleaseEvent = true;
    }
    app_do_modal = qt_modal_stack != 0;
}

bool qt_try_modal(QWidget *widget, MSG *msg, int& ret)
{
#if defined(Q_OS_WINCE)
    Q_UNUSED(ret);
#endif
    QWidget * top = 0;

    if (QApplicationPrivate::tryModalHelper(widget, &top))
        return true;

    int type = msg->message;

    bool block_event = false;
#ifndef Q_OS_WINCE
    if (type != WM_NCHITTEST)
#endif
        if ((type >= WM_MOUSEFIRST && type <= WM_MOUSELAST) ||
             type == WM_MOUSEWHEEL || type == (int)WM95_MOUSEWHEEL ||
             type == WM_MOUSELEAVE ||
             (type >= WM_KEYFIRST && type <= WM_KEYLAST)
#ifndef Q_OS_WINCE
            || type == WM_NCMOUSEMOVE
#endif
         ) {
            if (type == WM_MOUSEMOVE
#ifndef Q_OS_WINCE
                 || type == WM_NCMOUSEMOVE
#endif
            ) {
#ifndef QT_NO_CURSOR
                QCursor *c = qt_grab_cursor();
                if (!c)
                    c = QApplication::overrideCursor();
                if (c)                                // application cursor defined
                    SetCursor(c->handle());
                else
                    SetCursor(QCursor(Qt::ArrowCursor).handle());
#endif // QT_NO_CURSOR
            }
            block_event = true;
        } else if (type == WM_CLOSE) {
            block_event = true;
        }
#ifndef Q_OS_WINCE
    else if (type == WM_MOUSEACTIVATE || type == WM_NCLBUTTONDOWN){
        if (!top->isActiveWindow()) {
            top->activateWindow();
        } else {
            QApplication::beep();
        }
        block_event = true;
        ret = MA_NOACTIVATEANDEAT;
    } else if (type == WM_SYSCOMMAND) {
        if (!(msg->wParam == SC_RESTORE && widget->isMinimized()))
            block_event = true;
    }
#endif

    return !block_event;
}


/*****************************************************************************
  Popup widget mechanism

  openPopup()
        Adds a widget to the list of popup widgets
        Arguments:
            QWidget *widget        The popup widget to be added

  closePopup()
        Removes a widget from the list of popup widgets
        Arguments:
            QWidget *widget        The popup widget to be removed
 *****************************************************************************/

void QApplicationPrivate::openPopup(QWidget *popup)
{
    if (!QApplicationPrivate::popupWidgets)
        QApplicationPrivate::popupWidgets = new QWidgetList;
    QApplicationPrivate::popupWidgets->append(popup);
    if (!popup->isEnabled())
        return;
    
    // close any opened 'ime candidate window'
    if (imeParentWnd)
        ::SendMessage(imeParentWnd, WM_IME_ENDCOMPOSITION, 0, 0);
    
    if (QApplicationPrivate::popupWidgets->count() == 1 && !qt_nograb()) {
        Q_ASSERT(popup->testAttribute(Qt::WA_WState_Created));
        setAutoCapture(popup->internalWinId());        // grab mouse/keyboard
    }
    // Popups are not focus-handled by the window system (the first
    // popup grabbed the keyboard), so we have to do that manually: A
    // new popup gets the focus
    if (popup->focusWidget()) {
        popup->focusWidget()->setFocus(Qt::PopupFocusReason);
    } else if (QApplicationPrivate::popupWidgets->count() == 1) { // this was the first popup
        if (QWidget *fw = q_func()->focusWidget()) {
            QFocusEvent e(QEvent::FocusOut, Qt::PopupFocusReason);
            q_func()->sendEvent(fw, &e);
        }
    }
}

void QApplicationPrivate::closePopup(QWidget *popup)
{
    if (!QApplicationPrivate::popupWidgets)
        return;
    QApplicationPrivate::popupWidgets->removeAll(popup);
    POINT curPos;
    GetCursorPos(&curPos);
    
    // close any opened 'ime candidate window'
    if (imeParentWnd)
        ::SendMessage(imeParentWnd, WM_IME_ENDCOMPOSITION, 0, 0);

    if (QApplicationPrivate::popupWidgets->isEmpty()) { // this was the last popup
        delete QApplicationPrivate::popupWidgets;
        QApplicationPrivate::popupWidgets = 0;
        replayPopupMouseEvent = (!popup->geometry().contains(QPoint(curPos.x, curPos.y))
                                && !popup->testAttribute(Qt::WA_NoMouseReplay));
        if (!popup->isEnabled())
            return;
        if (!qt_nograb())                        // grabbing not disabled
            releaseAutoCapture();
        QWidget *fw = QApplicationPrivate::active_window ? QApplicationPrivate::active_window->focusWidget()
            : q_func()->focusWidget();
        if (fw) {
            if (fw != q_func()->focusWidget()) {
                fw->setFocus(Qt::PopupFocusReason);
            } else {
                QFocusEvent e(QEvent::FocusIn, Qt::PopupFocusReason);
                q_func()->sendEvent(fw, &e);
            }
        }
    } else {
        // Popups are not focus-handled by the window system (the
        // first popup grabbed the keyboard), so we have to do that
        // manually: A popup was closed, so the previous popup gets
        // the focus.
        QWidget* aw = QApplicationPrivate::popupWidgets->last();
        if (QApplicationPrivate::popupWidgets->count() == 1) {
            Q_ASSERT(aw->testAttribute(Qt::WA_WState_Created));
            setAutoCapture(aw->internalWinId());
        }
        if (QWidget *fw = aw->focusWidget())
            fw->setFocus(Qt::PopupFocusReason);
    }
}




/*****************************************************************************
  Event translation; translates Windows events to Qt events
 *****************************************************************************/

//
// Auto-capturing for mouse press and mouse release
//

static void setAutoCapture(HWND h)
{
    if (autoCaptureWnd)
        releaseAutoCapture();
    autoCaptureWnd = h;
    SetCapture(h);
}

static void releaseAutoCapture()
{
    if (autoCaptureWnd) {
        ReleaseCapture();
        autoCaptureWnd = 0;
    }
}


//
// Mouse event translation
//
// Non-client mouse messages are not translated
//

static const ushort mouseTbl[] = {
    WM_MOUSEMOVE,        QEvent::MouseMove,               0,
    WM_LBUTTONDOWN,      QEvent::MouseButtonPress,        Qt::LeftButton,
    WM_LBUTTONUP,        QEvent::MouseButtonRelease,      Qt::LeftButton,
    WM_LBUTTONDBLCLK,    QEvent::MouseButtonDblClick,     Qt::LeftButton,
    WM_RBUTTONDOWN,      QEvent::MouseButtonPress,        Qt::RightButton,
    WM_RBUTTONUP,        QEvent::MouseButtonRelease,      Qt::RightButton,
    WM_RBUTTONDBLCLK,    QEvent::MouseButtonDblClick,     Qt::RightButton,
    WM_MBUTTONDOWN,      QEvent::MouseButtonPress,        Qt::MidButton,
    WM_MBUTTONUP,        QEvent::MouseButtonRelease,      Qt::MidButton,
    WM_MBUTTONDBLCLK,    QEvent::MouseButtonDblClick,     Qt::MidButton,
    // use XButton1 for now, the real X button is decided later
    WM_XBUTTONDOWN,      QEvent::MouseButtonPress,        Qt::XButton1,
    WM_XBUTTONUP,        QEvent::MouseButtonRelease,      Qt::XButton1,
    WM_XBUTTONDBLCLK,    QEvent::MouseButtonDblClick,     Qt::XButton1,

#ifndef Q_OS_WINCE
    WM_NCMOUSEMOVE,      QEvent::NonClientAreaMouseMove,           0,
    WM_NCLBUTTONDOWN,    QEvent::NonClientAreaMouseButtonPress,    Qt::LeftButton,
    WM_NCLBUTTONUP,      QEvent::NonClientAreaMouseButtonRelease,  Qt::LeftButton,
    WM_NCLBUTTONDBLCLK,  QEvent::NonClientAreaMouseButtonDblClick, Qt::LeftButton,
    WM_NCRBUTTONDOWN,    QEvent::NonClientAreaMouseButtonPress,    Qt::RightButton,
    WM_NCRBUTTONUP,      QEvent::NonClientAreaMouseButtonRelease,  Qt::RightButton,
    WM_NCRBUTTONDBLCLK,  QEvent::NonClientAreaMouseButtonDblClick, Qt::RightButton,
    WM_NCMBUTTONDOWN,    QEvent::NonClientAreaMouseButtonPress,    Qt::MidButton,
    WM_NCMBUTTONUP,      QEvent::NonClientAreaMouseButtonRelease,  Qt::MidButton,
    WM_NCMBUTTONDBLCLK,  QEvent::NonClientAreaMouseButtonDblClick, Qt::MidButton,
#endif

    0,                        0,                                0
};

static int translateButtonState(int s, int type, int button)
{
    Q_UNUSED(type);
    Q_UNUSED(button);
    int bst = 0;
    if (s & MK_LBUTTON)
        bst |= Qt::LeftButton;
    if (s & MK_MBUTTON)
        bst |= Qt::MidButton;
    if (s & MK_RBUTTON)
        bst |= Qt::RightButton;
    if (s & MK_SHIFT)
        bst |= Qt::ShiftModifier;
    if (s & MK_CONTROL)
        bst |= Qt::ControlModifier;

    if (s & MK_XBUTTON1)
        bst |= Qt::XButton1;
    if (s & MK_XBUTTON2)
        bst |= Qt::XButton2;

    if (GetKeyState(VK_MENU) < 0)
        bst |= Qt::AltModifier;

    if ((GetKeyState(VK_LWIN) < 0) ||
         (GetKeyState(VK_RWIN) < 0))
        bst |= Qt::MetaModifier;

    return bst;
}

void qt_win_eatMouseMove()
{
    // after closing a windows dialog with a double click (i.e. open a file)
    // the message queue still contains a dubious WM_MOUSEMOVE message where
    // the left button is reported to be down (wParam != 0).
    // remove all those messages (usually 1) and post the last one with a
    // reset button state

    MSG msg = {0, 0, 0, 0, 0, 0, 0};
    QT_WA( {
        while (PeekMessage(&msg, 0, WM_MOUSEMOVE, WM_MOUSEMOVE, PM_REMOVE))
            ;
        if (msg.message == WM_MOUSEMOVE)
            PostMessage(msg.hwnd, msg.message, 0, msg.lParam);
    }, {
        MSG msg;
        msg.message = 0;
        while (PeekMessageA(&msg, 0, WM_MOUSEMOVE, WM_MOUSEMOVE, PM_REMOVE))
            ;
        if (msg.message == WM_MOUSEMOVE)
            PostMessageA(msg.hwnd, msg.message, 0, msg.lParam);
    } );
}

// In DnD, the mouse release event never appears, so the
// mouse button state machine must be manually reset
/*! \internal */
void QApplication::winMouseButtonUp()
{
    qt_button_down = 0;
    releaseAutoCapture();
}

void QETWidget::repolishStyle(QStyle &)
{
    QEvent e(QEvent::StyleChange);
    QApplication::sendEvent(this, &e);
}

bool QETWidget::translateMouseEvent(const MSG &msg)
{
    if (!isWindow() && testAttribute(Qt::WA_NativeWindow))
        Q_ASSERT(internalWinId());

    static QPoint pos;
    static POINT gpos={-1,-1};
    QEvent::Type type;                                // event parameters
    int           button;
    int           state;
    int           i;

    if (sm_blockUserInput) //block user interaction during session management
        return true;

    // Compress mouse move events
    if (msg.message == WM_MOUSEMOVE) {
        MSG mouseMsg;
        while (winPeekMessage(&mouseMsg, msg.hwnd, WM_MOUSEFIRST,
                WM_MOUSELAST, PM_NOREMOVE)) {
            if (mouseMsg.message == WM_MOUSEMOVE) {
#define PEEKMESSAGE_IS_BROKEN 1
#ifdef PEEKMESSAGE_IS_BROKEN
                // Since the Windows PeekMessage() function doesn't
                // correctly return the wParam for WM_MOUSEMOVE events
                // if there is a key release event in the queue
                // _before_ the mouse event, we have to also consider
                // key release events (kls 2003-05-13):
                MSG keyMsg;
                bool done = false;
                while (winPeekMessage(&keyMsg, 0, WM_KEYFIRST, WM_KEYLAST,
                        PM_NOREMOVE)) {
                    if (keyMsg.time < mouseMsg.time) {
                        if ((keyMsg.lParam & 0xC0000000) == 0x40000000) {
                            winPeekMessage(&keyMsg, 0, keyMsg.message,
                                            keyMsg.message, PM_REMOVE);
                        } else {
                            done = true;
                            break;
                        }
                    } else {
                        break; // no key event before the WM_MOUSEMOVE event
                    }
                }
                if (done)
                    break;
#else
                // Actually the following 'if' should work instead of
                // the above key event checking, but apparently
                // PeekMessage() is broken :-(
                if (mouseMsg.wParam != msg.wParam)
                    break; // leave the message in the queue because
                           // the key state has changed
#endif
                MSG *msgPtr = (MSG *)(&msg);
                // Update the passed in MSG structure with the
                // most recent one.
                msgPtr->lParam = mouseMsg.lParam;
                msgPtr->wParam = mouseMsg.wParam;
                msgPtr->pt = mouseMsg.pt;
                // Remove the mouse move message
                winPeekMessage(&mouseMsg, msg.hwnd, WM_MOUSEMOVE,
                                WM_MOUSEMOVE, PM_REMOVE);
            } else {
                break; // there was no more WM_MOUSEMOVE event
            }
        }
    }

    for (i=0; (UINT)mouseTbl[i] != msg.message && mouseTbl[i]; i += 3)
        ;
    if (!mouseTbl[i])
        return false;
    type   = (QEvent::Type)mouseTbl[++i];        // event type
    button = mouseTbl[++i];                        // which button
    if (button == Qt::XButton1) {
        switch(GET_XBUTTON_WPARAM(msg.wParam)) {
        case XBUTTON1:
            button = Qt::XButton1;
            break;
        case XBUTTON2:
            button = Qt::XButton2;
            break;
        }
    }
    state  = translateButtonState(msg.wParam, type, button); // button state
    const QPoint widgetPos = mapFromGlobal(QPoint(msg.pt.x, msg.pt.y));
    QWidget *alienWidget = !internalWinId() ? this : childAt(widgetPos);
    if (alienWidget && alienWidget->internalWinId())
        alienWidget = 0;

    if (type == QEvent::MouseMove || type == QEvent::NonClientAreaMouseMove) {
        if (!(state & Qt::MouseButtonMask))
            qt_button_down = 0;
#ifndef QT_NO_CURSOR
        QCursor *c = qt_grab_cursor();
        if (!c)
            c = QApplication::overrideCursor();
        if (c)                                // application cursor defined
            SetCursor(c->handle());
        else if (type != QEvent::NonClientAreaMouseMove && !qt_button_down) {
            // use  widget cursor if widget is enabled
            QWidget *w = alienWidget ? alienWidget : this;
            while (!w->isWindow() && !w->isEnabled())
                w = w->parentWidget();
            SetCursor(w->cursor().handle());
        }
#endif // QT_NO_CURSOR

        HWND id = effectiveWinId();
        QWidget *mouseGrabber = QWidget::mouseGrabber();
        QWidget *activePopupWidget = qApp->activePopupWidget();
        if (mouseGrabber) {
            if (!activePopupWidget || (activePopupWidget == this && !rect().contains(widgetPos)))
                id = mouseGrabber->effectiveWinId();
        } else if (type == QEvent::NonClientAreaMouseMove) {
            id = 0;
        }

        if (curWin != id) {                // new current window
            if (id == 0) {
                QWidget *leave = qt_last_mouse_receiver;
                if (!leave)
                    leave = QWidget::find(curWin);
                QApplicationPrivate::dispatchEnterLeave(0, leave);
                qt_last_mouse_receiver = 0;
                curWin = 0;
            } else {
                QWidget *leave = 0;
                if (curWin && qt_last_mouse_receiver)
                    leave = qt_last_mouse_receiver;
                else
                    leave = QWidget::find(curWin);
                QWidget *enter = alienWidget ? alienWidget : this;
                if (mouseGrabber && activePopupWidget) {
                    if (leave != mouseGrabber)
                        enter = mouseGrabber;
                    else
                        enter = activePopupWidget == this ? this : mouseGrabber;
                }
                QApplicationPrivate::dispatchEnterLeave(enter, leave);
                qt_last_mouse_receiver = enter;
                curWin = enter ? enter->effectiveWinId() : 0;
            }
#ifndef Q_OS_WINCE

            if (curWin != 0) {
                static bool trackMouseEventLookup = false;
                typedef BOOL (WINAPI *PtrTrackMouseEvent)(LPTRACKMOUSEEVENT);
                static PtrTrackMouseEvent ptrTrackMouseEvent = 0;
                if (!trackMouseEventLookup) {
                    trackMouseEventLookup = true;
                    ptrTrackMouseEvent = (PtrTrackMouseEvent)QLibrary::resolve(QLatin1String("comctl32"), "_TrackMouseEvent");
                }
                if (ptrTrackMouseEvent && !qApp->d_func()->inPopupMode()) {
                    // We always have to set the tracking, since
                    // Windows detects more leaves than we do..
                    TRACKMOUSEEVENT tme;
                    tme.cbSize = sizeof(TRACKMOUSEEVENT);
                    tme.dwFlags = 0x00000002;    // TME_LEAVE
                    tme.hwndTrack = curWin;      // Track on window receiving msgs
                    tme.dwHoverTime = (DWORD)-1; // HOVER_DEFAULT
                    ptrTrackMouseEvent(&tme);
                }
            }
#endif // Q_OS_WINCE
        }

        POINT curPos = msg.pt;
        if (curPos.x == gpos.x && curPos.y == gpos.y)
            return true;                        // same global position
        gpos = curPos;

        Q_ASSERT(testAttribute(Qt::WA_WState_Created));
        ScreenToClient(internalWinId(), &curPos);

        pos.rx() = curPos.x;
        pos.ry() = curPos.y;
        pos = d_func()->mapFromWS(pos);
    } else {
        gpos = msg.pt;
        pos = mapFromGlobal(QPoint(gpos.x, gpos.y));

        // mouse button pressed
        if (!qt_button_down && (type == QEvent::MouseButtonPress || type == QEvent::MouseButtonDblClick)) {
            QWidget *tlw = window();
            if (QWidget *child = tlw->childAt(mapTo(tlw, pos)))
                qt_button_down = child;
            else
                qt_button_down = this;
        }
    }

    bool res = false;

    bool nonClientAreaEvent = type >= QEvent::NonClientAreaMouseMove
                                && type <= QEvent::NonClientAreaMouseButtonDblClick;

    if (qApp->d_func()->inPopupMode()) {                        // in popup mode

        if (nonClientAreaEvent)
            return false;

        replayPopupMouseEvent = false;
        QWidget* activePopupWidget = qApp->activePopupWidget();
        QWidget *target = activePopupWidget;
        const QPoint globalPos(gpos.x, gpos.y);

        if (target != this) {
            if ((windowType() == Qt::Popup) && rect().contains(pos) && 0)
                target = this;
            else                                // send to last popup
                pos = target->mapFromGlobal(globalPos);
        }
        QWidget *popupChild = target->childAt(pos);
        bool releaseAfter = false;
        switch (type) {
            case QEvent::MouseButtonPress:
            case QEvent::MouseButtonDblClick:
                popupButtonFocus = popupChild;
                break;
            case QEvent::MouseButtonRelease:
                releaseAfter = true;
                break;
            default:
                break;                                // nothing for mouse move
        }

        if (target->isEnabled()) {
            if (popupButtonFocus) {
                target = popupButtonFocus;
            } else if (popupChild) {
                // forward mouse events to the popup child. mouse move events
                // are only forwarded to popup children that enable mouse tracking.
                if (type != QEvent::MouseMove || popupChild->hasMouseTracking())
                    target = popupChild;
            }

            pos = target->mapFromGlobal(globalPos);
                QMouseEvent e(type, pos, globalPos,
                            Qt::MouseButton(button),
                            Qt::MouseButtons(state & Qt::MouseButtonMask),
                            Qt::KeyboardModifiers(state & Qt::KeyboardModifierMask));
                res = QApplicationPrivate::sendMouseEvent(target, &e, alienWidget, this, &qt_button_down,
                                                          qt_last_mouse_receiver);
            res = res && e.isAccepted();
        } else {
            // close disabled popups when a mouse button is pressed or released
            switch (type) {
            case QEvent::MouseButtonPress:
            case QEvent::MouseButtonDblClick:
            case QEvent::MouseButtonRelease:
                target->close();
                break;
            default:
                break;
            }
        }

        if (releaseAfter) {
            popupButtonFocus = 0;
            qt_button_down = 0;
        }

        if (type == QEvent::MouseButtonPress
            && qApp->activePopupWidget() != activePopupWidget
            && replayPopupMouseEvent) {
            // the popup dissappeared. Replay the event
            QWidget* w = QApplication::widgetAt(gpos.x, gpos.y);
            if (w && !QApplicationPrivate::isBlockedByModal(w)) {
                Q_ASSERT(w->testAttribute(Qt::WA_WState_Created));
                if (QWidget::mouseGrabber() == 0)
                    setAutoCapture(w->internalWinId());
                if (!w->isActiveWindow())
                    w->activateWindow();
                POINT widgetpt = gpos;
                ScreenToClient(w->internalWinId(), &widgetpt);
                LPARAM lParam = MAKELPARAM(widgetpt.x, widgetpt.y);
                winPostMessage(w->internalWinId(), msg.message, msg.wParam, lParam);
            }
        } else if (type == QEvent::MouseButtonRelease && button == Qt::RightButton
                && qApp->activePopupWidget() == activePopupWidget) {
            // popup still alive and received right-button-release
            QContextMenuEvent e2(QContextMenuEvent::Mouse, pos, globalPos,
                              qt_win_getKeyboardModifiers());
            bool res2 = QApplication::sendSpontaneousEvent( target, &e2 );
            if (!res) // RMB not accepted
                res = res2 && e2.isAccepted();
        }
    } else {                                        // not popup mode
        int bs = state & Qt::MouseButtonMask;
        if ((type == QEvent::MouseButtonPress ||
              type == QEvent::MouseButtonDblClick) && bs == button) {
            Q_ASSERT(testAttribute(Qt::WA_WState_Created));
            if (QWidget::mouseGrabber() == 0)
                setAutoCapture(internalWinId());
        } else if (type == QEvent::MouseButtonRelease && bs == 0) {
            if (QWidget::mouseGrabber() == 0)
                releaseAutoCapture();
        }

        const QPoint globalPos(gpos.x,gpos.y);
        QWidget *widget = QApplicationPrivate::pickMouseReceiver(this, globalPos, pos, type,
                                                                 Qt::MouseButtons(bs),
                                                                 qt_button_down, alienWidget);
        if (!widget)
            return false; // don't send event

        QMouseEvent e(type, pos, globalPos, Qt::MouseButton(button),
                      Qt::MouseButtons(state & Qt::MouseButtonMask),
                      Qt::KeyboardModifiers(state & Qt::KeyboardModifierMask));

        res = QApplicationPrivate::sendMouseEvent(widget, &e, alienWidget, this, &qt_button_down,
                                                  qt_last_mouse_receiver);

        // non client area events are only informational, you cannot "handle" them
        res = res && e.isAccepted() && !nonClientAreaEvent;
        if (type == QEvent::MouseButtonRelease && button == Qt::RightButton) {
            QContextMenuEvent e2(QContextMenuEvent::Mouse, pos, globalPos,
                              qt_win_getKeyboardModifiers());
            bool res2 = QApplication::sendSpontaneousEvent(widget, &e2);
            if (!res)
                res = res2 && e2.isAccepted();
        }

        if (type != QEvent::MouseMove)
            pos.rx() = pos.ry() = -9999;        // init for move compression
    }
    return res;
}

bool QETWidget::translateWheelEvent(const MSG &msg)
{
    int  state = 0;

    if (sm_blockUserInput) // block user interaction during session management
        return true;

    state = translateButtonState(GET_KEYSTATE_WPARAM(msg.wParam), 0, 0);

    int delta;
    if (msg.message == WM_MOUSEWHEEL)
        delta = (short) HIWORD (msg.wParam);
    else
        delta = (int) msg.wParam;

    Qt::Orientation orient = (state&Qt::AltModifier
#if 0
    // disabled for now - Trenton's one-wheel mouse makes trouble...
    // "delta" for usual wheels is +-120. +-240 seems to indicate
    // the second wheel see more recent MSDN for WM_MOUSEWHEEL

    ( // <- parantheses added to make update happy, remove if the
      // #if 0 is removed 
        || delta == 240 || delta == -240)?Qt::Horizontal:Vertical;
    if (delta == 240 || delta == -240)
        delta /= 2;
#endif
       ) ? Qt::Horizontal : Qt::Vertical;

    QPoint globalPos;

    globalPos.rx() = (short)LOWORD (msg.lParam);
    globalPos.ry() = (short)HIWORD (msg.lParam);


    // if there is a widget under the mouse and it is not shadowed
    // by modality, we send the event to it first
    int ret = 0;
    QWidget* w = QApplication::widgetAt(globalPos);
    if (!w || !qt_try_modal(w, (MSG*)&msg, ret)) {
        //synaptics touchpad shows its own widget at this position
        //so widgetAt() will fail with that HWND, try child of this widget
        w = this->childAt(this->mapFromGlobal(globalPos));
        if (!w)
            w = this;
    }

    // send the event to the widget or its ancestors
    {
        QWidget* popup = qApp->activePopupWidget();
        if (popup && w->window() != popup)
            popup->close();
#ifndef QT_NO_WHEELEVENT
        QWheelEvent e(w->mapFromGlobal(globalPos), globalPos, delta,
                      Qt::MouseButtons(state & Qt::MouseButtonMask),
                      Qt::KeyboardModifier(state & Qt::KeyboardModifierMask), orient);

        if (QApplication::sendSpontaneousEvent(w, &e))
#else
        Q_UNUSED(orient);
#endif //QT_NO_WHEELEVENT
            return true;
    }

    // send the event to the widget that has the focus or its ancestors, if different
    if (w != qApp->focusWidget() && (w = qApp->focusWidget())) {
        QWidget* popup = qApp->activePopupWidget();
        if (popup && w->window() != popup)
            popup->close();
#ifndef QT_NO_WHEELEVENT
        QWheelEvent e(w->mapFromGlobal(globalPos), globalPos, delta,
                      Qt::MouseButtons(state & Qt::MouseButtonMask),
                      Qt::KeyboardModifier(state & Qt::KeyboardModifierMask), orient);
        if (QApplication::sendSpontaneousEvent(w, &e))
#endif //QT_NO_WHEELEVENT
            return true;
    }
    return false;
}


//
// Windows Wintab to QTabletEvent translation
//

// the following is adapted from the wintab syspress example (public domain)
/* -------------------------------------------------------------------------- */
static void tabletInit(UINT wActiveCsr, HCTX hTab)
{
    /* browse WinTab's many info items to discover pressure handling. */
    if (ptrWTInfo && ptrWTGet) {
        AXIS np;
        LOGCONTEXT lc;
        BYTE wPrsBtn;
        BYTE logBtns[32];
        UINT size;

        /* discover the LOGICAL button generated by the pressure channel. */
        /* get the PHYSICAL button from the cursor category and run it */
        /* through that cursor's button map (usually the identity map). */
        wPrsBtn = (BYTE)-1;
        ptrWTInfo(WTI_CURSORS + wActiveCsr, CSR_NPBUTTON, &wPrsBtn);
        size = ptrWTInfo(WTI_CURSORS + wActiveCsr, CSR_BUTTONMAP, &logBtns);
        if ((UINT)wPrsBtn < size)
            wPrsBtn = logBtns[wPrsBtn];

        /* get the current context for its device variable. */
        ptrWTGet(hTab, &lc);

        /* get the size of the pressure axis. */
        QTabletDeviceData tdd;
        ptrWTInfo(WTI_DEVICES + lc.lcDevice, DVC_NPRESSURE, &np);
        tdd.minPressure = int(np.axMin);
        tdd.maxPressure = int(np.axMax);

        ptrWTInfo(WTI_DEVICES + lc.lcDevice, DVC_TPRESSURE, &np);
        tdd.minTanPressure = int(np.axMin);
        tdd.maxTanPressure = int(np.axMax);

        ptrWTInfo(WTI_DEVICES + lc.lcDevice, DVC_X, &np);
        tdd.minX = int(np.axMin);
        tdd.maxX = int(np.axMax);

        ptrWTInfo(WTI_DEVICES + lc.lcDevice, DVC_Y, &np);
        tdd.minY = int(np.axMin);
        tdd.maxY = int(np.axMax);

        ptrWTInfo(WTI_DEVICES + lc.lcDevice, DVC_Z, &np);
        tdd.minZ = int(np.axMin);
        tdd.maxZ = int(np.axMax);

        int csr_type,
            csr_physid;
        ptrWTInfo(WTI_CURSORS + wActiveCsr, CSR_TYPE, &csr_type);
        ptrWTInfo(WTI_CURSORS + wActiveCsr, CSR_PHYSID, &csr_physid);
        tdd.llId = csr_type & 0x0F06;
        tdd.llId = (tdd.llId << 24) | csr_physid;
#ifndef QT_NO_TABLETEVENT
        switch (csr_type & 0x0F06) {
        case 0x0802:
            tdd.currentDevice = QTabletEvent::Stylus;
            break;
        case 0x0902:
            tdd.currentDevice = QTabletEvent::Airbrush;
            break;
        case 0x0004:
            tdd.currentDevice = QTabletEvent::FourDMouse;
            break;
        case 0x0006:
            tdd.currentDevice = QTabletEvent::Puck;
            break;
        case 0x0804:
            tdd.currentDevice = QTabletEvent::RotationStylus;
            break;
        default:
            tdd.currentDevice = QTabletEvent::NoDevice;
        }


        switch (wActiveCsr % 3) {
        case 2:
            tdd.currentPointerType = QTabletEvent::Eraser;
            break;
        case 1:
            tdd.currentPointerType = QTabletEvent::Pen;
            break;
        case 0:
            tdd.currentPointerType = QTabletEvent::Cursor;
            break;
        default:
            tdd.currentPointerType = QTabletEvent::UnknownPointer;
        }
#endif // QT_NO_TABLETEVENT
        tCursorInfo()->insert(wActiveCsr, tdd);
    }
}

bool QETWidget::translateTabletEvent(const MSG &msg, PACKET *localPacketBuf,
                                      int numPackets)
{
    Q_UNUSED(msg);
    POINT ptNew;
    static DWORD btnNew, btnOld, btnChange;
    qreal prsNew;
    ORIENTATION ort;
    static bool button_pressed = false;
    int i,
        tiltX,
        tiltY;
    bool sendEvent = false;
    QEvent::Type t;
    int z = 0;
    qreal rotation = 0.0;
    qreal tangentialPressure;

    // the most common event that we get...
    t = QEvent::TabletMove;
    for (i = 0; i < numPackets; i++) {
        // get the unique ID of the device...
        btnOld = btnNew;
        btnNew = localPacketBuf[i].pkButtons;
        btnChange = btnOld ^ btnNew;

        if (btnNew & btnChange) {
            button_pressed = true;
            t = QEvent::TabletPress;
        }
        ptNew.x = UINT(localPacketBuf[i].pkX);
        ptNew.y = UINT(localPacketBuf[i].pkY);
#ifndef QT_NO_TABLETEVENT
        z = (currentTabletPointer.currentDevice == QTabletEvent::FourDMouse) ? UINT(localPacketBuf[i].pkZ) : 0;
#else
        Q_UNUSED(z);
#endif // QT_NO_TABLETEVENT
        prsNew = 0.0;
        QRect desktopArea = QApplication::desktop()->geometry();
        QPointF hiResGlobal = currentTabletPointer.scaleCoord(ptNew.x, ptNew.y, desktopArea.left(),
                                                              desktopArea.width(), desktopArea.top(),
                                                              desktopArea.height());

        if (btnNew) {
#ifndef QT_NO_TABLETEVENT
            if (currentTabletPointer.currentPointerType == QTabletEvent::Pen || currentTabletPointer.currentPointerType == QTabletEvent::Eraser)
                prsNew = localPacketBuf[i].pkNormalPressure
                            / qreal(currentTabletPointer.maxPressure
                                    - currentTabletPointer.minPressure);
            else
#endif // QT_NO_TABLETEVENT
                prsNew = 0;
        } else if (button_pressed) {
            // One button press, should only give one button release
            t = QEvent::TabletRelease;
            button_pressed = false;
        }
        QPoint globalPos(qRound(hiResGlobal.x()), qRound(hiResGlobal.y()));

        // make sure the tablet event get's sent to the proper widget...
        QWidget *w = QApplication::widgetAt(globalPos);
        if (qt_button_down)
            w = qt_button_down; // Pass it to the thing that's grabbed it.

        if (!w)
            w = this;
        QPoint localPos = w->mapFromGlobal(globalPos);
#ifndef QT_NO_TABLETEVENT
        if (currentTabletPointer.currentDevice == QTabletEvent::Airbrush) {
            tangentialPressure = localPacketBuf[i].pkTangentPressure
                                / qreal(currentTabletPointer.maxTanPressure
                                        - currentTabletPointer.minTanPressure);
        } else {
            tangentialPressure = 0.0;
        }
#else
        tangentialPressure = 0.0;
#endif // QT_NO_TABLETEVENT

        if (!qt_tablet_tilt_support) {
            tiltX = tiltY = 0;
            rotation = 0.0;
        } else {
            ort = localPacketBuf[i].pkOrientation;
            // convert from azimuth and altitude to x tilt and y tilt
            // what follows is the optimized version.  Here are the equations
            // I used to get to this point (in case things change :)
            // X = sin(azimuth) * cos(altitude)
            // Y = cos(azimuth) * cos(altitude)
            // Z = sin(altitude)
            // X Tilt = arctan(X / Z)
            // Y Tilt = arctan(Y / Z)
            double radAzim = (ort.orAzimuth / 10) * (Q_PI / 180);
            //double radAlt = abs(ort.orAltitude / 10) * (Q_PI / 180);
            double tanAlt = tan((abs(ort.orAltitude / 10)) * (Q_PI / 180));

            double degX = atan(sin(radAzim) / tanAlt);
            double degY = atan(cos(radAzim) / tanAlt);
            tiltX = int(degX * (180 / Q_PI));
            tiltY = int(-degY * (180 / Q_PI));
            rotation = ort.orTwist;
        }
#ifndef QT_NO_TABLETEVENT
        QTabletEvent e(t, localPos, globalPos, hiResGlobal, currentTabletPointer.currentDevice,
                       currentTabletPointer.currentPointerType, prsNew, tiltX, tiltY,
                       tangentialPressure, rotation, z, QApplication::keyboardModifiers(), currentTabletPointer.llId);
        sendEvent = QApplication::sendSpontaneousEvent(w, &e);
#endif // QT_NO_TABLETEVENT
    }
    return sendEvent;
}

extern bool qt_is_gui_used;
static void initWinTabFunctions()
{
#if defined(Q_OS_WINCE) 
    return;
#else
    if (!qt_is_gui_used)
        return;

    QLibrary library(QLatin1String("wintab32"));
    if (library.load()) {
        QT_WA({
            ptrWTInfo = (PtrWTInfo)library.resolve("WTInfoW");
            ptrWTGet = (PtrWTGet)library.resolve("WTGetW");
        } , {
            ptrWTInfo = (PtrWTInfo)library.resolve("WTInfoA");
            ptrWTGet = (PtrWTGet)library.resolve("WTGetA");
        });

        ptrWTEnable = (PtrWTEnable)library.resolve("WTEnable");
        ptrWTOverlap = (PtrWTEnable)library.resolve("WTOverlap");
        ptrWTPacketsGet = (PtrWTPacketsGet)library.resolve("WTPacketsGet");
    }
#endif // Q_OS_WINCE
}


//
// Paint event translation
//
bool QETWidget::translatePaintEvent(const MSG &msg)
{
    if (!isWindow() && testAttribute(Qt::WA_NativeWindow))
        Q_ASSERT(internalWinId());

    Q_ASSERT(testAttribute(Qt::WA_WState_Created));
    if (!GetUpdateRect(internalWinId(), 0, FALSE)) { // The update bounding rect is invalid
        d_func()->hd = 0;
        setAttribute(Qt::WA_PendingUpdate, false);
        return false;
    }

    if(msg.message == WM_ERASEBKGND) {
        return true;
    } else {
        setAttribute(Qt::WA_PendingUpdate, false);
        const bool blitToScreen = !d_func()->paintOnScreen() && qt_dirtyRegion(this, true).isEmpty();
        // Make sure the invalidated region contains the region we're about to repaint.
        // BeginPaint will set the clip to the invalidated region and it is impossible
        // to enlarge it afterwards (only shrink it). Using GetDCEx is not suffient
        // as it may return an invalid context (especially on Windows Vista).
        if (!blitToScreen && !d_func()->dirtyOnScreen.isEmpty())
            InvalidateRgn(internalWinId(), d_func()->dirtyOnScreen.handle(), false);
        PAINTSTRUCT ps;
        d_func()->hd = BeginPaint(internalWinId(), &ps);

        const QRect updateRect(QPoint(ps.rcPaint.left, ps.rcPaint.top),
                               QPoint(ps.rcPaint.right, ps.rcPaint.bottom));

        if (blitToScreen) {
            // Nothing to repaint; copy from backing store to screen.
            QWidgetBackingStore::blitToScreen(QRegion(updateRect), this);
        } else {
            // Mapping region from system to qt (32 bit) coordinate system.
            const QRegion rgn(updateRect.translated(data->wrect.topLeft()));
            qt_syncBackingStore(rgn, this, true);
        }

        d_func()->hd = 0;
        EndPaint(internalWinId(), &ps);
    }
    return true;
}

//
// Window move and resize (configure) events
//

bool QETWidget::translateConfigEvent(const MSG &msg)
{
    if (!testAttribute(Qt::WA_WState_Created))                // in QWidget::create()
        return true;
    if (testAttribute(Qt::WA_WState_ConfigPending))
        return true;
    if (!isWindow())
        return true;
    setAttribute(Qt::WA_WState_ConfigPending);                // set config flag
    QRect cr = geometry();
    if (msg.message == WM_SIZE) {                // resize event
        WORD a = LOWORD(msg.lParam);
        WORD b = HIWORD(msg.lParam);
        QSize oldSize = size();
        QSize newSize(a, b);
#ifdef Q_OS_WINCE_WM
        if (isFullScreen() && (oldSize.width() == newSize.height()) && (oldSize.height() == newSize.width()))
            qt_wince_hide_taskbar(internalWinId());
#endif
        cr.setSize(newSize);
        if (msg.wParam != SIZE_MINIMIZED)
            data->crect = cr;
        if (isWindow()) {                        // update title/icon text
            d_func()->createTLExtra();
            // Capture SIZE_MINIMIZED without preceding WM_SYSCOMMAND
            // (like Windows+M)
            if (msg.wParam == SIZE_MINIMIZED && !isMinimized()) {
            data->window_state |= Qt::WindowMinimized;
            if (isVisible()) {
                QHideEvent e;
                QApplication::sendSpontaneousEvent(this, &e);
                hideChildren(true);
            }
        } else if (msg.wParam != SIZE_MINIMIZED && isMinimized()) {
            data->window_state &= ~Qt::WindowMinimized;
            showChildren(true);
            QShowEvent e;
            QApplication::sendSpontaneousEvent(this, &e);
        }
        QString txt;
#ifndef Q_OS_WINCE
        if (IsIconic(internalWinId()) && windowIconText().size())
            txt = windowIconText();
        else
#endif
        txt = windowTitle();
        if (!txt.isEmpty())
            d_func()->setWindowTitle_helper(txt);
    }
    if (msg.wParam != SIZE_MINIMIZED && oldSize != newSize) {
        if (isVisible()) {
            QTLWExtra *tlwExtra = maybeTopData();
            static bool slowResize = qgetenv("QT_SLOW_TOPLEVEL_RESIZE").toInt();
            if (!slowResize && tlwExtra)
                tlwExtra->inTopLevelResize = true;
            QResizeEvent e(newSize, oldSize);
            QApplication::sendSpontaneousEvent(this, &e);
            if (!testAttribute(Qt::WA_StaticContents))
                qt_syncBackingStore(rect(), this);
            if (!slowResize && tlwExtra)
                tlwExtra->inTopLevelResize = false;
        } else {
            QResizeEvent *e = new QResizeEvent(newSize, oldSize);
            QApplication::postEvent(this, e);
        }
    }
} else if (msg.message == WM_MOVE) {        // move event
        int a = (int) (short) LOWORD(msg.lParam);
        int b = (int) (short) HIWORD(msg.lParam);
        QPoint oldPos = geometry().topLeft();
        QPoint newCPos(a, b);
        // Ignore silly Windows move event to wild pos after iconify.
#if !defined(Q_OS_WINCE)
        if (!IsIconic(internalWinId()) && newCPos != oldPos) {
#endif
            cr.moveTopLeft(newCPos);
            data->crect = cr;
            if (isVisible()) {
                QMoveEvent e(newCPos, oldPos);  // cpos (client position)
                QApplication::sendSpontaneousEvent(this, &e);
            } else {
                QMoveEvent * e = new QMoveEvent(newCPos, oldPos);
                QApplication::postEvent(this, e);
            }
#if !defined(Q_OS_WINCE)
        }
#endif
    }
    setAttribute(Qt::WA_WState_ConfigPending, false);                // clear config flag
    return true;
}


//
// Close window event translation.
//
// This class is a friend of QApplication because it needs to emit the
// lastWindowClosed() signal when the last top level widget is closed.
//

bool QETWidget::translateCloseEvent(const MSG &)
{
    return d_func()->close_helper(QWidgetPrivate::CloseWithSpontaneousEvent);
}


void  QApplication::setCursorFlashTime(int msecs)
{
    SetCaretBlinkTime(msecs / 2);
    QApplicationPrivate::cursor_flash_time = msecs;
}


int QApplication::cursorFlashTime()
{
    int blink = (int)GetCaretBlinkTime();
    if (!blink)
        return QApplicationPrivate::cursor_flash_time;
    if (blink > 0)
        return 2*blink;
    return 0;
}


void QApplication::setDoubleClickInterval(int ms)
{
#ifndef Q_OS_WINCE
    SetDoubleClickTime(ms);
#endif
    QApplicationPrivate::mouse_double_click_time = ms;
}

int QApplication::doubleClickInterval()
{
    int ms = GetDoubleClickTime();
    if (ms != 0)
        return ms;
    return QApplicationPrivate::mouse_double_click_time;
}


void QApplication::setKeyboardInputInterval(int ms)
{
    QApplicationPrivate::keyboard_input_time = ms;
}

int QApplication::keyboardInputInterval()
{
    // FIXME: get from the system
    return QApplicationPrivate::keyboard_input_time;
}

#ifndef QT_NO_WHEELEVENT
void QApplication::setWheelScrollLines(int n)
{
#ifdef SPI_SETWHEELSCROLLLINES
    if (n < 0)
        n = 0;
    QT_WA({
        SystemParametersInfo(SPI_SETWHEELSCROLLLINES, (uint)n, 0, 0);
    } , {
        SystemParametersInfoA(SPI_SETWHEELSCROLLLINES, (uint)n, 0, 0);
    });
#else
    QApplicationPrivate::wheel_scroll_lines = n;
#endif
}

int QApplication::wheelScrollLines()
{
#ifdef SPI_GETWHEELSCROLLLINES
    uint i = 3;
    QT_WA({
        SystemParametersInfo(SPI_GETWHEELSCROLLLINES, sizeof(uint), &i, 0);
    } , {
        SystemParametersInfoA(SPI_GETWHEELSCROLLLINES, sizeof(uint), &i, 0);
    });
    if (i > INT_MAX)
        i = INT_MAX;
    return i;
#else
    return QApplicationPrivate::wheel_scroll_lines;
#endif
}
#endif //QT_NO_WHEELEVENT

static bool effect_override = false;

void QApplication::setEffectEnabled(Qt::UIEffect effect, bool enable)
{
    effect_override = true;
    switch (effect) {
    case Qt::UI_AnimateMenu:
        QApplicationPrivate::animate_menu = enable;
        break;
    case Qt::UI_FadeMenu:
        QApplicationPrivate::fade_menu = enable;
        break;
    case Qt::UI_AnimateCombo:
        QApplicationPrivate::animate_combo = enable;
        break;
    case Qt::UI_AnimateTooltip:
        QApplicationPrivate::animate_tooltip = enable;
        break;
    case Qt::UI_FadeTooltip:
        QApplicationPrivate::fade_tooltip = enable;
        break;
    case Qt::UI_AnimateToolBox:
        QApplicationPrivate::animate_toolbox = enable;
        break;
    default:
        QApplicationPrivate::animate_ui = enable;
        break;
    }
}

bool QApplication::isEffectEnabled(Qt::UIEffect effect)
{
    if (QColormap::instance().depth() < 16)
        return false;

    if (!effect_override && desktopSettingsAware()
        && !(QSysInfo::WindowsVersion == QSysInfo::WV_95 || QSysInfo::WindowsVersion == QSysInfo::WV_NT)) {
        // we know that they can be used when we are here
        BOOL enabled = false;
        UINT api;
        switch (effect) {
        case Qt::UI_AnimateMenu:
            api = SPI_GETMENUANIMATION;
            break;
        case Qt::UI_FadeMenu:
            if (QSysInfo::WindowsVersion & QSysInfo::WV_DOS_based)
                return false;
            api = SPI_GETMENUFADE;
            break;
        case Qt::UI_AnimateCombo:
            api = SPI_GETCOMBOBOXANIMATION;
            break;
        case Qt::UI_AnimateTooltip:
            if (QSysInfo::WindowsVersion & QSysInfo::WV_DOS_based)
                api = SPI_GETMENUANIMATION;
            else
                api = SPI_GETTOOLTIPANIMATION;
            break;
        case Qt::UI_FadeTooltip:
            if (QSysInfo::WindowsVersion & QSysInfo::WV_DOS_based)
                return false;
            api = SPI_GETTOOLTIPFADE;
            break;
        default:
            api = SPI_GETUIEFFECTS;
            break;
        }
        QT_WA({
            SystemParametersInfo(api, 0, &enabled, 0);
        } , {
            SystemParametersInfoA(api, 0, &enabled, 0);
        });
        return enabled;
    }

    switch(effect) {
    case Qt::UI_AnimateMenu:
        return QApplicationPrivate::animate_menu;
    case Qt::UI_FadeMenu:
        return QApplicationPrivate::fade_menu;
    case Qt::UI_AnimateCombo:
        return QApplicationPrivate::animate_combo;
    case Qt::UI_AnimateTooltip:
        return QApplicationPrivate::animate_tooltip;
    case Qt::UI_FadeTooltip:
        return QApplicationPrivate::fade_tooltip;
    case Qt::UI_AnimateToolBox:
        return QApplicationPrivate::animate_toolbox;
    default:
        return QApplicationPrivate::animate_ui;
    }
}

#ifndef QT_NO_SESSIONMANAGER

bool QSessionManager::allowsInteraction()
{
    sm_blockUserInput = false;
    return true;
}

bool QSessionManager::allowsErrorInteraction()
{
    sm_blockUserInput = false;
    return true;
}

void QSessionManager::release()
{
    if (sm_smActive)
        sm_blockUserInput = true;
}

void QSessionManager::cancel()
{
    sm_cancel = true;
}

#endif //QT_NO_SESSIONMANAGER

QT_END_NAMESPACE
