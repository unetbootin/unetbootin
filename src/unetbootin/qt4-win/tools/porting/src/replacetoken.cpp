/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the qt3to4 porting application of the Qt Toolkit.
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

#include "replacetoken.h"
#include "tokenreplacements.h"
#include <QByteArray>

QT_BEGIN_NAMESPACE

/*
    Add an entry to the tokenRuleLookup map for each token replacement rule.
*/
ReplaceToken::ReplaceToken(const QList<TokenReplacement*> &tokenReplacementRules)
:tokenReplacementRules(tokenReplacementRules)
{
    foreach (TokenReplacement* rep, tokenReplacementRules) {
        QByteArray key = rep->getReplaceKey();
        if(!key.isEmpty()) {
            tokenRuleLookup.insert(key, rep);
        }
    }
}

TextReplacements ReplaceToken::getTokenTextReplacements(const TokenEngine::TokenContainer &container)
{
    TextReplacements textReplacements;

    int t=0;
    const int numTokens = container.count();
    while(t < numTokens) {
        QByteArray tokenText = container.text(t);
        bool changed = false;

        if(isPreprocessorDirective(tokenText)) {
            foreach(TokenReplacement *tokenReplacementRule, tokenReplacementRules) {
                if(!changed)
                    changed = tokenReplacementRule->doReplace(container, t, textReplacements);
                if(changed)
                    break;
            }
        } else if (isInterestingToken(tokenText.trimmed())) {
            foreach (TokenReplacement* value, tokenRuleLookup.values(tokenText)) {
                changed = value->doReplace(container, t, textReplacements);
                if(changed) {
                    goto end;
                }
            }
        }
    end:
        ++t;
    }
    return textReplacements;
}

bool ReplaceToken::isInterestingToken(const QByteArray &text)
{
    return !(text.isEmpty() || text==";" || text=="(" || text==")" || text=="{" || text=="}" || text=="="
            || text=="+=" || text=="-=" || text=="if" || text=="then" || text=="else"
    );
}

bool ReplaceToken::isPreprocessorDirective(const QByteArray &token)
{
    return (token[0]=='#');
}

QT_END_NAMESPACE
