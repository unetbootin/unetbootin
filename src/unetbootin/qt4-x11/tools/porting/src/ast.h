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

#ifndef AST_H
#define AST_H

#include "smallobject.h"
#include "list.h"

QT_BEGIN_NAMESPACE

class AST;
class NameAST;
class TypeIdAST;
class TypeSpecifierAST;
class DeclaratorAST;

class Symbol;
class Scope;

enum NodeType
{
    NodeType_Generic = 0,

    NodeType_TemplateArgumentList = 1000,
    NodeType_ClassOrNamespaceName,
    NodeType_Name,
    NodeType_Declaration,
    NodeType_TypeSpecifier,
    NodeType_BaseSpecifier,
    NodeType_BaseClause,
    NodeType_ClassSpecifier,
    NodeType_Enumerator,
    NodeType_EnumSpecifier,
    NodeType_ElaboratedTypeSpecifier,
    NodeType_LinkageBody,
    NodeType_LinkageSpecification,
    NodeType_Namespace,
    NodeType_NamespaceAlias,
    NodeType_Using,
    NodeType_UsingDirective,
    NodeType_InitDeclaratorList,
    NodeType_Typedef,
    NodeType_Declarator,
    NodeType_InitDeclarator,
    NodeType_TemplateDeclaration,
    NodeType_SimpleDeclaration,
    NodeType_Statement,
    NodeType_StatementList,
    NodeType_IfStatement,
    NodeType_WhileStatement,
    NodeType_DoStatement,
    NodeType_ForStatement,
    NodeType_SwitchStatement,
    NodeType_DeclarationStatement,
    NodeType_LabeledStatement,
    NodeType_ReturnStatement,
    NodeType_TranslationUnit,
    NodeType_FunctionDefinition,
    NodeType_ExpressionStatement,
    NodeType_ParameterDeclaration,
    NodeType_ParameterDeclarationList,
    NodeType_ParameterDeclarationClause,
    NodeType_AccessDeclaration,
    NodeType_TypeParameter,
    NodeType_TemplateParameter,
    NodeType_TemplateParameterList,
    NodeType_Condition,

    NodeType_TypeId,

    NodeType_Expression = 2000,
    NodeType_BinaryExpression,
    NodeType_PrimaryExpression,

//
// postfix expression
//
    NodeType_PostfixExpression,
    NodeType_Subscripting,
    NodeType_FunctionCall,
    NodeType_ExplicitTypeConversion,
    NodeType_PseudoConstructorCall,
    NodeType_ClassMemberAccess,
    NodeType_IncrDecr,
    NodeType_CppCastExpression,
    NodeType_TypeIdentification,

    NodeType_UnaryExpression,
    NodeType_NewExpression,
    NodeType_NewTypeId,
    NodeType_NewDeclarator,
    NodeType_NewInitializer,
    NodeType_DeleteExpression,
    NodeType_CastExpression,
    NodeType_ConditionalExpression,
    NodeType_ThrowExpression,

    NodeType_Custom = 3000
};


template <typename T>
inline int length(List<T> *e)
{
    return e ? e->size() : 0;
}

class AST
{
public:
    enum { Type=NodeType_Generic };

    pool *_pool;

    static int N;

public:
    AST(int startToken=0, int count=1);
    virtual ~AST() { --N; }

    inline int startToken() const
    { return m_startToken; }

    inline int endToken() const
    { return m_endToken; }

    inline void setPosition(int startToken, int endToken)
    {
        m_startToken = startToken;
        m_endToken = endToken;
    }

    inline int nodeType() const
    { return m_nodeType; }

    inline void setNodeType(int nodeType)
    { m_nodeType = nodeType; }

    inline AST *parent() const
    { return m_parent; }

    void setParent(AST *parent);

    inline List<AST *> *children() const
    { return m_children; }

    void appendChild(AST *child);
    void removeChild(AST *child);

// ### move
    inline Scope *scope() const
    {
        if (m_scope)
            return m_scope;

        return m_parent ? m_parent->scope() : 0;
    }

    inline void setScope(Scope *scope)
    { m_scope = scope; }

private:
    Scope *m_scope;
    int m_nodeType;
    int m_startToken;
    int m_endToken;
    AST *m_parent;
    List<AST *> *m_children;

private:
    AST(const AST &source);
    void operator = (const AST &source);
};

class AbstractExpressionAST: public AST
{
public:
    enum { Type = NodeType_Expression };

    AbstractExpressionAST();

    inline Symbol *symbol() const
    { return m_symbol; }

    inline void setSymbol(Symbol *symbol)
    { m_symbol = symbol; }

private:
    Symbol *m_symbol;
};

// ### remove me
template <int kind, class Base = AbstractExpressionAST>
class ExpressionAST: public Base
{
public:
    enum { Type = kind };

public:
    inline ExpressionAST() {}

private:
    ExpressionAST(const ExpressionAST &source);
    void operator = (const ExpressionAST &source);
};

class BinaryExpressionAST: public AbstractExpressionAST
{
public:
    enum { Type = NodeType_BinaryExpression };

public:
    BinaryExpressionAST();

    inline AST *op() const
    { return m_op; }

    inline AbstractExpressionAST *leftExpression() const
    { return m_left; }

    inline AbstractExpressionAST *rightExpression() const
    { return m_right; }

    void setOp(AST *op);
    void setLeftExpression(AbstractExpressionAST *left);
    void setRightExpression(AbstractExpressionAST *right);

private:
    AST *m_op;
    AbstractExpressionAST *m_left;
    AbstractExpressionAST *m_right;

private:
    BinaryExpressionAST(const BinaryExpressionAST &source);
    void operator = (const BinaryExpressionAST &source);
};

class ConditionalExpressionAST: public AbstractExpressionAST
{
public:
    enum { Type = NodeType_ConditionalExpression };

public:
    ConditionalExpressionAST();

    inline AbstractExpressionAST *condition() const
    { return m_condition; }

    inline AbstractExpressionAST *leftExpression() const
    { return m_left; }

    inline AbstractExpressionAST *rightExpression() const
    { return m_right; }

    void setCondition(AbstractExpressionAST *condition);
    void setLeftExpression(AbstractExpressionAST *left);
    void setRightExpression(AbstractExpressionAST *right);

private:
    AbstractExpressionAST *m_condition;
    AbstractExpressionAST *m_left;
    AbstractExpressionAST *m_right;

private:
    ConditionalExpressionAST(const ConditionalExpressionAST& source);
    void operator = (const ConditionalExpressionAST &source);
};


//
// postfix expression
//

class SubscriptingAST: public AbstractExpressionAST
{
public:
    enum { Type = NodeType_Subscripting };

public:
    SubscriptingAST();

    inline AbstractExpressionAST *expression() const
    { return m_expression; }

    inline AbstractExpressionAST *subscript() const
    { return m_subscript; }

    void setExpression(AbstractExpressionAST *expression);
    void setSubscript(AbstractExpressionAST *subscript);

private:
    AbstractExpressionAST *m_expression;
    AbstractExpressionAST *m_subscript;

private:
    SubscriptingAST(const SubscriptingAST &source);
    void operator = (const SubscriptingAST &source);
};

class FunctionCallAST: public AbstractExpressionAST
{
public:
    enum { Type = NodeType_FunctionCall };

public:
    FunctionCallAST();

    inline AbstractExpressionAST *expression() const
    { return m_expression; }

    inline AbstractExpressionAST *arguments() const
    { return m_arguments; }

    void setExpression(AbstractExpressionAST *expression);
    void setArguments(AbstractExpressionAST *arguments);

private:
    AbstractExpressionAST *m_expression;
    AbstractExpressionAST *m_arguments;

private:
    FunctionCallAST(const FunctionCallAST &source);
    void operator = (const FunctionCallAST &source);
};

class ExplicitTypeConversionAST: public AbstractExpressionAST
{
public:
    enum { Type = NodeType_ExplicitTypeConversion };

public:
    ExplicitTypeConversionAST();

private:
    ExplicitTypeConversionAST(const ExplicitTypeConversionAST &source);
    void operator = (const ExplicitTypeConversionAST &source);
};

class PseudoDestructorCallAST: public AbstractExpressionAST
{
public:
    enum { Type = NodeType_PseudoConstructorCall };

public:
    PseudoDestructorCallAST();

private:
    PseudoDestructorCallAST(const PseudoDestructorCallAST &source);
    void operator = (const PseudoDestructorCallAST &source);
};

class ClassMemberAccessAST: public AbstractExpressionAST
{
public:
    enum { Type = NodeType_ClassMemberAccess };

public:
    ClassMemberAccessAST();

    inline AST *op() const
    { return m_op; }

    inline AbstractExpressionAST *expression() const
    { return m_expression; }

    inline NameAST *name() const
    { return m_name; }

    void setOp(AST *op);
    void setExpression(AbstractExpressionAST *expression);
    void setName(NameAST *name);

private:
    AST *m_op;
    AbstractExpressionAST *m_expression;
    AST *m_templ;
    NameAST *m_name;

private:
    ClassMemberAccessAST(const ClassMemberAccessAST &source);
    void operator = (const ClassMemberAccessAST &source);
};

class IncrDecrAST: public AbstractExpressionAST
{
public:
    enum { Type = NodeType_IncrDecr };

public:
    IncrDecrAST();

    inline AST *op() const
    { return m_op; }

    inline AbstractExpressionAST *expression() const
    { return m_expression; }

    void setOp(AST *op);
    void setExpression(AbstractExpressionAST *expression);

private:
    AST *m_op;
    AbstractExpressionAST *m_expression;

private:
    IncrDecrAST(const IncrDecrAST &source);
    void operator = (const IncrDecrAST &source);
};

class CppCastExpressionAST: public AbstractExpressionAST
{
public:
    enum { Type = NodeType_CppCastExpression };

public:
    CppCastExpressionAST();

    inline AST *castOp() const
    { return m_castOp; }

    inline AST *typeId() const
    { return m_typeId; }

    inline AbstractExpressionAST *expression() const
    { return m_expression; }

    void setCastOp(AST *castOp);
    void setTypeId(AST *typeId);
    void setExpression(AbstractExpressionAST *expression);

private:
    AST *m_castOp;
    AST *m_typeId;
    AbstractExpressionAST *m_expression;

private:
    CppCastExpressionAST(const CppCastExpressionAST &source);
    void operator = (const CppCastExpressionAST &source);
};

class TypeIdentificationAST: public AbstractExpressionAST
{
public:
    enum { Type = NodeType_TypeIdentification };

public:
    TypeIdentificationAST();

private:
    TypeIdentificationAST(const TypeIdentificationAST &source);
    void operator = (const TypeIdentificationAST &source);
};

class TypeIdAST: public AST
{
public:
    enum { Type = NodeType_TypeId };

public:
    TypeIdAST();

    inline TypeSpecifierAST *typeSpecifier() const
    { return m_typeSpecifier; }

    inline DeclaratorAST *declarator() const
    { return m_declarator; }

    void setTypeSpecifier(TypeSpecifierAST *typeSpecifier);
    void setDeclarator(DeclaratorAST *declarator);

private:
    TypeSpecifierAST *m_typeSpecifier;
    DeclaratorAST *m_declarator;

private:
    TypeIdAST(const TypeIdAST &source);
    void operator = (const TypeIdAST &source);
};

class StatementAST: public AST
{
public:
    enum { Type = NodeType_Statement };
};

class TemplateArgumentListAST: public AST
{
public:
    enum { Type = NodeType_TemplateArgumentList };

public:
    TemplateArgumentListAST();

    void addArgument(AST *arg);
    inline List<AST *> *argumentList() const { return m_argumentList; }

private:
    List<AST *> *m_argumentList;

private:
    TemplateArgumentListAST(const TemplateArgumentListAST &source);
    void operator = (const TemplateArgumentListAST &source);
};

class ClassOrNamespaceNameAST: public AST
{
public:
    enum { Type = NodeType_ClassOrNamespaceName };

public:
    ClassOrNamespaceNameAST();

    inline AST *name() const { return m_name; }
    void setName(AST *name);

    inline TemplateArgumentListAST *templateArgumentList() const { return m_templateArgumentList; }
    void setTemplateArgumentList(TemplateArgumentListAST *templateArgumentList);

private:
    AST* m_name;
    TemplateArgumentListAST* m_templateArgumentList;

private:
    ClassOrNamespaceNameAST(const ClassOrNamespaceNameAST &source);
    void operator = (const ClassOrNamespaceNameAST &source);
};

class NameAST: public AST
{
public:
    enum { Type = NodeType_Name };

public:
    NameAST();

    inline bool isGlobal() const { return m_global; }
    void setGlobal(bool b);

    void addClassOrNamespaceName(ClassOrNamespaceNameAST *classOrNamespaceName);
    inline List<ClassOrNamespaceNameAST *> *classOrNamespaceNameList() const { return m_classOrNamespaceNameList; }

    inline ClassOrNamespaceNameAST *unqualifiedName() const { return m_unqualifiedName; }
    void setUnqualifiedName(ClassOrNamespaceNameAST *unqualifiedName);

private:
    bool m_global;
    ClassOrNamespaceNameAST* m_unqualifiedName;
    List<ClassOrNamespaceNameAST *> *m_classOrNamespaceNameList;

private:
    NameAST(const NameAST &source);
    void operator = (const NameAST &source);
};

class TypeParameterAST: public AST
{
public:
    enum { Type = NodeType_TypeParameter };

public:
    TypeParameterAST();

    inline AST *kind() const { return m_kind; }
    void setKind(AST *kind);

    inline class TemplateParameterListAST *templateParameterList() const { return m_templateParameterList; }
    void setTemplateParameterList(class TemplateParameterListAST *templateParameterList);

    inline NameAST *name() const { return m_name; }
    void setName(NameAST *name);

    inline AST *typeId() const { return m_typeId; }
    void setTypeId(AST *typeId);

private:
    AST* m_kind;
    class TemplateParameterListAST *m_templateParameterList;
    NameAST* m_name;
    AST* m_typeId;

private:
    TypeParameterAST(const TypeParameterAST &source);
    void operator = (const TypeParameterAST &source);
};

class DeclarationAST: public AST
{
public:
    enum { Type = NodeType_Declaration };

public:
    DeclarationAST();

private:
    DeclarationAST(const DeclarationAST &source);
    void operator = (const DeclarationAST &source);
};

class AccessDeclarationAST: public DeclarationAST
{
public:
    enum { Type = NodeType_AccessDeclaration };

public:
    AccessDeclarationAST();

    inline List<AST *> *accessList() const { return m_accessList; }
    void addAccess(AST *access);

private:
    List<AST *> *m_accessList;

private:
    AccessDeclarationAST(const AccessDeclarationAST &source);
    void operator = (const AccessDeclarationAST &source);
};

class TypeSpecifierAST: public AST
{
public:
    enum { Type = NodeType_TypeSpecifier };

public:
    TypeSpecifierAST();

    inline virtual NameAST *name() const { return m_name; }
    virtual void setName(NameAST *name);

    inline AST *cvQualify() const { return m_cvQualify; }
    void setCvQualify(AST *cvQualify);

    inline AST *cv2Qualify() const { return m_cv2Qualify; }
    void setCv2Qualify(AST *cv2Qualify);

private:
    NameAST* m_name;
    AST* m_cvQualify;
    AST* m_cv2Qualify;

private:
    TypeSpecifierAST(const TypeSpecifierAST &source);
    void operator = (const TypeSpecifierAST &source);
};

class BaseSpecifierAST: public AST
{
public:
    enum { Type = NodeType_BaseSpecifier };

public:
    BaseSpecifierAST();

    inline AST *isVirtual() const { return m_isVirtual; }
    void setIsVirtual(AST *isVirtual);

    inline AST *access() const { return m_access; }
    void setAccess(AST *access);

    inline NameAST *name() const { return m_name; }
    void setName(NameAST *name);

private:
    AST* m_isVirtual;
    AST* m_access;
    NameAST* m_name;

private:
    BaseSpecifierAST(const BaseSpecifierAST &source);
    void operator = (const BaseSpecifierAST &source);
};

class BaseClauseAST: public AST
{
public:
    enum { Type = NodeType_BaseClause };

public:
    BaseClauseAST();

    void addBaseSpecifier(BaseSpecifierAST *baseSpecifier);
    inline List<BaseSpecifierAST *> *baseSpecifierList() const { return m_baseSpecifierList; }

private:
    List<BaseSpecifierAST *> *m_baseSpecifierList;

private:
    BaseClauseAST(const BaseClauseAST &source);
    void operator = (const BaseClauseAST &source);
};

class ClassSpecifierAST: public TypeSpecifierAST
{
public:
    enum { Type = NodeType_ClassSpecifier };

public:
    ClassSpecifierAST();

    inline AST *winDeclSpec() const { return m_winDeclSpec; }
    void setWinDeclSpec(AST *winDeclSpec);

    inline AST *classKey() const { return m_classKey; }
    void setClassKey(AST *classKey);

    inline BaseClauseAST *baseClause() const { return m_baseClause; }
    void setBaseClause(BaseClauseAST *baseClause);

    inline List<DeclarationAST *> *declarationList() const { return m_declarationList; }
    void addDeclaration(DeclarationAST *declaration);

private:
    AST* m_winDeclSpec;
    AST* m_classKey;
    BaseClauseAST* m_baseClause;
    List<DeclarationAST *> *m_declarationList;

private:
    ClassSpecifierAST(const ClassSpecifierAST &source);
    void operator = (const ClassSpecifierAST &source);
};

class EnumeratorAST: public AST
{
public:
    enum { Type = NodeType_Enumerator };

public:
    EnumeratorAST();

    inline AST *id() const { return m_id; }
    void setId(AST *id);

    inline AbstractExpressionAST *expression() const { return m_expression; }
    void setExpression(AbstractExpressionAST *expr);

private:
    AST* m_id;
    AbstractExpressionAST* m_expression;

private:
    EnumeratorAST(const EnumeratorAST &source);
    void operator = (const EnumeratorAST &source);
};

class EnumSpecifierAST: public TypeSpecifierAST
{
public:
    enum { Type = NodeType_EnumSpecifier };

public:
    EnumSpecifierAST();

    void addEnumerator(EnumeratorAST *enumerator);
    inline List<EnumeratorAST *> *enumeratorList() const { return m_enumeratorList; }

private:
    List<EnumeratorAST *> *m_enumeratorList;

private:
    EnumSpecifierAST(const EnumSpecifierAST &source);
    void operator = (const EnumSpecifierAST &source);
};

class ElaboratedTypeSpecifierAST: public TypeSpecifierAST
{
public:
    enum { Type = NodeType_ElaboratedTypeSpecifier };

public:
    ElaboratedTypeSpecifierAST();

    inline AST *kind() const { return m_kind; }
    void setKind(AST *kind);

private:
    AST* m_kind;

private:
    ElaboratedTypeSpecifierAST(const ElaboratedTypeSpecifierAST &source);
    void operator = (const ElaboratedTypeSpecifierAST &source);
};


class LinkageBodyAST: public AST
{
public:
    enum { Type = NodeType_LinkageBody };

public:
    LinkageBodyAST();

    void addDeclaration(DeclarationAST *ast);
    inline List<DeclarationAST *> *declarationList() const { return m_declarationList; }

private:
    List<DeclarationAST *> *m_declarationList;

private:
    LinkageBodyAST(const LinkageBodyAST &source);
    void operator = (const LinkageBodyAST &source);
};

class LinkageSpecificationAST: public DeclarationAST
{
public:
    enum { Type = NodeType_LinkageSpecification };

public:
    LinkageSpecificationAST();

    inline AST *externType() const { return m_externType; }
    void setExternType(AST *externType);

    inline LinkageBodyAST *linkageBody() const { return m_linkageBody; }
    void setLinkageBody(LinkageBodyAST *linkageBody);

    inline DeclarationAST *declaration() const { return m_declaration; }
    void setDeclaration(DeclarationAST *decl);

private:
    AST* m_externType;
    LinkageBodyAST* m_linkageBody;
    DeclarationAST* m_declaration;

private:
    LinkageSpecificationAST(const LinkageSpecificationAST &source);
    void operator = (const LinkageSpecificationAST &source);
};

class NamespaceAST: public DeclarationAST
{
public:
    enum { Type = NodeType_Namespace };

public:
    NamespaceAST();

    inline AST *namespaceName() const { return m_namespaceName; }
    void setNamespaceName(AST *namespaceName);

    inline LinkageBodyAST *linkageBody() const { return m_linkageBody; }
    void setLinkageBody(LinkageBodyAST *linkageBody);

private:
    AST* m_namespaceName;
    LinkageBodyAST* m_linkageBody;

private:
    NamespaceAST(const NamespaceAST &source);
    void operator = (const NamespaceAST &source);
};

class NamespaceAliasAST: public DeclarationAST
{
public:
    enum { Type = NodeType_NamespaceAlias };

public:
    NamespaceAliasAST();

    inline AST *namespaceName() const { return m_namespaceName; }
    void setNamespaceName(AST *name);

    inline NameAST *aliasName() const { return m_aliasName; }
    void setAliasName(NameAST *name);

private:
    AST* m_namespaceName;
    NameAST* m_aliasName;

private:
    NamespaceAliasAST(const NamespaceAliasAST &source);
    void operator = (const NamespaceAliasAST &source);
};

class UsingAST: public DeclarationAST
{
public:
    enum { Type = NodeType_Using };

public:
    UsingAST();

    inline AST *typeName() const { return m_typeName; }
    void setTypeName(AST *typeName);

    inline NameAST *name() const { return m_name; }
    void setName(NameAST *name);

private:
    AST* m_typeName;
    NameAST* m_name;

private:
    UsingAST(const UsingAST &source);
    void operator = (const UsingAST &source);
};

class UsingDirectiveAST: public DeclarationAST
{
public:
    enum { Type = NodeType_UsingDirective };

public:
    UsingDirectiveAST();

    inline NameAST *name() const { return m_name; }
    void setName(NameAST *name);

private:
    NameAST* m_name;

private:
    UsingDirectiveAST(const UsingDirectiveAST &source);
    void operator = (const UsingDirectiveAST &source);
};

class DeclaratorAST: public AST
{
public:
    enum { Type = NodeType_Declarator };

public:
    DeclaratorAST();

    inline List<AST *> *ptrOpList() const { return m_ptrOpList; }
    void addPtrOp(AST *ptrOp);

    inline DeclaratorAST *subDeclarator() const { return m_subDeclarator; }
    void setSubDeclarator(DeclaratorAST *subDeclarator);

    inline NameAST *declaratorId() const { return m_declaratorId; }
    void setDeclaratorId(NameAST *declaratorId);

    inline AST *bitfieldInitialization() const { return m_bitfieldInitialization; }
    void setBitfieldInitialization(AST *bitfieldInitialization);

    inline List<AST *> *arrayDimensionList() const { return m_arrayDimensionList; }
    void addArrayDimension(AST *arrayDimension);

    inline class ParameterDeclarationClauseAST *parameterDeclarationClause() const { return m_parameterDeclarationClause; }
    void setParameterDeclarationClause(class ParameterDeclarationClauseAST  *parameterDeclarationClause);

    // ### replace 'constant' with cvQualify
    inline AST *constant() const { return m_constant; }
    void setConstant(AST *constant);

    inline AST *exceptionSpecification() const { return m_exceptionSpecification; }
    void setExceptionSpecification(AST *exceptionSpecification);

private:
    List<AST *> *m_ptrOpList;
    DeclaratorAST * m_subDeclarator;
    NameAST* m_declaratorId;
    AST* m_bitfieldInitialization;
    List<AST *> *m_arrayDimensionList;
    class ParameterDeclarationClauseAST * m_parameterDeclarationClause;
    AST* m_constant;
    AST* m_exceptionSpecification;

private:
    DeclaratorAST(const DeclaratorAST &source);
    void operator = (const DeclaratorAST &source);
};

class ParameterDeclarationAST: public AST
{
public:
    enum { Type = NodeType_ParameterDeclaration };

public:
    ParameterDeclarationAST();

    inline TypeSpecifierAST *typeSpec() const { return m_typeSpec; }
    void setTypeSpec(TypeSpecifierAST *typeSpec);

    inline DeclaratorAST *declarator() const { return m_declarator; }
    void setDeclarator(DeclaratorAST *declarator);

    inline AbstractExpressionAST *expression() const { return m_expression; }
    void setExpression(AbstractExpressionAST *expression);

private:
    TypeSpecifierAST* m_typeSpec;
    DeclaratorAST* m_declarator;
    AbstractExpressionAST* m_expression;

private:
    ParameterDeclarationAST(const ParameterDeclarationAST &source);
    void operator = (const ParameterDeclarationAST &source);
};

class ParameterDeclarationListAST: public AST
{
public:
    enum { Type = NodeType_ParameterDeclarationList };

public:
    ParameterDeclarationListAST();

    inline List<ParameterDeclarationAST *> *parameterList() const { return m_parameterList; }
    void addParameter(ParameterDeclarationAST *parameter);

private:
    List<ParameterDeclarationAST *> *m_parameterList;

private:
    ParameterDeclarationListAST(const ParameterDeclarationListAST &source);
    void operator = (const ParameterDeclarationListAST &source);
};

class ParameterDeclarationClauseAST: public AST
{
public:
    enum { Type = NodeType_ParameterDeclarationClause };

public:
    ParameterDeclarationClauseAST();

    inline ParameterDeclarationListAST *parameterDeclarationList() const { return m_parameterDeclarationList; }
    void setParameterDeclarationList(ParameterDeclarationListAST *parameterDeclarationList);

    inline AST *ellipsis() const { return m_ellipsis; }
    void setEllipsis(AST *ellipsis);

private:
    ParameterDeclarationListAST* m_parameterDeclarationList;
    AST* m_ellipsis;

private:
    ParameterDeclarationClauseAST(const ParameterDeclarationClauseAST &source);
    void operator = (const ParameterDeclarationClauseAST &source);
};


class InitDeclaratorAST: public AST
{
public:
    enum { Type = NodeType_InitDeclarator };

public:
    InitDeclaratorAST();

    inline DeclaratorAST *declarator() const { return m_declarator; }
    void setDeclarator(DeclaratorAST *declarator);

    inline AST *initializer() const { return m_initializer; }
    void setInitializer(AST *initializer);

private:
    DeclaratorAST* m_declarator;
    AST* m_initializer;

private:
    InitDeclaratorAST(const InitDeclaratorAST &source);
    void operator = (const InitDeclaratorAST &source);
};

class InitDeclaratorListAST: public AST
{
public:
    enum { Type = NodeType_InitDeclaratorList };

public:
    InitDeclaratorListAST();

    inline List<InitDeclaratorAST *> *initDeclaratorList() const { return m_initDeclaratorList; }
    void addInitDeclarator(InitDeclaratorAST *decl);

private:
    List<InitDeclaratorAST *> *m_initDeclaratorList;

private:
    InitDeclaratorListAST(const InitDeclaratorListAST &source);
    void operator = (const InitDeclaratorListAST &source);
};

class TypedefAST: public DeclarationAST
{
public:
    enum { Type = NodeType_Typedef };

public:
    TypedefAST();

    inline TypeSpecifierAST *typeSpec() const { return m_typeSpec; }
    void setTypeSpec(TypeSpecifierAST *typeSpec);

    inline InitDeclaratorListAST *initDeclaratorList() const { return m_initDeclaratorList; }
    void setInitDeclaratorList(InitDeclaratorListAST *initDeclaratorList);

private:
    TypeSpecifierAST* m_typeSpec;
    InitDeclaratorListAST* m_initDeclaratorList;

private:
    void operator = (const TypedefAST &source);
};

class TemplateParameterAST: public AST
{
public:
    enum { Type = NodeType_TemplateParameter };

public:
    TemplateParameterAST();

    inline TypeParameterAST *typeParameter() const { return m_typeParameter; }
    void setTypeParameter(TypeParameterAST *typeParameter);

    inline ParameterDeclarationAST *typeValueParameter() const { return m_typeValueParameter; }
    void setTypeValueParameter(ParameterDeclarationAST *typeValueParameter);

private:
    TypeParameterAST* m_typeParameter;
    ParameterDeclarationAST* m_typeValueParameter;

private:
    TemplateParameterAST(const TemplateParameterAST &source);
    void operator = (const TemplateParameterAST &source);
};

class TemplateParameterListAST: public AST
{
public:
    enum { Type = NodeType_TemplateParameterList };

public:
    TemplateParameterListAST();

    inline List<TemplateParameterAST *> *templateParameterList() const { return m_templateParameterList; }
    void addTemplateParameter(TemplateParameterAST *templateParameter);

private:
    List<TemplateParameterAST *> *m_templateParameterList;

private:
    TemplateParameterListAST(const TemplateParameterListAST &source);
    void operator = (const TemplateParameterListAST &source);
};

class TemplateDeclarationAST: public DeclarationAST
{
public:
    enum { Type = NodeType_TemplateDeclaration };

public:
    TemplateDeclarationAST();

    inline AST *exported() const { return m_exported; }
    void setExported(AST *exported);

    inline TemplateParameterListAST *templateParameterList() const { return m_templateParameterList; }
    void setTemplateParameterList(TemplateParameterListAST *templateParameterList);

    inline DeclarationAST *declaration() const { return m_declaration; }
    void setDeclaration(DeclarationAST *declaration);

private:
    AST* m_exported;
    TemplateParameterListAST* m_templateParameterList;
    DeclarationAST* m_declaration;

private:
    TemplateDeclarationAST(const TemplateDeclarationAST &source);
    void operator = (const TemplateDeclarationAST &source);
};

class SimpleDeclarationAST: public DeclarationAST
{
public:
    enum { Type = NodeType_SimpleDeclaration };

public:
    SimpleDeclarationAST();

    inline AST *functionSpecifier() const { return m_functionSpecifier; }
    void setFunctionSpecifier(AST *functionSpecifier);

    inline AST *storageSpecifier() const { return m_storageSpecifier; }
    void setStorageSpecifier(AST *storageSpecifier);

    inline TypeSpecifierAST *typeSpec() const { return m_typeSpec; }
    void setTypeSpec(TypeSpecifierAST *typeSpec);

    inline InitDeclaratorListAST *initDeclaratorList() const { return m_initDeclaratorList; }
    void setInitDeclaratorList(InitDeclaratorListAST *initDeclaratorList);

    inline AST *winDeclSpec() const { return m_winDeclSpec; }
    void setWinDeclSpec(AST *winDeclSpec);

private:
    AST* m_functionSpecifier;
    AST* m_storageSpecifier;
    TypeSpecifierAST* m_typeSpec;
    InitDeclaratorListAST* m_initDeclaratorList;
    AST* m_winDeclSpec;

private:
    SimpleDeclarationAST(const SimpleDeclarationAST &source);
    void operator = (const SimpleDeclarationAST &source);
};

class ExpressionStatementAST: public StatementAST
{
public:
    enum { Type = NodeType_ExpressionStatement };

public:
    ExpressionStatementAST();

    inline AbstractExpressionAST *expression() const { return m_expression; }
    void setExpression(AbstractExpressionAST *expression);

private:
    AbstractExpressionAST* m_expression;

private:
    ExpressionStatementAST(const ExpressionStatementAST &source);
    void operator = (const ExpressionStatementAST &source);
};

class ReturnStatementAST: public StatementAST
{
public:
    enum { Type = NodeType_ReturnStatement };

public:
    ReturnStatementAST();

    inline AbstractExpressionAST *expression() const { return m_expression; }
    void setExpression(AbstractExpressionAST *expression);

private:
    AbstractExpressionAST* m_expression;

private:
    ReturnStatementAST(const ReturnStatementAST &source);
    void operator = (const ReturnStatementAST &source);
};


class ConditionAST: public AST
{
public:
    enum { Type = NodeType_Condition };

public:
    ConditionAST();

    inline TypeSpecifierAST *typeSpec() const { return m_typeSpec; }
    void setTypeSpec(TypeSpecifierAST *typeSpec);

    inline DeclaratorAST *declarator() const { return m_declarator; }
    void setDeclarator(DeclaratorAST *declarator);

    inline AbstractExpressionAST *expression() const { return m_expression; }
    void setExpression(AbstractExpressionAST *expression);

private:
    TypeSpecifierAST* m_typeSpec;
    DeclaratorAST* m_declarator;
    AbstractExpressionAST* m_expression;

private:
    ConditionAST(const ConditionAST &source);
    void operator = (const ConditionAST &source);
};

class IfStatementAST: public StatementAST
{
public:
    enum { Type = NodeType_IfStatement };

public:
    IfStatementAST();

    inline ConditionAST *condition() const { return m_condition; }
    void setCondition(ConditionAST *condition);

    inline StatementAST *statement() const { return m_statement; }
    void setStatement(StatementAST *statement);

    inline StatementAST *elseStatement() const { return m_elseStatement; }
    void setElseStatement(StatementAST *statement);

private:
    ConditionAST* m_condition;
    StatementAST* m_statement;
    StatementAST* m_elseStatement;

private:
    IfStatementAST(const IfStatementAST &source);
    void operator = (const IfStatementAST &source);
};

class WhileStatementAST: public StatementAST
{
public:
    enum { Type = NodeType_WhileStatement };

public:
    WhileStatementAST();

    inline ConditionAST *condition() const { return m_condition; }
    void setCondition(ConditionAST *condition);

    inline StatementAST *statement() const { return m_statement; }
    void setStatement(StatementAST *statement);

private:
    ConditionAST* m_condition;
    StatementAST* m_statement;

private:
    WhileStatementAST(const WhileStatementAST &source);
    void operator = (const WhileStatementAST &source);
};

class DoStatementAST: public StatementAST
{
public:
    enum { Type = NodeType_DoStatement };

public:
    DoStatementAST();

    inline ConditionAST *condition() const { return m_condition; }
    void setCondition(ConditionAST *condition);

    inline StatementAST *statement() const { return m_statement; }
    void setStatement(StatementAST *statement);

private:
    ConditionAST* m_condition;
    StatementAST* m_statement;

private:
    DoStatementAST(const DoStatementAST &source);
    void operator = (const DoStatementAST &source);
};

class ForStatementAST: public StatementAST
{
public:
    enum { Type = NodeType_ForStatement };

public:
    ForStatementAST();

    inline StatementAST *initStatement() const { return m_initStatement; }
    void setInitStatement(StatementAST *statement);

    inline ConditionAST *condition() const { return m_condition; }
    void setCondition(ConditionAST *condition);

    inline AbstractExpressionAST *expression() const { return m_expression; }
    void setExpression(AbstractExpressionAST *expression);

    inline StatementAST *statement() const { return m_statement; }
    void setStatement(StatementAST *statement);

private:
    ConditionAST* m_condition;
    StatementAST* m_initStatement;
    StatementAST* m_statement;
    AbstractExpressionAST* m_expression;

private:
    ForStatementAST(const ForStatementAST &source);
    void operator = (const ForStatementAST &source);
};

class SwitchStatementAST: public StatementAST
{
public:
    enum { Type = NodeType_SwitchStatement };

public:
    SwitchStatementAST();

    inline ConditionAST *condition() const { return m_condition; }
    void setCondition(ConditionAST *condition);

    inline StatementAST *statement() const { return m_statement; }
    void setStatement(StatementAST *statement);

private:
    ConditionAST* m_condition;
    StatementAST* m_statement;

private:
    SwitchStatementAST(const SwitchStatementAST &source);
    void operator = (const SwitchStatementAST &source);
};

class StatementListAST: public StatementAST
{
public:
    enum { Type = NodeType_StatementList };

public:
    StatementListAST();

    inline List<StatementAST *> *statementList() const { return m_statementList; }
    void addStatement(StatementAST *statement);

private:
    List<StatementAST *> *m_statementList;

private:
    StatementListAST(const StatementListAST &source);
    void operator = (const StatementListAST &source);
};

class DeclarationStatementAST: public StatementAST
{
public:
    enum { Type = NodeType_DeclarationStatement };

public:
    DeclarationStatementAST();

    inline DeclarationAST *declaration() const { return m_declaration; }
    void setDeclaration(DeclarationAST *declaration);

private:
    DeclarationAST* m_declaration;

private:
    DeclarationStatementAST(const DeclarationStatementAST &source);
    void operator = (const DeclarationStatementAST &source);
};

/*
    LabeledStatementAST:
        case constant-expression : statement
        default : statement (expression is 0)
*/
class LabeledStatementAST: public StatementAST
{
public:
    enum { Type = NodeType_LabeledStatement };
public:
    LabeledStatementAST();

    inline StatementAST *statement() const { return m_statement; }
    void setStatement(StatementAST *statement);

    inline AbstractExpressionAST *expression() const { return m_expression; }
    void setExpression(AbstractExpressionAST *expression);
private:
    StatementAST* m_statement;
    AbstractExpressionAST *m_expression;

private:
    LabeledStatementAST(const LabeledStatementAST &source);
    void operator = (const LabeledStatementAST &source);
};

class FunctionDefinitionAST: public DeclarationAST
{
public:
    enum { Type = NodeType_FunctionDefinition };

public:
    FunctionDefinitionAST();

    inline AST *functionSpecifier() const { return m_functionSpecifier; }
    void setFunctionSpecifier(AST *functionSpecifier);

    inline AST *storageSpecifier() const { return m_storageSpecifier; }
    void setStorageSpecifier(AST *storageSpecifier);

    inline TypeSpecifierAST *typeSpec() const { return m_typeSpec; }
    void setTypeSpec(TypeSpecifierAST *typeSpec);

    inline InitDeclaratorAST *initDeclarator() const { return m_initDeclarator; }
    void setInitDeclarator(InitDeclaratorAST *initDeclarator);

    inline StatementListAST *functionBody() const { return m_functionBody; }
    void setFunctionBody(StatementListAST *functionBody);

    inline AST *winDeclSpec() const { return m_winDeclSpec; }
    void setWinDeclSpec(AST *winDeclSpec);

private:
    AST* m_functionSpecifier;
    AST* m_storageSpecifier;
    TypeSpecifierAST* m_typeSpec;
    InitDeclaratorAST* m_initDeclarator;
    StatementListAST* m_functionBody;
    AST* m_winDeclSpec;

private:
    FunctionDefinitionAST(const FunctionDefinitionAST &source);
    void operator = (const FunctionDefinitionAST &source);
};

class TranslationUnitAST: public AST
{
public:
    enum { Type = NodeType_TranslationUnit };

public:
    TranslationUnitAST();

    void addDeclaration(DeclarationAST *ast);
    inline List<DeclarationAST *> *declarationList() const { return m_declarationList; }

private:
    List<DeclarationAST *> *m_declarationList;

private:
    TranslationUnitAST(const TranslationUnitAST &source);
    void operator = (const TranslationUnitAST &source);
};

template <class T> T* CreateNode(pool *p)
{
    T* node = new (p->allocate(sizeof(T))) T;
    node->setNodeType(T::Type);
    node->_pool = p;
    return node;
}

template <int kind> ExpressionAST<kind> *CreateExpression(pool *p)
{
    ExpressionAST<kind>* node = new (p->allocate(sizeof(ExpressionAST<kind>))) ExpressionAST<kind>;
    node->setNodeType(kind);
    node->_pool = p;
    return node;
}

/*
template <typename T>
inline List<T *> *snoc(List<T *> *e, T *d, pool *p)
{ if (!e) e = new (p->allocate(sizeof(List<T*>))) List<T *>(p); e->append(d); return e; }
*/

//Workaround for ICE on MSVC, use macro instead of template.
#define SNOC(ListType, ListValueType) \
inline ListType *snoc(ListType *e, ListValueType *d, pool *p) \
{ if (!e) e = new (p->allocate(sizeof(ListType))) ListType(p); e->append(d); return e; }

SNOC(List<AST *>, AST)
SNOC(List<ClassOrNamespaceNameAST *>, ClassOrNamespaceNameAST)
SNOC(List<BaseSpecifierAST *>, BaseSpecifierAST)
SNOC(List<DeclarationAST *>, DeclarationAST)
SNOC(List<EnumeratorAST *>, EnumeratorAST)
SNOC(List<ParameterDeclarationAST *>, ParameterDeclarationAST)
SNOC(List<InitDeclaratorAST *>, InitDeclaratorAST)
SNOC(List<TemplateParameterAST *>, TemplateParameterAST)
SNOC(List<StatementAST *>, StatementAST)

QT_END_NAMESPACE

#endif // AST_H
