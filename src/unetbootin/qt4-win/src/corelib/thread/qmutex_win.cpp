/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the QtCore module of the Qt Toolkit.
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

#include <windows.h>

#include "qmutex.h"
#include <qatomic.h>
#include "qmutex_p.h"

QT_BEGIN_NAMESPACE

QMutexPrivate::QMutexPrivate(QMutex::RecursionMode mode)
    : recursive(mode == QMutex::Recursive), contenders(0), owner(0), count(0)
{
    if (QSysInfo::WindowsVersion == 0) {
        // mutex was created before initializing WindowsVersion. this
        // can happen when creating the resource file engine handler,
        // for example. try again with just the A version
#ifndef Q_OS_WINCE
        event = CreateEventA(0, FALSE, FALSE, 0);
#endif
    } else {
        event = QT_WA_INLINE(CreateEventW(0, FALSE, FALSE, 0),
                             CreateEventA(0, FALSE, FALSE, 0));
    }
    if (!event)
        qWarning("QMutexPrivate::QMutexPrivate: Cannot create event");
}

QMutexPrivate::~QMutexPrivate()
{ CloseHandle(event); }

ulong QMutexPrivate::self()
{ return GetCurrentThreadId(); }

bool QMutexPrivate::wait(int timeout)
{
    return WaitForSingleObject(event, timeout < 0 ? INFINITE : timeout) ==  WAIT_OBJECT_0;
}

void QMutexPrivate::wakeUp()
{ SetEvent(event); }

QT_END_NAMESPACE
