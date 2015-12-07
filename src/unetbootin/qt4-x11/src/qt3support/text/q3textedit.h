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

#ifndef Q3TEXTEDIT_H
#define Q3TEXTEDIT_H

#include <Qt3Support/q3scrollview.h>
#include <Qt3Support/q3stylesheet.h>
#include <Qt3Support/q3mimefactory.h>
#include <QtCore/qmap.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Qt3SupportLight)

#ifndef QT_NO_TEXTEDIT
// uncomment below to enable optimization mode - also uncomment the
// optimDoAutoScroll() private slot since moc ignores #ifdefs..
#define QT_TEXTEDIT_OPTIMIZATION

class QPainter;
class Q3TextDocument;
class Q3TextCursor;
class QKeyEvent;
class QResizeEvent;
class QMouseEvent;
class QTimer;
class Q3TextString;
class QTextCommand;
class Q3TextParagraph;
class Q3TextFormat;
class QFont;
class QColor;
class Q3TextEdit;
class QTextBrowser;
class Q3TextString;
struct QUndoRedoInfoPrivate;
class Q3PopupMenu;
class Q3TextEditPrivate;
class Q3SyntaxHighlighter;
class Q3TextDrag;

#ifdef QT_TEXTEDIT_OPTIMIZATION
class Q3TextEditOptimPrivate
{
public:
    // Note: no left-tag has any value for leftTag or parent, and
    // no right-tag has any formatting flags set.
    enum TagType { Color = 0, Format = 1 };
    struct Tag {
        TagType type:2;
        bool bold:1;
        bool italic:1;
        bool underline:1;
        int line;
        int index;
        Tag * leftTag; // ptr to left-tag in a left-right tag pair
        Tag * parent;  // ptr to parent left-tag in a nested tag
        Tag * prev;
        Tag * next;
        QString tag;
    };
    Q3TextEditOptimPrivate()
    {
        len = numLines = maxLineWidth = 0;
        selStart.line = selStart.index = -1;
        selEnd.line = selEnd.index = -1;
        search.line = search.index = 0;
        tags = lastTag = 0;
    }
    void clearTags()
    {
        Tag * itr = tags;
        while (tags) {
            itr  = tags;
            tags = tags->next;
            delete itr;
        }
        tags = lastTag = 0;
        tagIndex.clear();
    }
    ~Q3TextEditOptimPrivate()
    {
        clearTags();
    }
    int len;
    int numLines;
    int maxLineWidth;
    struct Selection {
        int line;
        int index;
    };
    Selection selStart, selEnd, search;
    Tag * tags, * lastTag;
    QMap<int, QString> lines;
    QMap<int, Tag *> tagIndex;
};
#endif

class Q_COMPAT_EXPORT Q3TextEdit : public Q3ScrollView
{
    friend class Q3TextBrowser;
    friend class Q3SyntaxHighlighter;

    Q_OBJECT
    Q_ENUMS(WordWrap WrapPolicy)
    Q_FLAGS(AutoFormattingFlag)
    Q_PROPERTY(Qt::TextFormat textFormat READ textFormat WRITE setTextFormat)
    Q_PROPERTY(QString text READ text WRITE setText)
    Q_PROPERTY(QBrush paper READ paper WRITE setPaper)
    Q_PROPERTY(bool linkUnderline READ linkUnderline WRITE setLinkUnderline)
    Q_PROPERTY(QString documentTitle READ documentTitle)
    Q_PROPERTY(int length READ length)
    Q_PROPERTY(WordWrap wordWrap READ wordWrap WRITE setWordWrap)
    Q_PROPERTY(int wrapColumnOrWidth READ wrapColumnOrWidth WRITE setWrapColumnOrWidth)
    Q_PROPERTY(WrapPolicy wrapPolicy READ wrapPolicy WRITE setWrapPolicy)
    Q_PROPERTY(bool hasSelectedText READ hasSelectedText)
    Q_PROPERTY(QString selectedText READ selectedText)
    Q_PROPERTY(int undoDepth READ undoDepth WRITE setUndoDepth)
    Q_PROPERTY(bool overwriteMode READ isOverwriteMode WRITE setOverwriteMode)
    Q_PROPERTY(bool modified READ isModified WRITE setModified DESIGNABLE false)
    Q_PROPERTY(bool readOnly READ isReadOnly WRITE setReadOnly)
    Q_PROPERTY(bool undoRedoEnabled READ isUndoRedoEnabled WRITE setUndoRedoEnabled)
    Q_PROPERTY(int tabStopWidth READ tabStopWidth WRITE setTabStopWidth)
    Q_PROPERTY(bool tabChangesFocus READ tabChangesFocus WRITE setTabChangesFocus)
    Q_PROPERTY(AutoFormattingFlag autoFormatting READ autoFormatting WRITE setAutoFormatting)

public:
    enum WordWrap {
        NoWrap,
        WidgetWidth,
        FixedPixelWidth,
        FixedColumnWidth
    };

    enum WrapPolicy {
        AtWordBoundary,
        AtWhiteSpace = AtWordBoundary, // AtWhiteSpace is deprecated
        Anywhere,
        AtWordOrDocumentBoundary
    };

    enum AutoFormattingFlag {
        AutoNone = 0,
        AutoBulletList = 0x00000001,
        AutoAll = 0xffffffff
    };

    Q_DECLARE_FLAGS(AutoFormatting, AutoFormattingFlag)

    enum KeyboardAction {
        ActionBackspace,
        ActionDelete,
        ActionReturn,
        ActionKill,
        ActionWordBackspace,
        ActionWordDelete
    };

    enum CursorAction {
        MoveBackward,
        MoveForward,
        MoveWordBackward,
        MoveWordForward,
        MoveUp,
        MoveDown,
        MoveLineStart,
        MoveLineEnd,
        MoveHome,
        MoveEnd,
        MovePgUp,
        MovePgDown
    };

    enum VerticalAlignment {
        AlignNormal,
        AlignSuperScript,
        AlignSubScript
    };

    enum TextInsertionFlags {
        RedoIndentation = 0x0001,
        CheckNewLines = 0x0002,
        RemoveSelected = 0x0004
    };

    Q3TextEdit(const QString& text, const QString& context = QString(),
               QWidget* parent=0, const char* name=0);
    Q3TextEdit(QWidget* parent=0, const char* name=0);
    virtual ~Q3TextEdit();

    QString text() const;
    QString text(int para) const;
    Qt::TextFormat textFormat() const;
    QString context() const;
    QString documentTitle() const;

    void getSelection(int *paraFrom, int *indexFrom,
                    int *paraTo, int *indexTo, int selNum = 0) const;
    virtual bool find(const QString &expr, bool cs, bool wo, bool forward = true,
                       int *para = 0, int *index = 0);

    int paragraphs() const;
    int lines() const;
    int linesOfParagraph(int para) const;
    int lineOfChar(int para, int chr);
    int length() const;
    QRect paragraphRect(int para) const;
    int paragraphAt(const QPoint &pos) const;
    int charAt(const QPoint &pos, int *para) const;
    int paragraphLength(int para) const;

    Q3StyleSheet* styleSheet() const;
#ifndef QT_NO_MIME
    Q3MimeSourceFactory* mimeSourceFactory() const;
#endif
    QBrush paper() const;
    bool linkUnderline() const;

    int heightForWidth(int w) const;

    bool hasSelectedText() const;
    QString selectedText() const;
    bool isUndoAvailable() const;
    bool isRedoAvailable() const;

    WordWrap wordWrap() const;
    int wrapColumnOrWidth() const;
    WrapPolicy wrapPolicy() const;

    int tabStopWidth() const;

    QString anchorAt(const QPoint& pos, Qt::AnchorAttribute a = Qt::AnchorHref);

    QSize sizeHint() const;

    bool isReadOnly() const { return readonly; }

    void getCursorPosition(int *parag, int *index) const;

    bool isModified() const;
    bool italic() const;
    bool bold() const;
    bool underline() const;
    QString family() const;
    int pointSize() const;
    QColor color() const;
    QFont font() const;
    QFont currentFont() const;
    int alignment() const;
    VerticalAlignment verticalAlignment() const;
    int undoDepth() const;

    // do not use, will go away
    virtual bool getFormat(int para, int index, QFont *font, QColor *color, VerticalAlignment *verticalAlignment);
    // do not use, will go away
    virtual bool getParagraphFormat(int para, QFont *font, QColor *color,
                                     VerticalAlignment *verticalAlignment, int *alignment,
                                     Q3StyleSheetItem::DisplayMode *displayMode,
                                     Q3StyleSheetItem::ListStyle *listStyle,
                                     int *listDepth);


    bool isOverwriteMode() const { return overWrite; }
    QColor paragraphBackgroundColor(int para) const;

    bool isUndoRedoEnabled() const;
    bool eventFilter(QObject *o, QEvent *e);
    bool tabChangesFocus() const;

    void setAutoFormatting(AutoFormatting);
    AutoFormatting autoFormatting() const;
    Q3SyntaxHighlighter *syntaxHighlighter() const;

public Q_SLOTS:
#ifndef QT_NO_MIME
    virtual void setMimeSourceFactory(Q3MimeSourceFactory* factory);
#endif
    virtual void setStyleSheet(Q3StyleSheet* styleSheet);
    virtual void scrollToAnchor(const QString& name);
    virtual void setPaper(const QBrush& pap);
    virtual void setLinkUnderline(bool);

    virtual void setWordWrap(Q3TextEdit::WordWrap mode);
    virtual void setWrapColumnOrWidth(int);
    virtual void setWrapPolicy(Q3TextEdit::WrapPolicy policy);

    virtual void copy();
    virtual void append(const QString& text);

    void setText(const QString &txt) { setText(txt, QString()); }
    virtual void setText(const QString &txt, const QString &context);
    virtual void setTextFormat(Qt::TextFormat f);

    virtual void selectAll(bool select = true);
    virtual void setTabStopWidth(int ts);
    virtual void zoomIn(int range);
    virtual void zoomIn() { zoomIn(1); }
    virtual void zoomOut(int range);
    virtual void zoomOut() { zoomOut(1); }
    virtual void zoomTo(int size);

    virtual void sync();
    virtual void setReadOnly(bool b);

    virtual void undo();
    virtual void redo();
    virtual void cut();
    virtual void paste();
#ifndef QT_NO_CLIPBOARD
    virtual void pasteSubType(const QByteArray &subtype);
#endif
    virtual void clear();
    virtual void del();
    virtual void indent();
    virtual void setItalic(bool b);
    virtual void setBold(bool b);
    virtual void setUnderline(bool b);
    virtual void setFamily(const QString &f);
    virtual void setPointSize(int s);
    virtual void setColor(const QColor &c);
    virtual void setVerticalAlignment(Q3TextEdit::VerticalAlignment a);
    virtual void setAlignment(int a);

    // do not use, will go away
    virtual void setParagType(Q3StyleSheetItem::DisplayMode dm, Q3StyleSheetItem::ListStyle listStyle);

    virtual void setCursorPosition(int parag, int index);
    virtual void setSelection(int parag_from, int index_from, int parag_to, int index_to, int selNum = 0);
    virtual void setSelectionAttributes(int selNum, const QColor &back, bool invertText);
    virtual void setModified(bool m);
    virtual void resetFormat();
    virtual void setUndoDepth(int d);
    virtual void setFormat(Q3TextFormat *f, int flags);
    virtual void ensureCursorVisible();
    virtual void placeCursor(const QPoint &pos, Q3TextCursor *c = 0);
    virtual void moveCursor(Q3TextEdit::CursorAction action, bool select);
    virtual void doKeyboardAction(Q3TextEdit::KeyboardAction action);
    virtual void removeSelectedText(int selNum = 0);
    virtual void removeSelection(int selNum = 0);
    virtual void setCurrentFont(const QFont &f);
    virtual void setOverwriteMode(bool b) { overWrite = b; }

    virtual void scrollToBottom();

    virtual void insert(const QString &text, uint insertionFlags = CheckNewLines | RemoveSelected);

    // obsolete
    virtual void insert(const QString &text, bool, bool = true, bool = true);

    virtual void insertAt(const QString &text, int para, int index);
    virtual void removeParagraph(int para);
    virtual void insertParagraph(const QString &text, int para);

    virtual void setParagraphBackgroundColor(int para, const QColor &bg);
    virtual void clearParagraphBackground(int para);

    virtual void setUndoRedoEnabled(bool b);
    virtual void setTabChangesFocus(bool b);

#ifdef QT_TEXTEDIT_OPTIMIZATION
    void polishEvent(QEvent*);
    void setMaxLogLines(int numLines);
    int maxLogLines() const;
#endif

Q_SIGNALS:
    void textChanged();
    void selectionChanged();
    void copyAvailable(bool);
    void undoAvailable(bool yes);
    void redoAvailable(bool yes);
    void currentFontChanged(const QFont &f);
    void currentColorChanged(const QColor &c);
    void currentAlignmentChanged(int a);
    void currentVerticalAlignmentChanged(Q3TextEdit::VerticalAlignment a);
    void cursorPositionChanged(Q3TextCursor *c);
    void cursorPositionChanged(int para, int pos);
    void returnPressed();
    void modificationChanged(bool m);
    void clicked(int parag, int index);
    void doubleClicked(int parag, int index);

protected:
    void repaintChanged();
    void updateStyles();
    void drawContents(QPainter *p, int cx, int cy, int cw, int ch);
    bool event(QEvent *e);
    void changeEvent(QEvent *);
    void keyPressEvent(QKeyEvent *e);
    void resizeEvent(QResizeEvent *e);
    void viewportResizeEvent(QResizeEvent*);
    void contentsMousePressEvent(QMouseEvent *e);
    void contentsMouseMoveEvent(QMouseEvent *e);
    void contentsMouseReleaseEvent(QMouseEvent *e);
    void contentsMouseDoubleClickEvent(QMouseEvent *e);
#ifndef QT_NO_WHEELEVENT
    void contentsWheelEvent(QWheelEvent *e);
#endif
    void inputMethodEvent(QInputMethodEvent *);
#ifndef QT_NO_DRAGANDDROP
    void contentsDragEnterEvent(QDragEnterEvent *e);
    void contentsDragMoveEvent(QDragMoveEvent *e);
    void contentsDragLeaveEvent(QDragLeaveEvent *e);
    void contentsDropEvent(QDropEvent *e);
#endif
    void contentsContextMenuEvent(QContextMenuEvent *e);
    bool focusNextPrevChild(bool next);
    Q3TextDocument *document() const;
    Q3TextCursor *textCursor() const;
    void setDocument(Q3TextDocument *doc);
    virtual Q3PopupMenu *createPopupMenu(const QPoint& pos);
    virtual Q3PopupMenu *createPopupMenu();
    void drawCursor(bool visible);

protected Q_SLOTS:
    virtual void doChangeInterval();
    virtual void sliderReleased();

private Q_SLOTS:
    void formatMore();
    void doResize();
    void autoScrollTimerDone();
    void blinkCursor();
    void setModified();
    void startDrag();
    void documentWidthChanged(int w);
    void clipboardChanged();

private:
    struct Q_COMPAT_EXPORT UndoRedoInfo {
        enum Type { Invalid, Insert, Delete, Backspace, Return, RemoveSelected, Format, Style, IME };

        UndoRedoInfo(Q3TextDocument *dc);
        ~UndoRedoInfo();
        void clear();
        bool valid() const;

        QUndoRedoInfoPrivate *d;
        int id;
        int index;
        int eid;
        int eindex;
        Q3TextFormat *format;
        int flags;
        Type type;
        Q3TextDocument *doc;
        QByteArray styleInformation;
    };

private:
    void updateCursor(const QPoint & pos);
    void handleMouseMove(const QPoint& pos);
    void drawContents(QPainter *);
    virtual bool linksEnabled() const { return false; }
    void init();
    void checkUndoRedoInfo(UndoRedoInfo::Type t);
    void updateCurrentFormat();
    bool handleReadOnlyKeyEvent(QKeyEvent *e);
    void makeParagVisible(Q3TextParagraph *p);
    void normalCopy();
    void copyToClipboard();
#ifndef QT_NO_MIME
    QByteArray pickSpecial(QMimeSource* ms, bool always_ask, const QPoint&);
    Q3TextDrag *dragObject(QWidget *parent = 0) const;
#endif
#ifndef QT_NO_MIMECLIPBOARD
    void pasteSpecial(const QPoint&);
#endif
    void setFontInternal(const QFont &f);

    virtual void emitHighlighted(const QString &) {}
    virtual void emitLinkClicked(const QString &) {}

    void readFormats(Q3TextCursor &c1, Q3TextCursor &c2, Q3TextString &text, bool fillStyles = false);
    void clearUndoRedo();
    void paintDocument(bool drawAll, QPainter *p, int cx = -1, int cy = -1, int cw = -1, int ch = -1);
    void moveCursor(CursorAction action);
    void ensureFormatted(Q3TextParagraph *p);
    void placeCursor(const QPoint &pos, Q3TextCursor *c, bool link);
    QVariant inputMethodQuery(Qt::InputMethodQuery query) const;

#ifdef QT_TEXTEDIT_OPTIMIZATION
    bool checkOptimMode();
    QString optimText() const;
    void optimSetText(const QString &str);
    void optimAppend(const QString &str);
    void optimInsert(const QString &str, int line, int index);
    void optimDrawContents(QPainter * p, int cx, int cy, int cw, int ch);
    void optimMousePressEvent(QMouseEvent * e);
    void optimMouseReleaseEvent(QMouseEvent * e);
    void optimMouseMoveEvent(QMouseEvent * e);
    int  optimCharIndex(const QString &str, int mx) const;
    void optimSelectAll();
    void optimRemoveSelection();
    void optimSetSelection(int startLine, int startIdx, int endLine,
                            int endIdx);
    bool optimHasSelection() const;
    QString optimSelectedText() const;
    bool optimFind(const QString & str, bool, bool, bool, int *, int *);
    void optimParseTags(QString * str, int lineNo = -1, int indexOffset = 0);
    Q3TextEditOptimPrivate::Tag * optimPreviousLeftTag(int line);
    void optimSetTextFormat(Q3TextDocument *, Q3TextCursor *, Q3TextFormat * f,
                             int, int, Q3TextEditOptimPrivate::Tag * t);
    Q3TextEditOptimPrivate::Tag * optimAppendTag(int index, const QString & tag);
    Q3TextEditOptimPrivate::Tag * optimInsertTag(int line, int index, const QString & tag);
    void optimCheckLimit(const QString& str);
    bool optimHasBoldMetrics(int line);

private Q_SLOTS:
    void optimDoAutoScroll();
#endif // QT_TEXTEDIT_OPTIMIZATION

private:
#ifndef QT_NO_CLIPBOARD
    void pasteSubType(const QByteArray &subtype, QMimeSource *m);
#endif

private:
    Q_DISABLE_COPY(Q3TextEdit)

    Q3TextDocument *doc;
    Q3TextCursor *cursor;
    QTimer *formatTimer, *scrollTimer, *changeIntervalTimer, *blinkTimer, *dragStartTimer;
    Q3TextParagraph *lastFormatted;
    int interval;
    UndoRedoInfo undoRedoInfo;
    Q3TextFormat *currentFormat;
    int currentAlignment;
    QPoint oldMousePos, mousePos;
    QPoint dragStartPos;
    QString onLink;
    WordWrap wrapMode;
    WrapPolicy wPolicy;
    int wrapWidth;
    QString pressedLink;
    Q3TextEditPrivate *d;
    bool inDoubleClick : 1;
    bool mousePressed : 1;
    bool cursorVisible : 1;
    bool blinkCursorVisible : 1;
    bool modified : 1;
    bool mightStartDrag : 1;
    bool inDnD : 1;
    bool readonly : 1;
    bool undoEnabled : 1;
    bool overWrite : 1;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(Q3TextEdit::AutoFormatting)

inline Q3TextDocument *Q3TextEdit::document() const
{
    return doc;
}

inline Q3TextCursor *Q3TextEdit::textCursor() const
{
    return cursor;
}

inline void Q3TextEdit::setCurrentFont(const QFont &f)
{
    Q3TextEdit::setFontInternal(f);
}

#endif // QT_NO_TEXTEDIT

QT_END_NAMESPACE

QT_END_HEADER

#endif // Q3TEXTEDIT_H
