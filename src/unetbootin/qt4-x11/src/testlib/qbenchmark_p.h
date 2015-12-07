/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the QtTest module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Commercial License Agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Nokia.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
** package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QBENCHMARK_P_H
#define QBENCHMARK_P_H

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

#include <QtCore/qglobal.h>

#if defined(Q_OS_LINUX) && !defined(QT_NO_PROCESS)
#define QTESTLIB_USE_VALGRIND 
#else
#undef QTESTLIB_USE_VALGRIND
#endif

#include "QtTest/private/qbenchmarkmeasurement_p.h"
#include <QtCore/QMap>
#include <QtTest/qtest_global.h>
#ifdef QTESTLIB_USE_VALGRIND
#include "QtTest/private/qbenchmarkvalgrind_p.h"
#endif
#include "QtTest/private/qbenchmarkevent_p.h"

QT_BEGIN_NAMESPACE

struct QBenchmarkContext
{
    // None of the strings below are assumed to contain commas (see toString() below)
    QString slotName;
    QString tag; // from _data() function

    int checkpointIndex;

    QString toString() const
    {
        QString s = QString(QLatin1String("%1,%2,%3")).arg(slotName).arg(tag).arg(checkpointIndex);
        return s;
    }

    QBenchmarkContext() : checkpointIndex(-1) {}
};

class QBenchmarkResult
{
public:
    QBenchmarkContext context;
    qint64 value;
    int iterations;
    bool valid;

    QBenchmarkResult()
    : value(-1)
    , iterations(-1)
    , valid(false)
    {  }

    QBenchmarkResult(const QBenchmarkContext &context, const qint64 value, const int iterations)
        : context(context)
        , value(value)
        , iterations(iterations)
        , valid(true)
    {
    }

    bool operator<(const QBenchmarkResult &other) const 
    {
        return (value / iterations) < (other.value / other.iterations);
    }
};

/*
    The QBenchmarkGlobalData class stores global benchmark-related data.
    QBenchmarkGlobalData:current is created at the beginning of qExec()
    and cleared at the end.
*/
class QBenchmarkGlobalData
{
public:
    static QBenchmarkGlobalData *current;

    QBenchmarkGlobalData();
    ~QBenchmarkGlobalData();
    enum Mode { WallTime, CallgrindParentProcess, CallgrindChildProcess, TickCounter, EventCounter };
    void setMode(Mode mode);
    Mode mode() const { return mode_; }
    QBenchmarkMeasurerBase *createMeasurer();
    int adjustMedianIterationCount();

    QBenchmarkMeasurerBase *measurer;
    QBenchmarkContext context;
    int walltimeMinimum;
    int iterationCount;
    int medianIterationCount;
    bool createChart;
    bool verboseOutput;
    QString callgrindOutFileBase;
private:
    Mode mode_;
};

/*
    The QBenchmarkTestMethodData class stores all benchmark-related data
    for the current test case. QBenchmarkTestMethodData:current is
    created at the beginning of qInvokeTestMethod() and cleared at
    the end.
*/
class QBenchmarkTestMethodData
{
public:
    static QBenchmarkTestMethodData *current;
    QBenchmarkTestMethodData();
    ~QBenchmarkTestMethodData();

    // Called once for each data row created by the _data function,
    // before and after calling the test function itself.
    void beginDataRun();
    void endDataRun();

    bool isBenchmark() const { return result.valid; }
    bool resultsAccepted() const { return resultAccepted; }
    int adjustIterationCount(int suggestion);
    void setResult(qint64 value);

    QBenchmarkResult result;
    bool resultAccepted;
    int iterationCount;
};

// low-level API:
namespace QTest
{
    int iterationCount();
    void setIterationCountHint(int count);
    void setIterationCount(int count);

    Q_TESTLIB_EXPORT void beginBenchmarkMeasurement();
    Q_TESTLIB_EXPORT qint64 endBenchmarkMeasurement();

    void setResult(qint64 result);
    void setResult(const QString &tag, qint64 result);
}

QT_END_NAMESPACE

#endif // QBENCHMARK_H
