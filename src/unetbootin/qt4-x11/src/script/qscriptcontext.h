/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the QtScript module of the Qt Toolkit.
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

#ifndef QSCRIPTCONTEXT_H
#define QSCRIPTCONTEXT_H

#include <QtCore/qobjectdefs.h>

#ifndef QT_NO_SCRIPT

#include <QtScript/qscriptvalue.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Script)

class QScriptContextPrivate;

class Q_SCRIPT_EXPORT QScriptContext
{
public:
    enum ExecutionState {
        NormalState,
        ExceptionState
    };

    enum Error {
        UnknownError,
        ReferenceError,
        SyntaxError,
        TypeError,
        RangeError,
        URIError
    };

    ~QScriptContext();

    QScriptContext *parentContext() const;
    QScriptEngine *engine() const;

    ExecutionState state() const;
    QScriptValue callee() const;

    int argumentCount() const;
    QScriptValue argument(int index) const;
    QScriptValue argumentsObject() const;

    QScriptValueList scopeChain() const;
    void pushScope(const QScriptValue &object);
    QScriptValue popScope();

    QScriptValue returnValue() const;
    void setReturnValue(const QScriptValue &result);

    QScriptValue activationObject() const;
    void setActivationObject(const QScriptValue &activation);

    QScriptValue thisObject() const;
    void setThisObject(const QScriptValue &thisObject);

    bool isCalledAsConstructor() const;

    QScriptValue throwValue(const QScriptValue &value);
    QScriptValue throwError(Error error, const QString &text);
    QScriptValue throwError(const QString &text);

    QStringList backtrace() const;

    QString toString() const;

private:
    QScriptContext();

    QScriptContextPrivate *d_ptr;

    Q_DECLARE_PRIVATE(QScriptContext)
    Q_DISABLE_COPY(QScriptContext)
};

QT_END_NAMESPACE

QT_END_HEADER

#endif // QT_NO_SCRIPT
#endif
