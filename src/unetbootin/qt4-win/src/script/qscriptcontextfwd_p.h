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

    inline QScriptEngine *engine() const;
    inline QScriptEnginePrivate *enginePrivate() const;
    inline QScriptContext *parentContext() const;

    inline void init(QScriptContext *parent);
    inline QScriptValueImpl argument(int index) const;
    inline int argumentCount() const;

    inline void throwException();
    inline bool hasUncaughtException() const;
    const QScriptInstruction *findExceptionHandler(const QScriptInstruction *ip) const;
    const QScriptInstruction *findExceptionHandlerRecursive(
        const QScriptInstruction *ip, QScriptContextPrivate **handlerContext) const;
    QScriptContextPrivate *exceptionHandlerContext() const;
    inline void recover();
    QStringList backtrace() const;

    inline bool isNumerical(const QScriptValueImpl &v) const;

    inline bool eq_cmp(const QScriptValueImpl &lhs, const QScriptValueImpl &rhs);

    bool eq_cmp_helper(QScriptValueImpl lhs, QScriptValueImpl rhs);

#if defined(Q_CC_GNU) && __GNUC__ <= 3
    bool lt_cmp(QScriptValueImpl lhs, QScriptValueImpl rhs);
#else
    bool lt_cmp(const QScriptValueImpl &lhs, const QScriptValueImpl &rhs)
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

    bool lt_cmp_helper(QScriptValueImpl lhs, QScriptValueImpl rhs);
#endif

    bool le_cmp(const QScriptValueImpl &lhs, const QScriptValueImpl &rhs)
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

    bool le_cmp_helper(QScriptValueImpl lhs, QScriptValueImpl rhs);

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
    QScriptContext *previous;
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
