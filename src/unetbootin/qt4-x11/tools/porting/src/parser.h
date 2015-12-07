/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
** Copyright (C) 2001-2004 Roberto Raggi
**
** This file is part of the qt3to4 porting application of the Qt Toolkit.
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

#ifndef PARSER_H
#define PARSER_H

#include "ast.h"
#include "tokenstreamadapter.h"

#include <QString>
#include <QStringList>
#include <QList>

QT_BEGIN_NAMESPACE

class FileSymbol;
class TokenStream;
class Error;

class Parser
{
public:
    Parser();
    ~Parser();

//    TranslationUnitAST *parse(FileSymbol *file, pool *p);
    TranslationUnitAST *parse(TokenStreamAdapter::TokenStream *tokenStream, pool *p);
    TranslationUnitAST *parse(TokenStreamAdapter::TokenStream *tokenStream, pool *p, int targetMaxASTnodes, bool &done);
private:
    bool reportError(const Error& err);
    /** @todo remove*/ bool reportError(const QString& msg);
    /** @todo remove*/ void syntaxError();

public /*rules*/ :

    bool parseTranslationUnit(TranslationUnitAST *&node);

    bool parseDeclaration(DeclarationAST *&node);
    bool parseBlockDeclaration(DeclarationAST *&node);
    bool parseLinkageSpecification(DeclarationAST *&node);
    bool parseLinkageBody(LinkageBodyAST *&node);
    bool parseNamespace(DeclarationAST *&node);
    bool parseNamespaceAliasDefinition(DeclarationAST *&node);
    bool parseUsing(DeclarationAST *&node);
    bool parseUsingDirective(DeclarationAST *&node);
    bool parseTypedef(DeclarationAST *&node);
    bool parseAsmDefinition(DeclarationAST *&node);
    bool parseTemplateDeclaration(DeclarationAST *&node);
    bool parseDeclarationInternal(DeclarationAST *&node);

    bool parseStringLiteral(AST *&node);
    bool parseUnqualifiedName(ClassOrNamespaceNameAST *&node, bool parseTemplateId = true);
    bool parseName(NameAST *&node, bool parseTemplateId = true);
    bool parseOperatorFunctionId(AST *&node);
    bool parseTemplateArgumentList(TemplateArgumentListAST *&node, bool reportError = true);
    bool parseOperator(AST *&node);
    bool parseCvQualify(AST *&node);
    bool parseSimpleTypeSpecifier(TypeSpecifierAST *&node, bool onlyIntegral = false);
    bool parsePtrOperator(AST *&node);
    bool parseTemplateArgument(AST *&node);
    bool parseTypeSpecifier(TypeSpecifierAST *&node);
    bool parseTypeSpecifierOrClassSpec(TypeSpecifierAST *&node);
    bool parseDeclarator(DeclaratorAST *&node);
    bool parseTemplateParameterList(TemplateParameterListAST *&node);
    bool parseTemplateParameter(TemplateParameterAST *&node);
    bool parseStorageClassSpecifier(AST *&node);
    bool parseFunctionSpecifier(AST *&node);
    bool parseInitDeclaratorList(InitDeclaratorListAST *&node);
    bool parseInitDeclarator(InitDeclaratorAST *&node);
    bool parseParameterDeclarationClause(ParameterDeclarationClauseAST *&node);
    bool parseCtorInitializer(AST *&node);
    bool parsePtrToMember(AST *&node);
    bool parseEnumSpecifier(TypeSpecifierAST *&node);
    bool parseClassSpecifier(TypeSpecifierAST *&node);
    bool parseWinDeclSpec(AST *&node);
    bool parseElaboratedTypeSpecifier(TypeSpecifierAST *&node);
    bool parseDeclaratorId(NameAST *&node);
    bool parseExceptionSpecification(AST *&node);
    bool parseEnumerator(EnumeratorAST *&node);
    bool parseTypeParameter(TypeParameterAST *&node);
    bool parseParameterDeclaration(ParameterDeclarationAST *&node);
    bool parseTypeId(TypeIdAST *&node);
    bool parseAbstractDeclarator(DeclaratorAST *&node);
    bool parseParameterDeclarationList(ParameterDeclarationListAST *&node);
    bool parseMemberSpecification(DeclarationAST *&node);
    bool parseAccessSpecifier(AST *&node);
    bool parseTypeIdList(AST *&node);
    bool parseMemInitializerList(AST *&node);
    bool parseMemInitializer(AST *&node);
    bool parseInitializer(AST *&node);
    bool parseBaseClause(BaseClauseAST *&node);
    bool parseBaseSpecifier(BaseSpecifierAST *&node);
    bool parseInitializerClause(AST *&node);
    bool parseMemInitializerId(NameAST *&node);
    bool parseFunctionBody(StatementListAST *&node);

    // expression
    bool skipExpression(AbstractExpressionAST *&node);
    bool skipCommaExpression(AbstractExpressionAST *&node);
    bool skipExpressionStatement(StatementAST *&node);

    bool parseExpression(AbstractExpressionAST *&node);
    bool parsePrimaryExpression(AbstractExpressionAST *&node);
    bool parsePostfixExpression(AbstractExpressionAST *&node);
    bool parsePostfixExpressionInternal(AbstractExpressionAST *expr, AbstractExpressionAST *&node);
    bool parseUnaryExpression(AbstractExpressionAST *&node);
    bool parseNewExpression(AbstractExpressionAST *&node);
    bool parseNewTypeId(AbstractExpressionAST *&node);
    bool parseNewDeclarator(AbstractExpressionAST *&node);
    bool parseNewInitializer(AbstractExpressionAST *&node);
    bool parseDeleteExpression(AbstractExpressionAST *&node);
    bool parseCastExpression(AbstractExpressionAST *&node);
    bool parsePmExpression(AbstractExpressionAST *&node);
    bool parseMultiplicativeExpression(AbstractExpressionAST *&node);
    bool parseAdditiveExpression(AbstractExpressionAST *&node);
    bool parseShiftExpression(AbstractExpressionAST *&node);
    bool parseRelationalExpression(AbstractExpressionAST *&node, bool templArgs = false);
    bool parseEqualityExpression(AbstractExpressionAST *&node, bool templArgs = false);
    bool parseAndExpression(AbstractExpressionAST *&node, bool templArgs = false);
    bool parseExclusiveOrExpression(AbstractExpressionAST *&node, bool templArgs = false);
    bool parseInclusiveOrExpression(AbstractExpressionAST *&node, bool templArgs = false);
    bool parseLogicalAndExpression(AbstractExpressionAST *&node, bool templArgs = false);
    bool parseLogicalOrExpression(AbstractExpressionAST *&node, bool templArgs = false);
    bool parseConditionalExpression(AbstractExpressionAST *&node);
    bool parseAssignmentExpression(AbstractExpressionAST *&node);
    bool parseConstantExpression(AbstractExpressionAST *&node);
    bool parseCommaExpression(AbstractExpressionAST *&node);
    bool parseThrowExpression(AbstractExpressionAST *&node);

    // statement
    bool parseCondition(ConditionAST *&node);
    bool parseStatement(StatementAST *&node);
    bool parseWhileStatement(StatementAST *&node);
    bool parseDoStatement(StatementAST *&node);
    bool parseForStatement(StatementAST *&node);
    bool parseCompoundStatement(StatementAST *&node);
    bool parseForInitStatement(StatementAST *&node);
    bool parseIfStatement(StatementAST *&node);
    bool parseSwitchStatement(StatementAST *&node);
    bool parseLabeledStatement(StatementAST *&node);
    bool parseDeclarationStatement(StatementAST *&node);
    bool parseTryBlockStatement(StatementAST *&node);

    // objective c
    bool parseObjcDef(DeclarationAST *&node);
    bool parseObjcClassDef(DeclarationAST *&node);
    bool parseObjcClassDecl(DeclarationAST *&node);
    bool parseObjcProtocolDecl(DeclarationAST *&node);
    bool parseObjcAliasDecl(DeclarationAST *&node);
    bool parseObjcProtocolDef(DeclarationAST *&node);
    bool parseObjcMethodDef(DeclarationAST *&node);

    bool parseIvarDeclList(AST *&node);
    bool parseIvarDecls(AST *&node);
    bool parseIvarDecl(AST *&node);
    bool parseIvars(AST *&node);
    bool parseIvarDeclarator(AST *&node);
    bool parseMethodDecl(AST *&node);
    bool parseUnarySelector(AST *&node);
    bool parseKeywordSelector(AST *&node);
    bool parseSelector(AST *&node);
    bool parseKeywordDecl(AST *&node);
    bool parseReceiver(AST *&node);
    bool parseObjcMessageExpr(AST *&node);
    bool parseMessageArgs(AST *&node);
    bool parseKeywordExpr(AST *&node);
    bool parseKeywordArgList(AST *&node);
    bool parseKeywordArg(AST *&node);
    bool parseReservedWord(AST *&node);
    bool parseMyParms(AST *&node);
    bool parseMyParm(AST *&node);
    bool parseOptParmList(AST *&node);
    bool parseObjcSelectorExpr(AST *&node);
    bool parseSelectorArg(AST *&node);
    bool parseKeywordNameList(AST *&node);
    bool parseKeywordName(AST *&node);
    bool parseObjcEncodeExpr(AST *&node);
    bool parseObjcString(AST *&node);
    bool parseProtocolRefs(AST *&node);
    bool parseIdentifierList(AST *&node);
    bool parseIdentifierColon(AST *&node);
    bool parseObjcProtocolExpr(AST *&node);
    bool parseObjcOpenBracketExpr(AST *&node);
    bool parseObjcCloseBracket(AST *&node);

    bool skipUntil(int token);
    bool skipUntilDeclaration();
    bool skipUntilStatement();
    bool skip(int l, int r);

    void advance();

private:
    int m_problems;
    int m_maxProblems;
    bool objcp;
    TokenStreamAdapter::TokenStream *tokenStream;
    pool *m_pool;
    FileSymbol *m_file;

private:
    Parser(const Parser& source);
    void operator = (const Parser& source);
};

QT_END_NAMESPACE

#endif
