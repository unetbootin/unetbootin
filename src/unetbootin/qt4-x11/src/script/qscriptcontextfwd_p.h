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

#ifndef QSCRIPTCONTEXTFWD_P_H
#define QSCRIPTCONTEXTFWD_P_H

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

#include "qscriptvalueimplfwd_p.h"

#ifndef QT_NO_SCRIPT

#include "qscriptcontext.h"

#include <QtCore/qobjectdefs.h>

#if defined Q_CC_MSVC && !defined Q_CC_MSVC_NET
#include <QtCore/qnumeric.h>
#endif

QT_BEGIN_NAMESPACE

namespace QScript {
    namespace AST {
    class Node;
    }
class Code;
}

class QScriptInstruction;

class QScriptContextPrivate
{
    Q_DECLARE_PUBLIC(QScriptContext)
public:
    inline QScriptContextPrivate();

    static inline QScriptContextPrivate *get(QScriptContext *q);
    static inline const QScriptContextPrivate *get(const QScriptContext *q);
    static QScriptContext *get(QScriptContextPrivate *d);

    static inline QScriptContext *create();

    inline QScriptEnginePrivate *engine() const;
    inline QScriptContextPrivate *parentContext() const;

    inline void init(QScriptContextPrivate *parent);
    inline QScriptValueImpl argument(int index) const;
    inline int argumentCount() const;
    inline QScriptValueImpl argumentsObject() const;

    inline void throwException();
    inline bool hasUncaughtException() const;
    const QScriptInstruction *findExceptionHandler(const QScriptInstruction *ip) const;
    const QScriptInstruction *findExceptionHandlerRecursive(
        const QScriptInstruction *ip, QScriptContextPrivate **handlerContext) const;
    QScriptContextPrivate *exceptionHandlerContext() const;
    inline void recover();
    QStringList backtrace() const;

    static inline bool isNumerical(const QScriptValueImpl &v);

    static inline bool eq_cmp(const QScriptValueImpl &lhs, const QScriptValueImpl &rhs);

    static bool eq_cmp_helper(QScriptValueImpl lhs, QScriptValueImpl rhs);

#if defined(Q_CC_GNU) && __GNUC__ <= 3
    static bool lt_cmp(QScriptValueImpl lhs, QScriptValueImpl rhs);
#else
    static bool lt_cmp(const QScriptValueImpl &lhs, const QScriptValueImpl &rhs)
    {
        if (lhs.type() == rhs.type()) {
            switch (lhs.type()) {
            case QScript::UndefinedType:
            case QScript::NullType:
                return false;

            case QScript::NumberType:
#if defined Q_CC_MSVC && !defined Q_CC_MSVC_NET
                if (qIsNaN(lhs.m_number_value) || qIsNaN(rhs.m_number_value))
                    return false;
#endif
                return lhs.m_number_value < rhs.m_number_value;

            case QScript::IntegerType:
                return lhs.m_int_value < rhs.m_int_value;

            case QScript::BooleanType:
                return lhs.m_bool_value < rhs.m_bool_value;

            default:
                break;
            } // switch
        }

        return lt_cmp_helper(lhs, rhs);
    }

    static bool lt_cmp_helper(QScriptValueImpl lhs, QScriptValueImpl rhs);
#endif

    static bool le_cmp(const QScriptValueImpl &lhs, const QScriptValueImpl &rhs)
    {
        if (lhs.type() == rhs.type()) {
            switch (lhs.type()) {
            case QScript::UndefinedType:
            case QScript::NullType:
                return true;

            case QScript::NumberType:
                return lhs.m_number_value <= rhs.m_number_value;

            case QScript::IntegerType:
                return lhs.m_int_value <= rhs.m_int_value;

            case QScript::BooleanType:
                return lhs.m_bool_value <= rhs.m_bool_value;

            default:
                break;
            } // switch
        }

        return le_cmp_helper(lhs, rhs);
    }

    static bool le_cmp_helper(QScriptValueImpl lhs, QScriptValueImpl rhs);

    static inline bool strict_eq_cmp(const QScriptValueImpl &lhs, const QScriptValueImpl &rhs);

    bool resolveField(QScriptEnginePrivate *eng, QScriptValueImpl *stackPtr,
                      QScriptValueImpl *value);

    void execute(QScript::Code *code);

    QScriptValueImpl throwError(QScriptContext::Error error, const QString &text);
    QScriptValueImpl throwError(const QString &text);

#ifndef Q_SCRIPT_NO_EVENT_NOTIFY
    qint64 scriptId() const;
#endif
    QString fileName() const;
    QString functionName() const;
    void setDebugInformation(QScriptValueImpl *error) const;

    QScriptValueImpl throwNotImplemented(const QString &name);
    QScriptValueImpl throwNotDefined(const QString &name);
    QScriptValueImpl throwNotDefined(QScriptNameIdImpl *nameId);

    inline QScriptValueImpl throwTypeError(const QString &text);
    inline QScriptValueImpl throwSyntaxError(const QString &text);

    inline QScriptValueImpl thisObject() const;
    inline void setThisObject(const QScriptValueImpl &object);

    inline QScriptValueImpl callee() const;
    inline bool isCalledAsConstructor() const;

    inline QScriptValueImpl returnValue() const;
    inline void setReturnValue(const QScriptValueImpl &value);

    inline QScriptValueImpl activationObject() const;
    inline void setActivationObject(const QScriptValueImpl &activation);

    inline const QScriptInstruction *instructionPointer();
    inline void setInstructionPointer(const QScriptInstruction *instructionPointer);

    inline const QScriptValueImpl *baseStackPointer() const;
    inline const QScriptValueImpl *currentStackPointer() const;

    inline QScriptContext::ExecutionState state() const;

public:
    QScriptContextPrivate *previous;
    int argc;
    QScriptContext::ExecutionState m_state;

    QScriptValueImpl m_activation;
    QScriptValueImpl m_thisObject;
    QScriptValueImpl m_result;
    QScriptValueImpl m_scopeChain;
    QScriptValueImpl m_callee;
    QScriptValueImpl m_arguments;

    QScriptValueImpl *args;
    QScriptValueImpl *tempStack;
    QScriptValueImpl *stackPtr;

    QScript::Code *m_code;
    const QScriptInstruction *iPtr;
    const QScriptInstruction *firstInstruction;
    const QScriptInstruction *lastInstruction;

    int currentLine;
    int currentColumn;

    int errorLineNumber;

    bool catching;
    bool m_calledAsConstructor;

    int calleeMetaIndex;

    QScriptContext *q_ptr;
};

QT_END_NAMESPACE

#endif // QT_NO_SCRIPT
#endif
