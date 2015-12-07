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

#ifndef QSCRIPTASM_P_H
#define QSCRIPTASM_P_H

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

#include <QtCore/qglobal.h>

#ifndef QT_NO_SCRIPT

#include <QtCore/qvector.h>

#include "qscriptvalueimplfwd_p.h"

QT_BEGIN_NAMESPACE

class QTextStream;

class QScriptInstruction
{
public:
    enum Operator {
#define Q_SCRIPT_DEFINE_OPERATOR(op) OP_##op,
#include "instruction.table"
#undef Q_SCRIPT_DEFINE_OPERATOR
        OP_Dummy
    };

public:
    Operator op;
    QScriptValueImpl operand[2];
#if defined(Q_SCRIPT_DIRECT_CODE)
    void *code;
#endif

    void print(QTextStream &out) const;

    static const char *opcode[];
};

namespace QScript {

class NodePool;

class ExceptionHandlerDescriptor
{
public:
    ExceptionHandlerDescriptor() {}

    ExceptionHandlerDescriptor(
        int startInstruction,
        int endInstruction,
        int handlerInstruction)
        : m_startInstruction(startInstruction),
          m_endInstruction(endInstruction),
          m_handlerInstruction(handlerInstruction) {}

    inline int startInstruction() const { return m_startInstruction; }
    inline int endInstruction() const { return m_endInstruction; }
    inline int handlerInstruction() const { return m_handlerInstruction; }

private:
    int m_startInstruction;
    int m_endInstruction;
    int m_handlerInstruction;
};

class CompilationUnit
{
public:
    CompilationUnit(): m_valid(true),
        m_errorLineNumber(-1) {}

    bool isValid() const { return m_valid; }

    void setError(const QString &message, int lineNumber)
    {
        m_errorMessage = message;
        m_errorLineNumber = lineNumber;
        m_valid = false;
    }

    QString errorMessage() const
        { return m_errorMessage; }
    int errorLineNumber() const
        { return m_errorLineNumber; }

    QVector<QScriptInstruction> instructions() const
        { return m_instructions; }
    void setInstructions(const QVector<QScriptInstruction> &instructions)
        { m_instructions = instructions; }

    QVector<ExceptionHandlerDescriptor> exceptionHandlers() const
        { return m_exceptionHandlers; }
    void setExceptionHandlers(const QVector<ExceptionHandlerDescriptor> &exceptionHandlers)
        { m_exceptionHandlers = exceptionHandlers; }

private:
    bool m_valid;
    QString m_errorMessage;
    int m_errorLineNumber;
    QVector<QScriptInstruction> m_instructions;
    QVector<ExceptionHandlerDescriptor> m_exceptionHandlers;
};

class Code
{
public:
    Code();
    ~Code();

    void init(const CompilationUnit &compilation, NodePool *astPool);

public: // attributes
    bool optimized;
    QScriptInstruction *firstInstruction;
    QScriptInstruction *lastInstruction;
    QVector<ExceptionHandlerDescriptor> exceptionHandlers;
    NodePool *astPool;

private:
    Q_DISABLE_COPY(Code)
};


} // namespace QScript

QT_END_NAMESPACE

#endif // QT_NO_SCRIPT
#endif // QSCRIPTASM_P_H
