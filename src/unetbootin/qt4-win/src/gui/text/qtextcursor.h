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

#ifndef QTEXTCURSOR_H
#define QTEXTCURSOR_H

#include <QtCore/qstring.h>
#include <QtCore/qshareddata.h>
#include <QtGui/qtextformat.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Gui)

class QTextDocument;
class QTextCursorPrivate;
class QTextDocumentFragment;
class QTextCharFormat;
class QTextBlockFormat;
class QTextListFormat;
class QTextTableFormat;
class QTextFrameFormat;
class QTextImageFormat;
class QTextDocumentPrivate;
class QTextList;
class QTextTable;
class QTextFrame;
class QTextBlock;

class Q_GUI_EXPORT QTextCursor
{
public:
    QTextCursor();
    explicit QTextCursor(QTextDocument *document);
    QTextCursor(QTextDocumentPrivate *p, int pos);
    explicit QTextCursor(QTextFrame *frame);
    explicit QTextCursor(const QTextBlock &block);
    explicit QTextCursor(QTextCursorPrivate *d);
    QTextCursor(const QTextCursor &cursor);
    QTextCursor &operator=(const QTextCursor &other);
    ~QTextCursor();

    bool isNull() const;

    enum MoveMode {
        MoveAnchor,
        KeepAnchor
    };

    void setPosition(int pos, MoveMode mode = MoveAnchor);
    int position() const;

    int anchor() const;

    void insertText(const QString &text);
    void insertText(const QString &text, const QTextCharFormat &format);

    enum MoveOperation {
        NoMove,

        Start,
        Up,
        StartOfLine,
        StartOfBlock,
        StartOfWord,
        PreviousBlock,
        PreviousCharacter,
        PreviousWord,
        Left,
        WordLeft,

        End,
        Down,
        EndOfLine,
        EndOfWord,
        EndOfBlock,
        NextBlock,
        NextCharacter,
        NextWord,
        Right,
        WordRight
    };

    bool movePosition(MoveOperation op, MoveMode = MoveAnchor, int n = 1);

    bool visualNavigation() const;
    void setVisualNavigation(bool b);

    void deleteChar();
    void deletePreviousChar();

    enum SelectionType {
        WordUnderCursor,
        LineUnderCursor,
        BlockUnderCursor,
        Document
    };
    void select(SelectionType selection);

    bool hasSelection() const;
    bool hasComplexSelection() const;
    void removeSelectedText();
    void clearSelection();
    int selectionStart() const;
    int selectionEnd() const;

    QString selectedText() const;
    QTextDocumentFragment selection() const;
    void selectedTableCells(int *firstRow, int *numRows, int *firstColumn, int *numColumns) const;

    QTextBlock block() const;

    QTextCharFormat charFormat() const;
    void setCharFormat(const QTextCharFormat &format);
    void mergeCharFormat(const QTextCharFormat &modifier);

    QTextBlockFormat blockFormat() const;
    void setBlockFormat(const QTextBlockFormat &format);
    void mergeBlockFormat(const QTextBlockFormat &modifier);

    QTextCharFormat blockCharFormat() const;
    void setBlockCharFormat(const QTextCharFormat &format);
    void mergeBlockCharFormat(const QTextCharFormat &modifier);

    bool atBlockStart() const;
    bool atBlockEnd() const;
    bool atStart() const;
    bool atEnd() const;

    void insertBlock();
    void insertBlock(const QTextBlockFormat &format);
    void insertBlock(const QTextBlockFormat &format, const QTextCharFormat &charFormat);

    QTextList *insertList(const QTextListFormat &format);
    QTextList *insertList(QTextListFormat::Style style);

    QTextList *createList(const QTextListFormat &format);
    QTextList *createList(QTextListFormat::Style style);
    QTextList *currentList() const;

    QTextTable *insertTable(int rows, int cols, const QTextTableFormat &format);
    QTextTable *insertTable(int rows, int cols);
    QTextTable *currentTable() const;

    QTextFrame *insertFrame(const QTextFrameFormat &format);
    QTextFrame *currentFrame() const;

    void insertFragment(const QTextDocumentFragment &fragment);

#ifndef QT_NO_TEXTHTMLPARSER
    void insertHtml(const QString &html);
#endif // QT_NO_TEXTHTMLPARSER

    void insertImage(const QTextImageFormat &format, QTextFrameFormat::Position alignment);
    void insertImage(const QTextImageFormat &format);
    void insertImage(const QString &name);

    void beginEditBlock();
    void joinPreviousEditBlock();
    void endEditBlock();

    bool operator!=(const QTextCursor &rhs) const;
    bool operator<(const QTextCursor &rhs) const;
    bool operator<=(const QTextCursor &rhs) const;
    bool operator==(const QTextCursor &rhs) const;
    bool operator>=(const QTextCursor &rhs) const;
    bool operator>(const QTextCursor &rhs) const;

    bool isCopyOf(const QTextCursor &other) const;

    int blockNumber() const;
    int columnNumber() const;

private:
    QSharedDataPointer<QTextCursorPrivate> d;
    friend class QTextDocumentFragmentPrivate;
    friend class QTextCopyHelper;
};

QT_END_NAMESPACE

QT_END_HEADER

#endif // QTEXTCURSOR_H
