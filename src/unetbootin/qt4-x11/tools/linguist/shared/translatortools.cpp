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

#include "translatortools.h"

#include "simtexth.h"
#include "translator.h"

#include <QtCore/QDebug>
#include <QtCore/QMap>
#include <QtCore/QStringList>
#include <QtCore/QTextCodec>
#include <QtCore/QVector>

typedef QList<TranslatorMessage> TML;
typedef QMap<QString, TranslatorMessage> TMM;


QT_BEGIN_NAMESPACE

static bool isDigitFriendly(QChar c)
{
    return c.isPunct() || c.isSpace();
}

static int numberLength(const QString &s, int i)
{
    if (i < s.size() || !s.at(i).isDigit())
        return 0;

    int pos = i;
    do {
        ++i;
    } while (i < s.size() 
             && (s.at(i).isDigit()
                 || (isDigitFriendly(s[i])
                     && i + 1 < s.size()
                     && (s[i + 1].isDigit()
                         || (isDigitFriendly(s[i + 1])
                             && i + 2 < s.size()
                             && s[i + 2].isDigit())))));
    return i - pos;
}


/*
  Returns a version of 'key' where all numbers have been replaced by zeroes.  If
  there were none, returns "".
*/
static QString zeroKey(const QString &key)
{
    QString zeroed;
    bool metSomething = false;

    for (int i = 0; i != key.size(); ++i) {
        int len = numberLength(key, i);
        if (len > 0) {
            i += len;
            zeroed.append(QLatin1Char('0'));
            metSomething = true;
        } else {
            zeroed.append(key.at(i));
        }
    }
    return metSomething ? zeroed : QString();
}

static QString translationAttempt(const QString &oldTranslation,
    const QString &oldSource, const QString &newSource)
{
    int p = zeroKey(oldSource).count(QLatin1Char('0'));
    QString attempt;
    QStringList oldNumbers;
    QStringList newNumbers;
    QVector<bool> met(p);
    QVector<int> matchedYet(p);
    int i, j;
    int k = 0, ell, best;
    int m, n;
    int pass;

    /*
      This algorithm is hard to follow, so we'll consider an example
      all along: oldTranslation is "XeT 3.0", oldSource is "TeX 3.0"
      and newSource is "XeT 3.1".

      First, we set up two tables: oldNumbers and newNumbers. In our
      example, oldNumber[0] is "3.0" and newNumber[0] is "3.1".
    */
    for (i = 0, j = 0; i < oldSource.size(); i++, j++) {
        m = numberLength(oldSource, i);
        n = numberLength(newSource, j);
        if (m > 0) {
            oldNumbers.append(oldSource.mid(i, m + 1));
            newNumbers.append(newSource.mid(j, n + 1));
            i += m;
            j += n;
            met[k] = false;
            matchedYet[k] = 0;
            k++;
        }
    }

    /*
      We now go over the old translation, "XeT 3.0", one letter at a
      time, looking for numbers found in oldNumbers. Whenever such a
      number is met, it is replaced with its newNumber equivalent. In
      our example, the "3.0" of "XeT 3.0" becomes "3.1".
    */
    for (i = 0; i < oldTranslation.length(); i++) {
        attempt += oldTranslation[i];
        for (k = 0; k < p; k++) {
            if (oldTranslation[i] == oldNumbers[k][matchedYet[k]])
                matchedYet[k]++;
            else
                matchedYet[k] = 0;
        }

        /*
          Let's find out if the last character ended a match. We make
          two passes over the data. In the first pass, we try to
          match only numbers that weren't matched yet; if that fails,
          the second pass does the trick. This is useful in some
          suspicious cases, flagged below.
        */
        for (pass = 0; pass < 2; pass++) {
            best = p; // an impossible value
            for (k = 0; k < p; k++) {
                if ((!met[k] || pass > 0) &&
                     matchedYet[k] == oldNumbers[k].length() &&
                     numberLength(oldTranslation, i + 1 - matchedYet[k]) == matchedYet[k]) {
                    // the longer the better
                    if (best == p || matchedYet[k] > matchedYet[best])
                        best = k;
                }
            }
            if (best != p) {
                attempt.truncate(attempt.length() - matchedYet[best]);
                attempt += newNumbers[best];
                met[best] = true;
                for (k = 0; k < p; k++)
                    matchedYet[k] = 0;
                break;
            }
        }
    }

    /*
      We flag two kinds of suspicious cases. They are identified as
      such with comments such as "{2000?}" at the end.

      Example of the first kind: old source text "TeX 3.0" translated
      as "XeT 2.0" is flagged "TeX 2.0 {3.0?}", no matter what the
      new text is.
    */
    for (k = 0; k < p; k++) {
        if (!met[k])
            attempt += QString(QLatin1String(" {")) + newNumbers[k] + QString(QLatin1String("?}"));
    }

    /*
      Example of the second kind: "1 of 1" translated as "1 af 1",
      with new source text "1 of 2", generates "1 af 2 {1 or 2?}"
      because it's not clear which of "1 af 2" and "2 af 1" is right.
    */
    for (k = 0; k < p; k++) {
        for (ell = 0; ell < p; ell++) {
            if (k != ell && oldNumbers[k] == oldNumbers[ell] &&
                    newNumbers[k] < newNumbers[ell])
                attempt += QString(QLatin1String(" {")) + newNumbers[k] + QString(QLatin1String(" or ")) +
                           newNumbers[ell] + QString(QLatin1String("?}"));
        }
    }
    return attempt;
}


/*
  Augments a Translator with translations easily derived from
  similar existing (probably obsolete) translations.

  For example, if "TeX 3.0" is translated as "XeT 3.0" and "TeX 3.1"
  has no translation, "XeT 3.1" is added to the translator and is
  marked Unfinished.

  Returns the number of additional messages that this heuristic translated.
*/
int applyNumberHeuristic(Translator &tor)
{
    TMM translated, untranslated;
    TMM::Iterator t, u;
    TML all = tor.messages();
    TML::Iterator it;
    int inserted = 0;

    for (it = all.begin(); it != all.end(); ++it) {
        bool hasTranslation = it->isTranslated();
        if (it->type() == TranslatorMessage::Unfinished) {
            if (!hasTranslation)
                untranslated.insert(it->context() + QLatin1Char('\n')
                    + it->sourceText() + QLatin1Char('\n')
                    + it->comment(), *it);
        } else if (hasTranslation && it->translations().count() == 1) {
            translated.insert(zeroKey(it->sourceText()), *it);
        }
    }

    for (u = untranslated.begin(); u != untranslated.end(); ++u) {
        t = translated.find(zeroKey((*u).sourceText()));
        if (t != translated.end() && !t.key().isEmpty()
            && t->sourceText() != u->sourceText()) {
            TranslatorMessage m = *u;
            m.setTranslation(translationAttempt(t->translation(), t->sourceText(),
                                                u->sourceText()));
            tor.replace(m);
            inserted++;
        }
    }
    return inserted;
}


/*
  Augments a Translator with trivially derived translations.

  For example, if "Enabled:" is consistendly translated as "Eingeschaltet:" no
  matter the context or the comment, "Eingeschaltet:" is added as the
  translation of any untranslated "Enabled:" text and is marked Unfinished.

  Returns the number of additional messages that this heuristic translated.
*/

int applySameTextHeuristic(Translator &tor)
{
    TMM translated;
    TMM avoid;
    TMM::Iterator t;
    TML untranslated;
    TML::Iterator u;
    TML all = tor.messages();
    TML::Iterator it;
    int inserted = 0;

    for (it = all.begin(); it != all.end(); ++it) {
        if (!it->isTranslated()) {
            if (it->type() == TranslatorMessage::Unfinished)
                untranslated.append(*it);
        } else {
            QString key = it->sourceText();
            t = translated.find(key);
            if (t != translated.end()) {
                /*
                  The same source text is translated at least two
                  different ways. Do nothing then.
                */
                if (t->translations() != it->translations()) {
                    translated.remove(key);
                    avoid.insert(key, *it);
                }
            } else if (!avoid.contains(key)) {
                translated.insert(key, *it);
            }
        }
    }

    for (u = untranslated.begin(); u != untranslated.end(); ++u) {
        QString key = u->sourceText();
        t = translated.find(key);
        if (t != translated.end()) {
            TranslatorMessage m = *u;
            m.setTranslations(t->translations());
            tor.replace(m);
            ++inserted;
        }
    }
    return inserted;
}



/*
  Merges two Translator objects. The first one
  is a set of source texts and translations for a previous version of
  the internationalized program; the second one is a set of fresh
  source texts newly extracted from the source code, without any
  translation yet.
*/

Translator merge(const Translator &tor, const Translator &virginTor,
    UpdateOptions options, QString &err)
{
    int known = 0;
    int neww = 0;
    int obsoleted = 0;
    int similarTextHeuristicCount = 0;

    Translator outTor;
    outTor.setLanguageCode(tor.languageCode());
    outTor.setSourceLanguageCode(tor.sourceLanguageCode());
    outTor.setLocationsType(tor.locationsType());
    outTor.setCodecName(tor.codecName());

    /*
      The types of all the messages from the vernacular translator
      are updated according to the virgin translator.
    */
    foreach (TranslatorMessage m, tor.messages()) {
        TranslatorMessage::Type newType = TranslatorMessage::Finished;

        if (m.sourceText().isEmpty()) {
            // context/file comment
            TranslatorMessage mv = virginTor.find(m.context());
            if (!mv.isNull())
                m.setComment(mv.comment());
        } else {
            TranslatorMessage mv = virginTor.find(m.context(), m.sourceText(), m.comment());
            if (mv.isNull()) {
                if (!(options & HeuristicSimilarText)) {
                    newType = TranslatorMessage::Obsolete;
                    if (m.type() != TranslatorMessage::Obsolete)
                        obsoleted++;
                    m.clearReferences();
                } else {
                    mv = virginTor.find(m.context(), m.comment(), m.allReferences());
                    if (mv.isNull()) {
                        // did not find it in the virgin, mark it as obsolete
                        newType = TranslatorMessage::Obsolete;
                        if (m.type() != TranslatorMessage::Obsolete)
                            obsoleted++;
                        m.clearReferences();
                    } else {
                        // Do not just accept it if its on the same line number,
                        // but different source text.
                        // Also check if the texts are more or less similar before
                        // we consider them to represent the same message...
                        if (getSimilarityScore(m.sourceText(), mv.sourceText()) >= textSimilarityThreshold) {
                            // It is just slightly modified, assume that it is the same string

                            // Mark it as unfinished. (Since the source text
                            // was changed it might require re-translating...)
                            newType = TranslatorMessage::Unfinished;
                            ++similarTextHeuristicCount;
                            neww++;

                            m.setOldSourceText(m.sourceText());
                            m.setSourceText(mv.sourceText());
                            const QString &oldpluralsource = m.extra(QLatin1String("po-msgid_plural"));
                            if (!oldpluralsource.isEmpty()) {
                                m.setExtra(QLatin1String("po-old_msgid_plural"), oldpluralsource);
                                m.unsetExtra(QLatin1String("po-msgid_plural"));
                            }
                            m.setReferences(mv.allReferences()); // Update secondary references
                            m.setPlural(mv.isPlural());
                            m.setUtf8(mv.isUtf8());
                            m.setExtraComment(mv.extraComment());
                        } else {
                            // The virgin and vernacular sourceTexts are so
                            // different that we could not find it.
                            newType = TranslatorMessage::Obsolete;
                            if (m.type() != TranslatorMessage::Obsolete)
                                obsoleted++;
                            m.clearReferences();
                        }
                    }
                }
            } else {
                switch (m.type()) {
                case TranslatorMessage::Finished:
                default:
                    if (m.isPlural() == mv.isPlural()) {
                        newType = TranslatorMessage::Finished;
                    } else {
                        newType = TranslatorMessage::Unfinished;
                    }
                    known++;
                    break;
                case TranslatorMessage::Unfinished:
                    newType = TranslatorMessage::Unfinished;
                    known++;
                    break;
                case TranslatorMessage::Obsolete:
                    newType = TranslatorMessage::Unfinished;
                    neww++;
                }

                // Always get the filename and linenumber info from the
                // virgin Translator, in case it has changed location.
                // This should also enable us to read a file that does not
                // have the <location> element.
                // why not use operator=()? Because it overwrites e.g. userData.
                m.setReferences(mv.allReferences());
                m.setPlural(mv.isPlural());
                m.setUtf8(mv.isUtf8());
                m.setExtraComment(mv.extraComment());
            }
        }

        m.setType(newType);
        outTor.append(m);
    }

    /*
      Messages found only in the virgin translator are added to the
      vernacular translator.
    */
    foreach (const TranslatorMessage &mv, virginTor.messages()) {
        if (mv.sourceText().isEmpty()) {
            if (tor.contains(mv.context()))
                continue;
        } else { 
            if (tor.contains(mv.context(), mv.sourceText(), mv.comment()))
                continue;
            if (options & HeuristicSimilarText) {
                TranslatorMessage m = tor.find(mv.context(), mv.comment(), mv.allReferences());
                if (!m.isNull()) {
                    if (getSimilarityScore(m.sourceText(), mv.sourceText()) >= textSimilarityThreshold)
                        continue;
                }
            }
        }
        if (options & NoLocations)
            outTor.append(mv);
        else
            outTor.appendSorted(mv);
        if (!mv.sourceText().isEmpty())
            ++neww;
    }

    /*
      The same-text heuristic handles cases where a message has an
      obsolete counterpart with a different context or comment.
    */
    int sameTextHeuristicCount = (options & HeuristicSameText) ? applySameTextHeuristic(outTor) : 0;

    /*
      The number heuristic handles cases where a message has an
      obsolete counterpart with mostly numbers differing in the
      source text.
    */
    int sameNumberHeuristicCount = (options & HeuristicNumber) ? applyNumberHeuristic(outTor) : 0;

    if (options & Verbose) {
        int totalFound = neww + known;
        err += QObject::tr("    Found %n source text(s) (%1 new and %2 already existing)\n", 0, totalFound).arg(neww).arg(known);

        if (obsoleted) {
            if (options & NoObsolete) {
                err += QObject::tr("    Removed %n obsolete entries\n", 0, obsoleted);
            } else {
                err += QObject::tr("    Kept %n obsolete entries\n", 0, obsoleted);
            }
        }

        if (sameNumberHeuristicCount)
            err += QObject::tr("    Number heuristic provided %n translation(s)\n",
                      0, sameNumberHeuristicCount);
        if (sameTextHeuristicCount)
            err += QObject::tr("    Same-text heuristic provided %n translation(s)\n",
                      0, sameTextHeuristicCount);
        if (similarTextHeuristicCount)
            err += QObject::tr("    Similar-text heuristic provided %n translation(s)\n",
                      0, similarTextHeuristicCount);
    }
    return outTor;
}

QT_END_NAMESPACE
