/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the Qt3Support module of the Qt Toolkit.
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

#include "q3syntaxhighlighter.h"
#include "q3syntaxhighlighter_p.h"

#ifndef QT_NO_SYNTAXHIGHLIGHTER
#include "q3textedit.h"
#include "qtimer.h"

QT_BEGIN_NAMESPACE

/*!
    \class Q3SyntaxHighlighter qsyntaxhighlighter.h
    \brief The Q3SyntaxHighlighter class is a base class for
    implementing Q3TextEdit syntax highlighters.

    \compat

    A syntax highligher automatically highlights parts of the text in
    a Q3TextEdit. Syntax highlighters are often used when the user is
    entering text in a specific format (for example, source code) and
    help the user to read the text and identify syntax errors.

    To provide your own syntax highlighting for Q3TextEdit, you must
    subclass Q3SyntaxHighlighter and reimplement highlightParagraph().

    When you create an instance of your Q3SyntaxHighlighter subclass,
    pass it the Q3TextEdit that you want the syntax highlighting to be
    applied to. After this your highlightParagraph() function will be
    called automatically whenever necessary. Use your
    highlightParagraph() function to apply formatting (e.g. setting
    the font and color) to the text that is passed to it.
*/

/*!
    Constructs the Q3SyntaxHighlighter and installs it on \a textEdit.
    Ownership of the Q3SyntaxHighlighter is transferred to the \a
    textEdit
*/

Q3SyntaxHighlighter::Q3SyntaxHighlighter(Q3TextEdit *textEdit)
    : para(0), edit(textEdit), d(new Q3SyntaxHighlighterPrivate)
{
    textEdit->document()->setPreProcessor(new Q3SyntaxHighlighterInternal(this));
    textEdit->document()->invalidate();
    QTimer::singleShot(0, textEdit->viewport(), SLOT(update()));
}

/*!
    Destructor. Uninstalls this syntax highlighter from the textEdit()
*/

Q3SyntaxHighlighter::~Q3SyntaxHighlighter()
{
    delete d;
    textEdit()->document()->setPreProcessor(0);
}

/*!
    \fn int Q3SyntaxHighlighter::highlightParagraph(const QString &text, int endStateOfLastPara)

    This function is called when necessary by the rich text engine,
    i.e. on paragraphs which have changed.

    In your reimplementation you should parse the paragraph's \a text
    and call setFormat() as often as necessary to apply any font and
    color changes that you require. Your function must return a value
    which indicates the paragraph's end state: see below.

    Some syntaxes can have constructs that span paragraphs. For
    example, a C++ syntax highlighter should be able to cope with
    \c{/}\c{*...*}\c{/} comments that span paragraphs. To deal
    with these cases it is necessary to know the end state of the
    previous paragraph (e.g. "in comment").

    If your syntax does not have paragraph spanning constructs, simply
    ignore the \a endStateOfLastPara parameter and always return 0.

    Whenever highlightParagraph() is called it is passed a value for
    \a endStateOfLastPara. For the very first paragraph this value is
    always -2. For any other paragraph the value is the value returned
    by the most recent highlightParagraph() call that applied to the
    preceding paragraph.

    The value you return is up to you. We recommend only returning 0
    (to signify that this paragraph's syntax highlighting does not
    affect the following paragraph), or a positive integer (to signify
    that this paragraph has ended in the middle of a paragraph
    spanning construct).

    To find out which paragraph is highlighted, call
    currentParagraph().

    For example, if you're writing a simple C++ syntax highlighter,
    you might designate 1 to signify "in comment". For a paragraph
    that ended in the middle of a comment you'd return 1, and for
    other paragraphs you'd return 0. In your parsing code if \a
    endStateOfLastPara was 1, you would highlight the text as a C++
    comment until you reached the closing \c{*}\c{/}.
*/

/*!
    This function is applied to the syntax highlighter's current
    paragraph (the text of which is passed to the highlightParagraph()
    function).

    The specified \a font and \a color are applied to the text from
    position \a start for \a count characters. (If \a count is 0,
    nothing is done.)
*/

void Q3SyntaxHighlighter::setFormat(int start, int count, const QFont &font, const QColor &color)
{
    if (!para || count <= 0)
        return;
    Q3TextFormat *f = 0;
    f = para->document()->formatCollection()->format(font, color);
    para->setFormat(start, count, f);
    f->removeRef();
}

/*! \overload */

void Q3SyntaxHighlighter::setFormat(int start, int count, const QColor &color)
{
    if (!para || count <= 0)
        return;
    Q3TextFormat *f = 0;
    QFont fnt = textEdit()->QWidget::font();
    f = para->document()->formatCollection()->format(fnt, color);
    para->setFormat(start, count, f);
    f->removeRef();
}

/*! \overload */

void Q3SyntaxHighlighter::setFormat(int start, int count, const QFont &font)
{
    if (!para || count <= 0)
        return;
    Q3TextFormat *f = 0;
    QColor c = textEdit()->viewport()->palette().color(textEdit()->viewport()->foregroundRole());
    f = para->document()->formatCollection()->format(font, c);
    para->setFormat(start, count, f);
    f->removeRef();
}

/*!
    \fn Q3TextEdit *Q3SyntaxHighlighter::textEdit() const

    Returns the Q3TextEdit on which this syntax highlighter is
    installed
*/

/*!  Redoes the highlighting of the whole document.
*/

void Q3SyntaxHighlighter::rehighlight()
{
    Q3TextParagraph *s = edit->document()->firstParagraph();
    while (s) {
        s->invalidate(0);
        s->state = -1;
        s->needPreProcess = true;
        s = s->next();
    }
    edit->repaintContents();
}

/*!
    Returns the id of the paragraph which is highlighted, or -1 of no
    paragraph is currently highlighted.

    Usually this function is called from within highlightParagraph().
*/

int Q3SyntaxHighlighter::currentParagraph() const
{
    return d->currentParagraph;
}

QT_END_NAMESPACE

#endif
