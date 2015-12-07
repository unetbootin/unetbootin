/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the Qt Assistant of the Qt Toolkit.
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

#include <QtGui/QFileDialog>

#include "pathpage.h"

QT_BEGIN_NAMESPACE

PathPage::PathPage(QWidget *parent)
    : QWizardPage(parent)
{
    setTitle(tr("Source File Paths"));
    setSubTitle(tr("Specify the paths where the sources files "
        "are located. By default, all files in those directories "
        "matched by the file filter will be included."));

    m_ui.setupUi(this);
    connect(m_ui.addButton, SIGNAL(clicked()),
        this, SLOT(addPath()));
    connect(m_ui.removeButton, SIGNAL(clicked()),
        this, SLOT(removePath()));

    m_ui.filterLineEdit->setText(QLatin1String("*.html, *.htm, *.png, *.jpg, *.css"));

    registerField(QLatin1String("sourcePathList"), m_ui.pathListWidget);
    m_firstTime = true;
}

void PathPage::setPath(const QString &path)
{
    if (!m_firstTime)
        return;

    m_ui.pathListWidget->addItem(path);
    m_firstTime = false;
    m_ui.pathListWidget->setCurrentRow(0);
}

QStringList PathPage::paths() const
{
    QStringList lst;
    for (int i = 0; i<m_ui.pathListWidget->count(); ++i)
        lst.append(m_ui.pathListWidget->item(i)->text());
    return lst;
}

QStringList PathPage::filters() const
{
    QStringList lst;
    foreach (const QString &s, m_ui.filterLineEdit->text().split(QLatin1Char(','))) {
        lst.append(s.trimmed());
    }
    return lst;
}

void PathPage::addPath()
{
    QString dir = QFileDialog::getExistingDirectory(this,
        tr("Source File Path"));
    if (!dir.isEmpty())
        m_ui.pathListWidget->addItem(dir);
}

void PathPage::removePath()
{
    QListWidgetItem *i = m_ui.pathListWidget
        ->takeItem(m_ui.pathListWidget->currentRow());
    delete i;
    if (!m_ui.pathListWidget->count())
        m_ui.removeButton->setEnabled(false);
}

QT_END_NAMESPACE
