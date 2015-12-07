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

#include "qscriptcompletiontaskinterface_p.h"
#include "qscriptcompletiontaskinterface_p_p.h"

#include "private/qobject_p.h"

QT_BEGIN_NAMESPACE

QScriptCompletionTaskInterfacePrivate::QScriptCompletionTaskInterfacePrivate()
{
    type = QScriptCompletionTaskInterface::NoCompletion;
}

QScriptCompletionTaskInterfacePrivate::~QScriptCompletionTaskInterfacePrivate()
{
}

QScriptCompletionTaskInterface::~QScriptCompletionTaskInterface()
{
}

QScriptCompletionTaskInterface::QScriptCompletionTaskInterface(
    QScriptCompletionTaskInterfacePrivate &dd, QObject *parent)
    : QObject(dd, parent)
{
}

QScriptCompletionTaskInterface::CompletionType QScriptCompletionTaskInterface::completionType() const
{
    Q_D(const QScriptCompletionTaskInterface);
    return static_cast<QScriptCompletionTaskInterface::CompletionType>(d->type);
}

int QScriptCompletionTaskInterface::resultCount() const
{
    Q_D(const QScriptCompletionTaskInterface);
    return d->results.size();
}

QString QScriptCompletionTaskInterface::resultAt(int index) const
{
    Q_D(const QScriptCompletionTaskInterface);
    return d->results.value(index);
}

void QScriptCompletionTaskInterface::addResult(const QString &result)
{
    Q_D(QScriptCompletionTaskInterface);
    d->results.append(result);
}

int QScriptCompletionTaskInterface::position() const
{
    Q_D(const QScriptCompletionTaskInterface);
    return d->position;
}

int QScriptCompletionTaskInterface::length() const
{
    Q_D(const QScriptCompletionTaskInterface);
    return d->length;
}

QString QScriptCompletionTaskInterface::appendix() const
{
    Q_D(const QScriptCompletionTaskInterface);
    return d->appendix;
}

QT_END_NAMESPACE
