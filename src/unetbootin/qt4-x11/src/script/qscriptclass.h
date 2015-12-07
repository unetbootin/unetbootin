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

#ifndef QSCRIPTCLASS_H
#define QSCRIPTCLASS_H

#include <QtCore/qstring.h>

#ifndef QT_NO_SCRIPT

#include <QtCore/qvariant.h>
#include <QtScript/qscriptvalue.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Script)

class QScriptString;
class QScriptClassPropertyIterator;

class QScriptClassPrivate;
class Q_SCRIPT_EXPORT QScriptClass
{
public:
    enum QueryFlag {
        HandlesReadAccess = 0x01,
        HandlesWriteAccess = 0x02
    };
    Q_DECLARE_FLAGS(QueryFlags, QueryFlag)

    enum Extension {
        Callable,
        HasInstance
    };

    QScriptClass(QScriptEngine *engine);
    virtual ~QScriptClass();

    QScriptEngine *engine() const;

    virtual QueryFlags queryProperty(const QScriptValue &object,
                                     const QScriptString &name,
                                     QueryFlags flags, uint *id);

    virtual QScriptValue property(const QScriptValue &object,
                                  const QScriptString &name, uint id);

    virtual void setProperty(QScriptValue &object, const QScriptString &name,
                             uint id, const QScriptValue &value);

    virtual QScriptValue::PropertyFlags propertyFlags(
        const QScriptValue &object, const QScriptString &name, uint id);

    virtual QScriptClassPropertyIterator *newIterator(const QScriptValue &object);

    virtual QScriptValue prototype() const;

    virtual QString name() const;

    virtual bool supportsExtension(Extension extension) const;
    virtual QVariant extension(Extension extension,
                               const QVariant &argument = QVariant());

protected:
    QScriptClass(QScriptEngine *engine, QScriptClassPrivate &dd);
    QScriptClassPrivate *d_ptr;

private:
    Q_DECLARE_PRIVATE(QScriptClass)
    Q_DISABLE_COPY(QScriptClass)
};

Q_DECLARE_OPERATORS_FOR_FLAGS(QScriptClass::QueryFlags)

QT_END_NAMESPACE

QT_END_HEADER

#endif // QT_NO_SCRIPT

#endif
