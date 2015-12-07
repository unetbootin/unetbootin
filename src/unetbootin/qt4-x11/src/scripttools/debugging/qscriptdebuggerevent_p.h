/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the QtScriptTools module of the Qt Toolkit.
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

#ifndef QSCRIPTDEBUGGEREVENT_P_H
#define QSCRIPTDEBUGGEREVENT_P_H

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

#include <QtCore/qobjectdefs.h>
#include <QtCore/qcoreevent.h>
#include <QtCore/qhash.h>
#include <QtCore/qvariant.h>

QT_BEGIN_NAMESPACE

class QDataStream;
class QScriptDebuggerValue;

class QScriptDebuggerEventPrivate;
class Q_AUTOTEST_EXPORT QScriptDebuggerEvent
{
public:
    friend Q_AUTOTEST_EXPORT QDataStream &operator<<(QDataStream &, const QScriptDebuggerEvent &);
    friend Q_AUTOTEST_EXPORT QDataStream &operator>>(QDataStream &, QScriptDebuggerEvent &);

    enum Type {
        None,
        Interrupted,
        SteppingFinished,
        LocationReached,
        Breakpoint,
        Exception,
        Trace,
        InlineEvalFinished,
        DebuggerInvocationRequest,
        ForcedReturn,
        UserEvent = 1000,
        MaxUserEvent = 32767
    };

    enum Attribute {
        ScriptID,
        FileName,
        BreakpointID,
        LineNumber,
        ColumnNumber,
        Value,
        Message,
        IsNestedEvaluate,
        HasExceptionHandler,
        UserAttribute = 1000,
        MaxUserAttribute = 32767
    };

    QScriptDebuggerEvent();
    QScriptDebuggerEvent(Type type);
    QScriptDebuggerEvent(Type type, qint64 scriptId, int lineNumber, int columnNumber);
    QScriptDebuggerEvent(const QScriptDebuggerEvent &other);
    ~QScriptDebuggerEvent();

    Type type() const;

    QVariant attribute(Attribute attribute,
                       const QVariant &defaultValue = QVariant()) const;
    void setAttribute(Attribute attribute, const QVariant &value);
    QHash<Attribute, QVariant> attributes() const;

    qint64 scriptId() const;
    void setScriptId(qint64 id);
    QString fileName() const;
    void setFileName(const QString &fileName);
    int lineNumber() const;
    void setLineNumber(int lineNumber);
    int columnNumber() const;
    void setColumnNumber(int columnNumber);
    int breakpointId() const;
    void setBreakpointId(int id);
    QString message() const;
    void setMessage(const QString &message);
    QScriptDebuggerValue scriptValue() const;
    void setScriptValue(const QScriptDebuggerValue &value);
    void setNestedEvaluate(bool nested);
    bool isNestedEvaluate() const;
    void setHasExceptionHandler(bool hasHandler);
    bool hasExceptionHandler() const;

    QScriptDebuggerEvent &operator=(const QScriptDebuggerEvent &other);

    bool operator==(const QScriptDebuggerEvent &other) const;
    bool operator!=(const QScriptDebuggerEvent &other) const;

private:
    QScriptDebuggerEventPrivate *d_ptr;

    Q_DECLARE_PRIVATE(QScriptDebuggerEvent)
};

Q_AUTOTEST_EXPORT QDataStream &operator<<(QDataStream &, const QScriptDebuggerEvent &);
Q_AUTOTEST_EXPORT QDataStream &operator>>(QDataStream &, QScriptDebuggerEvent &);

// helper class that's used to transport a debugger event through the Qt event loop
class QScriptDebuggerEventEvent : public QEvent
{
public:
    QScriptDebuggerEventEvent(const QScriptDebuggerEvent &event)
        : QEvent(QEvent::Type(QEvent::User+1)), m_event(event) {}
    ~QScriptDebuggerEventEvent() {}
    const QScriptDebuggerEvent &event() const
        { return m_event; }
private:
    QScriptDebuggerEvent m_event;
};

QT_END_NAMESPACE

#endif
