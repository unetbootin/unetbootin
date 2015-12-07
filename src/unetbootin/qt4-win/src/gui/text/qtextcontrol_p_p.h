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

#ifndef QTEXTCONTROL_P_P_H
#define QTEXTCONTROL_P_P_H

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

#include "QtGui/qtextdocumentfragment.h"
#include "QtGui/qscrollbar.h"
#include "QtGui/qtextcursor.h"
#include "QtGui/qtextformat.h"
#include "QtGui/qmenu.h"
#include "QtGui/qabstracttextdocumentlayout.h"
#include "QtCore/qbasictimer.h"
#include "QtCore/qpointer.h"
#include "private/qobject_p.h"

QT_BEGIN_NAMESPACE

class QMimeData;
class QAbstractScrollArea;

class QTextControlPrivate : public QObjectPrivate
{
    Q_DECLARE_PUBLIC(QTextControl)
public:
    QTextControlPrivate();

    bool cursorMoveKeyEvent(QKeyEvent *e);

    void updateCurrentCharFormat();

    void indent();
    void outdent();

    void gotoNextTableCell();
    void gotoPreviousTableCell();

    void createAutoBulletList();

    void init(Qt::TextFormat format = Qt::RichText, const QString &text = QString(),
              QTextDocument *document = 0);
    void setContent(Qt::TextFormat format = Qt::RichText, const QString &text = QString(),
                    QTextDocument *document = 0);
    void startDrag();

    void paste(const QMimeData *source);

    void setCursorPosition(const QPointF &pos);
    void setCursorPosition(int pos, QTextCursor::MoveMode mode = QTextCursor::MoveAnchor);

    void repaintCursor();
    inline void repaintSelection()
    { repaintOldAndNewSelection(QTextCursor()); }
    void repaintOldAndNewSelection(const QTextCursor &oldSelection);

    void selectionChanged(bool forceEmitSelectionChanged = false);

    void _q_updateCurrentCharFormatAndSelection();

#ifndef QT_NO_CLIPBOARD
    void setClipboardSelection();
#endif

    void _q_emitCursorPosChanged(const QTextCursor &someCursor);

    void setBlinkingCursorEnabled(bool enable);

    void extendWordwiseSelection(int suggestedNewPosition, qreal mouseXPosition);
    void extendLinewiseSelection(int suggestedNewPosition);

    void _q_deleteSelected();

    void _q_setCursorAfterUndoRedo(int undoPosition, int charsAdded, int charsRemoved);

    QRectF rectForPosition(int position) const;
    QRectF selectionRect(const QTextCursor &cursor) const;
    inline QRectF selectionRect() const
    { return selectionRect(this->cursor); }

    QString anchorForCursor(const QTextCursor &anchor) const;

    void keyPressEvent(QKeyEvent *e);
    void mousePressEvent(Qt::MouseButton button, const QPointF &pos,
                         Qt::KeyboardModifiers modifiers,
                         Qt::MouseButtons buttons,
                         const QPoint &globalPos = QPoint());
    void mouseMoveEvent(Qt::MouseButtons buttons, const QPointF &pos);
    void mouseReleaseEvent(Qt::MouseButton button, const QPointF &pos);
    void mouseDoubleClickEvent(QEvent *e, Qt::MouseButton button, const QPointF &pos);
    void contextMenuEvent(const QPoint &screenPos, const QPointF &docPos, QWidget *contextWidget);
    void focusEvent(QFocusEvent *e);
#ifdef QT_KEYPAD_NAVIGATION
    void editFocusEvent(QEvent *e);
#endif
    bool dragEnterEvent(QEvent *e, const QMimeData *mimeData);
    void dragLeaveEvent();
    bool dragMoveEvent(QEvent *e, const QMimeData *mimeData, const QPointF &pos);
    bool dropEvent(const QMimeData *mimeData, const QPointF &pos, Qt::DropAction dropAction, QWidget *source);

    void inputMethodEvent(QInputMethodEvent *);

    void activateLinkUnderCursor(QString href = QString());

#ifndef QT_NO_TOOLTIP
    void showToolTip(const QPoint &globalPos, const QPointF &pos, QWidget *contextWidget);
#endif

    void append(const QString &text, Qt::TextFormat format = Qt::AutoText);

    QTextDocument *doc;
    bool cursorOn;
    QTextCursor cursor;
    bool cursorIsFocusIndicator;
    QTextCharFormat lastCharFormat;

    QTextCursor dndFeedbackCursor;

    Qt::TextInteractionFlags interactionFlags;

    QBasicTimer cursorBlinkTimer;
    QBasicTimer trippleClickTimer;
    QPointF trippleClickPoint;

    bool mousePressed;

    bool mightStartDrag;
    QPoint dragStartPos;
    QPointer<QWidget> contextWidget;

    bool lastSelectionState;

    bool ignoreAutomaticScrollbarAdjustement;

    QTextCursor selectedWordOnDoubleClick;
    QTextCursor selectedLineOnDoubleClick;

    bool overwriteMode;
    bool acceptRichText;

    int preeditCursor;
    bool hideCursor; // used to hide the cursor in the preedit area

    QVector<QAbstractTextDocumentLayout::Selection> extraSelections;

    QPalette palette;
    bool hasFocus;
#ifdef QT_KEYPAD_NAVIGATION
    bool hasEditFocus;
#endif
    Qt::LayoutDirection layoutDirection;
    bool isEnabled;

    QString highlightedAnchor; // Anchor below cursor
    QString anchorOnMousePress;
    bool hadSelectionOnMousePress;

    bool openExternalLinks;

    QString linkToCopy;
    void _q_copyLink();
    void _q_updateBlock(const QTextBlock &);
    void _q_documentLayoutChanged();
};

QT_END_NAMESPACE

#endif // QTEXTCONTROL_P_H
