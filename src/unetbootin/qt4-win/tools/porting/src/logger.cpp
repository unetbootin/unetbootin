/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the qt3to4 porting application of the Qt Toolkit.
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

#include "logger.h"
#include <QFile>
#include <QDateTime>

QT_BEGIN_NAMESPACE

LogEntry::LogEntry(QString type, QString location)
:type(type), location(location)
{}

PlainLogEntry::PlainLogEntry(QString type, QString location, QString text)
:LogEntry(type, location), text(text)
{}

SourcePointLogEntry::SourcePointLogEntry(QString type, QString location, QString file, int line, int column, QString text)
:LogEntry(type, location), file(file), line(line), column(column), text(text)
{}

QString SourcePointLogEntry::description() const
{
    return QLatin1String("In file ")  + file +
           QLatin1String(" at line ") + QString(QLatin1String("%1")).arg(line + 1) + //line count is zero based, adjust here.
           QLatin1String(" column ")  + QString(QLatin1String("%1")).arg(column) +
           QLatin1String(": ") + text ;
}

void SourcePointLogEntry::updateLinePos(int threshold,  int delta)
{
    if (line >= threshold)
        line += delta;
}

/////////////////////////////////////////////////////


Logger::~Logger()
{
   qDeleteAll(logEntries);
}

Logger *Logger::theInstance  = 0;
Logger *Logger::instance()
{
    if(!theInstance)
        theInstance = new Logger();
        return theInstance;
}

void Logger::deleteInstance()
{
    if(theInstance)
        delete theInstance;
}

void Logger::addEntry(LogEntry *entry)
{
   Q_ASSERT(entry);
   pendingLogEntries.append(entry);
}

void Logger::beginSection()
{
    commitSection();
}

void Logger::commitSection()
{
    logEntries += pendingLogEntries;
    pendingLogEntries.clear();
}

void Logger::revertSection()
{
    qDeleteAll(pendingLogEntries);
    pendingLogEntries.clear();
}

int Logger::numEntries()
{
    commitSection();
    return logEntries.size();
}

QStringList Logger::fullReport()
{
    commitSection();
    QStringList report;
    report << QLatin1String("Log for qt3to4 on ") + QDateTime::currentDateTime().toString() +
              QLatin1String(". Number of log entries: ") + QString(QLatin1String("%1")).arg(logEntries.size());
    foreach(LogEntry *logEntry, logEntries) {
        report << logEntry->description();
    }
    return report;
}

/*
    Update the line for all SourcePointLogEntrys in the list of pending log
    entries located on or after insertLine.
*/
void Logger::updateLineNumbers(int insertLine, int numLines)
{
    foreach(LogEntry *logEntry, pendingLogEntries) {
        logEntry->updateLinePos(insertLine, numLines);
    }
}

QT_END_NAMESPACE
