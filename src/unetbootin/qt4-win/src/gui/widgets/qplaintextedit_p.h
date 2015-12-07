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

#ifndef QPLAINTEXTEDIT_P_H
#define QPLAINTEXTEDIT_P_H

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

#include "private/qabstractscrollarea_p.h"
#include "QtGui/qtextdocumentfragment.h"
#include "QtGui/qscrollbar.h"
#include "QtGui/qtextcursor.h"
#include "QtGui/qtextformat.h"
#include "QtGui/qmenu.h"
#include "QtGui/qabstracttextdocumentlayout.h"
#include "QtCore/qbasictimer.h"
#include "private/qtextcontrol_p.h"
#include "qplaintextedit.h"

#ifndef QT_NO_TEXTEDIT

QT_BEGIN_NAMESPACE

#define MARGIN 4
class QMimeData;

class QPlainTextEdit;
class ExtraArea;

class QPlainTextEditControl : public QTextControl
{
    Q_OBJECT
public:
    QPlainTextEditControl(QPlainTextEdit *parent);


    QMimeData *createMimeDataFromSelection() const;
    bool canInsertFromMimeData(const QMimeData *source) const;
    void insertFromMimeData(const QMimeData *source);
    int hitTest(const QPointF &point, Qt::HitTestAccuracy = Qt::FuzzyHit) const;
    QRectF blockBoundingRect(const QTextBlock &block) const;
    inline QRectF cursorRect(const QTextCursor &cursor) const {
        QRectF r = QTextControl::cursorRect(cursor);
        r.setLeft(qMax(r.left(), (qreal) 0.));
        return r;
    }
    inline QRectF cursorRect() { return cursorRect(textCursor()); }
    void ensureCursorVisible() { textEdit->ensureCursorVisible(); }


    QPlainTextEdit *textEdit;
    int topBlock;
    QTextBlock firstVisibleBlock() const;

    QVariant loadResource(int type, const QUrl &name) {
        return textEdit->loadResource(type, name);
    }

};


class QPlainTextEditPrivate : public QAbstractScrollAreaPrivate
{
    Q_DECLARE_PUBLIC(QPlainTextEdit)
public:
    QPlainTextEditPrivate();

    void init(const QString &txt = QString());
    void _q_repaintContents(const QRectF &contentsRect);

    inline QPoint mapToContents(const QPoint &point) const
        { return QPoint(point.x() + horizontalOffset(), point.y() + verticalOffset()); }

    void _q_adjustScrollbars();
    void _q_verticalScrollbarActionTriggered(int action, int n = 1);
    void ensureViewportLayouted();
    void relayoutDocument();

    void pageUpDown(QTextCursor::MoveOperation op, QTextCursor::MoveMode moveMode, bool moveCursor = true);

    inline int horizontalOffset() const
        { return (q_func()->isRightToLeft() ? (hbar->maximum() - hbar->value()) : hbar->value()); }
    int verticalOffset(int topBlock, int topLine) const;
    int verticalOffset() const;

    inline void sendControlEvent(QEvent *e)
        { control->processEvent(e, QPointF(horizontalOffset(), verticalOffset()), viewport); }

    void updateDefaultTextOption();

    QPlainTextEditControl *control;

    bool tabChangesFocus;

    QBasicTimer autoScrollTimer;
    QPoint autoScrollDragPos;

    QPlainTextEdit::LineWrapMode lineWrap;
    QTextOption::WrapMode wordWrap;

    uint showCursorOnInitialShow : 1;
    uint backgroundVisible : 1;
    uint centerOnScroll : 1;
    uint inDrag : 1;

    int topLine;
    int lastTopLineInMaximumBlock;

    void setTopBlock(int newTopBlock, int newTopLine);

    int lineCount(int untilBlockNumber = -1) const;
    QPair<int,int> blockForLineNumber(int lineNumber) const;

    void ensureVisible(int position, bool center, bool forceCenter = false);
    void ensureCursorVisible(bool center = false);
    void updateViewport();

    QPointer<QPlainTextDocumentLayout> documentLayoutPtr;

    void append(const QString &text, Qt::TextFormat format = Qt::AutoText);

    qreal pageUpDownLastCursorY;
    bool pageUpDownLastCursorYIsValid;


#ifdef QT_KEYPAD_NAVIGATION
    QBasicTimer deleteAllTimer;
#endif

    void _q_cursorPositionChanged();

    void _q_modificationChanged(bool);
};

QT_END_NAMESPACE

#endif // QT_NO_TEXTEDIT

#endif // QPLAINTEXTEDIT_P_H
