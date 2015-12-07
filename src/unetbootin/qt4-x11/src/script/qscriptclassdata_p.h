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

#ifndef QSCRIPTCLASSDATA_P_H
#define QSCRIPTCLASSDATA_P_H

#include "qscriptglobals_p.h"

#ifndef QT_NO_SCRIPT

QT_BEGIN_NAMESPACE

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

class QScriptValueImpl;
class QScriptNameIdImpl;
class QScriptClassDataIterator;

namespace QScript {
    class Member;
}

class QScriptClassData
{
protected:
    QScriptClassData();

public:
    virtual ~QScriptClassData();

    virtual void mark(const QScriptValueImpl &object, int generation);
    virtual bool resolve(const QScriptValueImpl &object, QScriptNameIdImpl *nameId,
                         QScript::Member *member, QScriptValueImpl *base,
                         QScript::AccessMode access);
    virtual bool get(const QScriptValueImpl &obj, const QScript::Member &m,
                     QScriptValueImpl *result);
    virtual bool put(QScriptValueImpl *object, const QScript::Member &member,
                     const QScriptValueImpl &value);
    virtual bool removeMember(const QScriptValueImpl &object,
                              const QScript::Member &member);
    virtual bool implementsHasInstance(const QScriptValueImpl &object);
    virtual bool hasInstance(const QScriptValueImpl &object,
                             const QScriptValueImpl &value);
    virtual QScriptClassDataIterator *newIterator(const QScriptValueImpl &object);

private:
    Q_DISABLE_COPY(QScriptClassData)
};

class QScriptClassDataIterator
{
protected:
    QScriptClassDataIterator();

public:
    virtual ~QScriptClassDataIterator();

    virtual bool hasNext() const = 0;
    virtual void next(QScript::Member *member) = 0;

    virtual bool hasPrevious() const = 0;
    virtual void previous(QScript::Member *member) = 0;

    virtual void toFront() = 0;
    virtual void toBack() = 0;
};

QT_END_NAMESPACE

#endif // QT_NO_SCRIPT

#endif // QSCRIPTCLASSDATA_P_H
