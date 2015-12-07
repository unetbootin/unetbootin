/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the Qt Linguist of the Qt Toolkit.
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

/*  TRANSLATOR FindDialog

    Choose Edit|Find from the menu bar or press Ctrl+F to pop up the
    Find dialog
*/

#include "finddialog.h"

QT_BEGIN_NAMESPACE

FindDialog::FindDialog(QWidget *parent)
    : QDialog(parent)
{
    setupUi(this);

    findNxt->setEnabled(false);

    connect(findNxt, SIGNAL(clicked()), this, SLOT(emitFindNext()));
    connect(led, SIGNAL(textChanged(const QString &)), this, SLOT(verifyText(const QString &)));

    led->setFocus();
}

void FindDialog::verifyText(const QString &text)
{
    findNxt->setEnabled(!text.isEmpty());
}

void FindDialog::emitFindNext()
{
    DataModel::FindLocation where;
    if (sourceText != 0)
        where =
            DataModel::FindLocation(
                (sourceText->isChecked() ? DataModel::SourceText : 0) |
                (translations->isChecked() ? DataModel::Translations : 0) |
                (comments->isChecked() ? DataModel::Comments : 0));
    else
        where = DataModel::Translations;
    emit findNext(led->text(), where, matchCase->isChecked(), ignoreAccelerators->isChecked());
    led->selectAll();
}

void FindDialog::find()
{
    led->setFocus();

    show();
    activateWindow();
    raise();
}

QT_END_NAMESPACE
