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

#ifndef QTESTRESULT_P_H
#define QTESTRESULT_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include <QtTest/qtest_global.h>

QT_BEGIN_NAMESPACE

class QTestResultPrivate;
class QTestData;

class QTestResult
{
public:
    enum TestLocation { NoWhere = 0, DataFunc = 1, InitFunc = 2, Func = 3, CleanupFunc = 4 };

    static const char *currentTestObjectName();
    static bool currentTestFailed();
    static bool allDataPassed();
    static QTestData *currentTestData();
    static QTestData *currentGlobalTestData();
    static const char *currentTestFunction();
    static TestLocation currentTestLocation();
    static const char *currentDataTag();
    static const char *currentGlobalDataTag();
    static void finishedCurrentTestFunction();
    static void reset();

    static int passCount();
    static int failCount();
    static int skipCount();

    static void ignoreMessage(QtMsgType type, const char *msg);

    static void addFailure(const char *message, const char *file, int line);
    static bool compare(bool success, const char *msg, const char *file, int line);
    static bool compare(bool success, const char *msg, char *val1, char *val2,
                        const char *actual, const char *expected, const char *file, int line);

    static void setCurrentGlobalTestData(QTestData *data);
    static void setCurrentTestData(QTestData *data);
    static void setCurrentTestFunction(const char *func);
    static void setCurrentTestLocation(TestLocation loc);
    static void setCurrentTestObject(const char *name);
    static void addSkip(const char *message, QTest::SkipMode mode,
                        const char *file, int line);
    static bool expectFail(const char *dataIndex, const char *comment,
                           QTest::TestFailMode mode, const char *file, int line);
    static bool verify(bool statement, const char *statementStr, const char *extraInfo,
                       const char *file, int line);
    static bool testFailed();
    static void setSkipCurrentTest(bool value);
    static bool skipCurrentTest();

private:
    Q_DISABLE_COPY(QTestResult)
};

QT_END_NAMESPACE

#endif
