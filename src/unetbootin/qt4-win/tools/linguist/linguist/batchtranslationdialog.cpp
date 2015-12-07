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

#include "batchtranslationdialog.h"
#include <QtCore>
#include <QtGui>
#include <QtGui/QProgressDialog>
#include "phrase.h"
#include "messagemodel.h"

QT_BEGIN_NAMESPACE

CheckableListModel::CheckableListModel(QObject *parent)
: QStandardItemModel(parent)
{
}

Qt::ItemFlags CheckableListModel::flags(const QModelIndex &/*index*/) const
{
    return Qt::ItemIsUserCheckable | Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

BatchTranslationDialog::BatchTranslationDialog(MessageModel *model, QWidget *w)
 : QDialog(w), m_model(this), m_messagemodel(model)
{
    m_ui.setupUi(this);
    connect(m_ui.runButton, SIGNAL(clicked()), this, SLOT(startTranslation()));
    connect(m_ui.cancelButton, SIGNAL(clicked()), this, SLOT(reject()));
    connect(m_ui.moveUpButton, SIGNAL(clicked()), this, SLOT(movePhraseBookUp()));
    connect(m_ui.moveDownButton, SIGNAL(clicked()), this, SLOT(movePhraseBookDown()));

    m_ui.phrasebookList->setModel(&m_model);
    m_ui.phrasebookList->setSelectionBehavior(QAbstractItemView::SelectItems);
    m_ui.phrasebookList->setSelectionMode(QAbstractItemView::SingleSelection);
}


void BatchTranslationDialog::setPhraseBooks(const QList<PhraseBook *> &phrasebooks)
{
    m_model.clear();
    m_model.insertColumn(0);
    m_phrasebooks = phrasebooks;
    int count = phrasebooks.count();
    m_model.insertRows(0, count);
    for (int i = 0; i < count; ++i) {
        QString name = phrasebooks[i]->friendlyPhraseBookName();
        m_model.setData(m_model.index(i, 0), name);
        m_model.setData(m_model.index(i, 0), Qt::Checked, Qt::CheckStateRole);
    }
}

PhraseBook *BatchTranslationDialog::GetNamedPhraseBook(const QString &name)
{
    for (int i = 0; i < m_phrasebooks.count(); ++i) {
        if (m_phrasebooks[i]->friendlyPhraseBookName() == name) return m_phrasebooks[i];
    }
    return 0;
}

void BatchTranslationDialog::startTranslation()
{
    int translatedcount = 0;
    QCursor oldCursor = cursor();
    setCursor(Qt::BusyCursor);
    int messageCount = m_messagemodel->getMessageCount();

    QProgressDialog *dlgProgress;
    dlgProgress = new QProgressDialog(tr("Searching, please wait..."), tr("&Cancel"), 0, messageCount, this);
    dlgProgress->show();

    MessageModel::iterator it = m_messagemodel->begin();
    int msgidx = 0;
    bool doProcess = true;
    for ( ;it.current() && doProcess; ++it) {
        MessageItem *m  = it.current();
        if ( m_ui.ckOnlyUntranslated->isChecked() ) {
            if (!m->translation().isEmpty()) continue;
        }
        // Go through them in the order the user specified in the phrasebookList
        for (int b = 0; b < m_model.rowCount() && doProcess; ++b) {
            QVariant checkState = m_model.data(m_model.index(b, 0), Qt::CheckStateRole);
            if (checkState == Qt::Checked) {
                QVariant pbname = m_model.data(m_model.index(b, 0));
                PhraseBook *pb = GetNamedPhraseBook(pbname.toString());
                foreach (const Phrase *ph, pb->phrases()) {
                    if (ph->source() == m->sourceText() && !m->finished()) {
                        m->setTranslation(ph->target());
                        m->setFinished(m_ui.ckMarkFinished->isChecked());
                        ++translatedcount;
                    }
                }
                if (dlgProgress->wasCanceled()) {
                    doProcess = false;
                    break;
                }
            }
            qApp->processEvents();
        }
        ++msgidx;
        dlgProgress->setValue(msgidx);
    }
    dlgProgress->setValue(messageCount);

    setCursor(oldCursor);
    m_messagemodel->updateAll();
    emit finished();
    QMessageBox::information(this, tr("Linguist batch translator"),
        tr("Batch translated %n entries", "", translatedcount), QMessageBox::Ok);

    //### update stats
    //### update translationcount etc.
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
