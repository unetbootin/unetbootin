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

#include "scriptbreakpointmanager.h"

ScriptBreakpointManager::ScriptBreakpointManager()
{
}

ScriptBreakpointManager::~ScriptBreakpointManager()
{
}

bool ScriptBreakpointManager::hasBreakpoints() const
{
    return !breakpoints.isEmpty();
}

int ScriptBreakpointManager::setBreakpoint(const QString &fileName, int lineNumber)
{
    breakpoints.append(ScriptBreakpointInfo(fileName, lineNumber));
    return breakpoints.size() - 1;
}

int ScriptBreakpointManager::setBreakpoint(const QString &functionName, const QString &fileName)
{
    breakpoints.append(ScriptBreakpointInfo(functionName, fileName));
    return breakpoints.size() - 1;
}

int ScriptBreakpointManager::setBreakpoint(const QScriptValue &function)
{
    breakpoints.append(ScriptBreakpointInfo(function));
    return breakpoints.size() - 1;
}

void ScriptBreakpointManager::removeBreakpoint(int id)
{
    if (id >= 0 && id < breakpoints.size())
        breakpoints[id] = ScriptBreakpointInfo();
}

int ScriptBreakpointManager::findBreakpoint(const QString &fileName, int lineNumber) const
{
    for (int i = 0; i < breakpoints.size(); ++i) {
        const ScriptBreakpointInfo &brk = breakpoints.at(i);
        if (brk.type != ScriptBreakpointInfo::File)
            continue;
        if (brk.fileName == fileName && brk.lineNumber == lineNumber)
            return i;
    }
    return -1;
}

int ScriptBreakpointManager::findBreakpoint(const QString &functionName, const QString &fileName) const
{
    for (int i = 0; i < breakpoints.size(); ++i) {
        const ScriptBreakpointInfo &brk = breakpoints.at(i);
        if (brk.type != ScriptBreakpointInfo::FunctionName)
            continue;
        if (brk.functionName == functionName && brk.fileName == fileName)
            return i;
    }
    return -1;
}

int ScriptBreakpointManager::findBreakpoint(const QScriptValue &function) const
{
    for (int i = 0; i < breakpoints.size(); ++i) {
        const ScriptBreakpointInfo &brk = breakpoints.at(i);
        if (brk.type != ScriptBreakpointInfo::Function)
            continue;
        if (brk.function.strictlyEquals(function))
            return i;
    }
    return -1;
}

bool ScriptBreakpointManager::isBreakpointEnabled(int id) const
{
    return breakpoints.value(id).enabled;
}

void ScriptBreakpointManager::setBreakpointEnabled(int id, bool enabled)
{
    if (id >= 0 && id < breakpoints.size())
        breakpoints[id].enabled = enabled;
}

QString ScriptBreakpointManager::breakpointCondition(int id) const
{
    return breakpoints.value(id).condition;
}

void ScriptBreakpointManager::setBreakpointCondition(int id, const QString &expression)
{
    if (id >= 0 && id < breakpoints.size())
        breakpoints[id].condition = expression;
}

int ScriptBreakpointManager::breakpointIgnoreCount(int id) const
{
    return breakpoints.value(id).ignoreCount;
}

void ScriptBreakpointManager::setBreakpointIgnoreCount(int id, int ignoreCount)
{
    if (id >= 0 && id < breakpoints.size())
        breakpoints[id].ignoreCount = ignoreCount;
}

bool ScriptBreakpointManager::isBreakpointSingleShot(int id) const
{
    return breakpoints.value(id).singleShot;
}

void ScriptBreakpointManager::setBreakpointSingleShot(int id, bool singleShot)
{
    if (id >= 0 && id < breakpoints.size())
        breakpoints[id].singleShot = singleShot;
}
