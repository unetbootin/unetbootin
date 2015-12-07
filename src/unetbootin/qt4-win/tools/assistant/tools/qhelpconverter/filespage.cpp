/****************************************************************************
**
** Copyright (C) 2007-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the Qt Assistant of the Qt Toolkit.
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
    foreach (QListWidgetItem *item, m_ui.fileListWidget->selectedItems()) {
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
