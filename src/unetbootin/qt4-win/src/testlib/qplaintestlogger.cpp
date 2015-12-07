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

#include "QtTest/private/qtestresult_p.h"
#include "QtTest/qtestassert.h"
#include "QtTest/private/qtestlog_p.h"

#include "QtTest/private/qplaintestlogger_p.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef Q_OS_WIN
#include "windows.h"
#endif

#ifdef Q_OS_WINCE
#include <QtCore/QString>
#endif

#include <QtCore/QByteArray>

QT_BEGIN_NAMESPACE

namespace QTest {

#if defined(Q_OS_WIN) && !defined(Q_OS_WINCE)

    static CRITICAL_SECTION outputCriticalSection;
    static HANDLE hConsole = INVALID_HANDLE_VALUE;
    static WORD consoleAttributes = 0;

    static const char *qWinColoredMsg(int prefix, int color, const char *msg)
    {
        if (!hConsole)
            return msg;

        WORD attr = consoleAttributes & ~(FOREGROUND_GREEN | FOREGROUND_BLUE
                  | FOREGROUND_RED | FOREGROUND_INTENSITY);
        if (prefix)
            attr |= FOREGROUND_INTENSITY;
        if (color == 32)
            attr |= FOREGROUND_GREEN;
        if (color == 31)
            attr |= FOREGROUND_RED | FOREGROUND_INTENSITY;
        if (color == 37)
            attr |= FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;
        if (color == 33)
            attr |= FOREGROUND_RED | FOREGROUND_GREEN;
        SetConsoleTextAttribute(hConsole, attr);
        printf(msg);
        SetConsoleTextAttribute(hConsole, consoleAttributes);
        return "";
    }

# define COLORED_MSG(prefix, color, msg) colored ? qWinColoredMsg(prefix, color, msg) : msg
#else
# define COLORED_MSG(prefix, color, msg) colored && QAbstractTestLogger::isTtyOutput() ? "\033["#prefix";"#color"m" msg "\033[0m" : msg
#endif

    static const char *incidentType2String(QAbstractTestLogger::IncidentTypes type)
    {
        static bool colored = (!qgetenv("QTEST_COLORED").isEmpty());
        switch (type) {
        case QAbstractTestLogger::Pass:
            return COLORED_MSG(0, 32, "PASS   "); //green
        case QAbstractTestLogger::XFail:
            return COLORED_MSG(1, 32, "XFAIL  "); //light green
        case QAbstractTestLogger::Fail:
            return COLORED_MSG(0, 31, "FAIL!  "); //red
        case QAbstractTestLogger::XPass:
            return COLORED_MSG(0, 31, "XPASS  "); //red, too
        }
        return "??????";
    }



    static const char *messageType2String(QAbstractTestLogger::MessageTypes type)
    {
        static bool colored = (!qgetenv("QTEST_COLORED").isEmpty());
        switch (type) {
        case QAbstractTestLogger::Skip:
            return COLORED_MSG(0, 37, "SKIP   "); //white
        case QAbstractTestLogger::Warn:
            return COLORED_MSG(0, 33, "WARNING"); // yellow
        case QAbstractTestLogger::QWarning:
            return COLORED_MSG(1, 33, "QWARN  ");
        case QAbstractTestLogger::QDebug:
            return COLORED_MSG(1, 33, "QDEBUG ");
        case QAbstractTestLogger::QSystem:
            return COLORED_MSG(1, 33, "QSYSTEM");
        case QAbstractTestLogger::QFatal:
            return COLORED_MSG(0, 31, "QFATAL "); // red
        case QAbstractTestLogger::Info:
            return "INFO   "; // no coloring
        }
        return "??????";
    }

    static void outputMessage(const char *str)
    {
#if defined(Q_OS_WINCE)
        int length = strlen(str);
        for (int pos = 0; pos < length; pos +=255) {
            QString uniText = QString::fromLatin1(str + pos, 255);
            OutputDebugStringW(uniText.utf16());
        }
        if (QTestLog::outputFileName())
#elif defined(Q_OS_WIN)
        EnterCriticalSection(&outputCriticalSection);
        // OutputDebugString is not threadsafe
        OutputDebugStringA(str);
        LeaveCriticalSection(&outputCriticalSection);
#endif
        QAbstractTestLogger::outputString(str);
    }

    static void printMessage(const char *type, const char *msg, const char *file = 0, int line = 0)
    {
        QTEST_ASSERT(type);
        QTEST_ASSERT(msg);

        char buf[1024];

        const char *fn = QTestResult::currentTestFunction() ? QTestResult::currentTestFunction()
            : "UnknownTestFunc";
        const char *tag = QTestResult::currentDataTag() ? QTestResult::currentDataTag() : "";
        const char *gtag = QTestResult::currentGlobalDataTag()
                         ? QTestResult::currentGlobalDataTag()
                         : "";
        const char *filler = (tag[0] && gtag[0]) ? ":" : "";
        if (file) {
            QTest::qt_snprintf(buf, sizeof(buf), "%s: %s::%s(%s%s%s)%s%s\n"
#ifdef Q_OS_WIN
                          "%s(%d) : failure location\n"
#else
                          "   Loc: [%s(%d)]\n"
#endif
                          , type, QTestResult::currentTestObjectName(), fn, gtag, filler, tag,
                          msg[0] ? " " : "", msg, file, line);
        } else {
            QTest::qt_snprintf(buf, sizeof(buf), "%s: %s::%s(%s%s%s)%s%s\n",
                    type, QTestResult::currentTestObjectName(), fn, gtag, filler, tag,
                    msg[0] ? " " : "", msg);
        }
        memcpy(buf, type, strlen(type));
        outputMessage(buf);
    }
}

QPlainTestLogger::QPlainTestLogger()
{
#if defined(Q_OS_WIN) && !defined(Q_OS_WINCE)
    InitializeCriticalSection(&QTest::outputCriticalSection);
    QTest::hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    if (QTest::hConsole != INVALID_HANDLE_VALUE) {
        CONSOLE_SCREEN_BUFFER_INFO info;
        if (GetConsoleScreenBufferInfo(QTest::hConsole, &info)) {
            QTest::consoleAttributes = info.wAttributes;
        } else {
            QTest::hConsole = INVALID_HANDLE_VALUE;
        }
    }
#endif
}

QPlainTestLogger::~QPlainTestLogger()
{
#if defined(Q_OS_WIN) && !defined(Q_OS_WINCE)
	DeleteCriticalSection(&QTest::outputCriticalSection);
#endif
}

void QPlainTestLogger::startLogging()
{
    QAbstractTestLogger::startLogging();

    char buf[1024];
    if (QTestLog::verboseLevel() < 0) {
        QTest::qt_snprintf(buf, sizeof(buf), "Testing %s\n",
                           QTestResult::currentTestObjectName());
    } else {
        QTest::qt_snprintf(buf, sizeof(buf),
                         "********* Start testing of %s *********\n"
                         "Config: Using QTest library " QTEST_VERSION_STR
                         ", Qt %s\n", QTestResult::currentTestObjectName(), qVersion());
    }
    QTest::outputMessage(buf);
}

void QPlainTestLogger::stopLogging()
{
    char buf[1024];
    if (QTestLog::verboseLevel() < 0) {
        QTest::qt_snprintf(buf, sizeof(buf), "Totals: %d passed, %d failed, %d skipped\n",
                           QTestResult::passCount(), QTestResult::failCount(),
                           QTestResult::skipCount());
    } else {
        QTest::qt_snprintf(buf, sizeof(buf),
                         "Totals: %d passed, %d failed, %d skipped\n"
                         "********* Finished testing of %s *********\n",
                         QTestResult::passCount(), QTestResult::failCount(),
                         QTestResult::skipCount(), QTestResult::currentTestObjectName());
    }
    QTest::outputMessage(buf);

    QAbstractTestLogger::stopLogging();
}


void QPlainTestLogger::enterTestFunction(const char * /*function*/)
{
    if (QTestLog::verboseLevel() >= 1)
        QTest::printMessage(QTest::messageType2String(Info), "entering");
}

void QPlainTestLogger::leaveTestFunction()
{
}

void QPlainTestLogger::addIncident(IncidentTypes type, const char *description,
                                   const char *file, int line)
{
    // suppress PASS in silent mode
    if (type == QAbstractTestLogger::Pass && QTestLog::verboseLevel() < 0)
        return;

    QTest::printMessage(QTest::incidentType2String(type), description, file, line);
}

void QPlainTestLogger::addMessage(MessageTypes type, const char *message,
                                  const char *file, int line)
{
    // suppress PASS in silent mode
    if ((type == QAbstractTestLogger::Skip || type == QAbstractTestLogger::Info)
       && QTestLog::verboseLevel() < 0)
        return;

    QTest::printMessage(QTest::messageType2String(type), message, file, line);
}

QT_END_NAMESPACE
