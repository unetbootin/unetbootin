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

#include <QtCore/qglobal.h>
#include <QtCore/qhash.h>

QT_BEGIN_NAMESPACE

QT_USE_NAMESPACE

#define UNLOCKED    {-1,-1,-1,-1}
#define UNLOCKED2      UNLOCKED,UNLOCKED
#define UNLOCKED4     UNLOCKED2,UNLOCKED2
#define UNLOCKED8     UNLOCKED4,UNLOCKED4
#define UNLOCKED16    UNLOCKED8,UNLOCKED8
#define UNLOCKED32   UNLOCKED16,UNLOCKED16
#define UNLOCKED64   UNLOCKED32,UNLOCKED32
#define UNLOCKED128  UNLOCKED64,UNLOCKED64
#define UNLOCKED256 UNLOCKED128,UNLOCKED128

// use a 4k page for locks
static int locks[256][4] = { UNLOCKED256 };

int *getLock(volatile void *addr)
{ return locks[qHash(const_cast<void *>(addr)) % 256]; }

static int *align16(int *lock)
{
    ulong off = (((ulong) lock) % 16);
    return off ? (int *)(ulong(lock) + 16 - off) : lock;
}

extern "C" {

    int q_ldcw(volatile int *addr);

    void q_atomic_lock(int *lock)
    {
        // ldcw requires a 16-byte aligned address
        volatile int *x = align16(lock);
        while (q_ldcw(x) == 0)
	    ;
    }

    void q_atomic_unlock(int *lock)
    { lock[0] = lock[1] = lock[2] = lock[3] = -1; }
}


QT_END_NAMESPACE
