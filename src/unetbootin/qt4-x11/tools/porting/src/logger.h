/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the qt3to4 porting application of the Qt Toolkit.
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

#ifndef LOGGER_H
#define LOGGER_H

#include <QString>
#include <QStringList>
#include <QList>
#include <QMap>

QT_BEGIN_NAMESPACE
/*
    Base class for logger entries;
    description() should return a text for this entry,
*/
class LogEntry
{
public:
    LogEntry(QString type, QString location);
    virtual ~LogEntry(){};
    virtual QString description() const =0;
    virtual void updateLinePos(int threshold, int delta) {Q_UNUSED(threshold); Q_UNUSED(delta)};
protected:
    QString type; // Error, Warning, Info, etc
    QString location;// preprocessor, c++parser, porting, etc
};

class PlainLogEntry: public LogEntry
{
public:
     PlainLogEntry(QString type, QString lcation, QString text);
     QString description() const {return text;};
protected:
     QString text;
};

/*
    A log entry that stores a source point: file, line and column.
*/
class SourcePointLogEntry : public LogEntry
{
public:
    SourcePointLogEntry(QString type, QString location, QString file, int line, int column, QString text);
    QString description() const;
    void updateLinePos(int threshold, int delta);
protected:
    QString file;
    int line;
    int column;
    QString text;
};


class Logger
{
public:
    Logger(){};
    ~Logger();
    static Logger *instance();
    static void deleteInstance();

    void beginSection();
    void commitSection();
    void revertSection();
    void addEntry(LogEntry *entry);

    QStringList fullReport();
    int numEntries();
    void updateLineNumbers(int insertLine, int numLines);
    /*
        glabalState can be used for storage of application state
        together with the logger. This can be useful in some cases,
        for example the current filename is stored here when processing
        files.
    */
    QMap<QString, QString> globalState;
private:
    static Logger *theInstance;
    QList<LogEntry*> logEntries;
    QList<LogEntry*> pendingLogEntries;
};

QT_END_NAMESPACE

#endif
