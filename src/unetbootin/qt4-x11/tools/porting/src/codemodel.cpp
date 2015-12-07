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

#include "codemodel.h"

#include <QList>
#include <QByteArray>
#include <QtDebug>

QT_BEGIN_NAMESPACE

namespace CodeModel {

BuiltinType BuiltinType::Bool("bool", 0 );
BuiltinType BuiltinType::Void("void", 0 );
BuiltinType BuiltinType::Char("char", 0 );
BuiltinType BuiltinType::Short("short", 0 );
BuiltinType BuiltinType::Int("int", 0 );
BuiltinType BuiltinType::Long("long", 0 );
BuiltinType BuiltinType::Double("double", 0 );
BuiltinType BuiltinType::Float("float", 0 );
BuiltinType BuiltinType::Unsigned("unsigned", 0 );
BuiltinType BuiltinType::Signed("signed", 0 );

void Scope::addScope(Scope *scope)
{
    scope->setParent(this);
    m_scopes.add(scope);
}

void Scope::addType(Type *type)
{
    if (ClassType *klass = type->toClassType())
        klass->setParent(this);
    m_types.add(type);
}

void Scope::addMember(Member *member)
{
    member->setParent(this);
    m_members.add(member);
}

void Scope::addNameUse(NameUse *nameUse)
{
    nameUse->setParent(this);
    m_nameUses.add(nameUse);
}

} //namepsace CodeModel

QT_END_NAMESPACE
