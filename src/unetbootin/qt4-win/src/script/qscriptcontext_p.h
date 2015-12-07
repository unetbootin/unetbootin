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

#ifndef QSCRIPTCONTEXT_P_H
#define QSCRIPTCONTEXT_P_H

#include "qscriptcontextfwd_p.h"

#ifndef QT_NO_SCRIPT

#include "qscriptenginefwd_p.h"
#include "qscriptnameid_p.h"

#include <QtCore/qnumeric.h>

QT_BEGIN_NAMESPACE

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

inline QScriptContextPrivate::QScriptContextPrivate()
{
}

inline QScriptContextPrivate *QScriptContextPrivate::get(QScriptContext *q)
{
    if (q)
        return q->d_func();
    return 0;
}

inline const QScriptContextPrivate *QScriptContextPrivate::get(const QScriptContext *q)
{
    if (q)
        return q->d_func();
    return 0;
}

inline QScriptContext *QScriptContextPrivate::create()
{
    return new QScriptContext;
}

inline QScriptEngine *QScriptContextPrivate::engine() const
{
    return m_activation.engine();
}

inline QScriptEnginePrivate *QScriptContextPrivate::enginePrivate() const
{
    return QScriptEnginePrivate::get(engine());
}

inline QScriptContext *QScriptContextPrivate::parentContext() const
{
    return previous;
}

inline void QScriptContextPrivate::init(QScriptContext *parent)
{
    m_state = QScriptContext::NormalState;
    previous = parent;
    args = 0;
    argc = 0;
    m_code = 0;
    iPtr = firstInstruction = lastInstruction = 0;
    stackPtr = tempStack = (parent != 0) ? parent->d_func()->stackPtr : 0;
    m_activation.invalidate();
    m_thisObject.invalidate();
    m_result.invalidate();
    m_scopeChain.invalidate();
    m_callee.invalidate();
    m_arguments.invalidate();
    currentLine = -1;
    currentColumn = -1;
    errorLineNumber = -1;
    m_calledAsConstructor = false;
}

inline QScriptValueImpl QScriptContextPrivate::argument(int index) const
{
    if (index >= argc)
        return QScriptEnginePrivate::get(engine())->undefinedValue();

    Q_ASSERT(args != 0);
    return args[index];
}

inline int QScriptContextPrivate::argumentCount() const
{
    return argc;
}

inline void QScriptContextPrivate::throwException()
{
    m_state = QScriptContext::ExceptionState;
}

inline bool QScriptContextPrivate::hasUncaughtException() const
{
    return m_state == QScriptContext::ExceptionState;
}

inline void QScriptContextPrivate::recover()
{
    m_state = QScriptContext::NormalState;
    errorLineNumber = 0;
}

inline bool QScriptContextPrivate::isNumerical(const QScriptValueImpl &v) const
{
    switch (v.type()) {
    case QScript::BooleanType:
    case QScript::IntegerType:
    case QScript::NumberType:
        return true;

    default:
        return false;
    }
}

inline bool QScriptContextPrivate::eq_cmp(const QScriptValueImpl &lhs, const QScriptValueImpl &rhs)
{
    if (lhs.type() == rhs.type()) {
        switch (lhs.type()) {
        case QScript::UndefinedType:
        case QScript::NullType:
            return true;

        case QScript::NumberType:
            return lhs.m_number_value == rhs.m_number_value;

        case QScript::ReferenceType:
        case QScript::IntegerType:
            return lhs.m_int_value == rhs.m_int_value;

        case QScript::BooleanType:
            return lhs.m_bool_value == rhs.m_bool_value;

        case QScript::StringType:
            if (lhs.m_string_value->unique && rhs.m_string_value->unique)
                return lhs.m_string_value == rhs.m_string_value;
            return lhs.m_string_value->s == rhs.m_string_value->s;

        case QScript::PointerType:
            return lhs.m_ptr_value == rhs.m_ptr_value;

        case QScript::ObjectType:
            if (lhs.isVariant())
                return lhs.m_object_value == rhs.m_object_value || lhs.toVariant() == rhs.toVariant();
#ifndef QT_NO_QOBJECT
            else if (lhs.isQObject())
                return lhs.m_object_value == rhs.m_object_value || lhs.toQObject() == rhs.toQObject();
#endif
            else
                return lhs.m_object_value == rhs.m_object_value;
        }
    }

    return eq_cmp_helper(lhs, rhs);
}

inline bool QScriptContextPrivate::strict_eq_cmp(const QScriptValueImpl &lhs, const QScriptValueImpl &rhs)
{
    if (lhs.type() != rhs.type())
        return false;

    switch (lhs.type()) {
    case QScript::UndefinedType:
    case QScript::NullType:
        return true;

    case QScript::NumberType:
        if (qIsNaN(lhs.m_number_value) || qIsNaN(rhs.m_number_value))
            return false;
        return lhs.m_number_value == rhs.m_number_value;

    case QScript::IntegerType:
        return lhs.m_int_value == rhs.m_int_value;

    case QScript::BooleanType:
        return lhs.m_bool_value == rhs.m_bool_value;

    case QScript::StringType:
        if (lhs.m_string_value->unique && rhs.m_string_value->unique)
            return lhs.m_string_value == rhs.m_string_value;
        return lhs.m_string_value->s == rhs.m_string_value->s;

    default:
        if (lhs.isObject())
            return lhs.m_object_value == rhs.m_object_value;
        break;
    }

    return false;
}

inline QScriptValueImpl QScriptContextPrivate::throwTypeError(const QString &text)
{
    return throwError(QScriptContext::TypeError, text);
}

inline QScriptValueImpl QScriptContextPrivate::throwSyntaxError(const QString &text)
{
    return throwError(QScriptContext::SyntaxError, text);
}

inline QScriptValueImpl QScriptContextPrivate::thisObject() const
{
    return m_thisObject;
}

inline void QScriptContextPrivate::setThisObject(const QScriptValueImpl &object)
{
    m_thisObject = object;
}

inline QScriptValueImpl QScriptContextPrivate::callee() const
{
    return m_callee;
}

inline bool QScriptContextPrivate::isCalledAsConstructor() const
{
    return m_calledAsConstructor;
}

inline QScriptValueImpl QScriptContextPrivate::returnValue() const
{
    return m_result;
}

inline void QScriptContextPrivate::setReturnValue(const QScriptValueImpl &value)
{
    m_result = value;
}

inline QScriptValueImpl QScriptContextPrivate::activationObject() const
{
    return m_activation;
}

inline void QScriptContextPrivate::setActivationObject(const QScriptValueImpl &activation)
{
    m_activation = activation;
}

inline const QScriptInstruction *QScriptContextPrivate::instructionPointer()
{
    return iPtr;
}

inline void QScriptContextPrivate::setInstructionPointer(const QScriptInstruction *instructionPointer)
{
    iPtr = instructionPointer;
}

inline const QScriptValueImpl *QScriptContextPrivate::baseStackPointer() const
{
    return tempStack;
}

inline const QScriptValueImpl *QScriptContextPrivate::currentStackPointer() const
{
    return stackPtr;
}

inline QScriptContext::ExecutionState QScriptContextPrivate::state() const
{
    return m_state;
}

QT_END_NAMESPACE

#endif // QT_NO_SCRIPT
#endif
