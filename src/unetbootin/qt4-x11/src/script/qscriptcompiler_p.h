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

#ifndef QSCRIPTCOMPILER_P_H
#define QSCRIPTCOMPILER_P_H

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

#include <QtCore/QMap>

#ifndef QT_NO_SCRIPT

#include <QtCore/QVector>

#include "qscriptastvisitor_p.h"
#include "qscriptasm_p.h"

QT_BEGIN_NAMESPACE

class QScriptEnginePrivate;

namespace QScript {

class Compiler: protected AST::Visitor
{
public:
    Compiler(QScriptEnginePrivate *eng);
    virtual ~Compiler();

    inline QScriptEnginePrivate *engine() const { return m_eng; }

    bool topLevelCompiler() const;
    void setTopLevelCompiler(bool b);

    CompilationUnit compile(AST::Node *node, const QList<QScriptNameIdImpl *> &formals
                            = QList<QScriptNameIdImpl *>());

    struct Label {
        Label(int o = 0):
            offset (o) {}

        int offset;
        QVector<int> uses;
    };

// instruction set
    void iAdd();
    void iAssign();
    void iBitAnd();
    void iBitOr();
    void iBitXor();
    void iBitNot();
    void iBranch(int index);
    void iBranchTrue(int index);
    void iBranchFalse(int index);
    void iCall(int argc);
    void iDeclareLocal(QScriptNameIdImpl *id, bool readOnly);
    void iDecr();
    void iDelete();
    void iDiv();
    void iDuplicate();
    void iEqual();
    void iFetch(QScriptNameIdImpl *id);
    void iFetchField();
    void iLazyArguments();
    void iGreatOrEqual();
    void iGreatThan();
    void iIncr();
    void iInplaceAdd();
    void iInplaceAnd();
    void iInplaceDecr();
    void iInplaceDiv();
    void iInplaceLeftShift();
    void iInplaceMod();
    void iInplaceMul();
    void iInplaceOr();
    void iInplaceRightShift();
    void iInplaceSub();
    void iInplaceURightShift();
    void iInstanceOf();
    void iLeftShift();
    void iLessOrEqual();
    void iLessThan();
    void iLoadFalse();
    void iLoadNumber(double number);
    void iLoadString(QScriptNameIdImpl *id);
    void iNewString(QScriptNameIdImpl *id);
    void iLoadThis();
    void iLoadActivation();
    void iLoadNull();
    void iLoadTrue();
    void iLoadUndefined();
    void iMod();
    void iMul();
    void iUnaryPlus();
    void iUnaryMinus();
    void iNew(int argc);
    void iNewArray();
    void iNewClosure(AST::FunctionExpression *expr);
    void iNewObject();
    void iNewRegExp(QScriptNameIdImpl *pattern);
    void iNewRegExp(QScriptNameIdImpl *pattern, int flags);
    void iNot();
    void iNotEqual();
    void iPop();
    void iPostDecr();
    void iPostIncr();
    void iPutField();
    void iResolve(QScriptNameIdImpl *id);
    void iRet();
    void iRightShift();
    void iSub();
    void iStrictEqual();
    void iStrictNotEqual();
    void iSwap();
    void iThrow();
    void iTypeOf();
    void iURightShift();
    void iInplaceXor();
    void iLine(AST::Node *node);
    void iNewEnumeration();
    void iToFirstElement();
    void iHasNextElement();
    void iNextElement();
    void iEnterWith();
    void iLeaveWith();
    void iBeginCatch(QScriptNameIdImpl *id);
    void iEndCatch();
    void iSync();
    void iHalt();
    void iMakeReference();
    void iIn();
    void iNop();
    void iDebugger();

protected:
    virtual bool preVisit(AST::Node *node);

    virtual bool visit(AST::ArrayLiteral *node);
    virtual bool visit(AST::ArrayMemberExpression *node);
    virtual bool visit(AST::Expression *node);
    virtual bool visit(AST::BinaryExpression *node);
    virtual bool visit(AST::BreakStatement *node);
    virtual bool visit(AST::CallExpression *node);
    virtual bool visit(AST::ConditionalExpression *node);
    virtual bool visit(AST::ContinueStatement *node);
    virtual bool visit(AST::DeleteExpression *node);
    virtual bool visit(AST::DoWhileStatement *node);
    virtual bool visit(AST::ExpressionStatement *node);
    virtual bool visit(AST::FalseLiteral *node);
    virtual bool visit(AST::FieldMemberExpression *node);
    virtual bool visit(AST::ForEachStatement *node);
    virtual bool visit(AST::LocalForEachStatement *node);
    virtual bool visit(AST::ForStatement *node);
    virtual bool visit(AST::FunctionDeclaration *node);
    virtual bool visit(AST::FunctionExpression *node);
    virtual bool visit(AST::FunctionSourceElement *node);
    virtual bool visit(AST::IdentifierExpression *node);
    virtual bool visit(AST::IfStatement *node);
    virtual bool visit(AST::LabelledStatement *node);
    virtual bool visit(AST::LocalForStatement *node);
    virtual bool visit(AST::NewExpression *node);
    virtual bool visit(AST::NewMemberExpression *node);
    virtual bool visit(AST::NumericLiteral *node);
    virtual bool visit(AST::ObjectLiteral *node);
    virtual bool visit(AST::PostDecrementExpression *node);
    virtual bool visit(AST::PostIncrementExpression *node);
    virtual bool visit(AST::PreDecrementExpression *node);
    virtual bool visit(AST::PreIncrementExpression *node);
    virtual bool visit(AST::RegExpLiteral *node);
    virtual bool visit(AST::ReturnStatement *node);
    virtual bool visit(AST::SourceElements *node);
    virtual bool visit(AST::StatementList *node);
    virtual bool visit(AST::StatementSourceElement *node);
    virtual bool visit(AST::StringLiteral *node);
    virtual bool visit(AST::SwitchStatement *node);
    virtual bool visit(AST::ThisExpression *node);
    virtual bool visit(AST::NullExpression *node);
    virtual bool visit(AST::ThrowStatement *node);
    virtual bool visit(AST::TryStatement *node);
    virtual bool visit(AST::TrueLiteral *node);
    virtual bool visit(AST::VariableStatement *node);
    virtual bool visit(AST::VariableDeclaration *node);
    virtual bool visit(AST::WhileStatement *node);
    virtual bool visit(AST::WithStatement *node);
    virtual bool visit(AST::Block *node);
    virtual bool visit(AST::TypeOfExpression *node);
    virtual bool visit(AST::DebuggerStatement *node);

    virtual void endVisit(AST::BreakStatement *node);
    virtual void endVisit(AST::ContinueStatement *node);
    virtual void endVisit(AST::ExpressionStatement *node);
    virtual void endVisit(AST::NotExpression *node);
    virtual void endVisit(AST::ReturnStatement *node);
    virtual void endVisit(AST::ThrowStatement *node);
    virtual void endVisit(AST::TildeExpression *node);
    virtual void endVisit(AST::UnaryPlusExpression *node);
    virtual void endVisit(AST::UnaryMinusExpression *node);
    virtual void endVisit(AST::VoidExpression *node);
    virtual void endVisit(AST::EmptyStatement *node);

    void visitForInternal(AST::Statement *node, AST::ExpressionNode *condition,
                          AST::Statement *statement, AST::ExpressionNode *expression);

    bool isAssignmentOperator(int op) const;
    int inplaceAssignmentOperator(int op) const;

    inline int nextInstructionOffset() const
    { return m_instructions.count(); }

    inline void pushInstruction(QScriptInstruction::Operator op)
    {
        pushInstruction(op, QScriptValueImpl(), QScriptValueImpl());
    }

    inline void pushInstruction(QScriptInstruction::Operator op,
                                const QScriptValueImpl &arg1)
    {
        pushInstruction(op, arg1, QScriptValueImpl());
    }

    inline void pushInstruction(QScriptInstruction::Operator op,
                                const QScriptValueImpl &arg1,
                                const QScriptValueImpl &arg2)
    {
        QScriptInstruction i;
        i.op = op;
        i.operand[0] = arg1;
        i.operand[1] = arg2;
        m_instructions.append(i);
    }

    inline bool generateReferences(bool b)
    {
        bool was = m_generateReferences;
        m_generateReferences = b;
        return was;
    }

    inline bool generateFastArgumentLookup(bool b)
    {
        bool was = m_generateFastArgumentLookup;
        m_generateFastArgumentLookup= b;
        return was;
    }

    inline bool iterationStatement(bool b)
    {
        bool was = m_iterationStatement;
        m_iterationStatement = b;
        return was;
    }

    inline bool switchStatement(bool b)
    {
        bool was = m_switchStatement;
        m_switchStatement = b;
        return was;
    }

    inline bool changeParseStatements(bool b)
    {
        bool was = m_parseStatements;
        m_parseStatements = b;
        return was;
    }

    inline bool withStatement(bool b)
    {
        bool was = m_withStatement;
        m_withStatement = b;
        return was;
    }

    inline bool generateLeaveOnBreak(bool b)
    {
        bool was = m_generateLeaveWithOnBreak;
        m_generateLeaveWithOnBreak = b;
        return was;
    }

    void patchInstruction(int index, int offset);

private:
    QScriptEnginePrivate *m_eng;

    uint m_generateReferences: 1;
    uint m_iterationStatement: 1;
    uint m_switchStatement: 1;
    uint m_withStatement: 1;
    uint m_generateLeaveWithOnBreak: 1;
    uint m_generateFastArgumentLookup: 1;
    uint m_parseStatements: 1;
    uint m_pad: 25;

    bool m_topLevelCompiler; // bit
    QVector<QScriptInstruction> m_instructions;
    QVector<ExceptionHandlerDescriptor> m_exceptionHandlers;
    QList<QScriptNameIdImpl *> m_formals;

    struct Loop {
        Loop(QScriptNameIdImpl *n = 0):
            name(n) {}

        QScriptNameIdImpl *name;
        Label breakLabel;
        Label continueLabel;
    };

    inline Loop *changeActiveLoop(Loop *activeLoop)
    {
        Loop *was = m_activeLoop;
        m_activeLoop = activeLoop;
        return was;
    }

    Loop *findLoop(QScriptNameIdImpl *name = 0);

    Loop *m_activeLoop;
    QMap<AST::Statement*, Loop> m_loops;
    CompilationUnit m_compilationUnit;
};

} // namespace QScript

#endif // QT_NO_SCRIPT

QT_END_NAMESPACE

#endif
