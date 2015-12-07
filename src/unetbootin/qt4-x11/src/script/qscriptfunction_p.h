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

#ifndef QSCRIPTFUNCTION_P_H
#define QSCRIPTFUNCTION_P_H

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

#include "qscriptobjectdata_p.h"

#ifndef QT_NO_SCRIPT

#include "qscriptglobals_p.h"
#include "qscriptnodepool_p.h"

#include <QtCore/QList>

#ifndef QT_NO_QOBJECT
# include <QtCore/QPointer>
# include <QtCore/QMetaMethod>
#endif

QT_BEGIN_NAMESPACE

class QScriptContext;
class QScriptContextPrivate;
class QScriptNameIdImpl;

class QScriptFunction: public QScriptObjectData
{
public:
    enum Type {
        Unknown,
        Script,
        C,
        C2,
        C3,
        Qt,
        QtProperty
    };

    QScriptFunction(int len = 0)
        : length(len)
        { }
    virtual ~QScriptFunction();

    virtual void execute(QScriptContextPrivate *context) = 0;
    virtual QString toString(QScriptContextPrivate *context) const;

    virtual Type type() const { return Unknown; }

    // name of the file the function is defined in
    virtual QString fileName() const;

    virtual QString functionName() const;

    virtual int startLineNumber() const;

    virtual int endLineNumber() const;

    virtual void mark(QScriptEnginePrivate *engine, int generation);

public: // ### private
    int length;
    QList<QScriptNameIdImpl*> formals;
};

namespace QScript {

// public API function
class CFunction: public QScriptFunction
{
public:
    CFunction(QScriptFunctionSignature funPtr, int length)
        : QScriptFunction(length), m_funPtr(funPtr)
        { }

    virtual ~CFunction() { }

    virtual void execute(QScriptContextPrivate *context);

    virtual Type type() const { return QScriptFunction::C; }

    virtual QString functionName() const;

private:
    QScriptFunctionSignature m_funPtr;
};

// internal API function
class C2Function: public QScriptFunction
{
public:
    C2Function(QScriptInternalFunctionSignature funPtr, int length,
               QScriptClassInfo *classInfo, const QString &name)
        : QScriptFunction(length), m_funPtr(funPtr),
          m_classInfo(classInfo), m_name(name)
        { }

    virtual ~C2Function() {}

    virtual void execute(QScriptContextPrivate *context);

    virtual Type type() const { return QScriptFunction::C2; }

    virtual QString functionName() const;

private:
    QScriptInternalFunctionSignature m_funPtr;
    QScriptClassInfo *m_classInfo;
    QString m_name;
};

class C3Function: public QScriptFunction
{
public:
    C3Function(QScriptFunctionWithArgSignature funPtr, void *arg, int length)
        : QScriptFunction(length), m_funPtr(funPtr), m_arg(arg)
        { }

    virtual ~C3Function() { }

    virtual void execute(QScriptContextPrivate *context);

    virtual Type type() const { return QScriptFunction::C3; }

private:
    QScriptFunctionWithArgSignature m_funPtr;
    void *m_arg;
};

namespace AST {
    class FunctionExpression;
}

// implemented in qscriptcontext_p.cpp
class ScriptFunction: public QScriptFunction
{
public:
    ScriptFunction(AST::FunctionExpression *definition, NodePool *astPool):
        m_definition(definition), m_astPool(astPool), m_compiledCode(0) {}

    virtual ~ScriptFunction() {}

    virtual void execute(QScriptContextPrivate *context);

    virtual QString toString(QScriptContextPrivate *context) const;

    virtual Type type() const
    { return QScriptFunction::Script; }

    virtual QString fileName() const;

    virtual QString functionName() const;

    virtual int startLineNumber() const;

    virtual int endLineNumber() const;

private:
    AST::FunctionExpression *m_definition;
    QExplicitlySharedDataPointer<NodePool> m_astPool;
    Code *m_compiledCode;
};

} // namespace QScript

QT_END_NAMESPACE

#endif // QT_NO_SCRIPT

#endif // QSCRIPTFUNCTION_P_H
