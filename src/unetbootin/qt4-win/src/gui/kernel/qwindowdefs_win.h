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

#ifndef QWINDOWDEFS_WIN_H
#define QWINDOWDEFS_WIN_H

#include <QtCore/qglobal.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Gui)

QT_END_NAMESPACE

#if !defined(Q_NOWINSTRICT)
#define Q_WINSTRICT
#endif

#if defined(Q_WINSTRICT)

#if !defined(STRICT)
#define STRICT
#endif
#undef NO_STRICT
#define Q_DECLARE_HANDLE(name) struct name##__; typedef struct name##__ *name

#else

#if !defined(NO_STRICT)
#define NO_STRICT
#endif
#undef  STRICT
#define Q_DECLARE_HANDLE(name) typedef HANDLE name

#endif

#ifndef HINSTANCE
Q_DECLARE_HANDLE(HINSTANCE);
#endif
#ifndef HDC
Q_DECLARE_HANDLE(HDC);
#endif
#ifndef HWND
Q_DECLARE_HANDLE(HWND);
#endif
#ifndef HFONT
Q_DECLARE_HANDLE(HFONT);
#endif
#ifndef HPEN
Q_DECLARE_HANDLE(HPEN);
#endif
#ifndef HBRUSH
Q_DECLARE_HANDLE(HBRUSH);
#endif
#ifndef HBITMAP
Q_DECLARE_HANDLE(HBITMAP);
#endif
#ifndef HICON
Q_DECLARE_HANDLE(HICON);
#endif
#ifndef HCURSOR
typedef HICON HCURSOR;
#endif
#ifndef HPALETTE
Q_DECLARE_HANDLE(HPALETTE);
#endif
#ifndef HRGN
Q_DECLARE_HANDLE(HRGN);
#endif
#ifndef HMONITOR
Q_DECLARE_HANDLE(HMONITOR);
#endif
#ifndef HRESULT
typedef long HRESULT;
#endif

typedef struct tagMSG MSG;
typedef HWND WId;


QT_BEGIN_NAMESPACE

Q_CORE_EXPORT HINSTANCE qWinAppInst();
Q_CORE_EXPORT HINSTANCE qWinAppPrevInst();
Q_GUI_EXPORT int           qWinAppCmdShow();
Q_GUI_EXPORT HDC           qt_win_display_dc();

QT_END_NAMESPACE

QT_END_HEADER

#endif // QWINDOWDEFS_WIN_H
