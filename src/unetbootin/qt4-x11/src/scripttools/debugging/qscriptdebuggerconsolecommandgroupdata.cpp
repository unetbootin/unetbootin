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

#include "qscriptdebuggerconsolecommandgroupdata_p.h"

#include <QtCore/qstring.h>

QT_BEGIN_NAMESPACE

/*!
  \since 4.5
  \class QScriptDebuggerConsoleCommandGroupData
  \internal

  \brief The QScriptDebuggerConsoleCommandGroupData class holds data associated with a console command group.
*/

class QScriptDebuggerConsoleCommandGroupDataPrivate
{
public:
    QScriptDebuggerConsoleCommandGroupDataPrivate();
    ~QScriptDebuggerConsoleCommandGroupDataPrivate();

    QString shortDescription;
    QString longDescription;

    QBasicAtomicInt ref;
};

QScriptDebuggerConsoleCommandGroupDataPrivate::QScriptDebuggerConsoleCommandGroupDataPrivate()
{
    ref = 0;
}

QScriptDebuggerConsoleCommandGroupDataPrivate::~QScriptDebuggerConsoleCommandGroupDataPrivate()
{
}

QScriptDebuggerConsoleCommandGroupData::QScriptDebuggerConsoleCommandGroupData()
    : d_ptr(0)
{
}

QScriptDebuggerConsoleCommandGroupData::QScriptDebuggerConsoleCommandGroupData(
    const QString &shortDescription, const QString &longDescription)
    : d_ptr(new QScriptDebuggerConsoleCommandGroupDataPrivate)
{
    d_ptr->shortDescription = shortDescription;
    d_ptr->longDescription = longDescription;
    d_ptr->ref.ref();
}

QScriptDebuggerConsoleCommandGroupData::QScriptDebuggerConsoleCommandGroupData(
    const QScriptDebuggerConsoleCommandGroupData &other)
    : d_ptr(other.d_ptr)
{
    if (d_ptr)
        d_ptr->ref.ref();
}

QScriptDebuggerConsoleCommandGroupData::~QScriptDebuggerConsoleCommandGroupData()
{
    if (d_ptr && !d_ptr->ref.deref()) {
        delete d_ptr;
        d_ptr = 0;
    }
}

QScriptDebuggerConsoleCommandGroupData &QScriptDebuggerConsoleCommandGroupData::operator=(
    const QScriptDebuggerConsoleCommandGroupData &other)
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

QString QScriptDebuggerConsoleCommandGroupData::shortDescription() const
{
    Q_D(const QScriptDebuggerConsoleCommandGroupData);
    return d->shortDescription;
}

QString QScriptDebuggerConsoleCommandGroupData::longDescription() const
{
    Q_D(const QScriptDebuggerConsoleCommandGroupData);
    return d->longDescription;
}

bool QScriptDebuggerConsoleCommandGroupData::isValid() const
{
    Q_D(const QScriptDebuggerConsoleCommandGroupData);
    return (d != 0);
}

QT_END_NAMESPACE
