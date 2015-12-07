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

/*  TRANSLATOR PhraseBookBox

  Go to Phrase > Edit Phrase Book...  The dialog that pops up is a
  PhraseBookBox.
*/

#include "phrasebookbox.h"

#include <QtEvents>
#include <QLineEdit>
#include <QMessageBox>
#include <QHeaderView>
#include <QSortFilterProxyModel>

QT_BEGIN_NAMESPACE

#define NewPhrase tr("(New Phrase)")

PhraseBookBox::PhraseBookBox(PhraseBook *phraseBook, QWidget *parent)
    : QDialog(parent),
      m_phraseBook(phraseBook)
{
    setupUi(this);
    setWindowTitle(tr("%1 - %2[*]").arg(tr("Qt Linguist")).arg(m_phraseBook->friendlyPhraseBookName()));
    setWindowModified(m_phraseBook->isModified());

    phrMdl = new PhraseModel(this);

    m_sortedPhraseModel = new QSortFilterProxyModel(this);
    m_sortedPhraseModel->setSortCaseSensitivity(Qt::CaseInsensitive);
    m_sortedPhraseModel->setSortLocaleAware(true);
    m_sortedPhraseModel->setDynamicSortFilter(true);
    m_sortedPhraseModel->setSourceModel(phrMdl);

    phraseList->setModel(m_sortedPhraseModel);
    phraseList->header()->setDefaultSectionSize(150);
    phraseList->header()->setResizeMode(QHeaderView::Interactive);

    connect(sourceLed, SIGNAL(textChanged(QString)),
            this, SLOT(sourceChanged(QString)));
    connect(targetLed, SIGNAL(textChanged(QString)),
            this, SLOT(targetChanged(QString)));
    connect(definitionLed, SIGNAL(textChanged(QString)),
            this, SLOT(definitionChanged(QString)));
    connect(phraseList->selectionModel(), SIGNAL(currentChanged(const QModelIndex &, const QModelIndex &)),
            this, SLOT(selectionChanged()));
    connect(newBut, SIGNAL(clicked()), this, SLOT(newPhrase()));
    connect(removeBut, SIGNAL(clicked()), this, SLOT(removePhrase()));
    connect(saveBut, SIGNAL(clicked()), this, SLOT(save()));
    connect(closeBut, SIGNAL(clicked()), this, SLOT(accept()));
    connect(m_phraseBook, SIGNAL(modifiedChanged(bool)), this, SLOT(setWindowModified(bool)));

    sourceLed->installEventFilter(this);
    targetLed->installEventFilter(this);
    definitionLed->installEventFilter(this);

    foreach (Phrase *p, phraseBook->phrases())
        phrMdl->addPhrase(p);

    phraseList->sortByColumn(0, Qt::AscendingOrder);

    enableDisable();
}

bool PhraseBookBox::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::KeyPress &&
            (obj == sourceLed || obj == targetLed || obj == definitionLed))
    {
        const QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
        const int key = keyEvent->key();

        switch (key) {
        case Qt::Key_Down:
        case Qt::Key_Up:
        case Qt::Key_PageDown:
        case Qt::Key_PageUp:
            return QApplication::sendEvent(phraseList, event);
        }
    }
    return QDialog::eventFilter(obj, event);
}

void PhraseBookBox::newPhrase()
{
    Phrase *p = new Phrase();
    p->setSource(NewPhrase);
    m_phraseBook->append(p);
    selectItem(phrMdl->addPhrase(p));
}

void PhraseBookBox::removePhrase()
{
    QModelIndex index = currentPhraseIndex();
    Phrase *phrase = phrMdl->phrase(index);
    m_phraseBook->remove(phrase);
    phrMdl->removePhrase(index);
    delete phrase;
}

void PhraseBookBox::save()
{
    const QString &fileName = m_phraseBook->fileName();
    if (!m_phraseBook->save(fileName))
        QMessageBox::warning(this,
                             tr("Qt Linguist"),
                             tr("Cannot save phrase book '%1'.").arg(fileName));
}

void PhraseBookBox::sourceChanged(const QString& source)
{
    QModelIndex index = currentPhraseIndex();
    if (index.isValid())
        phrMdl->setData(phrMdl->index(index.row(), 0), source);
}

void PhraseBookBox::targetChanged(const QString& target)
{
    QModelIndex index = currentPhraseIndex();
    if (index.isValid())
        phrMdl->setData(phrMdl->index(index.row(), 1), target);
}

void PhraseBookBox::definitionChanged(const QString& definition)
{
    QModelIndex index = currentPhraseIndex();
    if (index.isValid())
        phrMdl->setData(phrMdl->index(index.row(), 2), definition);
}

void PhraseBookBox::selectionChanged()
{
    enableDisable();
}

void PhraseBookBox::selectItem(const QModelIndex &index)
{
    const QModelIndex &sortedIndex = m_sortedPhraseModel->mapFromSource(index);
    phraseList->scrollTo(sortedIndex);
    phraseList->setCurrentIndex(sortedIndex);
}

void PhraseBookBox::enableDisable()
{
    QModelIndex index = currentPhraseIndex();

    sourceLed->blockSignals(true);
    targetLed->blockSignals(true);
    definitionLed->blockSignals(true);

    bool indexValid = index.isValid();

    if (indexValid) {
        Phrase *p = phrMdl->phrase(index);
        sourceLed->setText(p->source().simplified());
        targetLed->setText(p->target().simplified());
        definitionLed->setText(p->definition());
    }
    else {
        sourceLed->setText(QString());
        targetLed->setText(QString());
        definitionLed->setText(QString());
    }

    sourceLed->setEnabled(indexValid);
    targetLed->setEnabled(indexValid);
    definitionLed->setEnabled(indexValid);
    removeBut->setEnabled(indexValid);

    sourceLed->blockSignals(false);
    targetLed->blockSignals(false);
    definitionLed->blockSignals(false);

    QWidget *f = QApplication::focusWidget();
    if (f != sourceLed && f != targetLed && f != definitionLed) {
        QLineEdit *led = (sourceLed->text() == NewPhrase ? sourceLed : targetLed);
        led->setFocus();
        led->selectAll();
    } else {
        static_cast<QLineEdit*>(f)->selectAll();
    }
}

QModelIndex PhraseBookBox::currentPhraseIndex() const
{
    return m_sortedPhraseModel->mapToSource(phraseList->currentIndex());
}

QT_END_NAMESPACE
