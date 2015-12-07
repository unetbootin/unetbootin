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

#include <stdio.h>
#include <string.h>
#include <QtCore/qglobal.h>

#include "QtTest/private/qxmltestlogger_p.h"
#include "QtTest/private/qtestresult_p.h"
#include "QtTest/private/qbenchmark_p.h"

QT_BEGIN_NAMESPACE

namespace QTest {

    static const char* xmlMessageType2String(QAbstractTestLogger::MessageTypes type)
    {
        switch (type) {
        case QAbstractTestLogger::Warn:
            return "warn";
        case QAbstractTestLogger::QSystem:
            return "system";
        case QAbstractTestLogger::QDebug:
            return "qdebug";
        case QAbstractTestLogger::QWarning:
            return "qwarn";
        case QAbstractTestLogger::QFatal:
            return "qfatal";
        case QAbstractTestLogger::Skip:
            return "skip";
        case QAbstractTestLogger::Info:
            return "info";
        }
        return "??????";
    }

    static const char* xmlIncidentType2String(QAbstractTestLogger::IncidentTypes type)
    {
        switch (type) {
        case QAbstractTestLogger::Pass:
            return "pass";
        case QAbstractTestLogger::XFail:
            return "xfail";
        case QAbstractTestLogger::Fail:
            return "fail";
        case QAbstractTestLogger::XPass:
            return "xpass";
        }
        return "??????";
    }

}


QXmlTestLogger::QXmlTestLogger(XmlMode mode ):
    xmlmode(mode)
{

}

QXmlTestLogger::~QXmlTestLogger()
{

}


void QXmlTestLogger::startLogging()
{
    QAbstractTestLogger::startLogging();
    char buf[1024];

    if (xmlmode == QXmlTestLogger::Complete) {
        QTest::qt_snprintf(buf, sizeof(buf),
                "<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>\n"
                "<TestCase name=\"%s\">\n", QTestResult::currentTestObjectName());
        outputString(buf);
    }

    QTest::qt_snprintf(buf, sizeof(buf),
            "<Environment>\n"
            "    <QtVersion>%s</QtVersion>\n"
            "    <QTestVersion>"QTEST_VERSION_STR"</QTestVersion>\n"
            "</Environment>\n", qVersion());
    outputString(buf);
}

void QXmlTestLogger::stopLogging()
{
    if (xmlmode == QXmlTestLogger::Complete) {
        outputString("</TestCase>\n");
    }

    QAbstractTestLogger::stopLogging();
}

void QXmlTestLogger::enterTestFunction(const char *function)
{
    char buf[1024];
    QTest::qt_snprintf(buf, sizeof(buf), "<TestFunction name=\"%s\">\n", function);
    outputString(buf);
}

void QXmlTestLogger::leaveTestFunction()
{
    outputString("</TestFunction>\n");
}

namespace QTest
{

inline static bool isEmpty(const char *str)
{
    return !str || !str[0];
}

static const char *incidentFormatString(bool noDescription, bool noTag)
{
    if (noDescription) {
        if (noTag)
            return "<Incident type=\"%s\" file=\"%s\" line=\"%d\" />\n";
        else
            return "<Incident type=\"%s\" file=\"%s\" line=\"%d\">\n"
                   "    <DataTag><![CDATA[%s%s%s%s]]></DataTag>\n"
                   "</Incident>\n";
    } else {
        if (noTag)
            return "<Incident type=\"%s\" file=\"%s\" line=\"%d\">\n"
                   "    <Description><![CDATA[%s%s%s%s]]></Description>\n"
                   "</Incident>\n";
        else
            return "<Incident type=\"%s\" file=\"%s\" line=\"%d\">\n"
                   "    <DataTag><![CDATA[%s%s%s]]></DataTag>\n"
                   "    <Description><![CDATA[%s]]></Description>\n"
                   "</Incident>\n";
    }
}

static const char *benchmarkResultFormatString()
{
    return "<BenchmarkResult metric=\"%s\" tag=\"%s\" value=\"%s\" iterations=\"%d\" />\n";
}

static const char *messageFormatString(bool noDescription, bool noTag)
{
    if (noDescription) {
        if (noTag)
            return "<Message type=\"%s\" file=\"%s\" line=\"%d\" />\n";
        else
            return "<Message type=\"%s\" file=\"%s\" line=\"%d\">\n"
                   "    <DataTag><![CDATA[%s%s%s%s]]></DataTag>\n"
                   "</Message>\n";
    } else {
        if (noTag)
            return "<Message type=\"%s\" file=\"%s\" line=\"%d\">\n"
                   "    <Description><![CDATA[%s%s%s%s]]></Description>\n"
                   "</Message>\n";
        else
            return "<Message type=\"%s\" file=\"%s\" line=\"%d\">\n"
                   "    <DataTag><![CDATA[%s%s%s]]></DataTag>\n"
                   "    <Description><![CDATA[%s]]></Description>\n"
                   "</Message>\n";
    }
}

} // namespace

void QXmlTestLogger::addIncident(IncidentTypes type, const char *description,
                                 const char *file, int line)
{
    char buf[1536];
    const char *tag = QTestResult::currentDataTag();
    const char *gtag = QTestResult::currentGlobalDataTag();
    const char *filler = (tag && gtag) ? ":" : "";
    const bool notag = QTest::isEmpty(tag) && QTest::isEmpty(gtag);

    QTest::qt_snprintf(buf, sizeof(buf),
            QTest::incidentFormatString(QTest::isEmpty(description), notag),
            QTest::xmlIncidentType2String(type),
            file ? file : "", line,
            gtag ? gtag : "",
            filler,
            tag ? tag : "",
            description ? description : "");

    outputString(buf);
}

void QXmlTestLogger::addBenchmarkResult(const QBenchmarkResult &result)
{
    char buf[1536];
    QTest::qt_snprintf(
        buf, sizeof(buf),
        QTest::benchmarkResultFormatString(),
        QBenchmarkGlobalData::current->measurer->metricText().toAscii().data(),
        result.context.tag.toAscii().data(),
        QByteArray::number(result.value).constData(),  //no 64-bit qt_snprintf support
        result.iterations); 
    outputString(buf);
}

void QXmlTestLogger::addMessage(MessageTypes type, const char *message,
                                const char *file, int line)
{
    char buf[1536];
    char msgbuf[1024];
    const char *tag = QTestResult::currentDataTag();
    const char *gtag = QTestResult::currentGlobalDataTag();
    const char *filler = (tag && gtag) ? ":" : "";
    const bool notag = QTest::isEmpty(tag) && QTest::isEmpty(gtag);

    QTest::qt_snprintf(msgbuf, sizeof(msgbuf), "%s",
                        message ? message : "");

    QTest::qt_snprintf(buf, sizeof(buf),
            QTest::messageFormatString(QTest::isEmpty(message), notag),
            QTest::xmlMessageType2String(type),
            file ? file : "", line,
            gtag ? gtag : "",
            filler,
            tag ? tag : "",
            msgbuf);

    outputString(buf);
}

QT_END_NAMESPACE
