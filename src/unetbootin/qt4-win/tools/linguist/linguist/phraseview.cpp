/****************************************************************************
**
** Copyright (C) 2007-2008 Trolltech ASA. All rights reserved.
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

#include <QHeaderView>
#include <QKeyEvent>
#include <QTreeView>
#include <QWidget>
#include <QDebug>

#include "messagemodel.h"
#include "phrase.h"
#include "phraseview.h"
#include "phrasemodel.h"
#include "simtexth.h"
#include "trwindow.h"

QT_BEGIN_NAMESPACE

// Maximum number of guesses to display
static const int MaxCandidates = 5;


PhraseView::PhraseView(MessageModel *model, QHash<QString, QList<Phrase *> > *phraseDict, QWidget *parent)
    : QTreeView(parent),
      m_messageModel(model),
      m_phraseDict(phraseDict),
      m_doGuesses(true)
{
    setObjectName(QLatin1String("phrase list view"));

    m_phraseModel = new PhraseModel(this);

    setModel(m_phraseModel);
    setAlternatingRowColors(true);
    setSelectionBehavior(QAbstractItemView::SelectRows);
    setSelectionMode(QAbstractItemView::SingleSelection);
    setRootIsDecorated(false);
    setItemsExpandable(false);

    for (int i = 0; i < 10; i++)
        (void) new GuessShortcut(i, this, SLOT(guessShortcut(int)));

    header()->setResizeMode(QHeaderView::Stretch);
    header()->setClickable(true);

    connect(this, SIGNAL(activated(const QModelIndex&)), this, SLOT(selectPhrase(const QModelIndex&)));
}

PhraseView::~PhraseView()
{
    deleteGuesses();
}

void PhraseView::toggleGuessing()
{
    m_doGuesses = !m_doGuesses;
    update();
}

void PhraseView::update()
{
    setSourceText(m_sourceText);
}


void PhraseView::contextMenuEvent(QContextMenuEvent *event)
{
    QModelIndex index = indexAt(event->pos());
    if (!index.isValid())
        return;

    QMenu *contextMenu = new QMenu(this);

    QAction *insertAction = new QAction(tr("Insert"), contextMenu);
    connect(insertAction, SIGNAL(triggered()), this, SLOT(selectPhrase()));

    QAction *editAction = new QAction(tr("Edit"), contextMenu);
    connect(editAction, SIGNAL(triggered()), this, SLOT(editPhrase()));
    editAction->setEnabled(model()->flags(index) & Qt::ItemIsEditable);

    contextMenu->addAction(insertAction);
    contextMenu->addAction(editAction);

    contextMenu->exec(event->globalPos());
    event->accept();
}

void PhraseView::mouseDoubleClickEvent(QMouseEvent *event)
{
    QModelIndex index = indexAt(event->pos());
    if (!index.isValid()) 
        return;

    emit phraseSelected(m_phraseModel->phrase(index)->target());
    event->accept();
}

void PhraseView::guessShortcut(int key)
{
    foreach (const Phrase *phrase, m_phraseModel->phraseList())
        if (phrase->shortcut() == key) {
            emit phraseSelected(phrase->target());
            return;
        }
}

void PhraseView::selectPhrase(const QModelIndex &index)
{
    emit phraseSelected(m_phraseModel->phrase(index)->target());
}

void PhraseView::selectPhrase()
{
    emit phraseSelected(m_phraseModel->phrase(currentIndex())->target());
}

void PhraseView::editPhrase()
{
    edit(currentIndex());
}

static CandidateList similarTextHeuristicCandidates( MessageModel::iterator it,
                        const char *text,
                        int maxCandidates )
{
    QList<int> scores;
    CandidateList candidates;

    StringSimilarityMatcher stringmatcher(QString::fromLatin1(text));

    for (MessageItem *m = 0; (m = it.current()); ++it) {
        TranslatorMessage mtm = m->message();
        if ( mtm.type() == TranslatorMessage::Unfinished ||
             mtm.translation().isEmpty() )
            continue;

        QString s = m->sourceText();

        int score = stringmatcher.getSimilarityScore(s);

        if ( (int) candidates.count() == maxCandidates &&
             score > scores[maxCandidates - 1] )
            candidates.removeAt( candidates.size()-1 );
        if ( (int) candidates.count() < maxCandidates && score >= textSimilarityThreshold ) {
            Candidate cand( s, mtm.translation() );

            int i;
            for ( i = 0; i < (int) candidates.size(); i++ ) {
                if ( score >= scores.at(i) ) {
                    if ( score == scores.at(i) ) {
                        if ( candidates.at(i) == cand )
                            goto continue_outer_loop;
                    } else {
                        break;
                    }
                }
            }
            scores.insert( i, score );
            candidates.insert( i, cand );
        }
        continue_outer_loop:
        ;
    }
    return candidates;
}


void PhraseView::setSourceText(QString sourceText)
{
    m_sourceText = sourceText;
    m_phraseModel->removePhrases();
    deleteGuesses();

    foreach (Phrase *p, getPhrases(sourceText)) {
        m_phraseModel->addPhrase(p);
    }

    if (!sourceText.isEmpty() && m_doGuesses) {
        CandidateList cl = similarTextHeuristicCandidates(m_messageModel->begin(),
            sourceText.toLatin1(), MaxCandidates);
        int n = 0;
        foreach (const Candidate &candidate, cl) {
            QString def;
            if (n < 9)
                def = tr("Guess (%1)").arg(QString(QKeySequence(Qt::CTRL | (Qt::Key_0 + (n + 1)))));
            else
                def = tr("Guess");
            Phrase *guess = new Phrase(candidate.source, candidate.target, def, n);
            m_guesses.append(guess);
            m_phraseModel->addPhrase(guess);
            ++n;
        }
    }
}

QList<Phrase *> PhraseView::getPhrases(const QString &source)
{
    QList<Phrase *> phrases;
    QString f = TrWindow::friendlyString(source);
    QStringList lookupWords = f.split(QLatin1Char(' '));

    foreach (const QString &s, lookupWords) {
        if (m_phraseDict->contains(s)) {
            foreach (Phrase *p, m_phraseDict->value(s)) {
                if (f.indexOf(TrWindow::friendlyString(p->source())) >= 0)
                    phrases.append(p);
            }
        }
    }
    return phrases;
}

void PhraseView::deleteGuesses()
{
    foreach (Phrase *guess, m_guesses)
        delete guess;
    m_guesses.clear();
}

QT_END_NAMESPACE
