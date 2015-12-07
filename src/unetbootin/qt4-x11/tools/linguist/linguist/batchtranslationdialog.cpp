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

#include "batchtranslationdialog.h"
#include "phrase.h"
#include "messagemodel.h"

#include <QtGui/QMessageBox>
#include <QtGui/QProgressDialog>

QT_BEGIN_NAMESPACE

CheckableListModel::CheckableListModel(QObject *parent)
  : QStandardItemModel(parent)
{
}

Qt::ItemFlags CheckableListModel::flags(const QModelIndex &index) const
{
    Q_UNUSED(index);
    return Qt::ItemIsUserCheckable | Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

BatchTranslationDialog::BatchTranslationDialog(MultiDataModel *dataModel, QWidget *w)
 : QDialog(w), m_model(this), m_dataModel(dataModel)
{
    m_ui.setupUi(this);
    connect(m_ui.runButton, SIGNAL(clicked()), this, SLOT(startTranslation()));
    connect(m_ui.moveUpButton, SIGNAL(clicked()), this, SLOT(movePhraseBookUp()));
    connect(m_ui.moveDownButton, SIGNAL(clicked()), this, SLOT(movePhraseBookDown()));

    m_ui.phrasebookList->setModel(&m_model);
    m_ui.phrasebookList->setSelectionBehavior(QAbstractItemView::SelectItems);
    m_ui.phrasebookList->setSelectionMode(QAbstractItemView::SingleSelection);
}


void BatchTranslationDialog::setPhraseBooks(const QList<PhraseBook *> &phrasebooks, int modelIndex)
{
    QString fn = QFileInfo(m_dataModel->srcFileName(modelIndex)).baseName();
    setWindowTitle(tr("Batch Translation of '%1' - Qt Linguist").arg(fn));
    m_model.clear();
    m_model.insertColumn(0);
    m_phrasebooks = phrasebooks;
    m_modelIndex = modelIndex;
    int count = phrasebooks.count();
    m_model.insertRows(0, count);
    for (int i = 0; i < count; ++i) {
        QModelIndex idx(m_model.index(i, 0));
        m_model.setData(idx, phrasebooks[i]->friendlyPhraseBookName());
        int sortOrder;
        if (phrasebooks[i]->language() != QLocale::C
            && m_dataModel->language(m_modelIndex) != QLocale::C) {
            if (phrasebooks[i]->language() != m_dataModel->language(m_modelIndex))
                sortOrder = 3;
            else
                sortOrder = (phrasebooks[i]->country()
                             == m_dataModel->model(m_modelIndex)->country()) ? 0 : 1;
        } else {
            sortOrder = 2;
        }
        m_model.setData(idx, sortOrder == 3 ? Qt::Unchecked : Qt::Checked, Qt::CheckStateRole);
        m_model.setData(idx, sortOrder, Qt::UserRole + 1);
        m_model.setData(idx, i, Qt::UserRole);
    }
    m_model.setSortRole(Qt::UserRole + 1);
    m_model.sort(0);
}

void BatchTranslationDialog::startTranslation()
{
    int translatedcount = 0;
    QCursor oldCursor = cursor();
    setCursor(Qt::BusyCursor);
    int messageCount = m_dataModel->messageCount();

    QProgressDialog *dlgProgress;
    dlgProgress = new QProgressDialog(tr("Searching, please wait..."), tr("&Cancel"), 0, messageCount, this);
    dlgProgress->show();

    int msgidx = 0;
    const bool translateTranslated = m_ui.ckTranslateTranslated->isChecked();
    const bool translateFinished = m_ui.ckTranslateFinished->isChecked();
    for (MultiDataModelIterator it(m_dataModel, m_modelIndex); it.isValid(); ++it) {
        if (MessageItem *m = it.current()) {
            if (!m->isObsolete()
                && (translateTranslated || m->translation().isEmpty())
                && (translateFinished || !m->isFinished())) {

                // Go through them in the order the user specified in the phrasebookList
                for (int b = 0; b < m_model.rowCount(); ++b) {
                    QModelIndex idx(m_model.index(b, 0));
                    QVariant checkState = m_model.data(idx, Qt::CheckStateRole);
                    if (checkState == Qt::Checked) {
                        PhraseBook *pb = m_phrasebooks[m_model.data(idx, Qt::UserRole).toInt()];
                        foreach (const Phrase *ph, pb->phrases()) {
                            if (ph->source() == m->text()) {
                                m_dataModel->setTranslation(it, ph->target());
                                m_dataModel->setFinished(it, m_ui.ckMarkFinished->isChecked());
                                ++translatedcount;
                                goto done; // break 2;
                            }
                        }
                    }
                }
            }
        }
      done:
        ++msgidx;
        if (!(msgidx & 15))
            dlgProgress->setValue(msgidx);
        qApp->processEvents();
        if (dlgProgress->wasCanceled())
            break;
    }
    dlgProgress->hide();

    setCursor(oldCursor);
    emit finished();
    QMessageBox::information(this, tr("Linguist batch translator"),
        tr("Batch translated %n entries", "", translatedcount), QMessageBox::Ok);
}

void BatchTranslationDialog::movePhraseBookUp()
{
    QModelIndexList indexes = m_ui.phrasebookList->selectionModel()->selectedIndexes();
    if (indexes.count() <= 0) return;

    QModelIndex sel = indexes[0];
    int row = sel.row();
    if (row > 0) {
        QModelIndex other = m_model.index(row - 1, 0);
        QMap<int, QVariant> seldata = m_model.itemData(sel);
        m_model.setItemData(sel, m_model.itemData(other));
        m_model.setItemData(other, seldata);
        m_ui.phrasebookList->selectionModel()->setCurrentIndex(other, QItemSelectionModel::ClearAndSelect);
    }
}

void BatchTranslationDialog::movePhraseBookDown()
{
    QModelIndexList indexes = m_ui.phrasebookList->selectionModel()->selectedIndexes();
    if (indexes.count() <= 0) return;

    QModelIndex sel = indexes[0];
    int row = sel.row();
    if (row < m_model.rowCount() - 1) {
        QModelIndex other = m_model.index(row + 1, 0);
        QMap<int, QVariant> seldata = m_model.itemData(sel);
        m_model.setItemData(sel, m_model.itemData(other));
        m_model.setItemData(other, seldata);
        m_ui.phrasebookList->selectionModel()->setCurrentIndex(other, QItemSelectionModel::ClearAndSelect);
    }
}

QT_END_NAMESPACE
