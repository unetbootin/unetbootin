/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the QtScript module of the Qt Toolkit.
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

#ifndef QSCRIPTCLASSDATA_P_H
#define QSCRIPTCLASSDATA_P_H

#include <QtCore/qshareddata.h>

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

class QScriptClassData: public QSharedData
{
protected:
    QScriptClassData();

public:
    virtual ~QScriptClassData();

    virtual void mark(const QScriptValueImpl &object, int generation);
    virtual bool resolve(const QScriptValueImpl &object, QScriptNameIdImpl *nameId,
                         QScript::Member *member, QScriptValueImpl *base);
    virtual bool get(const QScriptValueImpl &obj, const QScript::Member &m,
                     QScriptValueImpl *result);
    virtual bool put(QScriptValueImpl *object, const QScript::Member &member,
                     const QScriptValueImpl &value);
    virtual bool removeMember(const QScriptValueImpl &object,
                              const QScript::Member &member);
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
