/****************************************************************************
**
** Copyright (C) 2004-2008 Trolltech ASA. All rights reserved.
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

#include "codemodelwalker.h"

QT_BEGIN_NAMESPACE
using namespace CodeModel;

void CodeModelWalker::parseScope(CodeModel::Scope *scope)
{
    if(!scope)
        return;

    if(scope->toClassScope())
        parseClassScope(scope->toClassScope());
    if(scope->toNamespaceScope())
        parseNamespaceScope(scope->toNamespaceScope());
    if(scope->toBlockScope())
        parseBlockScope(scope->toBlockScope());


    {
        MemberCollection collection = scope->members();
        MemberCollection::ConstIterator it = collection.constBegin();
        while(it != collection.constEnd())
            parseMember(*it++);
    }
    {
        ScopeCollection collection = scope->scopes();
        ScopeCollection::ConstIterator it = collection.constBegin();
        while(it != collection.constEnd())
            parseScope(*it++);
    }
    {
        NameUseCollection collection = scope->nameUses();
        NameUseCollection::ConstIterator it = collection.constBegin();
        while(it != collection.constEnd())
            parseNameUse(*it++);
    }
}

void CodeModelWalker::parseType(CodeModel::Type *type)
{
    if(!type)
        return;
    if (type->toEnumType())
        parseEnumType(type->toEnumType());
    else if (type->toClassType())
        parseClassType(type->toClassType());
    else if (type->toBuiltinType())
        parseBuiltinType(type->toBuiltinType());
    else if (type->toPointerType())
        parsePointerType(type->toPointerType());
    else if (type->toReferenceType())
        parseReferenceType(type->toReferenceType());
    else if (type->toGenericType())
        parseGenericType(type->toGenericType());
    else if (type->toAliasType())
        parseAliasType(type->toAliasType());
    else if (type->toUnknownType())
        parseUnknownType(type->toUnknownType());
}

void CodeModelWalker::parseMember(CodeModel::Member *member)
{
    if(!member)
        return;

    if (member->toFunctionMember())
        parseFunctionMember(member->toFunctionMember());
    else if (member->toVariableMember())
        parseVariableMember(member->toVariableMember());
    else if (member->toUsingDeclarationMember())
        parseUsingDeclarationMember(member->toUsingDeclarationMember());
    else if (member->toTypeMember())
        parseTypeMember(member->toTypeMember());
}

void CodeModelWalker::parseFunctionMember(CodeModel::FunctionMember *member)
{
    if(!member)
        return;
    if(member->functionBodyScope())
        parseScope(member->functionBodyScope());
}

QT_END_NAMESPACE
