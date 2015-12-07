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

inline QScriptEnginePrivate *QScriptContextPrivate::engine() const
{
    return m_activation.engine();
}

inline QScriptContextPrivate *QScriptContextPrivate::parentContext() const
{
    return previous;
}

inline void QScriptContextPrivate::init(QScriptContextPrivate *parent)
{
    m_state = QScriptContext::NormalState;
    previous = parent;
    args = 0;
    argc = 0;
    m_code = 0;
    iPtr = firstInstruction = lastInstruction = 0;
    stackPtr = tempStack = (parent != 0) ? parent->stackPtr : 0;
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
        return engine()->undefinedValue();

    Q_ASSERT(args != 0);
    return args[index];
}

inline int QScriptContextPrivate::argumentCount() const
{
    return argc;
}

inline QScriptValueImpl QScriptContextPrivate::argumentsObject() const
{
    if (!m_arguments.isValid() && m_activation.isValid()) {
        QScriptContextPrivate *dd = const_cast<QScriptContextPrivate*>(this);
        engine()->newArguments(&dd->m_arguments, m_activation,
                               argc, m_callee);
    }
    return m_arguments;
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
    errorLineNumber = -1;
}

inline bool QScriptContextPrivate::isNumerical(const QScriptValueImpl &v)
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
        case QScript::InvalidType:
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

        case QScript::LazyStringType:
            return *lhs.m_lazy_string_value == *rhs.m_lazy_string_value;
        }
    }

    return eq_cmp_helper(lhs, rhs);
}

inline bool QScriptContextPrivate::strict_eq_cmp( const QScriptValueImpl &lhs, const QScriptValueImpl &rhs)
{
    if (lhs.type() != rhs.type())
        return false;

    switch (lhs.type()) {
    case QScript::InvalidType:
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

    case QScript::ObjectType:
        return lhs.m_object_value == rhs.m_object_value;

    case QScript::ReferenceType:
        return lhs.m_int_value == rhs.m_int_value;

    case QScript::PointerType:
        return lhs.m_ptr_value == rhs.m_ptr_value;

    case QScript::LazyStringType:
        return *lhs.m_lazy_string_value == *rhs.m_lazy_string_value;
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
    if (previous && !m_activation.property(QLatin1String("arguments")).isValid()) {
        QScriptContextPrivate *dd = const_cast<QScriptContextPrivate*>(this);
        dd->m_activation.setProperty(QLatin1String("arguments"), argumentsObject());
    }
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
