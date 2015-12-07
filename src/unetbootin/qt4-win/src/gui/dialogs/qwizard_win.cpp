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

#ifndef QT_NO_WIZARD
#ifndef QT_NO_STYLE_WINDOWSVISTA

#include "qwizard_win_p.h"
#include "qlibrary.h"
#include "qwizard.h"
#include "qpaintengine.h"
#include "qapplication.h"
#include <QtGui/QMouseEvent>
#include <QtGui/QDesktopWidget>

// Note, these tests are duplicates in qwindowsxpstyle_p.h.
#ifdef Q_CC_GNU
#  include <w32api.h>
#  if (__W32API_MAJOR_VERSION >= 3 || (__W32API_MAJOR_VERSION == 2 && __W32API_MINOR_VERSION >= 5))
#    ifdef _WIN32_WINNT
#      undef _WIN32_WINNT
#    endif
#    define _WIN32_WINNT 0x0501
#    include <commctrl.h>
#  endif
#endif

#include <uxtheme.h>

QT_BEGIN_NAMESPACE

//DWM related
typedef struct  {       //MARGINS       
    int cxLeftWidth;    // width of left border that retains its size
    int cxRightWidth;   // width of right border that retains its size
    int cyTopHeight;    // height of top border that retains its size
    int cyBottomHeight; // height of bottom border that retains its size
} WIZ_MARGINS;
typedef struct {        //DTTOPTS
    DWORD dwSize;
    DWORD dwFlags;
    COLORREF crText;
    COLORREF crBorder;
    COLORREF crShadow;
    int eTextShadowType;
    POINT ptShadowOffset;
    int iBorderSize;
    int iFontPropId;
    int iColorPropId;
    int iStateId;
    BOOL fApplyOverlay;
    int iGlowSize;
} WIZ_DTTOPTS;

#define WIZ_DT_CENTER                   0x00000001 //DT_CENTER
#define WIZ_DT_VCENTER                  0x00000004
#define WIZ_DT_SINGLELINE               0x00000020
#define WIZ_DT_NOPREFIX                 0x00000800

enum WIZ_NAVIGATIONPARTS {          //NAVIGATIONPARTS
	WIZ_NAV_BACKBUTTON = 1,
	WIZ_NAV_FORWARDBUTTON = 2,
	WIZ_NAV_MENUBUTTON = 3,
};

enum WIZ_NAV_BACKBUTTONSTATES {     //NAV_BACKBUTTONSTATES
	WIZ_NAV_BB_NORMAL = 1,
	WIZ_NAV_BB_HOT = 2,
	WIZ_NAV_BB_PRESSED = 3,
	WIZ_NAV_BB_DISABLED = 4,
};

#define WIZ_TMT_CAPTIONFONT (801)           //TMT_CAPTIONFONT
#define WIZ_DTT_COMPOSITED  (1UL << 13)     //DTT_COMPOSITED
#define WIZ_DTT_GLOWSIZE    (1UL << 11)     //DTT_GLOWSIZE

#define WIZ_WM_NCMOUSELEAVE 674             //WM_NCMOUSELEAVE

typedef BOOL (WINAPI *PtrDwmDefWindowProc)(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam, LRESULT *plResult);
typedef HRESULT (WINAPI *PtrDwmIsCompositionEnabled)(BOOL* pfEnabled);
typedef HRESULT (WINAPI *PtrDwmExtendFrameIntoClientArea)(HWND hWnd, const WIZ_MARGINS* pMarInset);

static PtrDwmDefWindowProc pDwmDefWindowProc = 0;
static PtrDwmIsCompositionEnabled pDwmIsCompositionEnabled= 0;
static PtrDwmExtendFrameIntoClientArea pDwmExtendFrameIntoClientArea= 0;

//Theme related
typedef bool (WINAPI *PtrIsAppThemed)();
typedef bool (WINAPI *PtrIsThemeActive)();
typedef HANDLE (WINAPI *PtrOpenThemeData)(HWND hwnd, LPCWSTR pszClassList);
typedef HRESULT (WINAPI *PtrCloseThemeData)(HANDLE hTheme);
typedef HRESULT (WINAPI *PtrGetThemeSysFont)(HANDLE hTheme, int iFontId, LOGFONTW *plf);
typedef HRESULT (WINAPI *PtrDrawThemeTextEx)(HANDLE hTheme, HDC hdc, int iPartId, int iStateId, LPCWSTR pszText, int cchText, DWORD dwTextFlags, LPRECT pRect, const WIZ_DTTOPTS *pOptions);
typedef HRESULT (WINAPI *PtrDrawThemeBackground)(HANDLE hTheme, HDC hdc, int iPartId, int iStateId, const RECT *pRect, OPTIONAL const RECT *pClipRect);
typedef HRESULT (WINAPI *PtrGetThemePartSize)(HANDLE hTheme, HDC hdc, int iPartId, int iStateId, OPTIONAL RECT *prc, enum THEMESIZE eSize, OUT SIZE *psz);

static PtrIsAppThemed pIsAppThemed = 0;
static PtrIsThemeActive pIsThemeActive = 0;
static PtrOpenThemeData pOpenThemeData = 0;
static PtrCloseThemeData pCloseThemeData = 0;
static PtrGetThemeSysFont pGetThemeSysFont = 0;
static PtrDrawThemeTextEx pDrawThemeTextEx = 0;
static PtrDrawThemeBackground pDrawThemeBackground = 0;
static PtrGetThemePartSize pGetThemePartSize = 0;

bool QVistaHelper::is_vista = false;

/******************************************************************************
** QVistaBackButton
*/

QVistaBackButton::QVistaBackButton(QWidget *widget)
    : QAbstractButton(widget)
{
    setFocusPolicy(Qt::NoFocus);
}

QSize QVistaBackButton::sizeHint() const
{
    ensurePolished();
    int width = 32, height = 32;
/*
    HANDLE theme = pOpenThemeData(0, L"Navigation");
    SIZE size;
    if (pGetThemePartSize(theme, 0, WIZ_NAV_BACKBUTTON, WIZ_NAV_BB_NORMAL, 0, TS_TRUE, &size) == S_OK) {
        width = size.cx;
        height = size.cy;
    }
*/
    return QSize(width, height);
}

void QVistaBackButton::enterEvent(QEvent *event)
{
    if (isEnabled())
        update();
    QAbstractButton::enterEvent(event);
}

void QVistaBackButton::leaveEvent(QEvent *event)
{
    if (isEnabled())
        update();
    QAbstractButton::leaveEvent(event);
}

void QVistaBackButton::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    QRect r = rect();
    HANDLE theme = pOpenThemeData(0, L"Navigation");
    //RECT rect;
    RECT clipRect;
    int xoffset = QWidget::mapToParent(r.topLeft()).x();
    int yoffset = QWidget::mapToParent(r.topLeft()).y();

    clipRect.top = r.top() + yoffset;
    clipRect.bottom = r.bottom() + yoffset;
    clipRect.left = r.left() + xoffset;
    clipRect.right = r.right()  + xoffset;

    int state = WIZ_NAV_BB_NORMAL;
    if (!isEnabled())
        state = WIZ_NAV_BB_DISABLED;
    else if (isDown())
        state = WIZ_NAV_BB_PRESSED;
    else if (underMouse())
        state = WIZ_NAV_BB_HOT;

   pDrawThemeBackground(theme, p.paintEngine()->getDC(), WIZ_NAV_BACKBUTTON, state, &clipRect, &clipRect); 
}

/******************************************************************************
** QVistaHelper
*/

QVistaHelper::QVistaHelper(QWizard *wizard)
    : pressed(false)
    , wizard(wizard)
{
    is_vista = resolveSymbols();
    backButton_ = new QVistaBackButton(wizard);
}

QVistaHelper::~QVistaHelper()
{
}

bool QVistaHelper::isCompositionEnabled()
{
    bool value = is_vista;
    if (is_vista) {
        HRESULT hr;
        BOOL bEnabled;

        hr = pDwmIsCompositionEnabled(&bEnabled);
        value = (SUCCEEDED(hr) && bEnabled);
    }
    return value;
}

bool QVistaHelper::setDWMTitleBar(TitleBarChangeType type)
{
    bool value = false;
    if (isCompositionEnabled()) {
        WIZ_MARGINS mar = {0};
        if (type == NormalTitleBar)
            mar.cyTopHeight = 0;
        else
            mar.cyTopHeight = titleBarSize() + topOffset();
        HRESULT hr = pDwmExtendFrameIntoClientArea(wizard->winId(), &mar);
        value = SUCCEEDED(hr);
    }
    return value;
}

void QVistaHelper::drawTitleBar(QPainter *painter)
{
    drawBlackRect(
        QRect(0, 0, wizard->width(), titleBarSize() + topOffset()),
        painter->paintEngine()->getDC());

    const int btnTop = backButton_->mapToParent(QPoint()).y();
    const int btnHeight = backButton_->size().height();
    const int verticalCenter = (btnTop + btnHeight / 2);

    wizard->windowIcon().paint(
        painter, QRect(leftMargin(), verticalCenter - iconSize() / 2, iconSize(), iconSize()));

    const QString text = wizard->window()->windowTitle();
    const QFont font = QApplication::font("QWorkspaceTitleBar");
    const QFontMetrics fontMetrics(font);
    const QRect brect = fontMetrics.boundingRect(text);
    const int textHeight = brect.height() + 2*glowSize();
    const int textWidth = brect.width() + 2*glowSize();
    drawTitleText(
        text,
        QRect(titleOffset(), verticalCenter - textHeight / 2, textWidth, textHeight),
        painter->paintEngine()->getDC());
}

bool QVistaHelper::winEvent(MSG* msg, long* result)
{
    if (!isCompositionEnabled())
        return false;

    bool retval = true;

    switch (msg->message) {
    case WM_NCHITTEST: {
        LRESULT lResult;
        pDwmDefWindowProc(msg->hwnd, msg->message, msg->wParam, msg->lParam, &lResult);
        if (lResult == HTCLOSE || lResult == HTMAXBUTTON || lResult == HTMINBUTTON || lResult == HTHELP)
            *result = lResult;
        else
            *result = DefWindowProc(msg->hwnd, msg->message, msg->wParam, msg->lParam);
        break;
    }
    case WM_NCMOUSEMOVE:
    case WM_NCLBUTTONDOWN:
    case WM_NCLBUTTONUP:
    case WIZ_WM_NCMOUSELEAVE: {
        LRESULT lResult;
        pDwmDefWindowProc(msg->hwnd, msg->message, msg->wParam, msg->lParam, &lResult);
        *result = DefWindowProc(msg->hwnd, msg->message, msg->wParam, msg->lParam);
        break;
    }
    case WM_NCCALCSIZE: {
        NCCALCSIZE_PARAMS* lpncsp = (NCCALCSIZE_PARAMS*)msg->lParam;
        *result = DefWindowProc(msg->hwnd, msg->message, msg->wParam, msg->lParam);
        lpncsp->rgrc[0].top -= titleBarSize();
        break;
    }
    default:
        retval = false;
    }

    return retval;
}

void QVistaHelper::setMouseCursor(QPoint pos)
{
    if (rtTop.contains(pos))
        wizard->setCursor(Qt::SizeVerCursor);
    else
        wizard->setCursor(Qt::ArrowCursor);
}

void QVistaHelper::mouseEvent(QEvent *event)
{
    switch (event->type()) {
    case QEvent::MouseMove:
        mouseMoveEvent(static_cast<QMouseEvent *>(event));
        break;
    case QEvent::MouseButtonPress:
        mousePressEvent(static_cast<QMouseEvent *>(event));
        break;
    case QEvent::MouseButtonRelease:
        mouseReleaseEvent(static_cast<QMouseEvent *>(event));
        break;
    default:
        break;
    }
}

// The following hack ensures that the titlebar is updated correctly
// when the wizard style changes to and from AeroStyle. Specifically,
// this function causes a Windows message of type WM_NCCALCSIZE to
// be triggered.
void QVistaHelper::setWindowPosHack()
{
    const int x = wizard->geometry().x(); // ignored by SWP_NOMOVE
    const int y = wizard->geometry().y(); // ignored by SWP_NOMOVE
    const int w = wizard->width();
    const int h = wizard->height();
    SetWindowPos(wizard->winId(), 0, x, y, w, h, SWP_NOMOVE | SWP_NOZORDER | SWP_FRAMECHANGED);
}

// The following hack allows any QWidget subclass to access
// QWidgetPrivate::topData() without being declared as a
// friend by QWidget.
class QHackWidget : public QWidget
{
public:
    Q_DECLARE_PRIVATE(QWidget)
    QTLWExtra* topData() { return d_func()->topData(); }
};

void QVistaHelper::collapseTopFrameStrut()
{
    QTLWExtra *top = ((QHackWidget *)wizard)->d_func()->topData();
    int x1, y1, x2, y2;
    top->frameStrut.getCoords(&x1, &y1, &x2, &y2);
    top->frameStrut.setCoords(x1, 0, x2, y2);
}

bool QVistaHelper::handleWinEvent(MSG *message, long *result)
{
    bool status = false;
    if (wizard->wizardStyle() == QWizard::AeroStyle) {
        status = winEvent(message, result);
        if (message->message == WM_NCCALCSIZE) {
            if (status)
                collapseTopFrameStrut();
        } else if (message->message == WM_NCPAINT) {
            wizard->update();
        }
    }
    return status;
}

void QVistaHelper::resizeEvent(QResizeEvent * event)
{
    Q_UNUSED(event);
    rtTop = QRect (0, 0, wizard->width(), frameSize());
    rtTitle = QRect (0, frameSize(), wizard->width(), captionSize() + topOffset());
}

void QVistaHelper::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(wizard);
    drawTitleBar(&painter);
}

void QVistaHelper::mouseMoveEvent(QMouseEvent *event)
{
    if (wizard->windowState() & Qt::WindowMaximized) {
        event->ignore();
        return;
    }

    QRect rect = wizard->geometry();
    if (pressed) {
        switch (change) {
        case resizeTop:
            {
                const int dy = event->pos().y() - pressedPos.y();
                if ((dy > 0 && rect.height() > wizard->minimumHeight())
                    || (dy < 0 && rect.height() < wizard->maximumHeight()))
                    rect.setTop(rect.top() + dy);
            }
            break;
        case movePosition: {
            QPoint newPos = event->pos() - pressedPos;
            rect.moveLeft(rect.left() + newPos.x());
            rect.moveTop(rect.top() + newPos.y());
            break; }
        default:
            break;
        }
        wizard->setGeometry(rect);

    } else {
        setMouseCursor(event->pos());
    }
    event->ignore();
}

void QVistaHelper::mousePressEvent(QMouseEvent *event)
{
    change = noChange;

    if (wizard->windowState() & Qt::WindowMaximized) {
        event->ignore();
        return;
    }

    if (rtTop.contains(event->pos()))
        change = resizeTop;
    else if (rtTitle.contains(event->pos()))
        change = movePosition;

    if (change != noChange) {
        setMouseCursor(event->pos());
        pressed = true;
        pressedPos = event->pos();
    } else {
        event->ignore();
    }
}

void QVistaHelper::mouseReleaseEvent(QMouseEvent *event)
{
    change = noChange;
    if (pressed) {
        pressed = false;
        wizard->releaseMouse();
        setMouseCursor(event->pos());
    }
    event->ignore();
}

bool QVistaHelper::eventFilter(QObject *obj, QEvent *event)
{
    if (obj != wizard)
        return QObject::eventFilter(obj, event);

    if (event->type() == QEvent::MouseMove) {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
        long result;
        MSG msg;
        msg.message = WM_NCHITTEST;
        msg.wParam  = 0;
        msg.lParam = MAKELPARAM(mouseEvent->globalX(), mouseEvent->globalY());
        msg.hwnd = wizard->winId();
        winEvent(&msg, &result);
        msg.wParam = result;
        msg.message = WM_NCMOUSEMOVE;
        winEvent(&msg, &result);
     } else if (event->type() == QEvent::MouseButtonPress) {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
        long result;
        MSG msg;
        msg.message = WM_NCHITTEST;
        msg.wParam  = 0;
        msg.lParam = MAKELPARAM(mouseEvent->globalX(), mouseEvent->globalY());
        msg.hwnd = wizard->winId();
        winEvent(&msg, &result);
        msg.wParam = result;
        msg.message = WM_NCLBUTTONDOWN;
        winEvent(&msg, &result);
     } else if (event->type() == QEvent::MouseButtonRelease) {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
        long result;
        MSG msg;
        msg.message = WM_NCHITTEST;
        msg.wParam  = 0;
        msg.lParam = MAKELPARAM(mouseEvent->globalX(), mouseEvent->globalY());
        msg.hwnd = wizard->winId();
        winEvent(&msg, &result);
        msg.wParam = result;
        msg.message = WM_NCLBUTTONUP;
        winEvent(&msg, &result);
     }

     return false;
}

HFONT QVistaHelper::getCaptionFont(HANDLE hTheme)
{
    LOGFONT lf = {0};

    if (!hTheme)
        pGetThemeSysFont(hTheme, WIZ_TMT_CAPTIONFONT, &lf);
    else
    {
        NONCLIENTMETRICS ncm = {sizeof(NONCLIENTMETRICS)};
        SystemParametersInfo(SPI_GETNONCLIENTMETRICS, sizeof(NONCLIENTMETRICS), &ncm, false);
        lf = ncm.lfMessageFont;
    }
    return CreateFontIndirect(&lf);
}

bool QVistaHelper::drawTitleText(const QString &text, const QRect &rect, HDC hdc)
{
    bool value = false;
    if(isCompositionEnabled()){
        HANDLE hTheme= pOpenThemeData(qApp->desktop()->winId(), L"WINDOW");
        if (!hTheme) return false;
        // Set up a memory DC and bitmap that we'll draw into
        HDC dcMem;
        HBITMAP bmp;
        BITMAPINFO dib = {0};
        dcMem = CreateCompatibleDC(hdc);

        dib.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
        dib.bmiHeader.biWidth = rect.width();
        dib.bmiHeader.biHeight = -rect.height();
        dib.bmiHeader.biPlanes = 1;
        dib.bmiHeader.biBitCount = 32;
        dib.bmiHeader.biCompression = BI_RGB;
 
        bmp = CreateDIBSection(hdc, &dib, DIB_RGB_COLORS, NULL, NULL, 0);

        // Set up the DC
        HFONT hCaptionFont = getCaptionFont(hTheme);
        HBITMAP hOldBmp = (HBITMAP)SelectObject(dcMem, (HGDIOBJ) bmp);
        HFONT hOldFont = (HFONT)SelectObject(dcMem, (HGDIOBJ) hCaptionFont);
 
        // Draw the text!
        WIZ_DTTOPTS dto = { sizeof(WIZ_DTTOPTS) };
        const UINT uFormat = WIZ_DT_SINGLELINE|WIZ_DT_CENTER|WIZ_DT_VCENTER|WIZ_DT_NOPREFIX;
        RECT rctext ={0,0, rect.width(), rect.height()};

        dto.dwFlags = WIZ_DTT_COMPOSITED|WIZ_DTT_GLOWSIZE;
        dto.iGlowSize = glowSize();
 
        pDrawThemeTextEx(hTheme, dcMem, 0, 0, (LPCWSTR)text.utf16(), -1, uFormat, &rctext, &dto );
        BitBlt(hdc, rect.left(), rect.top(), rect.width(), rect.height(), dcMem, 0, 0, SRCCOPY);
        SelectObject(dcMem, (HGDIOBJ) hOldBmp);
        SelectObject(dcMem, (HGDIOBJ) hOldFont);
        DeleteObject(bmp);
        DeleteObject(hCaptionFont);
        DeleteDC(dcMem);
        //ReleaseDC(hwnd, hdc);
    }
    return value;
}

bool QVistaHelper::drawBlackRect(const QRect &rect, HDC hdc)
{
    bool value = false;
    if (isCompositionEnabled()){
        // Set up a memory DC and bitmap that we'll draw into
        HDC dcMem;
        HBITMAP bmp;
        BITMAPINFO dib = {0};
        dcMem = CreateCompatibleDC(hdc);

        dib.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
        dib.bmiHeader.biWidth = rect.width();
        dib.bmiHeader.biHeight = -rect.height();
        dib.bmiHeader.biPlanes = 1;
        dib.bmiHeader.biBitCount = 32;
        dib.bmiHeader.biCompression = BI_RGB;
 
        bmp = CreateDIBSection(hdc, &dib, DIB_RGB_COLORS, NULL, NULL, 0);
        HBITMAP hOldBmp = (HBITMAP)SelectObject(dcMem, (HGDIOBJ) bmp);

        BitBlt(hdc, rect.left(), rect.top(), rect.width(), rect.height(), dcMem, 0, 0, SRCCOPY);
        SelectObject(dcMem, (HGDIOBJ) hOldBmp);

        DeleteObject(bmp);
        DeleteDC(dcMem);
    }
    return value;
}

bool QVistaHelper::resolveSymbols()
{
    static bool tried = false;
    if (!tried) {
        tried = true;
        QLibrary dwmLib(QString::fromAscii("dwmapi"));
        pDwmIsCompositionEnabled =
            (PtrDwmIsCompositionEnabled)dwmLib.resolve("DwmIsCompositionEnabled");
        if (pDwmIsCompositionEnabled) {
            pDwmDefWindowProc = (PtrDwmDefWindowProc)dwmLib.resolve("DwmDefWindowProc");
            pDwmExtendFrameIntoClientArea =
                (PtrDwmExtendFrameIntoClientArea)dwmLib.resolve("DwmExtendFrameIntoClientArea");
        }
        QLibrary themeLib(QString::fromAscii("uxtheme"));
        pIsAppThemed = (PtrIsAppThemed)themeLib.resolve("IsAppThemed");
        if (pIsAppThemed) {
            pDrawThemeBackground = (PtrDrawThemeBackground)themeLib.resolve("DrawThemeBackground");
            pGetThemePartSize = (PtrGetThemePartSize)themeLib.resolve("GetThemePartSize");
            pIsThemeActive = (PtrIsThemeActive)themeLib.resolve("IsThemeActive");
            pOpenThemeData = (PtrOpenThemeData)themeLib.resolve("OpenThemeData");
            pCloseThemeData = (PtrCloseThemeData)themeLib.resolve("CloseThemeData");
            pGetThemeSysFont = (PtrGetThemeSysFont)themeLib.resolve("GetThemeSysFont");
            pDrawThemeTextEx = (PtrDrawThemeTextEx)themeLib.resolve("DrawThemeTextEx");
        }
    }

    return (
        pDwmIsCompositionEnabled != 0
        && pDwmDefWindowProc != 0
        && pDwmExtendFrameIntoClientArea != 0
        && pIsAppThemed != 0
        && pDrawThemeBackground != 0
        && pGetThemePartSize != 0
        && pIsThemeActive != 0
        && pOpenThemeData != 0
        && pCloseThemeData != 0
        && pGetThemeSysFont != 0
        && pDrawThemeTextEx != 0);
}

int QVistaHelper::titleOffset()
{
    int iconOffset = wizard ->windowIcon().isNull() ? 0 : iconSize() + padding();
    return leftMargin() + iconOffset;
}

QT_END_NAMESPACE

#endif // QT_NO_STYLE_WINDOWSVISTA

#endif // QT_NO_WIZARD
