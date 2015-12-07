/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the QtScript module of the Qt Toolkit.
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

#include "qscriptclassdata_p.h"

#ifndef QT_NO_SCRIPT

QT_BEGIN_NAMESPACE

QScriptClassData::QScriptClassData()
{
}

QScriptClassData::~QScriptClassData()
{
}

void QScriptClassData::mark(const QScriptValueImpl &, int)
{
}

bool QScriptClassData:: resolve(const QScriptValueImpl &, QScriptNameIdImpl *,
                                QScript::Member *, QScriptValueImpl *,
                                QScript::AccessMode)
{
    return false;
}

bool QScriptClassData::get(const QScriptValueImpl &, const QScript::Member &,
                           QScriptValueImpl *)
{
    Q_ASSERT_X(false, "QScriptClassData::get()",
               "implement if resolveMember is implemented");
    return false;
}

bool QScriptClassData::put(QScriptValueImpl *, const QScript::Member &,
                           const QScriptValueImpl &)
{
    Q_ASSERT_X(false, "QScriptClassData::put()",
               "implement if resolveMember is implemented");
    return false;
}

bool QScriptClassData::removeMember(const QScriptValueImpl &,
                                    const QScript::Member &)
{
    return true;
}

bool QScriptClassData::implementsHasInstance(const QScriptValueImpl &)
{
    return false;
}

bool QScriptClassData::hasInstance(const QScriptValueImpl &,
                                   const QScriptValueImpl &)
{
    Q_ASSERT_X(false, "QScriptClassData::hasInstance()",
               "implement if implementsHasInstance() returns true");
    return false;
}

QScriptClassDataIterator *QScriptClassData::newIterator(const QScriptValueImpl &)
{
    return 0;
}

QScriptClassDataIterator::QScriptClassDataIterator()
{
}

QScriptClassDataIterator::~QScriptClassDataIterator()
{
}

QT_END_NAMESPACE

#endif
