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
#include <qplatformdefs.h>
#include "q3multilineedit.h"
#ifndef QT_NO_MULTILINEEDIT
#include "qpainter.h"
#include "qscrollbar.h"
#include "qcursor.h"
#include "qclipboard.h"
#include "qpixmap.h"
#include "qregexp.h"
#include "qapplication.h"
#include "q3dragobject.h"
#include "qtimer.h"
#include <private/q3richtext_p.h>

QT_BEGIN_NAMESPACE

/*!
  \class Q3MultiLineEdit qmultilineedit.h

  \brief The Q3MultiLineEdit widget is a simple editor for inputting text.

  \compat

  The Q3MultiLineEdit was a simple editor widget in former Qt versions.  Qt
  3.0 includes a new richtext engine which obsoletes Q3MultiLineEdit. It is
  still included for compatibility reasons. It is now a subclass of
  \l Q3TextEdit, and provides enough of the old Q3MultiLineEdit API to keep old
  applications working.

  If you implement something new with Q3MultiLineEdit, we suggest using
  \l Q3TextEdit instead and call Q3TextEdit::setTextFormat(Qt::PlainText).

  Although most of the old Q3MultiLineEdit API is still available, there is
  a few difference. The old Q3MultiLineEdit operated on lines, not on
  paragraphs.  As lines change all the time during wordwrap, the new
  richtext engine uses paragraphs as basic elements in the data structure.
  All functions (numLines(), textLine(), etc.) that operated on lines, now
  operate on paragraphs. Further, getString() has been removed completely.
  It revealed too much of the internal data structure.

  Applications which made normal and reasonable use of Q3MultiLineEdit
  should still work without problems. Some odd usage will require some
  porting. In these cases, it may be better to use \l Q3TextEdit now.

  \sa Q3TextEdit
*/

/*!
  \fn bool Q3MultiLineEdit::autoUpdate() const

  This function is a noop that always returns true.
*/

/*!
  \fn virtual void Q3MultiLineEdit::setAutoUpdate(bool b)

  \internal
*/

/*!
  \fn int Q3MultiLineEdit::totalWidth() const
*/

/*!
  \fn int Q3MultiLineEdit::totalHeight() const
*/

/*!
  \fn int Q3MultiLineEdit::maxLines() const
*/

/*!
  \fn void Q3MultiLineEdit::setMaxLines(int max)

  Sets the maximum number of lines this Q3MultiLineEdit will hold to
  \a max.
*/

/*!
  \fn void Q3MultiLineEdit::deselect()
*/


class Q3MultiLineEditData
{
};


/*!
  Constructs a new, empty, Q3MultiLineEdit with parent \a parent called
  \a name.
*/

Q3MultiLineEdit::Q3MultiLineEdit(QWidget *parent , const char *name)
    : Q3TextEdit(parent, name)
{
    d = new Q3MultiLineEditData;
    setTextFormat(Qt::PlainText);
}

/*! \property Q3MultiLineEdit::numLines
  \brief the number of paragraphs in the editor

  The count includes any empty paragraph at top and bottom, so for an
  empty editor this method returns 1.
*/

int Q3MultiLineEdit::numLines() const
{
    return document()->lastParagraph()->paragId() + 1;
}

/*! \property Q3MultiLineEdit::atEnd
  \brief whether the cursor is placed at the end of the text

  \sa atBeginning
*/

bool Q3MultiLineEdit::atEnd() const
{
    return textCursor()->paragraph() == document()->lastParagraph() && textCursor()->atParagEnd();
}


/*! \property Q3MultiLineEdit::atBeginning
  \brief whether the cursor is placed at the beginning of the text

  \sa atEnd
*/

bool Q3MultiLineEdit::atBeginning() const
{
    return textCursor()->paragraph() == document()->firstParagraph() && textCursor()->atParagStart();
}

/*!  Returns the number of characters at paragraph number \a row. If
  \a row is out of range, -1 is returned.
*/

int Q3MultiLineEdit::lineLength(int row) const
{
    if (row < 0 || row > numLines())
        return -1;
    return document()->paragAt(row)->length() - 1;
}


/*! Destructor. */

Q3MultiLineEdit::~Q3MultiLineEdit()
{
    delete d;
}

/*!
  If there is selected text, sets \a line1, \a col1, \a line2 and \a col2
  to the start and end of the selected region and returns true. Returns
  false if there is no selected text.
 */
bool Q3MultiLineEdit::getMarkedRegion(int *line1, int *col1,
                                      int *line2, int *col2) const
{
    int p1,c1, p2, c2;
    getSelection(&p1, &c1, &p2, &c2);
    if (p1 == -1 && c1 == -1 && p2 == -1 && c2 == -1)
        return false;
    if (line1)
        *line1 = p1;
    if (col1)
        *col1 = c1;
    if (line2)
        *line2 = p2;
    if (col2)
        *col2 = c2;
    return true;
}


/*!
  Returns true if there is selected text.
*/

bool Q3MultiLineEdit::hasMarkedText() const
{
    return hasSelectedText();
}


/*!
  Returns a copy of the selected text.
*/

QString Q3MultiLineEdit::markedText() const
{
    return selectedText();
}

/*!
  Moves the cursor one page down.  If \a mark is true, the text
  is selected.
*/

void Q3MultiLineEdit::pageDown(bool mark)
{
    moveCursor(MoveDown, mark);
}


/*!
  Moves the cursor one page up.  If \a mark is true, the text
  is selected.
*/

void Q3MultiLineEdit::pageUp(bool mark)
{
    moveCursor(MovePgUp, mark);
}


/*!  Inserts \a txt at paragraph number \a line. If \a line is less
  than zero, or larger than the number of paragraphs, the new text is
  put at the end.  If \a txt contains newline characters, several
  paragraphs are inserted.

  The cursor position is not changed.
*/

void Q3MultiLineEdit::insertLine(const QString &txt, int line)
{
    insertParagraph(txt, line);
}

/*!  Deletes the paragraph at paragraph number \a paragraph. If \a
  paragraph is less than zero or larger than the number of paragraphs,
  nothing is deleted.
*/

void Q3MultiLineEdit::removeLine(int paragraph)
{
    removeParagraph(paragraph);
}

/*!  Inserts \a str at the current cursor position and selects the
  text if \a mark is true.
*/

void Q3MultiLineEdit::insertAndMark(const QString& str, bool mark)
{
    insert(str);
    if (mark)
        document()->setSelectionEnd(Q3TextDocument::Standard, *textCursor());
}

/*!  Splits the paragraph at the current cursor position.
*/

void Q3MultiLineEdit::newLine()
{
    insert(QString(QLatin1Char('\n')));
}


/*!  Deletes the character on the left side of the text cursor and
  moves the cursor one position to the left. If a text has been selected
  by the user (e.g. by clicking and dragging) the cursor is put at the
  beginning of the selected text and the selected text is removed.  \sa
  del()
*/

void Q3MultiLineEdit::backspace()
{
    if (document()->hasSelection(Q3TextDocument::Standard)) {
        removeSelectedText();
        return;
    }

    if (!textCursor()->paragraph()->prev() &&
         textCursor()->atParagStart())
        return;

    doKeyboardAction(ActionBackspace);
}


/*!  Moves the text cursor to the left end of the line. If \a mark is
  true, text is selected toward the first position. If it is false and the
  cursor is moved, all selected text is unselected.

  \sa end()
*/

void Q3MultiLineEdit::home(bool mark)
{
    moveCursor(MoveLineStart, mark);
}

/*!  Moves the text cursor to the right end of the line. If \a mark is
  true, text is selected toward the last position.  If it is false and the
  cursor is moved, all selected text is unselected.

  \sa home()
*/

void Q3MultiLineEdit::end(bool mark)
{
    moveCursor(MoveLineEnd, mark);
}


/*!
  \fn void Q3MultiLineEdit::setCursorPosition(int line, int col)
  \reimp
*/

/*!  Sets the cursor position to character number \a col in paragraph
  number \a line.  The parameters are adjusted to lie within the legal
  range.

  If \a mark is false, the selection is cleared. otherwise it is extended.

*/

void Q3MultiLineEdit::setCursorPosition(int line, int col, bool mark)
{
    if (!mark)
        selectAll(false);
    Q3TextEdit::setCursorPosition(line, col);
    if (mark)
        document()->setSelectionEnd(Q3TextDocument::Standard, *textCursor());
}

/*!  Returns the top center point where the cursor is drawn.
*/

QPoint Q3MultiLineEdit::cursorPoint() const
{
    return QPoint(textCursor()->x(), textCursor()->y() + textCursor()->paragraph()->rect().y());
}

/*!  \property Q3MultiLineEdit::alignment
  \brief The editor's paragraph alignment

  Sets the alignment to flag, which must be Qt::AlignLeft,
  Qt::AlignHCenter, or \c Qt::AlignRight.

  If flag is an illegal flag, nothing happens.
*/
void Q3MultiLineEdit::setAlignment(Qt::Alignment flag)
{
    if (flag == Qt::AlignCenter)
        flag = Qt::AlignHCenter;
    if (flag != Qt::AlignLeft && flag != Qt::AlignRight && flag != Qt::AlignHCenter)
        return;
    Q3TextParagraph *p = document()->firstParagraph();
    while (p) {
        p->setAlignment(flag);
        p = p->next();
    }
}

Qt::Alignment Q3MultiLineEdit::alignment() const
{
    return QFlag(document()->firstParagraph()->alignment());
}


void Q3MultiLineEdit::setEdited(bool e)
{
    setModified(e);
}

/*!  \property Q3MultiLineEdit::edited
  \brief whether the document has been edited by the user

  This is the same as Q3TextEdit's "modifed" property.
*/
bool Q3MultiLineEdit::edited() const
{
    return isModified();
}

/*!  Moves the cursor one word to the right.  If \a mark is true, the text
  is selected.

  \sa cursorWordBackward()
*/
void Q3MultiLineEdit::cursorWordForward(bool mark)
{
    moveCursor(MoveWordForward, mark);
}

/*!  Moves the cursor one word to the left.  If \a mark is true, the
  text is selected.

  \sa cursorWordForward()
*/
void Q3MultiLineEdit::cursorWordBackward(bool mark)
{
    moveCursor(MoveWordBackward, mark);
}

/*!
  \fn Q3MultiLineEdit::insertAt(const QString &s, int line, int col)
  \reimp
*/

/*!  Inserts string \a s at paragraph number \a line, after character
  number \a col in the paragraph.  If \a s contains newline
  characters, new lines are inserted.
  If \a mark is true the inserted string will be selected.

  The cursor position is adjusted.
 */

void Q3MultiLineEdit::insertAt(const QString &s, int line, int col, bool mark)
{
    Q3TextEdit::insertAt(s, line, col);
    if (mark)
        setSelection(line, col, line, col + s.length());
}

// ### reggie - is this documentation correct?

/*!  Deletes text from the current cursor position to the end of the
  line. (Note that this function still operates on lines, not paragraphs.)
*/

void Q3MultiLineEdit::killLine()
{
    doKeyboardAction(ActionKill);
}

/*!  Moves the cursor one character to the left. If \a mark is true,
  the text is selected.
  The \a wrap parameter is currently ignored.

  \sa cursorRight() cursorUp() cursorDown()
*/

void Q3MultiLineEdit::cursorLeft(bool mark, bool)
{
    moveCursor(MoveBackward, mark);
}

/*!  Moves the cursor one character to the right.  If \a mark is true,
  the text is selected.
  The \a wrap parameter is currently ignored.

  \sa cursorLeft() cursorUp() cursorDown()
*/

void Q3MultiLineEdit::cursorRight(bool mark, bool)
{
    moveCursor(MoveForward, mark);
}

/*!  Moves the cursor up one line.  If \a mark is true, the text is
  selected.

  \sa cursorDown() cursorLeft() cursorRight()
*/

void Q3MultiLineEdit::cursorUp(bool mark)
{
    moveCursor(MoveUp, mark);
}

/*!
  Moves the cursor one line down.  If \a mark is true, the text
  is selected.
  \sa cursorUp() cursorLeft() cursorRight()
*/

void Q3MultiLineEdit::cursorDown(bool mark)
{
    moveCursor(MoveDown, mark);
}


/*!  Returns the text at line number \a line (possibly the empty
  string), or a null if \a line is invalid.
*/

QString Q3MultiLineEdit::textLine(int line) const
{
    if (line < 0 || line >= numLines())
        return QString();
    QString str = document()->paragAt(line)->string()->toString();
    str.truncate(str.length() - 1);
    return str;
}

QT_END_NAMESPACE

#endif
