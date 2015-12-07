/****************************************************************************
**
** Copyright (C) 2007-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the example classes of the Qt Toolkit.
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

#ifndef SCRIPTBREAKPOINTMANAGER_H
#define SCRIPTBREAKPOINTMANAGER_H

#include <QtCore/qlist.h>
#include <QtCore/qstring.h>
#include <QtScript/qscriptvalue.h>

class ScriptBreakpointInfo
{
public:
    enum Type {
        File,
        FunctionName,
        Function,
        Invalid
    };

    Type type;
    QString functionName;
    QString fileName;
    int lineNumber;
    QScriptValue function;
    bool enabled;
    QString condition;
    int ignoreCount;
    bool singleShot;

    ScriptBreakpointInfo(const QString &fileName, int lineNumber)
        : type(File), fileName(fileName), lineNumber(lineNumber),
          enabled(true), ignoreCount(0), singleShot(false)
        { }
    ScriptBreakpointInfo(const QString &functionName, const QString &fileName = QString())
        : type(FunctionName), functionName(functionName), fileName(fileName),
          enabled(true), ignoreCount(0), singleShot(false)
        { }
    ScriptBreakpointInfo(const QScriptValue &function)
        : type(Function), function(function),
          enabled(true), ignoreCount(0), singleShot(false)
        { }
    ScriptBreakpointInfo()
        : type(Invalid)
        { }
};

class ScriptBreakpointManager
{
public:
    ScriptBreakpointManager();
    ~ScriptBreakpointManager();

    bool hasBreakpoints() const;

    int setBreakpoint(const QString &fileName, int lineNumber);
    int setBreakpoint(const QString &functionName, const QString &fileName = QString());
    int setBreakpoint(const QScriptValue &function);

    void removeBreakpoint(int id);

    int findBreakpoint(const QString &fileName, int lineNumber) const;
    int findBreakpoint(const QString &functionName, const QString &fileName = QString()) const;
    int findBreakpoint(const QScriptValue &function) const;

    bool isBreakpointEnabled(int id) const;
    void setBreakpointEnabled(int id, bool enabled);

    QString breakpointCondition(int id) const;
    void setBreakpointCondition(int id, const QString &expression);

    int breakpointIgnoreCount(int id) const;
    void setBreakpointIgnoreCount(int id, int ignoreCount);

    bool isBreakpointSingleShot(int id) const;
    void setBreakpointSingleShot(int id, bool singleShot);

private:
    QList<ScriptBreakpointInfo> breakpoints;

    Q_DISABLE_COPY(ScriptBreakpointManager)
};

#endif // SCRIPTBREAKPOINTMANAGER_H
