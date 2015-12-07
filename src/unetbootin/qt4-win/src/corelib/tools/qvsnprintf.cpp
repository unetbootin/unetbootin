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

#include "qplatformdefs.h"

#include "qbytearray.h"
#include "qstring.h"

#include "string.h"

QT_BEGIN_NAMESPACE

#ifndef QT_VSNPRINTF

/*!
    \relates QByteArray

    A portable \c vsnprintf() function. Will call \c ::vsnprintf(), \c
    ::_vsnprintf(), or \c ::vsnprintf_s depending on the system, or
    fall back to an internal version.

    \a fmt is the \c printf() format string. The result is put into
    \a str, which is a buffer of at least \a n bytes.

    The caller is responsible to call \c va_end() on \a ap.

    \warning Since vsnprintf() shows different behavior on certain
    platforms, you should not rely on the return value or on the fact
    that you will always get a 0 terminated string back.

    Ideally, you should never call this function but use QString::sprintf()
    instead.

    \sa qsnprintf(), QString::sprintf()
*/

int qvsnprintf(char *str, size_t n, const char *fmt, va_list ap)
{
    if (!str || !fmt)
        return -1;

    QString buf;
    buf.vsprintf(fmt, ap);

    QByteArray ba = buf.toLocal8Bit();

    if (n > 0) {
        size_t blen = qMin(size_t(ba.length()), size_t(n - 1));
        memcpy(str, ba.constData(), blen);
        str[blen] = '\0'; // make sure str is always 0 terminated
    }

    return ba.length();
}

#else

QT_BEGIN_INCLUDE_NAMESPACE
#include <stdio.h>
QT_END_INCLUDE_NAMESPACE

int qvsnprintf(char *str, size_t n, const char *fmt, va_list ap)
{
    return QT_VSNPRINTF(str, n, fmt, ap);
}

#endif

/*!
    \relates QByteArray

    A portable snprintf() function, calls qvsnprintf.

    \a fmt is the \c printf() format string. The result is put into
    \a str, which is a buffer of at least \a n bytes.

    \warning Call this function only when you know what you are doing
    since it shows different behavior on certain platforms.
    Use QString::sprintf() to format a string instead.

    \sa qvsnprintf(), QString::sprintf()
*/

int qsnprintf(char *str, size_t n, const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);

    int ret = qvsnprintf(str, n, fmt, ap);
    va_end(ap);

    return ret;
}

QT_END_NAMESPACE
