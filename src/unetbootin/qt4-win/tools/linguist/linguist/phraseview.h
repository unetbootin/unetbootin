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

#ifndef PHRASEVIEW_H
#define PHRASEVIEW_H

#include <QList>
#include <QShortcut>
#include <QTreeView>
#include "phrase.h"

QT_BEGIN_NAMESPACE

class MessageModel;
class PhraseModel;

class GuessShortcut : public QShortcut
{
    Q_OBJECT
public:
    GuessShortcut(int nkey, QWidget *parent, const char *member)
        : QShortcut(parent), nrkey(nkey)
    {
        setKey(Qt::CTRL + (Qt::Key_1 + nrkey));
        connect(this, SIGNAL(activated()), this, SLOT(keyActivated()));
        connect(this, SIGNAL(activated(int)), parent, member);
    }

private slots:
    void keyActivated() { emit activated(nrkey); }

signals:
    void activated(int nkey);

private:
    int nrkey;
};

class PhraseView : public QTreeView
{
    Q_OBJECT

public:
    PhraseView(MessageModel *model, QHash<QString, QList<Phrase *> > *phraseDict, QWidget *parent = 0);
    ~PhraseView();
    void setSourceText(QString sourceText);

public slots:
    void toggleGuessing();
    void update();

signals:
    void phraseSelected(const QString &phrase);

protected:
    // QObject
    virtual void contextMenuEvent(QContextMenuEvent *event);
    // QAbstractItemView
    virtual void mouseDoubleClickEvent(QMouseEvent *event);

private slots:
    void guessShortcut(int nkey);
    void selectPhrase(const QModelIndex &index);
    void selectPhrase();
    void editPhrase();

private:
    QList<Phrase *> getPhrases(const QString &sourceText);
    void deleteGuesses();

    MessageModel *m_messageModel;
    QHash<QString, QList<Phrase *> > *m_phraseDict;
    QList<Phrase *> m_guesses;
    PhraseModel *m_phraseModel;
    QString m_sourceText;
    bool m_doGuesses;
};

QT_END_NAMESPACE

#endif // PHRASEVIEW_H
