/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the QtGui module of the Qt Toolkit.
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

#include "qlineedit.h"
#include "qlineedit_p.h"

#ifndef QT_NO_LINEEDIT
#include "qaction.h"
#include "qapplication.h"
#include "qclipboard.h"
#include "qdrag.h"
#include "qdrawutil.h"
#include "qevent.h"
#include "qfontmetrics.h"
#include "qmenu.h"
#include "qpainter.h"
#include "qpixmap.h"
#include "qpointer.h"
#include "qstringlist.h"
#include "qstyle.h"
#include "qstyleoption.h"
#include "qtimer.h"
#include "qvalidator.h"
#include "qvariant.h"
#include "qvector.h"
#include "qwhatsthis.h"
#include "qdebug.h"
#include "qtextedit.h"
#include <private/qtextedit_p.h>
#ifndef QT_NO_ACCESSIBILITY
#include "qaccessible.h"
#endif
#ifndef QT_NO_IM
#include "qinputcontext.h"
#include "qlist.h"
#endif
#include "qabstractitemview.h"
#include "private/qstylesheetstyle_p.h"

#ifndef QT_NO_SHORTCUT
#include "private/qapplication_p.h"
#include "private/qshortcutmap_p.h"
#include "qkeysequence.h"
#define ACCEL_KEY(k) (!qApp->d_func()->shortcutMap.hasShortcutForKeySequence(k) ? QLatin1String("\t") + QString(QKeySequence(k)) : QString())
#else
#define ACCEL_KEY(k) QString()
#endif

#include <limits.h>

#define verticalMargin 1
#define horizontalMargin 2

QT_BEGIN_NAMESPACE

#ifdef Q_WS_MAC
extern void qt_mac_secure_keyboard(bool); //qapplication_mac.cpp
#endif

static inline bool shouldEnableInputMethod(QLineEdit *lineedit)
{
    const QLineEdit::EchoMode mode = lineedit->echoMode();
    return !lineedit->isReadOnly() && (mode == QLineEdit::Normal || mode == QLineEdit::PasswordEchoOnEdit);
}

/*!
    Initialize \a option with the values from this QLineEdit. This method
    is useful for subclasses when they need a QStyleOptionFrame or QStyleOptionFrameV2, but don't want
    to fill in all the information themselves. This function will check the version
    of the QStyleOptionFrame and fill in the additional values for a
    QStyleOptionFrameV2.

    \sa QStyleOption::initFrom()
*/
void QLineEdit::initStyleOption(QStyleOptionFrame *option) const
{
    if (!option)
        return;

    Q_D(const QLineEdit);
    option->initFrom(this);
    option->rect = contentsRect();
    option->lineWidth = d->frame ? style()->pixelMetric(QStyle::PM_DefaultFrameWidth, option, this)
                                 : 0;
    option->midLineWidth = 0;
    option->state |= QStyle::State_Sunken;
    if (d->readOnly)
        option->state |= QStyle::State_ReadOnly;
#ifdef QT_KEYPAD_NAVIGATION
    if (hasEditFocus())
        option->state |= QStyle::State_HasEditFocus;
#endif
    if (QStyleOptionFrameV2 *optionV2 = qstyleoption_cast<QStyleOptionFrameV2 *>(option))
        optionV2->features = QStyleOptionFrameV2::None;
}

/*!
    \class QLineEdit
    \brief The QLineEdit widget is a one-line text editor.

    \ingroup basicwidgets
    \mainclass

    A line edit allows the user to enter and edit a single line of
    plain text with a useful collection of editing functions,
    including undo and redo, cut and paste, and drag and drop.

    By changing the echoMode() of a line edit, it can also be used as
    a "write-only" field, for inputs such as passwords.

    The length of the text can be constrained to maxLength(). The text
    can be arbitrarily constrained using a validator() or an
    inputMask(), or both.

    A related class is QTextEdit which allows multi-line, rich text
    editing.

    You can change the text with setText() or insert(). The text is
    retrieved with text(); the displayed text (which may be different,
    see \l{EchoMode}) is retrieved with displayText(). Text can be
    selected with setSelection() or selectAll(), and the selection can
    be cut(), copy()ied and paste()d. The text can be aligned with
    setAlignment().

    When the text changes the textChanged() signal is emitted; when
    the text changes other than by calling setText() the textEdited()
    signal is emitted; when the cursor is moved the
    cursorPositionChanged() signal is emitted; and when the Return or
    Enter key is pressed the returnPressed() signal is emitted.

    When editing is finished, either because the line edit lost focus
    or Return/Enter is pressed the editingFinished() signal is
    emitted.

    Note that if there is a validator set on the line edit, the
    returnPressed()/editingFinished() signals will only be emitted if
    the validator returns QValidator::Acceptable.

    By default, QLineEdits have a frame as specified by the Windows
    and Motif style guides; you can turn it off by calling
    setFrame(false).

    The default key bindings are described below. The line edit also
    provides a context menu (usually invoked by a right mouse click)
    that presents some of these editing options.
    \target desc
    \table
    \header \i Keypress \i Action
    \row \i Left Arrow \i Moves the cursor one character to the left.
    \row \i Shift+Left Arrow \i Moves and selects text one character to the left.
    \row \i Right Arrow \i Moves the cursor one character to the right.
    \row \i Shift+Right Arrow \i Moves and selects text one character to the right.
    \row \i Home \i Moves the cursor to the beginning of the line.
    \row \i End \i Moves the cursor to the end of the line.
    \row \i Backspace \i Deletes the character to the left of the cursor.
    \row \i Ctrl+Backspace \i Deletes the word to the left of the cursor.
    \row \i Delete \i Deletes the character to the right of the cursor.
    \row \i Ctrl+Delete \i Deletes the word to the right of the cursor.
    \row \i Ctrl+A \i Select all.
    \row \i Ctrl+C \i Copies the selected text to the clipboard.
    \row \i Ctrl+Insert \i Copies the selected text to the clipboard.
    \row \i Ctrl+K \i Deletes to the end of the line.
    \row \i Ctrl+V \i Pastes the clipboard text into line edit.
    \row \i Shift+Insert \i Pastes the clipboard text into line edit.
    \row \i Ctrl+X \i Deletes the selected text and copies it to the clipboard.
    \row \i Shift+Delete \i Deletes the selected text and copies it to the clipboard.
    \row \i Ctrl+Z \i Undoes the last operation.
    \row \i Ctrl+Y \i Redoes the last undone operation.
    \endtable

    Any other key sequence that represents a valid character, will
    cause the character to be inserted into the line edit.

    \table 100%
    \row \o \inlineimage macintosh-lineedit.png Screenshot of a Macintosh style line edit
         \o A line edit shown in the \l{Macintosh Style Widget Gallery}{Macintosh widget style}.
    \row \o \inlineimage windows-lineedit.png Screenshot of a Windows XP style line edit
         \o A line edit shown in the \l{Windows XP Style Widget Gallery}{Windows XP widget style}.
    \row \o \inlineimage plastique-lineedit.png Screenshot of a Plastique style line edit
         \o A line edit shown in the \l{Plastique Style Widget Gallery}{Plastique widget style}.
    \endtable

    \sa QTextEdit, QLabel, QComboBox, {fowler}{GUI Design Handbook: Field, Entry}, {Line Edits Example}
*/


/*!
    \fn void QLineEdit::textChanged(const QString &text)

    This signal is emitted whenever the text changes. The \a text
    argument is the new text.

    Unlike textEdited(), this signal is also emitted when the text is
    changed programmatically, for example, by calling setText().
*/

/*!
    \fn void QLineEdit::textEdited(const QString &text)

    This signal is emitted whenever the text is edited. The \a text
    argument is the next text.

    Unlike textChanged(), this signal is not emitted when the text is
    changed programmatically, for example, by calling setText().
*/

/*!
    \fn void QLineEdit::cursorPositionChanged(int old, int new)

    This signal is emitted whenever the cursor moves. The previous
    position is given by \a old, and the new position by \a new.

    \sa setCursorPosition(), cursorPosition()
*/

/*!
    \fn void QLineEdit::selectionChanged()

    This signal is emitted whenever the selection changes.

    \sa hasSelectedText(), selectedText()
*/

/*!
    Constructs a line edit with no text.

    The maximum text length is set to 32767 characters.

    The \a parent argument is sent to the QWidget constructor.

    \sa setText(), setMaxLength()
*/
QLineEdit::QLineEdit(QWidget* parent)
    : QWidget(*new QLineEditPrivate, parent,0)
{
    Q_D(QLineEdit);
    d->init(QString());
}

/*!
    Constructs a line edit containing the text \a contents.

    The cursor position is set to the end of the line and the maximum
    text length to 32767 characters.

    The \a parent and argument is sent to the QWidget
    constructor.

    \sa text(), setMaxLength()
*/
QLineEdit::QLineEdit(const QString& contents, QWidget* parent)
    : QWidget(*new QLineEditPrivate, parent, 0)
{
    Q_D(QLineEdit);
    d->init(contents);
}


#ifdef QT3_SUPPORT
/*!
    Constructs a line edit with no text.

    The maximum text length is set to 32767 characters.

    The \a parent and \a name arguments are sent to the QWidget constructor.

    \sa setText(), setMaxLength()
*/
QLineEdit::QLineEdit(QWidget* parent, const char* name)
    : QWidget(*new QLineEditPrivate, parent,0)
{
    Q_D(QLineEdit);
    setObjectName(QString::fromAscii(name));
    d->init(QString());
}

/*!
    Constructs a line edit containing the text \a contents.

    The cursor position is set to the end of the line and the maximum
    text length to 32767 characters.

    The \a parent and \a name arguments are sent to the QWidget
    constructor.

    \sa text(), setMaxLength()
*/

QLineEdit::QLineEdit(const QString& contents, QWidget* parent, const char* name)
    : QWidget(*new QLineEditPrivate, parent, 0)
{
    Q_D(QLineEdit);
    setObjectName(QString::fromAscii(name));
    d->init(contents);
}

/*!
    Constructs a line edit with an input \a inputMask and the text \a
    contents.

    The cursor position is set to the end of the line and the maximum
    text length is set to the length of the mask (the number of mask
    characters and separators).

    The \a parent and \a name arguments are sent to the QWidget
    constructor.

    \sa setMask() text()
*/
QLineEdit::QLineEdit(const QString& contents, const QString &inputMask, QWidget* parent, const char* name)
    : QWidget(*new QLineEditPrivate, parent, 0)
{
    Q_D(QLineEdit);
    setObjectName(QString::fromAscii(name));
    d->parseInputMask(inputMask);
    if (d->maskData) {
        QString ms = d->maskString(0, contents);
        d->init(ms + d->clearString(ms.length(), d->maxLength - ms.length()));
        d->cursor = d->nextMaskBlank(ms.length());
    } else {
        d->init(contents);
    }
}
#endif

/*!
    Destroys the line edit.
*/

QLineEdit::~QLineEdit()
{
}


/*!
    \property QLineEdit::text
    \brief the line edit's text

    Setting this property clears the selection, clears the undo/redo
    history, moves the cursor to the end of the line and resets the
    \l modified property to false. The text is not validated when
    inserted with setText().

    The text is truncated to maxLength() length.

    By default, this property contains an empty string.

    \sa insert(), clear()
*/
QString QLineEdit::text() const
{
    Q_D(const QLineEdit);
    QString res = d->text;
    if (d->maskData)
        res = d->stripString(d->text);
    return (res.isNull() ? QString::fromLatin1("") : res);
}

void QLineEdit::setText(const QString& text)
{
    Q_D(QLineEdit);
    d->setText(text, -1, false);
#ifdef QT_KEYPAD_NAVIGATION
    d->origText = d->text;
#endif
}


/*!
    \property QLineEdit::displayText
    \brief the displayed text

    If \l echoMode is \l Normal this returns the same as text(); if
    \l EchoMode is \l Password or \l PasswordEchoOnEdit it returns a string of asterisks
    text().length() characters long, e.g. "******"; if \l EchoMode is
    \l NoEcho returns an empty string, "".

    By default, this property contains an empty string.

    \sa setEchoMode() text() EchoMode
*/

QString QLineEdit::displayText() const
{
    Q_D(const QLineEdit);
    if (d->echoMode == NoEcho)
        return QString::fromLatin1("");
    QString res = d->text;

    if (d->echoMode == Password || (d->echoMode == PasswordEchoOnEdit
                                    && !d->passwordEchoEditing)) {
        QStyleOptionFrameV2 opt;
        initStyleOption(&opt);
        res.fill(style()->styleHint(QStyle::SH_LineEdit_PasswordCharacter, &opt, this));
    }
    return (res.isNull() ? QString::fromLatin1("") : res);
}


/*!
    \property QLineEdit::maxLength
    \brief the maximum permitted length of the text

    If the text is too long, it is truncated at the limit.

    If truncation occurs any selected text will be unselected, the
    cursor position is set to 0 and the first part of the string is
    shown.

    If the line edit has an input mask, the mask defines the maximum
    string length.

    By default, this property contains a value of 32767.

    \sa inputMask
*/

int QLineEdit::maxLength() const
{
    Q_D(const QLineEdit);
    return d->maxLength;
}

void QLineEdit::setMaxLength(int maxLength)
{
    Q_D(QLineEdit);
    if (d->maskData)
        return;
    d->maxLength = maxLength;
    setText(d->text);
}



/*!
    \property QLineEdit::frame
    \brief whether the line edit draws itself with a frame

    If enabled (the default) the line edit draws itself inside a
    frame, otherwise the line edit draws itself without any frame.
*/
bool QLineEdit::hasFrame() const
{
    Q_D(const QLineEdit);
    return d->frame;
}


void QLineEdit::setFrame(bool enable)
{
    Q_D(QLineEdit);
    d->frame = enable;
    update();
    updateGeometry();
}


/*!
    \enum QLineEdit::EchoMode

    This enum type describes how a line edit should display its
    contents.

    \value Normal   Display characters as they are entered. This is the
                    default.
    \value NoEcho   Do not display anything. This may be appropriate
                    for passwords where even the length of the
                    password should be kept secret.
    \value Password  Display asterisks instead of the characters
                    actually entered.
    \value PasswordEchoOnEdit Display characters as they are entered
                    while editing otherwise display asterisks.

    \sa setEchoMode() echoMode()
*/


/*!
    \property QLineEdit::echoMode
    \brief the line edit's echo mode

    The echo mode determines how the text entered in the line edit is
    displayed (or echoed) to the user.

    The most common setting is \l Normal, in which the text entered by the
    user is displayed verbatim, but QLineEdit also supports modes that allow
    the entered text to be suppressed or obscured: these include \l NoEcho,
    \l Password and \l PasswordEchoOnEdit.

    The widget's display and the ability to copy or drag the text is
    affected by this setting.

    By default, this property is set to \l Normal.

    \sa EchoMode displayText()
*/

QLineEdit::EchoMode QLineEdit::echoMode() const
{
    Q_D(const QLineEdit);
    return (EchoMode) d->echoMode;
}

void QLineEdit::setEchoMode(EchoMode mode)
{
    Q_D(QLineEdit);
    if (mode == (EchoMode)d->echoMode)
        return;
    setAttribute(Qt::WA_InputMethodEnabled, shouldEnableInputMethod(this));
    d->echoMode = mode;
    d->passwordEchoEditing = false;
    d->updateTextLayout();
    update();
#ifdef Q_WS_MAC
    if (hasFocus())
        qt_mac_secure_keyboard(d->echoMode == Password || d->echoMode == NoEcho);
#endif
}


#ifndef QT_NO_VALIDATOR
/*!
    Returns a pointer to the current input validator, or 0 if no
    validator has been set.

    \sa setValidator()
*/

const QValidator * QLineEdit::validator() const
{
    Q_D(const QLineEdit);
    return d->validator;
}

/*!
    Sets this line edit to only accept input that the validator, \a v,
    will accept. This allows you to place any arbitrary constraints on
    the text which may be entered.

    If \a v == 0, setValidator() removes the current input validator.
    The initial setting is to have no input validator (i.e. any input
    is accepted up to maxLength()).

    \sa validator() QIntValidator QDoubleValidator QRegExpValidator
*/

void QLineEdit::setValidator(const QValidator *v)
{
    Q_D(QLineEdit);
    d->validator = const_cast<QValidator*>(v);
}
#endif // QT_NO_VALIDATOR

#ifndef QT_NO_COMPLETER
/*!
    \since 4.2

    Sets this line edit to provide auto completions from the completer, \a c.
    The completion mode is set using QCompleter::setCompletionMode().

    To use a QCompleter with a QValidator or QLineEdit::inputMask, you need to
    ensure that the model provided to QCompleter contains valid entries. You can
    use the QSortFilterProxyModel to ensure that the QCompleter's model contains
    only valid entries.

    If \a c == 0, setCompleter() removes the current completer, effectively
    disabling auto completion.

    \sa QCompleter
*/
void QLineEdit::setCompleter(QCompleter *c)
{
    Q_D(QLineEdit);
    if (c == d->completer)
        return;
    if (d->completer) {
        disconnect(d->completer, 0, this, 0);
        d->completer->setWidget(0);
        if (d->completer->parent() == this)
            delete d->completer;
    }
    d->completer = c;
    if (!c)
        return;
    if (c->widget() == 0)
        c->setWidget(this);
    if (hasFocus()) {
        QObject::connect(d->completer, SIGNAL(activated(QString)),
                         this, SLOT(setText(QString)));
        QObject::connect(d->completer, SIGNAL(highlighted(QString)),
                         this, SLOT(_q_completionHighlighted(QString)));
    }
}

/*!
    \since 4.2

    Returns the current QCompleter that provides completions.
*/
QCompleter *QLineEdit::completer() const
{
    Q_D(const QLineEdit);
    return d->completer;
}

// looks for an enabled item iterating forward(dir=1)/backward(dir=-1) from the
// current row based. dir=0 indicates a new completion prefix was set.
bool QLineEditPrivate::advanceToEnabledItem(int dir)
{
    int start = completer->currentRow();
    if (start == -1)
        return false;
    int i = start + dir;
    if (dir == 0) dir = 1;
    do {
        if (!completer->setCurrentRow(i)) {
            if (!completer->wrapAround())
                break;
            i = i > 0 ? 0 : completer->completionCount() - 1;
        } else {
            QModelIndex currentIndex = completer->currentIndex();
            if (completer->completionModel()->flags(currentIndex) & Qt::ItemIsEnabled)
                return true;
            i += dir;
        }
    } while (i != start);

    completer->setCurrentRow(start); // restore
    return false;
}

void QLineEditPrivate::complete(int key)
{
    if (!completer || readOnly || echoMode != QLineEdit::Normal)
        return;

    if (completer->completionMode() == QCompleter::InlineCompletion) {
        if (key == Qt::Key_Backspace)
            return;
        int n = 0;
        if (key == Qt::Key_Up || key == Qt::Key_Down) {
            if (selend != 0 && selend != text.length())
                return;
            QString prefix = hasSelectedText() ? text.left(selstart) : text;
            if (text.compare(completer->currentCompletion(), completer->caseSensitivity()) != 0
                || prefix.compare(completer->completionPrefix(), completer->caseSensitivity()) != 0) {
                completer->setCompletionPrefix(prefix);
            } else {
                n = (key == Qt::Key_Up) ? -1 : +1;
            }
        } else {
            completer->setCompletionPrefix(text);
        }
        if (!advanceToEnabledItem(n))
            return;
    } else {
#ifndef QT_KEYPAD_NAVIGATION
        if (text.isEmpty()) {
            completer->popup()->hide();
            return;
        }
#endif
        completer->setCompletionPrefix(text);
    }

    completer->complete();
}

void QLineEditPrivate::_q_completionHighlighted(QString newText)
{
    Q_Q(QLineEdit);
    if (completer->completionMode() != QCompleter::InlineCompletion)
        q->setText(newText);
    else {
        int c = cursor;
        q->setText(text.left(c) + newText.mid(c));
        q->setSelection(text.length(), c - newText.length());
    }
}
#endif // QT_NO_COMPLETER

/*!
    Returns a recommended size for the widget.

    The width returned, in pixels, is usually enough for about 15 to
    20 characters.
*/

QSize QLineEdit::sizeHint() const
{
    Q_D(const QLineEdit);
    ensurePolished();
    QFontMetrics fm(font());
    int h = qMax(fm.lineSpacing(), 14) + 2*verticalMargin
            + d->topTextMargin + d->bottomTextMargin
            + d->topmargin + d->bottommargin;
    int w = fm.width(QLatin1Char('x')) * 17 + 2*horizontalMargin
            + d->leftTextMargin + d->rightTextMargin
            + d->leftmargin + d->rightmargin; // "some"
    QStyleOptionFrameV2 opt;
    initStyleOption(&opt);
    return (style()->sizeFromContents(QStyle::CT_LineEdit, &opt, QSize(w, h).
                                      expandedTo(QApplication::globalStrut()), this));
}


/*!
    Returns a minimum size for the line edit.

    The width returned is enough for at least one character.
*/

QSize QLineEdit::minimumSizeHint() const
{
    Q_D(const QLineEdit);
    ensurePolished();
    QFontMetrics fm = fontMetrics();
    int h = fm.height() + qMax(2*verticalMargin, fm.leading())
            + d->topmargin + d->bottommargin;
    int w = fm.maxWidth() + d->leftmargin + d->rightmargin;
    QStyleOptionFrameV2 opt;
    initStyleOption(&opt);
    return (style()->sizeFromContents(QStyle::CT_LineEdit, &opt, QSize(w, h).
                                      expandedTo(QApplication::globalStrut()), this));
}


/*!
    \property QLineEdit::cursorPosition
    \brief the current cursor position for this line edit

    Setting the cursor position causes a repaint when appropriate.

    By default, this property contains a value of 0.
*/

int QLineEdit::cursorPosition() const
{
    Q_D(const QLineEdit);
    return d->cursor;
}

void QLineEdit::setCursorPosition(int pos)
{
    Q_D(QLineEdit);
    if (pos < 0)
        pos = 0;

    if (pos <= d->text.length())
        d->moveCursor(pos);
}

/*!
    Returns the cursor position under the point \a pos.
*/
// ### What should this do if the point is outside of contentsRect? Currently returns 0.
int QLineEdit::cursorPositionAt(const QPoint &pos)
{
    Q_D(QLineEdit);
    return d->xToPos(pos.x());
}


#ifdef QT3_SUPPORT
/*! \obsolete

    Use setText(), setCursorPosition() and setSelection() instead.
*/
bool QLineEdit::validateAndSet(const QString &newText, int newPos,
                                 int newMarkAnchor, int newMarkDrag)
{
    Q_D(QLineEdit);
    int priorState = d->undoState;
    d->selstart = 0;
    d->selend = d->text.length();
    d->removeSelectedText();
    d->insert(newText);
    d->finishChange(priorState);
    if (d->undoState > priorState) {
        d->cursor = newPos;
        d->selstart = qMin(newMarkAnchor, newMarkDrag);
        d->selend = qMax(newMarkAnchor, newMarkDrag);
        update();
        d->emitCursorPositionChanged();
        return true;
    }
    return false;
}
#endif //QT3_SUPPORT

/*!
    \property QLineEdit::alignment
    \brief the alignment of the line edit

    Both horizontal and vertical alignment is allowed here, Qt::AlignJustify
    will map to Qt::AlignLeft.

    By default, this property contains a combination of Qt::AlignLeft and Qt::AlignVCenter.

    \sa Qt::Alignment
*/

Qt::Alignment QLineEdit::alignment() const
{
    Q_D(const QLineEdit);
    return QFlag(d->alignment);
}

void QLineEdit::setAlignment(Qt::Alignment alignment)
{
    Q_D(QLineEdit);
    d->alignment = alignment;
    update();
}


/*!
    Moves the cursor forward \a steps characters. If \a mark is true
    each character moved over is added to the selection; if \a mark is
    false the selection is cleared.

    \sa cursorBackward()
*/

void QLineEdit::cursorForward(bool mark, int steps)
{
    Q_D(QLineEdit);
    int cursor = d->cursor;
    if (steps > 0) {
        while(steps--)
            cursor = d->textLayout.nextCursorPosition(cursor);
    } else if (steps < 0) {
        while (steps++)
            cursor = d->textLayout.previousCursorPosition(cursor);
    }
    d->moveCursor(cursor, mark);
}


/*!
    Moves the cursor back \a steps characters. If \a mark is true each
    character moved over is added to the selection; if \a mark is
    false the selection is cleared.

    \sa cursorForward()
*/
void QLineEdit::cursorBackward(bool mark, int steps)
{
    cursorForward(mark, -steps);
}

/*!
    Moves the cursor one word forward. If \a mark is true, the word is
    also selected.

    \sa cursorWordBackward()
*/
void QLineEdit::cursorWordForward(bool mark)
{
    Q_D(QLineEdit);
    d->moveCursor(d->textLayout.nextCursorPosition(d->cursor, QTextLayout::SkipWords), mark);
}

/*!
    Moves the cursor one word backward. If \a mark is true, the word
    is also selected.

    \sa cursorWordForward()
*/

void QLineEdit::cursorWordBackward(bool mark)
{
    Q_D(QLineEdit);
    d->moveCursor(d->textLayout.previousCursorPosition(d->cursor, QTextLayout::SkipWords), mark);
}


/*!
    If no text is selected, deletes the character to the left of the
    text cursor and moves the cursor one position to the left. If any
    text is selected, the cursor is moved to the beginning of the
    selected text and the selected text is deleted.

    \sa del()
*/
void QLineEdit::backspace()
{
    Q_D(QLineEdit);
    int priorState = d->undoState;
    if (d->hasSelectedText()) {
        d->removeSelectedText();
    } else if (d->cursor) {
            --d->cursor;
            if (d->maskData)
                d->cursor = d->prevMaskBlank(d->cursor);
            QChar uc = d->text.at(d->cursor);
            if (d->cursor > 0 && uc.unicode() >= 0xdc00 && uc.unicode() < 0xe000) {
                // second half of a surrogate, check if we have the first half as well,
                // if yes delete both at once
                uc = d->text.at(d->cursor - 1);
                if (uc.unicode() >= 0xd800 && uc.unicode() < 0xdc00) {
                    d->del(true);
                    --d->cursor;
                }
            }
            d->del(true);
    }
    d->finishChange(priorState);
}

/*!
    If no text is selected, deletes the character to the right of the
    text cursor. If any text is selected, the cursor is moved to the
    beginning of the selected text and the selected text is deleted.

    \sa backspace()
*/

void QLineEdit::del()
{
    Q_D(QLineEdit);
    int priorState = d->undoState;
    if (d->hasSelectedText()) {
        d->removeSelectedText();
    } else {
        int n = d->textLayout.nextCursorPosition(d->cursor) - d->cursor;
        while (n--)
            d->del();
    }
    d->finishChange(priorState);
}

/*!
    Moves the text cursor to the beginning of the line unless it is
    already there. If \a mark is true, text is selected towards the
    first position; otherwise, any selected text is unselected if the
    cursor is moved.

    \sa end()
*/

void QLineEdit::home(bool mark)
{
    Q_D(QLineEdit);
    d->moveCursor(0, mark);
}

/*!
    Moves the text cursor to the end of the line unless it is already
    there. If \a mark is true, text is selected towards the last
    position; otherwise, any selected text is unselected if the cursor
    is moved.

    \sa home()
*/

void QLineEdit::end(bool mark)
{
    Q_D(QLineEdit);
    d->moveCursor(d->text.length(), mark);
}


/*!
    \property QLineEdit::modified
    \brief whether the line edit's contents has been modified by the user

    The modified flag is never read by QLineEdit; it has a default value
    of false and is changed to true whenever the user changes the line
    edit's contents.

    This is useful for things that need to provide a default value but
    do not start out knowing what the default should be (perhaps it
    depends on other fields on the form). Start the line edit without
    the best default, and when the default is known, if modified()
    returns false (the user hasn't entered any text), insert the
    default value.

    Calling setText() resets the modified flag to false.
*/

bool QLineEdit::isModified() const
{
    Q_D(const QLineEdit);
    return d->modifiedState != d->undoState;
}

void QLineEdit::setModified(bool modified)
{
    Q_D(QLineEdit);
    if (modified)
        d->modifiedState = -1;
    else
        d->modifiedState = d->undoState;
}


/*!\fn QLineEdit::clearModified()

Use setModified(false) instead.

    \sa isModified()
*/


/*!
    \property QLineEdit::hasSelectedText
    \brief whether there is any text selected

    hasSelectedText() returns true if some or all of the text has been
    selected by the user; otherwise returns false.

    By default, this property is false.

    \sa selectedText()
*/


bool QLineEdit::hasSelectedText() const
{
    Q_D(const QLineEdit);
    return d->hasSelectedText();
}

/*!
    \property QLineEdit::selectedText
    \brief the selected text

    If there is no selected text this property's value is
    an empty string.

    By default, this property contains an empty string.

    \sa hasSelectedText()
*/

QString QLineEdit::selectedText() const
{
    Q_D(const QLineEdit);
    if (d->hasSelectedText())
        return d->text.mid(d->selstart, d->selend - d->selstart);
    return QString();
}

/*!
    selectionStart() returns the index of the first selected character in the
    line edit or -1 if no text is selected.

    \sa selectedText()
*/

int QLineEdit::selectionStart() const
{
    Q_D(const QLineEdit);
    return d->hasSelectedText() ? d->selstart : -1;
}


#ifdef QT3_SUPPORT

/*!
    \fn void QLineEdit::lostFocus()

    This signal is emitted when the line edit has lost focus.

    Use editingFinished() instead
    \sa editingFinished(), returnPressed()
*/

/*!
    Use isModified() instead.
*/
bool QLineEdit::edited() const { return isModified(); }
/*!
    Use setModified()  or setText().
*/
void QLineEdit::setEdited(bool on) { setModified(on); }

/*!
    There exists no equivalent functionality in Qt 4.
*/
int QLineEdit::characterAt(int xpos, QChar *chr) const
{
    Q_D(const QLineEdit);
    int pos = d->xToPos(xpos + contentsRect().x() - d->hscroll + horizontalMargin);
    if (chr && pos < (int) d->text.length())
        *chr = d->text.at(pos);
    return pos;

}

/*!
    Use selectedText() and selectionStart() instead.
*/
bool QLineEdit::getSelection(int *start, int *end)
{
    Q_D(QLineEdit);
    if (d->hasSelectedText() && start && end) {
        *start = d->selstart;
        *end = d->selend;
        return true;
    }
    return false;
}
#endif


/*!
    Selects text from position \a start and for \a length characters.
    Negative lengths are allowed.

    \sa deselect() selectAll() selectedText()
*/

void QLineEdit::setSelection(int start, int length)
{
    Q_D(QLineEdit);
    if (start < 0 || start > (int)d->text.length()) {
        qWarning("QLineEdit::setSelection: Invalid start position (%d)", start);
        return;
    } else {
        if (length > 0) {
            d->selstart = start;
            d->selend = qMin(start + length, (int)d->text.length());
            d->cursor = d->selend;
        } else {
            d->selstart = qMax(start + length, 0);
            d->selend = start;
            d->cursor = d->selstart;
        }
    }

    if (d->hasSelectedText()){
        QStyleOptionFrameV2 opt;
        initStyleOption(&opt);
        if (!style()->styleHint(QStyle::SH_BlinkCursorWhenTextSelected, &opt, this))
            d->setCursorVisible(false);
    }

    update();
    d->emitCursorPositionChanged();
}


/*!
    \property QLineEdit::undoAvailable
    \brief whether undo is available

    Undo becomes available once the user has modified the text in the line edit.

    By default, this property is false.
*/

bool QLineEdit::isUndoAvailable() const
{
    Q_D(const QLineEdit);
    return d->isUndoAvailable();
}

/*!
    \property QLineEdit::redoAvailable
    \brief whether redo is available

    Redo becomes available once the user has performed one or more undo operations
    on text in the line edit.

    By default, this property is false.
*/

bool QLineEdit::isRedoAvailable() const
{
    Q_D(const QLineEdit);
    return d->isRedoAvailable();
}

/*!
    \property QLineEdit::dragEnabled
    \brief whether the lineedit starts a drag if the user presses and
    moves the mouse on some selected text

    Dragging is disabled by default.
*/

bool QLineEdit::dragEnabled() const
{
    Q_D(const QLineEdit);
    return d->dragEnabled;
}

void QLineEdit::setDragEnabled(bool b)
{
    Q_D(QLineEdit);
    d->dragEnabled = b;
}


/*!
    \property QLineEdit::acceptableInput
    \brief whether the input satisfies the inputMask and the
    validator.

    By default, this property is true.

    \sa setInputMask(), setValidator()
*/
bool QLineEdit::hasAcceptableInput() const
{
    Q_D(const QLineEdit);
    return d->hasAcceptableInput(d->text);
}

/*!
    Sets the margins around the text inside the frame to have the
    sizes \a left, \a top, \a right, and \a bottom.
    \since 4.5

    See also getTextMargins().
*/
void QLineEdit::setTextMargins(int left, int top, int right, int bottom)
{
    Q_D(QLineEdit);
    d->leftTextMargin = left;
    d->topTextMargin = top;
    d->rightTextMargin = right;
    d->bottomTextMargin = bottom;
    updateGeometry();
    update();
}

/*!
    Returns the widget's text margins for \a left, \a top, \a right, and \a bottom.
    \since 4.5

    \sa setTextMargins()
*/
void QLineEdit::getTextMargins(int *left, int *top, int *right, int *bottom) const
{
    Q_D(const QLineEdit);
    if (left)
        *left = d->leftTextMargin;
    if (top)
        *top = d->topTextMargin;
    if (right)
        *right = d->rightTextMargin;
    if (bottom)
        *bottom = d->bottomTextMargin;
}

/*!
    \property QLineEdit::inputMask
    \brief The validation input mask

    If no mask is set, inputMask() returns an empty string.

    Sets the QLineEdit's validation mask. Validators can be used
    instead of, or in conjunction with masks; see setValidator().

    Unset the mask and return to normal QLineEdit operation by passing
    an empty string ("") or just calling setInputMask() with no
    arguments.

    The table below shows the characters that can be used in an input mask.
    A space character, the default character for a blank, is needed for cases
    where a character is \e{permitted but not required}.

    \table
    \header \i Character \i Meaning
    \row \i \c A \i ASCII alphabetic character required. A-Z, a-z.
    \row \i \c a \i ASCII alphabetic character permitted but not required.
    \row \i \c N \i ASCII alphanumeric character required. A-Z, a-z, 0-9.
    \row \i \c n \i ASCII alphanumeric character permitted but not required.
    \row \i \c X \i Any character required.
    \row \i \c x \i Any character permitted but not required.
    \row \i \c 9 \i ASCII digit required. 0-9.
    \row \i \c 0 \i ASCII digit permitted but not required.
    \row \i \c D \i ASCII digit required. 1-9.
    \row \i \c d \i ASCII digit permitted but not required (1-9).
    \row \i \c # \i ASCII digit or plus/minus sign permitted but not required.
    \row \i \c H \i Hexadecimal character required. A-F, a-f, 0-9.
    \row \i \c h \i Hexadecimal character permitted but not required.
    \row \i \c B \i Binary character required. 0-1.
    \row \i \c b \i Binary character permitted but not required.
    \row \i \c > \i All following alphabetic characters are uppercased.
    \row \i \c < \i All following alphabetic characters are lowercased.
    \row \i \c ! \i Switch off case conversion.
    \row \i \tt{\\} \i Use \tt{\\} to escape the special
                           characters listed above to use them as
                           separators.
    \endtable

    The mask consists of a string of mask characters and separators,
    optionally followed by a semicolon and the character used for
    blanks. The blank characters are always removed from the text
    after editing.

    Examples:
    \table
    \header \i Mask \i Notes
    \row \i \c 000.000.000.000;_ \i IP address; blanks are \c{_}.
    \row \i \c HH:HH:HH:HH:HH:HH;_ \i MAC address
    \row \i \c 0000-00-00 \i ISO Date; blanks are \c space
    \row \i \c >AAAAA-AAAAA-AAAAA-AAAAA-AAAAA;# \i License number;
    blanks are \c - and all (alphabetic) characters are converted to
    uppercase.
    \endtable

    To get range control (e.g., for an IP address) use masks together
    with \link setValidator() validators\endlink.

    \sa maxLength
*/
QString QLineEdit::inputMask() const
{
    Q_D(const QLineEdit);
    return (d->maskData ? d->inputMask + QLatin1Char(';') + d->blank : QString());
}

void QLineEdit::setInputMask(const QString &inputMask)
{
    Q_D(QLineEdit);
    d->parseInputMask(inputMask);
    if (d->maskData)
        d->moveCursor(d->nextMaskBlank(0));
}

/*!
    Selects all the text (i.e. highlights it) and moves the cursor to
    the end. This is useful when a default value has been inserted
    because if the user types before clicking on the widget, the
    selected text will be deleted.

    \sa setSelection() deselect()
*/

void QLineEdit::selectAll()
{
    Q_D(QLineEdit);
    d->selstart = d->selend = d->cursor = 0;
    d->moveCursor(d->text.length(), true);
}

/*!
    Deselects any selected text.

    \sa setSelection() selectAll()
*/

void QLineEdit::deselect()
{
    Q_D(QLineEdit);
    d->deselect();
    d->finishChange();
}


/*!
    Deletes any selected text, inserts \a newText, and validates the
    result. If it is valid, it sets it as the new contents of the line
    edit.

    \sa setText(), clear()
*/
void QLineEdit::insert(const QString &newText)
{
//     q->resetInputContext(); //#### FIX ME IN QT
    Q_D(QLineEdit);
    int priorState = d->undoState;
    d->removeSelectedText();
    d->insert(newText);
    d->finishChange(priorState);
}

/*!
    Clears the contents of the line edit.

    \sa setText(), insert()
*/
void QLineEdit::clear()
{
    Q_D(QLineEdit);
    int priorState = d->undoState;
    resetInputContext();
    d->selstart = 0;
    d->selend = d->text.length();
    d->removeSelectedText();
    d->separate();
    d->finishChange(priorState, /*update*/false, /*edited*/false);
}

/*!
    Undoes the last operation if undo is \link
    QLineEdit::undoAvailable available\endlink. Deselects any current
    selection, and updates the selection start to the current cursor
    position.
*/
void QLineEdit::undo()
{
    Q_D(QLineEdit);
    resetInputContext();
    d->undo();
    d->finishChange(-1, true);
}

/*!
    Redoes the last operation if redo is \link
    QLineEdit::redoAvailable available\endlink.
*/
void QLineEdit::redo()
{
    Q_D(QLineEdit);
    resetInputContext();
    d->redo();
    d->finishChange();
}


/*!
    \property QLineEdit::readOnly
    \brief whether the line edit is read only.

    In read-only mode, the user can still copy the text to the
    clipboard, or drag and drop the text (if echoMode() is \l Normal),
    but cannot edit it.

    QLineEdit does not show a cursor in read-only mode.

    By default, this property is false.

    \sa setEnabled()
*/

bool QLineEdit::isReadOnly() const
{
    Q_D(const QLineEdit);
    return d->readOnly;
}

void QLineEdit::setReadOnly(bool enable)
{
    Q_D(QLineEdit);
    if (d->readOnly != enable) {
        d->readOnly = enable;
        setAttribute(Qt::WA_MacShowFocusRect, !d->readOnly);
        setAttribute(Qt::WA_InputMethodEnabled, shouldEnableInputMethod(this));
#ifndef QT_NO_CURSOR
        setCursor(enable ? Qt::ArrowCursor : Qt::IBeamCursor);
#endif
        update();
    }
}


#ifndef QT_NO_CLIPBOARD
/*!
    Copies the selected text to the clipboard and deletes it, if there
    is any, and if echoMode() is \l Normal.

    If the current validator disallows deleting the selected text,
    cut() will copy without deleting.

    \sa copy() paste() setValidator()
*/

void QLineEdit::cut()
{
    if (hasSelectedText()) {
        copy();
        del();
    }
}


/*!
    Copies the selected text to the clipboard, if there is any, and if
    echoMode() is \l Normal.

    \sa cut() paste()
*/

void QLineEdit::copy() const
{
    Q_D(const QLineEdit);
    d->copy();
}

/*!
    Inserts the clipboard's text at the cursor position, deleting any
    selected text, providing the line edit is not \link
    QLineEdit::readOnly read-only\endlink.

    If the end result would not be acceptable to the current
    \link setValidator() validator\endlink, nothing happens.

    \sa copy() cut()
*/

void QLineEdit::paste()
{
    Q_D(QLineEdit);
    if (echoMode() == PasswordEchoOnEdit && !d->passwordEchoEditing) {
        // Clear the edit and reset to normal echo mode when pasting; the echo
        // mode switches back when the edit loses focus.  ### changes a public
        // property, resets current content
        d->updatePasswordEchoEditing(true);
        clear();
    }
    insert(QApplication::clipboard()->text(QClipboard::Clipboard));
}

void QLineEditPrivate::copy(bool clipboard) const
{
    Q_Q(const QLineEdit);
    QString t = q->selectedText();
    if (!t.isEmpty() && echoMode == QLineEdit::Normal) {
        q->disconnect(QApplication::clipboard(), SIGNAL(selectionChanged()), q, 0);
        QApplication::clipboard()->setText(t, clipboard ? QClipboard::Clipboard : QClipboard::Selection);
        q->connect(QApplication::clipboard(), SIGNAL(selectionChanged()),
                   q, SLOT(_q_clipboardChanged()));
    }
}

#endif // !QT_NO_CLIPBOARD

/*! \reimp
*/
bool QLineEdit::event(QEvent * e)
{
    Q_D(QLineEdit);
#ifndef QT_NO_SHORTCUT
    if (e->type() == QEvent::ShortcutOverride && !d->readOnly) {
        QKeyEvent* ke = (QKeyEvent*) e;
        if (ke == QKeySequence::Copy
            || ke == QKeySequence::Paste
            || ke == QKeySequence::Cut
            || ke == QKeySequence::Redo
            || ke == QKeySequence::Undo
            || ke == QKeySequence::MoveToNextWord
            || ke == QKeySequence::MoveToPreviousWord
            || ke == QKeySequence::MoveToStartOfDocument
            || ke == QKeySequence::MoveToEndOfDocument
            || ke == QKeySequence::SelectNextWord
            || ke == QKeySequence::SelectPreviousWord
            || ke == QKeySequence::SelectStartOfLine
            || ke == QKeySequence::SelectEndOfLine
            || ke == QKeySequence::SelectStartOfBlock
            || ke == QKeySequence::SelectEndOfBlock
            || ke == QKeySequence::SelectStartOfDocument
            || ke == QKeySequence::SelectAll
            || ke == QKeySequence::SelectEndOfDocument) {
            ke->accept();
        } else if (ke->modifiers() == Qt::NoModifier || ke->modifiers() == Qt::ShiftModifier
                   || ke->modifiers() == Qt::KeypadModifier) {
            if (ke->key() < Qt::Key_Escape) {
                ke->accept();
            } else {
                switch (ke->key()) {
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
        }
    } else
#endif
        if (e->type() == QEvent::Timer) {
        // should be timerEvent, is here for binary compatibility
        int timerId = ((QTimerEvent*)e)->timerId();
        if (timerId == d->cursorTimer) {
            QStyleOptionFrameV2 opt;
            initStyleOption(&opt);
            if(!hasSelectedText()
               || style()->styleHint(QStyle::SH_BlinkCursorWhenTextSelected, &opt, this))
                d->setCursorVisible(!d->cursorVisible);
#ifndef QT_NO_DRAGANDDROP
        } else if (timerId == d->dndTimer.timerId()) {
            d->drag();
#endif
        }
        else if (timerId == d->tripleClickTimer.timerId())
            d->tripleClickTimer.stop();
#ifdef QT_KEYPAD_NAVIGATION
        else if (timerId == d->deleteAllTimer.timerId()) {
            d->deleteAllTimer.stop();
            clear();
        }
#endif
    } else if (e->type() == QEvent::ContextMenu) {
#ifndef QT_NO_IM
        if (d->composeMode())
            return true;
#endif
        d->separate();
    } else if (e->type() == QEvent::WindowActivate) {
        QTimer::singleShot(0, this, SLOT(_q_handleWindowActivate()));
    }
#ifdef QT_KEYPAD_NAVIGATION
    if (QApplication::keypadNavigationEnabled()) {
        if ((e->type() == QEvent::KeyPress) || (e->type() == QEvent::KeyRelease)) {
            QKeyEvent *ke = (QKeyEvent *)e;
            if (ke->key() == Qt::Key_Back) {
                if (ke->isAutoRepeat()) {
                    // Swallow it. We don't want back keys running amok.
                    ke->accept();
                    return true;
                }
                if ((e->type() == QEvent::KeyRelease)
                    && !isReadOnly()
                    && d->deleteAllTimer.isActive()) {
                    d->deleteAllTimer.stop();
                    backspace();
                    ke->accept();
                    return true;
                }
            }
        } else if (e->type() == QEvent::EnterEditFocus) {
            end(false);
            if (!d->cursorTimer) {
                int cft = QApplication::cursorFlashTime();
                d->cursorTimer = cft ? startTimer(cft/2) : -1;
            }
        } else if (e->type() == QEvent::LeaveEditFocus) {
            d->setCursorVisible(false);
            if (d->cursorTimer > 0)
                killTimer(d->cursorTimer);
            d->cursorTimer = 0;

            if (!d->emitingEditingFinished) {
                if (hasAcceptableInput() || d->fixup()) {
                    d->emitingEditingFinished = true;
                    emit editingFinished();
                    d->emitingEditingFinished = false;
                }
            }
        }
    }
#endif
    return QWidget::event(e);
}

/*! \reimp
*/
void QLineEdit::mousePressEvent(QMouseEvent* e)
{
    Q_D(QLineEdit);
    if (d->sendMouseEventToInputContext(e))
	return;
    if (e->button() == Qt::RightButton)
        return;
#ifdef QT_KEYPAD_NAVIGATION
    if (QApplication::keypadNavigationEnabled() && !hasEditFocus()) {
        setEditFocus(true);
        // Get the completion list to pop up.
        if (d->completer)
            d->completer->complete();
    }
#endif
    if (d->tripleClickTimer.isActive() && (e->pos() - d->tripleClick).manhattanLength() <
         QApplication::startDragDistance()) {
        selectAll();
        return;
    }
    bool mark = e->modifiers() & Qt::ShiftModifier;
    int cursor = d->xToPos(e->pos().x());
#ifndef QT_NO_DRAGANDDROP
    if (!mark && d->dragEnabled && d->echoMode == Normal &&
         e->button() == Qt::LeftButton && d->inSelection(e->pos().x())) {
        d->cursor = cursor;
        update();
        d->dndPos = e->pos();
        if (!d->dndTimer.isActive())
            d->dndTimer.start(QApplication::startDragTime(), this);
        d->emitCursorPositionChanged();
    } else
#endif
    {
        d->moveCursor(cursor, mark);
    }
}

/*! \reimp
*/
void QLineEdit::mouseMoveEvent(QMouseEvent * e)
{
    Q_D(QLineEdit);
    if (d->sendMouseEventToInputContext(e))
	return;

    if (e->buttons() & Qt::LeftButton) {
#ifndef QT_NO_DRAGANDDROP
        if (d->dndTimer.isActive()) {
            if ((d->dndPos - e->pos()).manhattanLength() > QApplication::startDragDistance())
                d->drag();
        } else
#endif
        {
            d->moveCursor(d->xToPos(e->pos().x()), true);
        }
    }
}

/*! \reimp
*/
void QLineEdit::mouseReleaseEvent(QMouseEvent* e)
{
    Q_D(QLineEdit);
    if (d->sendMouseEventToInputContext(e))
	return;
#ifndef QT_NO_DRAGANDDROP
    if (e->button() == Qt::LeftButton) {
        if (d->dndTimer.isActive()) {
            d->dndTimer.stop();
            deselect();
            return;
        }
    }
#endif
#ifndef QT_NO_CLIPBOARD
    if (QApplication::clipboard()->supportsSelection()) {
        if (e->button() == Qt::LeftButton) {
            d->copy(false);
        } else if (!d->readOnly && e->button() == Qt::MidButton) {
            d->deselect();
            insert(QApplication::clipboard()->text(QClipboard::Selection));
        }
    }
#endif
}

/*! \reimp
*/
void QLineEdit::mouseDoubleClickEvent(QMouseEvent* e)
{
    Q_D(QLineEdit);
    if (d->sendMouseEventToInputContext(e))
	return;
    if (e->button() == Qt::LeftButton) {
        deselect();
        d->cursor = d->xToPos(e->pos().x());
        d->cursor = d->textLayout.previousCursorPosition(d->cursor, QTextLayout::SkipWords);
        // ## text layout should support end of words.
        int end = d->textLayout.nextCursorPosition(d->cursor, QTextLayout::SkipWords);
        while (end > d->cursor && d->text[end-1].isSpace())
            --end;
        d->moveCursor(end, true);
        d->tripleClickTimer.start(QApplication::doubleClickInterval(), this);
        d->tripleClick = e->pos();
    }
}

/*!
    \fn void  QLineEdit::returnPressed()

    This signal is emitted when the Return or Enter key is pressed.
    Note that if there is a validator() or inputMask() set on the line
    edit, the returnPressed() signal will only be emitted if the input
    follows the inputMask() and the validator() returns
    QValidator::Acceptable.
*/

/*!
    \fn void  QLineEdit::editingFinished()

    This signal is emitted when the Return or Enter key is pressed or
    the line edit loses focus. Note that if there is a validator() or
    inputMask() set on the line edit and enter/return is pressed, the
    editingFinished() signal will only be emitted if the input follows
    the inputMask() and the validator() returns QValidator::Acceptable.
*/

/*!
    Converts the given key press \a event into a line edit action.

    If Return or Enter is pressed and the current text is valid (or
    can be \link QValidator::fixup() made valid\endlink by the
    validator), the signal returnPressed() is emitted.

    The default key bindings are listed in the class's detailed
    description.
*/

void QLineEdit::keyPressEvent(QKeyEvent *event)
{
    Q_D(QLineEdit);

    bool inlineCompletionAccepted = false;

#ifndef QT_NO_COMPLETER
    if (d->completer) {
        QCompleter::CompletionMode completionMode = d->completer->completionMode();
        if ((completionMode == QCompleter::PopupCompletion
             || completionMode == QCompleter::UnfilteredPopupCompletion)
            &&d->completer->popup()
            && d->completer->popup()->isVisible()) {
            // The following keys are forwarded by the completer to the widget
            // Ignoring the events lets the completer provide suitable default behavior
            switch (event->key()) {
            case Qt::Key_Escape:
                event->ignore();
                return;
            case Qt::Key_Enter:
            case Qt::Key_Return:
            case Qt::Key_F4:
#ifdef QT_KEYPAD_NAVIGATION
            case Qt::Key_Select:
                if (!QApplication::keypadNavigationEnabled())
                    break;
#endif
                d->completer->popup()->hide(); // just hide. will end up propagating to parent
            default:
                break; // normal key processing
            }
        } else if (completionMode == QCompleter::InlineCompletion) {
            switch (event->key()) {
            case Qt::Key_Enter:
            case Qt::Key_Return:
            case Qt::Key_F4:
#ifdef QT_KEYPAD_NAVIGATION
            case Qt::Key_Select:
                if (!QApplication::keypadNavigationEnabled())
                    break;
#endif
                if (!d->completer->currentCompletion().isEmpty() && d->selend > d->selstart
                    && d->selend == d->text.length()) {
                    setText(d->completer->currentCompletion());
                    inlineCompletionAccepted = true;
                }
            default:
                break; // normal key processing
            }
        }
    }
#endif // QT_NO_COMPLETER

#ifdef QT_KEYPAD_NAVIGATION
    bool select = false;
    switch (event->key()) {
        case Qt::Key_Select:
            if (QApplication::keypadNavigationEnabled()) {
                if (hasEditFocus()) {
                    setEditFocus(false);
                    if (d->completer && d->completer->popup()->isVisible())
                        d->completer->popup()->hide();
                    select = true;
                }
            }
            break;
        case Qt::Key_Back:
        case Qt::Key_No:
            if (!QApplication::keypadNavigationEnabled() || !hasEditFocus()) {
                event->ignore();
                return;
            }
            break;
        default:
            if (QApplication::keypadNavigationEnabled()) {
                if (!hasEditFocus() && !(event->modifiers() & Qt::ControlModifier)) {
                    if (!event->text().isEmpty() && event->text().at(0).isPrint()
                        && !isReadOnly())
                    {
                        setEditFocus(true);
                        clear();
                    } else {
                        event->ignore();
                        return;
                    }
                }
            }
    }



    if (QApplication::keypadNavigationEnabled() && !select && !hasEditFocus()) {
        setEditFocus(true);
        if (event->key() == Qt::Key_Select)
            return; // Just start. No action.
    }
#endif

    if (echoMode() == PasswordEchoOnEdit
        && !d->passwordEchoEditing
        && !isReadOnly()
        && !event->text().isEmpty()
#ifdef QT_KEYPAD_NAVIGATION
        && event->key() != Qt::Key_Select
        && event->key() != Qt::Key_Up
        && event->key() != Qt::Key_Down
        && event->key() != Qt::Key_Back
#endif
        && !(event->modifiers() & Qt::ControlModifier)) {
        // Clear the edit and reset to normal echo mode while editing; the
        // echo mode switches back when the edit loses focus.  ### changes a
        // public property, resets current content.  dubious code; you can
        // navigate with keys up, down, back, and select(?), but if you press
        // "left" or "right" it clears?
        d->updatePasswordEchoEditing(true);
        clear();
    }

    d->setCursorVisible(true);
    if (event->key() == Qt::Key_Enter || event->key() == Qt::Key_Return) {
        if (hasAcceptableInput() || d->fixup()) {
            emit returnPressed();
            d->emitingEditingFinished = true;
            emit editingFinished();
            d->emitingEditingFinished = false;
        }
        if (inlineCompletionAccepted)
            event->accept();
        else
            event->ignore();
        return;
    }
    bool unknown = false;

    if (false) {
    }
#ifndef QT_NO_SHORTCUT
    else if (event == QKeySequence::Undo) {
        if (!d->readOnly)
            undo();
    }
    else if (event == QKeySequence::Redo) {
        if (!d->readOnly)
            redo();
    }
    else if (event == QKeySequence::SelectAll) {
        selectAll();
    }
#ifndef QT_NO_CLIPBOARD
    else if (event == QKeySequence::Copy) {
        copy();
    }
    else if (event == QKeySequence::Paste) {
        if (!d->readOnly)
            paste();
    }
    else if (event == QKeySequence::Cut) {
        if (!d->readOnly) {
            cut();
        }
    }
    else if (event == QKeySequence::DeleteEndOfLine) {
        if (!d->readOnly) {
            setSelection(d->cursor, d->text.size());
            copy();
            del();
        }
    }
#endif //QT_NO_CLIPBOARD
    else if (event == QKeySequence::MoveToStartOfLine) {
        home(0);
    }
    else if (event == QKeySequence::MoveToEndOfLine) {
        end(0);
    }
    else if (event == QKeySequence::SelectStartOfLine) {
        home(1);
    }
    else if (event == QKeySequence::SelectEndOfLine) {
        end(1);
    }
    else if (event == QKeySequence::MoveToNextChar) {
#if !defined(Q_WS_WIN) || defined(QT_NO_COMPLETER)
        if (d->hasSelectedText()) {
#else
        if (d->hasSelectedText() && d->completer
            && d->completer->completionMode() == QCompleter::InlineCompletion) {
#endif
            d->moveCursor(d->selend, false);
        } else {
            cursorForward(0, layoutDirection() == Qt::LeftToRight ? 1 : -1);
        }
    }
    else if (event == QKeySequence::SelectNextChar) {
        cursorForward(1, layoutDirection() == Qt::LeftToRight ? 1 : -1);
    }
    else if (event == QKeySequence::MoveToPreviousChar) {
#if !defined(Q_WS_WIN) || defined(QT_NO_COMPLETER)
        if (d->hasSelectedText()) {
#else
        if (d->hasSelectedText() && d->completer
            && d->completer->completionMode() == QCompleter::InlineCompletion) {
#endif
            d->moveCursor(d->selstart, false);
        } else {
            cursorBackward(0, layoutDirection() == Qt::LeftToRight ? 1 : -1);
        }
    }
    else if (event == QKeySequence::SelectPreviousChar) {
        cursorBackward(1, layoutDirection() == Qt::LeftToRight ? 1 : -1);
    }
    else if (event == QKeySequence::MoveToNextWord) {
        if (echoMode() == Normal)
            layoutDirection() == Qt::LeftToRight ? cursorWordForward(0) : cursorWordBackward(0);
        else
            layoutDirection() == Qt::LeftToRight ? end(0) : home(0);
    }
    else if (event == QKeySequence::MoveToPreviousWord) {
        if (echoMode() == Normal)
            layoutDirection() == Qt::LeftToRight ? cursorWordBackward(0) : cursorWordForward(0);
        else if (!d->readOnly) {
            layoutDirection() == Qt::LeftToRight ? home(0) : end(0);
        }
    }
    else if (event == QKeySequence::SelectNextWord) {
        if (echoMode() == Normal)
            layoutDirection() == Qt::LeftToRight ? cursorWordForward(1) : cursorWordBackward(1);
        else
            layoutDirection() == Qt::LeftToRight ? end(1) : home(1);
    }
    else if (event == QKeySequence::SelectPreviousWord) {
        if (echoMode() == Normal)
            layoutDirection() == Qt::LeftToRight ? cursorWordBackward(1) : cursorWordForward(1);
        else
            layoutDirection() == Qt::LeftToRight ? home(1) : end(1);
    }
    else if (event == QKeySequence::Delete) {
        if (!d->readOnly)
            del();
    }
    else if (event == QKeySequence::DeleteEndOfWord) {
        if (!d->readOnly) {
            cursorWordForward(true);
            del();
        }
    }
    else if (event == QKeySequence::DeleteStartOfWord) {
        if (!d->readOnly) {
            cursorWordBackward(true);
            del();
        }
    }
#endif // QT_NO_SHORTCUT
    else {
#ifdef Q_WS_MAC
        if (event->key() == Qt::Key_Up || event->key() == Qt::Key_Down) {
            Qt::KeyboardModifiers myModifiers = (event->modifiers() & ~Qt::KeypadModifier);
            if (myModifiers & Qt::ShiftModifier) {
                if (myModifiers == (Qt::ControlModifier|Qt::ShiftModifier)
                        || myModifiers == (Qt::AltModifier|Qt::ShiftModifier)
                        || myModifiers == Qt::ShiftModifier) {

                    event->key() == Qt::Key_Up ? home(1) : end(1);
                }
            } else {
                if ((myModifiers == Qt::ControlModifier
                     || myModifiers == Qt::AltModifier
                     || myModifiers == Qt::NoModifier)) {
                    event->key() == Qt::Key_Up ? home(0) : end(0);
                }
            }
        }
#endif
        if (event->modifiers() & Qt::ControlModifier) {
            switch (event->key()) {
            case Qt::Key_Backspace:
                if (!d->readOnly) {
                    cursorWordBackward(true);
                    del();
                }
                break;
#ifndef QT_NO_COMPLETER
            case Qt::Key_Up:
            case Qt::Key_Down:
                d->complete(event->key());
                break;
#endif
#if defined(Q_WS_X11)
            case Qt::Key_E:
                end(0);
                break;

            case Qt::Key_U:
                if (!d->readOnly) {
                    setSelection(0, d->text.size());
#ifndef QT_NO_CLIPBOARD
                    copy();
#endif
                    del();
                }
            break;
#endif
            default:
                unknown = true;
            }
        } else { // ### check for *no* modifier
            switch (event->key()) {
            case Qt::Key_Backspace:
                if (!d->readOnly) {
                    backspace();
#ifndef QT_NO_COMPLETER
                    d->complete(Qt::Key_Backspace);
#endif
                }
                break;
#ifdef QT_KEYPAD_NAVIGATION
            case Qt::Key_Back:
                if (QApplication::keypadNavigationEnabled() && !event->isAutoRepeat()
                    && !isReadOnly()) {
                    if (text().length() == 0) {
                        setText(d->origText);

                        if (d->passwordEchoEditing)
                            d->updatePasswordEchoEditing(false);

                        setEditFocus(false);
                    } else if (!d->deleteAllTimer.isActive()) {
                        d->deleteAllTimer.start(750, this);
                    }
                } else {
                    unknown = true;
                }
                break;
#endif

            default:
                unknown = true;
            }
        }
    }

    if (event->key() == Qt::Key_Direction_L || event->key() == Qt::Key_Direction_R) {
        setLayoutDirection((event->key() == Qt::Key_Direction_L) ? Qt::LeftToRight : Qt::RightToLeft);
        d->updateTextLayout();
        update();
        unknown = false;
    }

    if (unknown && !d->readOnly) {
        QString t = event->text();
        if (!t.isEmpty() && t.at(0).isPrint()) {
            insert(t);
#ifndef QT_NO_COMPLETER
            d->complete(event->key());
#endif
            event->accept();
            return;
        }
    }

    if (unknown)
        event->ignore();
    else
        event->accept();
}

/*!
  \since 4.4

  Returns a rectangle that includes the lineedit cursor.
*/
QRect QLineEdit::cursorRect() const
{
    Q_D(const QLineEdit);
    return d->cursorRect();
}

/*!
  This function is not intended as polymorphic usage. Just a shared code
  fragment that calls QInputContext::mouseHandler for this
  class.
*/
bool QLineEditPrivate::sendMouseEventToInputContext( QMouseEvent *e )
{
#if !defined QT_NO_IM
    Q_Q(QLineEdit);
    if ( composeMode() ) {
	int tmp_cursor = xToPos(e->pos().x());
	int mousePos = tmp_cursor - cursor;
	if ( mousePos < 0 || mousePos > textLayout.preeditAreaText().length() ) {
            mousePos = -1;
	    // don't send move events outside the preedit area
            if ( e->type() == QEvent::MouseMove )
                return true;
        }

        QInputContext *qic = q->inputContext();
        if ( qic )
            // may be causing reset() in some input methods
            qic->mouseHandler(mousePos, e);
        if (!textLayout.preeditAreaText().isEmpty())
            return true;
    }
#else
    Q_UNUSED(e);
#endif

    return false;
}

/*! \reimp
 */
void QLineEdit::inputMethodEvent(QInputMethodEvent *e)
{
    Q_D(QLineEdit);
    if (d->readOnly) {
        e->ignore();
        return;
    }

    if (echoMode() == PasswordEchoOnEdit && !d->passwordEchoEditing) {
        // Clear the edit and reset to normal echo mode while entering input
        // method data; the echo mode switches back when the edit loses focus.
        // ### changes a public property, resets current content.
        d->updatePasswordEchoEditing(true);
        clear();
    }

#ifdef QT_KEYPAD_NAVIGATION
    // Focus in if currently in navigation focus on the widget
    // Only focus in on preedits, to allow input methods to
    // commit text as they focus out without interfering with focus
    if (QApplication::keypadNavigationEnabled()
        && hasFocus() && !hasEditFocus()
        && !e->preeditString().isEmpty()) {
        setEditFocus(true);
        selectAll();        // so text is replaced rather than appended to
    }
#endif

    int priorState = d->undoState;
    d->removeSelectedText();

    int c = d->cursor; // cursor position after insertion of commit string
    if (e->replacementStart() <= 0)
        c += e->commitString().length() + qMin(-e->replacementStart(), e->replacementLength());

    d->cursor += e->replacementStart();

    // insert commit string
    if (e->replacementLength()) {
        d->selstart = d->cursor;
        d->selend = d->selstart + e->replacementLength();
        d->removeSelectedText();
    }
    if (!e->commitString().isEmpty())
        d->insert(e->commitString());

    d->cursor = qMin(c, d->text.length());

    d->textLayout.setPreeditArea(d->cursor, e->preeditString());
    d->preeditCursor = e->preeditString().length();
    d->hideCursor = false;
    QList<QTextLayout::FormatRange> formats;
    for (int i = 0; i < e->attributes().size(); ++i) {
        const QInputMethodEvent::Attribute &a = e->attributes().at(i);
        if (a.type == QInputMethodEvent::Cursor) {
            d->preeditCursor = a.start;
            d->hideCursor = !a.length;
        } else if (a.type == QInputMethodEvent::TextFormat) {
            QTextCharFormat f = qvariant_cast<QTextFormat>(a.value).toCharFormat();
            if (f.isValid()) {
                QTextLayout::FormatRange o;
                o.start = a.start + d->cursor;
                o.length = a.length;
                o.format = f;
                formats.append(o);
            }
        }
    }
    d->textLayout.setAdditionalFormats(formats);
    d->updateTextLayout();
    update();
    if (!e->commitString().isEmpty())
        d->emitCursorPositionChanged();
    d->finishChange(priorState);
#ifndef QT_NO_COMPLETER
    if (!e->commitString().isEmpty())
        d->complete(Qt::Key_unknown);
#endif
}

/*!\reimp
*/
QVariant QLineEdit::inputMethodQuery(Qt::InputMethodQuery property) const
{
    Q_D(const QLineEdit);
    switch(property) {
    case Qt::ImMicroFocus:
        return d->cursorRect();
    case Qt::ImFont:
        return font();
    case Qt::ImCursorPosition:
        return QVariant((d->selend - d->selstart == 0) ? d->cursor : d->selend);
    case Qt::ImSurroundingText:
        return QVariant(d->text);
    case Qt::ImCurrentSelection:
        return QVariant(selectedText());
    default:
        return QVariant();
    }
}

/*!\reimp
*/

void QLineEdit::focusInEvent(QFocusEvent *e)
{
    Q_D(QLineEdit);
    if (e->reason() == Qt::TabFocusReason ||
         e->reason() == Qt::BacktabFocusReason  ||
         e->reason() == Qt::ShortcutFocusReason) {
        if (d->maskData)
            d->moveCursor(d->nextMaskBlank(0));
        else if (!d->hasSelectedText())
            selectAll();
    }
#ifdef QT_KEYPAD_NAVIGATION
    if (!QApplication::keypadNavigationEnabled() || (hasEditFocus() && e->reason() == Qt::PopupFocusReason))
#endif
    if (!d->cursorTimer) {
        int cft = QApplication::cursorFlashTime();
        d->cursorTimer = cft ? startTimer(cft/2) : -1;
    }
    QStyleOptionFrameV2 opt;
    initStyleOption(&opt);
    if((!hasSelectedText() && d->textLayout.preeditAreaText().isEmpty())
       || style()->styleHint(QStyle::SH_BlinkCursorWhenTextSelected, &opt, this))
        d->setCursorVisible(true);
#ifdef Q_WS_MAC
    if (d->echoMode == Password || d->echoMode == NoEcho)
        qt_mac_secure_keyboard(true);
#endif
#ifdef QT_KEYPAD_NAVIGATION
    d->origText = d->text;
#endif
#ifndef QT_NO_COMPLETER
    if (d->completer) {
        d->completer->setWidget(this);
        QObject::connect(d->completer, SIGNAL(activated(QString)),
                         this, SLOT(setText(QString)));
        QObject::connect(d->completer, SIGNAL(highlighted(QString)),
                         this, SLOT(_q_completionHighlighted(QString)));
    }
#endif
    update();
}

/*!\reimp
*/

void QLineEdit::focusOutEvent(QFocusEvent *e)
{
    Q_D(QLineEdit);
    if (d->passwordEchoEditing) {
        // Reset the echomode back to PasswordEchoOnEdit when the widget loses
        // focus.
        d->updatePasswordEchoEditing(false);
    }

    Qt::FocusReason reason = e->reason();
    if (reason != Qt::ActiveWindowFocusReason &&
        reason != Qt::PopupFocusReason)
        deselect();

    d->setCursorVisible(false);
    if (d->cursorTimer > 0)
        killTimer(d->cursorTimer);
    d->cursorTimer = 0;

#ifdef QT_KEYPAD_NAVIGATION
    // editingFinished() is already emitted on LeaveEditFocus
    if (!QApplication::keypadNavigationEnabled())
#endif
    if (reason != Qt::PopupFocusReason
        || !(QApplication::activePopupWidget() && QApplication::activePopupWidget()->parentWidget() == this)) {
        if (!d->emitingEditingFinished) {
            if (hasAcceptableInput() || d->fixup()) {
                d->emitingEditingFinished = true;
                emit editingFinished();
                d->emitingEditingFinished = false;
            }
        }
#ifdef QT3_SUPPORT
        emit lostFocus();
#endif
    }
#ifdef Q_WS_MAC
    if (d->echoMode == Password || d->echoMode == NoEcho)
        qt_mac_secure_keyboard(false);
#endif
#ifdef QT_KEYPAD_NAVIGATION
    d->origText = QString();
#endif
#ifndef QT_NO_COMPLETER
    if (d->completer) {
        QObject::disconnect(d->completer, 0, this, 0);
    }
#endif
    update();
}

/*!\reimp
*/
void QLineEdit::paintEvent(QPaintEvent *)
{
    Q_D(QLineEdit);
    QPainter p(this);

    QRect r = rect();
    QPalette pal = palette();

    QStyleOptionFrameV2 panel;
    initStyleOption(&panel);
    style()->drawPrimitive(QStyle::PE_PanelLineEdit, &panel, &p, this);
    r = style()->subElementRect(QStyle::SE_LineEditContents, &panel, this);
    r.setX(r.x() + d->leftTextMargin);
    r.setY(r.y() + d->topTextMargin);
    r.setRight(r.right() - d->rightTextMargin);
    r.setBottom(r.bottom() - d->bottomTextMargin);
    p.setClipRect(r);

    QFontMetrics fm = fontMetrics();
    Qt::Alignment va = QStyle::visualAlignment(layoutDirection(), QFlag(d->alignment));
    switch (va & Qt::AlignVertical_Mask) {
     case Qt::AlignBottom:
         d->vscroll = r.y() + r.height() - fm.height() - verticalMargin;
         break;
     case Qt::AlignTop:
         d->vscroll = r.y() + verticalMargin;
         break;
     default:
         //center
         d->vscroll = r.y() + (r.height() - fm.height() + 1) / 2;
         break;
    }
    QRect lineRect(r.x() + horizontalMargin, d->vscroll, r.width() - 2*horizontalMargin, fm.height());
    QTextLine line = d->textLayout.lineAt(0);

    int cursor = d->cursor;
    if (d->preeditCursor != -1)
        cursor += d->preeditCursor;
    // locate cursor position
    int cix = qRound(line.cursorToX(cursor));

    // horizontal scrolling. d->hscroll is the left indent from the beginning
    // of the text line to the left edge of lineRect. we update this value
    // depending on the delta from the last paint event; in effect this means
    // the below code handles all scrolling based on the textline (widthUsed,
    // minLB, minRB), the line edit rect (lineRect) and the cursor position
    // (cix).
    int minLB = qMax(0, -fm.minLeftBearing());
    int minRB = qMax(0, -fm.minRightBearing());
    int widthUsed = qRound(line.naturalTextWidth()) + 1 + minRB;
    if ((minLB + widthUsed) <=  lineRect.width()) {
        // text fits in lineRect; use hscroll for alignment
        switch (va & ~(Qt::AlignAbsolute|Qt::AlignVertical_Mask)) {
        case Qt::AlignRight:
            d->hscroll = widthUsed - lineRect.width() + 1;
            break;
        case Qt::AlignHCenter:
            d->hscroll = (widthUsed - lineRect.width()) / 2;
            break;
        default:
            // Left
            d->hscroll = 0;
            break;
        }
        d->hscroll -= minLB;
    } else if (cix - d->hscroll >= lineRect.width()) {
        // text doesn't fit, cursor is to the right of lineRect (scroll right)
        d->hscroll = cix - lineRect.width() + 1;
    } else if (cix - d->hscroll < 0 && d->hscroll < widthUsed) {
        // text doesn't fit, cursor is to the left of lineRect (scroll left)
        d->hscroll = cix;
    } else if (widthUsed - d->hscroll < lineRect.width()) {
        // text doesn't fit, text document is to the left of lineRect; align
        // right
        d->hscroll = widthUsed - lineRect.width() + 1;
    }
    // the y offset is there to keep the baseline constant in case we have script changes in the text.
    QPoint topLeft = lineRect.topLeft() - QPoint(d->hscroll, d->ascent - fm.ascent());

    // draw text, selections and cursors
#ifndef QT_NO_STYLE_STYLESHEET
    if (QStyleSheetStyle* cssStyle = qobject_cast<QStyleSheetStyle*>(style())) {
        cssStyle->focusPalette(this, &panel, &pal);
    }
#endif
    p.setPen(pal.text().color());

    QVector<QTextLayout::FormatRange> selections;
#ifdef QT_KEYPAD_NAVIGATION
    if (!QApplication::keypadNavigationEnabled() || hasEditFocus())
#endif
    if (d->selstart < d->selend || (d->cursorVisible && d->maskData && !d->readOnly)) {
        QTextLayout::FormatRange o;
        if (d->selstart < d->selend) {
            o.start = d->selstart;
            o.length = d->selend - d->selstart;
            o.format.setBackground(pal.brush(QPalette::Highlight));
            o.format.setForeground(pal.brush(QPalette::HighlightedText));
        } else {
            // mask selection
            o.start = d->cursor;
            o.length = 1;
            o.format.setBackground(pal.brush(QPalette::Text));
            o.format.setForeground(pal.brush(QPalette::Window));
        }
        selections.append(o);
    }

    // Asian users see an IM selection text as cursor on candidate
    // selection phase of input method, so the ordinary cursor should be
    // invisible if we have a preedit string.
    d->textLayout.draw(&p, topLeft, selections, r);
    if (d->cursorVisible && !d->readOnly && !d->hideCursor)
        d->textLayout.drawCursor(&p, topLeft, cursor, style()->pixelMetric(QStyle::PM_TextCursorWidth));
}


#ifndef QT_NO_DRAGANDDROP
/*!\reimp
*/
void QLineEdit::dragMoveEvent(QDragMoveEvent *e)
{
    Q_D(QLineEdit);
    if (!d->readOnly && e->mimeData()->hasFormat(QLatin1String("text/plain"))) {
        e->acceptProposedAction();
        d->cursor = d->xToPos(e->pos().x());
        d->cursorVisible = true;
        update();
        d->emitCursorPositionChanged();
    }
}

/*!\reimp */
void QLineEdit::dragEnterEvent(QDragEnterEvent * e)
{
    QLineEdit::dragMoveEvent(e);
}

/*!\reimp */
void QLineEdit::dragLeaveEvent(QDragLeaveEvent *)
{
    Q_D(QLineEdit);
    if (d->cursorVisible) {
        d->cursorVisible = false;
        update();
    }
}

/*!\reimp */
void QLineEdit::dropEvent(QDropEvent* e)
{
    Q_D(QLineEdit);
    QString str = e->mimeData()->text();

    if (!str.isNull() && !d->readOnly) {
        if (e->source() == this && e->dropAction() == Qt::CopyAction)
            deselect();
        d->cursor =d->xToPos(e->pos().x());
        int selStart = d->cursor;
        int oldSelStart = d->selstart;
        int oldSelEnd = d->selend;
        d->cursorVisible = false;
        e->acceptProposedAction();
        insert(str);
        if (e->source() == this) {
            if (e->dropAction() == Qt::MoveAction) {
                if (selStart > oldSelStart && selStart <= oldSelEnd)
                    setSelection(oldSelStart, str.length());
                else if (selStart > oldSelEnd)
                    setSelection(selStart - str.length(), str.length());
                else
                    setSelection(selStart, str.length());
            } else {
                setSelection(selStart, str.length());
            }
        }
    } else {
        e->ignore();
        update();
    }
}

void QLineEditPrivate::drag()
{
    Q_Q(QLineEdit);
    dndTimer.stop();
    QMimeData *data = new QMimeData;
    data->setText(q->selectedText());
    QDrag *drag = new QDrag(q);
    drag->setMimeData(data);
    Qt::DropAction action = drag->start();
    if (action == Qt::MoveAction && !readOnly && drag->target() != q) {
        int priorState = undoState;
        removeSelectedText();
        finishChange(priorState);
    }
}

#endif // QT_NO_DRAGANDDROP

#ifndef QT_NO_CONTEXTMENU
/*!
    Shows the standard context menu created with
    createStandardContextMenu().

    If you do not want the line edit to have a context menu, you can set
    its \l contextMenuPolicy to Qt::NoContextMenu. If you want to
    customize the context menu, reimplement this function. If you want
    to extend the standard context menu, reimplement this function, call
    createStandardContextMenu() and extend the menu returned.

    \snippet doc/src/snippets/code/src_gui_widgets_qlineedit.cpp 0

    The \a event parameter is used to obtain the position where
    the mouse cursor was when the event was generated.

    \sa setContextMenuPolicy()
*/
void QLineEdit::contextMenuEvent(QContextMenuEvent *event)
{
    QPointer<QMenu> menu = createStandardContextMenu();
    menu->exec(event->globalPos());
    delete menu;
}

#if defined(Q_WS_WIN)
    extern bool qt_use_rtl_extensions;
#endif

/*!  This function creates the standard context menu which is shown
        when the user clicks on the line edit with the right mouse
        button. It is called from the default contextMenuEvent() handler.
        The popup menu's ownership is transferred to the caller.
*/

QMenu *QLineEdit::createStandardContextMenu()
{
    Q_D(QLineEdit);
    QMenu *popup = new QMenu(this);
    popup->setObjectName(QLatin1String("qt_edit_menu"));

    QAction *action = popup->addAction(QLineEdit::tr("&Undo") + ACCEL_KEY(QKeySequence::Undo));
    action->setEnabled(d->isUndoAvailable());
    connect(action, SIGNAL(triggered()), SLOT(undo()));

    action = popup->addAction(QLineEdit::tr("&Redo") + ACCEL_KEY(QKeySequence::Redo));
    action->setEnabled(d->isRedoAvailable());
    connect(action, SIGNAL(triggered()), SLOT(redo()));

    popup->addSeparator();

#ifndef QT_NO_CLIPBOARD
    action = popup->addAction(QLineEdit::tr("Cu&t") + ACCEL_KEY(QKeySequence::Cut));
    action->setEnabled(!d->readOnly && d->hasSelectedText());
    connect(action, SIGNAL(triggered()), SLOT(cut()));

    action = popup->addAction(QLineEdit::tr("&Copy") + ACCEL_KEY(QKeySequence::Copy));
    action->setEnabled(d->hasSelectedText());
    connect(action, SIGNAL(triggered()), SLOT(copy()));

    action = popup->addAction(QLineEdit::tr("&Paste") + ACCEL_KEY(QKeySequence::Paste));
    action->setEnabled(!d->readOnly && !QApplication::clipboard()->text().isEmpty());
    connect(action, SIGNAL(triggered()), SLOT(paste()));
#endif

    action = popup->addAction(QLineEdit::tr("Delete"));
    action->setEnabled(!d->readOnly && !d->text.isEmpty() && d->hasSelectedText());
    connect(action, SIGNAL(triggered()), SLOT(_q_deleteSelected()));

    popup->addSeparator();

    action = popup->addAction(QLineEdit::tr("Select All") + ACCEL_KEY(QKeySequence::SelectAll));
    action->setEnabled(!d->text.isEmpty() && !d->allSelected());
    d->selectAllAction = action;
    connect(action, SIGNAL(triggered()), SLOT(selectAll()));

#if !defined(QT_NO_IM)
    QInputContext *qic = inputContext();
    if (qic) {
        QList<QAction *> imActions = qic->actions();
        for (int i = 0; i < imActions.size(); ++i)
            popup->addAction(imActions.at(i));
    }
#endif

#if defined(Q_WS_WIN)
    if (!d->readOnly && qt_use_rtl_extensions) {
#else
    if (!d->readOnly) {
#endif
        popup->addSeparator();
        QUnicodeControlCharacterMenu *ctrlCharacterMenu = new QUnicodeControlCharacterMenu(this, popup);
        popup->addMenu(ctrlCharacterMenu);
    }
    return popup;
}
#endif // QT_NO_CONTEXTMENU

/*! \reimp */
void QLineEdit::changeEvent(QEvent *ev)
{
    Q_D(QLineEdit);
    if(ev->type() == QEvent::ActivationChange) {
        if (!palette().isEqual(QPalette::Active, QPalette::Inactive))
            update();
    } else if (ev->type() == QEvent::FontChange
               || ev->type() == QEvent::StyleChange
               || ev->type() == QEvent::LayoutDirectionChange) {
        d->updateTextLayout();
    }
    QWidget::changeEvent(ev);
}

void QLineEditPrivate::_q_clipboardChanged()
{
}

void QLineEditPrivate::_q_handleWindowActivate()
{
    Q_Q(QLineEdit);
    if (!q->hasFocus() && q->hasSelectedText())
        q->deselect();
}

void QLineEditPrivate::_q_deleteSelected()
{
    Q_Q(QLineEdit);
    if (!hasSelectedText())
        return;

    int priorState = undoState;
    q->resetInputContext();
    removeSelectedText();
    separate();
    finishChange(priorState);
}

void QLineEditPrivate::init(const QString& txt)
{
    Q_Q(QLineEdit);
#ifndef QT_NO_CURSOR
    q->setCursor(Qt::IBeamCursor);
#endif
    q->setFocusPolicy(Qt::StrongFocus);
    q->setAttribute(Qt::WA_InputMethodEnabled);
    //   Specifies that this widget can use more, but is able to survive on
    //   less, horizontal space; and is fixed vertically.
    q->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed, QSizePolicy::LineEdit));
    q->setBackgroundRole(QPalette::Base);
    q->setAttribute(Qt::WA_KeyCompression);
    q->setMouseTracking(true);
    q->setAcceptDrops(true);
    text = txt;
    updateTextLayout();
    cursor = text.length();

    q->setAttribute(Qt::WA_MacShowFocusRect);
}

void QLineEditPrivate::updatePasswordEchoEditing(bool editing)
{
    Q_Q(QLineEdit);
    passwordEchoEditing = editing;
    q->setAttribute(Qt::WA_InputMethodEnabled, shouldEnableInputMethod(q));
    updateTextLayout();
    q->update();
}

void QLineEditPrivate::updateTextLayout()
{
    // replace certain non-printable characters with spaces (to avoid
    // drawing boxes when using fonts that don't have glyphs for such
    // characters)
    Q_Q(QLineEdit);
    QString str = q->displayText();
    QChar* uc = str.data();
    for (int i = 0; i < (int)str.length(); ++i) {
        if ((uc[i] < 0x20 && uc[i] != 0x09)
            || uc[i] == QChar::LineSeparator
            || uc[i] == QChar::ParagraphSeparator
            || uc[i] == QChar::ObjectReplacementCharacter)
            uc[i] = QChar(0x0020);
    }
    textLayout.setFont(q->font());
    textLayout.setText(str);
    QTextOption option;
    option.setTextDirection(q->layoutDirection());
    option.setFlags(QTextOption::IncludeTrailingSpaces);
    textLayout.setTextOption(option);

    textLayout.beginLayout();
    QTextLine l = textLayout.createLine();
    textLayout.endLayout();
    ascent = qRound(l.ascent());
}

int QLineEditPrivate::xToPos(int x, QTextLine::CursorPosition betweenOrOn) const
{
    QRect cr = adjustedContentsRect();
    x-= cr.x() - hscroll + horizontalMargin;
    QTextLine l = textLayout.lineAt(0);
    return l.xToCursor(x, betweenOrOn);
}

QRect QLineEditPrivate::cursorRect() const
{
    Q_Q(const QLineEdit);
    QRect cr = adjustedContentsRect();
    int cix = cr.x() - hscroll + horizontalMargin;
    QTextLine l = textLayout.lineAt(0);
    int c = cursor;
    if (preeditCursor != -1)
        c += preeditCursor;
    cix += qRound(l.cursorToX(c));
    int ch = qMin(cr.height(), q->fontMetrics().height() + 1);
    int w = q->style()->pixelMetric(QStyle::PM_TextCursorWidth);
    return QRect(cix-5, vscroll, w + 9, ch);
}

QRect QLineEditPrivate::adjustedContentsRect() const
{
    Q_Q(const QLineEdit);
    QStyleOptionFrameV2 opt;
    q->initStyleOption(&opt);
    QRect r = q->style()->subElementRect(QStyle::SE_LineEditContents, &opt, q);
    r.setX(r.x() + leftTextMargin);
    r.setY(r.y() + topTextMargin);
    r.setRight(r.right() - rightTextMargin);
    r.setBottom(r.bottom() - bottomTextMargin);
    return r;
}

bool QLineEditPrivate::fixup() // this function assumes that validate currently returns != Acceptable
{
#ifndef QT_NO_VALIDATOR
    if (validator) {
        QString textCopy = text;
        int cursorCopy = cursor;
        validator->fixup(textCopy);
        if (validator->validate(textCopy, cursorCopy) == QValidator::Acceptable) {
            if (textCopy != text || cursorCopy != cursor)
                setText(textCopy, cursorCopy);
            return true;
        }
    }
#endif
    return false;
}

void QLineEditPrivate::moveCursor(int pos, bool mark)
{
    Q_Q(QLineEdit);
    if (pos != cursor) {
        separate();
        if (maskData)
            pos = pos > cursor ? nextMaskBlank(pos) : prevMaskBlank(pos);
    }
    bool fullUpdate = mark || hasSelectedText();
    if (mark) {
        int anchor;
        if (selend > selstart && cursor == selstart)
            anchor = selend;
        else if (selend > selstart && cursor == selend)
            anchor = selstart;
        else
            anchor = cursor;
        selstart = qMin(anchor, pos);
        selend = qMax(anchor, pos);
        updateTextLayout();
    } else {
        deselect();
    }
    if (fullUpdate) {
        cursor = pos;
        q->update();
    } else {
        setCursorVisible(false);
        cursor = pos;
        setCursorVisible(true);
        if (!adjustedContentsRect().contains(cursorRect()))
            q->update();
    }
    QStyleOptionFrameV2 opt;
    q->initStyleOption(&opt);
    if (mark && !q->style()->styleHint(QStyle::SH_BlinkCursorWhenTextSelected, &opt, q))
        setCursorVisible(false);
    if (mark || selDirty) {
        selDirty = false;
        emit q->selectionChanged();
    }
    emitCursorPositionChanged();
}

void QLineEditPrivate::finishChange(int validateFromState, bool update, bool edited)
{
    Q_Q(QLineEdit);
    bool lineDirty = selDirty;
    if (textDirty) {
        // do validation
        bool wasValidInput = validInput;
        validInput = true;
#ifndef QT_NO_VALIDATOR
        if (validator) {
            validInput = false;
            QString textCopy = text;
            int cursorCopy = cursor;
            validInput = (validator->validate(textCopy, cursorCopy) != QValidator::Invalid);
            if (validInput) {
                if (text != textCopy) {
                    setText(textCopy, cursorCopy);
                    return;
                }
                cursor = cursorCopy;
            }
        }
#endif
        if (validateFromState >= 0 && wasValidInput && !validInput) {
            undo(validateFromState);
            history.resize(undoState);
            if (modifiedState > undoState)
                modifiedState = -1;
            validInput = true;
            textDirty = false;
        }
        updateTextLayout();
        lineDirty |= textDirty;
        if (textDirty) {
            textDirty = false;
            QString actualText = maskData ? stripString(text) : text;
            if (edited)
                emit q->textEdited(actualText);
            q->updateMicroFocus();
#ifndef QT_NO_COMPLETER
            if (edited && completer && completer->completionMode() != QCompleter::InlineCompletion)
                complete(-1); // update the popup on cut/paste/del
#endif
            emit q->textChanged(actualText);
        }
#ifndef QT_NO_ACCESSIBILITY
        QAccessible::updateAccessibility(q, 0, QAccessible::ValueChanged);
#endif
    }
    if (selDirty) {
        selDirty = false;
        emit q->selectionChanged();
    }
    if (lineDirty || update)
        q->update();
    emitCursorPositionChanged();
}

void QLineEditPrivate::emitCursorPositionChanged()
{
    Q_Q(QLineEdit);
    if (cursor != lastCursorPos) {
        const int oldLast = lastCursorPos;
        lastCursorPos = cursor;
        emit q->cursorPositionChanged(oldLast, cursor);
    }
}

void QLineEditPrivate::setText(const QString& txt, int pos, bool edited)
{
    Q_Q(QLineEdit);
    q->resetInputContext();
    deselect();
    QString oldText = text;
    if (maskData) {
        text = maskString(0, txt, true);
        text += clearString(text.length(), maxLength - text.length());
    } else {
        text = txt.isEmpty() ? txt : txt.left(maxLength);
    }
    history.clear();
    modifiedState =  undoState = 0;
    cursor = (pos < 0 || pos > text.length()) ? text.length() : pos;
    textDirty = (oldText != text);
    finishChange(-1, true, edited);
}


void QLineEditPrivate::setCursorVisible(bool visible)
{
    Q_Q(QLineEdit);
    if ((bool)cursorVisible == visible)
        return;
    if (cursorTimer)
        cursorVisible = visible;
    QRect r = cursorRect();
    if (maskData)
        q->update();
    else
        q->update(r);
}

void QLineEditPrivate::addCommand(const Command& cmd)
{
    if (separator && undoState && history[undoState-1].type != Separator) {
        history.resize(undoState + 2);
        history[undoState++] = Command(Separator, cursor, 0, selstart, selend);
    } else {
        history.resize(undoState + 1);
    }
    separator = false;
    history[undoState++] = cmd;
}

void QLineEditPrivate::insert(const QString& s)
{
    if (hasSelectedText())
        addCommand(Command(SetSelection, cursor, 0, selstart, selend));
    if (maskData) {
        QString ms = maskString(cursor, s);
        for (int i = 0; i < (int) ms.length(); ++i) {
            addCommand (Command(DeleteSelection, cursor+i, text.at(cursor+i), -1, -1));
            addCommand(Command(Insert, cursor+i, ms.at(i), -1, -1));
        }
        text.replace(cursor, ms.length(), ms);
        cursor += ms.length();
        cursor = nextMaskBlank(cursor);
        textDirty = true;
    } else {
        int remaining = maxLength - text.length();
        if (remaining != 0) {
            text.insert(cursor, s.left(remaining));
            for (int i = 0; i < (int) s.left(remaining).length(); ++i)
               addCommand(Command(Insert, cursor++, s.at(i), -1, -1));
            textDirty = true;
        }
    }
}

void QLineEditPrivate::del(bool wasBackspace)
{
    if (cursor < (int) text.length()) {
        if (hasSelectedText())
            addCommand(Command(SetSelection, cursor, 0, selstart, selend));
        addCommand (Command((CommandType)((maskData?2:0)+(wasBackspace?Remove:Delete)), cursor, text.at(cursor), -1, -1));
        if (maskData) {
            text.replace(cursor, 1, clearString(cursor, 1));
            addCommand(Command(Insert, cursor, text.at(cursor), -1, -1));
        } else {
            text.remove(cursor, 1);
        }
        textDirty = true;
    }
}

void QLineEditPrivate::removeSelectedText()
{
    if (selstart < selend && selend <= (int) text.length()) {
        separate();
        int i ;
        addCommand(Command(SetSelection, cursor, 0, selstart, selend));
        if (selstart <= cursor && cursor < selend) {
            // cursor is within the selection. Split up the commands
            // to be able to restore the correct cursor position
            for (i = cursor; i >= selstart; --i)
                addCommand (Command(DeleteSelection, i, text.at(i), -1, 1));
            for (i = selend - 1; i > cursor; --i)
                addCommand (Command(DeleteSelection, i - cursor + selstart - 1, text.at(i), -1, -1));
        } else {
            for (i = selend-1; i >= selstart; --i)
                addCommand (Command(RemoveSelection, i, text.at(i), -1, -1));
        }
        if (maskData) {
            text.replace(selstart, selend - selstart,  clearString(selstart, selend - selstart));
            for (int i = 0; i < selend - selstart; ++i)
                addCommand(Command(Insert, selstart + i, text.at(selstart + i), -1, -1));
        } else {
            text.remove(selstart, selend - selstart);
        }
        if (cursor > selstart)
            cursor -= qMin(cursor, selend) - selstart;
        deselect();
        textDirty = true;

        // adjust hscroll to avoid gap
        const int minRB = qMax(0, -q_func()->fontMetrics().minRightBearing());
        updateTextLayout();
        const QTextLine line = textLayout.lineAt(0);
        const int widthUsed = qRound(line.naturalTextWidth()) + 1 + minRB;
        hscroll = qMin(hscroll, widthUsed);
    }
}

void QLineEditPrivate::parseInputMask(const QString &maskFields)
{
    int delimiter = maskFields.indexOf(QLatin1Char(';'));
    if (maskFields.isEmpty() || delimiter == 0) {
        if (maskData) {
            delete [] maskData;
            maskData = 0;
            maxLength = 32767;
            setText(QString());
        }
        return;
    }

    if (delimiter == -1) {
        blank = QLatin1Char(' ');
        inputMask = maskFields;
    } else {
        inputMask = maskFields.left(delimiter);
        blank = (delimiter + 1 < maskFields.length()) ? maskFields[delimiter + 1] : QLatin1Char(' ');
    }

    // calculate maxLength / maskData length
    maxLength = 0;
    QChar c = 0;
    for (int i=0; i<inputMask.length(); i++) {
        c = inputMask.at(i);
        if (i > 0 && inputMask.at(i-1) == QLatin1Char('\\')) {
            maxLength++;
            continue;
        }
        if (c != QLatin1Char('\\') && c != QLatin1Char('!') &&
             c != QLatin1Char('<') && c != QLatin1Char('>') &&
             c != QLatin1Char('{') && c != QLatin1Char('}') &&
             c != QLatin1Char('[') && c != QLatin1Char(']'))
            maxLength++;
    }

    delete [] maskData;
    maskData = new MaskInputData[maxLength];

    MaskInputData::Casemode m = MaskInputData::NoCaseMode;
    c = 0;
    bool s;
    bool escape = false;
    int index = 0;
    for (int i = 0; i < inputMask.length(); i++) {
        c = inputMask.at(i);
        if (escape) {
            s = true;
            maskData[index].maskChar = c;
            maskData[index].separator = s;
            maskData[index].caseMode = m;
            index++;
            escape = false;
        } else if (c == QLatin1Char('<')) {
                m = MaskInputData::Lower;
        } else if (c == QLatin1Char('>')) {
            m = MaskInputData::Upper;
        } else if (c == QLatin1Char('!')) {
            m = MaskInputData::NoCaseMode;
        } else if (c != QLatin1Char('{') && c != QLatin1Char('}') && c != QLatin1Char('[') && c != QLatin1Char(']')) {
            switch (c.unicode()) {
            case 'A':
            case 'a':
            case 'N':
            case 'n':
            case 'X':
            case 'x':
            case '9':
            case '0':
            case 'D':
            case 'd':
            case '#':
            case 'H':
            case 'h':
            case 'B':
            case 'b':
                s = false;
                break;
            case '\\':
                escape = true;
            default:
                s = true;
                break;
            }

            if (!escape) {
                maskData[index].maskChar = c;
                maskData[index].separator = s;
                maskData[index].caseMode = m;
                index++;
            }
        }
    }
    setText(text);
}


/* checks if the key is valid compared to the inputMask */
bool QLineEditPrivate::isValidInput(QChar key, QChar mask) const
{
    switch (mask.unicode()) {
    case 'A':
        if (key.isLetter())
            return true;
        break;
    case 'a':
        if (key.isLetter() || key == blank)
            return true;
        break;
    case 'N':
        if (key.isLetterOrNumber())
            return true;
        break;
    case 'n':
        if (key.isLetterOrNumber() || key == blank)
            return true;
        break;
    case 'X':
        if (key.isPrint())
            return true;
        break;
    case 'x':
        if (key.isPrint() || key == blank)
            return true;
        break;
    case '9':
        if (key.isNumber())
            return true;
        break;
    case '0':
        if (key.isNumber() || key == blank)
            return true;
        break;
    case 'D':
        if (key.isNumber() && key.digitValue() > 0)
            return true;
        break;
    case 'd':
        if ((key.isNumber() && key.digitValue() > 0) || key == blank)
            return true;
        break;
    case '#':
        if (key.isNumber() || key == QLatin1Char('+') || key == QLatin1Char('-') || key == blank)
            return true;
        break;
    case 'B':
        if (key == QLatin1Char('0') || key == QLatin1Char('1'))
            return true;
        break;
    case 'b':
        if (key == QLatin1Char('0') || key == QLatin1Char('1') || key == blank)
            return true;
        break;
    case 'H':
        if (key.isNumber() || (key >= QLatin1Char('a') && key <= QLatin1Char('f')) || (key >= QLatin1Char('A') && key <= QLatin1Char('F')))
            return true;
        break;
    case 'h':
        if (key.isNumber() || (key >= QLatin1Char('a') && key <= QLatin1Char('f')) || (key >= QLatin1Char('A') && key <= QLatin1Char('F')) || key == blank)
            return true;
        break;
    default:
        break;
    }
    return false;
}

bool QLineEditPrivate::hasAcceptableInput(const QString &str) const
{
#ifndef QT_NO_VALIDATOR
    QString textCopy = str;
    int cursorCopy = cursor;
    if (validator && validator->validate(textCopy, cursorCopy)
        != QValidator::Acceptable)
        return false;
#endif

    if (!maskData)
        return true;

    if (str.length() != maxLength)
        return false;

    for (int i=0; i < maxLength; ++i) {
        if (maskData[i].separator) {
            if (str.at(i) != maskData[i].maskChar)
                return false;
        } else {
            if (!isValidInput(str.at(i), maskData[i].maskChar))
                return false;
        }
    }
    return true;
}

/*
  Applies the inputMask on \a str starting from position \a pos in the mask. \a clear
  specifies from where characters should be gotten when a separator is met in \a str - true means
  that blanks will be used, false that previous input is used.
  Calling this when no inputMask is set is undefined.
*/
QString QLineEditPrivate::maskString(uint pos, const QString &str, bool clear) const
{
    if (pos >= (uint)maxLength)
        return QString::fromLatin1("");

    QString fill;
    fill = clear ? clearString(0, maxLength) : text;

    int strIndex = 0;
    QString s = QString::fromLatin1("");
    int i = pos;
    while (i < maxLength) {
        if (strIndex < str.length()) {
            if (maskData[i].separator) {
                s += maskData[i].maskChar;
                if (str[(int)strIndex] == maskData[i].maskChar)
                    strIndex++;
                ++i;
            } else {
                if (isValidInput(str[(int)strIndex], maskData[i].maskChar)) {
                    switch (maskData[i].caseMode) {
                    case MaskInputData::Upper:
                        s += str[(int)strIndex].toUpper();
                        break;
                    case MaskInputData::Lower:
                        s += str[(int)strIndex].toLower();
                        break;
                    default:
                        s += str[(int)strIndex];
                    }
                    ++i;
                } else {
                    // search for separator first
                    int n = findInMask(i, true, true, str[(int)strIndex]);
                    if (n != -1) {
                        if (str.length() != 1 || i == 0 || (i > 0 && (!maskData[i-1].separator || maskData[i-1].maskChar != str[(int)strIndex]))) {
                            s += fill.mid(i, n-i+1);
                            i = n + 1; // update i to find + 1
                        }
                    } else {
                        // search for valid blank if not
                        n = findInMask(i, true, false, str[(int)strIndex]);
                        if (n != -1) {
                            s += fill.mid(i, n-i);
                            switch (maskData[n].caseMode) {
                            case MaskInputData::Upper:
                                s += str[(int)strIndex].toUpper();
                                break;
                            case MaskInputData::Lower:
                                s += str[(int)strIndex].toLower();
                                break;
                            default:
                                s += str[(int)strIndex];
                            }
                            i = n + 1; // updates i to find + 1
                        }
                    }
                }
                strIndex++;
            }
        } else
            break;
    }

    return s;
}



/*
  Returns a "cleared" string with only separators and blank chars.
  Calling this when no inputMask is set is undefined.
*/
QString QLineEditPrivate::clearString(uint pos, uint len) const
{
    if (pos >= (uint)maxLength)
        return QString();

    QString s;
    int end = qMin((uint)maxLength, pos + len);
    for (int i=pos; i<end; i++)
        if (maskData[i].separator)
            s += maskData[i].maskChar;
        else
            s += blank;

    return s;
}

/*
  Strips blank parts of the input in a QLineEdit when an inputMask is set,
  separators are still included. Typically "127.0__.0__.1__" becomes "127.0.0.1".
*/
QString QLineEditPrivate::stripString(const QString &str) const
{
    if (!maskData)
        return str;

    QString s;
    int end = qMin(maxLength, (int)str.length());
    for (int i=0; i < end; i++)
        if (maskData[i].separator)
            s += maskData[i].maskChar;
        else
            if (str[i] != blank)
                s += str[i];

    return s;
}

/* searches forward/backward in maskData for either a separator or a blank */
int QLineEditPrivate::findInMask(int pos, bool forward, bool findSeparator, QChar searchChar) const
{
    if (pos >= maxLength || pos < 0)
        return -1;

    int end = forward ? maxLength : -1;
    int step = forward ? 1 : -1;
    int i = pos;

    while (i != end) {
        if (findSeparator) {
            if (maskData[i].separator && maskData[i].maskChar == searchChar)
                return i;
        } else {
            if (!maskData[i].separator) {
                if (searchChar.isNull())
                    return i;
                else if (isValidInput(searchChar, maskData[i].maskChar))
                    return i;
            }
        }
        i += step;
    }
    return -1;
}

void QLineEditPrivate::undo(int until)
{
    if (!isUndoAvailable())
        return;
    deselect();
    while (undoState && undoState > until) {
        Command& cmd = history[--undoState];
        switch (cmd.type) {
        case Insert:
            text.remove(cmd.pos, 1);
            cursor = cmd.pos;
            break;
        case SetSelection:
            selstart = cmd.selStart;
            selend = cmd.selEnd;
            cursor = cmd.pos;
            break;
        case Remove:
        case RemoveSelection:
            text.insert(cmd.pos, cmd.uc);
            cursor = cmd.pos + 1;
            break;
        case Delete:
        case DeleteSelection:
            text.insert(cmd.pos, cmd.uc);
            cursor = cmd.pos;
            break;
        case Separator:
            continue;
        }
        if (until < 0 && undoState) {
            Command& next = history[undoState-1];
            if (next.type != cmd.type && next.type < RemoveSelection
                 && (cmd.type < RemoveSelection || next.type == Separator))
                break;
        }
    }
    textDirty = true;
    emitCursorPositionChanged();
}

void QLineEditPrivate::redo() {
    if (!isRedoAvailable())
        return;
    deselect();
    while (undoState < (int)history.size()) {
        Command& cmd = history[undoState++];
        switch (cmd.type) {
        case Insert:
            text.insert(cmd.pos, cmd.uc);
            cursor = cmd.pos + 1;
            break;
        case SetSelection:
            selstart = cmd.selStart;
            selend = cmd.selEnd;
            cursor = cmd.pos;
            break;
        case Remove:
        case Delete:
        case RemoveSelection:
        case DeleteSelection:
            text.remove(cmd.pos, 1);
            cursor = cmd.pos;
            break;
        case Separator:
            selstart = cmd.selStart;
            selend = cmd.selEnd;
            cursor = cmd.pos;
            break;
        }
        if (undoState < (int)history.size()) {
            Command& next = history[undoState];
            if (next.type != cmd.type && cmd.type < RemoveSelection && next.type != Separator
                 && (next.type < RemoveSelection || cmd.type == Separator))
                break;
        }
    }
    textDirty = true;
    emitCursorPositionChanged();
}

/*!
    \fn void QLineEdit::repaintArea(int a, int b)

    Use update() instead.
*/

/*!
    \fn void QLineEdit::cursorLeft(bool mark, int steps)

    Use cursorForward() with a negative number of steps instead. For
    example, cursorForward(mark, -steps).
*/

/*!
    \fn void QLineEdit::cursorRight(bool mark, int steps)

    Use cursorForward() instead.
*/

/*!
    \fn bool QLineEdit::frame() const

    Use hasFrame() instead.
*/

/*!
    \fn void QLineEdit::clearValidator()

    Use setValidator(0) instead.
*/

/*!
    \fn bool QLineEdit::hasMarkedText() const

    Use hasSelectedText() instead.
*/

/*!
    \fn QString QLineEdit::markedText() const

    Use selectedText() instead.
*/

/*!
    \fn void QLineEdit::setFrameRect(QRect)
    \internal
*/

/*!
    \fn QRect QLineEdit::frameRect() const
    \internal
*/
/*!
    \enum QLineEdit::DummyFrame
    \internal

    \value Box
    \value Sunken
    \value Plain
    \value Raised
    \value MShadow
    \value NoFrame
    \value Panel
    \value StyledPanel
    \value HLine
    \value VLine
    \value GroupBoxPanel
    \value WinPanel
    \value ToolBarPanel
    \value MenuBarPanel
    \value PopupPanel
    \value LineEditPanel
    \value TabWidgetPanel
    \value MShape
*/

/*!
    \fn void QLineEdit::setFrameShadow(DummyFrame)
    \internal
*/

/*!
    \fn DummyFrame QLineEdit::frameShadow() const
    \internal
*/

/*!
    \fn void QLineEdit::setFrameShape(DummyFrame)
    \internal
*/

/*!
    \fn DummyFrame QLineEdit::frameShape() const
    \internal
*/

/*!
    \fn void QLineEdit::setFrameStyle(int)
    \internal
*/

/*!
    \fn int QLineEdit::frameStyle() const
    \internal
*/

/*!
    \fn int QLineEdit::frameWidth() const
    \internal
*/

/*!
    \fn void QLineEdit::setLineWidth(int)
    \internal
*/

/*!
    \fn int QLineEdit::lineWidth() const
    \internal
*/

/*!
    \fn void QLineEdit::setMargin(int margin)
    Sets the width of the margin around the contents of the widget to \a margin.

    Use QWidget::setContentsMargins() instead.
    \sa margin(), QWidget::setContentsMargins()
*/

/*!
    \fn int QLineEdit::margin() const
    Returns the with of the the margin around the contents of the widget.

    Use QWidget::getContentsMargins() instead.
    \sa setMargin(), QWidget::getContentsMargins()
*/

/*!
    \fn void QLineEdit::setMidLineWidth(int)
    \internal
*/

/*!
    \fn int QLineEdit::midLineWidth() const
    \internal
*/

QT_END_NAMESPACE

#include "moc_qlineedit.cpp"

#endif // QT_NO_LINEEDIT
