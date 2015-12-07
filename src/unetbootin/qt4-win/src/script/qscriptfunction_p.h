/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the QtScript module of the Qt Toolkit.
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
