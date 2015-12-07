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

#include "smallobject.h"
#include "tokenengine.h"
#include "semantic.h"
#include <QtDebug>
#include <QString>
#include <QRegExp>

QT_BEGIN_NAMESPACE

using namespace TokenStreamAdapter;
using namespace TokenEngine;
using namespace CodeModel;

Semantic::Semantic(CodeModel::NamespaceScope *globalScope,
                   TokenStreamAdapter::TokenStream *tokenStream,
                   TypedPool<CodeModel::Item> *storage)
{
    m_storage = storage;
    m_tokenStream = tokenStream;

    m_currentAccess = CodeModel::Member::Public;
    m_inSlots = false;
    m_inSignals = false;
    m_inStorageSpec = false;
    m_inTypedef = false;

    globalScope->setName("::");
    currentScope.push(globalScope);

    //create global UnknownType and UnknownTypeMember
    UnknownType *type = Create<UnknownType>(m_storage);
    type->setName("__UnknownType");
    globalScope->addType(type);
    type->setParent(globalScope);

    m_sharedUnknownMember = Create<TypeMember>(m_storage);
    m_sharedUnknownMember->setNameToken(TokenRef());
    m_sharedUnknownMember->setName("Unknown");
    m_sharedUnknownMember->setType(type);
    globalScope->addMember(m_sharedUnknownMember);
    m_sharedUnknownMember->setParent(globalScope);

}

void Semantic::parseAST(TranslationUnitAST *node)
{
    TreeWalker::parseTranslationUnit(node);
}


void Semantic::parseLinkageSpecification(LinkageSpecificationAST *ast)
{
    if(!ast)
        return;
    int inStorageSpec = m_inStorageSpec;
    m_inStorageSpec = true;
    TreeWalker::parseLinkageSpecification(ast);
    m_inStorageSpec = inStorageSpec;
}

void Semantic::parseNamespace(NamespaceAST *ast)
{
    CodeModel::NamespaceScope *parent = currentScope.top()->toNamespaceScope();
    if(!parent->toNamespaceScope()) {
        emit error("Error in Semantic::parseNamespace: parent scope was not a namespace");
        return;
    }

    QByteArray nsName;
    if (!ast->namespaceName() || textOf(ast->namespaceName()).isEmpty()){
        nsName = "(__QT_ANON_NAMESPACE)";
    } else {
        nsName = textOf(ast->namespaceName());
    }

    CodeModel::NamespaceScope *namespaceScope = 0;

    // Look up namespace scope in case it is already defined.
    // (Unlike classes, C++ namespaces are "open" and can be added to.)
    CodeModel::Scope *scope = parent->scopes().value(nsName);
    if (scope)
        namespaceScope = scope->toNamespaceScope();

    // Create new namespace if not found.
    if (!namespaceScope) {
        namespaceScope = CodeModel::Create<CodeModel::NamespaceScope>(m_storage);
        namespaceScope->setName(nsName);
        parent->addScope(namespaceScope);

        NamespaceMember *namespaceMember = Create<NamespaceMember>(m_storage);
        namespaceMember->setNameToken(tokenRefFromAST(ast->namespaceName()));
        namespaceMember->setName(nsName);
        namespaceMember->setNamespaceScope(namespaceScope);
        currentScope.top()->addMember(namespaceMember);
        namespaceMember->setParent(currentScope.top());
    }

    currentScope.push(namespaceScope);
    TreeWalker::parseNamespace(ast);
    currentScope.pop();
}

void Semantic::parseClassSpecifier(ClassSpecifierAST *ast)
{
    if (!ast->name()){
        return;
    }

    QByteArray kind = textOf(ast->classKey());
    if (kind == "class")
        m_currentAccess = CodeModel::Member::Private;
    else // kind =="struct"
        m_currentAccess = CodeModel::Member::Public;

    QByteArray className = textOf(ast->name()->unqualifiedName());

    //create ClassScope
    CodeModel::ClassScope *klass = CodeModel::Create<CodeModel::ClassScope>(m_storage);
    klass->setName(className);
    currentScope.top()->addScope(klass);

    //create ClassType
    CodeModel::ClassType *type = CodeModel::Create<CodeModel::ClassType>(m_storage);
    type->setScope(klass);
    currentScope.top()->addType(type);
    type->setParent(currentScope.top());

    //create TypeMember
    CodeModel::TypeMember *typeMember = CodeModel::Create<CodeModel::TypeMember>(m_storage);
    typeMember->setNameToken(tokenRefFromAST(ast->name()->unqualifiedName()));
    typeMember->setName(className);
    typeMember->setType(type);
    currentScope.top()->addMember(typeMember);
    typeMember->setParent(currentScope.top());

    currentScope.push(klass);
    if (ast->baseClause())
        parseBaseClause(ast->baseClause(), klass);

    //TreeWalker::parseClassSpecifier(ast);
    parseNode(ast->winDeclSpec());
    parseNode(ast->classKey());
    parseNode(ast->baseClause());

    // Here's the trick for parsing c++ classes:
    // All inline function definitions must be interpreted as if they were
    // written after any other declarations in the class.
    QList<DeclarationAST *> functionDefinitions;
    if (ast->declarationList())
        foreach(DeclarationAST *decl, *ast->declarationList()) {
            if(decl->nodeType() == NodeType_FunctionDefinition)
                functionDefinitions.append(decl);
            else
            parseNode(decl);
        }
    foreach(DeclarationAST *decl, functionDefinitions)
        parseNode(decl);

    currentScope.pop();
}
/*
    Parse a class, struct or enum forward decalration.
*/
void Semantic::parseElaboratedTypeSpecifier(ElaboratedTypeSpecifierAST *node)
{
    if (!node)
        return;
    AST *kind = node->kind();
    if (!kind)
        return;

    const QByteArray kindText = textOf(kind);
    const QByteArray nameText = textOf(node->name());

    // Don't do anything if the class, struct or enum has already been declared or defined.
    if (lookupNameInScope(currentScope.top(), node->name()).count() > 0)
        return;

    if (kindText == "class" || kindText == "struct") {
        // Create ClassType.
        CodeModel::ClassType *type = CodeModel::Create<CodeModel::ClassType>(m_storage);
        type->setScope(0);
        currentScope.top()->addType(type);
        type->setParent(currentScope.top());

        // Create TypeMember.
        CodeModel::TypeMember *typeMember = CodeModel::Create<CodeModel::TypeMember>(m_storage);
        typeMember->setNameToken(tokenRefFromAST(node->name()->unqualifiedName()));
        typeMember->setName(nameText);
        typeMember->setType(type);
        currentScope.top()->addMember(typeMember);
        typeMember->setParent(currentScope.top());
    } else if (kindText == "enum") {
        //create a Type
        CodeModel::EnumType *enumType = CodeModel::Create<CodeModel::EnumType>(m_storage);
        enumType->setName(nameText);
        currentScope.top()->addType(enumType);
        enumType->setParent(currentScope.top());

        //create a TypeMember
        CodeModel::TypeMember *typeMember = CodeModel::Create<CodeModel::TypeMember>(m_storage);
        if(node->name())
            typeMember->setNameToken(tokenRefFromAST(node->name()->unqualifiedName()));
        typeMember->setName(nameText);
        typeMember->setType(enumType);
        currentScope.top()->addMember(typeMember);
        typeMember->setParent(currentScope.top());
    }
}

void Semantic::parseSimpleDeclaration(SimpleDeclarationAST *ast)
{
    TypeSpecifierAST *typeSpec = ast->typeSpec();
    InitDeclaratorListAST *declarators = ast->initDeclaratorList();

    if (typeSpec)
        parseTypeSpecifier(typeSpec);

    if (declarators){
        List<InitDeclaratorAST*> l = *declarators->initDeclaratorList();

        foreach (InitDeclaratorAST *current, l) {
            parseDeclaration(ast->functionSpecifier(), ast->storageSpecifier(), typeSpec, current);
        }
    }
}

void Semantic::parseDeclaration(AST *funSpec, AST *storageSpec, TypeSpecifierAST *typeSpec, InitDeclaratorAST *decl)
{
    if (m_inStorageSpec)
            return;

    if(!decl)
        return;

    DeclaratorAST *d = decl->declarator();
    if (!d)
        return;

    if (!d->subDeclarator() && d->parameterDeclarationClause()) {
        parseFunctionDeclaration(funSpec, storageSpec, typeSpec, decl);
		return;
	}
    if(!typeSpec || !typeSpec->name())
        return;

    DeclaratorAST *t = d;
    while (t && t->subDeclarator())
        t = t->subDeclarator();

    QByteArray id;
    if (t && t->declaratorId() && t->declaratorId()->unqualifiedName())
        id = textOf(t->declaratorId()->unqualifiedName());

    if (!t || !t->declaratorId() || !t->declaratorId()->unqualifiedName())
        return;
    AST *nameAST = t->declaratorId()->unqualifiedName();
    QByteArray name = textOf(nameAST);


    if (!scopeOfDeclarator(d, QList<QByteArray>()).isEmpty()){
        return;
    }

    //Check if this is possibly a function call by searching for '(' and ')'
    const QByteArray declText = textOf(decl);
    if (declText.contains("(") && declText.contains(")")) {
	if (decl->declarator() && decl->declarator()->subDeclarator()) {

        NameAST * name = decl->declarator()->subDeclarator()->declaratorId();
	if (name)
            parseNameUse(name);
	    return;	
        } 
    }

    //create VariableMember
    CodeModel::VariableMember *variableMember = CodeModel::Create<CodeModel::VariableMember>(m_storage);
    variableMember->setNameToken(tokenRefFromAST(nameAST));
    variableMember->setName(name);
    variableMember->setAccess(m_currentAccess);
    variableMember->setParent(currentScope.top());
    currentScope.top()->addMember(variableMember);

    //look up type of variableMember,

    TypeMember *typeMember = typeLookup(currentScope.top(), typeSpec->name());
    if(typeMember) {
        variableMember->setType(typeMember->type());
    } else {
        QByteArray text = typeOfDeclaration(typeSpec, d);
        CodeModel::UnknownType *type = CodeModel::Create<CodeModel::UnknownType>(m_storage);
        type->setName(text);
        variableMember->setType(type);
    }

    if (decl)
        parseNode(decl->initializer());

}

void Semantic::parseFunctionDeclaration(AST *funSpec, AST *storageSpec,
                                        TypeSpecifierAST * typeSpec, InitDeclaratorAST * initDeclarator)
{
    bool isFriend = false;
    bool isVirtual = false;
    bool isStatic = false;
    bool isInline = false;
    bool isPure = initDeclarator->initializer() != 0;

    if (funSpec){
        List<AST*> l = *funSpec->children();
        foreach (AST *current, l) {
            QByteArray text = textOf(current);
            if (text == "virtual") isVirtual = true;
            else if (text == "inline") isInline = true;
        }
    }

    if (storageSpec){
        List<AST*> l = *storageSpec->children();
        foreach (AST *current, l) {
            QByteArray text = textOf(current);
            if (text == "friend") isFriend = true;
            else if (text == "static") isStatic = true;
        }
    }
    DeclaratorAST *declarator = initDeclarator->declarator();
    if(!declarator || !declarator->declaratorId())
        return;
    AST *nameAST = declarator->declaratorId()->unqualifiedName();
    QByteArray name = textOf(nameAST);

    CodeModel::FunctionMember *method = CodeModel::Create<CodeModel::FunctionMember>(m_storage);
    method->setNameToken(tokenRefFromAST(nameAST));
    method->setName(name);
    method->setAccess(m_currentAccess);
    method->setStatic(isStatic);
    method->setVirtual(isVirtual);
    method->setAbstract(isPure);

    parseFunctionArguments(declarator, method);

    if (m_inSignals)
        method->setSignal(true);

    if (m_inSlots)
        method->setSlot(true);

    method->setConstant(declarator->constant() != 0);

    QByteArray text = typeOfDeclaration(typeSpec, declarator);
    if (!text.isEmpty()) {
        CodeModel::UnknownType *type = CodeModel::Create<CodeModel::UnknownType>(m_storage);
        type->setName(text);
        method->setReturnType(type);
    }

    method->setParent(currentScope.top());
    currentScope.top()->addMember(method);
}


void Semantic::parseBaseClause(BaseClauseAST * baseClause, CodeModel::ClassScope *klass)
{
    if(!baseClause)
        return;
    if(!klass)
        return;
    List<BaseSpecifierAST*> *l = baseClause->baseSpecifierList();
    if (!l)
        return;
    foreach (BaseSpecifierAST *baseSpecifier, *l) {
        QByteArray baseName;
        if (!baseSpecifier->name())
            continue;

        // Look up a class with the correct name.
        QList<Member *> candidates = nameLookup(klass, baseSpecifier->name());
        if (candidates.count() == 1 ) {
            Member *member = candidates.at(0);
            Q_ASSERT(member);
            TypeMember *typeMember = member->toTypeMember();
            if (typeMember) {
                Q_ASSERT(typeMember->type());
                ClassType *classType = typeMember->type()->toClassType();
                if (classType) {
                    klass->addBaseClass(classType);
                }
            }
        }
    }
}
void Semantic::parseFunctionArguments(const DeclaratorAST *declarator, CodeModel::FunctionMember *method)
{
    if(!declarator || !method)
        return;

    ParameterDeclarationClauseAST *clause = declarator->parameterDeclarationClause();

    if (clause && clause->parameterDeclarationList()){
        ParameterDeclarationListAST *params = clause->parameterDeclarationList();
        List<ParameterDeclarationAST*> *l = params->parameterList();
        if (!l)
            return;
        foreach (ParameterDeclarationAST *param, *l) {
            CodeModel::Argument *arg = CodeModel::Create<CodeModel::Argument>(m_storage);
            arg->setParent(method);

            if (param->declarator()){
                QByteArray text = declaratorToString(param->declarator(), QByteArray(), true);
                if(param->declarator()->declaratorId())
                    arg->setNameToken(tokenRefFromAST(param->declarator()->declaratorId()->unqualifiedName()));
                if (!text.isEmpty())
                    arg->setName(text);
            }

            QByteArray tp = typeOfDeclaration(param->typeSpec(), param->declarator());
            if (!tp.isEmpty()) {
                CodeModel::UnknownType *type = CodeModel::Create<CodeModel::UnknownType>(m_storage);
                type->setName(tp);
                arg->setType(type);
            }

            method->addArgument(arg);
        }
    }
}

// using directive (using namespace A)
void Semantic::parseUsingDirective(UsingDirectiveAST *ast)
{
    QByteArray qualifiedname = textOf(ast->name());
    QByteArray name = textOf(ast->name()->unqualifiedName());

    //look up target namespace name
    QList<Member *> memberList = nameLookup(currentScope.top(), ast->name());

    NamespaceScope *targetNamespace = 0;

    // search for namespace in member list.
    QList<Member *>::ConstIterator it = memberList.constBegin();
    while(it != memberList.constEnd()) {
        if (NamespaceMember *namespaceMember = (*it)->toNamespaceMember()) {
            targetNamespace = namespaceMember->namespaceScope();
            break;
        }
        ++it;
    }

    if (targetNamespace == 0)
        return;

    // Find the insertion namespace, which is the first common
    // ancesotor namespace for the current scope and the target namespace

    // currentScope might be a block scope, find its first namespace parent
    CodeModel::Scope *currentParent = currentScope.top();
    while (currentParent->toNamespaceScope() == 0) {
        currentParent = currentParent->parent();
    }

    CodeModel::Scope *namespaceA = currentParent;
    while (namespaceA != 0) {
        CodeModel::Scope *namespaceB = targetNamespace;
        while (namespaceB != 0) {
            if (namespaceB == namespaceA)
                break;
            namespaceB = namespaceB->parent();
        }
        if (namespaceB == namespaceA)
            break;
        namespaceA = namespaceA->parent();
    }

    if (namespaceA == 0 || namespaceA->toNamespaceScope() == 0)
        return;

    NamespaceScope *insertionNamespace = namespaceA->toNamespaceScope();

    // Create using directive link
    UsingDirectiveLink *usingDirectiveLink = Create<UsingDirectiveLink>(m_storage);
    usingDirectiveLink->setParent(currentScope.top());
    usingDirectiveLink->setTargetNamespace(targetNamespace);
    usingDirectiveLink->setInsertionNamespace(insertionNamespace);

    // add it to current namespace
    if (NamespaceScope *namespaceScope = currentScope.top()->toNamespaceScope())
        namespaceScope->addUsingDirectiveLink(usingDirectiveLink);
    else if (BlockScope *blockScope = currentScope.top()->toBlockScope())
        blockScope->addUsingDirectiveLink(usingDirectiveLink);
}

void Semantic::parseFunctionDefinition(FunctionDefinitionAST *ast)
{
    AST *funSpec = ast->functionSpecifier();
    AST *storageSpec = ast->storageSpecifier();
    TypeSpecifierAST *typeSpec = ast->typeSpec();
    InitDeclaratorAST *initDeclarator = ast->initDeclarator();
    if (!ast->initDeclarator())
        return;

    DeclaratorAST *d = initDeclarator->declarator();

    if (!d->declaratorId())
        return;

    parseFunctionDeclaration(funSpec, storageSpec, typeSpec, initDeclarator);
    CodeModel::FunctionMember *method = functionLookup(currentScope.top(), d);

    if(!method) {
        emit error("Error in Semantic::parseFunctionDefinition: Could not find declaration for function definition");
        return;
    }

    CodeModel::Scope *parent = method->parent();

    if(!ast->functionBody()) {
        emit error("Error in Semantic::parseFunctionDefinition: no function body in function definition");
        return;
    }

    //create child function scope
    QByteArray id = textOf(d->declaratorId()->unqualifiedName());
    CodeModel::BlockScope *functionScope = CodeModel::Create<CodeModel::BlockScope>(m_storage);
    functionScope->setName(QByteArray("__QT_ANON_BLOCK_SCOPE(Function: ") + id + QByteArray(")"));
    functionScope->setParent(parent);
    method->setFunctionBodyScope(functionScope);

    //add arguments to child scope
     ArgumentCollection arguments = method->arguments();
     ArgumentCollection::ConstIterator it = arguments.constBegin();
     while(it != arguments.constEnd()) {
         CodeModel::Argument *argument = *it;
         CodeModel::VariableMember *variableMember = CodeModel::Create<CodeModel::VariableMember>(m_storage);
         variableMember->setNameToken(argument->nameToken());
         variableMember->setType(argument->type());
         variableMember->setName(argument->name());
         variableMember->setParent(functionScope);
         functionScope->addMember(variableMember);
         ++it;
     }

    //push function scope and parse function body
    currentScope.push(functionScope);
    parseStatementList(ast->functionBody());
    currentScope.pop();
}

void Semantic::parseStatementList(StatementListAST *statemenList)
{
    if(!statemenList)
        return;
    CodeModel::BlockScope *blockScope = CodeModel::Create<CodeModel::BlockScope>(m_storage);
    blockScope->setName("__QT_ANON_BLOCK_SCOPE");
    blockScope->setParent(currentScope.top());
    currentScope.top()->addScope(blockScope);

    currentScope.push(blockScope);
    TreeWalker::parseStatementList(statemenList);
    currentScope.pop();
}

void Semantic::parseExpression(AbstractExpressionAST* node)
{
    if(!node)
        return;
    if(node->nodeType() == NodeType_ClassMemberAccess)
        parseClassMemberAccess(static_cast<ClassMemberAccessAST *>(node));
    else
        TreeWalker::parseExpression(node);
}

/*
    Pretty hardwired code for handling class member access of the types:
    object.member and objectPtr->member.

    This function creates a name use for object to its declaration, and a
    name use from member to its declaration in the class.
*/
void Semantic::parseClassMemberAccess(ClassMemberAccessAST *node)
{
    if(!node)
        return;
    parseExpression(node->expression());
    // Get a name use for the 'object' name.
    NameUse *nameUse = findNameUse(node->expression());
    // Since the NameUse refers to an object, its decalaration must be
    // a ClassType. Get the scope of this class type.
    if(    nameUse
        && nameUse->declaration()
        && nameUse->declaration()->toVariableMember()
        && nameUse->declaration()->toVariableMember()->type()
        && nameUse->declaration()->toVariableMember()->type()->toClassType()
        && nameUse->declaration()->toVariableMember()->type()->toClassType()->scope())   {

        CodeModel::Scope *scope = nameUse->declaration()->toVariableMember()->type()->toClassType()->scope();
        QList<CodeModel::Member *> members = lookupNameInScope(scope, node->name());
            if(members.count() != 0) {
                createNameUse(members.at(0), node->name());
                return;
            }
    }
    // Create a NameUse that refers to the global shared unknown type.
    createNameUse(m_sharedUnknownMember, node->name());
}

void Semantic::parseExpressionStatement(ExpressionStatementAST *node)
{
    TreeWalker::parseExpressionStatement(node);
}

// using declaration (using A::b)
void Semantic::parseUsing(UsingAST *ast)
{
    //CodeModel::Scope *s = lookUpScope(currentScope.top(), ast->name());
    QList<CodeModel::Member *> members = nameLookup(currentScope.top(), ast->name());
    if(members.isEmpty()) {
        emit error("Error in Semantic::parseUsing: could not look up using target");
        return;
    }
    //TODO: handle multiple members (when nameLookup returns a set of overloded functions)
    CodeModel::Member *member = members[0];
    CodeModel::Scope *targetScope = member->parent();
    if(!targetScope) {
        emit error("Error in Semantic::parseUsing: target has no parent scope");
        return;
    }

    if(!ast->name())
        return;
    AST *nameAST = ast->name()->unqualifiedName();
    if(!nameAST)
        return;
    QByteArray name = textOf(nameAST);
}

void Semantic::parseEnumSpecifier(EnumSpecifierAST *ast)
{
    if (!ast->name())
         return;

    QByteArray name = textOf(ast->name());

    //create a Type
    CodeModel::EnumType *enumType = CodeModel::Create<CodeModel::EnumType>(m_storage);
    enumType->setName(name);
    currentScope.top()->addType(enumType);
    enumType->setParent(currentScope.top());

    //create a TypeMember
    CodeModel::TypeMember *typeMember = CodeModel::Create<CodeModel::TypeMember>(m_storage);
    if(ast->name())
        typeMember->setNameToken(tokenRefFromAST(ast->name()->unqualifiedName()));
    typeMember->setName(name);
    typeMember->setType(enumType);
    currentScope.top()->addMember(typeMember);
    typeMember->setParent(currentScope.top());

    //parse the eneumerators
    List<EnumeratorAST*> *list = ast->enumeratorList();
    if (!list)
        return;
    foreach (EnumeratorAST *current, *list) {
        CodeModel::VariableMember *enumerator = CodeModel::Create<CodeModel::VariableMember>(m_storage);
        enumerator->setNameToken(tokenRefFromAST(current->id()));
        enumerator->setName(textOf(current->id()));
        enumerator->setAccess(m_currentAccess);
        enumerator->setStatic(true);
        enumerator->setType(enumType);
        currentScope.top()->addMember(enumerator);
        enumerator->setParent(currentScope.top());
    }

}

void Semantic::parseTypedef(TypedefAST *ast)
{
    TypeSpecifierAST *typeSpec = ast->typeSpec();
    InitDeclaratorListAST *declarators = ast->initDeclaratorList();

    if (typeSpec && declarators){
        QByteArray typeId;

        if (typeSpec->name())
            typeId = textOf(typeSpec->name());

        List<InitDeclaratorAST*> *l = declarators->initDeclaratorList();
        if (!l)
            return;
        foreach (InitDeclaratorAST *initDecl, *l) {
            QByteArray type, id;
            if (initDecl->declarator()){
               type = typeOfDeclaration(typeSpec, initDecl->declarator());

               DeclaratorAST *d = initDecl->declarator();
               while (d->subDeclarator()){
                   d = d->subDeclarator();
               }

               if (d->declaratorId())
                  id = textOf(d->declaratorId());
            }

            //create a type
            CodeModel::Scope *scope = currentScope.top();
            CodeModel::AliasType *typeAlias = CodeModel::Create<CodeModel::AliasType>(m_storage);
            //typeAlias->setName(id);
            //typeAlias->setParent(scope);
            scope->addType(typeAlias);

            //create a TypeMember
            CodeModel::TypeMember *typeMember = CodeModel::Create<CodeModel::TypeMember>(m_storage);
            if(typeSpec->name())
                typeMember->setNameToken(tokenRefFromAST(typeSpec->name()->unqualifiedName()));
            typeMember->setName(id);
            typeMember->setType(typeAlias);
            currentScope.top()->addMember(typeMember);
            typeMember->setParent(currentScope.top());

        }

    }
}

void Semantic::parseTypeSpecifier(TypeSpecifierAST *ast)
{
    // If this is a classSpecifier or a EnumSpecifier we skip the name lookup,
    // becuase looking up the name "E" in a class definition like
    // "class E { ..." makes no sense. (There might be a variable named E
    // already declared, but that variable is now shadowed by the class type.)
    if(   ast->nodeType() != NodeType_EnumSpecifier
       && ast->nodeType() != NodeType_ClassSpecifier
       && ast->nodeType() != NodeType_ElaboratedTypeSpecifier )
        parseNameUse(ast->name());
    TreeWalker::parseTypeSpecifier(ast);
}

/*
    Parses a name: looks up name, creates name use.
*/
void Semantic::parseNameUse(NameAST* name)
{
    if(!name)
        return;

    // Look up name
    QList<CodeModel::Member *> members = nameLookup(currentScope.top(), name);
    if(members.isEmpty()) {
        //cout << "no declaration found for " << textOf(name).constData() << endl;
        // Create NameUse that refer to a shared UnknownMember
        createNameUse(m_sharedUnknownMember, name);
        return;
    }

    //TODO: handle multiple members (when nameLookup returns a set of overloaded functions)
    CodeModel::Member *member = members[0];
    if(!member->parent()) {
        emit error("Error in Semantic::parseUsing: target has no parent scope");
        return;
    }

    createNameUse(member, name);
}

/*
    looks up name used in basescope. If name->isGlobal() is true or if classOrNamespaceList()
    returns a non-emty list, the C++ qualified name lookup rules are used. Otherwise the
    unquialified name lookup rules are used.  Returns the a list of members that was found,
    In most cases this list will contain zero or one element, exept in the case of overloaded functions.
    TODO: Argument-dependent name lookup
*/
QList<CodeModel::Member *> Semantic::nameLookup(CodeModel::Scope *baseScope, const NameAST* name)
{
    if (name->isGlobal() || (name->classOrNamespaceNameList()
                              && name->classOrNamespaceNameList()->size()>0 )) {
        return qualifiedNameLookup(baseScope, name);
    } else {
        return unqualifiedNameLookup(baseScope, name);
    }
}

//look up an unqualified name
QList<CodeModel::Member *> Semantic::unqualifiedNameLookup(CodeModel::Scope *baseScope, const NameAST* name)
{
    QList<UsingDirectiveLink *> usingDirectiveLinks;
    CodeModel::Scope *currentScope = baseScope;
    QList<CodeModel::Member *>  entities;

    while (currentScope != 0) {
        // Add any "using namespace" directive links for the current scope to
        // usingDirectiveLinks
        if (NamespaceScope *namespaceScope = currentScope->toNamespaceScope())
            usingDirectiveLinks += namespaceScope->usingDirectiveLinks();
        if (BlockScope *blockScope = currentScope->toBlockScope())
            usingDirectiveLinks += blockScope->usingDirectiveLinks();

        // Search usingDirectiveLinks for a link where currentScope is the
        // insertion namespace. If found look up name in the target namespace
        // for that link.
        if (NamespaceScope *namespaceScope = currentScope->toNamespaceScope()) {
            QList<UsingDirectiveLink *>::ConstIterator it = usingDirectiveLinks.constBegin();
            while (it != usingDirectiveLinks.constEnd()) {
                if ((*it)->insertionNamespace() == namespaceScope)
                    entities = lookupNameInScope((*it)->targetNamespace(), name);
                ++it;
            }
        }

        // Look up names in this scope.
        entities += lookupNameInScope(currentScope, name);
        if (!entities.isEmpty())
            break;
        currentScope = currentScope->parent();
    }
    return entities;
}

//look up a qualified name
QList<CodeModel::Member *> Semantic::qualifiedNameLookup(CodeModel::Scope *baseScope, const NameAST* name)
{
    QList<CodeModel::Member *> entities;
    CodeModel::Scope *currentScope = baseScope;

    // Check if the global ("::") scope has been specified.
    if(name->isGlobal()) {
        while (currentScope->parent())
            currentScope = currentScope->parent();
    }

    while (entities.isEmpty() && currentScope != 0) {
        CodeModel::Scope *targetScope = scopeLookup(currentScope, name);
        entities = lookupNameInScope(targetScope, name);
        currentScope = currentScope->parent();
    }

    return entities;
}

//looks up a name in a scope, includes base classes if scope is a class scope
QList<CodeModel::Member *> Semantic::lookupNameInScope(CodeModel::Scope *scope, const NameAST* name)
{
    QList<CodeModel::Member *> entities;

    if(!scope || !name)
        return entities;

    QByteArray nameText = textOf(name->unqualifiedName()->name());
    //look up name in members of current scope
    const CodeModel::MemberCollection members = scope->members();
    if (members.contains(nameText))
        entities.append(members.value(nameText));

    // if not found, look up name in  base classes (if any)
    CodeModel::ClassScope *classScope = scope->toClassScope();
    if (entities.isEmpty() && classScope) {
        const TypeCollection baseClasses = classScope->baseClasses();
        TypeCollection::ConstIterator it = baseClasses.constBegin();
        while (it != baseClasses.constEnd()) {
            CodeModel::Scope *baseClass = it.value()->toClassType()->scope();
            if (scope != baseClass)
                entities += lookupNameInScope(baseClass, name);
            ++it;
        }

        if (entities.count() > 1)
            emit error("Error in Semantic::lookupNameInScope: name "
            + nameText + " is ambigous");
    }
    return entities;
}

/*
    Resolves the classOrNamespaceNameList part of a NameAST against a base scope.
*/
CodeModel::Scope *Semantic::scopeLookup(CodeModel::Scope *baseScope, const NameAST* name)
{
    CodeModel::Scope *currentScope = baseScope;
    const List<ClassOrNamespaceNameAST *> *scopeList = name->classOrNamespaceNameList();
    // if there is no scope list, then the scope we are looking for is baseScope
    if (!scopeList)
        return baseScope;

    // Check if the global ("::") scope has been specified.
    if(name->isGlobal()) {
        while (currentScope->parent())
            currentScope = currentScope->parent();
    }

    while(currentScope != 0) {
        int nestingCounter = 0;
        CodeModel::Scope *nestedScope = currentScope;
        while (nestingCounter < scopeList->count()) {
            const QByteArray nameText = textOf((*scopeList)[nestingCounter]->name());
            nestedScope = nestedScope->scopes().value(nameText);
            if (!nestedScope)
                break;
            ++nestingCounter;
        }
        if(nestedScope) // found target scope?
            return nestedScope;

        currentScope = currentScope->parent(); //look in parent scope
    }

    return 0;
}

TypeMember *Semantic::typeLookup(CodeModel::Scope *baseScope, const NameAST* name)
{
    QList<CodeModel::Member *> memberList = nameLookup(baseScope, name);

    foreach(Member *member, memberList) {
        if(TypeMember *typeMember = member->toTypeMember())
            return typeMember;
    }
    return 0;
}

FunctionMember *Semantic::functionLookup(CodeModel::Scope *baseScope,
                                          const DeclaratorAST *functionDeclarator)
{

    QList<CodeModel::Member*> candidateList =
                nameLookup(baseScope, functionDeclarator->declaratorId());
    return selectFunction(candidateList, functionDeclarator);
}

/*
    This is a simplified function lookup routine, for matching member function
    definitions with member function declarations. It does not implement
    the general C++ function overload resolution rules.
*/
FunctionMember *Semantic::selectFunction(QList<CodeModel::Member*> candidatateList, const DeclaratorAST *functionDeclarator)
{
    // get arguments for funciton we are looking for
    FunctionMember testFunction;
    parseFunctionArguments(functionDeclarator, &testFunction);
    const ArgumentCollection testArgumentCollection = testFunction.arguments();

    //test againts functions in overload list.
    foreach(Member* member, candidatateList) {
        FunctionMember *function = member->toFunctionMember();
        if (!function)
            continue;
        const ArgumentCollection argumentCollection = function->arguments();

        //test argument types and number of arguments
        ArgumentCollection::ConstIterator arg1 = argumentCollection.constBegin();
        ArgumentCollection::ConstIterator arg2 = testArgumentCollection.constBegin();
        bool match = true;
        while(arg1 != argumentCollection.constEnd() && arg2 != testArgumentCollection.constEnd()) {
            if( arg1.value()->type()->name() != arg2.value()->type()->name() ) {
                match = false;
                break;
            }
            ++arg1;
            ++arg2;
        }
        if(match)
            return function;
    }
    return 0;
}

QByteArray Semantic::typeOfDeclaration(TypeSpecifierAST *typeSpec, DeclaratorAST *declarator)
{
    if (!typeSpec)
        return QByteArray();

    QByteArray text;

    if (typeSpec->cvQualify()) {
        List<AST*> cv = *typeSpec->cvQualify()->children();
        foreach (AST *current, cv) {
            text += " " + textOf(current);
        }
        text += " ";
    }


    text += textOf(typeSpec);

    if (typeSpec->cv2Qualify()) {
        List<AST*> cv = *typeSpec->cv2Qualify()->children();
        foreach (AST *current, cv) {
            text += textOf(current) + " ";
        }
    }

    if (declarator && declarator->ptrOpList()) {
        List<AST*> ptrOpList = *declarator->ptrOpList();
        foreach (AST *current, ptrOpList) {
            text += " " + textOf(current);
        }
        text += " ";
    }

    return text.trimmed().simplified();
}



QList<QByteArray> Semantic::scopeOfName(NameAST *id, const QList<QByteArray>& startScope)
{
    QList<QByteArray> scope = startScope;
    if (id && id->classOrNamespaceNameList()){
        if (id->isGlobal())
            scope.clear();

        List<ClassOrNamespaceNameAST*> l = *id->classOrNamespaceNameList();
        foreach (ClassOrNamespaceNameAST *current, l) {
            if (current->name())
               scope << textOf(current->name());
        }
    }

    return scope;
}

QList<QByteArray> Semantic::scopeOfDeclarator(DeclaratorAST *d, const QList<QByteArray>& startScope)
{
    if(!d)
        return QList<QByteArray>();
    return scopeOfName(d->declaratorId(), startScope);
}

QByteArray Semantic::typeSpecToString(TypeSpecifierAST* typeSpec)
{
    if (!typeSpec)
        return QByteArray();

    QByteArray tp;
    if (typeSpec->cvQualify()) {
        tp += "const ";
    }

    tp += (QString::fromLatin1(textOf(typeSpec)).replace(QRegExp(QLatin1String(" :: ")), QString::fromUtf8("::"))).toLatin1();
    return tp;
}

QByteArray Semantic::declaratorToString(DeclaratorAST* declarator, const QByteArray& scope, bool skipPtrOp)
{
    if (!declarator)
        return QByteArray();

    QByteArray text;

    if (!skipPtrOp && declarator->ptrOpList()){
        List<AST*> ptrOpList = *declarator->ptrOpList();
        foreach (AST *current, ptrOpList) {
            text += textOf(current);
        }
        text += QByteArray(" ");
    }

    text += scope;

    if (declarator->subDeclarator())
        text += QByteArray("(") + declaratorToString(declarator->subDeclarator()) + QByteArray(")");

    if (declarator->declaratorId())
        text += textOf(declarator->declaratorId());

    if (declarator->arrayDimensionList()) {
        List<AST*> arrays = *declarator->arrayDimensionList();
        foreach (AST *current, arrays) {
            current=current;    //silence unused symbol warning
            text += QByteArray("[]");
        }
    }

    if (declarator->parameterDeclarationClause()){
        text += QByteArray("(");

        ParameterDeclarationListAST* l = declarator->parameterDeclarationClause()->parameterDeclarationList();
        if (l != 0){
            List<ParameterDeclarationAST*> params = *l->parameterList();
            foreach (ParameterDeclarationAST *current, params) {
                QByteArray type = typeSpecToString(current->typeSpec());
                text += type;
                if (!type.isEmpty())
                    text += QByteArray(" ");
                text += declaratorToString(current->declarator());

                // ### FIXME if (it.current())
                    text += QByteArray(", ");
            }
        }

        text += QByteArray(")");

        if (declarator->constant() != 0)
            text += QByteArray(" const");
    }

    return QString::fromLatin1(text).replace(QRegExp(QLatin1String(" :: ")), QLatin1String("::")).simplified().toLatin1();
}

QByteArray Semantic::textOf(const AST *node) const
{
    if (!node)
        return QByteArray();
    QByteArray text;
    for (int i = node->startToken(); i < node->endToken(); ++i) {
        if (!m_tokenStream->isHidden(i)) {
            if (i != node->startToken())
                text += QByteArray(" ");
            text += m_tokenStream->tokenText(i);
        }
    }
    return text;
}

void Semantic::createNameUse(Member *member, NameAST *name)
{
    if (!name)
       return;

    AST *unqualifedName = name->unqualifiedName()->name();

    if(!unqualifedName || !member)
        return;

    CodeModel::NameUse *nameUse = CodeModel::Create<CodeModel::NameUse>(m_storage);
    nameUse->setParent(currentScope.top());
    nameUse->setNameToken(tokenRefFromAST(unqualifedName));
    nameUse->setName(textOf(unqualifedName));
    nameUse->setDeclaration(member);

    currentScope.top()->addNameUse(nameUse);
    addNameUse(unqualifedName, nameUse);
}

void Semantic::addNameUse(AST *node, NameUse *nameUse)
{
    const int tokenIndex = node->startToken();
    m_nameUses.insert(tokenIndex, nameUse);
}

/*
    Searches a AST node and all its children for a nameUse. The name use is
    found by looking up each node's tokens in the m_nameUses map. A depth-first
    search is used.
*/
NameUse *Semantic::findNameUse(AST *node)
{
    if(!node)
        return 0;

    List<AST*> *children = node->children();
    if(children) {
        NameUse *nameUse = 0;
        foreach(AST* child , *children) {
            nameUse = findNameUse(child);
            if(nameUse)
                break;
        }
        if (nameUse)
            return nameUse;
    }

    for (int t = node->startToken(); t < node->endToken(); ++t) {
 //       cout << t <<" |" <<m_tokenStream->tokenText(t).constData() << "|" << endl;
        if (m_nameUses.contains(t))
            return m_nameUses.value(t);
    }
    return 0;
}

/*
    Gets a TokenRef from an AST node.
    Assumes that the node only covers one token, which means that
    node->statToken() == node->endToken(). If this is not the case
    then the TokenRef will reference the token at startToken.
*/
TokenEngine::TokenRef Semantic::tokenRefFromAST(AST *node)
{
    const int startTokenIndex = node->startToken();
    const TokenEngine::TokenContainer tokenContainer = m_tokenStream->tokenContainer(startTokenIndex);
    const int containerIndex = m_tokenStream->containerIndex(startTokenIndex);
    return TokenEngine::TokenRef(tokenContainer, containerIndex);
}

QT_END_NAMESPACE
