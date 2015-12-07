/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the Qt3Support module of the Qt Toolkit.
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

#ifndef Q3SYNTAXHIGHLIGHTER_P_H
#define Q3SYNTAXHIGHLIGHTER_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#ifndef QT_NO_SYNTAXHIGHLIGHTER
#include "q3syntaxhighlighter.h"
#include "private/q3richtext_p.h"

QT_BEGIN_NAMESPACE

class Q3SyntaxHighlighterPrivate
{
public:
    Q3SyntaxHighlighterPrivate() :
        currentParagraph(-1)
        {}

    int currentParagraph;
};

class Q3SyntaxHighlighterInternal : public Q3TextPreProcessor
{
public:
    Q3SyntaxHighlighterInternal(Q3SyntaxHighlighter *h) : highlighter(h) {}
    void process(Q3TextDocument *doc, Q3TextParagraph *p, int, bool invalidate) {
        if (p->prev() && p->prev()->endState() == -1)
            process(doc, p->prev(), 0, false);

        highlighter->para = p;
        QString text = p->string()->toString();
        int endState = p->prev() ? p->prev()->endState() : -2;
        int oldEndState = p->endState();
        highlighter->d->currentParagraph = p->paragId();
        p->setEndState(highlighter->highlightParagraph(text, endState));
        highlighter->d->currentParagraph = -1;
        highlighter->para = 0;

        p->setFirstPreProcess(false);
        Q3TextParagraph *op = p;
        p = p->next();
        if ((!!oldEndState || !!op->endState()) && oldEndState != op->endState() &&
             invalidate && p && !p->firstPreProcess() && p->endState() != -1) {
            while (p) {
                if (p->endState() == -1)
                    return;
                p->setEndState(-1);
                p = p->next();
            }
        }
    }
    Q3TextFormat *format(int) { return 0; }

private:
    Q3SyntaxHighlighter *highlighter;

    friend class Q3TextEdit;
};

#endif // QT_NO_SYNTAXHIGHLIGHTER

QT_END_NAMESPACE

#endif // Q3SYNTAXHIGHLIGHTER_P_H
