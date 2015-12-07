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

#ifndef PHRASE_H
#define PHRASE_H

#include <QObject>
#include <QString>
#include <QList>

QT_BEGIN_NAMESPACE

class PhraseBook;

class Phrase
{
public:
    Phrase();
    Phrase(const QString &source, const QString &target,
            const QString &definition, int sc = -1);
    Phrase(const QString &source, const QString &target,
            const QString &definition, PhraseBook *phraseBook);

    QString source() const { return s; }
    void setSource(const QString &ns);
    QString target() const {return t;}
    void setTarget(const QString &nt);
    QString definition() const {return d;}
    void setDefinition (const QString &nd);
    int shortcut() const { return shrtc; }
    PhraseBook *phraseBook() const { return m_phraseBook; }
    void setPhraseBook(PhraseBook *book) { m_phraseBook = book; }

private:
    int shrtc;
    QString s;
    QString t;
    QString d;
    PhraseBook *m_phraseBook;
};

bool operator==(const Phrase &p, const Phrase &q);
inline bool operator!=(const Phrase &p, const Phrase &q) {
    return !(p == q);
}

class QphHandler;

class PhraseBook : public QObject
{
    Q_OBJECT

public:
    PhraseBook();
    ~PhraseBook();
    bool load(const QString &fileName);
    bool save(const QString &fileName);
    const QList<Phrase *> phrases() { return m_phrases; }
    void append(Phrase *phrase);
    void remove(Phrase *phrase);
    const QString fileName() const { return m_fileName; }
    const QString friendlyPhraseBookName() const;
    bool isModified() { return m_changed; }

signals:
    void modifiedChanged(bool changed);
    void listChanged();

private:
    // Prevent copying
    PhraseBook(const PhraseBook &);
    PhraseBook& operator=(const PhraseBook &);

    void setModified(bool modified);
    void phraseChanged(Phrase *phrase);

    QList<Phrase *> m_phrases;
    QString m_fileName;
    bool m_changed;

    friend class QphHandler;
    friend class Phrase;
};

QT_END_NAMESPACE

#endif
