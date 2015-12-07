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

#ifndef CODEMODEL_H
#define CODEMODEL_H

#include "smallobject.h"
#include "tokenengine.h"

#include <QByteArray>
#include <QList>
#include <QMap>
#include <QHash>

QT_BEGIN_NAMESPACE

namespace CodeModel
{

// types
struct Type;
struct EnumType;
struct EnumeratorType;
struct ClassType;
struct BuiltinType;
struct PointerType;
struct ReferenceType;
struct GenericType;
struct AliasType;
struct FunctionType;
struct UnknownType;

// Scopes contain child scopes, members and types.
struct Scope;
struct ClassScope;
struct NamespaceScope;
struct BlockScope;

// Members introduces names into scopes, and are also linked to a specific
// token in a source file.
struct Member;
struct FunctionMember;
struct VariableMember;
struct UsingDeclarationMember;
struct NamespaceMember;
struct TypeMember;

// Name uses links uses of a name to its declaration (a Member), and also to a
// token in a source file.
struct NameUse;

struct Argument;
struct UsingDirectiveLink;

template <typename CollectedType>
class Collection: public QMultiHash<QByteArray, CollectedType *>
{
public:
     void add(CollectedType *collectedItem)
    { insert(collectedItem->name(), collectedItem); }
};

typedef Collection<Scope> ScopeCollection;
typedef Collection<Member> MemberCollection;
typedef Collection<Type> TypeCollection;
typedef Collection<NameUse> NameUseCollection;
typedef Collection<Argument> ArgumentCollection;

struct SemanticInfo
{
    CodeModel::NamespaceScope *codeModel;

    // tokenindex -> NameUse* map. Use map here bacause we expect name uses to
    // be sparesly distributed among the tokens.
    QMap<int, NameUse*> nameUses;
};


struct Item
{
    Item()  {}
    virtual ~Item() {}
    virtual QByteArray name() const = 0;
};

struct Type: public Item
{
    virtual QByteArray name() const =0;

    virtual EnumType *toEnumType() const
    { return 0; }

    virtual ClassType *toClassType() const
    { return 0; }

    virtual UnknownType *toUnknownType() const
    { return 0; }

    virtual BuiltinType *toBuiltinType() const
    { return 0; }

    virtual PointerType *toPointerType() const
    { return 0; }

    virtual ReferenceType *toReferenceType() const
    { return 0; }

    virtual GenericType *toGenericType() const
    { return 0; }

    virtual AliasType *toAliasType() const
    { return 0; }
};

struct Scope: public Item
{
    Scope()
     : m_parent(0) {}

    void setParent(Scope *parent)
    { m_parent = parent; }

    Scope *parent() const
    { return m_parent; }

    QByteArray name() const
    { return m_name; }

    void setName(const QByteArray &name)
    { m_name=name; }

    virtual NamespaceScope *toNamespaceScope() const
    { return 0; }

    virtual ClassScope *toClassScope() const
    { return 0; }

    virtual BlockScope *toBlockScope() const
    { return 0; }

    const Collection<Scope> scopes() const
    { return m_scopes; }
    const Collection<Type> types() const
    { return m_types; }
    const Collection<Member> members() const
    { return m_members; }
    const Collection<NameUse> nameUses() const
    { return m_nameUses; }

    void addScope(Scope *scope);
    void addType(Type *type);
    void addMember(Member *member);
    void addNameUse(NameUse *nameUse);
private:
    Scope *m_parent;
    QByteArray m_name;
    Collection<Scope> m_scopes;
    Collection<Type> m_types;
    Collection<Member> m_members;
    Collection<NameUse> m_nameUses;
};

struct Member: public Item
{
    enum Binding // ### not used yet
    {
        Static,
        Instance
    };

    enum Access // ### not used yet
    {
        Public,
        Protected,
        Private
    };

    Member()
        : m_binding(Static), m_access(Public),
        m_parent(0), m_constant(0), m_static(0)   {}

    QByteArray name() const
    { return m_name; }

    void setName(const QByteArray &name)
    { m_name = name; }

    TokenEngine::TokenRef nameToken() const
    { return m_nameToken; }

    void setNameToken(TokenEngine::TokenRef nameToken)
    { m_nameToken = nameToken; }

    Binding binding() const
    { return m_binding; }

    void setBinding(Binding binding)
    { m_binding = binding; }

    Access access() const
    { return m_access; }

    void setAccess(Access access)
    { m_access = access; }

    bool isConstant() const
    { return m_constant; }

    void setConstant(bool b)
    { m_constant = b; }

    bool isStatic() const
    { return m_static; }

    void setStatic(bool b)
    { m_static = b; }

    Scope *parent() const
    { return m_parent; }

    void setParent(Scope *parent)
    { m_parent = parent; }

    virtual FunctionMember *toFunctionMember() const
    { return 0; }

    virtual VariableMember *toVariableMember() const
    { return 0; }

    virtual UsingDeclarationMember *toUsingDeclarationMember() const
    { return 0; }

    virtual NamespaceMember *toNamespaceMember() const
    { return 0; }

    virtual TypeMember *toTypeMember() const
    { return 0; }

 private:
     Binding m_binding;
     Access m_access;
     Scope *m_parent;
     QByteArray m_name;
     TokenEngine::TokenRef m_nameToken;
     uint m_constant : 1;
     uint m_static : 1;
};

struct ClassScope: public Scope
{
    const Collection<Type> baseClasses() const
    { return m_baseClasses; }

    void addBaseClass(Type *baseClass)
    {
        Q_ASSERT(baseClass->toClassType());
        m_baseClasses.add(baseClass);
    }

    virtual ClassScope *toClassScope() const
    { return const_cast<ClassScope*>(this); }

private:
    Collection<Type> m_baseClasses;
};

struct UsingDirectiveLinkable : public Scope
{
    const QList<UsingDirectiveLink *> usingDirectiveLinks() const
    { return m_usingDirectiveLinks; }

    void addUsingDirectiveLink(UsingDirectiveLink *usingDirectiveLink)
    { m_usingDirectiveLinks.append(usingDirectiveLink); }
private:
    QList<UsingDirectiveLink *> m_usingDirectiveLinks;
};

struct NamespaceScope: public UsingDirectiveLinkable
{
    NamespaceScope() {}

    virtual NamespaceScope *toNamespaceScope() const
    { return const_cast<NamespaceScope*>(this); }
};

struct BlockScope: public UsingDirectiveLinkable
{
    BlockScope() {}

    virtual BlockScope *toBlockScope() const
    { return const_cast<BlockScope*>(this); }
};

struct EnumType: public Type
{
    EnumType()
        : m_parent(0) {}

    QByteArray name() const
    { return m_name; }

    void setName(const QByteArray &name)
    { m_name = name; }

    Scope *parent() const
    { return m_parent; }

    void setParent(Scope *parent)
    { m_parent = parent; }

    virtual EnumType *toEnumType() const
    { return const_cast<EnumType*>(this); }

private:
    Scope *m_parent;
    QByteArray m_name;
};

struct UnknownType: public Type
{
    UnknownType()
        : m_parent(0) {}

    QByteArray name() const
    { return m_name; }

    void setName(const QByteArray &name)
    { m_name = name; }

    Scope *parent() const
    { return m_parent; }

    void setParent(Scope *parent)
    { m_parent = parent; }

    virtual UnknownType *toUnknownType() const
    { return const_cast<UnknownType*>(this); }

private:
    Scope *m_parent;
    QByteArray m_name;
};

struct ClassType: public Type
{
    ClassType()
        : m_scope(0) {}

    ClassScope *scope() const
    { return m_scope; }

    void setScope(ClassScope *scope)
    { m_scope = scope; }

    QByteArray name() const
    { return m_scope ? m_scope->name() : /*anonymous*/ QByteArray(); }

    Scope *parent() const
    { return m_parent; }

    void setParent(Scope *parent)
    { m_parent = parent; }

    virtual ClassType *toClassType() const
    { return const_cast<ClassType*>(this); }

private:
    Scope *m_parent;
    ClassScope *m_scope;

};

struct BuiltinType: public Type
{
protected:
    BuiltinType(const QByteArray &name, Scope *parent)
        : m_name(name), m_parent(parent) {}

public:
    QByteArray name() const
    { return m_name; }

    Scope *parent() const
    { return m_parent; }

    virtual BuiltinType *toBuiltinType() const
    { return const_cast<BuiltinType*>(this); }

    static BuiltinType Bool;
    static BuiltinType Void;
    static BuiltinType Char;
    static BuiltinType Short;
    static BuiltinType Int;
    static BuiltinType Long;
    static BuiltinType Double;
    static BuiltinType Float;
    static BuiltinType Unsigned;
    static BuiltinType Signed;
    // ### more

private:
    QByteArray m_name;
    Scope *m_parent;
};

struct PointerType: public Type
{
    PointerType()
        : m_parent(0), m_baseType(0) {}

    Type *baseType() const
    { return m_baseType; }

    void setBaseType(Type *baseType)
    { m_baseType = baseType; }

    QByteArray name() const
    { return m_baseType->name(); }

    Scope *parent() const
    { return m_parent; }

    void setParent(Scope *parent)
    { m_parent = parent; }

    virtual PointerType *toPointerType() const
    { return const_cast<PointerType*>(this); }

private:
    Scope *m_parent;
    Type *m_baseType;
};

struct ReferenceType: public Type
{
    ReferenceType()
        : m_parent(0), m_baseType(0) {}

    Type *baseType() const
    { return m_baseType; }

    void setBaseType(Type *baseType)
    { m_baseType = baseType; }

    QByteArray name() const
    { return m_baseType->name(); }

    Scope *parent() const
    { return m_parent; }

    void setParent(Scope *parent)
    { m_parent = parent; }

    virtual ReferenceType *toReferenceType() const
    { return const_cast<ReferenceType*>(this); }

private:
    Scope *m_parent;
    Type *m_baseType;
};

struct GenericType: public Type // ### implement me
{
    virtual GenericType *toGenericType() const
    { return const_cast<GenericType*>(this); }
};

struct AliasType: public Type // ### implement me
{
    AliasType ()
        : m_parent(0) {}

    QByteArray name() const
    {  return m_name;  }

    Scope *parent() const
    {  return m_parent;  }

    virtual AliasType *toAliasType() const
    { return const_cast<AliasType*>(this); }
private:
    QByteArray m_name;
    Scope *m_parent;
};

struct Argument: public Item
{
    Argument()
        : m_parent(0), m_type(0) {}

    Type *type() const
    { return m_type; }

    void setType(Type *type)
    { m_type = type; }

    QByteArray name() const
    { return m_name; }

    void setName(const QByteArray &name)
    { m_name = name; }

    TokenEngine::TokenRef nameToken() const
    { return m_nameToken; }

    void setNameToken(TokenEngine::TokenRef nameToken)
    { m_nameToken = nameToken; }

    virtual FunctionMember *parent() const
    { return m_parent; }

    void setParent(FunctionMember *parent)
    { m_parent = parent; }

private:
    FunctionMember *m_parent;
    Type *m_type;
    QByteArray m_name;
    TokenEngine::TokenRef m_nameToken;
};

struct FunctionMember: public Member
{
    inline FunctionMember()
        : m_returnType(0),
          m_functionBodyScope(0),
          m_signal(0),
          m_virtual(0), m_abstract(0) { m_slot = 0; }

    virtual FunctionMember *toFunctionMember() const
    { return const_cast<FunctionMember*>(this); }

    Type *returnType() const
    { return m_returnType; }

    void setReturnType(Type *type)
    { m_returnType = type; }

    const Collection<Argument> arguments() const
    { return m_arguments; }

    void addArgument(Argument *argument)
    { m_arguments.insert(argument->name(), argument); }

    void setFunctionBodyScope(BlockScope *functionBodyScope)
    { m_functionBodyScope = functionBodyScope; }

    BlockScope *functionBodyScope() const
    {return m_functionBodyScope;}

    bool isSignal() const
    { return m_signal; }

    void setSignal(bool b)
    { m_signal = b; }

    bool isSlot() const
    { return m_slot; }

    void setSlot(bool b)
    { m_slot = b; }

    bool isVirtual() const
    { return m_virtual; }

    void setVirtual(bool b)
    { m_virtual = b; }

    bool isAbstract() const
    { return m_abstract; }

    void setAbstract(bool b)
    { m_abstract = b; }

private:
    Type *m_returnType;
    Collection<Argument> m_arguments;
    BlockScope *m_functionBodyScope;
    uint m_signal: 1;
    uint m_slot: 1;
    uint m_virtual: 1;
    uint m_abstract: 1;
};

struct VariableMember: public Member
{
    VariableMember()
        : m_type(0) {}

    Type *type() const
    { return m_type; }

    void setType(Type *type)
    { m_type = type; }

    virtual VariableMember *toVariableMember() const
    { return const_cast<VariableMember*>(this); }

private:
    Type *m_type;
};

struct UsingDeclarationMember: public Member
{
    UsingDeclarationMember()
    : m_member(0) {}

    virtual UsingDeclarationMember *toUsingDeclarationMember() const
    { return const_cast<UsingDeclarationMember*>(this); }

    Member *member() const
    { return m_member; }

    void setMember(Member *member)
    { m_member = member; }

private:
    Member *m_member;
};

struct NamespaceMember: public Member
{
    NamespaceMember()
        :m_namespaceScope(0) {}

    virtual NamespaceMember *toNamespaceMember() const
    { return const_cast<NamespaceMember*>(this); }

    NamespaceScope *namespaceScope() const
    { return m_namespaceScope; }

    void setNamespaceScope(NamespaceScope *namespaceScope)
    { m_namespaceScope = namespaceScope; }
private:
    NamespaceScope *m_namespaceScope;
};

struct TypeMember: public Member
{
    TypeMember()
        :m_type(0) {}

    virtual TypeMember *toTypeMember() const
    { return const_cast<TypeMember*>(this); }

    Type *type() const
    { return m_type; }

    void setType(Type *type)
    { m_type = type; }
private:
    Type *m_type;

};

struct NameUse: public Item
{
    NameUse()
    : m_declaration(0), m_parent(0) {}

    QByteArray name() const
    { return m_name; }

    void setName(const QByteArray &name)
    {  m_name = name; }

     TokenEngine::TokenRef nameToken() const
    { return m_nameToken; }

    void setNameToken(TokenEngine::TokenRef nameToken)
    { m_nameToken = nameToken; }

    Scope *parent() const
    { return m_parent; }

    void setParent(Scope *parent)
    { m_parent = parent; }

    Member *declaration() const
    { return m_declaration; }

    void setDeclaration(Member *parent)
    { m_declaration = parent; }

private:
    QByteArray m_name;
    TokenEngine::TokenRef m_nameToken;
    Member *m_declaration;
    Scope *m_parent;
};

struct UsingDirectiveLink: public Item
{
    UsingDirectiveLink()
        : m_parent(0) {}

    QByteArray name() const
    { return QByteArray(); }

    Scope *parent() const
    { return m_parent; }

    void setParent(Scope *parent)
    { m_parent = parent; }

    NamespaceScope *targetNamespace() const
    { return m_targetNamespace; }

    void setTargetNamespace(NamespaceScope *targetNamespace)
    { m_targetNamespace = targetNamespace; }

    NamespaceScope *insertionNamespace() const
    { return m_insertionNamespace; }

    void setInsertionNamespace(NamespaceScope *insertionNamespace)
    { m_insertionNamespace = insertionNamespace; }
private:
    Scope *m_parent;
    // targetNamespace is the namespace specified by the using directive.
    NamespaceScope *m_targetNamespace;
    // m_insertionNamespace is the namespace where the names from
    // targetNamespace will be inserted. The C++ standard (7.3.4.1)
    // defines this as the nearest namespace that includes both m_parent
    // and m_targetNamespace.
    NamespaceScope *m_insertionNamespace;
};

template <class T>
T *Create(TypedPool<CodeModel::Item> *p)
{
    return new (p->allocate(sizeof(T))) T();
}

} // namespace CodeModel

QT_END_NAMESPACE

#endif // CODEMODEL_H
