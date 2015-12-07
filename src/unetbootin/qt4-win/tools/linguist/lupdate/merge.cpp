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

#include "lupdate.h"
#include "metatranslator.h"
#include "simtexth.h"
#include <stdio.h>

#include <QDebug>

QT_BEGIN_NAMESPACE

// defined in numberh.cpp
extern int applyNumberHeuristic( MetaTranslator *tor );
// defined in sametexth.cpp
extern int applySameTextHeuristic( MetaTranslator *tor );

typedef QList<TranslatorMessage> TML;

/*
  Merges two MetaTranslator objects into the first one. The first one
  is a set of source texts and translations for a previous version of
  the internationalized program; the second one is a set of fresh
  source texts newly extracted from the source code, without any
  translation yet.
*/

void LupdateApplication::merge( const MetaTranslator *tor, const MetaTranslator *virginTor, MetaTranslator *outTor, bool verbose, bool noObsolete )
{
    int known = 0;
    int neww = 0;
    int obsoleted = 0;
    int UntranslatedObsoleted = 0;
    int similarTextHeuristicCount = 0;
    TML all = tor->messages();
    TML::Iterator it;
    outTor->setLanguageCode(tor->languageCode());

    /*
      The types of all the messages from the vernacular translator
      are updated according to the virgin translator.
    */
    for ( it = all.begin(); it != all.end(); ++it ) {
        TranslatorMessage::Type newType = TranslatorMessage::Finished;
        TranslatorMessage m = *it;

        // skip context comment
        if ( !QByteArray(m.sourceText()).isEmpty() ) {
            TranslatorMessage mv = virginTor->find(m.context(), m.sourceText(), m.comment());
            if ( mv.isNull() ) {
                mv = virginTor->find(m.context(), m.comment(), m.fileName(), m.lineNumber());
                if ( mv.isNull() ) {
                    // did not find it in the virgin, mark it as obsolete
                    newType = TranslatorMessage::Obsolete;
                    if ( m.type() != TranslatorMessage::Obsolete )
                        obsoleted++;
                } else {
                    // Do not just accept it if its on the same line number,
                    // but different source text.
                    // Also check if the texts are more or less similar before
                    // we consider them to represent the same message...
                    // ### The QString() cast is evil
                    if (getSimilarityScore(QLatin1String(m.sourceText()), mv.sourceText()) >= textSimilarityThreshold) {
                        // It is just slightly modified, assume that it is the same string
                        m = TranslatorMessage(m.context(), mv.sourceText(), m.comment(), "", m.fileName(), m.lineNumber(), m.translations(), mv.utf8());
                        m.setPlural(mv.isPlural());

                        // Mark it as unfinished. (Since the source text
                        // was changed it might require re-translating...)
                        newType = TranslatorMessage::Unfinished;
                        ++similarTextHeuristicCount;
                    } else {
                        // The virgin and vernacular sourceTexts are so
                        // different that we could not find it.
                        newType = TranslatorMessage::Obsolete;
                        if ( m.type() != TranslatorMessage::Obsolete )
                            obsoleted++;
                    }
                    neww++;
                }
            } else {
                switch ( m.type() ) {
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
                m.setFileName(mv.fileName());
                m.setLineNumber(mv.lineNumber());
                m.setPlural(mv.isPlural());
            }

            if (newType == TranslatorMessage::Obsolete && !m.isTranslated()) {
                ++UntranslatedObsoleted;
            }

            m.setType(newType);
            outTor->insert(m);
        }
    }

    /*
      Messages found only in the virgin translator are added to the
      vernacular translator. Among these are all the context comments.
    */
    all = virginTor->messages();

    for ( it = all.begin(); it != all.end(); ++it ) {
        TranslatorMessage mv = *it;
        bool found = tor->contains(mv.context(), mv.sourceText(), mv.comment());
        if (!found) {
            TranslatorMessage m = tor->find(mv.context(), mv.comment(), mv.fileName(), mv.lineNumber());
            if (!m.isNull()) {
                if (getSimilarityScore(QLatin1String(m.sourceText()), mv.sourceText()) >= textSimilarityThreshold) {
                    found = true;
                }
            } else {
                found = false;
            }
        }
        if ( !found ) {
            outTor->insert( mv );
            if ( !QByteArray(mv.sourceText()).isEmpty() )
                neww++;
        }
    }

    /*
      The same-text heuristic handles cases where a message has an
      obsolete counterpart with a different context or comment.
    */
    int sameTextHeuristicCount = applySameTextHeuristic( outTor );

    /*
      The number heuristic handles cases where a message has an
      obsolete counterpart with mostly numbers differing in the
      source text.
    */
    int sameNumberHeuristicCount = applyNumberHeuristic( outTor );

    if ( verbose ) {
        int totalFound = neww + known;
        Console::out(tr("    Found %n source text(s) (%1 new and %2 already existing)\n", 0, totalFound).arg(neww).arg(known));

        if (obsoleted) {
            if (noObsolete) {
                Console::out(tr("    Removed %n obsolete entries\n", 0, obsoleted));
            } else {
                int total = obsoleted - UntranslatedObsoleted;
                Console::out(tr("    Kept %n obsolete translation(s)\n", 0, total));
                Console::out(tr("    Removed %n obsolete untranslated entries\n", 0, UntranslatedObsoleted));
            }
        }

        if (sameNumberHeuristicCount)
            Console::out(tr("    Number heuristic provided %n translation(s)\n", 0, sameNumberHeuristicCount));
        if (sameTextHeuristicCount)
            Console::out(tr("    Same-text heuristic provided %n translation(s)\n", 0, sameTextHeuristicCount));
        if (similarTextHeuristicCount)
            Console::out(tr("    Similar-text heuristic provided %n translation(s)\n", 0, similarTextHeuristicCount));
    }
}

QT_END_NAMESPACE
