/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the QtScriptTools module of the Qt Toolkit.
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

#include "qscriptvalueproperty_p.h"

#include <QtCore/qatomic.h>
#include <QtCore/qstring.h>

QT_BEGIN_NAMESPACE

class QScriptValuePropertyPrivate
{
public:
    QScriptValuePropertyPrivate();
    ~QScriptValuePropertyPrivate();

    QString name;
    QScriptValue value;
    QScriptValue::PropertyFlags flags;

    QBasicAtomicInt ref;
};

QScriptValuePropertyPrivate::QScriptValuePropertyPrivate()
{
    ref = 0;
}

QScriptValuePropertyPrivate::~QScriptValuePropertyPrivate()
{
}

/*!
  Constructs an invalid QScriptValueProperty.
*/
QScriptValueProperty::QScriptValueProperty()
    : d_ptr(0)
{
}

/*!
  Constructs a QScriptValueProperty with the given \a name,
  \a value and \a flags.
*/
QScriptValueProperty::QScriptValueProperty(const QString &name,
                                           const QScriptValue &value,
                                           QScriptValue::PropertyFlags flags)
    : d_ptr(new QScriptValuePropertyPrivate)
{
    d_ptr->name = name;
    d_ptr->value = value;
    d_ptr->flags = flags;
    d_ptr->ref.ref();
}

/*!
  Constructs a QScriptValueProperty that is a copy of the \a other property.
*/
QScriptValueProperty::QScriptValueProperty(const QScriptValueProperty &other)
    : d_ptr(other.d_ptr)
{
    if (d_ptr)
        d_ptr->ref.ref();
}

/*!
  Destroys this QScriptValueProperty.
*/
QScriptValueProperty::~QScriptValueProperty()
{
    if (d_ptr && !d_ptr->ref.deref()) {
        delete d_ptr;
        d_ptr = 0;
    }
}

/*!
  Assigns the \a other property to this QScriptValueProperty.
*/
QScriptValueProperty &QScriptValueProperty::operator=(const QScriptValueProperty &other)
{
    if (d_ptr == other.d_ptr)
        return *this;
    if (d_ptr && !d_ptr->ref.deref())
        delete d_ptr;
    d_ptr = other.d_ptr;
    if (d_ptr)
        d_ptr->ref.ref();
    return *this;
}

/*!
  Returns the name of this QScriptValueProperty.
*/
QString QScriptValueProperty::name() const
{
    Q_D(const QScriptValueProperty);
    if (!d)
        return QString();
    return d->name;
}

/*!
  Returns the value of this QScriptValueProperty.
*/
QScriptValue QScriptValueProperty::value() const
{
    Q_D(const QScriptValueProperty);
    if (!d)
        return QScriptValue();
    return d->value;
}

/*!
  Returns the flags of this QScriptValueProperty.
*/
QScriptValue::PropertyFlags QScriptValueProperty::flags() const
{
    Q_D(const QScriptValueProperty);
    if (!d)
        return 0;
    return d->flags;
}

/*!
  Returns true if this QScriptValueProperty is valid, otherwise
  returns false.
*/
bool QScriptValueProperty::isValid() const
{
    Q_D(const QScriptValueProperty);
    return (d != 0);
}

QT_END_NAMESPACE
