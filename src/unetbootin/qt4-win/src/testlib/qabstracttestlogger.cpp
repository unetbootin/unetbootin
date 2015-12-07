/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the QtTest module of the Qt Toolkit.
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

#include "QtTest/private/qabstracttestlogger_p.h"
#include "QtTest/private/qtestlog_p.h"
#include "QtTest/qtestassert.h"

#include <stdio.h>
#include <stdlib.h>

#ifndef Q_OS_WIN
#include <unistd.h>
#endif

QT_BEGIN_NAMESPACE

namespace QTest
{
    static FILE *stream = 0;
}

void QAbstractTestLogger::outputString(const char *msg)
{
    QTEST_ASSERT(QTest::stream);

    ::fputs(msg, QTest::stream);
    ::fflush(QTest::stream);
}

bool QAbstractTestLogger::isTtyOutput()
{
    QTEST_ASSERT(QTest::stream);

#ifdef Q_OS_WIN
    return true;
#else
    static bool ttyoutput = isatty(fileno(QTest::stream));
    return ttyoutput;
#endif
}

void QAbstractTestLogger::startLogging()
{
    QTEST_ASSERT(!QTest::stream);

    const char *out = QTestLog::outputFileName();
    if (!out) {
        QTest::stream = stdout;
        return;
    }
#if defined(_MSC_VER) && _MSC_VER >= 1400 && !defined(Q_OS_WINCE)
    if (::fopen_s(&QTest::stream, out, "wt")) {
#else
    QTest::stream = ::fopen(out, "wt");
    if (!QTest::stream) {
#endif
        printf("Unable to open file for logging: %s", out);
        ::exit(1);
    }
}

void QAbstractTestLogger::stopLogging()
{
    QTEST_ASSERT(QTest::stream);
    if (QTest::stream != stdout)
        fclose(QTest::stream);
    QTest::stream = 0;
}

QT_END_NAMESPACE
