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

#include "bookmarkdialog.h"
#include "bookmarkwidget.h"

#include <QtHelp/QHelpEngine>

QT_BEGIN_NAMESPACE

BookmarkDialog::BookmarkDialog(QHelpEngine *engine, const QString &title, 
                               const QString &url, QWidget *parent)
    : QDialog(parent)
    , m_url(url)
    , m_title(title)
    , helpEngine(engine)
{
    ui.setupUi(this);

    ui.bookmarkEdit->setText(title);
    ui.bookmarkWidget->setVisible(false);
    ui.newFolderButton->setVisible(false);
    ui.bookmarkWidget->setHeaderLabel(tr("Bookmarks"));
    ui.buttonBox->button(QDialogButtonBox::Ok)->setDefault(true);

    ui.bookmarkWidget->setBookmarks(engine->customValue(QLatin1String("Bookmarks")).toByteArray());
    connect(ui.bookmarkWidget, SIGNAL(currentItemChanged(const QString&)),
        this, SLOT(currentItemChanged(const QString&)));
    connect(ui.bookmarkWidget->selectionModel(), SIGNAL(currentChanged(const QModelIndex&, const QModelIndex&)),
        this, SLOT(currentChanged(const QModelIndex&, const QModelIndex&)));
    
    ui.bookmarkFolders->addItems(ui.bookmarkWidget->bookmarkFolders());
    connect(ui.bookmarkFolders, SIGNAL(currentIndexChanged(const QString&)), 
        ui.bookmarkWidget, SLOT(selectBookmarkFolder(const QString&)));

    connect(ui.bookmarkEdit, SIGNAL(textChanged(const QString&)), 
        this, SLOT(textChanged(const QString&)));
    
    connect(ui.toolButton, SIGNAL(clicked()), this, SLOT(toolButtonClicked()));
    connect(ui.newFolderButton, SIGNAL(clicked()), this, SLOT(addNewFolder()));

    connect(ui.buttonBox, SIGNAL(accepted()), this, SLOT(addAccepted()));
    connect(ui.buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
}

BookmarkDialog::~BookmarkDialog()
{
    // nothing todo
}

void BookmarkDialog::addAccepted()
{
    ui.bookmarkWidget->addNewBookmark(ui.bookmarkEdit->text(), m_url);
    helpEngine->setCustomValue(QLatin1String("Bookmarks"), ui.bookmarkWidget->bookmarks());
    accept();
}

void BookmarkDialog::addNewFolder()
{
    ui.bookmarkWidget->addNewFolder(QLatin1String(""));
}

void BookmarkDialog::toolButtonClicked()
{
    bool visible = !ui.bookmarkWidget->isVisible();
    ui.bookmarkWidget->setVisible(visible);
    ui.newFolderButton->setVisible(visible);

    if (visible) {
        resize(QSize(width(), 400));
        ui.toolButton->setText(QLatin1String("-"));
    } else {
        resize(width(), minimumHeight());
        ui.toolButton->setText(QLatin1String("+"));
    }
}

void BookmarkDialog::textChanged(const QString &text)
{
    ui.buttonBox->button(QDialogButtonBox::Ok)->setEnabled(!text.isEmpty());
}

void BookmarkDialog::currentItemChanged(const QString &title)
{
    QString name = title;
    if (name.isEmpty())
        name = tr("Bookmarks");

    QString current = ui.bookmarkFolders->currentText();
    QStringList list = ui.bookmarkWidget->bookmarkFolders();
    if (list.contains(name) && name != current) {
        ui.bookmarkFolders->clear();
        ui.bookmarkFolders->addItems(list);
        ui.bookmarkFolders->setCurrentIndex(ui.bookmarkFolders->findText(name));
    }
}

void BookmarkDialog::currentChanged(const QModelIndex& current, const QModelIndex& previous)
{
    Q_UNUSED(previous)

    if (!current.isValid()) {
        ui.bookmarkFolders->setCurrentIndex(ui.bookmarkFolders->findText(tr("Bookmarks")));
        return;
    }

    ui.bookmarkFolders->setCurrentIndex(ui.bookmarkFolders->findText(current.data().toString()));
}

QT_END_NAMESPACE
