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

#include "q3textedit.h"

#ifndef QT_NO_TEXTEDIT

#include <private/q3richtext_p.h>
#include "qpainter.h"
#include "qpen.h"
#include "qbrush.h"
#include "qpixmap.h"
#include "qfont.h"
#include "qcolor.h"
#include "qstyle.h"
#include "qsize.h"
#include "qevent.h"
#include "qtimer.h"
#include "qapplication.h"
#include "q3listbox.h"
#include "qclipboard.h"
#include "qcolordialog.h"
#include "q3stylesheet.h"
#include "q3dragobject.h"
#include "qurl.h"
#include "qcursor.h"
#include "qregexp.h"
#include "q3popupmenu.h"
#include "qstack.h"
#include "qmetaobject.h"
#include "q3textbrowser.h"
#include "private/q3syntaxhighlighter_p.h"
#include "qtextformat.h"
#ifndef QT_NO_IM
#include <qinputcontext.h>
#endif

#ifndef QT_NO_ACCEL
#include <qkeysequence.h>
#define ACCEL_KEY(k) QLatin1Char('\t') + QString(QKeySequence(Qt::CTRL | Qt::Key_ ## k))
#else
#define ACCEL_KEY(k) QLatin1Char('\t' )+ QString(QLatin1String("Ctrl+" #k))
#endif

#ifdef QT_TEXTEDIT_OPTIMIZATION
#define LOGOFFSET(i) d->logOffset + i
#endif

QT_BEGIN_NAMESPACE

struct QUndoRedoInfoPrivate
{
    Q3TextString text;
};

class Q3TextEditPrivate
{
public:
    Q3TextEditPrivate()
        :preeditStart(-1),preeditLength(-1),numPreeditSelections(0),ensureCursorVisibleInShowEvent(false),
         tabChangesFocus(false),
#ifndef QT_NO_CLIPBOARD
         clipboard_mode(QClipboard::Clipboard),
#endif
#ifdef QT_TEXTEDIT_OPTIMIZATION
         od(0), optimMode(false),
         maxLogLines(-1),
         logOffset(0),
#endif
         autoFormatting((uint)Q3TextEdit::AutoAll),
         cursorRepaintMode(false),
         cursorBlinkActive(false)

    {
        for (int i=0; i<7; i++)
            id[i] = 0;
    }
    int id[7];
    int preeditStart;
    int preeditLength;
    int numPreeditSelections;
    uint ensureCursorVisibleInShowEvent : 1;
    uint tabChangesFocus : 1;
    QString scrollToAnchor; // used to deferr scrollToAnchor() until the show event when we are resized
    QString pressedName;
    QString onName;
#ifndef QT_NO_CLIPBOARD
    QClipboard::Mode clipboard_mode;
#endif
    QTimer *trippleClickTimer;
    QPoint trippleClickPoint;
#ifdef QT_TEXTEDIT_OPTIMIZATION
    Q3TextEditOptimPrivate * od;
    bool optimMode : 1;
    int maxLogLines;
    int logOffset;
#endif
    Q3TextEdit::AutoFormatting autoFormatting;
    uint cursorRepaintMode : 1;
    uint cursorBlinkActive : 1;
};

#ifndef QT_NO_MIME
class Q3RichTextDrag : public Q3TextDrag
{
public:
    Q3RichTextDrag(QWidget *dragSource = 0, const char *name = 0);

    void setPlainText(const QString &txt) { setText(txt); }
    void setRichText(const QString &txt) { richTxt = txt; }

    virtual QByteArray encodedData(const char *mime) const;
    virtual const char* format(int i) const;

    static bool decode(QMimeSource *e, QString &str, const QString &mimetype,
                        const QString &subtype);
    static bool canDecode(QMimeSource* e);

private:
    QString richTxt;

};

Q3RichTextDrag::Q3RichTextDrag(QWidget *dragSource, const char *name)
    : Q3TextDrag(dragSource, name)
{
}

QByteArray Q3RichTextDrag::encodedData(const char *mime) const
{
    if (qstrcmp("application/x-qrichtext", mime) == 0) {
        return richTxt.toUtf8(); // #### perhaps we should use USC2 instead?
    } else
        return Q3TextDrag::encodedData(mime);
}

bool Q3RichTextDrag::decode(QMimeSource *e, QString &str, const QString &mimetype,
                            const QString &subtype)
{
    if (mimetype == QLatin1String("application/x-qrichtext")) {
        // do richtext decode
        const char *mime;
        int i;
        for (i = 0; (mime = e->format(i)); ++i) {
            if (qstrcmp("application/x-qrichtext", mime) != 0)
                continue;
            str = QString::fromUtf8(e->encodedData(mime));
            return true;
        }
        return false;
    }

    // do a regular text decode
    QString st = subtype;
    return Q3TextDrag::decode(e, str, st);
}

bool Q3RichTextDrag::canDecode(QMimeSource* e)
{
    if (e->provides("application/x-qrichtext"))
        return true;
    return Q3TextDrag::canDecode(e);
}

const char* Q3RichTextDrag::format(int i) const
{
    if (Q3TextDrag::format(i))
        return Q3TextDrag::format(i);
    if (Q3TextDrag::format(i-1))
        return "application/x-qrichtext";
    return 0;
}

#endif

static bool block_set_alignment = false;

/*!
    \class Q3TextEdit qtextedit.h
    \brief The Q3TextEdit widget provides a powerful single-page rich text editor.

    \compat

    \tableofcontents

    \section1 Introduction and Concepts

    Q3TextEdit is an advanced WYSIWYG viewer/editor supporting rich
    text formatting using HTML-style tags. It is optimized to handle
    large documents and to respond quickly to user input.

    Q3TextEdit has four modes of operation:
    \table
    \header \i Mode \i Command \i Notes
    \row \i Plain Text Editor \i setTextFormat(Qt::PlainText)
         \i Set text with setText(); text() returns plain text. Text
         attributes (e.g. colors) can be set, but plain text is always
         returned.
    \row \i Rich Text Editor \i setTextFormat(Qt::RichText)
         \i Set text with setText(); text() returns rich text. Rich
         text editing is fairly limited. You can't set margins or
         insert images for example (although you can read and
         correctly display files that have margins set and that
         include images). This mode is mostly useful for editing small
         amounts of rich text.
    \row \i Text Viewer \i setReadOnly(true)
         \i Set text with setText() or append() (which has no undo
         history so is faster and uses less memory); text() returns
         plain or rich text depending on the textFormat(). This mode
         can correctly display a large subset of HTML tags.
    \row \i Log Viewer \i setTextFormat(Qt::LogText)
         \i Append text using append(). The widget is set to be read
         only and rich text support is disabled although a few HTML
         tags (for color, bold, italic and underline) may be used.
         (See \link #logtextmode Qt::LogText mode\endlink for details.)
    \endtable

    Q3TextEdit can be used as a syntax highlighting editor when used in
    conjunction with QSyntaxHighlighter.

    We recommend that you always call setTextFormat() to set the mode
    you want to use. If you use Qt::AutoText then setText() and
    append() will try to determine whether the text they are given is
    plain text or rich text. If you use Qt::RichText then setText() and
    append() will assume that the text they are given is rich text.
    insert() simply inserts the text it is given.

    Q3TextEdit works on paragraphs and characters. A paragraph is a
    formatted string which is word-wrapped to fit into the width of
    the widget. By default when reading plain text, one newline
    signify a paragraph. A document consists of zero or more
    paragraphs, indexed from 0. Characters are indexed on a
    per-paragraph basis, also indexed from 0. The words in the
    paragraph are aligned in accordance with the paragraph's
    alignment(). Paragraphs are separated by hard line breaks. Each
    character within a paragraph has its own attributes, for example,
    font and color.

    The text edit documentation uses the following concepts:
    \list
    \i \e{current format} --
    this is the format at the current cursor position, \e and it
    is the format of the selected text if any.
    \i \e{current paragraph} -- the paragraph which contains the
    cursor.
    \endlist

    Q3TextEdit can display images (using Q3MimeSourceFactory), lists and
    tables. If the text is too large to view within the text edit's
    viewport, scroll bars will appear. The text edit can load both
    plain text and HTML files (a subset of HTML 3.2 and 4). The
    rendering style and the set of valid tags are defined by a
    styleSheet(). Custom tags can be created and placed in a custom
    style sheet. Change the style sheet with \l{setStyleSheet()}; see
    Q3StyleSheet for details. The images identified by image tags are
    displayed if they can be interpreted using the text edit's
    \l{Q3MimeSourceFactory}; see setMimeSourceFactory().

    If you want a text browser with more navigation use QTextBrowser.
    If you just need to display a small piece of rich text use QLabel
    or QSimpleRichText.

    If you create a new Q3TextEdit, and want to allow the user to edit
    rich text, call setTextFormat(Qt::RichText) to ensure that the
    text is treated as rich text. (Rich text uses HTML tags to set
    text formatting attributes. See Q3StyleSheet for information on the
    HTML tags that are supported.). If you don't call setTextFormat()
    explicitly the text edit will guess from the text itself whether
    it is rich text or plain text. This means that if the text looks
    like HTML or XML it will probably be interpreted as rich text, so
    you should call setTextFormat(Qt::PlainText) to preserve such
    text.

    Note that we do not intend to add a full-featured web browser
    widget to Qt (because that would easily double Qt's size and only
    a few applications would benefit from it). The rich
    text support in Qt is designed to provide a fast, portable and
    efficient way to add reasonable online help facilities to
    applications, and to provide a basis for rich text editors.

    \section1 Using Q3TextEdit as a Display Widget

    Q3TextEdit can display a large HTML subset, including tables and
    images.

    The text is set or replaced using setText() which deletes any
    existing text and replaces it with the text passed in the
    setText() call. If you call setText() with legacy HTML (with
    setTextFormat(Qt::RichText) in force), and then call text(), the text
    that is returned may have different markup, but will render the
    same. Text can be inserted with insert(), paste(), pasteSubType()
    and append(). Text that is appended does not go into the undo
    history; this makes append() faster and consumes less memory. Text
    can also be cut(). The entire text is deleted with clear() and the
    selected text is deleted with removeSelectedText(). Selected
    (marked) text can also be deleted with del() (which will delete
    the character to the right of the cursor if no text is selected).

    Loading and saving text is achieved using setText() and text(),
    for example:
    \snippet doc/src/snippets/code/src_qt3support_text_q3textedit.cpp 0

    By default the text edit wraps words at whitespace to fit within
    the text edit widget. The setWordWrap() function is used to
    specify the kind of word wrap you want, or \c NoWrap if you don't
    want any wrapping. Call setWordWrap() to set a fixed pixel width
    \c FixedPixelWidth, or character column (e.g. 80 column) \c
    FixedColumnWidth with the pixels or columns specified with
    setWrapColumnOrWidth(). If you use word wrap to the widget's width
    \c WidgetWidth, you can specify whether to break on whitespace or
    anywhere with setWrapPolicy().

    The background color is set differently than other widgets, using
    setPaper(). You specify a brush style which could be a plain color
    or a complex pixmap.

    Hypertext links are automatically underlined; this can be changed
    with setLinkUnderline(). The tab stop width is set with
    setTabStopWidth().

    The zoomIn() and zoomOut() functions can be used to resize the
    text by increasing (decreasing for zoomOut()) the point size used.
    Images are not affected by the zoom functions.

    The lines() function returns the number of lines in the text and
    paragraphs() returns the number of paragraphs. The number of lines
    within a particular paragraph is returned by linesOfParagraph().
    The length of the entire text in characters is returned by
    length().

    You can scroll to an anchor in the text, e.g.
    \c{<a name="anchor">} with scrollToAnchor(). The find() function
    can be used to find and select a given string within the text.

    A read-only Q3TextEdit provides the same functionality as the
    (obsolete) QTextView. (QTextView is still supplied for
    compatibility with old code.)

    \section2 Read-only key bindings

    When Q3TextEdit is used read-only the key-bindings are limited to
    navigation, and text may only be selected with the mouse:
    \table
    \header \i Keypresses \i Action
    \row \i Up        \i Move one line up
    \row \i Down        \i Move one line down
    \row \i Left        \i Move one character left
    \row \i Right        \i Move one character right
    \row \i PageUp        \i Move one (viewport) page up
    \row \i PageDown        \i Move one (viewport) page down
    \row \i Home        \i Move to the beginning of the text
    \row \i End                \i Move to the end of the text
    \row \i Shift+Wheel
         \i Scroll the page horizontally (the Wheel is the mouse wheel)
    \row \i Ctrl+Wheel        \i Zoom the text
    \endtable

    The text edit may be able to provide some meta-information. For
    example, the documentTitle() function will return the text from
    within HTML \c{<title>} tags.

    The text displayed in a text edit has a \e context. The context is
    a path which the text edit's Q3MimeSourceFactory uses to resolve
    the locations of files and images. It is passed to the
    mimeSourceFactory() when quering data. (See Q3TextEdit() and
    \l{context()}.)

    \target logtextmode
    \section2 Using Q3TextEdit in Qt::LogText Mode

    Setting the text format to Qt::LogText puts the widget in a special
    mode which is optimized for very large texts. In this mode editing
    and rich text support are disabled (the widget is explicitly set
    to read-only mode). This allows the text to be stored in a
    different, more memory efficient manner. However, a certain degree
    of text formatting is supported through the use of formatting
    tags. A tag is delimited by \c < and \c {>}. The characters \c
    {<}, \c > and \c & are escaped by using \c {&lt;}, \c {&gt;} and
    \c {&amp;}. A tag pair consists of a left and a right tag (or
    open/close tags). Left-tags mark the starting point for
    formatting, while right-tags mark the ending point. A right-tag
    always start with a \c / before the tag keyword. For example \c
    <b> and \c </b> are a tag pair. Tags can be nested, but they
    have to be closed in the same order as they are opened. For
    example, \c <b><u></u></b> is valid, while \c
    <b><u></b></u> will output an error message.

    By using tags it is possible to change the color, bold, italic and
    underline settings for a piece of text. A color can be specified
    by using the HTML font tag \c {<font color=colorname>}. The color
    name can be one of the color names from the X11 color database, or
    a RGB hex value (e.g \c {#00ff00}). Example of valid color tags:
    \c {<font color=red>}, \c{<font color="light blue">},\c {<font
    color="#223344">}. Bold, italic and underline settings can be
    specified by the tags \c {<b>}, \c <i> and \c {<u>}. Note that a
    tag does not necessarily have to be closed. A valid example:
    \snippet doc/src/snippets/code/src_qt3support_text_q3textedit.cpp 1

    Stylesheets can also be used in Qt::LogText mode. To create and use a
    custom tag, you could do the following:
    \snippet doc/src/snippets/code/src_qt3support_text_q3textedit.cpp 2
    Note that only the color, bold, underline and italic attributes of
    a Q3StyleSheetItem is used in Qt::LogText mode.

    Note that you can use setMaxLogLines() to limit the number of
    lines the widget can hold in Qt::LogText mode.

    There are a few things that you need to be aware of when the
    widget is in this mode:
    \list
    \i Functions that deal with rich text formatting and cursor
    movement will not work or return anything valid.
    \i Lines are equivalent to paragraphs.
    \endlist

    \section1 Using Q3TextEdit as an Editor

    All the information about using Q3TextEdit as a display widget also
    applies here.

    The current format's attributes are set with setItalic(),
    setBold(), setUnderline(), setFamily() (font family),
    setPointSize(), setColor() and setCurrentFont(). The current
    paragraph's alignment is set with setAlignment().

    Use setSelection() to select text. The setSelectionAttributes()
    function is used to indicate how selected text should be
    displayed. Use hasSelectedText() to find out if any text is
    selected. The currently selected text's position is available
    using getSelection() and the selected text itself is returned by
    selectedText(). The selection can be copied to the clipboard with
    copy(), or cut to the clipboard with cut(). It can be deleted with
    removeSelectedText(). The entire text can be selected (or
    unselected) using selectAll(). Q3TextEdit supports multiple
    selections. Most of the selection functions operate on the default
    selection, selection 0. If the user presses a non-selecting key,
    e.g. a cursor key without also holding down Shift, all selections
    are cleared.

    Set and get the position of the cursor with setCursorPosition()
    and getCursorPosition() respectively. When the cursor is moved,
    the signals currentFontChanged(), currentColorChanged() and
    currentAlignmentChanged() are emitted to reflect the font, color
    and alignment at the new cursor position.

    If the text changes, the textChanged() signal is emitted, and if
    the user inserts a new line by pressing Return or Enter,
    returnPressed() is emitted. The isModified() function will return
    true if the text has been modified.

    Q3TextEdit provides command-based undo and redo. To set the depth
    of the command history use setUndoDepth() which defaults to 100
    steps. To undo or redo the last operation call undo() or redo().
    The signals undoAvailable() and redoAvailable() indicate whether
    the undo and redo operations can be executed.

    \section2 Editing key bindings

    The list of key-bindings which are implemented for editing:
    \table
    \header \i Keypresses \i Action
    \row \i Backspace \i Delete the character to the left of the cursor
    \row \i Delete \i Delete the character to the right of the cursor
    \row \i Ctrl+A \i Move the cursor to the beginning of the line
    \row \i Ctrl+B \i Move the cursor one character left
    \row \i Ctrl+C \i Copy the marked text to the clipboard (also
                      Ctrl+Insert under Windows)
    \row \i Ctrl+D \i Delete the character to the right of the cursor
    \row \i Ctrl+E \i Move the cursor to the end of the line
    \row \i Ctrl+F \i Move the cursor one character right
    \row \i Ctrl+H \i Delete the character to the left of the cursor
    \row \i Ctrl+K \i Delete to end of line
    \row \i Ctrl+N \i Move the cursor one line down
    \row \i Ctrl+P \i Move the cursor one line up
    \row \i Ctrl+V \i Paste the clipboard text into line edit
                      (also Shift+Insert under Windows)
    \row \i Ctrl+X \i Cut the marked text, copy to clipboard
                      (also Shift+Delete under Windows)
    \row \i Ctrl+Z \i Undo the last operation
    \row \i Ctrl+Y \i Redo the last operation
    \row \i Left            \i Move the cursor one character left
    \row \i Ctrl+Left  \i Move the cursor one word left
    \row \i Right            \i Move the cursor one character right
    \row \i Ctrl+Right \i Move the cursor one word right
    \row \i Up            \i Move the cursor one line up
    \row \i Ctrl+Qt::Up    \i Move the cursor one word up
    \row \i DownArrow            \i Move the cursor one line down
    \row \i Ctrl+Down \i Move the cursor one word down
    \row \i PageUp            \i Move the cursor one page up
    \row \i PageDown            \i Move the cursor one page down
    \row \i Home            \i Move the cursor to the beginning of the line
    \row \i Ctrl+Home            \i Move the cursor to the beginning of the text
    \row \i End                    \i Move the cursor to the end of the line
    \row \i Ctrl+End            \i Move the cursor to the end of the text
    \row \i Shift+Wheel            \i Scroll the page horizontally
                            (the Wheel is the mouse wheel)
    \row \i Ctrl+Wheel            \i Zoom the text
    \endtable

    To select (mark) text hold down the Shift key whilst pressing one
    of the movement keystrokes, for example, \e{Shift+Right}
    will select the character to the right, and \e{Shift+Ctrl+Right} will select the word to the right, etc.

    By default the text edit widget operates in insert mode so all
    text that the user enters is inserted into the text edit and any
    text to the right of the cursor is moved out of the way. The mode
    can be changed to overwrite, where new text overwrites any text to
    the right of the cursor, using setOverwriteMode().
*/

/*!
    \enum Q3TextEdit::AutoFormattingFlag

    \value AutoNone Do not perform any automatic formatting
    \value AutoBulletList Only automatically format bulletted lists
    \value AutoAll Apply all available autoformatting
*/


/*!
    \enum Q3TextEdit::KeyboardAction

    This enum is used by doKeyboardAction() to specify which action
    should be executed:

    \value ActionBackspace  Delete the character to the left of the
    cursor.

    \value ActionDelete  Delete the character to the right of the
    cursor.

    \value ActionReturn  Split the paragraph at the cursor position.

    \value ActionKill If the cursor is not at the end of the
    paragraph, delete the text from the cursor position until the end
    of the paragraph. If the cursor is at the end of the paragraph,
    delete the hard line break at the end of the paragraph: this will
    cause this paragraph to be joined with the following paragraph.

    \value ActionWordBackspace Delete the word to the left of the
    cursor position.

    \value ActionWordDelete Delete the word to the right of the
    cursor position

*/

/*!
    \enum Q3TextEdit::VerticalAlignment

    This enum is used to set the vertical alignment of the text.

    \value AlignNormal Normal alignment
    \value AlignSuperScript Superscript
    \value AlignSubScript Subscript
*/

/*!
    \enum Q3TextEdit::TextInsertionFlags

    \internal

    \value RedoIndentation
    \value CheckNewLines
    \value RemoveSelected
*/


/*!
    \fn void Q3TextEdit::copyAvailable(bool yes)

    This signal is emitted when text is selected or de-selected in the
    text edit.

    When text is selected this signal will be emitted with \a yes set
    to true. If no text has been selected or if the selected text is
    de-selected this signal is emitted with \a yes set to false.

    If \a yes is true then copy() can be used to copy the selection to
    the clipboard. If \a yes is false then copy() does nothing.

    \sa selectionChanged()
*/


/*!
    \fn void Q3TextEdit::textChanged()

    This signal is emitted whenever the text in the text edit changes.

    \sa setText() append()
*/

/*!
    \fn void Q3TextEdit::selectionChanged()

    This signal is emitted whenever the selection changes.

    \sa setSelection() copyAvailable()
*/

/*!  \fn Q3TextDocument *Q3TextEdit::document() const

    \internal

  This function returns the Q3TextDocument which is used by the text
  edit.
*/

/*!  \fn void Q3TextEdit::setDocument(Q3TextDocument *doc)

    \internal

  This function sets the Q3TextDocument which should be used by the text
  edit to \a doc. This can be used, for example, if you want to
  display a document using multiple views. You would create a
  Q3TextDocument and set it to the text edits which should display it.
  You would need to connect to the textChanged() and
  selectionChanged() signals of all the text edits and update them all
  accordingly (preferably with a slight delay for efficiency reasons).
*/

/*!
    \enum Q3TextEdit::CursorAction

    This enum is used by moveCursor() to specify in which direction
    the cursor should be moved:

    \value MoveBackward  Moves the cursor one character backward

    \value MoveWordBackward Moves the cursor one word backward

    \value MoveForward  Moves the cursor one character forward

    \value MoveWordForward Moves the cursor one word forward

    \value MoveUp  Moves the cursor up one line

    \value MoveDown  Moves the cursor down one line

    \value MoveLineStart  Moves the cursor to the beginning of the line

    \value MoveLineEnd Moves the cursor to the end of the line

    \value MoveHome  Moves the cursor to the beginning of the document

    \value MoveEnd Moves the cursor to the end of the document

    \value MovePgUp  Moves the cursor one viewport page up

    \value MovePgDown  Moves the cursor one viewport page down
*/

/*!
    \property Q3TextEdit::overwriteMode
    \brief the text edit's overwrite mode

    If false (the default) characters entered by the user are inserted
    with any characters to the right being moved out of the way. If
    true, the editor is in overwrite mode, i.e. characters entered by
    the user overwrite any characters to the right of the cursor
    position.
*/

/*!
    \fn void Q3TextEdit::setCurrentFont(const QFont &f)

    Sets the font of the current format to \a f.

    If the widget is in Qt::LogText mode this function will do
    nothing. Use setFont() instead.

    \sa currentFont() setPointSize() setFamily()
*/

/*!
    \property Q3TextEdit::undoDepth
    \brief the depth of the undo history

    The maximum number of steps in the undo/redo history. The default
    is 100.

    \sa undo() redo()
*/

/*!
    \fn void Q3TextEdit::undoAvailable(bool yes)

    This signal is emitted when the availability of undo changes. If
    \a yes is true, then undo() will work until undoAvailable(false)
    is next emitted.

    \sa undo() undoDepth()
*/

/*!
    \fn void Q3TextEdit::modificationChanged(bool m)

    This signal is emitted when the modification status of the
    document has changed. If \a m is true, the document was modified,
    otherwise the modification state has been reset to unmodified.

    \sa modified
*/

/*!
    \fn void Q3TextEdit::redoAvailable(bool yes)

    This signal is emitted when the availability of redo changes. If
    \a yes is true, then redo() will work until redoAvailable(false)
    is next emitted.

    \sa redo() undoDepth()
*/

/*!
    \fn void Q3TextEdit::currentFontChanged(const QFont &f)

    This signal is emitted if the font of the current format has
    changed.

    The new font is \a f.

    \sa setCurrentFont()
*/

/*!
    \fn void Q3TextEdit::currentColorChanged(const QColor &c)

    This signal is emitted if the color of the current format has
    changed.

    The new color is \a c.

    \sa setColor()
*/

/*!
    \fn void Q3TextEdit::currentVerticalAlignmentChanged(Q3TextEdit::VerticalAlignment a)

    This signal is emitted if the vertical alignment of the current
    format has changed.

    The new vertical alignment is \a a.
*/

/*!
    \fn void Q3TextEdit::currentAlignmentChanged(int a)

    This signal is emitted if the alignment of the current paragraph
    has changed.

    The new alignment is \a a.

    \sa setAlignment()
*/

/*!
    \fn void Q3TextEdit::cursorPositionChanged(Q3TextCursor *c)

    \internal
*/

/*!
    \fn void Q3TextEdit::cursorPositionChanged(int para, int pos)

    \overload

    This signal is emitted if the position of the cursor has changed.
    \a para contains the paragraph index and \a pos contains the
    character position within the paragraph.

    \sa setCursorPosition()
*/

/*!
    \fn void Q3TextEdit::clicked(int para, int pos)

    This signal is emitted when the mouse is clicked on the paragraph
    \a para at character position \a pos.

    \sa doubleClicked()
*/

/*! \fn void Q3TextEdit::doubleClicked(int para, int pos)

  This signal is emitted when the mouse is double-clicked on the
  paragraph \a para at character position \a pos.

  \sa clicked()
*/


/*!
    \fn void Q3TextEdit::returnPressed()

    This signal is emitted if the user pressed the Return or the Enter
    key.
*/

/*!
    \fn Q3TextCursor *Q3TextEdit::textCursor() const

    Returns the text edit's text cursor.

    \warning Q3TextCursor is not in the public API, but in special
    circumstances you might wish to use it.
*/

/*!
    Constructs an empty Q3TextEdit called \a name, with parent \a
    parent.
*/

Q3TextEdit::Q3TextEdit(QWidget *parent, const char *name)
    : Q3ScrollView(parent, name, Qt::WStaticContents | Qt::WNoAutoErase),
      doc(new Q3TextDocument(0)), undoRedoInfo(doc)
{
    init();
}

/*!
    Constructs a Q3TextEdit called \a name, with parent \a parent. The
    text edit will display the text \a text using context \a context.

    The \a context is a path which the text edit's Q3MimeSourceFactory
    uses to resolve the locations of files and images. It is passed to
    the mimeSourceFactory() when quering data.

    For example if the text contains an image tag,
    \c{<img src="image.png">}, and the context is "path/to/look/in", the
    Q3MimeSourceFactory will try to load the image from
    "path/to/look/in/image.png". If the tag was
    \c{<img src="/image.png">}, the context will not be used (because
    Q3MimeSourceFactory recognizes that we have used an absolute path)
    and will try to load "/image.png". The context is applied in exactly
    the same way to \e hrefs, for example,
    \c{<a href="target.html">Target</a>}, would resolve to
    "path/to/look/in/target.html".
*/

Q3TextEdit::Q3TextEdit(const QString& text, const QString& context,
                      QWidget *parent, const char *name)
    : Q3ScrollView(parent, name, Qt::WStaticContents | Qt::WNoAutoErase),
      doc(new Q3TextDocument(0)), undoRedoInfo(doc)
{
    init();
    setText(text, context);
}

/*!
    Destructor.
*/

Q3TextEdit::~Q3TextEdit()
{
    delete undoRedoInfo.d;
    undoRedoInfo.d = 0;
    delete cursor;
    delete doc;
#ifdef QT_TEXTEDIT_OPTIMIZATION
    delete d->od;
#endif
    delete d;
}

void Q3TextEdit::init()
{
    d = new Q3TextEditPrivate;
    doc->formatCollection()->setPaintDevice(this);
    undoEnabled = true;
    readonly = true;
    setReadOnly(false);
    setFrameStyle(LineEditPanel | Sunken);
    connect(doc, SIGNAL(minimumWidthChanged(int)),
             this, SLOT(documentWidthChanged(int)));

    mousePressed = false;
    inDoubleClick = false;
    modified = false;
    mightStartDrag = false;
    onLink.clear();
    d->onName.clear();
    overWrite = false;
    wrapMode = WidgetWidth;
    wrapWidth = -1;
    wPolicy = AtWhiteSpace;
    inDnD = false;
    doc->setFormatter(new Q3TextFormatterBreakWords);
    QFont f = Q3ScrollView::font();
    if (f.kerning())
        f.setKerning(false);
    doc->formatCollection()->defaultFormat()->setFont(f);
    doc->formatCollection()->defaultFormat()->setColor(palette().color(QPalette::Text));
    currentFormat = doc->formatCollection()->defaultFormat();
    currentAlignment = Qt::AlignAuto;

    setBackgroundRole(QPalette::Base);
    viewport()->setBackgroundRole(QPalette::Base);

    viewport()->setAcceptDrops(true);
    resizeContents(0, doc->lastParagraph() ?
                    (doc->lastParagraph()->paragId() + 1) * doc->formatCollection()->defaultFormat()->height() : 0);

    setAttribute(Qt::WA_KeyCompression, true);
    viewport()->setMouseTracking(true);
#ifndef QT_NO_CURSOR
    viewport()->setCursor(isReadOnly() ? Qt::ArrowCursor : Qt::IBeamCursor);
#endif
    cursor = new Q3TextCursor(doc);

    formatTimer = new QTimer(this);
    connect(formatTimer, SIGNAL(timeout()),
             this, SLOT(formatMore()));
    lastFormatted = doc->firstParagraph();

    scrollTimer = new QTimer(this);
    connect(scrollTimer, SIGNAL(timeout()),
             this, SLOT(autoScrollTimerDone()));

    interval = 0;
    changeIntervalTimer = new QTimer(this);
    connect(changeIntervalTimer, SIGNAL(timeout()),
             this, SLOT(doChangeInterval()));

    cursorVisible = true;
    blinkTimer = new QTimer(this);
    connect(blinkTimer, SIGNAL(timeout()),
             this, SLOT(blinkCursor()));

#ifndef QT_NO_DRAGANDDROP
    dragStartTimer = new QTimer(this);
    connect(dragStartTimer, SIGNAL(timeout()),
             this, SLOT(startDrag()));
#endif

    d->trippleClickTimer = new QTimer(this);

    formatMore();

    blinkCursorVisible = false;

    viewport()->setFocusProxy(this);
    viewport()->setFocusPolicy(Qt::WheelFocus);
    setFocusPolicy(Qt::WheelFocus);
    setInputMethodEnabled(true);
    viewport()->installEventFilter(this);
    connect(this, SIGNAL(horizontalSliderReleased()), this, SLOT(sliderReleased()));
    connect(this, SIGNAL(verticalSliderReleased()), this, SLOT(sliderReleased()));
    installEventFilter(this);
}

void Q3TextEdit::paintDocument(bool drawAll, QPainter *p, int cx, int cy, int cw, int ch)
{
#ifdef QT_TEXTEDIT_OPTIMIZATION
    Q_ASSERT(!d->optimMode);
    if (d->optimMode)
        return;
#endif

    bool drawCur = blinkCursorVisible && (hasFocus() || viewport()->hasFocus());
    if ((hasSelectedText() && !style()->styleHint(QStyle::SH_BlinkCursorWhenTextSelected, 0, this)) ||
        isReadOnly() || !cursorVisible)
        drawCur = false;
    QPalette pal = palette();
    if (doc->paper())
        pal.setBrush(QPalette::Base, *doc->paper());

    if (contentsY() < doc->y()) {
        p->fillRect(contentsX(), contentsY(), visibleWidth(), doc->y(),
                     pal.base());
    }
    if (drawAll && doc->width() - contentsX() < cx + cw) {
        p->fillRect(doc->width() - contentsX(), cy, cx + cw - doc->width() + contentsX(), ch,
                     pal.base());
    }

    p->setBrushOrigin(-contentsX(), -contentsY());

    lastFormatted = doc->draw(p, cx, cy, cw, ch, pal, !drawAll, drawCur, cursor);

    if (lastFormatted == doc->lastParagraph())
        resizeContents(contentsWidth(), doc->height());

    if (contentsHeight() < visibleHeight() && (!doc->lastParagraph() || doc->lastParagraph()->isValid()) && drawAll)
        p->fillRect(0, contentsHeight(), visibleWidth(),
                     visibleHeight() - contentsHeight(), pal.base());
}

/*!
    \reimp
*/

void Q3TextEdit::drawContents(QPainter *p, int cx, int cy, int cw, int ch)
{
#ifdef QT_TEXTEDIT_OPTIMIZATION
    if (d->optimMode) {
        optimDrawContents(p, cx, cy, cw, ch);
        return;
    }
#endif
    paintDocument(true, p, cx, cy, cw, ch);
    int v;
    p->setPen(palette().color(foregroundRole()));
    if (document()->isPageBreakEnabled() &&  (v = document()->flow()->pageSize()) > 0) {
        int l = int(cy / v) * v;
        while (l < cy + ch) {
            p->drawLine(cx, l, cx + cw - 1, l);
            l += v;
        }
    }
}

/*!
    \internal
*/

void Q3TextEdit::drawContents(QPainter *p)
{
    if (horizontalScrollBar()->isVisible() &&
         verticalScrollBar()->isVisible()) {
        const QRect verticalRect = verticalScrollBar()->geometry();
        const QRect horizontalRect = horizontalScrollBar()->geometry();

        QRect cornerRect;
        cornerRect.setTop(verticalRect.bottom());
        cornerRect.setBottom(horizontalRect.bottom());
        cornerRect.setLeft(verticalRect.left());
        cornerRect.setRight(verticalRect.right());

        p->fillRect(cornerRect, palette().background());
    }
}

/*!
    \reimp
*/

bool Q3TextEdit::event(QEvent *e)
{
    if (e->type() == QEvent::AccelOverride && !isReadOnly()) {
        QKeyEvent* ke = (QKeyEvent*) e;
        switch(ke->state()) {
        case Qt::NoButton:
        case Qt::Keypad:
        case Qt::ShiftButton:
            if (ke->key() < Qt::Key_Escape) {
                ke->accept();
            } else {
                switch (ke->key()) {
                case Qt::Key_Return:
                case Qt::Key_Enter:
                case Qt::Key_Delete:
                case Qt::Key_Home:
                case Qt::Key_End:
                case Qt::Key_Backspace:
                case Qt::Key_Left:
                case Qt::Key_Right:
                    ke->accept();
                default:
                    break;
                }
            }
            break;

        case Qt::ControlButton:
        case Qt::ControlButton|Qt::ShiftButton:
        case Qt::ControlButton|Qt::Keypad:
        case Qt::ControlButton|Qt::ShiftButton|Qt::Keypad:
            switch (ke->key()) {
            case Qt::Key_Tab:
            case Qt::Key_Backtab:
                ke->ignore();
                break;
// Those are too frequently used for application functionality
/*            case Qt::Key_A:
            case Qt::Key_B:
            case Qt::Key_D:
            case Qt::Key_E:
            case Qt::Key_F:
            case Qt::Key_H:
            case Qt::Key_I:
            case Qt::Key_K:
            case Qt::Key_N:
            case Qt::Key_P:
            case Qt::Key_T:
*/
            case Qt::Key_C:
            case Qt::Key_V:
            case Qt::Key_X:
            case Qt::Key_Y:
            case Qt::Key_Z:
            case Qt::Key_Left:
            case Qt::Key_Right:
            case Qt::Key_Up:
            case Qt::Key_Down:
            case Qt::Key_Home:
            case Qt::Key_End:
#if defined (Q_WS_WIN)
            case Qt::Key_Insert:
            case Qt::Key_Delete:
#endif
                ke->accept();
            default:
                break;
            }
            break;

        default:
            switch (ke->key()) {
#if defined (Q_WS_WIN)
            case Qt::Key_Insert:
                ke->accept();
#endif
            default:
                break;
            }
            break;
        }
    }

    if (e->type() == QEvent::Show) {
        if (
#ifdef QT_TEXTEDIT_OPTIMIZATION
             !d->optimMode &&
#endif
             d->ensureCursorVisibleInShowEvent ) {
            ensureCursorVisible();
            d->ensureCursorVisibleInShowEvent = false;
        }
        if (!d->scrollToAnchor.isEmpty()) {
            scrollToAnchor(d->scrollToAnchor);
            d->scrollToAnchor.clear();
        }
    }
    return QWidget::event(e);
}

/*!
    Processes the key event, \a e. By default key events are used to
    provide keyboard navigation and text editing.
*/

void Q3TextEdit::keyPressEvent(QKeyEvent *e)
{
    changeIntervalTimer->stop();
    interval = 10;
    bool unknownKey = false;
    if (isReadOnly()) {
        if (!handleReadOnlyKeyEvent(e))
            Q3ScrollView::keyPressEvent(e);
        changeIntervalTimer->start(100, true);
        return;
    }


    bool selChanged = false;
    for (int i = 1; i < doc->numSelections(); ++i) // start with 1 as we don't want to remove the Standard-Selection
        selChanged = doc->removeSelection(i) || selChanged;

    if (selChanged) {
        cursor->paragraph()->document()->nextDoubleBuffered = true;
        repaintChanged();
    }

    bool clearUndoRedoInfo = true;


    switch (e->key()) {
    case Qt::Key_Left:
    case Qt::Key_Right: {
        // a bit hacky, but can't change this without introducing new enum values for move and keeping the
        // correct semantics and movement for BiDi and non BiDi text.
        CursorAction a;
        if (cursor->paragraph()->string()->isRightToLeft() == (e->key() == Qt::Key_Right))
            a = e->state() & Qt::ControlButton ? MoveWordBackward : MoveBackward;
        else
            a = e->state() & Qt::ControlButton ? MoveWordForward : MoveForward;
        moveCursor(a, e->state() & Qt::ShiftButton);
        break;
    }
    case Qt::Key_Up:
        moveCursor(e->state() & Qt::ControlButton ? MovePgUp : MoveUp, e->state() & Qt::ShiftButton);
        break;
    case Qt::Key_Down:
        moveCursor(e->state() & Qt::ControlButton ? MovePgDown : MoveDown, e->state() & Qt::ShiftButton);
        break;
    case Qt::Key_Home:
        moveCursor(e->state() & Qt::ControlButton ? MoveHome : MoveLineStart, e->state() & Qt::ShiftButton);
        break;
    case Qt::Key_End:
        moveCursor(e->state() & Qt::ControlButton ? MoveEnd : MoveLineEnd, e->state() & Qt::ShiftButton);
        break;
    case Qt::Key_Prior:
        moveCursor(MovePgUp, e->state() & Qt::ShiftButton);
        break;
    case Qt::Key_Next:
        moveCursor(MovePgDown, e->state() & Qt::ShiftButton);
        break;
    case Qt::Key_Return: case Qt::Key_Enter:
        if (doc->hasSelection(Q3TextDocument::Standard, false))
            removeSelectedText();
        if (textFormat() == Qt::RichText && (e->state() & Qt::ControlButton)) {
            // Ctrl-Enter inserts a line break in rich text mode
            insert(QString(QChar(QChar::LineSeparator)), true, false);
        } else {
#ifndef QT_NO_CURSOR
            viewport()->setCursor(isReadOnly() ? Qt::ArrowCursor : Qt::IBeamCursor);
#endif
            clearUndoRedoInfo = false;
            doKeyboardAction(ActionReturn);
            emit returnPressed();
        }
        break;
    case Qt::Key_Delete:
#if defined (Q_WS_WIN)
        if (e->state() & Qt::ShiftButton) {
            cut();
            break;
        } else
#endif
        if (doc->hasSelection(Q3TextDocument::Standard, true)) {
            removeSelectedText();
            break;
        }
        doKeyboardAction(e->state() & Qt::ControlButton ? ActionWordDelete
                          : ActionDelete);
        clearUndoRedoInfo = false;

        break;
    case Qt::Key_Insert:
        if (e->state() & Qt::ShiftButton)
            paste();
#if defined (Q_WS_WIN)
        else if (e->state() & Qt::ControlButton)
            copy();
#endif
        else
            setOverwriteMode(!isOverwriteMode());
        break;
    case Qt::Key_Backspace:
#if defined (Q_WS_WIN)
        if (e->state() & Qt::AltButton) {
            if (e->state() & Qt::ControlButton) {
                break;
            } else if (e->state() & Qt::ShiftButton) {
                redo();
                break;
            } else {
                undo();
                break;
            }
        } else
#endif
        if (doc->hasSelection(Q3TextDocument::Standard, true)) {
            removeSelectedText();
            break;
        }

        doKeyboardAction(e->state() & Qt::ControlButton ? ActionWordBackspace
                          : ActionBackspace);
        clearUndoRedoInfo = false;
        break;
    case Qt::Key_F16: // Copy key on Sun keyboards
        copy();
        break;
    case Qt::Key_F18:  // Paste key on Sun keyboards
        paste();
        break;
    case Qt::Key_F20:  // Cut key on Sun keyboards
        cut();
        break;
    case Qt::Key_Direction_L:
        if (doc->textFormat() == Qt::PlainText) {
            // change the whole doc
            Q3TextParagraph *p = doc->firstParagraph();
            while (p) {
                p->setDirection(QChar::DirL);
                p->setAlignment(Qt::AlignLeft);
                p->invalidate(0);
                p = p->next();
            }
        } else {
            if (!cursor->paragraph() || cursor->paragraph()->direction() == QChar::DirL)
                return;
            cursor->paragraph()->setDirection(QChar::DirL);
            if (cursor->paragraph()->length() <= 1&&
                 ((cursor->paragraph()->alignment() & (Qt::AlignLeft | Qt::AlignRight)) != 0))
                setAlignment(Qt::AlignLeft);
        }
        repaintChanged();
        break;
    case Qt::Key_Direction_R:
        if (doc->textFormat() == Qt::PlainText) {
            // change the whole doc
            Q3TextParagraph *p = doc->firstParagraph();
            while (p) {
                p->setDirection(QChar::DirR);
                p->setAlignment(Qt::AlignRight);
                p->invalidate(0);
                p = p->next();
            }
        } else {
            if (!cursor->paragraph() || cursor->paragraph()->direction() == QChar::DirR)
                return;
            cursor->paragraph()->setDirection(QChar::DirR);
            if (cursor->paragraph()->length() <= 1&&
                 ((cursor->paragraph()->alignment() & (Qt::AlignLeft | Qt::AlignRight)) != 0))
                setAlignment(Qt::AlignRight);
        }
        repaintChanged();
        break;
    default: {
            unsigned char ascii = e->text().length() ? e->text().unicode()->latin1() : 0;
            if (e->text().length() &&
                ((!(e->state() & Qt::ControlButton) &&
#ifndef Q_OS_MAC
                  !(e->state() & Qt::AltButton) &&
#endif
                  !(e->state() & Qt::MetaButton)) ||
                 (((e->state() & (Qt::ControlButton | Qt::AltButton))) == (Qt::ControlButton|Qt::AltButton))) &&
                 (!ascii || ascii >= 32 || e->text() == QString(QLatin1Char('\t')))) {
                clearUndoRedoInfo = false;
                if (e->key() == Qt::Key_Tab) {
                    if (d->tabChangesFocus) {
                        e->ignore();
                        break;
                    }
                    if (textFormat() == Qt::RichText && cursor->index() == 0
                         && (cursor->paragraph()->isListItem() || cursor->paragraph()->listDepth())) {
                        clearUndoRedo();
                        undoRedoInfo.type = UndoRedoInfo::Style;
                        undoRedoInfo.id = cursor->paragraph()->paragId();
                        undoRedoInfo.eid = undoRedoInfo.id;
                        undoRedoInfo.styleInformation = Q3TextStyleCommand::readStyleInformation(doc, undoRedoInfo.id, undoRedoInfo.eid);
                        cursor->paragraph()->setListDepth(cursor->paragraph()->listDepth() +1);
                        clearUndoRedo();
                        drawCursor(false);
                        repaintChanged();
                        drawCursor(true);
                        break;
                    }
                } else if (e->key() == Qt::Key_BackTab) {
                    if (d->tabChangesFocus) {
                        e->ignore();
                        break;
                    }
                }

                if ((autoFormatting() & AutoBulletList) &&
                     textFormat() == Qt::RichText && cursor->index() == 0
                     && !cursor->paragraph()->isListItem()
                     && (e->text()[0] == QLatin1Char('-') || e->text()[0] == QLatin1Char('*'))) {
                        clearUndoRedo();
                        undoRedoInfo.type = UndoRedoInfo::Style;
                        undoRedoInfo.id = cursor->paragraph()->paragId();
                        undoRedoInfo.eid = undoRedoInfo.id;
                        undoRedoInfo.styleInformation = Q3TextStyleCommand::readStyleInformation(doc, undoRedoInfo.id, undoRedoInfo.eid);
                        setParagType(Q3StyleSheetItem::DisplayListItem, Q3StyleSheetItem::ListDisc);
                        clearUndoRedo();
                        drawCursor(false);
                        repaintChanged();
                        drawCursor(true);
                        break;
                }
		if (overWrite && !cursor->atParagEnd() && !doc->hasSelection(Q3TextDocument::Standard)) {
                    doKeyboardAction(ActionDelete);
                    clearUndoRedoInfo = false;
                }
                QString t = e->text();
                insert(t, true, false);
                break;
            } else if (e->state() & Qt::ControlButton) {
                switch (e->key()) {
                case Qt::Key_C: case Qt::Key_F16: // Copy key on Sun keyboards
                    copy();
                    break;
                case Qt::Key_V:
                    paste();
                    break;
                case Qt::Key_X:
                    cut();
                    break;
                case Qt::Key_I: case Qt::Key_T: case Qt::Key_Tab:
                    if (!d->tabChangesFocus)
                        indent();
                    break;
                case Qt::Key_A:
#if defined(Q_WS_X11)
                    moveCursor(MoveLineStart, e->state() & Qt::ShiftButton);
#else
                    selectAll(true);
#endif
                    break;
                case Qt::Key_B:
                    moveCursor(MoveBackward, e->state() & Qt::ShiftButton);
                    break;
                case Qt::Key_F:
                    moveCursor(MoveForward, e->state() & Qt::ShiftButton);
                    break;
                case Qt::Key_D:
                    if (doc->hasSelection(Q3TextDocument::Standard)) {
                        removeSelectedText();
                        break;
                    }
                    doKeyboardAction(ActionDelete);
                    clearUndoRedoInfo = false;
                    break;
                case Qt::Key_H:
                    if (doc->hasSelection(Q3TextDocument::Standard)) {
                        removeSelectedText();
                        break;
                    }
                    if (!cursor->paragraph()->prev() &&
                         cursor->atParagStart())
                        break;

                    doKeyboardAction(ActionBackspace);
                    clearUndoRedoInfo = false;
                    break;
                case Qt::Key_E:
                    moveCursor(MoveLineEnd, e->state() & Qt::ShiftButton);
                    break;
                case Qt::Key_N:
                    moveCursor(MoveDown, e->state() & Qt::ShiftButton);
                    break;
                case Qt::Key_P:
                    moveCursor(MoveUp, e->state() & Qt::ShiftButton);
                    break;
                case Qt::Key_Z:
                    if(e->state() & Qt::ShiftButton)
                        redo();
                    else
                        undo();
                    break;
                case Qt::Key_Y:
                    redo();
                    break;
                case Qt::Key_K:
                    doKeyboardAction(ActionKill);
                    break;
#if defined(Q_WS_WIN)
                case Qt::Key_Insert:
                    copy();
                    break;
                case Qt::Key_Delete:
                    del();
                    break;
#endif
                default:
                    unknownKey = false;
                    break;
                }
            } else {
                unknownKey = true;
            }
        }
    }

    emit cursorPositionChanged(cursor);
    emit cursorPositionChanged(cursor->paragraph()->paragId(), cursor->index());
    if (clearUndoRedoInfo)
        clearUndoRedo();
    changeIntervalTimer->start(100, true);
    if (unknownKey)
        e->ignore();
}

/*!
    \reimp
*/
void Q3TextEdit::inputMethodEvent(QInputMethodEvent *e)
{
    if (isReadOnly()) {
        e->ignore();
        return;
    }

    if (hasSelectedText())
        removeSelectedText();
    clearUndoRedo();
    undoRedoInfo.type = UndoRedoInfo::IME;

    bool oldupdate = updatesEnabled();
    if (oldupdate)
        setUpdatesEnabled(false);
    bool sigs_blocked = signalsBlocked();
    blockSignals(true);
    const int preeditSelectionBase = 31900;
    for (int i = 0; i < d->numPreeditSelections; ++i)
        doc->removeSelection(preeditSelectionBase + i);
    d->numPreeditSelections = 0;

    if (d->preeditLength > 0 && cursor->paragraph()) {
        cursor->setIndex(d->preeditStart);
        cursor->paragraph()->remove(d->preeditStart, d->preeditLength);
        d->preeditStart = d->preeditLength = -1;
    }

    if (!e->commitString().isEmpty() || e->replacementLength()) {
        int c = cursor->index(); // cursor position after insertion of commit string
        if (e->replacementStart() <= 0)
            c += e->commitString().length() + qMin(-e->replacementStart(), e->replacementLength());
        cursor->setIndex(cursor->index() + e->replacementStart());
        doc->setSelectionStart(Q3TextDocument::Standard, *cursor);
        cursor->setIndex(cursor->index() + e->replacementLength());
        doc->setSelectionEnd(Q3TextDocument::Standard, *cursor);
        removeSelectedText();
	if (undoRedoInfo.type == UndoRedoInfo::IME)
	    undoRedoInfo.type = UndoRedoInfo::Invalid;
        insert(e->commitString());
	undoRedoInfo.type = UndoRedoInfo::IME;
        cursor->setIndex(c);
    }

    if (!e->preeditString().isEmpty()) {
        d->preeditStart = cursor->index();
        d->preeditLength = e->preeditString().length();
	insert(e->preeditString());
        cursor->setIndex(d->preeditStart);

        Q3TextCursor c = *cursor;
        for (int i = 0; i < e->attributes().size(); ++i) {
            const QInputMethodEvent::Attribute &a = e->attributes().at(i);
            if (a.type == QInputMethodEvent::Cursor)
                cursor->setIndex(cursor->index() + a.start);
            else if (a.type != QInputMethodEvent::TextFormat)
                continue;
            QTextCharFormat f = qvariant_cast<QTextFormat>(a.value).toCharFormat();
            if (f.isValid()) {
                Q3TextCursor c2 = c;
                c2.setIndex(c.index() + a.start);
                doc->setSelectionStart(preeditSelectionBase + d->numPreeditSelections, c2);
                c2.setIndex(c.index() + a.start + a.length);
                doc->setSelectionEnd(preeditSelectionBase + d->numPreeditSelections, c2);

                QColor c = f.hasProperty(QTextFormat::BackgroundBrush) ? f.background().color() : QColor();
                doc->setSelectionColor(preeditSelectionBase + d->numPreeditSelections, c);
                c = f.hasProperty(QTextFormat::ForegroundBrush) ? f.foreground().color() : QColor();
                doc->setSelectionTextColor(preeditSelectionBase + d->numPreeditSelections, c);
                if (f.fontUnderline()) {
                    Q3TextParagraph *par = cursor->paragraph();
                    Q3TextFormat f(*par->string()->at(d->preeditStart).format());
                    f.setUnderline(true);
                    Q3TextFormat *f2 = doc->formatCollection()->format(&f);
                    par->setFormat(d->preeditStart + a.start, a.length, f2);
                }
                ++d->numPreeditSelections;
            }
	}
    } else {
	undoRedoInfo.type = UndoRedoInfo::Invalid;
    }
    blockSignals(sigs_blocked);
    if (oldupdate)
        setUpdatesEnabled(true);
    if (!e->commitString().isEmpty())
        emit textChanged();
    repaintChanged();
}


static bool qtextedit_ignore_readonly = false;

/*!
    Executes keyboard action \a action. This is normally called by a
    key event handler.
*/

void Q3TextEdit::doKeyboardAction(Q3TextEdit::KeyboardAction action)
{
    if (isReadOnly() && !qtextedit_ignore_readonly)
        return;

    if (cursor->nestedDepth() != 0)
        return;

    lastFormatted = cursor->paragraph();
    drawCursor(false);
    bool doUpdateCurrentFormat = true;

    switch (action) {
    case ActionWordDelete:
    case ActionDelete:
        if (action == ActionDelete && !cursor->atParagEnd()) {
            if (undoEnabled) {
                checkUndoRedoInfo(UndoRedoInfo::Delete);
                if (!undoRedoInfo.valid()) {
                    undoRedoInfo.id = cursor->paragraph()->paragId();
                    undoRedoInfo.index = cursor->index();
                    undoRedoInfo.d->text.clear();
                }
                int idx = cursor->index();
                do {
                    undoRedoInfo.d->text.insert(undoRedoInfo.d->text.length(), cursor->paragraph()->at(idx++), true);
                } while (!cursor->paragraph()->string()->validCursorPosition(idx));
            }
            cursor->remove();
        } else {
            clearUndoRedo();
            doc->setSelectionStart(Q3TextDocument::Temp, *cursor);
            if (action == ActionWordDelete && !cursor->atParagEnd()) {
                cursor->gotoNextWord();
            } else {
                cursor->gotoNextLetter();
            }
            doc->setSelectionEnd(Q3TextDocument::Temp, *cursor);
            removeSelectedText(Q3TextDocument::Temp);
        }
        break;
    case ActionWordBackspace:
    case ActionBackspace:
        if (textFormat() == Qt::RichText
             && (cursor->paragraph()->isListItem()
                 || cursor->paragraph()->listDepth())
             && cursor->index() == 0) {
            if (undoEnabled) {
                clearUndoRedo();
                undoRedoInfo.type = UndoRedoInfo::Style;
                undoRedoInfo.id = cursor->paragraph()->paragId();
                undoRedoInfo.eid = undoRedoInfo.id;
                undoRedoInfo.styleInformation = Q3TextStyleCommand::readStyleInformation(doc, undoRedoInfo.id, undoRedoInfo.eid);
            }
            int ldepth = cursor->paragraph()->listDepth();
            if (cursor->paragraph()->isListItem() && ldepth == 1) {
                cursor->paragraph()->setListItem(false);
            } else if (qMax(ldepth, 1) == 1) {
                cursor->paragraph()->setListItem(false);
                cursor->paragraph()->setListDepth(0);
            } else {
                cursor->paragraph()->setListDepth(ldepth - 1);
            }
            clearUndoRedo();
            lastFormatted = cursor->paragraph();
            repaintChanged();
            drawCursor(true);
            return;
        }

        if (action == ActionBackspace && !cursor->atParagStart()) {
            if (undoEnabled) {
                checkUndoRedoInfo(UndoRedoInfo::Delete);
                if (!undoRedoInfo.valid()) {
                    undoRedoInfo.id = cursor->paragraph()->paragId();
                    undoRedoInfo.index = cursor->index();
                    undoRedoInfo.d->text.clear();
                }
                undoRedoInfo.d->text.insert(0, cursor->paragraph()->at(cursor->index()-1), true);
                undoRedoInfo.index = cursor->index()-1;
            }
            cursor->removePreviousChar();
            lastFormatted = cursor->paragraph();
        } else if (cursor->paragraph()->prev()
                    || (action == ActionWordBackspace
                        && !cursor->atParagStart())) {
            clearUndoRedo();
            doc->setSelectionStart(Q3TextDocument::Temp, *cursor);
            if (action == ActionWordBackspace && !cursor->atParagStart()) {
                cursor->gotoPreviousWord();
            } else {
                cursor->gotoPreviousLetter();
            }
            doc->setSelectionEnd(Q3TextDocument::Temp, *cursor);
            removeSelectedText(Q3TextDocument::Temp);
        }
        break;
    case ActionReturn:
        if (undoEnabled) {
            checkUndoRedoInfo(UndoRedoInfo::Return);
            if (!undoRedoInfo.valid()) {
                undoRedoInfo.id = cursor->paragraph()->paragId();
                undoRedoInfo.index = cursor->index();
                undoRedoInfo.d->text.clear();
            }
            undoRedoInfo.d->text += QString(QLatin1Char('\n'));
        }
        cursor->splitAndInsertEmptyParagraph();
        if (cursor->paragraph()->prev()) {
            lastFormatted = cursor->paragraph()->prev();
            lastFormatted->invalidate(0);
        }
        doUpdateCurrentFormat = false;
        break;
    case ActionKill:
        clearUndoRedo();
        doc->setSelectionStart(Q3TextDocument::Temp, *cursor);
        if (cursor->atParagEnd())
            cursor->gotoNextLetter();
        else
            cursor->setIndex(cursor->paragraph()->length() - 1);
        doc->setSelectionEnd(Q3TextDocument::Temp, *cursor);
        removeSelectedText(Q3TextDocument::Temp);
        break;
    }

    formatMore();
    repaintChanged();
    ensureCursorVisible();
    drawCursor(true);
    if (doUpdateCurrentFormat)
        updateCurrentFormat();
    setModified();
    emit textChanged();
}

void Q3TextEdit::readFormats(Q3TextCursor &c1, Q3TextCursor &c2, Q3TextString &text, bool fillStyles)
{
#ifndef QT_NO_DATASTREAM
    QDataStream styleStream(&undoRedoInfo.styleInformation, IO_WriteOnly);
#endif
    c2.restoreState();
    c1.restoreState();
    int lastIndex = text.length();
    if (c1.paragraph() == c2.paragraph()) {
        for (int i = c1.index(); i < c2.index(); ++i)
            text.insert(lastIndex + i - c1.index(), c1.paragraph()->at(i), true);
#ifndef QT_NO_DATASTREAM
        if (fillStyles) {
            styleStream << (int) 1;
            c1.paragraph()->writeStyleInformation(styleStream);
        }
#endif
    } else {
        int i;
        for (i = c1.index(); i < c1.paragraph()->length()-1; ++i)
            text.insert(lastIndex++, c1.paragraph()->at(i), true);
        int num = 2; // start and end, being different
        text += QString(QLatin1Char('\n')); lastIndex++;

        if (c1.paragraph()->next() != c2.paragraph()) {
            num += text.appendParagraphs(c1.paragraph()->next(), c2.paragraph());
            lastIndex = text.length();
        }

        for (i = 0; i < c2.index(); ++i)
            text.insert(i + lastIndex, c2.paragraph()->at(i), true);
#ifndef QT_NO_DATASTREAM
        if (fillStyles) {
            styleStream << num;
            for (Q3TextParagraph *p = c1.paragraph(); --num >= 0; p = p->next())
                p->writeStyleInformation(styleStream);
        }
#endif
    }
}

/*!
    Removes the selection \a selNum (by default 0). This does not
    remove the selected text.

    \sa removeSelectedText()
*/

void Q3TextEdit::removeSelection(int selNum)
{
    doc->removeSelection(selNum);
    repaintChanged();
}

/*!
    Deletes the text of selection \a selNum (by default, the default
    selection, 0). If there is no selected text nothing happens.

    \sa selectedText removeSelection()
*/

void Q3TextEdit::removeSelectedText(int selNum)
{
    Q3TextCursor c1 = doc->selectionStartCursor(selNum);
    c1.restoreState();
    Q3TextCursor c2 = doc->selectionEndCursor(selNum);
    c2.restoreState();

    // ### no support for editing tables yet, plus security for broken selections
    if (c1.nestedDepth() || c2.nestedDepth())
        return;

    for (int i = 0; i < (int)doc->numSelections(); ++i) {
        if (i == selNum)
            continue;
        doc->removeSelection(i);
    }

    drawCursor(false);
    if (undoEnabled) {
        checkUndoRedoInfo(UndoRedoInfo::RemoveSelected);
        if (!undoRedoInfo.valid()) {
            doc->selectionStart(selNum, undoRedoInfo.id, undoRedoInfo.index);
            undoRedoInfo.d->text.clear();
        }
        readFormats(c1, c2, undoRedoInfo.d->text, true);
    }

    doc->removeSelectedText(selNum, cursor);
    if (cursor->isValid()) {
        lastFormatted = 0; // make sync a noop
        ensureCursorVisible();
        lastFormatted = cursor->paragraph();
        formatMore();
        repaintContents();
        ensureCursorVisible();
        drawCursor(true);
        clearUndoRedo();
#if defined(Q_WS_WIN)
        // there seems to be a problem with repainting or erasing the area
        // of the scrollview which is not the contents on windows
        if (contentsHeight() < visibleHeight())
            viewport()->repaint(0, contentsHeight(), visibleWidth(), visibleHeight() - contentsHeight());
#endif
#ifndef QT_NO_CURSOR
        viewport()->setCursor(isReadOnly() ? Qt::ArrowCursor : Qt::IBeamCursor);
#endif
    } else {
        lastFormatted = doc->firstParagraph();
        delete cursor;
        cursor = new Q3TextCursor(doc);
        drawCursor(true);
        repaintContents();
    }
    setModified();
    emit textChanged();
    emit selectionChanged();
    emit copyAvailable(doc->hasSelection(Q3TextDocument::Standard));
}

/*!
    Moves the text cursor according to \a action. This is normally
    used by some key event handler. \a select specifies whether the
    text between the current cursor position and the new position
    should be selected.
*/

void Q3TextEdit::moveCursor(Q3TextEdit::CursorAction action, bool select)
{
#ifdef QT_TEXTEDIT_OPTIMIZATION
    if (d->optimMode)
        return;
#endif
#ifdef Q_WS_MAC
    Q3TextCursor c1 = *cursor;
    Q3TextCursor c2;
#endif
    drawCursor(false);
    if (select) {
        if (!doc->hasSelection(Q3TextDocument::Standard))
            doc->setSelectionStart(Q3TextDocument::Standard, *cursor);
        moveCursor(action);
#ifdef Q_WS_MAC
        c2 = *cursor;
        if (c1 == c2)
            if (action == MoveDown || action == MovePgDown)
                moveCursor(MoveEnd);
            else if (action == MoveUp || action == MovePgUp)
                moveCursor(MoveHome);
#endif
        if (doc->setSelectionEnd(Q3TextDocument::Standard, *cursor)) {
            cursor->paragraph()->document()->nextDoubleBuffered = true;
            repaintChanged();
        } else {
            drawCursor(true);
        }
        ensureCursorVisible();
        emit selectionChanged();
        emit copyAvailable(doc->hasSelection(Q3TextDocument::Standard));
    } else {
#ifdef Q_WS_MAC
        Q3TextCursor cStart = doc->selectionStartCursor(Q3TextDocument::Standard);
        Q3TextCursor cEnd = doc->selectionEndCursor(Q3TextDocument::Standard);
        bool redraw = doc->removeSelection(Q3TextDocument::Standard);
        if (redraw && action == MoveDown)
            *cursor = cEnd;
        else if (redraw && action == MoveUp)
            *cursor = cStart;
        if (redraw && action == MoveForward)
            *cursor = cEnd;
        else if (redraw && action == MoveBackward)
            *cursor = cStart;
        else
            moveCursor(action);
        c2 = *cursor;
        if (c1 == c2)
            if (action == MoveDown)
                moveCursor(MoveEnd);
            else if (action == MoveUp)
                moveCursor(MoveHome);
#else
        bool redraw = doc->removeSelection(Q3TextDocument::Standard);
        moveCursor(action);
#endif
        if (!redraw) {
            ensureCursorVisible();
            drawCursor(true);
        } else {
            cursor->paragraph()->document()->nextDoubleBuffered = true;
            repaintChanged();
            ensureCursorVisible();
            drawCursor(true);
#ifndef QT_NO_CURSOR
            viewport()->setCursor(isReadOnly() ? Qt::ArrowCursor : Qt::IBeamCursor);
#endif
        }
        if (redraw) {
            emit copyAvailable(doc->hasSelection(Q3TextDocument::Standard));
            emit selectionChanged();
        }
    }

    drawCursor(true);
    updateCurrentFormat();
}

/*!
    \overload
*/

void Q3TextEdit::moveCursor(Q3TextEdit::CursorAction action)
{
    resetInputContext();
    switch (action) {
    case MoveBackward:
        cursor->gotoPreviousLetter();
        break;
    case MoveWordBackward:
        cursor->gotoPreviousWord();
        break;
    case MoveForward:
        cursor->gotoNextLetter();
        break;
    case MoveWordForward:
        cursor->gotoNextWord();
        break;
    case MoveUp:
        cursor->gotoUp();
        break;
    case MovePgUp:
        cursor->gotoPageUp(visibleHeight());
        break;
    case MoveDown:
        cursor->gotoDown();
        break;
    case MovePgDown:
        cursor->gotoPageDown(visibleHeight());
        break;
    case MoveLineStart:
        cursor->gotoLineStart();
        break;
    case MoveHome:
        cursor->gotoHome();
        break;
    case MoveLineEnd:
        cursor->gotoLineEnd();
        break;
    case MoveEnd:
        ensureFormatted(doc->lastParagraph());
        cursor->gotoEnd();
        break;
    }
    updateCurrentFormat();
}

/*!
    \reimp
*/

void Q3TextEdit::resizeEvent(QResizeEvent *e)
{
    Q3ScrollView::resizeEvent(e);
    if (doc->visibleWidth() == 0)
        doResize();
}

/*!
    \reimp
*/

void Q3TextEdit::viewportResizeEvent(QResizeEvent *e)
{
    Q3ScrollView::viewportResizeEvent(e);
    if (e->oldSize().width() != e->size().width()) {
        bool stayAtBottom = e->oldSize().height() != e->size().height() &&
               contentsY() > 0 && contentsY() >= doc->height() - e->oldSize().height();
        doResize();
        if (stayAtBottom)
            scrollToBottom();
    }
}

/*!
    Ensures that the cursor is visible by scrolling the text edit if
    necessary.

    \sa setCursorPosition()
*/

void Q3TextEdit::ensureCursorVisible()
{
    // Not visible or the user is dragging the window, so don't position to caret yet
    if (!updatesEnabled() || !isVisible() || isHorizontalSliderPressed() || isVerticalSliderPressed()) {
        d->ensureCursorVisibleInShowEvent = true;
        return;
    }
    sync();
    Q3TextStringChar *chr = cursor->paragraph()->at(cursor->index());
    int h = cursor->paragraph()->lineHeightOfChar(cursor->index());
    int x = cursor->paragraph()->rect().x() + chr->x + cursor->offsetX();
    int y = 0; int dummy;
    cursor->paragraph()->lineHeightOfChar(cursor->index(), &dummy, &y);
    y += cursor->paragraph()->rect().y() + cursor->offsetY();
    int w = 1;
    ensureVisible(x, y + h / 2, w, h / 2 + 2);
}

/*!
    \internal
*/
void Q3TextEdit::sliderReleased()
{
    if (d->ensureCursorVisibleInShowEvent && isVisible()) {
        d->ensureCursorVisibleInShowEvent = false;
        ensureCursorVisible();
    }
}

/*!
    \internal

    If \a visible is true, the cursor is shown; otherwise it is
    hidden.
*/
void Q3TextEdit::drawCursor(bool visible)
{
    d->cursorRepaintMode = true;
    blinkCursorVisible = visible;
    QRect r(cursor->topParagraph()->rect());
    if (!cursor->nestedDepth()) {
        int h = cursor->paragraph()->lineHeightOfChar(cursor->index());
        r = QRect(r.x(), r.y() + cursor->y(), r.width(), h);
    }
    r.moveBy(-contentsX(), -contentsY());
    viewport()->update(r);
}

enum {
    IdUndo = 0,
    IdRedo = 1,
    IdCut = 2,
    IdCopy = 3,
    IdPaste = 4,
    IdClear = 5,
    IdSelectAll = 6
};

/*!
    \reimp
*/
#ifndef QT_NO_WHEELEVENT
void Q3TextEdit::contentsWheelEvent(QWheelEvent *e)
{
    if (isReadOnly()) {
        if (e->state() & Qt::ControlButton) {
            if (e->delta() > 0)
                zoomOut();
            else if (e->delta() < 0)
                zoomIn();
            return;
        }
    }
    Q3ScrollView::contentsWheelEvent(e);
}
#endif

/*!
    \reimp
*/

void Q3TextEdit::contentsMousePressEvent(QMouseEvent *e)
{
#ifdef QT_TEXTEDIT_OPTIMIZATION
    if (d->optimMode) {
        optimMousePressEvent(e);
        return;
    }
#endif

#if !defined(QT_NO_IM)
    if (e->button() == Qt::LeftButton && d->preeditLength > 0 && cursor->paragraph()) {
        Q3TextCursor c = *cursor;
        placeCursor(e->pos(), &c, false);
        inputContext()->mouseHandler(c.index() - d->preeditStart, e);
        if (d->preeditLength > 0)
            return;
    }
#endif
        
    if (d->trippleClickTimer->isActive() &&
         (e->globalPos() - d->trippleClickPoint).manhattanLength() <
         QApplication::startDragDistance()) {
        Q3TextCursor c1 = *cursor;
        Q3TextCursor c2 = *cursor;
        c1.gotoLineStart();
        c2.gotoLineEnd();
        doc->setSelectionStart(Q3TextDocument::Standard, c1);
        doc->setSelectionEnd(Q3TextDocument::Standard, c2);
        *cursor = c2;
        repaintChanged();
        mousePressed = true;
        return;
    }

    clearUndoRedo();
    Q3TextCursor oldCursor = *cursor;
    Q3TextCursor c = *cursor;
    mousePos = e->pos();
    mightStartDrag = false;
    pressedLink.clear();
    d->pressedName.clear();

    if (e->button() == Qt::LeftButton) {
        mousePressed = true;
        drawCursor(false);
        placeCursor(e->pos());
        ensureCursorVisible();

        if (isReadOnly() && linksEnabled()) {
            Q3TextCursor c = *cursor;
            placeCursor(e->pos(), &c, true);
            if (c.paragraph() && c.paragraph()->at(c.index()) &&
                 c.paragraph()->at(c.index())->isAnchor()) {
                pressedLink = c.paragraph()->at(c.index())->anchorHref();
                d->pressedName = c.paragraph()->at(c.index())->anchorName();
            }
        }

#ifndef QT_NO_DRAGANDDROP
        if (doc->inSelection(Q3TextDocument::Standard, e->pos())) {
            mightStartDrag = true;
            drawCursor(true);
            dragStartTimer->start(QApplication::startDragTime(), true);
            dragStartPos = e->pos();
            return;
        }
#endif

        bool redraw = false;
        if (doc->hasSelection(Q3TextDocument::Standard)) {
            if (!(e->state() & Qt::ShiftButton)) {
                redraw = doc->removeSelection(Q3TextDocument::Standard);
                doc->setSelectionStart(Q3TextDocument::Standard, *cursor);
            } else {
                redraw = doc->setSelectionEnd(Q3TextDocument::Standard, *cursor) || redraw;
            }
        } else {
            if (isReadOnly() || !(e->state() & Qt::ShiftButton)) {
                doc->setSelectionStart(Q3TextDocument::Standard, *cursor);
            } else {
                doc->setSelectionStart(Q3TextDocument::Standard, c);
                redraw = doc->setSelectionEnd(Q3TextDocument::Standard, *cursor) || redraw;
            }
        }

        for (int i = 1; i < doc->numSelections(); ++i) // start with 1 as we don't want to remove the Standard-Selection
            redraw = doc->removeSelection(i) || redraw;

        if (!redraw) {
            drawCursor(true);
        } else {
            repaintChanged();
#ifndef QT_NO_CURSOR
            viewport()->setCursor(isReadOnly() ? Qt::ArrowCursor : Qt::IBeamCursor);
#endif
        }
    } else if (e->button() == Qt::MidButton) {
        bool redraw = doc->removeSelection(Q3TextDocument::Standard);
        if (!redraw) {
            drawCursor(true);
        } else {
            repaintChanged();
#ifndef QT_NO_CURSOR
            viewport()->setCursor(isReadOnly() ? Qt::ArrowCursor : Qt::IBeamCursor);
#endif
        }
    }

    if (*cursor != oldCursor)
        updateCurrentFormat();
}

/*!
    \reimp
*/

void Q3TextEdit::contentsMouseMoveEvent(QMouseEvent *e)
{
#ifdef QT_TEXTEDIT_OPTIMIZATION
    if (d->optimMode) {
        optimMouseMoveEvent(e);
        return;
    }
#endif
    
#if !defined(QT_NO_IM)
    if (d->preeditLength > 0) 
        return;
#endif
    
    if (mousePressed) {
#ifndef QT_NO_DRAGANDDROP
        if (mightStartDrag) {
            dragStartTimer->stop();
            if ((e->pos() - dragStartPos).manhattanLength() > QApplication::startDragDistance())
                startDrag();
#ifndef QT_NO_CURSOR
            if (!isReadOnly())
                viewport()->setCursor(Qt::IBeamCursor);
#endif
            return;
        }
#endif
        mousePos = e->pos();
        handleMouseMove(mousePos);
        oldMousePos = mousePos;
    }

#ifndef QT_NO_CURSOR
    if (!isReadOnly() && !mousePressed) {
        if (doc->hasSelection(Q3TextDocument::Standard) && doc->inSelection(Q3TextDocument::Standard, e->pos()))
            viewport()->setCursor(Qt::ArrowCursor);
        else
            viewport()->setCursor(Qt::IBeamCursor);
    }
#endif
    updateCursor(e->pos());
}

void Q3TextEdit::copyToClipboard()
{
#ifndef QT_NO_CLIPBOARD
    if (QApplication::clipboard()->supportsSelection()) {
        d->clipboard_mode = QClipboard::Selection;

        // don't listen to selection changes
        disconnect(QApplication::clipboard(), SIGNAL(selectionChanged()), this, 0);
        copy();
        // listen to selection changes
        connect(QApplication::clipboard(), SIGNAL(selectionChanged()),
                 this, SLOT(clipboardChanged()));

        d->clipboard_mode = QClipboard::Clipboard;
    }
#endif
}

/*!
    \reimp
*/

void Q3TextEdit::contentsMouseReleaseEvent(QMouseEvent * e)
{
    if (!inDoubleClick) { // could be the release of a dblclick
        int para = 0;
        int index = charAt(e->pos(), &para);
        emit clicked(para, index);
    }
#ifdef QT_TEXTEDIT_OPTIMIZATION
    if (d->optimMode) {
        optimMouseReleaseEvent(e);
        return;
    }
#endif
    Q3TextCursor oldCursor = *cursor;
    if (scrollTimer->isActive())
        scrollTimer->stop();
#ifndef QT_NO_DRAGANDDROP
    if (dragStartTimer->isActive())
        dragStartTimer->stop();
    if (mightStartDrag) {
        selectAll(false);
        mousePressed = false;
    }
#endif
    if (mousePressed) {
        mousePressed = false;
        copyToClipboard();
    }
#ifndef QT_NO_CLIPBOARD
    else if (e->button() == Qt::MidButton && !isReadOnly()) {
        // only do middle-click pasting on systems that have selections (ie. X11)
        if (QApplication::clipboard()->supportsSelection()) {
            drawCursor(false);
            placeCursor(e->pos());
            ensureCursorVisible();
            doc->setSelectionStart(Q3TextDocument::Standard, oldCursor);
            bool redraw = false;
            if (doc->hasSelection(Q3TextDocument::Standard)) {
                redraw = doc->removeSelection(Q3TextDocument::Standard);
                doc->setSelectionStart(Q3TextDocument::Standard, *cursor);
            } else {
                doc->setSelectionStart(Q3TextDocument::Standard, *cursor);
            }
            // start with 1 as we don't want to remove the Standard-Selection
            for (int i = 1; i < doc->numSelections(); ++i)
                redraw = doc->removeSelection(i) || redraw;
            if (!redraw) {
                drawCursor(true);
            } else {
                repaintChanged();
#ifndef QT_NO_CURSOR
                viewport()->setCursor(Qt::IBeamCursor);
#endif
            }
            d->clipboard_mode = QClipboard::Selection;
            paste();
            d->clipboard_mode = QClipboard::Clipboard;
        }
    }
#endif
    emit cursorPositionChanged(cursor);
    emit cursorPositionChanged(cursor->paragraph()->paragId(), cursor->index());
    if (oldCursor != *cursor)
        updateCurrentFormat();
    inDoubleClick = false;

#ifndef QT_NO_NETWORKPROTOCOL
    if ((  (!onLink.isEmpty() && onLink == pressedLink)
          || (!d->onName.isEmpty() && d->onName == d->pressedName))
         && linksEnabled()) {
        if (!onLink.isEmpty()) {
            QUrl u = QUrl(doc->context()).resolved(onLink);
            emitLinkClicked(u.toString(QUrl::None));
        }
        if (Q3TextBrowser *browser = qobject_cast<Q3TextBrowser*>(this))
            emit browser->anchorClicked(d->onName, onLink);

        // emitting linkClicked() may result in that the cursor winds
        // up hovering over a different valid link - check this and
        // set the appropriate cursor shape
        updateCursor(e->pos());
    }
#endif
    drawCursor(true);
    if (!doc->hasSelection(Q3TextDocument::Standard, true))
        doc->removeSelection(Q3TextDocument::Standard);

    emit copyAvailable(doc->hasSelection(Q3TextDocument::Standard));
    emit selectionChanged();
}

/*!
    \reimp
*/

void Q3TextEdit::contentsMouseDoubleClickEvent(QMouseEvent * e)
{
    if (e->button() != Qt::LeftButton) {
        e->ignore();
        return;
    }
#if !defined(QT_NO_IM)
    if (d->preeditLength > 0) 
        return;
#endif
    
    int para = 0;
    int index = charAt(e->pos(), &para);
#ifdef QT_TEXTEDIT_OPTIMIZATION
    if (d->optimMode) {
        QString str = d->od->lines[LOGOFFSET(para)];
        int startIdx = index, endIdx = index, i;
        if (!str[index].isSpace()) {
            i = startIdx;
            // find start of word
            while (i >= 0 && !str[i].isSpace()) {
                startIdx = i--;
            }
            i = endIdx;
            // find end of word..
            while (i < str.length() && !str[i].isSpace()) {
                endIdx = ++i;
            }
            // ..and start of next
            while (i < str.length() && str[i].isSpace()) {
                endIdx = ++i;
            }
            optimSetSelection(para, startIdx, para, endIdx);
            repaintContents();
        }
    } else
#endif
    {
        Q3TextCursor c1 = *cursor;
        Q3TextCursor c2 = *cursor;
#if defined(Q_OS_MAC)
        Q3TextParagraph *para = cursor->paragraph();
        if (cursor->isValid()) {
            if (para->at(cursor->index())->c.isLetterOrNumber()) {
                while (c1.index() > 0 &&
                        c1.paragraph()->at(c1.index()-1)->c.isLetterOrNumber())
                    c1.gotoPreviousLetter();
                while (c2.paragraph()->at(c2.index())->c.isLetterOrNumber() &&
                        !c2.atParagEnd())
                    c2.gotoNextLetter();
            } else if (para->at(cursor->index())->c.isSpace()) {
                while (c1.index() > 0 &&
                        c1.paragraph()->at(c1.index()-1)->c.isSpace())
                    c1.gotoPreviousLetter();
                while (c2.paragraph()->at(c2.index())->c.isSpace() &&
                        !c2.atParagEnd())
                    c2.gotoNextLetter();
            } else if (!c2.atParagEnd()) {
                c2.gotoNextLetter();
            }
        }
#else
        if (cursor->index() > 0 && !cursor->paragraph()->at(cursor->index()-1)->c.isSpace())
            c1.gotoPreviousWord();
        if (!cursor->paragraph()->at(cursor->index())->c.isSpace() && !cursor->atParagEnd())
            c2.gotoNextWord();
#endif
        doc->setSelectionStart(Q3TextDocument::Standard, c1);
        doc->setSelectionEnd(Q3TextDocument::Standard, c2);

        *cursor = c2;

        repaintChanged();

        d->trippleClickTimer->start(qApp->doubleClickInterval(), true);
        d->trippleClickPoint = e->globalPos();
    }
    inDoubleClick = true;
    mousePressed = true;
    emit doubleClicked(para, index);
}

#ifndef QT_NO_DRAGANDDROP

/*!
    \reimp
*/

void Q3TextEdit::contentsDragEnterEvent(QDragEnterEvent *e)
{
    if (isReadOnly() || !Q3TextDrag::canDecode(e)) {
        e->ignore();
        return;
    }
    e->acceptAction();
    inDnD = true;
}

/*!
    \reimp
*/

void Q3TextEdit::contentsDragMoveEvent(QDragMoveEvent *e)
{
    if (isReadOnly() || !Q3TextDrag::canDecode(e)) {
        e->ignore();
        return;
    }
    drawCursor(false);
    placeCursor(e->pos(),  cursor);
    drawCursor(true);
    e->acceptAction();
}

/*!
    \reimp
*/

void Q3TextEdit::contentsDragLeaveEvent(QDragLeaveEvent *)
{
    drawCursor(false);
    inDnD = false;
}

/*!
    \reimp
*/

void Q3TextEdit::contentsDropEvent(QDropEvent *e)
{
    if (isReadOnly())
        return;
    inDnD = false;
    e->acceptAction();
    bool intern = false;
    if (Q3RichTextDrag::canDecode(e)) {
        bool hasSel = doc->hasSelection(Q3TextDocument::Standard);
        bool internalDrag = e->source() == this || e->source() == viewport();
        int dropId, dropIndex;
        Q3TextCursor insertCursor = *cursor;
        dropId = cursor->paragraph()->paragId();
        dropIndex = cursor->index();
        if (hasSel && internalDrag) {
            Q3TextCursor c1, c2;
            int selStartId, selStartIndex;
            int selEndId, selEndIndex;
            c1 = doc->selectionStartCursor(Q3TextDocument::Standard);
            c1.restoreState();
            c2 = doc->selectionEndCursor(Q3TextDocument::Standard);
            c2.restoreState();
            selStartId = c1.paragraph()->paragId();
            selStartIndex = c1.index();
            selEndId = c2.paragraph()->paragId();
            selEndIndex = c2.index();
            if (((dropId > selStartId) ||
                   (dropId == selStartId && dropIndex > selStartIndex)) &&
                 ((dropId < selEndId) ||
                   (dropId == selEndId && dropIndex <= selEndIndex)))
                insertCursor = c1;
            if (dropId == selEndId && dropIndex > selEndIndex) {
                insertCursor = c1;
                if (selStartId == selEndId) {
                    insertCursor.setIndex(dropIndex -
                                           (selEndIndex - selStartIndex));
                } else {
                    insertCursor.setIndex(dropIndex - selEndIndex +
                                           selStartIndex);
                }
            }
         }

        if (internalDrag && e->action() == QDropEvent::Move) {
            removeSelectedText();
            intern = true;
            doc->removeSelection(Q3TextDocument::Standard);
        } else {
            doc->removeSelection(Q3TextDocument::Standard);
#ifndef QT_NO_CURSOR
            viewport()->setCursor(isReadOnly() ? Qt::ArrowCursor : Qt::IBeamCursor);
#endif
        }
        drawCursor(false);
        cursor->setParagraph(insertCursor.paragraph());
        cursor->setIndex(insertCursor.index());
        drawCursor(true);
        if (!cursor->nestedDepth()) {
            QString subType = QLatin1String("plain");
            if (textFormat() != Qt::PlainText) {
                if (e->provides("application/x-qrichtext"))
                    subType = QLatin1String("x-qrichtext");
            }
#ifndef QT_NO_CLIPBOARD
            pasteSubType(subType.toLatin1(), e);
#endif
            // emit appropriate signals.
            emit selectionChanged();
            emit cursorPositionChanged(cursor);
            emit cursorPositionChanged(cursor->paragraph()->paragId(), cursor->index());
        } else {
            if (intern)
                undo();
            e->ignore();
        }
    }
}

#endif

/*!
    \reimp
*/
void Q3TextEdit::contentsContextMenuEvent(QContextMenuEvent *e)
{
    clearUndoRedo();
    mousePressed = false;

    e->accept();
#ifndef QT_NO_POPUPMENU
    Q3PopupMenu *popup = createPopupMenu(e->pos());
    if (!popup)
        popup = createPopupMenu();
    if (!popup)
        return;
    int r = popup->exec(e->globalPos(), -1);
    delete popup;

    if (r == d->id[IdClear])
        clear();
    else if (r == d->id[IdSelectAll]) {
        selectAll();
#ifndef QT_NO_CLIPBOARD
        // if the clipboard support selections, put the newly selected text into
        // the clipboard
        if (QApplication::clipboard()->supportsSelection()) {
            d->clipboard_mode = QClipboard::Selection;

            // don't listen to selection changes
            disconnect(QApplication::clipboard(), SIGNAL(selectionChanged()), this, 0);
            copy();
            // listen to selection changes
            connect(QApplication::clipboard(), SIGNAL(selectionChanged()),
                     this, SLOT(clipboardChanged()));

            d->clipboard_mode = QClipboard::Clipboard;
        }
#endif
    } else if (r == d->id[IdUndo])
        undo();
    else if (r == d->id[IdRedo])
        redo();
#ifndef QT_NO_CLIPBOARD
    else if (r == d->id[IdCut])
        cut();
    else if (r == d->id[IdCopy])
        copy();
    else if (r == d->id[IdPaste])
        paste();
#endif
#endif
}


void Q3TextEdit::autoScrollTimerDone()
{
    if (mousePressed)
        handleMouseMove( viewportToContents(viewport()->mapFromGlobal(QCursor::pos()) ));
}

void Q3TextEdit::handleMouseMove(const QPoint& pos)
{
    if (!mousePressed)
        return;

    if ((!scrollTimer->isActive() && pos.y() < contentsY()) || pos.y() > contentsY() + visibleHeight())
        scrollTimer->start(100, false);
    else if (scrollTimer->isActive() && pos.y() >= contentsY() && pos.y() <= contentsY() + visibleHeight())
        scrollTimer->stop();

    drawCursor(false);
    Q3TextCursor oldCursor = *cursor;

    placeCursor(pos);

    if (inDoubleClick) {
        Q3TextCursor cl = *cursor;
        cl.gotoPreviousWord();
        Q3TextCursor cr = *cursor;
        cr.gotoNextWord();

        int diff = QABS(oldCursor.paragraph()->at(oldCursor.index())->x - mousePos.x());
        int ldiff = QABS(cl.paragraph()->at(cl.index())->x - mousePos.x());
        int rdiff = QABS(cr.paragraph()->at(cr.index())->x - mousePos.x());


        if (cursor->paragraph()->lineStartOfChar(cursor->index()) !=
             oldCursor.paragraph()->lineStartOfChar(oldCursor.index()))
            diff = 0xFFFFFF;

        if (rdiff < diff && rdiff < ldiff)
            *cursor = cr;
        else if (ldiff < diff && ldiff < rdiff)
            *cursor = cl;
        else
            *cursor = oldCursor;

    }
    ensureCursorVisible();

    bool redraw = false;
    if (doc->hasSelection(Q3TextDocument::Standard)) {
        redraw = doc->setSelectionEnd(Q3TextDocument::Standard, *cursor) || redraw;
    }

    if (!redraw) {
        drawCursor(true);
    } else {
        repaintChanged();
        drawCursor(true);
    }

    if (currentFormat && currentFormat->key() != cursor->paragraph()->at(cursor->index())->format()->key()) {
        currentFormat->removeRef();
        currentFormat = doc->formatCollection()->format(cursor->paragraph()->at(cursor->index())->format());
        if (currentFormat->isMisspelled()) {
            currentFormat->removeRef();
            currentFormat = doc->formatCollection()->format(currentFormat->font(), currentFormat->color());
        }
        emit currentFontChanged(currentFormat->font());
        emit currentColorChanged(currentFormat->color());
        emit currentVerticalAlignmentChanged((VerticalAlignment)currentFormat->vAlign());
    }

    if (currentAlignment != cursor->paragraph()->alignment()) {
        currentAlignment = cursor->paragraph()->alignment();
        block_set_alignment = true;
        emit currentAlignmentChanged(currentAlignment);
        block_set_alignment = false;
    }
}

/*! \internal */

void Q3TextEdit::placeCursor(const QPoint &pos, Q3TextCursor *c, bool link)
{
#ifdef QT_TEXTEDIT_OPTIMIZATION
    if (d->optimMode)
        return;
#endif
    if (!c)
        c = cursor;

    if(c == cursor)
        resetInputContext();
    c->restoreState();
    Q3TextParagraph *s = doc->firstParagraph();
    c->place(pos, s, link);
}


QVariant Q3TextEdit::inputMethodQuery(Qt::InputMethodQuery query) const
{
    Q3TextCursor c(*cursor);

    switch(query) {
    case Qt::ImMicroFocus: {
        int h = c.paragraph()->lineHeightOfChar(cursor->index());
        return QRect(c.x() - contentsX() + frameWidth(),
                     c.y() + cursor->paragraph()->rect().y() - contentsY() + frameWidth(), 1, h);
    }
    case Qt::ImFont:
            return c.paragraph()->at(c.index())->format()->font();
    default:
    // ##### fix the others!
        return QWidget::inputMethodQuery(query);
    }
}



void Q3TextEdit::formatMore()
{
    if (!lastFormatted)
        return;

    int bottom = contentsHeight();
    int lastTop = -1;
    int lastBottom = -1;
    int to = 20;
    bool firstVisible = false;
    QRect cr(contentsX(), contentsY(), visibleWidth(), visibleHeight());
    for (int i = 0; lastFormatted &&
          (i < to || (firstVisible && lastTop < contentsY()+height()));
          i++) {
        lastFormatted->format();
        lastTop = lastFormatted->rect().top();
        lastBottom = lastFormatted->rect().bottom();
        if (i == 0)
            firstVisible = lastBottom < cr.bottom();
        bottom = qMax(bottom, lastBottom);
        lastFormatted = lastFormatted->next();
    }

    if (bottom > contentsHeight()) {
        resizeContents(contentsWidth(), qMax(doc->height(), bottom));
    } else if (!lastFormatted && lastBottom < contentsHeight()) {
        resizeContents(contentsWidth(), qMax(doc->height(), lastBottom));
        if (contentsHeight() < visibleHeight())
            updateContents(0, contentsHeight(), visibleWidth(),
                            visibleHeight() - contentsHeight());
    }

    if (lastFormatted)
        formatTimer->start(interval, true);
    else
        interval = qMax(0, interval);
}

void Q3TextEdit::doResize()
{
#ifdef QT_TEXTEDIT_OPTIMIZATION
    if (!d->optimMode)
#endif
    {
        if (wrapMode == FixedPixelWidth)
            return;
        doc->setMinimumWidth(-1);
        resizeContents(0, 0);
        doc->setWidth(visibleWidth());
        doc->invalidate();
        lastFormatted = doc->firstParagraph();
        interval = 0;
        formatMore();
    }
    repaintContents();
}

/*! \internal */

void Q3TextEdit::doChangeInterval()
{
    interval = 0;
}

/*!
    \reimp
*/

bool Q3TextEdit::eventFilter(QObject *o, QEvent *e)
{
#ifdef QT_TEXTEDIT_OPTIMIZATION
    if (!d->optimMode && (o == this || o == viewport())) {
#else
    if (o == this || o == viewport()) {
#endif
        if (d->cursorBlinkActive && e->type() == QEvent::FocusIn) {
            if (QApplication::cursorFlashTime() > 0)
                blinkTimer->start(QApplication::cursorFlashTime() / 2);
            drawCursor(true);
        } else if (e->type() == QEvent::FocusOut) {
            blinkTimer->stop();
            drawCursor(false);
        }
    }

    if (o == this && e->type() == QEvent::PaletteChange) {
        QColor old(viewport()->palette().color(QPalette::Text));
        if (old != palette().color(QPalette::Text)) {
            QColor c(palette().color(QPalette::Text));
            doc->setMinimumWidth(-1);
            doc->setDefaultFormat(doc->formatCollection()->defaultFormat()->font(), c);
            lastFormatted = doc->firstParagraph();
            formatMore();
            repaintChanged();
        }
    }

    return Q3ScrollView::eventFilter(o, e);
}

/*!
    Inserts the given \a text. If \a indent is true the paragraph that
    contains the text is reindented; if \a checkNewLine is true the \a
    text is checked for newlines and relaid out. If \a removeSelected
    is true and there is a selection, the insertion replaces the
    selected text.
 */
void Q3TextEdit::insert(const QString &text, bool indent,
                        bool checkNewLine, bool removeSelected)
{
    uint f = 0;
    if (indent)
        f |= RedoIndentation;
    if (checkNewLine)
        f |= CheckNewLines;
    if (removeSelected)
        f |= RemoveSelected;
    insert(text, f);
}

/*!
    Inserts \a text at the current cursor position.

    The \a insertionFlags define how the text is inserted. If \c
    RedoIndentation is set, the paragraph is re-indented. If \c
    CheckNewLines is set, newline characters in \a text result in hard
    line breaks (i.e. new paragraphs). If \c checkNewLine is not set,
    the behavior of the editor is undefined if the \a text contains
    newlines. (It is not possible to change Q3TextEdit's newline handling
    behavior, but you can use QString::replace() to preprocess text
    before inserting it.) If \c RemoveSelected is set, any selected
    text (in selection 0) is removed before the text is inserted.

    The default flags are \c CheckNewLines | \c RemoveSelected.

    If the widget is in Qt::LogText mode this function will do nothing.

    \sa paste() pasteSubType()
*/


void Q3TextEdit::insert(const QString &text, uint insertionFlags)
{
#ifdef QT_TEXTEDIT_OPTIMIZATION
    if (d->optimMode)
        return;
#endif

    if (cursor->nestedDepth() != 0) // #### for 3.0, disable editing of tables as this is not advanced enough
        return;

    bool indent = insertionFlags & RedoIndentation;
    bool checkNewLine = insertionFlags & CheckNewLines;
    bool removeSelected = insertionFlags & RemoveSelected;
    QString txt(text);
    drawCursor(false);
    if (!isReadOnly() && doc->hasSelection(Q3TextDocument::Standard) && removeSelected)
        removeSelectedText();
    Q3TextCursor c2 = *cursor;
    int oldLen = 0;

    if ( undoEnabled && !isReadOnly() && undoRedoInfo.type != UndoRedoInfo::IME ) {
        checkUndoRedoInfo(UndoRedoInfo::Insert);

        // If we are inserting at the end of the previous insertion, we keep this in
        // the same undo/redo command. Otherwise, we separate them in two different commands.
        if (undoRedoInfo.valid() && undoRedoInfo.index + undoRedoInfo.d->text.length() != cursor->index()) {
            clearUndoRedo();
            undoRedoInfo.type = UndoRedoInfo::Insert;
        }

        if (!undoRedoInfo.valid()) {
            undoRedoInfo.id = cursor->paragraph()->paragId();
            undoRedoInfo.index = cursor->index();
            undoRedoInfo.d->text.clear();
        }
        oldLen = undoRedoInfo.d->text.length();
    }

    lastFormatted = checkNewLine && cursor->paragraph()->prev() ?
                    cursor->paragraph()->prev() : cursor->paragraph();
    Q3TextCursor oldCursor = *cursor;
    cursor->insert(txt, checkNewLine);
    if (doc->useFormatCollection() && !doc->preProcessor()) {
        doc->setSelectionStart(Q3TextDocument::Temp, oldCursor);
        doc->setSelectionEnd(Q3TextDocument::Temp, *cursor);
        doc->setFormat(Q3TextDocument::Temp, currentFormat, Q3TextFormat::Format);
        doc->removeSelection(Q3TextDocument::Temp);
    }

    if (indent && (txt == QString(QLatin1Char('{')) || txt == QString(QLatin1Char('}')) || txt == QString(QLatin1Char(':')) || txt == QString(QLatin1Char('#'))))
        cursor->indent();
    formatMore();
    repaintChanged();
    ensureCursorVisible();
    drawCursor(true);

    if ( undoEnabled && !isReadOnly() && undoRedoInfo.type != UndoRedoInfo::IME ) {
        undoRedoInfo.d->text += txt;
        if (!doc->preProcessor()) {
            for (int i = 0; i < (int)txt.length(); ++i) {
                if (txt[i] != QLatin1Char('\n') && c2.paragraph()->at(c2.index())->format()) {
                    c2.paragraph()->at(c2.index())->format()->addRef();
                    undoRedoInfo.d->text.
                        setFormat(oldLen + i,
                                   c2.paragraph()->at(c2.index())->format(), true);
                }
                c2.gotoNextLetter();
            }
        }
    }

    if (!removeSelected) {
        doc->setSelectionStart(Q3TextDocument::Standard, oldCursor);
        doc->setSelectionEnd(Q3TextDocument::Standard, *cursor);
        repaintChanged();
    }

    setModified();
    emit textChanged();
}

/*!
    Inserts \a text in the paragraph \a para at position \a index.
*/

void Q3TextEdit::insertAt(const QString &text, int para, int index)
{
#ifdef QT_TEXTEDIT_OPTIMIZATION
    if (d->optimMode) {
        optimInsert(text, para, index);
        return;
    }
#endif
    Q3TextParagraph *p = doc->paragAt(para);
    if (!p)
        return;
    removeSelection(Q3TextDocument::Standard);
    Q3TextCursor tmp = *cursor;
    cursor->setParagraph(p);
    cursor->setIndex(index);
    insert(text, false, true, false);
    *cursor = tmp;
    removeSelection(Q3TextDocument::Standard);
}

/*!
    Inserts \a text as a new paragraph at position \a para. If \a para
    is -1, the text is appended. Use append() if the append operation
    is performance critical.
*/

void Q3TextEdit::insertParagraph(const QString &text, int para)
{
#ifdef QT_TEXTEDIT_OPTIMIZATION
    if (d->optimMode) {
        optimInsert(text + QLatin1Char('\n'), para, 0);
        return;
    }
#endif
    for (int i = 0; i < (int)doc->numSelections(); ++i)
        doc->removeSelection(i);

    Q3TextParagraph *p = doc->paragAt(para);

    bool append = !p;
    if (!p)
        p = doc->lastParagraph();

    Q3TextCursor old = *cursor;
    drawCursor(false);

    cursor->setParagraph(p);
    cursor->setIndex(0);
    clearUndoRedo();
    qtextedit_ignore_readonly = true;
    if (append && cursor->paragraph()->length() > 1) {
        cursor->setIndex(cursor->paragraph()->length() - 1);
        doKeyboardAction(ActionReturn);
    }
    insert(text, false, true, true);
    doKeyboardAction(ActionReturn);
    qtextedit_ignore_readonly = false;

    drawCursor(false);
    *cursor = old;
    drawCursor(true);

    repaintChanged();
}

/*!
    Removes the paragraph \a para.
*/

void Q3TextEdit::removeParagraph(int para)
{
#ifdef QT_TEXTEDIT_OPTIMIZATION
    if (d->optimMode)
        return;
#endif
    Q3TextParagraph *p = doc->paragAt(para);
    if (!p)
        return;

    for (int i = 0; i < doc->numSelections(); ++i)
        doc->removeSelection(i);

    Q3TextCursor start(doc);
    Q3TextCursor end(doc);
    start.setParagraph(p);
    start.setIndex(0);
    end.setParagraph(p);
    end.setIndex(p->length() - 1);

    if (!(p == doc->firstParagraph() && p == doc->lastParagraph())) {
        if (p->next()) {
            end.setParagraph(p->next());
            end.setIndex(0);
        } else if (p->prev()) {
            start.setParagraph(p->prev());
            start.setIndex(p->prev()->length() - 1);
        }
    }

    doc->setSelectionStart(Q3TextDocument::Temp, start);
    doc->setSelectionEnd(Q3TextDocument::Temp, end);
    removeSelectedText(Q3TextDocument::Temp);
}

/*!
    Undoes the last operation.

    If there is no operation to undo, i.e. there is no undo step in
    the undo/redo history, nothing happens.

    \sa undoAvailable() redo() undoDepth()
*/

void Q3TextEdit::undo()
{
    clearUndoRedo();
    if (isReadOnly() || !doc->commands()->isUndoAvailable() || !undoEnabled)
        return;

    for (int i = 0; i < (int)doc->numSelections(); ++i)
        doc->removeSelection(i);

#ifndef QT_NO_CURSOR
    viewport()->setCursor(isReadOnly() ? Qt::ArrowCursor : Qt::IBeamCursor);
#endif

    clearUndoRedo();
    drawCursor(false);
    Q3TextCursor *c = doc->undo(cursor);
    if (!c) {
        drawCursor(true);
        return;
    }
    lastFormatted = 0;
    repaintChanged();
    ensureCursorVisible();
    drawCursor(true);
    setModified();
    // ### If we get back to a completely blank textedit, it
    // is possible that cursor is invalid and further actions
    // might not fix the problem, so reset the cursor here.
    // This is copied from removeSeletedText(), it might be
    // okay to just call that.
    if (!cursor->isValid()) {
        delete cursor;
        cursor = new Q3TextCursor(doc);
        drawCursor(true);
        repaintContents();
    }
    emit undoAvailable(isUndoAvailable());
    emit redoAvailable(isRedoAvailable());
    emit textChanged();
}

/*!
    Redoes the last operation.

    If there is no operation to redo, i.e. there is no redo step in
    the undo/redo history, nothing happens.

    \sa redoAvailable() undo() undoDepth()
*/

void Q3TextEdit::redo()
{
    if (isReadOnly() || !doc->commands()->isRedoAvailable() || !undoEnabled)
        return;

    for (int i = 0; i < (int)doc->numSelections(); ++i)
        doc->removeSelection(i);

#ifndef QT_NO_CURSOR
    viewport()->setCursor(isReadOnly() ? Qt::ArrowCursor : Qt::IBeamCursor);
#endif

    clearUndoRedo();
    drawCursor(false);
    Q3TextCursor *c = doc->redo(cursor);
    if (!c) {
        drawCursor(true);
        return;
    }
    lastFormatted = 0;
    ensureCursorVisible();
    repaintChanged();
    ensureCursorVisible();
    drawCursor(true);
    setModified();
    emit undoAvailable(isUndoAvailable());
    emit redoAvailable(isRedoAvailable());
    emit textChanged();
}

/*!
    Pastes the text from the clipboard into the text edit at the
    current cursor position. Only plain text is pasted.

    If there is no text in the clipboard nothing happens.

    \sa pasteSubType() cut() Q3TextEdit::copy()
*/

void Q3TextEdit::paste()
{
#ifndef QT_NO_MIMECLIPBOARD
    if (isReadOnly())
        return;
    QString subType = QLatin1String("plain");
    if (textFormat() != Qt::PlainText) {
        QMimeSource *m = QApplication::clipboard()->data(d->clipboard_mode);
        if (!m)
            return;
        if (m->provides("application/x-qrichtext"))
            subType = QLatin1String("x-qrichtext");
    }

    pasteSubType(subType.toLatin1());
#endif
}

void Q3TextEdit::checkUndoRedoInfo(UndoRedoInfo::Type t)
{
    if (undoRedoInfo.valid() && t != undoRedoInfo.type) {
        clearUndoRedo();
    }
    undoRedoInfo.type = t;
}

/*!
    Repaints any paragraphs that have changed.

    Although used extensively internally you shouldn't need to call
    this yourself.
*/
void Q3TextEdit::repaintChanged()
{
    if (!updatesEnabled() || !viewport()->updatesEnabled())
        return;

    if (doc->firstParagraph())
        lastFormatted = doc->firstParagraph();
    updateContents(); // good enough until this class is rewritten
}

#ifndef QT_NO_MIME
Q3TextDrag *Q3TextEdit::dragObject(QWidget *parent) const
{
    if (!doc->hasSelection(Q3TextDocument::Standard) ||
         doc->selectedText(Q3TextDocument::Standard).isEmpty())
        return 0;
    if (textFormat() != Qt::RichText)
        return new Q3TextDrag(doc->selectedText(Q3TextDocument::Standard), parent);
    Q3RichTextDrag *drag = new Q3RichTextDrag(parent);
    drag->setPlainText(doc->selectedText(Q3TextDocument::Standard));
    drag->setRichText(doc->selectedText(Q3TextDocument::Standard, true));
    return drag;
}
#endif

/*!
    Copies the selected text (from selection 0) to the clipboard and
    deletes it from the text edit.

    If there is no selected text (in selection 0) nothing happens.

    \sa Q3TextEdit::copy() paste() pasteSubType()
*/

void Q3TextEdit::cut()
{
    if (isReadOnly())
        return;
    normalCopy();
    removeSelectedText();
}

void Q3TextEdit::normalCopy()
{
#ifndef QT_NO_MIME
    Q3TextDrag *drag = dragObject();
    if (!drag)
        return;
#ifndef QT_NO_MIMECLIPBOARD
    QApplication::clipboard()->setData(drag, d->clipboard_mode);
#endif // QT_NO_MIMECLIPBOARD
#endif // QT_NO_MIME
}

/*!
    Copies any selected text (from selection 0) to the clipboard.

    \sa hasSelectedText() copyAvailable()
*/

void Q3TextEdit::copy()
{
#ifndef QT_NO_CLIPBOARD
# ifdef QT_TEXTEDIT_OPTIMIZATION
    if (d->optimMode && optimHasSelection())
        QApplication::clipboard()->setText(optimSelectedText(), d->clipboard_mode);
    else
        normalCopy();
# else
    normalCopy();
# endif
#endif
}

/*!
    \internal

    Re-indents the current paragraph.
*/

void Q3TextEdit::indent()
{
    if (isReadOnly())
        return;

    drawCursor(false);
    if (!doc->hasSelection(Q3TextDocument::Standard))
        cursor->indent();
    else
        doc->indentSelection(Q3TextDocument::Standard);
    repaintChanged();
    drawCursor(true);
    setModified();
    emit textChanged();
}

/*!
    Reimplemented to allow tabbing through links. If \a n is true the
    tab moves the focus to the next child; if \a n is false the tab
    moves the focus to the previous child. Returns true if the focus
    was moved; otherwise returns false.
 */

bool Q3TextEdit::focusNextPrevChild(bool n)
{
    if (!isReadOnly() || !linksEnabled())
        return false;
    bool b = doc->focusNextPrevChild(n);
    repaintChanged();
    if (b) {
        Q3TextParagraph *p = doc->focusIndicator.parag;
        int start = doc->focusIndicator.start;
        int len = doc->focusIndicator.len;

        int y = p->rect().y();
        while (p
               && len == 0
               && p->at(start)->isCustom()
               && p->at(start)->customItem()->isNested()) {

            Q3TextTable *t = (Q3TextTable*)p->at(start)->customItem();
            QList<Q3TextTableCell *> cells = t->tableCells();
            for (int idx = 0; idx < cells.count(); ++idx) {
                Q3TextTableCell *c = cells.at(idx);
                Q3TextDocument *cellDoc = c->richText();
                if ( cellDoc->hasFocusParagraph() ) {
                    y += c->geometry().y() + c->verticalAlignmentOffset();

                    p = cellDoc->focusIndicator.parag;
                    start = cellDoc->focusIndicator.start;
                    len = cellDoc->focusIndicator.len;
                    if ( p )
                        y += p->rect().y();

                    break;
                }
            }
        }
        setContentsPos( contentsX(), QMIN( y, contentsHeight() - visibleHeight() ) );
    }
    return b;
}

/*!
    \internal

  This functions sets the current format to \a f. Only the fields of \a
  f which are specified by the \a flags are used.
*/

void Q3TextEdit::setFormat(Q3TextFormat *f, int flags)
{
    if (doc->hasSelection(Q3TextDocument::Standard)) {
        drawCursor(false);
        Q3TextCursor c1 = doc->selectionStartCursor(Q3TextDocument::Standard);
        c1.restoreState();
        Q3TextCursor c2 = doc->selectionEndCursor(Q3TextDocument::Standard);
        c2.restoreState();
        if (undoEnabled) {
            clearUndoRedo();
            undoRedoInfo.type = UndoRedoInfo::Format;
            undoRedoInfo.id = c1.paragraph()->paragId();
            undoRedoInfo.index = c1.index();
            undoRedoInfo.eid = c2.paragraph()->paragId();
            undoRedoInfo.eindex = c2.index();
            readFormats(c1, c2, undoRedoInfo.d->text);
            undoRedoInfo.format = f;
            undoRedoInfo.flags = flags;
            clearUndoRedo();
        }
        doc->setFormat(Q3TextDocument::Standard, f, flags);
        repaintChanged();
        formatMore();
        drawCursor(true);
        setModified();
        emit textChanged();
    }
    if (currentFormat && currentFormat->key() != f->key()) {
        currentFormat->removeRef();
        currentFormat = doc->formatCollection()->format(f);
        if (currentFormat->isMisspelled()) {
            currentFormat->removeRef();
            currentFormat = doc->formatCollection()->format(currentFormat->font(),
                                                             currentFormat->color());
        }
        emit currentFontChanged(currentFormat->font());
        emit currentColorChanged(currentFormat->color());
        emit currentVerticalAlignmentChanged((VerticalAlignment)currentFormat->vAlign());
        if (cursor->index() == cursor->paragraph()->length() - 1) {
            currentFormat->addRef();
            cursor->paragraph()->string()->setFormat(cursor->index(), currentFormat, true);
            if (cursor->paragraph()->length() == 1) {
                cursor->paragraph()->invalidate(0);
                cursor->paragraph()->format();
                repaintChanged();
            }
        }
    }
}

/*! \internal
  \warning In Qt 3.1 we will provide a cleaer API for the
  functionality which is provided by this function and in Qt 4.0 this
  function will go away.

  Sets the paragraph style of the current paragraph
  to \a dm. If \a dm is Q3StyleSheetItem::DisplayListItem, the
  type of the list item is set to \a listStyle.

  \sa setAlignment()
*/

void Q3TextEdit::setParagType(Q3StyleSheetItem::DisplayMode dm,
                              Q3StyleSheetItem::ListStyle listStyle)
{
    if (isReadOnly())
        return;

    drawCursor(false);
    Q3TextParagraph *start = cursor->paragraph();
    Q3TextParagraph *end = start;
    if (doc->hasSelection(Q3TextDocument::Standard)) {
        start = doc->selectionStartCursor(Q3TextDocument::Standard).topParagraph();
        end = doc->selectionEndCursor(Q3TextDocument::Standard).topParagraph();
        if (end->paragId() < start->paragId())
            return; // do not trust our selections
    }

    clearUndoRedo();
    undoRedoInfo.type = UndoRedoInfo::Style;
    undoRedoInfo.id = start->paragId();
    undoRedoInfo.eid = end->paragId();
    undoRedoInfo.styleInformation = Q3TextStyleCommand::readStyleInformation(doc, undoRedoInfo.id, undoRedoInfo.eid);

    while (start != end->next()) {
        start->setListStyle(listStyle);
        if (dm == Q3StyleSheetItem::DisplayListItem) {
            start->setListItem(true);
            if(start->listDepth() == 0)
                start->setListDepth(1);
        } else if (start->isListItem()) {
            start->setListItem(false);
            start->setListDepth(qMax(start->listDepth()-1, 0));
        }
        start = start->next();
    }

    clearUndoRedo();
    repaintChanged();
    formatMore();
    drawCursor(true);
    setModified();
    emit textChanged();
}

/*!
    Sets the alignment of the current paragraph to \a a. Valid
    alignments are Qt::AlignLeft, Qt::AlignRight,
    Qt::AlignJustify and Qt::AlignCenter (which centers
    horizontally).
*/

void Q3TextEdit::setAlignment(int a)
{
    if (isReadOnly() || block_set_alignment)
        return;

    drawCursor(false);
    Q3TextParagraph *start = cursor->paragraph();
    Q3TextParagraph *end = start;
    if (doc->hasSelection(Q3TextDocument::Standard)) {
        start = doc->selectionStartCursor(Q3TextDocument::Standard).topParagraph();
        end = doc->selectionEndCursor(Q3TextDocument::Standard).topParagraph();
        if (end->paragId() < start->paragId())
            return; // do not trust our selections
    }

    clearUndoRedo();
    undoRedoInfo.type = UndoRedoInfo::Style;
    undoRedoInfo.id = start->paragId();
    undoRedoInfo.eid = end->paragId();
    undoRedoInfo.styleInformation = Q3TextStyleCommand::readStyleInformation(doc, undoRedoInfo.id, undoRedoInfo.eid);

    while (start != end->next()) {
        start->setAlignment(a);
        start = start->next();
    }

    clearUndoRedo();
    repaintChanged();
    formatMore();
    drawCursor(true);
    if (currentAlignment != a) {
        currentAlignment = a;
        emit currentAlignmentChanged(currentAlignment);
    }
    setModified();
    emit textChanged();
}

void Q3TextEdit::updateCurrentFormat()
{
    int i = cursor->index();
    if (i > 0)
        --i;
    if (doc->useFormatCollection() &&
         (!currentFormat || currentFormat->key() != cursor->paragraph()->at(i)->format()->key())) {
        if (currentFormat)
            currentFormat->removeRef();
        currentFormat = doc->formatCollection()->format(cursor->paragraph()->at(i)->format());
        if (currentFormat->isMisspelled()) {
            currentFormat->removeRef();
            currentFormat = doc->formatCollection()->format(currentFormat->font(), currentFormat->color());
        }
        emit currentFontChanged(currentFormat->font());
        emit currentColorChanged(currentFormat->color());
        emit currentVerticalAlignmentChanged((VerticalAlignment)currentFormat->vAlign());
    }

    if (currentAlignment != cursor->paragraph()->alignment()) {
        currentAlignment = cursor->paragraph()->alignment();
        block_set_alignment = true;
        emit currentAlignmentChanged(currentAlignment);
        block_set_alignment = false;
    }
}

/*!
    If \a b is true sets the current format to italic; otherwise sets
    the current format to non-italic.

    \sa italic()
*/

void Q3TextEdit::setItalic(bool b)
{
    Q3TextFormat f(*currentFormat);
    f.setItalic(b);
    Q3TextFormat *f2 = doc->formatCollection()->format(&f);
    setFormat(f2, Q3TextFormat::Italic);
}

/*!
    If \a b is true sets the current format to bold; otherwise sets
    the current format to non-bold.

    \sa bold()
*/

void Q3TextEdit::setBold(bool b)
{
    Q3TextFormat f(*currentFormat);
    f.setBold(b);
    Q3TextFormat *f2 = doc->formatCollection()->format(&f);
    setFormat(f2, Q3TextFormat::Bold);
}

/*!
    If \a b is true sets the current format to underline; otherwise
    sets the current format to non-underline.

    \sa underline()
*/

void Q3TextEdit::setUnderline(bool b)
{
    Q3TextFormat f(*currentFormat);
    f.setUnderline(b);
    Q3TextFormat *f2 = doc->formatCollection()->format(&f);
    setFormat(f2, Q3TextFormat::Underline);
}

/*!
    Sets the font family of the current format to \a fontFamily.

    \sa family() setCurrentFont()
*/

void Q3TextEdit::setFamily(const QString &fontFamily)
{
    Q3TextFormat f(*currentFormat);
    f.setFamily(fontFamily);
    Q3TextFormat *f2 = doc->formatCollection()->format(&f);
    setFormat(f2, Q3TextFormat::Family);
}

/*!
    Sets the point size of the current format to \a s.

    Note that if \a s is zero or negative, the behavior of this
    function is not defined.

    \sa pointSize() setCurrentFont() setFamily()
*/

void Q3TextEdit::setPointSize(int s)
{
    Q3TextFormat f(*currentFormat);
    f.setPointSize(s);
    Q3TextFormat *f2 = doc->formatCollection()->format(&f);
    setFormat(f2, Q3TextFormat::Size);
}

/*!
    Sets the color of the current format, i.e. of the text, to \a c.

    \sa color() setPaper()
*/

void Q3TextEdit::setColor(const QColor &c)
{
    Q3TextFormat f(*currentFormat);
    f.setColor(c);
    Q3TextFormat *f2 = doc->formatCollection()->format(&f);
    setFormat(f2, Q3TextFormat::Color);
}

/*!
    Sets the vertical alignment of the current format, i.e. of the
    text, to \a a.

    \sa color() setPaper()
*/

void Q3TextEdit::setVerticalAlignment(Q3TextEdit::VerticalAlignment a)
{
    Q3TextFormat f(*currentFormat);
    f.setVAlign((Q3TextFormat::VerticalAlignment)a);
    Q3TextFormat *f2 = doc->formatCollection()->format(&f);
    setFormat(f2, Q3TextFormat::VAlign);
}

void Q3TextEdit::setFontInternal(const QFont &f_)
{
    QFont font = f_;
    if (font.kerning())
        font.setKerning(false);
    Q3TextFormat f(*currentFormat);
    f.setFont(font);
    Q3TextFormat *f2 = doc->formatCollection()->format(&f);
    setFormat(f2, Q3TextFormat::Font);
}


QString Q3TextEdit::text() const
{
#ifdef QT_TEXTEDIT_OPTIMIZATION
    if (d->optimMode)
        return optimText();
#endif

    Q3TextParagraph *p = doc->firstParagraph();
    if (!p || (!p->next() && p->length() <= 1))
        return QString::fromLatin1("");

    if (isReadOnly())
        return doc->originalText();
    return doc->text();
}

/*!
    \overload

    Returns the text of paragraph \a para.

    If textFormat() is Qt::RichText the text will contain HTML
    formatting tags.
*/

QString Q3TextEdit::text(int para) const
{
#ifdef QT_TEXTEDIT_OPTIMIZATION
    if (d->optimMode && (d->od->numLines >= para)) {
        QString paraStr = d->od->lines[LOGOFFSET(para)];
        if (paraStr.isEmpty())
            paraStr = QLatin1Char('\n');
        return paraStr;
    } else
#endif
    return doc->text(para);
}

/*!
    \overload

    Changes the text of the text edit to the string \a text and the
    context to \a context. Any previous text is removed.

    \a text may be interpreted either as plain text or as rich text,
    depending on the textFormat(). The default setting is Qt::AutoText,
    i.e. the text edit auto-detects the format from \a text.

    For rich text the rendering style and available tags are defined
    by a styleSheet(); see Q3StyleSheet for details.

    The optional \a context is a path which the text edit's
    Q3MimeSourceFactory uses to resolve the locations of files and
    images. (See \l{Q3TextEdit::Q3TextEdit()}.) It is passed to the text
    edit's Q3MimeSourceFactory when quering data.

    Note that the undo/redo history is cleared by this function.

    \sa text(), setTextFormat()
*/

void Q3TextEdit::setText(const QString &text, const QString &context)
{
#ifdef QT_TEXTEDIT_OPTIMIZATION
    if (d->optimMode) {
        optimSetText(text);
        return;
    }
#endif
    if (!isModified() && isReadOnly() &&
         this->context() == context && this->text() == text)
        return;

    emit undoAvailable(false);
    emit redoAvailable(false);
    undoRedoInfo.clear();
    doc->commands()->clear();

    lastFormatted = 0;
    int oldCursorPos = cursor->index();
    int oldCursorPar = cursor->paragraph()->paragId();
    cursor->restoreState();
    delete cursor;
    doc->setText(text, context);

    if (wrapMode == FixedPixelWidth) {
        resizeContents(wrapWidth, 0);
        doc->setWidth(wrapWidth);
        doc->setMinimumWidth(wrapWidth);
    } else {
        doc->setMinimumWidth(-1);
        resizeContents(0, 0);
    }

    lastFormatted = doc->firstParagraph();
    cursor = new Q3TextCursor(doc);
    updateContents();

    if (isModified())
        setModified(false);
    emit textChanged();
    if (cursor->index() != oldCursorPos || cursor->paragraph()->paragId() != oldCursorPar) {
        emit cursorPositionChanged(cursor);
        emit cursorPositionChanged(cursor->paragraph()->paragId(), cursor->index());
    }
    formatMore();
    updateCurrentFormat();
    d->scrollToAnchor.clear();
}

/*!
    \property Q3TextEdit::text
    \brief the text edit's text

    There is no default text.

    On setting, any previous text is deleted.

    The text may be interpreted either as plain text or as rich text,
    depending on the textFormat(). The default setting is Qt::AutoText,
    i.e. the text edit auto-detects the format of the text.

    For richtext, calling text() on an editable Q3TextEdit will cause
    the text to be regenerated from the textedit. This may mean that
    the QString returned may not be exactly the same as the one that
    was set.

    \sa textFormat
*/


/*!
    \property Q3TextEdit::readOnly
    \brief whether the text edit is read-only

    In a read-only text edit the user can only navigate through the
    text and select text; modifying the text is not possible.

    This property's default is false.
*/

/*!
    Finds the next occurrence of the string, \a expr. Returns true if
    \a expr was found; otherwise returns false.

    If \a para and \a index are both 0 the search begins from the
    current cursor position. If \a para and \a index are both not 0,
    the search begins from the \c{*}\a{index} character position in the
    \c{*}\a{para} paragraph.

    If \a cs is true the search is case sensitive, otherwise it is
    case insensitive. If \a wo is true the search looks for whole word
    matches only; otherwise it searches for any matching text. If \a
    forward is true (the default) the search works forward from the
    starting position to the end of the text, otherwise it works
    backwards to the beginning of the text.

    If \a expr is found the function returns true. If \a index and \a
    para are not 0, the number of the paragraph in which the first
    character of the match was found is put into \c{*}\a{para}, and the
    index position of that character within the paragraph is put into
    \c{*}\a{index}.

    If \a expr is not found the function returns false. If \a index
    and \a para are not 0 and \a expr is not found, \c{*}\a{index}
    and \c{*}\a{para} are undefined.

    Please note that this function will make the next occurrence of
    the string (if found) the current selection, and will thus
    modify the cursor position.

    Using the \a para and \a index parameters will not work correctly
    in case the document contains tables.
*/

bool Q3TextEdit::find(const QString &expr, bool cs, bool wo, bool forward,
                      int *para, int *index)
{
#ifdef QT_TEXTEDIT_OPTIMIZATION
    if (d->optimMode)
        return optimFind(expr, cs, wo, forward, para, index);
#endif
    drawCursor(false);
#ifndef QT_NO_CURSOR
    viewport()->setCursor(isReadOnly() ? Qt::ArrowCursor : Qt::IBeamCursor);
#endif
    Q3TextCursor findcur = *cursor;
    if (para && index) {
        if (doc->paragAt(*para))
            findcur.gotoPosition(doc->paragAt(*para), *index);
        else
            findcur.gotoEnd();
    } else if (doc->hasSelection(Q3TextDocument::Standard)){
        // maks sure we do not find the same selection again
        if (forward)
            findcur.gotoNextLetter();
        else
            findcur.gotoPreviousLetter();
    } else if (!forward && findcur.index() == 0 && findcur.paragraph() == findcur.topParagraph()) {
        findcur.gotoEnd();
    }
    removeSelection(Q3TextDocument::Standard);
    bool found = doc->find(findcur, expr, cs, wo, forward);
    if (found) {
        if (para)
            *para = findcur.paragraph()->paragId();
        if (index)
            *index = findcur.index();
        *cursor = findcur;
        repaintChanged();
        ensureCursorVisible();
    }
    drawCursor(true);
    if (found) {
        emit cursorPositionChanged(cursor);
        emit cursorPositionChanged(cursor->paragraph()->paragId(), cursor->index());
    }
    return found;
}

void Q3TextEdit::blinkCursor()
{
    bool cv = cursorVisible;
    blinkCursorVisible = !blinkCursorVisible;
    drawCursor(blinkCursorVisible);
    cursorVisible = cv;
}

/*!
    Sets the cursor to position \a index in paragraph \a para.

    \sa getCursorPosition()
*/

void Q3TextEdit::setCursorPosition(int para, int index)
{
    Q3TextParagraph *p = doc->paragAt(para);
    if (!p)
        return;

    if (index > p->length() - 1)
        index = p->length() - 1;

    drawCursor(false);
    cursor->setParagraph(p);
    cursor->setIndex(index);
    ensureCursorVisible();
    drawCursor(true);
    updateCurrentFormat();
    emit cursorPositionChanged(cursor);
    emit cursorPositionChanged(cursor->paragraph()->paragId(), cursor->index());
}

/*!
    This function sets the \c{*}\a{para} and \c{*}\a{index} parameters to the
    current cursor position. \a para and \a index must not be 0.

    \sa setCursorPosition()
*/

void Q3TextEdit::getCursorPosition(int *para, int *index) const
{
    if (!para || !index)
        return;
    *para = cursor->paragraph()->paragId();
    *index = cursor->index();
}

/*!
    Sets a selection which starts at position \a indexFrom in
    paragraph \a paraFrom and ends at position \a indexTo in paragraph
    \a paraTo.

    Any existing selections which have a different id (\a selNum) are
    left alone, but if an existing selection has the same id as \a
    selNum it is removed and replaced by this selection.

    Uses the selection settings of selection \a selNum. If \a selNum
    is 0, this is the default selection.

    The cursor is moved to the end of the selection if \a selNum is 0,
    otherwise the cursor position remains unchanged.

    \sa getSelection() selectedText
*/

void Q3TextEdit::setSelection(int paraFrom, int indexFrom,
                              int paraTo, int indexTo, int selNum)
{
#ifdef QT_TEXTEDIT_OPTIMIZATION
    if (d->optimMode) {
        optimSetSelection(paraFrom, indexFrom, paraTo, indexTo);
        repaintContents();
        return;
    }
#endif
    if (doc->hasSelection(selNum)) {
        doc->removeSelection(selNum);
        repaintChanged();
    }
    if (selNum > doc->numSelections() - 1)
        doc->addSelection(selNum);
    Q3TextParagraph *p1 = doc->paragAt(paraFrom);
    if (!p1)
        return;
    Q3TextParagraph *p2 = doc->paragAt(paraTo);
    if (!p2)
        return;

    if (indexFrom > p1->length() - 1)
        indexFrom = p1->length() - 1;
    if (indexTo > p2->length() - 1)
        indexTo = p2->length() - 1;

    drawCursor(false);
    Q3TextCursor c = *cursor;
    Q3TextCursor oldCursor = *cursor;
    c.setParagraph(p1);
    c.setIndex(indexFrom);
    cursor->setParagraph(p2);
    cursor->setIndex(indexTo);
    doc->setSelectionStart(selNum, c);
    doc->setSelectionEnd(selNum, *cursor);
    repaintChanged();
    ensureCursorVisible();
    if (selNum != Q3TextDocument::Standard)
        *cursor = oldCursor;
    drawCursor(true);
}

/*!
    If there is a selection, \c{*}\a{paraFrom} is set to the number of the
    paragraph in which the selection begins and \c{*}\a{paraTo} is set to
    the number of the paragraph in which the selection ends. (They
    could be the same.) \c{*}\a{indexFrom} is set to the index at which the
    selection begins within \c{*}\a{paraFrom}, and \c{*}\a{indexTo} is set to
    the index at which the selection ends within \c{*}\a{paraTo}.

    If there is no selection, \c{*}\a{paraFrom}, \c{*}\a{indexFrom},
    \c{*}\a{paraTo} and \c{*}\a{indexTo} are all set to -1.

    If \a paraFrom, \a indexFrom, \a paraTo or \a indexTo is 0 this
    function does nothing.

    The \a selNum is the number of the selection (multiple selections
    are supported). It defaults to 0 (the default selection).

    \sa setSelection() selectedText
*/

void Q3TextEdit::getSelection(int *paraFrom, int *indexFrom,
                              int *paraTo, int *indexTo, int selNum) const
{
    if (!paraFrom || !paraTo || !indexFrom || !indexTo)
        return;
#ifdef QT_TEXTEDIT_OPTIMIZATION
    if (d->optimMode) {
        *paraFrom = d->od->selStart.line;
        *paraTo = d->od->selEnd.line;
        *indexFrom = d->od->selStart.index;
        *indexTo = d->od->selEnd.index;
        return;
    }
#endif
    if (!doc->hasSelection(selNum)) {
        *paraFrom = -1;
        *indexFrom = -1;
        *paraTo = -1;
        *indexTo = -1;
        return;
    }

    doc->selectionStart(selNum, *paraFrom, *indexFrom);
    doc->selectionEnd(selNum, *paraTo, *indexTo);
}

/*!
    \property Q3TextEdit::textFormat
    \brief the text format: rich text, plain text, log text or auto text.

    The text format is one of the following:
    \list
    \i Qt::PlainText - all characters, except newlines, are displayed
    verbatim, including spaces. Whenever a newline appears in the text
    the text edit inserts a hard line break and begins a new
    paragraph.
    \i Qt::RichText - rich text rendering. The available styles are
    defined in the default stylesheet Q3StyleSheet::defaultSheet().
    \i Qt::LogText -  optimized mode for very large texts. Supports a very
    limited set of formatting tags (color, bold, underline and italic
    settings).
    \i Qt::AutoText - this is the default. The text edit autodetects which
    rendering style is best, Qt::PlainText or Qt::RichText. This is done
    by using the Q3StyleSheet::mightBeRichText() function.
    \endlist
*/

void Q3TextEdit::setTextFormat(Qt::TextFormat format)
{
    doc->setTextFormat(format);
#ifdef QT_TEXTEDIT_OPTIMIZATION
    checkOptimMode();
#endif
}

Qt::TextFormat Q3TextEdit::textFormat() const
{
    return doc->textFormat();
}

/*!
    Returns the number of paragraphs in the text; an empty textedit is always
    considered to have one paragraph, so 1 is returned in this case.
*/

int Q3TextEdit::paragraphs() const
{
#ifdef QT_TEXTEDIT_OPTIMIZATION
    if (d->optimMode) {
        return d->od->numLines;
    }
#endif
    return doc->lastParagraph()->paragId() + 1;
}

/*!
    Returns the number of lines in paragraph \a para, or -1 if there
    is no paragraph with index \a para.
*/

int Q3TextEdit::linesOfParagraph(int para) const
{
#ifdef QT_TEXTEDIT_OPTIMIZATION
    if (d->optimMode) {
        if (d->od->numLines >= para)
            return 1;
        else
            return -1;
    }
#endif
    Q3TextParagraph *p = doc->paragAt(para);
    if (!p)
        return -1;
    return p->lines();
}

/*!
    Returns the length of the paragraph \a para (i.e. the number of
    characters), or -1 if there is no paragraph with index \a para.

    This function ignores newlines.
*/

int Q3TextEdit::paragraphLength(int para) const
{
#ifdef QT_TEXTEDIT_OPTIMIZATION
    if (d->optimMode) {
        if (d->od->numLines >= para) {
            if (d->od->lines[LOGOFFSET(para)].isEmpty()) // CR
                return 1;
            else
                return d->od->lines[LOGOFFSET(para)].length();
        }
        return -1;
    }
#endif
    Q3TextParagraph *p = doc->paragAt(para);
    if (!p)
        return -1;
    return p->length() - 1;
}

/*!
    Returns the number of lines in the text edit; this could be 0.

    \warning This function may be slow. Lines change all the time
    during word wrapping, so this function has to iterate over all the
    paragraphs and get the number of lines from each one individually.
*/

int Q3TextEdit::lines() const
{
#ifdef QT_TEXTEDIT_OPTIMIZATION
    if (d->optimMode) {
        return d->od->numLines;
    }
#endif
    Q3TextParagraph *p = doc->firstParagraph();
    int l = 0;
    while (p) {
        l += p->lines();
        p = p->next();
    }

    return l;
}

/*!
    Returns the line number of the line in paragraph \a para in which
    the character at position \a index appears. The \a index position is
    relative to the beginning of the paragraph. If there is no such
    paragraph or no such character at the \a index position (e.g. the
    index is out of range) -1 is returned.
*/

int Q3TextEdit::lineOfChar(int para, int index)
{
    Q3TextParagraph *p = doc->paragAt(para);
    if (!p)
        return -1;

    int idx, line;
    Q3TextStringChar *c = p->lineStartOfChar(index, &idx, &line);
    if (!c)
        return -1;

    return line;
}

void Q3TextEdit::setModified(bool m)
{
    bool oldModified = modified;
    modified = m;
    if (modified && doc->oTextValid)
        doc->invalidateOriginalText();
    if (oldModified != modified)
        emit modificationChanged(modified);
}

/*!
    \property Q3TextEdit::modified
    \brief whether the document has been modified by the user
*/

bool Q3TextEdit::isModified() const
{
    return modified;
}

void Q3TextEdit::setModified()
{
    if (!isModified())
        setModified(true);
}

/*!
    Returns true if the current format is italic; otherwise returns false.

    \sa setItalic()
*/

bool Q3TextEdit::italic() const
{
    return currentFormat->font().italic();
}

/*!
    Returns true if the current format is bold; otherwise returns false.

    \sa setBold()
*/

bool Q3TextEdit::bold() const
{
    return currentFormat->font().bold();
}

/*!
    Returns true if the current format is underlined; otherwise returns
    false.

    \sa setUnderline()
*/

bool Q3TextEdit::underline() const
{
    return currentFormat->font().underline();
}

/*!
    Returns the font family of the current format.

    \sa setFamily() setCurrentFont() setPointSize()
*/

QString Q3TextEdit::family() const
{
    return currentFormat->font().family();
}

/*!
    Returns the point size of the font of the current format.

    \sa setFamily() setCurrentFont() setPointSize()
*/

int Q3TextEdit::pointSize() const
{
    return currentFormat->font().pointSize();
}

/*!
    Returns the color of the current format.

    \sa setColor() setPaper()
*/

QColor Q3TextEdit::color() const
{
    return currentFormat->color();
}

/*!
    Returns Q3ScrollView::font()

    \warning In previous versions this function returned the font of
    the current format. This lead to confusion. Please use
    currentFont() instead.
*/

QFont Q3TextEdit::font() const
{
    return Q3ScrollView::font();
}

/*!
    Returns the font of the current format.

    \sa setCurrentFont() setFamily() setPointSize()
*/

QFont Q3TextEdit::currentFont() const
{
    return currentFormat->font();
}


/*!
    Returns the alignment of the current paragraph.

    \sa setAlignment()
*/

int Q3TextEdit::alignment() const
{
    return currentAlignment;
}

/*!
    Returns the vertical alignment of the current format.

    \sa setVerticalAlignment()
*/

Q3TextEdit::VerticalAlignment Q3TextEdit::verticalAlignment() const
{
    return (Q3TextEdit::VerticalAlignment) currentFormat->vAlign();
}

void Q3TextEdit::startDrag()
{
#ifndef QT_NO_DRAGANDDROP
    mousePressed = false;
    inDoubleClick = false;
    Q3DragObject *drag = dragObject(viewport());
    if (!drag)
        return;
    if (isReadOnly()) {
        drag->dragCopy();
    } else {
        if (drag->drag() && Q3DragObject::target() != this && Q3DragObject::target() != viewport())
            removeSelectedText();
    }
#endif
}

/*!
    If \a select is true (the default), all the text is selected as
    selection 0. If \a select is false any selected text is
    unselected, i.e. the default selection (selection 0) is cleared.

    \sa selectedText
*/

void Q3TextEdit::selectAll(bool select)
{
#ifdef QT_TEXTEDIT_OPTIMIZATION
    if (d->optimMode) {
        if (select)
            optimSelectAll();
        else
            optimRemoveSelection();
        return;
    }
#endif
    if (!select)
        doc->removeSelection(Q3TextDocument::Standard);
    else
        doc->selectAll(Q3TextDocument::Standard);
    repaintChanged();
    emit copyAvailable(doc->hasSelection(Q3TextDocument::Standard));
    emit selectionChanged();
#ifndef QT_NO_CURSOR
    viewport()->setCursor(isReadOnly() ? Qt::ArrowCursor : Qt::IBeamCursor);
#endif
}

void Q3TextEdit::UndoRedoInfo::clear()
{
    if (valid()) {
        if (type == Insert || type == Return)
            doc->addCommand(new Q3TextInsertCommand(doc, id, index, d->text.rawData(), styleInformation));
        else if (type == Format)
            doc->addCommand(new Q3TextFormatCommand(doc, id, index, eid, eindex, d->text.rawData(), format, flags));
        else if (type == Style)
            doc->addCommand(new Q3TextStyleCommand(doc, id, eid, styleInformation));
        else if (type != Invalid) {
            doc->addCommand(new Q3TextDeleteCommand(doc, id, index, d->text.rawData(), styleInformation));
        }
    }
    type = Invalid;
    d->text.clear();
    id = -1;
    index = -1;
    styleInformation = QByteArray();
}


/*!
    If there is some selected text (in selection 0) it is deleted. If
    there is no selected text (in selection 0) the character to the
    right of the text cursor is deleted.

    \sa removeSelectedText() cut()
*/

void Q3TextEdit::del()
{
    if (doc->hasSelection(Q3TextDocument::Standard)) {
        removeSelectedText();
        return;
    }

    doKeyboardAction(ActionDelete);
}


Q3TextEdit::UndoRedoInfo::UndoRedoInfo(Q3TextDocument *dc)
    : type(Invalid), doc(dc)
{
    d = new QUndoRedoInfoPrivate;
    d->text.clear();
    id = -1;
    index = -1;
}

Q3TextEdit::UndoRedoInfo::~UndoRedoInfo()
{
    delete d;
}

bool Q3TextEdit::UndoRedoInfo::valid() const
{
    return id >= 0 &&  type != Invalid;
}

/*!
    \internal

  Resets the current format to the default format.
*/

void Q3TextEdit::resetFormat()
{
    setAlignment(Qt::AlignAuto);
    setParagType(Q3StyleSheetItem::DisplayBlock, Q3StyleSheetItem::ListDisc);
    setFormat(doc->formatCollection()->defaultFormat(), Q3TextFormat::Format);
}

/*!
    Returns the Q3StyleSheet which is being used by this text edit.

    \sa setStyleSheet()
*/

Q3StyleSheet* Q3TextEdit::styleSheet() const
{
    return doc->styleSheet();
}

/*!
    Sets the stylesheet to use with this text edit to \a styleSheet.
    Changes will only take effect for new text added with setText() or
    append().

    \sa styleSheet()
*/

void Q3TextEdit::setStyleSheet(Q3StyleSheet* styleSheet)
{
    doc->setStyleSheet(styleSheet);
}

/*!
    \property Q3TextEdit::paper
    \brief the background (paper) brush.

    The brush that is currently used to draw the background of the
    text edit. The initial setting is an empty brush.
*/

void Q3TextEdit::setPaper(const QBrush& pap)
{
    doc->setPaper(new QBrush(pap));
    if ( pap.pixmap() )
        viewport()->setBackgroundPixmap( *pap.pixmap() );
    QPalette pal = palette();
    pal.setColor(QPalette::Window, pap.color());
    setPalette(pal);
    pal = viewport()->palette();
    pal.setColor(QPalette::Window, pap.color());
    viewport()->setPalette(pal);
#ifdef QT_TEXTEDIT_OPTIMIZATION
    // force a repaint of the entire viewport - using updateContents()
    // would clip the coords to the content size
    if (d->optimMode)
        repaintContents(contentsX(), contentsY(), viewport()->width(), viewport()->height());
    else
#endif
        updateContents();
}

QBrush Q3TextEdit::paper() const
{
    if (doc->paper())
        return *doc->paper();
    return QBrush(palette().base());
}

/*!
    \property Q3TextEdit::linkUnderline
    \brief whether hypertext links will be underlined

    If true (the default) hypertext links will be displayed
    underlined. If false links will not be displayed underlined.
*/

void Q3TextEdit::setLinkUnderline(bool b)
{
    if (doc->underlineLinks() == b)
        return;
    doc->setUnderlineLinks(b);
    repaintChanged();
}

bool Q3TextEdit::linkUnderline() const
{
    return doc->underlineLinks();
}

/*!
    Sets the text edit's mimesource factory to \a factory. See
    Q3MimeSourceFactory for further details.

    \sa mimeSourceFactory()
 */

#ifndef QT_NO_MIME
void Q3TextEdit::setMimeSourceFactory(Q3MimeSourceFactory* factory)
{
    doc->setMimeSourceFactory(factory);
}

/*!
    Returns the Q3MimeSourceFactory which is being used by this text
    edit.

    \sa setMimeSourceFactory()
*/

Q3MimeSourceFactory* Q3TextEdit::mimeSourceFactory() const
{
    return doc->mimeSourceFactory();
}
#endif

/*!
    Returns how many pixels high the text edit needs to be to display
    all the text if the text edit is \a w pixels wide.
*/

int Q3TextEdit::heightForWidth(int w) const
{
    int oldw = doc->width();
    doc->doLayout(0, w);
    int h = doc->height();
    doc->setWidth(oldw);
    doc->invalidate();
    ((Q3TextEdit*)this)->formatMore();
    return h;
}

/*!
    Appends a new paragraph with \a text to the end of the text edit. Note that
    the undo/redo history is cleared by this function, and no undo
    history is kept for appends which makes them faster than
    insert()s. If you want to append text which is added to the
    undo/redo history as well, use insertParagraph().
*/

void Q3TextEdit::append(const QString &text)
{
#ifdef QT_TEXTEDIT_OPTIMIZATION
    if (d->optimMode) {
        optimAppend(text);
        return;
    }
#endif
    // flush and clear the undo/redo stack if necessary
    undoRedoInfo.clear();
    doc->commands()->clear();

    doc->removeSelection(Q3TextDocument::Standard);
    Qt::TextFormat f = doc->textFormat();
    if (f == Qt::AutoText) {
        if (Q3StyleSheet::mightBeRichText(text))
            f = Qt::RichText;
        else
            f = Qt::PlainText;
    }

    drawCursor(false);
    Q3TextCursor oldc(*cursor);
    ensureFormatted(doc->lastParagraph());
    bool atBottom = contentsY() >= contentsHeight() - visibleHeight();
    cursor->gotoEnd();
    if (cursor->index() > 0)
        cursor->splitAndInsertEmptyParagraph();
    Q3TextCursor oldCursor2 = *cursor;

    if (f == Qt::PlainText) {
        cursor->insert(text, true);
        if (doc->useFormatCollection() && !doc->preProcessor() &&
            currentFormat != cursor->paragraph()->at( cursor->index() )->format()) {
            doc->setSelectionStart( Q3TextDocument::Temp, oldCursor2 );
            doc->setSelectionEnd( Q3TextDocument::Temp, *cursor );
            doc->setFormat( Q3TextDocument::Temp, currentFormat, Q3TextFormat::Format );
            doc->removeSelection( Q3TextDocument::Temp );
        }
    } else {
        cursor->paragraph()->setListItem(false);
        cursor->paragraph()->setListDepth(0);
        if (cursor->paragraph()->prev())
            cursor->paragraph()->prev()->invalidate(0); // vertical margins might have to change
        doc->setRichTextInternal(text);
    }
    formatMore();
    repaintChanged();
    if (atBottom)
        scrollToBottom();
    *cursor = oldc;
    if (!isReadOnly())
        cursorVisible = true;
    setModified();
    emit textChanged();
}

/*!
    \property Q3TextEdit::hasSelectedText
    \brief whether some text is selected in selection 0
*/

bool Q3TextEdit::hasSelectedText() const
{
#ifdef QT_TEXTEDIT_OPTIMIZATION
    if (d->optimMode)
        return optimHasSelection();
    else
#endif
        return doc->hasSelection(Q3TextDocument::Standard);
}

/*!
    \property Q3TextEdit::selectedText
    \brief The selected text (from selection 0) or an empty string if
    there is no currently selected text (in selection 0).

    The text is always returned as Qt::PlainText if the textFormat() is
    Qt::PlainText or Qt::AutoText, otherwise it is returned as HTML.

    \sa hasSelectedText
*/

QString Q3TextEdit::selectedText() const
{
#ifdef QT_TEXTEDIT_OPTIMIZATION
    if (d->optimMode)
        return optimSelectedText();
    else
#endif
        return doc->selectedText(Q3TextDocument::Standard, textFormat() == Qt::RichText);
}

bool Q3TextEdit::handleReadOnlyKeyEvent(QKeyEvent *e)
{
    switch(e->key()) {
    case Qt::Key_Down:
        setContentsPos(contentsX(), contentsY() + 10);
        break;
    case Qt::Key_Up:
        setContentsPos(contentsX(), contentsY() - 10);
        break;
    case Qt::Key_Left:
        setContentsPos(contentsX() - 10, contentsY());
        break;
    case Qt::Key_Right:
        setContentsPos(contentsX() + 10, contentsY());
        break;
    case Qt::Key_PageUp:
        setContentsPos(contentsX(), contentsY() - visibleHeight());
        break;
    case Qt::Key_PageDown:
        setContentsPos(contentsX(), contentsY() + visibleHeight());
        break;
    case Qt::Key_Home:
        setContentsPos(contentsX(), 0);
        break;
    case Qt::Key_End:
        setContentsPos(contentsX(), contentsHeight() - visibleHeight());
        break;
    case Qt::Key_F16: // Copy key on Sun keyboards
        copy();
        break;
#ifndef QT_NO_NETWORKPROTOCOL
    case Qt::Key_Return:
    case Qt::Key_Enter:
    case Qt::Key_Space: {
        if (!doc->focusIndicator.href.isEmpty()
                || !doc->focusIndicator.name.isEmpty()) {
            if (!doc->focusIndicator.href.isEmpty()) {
                QUrl u = QUrl(doc->context()).resolved(doc->focusIndicator.href);
                emitLinkClicked(u.toString(QUrl::None));
            }
            if (!doc->focusIndicator.name.isEmpty())
                if (Q3TextBrowser *browser = qobject_cast<Q3TextBrowser*>(this))
                    emit browser->anchorClicked(doc->focusIndicator.name, doc->focusIndicator.href);

#ifndef QT_NO_CURSOR
            viewport()->setCursor(isReadOnly() ? Qt::ArrowCursor : Qt::IBeamCursor);
#endif
        }
    } break;
#endif
    default:
        if (e->state() & Qt::ControlButton) {
            switch (e->key()) {
            case Qt::Key_C: case Qt::Key_F16: // Copy key on Sun keyboards
                copy();
                break;
#ifdef Q_WS_WIN
            case Qt::Key_Insert:
                copy();
                break;
            case Qt::Key_A:
                selectAll();
                break;
#endif
            }

        }
        return false;
    }
    return true;
}

/*!
    Returns the context of the text edit. The context is a path which
    the text edit's Q3MimeSourceFactory uses to resolve the locations
    of files and images.

    \sa text
*/

QString Q3TextEdit::context() const
{
    return doc->context();
}

/*!
    \property Q3TextEdit::documentTitle
    \brief the title of the document parsed from the text.

    For Qt::PlainText the title will be an empty string. For \c
    Qt::RichText the title will be the text between the \c{<title>} tags,
    if present, otherwise an empty string.
*/

QString Q3TextEdit::documentTitle() const
{
    return doc->attributes()[QLatin1String("title")];
}

void Q3TextEdit::makeParagVisible(Q3TextParagraph *p)
{
    setContentsPos(contentsX(), qMin(p->rect().y(), contentsHeight() - visibleHeight()));
}

/*!
    Scrolls the text edit to make the text at the anchor called \a
    name visible, if it can be found in the document. If the anchor
    isn't found no scrolling will occur. An anchor is defined using
    the HTML anchor tag, e.g. \c{<a name="target">}.
*/

void Q3TextEdit::scrollToAnchor(const QString& name)
{
    if (!isVisible()) {
        d->scrollToAnchor = name;
        return;
    }
    if (name.isEmpty())
        return;
    sync();
    Q3TextCursor cursor(doc);
    Q3TextParagraph* last = doc->lastParagraph();
    for (;;) {
        Q3TextStringChar* c = cursor.paragraph()->at(cursor.index());
        if(c->isAnchor()) {
            QString a = c->anchorName();
            if (a == name ||
                 (a.contains(QLatin1Char('#')) && a.split(QLatin1Char('#')).contains(name))) {
                setContentsPos(contentsX(), qMin(cursor.paragraph()->rect().top() + cursor.totalOffsetY(), contentsHeight() - visibleHeight()));
                break;
            }
        }
        if (cursor.paragraph() == last && cursor.atParagEnd() )
            break;
        cursor.gotoNextLetter();
    }
}

/*!
    Returns the text for the attribute \a attr (Qt::AnchorHref by
    default) if there is an anchor at position \a pos (in contents
    coordinates); otherwise returns an empty string.
*/

QString Q3TextEdit::anchorAt(const QPoint& pos, Qt::AnchorAttribute attr)
{
    Q3TextCursor c(doc);
    placeCursor(pos, &c, true);
    switch(attr) {
        case Qt::AnchorName:
            return c.paragraph()->at(c.index())->anchorName();
        case Qt::AnchorHref:
            return c.paragraph()->at(c.index())->anchorHref();
    }
    // incase the compiler is really dumb about determining if a function
    // returns something :)
    return QString();
}

void Q3TextEdit::documentWidthChanged(int w)
{
    resizeContents(qMax(visibleWidth(), w), contentsHeight());
}

/*! \internal

  This function does nothing
*/

void Q3TextEdit::updateStyles()
{
}

void Q3TextEdit::setDocument(Q3TextDocument *dc)
{
    if (dc == 0) {
        qWarning("Q3TextEdit::setDocument() called with null Q3TextDocument pointer");
        return;
    }
    if (dc == doc)
        return;
    doc = dc;
    delete cursor;
    cursor = new Q3TextCursor(doc);
    clearUndoRedo();
    undoRedoInfo.doc = doc;
    lastFormatted = 0;
}

#ifndef QT_NO_CLIPBOARD

/*!
    Pastes the text with format \a subtype from the clipboard into the
    text edit at the current cursor position. The \a subtype can be
    "plain" or "html".

    If there is no text with format \a subtype in the clipboard
    nothing happens.

    \sa paste() cut() Q3TextEdit::copy()
*/

void Q3TextEdit::pasteSubType(const QByteArray &subtype)
{
#ifndef QT_NO_MIMECLIPBOARD
    QMimeSource *m = QApplication::clipboard()->data(d->clipboard_mode);
    pasteSubType(subtype, m);
#endif
}

/*! \internal */

void Q3TextEdit::pasteSubType(const QByteArray& subtype, QMimeSource *m)
{
#ifndef QT_NO_MIME
    QByteArray st = subtype;

    if (subtype != "x-qrichtext")
        st.prepend("text/");
    else
        st.prepend("application/");
    if (!m)
        return;
    if (doc->hasSelection(Q3TextDocument::Standard))
        removeSelectedText();
    if (!Q3RichTextDrag::canDecode(m))
        return;
    QString t;
    if (!Q3RichTextDrag::decode(m, t, QString::fromLatin1(st), QString::fromLatin1(subtype)))
        return;
    if (st == "application/x-qrichtext") {
        int start;
        if ((start = t.indexOf(QLatin1String("<!--StartFragment-->"))) != -1) {
            start += 20;
            int end = t.indexOf(QLatin1String("<!--EndFragment-->"));
            Q3TextCursor oldC = *cursor;

            // during the setRichTextInternal() call the cursors
            // paragraph might get joined with the provious one, so
            // the cursors one would get deleted and oldC.paragraph()
            // would be a dnagling pointer. To avoid that try to go
            // one letter back and later go one forward again.
            oldC.gotoPreviousLetter();
            bool couldGoBack = oldC != *cursor;
            // first para might get deleted, so remember to reset it
            bool wasAtFirst = oldC.paragraph() == doc->firstParagraph();

            if (start < end)
                t = t.mid(start, end - start);
            else
                t = t.mid(start);
            lastFormatted = cursor->paragraph();
            if (lastFormatted->prev())
                lastFormatted = lastFormatted->prev();
            doc->setRichTextInternal(t, cursor);

            // the first para might have been deleted in
            // setRichTextInternal(). To be sure, reset it if
            // necessary.
            if (wasAtFirst) {
                int index = oldC.index();
                oldC.setParagraph(doc->firstParagraph());
                oldC.setIndex(index);
            }

            // if we went back one letter before (see last comment),
            // go one forward to point to the right position
            if (couldGoBack)
                oldC.gotoNextLetter();

            if (undoEnabled && !isReadOnly()) {
                doc->setSelectionStart(Q3TextDocument::Temp, oldC);
                doc->setSelectionEnd(Q3TextDocument::Temp, *cursor);

                checkUndoRedoInfo(UndoRedoInfo::Insert);
                if (!undoRedoInfo.valid()) {
                    undoRedoInfo.id = oldC.paragraph()->paragId();
                    undoRedoInfo.index = oldC.index();
                    undoRedoInfo.d->text.clear();
                }
                int oldLen = undoRedoInfo.d->text.length();
                if (!doc->preProcessor()) {
                    QString txt = doc->selectedText(Q3TextDocument::Temp);
                    undoRedoInfo.d->text += txt;
                    for (int i = 0; i < (int)txt.length(); ++i) {
                        if (txt[i] != QLatin1Char('\n') && oldC.paragraph()->at(oldC.index())->format()) {
                            oldC.paragraph()->at(oldC.index())->format()->addRef();
                            undoRedoInfo.d->text.
                                setFormat(oldLen + i, oldC.paragraph()->at(oldC.index())->format(), true);
                        }
                        oldC.gotoNextLetter();
                    }
                }
                undoRedoInfo.clear();
                removeSelection(Q3TextDocument::Temp);
            }

            formatMore();
            setModified();
            emit textChanged();
            repaintChanged();
            ensureCursorVisible();
            return;
        }
    } else {
#if defined(Q_OS_WIN32)
        // Need to convert CRLF to LF
        t.replace(QLatin1String("\r\n"), QLatin1String("\n"));
#elif defined(Q_OS_MAC)
        //need to convert CR to LF
        t.replace(QLatin1Char('\r'), QLatin1Char('\n'));
#endif
        QChar *uc = (QChar *)t.unicode();
        for (int i = 0; i < t.length(); i++) {
            if (uc[i] < QLatin1Char(' ') && uc[i] != QLatin1Char('\n') && uc[i] != QLatin1Char('\t'))
                uc[i] = QLatin1Char(' ');
        }
        if (!t.isEmpty())
            insert(t, false, true);
    }
#endif //QT_NO_MIME
}

#ifndef QT_NO_MIMECLIPBOARD
/*!
    Prompts the user to choose a type from a list of text types
    available, then copies text from the clipboard (if there is any)
    into the text edit at the current text cursor position. Any
    selected text (in selection 0) is first deleted.
*/
void Q3TextEdit::pasteSpecial(const QPoint& pt)
{
    QByteArray st = pickSpecial(QApplication::clipboard()->data(d->clipboard_mode),
                               true, pt);
    if (!st.isEmpty())
        pasteSubType(st);
}
#endif
#ifndef QT_NO_MIME
QByteArray Q3TextEdit::pickSpecial(QMimeSource* ms, bool always_ask, const QPoint& pt)
{
    if (ms)  {
#ifndef QT_NO_MENU
        QMenu popup(this);
        QString fmt;
        int n = 0;
        QHash<QString, bool> done;
        for (int i = 0; !(fmt = QLatin1String(ms->format(i))).isNull(); i++) {
            int semi = fmt.indexOf(QLatin1Char(';'));
            if (semi >= 0)
                fmt = fmt.left(semi);
            if (fmt.left(5) == QLatin1String("text/")) {
                fmt = fmt.mid(5);
                if (!done.contains(fmt)) {
                    done.insert(fmt,true);
                    popup.insertItem(fmt, i);
                    n++;
                }
            }
        }
        if (n) {
            QAction *action = (n == 1 && !always_ask)
                ? popup.actions().at(0)
                : popup.exec(pt);
            if (action)
                return action->text().toLatin1();
        }
#else
        QString fmt;
        for (int i = 0; !(fmt = ms->format(i)).isNull(); i++) {
            int semi = fmt.indexOf(';');
            if (semi >= 0)
                fmt = fmt.left(semi);
            if (fmt.left(5) == "text/") {
                fmt = fmt.mid(5);
                return fmt.latin1();
            }
        }
#endif
    }
    return QByteArray();
}
#endif // QT_NO_MIME
#endif // QT_NO_CLIPBOARD

/*!
    \enum Q3TextEdit::WordWrap

    This enum defines the Q3TextEdit's word wrap modes.

    \value NoWrap Do not wrap the text.

    \value WidgetWidth Wrap the text at the current width of the
    widget (this is the default). Wrapping is at whitespace by
    default; this can be changed with setWrapPolicy().

    \value FixedPixelWidth Wrap the text at a fixed number of pixels
    from the widget's left side. The number of pixels is set with
    wrapColumnOrWidth().

    \value FixedColumnWidth Wrap the text at a fixed number of
    character columns from the widget's left side. The number of
    characters is set with wrapColumnOrWidth(). This is useful if you
    need formatted text that can also be displayed gracefully on
    devices with monospaced fonts, for example a standard VT100
    terminal, where you might set wrapColumnOrWidth() to 80.

    \sa setWordWrap() wordWrap()
*/

/*!
    \property Q3TextEdit::wordWrap
    \brief the word wrap mode

    The default mode is \c WidgetWidth which causes words to be
    wrapped at the right edge of the text edit. Wrapping occurs at
    whitespace, keeping whole words intact. If you want wrapping to
    occur within words use setWrapPolicy(). If you set a wrap mode of
    \c FixedPixelWidth or \c FixedColumnWidth you should also call
    setWrapColumnOrWidth() with the width you want.

    \sa WordWrap, wrapColumnOrWidth, wrapPolicy,
*/

void Q3TextEdit::setWordWrap(WordWrap mode)
{
    if (wrapMode == mode)
        return;
    wrapMode = mode;
    switch (mode) {
    case NoWrap:
        document()->formatter()->setWrapEnabled(false);
        document()->formatter()->setWrapAtColumn(-1);
        doc->setWidth(visibleWidth());
        doc->setMinimumWidth(-1);
        doc->invalidate();
        updateContents();
        lastFormatted = doc->firstParagraph();
        interval = 0;
        formatMore();
        break;
    case WidgetWidth:
        document()->formatter()->setWrapEnabled(true);
        document()->formatter()->setWrapAtColumn(-1);
        doResize();
        break;
    case FixedPixelWidth:
        document()->formatter()->setWrapEnabled(true);
        document()->formatter()->setWrapAtColumn(-1);
        if (wrapWidth < 0)
            wrapWidth = 200;
        setWrapColumnOrWidth(wrapWidth);
        break;
    case FixedColumnWidth:
        if (wrapWidth < 0)
            wrapWidth = 80;
        document()->formatter()->setWrapEnabled(true);
        document()->formatter()->setWrapAtColumn(wrapWidth);
        setWrapColumnOrWidth(wrapWidth);
        break;
    }
#ifdef QT_TEXTEDIT_OPTIMIZATION
    checkOptimMode();
#endif
}

Q3TextEdit::WordWrap Q3TextEdit::wordWrap() const
{
    return wrapMode;
}

/*!
    \property Q3TextEdit::wrapColumnOrWidth
    \brief the position (in pixels or columns depending on the wrap mode) where text will be wrapped

    If the wrap mode is \c FixedPixelWidth, the value is the number of
    pixels from the left edge of the text edit at which text should be
    wrapped. If the wrap mode is \c FixedColumnWidth, the value is the
    column number (in character columns) from the left edge of the
    text edit at which text should be wrapped.

    \sa wordWrap
*/
void Q3TextEdit::setWrapColumnOrWidth(int value)
{
    wrapWidth = value;
    if (wrapMode == FixedColumnWidth) {
        document()->formatter()->setWrapAtColumn(wrapWidth);
        resizeContents(0, 0);
        doc->setWidth(visibleWidth());
        doc->setMinimumWidth(-1);
    } else if (wrapMode == FixedPixelWidth) {
        document()->formatter()->setWrapAtColumn(-1);
        resizeContents(wrapWidth, 0);
        doc->setWidth(wrapWidth);
        doc->setMinimumWidth(wrapWidth);
    } else {
        return;
    }
    doc->invalidate();
    updateContents();
    lastFormatted = doc->firstParagraph();
    interval = 0;
    formatMore();
}

int Q3TextEdit::wrapColumnOrWidth() const
{
    if (wrapMode == WidgetWidth)
        return visibleWidth();
    return wrapWidth;
}


/*!
    \enum Q3TextEdit::WrapPolicy

    This enum defines where text can be wrapped in word wrap mode.

    \value AtWhiteSpace Don't use this deprecated value (it is a
    synonym for \c AtWordBoundary which you should use instead).
    \value Anywhere  Break anywhere, including within words.
    \value AtWordBoundary Break lines at word boundaries, e.g. spaces or
    newlines
    \value AtWordOrDocumentBoundary Break lines at whitespace, e.g.
    spaces or newlines if possible. Break it anywhere otherwise.

    \sa setWrapPolicy()
*/

/*!
    \property Q3TextEdit::wrapPolicy
    \brief the word wrap policy, at whitespace or anywhere

    Defines where text can be wrapped when word wrap mode is not \c
    NoWrap. The choices are \c AtWordBoundary (the default), \c
    Anywhere and \c AtWordOrDocumentBoundary

    \sa wordWrap
*/

void Q3TextEdit::setWrapPolicy(WrapPolicy policy)
{
    if (wPolicy == policy)
        return;
    wPolicy = policy;
    Q3TextFormatter *formatter;
    if (policy == AtWordBoundary || policy == AtWordOrDocumentBoundary) {
        formatter = new Q3TextFormatterBreakWords;
        formatter->setAllowBreakInWords(policy == AtWordOrDocumentBoundary);
    } else {
        formatter = new Q3TextFormatterBreakInWords;
    }
    formatter->setWrapAtColumn(document()->formatter()->wrapAtColumn());
    formatter->setWrapEnabled(document()->formatter()->isWrapEnabled(0));
    document()->setFormatter(formatter);
    doc->invalidate();
    updateContents();
    lastFormatted = doc->firstParagraph();
    interval = 0;
    formatMore();
}

Q3TextEdit::WrapPolicy Q3TextEdit::wrapPolicy() const
{
    return wPolicy;
}

/*!
    Deletes all the text in the text edit.

    \sa cut() removeSelectedText() setText()
*/

void Q3TextEdit::clear()
{
#ifdef QT_TEXTEDIT_OPTIMIZATION
    if (d->optimMode) {
        optimSetText(QLatin1String(""));
    } else
#endif
    {
        // make clear undoable
        doc->selectAll(Q3TextDocument::Temp);
        removeSelectedText(Q3TextDocument::Temp);
        setContentsPos(0, 0);
        if (cursor->isValid())
            cursor->restoreState();
        doc->clear(true);
        delete cursor;
        cursor = new Q3TextCursor(doc);
        lastFormatted = 0;
    }
    updateContents();

    emit cursorPositionChanged(cursor);
    emit cursorPositionChanged(cursor->paragraph()->paragId(), cursor->index());
}

int Q3TextEdit::undoDepth() const
{
    return document()->undoDepth();
}

/*!
    \property Q3TextEdit::length
    \brief the number of characters in the text
*/

int Q3TextEdit::length() const
{
#ifdef QT_TEXTEDIT_OPTIMIZATION
    if (d->optimMode)
        return d->od->len;
    else
#endif
        return document()->length();
}

/*!
    \property Q3TextEdit::tabStopWidth
    \brief the tab stop width in pixels
*/

int Q3TextEdit::tabStopWidth() const
{
    return document()->tabStopWidth();
}

void Q3TextEdit::setUndoDepth(int d)
{
    document()->setUndoDepth(d);
}

void Q3TextEdit::setTabStopWidth(int ts)
{
    document()->setTabStops(ts);
    doc->invalidate();
    lastFormatted = doc->firstParagraph();
    interval = 0;
    formatMore();
    updateContents();
}

/*!
    \reimp
*/

QSize Q3TextEdit::sizeHint() const
{
    // cf. Q3ScrollView::sizeHint()
    ensurePolished();
    int f = 2 * frameWidth();
    int h = fontMetrics().height();
    QSize sz(f, f);
    return sz.expandedTo(QSize(12 * h, 8 * h));
}

void Q3TextEdit::clearUndoRedo()
{
    if (!undoEnabled)
        return;
    undoRedoInfo.clear();
    emit undoAvailable(doc->commands()->isUndoAvailable());
    emit redoAvailable(doc->commands()->isRedoAvailable());
}

/*!  \internal
  \warning In Qt 3.1 we will provide a cleaer API for the
  functionality which is provided by this function and in Qt 4.0 this
  function will go away.

  This function gets the format of the character at position \a
  index in paragraph \a para. Sets \a font to the character's font, \a
  color to the character's color and \a verticalAlignment to the
  character's vertical alignment.

  Returns false if \a para or \a index is out of range otherwise
  returns true.
*/

bool Q3TextEdit::getFormat(int para, int index, QFont *font, QColor *color, VerticalAlignment *verticalAlignment)
{
    if (!font || !color)
        return false;
    Q3TextParagraph *p = doc->paragAt(para);
    if (!p)
        return false;
    if (index < 0 || index >= p->length())
        return false;
    *font = p->at(index)->format()->font();
    *color = p->at(index)->format()->color();
    *verticalAlignment = (VerticalAlignment)p->at(index)->format()->vAlign();
    return true;
}

/*!  \internal
  \warning In Qt 3.1 we will provide a cleaer API for the
  functionality which is provided by this function and in Qt 4.0 this
  function will go away.

  This function gets the format of the paragraph \a para. Sets \a
  font to the paragraphs's font, \a color to the paragraph's color, \a
  verticalAlignment to the paragraph's vertical alignment, \a
  alignment to the paragraph's alignment, \a displayMode to the
  paragraph's display mode, \a listStyle to the paragraph's list style
  (if the display mode is Q3StyleSheetItem::DisplayListItem) and \a
  listDepth to the depth of the list (if the display mode is
  Q3StyleSheetItem::DisplayListItem).

  Returns false if \a para is out of range otherwise returns true.
*/

bool Q3TextEdit::getParagraphFormat(int para, QFont *font, QColor *color,
                                    VerticalAlignment *verticalAlignment, int *alignment,
                                    Q3StyleSheetItem::DisplayMode *displayMode,
                                    Q3StyleSheetItem::ListStyle *listStyle,
                                    int *listDepth)
{
    if (!font || !color || !alignment || !displayMode || !listStyle)
        return false;
    Q3TextParagraph *p = doc->paragAt(para);
    if (!p)
        return false;
    *font = p->at(0)->format()->font();
    *color = p->at(0)->format()->color();
    *verticalAlignment = (VerticalAlignment)p->at(0)->format()->vAlign();
    *alignment = p->alignment();
    *displayMode = p->isListItem() ? Q3StyleSheetItem::DisplayListItem : Q3StyleSheetItem::DisplayBlock;
    *listStyle = p->listStyle();
    *listDepth = p->listDepth();
    return true;
}



/*!
    This function is called to create a right mouse button popup menu
    at the document position \a pos. If you want to create a custom
    popup menu, reimplement this function and return the created popup
    menu. Ownership of the popup menu is transferred to the caller.

    \warning The QPopupMenu ID values 0-7 are reserved, and they map to the
    standard operations. When inserting items into your custom popup menu, be
    sure to specify ID values larger than 7.
*/

Q3PopupMenu *Q3TextEdit::createPopupMenu(const QPoint& pos)
{
    Q_UNUSED(pos)
#ifndef QT_NO_POPUPMENU
    Q3PopupMenu *popup = new Q3PopupMenu(this, "qt_edit_menu");
    if (!isReadOnly()) {
        d->id[IdUndo] = popup->insertItem(tr("&Undo") + ACCEL_KEY(Z));
        d->id[IdRedo] = popup->insertItem(tr("&Redo") + ACCEL_KEY(Y));
        popup->addSeparator();
    }
#ifndef QT_NO_CLIPBOARD
    if (!isReadOnly())
        d->id[IdCut] = popup->insertItem(tr("Cu&t") + ACCEL_KEY(X));
    d->id[IdCopy] = popup->insertItem(tr("&Copy") + ACCEL_KEY(C));
    if (!isReadOnly())
        d->id[IdPaste] = popup->insertItem(tr("&Paste") + ACCEL_KEY(V));
#endif
    if (!isReadOnly()) {
        d->id[IdClear] = popup->insertItem(tr("Clear"));
        popup->addSeparator();
    }
#if defined(Q_WS_X11)
    d->id[IdSelectAll] = popup->insertItem(tr("Select All"));
#else
    d->id[IdSelectAll] = popup->insertItem(tr("Select All") + ACCEL_KEY(A));
#endif
    popup->setItemEnabled(d->id[IdUndo], !isReadOnly() && doc->commands()->isUndoAvailable());
    popup->setItemEnabled(d->id[IdRedo], !isReadOnly() && doc->commands()->isRedoAvailable());
#ifndef QT_NO_CLIPBOARD
    popup->setItemEnabled(d->id[IdCut], !isReadOnly() && doc->hasSelection(Q3TextDocument::Standard, true));
#ifdef QT_TEXTEDIT_OPTIMIZATION
    popup->setItemEnabled(d->id[IdCopy], d->optimMode ? optimHasSelection() : doc->hasSelection(Q3TextDocument::Standard, true));
#else
    popup->setItemEnabled(d->id[IdCopy], doc->hasSelection(Q3TextDocument::Standard, true));
#endif
    popup->setItemEnabled(d->id[IdPaste], !isReadOnly() && !QApplication::clipboard()->text(d->clipboard_mode).isEmpty());
#endif
    const bool isEmptyDocument = (length() == 0);
    popup->setItemEnabled(d->id[IdClear], !isReadOnly() && !isEmptyDocument);
    popup->setItemEnabled(d->id[IdSelectAll], !isEmptyDocument);
    return popup;
#else
    return 0;
#endif
}

/*! \overload
    This function is called to create a right mouse button popup menu.
    If you want to create a custom popup menu, reimplement this function
    and return the created popup menu. Ownership of the popup menu is
    transferred to the caller.

    This function is only called if createPopupMenu(const QPoint &)
    returns 0.
*/

Q3PopupMenu *Q3TextEdit::createPopupMenu()
{
    return 0;
}

/*!
    \fn Q3TextEdit::zoomIn()

    \overload

    Zooms in on the text by making the base font size one point
    larger and recalculating all font sizes to be the new size. This
    does not change the size of any images.

    \sa zoomOut()
*/

/*!
    \fn Q3TextEdit::zoomOut()

    \overload

    Zooms out on the text by making the base font size one point
    smaller and recalculating all font sizes to be the new size. This
    does not change the size of any images.

    \sa zoomIn()
*/


/*!
    Zooms in on the text by making the base font size \a range
    points larger and recalculating all font sizes to be the new size.
    This does not change the size of any images.

    \sa zoomOut()
*/

void Q3TextEdit::zoomIn(int range)
{
    QFont f(Q3ScrollView::font());
    f.setPointSize(f.pointSize() + range);
    setFont(f);
}

/*!
    Zooms out on the text by making the base font size \a range points
    smaller and recalculating all font sizes to be the new size. This
    does not change the size of any images.

    \sa zoomIn()
*/

void Q3TextEdit::zoomOut(int range)
{
    QFont f(Q3ScrollView::font());
    f.setPointSize(qMax(1, f.pointSize() - range));
    setFont(f);
}

/*!
    Zooms the text by making the base font size \a size points and
    recalculating all font sizes to be the new size. This does not
    change the size of any images.
*/

void Q3TextEdit::zoomTo(int size)
{
    QFont f(Q3ScrollView::font());
    f.setPointSize(size);
    setFont(f);
}

/*!
   Q3TextEdit is optimized for large amounts text. One of its
   optimizations is to format only the visible text, formatting the rest
   on demand, e.g. as the user scrolls, so you don't usually need to
   call this function.

    In some situations you may want to force the whole text
    to be formatted. For example, if after calling setText(), you wanted
    to know the height of the document (using contentsHeight()), you
    would call this function first.
*/

void Q3TextEdit::sync()
{
#ifdef QT_TEXTEDIT_OPTIMIZATION
    if (d->optimMode) {
        QFontMetrics fm(Q3ScrollView::font());
        resizeContents(d->od->maxLineWidth + 4, d->od->numLines * fm.lineSpacing() + 1);
    } else
#endif
    {
        while (lastFormatted) {
            lastFormatted->format();
            lastFormatted = lastFormatted->next();
        }
        resizeContents(contentsWidth(), doc->height());
    }
    updateScrollBars();
}

/*!
    Sets the background color of selection number \a selNum to \a back
    and specifies whether the text of this selection should be
    inverted with \a invertText.

    This only works for \a selNum > 0. The default selection (\a
    selNum == 0) gets its attributes from the text edit's
    palette().
*/

void Q3TextEdit::setSelectionAttributes(int selNum, const QColor &back, bool invertText)
{
    if (selNum < 1)
        return;
    if (selNum > doc->numSelections())
        doc->addSelection(selNum);
    doc->setSelectionColor(selNum, back);
    if (invertText)
        doc->setSelectionTextColor(selNum, palette().color(QPalette::HighlightedText));
}

/*!
    \reimp
*/
void Q3TextEdit::changeEvent(QEvent *ev)
{
    if(ev->type() == QEvent::ActivationChange) {
        if (!isActiveWindow() && scrollTimer)
            scrollTimer->stop();
        if (!palette().isEqual(QPalette::Active, QPalette::Inactive))
            updateContents();
    }

#ifdef QT_TEXTEDIT_OPTIMIZATION
    if (d->optimMode && (ev->type() == QEvent::ApplicationFontChange
                         || ev->type() == QEvent::FontChange)) {
        QFont f = font();
        if (f.kerning())
            f.setKerning(false);

        setFont(f);

        Q3ScrollView::setFont(f);
        doc->setDefaultFormat(f, doc->formatCollection()->defaultFormat()->color());
        // recalculate the max string width
        QFontMetrics fm(f);
        int i, sw;
        d->od->maxLineWidth = 0;
        for (i = 0; i < d->od->numLines; i++) {
            sw = fm.width(d->od->lines[LOGOFFSET(i)]);
            if (d->od->maxLineWidth < sw)
                d->od->maxLineWidth = sw;
        }
        resizeContents(d->od->maxLineWidth + 4, d->od->numLines * fm.lineSpacing() + 1);
        return;
    }
#endif

    Q3ScrollView::changeEvent(ev);

    if (textFormat() == Qt::PlainText) {
        if (ev->type() == QEvent::ApplicationPaletteChange || ev->type() == QEvent::PaletteChange
            || ev->type() == QEvent::EnabledChange) {
            Q3TextFormat *f = doc->formatCollection()->defaultFormat();
            f->setColor(palette().text().color());
            updateContents();
        }
    }

    if (ev->type() == QEvent::ApplicationFontChange || ev->type() == QEvent::FontChange) {
        QFont f = font();
        if (f.kerning())
            f.setKerning(false);
        doc->setMinimumWidth(-1);
        doc->setDefaultFormat(f, doc->formatCollection()->defaultFormat()->color());
        lastFormatted = doc->firstParagraph();
        formatMore();
        repaintChanged();
    }
}

void Q3TextEdit::setReadOnly(bool b)
{
    if (readonly == b)
        return;
    readonly = b;
    d->cursorBlinkActive = !b;
#ifndef QT_NO_CURSOR
    if (readonly)
        viewport()->setCursor(Qt::ArrowCursor);
    else
        viewport()->setCursor(Qt::IBeamCursor);
    setInputMethodEnabled(!readonly);
#endif
#ifdef QT_TEXTEDIT_OPTIMIZATION
    checkOptimMode();
#endif
}

/*!
    Scrolls to the bottom of the document and does formatting if
    required.
*/

void Q3TextEdit::scrollToBottom()
{
    sync();
    setContentsPos(contentsX(), contentsHeight() - visibleHeight());
}

/*!
    Returns the rectangle of the paragraph \a para in contents
    coordinates, or an invalid rectangle if \a para is out of range.
*/

QRect Q3TextEdit::paragraphRect(int para) const
{
    Q3TextEdit *that = (Q3TextEdit *)this;
    that->sync();
    Q3TextParagraph *p = doc->paragAt(para);
    if (!p)
        return QRect(-1, -1, -1, -1);
    return p->rect();
}

/*!
    Returns the paragraph which is at position \a pos (in contents
    coordinates).
*/

int Q3TextEdit::paragraphAt(const QPoint &pos) const
{
#ifdef QT_TEXTEDIT_OPTIMIZATION
    if (d->optimMode) {
        QFontMetrics fm(Q3ScrollView::font());
        int parag = pos.y() / fm.lineSpacing();
        if (parag <= d->od->numLines)
            return parag;
        else
            return 0;
    }
#endif
    Q3TextCursor c(doc);
    c.place(pos, doc->firstParagraph());
    if (c.paragraph())
        return c.paragraph()->paragId();
    return -1; // should never happen..
}

/*!
    Returns the index of the character (relative to its paragraph) at
    position \a pos (in contents coordinates). If \a para is not 0,
    \c{*}\a{para} is set to the character's paragraph.
*/

int Q3TextEdit::charAt(const QPoint &pos, int *para) const
{
#ifdef QT_TEXTEDIT_OPTIMIZATION
    if (d->optimMode) {
        int par = paragraphAt(pos);
        if (para)
            *para = par;
        return optimCharIndex(d->od->lines[LOGOFFSET(par)], pos.x());
    }
#endif
    Q3TextCursor c(doc);
    c.place(pos, doc->firstParagraph());
    if (c.paragraph()) {
        if (para)
            *para = c.paragraph()->paragId();
        return c.index();
    }
    return -1; // should never happen..
}

/*!
    Sets the background color of the paragraph \a para to \a bg.
*/

void Q3TextEdit::setParagraphBackgroundColor(int para, const QColor &bg)
{
    Q3TextParagraph *p = doc->paragAt(para);
    if (!p)
        return;
    p->setBackgroundColor(bg);
    repaintChanged();
}

/*!
    Clears the background color of the paragraph \a para, so that the
    default color is used again.
*/

void Q3TextEdit::clearParagraphBackground(int para)
{
    Q3TextParagraph *p = doc->paragAt(para);
    if (!p)
        return;
    p->clearBackgroundColor();
    repaintChanged();
}

/*!
    Returns the background color of the paragraph \a para or an
    invalid color if \a para is out of range or the paragraph has no
    background set
*/

QColor Q3TextEdit::paragraphBackgroundColor(int para) const
{
    Q3TextParagraph *p = doc->paragAt(para);
    if (!p)
        return QColor();
    QColor *c = p->backgroundColor();
    if (c)
        return *c;
    return QColor();
}

/*!
    \property Q3TextEdit::undoRedoEnabled
    \brief whether undo/redo is enabled

    When changing this property, the undo/redo history is cleared.

    The default is true.
*/

void Q3TextEdit::setUndoRedoEnabled(bool b)
{
    undoRedoInfo.clear();
    doc->commands()->clear();

    undoEnabled = b;
}

bool Q3TextEdit::isUndoRedoEnabled() const
{
    return undoEnabled;
}

/*!
    Returns true if undo is available; otherwise returns false.
*/

bool Q3TextEdit::isUndoAvailable() const
{
    return undoEnabled && (doc->commands()->isUndoAvailable() || undoRedoInfo.valid());
}

/*!
    Returns true if redo is available; otherwise returns false.
*/

bool Q3TextEdit::isRedoAvailable() const
{
    return undoEnabled && doc->commands()->isRedoAvailable();
}

void Q3TextEdit::ensureFormatted(Q3TextParagraph *p)
{
    while (!p->isValid()) {
        if (!lastFormatted)
            return;
        formatMore();
    }
}

/*! \internal */
void Q3TextEdit::updateCursor(const QPoint & pos)
{
    if (isReadOnly() && linksEnabled()) {
        Q3TextCursor c = *cursor;
        placeCursor(pos, &c, true);

#ifndef QT_NO_NETWORKPROTOCOL
        bool insideParagRect = true;
        if (c.paragraph() == doc->lastParagraph()
            && c.paragraph()->rect().y() + c.paragraph()->rect().height() < pos.y())
            insideParagRect = false;
        if (insideParagRect && c.paragraph() && c.paragraph()->at(c.index()) &&
            c.paragraph()->at(c.index())->isAnchor()) {
            if (!c.paragraph()->at(c.index())->anchorHref().isEmpty()
                    && c.index() < c.paragraph()->length() - 1)
                onLink = c.paragraph()->at(c.index())->anchorHref();
            else
                onLink.clear();

            if (!c.paragraph()->at(c.index())->anchorName().isEmpty()
                    && c.index() < c.paragraph()->length() - 1)
                d->onName = c.paragraph()->at(c.index())->anchorName();
            else
                d->onName.clear();

            if (!c.paragraph()->at(c.index())->anchorHref().isEmpty()) {
#ifndef QT_NO_CURSOR
                viewport()->setCursor(onLink.isEmpty() ? Qt::ArrowCursor : Qt::PointingHandCursor);
#endif
                QUrl u = QUrl(doc->context()).resolved(onLink);
                emitHighlighted(u.toString(QUrl::None));
            }
        } else {
#ifndef QT_NO_CURSOR
            viewport()->setCursor(isReadOnly() ? Qt::ArrowCursor : Qt::IBeamCursor);
#endif
            onLink.clear();
            emitHighlighted(QString());
        }
#endif
    }
}

/*!
  Places the cursor \a c at the character which is closest to position
  \a pos (in contents coordinates). If \a c is 0, the default text
  cursor is used.

  \sa setCursorPosition()
*/
void Q3TextEdit::placeCursor(const QPoint &pos, Q3TextCursor *c)
{
    placeCursor(pos, c, false);
}

/*! \internal */
void Q3TextEdit::clipboardChanged()
{
#ifndef QT_NO_CLIPBOARD
    // don't listen to selection changes
    disconnect(QApplication::clipboard(), SIGNAL(selectionChanged()), this, 0);
#endif
    selectAll(false);
}

/*! \property Q3TextEdit::tabChangesFocus
  \brief whether TAB changes focus or is accepted as input

  In some occasions text edits should not allow the user to input
  tabulators or change indentation using the TAB key, as this breaks
  the focus chain. The default is false.

*/

void Q3TextEdit::setTabChangesFocus(bool b)
{
    d->tabChangesFocus = b;
}

bool Q3TextEdit::tabChangesFocus() const
{
    return d->tabChangesFocus;
}

#ifdef QT_TEXTEDIT_OPTIMIZATION
/* Implementation of optimized Qt::LogText mode follows */

static void qSwap(int * a, int * b)
{
    if (!a || !b)
        return;
    int tmp = *a;
    *a = *b;
    *b = tmp;
}

/*! \internal */
bool Q3TextEdit::checkOptimMode()
{
    bool oldMode = d->optimMode;
    if (textFormat() == Qt::LogText) {
        d->optimMode = true;
        setReadOnly(true);
    } else {
        d->optimMode = false;
    }

    // when changing mode - try to keep selections and text
    if (oldMode != d->optimMode) {
        if (d->optimMode) {
            d->od = new Q3TextEditOptimPrivate;
            connect(scrollTimer, SIGNAL(timeout()), this, SLOT(optimDoAutoScroll()));
            disconnect(doc, SIGNAL(minimumWidthChanged(int)), this, SLOT(documentWidthChanged(int)));
            disconnect(scrollTimer, SIGNAL(timeout()), this, SLOT(autoScrollTimerDone()));
            disconnect(formatTimer, SIGNAL(timeout()), this, SLOT(formatMore()));
            optimSetText(doc->originalText());
            doc->clear(true);
            delete cursor;
            cursor = new Q3TextCursor(doc);
        } else {
            disconnect(scrollTimer, SIGNAL(timeout()), this, SLOT(optimDoAutoScroll()));
            connect(doc, SIGNAL(minimumWidthChanged(int)), this, SLOT(documentWidthChanged(int)));
            connect(scrollTimer, SIGNAL(timeout()), this, SLOT(autoScrollTimerDone()));
            connect(formatTimer, SIGNAL(timeout()), this, SLOT(formatMore()));
            setText(optimText());
            delete d->od;
            d->od = 0;
        }
    }
    return d->optimMode;
}

/*! \internal */
QString Q3TextEdit::optimText() const
{
    QString str, tmp;

    if (d->od->len == 0)
        return str;

    // concatenate all strings
    int i;
    int offset;
    QMap<int,Q3TextEditOptimPrivate::Tag *>::ConstIterator it;
    Q3TextEditOptimPrivate::Tag * ftag = 0;
    for (i = 0; i < d->od->numLines; i++) {
        if (d->od->lines[LOGOFFSET(i)].isEmpty()) { // CR lines are empty
            str += QLatin1Char('\n');
        } else {
            tmp = d->od->lines[LOGOFFSET(i)] + QLatin1Char('\n');
            // inject the tags for this line
            if ((it = d->od->tagIndex.constFind(LOGOFFSET(i))) != d->od->tagIndex.constEnd())
                ftag = it.value();
            offset = 0;
            while (ftag && ftag->line == i) {
                tmp.insert(ftag->index + offset, QLatin1Char('<') + ftag->tag + QLatin1Char('>'));
                offset += ftag->tag.length() + 2; // 2 -> the '<' and '>' chars
                ftag = ftag->next;
            }
            str += tmp;
        }
    }
    return str;
}

/*! \internal */
void Q3TextEdit::optimSetText(const QString &str)
{
    optimRemoveSelection();
// this is just too slow - but may have to go in due to compatibility reasons
//     if (str == optimText())
//         return;
    d->od->numLines = 0;
    d->od->lines.clear();
    d->od->maxLineWidth = 0;
    d->od->len = 0;
    d->od->clearTags();
    QFontMetrics fm(Q3ScrollView::font());
    if (!(str.isEmpty() || str.isNull() || d->maxLogLines == 0)) {
        QStringList strl = str.split(QLatin1Char('\n'));
        int lWidth = 0;
        for (QStringList::Iterator it = strl.begin(); it != strl.end(); ++it) {
            optimParseTags(&*it);
            optimCheckLimit(*it);
            lWidth = fm.width(*it);
            if (lWidth > d->od->maxLineWidth)
                d->od->maxLineWidth = lWidth;
        }
    }
    resizeContents(d->od->maxLineWidth + 4, d->od->numLines * fm.lineSpacing() + 1);
    repaintContents();
    emit textChanged();
}

/*! \internal

  Append \a tag to the tag list.
*/
Q3TextEditOptimPrivate::Tag * Q3TextEdit::optimAppendTag(int index, const QString & tag)
{
    Q3TextEditOptimPrivate::Tag * t = new Q3TextEditOptimPrivate::Tag, * tmp;

    if (d->od->tags == 0)
        d->od->tags = t;
    t->bold = t->italic = t->underline = false;
    t->line  = d->od->numLines;
    t->index = index;
    t->tag   = tag;
    t->leftTag = 0;
    t->parent  = 0;
    t->prev = d->od->lastTag;
    if (d->od->lastTag)
        d->od->lastTag->next = t;
    t->next = 0;
    d->od->lastTag = t;
    tmp = d->od->tagIndex[LOGOFFSET(t->line)];
    if (!tmp || (tmp && tmp->index > t->index)) {
        d->od->tagIndex.insert(LOGOFFSET(t->line), t);
    }
    return t;
}

/*! \internal

  Insert \a tag in the tag - according to line and index numbers
*/
Q3TextEditOptimPrivate::Tag *Q3TextEdit::optimInsertTag(int line, int index, const QString &tag)
{
    Q3TextEditOptimPrivate::Tag *t = new Q3TextEditOptimPrivate::Tag, *tmp;

    if (d->od->tags == 0)
        d->od->tags = t;
    t->bold = t->italic = t->underline = false;
    t->line  = line;
    t->index = index;
    t->tag   = tag;
    t->leftTag = 0;
    t->parent  = 0;
    t->next = 0;
    t->prev = 0;

    // find insertion pt. in tag struct.
    QMap<int,Q3TextEditOptimPrivate::Tag *>::ConstIterator it;
    if ((it = d->od->tagIndex.constFind(LOGOFFSET(line))) != d->od->tagIndex.constEnd()) {
        tmp = *it;
        if (tmp->index >= index) { // the existing tag may be placed AFTER the one we want to insert
            tmp = tmp->prev;
        } else {
            while (tmp && tmp->next && tmp->next->line == line && tmp->next->index <= index)
                tmp = tmp->next;
        }
    } else {
        tmp = d->od->tags;
        while (tmp && tmp->next && tmp->next->line < line)
            tmp = tmp->next;
        if (tmp == d->od->tags)
            tmp = 0;
    }

    t->prev = tmp;
    t->next = tmp ? tmp->next : 0;
    if (t->next)
        t->next->prev = t;
    if (tmp)
        tmp->next = t;

    tmp = d->od->tagIndex[LOGOFFSET(t->line)];
    if (!tmp || (tmp && tmp->index >= t->index)) {
        d->od->tagIndex.insert(LOGOFFSET(t->line), t);
    }
    return t;
}

/*! \internal

  Find tags in \a line, remove them from \a line and put them in a
  structure.

  A tag is delimited by '<' and '>'. The characters '<', '>' and '&'
  are escaped by using '&lt;', '&gt;' and '&amp;'. Left-tags marks
  the starting point for formatting, while right-tags mark the ending
  point. A right-tag is the same as a left-tag, but with a '/'
  appearing before the tag keyword.  E.g a valid left-tag: <b>, and
  a valid right-tag: </b>.  Tags can be nested, but they have to be
  closed in the same order as they are opened. E.g:
  <font color=red><font color=blue>blue</font>red</font> - is valid, while:
  <font color=red><b>bold red</font> just bold</b> - is invalid since the font tag is
  closed before the bold tag. Note that a tag does not have to be
  closed: '<font color=blue>Lots of text - and then some..'  is perfectly valid for
  setting all text appearing after the tag to blue.  A tag can be used
  to change the color of a piece of text, or set one of the following
  formatting attributes: bold, italic and underline.  These attributes
  are set using the <b>, <i> and <u> tags.  Example of valid tags:
  <font color=red>, </font>, <b>, <u>, <i>, </i>.
  Example of valid text:
  This is some <font color=red>red text</font>, while this is some <font color=green>green
  text</font>. <font color=blue><font color=yellow>This is yellow</font>, while this is
  blue.</font>

  Note that only the color attribute of the HTML font tag is supported.

  Limitations:
  1. A tag cannot span several lines.
  2. Very limited error checking - mismatching left/right-tags is the
  only thing that is detected.

*/
void Q3TextEdit::optimParseTags(QString * line, int lineNo, int indexOffset)
{
    int len = line->length();
    int i, startIndex = -1, endIndex = -1, escIndex = -1;
    int state = 0; // 0 = outside tag, 1 = inside tag
    bool tagOpen, tagClose;
    int bold = 0, italic = 0, underline = 0;
    QString tagStr;
    QStack<Q3TextEditOptimPrivate::Tag *> tagStack;

    for (i = 0; i < len; i++) {
        tagOpen = (*line)[i] == QLatin1Char('<');
        tagClose = (*line)[i] == QLatin1Char('>');

        // handle '&lt;' and '&gt;' and '&amp;'
        if ((*line)[i] == QLatin1Char('&')) {
            escIndex = i;
            continue;
        } else if (escIndex != -1 && (*line)[i] == QLatin1Char(';')) {
            QString esc = line->mid(escIndex, i - escIndex + 1);
            QString c;
            if (esc == QLatin1String("&lt;"))
                c = QLatin1Char('<');
            else if (esc == QLatin1String("&gt;"))
                c = QLatin1Char('>');
            else if (esc == QLatin1String("&amp;"))
                c = QLatin1Char('&');
            line->replace(escIndex, i - escIndex + 1, c);
            len = line->length();
            i -= i-escIndex;
            escIndex = -1;
            continue;
        }

        if (state == 0 && tagOpen) {
            state = 1;
            startIndex = i;
            continue;
        }
        if (state == 1 && tagClose) {
            state = 0;
            endIndex = i;
            if (!tagStr.isEmpty()) {
                Q3TextEditOptimPrivate::Tag * tag, * cur, * tmp;
                bool format = true;

                if (tagStr == QLatin1String("b"))
                    bold++;
                else if (tagStr == QLatin1String("/b"))
                    bold--;
                else if (tagStr == QLatin1String("i"))
                    italic++;
                else if (tagStr == QLatin1String("/i"))
                    italic--;
                else if (tagStr == QLatin1String("u"))
                    underline++;
                else if (tagStr == QLatin1String("/u"))
                    underline--;
                else
                    format = false;
                if (lineNo > -1)
                    tag = optimInsertTag(lineNo, startIndex + indexOffset, tagStr);
                else
                    tag = optimAppendTag(startIndex, tagStr);
                // everything that is not a b, u or i tag is considered
                // to be a color tag.
                tag->type = format ? Q3TextEditOptimPrivate::Format
                            : Q3TextEditOptimPrivate::Color;
                if (tagStr[0] == QLatin1Char('/')) {
                    // this is a right-tag - search for the left-tag
                    // and possible parent tag
                    cur = tag->prev;
                    if (!cur) {
                        qWarning("Q3TextEdit::optimParseTags: no left-tag for '<%s>' in line %d.",
                                  tag->tag.latin1(), tag->line + 1);
                        return; // something is wrong - give up
                    }
                    while (cur) {
                        if (cur->leftTag) { // push right-tags encountered
                            tagStack.push(cur);
                        } else {
                            tmp = tagStack.isEmpty() ? 0 : tagStack.pop();
                            if (!tmp) {
                                if (((QLatin1Char('/') + cur->tag) == tag->tag) ||
                                     (tag->tag == QLatin1String("/font") && cur->tag.left(4) == QLatin1String("font"))) {
                                    // set up the left and parent of this tag
                                    tag->leftTag = cur;
                                    tmp = cur->prev;
                                    if (tmp && tmp->parent) {
                                        tag->parent = tmp->parent;
                                    } else if (tmp && !tmp->leftTag) {
                                        tag->parent = tmp;
                                    }
                                    break;
                                } else if (!cur->leftTag) {
                                    qWarning("Q3TextEdit::optimParseTags: mismatching %s-tag for '<%s>' in line %d.",
                                              qPrintable(QString(cur->tag[0] == QLatin1Char('/') ? QLatin1String("left") : QLatin1String("right"))),
                                              cur->tag.latin1(), cur->line + 1);
                                    return; // something is amiss - give up
                                }
                            }
                        }
                        cur = cur->prev;
                    }
                } else {
                    tag->bold = bold > 0;
                    tag->italic = italic > 0;
                    tag->underline = underline > 0;
                    tmp = tag->prev;
                    while (tmp && tmp->leftTag) {
                        tmp = tmp->leftTag->parent;
                    }
                    if (tmp) {
                        tag->bold |= tmp->bold;
                        tag->italic |= tmp->italic;
                        tag->underline |= tmp->underline;
                    }
                }
            }
            if (startIndex != -1) {
                int l = (endIndex == -1) ?
                        line->length() - startIndex : endIndex - startIndex;
                line->remove(startIndex, l+1);
                len = line->length();
                i -= l+1;
            }
            tagStr = QLatin1String("");
            continue;
        }

        if (state == 1) {
            tagStr += (*line)[i];
        }
    }
}

// calculate the width of a string in pixels inc. tabs
static int qStrWidth(const QString& str, int tabWidth, const QFontMetrics& fm)
{
    int tabs = str.count(QLatin1Char('\t'));

    if (!tabs)
        return fm.width(str);

    int newIdx = 0;
    int lastIdx = 0;
    int strWidth = 0;
    int tn;
    for (tn = 1; tn <= tabs; ++tn) {
        newIdx = str.indexOf(QLatin1Char('\t'), newIdx);
        strWidth += fm.width(str.mid(lastIdx, newIdx - lastIdx));
        if (strWidth >= tn * tabWidth) {
            int u = tn;
            while (strWidth >= u * tabWidth)
                ++u;
            strWidth = u * tabWidth;
        } else {
            strWidth = tn * tabWidth;
        }
        lastIdx = ++newIdx;
    }
    if ((int)str.length() > newIdx)
        strWidth += fm.width(str.mid(newIdx));
    return strWidth;
}

bool Q3TextEdit::optimHasBoldMetrics(int line)
{
    Q3TextEditOptimPrivate::Tag *t;
    QMap<int,Q3TextEditOptimPrivate::Tag *>::ConstIterator it;
    if ((it = d->od->tagIndex.constFind(line)) != d->od->tagIndex.constEnd()) {
        t = *it;
        while (t && t->line == line) {
            if (t->bold)
                return true;
            t = t->next;
        }
    } else if ((t = optimPreviousLeftTag(line)) && t->bold) {
        return true;
    }
    return false;
}

/*! \internal

  Append \a str to the current text buffer. Parses each line to find
  formatting tags.
*/
void Q3TextEdit::optimAppend(const QString &str)
{
    if (str.isEmpty() || str.isNull() || d->maxLogLines == 0)
        return;

    QStringList strl = str.split(QLatin1Char('\n'));
    QStringList::Iterator it = strl.begin();

    QFontMetrics fm(Q3ScrollView::font());
    int lWidth = 0;
    for (; it != strl.end(); ++it) {
        optimParseTags(&*it);
        optimCheckLimit(*it);
        if (optimHasBoldMetrics(d->od->numLines-1)) {
            QFont fnt = Q3ScrollView::font();
            fnt.setBold(true);
            fm = QFontMetrics(fnt);
        }
        lWidth = qStrWidth(*it, tabStopWidth(), fm) + 4;
        if (lWidth > d->od->maxLineWidth)
            d->od->maxLineWidth = lWidth;
    }
    bool scrollToEnd = contentsY() >= contentsHeight() - visibleHeight();
    resizeContents(d->od->maxLineWidth + 4, d->od->numLines * fm.lineSpacing() + 1);
    if (scrollToEnd) {
        updateScrollBars();
        ensureVisible(contentsX(), contentsHeight(), 0, 0);
    }
    // when a max log size is set, the text may not be redrawn because
    // the size of the viewport may not have changed
    if (d->maxLogLines > -1)
        viewport()->update();
    emit textChanged();
}

static void qStripTags(QString *line)
{
    int len = line->length();
    int i, startIndex = -1, endIndex = -1, escIndex = -1;
    int state = 0; // 0 = outside tag, 1 = inside tag
    bool tagOpen, tagClose;

    for (i = 0; i < len; i++) {
        tagOpen = (*line)[i] == QLatin1Char('<');
        tagClose = (*line)[i] == QLatin1Char('>');

        // handle '&lt;' and '&gt;' and '&amp;'
        if ((*line)[i] == QLatin1Char('&')) {
            escIndex = i;
            continue;
        } else if (escIndex != -1 && (*line)[i] == QLatin1Char(';')) {
            QString esc = line->mid(escIndex, i - escIndex + 1);
            QString c;
            if (esc == QLatin1String("&lt;"))
                c = QLatin1Char('<');
            else if (esc == QLatin1String("&gt;"))
                c = QLatin1Char('>');
            else if (esc == QLatin1String("&amp;"))
                c = QLatin1Char('&');
            line->replace(escIndex, i - escIndex + 1, c);
            len = line->length();
            i -= i-escIndex;
            escIndex = -1;
            continue;
        }

        if (state == 0 && tagOpen) {
            state = 1;
            startIndex = i;
            continue;
        }
        if (state == 1 && tagClose) {
            state = 0;
            endIndex = i;
            if (startIndex != -1) {
                int l = (endIndex == -1) ?
                        line->length() - startIndex : endIndex - startIndex;
                line->remove(startIndex, l+1);
                len = line->length();
                i -= l+1;
            }
            continue;
        }
    }
}

/*! \internal

    Inserts the text into \a line at index \a index.
*/

void Q3TextEdit::optimInsert(const QString& text, int line, int index)
{
    if (text.isEmpty() || d->maxLogLines == 0)
        return;
    if (line < 0)
        line = 0;
    if (line > d->od->numLines-1)
        line = d->od->numLines-1;
    if (index < 0)
        index = 0;
    if (index > d->od->lines[line].length())
        index = d->od->lines[line].length();

    QStringList strl = text.split(QLatin1Char('\n'));
    int numNewLines = strl.count() - 1;
    Q3TextEditOptimPrivate::Tag *tag = 0;
    QMap<int,Q3TextEditOptimPrivate::Tag *>::ConstIterator ii;
    int x;

    if (numNewLines == 0) {
        // Case 1. Fast single line case - just inject it!
        QString stripped = text;
        qStripTags(&stripped);
        d->od->lines[LOGOFFSET(line)].insert(index, stripped);
        // move the tag indices following the insertion pt.
        if ((ii = d->od->tagIndex.constFind(LOGOFFSET(line))) != d->od->tagIndex.constEnd()) {
            tag = *ii;
            while (tag && (LOGOFFSET(tag->line) == line && tag->index < index))
                tag = tag->next;
            while (tag && (LOGOFFSET(tag->line) == line)) {
                tag->index += stripped.length();
                tag = tag->next;
            }
        }
        stripped = text;
        optimParseTags(&stripped, line, index);
    } else if (numNewLines > 0) {
        // Case 2. We have at least 1 newline char - split at
        // insertion pt. and make room for new lines - complex and slow!
        QString left = d->od->lines[LOGOFFSET(line)].left(index);
        QString right = d->od->lines[LOGOFFSET(line)].mid(index);

        // rearrange lines for insertion
        for (x = d->od->numLines - 1; x > line; x--)
            d->od->lines[x + numNewLines] = d->od->lines[x];
        d->od->numLines += numNewLines;

        // fix the tag index and the tag line/index numbers - this
        // might take a while..
        for (x = line; x < d->od->numLines; x++) {
            if ((ii = d->od->tagIndex.constFind(LOGOFFSET(line))) != d->od->tagIndex.constEnd()) {
                tag = ii.value();
                if (LOGOFFSET(tag->line) == line)
                    while (tag && (LOGOFFSET(tag->line) == line && tag->index < index))
                        tag = tag->next;
            }
        }

        // relabel affected tags with new line numbers and new index
        // positions
        while (tag) {
            if (LOGOFFSET(tag->line) == line)
                tag->index -= index;
            tag->line += numNewLines;
            tag = tag->next;
        }

        // generate a new tag index
        d->od->tagIndex.clear();
        tag = d->od->tags;
        while (tag) {
            if (!((ii = d->od->tagIndex.constFind(LOGOFFSET(tag->line))) != d->od->tagIndex.constEnd()))
                d->od->tagIndex[LOGOFFSET(tag->line)] = tag;
            tag = tag->next;
        }

        // update the tag indices on the spliced line - needs to be done before new tags are added
        QString stripped = strl[strl.count() - 1];
        qStripTags(&stripped);
        if ((ii = d->od->tagIndex.constFind(LOGOFFSET(line + numNewLines))) != d->od->tagIndex.constEnd()) {
            tag = *ii;
            while (tag && (LOGOFFSET(tag->line) == line + numNewLines)) {
                tag->index += stripped.length();
                tag = tag->next;
            }
        }

        // inject the new lines
        QStringList::Iterator it = strl.begin();
        x = line;
        int idx;
        for (; it != strl.end(); ++it) {
            stripped = *it;
            qStripTags(&stripped);
            if (x == line) {
                stripped = left + stripped;
                idx = index;
            } else {
                idx = 0;
            }
            d->od->lines[LOGOFFSET(x)] = stripped;
            optimParseTags(&*it, x++, idx);
        }
        d->od->lines[LOGOFFSET(x - 1)] += right;
    }
    // recalculate the pixel width of the longest injected line -
    QFontMetrics fm(Q3ScrollView::font());
    int lWidth = 0;
    for (x = line; x < line + numNewLines; x++) {
        if (optimHasBoldMetrics(x)) {
            QFont fnt = Q3ScrollView::font();
            fnt.setBold(true);
            fm = QFontMetrics(fnt);
        }
        lWidth = fm.width(d->od->lines[x]) + 4;
        if (lWidth > d->od->maxLineWidth)
            d->od->maxLineWidth = lWidth;
    }
    resizeContents(d->od->maxLineWidth + 4, d->od->numLines * fm.lineSpacing() + 1);
    repaintContents();
    emit textChanged();
}


/*! \internal

  Returns the first open left-tag appearing before line \a line.
 */
Q3TextEditOptimPrivate::Tag * Q3TextEdit::optimPreviousLeftTag(int line)
{
    Q3TextEditOptimPrivate::Tag * ftag = 0;
    QMap<int,Q3TextEditOptimPrivate::Tag *>::ConstIterator it;
    if ((it = d->od->tagIndex.constFind(LOGOFFSET(line))) != d->od->tagIndex.constEnd())
        ftag = it.value();
    if (!ftag) {
        // start searching for an open tag
        ftag = d->od->tags;
        while (ftag) {
            if (ftag->line > line || ftag->next == 0) {
                if (ftag->line > line)
                    ftag = ftag->prev;
                break;
            }
            ftag = ftag->next;
        }
    } else {
        ftag = ftag->prev;
    }

    if (ftag) {
        if (ftag && ftag->parent) // use the open parent tag
            ftag = ftag->parent;
        else if (ftag && ftag->leftTag) // this is a right-tag with no parent
            ftag = 0;
    }
    return ftag;
}

/*! \internal

  Set the format for the string starting at index \a start and ending
  at \a end according to \a tag. If \a tag is a Format tag, find the
  first open color tag appearing before \a tag and use that tag to
  color the string.
*/
void Q3TextEdit::optimSetTextFormat(Q3TextDocument * td, Q3TextCursor * cur,
                                    Q3TextFormat * f, int start, int end,
                                    Q3TextEditOptimPrivate::Tag * tag)
{
    int formatFlags = Q3TextFormat::Bold | Q3TextFormat::Italic |
                      Q3TextFormat::Underline;
    cur->setIndex(start);
    td->setSelectionStart(0, *cur);
    cur->setIndex(end);
    td->setSelectionEnd(0, *cur);
    Q3StyleSheetItem * ssItem = styleSheet()->item(tag->tag);
    if (!ssItem || tag->type == Q3TextEditOptimPrivate::Format) {
        f->setBold(tag->bold);
        f->setItalic(tag->italic);
        f->setUnderline(tag->underline);
        if (tag->type == Q3TextEditOptimPrivate::Format) {
            // check to see if there are any open color tags prior to
            // this format tag
            tag = tag->prev;
            while (tag && (tag->type == Q3TextEditOptimPrivate::Format ||
                            tag->leftTag)) {
                tag = tag->leftTag ? tag->parent : tag->prev;
            }
        }
        if (tag) {
            QString col = tag->tag.simplified();
            if (col.left(10) == QLatin1String("font color")) {
                int i = col.indexOf(QLatin1Char('='), 10);
                col = col.mid(i + 1).simplified();
                if (col[0] == QLatin1Char('\"'))
                    col = col.mid(1, col.length() - 2);
            }
            QColor color = QColor(col);
            if (color.isValid()) {
                formatFlags |= Q3TextFormat::Color;
                f->setColor(color);
            }
        }
    } else { // use the stylesheet tag definition
        if (ssItem->color().isValid()) {
            formatFlags |= Q3TextFormat::Color;
            f->setColor(ssItem->color());
        }
        f->setBold(ssItem->fontWeight() == QFont::Bold);
        f->setItalic(ssItem->fontItalic());
        f->setUnderline(ssItem->fontUnderline());
    }
    td->setFormat(0, f, formatFlags);
    td->removeSelection(0);
}

/*! \internal */
void Q3TextEdit::optimDrawContents(QPainter * p, int clipx, int clipy,
                                   int clipw, int cliph)
{
    QFontMetrics fm(Q3ScrollView::font());
    int startLine = clipy / fm.lineSpacing();

    // we always have to fetch at least two lines for drawing because the
    // painter may be translated so that parts of two lines cover the area
    // of a single line
    int nLines = (cliph / fm.lineSpacing()) + 2;
    int endLine = startLine + nLines;

    if (startLine >= d->od->numLines)
        return;
    if ((startLine + nLines) > d->od->numLines)
        nLines = d->od->numLines - startLine;

    int i = 0;
    QString str;
    for (i = startLine; i < (startLine + nLines); i++)
        str.append(d->od->lines[LOGOFFSET(i)] + QLatin1Char('\n'));

    Q3TextDocument * td = new Q3TextDocument(0);
    td->setDefaultFormat(Q3ScrollView::font(), QColor());
    td->setPlainText(str);
    td->setFormatter(new Q3TextFormatterBreakWords); // deleted by QTextDoc
    td->formatter()->setWrapEnabled(false);
    td->setTabStops(doc->tabStopWidth());

    // get the current text color from the current format
    td->selectAll(Q3TextDocument::Standard);
    Q3TextFormat f;
    f.setColor(palette().text().color());
    f.setFont(Q3ScrollView::font());
    td->setFormat(Q3TextDocument::Standard, &f,
                   Q3TextFormat::Color | Q3TextFormat::Font);
    td->removeSelection(Q3TextDocument::Standard);

    // add tag formatting
    if (d->od->tags) {
        int i = startLine;
        QMap<int,Q3TextEditOptimPrivate::Tag *>::ConstIterator it;
        Q3TextEditOptimPrivate::Tag * tag = 0, * tmp = 0;
        Q3TextCursor cur(td);
        // Step 1 - find previous left-tag
        tmp = optimPreviousLeftTag(i);
        for (; i < startLine + nLines; i++) {
            if ((it = d->od->tagIndex.constFind(LOGOFFSET(i))) != d->od->tagIndex.constEnd())
                tag = it.value();
            // Step 2 - iterate over tags on the current line
            int lastIndex = 0;
            while (tag && tag->line == i) {
                tmp = 0;
                if (tag->prev && !tag->prev->leftTag) {
                    tmp = tag->prev;
                } else if (tag->prev && tag->prev->parent) {
                    tmp = tag->prev->parent;
                }
                if ((tag->index - lastIndex) > 0 && tmp) {
                    optimSetTextFormat(td, &cur, &f, lastIndex, tag->index, tmp);
                }
                lastIndex = tag->index;
                tmp = tag;
                tag = tag->next;
            }
            // Step 3 - color last part of the line - if necessary
            if (tmp && tmp->parent)
                tmp = tmp->parent;
            if ((cur.paragraph()->length()-1 - lastIndex) > 0 && tmp && !tmp->leftTag) {
                optimSetTextFormat(td, &cur, &f, lastIndex,
                                    cur.paragraph()->length() - 1, tmp);
            }
            cur.setParagraph(cur.paragraph()->next());
        }
        // useful debug info
        //
//         tag = d->od->tags;
//         qWarning("###");
//         while (tag) {
//             qWarning("Tag: %p, parent: %09p, leftTag: %09p, Name: %-15s, ParentName: %s, %d%d%d", tag,
//                        tag->parent, tag->leftTag, tag->tag.latin1(), tag->parent ? tag->parent->tag.latin1():"<none>",
//                       tag->bold, tag->italic, tag->underline);
//             tag = tag->next;
//         }
    }

    // if there is a selection, make sure that the selection in the
    // part we need to redraw is set correctly
    if (optimHasSelection()) {
        Q3TextCursor c1(td);
        Q3TextCursor c2(td);
        int selStart = d->od->selStart.line;
        int idxStart = d->od->selStart.index;
        int selEnd = d->od->selEnd.line;
        int idxEnd = d->od->selEnd.index;
        if (selEnd < selStart) {
            qSwap(&selStart, &selEnd);
            qSwap(&idxStart, &idxEnd);
        }
        if (selEnd > d->od->numLines-1) {
            selEnd = d->od->numLines-1;
        }
        if (startLine <= selStart && endLine >= selEnd) {
            // case 1: area to paint covers entire selection
            int paragS = selStart - startLine;
            int paragE = paragS + (selEnd - selStart);
            Q3TextParagraph * parag = td->paragAt(paragS);
            if (parag) {
                c1.setParagraph(parag);
                if (td->text(paragS).length() >= idxStart)
                    c1.setIndex(idxStart);
            }
            parag = td->paragAt(paragE);
            if (parag) {
                c2.setParagraph(parag);
                if (td->text(paragE).length() >= idxEnd)
                    c2.setIndex(idxEnd);
            }
        } else if (startLine > selStart && endLine < selEnd) {
            // case 2: area to paint is all part of the selection
            td->selectAll(Q3TextDocument::Standard);
        } else if (startLine > selStart && endLine >= selEnd &&
                    startLine <= selEnd) {
            // case 3: area to paint starts inside a selection, ends past it
            c1.setParagraph(td->firstParagraph());
            c1.setIndex(0);
            int paragE = selEnd - startLine;
            Q3TextParagraph * parag = td->paragAt(paragE);
            if (parag) {
                c2.setParagraph(parag);
                if (td->text(paragE).length() >= idxEnd)
                    c2.setIndex(idxEnd);
            }
        } else if (startLine <= selStart && endLine < selEnd &&
                    endLine > selStart) {
            // case 4: area to paint starts before a selection, ends inside it
            int paragS = selStart - startLine;
            Q3TextParagraph * parag = td->paragAt(paragS);
            if (parag) {
                c1.setParagraph(parag);
                c1.setIndex(idxStart);
            }
            c2.setParagraph(td->lastParagraph());
            c2.setIndex(td->lastParagraph()->string()->toString().length() - 1);

        }
        // previously selected?
        if (!td->hasSelection(Q3TextDocument::Standard)) {
            td->setSelectionStart(Q3TextDocument::Standard, c1);
            td->setSelectionEnd(Q3TextDocument::Standard, c2);
        }
    }
    td->doLayout(p, contentsWidth());

    // have to align the painter so that partly visible lines are
    // drawn at the correct position within the area that needs to be
    // painted
    int offset = clipy % fm.lineSpacing() + 2;
    QRect r(clipx, 0, clipw, cliph + offset);
    p->translate(0, clipy - offset);
    td->draw(p, r.x(), r.y(), r.width(), r.height(), palette());
    p->translate(0, -(clipy - offset));
    delete td;
}

/*! \internal */
void Q3TextEdit::optimMousePressEvent(QMouseEvent * e)
{
    if (e->button() != Qt::LeftButton)
        return;

    QFontMetrics fm(Q3ScrollView::font());
    mousePressed = true;
    mousePos = e->pos();
    d->od->selStart.line = e->y() / fm.lineSpacing();
    if (d->od->selStart.line > d->od->numLines-1) {
        d->od->selStart.line = d->od->numLines-1;
        d->od->selStart.index = d->od->lines[LOGOFFSET(d->od->numLines-1)].length();
    } else {
        QString str = d->od->lines[LOGOFFSET(d->od->selStart.line)];
        d->od->selStart.index = optimCharIndex(str, mousePos.x());
    }
    d->od->selEnd.line = d->od->selStart.line;
    d->od->selEnd.index = d->od->selStart.index;
    oldMousePos = e->pos();
    repaintContents();
}

/*! \internal */
void Q3TextEdit::optimMouseReleaseEvent(QMouseEvent * e)
{
    if (e->button() != Qt::LeftButton)
        return;

    if (scrollTimer->isActive())
        scrollTimer->stop();
    if (!inDoubleClick) {
        QFontMetrics fm(Q3ScrollView::font());
        d->od->selEnd.line = e->y() / fm.lineSpacing();
        if (d->od->selEnd.line > d->od->numLines-1) {
            d->od->selEnd.line = d->od->numLines-1;
        }
        QString str = d->od->lines[LOGOFFSET(d->od->selEnd.line)];
        mousePos = e->pos();
        d->od->selEnd.index = optimCharIndex(str, mousePos.x());
        if (d->od->selEnd.line < d->od->selStart.line) {
            qSwap(&d->od->selStart.line, &d->od->selEnd.line);
            qSwap(&d->od->selStart.index, &d->od->selEnd.index);
        } else if (d->od->selStart.line == d->od->selEnd.line &&
                    d->od->selStart.index > d->od->selEnd.index) {
            qSwap(&d->od->selStart.index, &d->od->selEnd.index);
        }
        oldMousePos = e->pos();
        repaintContents();
    }
    if (mousePressed) {
        mousePressed = false;
        copyToClipboard();
    }

    inDoubleClick = false;
    emit copyAvailable(optimHasSelection());
    emit selectionChanged();
}

/*! \internal */
void Q3TextEdit::optimMouseMoveEvent(QMouseEvent * e)
{
    mousePos = e->pos();
    optimDoAutoScroll();
    oldMousePos = mousePos;
}

/*! \internal */
void Q3TextEdit::optimDoAutoScroll()
{
    if (!mousePressed)
        return;

    QFontMetrics fm(Q3ScrollView::font());
    QPoint pos(mapFromGlobal(QCursor::pos()));
    bool doScroll = false;
    int xx = contentsX() + pos.x();
    int yy = contentsY() + pos.y();

    // find out how much we have to scroll in either dir.
    if (pos.x() < 0 || pos.x() > viewport()->width() ||
         pos.y() < 0 || pos.y() > viewport()->height()) {
        int my = yy;
        if (pos.x() < 0)
            xx = contentsX() - fm.width(QLatin1Char('w'));
        else if (pos.x() > viewport()->width())
            xx = contentsX() + viewport()->width() + fm.width(QLatin1Char('w'));

        if (pos.y() < 0) {
            my = contentsY() - 1;
            yy = (my / fm.lineSpacing()) * fm.lineSpacing() + 1;
        } else if (pos.y() > viewport()->height()) {
            my = contentsY() + viewport()->height() + 1;
            yy = (my / fm.lineSpacing() + 1) * fm.lineSpacing() - 1;
        }
        d->od->selEnd.line = my / fm.lineSpacing();
        mousePos.setX(xx);
        mousePos.setY(my);
        doScroll = true;
    } else {
        d->od->selEnd.line = mousePos.y() / fm.lineSpacing();
    }

    if (d->od->selEnd.line < 0) {
        d->od->selEnd.line = 0;
    } else if (d->od->selEnd.line > d->od->numLines-1) {
        d->od->selEnd.line = d->od->numLines-1;
    }

    QString str = d->od->lines[LOGOFFSET(d->od->selEnd.line)];
    d->od->selEnd.index = optimCharIndex(str, mousePos.x());

    // have to have a valid index before generating a paint event
    if (doScroll)
        ensureVisible(xx, yy, 1, 1);

    // if the text document is smaller than the height of the viewport
    // - redraw the whole thing otherwise calculate the rect that
    // needs drawing.
    if (d->od->numLines * fm.lineSpacing() < viewport()->height()) {
        repaintContents(contentsX(), contentsY(), width(), height());
    } else {
        int h = QABS(mousePos.y() - oldMousePos.y()) + fm.lineSpacing() * 2;
        int y;
        if (oldMousePos.y() < mousePos.y()) {
            y = oldMousePos.y() - fm.lineSpacing();
        } else {
            // expand paint area for a fully selected line
            h += fm.lineSpacing();
            y = mousePos.y() - fm.lineSpacing()*2;
        }
        if (y < 0)
            y = 0;
        repaintContents(contentsX(), y, width(), h);
    }

    if ((!scrollTimer->isActive() && pos.y() < 0) || pos.y() > height())
        scrollTimer->start(100, false);
    else if (scrollTimer->isActive() && pos.y() >= 0 && pos.y() <= height())
        scrollTimer->stop();
}

/*! \internal

  Returns the index of the character in the string \a str that is
  currently under the mouse pointer.
*/
int Q3TextEdit::optimCharIndex(const QString &str, int mx) const
{
    QFontMetrics fm(Q3ScrollView::font());
    int i = 0;
    int dd, dist = 10000000;
    int curpos = 0;
    int strWidth;
    mx = mx - 4; // ### get the real margin from somewhere

    if (!str.contains(QLatin1Char('\t')) && mx > fm.width(str))
        return str.length();

    while (i < str.length()) {
        strWidth = qStrWidth(str.left(i), tabStopWidth(), fm);
        dd = strWidth - mx;
        if (QABS(dd) <= dist) {
            dist = QABS(dd);
            if (mx >= strWidth)
                curpos = i;
        }
        ++i;
    }
    return curpos;
}

/*! \internal */
void Q3TextEdit::optimSelectAll()
{
    d->od->selStart.line = d->od->selStart.index = 0;
    d->od->selEnd.line = d->od->numLines - 1;
    d->od->selEnd.index = d->od->lines[LOGOFFSET(d->od->selEnd.line)].length();

    repaintContents();
    emit copyAvailable(optimHasSelection());
    emit selectionChanged();
}

/*! \internal */
void Q3TextEdit::optimRemoveSelection()
{
    d->od->selStart.line = d->od->selEnd.line = -1;
    d->od->selStart.index = d->od->selEnd.index = -1;
    repaintContents();
}

/*! \internal */
void Q3TextEdit::optimSetSelection(int startLine, int startIdx,
                                       int endLine, int endIdx)
{
    d->od->selStart.line = startLine;
    d->od->selEnd.line = endLine;
    d->od->selStart.index = startIdx;
    d->od->selEnd.index = endIdx;
}

/*! \internal */
bool Q3TextEdit::optimHasSelection() const
{
    if (d->od->selStart.line != d->od->selEnd.line ||
         d->od->selStart.index != d->od->selEnd.index)
        return true;
    return false;
}

/*! \internal */
QString Q3TextEdit::optimSelectedText() const
{
    QString str;

    if (!optimHasSelection())
        return str;

    // concatenate all strings
    if (d->od->selStart.line == d->od->selEnd.line) {
        str = d->od->lines[LOGOFFSET(d->od->selEnd.line)].mid(d->od->selStart.index,
                           d->od->selEnd.index - d->od->selStart.index);
    } else {
        int i = d->od->selStart.line;
        str = d->od->lines[LOGOFFSET(i)].right(d->od->lines[LOGOFFSET(i)].length() -
                                  d->od->selStart.index) + QLatin1Char('\n');
        i++;
        for (; i < d->od->selEnd.line; i++) {
            if (d->od->lines[LOGOFFSET(i)].isEmpty()) // CR lines are empty
                str += QLatin1Char('\n');
            else
                str += d->od->lines[LOGOFFSET(i)] + QLatin1Char('\n');
        }
        str += d->od->lines[LOGOFFSET(d->od->selEnd.line)].left(d->od->selEnd.index);
    }
    return str;
}

/*! \internal */
bool Q3TextEdit::optimFind(const QString & expr, bool cs, bool /*wo*/,
                               bool fw, int * para, int * index)
{
    bool found = false;
    int parag = para ? *para : d->od->search.line,
          idx = index ? *index : d->od->search.index, i;

    if (d->od->len == 0)
        return false;

    for (i = parag; fw ? i < d->od->numLines : i >= 0; fw ? i++ : i--) {
        idx = fw
              ? d->od->lines[LOGOFFSET(i)].indexOf(expr, idx,
                                                     cs ? Qt::CaseSensitive : Qt::CaseInsensitive)
              : d->od->lines[LOGOFFSET(i)].lastIndexOf(expr, idx,
                                                         cs ? Qt::CaseSensitive : Qt::CaseInsensitive);
        if (idx != -1) {
            found = true;
            break;
        } else if (fw)
            idx = 0;
    }

    if (found) {
        if (index)
            *index = idx;
        if (para)
            *para = i;
        d->od->search.index = idx;
        d->od->search.line = i;
        optimSetSelection(i, idx, i, idx + expr.length());
        QFontMetrics fm(Q3ScrollView::font());
        int h = fm.lineSpacing();
        int x = fm.width(d->od->lines[LOGOFFSET(i)].left(idx + expr.length())) + 4;
        ensureVisible(x, i * h + h / 2, 1, h / 2 + 2);
        repaintContents(); // could possibly be optimized
    }
    return found;
}

/*! \reimp */
void Q3TextEdit::polishEvent(QEvent*)
{
    // this will ensure that the last line is visible if text have
    // been added to the widget before it is shown
    if (d->optimMode)
        scrollToBottom();
}

/*!
    Sets the maximum number of lines a Q3TextEdit can hold in \c
    Qt::LogText mode to \a limit. If \a limit is -1 (the default), this
    signifies an unlimited number of lines.

    \warning Never use formatting tags that span more than one line
    when the maximum log lines is set. When lines are removed from the
    top of the buffer it could result in an unbalanced tag pair, i.e.
    the left formatting tag is removed before the right one.
 */
void Q3TextEdit::setMaxLogLines(int limit)
{
    d->maxLogLines = limit;
    if (d->maxLogLines < -1)
        d->maxLogLines = -1;
    if (d->maxLogLines == -1)
        d->logOffset = 0;
}

/*!
    Returns the maximum number of lines Q3TextEdit can hold in \c
    Qt::LogText mode. By default the number of lines is unlimited, which
    is signified by a value of -1.
 */
int Q3TextEdit::maxLogLines() const
{
    return d->maxLogLines;
}

/*!
    Check if the number of lines in the buffer is limited, and uphold
    that limit when appending new lines.
 */
void Q3TextEdit::optimCheckLimit(const QString& str)
{
    if (d->maxLogLines > -1 && d->maxLogLines <= d->od->numLines) {
        // NB! Removing the top line in the buffer will potentially
        // destroy the structure holding the formatting tags - if line
        // spanning tags are used.
        Q3TextEditOptimPrivate::Tag *t = d->od->tags, *tmp, *itr;
        QList<Q3TextEditOptimPrivate::Tag *> lst;
        while (t) {
            t->line -= 1;
            // unhook the ptr from the tag structure
            if (((uint) LOGOFFSET(t->line) < (uint) d->logOffset &&
                  (uint) LOGOFFSET(t->line) < (uint) LOGOFFSET(d->od->numLines) &&
                  (uint) LOGOFFSET(d->od->numLines) > (uint) d->logOffset))
            {
                if (t->prev)
                    t->prev->next = t->next;
                if (t->next)
                    t->next->prev = t->prev;
                if (d->od->tags == t)
                    d->od->tags = t->next;
                if (d->od->lastTag == t) {
                    if (t->prev)
                        d->od->lastTag = t->prev;
                    else
                        d->od->lastTag = d->od->tags;
                }
                tmp = t;
                t = t->next;
                lst.append(tmp);
                delete tmp;
            } else {
                t = t->next;
            }
        }
        // Remove all references to the ptrs we just deleted
        itr = d->od->tags;
        while (itr) {
            for (int i = 0; i < lst.size(); ++i) {
                tmp = lst.at(i);
                if (itr->parent == tmp)
                    itr->parent = 0;
                if (itr->leftTag == tmp)
                    itr->leftTag = 0;
            }
            itr = itr->next;
        }
        // ...in the tag index as well
        QMap<int, Q3TextEditOptimPrivate::Tag *>::Iterator idx;
        if ((idx = d->od->tagIndex.find(d->logOffset)) != d->od->tagIndex.end())
            d->od->tagIndex.erase(idx);

        QMap<int,QString>::Iterator it;
        if ((it = d->od->lines.find(d->logOffset)) != d->od->lines.end()) {
            d->od->len -= (*it).length();
            d->od->lines.erase(it);
            d->od->numLines--;
            d->logOffset = LOGOFFSET(1);
        }
    }
    d->od->len += str.length();
    d->od->lines[LOGOFFSET(d->od->numLines++)] = str;
}

#endif // QT_TEXTEDIT_OPTIMIZATION

/*!
    \property Q3TextEdit::autoFormatting
    \brief the enabled set of auto formatting features

    The value can be any combination of the values in the \c
    AutoFormattingFlag enum.  The default is \c AutoAll. Choose \c AutoNone
    to disable all automatic formatting.

    Currently, the only automatic formatting feature provided is \c
    AutoBulletList; future versions of Qt may offer more.
*/

void Q3TextEdit::setAutoFormatting(AutoFormatting features)
{
    d->autoFormatting = features;
}

Q3TextEdit::AutoFormatting Q3TextEdit::autoFormatting() const
{
    return d->autoFormatting;
}

/*!
    Returns the QSyntaxHighlighter set on this Q3TextEdit. 0 is
    returned if no syntax highlighter is set.
 */
Q3SyntaxHighlighter * Q3TextEdit::syntaxHighlighter() const
{
    if (document()->preProcessor())
        return ((Q3SyntaxHighlighterInternal *) document()->preProcessor())->highlighter;
    else
        return 0;
}

QT_END_NAMESPACE

#endif //QT_NO_TEXTEDIT
