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

#include "qscriptcompiler_p.h"

#ifndef QT_NO_SCRIPT

#include "qscriptast_p.h"
#include "qscriptengine_p.h"
#include "qscriptvalueimpl_p.h"
#include "qscriptcontext_p.h"
#include "qscriptmember_p.h"
#include "qscriptobject_p.h"

#include <QtCore/QtDebug>

QT_BEGIN_NAMESPACE

namespace QScript {

class Compare : protected AST::Visitor
{
public:
    bool operator()(AST::ExpressionNode *e1, AST::ExpressionNode *e2)
    {
        if (!e1 || !e2)
            return (e1 == e2);

        if (e1->kind != e2->kind)
            return false;

        m_e2 = e2;
        m_equal = false;
        e1->accept(this);
        return m_equal;
    }

protected:
    virtual bool visit(AST::ThisExpression *)
    {
        m_equal = true;
        return false;
    }
    virtual bool visit(AST::NullExpression *)
    {
        m_equal = true;
        return false;
    }
    virtual bool visit(AST::VoidExpression *)
    {
        m_equal = true;
        return false;
    }
    virtual bool visit(AST::FalseLiteral *)
    {
        m_equal = true;
        return false;
    }
    virtual bool visit(AST::TrueLiteral *)
    {
        m_equal = true;
        return false;
    }
    virtual bool visit(AST::NumericLiteral *e1)
    {
        AST::NumericLiteral *e2 = static_cast<AST::NumericLiteral*>(m_e2);
        m_equal = (e1->value == e2->value);
        return false;
    }
    virtual bool visit(AST::RegExpLiteral *e1)
    {
        AST::RegExpLiteral *e2 = static_cast<AST::RegExpLiteral*>(m_e2);
        m_equal = (e1->pattern == e2->pattern)
                  && (e1->flags == e2->flags);
        return false;
    }
    virtual bool visit(AST::StringLiteral *e1)
    {
        AST::StringLiteral *e2 = static_cast<AST::StringLiteral*>(m_e2);
        m_equal = (e1->value == e2->value);
        return false;
    }
    virtual bool visit(AST::IdentifierExpression *e1)
    {
        AST::IdentifierExpression *e2 = static_cast<AST::IdentifierExpression*>(m_e2);
        m_equal = (e1->name == e2->name);
        return false;
    }
    virtual bool visit(AST::ArrayMemberExpression *e1)
    {
        AST::ArrayMemberExpression *e2 = static_cast<AST::ArrayMemberExpression*>(m_e2);
        m_equal = operator()(e1->base, e2->base)
                  && operator()(e1->expression, e2->expression);
        return false;
    }
    virtual bool visit(AST::FieldMemberExpression *e1)
    {
        AST::FieldMemberExpression *e2 = static_cast<AST::FieldMemberExpression*>(m_e2);
        m_equal = (e1->name == e2->name) && operator()(e1->base, e2->base);
        return false;
    }
    virtual bool visit(AST::BinaryExpression *e1)
    {
        AST::BinaryExpression *e2 = static_cast<AST::BinaryExpression*>(m_e2);
        m_equal = (e1->op == e2->op) && operator()(e1->left, e2->left)
                  && operator()(e1->right, e2->right);
        return false;
    }
    virtual bool visit(AST::ConditionalExpression *e1)
    {
        AST::ConditionalExpression *e2 = static_cast<AST::ConditionalExpression*>(m_e2);
        m_equal = operator()(e1->expression, e2->expression)
                  && operator()(e1->ok, e2->ok)
                  && operator()(e1->ko, e2->ko);
        return false;
    }
    virtual bool visit(AST::TypeOfExpression *e1)
    {
        AST::TypeOfExpression *e2 = static_cast<AST::TypeOfExpression*>(m_e2);
        m_equal = operator()(e1->expression, e2->expression);
        return false;
    }
    virtual bool visit(AST::UnaryPlusExpression *e1)
    {
        AST::UnaryPlusExpression *e2 = static_cast<AST::UnaryPlusExpression*>(m_e2);
        m_equal = operator()(e1->expression, e2->expression);
        return false;
    }
    virtual bool visit(AST::UnaryMinusExpression *e1)
    {
        AST::UnaryMinusExpression *e2 = static_cast<AST::UnaryMinusExpression*>(m_e2);
        m_equal = operator()(e1->expression, e2->expression);
        return false;
    }
    virtual bool visit(AST::TildeExpression *e1)
    {
        AST::TildeExpression *e2 = static_cast<AST::TildeExpression*>(m_e2);
        m_equal = operator()(e1->expression, e2->expression);
        return false;
    }
    virtual bool visit(AST::NotExpression *e1)
    {
        AST::NotExpression *e2 = static_cast<AST::NotExpression*>(m_e2);
        m_equal = operator()(e1->expression, e2->expression);
        return false;
    }
    virtual bool visit(AST::Expression *)
    { return false; }
    virtual bool visit(AST::ArrayLiteral *)
    { return false; }
    virtual bool visit(AST::ObjectLiteral *)
    { return false; }
    virtual bool visit(AST::CallExpression *)
    { return false; }
    virtual bool visit(AST::DeleteExpression *)
    { return false; }
    virtual bool visit(AST::FunctionExpression *)
    { return false; }
    virtual bool visit(AST::NewExpression *)
    { return false; }
    virtual bool visit(AST::NewMemberExpression *)
    { return false; }
    virtual bool visit(AST::PostDecrementExpression *)
    { return false; }
    virtual bool visit(AST::PostIncrementExpression *)
    { return false; }
    virtual bool visit(AST::PreDecrementExpression *)
    { return false; }
    virtual bool visit(AST::PreIncrementExpression *)
    { return false; }

private:
    AST::ExpressionNode *m_e2;
    bool m_equal;
};

class FetchName: protected AST::Visitor
{
public:
    inline FetchName(QScriptEnginePrivate *e):
        eng(e), name(0) {}

    QScriptNameIdImpl *operator() (AST::PropertyName *node)
    {
        name = 0;
        node->accept(this);
        return name;
    }

protected:
    virtual bool visit(AST::IdentifierPropertyName *node)
    {
        name = node->id;
        return false;
    }

    virtual bool visit(AST::StringLiteralPropertyName *node)
    {
        name = node->id;
        return false;
    }

    virtual bool visit(AST::NumericLiteralPropertyName *node)
    {
        name = eng->nameId(QString::number(node->id), /*persistent=*/false); // ### don't use QString::number
        name->persistent = true; // ### remove
        return false;
    }

private:
    QScriptEnginePrivate *eng;
    QScriptNameIdImpl *name;
};

class EmptySourceElements: protected AST::Visitor
{
public:
    EmptySourceElements(QScriptEngine *d):
        driver(d), empty(false) {}

    inline bool operator () (AST::Node *)
    {
        empty = false;
        return empty;
    }

private:
    QScriptEngine *driver;
    bool empty;
};

class DeclareLocals: protected AST::Visitor
{
public:
    DeclareLocals(Compiler *c):
        compiler(c),
        eng(c->engine())
    {
    }

    void operator () (AST::Node *node)
    {
        if (node)
            node->accept(this);
    }

protected:
    virtual bool visit(AST::FunctionDeclaration *node)
    {
        compiler->iDeclareLocal(node->name, /*readOnly=*/false);
        return false;
    }

    virtual bool visit(AST::FunctionExpression *)
    { return false; }

    virtual bool visit(AST::VariableDeclaration *node)
    {
        compiler->iDeclareLocal(node->name, node->readOnly);
        return false;
    }

private:
    Compiler *compiler;
    QScriptEnginePrivate *eng;
};

Compiler::Compiler(QScriptEnginePrivate *eng):
    m_eng(eng),
    m_generateReferences(0), m_iterationStatement(0),
    m_switchStatement(0), m_withStatement(0),
    m_generateLeaveWithOnBreak(0), m_generateFastArgumentLookup(0),
    m_parseStatements(0), m_pad(0),
    m_topLevelCompiler(false),
    m_activeLoop(0)
{
}

Compiler::~Compiler()
{
}

bool Compiler::topLevelCompiler() const
{
    return m_topLevelCompiler;
}

void Compiler::setTopLevelCompiler(bool b)
{
    m_topLevelCompiler = b;
}

 CompilationUnit Compiler::compile(AST::Node *node, const QList<QScriptNameIdImpl *> &formals)
{
    m_formals = formals;
    m_generateReferences = 0;
    m_iterationStatement = 0;
    m_switchStatement = 0;
    m_withStatement = 0;
    m_generateLeaveWithOnBreak = 0;
    m_generateFastArgumentLookup = 0;
    m_parseStatements = 0;
    m_pad = 0;
    m_instructions.clear();
    m_exceptionHandlers.clear();
    m_generateFastArgumentLookup = false; // ### !formals.isEmpty();  // ### disabled for now.. it's buggy :(

    m_compilationUnit = CompilationUnit();

    if (node)
        node->accept(this);

    // add a terminator
    if (topLevelCompiler()) {
        iHalt();
    } else if (m_instructions.isEmpty() || m_instructions.last().op != QScriptInstruction::OP_Ret) {
        iLoadUndefined();
        iRet();
    }

    m_compilationUnit.setInstructions(m_instructions);
    m_compilationUnit.setExceptionHandlers(m_exceptionHandlers);
    return m_compilationUnit;
}

bool Compiler::preVisit(AST::Node *)
{
    return m_compilationUnit.isValid();
}

bool Compiler::visit(AST::SourceElements *node)
{
    DeclareLocals declareLocals(this);
    declareLocals(node);

    bool was = changeParseStatements(false);

    for (AST::SourceElements *it = node; it != 0; it = it->next)
        it->element->accept(this);

    changeParseStatements(true);

    for (AST::SourceElements *it = node; it != 0; it = it->next)
        it->element->accept(this);

    changeParseStatements(was);

    return false;
}

bool Compiler::visit(AST::StatementList *)
{
    return true;
}

bool Compiler::visit(AST::FunctionSourceElement *)
{
    return m_parseStatements == 0;
}

bool Compiler::visit(AST::StatementSourceElement *)
{
    return m_parseStatements;
}

bool Compiler::visit(AST::ThisExpression *)
{
    iLoadThis();
    return false;
}

bool Compiler::visit(AST::NullExpression *)
{
    iLoadNull();
    return false;
}

bool Compiler::visit(AST::RegExpLiteral *node)
{
    Q_ASSERT(node->pattern != 0);

    if (node->flags)
        iNewRegExp(node->pattern, node->flags);
    else
        iNewRegExp(node->pattern);

    return false;
}

bool Compiler::visit(AST::NumericLiteral *node)
{
    iLoadNumber(node->value);
    return false;
}

bool Compiler::visit(AST::StringLiteral *node)
{
    iNewString(node->value);

    return false;
}

bool Compiler::visit(AST::ObjectLiteral *node)
{
    iNewObject();

    FetchName fetchName(m_eng);
    bool was = generateReferences(false);
    for (AST::PropertyNameAndValueList *it = node->properties; it != 0; it = it->next) {
        iLine(it->value);
        iDuplicate();

        QScriptNameIdImpl *name = fetchName(it->name);
        Q_ASSERT(name != 0);
        iLoadString(name);
        iMakeReference();

        it->value->accept(this);
        iPutField();
    }
    generateReferences(was);

    return false;
}

bool Compiler::visit(AST::IdentifierExpression *node)
{
    Q_ASSERT(node->name != 0);

    if (node->name == m_eng->idTable()->id_arguments)
        iLazyArguments();
    if (m_generateReferences)
        iResolve(node->name);
    else
        iFetch(node->name);

    return false;
}

bool Compiler::visit(AST::FunctionDeclaration *node)
{
    iLoadActivation();
    iLoadString(node->name);
    iMakeReference();
    iNewClosure(node);
    iPutField();
    return false;
}

bool Compiler::visit(AST::FunctionExpression *node)
{
    iNewClosure(node);
    if (node->name) {
        iDuplicate();
        iLoadActivation();
        iSwap();
        iLoadString(node->name);
        iSwap();
        iMakeReference();
        iSwap();
        iPutField();
    }
    return false;
}

bool Compiler::visit(AST::CallExpression *node)
{
    bool was = generateReferences(true);
    node->base->accept(this);
    generateReferences(false);

    int argc = 0;
    for (AST::ArgumentList *it = node->arguments; it != 0; it = it->next) {
        it->expression->accept(this);
        ++argc;
    }

    generateReferences(was);

    iCall(argc);
    return false;
}

bool Compiler::visit(AST::NewExpression *node)
{
    bool was = generateReferences(true);
    node->expression->accept(this);
    generateReferences(was);
    iNew(0);
    return false;
}

bool Compiler::visit(AST::NewMemberExpression *node)
{
    bool was = generateReferences(true);
    node->base->accept(this);
    generateReferences(false);

    int argc = 0;
    for (AST::ArgumentList *it = node->arguments; it != 0; it = it->next) {
        it->expression->accept(this);
        ++argc;
    }

    generateReferences(was);

    iNew(argc);
    return false;
}

bool Compiler::visit(AST::FieldMemberExpression *node)
{
    bool was = generateReferences(false);
    node->base->accept(this);
    generateReferences(was);

    iLoadString(node->name);

    if (! was)
        iFetchField();
    else
        iMakeReference();

    return false;
}

bool Compiler::visit(AST::ArrayMemberExpression *node)
{
    bool was = generateReferences(false);
    node->base->accept(this);
    node->expression->accept(this);
    generateReferences(was);

    if (! was)
        iFetchField();
    else
        iMakeReference();

    return false;
}

bool Compiler::visit(AST::PostIncrementExpression *node)
{
    bool was = generateReferences(true);
    node->base->accept(this);
    generateReferences(was);
    iPostIncr();

    return false;
}

bool Compiler::visit(AST::PostDecrementExpression *node)
{
    bool was = generateReferences(true);
    node->base->accept(this);
    generateReferences(was);
    iPostDecr();

    return false;
}

bool Compiler::visit(AST::PreIncrementExpression *node)
{
    bool was = generateReferences(true);
    node->expression->accept(this);
    generateReferences(was);
    iIncr();
    return false;
}

bool Compiler::visit(AST::PreDecrementExpression *node)
{
    bool was = generateReferences(true);
    node->expression->accept(this);
    generateReferences(was);
    iDecr();
    return false;
}

void Compiler::endVisit(AST::NotExpression *)
{
    iNot();
}

void Compiler::endVisit(AST::TildeExpression *)
{
    iBitNot();
}

bool Compiler::visit(AST::ThrowStatement *node)
{
    iLine(node);
    return true;
}

bool Compiler::visit(AST::TryStatement *node)
{
    int start = nextInstructionOffset();
    if (node->statement)
        node->statement->accept(this);
    int end = nextInstructionOffset();
    if (node->catchExpression) {
        iBranch(0); // skip the catch if no exception
        ExceptionHandlerDescriptor ehd(start, end, nextInstructionOffset());
        m_exceptionHandlers.append(ehd);
        iBeginCatch(node->catchExpression->name);
        node->catchExpression->statement->accept(this);
        iEndCatch();
        patchInstruction(end, nextInstructionOffset() - end);
    }
    if (node->finallyExpression) {
        if (!node->catchExpression) {
            ExceptionHandlerDescriptor ehd(start, end, nextInstructionOffset());
            m_exceptionHandlers.prepend(ehd);
        }
        node->finallyExpression->statement->accept(this);
    }
    return false;
}

void Compiler::endVisit(AST::ThrowStatement *node)
{
    if (! node->expression)
        iLoadUndefined();

    iThrow();
}

void Compiler::endVisit(AST::VoidExpression *)
{
    iPop();
    iLoadUndefined();
}

bool Compiler::visit(AST::TypeOfExpression *node)
{
    bool was = generateReferences(true);
    node->expression->accept(this);
    generateReferences(was);
    iTypeOf();
    return false;
}

bool Compiler::visit(AST::DeleteExpression *node)
{
    bool was = generateReferences(true);
    node->expression->accept(this);
    generateReferences(was);
    iDelete();
    return false;
}

bool Compiler::visit(AST::ReturnStatement *node)
{
    if (topLevelCompiler()) {
        m_compilationUnit.setError(QString::fromUtf8("return outside function body"),
                                   node->startLine);
        return false;
    }
    iLine(node);
    return true;
}

void Compiler::endVisit(AST::ReturnStatement *node)
{
    if (! node->expression)
        iLoadUndefined();

    iRet();
}

bool Compiler::visit(AST::VariableStatement *node)
{
    AST::VariableDeclarationList *lst = node->declarations;
    while (lst) {
        if (lst->declaration->expression) {
            iLine(node);
            break;
        }
        lst = lst->next;
    }
    return true;
}

bool Compiler::visit(AST::VariableDeclaration *node)
{
    if (node->expression != 0) {
        iResolve(node->name);
        node->expression->accept(this);
        iAssign();
        iPop();
    }

    return false;
}

bool Compiler::visit(AST::ConditionalExpression *node)
{
    node->expression->accept(this);

    int cond = nextInstructionOffset();
    iBranchFalse(0);

    node->ok->accept(this);

    if (! node->ko) {
        patchInstruction(cond, nextInstructionOffset() - cond);
    } else {
        int terminator = nextInstructionOffset();
        iBranch(0);
        node->ko->accept(this);

        patchInstruction(cond, terminator + 1 - cond);
        patchInstruction(terminator, nextInstructionOffset() - terminator);
    }

    return false;
}

bool Compiler::visit(AST::IfStatement *node)
{
    iLine(node);
    node->expression->accept(this);

    int cond = nextInstructionOffset();
    iBranchFalse(0);

    node->ok->accept(this);

    if (! node->ko) {
        patchInstruction(cond, nextInstructionOffset() - cond);
    } else {
        int terminator = nextInstructionOffset();
        iBranch(0);
        node->ko->accept(this);

        patchInstruction(cond, terminator + 1 - cond);
        patchInstruction(terminator, nextInstructionOffset() - terminator);
    }
    if (!m_instructions.isEmpty() && m_instructions.last().op == QScriptInstruction::OP_Ret)
        iNop();

    return false;
}

bool Compiler::visit(AST::Block *node)
{
    if (node->statements && m_loops.contains(node)) {
        Loop &loop = m_loops[node];

        node->statements->accept(this);

        loop.breakLabel.offset = nextInstructionOffset();

        foreach (int index, loop.breakLabel.uses) {
            patchInstruction(index, loop.breakLabel.offset - index);
        }

        return false;
    }

    return true;
}

bool Compiler::visit(AST::WhileStatement *node)
{
    Loop *previousLoop = changeActiveLoop(&m_loops[node]);
    m_activeLoop->continueLabel.offset = nextInstructionOffset();

    iLine(node);
    int again = nextInstructionOffset();
    node->expression->accept(this);

    int cond = nextInstructionOffset();
    iBranchFalse(0);

    bool was = iterationStatement(true);
    bool was2 = generateLeaveOnBreak(false);
    node->statement->accept(this);
    generateLeaveOnBreak(was2);
    iterationStatement(was);

    iBranch(again - nextInstructionOffset());
    patchInstruction(cond, nextInstructionOffset() - cond);

    m_activeLoop->breakLabel.offset = nextInstructionOffset();

    foreach (int index, m_activeLoop->breakLabel.uses) {
        patchInstruction(index, m_activeLoop->breakLabel.offset - index);
    }

    foreach (int index, m_activeLoop->continueLabel.uses) {
        patchInstruction(index, m_activeLoop->continueLabel.offset - index);
    }

    changeActiveLoop(previousLoop);
    m_loops.remove(node);

    return false;
}

bool Compiler::visit(AST::DoWhileStatement *node)
{
    Loop *previousLoop = changeActiveLoop(&m_loops[node]);
    int again = nextInstructionOffset();
    iLine(node);
    bool was = iterationStatement(true);
    node->statement->accept(this);
    iterationStatement(was);

    m_activeLoop->continueLabel.offset = nextInstructionOffset();

    node->expression->accept(this);

    iBranchTrue(again - nextInstructionOffset());
    m_activeLoop->breakLabel.offset = nextInstructionOffset();

    foreach (int index, m_activeLoop->breakLabel.uses) {
        patchInstruction(index, m_activeLoop->breakLabel.offset - index);
    }

    foreach (int index, m_activeLoop->continueLabel.uses) {
        patchInstruction(index, m_activeLoop->continueLabel.offset - index);
    }

    changeActiveLoop(previousLoop);
    m_loops.remove(node);

    return false;
}

bool Compiler::visit(AST::ForEachStatement *node)
{
    Loop *previousLoop = changeActiveLoop(&m_loops[node]);

    iLine(node);
    node->expression->accept(this);
    iNewEnumeration();
    iDuplicate();
    iToFirstElement();

    int again = nextInstructionOffset();
    m_activeLoop->continueLabel.offset = again;
    iDuplicate();
    iHasNextElement();
    int cond = nextInstructionOffset();
    iBranchFalse(0);
    bool was = generateReferences(true);
    node->initialiser->accept(this);
    generateReferences(was);
    iNextElement();
    iAssign();
    iPop();
    was = iterationStatement(true);
    node->statement->accept(this);
    iterationStatement(was);
    iBranch(again - nextInstructionOffset());
    patchInstruction(cond, nextInstructionOffset() - cond);

    m_activeLoop->breakLabel.offset = nextInstructionOffset();
    iPop(); // pop the Enumeration

    foreach (int index, m_activeLoop->breakLabel.uses) {
        patchInstruction(index, m_activeLoop->breakLabel.offset - index);
    }

    foreach (int index, m_activeLoop->continueLabel.uses) {
        patchInstruction(index, m_activeLoop->continueLabel.offset - index);
    }

    changeActiveLoop(previousLoop);
    m_loops.remove(node);

    return false;
}

bool Compiler::visit(AST::LocalForEachStatement *node)
{
    Loop *previousLoop = changeActiveLoop(&m_loops[node]);

    iLine(node);
    node->declaration->accept(this);
    node->expression->accept(this);
    iNewEnumeration();
    iDuplicate();
    iToFirstElement();

    int again = nextInstructionOffset();
    m_activeLoop->continueLabel.offset = again;
    iDuplicate();
    iHasNextElement();
    int cond = nextInstructionOffset();
    iBranchFalse(0);
    iResolve(node->declaration->name);
    iNextElement();
    iAssign();
    iPop();
    bool was = iterationStatement(true);
    node->statement->accept(this);
    iterationStatement(was);
    iBranch(again - nextInstructionOffset());
    patchInstruction(cond, nextInstructionOffset() - cond);

    m_activeLoop->breakLabel.offset = nextInstructionOffset();
    iPop(); // pop the Enumeration

    foreach (int index, m_activeLoop->breakLabel.uses) {
        patchInstruction(index, m_activeLoop->breakLabel.offset - index);
    }

    foreach (int index, m_activeLoop->continueLabel.uses) {
        patchInstruction(index, m_activeLoop->continueLabel.offset - index);
    }

    changeActiveLoop(previousLoop);
    m_loops.remove(node);

    return false;
}

void Compiler::visitForInternal(AST::Statement *node, AST::ExpressionNode *condition, AST::Statement *statement, AST::ExpressionNode *expression)
{
    Q_ASSERT(statement != 0);

    int again = nextInstructionOffset();
    if (condition != 0) {
//        iLine(condition);
        condition->accept(this);
    } else {
//        iLine(node);
        iLoadNumber(1);
    }

    int cond = nextInstructionOffset();
    iBranchFalse(0);

    Loop *previousLoop = changeActiveLoop(&m_loops[node]);

    bool was = iterationStatement(true);
    statement->accept(this);
    iterationStatement(was);

    m_activeLoop->continueLabel.offset = nextInstructionOffset();

    if (expression != 0) {
        expression->accept(this);
        iPop();
    }

    iBranch(again - nextInstructionOffset());
    patchInstruction(cond, nextInstructionOffset() - cond);

    m_activeLoop->breakLabel.offset = nextInstructionOffset();

    foreach (int index, m_activeLoop->breakLabel.uses) {
        patchInstruction(index, m_activeLoop->breakLabel.offset - index);
    }

    foreach (int index, m_activeLoop->continueLabel.uses) {
        patchInstruction(index, m_activeLoop->continueLabel.offset - index);
    }

    changeActiveLoop(previousLoop);
    m_loops.remove(node);
}

bool Compiler::visit(AST::ForStatement *node)
{
    iLine(node);

    if (node->initialiser != 0) {
        node->initialiser->accept(this);
        iPop();
    }

    visitForInternal(node, node->condition, node->statement, node->expression);
    return false;
}

bool Compiler::visit(AST::LocalForStatement *node)
{
    iLine(node);

    if (node->declarations)
        node->declarations->accept(this);

    visitForInternal(node, node->condition, node->statement, node->expression);
    return false;
}

bool Compiler::isAssignmentOperator(int op) const
{
    switch (op) {

    case QSOperator::Assign:
    case QSOperator::InplaceAnd:
    case QSOperator::InplaceSub:
    case QSOperator::InplaceDiv:
    case QSOperator::InplaceAdd:
    case QSOperator::InplaceLeftShift:
    case QSOperator::InplaceMod:
    case QSOperator::InplaceMul:
    case QSOperator::InplaceOr:
    case QSOperator::InplaceRightShift:
    case QSOperator::InplaceURightShift:
    case QSOperator::InplaceXor:
        return true;

    default:
        break;

    }

    return false;
}

int Compiler::inplaceAssignmentOperator(int op) const
{
    switch (op) {
    case QSOperator::BitAnd:
        return QSOperator::InplaceAnd;
    case QSOperator::Sub:
        return QSOperator::InplaceSub;
    case QSOperator::Div:
        return QSOperator::InplaceDiv;
    case QSOperator::Add:
        return QSOperator::InplaceAdd;
    case QSOperator::LShift:
        return QSOperator::InplaceLeftShift;
    case QSOperator::Mod:
        return QSOperator::InplaceMod;
    case QSOperator::Mul:
        return QSOperator::InplaceMul;
    case QSOperator::BitOr:
        return QSOperator::InplaceOr;
    case QSOperator::RShift:
        return QSOperator::InplaceRightShift;
    case QSOperator::URShift:
        return QSOperator::InplaceURightShift;
    case QSOperator::BitXor:
        return QSOperator::InplaceXor;

    default:
        break;

    }

    return(-1);
}

bool Compiler::visit(AST::Expression *node)
{
    node->left->accept(this);
    iPop(); // ### or iSync?
    node->right->accept(this);
    return false;
}

bool Compiler::visit(AST::BinaryExpression *node)
{
    if (isAssignmentOperator(node->op)) {
        bool was = generateReferences(true);
        node->left->accept(this);
        generateReferences(was);
    } else {
        node->left->accept(this);
    }

    int address = 0;
    if (node->op == QSOperator::Or || node->op == QSOperator::And) {
        iDuplicate();
        address = nextInstructionOffset();
        if (node->op == QSOperator::Or)
            iBranchTrue(0);
        else
            iBranchFalse(0);
        iPop();
    }

    int op = node->op;
    Compare compare;
    if ((op == QSOperator::Assign) && node->right->binaryExpressionCast()
        && (inplaceAssignmentOperator(node->right->binaryExpressionCast()->op) != -1)
        && compare(node->left, node->right->binaryExpressionCast()->left)) {
        // node->left is equivalent to node->right->left, so we generate
        // x op= y rather than x = x op y
        op = inplaceAssignmentOperator(node->right->binaryExpressionCast()->op);
        node->right->binaryExpressionCast()->right->accept(this);
    } else {
        node->right->accept(this);
    }

    switch (op) {

    case QSOperator::Assign:
        iAssign();
        break;

    case QSOperator::InplaceAnd:
        iInplaceAnd();
        break;

    case QSOperator::InplaceSub:
        iInplaceSub();
        break;

    case QSOperator::InplaceDiv:
        iInplaceDiv();
        break;

    case QSOperator::InplaceAdd:
        iInplaceAdd();
        break;

    case QSOperator::InplaceLeftShift:
        iInplaceLeftShift();
        break;

    case QSOperator::InplaceMod:
        iInplaceMod();
        break;

    case QSOperator::InplaceMul:
        iInplaceMul();
        break;

    case QSOperator::InplaceOr:
        iInplaceOr();
        break;

    case QSOperator::InplaceRightShift:
        iInplaceRightShift();
        break;

    case QSOperator::InplaceURightShift:
        iInplaceURightShift();
        break;

    case QSOperator::InplaceXor:
        iInplaceXor();
        break;

    case QSOperator::BitAnd:
        iBitAnd();
        break;

    case QSOperator::BitOr:
        iBitOr();
        break;

    case QSOperator::BitXor:
        iBitXor();
        break;

    case QSOperator::LShift:
        iLeftShift();
        break;

    case QSOperator::Mod:
        iMod();
        break;

    case QSOperator::RShift:
        iRightShift();
        break;

    case QSOperator::URShift:
        iURightShift();
        break;

    case QSOperator::InstanceOf:
        iInstanceOf();
        break;

    case QSOperator::Add:
        iAdd();
        break;

    case QSOperator::And:
        patchInstruction(address, nextInstructionOffset() - address);
        break;

    case QSOperator::Div:
        iDiv();
        break;

    case QSOperator::Equal:
        iEqual();
        break;

    case QSOperator::Ge:
        iGreatOrEqual();
        break;

    case QSOperator::Gt:
        iGreatThan();
        break;

    case QSOperator::Le:
        iLessOrEqual();
        break;

    case QSOperator::Lt:
        iLessThan();
        break;

    case QSOperator::Mul:
        iMul();
        break;

    case QSOperator::NotEqual:
        iNotEqual();
        break;

    case QSOperator::Or:
        patchInstruction(address, nextInstructionOffset() - address);
        break;

    case QSOperator::Sub:
        iSub();
        break;

    case QSOperator::StrictEqual:
        iStrictEqual();
        break;

    case QSOperator::StrictNotEqual:
        iStrictNotEqual();
        break;

    case QSOperator::In:
        iIn();
        break;
    }

    return false;
}

bool Compiler::visit(AST::TrueLiteral *)
{
    iLoadTrue();
    return false;
}

bool Compiler::visit(AST::FalseLiteral *)
{
    iLoadFalse();
    return false;
}

bool Compiler::visit(AST::SwitchStatement *node)
{
    iLine(node);
    Loop *previousLoop = changeActiveLoop(&m_loops[node]);

    node->expression->accept(this);

    bool was = switchStatement(true);

    AST::CaseClauses *clauses;
    int skipIndex = -1;
    int fallthroughIndex = -1;
    // ### make a function for this
    for (clauses = node->block->clauses; clauses != 0; clauses = clauses->next) {
        AST::CaseClause *clause = clauses->clause;
        if (skipIndex != -1)
            patchInstruction(skipIndex, nextInstructionOffset() - skipIndex);

        iDuplicate(); // expression
        clause->expression->accept(this);
        iStrictEqual();
        skipIndex = nextInstructionOffset();
        iBranchFalse(0); // next case

        if (fallthroughIndex != -1) // previous case falls through to here
            patchInstruction(fallthroughIndex, nextInstructionOffset() - fallthroughIndex);

        int breaksBefore = m_activeLoop->breakLabel.uses.count();
        if (clause->statements)
            clause->statements->accept(this);
        int breaksAfter = m_activeLoop->breakLabel.uses.count();
        if (breaksAfter == breaksBefore) { // fallthrough
            fallthroughIndex = nextInstructionOffset();
            iBranch(0);
        } else { // no fallthrough (break)
            fallthroughIndex = -1;
        }
    }

    if (fallthroughIndex != -1) {
        patchInstruction(fallthroughIndex, nextInstructionOffset() - fallthroughIndex);
        fallthroughIndex = -1;
    }

    int defaultIndex = -1;
    if (node->block->defaultClause) {
        int skipDefaultIndex = -1;
        if (!node->block->clauses && node->block->moreClauses) {
            skipDefaultIndex = nextInstructionOffset();
            iBranch(0);
        }
        defaultIndex = nextInstructionOffset();
        int breaksBefore = m_activeLoop->breakLabel.uses.count();
        if (node->block->defaultClause->statements)
            node->block->defaultClause->statements->accept(this);
        int breaksAfter = m_activeLoop->breakLabel.uses.count();
        if (breaksAfter == breaksBefore) { // fallthrough
            fallthroughIndex = nextInstructionOffset();
            iBranch(0);
        } else { // no fallthrough (break)
            fallthroughIndex = -1;
        }
        if (skipDefaultIndex != -1)
            patchInstruction(skipDefaultIndex, nextInstructionOffset() - skipDefaultIndex);
    }

    for (clauses = node->block->moreClauses; clauses != 0; clauses = clauses->next) {
        AST::CaseClause *clause = clauses->clause;
        if (skipIndex != -1)
            patchInstruction(skipIndex, nextInstructionOffset() - skipIndex);

        iDuplicate(); // expression
        clause->expression->accept(this);
        iStrictEqual();
        skipIndex = nextInstructionOffset();
        iBranchFalse(0); // next case

        if (fallthroughIndex != -1) // previous case falls through to here
            patchInstruction(fallthroughIndex, nextInstructionOffset() - fallthroughIndex);

        int breaksBefore = m_activeLoop->breakLabel.uses.count();
        if (clause->statements)
            clause->statements->accept(this);
        int breaksAfter = m_activeLoop->breakLabel.uses.count();
        if (breaksAfter == breaksBefore) { // fallthrough
            fallthroughIndex = nextInstructionOffset();
            iBranch(0);
        } else { // no fallthrough (break)
            fallthroughIndex = -1;
        }
    }

    if (skipIndex != -1) {
        patchInstruction(skipIndex, nextInstructionOffset() - skipIndex);
        if (defaultIndex != -1)
            iBranch(defaultIndex - nextInstructionOffset()); // goto default
    }

    if (fallthroughIndex != -1)
        patchInstruction(fallthroughIndex, nextInstructionOffset() - fallthroughIndex);

    // backpatch the breaks
    int term = nextInstructionOffset();
    foreach (int index, m_activeLoop->breakLabel.uses) {
        patchInstruction(index, term - index);
    }

    iPop(); // expression

    if (previousLoop && !m_activeLoop->continueLabel.uses.isEmpty()) {
        // join the continues and add to outer loop
        iBranch(3);
        foreach (int index, m_activeLoop->continueLabel.uses) {
            patchInstruction(index, nextInstructionOffset() - index);
        }
        iPop();
        iBranch(0);
        previousLoop->continueLabel.uses.append(nextInstructionOffset() - 1);
    }

    switchStatement(was);
    changeActiveLoop(previousLoop);
    m_loops.remove(node);
    return false;
}

bool Compiler::visit(AST::LabelledStatement *node)
{
    Loop *loop = findLoop(node->label);
    if (loop != 0) {
        QString str = m_eng->toString(node->label);
        m_compilationUnit.setError(QString::fromUtf8("duplicate label `%1'").arg(str),
                                   node->startLine);
        return false;
    }

    loop = &m_loops[node->statement];
    loop->name = node->label;
    node->statement->accept(this);
    if (m_loops.contains(node->statement)) {
        loop->breakLabel.offset = nextInstructionOffset();
        foreach (int index, loop->breakLabel.uses) {
            patchInstruction(index, loop->breakLabel.offset - index);
        }
        m_loops.remove(node->statement);
    }
    return false;
}

bool Compiler::visit(AST::ExpressionStatement *node)
{
    if (node->expression)
        iLine(node->expression);
    return true;
}

void Compiler::endVisit(AST::ExpressionStatement *)
{
    if (topLevelCompiler())
        iSync();
    else
        iPop();
}

void Compiler::endVisit(AST::UnaryPlusExpression *)
{
    iUnaryPlus();
}

void Compiler::endVisit(AST::UnaryMinusExpression *)
{
    iUnaryMinus();
}

bool Compiler::visit(AST::ContinueStatement *node)
{
    iLine(node);
    return true;
}

void Compiler::endVisit(AST::ContinueStatement *node)
{
    int offset = nextInstructionOffset();
    iBranch(0);

    Loop *loop = findLoop(node->label);
    if (!loop || !m_iterationStatement) {
        m_compilationUnit.setError(QString::fromUtf8("label not found"),
                                   node->startLine);
        return;
    }

    loop->continueLabel.uses.append(offset);
}

bool Compiler::visit(AST::BreakStatement *node)
{
    iLine(node);
    return true;
}

void Compiler::endVisit(AST::BreakStatement *node)
{
    Loop *loop = findLoop(node->label);
    if (! loop) {
        m_compilationUnit.setError(QString::fromUtf8("label not found"),
                                   node->startLine);
        return;
    }

    if (m_generateLeaveWithOnBreak)
        iLeaveWith();
    int offset = nextInstructionOffset();
    iBranch(0);
    loop->breakLabel.uses.append(offset);
}

void Compiler::endVisit(AST::EmptyStatement *node)
{
    iLine(node);
}

bool Compiler::visit(AST::DebuggerStatement *node)
{
    iLine(node);
    iDebugger();
    return false;
}

void Compiler::patchInstruction(int index, int offset)
{
    QScriptInstruction &i = m_instructions[index];

    switch (i.op) {
        case QScriptInstruction::OP_Branch:
        case QScriptInstruction::OP_BranchFalse:
        case QScriptInstruction::OP_BranchTrue:
            m_eng->newInteger(&i.operand[0], offset);
            break;

        default:
            Q_ASSERT_X(0, "Compiler::patchInstruction()", "expected a branch instruction");
            break;
    }
}

bool Compiler::visit(AST::WithStatement *node)
{
    iLine(node);
    node->expression->accept(this);
    iEnterWith();
    bool was = withStatement(true);
    bool was2 = generateLeaveOnBreak(true);
    node->statement->accept(this);
    generateLeaveOnBreak(was2);
    withStatement(was);
    iLeaveWith();
    return false;
}

bool Compiler::visit(AST::ArrayLiteral *node)
{
    iNewArray();

    int length = 0;

    for (AST::ElementList *it = node->elements; it != 0; it = it->next) {
        for (AST::Elision *eit = it->elision; eit != 0; eit = eit->next) {
            iDuplicate();
            iLoadNumber(length);
            iMakeReference();
            iLoadUndefined();
            iAssign();
            iPop();
            ++length;
        }

        if (it->expression) {
            iDuplicate();
            iLoadNumber(length);
            iMakeReference();
            it->expression->accept(this);
            iAssign();
            iPop();
            ++length;
        }
    }

    for (AST::Elision *eit = node->elision; eit != 0; eit = eit->next) {
        iDuplicate();
        iLoadNumber(length);
        iMakeReference();
        iLoadUndefined();
        iAssign();
        iPop();
        ++length;
    }

    return false;
}

void Compiler::iLoadUndefined()
{
    pushInstruction(QScriptInstruction::OP_LoadUndefined);
}

void Compiler::iLoadThis()
{
    pushInstruction(QScriptInstruction::OP_LoadThis);
}

void Compiler::iLoadActivation()
{
    pushInstruction(QScriptInstruction::OP_LoadActivation);
}

void Compiler::iLoadNull()
{
    pushInstruction(QScriptInstruction::OP_LoadNull);
}

void Compiler::iLoadNumber(double number)
{
    QScriptValueImpl arg0(number);
    pushInstruction(QScriptInstruction::OP_LoadNumber, arg0);
}

void Compiler::iLoadString(QScriptNameIdImpl *id)
{
    QScriptValueImpl arg0;
    id->persistent = true;
    m_eng->newNameId(&arg0, id);
    pushInstruction(QScriptInstruction::OP_LoadString, arg0);
}

void Compiler::iDuplicate()
{
    pushInstruction(QScriptInstruction::OP_Duplicate);
}

void Compiler::iSwap()
{
    pushInstruction(QScriptInstruction::OP_Swap);
}

void Compiler::iResolve(QScriptNameIdImpl *id)
{
    QScriptValueImpl arg0;
    id->persistent = true;
    m_eng->newNameId(&arg0, id);
    pushInstruction(QScriptInstruction::OP_Resolve, arg0);
}

void Compiler::iPutField()
{
    pushInstruction(QScriptInstruction::OP_PutField);
}

void Compiler::iCall(int argc)
{
    QScriptValueImpl arg0;
    m_eng->newInteger(&arg0, argc);
    pushInstruction(QScriptInstruction::OP_Call, arg0);
}

void Compiler::iNew(int argc)
{
    QScriptValueImpl arg0;
    m_eng->newInteger(&arg0, argc);
    pushInstruction(QScriptInstruction::OP_New, arg0);
}

void Compiler::iFetchField()
{
    pushInstruction(QScriptInstruction::OP_FetchField);
}

void Compiler::iLazyArguments()
{
    pushInstruction(QScriptInstruction::OP_LazyArguments);
}

void Compiler::iRet()
{
    pushInstruction(QScriptInstruction::OP_Ret);
}

void Compiler::iDeclareLocal(QScriptNameIdImpl *id, bool readOnly)
{
    QScriptValueImpl arg0;
    id->persistent = true;
    m_eng->newNameId(&arg0, id);
    QScriptValueImpl arg1;
    m_eng->newInteger(&arg1, readOnly);
    pushInstruction(QScriptInstruction::OP_DeclareLocal, arg0, arg1);
}

void Compiler::iAssign()
{
    pushInstruction(QScriptInstruction::OP_Assign);
}

void Compiler::iBitAnd()
{
    pushInstruction(QScriptInstruction::OP_BitAnd);
}

void Compiler::iBitOr()
{
    pushInstruction(QScriptInstruction::OP_BitOr);
}

void Compiler::iBitXor()
{
    pushInstruction(QScriptInstruction::OP_BitXor);
}

void Compiler::iLeftShift()
{
    pushInstruction(QScriptInstruction::OP_LeftShift);
}

void Compiler::iMod()
{
    pushInstruction(QScriptInstruction::OP_Mod);
}

void Compiler::iRightShift()
{
    pushInstruction(QScriptInstruction::OP_RightShift);
}

void Compiler::iURightShift()
{
    pushInstruction(QScriptInstruction::OP_URightShift);
}

void Compiler::iAdd()
{
    pushInstruction(QScriptInstruction::OP_Add);
}

void Compiler::iDiv()
{
    pushInstruction(QScriptInstruction::OP_Div);
}

void Compiler::iEqual()
{
    pushInstruction(QScriptInstruction::OP_Equal);
}

void Compiler::iGreatOrEqual()
{
    pushInstruction(QScriptInstruction::OP_GreatOrEqual);
}

void Compiler::iGreatThan()
{
    pushInstruction(QScriptInstruction::OP_GreatThan);
}

void Compiler::iLessOrEqual()
{
    pushInstruction(QScriptInstruction::OP_LessOrEqual);
}

void Compiler::iLessThan()
{
    pushInstruction(QScriptInstruction::OP_LessThan);
}

void Compiler::iMul()
{
    pushInstruction(QScriptInstruction::OP_Mul);
}

void Compiler::iNotEqual()
{
    pushInstruction(QScriptInstruction::OP_NotEqual);
}

void Compiler::iSub()
{
    pushInstruction(QScriptInstruction::OP_Sub);
}

void Compiler::iStrictEqual()
{
    pushInstruction(QScriptInstruction::OP_StrictEqual);
}

void Compiler::iStrictNotEqual()
{
    pushInstruction(QScriptInstruction::OP_StrictNotEqual);
}

void Compiler::iBranch(int index)
{
    QScriptValueImpl arg0;
    m_eng->newInteger(&arg0, index);
    pushInstruction(QScriptInstruction::OP_Branch, arg0);
}

void Compiler::iBranchFalse(int index)
{
    QScriptValueImpl arg0;
    m_eng->newInteger(&arg0, index);
    pushInstruction(QScriptInstruction::OP_BranchFalse, arg0);
}

void Compiler::iBranchTrue(int index)
{
    QScriptValueImpl arg0;
    m_eng->newInteger(&arg0, index);
    pushInstruction(QScriptInstruction::OP_BranchTrue, arg0);
}

void Compiler::iNewClosure(AST::FunctionExpression *expr)
{
    QScriptValueImpl arg0;
    m_eng->newPointer(&arg0, expr);

    pushInstruction(QScriptInstruction::OP_NewClosure, arg0);
}

void Compiler::iIncr()
{
    pushInstruction(QScriptInstruction::OP_Incr);
}

void Compiler::iDecr()
{
    pushInstruction(QScriptInstruction::OP_Decr);
}

void Compiler::iPop()
{
    pushInstruction(QScriptInstruction::OP_Pop);
}

void Compiler::iFetch(QScriptNameIdImpl *id)
{
    if (m_generateFastArgumentLookup) {
        int index = m_formals.indexOf(id);

        if (index != -1) {
            QScriptValueImpl arg0;
            m_eng->newInteger(&arg0, index);
            pushInstruction(QScriptInstruction::OP_Receive, arg0);
            return;
        }
    }

    QScriptValueImpl arg0;
    id->persistent = true;
    m_eng->newNameId(&arg0, id);
    pushInstruction(QScriptInstruction::OP_Fetch, arg0);
}

void Compiler::iLoadTrue()
{
    pushInstruction(QScriptInstruction::OP_LoadTrue);
}

void Compiler::iLoadFalse()
{
    pushInstruction(QScriptInstruction::OP_LoadFalse);
}

void Compiler::iUnaryMinus()
{
    pushInstruction(QScriptInstruction::OP_UnaryMinus);
}

void Compiler::iUnaryPlus()
{
    pushInstruction(QScriptInstruction::OP_UnaryPlus);
}

void Compiler::iPostIncr()
{
    pushInstruction(QScriptInstruction::OP_PostIncr);
}

void Compiler::iPostDecr()
{
    pushInstruction(QScriptInstruction::OP_PostDecr);
}

void Compiler::iNewArray()
{
    pushInstruction(QScriptInstruction::OP_NewArray);
}

void Compiler::iNewObject()
{
    pushInstruction(QScriptInstruction::OP_NewObject);
}

void Compiler::iTypeOf()
{
    pushInstruction(QScriptInstruction::OP_TypeOf);
}

void Compiler::iDelete()
{
    pushInstruction(QScriptInstruction::OP_Delete);
}

void Compiler::iInstanceOf()
{
    pushInstruction(QScriptInstruction::OP_InstanceOf);
}

void Compiler::iInplaceAnd()
{
    pushInstruction(QScriptInstruction::OP_InplaceAnd);
}

void Compiler::iInplaceSub()
{
    pushInstruction(QScriptInstruction::OP_InplaceSub);
}

void Compiler::iInplaceDiv()
{
    pushInstruction(QScriptInstruction::OP_InplaceDiv);
}

void Compiler::iInplaceAdd()
{
    pushInstruction(QScriptInstruction::OP_InplaceAdd);
}

void Compiler::iInplaceLeftShift()
{
    pushInstruction(QScriptInstruction::OP_InplaceLeftShift);
}

void Compiler::iInplaceMod()
{
    pushInstruction(QScriptInstruction::OP_InplaceMod);
}

void Compiler::iInplaceMul()
{
    pushInstruction(QScriptInstruction::OP_InplaceMul);
}

void Compiler::iInplaceOr()
{
    pushInstruction(QScriptInstruction::OP_InplaceOr);
}

void Compiler::iInplaceRightShift()
{
    pushInstruction(QScriptInstruction::OP_InplaceRightShift);
}

void Compiler::iInplaceURightShift()
{
    pushInstruction(QScriptInstruction::OP_InplaceURightShift);
}

void Compiler::iInplaceXor()
{
    pushInstruction(QScriptInstruction::OP_InplaceXor);
}

void Compiler::iThrow()
{
    pushInstruction(QScriptInstruction::OP_Throw);
}

void Compiler::iLine(AST::Node *node)
{
    if (! node)
        return;

    QScriptValueImpl arg0;
    m_eng->newInteger(&arg0, node->startLine);

    QScriptValueImpl arg1;
    m_eng->newInteger(&arg1, node->startColumn);

    pushInstruction(QScriptInstruction::OP_Line, arg0, arg1);
}

void Compiler::iBitNot()
{
    pushInstruction(QScriptInstruction::OP_BitNot);
}

void Compiler::iNot()
{
    pushInstruction(QScriptInstruction::OP_Not);
}

void Compiler::iNewRegExp(QScriptNameIdImpl *pattern)
{
    QScriptValueImpl arg0;
    pattern->persistent = true;
    m_eng->newNameId(&arg0, pattern);
    pushInstruction(QScriptInstruction::OP_NewRegExp, arg0);
}

void Compiler::iNewRegExp(QScriptNameIdImpl *pattern, int flags)
{
    QScriptValueImpl arg0;
    pattern->persistent = true;
    m_eng->newNameId(&arg0, pattern);

    QScriptValueImpl arg1;
    m_eng->newInteger(&arg1, flags);

    pushInstruction(QScriptInstruction::OP_NewRegExp, arg0, arg1);
}

void Compiler::iNewEnumeration()
{
    pushInstruction(QScriptInstruction::OP_NewEnumeration);
}

void Compiler::iToFirstElement()
{
    pushInstruction(QScriptInstruction::OP_ToFirstElement);
}

void Compiler::iHasNextElement()
{
    pushInstruction(QScriptInstruction::OP_HasNextElement);
}

void Compiler::iNextElement()
{
    pushInstruction(QScriptInstruction::OP_NextElement);
}

void Compiler::iEnterWith()
{
    pushInstruction(QScriptInstruction::OP_EnterWith);
}

void Compiler::iLeaveWith()
{
    pushInstruction(QScriptInstruction::OP_LeaveWith);
}

void Compiler::iBeginCatch(QScriptNameIdImpl *id)
{
    QScriptValueImpl arg0;
    id->persistent = true;
    m_eng->newNameId(&arg0, id);
    pushInstruction(QScriptInstruction::OP_BeginCatch, arg0);
}

void Compiler::iEndCatch()
{
    pushInstruction(QScriptInstruction::OP_EndCatch);
}

void Compiler::iSync()
{
    pushInstruction(QScriptInstruction::OP_Sync);
}

void Compiler::iHalt()
{
    pushInstruction(QScriptInstruction::OP_Halt);
}

void Compiler::iMakeReference()
{
    pushInstruction(QScriptInstruction::OP_MakeReference);
}

void Compiler::iIn()
{
    pushInstruction(QScriptInstruction::OP_In);
}

void Compiler::iNop()
{
    pushInstruction(QScriptInstruction::OP_Nop);
}

void Compiler::iNewString(QScriptNameIdImpl *id)
{
    QScriptValueImpl arg0;
    id->persistent = true;
    m_eng->newNameId(&arg0, id);
    pushInstruction(QScriptInstruction::OP_NewString, arg0);
}

void Compiler::iDebugger()
{
    pushInstruction(QScriptInstruction::OP_Debugger);
}

Compiler::Loop *Compiler::findLoop(QScriptNameIdImpl *name)
{
    if (! name)
        return m_activeLoop;

    QMap<AST::Statement*, Loop>::iterator it = m_loops.begin();

    for (; it != m_loops.end(); ++it) {
        Loop &loop = *it;

        if (loop.name == name)
            return &loop;
    }

    return 0;
}


} // namespace QScript

QT_END_NAMESPACE

#endif // QT_NO_SCRIPT
