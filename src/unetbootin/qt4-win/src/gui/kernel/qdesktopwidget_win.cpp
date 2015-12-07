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

#include "qdesktopwidget.h"
#include "qt_windows.h"
#include "qapplication_p.h"
#include "qlibrary.h"
#include <qvector.h>
#include <limits.h>
#ifdef Q_OS_WINCE
#include <sipapi.h>
#endif
#include "qwidget_p.h"
#include "qdebug.h"

QT_BEGIN_NAMESPACE

class QDesktopWidgetPrivate : public QWidgetPrivate
{
public:
    QDesktopWidgetPrivate();
    ~QDesktopWidgetPrivate();

    static void init(QDesktopWidget *that);
    static void cleanup();
    static int screenCount;
    static int primaryScreen;

    static QVector<QRect> *rects;
    static QVector<QRect> *workrects;

    struct MONITORINFO
    {
        DWORD   cbSize;
        RECT    rcMonitor;
        RECT    rcWork;
        DWORD   dwFlags;
    };

    typedef BOOL (WINAPI *InfoFunc)(HMONITOR, MONITORINFO*);
    typedef BOOL (CALLBACK *EnumProc)(HMONITOR, HDC, LPRECT, LPARAM);
    typedef BOOL (WINAPI *EnumFunc)(HDC, LPCRECT, EnumProc, LPARAM);

    static EnumFunc enumDisplayMonitors;
    static InfoFunc getMonitorInfo;
    static int refcount;
};

int QDesktopWidgetPrivate::screenCount = 1;
int QDesktopWidgetPrivate::primaryScreen = 0;
QDesktopWidgetPrivate::EnumFunc QDesktopWidgetPrivate::enumDisplayMonitors = 0;
QDesktopWidgetPrivate::InfoFunc QDesktopWidgetPrivate::getMonitorInfo = 0;
QVector<QRect> *QDesktopWidgetPrivate::rects = 0;
QVector<QRect> *QDesktopWidgetPrivate::workrects = 0;
static int screen_number = 0;
int QDesktopWidgetPrivate::refcount = 0;
#ifdef Q_OS_WINCE_WM
// Use SIP information, if available
// SipGetInfo is not supported by SSDK (no definition!). 
static inline void qt_get_sip_info(QRect &rect)
{
    SIPINFO sip;
    memset(&sip, 0, sizeof(SIPINFO));
    sip.cbSize = sizeof(SIPINFO);
    if (SipGetInfo(&sip))
        rect = QRect(QPoint(sip.rcVisibleDesktop.left, sip.rcVisibleDesktop.top),
        QPoint(sip.rcVisibleDesktop.right - 1, sip.rcVisibleDesktop.bottom - 1));
}
#endif


BOOL CALLBACK enumCallback(HMONITOR hMonitor, HDC, LPRECT, LPARAM)
{
    QDesktopWidgetPrivate::screenCount++;
    QDesktopWidgetPrivate::rects->resize(QDesktopWidgetPrivate::screenCount);
    QDesktopWidgetPrivate::workrects->resize(QDesktopWidgetPrivate::screenCount);
    // Get the MONITORINFO block
    QDesktopWidgetPrivate::MONITORINFO info;
    memset(&info, 0, sizeof(QDesktopWidgetPrivate::MONITORINFO));
    info.cbSize = sizeof(QDesktopWidgetPrivate::MONITORINFO);
    BOOL res = QDesktopWidgetPrivate::getMonitorInfo(hMonitor, &info);
    if (!res) {
        (*QDesktopWidgetPrivate::rects)[screen_number] = QRect();
        (*QDesktopWidgetPrivate::workrects)[screen_number] = QRect();
        return true;
    }

    // Fill list of rects
    RECT r = info.rcMonitor;
    QRect qr(QPoint(r.left, r.top), QPoint(r.right - 1, r.bottom - 1));
    (*QDesktopWidgetPrivate::rects)[screen_number] = qr;

    r = info.rcWork;
    qr = QRect(QPoint(r.left, r.top), QPoint(r.right - 1, r.bottom - 1));
    (*QDesktopWidgetPrivate::workrects)[screen_number] = qr;

    if (info.dwFlags & 0x00000001) //MONITORINFOF_PRIMARY
        QDesktopWidgetPrivate::primaryScreen = screen_number;

    ++screen_number;
    // Stop the enumeration if we have them all
    return true;
}

QDesktopWidgetPrivate::QDesktopWidgetPrivate()
{
    ++refcount;
}

void QDesktopWidgetPrivate::init(QDesktopWidget *that)
{
    if (rects)
        return;

    rects = new QVector<QRect>();
    workrects = new QVector<QRect>();

#ifndef Q_OS_WINCE
    if (QSysInfo::WindowsVersion != QSysInfo::WV_95 && QSysInfo::WindowsVersion != QSysInfo::WV_NT) {
        screenCount = 0;
        // Trying to get the function pointers to Win98/2000 only functions
        QLibrary user32Lib(QLatin1String("user32"));
        if (!user32Lib.load())
            return;
        enumDisplayMonitors = (EnumFunc)user32Lib.resolve("EnumDisplayMonitors");
        QT_WA({
            getMonitorInfo = (InfoFunc)user32Lib.resolve("GetMonitorInfoW");
        } , {
            getMonitorInfo = (InfoFunc)user32Lib.resolve("GetMonitorInfoA");
        });

        if (!enumDisplayMonitors || !getMonitorInfo) {
            screenCount = GetSystemMetrics(80);  // SM_CMONITORS
            rects->resize(screenCount);
            for (int i = 0; i < screenCount; ++i)
                rects->replace(i, that->rect());
            return;
        }
        // Calls enumCallback
        enumDisplayMonitors(0, 0, enumCallback, 0);
        enumDisplayMonitors = 0;
        getMonitorInfo = 0;
    } else {
        rects->resize(1);
        rects->replace(0, that->rect());
        workrects->resize(1);
        workrects->replace(0, that->rect());
    }
#else
    screenCount = 0;

    QLibrary coreLib(QLatin1String("coredll"));
    if (coreLib.load()) {
        // CE >= 4.0 case
        enumDisplayMonitors = (EnumFunc)coreLib.resolve("EnumDisplayMonitors");
        getMonitorInfo = (InfoFunc)coreLib.resolve("GetMonitorInfo");
    }

    if ((!enumDisplayMonitors || !getMonitorInfo)) {
        screenCount = GetSystemMetrics(SM_CMONITORS);
        return;
    }

    if (!coreLib.isLoaded() || !enumDisplayMonitors || !getMonitorInfo) {
        rects->resize(screenCount);
        for (int i = 0; i < screenCount; ++i)
            (*rects)[i] = that->rect();

        RECT r;
        SystemParametersInfo(SPI_GETWORKAREA, 0, &r, 0);
        QRect qr = QRect(QPoint(r.left, r.top), QPoint(r.right - 1, r.bottom - 1));

#if defined(Q_OS_WINCE_WM)
        qt_get_sip_info(qr);
#endif

        workrects->resize(screenCount);
        for (int j = 0; j < screenCount; ++j)
            (*workrects)[j] = qr;
        return;
    }

    // Calls enumCallback
    enumDisplayMonitors(0, 0, enumCallback, 0);
    enumDisplayMonitors = 0;
    getMonitorInfo = 0;
#endif // Q_OS_WINCE
}

QDesktopWidgetPrivate::~QDesktopWidgetPrivate()
{
    if (!--refcount)
        cleanup();
}

void QDesktopWidgetPrivate::cleanup()
{
    screen_number = 0;
    screenCount = 1;
    primaryScreen = 0;
    enumDisplayMonitors = 0;
    getMonitorInfo = 0;
    delete rects;
    rects = 0;
    delete workrects;
    workrects = 0;
}

/*
    \omit
    Function is commented out in header
    \fn void *QDesktopWidget::handle(int screen) const

    Returns the window system handle of the display device with the
    index \a screen, for low-level access.  Using this function is not
    portable.

    The return type varies with platform; see qwindowdefs.h for details.

    \sa x11Display(), QPaintDevice::handle()
    \endomit
*/

/*!
    \class QDesktopWidget
    \brief The QDesktopWidget class provides access to screen information on multi-head systems.

    \ingroup advanced
    \ingroup desktop
    \ingroup environment
    \mainclass

    Systems with more than one graphics card and monitor can manage the
    physical screen space available either as multiple desktops, or as a
    large virtual desktop, which usually has the size of the bounding
    rectangle of all the screens (see isVirtualDesktop()). For an
    application, one of the available screens is the primary screen, i.e.
    the screen where the main widget resides (see primaryScreen()). All
    windows opened in the context of the application should be
    constrained to the boundaries of the primary screen; for example,
    it would be inconvenient if a dialog box popped up on a different
    screen, or split over two screens.

    The QDesktopWidget provides information about the geometry of the
    available screens with screenGeometry(). The number of screens
    available is returned by numScreens(). The screen number that a
    particular point or widget is located in is returned by
    screenNumber().

    Widgets provided by Qt use this class, for example, to place
    tooltips, menus and dialog boxes according to the parent or
    application widget.

    Applications can use this class to save window positions, or to place
    child widgets on one screen.

    \img qdesktopwidget.png Managing Multiple Screens

    In the illustration above, Application One's primary screen is
    screen 0, and App Two's primary screen is screen 1.

    \sa QApplication, QX11Info::appRootWindow()
*/

/*!
    Creates the desktop widget.

    If the system supports a virtual desktop, this widget will have
    the size of the virtual desktop; otherwise this widget will have
    the size of the primary screen.

    Instead of using QDesktopWidget directly, use QApplication::desktop().
*/
QDesktopWidget::QDesktopWidget()
    : QWidget(*new QDesktopWidgetPrivate, 0, Qt::Desktop)
{
    setObjectName(QLatin1String("desktop"));
    QDesktopWidgetPrivate::init(this);
}

/*!
    Destroy the object and free allocated resources.
*/
QDesktopWidget::~QDesktopWidget()
{
}

/*!
    Returns true if the system manages the available screens in a
    virtual desktop; otherwise returns false.

    For virtual desktops, screen() will always return the same widget.
    The size of the virtual desktop is the size of this desktop
    widget.
*/
bool QDesktopWidget::isVirtualDesktop() const
{
    return true;
}

/*!
    Returns the index of the primary screen.

    \sa numScreens()
*/
int QDesktopWidget::primaryScreen() const
{
    return d_func()->primaryScreen;
}

/*!
    Returns the number of available screens.

    \sa primaryScreen()
*/
int QDesktopWidget::numScreens() const
{
    return d_func()->screenCount;
}

/*!
    Returns a widget that represents the screen with index \a screen.

    If the system uses a virtual desktop, the returned widget will
    have the geometry of the entire virtual desktop; i.e., bounding
    every \a screen.

    \sa primaryScreen(), numScreens(), isVirtualDesktop()
*/
QWidget *QDesktopWidget::screen(int /* screen */)
{
    // It seems that a Qt::WType_Desktop cannot be moved?
    return this;
}

/*!
  Returns the available geometry of the screen with index \a screen. What
  is available will be subrect of screenGeometry() based on what the
  platform decides is available (for example excludes the dock and menu bar
  on Mac OS X, or the task bar on Windows).

  \sa screenNumber(), screenGeometry()
*/

//
// MSVC 7.10 warns that d (the result of the expanded Q_D macro) as a local variable that is not referenced.
// Therefore, we ignore that warning with the following pragmas
// I've also tried to eliminate the macro, but to no use...
// We pop it further down
#ifdef Q_CC_MSVC
# pragma warning(push)
# pragma warning(disable : 4189)
#endif
const QRect QDesktopWidget::availableGeometry(int screen) const
{
    Q_D(const QDesktopWidget);
#ifdef Q_OS_WINCE_WM
    for(int i=0; i < d->workrects->size(); ++i)
        qt_get_sip_info((*d->workrects)[i]);
#endif
    if (QSysInfo::WindowsVersion != QSysInfo::WV_95 && QSysInfo::WindowsVersion != QSysInfo::WV_NT) {
        if (screen < 0 || screen >= d->screenCount)
            screen = d->primaryScreen;

        return d->workrects->at(screen);
    } else {
        return d->workrects->at(d->primaryScreen);
    }
}

/*!
    \fn const QRect QDesktopWidget::availableGeometry(const QWidget *widget) const
    \overload

    Returns the available geometry of the screen which contains \a widget.

    \sa screenGeometry()
*/

/*!
    \fn const QRect QDesktopWidget::availableGeometry(const QPoint &p) const
    \overload

    Returns the available geometry of the screen which contains \a p.

    \sa screenGeometry()
*/


/*!
    Returns the geometry of the screen with index \a screen.

    \sa screenNumber()
*/
const QRect QDesktopWidget::screenGeometry(int screen) const
{
    const QDesktopWidgetPrivate *d = d_func();
    if (QSysInfo::WindowsVersion != QSysInfo::WV_95 && QSysInfo::WindowsVersion != QSysInfo::WV_NT) {
        if (screen < 0 || screen >= d->screenCount)
            screen = d->primaryScreen;

        return d->rects->at(screen);
    } else {
        return d->rects->at(d->primaryScreen);
    }
}

/*!
    \fn const QRect QDesktopWidget::screenGeometry(const QWidget *widget) const
    \overload

    Returns the geometry of the screen which contains \a widget.
*/

/*!
    \fn const QRect QDesktopWidget::screenGeometry(const QPoint &p) const
    \overload

    Returns the geometry of the screen which contains \a p.
*/


/*!
    Returns the index of the screen that contains the largest
    part of \a widget, or -1 if the widget not on a screen.

    \sa primaryScreen()
*/
int QDesktopWidget::screenNumber(const QWidget *widget) const
{
    Q_D(const QDesktopWidget);
    if (QSysInfo::WindowsVersion != QSysInfo::WV_95 && QSysInfo::WindowsVersion != QSysInfo::WV_NT) {
        if (!widget)
            return d->primaryScreen;
        QRect frame = widget->frameGeometry();
        if (!widget->isWindow())
            frame.moveTopLeft(widget->mapToGlobal(QPoint(0,0)));

        int maxSize = -1;
        int maxScreen = -1;

        for (int i = 0; i < d->screenCount; ++i) {
            QRect sect = d->rects->at(i).intersected(frame);
            int size = sect.width() * sect.height();
            if (size > maxSize && sect.width() > 0 && sect.height() > 0) {
                maxSize = size;
                maxScreen = i;
            }
        }
        return maxScreen;
    } else {
        return d->primaryScreen;
    }
}

/*!
    \overload
    Returns the index of the screen that contains the \a point, or the
    screen which is the shortest distance from the \a point.

    \sa primaryScreen()
*/

int QDesktopWidget::screenNumber(const QPoint &point) const
{
    Q_D(const QDesktopWidget);
    int closestScreen = -1;
    if (QSysInfo::WindowsVersion != QSysInfo::WV_95 && QSysInfo::WindowsVersion != QSysInfo::WV_NT) {
        int shortestDistance = INT_MAX;
        for (int i = 0; i < d->screenCount; ++i) {
            int thisDistance = d->pointToRect(point, d->rects->at(i));
            if (thisDistance < shortestDistance) {
                shortestDistance = thisDistance;
                closestScreen = i;
            }
        }
    }
    return closestScreen;
}

/*!
    \reimp
*/
void QDesktopWidget::resizeEvent(QResizeEvent *)
{
    Q_D(QDesktopWidget);
    QVector<QRect> oldrects;
    oldrects = *d->rects;
    QVector<QRect> oldworkrects;
    oldworkrects = *d->workrects;
    int oldscreencount = d->screenCount;

    QDesktopWidgetPrivate::cleanup();
    QDesktopWidgetPrivate::init(this);
#ifdef Q_OS_WINCE_WM
    for(int i=0; i < d->workrects->size(); ++i)
        qt_get_sip_info((*d->workrects)[i]);
#endif

    for (int i = 0; i < qMin(oldscreencount, d->screenCount); ++i) {
        QRect oldrect = oldrects[i];
        QRect newrect = d->rects->at(i);
        if (oldrect != newrect)
            emit resized(i);
    }

    for (int j = 0; j < qMin(oldscreencount, d->screenCount); ++j) {
        QRect oldrect = oldworkrects[j];
        QRect newrect = d->workrects->at(j);
        if (oldrect != newrect)
            emit workAreaResized(j);
    }
}

#ifdef Q_CC_MSVC
# pragma warning(pop)
#endif

/*! \fn void QDesktopWidget::resized(int screen)
    This signal is emitted when the size of \a screen changes.
*/

/*! \fn void QDesktopWidget::workAreaResized(int screen)
    This signal is emitted when the work area available on \a screen changes.
*/

QT_END_NAMESPACE
