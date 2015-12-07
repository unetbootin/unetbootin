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

#include <QtGui/QKeyEvent>
#include "filespage.h"

QT_BEGIN_NAMESPACE

FilesPage::FilesPage(QWidget *parent)
    : QWizardPage(parent)
{
    setTitle(tr("Unreferenced Files"));
    setSubTitle(tr("Remove files which are neither referenced "
        "by a keyword nor by the TOC."));

    m_ui.setupUi(this);
    m_ui.fileListWidget->setSelectionMode(QAbstractItemView::ExtendedSelection);
    m_ui.fileListWidget->installEventFilter(this);
    connect(m_ui.removeButton, SIGNAL(clicked()),
        this, SLOT(removeFile()));
    connect(m_ui.removeAllButton, SIGNAL(clicked()),
        this, SLOT(removeAllFiles()));

    m_ui.fileLabel->setText(tr("<p><b>Warning:</b> Be aware "
        "when removing images or stylesheets since those files "
        "are not directly referenced by the .adp or .dcf "
        "file.</p>"));
}

void FilesPage::setFilesToRemove(const QStringList &files)
{
    m_files = files;
    m_ui.fileListWidget->clear();
    m_ui.fileListWidget->addItems(files);
}

QStringList FilesPage::filesToRemove() const
{
    return m_filesToRemove;
}

void FilesPage::removeFile()
{
    int row = m_ui.fileListWidget->currentRow()
        - m_ui.fileListWidget->selectedItems().count() + 1;
    foreach (const QListWidgetItem *item, m_ui.fileListWidget->selectedItems()) {
        m_filesToRemove.append(item->text());
        delete item;
    }
    if (m_ui.fileListWidget->count() > row && row >= 0)
        m_ui.fileListWidget->setCurrentRow(row);
    else
        m_ui.fileListWidget->setCurrentRow(m_ui.fileListWidget->count());
}

void FilesPage::removeAllFiles()
{
    m_ui.fileListWidget->clear();
    m_filesToRemove = m_files;
}

bool FilesPage::eventFilter(QObject *obj, QEvent *event)
{
    if (obj == m_ui.fileListWidget && event->type() == QEvent::KeyPress) {
        QKeyEvent *ke = static_cast<QKeyEvent*>(event);
        if (ke->key() == Qt::Key_Delete) {
            removeFile();
            return true;
        }
    }
    return QWizardPage::eventFilter(obj, event);
}

QT_END_NAMESPACE
