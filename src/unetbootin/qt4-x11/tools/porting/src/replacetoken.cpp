/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the qt3to4 porting application of the Qt Toolkit.
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
