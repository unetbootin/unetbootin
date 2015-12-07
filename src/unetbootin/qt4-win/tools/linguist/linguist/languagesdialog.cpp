/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the Qt Linguist of the Qt Toolkit.
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

#include "languagesdialog.h"
#include "trwindow.h"
#include "languagesmanager.h"
#include "messagemodel.h"
#include <QFileDialog>
#include <QCheckBox>

QT_BEGIN_NAMESPACE

LanguagesDialog::LanguagesDialog(LanguagesManager *languagesManager, QWidget *w) : QDialog(w), m_languagesManager(languagesManager)
{
    setupUi(this);
    openFileButton->setIcon(QIcon(TrWindow::resourcePrefix() + QLatin1String("/fileopen.png")));
    connect(languagesList, SIGNAL(itemSelectionChanged()), this, SLOT(selectionChanged()));
    connect(m_languagesManager, SIGNAL(listChanged()), this, SLOT(messageModelListChanged()));

    setupList();
    update();
}

void LanguagesDialog::messageModelListChanged()
{
    setupList();
    update();
}

void LanguagesDialog::setupList()
{
    int row = selectedRow();
    languagesList->clear();

    foreach (MessageModel *messageModel, m_languagesManager->auxModels()) {
        QString locale(localeString(messageModel->language(), messageModel->country()));
        QTreeWidgetItem *item = new QTreeWidgetItem();
        item->setText(0, locale);
        item->setText(1, QFileInfo(messageModel->srcFileName()).fileName());
        languagesList->addTopLevelItem(item);
    }

    selectRow(row);
}

QString LanguagesDialog::localeString(QLocale::Language lang, QLocale::Country country)
{
    return QLocale::languageToString(lang)
        + QLatin1Char('/') + QLocale::countryToString(country);
}

int LanguagesDialog::selectedRow()
{
    QList<QTreeWidgetItem *> rows = languagesList->selectedItems();
    if (rows.isEmpty())
        return -1;
    return languagesList->indexOfTopLevelItem(rows.first());
}

void LanguagesDialog::selectRow(int row)
{
    if (row >= 0)
        languagesList->setCurrentItem(languagesList->topLevelItem(row));
}

void LanguagesDialog::on_openFileButton_clicked()
{
    QFileInfo mainFile(m_languagesManager->mainModel()->srcFileName());
    QString mainFileDir = mainFile.absolutePath();
    QString mainFileTrunk = mainFile.fileName().split(QLatin1Char('_')).first();
    QString mainFileSuffix = mainFile.suffix();

    QStringList fileNames = QFileDialog::getOpenFileNames(this, tr("Open Translation File"), mainFileDir,
        tr("Qt translation sources (%1);;"
           "Qt translation sources (*.ts);;"
           "XLIFF localization files (*.xlf);;"
           "All files (*)").arg(
           mainFileTrunk+QLatin1String("*.")+mainFileSuffix));
    foreach (const QString &name, fileNames)
        m_languagesManager->openAuxLanguageFile(name);
}

void LanguagesDialog::on_removeButton_clicked()
{
    m_languagesManager->removeAuxLanguage(m_languagesManager->auxModels().at(selectedRow()));
}

void LanguagesDialog::on_upButton_clicked()
{
    const int row = selectedRow();
    selectRow(row - 1);
    MessageModel *model = m_languagesManager->auxModels().at(row);
    m_languagesManager->moveAuxLanguage(model, m_languagesManager->getPos(model) - 1);
}

void LanguagesDialog::on_downButton_clicked()
{
    const int row = selectedRow();
    selectRow(row + 1);
    MessageModel *model = m_languagesManager->auxModels().at(row);
    m_languagesManager->moveAuxLanguage(model, m_languagesManager->getPos(model) + 1);
}

void LanguagesDialog::selectionChanged()
{
    const int row = selectedRow();
    const bool auxLangSelected = row >= 0;

    removeButton->setEnabled(auxLangSelected);
    upButton->setEnabled(auxLangSelected && row != 0);
    downButton->setEnabled(auxLangSelected && row != languagesList->topLevelItemCount() - 1);
}

QT_END_NAMESPACE
