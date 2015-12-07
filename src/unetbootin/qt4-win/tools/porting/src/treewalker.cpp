/****************************************************************************
**
** Copyright (C) 2004-2008 Trolltech ASA. All rights reserved.
** Copyright (C) 2001-2004 Roberto Raggi
**
** This file is part of the qt3to4 porting application of the Qt Toolkit.
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

#include "treewalker.h"

QT_BEGIN_NAMESPACE

/*
template <class T>
inline void parseAll(TreeWalker *w, const List<T *> *l)
{
    if (!l)
        return;

    foreach(T *e, *l)
        w->parseNode(e);
}
*/

//Workaround for ICE on MSVC, use macro instead of template.
#define PARSE_ALL(ListType, ListValueType) \
inline void parseAll(TreeWalker *w, const ListType *l) \
{ \
    if (!l) \
        return; \
    foreach(ListValueType *e, *l) \
        w->parseNode(e); \
} \

PARSE_ALL(List<AST *>, AST)
PARSE_ALL(List<ClassOrNamespaceNameAST *>, ClassOrNamespaceNameAST)
PARSE_ALL(List<BaseSpecifierAST *>, BaseSpecifierAST)
PARSE_ALL(List<DeclarationAST *>, DeclarationAST)
PARSE_ALL(List<EnumeratorAST *>, EnumeratorAST)
PARSE_ALL(List<ParameterDeclarationAST *>, ParameterDeclarationAST)
PARSE_ALL(List<InitDeclaratorAST *>, InitDeclaratorAST)
PARSE_ALL(List<TemplateParameterAST *>, TemplateParameterAST)
PARSE_ALL(List<StatementAST *>, StatementAST)


void TreeWalker::parseTemplateArgumentList(TemplateArgumentListAST *node)
{
    List<AST *> *arglist = node->argumentList();
	parseAll(this, arglist);
}

void TreeWalker::parseClassOrNamespaceName(ClassOrNamespaceNameAST *node)
{
    parseNode(node->name());
    parseNode(node->templateArgumentList());
}

void TreeWalker::parseName(NameAST *node)
{
    parseAll(this, node->classOrNamespaceNameList());
    parseNode(node->unqualifiedName());
}

void TreeWalker::parseTypeParameter(TypeParameterAST *node)
{
    parseNode(node->templateParameterList());
    parseNode(node->name());
    parseNode(node->typeId());
}

void TreeWalker::parseDeclaration(DeclarationAST *node)
{
    switch (node->nodeType()) {
    case NodeType_AccessDeclaration:
        parseAccessDeclaration(static_cast<AccessDeclarationAST*>(node));
        break;
    case NodeType_LinkageSpecification:
        parseLinkageSpecification(static_cast<LinkageSpecificationAST*>(node));
        break;
    case NodeType_Namespace:
        parseNamespace(static_cast<NamespaceAST*>(node));
        break;
    case NodeType_NamespaceAlias:
        parseNamespaceAlias(static_cast<NamespaceAliasAST*>(node));
        break;
    case NodeType_Using:
        parseUsing(static_cast<UsingAST*>(node));
        break;
    case NodeType_UsingDirective:
        parseUsingDirective(static_cast<UsingDirectiveAST*>(node));
        break;
    case NodeType_Typedef:
        parseTypedef(static_cast<TypedefAST*>(node));
        break;
    case NodeType_TemplateDeclaration:
        parseTemplateDeclaration(static_cast<TemplateDeclarationAST*>(node));
        break;
    case NodeType_SimpleDeclaration:
        parseSimpleDeclaration(static_cast<SimpleDeclarationAST*>(node));
        break;
    case NodeType_FunctionDefinition:
        parseFunctionDefinition(static_cast<FunctionDefinitionAST*>(node));
        break;
    default:
        break;
    }
}

void TreeWalker::parseAccessDeclaration(AccessDeclarationAST *node)
{
    parseAll(this, node->accessList());
}

void TreeWalker::parseTypeSpecifier(TypeSpecifierAST *node)
{
    parseNode(node->name());
    parseNode(node->cvQualify());
    parseNode(node->cv2Qualify());

    switch (node->nodeType()) {
    case NodeType_ClassSpecifier:
        parseClassSpecifier(static_cast<ClassSpecifierAST*>(node));
        break;
    case NodeType_EnumSpecifier:
        parseEnumSpecifier(static_cast<EnumSpecifierAST*>(node));
        break;
    case NodeType_ElaboratedTypeSpecifier:
        parseElaboratedTypeSpecifier(static_cast<ElaboratedTypeSpecifierAST*>(node));
        break;
    default:
        break;
    }
}

void TreeWalker::parseBaseSpecifier(BaseSpecifierAST *node)
{
    parseNode(node->isVirtual());
    parseNode(node->access());
    parseNode(node->name());
}

void TreeWalker::parseBaseClause(BaseClauseAST *node)
{
    parseAll(this, node->baseSpecifierList());
}

void TreeWalker::parseClassSpecifier(ClassSpecifierAST *node)
{
    parseNode(node->winDeclSpec());
    parseNode(node->classKey());
    parseNode(node->baseClause());
    parseAll(this, node->declarationList());
}

void TreeWalker::parseEnumerator(EnumeratorAST *node)
{
    parseNode(node->id());
    parseNode(node->expression());
}

void TreeWalker::parseEnumSpecifier(EnumSpecifierAST *node)
{
    parseAll(this, node->enumeratorList());
}

void TreeWalker::parseElaboratedTypeSpecifier(ElaboratedTypeSpecifierAST *node)
{
    parseNode(node->kind());
}

void TreeWalker::parseLinkageBody(LinkageBodyAST *node)
{
    parseAll(this, node->declarationList());
}

void TreeWalker::parseLinkageSpecification(LinkageSpecificationAST *node)
{
    parseNode(node->externType());
    parseNode(node->linkageBody());
    parseNode(node->declaration());
}

void TreeWalker::parseNamespace(NamespaceAST *node)
{
    parseNode(node->namespaceName());
    parseNode(node->linkageBody());
}

void TreeWalker::parseNamespaceAlias(NamespaceAliasAST *node)
{
    parseNode(node->namespaceName());
    parseNode(node->aliasName());
}

void TreeWalker::parseUsing(UsingAST *node)
{
    parseNode(node->typeName());
    parseNode(node->name());
}

void TreeWalker::parseUsingDirective(UsingDirectiveAST *node)
{
    parseNode(node->name());
}

void TreeWalker::parseDeclarator(DeclaratorAST *node)
{
    parseAll(this, node->ptrOpList());
    parseNode(node->subDeclarator());
    parseNode(node->declaratorId());
    parseNode(node->bitfieldInitialization());
    parseAll(this, node->arrayDimensionList());
    parseNode(node->parameterDeclarationClause());
    parseNode(node->constant());
    parseNode(node->exceptionSpecification());
}

void TreeWalker::parseParameterDeclaration(ParameterDeclarationAST *node)
{
    parseNode(node->typeSpec());
    parseNode(node->declarator());
    parseNode(node->expression());
}

void TreeWalker::parseParameterDeclarationList(ParameterDeclarationListAST *node)
{
    parseAll(this, node->parameterList());
}

void TreeWalker::parseParameterDeclarationClause(ParameterDeclarationClauseAST *node)
{
    parseNode(node->parameterDeclarationList());
    parseNode(node->ellipsis());
}

void TreeWalker::parseInitDeclarator(InitDeclaratorAST *node)
{
    parseNode(node->declarator());
    parseNode(node->initializer());
}

void TreeWalker::parseInitDeclaratorList(InitDeclaratorListAST *node)
{
    parseAll(this, node->initDeclaratorList());
}

void TreeWalker::parseTypedef(TypedefAST *node)
{
    parseNode(node->typeSpec());
    parseNode(node->initDeclaratorList());
}

void TreeWalker::parseTemplateParameter(TemplateParameterAST *node)
{
    parseNode(node->typeParameter());
    parseNode(node->typeValueParameter());
}

void TreeWalker::parseTemplateParameterList(TemplateParameterListAST *node)
{
    parseAll(this, node->templateParameterList());
}

void TreeWalker::parseTemplateDeclaration(TemplateDeclarationAST *node)
{
    parseNode(node->exported());
    parseNode(node->templateParameterList());
    parseNode(node->declaration());
}

void TreeWalker::parseSimpleDeclaration(SimpleDeclarationAST *node)
{
    parseNode(node->functionSpecifier());
    parseNode(node->storageSpecifier());
    parseNode(node->typeSpec());
    parseNode(node->initDeclaratorList());
    parseNode(node->winDeclSpec());
}

void TreeWalker::parseStatement(StatementAST *node)
{
    switch (node->nodeType()) {
    case NodeType_ExpressionStatement:
        parseExpressionStatement(static_cast<ExpressionStatementAST*>(node));
        break;

    case NodeType_IfStatement:
        parseIfStatement(static_cast<IfStatementAST*>(node));
        break;

    case NodeType_WhileStatement:
        parseWhileStatement(static_cast<WhileStatementAST*>(node));
        return;

    case NodeType_DoStatement:
        parseDoStatement(static_cast<DoStatementAST*>(node));
        break;

    case NodeType_ForStatement:
        parseForStatement(static_cast<ForStatementAST*>(node));
        break;

    case NodeType_SwitchStatement:
        parseSwitchStatement(static_cast<SwitchStatementAST*>(node));
        break;

    case NodeType_LabeledStatement:
        parseLabeledStatement(static_cast<LabeledStatementAST*>(node));
        break;

    case NodeType_StatementList:
        parseStatementList(static_cast<StatementListAST*>(node));
        break;

    case NodeType_DeclarationStatement:
        parseDeclarationStatement(static_cast<DeclarationStatementAST*>(node));
        break;

    case NodeType_ReturnStatement:
        parseReturnStatement(static_cast<ReturnStatementAST*>(node));
        break;

    default:
        break;
    }
}

void TreeWalker::parseExpressionStatement(ExpressionStatementAST *node)
{
    parseNode(node->expression());
}

void TreeWalker::parseCondition(ConditionAST *node)
{
    parseNode(node->typeSpec());
    parseNode(node->declarator());
    parseNode(node->expression());
}

void TreeWalker::parseIfStatement(IfStatementAST *node)
{
    parseNode(node->condition());
    parseNode(node->statement());
    parseNode(node->elseStatement());
}

void TreeWalker::parseWhileStatement(WhileStatementAST *node)
{
    parseNode(node->condition());
    parseNode(node->statement());
}

void TreeWalker::parseDoStatement(DoStatementAST *node)
{
    parseNode(node->condition());
    parseNode(node->statement());
}

void TreeWalker::parseForStatement(ForStatementAST *node)
{
    parseNode(node->initStatement());
    parseNode(node->condition());
    parseNode(node->expression());
    parseNode(node->statement());
}

void TreeWalker::parseSwitchStatement(SwitchStatementAST *node)
{
    parseNode(node->condition());
    parseNode(node->statement());
}

void TreeWalker::parseLabeledStatement(LabeledStatementAST *node)
{
    parseNode(node->expression());
    parseNode(node->statement());
}

void TreeWalker::parseStatementList(StatementListAST *node)
{
    parseAll(this, node->statementList());
}

void TreeWalker::parseDeclarationStatement(DeclarationStatementAST *node)
{
    parseNode(node->declaration());
}

void TreeWalker::parseFunctionDefinition(FunctionDefinitionAST *node)
{
    parseNode(node->functionSpecifier());
    parseNode(node->storageSpecifier());
    parseNode(node->typeSpec());
    parseNode(node->initDeclarator());
    parseNode(node->functionBody());
    parseNode(node->winDeclSpec());
}

void TreeWalker::parseTranslationUnit(TranslationUnitAST *node)
{
    parseAll(this, node->declarationList());
}


void TreeWalker::parseExpression(AbstractExpressionAST *node)
{
    parseAll(this, node->children());
}


void TreeWalker::parseBinaryExpression(BinaryExpressionAST *node)
{
    parseNode(node->op());
    parseNode(node->leftExpression());
    parseNode(node->rightExpression());
}

void TreeWalker::parseReturnStatement(ReturnStatementAST *node)
{
    parseNode(node->expression());
}

QT_END_NAMESPACE
