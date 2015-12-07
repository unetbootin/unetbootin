/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the QtGui module of the Qt Toolkit.
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
          emitingEditingFinished(false), resumePassword(false)
#ifndef QT_NO_COMPLETER
        , completer(0)
#endif
        {
#ifndef QT_NO_MENU
            actions[UndoAct] = 0;
#endif
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

    enum { UndoAct, RedoAct, CutAct, CopyAct, PasteAct, ClearAct, SelectAllAct, NCountActs };
#ifndef QT_NO_MENU
    QAction *actions[NCountActs];
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

    bool resumePassword;

#ifndef QT_NO_COMPLETER
    QPointer<QCompleter> completer;
    void complete(int key = -1);
    void _q_completionHighlighted(QString);
    bool advanceToEnabledItem(int n);
#endif
};

#endif // QT_NO_LINEEDIT

QT_END_NAMESPACE

#endif // QLINEEDIT_P_H
