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

#ifndef QLINEEDIT_P_H
#define QLINEEDIT_P_H

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

#include "QtCore/qglobal.h"

#ifndef QT_NO_LINEEDIT
#include "private/qwidget_p.h"
#include "QtGui/qlineedit.h"
#include "QtGui/qtextlayout.h"
#include "QtGui/qstyleoption.h"
#include "QtCore/qbasictimer.h"
#include "QtGui/qcompleter.h"
#include "QtCore/qpointer.h"
#include "QtGui/qlineedit.h"

QT_BEGIN_NAMESPACE

class QLineEditPrivate : public QWidgetPrivate
{
    Q_DECLARE_PUBLIC(QLineEdit)
public:

    QLineEditPrivate()
        : cursor(0), preeditCursor(0), cursorTimer(0), frame(1),
          cursorVisible(0), hideCursor(false), separator(0), readOnly(0),
          dragEnabled(0), contextMenuEnabled(1), echoMode(0), textDirty(0),
          selDirty(0), validInput(1), alignment(Qt::AlignLeading | Qt::AlignVCenter), ascent(0),
          maxLength(32767), hscroll(0), vscroll(0), lastCursorPos(-1), maskData(0),
          modifiedState(0), undoState(0), selstart(0), selend(0), userInput(false),
          emitingEditingFinished(false), passwordEchoEditing(false)
#ifndef QT_NO_COMPLETER
        , completer(0)
#endif
        , leftTextMargin(0), topTextMargin(0), rightTextMargin(0), bottomTextMargin(0)
        {
        }

    ~QLineEditPrivate()
    {
        delete [] maskData;
    }
    void init(const QString&);

    QString text;
    int cursor;
    int preeditCursor;
    int cursorTimer; // -1 for non blinking cursor.
    QPoint tripleClick;
    QBasicTimer tripleClickTimer;
    uint frame : 1;
    uint cursorVisible : 1;
    uint hideCursor : 1; // used to hide the cursor inside preedit areas
    uint separator : 1;
    uint readOnly : 1;
    uint dragEnabled : 1;
    uint contextMenuEnabled : 1;
    uint echoMode : 2;
    uint textDirty : 1;
    uint selDirty : 1;
    uint validInput : 1;
    uint alignment;
    int ascent;
    int maxLength;
    int hscroll;
    int vscroll;
    int lastCursorPos;

#ifndef QT_NO_CONTEXTMENU
    QPointer<QAction> selectAllAction;
#endif

    inline void emitCursorPositionChanged();
    bool sendMouseEventToInputContext(QMouseEvent *e);

    void finishChange(int validateFromState = -1, bool update = false, bool edited = true);

    QPointer<QValidator> validator;
    struct MaskInputData {
        enum Casemode { NoCaseMode, Upper, Lower };
        QChar maskChar; // either the separator char or the inputmask
        bool separator;
        Casemode caseMode;
    };
    QString inputMask;
    QChar blank;
    MaskInputData *maskData;
    inline int nextMaskBlank(int pos) {
        int c = findInMask(pos, true, false);
        separator |= (c != pos);
        return (c != -1 ?  c : maxLength);
    }
    inline int prevMaskBlank(int pos) {
        int c = findInMask(pos, false, false);
        separator |= (c != pos);
        return (c != -1 ? c : 0);
    }

    void setCursorVisible(bool visible);


    // undo/redo handling
    enum CommandType { Separator, Insert, Remove, Delete, RemoveSelection, DeleteSelection, SetSelection };
    struct Command {
        inline Command() {}
        inline Command(CommandType t, int p, QChar c, int ss, int se) : type(t),uc(c),pos(p),selStart(ss),selEnd(se) {}
        uint type : 4;
        QChar uc;
        int pos, selStart, selEnd;
    };
    int modifiedState;
    int undoState;
    QVector<Command> history;
    void addCommand(const Command& cmd);
    void insert(const QString& s);
    void del(bool wasBackspace = false);
    void remove(int pos);

    inline void separate() { separator = true; }
    void undo(int until = -1);
    void redo();
    inline bool isUndoAvailable() const { return !readOnly && undoState; }
    inline bool isRedoAvailable() const { return !readOnly && undoState < (int)history.size(); }

    // selection
    int selstart, selend;
    inline bool allSelected() const { return !text.isEmpty() && selstart == 0 && selend == (int)text.length(); }
    inline bool hasSelectedText() const { return !text.isEmpty() && selend > selstart; }
    inline void deselect() { selDirty |= (selend > selstart); selstart = selend = 0; }
    void removeSelectedText();
#ifndef QT_NO_CLIPBOARD
    void copy(bool clipboard = true) const;
#endif
    inline bool inSelection(int x) const
    { if (selstart >= selend) return false;
    int pos = xToPos(x, QTextLine::CursorOnCharacter);  return pos >= selstart && pos < selend; }

    // masking
    void parseInputMask(const QString &maskFields);
    bool isValidInput(QChar key, QChar mask) const;
    bool hasAcceptableInput(const QString &text) const;
    QString maskString(uint pos, const QString &str, bool clear = false) const;
    QString clearString(uint pos, uint len) const;
    QString stripString(const QString &str) const;
    int findInMask(int pos, bool forward, bool findSeparator, QChar searchChar = QChar()) const;

    // input methods
    bool composeMode() const { return !textLayout.preeditAreaText().isEmpty(); }

    // complex text layout
    QTextLayout textLayout;
    void updateTextLayout();
    void moveCursor(int pos, bool mark = false);
    void setText(const QString& txt, int pos = -1, bool edited = true);
    int xToPos(int x, QTextLine::CursorPosition = QTextLine::CursorBetweenCharacters) const;
    QRect cursorRect() const;
    bool fixup();

    QRect adjustedContentsRect() const;

#ifndef QT_NO_DRAGANDDROP
    // drag and drop
    QPoint dndPos;
    QBasicTimer dndTimer;
    void drag();
#endif

    void _q_clipboardChanged();
    void _q_handleWindowActivate();
    void _q_deleteSelected();
    bool userInput;
    bool emitingEditingFinished;

#ifdef QT_KEYPAD_NAVIGATION
    QBasicTimer deleteAllTimer; // keypad navigation
    QString origText;
#endif

    bool passwordEchoEditing;
    void updatePasswordEchoEditing(bool editing);

#ifndef QT_NO_COMPLETER
    QPointer<QCompleter> completer;
    void complete(int key = -1);
    void _q_completionHighlighted(QString);
    bool advanceToEnabledItem(int n);
#endif

    int leftTextMargin;
    int topTextMargin;
    int rightTextMargin;
    int bottomTextMargin;
};

#endif // QT_NO_LINEEDIT

QT_END_NAMESPACE

#endif // QLINEEDIT_P_H
