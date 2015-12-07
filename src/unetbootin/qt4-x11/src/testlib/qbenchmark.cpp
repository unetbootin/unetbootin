
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

#include "QtTest/qbenchmark.h"
#include "QtTest/private/qbenchmark_p.h"

#ifdef QT_GUI_LIB
#include <QtGui/qapplication.h>
#endif

#include <QtCore/qprocess.h>
#include <QtCore/qdir.h>
#include <QtCore/qset.h>
#include <QtCore/qdebug.h>

QT_BEGIN_NAMESPACE

QBenchmarkGlobalData *QBenchmarkGlobalData::current;

QBenchmarkGlobalData::QBenchmarkGlobalData()
    : measurer(0)
    , walltimeMinimum(-1)
    , iterationCount(-1)
    , medianIterationCount(-1)
    , createChart(false)
    , verboseOutput(false)
    , mode_(WallTime)
{
    setMode(mode_);
}

QBenchmarkGlobalData::~QBenchmarkGlobalData()
{
    delete measurer;
    QBenchmarkGlobalData::current = 0;
}

void QBenchmarkGlobalData::setMode(Mode mode)
{
    mode_ = mode;

    if (measurer)
        delete measurer;
    measurer = createMeasurer();
}

QBenchmarkMeasurerBase * QBenchmarkGlobalData::createMeasurer()
{
    QBenchmarkMeasurerBase *measurer = 0;
    if (0) {
#ifdef QTESTLIB_USE_VALGRIND
    } else if (mode_ == CallgrindChildProcess || mode_ == CallgrindParentProcess) {
        measurer = new QBenchmarkCallgrindMeasurer;
#endif
#ifdef HAVE_TICK_COUNTER
    } else if (mode_ == TickCounter) {
        measurer = new QBenchmarkTickMeasurer;
#endif
    } else if (mode_ == EventCounter) {
        measurer = new QBenchmarkEvent;
    } else {
        measurer =  new QBenchmarkTimeMeasurer;
    }
    measurer->init();
    return measurer;
}

int QBenchmarkGlobalData::adjustMedianIterationCount()
{
    if (medianIterationCount != -1) {
        return medianIterationCount;
    } else {
        return measurer->adjustMedianCount(1);
    }
}


QBenchmarkTestMethodData *QBenchmarkTestMethodData::current;

QBenchmarkTestMethodData::QBenchmarkTestMethodData()
:resultAccepted(false), iterationCount(-1)
{
   
}

QBenchmarkTestMethodData::~QBenchmarkTestMethodData()
{
    QBenchmarkTestMethodData::current = 0;
}

void QBenchmarkTestMethodData::beginDataRun()
{
    iterationCount = adjustIterationCount(1);
}

void QBenchmarkTestMethodData::endDataRun()
{

}

int QBenchmarkTestMethodData::adjustIterationCount(int suggestion)
{
    // Let the -iteration-count option override the measurer.
    if (QBenchmarkGlobalData::current->iterationCount != -1) {
        iterationCount = QBenchmarkGlobalData::current->iterationCount;
    } else {
        iterationCount = QBenchmarkGlobalData::current->measurer->adjustIterationCount(suggestion);
    }

    return iterationCount;
}

void QBenchmarkTestMethodData::setResult(qint64 value)
{
    bool accepted = false;

    // Always accept the result if the iteration count has been
    // specified on the command line with -iteartion-count.
    if (QBenchmarkGlobalData::current->iterationCount != -1)
        accepted = true;

    // Test the result directly without calling the measurer if the minimum time 
    // has been specifed on the command line with -minimumvalue.
    else if (QBenchmarkGlobalData::current->walltimeMinimum != -1)
        accepted = (value > QBenchmarkGlobalData::current->walltimeMinimum);
    else
        accepted = QBenchmarkGlobalData::current->measurer->isMeasurementAccepted(value);

    // Accept the result or double the number of iterations.
    if (accepted)
        resultAccepted = true;
    else
        iterationCount *= 2;

    this->result = 
        QBenchmarkResult(QBenchmarkGlobalData::current->context, value, iterationCount);
}

/*! \internal
    The QBenchmarkIterationController class is used by the QBENCHMARK macro to
    drive the benchmarking loop. It is repsonsible for starting and stopping
    the timing measurements as well as calling the result reporting functions.
*/
QTest::QBenchmarkIterationController::QBenchmarkIterationController()
{
    QTest::beginBenchmarkMeasurement();
    i = 0;
}
/*! \internal
*/
QTest::QBenchmarkIterationController::~QBenchmarkIterationController()
{
    QBenchmarkTestMethodData::current->setResult(QTest::endBenchmarkMeasurement());
}

/*! \internal
*/
bool QTest::QBenchmarkIterationController::isDone()
{
    return i >= QTest::iterationCount();
}

/*! \internal
*/
void QTest::QBenchmarkIterationController::next()
{
    ++i;
}

/*! \internal
*/
int QTest::iterationCount()
{
    return QBenchmarkTestMethodData::current->iterationCount;
}

/*! \internal
*/
void QTest::setIterationCountHint(int count)
{
    QBenchmarkTestMethodData::current->adjustIterationCount(count);
}
/*! \internal
*/
void QTest::setIterationCount(int count)
{
    QBenchmarkTestMethodData::current->iterationCount = count;
    QBenchmarkTestMethodData::current->resultAccepted = true;
}

/*! \internal
*/
void QTest::beginBenchmarkMeasurement()
{
    QBenchmarkGlobalData::current->measurer->start();
    // the clock is ticking after the line above, don't add code here.
}

/*! \internal
*/
qint64 QTest::endBenchmarkMeasurement()
{
    // the clock is ticking before the line below, don't add code here.
    return QBenchmarkGlobalData::current->measurer->stop();    
}

/*! \internal
*/
void QTest::setResult(qint64 result)
{
    QBenchmarkTestMethodData::current->setResult(result);
}

/*! \internal
*/
void QTest::setResult(const QString &tag, qint64 result)
{
    QBenchmarkContext context = QBenchmarkGlobalData::current->context;
    context.tag = tag;
    QBenchmarkTestMethodData::current->result = 
        QBenchmarkResult( context, result,
            QBenchmarkTestMethodData::current->iterationCount);
}

template <typename T>
Q_TYPENAME T::value_type qAverage(const T &container)
{
    Q_TYPENAME T::const_iterator it = container.constBegin();
    Q_TYPENAME T::const_iterator end = container.constEnd();
    Q_TYPENAME T::value_type acc = Q_TYPENAME T::value_type();
    int count = 0;
    while (it != end) {
        acc += *it;
        ++it;
        ++count;
    }
    return acc / count;
}


QT_END_NAMESPACE

