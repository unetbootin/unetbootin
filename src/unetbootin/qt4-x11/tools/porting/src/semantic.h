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

#ifndef SEMANTIC_H
#define SEMANTIC_H

#include "treewalker.h"
#include "codemodel.h"
#include "tokenstreamadapter.h"

#include <QObject>
#include <QStack>
#include <QList>
#include <QByteArray>

QT_BEGIN_NAMESPACE

class Semantic: public QObject, public TreeWalker
{
Q_OBJECT
public:
    Semantic(CodeModel::NamespaceScope *globalScope,
             TokenStreamAdapter::TokenStream *tokenStream,
             TypedPool<CodeModel::Item> *storage);

    void parseAST(TranslationUnitAST *node);
signals:
    void error(const QByteArray &message);
    void warning(const QByteArray &message);
protected:
    virtual void parseNamespace(NamespaceAST *);
    virtual void parseClassSpecifier(ClassSpecifierAST *);
    virtual void parseElaboratedTypeSpecifier(ElaboratedTypeSpecifierAST *node);
    virtual void parseSimpleDeclaration(SimpleDeclarationAST *);
    virtual void parseDeclaration(AST *funSpec, AST *storageSpec, TypeSpecifierAST *typeSpec, InitDeclaratorAST *decl);
    virtual void parseFunctionDeclaration(AST *funSpec, AST *storageSpec, TypeSpecifierAST *typeSpec, InitDeclaratorAST *decl);
    virtual void parseFunctionArguments(const DeclaratorAST *declarator, CodeModel::FunctionMember *method);
    virtual void parseFunctionDefinition(FunctionDefinitionAST *);
    virtual void parseStatementList(StatementListAST *);
    virtual void parseBaseClause(BaseClauseAST *baseClause, CodeModel::ClassScope * klass);
    virtual void parseLinkageSpecification(LinkageSpecificationAST *);
    virtual void parseUsing(UsingAST *);
    virtual void parseUsingDirective(UsingDirectiveAST *);
    virtual void parseExpression(AbstractExpressionAST*);
    virtual void parseExpressionStatement(ExpressionStatementAST *node);
    virtual void parseClassMemberAccess(ClassMemberAccessAST *node);
    virtual void parseNameUse(NameAST*);
    virtual void parseEnumSpecifier(EnumSpecifierAST *);
    virtual void parseTypedef(TypedefAST *);
    virtual void parseTypeSpecifier(TypeSpecifierAST *);

    QList<CodeModel::Member *> nameLookup(CodeModel::Scope *baseScope, const NameAST* name);
    QList<CodeModel::Member *> unqualifiedNameLookup(CodeModel::Scope *baseScope, const NameAST* name);
    QList<CodeModel::Member *> qualifiedNameLookup(CodeModel::Scope *baseScope, const NameAST* name);
    QList<CodeModel::Member *> lookupNameInScope(CodeModel::Scope *scope, const NameAST* name);

    CodeModel::TypeMember *typeLookup(CodeModel::Scope *baseScope, const NameAST* name);
    CodeModel::FunctionMember *functionLookup(CodeModel::Scope *baseScope,  const DeclaratorAST *functionDeclarator);
    CodeModel::Scope *scopeLookup(CodeModel::Scope *baseScope, const NameAST* name);
    CodeModel::FunctionMember *selectFunction(QList<CodeModel::Member*> candidatateList, const DeclaratorAST *functionDeclarator);

    QByteArray typeOfDeclaration(TypeSpecifierAST *typeSpec, DeclaratorAST *declarator);
    QList<QByteArray> scopeOfName(NameAST *id, const QList<QByteArray> &scope);
    QList<QByteArray> scopeOfDeclarator(DeclaratorAST *d, const QList<QByteArray> &scope);
    QByteArray declaratorToString(DeclaratorAST* declarator, const QByteArray& scope = QByteArray(), bool skipPtrOp = false);
    QByteArray typeSpecToString(TypeSpecifierAST* typeSpec);

    QByteArray textOf(const AST *node) const;
    void createNameUse(CodeModel::Member *member, NameAST *name);
    void addNameUse(AST *node, CodeModel::NameUse *nameUse);
    CodeModel::NameUse *findNameUse(AST *node);
    TokenEngine::TokenRef tokenRefFromAST(AST *node);
private:
    TokenStreamAdapter::TokenStream *m_tokenStream;
    TypedPool<CodeModel::Item> *m_storage;
    CodeModel::Member::Access m_currentAccess;
    bool m_inSlots;
    bool m_inSignals;
    bool m_inStorageSpec;
    bool m_inTypedef;

    QMap<int, CodeModel::NameUse *> m_nameUses;
    QStack<CodeModel::Scope *> currentScope;
    CodeModel::TypeMember *m_sharedUnknownMember;
private:
    Semantic(const Semantic &source);
    void operator = (const Semantic &source);
};

QT_END_NAMESPACE

#endif // SEMANTIC_H
