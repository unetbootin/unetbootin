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

#include "qsystemtrayicon_p.h"
#ifndef QT_NO_SYSTEMTRAYICON
//#define _WIN32_IE 0x0500
#define _WIN32_IE 0x0600 //required for NOTIFYICONDATAW_V2_SIZE

//missing defines for MINGW :
#ifndef NIN_BALLOONTIMEOUT
#define NIN_BALLOONTIMEOUT  (WM_USER + 4)
#endif
#ifndef NIN_BALLOONUSERCLICK
#define NIN_BALLOONUSERCLICK (WM_USER + 5)
#endif

#include <qt_windows.h>
#include <commctrl.h>
#include <shlwapi.h>
#include <QBitmap>
#include <QLibrary>
#include <QApplication>
#include <QToolTip>
#include <QDesktopWidget>

#if defined(Q_OS_WINCE) && !defined(STANDARDSHELL_UI_MODEL) 
#   include <streams.h>
#endif

QT_BEGIN_NAMESPACE

#if defined(Q_OS_WINCE)
static const UINT q_uNOTIFYICONID = 13;     // IDs from 0 to 12 are reserved on WinCE.
#else
static const UINT q_uNOTIFYICONID = 0;
#endif

static uint MYWM_TASKBARCREATED = 0;
#define MYWM_NOTIFYICON	(WM_APP+101)

typedef BOOL (WINAPI *PtrShell_NotifyIcon)(DWORD,PNOTIFYICONDATA);
static PtrShell_NotifyIcon ptrShell_NotifyIcon = 0;

static void resolveLibs()
{
    static bool triedResolve = false;
#if defined Q_OS_WINCE
    QString libName(QLatin1String("coredll"));
    const char* funcName = "Shell_NotifyIcon";
#else
    QString libName(QLatin1String("shell32"));
    const char* funcName = "Shell_NotifyIconW";
#endif
    if (!triedResolve) {
        QLibrary lib(libName);
	    triedResolve = true;
	    ptrShell_NotifyIcon = (PtrShell_NotifyIcon) lib.resolve(funcName);
    }
}

class QSystemTrayIconSys : QWidget
{
public:
    QSystemTrayIconSys(QSystemTrayIcon *object);
    ~QSystemTrayIconSys();
    bool winEvent( MSG *m, long *result );
    bool trayMessageA(DWORD msg);
    bool trayMessageW(DWORD msg);
    bool trayMessage(DWORD msg);
    bool iconDrawItem(LPDRAWITEMSTRUCT lpdi);
    void setIconContentsW(NOTIFYICONDATAW &data);
    void setIconContentsA(NOTIFYICONDATAA &data);
    bool showMessageW(const QString &title, const QString &message, QSystemTrayIcon::MessageIcon type, uint uSecs);
    bool showMessageA(const QString &title, const QString &message, QSystemTrayIcon::MessageIcon type, uint uSecs);
    bool supportsMessages();
    QRect findIconGeometry(const int a_iButtonID);
    QRect findTrayGeometry();
    HBITMAP createIconMask(const QBitmap &bitmap);
    void createIcon();
    int detectShellVersion() const;
    HICON hIcon;
    QPoint globalPos;
    QSystemTrayIcon *q;
private:
    uint notifyIconSizeW;
    uint notifyIconSizeA;
    int currentShellVersion;
    int maxTipLength;
};

// Checks for the shell32 dll version number, since only version
// 5 or later of supports ballon messages
bool QSystemTrayIconSys::supportsMessages()
{
    if (currentShellVersion >= 5)
        return true;
    else
        return false;
}

//Returns the runtime major version of the shell32 dll
int QSystemTrayIconSys::detectShellVersion() const
{
#ifndef Q_OS_WINCE
    int shellVersion = 4; //NT 4.0 and W95
    DLLGETVERSIONPROC pDllGetVersion = (DLLGETVERSIONPROC)QLibrary::resolve(
            QLatin1String("shell32"), "DllGetVersion");
    if (pDllGetVersion)
    {
        DLLVERSIONINFO dvi;
        HRESULT hr;
        ZeroMemory(&dvi, sizeof(dvi));
        dvi.cbSize = sizeof(dvi);
        hr = (*pDllGetVersion)(&dvi);
        if (SUCCEEDED(hr)) {
            if (dvi.dwMajorVersion >= 5)
            {
                shellVersion = dvi.dwMajorVersion;
            }
        }
    }
    return shellVersion;
#endif
    return 4; //No ballonMessages and MaxTipLength = 64 for WindowsCE
}

QSystemTrayIconSys::QSystemTrayIconSys(QSystemTrayIcon *object)
    : hIcon(0), q(object)
{
    currentShellVersion = detectShellVersion();
    notifyIconSizeA = FIELD_OFFSET(NOTIFYICONDATAA, szTip[64]); // NOTIFYICONDATAA_V1_SIZE
    notifyIconSizeW = FIELD_OFFSET(NOTIFYICONDATAW, szTip[64]); // NOTIFYICONDATAW_V1_SIZE;
    maxTipLength = 64;

#if NOTIFYICON_VERSION >= 3
    if (currentShellVersion >=5) {
        notifyIconSizeA = FIELD_OFFSET(NOTIFYICONDATAA, guidItem); // NOTIFYICONDATAA_V2_SIZE
        notifyIconSizeW = FIELD_OFFSET(NOTIFYICONDATAW, guidItem); // NOTIFYICONDATAW_V2_SIZE;
        maxTipLength = 128;
    }
#endif

    // For restoring the tray icon after explorer crashes
    if (!MYWM_TASKBARCREATED) {
        MYWM_TASKBARCREATED = QT_WA_INLINE(RegisterWindowMessageW(L"TaskbarCreated"),RegisterWindowMessageA("TaskbarCreated"));
    }
}

QSystemTrayIconSys::~QSystemTrayIconSys()
{
    if (hIcon)
        DestroyIcon(hIcon);
}

void QSystemTrayIconSys::setIconContentsW(NOTIFYICONDATAW &tnd)
{
    tnd.uFlags = NIF_MESSAGE|NIF_ICON|NIF_TIP;
    tnd.uCallbackMessage = MYWM_NOTIFYICON;
    tnd.hIcon = hIcon;
    QString tip = q->toolTip();

    if (!tip.isNull()) {
        // Tip is limited to maxTipLength - NULL; lstrcpyn appends a NULL terminator.
        tip = tip.left(maxTipLength - 1) + QChar();
#if defined(Q_OS_WINCE)
        wcsncpy(tnd.szTip, reinterpret_cast<const wchar_t *> (tip.utf16()), qMin(tip.length()+1, maxTipLength));
#else
        lstrcpynW(tnd.szTip, (TCHAR*)tip.utf16(), qMin(tip.length()+1, maxTipLength));
#endif
    }
}

void QSystemTrayIconSys::setIconContentsA(NOTIFYICONDATAA &tnd)
{
    tnd.uFlags = NIF_MESSAGE|NIF_ICON|NIF_TIP;
    tnd.uCallbackMessage = MYWM_NOTIFYICON;
    tnd.hIcon = hIcon;
    QString tip = q->toolTip();

    if (!tip.isNull()) {
        // Tip is limited to maxTipLength - NULL; lstrcpyn appends a NULL terminator.
        tip = tip.left(maxTipLength - 1) + QChar();
#if defined(Q_OS_WINCE)
        strncpy(tnd.szTip, tip.toLocal8Bit().constData(), qMin(tip.length()+1, maxTipLength));
#else
        lstrcpynA(tnd.szTip, tip.toLocal8Bit().constData(), qMin(tip.length()+1, maxTipLength));
#endif
    }
}

int iconFlag( QSystemTrayIcon::MessageIcon icon )
{
    int flag = 0;
#if NOTIFYICON_VERSION >= 3
    switch (icon) {
        case QSystemTrayIcon::NoIcon:
            break;
        case QSystemTrayIcon::Critical:
            flag = NIIF_ERROR;
            break;
        case QSystemTrayIcon::Warning:
            flag = NIIF_WARNING;
            break;
        case QSystemTrayIcon::Information:
        default : // fall through
            flag = NIIF_INFO;
    }
#else
    Q_UNUSED(icon);
#endif
    return flag;
}

bool QSystemTrayIconSys::showMessageW(const QString &title, const QString &message, QSystemTrayIcon::MessageIcon type, uint uSecs)
{
#if NOTIFYICON_VERSION>=3
    NOTIFYICONDATA tnd;
    memset(&tnd, 0, notifyIconSizeW);
    Q_ASSERT(testAttribute(Qt::WA_WState_Created));

    setIconContentsW(tnd);
#if defined(Q_OS_WINCE)
    wcsncpy(tnd.szInfo, message.utf16(), qMin(message.length() + 1, 256));
    wcsncpy(tnd.szInfoTitle, title.utf16(), qMin(title.length()+1, 64));
#else
    lstrcpynW(tnd.szInfo, (TCHAR*)message.utf16(), qMin(message.length() + 1, 256));
    lstrcpynW(tnd.szInfoTitle, (TCHAR*)title.utf16(), qMin(title.length() + 1, 64));
#endif
    tnd.uID = q_uNOTIFYICONID;
    tnd.dwInfoFlags = iconFlag(type);
    tnd.cbSize = notifyIconSizeW;
    tnd.hWnd = winId();
    tnd.uTimeout = uSecs;
    tnd.uFlags = NIF_INFO;
    return ptrShell_NotifyIcon(NIM_MODIFY, &tnd);
#else
    Q_UNUSED(title);
    Q_UNUSED(message);
    Q_UNUSED(type);
    Q_UNUSED(uSecs);
    return false;
#endif
}

bool QSystemTrayIconSys::showMessageA(const QString &title, const QString &message, QSystemTrayIcon::MessageIcon type, uint uSecs)
{
#if NOTIFYICON_VERSION>=3
    NOTIFYICONDATAA tnd;
    memset(&tnd, 0, notifyIconSizeA);
    Q_ASSERT(testAttribute(Qt::WA_WState_Created));

    setIconContentsA(tnd);
#if defined(Q_OS_WINCE)
    strncpy(tnd.szInfo, message.toLocal8Bit().constData(), qMin(message.length() + 1, 256));
    strncpy(tnd.szInfoTitle, title.toLocal8Bit().constData(), qMin(title.length()+1, 64));
#else
    lstrcpynA(tnd.szInfo, message.toLocal8Bit().constData(), qMin(message.length() + 1, 256));
    lstrcpynA(tnd.szInfoTitle, title.toLocal8Bit().constData(), qMin(title.length() + 1, 64));
#endif
    tnd.uID = q_uNOTIFYICONID;
    tnd.dwInfoFlags = iconFlag(type);
    tnd.cbSize = notifyIconSizeA;
    tnd.hWnd = winId();
    tnd.uTimeout = uSecs;
    tnd.uFlags = NIF_INFO;
    return Shell_NotifyIconA(NIM_MODIFY, &tnd);
#else
    Q_UNUSED(title);
    Q_UNUSED(message);
    Q_UNUSED(type);
    Q_UNUSED(uSecs);
    return false;
#endif
}

bool QSystemTrayIconSys::trayMessageA(DWORD msg)
{
#if !defined(Q_OS_WINCE)
    NOTIFYICONDATAA tnd;
    memset(&tnd, 0, notifyIconSizeA);
    tnd.uID = q_uNOTIFYICONID;
    tnd.cbSize = notifyIconSizeA;
    tnd.hWnd = winId();
    Q_ASSERT(testAttribute(Qt::WA_WState_Created));

    if (msg != NIM_DELETE) {
        setIconContentsA(tnd);
    }
    return Shell_NotifyIconA(msg, &tnd);
#else
    Q_UNUSED(msg);
    return false;
#endif
}

bool QSystemTrayIconSys::trayMessageW(DWORD msg)
{
    NOTIFYICONDATAW tnd;
    memset(&tnd, 0, notifyIconSizeW);
    tnd.uID = q_uNOTIFYICONID;
    tnd.cbSize = notifyIconSizeW;
    tnd.hWnd = winId();
    Q_ASSERT(testAttribute(Qt::WA_WState_Created));

    if (msg != NIM_DELETE) {
        setIconContentsW(tnd);
    }
    return ptrShell_NotifyIcon(msg, &tnd);
}

bool QSystemTrayIconSys::trayMessage(DWORD msg)
{
    resolveLibs();
    if (!(ptrShell_NotifyIcon))
        return false;

    QT_WA({
        return trayMessageW(msg);
    },
    {
        return trayMessageA(msg);
    });
}

bool QSystemTrayIconSys::iconDrawItem(LPDRAWITEMSTRUCT lpdi)
{
    if (!hIcon)
	return false;

    DrawIconEx(lpdi->hDC, lpdi->rcItem.left, lpdi->rcItem.top, hIcon, 0, 0, 0, 0, DI_NORMAL);
    return true;
}

HBITMAP QSystemTrayIconSys::createIconMask(const QBitmap &bitmap)
{
    QImage bm = bitmap.toImage().convertToFormat(QImage::Format_Mono);
    int w = bm.width();
    int h = bm.height();
    int bpl = ((w+15)/16)*2;                        // bpl, 16 bit alignment
    uchar *bits = new uchar[bpl*h];
    bm.invertPixels();
    for (int y=0; y<h; y++)
        memcpy(bits+y*bpl, bm.scanLine(y), bpl);
    HBITMAP hbm = CreateBitmap(w, h, 1, 1, bits);
    delete [] bits;
    return hbm;
}

void QSystemTrayIconSys::createIcon()
{
    hIcon = 0;
    QIcon icon = q->icon();
    if (icon.isNull())
        return;

    const int iconSizeX = GetSystemMetrics(SM_CXSMICON);
    const int iconSizeY = GetSystemMetrics(SM_CYSMICON);
    QSize size = icon.actualSize(QSize(iconSizeX, iconSizeY));
    QPixmap pm = icon.pixmap(size);
    if (pm.isNull())
        return;

    QBitmap mask = pm.mask();
    if (mask.isNull()) {
        mask = QBitmap(pm.size());
        mask.fill(Qt::color1);
    }

    HBITMAP im = createIconMask(mask);
    ICONINFO ii;
    ii.fIcon    = true;
    ii.hbmMask  = im;
    ii.hbmColor = pm.toWinHBITMAP(QPixmap::PremultipliedAlpha);
    ii.xHotspot = 0;
    ii.yHotspot = 0;
    hIcon = CreateIconIndirect(&ii);

    DeleteObject(ii.hbmColor);
    DeleteObject(im);
}

bool QSystemTrayIconSys::winEvent( MSG *m, long *result )
{
    switch(m->message) {
    case WM_CREATE:
#ifdef GWLP_USERDATA
        SetWindowLongPtr(winId(), GWLP_USERDATA, (LONG_PTR)((CREATESTRUCTW*)m->lParam)->lpCreateParams);
#else
        SetWindowLong(winId(), GWL_USERDATA, (LONG)((CREATESTRUCTW*)m->lParam)->lpCreateParams);
#endif
        break;

    case WM_DRAWITEM:
	return iconDrawItem((LPDRAWITEMSTRUCT)m->lParam);

    case MYWM_NOTIFYICON:
	{
            RECT r;
            GetWindowRect(winId(), &r);
            QEvent *e = 0;
            Qt::KeyboardModifiers keys = QApplication::keyboardModifiers();
            QPoint gpos = QCursor::pos();

            switch (m->lParam) {
            case WM_LBUTTONUP:
                emit q->activated(QSystemTrayIcon::Trigger);
                break;

#if !defined(Q_OS_WINCE)
            case WM_LBUTTONDBLCLK:
                emit q->activated(QSystemTrayIcon::DoubleClick);
                break;

            case WM_RBUTTONUP:
                if (q->contextMenu()) {
                    q->contextMenu()->popup(gpos);
                    q->contextMenu()->activateWindow();
                    //Must be activated for proper keyboardfocus and menu closing on windows:
                }
                emit q->activated(QSystemTrayIcon::Context);
                break;

            case NIN_BALLOONUSERCLICK:
                emit q->messageClicked();
                break;

            case WM_MBUTTONUP:
                emit q->activated(QSystemTrayIcon::MiddleClick);
                break;
#endif
            default:
		        break;
	    }
	    if (e) {
		bool res = QApplication::sendEvent(q, e);
		delete e;
		return res;
	    }
	    break;
	}
    default:
	if (m->message == MYWM_TASKBARCREATED)
            trayMessage(NIM_ADD);
        else
            return QWidget::winEvent(m, result);
	break;
    }
    return 0;
}

void QSystemTrayIconPrivate::install_sys()
{
    Q_Q(QSystemTrayIcon);
    if (!sys) {
        sys = new QSystemTrayIconSys(q);
        sys->createIcon();
        sys->trayMessage(NIM_ADD);
    }
}

//fallback on win 95/98
QRect QSystemTrayIconSys::findTrayGeometry()
{
    //Use lower right corner as fallback
    QPoint brCorner = qApp->desktop()->screenGeometry().bottomRight();
    QRect ret(brCorner.x() - 10, brCorner.y() - 10, 10, 10);
#if defined(Q_OS_WINCE)
    HWND trayHandle = FindWindowW(L"Shell_TrayWnd", NULL);
#else
    HWND trayHandle = FindWindowA("Shell_TrayWnd", NULL);
#endif
    if (trayHandle) {
#if defined(Q_OS_WINCE)
        trayHandle = FindWindowW(L"TrayNotifyWnd", NULL);
#else
        trayHandle = FindWindowExA(trayHandle, NULL, "TrayNotifyWnd", NULL);
#endif
        if (trayHandle) {
            RECT r;
            if (GetWindowRect(trayHandle, &r)) {
                ret = QRect(r.left, r.top, r.right- r.left, r.bottom - r.top);
            }
        }
    }
    return ret;
}

/*
* This function tries to determine the icon geometry from the tray
*
* If it fails an invalid rect is returned.
*/
QRect QSystemTrayIconSys::findIconGeometry(const int iconId)
{
    QRect ret;
    
    TBBUTTON buttonData;
    DWORD processID = 0;
#if defined(Q_OS_WINCE)
    HWND trayHandle = FindWindowW(L"Shell_TrayWnd", NULL);
#else
    HWND trayHandle = FindWindowA("Shell_TrayWnd", NULL);
#endif

    //find the toolbar used in the notification area
    if (trayHandle) {
#if defined(Q_OS_WINCE)
        trayHandle = FindWindowW(L"TrayNotifyWnd", NULL);
#else
        trayHandle = FindWindowExA(trayHandle, NULL, "TrayNotifyWnd", NULL);
#endif
        if (trayHandle) {
#if defined(Q_OS_WINCE)
            HWND hwnd = FindWindowW(L"SysPager", NULL);
#else
            HWND hwnd = FindWindowEx(trayHandle, NULL, L"SysPager", NULL);
#endif
            if (hwnd) {
#if defined(Q_OS_WINCE)
                hwnd = FindWindow(L"ToolbarWindow32", NULL);
#else
                hwnd = FindWindowEx(hwnd, NULL, L"ToolbarWindow32", NULL);
#endif
                if (hwnd)
                    trayHandle = hwnd;
            }
        }
    }

    if (!trayHandle)
        return ret;

    GetWindowThreadProcessId(trayHandle, &processID);
    if (processID <= 0)
        return ret;

    HANDLE trayProcess = OpenProcess(PROCESS_ALL_ACCESS, 0, processID);
    if (!trayProcess)
        return ret;

    int buttonCount = SendMessage(trayHandle, TB_BUTTONCOUNT, 0, 0);
#if defined(Q_OS_WINCE)
    LPVOID data = VirtualAlloc(NULL, sizeof(TBBUTTON), MEM_COMMIT, PAGE_READWRITE);
#else
    LPVOID data = VirtualAllocEx(trayProcess, NULL, sizeof(TBBUTTON), MEM_COMMIT, PAGE_READWRITE);
#endif

    if ( buttonCount < 1 || !data ) {
	CloseHandle(trayProcess);
        return ret;
    }

    //search for our icon among all toolbar buttons
    for (int toolbarButton = 0; toolbarButton  < buttonCount; ++toolbarButton ) {
        SIZE_T numBytes = 0;
        DWORD appData[2] = { 0, 0 };
        SendMessage(trayHandle, TB_GETBUTTON, toolbarButton , (LPARAM)data);

        if(!ReadProcessMemory(trayProcess, data, &buttonData, sizeof(TBBUTTON), &numBytes))
            continue;

        if(!ReadProcessMemory(trayProcess, (LPVOID) buttonData.dwData, appData, sizeof(appData), &numBytes))
            continue;

        int currentIconId = appData[1];
        HWND currentIconHandle = (HWND) appData[0];
        bool isHidden = buttonData.fsState & TBSTATE_HIDDEN;

        if (currentIconHandle == winId() &&
            currentIconId == iconId && !isHidden) {
            SendMessage(trayHandle, TB_GETITEMRECT, toolbarButton , (LPARAM)data);
            RECT iconRect = {0, 0};
            if(ReadProcessMemory(trayProcess, data, &iconRect, sizeof(RECT), &numBytes)) {
    	        MapWindowPoints(trayHandle, NULL, (LPPOINT)&iconRect, 2);
                QRect geometry(iconRect.left + 1, iconRect.top + 1,
                                iconRect.right - iconRect.left - 2,
                                iconRect.bottom - iconRect.top - 2);
                if (geometry.isValid())
                    ret = geometry;
                break;
            }
        }
    }
#if defined(Q_OS_WINCE)
    VirtualFree(data, 0, MEM_RELEASE);
#else
    VirtualFreeEx(trayProcess, data, 0, MEM_RELEASE);
#endif
    CloseHandle(trayProcess);
    return ret;
}


void QSystemTrayIconPrivate::showMessage_sys(const QString &title, const QString &message, QSystemTrayIcon::MessageIcon type, int timeOut)
{
    if (!sys)
        return;

    uint uSecs = 0;
    if ( timeOut < 0)
        uSecs = 10000; //10 sec default
    else uSecs = (int)timeOut;

    resolveLibs();

    //message is limited to 255 chars + NULL
    QString messageString;
    if (message.isEmpty())
        messageString = QLatin1String(" "); //ensures that the message shows when only title is set
    else
        messageString = message.left(255) + QChar();

    //title is limited to 63 chars + NULL
    QString titleString = title.left(63) + QChar();

    if (sys->supportsMessages()) {
        QT_WA({
            sys->showMessageW(titleString, messageString, type, (unsigned int)uSecs);
        }, {
            sys->showMessageA(titleString, messageString, type, (unsigned int)uSecs);
        });
    } else {
        //use fallbacks
        QRect iconPos = sys->findIconGeometry(0);
        if (iconPos.isValid()) {
            QBalloonTip::showBalloon(type, title, message, sys->q, iconPos.center(), uSecs, true);
        } else {
            QRect trayRect = sys->findTrayGeometry();
            QBalloonTip::showBalloon(type, title, message, sys->q, QPoint(trayRect.left(),
                                     trayRect.center().y()), uSecs, false);
        }
    }
}

QRect QSystemTrayIconPrivate::geometry_sys() const
{
    if (!sys)
        return QRect();
    return sys->findIconGeometry(0);
}

void QSystemTrayIconPrivate::remove_sys()
{
    if (!sys)
	return;

    sys->trayMessage(NIM_DELETE);
    delete sys;
    sys = 0;
}

void QSystemTrayIconPrivate::updateIcon_sys()
{
    if (!sys)
	return;

    HICON hIconToDestroy = sys->hIcon;

    sys->createIcon();
    sys->trayMessage(NIM_MODIFY);

    if (hIconToDestroy)
        DestroyIcon(hIconToDestroy);
}

void QSystemTrayIconPrivate::updateMenu_sys()
{

}

void QSystemTrayIconPrivate::updateToolTip_sys()
{
#ifdef Q_OS_WINCE
    // Calling sys->trayMessage(NIM_MODIFY) on an existing icon is broken on Windows CE.
    // So we need to call updateIcon_sys() which creates a new icon handle.
    updateIcon_sys();
#else
    if (!sys)
	return;

    sys->trayMessage(NIM_MODIFY);
#endif
}

bool QSystemTrayIconPrivate::isSystemTrayAvailable_sys()
{
    return true;
}

QT_END_NAMESPACE

#endif
