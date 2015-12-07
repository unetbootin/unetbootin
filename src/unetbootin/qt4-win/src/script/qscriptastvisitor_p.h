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

#ifndef QSCRIPTASTVISITOR_P_H
#define QSCRIPTASTVISITOR_P_H

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

#include "qscriptastfwd_p.h"

QT_BEGIN_NAMESPACE

namespace QScript { namespace AST {

class Visitor
{
public:
    Visitor();
    virtual ~Visitor();

    virtual bool preVisit(Node *) { return true; }
    virtual void postVisit(Node *) {}

    virtual bool visit(ThisExpression *) { return true; }
    virtual void endVisit(ThisExpression *) {}

    virtual bool visit(IdentifierExpression *) { return true; }
    virtual void endVisit(IdentifierExpression *) {}

    virtual bool visit(NullExpression *) { return true; }
    virtual void endVisit(NullExpression *) {}

    virtual bool visit(TrueLiteral *) { return true; }
    virtual void endVisit(TrueLiteral *) {}

    virtual bool visit(FalseLiteral *) { return true; }
    virtual void endVisit(FalseLiteral *) {}

    virtual bool visit(StringLiteral *) { return true; }
    virtual void endVisit(StringLiteral *) {}

    virtual bool visit(NumericLiteral *) { return true; }
    virtual void endVisit(NumericLiteral *) {}

    virtual bool visit(RegExpLiteral *) { return true; }
    virtual void endVisit(RegExpLiteral *) {}

    virtual bool visit(ArrayLiteral *) { return true; }
    virtual void endVisit(ArrayLiteral *) {}

    virtual bool visit(ObjectLiteral *) { return true; }
    virtual void endVisit(ObjectLiteral *) {}

    virtual bool visit(ElementList *) { return true; }
    virtual void endVisit(ElementList *) {}

    virtual bool visit(Elision *) { return true; }
    virtual void endVisit(Elision *) {}

    virtual bool visit(PropertyNameAndValueList *) { return true; }
    virtual void endVisit(PropertyNameAndValueList *) {}

    virtual bool visit(IdentifierPropertyName *) { return true; }
    virtual void endVisit(IdentifierPropertyName *) {}

    virtual bool visit(StringLiteralPropertyName *) { return true; }
    virtual void endVisit(StringLiteralPropertyName *) {}

    virtual bool visit(NumericLiteralPropertyName *) { return true; }
    virtual void endVisit(NumericLiteralPropertyName *) {}

    virtual bool visit(ArrayMemberExpression *) { return true; }
    virtual void endVisit(ArrayMemberExpression *) {}

    virtual bool visit(FieldMemberExpression *) { return true; }
    virtual void endVisit(FieldMemberExpression *) {}

    virtual bool visit(NewMemberExpression *) { return true; }
    virtual void endVisit(NewMemberExpression *) {}

    virtual bool visit(NewExpression *) { return true; }
    virtual void endVisit(NewExpression *) {}

    virtual bool visit(CallExpression *) { return true; }
    virtual void endVisit(CallExpression *) {}

    virtual bool visit(ArgumentList *) { return true; }
    virtual void endVisit(ArgumentList *) {}

    virtual bool visit(PostIncrementExpression *) { return true; }
    virtual void endVisit(PostIncrementExpression *) {}

    virtual bool visit(PostDecrementExpression *) { return true; }
    virtual void endVisit(PostDecrementExpression *) {}

    virtual bool visit(DeleteExpression *) { return true; }
    virtual void endVisit(DeleteExpression *) {}

    virtual bool visit(VoidExpression *) { return true; }
    virtual void endVisit(VoidExpression *) {}

    virtual bool visit(TypeOfExpression *) { return true; }
    virtual void endVisit(TypeOfExpression *) {}

    virtual bool visit(PreIncrementExpression *) { return true; }
    virtual void endVisit(PreIncrementExpression *) {}

    virtual bool visit(PreDecrementExpression *) { return true; }
    virtual void endVisit(PreDecrementExpression *) {}

    virtual bool visit(UnaryPlusExpression *) { return true; }
    virtual void endVisit(UnaryPlusExpression *) {}

    virtual bool visit(UnaryMinusExpression *) { return true; }
    virtual void endVisit(UnaryMinusExpression *) {}

    virtual bool visit(TildeExpression *) { return true; }
    virtual void endVisit(TildeExpression *) {}

    virtual bool visit(NotExpression *) { return true; }
    virtual void endVisit(NotExpression *) {}

    virtual bool visit(BinaryExpression *) { return true; }
    virtual void endVisit(BinaryExpression *) {}

    virtual bool visit(ConditionalExpression *) { return true; }
    virtual void endVisit(ConditionalExpression *) {}

    virtual bool visit(Expression *) { return true; }
    virtual void endVisit(Expression *) {}

    virtual bool visit(Block *) { return true; }
    virtual void endVisit(Block *) {}

    virtual bool visit(StatementList *) { return true; }
    virtual void endVisit(StatementList *) {}

    virtual bool visit(VariableStatement *) { return true; }
    virtual void endVisit(VariableStatement *) {}

    virtual bool visit(VariableDeclarationList *) { return true; }
    virtual void endVisit(VariableDeclarationList *) {}

    virtual bool visit(VariableDeclaration *) { return true; }
    virtual void endVisit(VariableDeclaration *) {}

    virtual bool visit(EmptyStatement *) { return true; }
    virtual void endVisit(EmptyStatement *) {}

    virtual bool visit(ExpressionStatement *) { return true; }
    virtual void endVisit(ExpressionStatement *) {}

    virtual bool visit(IfStatement *) { return true; }
    virtual void endVisit(IfStatement *) {}

    virtual bool visit(DoWhileStatement *) { return true; }
    virtual void endVisit(DoWhileStatement *) {}

    virtual bool visit(WhileStatement *) { return true; }
    virtual void endVisit(WhileStatement *) {}

    virtual bool visit(ForStatement *) { return true; }
    virtual void endVisit(ForStatement *) {}

    virtual bool visit(LocalForStatement *) { return true; }
    virtual void endVisit(LocalForStatement *) {}

    virtual bool visit(ForEachStatement *) { return true; }
    virtual void endVisit(ForEachStatement *) {}

    virtual bool visit(LocalForEachStatement *) { return true; }
    virtual void endVisit(LocalForEachStatement *) {}

    virtual bool visit(ContinueStatement *) { return true; }
    virtual void endVisit(ContinueStatement *) {}

    virtual bool visit(BreakStatement *) { return true; }
    virtual void endVisit(BreakStatement *) {}

    virtual bool visit(ReturnStatement *) { return true; }
    virtual void endVisit(ReturnStatement *) {}

    virtual bool visit(WithStatement *) { return true; }
    virtual void endVisit(WithStatement *) {}

    virtual bool visit(SwitchStatement *) { return true; }
    virtual void endVisit(SwitchStatement *) {}

    virtual bool visit(CaseBlock *) { return true; }
    virtual void endVisit(CaseBlock *) {}

    virtual bool visit(CaseClauses *) { return true; }
    virtual void endVisit(CaseClauses *) {}

    virtual bool visit(CaseClause *) { return true; }
    virtual void endVisit(CaseClause *) {}

    virtual bool visit(DefaultClause *) { return true; }
    virtual void endVisit(DefaultClause *) {}

    virtual bool visit(LabelledStatement *) { return true; }
    virtual void endVisit(LabelledStatement *) {}

    virtual bool visit(ThrowStatement *) { return true; }
    virtual void endVisit(ThrowStatement *) {}

    virtual bool visit(TryStatement *) { return true; }
    virtual void endVisit(TryStatement *) {}

    virtual bool visit(Catch *) { return true; }
    virtual void endVisit(Catch *) {}

    virtual bool visit(Finally *) { return true; }
    virtual void endVisit(Finally *) {}

    virtual bool visit(FunctionDeclaration *) { return true; }
    virtual void endVisit(FunctionDeclaration *) {}

    virtual bool visit(FunctionExpression *) { return true; }
    virtual void endVisit(FunctionExpression *) {}

    virtual bool visit(FormalParameterList *) { return true; }
    virtual void endVisit(FormalParameterList *) {}

    virtual bool visit(FunctionBody *) { return true; }
    virtual void endVisit(FunctionBody *) {}

    virtual bool visit(Program *) { return true; }
    virtual void endVisit(Program *) {}

    virtual bool visit(SourceElements *) { return true; }
    virtual void endVisit(SourceElements *) {}

    virtual bool visit(FunctionSourceElement *) { return true; }
    virtual void endVisit(FunctionSourceElement *) {}

    virtual bool visit(StatementSourceElement *) { return true; }
    virtual void endVisit(StatementSourceElement *) {}
};

} } // namespace AST

QT_END_NAMESPACE

#endif // QSCRIPTASTVISITOR_P_H
