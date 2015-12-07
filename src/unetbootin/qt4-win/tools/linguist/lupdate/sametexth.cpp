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

#include "metatranslator.h"

#include <QMap>
#include <stdio.h>

QT_BEGIN_NAMESPACE

typedef QMap<QByteArray, TranslatorMessage> TMM;
typedef QList<TranslatorMessage> TML;

/*
  Augments a MetaTranslator with trivially derived translations.

  For example, if "Enabled:" is consistendly translated as "Eingeschaltet:" no
  matter the context or the comment, "Eingeschaltet:" is added as the
  translation of any untranslated "Enabled:" text and is marked Unfinished.

  Returns the number of additional messages that this heuristic translated.
*/

int applySameTextHeuristic( MetaTranslator *tor )
{
    TMM translated;
    TMM avoid;
    TMM::Iterator t;
    TML untranslated;
    TML::Iterator u;
    TML all = tor->messages();
    TML::Iterator it;
    int inserted = 0;

    for ( it = all.begin(); it != all.end(); ++it ) {
        if ( (*it).type() == TranslatorMessage::Unfinished ) {
            if ( !(*it).isTranslated() )
                untranslated.append( *it );
        } else {
            QByteArray key = (*it).sourceText();
            t = translated.find( key );
            if ( t != translated.end() ) {
                /*
                  The same source text is translated at least two
                  different ways. Do nothing then.
                */
                if ( (*t).translations() != (*it).translations() ) {
                    translated.remove( key );
                    avoid.insert( key, *it );
                }
            } else if ( !avoid.contains(key) && (*it).isTranslated() ) {
                translated.insert( key, *it );
            }
        }
    }

    for ( u = untranslated.begin(); u != untranslated.end(); ++u ) {
        QByteArray key = (*u).sourceText();
        t = translated.find( key );
        if ( t != translated.end() ) {
            TranslatorMessage m( *u );
            m.setTranslations( (*t).translations() );
            tor->insert( m );
            inserted++;
        }
    }
    return inserted;
}

QT_END_NAMESPACE
