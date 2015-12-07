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

#include "ast.h"
#include <QStringList>
#include <stdio.h>

QT_BEGIN_NAMESPACE

int AST::N = 0;

// ------------------------------------------------------------------------
AST::AST(int startToken, int count)
    : m_scope(0),
      m_startToken(startToken),
      m_endToken(startToken + count),
      m_parent(0),
      m_children(0)
{
    ++N;
}

void AST::setParent(AST *parent)
{
    if (m_parent)
        m_parent->removeChild(this);

    m_parent = parent;

    if (m_parent)
        m_parent->appendChild(this);
}

void AST::appendChild(AST *child)
{
    m_children = snoc(m_children, child, _pool);
}

void AST::removeChild(AST *child)
{
    fprintf(stderr, "AST::removeChild child: %p not implemented yet\n", child);
}

// ------------------------------------------------------------------------
NameAST::NameAST()
    : m_global(false), m_unqualifiedName(0), m_classOrNamespaceNameList(0)
{
}

void NameAST::setGlobal(bool b)
{
    m_global = b;
}

void NameAST::setUnqualifiedName(ClassOrNamespaceNameAST *unqualifiedName)
{
    m_unqualifiedName = unqualifiedName;
    if (m_unqualifiedName) m_unqualifiedName->setParent(this);
}

void NameAST::addClassOrNamespaceName(ClassOrNamespaceNameAST *classOrNamespaceName)
{
    if(!classOrNamespaceName)
        return;

    classOrNamespaceName->setParent(this);
    m_classOrNamespaceNameList = snoc(m_classOrNamespaceNameList, classOrNamespaceName, _pool);
}

// ------------------------------------------------------------------------
DeclarationAST::DeclarationAST()
{
}

// ------------------------------------------------------------------------
LinkageBodyAST::LinkageBodyAST()
    : m_declarationList(0)
{
}

void LinkageBodyAST::addDeclaration(DeclarationAST *ast)
{
    if(!ast)
        return;

    ast->setParent(this);
    m_declarationList = snoc(m_declarationList, ast, _pool);
}

// ------------------------------------------------------------------------
LinkageSpecificationAST::LinkageSpecificationAST()
    : m_externType(0),
      m_linkageBody(0),
      m_declaration(0)
{
}

void LinkageSpecificationAST::setExternType(AST *externType)
{
    m_externType = externType;
    if (m_externType) m_externType->setParent(this);
}

void LinkageSpecificationAST::setLinkageBody(LinkageBodyAST *linkageBody)
{
    m_linkageBody = linkageBody;
    if (m_linkageBody) m_linkageBody->setParent(this);
}

void LinkageSpecificationAST::setDeclaration(DeclarationAST *decl)
{
    m_declaration = decl;
    if (m_declaration) m_declaration->setParent(this);
}

// ------------------------------------------------------------------------
TranslationUnitAST::TranslationUnitAST()
    : m_declarationList(0)
{
    //kdDebug(9007) << "++ TranslationUnitAST::TranslationUnitAST()" << endl;
}

void TranslationUnitAST::addDeclaration(DeclarationAST *ast)
{
    if(!ast)
        return;

    ast->setParent(this);
    m_declarationList = snoc(m_declarationList, ast, _pool);
}

// ------------------------------------------------------------------------
NamespaceAST::NamespaceAST()
    : m_namespaceName(0),
      m_linkageBody(0)
{
}

void NamespaceAST::setNamespaceName(AST *namespaceName)
{
    m_namespaceName = namespaceName;
    if (m_namespaceName) m_namespaceName->setParent(this);
}

void NamespaceAST::setLinkageBody(LinkageBodyAST *linkageBody)
{
    m_linkageBody = linkageBody;
    if (m_linkageBody) m_linkageBody->setParent(this);
}


// ------------------------------------------------------------------------
NamespaceAliasAST::NamespaceAliasAST()
    : m_namespaceName(0),
      m_aliasName(0)
{
}

void NamespaceAliasAST::setNamespaceName(AST *namespaceName)
{
    m_namespaceName = namespaceName;
    if (m_namespaceName) m_namespaceName->setParent(this);
}

void NamespaceAliasAST::setAliasName(NameAST *name)
{
    m_aliasName = name;
    if (m_aliasName) m_aliasName->setParent(this);
}

// ------------------------------------------------------------------------
UsingAST::UsingAST()
    : m_typeName(0),
      m_name(0)
{
}

void UsingAST::setTypeName(AST *typeName)
{
    m_typeName = typeName;
    if (m_typeName) m_typeName->setParent(this);
}

void UsingAST::setName(NameAST *name)
{
    m_name = name;
    if (m_name) m_name->setParent(this);
}

// ------------------------------------------------------------------------
UsingDirectiveAST::UsingDirectiveAST()
    : m_name(0)
{
}

void UsingDirectiveAST::setName(NameAST *name)
{
    m_name = name;
    if (m_name) m_name->setParent(this);
}

TypedefAST::TypedefAST()
    : m_typeSpec(0),
      m_initDeclaratorList(0)
{
}

void TypeSpecifierAST::setName(NameAST *name)
{
    m_name = name;
    if (m_name) m_name->setParent(this);
}

void TypedefAST::setTypeSpec(TypeSpecifierAST *typeSpec)
{
    m_typeSpec = typeSpec;
    if (m_typeSpec) m_typeSpec->setParent(this);
}

void TypedefAST::setInitDeclaratorList(InitDeclaratorListAST *initDeclaratorList)
{
    m_initDeclaratorList = initDeclaratorList;
    if (m_initDeclaratorList) m_initDeclaratorList->setParent(this);
}

// ------------------------------------------------------------------------
TemplateArgumentListAST::TemplateArgumentListAST()
    : m_argumentList(0)
{
}

void TemplateArgumentListAST::addArgument(AST *arg)
{
    if(!arg)
        return;

    arg->setParent(this);
    m_argumentList = snoc(m_argumentList, arg, _pool);
}

// ------------------------------------------------------------------------
TemplateDeclarationAST::TemplateDeclarationAST()
    : m_exported(0),
      m_templateParameterList(0),
      m_declaration(0)
{
}

void TemplateDeclarationAST::setExported(AST *exported)
{
    m_exported = exported;
    if (m_exported) m_exported->setParent(this);
}

void TemplateDeclarationAST::setTemplateParameterList(TemplateParameterListAST *templateParameterList)
{
    m_templateParameterList = templateParameterList;
    if (m_templateParameterList) m_templateParameterList->setParent(this);
}

void TemplateDeclarationAST::setDeclaration(DeclarationAST *declaration)
{
    m_declaration = declaration;
    if (m_declaration) m_declaration->setParent(this);
}

// ------------------------------------------------------------------------
ClassOrNamespaceNameAST::ClassOrNamespaceNameAST()
    : m_name(0), m_templateArgumentList(0)
{
}

void ClassOrNamespaceNameAST::setName(AST *name)
{
    m_name = name;
    if (m_name) m_name->setParent(this);
}

void ClassOrNamespaceNameAST::setTemplateArgumentList(TemplateArgumentListAST *templateArgumentList)
{
    m_templateArgumentList = templateArgumentList;
    if (m_templateArgumentList) m_templateArgumentList->setParent(this);
}

// ------------------------------------------------------------------------
TypeSpecifierAST::TypeSpecifierAST()
    : m_name(0), m_cvQualify(0), m_cv2Qualify(0)

{
}

void TypeSpecifierAST::setCvQualify(AST *cvQualify)
{
    m_cvQualify = cvQualify;
    if (m_cvQualify) m_cvQualify->setParent(this);
}

void TypeSpecifierAST::setCv2Qualify(AST *cv2Qualify)
{
    m_cv2Qualify = cv2Qualify;
    if (m_cv2Qualify) m_cv2Qualify->setParent(this);
}

// ------------------------------------------------------------------------
ClassSpecifierAST::ClassSpecifierAST()
    : m_winDeclSpec(0),
      m_classKey(0),
      m_baseClause(0),
      m_declarationList(0)
{
}

void ClassSpecifierAST::setClassKey(AST *classKey)
{
    m_classKey = classKey;
    if (m_classKey) m_classKey->setParent(this);
}

void ClassSpecifierAST::addDeclaration(DeclarationAST *declaration)
{
    if(!declaration)
        return;

    declaration->setParent(this);
    m_declarationList = snoc(m_declarationList, declaration, _pool);
}

void ClassSpecifierAST::setBaseClause(BaseClauseAST *baseClause)
{
    m_baseClause = baseClause;
    if (m_baseClause) m_baseClause->setParent(this);
}

// ------------------------------------------------------------------------
EnumSpecifierAST::EnumSpecifierAST()
    : m_enumeratorList(0)
{
}

void EnumSpecifierAST::addEnumerator(EnumeratorAST *enumerator)
{
    if(!enumerator)
        return;

    enumerator->setParent(this);
    m_enumeratorList = snoc(m_enumeratorList, enumerator, _pool);
}


// ------------------------------------------------------------------------
ElaboratedTypeSpecifierAST::ElaboratedTypeSpecifierAST()
    : m_kind(0)
{
}

void ElaboratedTypeSpecifierAST::setKind(AST *kind)
{
    m_kind = kind;
    if (m_kind) m_kind->setParent(this);
}

// ------------------------------------------------------------------------
EnumeratorAST::EnumeratorAST()
    : m_id(0),
      m_expression(0)
{
}

void EnumeratorAST::setId(AST *id)
{
    m_id = id;
    if (m_id) m_id->setParent(this);
}

void EnumeratorAST::setExpression(AbstractExpressionAST *expression)
{
    m_expression = expression;
    if (m_expression) m_expression->setParent(this);
}

// ------------------------------------------------------------------------
BaseClauseAST::BaseClauseAST()
    : m_baseSpecifierList(0)
{
}

void BaseClauseAST::addBaseSpecifier(BaseSpecifierAST *baseSpecifier)
{
    if(!baseSpecifier)
        return;

    baseSpecifier->setParent(this);
    m_baseSpecifierList = snoc(m_baseSpecifierList, baseSpecifier, _pool);
}

// ------------------------------------------------------------------------
BaseSpecifierAST::BaseSpecifierAST()
    : m_isVirtual(0), m_access(0), m_name(0)

{
}

void BaseSpecifierAST::setIsVirtual(AST *isVirtual)
{
    m_isVirtual = isVirtual;
    if (m_isVirtual) m_isVirtual->setParent(this);
}

void BaseSpecifierAST::setAccess(AST *access)
{
    m_access = access;
    if (m_access) m_access->setParent(this);
}

void BaseSpecifierAST::setName(NameAST *name)
{
    m_name = name;
    if (m_name) m_name->setParent(this);
}

// ------------------------------------------------------------------------
SimpleDeclarationAST::SimpleDeclarationAST()
    : m_functionSpecifier(0),
      m_storageSpecifier(0),
      m_typeSpec(0),
      m_initDeclaratorList(0),
      m_winDeclSpec(0)
{
}

void SimpleDeclarationAST::setFunctionSpecifier(AST *functionSpecifier)
{
    m_functionSpecifier = functionSpecifier;
    if (m_functionSpecifier) m_functionSpecifier->setParent(this);
}

void SimpleDeclarationAST::setStorageSpecifier(AST *storageSpecifier)
{
    m_storageSpecifier = storageSpecifier;
    if (m_storageSpecifier) m_storageSpecifier->setParent(this);
}

void SimpleDeclarationAST::setTypeSpec(TypeSpecifierAST *typeSpec)
{
    m_typeSpec = typeSpec;
    if (m_typeSpec) m_typeSpec->setParent(this);
}

void SimpleDeclarationAST::setInitDeclaratorList(InitDeclaratorListAST *initDeclaratorList)
{
    m_initDeclaratorList = initDeclaratorList;
    if (m_initDeclaratorList) m_initDeclaratorList->setParent(this);
}

void SimpleDeclarationAST::setWinDeclSpec(AST *winDeclSpec)
{
    m_winDeclSpec = winDeclSpec;
    if (m_winDeclSpec) m_winDeclSpec->setParent(this);
}

// ------------------------------------------------------------------------
InitDeclaratorListAST::InitDeclaratorListAST()
    : m_initDeclaratorList(0)
{
}

void InitDeclaratorListAST::addInitDeclarator(InitDeclaratorAST *decl)
{
    if(!decl)
        return;

    decl->setParent(this);
    m_initDeclaratorList = snoc(m_initDeclaratorList, decl, _pool);
}

// ------------------------------------------------------------------------
DeclaratorAST::DeclaratorAST()
    : m_ptrOpList(0),
      m_subDeclarator(0),
      m_declaratorId(0),
      m_bitfieldInitialization(0),
      m_arrayDimensionList(0),
      m_parameterDeclarationClause(0),
      m_constant(0),
      m_exceptionSpecification(0)
{
}

void DeclaratorAST::setSubDeclarator(DeclaratorAST *subDeclarator)
{
    m_subDeclarator = subDeclarator;
    if (m_subDeclarator) m_subDeclarator->setParent(this);
}

void DeclaratorAST::setDeclaratorId(NameAST *declaratorId)
{
    m_declaratorId = declaratorId;
    if (m_declaratorId) m_declaratorId->setParent(this);
}

void DeclaratorAST::setBitfieldInitialization(AST *bitfieldInitialization)
{
    m_bitfieldInitialization = bitfieldInitialization;
    if (m_bitfieldInitialization) m_bitfieldInitialization->setParent(this);
}

void DeclaratorAST::addArrayDimension(AST *arrayDimension)
{
    if(!arrayDimension)
        return;

    arrayDimension->setParent(this);
    m_arrayDimensionList = snoc(m_arrayDimensionList, arrayDimension, _pool);
}

void DeclaratorAST::setParameterDeclarationClause(ParameterDeclarationClauseAST *parameterDeclarationClause)
{
    m_parameterDeclarationClause = parameterDeclarationClause;
    if (m_parameterDeclarationClause) m_parameterDeclarationClause->setParent(this);
}

void DeclaratorAST::setConstant(AST *constant)
{
    m_constant = constant;
    if (m_constant) m_constant->setParent(this);
}

void DeclaratorAST::setExceptionSpecification(AST *exceptionSpecification)
{
    m_exceptionSpecification = exceptionSpecification;
    if (m_exceptionSpecification) m_exceptionSpecification->setParent(this);
}

void DeclaratorAST::addPtrOp(AST *ptrOp)
{
    if(!ptrOp)
        return;

    ptrOp->setParent(this);
    m_ptrOpList = snoc(m_ptrOpList, ptrOp, _pool);
}

// --------------------------------------------------------------------------
InitDeclaratorAST::InitDeclaratorAST()
    : m_declarator(0),
      m_initializer(0)
{
}

void InitDeclaratorAST::setDeclarator(DeclaratorAST *declarator)
{
    m_declarator = declarator;
    if (m_declarator) m_declarator->setParent(this);
}

void InitDeclaratorAST::setInitializer(AST *initializer)
{
    m_initializer = initializer;
    if (m_initializer) m_initializer->setParent(this);
}

// --------------------------------------------------------------------------
FunctionDefinitionAST::FunctionDefinitionAST()
    : m_functionSpecifier(0),
      m_storageSpecifier(0),
      m_typeSpec(0),
      m_initDeclarator(0),
      m_functionBody(0),
      m_winDeclSpec(0)
{
}

void FunctionDefinitionAST::setFunctionSpecifier(AST *functionSpecifier)
{
    m_functionSpecifier = functionSpecifier;
    if (m_functionSpecifier) m_functionSpecifier->setParent(this);
}

void FunctionDefinitionAST::setStorageSpecifier(AST *storageSpecifier)
{
    m_storageSpecifier = storageSpecifier;
    if (m_storageSpecifier) m_storageSpecifier->setParent(this);
}

void FunctionDefinitionAST::setTypeSpec(TypeSpecifierAST *typeSpec)
{
    m_typeSpec = typeSpec;
    if (m_typeSpec) m_typeSpec->setParent(this);
}

void FunctionDefinitionAST::setInitDeclarator(InitDeclaratorAST *initDeclarator)
{
    m_initDeclarator = initDeclarator;
    if (m_initDeclarator) m_initDeclarator->setParent(this);
}

void FunctionDefinitionAST::setFunctionBody(StatementListAST *functionBody)
{
    m_functionBody = functionBody;
    if (m_functionBody) m_functionBody->setParent(this);
}

void FunctionDefinitionAST::setWinDeclSpec(AST *winDeclSpec)
{
    m_winDeclSpec = winDeclSpec;
    if (m_winDeclSpec) m_winDeclSpec->setParent(this);
}

// --------------------------------------------------------------------------
StatementListAST::StatementListAST()
    : m_statementList(0)
{
}

void StatementListAST::addStatement(StatementAST *statement)
{
    if(!statement)
        return;

    statement->setParent(this);
    m_statementList = snoc(m_statementList, statement, _pool);
}

// --------------------------------------------------------------------------
IfStatementAST::IfStatementAST()
    : m_condition(0),
      m_statement(0),
      m_elseStatement(0)
{
}

void IfStatementAST::setCondition(ConditionAST *condition)
{
    m_condition = condition;
    if (m_condition) m_condition->setParent(this);
}

void IfStatementAST::setStatement(StatementAST *statement)
{
    m_statement = statement;
    if (m_statement) m_statement->setParent(this);
}

void IfStatementAST::setElseStatement(StatementAST *elseStatement)
{
    m_elseStatement = elseStatement;
    if (m_elseStatement) m_elseStatement->setParent(this);
}

// --------------------------------------------------------------------------
WhileStatementAST::WhileStatementAST()
    : m_condition(0),
      m_statement(0)
{
}

void WhileStatementAST::setCondition(ConditionAST *condition)
{
    m_condition = condition;
    if (m_condition) m_condition->setParent(this);
}

void WhileStatementAST::setStatement(StatementAST *statement)
{
    m_statement = statement;
    if (m_statement) m_statement->setParent(this);
}

// --------------------------------------------------------------------------
DoStatementAST::DoStatementAST()
    : m_condition(0),
      m_statement(0)
{
}

void DoStatementAST::setCondition(ConditionAST *condition)
{
    m_condition = condition;
    if (m_condition) m_condition->setParent(this);
}

void DoStatementAST::setStatement(StatementAST *statement)
{
    m_statement = statement;
    if (m_statement) m_statement->setParent(this);
}

// --------------------------------------------------------------------------
ForStatementAST::ForStatementAST()
    : m_condition(0),
      m_initStatement(0),
      m_statement(0),
      m_expression(0)
{
}

void ForStatementAST::setCondition(ConditionAST *condition)
{
    m_condition = condition;
    if (m_condition) m_condition->setParent(this);
}

void ForStatementAST::setExpression(AbstractExpressionAST *expression)
{
    m_expression = expression;
    if (m_expression) m_expression->setParent(this);
}

void ForStatementAST::setStatement(StatementAST *statement)
{
    m_statement = statement;
    if (m_statement) m_statement->setParent(this);
}

void ForStatementAST::setInitStatement(StatementAST *initStatement)
{
    m_initStatement = initStatement;
    if (m_initStatement) m_initStatement->setParent(this);
}

// --------------------------------------------------------------------------
SwitchStatementAST::SwitchStatementAST()
    : m_condition(0),
      m_statement(0)
{
}

void SwitchStatementAST::setCondition(ConditionAST *condition)
{
    m_condition = condition;
    if (m_condition) m_condition->setParent(this);
}

void SwitchStatementAST::setStatement(StatementAST *statement)
{
    m_statement = statement;
    if (m_statement) m_statement->setParent(this);
}

// --------------------------------------------------------------------------
DeclarationStatementAST::DeclarationStatementAST()
    : m_declaration(0)
{
}

void DeclarationStatementAST::setDeclaration(DeclarationAST *declaration)
{
    m_declaration = declaration;
    if (m_declaration) m_declaration->setParent(this);
}

// --------------------------------------------------------------------------
LabeledStatementAST::LabeledStatementAST()
    : m_statement(0), m_expression(0)
{
}

void LabeledStatementAST::setStatement(StatementAST *statement)
{
    m_statement = statement;
    if (m_statement) m_statement->setParent(this);
}

void LabeledStatementAST::setExpression(AbstractExpressionAST *expression)
{
    m_expression = expression;
    if (m_expression) m_expression->setParent(this);
}

// --------------------------------------------------------------------------
ExpressionStatementAST::ExpressionStatementAST()
    : m_expression(0)
{
}

void ExpressionStatementAST::setExpression(AbstractExpressionAST *expression)
{
    m_expression = expression;
    if (m_expression) m_expression->setParent(this);
}


// --------------------------------------------------------------------------
ParameterDeclarationAST::ParameterDeclarationAST()
    : m_typeSpec(0),
      m_declarator(0),
      m_expression(0)
{
}

void ParameterDeclarationAST::setTypeSpec(TypeSpecifierAST *typeSpec)
{
    m_typeSpec = typeSpec;
    if (m_typeSpec) m_typeSpec->setParent(this);
}

void ParameterDeclarationAST::setDeclarator(DeclaratorAST *declarator)
{
    m_declarator = declarator;
    if (m_declarator) m_declarator->setParent(this);
}

void ParameterDeclarationAST::setExpression(AbstractExpressionAST *expression)
{
    m_expression = expression;
    if (m_expression) m_expression->setParent(this);
}

// --------------------------------------------------------------------------
ParameterDeclarationListAST::ParameterDeclarationListAST()
    : m_parameterList(0)
{
}

void ParameterDeclarationListAST::addParameter(ParameterDeclarationAST *parameter)
{
    if(!parameter)
        return;

    parameter->setParent(this);
    m_parameterList = snoc(m_parameterList, parameter, _pool);
}

// --------------------------------------------------------------------------
ParameterDeclarationClauseAST::ParameterDeclarationClauseAST()
    : m_parameterDeclarationList(0),
      m_ellipsis(0)
{
}

void ParameterDeclarationClauseAST::setParameterDeclarationList(ParameterDeclarationListAST *parameterDeclarationList)
{
    m_parameterDeclarationList = parameterDeclarationList;
    if (m_parameterDeclarationList) m_parameterDeclarationList->setParent(this);
}

void ParameterDeclarationClauseAST::setEllipsis(AST *ellipsis)
{
    m_ellipsis = ellipsis;
    if (m_ellipsis) m_ellipsis->setParent(this);
}

// --------------------------------------------------------------------------
AccessDeclarationAST::AccessDeclarationAST()
    : m_accessList(0)
{
}

void AccessDeclarationAST::addAccess(AST *access)
{
    if(!access)
        return;

    access->setParent(this);
    m_accessList = snoc(m_accessList, access, _pool);
}

// --------------------------------------------------------------------------
TypeParameterAST::TypeParameterAST()
    : m_kind(0), m_templateParameterList(0),
      m_name(0), m_typeId(0)

{
}

void TypeParameterAST::setKind(AST *kind)
{
    m_kind = kind;
}

void TypeParameterAST::setTemplateParameterList(TemplateParameterListAST *templateParameterList)
{
    m_templateParameterList = templateParameterList;
    if (m_templateParameterList) m_templateParameterList->setParent(this);
}

void TypeParameterAST::setName(NameAST *name)
{
    m_name = name;
    if (m_name) m_name->setParent(this);
}

void TypeParameterAST::setTypeId(AST *typeId)
{
    m_typeId = typeId;
    if (m_typeId) m_typeId->setParent(this);
}

// --------------------------------------------------------------------------
TemplateParameterAST::TemplateParameterAST()
    : m_typeParameter(0),
      m_typeValueParameter(0)
{
}

void TemplateParameterAST::setTypeParameter(TypeParameterAST *typeParameter)
{
    m_typeParameter = typeParameter;
    if (m_typeParameter) m_typeParameter->setParent(this);
}

void TemplateParameterAST::setTypeValueParameter(ParameterDeclarationAST *typeValueParameter)
{
    m_typeValueParameter = typeValueParameter;
    if (m_typeValueParameter) m_typeValueParameter->setParent(this);
}

// --------------------------------------------------------------------------
TemplateParameterListAST::TemplateParameterListAST()
    : m_templateParameterList(0)
{
}

void TemplateParameterListAST::addTemplateParameter(TemplateParameterAST *templateParameter)
{
    if(!templateParameter)
        return;

    templateParameter->setParent(this);
    m_templateParameterList = snoc(m_templateParameterList, templateParameter, _pool);
}

// --------------------------------------------------------------------------
ConditionAST::ConditionAST()
    : m_typeSpec(0),
      m_declarator(0),
      m_expression(0)
{
}

void ConditionAST::setTypeSpec(TypeSpecifierAST *typeSpec)
{
    m_typeSpec = typeSpec;
    if (m_typeSpec) m_typeSpec->setParent(this);
}

void ConditionAST::setDeclarator(DeclaratorAST *declarator)
{
    m_declarator = declarator;
    if (m_declarator) m_declarator->setParent(this);
}

void ConditionAST::setExpression(AbstractExpressionAST *expression)
{
    m_expression = expression;
    if (m_expression) m_expression->setParent(this);
}

void ClassSpecifierAST::setWinDeclSpec(AST *winDeclSpec)
{
    m_winDeclSpec = winDeclSpec;
    if (m_winDeclSpec) m_winDeclSpec->setParent(this);
}

// --------------------------------------------------------------------------
ReturnStatementAST::ReturnStatementAST()
    : m_expression(0)
{
}

void ReturnStatementAST::setExpression(AbstractExpressionAST *expression)
{
    m_expression = expression;
    if (m_expression) m_expression->setParent(this);
}

// --------------------------------------------------------------------------
BinaryExpressionAST::BinaryExpressionAST()
    : m_op(0), m_left(0), m_right(0)
{
}

void BinaryExpressionAST::setOp(AST *op)
{
    m_op = op;
    if (m_op)
        m_op->setParent(this);
}

void BinaryExpressionAST::setLeftExpression(AbstractExpressionAST *left)
{
    m_left = left;
    if (m_left)
        m_left->setParent(this);
}

void BinaryExpressionAST::setRightExpression(AbstractExpressionAST *right)
{
    m_right = right;
    if (m_right)
        m_right->setParent(this);
}

// --------------------------------------------------------------------------
ConditionalExpressionAST::ConditionalExpressionAST()
    : m_condition(0), m_left(0), m_right(0)
{
}

void ConditionalExpressionAST::setCondition(AbstractExpressionAST *condition)
{
    m_condition = condition;
    if (m_condition)
        m_condition->setParent(this);
}

void ConditionalExpressionAST::setLeftExpression(AbstractExpressionAST *left)
{
    m_left = left;
    if (m_left)
        m_left->setParent(this);
}

void ConditionalExpressionAST::setRightExpression(AbstractExpressionAST *right)
{
    m_right = right;
    if (m_right)
        m_right->setParent(this);
}

// --------------------------------------------------------------------------
CppCastExpressionAST::CppCastExpressionAST()
    : m_castOp(0), m_typeId(0), m_expression(0)
{
}

void CppCastExpressionAST::setCastOp(AST *castOp)
{
    m_castOp = castOp;
    if (m_castOp)
        m_castOp->setParent(this);
}

void CppCastExpressionAST::setTypeId(AST *typeId)
{
    m_typeId = typeId;
    if (m_typeId)
        m_typeId->setParent(this);
}

void CppCastExpressionAST::setExpression(AbstractExpressionAST *expression)
{
    m_expression = expression;
    if (m_expression)
        m_expression->setParent(this);
}

// --------------------------------------------------------------------------
SubscriptingAST::SubscriptingAST()
    : m_expression(0), m_subscript(0)
{
}

void SubscriptingAST::setSubscript(AbstractExpressionAST *subscript)
{
    m_subscript = subscript;
    if (m_subscript)
        m_subscript->setParent(this);
}

void SubscriptingAST::setExpression(AbstractExpressionAST *expression)
{
    m_expression = expression;
    if (m_expression)
        m_expression->setParent(this);
}

// --------------------------------------------------------------------------
FunctionCallAST::FunctionCallAST()
    : m_expression(0), m_arguments(0)
{
}

void FunctionCallAST::setExpression(AbstractExpressionAST *expression)
{
    m_expression = expression;
    if (m_expression)
        m_expression->setParent(this);
}

void FunctionCallAST::setArguments(AbstractExpressionAST *arguments)
{
    m_arguments = arguments;
    if (m_arguments)
        m_arguments->setParent(this);
}

// --------------------------------------------------------------------------
ExplicitTypeConversionAST::ExplicitTypeConversionAST()
{
}

// --------------------------------------------------------------------------
PseudoDestructorCallAST::PseudoDestructorCallAST()
{
}

// --------------------------------------------------------------------------
ClassMemberAccessAST::ClassMemberAccessAST()
    : m_op(0), m_expression(0), m_templ(0), m_name(0)
{
}

void ClassMemberAccessAST::setOp(AST *op)
{
    m_op = op;
    if (m_op)
        m_op->setParent(this);
}

void ClassMemberAccessAST::setExpression(AbstractExpressionAST *expression)
{
    m_expression = expression;
    if (m_expression)
        m_expression->setParent(this);
}

void ClassMemberAccessAST::setName(NameAST *name)
{
    m_name = name;
    if (m_name)
        m_name->setParent(this);
}

// --------------------------------------------------------------------------
IncrDecrAST::IncrDecrAST()
    : m_op(0), m_expression(0)
{
}

void IncrDecrAST::setOp(AST *op)
{
    m_op = op;
    if (m_op)
        m_op->setParent(this);
}

void IncrDecrAST::setExpression(AbstractExpressionAST *expression)
{
    m_expression = expression;
    if (m_expression)
        m_expression->setParent(this);
}

// --------------------------------------------------------------------------
TypeIdentificationAST::TypeIdentificationAST()
{
}

// --------------------------------------------------------------------------
TypeIdAST::TypeIdAST()
    : m_typeSpecifier(0), m_declarator(0)
{
}

void TypeIdAST::setTypeSpecifier(TypeSpecifierAST *typeSpecifier)
{
    m_typeSpecifier = typeSpecifier;
    if (m_typeSpecifier)
        m_typeSpecifier->setParent(this);
}

void TypeIdAST::setDeclarator(DeclaratorAST *declarator)
{
    m_declarator = declarator;
    if (m_declarator)
        m_declarator->setParent(this);
}

// --------------------------------------------------------------------------
AbstractExpressionAST::AbstractExpressionAST()
{
    m_symbol = 0;
}

QT_END_NAMESPACE
